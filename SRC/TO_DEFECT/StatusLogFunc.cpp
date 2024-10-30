#include "StdAfx.h"
#include "StatusLogFunc.h"

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
StatusLogFunc::StatusLogFunc(void) :
FuncBase("StatFunc")
{
	// ������
	memset( &m_oldStatusLog, 0x00, sizeof(m_oldStatusLog));
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
StatusLogFunc::~StatusLogFunc(void)
{

}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����

//------------------------------------------
// �����ݒ�
//------------------------------------------
void StatusLogFunc::Alloc()
{
	// AutoNo�ɕK�v�ȃf�[�^�������Z�b�g����
	InitAutoNo();
}

//------------------------------------------
// AutoNo�ɕK�v�ȃf�[�^�������Z�b�g����
//------------------------------------------
void StatusLogFunc::InitAutoNo()
{
	//// ���ݓ����Z�b�g
	m_nDay = CTime::GetCurrentTime().GetDay(); 

	//// ODBC�̐ݒ�ʒu���擾����
	// ���݂̃Z�b�V�������擾����
	char Session[128];
	GetPrivateProfileString("DB", "SESSION", "", (char *)Session, sizeof(Session), PCINI_NAME);
	// �ʒu����
	char SessionWK[128];
	CString		cKey;
	for(int ii=1; ii<10; ii++) {
		cKey.Format("ODBCSET_%d", ii); 
		GetPrivateProfileString("DB", cKey, "", SessionWK, sizeof(SessionWK), PCINI_NAME);
		if(0 == strcmp(Session, SessionWK)) {
			m_nOdbcID = ii;
			break;
		}
	}


	//// �ŐV���t��AutoNo���擾����
	CString cSql;					// SQL��

	// SQL��
	cSql.Format("SELECT TOP(1) AutoNo FROM %s ORDER BY �o�^���t desc, AutoNo desc", DB_STATUS_LOG);

	// ���s
	int nWk;
	if( ! KizuODBC::OdbcBase::GetSelectKey_Static(cSql, &nWk, TASKINI_NAME, INI_DB) ) {
		LOG(em_ERR), "[%s] �I�[�gNo �ŐV�l �擾���s�I�I", my_sThreadName );
		m_nAutoNo = 0;
	} else {
		m_nAutoNo = nWk / 10;
		LOG(em_INF), "[%s] �I�[�gNo �ŐV�l DB��AutoNo=%d, ���ݓ��t=%d (�A��=%d, ID=%d)", my_sThreadName, nWk, m_nDay, m_nAutoNo, m_nOdbcID);
	}
}

//------------------------------------------
// ���t���`�F�b�N���āA�����ς��΁A�I�[�gNo�����Z�b�g����B
// int nDay �o�^��
//------------------------------------------
void StatusLogFunc::RefrashAutoNo(int nDay)
{
	// �����ύX���ꂽ�H
	if(m_nDay == nDay) {
		// �������Ȃ� AutoNo���X�V���邾��
		m_nAutoNo ++;
		return;
	}

	// �Z�b�g
	LOG(em_MSG), "[%s] �I�[�gNo���Z�b�g �ύX��[%d��%d]", my_sThreadName, m_nDay, nDay);
	m_nDay	  = nDay;
	m_nAutoNo = 1;
}

//------------------------------------------
// �X�e�[�^�X���`�F�b�N
// TYP_STATUS_LOG* pTyp ����̏��
//------------------------------------------
void StatusLogFunc::CheckStatusLog(TYP_STATUS_LOG* pTyp)
{
	bool bAdd = false;

	//--------------------------------
	// �o�^�`�F�b�N

	// �O�񂩂�ω��L��H
	if( 0 != memcmp( &m_oldStatusLog, pTyp, sizeof(TYP_STATUS_LOG::CHECK_DATA)) ) {
		memcpy( &m_oldStatusLog, pTyp, sizeof(m_oldStatusLog));
		bAdd = true;
	}
	// �ғ���Ԃ��A���������̂Ƃ��́A�o�^���Ȃ��B�T������
	if( DIV_KADOU_INIT == pTyp->Data.emKadou ) {
		bAdd = false;
	}


	//---------------
	// DB�o�^
	if(bAdd) {
		//// ���O����
		DELIVERY_DB* pDeli = new DELIVERY_DB;
		pDeli->kubun = ENUM_DB_KUBUN::em_STATUS_LOG;
		pDeli->data = (BYTE*)new DBDATA_STATUS_LOG;
		DBDATA_STATUS_LOG* pWk = (DBDATA_STATUS_LOG*)pDeli->data;

		// �f�[�^
		pWk->emKadou	= pTyp->Data.emKadou;
		for(int ii=0; ii<NUM_MEN; ii++) {
			pWk->emKensa[ii]	= pTyp->Data.emKensa[ii];
			pWk->emKiki[ii]		= pTyp->Data.emKiki[ii];
		}
		memcpy(pWk->cMsg, pTyp->Data.cMsg, sizeof(pTyp->Data.cMsg));
		memcpy(pWk->cKizukenNo, pTyp->cKizukenNo, sizeof(pTyp->cKizukenNo));
		memcpy(pWk->cCoilNo, pTyp->cCoilNo, sizeof(pTyp->cCoilNo));

		// ��L�[�Z�b�g
		pWk->time = CTime::GetCurrentTime();
		RefrashAutoNo(pWk->time.GetDay());			// ���t�ɂ��I�[�gNo���Z�b�g
		pWk->nAutoNo = (m_nAutoNo*10 + m_nOdbcID); 

		LOG(em_MSG), "[%s] �ω�[%s][%s][%s/%s][%s/%s]", my_sThreadName, pWk->cCoilNo, pWk->cKizukenNo,
			DivNameManager::GetDivKensa(pWk->emKensa[0]), DivNameManager::GetDivKensa(pWk->emKensa[1]),
			DivNameManager::GetDivKiki(pWk->emKiki[0]), DivNameManager::GetDivKiki(pWk->emKiki[1]) );


		//// �o�^�˗�
		if( ! mque_pAddSQLData->AddToTailFreeCheck(pDeli, 10) ) { 
			// �L���[�t��
			delete pDeli->data;
			delete pDeli;

			// DB���ُ� (�ʏ킠�肦�Ȃ�)
			LOG(em_ERR), "[%s] DB�o�^�˗��L���[�t�� [DBDATA_STATUS_LOG]", my_sThreadName);
			syslog(SYSNO_QUEFULL_WAR, "[DBDATA_STATUS_LOG]");
		}
	}
}

