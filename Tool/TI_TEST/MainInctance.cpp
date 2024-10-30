#include "StdAfx.h"
#include "MainInctance.h"

#include "TI_TEST.h"			// ��ʂ̕��i�ɒ��ڃA�N�Z�X���邽��


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
m_nCamPair(0),
m_nMen(0),
m_emKindHt(DIV_HANTEI_HT),
m_bNgCam(false),
m_nAppParam(0),
m_nSysMode(0),
ThreadManager("MI")
{
	// ini�t�@�C������J�����䐔�A�J�����J�n�ʒu���擾
	// �ȍ~�̑S���ۑ��Ƃ̐ڑ����ȂǂɂȂ�l
	g_nNumCam = GetPrivateProfileInt(TI_TEST, "NUM_CAM", NUM_CAM_POS, TASKINI_NAME);
	g_nCamStart = GetPrivateProfileInt(TI_TEST, "CAM_START", 1, TASKINI_NAME);

	for (int ii=0; ii<NUM_CAM_POS; ii++) {
	//for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
		gcls_pSockRpi[ii] = NULL;
		gque_pSockRpi[ii] = NULL;
		gcls_pCull[ii] = NULL;
	}
	
	for (int ii=0; ii<NUM_MEN; ii++) {
		m_nNowScode[ii] = MAX_SCODE;
	}

	// �V�O�i������
	m_evThStart = CreateEvent(NULL, FALSE, FALSE, NULL);				// ���C���X���b�h�����p
	m_evKenStart = CreateEvent(NULL, FALSE, FALSE, NULL);				// ���C���X���b�h�����p
	m_evKenStop = CreateEvent(NULL, FALSE, FALSE, NULL);				// ���C���X���b�h�����p

	m_evTimerExec = CreateWaitableTimer(NULL, FALSE, NULL);				// ���������J�n�^�C�}�[�C�x���g
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
MainInctance::~MainInctance(void)
{
	// �V�O�i���J��
	CloseHandle(m_evThStart);
	CloseHandle(m_evKenStart);
	CloseHandle(m_evKenStop);

	// �������~
	CancelWaitableTimer(m_evTimerExec);
	SAFE_DELETE_HANDLE(m_evTimerExec);
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
	//// ���C���C���X�^���X�̂݁A�X���b�h�����͍���Ă��� (�����ɔ�V�O�i���Ƃ���)
	//// �p�����N���X���s (���C������X���b�h�ׁ̈A������ƗD��x����)
	ThreadManager::Start(THREAD_PRIORITY_ABOVE_NORMAL );

	// �X���b�hID��������܂ł�����Ƌx�e
	while( 0==GetTid() ) {
		LOG(em_MSG), "[%s] �X���b�hID���蓖�Ē��E�E�E", my_sThreadName);
		Sleep(1);			// �X���b�h�N���܂ł�����Ƒ҂�
	}


	////////////////////////////////////
	//// �p�����[�^�Ǎ��҂�

	// �p�����[�^�N���X
	gcls_PrmMgr.SetLogMgr(mcls_pLog); 
	while( true ) {
		if( gcls_PrmMgr.ReadParam_All() ) break;
		LOG(em_MSG), "[%s] �p�����[�^�N���X �p�����[�^�Ǎ����s(DB�ڑ��ُ�H�I)�B���g���C���E�E�E", my_sThreadName);
		Sleep(5000);
	}

	////////////////////////////////////
	//// �S���摜�֌W
