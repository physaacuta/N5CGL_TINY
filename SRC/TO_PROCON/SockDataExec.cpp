#include "StdAfx.h"
#include "SockDataExec.h"

//#define tDEBUG

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
SockDataExec::SockDataExec(void) :
mque_AddDB(64)
{

///*/----> ���I�����C�����͕K���R�����g�����邱��
	m_hCoil = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(TO_COIL_TBL), TBL_TO_COIL_NAME);
	mtbl_pCoil =(TO_COIL_TBL*) MapViewOfFile(m_hCoil, FILE_MAP_ALL_ACCESS, 0, 0, 0);
//*///<<---

#ifndef tDEBUG			// �f�o�b�N���́A������
	// �R�C�����e�[�u��
	mem_connect(&m_hCoil, (LPVOID *)&mtbl_pCoil, TBL_TO_COIL_NAME);
	if(m_hCoil == NULL) {
		syslog(SYSNO_MEM_OPEN, "�R�C�����e�[�u�� �A�N�Z�X���s");
		_ASSERT(FALSE);
	}
	memset(mtbl_pCoil, 0x00, sizeof(TO_COIL_TBL));

#endif


}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
SockDataExec::~SockDataExec(void)
{
#ifndef tDEBUG			// �f�o�b�N���́A������
	//// ���L�������J��
	mem_close(&m_hCoil, (LPVOID *)&mtbl_pCoil);							// �R�C�����
#endif


}

//------------------------------------------
// �֐�������
//------------------------------------------
void SockDataExec::Alloc()
{
#ifndef tDEBUG			// �f�o�b�N���́A������
	//// DB�o�^�N���X����
	mcls_pDbs = new DBSaveManager(0, &mque_AddDB);
	mcls_pDbs->SetLogMgr(mcls_pLog);
	mcls_pDbs->SetSendDbManager(false);
	mcls_pDbs->Start();
#endif
}

//------------------------------------------
// �֐��J��
//------------------------------------------
void SockDataExec::Free()
{
#ifndef tDEBUG			// �f�o�b�N���́A������
	//// DB�o�^�N���X���
	mcls_pDbs->Stop(3000);
	delete mcls_pDbs;
#endif
}

//------------------------------------------
// ���M�f�[�^�擾 (���̒��ŁA�o�C�g���]�����Ⴄ��)
// Delivery* deli �v�����
// int* nTc �g�����U�N�V�����R�[�h
// int* nSize ���M�f�[�^��
// BYTE* data ���M�f�[�^
//------------------------------------------
bool SockDataExec::AddQue_Ziseki(COMMON_QUE const* pQue)
{
	// ���M�v�� �L���[�ɓo�^
	Delivery* deli = new Delivery;
	deli->pData = (BYTE*) new PRO_TYPE_ZISEKI;
	deli->eStat = QUE_ZISEKI_SEND;
	strcpy(deli->cKey, pQue->fc.data);

	CString		cSql;

	//// ���ѓ`������
	PRO_TYPE_ZISEKI* buf = (PRO_TYPE_ZISEKI*)deli->pData;

	//// DB������уf�[�^���擾
#ifndef tDEBUG											//// �{�ԃf�[�^
	// SQL���쐬
	cSql =  "SELECT ���`�� ";
	cSql += " FROM ";
	cSql += DB_BUF_PRO_S_ZISEKI;
	cSql += " WHERE ID=";
	CreateSQL::sqlColumnInsert(&cSql, deli->cKey, sizeof(deli->cKey));		// ID
	CreateSQL::sqlColumnTrim(&cSql);
	// �擾
	if( ! OdbcBase::GetSelectImage_Static(cSql, sizeof(PRO_TYPE_ZISEKI), (char*)buf, TASKINI_NAME, INI_DB ) ) {
		// �擾���s
		LOG(em_ERR), "[%s] �������ѓ`�� DB�擾���s [ID=%s]", my_sThreadName, deli->cKey);
		LOG(em_NON), "%s", cSql);
		SAFE_DELETE(deli->pData);
		SAFE_DELETE(deli);
		return false;
	}
