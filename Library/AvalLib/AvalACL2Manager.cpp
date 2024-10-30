#include "StdAfx.h"
#include <vector>
#include "AvalACL2Manager.h"

#include <AcapLib2.h>
#pragma comment(lib, "AcapLib2.lib")


// �e�X�g�摜�擾�p
//#include "..\ImgLib\CLASS\KizuAviManager.h"									// AVI�t�@�C��
#include "..\KizuLib\CLASS\KizuBitmapManager.h"								// BitMap�t�@�C��

#include <io.h>
#include <fcntl.h>



// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

using namespace AvalLib;



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���[�J���֐�

//------------------------------------------
// �R���X�g���N�^
// CString sMyManagerName ���N���X�̖���
// CString sMyGrabberName Grabber�N���X�̖���
//------------------------------------------
AvalACL2Manager::AvalACL2Manager(CString sMyManagerName, CString sMyGrabberName):
// ���O
GrabberManager(GRAB_TYPE_AVAL, sMyManagerName),

my_emGrabMode(GRAB_MODE_HW),
my_nFrameNum(0),
my_nImgSize(0),
my_nFrameW(0),
my_nFrameH(0),

my_nGrabber(0),
my_pGrabber(NULL),
my_pDummyBuf(NULL),
my_bImgTurn_LR(false),
my_bThreadLoop(false)
{
	my_nChannel = 1;		// �O���[�摜

	memset(my_hAddInfo, 0x00, sizeof(my_hAddInfo));
	my_evSmpImgTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	my_hHwRun = CreateEvent(NULL, TRUE, FALSE, NULL);		// �}�j���A��
	my_hInited =  CreateEvent(NULL, FALSE, FALSE, NULL);


	// �e�X�g�摜�p
	my_nSmpNowNo = 0;
	for(int ii=0; ii<MAX_CAMNUM; ii++) {
		for(int jj=0; jj<MAX_COLOR_CHANNEL; jj++ ) mycls_Aiv[jj][ii] = NULL;
	}
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
AvalACL2Manager::~AvalACL2Manager(void)
{
	Dispose();

	SAFE_DELETE_HANDLE(my_evSmpImgTimer);
	SAFE_DELETE_HANDLE(my_hHwRun);
}


//------------------------------------------
// ������
// int board_id		�{�[�h�ԍ�(0�I���W��)
// int board_ch_id	�I�[�v���`�����l�� (1�I���W��)
// const char* ini_filename	�ݒ�t�@�C���t���p�X
// int nbuff		�o�b�t�@��

// DWORD nFrameW �t���[���T�C�Y
// DWORD nFrameH �t���[���T�C�Y
// BYTE nDummyVal �_�~�[�摜�̐F
//------------------------------------------
BOOL AvalACL2Manager::Init(int board_id, int board_ch_id, const char* ini_filename, int nbuff, DWORD nFrameW, DWORD nFrameH, BYTE nDummyVal)
{
	my_nGrabber = 1;
	my_pGrabber = new AvalACL2Base[my_nGrabber];
	my_pGrabber[0].SetThreadName("AvalBase");
	my_pGrabber[0].SetLogMgr(mcls_pLog );

	my_nBatchSize.resize(my_nGrabber);
	my_nMaxBatchSize.resize(my_nGrabber);

	if( ! my_pGrabber[0].Init(board_id, board_ch_id, ini_filename, nbuff) ) {
		return FALSE;
	}

	my_nBaseBufNum = nbuff;
	InitInfo(nFrameW, nFrameH, nDummyVal);
	my_bThreadLoop = true;
	StartLock(THREAD_PRIORITY_TIME_CRITICAL);
	return TRUE;
}

//------------------------------------------
// ������ (�{�[�h�ԍ��̏��������Ɋ��蓖�Ă�)
// int nchannel				�ڑ���
// const char* ini_filename	�ݒ�t�@�C���t���p�X
// int nbuff				�o�b�t�@��

// DWORD nFrameW �t���[���T�C�Y
// DWORD nFrameH �t���[���T�C�Y
// BYTE nDummyVal �_�~�[�摜�̐F
//------------------------------------------
BOOL AvalACL2Manager::Init(int nchannel, const char* ini_filename, int nbuff, DWORD nFrameW, DWORD nFrameH, BYTE nDummyVal)
{
	// �{�[�h���擾
    ACAPBOARDINFOEX		bi;
    if( ! AcapGetBoardInfoEx(&bi) ) {
        return FALSE;
    }

	// �����ݒ�
	my_nGrabber			= nchannel;
	my_pGrabber			= new AvalACL2Base[my_nGrabber];
	my_nBatchSize.resize(my_nGrabber);
	my_nMaxBatchSize.resize(my_nGrabber);

	my_nBaseBufNum		= nbuff;

	int	nch = 0;
	for(int ii=0; ii<bi.nBoardNum && nch < nchannel; ii++ ) {
		for(int jj=0; jj<bi.BOARDINDEX[ii].nChannelNum && nch < nchannel; jj++ ) {
	
			const int board_ch_id = jj + 1;
	        
			CString sWk;
			sWk.Format("AvalBase_%d(%d-%d)", nch, bi.BOARDINDEX[ii].nBoardID, board_ch_id); 
			my_pGrabber[nch].SetThreadName(sWk);
			my_pGrabber[nch].SetLogMgr(mcls_pLog );

			if( ! my_pGrabber[nch].Init( bi.BOARDINDEX[ii].nBoardID, board_ch_id, ini_filename, nbuff) ) {
				return FALSE;
			}
			LOG(em_MSG), "[%s] Init(): grabber[%d] --> (%d, %d)", my_sThreadName, nch, bi.BOARDINDEX[ii].nBoardID, board_ch_id);
			nch ++;
		}
	}
	if( nch < nchannel) {
        // channel������Ȃ�
        LOG(em_ERR), "[%s] Init(): nch < nchannel", my_sThreadName);
        return FALSE;
	}

	InitInfo(nFrameW, nFrameH, nDummyVal);
	my_bThreadLoop = true;
	StartLock(THREAD_PRIORITY_TIME_CRITICAL);
	return TRUE;
}

