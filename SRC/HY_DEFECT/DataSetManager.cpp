#include "StdAfx.h"
#include "DataSetManager.h"

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
DataSetManager::DataSetManager(void) :
DataSetBaseManager(),
m_bQueState(false)
{

}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
DataSetManager::~DataSetManager(void)
{
}

//------------------------------------------
// �X���b�h����
//------------------------------------------
void DataSetManager::Allock()
{
	//// ini�t�@�C���ݒ�擾
	GetPrivateProfileString(HY_DEFECT, "IMAGE_PATH", "", m_cPath_Img, SIZE_IMAGE_PATH, TASKINI_NAME);
	_ASSERT(strlen(m_cPath_Img));

	GetPrivateProfileString(HY_DEFECT, "MAEIMG_PATH", "", m_cPath_Mae, SIZE_IMAGE_PATH, TASKINI_NAME);
	_ASSERT(strlen(m_cPath_Mae));

	GetPrivateProfileString(HY_DEFECT, "LSTIMG_PATH", "", m_cPath_Lst, SIZE_IMAGE_PATH, TASKINI_NAME);
	_ASSERT(strlen(m_cPath_Lst));

	m_nImgSaveOff = GetPrivateProfileInt(HY_DEFECT, "IMG_SAVE_OFF", 0, TASKINI_NAME);
	m_nMaeImgSaveOff = GetPrivateProfileInt(HY_DEFECT, "MAEIMG_SAVE_OFF", 0, TASKINI_NAME);
	m_nLstImgSaveOff = GetPrivateProfileInt(HY_DEFECT, "LSTIMG_SAVE_OFF", 0, TASKINI_NAME);

	if (1 == m_nImgSaveOff) {
		LOG(em_MSG), "[%s] �T����ʕ\���p�̉摜�ۑ����� <%d>", my_sThreadName, m_nImgSaveOff);
	}
	if (1 == m_nMaeImgSaveOff) {
		LOG(em_MSG), "[%s] �O�H���r�摜�ۑ����� <%d>", my_sThreadName, m_nMaeImgSaveOff);
	}
	if (1 == m_nLstImgSaveOff) {
		LOG(em_MSG), "[%s] �d�����r�摜�ۑ����� <%d>", my_sThreadName, m_nLstImgSaveOff);
	}

	m_nLstSaveNo[0] = 0;
	m_nLstSaveNo[1] = 0;

	//// �\�ʌ�����ʒu
	//m_nKensaNum[0] = GetPrivateProfileInt(HY_DEFECT, "KENSA_NUM_O", DIV_LEN_POS_KEN_TOP, TASKINI_NAME);
	//if (1 < NUM_MEN) {
	//	// ���ʌ�����ʒu
	//	m_nKensaNum[1] = GetPrivateProfileInt(HY_DEFECT, "KENSA_NUM_U", DIV_LEN_POS_KEN_BOT, TASKINI_NAME);
	//}

	//// �t�H���_����
	this->CreateFolder(m_cPath_Img, TBL_HY_REC_MAX);

	////// �摜���폜�˗�
	CString sPath;
	//sPath = this->GetFolderPath(m_cPath_Img, 0);
	//mcls_pFDele->DeleteStart(sPath);

	FolderFunc  clsF;
	CString sWk;

	for (int ii = 0; ii < TBL_HY_REC_MAX; ii++) {
		sPath = this->GetFolderPath(m_cPath_Img, ii);
		mcls_pFDele->DeleteAll(sPath);
		LOG(em_MSG), "[%s] �r�摜�폜�v�� <%s>", my_sThreadName, sPath);
	}

	//// �t�H���_����
	this->CreateFolder(m_cPath_Mae, TBL_HY_REC_MAX);
	for (int ii = 0; ii < TBL_HY_REC_MAX; ii++) {
		sWk = "";
		sWk = GetFolderPath(m_cPath_Mae, ii);
		sWk += "\\*";
		if (clsF.DeleteFileAll(sWk)) {
			LOG(em_MSG), "[%s] �O�H���摜�폜���� <%s>", my_sThreadName, sWk);
		}
	}

	//// �t�H���_����
	_mkdir(m_cPath_Lst);
	sWk = "";
	sWk = GetFolderPath(m_cPath_Lst, -1);
	sWk += "\\*";
	if (clsF.DeleteFileAll(sWk)) {
		LOG(em_MSG), "[%s] �d���׉摜�폜���� <%s>", my_sThreadName, sWk);
	}

	//// ���L����������o�^
	this->AddMemInf(&m_hStatus,		(void**)&mtbl_pStatus,		sizeof(TBL_HY_STATUS),		TBL_HY_STATUS_NAME, true); 
	this->AddMemInf(&m_hPoint,		(void**)&mtbl_pPoint,		sizeof(TBL_HY_POINT),		TBL_HY_POINT_NAME); 
	this->AddMemInf(&m_hCoilInf,	(void**)&mtbl_pCoilInf,		sizeof(TBL_HY_COILINF),		TBL_HY_COILINF_NAME); 
	this->AddMemInf(&m_hCoilResult, (void**)&mtbl_pCoilResult,	sizeof(TBL_HY_COILRESULT),	TBL_HY_COILRESULT_NAME); 
	this->AddMemInf(&m_hMapRow,		(void**)&mtbl_pMapRow,		sizeof(TBL_HY_MAPROW),		TBL_HY_MAP_ROW_NAME); 
	this->AddMemInf(&m_hDefectInf,	(void**)&mtbl_pDefectInf,	sizeof(TBL_HY_DEFECTINF),	TBL_HY_DEFECTINF_NAME); 
	this->AddMemInf(&m_hAlarm,      (void**)&mtbl_pAlarm,       sizeof(TBL_HY_ALARM),       TBL_HY_ALARM_NAME);
	this->AddMemInf(&m_hMaeInf,		(void**)&mtbl_pMaeInf,		sizeof(TBL_HY_MAE_INF),		TBL_HY_MAE_INF_NAME);
	//this->AddMemInf(&m_hCycleInf,	(void**)&mtbl_pCycleInf,	sizeof(TBL_HY_CYCLE_INF),	TBL_HY_CYCLE_INF_NAME);
	this->AddMemInf(&m_hDefList,	(void**)&mtbl_pDefList,		sizeof(TBL_HY_DEFLIST),		TBL_HY_DEFLIST_NAME, true);
	// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	this->AddMemInf(&ml_hLongPd,	(void**)&mtbl_pLongPd,		sizeof(TBL_HY_LONGPD),		TBL_HY_LONGPD_NAME);
	// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

#ifdef LOCAL
//*/----> �������ŋ��L�����������
	for(int ii=0; ii<m_nMemSetCnt; ii++) {
		CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, m_MemInf[ii].nSize, m_MemInf[ii].cName);

//HANDLE h = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, m_MemInf[ii].nSize, m_MemInf[ii].cName);
//void* pTbl =(LPVOID *) MapViewOfFile(h, FILE_MAP_ALL_ACCESS, 0, 0, 0);
//int a = GetLastError();
//
//_RPT4(_CRT_WARN, "1-%d (%p)(%p) (%d)\n", ii, h, pTbl, a);
	}
//*///<<---
#endif

}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���z�֐�

// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>	
//------------------------------------------
// �V�O�i���҂��s��̒ǉ�
//------------------------------------------
void DataSetManager::ThreadFirst_AddHandle()
{
	AddEventNum(1, &mque_DataRequest->g_evSem);
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	AddEventNum(1, &mque_DataRequestLongPd->g_evSem);
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------
}

//------------------------------------------
// �V�O�i����ǉ������z�̃C�x���g (�ǉ��������̂�0�n�܂�Ƃ���)
//------------------------------------------
void DataSetManager::OnAddSignal(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_MAE_QUE,							// �O�H�����ҏW�v��
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
			EV_LONGPD_QUE,						// �����������ʒm
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------
			EV_LAST
	};

	////// �V�O�i����������
	//======================================================
	// �O�H�����ҏW�v��
	if ( nEventNo == EV_MAE_QUE ) 
	{
		TYP_DATASET_REQMAE* pQue;							// �L���[���
		pQue = mque_DataRequest->GetFromHead();				// �L���[���擾
		
		EditMaeDefectOutLen(pQue);

		delete pQue;										// �L���[���J��
	}
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	//======================================================
	// �����������ʒm
	else if ( nEventNo == EV_LONGPD_QUE ) 
	{
		TYP_DATASET_REQLONGPD* pQue;							// �L���[���
		pQue = mque_DataRequestLongPd->GetFromHead();			// �L���[���擾
		
		EditLongPdInf(pQue);

		delete pQue;										// �L���[���J��
	}
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------
}
// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------

//------------------------------------------
// ���L������������
// bMode ���������[�h True:�S�Ẵe�[�u���N���A False:���ʂ̏�����
//------------------------------------------
void DataSetManager::OnClearTbl(bool bMode)
{

	//// �p���������s
	DataSetBaseManager::OnClearTbl(bMode);
	
	//// ��ʃ��O�h�~�΍�
	m_nErrCnt_Skip_Defect	= 0;
	m_nErrCnt_Err_Key		= 0;
	m_nErrCnt_Err_Coil		= 0;
	m_nErrCnt_NG_Val		= 0;
	m_nErrCnt_Max_Cnt		= 0;

	//// �X�e�[�^�X�A�|�C���^�̂݃N���A
	//memset(mtbl_pStatus, 0x00, sizeof(TBL_HY_STATUS));
	//memset(mtbl_pPoint->nRecDisp, 0x00, sizeof(mtbl_pPoint->nRecDisp));

	if (!bMode) {
		for (int nMen = 0; nMen<NUM_MEN; nMen++) {
			// �S���R�[�h
			for (int nRec = 0; nRec<HY_LST_DEFLIST_MAX; nRec++) {
				// �����\���Ώۃt���O�̂�OFF����B
				mtbl_pDefList->data[nMen].defect[nRec].typ.data.nModeDeflist = 0;
			}
		}
	}
}

//------------------------------------------
// ��M�f�[�^���L�������Z�b�g
// TO_SO_HEAD const* pHead �`���w�b�_
//------------------------------------------
void DataSetManager::OnSetTbl(TO_SO_HEAD const* pHead)
{
	// �x���e�X�g�p
	BYTE* pD = &((BYTE*)pHead)[sizeof(TO_SO_HEAD)];	// �f�[�^��

	switch (pHead->kind) {

//-----------------------------------------------------------
	case TO_SO_DATA_KIND_STATUS:					// 0:�X�e�[�^�X���
		SetData_Status(pHead, (TO_SO_DATA_STATUS*)pD);
		break;

//-----------------------------------------------------------
	case TO_SO_DATA_KIND_COIL_KEN:					// 1:�R�C����� (�����p)
		SetData_PointWrite(pHead, mtbl_pPoint);					// �����p����M�� �����|�C���^�X�V
		SetData_CoilInf(pHead, (TO_SO_DATA_COIL*)pD);
		break;

//-----------------------------------------------------------
	case TO_SO_DATA_KIND_COIL_RESULT:				// 2:�R�C������ (1�R�C������������)
		SetData_CoilResult(pHead);
		break;

//-----------------------------------------------------------
	case TO_SO_DATA_KIND_COIL_DSP:					// 3:�R�C����� (�\���p)
		SetData_CoilDsp(pHead);
		//DeleteNotDispImg();
		break;

//-----------------------------------------------------------
	case TO_SO_DATA_KIND_DETECT:					// 4:�r��� (�r���+�r�摜)
		SetData_Detect(pHead, (TO_SO_DATA_DETECT*)pD);
		break;
		//switch (pHead->base_head.subinf) {
		//case DIV_SYSTEM_OUT:
		//	SetData_CycleInf(pHead, (TO_SO_DATA_DETECT*)pD);
		//	break;
		//default:
		//	SetData_Detect(pHead, (TO_SO_DATA_DETECT*)pD);
		//	break;
		//}
		//break;

//-----------------------------------------------------------
	case TO_SO_DATA_KIND_LENGTH:					// 5:�ʔʒu���(�������u�ʒu�A�\����_�ʒu�A�e�\���R�C���ʔʒu���)
		SetData_Length(pHead, (TO_SO_DATA_LENGTH*)pD);
		break;
//-----------------------------------------------------------
	case TO_SO_DATA_KIND_CUT:						// 6:�J�b�g���
		SetData_Cut(pHead, (TO_SO_DATA_CUT*)pD);
		break;

//-----------------------------------------------------------
	case TO_SO_DATA_KIND_ALARM:						// 7:�x����
		SetData_Alarm(pHead, (TO_SO_DATA_ALARM*)pD);
		break;

//-----------------------------------------------------------
	case TO_SO_DATA_KIND_DEFLIST_CANCEL:			// 9:�d�����r���폜�v����M
		SetData_DefLst_Cancel(pHead);
		break;

//-----------------------------------------------------------
	case TO_SO_DATA_KIND_DETECT_NUM:				// 10:�r����� (�ŏ���ԓ�)
		SetData_Detect_Count(pHead);
		break;

//-----------------------------------------------------------

	default:
		LOG(em_ERR), "[%s] �f�[�^�敪�ُ� [OnSetTbl] [kind= %d] ", my_sThreadName, pHead->kind );
		break;
	}
}

