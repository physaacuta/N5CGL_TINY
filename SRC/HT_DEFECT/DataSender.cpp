#include "StdAfx.h"
#include "DataSender.h"

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
DataSender::DataSender(void) :
mcls_pStatus(NULL),
mcls_pData(NULL),
mcls_pSock(NULL),
mcls_pCull(NULL),
m_bSendHls(false),
m_nJpegQuality(80),
m_bFrameSend(true),
ThreadManager("DataSender")
{
#ifdef SOT_TEST
	m_emKindHt = DIV_HANTEI_HT;
	m_nCamPair = 1;
#endif

	// ������^�C�}�[
	m_evTimeHls = CreateWaitableTimer(NULL, FALSE, NULL);

	ClearFrameNo();

	// ���O
	mcls_pLogSock = new LogFileManager("SOCK", NULL, 50, 2);
	LOGSOCK(em_MSG), "[%s] �N��", my_sThreadName);
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
DataSender::~DataSender(void)
{
	// ������^�C�}�[
	CancelWaitableTimer(m_evTimeHls);
	CloseHandle(m_evTimeHls);

	// ���O
	LOGSOCK(em_MSG), "[%s] �I��", my_sThreadName);
	if (NULL != mcls_pLogSock) {
		delete mcls_pLogSock;
		mcls_pLogSock = NULL;
	}
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int DataSender::ThreadFirst()
{
	//// �C�x���g�ݒ�
	for(int ii=0; ii<HT_CONNECT_NUM; ii++) {
		AddEventNum(1, &mcls_pSock[ii]->gque_SendBack.g_evSem );
	}
	AddEventNum(1, &mcls_pData->gque_Deli.g_evSem );
	AddEventNum(1, &m_evTimeHls);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int DataSender::ThreadLast()
{
	CancelWaitableTimer(m_evTimeHls);
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void DataSender::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_NEW_DATA_SEND_BACK = 0,								// ��L�̃f�[�^�������ɑ��M����������ʒm
			EV_NEW_DATA = (EV_NEW_DATA_SEND_BACK + HT_CONNECT_NUM),	// �t���[����񂪋��L�������ɃZ�b�g���ꂽ��ʒm
			EV_HLS = EV_NEW_DATA + 1								// ������ʒm

	};

	//-----------------------------------------------------------------------------------------------
	if( EV_NEW_DATA_SEND_BACK <= nEventNo && EV_NEW_DATA > nEventNo) {		// ��L�̃f�[�^�������ɑ��M����������ʒm

		NewDataSendBack(nEventNo-EV_NEW_DATA_SEND_BACK); 

	//-----------------------------------------------------------------------------------------------
	} else if (EV_NEW_DATA == nEventNo) {									// �t���[����񂪋��L�������ɃZ�b�g���ꂽ��ʒm
		if(true) {
			BYTE * pNewData = mcls_pData->gque_Deli.GetFromHead();
			NewDataExec(pNewData);						// �f�[�^�p�C�v��� 1�t���[����������
			SAFE_DELETE_ARRAY(pNewData);
		}

	//-----------------------------------------------------------------------------------------------
	} else if (EV_HLS == nEventNo) {										// ������ʒm
		SendHls();

	} else {
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}

}

//------------------------------------------
// FrameNo�֌W������
//------------------------------------------
void DataSender::ClearFrameNo()
{
	for(int ii=0; ii<MAX_HT1_CAMSET; ii++) {
		m_nFrameNo[ii]		= -1;
		m_nRecvFrameNo[ii]	= -1;
	}
	m_nDspFrameNo			= -1;
	m_nDspRecvFrameNo		= -1;

	// ����
	mcls_Time.InitAve();
	mcls_TimeCycle.InitCycle();

	memset(g_nTime_All, 0x00, sizeof(g_nTime_All));
	memset(g_nTime_Rpi, 0x00, sizeof(g_nTime_Rpi));
	memset(g_nTime_To, 0x00, sizeof(g_nTime_To));
	memset(g_nTime_ToFrame, 0x00, sizeof(g_nTime_ToFrame));
}

