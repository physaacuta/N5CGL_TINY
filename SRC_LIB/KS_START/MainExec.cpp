#include "StdAfx.h"
#include "MainExec.h"

//------------------------------------------
// �R���X�g���N�^
// LogFileManager* pLog ���O�N���X
// bool* bCancel �����L�����Z���|�C���^
//------------------------------------------
MainExec::MainExec(LogFileManager* pLog, bool* bCancel) :
m_nIniNowID(-1),
m_pCancel(bCancel),
mcls_pLog(pLog)
{
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
MainExec::~MainExec(void)
{
}


//------------------------------------------
// ���C������
//------------------------------------------
void MainExec::DoExec()
{
	CString		cKey;				
	int			nReTryCnt;				// ���݂̃��g���C��
	int			nReTryMax;				// �ő僊�g���C��

	//// �t�H���_�[�R�s�[
	LOG(em_MSG), "�t�H���_�R�s�[�J�n");
	nReTryMax = GetPrivateProfileInt("FOLDERCOPY", "RETRY", 0, PCINI_NAME);
	nReTryCnt = 0;
	while(true) {
		if( *m_pCancel ) return;
		if( ExecFolderCopy() ) break;
		
		// ���g���C�`�F�b�N
		if( 0 < nReTryMax && nReTryMax <= nReTryCnt) {
			LOG(em_ERR), "�w�胊�g���C�񐔂ɓ��B�����ׁA�t�H���_�R�s�[���f�I");
			break;
		}
		if( 0 < nReTryMax) nReTryCnt += 1;

		LOG(em_ERR), "�t�H���_�R�s�[�҂��E�E�E");
		Sleep(5000);
	}

	//// �t�@�C���R�s�[ (�������s���́A�������ُ�̂͂�������A��������܂ő҂�)
	LOG(em_MSG), "�t�@�C���R�s�[�J�n");
	nReTryMax = GetPrivateProfileInt("FILECOPY", "RETRY", 0, PCINI_NAME);
	nReTryCnt = 0;
	while(true) {
		if( *m_pCancel ) return;
		if( ExecFileCopy() ) break;

		// ���g���C�`�F�b�N
		if( 0 < nReTryMax && nReTryMax <= nReTryCnt) {
			LOG(em_ERR), "�w�胊�g���C�񐔂ɓ��B�����ׁA�t�H���_�R�s�[���f�I");
			break;
		}
		if( 0 < nReTryMax) nReTryCnt += 1;

		LOG(em_ERR), "�t�@�C���R�s�[�҂��E�E�E");
		Sleep(5000);
	}

	//// WakeOnLan ���s
	LOG(em_MSG), "Wake On Lan ���s");
	MagicPacket::Send(PCINI_NAME); 


	//// �@�\�ؑ�
	// ���@�\ID��荞��
	LOG(em_MSG), "���@�\ID��荞�݊J�n");
	m_nIniNowID = GetPrivateProfileInt("NOWID", "ID", -1, PCINI_NAME);
	if(0 > m_nIniNowID ) {
		LOG(em_ERR), "���@�\ID�ُ� [%d]", m_nIniNowID);
		_ASSERT(0);
		return;
	}
	// �@�\�ؑ֌��p�X�擾
	LOG(em_MSG), "�ݒ�؂�ւ��J�n ���@�\ID=%d", m_nIniNowID);
	char		cFrom[MAX_PATH];
	cKey.Format("NAME_%d", m_nIniNowID); 
	GetPrivateProfileString("KIZUTASKINI", cKey, "", cFrom, MAX_PATH, PCINI_NAME);
	if(0 == strlen(cFrom) ) {
		LOG(em_WAR), "KizuTask.ini�ؑ֌��p�X ���� [%s]", cFrom);
		//return; �����̏ꍇ������
	} else {
		// �ؑ�
		if( ! CopyFile(cFrom, TASKINI_NAME, FALSE) ) {
			LOG(em_ERR), "KizuTask.ini�ؑֈُ� [%s��%s]", cFrom, TASKINI_NAME);
			//return; �����̏ꍇ������
		}
	}
	
	//// ODBC�̎����؊���
	Change_DB();
	Sleep(5000);	// �؂�ւ����炿����Ƒ҂������������H


	//// �^�X�N�̎��s
	LOG(em_MSG), "�^�X�N���s�J�n");
	int cnt = 0;
	while(true) {
		cKey.Format("TASK_%d_%d", m_nIniNowID, cnt); 
		GetPrivateProfileString("EXECUTE", cKey, "", cFrom, MAX_PATH, PCINI_NAME);
		
		// �L��
		if(0 == strlen(cFrom) ) break;

		// ���s
		ProcessManager::WinExec(cFrom);

		// ��
		cnt ++;
	}
}


