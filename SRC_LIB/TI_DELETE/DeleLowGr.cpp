#include "StdAfx.h"
#include "DeleLowGr.h"

//------------------------------------------
// �R���X�g���N�^
// int id ������ID (1�I���W��)
// HANDLE evEndExec ���s�����V�O�i��
// SETTING_TBL const* pTyp �폜�e�[�u�����
//------------------------------------------
DeleLowGr::DeleLowGr(int id, HANDLE evEndExec, SETTING_TBL const* pTyp) :
ThreadManager( GetThreadNameArray("LowGr", id) ),
m_nID(id),
m_evEndExec(evEndExec),
m_pTypSet(pTyp),
m_bIsExec(false),
m_bStopFlg(false)
{
	char		cWk[1024];
	GetPrivateProfileString(TI_DELETE, "SQL_SELECT_END", "", cWk, sizeof(cWk), TASKINI_NAME );
	m_cIniSqlSelectEnd = cWk;


	// ������돉����
	memset(m_cKey, 0x00, sizeof(m_cKey));

	// �C�x���g����
	m_evExec = CreateEvent(NULL, FALSE, FALSE, NULL);

}


//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
DeleLowGr::~DeleLowGr(void)
{
	// �C�x���g�j��
	CloseHandle(m_evExec);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �X���b�h����

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int DeleLowGr::ThreadFirst()
{
	//// �C�x���g�ݒ�
	HANDLE hArray[] = { m_evExec			// �폜���s�˗�
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int DeleLowGr::ThreadLast()
{
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void DeleLowGr::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_EXEC = 0,						// �폜���s�˗�
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_EXEC:											// �폜���s�˗�
		LOG(em_MSG), "[%s] ���QGr�폜 ���s�˗� [%s]", my_sThreadName, m_cKey);
		if(m_bStopFlg) break;		// �I���H

		// �e�[�u���ɍ폜�������ʂ𔽉f
		if( ! StartExec() ) {
			// �I���
			m_bIsExec = false;// �O�̈�
			memset(m_cKey, 0x00, sizeof(m_cKey));
		}
		break;
//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���C������

//------------------------------------------
// ���QGr�폜�J�n
//------------------------------------------
bool DeleLowGr::StartExec()
{
	CString sql;
	int		nSubNo;

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, INI_DB)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] [%s] DB�ڑ��G���[", my_sThreadName, INI_DB);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// ���э폜
	for(int ii=0; ii<m_pTypSet->nMainCnt; ii++) {
		while(true) {
			//// ����̍폜�Ώۂ��rNo���擾
			sql.Format("SELECT ISNULL(MAX(a.%s),0) FROM (SELECT TOP %d %s FROM %s \
					   WHERE %s='%s' %s ORDER BY %s DESC, %s ASC) AS a",
				m_pTypSet->cKeySub, DELETE_MAXCNT, m_pTypSet->cKeySub, m_pTypSet->main[ii].cTblMain, 
				m_pTypSet->cKeyMain, m_cKey, m_pTypSet->main[ii].cSqlWhere, m_pTypSet->cKeyMain, m_pTypSet->cKeySub );
			clsDB.GetSelectKey(sql, &nSubNo);
			if(m_bStopFlg) return false;		// �I���H

			//// �I���H
			if(0 == nSubNo) break;


			//// �폜�e�[�u���̎��т��폜
			for(int jj=0; jj<m_pTypSet->main[ii].nSubCnt; jj++) {
				if( ! DeleteInf( &clsDB, m_pTypSet->main[ii].cTblMain, m_pTypSet->main[ii].cSqlWhere, m_pTypSet->main[ii].cTblSub[jj], m_cKey, nSubNo) ) return false;
				if(m_bStopFlg) return false; // �I���H
				Sleep(SLEEP_TIME);
				if(m_bStopFlg) return false; // �I���H
			}

			//// �L�[�e�[�u���̎��т��폜
			if( ! DeleteInf( &clsDB, m_pTypSet->main[ii].cTblMain, m_pTypSet->main[ii].cSqlWhere, m_pTypSet->main[ii].cTblMain, m_cKey, nSubNo) ) return false;
			if(m_bStopFlg) return false; // �I���H

			LOG(em_MSG), "[%s] �폜�� ��� [%s] [%s] �rNo=%d �܂Ŋ���", my_sThreadName, m_pTypSet->main[ii].cTblMain, m_cKey, nSubNo);
		}
	}

	//// �Ǘ��e�[�u���X�V
	SetDeleteOkFlg(&clsDB, m_cKey);
	clsDB.ExecuteDirect_ShrinkFile();				// DB���k 
	return true;
}

//------------------------------------------
// ���QGr�폜���s
// OdbcBase* pClsDB DB����N���X
// char const* cKeyTbl ��ƂȂ�e�[�u�� (DEFECT_INF�̂�)
// char const* cKeyWhereSql ���oSQL
// char const* cDeleTbl �폜�ΏۂƂȂ���уe�[�u��
// char const* cKey �Ώۂ��r���Ǘ�No
// int nSubNo �Ώۂ̍ő��rNo
//------------------------------------------
bool DeleLowGr::DeleteInf(OdbcBase* pClsDB, char const* cKeyTbl, char const* cKeyWhereSql, char const* cDeleTbl, char const* cKey, int nSubNo)
{
	CString sql;

	// DEFECT_INF�̖��Q�ꎞ�ۑ��t���O�ɕR�t�� �rNo ���폜����
	sql.Format("DELETE FROM %s WHERE %s='%s' AND %s IN \
			   (SELECT %s FROM %s WHERE %s='%s' AND %s<=%d %s)", 
		cDeleTbl, m_pTypSet->cKeyMain, cKey, m_pTypSet->cKeySub, 
		m_pTypSet->cKeySub, cKeyTbl, m_pTypSet->cKeyMain, cKey, m_pTypSet->cKeySub, nSubNo, cKeyWhereSql);

	if( !pClsDB->ExecuteDirect(sql) ) {
		LOG(em_ERR), "[%s] %s�폜���s", my_sThreadName, cDeleTbl);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDeleTbl);
		return false;
	}
	return true;
}

//------------------------------------------
// ���QGr�폜�Ǘ��e�[�u�����A�b�v�f�[�g
// OdbcBase* pClsDB DB����N���X
// char const* cKey �Ώۂ��r���Ǘ�No
//------------------------------------------
bool DeleLowGr::SetDeleteOkFlg(OdbcBase* pClsDB, char const* cKey)
{
	CString sql;

	// �Ǘ��e�[�u�����X�V
	if( 0 != m_cIniSqlSelectEnd.GetLength() ) {
		sql.Format(m_cIniSqlSelectEnd, cKey);
		if( !pClsDB->ExecuteDirect(sql) ) {
			LOG(em_ERR), "[%s] �X�V���s (%s)", my_sThreadName, m_cIniSqlSelectEnd);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", m_cIniSqlSelectEnd);
			return false;
		}
	}

	// ���튮�����̂�
	LOG(em_MSG), "[%s] ���QGr�폜����[%s]", my_sThreadName, cKey);
	m_bIsExec = false;
	memset(m_cKey, 0x00, sizeof(m_cKey));
	Sleep(SLEEP_TIME);

	SetEvent(m_evEndExec);
	return true;
}