//------------------------------------------
// �w���V�[
//------------------------------------------
void DataSender::StartHls()
{
	__int64 ts = 1000 * -10000;			// 1�~���b��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evTimeHls, (LARGE_INTEGER *)&ts, ST_SEND_INTERVAL, NULL, NULL, FALSE);

}
void DataSender::StopHls()
{
	CancelWaitableTimer(m_evTimeHls);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���C������


//------------------------------------------
// �f�[�^�p�C�v��� 1�t���[���������� (���ꂪ����̃��C���@�\)
// BYTE* pNewData �p�C�v���M�f�[�^
//------------------------------------------
void DataSender::NewDataExec(BYTE* pNewData)
{
	ParamManager::P_COMMON_MAST const& prm = mcls_pPrmMgr->GetCommonMast();

	//nc_smem_pipe_data* pPipe = (nc_smem_pipe_data*) pNewData;
	NCL_SMEM_PIPE_DATA* pPipe = (NCL_SMEM_PIPE_DATA*) pNewData;
	int nFrameSkip = 0;			// 0:�ȊO  NG


	//// ���Ԍv��
	mcls_Time.CounterStart();

	mcls_Time.PutTimestamp(MC_TIME_ALL_S);

	// �����J�����Z�b�g�̏ꍇ�́A�ŏ��̃J�����Z�b�g�̂Ƃ��Ɏ��Ԍv��
	int camset = mcls_pStatus->GetCamPairItemCamset( m_nCamPair, 0, m_emKindHt);
	if( pPipe->camid == camset ) {
		mcls_TimeCycle.CounterCycle();
	}

	// �S���C���X�^���X�̉��Ԗڂ�����
	int nRpiIdx = 0;
	for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
		int camset = mcls_pStatus->GetCamPairItemCamset( m_nCamPair, ii, m_emKindHt);
		if( pPipe->camid == camset ) {
			nRpiIdx = ii;
			break;
		}
	}


	////////////////////////////////////
	// �������ȊO�� �������Ȃ��̂ŁA�I�t�Z�b�g�̕ԋp�̂�
#ifndef SOT_TEST
	if( ! mcls_pStatus->IsRunning() ) {
		// �I�t�Z�b�g��Ԃ��ׂ����̂̂�
		if (pPipe->smem_size > 0) {
			mcls_pData->SendBack(pPipe->smem_offset);
		}
		return;
	}
#endif	

	// ���W�b�N�p���������͏������Ȃ��̂ŁA�I�t�Z�b�g�̕ԋp�̂�
	if (DIV_HANTEI_PR == m_emKindHt)
	{
		// �I�t�Z�b�g��Ԃ��ׂ����̂̂�
		if (pPipe->smem_size > 0) {
			mcls_pData->SendBack(pPipe->smem_offset);
		}
		return;
	}

	////////////////////////////////////
	// �������`�F�N
	DWORD	frame_num = pPipe->frame_num;

	// �I�t�Z�b�g�l �ُ�
	if( 0 > pPipe->smem_size ) {
		LOG(em_ERR), "[%s] ���L�������I�t�Z�b�g�l�G���[ [fNo=%d camid=%d offset=%d size=%d]", my_sThreadName, frame_num, pPipe->camid, pPipe->smem_offset, pPipe->smem_size );
		syslog(109, "[fNo=%d camid=%d offset=%d size=%d]", frame_num, pPipe->camid, pPipe->smem_offset, pPipe->smem_size);
		nFrameSkip = 9;

	// ���L�������󂫖���
	} else if( 0 == pPipe->smem_size && NCL_SMEMPIPE_SMEMFULL == pPipe->smem_offset  ) {
		LOG(em_ERR), "[%s] [NCL_SMEMPIPE_SMEMFULL] ���L�������󂫖��� [fNo=%d camid=%d offset=%d size=%d]", my_sThreadName, frame_num, pPipe->camid, pPipe->smem_offset, pPipe->smem_size );
		syslog(112, "[NCL_SMEMPIPE_SMEMFULL����(���L�������󂫖���):fNo=%d camid=%d offset=%d size=%d]", frame_num, pPipe->camid, pPipe->smem_offset, pPipe->smem_size );
		nFrameSkip = NCL_SMEMPIPE_SMEMFULL;


	// �s��
	} else if( 0 == pPipe->smem_size && 0 > pPipe->smem_offset) {
		LOG(em_ERR), "[%s] [NP_SMEMPIPE_???] �p�C�v�f�[�^�s�� [fNo=%d camid=%d offset=%d size=%d]", my_sThreadName, frame_num, pPipe->camid, pPipe->smem_offset, pPipe->smem_size );
		syslog(110, "[�p�C�v�f�[�^�s��:fNo=%d camid=%d offset=%d size=%d]", frame_num, pPipe->camid, pPipe->smem_offset, pPipe->smem_size );
		nFrameSkip = 1;
	}


	//// �t���[��No�������`�F�b�N
	int nIndex = pPipe->camid - mcls_pStatus->GetCamPairItemCamset(m_nCamPair, 0, m_emKindHt);
	if(m_nRecvFrameNo[nIndex] +1 != pPipe->frame_num) {
		LOG(em_ERR), "[%s] �摜�����^�X�N������ �t���[��No��M���� [camset=%d now=%d pre=%d]", my_sThreadName, pPipe->camid, pPipe->frame_num, m_nRecvFrameNo[nIndex]);
		syslog(113, "[�摜�����^�X�N������ �t���[��No��M���� camset=%d now=%d pre=%d]", pPipe->camid, pPipe->frame_num, m_nRecvFrameNo[nIndex]);
	}
	m_nRecvFrameNo[nIndex] = pPipe->frame_num;
	m_nDspRecvFrameNo = pPipe->frame_num;

	



	////////////////////////////////////
	// �ǂ��g�����̂�����
	//nc_smem_frame_header* pSmemH = (nc_smem_frame_header*)(mcls_pData->GetMapPointer() + pPipe->offset);	// ���L�������̐擪�A�h���X + �I�t�Z�b�g�l = �{�t���[���ł̋��L�������̈ʒu
    NCL_SMEM_FRAME_HEADER* pSmemH = (NCL_SMEM_FRAME_HEADER*)(mcls_pData->GetMapPointer() + pPipe->smem_offset);	// ���L�������̐擪�A�h���X + �I�t�Z�b�g�l = �{�t���[���ł̋��L�������̈ʒu
	BYTE* pSmemAry = (BYTE*)pSmemH;					// �g���₷���悤�ɁB���̃t���[���̐擪�|�C���^
	if( 0 == nFrameSkip ) {
		if( pSmemH->camid != pPipe->camid || pSmemH->frame_num != pPipe->frame_num ){
			LOG(em_ERR), "<SMEM-PIPE Mismatch>, cam %d:%d, frame %d:%d",
				pSmemH->camid , pPipe->camid , pSmemH->frame_num ,pPipe->frame_num);
		}
	}


	////////////////////////////////////
	// ��\�G�b�W�ʒu �Z�o
	float	dEdgeAvePos[2];									// ���σG�b�W�ʒu�B�s���̏ꍇ 0�B�i�J�������W�n)mm�P��
	float	dEdgeMaxPos[2];									// �ő�G�b�W�ʒu�B�s���̏ꍇ 0�B�i�J�������W�n)mm�P��
	float	dEdgeMinPos[2];									// �ŏ��G�b�W�ʒu�B�s���̏ꍇ 0�B�i�J�������W�n)mm�P��
	memset(dEdgeAvePos, 0x00, sizeof(dEdgeAvePos));
	if( 0 == nFrameSkip ) {
		float* pEdge = (float*)&pSmemAry[ pSmemH->edge_offset ];

		// ���ώZ�o
		for(int jj=0; jj<2; jj++) {
			double dSum = 0;
			long nCnt = 0;
			for(int kk=0; kk<SIZE_FRAME_IMG_Y; kk++) {
				//int idx = (2*kk) + jj;
				int idx = (SIZE_FRAME_IMG_Y*jj) + kk;
//LOG(em_MSG), "[%d] v=%.1f", idx, pEdge[idx]);
				//if(2>=kk) LOG(em_MSG), "[%d] v=%.1f", idx, pEdge[idx]);
				dSum += pEdge[idx]; nCnt++;
			}

			// ���ώZ�o
			if(0 == nCnt)	dEdgeAvePos[jj] = 0;
			else			dEdgeAvePos[jj] = (float)(dSum/nCnt);
//LOG(em_MSG), "EdgeAvePos[%d]=%.1f (%.1f/%d)", jj, dEdgeAvePos[jj], dSum, nCnt);
		}

		// �ő�,�ŏ��Z�o
		if( true ) {
			long nCnt = 0;
			double dMax[2] = { 0, 0 };
			double dMin[2] = {COIL_WID_MAX, COIL_WID_MAX};
			for(int kk=0; kk<SIZE_FRAME_IMG_Y; kk++) {
				int idx1 = (SIZE_FRAME_IMG_Y*0) + kk;
				int idx2 = (SIZE_FRAME_IMG_Y*1) + kk;
				if (abs(dMax[0])+abs(dMax[1]) < abs(pEdge[idx1])+abs(pEdge[idx2])) {
					dMax[0] = pEdge[idx1];
					dMax[1] = pEdge[idx2];
				}
				if (abs(dMin[0])+ abs(dMin[1]) > abs(pEdge[idx1])+abs(pEdge[idx2])) {
					dMin[0] = pEdge[idx1];
					dMin[1] = pEdge[idx2];
				}
				nCnt++;
			}

			// �ő�, �ŏ��Z�o
			if(0 == nCnt) {
				for(int jj=0; jj<2; jj++) {
					dEdgeMaxPos[jj] = 0;
					dEdgeMinPos[jj] = 0;
				}
			}
			else {
				for(int jj=0; jj<2; jj++) {
					dEdgeMaxPos[jj] = (float)dMax[jj];
					dEdgeMinPos[jj] = (float)dMin[jj];
				}
			}
		}

	}


	////////////////////////////////////
	// �S���摜�����֑��M

