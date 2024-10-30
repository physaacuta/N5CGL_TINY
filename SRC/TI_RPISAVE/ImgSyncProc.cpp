#include "StdAfx.h"
#include "ImgSyncProc.h"

//�O��F�t���[��NO�����낦��΁A�G�������B
//���t���[��NO�́A�J�����ԂŊ�{�I�ɓ���B100f�����ꂽ�肵�Ȃ�
//�����͏���B

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

#define LOG2( em, ... ) do { \
	if( mcls_detail ){   mcls_detail->Write(KizuLib::LogFileManager:: em, __VA_ARGS__ ); }\
	if(!mcls_detail || \
		KizuLib::LogFileManager::em == KizuLib::LogFileManager::em_ERR ||\
	    KizuLib::LogFileManager::em == KizuLib::LogFileManager::em_WAR) {\
		if(mcls_pLog){       mcls_pLog->Write(KizuLib::LogFileManager:: em, __VA_ARGS__ ); }\
	}\
}while(0)

#define LOG2BOTH( em, ... ) do { \
	if( mcls_detail){ mcls_detail->Write(KizuLib::LogFileManager:: em, __VA_ARGS__ ); }\
	if(mcls_pLog   ){   mcls_pLog->Write(KizuLib::LogFileManager:: em, __VA_ARGS__ ); }\
}while(0)




