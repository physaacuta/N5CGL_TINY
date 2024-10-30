#include "StdAfx.h"
#include "IportManager.h"

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
//------------------------------------------
IportManager::IportManager(CString sMyManagerName) :
// ���O
GrabberManager(GRAB_TYPE_IPORT, sMyManagerName),

my_nCamsetNum(0),
my_nAngleNum(0),
my_nFrameNum(0),
my_nResendTotal(0),
my_nLostTotal(0),
my_nQueueCount(0),
my_nMaxQueueCount(0),
my_nImgSize(0),
my_nFrameW(0),
my_nFrameH(0),
my_nChannel(1),

my_reset_module(true),	// �����l�́ASpyderGigE ���[�h
my_pDummyBuf(NULL),

my_bImgTurn_LR(false),

my_nSmpPathFormatMode(0),
my_hThreadReady(INVALID_HANDLE_VALUE),
my_hThreadGoStop(INVALID_HANDLE_VALUE),
my_bThreadLoop(false)
{


	for(int ii=0; ii<MAX_IPORT_CAMNUM; ii++) {
		my_nSeqNum[ii]		= 0;
		my_nSkipCnt[ii]		= 0;

		for(int jj=0; jj<MAX_IPORT_CHANNEL; jj++ ) mycls_Aiv[jj][ii] = NULL;
	}

	my_evSmpImgTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	my_hThreadGoStop = CreateEvent(NULL, TRUE, FALSE, NULL);		// �}�j���A��
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
IportManager::~IportManager(void)
{
	SAFE_DELETE_HANDLE(my_evSmpImgTimer);
	SAFE_DELETE_HANDLE(my_hThreadGoStop);
}




// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �T���v���摜����n

//------------------------------------------
// �A���r�b�g�}�b�v��`
// char const* cPath �r�b�g�}�b�v�t�@�C���̃t�H�[�}�b�g(�`���́A�J�����Z�b�g(0�I���W��),�J�����p�x(0�I���W��),�A��) (��:test_Set0_Ang0_No0.bmp)
// int nNoFrom �A�Ԃ̊J�n�ʒu
// int nNoTo �A�Ԃ̏I���ʒu
// int nCycleTimer �摜�捞���� [ms]
//------------------------------------------
bool IportManager::OpenBmp(char const* cPathFormat, int nNoFrom, int nNoTo, int nCycleTimer)
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
	for(DWORD ii=0; ii<my_nCamsetNum; ii++) {
		for(DWORD jj=0; jj<my_nAngleNum; jj++) {
			for(int kk=my_nSmpIntervalNo[0]; kk<=my_nSmpIntervalNo[1]; kk++) {

				if( SMPPATHFROMAT_NOM == my_nSmpPathFormatMode ) {

					// �t�@�C���p�X�̐���
					sPath.Format(my_sSmpPathFormat, ii, jj, kk);
					int fh = _open( sPath, _O_RDONLY ); 
					if(-1 == fh ) {
						LOG(em_ERR), "[%s] BitMap�t�@�C��������܂���B(%s)", my_sThreadName, sPath);
						return false;
					}
					_close(fh);

				} else if( SMPPATHFROMAT_CHN == my_nSmpPathFormatMode ) {

					for(int ll=0; ll<my_nChannel; ll++) {

						// �t�@�C���p�X�̐���
						sPath.Format(my_sSmpPathFormat, ii, jj, ll, kk);
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
	}

	//// ���̑��K�v�ȃf�[�^
	my_nSmpNowNo		 = nNoFrom;
	my_emGrabMode		 = IportManager::GRAB_MODE_BMP;
	return true;
}

//------------------------------------------
// �A���r�b�g�}�b�v���
//------------------------------------------
void IportManager::CloseBmp()
{
	//// �����͂������ǁA���̂Ƃ��뉽�����Ȃ�����
}

//------------------------------------------
// AVI��`
// char const* cPath AVI�t�@�C���̃t�H�[�}�b�g(�`���́A�J�����Z�b�g(0�I���W��),�J�����p�x(0�I���W��)) (��:test_Set0_Ang0.avi)
// int nCycleTimer �摜�捞���� [ms]
//------------------------------------------
bool IportManager::OpenAvi(char const* cPathFormat, int nCycleTimer)
{
	//// �f�[�^�Z�b�g
	my_sSmpPathFormat	 = cPathFormat;
	my_nSmpCycleTimer	 = nCycleTimer;

	//// AVI�t�@�C���̏���
	CString				sPath;
	DWORD				ch;
	bool				ret;

	for(DWORD ii=0; ii<my_nCamsetNum; ii++) {
		for(DWORD jj=0; jj<my_nAngleNum; jj++) {
			ch = GetCamIndex(ii, jj);
		

			if( SMPPATHFROMAT_NOM == my_nSmpPathFormatMode ) {
				// ����		
				mycls_Aiv[0][ch] = new KizuAviManager();
				// �t�@�C���p�X�̐���
				sPath.Format(my_sSmpPathFormat, ii, jj);
				// AVI�t�@�C���I�[�v��
				ret = mycls_Aiv[0][ch]->OpenAVIFile(sPath); 
				if( ! ret) {
					LOG(em_ERR), "[%s] AVI�t�@�C��������܂���B(%s)", my_sThreadName, sPath);
					return false;
				}

			} else if( SMPPATHFROMAT_CHN == my_nSmpPathFormatMode ) {
				for(int ll=0; ll<my_nChannel; ll++) {
					// ����		
					mycls_Aiv[ll][ch] = new KizuAviManager();
					// �t�@�C���p�X�̐���
					sPath.Format(my_sSmpPathFormat, ii, jj, ll);
					// AVI�t�@�C���I�[�v��
					ret = mycls_Aiv[ll][ch]->OpenAVIFile(sPath); 
					if( ! ret) {
						LOG(em_ERR), "[%s] AVI�t�@�C��������܂���B(%s)", my_sThreadName, sPath);
						return false;
					}
				}
			}

			// �ǂݍ���
			my_nSmpIntervalNo[0] = 0;
			my_nSmpIntervalNo[1] = mycls_Aiv[0][ch]->GetFrameCnt();
		}
	}

	//// ���̑��K�v�ȃf�[�^
	my_nSmpNowNo		 = 0;
	my_emGrabMode		 = IportManager::GRAB_MODE_AVI;
	return true;
}

//------------------------------------------
// AVI���
//------------------------------------------
void IportManager::CloseAvi()
{
	for(int ii=0; ii<MAX_IPORT_CAMNUM; ii++) {
		for(int jj=0; jj<MAX_IPORT_CHANNEL; jj++) {
			if(NULL == mycls_Aiv[jj][ii]) continue;

			mycls_Aiv[jj][ii]->CloseAVIFile();
			SAFE_DELETE( mycls_Aiv[jj][ii] );
		}
	}
}

//------------------------------------------
// �摜�f�[�^���蓮�Œǉ�
//------------------------------------------
bool IportManager::OpenManual()
{
	my_nSmpNowNo		 = 0;
	my_emGrabMode		 = IportManager::GRAB_MODE_MAN;
	return true;
}
//------------------------------------------
// �摜�f�[�^���蓮�Œǉ�
//------------------------------------------
void IportManager::CloseManual()
{
	//// �����͂������ǁA���̂Ƃ��뉽�����Ȃ�����
}


//------------------------------------------
// �S�J�����Z�b�g�A�J�����p�x�ɑ΂��ăr�b�g�}�b�v�摜���Z�b�g
//------------------------------------------
void IportManager::OnBmpFrameGrabbed()
{
	KizuBitmapManager	cls_bmp;
	CString				sPath;
	int					nW;
	int					nH;

	for(DWORD ii=0; ii<my_nCamsetNum; ii++) {
		//// �󂫊���擾
		FRAME_DATA* p = myque_pFrame->GetFree(0);				// �Z�}�t�H�[�҂��o�[�W����(�����Ɍ��ʂ�Ԃ�����)
		if( NULL == p ) {
			// ��󂫂Ȃ� (�񕜉\�ȃG���[)
			LOG(em_WAR), "[%s] GRAB�o�b�t�@�I�[�o����(set=%d, No=%d):�X���b�h���[�v�L���[�󂫖���!", my_sThreadName, ii, my_nFrameNum);
			OnAddInfo(IportLib::DIV_ID_ERROR_NO_IMG_QUEFULL, my_nFrameNum, ii, 0, 0 );
			for(DWORD jj=0; jj<my_nAngleNum; jj++) my_nSeqNum[jj] ++;
			my_nFrameNum ++;
			return;
		}

//memset(p->pImgs,0xff,128);//��

		p->cls_Time.PutTimestamp(0);						// ���O�F�ŏ��̃J�����p�x���擾�������� (=��)


		//// Bitmap�t�@�C�����擾
		LOM(em_MSG), "lock (set=%d)", ii);
		for(DWORD jj=0; jj<my_nAngleNum; jj++) {
			p->nSeqNum[jj] = my_nSeqNum[jj];
			p->hInfo[jj]   = my_hAddInfo[GetCamIndex(ii, jj)];


			if( SMPPATHFROMAT_NOM == my_nSmpPathFormatMode ) {
				sPath.Format( my_sSmpPathFormat, ii, jj, my_nSmpNowNo );
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

					sPath.Format( my_sSmpPathFormat, ii, jj, kk, my_nSmpNowNo );
					cls_bmp.BMPfileToData_Gray( sPath, &nW, &nH, &p->pImgs[ my_nImgSize * (jj*my_nChannel+kk) ] );
					
					// �摜�㉺���]
					KizuAviManager::HantenTB(nW, nH, &p->pImgs[ my_nImgSize * (jj*my_nChannel+kk)  ]);
					_ASSERT( nW == my_nFrameW && nH == my_nFrameH );
				}
			}

			//nW = my_nFrameW;
			//nH = my_nFrameH;
			//memcpy( &p->pImgs[ii*my_nImgSize], my_pDummyBuf, my_nImgSize );	

	//nW = my_nFrameW;
	//nH = my_nFrameH; 
//memset(&p->pImgs[ my_nFrameW*0 ],0xff,128);
//memset(&p->pImgs[ my_nFrameW*1 ],0xff,128);
//memset(&p->pImgs[ my_nFrameW*2 ],0xff,128);
//memset(&p->pImgs[ my_nFrameW*3 ],0xff,128);
//memset(&p->pImgs[ my_nFrameW*4 ],0xff,128);
			
		}

		p->cls_Time.PutTimestamp();							// ���P�F�S���̃J�����p�x������������ (=��)

		LOM(em_MSG), "->Fno=%d, path=%s", my_nFrameNum, sPath);


		//// �f�[�^�R�s�[
		p->nCamset	= ii;
		p->nLen		= nW*nH;
		p->nNo		= my_nFrameNum;

		//// �o�^
		myque_pFrame->AddPool(p); 
		//myque_pFrame->AddFree(p);  // �e�X�g
	}

	//// ���̂���
	for(DWORD jj=0; jj<my_nAngleNum; jj++) my_nSeqNum[jj] ++;
	my_nFrameNum ++;
	my_nSmpNowNo ++;
	if( my_nSmpNowNo > my_nSmpIntervalNo[1] ) my_nSmpNowNo = my_nSmpIntervalNo[0];		// ����܂��������ŏ��̉摜���擾
}