//	if( NULL != mcls_pCull) {
	if(DIV_HANTEI_HT == m_emKindHt) {
		mcls_Time.PutTimestamp(MC_TIME_RPI_S);

		NCL_FRAME_IMAGE*	pF;	// ���L�������̉摜���擪�ʒu
		CullManager::DELI* pDeli;

		if (0 == nFrameSkip && pSmemH->frame_offset >0) {
			pF = (NCL_FRAME_IMAGE*)&pSmemAry[pSmemH->frame_offset];	// ���L�������̉摜���擪�ʒu

			// �����T�C�Y����
			int nDeliSize = sizeof(CullManager::DELI) - sizeof(BYTE);
			for (int ii = 0; ii<MAX_IMAGE_CNT; ii++) {
				if (TRUE == prm.nImgSave[ii]) {
					//nDeliSize += SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y;
					nDeliSize += pF->stride * pF->height;
				}
			}

			// ����
			BYTE* pWk = (BYTE*)_aligned_malloc(nDeliSize, 16);//new BYTE[nDeliSize];
			pDeli = (CullManager::DELI*)pWk;

			// �Z�b�g
			pDeli->nFrameSkip = nFrameSkip;
			pDeli->nCamId = (pPipe->camid - 1) % NUM_CAM_POS;
			pDeli->nFrameNo = pPipe->frame_num;
			for (int ii = 0; ii<2; ii++) {
				pDeli->dEdgePos[ii] = dEdgeAvePos[ii];
			}

			pDeli->nWidth = pF->width;
			pDeli->nHeight = pF->height;
			pDeli->nStride = pF->stride;
			pDeli->nChannel = pF->channel;

			int nOffset = 0;
			for (int ii = 0; ii<MAX_IMAGE_CNT; ii++) {
				if (TRUE == prm.nImgSave[ii]) {
#if 0	// �t���[���摜���G���W���Ԉ����Ή��O
					pDeli->nImgSize[ii] = SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y;
					//memcpy( &pDeli->pImg[nOffset], 
					//	&pSmemAry[(pSmemH->frame_offset + (SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y*ii))],
					//	SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y);
					KizuMMX::mem_copy_64(&pSmemAry[(pSmemH->frame_offset + (SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y*ii))], SIZE_FRAME_IMG_X,
						&pDeli->pImg[nOffset], SIZE_FRAME_IMG_X,
						SIZE_FRAME_IMG_X, SIZE_FRAME_IMG_Y);
					//KizuMMX::mem_copy_64xmm(&pSmemAry[(pSmemH->frame_offset + (SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y*ii))], SIZE_FRAME_IMG_X, 
					//	&pDeli->pImg[nOffset], SIZE_FRAME_IMG_X,
					//	SIZE_FRAME_IMG_X, SIZE_FRAME_IMG_Y);
					//KizuMMX::mem_copy_64xmm_su(&pDeli->pImg[nOffset], &pSmemAry[(pSmemH->frame_offset + (SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y*ii))],
					//	SIZE_FRAME_IMG_X * SIZE_FRAME_IMG_Y);
					nOffset += SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y;
#else
					pDeli->nImgSize[ii] = pF->stride * pF->height;
					//KizuMMX::mem_copy_64(&pF->image[pF->stride*pF->height*ii], pF->stride, &pDeli->pImg[nOffset], pF->stride, pF->stride, pF->height);
					KizuMMX::mem_copy_64xmm(&pF->image[pF->stride*pF->height*ii], pF->stride, &pDeli->pImg[nOffset], pF->stride, pF->stride, pF->height);
					nOffset += pF->stride * pF->height;
#endif
				}
				else {
					pDeli->nImgSize[ii] = 0;
				}
			}
		}
		else {																	// �����T�C�Y����
			int nDeliSize = sizeof(CullManager::DELI) - sizeof(BYTE);
			for (int ii = 0; ii<MAX_IMAGE_CNT; ii++) {
				if (TRUE == prm.nImgSave[ii]) {
					nDeliSize += (SIZE_FRAME_IMG_X / prm.nImgCullX)*(SIZE_FRAME_IMG_Y / prm.nImgCullY);
				}
			}

			// ����
			BYTE* pWk = (BYTE*)_aligned_malloc(nDeliSize, 16);//new BYTE[nDeliSize];
			pDeli = (CullManager::DELI*)pWk;

			// �Z�b�g
			pDeli->nFrameSkip = nFrameSkip;
			pDeli->nCamId = (pPipe->camid - 1) % NUM_CAM_POS;
			pDeli->nFrameNo = pPipe->frame_num;
			for (int ii = 0; ii<2; ii++) {
				pDeli->dEdgePos[ii] = dEdgeAvePos[ii];
			}

			pDeli->nWidth = SIZE_FRAME_IMG_X / prm.nImgCullX;
			pDeli->nHeight = SIZE_FRAME_IMG_Y / prm.nImgCullY;
			pDeli->nStride = SIZE_FRAME_IMG_X / prm.nImgCullX;
			pDeli->nChannel = MAX_IMAGE_CNT;

			int nOffset = 0;
			for (int ii = 0; ii<MAX_IMAGE_CNT; ii++) {
				if (TRUE == prm.nImgSave[ii]) {
					pDeli->nImgSize[ii] = pDeli->nStride * pDeli->nHeight;
					//KizuMMX::mem_copy_64xmm(&pF->image[pF->stride*pF->height*ii], pF->stride, &pDeli->pImg[nOffset], pF->stride, pF->stride, pF->height);
					nOffset += pDeli->nStride * pDeli->nHeight;
				}
				else {
					pDeli->nImgSize[ii] = 0;
				}
			}
		}


		// �˗�
		if( ! mcls_pCull[nRpiIdx]->gque_Deli.AddToTailFreeCheck(pDeli) ) {
			delete pDeli;
			LOG(em_ERR), "[%s] RAW�Ԉ����˗� �L���[�t��!!", my_sThreadName);
		}
		mcls_Time.PutTimestamp(MC_TIME_RPI_E);		
	}


/*
	//   ���摜��S���肩�瑗�邽�߁A�]�������ł͊Ԃɍ���Ȃ��B�摜�����Ŏ��{���Ă���悤�ȉ�]���i�`���Ƃ���
	if ( NULL != mcls_pSockRpi[nRpiIdx] && mcls_pSockRpi[nRpiIdx]->IsConnect()) {
		mcls_Time.PutTimestamp(MC_TIME_RPI_S);


		// �L���[���o��
		HT_RPI_FDATA* pRpiH = NULL;
		int nCnt = 3;												// ���g���C�񐔁B���܂葽���ƕ��Q�������̂ŁA�P�t���[���Ԋu���ʂ�
		do {
			pRpiH = (HT_RPI_FDATA*)mque_pSockRpi[nRpiIdx]->GetFree(0);		// �Z�}�t�H�[�҂��o�[�W����(�����Ɍ��ʂ�Ԃ�����)
			if (NULL != pRpiH) break;

			LOG(em_WAR), "[%s] �t���[�����M�o�b�t�@�L���[�t�� [%d]", my_sThreadName, m_nRecvFrameNo[nIndex]);
			Sleep(30);
			nCnt--;
		} while (nCnt);

		// �f�[�^�Z�b�g
		if (NULL == pRpiH) {
			LOG(em_ERR), "[%s] �t���[�����M�o�b�t�@�L���[�t�� [%d]", my_sThreadName, m_nRecvFrameNo[nIndex] );
			syslog(SYSNO_QUEFULL_WAR, "�t���[�����M [%d]", m_nRecvFrameNo[nIndex]);
		} else {
		
			//------------------------------------
			//// �x�[�X�w�b�_�[������
			//		�g���܂킵�L���[�Ȃ̂ŁA�g��Ȃ��Ă��K���f�[�^�Z�b�g���邱��
			memcpy( pRpiH->base_head.code, T_CODE_DEFECT, sizeof(pRpiH->base_head.code) );
			memcpy( pRpiH->base_head.pname, getmypcname(), sizeof(pRpiH->base_head.pname) );
			pRpiH->base_head.subinf = 0;			
	
			//// �f�[�^�Z�b�g
			pRpiH->nCamId = (pPipe->camid-1) % NUM_CAM_POS;
			pRpiH->nFrameNo = pPipe->frame_num;
			// �G�b�W
			for(int ii=0; ii<2; ii++) {
				pRpiH->dEdgePos[ii] = dEdgeAvePos[ii];
			}			
			pRpiH->nWidth = SIZE_FRAME_IMG_X / prm.nImgCullX;
			pRpiH->nHeight= SIZE_FRAME_IMG_Y / prm.nImgCullY;
			pRpiH->nImgCnt = 0;
	

			//------------------------------------
			// ���L���������f�[�^�Z�b�g
			BYTE* pImg = (BYTE*)pRpiH->base_head.addr[0];		// �摜�|�C���^
			int nSize = pRpiH->nWidth * pRpiH->nHeight;
			int nImgSetSize = 0;								// �t���[���摜�Z�b�g�T�C�Y


			for(int ii=0; ii<MAX_IMAGE_CNT; ii++) {
				if( FALSE == prm.nImgSave[ii] ) {
					pRpiH->nImgSize[ii] = 0;
					pRpiH->nImgOffset[ii] = nImgSetSize;
					continue;
				}

				//----------------------
				// �摜����
				if( 0 == nFrameSkip ) {
					if( 1 == prm.nImgCullX && 1 == prm.nImgCullY ) {
						// �Ԉ�������
						memcpy(&pImg[nImgSetSize],
							&pSmemAry[(pSmemH->frame_offset + (SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y*ii))],
							nSize);
					} else {
						// �Ԉ�������
						ResizeRaw(&pSmemAry[(pSmemH->frame_offset + (SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y*ii))],
							SIZE_FRAME_IMG_X, SIZE_FRAME_IMG_Y,
							prm.nImgCullX, prm.nImgCullY,
							&pImg[nImgSetSize],
							1);
					}
				} else {
					// �_�~�[�摜
					memset( &pImg[ nImgSetSize ], 0x00, nSize );
					LOG(em_WAR), "[%s] [%d] Rpi���摜 �_�~�[�t���[�����M", my_sThreadName, pRpiH->nFrameNo);
				}

				// ��n��
				pRpiH->nImgSize[ii] = nSize;
				pRpiH->nImgOffset[ii] = nImgSetSize;
				nImgSetSize += nSize;
				pRpiH->nImgCnt++;



				//// ����
				//for(int jj=0; jj<200; jj++) {
				//	memset( &pImg[ pImg[ pRpiH->nImgOffset[ii] ]  + 2048*jj], 0xff, 2048);
				//}

#ifdef SOT_IMG_OUTPUT
//// BMP
if(true) {
	CString s;
	//s.Format("to_raw_%d_%d.bmp", pRpiH->nFCNo, ii);
	s.Format("to_raw_%d.bmp", ii);
	KizuBitmapManager kb;
	kb.DataToBMPfile_Gray(pRpiH->nWidth, pRpiH->nHeight, &pImg[ pRpiH->nImgOffset[ii] ], s);
}
#endif
			}		


			//------------------------------------
			// �`�����M

			//// �`���� (�f�[�^��������ꍇ�́A���L�������ɃZ�b�g���ꂽ���������Z����)
			pRpiH->base_head.len = sizeof(HT_RPI_FDATA) + nImgSetSize;
			pRpiH->base_head_addr.len_head = sizeof(HT_RPI_FDATA);
			pRpiH->base_head_addr.len_addr[0] = nImgSetSize;
			mque_pSockRpi[nRpiIdx]->AddPool(pRpiH);								// ���M
		}

		mcls_Time.PutTimestamp(MC_TIME_RPI_E);		
	}
*/


	////////////////////////////////////
	// �����֑��M
	HT_TO_HEAD* pToH = new HT_TO_HEAD;
	memset(pToH, 0x00, sizeof(HT_TO_HEAD));
