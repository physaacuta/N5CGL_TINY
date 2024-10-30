#include "StdAfx.h"
#include "MainInstance.h"

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
MainInstance::MainInstance(void) :
ThreadManager("MI")
{
	// �V�O�i������
	m_evThStart  = CreateEvent(NULL, FALSE, FALSE, NULL);				// ���C���X���b�h�����p

	// �����V�O�i�� (�^�C�}�[)
	m_evTimeReStart		= CreateWaitableTimer(NULL, FALSE, NULL);		// ���������ċN���҂��^�C�}�[
	m_evTimeInitStart	= CreateWaitableTimer(NULL, FALSE, NULL);		// �J���������N�X�C�b�`�ݒ�A���T�[�҂��^�C�}�[
	m_evTimeWaitStart	= CreateWaitableTimer(NULL, FALSE, NULL);		// �����J�n ����A���T�[�҂��^�C�}�[

	// �����V�O�i�� (�g���b�L���O)
	m_evTr_Ken = CreateEvent(NULL, FALSE, FALSE, NULL);					// Tr ������_WPD�`��ʌ������u�ʒu�܂ł̃g���b�L���O
	m_evTr_KenGate = CreateEvent(NULL, FALSE, FALSE, NULL);				// Tr �Q�[�g�M��ON�`������_�܂ł̃g���b�L���O
	m_evTr_DspGate = CreateEvent(NULL, FALSE, FALSE, NULL);				// Tr �Q�[�g�M��ON�`�\����_�܂ł̃g���b�L���O

	// WPD�Q�[�g�M��ON�`WPD�M��ON�̊�true�ɂȂ�t���O
	m_bIsWpdGateTrKen = false;
	m_bIsWpdGateTrDsp = false;
	
	// �O�񑗐M�����R�}���h����������
	memset(m_strCmd, 0x00, sizeof(m_strCmd));

	// �_���t���[��No������
	for (int ii = 0; ii<NUM_MEN; ii++) {
		for (int jj = 0; jj<NUM_CAM_POS; jj++) {
			m_nFrameNo_V[ii][jj] = 0;
		}
	}

	// �R�}���h���X�g�N���A
	LogicParaRunFunc::ClearLinkswCmd();
}
//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
MainInstance::~MainInstance(void)
{
	// �V�O�i���J��
	CloseHandle(m_evThStart);
	CloseHandle(m_evTimeReStart);
	CloseHandle(m_evTimeInitStart);
	CloseHandle(m_evTimeWaitStart);
	CloseHandle(LogicParaRunFunc::GetEvTimeCamset());
	CloseHandle(LogicParaRunFunc::GetEvTimeResumeStart());
	for (int ii = 0; ii < NUM_ALL_HTPARA; ii++) {
		CloseHandle(LogicParaRunFunc::GetEvTimeResumeWait(ii));
	}
	CloseHandle(m_evTr_Ken);
	CloseHandle(m_evTr_KenGate);
	CloseHandle(m_evTr_DspGate);
}

//------------------------------------------
// �X���b�h�L���[�Ƀ��[���f�[�^�o�^
// ENUM_MAININSTANCE emNo ���b�Z�[�WNo (�C�x���gNo���̂܂܂ɂ��悤���Ǝv�������AFACT??_01�Ƃ����ƕ�����ɂ��������߂�)
// COMMON_QUE* que ���[���X���b�g�\���̂��̂���
//------------------------------------------
void MainInstance::SetDeliveryMail(ENUM_MAININSTANCE emNo, COMMON_QUE* que)
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
BOOL MainInstance::Init()
{
	CString cWk;
	bool    bWk;

	//// ���C���C���X�^���X�̂݁A�X���b�h�����͍���Ă��� (�����ɔ�V�O�i���Ƃ���)
	//// �p�����N���X���s (���C������X���b�h�ׁ̈A������ƗD��x����)
	ThreadManager::Start(THREAD_PRIORITY_ABOVE_NORMAL );

	// �X���b�hID��������܂ł�����Ƌx�e
	while( 0==GetTid() ) {
		LOG(em_MSG), "[%s] �X���b�hID���蓖�Ē��E�E�E", my_sThreadName);
		Sleep(1);			// �X���b�h�N���܂ł�����Ƒ҂�
	}


	////////////////////////////////////
	//// ���[�J�[�X���b�h ������
	//// �� ���ʂ��イ�悤 ��
	////////////////////////////////////

	//// �p�����[�^�Ǘ��N���X
	gcls_pParam = new ParamManager();
	gcls_pParam->SetLogMgr(mcls_pLog);
	while (true) {
		if (gcls_pParam->ReadParam_All()) break;

		LOG(em_MSG), "[%s] �p�����[�^�Ǎ����s(DB�ڑ��ُ�H)�B���g���C�����", my_sThreadName);
		Sleep(10000);
	}

	// �O���[�v���̕K�v����PC�䐔���擾�i�Ώۂ͔���PC�̂݁j
	gcls_pParam->Read_HtGroupNum(DIV_GROUP_MAX - 1);
	int nNumHt[DIV_GROUP_MAX - 1];
	gcls_pParam->GetHtGroupNum(nNumHt);
	LOG(em_MSG), "[%s] ����PC�䐔 �\�ʁF<Group1:%d><Group2:%d> ���ʁF<Group3:%d><Group4:%d>",
		my_sThreadName, nNumHt[0], nNumHt[1], nNumHt[2], nNumHt[3]);

	// �R���X�g���N�^�Ăяo���ׂ̈̃C���X�^���X���i��x�����j
	LogicParaRunFunc objLogicParaRunFunc(mcls_pLog, &gcls_Status, &gcls_Plg);
	ToLinkswUtil objToLinkswUtil(mcls_pLog, &gcls_Status);
		
	// ���W�b�N�p�������p�J�����Z�b�g���Z�b�g
	// ���O��l��ݒ�
	LogicParaRunFunc::SetHtParaCamNo(&gcls_pParam->GetCommonMast().nLogiParaCam[0][0]);

	//// �t���[��No�g���b�L���O�N���X
	gcls_TrFrm.Alloc(MAX_TR_FRAME);


	//// �X�e�[�^�X���O�N���X
	gcls_pStatusFunc = new StatusLogFunc();
	gcls_pStatusFunc->SetLogMgr(mcls_pLog);
	gcls_pStatusFunc->SetQueAddSQL((ThreadQueue<DELIVERY_DB>*)&gcls_DB.gque_AddSQLData);	// �ˑ��֌W���z�ɂȂ��Ă��܂����߁A�^�𓯂��\���̂̓z�ɃL���X�g
	gcls_pStatusFunc->Alloc();

	// �O�H�����Ǘ��N���X
	gcls_Mae.SetLogMgr(mcls_pLog);

	//// �X�e�[�^�X����N���X
	gcls_Status.SetLogMgr(mcls_pLog);
	gcls_Status.SetParamMgr(gcls_pParam);
	gcls_Status.SetStatusFunc(gcls_pStatusFunc);
	gcls_Status.SetKadou( DIV_KADOU_INIT );
	gcls_Status.SetDspSyslog(true);
	gcls_Status.SetAddStlog(true);

	gcls_Status.Start();			// �@��ID���擾�������ׁA�ŏ��ɓ������Ă���
	gcls_Status.SetStatusRead();	// �@��ID���擾�������ׁA�����ǂݍ���


	////////////////////////////////////
	//// ���[�J�[�X���b�h ������
	////////////////////////////////////
	
	//// DB�o�^����N���X
	gcls_DB.SetLogMgr(mcls_pLog);
	gcls_DB.SetStatusMgr(&gcls_Status);
	gcls_DB.Alloc();


	//// PIO�Ǘ��N���X
	// ����͖���

	//// �R�C�����Ǘ��N���X
	gcls_pCoil = new CoilManager();
	gcls_pCoil->SetLogMgr(mcls_pLog);
	gcls_pCoil->SetLogMgrA(mcls_pLogA);
	gcls_pCoil->SetStatusMgr(&gcls_Status);
	gcls_pCoil->SetParamMgr(gcls_pParam);
	gcls_pCoil->SetDbMgr(&gcls_DB);
	gcls_pCoil->SetEdgePosCont(&gcls_EdgePosCont);
	gcls_pCoil->SetMaeMgr(&gcls_Mae);
	gcls_pCoil->SetQueAddSQL(&gcls_DB.gque_AddSQLData);
	gcls_pCoil->SetDistPostion(gcls_pParam->GetParamCommon().nDistPosition);
	gcls_pCoil->SetDmemMgr(&gcls_Dmem);

	//// ���Ə�ԊǗ��N���X
	gcls_pSogyo = new SogyoManager("SOGYO_MODE");
	gcls_pSogyo->SetLogMgr(mcls_pLog);
	gcls_pSogyo->SetStatusMgr(&gcls_Status);

	//// �G�b�W���o�������Ǘ��N���X
	gcls_EdgePosCont.SetLogMgr(mcls_pLog);
	gcls_EdgePosCont.SetParamMgr(gcls_pParam);
	gcls_EdgePosCont.SetFrameSkipAddDB(true);										// �X�L�b�v����DB�ɓo�^
	gcls_EdgePosCont.SetOldMode(false);												// �̂Ɠ����@�\�̏ꍇ�́ATrue
	gcls_EdgePosCont.SetQueAddSQL(&gcls_DB.gque_AddSQLData);

	//// �f�[�^�������ʐM�Ǘ��N���X
	gcls_Dmem.SetLogMgr(mcls_pLog);
	gcls_Dmem.SetLogMgrA(mcls_pLogA);
	gcls_Dmem.SetParamMgr(gcls_pParam);
	//gcls_Dmem.SetCoilMgr(gcls_pCoil);
	gcls_Dmem.SetStatusMgr(&gcls_Status);
	//gcls_Dmem.SetPioMgr(&gcls_Pio);
	gcls_Dmem.SetQueAddSQL(&gcls_DB.gque_AddSQLData);
	//gcls_Dmem.SetAlarmeMgr(&gcls_Arm);

	// �V�[�P���T�ʐM��M�N���X
	gcls_pSeqFrameR = new SeqFrameRecv();
	gcls_pSeqFrameR->SetLogMgr(mcls_pLog);
	gcls_pSeqFrameR->SetLogSeqMgr(mcls_pLogSeq);
	gcls_pSeqFrameR->SetStatusMgr(&gcls_Status);

	//// �G�b�W���o�͈͎w��N���X
	gcls_EdgeRestSd.SetLogMgr(mcls_pLog);
	gcls_EdgeRestSd.SetParamMgr(gcls_pParam);
	gcls_EdgeRestSd.SetStatusMgr(&gcls_Status);

	//// PLG�Ǘ��N���X
	gcls_Plg.SetLogMgr(mcls_pLog);
	gcls_Plg.SetLockMgr(&gcls_Lock);
	gcls_Plg.SetStatusMgr(&gcls_Status);
	gcls_Plg.SetParamMgr(gcls_pParam);
	gcls_Plg.SetCoilMgr(gcls_pCoil);
	gcls_Plg.SetSeqFrameR(gcls_pSeqFrameR);
	gcls_Plg.SetSogyoMgr(gcls_pSogyo);
	gcls_Plg.SetEdgePosCont(&gcls_EdgePosCont);
	gcls_Plg.SetDmemMgr(&gcls_Dmem);
	gcls_Plg.SetEdgeRestMgr(&gcls_EdgeRestSd);
	gcls_Plg.SetDistPostion(gcls_pParam->GetParamCommon().nDistPosition);
	gcls_Plg.SetDistLen((DWORD)gcls_pParam->GetParamCommon().dDistLen);
#ifdef DSP_POS_REV_OPERATION
	gcls_Plg.SetPioMgr(&gcls_Pio);		// �����Ǝ��@�\�̂���
#endif
	//gcls_Plg.SetLogMMgr(mcls_pLogMark);
	gcls_Plg.SetTrFrmMgr(&gcls_TrFrm);
	gcls_Plg.Alloc();

	// �x��o�͊Ǘ��N���X
	gcls_Arm.SetLogMgr(mcls_pLog);
	gcls_Arm.SetLogMgrA(mcls_pLogA);
	gcls_Arm.SetLockMgr(&gcls_Lock);
	gcls_Arm.SetStatusMgr(&gcls_Status);
	gcls_Arm.SetParamMgr(gcls_pParam);
	gcls_Arm.SetCoilMgr(gcls_pCoil);
	gcls_Arm.SetPlgMgr(&gcls_Plg);
	gcls_Arm.SetLogGMgr(mcls_pLogG);
	gcls_Arm.SetQue_Pio(&gcls_Dmem.mque_AddData);		// �J�b�g�w���A�����w�� PO�o�͗p
	
	// �o�������R�C���P�ʂ̏�ԊǗ��p�g���b�L���O�N���X (�K��PLG�N���X�̏�������)
	gcls_pTlCoilOutSt = new TrackingList(gcls_Plg.Get_pLenKenDist());
	gcls_pCoil->SetTrackingList(gcls_pTlCoilOutSt);

	//// �r���t�@�C���Ǘ��N���X
	setlocale(LC_ALL, "Japanese");	// ���ꂪ�����Ɠ��{��p�X���܂ރt�H���_�̏ꍇ�A�ۑ��ł��Ȃ��B(���C�u�����̒��Ŏ��s����ׂ��������Ă��邪�A�ύX����ƃ^�X�N�S�̂ɉe������̂ŁA�Ƃ肠���������Őݒ�)
	CString sNameKizu;

	for (int ii = 0; ii<NUM_MEN; ii++) {
		// �rCSV�o�͸׽
		sNameKizu = GetDefectDumpName(ii);
		// (�����P��)
		//gcls_pDefDump[ii] = new DefectDumpManager(sNameKizu, SIZE_COIL_BUF, 100);
		// (�o���P��)
		gcls_pDefDump[ii] = new DefectDumpManager(ii, sNameKizu, SIZE_COIL_OUT_BUF, 100);

		gcls_pDefDump[ii]->SetLogMgr(mcls_pLog);

	}

	//// �V�[�P���T�p�ʐM����N���X(�ʏ�`��)

	//// ����˓����ԒʐM�Ǘ��N���X (0:�\,1:��)
	for (int ii = 0; ii<NUM_MEN; ii++) {
#ifdef ENABLE_CYCLE
		if(0==ii)	cWk = INI_CY_RECV_TOP;
		else		cWk = INI_CY_RECV_BOT;
		if(ii==gcls_pParam->GetParamCommon().nDistPosition) bWk = true;
		else												bWk = false;

		gcls_pCyRecv[ii] = new CyRecv(cWk, ii, bWk);
		gcls_pCyRecv[ii]->SetLogMgr(mcls_pLog);
		gcls_pCyRecv[ii]->SetLogMgrD(mcls_pLogD);
		gcls_pCyRecv[ii]->SetLockMgr(&gcls_Lock);
		gcls_pCyRecv[ii]->SetStatusMgr(&gcls_Status);
		gcls_pCyRecv[ii]->SetParamMgr(gcls_pParam);
		gcls_pCyRecv[ii]->SetCoilMgr(gcls_pCoil);
		gcls_pCyRecv[ii]->SetPlgMgr(&gcls_Plg);
		gcls_pCyRecv[ii]->SetDbMgr(&gcls_DB);

		gcls_pCyRecv[ii]->SetQueAddSQL(&gcls_DB.gque_AddSQLData);
		gcls_pCyRecv[ii]->SetDumpMgr(gcls_pDefDump[ii]);
		gcls_pCyRecv[ii]->SetTrFrmMgr(&gcls_TrFrm);

#endif

		if(0==ii)	cWk = INI_HT_RECV_TOP;
		else		cWk = INI_HT_RECV_BOT;
		if(ii==gcls_pParam->GetParamCommon().nDistPosition) bWk = true;
		else												bWk = false;

		gcls_pHtRecv[ii] = new HtRecv(cWk, ii, bWk);
		gcls_pHtRecv[ii]->SetLogMgr(mcls_pLog);
		//gcls_pHtRecv[ii]->SetLogMMgr(mcls_pLogMark);
		gcls_pHtRecv[ii]->SetLogMgrD(mcls_pLogD);
		gcls_pHtRecv[ii]->SetLogMgrA(mcls_pLogA);
		gcls_pHtRecv[ii]->SetLockMgr(&gcls_Lock);
		gcls_pHtRecv[ii]->SetStatusMgr(&gcls_Status);
		gcls_pHtRecv[ii]->SetParamMgr(gcls_pParam);
		gcls_pHtRecv[ii]->SetCoilMgr(gcls_pCoil);
		gcls_pHtRecv[ii]->SetPlgMgr(&gcls_Plg);
		gcls_pHtRecv[ii]->SetDbMgr(&gcls_DB);
#ifndef DISABLE_GAP_CALC
		gcls_pHtRecv[ii]->SetGapInfMgr(gcls_pGap); 
#endif
#ifndef DISABLE_TINY_CALC
		gcls_pHtRecv[ii]->SetTinyMgr(gcls_pTiny[ii]); 
#endif
		gcls_pHtRecv[ii]->SetEdgePosCont(&gcls_EdgePosCont); 

		//gcls_pHtRecv[ii]->SetCountMgr(gcls_pCount[ii]);


		//gcls_pHtRecv[ii]->SetMarkMgr(gcls_pMark[ii]);

		gcls_pHtRecv[ii]->SetQueAddSQL(&gcls_DB.gque_AddSQLData);
		gcls_pHtRecv[ii]->SetQueDmem(&gcls_Dmem.mque_AddData);
		gcls_pHtRecv[ii]->SetDumpMgr(gcls_pDefDump[ii]);
		gcls_pHtRecv[ii]->SetOneFrameLen( gcls_pParam->CalcResV(DIV_PLG_LINE)*SIZE_FRAME_IMG_Y );
		gcls_pHtRecv[ii]->SetTrFrmMgr(&gcls_TrFrm);
		gcls_pHtRecv[ii]->SetArmMgr(&gcls_Arm);

#ifdef ENABLE_CYCLE
		gcls_pHtRecv[ii]->SetQueCycleData(&gcls_pCyRecv[ii]->gque_Deli);
#endif
	}

	//// �������\�� ���M�N���X
	for(int ii=0;ii<HY_SOCK_NUM; ii++) {
		CString sWk;
		sWk.Format("%s_%d", INI_OP_SEND, ii+1);
		gcls_pOpS[ii] = new KizuOpSend(sWk);
		gcls_pOpS[ii]->SetLogMgr(mcls_pLog);
	}

	//// �������\�� ���M��񐶐����W���[��
	gcls_OpSend.SetLogMgr(mcls_pLog);
	for(int ii=0; ii<HY_SOCK_NUM; ii++)gcls_OpSend.SetQueOpSend(&gcls_pOpS[ii]->gque_Deli, ii);
	gcls_OpSend.SetCsLockCoil(gcls_pCoil->GetCsLock());
	gcls_OpSend.SetStatusMgr(&gcls_Status);
	gcls_OpSend.SetParamMgr(gcls_pParam);
	gcls_OpSend.SetCoilMgr(gcls_pCoil);
	gcls_OpSend.SetPlgMgr(&gcls_Plg);
	for(int ii=0; ii<NUM_MEN; ii++)	gcls_OpSend.SetHtRecv(ii, gcls_pHtRecv[ii]);
	
	//// �g���b�L���O�Ǘ��N���X (�K��PLG�N���X�̏�������)
	gcls_pTrKenDistPos = new Tracking(gcls_pParam->GetCommonMast().Dist.nKenBase, gcls_Plg.Get_pLenKenDist(), &m_evTr_Ken);

	//// iPortViewer�R�}���h��t�N���X
	gcls_IPort.SetLogMgr(mcls_pLog);
	gcls_IPort.SetStatusMgr(&gcls_Status);

	// �J���������N�X�C�b�`�p �V���A���ʐM���x�ύX�N���X
	gcls_CamLinkSpdMgr.SetLogMgr(mcls_pLog);
	gcls_CamLinkSpdMgr.SetStatusMgr(&gcls_Status);

#ifdef ENABLE_RECORDER_UDP
	gcls_CmdSender.SetLogMgr(mcls_pLog);
#endif

	Sleep(100);

	////////////////////////////////////
	//// ���[�J�[�X���b�h �N��
	////////////////////////////////////
	gcls_pParam->Start();
	gcls_DB.Start();
	gcls_Mae.Start();
	gcls_pCoil->Start();

	gcls_pSogyo->Start();
	gcls_Plg.Start(THREAD_PRIORITY_TIME_CRITICAL);
	gcls_Arm.Start();

#ifndef DISABLE_TINY_CALC
	for (int ii = 0; ii<NUM_MEN; ii++)	gcls_pTiny[ii]->Start();
#endif
	for (int ii = 0; ii<NUM_MEN; ii++)	gcls_pHtRecv[ii]->Start();
#ifdef ENABLE_CYCLE
	for (int ii = 0; ii<NUM_MEN; ii++)	gcls_pCyRecv[ii]->Start();
#endif

	for (int ii = 0; ii<HY_SOCK_NUM; ii++)gcls_pOpS[ii]->Start();
	gcls_pSeqFrameR->Start();

	gcls_Dmem.Start();

	gcls_EdgeRestSd.Start();

	gcls_pTrKenDistPos->Start();

	gcls_IPort.Start();

	for (int ii = 0; ii<NUM_MEN; ii++) {
		gcls_pDefDump[ii]->Start();
	}

	gcls_CamLinkSpdMgr.Start();

#ifdef ENABLE_RECORDER_UDP
	gcls_CmdSender.Start();
#endif

	// ���X���b�h�������ƋN������܂ł�����Ƒ҂�
	Sleep(50);

	return TRUE;
}

//------------------------------------------
// �I������ (��x�̂݋N��)
//------------------------------------------
void MainInstance::Exit()
{

	//// ���[�J�[�X���b�h ��~
#ifndef DISABLE_TINY_CALC
	for (int ii = 0; ii<NUM_MEN; ii++)		gcls_pTiny[ii]->Stop();
#endif
	for (int ii = 0; ii<NUM_MEN; ii++)		gcls_pHtRecv[ii]->Stop();
#ifdef ENABLE_CYCLE
	for (int ii = 0; ii<NUM_MEN; ii++)		gcls_pCyRecv[ii]->Stop();
#endif
	gcls_pCoil->Stop();
	for (int ii = 0; ii<HY_SOCK_NUM; ii++)gcls_pOpS[ii]->Stop();
	gcls_pSeqFrameR->Stop();
	gcls_pSogyo->Stop();

	gcls_pTrKenDistPos->Stop();
	gcls_pTlCoilOutSt->QueAllFree();
	gcls_pTlCoilOutSt->Stop();
	gcls_pParam->Stop();


	//// �P��N���X�̒�~
	gcls_Arm.Stop();
	gcls_Plg.Stop();
	gcls_Mae.Stop();
	gcls_DB.Stop();
	gcls_DB.Free();
	gcls_Status.Stop();
	gcls_EdgeRestSd.Stop();
	gcls_Dmem.Stop();
	gcls_IPort.Stop();

	for (int ii = 0; ii<NUM_MEN; ii++) {
		gcls_pDefDump[ii]->Stop();
	}

	gcls_CamLinkSpdMgr.Stop();

#ifdef ENABLE_RECORDER_UDP
	gcls_CmdSender.Stop();
#endif

	//// ���[�J�[�X���b�h �J��
#ifndef DISABLE_TINY_CALC
	for (int ii = 0; ii<NUM_MEN; ii++) delete gcls_pTiny[ii];
#endif
	for (int ii = 0; ii<NUM_MEN; ii++) delete gcls_pHtRecv[ii];
#ifdef ENABLE_CYCLE
	for (int ii = 0; ii<NUM_MEN; ii++) delete gcls_pCyRecv[ii];
#endif
	for (int ii = 0; ii<MAX_TR_FRAME; ii++) gcls_TrFrm.DelAll(ii);
	gcls_TrFrm.Free();

	delete gcls_pCoil;
	for (int ii = 0; ii<HY_SOCK_NUM; ii++)delete gcls_pOpS[ii];
	delete gcls_pSeqFrameR;
	delete gcls_pSogyo;

	delete gcls_pTrKenDistPos;
	delete gcls_pTlCoilOutSt;
	delete gcls_pParam;
	delete gcls_pStatusFunc;

	//// ���O�֌W
	for (int ii = 0; ii<NUM_MEN; ii++) {
		delete gcls_pDefDump[ii];
	}
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int MainInstance::ThreadFirst()
{
	////// �����[�N�X���b�h�̏����������܂őҋ@ (MainInstance�̂�)
	LOG(em_MSG), "[%s] �����[�N�X���b�h�̏����������܂őҋ@���E�E�E [0x%X]", my_sThreadName, GetTid() );
	WaitForSingleObject(m_evThStart, INFINITE);


	//////////////////////////////////////////////////////////////////////
	// �����Ǘ��̏����������ł����̂ŁA�����J�n

	//// �X�e�[�^�X������
	for(int ii=0; ii<NUM_MEN; ii++){
		gcls_Status.SetSys(ii, DIV_SYS_IDLE);
#ifdef ENABLE_CYCLE
		gcls_Status.SetCycleSys(ii, DIV_SYS_IDLE);
#endif
	}
	// ----------------------
	// �p�������@�\�V�X�e����ԕύX
	for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
	{
		gcls_Status.SetParaSys(ii, DIV_SYS_IDLE);
	}
	gcls_Status.SetKadou( DIV_KADOU_STOP );
	gcls_Status.SetUnten( DIV_UNTEN_NOMAL );
	gcls_Status.SetPlg( DIV_PLG_LINE );

	//// �����Ԗ⍇��
	Send_HtDefect_Status();

	//// �O���֎����ݒ�
	ToOutMailSender::Send_KsMaster_TimeSet();

#ifndef DISABLE_AUTOSTART
	//// �����T���J�n
	__int64 ts = (__int64)300 * 1000 * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evTimeReStart, (LARGE_INTEGER*)&ts, 0, NULL, NULL, FALSE);
	LOG(em_WAR), "[%s] �����T���J�n�܂Ŏb�����҂��������B(��%d�b)", my_sThreadName, ts / (1000 * -10000));
