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

	// ������
	for(int ii=0; ii<MAX_THREAD; ii++) gcls_pClrMgr[ii] = NULL;
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
	int nTempThr	= GetPrivateProfileInt(KT_CHILER, "TEMP_THR", 40, TASKINI_NAME);
	int nTimerCycle = GetPrivateProfileInt(KT_CHILER, "TIMER_CYCLE", 180, TASKINI_NAME);

	CString sWk;
	char	cKey[64];
	for(int ii=0; ii<MAX_THREAD; ii++) {
		// �L�[�`�F�b�N
		sWk.Format("NAME_%d", ii+1);
		GetPrivateProfileString(KT_CHILER, sWk, "", cKey, sizeof(cKey), TASKINI_NAME);
		if(0 == strlen(cKey) ) break;

		// �C���X�^���X����
		gcls_pClrMgr[ii] = new ChillerManager(cKey, nTempThr, nTimerCycle);
		gcls_pClrMgr[ii]->SetLogMgr(mcls_pLog);
	}

	////////////////////////////////////
	//// ���[�J�[�X���b�h �N��
	////////////////////////////////////

	for(int ii=0; ii<MAX_THREAD; ii++) {
		if(NULL == gcls_pClrMgr[ii]) break;
		gcls_pClrMgr[ii]->Start();
	}

	return TRUE;
}

//------------------------------------------
// �I������ (��x�̂݋N��)
//------------------------------------------
void MainInctance::Exit()
{
	//// ���[�J�[�X���b�h��~
	for(int ii=0; ii<MAX_THREAD; ii++) {
		if(NULL == gcls_pClrMgr[ii]) continue;
		gcls_pClrMgr[ii]->Stop(100);
	}

	//// ���[�J�[�X���b�h���
	for(int ii=0; ii<MAX_THREAD; ii++) {
		if(NULL == gcls_pClrMgr[ii]) continue;
		SAFE_DELETE( gcls_pClrMgr[ii] );
	}
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
		case 1:
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
