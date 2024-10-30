#include "StdAfx.h"
#include "ImgUnionProc.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

#define LOG2( em, ... ) do { \
	if(mcls_detail){   mcls_detail->Write(KizuLib::LogFileManager:: em, __VA_ARGS__ ); }\
	if(!mcls_detail || \
		KizuLib::LogFileManager::em == KizuLib::LogFileManager::em_ERR ||\
	    KizuLib::LogFileManager::em == KizuLib::LogFileManager::em_WAR) {\
		if(mcls_pLog){       mcls_pLog->Write(KizuLib::LogFileManager:: em, __VA_ARGS__ ); }\
	}\
}while(0)

//------------------------------------------
// �R���X�g���N�^
// int nId �C���X�^���XNo
//------------------------------------------
ImgUnionProc::ImgUnionProc(int nId) :
ThreadManager( GetThreadNameArray("UnionP", nId) ),
m_bIsExec(false),
mque_pComp(NULL),
mque_pFrame(NULL),
mcls_detail(NULL)
{
	if( InterlockedCompareExchangePointer((PVOID*)&m_BlankData,this, 0) == 0 ){
		m_BlankData = (const BYTE*)VirtualAlloc( 0, SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y*MAX_COLOR_NUM, MEM_COMMIT, PAGE_READONLY);	// �u�����N�f�[�^
	}
	Reset();
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
ImgUnionProc::~ImgUnionProc(void)
{
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int ImgUnionProc::ThreadFirst()
{
	//// �C�x���g�ݒ�
	HANDLE hArray[] = { this->mque_pIn->GetEvQue() 
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int ImgUnionProc::ThreadLast()
{
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void ImgUnionProc::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_ADD_IMG = 0,					// �t���[���摜�L���[�C���O
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_ADD_IMG: 						// �t���[���捞
		this->ExecQue();
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
void ImgUnionProc::Reset()
{
}
//------------------------------------------
// ������
// Tin* pIn		����Que�f�[�^
// �߂�l		NULL:�ԋp���Ȃ� �o��Que�f�[�^:���̃X���b�h�֕ԋp����ꍇ  
//------------------------------------------
FRAME_DATA_ARRAY* ImgUnionProc::OnAddNewItem( FRAME_DATA_ARRAY* p )
{
	if( NULL == p ) {	
		LOG2(em_ERR, "[%s] �X���b�h���[�v�L���[����!", my_sThreadName);
		return p;
	}

	
//LOG(em_WAR), "[%s]<%d> %d Fno=%d, CNo=%d", my_sThreadName, p->nIns, p->emImgSaveStat, p->Udp.udp.framenum, p->nCFno);

	//// ����
	if(m_bIsExec) {
		Exec(p);
	} else {
		LOG2(em_INF, "[%s]<%d> �L�����Z��", my_sThreadName, p->nIns);
	}

	return p;
}


//------------------------------------------
// �������s
// FRAME_DATA_ARRAY* p ���������̃t���[�����
//------------------------------------------
void ImgUnionProc::Exec(FRAME_DATA_ARRAY* p)
{
	int ii, jj;
	int nIns = p->nIns;
	
	__int64		nTimeStart;					// �����J�n����
	QueryPerformanceCounter((LARGE_INTEGER *)&nTimeStart);


	// =========================================================
	// �悭�g���p�����[�^���G�C���A�X
	ParamManager::P_MAIN		const& prm	= mcls_pParam->GetMain();
	ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[nIns];
	int nMen = prmI.nMen;


	// =========================================================
	// �������A����̃t���[���̈�� �󂫊���擾
	bool bGetQue = true;						// �L���[���擾
	HT_RPI_FDATA* hrf = p->pData[ p->nIdx ];	// �悭�g�����

	//---------------------->>> ���b�N
	mque_pFrame->EnterCS();
	if( hrf->nImgCnt <= mque_pFrame->GetCountFree() ) {				// �����ŋ󂫂��Ȃ���΁A�z���g��NG
		for(int ii=0; ii<MAX_COLOR_NUM; ii++) {
			if( 0 == hrf->nImgSize[ii] ) continue;

			p->Fdc.pFrameDataMgr[ii] = mque_pFrame->GetFree(100);			// �Z�}�t�H�[�҂��o�[�W����(�����Ɍ��ʂ�Ԃ�����)
			if( NULL == p->Fdc.pFrameDataMgr[ii] ) break;
			((FrameDataManager*)p->Fdc.pFrameDataMgr[ii])->Clear();			// ������
		
//LOG2(em_ERR, "[%s] �A���L���[ �c��=%d", my_sThreadName, mque_pFrame->GetCountFree());
		}
		p->Fdc.bOk = true;
	} else {
		bGetQue = false;		// ���ɋ󂫖���
	}
	mque_pFrame->LeaveCS();
	//<<<---------------------- �A�����b�N

	if( ! bGetQue ) {
		// ��󂫂Ȃ� (�񕜉\�ȃG���[)
		LOG2(em_ERR, "[%s] �A���L���[ �햳��!(���ꂪ�������邱�Ǝ��́A���Ȃ�ُ�)", my_sThreadName);
		mcls_pStatus->SetFrameSyncSt( nIns, false );


		// �L���[�𒆓r���[�Ɏ擾���Ă����炢���Ȃ��̂� �ԋp
		for(int ii=0; ii<MAX_COLOR_NUM; ii++) {
			if( NULL == p->Fdc.pFrameDataMgr[ii] ) continue;
			mque_pFrame->AddFree( (FrameDataManager*)p->Fdc.pFrameDataMgr[ii] ); 
			p->Fdc.pFrameDataMgr[ii] = NULL;
			p->Fdc.bOk = false;
		}
		return;
	}

/*
	CString sWk;
	sWk.Format("c:\\testt1_%d.bmp", m_nIns); 
	KizuBitmapManager cls;
	cls.DataToBMPfile_Gray(2048, 2048, p->pData[0]->pImgs, sWk);
//*/

	// =========================================================
	// �t���[����
	//		������
	bool bSkip		 = false;									// �摜�A���X�L�b�v�L��
	EM_COMP_TYPE emCompType = p->emCompType;					// ���k�^�C�v
	int nCompQuality = p->nCompQuality;							// ���k��
	int nExecQueCnt  = mque_pComp->GetSyncQueCount();
	

	if (COMP_TYPE_LAC == emCompType) {
		// ���낻��Ԃɍ���Ȃ��\��������̂ŁA���k�������߂ď������Ԃ𑁂�����
		if(nExecQueCnt > (prm.nLoopQueFrameSize * 0.3) ) {
			nCompQuality = p->nCompQuality*2;
			if(nCompQuality >= 80 ) nCompQuality = 80;
		}
		if(nExecQueCnt > (prm.nLoopQueFrameSize * 0.5) ) {
			if(nCompQuality < 80 ) nCompQuality = 80;
		}
		if(nExecQueCnt > (prm.nLoopQueFrameSize * 0.6) ) {
			if(nCompQuality < 100 ) nCompQuality = 100;
		}
	} else if (COMP_TYPE_NUC == emCompType) {
		// ���낻��Ԃɍ���Ȃ��\��������̂ŁA���k�������߂ď������Ԃ𑁂�����
		if(nExecQueCnt > (prm.nLoopQueFrameSize * 0.3) ) {
			nCompQuality = p->nCompQuality*2;
			if(nCompQuality <= -80 ) nCompQuality = -80;
		}
		if(nExecQueCnt > (prm.nLoopQueFrameSize * 0.35) ) {
			if(nCompQuality > -80 ) nCompQuality = -80;
		}
		if(nExecQueCnt > (prm.nLoopQueFrameSize * 0.4) ) {
			if(nCompQuality > -100 ) nCompQuality = -100;
		}


	} else if (COMP_TYPE_JPEG == emCompType) {
		// ���낻��Ԃɍ���Ȃ��\��������̂ŁA���k�������߂ď������Ԃ𑁂�����
		if(nExecQueCnt > (prm.nLoopQueFrameSize * 0.3) ) {
			nCompQuality = p->nCompQuality/2;
			if(nCompQuality <= 30 ) nCompQuality = 30;
		}
		if(nExecQueCnt > (prm.nLoopQueFrameSize * 0.5) ) {
			if(nCompQuality > 30 ) nCompQuality = 30;
		}
		if(nExecQueCnt > (prm.nLoopQueFrameSize * 0.6) ) {
			if(nCompQuality > 20 ) nCompQuality = 20;
		}
	}


	// ----------------
	// ��������
	// �o�b�`���C���́A���[���o�ň�C�ɗ���Ă��邽�߁A�t���[�����������ƊÂ�����
	if (COMP_TYPE_LAC == emCompType) {

		// �Ԃɍ���Ȃ��̂ō����߂��ď������y�����Ē��������͂��킹��
		if( nExecQueCnt > (prm.nLoopQueFrameSize * 0.5) ) { 
			LOG2(em_ERR, "[%s]<%d> ���k�X�L�b�v", my_sThreadName, nIns);
			nCompQuality = 200;
			bSkip		 = true;
		}

	} else if (COMP_TYPE_NUC == emCompType) {
		// �Ԃɍ���Ȃ��̂ō����߂��ď������y�����Ē��������͂��킹��
		if( nExecQueCnt > (prm.nLoopQueFrameSize * 0.7) ) { 
			LOG2(em_ERR, "[%s]<%d> ���k�X�L�b�v", my_sThreadName, nIns);
			nCompQuality = -100;
			bSkip		 = true;
		}

	} else if (COMP_TYPE_JPEG == emCompType) {
		// �Ԃɍ���Ȃ��̂ō����߂��ď������y�����Ē��������͂��킹��
		if( nExecQueCnt > (prm.nLoopQueFrameSize * 0.7) ) { 
			LOG2(em_ERR, "[%s]<%d> ���k�X�L�b�v", my_sThreadName, nIns);
			nCompQuality = 1;
			bSkip		 = true;
		}
	}


	// =========================================================
	// �摜����

	// �P�x�␳
	Exec_Kido(p);

	// �摜�A��
	Exec_Union(p, bSkip);



	// =========================================================
	// ���̑��̎󂯓n���f�[�^�Z�b�g
	for(int rec=0; rec<MAX_COLOR_NUM; rec++) {
		if( NULL == p->Fdc.pFrameDataMgr[rec] ) continue;	// �ۑ��F�����������Ȃ��Ή�
		FrameDataManager* pDeli = (FrameDataManager*)p->Fdc.pFrameDataMgr[rec];

		// ��
		pDeli->nLen			= prmI.nUionHeight * prmI.nUionWidth + prm.nBmpHeadSize; //nTo[rec];
		pDeli->nNo			= hrf->nFrameNo;			// �J���������N�X�C�b�`�Ȃ̂�iPortNo���Ȃ��B�t���[��No���Z�b�g���Ă���
		for(jj=0; jj<prmI.nCamsetNum; jj++ ) {
			if( ! p->nIsOk[jj] ) continue;
			pDeli->nSeqNum[jj] = p->pData[jj]->nFrameNo;
		}
		//// �t���[����{���
		pDeli->nIns			= nIns;
		pDeli->nStartCamset	= prmI.nStartCamset;
		pDeli->nAngle		= prmI.nCamAngle;
		pDeli->nColor		= rec;
		pDeli->nMen			= prmI.nMen;
		pDeli->nUnionWidth	= prmI.nUionWidth;
		pDeli->nUnionHeight	= prmI.nUionHeight;


		//// �t���[�����̏��
		pDeli->nCFno		= p->nCFno;
		pDeli->nEdgeIdx[0]	= p->nEdgeIdx[0]; 
		pDeli->nEdgeIdx[1]	= p->nEdgeIdx[1];
		pDeli->bRpiSave		= true;
		pDeli->emImgSaveStat= p->emImgSaveStat;

	
		//// �t�ѓI�ȏ��




		//// �R�C����� 
		memcpy(pDeli->cKizukenNo, p->cKanriNo, SIZE_KIZUKEN_NO); 
		strcpy(pDeli->cCoilNo, p->cCoilNo);
		pDeli->emCompType	= p->emCompType;
		pDeli->nCompQuality = nCompQuality;
		pDeli->bDummyCoil   = p->bDummyCoil;

		//// UDP��� ��������ۂɂ�UDP���͖������A���������Ȃ����邽�߂ɁA�g���Ă�������Z�b�g���Ă���
		//memcpy( &pDeli->UdpData, &p->Udp.udp, sizeof(UDP_DATA)); 
		memset(&pDeli->UdpData, 0x00, sizeof(pDeli->UdpData));
		pDeli->UdpData.framenum = p->nFno;
		pDeli->UdpData.vRes = prmI.reso_y;
		pDeli->UdpData.pos[prmI.nMen].edgePosL  = p->nEdgePos[0];
		pDeli->UdpData.pos[prmI.nMen].edgePosR  = p->nEdgePos[1];
		pDeli->UdpData.pos[prmI.nMen].posFromHead  
			  = p->mmFromHeadAtCoilStart 
			  + p->nCFno * prmI.reso_y * prm.nFrameSizeY;
		//(�Ԉ�����̃t���[���T�C�Y�ɁA�Ԉ�����̕���\�������Ă���̂ł悢�B)




		//// ���ԃf�[�^�Z�b�g
		// �J�����擾����
		__int64 nMin, nMax;
		nMin = nMax = hrf->nTimeFrameRecv;
		for(ii=0; ii<prmI.nCamsetNum; ii++) {
			if( ! p->nIsOk[ii] ) continue;
			nMin = __min(nMin, p->pData[ii]->nTimeFrameRecv);
			nMax = __max(nMax, p->pData[ii]->nTimeFrameRecv);
		}
		pDeli->cls_Time.PutTimestamp( FrameDataManager::TIME_GetCamFast, nMin );	// �ŏ��̃J����
		pDeli->cls_Time.PutTimestamp( FrameDataManager::TIME_GetCamLast, nMax );	// �Ō�̃J����
		// ���̑�
		pDeli->cls_Time.PutTimestamp( FrameDataManager::TIME_PlgSync_END, p->nTimePlgSyncEnd);
		pDeli->cls_Time.PutTimestamp( FrameDataManager::TIME_ImgUnion_START, nTimeStart);
		pDeli->cls_Time.PutTimestamp( FrameDataManager::TIME_ImgUnion_END);		// �Ō�


		// ==========================
		// �f�o�b�N���
		if( 1 <= prm.nDispPrint ) {
			CString sWk;
			KizuBitmapManager	cls;			
			FrameDataManager* pDeli = (FrameDataManager*)p->Fdc.pFrameDataMgr[rec];
			BYTE* pWk = &pDeli->pImgs[prm.nBmpHeadSize];

			int nF = 50; //100;
			int nH = 50; //100;	// �t�H���g100�̂Ƃ���100
			sWk.Format("CFno=%06d", pDeli->nCFno+1 );
			cls.DrawText_Gray( pDeli->nUnionWidth, pDeli->nUnionHeight, pWk, 20, nH, sWk, nF, 255);
			
			sWk.Format("SeqNo =%06d", pDeli->nSeqNum[0] );
			cls.DrawText_Gray( pDeli->nUnionWidth, pDeli->nUnionHeight, pWk, 20, nH*2, sWk, nF, 255);
			for(int ii=1; ii<prmI.nCamsetNum; ii++) {
				sWk.Format("       %06d", pDeli->nSeqNum[ii] );
				cls.DrawText_Gray( pDeli->nUnionWidth, pDeli->nUnionHeight, pWk, 20, (nH*2)+nH*ii, sWk, nF, 255);
			}
		}
	}


//��
	LOG2(em_INF, "[%s] ������ (Fno=%d/%d) st=%d", my_sThreadName, p->nFno, p->nCFno, p->emImgSaveStat );


	////// �e�X�g�I�ɉ摜�o��
/*/
	CString sWk;
	sWk.Format("c:\\compgo%d.bmp", m_nIns); 

	std::ofstream   os;
	os.open(sWk, std::ios::binary);
	os.write((const char*)pDeli->pImgs, pDeli->nLen );
	os.close();
//*/
}

//------------------------------------------
// �P�x�␳
// FRAME_DATA_ARRAY* p ���������̃t���[�����
//------------------------------------------
void ImgUnionProc::Exec_Kido(FRAME_DATA_ARRAY* p)
{
	// =========================================================
	// �悭�g���p�����[�^���G�C���A�X
	int nIns = p->nIns;
	ParamManager::P_MAIN		const& prm = mcls_pParam->GetMain();
	ParamManager::P_MAIN::INS   const& prmI = mcls_pParam->GetMain().ins[nIns];
	int nMen = prmI.nMen;


	//// �����������{�Ȃ��H
	bool bNon = false;
	for (int cam = 0; cam<prmI.nCamsetNum; cam++) {
		if (prmI.camset[cam].bKidoUmu) { bNon = true; break; }
	}
	if (!bNon) return;


	//////////////////////////////////////////////////////////////////////////////////
	// �e�摜���Ƃ̏���

	short nWk;
	short*  vKido = new short[prm.nFrameSizeX];				// �␳�l

	for (int cam = 0; cam<prmI.nCamsetNum; cam++) {
		for (int rec = 0; rec<MAX_COLOR_NUM; rec++) {
			if (NULL == p->Fdc.pFrameDataMgr[rec]) continue;	// �ۑ��F�����������Ȃ��Ή�

			if (!prmI.camset[cam].bKidoUmu) continue;

			//int nFrom = prm.nFrameSizeX * prm.nFrameSizeY * rec;				// �R�s�[���̔z��ʒu (�F������ł��邽��)
			//int idx = nFrom;
			int idx = 0;
			BYTE* pImg = (BYTE*)p->pData[cam]->base_head.addr[0];	// ����Ώۉ摜�̐擪�A�h���X
			if (NULL == pImg[idx]) continue;

														// �␳�l�Z�b�g�B ���� vector[] �ŃA�N�Z�X����Ƃ��������Ԃ��|���邽��
			for (int xx = 0; xx < prm.nFrameSizeX; xx++) vKido[xx] = mcls_pParam->GetKido(nIns, cam)[xx];

			// �␳
			for (int yy = 0; yy < prm.nFrameSizeY; yy++) {
				for (int xx = 0; xx < prm.nFrameSizeX; xx++) {
					nWk = (short)pImg[idx] + vKido[xx];
					pImg[idx] = BYTE_CAST(nWk);

					idx++;
				}
			}
		}
	}

	// ��n��
	delete[] vKido;
}

//------------------------------------------
// �摜�A��
// FRAME_DATA_ARRAY* p ���������̃t���[�����
// bool bSkip �X�L�b�v
//------------------------------------------
void ImgUnionProc::Exec_Union(FRAME_DATA_ARRAY* p, bool bSkip)
{
	int jj, kk;
	int nWk;
	const BYTE bMask	= 0x00;				// �G�b�W�O�̃}�X�N�F


	// =========================================================
	// �悭�g���p�����[�^���G�C���A�X
	int nIns = p->nIns;
	ParamManager::P_MAIN		const& prm	= mcls_pParam->GetMain();
	ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[nIns];
	int nMen = prmI.nMen;

	// =========================================================
	// �G�b�W�O �h��ׂ��͈͂�����
	int nBlackL=0, nBlackR=0;							// �h��ׂ��� [pixel]

	if (prmI.bEdgeDeleteEnabel && 0 < p->nEdgeIdx[0]) {
		nBlackL = p->nEdgeIdx[0] - prmI.nEdgeDelete[0];						// ���[����̍폜�ʒu
	}

	// �E�G�b�W
	if (prmI.bEdgeDeleteEnabel && 0 < p->nEdgeIdx[1]) {
		nBlackR = prmI.nUionWidth - (p->nEdgeIdx[1] + prmI.nEdgeDelete[1]);	// �E�[����̍폜�ʒu
	}

	nBlackL = max( nBlackL, prmI.nImgDelete[0]);					// ���G�b�W
	nBlackR = max( nBlackR, prmI.nImgDelete[1]);					// �E�G�b�W

//LOG(em_MSG), "ID=%d   img=%d, C=%d, E=%d/%d, ED=%d/%d R=%d/%d",m_nIns, prmI.nUionWidth, prmI.nLineCenter, 
//(int)p->Udp.udp.pos[nMen].edgePosL, (int)p->Udp.udp.pos[nMen].edgePosR, prmI.nEdgeDelete[0], prmI.nEdgeDelete[1], nBlackL, nBlackR);




	//////////////////////////////////////////////////////////////////////////////////
	// �e�摜���Ƃ̏���
	int nTo[MAX_COLOR_NUM];				// �R�s�[��̔z��ʒu
	int nColorNum = 0;
	for(int rec=0; rec<MAX_COLOR_NUM; rec++) {
		if( NULL == p->Fdc.pFrameDataMgr[rec] ) continue;	// �ۑ��F�����������Ȃ��Ή�

		int nBase	= 0;													// �R�s�[��̃x�[�X
		//int nFrom	= prm.nCullSizeX * prm.nCullSizeY * nColorNum;		// �R�s�[���̔z��ʒu (�F������ł��邽��)
		int nFrom	= prm.nFrameSizeX * prm.nFrameSizeY * nColorNum;		// �R�s�[���̔z��ʒu (�F������ł��邽��)
		nColorNum ++;
		FrameDataManager* pDeli = (FrameDataManager*)p->Fdc.pFrameDataMgr[rec];
		

		// =========================================================
		// BMP�w�b�_�[��t�^
		nBase = prm.nBmpHeadSize;
		memcpy( pDeli->pImgs, prmI.BmpHead, nBase);
		nTo[rec] = nBase;


		// =========================================================
		// �X�L�b�v����
		if( bSkip ) {
			// ���k�������Ԃɍ���Ȃ��̂ō����߂��Ĉ��k���Ԃ�Z�����Ē��������͂��킹��
			int nSize = prmI.nUionWidth*prmI.nUionHeight;
			memset(&pDeli->pImgs[ nTo[rec] ], 0x00, nSize);
			nTo[rec] += nSize;
		}

		// =========================================================
		// �摜�A��
		if( ! bSkip ) {

			for(int yy=0; yy<prmI.nUionHeight; yy++) {

				//------------------------------
				//// �^�񒆃f�[�^��
				for(jj=0; jj<prmI.nCamsetNum; jj++ ) {
					
					// �g���₷���悤�ɉ摜�|�C���^�����o��
					const BYTE* pImg  = m_BlankData;	// �
					const BYTE* pImg2 = m_BlankData;	// �E�ƂȂ�
					if(	p->nIsOk[jj] ) {
						int off = p->pData[jj]->nImgOffset[rec];
						pImg = (BYTE*)p->pData[jj]->base_head.addr[0];
					} 
					if( jj != prmI.nCamsetNum-1 && p->nIsOk[jj+1] ) {
						int off = p->pData[jj+1]->nImgOffset[rec];
						pImg2 = (BYTE*)p->pData[jj+1]->base_head.addr[0];
					} 


					// �P���ȃR�s�[
					volatile int src_index = nFrom + prmI.camset[jj].nCopyImg[0];
					volatile int src_len   =         prmI.camset[jj].nCopyImg[1];
					memcpy( &pDeli->pImgs[ nTo[rec] ], &pImg[ src_index ], src_len);
					nTo[rec] += src_len;

					// �����⊮
					if(0 < prmI.camset[jj].nHokanImg[0]) {
						for(kk=0; kk<prmI.camset[jj].nHokanImg[1]; kk++) {
							nWk = (int)( (pImg [ kk + nFrom + prmI.camset[jj].nHokanImg[0] ] * ( prmI.camset[jj].nHokanImg[1] -1 - kk ) +
										  pImg2[ kk + nFrom + (prmI.camset[jj].nOverLap-prmI.camset[jj].nHokanImg[1])] * ( kk )) / ( prmI.camset[jj].nHokanImg[1]));
							pDeli->pImgs[ nTo[rec] ] = BYTE_CAST(nWk); //(BYTE)(nWk<=255 ? nWk : 255);
							nTo[rec]++;
						}
					}
				}

				//------------------------------
				//// ���h��ׂ�
				if(0 != nBlackL) {
					memset( &pDeli->pImgs[ nBase + prmI.nUionWidth * yy ], bMask, nBlackL);
				}
				
				//------------------------------
				//// �E�h��ׂ� �� �E�}��
				if(0 != nBlackR || 0 != prmI.nInsertRight ) {
					memset( &pDeli->pImgs[ nBase + prmI.nUionWidth * (yy+1) - (nBlackR+prmI.nInsertRight)], bMask, nBlackR+prmI.nInsertRight);
				
					nTo[rec] +=  prmI.nInsertRight;
				}

				//------------------------------
				//// ���̂���
				nFrom += prm.nFrameSizeX;


				////// �� �Ƃ肠�����m�F�p
				//if( 0 != nTo[rec] % prm.nUionWidth ) {
				//	LOG(em_MSG), "�摜���ُ�=%d/%d", nTo[rec], prm.nUionWidth);
				//}
			}

/*
//// BMP
if(true) {
		//int off = nBase + rec*(prmI.nUionWidth*prmI.nUionHeight);
		BYTE* pImg = (BYTE*)&pDeli->pImgs[ nBase ];

		CString s;
		s.Format("union_%d_%d.bmp", p->nFno, rec);
		KizuBitmapManager kb;
		kb.DataToBMPfile_Gray(prmI.nUionWidth, prmI.nUionHeight, &pImg[0], s);
}
//*/


		}
	}


	// =========================================================
	// ���̑��̎󂯓n���f�[�^�Z�b�g
	for(int rec=0; rec<MAX_COLOR_NUM; rec++) {
		if( NULL == p->Fdc.pFrameDataMgr[rec] ) continue;	// �f�[�^�����Ή�

		FrameDataManager* pDeli = (FrameDataManager*)p->Fdc.pFrameDataMgr[rec];

		// ��ʗp
		nWk = 0;
		for(jj=0; jj<prmI.nCamsetNum; jj++ ) {
			// �ʏ�t���[���摜���E��
			nWk += prmI.camset[jj].nCopyImg[1];
			pDeli->nImgLapPos[jj][0] = nWk;
			// �I�[�o�[���b�v���E��
			if(0 < prmI.camset[jj].nHokanImg[0]) {
				nWk += prmI.camset[jj].nHokanImg[1];
				pDeli->nImgLapPos[jj][1] = nWk;
			}
		}
		pDeli->nImgClipPos[0] = nBlackL;
		pDeli->nImgClipPos[1] = prmI.nUionWidth - nBlackR;
	}
}


const BYTE *ImgUnionProc::m_BlankData;