//------------------------------------------
// ������	(�����w��`�����l���Ɋ��蓖��)
// int nchannel				�ڑ���
// int board_id*		�{�[�h�ԍ�(0�I���W��)
// int board_ch_id*		�I�[�v���`�����l�� (1�I���W��)
// const char* ini_filename	�ݒ�t�@�C���t���p�X
// int nbuff		�o�b�t�@��

// DWORD nFrameW �t���[���T�C�Y
// DWORD nFrameH �t���[���T�C�Y
// BYTE nDummyVal �_�~�[�摜�̐F
//------------------------------------------
BOOL AvalACL2Manager::Init(int nchannel, int const* board_id, int const* board_ch_id, const char* ini_filename, int nbuff, DWORD nFrameW, DWORD nFrameH, BYTE nDummyVal)
{
	// �{�[�h���擾
    ACAPBOARDINFOEX		bi;
    if( ! AcapGetBoardInfoEx(&bi) ) {
        return FALSE;
    }

	// �����ݒ�
	my_nGrabber			= nchannel;
	my_pGrabber			= new AvalACL2Base[my_nGrabber];
	my_nBatchSize.resize(my_nGrabber);
	my_nMaxBatchSize.resize(my_nGrabber);

	my_nBaseBufNum		= nbuff;


	for(int ii=0; ii<nchannel; ii++ ) {
		CString sWk;
		sWk.Format("AvalBase_%d(%d-%d)", ii, board_id[ii], board_ch_id[ii]); 
		my_pGrabber[ii].SetThreadName(sWk);
		my_pGrabber[ii].SetLogMgr(mcls_pLog );

		if( ! my_pGrabber[ii].Init( board_id[ii], board_ch_id[ii], ini_filename, nbuff) ) {
			return FALSE;
		}
		LOG(em_MSG), "[%s] Init(): grabber[%d] --> (%d, %d)", my_sThreadName, ii, board_id[ii], board_ch_id[ii]);	
	}
	
	InitInfo(nFrameW, nFrameH, nDummyVal);
	my_bThreadLoop = true;
	StartLock(THREAD_PRIORITY_TIME_CRITICAL);
	return TRUE;
}

//------------------------------------------
// �������Z�b�g
//------------------------------------------
void AvalACL2Manager::InitInfo(DWORD nFrameW, DWORD nFrameH, BYTE nDummyVal)
{
	my_nImgSize	  = nFrameW*nFrameH;	// �g���@����������`���Ă���
	my_nFrameW	  = nFrameW;
	my_nFrameH	  = nFrameH;

	//// �_�~�[�t���[���̐���
	if (NULL != my_pDummyBuf) _aligned_free(my_pDummyBuf);		// �O�̂���
	DWORD image_size = my_nFrameW * my_nFrameH * my_nChannel;
	my_pDummyBuf = _aligned_malloc(image_size, 32);
	_ASSERT(my_pDummyBuf);
	memset(my_pDummyBuf, nDummyVal, image_size);

	SetEvent(my_hInited);
}

//------------------------------------------
// �J��
//------------------------------------------
void AvalACL2Manager::Dispose()
{
	// �X���b�h����~���Ă��Ȃ��������~
	Stop();	// ��


	if(NULL != my_pGrabber) {
		for(int ii=0; ii<my_nGrabber; ii++){
			my_pGrabber[ii].Dispose();
		}
		SAFE_DELETE_ARRAY(my_pGrabber);
	}


	my_nGrabber = 0;
	my_nFrameNum = 0;
	my_nBaseBufNum = 0;	
	my_nBatchSize.clear();
	my_nMaxBatchSize.clear();


	//// �_�~�[�摜�̈���
	if (NULL != my_pDummyBuf) {
		_aligned_free(my_pDummyBuf);
		my_pDummyBuf = NULL;
	}
}

//------------------------------------------
// �X���b�h�J�n
//------------------------------------------
bool AvalACL2Manager::Start()
{
	return true;
}

//------------------------------------------
// �X���b�h��~
//------------------------------------------
bool AvalACL2Manager::Stop()
{
	my_bThreadLoop = false;
	StopGrab();

	for(int ii=0; ii<my_nGrabber; ii++){
		my_pGrabber[ii].AbortFrameWait();
	}

	ThreadManager::Stop(5000);
	return true;
}	



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �B���n

//------------------------------------------
// �B���J�n
//------------------------------------------
BOOL AvalACL2Manager::StartGrab()
{
	LOG(em_MSG), "[%s] StartGrab���", my_sThreadName);	
	LOM(em_MSG), "[%s] StartGrab���", my_sThreadName);

	//==============================================
	// ������돉����
	my_nFrameNum		= 0;


	//==============================================
	// HW
	if( GRAB_MODE_HW == my_emGrabMode ) {

		for(int ii=0; ii<my_nGrabber; ii++){
			BOOL ret = my_pGrabber[ii].StartGrab();
			if( ! ret ) {
				DWORD cErr, bErr, eErr;
				cErr = my_pGrabber[ii].GetLastError(&bErr, &eErr);
				LOG(em_ERR), "[%s] StartGrab���s [%d], Common = %d, Board = %d, Extend = %d", my_sThreadName, ii, cErr, bErr, eErr);
				LOM(em_ERR), "[%s] StartGrab���s [%d], Common = %d, Board = %d, Extend = %d", my_sThreadName, ii, cErr, bErr, eErr);


				// StartGrab()�����̂��~�߂�
				for(int jj=0; jj<ii; jj++)
				{
					my_pGrabber[jj].AbortFrameWait();
					my_pGrabber[jj].StopGrab();
				}

				return cErr;
			}
		}

		//// �����܂ŗ����� StartGrab�������������R������
		SetEvent(my_hHwRun);			// ���C���X���b�h�����s���
	}

	//==============================================
	// BitMap or AVI
	else if( GRAB_MODE_BMP == my_emGrabMode || 
			 GRAB_MODE_AVI == my_emGrabMode ) {

///		my_nSmpNowNo = my_nSmpIntervalNo[0];
		__int64 ts = my_nSmpCycleTimer * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
		SetWaitableTimer(my_evSmpImgTimer, (LARGE_INTEGER *)&ts, my_nSmpCycleTimer, NULL, NULL, FALSE);

	}


	my_bGrabRun = true;
	LOG(em_MSG), "[%s] StartGrab ����", my_sThreadName);
	LOM(em_MSG), "[%s] StartGrab ����", my_sThreadName);	
	return 0;
}