#endif

	//// �C�x���g�ݒ�
	HANDLE hArray[] = { 
						m_evTimeReStart,						// ���������ċN���^�C�}�[
						m_evTimeInitStart,						// �J���������N�X�C�b�`�ݒ�A���T�[�҂��^�C���A�E�g
						m_evTimeWaitStart,						// �����J�n�^�C���A�E�g
						LogicParaRunFunc::GetEvTimeCamset(),	// �J�����I��ݒ�A���T�[�҂��^�C���A�E�g
						LogicParaRunFunc::GetEvTimeResumeStart(),// �r���Q���ݒ�A���T�[�҂��^�C���A�E�g
						LogicParaRunFunc::GetEvTimeResumeWait(0),			// �r���J�n����A���T�[�҂��^�C���A�E�g
						LogicParaRunFunc::GetEvTimeResumeWait(1),			// �r���J�n����A���T�[�҂��^�C���A�E�g
						//m_evTimeJoinStart[0],					// �S������ �r���Q���^�C���A�E�g(�\)
						//m_evTimeJoinStart[1],					// �S������ �r���Q���^�C���A�E�g(��)

						gcls_Dmem.GetEvPI_WPD_KenBaseGateOn(),	// ������_WPD �Q�[�g�M��ON
						gcls_Dmem.GetEvPI_WPD_KenBase(),		// ������_WPD
						gcls_Dmem.GetEvPI_WPD_KenBaseGateOff(),	// ������_WPD �Q�[�g�M��OFF
						gcls_pCoil->GetNextCoilKen_Recv(),	// �����p ���R�C������M����

						gcls_Dmem.GetEvPI_WPD_DspBaseGateOn(),	// �\����_WPD �Q�[�g�M��ON
						gcls_Dmem.GetEvPI_WPD_DspBase(),		// �\����_WPD
						gcls_Dmem.GetEvPI_WPD_DspBaseGateOff(),	// �\����_WPD �Q�[�g�M��OFF

						gcls_Dmem.GetEvPI_WPD_ShacutF(),		// �t�@�[�X�g�J�b�g�M�� ON
						gcls_Dmem.GetEvPI_WPD_ShacutL(),		// ���X�g�J�b�g�M�� ON
						gcls_Dmem.GetEvPI_WPD_PatoReset(),		// �p�g���C�g�r���Z�b�g�M�� ON


						gcls_pTrKenDistPos->GetEv_TrEnd(),		// ��ʌ������u�ʒu�ɗn�ړ_/�َ�_���B

						gcls_Plg.GetEvPLG_KenNotPos(),			// ��ʂƈقȂ���̌������u�ʒu�ɗn�ړ_���B
						
						gcls_pOpS[0]->GetEvConnect(),			// �ʐM��Ԋ֘A
						gcls_pOpS[1]->GetEvConnect(),			// �ʐM��Ԋ֘A
						gcls_pOpS[2]->GetEvConnect(),			// �ʐM��Ԋ֘A

						gcls_pHtRecv[0]->GetEvErrToClose(),		// �ُ폈���֌W
						gcls_pHtRecv[1]->GetEvErrToClose(),

						gcls_Status.GetQueMyErrWar().g_evSem,	// �G���[�ʒm�L���[

						gcls_pParam->GetEvPrm_ReadEnd(),		// �p�����[�^�Ǎ�����

						mque_DeliMail.g_evSem					// ���[���X���b�g�L���[
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);

	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void MainInstance::ThreadEvent(int nEventNo)
{
	DELIVERY_MAIL* pMail;

	////// �V�O�i���̏���
	enum {
			EV_RESTART = 0,					// ���������ċN���^�C�}�[
			EV_WAIT_INIT_START,				// �J���������N�X�C�b�`�ݒ�A���T�[�҂��^�C���A�E�g
			EV_DETECT_WAIT_START,			// �����J�n ����A���T�[�҂��^�C���A�E�g
			EV_WAIT_CAMSET,					// �J���������N�X�C�b�`�ݒ�A���T�[�҂��^�C���A�E�g
			EV_WAIT_RESUME_START,			// �r���Q���ݒ�A���T�[�҂��^�C���A�E�g
			EV_WAIT_RESUME_WAIT1,			// �r���J�n����A���T�[�҂��^�C���A�E�g
			EV_WAIT_RESUME_WAIT2,			// �r���J�n����A���T�[�҂��^�C���A�E�g
			//EV_RPI_JOIN_START_TOP,			// �S������ �r���Q���^�C���A�E�g(�\)
			//EV_RPI_JOIN_START_BOT,			// �S������ �r���Q���^�C���A�E�g(��)


			EV_PI_WPD_KEN_BASE_GATE,		// ������_WPD �Q�[�g�M��ON
			EV_PI_WPD_KEN_BASE,				// ������_WPD
			EV_PI_WPD_KEN_BASE_GATE_OFF,	// ������_WPD �Q�[�g�M��OFF
			EV_PI_NEXT_KEN_COILINF_RECV,	// �����p���R�C������M����

			EV_PI_WPD_DSP_BASE_GATE,		// �\����_WPD �Q�[�g�M��ON
			EV_PI_WPD_DSP_BASE,				// �\����_WPD
			EV_PI_WPD_DSP_BASE_GATE_OFF,	// �\����_WPD �Q�[�g�M��OFF

			EV_PI_SHACUT_F,					// �t�@�[�X�g�J�b�g�M�� ON
			EV_PI_SHACUT_L,					// ���X�g�J�b�g�M�� ON
			EV_PI_PATO_RESET,				// �p�g���C�g�r���Z�b�g�M�� ON

			EV_TR_KEN_BASE,					// �����ʒu�ɗn�ړ_/�َ�_���B

			EV_TR_KEN_NOTPOS,				// ��ʂƈقȂ���̌������u�ʒu�ɗn�ړ_���B

			EV_OP_CONNECT_1,				// �I�y���[�^�@�\���ڑ��L��
			EV_OP_CONNECT_2,				// �I�y���[�^�@�\���ڑ��L��
			EV_OP_CONNECT_3,				// �I�y���[�^�@�\���ڑ��L��

			EV_HT_T_ERRCLOSE,				// ���� �\�� �ُ�ؒf
			EV_HT_B_ERRCLOSE,				// ���� ���� �ُ�ؒf

			EV_OTHER_ONERROR,				// ��������� �ُ픭��
			EV_PARAM_READ_END,				// �p�����[�^�ǂݍ��݊����ʒm

			EV_MAIL,						// ���[���X���b�g �ʒm (���[���͈�ԍŌ�ɂ��悤����)
	};

	////// �V�O�i����������
	switch (nEventNo) {
//-----------------------------------------------------------------------------------------------
	case EV_RESTART:									// ���������ċN���^�C�}�[
		// ���Ɍ������Ȃ疳����
		if( ! gcls_Status.IsRun() ) {
			LOG(em_ERR), "[%s] ���������ċN���J�n", my_sThreadName);
			syslog(232, "");
			// �f�[�^����
			COMMON_QUE que;
			que.fl.data[0] = gcls_Status.GetUnten();
			que.fl.data[1] = gcls_Status.GetPlg();
			que.fl.data[2] = gcls_Status.GetMenMode();
			que.fl.data[3] = gcls_pCoil->GetInitScode(0);
			que.fl.data[4] = gcls_pCoil->GetInitScode(1);
			que.fl.data[5] = gcls_pCoil->GetInitPcode(0);
			que.fl.data[6] = gcls_pCoil->GetInitPcode(1);
			que.fl.data[7] = LogicParaRunFunc::GetHtParaCamNo(0, 0);	// ���W�b�N�p�������@�\01 ���̓J����01
			que.fl.data[8] = LogicParaRunFunc::GetHtParaCamNo(0, 1);	// ���W�b�N�p�������@�\01 ���̓J����02
			que.fl.data[9] = LogicParaRunFunc::GetHtParaCamNo(1, 0);	// ���W�b�N�p�������@�\02 ���̓J����01
			que.fl.data[10] = LogicParaRunFunc::GetHtParaCamNo(1, 1);	// ���W�b�N�p�������@�\02 ���̓J����02

			// �����J�n�ʒm�𑗂�
			SetDeliveryMail(E_DEF_ML_START, &que);
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_WAIT_INIT_START:							// �J���������N�X�C�b�`�ݒ�A���T�[�҂��^�C���A�E�g
		LOG(em_ERR), "[%s] �J���������N�X�C�b�`�ݒ�A���T�[�҂��^�C���A�E�g�����I�I", my_sThreadName);

		// �R�}���h���X�g�N���A
		LogicParaRunFunc::ClearLinkswCmd();

		// �����J�n�������҂�����(�^�C���A�E�g����)
		WaitInit_TimeOut();
		break;

//-----------------------------------------------------------------------------------------------
	case EV_DETECT_WAIT_START:							// �����J�n ����A���T�[�҂��^�C���A�E�g
		LOG(em_ERR), "[%s] �����J�n�ݒ�A���T�[�^�C���A�E�g�����I", my_sThreadName);
		WaitStart_TimeOut();
		break;

//-----------------------------------------------------------------------------------------------
	case EV_WAIT_CAMSET:								// �J�����I��ݒ�A���T�[�҂��^�C���A�E�g
		LOG(em_ERR), "[%s] �J�����I��ݒ�A���T�[�҂��^�C���A�E�g�����I�I", my_sThreadName);
		LogicParaRunFunc::WaitCamset_TimeOut();
		break;

//-----------------------------------------------------------------------------------------------
	case EV_WAIT_RESUME_START:							// �r���Q���ݒ�A���T�[�҂��^�C���A�E�g
		LOG(em_ERR), "[%s] �r���Q���ݒ�A���T�[�҂��^�C���A�E�g�����I�I", my_sThreadName);
		LogicParaRunFunc::WaitResumeStart_Timeout();
		break;

//-----------------------------------------------------------------------------------------------
	case EV_WAIT_RESUME_WAIT1:							// �r���J�n����A���T�[�҂��^�C���A�E�g
	case EV_WAIT_RESUME_WAIT2:							// �r���J�n����A���T�[�҂��^�C���A�E�g
		if (true) {
			int nKind = nEventNo - EV_WAIT_RESUME_WAIT1;		// �����J�n�敪�i0�F���W�b�N�p�������@�\1�A1�F���W�b�N�p�������@�\2�j
			LOG(em_ERR), "[%s] �r���J�n����A���T�[�҂��^�C���A�E�g�����I�I ���W�b�N�p�������@�\%d", my_sThreadName, nKind + 1);
			LogicParaRunFunc::WaitResumeWait_Timeout(nKind);
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_PI_WPD_KEN_BASE_GATE:						// ������_WPD �Q�[�g�M��ON
		if(true){
			// �t���O��ON
			m_bIsWpdGateTrKen = true;

			if( ! gcls_Status.IsDoSequence() )	break;			// �V�[�P���X���s�\�H

			// �A��ON�͖���
			if( gcls_Plg.IsCoilChange_Ken() ){
				LOG(em_ERR), "[%s] ������_WPD �Q�[�g�M��ON [%0.3fm] <�A����ON�ɂ�薳��>", my_sThreadName, gcls_Plg.GetLen_KenDist()/1000.0 );
				syslog(267, "[�����p�F�A������ <%0.3fm�����ō�ON>]", gcls_Plg.GetLen_KenDist()/1000.0);
				break;
			}

#ifndef LOCAL
			// �����J�n��{�ځA�����ĒT����{�ځA�R�C����������B��̃R�C���͏��R�C�����肵�Ȃ�
			if( (0 == gcls_Status.GetStatusRead().Coil_Init.stat &&
				 0 == gcls_Status.GetStatusRead().Coil_ReStart.stat &&
				 0 == gcls_Status.GetStatusRead().MaxLen_Ken.stat ) && 
				COIL_LEN_MIN*1000 > gcls_Plg.GetLen_KenDist() + gcls_pParam->GetCommonMast().Dist.nKenBase ) {
				LOG(em_ERR), "[%s] ������_WPD �Q�[�g�M��ON [%0.3fm] <���R�C�������ɂ�薳��>", my_sThreadName, gcls_Plg.GetLen_KenDist()/1000.0 );
				syslog(267, "[�����p�F���R�C������ <%0.3fm>]", gcls_Plg.GetLen_KenDist()/1000.0);
				break;
			}
#endif

			LOG(em_MSG), "[%s] ������_WPD �Q�[�g�M��ON [%0.3fm]", my_sThreadName, gcls_Plg.GetLen_KenDist()/1000.0);
			syslog(290, "[�����p:������_WPD �Q�[�g�M��ON: %0.3fm]", gcls_Plg.GetLen_KenDist() / 1000.0);
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_PI_WPD_KEN_BASE:							// ������_WPD
		if(true){
			if( ! gcls_Status.IsDoSequence() )	break;			// �V�[�P���X���s�\�H

			// �A��ON�͖���
			if( gcls_pTrKenDistPos->IsRunning() || gcls_Plg.IsCoilChange_Ken()){
				LOG(em_ERR), "[%s] ������_WPD ON [%0.3fm] <�A����ON�ɂ�薳��> Chg:%d", my_sThreadName, gcls_Plg.GetLen_KenDist()/1000.0, gcls_Plg.IsCoilChange_Ken() );
				syslog(267, "[�����p�FWPD �A������ <%0.3fm�����ō�ON>]", gcls_Plg.GetLen_KenDist()/1000.0);
				break;
			}
			// �Q�[�g�M��OFF�͖���
#ifndef PIO_DEBUG
#ifdef ENABLE_GATE_KEN
			if( !gcls_Dmem.GetPI_KenGateYou() ) {
				LOG(em_ERR), "[%s] ������_WPD ON [%0.3fm] <�Q�[�g�M��OFF�ɂ�薳��>", my_sThreadName, gcls_Plg.GetLen_KenDist()/1000.0 );
				syslog(267, "[�����p�F�Q�[�g�M��OFF <%0.3fm��ON>]", gcls_Plg.GetLen_KenDist()/1000.0);
				break;
			}
#endif
#endif

#ifndef LOCAL
			// �����J�n��{�ځA�����ĒT����{�ځA�R�C����������B��̃R�C���͏��R�C�����肵�Ȃ�
			if( (0 == gcls_Status.GetStatusRead().Coil_Init.stat &&
				 0 == gcls_Status.GetStatusRead().Coil_ReStart.stat &&
				 0 == gcls_Status.GetStatusRead().MaxLen_Ken.stat ) && 
				COIL_LEN_MIN*1000 > gcls_Plg.GetLen_KenDist() + gcls_pParam->GetCommonMast().Dist.nKenBase) {
				LOG(em_ERR), "[%s] ������_WPD ON [%0.3fm] <���R�C�������ɂ�薳��>", my_sThreadName, gcls_Plg.GetLen_KenDist()/1000.0 );
				syslog(267, "[�����p�F���R�C������ <%0.3fm>]", gcls_Plg.GetLen_KenDist()/1000.0);
				break;
			}
#endif

			// �t���O��OFF
			m_bIsWpdGateTrKen = false;

			// �����pWPD�F��
			double dSpmDiff = 0.0;
			WPD_KenBase(0, &dSpmDiff);

			LOG(em_MSG), "[%s] ������_WPD ON ��=%0.3fm��%0.3fm [%0.3fm��Ɋ�ʓ��B] SPM�L��=%.4f DmFrameNo=%d PlgFrameNo=%d LineSpeed=%d", my_sThreadName,
				(double)gcls_Plg.GetLen_KenDist() / 1000.0,
				((double)gcls_Plg.GetLen_KenDist() + (double)gcls_pParam->GetCommonMast().Dist.nKenBase + dSpmDiff) / 1000.0,
				((double)gcls_pParam->GetCommonMast().Dist.nKenBase + dSpmDiff) / 1000.0,
				gcls_pCoil->GetLastSpm(),
				gcls_Dmem.GetDmRecv().ken.nWpdOnFrameNo,
				gcls_Plg.GetFrameNo(),
				gcls_pSogyo->GetSpeed());
			syslog(265, "[������_WPD: ��=%0.3fm��%0.3fm [%0.3fm��Ɋ�ʓ��B] SPM�L��=%.4f] DmFrameNo=,%d, PlgFrameNo=,%d, LineSpeed=,%d",
				(double)gcls_Plg.GetLen_KenDist() / 1000.0,
				((double)gcls_Plg.GetLen_KenDist() + (double)gcls_pParam->GetCommonMast().Dist.nKenBase + dSpmDiff) / 1000.0,
				((double)gcls_pParam->GetCommonMast().Dist.nKenBase + dSpmDiff) / 1000.0,
				gcls_pCoil->GetLastSpm(),
				gcls_Dmem.GetDmRecv().ken.nWpdOnFrameNo,
				gcls_Plg.GetFrameNo(),
				gcls_pSogyo->GetSpeed());
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_PI_WPD_KEN_BASE_GATE_OFF:					// ������_WPD �Q�[�g�M��OFF
		if(true){
			// �t���O��OFF
			bool bWk = m_bIsWpdGateTrKen;
			m_bIsWpdGateTrKen = false;


#ifdef ENABLE_GATE_KEN
			if( ! gcls_Status.IsDoSequence() )	break;			// �V�[�P���X���s�\�H
			syslog(290, "[�����p:������_WPD �Q�[�g�M��OFF: %0.3fm]", gcls_Plg.GetLen_KenDist()/1000.0);

			// ���Ɍ�����_WPD ON���Ă���Ζ���
			//if( gcls_pTrKenDistPos->IsRunning() ){
			if (!bWk) {
				LOG(em_INF), "[%s] ������_WPD�`�Q�[�g�M��OFF [%0.3fm] <������_WPD���o�ςɂ�����WPD�o�b�N�A�b�v�s�v>", my_sThreadName, gcls_Plg.GetLen_KenDist()/1000.0 );
				break;
			}

			// �����pWPD�F��
			double dSpmDiff = 0.0;
			WPD_KenBase(2, &dSpmDiff);

			LOG(em_MSG), "[%s] ������_WPD(GATE) OFF <����WPD�o�b�N�A�b�v> ��=%0.3fm��%0.3fm [%0.3fm��Ɋ�ʓ��B] SPM�L��=%.4f DmFrameNo=%d PlgFrameNo=%d LineSpeed=%d", my_sThreadName,
				(double)gcls_Plg.GetLen_KenDist() / 1000.0,
				((double)gcls_Plg.GetLen_KenDist() + (double)gcls_pParam->GetCommonMast().Dist.nKenBase + dSpmDiff) / 1000.0,
				((double)gcls_pParam->GetCommonMast().Dist.nKenBase + dSpmDiff) / 1000.0,
				gcls_pCoil->GetLastSpm(),
				gcls_Dmem.GetDmRecv().ken.nFrameNo,
				gcls_Plg.GetFrameNo(),
				gcls_pSogyo->GetSpeed());
			syslog(269, "[������_WPD�`�Q�[�g�M��OFF <����WPD�o�b�N�A�b�v> ��=%0.3fm��%0.3fm [%0.3fm��Ɋ�ʓ��B] SPM�L��=%.4f DmFrameNo=,%d, PlgFrameNo=,%d, LineSpeed=,%d,]",
				(double)gcls_Plg.GetLen_KenDist() / 1000.0,
				((double)gcls_Plg.GetLen_KenDist() + (double)gcls_pParam->GetCommonMast().Dist.nKenBase + dSpmDiff) / 1000.0,
				((double)gcls_pParam->GetCommonMast().Dist.nKenBase + dSpmDiff) / 1000.0,
				gcls_pCoil->GetLastSpm(),
				gcls_Dmem.GetDmRecv().ken.nFrameNo,
				gcls_Plg.GetFrameNo(),
				gcls_pSogyo->GetSpeed());
#else
			LOG(em_MSG), "[%s] ������_WPD(GATE) OFF ��:%0.3fm", my_sThreadName, gcls_Plg.GetLen_KenDist() / 1000.0);
			syslog(290, "[�����p:������_WPD �Q�[�g�M��OFF: %0.3fm]", gcls_Plg.GetLen_KenDist() / 1000.0);
#endif
		}
		break;
		
//-----------------------------------------------------------------------------------------------
	case EV_PI_NEXT_KEN_COILINF_RECV:					// �����p���R�C������M����
		if(true) {
			LOG(em_MSG), "[%s] �����p���R�C������M����", my_sThreadName);
			SetParam_NextCoilInfRecv();
			// ���Ƀg���b�L���O���̏ꍇ�͕\��PC�Ɍ����p�R�C�����𑗐M����
			if( gcls_pTrKenDistPos->IsRunning() || gcls_Plg.IsCoilChange_Ken()){
				if(!gcls_pCoil->IsCoilNextKen()) break;
				////// �R�C����� ��\��PC�ɑ��� 
				//// �� �]���́A�O�H������O�����Ď擾���邽�߂ɃR�C������M���ɑ��M���Ă������A
				//		�����Ǘ�No���R�C������M���ɍX�V����������WPD ON���R�C������M�ςݎ��ɑ��M����
				OpSendFunc::SendOp_CoilInf_Ken( gcls_pCoil->GetCoilNextKen() );
			}
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_PI_WPD_DSP_BASE_GATE:						// �\����_WPD �Q�[�g�M��ON
		if(true){
			// �t���O��ON
			m_bIsWpdGateTrDsp = true;

			if( ! gcls_Status.IsDoSequence() )	break;			// �V�[�P���X���s�\�H

			// �A��ON�͖���
			if( gcls_Plg.IsCoilChange_Dsp() ){
				LOG(em_ERR), "[%s] �\����_WPD �Q�[�g�M��ON [%0.3fm] <�A����ON�ɂ�薳��>", my_sThreadName, gcls_Plg.GetLen_BaseDsp()/1000.0 );
				syslog(292, "[�\���p�F�A������ <%0.3fm�����ō�ON>]", gcls_Plg.GetLen_BaseDsp()/1000.0);
				break;
			}