#else													//// �e�X�g�p�_�~�[�f�[�^
	memset(buf, 0x20, sizeof(PRO_TYPE_ZISEKI));
	memset(buf->data.grade_inf, 0x00, sizeof(buf->data.grade_inf) );

	buf->head.bHead[0] = gcPRO_HEAD_H_GYOUMU;
	buf->head.bHead[1] = gcPRO_HEAD_L_OK;
	buf->head.nLen = PRO_TYPE_LENGTH(sizeof(PRO_TYPE_ZISEKI));

	buf->head.nTC = PRO_TC_ZISEKI;
	buf->head.nKubun = PRO_KUBUN_NOMAL;
	buf->head.nDate[0] = 2012;
	buf->head.nDate[1] = 3;
	buf->head.nDate[2] = 23;
	buf->head.nDate[3] = 9;
	buf->head.nDate[4] = 30;
	buf->head.nDate[5] = 59;
	buf->head.nGrandFlg = 0;
	buf->data.nZiMode = 0;
	buf->data.nKikiMode = 0;
	//memcpy(buf->data.cMeiNo, "0123", 4);
	//memcpy(buf->data.cCoilNo, "1-2345      ", 12);
	//buf->data.nKoban = 1;
		
	memcpy(buf->data.cMeiNo, &mtyp_Cut.data.cMeiNo[0], sizeof(buf->data.cMeiNo));		// ����No
	memcpy(buf->data.cCoilNo, &mtyp_Cut.data.cCoilNo[0], sizeof(buf->data.cCoilNo));	// �R�C��No
	buf->data.nKoban = mtyp_Cut.data.nKoban;									// �q��

	for(int ii=0; ii<6; ii++ ) {
		buf->data.grade_inf[ii].nMixedRate = (ii+1)*100; 
		buf->data.grade_inf[ii].nTypeCnt[0] = 1;
		buf->data.grade_inf[ii].nTypeCnt[1] = 2; 
		buf->data.grade_inf[ii].nTypeCnt[2] = 3; 
		buf->data.grade_inf[ii].nTypeCnt[3] = 4; 
		buf->data.grade_inf[ii].nTypeCnt[4] = 5; 
		buf->data.grade_inf[ii].nTypeCnt[5] = 6; 
		buf->data.grade_inf[ii].nTypeCnt[6] = 7; 
		buf->data.grade_inf[ii].nTypeCnt[7] = 8; 
		buf->data.grade_inf[ii].nTypeCnt[8] = 9; 
		buf->data.grade_inf[ii].nTypeCnt[9] = 10;
	}
#endif

	//// ���M�f�[�^ �ҏW
	//*size = buf->head.nLen;
	deli->nSize = sizeof(PRO_TYPE_ZISEKI);
	deli->nTc	= buf->head.nTC;

	CString		cDumpName;
#ifdef BYTE_HANTEN
	// ���f�[�^�_���v
	cDumpName.Format("%s ���M(��)�_���v %s(%s, EvtNo=%d).log", TO_PROCON, "[���M�|�[�g]", "�Ɩ��`��", deli->nTc);
	KizuFunction::DumpData( cDumpName, deli->nSize, (BYTE *)buf, m_bDumpMode);

	// ------------------------------------
	// �v���R�����M�`�� �o�C�g���]
	// ------------------------------------
	ByteHantenProDataSend(deli->nTc, (BYTE *)buf);
#endif

	// �σf�[�^�_���v
	cDumpName.Format("%s ���M(��)�_���v %s(%s, EvtNo=%d).log", TO_PROCON, "[���M�|�[�g]", "�Ɩ��`��", deli->nTc);
	KizuFunction::DumpData(cDumpName, deli->nSize, (BYTE *)buf, m_bDumpMode);

	//================================================
	// ���M
	//	LOG(em_MSG), "[%s] �������ѓ`�����M�˗�", my_sThreadName);
	//// ���M�˗�
	if (!mque_pDeli->AddToTailFreeCheck(deli, 0)) {
		LOG(em_ERR), "[%s] ���M�˗��ُ�L���[�t��", my_sThreadName);
		syslog(SYSNO_QUEFULL_WAR, "[���M�˗�]");
		SAFE_DELETE(deli->pData);
		SAFE_DELETE(deli);
		return false;
	}
	return true;
}
//------------------------------------------
// ���ё��M����
// Delivery* deli �v�����
// bool bAns ���M����
//------------------------------------------
bool SockDataExec::SetSendDataAnser(Delivery* deli, bool bAns)
{
#ifdef tDEBUG			// �f�o�b�N���́A������
	return true;
#endif

	int imode = bAns == true ? 0 : -1;


	// SQL������
	CString		sql;
	sql = "UPDATE ";
	sql += DB_BUF_PRO_S_ZISEKI;
	sql += " SET ";
	CreateSQL::sqlColumnUpdate(&sql, "�`���敪",imode); 
	sql += "���M���t=";	CreateSQL::sqlColumnAddTime(&sql); 
	sql += " WHERE ID=";
	CreateSQL::sqlColumnInsert(&sql, deli->cKey, sizeof(deli->cKey));		// ID
	CreateSQL::sqlColumnTrim(&sql);

	//// ��������SQL���� DB�o�^�X���b�h�ɓo�^�˗�����
	DBSaveManager::DeliverySQL*  pDeli;
	pDeli = new DBSaveManager::DeliverySQL();
	pDeli->addmode	= true;
	pDeli->strSql	= sql;
	pDeli->cnt		= 0;
	if( ! mque_AddDB.AddToTail(pDeli, 20, 20) ) {		// ��Γo�^
		// ��Γo�^�������E�E�E
		LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [%s]", my_sThreadName, DB_BUF_PRO_S_ZISEKI);
		syslog(SYSNO_QUEFULL_ERR, "[%s]", DB_BUF_PRO_S_ZISEKI);
		SAFE_DELETE( pDeli );
	}
	else
	{
		LOG(em_MSG), "[%s] ���ё��M���� DB�o�^����", my_sThreadName);
	}
	return true;
}	

