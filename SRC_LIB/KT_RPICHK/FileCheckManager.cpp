#include "StdAfx.h"
#include "FileCheckManager.h"

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
FileCheckManager::FileCheckManager(int nId) :
ThreadManager( GetThreadNameArray("FileCheckMgr", nId) ),
m_nId(nId)
{
	CString	sIniSec;									// ��Ini�t�@�C���Z�N�V����

	//// �n���h������
	m_evCheckStart = CreateEvent(NULL, FALSE, FALSE, NULL);	
	m_evTimerExec  = CreateWaitableTimer(NULL, FALSE, NULL);

	//// Ini�t�@�C���擾
	m_nTimeFirst  = GetPrivateProfileInt(KT_RPICHK, "TIME_FIRST", -1, TASKINI_NAME);
	m_nTimeFirst  = m_nTimeFirst * 60 * 1000;								// �� �� ms
	if(0 > m_nTimeFirst) m_nTimeFirst = 100;								// �ݒ薳�����́A100msec
	m_nCheckTime  = GetPrivateProfileInt(KT_RPICHK, "CYCLE_TIME", 1, TASKINI_NAME);
	m_nCheckTime  *= 60 * 1000;		// [ms]
	m_nCheckOkNum = GetPrivateProfileInt(KT_RPICHK, "CHECK_OK_NUM", 1, TASKINI_NAME);

	GetPrivateProfileString(KT_RPICHK, "RPI_CHECK_SQL", "", m_cRpiCheckSql, sizeof(m_cRpiCheckSql), TASKINI_NAME);

	sIniSec.Format("%s_ID_%d", KT_RPICHK, m_nId); 
	m_nRpiCheckMode = GetPrivateProfileInt(sIniSec, "RPI_CHECK_MODE", 0, TASKINI_NAME); 

	GetPrivateProfileString( sIniSec, "RPISAVE_INI_PATH", "", m_cRpiSaveIniPath, sizeof(m_cRpiSaveIniPath), TASKINI_NAME );
	GetPrivateProfileString( sIniSec, "RPISAVE_INS_SEC",  "", m_cRpiSaveInsSec,  sizeof(m_cRpiSaveInsSec), TASKINI_NAME );
	m_nStateId = GetPrivateProfileInt(sIniSec, "ERR_ID", 0, TASKINI_NAME);
	GetPrivateProfileString( sIniSec, "REBOOT_PCNAME",  "", m_cRebootPcname,  sizeof(m_cRebootPcname), TASKINI_NAME );


	//// �@���Ԑ���߂�
	m_emRpiFile = DIV_BOOL_NON;
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
FileCheckManager::~FileCheckManager(void)
{
	//// �n���h�����
	CloseHandle(m_evCheckStart);
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int FileCheckManager::ThreadFirst()
{
	//// ���O����
	if(0 != m_nTimeFirst) {
		__int64 ts = (__int64)m_nTimeFirst * (__int64)-10000;	// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
		//__int64 ts = 100 * -10000; 
		SetWaitableTimer(m_evTimerExec, (LARGE_INTEGER *)&ts, m_nCheckTime, NULL, NULL, FALSE);
	}


	//// �C�x���g�ݒ�
	HANDLE hArray[] = { m_evCheckStart,
						m_evTimerExec
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int FileCheckManager::ThreadLast()
{
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void FileCheckManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_EXEC = 0,				// �������s
			EV_TIMER_EXEC				// ��������s
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_EXEC: 						// �������s
	case EV_TIMER_EXEC: 				// ��������s
		if(true) {
			LOG(em_MSG), "[%s] RPI�t�@�C���`�F�b�N�J�n", my_sThreadName);
			EM_DIV_BOOL emWk = CheckRpiFile();
			LOG(em_MSG), "[%s] RPI�t�@�C���`�F�b�N���� [%d]", my_sThreadName, emWk);


			if(DIV_BOOL_NULL != emWk) {
				if(m_emRpiFile != emWk) {
					// �ُ� �� ����
					if(DIV_BOOL_TRUE == emWk)  {
						KizuFunction::SetStatus(m_nStateId, true, false);	
					}

					// ���� �� �ُ�
					if(DIV_BOOL_FALSE == emWk) {
						KizuFunction::SetStatus(m_nStateId, false, false);	
						
						// �ċN���v�� �S���摜���������΍�
						if( 0 != strlen(m_cRebootPcname) ) {
							LOG(em_ERR), "[%s] RPI�t�@�C���ُ� �����ċN��[%s]", my_sThreadName, m_cRebootPcname);
							COMMON_QUE  sQue;
							sQue.nEventNo = FACT_KS_SLAVE_06;
							send_mail(KS_SLAVE, m_cRebootPcname, &sQue);
						}
					}
				}
				m_emRpiFile = emWk;
			}
		}
		return;
	
//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}


//------------------------------------------
// RPI�t�@�C���`�F�b�N
//------------------------------------------
EM_DIV_BOOL FileCheckManager::CheckRpiFile()
{
	bool bWk;
	CString sWk;
	bool bRpiOk = false;		// RPI�t�@�C���������true


	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, INI_DB )) {
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return DIV_BOOL_NULL;
	}



	// ����x�R�C�������ĂP��RPI�t�@�C�����Ȃ����NG
	for(int ii=1; ii<=m_nCheckOkNum; ii++) {
		//================================================
		// ���גጸ���� 1

		// ����̏ꍇ�́A�S�`�F�b�N
		// �O��OK�̏ꍇ�́A�r���Ǘ�No���ω������ꍇ�ɍă`�F�b�N
		// �O��NG�̏ꍇ�́A�擪�̂݃`�F�b�N�B

		if( DIV_BOOL_FALSE == m_emRpiFile ) {
			if( 1 < ii ) {
				return DIV_BOOL_NULL;
			}
		}


		//================================================
		// x�R�O���r���Ǘ�No�����
		char cKizukenNo[SIZE_KIZUKEN_NO];
		memset(cKizukenNo, 0x00, sizeof(cKizukenNo));
		
		CString sSql;

		if( 0 == strlen(m_cRpiCheckSql) ){
#ifndef DB_ADD_M
			sSql.Format( "SELECT MIN(c.�r���Ǘ�No) FROM (SELECT TOP %d �r���Ǘ�No FROM COIL_RESULT WHERE ���ъi�[�敪=1 ORDER BY �r���Ǘ�No DESC) AS c", ii);
#else
			sSql.Format( "SELECT MIN(c.�Ǘ�No) FROM (SELECT TOP %d �Ǘ�No FROM T_COIL_RESULT WHERE ���ъi�[�敪=1 ORDER BY �Ǘ�No DESC) AS c", ii);
#endif
		}else{
			sSql.Format( m_cRpiCheckSql, ii );
		}
		
		if( ! clsDB.GetSelectKey(sSql, sizeof(cKizukenNo), cKizukenNo) ) return DIV_BOOL_NULL;
		if( 0 >= strlen(cKizukenNo) ) return DIV_BOOL_NULL;


		//================================================
		// ���גጸ���� 2
		if( DIV_BOOL_TRUE == m_emRpiFile ) {
			if( 1 == ii ) {
				if( 0 == memcmp( cKizukenNo, m_cKizukenNo, SIZE_KIZUKEN_NO ) ) {
					return DIV_BOOL_NULL;
				}
			}
		}
		if( 1 == ii ) {
			memcpy(m_cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);
		}

		//================================================
		// RPI�t�@�C�������邩����
		char cRpiPath[1024];			// RPI�p�X
		int nCnt=1;
		while(true) {
			//// �����Ώۃt�H���_����
			sWk.Format("RPI_FOLDER_SAVE_%d", nCnt);			// TI_RPISAVE �ݒ��INI�t�@�C���𒼐ڎQ��
			if(0 == m_nRpiCheckMode) {
				GetPrivateProfileString( m_cRpiSaveInsSec, sWk,  "", cRpiPath, sizeof(cRpiPath), m_cRpiSaveIniPath );	
			} else {
				CString	sIniSec;									// ��Ini�t�@�C���Z�N�V����
				sIniSec.Format("%s_ID_%d", KT_RPICHK, m_nId); 
				GetPrivateProfileString( sIniSec, sWk,  "", cRpiPath, sizeof(cRpiPath), TASKINI_NAME );				
			}

			if( 0 >= strlen(cRpiPath) ) break;
			nCnt ++;

			//// �t�H���_����RPI�t�@�C���ꗗ���擾
			sWk.Format("%s\\%s*.rpi", cRpiPath, cKizukenNo);
			WIN32_FIND_DATA	fd;
			HANDLE			hFind;
			memset(&fd, 0x00, sizeof(fd));
			hFind = FindFirstFile(sWk, &fd);
			if( hFind != INVALID_HANDLE_VALUE ) {
				LOG(em_MSG), "[%s] RPI�t�@�C�� %d�R�OOK (%s)", my_sThreadName, ii, fd.cFileName);
				bRpiOk = true;
			}
			FindClose(hFind);
			if(bRpiOk) break;
		}
		if(bRpiOk) {
			break;
		} else {
			LOG(em_WAR), "[%s] RPI�t�@�C�� %d�R�O���� (%s)", my_sThreadName, ii, cKizukenNo);
		}
	}


	//// ����
	if(bRpiOk) {
		return DIV_BOOL_TRUE;
	} else {
		LOG(em_ERR), "[%s] RPI�t�@�C�� ����", my_sThreadName);
		return DIV_BOOL_FALSE;
	}
}