//LOGSOCK(em_MSG), "[%d] �w�b�_���FHT_TO_HEAD=%d", pPipe->frame_num, sizeof(HT_TO_HEAD));

	// �������ς����łȂ��A
	// ���ԍ��ڂ��ϑΉ��Ƃ���̂ŁA��U�ő�̈�𖈉� New ����B
	int nToSendSize = 0 ;				// ���ۂ̑��M�T�C�Y
	int nToSetMaxSize = sizeof(HT_TO_DATA) + pPipe->smem_size;		// New�ő�T�C�Y

	//// Jpeg�t���[���摜 ��������t���[���H
	bool bIsDebug_JpegFrame = false;			// �������������������������������������� ���S�e�X�g�p�B���ۂɂ̓��[���X���b�g���Ŕ��f�H�B�ȍ~�̏����̓`�����g�ł��Ă���

	//if(bIsDebug_JpegFrame && pSmemH->frame_offset > 0) {
	//	//nToSetMaxSize += SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y*MAX_IMAGE_CNT;
	//	NCL_FRAME_IMAGE*	pF = (NCL_FRAME_IMAGE*)&pSmemAry[pSmemH->frame_offset];	// ���L�������̉摜���擪�ʒu
	//	nToSetMaxSize += pF->stride * pF->height * MAX_IMAGE_CNT;
	//}

//	LOG(em_INF), "frame%d : ofs %d, smemsz %d, to %d", pPipe->frame_num, pPipe->smem_offset, pPipe->smem_size, nToSetMaxSize);

	if(0 != nFrameSkip) nToSetMaxSize = sizeof(HT_TO_DATA) + sizeof(NCL_SMEM_FRAME_HEADER);

//LOG(em_INF), "[%s] [%d] Jpeg�t���[���摜 Size=%d Skip=%d smem_size=%d", my_sThreadName, pToH->nFrameNo, nToSetMaxSize, nFrameSkip, pPipe->smem_size);


LOGSOCK(em_MSG), "[%d] �f�[�^���F�f�[�^�̈�=%d", pToH->nFrameNo, nToSetMaxSize);
	BYTE* pToAry = new BYTE[nToSetMaxSize];
	HT_TO_DATA* pToD = (HT_TO_DATA*)&pToAry[0];
	memset(pToAry, 0x00, sizeof(HT_TO_DATA));		// �w�b�_�[���̂݁B��͑S���R�s�[����̂ŗǂ�����

	int idx = 0;									// �Жʓ��쎞�� 0�Œ�


	//------------------------------------
	//// �x�[�X�w�b�_�[������
	// �g�����U�N�V�����R�[�h
	memcpy(pToH->base_head.code, T_CODE_DEFECT, sizeof(pToH->base_head.code) );
	memcpy(pToH->base_head.pname, getmypcname(), sizeof(pToH->base_head.pname) );

	// �Ɩ��`���̏ꍇ�́A�_�����ID���Z�b�g (0�n�܂�)
	pToH->base_head.subinf = (short)(pPipe->camid-1) % NUM_CAM_POS;
	pToH->base_head.addr[0] = (long*)&pToAry[0];	// ���� New�����|�C���^���Z�b�g
	pToH->base_head.addr[1] = NULL;
	pToH->base_head.len = sizeof(HT_TO_HEAD);	// �`���� �͈�ԍŌ�
	//if(pPipe->offset > 0) pToH->base_head.len += pPipe->size;


	//------------------------------------
	//// �w�b�_�[������
	pToH->nFrameNo			= pPipe->frame_num;
	pToH->nFrameSkip		= nFrameSkip;
	pToH->nJpegCmp			= 0;				// �����_�ł͖��m��

	pToH->camset			= pPipe->camid;
	pToH->offset			= pPipe->smem_offset;
	pToH->size				= pPipe->smem_size;

	if(0 == nFrameSkip) {
		pToH->nEdgeNg			= STATUS_EDGELOST == pSmemH->status ? 1 : 0;
		pToH->nFrameSt			= pSmemH->status;

		pToH->frame_pos			= (long)pSmemH->front_pos;
		memcpy( pToH->br,		pSmemH->frame_brightness, sizeof(pToH->br) );
	}

	memcpy( pToH->edge_pos,	dEdgeAvePos, sizeof(pToH->edge_pos) );
	memcpy( pToH->edge_pos_max,	dEdgeMaxPos, sizeof(pToH->edge_pos_max) );
	memcpy( pToH->edge_pos_min,	dEdgeMinPos, sizeof(pToH->edge_pos_min) );


	//------------------------------------
	//// �f�[�^������
	// 2023.08.29 ���� SMEMFULL���̋��L�������A�N�Z�X�ᔽ�Ή� --------------------------->>>>>>
	// pToD->defect_num = pSmemH->defect_num;
	// pToD->image_num = pSmemH->image_num;
	if(0 == nFrameSkip)
	{
		pToD->defect_num = pSmemH->defect_num;
		pToD->image_num = pSmemH->image_num;
	}
	else
	{
		pToD->defect_num = 0;
		pToD->image_num = 0;
	}
	// 2023.08.29 ���� SMEMFULL���̋��L�������A�N�Z�X�ᔽ�Ή� <<<<<<---------------------------




	//------------------------------------
	// ���L��������
	// ��{�I��8�̔{���Ƃ���
	nToSendSize += GET_JUST_SIZE(sizeof(HT_TO_DATA), 8);	//8�̔{��
	BYTE*	pSemF = NULL;		// ���L�������Ǎ���
	int		nWriteSize = 0;		// �������݃T�C�Y