#ifdef FRAME_RAW_SEND
	for (int ii=0; ii<NUM_CAM_POS; ii++) {				// �����c�[���̂��߁A�������S�J����
	//for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
		// �o�b�t�@�m��
		// �T�C�Y���傫������H�H�@MAX_BUF_QUE_RPISOCK�����ɂQ�ɕύX
		//gque_pSockRpi[ii] = new ThreadLoopQueue<HT_RPI_FDATA>(MAX_BUF_QUE_RPISOCK);
		//int n[1] = { SIZE_FRAME_IMG_X * SIZE_FRAME_IMG_Y * MAX_IMAGE_CNT };
		//QueueAlloc(ii, MAX_BUF_QUE_RPISOCK, n);
		int nMaxBufQueRpisock = 4;
		gque_pSockRpi[ii] = new ThreadLoopQueue<HT_RPI_FDATA>(nMaxBufQueRpisock);
		int n[1] = { (SIZE_FRAME_IMG_X * SIZE_FRAME_IMG_Y * MAX_IMAGE_CNT) / 4 };
		//QueueAlloc(ii, MAX_BUF_QUE_RPISOCK, n);
		QueueAlloc(ii, nMaxBufQueRpisock, n);
		// ����M�X���b�h�N��
		gcls_pSockRpi[ii] = new SockOneToAnySend();
		gcls_pSockRpi[ii]->SetLogMgr(mcls_pLog);
		gcls_pSockRpi[ii]->SetTLQue((ThreadLoopQueue<SOCK_F_HEAD2>*) gque_pSockRpi[ii]);
		////gcls_pSockRpi[ii]->StartLock(THREAD_PRIORITY_TIME_CRITICAL);

		// �Ԉ����N���X
		gcls_pCull[ii] = new CullManager();
		gcls_pCull[ii]->SetLogMgr(mcls_pLog);
		gcls_pCull[ii]->SetLogMgrCull(mcls_pLogCull);
		gcls_pCull[ii]->SetParamMgr(&gcls_PrmMgr);
		gcls_pCull[ii]->SetRpiToSend(gcls_pSockRpi[ii]);
		gcls_pCull[ii]->SetQueSockRpi(gque_pSockRpi[ii]);
		////gcls_pCull[ii]->StartLock();

	}
#endif





	////////////////////////////////////
	//// ���[�J�[�X���b�h ������
	// �e�X�g�Ǘ�
	gcls_pTest = new TestManager(g_nNumCam, g_nCamStart);
	gcls_pTest->SetLogMgr(mcls_pLog);
	gcls_pTest->SetParamMgr(&gcls_PrmMgr);
	gcls_pTest->SetCullMgr(gcls_pCull);
	gcls_pTest->Alloc();
	// �����p�����ݒ�
	gcls_pTest->SetChangeFrm(2000);


	////////////////////////////////////
	//// ���[�J�[�X���b�h �N��
	gcls_PrmMgr.StartLock();
	gcls_pTest->Start();

	////////////////////////////////////
	//// �������

	int pcid = getpcid();
	return TRUE;
}