//------------------------------------------
// �������݃|�C���^�X�V����
// TO_SO_HEAD const* pH �����p�R�C�����
// int rec ����̃��R�[�h (0�I���W��)
//------------------------------------------
void DataSetManager::UpDate_PintWrite(TO_SO_HEAD const* pH, int rec)
{

	//// �Ώۃe�[�u�����R�[�h��������
	memset(&mtbl_pPoint->pnt[rec], 0x00, sizeof(mtbl_pPoint->pnt[rec]));
	memset(&mtbl_pCoilInf->rec[rec], 0x00, sizeof(mtbl_pCoilInf->rec[rec]));
	memset(&mtbl_pCoilResult->rec[rec], 0x00, sizeof(mtbl_pCoilResult->rec[rec]));
	memset(&mtbl_pMapRow->rec[rec], 0x00, sizeof(mtbl_pMapRow->rec[rec]));
	memset(&mtbl_pDefectInf->rec[rec], 0x00, sizeof(mtbl_pDefectInf->rec[rec]));
	//memset(&mtbl_pCycleInf->rec[rec], 0x00, sizeof(mtbl_pCycleInf->rec[rec]));
	memset(&mtbl_pMaeInf->rec[rec], 0x00, sizeof(mtbl_pMaeInf->rec[rec]));
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	memset(&mtbl_pLongPd->rec[rec], 0x00, sizeof(mtbl_pLongPd->rec[rec]));
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

	////�Ώۃ��R�[�h�̎����R�[�h��������
	// �|�C���^���폜
	int nNextRec = GetRecIndex(rec, -1);	// (0�I���W��)
	memset(&mtbl_pPoint->pnt[nNextRec], 0x00, sizeof(mtbl_pPoint->pnt[nNextRec]));

	//// �摜���폜�˗�(�Ώۃ��R�[�h�̎����R�[�h)
	CString sPath;
	sPath = this->GetFolderPath(m_cPath_Img, nNextRec);
	mcls_pFDele->DeleteStart(sPath);

}
////------------------------------------------
//// �\���͈͊O�r�摜�폜����
////------------------------------------------
//void DataSetManager::DeleteNotDispImg()
//{
//	CString sPath;
//	//// �\���ΏۊO�摜�폜�˗�
//	int nDelrec = GetDeleteRec();
//	if (0 < nDelrec) {
//		LOG(em_MSG), "[%s] �\���͈͊O���R�[�h�摜�폜 [rec=%d]", my_sThreadName, nDelrec);
//		sPath = this->GetFolderPath(m_cPath_Img, nDelrec-1);
//		mcls_pFDele->DeleteStart(sPath);
//	}
//	else {
//		LOG(em_MSG), "[%s] �\���͈͊O���R�[�h�摜�폜���� [rec=%d]", my_sThreadName, nDelrec);
//	}
//}
//
////------------------------------------------
//// �\���͈͊O���R�[�h�擾����
//// Out 0:�����A1�` ���R�[�h�ԍ��i1�I���W���j
////------------------------------------------
//int DataSetManager::GetDeleteRec()
//{
//	// �\����_���R�[�h�ʒu�擾(0�I���W���ɂ���)
//	int nRec = mtbl_pPoint->nRecDispBase - 1;
//	// �\����_�ɖ����ꍇ�͏I��
//	if (0 > nRec) return 0;
//
//	int nCnt = 0;
//	for (int ii = 0; ii < TBL_HY_REC_MAX; ii++) {
//		// �\���\�܂��͕\���\��
//		if (mtbl_pPoint->pnt[nRec].emDispStatus == DIV_DSP_OK || mtbl_pPoint->pnt[nRec].emDispStatus == DIV_DSP_STANDBY) {
//			nCnt = nCnt + 1;
//		}
//		if (nCnt > DSP_COIL_MAX) {
//			return nRec + 1;
//		}
//		// �O��\�����R�[�h�ʒu�擾(0�I���W��)
//		nRec = GetRecIndex(nRec, 1);
//	}
//	// �\���R�C��MAX�ɓ��B���Ȃ������̂Ŗ���
//	return 0;
//}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���ۂ̏������݊֌W

//------------------------------------------
// 0:�X�e�[�^�X���
// TO_SO_HEAD const* pH ��M�w�b�_
//  const* pD �t�����
//------------------------------------------
void DataSetManager::SetData_Status(TO_SO_HEAD const* pH, TO_SO_DATA_STATUS const* pD)
{

	LOG(em_MSG), "[%s][0:�ð��] �^�]=(%s), �ғ�=(%s), ����=(%s/%s), �@��=(%s/%s)", my_sThreadName,
		DivNameManager::GetDivUnten(pD->nUnten), DivNameManager::GetDivKadou(pD->nKadou), 
		DivNameManager::GetDivKensa(pD->status.kensa[0]), DivNameManager::GetDivKensa(pD->status.kensa[1]), 
		DivNameManager::GetDivKiki(pD->status.kiki[0]), DivNameManager::GetDivKiki(pD->status.kiki[1])
		);

	////// �����ȊO�͖���
	//if (DIV_SYSTEM_SPM != pH->base_head.subinf) return;

	//// �f�[�^�Z�b�g
	mtbl_pStatus->emUnten = (EM_DIV_UNTEN)pD->nUnten;
	mtbl_pStatus->emKadou = (EM_DIV_KADOU)pD->nKadou;

	memcpy( &mtbl_pStatus->status, &pD->status, sizeof(TYP_STATUS_NOWSYSTEM));

	//// �R�C���P�ʂ̌����ُ���Z�b�g
	for(int nMen=0; nMen<NUM_MEN; nMen++) {
		int pos = (0 == nMen) ? DIV_LEN_POS_KEN_TOP: DIV_LEN_POS_KEN_BOT;
		int nRec = mtbl_pPoint->nRecDisp[pos] - 1;				// �e������ʒu�ł̃��R�[�h����
		if( 0 > nRec ) continue;								// �܂����R�[�h�������蓖��

		// �e������ʒu�̒ʔ���[m]���擾
		int nLenDisp = mtbl_pPoint->pnt[nRec].typ.nLenDisp[pos];

		if (DIV_KENSA_OK != mtbl_pStatus->status.kensa[nMen]) {
			// �X�e�[�^�X�̌����ُ�x������ȊO�́A�e������ʒu���R�[�h�̃R�C�����.������Ԃ�1(���̃R�C���ň��Ԃł��k�ގ�)���Z�b�g
			mtbl_pCoilInf->rec[nRec].coilinf.nCoilKensa = 1;
		}
		// �s���Ɍ��ݏ�Ԃ��Z�b�g
		SetRow_Inf(nRec, nMen, nLenDisp);
	}

	// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	// �������K�C�_���X�\������
	SetLongPdGuidance();
	// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------
}

//------------------------------------------
// 1:�R�C�����i�����p�j�X�V
// TO_SO_HEAD const* pH ��M�w�b�_
//  * pD			�t�����
//------------------------------------------
void DataSetManager::SetData_CoilInf(TO_SO_HEAD const* pH, TO_SO_DATA_COIL const* pD)
{
	LOG(em_MSG), "[%s][1:�����p���] <%s> %s, p=%d/%d, s=%d/%d, map=%d", my_sThreadName,
		pH->KizukenNo, pD->coilbase.cCoilNoIn, pD->pcode[0], pD->pcode[1], pD->scode[0], pD->scode[1], pD->nMapColNum );

	////// �����ȊO�͖���
	//if (DIV_SYSTEM_SPM != pH->base_head.subinf) return;

	//================================================
	// ����������

	//// ���R�[�h����
	int nRec = CheckKanriNo(pH->KizukenNo, mtbl_pPoint);
	if (0 > nRec) {
		LOG(em_ERR), "[%s] �Y�����R�[�h����", my_sThreadName);
		syslog(400, "[�����p�R�C�����:�Ǘ�No=%s]", pH->KizukenNo);		// �Y���Ǘ���񖳂��̂��߁A��M���������ł��܂���B
		return;
	}

	if (DIV_DSP_STANDBY != mtbl_pPoint->pnt[nRec].emDispStatus) {
		// �R�C����񏑍��ݕs��
		LOG(em_ERR), "[%s][1:�����p���] �R�C����񏑍��ݕs��(%d)", my_sThreadName, mtbl_pPoint->pnt[nRec].emDispStatus);
		syslog(406, "[�Ǘ�No=%s]", pH->KizukenNo);	// �����p�R�C�������������邱�Ƃ��ł��܂���ł����B
		return;
	}

	LOG(em_MSG), "[%s] ���R�[�h�m�� rec=%d", my_sThreadName, nRec);

	//================================================
	// �����p�R�C����� �Z�b�g

	COINFO_BASE& coilinf = mtbl_pCoilInf->rec[nRec].coilinf;
	memcpy(coilinf.cKizukenNo, pH->KizukenNo, SIZE_KIZUKEN_NO);
	for(int nMen=0; nMen<NUM_MEN; nMen++) {
		coilinf.scode[nMen] = pD->scode[nMen];	// �����\�ʏ��
		coilinf.pcode[nMen] = pD->pcode[nMen];	// �����p�^�[��
	}
	coilinf.nMapColNum = pD->nMapColNum;	// �}�b�v��
	coilinf.nCoilKensa = 0;					// �������(�܂��͐���)
	memcpy(&coilinf.base, &pD->coilbase, sizeof(pD->coilbase));		// �V�X�e�����ʂ̃R�C�����
	memcpy(&coilinf.data, &pD->coilinf,  sizeof(pD->coilinf));		// �R�C�����

	// ���������A�o�������Z�b�g
	int	nOutOffset[MAP_STAGE_NUM];
	GetOffsetOutLen(nRec, nOutOffset);		// �o���I�t�Z�b�g�ʒu�擾

	//================================================
	// ���̑��e�[�u���X�V
	//// �r���
	memcpy(mtbl_pDefectInf->rec[nRec].KizukenNo, pH->KizukenNo, SIZE_KIZUKEN_NO);
	memcpy(mtbl_pDefectInf->rec[nRec].cMeiNo, pD->coilbase.cMeiNo, SIZE_MEI_NO+1);

	for (int nStage = 0; nStage<MAP_STAGE_NUM; nStage++) {
		for (int nMen = 0; nMen<NUM_MEN; nMen++) {
			int nMaxIndex = GET_JUST_INDEX(COIL_LEN_MAX, mcls_pParam->GetCommonMast().nMapDsp[nStage]);	// �ő僌�R�[�h�ʒu
			int nInLen = 0;						// �����J�n��0
			int nOutLen = nOutOffset[nStage];	// �o���J�n�͏o���I�t�Z�b�g�ʒu�B
			for (int ii = 0; ii <= nMaxIndex; ii++) {
				nInLen += mcls_pParam->GetCommonMast().nMapDsp[nStage];
				nOutLen += mcls_pParam->GetCommonMast().nMapDsp[nStage];
				mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[ii].typ.nLen_i = nInLen;
				mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[ii].typ.nLen_o = nOutLen;
				mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[ii].typ.nKizuCnt = -1;
			}
		}
	}

	// �o�������̍Đݒ�i���i�㗬��)�̃��R�[�h����j
	int nNextRec = GetRecIndex(nRec, -1);
	// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
	//RecalOutLen(nRec);
	RecalOutLen(nRec, true);
	// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------

	return;
}