//------------------------------------------
// �B����~
//------------------------------------------
BOOL AvalACL2Manager::StopGrab()
{
	LOG(em_MSG), "[%s] StopGrab���", my_sThreadName);	
	LOM(em_MSG), "[%s] StopGrab���", my_sThreadName);

	BOOL ret = TRUE;

	//==============================================
	// iPort
	if( GRAB_MODE_HW == my_emGrabMode ) {
		ResetEvent(my_hHwRun);			// ���C���X���b�h�����s���


		for(int ii=0; ii<my_nGrabber; ii++){
			BOOL r = my_pGrabber[ii].StopGrab();
			if( ! r ) {
				DWORD cErr, bErr, eErr;
				cErr = my_pGrabber[ii].GetLastError(&bErr, &eErr);
				LOG(em_ERR), "[%s] StopGrab���s [%d], Common = %d, Board = %d, Extend = %d", my_sThreadName, ii, cErr, bErr, eErr);
				LOM(em_ERR), "[%s] StopGrab���s [%d], Common = %d, Board = %d, Extend = %d", my_sThreadName, ii, cErr, bErr, eErr);
			}
			ret &= r;
		}
	}

	//==============================================
	// BitMap or AVI
	else if( GRAB_MODE_BMP == my_emGrabMode || 
			 GRAB_MODE_AVI == my_emGrabMode ) {
		CancelWaitableTimer(my_evSmpImgTimer);
	}

	my_bGrabRun = false;
	LOG(em_MSG), "[%s] StopGrab ����", my_sThreadName);
	LOM(em_MSG), "[%s] StopGrab ����", my_sThreadName);	
	return ret ? 0 : -1;
}

