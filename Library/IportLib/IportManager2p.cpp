#include "StdAfx.h"
#include "IportManager2p.h"

#include <io.h>
#include <fcntl.h>

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

using namespace IportLib;


////===================================================================
//// �}�N��
////===================================================================
//#define LOM(t) mcls_LogMe.Write(KizuLib::LogFileManager::t			// ���[�J����p���O



//------------------------------------------
// �R���X�g���N�^
// CString sMyManagerName ���N���X�̖���
// CString sMyIport iPort�N���X�̖���
//------------------------------------------
IportManager2p::IportManager2p(CString sMyManagerName, CString sMyIport) :
// 1.���O
IportManager(sMyManagerName),

my_nChDiffMax(0)
{
	for(int ii=0; ii<MAX_IPORT_CAMNUM; ii++) {
		mycls_pIport[ii]	= NULL;
		my_nSeqNumSlave[ii]	= 0;
		my_nChDiff[ii][0]	= my_nChDiff[ii][1] = 0;
	}
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
IportManager2p::~IportManager2p(void)
{
	// �O�̂��߂ɉ��
	Dispose();
}

//------------------------------------------
// ������ �� �����ŉ摜���L���[�C���O�������ꍇ�Ɏg������
// DWORD nFrameW �t���[���T�C�Y (2���𑫂����킹���T�C�Y)
// DWORD nFrameH �t���[���T�C�Y
// DWORD camset ��������
// DWORD angle �p�x��
// BYTE nDummyVal �_�~�[�摜�̐F
//------------------------------------------
bool IportManager2p::Init(DWORD nFrameW, DWORD nFrameH, DWORD camset, DWORD angle, BYTE nDummyVal)
{
	//// �����o�[�ϐ��ɓo�^
	my_nImgSize	  = nFrameW*nFrameH;	// �g���@����������`���Ă���
	my_nFrameW	  = nFrameW;
	my_nFrameH	  = nFrameH;
	my_nCamsetNum = camset;
	my_nAngleNum  = angle;


	//// ���낢��`�F�b�N	
	int camnum			= camset * angle;
	int nMaxQueSize		= myque_pFrame->GetSemCount();

	if(1 > my_nImgSize) {
		LOG(em_ERR), "[%s] �t���[���T�C�Y�ُ� (w=%d, h=%d)", my_sThreadName, nFrameW, nFrameH);
		_ASSERT(false);
		return false;
	}
	if(1 > nMaxQueSize) {
		LOG(em_ERR), "[%s] �L���[�T�C�Y�ُ�=%d", my_sThreadName, nMaxQueSize);
		_ASSERT(false);
		return false;
	}
	_ASSERT(camset		   < MAX_IPORT_CAMSET );
	_ASSERT(angle		   < MAX_IPORT_CAMANGLE );
	_ASSERT(camset * angle < MAX_IPORT_CAMNUM );

/*  �^�X�N�̕��Ő�������悤�ɕύX
	//// ��]���i�`���̃L���[���m��
	int  nAryWk[5];
	nAryWk[0] = my_nImgSize;
	nAryWk[1] = my_nFrameW;
	nAryWk[2] = my_nFrameH;
	nAryWk[3] = my_nCamsetNum;
	nAryWk[4] = my_nAngleNum;	//camnum;
	myque_pFrame->QueueAlloc(nMaxQueSize, nAryWk); 
*/

	//// �_�~�[�t���[���̐��� (�g���܂킵���ł���l�ɁA�A����̃T�C�Y���m�ۂ��Ă���)
	if (NULL != my_pDummyBuf) _aligned_free(my_pDummyBuf);		// �O�̂���
	DWORD image_size = my_nFrameW * my_nFrameH * my_nChannel;
	my_pDummyBuf = _aligned_malloc(image_size, 32);
	_ASSERT(my_pDummyBuf);
	memset(my_pDummyBuf, 0x80, image_size);

	my_bInit = true;
	return true;
}

//------------------------------------------
// ���
//------------------------------------------
void IportManager2p::Dispose()
{
	//// iPort�Ɛؒf (�O�̂���)
	DisConnect();
	CloseBmp();
	CloseAvi();

/* �^�X�N�̕��ŉ������悤�ɕύX
	//// ��]���i�`���̃L���[�����
	myque_pFrame->QueueFree();
*/

	//// �_�~�[�摜�̈���
	if (NULL != my_pDummyBuf) {
		_aligned_free(my_pDummyBuf);
		my_pDummyBuf = NULL;
	}

	my_bInit = false;
}

//------------------------------------------
// �X���b�h�J�n �� ���̃X���b�h�́A�z���g�ɃX���b�h�����s�����܂Ń��b�N�����
//------------------------------------------
bool IportManager2p::Start()
{
	// �O����
	my_bThreadLoop	= true;
	my_hThreadReady	= CreateEvent(NULL, FALSE, FALSE, NULL);
	ResetEvent(my_hThreadGoStop);				// ��~��Ԃɂ��Ă���

	// ��{���� �N���e�B�J���Ŏ��s
	ThreadManager::Start(THREAD_PRIORITY_TIME_CRITICAL);
	
	// ���̃X���b�h�͂����ƃX���b�h���N������܂őҋ@
    const unsigned long timeout = 1000;
    DWORD wait = WaitForSingleObject(my_hThreadReady, timeout);
	bool ret = WAIT_OBJECT_0 == wait ? true : false;

	// ��n��
	SAFE_DELETE_HANDLE(my_hThreadReady);
	return ret;
}