//LOGSOCK(em_MSG), "[%d] �f�[�^���FHT_TO_DATA=%d defect_num=%d image_num=%d", pToH->nFrameNo, GET_JUST_SIZE(sizeof(HT_TO_DATA), 8), pToD->defect_num, pToD->image_num);



	//----------
	// NCL_SMEM_FRAME_HEADER
	if( 0 == nFrameSkip ) {
		pSemF = &pSmemAry[0];		// = pSmemH = NCL_SMEM_FRAME_HEADER
		nWriteSize = sizeof(NCL_SMEM_FRAME_HEADER);
		pToD->header_offset = nToSendSize;
		memcpy(&pToAry[nToSendSize], pSemF, nWriteSize);
		nToSendSize += GET_JUST_SIZE(nWriteSize, 8);
//LOGSOCK(em_MSG), "[%d] �f�[�^���FNCL_SMEM_FRAME_HEADER=%d", pToH->nFrameNo, GET_JUST_SIZE(nWriteSize, 8));
	}

	//----------
	// �r���֘A
	if (0 != nFrameSkip || 0 == pSmemH->defect_num) {
		// �r�����̏ꍇ�A�������Ȃ��悤�ɂ���H�H
		pToD->result_offset = 0;
		pToD->attr_offset = 0;
	} else {
		//----------
		//nc_defect_result
		pSemF = &pSmemAry[pSmemH->result_offset];
		nWriteSize = sizeof(NCL_DEFECT_RESULT) * pSmemH->defect_num;
		pToD->result_offset = nToSendSize;
		memcpy(&pToAry[nToSendSize], pSemF, nWriteSize);
		nToSendSize += GET_JUST_SIZE(nWriteSize, 8);
//LOGSOCK(em_MSG), "[%d] �f�[�^���Fnc_defect_result=%d", pToH->nFrameNo, GET_JUST_SIZE(nWriteSize, 8));

		//----------
		//attr
		pSemF = &pSmemAry[pSmemH->attr_offset];
		nWriteSize = sizeof(double)*(NCL_nattr_common+(NCL_nattr_angle*MAX_IMAGE_CNT))*pSmemH->defect_num;
		pToD->attr_offset = nToSendSize;
		memcpy(&pToAry[nToSendSize], pSemF, nWriteSize);
		nToSendSize += GET_JUST_SIZE(nWriteSize, 8);
//LOGSOCK(em_MSG), "[%d] �f�[�^���Fattr=%d", pToH->nFrameNo, GET_JUST_SIZE(nWriteSize, 8));
	}


	//----------
	// �r�摜�֘A
	mcls_Time.PutTimestamp(MC_TIME_JPEG_S);
//	if ( true) {
	if ( 0 != nFrameSkip ||0 == pSmemH->image_num) {
		pToD->image_offset = 0;
	} else {
		pToD->image_offset = nToSendSize;
		{
			int nJpegSize = CreateJpegData(pSmemH, &pToAry[nToSendSize], nToSetMaxSize -nToSendSize );
			nToSendSize += nJpegSize;
			//LOG(em_MSG), "�摜(%d)=%d(%d)", nJpegSize, nToSendSize);
LOGSOCK(em_MSG), "[%d] �f�[�^���F�r�摜�T�C�Y=%d", pToH->nFrameNo, nJpegSize);
		}
	}
	mcls_Time.PutTimestamp(MC_TIME_JPEG_E);


	//----------
	// Jpeg�t���[���摜�֘A
	mcls_Time.PutTimestamp(MC_TIME_JF_S);
	if (bIsDebug_JpegFrame && pSmemH->frame_offset > 0) {
		pToD->frame_jpeg_offset = nToSendSize;
		int nJpegSize = CreateJpegFrame(pSmemH, &pToAry[nToSendSize]);
		nToSendSize += nJpegSize;
LOGSOCK(em_MSG), "[%d] �f�[�^���FJpeg�t���[���摜�T�C�Y=%d", pToH->nFrameNo, nJpegSize);
	} else {
		pToD->frame_jpeg_offset = 0;
	}
	mcls_Time.PutTimestamp(MC_TIME_JF_E);

LOGSOCK(em_MSG), "[%d] �f�[�^���FHT_TO_DATA=%d defect_num=%d image_num=%d header_offset=%d result_offset=%d attr_offset=%d image_offset=%d frame_jpeg_offset=%d",
pToH->nFrameNo,
GET_JUST_SIZE(sizeof(HT_TO_DATA), 8),
pToD->defect_num,
pToD->image_num,
pToD->header_offset,
pToD->result_offset,
pToD->attr_offset,
pToD->image_offset,
pToD->frame_jpeg_offset
);
// �_�~�[�f�[�^
//if (0 == pSmemH->defect_num)
//{
//	LOGSOCK(em_MSG), "[%d] %d ���� %d byte�� 0xFF�Ńp�f�B���O", pToH->nFrameNo, nToSendSize, sizeof(HT_TO_HEAD) - SIZE_SOCK_HEAD);
//	memset(&pToAry[nToSendSize], 0xFF, sizeof(HT_TO_HEAD) - SIZE_SOCK_HEAD);
//	pToAry[nToSendSize + (sizeof(HT_TO_HEAD) - SIZE_SOCK_HEAD - 1)] = 0xfe;
//}

	// ����PC�̃p�t�H�[�}���X�J�E���^�̎��g��
	QueryPerformanceFrequency((LARGE_INTEGER*)&pToH->ht_freq[0]);

	mcls_Time.PutTimestamp(MC_TIME_ALL_E);

	__int64 nWk;
	nWk = mcls_Time.GetRawTimestamp(MC_TIME_ALL_S);
	memcpy(&pToH->ht_timestamp_start, (void*)&nWk, sizeof(__int64));	// �摜�����G���W������t���[����M���̃^�C���X�^���v
	nWk = mcls_Time.GetRawTimestamp(MC_TIME_ALL_E);
	memcpy(&pToH->ht_timestamp_end, (void*)&nWk, sizeof(__int64));		// HT��TO���M���̃^�C���X�^���v


	////��	
	//LOG(em_MSG), "[%s] [%d] ���M�J�n <cam=%d> �r=%d �摜=%d DataSize=%dbyte �؏oJpeg���k=%dsm F/Cut����=%d/%dmm Edge=%d/%d", my_sThreadName,
	//	pToH->nFrameNo, pToH->camset, pSmemH->defect_num, pSmemH->image_num, nToSendSize,
	//	(int)mcls_Time.GetTimestamp(MC_TIME_JPEG_E, MC_TIME_JPEG_S), 
	//	(int)pSmemH->front_pos, (int)pSmemH->shear_cut_pos, 
	//	(int)dEdgeAvePos[0], (int)dEdgeAvePos[1]
	//	);

	LOGSOCK(em_MSG), "[%d] ���M�J�n <cam=%d> Size=%.1fKb (Que=%d) All:%dms Rpi:%dms Jpeg:%dms", pToH->nFrameNo, pToH->camset, (double)nToSendSize / 1024.0, mcls_pSock[idx]->gque_Deli.GetCount(), (int)mcls_Time.GetTimestamp(MC_TIME_ALL_E, MC_TIME_ALL_S), (int)mcls_Time.GetTimestamp(MC_TIME_RPI_E, MC_TIME_RPI_S), (int)mcls_Time.GetTimestamp(MC_TIME_JPEG_E, MC_TIME_JPEG_S));



	//------------------------------------
	//// �T�C�Y���f
	pToH->base_head.len = sizeof(HT_TO_HEAD) + nToSendSize;		// �w�b�_�[�{�f�[�^��
LOGSOCK(em_MSG), "[%d] �@�@�@�v=%d(%d)", pToH->nFrameNo, pToH->base_head.len, nToSendSize);

	// �t���[�����𓝊��Ǘ��ɑ��M����
	// �����W�b�N�p���������͑��M�}��
	if (m_bFrameSend)
	{
		//// �����֑��M
		// �X���b�h�L���[�ɓo�^
		if (mcls_pSock[idx]->IsConnect() && mcls_pSock[idx]->gque_Deli.GetFreeCount() > 0) {
			mcls_pSock[idx]->gque_Deli.AddToTail((SOCK_F_HEAD*)pToH);
		}
		else {
			// �󂫖���
			//mcls_pData->SendBack(pToH->offset);		// ���Ŗ������ɕԂ��̂ł����ł̕ԋp����
			delete[] pToAry;
			delete pToH;

			if (mcls_pSock[idx]->IsConnect()) {
				LOG(em_ERR), "[%s] �������M�L���[ �L���[�t��!!", my_sThreadName);
				syslog(SYSNO_QUEFULL_WAR, "[�������M]");
			}
		}
	}
	else
	{
		// ���M�����ɔj��
		delete[] pToAry;
		delete pToH;
	}

	//// �I�t�Z�b�g�l�̕ԋp�Ώ�  
	// ������͋��L���������璼�ڑ��M����Ȃ��̂ŁA�����ł����ԋp
	if(NCL_SMEMPIPE_SMEMFULL != pPipe->smem_offset ) {
		mcls_pData->SendBack(pPipe->smem_offset);
	}


	//// ����
	mcls_Time.CounterEnd();

	g_nTime_All[0]		= (int)mcls_Time.GetTimestamp(MC_TIME_ALL_E, MC_TIME_ALL_S);
	g_nTime_Rpi[0]		= (int)mcls_Time.GetTimestamp(MC_TIME_RPI_E, MC_TIME_RPI_S);
	g_nTime_To[0]		= (int)mcls_Time.GetTimestamp(MC_TIME_JPEG_E, MC_TIME_JPEG_S);
	g_nTime_ToFrame[0]	= (int)mcls_Time.GetTimestamp(MC_TIME_JF_E, MC_TIME_JF_S); 

	g_nTime_All[1]		= max(g_nTime_All[0], g_nTime_All[1]);
	g_nTime_Rpi[1]		= max(g_nTime_Rpi[0], g_nTime_Rpi[1]);
	g_nTime_To[1]		= max(g_nTime_To[0], g_nTime_To[1]);
	g_nTime_ToFrame[1]	= max(g_nTime_ToFrame[0], g_nTime_ToFrame[1]);
}

//------------------------------------------
// �����
//------------------------------------------
void DataSender::SendHls()
{
	HT_TO_HEAD_HLS* pToH = new HT_TO_HEAD_HLS;
	memset(pToH, 0x00, sizeof(HT_TO_HEAD_HLS));

	//------------------------------------
	// �w�b�_
	memcpy( pToH->base_head.code, T_CODE_HELCY, sizeof(pToH->base_head.code) );
	memcpy( pToH->base_head.pname, getmypcname(), sizeof(pToH->base_head.pname) );
	pToH->base_head.subinf = 0;
	pToH->base_head.addr[0] = NULL;
	pToH->base_head.addr[1] = NULL;


	//------------------------------------
	// �f�[�^
	pToH->nEngStatSet		= mcls_pStatus->IsEngStatNew();
	ENG_STAT				EngStatWk;
	mcls_pStatus->GetEngStat(&EngStatWk);
	memcpy( &pToH->EngStat, &EngStatWk, sizeof(ENG_STAT) );

#ifdef DEBUG_LOG_HT_HEALTHY
	for (int ii = 0; ii < 2; ii++)
	{
		LOG(em_FIL), "[%s] �G���W����� ���聨�����w���V�[ [ID:%d][���x:%.3f][�I��:%.3f %.3f %.3f][�Q�C��:%.3f %.3f %.3f][�d��:%.3f]",
			my_sThreadName,
			pToH->EngStat.camid[ii],
			pToH->EngStat.temp[ii],
			pToH->EngStat.exptime[0][ii],
			pToH->EngStat.exptime[1][ii],
			pToH->EngStat.exptime[2][ii],
			pToH->EngStat.gain[0][ii],
			pToH->EngStat.gain[1][ii],
			pToH->EngStat.gain[2][ii],
			pToH->EngStat.volt[ii]);
	}
#endif

	int idx = 0;
	if( ! mcls_pSock[idx]->gque_Deli.AddToTailFreeCheck( (SOCK_F_HEAD*)pToH, 0) ) {
		LOG(em_ERR), "[%s] ���聨������ ��������M�L���[�t��", my_sThreadName);
		delete pToH;
	}
}

//------------------------------------------
// NewDataExec �� New�������M�f�[�^�̊J�� ����� ���L�������̃I�t�Z�b�g�l�J��
// int idx �ǂ����̃\�P�b�g (0�I���W��)
//------------------------------------------
void DataSender::NewDataSendBack(int idx)
{
	HT_TO_HEAD* pData = (HT_TO_HEAD*)mcls_pSock[idx]->gque_SendBack.GetFromHead();
		
//��	LOG(em_MSG), "[%s] ����[%d]", my_sThreadName, pData->nFrameNo);
// ����
	if (pData->camset <= 0 || pData->camset > MAX_CAMSET) {
		LOG(em_ERR), "[%s] ���聨������ �ԋp�pNewDataSendBack �ُ� [camset=%d fno=%d]", my_sThreadName, pData->camset, pData->nFrameNo);
		return;
	}

	//// �t���[��No�A�����`�F�b�N
	int nIndex = pData->camset - mcls_pStatus->GetCamPairItemCamset(m_nCamPair, 0, m_emKindHt);
	if(m_nFrameNo[nIndex] +1 != pData->nFrameNo) {
		LOG(em_ERR), "[%s] ���聨������ �t���[��No���M���� [camset=%d now=%d pre=%d]", my_sThreadName, pData->camset, pData->nFrameNo, m_nFrameNo[nIndex]);
		syslog(113, "[���聨������ �t���[��No���M���� camset=%d now=%d pre=%d]", pData->camset, pData->nFrameNo, m_nFrameNo[nIndex]);
	}
	LOGSOCK(em_MSG), "[%d] ���M���� <cam=%d> (Que=%d) Jpeg���k����:%dms", pData->nFrameNo, pData->camset, mcls_pSock[idx]->gque_Deli.GetCount(), g_nTime_To[0]);


	//// ���M���������t���[��No���Z�b�g
	m_nFrameNo[nIndex] = pData->nFrameNo;
	m_nDspFrameNo = pData->nFrameNo;


	//// �K���ԋp�A�I�t�Z�b�g�l�̕ԋp�Ώ�
	////mcls_pData->SendBack( pData->offset );

	
	//// �����֑��M�f�[�^���J��
	SAFE_DELETE( pData->base_head.addr[0] );
	SAFE_DELETE( pData ); 
}




// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Jpeg�ϊ�
//------------------------------------------
// ����Ǘ���JPEG�ϊ� (�؂�o���摜)
// NCL_SMEM_FRAME_HEADER* pSmem �{�t���[���̋��L�������擪�A�h���X
// BYTE* pImgAddr �������M�p�\���� �؂�o���摜�Z�b�g�擪�A�h���X
// �߂�l�F�؂�o���摜�֘A �g�[�^���T�C�Y
//------------------------------------------
int DataSender::CreateJpegData(NCL_SMEM_FRAME_HEADER* pSmem, BYTE* pImgAddr, int freesize)
{
	BYTE* pSmemAry = (BYTE*)pSmem;						// �g���₷���悤�ɁB���̋��L�������擪�|�C���^


//	mcls_pLog->WriteLock( LogFileManager::em_MSG, "[%s] ���k�J�n �r=%d, �T�C�Y=%d, off_img=%d off_��=%d,%d,%d", 
//		my_sThreadName, pH->image_num, pSmem->size, pH->image_offset, pH->frame_jpg_offset, pH->profile_offset, pH->rawframe_jpg_offset);


	//// ���s�L���`�F�b�N
	int nImgCnt = pSmem->image_num;
	if (0 == nImgCnt) return 0;


	//// BMP��JPEG
	int nTotalBmpSize = 0;						// ���M�Ώۂ̃r�b�g�}�b�v�摜�̈�T�C�Y
	int nTotalJpegSize = 0;
	int nFreeSize = freesize;
	bool bJpegError = false;

	for(int ii=0; ii<nImgCnt; ii++) {
		DEFECT_HEAD_IMAGE*	pT = (DEFECT_HEAD_IMAGE*)&pImgAddr[nTotalJpegSize];		// �����̏������ݗ̈�
		NCL_IMAGE_HEADER*	pF = (NCL_IMAGE_HEADER*)&pSmemAry[pSmem->image_offset + nTotalBmpSize];	// ���L�������̉摜���擪�ʒu

		// �f�[�^�Z�b�g
		pT->nWidth    = pF->width;
		pT->nHeight   = pF->height;
		pT->fShrinkRatio = pF->shrinkratio;

		// Jpeg�摜�ɕϊ�
		int nOffset   = sizeof(DEFECT_HEAD_IMAGE);		// ���\���̓���Jpeg�摜�I�t�Z�b�g
		int nJpegSize = 0;
		for(int jj=0; jj<MAX_IMAGE_CNT; jj++) {

#ifdef SOT_IMG_OUTPUT
//// BMP
if(true) {
	CString s;
	//s.Format("ht%d.bmp", jj);
	//s.Format("to_kiri%d_%d_%d_%d.bmp", pSmem->frame_num, pSmem->camset, ii, jj);
	s.Format("to_kiri%d_%d_%d.bmp", pSmem->camid, ii, jj);
	KizuBitmapManager kb;
	kb.DataToBMPfile_Gray(pT->nWidth, pT->nHeight, &pF->image[ pT->nWidth*pT->nHeight*jj ], s);
}
#endif

			if( nFreeSize - (nTotalJpegSize + nOffset) < (int)(pT->nWidth * pT->nHeight / 2) )
				DebugBreak();

#ifdef JPEGLIB_IPP
			KizuJpegManager::BmpToJpeg(pT->nWidth, pT->nHeight, &pF->image[pT->nWidth*pT->nHeight*jj],
				m_nJpegQuality, &nJpegSize, &pImgAddr[nTotalJpegSize + nOffset]);
#else
			int resp = mcls_KizuJpegManager.BmpToJpeg(pT->nWidth, pT->nHeight, &pF->image[pT->nWidth*pT->nHeight*jj],
				m_nJpegQuality, &nJpegSize, &pImgAddr[nTotalJpegSize + nOffset]);
			// Jpeg�ϊ��G���[
			if (0 != resp && !bJpegError)
			{
				// 1�t���[���̏����ɂ��A����̂݃��O�o��
				LOG(em_ERR), "[%s] Jpeg�摜�ϊ��G���[ [frameno=%d camid=%d defno=%d imgno=%d defnum=%d, imgnum=%d][v:%d h:%d] result=%d", my_sThreadName, pSmem->frame_num, pSmem->camid, ii + 1, jj + 1, pSmem->defect_num, pSmem->image_num, pT->nWidth, pT->nHeight, resp);
				bJpegError = true;
			}
#endif

#ifdef SOT_IMG_OUTPUT
//// JPEG
if(true) {
	CString s;
	//s.Format("ht%d.jpg", jj);
	//s.Format("to_kiri%d_%d_%d_%d.jpg", pSmem->frame_num, pSmem->camset, ii, jj);
	s.Format("to_kiri%d_%d_%d.jpg", pSmem->camid, ii, jj);
	std::ofstream   os;
	os.open(s, std::ios::binary);
	os.write((const char*)&pImgAddr[nTotalJpegSize + nOffset], nJpegSize );
	os.close();
}
#endif

			pT->nSize[jj]		= nJpegSize;
			pT->nImgOffset[jj]	= nOffset;
			
			// ���̂���
			nOffset				+= GET_JUST_SIZE(nJpegSize, 8);		// 8�̔{���ŁA����̔z��T�C�Y (=���̊J�n�ʒu)
		}

		// ���̂���
		pT->nNextImgOffset  =  nOffset;  //(((sizeof(DEFECT_HEAD_IMAGE) + nJpegNext)/8)+1)*8;	
		nTotalJpegSize		+= nOffset;
		//nTotalBmpSize		+= (pF->width*pF->height*MAX_IMAGE_CNT) + sizeof(DWORD)*4;
		nTotalBmpSize		+= (pF->width*pF->height*MAX_IMAGE_CNT) + (sizeof(NCL_IMAGE_HEADER) - 1);
	}


	//// �������Ȃ����������Z�o
	int nSa = nTotalBmpSize - nTotalJpegSize;
	if(0 > nSa) {
		// �̈�j��I�I
		LOG(em_ERR), "[%s]    BMP�摜����JPEG�摜�̕����T�C�Y���傫���I[��=%d] Bmp:%d Jpeg:%d", my_sThreadName, nSa, nTotalBmpSize, nTotalJpegSize);
		LOGSOCK(em_ERR), "[%s]    BMP�摜����JPEG�摜�̕����T�C�Y���傫���I[��=%d] Bmp:%d Jpeg:%d", my_sThreadName, nSa, nTotalBmpSize, nTotalJpegSize);
	} else {
		LOGSOCK(em_MSG), "[%s]    BMP�摜��JPEG�摜���k [��=%dbyte] %d��%d", my_sThreadName, nSa, nTotalBmpSize, nTotalJpegSize);
	}
	return nTotalJpegSize;
}

//------------------------------------------
// ����Ǘ���JPEG�ϊ� (�t���[���摜)
// NCL_SMEM_FRAME_HEADER* pSmem �{�t���[���̋��L�������擪�A�h���X
// BYTE* pImgAddr �������M�p�\���� �؂�o���摜�Z�b�g�擪�A�h���X
// �߂�l�F�؂�o���摜�֘A �g�[�^���T�C�Y
//------------------------------------------
int DataSender::CreateJpegFrame(NCL_SMEM_FRAME_HEADER* pSmem, BYTE* pImgAddr)
{
	BYTE* pSmemAry = (BYTE*)pSmem;						// �g���₷���悤�ɁB���̋��L�������擪�|�C���^


	//// BMP��JPEG


	DEFECT_HEAD_IMAGE*	pT = (DEFECT_HEAD_IMAGE*)&pImgAddr[0];		// �����̏������ݗ̈�
	//BYTE*				pF = (BYTE*)&pSmemAry[pSmem->frame_offset];	// ���L�������̉摜���擪�ʒu
	NCL_FRAME_IMAGE*	pF = (NCL_FRAME_IMAGE*)&pSmemAry[pSmem->frame_offset];	// ���L�������̉摜���擪�ʒu

	// �f�[�^�Z�b�g
	//pT->nWidth    = SIZE_FRAME_IMG_X;
	//pT->nHeight   = SIZE_FRAME_IMG_Y;
	pT->nWidth    = pF->width;
	pT->nHeight   = pF->height;


	// Jpeg�摜�ɕϊ�
	int nOffset   = sizeof(DEFECT_HEAD_IMAGE);		// ���\���̓���Jpeg�摜�I�t�Z�b�g
	int nJpegSize = 0;
	for(int jj=0; jj<(int)pF->channel; jj++) {

#ifdef JPEGLIB_IPP
		//KizuJpegManager::BmpToJpeg(pT->nWidth, pT->nHeight, &pF[ pT->nWidth*pT->nHeight*jj ], m_nJpegQuality, &nJpegSize, &pImgAddr[0 + nOffset]);
		KizuJpegManager::BmpToJpeg(pT->nWidth, pT->nHeight, &pF->image[pF->stride*pF->height*jj], m_nJpegQuality, &nJpegSize, &pImgAddr[0 + nOffset]);
#else
		mcls_KizuJpegManager.BmpToJpeg(pT->nWidth, pT->nHeight, &pF->image[pF->stride*pF->height*jj], m_nJpegQuality, &nJpegSize, &pImgAddr[0 + nOffset]);
#endif

		pT->nSize[jj]		= nJpegSize;
		pT->nImgOffset[jj]	= nOffset;
		
		// ���̂���
		nOffset				+= GET_JUST_SIZE(nJpegSize, 8);		// 8�̔{���ŁA����̔z��T�C�Y (=���̊J�n�ʒu)
	}


	int nTotalJpegSize		= nOffset;
	//int nTotalBmpSize		= (SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y*MAX_IMAGE_CNT);
	int nTotalBmpSize		= (pF->stride*pF->height*pF->channel);




	//// �������Ȃ����������Z�o
	int nSa = nTotalBmpSize - nTotalJpegSize;
	if(0 > nSa) {
		// �̈�j��I�I
		LOG(em_ERR), "[%s]    (�t���[��)BMP�摜����JPEG�摜�̕����T�C�Y���傫���I[��=%d]", my_sThreadName, nSa);
		LOGSOCK(em_ERR), "[%s]    (�t���[��)BMP�摜����JPEG�摜�̕����T�C�Y���傫���I[��=%d]", my_sThreadName, nSa);
	} else {
		LOGSOCK(em_MSG), "[%s]   (�t���[��) BMP�摜��JPEG�摜���k [��=%dbyte] %d��%d", my_sThreadName, nSa, nTotalBmpSize, nTotalJpegSize);
	}
	return nTotalJpegSize;
}