//------------------------------------------
// ���Z�b�g
//------------------------------------------
bool AvalACL2Manager::Reset()
{
	my_nFrameNum = 0;
	return true;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �X���b�h����

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int AvalACL2Manager::ThreadFirst()
{
	// �C�x���g�ݒ�
	HANDLE hArray[] = { my_evSmpImgTimer,		// �_�~�[�t���[���� (�r�b�g�}�b�v��AVI��)
						my_hHwRun				// �{�[�h�ڑ����̂�
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}


//------------------------------------------
// �X���b�h���C�����[�v
//------------------------------------------
int AvalACL2Manager::ThreadMainLoop()
{
	DWORD	ret;

	////// �V�O�i���̏���
	enum {	EV_STOP = WAIT_OBJECT_0,			// ��~�V�O�i��
			EV_SMP_IMG_TIMER,					// �e�X�g�摜�擾�� (�r�b�g�}�b�v��AVI��)
			EV_RUN								// �摜�捞 (�}�j���A���̃V�O�i��)
	};


	////////////////////////////////////
	// �̈�m��

	WaitForSingleObject(my_hInited, INFINITE);

	AvalACL2Base::FrameWaitResult*	emWaitResult = new AvalACL2Base::FrameWaitResult[my_nGrabber];
	void**	p = new void* [my_nGrabber];

	std::vector< void *> p_batch [MAX_CAMNUM];
	for(int ii=0; ii<my_nGrabber; ii++) {
		p_batch[ii].resize( my_nBaseBufNum );
	}
	DWORD nSeqNum[64] = {};
//	memset(nSeqNum, 0, sizeof(DWORD) * my_nGrabber);
	m_nSeqNum = nSeqNum;

	


	////////////////////////////////////
	// �O����
	bool bWaitAbort = false;
	int nRetc = ThreadFirst();						// �X���b�h�J�n���R�[���o�b�N
	if (INVALID_HANDLE_VALUE != my_hThreadReady) { SetEvent(my_hThreadReady); }		// ���s�ҋ@��

	LOG(em_MSG), "[%s] �X���b�h�J�n=%d [0x%X]", my_sThreadName, nRetc, GetTid());


	////////////////////////////////////
	// ���C�����[�v
	while( my_bThreadLoop ) {

		ret = WaitForMultipleObjects((my_nWaitNum+1), my_pEvWait, FALSE, INFINITE);

//-----------------------------------------------------------------------------------------------
		// �X���b�h�I��
		if( ret == EV_STOP || ! my_bThreadLoop) {
			LOG(em_MSG), "[%s] �X���b�h�I��=%d [0x%X]", my_sThreadName, ret, GetTid());
			break;
		}


//-----------------------------------------------------------------------------------------------
		// �摜�捞
		if( ret == EV_RUN ) {

			//------------------------------
			// �t���[�������҂�
			DWORD	nframe = 0;				// �擾�\�ȃt���[����
			__int64	nFastGetTime;			// �ŏ��̏����擾��������

			for(int ii=0; ii<my_nGrabber; ii++) {

				if( 0 ) {
					LOM(em_MSG), "lock (set=%d)", ii);
					emWaitResult[ii]	= my_pGrabber[ii].WaitNFrameReady( my_nBatchSize[ii], INFINITE);
					LOM(em_MSG), "(%d)->ret=%d, Fno=%d, batch=%d", ii, emWaitResult[ii], my_nFrameNum, my_nBatchSize[ii]);
				}
				else{
					emWaitResult[ii]	= my_pGrabber[ii].WaitNFrameReady( my_nBatchSize[ii], 0);
					if(0 == my_nBatchSize[ii]){
						//�҂K�v������
						//// �t���[���摜�擾�B���u���b�N�B
						emWaitResult[ii]	= my_pGrabber[ii].WaitNFrameReady( my_nBatchSize[ii], 300 );
						if( my_nBatchSize[ii] > 0 )
						LOM(em_MSG), "(%d)->ret=%d, Fno=%d, batch=%d", ii, emWaitResult[ii], my_nFrameNum, my_nBatchSize[ii]);
					}
					else{
					//�҂��Ȃ��Ă�����
						LOM(em_MSG), "get  (set=%d) ->ret=%d, Fno=%d, batch=%d", ii, emWaitResult[ii], my_nFrameNum, my_nBatchSize[ii]);
					}
				}
				my_nMaxBatchSize[ii ]= __max( my_nMaxBatchSize[ii], my_nBatchSize[ii]);			
				if( 0 == ii) {
					nframe		= my_nBatchSize[ii] ;
					QueryPerformanceCounter((LARGE_INTEGER *)&nFastGetTime);
				}
				else{
					nframe =  __min(my_nBatchSize[ii], nframe) ;
				}

				// �X���b�h���Ƃ߂鎞����ABORT����
				if( AvalACL2Base::FrameWait_Aborted == emWaitResult[ii] ) {
					bWaitAbort = true;
					break;
				}

				// ����ȊO
				if( AvalACL2Base::FrameWait_Success != emWaitResult[ii] &&
					AvalACL2Base::FrameWait_Timeout != emWaitResult[ii]) {
					OnGrabError( my_nFrameNum, ii, emWaitResult[ii] );	
				}
			}
			if(bWaitAbort) break;

			if( nframe == 0 ){
//				LOM(em_MSG), "!? nframe = 0, bo=%d, my=%d",  my_nBatchSize[0], my_nFrameNum);
				continue;
			}


			//------------------------------
			// �t���[�����o��
			nframe = __min( nframe, my_nBaseBufNum );
			for(int ii=0; ii<my_nGrabber; ii++) {
				DWORD nvalid;
				emWaitResult[ii]	= my_pGrabber[ii].GetFramePointers( &p_batch[ii][0], nframe, nvalid, nSeqNum[ii] );
				_ASSERT(nframe == nvalid);

				if( AvalACL2Base::FrameWait_Success != emWaitResult[ii] ) {
					OnGrabError( my_nFrameNum, ii, emWaitResult[ii] );
				}
			}

			//------------------------------
			// ������
			for(DWORD ii=0; ii<nframe; ii++ ) {
				for(int jj=0; jj<my_nGrabber; jj++) {
					p[jj] = p_batch[jj][ii];
				}
				// 1�J�����Z�b�g���̉摜���擾���āA�X���b�h���[�v�L���[�ɃL���[�C���O 
				OnFrameGrabbed(p, my_nFrameNum++, (int*)nSeqNum, nFastGetTime );
				// �V�[�P���XNo���Z
				for(int jj=0; jj<my_nGrabber; jj++) {
					nSeqNum[jj] ++;
				}
			}
			for(int ii=0; ii<my_nGrabber; ii++) {
				if( ! my_pGrabber[ii].CheckOverrun(nframe) ) {
					OnGrabError(my_nFrameNum, ii, AvalACL2Base::FrameWaitResult::FrameWait_Overrun);
				}
			}
		}

//-----------------------------------------------------------------------------------------------
		//// �e�X�g�摜�p
		else if( ret == EV_SMP_IMG_TIMER) {
			if (GRAB_MODE_BMP == my_emGrabMode) {
				OnBmpFrameGrabbed();
			}
			for (int jj = 0; jj<my_nGrabber; jj++) {
				nSeqNum[jj] ++;
			}
		//	} else if(IportManager::GRAB_MODE_AVI == my_emGrabMode ) {
		//		OnAviFrameGrabbed();
		//	}
		}

	}


	////////////////////////////////////
	// ��n��
	delete [] emWaitResult;
//	delete [] nSeqNum;
	delete []p;
	
	m_nSeqNum = NULL;

	//nRetc = ThreadLast();				// �X���b�h�I�����R�[���o�b�N
	LOG(em_MSG), "[%s] �X���b�h�I��=%d [0x%X]", my_sThreadName, nRetc, GetTid());
	return nRetc;
}


