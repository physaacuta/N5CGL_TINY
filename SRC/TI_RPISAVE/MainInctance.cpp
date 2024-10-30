#include "StdAfx.h"
#include "MainInctance.h"

#include "TI_RPISAVE.h"			// ��ʂ̕��i�ɒ��ڃA�N�Z�X���邽��


#include "DummyFrame.h"

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
MainInctance::MainInctance(void) :
ThreadManager("MI")
{
	// �V�O�i������
	m_evThStart  = CreateEvent(NULL, FALSE, FALSE, NULL);				// ���C���X���b�h�����p


	// ������ �L���[
	gque_pUnion				= NULL;
	gque_pFrame				= NULL;
	gque_pComp				= NULL;

	for(int ii=0; ii<MAX_INS; ii++) {
		gque_pSockRecv[ii]	= NULL;
		gque_pCoilC[ii] = NULL;
		for(int jj=0; jj<MAX_COLOR_NUM; jj++ )	gque_pRpiWrite[ii][jj] = NULL;
	}

	// ������ �N���X
	gcls_pImgBufProc = NULL;
	gcls_pRpiCProc = NULL;
	gcls_pDispProc = NULL;
	for(int ii=0; ii<MAX_INS; ii++) {
		gcls_InsLog[ii] = NULL;
		for(int jj=0; jj<NUM_CAM_POS; jj++) {gcls_pSock[ii][jj] = NULL; }
		gcls_pSyncProc[ii] = NULL;
		gcls_pCoilProc[ii] = NULL;
		for(int jj=0; jj<MAX_COLOR_NUM; jj++ )	gcls_pRpiWProc[ii][jj] = NULL;
	}
	gcls_InsLog_ImgBuf_Union = NULL;
	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) gcls_pUnionProc[ii] = NULL;
	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) gcls_pCompProc[ii] = NULL;

}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
MainInctance::~MainInctance(void)
{
	// �V�O�i���J��
	CloseHandle(m_evThStart);
}

//------------------------------------------
// �X���b�h�L���[�Ƀ��[���f�[�^�o�^
// ENUM_MAININCTANCE emNo ���b�Z�[�WNo (�C�x���gNo���̂܂܂ɂ��悤���Ǝv�������AFACT??_01�Ƃ����ƕ�����ɂ��������߂�)
// COMMON_QUE* que ���[���X���b�g�\���̂��̂���
//------------------------------------------
void MainInctance::SetDeliveryMail(ENUM_MAININCTANCE emNo, COMMON_QUE* que)
{
	DELIVERY_MAIL* pMail = new DELIVERY_MAIL;
	pMail->emNo	= emNo;
	memcpy( &pMail->que, que, sizeof(COMMON_QUE));

	// �X���b�h�L���[�ɓo�^
	if( ! mque_DeliMail.AddToTailFreeCheck(pMail) ) {
		delete pMail;
		LOG(em_ERR), "[%s] mque_DeliMail �L���[�t��!!", my_sThreadName);
	}
}