//------------------------------------------
// 2:�R�C������ (1�R�C������������)
// TO_SO_HEAD const* pH ��M�w�b�_
//------------------------------------------
void DataSetManager::SetData_CoilResult(TO_SO_HEAD const* pH)
{
	LOG(em_MSG), "[%s][2:��َ���] <%s> %dmm", my_sThreadName, pH->KizukenNo, pH->data_coilresult.len );

	////// �����ȊO�͖���
	//if (DIV_SYSTEM_SPM != pH->base_head.subinf) return;

	//================================================
	// ����������
	// �R�C�����R�[�h�̎擾
	int nRec = CheckKanriNo(pH->KizukenNo, mtbl_pPoint);
	if( 0 > nRec) {
		LOG(em_ERR), "[%s][2:��َ���] �Y�����R�[�h���� [%s]", my_sThreadName, pH->KizukenNo );
		syslog(400, "[�R�C������:�Ǘ�No=%s]", pH->KizukenNo);	// �Y���R�C�������̂��߁A��M���������ł��܂���B
		return;
	}

	//// �������Z�b�g
	int	nRowLast[MAP_STAGE_NUM];

	mtbl_pCoilResult->rec[nRec].nLen = pH->data_coilresult.len;

	for (int nStage = 0; nStage < MAP_STAGE_NUM; nStage++) {
		// ��M�����������ɂ�����s�ʒu���擾
		nRowLast[nStage] = GetStageLenToIndex(nStage, (pH->data_coilresult.len / 1000));
	}
	

	//// �s�␳
	for (int nMen = 0; nMen < NUM_MEN; nMen++) {
		for (int nStage = 0; nStage < MAP_STAGE_NUM; nStage++) {
			// ���ݏ������ݍs
			int nRowWrite = mtbl_pPoint->pnt[nRec].typ.nRowWrite[nMen][nStage];
			if (nRowLast[nStage] > nRowWrite) {
				// ���ݏ������ݍs������M�����������ɂ�����s�ʒu���傫���̂ŁA���̊Ԃ̍s���𖄂߂�
				RefRowStatus(nRec, nStage, nMen, nRowWrite, nRowLast[nStage]);
				// ���ݏ������ݍs���X�V(0�I���W��)
				mtbl_pPoint->pnt[nRec].typ.nRowWrite[nMen][nStage] = nRowLast[nStage];
			}
			//else if (nRowLast[nSpm][nStage] < nRowWrite) {

			// ���ݏ������ݍs������M�����������ɂ�����s�ʒu���������ꍇ�́A���̊Ԃ��r���|�C���^��
			// �������ɂ�����s�ʒu�ɍăZ�b�g����B
			int nMin = 0;
			int nMax = 0;
			int nKizuCnt = 0;
			// �r���
			//for (int nRow = nRowWrite; nRow > nRowLast[nSpm][nStage]; nRow--) {
			for (int nRow = MAP_ROW_NUM-1; nRow > nRowLast[nStage]; nRow--) {
				DSP_TYP_ROW& pWk = mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRow].typ;
				
				// �r��
				if (-1 < pWk.nKizuCnt) nKizuCnt += pWk.nKizuCnt;
				// �r��������
				pWk.nKizuCnt = -1;

				// �|�C���^���
				if ((0 == pWk.pnt[0]) || (0 == pWk.pnt[1]))	continue;
				if (0 == nMin || pWk.pnt[0] < nMin) nMin = pWk.pnt[0];			// �ŏ��r���|�C���^
				if (0 == nMax || pWk.pnt[1] > nMax) nMax = pWk.pnt[1];			// �ő��r���|�C���^
			
				// �|�C���^��񏉊���
				pWk.pnt[0] = 0;
				pWk.pnt[1] = 0;
				
			}
			for (int ii = nMin; ii <= nMax; ii++) {
				if (1 > ii) continue;
				DSP_TYP_DEFECTINF& pDef = mtbl_pDefectInf->rec[nRec].pnt[ii - 1].typ;
				// �r�̒���ʒu�����������Z���ꍇ�͑ΏۊO
				if (pDef.data.y <= pH->data_coilresult.len) continue;
				DSP_TYP_ROW& pWk = mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRowLast[nStage]].typ;
				if (0 == pWk.pnt[0] || ii < pWk.pnt[0]) pWk.pnt[0] = ii;		// �ŏ��r���|�C���^
				if (0 == pWk.pnt[1] || ii > pWk.pnt[1]) pWk.pnt[1] = ii;		// �ő��r���|�C���^
			}
			// �r���̐ݒ�			
			if (0 < nKizuCnt) {
				DSP_TYP_ROW& pWk = mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRowLast[nStage]].typ;
				pWk.nKizuCnt = nKizuCnt;
			}


			nMin = 0;
			nMax = 0;
			// �O�H���r���
			//for (int nRow = nRowWrite; nRow > nRowLast[nSpm][nStage]; nRow--) {
			for (int nRow = MAP_ROW_NUM - 1; nRow > nRowLast[nStage]; nRow--) {
					DSP_TYP_ROW& pWk = mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRow].typ;
				if ((0 == pWk.pntMae[0]) || (0 == pWk.pntMae[1]))	continue;
				if (0 == nMin || pWk.pntMae[0] < nMin) nMin = pWk.pntMae[0];			// �ŏ��r���|�C���^
				if (0 == nMax || pWk.pntMae[1] > nMax) nMax = pWk.pntMae[1];			// �ő��r���|�C���^

				// �|�C���^��񏉊���
				pWk.pntMae[0] = 0;
				pWk.pntMae[1] = 0;
			}
			for (int ii = nMin; ii <= nMax; ii++) {
				if (1 > ii) continue;
				DSP_TYP_MAE_FFINF& pDef = mtbl_pMaeInf->rec[nRec].men[nMen].maeinf[ii - 1].ff;
				// �r�̒���ʒu�����������Z���ꍇ�͑ΏۊO
				if (pDef.nY <= pH->data_coilresult.len) continue;
				DSP_TYP_ROW& pWk = mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRowLast[nStage]].typ;
				if (0 == pWk.pntMae[0] || ii < pWk.pntMae[0]) pWk.pntMae[0] = ii;		// �ŏ��r���|�C���^
				if (0 == pWk.pntMae[1] || ii > pWk.pntMae[1]) pWk.pntMae[1] = ii;		// �ő��r���|�C���^
			}
			//}
		}
	}
	

	//// �|�C���^���X�V
	for (int nStage = 0; nStage < MAP_STAGE_NUM; nStage++) {
		// ��M�����������ɂ�����s�ʒu�����������s�Ƃ���B(1�I���W��)
		mtbl_pPoint->pnt[nRec].typ.nRowLast[nStage] = nRowLast[nStage] + 1;
	}

	//// �o�������̍Đݒ�i���i�㗬��)�̃��R�[�h����j
	int nNextRec = GetRecIndex(nRec, -1);
	// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
	//RecalOutLen(nNextRec);
	RecalOutLen(nNextRec, true);
	// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------

	//================================================
	// �R�C���G���h����
	//// ��ʃ��O�o��
	// ���ʃ��O�o��
	if( m_nErrCnt_Skip_Defect ) { 
		LOG(em_ERR), "[%s] <��ʃ��O����> �r���X�L�b�v(%s:%d��)", my_sThreadName, pH->KizukenNo, m_nErrCnt_Skip_Defect); 
		syslog(404, "[�r���X�L�b�v:%s:%d��]", pH->KizukenNo, m_nErrCnt_Skip_Defect); 
	}
	if( m_nErrCnt_Err_Key ) { 
		LOG(em_ERR), "[%s] <��ʃ��O����> �Y�����R�[�h����(%s:%d��)", my_sThreadName, pH->KizukenNo, m_nErrCnt_Err_Key); 
		syslog(404, "[�Y�����R�[�h����:%s:%d��]", pH->KizukenNo, m_nErrCnt_Err_Key); 
	}
	if( m_nErrCnt_Err_Coil ) {
		LOG(em_ERR), "[%s] <��ʃ��O����> �Y���_���R�C���Ȃ�(%s:%d��)", my_sThreadName, pH->KizukenNo, m_nErrCnt_Err_Coil); 
		syslog(404, "[�Y�����R�[�h����:%s:%d��]", pH->KizukenNo, m_nErrCnt_Err_Coil); 
	}
	if( m_nErrCnt_NG_Val ) { 
		LOG(em_ERR), "[%s] <��ʃ��O����> �������ُ�(%s:%d��)", my_sThreadName, pH->KizukenNo, m_nErrCnt_NG_Val); 
		syslog(404, "[�������ُ�:%s:%d��]", pH->KizukenNo, m_nErrCnt_NG_Val); 
	}
	if( m_nErrCnt_Max_Cnt ) { 
		LOG(em_ERR), "[%s] <��ʃ��O����> �i�[����������B(%s:%d��)", my_sThreadName, pH->KizukenNo, m_nErrCnt_Max_Cnt); 
		syslog(404, "[�i�[����������B:%s:%d��]", pH->KizukenNo, m_nErrCnt_Max_Cnt); 
	}

	// ������
	m_nErrCnt_Skip_Defect	= 0;
	m_nErrCnt_Err_Key		= 0;
	m_nErrCnt_Err_Coil		= 0;
	m_nErrCnt_NG_Val		= 0;
	m_nErrCnt_Max_Cnt		= 0;

	LOG(em_MSG), "--------------------------- �����R�C���؂�ւ�� ---------------------------");
}

//------------------------------------------
// 3:�R�C�����i�\���p�j�X�V
// TO_SO_HEAD const* pH ��M�w�b�_
//------------------------------------------
void DataSetManager::SetData_CoilDsp(TO_SO_HEAD const* pH)
{
	bool	bRecal = false;				// �o�������Čv�Z�t���O

	LOG(em_MSG), "[%s][3:�\���p���] <%s>", my_sThreadName, pH->KizukenNo);

	// ���R�[�h�̎擾
	int nRec = CheckKanriNo(pH->KizukenNo, mtbl_pPoint);
	if( 0 > nRec) {
		LOG(em_ERR), "[%s][3:�\���p���] �Y�����R�[�h���� [%s]", my_sThreadName, pH->KizukenNo );
		syslog(400, "[�\���p�R�C�����:�Ǘ�No=%s]", pH->KizukenNo);	// �Y���R�C�������̂��߁A��M���������ł��܂���B
		return;
	}

	// �\����Ԃ��m�F
	if (DIV_DSP_STANDBY != mtbl_pPoint->pnt[nRec].emDispStatus) {
		// �ҋ@���ł͂Ȃ�
		LOG(em_ERR), "[%s][3:�\���p���] ��ԃG���[�̂��ߎ�t�s�� [%s].(%d)", my_sThreadName, pH->KizukenNo, mtbl_pPoint->pnt[nRec].emDispStatus );
		if (DIV_DSP_OK != mtbl_pPoint->pnt[nRec].emDispStatus) {
			// �d����M�ȊO�i���̃R�C���̌�Ɍ�����M�����R�C������ɕ\����M�������H�j
			syslog(408, "[�Ǘ�No=%s]", pH->KizukenNo);	// �\���p�R�C�������������邱�Ƃ��ł��܂���ł����B
		}
		return;
	}

	mtbl_pPoint->nRecDispBase = nRec + 1;				// �\����_�ʒu�ł̌��ݕ\�����R�[�h���X�V(1�I���W��)

	mtbl_pPoint->pnt[nRec].emDispStatus = DIV_DSP_OK;	// �\����Ԃ�\���ɂ���B

	// ���̃R�C��������Ɍ�����M���Ă���̂ɁA�\����M�����Ă��Ȃ��R�C���̉�ʕ\����Ԃ�ύX
	int ii = 1;
	while (1) {
		int nWk = GetRecIndex(nRec, ii);
		if (nWk == nRec) {
			// ���񂵂��̂ŏI��
			break;
		}
		if ((0 == mtbl_pPoint->pnt[nWk].emDispStatus)||(DIV_DSP_OK == mtbl_pPoint->pnt[nWk].emDispStatus)) {
			//�@�\����Ԃ��\���A�������́A0(�f�[�^�Ȃ�)�̃R�C��������������I��
			break;
		}
		if (DIV_DSP_STANDBY == mtbl_pPoint->pnt[nWk].emDispStatus) {
			// ���̃R�C������Ɍ�����M���Ă���̂ɁA�\����M����M���Ă��Ȃ�
			LOG(em_ERR), "[%s][3:�\���p���] �\������M�̂��ߏ��NG [%s]", my_sThreadName, mtbl_pPoint->pnt[nWk].cKizukenNo);
			syslog(409, "[�Ǘ�No=%s]", mtbl_pPoint->pnt[nWk].cKizukenNo);		// �\���p�R�C��������M���Ă��܂���B
			mtbl_pPoint->pnt[nWk].emDispStatus = DIV_DSP_NG;
			bRecal = true;
		}
		ii++;
	}

	if (bRecal) {
		// �o�������̍Đݒ�
		// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
		//RecalOutLen(nRec);
		RecalOutLen(nRec, true);
		// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------
	}

	return;
}

//------------------------------------------
// 4:�r��� (�r���+�r�摜)
// TO_SO_HEAD const* pH ��M�w�b�_
//  * pD			�t�����
//------------------------------------------
void DataSetManager::SetData_Detect(TO_SO_HEAD const* pH, TO_SO_DATA_DETECT const* pD)
{
	int   nMen = pH->men;
	int	  nRow[MAP_STAGE_NUM];	// �}�b�v�i�[�s�ʒu (0�I���W��)
	int   nKizuLen;
	//================================================
	// ����������

	//// ���R�[�h����
	// ���R�[�h�̎擾
	int nRec = CheckKanriNo(pH->KizukenNo, mtbl_pPoint);
	if (0 > nRec) {
		if (0 == m_nErrCnt_Err_Key) {
			LOG(em_ERR), "[%s][4:�r���] �Y�����R�[�h���� [%s]", my_sThreadName, pH->KizukenNo );
			syslog(400, "[�r���:�Ǘ�No=%s]", pH->KizukenNo);	// �Y���R�C�������̂��߁A��M���������ł��܂���B
		}
		m_nErrCnt_Err_Key++;
		return;
	}

	LOG(em_MSG), "[%s][4:�r���] <%s:%d:%d> Y=%dmm, T=%d, G=%d �T��=%d �d����=%d ���ײ��r=%d", my_sThreadName, 
		pH->KizukenNo, pH->men, pD->datas.nKizuNo, (int)pD->datas.y, pD->datas.nTid, pD->datas.nGid,
		pD->datas.nObjTansho, pD->datas.nObjDeflist, pD->datas.nModeDeflist);

	//================================================
	// �r�̒���ʒu����s�ʒu���Z�o����B
	nKizuLen = (int)pD->datas.y;
	// �����������ݒ�ς̉ߋ��R�C�� ���� �r�̒���ʒu�������������𒴂���ꍇ
	if ((0 < mtbl_pCoilResult->rec[nRec].nLen) && (nKizuLen > mtbl_pCoilResult->rec[nRec].nLen)) {
		// �s�ʒu�������������̈ʒu�ɕ␳
		nKizuLen = mtbl_pCoilResult->rec[nRec].nLen;
	}

	for (int nStage = 0; nStage<MAP_STAGE_NUM; nStage++) {
		nRow[nStage] = GetStageLenToIndex(nStage, int(nKizuLen / 1000));
	}
	

	//// �T���A�d���׈ꗗ��ʏo�͂̉ۂ��m�F
	if( 0 == pD->datas.nObjTansho && 0 == pD->datas.nObjDeflist){
		// �r���f�[�^�ɂāA�S��ʏo�͑Ώ��r�ł͂Ȃ�
		LOG(em_MSG), "[%s][4:�r���] �\���ΏۊO1 <%s:%d> Y=%d[mm]:%d/%d/%d  X=%d KizuNo=%d T=%d G=%d objT=%d objL=%d",
			my_sThreadName,
			pH->KizukenNo, pH->men, (int)pD->datas.y, nRow[0], nRow[1], nRow[2], pD->datas.nWidDiv,
			pD->datas.nKizuNo, pD->datas.nTid, pD->datas.nGid,
			pD->datas.nObjTansho, pD->datas.nObjDeflist );
		return;
	}

	//// �R�C�����̌����p�^�[���ɂ��A�r��A�O���[�h���T����ʏo�͑Ώۂł��邩���m�F
	//int tid = pD->data[0].nTid;
	//int gid = pD->data[0].nGid;
	//int	p = mtbl_pCoilInf->rec[nRec].coilinf.pcode[nMen];
	//bool bDspTansho = mcls_pParam->Check_Output( (ParamManager::EM_OUTPUTFLG)(ParamManager::OUTPUTFLG_DSP_O+nMen), p-1, tid, gid );
	//if( false == bDspTansho ){
	//	// �r��A�O���[�h���\���Ώۂł͂Ȃ�
	//	LOG(em_MSG), "[%s][4:�r���] �\���ΏۊO2 <%s:%d> Y=%dmm:%d/%d/%d X=%d, KizuNo=%d, T=%d, G=%d objT=%d", my_sThreadName, 
	//		pH->KizukenNo, pH->men, nRow[0][0], nRow[0][1], nRow[0][2], pD->data[0].nWidDiv,
	//		pD->data[0].nKizuNo, pD->data[0].nTid, pD->data[0].nGid, pD->data[0].nObjTansho );
	//	return;
	//}

	//// �ʃ`�F�b�N
	if( (0 > nMen) || (NUM_MEN <= nMen)) {
		if( 0 == m_nErrCnt_NG_Val ) {
			LOG(em_ERR), "[%s][4:�r���] �Y���ʏ��ُ� ��=%d", my_sThreadName, nMen);
			syslog(401, "[�r���:�Ǘ�No=%s:��=%d]", pH->KizukenNo, nMen);	// �������ُ�̂��߁A��M���������ł��܂���B
		}
		m_nErrCnt_NG_Val++;
		return;
	}