//------------------------------------------
// �X���b�h��~
//------------------------------------------
bool IportManager2p::Stop()
{
	// ��~�t���O�Z�b�g
	my_bThreadLoop = false;						// �������������ꍇ�A�r�����f�p
	WorkThread::StopSignal();					// �ʏ�̒�~�V�O�i���𔭍s
	SetEvent(my_hThreadGoStop);					// ��~���Ă���X���b�h�𓮂�������


	// LockFrame()�Ńu���b�N���̏ꍇ�A�L�����Z��
	for(int ii=0; ii<MAX_IPORT_CAMNUM; ii++) {
		if( NULL == mycls_pIport[ii] ) continue;

		mycls_pIport[ii]->AbortLock();			// LockFrame()����o��
	}

	// ��{����
	ThreadManager::Stop(5000);
	ResetEvent(my_hThreadGoStop);				// ���̒�~��Ԃɖ߂�
	return true;
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �B���n

//------------------------------------------
// �B���J�n
//------------------------------------------
int IportManager2p::StartGrab()
{
	DWORD ii, jj, ch;

	LOG(em_MSG), "[%s] StartGrab���", my_sThreadName);	
	LOM(em_MSG), "[%s] StartGrab���", my_sThreadName);


	//==============================================
	// iPort
	if( IportManager::GRAB_MODE_IPORT == my_emGrabMode ) {
		//// �����`�F�b�N
		for(ii=0; ii<my_nCamsetNum; ii++) {
			for(jj=0; jj<my_nAngleNum; jj++) {
				ch = GetCamIndex(ii, jj);

				_ASSERT(NULL != mycls_pIport[ch]);
				_ASSERT(mycls_pIport[ch]->GetFrameNChannel() == my_nChannel);
			}
		}

		//// StartGrab
		for(ii=0; ii<my_nCamsetNum; ii++) {
			for(jj=0; jj<my_nAngleNum; jj++) {
				ch = GetCamIndex(ii, jj);

				mycls_pIport[ch]->ClearQueueItemInBuffer();
				ERRCODE ret = mycls_pIport[ch]->StartGrab();
				if( NoErr == ret ) {
					LOG(em_MSG), "[%s] [%s] OK", my_sThreadName, mycls_pIport[ch]->GetThreadName());	
					LOM(em_MSG), "[%s] [%s] OK", my_sThreadName, mycls_pIport[ch]->GetThreadName());	
				} else {
					LOG(em_ERR), "[%s] [%s] Failed(ret=%d)", my_sThreadName, mycls_pIport[ch]->GetThreadName(), ret);	
					LOM(em_ERR), "[%s] [%s] Failed(ret=%d)", my_sThreadName, mycls_pIport[ch]->GetThreadName(), ret);	
					return ret;
				}
			}
		}
	}

	//==============================================
	// BitMap or AVI
	else if( IportManager::GRAB_MODE_BMP == my_emGrabMode || 
			 IportManager::GRAB_MODE_AVI == my_emGrabMode ) {

		my_nSmpNowNo = my_nSmpIntervalNo[0];
		__int64 ts = my_nSmpCycleTimer * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
		SetWaitableTimer(my_evSmpImgTimer, (LARGE_INTEGER *)&ts, my_nSmpCycleTimer, NULL, NULL, FALSE);
	}



	//// ������돉����
	for(ii=0; ii<MAX_IPORT_CAMNUM; ii++) {
		my_nSeqNum[ii] = 0; my_nSeqNumSlave[ii] = 0;
		my_nChDiff[ii][0]  = my_nChDiff[ii][1] = 0;
		my_nSkipCnt[ii] = 0;
	}
	my_nFrameNum		= 0;
	my_nQueueCount		= 0;
	my_nMaxQueueCount	= 0;
	//my_nResendTotal		= 0;
	//my_nLostTotal		= 0;
	//my_nChDiffMax		= 0;

	my_bGrabRun			= true;
	LOG(em_MSG), "[%s] StartGrab ����", my_sThreadName);
	LOM(em_MSG), "[%s] StartGrab ����", my_sThreadName);	

	//// iPort���݂̂̏���
	if( IportManager::GRAB_MODE_IPORT == my_emGrabMode ) {
		SetEvent(my_hThreadGoStop);			// ���C���X���b�h�����s���
	}
	return NoErr;
}

//------------------------------------------
// �B����~
//------------------------------------------
int IportManager2p::StopGrab()
{
	DWORD ii, jj, ch;
	

	LOG(em_MSG), "[%s] StopGrab���", my_sThreadName);	
	LOM(em_MSG), "[%s] StopGrab���", my_sThreadName);
	
	ERRCODE ret = NoErr;

	//==============================================
	// iPort
	if( IportManager::GRAB_MODE_IPORT == my_emGrabMode ) {
		ResetEvent(my_hThreadGoStop);		// ���C���X���b�h�����s���~

		//// StopGrab
		for(ii=0; ii<my_nCamsetNum; ii++) {
			for(jj=0; jj<my_nAngleNum; jj++) {
				ch = GetCamIndex(ii, jj);
		
				if( NULL != mycls_pIport[ch] ) {

					LOG(em_MSG), "[%s] [%s] StopGrab���s", my_sThreadName, mycls_pIport[ch]->GetThreadName());
					LOM(em_MSG), "[%s] [%s] StopGrab���s", my_sThreadName, mycls_pIport[ch]->GetThreadName());
					ERRCODE ret2 = mycls_pIport[ch]->StopGrab();

					if (ret2 != NoErr) ret = ret2;

					LOG(em_MSG), "[%s] [%s] AbortLock���s", my_sThreadName, mycls_pIport[ch]->GetThreadName());
					LOM(em_MSG), "[%s] [%s] AbortLock���s", my_sThreadName, mycls_pIport[ch]->GetThreadName());
					mycls_pIport[ch]->AbortLock();
				}
			}
		}
	}
	
	//==============================================
	// BitMap or AVI
	else if( IportManager::GRAB_MODE_BMP == my_emGrabMode || 
			 IportManager::GRAB_MODE_AVI == my_emGrabMode ) {

		CancelWaitableTimer(my_evSmpImgTimer);
	}

	my_bGrabRun			= false;
	LOG(em_MSG), "[%s] StopGrab ����", my_sThreadName);
	LOM(em_MSG), "[%s] StopGrab ����", my_sThreadName);
	return ret;
}