//------------------------------------------
// ���������� (��x�̂݋N��)
//------------------------------------------
BOOL MainInctance::Init()
{
	CString sWk;
	int		nWk;

	//// ���C���C���X�^���X�̂݁A�X���b�h�����͍���Ă��� (�����ɔ�V�O�i���Ƃ���)
	//// �p�����N���X���s (���C������X���b�h�ׁ̈A������ƗD��x����)
	ThreadManager::Start(THREAD_PRIORITY_ABOVE_NORMAL );

	// �X���b�hID��������܂ł�����Ƌx�e
	while( 0==GetTid() ) {
		LOG(em_MSG), "[%s] �X���b�hID���蓖�Ē��E�E�E", my_sThreadName);
		Sleep(1);			// �X���b�h�N���܂ł�����Ƒ҂�
	}


	////////////////////////////////////
	// ������
	//// �p�����[�^�Ǎ��҂�
	gcls_pParam = new ParamManager();
	gcls_pParam->SetLogMgr(mcls_pLog);
	while( true ) {
		if( gcls_pParam->ReadParam_All() ) break;

		
		LOG(em_MSG), "[%s] �p�����[�^�Ǎ����s(DB�ڑ��ُ�H�I)�B���g���C���E�E�E", my_sThreadName);
		Sleep(10000);
	}


	//// �@���ԏ�����
	gcls_Status.SetLogMgr(mcls_pLog);
	gcls_Status.SetParamMgr(gcls_pParam);


	////////////////////////////////////
	////  ������
	ParamManager::P_MAIN		const& prm  = gcls_pParam->GetMain();
	ParamManager::P_MAIN::INS   const& prmI	= gcls_pParam->GetMain().ins[0];
	int nInsNum = prm.nIns;
	
	if( prmI.nCamsetNum > NUM_CAM_POS ){
		CString err;
		err.Format("[DB]prm.nCamSet(%d) >= [CNST]NUM_CAM_POS(%d)�Ȃ̂͂�������", prmI.nCamsetNum, NUM_CAM_POS);
		MessageBox(NULL,err, "�������`�F�b�N�ُ�", MB_OK | MB_ICONEXCLAMATION);
	}

	//---------------------
	//// ���̃Z�b�g
	CString		cTitle;
	CWnd*   cwnd		= ((CTI_RPISAVEApp*)AfxGetApp())->GetMainWnd();	// �g���₷���悤�ɃG�C���A�X
	cwnd->GetWindowTextA(cTitle);
	cTitle += " <";
	if( COMP_TYPE_JPEG==prmI.emCompType)	cTitle += "JPEG; ";	
	if( COMP_TYPE_LAC==prmI.emCompType )	cTitle += "LAC; ";	
	if( COMP_TYPE_RAW==prmI.emCompType )	cTitle += "RAW; ";	
	if( COMP_TYPE_BMP==prmI.emCompType )	cTitle += "BMP; ";	
	if( COMP_TYPE_NUC==prmI.emCompType )	cTitle += "NUC; ";	
	if( COMP_TYPE_LAC5==prmI.emCompType )	cTitle += "LAC5; ";	
	cTitle += ">";
	cwnd->SetWindowTextA(cTitle);

	//---------------------
	//// �̈�m��

	gque_pFrame = new ThreadLoopQueueImpl<FrameDataManager>( prm.nLoopQueFrameSize );

	gque_pUnion = new ThreadSyncManagerImpl<FRAME_DATA_ARRAY>("QueUnion", prm.nLoopQueRecvSize, prm.nLoopQueRecvSize);
	gque_pUnion->SetLogMgr(mcls_pLog);

	gque_pComp  = new ThreadSyncManagerImpl<FrameDataManager>("QueComp", prm.nLoopQueFrameSize, prm.nLoopQueFrameSize);
	gque_pComp->SetLogMgr(mcls_pLog);


	//---------------------
	// ��M��n�L���[
	{
		CDummyQueueFree::InitMemory(prm.nFrameSizeX * prm.nFrameSizeY * MAX_IMAGE_CNT);

		for(int ii=0; ii<nInsNum; ii++) {
			gque_pSockRecv[ii] = new ThreadLoopQueueImpl<HT_RPI_FDATA>( prm.nLoopQueRecvSize );
			// �̈�m��
			for(int jj=0; jj<prm.nLoopQueRecvSize; jj++) {
				static const int n[1] = { prm.nFrameSizeX * prm.nFrameSizeY * MAX_IMAGE_CNT};		// �L���[�T�C�Y

				HT_RPI_FDATA* p = new HT_RPI_FDATA;
				p->Alloc( n );
				gque_pSockRecv[ii]->AddFree(p);			// ���ǉ�
			}
		}
		int nMemSize = ((__int64)nInsNum * prm.nFrameSizeX * prm.nFrameSizeY * MAX_IMAGE_CNT * prm.nLoopQueRecvSize)/1024/1024;
		LOG(em_MSG),"[%s] ��M�L���[�T�C�Y [%dMB]", my_sThreadName, nMemSize);
	}

	for(int ii=0; ii<nInsNum; ii++) {
		gque_pCoilC[ii]		= new ThreadQueueImpl<FRAME_DATA_ARRAY>();	
		for(int jj=0; jj<MAX_COLOR_NUM; jj++ ) {
			gque_pRpiWrite[ii][jj]	= new ThreadQueueImpl<DELIVERY_RPIWRITE>();
		}
	}

	//---------------------
	// �t���[���摜�A����p
	int  nAryWk[8];				// 0�`1 �́A�Œ�B
								// 2 ���k���1�t���[���̃f�[�^�T�C�Y(����؂�鐔)
								// 3 �����݃��[�h (0:���������݃��[�h 1:�Z�N�^�����݃��[�h)
								// 4 �������Ǘ��֐��|�C���^
								// 5 ����
	nWk = prm.nBmpHeadSize + (prm.nFrameSizeX * prm.nFrameSizeY * NUM_CAM_POS); //�����I�ȍő�Ƃ���//(prm.nMaxWidth * prm.nMaxHeight);			// ���A���摜�T�C�Y (���A���摜�̐擪�ɂ� BMP�w�b�_�[��t�^)
	nAryWk[0] = GET_SIZE_JUST(nWk, 32);									// 32�o�C�g �A���C�����g�̗̈�m�ۂ̂���
	nAryWk[1] =	1;														// �J�����p�x��(=1�Œ�)
	nWk = prm.nBmpHeadSize + RPIFrameSizeInfo::LINK_SIZE_Cf + (prm.nMaxWidth * prm.nMaxHeight);
	if(1 == prm.nAddFrameInfoType) {
		nWk += sizeof(RpiLib::T_RPI_FRAME_INFO_T1);	
	}
	nAryWk[2] = GET_SIZE_JUST(nWk, 512);
	nAryWk[3] =1;
	// GPU��LAC �������Ǘ���`
	gcls_pCudaAlloc = new CudaAlloc(&gcls_Lac);
	memcpy(&nAryWk[4], &gcls_pCudaAlloc, sizeof(void *));
	//nAryWk[4] = 0;
	//nAryWk[5] = 0;

	gque_pFrame->QueueAlloc(gque_pFrame->GetSemCount(), nAryWk); 

	LOG(em_MSG), "[%s] ��]���i�`���̃L���[���m�� [%dMB]", my_sThreadName, ((__int64)nAryWk[0]*nAryWk[1]*gque_pFrame->GetSemCount())/1024/1024 );




	////////////////////////////////////
	//// ���[�J�[�X���b�h ������


	// �摜�\���N���X
	gcls_ImgWin.SetThreadName("ImgWin");
	gcls_ImgWin.SetLogMgr(mcls_pLog);

	// HDD�Ǘ��N���X
	gcls_Hdd.SetLogMgr(mcls_pLog);
	gcls_Hdd.SetParamMgr(gcls_pParam);
	gcls_Hdd.SetQueRpiMove(&gque_RpiMove);

	////////////////////////////////////
	//// �摜�����p

	for( int ii=0; ii<nInsNum; ii++ ){
		que_pSockRecv_Free.push_back( new CDummyQueueFree(gque_pSockRecv[ii]->gIQue_pFree) );
	}

	// ��M�N���X
	for(int ii=0; ii<nInsNum; ii++) {
		for(int jj=0; jj<NUM_CAM_POS; jj++) {
			gcls_pSock[ii][jj] = new SockOneToAnyRecv();
			gcls_pSock[ii][jj]->SetLogMgr(mcls_pLog);
			gcls_pSock[ii][jj]->SetTLQue((ThreadLoopQueue<SOCK_F_HEAD2>*) gque_pSockRecv[ii]);	// �������̂�n��

			sWk.Format("HT_RPI_%d_%d", ii, jj);
			gcls_pSock[ii][jj]->SetSession(sWk);
		}
	}
	// �������R�t���N���X
	for(int ii=0; ii<nInsNum; ii++) {
		gcls_pSyncProc[ii] = new ImgSyncProc(ii);
		gcls_pSyncProc[ii]->SetLogMgr(mcls_pLog);
		gcls_pSyncProc[ii]->SetDetailLog( gcls_InsLog[ii] );
		gcls_pSyncProc[ii]->SetStatusMgr(&gcls_Status);
		gcls_pSyncProc[ii]->SetParamMgr(gcls_pParam);

		
		gcls_pSyncProc[ii]->SetQueFree(que_pSockRecv_Free[ii]);
		gcls_pSyncProc[ii]->SetQueIn(gque_pSockRecv[ii]->gIQue_pPool);
		gcls_pSyncProc[ii]->SetQueOut(gque_pCoilC[ii]);

	}
	// �R�C���Ǘ��N���X
	for(int ii=0; ii<nInsNum; ii++) {
		gcls_pCoilProc[ii] = new CoilControlProc(ii);
		gcls_pCoilProc[ii]->SetLogMgr(mcls_pLog);
		gcls_pCoilProc[ii]->SetParamMgr(gcls_pParam);
		gcls_pCoilProc[ii]->SetQueIn(gque_pCoilC[ii]);
		gcls_pCoilProc[ii]->SetQueOut(gque_pUnion->gIQue_pExec);
		gcls_pCoilProc[ii]->SetQueRecvFree(que_pSockRecv_Free[ii]);
	}


	// �������J���������N���X (���񏈗�)
	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) gcls_pUnionProc[ii] = NULL;
	for(int ii=0; ii<prm.nThreadUnionNum; ii++) {
		gcls_pUnionProc[ii] = new ImgUnionProc(ii);
		gcls_pUnionProc[ii]->SetLogMgr(mcls_pLog);
		gcls_pUnionProc[ii]->SetDetailLog( gcls_InsLog_ImgBuf_Union );
		gcls_pUnionProc[ii]->SetStatusMgr(&gcls_Status);
		gcls_pUnionProc[ii]->SetParamMgr(gcls_pParam);
		gcls_pUnionProc[ii]->SetQueIn(gque_pUnion->gIQue_pExec);
		gcls_pUnionProc[ii]->SetQueOut(gque_pUnion->gIQue_pEnd);
		gcls_pUnionProc[ii]->SetQueFrame(gque_pFrame);
		gcls_pUnionProc[ii]->SetQueComp(gque_pComp);		// �L���[���Q�Ƃ̂��߂���
	}

	// �t���[���摜 �o�b�t�@�����O�Ǘ��N���X
	gcls_pImgBufProc = new ImgBufProc();
	gcls_pImgBufProc->SetLogMgr(mcls_pLog);
	gcls_pImgBufProc->SetDetailLog(gcls_InsLog_ImgBuf_Union);
	gcls_pImgBufProc->SetParamMgr(gcls_pParam);
	gcls_pImgBufProc->SetQueIn(gque_pUnion->gIQue_pEnd);
	gcls_pImgBufProc->SetQueOut(gque_pComp->gIQue_pExec);
	gcls_pImgBufProc->SetQueOutSkip(&gque_Disp);

	gcls_pImgBufProc->SetQueRecvFree( &que_pSockRecv_Free[0]);
	gcls_pImgBufProc->Alloc();


	// �摜���k�N���X (���񏈗�)
	KizuFunction::SetStatus(gcls_pParam->GetMain().nStateId, true, false);		// �@���Ԑ���
	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) gcls_pCompProc[ii] = NULL;
	for(int ii=0; ii<prm.nThreadCompNum; ii++) {
		gcls_pCompProc[ii] = new ImgCompProc(ii);
		gcls_pCompProc[ii]->SetLogMgr(mcls_pLog);
		gcls_pCompProc[ii]->SetParamMgr(gcls_pParam);
		gcls_pCompProc[ii]->SetQueIn(gque_pComp->gIQue_pExec);
		gcls_pCompProc[ii]->SetQueOut(gque_pComp->gIQue_pEnd);
	}



	// RPI�t�@�C�������N���X
	for(int ii=0; ii<nInsNum; ii++) {
		for(int jj=0; jj<prm.nImgNumTotal; jj++ ) {
			gcls_pRpiWProc[ii][jj] = new RpiWriteProc(ii, jj);
			gcls_pRpiWProc[ii][jj]->SetLogMgr(mcls_pLog);
			gcls_pRpiWProc[ii][jj]->SetParamMgr(gcls_pParam);
			gcls_pRpiWProc[ii][jj]->SetQueIn(gque_pRpiWrite[ii][jj]);
			gcls_pRpiWProc[ii][jj]->SetQueOut(&gque_Disp);
			gcls_pRpiWProc[ii][jj]->SetQueRpiMove(&gque_RpiMove);
		}
	}

	// RPI�t�@�C������N���X
	gcls_pRpiCProc = new RpiControlProc();
	gcls_pRpiCProc->SetLogMgr(mcls_pLog);
	gcls_pRpiCProc->SetParamMgr(gcls_pParam);
	gcls_pRpiCProc->SetRpiWProc(gcls_pRpiWProc);
	gcls_pRpiCProc->SetQueIn(gque_pComp->gIQue_pEnd);
	gcls_pRpiCProc->SetQueOut(gque_pRpiWrite);
	gcls_pRpiCProc->Init();


	// ���ʕ\���N���X
	gcls_pDispProc = new DispProc();
	gcls_pDispProc->SetLogMgr(mcls_pLog);
	gcls_pDispProc->SetParamMgr(gcls_pParam);
	gcls_pDispProc->SetImgWin(&gcls_ImgWin);
	gcls_pDispProc->SetQueIn(&gque_Disp);
	gcls_pDispProc->SetQueOut(gque_pFrame->gIQue_pFree);


	////////////////////////////////////
	// ���[�J�[�X���b�h �N��

	//// �L���[
	gque_pUnion->Start();
	gque_pComp->Start();

	//// �N���X
	gcls_Status.StartLock();


	//// ���C�����[�v
	for(int ii=0; ii<nInsNum; ii++) {
		for(int jj=0; jj<NUM_CAM_POS; jj++) {
			gcls_pSock[ii][jj]->StartLock(THREAD_PRIORITY_TIME_CRITICAL);	

		}
	}
	for(int ii=0; ii<nInsNum; ii++) {
		gcls_pSyncProc[ii]->StartLock();
		gcls_pCoilProc[ii]->StartLock();
	}
	gcls_pImgBufProc->StartLock();
	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) {
		if(NULL == gcls_pUnionProc[ii]) continue;
		gcls_pUnionProc[ii]->StartLock(THREAD_PRIORITY_TIME_CRITICAL);
	}
	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) {
		if(NULL == gcls_pCompProc[ii]) continue;
		gcls_pCompProc[ii]->StartLock();//THREAD_PRIORITY_LOWEST
	}
	gcls_pRpiCProc->StartLock();
	for(int ii=0; ii<nInsNum; ii++) {
		for(int jj=0; jj<prm.nImgNumTotal; jj++) gcls_pRpiWProc[ii][jj]->Start(THREAD_PRIORITY_LOWEST);
	}
	gcls_pDispProc->StartLock();

	// ��
	gcls_Hdd.Start();

	// ����
	gcls_Status.SetMainSt(StatusManager::STATE_INIT);

	return TRUE;
}

