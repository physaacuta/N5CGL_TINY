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
	//// ���[�J�[����
	////////////////////////////////////
	gcls_pCoil = new CoilManager(mcls_pLog);

	// �������T�C�Y�\���i�e�X�g�j
	LOG(em_MSG), "[%s] COIL_SETTING Size[%d]", my_sThreadName,sizeof(COIL_SETTING));
	LOG(em_MSG), "[%s] COIL_BASEDATA Size[%d]", my_sThreadName,sizeof(COIL_BASEDATA));
	LOG(em_MSG), "[%s] TO_COIL_REC Size[%d]", my_sThreadName,sizeof(TO_COIL_TBL::TO_COIL_REC));
	LOG(em_MSG), "[%s] TO_COIL_TBL Size[%d]", my_sThreadName,sizeof(TO_COIL_TBL));


	return TRUE;
}

//------------------------------------------
// �I������ (��x�̂݋N��)
//------------------------------------------
void MainInctance::Exit()
{
	delete gcls_pCoil;
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
		case E_DEF_ML_COIL:				// �R�C������M
			if(true) {
				int  nRec = pMail->que.fl.data[0];							// ���L���������R�[�h�ʒu (0�I���W��)
				gcls_pCoil->Check_CoilInf(nRec);
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