//------------------------------------------
// �S�J�����Z�b�g�A�J�����p�x�ɑ΂���AVI�摜���Z�b�g
//------------------------------------------
void IportManager::OnAviFrameGrabbed()
{
	bool ret;

	for(DWORD ii=0; ii<my_nCamsetNum; ii++) {
		//// �󂫊���擾
		FRAME_DATA* p = myque_pFrame->GetFree(0);				// �Z�}�t�H�[�҂��o�[�W����(�����Ɍ��ʂ�Ԃ�����)
		if( NULL == p ) {
			// ��󂫂Ȃ� (�񕜉\�ȃG���[)
			LOG(em_WAR), "[%s] GRAB�o�b�t�@�I�[�o����(set=%d, No=%d):�X���b�h���[�v�L���[�󂫖���!", my_sThreadName, ii, my_nFrameNum);
			OnAddInfo(IportLib::DIV_ID_ERROR_NO_IMG_QUEFULL, my_nFrameNum, ii, 0, 0 );
			for(DWORD jj=0; jj<my_nAngleNum; jj++) my_nSeqNum[jj] ++;
			my_nFrameNum ++;
			return;
		}


		p->cls_Time.PutTimestamp(0);						// ���O�F�ŏ��̃J�����p�x���擾�������� (=��)

		
		//// AVI�摜�ǂݍ���
		LOM(em_MSG), "lock (set=%d)", ii);
		for(DWORD jj=0; jj<my_nAngleNum; jj++) {
			p->nSeqNum[jj] = my_nSeqNum[jj];
			p->hInfo[jj]   = my_hAddInfo[GetCamIndex(ii, jj)];

			if( SMPPATHFROMAT_NOM == my_nSmpPathFormatMode ) {
				ret = mycls_Aiv[0][ii]->LoadFromAVI( my_nSmpNowNo, &p->pImgs[ my_nImgSize * (jj*my_nChannel) ], true );
				if( ! ret ) {
					LOG(em_ERR), "[%s] AVI�t�@�C���ǂݍ��ݎ��s (set=%d, ang=%d, no=%d)", my_nFrameNum, ii, jj, my_nSmpNowNo);
				}

				// �`�����l���������[�v�B�F���Ή�
				for(int kk=1; kk<my_nChannel; kk++) {
					memcpy( &p->pImgs[ (jj*my_nChannel+kk) * my_nImgSize], &p->pImgs[ my_nImgSize * (jj*my_nChannel) ], my_nImgSize );	
				}
			} else if( SMPPATHFROMAT_CHN == my_nSmpPathFormatMode ) {

				for(int kk=0; kk<my_nChannel; kk++) {
					ret = mycls_Aiv[kk][ii]->LoadFromAVI( my_nSmpNowNo, &p->pImgs[ my_nImgSize * (jj*my_nChannel+kk) ], true );
					if( ! ret ) {
						LOG(em_ERR), "[%s] AVI�t�@�C���ǂݍ��ݎ��s (set=%d, ang=%d, ch=%d, no=%d)", my_nFrameNum, ii, jj, kk, my_nSmpNowNo);
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
		p->nCamset	= ii;
		p->nLen		= my_nImgSize;
		p->nNo		= my_nFrameNum;


		//// �o�^
		myque_pFrame->AddPool(p); 
		//myque_pFrame->AddFree(p);  // �e�X�g
	}

	//// ���̂���
	for(DWORD jj=0; jj<my_nAngleNum; jj++) my_nSeqNum[jj] ++;
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
bool IportManager::OnManualFrameGrabbed(BYTE* pImgs, DWORD nBytes, DWORD nCamset)
{
	//// �󂫊���擾
	FRAME_DATA* p = myque_pFrame->GetFree(0);				// �Z�}�t�H�[�҂��o�[�W����(�����Ɍ��ʂ�Ԃ�����)
	if( NULL == p ) {
		// ��󂫂Ȃ� (�񕜉\�ȃG���[)
		LOG(em_ERR), "[%s] GRAB�o�b�t�@�I�[�o����(set=%d, No=%d):�X���b�h���[�v�L���[�󂫖���!", my_sThreadName, nCamset, my_nFrameNum);
		OnAddInfo(IportLib::DIV_ID_ERROR_NO_IMG_QUEFULL, my_nFrameNum, nCamset, 0, 0 );
		//my_nSeqNum	 ++;
		for(DWORD jj=0; jj<my_nAngleNum; jj++) my_nSeqNum[jj] ++;
		my_nFrameNum ++;
		return false;
	}
	p->cls_Time.PutTimestamp(0);						// ���O�F�ŏ��̃J�����p�x���擾�������� (=��)

	// �f�[�^�R�s�[
	p->nCamset	= nCamset;
	p->nLen		= nBytes;
	p->nNo		= my_nFrameNum;
	for(DWORD ii=0; ii<my_nAngleNum; ii++) {
		p->nSeqNum[ii] = my_nSeqNum[ii];
		p->hInfo[ii]   = my_hAddInfo[GetCamIndex(nCamset, ii)];

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
	//my_nSeqNum	 ++;
	for(DWORD jj=0; jj<my_nAngleNum; jj++) my_nSeqNum[jj] ++;
	my_nFrameNum ++;
	my_nSmpNowNo ++;
	return true;
}

//------------------------------------------
// �t�я��L���[�C���O
// EM_DELIVERY_ID em �敪
// DWORD nFrameNum �t���[��No
// DWORD set �J�����Z�b�g (0�I���W��)
// DWORD ang �J�����p�x (0�I���W��)
// DWORD st �X�e�[�^�X���
//------------------------------------------
void IportManager::OnAddInfo(EM_DELIVERY_ID em, DWORD nFrameNum, DWORD set, DWORD ang, DWORD st)
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
