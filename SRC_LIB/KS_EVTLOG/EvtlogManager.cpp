#include "StdAfx.h"
#include "EvtlogManager.h"
#include <vector>
#include <algorithm>


//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
EvtlogManager::EvtlogManager(void) :
mcls_pLog(NULL)
{
	//// ini�t�@�C���ǂݍ���
	// �C�x���g���O�`�F�b�N����H
	m_nEnable = GetPrivateProfileInt(KS_EVTLOG, "ENABLE", -1, PCINI_NAME);		
	if(m_nEnable == -1) _ASSERT(FALSE);
	// �A�v���P�[�V�����|�b�v�A�b�v�`�F�b�N���Ɉُ�ʒm����@��ID
	m_nApId = GetPrivateProfileInt(KS_EVTLOG, "APID", -1, PCINI_NAME);	
	if(m_nApId == -1) _ASSERT(FALSE);
	// ���b�Z�[�W�ُ펞�Ɉُ�ʒm����@��ID
	m_nStateId = GetPrivateProfileInt(KS_EVTLOG, "STATEID", -1, PCINI_NAME);	
	if(m_nStateId == -1) _ASSERT(FALSE);
	// �`�F�b�N���郁�b�Z�[�W����
	CString  cKey;
	char wk[256];
	memset(m_cMsg, 0x00, sizeof(m_cMsg));
	m_nMsgCheckCnt = 0;
	for(int ii=0; ii<MSG_CHECK_CNT; ii++) {
		cKey.Format("MSG%d", ii+1);
		GetPrivateProfileString(KS_EVTLOG, cKey, "", m_cMsg[ii], sizeof(wk), PCINI_NAME);
		if( 0 == strlen(m_cMsg[ii]) ) {
			break;
		} else {
			m_nMsgCheckCnt += 1;
		}
	}

	m_nUnknownEventIndex = 0;


	//// �����N�����Ԏ擾
	time_t	osBinaryTime = time(NULL);  

	int secPreExec = GetPrivateProfileInt(KS_EVTLOG, "SECPREEXEC", 5*60, PCINI_NAME);
 	m_dayLastCheckTime = (DWORD)max(0, osBinaryTime - secPreExec);
	m_tmRealBoot       = osBinaryTime;
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
EvtlogManager::~EvtlogManager(void)
{
}

//------------------------------------------
// �X���b�h
//------------------------------------------
UINT WINAPI EvtlogManager::MainThread(LPVOID param)
{
	////// ���C���C���X�^���X���o��
	EvtlogManager *p = (EvtlogManager *)param;				// ���N���X�̃C���X�^���X���擾
	p->SetTid();										// ���X���b�hID�Z�b�g


	////////////////////////////////////
	// ���O����
	////////////////////////////////////
	HANDLE evTimeCycleExec = CreateWaitableTimer(NULL, FALSE, NULL);			

	//// �C�x���g���O�`�F�b�N����H
	if( 1 == p->m_nEnable ) {
		// ������^�C�}�[���s
		__int64 ts = 30 * -10000;	// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
		SetWaitableTimer(evTimeCycleExec,   (LARGE_INTEGER *)&ts, PARAM_TIME_CYCLE, NULL, NULL, FALSE);

		// �@���ԏ�����
		if( 0 != p->m_nApId    ) KizuFunction::SetStatus( p->m_nApId,	 true, false);
		if( 0 != p->m_nStateId ) KizuFunction::SetStatus( p->m_nStateId, true, false);
	}


	////////////////////////////////////
	// �ʏ�̃��C���X���b�h����
	////////////////////////////////////
	// WaitForMultipleObjects�C�x���g
	HANDLE	hArray[] = {p->my_evStop,
						evTimeCycleExec
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// �X���b�h�I��
			EV_CYCLE_EXEC,					// ��������s
			EV_END	};


	// ���b�Z�[�W�L���[�ǂݍ���
	pLOG(em_MSG), "[EL] �X���b�h�J�n [0x%X]", p->GetTid());


	CTime dayWk = p->m_dayLastCheckTime ;
	pLOG(em_INF), "[EL] %04d/%02d/%02d %02d:%02d:%02d ����`�F�b�N�J�n",
		dayWk.GetYear(), dayWk.GetMonth(), dayWk.GetDay(), dayWk.GetHour(), dayWk.GetMinute(), dayWk.GetSecond());

	while (1) {
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);

		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// �X���b�h�I��
			pLOG(em_MSG), "[EL] �X���b�h�I�� [0x%X]", p->GetTid());

			// ���̑���~����
			CancelWaitableTimer(evTimeCycleExec);
			CloseHandle(evTimeCycleExec);
			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_CYCLE_EXEC:											// ��������s
			if( ! p->CheckEventLog() ) {
				pLOG(em_ERR), "[EL] �C�x���g���O���`�F�b�N���s�I�I");
			}
			break;

