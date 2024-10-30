#include "StdAfx.h"
#include "KizuPcInfManager.h"

using namespace KizuLib;

//------------------------------------------
// �R���X�g���N�^
// int nPCID PCID (0:DB�ڑ�����(PC����.)�A -1:DB�ڑ�����(PC���̓R���s���[�^�����擾)�A -9:DB�ڑ�����(PC���̓R���s���[�^�����擾))
//------------------------------------------
KizuPcInfManager::KizuPcInfManager(int nPCID)
{
	// ������
	m_nPcId = nPCID;
	memset(m_cMyPcName, 0x00, sizeof(m_cMyPcName));
	if( 0 == nPCID ) m_cMyPcName[0] = '.';
	else			 m_cMyPcName[0] = '*';
	
	// �R���s���[�^�����擾
	if( -1 == nPCID || -9 == nPCID) {
		char  cName[256];
		DWORD len = sizeof(cName);
		if( GetComputerName(cName, &len) ) {
			memset(m_cMyPcName, 0x00, sizeof(m_cMyPcName));
			strcpy(m_cMyPcName, cName);
		}
	}

	// ���̑�������
	m_nPcMode = 1;
	m_nPcKind = 0;
	memset(m_cPcKindName, 0x00, sizeof(m_cPcKindName));
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
KizuPcInfManager::~KizuPcInfManager(void)
{

}

//------------------------------------------
// PC���̎擾
//------------------------------------------
bool KizuPcInfManager::SetPcName(char *pcname, LPCSTR cIniPath, LPCSTR cSession )
{
	int ii;
	char cWk[MAX_PCCOUNT][SIZE_NAME_PC];

	// ���[�N�ɏ����l���Z�b�g
	memset(cWk, 0x00, sizeof(cWk));
	// ���[�J���̂݁H
	if( 0 == m_nPcId || -1 == m_nPcId ) {
		for( ii=0; ii<MAX_PCCOUNT; ii++) cWk[ii][0] = '.';
		memcpy(pcname, cWk[0], sizeof(cWk));
		return true;
	}

	// DB�ڑ��L��
	for( ii=0; ii<MAX_PCCOUNT; ii++) cWk[ii][0] = '*';

	// DB�ڑ��`�F�b�N
	OdbcBase cls_Db;
	if( ! cls_Db.Connect(cIniPath, cSession ) ) {
		// �ڑ����s�̈� * ���Z�b�g
		memcpy(pcname, cWk[0], sizeof(cWk));
		return false;
	}

	////// DB���PC��ʂ��擾����
	//// SQL�� ����
	int kind;
	int id;
	char sql[256];
	memset(sql, 0x00, sizeof(sql));
	sprintf(sql, "SELECT i.PCKIND, m.PC��, m.PCID, i.PC�敪, i.PC��� FROM %s m INNER JOIN %s i ON m.PCID = i.PCID ORDER BY i.PCKIND ASC",
								 DB_PC_MASTER, DB_PC_INF );


	//// ���s
	if( !cls_Db.ExecuteDirect(sql)) {
		//syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", DB_PC_INF);
		return false;
	}

	//// �擾
	while (TRUE) {
		int sqlrc = cls_Db.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��
			cls_Db.GetData(1, &kind );
			if(1<kind && MAX_PCCOUNT<kind) {
				// kind ng 	
				continue;
			}

			cls_Db.GetData(2, cWk[kind-1], SIZE_NAME_PC );
			cls_Db.GetData(3, &id );

			// ��PCID�Ɠ���PCID�H
			if(m_nPcId == id) {
				// ��PC�̖���������
				m_nPcKind = kind;
				memcpy(m_cMyPcName, cWk[kind-1], SIZE_NAME_PC);
				cls_Db.GetData(4, &m_nPcMode );
				cls_Db.GetData(5, m_cPcKindName, sizeof(m_cPcKindName) );
			}

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			//syslog(SYSNO_DB_FETCH_ERR, "[%s]", DB_PC_INF);
			return false;
		}
	}
	// �ꊇ�Z�b�g
	memcpy(pcname, cWk[0], sizeof(cWk));
	return true;
}