//------------------------------------------
// ���Z�b�g
//------------------------------------------
bool IportManager2p::Reset()
{
	DWORD ii, jj, ch;

	//==============================================
	// iPort
	if( IportManager::GRAB_MODE_IPORT == my_emGrabMode ) {
		for(ii=0; ii<my_nCamsetNum; ii++) {
			for(jj=0; jj<my_nAngleNum; jj++) {
				ch = GetCamIndex(ii, jj);

				// iPort�̃o�b�t�@�N���A
				mycls_pIport[ch]->ClearQueueItemInBuffer();

				// �}�X�^�[�̏ꍇ�A���Z�b�g���W���[�����s
				if( my_reset_module ) {
					mycls_pIport[ch]->ResetModule();
				}

				// ���̑������o�[������
				my_nChDiff[ch][0] = my_nChDiff[ch][1] = 0;
			}
		}
	}
	
	//==============================================
	// BitMap or AVI
	else if( IportManager::GRAB_MODE_BMP == my_emGrabMode || 
			 IportManager::GRAB_MODE_AVI == my_emGrabMode ) {

		 my_nSmpNowNo = my_nSmpIntervalNo[0];
	}
	
	
	//// ���̑������o�[������
	for(ii=0; ii<MAX_IPORT_CAMNUM; ii++) { 
		my_nSeqNum[ii] = 0; my_nSeqNumSlave[ii] = 0;
	}
	my_nFrameNum		= 0;
	my_nQueueCount		= 0;
	my_nMaxQueueCount	= 0;
	my_nResendTotal		= 0;
	my_nLostTotal		= 0;
	my_nChDiffMax		= 0;

	return true;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// iPort ����n

//------------------------------------------
// iPort�ɐڑ�
// DWORD set �J�����Z�b�g (0�I���W��)
// DWORD ang �J�����p�x (0�I���W��)
// CString sXmlPath XML�t���p�X
// bool bSaveIpAddr �J�����{�̂�IP�A�h���X���L�������邩�H (true:XML�ɏ�����Ă���IP��ݒ�) �� ���Ԃ�false���́AIP�A�h���X�ŃR�l�N�g�Btrue���́AMAC�A�h���X�ŃR�l�N�g����Ǝv����
//						(�J�����d��OFF��ON�ŁAIP��Y��Ă��܂�����)
// bool bIsSlave SLAVE���[�h�Őڑ� (true:XML�Ń}�X�^�[�w�莞�ł��A�����I��SLAVE�ɂȂ�Bfalse:XML�̎w��ʂ�)
// long nAdapterIndex iPort�p�f�o�C�X�}�l�[�W���[�̉����ڂ��g�p���邩 (0�I���W��) (-1:XML�ɋL�q����Ă���A�_�v�^�[���g�p)
// int nGrabQueSize �O���o�[�̃����O�o�b�t�@�T�C�Y
// IportLib::ERRCODE ���A���
//------------------------------------------
IportLib::ERRCODE IportManager2p::Connect(DWORD set, DWORD ang, CString sXmlPath, long nAdapterIndex1,
										  const char* mac_addr2, const char* ip_addr2, long nAdapterIndex2, char const* mc_addr2,
										  bool bSaveIpAddr, bool bIsSlave, int nGrabQueSize)
{
/***
	IportLib::ERRCODE	ret;
	int ch = GetCamIndex(set, ang);

	_ASSERT( NULL == mycls_pIport[ch]);

	//// �J�����̏���
	CString sWk;
	sWk.Format("iPortBase_%d", ch+1); 
	mycls_pIport[ch] = new IportBase2p(sWk);
	mycls_pIport[ch]->SetLogMgr(mcls_pLog);
	if( ! mycls_pIport[ch]->Init() ) {
		LOG(em_ERR), "[%s] Init %s ���s", my_sThreadName, mycls_pIport[ch]->GetThreadName());
		LOM(em_ERR), "[%s] Init %s ���s", my_sThreadName, mycls_pIport[ch]->GetThreadName());
		DisConnect();
		return ApErr;
	}


	//// iPort�ڑ�
	ret = mycls_pIport[ch]->SetupFromXMLFile(sXmlPath, nAdapterIndex1, mac_addr2, ip_addr2, nAdapterIndex2, mc_addr2, bSaveIpAddr, bIsSlave, nGrabQueSize);
	if( NoErr == ret ) {
		
		// �� ��������pitch8�̂�
		if( 0 != mycls_pIport[ch]->GetFrameWidth() % (mycls_pIport[ch]->GetFramePixelNBit()/my_nChannel) ) {
			LOG(em_ERR), "[%s] %s pitch�T�C�Y�s���� (%d)", my_sThreadName, mycls_pIport[ch]->GetBase(0).GetDeviceIP(), mycls_pIport[ch]->GetFramePixelNBit());	
			LOM(em_ERR), "[%s] %s pitch�T�C�Y�s���� (%d)", my_sThreadName, mycls_pIport[ch]->GetBase(0).GetDeviceIP(), mycls_pIport[ch]->GetFramePixelNBit());	
			return ApErr;
		}

		// �Œ����XML�������`�F�b�N
		if( my_nFrameW != mycls_pIport[ch]->GetFrameWidth() ||
			my_nFrameH != mycls_pIport[ch]->GetFrameHeight() ) {
			LOG(em_ERR), "[%s] %s XML�T�C�Y�s���� (w=%d/%d,h=%d/%d)", my_sThreadName, mycls_pIport[ch]->GetBase(0).GetDeviceIP(), my_nFrameW, mycls_pIport[ch]->GetFrameWidth(), my_nFrameH, mycls_pIport[ch]->GetFrameHeight());	
			LOM(em_ERR), "[%s] %s XML�T�C�Y�s���� (w=%d/%d,h=%d/%d)", my_sThreadName, mycls_pIport[ch]->GetBase(0).GetDeviceIP(), my_nFrameW, mycls_pIport[ch]->GetFrameWidth(), my_nFrameH, mycls_pIport[ch]->GetFrameHeight());	
			return ApErr;
		} else {

			LOG(em_INF), "[%s] %s �ڑ�����(w=%d,h=%d)[%s]", my_sThreadName, mycls_pIport[ch]->GetBase(0).GetDeviceIP(), my_nFrameW, my_nFrameH, sXmlPath);	
			LOM(em_INF), "[%s] %s �ڑ�����(w=%d,h=%d)[%s]", my_sThreadName, mycls_pIport[ch]->GetBase(0).GetDeviceIP(), my_nFrameW, my_nFrameH, sXmlPath);	
		}
	} else {
		LOG(em_ERR), "[%s] SetupFromXMLFile���s=%d [%s]", my_sThreadName, ret, sXmlPath);
		LOM(em_ERR), "[%s] SetupFromXMLFile���s=%d [%s]", my_sThreadName, ret, sXmlPath);
		return ret;
	}

	
	//// ���̑��K�v�ȃf�[�^
	my_emGrabMode = IportManager::GRAB_MODE_IPORT;
	return NoErr;
***/

	CyResult ret = CY_RESULT_OK;
	CString		ip;
	CString		mac;
	CString		mc;

	// IP�擾
	ret = IportBase::GetXMLFileToIP(sXmlPath, &ip, &mac, &mc);
	if ( CY_RESULT_OK != ret) RETURN_CY_ERR(ret);

	// �ڑ�
	//ret = Connect(set, ang, sXmlPath, ip, mac, nAdapterIndex1, mc, mac_addr2, ip_addr2, nAdapterIndex2, mc_addr2, bSaveIpAddr, bIsSlave, nGrabQueSize);
	ret = Connect(set, ang, sXmlPath, 
		mac, ip, nAdapterIndex1, mc, 
		mac_addr2, ip_addr2, nAdapterIndex2, mc_addr2,
		bSaveIpAddr, bIsSlave, nGrabQueSize);
	RETURN_CY_ERR(ret);
}
IportLib::ERRCODE IportManager2p::Connect(DWORD set, DWORD ang, CString sXmlPath,
									      const char* mac_addr1, const char* ip_addr1, long nAdapterIndex1, char const* mc_addr1,
										  const char* mac_addr2, const char* ip_addr2, long nAdapterIndex2, char const* mc_addr2,
										  bool bSaveIpAddr, bool bIsSlave, int nGrabQueSize)
{

	IportLib::ERRCODE	ret;
	int ch = GetCamIndex(set, ang);

	_ASSERT( NULL == mycls_pIport[ch]);

	//// �J�����̏���
	CString sWk;
	sWk.Format("iPortBase_%d", ch+1); 
	mycls_pIport[ch] = new IportBase2p(sWk);
	mycls_pIport[ch]->SetLogMgr(mcls_pLog);
	if( ! mycls_pIport[ch]->Init() ) {
		LOG(em_ERR), "[%s] Init %s ���s", my_sThreadName, mycls_pIport[ch]->GetThreadName());
		LOM(em_ERR), "[%s] Init %s ���s", my_sThreadName, mycls_pIport[ch]->GetThreadName());
		DisConnect();
		return ApErr;
	}


	//// iPort�ڑ�

//	ret = mycls_pIport[ch]->SetupFromXMLFile(sXmlPath, mac_addr1, ip_addr2, nAdapterIndex1, mc_addr1, mac_addr2, ip_addr2, nAdapterIndex2, mc_addr2, bSaveIpAddr, bIsSlave, nGrabQueSize);
	ret = mycls_pIport[ch]->SetupFromXMLFile(sXmlPath, 
											mac_addr1, ip_addr1, nAdapterIndex1, mc_addr1,
											mac_addr2, ip_addr2, nAdapterIndex2, mc_addr2,
											bSaveIpAddr, bIsSlave, nGrabQueSize);
	if( NoErr == ret ) {
		
		// �� ��������pitch8�̂�
		if( 0 != mycls_pIport[ch]->GetFrameWidth() % (mycls_pIport[ch]->GetFramePixelNBit()/my_nChannel) ) {
			LOG(em_ERR), "[%s] %s pitch�T�C�Y�s���� (%d)", my_sThreadName, mycls_pIport[ch]->GetBase(0).GetDeviceIP(), mycls_pIport[ch]->GetFramePixelNBit());	
			LOM(em_ERR), "[%s] %s pitch�T�C�Y�s���� (%d)", my_sThreadName, mycls_pIport[ch]->GetBase(0).GetDeviceIP(), mycls_pIport[ch]->GetFramePixelNBit());	
			return ApErr;
		}

		// �Œ����XML�������`�F�b�N
		if( my_nFrameW != mycls_pIport[ch]->GetFrameWidth() ||
			my_nFrameH != mycls_pIport[ch]->GetFrameHeight() ) {
			LOG(em_ERR), "[%s] %s XML�T�C�Y�s���� (w=%d/%d,h=%d/%d)", my_sThreadName, mycls_pIport[ch]->GetBase(0).GetDeviceIP(), my_nFrameW, mycls_pIport[ch]->GetFrameWidth(), my_nFrameH, mycls_pIport[ch]->GetFrameHeight());	
			LOM(em_ERR), "[%s] %s XML�T�C�Y�s���� (w=%d/%d,h=%d/%d)", my_sThreadName, mycls_pIport[ch]->GetBase(0).GetDeviceIP(), my_nFrameW, mycls_pIport[ch]->GetFrameWidth(), my_nFrameH, mycls_pIport[ch]->GetFrameHeight());	
			return ApErr;
		} else {

			LOG(em_INF), "[%s] %s �ڑ�����(w=%d,h=%d)[%s]", my_sThreadName, mycls_pIport[ch]->GetBase(0).GetDeviceIP(), my_nFrameW, my_nFrameH, sXmlPath);	
			LOM(em_INF), "[%s] %s �ڑ�����(w=%d,h=%d)[%s]", my_sThreadName, mycls_pIport[ch]->GetBase(0).GetDeviceIP(), my_nFrameW, my_nFrameH, sXmlPath);	
		}
	} else {
		LOG(em_ERR), "[%s] SetupFromXMLFile���s=%d [%s]", my_sThreadName, ret, sXmlPath);
		LOM(em_ERR), "[%s] SetupFromXMLFile���s=%d [%s]", my_sThreadName, ret, sXmlPath);
		return ret;
	}

	
	//// ���̑��K�v�ȃf�[�^
	my_emGrabMode = IportManager::GRAB_MODE_IPORT;
	return NoErr;
}