//------------------------------------------
// �R���X�g���N�^
// int nIns �C���X�^���XNo
//------------------------------------------
ImgSyncProc::ImgSyncProc(int nIns) :
ThreadManager( GetThreadNameArray("SyncP", nIns) ),
m_nIns(nIns),
m_bIsExec(false),
mcls_detail(NULL)
{
	memset(m_nRecvFrameNo, 0xff, sizeof(m_nRecvFrameNo));
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
ImgSyncProc::~ImgSyncProc(void)
{
}


//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int ImgSyncProc::ThreadFirst()
{
	Reset();

	// �C�x���g�ݒ�
	HANDLE hArray[] = { mque_pIn->GetEvQue()
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int ImgSyncProc::ThreadLast()
{
	//// ���ԋp�̊킪����Εԋp
	ResetBufFrame();
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void ImgSyncProc::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_ADD_IMG = 0,					// �t���[���摜�L���[�C���O
			EV_FREE							// ����V�O�i��
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_ADD_IMG: 						// �t���[���捞
		AddNewItem();
		return;
	
//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG2(em_ERR, "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����

//------------------------------------------
// ���Z�b�g
//------------------------------------------
void ImgSyncProc::Reset()
{
	ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[m_nIns];
	ParamManager::P_PARAM_CAMSET::INS const& prmCI = mcls_pParam->GetParamCamSet().ins[m_nIns];

	//// �|�C���^��������
	ResetBufFrame();

	//// �����t���O
	m_nEndFrameNo = 0;
	m_bIsSync = false;

	for (int ii = 0; ii < NUM_CAM_POS; ii++) {
		m_nRecvFrameNo[ii] = -1;
	}



	//�L���������R�s�[���Ă����B
	for (int ii = 0; ii < NUM_CAM_POS; ii++) {
		m_bEnabled[ii] = prmCI.camset[ii].bIsEnable;
	}


	mcls_Time.InitCycle();
}


//------------------------------------------
// �摜�擾
//	�� ������Ɠ���B
//		���������̃t���[���摜�������܂ŁA���ێ�����B
//------------------------------------------
void ImgSyncProc::AddNewItem()
{
	//// �t���[���摜�L���[���擾
	HT_RPI_FDATA* p = mque_pIn->GetFromHead();
	if (NULL == p) {							// �ʏ킠�肦�Ȃ��͂�
		LOG2(em_ERR, "[%s] �X���b�h���[�v�L���[����!", my_sThreadName);
		return;
	}
	QueryPerformanceCounter((LARGE_INTEGER *)&p->nTimeFrameRecv);

	CString strSetResult;
	for (int ii = 0; ii < NUM_CAM_POS; ii++) {
		strSetResult.AppendFormat("%02d,", (int)m_pBufFrame[ii].size());
	}


	//TODO�F���肪�Ў��ɂ����Ƃ��ɁA�Б������t���[��������̂ŁA
	// ���̂Ƃ��A�Â��o�b�t�@�������Ȃ��Ƃ��߂����B

	// ��M�󋵃��O
	LOG2(em_MSG, "[%s.c%d] �t���[����M [fno=%d] (%d:%s) Size=%d Que:%d PcName[%s] Subinf:%d [%p, %p, %p]", my_sThreadName, p->nCamId, p->nFrameNo,
		m_nEndFrameNo + 1, strSetResult, p->base_head.len, mque_pIn->GetCount(), p->base_head.pname, p->base_head.subinf, p, p->base_head.addr[0], p->base_head.addr[1]);


	//------------------------------ >>>>
	// �e�퐮�����`�F�b�N �B���ꂪ�~�\

	if (!mcls_pStatus->IsRun()) {
		mque_pFree->AddToTail(p);
		return;
	}

	// �P�J�����ł̑��M�A�ԃ`�F�b�N
	//�@�{�_�~�[�t���[���}��
	if (!CheckSeqNo(p)) {
		mque_pFree->AddToTail(p);
		return;
	}

	//�񓯊��o�b�t�@�ɂ��߂�
	if (!this->CheckAndAdd(p))
	{
		mque_pFree->AddToTail(p);
		return;
	}



	//// �S�J�����Z�b�g�����������`�F�b�N
	if (!CheckCamset()) return;
	// <<<---------------------------------



	//// ����
	if (m_bIsExec) {
		mcls_Time.CounterCycle();

		Exec();

	}
	else {
		PoolBack_RecvFreeQue(-1);
	}
}
#include <algorithm>
//------------------------------------------
// �S�J�����Z�b�g�œ��������H
// FRAME_DATA* p �t���[�����
// �߂�l : true:��������
//------------------------------------------
bool ImgSyncProc::CheckAndAdd(HT_RPI_FDATA* p)
{
	const int cam = p->nCamId;		// 0�I���W��


	ParamManager::P_MAIN		const& prm = mcls_pParam->GetMain();

	// =========================================================
	// ����
	if (2 <= prm.nDispPrint) {
		CString sWk;
		KizuBitmapManager	cls;

		BYTE* pImg = (BYTE*)p->base_head.addr[0];
		if (pImg &&
			p->nTimeFrameRecv != INT_MIN		//�_�~�[�t���[���͓ǂݍ��ݐ�p
			) {
			for (int rec = 0; rec < MAX_IMAGE_CNT; rec++) {
				if (0 == p->nImgSize[rec]) continue;

				int nFrom = p->nImgOffset[rec];
				BYTE* pWk = &pImg[nFrom];

				// �V�[�P���XNo
				sWk.Format("%06d", p->nFrameNo);
				cls.DrawText_Gray(prm.nFrameSizeX, prm.nFrameSizeY, pWk, 1000, 200 + 100 * 0, sWk, 100, 255);
			}
		}
	}

	if (!AddToTail(cam, p))
	{
		if (!m_bIsSync) {
			//�������o�b�t�@�����ӂꂽ�̂ŁA��ԌÂ����ԋp�B
			HT_RPI_FDATA* pFirstInBuf = GetFromHead(cam);



			LOG2(em_WAR, "[%s.c%d] �����O�ɕ����������@�\�����ӂꂽ�I "
				"[fno=%d]. �ŏ���Q[fno=%d]��ԋp",
				my_sThreadName, p->nCamId, p->nFrameNo,
				pFirstInBuf->nFrameNo);
				//Get�����̂ŁAAdd�ł���͂��B
				AddToTail(cam, p);
				//Get�����t���[���͕ԋp
				mque_pFree->AddToTail(pFirstInBuf);
		}
		else {
			LOG2(em_ERR, "[%s.c%d] ��M�t���[���摜����(�������p�o�b�t�@���ӂ�j�I�I [FNo=%d,%d]", my_sThreadName, cam, PeekFromHead(cam)->nFrameNo, p->nFrameNo);

			int nCount = OutputIncompleteQueue(p);
			if (nCount > 0) {
				LOG(em_WAR), "[%s] �����҂��ҋ@... %d ms", my_sThreadName, nCount * 300);
				Sleep(nCount * 300);
			}

			/*
					PoolBack_RecvFreeQue(cam);						// ��ԌÂ����ԋp

					// 2�ԖڂɌÂ�������o���B=�ŌÃL���[
					m_SetArray[cam].pData  = GetFromHead(cam);
					m_SetArray[cam].bIsSet = true;

					// �P�󂫂��ł����̂ŁA�ēo�^
					bRet = AddToTail(cam, p);
					if( ! bRet ) {	// ���肦�Ȃ�
						LOG2(em_ERR, "[%s] ��M�t���[���摜���������s�\�I�I [cam=%d : FNo=%d]", my_sThreadName, cam, p->nFrameNo);
					}
			*/
			mcls_pStatus->SetFrameSyncSt(m_nIns, false);

			// �����ċN��
			mcls_pStatus->ResetAp();
			return false;
		}
	}
	return true;
}

//�����Ă�Ȃ��ŁA�ŏ��̃t���[��No�����t���[�����������B
// bNeedAllCamera : �S�J��������t���[�������Ă���O�񂩁H
// pnCamIndex[opt]: �ŏ��t���[��NO�́A�ǂ̃J�����C���f�N�X�icamIndex)�ɑ������H
HT_RPI_FDATA *ImgSyncProc::GetMinimumFrame( bool bNeedAllCamera, int *pnCamIndex )
{
	ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[m_nIns];

	HT_RPI_FDATA *pMinData  = NULL;
	int  nMinFrameNo = INT_MAX;
	for( int ii=0; ii<prmI.nCamsetNum; ii++){
		if( !m_bEnabled[ii] ) continue;

		HT_RPI_FDATA*pi = PeekFromHead(ii);
		if( !pi )
		{
			//�f�[�^���Ȃ����S���̃J�������炫�ĂȂ��B
			if( bNeedAllCamera)
				return NULL;

			continue;
		}

		if( nMinFrameNo > (int)pi->nFrameNo) {
			pMinData   =  pi;
			nMinFrameNo= pMinData->nFrameNo;
			if( pnCamIndex )
				*pnCamIndex = ii;
		}
	}
	return pMinData;

}


//------------------------------------------
// �S�J�����Z�b�g�œ��������H
// FRAME_DATA* p �t���[�����
// �߂�l : true:��������
//------------------------------------------
bool ImgSyncProc::CheckCamset()
{
	ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[m_nIns];


	HT_RPI_FDATA *pMinData = GetMinimumFrame(true, NULL);
	if( !pMinData ){
		return false;
	}
	int  nMinFrameNo = pMinData->nFrameNo;

	//�S���̃J�������牽������t���[���������I�i����Ă��邩���j
	bool bWidthsyncWithDummy = false;
	char dummy_stat[MAX_CAMSET+1] = "";
	for( int ii=0; ii<prmI.nCamsetNum; ii++){
		
		HT_RPI_FDATA *pi = PeekFromHead(ii);
		//����Ă邵�B�B
		if( pi && pi->nFrameNo != nMinFrameNo){
			//�Ƃ肠�����A�_�~�[�t���[�������āAFrameNo�����낦��B�B
			HT_RPI_FDATA *pDummty = CreateDummyFrame(pMinData, pi->nCamId, ii);

			m_pBufFrame[ii].push_front( pDummty );

			bWidthsyncWithDummy  = true;
			dummy_stat[ii] = 'X';		
		}
		else{
			dummy_stat[ii] = 'O';		
		}
	}
	if( bWidthsyncWithDummy ){
		LOG(em_WAR), "[%s] �_�~�[�������ĕ����� [FNo=%d:%s]", my_sThreadName, nMinFrameNo, dummy_stat);
	}
	//����ŁAm_SetArray�͂�������I(disabled���̂����āj



	//// �����t���[�����H
	
	if( !m_bIsSync )
	{
		//�����͏���B
		{
			m_bIsSync = true;
			LOG(em_MSG), "[%s] �������� [fno=%d]==================", my_sThreadName, pMinData->nFrameNo);
			mcls_pStatus->SetFrameSyncSt(m_nIns, true);
			return true;
		}
		//�����܂��Ȃ̂ŁA���܍���������܂߂āA���P�ʂł�����
		{
			PoolBack_RecvFreeQue(-1);
			return false;
		}
	}
	else{
		return true;
	}
}

//		if( ! prmCI.camset[ii].bIsEnable ) continue;	// �����J�����́A�f�[�^������Ƃ��ď���

// �������o�b�t�@�����ӂꂽ�̂ŁA�_�~�[���g���āA���܂��Ă镪���������ĉ����ցB
int ImgSyncProc::OutputIncompleteQueue(HT_RPI_FDATA* pLast)
{
	ParamManager::P_MAIN		const& prm	= mcls_pParam->GetMain();
	ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[m_nIns];
	
	int ncount = 0;
	while(1)
	{
		int minCamIndex = 0;
		HT_RPI_FDATA *pMinData = GetMinimumFrame(false, &minCamIndex );
	
		//�S���f���I��������Ă��ƂȂ̂ŏI��
		if( pMinData == NULL ) {
			break;
		}
		
		char dummy_stat[MAX_CAMSET+1] = "";
		
		for(int ii=0; ii<prmI.nCamsetNum; ii++ ) {
			if( !m_bEnabled[ii] ) {
				dummy_stat[ii] = 'D';
				continue;	// �����J�����́A�f�[�^������Ƃ��ď���
			}
			if( m_pBufFrame[ii].empty()) {
				int mincam = pMinData->nCamId;		// 0�I���W��
				
				//// �_�~�[��擪�ɋl�߂�
				HT_RPI_FDATA* pDmy = CreateDummyFrame(pMinData, mincam + (ii - minCamIndex), ii);
				m_pBufFrame[ii].push_front(pDmy);

				dummy_stat[ii] = 'X';		
			}
			else{
				dummy_stat[ii] = 'O';		
			}
		}
		LOG2(em_ERR, "[%s] �������o�b�t�@�����ӂꂽ�̂ŁA�_�~�[���g���ĉ����֗����܂��B [fno=%d:%s]", my_sThreadName, pMinData->nFrameNo,dummy_stat);

		//�����ցB
		{
			ncount++;
			Exec();
		}

	}
	return ncount;
}

HT_RPI_FDATA *new_dummy_frame();

HT_RPI_FDATA* ImgSyncProc::CreateDummyFrame( const HT_RPI_FDATA* p, int nCamId, int nCamIndex)
{
	//�_�~�[�t���[�������Ȃ��Ƃ��A���Ԃ𕡎G�ɂ������Ȃ̂ŁA
	// Frame���̂�new���ċ��L�摜�ɂ��邱�ƂŁA�e�Չ���}��


	//HT_RPI_FDATA* pDmy = mque_pFree->GetFromHead();				// �󂫂���擾
	//if( NULL == pDmy ) {
	//	// ��󂫂Ȃ� (�񕜉\�ȃG���[)
	//	LOG(em_ERR), "[%s] GRAB�󂫃L���[����[%d���][cam=%d]!", my_sThreadName, nCamIndex);
	//	return NULL;
	//}


	HT_RPI_FDATA* pDmy = new_dummy_frame();

	//// �_�~�[�t���[������
	pDmy->nCamId	= nCamId;//p->nCamId + (nCamIndex - pCamIndex );
	pDmy->nFrameNo	= p->nFrameNo;
	pDmy->nTimeFrameRecv = INT_MIN;		//�_�~�[�}�[�N�B
	memset(pDmy->dEdgePos, 0x00, sizeof(p->dEdgePos));

	//�摜�n
	pDmy->nWidth = p->nWidth;
	pDmy->nHeight = p->nHeight;
	pDmy->nImgCnt	= p->nImgCnt;
	for(int jj=0; jj<MAX_IMAGE_CNT; jj++) {
		pDmy->nImgSize[jj]	= p->nImgSize[jj];
		pDmy->nImgOffset[jj]= p->nImgOffset[jj];
	}

	return pDmy;
}
//------------------------------------------
// �P�J�����ł̑��M�A�ԃ`�F�b�N
// FRAME_DATA* p �t���[�����
// �߂�l : false ���t���[����j���B  true ������go
//------------------------------------------
bool ImgSyncProc::CheckSeqNo(HT_RPI_FDATA* p)
{
	ParamManager::P_MAIN		const& prm = mcls_pParam->GetMain();


	int cam = p->nCamId;		// 0�I���W��

	// =========================================================
	// �A�ԃ`�F�b�N

	//WORD a = -1;
	//WORD b = 0;
	//WORD c = 1;
	//long n;


	//n=b- (WORD)(a+1);		// 0 
	//n=c- (WORD)(b+1);

	//n=c-(WORD)(a+1);		// 1�R����
	//n=b-(WORD)(c+1);		// 1�R�e���R
	//n=a-(WORD)(c+1);		// 2�R�e���R	�����ꂪ�����Ǝ��Ȃ�

	//�ŏ��̃t���[���͖������Ƃ���
	if (m_nRecvFrameNo[cam] == -1) {
		m_nRecvFrameNo[cam] = p->nFrameNo - 1;
	}

	long nSa = p->nFrameNo - (m_nRecvFrameNo[cam] + 1);		// ���B(1�Ȃ�1�t���[�������B -1�Ȃ�d���B -2�ȍ~�Ȃ�߂�) 0�Ȃ�A��


//LOG(em_WAR), "[%s] iPort�V�[�P���XNo������ [cam=%d : INo=%d->%d]", my_sThreadName, cam, nOld, p->nSeqNum[0]);


	if (0 == nSa) {
		m_nRecvFrameNo[cam] = p->nFrameNo;
		return true;		// �A�ԃ`�F�b�N
	}

	//	//// �t���[���������s�L��
	//	if( 1 != prm.nExecDummyInsert ) return true;


	//// �e���R����
	// �����������傫�������ꍇ�A�e���R��( 65,535 - 1+1) �Ȃǂ̃P�[�X�ƂȂ�
	// �ǂ���ɂ��Ă��}�C�i�X�l(�e���R)���́A�e���R�t���[���͔j�����邽�ߑ卷�Ȃ��B
	if (3 < abs(nSa)) {
		LOG2(em_ERR, "[%s.c%d] �A�ԃ`�F�b�N�������ُ�(��:%d) [FNo=%d != %d]", my_sThreadName, cam, nSa, p->nFrameNo, m_nRecvFrameNo[cam] + 1);
		return false;
	}


	// =========================================================
	// ����ȍ~�́A�t���[����������
	LOG2(em_ERR, "[%s.c%d] �A��No�������ُ�(�t���[����������)(��:%d) [FNo:%d != have:%d]", my_sThreadName, cam, nSa, p->nFrameNo, m_nRecvFrameNo[cam] + 1);
//	�����p���O--->>>
	//syslog(900, "[%s.c%d] �A��No�������ُ�(�t���[����������)(��:%d) [FNo:%d != have:%d]", my_sThreadName, cam, nSa, p->nFrameNo, m_nRecvFrameNo[cam] + 1);
//	�����p���O---<<<
	m_nRecvFrameNo[cam] = p->nFrameNo;

	// �������� �_�~�[�t���[���}��
	for (int ii = 0; ii < nSa; ii++) {

		//// �_�~�[�t���[������
		HT_RPI_FDATA* pDmy = CreateDummyFrame(p, p->nCamId, ii);
		pDmy->nFrameNo = p->nFrameNo - (nSa - ii);

		LOG(em_WAR), "[%s.c%d]�_�~�[�t���[���}�� [FNo=%d]", my_sThreadName, cam, pDmy->nFrameNo);

		//// �}��
		if (!CheckAndAdd(pDmy)) return true;
	}
	return true;
}