//------------------------------------------
// �t�@�C���R�s�[���s
// �߂�l 1��ł��R�s�[�ł�����true�B�������́A�R�s�[�t�@�C���������ꍇ��true
//------------------------------------------
bool MainExec::ExecFileCopy()
{
	char		cBaseFrom[MAX_PATH];	// �R�s�[���x�[�X�p�X
	char		cBaseTo[MAX_PATH];		// �R�s�[��x�[�X�p�X
	char		cWk[MAX_PATH];

	char		cFrom[MAX_PATH];		// �R�s�[��
	char		cTo[MAX_PATH];			// �R�s�[��
	CString		cKey;

	// �x�[�X�p�X�擾 (���ݒ苖����)
	GetPrivateProfileString("FILECOPY", "F_BASE", "", cBaseFrom, MAX_PATH, PCINI_NAME);
	GetPrivateProfileString("FILECOPY", "T_BASE", "", cBaseTo,   MAX_PATH, PCINI_NAME);

	// �R�s�[���s
	//bool retc = false;
	int cnt = 1;
	while(true) {
		//cKey.Format("F_%d", cnt); 
		//GetPrivateProfileString("FILECOPY", cKey, "", cFrom, MAX_PATH, PCINI_NAME);
		//cKey.Format("T_%d", cnt); 
		//GetPrivateProfileString("FILECOPY", cKey, "", cTo,   MAX_PATH, PCINI_NAME);

		// From
		cKey.Format("F_%d", cnt); 
		if( 0 == strlen(cBaseFrom) ) {
			GetPrivateProfileString("FILECOPY", cKey, "", cFrom, MAX_PATH, PCINI_NAME);
		} else {
			GetPrivateProfileString("FILECOPY", cKey, "", cWk,   MAX_PATH, PCINI_NAME);
			if(0 == strlen(cWk) )	cFrom[0] = NULL;
			else					sprintf(cFrom, "%s%s", cBaseFrom, cWk);
		}
		// To
		cKey.Format("T_%d", cnt); 
		if( 0 == strlen(cBaseTo) ) {
			GetPrivateProfileString("FILECOPY", cKey, "", cTo, MAX_PATH, PCINI_NAME);
		} else {
			GetPrivateProfileString("FILECOPY", cKey, "", cWk, MAX_PATH, PCINI_NAME);
			if(0 == strlen(cWk) )	cFrom[0] = NULL;
			else					sprintf(cTo, "%s%s", cBaseTo, cWk);	
		}

		// �L��
		if(0 == strlen(cFrom) || 0 == strlen(cTo)) {
			// �R�s�[�t�@�C��������
			//if(1==cnt) retc = true;
			//break;
			return true;
		}

		// �R�s�[
		if( ! CopyFile(cFrom, cTo, FALSE) ) {
			LOG(em_ERR), "[FILECOPY] ���s [%s]��[%s]", cFrom, cTo);
			//return retc;
			if(1==cnt) return false;	// �܂��R�s�[��ƂȂ����Ă��Ȃ�
		} else {
			LOG(em_MSG), "[FILECOPY] ���s [%s]��[%s]", cFrom, cTo);
		}
		//retc = true;
		cnt ++;
	}
	return true;
}