//------------------------------------------
// 1�J�����Z�b�g���̉摜���擾���āA�X���b�h���[�v�L���[�ɃL���[�C���O
// void** pImgs �摜(�J�����p�x��)
// DWORD nFrameNum �t���[��No (1�R����)
// int* nSeqNum iPort�V�[�P���X�ԍ�
// __int64 nFastTime �ŏ��̃J�����p�x���擾��������
//------------------------------------------
void AvalACL2Manager::OnFrameGrabbed(void** pImgs, DWORD nFrameNum, int* nSeqNum, __int64 nFastTime)
{
	//// �󂫊���擾
	FRAME_DATA* p = myque_pFrame->GetFree(0);				// �Z�}�t�H�[�҂��o�[�W����(�����Ɍ��ʂ�Ԃ�����)
	if( NULL == p ) {
		// ��󂫂Ȃ� (�񕜉\�ȃG���[)
		LOG(em_ERR), "[%s] GRAB�o�b�t�@�I�[�o����(SNo=%d, FNo=%d):�X���b�h���[�v�L���[�󂫖���!", my_sThreadName, nSeqNum[0], nFrameNum);
		OnAddInfo(IportLib::DIV_ID_ERROR_NO_IMG_QUEFULL, nFrameNum, nSeqNum[0], 0, 0 );
		return;
	}


	//// �摜���i�[
	p->cls_Time.PutTimestamp(0, nFastTime);			// ���O�F�ŏ��̃J�����p�x���擾��������


	// �f�[�^�R�s�[
	const long nImageSize = my_pGrabber[0].GetWidth() * my_pGrabber[0].GetHeight();
	p->nCamset	= 0;
	p->nLen		= nImageSize;
	p->nNo		= nFrameNum;


	for(int ii=0; ii<my_nGrabber; ii++) {
		p->nSeqNum[ii] = (WORD)nSeqNum[ii];		//iportSQNO�Ƃ̌݊������Ƃ邽�߁A16bit�ɂ���
		p->hInfo[ii]   = my_hAddInfo[ii];

		if( ! my_bImgTurn_LR ) {
			KizuMMX::mem_copy_64xmm(&p->pImgs[ii*nImageSize], pImgs[ii], nImageSize);
		} else {
			_ASSERT( false );	// ���Ή�
		}
	}

	p->cls_Time.PutTimestamp(1);					// ���P�F�S���̃J�����p�x������������ (=��)


	// �o�^
	myque_pFrame->AddPool(p); 
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���̑�����

//------------------------------------------
// �G���[��`
// unsigned long frame_num				�t���[��No
// int grabber_index					Ch
// AvalACL2Base::FrameWaitResult err	�G���[No
//------------------------------------------
void AvalACL2Manager::OnGrabError(unsigned long frame_num, int grabber_index, AvalACL2Base::FrameWaitResult err)
{
	LOG(em_WAR), "[%s] Grab Error : frame_num=%d, grabber_index=%d, err=%d", my_sThreadName, frame_num, grabber_index, err);
	LOM(em_WAR), "[%s] Grab Error : frame_num=%d, grabber_index=%d, err=%d", my_sThreadName, frame_num, grabber_index, err);


	if (err == AvalACL2Base::FrameWait_ACLError) {
		DWORD cErr, bErr, eErr;
		cErr = my_pGrabber[grabber_index].GetLastError(&bErr, &eErr);

		LOG(em_ERR), "[%s] -> ACLError : Common = %d, Board = %d, Extend = %d", my_sThreadName, cErr, bErr, eErr);
		LOM(em_ERR), "[%s] -> ACLError : Common = %d, Board = %d, Extend = %d", my_sThreadName, cErr, bErr, eErr);
	}

	// �O��
	OnAddInfo(DIV_ID_ERROR_GRABBER_FRAME_LOCK_FAILED, frame_num, grabber_index, 0, err);
}


//------------------------------------------
// �t�я��L���[�C���O
// EM_DELIVERY_ID em �敪
// DWORD nFrameNum �t���[��No
// DWORD set �J�����Z�b�g (0�I���W��)
// DWORD ang �J�����p�x (0�I���W��)
// DWORD st �X�e�[�^�X���
//------------------------------------------
void AvalACL2Manager::OnAddInfo(EM_DELIVERY_ID em, DWORD nFrameNum, DWORD set, DWORD ang, DWORD st)
{
	//// �f�[�^����
	DELIVERY_INFO* pDeli = new DELIVERY_INFO;
	pDeli->em		 = em;
	pDeli->nFrameNum = nFrameNum;
	pDeli->nCamset	 = set;
	pDeli->nAngle	 = ang;
	pDeli->status	 = st;

	//// �L���[�C���O
	if( ! myque_Info.AddToTailFreeCheck(pDeli) ) {
		LOG(em_ERR), "[%s] �X���b�h�L���[�󂫖���!", my_sThreadName);
		delete pDeli;
	}
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �T���v���摜����n

//------------------------------------------
// �A���r�b�g�}�b�v��`
// char const* cPath �r�b�g�}�b�v�t�@�C���̃t�H�[�}�b�g(�`���́A�J�����p�x(0�I���W��),�A��) (��:test_Ang0_No0.bmp)
// int nNoFrom �A�Ԃ̊J�n�ʒu
// int nNoTo �A�Ԃ̏I���ʒu
// int nCycleTimer �摜�捞���� [ms]
//------------------------------------------
bool AvalACL2Manager::OpenBmp(char const* cPathFormat, int nNoFrom, int nNoTo, int nCycleTimer)
{
	//// �f�[�^�Z�b�g
	my_sSmpPathFormat	 = cPathFormat;
	my_nSmpIntervalNo[0] = nNoFrom;
	my_nSmpIntervalNo[1] = nNoTo;
	my_nSmpCycleTimer	 = nCycleTimer;


	//// �I�[�v���`�F�b�N
	// �r�b�g�}�b�v�̏ꍇ�́A���ۂ̃I�[�v���͖���摜�捞���ɍs���B
	// �����ł́A�t�@�C�������邩���x�̃`�F�b�N���s��
	CString				sPath;

	for(int jj=0; jj<my_nGrabber; jj++) {
		for(int kk=my_nSmpIntervalNo[0]; kk<=my_nSmpIntervalNo[1]; kk++) {

			if( SMPPATHFROMAT_NOM == my_nSmpPathFormatMode ) {

				// �t�@�C���p�X�̐���
				sPath.Format(my_sSmpPathFormat, jj, kk);
				int fh = _open( sPath, _O_RDONLY ); 
				if(-1 == fh ) {
					LOG(em_ERR), "[%s] BitMap�t�@�C��������܂���B(%s)", my_sThreadName, sPath);
					return false;
				}
				_close(fh);

			} else if( SMPPATHFROMAT_CHN == my_nSmpPathFormatMode ) {

				for(int ll=0; ll<my_nChannel; ll++) {

					// �t�@�C���p�X�̐���
					sPath.Format(my_sSmpPathFormat, jj, ll, kk);
					int fh = _open( sPath, _O_RDONLY ); 
					if(-1 == fh ) {
						LOG(em_ERR), "[%s] BitMap�t�@�C��������܂���B(%s)", my_sThreadName, sPath);
						return false;
					}
					_close(fh);
				}
			}
		}
	}

	//// ���̑��K�v�ȃf�[�^
	my_nSmpNowNo		 = nNoFrom;
	my_emGrabMode		 = GrabberManager::GRAB_MODE_BMP;
	my_nBaseBufNum = 1;