//------------------------------------------
// �������s
//------------------------------------------
void ImgSyncProc::Exec()
{
	// =========================================================
	//// ���̃L���[���𐶐�
	FRAME_DATA_ARRAY* pDeli = new FRAME_DATA_ARRAY;
	memset(pDeli, 0x00, sizeof(FRAME_DATA_ARRAY));


	//// �f�[�^�Z�b�g
	pDeli->nIns = m_nIns;
	pDeli->nNum = mcls_pParam->GetMain().ins[m_nIns].nCamsetNum;

	QueryPerformanceCounter((LARGE_INTEGER *)&pDeli->nTimePlgSyncEnd);

	pDeli->emImgSaveStat = IMG_SAVE_STAT_SAVE;

	for (int ii = 0; ii < pDeli->nNum; ii++) {
		pDeli->nIsOk[ii] = false;
		pDeli->pData[ii] = GetFromHead(ii);

		if (pDeli->pData[ii]) {
			pDeli->nIsOk[ii] = true;
			pDeli->nFno = pDeli->pData[ii]->nFrameNo;
			pDeli->nIdx = ii;
			//�_�~�[�t���[���Ȃ̂ŁA�`��p�̃t���[��NO��-1�ɂ���B
			if (pDeli->pData[ii]->nTimeFrameRecv == INT_MIN) {
				pDeli->pData[ii]->nFrameNo = -1;
			}
		}


		/*
		//// BMP
		if(true) {
			if( ! m_SetArray[ii].bIsSet) continue;
			for(int jj=0; jj<m_SetArray[ii].pData->nImgCnt; jj++) {

				int off = m_SetArray[ii].pData->nImgOffset[jj];
				BYTE* p = (BYTE*)m_SetArray[ii].pData->base_head.addr[0];

				CString s;
				s.Format("to_raw_%d_%d_%d.bmp", m_SetArray[ii].pData->nFCNo, ii, jj);
				KizuBitmapManager kb;
				kb.DataToBMPfile_Gray(SIZE_FRAME_IMG_X, SIZE_FRAME_IMG_Y, &p[off], s);
			}
		}
		*/
	}




	m_nEndFrameNo = pDeli->nFno;
	LOG2(em_MSG, "[%s] ������ [fno=%d]", my_sThreadName, pDeli->nFno);

	////// ����
	if (!mque_pOut->AddToTail(pDeli)) {
		// �m�ۂ�����͑S���J��
		for (int ii = 0; ii < pDeli->nNum; ii++) {
			mque_pFree->AddToTail(pDeli->pData[ii]);
		}
		delete pDeli;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �o�b�t�@�����O�Ǘ�

//------------------------------------------
// �o�b�t�@�����O
//------------------------------------------
bool ImgSyncProc::AddToTail(int cam, HT_RPI_FDATA* p)
{
	//�S����������o�b�t�@����O��Ȃ̂ŁA�킴�킴���O�ɏo���Ȃ��Ă�����
	//if(! m_pBufFrame[cam].empty() ){
	//	LOG(em_MSG), "[%s.c%d] �o�b�t�@�����O [FCno=%d] (%d, MAX=%d)", my_sThreadName, cam, p->nFCNo, m_pBufFrame[cam].size()+1, MAX_BUF_FRAME);	// +1�͍���^���镪
	//}


	if(MAX_BUF_FRAME <= m_pBufFrame[cam].size() ) return false;
	m_pBufFrame[cam].push_back(p);
	return true;
}

//------------------------------------------
// �o�b�t�@���o��
//------------------------------------------
HT_RPI_FDATA* ImgSyncProc::GetFromHead(int cam)
{
	_ASSERT( cam < NUM_CAM_POS);
	//if(0 >= m_pBufFrame[cam].size() ) return NULL;
	if( m_pBufFrame[cam].empty() ) return NULL;

	HT_RPI_FDATA* p = m_pBufFrame[cam].front();
	m_pBufFrame[cam].pop_front();
	return p;
}	


//------------------------------------------
// �o�b�t�@�݂邾��
//------------------------------------------
HT_RPI_FDATA* ImgSyncProc::PeekFromHead(int cam)
{
	_ASSERT( cam < NUM_CAM_POS);
	//if(0 >= m_pBufFrame[cam].size() ) return NULL;
	if( m_pBufFrame[cam].empty() ) return NULL;

	return m_pBufFrame[cam].front();
}	


//------------------------------------------
// �o�b�t�@�N���A
//------------------------------------------
void ImgSyncProc::ResetBufFrame()
{
	// �J�����P�ʂ̃o�b�t�@�����O
	for(int ii=0; ii<NUM_CAM_POS; ii++) {
		ResetBufFrame(ii);
	}
}
void ImgSyncProc::ResetBufFrame(int nIndex)
{
	while(HT_RPI_FDATA* p = GetFromHead(nIndex)) {
		mque_pFree->AddToTail(p);
	}
}

//------------------------------------------
// ��M�L���[�������o�b�t�@��1�񕪕ԋp�B
// int nIndex -1:�S�� 0�`:�w�肵���C���f�b�N�X�̂�
//------------------------------------------
void ImgSyncProc::PoolBack_RecvFreeQue(int nIndex)
{
	int s, e;
	if (-1 == nIndex) { s = 0; e = NUM_CAM_POS - 1; }
	else { s = e = nIndex; }

	LOG2(em_INF, "[%s] �o�b�t�@�N���A(%d�`%d)", my_sThreadName, s, e);


	for (int ii = s; ii <= e; ii++) {
		HT_RPI_FDATA* p = GetFromHead(ii);
		if (NULL == p) break;
		mque_pFree->AddToTail(p);
	}

}


//------------------------------------------
// �o�b�t�@�ő吔
//------------------------------------------
int ImgSyncProc::GetMaxBufFrameNum()
{
	int nMax = 0;
	// �J�����P�ʂ̃o�b�t�@�����O
	for(int ii=0; ii<NUM_CAM_POS; ii++) {
		if( nMax < (int)m_pBufFrame[ii].size() ) 
			nMax = (int)m_pBufFrame[ii].size();
	}
	return nMax;
}

