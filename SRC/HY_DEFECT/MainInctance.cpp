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
gque_OpRecv(QSIZE_TO_HY),
gque_ImgSave(QSIZE_IMG_SAVE),
gque_ImgSaveLow(QSIZE_IMG_SAVE_LOW),
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
	// ��M�N���X
	gcls_pRecv = new KizuDefaultRecv(HY_DEFECT, &gque_OpRecv);
	gcls_pRecv->SetLogMgr(mcls_pLog);

	// �t�@�C���폜�Ǘ��N���X
	gcls_FDele.SetLogMgr(mcls_pLog);

	// �t�@�C���ۑ��Ǘ��N���X
	for(int ii=0; ii<THREAD_IMG_SAVE; ii++) {
		gcls_pFSave[ii] = new FileSaveManager();
		gcls_pFSave[ii]->SetLogMgr(mcls_pLog);
		gcls_pFSave[ii]->SetQueImgSave(&gque_ImgSave);
		gcls_pFSave[ii]->SetQueImgSaveLow(&gque_ImgSaveLow);
	}

	// �f�[�^�Z�b�g�N���X
	gcls_pData = new DataSetManager();
	gcls_pData->SetLogMgr(mcls_pLog);
	gcls_pData->SetParamMgr(gcls_pParam);
	gcls_pData->SetFDeleMgr(&gcls_FDele);
	gcls_pData->SetQueImgSave(&gque_ImgSave);
	gcls_pData->SetQueImgSaveLow(&gque_ImgSaveLow);
	gcls_pData->SetQueOpRecv(&gque_OpRecv);
// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
	gcls_pData->SetQue_MaeRequest(&gque_DataRequest);
// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	gcls_pData->SetQue_LongPdRequest(&gque_DataRequestLongPd);
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------
	gcls_pData->Allock();

	// �O�H���Ǘ��N���X
	gcls_pMaeMgr = new MaeManager();
	gcls_pMaeMgr->SetLogMgr(mcls_pLog);
	gcls_pMaeMgr->SetParamMgr(gcls_pParam);
	gcls_pMaeMgr->SetDataSetMgr(gcls_pData);
	gcls_pMaeMgr->SetQue_MaeRequest(&gque_MaeRequest);

	////////////////////////////////////
	//// ���[�J�[�X���b�h �N��
	////////////////////////////////////
	gcls_FDele.Start();
	for(int ii=0; ii<THREAD_IMG_SAVE; ii++) gcls_pFSave[ii]->Start();

	gcls_pParam->Start();
	gcls_pRecv->Start();
	gcls_pData->Start();
	gcls_pMaeMgr->Start();

	return TRUE;
}

//------------------------------------------
// �I������ (��x�̂݋N��)
//------------------------------------------
void MainInctance::Exit()
{
	//// ���[�J�[�X���b�h ��~
	gcls_pMaeMgr->Stop();
	gcls_pRecv->Stop();
	gcls_pData->Stop();
	gcls_pParam->Stop();
	gcls_FDele.Stop();

	gcls_pFSave[0]->SetEvClearQue();			// �������L���[�J�� 
	for(int ii=0; ii<THREAD_IMG_SAVE; ii++) gcls_pFSave[ii]->Stop();

	//// ���[�J�[�X���b�h �J��
	delete gcls_pRecv;
	delete gcls_pData;
	delete gcls_pMaeMgr;
	delete gcls_pParam;

	for(int ii=0; ii<THREAD_IMG_SAVE; ii++) delete gcls_pFSave[ii];

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
	enum {	EV_MAIL = 0,					// ���[���X���b�g �ʒm (���[���͈�ԍŌ�ɂ��悤����)
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_MAIL: 							// ���[���X���b�g �ʒm
		pMail = mque_DeliMail.GetFromHead();
		switch(pMail->emNo) {

//-----------------------------------------------------------
		case E_DEF_ML_PARAM:				// �p�����[�^�ύX�ʒm
			if(true) {
				Send_Hy_Param(&pMail->que);

				// �p�����[�^�ύX�R�}���h
				COMMON_QUE* pQue = new COMMON_QUE;
				memcpy( pQue, &pMail->que, sizeof(COMMON_QUE));
				gcls_pParam->gque_Deli.AddToTailFreeCheck(pQue); 
			}
			break;
//-----------------------------------------------------------
		case E_DEF_ML_INIT:					// �C�j�V�����ʒm
			Send_Hy_Init();
			gcls_pData->SetEvClearTbl();	// ���L�������ɑ΂��Ă��������ʒm�𑗂� 
			break;

			//-----------------------------------------------------------
		case E_DEF_ML_MAE_READ:				// �O�H���r���Ǎ��ʒm
			if (true) {
				MaeManager::TYP_MAE_REQUEST* pDeli = new MaeManager::TYP_MAE_REQUEST;
				pDeli->nMode = 0;
				pDeli->pData = NULL;	// �Ǎ��˗����� NULL�Œ�
				memcpy(pDeli->cKanriNo, pMail->que.fc.data, SIZE_KIZUKEN_NO);
				gque_MaeRequest.AddToTailFreeCheck(pDeli);
			}
			break;

// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
//-----------------------------------------------------------
		case E_DEF_ML_MAE_READ_END:			// �O�H���r���Ǎ������ʒm
			if (true) {
				DataSetManager::TYP_DATASET_REQMAE* pDeli = new DataSetManager::TYP_DATASET_REQMAE;
				pDeli->nRec = pMail->que.fl.data[0];
				gque_DataRequest.AddToTailFreeCheck(pDeli);
			}
			break;
// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------

// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
//-----------------------------------------------------------
		case E_DEF_ML_LONGPD_OCCUR:			// �����������ʒm
			if (true) {
				DataSetManager::TYP_DATASET_REQLONGPD* pDeli = new DataSetManager::TYP_DATASET_REQLONGPD;
				memset(pDeli, 0x00, sizeof(DataSetManager::TYP_DATASET_REQLONGPD));
				memcpy(pDeli->cKanriNo, pMail->que.mix.cdata[0], SIZE_KIZUKEN_NO);

				gque_DataRequestLongPd.AddToTailFreeCheck(pDeli);
			}
			break;
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

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
// �O���^�X�N�ʒm


//------------------------------------------
// ���^�X�N�Ƀp�����[�^�ύX�ʒm�𑗐M
// COMMON_QUE* pQue �L���[�󂯓n��
//------------------------------------------
void MainInctance::Send_Hy_Param(COMMON_QUE* pQue)
{
	COMMON_QUE  que;
	memcpy(&que, pQue, sizeof(que));

	// �T�����
	que.nEventNo = FACT_HY_TANSHO_01;
	send_mail(HY_TANSHO, ".", &que);

	// ���̑����
	// �d���׈ꗗ���
	que.nEventNo = FACT_HY_DEFLIST_01;
	send_mail(HY_DEFLIST, ".", &que);
}


//------------------------------------------
// ���^�X�N�ɏ������ʒm�𑗐M
//------------------------------------------
void MainInctance::Send_Hy_Init()
{
	COMMON_QUE  que;
	memset(&que, 0x00, sizeof(que));
	//que.nLineNo = getlineid();

	// �T�����
	que.nEventNo = FACT_HY_TANSHO_02;	
	send_mail(HY_TANSHO,".", &que);

}



