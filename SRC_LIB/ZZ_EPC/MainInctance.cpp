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
	// �f�[�^���M�p�N���X
	gcls_pData = new DataSender();
	gcls_pData->SetLogMgr(mcls_pLog);
	
	// �R�}���h��M�p�N���X
	gcls_pCommand = new CommandRecvre();
	gcls_pCommand->SetLogMgr(mcls_pLog);
	gcls_pCommand->SetMainPostTid(GetTid()); 
	gcls_pCommand->SetDataSender(gcls_pData); 
	// ���xUDP��M�p�N���X
	gcls_pPlg = new PLGReceive();
	gcls_pPlg->SetLogMgr(mcls_pLog);
	gcls_pPlg->SetDataSender(gcls_pData); 
	// �G�b�W�ʒu���UDP��M�p�N���X
	gcls_pEdge = new EdgeReceive();
	gcls_pEdge->SetLogMgr(mcls_pLog);
	gcls_pEdge->SetDataSender(gcls_pData); 

	//// ���[�J�[�X���b�h �N��
	gcls_pCommand->Start();
	gcls_pData->Start();
	gcls_pPlg->Start();
	gcls_pEdge->Start();
	return TRUE;
}

//------------------------------------------
// �I������ (��x�̂݋N��)
//------------------------------------------
void MainInctance::Exit()
{

	//// ���[�J�[�X���b�h ��~
	gcls_pCommand->Stop();
	gcls_pData->Stop();
	gcls_pPlg->Stop();
	gcls_pEdge->Stop();
	//// ���[�J�[�X���b�h �J��
	delete gcls_pCommand;
	delete gcls_pData;
	delete gcls_pPlg;
	delete gcls_pEdge;
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