// �\���͏��R�C������Ȃ�
//#ifndef LOCAL
//			// �����J�n��{�ځA�����ĒT����{�ځA�R�C����������B��̃R�C���͏��R�C�����肵�Ȃ�
//			if( (0 == gcls_Status.GetStatusRead().Coil_Init.stat &&
//				 0 == gcls_Status.GetStatusRead().Coil_ReStart.stat &&
//				 0 == gcls_Status.GetStatusRead().MaxLen_Dsp.stat ) && 
//				//COIL_LEN_MIN*1000 > gcls_Plg.GetLen_BaseDsp() ) {
//				LOG(em_ERR), "[%s] ������_WPD �Q�[�g�M��ON [%0.3fm] <���R�C�������ɂ�薳��>", my_sThreadName, gcls_Plg.GetLen_BaseDsp()/1000.0 );
//				syslog(268, "[�\���p�F���R�C������ <%0.3fm>]", gcls_Plg.GetLen_BaseDsp()/1000.0);
//				break;
//			}
//#endif
			LOG(em_MSG), "[%s] �\����_WPD �Q�[�g�M��ON [%0.3fm]", my_sThreadName, gcls_Plg.GetLen_BaseDsp()/1000.0);
			syslog(292, "[�\���p:�\����_WPD �Q�[�g�M��ON: %0.3fm]", gcls_Plg.GetLen_BaseDsp() / 1000.0);
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_PI_WPD_DSP_BASE:							// �\����_WPD
		if(true) {

			if( ! gcls_Status.IsDoSequence() ) break;							// �V�[�P���X���s�\?
			
			// �Q�[�g�M��OFF�͖���
#ifndef PIO_DEBUG
#ifdef ENABLE_GATE_DSP
			if( !gcls_Dmem.GetPI_DspGateYou() ) {
				LOG(em_ERR), "[%s] �\����_WPD ON [%0.3fm] <�Q�[�g�M��OFF�ɂ�薳��>", my_sThreadName, gcls_Plg.GetLen_BaseDsp()/1000.0 );
				syslog(268, "[�\���p�F�Q�[�g�M��OFF <%0.3fm��ON>]", gcls_Plg.GetLen_BaseDsp()/1000.0);
				break;
			}
#endif
#endif

			// �t���O��OFF
			m_bIsWpdGateTrDsp = false;

			LOG(em_MSG), "[%s] �\����_WPD ON [%0.3fm]", my_sThreadName, gcls_Plg.GetLen_BaseDsp()/1000.0 );
			syslog(266, "[�\����_WPD: %0.3fm]", gcls_Plg.GetLen_BaseDsp()/1000.0 );

			// �\���pWPD�F��
			WPD_DspBase();

		}
		break;

		
//-----------------------------------------------------------------------------------------------
	case EV_PI_WPD_DSP_BASE_GATE_OFF:					// �\����_WPD �Q�[�g�M��OFF
		if(true){
			// �t���O��OFF
			bool bWk = m_bIsWpdGateTrDsp;
			m_bIsWpdGateTrDsp = false;

#ifdef ENABLE_GATE_DSP
			if (!gcls_Status.IsDoSequence())	break;			// �V�[�P���X���s�\�H
			syslog(292, "[�\���p:�\����_WPD �Q�[�g�M��OFF: %0.3fm]", gcls_Plg.GetLen_BaseDsp()/1000.0);

			// ���Ɍ�����_WPD ON���Ă���Ζ���
			//if (gcls_pTrKenDistPos->IsRunning()) {
			if (!bWk) {
				LOG(em_INF), "[%s] �\����_WPD�`�Q�[�g�M��OFF [%0.3fm] <�\����_WPD���o�ςɂ��\��WPD�o�b�N�A�b�v�s�v>", my_sThreadName, gcls_Plg.GetLen_BaseDsp() / 1000.0 );
				break;
			}
			
			// �\���pWPD�F��
			WPD_DspBase();

			int nVal = gcls_pParam->GetCommonMast().Dist.nDspBaseKensaBot;
			LOG(em_MSG), "[%s] �\����_WPD(GATE) OFF ��:%0.3fm��%0.3fm [%0.3fm��ɕ\����_�ʒu���B] <�\��WPD�o�b�N�A�b�v>", my_sThreadName, gcls_Plg.GetLen_BaseDsp() / 1000.0, (gcls_Plg.GetLen_BaseDsp() + nVal) / 1000.0, nVal / 1000.0);
			syslog(270, "[�\����_WPD�`�Q�[�g�M��OFF: %0.3fm��ɕ\����_�ʒu�ɓ��B] <�\��WPD�o�b�N�A�b�v>", nVal / 1000.0);
#else
			LOG(em_MSG), "[%s] �\����_WPD(GATE) OFF ��:%0.3fm", my_sThreadName, gcls_Plg.GetLen_BaseDsp() / 1000.0);
			syslog(292, "[�\���p:�\����_WPD �Q�[�g�M��OFF: %0.3fm]", gcls_Plg.GetLen_BaseDsp() / 1000.0);
#endif
		}
		break;
				
//-----------------------------------------------------------------------------------------------
	case EV_PI_SHACUT_F:								// �t�@�[�X�g�J�b�g�M�� ON
		if(true) {
			// ���݌������ł���Ζ���
			if( ! gcls_Status.IsRun() ) break;
			// �ʏ�^�]���[�h���V�~�����[�V�������[�h�ȊO�Ȃ疳��
			if((DIV_UNTEN_NOMAL != gcls_Status.GetUnten() && DIV_UNTEN_SMYU != gcls_Status.GetUnten()) ) break;
			
			// �����`�F�b�N
			if( NULL == gcls_pCoil->GetCoilDsp(POS_DSP_SHR_I) ) {
				LOG(em_WAR), "[MI] �V���[�J�b�g�ʒu�܂Ńg���b�L���O���B���ɁA�V���[�J�b�gON");
				break;
			}			

			// �V���[�J�b�g�F��
			if(true) {
				gcls_Lock.LockTh();	//// -------------->>>>>

				double dInLen  = gcls_Plg.GetLen_Dsp(POS_DSP_SHR_I);	// �V���[�J�b�g�ʒu�ł̓����_���R�C������
				double dOutLen = gcls_Plg.GetLen_OutCut();				// �V���[�J�b�g�ʒu�ł̏o�������R�C������
				COIL_INF const* typCoil = gcls_pCoil->GetCoilDsp(POS_DSP_SHR_I);

				// �o�������P�ʂ̋����N���A
				gcls_Plg.CoilChange_OutCut();
	
				LOG(em_MSG), "[MI] �t�@�[�X�g�J�b�g ON <%.12s> %s [�����_��=%0.3fm �o������=%0.3fm]",
					typCoil->data.cCoilNoIn, typCoil->cKizukenNo, dInLen/1000.0, dOutLen/1000.0 );
				syslog(293, "[%.12s �����_��=%0.3fm �o������=%0.3fm]", typCoil->data.cCoilNoIn, dInLen/1000.0, dOutLen/1000.0 );

				// �R�C�����ɓo�^
				gcls_pCoil->SetData_ShaCut( dInLen );

				// PLG�Ǘ��N���X�V���[�J�b�g�F��
				gcls_Plg.CoilChange_FirstCut();

				gcls_Lock.UnlockTh();	//// <<<<<--------------
			}
		}
		break;
//-----------------------------------------------------------------------------------------------
	case EV_PI_SHACUT_L:								// ���X�g�J�b�g�M�� ON
		if(true) {
			// ���݌������ł���Ζ���
			if( ! gcls_Status.IsRun() ) break;
			// �ʏ�^�]���[�h���V�~�����[�V�������[�h�ȊO�Ȃ疳��
			if((DIV_UNTEN_NOMAL != gcls_Status.GetUnten() && DIV_UNTEN_SMYU != gcls_Status.GetUnten()) ) break;

			double dInLen = gcls_Plg.GetLen_Dsp(POS_DSP_SHR_I);		// �V���[�J�b�g�ʒu�ł̓����_���R�C������
			double dOutLen = gcls_Plg.GetLen_OutCut();				// �V���[�J�b�g�ʒu�ł̏o�������R�C������
			COIL_INF const* typCoil = gcls_pCoil->GetCoilDsp(POS_DSP_SHR_I);

			LOG(em_MSG), "[MI] ���X�g�J�b�g ON <%.12s> %s [�����_��=%0.3fm �o������=%0.3fm]",
				typCoil->data.cCoilNoIn, typCoil->cKizukenNo, dInLen / 1000.0, dOutLen / 1000.0 );
		}
		break;
//-----------------------------------------------------------------------------------------------
	case EV_PI_PATO_RESET:									// PIO �p�g���C�g�r���Z�b�g�M�� ON
		gcls_Arm.Reset_Pato();
		syslog(296, "");
		break;

				
//-----------------------------------------------------------------------------------------------
	case EV_TR_KEN_BASE:								// ��ʌ������u�ʒu�ɗn�ړ_/�َ�_���B
		if(true){
			if( ! gcls_Status.IsDoSequence() )	break;			// �V�[�P���X���s�\�H

			gcls_Lock.LockTh();		//// ----------------->>>>>>



			long		nMode = (long)gcls_pTrKenDistPos->g_pAddr;		// 0:�n�ړ_ 1:�َ�_ 2:�n�ړ_(GATE)
			double	dLen  = gcls_Plg.GetLen_Ken(gcls_Plg.GetDistPos());

			CString sWk = (0==nMode?"�n�ړ_":(1==nMode?"�َ�_":"�n�ړ_�o�b�N�A�b�v"));

			LOG(em_INF),  "===========================================================>>>>");
			LOG(em_INF),  "[%s] �_���R�C�����E�_ ��ʌ������u�ʒu�ʉ߁I�I <%s> �O��R�C����[%0.3fm]", my_sThreadName, sWk, dLen/1000.0 );

			// �R�C���؂�ւ����� (���)
			ChangeCoil_Ken( gcls_Plg.GetDistPos() );

			// �R�C���ؑ֏����Z�b�g
			if(2 == nMode)	gcls_pCoil->SetData_StartWpd(1);
			gcls_pCoil->SetData_CoilCnt();

//#ifdef DSP_NO_TRACKING		// ���A���^�C���\���̏ꍇ�A�����ŕ\�����ؑ�
//			
//			WPD_DspBase();
//
//#endif

			gcls_Lock.UnlockTh();	//// <<<<<<-----------------
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_TR_KEN_NOTPOS:								// ��ʂƈقȂ���̌������u�ʒu�ɗn�ړ_���B
		if(true){
			if( ! gcls_Status.IsDoSequence() )	break;			// �V�[�P���X���s�\�H

			gcls_Lock.LockTh();		//// -------------------------->>>>


			LOG(em_INF), "[%s] --------------", my_sThreadName);
			LOG(em_INF), "[%s] �_���R�C�����E�_ ���̑��������u�ʒu�ʉ߁I�I", my_sThreadName);

			// �R�C���ؑ֐���
			ChangeCoil_Ken( gcls_Plg.GetDistNotPos() );

#ifdef DSP_NO_TRACKING		// ���A���^�C���\���̏ꍇ�A�����ŕ\�����ؑ�
			
			WPD_DspBase();

#endif



			gcls_Lock.UnlockTh();	//// <<<<--------------------------
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_OP_CONNECT_1:									// �I�y���[�^�@�\���ڑ��L��
	case EV_OP_CONNECT_2:
	case EV_OP_CONNECT_3:
		//// �ǂ�������q�������ׁA�݂�Ȃɑ��M����
		// ����Ԃ𑗐M
		OpSendFunc::SendOp_Status();
		OpSendFunc::SendOp_AlarmData(gcls_Arm.GetAlarmeData());
		
		// �ғ����̏ꍇ�́A���̃R�C�����𑗐M����
		if( gcls_Status.IsRun() && DIV_UNTEN_CAMERA != gcls_Status.GetUnten() ){
			const COIL_INF*		pCoilInf = gcls_pCoil->GetCoilBaseKen();
			OpSendFunc::SendOp_CoilInf_Ken( pCoilInf );
			MainInstance::Send_HyDefect_MaeRead(pCoilInf->cKizukenNo);
			if( pCoilInf->bDspOk ) OpSendFunc::SendOp_CoilInf_Dsp( pCoilInf );

			for (int jj = 0; jj<pCoilInf->nCutCount; jj++) {
				TO_SO_DATA_CUT  data;
				memset(&data, 0x00, sizeof(data));
				data.nMode = DIV_CUT_FCUT;
				data.nPos = pCoilInf->nCutLen[jj];
				OpSendFunc::SendOp_CutInf(pCoilInf->cKizukenNo, &data);
			}

			// �s��A�s���
			for(int ii=0; ii<NUM_MEN; ii++) {
				gcls_pHtRecv[ii]->SetEvOpNewConnect(); 
			}
			// �ʔ���
			OpSendFunc::SendOp_Length();
		}
		break;
//-----------------------------------------------------------------------------------------------
	case EV_HT_T_ERRCLOSE:								// ���� �\�� �ُ�ؒf
		LOG(em_ERR), "[%s] ���� �\�� �ُ�ؒf[%s][%s][%d]", my_sThreadName,
			DivNameManager::GetDivKadou(gcls_Status.GetKadou()), gcls_Status.GetMenModeName(gcls_Status.GetMenMode()), gcls_Status.GetSys(0) );
		if( ! gcls_Status.IsRun() ) break;
		// �Жʑ��s�\�H
		if(gcls_Status.GetMenMode() && DIV_SYS_RUN == gcls_Status.GetSys(0) ) {
			DetectRun_Cancel(0);					// �\�ʌ�����~
		} else {
			gcls_Status.SetStatusQue("HANTEI_TOP_ANS_ID", false);
			gcls_Status.RefreshStatus();			// �������f
			AllDetectStop(false, true);				// ������~���Ă��猟�������ċN����
		}
		break;

	case EV_HT_B_ERRCLOSE:										// ���� ���� �ُ�ؒf
		LOG(em_ERR), "[%s] ���� ���� �ُ�ؒf[%s][%s][%d]", my_sThreadName,
			DivNameManager::GetDivKadou(gcls_Status.GetKadou()), gcls_Status.GetMenModeName(gcls_Status.GetMenMode()), gcls_Status.GetSys(0) );
		if( ! gcls_Status.IsRun() ) break;
		// �Жʑ��s�\�H
		if(gcls_Status.GetMenMode() && DIV_SYS_RUN == gcls_Status.GetSys(1) ) {
			DetectRun_Cancel(1);					// ���ʌ�����~
		} else {
			gcls_Status.SetStatusQue("HANTEI_BOT_ANS_ID", false);
			gcls_Status.RefreshStatus();			// �������f
			AllDetectStop(false, true);				// ������~���Ă��猟�������ċN����
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_OTHER_ONERROR:										// ��������� �ُ픭��
		if(true) {
			StatusBaseManager::TYP_MY_ERRWAR_INF* pInf = gcls_Status.GetQueMyErrWar().GetFromHead();
			bool retc = gcls_Status.OnMyErrWar(pInf);
			delete pInf;

			// �d�̏�̂��߁A������~
			if( retc && gcls_Status.IsRun() ) {
				gcls_Status.RefreshStatus();			// �������f
				AllDetectStop(false, true);				// ������~���Ă��猟�������ċN����
			}
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_PARAM_READ_END:										// �p�����[�^�ǂݍ��݊����ʒm
		// ���܂ŗǂ��ق����Ȃ鎞���������̂ŁA����Ă݂�
		if(true) {
			// DB�N���X�ւ�Jpeg���k���ύX�̂�
			gcls_DB.SetJpgQuality(gcls_pParam->GetCommonMast().nJpegQuality ); 
			
			// �����֌W�̍X�V
			gcls_Plg.SetDistLen( (long)gcls_pParam->GetParamCommon().dDistLen );

		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_MAIL: 						// ���[���X���b�g �ʒm
		pMail = mque_DeliMail.GetFromHead();
		switch(pMail->emNo) {

	//-----------------------------------------------------------
		case E_DEF_ML_PARAM:							// �p�����[�^�ύX�v��
			if (true) {
				// �p�����[�^�ύX�R�}���h
				COMMON_QUE* pQue = new COMMON_QUE;
				memcpy( pQue, &pMail->que, sizeof(COMMON_QUE) );

				// SCODE����ʌ����ʒu��ʉߒ���SCODE�ɏ���
				if( CH_SCODE_NON == pQue->fl.data[2] || CH_SCODE_ALL == pQue->fl.data[2]) {
					int men = gcls_pCoil->GetDistPos();
					pQue->fl.data[2] = gcls_pCoil->GetCoilKenV( men )->setting.scode[ men ];	// ���
				}
				if (CH_SCODE_NON == pQue->fl.data[2]) pQue->fl.data[2] = MAX_SCODE;

				// �ʌŒ�ł̌����\�ʏ��
				for(int ii=0; ii<NUM_MEN; ii++) {
					pQue->fl.data[3+ii] = gcls_pCoil->GetCoilKenV( ii )->setting.scode[ ii ];		// �ʌŒ�
					if (CH_SCODE_NON == pQue->fl.data[3+ii]) pQue->fl.data[3+ii] = MAX_SCODE;
				}
								
				gcls_pParam->gque_Deli.AddToTail( pQue ); 
			}
			break;
	//-----------------------------------------------------------
		case E_DEF_ML_START:							// �����J�n�v��
			// �����Ԗ⍇��
			Send_HtDefect_Status();
			Sleep(1000);			// ���ʂ��Ԃ�܂ł�����Ƒ҂�

			// ����������������
			DetectInitStart(&pMail->que);
			break;

	//-----------------------------------------------------------
		case E_DEF_ML_STOP:								// ������~�v��
			AllDetectStop(true, false);
			break;

	//-----------------------------------------------------------
		case E_DEF_ML_WAIT_START_ANSWER:				// ����J�n�A���T�[
			WaitStart_Answer(&pMail->que);
			break;

	//-----------------------------------------------------------
		case E_DEF_ML_WAIT_STOP_ANSWER:					// �����~�A���T�[
			WaitStop_Answer(&pMail->que);
			break;

	//-----------------------------------------------------------
		case E_DEF_ML_STATE_ANSWER:						// �����Ԗ⍇���A���T�[
			HT_StatAnswer(&pMail->que);
			break;

	////-----------------------------------------------------------
	//	case E_DEF_ML_RECV_COILINF:						// �����p�R�C������M
	//		break;

	//-----------------------------------------------------------
		case E_DEF_ML_INITIAL:							// �C�j�V�����`����M
			if(true) {
				// �������ȊO�͖���
				if (!gcls_Status.IsDoSequence()) {
					LOG(em_ERR), "[MI] �������ȊO�ŃC�j�V�����`����M");
					break;
				}
				
				syslog(264, "");
				AllDetectStop(true, true);								// ������~ �� ���������ċN��
			}
			break;

	//-----------------------------------------------------------
		case E_DEF_ML_ALAMECANCEL:						// �x��o�͉����ʒm
			if( 0 == pMail->que.fl.data[0] ) {
				//// �ݔ��ُ�x����� (���한�A����܂ŗL��)
				// ���ł��L��
				// �f�[�^�������Ǘ��N���X�֒ʒm
				gcls_Arm.SetVoicErrCancel();
				LOGA(em_MSG), "[%s] �ݔ��ُ�x����� ��t�����I", my_sThreadName);
			} else {
				//// ���׌��o�x�����

				// �������̂�
				if( ! gcls_Status.IsRun() ) break;
				// �R�C�����Ǘ��N���X�֒ʒm
				gcls_pCoil->SetData_AlarmCancel(DIV_LEN_POS_DSP_TOP);
				// �p�g���C�g�o�̓N���X�֒ʒm
				if( NULL != gcls_pCoil->GetCoilDsp(0) ) {
					gcls_Arm.AlarmCancel_Pato( gcls_pCoil->GetCoilDsp(0)->cKizukenNo );
				}
			}
			break;
			
	//-----------------------------------------------------------
		case E_DEF_ML_MAE_WRITE:						// �O�H���r��񏑍��݊����ʒm		
			if (true) {
				COMMON_QUE* pQue = new COMMON_QUE;
				memcpy(pQue, &pMail->que, sizeof(COMMON_QUE));

				// �f�[�^�Z�b�g
				gcls_Mae.RecvMaeWrite(pQue->mix.cdata[0], pQue->mix.cdata[1]);
				delete pQue;
			}
			break;

	//-----------------------------------------------------------
		case E_DEF_ML_DB_ADD_SYNC:						// DB���ѕۑ��󋵖₢���킹�ʒm
			Add_Synclock(&pMail->que);
			break;
			
	//-----------------------------------------------------------
		case E_DEF_ML_LINKSW_ANSWER:					// �J���������N�X�C�b�`�ݒ芮���ʒm
			// �����������҂����� (�J���������N�X�C�b�`�ݒ芮���ʒm)
			WaitInit_Answer(&pMail->que);
			break;

	//-----------------------------------------------------------
		case E_DEF_ML_LINKSW_CAMSET_ANSWER:				// �J�����I��ݒ芮���ʒm
			// �J�����I��ݒ�A���T�[�҂��^�C�}�[�j��
			CancelWaitableTimer(LogicParaRunFunc::GetEvTimeCamset());
			LogicParaRunFunc::WaitCamset_Answer(&pMail->que);
			break;

	//-----------------------------------------------------------
		case E_DEF_ML_LINKSW_JOIN_ANSWER:				// �r���Q���ݒ芮���ʒm
			// �r���Q���ݒ�A���T�[�҂��^�C�}�[�j��
			CancelWaitableTimer(LogicParaRunFunc::GetEvTimeResumeStart());
			LogicParaRunFunc::WaitLinkswJoin_Answer(&pMail->que, gcls_pCoil);
			break;

	//-----------------------------------------------------------
		case E_DEF_ML_RESUME_START_ANSWER:				// �r���J�n�A���T�[
			LogicParaRunFunc::WaitResumeStart_Answer(&pMail->que);
			break;

	//-----------------------------------------------------------
		case E_DEF_ML_RESUME_STOP_ANSWER:				// �r����~�A���T�[
			LogicParaRunFunc::WaitResumeStop_Answer(&pMail->que);
			break;
		
	//-----------------------------------------------------------
		case E_DEF_ML_RESUME_START:						// �r���J�n�v��
			LogicParaRunFunc::ResumeStart(&pMail->que);
			break;

	//-----------------------------------------------------------
		case E_DEF_ML_RESUME_STOP:						// �r����~�v��
			LogicParaRunFunc::ResumeStop(&pMail->que);
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




//==========================================
// �S��~
//------------------------------------------
// bool bMode	: true:�����~ false:�ُ��~
// bool bRetry	: true:�����ċN������,false:����
//==========================================
void MainInstance::AllDetectStop(bool bMode, bool bRetry)
{
#ifdef ENABLE_CYCLE
	Cy_DetectStop(2, bMode);
#endif
	DetectStop(bMode);

	if( bRetry ) DetectReStart();
}


//==========================================
// ���������ċN�� �v�� (���������ċN������)
//==========================================
void MainInstance::DetectReStart()
{

	//// ���� ���������ċN�����Ȃ� ������ (�^�C�}�[����U���Z�b�g������)
	if( DIV_SYS_RETRY == gcls_Status.GetSys(0) ) return;
	LOG(em_ERR), "[%s] %d�b��ɋ��������ċN��", my_sThreadName, PARAM_TIMEOUT_RESTART / 1000);

	// ���O�܂œ��삵�Ă���^�C�}�[��j��
	CancelWaitableTimer(m_evTimeReStart);
	CancelWaitableTimer(m_evTimeInitStart);
	CancelWaitableTimer(m_evTimeWaitStart);
	CancelWaitableTimer(LogicParaRunFunc::GetEvTimeCamset());
	CancelWaitableTimer(LogicParaRunFunc::GetEvTimeResumeStart());

	//// �^�C�}�[�N��
	__int64 ts = (__int64)PARAM_TIMEOUT_RESTART * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evTimeReStart, (LARGE_INTEGER*)&ts, 0, NULL, NULL, FALSE);

	//// �X�e�[�^�X�ύX
	gcls_Status.SetKadou( DIV_KADOU_RETRY );
	for(int ii=0; ii<NUM_MEN; ii++) {
		gcls_Status.SetSys(ii, DIV_SYS_RETRY);
#ifdef ENABLE_CYCLE
		gcls_Status.SetCycleSys(ii, DIV_SYS_RETRY);
#endif
	}
	// ----------------------
	// �p�������@�\�V�X�e����ԕύX
	for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
	{
		gcls_Status.SetParaSys(ii, DIV_SYS_RETRY);
	}

	//// ����PC�Ɍ��݂̏�Ԃ�₢���킹
	Send_HtDefect_Status();
	Send_HtDefect_PcCanOk();				// �Ƃ肠��������PC�Ɏ��s�s�\�����𑗂��Ă����B�^���悯��Ύ��񕜋����Ă���͂�

	//// �I�y���[�^�@�\�֌���Ԃ𑗐M
	Send_SoGamenn_MailStatus();
	OpSendFunc::SendOp_Status(); 
}