//-----------------------------------------------------------------------------------------------
		default :
			// ���肦�Ȃ��I�I
			break;
		}
	}

	return 0;
}




//�܂�ɁA�C�x���g�̖��ߍ��ݕ����񂪃C�x���g�̑z�肷��t�H�[�}�b�g���*����Ȃ�*���Ƃ�����A
//���ꂪFormatMessage���S�̈���ł���B
//���̂��߁A�Ƃ肠�����_�~�[�̖��ߍ��ݕ������p�ӂ��Ă���
static const char *s_unknown_args[] = {
	"uARG_00",	"uARG_01",	"uARG_02",	"uARG_03",	"uARG_04",	"uARG_05",	"uARG_06",	"uARG_07",	"uARG_08",	"uARG_09",
	"uARG_10",	"uARG_11",	"uARG_12",	"uARG_13",	"uARG_14",	"uARG_15",	"uARG_16",	"uARG_17",	"uARG_18",	"uARG_19",
	"uARG_20",	"uARG_21",	"uARG_22",	"uARG_23",	"uARG_24",	"uARG_25",	"uARG_26",	"uARG_27",	"uARG_28",	"uARG_29",
};

//------------------------------------------
// �����ݕ�����̎擾
//------------------------------------------
static std::vector<const char *> GetArgs(const EVENTLOGRECORD *pBuf) 
{
	//�܂��_�~�[�ŏ������B�܂��A30������Α��v
	std::vector<const char *> pArgs(&s_unknown_args[0], &s_unknown_args[ sizeof(s_unknown_args) / sizeof(s_unknown_args[0])] );

	// �������X�g���擾�B�_�~�[���㏑�����Ă����B
	const char *cp = (const char *)pBuf + (pBuf->StringOffset);
	for(WORD ii=0; ii < min(pBuf->NumStrings, (WORD)pArgs.size()); ii++) {
		pArgs[ii] = cp ;
		cp += strlen(cp) + 1;
	}
	return pArgs;
}


