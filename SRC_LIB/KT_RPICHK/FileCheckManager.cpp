#include "StdAfx.h"
#include "FileCheckManager.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// コンストラクタ
//------------------------------------------
FileCheckManager::FileCheckManager(int nId) :
ThreadManager( GetThreadNameArray("FileCheckMgr", nId) ),
m_nId(nId)
{
	CString	sIniSec;									// 自Iniファイルセクション

	//// ハンドル生成
	m_evCheckStart = CreateEvent(NULL, FALSE, FALSE, NULL);	
	m_evTimerExec  = CreateWaitableTimer(NULL, FALSE, NULL);

	//// Iniファイル取得
	m_nTimeFirst  = GetPrivateProfileInt(KT_RPICHK, "TIME_FIRST", -1, TASKINI_NAME);
	m_nTimeFirst  = m_nTimeFirst * 60 * 1000;								// 分 → ms
	if(0 > m_nTimeFirst) m_nTimeFirst = 100;								// 設定無し時は、100msec
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


	//// 機器状態正常戻し
	m_emRpiFile = DIV_BOOL_NON;
}

//------------------------------------------
// デストラクタ
//------------------------------------------
FileCheckManager::~FileCheckManager(void)
{
	//// ハンドル解放
	CloseHandle(m_evCheckStart);
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int FileCheckManager::ThreadFirst()
{
	//// 事前準備
	if(0 != m_nTimeFirst) {
		__int64 ts = (__int64)m_nTimeFirst * (__int64)-10000;	// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
		//__int64 ts = 100 * -10000; 
		SetWaitableTimer(m_evTimerExec, (LARGE_INTEGER *)&ts, m_nCheckTime, NULL, NULL, FALSE);
	}


	//// イベント設定
	HANDLE hArray[] = { m_evCheckStart,
						m_evTimerExec
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int FileCheckManager::ThreadLast()
{
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void FileCheckManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_EXEC = 0,				// 強制実行
			EV_TIMER_EXEC				// 定周期実行
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_EXEC: 						// 処理実行
	case EV_TIMER_EXEC: 				// 定周期実行
		if(true) {
			LOG(em_MSG), "[%s] RPIファイルチェック開始", my_sThreadName);
			EM_DIV_BOOL emWk = CheckRpiFile();
			LOG(em_MSG), "[%s] RPIファイルチェック完了 [%d]", my_sThreadName, emWk);


			if(DIV_BOOL_NULL != emWk) {
				if(m_emRpiFile != emWk) {
					// 異常 → 正常
					if(DIV_BOOL_TRUE == emWk)  {
						KizuFunction::SetStatus(m_nStateId, true, false);	
					}

					// 正常 → 異常
					if(DIV_BOOL_FALSE == emWk) {
						KizuFunction::SetStatus(m_nStateId, false, false);	
						
						// 再起動要求 全長画像処理復活対策
						if( 0 != strlen(m_cRebootPcname) ) {
							LOG(em_ERR), "[%s] RPIファイル異常 強制再起動[%s]", my_sThreadName, m_cRebootPcname);
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
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}


//------------------------------------------
// RPIファイルチェック
//------------------------------------------
EM_DIV_BOOL FileCheckManager::CheckRpiFile()
{
	bool bWk;
	CString sWk;
	bool bRpiOk = false;		// RPIファイルがあればtrue


	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, INI_DB )) {
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return DIV_BOOL_NULL;
	}



	// 直近xコイルを見て１つもRPIファイルがなければNG
	for(int ii=1; ii<=m_nCheckOkNum; ii++) {
		//================================================
		// 負荷低減処理 1

		// 初回の場合は、全チェック
		// 前回OKの場合は、疵検管理Noが変化した場合に再チェック
		// 前回NGの場合は、先頭のみチェック。

		if( DIV_BOOL_FALSE == m_emRpiFile ) {
			if( 1 < ii ) {
				return DIV_BOOL_NULL;
			}
		}


		//================================================
		// xコ前の疵検管理Noを特定
		char cKizukenNo[SIZE_KIZUKEN_NO];
		memset(cKizukenNo, 0x00, sizeof(cKizukenNo));
		
		CString sSql;

		if( 0 == strlen(m_cRpiCheckSql) ){
#ifndef DB_ADD_M
			sSql.Format( "SELECT MIN(c.疵検管理No) FROM (SELECT TOP %d 疵検管理No FROM COIL_RESULT WHERE 実績格納区分=1 ORDER BY 疵検管理No DESC) AS c", ii);
#else
			sSql.Format( "SELECT MIN(c.管理No) FROM (SELECT TOP %d 管理No FROM T_COIL_RESULT WHERE 実績格納区分=1 ORDER BY 管理No DESC) AS c", ii);
#endif
		}else{
			sSql.Format( m_cRpiCheckSql, ii );
		}
		
		if( ! clsDB.GetSelectKey(sSql, sizeof(cKizukenNo), cKizukenNo) ) return DIV_BOOL_NULL;
		if( 0 >= strlen(cKizukenNo) ) return DIV_BOOL_NULL;


		//================================================
		// 負荷低減処理 2
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
		// RPIファイルがあるか検索
		char cRpiPath[1024];			// RPIパス
		int nCnt=1;
		while(true) {
			//// 検索対象フォルダ特定
			sWk.Format("RPI_FOLDER_SAVE_%d", nCnt);			// TI_RPISAVE 設定のINIファイルを直接参照
			if(0 == m_nRpiCheckMode) {
				GetPrivateProfileString( m_cRpiSaveInsSec, sWk,  "", cRpiPath, sizeof(cRpiPath), m_cRpiSaveIniPath );	
			} else {
				CString	sIniSec;									// 自Iniファイルセクション
				sIniSec.Format("%s_ID_%d", KT_RPICHK, m_nId); 
				GetPrivateProfileString( sIniSec, sWk,  "", cRpiPath, sizeof(cRpiPath), TASKINI_NAME );				
			}

			if( 0 >= strlen(cRpiPath) ) break;
			nCnt ++;

			//// フォルダ内のRPIファイル一覧を取得
			sWk.Format("%s\\%s*.rpi", cRpiPath, cKizukenNo);
			WIN32_FIND_DATA	fd;
			HANDLE			hFind;
			memset(&fd, 0x00, sizeof(fd));
			hFind = FindFirstFile(sWk, &fd);
			if( hFind != INVALID_HANDLE_VALUE ) {
				LOG(em_MSG), "[%s] RPIファイル %dコ前OK (%s)", my_sThreadName, ii, fd.cFileName);
				bRpiOk = true;
			}
			FindClose(hFind);
			if(bRpiOk) break;
		}
		if(bRpiOk) {
			break;
		} else {
			LOG(em_WAR), "[%s] RPIファイル %dコ前無し (%s)", my_sThreadName, ii, cKizukenNo);
		}
	}


	//// 結果
	if(bRpiOk) {
		return DIV_BOOL_TRUE;
	} else {
		LOG(em_ERR), "[%s] RPIファイル 無し", my_sThreadName);
		return DIV_BOOL_FALSE;
	}
}