//------------------------------------------
// ��M�f�[�^ ���㏈��
// int nTc �g�����U�N�V�����R�[�h
// int nSize ��M�f�[�^��
// BYTE* data ��M�f�[�^
//------------------------------------------
bool SockDataExec::SetRecvData(int nTc, int nSize, BYTE * data)
{
	COMMON_QUE		que;
	bool bRetc = true;


	memset(&que, 0x00, sizeof(que));
	que.nLineNo = getlineid();
	
	CString			sql;
	char			cKey[SIZE_SERIAL_FF_NO];		// �~���b�P�ʂ̎�L�[
	KizuFunction::GetSerialNoFF(cKey); 

	// ------------------------------------
	// �v���R����M�`�� �o�C�g���]
	// ------------------------------------
#ifdef BYTE_HANTEN
	ByteHantenProDataRecv(nTc, data);
#endif

	//// �e�`������
	switch(nTc) {
	//---------------------------------------------------------
	case PRO_TC_INIT:								// �C�j�V�����`��
		if(true) {
			//// �`������
			PRO_TYPE_INIT* buf = (PRO_TYPE_INIT*)data;

			syslog(329, "");

			// �����Ǘ��֒ʒm
			que.nEventNo = FACT_TO_DEFECT_15;
			send_mail(TO_DEFECT, ".", &que);
		}
		break;

	//---------------------------------------------------------
	case PRO_TC_COIL:								// �R�C�����
		if(true) {
			//// �`������
			PRO_TYPE_COILINF* buf = (PRO_TYPE_COILINF*)data;

			int nAlmNo = (1 == buf->data.CoilInf.nSendPoint) ? 325 : 326;
			syslog(nAlmNo, "[%s] [%.12s] ���M�ʒu=%d", cKey, buf->data.CoilInf.KeyInf.cCoilNo, buf->data.CoilInf.nSendPoint);
			LOG(em_MSG), "[%s] [%s] [%.12s] ���M�ʒu=%d", my_sThreadName, cKey, buf->data.CoilInf.KeyInf.cCoilNo, buf->data.CoilInf.nSendPoint);

#ifndef tDEBUG			// �f�o�b�N���́A������
			//// ------------------------------
			//// ���M�ʒu         TBL_TO_COIL
			////  1 : �����ʒu ��  Record : 0
			////  2 : �\���ʒu ��  Record : 1
			//// ------------------------------
			//// ���L�������ɏ�������
			int nRec = buf->data.CoilInf.nSendPoint - 1;
			if(nRec < 0 || nRec >= 2 ) {
				LOG(em_ERR), "[%s] �R�C�����`�� ���M�ʒu�ُ� [%d]", my_sThreadName, nRec);
				return false;
			}
			memset( &mtbl_pCoil->rec[nRec], 0x00, sizeof(mtbl_pCoil->rec[nRec]));
			memcpy( mtbl_pCoil->rec[nRec].val, buf, nSize);
#endif

			//// �����Ǘ��֒ʒm
			que.nEventNo = FACT_TO_GETDAT_02;
			que.fl.data[0] = nRec;				// ��M�ʒu(���L�������i�[�ʒu)
			send_mail(TO_GETDAT, ".", &que);

			// BUF�e�[�u���o�^
			InsertBufProRData(nTc, data);

		}
		break;


	//---------------------------------------------------------
	case PRO_TC_SAISUN:								// �̐�����
		if(true) {
			//// �`������
			PRO_TYPE_SAISUN* buf = (PRO_TYPE_SAISUN*)data;

			syslog(327, "[%s] [%.12s]", cKey, buf->data.KeyInf.cCoilNo);

			// BUF�e�[�u���o�^
			InsertBufProRData(nTc, data);
		}
		break;

	//---------------------------------------------------------
	case PRO_TC_CUT:								// �J�b�g���ѓ`��
		if(true) {
			//// �`������
			PRO_TYPE_CUT* buf = (PRO_TYPE_CUT*)data;

			syslog(328, "[%s] [%.12s]", cKey, buf->data.KeyInf.cCoilNo);

			// BUF�e�[�u���o�^
			InsertBufProRData(nTc, data);

#ifdef tDEBUG			// �f�o�b�N���̂�
			
			// �J�b�g���ۑ�
			memset(&mtyp_Cut, 0x00, sizeof(PRO_TYPE_CUT));
			memcpy(&mtyp_Cut, buf, sizeof(mtyp_Cut));

			Sleep(10000);
			memset(&que, 0x00, sizeof(que));
			que.nEventNo = FACT_TO_PROCON_01;
			que.nLineNo = getlineid();
			send_mail(TO_PROCON, ".", &que);
			
#endif

		}
		break;
	//---------------------------------------------------------
	case PRO_TC_HELS:								// �w���V�[�`��
		if (true) {
			// �w���V�[�`���Ď� �J�n
			SetHelcy();
		}
		break;

	//---------------------------------------------------------
	default:
		LOG(em_ERR), "[%s] TC�ُ� [%d]", my_sThreadName, nTc);
		syslog(324, "[��M�`��:TC�ُ�=%d]", nTc);
		bRetc = false;
		break;
	}


	// �σf�[�^�_���v
	CString		cDumpName;
	cDumpName.Format("%s ��M(��)�_���v %s(%s, EvtNo=%d).log", TO_PROCON, "[��M�|�[�g]", "�Ɩ��`��", nTc );
	KizuFunction::DumpData(cDumpName, nSize, data, m_bDumpMode);

	return bRetc;
}