//------------------------------------------
// iPort�Ɛؒf
//------------------------------------------
void IportManager2p::DisConnect()
{
	//// �J�����̉��
	for(int ii=0; ii<MAX_IPORT_CAMNUM; ii++) {
		if( NULL != mycls_pIport[ii] ) {
			mycls_pIport[ii]->Dispose();
			SAFE_DELETE(mycls_pIport[ii]);
		}
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �X���b�h����

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int IportManager2p::ThreadFirst()
{
	//// �C�x���g�ݒ�
	HANDLE hArray[] = { my_evSmpImgTimer,		// �_�~�[�t���[���� (�r�b�g�}�b�v��AVI��)
						my_hThreadGoStop		// iPort�ڑ���
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h���C�����[�v
//------------------------------------------
int IportManager2p::ThreadMainLoop()
{
	int		nRetc;
	DWORD	ret;

	////// �V�O�i���̏���
	enum {	EV_STOP = WAIT_OBJECT_0,			// ��~�V�O�i��
			EV_SMP_IMG_TIMER,					// �e�X�g�摜�擾�� (�r�b�g�}�b�v��AVI��)
			EV_IPORT_IMG						// iPort�摜�捞 (�}�j���A���̃V�O�i��)
	};


	////////////////////////////////////
	// �O����
	////////////////////////////////////
	nRetc = ThreadFirst();						// �X���b�h�J�n���R�[���o�b�N
	LOG(em_MSG), "[%s] �X���b�h�J�n=%d [0x%X]", my_sThreadName, nRetc, GetTid());

	SetEvent(my_hThreadReady);					// �X���b�h���N���������Ƃ�`����
	

	////////////////////////////////////
	// ���C�����[�v
	////////////////////////////////////

	while( 1 ) {
		ret = WaitForMultipleObjects((my_nWaitNum+1), my_pEvWait, FALSE, INFINITE);

//-----------------------------------------------------------------------------------------------
		// �X���b�h�I��
		if( ret == EV_STOP || ! my_bThreadLoop) {
			//nRetc = ThreadLast();				// �X���b�h�I�����R�[���o�b�N
			LOG(em_MSG), "[%s] �X���b�h�I��=%d [0x%X]", my_sThreadName, nRetc, GetTid());
			return nRetc;
		}

//-----------------------------------------------------------------------------------------------
		// iPort�摜�捞
		if( ret == EV_IPORT_IMG ) {
			DWORD ch;

			for(DWORD ii=0; ii<my_nCamsetNum; ii++) {
				bool	camset_noerr = true;
				void*	p[MAX_IPORT_CAMNUM][2];				// �z���g��MAX_IPORT_CAMANGLE�����������Ach��jj�̔z��v�f���g���ӏ��������Ȃ�̂ŁA�ǐ��D��� MAX_IPORT_CAMNUM �ɂ��Ă���
				DWORD	len[MAX_IPORT_CAMNUM][2];
				WORD	seq_number[MAX_IPORT_CAMNUM][2];						// iPort�ԍ���short�Ȃ̂Œ���
				DWORD	seqwk[2];
				DWORD	status[MAX_IPORT_CAMNUM][2];							// �ꉞ�G�͎�ꂽ���ǂ��A�Ȃ񂩂��������ꍇ���Ǝv��
				ERRCODE ret[MAX_IPORT_CAMNUM];								// ����������LockForRead�̎��s
				__int64	nFastGetTime;										// �ŏ��̃J�����p�x���擾��������
				bool	lock[MAX_IPORT_CAMNUM];								// LockForRead���������Ă���������܂ł̊� true
				memset(lock, 0x00, sizeof(lock));

				
				//// 1�J�����Z�b�g���捞
				for(DWORD jj=0; jj<my_nAngleNum; jj++) {
					ch = GetCamIndex(ii, jj);

					// ���b�N
					LOM(em_MSG), "lock (set=%d, angle=%d)", ii, jj);

					ret[ch]			= mycls_pIport[ch]->LockFrame(p[ch][0], len[ch][0], seqwk[0], status[ch][0], p[ch][1], len[ch][1], seqwk[1], status[ch][1]);		// LockForRead
					seq_number[ch][0] = (WORD)seqwk[0];		seq_number[ch][1] = (WORD)seqwk[1];
					if( 0==ret[ch] ) lock[ch] = true;					
					if( 0==jj ) QueryPerformanceCounter((LARGE_INTEGER *)&nFastGetTime);
					DWORD	nResend = mycls_pIport[ch]->GetResendCount();	// ���������ꍇ�ɂ����Z�b�g����Ȃ�
					DWORD	nLost	= mycls_pIport[ch]->GetLostCount();		// ���������ꍇ�ɂ����Z�b�g����Ȃ�	
					DWORD	nQueCnt = __max( mycls_pIport[ch]->GetNQueueItemsInBuffer(true),mycls_pIport[ch]->GetNQueueItemsInBuffer(false));

					// �Z�b�g
					my_nResendTotal  += nResend;
					my_nLostTotal    += nLost;
					my_nQueueCount	  = nQueCnt;
					my_nMaxQueueCount = __max( my_nMaxQueueCount, nQueCnt );
					if( NoErr != ret[ch] ) {
						camset_noerr = false;
						LOM(em_ERR), "->ret=%d, st=%d:%d, wait=%02d, lost=%d, resend=%d, Fno=%d, seq(��,iport)=%d:%d/%d:%d", ret[ch], status[ch][0], status[ch][1], nQueCnt, nLost, nResend, my_nFrameNum, my_nSeqNum[ch], my_nSeqNumSlave[ch], seq_number[ch][0], seq_number[ch][1]);
						LOG(em_ERR), "->ret=%d, st=%d:%d, wait=%02d, lost=%d, resend=%d, Fno=%d, seq(��,iport)=%d:%d/%d:%d", ret[ch], status[ch][0], status[ch][1], nQueCnt, nLost, nResend, my_nFrameNum, my_nSeqNum[ch], my_nSeqNumSlave[ch], seq_number[ch][0], seq_number[ch][1]);

					} else {
						LOM(em_MSG), "->ret=%d, st=%d:%d, wait=%02d, lost=%d, resend=%d, Fno=%d, seq(��,iport)=%d:%d/%d:%d", ret[ch], status[ch][0], status[ch][1], nQueCnt, nLost, nResend, my_nFrameNum, my_nSeqNum[ch], my_nSeqNumSlave[ch], seq_number[ch][0], seq_number[ch][1]);
					}


					// �X�L�b�v�t���[���H
					if( 0 != my_nSkipCnt[ch] ) {
						// �X�L�b�v�t���[���Ȃ�������t���[����A���Ŏ擾
						LOG(em_MSG), "[%s] �t���[�������ׁ̈A�X�L�b�v (set=%d, ang=%d)(Fno=%d,iPort=%d:%d)",my_sThreadName, ii, jj, my_nFrameNum, seq_number[ch][0], seq_number[ch][1]); 
						mycls_pIport[jj]->ReleaseFrame();	// �ԋp
						jj--;								// �ă��[�v
						my_nSkipCnt[ch] --;
						continue;
					}
				}

				//// ��~�m�F 
				if (WAIT_TIMEOUT == WaitForSingleObject(my_hThreadGoStop, 0)) {
					// ��荞�񂾃��b�N���̃t���[���摜����� (EPC�ɂ͖���) ������ς胊�[�N�̌����ƂȂ��Ă�����ۂ�
					for(DWORD jj=0; jj<my_nCamsetNum; jj++) {
						if(lock[jj]) mycls_pIport[jj]->ReleaseFrame();
					}
					LOM(em_MSG), "��~�F��!");
					break;	// ��~���������Ă����烋�[�v�𔲂���
				}


				//// �J�����Z�b�g�P�ʂ̏��� (���펞)
				if(camset_noerr) {
					// BadImagePassthrough�ɂ����ꍇ�Alen���t���[���T�C�Y��菬�����ꍇ������B
					//   ���̏ꍇ�A�_�~�[�t���[���}��
					for(DWORD jj=0; jj<my_nAngleNum; jj++) {
						ch = GetCamIndex(ii, jj);
						const unsigned long size0 = mycls_pIport[ch]->GetBase(0).GetFrameWidth() * mycls_pIport[ch]->GetBase(0).GetFrameHeight() * my_nChannel;
						const unsigned long size1 = mycls_pIport[ch]->GetBase(1).GetFrameWidth() * mycls_pIport[ch]->GetBase(1).GetFrameHeight() * my_nChannel;

						_ASSERT( (my_nImgSize * my_nChannel) == (size0+size1) ); 

						// ���Ԃ�A0x1f��mValue�Œ�`����Ă���17�R�̂�����ł�NG�Ȃ�_���Ƃ��Ă���Ǝv����B
						if(len[ch][0] != size0 || (status[ch][0] & 0x1f) ) {
							LOG(em_WAR), "[%s] �_�~�[�t���[���Z�b�g �}�X�^�[ (set=%d, ang=%d, no=%d) missing byte %d", my_sThreadName, ii, jj, seq_number[ch][0], size0-len[ch][0]);
							p[ch][0]  = my_pDummyBuf;
							len[ch][0]= size0;
						}
						if(len[ch][1] != size1 || (status[ch][1] & 0x1f) ) {
							LOG(em_WAR), "[%s] �_�~�[�t���[���Z�b�g �X���[�u (set=%d, ang=%d, no=%d) missing byte %d", my_sThreadName, ii, jj, seq_number[ch][1], size0-len[ch][1]);
							p[ch][1]  = my_pDummyBuf;
							len[ch][1]= size1;
						}
					}
					
					// iPortSeqNo �� 0 �̂Ƃ�������΍�
					if( 0 == seq_number[ch][0] || 0 == seq_number[ch][1]  ) {
						if( 0 != my_nFrameNum && 0 == seq_number[ch][0] && 0 == seq_number[ch][1] ) {
							LOG(em_WAR), "[%s] iPort�V�[�P���X�ԍ��擾���s (set=%d)(Fno=%d, ��=%d:%d, iPort=%d:%d)", my_sThreadName, ii, my_nFrameNum, my_nSeqNum[ch], my_nSeqNumSlave[ch], seq_number[ch][0], seq_number[ch][1]);
						}
						if( 0 == seq_number[ch][0] )seq_number[ch][0] = my_nSeqNum[ch] - my_nChDiff[ch][0];
						else						seq_number[ch][1] = my_nSeqNumSlave[ch] - my_nChDiff[ch][1];
					}


					// �摜�擾
					//   EPC�� FrameNum���g�p�B�A�v������%�J�����Z�b�g���āA���Z�b�g�ڂ̃f�[�^�����肵�Ă���B
					OnFrameGrabbed(&p[ii*my_nAngleNum][0], mycls_pIport[0]->GetBase(0).GetFrameWidth(), mycls_pIport[0]->GetBase(0).GetFrameHeight(), my_nFrameNum, ii, &seq_number[ch][0], nFastGetTime);


					// �ʂ̃X�e�[�^�X�`�F�b�N (LockForRead�����s�����ꍇ�́Acamset_noerr��Ng�ƂȂ�)
					for(DWORD jj=0; jj<my_nAngleNum; jj++) {
						ch = GetCamIndex(ii, jj);
				
						// LockForRead�̉�� (�� �Q�񂷂�Ɨ�����̂Œ��ӁB�K��1��1�ƂȂ邱��)
						mycls_pIport[ch]->ReleaseFrame();

			
						for(int kk=0; kk<2; kk++) {
							WORD& mseq = 0==kk ? my_nSeqNum[ch] : my_nSeqNumSlave[ch];			// �G�C���A�X

							// �ُ�`�F�b�N (�񕜉\�ȃG���[)
							//if( mseq != seq_number[ch][kk]+my_nChDiff[ch][kk] ) {
							if( (long)mseq != ((long)seq_number[ch][kk]+(long)my_nChDiff[ch][kk]) ) {

								if(0 == my_nFrameNum / my_nCamsetNum) {	// stop�����Ƃ���seq_num�������p����Ă���ꍇ������̂ŏ���̂ݓ��ʈ���
									mseq = seq_number[ch][kk]+my_nChDiff[ch][kk];
								} else {
									LOG(em_WAR), "[%s] �t���[���Y������:�V�[�P���X�ԍ��s��v (set=%d, ang=%d, id=%d)(Fno=%d, ��:%d != iPort:%d + chdiff:%d)", my_sThreadName, ii, jj, kk, my_nFrameNum, mseq, seq_number[ch][kk], my_nChDiff[ch][kk]);
									OnAddInfo(IportLib::DIV_ID_ERROR_GRABBER_IMAGEID_MISMATCH, my_nFrameNum, ii, jj, seq_number[ch][kk] );
								}

								my_nChDiff[ch][kk] = (short)((long)mseq - (long)seq_number[ch][kk]);
								if( abs(my_nChDiffMax) > abs(my_nChDiff[ch][kk]) ) my_nChDiffMax = my_nChDiff[ch][kk];
							}

							// �V�[�P���X�ԍ����Z
							mseq ++;				// my_seq_num��iport�ƈ�v����悤�ɃC���N�������g���� (iPort�ԍ���short������)

							// mValue �Œ�` (�񕜉\�ȃG���[)
							if( IportBase::IsFrameOverrun(status[ch][kk]) ) {
								LOG(em_WAR), "[%s] �t���[���I�[�o�[�������� (set=%d, ang=%d, id=%d, no=%d, st=%d)", my_sThreadName, ii, jj, kk, my_nFrameNum, status[ch][kk]);
								OnAddInfo(IportLib::DIV_ID_ERROR_GRABBER_FRAME_OVERRUN, my_nFrameNum, ii, jj, status[ch][kk] );
							}
							if( IportBase::IsGrabberFIFOOverrun(status[ch][kk]) ) {
								LOG(em_WAR), "[%s] FIFO�I�[�o�[�������� (set=%d, ang=%d, id=%d, no=%d, st=%d)", my_sThreadName, ii, jj, kk, my_nFrameNum, status[ch][kk]);
								OnAddInfo(IportLib::DIV_ID_ERROR_GRABBER_FIFO_OVERRUN, my_nFrameNum, ii, jj, status[ch][kk] );
							}
							if( IportBase::IsImageDropped(status[ch][kk]) ) {
								LOG(em_WAR), "[%s] IMAGE DROP���� (set=%d, ang=%d, id=%d, no=%d, st=%d)", my_sThreadName, ii, jj, kk, my_nFrameNum, status[ch][kk]);
								OnAddInfo(IportLib::DIV_ID_ERROR_GRABBER_IMAGE_DROPPED, my_nFrameNum, ii, jj, status[ch][kk] );
							}
							if( IportBase::IsPartialLineMissing(status[ch][kk]) ) {
								LOG(em_WAR), "[%s] PARTIAL LINE MISSING���� (set=%d, ang=%d, id=%d, no=%d, st=%d)", my_sThreadName, ii, jj, kk, my_nFrameNum, status[ch][kk]);
								OnAddInfo(IportLib::DIV_ID_ERROR_GRABBER_PARTIAL_LINE_MISSING, my_nFrameNum, ii, jj, status[ch][kk] );
							}
							if( IportBase::IsFullLineMissing(status[ch][kk]) ) {
								LOG(em_WAR), "[%s] FULL LINE MISSING���� (set=%d, ang=%d, id=%d, no=%d, st=%d)", my_sThreadName, ii, jj, kk, my_nFrameNum, status[ch][kk]);
								OnAddInfo(IportLib::DIV_ID_ERROR_GRABBER_FULL_LINE_MISSING, my_nFrameNum, ii, jj, status[ch][kk] );
							}
						}
					}
				}

				//// FYI: AbortLock()���������ret��CY_RESULT_ABORTED�ɂȂ�
				else {
#if 0
					// ���s����ch(�܂��~�����̂�)�ɂ� �_�~�[�o�b�t�@�̃A�h���X������
					bool len_ok = true;
					for(DWORD jj=0; jj<my_nAngleNum; jj++) {
						ch = GetCamIndex(ii, jj);
						if( ret[ch] == CY_RESULT_MISSING_PACKETS || ret[ch] == CY_RESULT_IMAGE_ERROR) {
							p[ch]	= my_pDummyBuf;
							len[ch] = my_nImgSize;
						}
						if( len[ch] != my_nImgSize) len_ok = false;
					}

					// �摜�擾
					//   �摜�T�C�Y������ȏꍇ�̂�
					if(len_ok) OnFrameGrabbed(&p[ii*my_nAngleNum], len[ii*my_nAngleNum], my_nFrameNum, ii, seq_number, nFastGetTime);
#endif					

					for(DWORD jj=0; jj<my_nAngleNum; jj++) {
						ch = GetCamIndex(ii, jj);
						// ���킾����iPort�̂݉��
						if( ret[ch] == NoErr ) {
							// LockForRead�̉�� (�� �Q�񂷂�Ɨ�����̂Œ��ӁB�K��1��1�ƂȂ邱��)
							mycls_pIport[ch]->ReleaseFrame();
							

							for(int kk=0; kk<2; kk++) {
								WORD& mseq = 0==kk ? my_nSeqNum[ch] : my_nSeqNumSlave[ch]; // �G�C���A�X

								// �ُ�`�F�b�N (�񕜉\�ȃG���[)
								if( mseq != seq_number[ch][kk]+my_nChDiff[ch][kk] ) {
									LOG(em_WAR), "[%s] �t���[���Y������:�V�[�P���X�ԍ��s��v (set=%d, ang=%d, id=%d)(Fno=%d, ��=%d, iPort=%d + chdiff=%d)", my_sThreadName, 
										ii, jj, kk, my_nFrameNum, mseq, seq_number[ch][kk], my_nChDiff[ch][kk]);
									OnAddInfo(IportLib::DIV_ID_ERROR_GRABBER_IMAGEID_MISMATCH, my_nFrameNum, ii, jj, seq_number[ch][kk] );
								
									my_nChDiff[ch][kk] = (short)((int)mseq - (int)seq_number[ch][kk]);
									if( abs(my_nChDiffMax) > abs(my_nChDiff[ch][kk]) ) my_nChDiffMax = my_nChDiff[ch][kk];
								}

								// �V�[�P���X�ԍ����Z
								mseq ++;				// my_seq_num��iport�ƈ�v����悤�ɃC���N�������g���� (iPort�ԍ���short������)				
							}

						} else if( ret[ch] == CY_RESULT_TIMEOUT ) {
							// LockForRead ���s (�񕜉\�ȃG���[)
							LOG(em_WAR), "[%s] LockForRead�^�C���A�E�g (set=%d, ang=%d, no=%d)", my_sThreadName, ii, jj, my_nFrameNum);
							OnAddInfo(IportLib::DIV_ID_ERROR_GRABBER_FRAME_LOCK_TIMEOUT, my_nFrameNum, ii, jj, 0 );

						} else if( ret[ch] != CY_RESULT_ABORTED ) {
							// LockForRead ���s (�񕜉\�ȃG���[)
							LOG(em_WAR), "[%s] LockForRead���s (set=%d, ang=%d, no=%d, retCy=%d)", my_sThreadName, ii, jj, my_nFrameNum, ret[ch]);
							OnAddInfo(IportLib::DIV_ID_ERROR_GRABBER_FRAME_LOCK_FAILED, my_nFrameNum, ii, jj, ret[ch] );

						} else {
							// CY_RESULT_ABORTED ���́A��̒�~�m�F�ŃL���b�`�ł��Ă���͂��Ȃ̂ŁA�����܂ŗ��邱�Ƃ͂��肦�Ȃ� 
						}
					}
				}
			}
			my_nFrameNum ++;			// �t���[���ԍ�
		}