//------------------------------------------
// �I������ (��x�̂݋N��)
//------------------------------------------
void MainInctance::Exit()
{
	//// ���[�J�[�X���b�h ��~
	gcls_PrmMgr.Stop();
	gcls_pTest->Free();
	gcls_pTest->Stop();

	//// ���[�J�[�X���b�h �J��
	delete gcls_pTest;

	////////////////////////////////////
	// �S���摜�֌W
#ifdef FRAME_RAW_SEND
	QueueFree();

	for (int ii=0; ii<NUM_CAM_POS; ii++) {
	//for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
		if( gcls_pSockRpi[ii]->IsThreadEvOn() ) gcls_pSockRpi[ii]->Stop();
		if( gcls_pCull[ii]->IsThreadEvOn()) gcls_pCull[ii]->Stop();

		delete gque_pSockRpi[ii];
		delete gcls_pSockRpi[ii];
		delete gcls_pCull[ii];
	}
#endif
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int MainInctance::ThreadFirst()
{
	////// �����[�N�X���b�h�̏����������܂őҋ@ (MainInctance�̂�)
	LOG(em_MSG), "[%s] �����[�N�X���b�h�̏����������܂őҋ@���E�E�E [0x%X]", my_sThreadName, GetTid() );
	WaitForSingleObject(m_evThStart, INFINITE);


	HANDLE h[NUM_CAM_POS];
	//// �C�x���g�ݒ�
	AddEventNum(1, &mque_DeliMail.g_evSem);
	AddEventNum(1, &m_evKenStart);
	AddEventNum(1, &m_evKenStop);
	AddEventNum(1, &m_evTimerExec);

	// �S��
#ifdef FRAME_RAW_SEND
	for (int ii=0; ii<NUM_CAM_POS; ii++) h[ii] = gcls_pSockRpi[ii]->GetEvConnect();
	AddEventNum(NUM_CAM_POS, h);		// �S���ڑ�
	//for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) h[ii] = gcls_pSockRpi[ii]->GetEvConnect();	
	//AddEventNum(FRAME_HT_SEND_PORT, h);		// �S���ڑ�
#endif

	// �^�C�}�[�Z�b�g
	LOG(em_MSG), "[%s] ���������J�n�^�C�}�[�Z�b�g [%d�b��]", my_sThreadName, (PARAM_TIMEOUT_AUTO_START/1000) );
	__int64 ts = PARAM_TIMEOUT_AUTO_START * -10000;		// 1�~���b��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	int nCycleTime = 0;							// ������Ԋu
	SetWaitableTimer(m_evTimerExec, (LARGE_INTEGER *)&ts, nCycleTime, NULL, NULL, FALSE);

	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int MainInctance::ThreadLast()
{
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void MainInctance::ThreadEvent(int nEventNo)
{
	DELIVERY_MAIL* pMail;
	//int retc;

	////// �V�O�i���̏���
	enum {	EV_MAIL = 0,					// ���[���X���b�g �ʒm (���[���͈�ԍŌ�ɂ��悤����)

			EV_KEN_START,					// �����J�n
		    EV_KEN_STOP,					// ������~
			EV_TIMER_START,					// ���������J�n

#ifdef FRAME_RAW_SEND
			EV_RPI_SOCK_CONECT,				// �ΑS�� �ڑ� (FRAME_HT_SEND_PORT��)
#endif
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_MAIL: 						// ���[���X���b�g �ʒm
	{
		pMail = mque_DeliMail.GetFromHead();
		switch(pMail->emNo) {

//-----------------------------------------------------------
		case E_DEF_ML_PARAM :									// �p�����[�^�ύX�v�� 
			break;	

//-----------------------------------------------------------
		}
		// �J��
		delete pMail;
		break;
	}

//-----------------------------------------------------------
	case EV_KEN_START: 					// �����J�n
	//---------------------------------------
	//// �S�������Ɛڑ�
		if (true) {
#ifdef FRAME_RAW_SEND
			if (1 == m_nSysMode) {
				LOG(em_INF), "[%s] �����J�n���ɊJ�n�v���̂��߁A�j��", my_sThreadName);
				break;
			}

			CancelWaitableTimer(m_evTimerExec);			// �����J�n�v���̃^�C�}�[��~

			CString				sWk;

			for (int ii = 0; ii < NUM_CAM_POS; ii++) {
				//for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
				gcls_pSockRpi[ii]->StartLock(THREAD_PRIORITY_TIME_CRITICAL);
				gcls_pCull[ii]->StartLock();
			}
			Sleep(100);

			for (int ii = 0; ii < NUM_CAM_POS; ii++) {
				//for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {

				if (ii == (g_nCamStart - 1)) {
					for (int jj = g_nCamStart-1; jj < ((g_nCamStart-1)+g_nNumCam); jj++) {

						int nId = (jj) % NUM_CAM_POS;
						sWk.Format("%s_RPI_TO_%d_%d", HT_DEFECT, m_nMen, nId);
						gcls_pSockRpi[jj]->SetSession(sWk);
						gcls_pSockRpi[jj]->SetRetryMode(true);
						gcls_pSockRpi[jj]->SetConnect();
					}
					break;
				}
			}
			m_nSysMode = 1;	// �J�n
#endif
		}

		break;

//-----------------------------------------------------------
	case EV_KEN_STOP: 					// ������~
		//---------------------------------------
		// �S������ �ؒf
		if (true) {

#ifdef FRAME_RAW_SEND
			if (0 == m_nSysMode) {
				LOG(em_INF), "[%s] ������~���ɒ�~�v���̂��߁A�j��", my_sThreadName);
				break;
			}

			for (int ii = 0; ii < NUM_CAM_POS; ii++) {
				//for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {

				if (gcls_pSockRpi[ii]->IsThreadEvOn()) {
					gcls_pSockRpi[ii]->SetRetryMode(false);		// ���g���C�͂������Ȃ�
					gcls_pSockRpi[ii]->SetDisConnect();
				}
			}
			Sleep(1000);		// �񓯊��őS�������ؒf���Ă��邽�߁A�ؒf�����O�ɃX���b�h��~����Ă��܂��ƈُ�I���ƂȂ�B������ƒ��߂ɑ҂�
			//Sleep(300);
			for (int ii = 0; ii < NUM_CAM_POS; ii++) {
				//for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {		
				if (gcls_pSockRpi[ii]->IsThreadEvOn()) gcls_pSockRpi[ii]->Stop();
				if (gcls_pCull[ii]->IsThreadEvOn()) gcls_pCull[ii]->Stop();
			}
			m_nSysMode = 0;	// ��~
#endif
		}
		break;

//-----------------------------------------------------------
	case EV_TIMER_START: 				// ���������J�n
		if (true) {
			// �J�n�C�x���g
			SetEvKenStart();
		}
		break;

//-----------------------------------------------------------
#ifdef FRAME_RAW_SEND
	case EV_RPI_SOCK_CONECT + 0:									// �ΑS�� �ڑ�
		LOG(em_MSG), "[%s] �ΑS��1  �ڑ�����", my_sThreadName);
		break;
	case EV_RPI_SOCK_CONECT + 1:									// �ΑS�� �ڑ�
		LOG(em_MSG), "[%s] �ΑS��2  �ڑ�����", my_sThreadName);
		break;
	case EV_RPI_SOCK_CONECT + 2:									// �ΑS�� �ڑ�
		LOG(em_MSG), "[%s] �ΑS��3  �ڑ�����", my_sThreadName);
		break;
	case EV_RPI_SOCK_CONECT + 3:									// �ΑS�� �ڑ�
		LOG(em_MSG), "[%s] �ΑS��4  �ڑ�����", my_sThreadName);
		break;
	case EV_RPI_SOCK_CONECT + 4:									// �ΑS�� �ڑ�
		LOG(em_MSG), "[%s] �ΑS��5  �ڑ�����", my_sThreadName);
		break;
	case EV_RPI_SOCK_CONECT + 5:									// �ΑS�� �ڑ�
		LOG(em_MSG), "[%s] �ΑS��6  �ڑ�����", my_sThreadName);
		break;
	case EV_RPI_SOCK_CONECT + 6:									// �ΑS�� �ڑ�
		LOG(em_MSG), "[%s] �ΑS��7  �ڑ�����", my_sThreadName);
		break;
	case EV_RPI_SOCK_CONECT + 7:									// �ΑS�� �ڑ�
		LOG(em_MSG), "[%s] �ΑS��8  �ڑ�����", my_sThreadName);
		break;
	case EV_RPI_SOCK_CONECT + 8:									// �ΑS�� �ڑ�
		LOG(em_MSG), "[%s] �ΑS��9  �ڑ�����", my_sThreadName);
		break;
	case EV_RPI_SOCK_CONECT + 9:									// �ΑS�� �ڑ�
		LOG(em_MSG), "[%s] �ΑS��10 �ڑ�����", my_sThreadName);
		break;
