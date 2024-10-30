#include "StdAfx.h"
#include "CullManager.h"

#include "..\..\Library\KizuLib\Class\KizuBitmapManager.h"	// �f�o�b�N�p

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
CullManager::CullManager(void) :
ThreadManager("CullMgr")
{
	memset(g_nTime_Rpi, 0x00, sizeof(g_nTime_Rpi));

	// ����
	mcls_Time.InitAve();

}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
CullManager::~CullManager(void)
{
}


//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int CullManager::ThreadFirst()
{
	//// ���O����

	//// �C�x���g�ݒ�
	HANDLE hArray[] = { gque_Deli.g_evSem 
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int CullManager::ThreadLast()
{
	// �������~
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void CullManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_QUE = 0,				// QUE�˗�
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_QUE: 					// QUE�˗�
		{
			DELI* pDeli = gque_Deli.GetFromHead();
			Exec(pDeli);
			_aligned_free(pDeli);
			//delete pDeli;
		}
		return;

	
//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

//------------------------------------------
// ����
//------------------------------------------
void CullManager::Exec(DELI* pDeli)
{
	ParamManager::P_COMMON_MAST const& prm = mcls_pPrmMgr->GetCommonMast();

	// ���摜��S���肩�瑗�邽�߁A�]�������ł͊Ԃɍ���Ȃ��B�摜�����Ŏ��{���Ă���悤�ȉ�]���i�`���Ƃ���
	if ( NULL == mcls_pSockRpi || ! mcls_pSockRpi->IsConnect()) return;


	//// ���Ԍv��
	mcls_Time.CounterStart();

	mcls_Time.PutTimestamp(MC_TIME_RPI_S);


	// �L���[���o��
	HT_RPI_FDATA* pRpiH = NULL;
	int nCnt = 3;												// ���g���C�񐔁B���܂葽���ƕ��Q�������̂ŁA�P�t���[���Ԋu���ʂ�
	do {
		pRpiH = (HT_RPI_FDATA*)mque_pSockRpi->GetFree(0);		// �Z�}�t�H�[�҂��o�[�W����(�����Ɍ��ʂ�Ԃ�����)
		if (NULL != pRpiH) break;

		LOG(em_WAR), "[%s] �t���[�����M�o�b�t�@�L���[�t�� <%d> [%d]", my_sThreadName, pDeli->nCamId, pDeli->nFrameNo);
		Sleep(30);
		nCnt--;
	} while (nCnt);


	// �f�[�^�Z�b�g
	if (NULL == pRpiH) {
		LOG(em_ERR), "[%s] �t���[�����M�o�b�t�@�L���[�t�� <%d> [%d]", my_sThreadName, pDeli->nCamId, pDeli->nFrameNo );
		syslog(SYSNO_QUEFULL_WAR, "�t���[�����M <%d> [%d]", pDeli->nCamId, pDeli->nFrameNo);
	} else {
	
		//------------------------------------
		//// �x�[�X�w�b�_�[������
		//		�g���܂킵�L���[�Ȃ̂ŁA�g��Ȃ��Ă��K���f�[�^�Z�b�g���邱��
		memcpy( pRpiH->base_head.code, T_CODE_DEFECT, sizeof(pRpiH->base_head.code) );
		memcpy( pRpiH->base_head.pname, getmypcname(), sizeof(pRpiH->base_head.pname) );
		pRpiH->base_head.subinf = (short)(pDeli->nCamId) % NUM_CAM_POS;;

		//// �f�[�^�Z�b�g
		pRpiH->nCamId = pDeli->nCamId;
		pRpiH->nFrameNo = pDeli->nFrameNo;
		// �G�b�W
		for(int ii=0; ii<2; ii++) {
			pRpiH->dEdgePos[ii] = pDeli->dEdgePos[ii];
		}			
		// �摜�����G���W������̉摜�o�̓T�C�Y�̂܂�
		//pRpiH->nWidth = SIZE_FRAME_IMG_X / prm.nImgCullX;
		//pRpiH->nHeight= SIZE_FRAME_IMG_Y / prm.nImgCullY;
		//pRpiH->nWidth = SIZE_FRAME_IMG_X / prm.nImgCullX;		// �ϊ���T�C�Y
		//pRpiH->nHeight= SIZE_GRAB_IMG_Y / prm.nImgCullY;
		pRpiH->nWidth = pDeli->nStride;							// �摜�����G���W������̏o�̓T�C�Y
		pRpiH->nHeight = pDeli->nHeight;
		pRpiH->nImgCnt = 0;


		//------------------------------------
		// ���L���������f�[�^�Z�b�g
		BYTE* pImg = (BYTE*)pRpiH->base_head.addr[0];		// �摜�|�C���^
		int nSize = pRpiH->nWidth * pRpiH->nHeight;
		int nImgSetSize = 0;								// �t���[���摜�Z�b�g�T�C�Y
		int nImgFromOffset = 0;								// ���o�b�t�@�I�t�Z�b�g

		// �摜�����G���W�����ɂĊԈ��������{���邽�߁A���̂܂܉摜�f�[�^�R�s�[
		//double dXF = pRpiH->nWidth / (double)SIZE_GRAB_IMG_X;			// �k�ڗ�
		//double dYF = 1.0 / (double)prm.nImgCullY;


		for(int ii=0; ii<MAX_IMAGE_CNT; ii++) {
			if( FALSE == prm.nImgSave[ii] ) {
				pRpiH->nImgSize[ii] = 0;
				pRpiH->nImgOffset[ii] = nImgSetSize;
				continue;
			}

			//----------------------
			// �摜����
			if( 0 == pDeli->nFrameSkip ) {
		
				// �摜�����G���W�����ɂĊԈ��������{���邽�߁A���̂܂܉摜�f�[�^�R�s�[
				//////ResizeRaw( &pDeli->pImg[nImgFromOffset],
				//////	SIZE_FRAME_IMG_X, SIZE_FRAME_IMG_Y,			// ���摜�T�C�Y
				//////	0, 0, SIZE_GRAB_IMG_X, SIZE_GRAB_IMG_Y,		// ���摜��ROI
				//////	pRpiH->nWidth, pRpiH->nHeight,				// �ϊ���摜�T�C�Y
				//////	dXF, dYF,									// �k�ڗ�
				//////	&pImg[nImgSetSize],
				//////	0);											// ���[�h
				memcpy( &pImg[ nImgSetSize ], &pDeli->pImg[nImgFromOffset], nSize );
			} else {
				// �_�~�[�摜
				memset( &pImg[ nImgSetSize ], 0x00, nSize );
				LOG(em_WAR), "[%s] <%d>[%d] Rpi���摜 �_�~�[�t���[�����M", my_sThreadName, pRpiH->nCamId, pRpiH->nFrameNo);
			}

			// ��n��
			pRpiH->nImgSize[ii] = nSize;
			pRpiH->nImgOffset[ii] = nImgSetSize;
			nImgSetSize += nSize;
			nImgFromOffset += pDeli->nImgSize[ii];
			pRpiH->nImgCnt++;



			//// ����
			//for(int jj=0; jj<200; jj++) {
			//	memset( &pImg[ pImg[ pRpiH->nImgOffset[ii] ]  + 2048*jj], 0xff, 2048);
			//}

//#ifdef SOT_IMG_OUTPUT
//// BMP
if(true) {
CString s;
//s.Format("to_raw_%d_%d.bmp", pRpiH->nFCNo, ii);
s.Format("to_raw_%d.bmp", ii);
KizuBitmapManager kb;
kb.DataToBMPfile_Gray(pRpiH->nWidth, pRpiH->nHeight, &pImg[ pRpiH->nImgOffset[ii] ], s);
}
//#endif
		}		

		////LOG(em_MSG), "[%s] �S�����M�˗� <cam=%d> [%d]", my_sThreadName,	pRpiH->nCamId, pRpiH->nFrameNo );
		LOGCULL(em_MSG), "[%s] �S�����M�˗� <cam=%d> [%d]", my_sThreadName,	pRpiH->nCamId, pRpiH->nFrameNo );



		//------------------------------------
		// �`�����M

		//// �`���� (�f�[�^��������ꍇ�́A���L�������ɃZ�b�g���ꂽ���������Z����)
		pRpiH->base_head.len = sizeof(HT_RPI_FDATA) + nImgSetSize;
		pRpiH->base_head_addr.len_head = sizeof(HT_RPI_FDATA);
		pRpiH->base_head_addr.len_addr[0] = nImgSetSize;
		mque_pSockRpi->AddPool(pRpiH);								// ���M

		
	}

	mcls_Time.PutTimestamp(MC_TIME_RPI_E);		

	//// ����
	mcls_Time.CounterEnd();

	// ���Ԋ֌W
	g_nTime_Rpi[0]		= (int)mcls_Time.GetTimestamp(MC_TIME_RPI_E, MC_TIME_RPI_S); 
	g_nTime_Rpi[1]		= max(g_nTime_Rpi[0], g_nTime_Rpi[1]); 

}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���S�t�ы@�\