//------------------------------------------
// �t�H���_�[�R�s�[���s
// �߂�l 1��ł��R�s�[�ł�����true�B�������́A�R�s�[�t�H���_�������ꍇ��true
//------------------------------------------
bool MainExec::ExecFolderCopy()
{

	char		cBaseFrom[MAX_PATH];	// �R�s�[���x�[�X�p�X
	char		cBaseTo[MAX_PATH];		// �R�s�[��x�[�X�p�X
	char		cWk[MAX_PATH];

	char		cFrom[MAX_PATH];		// �R�s�[���t�H���_ (\����)
	char		cTo[MAX_PATH];			// �R�s�[��t�H���_ (\����)
	CString		cKey;
	CString		cFromFPath;
	CString		cToFPath;

	CString		cSearchFile;			// �t�H���_���̃t�@�C������������L�[
	BOOL		bRet;

	WIN32_FIND_DATA		fd;
	HANDLE				hFind;

//	// �������g( PCINI_NAME ) ���������������܂������珑�������Ȃ��p�Ƀt�@�C�����J���Ă���
//	FILE* f = fopen(PCINI_NAME, "N");

	// �x�[�X�p�X�擾 (���ݒ苖����)
	GetPrivateProfileString("FOLDERCOPY", "F_BASE", "", cBaseFrom, MAX_PATH, PCINI_NAME);
	GetPrivateProfileString("FOLDERCOPY", "T_BASE", "", cBaseTo,   MAX_PATH, PCINI_NAME);


	////// �R�s�[���s
	bool retc = false;
	int cnt = 1;
	while(true) {
		//cKey.Format("F_%d", cnt); 
		//GetPrivateProfileString("FOLDERCOPY", cKey, "", cFrom, MAX_PATH, PCINI_NAME);
		//cKey.Format("T_%d", cnt); 
		//GetPrivateProfileString("FOLDERCOPY", cKey, "", cTo,   MAX_PATH, PCINI_NAME);
	
		// From
		cKey.Format("F_%d", cnt); 
		if( 0 == strlen(cBaseFrom) ) {
			GetPrivateProfileString("FOLDERCOPY", cKey, "", cFrom, MAX_PATH, PCINI_NAME);
		} else {
			GetPrivateProfileString("FOLDERCOPY", cKey, "", cWk,   MAX_PATH, PCINI_NAME);
			if(0 == strlen(cWk) )	cFrom[0] = NULL;
			else					sprintf(cFrom, "%s%s", cBaseFrom, cWk);
		}
		// To
		cKey.Format("T_%d", cnt); 
		if( 0 == strlen(cBaseTo) ) {
			GetPrivateProfileString("FOLDERCOPY", cKey, "", cTo, MAX_PATH, PCINI_NAME);
		} else {
			GetPrivateProfileString("FOLDERCOPY", cKey, "", cWk, MAX_PATH, PCINI_NAME);
			if(0 == strlen(cWk) )	cFrom[0] = NULL;
			else					sprintf(cTo, "%s%s", cBaseTo, cWk);	
		}

		// �L��
		if(0 == strlen(cFrom) || 0 == strlen(cTo)) {
			// �R�s�[�t�H���_������
			if(1==cnt) retc = true;
			break;
		}

		// �t�H���_�`�F�b�N �� ����
		if( ! CreateFolder(cTo) ) {
			LOG(em_ERR), "[FOLDERCOPY] [%s]�t�H���_�A�N�Z�X�A�������s", cTo);
			cnt ++;
			continue;
		}

		// �T�[�`����t�@�C��
		cSearchFile.Format( "%s\\*.*", cFrom);
		
		//// �t�@�C���T�[�`
		memset(&fd, 0x00, sizeof(fd));
		bRet = TRUE;
		hFind= FindFirstFile(cSearchFile, &fd);
		while(hFind != INVALID_HANDLE_VALUE && bRet) {
			// �t�@�C���̂�
			// �� Exe��KizuPc.ini�͏�����������_��
			if( FILE_ATTRIBUTE_DIRECTORY != (FILE_ATTRIBUTE_DIRECTORY & fd.dwFileAttributes) &&
				0 != strcmp(fd.cFileName, "KizuPc.ini") ) {
				
				// �R�s�[��A���̃t���p�X����
				cFromFPath.Format("%s\\%s", cFrom, fd.cFileName );
				cToFPath.Format  ("%s\\%s", cTo,   fd.cFileName );
			
				// �R�s�[
				if( ! CopyFile(cFromFPath, cToFPath, FALSE) ) {
					LOG(em_ERR), "[FOLDERCOPY] ���s [%s]��[%s]", cFromFPath, cToFPath);
				} else {
					LOG(em_MSG), "[FOLDERCOPY] ���s [%s]��[%s]", cFromFPath, cToFPath);
				}
			}

			// ��
			bRet = FindNextFile(hFind, &fd);
			retc = true;		// �����܂ŗ��� = �����ɃA�N�Z�X�o����
		}
		FindClose(hFind);

		// ���̃t�H���_
		cnt ++;
	}

	//// �J���Ă����t�@�C�������
//	fclose(f);
	return retc;
}

//------------------------------------------
// �t�H���_�[����
// char const* cPath �����t�H���_�p�X
// �߂�l false:�������ł��Ȃ�����
//------------------------------------------
bool MainExec::CreateFolder(char const* cPath)
{
	if( PathIsDirectory( cPath ) ) return true;

	// ����
	LOG(em_WAR), "[FOLDERCOPY] [%s]���������߃t�H���_�����J�n", cPath);
	_mkdir(cPath);
	return PathIsDirectory( cPath );

}

//===========================================================================
// DB�̑���֘A
//===========================================================================
//------------------------------------------
// DB�̕ύX
// char* Session DB�̃Z�b�V���� (�Y�������̏ꍇ��NULL)
//------------------------------------------
void MainExec::Change_DB()
{
	// �K�v�ȏ��擾
	char Session[128];
	GetPrivateProfileString("DB", "SESSION", "", (char *)Session, sizeof(Session), PCINI_NAME);
	if(0 == strlen(Session)) {
		LOG(em_ERR), "DB�̃Z�b�V������񖳂�");
		return;
	}

	//// ���ݐݒ肳��Ă���Z�b�V������KizuTask.ini������������
	ChangeIni_DBSession(Session);
	
	// 2019.01.23 DB�̃Z�b�V�����؂�ւ��𕡐��w��Ή� --->>>
	{
		CString		cKey;
		CString		cDbSection;
		int			cnt = 1;
		while (true) {
			cKey.Format("SESSION_%d", cnt);
			cDbSection.Format("%s_%d", INI_DB, cnt);
			memset(Session, 0x00, sizeof(Session));
			GetPrivateProfileString("DB", cKey, "", (char *)Session, sizeof(Session), PCINI_NAME);
			if (0 == strlen(Session)) break;

			//// ���ݐݒ肳��Ă���Z�b�V������KizuTask.ini������������
			ChangeIni_DBSession_N(Session, cDbSection);
			cnt++;
		}
	}
	// 2019.01.23 DB�̃Z�b�V�����؂�ւ��𕡐��w��Ή� ---<<<

	//// KizuTask.ini��DB����ODBC�̎����ݒ���s��
	CString		cKey;
	int			cnt = 1;
	while(true) {
		cKey.Format("ODBCSET_%d", cnt); 
		memset(Session, 0x00, sizeof(Session));
		GetPrivateProfileString("DB", cKey, "", Session, sizeof(Session), PCINI_NAME);
		if(0 == strlen(Session)) break;
		
		// �����o�^
		ChangeODBC_Setting(Session, cnt);
		cnt ++;
	}
}