#ifdef LOG_DEBUG
	LOG(em_MSG), "[%s][4:�r���] <%s><Men:%d> Y=%dmm:%d/%d/%d X=%d, KizuNo=%d, T=%d, G=%d objT=%d objL=%d objM=%d", my_sThreadName, 
	pH->KizukenNo, pH->men, nRow[0][0], nRow[0][1], nRow[0][2], pD->data[0].nWidDiv,
	pD->data[0].nKizuNo, pD->data[0].nTid, pD->data[0].nGid, pD->data[0].nObjTansho, pD->datas[0].nObjDeflist, pD->datas[0].nObjDefmap );
#endif

	//// �D��x
	if(0 == pD->datas.nTYuu || 0 == pD->datas.nGYuu ) {
		if( 0 == m_nErrCnt_NG_Val ) {
			LOG(em_ERR), "[%s][4:�r���] �Y���r���ُ� �D��xT=%d �D��xG=%d", my_sThreadName, pD->datas.nTYuu, pD->datas.nGYuu);
			syslog(401, "[�r���:�Ǘ�No=%s:�D��xT=%d �D��xG=%d]", pH->KizukenNo, pD->datas.nTYuu, pD->datas.nGYuu);	// �������ُ�̂��߁A��M���������ł��܂���B
		}
		m_nErrCnt_NG_Val++;
		return;
	}

	//// ����`�F�b�N
	int nNum = mtbl_pDefectInf->rec[nRec].num;				// ������B�H
	if (MAX_MAP_DEFECT_NUM <= nNum) {
		if( 0 == m_nErrCnt_Max_Cnt) {
			LOG(em_ERR), "[%s][4:�r���] �i�[����������B�ɂ��j��", my_sThreadName);
			syslog(402, "[�Ǘ�No=%s]", pH->KizukenNo);	// �r�i�[����������B�ɂ��A�ȍ~���r����j�����܂��B
		}
		m_nErrCnt_Max_Cnt++;

		// �X�L�b�v�������Ƃ��r�}�b�v�ɒʒm
		SetRecvStatus(nRec, nMen, nRow[0], DIV_HY_RECVST_SKIP);

		return;
	}

	//================================================
	// ���ב΍�
	// �������L���[���w�茏���𒴂�����A�����X�L�b�v���{
	if(QUE_SKIP_COUNT < GetToHyQueCount()) {
		// �X�L�b�v�������Ƃ��r�}�b�v�ɒʒm
		SetRecvStatus(nRec, nMen, nRow[0], DIV_HY_RECVST_SKIP);
		
		if( 0 == m_nErrCnt_Skip_Defect ) {
			LOG(em_ERR), "[%s][4:�r���] �Y���r���X�L�b�v ������Que=%d", my_sThreadName, GetToHyQueCount());
			syslog(403, "[�Ǘ�No=%s]", pH->KizukenNo);		// �r����ʎ�M�ɂ��A�����ł��܂���B�r�����ꕔ�X�L�b�v���܂��B
		}
		m_nErrCnt_Skip_Defect++;
		return;
	}

	// �r���Z�b�g
	SetDefectInfo(pD, nRec, nMen, nNum);

	if (1 == pD->datas.nObjDeflist) {
		SetDefLst(pD, pH->KizukenNo, nRec, nMen);
	}


	// �r�|�C���^��o�^�i�ŏ��A�ő���X�V�j
	for (int nStage = 0; nStage<MAP_STAGE_NUM; nStage++) {
		DSP_TYP_ROW& pWk = mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRow[nStage]].typ;
		if (0 == pWk.pnt[0] || pWk.pnt[0] > nNum + 1) pWk.pnt[0] = nNum + 1;			// �ŏ��r���|�C���^(1�I���W��)
		if (0 == pWk.pnt[1] || pWk.pnt[1] < nNum + 1) pWk.pnt[1] = nNum + 1;			// �ő��r���|�C���^(1�I���W��)
	}

			//// �s���X�V
			//SetRow_Inf(nRec, nMen, nSpm, (int)(pD->datas[nSpm].y / 1000));

			//for (int nStage = 0; nStage<MAP_STAGE_NUM; nStage++) {

			//	int nRowWrite = mtbl_pPoint->pnt[nRec].typ[nSpm].nRowWrite[nMen][nStage];
			//	if (nRowWrite >= nRow[nSpm][nStage]) {
			//		// �i�[����Ă���f�[�^�Ɠ������A�������́A�傫���ꍇ�́A���݂̏����ݍs�͍X�V���Ȃ�
			//		continue;
			//	}

			//	// ���ݏ����ݍs�`�V���ȍs�܂ł̊Ԃ̍s���𖄂߂�
			//	DSP_TYP_ROW& pBaseRow = mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRowWrite].typ[nSpm];
			//	for (nRowWrite = nRowWrite + 1; nRowWrite<nRow[nSpm][nStage]; nRowWrite++) {
			//		DSP_TYP_ROW& pNewRow = mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRowWrite].typ[nSpm];
			//		// �ڍ׏��Z�b�g
			//		pNewRow.emKadou = pBaseRow.emKadou;										// �ғ����
			//		pNewRow.emKensa[DIV_SYSTEM_SPM] = pBaseRow.emKensa[DIV_SYSTEM_SPM];		// �������
			//		pNewRow.emKiki[DIV_SYSTEM_SPM] = pBaseRow.emKiki[DIV_SYSTEM_SPM];		// �@����
			//		SetDataStatusMerge(nRec, nStage, nMen, nSpm, nRowWrite, DIV_SYSTEM_SPM);
			//	}
			//	// ���ݏ������ݍs���X�V(0�I���W��)
			//	mtbl_pPoint->pnt[nRec].typ[nSpm].nRowWrite[nMen][nStage] = nRow[nSpm][nStage];
			//}


	return;
}



//------------------------------------------
// 5:�ʔʒu���(�e�ʔ|�C���g�ʒu���)
// TO_SO_HEAD const* pH ��M�w�b�_
//  * pD			�t�����
//------------------------------------------
void DataSetManager::SetData_Length(TO_SO_HEAD const* pH, TO_SO_DATA_LENGTH const* pD)
{
	/*
	LOG(em_MSG), "[%s][5:�ʔʒu���] (�J�����ʒu(�\)=%s:%d) (�\���ʒu(�\)=%s:%d)", my_sThreadName,
	pD->dsp[DIV_LEN_POS_KEN_TOP].KizukenNo, pD->dsp[DIV_LEN_POS_KEN_TOP].len, pD->dsp[DIV_LEN_POS_DSP_TOP].KizukenNo, pD->dsp[DIV_LEN_POS_DSP_TOP].len);
	LOG(em_MSG), "[%s][5:�ʔʒu���] (�J�����ʒu(��)=%s:%d) (�\���ʒu(��)=%s:%d)", my_sThreadName, 
	pD->dsp[DIV_LEN_POS_KEN_BOT].KizukenNo, pD->dsp[DIV_LEN_POS_KEN_BOT].len, pD->dsp[DIV_LEN_POS_DSP_BOT].KizukenNo, pD->dsp[DIV_LEN_POS_DSP_BOT].len);
*/

	bool	bKensa;
	int		nMen;
	bool	bRecal = false;				// �o�������Čv�Z�t���O
	int		nRecWk = 0;

	// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	SetLongPdGuidance();
	// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

	//================================================
	// �e�ʔ|�C���g
	for(int pos=0; pos<MAX_DISP_POS; pos++) {
		// ���R�[�h�s�����͖�����
		if( NULL == pD->dsp[pos].KizukenNo[0] ) continue;

		// ���R�[�h�̎擾
		int nRec = CheckKanriNo(pD->dsp[pos].KizukenNo, mtbl_pPoint);
		if (0 > nRec) {
			LOG(em_ERR), "[%s][5:�ʔʒu���] �e�ʔ|�C���g�Y�����R�[�h�����B[%s] pos<%d>", my_sThreadName, pD->dsp[pos].KizukenNo, pos);
			continue;
		}

		// �ʔ����̍s�ʒu���擾
		int nRow[MAP_STAGE_NUM];
		for (int nStage = 0; nStage < MAP_STAGE_NUM; nStage++) {
			nRow[nStage] = GetStageLenToIndex(nStage, pD->dsp[pos].len);
		}

		// �e�ʔ|�C���g�ʒu�ł̌��݃��R�[�h���X�V
		mtbl_pPoint->nRecDisp[pos] = nRec + 1;

		switch (pos) {
		case DIV_LEN_POS_KEN_TOP:					// �J�����ʒu(�\��)
			mtbl_pPoint->nRecWrite[0] = nRec + 1;	// �������u�ʒu�ł̌��݃��R�[�h���X�V(1�I���W��)
			bKensa = true;
			nMen = 0;
			break;
		case DIV_LEN_POS_KEN_BOT:					// �J�����ʒu(����)
			mtbl_pPoint->nRecWrite[1] = nRec + 1;	// �������u�ʒu�ł̌��݃��R�[�h���X�V(1�I���W��)
			bKensa = true;
			nMen = 1;
			break;
		case DIV_LEN_POS_DSP_TOP:					// �\����_(�\��)
			bKensa = false;
			nMen = 0;
			break;
		case DIV_LEN_POS_DSP_BOT:					// �\����_(����)
			bKensa = false;
			nMen = 1;
			break;
		default:
			bKensa = false;
		}
		if (bKensa) {
			// �s���Ɍ��ݏ�Ԃ��Z�b�g
			SetRow_Inf(nRec, nMen, pD->dsp[pos].len);
			for (int nStage = 0; nStage<MAP_STAGE_NUM; nStage++) {
				int nRowWrite = mtbl_pPoint->pnt[nRec].typ.nRowWrite[nMen][nStage];
				if (nRowWrite >= nRow[nStage]) {
					// �i�[����Ă���f�[�^�Ɠ������A�������́A�傫���ꍇ�́A���݂̏����ݍs�͍X�V���Ȃ�
					continue;
				}

				// ���ݏ������ݍs����V���ȏ����ݍs-1�܂ŁA�s���X�e�[�^�X���R�s�[
				RefRowStatus(nRec, nStage, nMen, nRowWrite, nRow[nStage] - 1);

				// ���ݏ������ݍs���X�V(0�I���W��)
				mtbl_pPoint->pnt[nRec].typ.nRowWrite[nMen][nStage] = nRow[nStage];
			}
			if (DIV_KENSA_OK != mtbl_pStatus->status.kensa[nMen]) {
				// �������u�ʒu���R�[�h�̃R�C�����.������Ԃ�1(���̃R�C���ň��Ԃł��k�ގ�)���Z�b�g
				mtbl_pCoilInf->rec[nRec].coilinf.nCoilKensa = 1;
			}
		}
						
		// �e������ʒu�̒ʔ���
		mtbl_pPoint->pnt[nRec].typ.nLenDisp[pos] = pD->dsp[pos].len;
		for (int nStage = 0; nStage<MAP_STAGE_NUM; nStage++) {
			// �e������ʒu�ł̌��ݕ\���s���X�V(1�I���W��)
			mtbl_pPoint->pnt[nRec].typ.nRowDisp[pos][nStage] = nRow[nStage] + 1;
			//----------->>>
			if (0 < mtbl_pPoint->pnt[nRec].typ.nRowLast[nStage]) {
				if (nRow[nStage] + 1 > mtbl_pPoint->pnt[nRec].typ.nRowLast[nStage]) {
					mtbl_pPoint->pnt[nRec].typ.nRowLast[nStage] = nRow[nStage] + 1;
					DSP_TYP_ROW& pWk = mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRow[nStage]].typ;
					pWk.emDspKensa = DIV_KENSA_OK;
					pWk.emDspKiki = DIV_KIKI_OK;
					bRecal = true;
					if (0 != nRecWk) nRecWk = nRec;
					LOG(em_MSG), "[%s] ���������s�ύX[%s] nRec=%d Stage=%d men=%d row=%d", 
						my_sThreadName, mtbl_pPoint->pnt[nRec].cKizukenNo, nRec, nStage, nMen, nRow[nStage]);
				}
			}
			//<<<-----------
		}
	}
	
	if (bRecal) {
		int nNextRec = GetRecIndex(nRecWk, -1);
		// �o�������̍Đݒ�
		// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
		//RecalOutLen(nNextRec);
		RecalOutLen(nNextRec, true);
		// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------
	}

	return;
}