//==========================================
// �������������� (�J���������N�X�C�b�`�ݒ芮���A���T�[�҂���)
//------------------------------------------
// COMMON_QUE *		que		: �N�����
//==========================================
void MainInstance::DetectInitStart(COMMON_QUE * que)
{
	//========================================================
	// ������
	EM_DIV_UNTEN	nUnten = (EM_DIV_UNTEN)que->fl.data[0];					// �^�]���
	EM_DIV_PLG		nPlg   = (EM_DIV_PLG)que->fl.data[1];					// PLG�敪
	bool			bMenMode = ( 0 == que->fl.data[2] ? false : true );		// �Жʌ�������/�s����(true:����)
	int				scode[2] = {que->fl.data[3], que->fl.data[4]};			// �����\�ʏ��
	int				pcode[2] = {que->fl.data[5], que->fl.data[6]};			// �����p�^�[��
	LogicParaRunFunc::SetHtParaCamNo(0, 0, que->fl.data[7]);					// ���W�b�N�p�������@�\01 ���̓J����01
	LogicParaRunFunc::SetHtParaCamNo(0, 1, que->fl.data[8]);					// ���W�b�N�p�������@�\01 ���̓J����02
	LogicParaRunFunc::SetHtParaCamNo(1, 0, que->fl.data[9]);					// ���W�b�N�p�������@�\02 ���̓J����01
	LogicParaRunFunc::SetHtParaCamNo(1, 1, que->fl.data[10]);					// ���W�b�N�p�������@�\02 ���̓J����02

	//========================================================
	// ���O�`�F�b�N
	// �@���̎��_�ł̃G���[�́A���g���C���Ȃ�
	
	//// �K����~���̂�
	for(int ii=0; ii<NUM_MEN; ii++){
		if( DIV_SYS_IDLE  != gcls_Status.GetSys(ii) &&
			DIV_SYS_STOP  != gcls_Status.GetSys(ii) &&
#ifdef ENABLE_CYCLE
			DIV_SYS_IDLE  != gcls_Status.GetCycleSys(ii) &&
			DIV_SYS_STOP  != gcls_Status.GetCycleSys(ii) &&
			DIV_SYS_RETRY != gcls_Status.GetCycleSys(ii) &&
#endif
			DIV_SYS_RETRY != gcls_Status.GetSys(ii) ){
			
			LOG(em_ERR), "[%s] <%s> [%s] ���Ɍ����������v����M", my_sThreadName, DivNameManager::GetTorB(ii), GetDivSys(gcls_Status.GetSys(ii)) );
			syslog(236, "[<%s> %s ���Ɍ����������v����M]", DivNameManager::GetTorB(ii), GetDivSys(gcls_Status.GetSys(ii)) );
			return ;
		}
	}

#ifdef CAM_ADJUST_REMOTE
	//// �J���������Ƀ����[�g�f�X�N�g�b�v�g�p
	if( DIV_UNTEN_CAMERA == nUnten ){
		gcls_Status.SetMenMode(bMenMode);
		gcls_Status.SetPlg(nPlg);
		gcls_Status.SetUnten(nUnten);
		gcls_Status.SetKadou( DIV_KADOU_START );
		for(int ii=0; ii<NUM_MEN; ii++){
			gcls_Status.SetSys(ii, DIV_SYS_RUN);

			// �_����ԂɕύX�BKT_LEDMGR�̂�
#ifdef KT_LEDMGR
			ToOutMailSender::Send_KtLedMgr_OnOff(ii, true);
			ToOutMailSender::Send_KtLedMgr_CoilChange(ii, gcls_pCoil->GetInitScode(ii));
#endif
		}


		//// �I�y���[�^�@�\�֑��M
		Send_SoGamenn_MailStatus();
		OpSendFunc::SendOp_Status();
		return;
	}
#endif

	//======================================================
	// ����PC�̊���U��J�n
	bool bOk[NUM_MEN];							// �P�����茋��(true:����, false:���s)

	if( ! gcls_Status.SelectHanteiPC_MultiGroup( bOk )){
		for(int ii=0; ii<NUM_MEN; ii++){
			if(!bOk[ii]){
				LOG(em_ERR), "[%s] <%s> ����PC�̊���U��s�\", my_sThreadName, DivNameManager::GetTorB(ii));
				syslog(238, "[<%s> ����PC�̊���U��s�\]", DivNameManager::GetTorB(ii));
			
				gcls_Status.SetSys(ii, DIV_SYS_STOP);
			}
		}
	}
	// ����ُ�^���한�A���Z�b�g
	KizuFunction::SetStatus("HANTEI_TOP_ANS_ID", bOk[0]);	// �\
	KizuFunction::SetStatus("HANTEI_BOT_ANS_ID", bOk[1]);	// ��

#ifdef ENABLE_CYCLE
	//======================================================
	// ����PC�̊���U��J�n
	bool bCycleOk[NUM_MEN];						// �������茋��(true:����, false:���s)

	if( ! gcls_Status.SelectHanteiPC_BackupGroup( DIV_CYCLE, bCycleOk )){
		for(int ii=0; ii<NUM_MEN; ii++){
			if(!bCycleOk[ii]){
				LOG(em_ERR), "[%s] <%s> ����PC�̊���U��s�\", my_sThreadName, DivNameManager::GetTorB(ii));
				syslog(238, "[<%s> ����PC�̊���U��s�\]", DivNameManager::GetTorB(ii));
			
				gcls_Status.SetCycleSys(ii, DIV_SYS_STOP);
			}
		}
	}
	// �����ُ�^���한�A���Z�b�g
	KizuFunction::SetStatus("CYCLE_TOP_ANS_ID", bCycleOk[0]);	// �\
	KizuFunction::SetStatus("CYCLE_BOT_ANS_ID", bCycleOk[1]);	// ��
#endif

	// ----------------------
	// ���W�b�N�p������PC�̃`�F�b�N
	bool bParaOk = gcls_Status.SelectHanteiPC_ResumeGroup();
	if (DIV_UNTEN_CAMERA != nUnten)
	{
		// �u�J�����Z���v���[�h�ȊO

		if (!bParaOk)
		{
			LOG(em_ERR), "[%s] ���W�b�N�p������PC��`�ُ�", my_sThreadName);
			syslog(238, "[���W�b�N�p������PC�̊���U��s�\]");

			for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
			{
				gcls_Status.SetParaSys(ii, DIV_SYS_STOP);
			}
		}
	}
	else
	{
		// �u�J�����Z���v���[�h��
		bParaOk = false;
	}

	//======================================================
	// �O���ʒm

	// �SPC�֓��������𑗐M
	ToOutMailSender::Send_KsMaster_TimeSet();

	// PC���ύX�ʒm
	ToOutMailSender::Send_KsMaster_PcChange();
	Sleep(200);									// HT_DEFECT�ɂ����킽��܂ł�����Ƒ҂�


	//======================================================
	// �����J�n�`�F�b�N

	// ���ʌ������́A��ł��G���[������� NG
	// �Жʋ����́A�����G���[�� NG
	if( ( ! bMenMode && (!bOk[0] || !bOk[1]) ) ||
		(   bMenMode && (!bOk[0] && !bOk[1]) ) )  {

		gcls_Status.SetKadou( DIV_KADOU_STOP );
		LOG(em_ERR), "[%s] �����J�n���s�I�I", my_sThreadName );
		syslog(238, "[�����J�n���s]");

		//// �I�y���[�^�@�\�ւ𑗐M
		Send_SoGamenn_MailStatus();
		OpSendFunc::SendOp_Status(); 
		return;
	}

	//======================================================
	// �J�n����

	//// �S�p�����[�^�Ǎ�
	if( ! gcls_pParam->ReadParam_All() ){
		LOG(em_ERR), "[%s] �S�p�����[�^�Ǎ����s", my_sThreadName);
	}


	//// ���^�X�N�@�����X���b�h�֒ʒm
	// �J�����������ȊO
	if( DIV_UNTEN_CAMERA != nUnten ){
		// �����M�N���X�̌����J�n�҂��ʒm
		for(int ii=0; ii<NUM_MEN; ii++){
			if(!bOk[ii]) continue;
			gcls_pHtRecv[ii]->SetEvDetectWaitStart();
#ifdef ENABLE_CYCLE
			gcls_pCyRecv[ii]->SetEvDetectWaitStart();
#endif
		}
	}

	//======================================================
	// ���N���X�̏�����
	// �R�C����񏉊���
	for(int ii=0; ii<NUM_MEN; ii++){
		gcls_pCoil->SetInitScode(ii, scode[ii]);
		gcls_pCoil->SetInitPcode(ii, pcode[ii]);
	}
	gcls_pCoil->AllDelete();
	// �G�b�W���o����
	gcls_EdgePosCont.Init();
	if( DIV_UNTEN_CAMERA == nUnten || DIV_UNTEN_SAMP == nUnten) gcls_EdgePosCont.SetOldMode(true);	// �J�����������́A��������������Ȃ�
	else														gcls_EdgePosCont.SetOldMode(false);


	//======================================================
	// ���^�X�N�Ɍ����J�n�ʒm�𑗐M

	//// �J�����R���g���[���Ɍ����J�n�ʒm
	ToOutMailSender::Send_ToCamera_Palse(false);				// ���������́A�p���X�o�͒�~�̂�
	ToOutMailSender::Send_ToCamera_Start(nPlg);

	//// �����J�n�ꔭ�ڗp�̐ݒ���e�@��ɒʒm
	for(int ii=0; ii<NUM_MEN; ii++){
		ToOutMailSender::Send_ToCamera_CoilChange(ii, gcls_pCoil->GetInitScode(ii));
#ifndef KT_LEDMGR
		ToOutMailSender::Send_ToLights_CoilChange(ii, gcls_pCoil->GetInitScode(ii));
#else
		ToOutMailSender::Send_KtLedMgr_OnOff(ii, TRUE);
		ToOutMailSender::Send_KtLedMgr_CoilChange(ii, gcls_pCoil->GetInitScode(ii));
#endif
	}

	//// �I�y���[�^�@�\�֕\����񏉊����ʒm
	Send_HyDefect_MemDataInit();

#ifdef ENABLE_RPI_SAVE
	// �S���ۑ��J�n
	gcls_IfRpiSave.Send_TiRpiSave_Start();
#endif

	// ----------------------
	// �J�����I���R�}���h���O����
	LogicParaRunFunc::ClearLinkswCmd(LogicParaRunFunc::E_LINKSW_CMD_START);		// �R�}���h���X�g�N���A

	if (DIV_UNTEN_CAMERA != nUnten)
	{
		// �u�J�����Z���v���[�h�ȊO

		// �J�����I��ݒ�R�}���h�L���[�C���O�i�S���W�b�N�p������PC���j
		LogicParaRunFunc::RegistLinkswCmd();
	}

	//// �J���������N�X�C�b�`�ʐM�� �����J�n�ʒm (�� DB�o�^�������ɑ��M)
	//// ���O����
	DBManager::DeliveryDB* deliDB = new DBManager::DeliveryDB;
	deliDB->kubun	= DBManager::em_CAMLINKSW;
	deliDB->data	= (BYTE*)new DBDATA_CAMERA_LINKSWITCH;
	DBDATA_CAMERA_LINKSWITCH* pwk = (DBDATA_CAMERA_LINKSWITCH*)deliDB->data;
	memset(pwk, 0x00, sizeof(DBDATA_CAMERA_LINKSWITCH));

	//// �f�[�^�Z�b�g
	for(int ii=0; ii<NUM_MEN; ii++){
		if(!bOk[ii]) continue;
		for(int jj=0; jj<NUM_HANTEI; jj++){
			// ����PCID
			pwk->nPCID[ii][jj] = gcls_Status.GetHtID(ii,jj);
		}
	}
	for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
	{
		if (!bParaOk) continue;
		// ���W�b�N�p������PCID
		pwk->nParaPCID[ii] = gcls_Status.GetHtParaID(ii);
	}
	// ���W�b�N�p�������p�J�����Z�b�g
	// �����݃������ێ����Ă���l�i�ŐV�j��ݒ肷��
	memcpy(pwk->nLogiParaCam, LogicParaRunFunc::GetHtParaCamNo(), LogicParaRunFunc::GetHtParaCamSize());

	if( ! gcls_DB.gque_AddSQLData.AddToHead( deliDB ) )
	{
		LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [DBDATA_CAMERA_LINKSWITCH]", my_sThreadName);
		syslog(SYSNO_QUEFULL_ERR, "[DBDATA_CAMERA_LINKSWITCH]");
		delete deliDB->data;
		delete deliDB;
		// DB���ُ� (�ʏ킠��Ȃ�)
		gcls_Status.AddQueMyErrWar( true, ON_QUE_CAMLINKSW );
		return;
	}


	////////////////////////////////////
	// �I������
	////////////////////////////////////
	//// �X�e�[�^�X�ύX
	for( int ii=0; ii<NUM_MEN; ii++ )
	{
		if( bOk[ii] ) gcls_Status.SetSys( ii, DIV_SYS_INITSTART );
		else		  gcls_Status.SetSys( ii, DIV_SYS_STOP );
#ifdef ENABLE_CYCLE
		if( bCycleOk[ii] ) gcls_Status.SetCycleSys( ii, DIV_SYS_INITSTART );
		else			   gcls_Status.SetCycleSys( ii, DIV_SYS_STOP );
#endif
	}
	gcls_Status.SetMenMode( bMenMode );
	gcls_Status.SetPlg( nPlg );
	gcls_Status.SetUnten( nUnten );
	for(int ii=0; ii<NUM_MEN; ii++) gcls_Status.SetHtState_AllOff(ii);


#ifndef CAM_ADJUST_REMOTE
	//======================================================
	//// �J�����������A�e�����N�X�C�b�`�̐擪����ȊO�͐�����Z�b�g
	if (DIV_UNTEN_CAMERA == nUnten)
	{
		int nGroupNo = -1;
		for (int ii = 0; ii < NUM_MEN; ii++)
		{
			for (int jj = 0; jj < NUM_HANTEI; jj++)
			{
				int nWk = ToLinkswUtil::GetHanteiGroup(ii, jj, 1);
				if (nWk == -1 || nGroupNo == nWk)
				{
					gcls_Status.SetHtState(ii, jj, DIV_BOOL_TRUE);
				}
				if (nWk != -1 && nGroupNo != nWk) nGroupNo = nWk;
			}
		}
	}
#endif

	// ----------------------
	// �@���ԁ|�J���������N�X�C�b�`�ʐM���������i����j
	KizuFunction::SetStatus("CAMLINKSW_ANS_ID", true);
	if (DIV_UNTEN_CAMERA == nUnten) LOG(em_INF), "[%s] �J�����Z�����[�h���̓��W�b�N�p����������", my_sThreadName);
	for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
	{
		// �p�������@�\�V�X�e����ԕύX
		if (bParaOk)  gcls_Status.SetParaSys(ii, DIV_SYS_INITSTART);
		else		  gcls_Status.SetParaSys(ii, DIV_SYS_STOP);

		// �@���ԁ|���W�b�N�p�������@�\ 
		for (int jj = 0; jj < NUM_CAMLINK_BOARD; jj++)
		{
			// �I�����ʁi����j
			KizuFunction::SetStatus(gcls_Status.GetStatusRead().ParaState[ii][jj].id, true, false);
			// ���̓J�����i�s���j
			KizuFunction::SetStatus(gcls_Status.GetStatusRead().ParaCam[ii][jj].id, -1, false);
		}
	}
	// �J�����I��ݒ�A���T�[�҂��^�C�}�[�j��
	CancelWaitableTimer(LogicParaRunFunc::GetEvTimeCamset());
	// �r���Q���ݒ�A���T�[�҂��^�C�}�[�j��
	CancelWaitableTimer(LogicParaRunFunc::GetEvTimeResumeStart());
	// ���W�b�N�p��������Ԃ����Z�b�g�i�����j
	gcls_Status.SetHtParaState_AllOff();

	// �J���������N�X�C�b�`�ݒ�A���T�[�҂��^�C�} �N��
	__int64 ts = (__int64)PARAM_TIMEOUT_DETECT_INIT_START * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer( m_evTimeInitStart, (LARGE_INTEGER*)&ts, 0, NULL, NULL, FALSE );

	LOG(em_MSG), "[%s] �J���������N�X�C�b�`�ݒ芮���ʒm�҂�....", my_sThreadName);
}


//==========================================
// �����������҂�����
// (�J���������N�X�C�b�`�ݒ芮���ʒm)
//==========================================
void MainInstance::WaitInit_Answer(COMMON_QUE* que)
{
	////////////////////////////////////
	// ���O�`�F�b�N
	//   ���̎��_�ł̃G���[���́A���g���C���Ȃ�
	////////////////////////////////////
	//// �X�e�[�^�X�m�F
	for (int ii = 0; ii<NUM_MEN; ii++)
	{
		// �J��������
		if (DIV_UNTEN_CAMERA == gcls_Status.GetUnten()) {
			LOG(em_MSG), "[%s] <%s> �J�����Z�����[�h���̃J���������N�X�C�b�`�ݒ芮���ʒm", my_sThreadName, DivNameManager::GetTorB(ii));
			//return;		// iPortViewer����̃J�����؂�ւ���
		}

		// �Жʌ��������́A�����̖ʂ������J�n���������ł���΁A�����M�Ƃ���
		if (DIV_SYS_STOP == gcls_Status.GetSys(ii)) {
			int ou_not = ii == 1 ? 0 : 1;								// �����̖�
			if (!gcls_Status.GetMenMode() ||
				(gcls_Status.GetMenMode() && DIV_SYS_INITSTART != gcls_Status.GetSys(ou_not))) {
				LOG(em_MSG), "[%s] <%s> [%s] ���Ɍ����������A���T�[�ʒm��M(�J�����ؑ�)", my_sThreadName, DivNameManager::GetTorB(ii), GetDivSys(gcls_Status.GetSys(ii)));
				return;
			}
		}

		if (DIV_SYS_INITSTART != gcls_Status.GetSys(ii) &&
			DIV_SYS_STOP != gcls_Status.GetSys(ii))
		{
			LOG(em_ERR), "[%s] <%s> [%s] ���Ɍ����������A���T�[�ʒm��M", my_sThreadName, DivNameManager::GetTorB(ii), GetDivSys(gcls_Status.GetSys(ii)));
			return;
		}
	}

	//// �������^�C�}�[�j��
	CancelWaitableTimer(m_evTimeInitStart);

	// �ݒ茋�ʃ`�F�b�N
	if (1 != que->fl.data[0])
	{
		// �ݒ茋�ʁF���s
		LOG(em_ERR), "[%s] �J���������N�X�C�b�`�ݒ芮���ʒm �ُ팟�o(���ʈُ�) �� ���������ċN�� �v��", my_sThreadName);

		// �@���ԁ|�J���������N�X�C�b�`�ʐM���������i�ُ�j
		KizuFunction::SetStatus("CAMLINKSW_ANS_ID", false);

		// �R�}���h���X�g�N���A
		LogicParaRunFunc::ClearLinkswCmd();

		// ������~�E�J�n
		syslog(235, "[�J���������N�X�C�b�`�ݒ芮���ʒm ���ʈُ�]");
		DetectStop(false);							// ������~
		DetectReStart();							// ���������ċN��
	}
	else
	{
		// �ݒ茋�ʁF����

		// �ݒ茋�ʁF����
		LOG(em_MSG), "[%s] �J���������N�X�C�b�`�ݒ芮���I�I", my_sThreadName);

		// �����J�n�҂�����
		DetectWaitStart();
	}

	if (DIV_UNTEN_CAMERA != gcls_Status.GetUnten())
	{
		// �u�J�����Z���v���[�h���̓��W�b�N�p����������

		LOG(em_MSG), "[%s] ���W�b�N�p�������ݒ蒆....", my_sThreadName);

		// ���̃R�}���h���M�i�L���[����j
		LogicParaRunFunc::Send_NextCmdSelect();
	}
}

//==========================================
// �����J�n�҂��J�n���� (���茟���J�n�A���T�[�҂���)
//==========================================
void MainInstance::DetectWaitStart()
{

	//======================================================
	// ���O�`�F�b�N
	//   ���̎��_�ł̃G���[���́A���g���C���Ȃ�
	// �K����~���̂�
	for(int ii=0; ii<NUM_MEN; ii++) {
		// ���X�e�[�^�X�m�F
		if( DIV_SYS_IDLE      != gcls_Status.GetSys(ii) && 
			DIV_SYS_INITSTART != gcls_Status.GetSys(ii) &&
			DIV_SYS_STOP      != gcls_Status.GetSys(ii) &&
#ifdef ENABLE_CYCLE
			DIV_SYS_IDLE      != gcls_Status.GetCycleSys(ii) && 
			DIV_SYS_INITSTART != gcls_Status.GetCycleSys(ii) && 
			DIV_SYS_STOP      != gcls_Status.GetCycleSys(ii) &&
			DIV_SYS_RETRY     != gcls_Status.GetCycleSys(ii) &&
#endif
			DIV_SYS_RETRY!= gcls_Status.GetSys(ii)  ) {
			LOG(em_ERR), "[%s] <%s> [%s] ���Ɍ����J�n�v����M", my_sThreadName, DivNameManager::GetTorB(ii), GetDivSys(gcls_Status.GetSys(ii)) );
			syslog(236, "[<%s> %s ���Ɍ����J�n�v����M]", DivNameManager::GetTorB(ii), GetDivSys(gcls_Status.GetSys(ii)) );
			return ;
		}
	}

	////////////////////////////////////
	// �����J�n�҂�����
	////////////////////////////////////
	// PLG�J�E���^�l ���Z�b�g�v��
#ifndef DISABLE_PLG_RESET
	gcls_Dmem.PoPuls_CountReset();
	syslog(242, "�T���J�n���Ɏ������Z�b�g");
	LOG(em_INF), "[%s] �V�[�P���TPLG�J�E���g���Z�b�g(�����J�n��)[Exsync�l=%d][Plg�l=%d]", my_sThreadName, gcls_Dmem.GetDmRecv().ken.nLvalAdd, gcls_Dmem.GetDmRecv().ken.nPlgAdd);
#endif
	//// ����PC�Ɍ����J�n�ʒm�𑗐M
	Send_HtDefect_Start();
	//// ����PC����JPEG���k������
	Send_HtDefect_JpegQuality(gcls_pParam->GetCommonMast().nJpegQuality);

	//======================================================
	// �I������

	// �X�e�[�^�X�ύX
	for(int ii=0; ii<NUM_MEN; ii++) {
		if( DIV_SYS_STOP == gcls_Status.GetSys(ii) ) continue;
		gcls_Status.SetSys(ii, DIV_SYS_WAITSTART );
#ifdef ENABLE_CYCLE
		gcls_Status.SetCycleSys(ii, DIV_SYS_WAITSTART );
#endif
	}
	// �����J�n���ɂ͐��퉻
	KizuFunction::SetStatus("PLGLEN_KEN_CHECK_ID", true, false);	// �����pPLG�␳��
	KizuFunction::SetStatus("PLGLEN_DSP_CHECK_ID", true, false);	// �\���pPLG�␳��
	KizuFunction::SetStatus("HOSEIRITU_ID", true, false);	// �v���R�����␳��
	for (int ii = 0; ii<NUM_MEN; ii++) {
		KizuFunction::SetStatus(gcls_Status.GetStatusRead().FrameGrabStop[ii].id, true);	// �B���t���[����~
	}

	// �p�g���C�g�o�͊Ǘ�������
	gcls_Arm.Init_Pato();

	//======================================================
	// ����PC����̉����҂��^�C�}�[�N��
	__int64 ts = (__int64)PARAM_TIMEOUT_WAIT_START * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evTimeWaitStart, (LARGE_INTEGER*)&ts, 0, NULL, NULL, FALSE);
}