//------------------------------------------
// �w��O���[�v��PCID���擾
// (�ʏ픻��PC�̌��ƂȂ�PCID��S���Ƃ��Ă��铙�̎g����������)
// LPCSTR cIniPath ini�t�@�C���p�X
// int groupno �擾����GroupNo
// int maxcnt �擾����ő匏��
// int* count �擾��������
// int* nPcid �擾����PCID�̔z�� (����)
// bool bTablePcinf �Ǎ���e�[�u���̐ؑ�(�f�t�H���gfalse) false:PC_INF=�蓮�ݒ��PCID��ύX�����ꍇ�̏�Ԃ��擾�ł���Btrue:PC_MASTER=���K��PCID���擾�ł���B
// �߂�l ���A���
//------------------------------------------
bool KizuPcInfManager::GetGroupPcID(LPCSTR cIniPath, int groupno, int maxcnt, int* count, int* nPcid, bool bTablePcinf)
{
	*count = 0;

	// DB�ڑ��`�F�b�N
	OdbcBase cls_Db;
	if( ! cls_Db.Connect(cIniPath) ) {
		return false;
	}

	char sql[256];
	memset(sql, 0x00, sizeof(sql));
	/*
	// 20081125 �����߂��őΉ�
	//sprintf(sql, "SELECT PCID FROM %s WHERE GroupNo = %d ORDER BY PCID ASC", DB_PC_MASTER, groupno );
	sprintf(sql, "SELECT PCID FROM %s WHERE GroupNo = %d ORDER BY PCID ASC", DB_PC_INF, groupno );
	*/
	// 20090225 �����̃e�[�u����ǂރP�[�X������̂ŁA������ǉ�
	if(bTablePcinf)	sprintf(sql, "SELECT PCID FROM %s WHERE GroupNo = %d ORDER BY PCID ASC", DB_PC_MASTER, groupno );
	else			sprintf(sql, "SELECT PCID FROM %s WHERE GroupNo = %d ORDER BY PCID ASC", DB_PC_INF, groupno );

	//// ���s
	if( !cls_Db.ExecuteDirect(sql)) {
		return false;
	}

	//// �擾
	while (TRUE) {
		int sqlrc = cls_Db.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

			cls_Db.GetData(1, &nPcid[*count] );
			*count += 1;
			if( maxcnt<= *count) break;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			return false;
		}
	}
	return true;
}
//------------------------------------------
// �w��O���[�v��PCID���Z�b�g����  (PC_INF)
// (�ʏ픻��PC�̃o�b�N�A�b�v�؂�ւ�����PCID��U�蒼��)
// LPCSTR cIniPath ini�t�@�C���p�X
// int groupno �Z�b�g����GroupNo
// int count �Z�b�g���錏��
// int const* nPcid �Z�b�g������PCID�̔z�� (����)
// �߂�l ���A���
//------------------------------------------
bool KizuPcInfManager::SetGroupPcID(LPCSTR cIniPath, int groupno, int count, int const* nPcid )
{
	int cnt = 0;
	int wk;

	// DB�ڑ��`�F�b�N
	OdbcBase cls_Db_Select;
	if( ! cls_Db_Select.Connect(cIniPath) ) {
		return false;
	}
	// DB�ڑ��`�F�b�N
	OdbcBase cls_Db_UpDate;
	if( ! cls_Db_UpDate.Connect(cIniPath) ) {
		return false;
	}

	char sqlSelect[256];
	char sqlUpDate[256];
	memset(sqlSelect, 0x00, sizeof(sqlSelect));
	sprintf(sqlSelect, "SELECT PCKIND FROM %s WHERE GroupNo = %d ORDER BY PCKIND ASC", DB_PC_INF, groupno );

	//// ���s
	if( !cls_Db_Select.ExecuteDirect(sqlSelect)) {
		return false;
	}

	//// �擾 ����� �Z�b�g
	while (TRUE) {
		int sqlrc = cls_Db_Select.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

			//// �擾�������� PCID��ύX���Ă���
			// �擾
			cls_Db_Select.GetData(1, &wk );
			
			// �ύX
			memset(sqlUpDate, 0x00, sizeof(sqlUpDate));
			sprintf(sqlUpDate, "UPDATE %s SET PCID=%d WHERE PCKIND=%d", DB_PC_INF, nPcid[cnt], wk );
			cnt ++;
			if( !cls_Db_UpDate.ExecuteDirect(sqlUpDate)) {
				return false;
			}
			if( count <= cnt) break;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			return false;
		}
	}

	cls_Db_UpDate.TranCommit();
	return true;
}