//	InitInfo(3968, 4096, 1);
	//InitInfo(4096, 4096, 1);
	//my_bThreadLoop = true;
	//StartLock(THREAD_PRIORITY_TIME_CRITICAL);
	return TRUE;

	return true;

}

//------------------------------------------
// �A���r�b�g�}�b�v���
//------------------------------------------
void AvalACL2Manager::CloseBmp()
{
	//// �����͂������ǁA���̂Ƃ��뉽�����Ȃ�����
}

//------------------------------------------
// AVI��`
// char const* cPath AVI�t�@�C���̃t�H�[�}�b�g(�`���́A�J�����p�x(0�I���W��)) (��:test_Set0_Ang0.avi)
// int nCycleTimer �摜�捞���� [ms]
//------------------------------------------
bool AvalACL2Manager::OpenAvi(char const* cPathFormat, int nCycleTimer)
{
	//// �f�[�^�Z�b�g
	my_sSmpPathFormat	 = cPathFormat;
	my_nSmpCycleTimer	 = nCycleTimer;

	//// AVI�t�@�C���̏���
	CString				sPath;
	bool				ret;

	for(int jj=0; jj<my_nGrabber; jj++) {

		if( SMPPATHFROMAT_NOM == my_nSmpPathFormatMode ) {
			// ����		
			mycls_Aiv[0][jj] = new KizuAviManager();
			// �t�@�C���p�X�̐���
			sPath.Format(my_sSmpPathFormat, jj);
			// AVI�t�@�C���I�[�v��
			ret = mycls_Aiv[0][jj]->OpenAVIFile(sPath); 
			if( ! ret) {
				LOG(em_ERR), "[%s] AVI�t�@�C��������܂���B(%s)", my_sThreadName, sPath);
				return false;
			}

		} else if( SMPPATHFROMAT_CHN == my_nSmpPathFormatMode ) {
			for(int ll=0; ll<my_nChannel; ll++) {
				// ����		
				mycls_Aiv[ll][jj] = new KizuAviManager();
				// �t�@�C���p�X�̐���
				sPath.Format(my_sSmpPathFormat, jj, ll);
				// AVI�t�@�C���I�[�v��
				ret = mycls_Aiv[ll][jj]->OpenAVIFile(sPath); 
				if( ! ret) {
					LOG(em_ERR), "[%s] AVI�t�@�C��������܂���B(%s)", my_sThreadName, sPath);
					return false;
				}
			}
		}

		// �ǂݍ���
		my_nSmpIntervalNo[0] = 0;
		my_nSmpIntervalNo[1] = mycls_Aiv[0][jj]->GetFrameCnt();
	}


	//// ���̑��K�v�ȃf�[�^
	my_nSmpNowNo		 = 0;
	my_emGrabMode		 = GrabberManager::GRAB_MODE_AVI;
	return true;
}

//------------------------------------------
// AVI���
//------------------------------------------
void AvalACL2Manager::CloseAvi()
{
	for(int ii=0; ii<MAX_CAMNUM; ii++) {
		for(int jj=0; jj<MAX_COLOR_CHANNEL; jj++) {
			if(NULL == mycls_Aiv[jj][ii]) continue;

			mycls_Aiv[jj][ii]->CloseAVIFile();
			SAFE_DELETE( mycls_Aiv[jj][ii] );
		}
	}
}

//------------------------------------------
// �摜�f�[�^���蓮�Œǉ�
//------------------------------------------
bool AvalACL2Manager::OpenManual()
{
	my_nSmpNowNo		 = 0;
	my_emGrabMode		 = GrabberManager::GRAB_MODE_MAN;
	return true;
}
//------------------------------------------
// �摜�f�[�^���蓮�Œǉ�
//------------------------------------------
void AvalACL2Manager::CloseManual()
{
	//// �����͂������ǁA���̂Ƃ��뉽�����Ȃ�����
}

//------------------------------------------
// �S�J�����Z�b�g�A�J�����p�x�ɑ΂��ăr�b�g�}�b�v�摜���Z�b�g
//------------------------------------------
void AvalACL2Manager::OnBmpFrameGrabbed()
{
	KizuBitmapManager	cls_bmp;
	CString				sPath;
	int					nW;
	int					nH;


	//// �󂫊���擾
	FRAME_DATA* p = myque_pFrame->GetFree(0);				// �Z�}�t�H�[�҂��o�[�W����(�����Ɍ��ʂ�Ԃ�����)
	if( NULL == p ) {
		// ��󂫂Ȃ� (�񕜉\�ȃG���[)
		LOG(em_WAR), "[%s] GRAB�o�b�t�@�I�[�o����(No=%d):�X���b�h���[�v�L���[�󂫖���!", my_sThreadName, my_nFrameNum);
		OnAddInfo(IportLib::DIV_ID_ERROR_NO_IMG_QUEFULL, my_nFrameNum, 0, 0, 0 );
// ���܂�		for(DWORD jj=0; jj<my_nGrabber; jj++) my_nSeqNum[jj] ++;
		my_nFrameNum ++;
		return;
	}

