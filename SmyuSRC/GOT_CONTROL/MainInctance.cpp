#include "StdAfx.h"
#include "MainInctance.h"


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
	////////////////////////////////////
	// PLG
	gcls_pPlg = new PlgControlManager();
	gcls_pPlg->SetLogMgr(mcls_pLog);
	// PIO
	gcls_pPio = new PioControlManager();
	gcls_pPio->SetLogMgr(mcls_pLog);
	gcls_pPio->SetPlgCls(gcls_pPlg);

	// �V�~�����[�^�Ǘ�
	for(int ii=0; ii<SMYU_STORY_MAX_FILE; ii++) {
		gcls_pSmyuPlg[ii] = new SmyuPlgManager(ii);
		gcls_pSmyuPlg[ii]->SetLogMgr(mcls_pLog);
		gcls_pSmyuPlg[ii]->SetPlgMgr(gcls_pPlg);
		gcls_pSmyuPlg[ii]->SetPioMgr(gcls_pPio);
	}
	for(int ii=0; ii<SMYU_STORY_MAX_FILE; ii++) {
		gcls_pSmyuTim[ii] = new SmyuTimManager(ii);
		gcls_pSmyuTim[ii]->SetLogMgr(mcls_pLog);
		gcls_pSmyuTim[ii]->SetPlgMgr(gcls_pPlg);
		gcls_pSmyuTim[ii]->SetPioMgr(gcls_pPio);
	}
	gcls_pSmyuEvt = new SmyuEvtManager();
	gcls_pSmyuEvt->SetLogMgr(mcls_pLog);
	gcls_pSmyuEvt->SetPlgMgr(gcls_pPlg);
	gcls_pSmyuEvt->SetPioMgr(gcls_pPio);


	////////////////////////////////////
	//// ���[�J�[�X���b�h �N��
	////////////////////////////////////
	gcls_pPlg->Start(THREAD_PRIORITY_TIME_CRITICAL);
	gcls_pPio->Start();

	//// �V�i���I�ݒ� �� �X���b�h�X�^�[�g
	ExecInit();

	return TRUE;
}

//------------------------------------------
// �I������ (��x�̂݋N��)
//------------------------------------------
void MainInctance::Exit()
{

	//// ���[�J�[�X���b�h ��~
	gcls_pPlg->Stop();
	gcls_pPio->Stop();
	
	for(int ii=0; ii<SMYU_STORY_MAX_FILE; ii++) gcls_pSmyuPlg[ii]->Stop();
	for(int ii=0; ii<SMYU_STORY_MAX_FILE; ii++) gcls_pSmyuTim[ii]->Stop();
	gcls_pSmyuEvt->Stop();

	//// ���[�J�[�X���b�h �J��
	delete gcls_pPlg;
	delete gcls_pPio;

	for(int ii=0; ii<SMYU_STORY_MAX_FILE; ii++) SAFE_DELETE( gcls_pSmyuPlg[ii] );
	for(int ii=0; ii<SMYU_STORY_MAX_FILE; ii++) SAFE_DELETE( gcls_pSmyuTim[ii] );
	SAFE_DELETE( gcls_pSmyuEvt );

}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int MainInctance::ThreadFirst()
{
	////// �����[�N�X���b�h�̏����������܂őҋ@ (MainInctance�̂�)
	LOG(em_MSG), "[%s] �����[�N�X���b�h�̏����������܂őҋ@���E�E�E [0x%X]", my_sThreadName, GetTid() );
	WaitForSingleObject(m_evThStart, INFINITE);

	//// �C�x���g�ݒ�
	HANDLE hArray[] = { mque_DeliMail.g_evSem
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
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
		case E_DEF_ML_SMYU_START:
			ExecStart();
			break;

//-----------------------------------------------------------
		case E_DEF_ML_SMYU_STOP:
			ExecStop();
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
// ����
//------------------------------------------
// �V�i���I�ݒ�
//------------------------------------------
void MainInctance::ExecInit()
{
	char	cWk[2048];
	CString cKey;

		
	//// �����쓮
	for(int ii=0; ii<SMYU_STORY_MAX_FILE; ii++) {
		cKey.Format("CSV_PATH_PLG_%d", ii);

		GetPrivateProfileString(GOT_CONTROL, cKey, "", cWk, sizeof(cWk), TASKINI_NAME);
		if( 0 == strlen(cWk)) break;
		
		gcls_pSmyuPlg[ii]->SetCsvFilePath(cWk);
		gcls_pSmyuPlg[ii]->Start();
	}

	//// ���ԋ쓮
	for(int ii=0; ii<SMYU_STORY_MAX_FILE; ii++) {
		cKey.Format("CSV_PATH_TIM_%d", ii);

		GetPrivateProfileString(GOT_CONTROL, cKey, "", cWk, sizeof(cWk), TASKINI_NAME);
		if( 0 == strlen(cWk)) break;
		
		gcls_pSmyuTim[ii]->SetCsvFilePath(cWk);
		gcls_pSmyuTim[ii]->Start();
	}

	//// �C�x���g�쓮
	cKey.Format("CSV_PATH_EVT_%d", 0);

	GetPrivateProfileString(GOT_CONTROL, cKey, "", cWk, sizeof(cWk), TASKINI_NAME);
	if( 0 != strlen(cWk)) {
		gcls_pSmyuEvt->SetCsvFilePath(cWk);
		gcls_pSmyuEvt->Start();
	}
}

//------------------------------------------
// �V�i���I�J�n
//------------------------------------------
void MainInctance::ExecStart()
{
	//// �����쓮
	for(int ii=0; ii<SMYU_STORY_MAX_FILE; ii++) {
		if( gcls_pSmyuPlg[ii]->IsThreadRun() ) gcls_pSmyuPlg[ii]->SmyuStart();
	}

	//// ���ԋ쓮
	for(int ii=0; ii<SMYU_STORY_MAX_FILE; ii++) {
		if( gcls_pSmyuTim[ii]->IsThreadRun() ) gcls_pSmyuTim[ii]->SmyuStart();
	}

	//// �C�x���g�쓮
	if( gcls_pSmyuEvt->IsThreadRun() ) gcls_pSmyuEvt->SmyuStart();


	//// PLG�����Ǘ�
	gcls_pPlg->SmyuStart();
	for(int ii=0; ii<SMYU_PLG_MAX_CHANNEL; ii++) {
		gcls_pPlg->SetTimer_SpeedCycle(ii, true);
	}
}

//------------------------------------------
// �V�i���I��~
//------------------------------------------
void MainInctance::ExecStop()
{
	//// PLG��~
	for(int ii=0; ii<SMYU_PLG_MAX_CHANNEL; ii++) {
		gcls_pPlg->SetTimer_SpeedCycle(ii, false);
	}

	//// �����쓮
	for(int ii=0; ii<SMYU_STORY_MAX_FILE; ii++) {
		if( gcls_pSmyuPlg[ii]->IsThreadRun() ) gcls_pSmyuPlg[ii]->SmyuStop();
	}

	//// ���ԋ쓮
	for(int ii=0; ii<SMYU_STORY_MAX_FILE; ii++) {
		if( gcls_pSmyuTim[ii]->IsThreadRun() ) gcls_pSmyuTim[ii]->SmyuStop();
	}

	//// �C�x���g�쓮
	if( gcls_pSmyuEvt->IsThreadRun() ) gcls_pSmyuEvt->SmyuStop();
}