//------------------------------------------
// 6:�J�b�g���
// TO_SO_HEAD const* pH ��M�w�b�_
//  * pD			�t�����
//------------------------------------------
void DataSetManager::SetData_Cut(TO_SO_HEAD const* pH, TO_SO_DATA_CUT const* pD)
{

	LOG(em_INF), "[%s][6:�J�b�g���] (�Ǘ�No=%s) (�敪=%d �ʒu=%dmm)", my_sThreadName, pH->KizukenNo, pD->nMode, pD->nPos);

	//================================================
	// ����������

	// �R�C�����R�[�h�̎擾
	int nRec = CheckKanriNo(pH->KizukenNo, mtbl_pPoint);
	if( 0 > nRec) {
		LOG(em_ERR), "[%s][6:�J�b�g���] �Y�����R�[�h����", my_sThreadName);
		syslog(400, "[�J�b�g���:�Ǘ�No=%s]", pH->KizukenNo);	// �Y���R�C�������̂��߁A��M���������ł��܂���B
		return;
	}

	//// �J�b�g���`�F�b�N
	int nCutCnt = mtbl_pCoilResult->rec[nRec].nCutCnt;
	if( MAX_SHACUT_COUNT <= nCutCnt ) {
		LOG(em_WAR), "[%s][6:�J�b�g���] �J�b�g���ُ� %d (%dmm)", my_sThreadName, pD->nMode, pD->nPos);
		return;
	}

	//================================================
	// �J�b�g���Z�b�g
	// ���菇�Ƀ\�[�g���邽�߁A�}���ʒu������
	int nInsert = 0;
// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
	bool bSamePos = false;
// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------
	for(nInsert=0; nInsert<mtbl_pCoilResult->rec[nRec].nCutCnt; nInsert++) {
		if( mtbl_pCoilResult->rec[nRec].cut[nInsert].nPos > pD->nPos ) break;
// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
		if( mtbl_pCoilResult->rec[nRec].cut[nInsert].nPos == pD->nPos ) 
		{
			bSamePos = true;
			break;
		}
// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------
	}
	// �}��
// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
	if( true == bSamePos ) 		
	{
		LOG(em_MSG), "[%s][6:�J�b�g���] ����ʒu��M�̂��ߕۑ����� %d (%dmm)", my_sThreadName, pD->nMode, pD->nPos);
	}
	else
	{
// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------	

		for(int ii=MAX_SHACUT_COUNT-1; ii>=nInsert; ii-- ) {
			if(MAX_SHACUT_COUNT-1 == ii) continue;
			memcpy( &mtbl_pCoilResult->rec[nRec].cut[ii+1], &mtbl_pCoilResult->rec[nRec].cut[ii], sizeof(TO_SO_DATA_CUT) );
		}
		// �Z�b�g	
		TO_SO_DATA_CUT& pWk = mtbl_pCoilResult->rec[nRec].cut[nInsert];
		pWk.nMode = pD->nMode;
		pWk.nPos = pD->nPos;
	mtbl_pCoilResult->rec[nRec].nCutCnt ++;

// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
	}
// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------

	//================================================
	// �V���[�J�b�g�敪�ݒ�
	BYTE nAdd = 0;
	if (DIV_CUT_FCUT == pD->nMode) nAdd = ROW_KIND_BOTTOM;					// ���i���[
	if (DIV_CUT_LCUT == pD->nMode) nAdd = ROW_KIND_UPPER;					// ���i��[
	if (DIV_CUT_DIV == pD->nMode) nAdd = (ROW_KIND_UPPER | ROW_KIND_BOTTOM);
	if (DIV_CUT_WPD_OTHER == pD->nMode) nAdd = ROW_KIND_YOSETU;				// �n�ړ_
	for (int nStage = 0; nStage<MAP_STAGE_NUM; nStage++) {
		int nRow = GetStageLenToIndex(nStage, pD->nPos / 1000);
		for (int nMen = 0; nMen<NUM_MEN; nMen++) {
			// �t�����ɃJ�b�g�敪�ɂ��t���O���Z�b�g�B
			mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRow].typ.nKind |= nAdd;
			if (ROW_KIND_BOTTOM & mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRow].typ.nKind) {
				// ���i���[�̏ꍇ�́A�V���[�J�b�g�敪��1�ɂ���B
				mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRow].typ.nCut = 1;
			}
		}
	}
	

	if(!(ROW_KIND_BOTTOM & nAdd)){
		// ���i���[�ȊO�͈ȍ~�̏����͍s��Ȃ�
		return;
	}

	// �o�������̍Đݒ�
	// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
	//RecalOutLen(nRec);
	RecalOutLen(nRec, true);
	// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------
	return;
}

//------------------------------------------
// 7:�x����
// TO_SO_HEAD const* pH ��M�w�b�_
//  * pD			�t�����
//------------------------------------------
void DataSetManager::SetData_Alarm(TO_SO_HEAD const* pH, TO_SO_DATA_ALARM const* pD)
{
	LOG(em_MSG), "[%s][7:�x����]", my_sThreadName );
	memcpy(mtbl_pAlarm, pD, sizeof(TO_SO_DATA_ALARM));
}


//------------------------------------------
// 9:�d�����r���폜�v��
// TO_SO_HEAD const* pH ��M�w�b�_
//------------------------------------------
void DataSetManager::SetData_DefLst_Cancel(TO_SO_HEAD const* pH)
{
	LOG(em_MSG), "[%s][9:�d�����r���폜�v��] (�Ǘ�No=%s) ������=%d �L�����Z���Ώۂ��r�A��=%d",
		my_sThreadName, pH->KizukenNo, pH->men, pH->deflist_cancel.nKizuSeq );

	//// �R�C�����R�[�h�̎擾
	//int nRec = CheckKanriNo(pH->KizukenNo, mtbl_pPoint);
	//if (0 > nRec) {
	//	LOG(em_ERR), "[%s][9:�d�����r���폜�v��] �Y�����R�[�h����", my_sThreadName);
	//	syslog(400, "[�d�����r���폜�v��:�Ǘ�No=%s]", pH->KizukenNo);	// �Y���R�C�������̂��߁A��M���������ł��܂���B
	//	return;
	//}

	//int nDefCnt;
	int nKizuSeq = pH->deflist_cancel.nKizuSeq;
	int nMen = pH->men;
	bool bFlg = false;


	// �d���׃e�[�u���X�V
	for (int ii = 0; ii<HY_LST_DEFLIST_MAX; ii++) {
		DSP_TYP_DEFECTINF& pWk = mtbl_pDefList->data[nMen].defect[ii].typ;
		if (0 == memcmp(mtbl_pDefList->data[nMen].defect[ii].KizuKenNo, pH->KizukenNo, SIZE_KIZUKEN_NO)) {
			// �Ώ��r���H
			if (nKizuSeq == pWk.data.nKizuNo) {
				if (1 == pWk.data.nModeDeflist) {
					pWk.data.nModeDeflist = 0;
					LOG(em_MSG), "[%s][�d�����r���폜�v��] �����\���t���O���� <%s> ��=%d �r�A��=%d Rec=%d",
						my_sThreadName, pH->KizukenNo, pH->men, pH->deflist_cancel.nKizuSeq, ii);
				}
				else {
					LOG(em_WAR), "[%s][�d�����r���폜�v��] �����\����Ώ� <%s> ��=%d �r�A��=%d Rec=%d",
						my_sThreadName, pH->KizukenNo, pH->men, pH->deflist_cancel.nKizuSeq, ii);
				}
				
				bFlg = true;
				break;
			}
		}
	}

	if (!bFlg) {
		LOG(em_WAR), "[%s][�d�����r���폜�v��] �Y���r�A�Ԗ��� <%s> ��=%d �rNo=%d",
			my_sThreadName, pH->KizukenNo, pH->men, pH->deflist_cancel.nKizuSeq);
	}
}