//memset(p->pImgs,0xff,128);//��

	p->cls_Time.PutTimestamp(0);						// ���O�F�ŏ��̃J�����p�x���擾�������� (=��)


	//// Bitmap�t�@�C�����擾
	LOM(em_MSG), "lock");
	for(int jj=0; jj<my_nGrabber; jj++) {
// ���܂�		p->nSeqNum[jj] = my_nSeqNum[jj];
		p->hInfo[jj]   = my_hAddInfo[jj];


		if( SMPPATHFROMAT_NOM == my_nSmpPathFormatMode ) {
			sPath.Format( my_sSmpPathFormat, jj, my_nSmpNowNo );
			cls_bmp.BMPfileToData_Gray( sPath, &nW, &nH, &p->pImgs[ my_nImgSize * (jj*my_nChannel) ] );
			
			// �摜�㉺���]
			KizuAviManager::HantenTB(nW, nH, &p->pImgs[ my_nImgSize * (jj*my_nChannel) ]);
			_ASSERT( nW == my_nFrameW && nH == my_nFrameH );

			// �`�����l���������[�v�B�F���Ή�
			for(int kk=1; kk<my_nChannel; kk++) {
				memcpy( &p->pImgs[ (jj*my_nChannel+kk) * my_nImgSize], &p->pImgs[ my_nImgSize * (jj*my_nChannel) ], my_nImgSize );	
			}

		} else if( SMPPATHFROMAT_CHN == my_nSmpPathFormatMode ) {
			for(int kk=0; kk<my_nChannel; kk++) {

				sPath.Format( my_sSmpPathFormat, jj, kk, my_nSmpNowNo );
				cls_bmp.BMPfileToData_Gray( sPath, &nW, &nH, &p->pImgs[ my_nImgSize * (jj*my_nChannel+kk) ] );
				
				// �摜�㉺���]
				KizuAviManager::HantenTB(nW, nH, &p->pImgs[ my_nImgSize * (jj*my_nChannel+kk)  ]);
				_ASSERT( nW == my_nFrameW && nH == my_nFrameH );
			}
		}	
	}

	p->cls_Time.PutTimestamp();							// ���P�F�S���̃J�����p�x������������ (=��)

	LOM(em_MSG), "->Fno=%d, path=%s", my_nFrameNum, sPath);


	//// �f�[�^�R�s�[
	p->nCamset	= 0;
	p->nLen		= nW*nH;
	p->nNo		= my_nFrameNum;


	for (int ii = 0; ii < my_nGrabber; ii++) {
		p->nSeqNum[ii] = (WORD)m_nSeqNum[ii];		//iportSQNO�Ƃ̌݊������Ƃ邽�߁A16bit�ɂ���
	}
	//// �o�^
	myque_pFrame->AddPool(p); 
	//myque_pFrame->AddFree(p);  // �e�X�g



	//// ���̂���
// ���܂�	for(DWORD jj=0; jj<my_nGrabber; jj++) my_nSeqNum[jj] ++;
	my_nFrameNum ++;
	my_nSmpNowNo ++;
	if( my_nSmpNowNo > my_nSmpIntervalNo[1] ) my_nSmpNowNo = my_nSmpIntervalNo[0];		// ����܂��������ŏ��̉摜���擾
}

//------------------------------------------
// �S�J�����Z�b�g�A�J�����p�x�ɑ΂���AVI�摜���Z�b�g
//------------------------------------------
void AvalACL2Manager::OnAviFrameGrabbed()
{
	bool ret;

	//// �󂫊���擾
	FRAME_DATA* p = myque_pFrame->GetFree(0);				// �Z�}�t�H�[�҂��o�[�W����(�����Ɍ��ʂ�Ԃ�����)
	if( NULL == p ) {
		// ��󂫂Ȃ� (�񕜉\�ȃG���[)
		LOG(em_WAR), "[%s] GRAB�o�b�t�@�I�[�o����(No=%d):�X���b�h���[�v�L���[�󂫖���!", my_sThreadName, my_nFrameNum);
		OnAddInfo(IportLib::DIV_ID_ERROR_NO_IMG_QUEFULL, my_nFrameNum, 0, 0, 0 );
// ���܂�		for(DWORD jj=0; jj<my_nAngleNum; jj++) my_nSeqNum[jj] ++;
		my_nFrameNum ++;
		return;
	}


	p->cls_Time.PutTimestamp(0);						// ���O�F�ŏ��̃J�����p�x���擾�������� (=��)

	
	//// AVI�摜�ǂݍ���
	LOM(em_MSG), "lock");
	for(int jj=0; jj<my_nGrabber; jj++) {
// ���܂�		p->nSeqNum[jj] = my_nSeqNum[jj];
		p->hInfo[jj]   = my_hAddInfo[jj];

		if( SMPPATHFROMAT_NOM == my_nSmpPathFormatMode ) {
			ret = mycls_Aiv[0][0]->LoadFromAVI( my_nSmpNowNo, &p->pImgs[ my_nImgSize * (jj*my_nChannel) ], true );
			if( ! ret ) {
				LOG(em_ERR), "[%s] AVI�t�@�C���ǂݍ��ݎ��s (ang=%d, no=%d)", my_nFrameNum, jj, my_nSmpNowNo);
			}

			// �`�����l���������[�v�B�F���Ή�
			for(int kk=1; kk<my_nChannel; kk++) {
				memcpy( &p->pImgs[ (jj*my_nChannel+kk) * my_nImgSize], &p->pImgs[ my_nImgSize * (jj*my_nChannel) ], my_nImgSize );	
			}
		} else if( SMPPATHFROMAT_CHN == my_nSmpPathFormatMode ) {

			for(int kk=0; kk<my_nChannel; kk++) {
				ret = mycls_Aiv[kk][0]->LoadFromAVI( my_nSmpNowNo, &p->pImgs[ my_nImgSize * (jj*my_nChannel+kk) ], true );
				if( ! ret ) {
					LOG(em_ERR), "[%s] AVI�t�@�C���ǂݍ��ݎ��s (ang=%d, ch=%d, no=%d)", my_nFrameNum, jj, kk, my_nSmpNowNo);
				}
			}
		}

