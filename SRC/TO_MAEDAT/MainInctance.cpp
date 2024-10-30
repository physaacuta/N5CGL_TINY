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
	m_evThStart  = CreateEvent( NULL, FALSE, FALSE, NULL );				// ���C���X���b�h�����p
}
//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
MainInctance::~MainInctance(void)
{
	// �V�O�i���J��
	CloseHandle( m_evThStart );
}

//------------------------------------------
// �X���b�h�L���[�Ƀ��[���f�[�^�o�^
// ENUM_MAEDATOPERATION emNo ���b�Z�[�WNo (�C�x���gNo���̂܂܂ɂ��悤���Ǝv�������AFACT??_01�Ƃ����ƕ�����ɂ��������߂�)
// COMMON_QUE* que ���[���X���b�g�\���̂��̂���
//------------------------------------------
void MainInctance::SetDeliveryMail( ENUM_MAEDATOPERATION emNo, COMMON_QUE* que )
{
	DELIVERY_MAIL* pMail = new DELIVERY_MAIL;
	pMail->emNo	= emNo;
	memcpy( &pMail->que, que, sizeof(COMMON_QUE) );

	// �X���b�h�L���[�ɓo�^
	if( ! mque_DeliMail.AddToTailFreeCheck( pMail ) ) {
		delete pMail;
		LOG(em_ERR), "[%s] mque_DeliMail �L���[�t��!!", my_sThreadName );
	}
}

//------------------------------------------
// ���������� (��x�̂݋N��)
//------------------------------------------
BOOL MainInctance::Init()
{
	//// ���C���C���X�^���X�̂݁A�X���b�h�����͍���Ă��� (�����ɔ�V�O�i���Ƃ���)
	//// �p�����N���X���s (���C������X���b�h�ׁ̈A������ƗD��x����)
	ThreadManager::Start( THREAD_PRIORITY_ABOVE_NORMAL );

	// �X���b�hID��������܂ł�����Ƌx�e
	while( 0==GetTid() ) {
		LOG(em_MSG), "[%s] �X���b�hID���蓖�Ē��E�E�E", my_sThreadName );
		Sleep(1);			// �X���b�h�N���܂ł�����Ƒ҂�
	}


	////////////////////////////////////
	//// ���[�J�[�X���b�h ������
	////////////////////////////////////
	// FF���M�Ǘ��N���X
	gcls_FFSend.SetLogMgr( mcls_pLog );

	// �p�����[�^�Ǘ��N���X
	gcls_Param.SetLogMgr( mcls_pLog );

	// �O�H���ҏW�N���X
	gcls_pPreDefect = new PreDefectManager();
	gcls_pPreDefect->SetLogMgr( mcls_pLog );

	// �O�H�����я��DB�o�^�Ǘ��N���X
	gcls_pPreDB = new PreDBManager();
	gcls_pPreDB->SetLogMgr( mcls_pLog );
    // 2014.07.25 �쌴  ���ԘA�g    >>>>>>>>
    gcls_pPreDB->SetParamManager( &gcls_Param );
    // 2014.07.25 �쌴  ���ԘA�g    <<<<<<<<

    // �O�H�����C���Ǘ��N���X
	gcls_PreProcManager = new PreProcManager();
	gcls_PreProcManager->SetLogMgr( mcls_pLog );
	gcls_PreProcManager->SetMainPostTid( GetTid() );
	gcls_PreProcManager->SetFFSendManager( &gcls_FFSend );
	gcls_PreProcManager->SetParamManager( &gcls_Param );
	gcls_PreProcManager->SetPreDefectManager( gcls_pPreDefect );
	gcls_PreProcManager->SetPreDBManager( gcls_pPreDB );
	gcls_PreProcManager->Alloc();

	////////////////////////////////////
	//// ���[�J�[�X���b�h �N��
	////////////////////////////////////
	gcls_PreProcManager->Start();
	gcls_FFSend.Start();
	gcls_Param.Start();
	gcls_pPreDefect->Start();
	gcls_pPreDB->Start();

	return TRUE;
}

//------------------------------------------
// �I������ (��x�̂݋N��)
//------------------------------------------
void MainInctance::Exit()
{

	//// ���[�J�[�X���b�h ��~
	gcls_PreProcManager->Stop();
	gcls_FFSend.Stop();
	gcls_Param .Stop();
	gcls_pPreDefect->Stop();
	gcls_pPreDB->Stop();

	//// ���[�J�[�X���b�h �J��
	delete gcls_PreProcManager;
	delete gcls_pPreDefect;
	delete gcls_pPreDB;

}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int MainInctance::ThreadFirst()
{
	////// �����[�N�X���b�h�̏����������܂őҋ@ (MainInctance�̂�)
	LOG(em_MSG), "[%s] �����[�N�X���b�h�̏����������܂őҋ@���E�E�E [0x%X]", my_sThreadName, GetTid() );
	WaitForSingleObject( m_evThStart, INFINITE );

	//// �C�x���g�ݒ�
	HANDLE hArray[] = { mque_DeliMail.g_evSem
						};
	SetEventNum( sizeof(hArray)/sizeof(HANDLE), hArray );

	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void MainInctance::ThreadEvent( int nEventNo )
{
	DELIVERY_MAIL* pMail;

	////// �V�O�i���̏���
	enum {	EV_MAIL = 0,				// ���[���X���b�g �ʒm (���[���͈�ԍŌ�ɂ��悤����)
		};

	////// �V�O�i����������
	switch ( nEventNo ) {

//-----------------------------------------------------------------------------------------------
	case EV_MAIL: 						// ���[���X���b�g �ʒm
		pMail = mque_DeliMail.GetFromHead();
		switch( pMail->emNo ) {

//-----------------------------------------------------------
		case E_PD_ML_COIL:		// �R�C������M�ʒm
			{
				COMMON_QUE* pQue = new COMMON_QUE;
				memcpy( pQue, &pMail->que, sizeof(COMMON_QUE) );

				// �e�e�O�H���r���Ǘ���
				gcls_PreProcManager->gque_Deli.AddToTail( pQue ); 
			}
			break;

//-----------------------------------------------------------
		case E_PD_ML_FF:		// FF�O�H���r����M�ʒm
			{
				COMMON_QUE* pQue = new COMMON_QUE;
				memcpy( pQue, &pMail->que, sizeof(COMMON_QUE) );
				
				// �e�e�O�H���r���Ǘ���
				gcls_PreProcManager->gque_Deli.AddToTail( pQue );
			}
			break;

//-----------------------------------------------------------
		case E_PD_END:
			break;


//-----------------------------------------------------------
		}
		// �J��
		SAFE_DELETE(pMail);
		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