//------------------------------------------
// 10:�r����� (�ŏ���ԓ�)
// TO_SO_HEAD const* pH ��M�w�b�_
//------------------------------------------
void DataSetManager::SetData_Detect_Count(TO_SO_HEAD const* pH)
{
	int nMen = pH->men;
	int nKizuCnt = 0;
	int nRow = 0;

	LOG(em_INF), "[%s][10:�r����� (�ŏ���ԓ�)] (�Ǘ�No=%s) ������=%d ������=%d �r��=%d", my_sThreadName, pH->KizukenNo, pH->men, pH->data_defnum.len, pH->data_defnum.num);

	//================================================
	// ����������

	// �R�C�����R�[�h�̎擾
	int nRec = CheckKanriNo(pH->KizukenNo, mtbl_pPoint);
	if (0 > nRec) {
		LOG(em_ERR), "[%s][10:�r����� (�ŏ���ԓ�)] �Y�����R�[�h����", my_sThreadName);
		syslog(400, "[�r����� (�ŏ���ԓ�):�Ǘ�No=%s]", pH->KizukenNo);	// �Y���R�C�������̂��߁A��M���������ł��܂���B
		return;
	}

	// �ʃ`�F�b�N
	if ((0 > nMen) || (NUM_MEN <= nMen)) {
		LOG(em_ERR), "[%s][10:�r����� (�ŏ���ԓ�)] �Y���ʏ��ُ� ��=%d", my_sThreadName, nMen);
		syslog(401, "[10:�r����� (�ŏ���ԓ�):�Ǘ�No=%s:��=%d]", pH->KizukenNo, nMen);	// �������ُ�̂��߁A��M���������ł��܂���B
		return;
	}

	//================================================
	// �r���̒���ʒu����s�ʒu���Z�o����B
	int nKizuLen = (int)pH->data_defnum.len;
	// �����������ݒ�ς̉ߋ��R�C�� ���� �r�̒���ʒu�������������𒴂���ꍇ
	if ((0 < mtbl_pCoilResult->rec[nRec].nLen) && (nKizuLen > mtbl_pCoilResult->rec[nRec].nLen)) {
		// �s�ʒu�������������̈ʒu�ɕ␳
		nKizuLen = mtbl_pCoilResult->rec[nRec].nLen;
	}

	//================================================
	// �ŏ���Ԃ��r����ݒ�
	nKizuCnt = pH->data_defnum.num;		// �\���G���A���A�\���G���A��ɉ��Z�����r��
	// �Ώ��r���̍s�ʒu���擾
	nRow = GetStageLenToIndex(0, (nKizuLen / 1000));
	DSP_TYP_ROW& pWk = mtbl_pMapRow->rec[nRec].stage[0].men[nMen].row[nRow].typ;
	
	// �ŏ���Ԃ��r���Ď擾��
	// ���L���������r����-1�̏ꍇ�A��������Ώۍs���r�����擾���Ă��Ȃ�
	if (-1 < pWk.nKizuCnt) {
		// �ŏ���Ԃ��r�����擾���Ă����ꍇ�A
		// �O����r����\���G���A���A�\���G���A�傩�珜���āA�V�K�Ɏ�M�����r�������Z����
		nKizuCnt -= pWk.nKizuCnt;
	}

	// �ŏ���Ԃ��r�������L�������ɔ��f�i�㏑���j
	pWk.nKizuCnt = pH->data_defnum.num;

	//================================================
	// �\���G���A���A�\���G���A����r����ݒ�
	for (int nStage = 1; nStage < MAP_STAGE_NUM; nStage++) {
		// �Ώ��r���̍s�ʒu���擾
		nRow = GetStageLenToIndex(nStage, (nKizuLen / 1000));
		DSP_TYP_ROW& pWk = mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRow].typ;
		
		// �r������M(-1)�̏ꍇ�͏�����
		if (0 > pWk.nKizuCnt) pWk.nKizuCnt = 0;

		// �ŏ���Ԃ��r����\���G���A�� or �\���G���A��ɔ��f�i���Z�j
		pWk.nKizuCnt += nKizuCnt;
	}

	return;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �e�[�u������
//------------------------------------------
// �r���Z�b�g
// TO_SO_DATA_DETECT* pD	�r���
// int nRec					�e�[�u�����R�[�hNo (0�I���W��)
// int nMen					�\���敪
// int nIndex				�r��񃌃R�[�h�ʒu (0�I���W��)
//------------------------------------------
void DataSetManager::SetDefectInfo(TO_SO_DATA_DETECT const* pD, const int nRec, const int nMen, const int nIndex)
{
	//================================================
	// �O����
	DSP_TYP_DEFECTINF& pSet = mtbl_pDefectInf->rec[nRec].pnt[ nIndex ].typ;

	char  cPath[MAX_HT1_CAMANGLE][32];
	char* pPath[MAX_HT1_CAMANGLE];						// ���L�������p�X
	pPath[0] = pSet.image_path_s;
	pPath[1] = pSet.image_path_r;
	pPath[2] = pSet.image_path_r2;
	pPath[3] = pSet.image_path_r3;


	//================================================
	// �r���Z�b�g
	memcpy(&pSet.data, &pD->datas, sizeof(DETECT_BASE_DATA));
// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
	// �r�P�ʂ̏o������ ��t������
	GetDeliveryPosY(nRec, pD->datas.y, &pSet.data.y_out);
// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------

	for(int ii=0; ii<MAX_CAMANGLE; ii++) {
		sprintf(cPath[ii], "%s\\%d_%d_%d.jpg", (LPCTSTR)this->GetFolderPath(m_cPath_Img, nRec), nMen, pSet.data.nKizuNo, ii);	// �摜���r�A�Ԃɂ���
	}

	//================================================
	// �r�摜�o�^
	int	nImgAdd = 0;				// 0:�摜�o�^��  -9:��\�r�j��  -1:���X�g�ɉB����r�j��  -99:�Y��PC�摜�ۑ�����

	//--------------------------------
	//// ���O�`�F�b�N
	//if( 0 != pD->data[0].nFrameDefMax) {
	if (1) {
		if( 0 == mque_pImgSave->GetFreeCount() ) nImgAdd = -9;
	} else {
		if( 0 == mque_pImgSaveLow->GetFreeCount() ) nImgAdd = -1;
	}
	// �摜�o�^�L��
	if (1 == m_nImgSaveOff) nImgAdd = -99;

	//--------------------------------
	//// �摜�o�^
	if( 0 == nImgAdd ) {
		// ����
		int nOffset = 0;

		FileSaveManager::TYP_SAVE_IMG* pImg = new FileSaveManager::TYP_SAVE_IMG;
		memset(pImg, 0x00, sizeof(FileSaveManager::TYP_SAVE_IMG));

		//// �Z�b�g
		pImg->nSetCnt = MAX_IMAGE_CNT;
		for(int ii=0; ii<MAX_IMAGE_CNT; ii++) {
			// �\���Ώ��r���H
			if( mcls_pParam->Check_ImgDsp( pD->datas.nTid, ii ) ) {
				if (0 < pD->image_len[ii]) {
					int nBufSize = GET_JUST_SIZE(pD->image_len[ii], 512);
					pImg->nSize[ii] = nBufSize;
					pImg->pImg[ii] = (BYTE*)VirtualAlloc(NULL, nBufSize, MEM_COMMIT, PAGE_READWRITE);		// ���������ݗp�ɃZ�N�^�P�ʂŗ̈���m��

					memcpy(pImg->pImg[ii], &pD->image[nOffset], pD->image_len[ii]);
					strcpy(pImg->cPath[ii], cPath[ii]);

					pImg->pMemPath[ii] = pPath[ii];				// ���L�������p�X
				}
			}
			nOffset += pD->image_len[ii];
		}

		//// �o�^
		BOOL bRetc;							// �o�^����

//		if(0 != pD->data[0].nFrameDefMax) {		
		if (1) {
				if( 10 < mque_pImgSave->GetCount() ) {
				bRetc = mque_pImgSave->AddToHead(pImg);			// ���܂��Ă���� ���A���^�C���\���̂Ƃ��ɁA����摜�\�����Ԃɍ���Ȃ��΍�
			} else {
				bRetc = mque_pImgSave->AddToTail(pImg);
			}
		} else {
			if( 10 < mque_pImgSaveLow->GetCount() ) {
				bRetc = mque_pImgSaveLow->AddToHead(pImg);
			} else {
				bRetc = mque_pImgSaveLow->AddToTail(pImg);
			}
		}

		if( ! bRetc ) {
			for(int ii=0; ii<pImg->nSetCnt; ii++ ) { SAFE_DELETE(pImg->pImg[ii]); }
			delete pImg;
			nImgAdd = -9;
		}

	}

	//// NG�̏ꍇ
	if( 0 != nImgAdd ) {
		// �摜������
		for(int ii=0; ii<MAX_HT1_CAMANGLE; ii++) {
			pPath[ii][0] = 0x00;		// �O�̂��߂ɋ��L�������̉摜�p�X���N���A
		}
		// �K�C�_���X
		if( -9 == nImgAdd) {
			LOG(em_ERR), "[%s] �r�摜�o�^�j�� [%s] index=%d, No=%d, T=%d, G=%d", 
				my_sThreadName, mtbl_pDefectInf->rec[nRec].KizukenNo, nIndex, pD->datas.nKizuNo, pD->datas.nTid, pD->datas.nGid);
		} else if( -1 == nImgAdd) {
			LOG(em_WAR), "[%s] �r�摜�o�^�X�L�b�v [%s] index=%d, No=%d", 
				my_sThreadName, mtbl_pDefectInf->rec[nRec].KizukenNo, nIndex, pD->datas.nKizuNo);
		} else if (-99 == nImgAdd) {
			//LOG(em_WAR), "[%s] �r�摜�o�^���� [%s] index=%d, No=%d",
			//	my_sThreadName, mtbl_pDefectInf->rec[nRec].KizukenNo, nIndex, pD->datas[0].nKizuNo);
		}
	}


	//================================================
	// �|�C���^�X�V
	mtbl_pDefectInf->rec[nRec].num = nIndex+1;	
}


//------------------------------------------
// �d���׏��Z�b�g
// TO_SO_DATA_DETECT* pD	�r���
// char* pKizukenNo			�Ǘ�No
// int nRec					�e�[�u�����R�[�hNo (0�I���W��)
// int nMen					�\���敪
//------------------------------------------
void DataSetManager::SetDefLst(TO_SO_DATA_DETECT const* pD, char const* pKizukenNo, const int nRec, const int nMen)
{
	int nNewRec = mtbl_pDefList->head.nPos[nMen] + 1;

	if (MAX_DEFLIST_BUF < nNewRec) {
		nNewRec = 1;
	}

	int ick;			// �㏑���\�f�[�^�����p
	int iDelRec;		// �ŌÂ̕����r�̃��R�[�h�ʒu�i�[�p(0�I���W��) & �f�[�^�V�t�g���{�t���O���p
	iDelRec = -1;
	if (0 < mtbl_pDefList->data[nMen].defect[nNewRec - 1].typ.data.nKizuNo ) {
		if (1 == mtbl_pDefList->data[nMen].defect[nNewRec - 1].typ.data.nModeDeflist) {
			// �ŌÂ̕����r��T��
			ick = nNewRec-1;
			for (int ii = 0; ii < MAX_DEFLIST_BUF; ii++) {
				if (0 == mtbl_pDefList->data[nMen].defect[ick].typ.data.nModeDeflist) {
					// �ŌÂ̕����r���R�[�h�ʒu�Z�b�g
					iDelRec = ick;
					break;
				}
				ick = ick + 1;
				if (MAX_DEFLIST_BUF <= ick) ick = 0;
			}
			if (-1 == iDelRec) {
				LOG(em_ERR), "�㏑���\�f�[�^����[�Ǘ�No = %s] ��=%d No=%d", pKizukenNo, nMen, mtbl_pDefList->data[nMen].defect[nNewRec - 1].typ.data.nKizuNo);
				// syslog(???, "[�r���Ǘ�No = %s]", pHead->KizukenNo);
				return;
			}
		}
	}

	// �f�[�^�V�t�g���{(iDelRec=-1�̎��͕s�v)
	if (-1 != iDelRec) {
		DataShiftDefList(nMen, iDelRec, nNewRec - 1);
	}
	// ������
	memset(&mtbl_pDefList->data[nMen].defect[nNewRec - 1], 0x00, sizeof(mtbl_pDefList->data[nMen].defect[nNewRec - 1]));
	// �f�[�^�Z�b�g
	memcpy(mtbl_pDefList->data[nMen].defect[nNewRec - 1].KizuKenNo, pKizukenNo, SIZE_KIZUKEN_NO);
	memcpy(mtbl_pDefList->data[nMen].defect[nNewRec - 1].CoilNo, mtbl_pCoilInf->rec[nRec].coilinf.base.cCoilNoIn, SIZE_COIL_NO + 1);
	memcpy(mtbl_pDefList->data[nMen].defect[nNewRec - 1].MeiNo, mtbl_pCoilInf->rec[nRec].coilinf.base.cMeiNo, SIZE_MEI_NO + 1);

	//================================================
	// �O����
	DSP_TYP_DEFECTINF& pSet = mtbl_pDefList->data[nMen].defect[nNewRec - 1].typ;

	char  cPath[MAX_HT1_CAMANGLE][32];
	char* pPath[MAX_HT1_CAMANGLE];						// ���L�������p�X
	pPath[0] = pSet.image_path_s;
	pPath[1] = pSet.image_path_r;
	pPath[2] = pSet.image_path_r2;
	pPath[3] = pSet.image_path_r3;

	//================================================
	// �r���Z�b�g
	memcpy(&pSet.data, &pD->datas, sizeof(DETECT_BASE_DATA));

	int nFileNo = 0;
	// �d���׈ꗗ�摜�t�@�C���ۑ��p�t�@�C��No�擾
	int nNoWk;
	nNoWk = m_nLstSaveNo[nMen] + 1;
	if (nNoWk > LST_SAVE_NO_MAX) {
		nNoWk = 1;
	}
	GetDefListFileNo(nMen, nNoWk, &nFileNo);
	m_nLstSaveNo[nMen] = nFileNo;

	for (int ii = 0; ii<MAX_CAMANGLE; ii++) {
		sprintf(cPath[ii], "%s\\%d_%d_%d.jpg", (LPCTSTR)this->GetFolderPath(m_cPath_Lst, -1), nMen, nFileNo, ii);	// �摜���r�A�Ԃɂ���
	}
	LOG(em_MSG), "[%s] �d���׏�� men=%d rec=%d FileNo=%d", my_sThreadName, nMen, nNewRec, nFileNo);

	//================================================
	// �r�摜�o�^
	int	nImgAdd = 0;				// 0:�摜�o�^��  -9:��\�r�j��  -1:���X�g�ɉB����r�j��  -99:�Y��PC�摜�ۑ�����

									//--------------------------------
									//// ���O�`�F�b�N
									//if( 0 != pD->data[0].nFrameDefMax) {
	if (1) {
		if (0 == mque_pImgSave->GetFreeCount()) nImgAdd = -9;
	}
	else {
		if (0 == mque_pImgSaveLow->GetFreeCount()) nImgAdd = -1;
	}
	// �摜�o�^�L��
	if (1 == m_nLstImgSaveOff) nImgAdd = -99;

	//--------------------------------
	//// �摜�o�^
	if (0 == nImgAdd) {
		// ����
		int nOffset = 0;

		FileSaveManager::TYP_SAVE_IMG* pImg = new FileSaveManager::TYP_SAVE_IMG;
		memset(pImg, 0x00, sizeof(FileSaveManager::TYP_SAVE_IMG));

		//// �Z�b�g
		pImg->nSetCnt = MAX_IMAGE_CNT;
		for (int ii = 0; ii<MAX_IMAGE_CNT; ii++) {
			// �\���Ώ��r���H
			if (mcls_pParam->Check_ImgDsp(pD->datas.nTid, ii)) {
				if (0 < pD->image_len[ii]) {
					int nBufSize = GET_JUST_SIZE(pD->image_len[ii], 512);
					pImg->nSize[ii] = nBufSize;
					pImg->pImg[ii] = (BYTE*)VirtualAlloc(NULL, nBufSize, MEM_COMMIT, PAGE_READWRITE);		// ���������ݗp�ɃZ�N�^�P�ʂŗ̈���m��

					memcpy(pImg->pImg[ii], &pD->image[nOffset], pD->image_len[ii]);
					strcpy(pImg->cPath[ii], cPath[ii]);

					pImg->pMemPath[ii] = pPath[ii];				// ���L�������p�X
				}
			}
			nOffset += pD->image_len[ii];
		}

		//// �o�^
		BOOL bRetc;							// �o�^����

											//		if(0 != pD->data[0].nFrameDefMax) {		
		if (1) {
			if (10 < mque_pImgSave->GetCount()) {
				bRetc = mque_pImgSave->AddToHead(pImg);			// ���܂��Ă���� ���A���^�C���\���̂Ƃ��ɁA����摜�\�����Ԃɍ���Ȃ��΍�
			}
			else {
				bRetc = mque_pImgSave->AddToTail(pImg);
			}
		}
		else {
			if (10 < mque_pImgSaveLow->GetCount()) {
				bRetc = mque_pImgSaveLow->AddToHead(pImg);
			}
			else {
				bRetc = mque_pImgSaveLow->AddToTail(pImg);
			}
		}

		if (!bRetc) {
			for (int ii = 0; ii<pImg->nSetCnt; ii++) { SAFE_DELETE(pImg->pImg[ii]); }
			delete pImg;
			nImgAdd = -9;
		}

	}

	//// NG�̏ꍇ
	if (0 != nImgAdd) {
		// �摜������
		for (int ii = 0; ii<MAX_HT1_CAMANGLE; ii++) {
			pPath[ii][0] = 0x00;		// �O�̂��߂ɋ��L�������̉摜�p�X���N���A
		}
		// �K�C�_���X
		if (-9 == nImgAdd) {
			LOG(em_ERR), "[%s] �d�����r�摜�o�^�j�� [%s] Rec=%d, No=%d, T=%d, G=%d",
				my_sThreadName, mtbl_pDefList->data[nMen].defect[nNewRec - 1].KizuKenNo, nNewRec, pD->datas.nKizuNo, pD->datas.nTid, pD->datas.nGid);
		}
		else if (-1 == nImgAdd) {
			LOG(em_WAR), "[%s] �d�����r�摜�o�^�X�L�b�v [%s] Rec=%d, No=%d",
				my_sThreadName, mtbl_pDefList->data[nMen].defect[nNewRec - 1].KizuKenNo, nNewRec, pD->datas.nKizuNo);
		}
		else if (-99 == nImgAdd) {
			//LOG(em_WAR), "[%s] �d�����r�摜�o�^���� [%s] Rec=%d, No=%d",
			//	my_sThreadName, mtbl_pDefList->data[nMen].defect[nNewRec - 1].KizuKenNo, nNewRec, pD->datas[0].nKizuNo);
		}
	}

	//================================================
	// �|�C���^�X�V
	mtbl_pDefList->head.nPos[nMen] = nNewRec;
}
////------------------------------------------
//// �d���׈ꗗ�摜�t�@�C���ۑ��p�t�@�C��No�擾
//// int nO_U					�\���敪
//// int nRec					�e�[�u�����R�[�hNo (1�I���W��)
//// int* nFileNo			    �d���׈ꗗ�摜�t�@�C���ۑ��p�t�@�C��No(1�I���W��)
////------------------------------------------
//void DataSetManager::GetDefListFileNo(int nO_U, int nRec, int* nFileNo)
//{
//	//================================================
//	// �O����
//
//	char  cPath[MAX_HT1_CAMANGLE][32];
//	char* pPath[MAX_HT1_CAMANGLE];						// ���L�������p�X
//
//	*nFileNo = nRec;
//
//	for (int ii = 0; ii<MAX_CAMANGLE; ii++) {
//		sprintf(cPath[ii], "%s\\%d_%d_%d.jpg", this->GetFolderPath(m_cPath_Lst, -1), nO_U, nRec, ii);	// �摜���r�A�Ԃɂ���
//	}
//
//	bool bCk = false;
//
//	// �Y�����R�[�h�ԍ��͎g�p�����H
//	for (int ii = 0; ii < HY_LST_DEFLIST_MAX; ii++) {
//		if (nRec - 1 == ii) continue;
//		DSP_TYP_DEFECTINF& pSet = mtbl_pDefList->data[nO_U].defect[ii].typ;
//		pPath[0] = pSet.image_path_s;
//		pPath[1] = pSet.image_path_r;
//		pPath[2] = pSet.image_path_r2;
//		pPath[3] = pSet.image_path_r3;
//
//		for (int jj = 0; jj < MAX_CAMANGLE; jj++) {
//			if (0 == strcmp(cPath[jj], pPath[jj])) {
//				// �g�p��
//				bCk = true;
//				break;
//			}
//		}
//		// �g�p���������̂Ō����I��
//		if (bCk) {
//			break;
//		}
//	}
//	// ���g�p�������B
//	if (!bCk) {
//		*nFileNo = nRec;
//		//LOG(em_MSG), "[%s] ���R�[�h�ԍ��g�p rec=%d FileNo=%d", my_sThreadName, nRec, *nFileNo);
//		return;
//	}
//
//	// �󂫂�T��
//	for (int ii = 0; ii < HY_LST_DEFLIST_MAX; ii++) {
//		if (nRec - 1 == ii) continue;
//		for (int jj = 0; jj<MAX_CAMANGLE; jj++) {
//			sprintf(cPath[jj], "%s\\%d_%d_%d.jpg", this->GetFolderPath(m_cPath_Lst, -1), nO_U, ii+1, jj);	// �摜���r�A�Ԃɂ���
//		}
//		bCk = false;
//		for (int jj = 0; jj < HY_LST_DEFLIST_MAX; jj++) {
//			DSP_TYP_DEFECTINF& pSet = mtbl_pDefList->data[nO_U].defect[jj].typ;
//			pPath[0] = pSet.image_path_s;
//			pPath[1] = pSet.image_path_r;
//			pPath[2] = pSet.image_path_r2;
//			pPath[3] = pSet.image_path_r3;
//			for (int kk = 0; kk < MAX_CAMANGLE; kk++) {
//				if (0 == strcmp(cPath[kk], pPath[kk])) {
//					// �g�p��
//					bCk = true;
//					break;
//				}
//			}
//
//			if (bCk) {
//				// �g�p���Ȃ̂Ŏ�������
//				break;
//			}
//		}
//		if ( !bCk) {
//			// ���g�p�Ȃ̂ł��̔ԍ����g�p
//			*nFileNo = ii + 1;
//			//LOG(em_MSG), "[%s] �󂫔ԍ��g�p rec=%d FileNo=%d", my_sThreadName, nRec, *nFileNo);
//			return;
//		}
//	}
//}