//------------------------------------------
// Ini�t�@�C����DB�̃Z�b�V��������������
// char* Session DB�̃Z�b�V���� (�Y�������̏ꍇ��NULL)
//------------------------------------------
void MainExec::ChangeIni_DBSession(char const * Session)
{
	char wk_addr[128];
	char wk_odbc[128];
	char wk_dsna[128];
	char wk_user[128];
	char wk_pass[128];

	// NULL�Ȃ�ύX����
	if ( 0==strlen(Session) ) return;

	// A�n��B�n�̃f�[�^���擾
	GetPrivateProfileString(Session, "DBADDLESS", "", (char *)wk_addr, sizeof(wk_addr), TASKINI_NAME);
	GetPrivateProfileString(Session, "ODBCNAME",  "", (char *)wk_odbc, sizeof(wk_odbc), TASKINI_NAME);
	GetPrivateProfileString(Session, "DSNAME",    "", (char *)wk_dsna, sizeof(wk_dsna), TASKINI_NAME);
	GetPrivateProfileString(Session, "USERID",    "", (char *)wk_user, sizeof(wk_user), TASKINI_NAME);
	GetPrivateProfileString(Session, "PASSWORD",  "", (char *)wk_pass, sizeof(wk_pass), TASKINI_NAME);
	// �f�[�^���S�����邩�`�F�b�N
	if( 0 == strlen(wk_addr) ||
		0 == strlen(wk_odbc) ||
		0 == strlen(wk_dsna) ||
		0 == strlen(wk_user) ||
		0 == strlen(wk_pass) ) {
		LOG(em_MSG), "[CheckDB] DB ini Session����������񖳂�");
		return;
	}

	// A�n��B�n�̃f�[�^���I�����C���ɃR�s�[
	WritePrivateProfileString( INI_DB, "DBADDLESS", wk_addr, TASKINI_NAME);
	WritePrivateProfileString( INI_DB, "ODBCNAME",	wk_odbc, TASKINI_NAME);
	WritePrivateProfileString( INI_DB, "DSNAME",	wk_dsna, TASKINI_NAME);
	WritePrivateProfileString( INI_DB, "USERID",	wk_user, TASKINI_NAME);
	WritePrivateProfileString( INI_DB, "PASSWORD",	wk_pass, TASKINI_NAME);
	LOG(em_MSG), "[CheckDB] DB ini Session�������� [%s]��[%s]", Session, INI_DB);

}

// 2019.01.23 DB�̃Z�b�V�����؂�ւ��𕡐��w��Ή� --->>>
//------------------------------------------
// Ini�t�@�C����DB�̃Z�b�V��������������
// char* Session DB�̃Z�b�V���� (�Y�������̏ꍇ��NULL)
// char* DbSection DB�̃Z�N�V������
//------------------------------------------
void MainExec::ChangeIni_DBSession_N(char const * Session, char const * DbSection)
{
	char wk_addr[128];
	char wk_odbc[128];
	char wk_dsna[128];
	char wk_user[128];
	char wk_pass[128];

	// NULL�Ȃ�ύX����
	if (0 == strlen(Session)) return;
	if (0 == strlen(DbSection)) return;

	// A�n��B�n�̃f�[�^���擾
	GetPrivateProfileString(Session, "DBADDLESS", "", (char *)wk_addr, sizeof(wk_addr), TASKINI_NAME);
	GetPrivateProfileString(Session, "ODBCNAME", "", (char *)wk_odbc, sizeof(wk_odbc), TASKINI_NAME);
	GetPrivateProfileString(Session, "DSNAME", "", (char *)wk_dsna, sizeof(wk_dsna), TASKINI_NAME);
	GetPrivateProfileString(Session, "USERID", "", (char *)wk_user, sizeof(wk_user), TASKINI_NAME);
	GetPrivateProfileString(Session, "PASSWORD", "", (char *)wk_pass, sizeof(wk_pass), TASKINI_NAME);
	// �f�[�^���S�����邩�`�F�b�N
	if (0 == strlen(wk_addr) ||
		0 == strlen(wk_odbc) ||
		0 == strlen(wk_dsna) ||
		0 == strlen(wk_user) ||
		0 == strlen(wk_pass)) {
		LOG(em_MSG), "[CheckDB] DB ini Session����������񖳂�(%s)", DbSection);
		return;
	}

	// A�n��B�n�̃f�[�^���I�����C���ɃR�s�[
	WritePrivateProfileString(DbSection, "DBADDLESS", wk_addr, TASKINI_NAME);
	WritePrivateProfileString(DbSection, "ODBCNAME", wk_odbc, TASKINI_NAME);
	WritePrivateProfileString(DbSection, "DSNAME", wk_dsna, TASKINI_NAME);
	WritePrivateProfileString(DbSection, "USERID", wk_user, TASKINI_NAME);
	WritePrivateProfileString(DbSection, "PASSWORD", wk_pass, TASKINI_NAME);
	LOG(em_MSG), "[CheckDB] DB ini Session�������� [%s]��[%s]", Session, DbSection);

}
// 2019.01.23 DB�̃Z�b�V�����؂�ւ��𕡐��w��Ή� ---<<<