//------------------------------------------
// �C�x���g���O����ُ��Ԃ𔻒�
//------------------------------------------
bool EvtlogManager::CheckEventLog()
{
	bool				bReturn = true;					// ���A���
	HANDLE				hEventLog = NULL;				// �C�x���g���O�n���h��
	EVENTLOGRECORD*		pBuf = NULL;

	char				cEvLogName[256];				// �I�[�v������C�x���g���O�t�@�C��
	bool				bApNg = false;
	bool				bStateNg = false;

	DWORD				dayNewTime = m_dayLastCheckTime;		// ����擾�����ŐV�̃��O����

	DWORD nCount = 0;

	//// �������̈抄�蓖��

	for(int ii=0; ii<em_END; ii++) {
		//// ���O�t�@�C���I��
		memset(cEvLogName, 0x00, sizeof(cEvLogName));
		if		( em_APPLICATION == ii)	{ strcpy(cEvLogName, "Application"); }
		else if	( em_SYSTEM == ii)		{ strcpy(cEvLogName, "System"); }
		else if	( em_SECURITY == ii)	{ strcpy(cEvLogName, "Security"); }


		//// �C�x���g���O�I�[�v��
		hEventLog = OpenEventLog(NULL, cEvLogName);
		if(NULL == hEventLog) {
			LOG(em_ERR), "[EL] �C�x���g���O�I�[�v���G���[[%s, %d]", cEvLogName, GetLastError());
			syslog(59, "[%s Open err_code=%d]", cEvLogName, GetLastError());
			bReturn = false;
			break;
		}


		//// �C�x���g���O�����[�v
		while(1) {
			//// �J��
			if( NULL != pBuf ) {
				GlobalFree(pBuf);
				pBuf = NULL;
			}

			//// �������̈抄�蓖��
			DWORD nBufSize = 1;								// �Ǎ��o�b�t�@�T�C�Y
			pBuf = (EVENTLOGRECORD*)GlobalAlloc(GMEM_FIXED, nBufSize);

			//// �C�x���g���O���ǂݍ��� (�T�C�Y�擾�p)
			DWORD nReadBytes;								// �擾�T�C�Y
			DWORD nNextSize;								// ����擾�T�C�Y
			BOOL bResult = ReadEventLog( hEventLog, 
										EVENTLOG_BACKWARDS_READ | EVENTLOG_SEQUENTIAL_READ,
										//EVENTLOG_FORWARDS_READ | EVENTLOG_SEQUENTIAL_READ,
										0,
										pBuf,
										nBufSize,
										&nReadBytes,
										&nNextSize );
			int api_err = GetLastError();
			if( !bResult && ERROR_INSUFFICIENT_BUFFER != api_err ) {
				if( ERROR_HANDLE_EOF != api_err ) {
					LOG(em_ERR), "[EL] %s �C�x���g���O���Ǎ� ���s=%d", cEvLogName, api_err);
					syslog(59, "[%s ��READ err_code=%d]", cEvLogName, GetLastError());
					bReturn = false;
					goto Exit;
				} else {
					// �S���ǂݍ��݊���
					break;
				}
			}

			//// �������̈�Ċ��蓖��
			GlobalFree(pBuf);
			pBuf = NULL;
			nBufSize = nNextSize;
			pBuf = (EVENTLOGRECORD *)GlobalAlloc(GMEM_FIXED, nBufSize);

			//// �C�x���g���O�ǂݍ���
			bResult = ReadEventLog( hEventLog,
									EVENTLOG_BACKWARDS_READ | EVENTLOG_SEQUENTIAL_READ,
									//EVENTLOG_FORWARDS_READ | EVENTLOG_SEQUENTIAL_READ,
									0,
									pBuf,
									nBufSize,
									&nReadBytes,
									&nNextSize);
			if( !bResult ) {
				LOG(em_ERR), "[EL] %s �C�x���g���O�Ǎ� ���s=%d", cEvLogName, GetLastError());
				syslog(59, "[%s READ err_code=%d]", cEvLogName, GetLastError());
				bReturn = false;
				goto Exit;
			}

			//// ���t�`�F�b�N
			// �V�������̂��珇�ɓǂ�ł����B(= �O�񎞊Ԃ��O�ɂȂ����炻��ł����܂�)
			if (  m_dayLastCheckTime >= pBuf->TimeGenerated ) {
				// �S���ǂݍ���
				break;
			}
			// ���񌟍��̍ŐV���Ԃ�����
			if(dayNewTime < pBuf->TimeGenerated) dayNewTime = pBuf->TimeGenerated;

			//// �\�[�X���E�R���s���[�^���擾
			const char *cp = (const char *)pBuf;
			cp += sizeof(EVENTLOGRECORD);

			const char *pSourceName = cp;
			cp += strlen(cp)+1;

			const char *pComputerName = cp;
			cp += strlen(cp)+1;


			//// ���ߍ��ݕ�����̎擾
			{
				std::vector<const char *> pArgs = GetArgs(pBuf);
				// ���b�Z�[�W�擾
				CString sMessage = 
					DispMessage((EM_EVLOGNO)ii, pSourceName, &pArgs[0], pBuf->EventID);


				//// �A�v���P�[�V�����|�b�v�A�b�v �`�F�b�N
				if( 0 != m_nApId ) {
					CheckApplicationPopup((EM_EVLOGNO)ii, pBuf->TimeWritten, pSourceName, pArgs[0], sMessage);
				}

				//// ���蕶���� �`�F�b�N
				if(  !sMessage.IsEmpty()  && 0 != m_nStateId) {
					CheckStringInEvent((EM_EVLOGNO)ii, pBuf->TimeWritten, pSourceName, pArgs[0], sMessage);
				}

				//�f�o�b�O�p�F�t�@�C���_���v
				if	( em_SECURITY != ii){
					if( (int)GetPrivateProfileInt(KS_EVTLOG, "DumpToText", -1, PCINI_NAME) > 0 ){
						if( FILE *fp = fopen("EvtLogDump.txt", "a") ){
							fprintf(fp, "%u\t%s\n", pBuf->TimeWritten, sMessage );
							fclose(fp);
						}
					}
				}

				nCount++;
			}
		}
		if( NULL != hEventLog ) {
			CloseEventLog(hEventLog);
			hEventLog = NULL;
		}
	}