//------------------------------------------
// �I������ (��x�̂݋N��)
//------------------------------------------
void MainInctance::Exit()
{

	ParamManager::P_MAIN  const& prm = gcls_pParam->GetMain();
	int nInsNum = prm.nIns;

	////////////////////////////////////
	// ���[�J�[�X���b�h ��~

	// �L���[
	gque_pUnion->Stop();
	gque_pComp->Stop();

	// �N���X��~
	gcls_Status.Stop();
	gcls_pParam->Stop();
	gcls_ImgWin.Dispose();		// �O�̂���
	gcls_Hdd.Stop();

	// ���C�����[�v
	gcls_pImgBufProc->Stop();
	gcls_pImgBufProc->Free();
	gcls_pRpiCProc->Stop();
	gcls_pRpiCProc->ClosingRpi();
	gcls_pDispProc->Stop();
	for(int ii=0; ii<nInsNum; ii++) {
		gcls_pCoilProc[ii]->Stop();
		gcls_pSyncProc[ii]->Stop();

		for(int jj=0; jj<NUM_CAM_POS; jj++) {
			gcls_pSock[ii][jj]->Stop();
		}
		for(int jj=0; jj<prm.nImgNumTotal; jj++) {
			gcls_pRpiWProc[ii][jj]->Stop();
		}
	}

	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) {
		if(NULL == gcls_pUnionProc[ii]) continue;
		gcls_pUnionProc[ii]->Stop();
	}
	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) {
		if(NULL == gcls_pCompProc[ii]) continue;
		gcls_pCompProc[ii]->Stop();
	}


	////////////////////////////////////
	//// ���[�J�[�X���b�h ���
	delete gcls_pImgBufProc;
	delete gcls_pRpiCProc;
	delete gcls_pDispProc;
	for(int ii=0; ii<nInsNum; ii++) {
		delete gcls_pCoilProc[ii];
		delete gcls_pSyncProc[ii];
		for(int jj=0; jj<NUM_CAM_POS; jj++) { SAFE_DELETE( gcls_pSock[ii][jj] ); }
		for(int jj=0; jj<prm.nImgNumTotal; jj++) delete gcls_pRpiWProc[ii][jj];
	}
	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) {
		if(NULL == gcls_pUnionProc[ii]) continue;
		delete gcls_pUnionProc[ii];
	}
	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) {
		if(NULL == gcls_pCompProc[ii]) continue;
		delete gcls_pCompProc[ii];
	}


	////////////////////////////////////
	// ��J��
	// ��M��n�L���[
	{
		//int nCnt = 0;

		int nCntPool = 0;
		int nCntFree = 0;

		HT_RPI_FDATA* p = NULL;
		for(int ii=0; ii<nInsNum; ii++) {
			// ��L�L���[
			while(true) {
				p = gque_pSockRecv[ii]->GetPool();
				if( NULL == p) break;
				p->Free();
				delete p;
				p = NULL;
				nCntPool ++;
			}
			// �󂫃L���[
			while(true) {
				p = gque_pSockRecv[ii]->GetFree();
				if( NULL == p) break;
				p->Free();
				delete p;
				p = NULL;
				nCntFree ++;
			}
			delete gque_pSockRecv[ii];
			LOG(em_MSG), "[%s]<%d> �L���[��� ��L��=%d, ���=%d", my_sThreadName, ii, nCntPool, nCntFree);
		}
	}
	gque_pFrame->QueueFree();
	LOG(em_MSG), "[%s] ��]���i�`���̃L���[�����", my_sThreadName);

	// �̈�
	for(int ii=0; ii<nInsNum; ii++) {
		delete gque_pCoilC[ii];	
		for(int jj=0; jj<MAX_COLOR_NUM; jj++ ) {
			delete gque_pRpiWrite[ii][jj];
		}
	}
	delete gque_pFrame;
	delete gque_pUnion;
	delete gque_pComp;


	for( int ii=0; ii< (int)que_pSockRecv_Free.size(); ii++){
		delete  que_pSockRecv_Free[ii];
	}
	que_pSockRecv_Free.clear();
	// �N���X
	delete gcls_pParam;

}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int MainInctance::ThreadFirst()
{
	////// �����[�N�X���b�h�̏����������܂őҋ@ (MainInctance�̂�)
	LOG(em_MSG), "[%s] �����[�N�X���b�h�̏����������܂őҋ@���E�E�E [0x%X]", my_sThreadName, GetTid() );
	WaitForSingleObject(m_evThStart, INFINITE);