//------------------------------------------
// ODBC�̐ݒ�ύX
// char* Session DB�̃Z�b�V����
//------------------------------------------
void MainExec::ChangeODBC_Setting(char const * Session, int nId)
{
	// ��ini�t�@�C���ɐݒ肳��Ă���DB�Z�b�V������ODBC��ݒ肷��
	char wk_addr[128];
	char wk_odbc[128];
	char wk_dsna[128];
	GetPrivateProfileString(Session, "DBADDLESS", "", (char *)wk_addr, sizeof(wk_addr), TASKINI_NAME);
	GetPrivateProfileString(Session, "ODBCNAME",  "", (char *)wk_odbc, sizeof(wk_odbc), TASKINI_NAME);
	GetPrivateProfileString(Session, "DSNAME",    "", (char *)wk_dsna, sizeof(wk_dsna), TASKINI_NAME);
	
	// �f�[�^���S�����邩�`�F�b�N
	if( 0 == strlen(wk_addr) ||
		0 == strlen(wk_odbc) ||
		0 == strlen(wk_dsna) ) {
		LOG(em_MSG), "[CheckDB] DB Session��񖳂�");
		return;
	}
	
	// �h���C�o
	int driverid = GetPrivateProfileInt("DB", "DRIVERID", -1, PCINI_NAME);
	// ===>> �⊪ 2018.10.21 ODBC�h���C�o�敪�ʎw��@�\�ǉ�
	CString sWk;
	sWk.Format("DRIVERID_%d", nId);
	driverid = GetPrivateProfileInt("DB", sWk, driverid, PCINI_NAME);
	// <<===
	if(0 > driverid ) {
		LOG(em_ERR), "ODBC�̃h���C�o�敪�ُ� [%d]", driverid);
		_ASSERT(0);
		return;
	}


	//// ���샂�[�h�ɂ�菈������
		// ODBC�ǉ����[�h (0:KS_START�Ɠ���  1:32bit��ODBC�̂�  2:64bit��ODBC�̂�  -1:�S��(32bit,64bit))
	int addmode = GetPrivateProfileInt("DB", "ADDMODE", 0, PCINI_NAME);
	if( 0 == addmode ) {

		//// ��������
		if( ODBC_Setting(driverid, wk_odbc, wk_addr, wk_dsna) ) {
			LOG(em_MSG), "[CheckDB] ODBC�ݒ菑������ [%s] ODBC=%s, DB=%s, IP=%s", Session, wk_odbc, wk_dsna, wk_addr);

		} else {
			LOG(em_ERR), "[CheckDB] ODBC�ݒ菑�����s [%s] ODBC=%s, DB=%s, IP=%s", Session, wk_odbc, wk_dsna, wk_addr);
		}

	} else {
		//// �ݒ�p�̏�����EXE�ɔC����
		int nRec;				// EXE�̖߂�l
		CString sCmd;			// EXE�R�}���h

		// 32bit�œo�^
		if(-1 == addmode || 1 == addmode) {
			// ===>> �⊪ 2018.10.21 ODBC�h���C�o�敪�ʎw��@�\�ǉ�
			//sCmd.Format("%s_ODBC.exe %s", KS_START, Session);
			sCmd.Format("%s_ODBC.exe %s %d", KS_START, Session, nId);
			// <<===
			nRec = ProcessManager::ProcessExecWait( sCmd, 0);

			if( 0 == nRec ) {
				LOG(em_MSG), "[CheckDB] ODBC�ݒ菑������ [%s]", sCmd);
			} else {
				LOG(em_ERR), "[CheckDB] ODBC�ݒ菑�����s [%s], rec=%d", sCmd, nRec);
			}
		}

		// 64bit�œo�^
		if(-1 == addmode || 2 == addmode) {
			// ===>> �⊪ 2018.10.21 ODBC�h���C�o�敪�ʎw��@�\�ǉ�
			//sCmd.Format("%s_ODBCx64.exe %s", KS_START, Session);
			sCmd.Format("%s_ODBCx64.exe %s %d", KS_START, Session, nId);
			// <<===
			nRec = ProcessManager::ProcessExecWait( sCmd, 0);

			if( 0 == nRec ) {
				LOG(em_MSG), "[CheckDB] ODBC�ݒ菑������ [%s]", sCmd);
			} else {
				LOG(em_ERR), "[CheckDB] ODBC�ݒ菑�����s [%s], rec=%d", sCmd, nRec);
			}
		}
	}
}
/*
//------------------------------------------
// ODBC�����ݒ� (����DNS�̏ꍇ�́A�ύX�ƂȂ�)
// int nDriverID �h���C�o�敪 (0:SQL Server, 1:SQL Native Client, 2:SQL Native Client 10.0)  
// LPCSTR cDNS ����ɕt����閼�O(ODBC�̖��O)
// LPCSTR cDataSource �f�[�^�x�[�X�ڑ��A�h���X
// LPCSTR cDatabase �f�[�^�x�[�X�� (NULL����)
// ���s����^�X�N�� ODBCCP32.lib ��ǉ����鎖
//------------------------------------------
BOOL MainExec::ODBC_Setting(int nDriverID, LPCSTR cDSN, LPCSTR cDataSource, LPCSTR cDataBase)
{
	// ODBC�N���X�ւ̒ǉ��́A��� ( = KizuLibCLI�ŃG���[����������)

// #include <odbcinst.h>	// ���K�v
// ODBCCP32.lib ���K�v

//DSN = ����ɕt����閼�O
//DESCRIPTION = ��������
//TRUSTED_CONNECTION=No �����SQL�F�؂ɏo����͂�

// �e���ڂ̊Ԃ�NULL���K�v
// ���X�g��NULL2�ȏ�K�v


	//// �ڑ�������
	int len;
	char wk[256];
	memset(wk, 0x00, sizeof(wk));


	//// �ǉ�����h���C�o���w��
	char deriver[256];
	memset(deriver, 0x00, sizeof(deriver));
	if(0 == nDriverID) {
		strcpy(deriver, "SQL Server");
	} else if(1 == nDriverID) {
		strcpy(deriver, "SQL Native Client");
	} else if(2 == nDriverID) {
		strcpy(deriver, "SQL Server Native Client 10.0");
	} else {
		return false;
	}

	// �폜�p������
	len  = sprintf(wk		, "Trusted_Connection=no");		len += 1;
	len += sprintf(wk + len , "DSN=%s", cDSN);				
	
	//// ODBC�����폜 (�h���C�o���͉��ł��悢�B�ł��󔒂�NG������)
	//  �� �����ɂ́ASQL Native Client ���g�p����ׁA�h���C�o���قȂ�B���ׁ̈A��x�����Ă���A�ēx�쐬����
	SQLConfigDataSource( NULL, ODBC_REMOVE_SYS_DSN, "SQL Server", wk );

	// �ǉ��p������
	len  = sprintf(wk		, "Trusted_Connection=no");		len += 1;
	len += sprintf(wk + len , "DSN=%s", cDSN);				len += 1;
	len += sprintf(wk + len	, "SERVER=%s", cDataSource);	len += 1;
	len += sprintf(wk + len	, "DataBase=%s", cDataBase);

	//// ODBC�����o�^ (���[�U�[�E�p�X�͐ݒ�s�\�B�ǂ����ɂ��Ă��v���O�����Ŏg���Ƃ��Ɏw�肷��וK�v�Ȃ��H)
	return SQLConfigDataSource( NULL, ODBC_ADD_SYS_DSN, deriver, wk );
}
*/