//------------------------------------------
// �d���׈ꗗ�摜�t�@�C���ۑ��p�t�@�C��No�擾
// int nO_U					�\���敪
// int nNo					�ۑ��p�t�@�C��No ���(1�I���W��)
// int* nFileNo			    �d���׈ꗗ�摜�t�@�C���ۑ��p�t�@�C��No(1�I���W��)
//------------------------------------------
void DataSetManager::GetDefListFileNo(int nO_U, int nNo, int* nFileNo)
{
	//================================================
	// �O����

	char  cPath[MAX_HT1_CAMANGLE][32];
	char* pPath[MAX_HT1_CAMANGLE];						// ���L�������p�X
	bool bCk = false;
	int nWk;
	
	nWk = nNo;
	if (1 > nWk || LST_SAVE_NO_MAX < nWk) {
		nWk = 1;
	}

	// �󂫂�T��
	for (int ii = 0; ii < LST_SAVE_NO_MAX; ii++) {

		for (int jj = 0; jj<MAX_CAMANGLE; jj++) {
			sprintf(cPath[jj], "%s\\%d_%d_%d.jpg", (LPCTSTR)this->GetFolderPath(m_cPath_Lst, -1), nO_U, nWk, jj);	// �摜���r�A�Ԃɂ���
		}
		bCk = false;
		for (int jj = 0; jj < HY_LST_DEFLIST_MAX; jj++) {
			DSP_TYP_DEFECTINF& pSet = mtbl_pDefList->data[nO_U].defect[jj].typ;
			pPath[0] = pSet.image_path_s;
			pPath[1] = pSet.image_path_r;
			pPath[2] = pSet.image_path_r2;
			pPath[3] = pSet.image_path_r3;
			for (int kk = 0; kk < MAX_CAMANGLE; kk++) {
				if (0 == strcmp(cPath[kk], pPath[kk])) {
					// �g�p��
					bCk = true;
					break;
				}
			}

			if (bCk) {
				// �g�p���Ȃ̂Ŏ�������
				break;
			}
		}
		if (!bCk) {
			// ���g�p�Ȃ̂ł��̔ԍ����g�p
			*nFileNo = nWk;
			//LOG(em_MSG), "[%s] �󂫔ԍ��g�p rec=%d FileNo=%d", my_sThreadName, nRec, *nFileNo);
			return;
		}
		nWk = nWk + 1;
		if (LST_SAVE_NO_MAX < nWk) {
			nWk = 1;
		}
	}
}

//------------------------------------------
// �d���׈ꗗ�\���r���f�[�^�V�t�g����
// int nO_U					�\���敪(0:�\, 1:��)
// int nStartPos            �V�t�g�J�n���R�[�h�ʒu(0�I���W��)
// int nEndPos              �V�t�g�I�����R�[�h�ʒu(0�I���W��)
//------------------------------------------
void DataSetManager::DataShiftDefList(int nO_U, int nStartPos, int nEndPos)
{
	int iNewRec;	// �R�s�[�惌�R�[�h�ʒu(0�I���W��)
	int iOldRec;	// �R�s�[�����R�[�h�ʒu(0�I���W��)
	int	ii;			// ���[�v�J�E���^

					// �V�t�g�s�v
	if (nStartPos == nEndPos) {
		// �ŏI���R�[�h������
		memset(&mtbl_pDefList->data[nO_U].defect[nEndPos], 0x00, sizeof(mtbl_pDefList->data[nO_U].defect[nEndPos]));
		return;
	}

	iNewRec = nStartPos;
	iOldRec = iNewRec - 1;
	if (0 > iOldRec) iOldRec = MAX_DEFLIST_BUF - 1;

	// �P���R�[�h�V�t�g
	for (ii = 0; ii < MAX_DEFLIST_BUF; ii++) {
		// �f�[�^�R�s�[
		memcpy(&mtbl_pDefList->data[nO_U].defect[iNewRec], &mtbl_pDefList->data[nO_U].defect[iOldRec], sizeof(mtbl_pDefList->data[nO_U].defect[iNewRec]));

		// ���ҏW���R�[�h�Z�b�g
		iNewRec = iNewRec - 1;
		if (0 > iNewRec) iNewRec = MAX_DEFLIST_BUF - 1;
		iOldRec = iNewRec - 1;
		if (0 > iOldRec) iOldRec = MAX_DEFLIST_BUF - 1;
		if (iNewRec == nEndPos) break;
	}
	// �ŏI���R�[�h������
	memset(&mtbl_pDefList->data[nO_U].defect[nEndPos], 0x00, sizeof(mtbl_pDefList->data[nO_U].defect[nEndPos]));

}

//------------------------------------------
// �r�}�b�v�̏���M��ԂɃf�[�^�Z�b�g
// int nRec	�Y���R�C���̃��R�[�h�ʒu (0�I���W��)
// int nMen	��
// int row  �ŏ��s���R�[�h�ʒu (0�I���W��)
// int st	�X�e�[�^�X���
//------------------------------------------
void DataSetManager::SetRecvStatus(const int nRec, const int nMen, const int row, const int st)
{
	int	  y;				// �}�b�v�i�[�s�ʒu (0�I���W��)
	for(int nStage=0; nStage<MAP_STAGE_NUM; nStage++) {
		// �Ώۈʒu�Z�o
		y = GetStageIndex(nStage, row);

		// ����
		bool bOk = false;
		int nWk = mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[y].typ.nEnable;
		if( 0 == nWk ) {					// ���Z�b�g
			bOk = true;							// �������ɃZ�b�gOK
		} else if(  0 < nWk ){				// ����̕�
			if(st>0 && nWk < st) bOk = true;		// ��萳��
			if(st<0) bOk = true;					// �ُ�

		} else if(  0 > nWk ){				// �ُ�̕�
			if(st<0 && nWk > st) bOk = true;		// ���ُ�
		}
		
		// �Z�b�g
		if(bOk) mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[y].typ.nEnable = st;
	}
}


//------------------------------------------
// �s���Ƀf�[�^�Z�b�g
// int nRec	�Y���R�C���̃��R�[�h�ʒu (0�I���W��)
// int nMen	��
// int nLen	���݋��� [m]
//------------------------------------------
void DataSetManager::SetRow_Inf(const int nRec, const int nMen,const int nLen)
{
	if(0 > nRec) return;
	bool nSkip = false;						// �s�����L�莞 true

	for(int nStage =0; nStage<MAP_STAGE_NUM; nStage++) {
		int nRow = GetStageLenToIndex(nStage, nLen);			// �Ώۈʒu�Z�o

		if (0 == nStage) {
			SetRecvStatus(nRec, nMen, nRow, DIV_HY_RECVST_OK);	// ����M��� ����
		}

		DSP_TYP_ROW& pWk = mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRow].typ;

		// �ڍ׏��Z�b�g
		if(pWk.emKadou < mtbl_pStatus->emKadou )			pWk.emKadou = mtbl_pStatus->emKadou;				// �ғ����
		if (pWk.emDspKensa < mtbl_pStatus->status.kensa[nMen]) pWk.emDspKensa = mtbl_pStatus->status.kensa[nMen];		// �������
		if (pWk.emDspKiki  < mtbl_pStatus->status.kiki[nMen]) pWk.emDspKiki = mtbl_pStatus->status.kiki[nMen];		// �@����

	}
}