//==========================================
// �����J�n�҂� ���� (�A���T�[��M)
//------------------------------------------
// COMMON_QUE* que	: �N�����
//==========================================
void MainInstance::WaitStart_Answer(COMMON_QUE* que)
{

	//======================================================
	// ������
	int campair = que->fl.data[0];							// �J�����y�A (1�I���W��)
	int pcid	= que->fl.data[1];							// PCID (1�I���W��)
	int ans		= que->fl.data[2];							// ���� (0:���� 1:�y�̏� -1:�d�̏�)
	EM_DIV_HANTEI emKind = (EM_DIV_HANTEI)que->fl.data[3];	// �S������


	int ou;													// �\��
	int no;													// No
	if( DIV_HANTEI_HT == emKind ) {
		ou		= ((campair-1) / (NUM_CAMPAIR/NUM_MEN) );
		no		= ((campair-1) % (NUM_CAMPAIR/NUM_MEN) );
	}
#ifdef ENABLE_CYCLE
	if( DIV_HANTEI_CY == emKind ) {
		ou		= ((campair-1) / (NUM_CAMPAIR_CYCLE/NUM_MEN) );
		no		= ((campair-1) % (NUM_CAMPAIR_CYCLE/NUM_MEN) );		
	}
#endif
	// [�S������] �p����������
	if (DIV_HANTEI_PR == emKind)
	{
		// �ُ퉞��
		// ����ɐ���ŕԂ���
		if (0 != ans)
		{
			LOG(em_ERR), "[%s] �����J�n�ُ�A���T�[��M(���W�b�N�p������) �J�����y�A=%d ans=%d", my_sThreadName, campair, ans);

			// [�@��ݒ�]�|����PCxx��������
			CString wk;
			wk.Format("HANTEI_ANS_%d_ID", pcid);
			KizuFunction::SetStatus(wk, false, true, "�����J�n�A���T�[�ُ�");
		}
		else
		{
			//// �p���������萳��
			if (0 != gcls_Status.GetStatusRead().Ht_NonAnswer[pcid - 1].stat)
			{
				// [�@��ݒ�]�|����PCxx��������
				KizuFunction::SetStatus("HANTEI_ANS_%d_ID", pcid, 0);
			}
		}

		// ���W�b�N�p������PC�̉��������͂����܂�
		return;
	}

	//======================================================
	// ���O�`�F�b�N
	//// �X�e�[�^�X
	if( DIV_HANTEI_HT == emKind &&
		gcls_Status.GetSys(ou) != DIV_SYS_WAITSTART ) {
		LOG(em_ERR), "[%s] <%s> [%s]���Ɍ����J�n�A���T�[�ʒm��M", my_sThreadName, DivNameManager::GetTorB(ou), GetDivSys(gcls_Status.GetSys(ou)));
		return;
	}
#ifdef ENABLE_CYCLE
	if( DIV_HANTEI_CY == emKind &&
		gcls_Status.GetCycleSys(ou) != DIV_SYS_WAITSTART ) {
		LOG(em_ERR), "[%s] <%s> [%s]���Ɏ������茟���J�n�A���T�[�ʒm��M", my_sThreadName, DivNameManager::GetTorB(ou), GetDivSys(gcls_Status.GetCycleSys(ou)));
		return;
	}
#endif

	//// �A���T�[����
	if( 0 != ans ) {
		LOG(em_ERR), "[%s] �����J�n�ُ�A���T�[��M �J�����y�A=%d ans=%d", my_sThreadName, campair, ans);
		syslog(236, "[�ُ�A���T�[��M �J�����y�A=%d ans=%d]", campair, ans);
		CString wk;
		wk.Format("HANTEI_ANS_%d_ID", pcid );
		KizuFunction::SetStatus(wk, false, false, "�����J�n�A���T�[�ُ�");
		gcls_Status.SetHtState(ou, no, DIV_BOOL_FALSE); 
//		return;
	}

	//// ������ �Z�b�g
	if( 0 == ans ) {
		if( DIV_HANTEI_HT == emKind ) gcls_Status.SetHtState(ou, no, DIV_BOOL_TRUE); 
		if( DIV_HANTEI_HT == emKind && 1 == NUM_CAMPAIR) gcls_Status.SetHtState(1, no, DIV_BOOL_TRUE);	// 1PC ���ʑΉ�
		if( 0 != gcls_Status.GetStatusRead().Ht_NonAnswer[pcid-1].stat ) {
			KizuFunction::SetStatus("HANTEI_ANS_%d_ID", pcid, 0);
		}
	}

	//// �S���肩�牽������̉������A���ė����H
	for(int ii=0; ii<NUM_MEN; ii++) {
		if( DIV_SYS_STOP == gcls_Status.GetSys(ii) ) continue;					// ����NG�Ƃ킩���Ă���ʂł���Ζ���
		
		EM_DIV_BOOL wk;
		for(int jj=0; jj<NUM_MEN; jj++){
			for(int kk=0; kk<NUM_HANTEI; kk++){
				wk =  gcls_Status.GetHtState(jj,kk);
			}
		}

		if( gcls_Status.IsHtState_OneOnOff(ii, DIV_BOOL_NON ) ) return;			// �P�ł������l�L��


#ifdef ENABLE_CYCLE
		// �Z�����́A�����͓������Ȃ�
		if( DIV_UNTEN_CAMERA != gcls_Status.GetUnten() ) {
			if( ! gcls_Status.IsCyState_AllOnOff(ii, DIV_BOOL_TRUE) ) return;
		}
#endif

		// �Z�����́A�\�P�b�g���q���Ȃ�����
		if( DIV_UNTEN_CAMERA != gcls_Status.GetUnten() ) {
			if( ! gcls_pHtRecv[ii]->IsConnectAll() ) return;
#ifdef ENABLE_CYCLE
			if( ! gcls_pCyRecv[ii]->IsConnectAll() ) return;
#endif
		}
	}
	
	//// �Жʌ��������`�F�b�N�Ή�
	bool bTimeUp = false;
	for(int ii=0; ii<NUM_MEN; ii++) {
		if( gcls_Status.IsHtState_OneOnOff(ii, DIV_BOOL_FALSE) ) bTimeUp = true;
	}
	if(bTimeUp) {
		WaitStart_TimeOut();	// �^�C���A�b�v�������ƂƂ���B
		return;
	}

	
	//// �����܂ł����猟�����֑J��
	DetectRun();
}

//==========================================
// ������~�҂� ���� (�A���T�[��M)
//------------------------------------------
// COMMON_QUE* que	: �N���v��	
//==========================================
void MainInstance::WaitStop_Answer(COMMON_QUE* que)
{
	//======================================================
	// ������
	int campair = que->fl.data[0];							// �J�����y�A (1�I���W��)
	int pcid	= que->fl.data[1];							// PCID (1�I���W��)
	int ans		= que->fl.data[2];							// ���� (0:���� 1:�y�̏� -1:�d�̏�)
	int mode	= que->fl.data[3];							// �敪 (0:���� 1:�����Ȃ�)
	EM_DIV_HANTEI emKind = (EM_DIV_HANTEI)que->fl.data[4];	// �S������


	int ou = 0;												// �\��
	int no = 0;												// 0�`3
	if( DIV_HANTEI_HT == emKind ) {
		ou		= ((campair-1) / (NUM_CAMPAIR/NUM_MEN) );
		no		= ((campair-1) % (NUM_CAMPAIR/NUM_MEN) );
	}
#ifdef ENABLE_CYCLE	
	if( DIV_HANTEI_CY == emKind ) {
		ou		= ((campair-1) / (NUM_CAMPAIR_CYCLE/NUM_MEN) );
		no		= ((campair-1) % (NUM_CAMPAIR_CYCLE/NUM_MEN) );		
	}
#endif
	// [�S������] �p����������
	if (DIV_HANTEI_PR == emKind)
	{
		// �ُ퉞��
		// ����ɐ���ŕԂ���
		if (0 != ans || 0 != mode)
		{
			LOG(em_ERR), "[%s] ������~�A���T�[ �ُ팟�o(���W�b�N�p������) pcid=%d", my_sThreadName, pcid);

			// [�@��ݒ�]�|����PCxx��������
			CString wk;
			wk.Format("HANTEI_ANS_%d_ID", pcid);
			KizuFunction::SetStatus(wk, false, true, "������~�A���T�[�ُ�");
		}
		else
		{
			//// �p���������萳��
			if (0 != gcls_Status.GetStatusRead().Ht_NonAnswer[pcid - 1].stat)
			{
				// [�@��ݒ�]�|����PCxx��������
				KizuFunction::SetStatus("HANTEI_ANS_%d_ID", pcid, 0);
			}
		}

		// ���W�b�N�p������PC�̏�ԕύX
		for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
		{
			// �����������W�b�N�p������PC�̂ݏ�ԕύX
			if (gcls_Status.GetHtParaID(ii) == pcid)
			{
				// ----------------------
				// ���W�b�N�p��������� �| �N���A
				// �i���W�b�N�p��������ԗ��\���p�j
				gcls_Status.SetHtParaState(ii, DIV_BOOL_FALSE);
				break;
			}
		}

		// ���W�b�N�p������PC�̉��������͂����܂�
		return;
	}
	int ou_not  = ou==1 ? 0 :1;								// �����̖�

	//======================================================
	// ���O�`�F�b�N
	if( 0 == mode ) {
		//// ������ �Z�b�g	
		if( DIV_HANTEI_HT == emKind ) gcls_Status.SetHtState(ou, no, DIV_BOOL_FALSE); 
		if( DIV_HANTEI_HT == emKind && 1 == NUM_CAMPAIR) gcls_Status.SetHtState(1, no, DIV_BOOL_FALSE);	// 1PC ���ʑΉ�
		//if( DIV_HANTEI_CY == emKind ) gcls_Status.SetCyState(ou, no, DIV_BOOL_FALSE); 
		//if( DIV_HANTEI_CY == emKind && 1 == NUM_CAMPAIR) gcls_Status.SetCyState(1, no, DIV_BOOL_FALSE);	// 1PC ���ʑΉ�

		//// ����
		if( DIV_HANTEI_HT == emKind ) {
			if( gcls_Status.GetSys(ou) == DIV_SYS_WAITSTOP ||
				gcls_Status.GetSys(ou) == DIV_SYS_STOP	) {
				return;
			} else {
				LOG(em_MSG), "[%s] �P�����茟����~�A���T�[ ��M��Ԉُ� %s campair=%d ans=%d", my_sThreadName, GetDivSys(gcls_Status.GetSys(ou)), campair, ans);
			}
		}
#ifdef ENABLE_CYCLE		
		if( DIV_HANTEI_CY == emKind ) {
			if(	gcls_Status.GetCycleSys(ou) == DIV_SYS_WAITSTOP ||
				gcls_Status.GetCycleSys(ou) == DIV_SYS_STOP	) {
				return;
			} else {
				LOG(em_MSG), "[%s] �������茟����~�A���T�[ ��M��Ԉُ� %s campair=%d ans=%d", my_sThreadName, GetDivSys(gcls_Status.GetCycleSys(ou)), campair, ans);
			}
		}
#endif
	}

	//// ���萳��
	if( 0 != gcls_Status.GetStatusRead().Ht_NonAnswer[pcid-1].stat ) {
		KizuFunction::SetStatus("HANTEI_ANS_%d_ID", pcid, 0);
	}


	//======================================================
	////// �����Ȃ��~�ʒm�������I�I ���́A �d�̏᎞
	if( DIV_KADOU_STOP == gcls_Status.GetKadou() ) return;
	if( DIV_HANTEI_HT == emKind ) {
		if( (1 == mode && gcls_Status.IsRun() ) || -1 == ans ) {
			// �E�Жʌ����s�����́A������~
			// �E�Жʌ��������ł��A���������NG�̎��́A������~
			if( ! gcls_Status.GetMenMode() ||
				( gcls_Status.GetMenMode() && DIV_SYS_RUN != gcls_Status.GetSys(ou_not)) ) {
				LOG(em_ERR), "[%s] ������~�A���T�[ �ُ팟�o pcid=%d", my_sThreadName, pcid);

				// ������~���Ă��猟�������ċN����
				AllDetectStop(false, true);
				return;

			// �E�Жʌ��������ŁA�����Ƃ�������
			} else if( gcls_Status.GetMenMode() && DIV_SYS_RUN == gcls_Status.GetSys(0) && DIV_SYS_RUN == gcls_Status.GetSys(1) ) {
				LOG(em_WAR), "[%s] ������~�A���T�[ �ُ팟�o�B�Жʂ݂̂Ō������s pcid=%d", my_sThreadName, pcid);
				// ����̖ʂ̂݌������f
				DetectRun_Cancel(ou);
			}	
		}
	}
}

//==========================================
// �����J�n�҂� ���� (�^�C���A�E�g����)
//==========================================
void MainInstance::WaitStart_TimeOut()
{

	//======================================================
	//// ���O�`�F�b�N
	// �X�e�[�^�X
#ifndef ENABLE_CYCLE
	if( ( gcls_Status.GetSys(0) != DIV_SYS_WAITSTART && gcls_Status.GetSys(0) != DIV_SYS_STOP ) &&
		( gcls_Status.GetSys(1) != DIV_SYS_WAITSTART && gcls_Status.GetSys(1) != DIV_SYS_STOP ) ) {
		LOG(em_ERR), "[%s] �����J�n�A���T�[�^�C���A�E�g �X�e�[�^�X�ُ� (�\=%s ��=%s)", my_sThreadName, GetDivSys(gcls_Status.GetSys(0)), GetDivSys(gcls_Status.GetSys(1)));
		return;
	}
#else
	if( ( gcls_Status.GetSys(0) != DIV_SYS_WAITSTART && gcls_Status.GetSys(0) != DIV_SYS_STOP ) &&
		( gcls_Status.GetSys(1) != DIV_SYS_WAITSTART && gcls_Status.GetSys(1) != DIV_SYS_STOP ) &&
		( gcls_Status.GetCycleSys(0) != DIV_SYS_WAITSTART && gcls_Status.GetCycleSys(0) != DIV_SYS_STOP ) &&
		( gcls_Status.GetCycleSys(1) != DIV_SYS_WAITSTART && gcls_Status.GetCycleSys(1) != DIV_SYS_STOP ) ) {
		LOG(em_ERR), "[%s] �����J�n�A���T�[�^�C���A�E�g �X�e�[�^�X�ُ� (�\=%s ��=%s �����\=%s ������=%s)", my_sThreadName, GetDivSys(gcls_Status.GetSys(0)), GetDivSys(gcls_Status.GetSys(1)), GetDivSys(gcls_Status.GetCycleSys(0)), GetDivSys(gcls_Status.GetCycleSys(1)) );
		return;
	}
#endif

	//======================================================
	//// �ǂ̔���@�\���_���H
	for(int ii=0; ii<NUM_MEN; ii++) {
		for(int jj=0; jj<NUM_HANTEI; jj++) {
			if( ! gcls_Status.IsHtState(ii, jj) ) {
				if(0 == gcls_Status.GetHtID(ii, jj) ) continue;
				KizuFunction::SetStatus("HANTEI_ANS_%d_ID", gcls_Status.GetHtID(ii, jj), 1, false);
			}
		}
#ifdef ENABLE_CYCLE
		for(int jj=0; jj<NUM_HANTEI_CYCLE; jj++) {
			if( ! gcls_Status.IsCyState(ii, jj) ) {
				if(0 == gcls_Status.GetCyID(ii, jj) ) continue;
				KizuFunction::SetStatus("HANTEI_ANS_%d_ID", gcls_Status.GetCyID(ii, jj), 1, false);
			}
		}
#endif
	}
	for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
	{
		if (!gcls_Status.IsHtParaState(ii))
		{
			if (0 == gcls_Status.GetHtParaID(ii)) continue;
			KizuFunction::SetStatus("HANTEI_ANS_%d_ID", gcls_Status.GetHtParaID(ii), 1, false);
		}
	}

	//======================================================
	//// �ǂ̖ʂ��_���H
	for(int ii=0; ii<NUM_MEN; ii++) {
		// �������� �J�n�����̎�����_���ȓz�͖�����
		if( gcls_Status.GetSys(ii) == DIV_SYS_STOP ) continue;

		// �_���H
		if( ! gcls_Status.IsHtState_AllOnOff(ii, DIV_BOOL_TRUE) ||												// �A���T�[
			( DIV_UNTEN_CAMERA != gcls_Status.GetUnten() && ! gcls_pHtRecv[ii]->IsConnectAll()) ) {		// �Z�����́A�\�P�b�g���q���Ȃ�����

			// �w��ʂ݂̂̌������f
			DetectRun_Cancel(ii);
		}
	}


	// ���ʌ������́A��ł��G���[������� NG
	// �Жʋ����́A�����G���[�� NG
	if( ( ! gcls_Status.GetMenMode() && (gcls_Status.GetSys(0) == DIV_SYS_STOP || gcls_Status.GetSys(1) == DIV_SYS_STOP) ) ||
		(   gcls_Status.GetMenMode() && (gcls_Status.GetSys(0) == DIV_SYS_STOP && gcls_Status.GetSys(1) == DIV_SYS_STOP) ) )  {
	
		for(int ii=0; ii<NUM_MEN; ii++) {
			if( gcls_Status.GetSys(ii) == DIV_SYS_STOP ) continue;
			DetectRun_Cancel(ii);
		}

#ifdef ENABLE_CYCLE
		for(int ii=0; ii<NUM_MEN; ii++) {
			if( gcls_Status.GetCycleSys(ii) == DIV_SYS_STOP ) continue;
			Cy_DetectRun_Cancel(ii);
		}
#endif


		//// �I�y���[�^�@�\�֌���Ԃ𑗐M
		Send_SoGamenn_MailStatus();
		OpSendFunc::SendOp_Status(); 

		LOG(em_ERR), "[%s] �����J�n�A���T�[�^�C���A�E�g ���������ċN�� �v��", my_sThreadName);
		DetectReStart();
		return;
	}

	//// �����܂ł����猟�����֑J��
	DetectRun();
}

//------------------------------------------
// �w��ʂ݂̂̌������f
// int ou �\:0 ��:1
//------------------------------------------
void MainInstance::DetectRun_Cancel(int ou)
{
	LOG(em_ERR), "[%s] %s�̌������f", my_sThreadName, DivNameManager::GetTorB(ou) );
	if (0 == ou) KizuFunction::SetStatus("HANTEI_TOP_ANS_ID", false, false);
	else	  KizuFunction::SetStatus("HANTEI_BOT_ANS_ID", false, false);
	gcls_Status.SetSys(ou, DIV_SYS_STOP);

	// �����ʏ�ԕύX
	int ou_not = ou == 1 ? 0 : 1;				// �����̖�
	gcls_Status.SetMenState(ou_not);		// NG�̖ʂƈقȂ���݂̂Ō������{

											// ��~����
	Send_HtDefect_Stop(ou);
	Sleep(500);									// ���肪���[�����󂯎���āA�����؂�܂ő҂�
	gcls_pHtRecv[ou]->SetEvDetectStop();
}

//==========================================
// �������J�n���� (��������)
//==========================================
void MainInstance::DetectRun()
{
	LOG(em_MSG), "[%s] ������ �����J�n�I�I", my_sThreadName);

	//======================================================
	//// �������̃^�C�}�[�j��
	CancelWaitableTimer(m_evTimeWaitStart);

	//======================================================
	//// ������돉����
	gcls_DB.ReSetMaxCount();
	for (int ii = 0; ii < HY_SOCK_NUM; ii++) gcls_pOpS[ii]->gque_Deli.ReSetMaxCount();
	gcls_Plg.CoilInit();						// ���������������Ă����Ȃ��ƁA�X�e�[�^�X�ύX����@���Ԃ��ύX����ď�Ԃ��ς���
	gcls_pCoil->Init( (DIV_KADOU_RETRY==gcls_Status.GetKadou() ? true : false) );	// �R�C����񏉊���
	// �O�񑗐M�����R�}���h����������
	memset(m_strCmd, 0x00, sizeof(m_strCmd));

	// --->>> 20240122 Add Kato       
        // FrameNo�g���b�L���O���~ (DetectStop�֐����őS�v�f������Ă��邪�A�����ȃ^�C�~���O�őS�v�f������PlgManager���ŗv�f�ǉ������ꍇ�����邽�߂̑΍�)
        for (int ii = 0; ii < MAX_TR_FRAME; ii++)
        {
             gcls_TrFrm.DelAll(ii);
        }
	// <<<--- 20240122 Add Kato       

	// �����J�n���ɐ��한�A����@���Ԃ��Z�b�g
	//////gcls_Status.SetStatusQue(gcls_Status.GetStatusRead().PlgDiff.id,	   true);	// �V�[�P���T��������

	//======================================================
	//// �X�e�[�^�X�ύX ��������
	gcls_Status.SetKadou( DIV_KADOU_START );
	for(int ii=0; ii<NUM_MEN; ii++) {
		if( gcls_Status.GetSys(ii) == DIV_SYS_STOP ) continue;
		gcls_Status.SetSys(ii, DIV_SYS_RUN );
	}
#ifdef ENABLE_CYCLE
	for(int ii=0; ii<NUM_MEN; ii++) {
		if( gcls_Status.GetCycleSys(ii) == DIV_SYS_STOP ) continue;
		gcls_Status.SetCycleSys(ii, DIV_SYS_RUN );
	}
#endif

	//// ����NG�͂��肦�Ȃ�
	if( DIV_SYS_RUN == gcls_Status.GetSys(0) && DIV_SYS_RUN == gcls_Status.GetSys(1) ) {	// ���ʌ������{
		gcls_Status.SetMenState(-1);
	} else if( DIV_SYS_RUN == gcls_Status.GetSys(0) ) {						// �\�ʂ̂݌������{
		gcls_Status.SetMenState(0);
	} else if( DIV_SYS_RUN == gcls_Status.GetSys(1) ) {						// ���ʂ̂݌������{
		gcls_Status.SetMenState(1);
	}


	//======================================================
	// �J���������ȊO�̏���
	if(DIV_UNTEN_CAMERA != gcls_Status.GetUnten()) {
		// ����R�C������ʔ\������
		OpSendFunc::SendOp_CoilInf_Ken( gcls_pCoil->GetCoilBaseKen() );		// �f�t�H���g�̌����p�R�C�����
		OpSendFunc::SendOp_CoilInf_Dsp( gcls_pCoil->GetCoilBaseDsp() );		// �����Ȃ�\���p�̃R�C�����𑗐M
		Send_HyDefect_MaeRead( gcls_pCoil->GetCoilBaseKen()->cKizukenNo );	// �\��PC�̋��L�������������ׂ̈ɒʒm

#ifndef SENDOP_LENGTH_PLG
		//// �����J�n��{�ڂ̃R�C������\�����邽�߂ɕ\���@�\�֑��M
		// �ʔʒu���
		OpSendFunc::SendOp_Length();
#endif

		// �����M�N���X�̌����J�n�ʒm
		for(int ii=0; ii<NUM_MEN; ii++) {
			if( gcls_Status.GetSys(ii) == DIV_SYS_STOP ) continue;
			gcls_pHtRecv[ii]->SetEvDetectRun();
		}
#ifdef ENABLE_CYCLE
		for(int ii=0; ii<NUM_MEN; ii++) {
			if( gcls_Status.GetCycleSys(ii) == DIV_SYS_STOP ) continue;
			gcls_pCyRecv[ii]->SetEvDetectRun();
		}
#endif

#ifdef ENABLE_RECORDER_UDP
		gcls_CmdSender.SendCommand_Start();					// �����J�n�R�}���h���M
#endif

		// �X�e�[�^�X�����ēǂݍ���
		gcls_Status.RefreshStatus();


		// iPort�g�p���A������iPort��������҂�
#ifndef LOCAL		// �e�X�g���͑҂K�v�Ȃ�
	//	LOG(em_INF), "[%s] �J�����������҂��E�E�E", my_sThreadName);
	//	Sleep(5000);		// ���蓙������낪������������܂ł�����Ƒ҂� (���삳�񂩂�̃C���v�b�g)
#endif

		//// �T���v�����O�J�n
		gcls_pSeqFrameR->SetRun(true);
		gcls_Plg.SetEvSampStart();

	}

	//======================================================
	//// �J�����R���g���[���փp���XON��ʒm (���̃^�C�~���O�ŏo�͂���̂́A�ʏ�^�]�̂�)
	// �T���v���B�e���ǉ�
	if( DIV_UNTEN_NOMAL == gcls_Status.GetUnten() || DIV_UNTEN_SMYU  == gcls_Status.GetUnten() || DIV_UNTEN_SAMP  == gcls_Status.GetUnten() ) {
		// PLG�J�E���^�l ���Z�b�g�v��
#ifndef DISABLE_PLG_RESET
		Sleep(1000);		// �V�[�P���T���������o����܂ő҂�
		gcls_Dmem.PoPuls_CountStart();
		syslog(241, "�T���J�n���Ɏ������Z�b�g");
		LOG(em_INF), "[%s] �V�[�P���TPLG�J�E���g�J�n�v��(�B���J�n��)[Exsync�l=%d][Plg�l=%d]", my_sThreadName, gcls_Dmem.GetDmRecv().ken.nLvalAdd, gcls_Dmem.GetDmRecv().ken.nPlgAdd);
		Sleep(500);			// �V�[�P���T�ɊJ�n�v�������Ă���J�E���g�X�^�[�g����܂ő҂�
#endif
		ToOutMailSender::Send_ToCamera_Palse(true);
		LOG(em_INF), "[%s] �J����PLG�J�E���g�X�^�[�g[Exsync�l=%d][Plg�l=%d]", my_sThreadName, gcls_Dmem.GetDmRecv().ken.nLvalAdd, gcls_Dmem.GetDmRecv().ken.nPlgAdd);
	}

	//======================================================
	//// �I�y���[�^�@�\�։ғ���Ԃ𑗐M
	Send_SoGamenn_MailStatus();
	OpSendFunc::SendOp_Status(); 
}


//==========================================
// ������~ ���� (���茟����~�A���T�[�҂���)
//------------------------------------------
// bool		bMode			: true:�����~�Afalse:�ُ��~	
//==========================================
void MainInstance::DetectStop(bool bMode)
{
	//======================================
	// �������̃^�C�}�[�j��
	CancelWaitableTimer(m_evTimeReStart);
	CancelWaitableTimer(m_evTimeInitStart);
	CancelWaitableTimer(m_evTimeWaitStart);
	CancelWaitableTimer(LogicParaRunFunc::GetEvTimeCamset());
	CancelWaitableTimer(LogicParaRunFunc::GetEvTimeResumeStart());

	gcls_pTrKenDistPos->TrackingStop();												// �g���b�L���O���Ȃ炻�����~
	gcls_pTlCoilOutSt->TrackingStop();												// �g���b�L���O���Ȃ炻�����~

	// �o�������R�C���P�ʂ̏�ԊǗ��p�g���b�L���O �L�����Z��
	gcls_pTlCoilOutSt->QueAllFree();												// �g���b�L���O���Ȃ炻�����~

	for(int ii=0; ii<MAX_TR_FRAME; ii++) gcls_TrFrm.DelAll(ii);						// FrameNo�g���b�L���O���~

	// �R�}���h���X�g�N���A
	LogicParaRunFunc::ClearLinkswCmd();

	//======================================
	// ���ђ��߃`�F�b�N
	bool bResult = false;
	if( DIV_KADOU_STOP   != gcls_Status.GetKadou() &&
		DIV_KADOU_RETRY  != gcls_Status.GetKadou() &&
		DIV_UNTEN_CAMERA != gcls_Status.GetUnten() )		bResult = true;


	//======================================
	// �X�e�[�^�X�ύX
	gcls_Status.SetKadou( DIV_KADOU_STOP );
	for(int ii=0; ii<NUM_MEN; ii++)	gcls_Status.SetSys(ii, DIV_SYS_WAITSTOP );
	if (DIV_UNTEN_CAMERA != gcls_Status.GetUnten())
	{
		// �u�J�����Z���v���[�h�ȊO
		for (int ii = 0; ii<NUM_ALL_HTPARA; ii++) gcls_Status.SetParaSys(ii, DIV_SYS_WAITSTOP);
	}


#ifdef CAM_ADJUST_REMOTE
	//======================================================
	// �J����������
	//	�� �����[�g�f�X�N�g�b�v���̂�
	if( DIV_UNTEN_CAMERA == gcls_Status.GetUnten() ) {
		gcls_Status.SetUnten( DIV_UNTEN_NOMAL );	// �ʏ�ɖ߂��Ă���
		gcls_Status.SetKadou( DIV_KADOU_STOP );
		for(int ii=0; ii<NUM_MEN; ii++) {
			gcls_Status.SetSys(ii, DIV_SYS_STOP );
		}

		//// �I�y���[�^�@�\�ւ𑗐M
		Send_SoGamenn_MailStatus();
		OpSendFunc::SendOp_Status(); 
		return ;
	}
#endif



	//======================================
	// ��~���� (��1�i)
	//	�E�T���v�����O��~
	//	�E����Ɍ�����~��ʒm
	//	�E�J�����R���g���[���[�փp���X�o�͒�~��ʒm
	//  �ELED�Ɩ� ����
	//	�E����Ƃ̉���ؒf

	//// �T���v�����O��~
	if(DIV_UNTEN_CAMERA != gcls_Status.GetUnten()){
		gcls_pSeqFrameR->SetRun(false);
		gcls_Plg.SetEvSampStop();
	}

#ifdef ENABLE_RPI_SAVE
	// �S���摜�ۑ���~
	gcls_IfRpiSave.Send_TiRpiSave_Stop();
#endif
#ifdef ENABLE_RECORDER_UDP
	// �����p���R�[�_�[�֌�����~�R�}���h���M
	gcls_CmdSender.SendCommand_Stop();
#endif

	//// ����Ɍ�����~��ʒm
	Send_HtDefect_Stop();
	Sleep(2000);			// ���蓙���X����~��������������܂ő҂�
	
	// PLG�J�E���^�l ���Z�b�g�v��
#ifndef DISABLE_PLG_RESET
	gcls_Dmem.PoPuls_CountReset();
	syslog(242, "�T����~���Ɏ������Z�b�g");
	LOG(em_INF), "[%s] �V�[�P���TPLG�J�E���g���Z�b�g(�����I����)[Exsync�l=%d][Plg�l=%d]", my_sThreadName, gcls_Dmem.GetDmRecv().ken.nLvalAdd, gcls_Dmem.GetDmRecv().ken.nPlgAdd);
#endif

	//// �J�����R���g���[���Ƀp���X�o�͒�~��ʒm
	ToOutMailSender::Send_ToCamera_Palse(false);


	//// LED�Ɩ����Ƃ߂�
	for(int ii=0; ii<NUM_MEN; ii++){
		ToOutMailSender::Send_KtLedMgr_OnOff(ii, FALSE);
	}

	//// ����Ƃ̉���ؒf
	for(int ii=0; ii<NUM_MEN; ii++){
		gcls_pHtRecv[ii]->SetEvDetectStop();
	}
	Sleep(20);				// ��~����������҂�

	
	//======================================
	// ���т̒���
	if(bResult) {
		int     endmode = bMode ? DIV_END_STOP_MANUAL : DIV_END_STOP_AUTO;
		
		double dKenLen = gcls_Plg.GetLen_KenDist() + gcls_Plg.GetDistance();
		double dKenLenSub = gcls_Dmem.GetDmRecv().nLengthSub + gcls_Plg.GetDistance();

		gcls_pCoil->SetData_CoilResult_Real( endmode, dKenLen, dKenLenSub);
		gcls_pCoil->SetData_CoilResult_Virtual();

		double	dInLen	= gcls_Plg.GetLen_Dsp(POS_DSP_SHR_I);		// �V���[�J�b�g�ʒu�ł̓����_���R�C������
		double	dOutLen	= gcls_Plg.GetLen_OutCut();					// �V���[�J�b�g�ʒu�ł̏o�������R�C������
		
		// �o���P��
		//gcls_pCoil->SetData_CoilOutInf_Real(dInLen, dOutLen);
		//gcls_pCoil->SetData_CoilOutInf_Virtual(dInLen, dOutLen, endmode);

		// �Ō�̃V���[�J�b�g�o�^
		gcls_pCoil->SetData_ShaCut(dKenLen, 1);

		gcls_pCoil->AllDelete();
	}


	//======================================
	// �I������

	//// �X�e�[�^�X�ύX
	for(int ii=0; ii<NUM_MEN; ii++) {
		gcls_Status.SetSys(ii, DIV_SYS_STOP );
	}
	if (DIV_UNTEN_CAMERA != gcls_Status.GetUnten())
	{
		// �u�J�����Z���v���[�h�ȊO
		for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
		{
			gcls_Status.SetParaSys(ii, DIV_SYS_STOP);
		}
	}
	LOG(em_MSG), "[%s] ������~�I�I", my_sThreadName);

	//// �I�y���[�^�@�\�֌���Ԃ𑗐M
	Send_SoGamenn_MailStatus();
	OpSendFunc::SendOp_Status(); 
}

//==========================================
// �����������҂�����		(�^�C���A�E�g����)
//==========================================
void MainInstance::WaitInit_TimeOut()
{
	////////////////////////////////////
	// ���O�`�F�b�N
	////////////////////////////////////
	//// �X�e�[�^�X�m�F
	for( int ii=0; ii<NUM_MEN; ii++ )
	{
		if( DIV_SYS_INITSTART != gcls_Status.GetSys(ii) &&
			DIV_SYS_STOP      != gcls_Status.GetSys(ii) )
		{
			LOG(em_ERR), "[%s] �����������A���T�[�^�C���A�E�g �X�e�[�^�X�ُ� (%d=%s)", my_sThreadName, ii, GetDivSys( gcls_Status.GetSys(ii) ));
			return;
		}
	}

	//// �@���� �ύX
	KizuFunction::SetStatus( "CAMLINKSW_ANS_ID", false );

	//// ������~�E�J�n
	LOG(em_ERR), "[%s] �����������A���T�[�^�C���A�E�g ���������ċN�� �v��", my_sThreadName);
	syslog(235, "[�����������A���T�[�^�C���A�E�g]");
	DetectStop( false );							// ������~
	DetectReStart();								// ���������ċN��


}


//==========================================
// ����₢���킹�A���T�[
//------------------------------------------
// COMMON_QUE* que �N���v��
//==========================================
void MainInstance::HT_StatAnswer(COMMON_QUE* que)
{
	// ������
	int campair = que->fl.data[0];				// �J�����y�A (1�I���W��)
	int pcid	= que->fl.data[1];				// PCID (1�I���W��)
	int ans		= que->fl.data[2];				// ���� (0:���� 1:�y�̏� -1:�d�̏�)

	CString wk;
	wk.Format("HANTEI_ANS_%d_ID", pcid );

	//// �����ԃZ�b�g
	if(0 == ans) {
		if( 0 != gcls_Status.GetStatusRead().Ht_NonAnswer[pcid-1].stat ) {
			gcls_Status.SetStatusQue(wk, true);
		}
	} else {
		if( 0 == gcls_Status.GetStatusRead().Ht_NonAnswer[pcid-1].stat ) {
			gcls_Status.SetStatusQue(wk, false);
		}
	}
}

//==========================================
// �e�������u�ʒu�ł̃R�C���؂�ւ�
//------------------------------------------
// int	ou	: 0:�\ 1:��	
// ENUM_END_MODE enEndMode
//==========================================
void MainInstance::ChangeCoil_Ken(int ou)
{
	// ��ʁH
	bool  isDistPos;					// ��ʂ̃R�C���؂�ւ��� true
	if (ou == gcls_pParam->GetParamCommon().nDistPosition)	isDistPos = true;
	else													isDistPos = false;

	//// PLG�؂�ւ�����
	//gcls_Plg.CoilChange_Ken(ou, emEndMode);
	gcls_Plg.CoilChange_Ken(ou);

	//// ��ʂ̏ꍇ�̂� ���R�C���̐؂�ւ�
	if (isDistPos) {
		double dKenLen = gcls_Plg.GetCoilOldLen_Ken();
		double dKenLenSub = gcls_Dmem.GetDmRecv().nWpdOnLengthSub;
		gcls_pCoil->SetData_CoilResult_Real(DIV_END_NORMAL, dKenLen, dKenLenSub);		// �O�R�C���̎��т̒���
		gcls_pCoil->ChangeCoilKenBase();										// ��ʂ̃R�C���؂�ւ�
		//if (DIV_END_BACKUP == emEndMode)	gcls_pCoil->ChangeCoilKenBase(false, false);	// ��ʂ̃R�C���؂�ւ� �R�C����������B
		//else								gcls_pCoil->ChangeCoilKenBase(true, false);		//						�@�ʏ�ؑ�

	}
	else {
		gcls_pCoil->ChangeCoilKenBase_NotPos();												// �����ʂ̃R�C���؂�ւ�
	}

	//// ������ʒu�`�V���[�ʒu�܂Ńg���b�L���O���̋�����ύX����B
	//gcls_pCoil->TrkDistChange(ou, (int)gcls_Plg.GetCoilOldLen_Ken());


	int nSt_Spm	  = gcls_pCoil->GetCoilKenR(ou)->nSt_Spm;
	int nScodePre = gcls_pCoil->GetCoilIndex(gcls_pCoil->GetKenRecBase(), 1)->setting.scode[ou];
	int nScodeNow = (1 == nSt_Spm ? gcls_pCoil->GetCoilKenR(ou)->setting.scode[ou] : gcls_pCoil->GetCoilKenR(ou)->setting.scode_spmoff[ou]);

	//// �؂�ւ����R�C���̍œK�ȏ����𑼋@��֒ʒm	
	if (nScodePre != nScodeNow)
	{
		// �\�ʏ�Ԃ��ω��������̂ݒʒm
		LOG(em_INF), "[%s] <%s> <%s> �R�C���ؑ�(FACT_TO_CAMERA_05) �\�ʏ��:%d", my_sThreadName, DivNameManager::GetTorB(ou), GetSpmStat((EM_DIV_SPM_STAT)gcls_Status.GetSpmStat()), nScodeNow);
		ToOutMailSender::Send_ToCamera_CoilChange(ou, nScodeNow);
	}
	else
	{
		LOG(em_INF), "[%s] <%s> �\�ʏ��:%d �ŕω������̂��߁A�J�����R���g���[���ɒʒm����", my_sThreadName, DivNameManager::GetTorB(ou), nScodeNow);
	}
#ifndef KT_LEDMGR
	ToOutMailSender::Send_ToLights_CoilChange(ou, gcls_pCoil->GetCoilBaseKen()->setting.scode[ou]);
#else
	ToOutMailSender::Send_KtLedMgr_CoilChange(ou, nScodeNow);
	LOG(em_INF), "[%s] <%s> <%s> �ݒ�v��(�\�ʏ�Ԏw��)(FACT_KT_LEDMGR_02) �\�ʏ��:%d", my_sThreadName, DivNameManager::GetTorB(ou), GetSpmStat((EM_DIV_SPM_STAT)gcls_Status.GetSpmStat()), nScodeNow);
#endif

	//// �؂�ւ����R�C���̍œK�ȏ����𑼃X���b�h�֒ʒm
	// �G�b�W����������̖��`�F�b�N�ݒ�
	gcls_EdgePosCont.CoilChange(ou);

	// ����Y���X���b�h
#ifndef DISABLE_GAP_CALC
	gcls_pGap->AddCoilChange(ou, gcls_pCoil->GetCoilBaseKen()->setting.scode[ou]);
#endif
	
	// �����p���R�[�_�[�ɃR�C����񑗐M
#ifdef ENABLE_RECORDER_UDP
	//////int nTiming = GetPrivateProfileInt(TO_DEFECT, "REC_COIL_TIMING", 0, TASKINI_NAME);
	//////// ���M�^�C�~���O�́AKizuTask.ini�̐ݒ�ɏ]��
	//////if ((0 == nTiming &&  isDistPos) ||
	//////	(1 == nTiming && !isDistPos)) {
	//////	TYPE_EPC_COIL data	= gcls_pCoil->GetEpcCoilInfo();
	//////	gcls_pCoil->SendUdpAllFrame(&data);							// �R�C����� �� EPC���S���ۑ�PC�ɑ��M
	//////	LOG(em_INF), "[%s] <%s> ���R�[�_�[PC�ɃR�C����񑗐M [%.10s]", my_sThreadName, DivNameManager::GetTorB(ou), gcls_pCoil->GetCoilBaseKen()->data_u.CoilInf.KeyInf.cCoilNo);
	//////}
	//if (DIV_END_NORMAL == emEndMode) {
	//	// �����p���R�[�_�[�ɃR�C����񑗐M
	//	gcls_CmdSender.SendCommand_CoilInf(ou, gcls_pCoil->GetEpcCoilInfo());
	//	gcls_CmdSender.SendCommand_Param(ou, gcls_pCoil->GetEpcScode(ou));
	//	LOG(em_INF), "[%s] <%s> �����p���R�[�_�[��UDP���M[%.12s][scode=%d]",
	//		my_sThreadName, (0 == ou ? "�\��" : "����"), gcls_pCoil->GetEpcCoilInfo().cCoilNo, gcls_pCoil->GetEpcScode(ou));
	//}
	// �����p���R�[�_�[�ɃR�C����񑗐M
	gcls_CmdSender.SendCommand_CoilInf(ou, gcls_pCoil->GetEpcCoilInfo());
	int nEpcScode = (gcls_Status.IsSpmStat() ? gcls_pCoil->GetEpcScode(ou) : gcls_pCoil->GetEpcScode_spmoff(ou));
	gcls_CmdSender.SendCommand_Param(ou, nEpcScode);
	LOG(em_INF), "[%s] <%s> �����p���R�[�_�[��UDP���M[%.12s][scode=%d][NowF=%d]",
		my_sThreadName, DivNameManager::GetTorB(ou), gcls_pCoil->GetEpcCoilInfo().cCoilNo, nEpcScode, gcls_Plg.GetFrameNo());
#endif

	// �O�񑗐M�����R�}���h�����N���A
	memset(m_strCmd, 0x00, sizeof(m_strCmd));
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// WPD����
//==========================================
// �����pWPD�F��
//------------------------------------------
// int nIsWpd		: 0:WPD, 1:�َ�_, 2:�Q�[�g�M��	
// double *pSpmDiff : SPM��Ԃɂ��L�ї�[mm]
//==========================================
void MainInstance::WPD_KenBase(int nIsWpd, double *pSpmDiff)
{
	gcls_Lock.LockTh();	//// -------------->>>>>

	//// �������`�F�b�N
	// ===>> �⊪ 2021.06.03 ���R�C�����ޒʔ̎��A�R�C������r������ɂł��Ȃ��s��C��
	//const COIL_INF*  pCoil = gcls_pCoil->GetCoilBaseKen();
	int nRecKenBase = gcls_pCoil->GetKenRecBase();
	COIL_INF*  pCoil = gcls_pCoil->GetCoilRec_DataSet(nRecKenBase);
	// <<===
	// �t���[�����ꂪ�������邽�߁A�ʔ̋����A�t���[��No�͓����^�C�~���O�Ŏ擾����
	double dKenLen		= gcls_Plg.GetLen_KenDist();	// ��ʌ������u�ʒu�ł̒ʔ��� [mm]
	double dKenLenNot	= gcls_Plg.GetLen_KenDistNot();	// �����ʌ������u�ʒu�ł̒ʔ��� [mm]
	DWORD nNowFrame		= gcls_Plg.GetFrameNo();		// ���݂̃t���[��No
	if(!pCoil->bCoilInit) {
		// ===>> �⊪ 2021.06.03 ���R�C�����ޒʔ̎��A�R�C������r������ɂł��Ȃ��s��C��
		//SEQ_DM_RECV_BASE &tbl = gcls_Dmem.GetDmRecv();
		//double dWpdOnKenLen = tbl.nWpdOnLength;
		//double dSubLen = tbl.nWpdOnLengthSub;
		//double dSa = dWpdOnKenLen - dSubLen;	// �������`�F�b�N

		//if( 0.0 < gcls_pParam->GetCommonMast().dPlgDiffLimit[0] &&
		//	(tbl.nWpdOnLength * gcls_pParam->GetCommonMast().dPlgDiffLimit[0] / 100) <= abs((int)dSa) ) {
		//	LOG(em_ERR), "[PlgManager] PLG �������`�F�b�N �ُ� [�����pPLG=%0.3fm:��r�pPLG=%0.3fm:��=%0.3fm]", tbl.nWpdOnLength /1000.0, tbl.nWpdOnLengthSub /1000.0, dSa/1000.0);
		//	syslog(SYSNO_PLG_ERR, "[�����pPLG=%0.3fm:��r�pPLG=%0.3fm:��=%0.3fm]", tbl.nWpdOnLength /1000.0, tbl.nWpdOnLengthSub /1000.0, dSa/1000.0);
		//	if( 0== gcls_Status.GetStatusRead().PlgLengthCheckKen.stat) {				// ���큨�ُ�
		//		gcls_Status.SetStatusQue("PLGLEN_KEN_CHECK_ID", false);
		//	}
		//} else {
		//	LOG(em_MSG), "[PlgManager] PLG �������`�F�b�N ���� [�����pPLG=%0.3fm:��r�pPLG=%0.3fm:��=%0.3fm]", tbl.nWpdOnLength /1000.0, tbl.nWpdOnLengthSub /1000.0, dSa/1000.0);

		//	if( 0!= gcls_Status.GetStatusRead().PlgLengthCheckKen.stat) {				// �ُ큨����
		//		LOG(em_MSG), "[PlgManager] PLG �������`�F�b�N ���한�A");
		//		gcls_Status.SetStatusQue("PLGLEN_KEN_CHECK_ID", true);
		//	}
		//}

		double dWpdOnKenLen = gcls_Dmem.GetWpdOnLength();
		double dSubLen = gcls_Dmem.GetWpdOnLengthSub();
		double dSa = dWpdOnKenLen - dSubLen;	// �������`�F�b�N
		pCoil->dWpdOnLength = dWpdOnKenLen;	// WPD ON���̃R�C������ێ�

		if (0.0 < gcls_pParam->GetCommonMast().dPlgDiffLimit[0] &&
			(dWpdOnKenLen * gcls_pParam->GetCommonMast().dPlgDiffLimit[0] / 100) <= abs((int)dSa)) {
			LOG(em_ERR), "[PlgManager] PLG �������`�F�b�N �ُ� [�����pPLG=%0.3fm:��r�pPLG=%0.3fm:��=%0.3fm]", dWpdOnKenLen / 1000.0, dSubLen / 1000.0, dSa / 1000.0);
			syslog(SYSNO_PLG_ERR, "[�����pPLG=%0.3fm:��r�pPLG=%0.3fm:��=%0.3fm]", dWpdOnKenLen / 1000.0, dSubLen / 1000.0, dSa / 1000.0);
			if (0 == gcls_Status.GetStatusRead().PlgLengthCheckKen.stat) {				// ���큨�ُ�
				gcls_Status.SetStatusQue("PLGLEN_KEN_CHECK_ID", false);
			}
		}
		else {
			LOG(em_MSG), "[PlgManager] PLG �������`�F�b�N ���� [�����pPLG=%0.3fm:��r�pPLG=%0.3fm:��=%0.3fm]", dWpdOnKenLen / 1000.0, dSubLen / 1000.0, dSa / 1000.0);

			if (0 != gcls_Status.GetStatusRead().PlgLengthCheckKen.stat) {				// �ُ큨����
				LOG(em_MSG), "[PlgManager] PLG �������`�F�b�N ���한�A");
				gcls_Status.SetStatusQue("PLGLEN_KEN_CHECK_ID", true);
			}
		}
		// <<===

		// ����LAN HUB�ُ̈�œ���-����Ԃ̉���ؒf��������
		// [����:������~�A����:������]�̃A���}�b�`��ԂƂȂ邪
		// �����R�C���͐؂�ւ�邪�A�_���R�C�����؂�ւ��Ȃ���Ԃ�
		// �ُ팟�o����Ȃ����߁A�R�C���ؑւ��̃^�C�~���O�Ń`�F�b�N���s��
		for (int ii = 0; ii<NUM_MEN; ii++) {
			bool bFrameNo = true;
			if (DIV_SYS_RUN != gcls_Status.GetSys(ii)) {
				// �������ȊO�̓`�F�b�N�ΏۊO
				continue;
			}

			for (int jj = 0; jj<NUM_CAM_POS; jj++) {
				DWORD nFrameNo_V = gcls_pHtRecv[ii]->GetFrameNo(jj);
				if (m_nFrameNo_V[ii][jj] == nFrameNo_V) {
					LOG(em_ERR), "[%s] �B���t���[����~���o <%s>[Cam:%d FrameNo:%d]", my_sThreadName, DivNameManager::GetTorB(ii), (jj + 1) + (ii*NUM_CAM_POS), nFrameNo_V);
					syslog(245, "�B���t���[����~���o <%s>[Cam:%d FrameNo:%d]", DivNameManager::GetTorB(ii), (jj + 1) + (ii*NUM_CAM_POS), nFrameNo_V);

					bFrameNo = false;
				}
				m_nFrameNo_V[ii][jj] = nFrameNo_V;			// �O��t���[��No�ێ�
			}
			if (!bFrameNo) {
				if (0 == gcls_Status.GetStatusRead().FrameGrabStop[ii].stat) {				// ���큨�ُ�
					LOG(em_ERR), "[%s] �B���t���[����~���o �@���ԍX�V <%s>", my_sThreadName, DivNameManager::GetTorB(ii));
					gcls_Status.SetStatusQue(gcls_Status.GetStatusRead().FrameGrabStop[ii].id, false);
				}
			} else {
				if (0 != gcls_Status.GetStatusRead().FrameGrabStop[ii].stat) {				// �ُ큨����
					LOG(em_MSG), "[%s] �B���t���[���������o �@���ԍX�V <%s>", my_sThreadName, DivNameManager::GetTorB(ii));
					gcls_Status.SetStatusQue(gcls_Status.GetStatusRead().FrameGrabStop[ii].id, true);
				}
			}
		}
	}


	// ������_�œ����R�C���؂�ւ�
	gcls_Plg.CoilChange_KenBase();

	// �g���b�L���O�J�n
	int nVal = gcls_pParam->GetCommonMast().Dist.nKenBase;
	// SPM����������Ă����狗����␳
	if (gcls_Status.IsSpmStat())
	{
		// ������_����SPM�܂ł̋�����SPM�L�ї��ɂ�鍷�������߉��Z����
		// ���O�R�C���̔��[���̕␳
		*pSpmDiff = (double)gcls_pParam->GetCommonMast().Dist.nKenBaseSpm * gcls_pCoil->GetLastSpm();
		nVal = (int)ceil((double)nVal + *pSpmDiff);

		LOG(em_MSG), "[%s] SPM�L�ї��␳(����) SPM�L�ї��F%.4f%% �␳�O�F%.2fm �� �␳��F%.2fm (�����F%.2fmm)", my_sThreadName,
			gcls_pCoil->GetLastSpm(),
			(double)gcls_pParam->GetCommonMast().Dist.nKenBase / 1000.0,
			(double)nVal / 1000.0,
			*pSpmDiff);
	}

	gcls_pTrKenDistPos->ChangeDist(nVal);		// �p�����[�^���ύX����Ă邩���m��Ȃ��̂ŁA����Z�b�g
	gcls_pTrKenDistPos->g_pAddr = (long*)nIsWpd;
	gcls_pTrKenDistPos->TrackingStart();

	double dLenYotei = dKenLen + nVal;			// �R�C���\�蒷�Z�b�g
	gcls_pCoil->SetData_CoilLenYotei(dLenYotei);

	LOG(em_MSG), "[%s] �R�C���\�蒷=%0.3fm(%0.3fm + %0.3fm)", my_sThreadName, dLenYotei / 1000.0, dKenLen / 1000.0, nVal / 1000.0);
	LOG(em_MSG), "[%s] WPD ON���̃t���[��No=%lu", my_sThreadName, nNowFrame);

	//// �R�C���ؑ֎��̓��������J�E���^�ݒ�
	SetPosition_CoilChange(nNowFrame, dKenLen, dKenLenNot, *pSpmDiff);
	
	// ���ɃR�C��������M���Ă���ꍇ�͕\��PC�Ɍ����p�R�C�����𑗐M����
	if( gcls_pCoil->IsCoilNextKen() ){
		////// �R�C����� ��\��PC�ɑ��� 
		//// �� �]���́A�O�H������O�����Ď擾���邽�߂ɃR�C������M���ɑ��M���Ă������A
		//		�����Ǘ�No���R�C������M���ɍX�V����������WPD ON���R�C������M�ςݎ��ɑ��M����
		OpSendFunc::SendOp_CoilInf_Ken( gcls_pCoil->GetCoilNextKen() );
	}
	gcls_Lock.UnlockTh();	//// <<<<<--------------
}