// IPP�֐����g�����ߖ����I�ɃC���N���[�h

#ifndef __IPPDEFS_H__
#include "ippdefs.h"
#endif
#ifndef __IPPCORE_H__
#include "ippcore.h"
#endif
#ifndef __IPPS_H__
#include "ipps.h"
#endif
#ifndef __IPPI_H__
#include "ippi.h"
#endif

//------------------------------------------
// RAW�摜 �Ԉ���
// BYTE* const pSrcRaw	���͉摜	
// long nSrcWidth		���͕�pixel
// long nSrcHeight		���͍�pixel
// long nCullX			�Ԉ����� X  2���w�肵����A1024��512  �ɂȂ�
// long nCullY			
// BYTE* pDstRaw		�o�͉摜
// int mode				�掿���[�h 0:����  1:���掿
//------------------------------------------
bool CullManager::ResizeRaw(BYTE* const pSrcRaw, long nSrcWidth, long nSrcHeight,
	long nCullX, long nCullY, BYTE* pDstRaw, int mode)
{
	long nDstWidth = nSrcWidth / nCullX;
	long nDstHeight = nSrcHeight / nCullY;

	return ResizeRaw(pSrcRaw, nSrcWidth, nSrcHeight, pDstRaw, nDstWidth, nDstHeight, mode);
}