Exit:
	// ����`�F�b�N���Ԃ��Z�b�g
	m_dayLastCheckTime = dayNewTime;
	CTime dayWk = dayNewTime;
	LOG(em_INF), "[EL] %04d/%02d/%02d %02d:%02d:%02d �܂Ń`�F�b�N����[%d��]",
		dayWk.GetYear(), dayWk.GetMonth(), dayWk.GetDay(), dayWk.GetHour(), dayWk.GetMinute(), dayWk.GetSecond(),nCount );
	
	// �J��
	if( NULL != pBuf ) GlobalFree(pBuf);
	if( NULL != hEventLog ) CloseEventLog(hEventLog);
	return bReturn;
}


//------------------------------------------
// �C�x���g���O����A�A�v���P�[�V�����|�b�v�A�b�v(�A�v�����S��)���������Ă��邩�`�F�b�N�B
// EM_EVLOGNO emEvLogNo		���O�t�@�C�����
// , time_t time			�C�x���g��������
// const char *SourceName  �\�[�X��
// const char *pArg1	   �C�x���g������
// CString &sMessage		�W�J�����C�x���g���O���b�Z�[�W
//------------------------------------------
void EvtlogManager::CheckApplicationPopup(EM_EVLOGNO ii, time_t time, const char *pSourceName, const char *pArg1, const CString &sMessage)
{
	char				cStrAns[256];
	char				cWk[256];					    // ������o�͗p	

	CTime				dayWk;							// �o�^����

	// �����̋N�����܂���Application Popup�͑���ɂ��Ȃ��B
	// �i���O�̃��O�I�t���Ɏ��S�����^�X�N�̃C�x���g���o��h�~�j
	// SECPREEXEC�́A��ʂ̃��O��T�����邽�߂̗p�r�B
	if( time < m_tmRealBoot )
		return ;
	

	//if( (em_SYSTEM == ii) && (0 != m_nApId) ) {
	//	if( 0 == memcmp( pSourceName, "Application Popup", strlen("Application Popup")) ) {
	// Win2008�Ή�
	if( ((em_SYSTEM == ii) && 0 == memcmp( pSourceName, "Application Popup", strlen("Application Popup")))  ||
		((em_APPLICATION == ii) && 0 == memcmp( pSourceName, "Application Error", strlen("Application Error"))) ) {

		memset(cStrAns, 0x00, sizeof(cStrAns));
		

		// .exe������ꍇ
		const char*				cStrData = strstr(pArg1, ".exe"); 
		if ( cStrData != NULL ) {
			int nResult = (int)(cStrData - pArg1);
			if ( (nResult - 1) <= 0 ) {
				// ��������
			} else {
				memset( cWk, 0x00, sizeof( cWk ));
				memcpy( cWk,pArg1, nResult );
				sprintf( cStrAns, "%s.exe", cWk );
			}

		} 
		else {
			cStrData = strstr(pArg1, ":"); 
			if ( cStrData != NULL ) {
				int nResult = (int)(cStrData - pArg1);
				if ( (nResult - 1) <= 0 ) {
					// ��������
				} else {
					memset( cWk, 0x00, sizeof( cWk ));
					memcpy( cWk, pArg1, nResult );
					sprintf( cStrAns, "%s", cWk );
				}
			}
		}
		// �V�X���O
		if(0 != strlen(cStrAns) ) {
			// ���O�o��
			dayWk = time;
			LOG(em_ERR), "[EL] Application Popup���� [%04d/%02d/%02d %02d:%02d:%02d��� %s]",
				dayWk.GetYear(), dayWk.GetMonth(), dayWk.GetDay(), dayWk.GetHour(), dayWk.GetMinute(), dayWk.GetSecond(), cStrAns );
			syslog(SYSNO_EVTLOG_AP_NG, "[%04d/%02d/%02d %02d:%02d:%02d��� %s]",
				dayWk.GetYear(), dayWk.GetMonth(), dayWk.GetDay(), dayWk.GetHour(), dayWk.GetMinute(), dayWk.GetSecond(), cStrAns );
			KizuFunction::SetStatus(m_nApId, false, false);
		}
	}
}