/*** �}�[�N�Ď����u �̋Z�p�����p
//------------------------------------------
// ����Ǘ���JPEG�ϊ�
// HT_TO_HEAD* pData �������M�p �\����
// nc_smem_pipe_data* pSmem ���L�������\����
// int* pSender �V�������M�|�C���^
// �߂�l�F�������Ȃ����T�C�Y
//------------------------------------------
int DataSender::CreateJpegData(HT_TO_HEAD* pData, nc_smem_pipe_data* pSmem, HANDLE* pSendAdder)
{
	int nIndex  = 0;						// pD�̐擪����̃A�N�Z�X�ʒu
	int nWkSize = 0;

	nc_smem_frame_header* pH = (nc_smem_frame_header*)(mcls_pData->GetMapPointer() + pSmem->offset);	// ���L�������̐擪�A�h���X + �I�t�Z�b�g�l = �{�t���[���ł̋��L�������̈ʒu
	BYTE* pHAry = (BYTE*)pH;					// �g���₷���悤�ɁB���̃t���[���̐擪�|�C���^


//	mcls_pLog->WriteLock( LogFileManager::em_MSG, "[%s] ���k�J�n �r=%d, �T�C�Y=%d, off_img=%d off_��=%d,%d,%d", 
//		my_sThreadName, pH->image_num, pSmem->size, pH->image_offset, pH->frame_jpg_offset, pH->profile_offset, pH->rawframe_jpg_offset);


	//// ���s�L���`�F�b�N
	int nImgCnt = pH->image_num;
	if( 0 == nImgCnt ) return 0;
	if( 0 >= pSmem->size ) return 0;				// ���ʂȈӖ�


	//// �̈�m��
	// �� ���̎��_�ł́A���M�T�C�Y�ł͂Ȃ�
	BYTE* pD = new BYTE[pSmem->size];			// �V���������ɑ���f�[�^�G���A

	//// �摜�̈�O�̃G���A���R�s�[
	nWkSize = pH->image_offset;				// BMP�摜�̑O�܂ł͕ύX���� 
	memcpy( &pD[nIndex], &pHAry[0], nWkSize);
	nIndex += nWkSize;


	//// BMP��JPEG
	int nTotalBmpSize = 0;						// ���M�Ώۂ̃r�b�g�}�b�v�摜�̈�T�C�Y
	int nTotalJpegSize = 0;
	//int nMemOffset = pH->image_offset;			// ���L�������̉摜���擪�ʒu
	for(int ii=0; ii<nImgCnt; ii++) {

		DEFECT_HEAD_IMAGE* pT = (DEFECT_HEAD_IMAGE*)&pD[nIndex];						// �����̗̈�
		nc_image_hdr* pF = (nc_image_hdr*)&pHAry[pH->image_offset + nTotalBmpSize];		// ���L�������̉摜���擪�ʒu

		// �f�[�^�Z�b�g
		pT->nWidth    = pF->width;
		pT->nHeight   = pF->height;
		nTotalBmpSize += (pF->width*pF->height*MAX_IMAGE_CNT) + sizeof(DWORD)*2;

		// Jpeg�摜�ɕϊ�
		int nOffset   = sizeof(DEFECT_HEAD_IMAGE);
		int nJpegSize = 0;
		for(int jj=0; jj<MAX_IMAGE_CNT; jj++) {


#ifdef SOT_IMG_OUTPUT
//// BMP
if(true) {
	CString s;
	//s.Format("ht%d.bmp", jj);
	//s.Format("to_kiri%d_%d_%d_%d.bmp", pSmem->frame_num, pSmem->camset, ii, jj);
	s.Format("to_kiri%d_%d_%d.bmp", pSmem->camset, ii, jj);
	KizuBitmapManager kb;
	kb.DataToBMPfile_Gray(pT->nWidth, pT->nHeight, &pF->image[ pT->nWidth*pT->nHeight*jj ], s);
}
#endif


			KizuJpegManager::BmpToJpeg(pT->nWidth, pT->nHeight, &pF->image[ pT->nWidth*pT->nHeight*jj ], m_nJpegQuality, &nJpegSize, &pD[nIndex + nOffset]);

#ifdef SOT_IMG_OUTPUT
//// JPEG
if(true) {
	CString s;
	//s.Format("ht%d.jpg", jj);
	//s.Format("to_kiri%d_%d_%d_%d.jpg", pSmem->frame_num, pSmem->camset, ii, jj);
	s.Format("to_kiri%d_%d_%d.jpg", pSmem->camset, ii, jj);
	std::ofstream   os;
	os.open(s, std::ios::binary);
	os.write((const char*)&pD[nIndex + nOffset], nJpegSize );
	os.close();
}
#endif

			pT->nImgOffset[jj]	= nOffset;
			pT->nSize[jj]		= nJpegSize;
			
			// ���̂���
			nOffset				+= ((nJpegSize/8)+1)*8;
		}

		// ���̂���
		pT->nNextImgOffset  =  nOffset;  //(((sizeof(DEFECT_HEAD_IMAGE) + nJpegNext)/8)+1)*8;	// 8�̔{���ŁA����̔z��T�C�Y (=���̊J�n�ʒu)
		nTotalJpegSize		+= nOffset;
		nIndex				+= nOffset;
	}


	// ���肪�����������p
	int nSa = nTotalBmpSize - nTotalJpegSize;
	int nImgNextPoint = pH->image_offset+nTotalBmpSize;
	int nAfter = pSmem->size - nImgNextPoint;
	if(0 > nSa) {
		// �̈�j��I�I
		LOG(em_ERR), "[%s] BMP�摜����JPEG�摜�̕����T�C�Y���傫���I[��=%d]", my_sThreadName, nSa);

	} else {
		//LOG(em_MSG), "[%s] BMP�摜��JPEG�摜���k [��=%dbyte]", my_sThreadName, nSa);

//		mcls_pLog->WriteLock( LogFileManager::em_ERR, "[%s] ���k�� BMP�摜����JPEG�摜�̕����T�C�Y���傫���I[��=%d]", my_sThreadName, nSa);
	}

//	mcls_pLog->WriteLock( LogFileManager::em_MSG, "[%s] ���k�� [��=%d] next=%d, after=%d, �덷=%d", my_sThreadName,
//		nSa, nImgNextPoint, nAfter, pSmem->size - nImgNextPoint - nAfter);

	memcpy( &pD[nIndex], &pHAry[nImgNextPoint], pSmem->size - nImgNextPoint);

	//// ���M����|�C���^���Z�b�g
	*pSendAdder = pD;
	return nSa;
}
***/