/*//// ----->>>>
		p->pImgs[ 10 +my_nFrameW*0] = 0x0f;
		p->pImgs[ 10 +my_nFrameW*1] = 0x0f;
		p->pImgs[ 10 +my_nFrameW*2] = 0x0f;
		p->pImgs[ 10 +my_nFrameW*3] = 0x0f;
		p->pImgs[ 10 +my_nFrameW*4] = 0x0f;

			CString str;
			str.Format("C:\\test\\a%d_%d_%d.bmp",ii,jj,my_nFrameNum) ;
			KizuBitmapManager k;
			k.DataToBMPfile_Gray(my_nFrameW, my_nFrameH, &p->pImgs[ my_nImgSize * jj ], str); 

		//	std::ofstream   os;
		//	os.open(str, std::ios::binary);
		//	os.write((const char*)&p->pImgs[ my_nImgSize * jj ], my_nImgSize);
		//	os.close();
							
//*/// <<<<-------	
		
	}
	p->cls_Time.PutTimestamp();							// ���P�F�S���̃J�����p�x������������ (=��)

	LOM(em_MSG), "->Fno=%d", my_nFrameNum);


	//// �f�[�^�R�s�[
	p->nCamset	= 0;
	p->nLen		= my_nImgSize;
	p->nNo		= my_nFrameNum;


	//// �o�^
	myque_pFrame->AddPool(p); 
	//myque_pFrame->AddFree(p);  // �e�X�g


	//// ���̂���
// ���܂�	for(DWORD jj=0; jj<my_nGrabber; jj++) my_nSeqNum[jj] ++;
	my_nFrameNum ++;
	my_nSmpNowNo ++;
	if( my_nSmpNowNo >= my_nSmpIntervalNo[1] ) my_nSmpNowNo = my_nSmpIntervalNo[0];		// ����܂��������ŏ��̉摜���擾
}

//------------------------------------------
// 1�J�����Z�b�g���̉摜���擾���āA�X���b�h���[�v�L���[�ɃL���[�C���O
// void** pImgs		�摜(�J�����p�x��)
//					my_nSmpPathFormatMode=1 : [�J�����p�x][�J����CH]�̏��̉摜
// DWORD nBytes �摜�T�C�Y (1�R����)
// DWORD nFrameNum �t���[��No (1�R����)
// DWORD nCamset �J�����Z�b�g(0�I���W��)
// __int64 nFastTime �ŏ��̃J�����p�x���擾��������
//------------------------------------------
bool AvalACL2Manager::OnManualFrameGrabbed(BYTE* pImgs, DWORD nBytes, DWORD nCamset)
{
	//// �󂫊���擾
	FRAME_DATA* p = myque_pFrame->GetFree(0);				// �Z�}�t�H�[�҂��o�[�W����(�����Ɍ��ʂ�Ԃ�����)
	if( NULL == p ) {
		// ��󂫂Ȃ� (�񕜉\�ȃG���[)
		LOG(em_ERR), "[%s] GRAB�o�b�t�@�I�[�o����(set=%d, No=%d):�X���b�h���[�v�L���[�󂫖���!", my_sThreadName, nCamset, my_nFrameNum);
		OnAddInfo(IportLib::DIV_ID_ERROR_NO_IMG_QUEFULL, my_nFrameNum, 0, 0, 0 );
// ���܂�		for(DWORD jj=0; jj<my_nGrabber; jj++) my_nSeqNum[jj] ++;
		my_nFrameNum ++;
		return false;
	}
	p->cls_Time.PutTimestamp(0);						// ���O�F�ŏ��̃J�����p�x���擾�������� (=��)

	// �f�[�^�R�s�[
	p->nCamset	= 0;
	p->nLen		= nBytes;
	p->nNo		= my_nFrameNum;
	for(int ii=0; ii<my_nGrabber; ii++) {
// ���܂�		p->nSeqNum[ii] = my_nSeqNum[ii];
		p->hInfo[ii]   = my_hAddInfo[ii];

		if( SMPPATHFROMAT_NOM == my_nSmpPathFormatMode ) {
			KizuMMX::mem_copy_64xmm(&p->pImgs[(ii*my_nChannel)*nBytes], &pImgs[ii*nBytes], nBytes);
			for(int kk=1; kk<my_nChannel; kk++) {			// �`�����l���������[�v�B�F���Ή�
				memcpy( &p->pImgs[(ii*my_nChannel+kk) * nBytes], &p->pImgs[ii*nBytes], nBytes );	
			}
		} else if( SMPPATHFROMAT_CHN == my_nSmpPathFormatMode ) {
			KizuMMX::mem_copy_64xmm(&p->pImgs[(ii*my_nChannel)*nBytes], &pImgs[my_nChannel*ii*nBytes], nBytes*my_nChannel);
		}
	}
	p->cls_Time.PutTimestamp(1);					// ���P�F�S���̃J�����p�x������������ (=��)

	//// �o�^
	myque_pFrame->AddPool(p); 

	//// ���̂���
// ���܂�	for(DWORD jj=0; jj<my_nGrabber; jj++) my_nSeqNum[jj] ++;
	my_nFrameNum ++;
	my_nSmpNowNo ++;
	return true;
}