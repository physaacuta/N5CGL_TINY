#include "StdAfx.h"
#include "MainInctance.h"

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
MainInctance::MainInctance(void) :
mcls_pLog(NULL)
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
// ���������� (��x�̂݋N��)
//------------------------------------------
BOOL MainInctance::Init()
{
	//// ���C���C���X�^���X�̂݁A�X���b�h�����͍���Ă��� (�����ɔ�V�O�i���Ƃ���)
	//// �p�����N���X���s (���C������X���b�h�ׁ̈A������ƗD��x����)
	WorkThread::Start(MainThread, THREAD_PRIORITY_ABOVE_NORMAL );

	// �X���b�hID��������܂ł�����Ƌx�e
	while( 0==GetTid() ) {
		LOG(em_MSG), "[MainInctance] �X���b�hID���蓖�Ē��E�E�E");
		Sleep(1);			// �X���b�h�N���܂ł�����Ƒ҂�
	}

	//// ���[�J�[�X���b�h ������
	gcls_pEvt = new EvtlogManager();
	gcls_pEvt->SetLogMgr(mcls_pLog);
	gcls_pEvt->SetMainPostTid(GetTid());


	//// ���[�J�[�X���b�h �N��
	gcls_pEvt->Start();

	return TRUE;
}

//------------------------------------------
// �I������ (��x�̂݋N��)
//------------------------------------------
void MainInctance::Exit()
{
	//// ���[�J�[�X���b�h ��~
	gcls_pEvt->Stop();

	//// ���[�J�[�X���b�h �J��
	delete gcls_pEvt;
}


//------------------------------------------
// �X���b�h
//------------------------------------------
UINT WINAPI MainInctance::MainThread(LPVOID param)
{
	MSG	msg;								// MSG�\����

	////// ���C���C���X�^���X���o��
	MainInctance *p = (MainInctance *)param;						// ���N���X�̃C���X�^���X���擾
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);			// ���b�Z�[�W�쐬
	p->SetTid();													// �X���b�hID�Z�b�g


	////// �����[�N�X���b�h�̏����������܂őҋ@
	pLOG(em_MSG), "[MainInctance] �����[�N�X���b�h�̏����������܂őҋ@���E�E�E [0x%X]", p->GetTid() );
	WaitForSingleObject(p->m_evThStart, INFINITE);


	////// ���b�Z�[�W�L���[�ǂݍ���
	pLOG(em_MSG), "[MainInctance] �X���b�h�J�n [0x%X]", p->GetTid());
	while( int ret = GetMessage(&msg, NULL, 0, 0) ){
		_ASSERT( -1 != ret );
		switch (msg.message) {
/*
//-----------------------------------------------------------------------------------------------
		case E_DEF_PCNAMESET: 					// PC���̕ύX�ʒm
			pLOG(em_MSG), "[MainInctance] PC���̕ύX�ʒm [0x%X]", p->GetTid());
			// PC���̕ύX�ʒm���󂯎�������̃��[���ʒm���������邱�� 
			SetEvent(p->gcls_pEvtManager->m_evChangePcName);
			break;
//-----------------------------------------------------------------------------------------------
		case E_DEF_DATAREAD: 					// ���[�U�[�ɂ�鋭���`�F�b�N�J�n�{�^��
			pLOG(em_MSG), "[MainInctance] ���[�U�[�ɂ�鋭���`�F�b�N�J�n�{�^�� [0x%X]", p->GetTid());
			SetEvent(p->gcls_pEvtManager->m_evhDataRead);
			break;
//-----------------------------------------------------------------------------------------------
*/
		case WM_USER_TH_END: 					// �X���b�h�I���v��
			pLOG(em_MSG), "[MainInctance] �X���b�h�I�� [0x%X]", p->GetTid());
			return 0;

		default :
			// ���肦�Ȃ��I�I
			break;
		}
	}

	// ���肦�Ȃ�
	pLOG(em_ERR), "[MainInctance] ���C�����[�v���甲�����I�I[0x%X]", p->GetTid());
	return 0;
}