//------------------------------------------
// �C�x���g���O����A�Ď��Ώۃ��b�Z�[�W���܂܂�邩�`�F�b�N�B
// EM_EVLOGNO emEvLogNo		���O�t�@�C�����
// time_t time			�C�x���g��������
// const char *SourceName  �\�[�X��
// const char *pArg1	   �C�x���g������
// CString &sMessage		�W�J�����C�x���g���O���b�Z�[�W
//------------------------------------------
void EvtlogManager::CheckStringInEvent(EM_EVLOGNO emEvLogNo , time_t time, const char *pSourceName, const char *pArg1, const CString &sMessage)
{
	char cEvLogName[256];
	if		( em_APPLICATION == emEvLogNo)	{ strcpy(cEvLogName, "Application"); }
	else if	( em_SYSTEM == emEvLogNo)		{ strcpy(cEvLogName, "System"); }
	else if	( em_SECURITY == emEvLogNo)		{ strcpy(cEvLogName, "Security"); }

	// ���蕶���� �m�F
	for(int ii=0; ii<m_nMsgCheckCnt; ii++ ) {
		int nPos = sMessage.Find( m_cMsg[ii] );
		if ( nPos >= 0 ) {


			CTime dayWk = time;
			LOG(em_ERR), "[EL] MSG�ُ픭�� [%04d/%02d/%02d %02d:%02d:%02d, %s, %s]",
				dayWk.GetYear(), dayWk.GetMonth(), dayWk.GetDay(), dayWk.GetHour(), dayWk.GetMinute(), dayWk.GetSecond(), cEvLogName, m_cMsg[ii] );
			syslog(SYSNO_EVTLOG_MSG_NG, "[%04d/%02d/%02d %02d:%02d:%02d, %s, %s]",
				dayWk.GetYear(), dayWk.GetMonth(), dayWk.GetDay(), dayWk.GetHour(), dayWk.GetMinute(), dayWk.GetSecond(), cEvLogName, m_cMsg[ii] );
			KizuFunction::SetStatus(m_nStateId, false, false);
		}
	}

}

//------------------------------------------
// ���W�X�g������A���b�Z�[�W������DLL��ǂݍ��ݕԂ��iProviderGuid�Łj
// HKEY hSourceKey				�C�x���g���O�T�[�r�X�̃��W�X�g���L�[(Services\\EventLog�j
// out char ModuleName[1024] 	DLL��
// ret �ǂݍ��߂��Ƃ�True
//------------------------------------------
static bool  	ReadEventReg_Guid( HKEY hSourceKey, char ModuleName[1024] )
{
	char cGuid[ 128 ] = "";
	DWORD GuidSize = sizeof(cGuid);

	DWORD lResult = RegQueryValueEx(
		hSourceKey,
		"ProviderGuid",
		NULL,
		NULL,
		(LPBYTE)cGuid,
		&GuidSize);
	if( ERROR_SUCCESS != lResult ) {
		return false;
	}

	HKEY hKeyPublishers = NULL;
	char cKeyName[256] = "";
	sprintf(cKeyName, "%s\\%s",
		"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\WINEVT\\Publishers",
		cGuid);
	lResult = RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		cKeyName,
		0,
		KEY_READ ,
		&hKeyPublishers);

#ifndef _M_X64
	//WINEVT\Publishers�́A64bit���̃��W�X�g���ɂ����Ȃ��B
	//������32bit EXE�Ȃ�AWOW64�̉\��������
	if( ERROR_SUCCESS != lResult ) {
		lResult = RegOpenKeyEx(
			HKEY_LOCAL_MACHINE,
			cKeyName,
			0,
			KEY_READ | 		KEY_WOW64_64KEY,
			&hKeyPublishers);
	}
#endif
	if( ERROR_SUCCESS != lResult ) {
		return false;
	}	
	DWORD ModuleNameSize = 1024;
	lResult = RegQueryValueEx(
		hKeyPublishers,
		"ResourceFileName",
		NULL,
		NULL,
		(LPBYTE)ModuleName,
		&ModuleNameSize);

	RegCloseKey( hKeyPublishers );

	if( ERROR_SUCCESS != lResult ) {
		return false;
	}
	return true;
}

