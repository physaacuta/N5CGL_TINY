#include "StdAfx.h"
#include "MainInstance.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
MainInstance::~MainInstance(void)
{
	// �V�O�i�����
	CloseHandle(m_evThStart);
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
	if (!mque_DeliMail.AddToTailFreeCheck(pMail))
	{
		SAFE_DELETE(pMail);
		LOG(em_ERR), "[%s] mque_DeliMail �L���[�t��!!", my_sThreadName);
	}
}

//------------------------------------------
// ���������� (��x�̂݋N��)
//------------------------------------------
BOOL MainInstance::Init()
{
	//// ���C���C���X�^���X�̂݁A�X���b�h�����͍���Ă��� (�����ɔ�V�O�i���Ƃ���)
	//// �p�����N���X���s (���C������X���b�h�ׁ̈A������ƗD��x����)
	ThreadManager::Start(THREAD_PRIORITY_ABOVE_NORMAL);

	// �X���b�hID��������܂ł�����Ƌx�e
	while (0==GetTid())
	{
		LOG(em_MSG), "[%s] �X���b�hID���蓖�Ē��E�E�E", my_sThreadName);
		Sleep(1);			// �X���b�h�N���܂ł�����Ƒ҂�
	}
	
	//// ���[�J�[�X���b�h ������
	// �v���R���������э쐬�Ǘ��N���X
	gcls_Pro.SetLogMgr(mcls_pLog);
	// �e�e�������э쐬�Ǘ��N���X
	gcls_FF.SetLogMgr(mcls_pLog);
	gcls_FF.SetMakeProMgr(&gcls_Pro); 

	//// ���[�J�[�X���b�h �N��
	gcls_Pro.Start();
	gcls_FF.Start();

	return TRUE;
}

//------------------------------------------
// �I������ (��x�̂݋N��)
//------------------------------------------
void MainInstance::Exit()
{
	//// ���[�J�[�X���b�h ��~
	gcls_Pro.SetEvDefectDbStop();	// �����҂������Ă���ꍇ�́A�����I�ɃL�����Z��
	gcls_Pro.Stop();
	gcls_FF.Stop();
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int MainInstance::ThreadFirst()
{
	////// �����[�N�X���b�h�̏����������܂őҋ@ (MainInstance�̂�)
	LOG(em_MSG), "[%s] �����[�N�X���b�h�̏����������܂őҋ@���E�E�E [0x%X]", my_sThreadName, GetTid() );
	WaitForSingleObject(m_evThStart, INFINITE);
	
	//// �C�x���g�ݒ�
	HANDLE hArray[] =
	{
		mque_DeliMail.g_evSem
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
	enum
	{
		EV_MAIL = 0,					// ���[���X���b�g �ʒm (���[���͈�ԍŌ�ɂ��悤����)
	};

	////// �V�O�i����������
	switch (nEventNo)
	{
//-----------------------------------------------------------------------------------------------
	case EV_MAIL: 						// ���[���X���b�g �ʒm
		pMail = mque_DeliMail.GetFromHead();
		switch (pMail->emNo)
		{
//-----------------------------------------------------------
		case E_DEF_ML_PRO_R_CUT:								// �J�b�g���ю�M�ʒm
			if (true)
			{
				MakeProManager::DELIVERY_KEY* pQue = new MakeProManager::DELIVERY_KEY;
				memset(pQue, 0x00, sizeof(MakeProManager::DELIVERY_KEY));
				strcpy(pQue->cKey, pMail->que.fc.data);
				gcls_Pro.gque_Deli.AddToTail(pQue); 
			}
			break;

//-----------------------------------------------------------
		case E_DEF_ML_PRO_R_SAI:								// �̐����ю�M�ʒm
			if (true)
			{
				MakeFfManager::DELIVERY_KEY* pQue = new MakeFfManager::DELIVERY_KEY;
				memset(pQue, 0x00, sizeof(MakeFfManager::DELIVERY_KEY));
				strcpy(pQue->cKey, pMail->que.fc.data);
				gcls_FF.gque_Deli.AddToTail(pQue); 
			}
			break;

//-----------------------------------------------------------
		case E_DEF_ML_COIL_RESULT_ADD:							// ���я������݊����ʒm
			// �V�O�i���ʒm
			gcls_Pro.SetEvDefectDbAddOk(pMail->que.fl.data[0]);
			break;

//-----------------------------------------------------------
		}
		// ���
		delete pMail;
		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}