#endif

//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma region // �S��

//------------------------------------------
// �S���������M�p�̉�]���i�L���[�m��
// int n		�L���[��
// int const* p	��n���
//					[0] ��n�摜�ő�T�C�Y
//------------------------------------------
void MainInctance::QueueAlloc(int idx, int n, int const* p)
{	
	HT_RPI_FDATA *pD = NULL;

	//// ��̊���쐬
	for (int ii = 0; ii < n; ii++) {
		pD = new HT_RPI_FDATA;
		pD->Alloc(p);
		gque_pSockRpi[idx]->AddFree(pD);			// ���ǉ�
	}
	
	LOG(em_MSG),"[%s] �S������<%d> ���M�L���[%d���m�� [%dMB]", my_sThreadName, idx, n, (__int64)p[0] * n / 1024 / 1024);
	return ;
}

//------------------------------------------
// �S���������M�p�̉�]���i�L���[���
//------------------------------------------
void MainInctance::QueueFree()
{	
	HT_RPI_FDATA *pD = NULL;
	int nPoolNum = 0;
	int nFreeNum = 0;

	for (int ii=0; ii<NUM_CAM_POS; ii++) {
	//for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
		// ��L�L���[
		while(true) {
			pD = gque_pSockRpi[ii]->GetPool();
			if( NULL == pD) break;
			pD->Free();
			delete pD;
			pD = NULL;
			nPoolNum ++;
		}
		// �󂫃L���[
		while(true) {
			pD = gque_pSockRpi[ii]->GetFree();
			if( NULL == pD) break;
			pD->Free();
			delete pD;
			pD = NULL;
			nFreeNum ++;
		}
	}
	LOG(em_MSG),"[%s] �S������ ���M�L���[��� [PoolQue=%d; FreeQue=%d]", my_sThreadName, nPoolNum, nFreeNum );
}