bool CullManager::ResizeRaw(BYTE* const pSrcRaw, long nSrcWidth, long nSrcHeight,
	BYTE* pDstRaw, long nDstWidth, long nDstHeight, int mode)
{
	const IppiSize	src_size = { nSrcWidth, nSrcHeight };
	const IppiSize	dst_size = { nDstWidth, nDstHeight };
	const IppiRect	src_roi = { 0, 0, nSrcWidth, nSrcHeight };		// ROI=�͈�
//	const IppiRect	dst_roi	 = { 0, 0, nDstWidth, nDstHeight};		// ROI=�͈�

	const double xf = nDstWidth / (double)nSrcWidth;
	const double yf = nDstHeight / (double)nSrcHeight;

	// ���[�h
	int interp;
	if (0 == mode) {
		// ����
		interp = IPPI_INTER_LINEAR;
	}
	else {
		// ���掿
		interp = IPPI_INTER_SUPER;
	}
	IppStatus s = ippiResize_8u_C1R(pSrcRaw, src_size, nSrcWidth, src_roi,
		pDstRaw, nDstWidth, dst_size, xf, yf, interp);

	return (ippStsNoErr == s ? true : false);
}

//
// dXF 1/2 �ł���� 0.5 ���Z�b�g
bool CullManager::ResizeRaw(BYTE* const pSrcRaw, 
	int nSrcWidth, int nSrcHeight,
	int nSrcRoiX, int nSrcRoiY, int nSrcRoiW, int nSrcRoiH,
	int nDstWidth, int nDstHeight,
	double dXF, double dYF,
	BYTE* pDstRaw, int mode)
{
	const IppiSize	src_size = { nSrcWidth, nSrcHeight };
	const IppiSize	dst_size = { nDstWidth, nDstHeight };
	const IppiRect	src_roi = { nSrcRoiX, nSrcRoiY, nSrcRoiW, nSrcRoiH };		// ROI=�͈�

	const double xf = dXF;
	const double yf = dYF;

	// ���[�h
	int interp;
	if (0 == mode) {
		// ����
		interp = IPPI_INTER_LINEAR;
	}
	else {
		// ���掿
		interp = IPPI_INTER_SUPER;
	}
	IppStatus s = ippiResize_8u_C1R(pSrcRaw, src_size, nSrcWidth, src_roi,
		pDstRaw, nDstWidth, dst_size, xf, yf, interp);

	return (ippStsNoErr == s ? true : false);

}