//*/
	//// �C�x���g�ݒ�
	HANDLE hArray[] = { mque_DeliMail.g_evSem
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
//*/
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void MainInctance::ThreadEvent(int nEventNo)
{
	DELIVERY_MAIL* pMail;

	////// �V�O�i���̏���
	enum {	EV_MAIL = 0,				// ���[���X���b�g �ʒm (���[���͈�ԍŌ�ɂ��悤����)
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_MAIL: 						// ���[���X���b�g �ʒm
		pMail = mque_DeliMail.GetFromHead();
		switch(pMail->emNo) {

//-----------------------------------------------------------
		case E_DEF_ML_START :									// �����J�n�v�� 
			DetectStart( &pMail->que );
			break;

//-----------------------------------------------------------
		case E_DEF_ML_STOP :									// ������~�v��
			DetectStop(0, 0);
			break;


//-----------------------------------------------------------
		case E_DEF_ML_NEXTCOIL :								// ���R�C�����

			if(true) {
				ParamManager::P_MAIN  const& prm = gcls_pParam->GetMain();
				int nMen = pMail->que.mix.idata[0];		// -1:�S��

				//// �R�C����񖳂��œ����Ă�̂ł���΁APC�����グ�ēr���Q���Ȃ̂ŁA�����I�ɊJ�n����
				if( ! gcls_Status.IsRun() && -1 != prm.nExecCoilStart) {
					LOG(em_WAR), "[%s] ���N����ԂŃR�C������M�I�����J�n", my_sThreadName);	

					// �����J�n
					COMMON_QUE que;
					memset(&que, 0x00, sizeof(que));
					que.nEventNo   = FACT_TI_RPISAVE_01;
					que.nLineNo	   = getlineid();
					send_mail(getmytaskname(), ".", &que);
				} 
				
				//// �Y���L�莞�A�ē���
				//if( gcls_Status.IsRun() && 1 == prm.nExecReSync ) {
				if( gcls_Status.IsRun() ) {
					int nIns = gcls_pParam->GetMain().nIns;

					for(int ii=0; ii<nIns; ii++) {
						ParamManager::P_MAIN::INS   const& prmI	= gcls_pParam->GetMain().ins[ii];
						if( -1 != nMen && prmI.nMen != nMen ) continue;	// �ΏۊO

						if( ! gcls_Status.GetFrameSyncSt(ii) ) {
							int nFno = gcls_pSyncProc[ii]->GetRecvFrameNo(0);
							if(0 != nFno) nFno += MAX_RESYNC_FNO;
							LOG(em_WAR), "[%s] �t���[���Y���L��I�ē����J�n Ins=%d Fno=%d", my_sThreadName, ii, nFno);

							gcls_pSyncProc[ii]->Reset();
							gcls_Status.ResetFrameSyncSt(ii, nFno);
						}
					}
				}
			}

			//// �ʏ�̃R�C������M
			if(true) {
				int nMen = pMail->que.mix.idata[0];		// -1:�S��

				int ii_s, ii_e;
				GetInsLoop(-1, ii_s, ii_e);
				for(int ii=ii_s; ii<=ii_e; ii++) {
					ParamManager::P_MAIN::INS   const& prmI	= gcls_pParam->GetMain().ins[ii];
					if( -1 != nMen && prmI.nMen != nMen ) continue;	// �ΏۊO

					gcls_pCoilProc[ii]->SetNextCoil(&pMail->que);
				}
			}

			break;


//-----------------------------------------------------------
		}
		// �J��
		delete pMail;
		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}

}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �V�[�P���X����

//------------------------------------------
// �����J�n����
// COMMON_QUE* que �N���v��
// �߂�l ���A���
//------------------------------------------
int MainInctance::DetectStart(COMMON_QUE* que)
{
	int		nRetcAnser	= 0;							// ���� (0:���� -1>:�d�̏� 1<:�y�̏�)
	DWORD	nFno = que->fl.data[0];						// �ē����pFno
	int		nIns = gcls_pParam->GetMain().nIns;

	////////////////////////////////////
	// ��ԃ`�F�b�N
	if( StatusManager::STATE_INIT != gcls_Status.GetMainSt() ) {
		LOG(em_ERR), "[%s] [%s] ���Ɍ����J�n�v����M", my_sThreadName, gcls_Status.GetMainStName() );
		return -1;
	}


	////////////////////////////////////
	// �O����
	//// static�p�����[�^���܂� �S�p�����[�^���Ď擾
	if( ! gcls_pParam->ReadParam_All(true) ) {
		LOG(em_ERR), "[%s] static�S�p�����[�^�Ǎ����s(DB�ڑ��ُ�H�I)�B", my_sThreadName);
		nRetcAnser = -10;
//		goto ending;
	}
	gcls_Status.ResetFrameSyncAllSt();

	////////////////////////////////////
	// �l�b�g���[�N�ڑ��҂�
	//		�q����Ȃ�����Ƃ����ĕʂɉ������Ȃ��B�ő僊�g���C���ԕ������҂��Ă���

	// ��x���ׂĐؒf���Ă���
	LOG(em_MSG), "[%s] ����������x���ׂĐؒf���{", my_sThreadName);
	for(int ii=0; ii<nIns; ii++) {
		for(int jj=0; jj<NUM_CAM_POS; jj++) {
			//gcls_pSock[ii][jj]->CloseAll(true);			// �X���b�h�ŉ���ؒf�����s���ɏd�Ȃ�Ƒ��d�J���ňُ�I�����Ă��܂�
			gcls_pSock[ii][jj]->SetEvCloseReq(); 
		}
	}
	Sleep(1000);

	// �ڑ��҂�
	for(int ii=0; ii<nIns; ii++) {
		int nReTry = 5;

		while( nReTry ) {
			int nConnectNum = 0;

			for(int jj=0; jj<NUM_CAM_POS; jj++) {
				if( SockOneToAnyRecv::STATE_NONE != gcls_pSock[ii][jj]->GetSockState(0) ) {
					nConnectNum++;
				}
			}

			// �҂�
			if(NUM_CAM_POS == nConnectNum) break;
			Sleep(1000);
			nReTry--;
		}
	}


	////////////////////////////////////
	// ���ʕ����̊J�n����
	// Reset��Init�Ȃ�
	for(int ii=0; ii<nIns; ii++) {
		gcls_Status.ResetFrameSyncSt(ii, nFno);
		gcls_pSyncProc[ii]->Reset();
		gcls_pSyncProc[ii]->SetIsExec(true);
		gcls_pCoilProc[ii]->Reset();
		gcls_pCoilProc[ii]->SetIsExec(true);

		for(int jj=0; jj<MAX_COLOR_NUM; jj++ ) {
			if(NULL == gcls_pRpiWProc[ii][jj]) continue;		
			gcls_pRpiWProc[ii][jj]->Reset();
			gcls_pRpiWProc[ii][jj]->SetIsExec(true);
		}
	}
	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) {
		if(NULL == gcls_pUnionProc[ii]) continue;
		gcls_pUnionProc[ii]->Reset();
		gcls_pUnionProc[ii]->SetIsExec(true);
	}
	gcls_pImgBufProc->Reset();
	gcls_pImgBufProc->SetIsExec(true);
	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) {
		if(NULL == gcls_pCompProc[ii]) continue;
		gcls_pCompProc[ii]->Reset();
		gcls_pCompProc[ii]->SetIsExec(true);
	}
	gcls_pDispProc->Reset();
	gcls_pDispProc->SetIsExec(true);

	gcls_pRpiCProc->Init();


	//////////////////////////////////////
	//// End����
	
	// �X�e�[�^�X�ύX
	gcls_Status.SetMainSt(StatusManager::EM_MAIN_STATE::STATE_RUN);
	LOG(em_MSG), "[%s] �����J�n����", my_sThreadName);