//------------------------------------------
// �v���R����M�`�� �o�C�g���]
// int nTc �g�����U�N�V�����R�[�h
// BYTE* data ��M�f�[�^
//------------------------------------------
void SockDataExec::ByteHantenProDataRecv(int nTc, BYTE * data)
{
#ifdef BYTE_HANTEN

	// �w�b�_�[��
	PRO_TYPE_HEAD *pHead = (PRO_TYPE_HEAD *)data;
	KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pHead->nLen);
	KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pHead->nTC);

	switch (nTc)
	{
		//-----------------------------------------------------------
	case PRO_TC_COIL:				// �R�C�����`��
		if (true)
		{
			// �f�[�^��
			PRO_TYPE_COILINF *pBuf = (PRO_TYPE_COILINF *)data;
			PRO_TYPE_DATA_COILINF *pData = (PRO_TYPE_DATA_COILINF *)&pBuf->data;
			KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pData->Common.nKubun);
			KizuFunction::ByteHanten2(2 * 6, (BYTE*)&pData->Common.nDate);
			KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pData->Common.nGrandFlg);

			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->CoilInf.nSendPoint);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->CoilInf.nLenIn);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->CoilInf.nAtuZai);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->CoilInf.nWidZai);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->CoilInf.nAtuSei);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->CoilInf.nWidSei);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->CoilInf.nWidTrim);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->CoilInf.nNobiSPM);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->CoilInf.nNobiTLV);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->CoilInf.nCutF);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->CoilInf.nCutT);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->CoilInf.nSPMRoolKei_Up);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->CoilInf.nSPMRoolKei_Down);
			KizuFunction::ByteHanten4(4 * 31, (BYTE*)&pData->CoilInf.nYobi);
			for (int ii = 0; ii < PRO_SIZE_NEXTINF_CNT; ii++)
			{
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->next_coil[ii].nFrontLen);
			}
		}
		break;

		//-----------------------------------------------------------
	case PRO_TC_SAISUN:				// �̐����ѓ`��
		if (true)
		{
			// �f�[�^��
			PRO_TYPE_SAISUN *pBuf = (PRO_TYPE_SAISUN *)data;
			PRO_TYPE_DATA_SAISUN *pData = (PRO_TYPE_DATA_SAISUN *)&pBuf->data;
			KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pData->Common.nKubun);
			KizuFunction::ByteHanten2(2 * 6, (BYTE*)&pData->Common.nDate);
			KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pData->Common.nGrandFlg);

			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->nLen);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->nMakiLenF);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->nSetCnt);
			KizuFunction::ByteHanten4(4 * 9, (BYTE*)&pData->nYobi);
			for (int ii = 0; ii < PRO_MAX_CUT_COUNT; ii++)
			{
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->saisun[ii].nStart);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->saisun[ii].nEnd);
			}
		}
		break;

		//-----------------------------------------------------------
	case PRO_TC_CUT:				// �J�b�g���ѓ`��
		if (true)
		{
			// �f�[�^��
			PRO_TYPE_CUT *pBuf = (PRO_TYPE_CUT *)data;
			PRO_TYPE_DATA_CUT *pData = (PRO_TYPE_DATA_CUT *)&pBuf->data;
			KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pData->Common.nKubun);
			KizuFunction::ByteHanten2(2 * 6, (BYTE*)&pData->Common.nDate);
			KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pData->Common.nGrandFlg);

			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->nMode);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->nStart);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->nEnd);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->nKoban);
		}
		break;

		//-----------------------------------------------------------
	case PRO_TC_INIT:				// �C�j�V�����`��
		// �f�[�^��
		PRO_TYPE_INIT *pBuf = (PRO_TYPE_INIT *)data;
		PRO_TYPE_DATA_INIT *pData = (PRO_TYPE_DATA_INIT *)&pBuf->data;
		KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pData->Common.nKubun);
		KizuFunction::ByteHanten2(2 * 6, (BYTE*)&pData->Common.nDate);
		KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pData->Common.nGrandFlg);
		break;

		//-----------------------------------------------------------
	default:
		// ���肦�Ȃ��I�I
		break;
	}