//------------------------------------------
// �o���I�t�Z�b�g�ʒu�擾
// int nRec			�Y���R�C���̃��R�[�h�ʒu (0�I���W��)
// int * pOutLen	�o���I�t�Z�b�g�ʒu(�ŏ�/��/��)
//------------------------------------------
void DataSetManager::GetOffsetOutLen(const int nRec, int * pOutLen)
{

	// �����l���Z�b�g���Ă����B
	for (int nStage = 0; nStage<MAP_STAGE_NUM; nStage++) {
		pOutLen[nStage] = 0;
	}

	//// �|�C���^���e�[�u�����`�F�b�N�i�㗬�������j
	int		ii = 1;		// ���̃��R�[�h����
	while (1) {
		int nWk = GetRecIndex(nRec, ii);
		if (ii && (nWk == nRec)) {
			// ���񂵂�(�����܂ŗ��邱�Ƃ͖����Ǝv�����B�B�B�j
			break;
		}
		int nSpm = 0;
		switch (mtbl_pPoint->pnt[nWk].emDispStatus) {
		case DIV_DSP_OK:
		case DIV_DSP_STANDBY:
			for (int nStage=0; nStage<MAP_STAGE_NUM; nStage++) {
				// �����I���s���擾
				int nRowLast = mtbl_pPoint->pnt[nWk].typ.nRowLast[nStage];
				if (0 == nRowLast) {
					// ���������O�Ȃ̂ŁA�R�C�����̏o���R�C��������I�[���Z�o����B
					COIL_BASEDATA &pWk = mtbl_pCoilInf->rec[nWk].coilinf.base;
					nRowLast = GetStageLenToIndex(nStage, pWk.nLenOut);
				} else {
					nRowLast -= 1;	// �����I���s��1�I���W���̂��߁A1�������Ă����B
				}
				pOutLen[nStage] = mtbl_pMapRow->rec[nWk].stage[nStage].men[0].row[nRowLast].typ.nLen_o;
				// �Ώۍs�ŃJ�b�g����Ă�����O�ɂ���B
				if (0 != mtbl_pMapRow->rec[nWk].stage[nStage].men[0].row[nRowLast].typ.nCut) {
					pOutLen[nStage] = 0;
				}
			}
			return;
			break;

		case DIV_DSP_NG:
			// ��(����)�̃��R�[�h��
			ii+=1;
			break;

		case DIV_DSP_INITIALIZE:
		default:
			return;
			break;
		}
	}

	return;
}

//------------------------------------------
// �o�������̍Đݒ�
// int nRec			�o���������Đݒ肷�郌�R�[�h�J�n�ʒu (0�I���W��)
//------------------------------------------
// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
//void DataSetManager::RecalOutLen(const int nRec)
//{

void DataSetManager::RecalOutLen(const int nRec, bool bDefect)
{
// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------

	// �X�V�J�n
	mtbl_pPoint->nRefresh = true;

	int nWriteRec = mtbl_pPoint->nRecWriteBase;
	if (TBL_HY_REC_MAX <= nWriteRec) nWriteRec = 0;

	//// �|�C���^���e�[�u�����`�F�b�N (�������㗬�j
	int		ii = 0;
	while (1) {
		int nWk = GetRecIndex(nRec, ii);

		if (nWriteRec == nWk) {
			// �������݃��R�[�h�ɂȂ�����I��
			break;
		}

		if (ii && (nWk == nRec)) {
			// ���񂵂�(�����܂ŗ��邱�Ƃ͖����Ǝv�����B�B�B�j
			break;
		}

		int	nOutOffset[MAP_STAGE_NUM];
		GetOffsetOutLen(nWk, nOutOffset);		// �o���I�t�Z�b�g�ʒu�擾

		for (int nStage = 0; nStage < MAP_STAGE_NUM; nStage++) {
			for (int nMen = 0; nMen < NUM_MEN; nMen++) {
				int nMaxIndex = GET_JUST_INDEX(COIL_LEN_MAX, mcls_pParam->GetCommonMast().nMapDsp[nStage]);	// �ő僌�R�[�h�ʒu
				int nOutLen = nOutOffset[nStage];	// �o���J�n�͏o���I�t�Z�b�g�ʒu�B
				for (int ii = 0; ii <= nMaxIndex; ii++) {
					DSP_TYP_ROW & pWk = mtbl_pMapRow->rec[nWk].stage[nStage].men[nMen].row[ii].typ;
					// �o�������Z�b�g
					nOutLen += mcls_pParam->GetCommonMast().nMapDsp[nStage];
					pWk.nLen_o = nOutLen;
					if (pWk.nCut) {
						// �V���[�J�b�g���ꂽ�s�Ȃ̂ŁA���̍s����o�����������Z�b�g�B
						nOutLen = 0;
					}
				}
			}
		}

// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>	
		if (true == bDefect)
		{
			// �r���̍Đݒ� 
			for (int nDefIndex = 0; nDefIndex < mtbl_pDefectInf->rec[nWk].num; nDefIndex++)
			{			
				GetDeliveryPosY(nWk, mtbl_pDefectInf->rec[nWk].pnt[nDefIndex].typ.data.y, &mtbl_pDefectInf->rec[nWk].pnt[nDefIndex].typ.data.y_out);
			}

			// �O�H���r���̍Đݒ�
			for (int nMen = 0; nMen < NUM_MEN; nMen++ )
			{
				for (int nDefIndex = 0; nDefIndex < mtbl_pMaeInf->rec[nWk].men[nMen].num; nDefIndex++)
				{
					GetDeliveryPosY(nWk, mtbl_pMaeInf->rec[nWk].men[nMen].maeinf[nDefIndex].ff.nY, &mtbl_pMaeInf->rec[nWk].men[nMen].maeinf[nDefIndex].ff.nY_Out);
				}
			}			
		}
// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------

		// ��(�㗬)�̃��R�[�h��
		ii -= 1;
	}

	// �X�V�I��
	mtbl_pPoint->nRefresh = false;
	return;
}

// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
/// <summary>
/// �r���̏o���ʒu���擾
/// </summary>
/// <param name="nRec">�Y���R�C���̃��R�[�h�ʒu (0�I���W��)</param>
/// <param name="dPosY">�����ʒu</param>
/// <param name="pdDeliveryPosY">�Z�o�����o���ʒu</param>
void DataSetManager::GetDeliveryPosY(const int nRec, float dPosY, float* pdDeliveryPosY)
{
	// �����l
	*pdDeliveryPosY = dPosY;

	// ���R�C������㗬�ɖ߂�A���߂̃J�b�g�ʒu����o���������Z�o����B	

	// ���R�C��
	for (int ii = 0; ii < mtbl_pCoilResult->rec[nRec].nCutCnt; ii++)
	{
		int nCutIndex = mtbl_pCoilResult->rec[nRec].nCutCnt - ii - 1;
		if (DIV_CUT_FCUT == mtbl_pCoilResult->rec[nRec].cut[nCutIndex].nMode)
		{
			bool bCut = false;
			int nCutPos = mtbl_pCoilResult->rec[nRec].cut[nCutIndex].nPos;
			if (dPosY >= nCutPos)
			{
				*pdDeliveryPosY = dPosY - nCutPos;				
				bCut = true;
			}

			if ( true == bCut )
			{
				return;
			}
		}
	}

	// �㗬
	for (int ii = 1; ii < TBL_HY_REC_MAX; ii++)
	{
		int nTargetRec = GetRecIndex(nRec, ii);
		if (DIV_DSP_OK == mtbl_pPoint->pnt[nTargetRec].emDispStatus || 
			DIV_DSP_STANDBY == mtbl_pPoint->pnt[nTargetRec].emDispStatus )
		{

			// �R�C�����̎擾
			int nCoilLen = 0;
			if (0 == mtbl_pCoilResult->rec[nTargetRec].nLen)
			{
				// �������������ꍇ�A�ʔ�������Z�o����B���ʏ�A�R�C���ؑւ����Ă��邩����������L�锤�B
				int nWk = 0;
				for (int jj = 0; jj < MAX_DISP_POS; jj++)
				{
					if (nWk < mtbl_pPoint->pnt[nTargetRec].typ.nLenDisp[jj])
					{
						nWk = mtbl_pPoint->pnt[nTargetRec].typ.nLenDisp[jj];
					}
				}
				nCoilLen = nWk * 1000;		// ��[m]->[mm]
			}
			else
			{
				nCoilLen = mtbl_pCoilResult->rec[nTargetRec].nLen;
			}

			// �Y���R�C���ɃJ�b�g�͗L�邩�H
			int nTargetCutPos = 0;
			if ( 0 < mtbl_pCoilResult->rec[nTargetRec].nCutCnt )
			{
				int nCutIndex = mtbl_pCoilResult->rec[nTargetRec].nCutCnt - 1;
				if (DIV_CUT_FCUT == mtbl_pCoilResult->rec[nTargetRec].cut[nCutIndex].nMode )
				{
					nTargetCutPos = mtbl_pCoilResult->rec[nTargetRec].cut[nCutIndex].nPos;
				}
			}

			if (0 < nTargetCutPos)
			{
				// �J�b�g�L�Ȃ獷�������Z
				*pdDeliveryPosY = *pdDeliveryPosY + (nCoilLen - nTargetCutPos < 0 ? 0 : nCoilLen - nTargetCutPos);
				return;
			}
			else
			{
				// �J�b�g���Ȃ�R�C���������Z
				*pdDeliveryPosY = *pdDeliveryPosY + nCoilLen;
			}
		}
	}

	// �����܂ŗ�����㗬�R�C�����J�b�g�������̂œ��������ɂ���B
	*pdDeliveryPosY = dPosY;
}

/// <summary>
/// �O�H���r��� �o���ʒu�ҏW 
/// </summary>
/// <param name="pQue"></param>
void  DataSetManager::EditMaeDefectOutLen(TYP_DATASET_REQMAE* pQue)
{
	int nRec = pQue->nRec;

	LOG(em_MSG), "[%s] �O�H���r��� �o���ʒu�ҏW Rec=%d num=%d/%d",
		my_sThreadName, nRec, mtbl_pMaeInf->rec[nRec].men[0].num, mtbl_pMaeInf->rec[nRec].men[1].num);

	for ( int nMen = 0; nMen < NUM_MEN; nMen++ )
	{
		for ( int nDefIdx = 0; nDefIdx < mtbl_pMaeInf->rec[nRec].men[nMen].num; nDefIdx++ ) 			
		{
			DSP_TYP_MAE_FFINF* pDef = &mtbl_pMaeInf->rec[nRec].men[nMen].maeinf[nDefIdx].ff;

			// �o�������Z�b�g
			GetDeliveryPosY(nRec, pDef->nY, &pDef->nY_Out);
		}
	}
}
// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------

// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
/// <summary>
/// ���������ҏW
/// </summary>
/// <param name="pQue"></param>
void DataSetManager::EditLongPdInf(TYP_DATASET_REQLONGPD* pQue)
{
	LOG(em_INF), "[%s][�����������ʒm] (�Ǘ�No=%s)", my_sThreadName, pQue->cKanriNo);

	//================================================
	// ����������

	// �R�C�����R�[�h�̎擾
	int nRec = CheckKanriNo(pQue->cKanriNo, mtbl_pPoint);
	if( 0 > nRec) {
		LOG(em_ERR), "[%s][�����������ʒm] �Y�����R�[�h����", my_sThreadName);
		return;
	}

	mtbl_pLongPd->rec[nRec].nLongPdOccurrenceFlag = 1;
	LOG(em_INF), "[%s][�����������ʒm] (rec=%d) ���������� ", my_sThreadName, nRec);

	// �K�C�_���X����
	SetLongPdGuidance();
}

/// <summary>
/// �������K�C�_���X�Z�b�g
/// </summary>
void DataSetManager::SetLongPdGuidance()
{
	if( 0 == mcls_pParam->GetCommonMast_LongPd().nLongPd_GuidanceAvailable ) 		
	{
		// �K�C�_���X�Ȃ�
		mtbl_pLongPd->nGuidanceFlag = 0;
		return;
	}

	if( 0 < mtbl_pStatus->status.nCnt ) 		
	{
		// ��������ُ̈킪�L��Ȃ�K�C�_���X�\���Ȃ�
		mtbl_pLongPd->nGuidanceFlag = 0;
		return;
	}

	int nRec = mtbl_pPoint->nRecDispBase;
	if( 0 >= nRec ) 		
	{
		// �\���R�C������
		mtbl_pLongPd->nGuidanceFlag = 0;
		return;
	}

	bool bExist = false;
	for( int ii = 0; ii < 2; ii++ ) 		
	{
		int nCheckRec = GetRecIndex(nRec - 1, ii);

		if( 1 == mtbl_pLongPd->rec[nCheckRec].nLongPdOccurrenceFlag ) 		
		{
			// ���������ׂ���
			bExist = true;
			break;
		}
	}

	if  ( true == bExist )
	{
		// �K�C�_���X�\��
		mtbl_pLongPd->nGuidanceFlag = 1;
	}
	else 		
	{
		mtbl_pLongPd->nGuidanceFlag = 0;
	}
}
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

//------------------------------------------
// �s�ʒu�̃X�e�[�^�X(�ғ����/�������/�@����)���w��s�܂Ŕ��f
// int nRec     �e�[�u�����R�[�h�ʒu (0�I���W��)
// int nStage   �s�X�e�[�W (0:�ŏ�/1:��/2:��)
// int nMen	    �� (0:�\/1:��)
// int nBaseRow �J�n�s (0�I���W��)
// int nBaseRow �I���s (0�I���W��)
// �߂�l -1:�Z�o�G���[  0�`:�o���R�C����[����̋��� [mm]
//------------------------------------------
void DataSetManager::RefRowStatus(const int nRec, const int nStage, const int nMen, const int nBaseRow, const int nEndRow)
{
	// ���ݏ������ݍs�̍s���i�ғ����/�������/�@����)���R�s�[
	for (int nRow=nBaseRow+1; nRow<=nEndRow; nRow++) {
		DSP_TYP_ROW& pSrc = mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRow-1].typ;
		DSP_TYP_ROW& pTag = mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRow].typ;
		if(pTag.emKadou < pSrc.emKadou)	pTag.emKadou = pSrc.emKadou;					// �ғ����
		if(pTag.emDspKensa < pSrc.emDspKensa)	pTag.emDspKensa = pSrc.emDspKensa;		// �������
		if(pTag.emDspKiki  < pSrc.emDspKiki)	pTag.emDspKiki = pSrc.emDspKiki;		// �@����

	}

	return;
}