ending:

	//// �ُ�ɂ��A����葱��
	//if( 0 != nRetcAnser ) {
	//	// �X�^�[�g�������̂��X�g�b�v���Ă���
	//	for(ii=0; ii<MAX_THREAD_MULTI_P; ii++) {
	//		if(NULL == gcls_pCompProc[ii]) continue;
	//		gcls_pCompProc[ii]->SetIsExec(false);
	//	}
	//	gcls_pDispProc->SetIsExec(false);
	//}

	return nRetcAnser;
}

//------------------------------------------
// ������~����
// int mode �v����� (0:����ȏI���v��  1:�������������ċ����I�ɏI���v�� -1:���M���Ȃ�)
// int ansRetc ���� (0:���� 1:�y�̏� -1:�d�̏�) �� �������ɁA�d�̏Ⴊ���������ꍇ�́A�d�̏�ɂȂ�
// �߂�l ���A���
//------------------------------------------
int MainInctance::DetectStop(int mode, int ansRetc)
{
	int		nRetcAnser	= ansRetc;						// ���� (0:���� -1>:�d�̏� 1<:�y�̏�)
	int		nInsNum		= gcls_pParam->GetMain().nIns;


	////////////////////////////////////
	// �O����

	//// ���X�e�[�^�X�m�F
	if( StatusManager::STATE_NON	== gcls_Status.GetMainSt() ||
		StatusManager::STATE_INIT	== gcls_Status.GetMainSt() ) {
		LOG(em_ERR), "[%s] [%s] ���Ɍ�����~�v����M", my_sThreadName, gcls_Status.GetMainStName() );
		return -1;
	}
	LOG(em_MSG), "[%s] ������~�v���B(%s) ==============>>>", my_sThreadName, 0 == mode ? "�����~�v��" : "�ُ�ɂ�鋭����~�v��");  



	////////////////////////////////////
	// ���ʕ����̒�~����

	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) {
		if(NULL == gcls_pUnionProc[ii]) continue;
		gcls_pUnionProc[ii]->SetIsExec(false);
	}
	gcls_pImgBufProc->SetIsExec(false);
	for(int ii=0; ii<MAX_THREAD_MULTI_P; ii++) {
		if(NULL == gcls_pCompProc[ii]) continue;
		gcls_pCompProc[ii]->SetIsExec(false);
	}

	gcls_pRpiCProc->ClosingRpi();

	for(int ii=0; ii<nInsNum; ii++) {
		gcls_pSyncProc[ii]->SetIsExec(false);
		gcls_pSyncProc[ii]->Reset();
		
		gcls_pCoilProc[ii]->SetIsExec(false);
		gcls_pCoilProc[ii]->Reset();
		for(int jj=0; jj<MAX_COLOR_NUM; jj++ ) {
			if(NULL == gcls_pRpiWProc[ii][jj]) continue;
			gcls_pRpiWProc[ii][jj]->SetIsExec(false);
		}
	}
	gcls_pDispProc->SetIsExec(false);


	////////////////////////////////////
	// End����
	////////////////////////////////////

	//// �X�e�[�^�X�ύX
	gcls_Status.SetMainSt(StatusManager::STATE_INIT);

	return nRetcAnser;

	return 0;
}