#endif
}

//------------------------------------------
// �v���R�����M�`��(�V�[�P���T�f�[�^)�o�C�g���]
// int nTc �g�����U�N�V�����R�[�h
// BYTE* data ��M�f�[�^
//------------------------------------------
void SockDataExec::ByteHantenProDataSend(int nTc, BYTE * data)
{
#ifdef BYTE_HANTEN


	// �w�b�_�[��
	PRO_TYPE_HEAD *pHead = (PRO_TYPE_HEAD *)data;
	KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pHead->nLen);
	KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pHead->nTC);

	switch (nTc)
	{
		//-----------------------------------------------------------
	case PRO_TC_ZISEKI:								// �r�������ѓ`��
		if (true)
		{
			// �f�[�^��
			PRO_TYPE_ZISEKI *pBuf = (PRO_TYPE_ZISEKI *)data;
			PRO_TYPE_DATA_ZISEKI *pData = (PRO_TYPE_DATA_ZISEKI *)&pBuf->data;
			KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pData->Common.nKubun);
			KizuFunction::ByteHanten2(2 * 6, (BYTE*)&pData->Common.nDate);
			KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pData->Common.nGrandFlg);

			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->nZiMode);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->nKikiMode);
			KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pData->nKoban);
			for (int ii = 0; ii < 6; ii++)
			{
				KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pData->grade_inf[ii].nMixedRate);
				KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pData->grade_inf[ii].nYobi1);
				KizuFunction::ByteHanten2(2 * 10, (BYTE*)&pData->grade_inf[ii].nTypeCnt);
				KizuFunction::ByteHanten2(2 * 2, (BYTE*)&pData->grade_inf[ii].nYobi2);
			}
		}
		break;

		//-----------------------------------------------------------
	default:
		// ���肦�Ȃ��I�I
		break;
	}
#endif
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���̑�


//------------------------------------------
// ��M�`����BUF�e�[�u���o�^
// int nTc �g�����U�N�V�����R�[�h
// BYTE* data ��M�f�[�^
// �߂�l �o�^����
//------------------------------------------
bool SockDataExec::InsertBufProRData(int nTc, BYTE * data)
{
	CString		sql;						// SQL��
	char		cKey[SIZE_SERIAL_FF_NO];	// ID
	KizuFunction::GetSerialNoFF(cKey);

	switch (nTc)
	{
		//-----------------------------------------------------------
	case PRO_TC_COIL:											// �R�C�����`��
		if (true)
		{
			PRO_TYPE_COILINF *pCoil = (PRO_TYPE_COILINF *)data;

			// SQL�쐬
			sql = "INSERT INTO ";
			sql += DB_BUF_PRO_R_COILINF;
			sql += " VALUES (";
			CreateSQL::sqlColumnInsert(&sql, cKey, sizeof(cKey));													// ID
			CreateSQL::sqlColumnAddTime(&sql, true);																// �o�^���t
			CreateSQL::sqlColumnInsert(&sql, (const char *)&pCoil->data.CoilInf.KeyInf, sizeof(pCoil->data.CoilInf.KeyInf));	// �L�[���
			CreateSQL::sqlColumnInsert(&sql, pCoil->data.CoilInf.nSendPoint);										// ���M�ʒu
			sql += "?";																								// ���`��
			sql += ")";

			// ��������SQL���� DB�o�^�X���b�h �ɓo�^�˗�
			DBSaveManager::DeliverySQL *pDeli = new DBSaveManager::DeliverySQL();
			pDeli->addmode = true;
			pDeli->strSql = sql;
			pDeli->cnt = 1;
			pDeli->size[0] = sizeof(PRO_TYPE_COILINF);
			pDeli->pImg[0] = new BYTE[pDeli->size[0]];
			memcpy(pDeli->pImg[0], pCoil, pDeli->size[0]);

			// DB�X�V�������� �R�C������M�ʒm
			// �� ���L�������i�[���_�ő��M�ς�

			// DB�o�^�˗�
			if (!mque_AddDB.AddToTail(pDeli, 20, 20))
			{
				// ��Γo�^�������E�E�E
				LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [%s]", my_sThreadName, DB_BUF_PRO_R_COILINF);
				syslog(SYSNO_QUEFULL_ERR, "[%s]", DB_BUF_PRO_R_COILINF);
				SAFE_DELETE(pDeli->pImg[0]);
				SAFE_DELETE(pDeli->SendMailItem);
				SAFE_DELETE(pDeli);
			}
			else
			{
				LOG(em_MSG), "[%s] �R�C�����`�� DB�o�^�˗�����", my_sThreadName);
				Sleep(100);		// �����p�ƕ\���p�̃R�C�����𓯎���M�����ꍇ�AID�������BUF�e�[�u���o�^�G���[�ƂȂ邽�߂̑΍�
			}
		}
		break;

		//-----------------------------------------------------------
	case PRO_TC_SAISUN:									// �̐����ѓ`��
		if (true)
		{
			PRO_TYPE_SAISUN *pSaisun = (PRO_TYPE_SAISUN *)data;

			// SQL�쐬
			sql = "INSERT INTO ";
			sql += DB_BUF_PRO_R_SAISUN;
			sql += " VALUES (";
			CreateSQL::sqlColumnInsert(&sql, cKey, sizeof(cKey));													// ID
			CreateSQL::sqlColumnAddTime(&sql, true);																// �o�^���t
			CreateSQL::sqlColumnInsert(&sql, (const char *)&pSaisun->data.KeyInf, sizeof(pSaisun->data.KeyInf));	// �L�[���
			sql += "?,";																							// ���`��
			sql += "NULL,";																							// �Ǘ�No
			sql += "NULL";																							// �ҏW���t
			sql += ")";

			// ��������SQL���� DB�o�^�X���b�h �ɓo�^�˗�
			DBSaveManager::DeliverySQL *pDeli = new DBSaveManager::DeliverySQL();
			pDeli->addmode = true;
			pDeli->strSql = sql;
			pDeli->cnt = 1;
			pDeli->size[0] = sizeof(PRO_TYPE_SAISUN);
			pDeli->pImg[0] = new BYTE[pDeli->size[0]];
			memcpy(pDeli->pImg[0], pSaisun, pDeli->size[0]);

			// DB�X�V�������� �v���R���̐����ю�M�ʒm
			pDeli->SendMailItem = new MAIL_ITEM;
			memset(pDeli->SendMailItem, 0x00, sizeof(MAIL_ITEM));
			strcpy(pDeli->SendMailItem->cPc, ".");
			strcpy(pDeli->SendMailItem->cTask, TO_PUTDAT);
			pDeli->SendMailItem->Que.nEventNo = FACT_TO_PUTDAT_02;
			pDeli->SendMailItem->Que.nLineNo = getlineid();
			memcpy(pDeli->SendMailItem->Que.fc.data, cKey, sizeof(cKey));

			// DB�o�^�˗�
			if (!mque_AddDB.AddToTail(pDeli, 20, 20))
			{
				// ��Γo�^�������E�E�E
				LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [%s]", my_sThreadName, DB_BUF_PRO_R_SAISUN);
				syslog(SYSNO_QUEFULL_ERR, "[%s]", DB_BUF_PRO_R_SAISUN);
				SAFE_DELETE(pDeli->pImg[0]);
				SAFE_DELETE(pDeli->SendMailItem);
				SAFE_DELETE(pDeli);
			}
			else
			{
				LOG(em_MSG), "[%s] �̐����ѓ`�� DB�o�^�˗�����", my_sThreadName);
			}
		}
		break;

		//-----------------------------------------------------------
	case PRO_TC_CUT:									// �J�b�g���ѓ`��
		if (true)
		{
			PRO_TYPE_CUT *pCut = (PRO_TYPE_CUT *)data;

			// SQL�쐬
			sql = "INSERT INTO ";
			sql += DB_BUF_PRO_R_CUT;
			sql += " VALUES (";
			CreateSQL::sqlColumnInsert(&sql, cKey, sizeof(cKey));													// ID
			CreateSQL::sqlColumnAddTime(&sql, true);																// �o�^���t
			CreateSQL::sqlColumnInsert(&sql, (const char *)&pCut->data.KeyInf, sizeof(pCut->data.KeyInf));			// �L�[���
			CreateSQL::sqlColumnInsert(&sql, pCut->data.nStart);													// �J�n�ʒu
			CreateSQL::sqlColumnInsert(&sql, pCut->data.nEnd);														// �I���ʒu
			sql += "?,";																							// ���`��
			sql += "NULL,";																							// �Ǘ�No
			sql += "NULL";																							// �ҏW���t
			sql += ")";

			// ��������SQL���� DB�o�^�X���b�h �ɓo�^�˗�
			DBSaveManager::DeliverySQL *pDeli = new DBSaveManager::DeliverySQL();
			pDeli->addmode = true;
			pDeli->strSql = sql;
			pDeli->cnt = 1;
			pDeli->size[0] = sizeof(PRO_TYPE_CUT);
			pDeli->pImg[0] = new BYTE[pDeli->size[0]];
			memcpy(pDeli->pImg[0], pCut, pDeli->size[0]);

			// DB�X�V�������� �v���R���J�b�g���ю�M�ʒm
			pDeli->SendMailItem = new MAIL_ITEM;
			memset(pDeli->SendMailItem, 0x00, sizeof(MAIL_ITEM));
			strcpy(pDeli->SendMailItem->cPc, ".");
			strcpy(pDeli->SendMailItem->cTask, TO_PUTDAT);
			pDeli->SendMailItem->Que.nEventNo = FACT_TO_PUTDAT_03;
			pDeli->SendMailItem->Que.nLineNo = getlineid();
			memcpy(pDeli->SendMailItem->Que.fc.data, cKey, sizeof(cKey));

			// DB�o�^�˗�
			if (!mque_AddDB.AddToTail(pDeli, 20, 20))
			{
				// ��Γo�^�������E�E�E
				LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [%s]", my_sThreadName, DB_BUF_PRO_R_CUT);
				syslog(SYSNO_QUEFULL_ERR, "[%s]", DB_BUF_PRO_R_CUT);
				SAFE_DELETE(pDeli->pImg[0]);
				SAFE_DELETE(pDeli->SendMailItem);
				SAFE_DELETE(pDeli);
			}
			else
			{
				LOG(em_MSG), "[%s] �J�b�g���ѓ`�� DB�o�^�˗�����", my_sThreadName);
			}
		}
		break;

		//-----------------------------------------------------------
	case PRO_TC_INIT:									// �C�j�V�����`��
																// ���ɖ���
		break;

		//-----------------------------------------------------------
	default:
		// ���肦�Ȃ��͂��I�I
		LOG(em_ERR), "[%s] InsertBufProRData ��M�`����� NG=%d", my_sThreadName, nTc);
		break;
	}

	return true;
}

