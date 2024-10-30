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
// QUE_SEND_STAT emNo ���b�Z�[�WNo (�C�x���gNo���̂܂܂ɂ��悤���Ǝv�������AFACT??_01�Ƃ����ƕ�����ɂ��������߂�)
// COMMON_QUE* que ���[���X���b�g�\���̂��̂���
//------------------------------------------
void MainInctance::SetDeliveryMail(QUE_SEND_STAT emNo, COMMON_QUE* que)
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
	//// ����M�f�[�^�󂯓n���N���X
	gcls_pData = new SockDataExec();
	gcls_pData->SetLogMgr(mcls_pLog);

	gcls_pSock = new SockManager();
	gcls_pSock->SetLogMgr(mcls_pLog);
//	gcls_pSock->SetMainPostTid(GetTid());
	gcls_pSock->SetSockDataExec(gcls_pData);


	////////////////////////////////////
	//// ���[�J�[�X���b�h �N��
	////////////////////////////////////
	gcls_pSock->Start();


	return TRUE;
}

//------------------------------------------
// �I������ (��x�̂݋N��)
//------------------------------------------
void MainInctance::Exit()
{
	//// ���[�J�[�X���b�h ��~
	gcls_pSock->Stop();

	//// ���[�J�[�X���b�h �J��
	delete gcls_pData;
	delete gcls_pSock;

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
	case EV_MAIL: 					// ���[���X���b�g �ʒm
		pMail = mque_DeliMail.GetFromHead();
		switch(pMail->emNo) {

//-----------------------------------------------------------
		case QUE_SEND_ZISEKI:		// �r���ё��M�v��
			{
				Delivery* deli = new Delivery;
				memset(deli, 0x00, sizeof(Delivery));
				deli->eStat = QUE_SEND_ZISEKI;											// �v�����[�h
				// �f�o�b�N�� ���ѕ\�t�@�C������
				memcpy( deli->cOmtFileName, pMail->que.mix.cdata[0], sizeof(pMail->que.mix.cdata[0]) );
				// �f�o�b�N�� ���ї��t�@�C������
				memcpy( deli->cUraFileName, pMail->que.mix.cdata[1], sizeof(pMail->que.mix.cdata[1]) );
				// �f�o�b�N�� ��ԕ\�t�@�C������
				memcpy(deli->cStOmtFileName, pMail->que.mix.cdata[2], sizeof(pMail->que.mix.cdata[2]));
				// �f�o�b�N�� ��ԗ��t�@�C������
				memcpy(deli->cStUraFileName, pMail->que.mix.cdata[3], sizeof(pMail->que.mix.cdata[3]));
				gcls_pSock->gque_Deli.AddToTail(deli);
			}
			break;

//-----------------------------------------------------------
		case QUE_SEND_MAE:			// �O�H�����їv��
			{
				Delivery* deli = new Delivery;
				memset(deli, 0x00, sizeof(Delivery));

				deli->eStat = QUE_SEND_MAE;												// �v�����[�h
				memcpy( &deli->keyinf, pMail->que.mix.cdata[0], SIZE_COIL_KEY);			// �R�C�����肷�邽�߂̃L�[���
				memcpy( deli->cLine_Code, pMail->que.mix.cdata[1], SIZE_KOUTEI_CODE);	// �H���R�[�h
				memcpy( deli->cMeiNo, pMail->que.mix.cdata[2], SIZE_MEI_NO);			// ����No
				memcpy( deli->cCoilNo, pMail->que.mix.cdata[3], SIZE_COIL_NO);			// �R�C��No
				gcls_pSock->gque_Deli.AddToTail(deli); 
			}
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
