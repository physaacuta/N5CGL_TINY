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
	//// �p�����[�^�Ǎ��҂�
	////////////////////////////////////
	// �p�����[�^�Ǘ��N���X
	gcls_pParam = new ParamManager();
	gcls_pParam->SetLogMgr(mcls_pLog);
	while( true ) {
		if( gcls_pParam->ReadParam_All(true) ) break;

		LOG(em_MSG), "[%s] �p�����[�^�Ǎ����s(DB�ڑ��ُ�H�I)�B���g���C���E�E�E", my_sThreadName);
		Sleep(10000);			
	}


	////////////////////////////////////
	//// ���[�J�[�X���b�h ������
	////////////////////////////////////

	gcls_pSend = new KizuOpSend(HY_DEFECT);
	gcls_pSend->SetLogMgr(mcls_pLog);

	gcls_pImgSend = new ImgSendManager();
	gcls_pImgSend->SetLogMgr(mcls_pLog);
	gcls_pImgSend->SetOpSend(gcls_pSend);
	gcls_pImgSend->SetParamMgr(gcls_pParam);

	// �O�H�����Ǘ�
	gcls_pMaeMgr = new TestMaeManager();
	gcls_pMaeMgr->SetLogMgr(mcls_pLog);

	// �e�X�g�Ǘ�
	gcls_pTest = new TestManager();
	gcls_pTest->SetLogMgr(mcls_pLog);
	gcls_pTest->SetParamMgr(gcls_pParam);
	gcls_pTest->SetOpSend(gcls_pSend);
	gcls_pTest->SetImgSend(gcls_pImgSend);
	gcls_pTest->SetMaeMgr(gcls_pMaeMgr);


	////////////////////////////////////
	//// ���[�J�[�X���b�h �N��
	////////////////////////////////////
	gcls_pSend->Start();
	gcls_pTest->Start();
	gcls_pImgSend->Start();
	gcls_pMaeMgr->Start();

	return TRUE;
}

//------------------------------------------
// �I������ (��x�̂݋N��)
//------------------------------------------
void MainInctance::Exit()
{

	//// ���[�J�[�X���b�h ��~
	gcls_pTest->Stop();
	gcls_pSend->Stop();
	gcls_pParam->Stop();
	gcls_pImgSend->Stop();
	gcls_pMaeMgr->Stop();

	//// ���[�J�[�X���b�h �J��
	delete gcls_pTest;
	delete gcls_pSend;
	delete gcls_pParam;
	delete gcls_pImgSend;
	delete gcls_pMaeMgr;

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