//------------------------------------------
// �o�b�t�@�t�@�C����Ǎ�
// int nTc				�g�����U�N�V�����R�[�h
// BYTE* data			��M�f�[�^
// char* cFileName		�t�@�C������
// int nMode			�ǂݍ��݃��[�h(0:�o�C�g���]�Ȃ�,1:�o�C�g���]����)
// �߂�l				true:���� false:�Ǎ��ݎ��s
//------------------------------------------
bool SockDataExec::ReadBufFile(int nTc, BYTE * data, char* cFileName, int nMode)
{
	BYTE nRecvBuf[1024];		// �`���o�b�t�@
	unsigned int nSize;			// �`���T�C�Y
	CString sFileName;			// �t�@�C������
	CFile file;

	sFileName.Format("%s", cFileName);

	switch (nTc) {
	case PRO_TC_COIL:
		nSize = sizeof(PRO_TYPE_COILINF);
		break;
	case PRO_TC_SAISUN:
		nSize = sizeof(PRO_TYPE_SAISUN);
		break;
	case PRO_TC_CUT:
		nSize = sizeof(PRO_TYPE_CUT);
		break;
	case PRO_TC_INIT:
		nSize = sizeof(PRO_TYPE_INIT);
		break;
	default:
		LOG(em_ERR), "[%s] TC�ΏۊO[%d]", my_sThreadName, nTc);
		return false;
	}

	// �t�@�C���ǂݍ���
	if (!file.Open(sFileName, CFile::typeBinary | CFile::modeRead | CFile::modeNoTruncate)) {
		LOG(em_ERR), "[%s] %s�ǂݍ��ݎ��s", my_sThreadName, sFileName);
		return false;
	}
	else if (nSize != (unsigned int)file.GetLength()) {
		LOG(em_ERR), "[%s] %s�T�C�Y�ُ�", my_sThreadName, sFileName);
		file.Close();
		return false;
	}
	else {
		nSize = (unsigned int)file.GetLength();
		file.Read(nRecvBuf, nSize);
		LOG(em_MSG), "[%s] %s�ǂݍ��ݐ��� TC:%d Size:%d Mode:%d", my_sThreadName, sFileName, nTc, nSize, nMode);
	}

	// �o�C�g���]����
	if (1 == nMode) {
		PRO_TYPE_HEAD* pHead = (PRO_TYPE_HEAD*)nRecvBuf;
		KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pHead->nLen);
		KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pHead->nTC);
		switch (nTc) {
		case PRO_TC_COIL:
			if (true) {
				PRO_TYPE_COILINF* pCoil = (PRO_TYPE_COILINF*)nRecvBuf;
				KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pCoil->data.Common.nKubun);
				KizuFunction::ByteHanten2(2 * 6, (BYTE*)&pCoil->data.Common.nDate);
				KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pCoil->data.Common.nGrandFlg);

				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCoil->data.CoilInf.nSendPoint);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCoil->data.CoilInf.nLenIn);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCoil->data.CoilInf.nAtuZai);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCoil->data.CoilInf.nWidZai);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCoil->data.CoilInf.nAtuSei);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCoil->data.CoilInf.nWidSei);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCoil->data.CoilInf.nWidTrim);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCoil->data.CoilInf.nNobiSPM);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCoil->data.CoilInf.nNobiTLV);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCoil->data.CoilInf.nCutF);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCoil->data.CoilInf.nCutT);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCoil->data.CoilInf.nSPMRoolKei_Up);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCoil->data.CoilInf.nSPMRoolKei_Down);
				KizuFunction::ByteHanten4(4 * 31, (BYTE*)&pCoil->data.CoilInf.nYobi);
				for (int ii = 0; ii < PRO_SIZE_NEXTINF_CNT; ii++)
				{
					KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCoil->data.next_coil[ii].nFrontLen);
				}
			}
			break;
		case PRO_TC_SAISUN:
			if (true) {
				PRO_TYPE_SAISUN* pSaisun = (PRO_TYPE_SAISUN*)nRecvBuf;
				KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pSaisun->data.Common.nKubun);
				KizuFunction::ByteHanten2(2 * 6, (BYTE*)&pSaisun->data.Common.nDate);
				KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pSaisun->data.Common.nGrandFlg);

				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pSaisun->data.nLen);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pSaisun->data.nMakiLenF);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pSaisun->data.nSetCnt);
				KizuFunction::ByteHanten4(4 * 9, (BYTE*)&pSaisun->data.nYobi);
				for (int ii = 0; ii < PRO_MAX_CUT_COUNT; ii++) {
					KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pSaisun->data.saisun[ii].nStart);
					KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pSaisun->data.saisun[ii].nEnd);
				}
			}
			break;
		case PRO_TC_CUT:
			if (true) {
				PRO_TYPE_CUT* pCut = (PRO_TYPE_CUT*)nRecvBuf;
				KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pCut->data.Common.nKubun);
				KizuFunction::ByteHanten2(2 * 6, (BYTE*)&pCut->data.Common.nDate);
				KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pCut->data.Common.nGrandFlg);

				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCut->data.nMode);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCut->data.nStart);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCut->data.nEnd);
				KizuFunction::ByteHanten4(4 * 1, (BYTE*)&pCut->data.nKoban);
			}
			break;
		case PRO_TC_INIT:
				PRO_TYPE_INIT* pInit = (PRO_TYPE_INIT*)nRecvBuf;

				KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pInit->data.Common.nKubun);
				KizuFunction::ByteHanten2(2 * 6, (BYTE*)&pInit->data.Common.nDate);
				KizuFunction::ByteHanten2(2 * 1, (BYTE*)&pInit->data.Common.nGrandFlg);
			break;
		}
	}

	memcpy(data, nRecvBuf, nSize);

	// �t�@�C���N���[�Y
	file.Close();

	return true;
}