//------------------------------------------
// �C�x���g���O�̃\�[�X������A���b�Z�[�W�̊܂܂�郂�W���[��(DLL)�����擾
// EM_EVLOGNO emEvLogNo		���O�t�@�C�����
// const char *SourceNamef  �\�[�X��
//  ret: ���W���[�����̔z��
//------------------------------------------
static std::vector<CString> GetModuleNameFromSourceNames(
	EvtlogManager::EM_EVLOGNO emEvLogNo,
	const char *SourceName )
	
{
	std::vector<CString>  sModules;		// �֐��߂�l
	CString sExpandName;
	
	HKEY hAppKey = NULL;
	HKEY hSourceKey = NULL;
	

	//// �I�[�v�����W�X�g���L�[
	char cRegKey[1024];			// �I�[�v�����W�X�g���L�[
	memset(cRegKey, 0x00, sizeof(cRegKey));
	if		( EvtlogManager::em_APPLICATION == emEvLogNo)	{ strcpy(cRegKey, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application"); }
	else if	( EvtlogManager::em_SYSTEM == emEvLogNo)		{ strcpy(cRegKey, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\System"); }
	else if	( EvtlogManager::em_SECURITY == emEvLogNo)		{ strcpy(cRegKey, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Security"); }


	//// ���O�p�̃��W�X�g���L�[���I�[�v��
	DWORD lResult = RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		cRegKey,
		0,
		KEY_READ,
		&hAppKey);
	if( ERROR_SUCCESS != lResult ) goto Exit;

	//// �\�[�X�̏�񂪊i�[����Ă��郌�W�X�g�����I�[�v��
	lResult = RegOpenKeyEx(
		hAppKey,
		SourceName,
		0,
		KEY_READ,
		&hSourceKey);
	if( ERROR_SUCCESS != lResult ) goto Exit;

	//// �\�[�X���W���[�������擾
	char ModuleName[1024];			// ���W���[������
	DWORD ModuleNameSize = sizeof(ModuleName);

	//�ŏ��́AGUID�`��
	if( !ReadEventReg_Guid( hSourceKey, ModuleName ) ){

		//�Ȃ���΁AEventMessageFile
		lResult = RegQueryValueEx(
			hSourceKey,
			"EventMessageFile",
			NULL,
			NULL,
			(LPBYTE)ModuleName,
			&ModuleNameSize);

		if( ERROR_SUCCESS != lResult ) {
			goto Exit;
		}
	}

	//// ���ϐ���W�J
	ExpandEnvironmentStrings((const char *)ModuleName, sExpandName.GetBuffer( 65536 ), 65536 );
	sExpandName.ReleaseBuffer();
	

	//�Z�~�R�����ŕ�����DLL���w�肳��Ă���ꍇ�A������
	int nPos = 0;
	do {
		CString rStr = sExpandName.Tokenize(";", nPos );
		if( ! rStr.IsEmpty() ) {
			sModules.push_back( rStr );
		}
	}while(  -1  != nPos );

Exit:
	if( NULL != hSourceKey ) RegCloseKey(hSourceKey);
	if( NULL != hAppKey )	 RegCloseKey(hAppKey);
	return sModules;
}

//------------------------------------------
// ���b�Z�[�W�𕶎��񉻂���i�\������O���L���b�`�����j
// HMODULE hSourceModule�F ���b�Z�[�W�̊܂܂��DLL�̃n���h���i���O��LoadLibrary���邱�Ɓj
// DWORD MessageId		�F ���b�Z�[�WID�i�C�x���g����j
// out char **ppMessage	�F ���ʍ쐬���ꂽ������iLocalAlloc�Ŋm�ۂ����̂ŁA�g�p���LocalFree���邱�Ɓj
// in const char **Args �F ���b�Z�[�W�̈���
//------------------------------------------
static int TryFormatMessage( HMODULE hSourceModule, DWORD MessageId,  char **ppMessage,  const char **Args ){
	
	DWORD nFlag = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_ARGUMENT_ARRAY;
	if( hSourceModule ){
		nFlag |= FORMAT_MESSAGE_FROM_HMODULE;
	}
	else{
		nFlag |= FORMAT_MESSAGE_FROM_SYSTEM;
	}
	__try{
		return FormatMessage(
			nFlag,
			hSourceModule,
			MessageId,
			0,
			(LPSTR)ppMessage,
			0,
			(va_list *)Args);
	}__except(EXCEPTION_EXECUTE_HANDLER){
	}

	//�\������O���o���B���ڃ��g���C��FORMAT_MESSAGE_IGNORE_INSERTS���Ƃ��B
	__try{
		nFlag |= FORMAT_MESSAGE_IGNORE_INSERTS;
		return FormatMessage(
			nFlag,
			hSourceModule,
			MessageId,
			0,
			(LPSTR)ppMessage,
			0,
			(va_list *)Args);
	}__except(EXCEPTION_EXECUTE_HANDLER){
		return -1;
	}
}

//------------------------------------------
// ���b�Z�[�W�𕶎��񉻂���
// EM_EVLOGNO emEvLogNo		���O�t�@�C�����
// const char *SourceName   �C�x���g�̃\�[�X��
// const char **Args		�C�x���g�̈����i���ߍ��ݕ�����j
// DWORD MessageId			�C�x���g�̃��b�Z�[�W�h�c
// ret
//  ���b�Z�[�W������B�װ���̓J��������
//------------------------------------------
CString EvtlogManager::DispMessage(
	EM_EVLOGNO emEvLogNo,
	const char *SourceName, 
	const char **Args, 
	DWORD MessageId) const
{

	//// �\�[�X���W���[�������擾
	std::vector<CString> sDllNames = GetModuleNameFromSourceNames(emEvLogNo, SourceName);

	for(int ii=0; ii<(int)sDllNames.size(); ii++) {
		//// �\�[�X���W���[�������[�h
		HMODULE hSourceModule = LoadLibraryEx(
			sDllNames[ii],
			NULL,
			DONT_RESOLVE_DLL_REFERENCES | LOAD_LIBRARY_AS_DATAFILE);
		
		if( !hSourceModule  ) continue;

		//// ���b�Z�[�W���쐬
		char *pMessage = NULL;
		int nRetc = TryFormatMessage( hSourceModule, MessageId, &pMessage, Args);

		FreeLibrary(hSourceModule);
		
		if( nRetc > 0  && pMessage){
			CString sRet( pMessage, nRetc );
			LocalFree( pMessage );
			return sRet;
		}

	}

	//DLL���Ȃ������Ƃ�����F���邯�ǁA�ϊ��ł��Ȃ������Ƃ��́A���Ȃ��B
	//�@���R�FKernel-PNP�Ƃ��́A���b�Z�[�WDLL��advapi32.dll�Ƃ��������ǁA�t�H�[�}�b�g�͂ł��Ȃ�
	//�@�@�@�@������AFROM_SYSTEM�ŕ����񉻂���ƁA�Ԉ�������b�Z�[�W���߂��Ă���B
	if( sDllNames.empty() ){
		char *pMessage = NULL;
		int nRetc = TryFormatMessage( NULL, MessageId, &pMessage, Args);
		if( nRetc > 0  && pMessage){
			CString sRet( pMessage, nRetc );
			LocalFree( pMessage );
			return sRet;
		}
	}


	//�����܂ł���ƁA�C�x���g�͉����s�\
	//�A�v�����A���C���X�g�[�����ꂽ�Ƃ��B
	//���x���G���[�o���Ă��d���Ȃ��̂ŁA��x�Ŏ~�߂�
	if( std::find( &m_nUnknownEvent[0], &m_nUnknownEvent[SZUNKNOWN], SourceName ) == &m_nUnknownEvent[SZUNKNOWN]){
		m_nUnknownEvent[ m_nUnknownEventIndex ] =  SourceName;
		m_nUnknownEventIndex = (m_nUnknownEventIndex + 1) % SZUNKNOWN ;
		LOG( em_WAR), "[DispMessage] �C�x���g�̕ϊ��s�\ : SRC=%s, ID=%d",  SourceName, MessageId);
	}
	return "";
}



//------------------------------------------
// �ُ��Ԃ���������
//------------------------------------------
void EvtlogManager::ResetErr()
{
	// �@���ԏ�����
	if( 0 != m_nApId    ) KizuFunction::SetStatus( m_nApId,	 true);
	if( 0 != m_nStateId ) KizuFunction::SetStatus( m_nStateId, true);
}