#pragma endregion


//------------------------------------------
// �S�����������p�c�[���A���N��
//------------------------------------------
void MainInctance::Send_TiTest_Rendo(int nCycle)
{
	// �����p �����J�n/��~
	COMMON_QUE que;
	que.nEventNo = FACT_TI_TEST_01;
	que.nLineNo = LINE_ID;
	que.fl.data[0] = nCycle;									// ����

	for (int ii = 0; ii < NUM_CAM_POS; ii++)
	{
		CString sTaskName;
		sTaskName.Format("%s%02d", TI_TEST, (ii + 1));
		if (0 == strcmp(sTaskName, getmytaskname())) continue;
		send_mail(sTaskName, "*", &que);

		LOG(em_MSG), "[%s] �S������ �A���N�� tn:%s Fact:%d %d", my_sThreadName, sTaskName, que.nEventNo, que.fl.data[0] );
	}

	return;
}

//------------------------------------------
// �S�����������p�c�[���A���N��
//------------------------------------------
void MainInctance::Send_TiTest_FrameNo(int nFrameNo, int nCycleTime)
{
	// �����p �����J�n/��~
	COMMON_QUE que;
	que.nEventNo = FACT_TI_TEST_02;
	que.nLineNo = LINE_ID;
	que.fl.data[0] = nFrameNo;									// �t���[��No
	que.fl.data[1] = nCycleTime;								// ��������[ms]

	for (int ii = 0; ii < NUM_CAM_POS; ii++)
	{
		CString sTaskName;
		sTaskName.Format("%s%02d", TI_TEST, (ii + 1));
		if (0 == strcmp(sTaskName, getmytaskname())) continue;
		send_mail(sTaskName, "*", &que);

		LOG(em_MSG), "[%s] �S������ �A���N�� tn:%s Fact:%d fl[%d][%d]", my_sThreadName, sTaskName, que.nEventNo, que.fl.data[0], que.fl.data[1] );
	}

	return;
}