//-----------------------------------------------------------------------------------------------
		// �e�X�g�摜�p
		else if( ret == EV_SMP_IMG_TIMER) {
			if( IportManager::GRAB_MODE_BMP == my_emGrabMode ) {
				OnBmpFrameGrabbed();

			} else if(IportManager::GRAB_MODE_AVI == my_emGrabMode ) {
				OnAviFrameGrabbed();
			}
		}
	}

	// ���肦�Ȃ�
	LOG(em_ERR), "[%s] ���C�����[�v���甲�����I�I=%d [0x%X]", my_sThreadName, nRetc, GetTid());
	return -1;
}

//------------------------------------------
// 1�J�����Z�b�g���̉摜���擾���āA�X���b�h���[�v�L���[�ɃL���[�C���O
// void** pImgs �摜(�J�����p�x��)
// long nX �摜�T�C�Y (1�R����)
// long nY �摜�T�C�Y (1�R����)
// DWORD nFrameNum �t���[��No (1�R����)
// DWORD nCamset �J�����Z�b�g�P�ʂ̉���(0�I���W��)
// WORD* nSeqNum iPort�V�[�P���X�ԍ�
// __int64 nFastTime �ŏ��̃J�����p�x���擾��������
//------------------------------------------
void IportManager2p::OnFrameGrabbed(void** pImgs, long nX, long nY, DWORD nFrameNum, DWORD nCamset, WORD* nSeqNum, __int64 nFastTime)
{
	//// �󂫊���擾
	FRAME_DATA* p = myque_pFrame->GetFree(0);				// �Z�}�t�H�[�҂��o�[�W����(�����Ɍ��ʂ�Ԃ�����)
	if( NULL == p ) {
		// ��󂫂Ȃ� (�񕜉\�ȃG���[)
		LOG(em_ERR), "[%s] GRAB�o�b�t�@�I�[�o����(set=%d, No=%d):�X���b�h���[�v�L���[�󂫖���!", my_sThreadName, nCamset, nFrameNum);
		OnAddInfo(IportLib::DIV_ID_ERROR_NO_IMG_QUEFULL, nFrameNum, nCamset, 0, 0 );
		return;
	}

	/*//// ----->>>>
			p->pImgs[ 10 +my_nFrameW*0] = 0x0f;
			p->pImgs[ 10 +my_nFrameW*1] = 0x0f;
			p->pImgs[ 10 +my_nFrameW*2] = 0x0f;
			p->pImgs[ 10 +my_nFrameW*3] = 0x0f;
			p->pImgs[ 10 +my_nFrameW*4] = 0x0f;

				CString str;
				str.Format("C:\\test\\a%d_%d.bmp",nCamset,nFrameNum) ;
				KizuBitmapManager k;
				k.DataToBMPfile_Gray(my_nFrameW, my_nFrameH, &p->pImgs[ my_nImgSize * 0 ], str); 

			//	std::ofstream   os;
			//	os.open(str, std::ios::binary);
			//	os.write((const char*)&p->pImgs[ my_nImgSize * jj ], my_nImgSize);
			//	os.close();
								
	//*/// <<<<-------	

	//// �摜���i�[
	// ���Ԃ��Z�b�g
	p->cls_Time.PutTimestamp(0, nFastTime);			// ���O�F�ŏ��̃J�����p�x���擾��������

	// �f�[�^�R�s�[
	p->nCamset	= nCamset;
	p->nLen		= (nX*2) * nY;	// 1�R����
	p->nNo		= nFrameNum;

	for(DWORD ii=0; ii<my_nAngleNum; ii++) {
		//p->nSeqNum[ii] = nSeqNum[ii];
		p->nSeqNum[ii*2+0] = nSeqNum[ii];
		p->nSeqNum[ii*2+1] = nSeqNum[ii+1];	// �����Ȃ�b��
		p->hInfo[ii]   = my_hAddInfo[GetCamIndex(nCamset, ii)];


		if( ! my_bImgTurn_LR ) {

			//�E�E����	SwdetectMMX::hflip_righthalf
			//	���摜
			//	�E�摜
			//	������̉摜�̈�
			//	������̕��s�b�`
			//	������̕���f
			//	������̍�����f
			KizuMMX::hflip_righthalf((BYTE*)pImgs[2*ii], (BYTE*)pImgs[2*ii+1], (BYTE*)&p->pImgs[ii*my_nImgSize], nX*2, nX*2, nY);
		} else {
			// ���E���]
			KizuMMX::hflip_righthalf((BYTE*)pImgs[2*ii+1], (BYTE*)pImgs[2*ii], (BYTE*)&p->pImgs[ii*my_nImgSize], nX*2, nX*2, nY);
		}

//// �e�X�g
//		memcpy( &p->pImgs[ii*my_nImgSize], pImgs[2*ii], nX * nY );				// ������
//		memcpy( &p->pImgs[ii*my_nImgSize + (nX * nY)], pImgs[2*ii+1], nX * nY );	// �E�����̂Ђ�����Ԃ��ĂȂ����

	
		//memset(&p->pImgs[ my_nFrameW*0 ],0xff,128);
		//memset(&p->pImgs[ my_nFrameW*1 ],0xff,128);
		//memset(&p->pImgs[ my_nFrameW*2 ],0xff,128);
		//memset(&p->pImgs[ my_nFrameW*3 ],0xff,128);
		//memset(&p->pImgs[ my_nFrameW*4 ],0xff,128);
	}

	p->cls_Time.PutTimestamp(1);					// ���P�F�S���̃J�����p�x������������ (=��)

	// �o�^
	myque_pFrame->AddPool(p); 