//==========================================
// �\���pWPD�F��
//==========================================
void MainInstance::WPD_DspBase()
{
	gcls_Lock.LockTh();		//// <<<<--------------------

	
	//// �������`�F�b�N
	const COIL_INF*  pCoil = gcls_pCoil->GetCoilBaseDsp();
	if (!pCoil->bCoilInit) {
		// ===>> �⊪ 2021.06.03 ���R�C�����ޒʔ̎��A�R�C������r������ɂł��Ȃ��s��C��
		//SEQ_DM_RECV_BASE &tbl = gcls_Dmem.GetDmRecv();
		//double dKenLen = tbl.nWpdOnLength;
		//double dDspLen = tbl.nWpdOnLengthDsp;
		//double dSa = dKenLen - dDspLen;	// �������`�F�b�N

		//if( 0.0 < gcls_pParam->GetCommonMast().dPlgDiffLimit[1] &&
		//	(tbl.nWpdOnLength * gcls_pParam->GetCommonMast().dPlgDiffLimit[1] / 100) <= abs((int)dSa) ) {
		//	LOG(em_ERR), "[PlgManager] �\��PLG �������`�F�b�N �ُ� [�����pPLG=%0.3fm:�\���pPLG=%0.3fm:��=%0.3fm]", tbl.nWpdOnLength /1000.0, tbl.nWpdOnLengthDsp /1000.0, dSa/1000.0);
		//	syslog(SYSNO_PLG_ERR, "[�����pPLG=%0.3fm:�\���pPLG=%0.3fm:��=%0.3fm]", tbl.nWpdOnLength /1000.0, tbl.nWpdOnLengthDsp /1000.0, dSa/1000.0);
		//	if( 0==gcls_Status.GetStatusRead().PlgLengthCheckDsp.stat) {				// ���큨�ُ�
		//		gcls_Status.SetStatusQue("PLGLEN_DSP_CHECK_ID", false);
		//	}
		//} else {
		//	LOG(em_MSG), "[PlgManager] �\��PLG �������`�F�b�N [�����pPLG=%0.3fm:�\���pPLG=%0.3fm:��=%0.3fm]", tbl.nWpdOnLength /1000.0, tbl.nWpdOnLengthDsp /1000.0, dSa/1000.0);

		//	if( 0!= gcls_Status.GetStatusRead().PlgLengthCheckDsp.stat) {				// �ُ큨����
		//		LOG(em_MSG), "[PlgManager] �\��PLG �������`�F�b�N ���한�A");
		//		gcls_Status.SetStatusQue("PLGLEN_DSP_CHECK_ID", true);
		//	}
		//}
		double dKenLen = pCoil->dWpdOnLength;
		double dDspLen = gcls_Dmem.GetWpdOnLengthDsp();
		double dSa = dKenLen - dDspLen;	// �������`�F�b�N

		if (0.0 < gcls_pParam->GetCommonMast().dPlgDiffLimit[1] &&
			(dKenLen * gcls_pParam->GetCommonMast().dPlgDiffLimit[1] / 100) <= abs((int)dSa)) {
			LOG(em_ERR), "[PlgManager] �\��PLG �������`�F�b�N �ُ� [�����pPLG=%0.3fm:�\���pPLG=%0.3fm:��=%0.3fm]", dKenLen / 1000.0, dDspLen / 1000.0, dSa / 1000.0);
			syslog(SYSNO_PLG_ERR, "[�����pPLG=%0.3fm:�\���pPLG=%0.3fm:��=%0.3fm]", dKenLen / 1000.0, dDspLen / 1000.0, dSa / 1000.0);
			if (0 == gcls_Status.GetStatusRead().PlgLengthCheckDsp.stat) {				// ���큨�ُ�
				gcls_Status.SetStatusQue("PLGLEN_DSP_CHECK_ID", false);
			}
		}
		else {
			LOG(em_MSG), "[PlgManager] �\��PLG �������`�F�b�N [�����pPLG=%0.3fm:�\���pPLG=%0.3fm:��=%0.3fm]", dKenLen / 1000.0, dDspLen / 1000.0, dSa / 1000.0);

			if (0 != gcls_Status.GetStatusRead().PlgLengthCheckDsp.stat) {				// �ُ큨����
				LOG(em_MSG), "[PlgManager] �\��PLG �������`�F�b�N ���한�A");
				gcls_Status.SetStatusQue("PLGLEN_DSP_CHECK_ID", true);
			}
		}
		// <<===
	}


	//// �������s����ׂɁA�e�N���X�ŃN���e�B�J���Z�N�V�������`���Ă���
	//// �R�C���ؑւ����������ꍇ�̂݁APLG������������ (���s�̏ꍇ�́A���̕\���p�R�C���������Ƒ����Ă���悤�ɂ���)

	double dNowDspLen = 0.0;
#ifdef DSP_NO_TRACKING
	dNowDspLen = gcls_Plg.GetCoilOldLen_Ken();
#else
	dNowDspLen = gcls_Plg.GetLen_BaseDsp();
#endif
	// �\���p�̃R�C������ؑ�
	if (gcls_pCoil->ChangeCoilDspBase(dNowDspLen)) {
		// �\���p��PLG����ؑ�
		gcls_Plg.CoilChange_BaseDsp();
	}

	gcls_Lock.UnlockTh();	//// -------------------->>>>
}


//==========================================
// �R�C���ؑ֎��̓��������J�E���^�ݒ�v��
//==========================================
void MainInstance::SetPosition_CoilChange(DWORD nNowFrame, double dKenDist, double dKenDistNot, double dSpmDiff)
{
	double dFrameLen	= gcls_Plg.GetResV() * SIZE_FRAME_IMG_Y;		// 1�t���[������[mm]
	double dDistLen		= gcls_pParam->GetParamCommon().dDistLen;		// �\���ԋ���[mm]
	int nKenBase		= gcls_pParam->GetCommonMast().Dist.nKenBase;	// ������_�`�㗬�J����[mm]

	bool  isDistPos;					// ��� true
	NCL_CMD_SET_POSITION	cmd;		// ���������J�E���^�\����

	memset(&cmd, 0x00, sizeof(cmd));
	cmd.cmd = NCL_SET_POSITION;
	cmd.update_mask = FRONT | TAIL;
	for(int ii=0; ii<NUM_MEN; ii++) {
		// ��ʁH
		if(ii == gcls_pParam->GetParamCommon().nDistPosition)	isDistPos = true;
		else													isDistPos = false;

		//double dLen	= gcls_pParam->GetCommonMast().Dist.nKenBase;			// ������_�`�㗬�J����[mm]
		//if(!isDistPos) dLen += gcls_pParam->GetParamCommon().dDistLen;		// ������_�`�����J����[mm]

		//double front_pos	= 0.0;
		//int    framenum		= 0;
		//// �����ʂ̃R�C���擪�t���[���́A0m�ƂȂ�Ȃ��B�\���ԋ������t���[���P�ʂŊ���؂�Ȃ����̌덷�������ŋz���B(�\���ԋ����������ɍl������)
		//// ���s�t���[���ԍ��������ʂ͑O��R�C�����𒴉߂������ɐؑւ̂��߁A+1�t���[���ƂȂ�
		////if(!isDistPos) front_pos = (int)((gcls_pParam->GetCommonMast().Dist.nKenBase / dDistance) + 1) * dDistance - gcls_pParam->GetParamCommon().dDistLen;
		//if(!isDistPos) front_pos = ((int)(dDistLen/dFrameLen)+1) * dFrameLen - dDistLen;
		//if(!isDistPos) framenum = 1;

		//cmd.framenum = (int)ceil(dLen / dFrameLen) + nNowFrame + framenum;	// ���s�t���[���ԍ�
		//cmd.front_pos = (float)front_pos;									// ���Z�J�E���^[mm]
		//cmd.tail_pos = (float)gcls_pCoil->GetCoilNextKen()->data.nLenIn * 1000;	// ���Z�J�E���^(�\��R�C����)[mm]

		int nOrgFrameNo = 0;
		if(isDistPos) {
			nOrgFrameNo = (int)ceil(nKenBase / dFrameLen) + nNowFrame;
			cmd.framenum = (int)ceil(((double)nKenBase + dSpmDiff) / dFrameLen) + nNowFrame;	// ���s�t���[���ԍ�
			cmd.front_pos = (float)0.0;											// ���Z�J�E���^[mm]
			cmd.tail_pos = (float)gcls_pCoil->GetCoilNextKen()->data.nLenIn;	// ���Z�J�E���^(�\��R�C����)[mm]
		} else {
			double dCoilLen_Ken	= (int)ceil((dKenDist + nKenBase + dSpmDiff) / dFrameLen) * dFrameLen;	// ��ʂ̃R�C���ؑ֋���(SPM��������)
			double dOrgCoilLen_Ken = (int)ceil((dKenDist + nKenBase) / dFrameLen) * dFrameLen;			// ��ʂ̃R�C���ؑ֋���(SPM�����Ȃ�)
			double dLen	= dCoilLen_Ken - dKenDistNot;							// ��ʐؑ֋��� - �����ʒʔ��� [mm]
			
			nOrgFrameNo = (int)ceil((dOrgCoilLen_Ken - dKenDistNot) / dFrameLen) + nNowFrame;
			//nOrgFrameNo = (int)ceil(dLen / dFrameLen) + nNowFrame;

			int nChgFrame = (int)ceil(dLen / dFrameLen);			// �����ʂ̌��݈ʒu����R�C���ؑւ܂ł̃t���[����
			//int nChgFrame = (int)ceil((dLen + dSpmDiff) / dFrameLen);			// �����ʂ̌��݈ʒu����R�C���ؑւ܂ł̃t���[����
			// �����ʂ̃R�C���擪�t���[���́A0m�ƂȂ�Ȃ��B�\���ԋ������t���[���P�ʂŊ���؂�Ȃ����̌덷�������ŋz���B(�\���ԋ����������ɍl������)
			double front_pos	= (dKenDistNot + (nChgFrame * dFrameLen)) - dCoilLen_Ken;	// �����ʂ̃R�C���ؑ֎��̐擪�ʒu
			cmd.framenum = nChgFrame + nNowFrame;								// ���s�t���[���ԍ�
			cmd.front_pos = (float)front_pos;									// ���Z�J�E���^[mm]
			cmd.tail_pos = (float)gcls_pCoil->GetCoilNextKen()->data.nLenIn - (float)front_pos;	// ���Z�J�E���^(�\��R�C����)[mm]
		}

		LOG(em_INF), "[%s] SPM�L�ї��␳(���s�t���[��) [%s] (SPM diff�F%.2fmm 1frame�F%.2fmm) �␳�O=%d �� �␳��=%d", my_sThreadName, DivNameManager::GetTorB(ii), dSpmDiff, dFrameLen, nOrgFrameNo, cmd.framenum);

		// �p�����[�^�ؑ֗v��
		LOG(em_INF), "[%s] �p�����[�^�ؑ֗v�� [%s] FrameNo:%d(%0.3fm) (���s=%d Scode:%d)", my_sThreadName, DivNameManager::GetTorB(ii), nNowFrame, gcls_Plg.GetLen_Ken(ii)/1000.0, cmd.framenum, gcls_pCoil->GetCoilNextKen()->setting.scode[ii]);
		int scode	= MAX_SCODE;
		if(0 != gcls_pCoil->GetCoilNextKen()->setting.scode[ii]) scode = gcls_pCoil->GetCoilNextKen()->setting.scode[ii];
		Send_HtDefect_Param(cmd.framenum, ii, scode, DIV_HANTEI_NON);

		LOG(em_INF), "[%s] ���������J�E���^�ݒ� [%s] FrameNo:%d(%0.3fm) (���s=%d F:%.0fmm T:%.0fmm)", my_sThreadName, DivNameManager::GetTorB(ii), nNowFrame, gcls_Plg.GetLen_Ken(ii)/1000.0, cmd.framenum, cmd.front_pos, cmd.tail_pos);
		Send_HtDefect_SetPosition(ii, DIV_HANTEI_NON, &cmd);				// ���������J�E���^�ݒ�v��

#ifdef ENABLE_RPI_SAVE
		// �S���؂�ւ�
		LOG(em_MSG), "[%s] FACT_TI_RPISAVE_04 ���M [men=%d][%s][%s][fno=%d][fpos=%d]"
			, my_sThreadName, ii, gcls_pCoil->GetCoilNextKen()->cKizukenNo, gcls_pCoil->GetCoilNextKen()->data.cCoilNoIn, cmd.framenum, cmd.front_pos);
		mcls_pIfRpiSave->Send_TiRpiSave_CoilSend(gcls_pCoil->GetCoilNextKen()->cKizukenNo, &gcls_pCoil->GetCoilNextKen()->setting, &gcls_pCoil->GetCoilNextKen()->data, cmd.framenum, (int)cmd.front_pos, ii);
#endif
		// ���񑗐M�̃R�}���h����ێ�
		memcpy(&m_strCmd[ii], &cmd, sizeof(NCL_CMD_SET_POSITION));

		// ���s�t���[��No��ێ�
		// PLG�̃R�C���ؑփt���[��No�F���Ɏg�p����
		gcls_Plg.SetCoilChange_Frame(ii, cmd.framenum);
	}
}

//==========================================
// �R�C�������ؑ֎��̓��������J�E���^�ݒ�v��(�����ؑ�)
//==========================================
void MainInstance::SetPosition_ForceCoilChange(DWORD nNowFrame, double dKenDist, double dKenDistNot)
{
	double dFrameLen	= gcls_Plg.GetResV() * SIZE_FRAME_IMG_Y;		// 1�t���[������[mm]
	double dDistLen		= gcls_pParam->GetParamCommon().dDistLen;		// �\���ԋ���[mm]
	int nKenBase		= gcls_pParam->GetCommonMast().Dist.nKenBase;	// ������_�`�㗬�J����[mm]

	bool  isDistPos;					// ��� true
	NCL_CMD_SET_POSITION	cmd;		// ���������J�E���^�\����

	memset(&cmd, 0x00, sizeof(cmd));
	cmd.cmd = NCL_SET_POSITION;
	cmd.update_mask = FRONT | TAIL;
	for(int ii=0; ii<NUM_MEN; ii++) {
		// ��ʁH
		if(ii == gcls_pParam->GetParamCommon().nDistPosition)	isDistPos = true;
		else													isDistPos = false;

		//double dLen			= 0.0;
		//if(!isDistPos) dLen += gcls_pParam->GetParamCommon().dDistLen;		// ������_�`�����J����[mm]

		//double front_pos	= 0.0;
		//int    framenum		= 0;
		//// �����ʂ̃R�C���擪�t���[���́A0m�ƂȂ�Ȃ��B�\���ԋ������t���[���P�ʂŊ���؂�Ȃ����̌덷�������ŋz���B(�\���ԋ����������ɍl������)
		//// ���s�t���[���ԍ��������ʂ͑O��R�C�����𒴉߂������ɐؑւ̂��߁A+1�t���[���ƂȂ�
		//if(!isDistPos) front_pos = ((int)(dDistLen/dFrameLen)+1) * dFrameLen - dDistLen;
		//if(!isDistPos) framenum = 1;

		//cmd.framenum = (int)ceil(dLen / dFrameLen) + nNowFrame + framenum;	// ���s�t���[���ԍ�(���t���[���Őؑւ��A���X�t���[���Ŏ��R�C���J�n)
		//cmd.front_pos = (float)front_pos;									// ���Z�J�E���^[mm]
		//cmd.tail_pos = (float)gcls_pCoil->GetCoilNextKen()->data.nLenIn * 1000;	// ���Z�J�E���^(�\��R�C����)[mm]
		

		if(isDistPos) {
			// 2024.02.22 mizoguchi --->>>
			//cmd.framenum = 0;													// ���s�t���[���ԍ�
			cmd.framenum = nNowFrame+1;													// ���s�t���[���ԍ�
			// 2024.02.22 mizoguchi <<<---
			cmd.front_pos = (float)0.0;											// ���Z�J�E���^[mm]
			cmd.tail_pos = (float)gcls_pCoil->GetCoilNextKen()->data.nLenIn;	// ���Z�J�E���^(�\��R�C����)[mm]
		} else {
			double dCoilLen_Ken	= dKenDist + (1.0 * dFrameLen);					// ��ʂ̃R�C���ؑ֋���
			double dLen	= dCoilLen_Ken - dKenDistNot;							// ��ʐؑ֋��� - �����ʒʔ��� [mm]
			int nChgFrame = (int)ceil(dLen / dFrameLen);						// �����ʂ̌��݈ʒu����R�C���ؑւ܂ł̃t���[����
			// �����ʂ̃R�C���擪�t���[���́A0m�ƂȂ�Ȃ��B�\���ԋ������t���[���P�ʂŊ���؂�Ȃ����̌덷�������ŋz���B(�\���ԋ����������ɍl������)
			double front_pos	= (dKenDistNot + (nChgFrame * dFrameLen)) - dCoilLen_Ken;	// �����ʂ̃R�C���ؑ֎��̐擪�ʒu
			cmd.framenum = nChgFrame + nNowFrame;								// ���s�t���[���ԍ�
			cmd.front_pos = (float)front_pos;									// ���Z�J�E���^[mm]
			cmd.tail_pos = (float)gcls_pCoil->GetCoilNextKen()->data.nLenIn - (float)front_pos;	// ���Z�J�E���^(�\��R�C����)[mm]
		}

		// ===>> �⊪�@2018.10.21 �R�C������M�`�R�C���؂�ւ��̊ԂɃC�j�V�����`������M�����Ƃ��A�G���W�������؂�ւ�������Ȃ��s�
		// �����s��PARAM,SET_POSITION������
		LOG(em_INF), "[%s] �O�񑗐M�̖����s�ݒ�̃L�����Z���v��", my_sThreadName);
		Send_HtDefect_ClearParam(ii, DIV_HANTEI_NON);
		// <<===

		// �p�����[�^�ؑ֗v��
		LOG(em_INF), "[%s] �p�����[�^�ؑ֗v��(����) [%s] FrameNo:%d (���s=%d Scode:%d)", my_sThreadName, DivNameManager::GetTorB(ii), nNowFrame, cmd.framenum, gcls_pCoil->GetCoilNextKen()->setting.scode[ii]);
		int scode	= MAX_SCODE;
		if(0 != gcls_pCoil->GetCoilNextKen()->setting.scode[ii]) scode = gcls_pCoil->GetCoilNextKen()->setting.scode[ii];
		Send_HtDefect_Param(cmd.framenum, ii, scode, DIV_HANTEI_NON);

		LOG(em_INF), "[%s] ���������J�E���^�ݒ�(����) [%s] FrameNo:%d (���s=%d F:%.0fmm T:%.0fmm)", my_sThreadName, DivNameManager::GetTorB(ii), nNowFrame, cmd.framenum, cmd.front_pos, cmd.tail_pos);
		Send_HtDefect_SetPosition(ii, DIV_HANTEI_NON, &cmd);				// ���������J�E���^�ݒ�v��

#ifdef ENABLE_RPI_SAVE
		// �S���؂�ւ�
		LOG(em_MSG), "[%s] FACT_TI_RPISAVE_04(����) ���M [men=%d][%s][%s][fno=%d][fpos=%d]"
			, my_sThreadName, ii, gcls_pCoil->GetCoilNextKen()->cKizukenNo, gcls_pCoil->GetCoilNextKen()->data.cCoilNoIn, cmd.framenum, cmd.front_pos);
		mcls_pIfRpiSave->Send_TiRpiSave_CoilSend(gcls_pCoil->GetCoilNextKen()->cKizukenNo, &gcls_pCoil->GetCoilNextKen()->setting, &gcls_pCoil->GetCoilNextKen()->data, cmd.framenum, (int)cmd.front_pos, ii);
#endif

		// ���s�t���[��No��ێ�
		// PLG�̃R�C���ؑփt���[��No�F���Ɏg�p����
		gcls_Plg.SetCoilChange_Frame(ii, cmd.framenum);

	}
}