//	myque_pFrame->AddFree(p);  // �e�X�g
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���̑�����

//------------------------------------------
// �J�������x���擾 (�J�����Ɛڑ��������_��)
// DWORD set �J�����Z�b�g (0�I���W��)
// DWORD ang �J�����p�x (0�I���W��)
// float& temp ���x [�x]
//------------------------------------------
bool IportManager2p::SendSerialTemp(DWORD set, DWORD ang, float& temp)
{
	DWORD	ch = GetCamIndex(set, ang);
	CString sAns;
	ERRCODE ret = mycls_pIport[ch]->SendSerialCommand("vt\r", &sAns, 40, false);

	if( NoErr != ret ) {temp = 0.0; return false;}

	//// ���l����
	int numstart = sAns.FindOneOf("0123456789.");
	CString numstr = sAns.Mid(numstart).SpanIncluding("0123456789.");
	temp = (float)atof((LPCTSTR)numstr);
	return true;
}
//------------------------------------------
// 1�J��������iPort�V�[�P���XNo�Y���␳�p 
// DWORD set �J�����Z�b�g (0�I���W��)
// DWORD ang �J�����p�x (0�I���W��)
// int nSkipCnt (0:���� 1:�}�X�^�[��1��X�L�b�v -1:�X���[�u��1��X�L�b�v)
//------------------------------------------
void IportManager2p::SetSyncPort(DWORD set, DWORD ang, int nSkipCnt)
{
	DWORD	ch = GetCamIndex(set, ang);
	mycls_pIport[ch]->SetSyncPort(nSkipCnt);
}