//==========================================
// ���R�C������M���̃p�����[�^�ݒ�v��
//==========================================
void MainInstance::SetParam_NextCoilInfRecv()
{
	// �R�C����񂪐�ɗ��Ă�����X���[����
	if (0 == m_strCmd[0].cmd && 0 == m_strCmd[1].cmd) {
		LOG(em_INF), "[%s] ���R�C������M���̃p�����[�^�ؑ֗v�� �X���[", my_sThreadName);
		return;
	}

	DWORD nNowFrame		= gcls_Plg.GetFrameNo();						// ���݂̃t���[��No
	//double dDistance	= gcls_Plg.GetDistance();						// 1�t���[������[mm]
	double dFrameLen	= gcls_Plg.GetResV() * SIZE_FRAME_IMG_Y;		// 1�t���[������[mm]
	double dDistLen		= gcls_pParam->GetParamCommon().dDistLen;		// �\���ԋ���[mm]

	bool  isDistPos;					// ��� true
	NCL_CMD_SET_POSITION	cmd;		// ���������J�E���^�\����

	for(int ii=0; ii<NUM_MEN; ii++) {
		memcpy(&cmd, &m_strCmd[ii], sizeof(NCL_CMD_SET_POSITION));
		// ��ʁH
		if(ii == gcls_pParam->GetParamCommon().nDistPosition)	isDistPos = true;
		else													isDistPos = false;

		double dLen	= gcls_pParam->GetCommonMast().Dist.nKenBase;			// ������_�`�㗬�J����[mm]
		if(!isDistPos) dLen += gcls_pParam->GetParamCommon().dDistLen;		// ������_�`�����J����[mm]

		//double front_pos	= 0.0;
		//int    framenum		= 0;
		// �����ʂ̃R�C���擪�t���[���́A0m�ƂȂ�Ȃ��B�\���ԋ������t���[���P�ʂŊ���؂�Ȃ����̌덷�������ŋz���B(�\���ԋ����������ɍl������)
		// ���s�t���[���ԍ��������ʂ͑O��R�C�����𒴉߂������ɐؑւ̂��߁A+1�t���[���ƂȂ�
		//if(!isDistPos) front_pos = (int)((gcls_pParam->GetCommonMast().Dist.nKenBase / dDistance) + 1) * dDistance - gcls_pParam->GetParamCommon().dDistLen;
		//if(!isDistPos) front_pos = ((int)(dDistLen/dFrameLen)+1) * dFrameLen - dDistLen;
		//if(!isDistPos) framenum = 1;

		//cmd.framenum = (int)ceil(dLen / dFrameLen) + nNowFrame + framenum;	// ���s�t���[���ԍ�
		//cmd.front_pos = (float)front_pos;									// ���Z�J�E���^[mm]
		//cmd.tail_pos = (float)gcls_pCoil->GetCoilNextKen()->data.nLenIn * 1000;	// ���Z�J�E���^(�\��R�C����)[mm]

		// �����s��PARAM,SET_POSITION������
		LOG(em_INF), "[%s] �O�񑗐M�̖����s�ݒ�̃L�����Z���v��" , my_sThreadName);
		Send_HtDefect_ClearParam(ii, DIV_HANTEI_NON);


		// �p�����[�^�ؑ֗v��
		LOG(em_INF), "[%s] ���R�C������M���̃p�����[�^�ؑ֗v�� [%s] FrameNo:%d(%0.3fm) (���s=%d Scode:%d)", my_sThreadName, DivNameManager::GetTorB(ii), nNowFrame, dLen/1000.0, cmd.framenum, gcls_pCoil->GetCoilNextKen()->setting.scode[ii]);
		int scode	= MAX_SCODE;
		if(0 != gcls_pCoil->GetCoilNextKen()->setting.scode[ii]) scode = gcls_pCoil->GetCoilNextKen()->setting.scode[ii];
		Send_HtDefect_Param(cmd.framenum, ii, scode, DIV_HANTEI_NON);

		LOG(em_INF), "[%s] ���������J�E���^�ݒ� [%s] FrameNo:%d(%0.3fm) (���s=%d F:%.0fmm T:%.0fmm)", my_sThreadName, DivNameManager::GetTorB(ii), nNowFrame, dLen/1000.0, cmd.framenum, cmd.front_pos, cmd.tail_pos);
		Send_HtDefect_SetPosition(ii, DIV_HANTEI_NON, &cmd);				// ���������J�E���^�ݒ�v��

#ifdef ENABLE_RPI_SAVE
		// �S���؂�ւ�
		LOG(em_MSG), "[%s] FACT_TI_RPISAVE_04 ���M [men=%d][%s][%s][fno=%d][fpos=%d]"
			, my_sThreadName, ii, gcls_pCoil->GetCoilNextKen()->cKizukenNo, gcls_pCoil->GetCoilNextKen()->data.cCoilNoIn, cmd.framenum, cmd.front_pos);
		mcls_pIfRpiSave->Send_TiRpiSave_CoilSend(gcls_pCoil->GetCoilNextKen()->cKizukenNo, &gcls_pCoil->GetCoilNextKen()->setting, &gcls_pCoil->GetCoilNextKen()->data, cmd.framenum, (int)cmd.front_pos, ii);
#endif

	}
	// �O��WPD�M����M���̓��������J�E���^�\���� ������
	memset(m_strCmd, 0x00, sizeof(m_strCmd));
}

//------------------------------------------
// DB�o�^�󋵖₢���킹
// COMMON_QUE* que �N���v��
//------------------------------------------
void MainInstance::Add_Synclock(COMMON_QUE* que)
{
	int okng = 0;
	const COIL_INF *pCoil = gcls_pCoil->GetCoilKizukenNo(que->mix.cdata[1]);

	//// ����������ʂɂ���ꍇ�́A�������ɑ���Ԃ��B(������f�[�^�����ŏ������Ă�)
	if( 200 < gcls_DB.gque_AddSQLData.GetCount() ) {
		LOG(em_ERR), "[%s] �o�^�󋵖₢���킹�B���łɑ�ʃf�[�^�L��ׁ̈A�₢���킹�����", my_sThreadName);
		okng = 1;
	}

	//================================================
	//// ���O����
	DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
	deliDB->kubun = DBManager::em_DB_ADD_SYNC;
	deliDB->data = (BYTE*)new DBDATA_ADD_SYNC;
	DBDATA_ADD_SYNC* pWk   = (DBDATA_ADD_SYNC*)deliDB->data;		// �G�C���A�X
	
	//================================================
	//// �f�[�^�Z�b�g
	memcpy( pWk->sendItem.cTask, TO_PUTDAT, SIZE_NAME_TASK);
	memcpy( pWk->sendItem.cPc, ".", SIZE_NAME_PC);
	pWk->sendItem.Que.nEventNo = FACT_TO_PUTDAT_04;
	pWk->sendItem.Que.nLineNo  = getlineid();
	// ���� (0:���� 1:�ُ�)
	pWk->sendItem.Que.fl.data[0] = okng;

	//================================================
	//// �o�^�˗�
	if( ! gcls_DB.gque_AddSQLData.AddToTailFreeCheck(deliDB) ) {	// �o�^
		LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [DBDATA_ADD_SYNC]", my_sThreadName);
		syslog(SYSNO_QUEFULL_WAR, "[DBDATA_ADD_SYNC]");
		delete deliDB->data;
		delete deliDB;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �O���^�X�N�ʒm
//// HT_DEFDECT
//==========================================
// ����PC �� �p�����[�^�ύX��ʒm
//------------------------------------------
// int				frame	: �t���[��No
// int				id		: ����PC�敪 (0:�\ 1:�� 2:����)
// int				scode	: �����\�ʏ��No
// EM_DIV_HANTEI	hantei	: �Ώۂ̔���@�\ (DIV_HANTEI_NON��������S�@�\)
//==========================================
void MainInstance::Send_HtDefect_Param(int frame, int id, int scode, EM_DIV_HANTEI hantei)
{
	COMMON_QUE que;
	que.nEventNo = FACT_HT_DEFECT_01;
	que.nLineNo = getlineid();
	que.fl.data[0] = frame;
	//que.fl.data[0] = 0;			// 0:����
	que.fl.data[1] = id;
	que.fl.data[2] = scode;
	que.fl.data[3] = hantei;

	// ���M
	int iRet = send_mail(HT_DEFECT, "*", &que);
	if (0 != iRet) syslog(SYSNO_MAIL_SNED_ERR, "[�p�����[�^�ύX�v�� err_code=%d]", iRet);
}

//==========================================
// ����PC �� �����J�n��ʒm
//==========================================
void MainInstance::Send_HtDefect_Start()
{
	EM_DIV_UNTEN  emUnten = gcls_Status.GetUnten();

	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_HT_DEFECT_02;
	que.nLineNo = getlineid();
	que.fl.data[0] = emUnten;
	que.fl.data[1] = gcls_pCoil->GetInitScode(0);
	que.fl.data[2] = gcls_pCoil->GetInitScode(1);

	int nDistLen = gcls_Plg.GetDistLen();
	int nDistPos = gcls_Plg.GetDistPos();
	int nDistNotPos = gcls_Plg.GetDistNotPos();
	que.fl.data[3 + nDistPos] = nDistLen;			// ��ʂ���ɐi��ł���
	que.fl.data[3 + nDistNotPos] = 0;


	//// �Ƃ肠���� �o�b�N�A�b�v��0���Z�b�g
	for (int ii = 0; ii<NUM_ALL_HANTEI; ii++) {
		que.fl.data[5 + 2 * ii + 0] = 0;
		que.fl.data[5 + 2 * ii + 1] = DIV_HANTEI_BK;
	}

	//// PCID���� �J�����y�A���Z�b�g
	LogicParaRunFunc::ClearSelectCamset();
	if (DIV_UNTEN_CAMERA != emUnten) {

		for (int ii = 0; ii<NUM_MEN; ii++) {
			// �P��
			for (int jj = 0; jj<NUM_HANTEI; jj++) {
				int pcid = gcls_Status.GetHtID(ii, jj);

				if (pcid > 0 && pcid <= NUM_ALL_HANTEI) {

					que.fl.data[5 + 2 * (pcid - 1) + 0] = ii*NUM_HANTEI + jj + 1;		// �J�����y�A�h�c
					que.fl.data[5 + 2 * (pcid - 1) + 1] = DIV_HANTEI_HT;				// �P���@�\
					LogicParaRunFunc::SetSelectCamset(pcid - 1, ii * NUM_HANTEI + jj + 1);
				}
			}
#ifdef ENABLE_CYCLE
			// ����
			for (int jj = 0; jj<NUM_HANTEI_CYCLE; jj++) {
				int pcid = gcls_Status.GetCyID(ii, jj);

				if (pcid > 0 && pcid <= NUM_ALL_HANTEI) {
					que.fl.data[5 + 2 * (pcid - 1) + 0] = ii*NUM_HANTEI_CYCLE + jj + 1;	// �J�����y�A�h�c
					que.fl.data[5 + 2 * (pcid - 1) + 1] = DIV_HANTEI_CY;				// �����@�\
				}
			}
#endif

		}
	}
	else
	{
		int nGroupNo = -1;
		for (int ii = 0; ii < NUM_MEN; ii++)
		{
			// �P��
			for (int jj = 0; jj < NUM_HANTEI; jj++)
			{
				int pcid = gcls_Status.GetHtID(ii, jj);
				if (pcid > 0 && pcid <= NUM_ALL_HANTEI)
				{
					int nWk = ToLinkswUtil::GetHanteiGroup(ii, jj, 1);
					// �e�����N�X�C�b�`�̐擪����PC�̂݃Z�b�g
					if (nWk != -1 && nGroupNo != nWk)
					{
						que.fl.data[5 + 2 * (pcid - 1) + 0] = ii * NUM_HANTEI + jj + 1;
						nGroupNo = nWk;
					}
					que.fl.data[5 + 2 * (pcid - 1) + 1] = DIV_HANTEI_HT;
					LogicParaRunFunc::SetSelectCamset(pcid - 1, ii * NUM_HANTEI + jj + 1);
				}
			}
		}
	}
	// ���[�h�Ɋւ�炸������
	for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
	{
		int pcid = gcls_Status.GetHtParaID(ii);

		if (pcid > 0 && pcid <= NUM_ALL_HANTEI)
		{
			que.fl.data[5 + 2 * (pcid - 1) + 0] = 0;								// �J�����y�A�h�c�i�N���A�j
			que.fl.data[5 + 2 * (pcid - 1) + 1] = DIV_HANTEI_PR;					// �p������
		}
	}


	//// ���M
	int iRet = send_mail(HT_DEFECT, "*", &que);
	if (0 != iRet) syslog(SYSNO_MAIL_SNED_ERR, "[�����J�n�v�� err_code=%d]", iRet);
}

//==========================================
// �S����PC �� ������~��ʒm
//==========================================
void MainInstance::Send_HtDefect_Stop()
{
	COMMON_QUE que;
	que.nEventNo = FACT_HT_DEFECT_03;
	que.nLineNo = getlineid();

	// �S���Ώ�
	// ���肪�~�܂�Ƃ��́A�����A�p���������Ƃ߂�
	for (int ii = 0; ii<NUM_ALL_HANTEI; ii++) {
		que.fl.data[ii] = 0;
	}

	send_mail(HT_DEFECT, "*", &que);
}

//==========================================
// �Y���ʂ̔���PC�A����PC �� ������~��ʒm
//------------------------------------------
// int	men	: �\��
//==========================================
void MainInstance::Send_HtDefect_Stop(int men)
{
	COMMON_QUE que;
	que.nEventNo = FACT_HT_DEFECT_03;
	que.nLineNo = getlineid();

	//// �܂��P���r�@�\,�����@�\�����͑ΏۊO�Ƃ��Ă���
	for (int ii = 0; ii<NUM_ALL_HANTEI; ii++) {
		que.fl.data[ii] = -1;
	}
	//// ����@�\
	for (int jj = 0; jj<NUM_HANTEI; jj++) {
		int pcid = gcls_Status.GetHtID(men, jj);

		if (pcid > 0 && pcid <= NUM_ALL_HANTEI) {
			que.fl.data[pcid - 1] = 0;		// ��~�v������Ă����
		}
	}
	//// ���M
	send_mail(HT_DEFECT, "*", &que);
}

//==========================================
// ����PC �� JPEG�摜���k���ݒ著�M
//------------------------------------------
// int	nJpegQuality	: 0:BMP�摜�`��, 1�`100:JPEG���k�摜�`��
//==========================================
void MainInstance::Send_HtDefect_JpegQuality(int nJpegQuality)
{
	COMMON_QUE que;
	que.nEventNo = FACT_HT_DEFECT_04;
	que.nLineNo = getlineid();
	que.fl.data[0] = nJpegQuality;
	send_mail(HT_DEFECT, "*", &que);
}


//==========================================
// ����PC �� ��ʏ�񑗐M
//------------------------------------------
// TYPE_EPC_COIL* pData �R�C�����
//==========================================
void MainInstance::Send_HtDefect_Pcinfo(TYPE_EPC_COIL* pData)
{
	COMMON_QUE que;
	que.nEventNo = FACT_HT_DEFECT_05;
	que.nLineNo = getlineid();

	int nCnt = GET_JUST_INDEX(sizeof(TYPE_EPC_COIL), DIVCOIL_ONE_SIZE_SEND) + 1;
	for (int ii = 0; ii < nCnt; ii++) {
		int nSize = sizeof(TYPE_EPC_COIL) - (DIVCOIL_ONE_SIZE_SEND*ii);
		if (nSize>DIVCOIL_ONE_SIZE_SEND) nSize = DIVCOIL_ONE_SIZE_SEND;

		//que.fl.data[0] = 1;				// �x�����s �L��
		que.fl.data[0] = 0;				// 0�Œ�
		que.fl.data[1] = nSize;
		que.fl.data[2] = ii + 1;
		que.fl.data[3] = nCnt;
		for (int jj = 0; jj<NUM_MEN; jj++) que.fl.data[4 + jj] = MAX_SCODE;
		memcpy(&que.fl.data[4 + NUM_MEN], &((BYTE*)pData)[ii*DIVCOIL_ONE_SIZE_SEND], nSize);
		send_mail(HT_DEFECT, "*", &que);
	}
}

//==========================================
// ����PC �� ���s�s�\����	
//==========================================
void MainInstance::Send_HtDefect_PcCanOk()
{
	COMMON_QUE que;
	que.nEventNo = FACT_HT_DEFECT_07;
	que.nLineNo = getlineid();
	send_mail(HT_DEFECT, "*", &que);
}

//==========================================
// ����PC �� ��Ԗ⍇��
//==========================================
void MainInstance::Send_HtDefect_Status()
{
	COMMON_QUE que;
	que.nEventNo = FACT_HT_DEFECT_08;
	que.nLineNo = getlineid();

	// ���M
	int iRet = send_mail(HT_DEFECT, "*", &que);
	if (0 != iRet) syslog(SYSNO_MAIL_SNED_ERR, "[��Ԗ₢���킹 err_code=%d]", iRet);
}

//==========================================
// ����PC �� �����s�ݒ�̃L�����Z���v��
//------------------------------------------
// int				id			: ����PC�敪 (0:�\ 1:�� 2:����)
// EM_DIV_HANTEI	hantei		: �Ώۂ̔���@�\ (DIV_HANTEI_NON��������S�@�\)
//==========================================
void MainInstance::Send_HtDefect_ClearParam(int id, EM_DIV_HANTEI hantei)
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_HT_DEFECT_19;
	que.nLineNo = getlineid();
	que.fl.data[0] = id;
	que.fl.data[1] = hantei;
	send_mail(HT_DEFECT, "*", &que);
}

//==========================================
// ����PC �� ���������J�E���^�ݒ�v��
//------------------------------------------
// int				id			: ����PC�敪 (0:�\ 1:�� 2:����)
// EM_DIV_HANTEI	hantei		: �Ώۂ̔���@�\ (DIV_HANTEI_NON��������S�@�\)
// NCL_CMD_SET_POSITION* cmd	: ���������J�E���^�ݒ���
//==========================================
void MainInstance::Send_HtDefect_SetPosition(int id, EM_DIV_HANTEI hantei, NCL_CMD_SET_POSITION* cmd)
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_HT_DEFECT_20;
	que.nLineNo = getlineid();
	que.fl.data[0] = id;
	que.fl.data[1] = hantei;
	memcpy(&que.fl.data[2], cmd, sizeof(NCL_CMD_SET_POSITION));
	send_mail(HT_DEFECT, "*", &que);
}

//==========================================
// ����PC �� �p�����[�^���O�Ǎ�
//------------------------------------------
// int				id			: ����PC�敪 (0:�\ 1:�� 2:����)
// EM_DIV_HANTEI	hantei		: �Ώۂ̔���@�\ (DIV_HANTEI_NON��������S�@�\)
// int				scode		: �����\�ʏ��No
//==========================================
void MainInstance::Send_HtDefect_PreLoadParam(int id, EM_DIV_HANTEI hantei, int scode)
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_HT_DEFECT_21;
	que.nLineNo = getlineid();
	que.fl.data[0] = id;
	que.fl.data[1] = hantei;
	que.fl.data[2] = scode;
	send_mail(HT_DEFECT, "*", &que);
}


//// HY_DEFECT
//==========================================
// �\���Ǘ��� �p�����[�^�ύX��ʒm
//------------------------------------------
// int			id		: �e�[�u��ID
//==========================================
void MainInstance::Send_HyDefect_Param(int id)
{
	COMMON_QUE que;
	que.nEventNo = FACT_HY_DEFECT_01;
	que.nLineNo = getlineid();
	que.fl.data[0] = id;
	// ���M
	send_mail(HY_DEFECT, "*", &que);
}

//------------------------------------------
// �\���Ǘ��ɕ\����񏉊����ʒm
//------------------------------------------
void MainInstance::Send_HyDefect_MemDataInit()
{
	COMMON_QUE que;
	que.nEventNo = FACT_HY_DEFECT_02;
	que.nLineNo = getlineid();

	send_mail(HY_DEFECT, "*", &que);
}

//------------------------------------------
// �r�_�}�b�v��ʂɃp�����[�^�ύX��ʒm
// int id �e�[�u��ID
//------------------------------------------
void MainInstance::Send_SoDefMap_Param(int id)
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nLineNo = getlineid();
	que.fl.data[0] = id;

	//// �r�_�}�b�v���
	que.nEventNo = FACT_SO_DEFMAP_01;
	send_mail(SO_DEFMAP, "*", &que);
}

//------------------------------------------
// �\���Ǘ��� �O�H���r���Ǎ��݂�ʒm [��MaeManager, CoilBaseManager]
//------------------------------------------
// char const* cKizukenNo	�Ǘ�No
//------------------------------------------
void MainInstance::Send_HyDefect_MaeRead(char const* cKizukenNo)
{
	COMMON_QUE que;
	que.nEventNo = FACT_HY_DEFECT_03;
	que.nLineNo = getlineid();
	memcpy(que.fc.data, cKizukenNo, SIZE_KIZUKEN_NO);

	// �v���r�W����
	send_mail(HY_DEFECT, "*", &que);
}

////// TO_GETDAT
////==========================================
//// ���̓f�[�^�ҏW�Ǘ��� �p�����[�^�ύX��ʒm
////------------------------------------------
//// int			id		: �e�[�u��ID
////==========================================
//void MainInstance::Send_ToGetdat_Param(int id)
//{
//	COMMON_QUE que;
//	que.nEventNo = FACT_TO_GETDAT_01;
//	que.nLineNo = getlineid();
//	que.fl.data[0] = id;
//	// ���M
//	send_mail(TO_GETDAT, ".", &que);
//}

////// TO_PUTDAT
////==========================================
//// ���ѕҏW�Ǘ��� �p�����[�^�ύX��ʒm
////------------------------------------------
//// int			id		: �e�[�u��ID
////==========================================
//void MainInstance::Send_ToPutdat_Param(int id)
//{
//	COMMON_QUE que;
//	que.nEventNo = FACT_TO_PUTDAT_01;
//	que.nLineNo = getlineid();
//	que.fl.data[0] = id;
//	// ���M
//	send_mail(TO_PUTDAT, ".", &que);
//}

////------------------------------------------
//// ���ѕҏW�^�X�N�� �o�^�����ʒm
//// int okng 0:���� 1:�ُ�
////------------------------------------------
//void MainInstance::Send_ToPutDat_Synclock(int okng)
//{
//	COMMON_QUE que;
//	que.nEventNo = FACT_TO_PUTDAT_03;
//	que.nLineNo = getlineid();
//	que.fl.data[0] = okng;
//
//	// ���M
//	int iRet = send_mail(TO_PUTDAT, ".", &que);
//}
//
////// TO_MAEDAT
////==========================================
//// �O�H�����ѕҏW�Ǘ��� �p�����[�^�ύX��ʒm
////------------------------------------------
//// int			id		: �e�[�u��ID
////==========================================
//void MainInstance::Send_ToMaedat_Param(int id)
//{
//	COMMON_QUE que;
//	que.nEventNo = FACT_TO_MAEDAT_01;
//	que.nLineNo = getlineid();
//	que.fl.data[0] = id;
//	// ���M
//	send_mail(TO_MAEDAT, ".", &que);
//}

//------------------------------------------
// �O�H�����ѕҏW�� �����p�R�C������M���� ��ʒm [��CoilManager]
// char* KizukenNo �r���Ǘ�No
// int rec ��M���R�[�h (0�I���W��)
//------------------------------------------
void MainInstance::Send_ToMaedat_CoilRecv(char* KizukenNo, int rec)
{
	COMMON_QUE que;
	que.nEventNo = FACT_TO_MAEDAT_02;
	que.nLineNo = getlineid();
	que.mix.idata[0] = rec;
	memcpy(que.mix.cdata[0], KizukenNo, SIZE_KIZUKEN_NO);

	// ���M
	int iRet = send_mail(TO_MAEDAT, ".", &que);
}

//// SO_GAMENN
//==========================================
// �I�y���[�^��ʂɌ��݂̏�Ԓʒm
//==========================================
void MainInstance::Send_SoGamenn_MailStatus()
{
	COMMON_QUE que;
	que.nEventNo = FACT_SO_GAMENN_01;
	que.nLineNo = getlineid();

	que.fl.data[0] = gcls_Status.GetUnten();
	que.fl.data[1] = gcls_Status.GetPlg();
	que.fl.data[2] = gcls_Status.GetMenMode() ? 1 : 0;
	que.fl.data[3] = gcls_Status.GetKadou();
	que.fl.data[4] = gcls_Status.Get_ParaKadou(gcls_Status.GetParaSys(0));
	que.fl.data[5] = gcls_Status.Get_ParaKadou(gcls_Status.GetParaSys(1));

	// ���M
	send_mail(SO_GAMENN, "*", &que);
}

//// KS_MASTER
//==========================================
// PC�}�X�^�[�Ǘ��ɍċN���v���ʒm
//==========================================
void MainInstance::Send_KsMaster_RebootPc(char* cPcName)
{
	COMMON_QUE que;
	que.nEventNo = FACT_KS_MASTER_07;
	que.nLineNo = getlineid();

	memcpy(que.fc.data, cPcName, SIZE_NAME_PC);

	// ���M
	send_mail(KS_MASTER, ".", &que);
}