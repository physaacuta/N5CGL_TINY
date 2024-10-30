#include "StdAfx.h"
#include "EvtlogManager.h"
#include <vector>
#include <algorithm>


//------------------------------------------
// コンストラクタ
//------------------------------------------
EvtlogManager::EvtlogManager(void) :
mcls_pLog(NULL)
{
	//// iniファイル読み込み
	// イベントログチェックする？
	m_nEnable = GetPrivateProfileInt(KS_EVTLOG, "ENABLE", -1, PCINI_NAME);		
	if(m_nEnable == -1) _ASSERT(FALSE);
	// アプリケーションポップアップチェック時に異常通知する機器ID
	m_nApId = GetPrivateProfileInt(KS_EVTLOG, "APID", -1, PCINI_NAME);	
	if(m_nApId == -1) _ASSERT(FALSE);
	// メッセージ異常時に異常通知する機器ID
	m_nStateId = GetPrivateProfileInt(KS_EVTLOG, "STATEID", -1, PCINI_NAME);	
	if(m_nStateId == -1) _ASSERT(FALSE);
	// チェックするメッセージ名称
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


	//// 初期起動時間取得
	time_t	osBinaryTime = time(NULL);  

	int secPreExec = GetPrivateProfileInt(KS_EVTLOG, "SECPREEXEC", 5*60, PCINI_NAME);
 	m_dayLastCheckTime = (DWORD)max(0, osBinaryTime - secPreExec);
	m_tmRealBoot       = osBinaryTime;
}

//------------------------------------------
// デストラクタ
//------------------------------------------
EvtlogManager::~EvtlogManager(void)
{
}

//------------------------------------------
// スレッド
//------------------------------------------
UINT WINAPI EvtlogManager::MainThread(LPVOID param)
{
	////// メインインスタンス取り出し
	EvtlogManager *p = (EvtlogManager *)param;				// 自クラスのインスタンスを取得
	p->SetTid();										// 自スレッドIDセット


	////////////////////////////////////
	// 事前準備
	////////////////////////////////////
	HANDLE evTimeCycleExec = CreateWaitableTimer(NULL, FALSE, NULL);			

	//// イベントログチェックする？
	if( 1 == p->m_nEnable ) {
		// 定周期タイマー実行
		__int64 ts = 30 * -10000;	// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
		SetWaitableTimer(evTimeCycleExec,   (LARGE_INTEGER *)&ts, PARAM_TIME_CYCLE, NULL, NULL, FALSE);

		// 機器状態初期化
		if( 0 != p->m_nApId    ) KizuFunction::SetStatus( p->m_nApId,	 true, false);
		if( 0 != p->m_nStateId ) KizuFunction::SetStatus( p->m_nStateId, true, false);
	}


	////////////////////////////////////
	// 通常のメインスレッド処理
	////////////////////////////////////
	// WaitForMultipleObjectsイベント
	HANDLE	hArray[] = {p->my_evStop,
						evTimeCycleExec
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// スレッド終了
			EV_CYCLE_EXEC,					// 定周期実行
			EV_END	};


	// メッセージキュー読み込み
	pLOG(em_MSG), "[EL] スレッド開始 [0x%X]", p->GetTid());


	CTime dayWk = p->m_dayLastCheckTime ;
	pLOG(em_INF), "[EL] %04d/%02d/%02d %02d:%02d:%02d からチェック開始",
		dayWk.GetYear(), dayWk.GetMonth(), dayWk.GetDay(), dayWk.GetHour(), dayWk.GetMinute(), dayWk.GetSecond());

	while (1) {
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);

		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// スレッド終了
			pLOG(em_MSG), "[EL] スレッド終了 [0x%X]", p->GetTid());

			// その他停止処理
			CancelWaitableTimer(evTimeCycleExec);
			CloseHandle(evTimeCycleExec);
			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_CYCLE_EXEC:											// 定周期実行
			if( ! p->CheckEventLog() ) {
				pLOG(em_ERR), "[EL] イベントログ情報チェック失敗！！");
			}
			break;

//-----------------------------------------------------------------------------------------------
		default :
			// ありえない！！
			break;
		}
	}

	return 0;
}




//まれに、イベントの埋め込み文字列がイベントの想定するフォーマットより*足りない*ことがあり、
//それがFormatMessage死亡の一因である。
//そのため、とりあえずダミーの埋め込み文字列を用意しておく
static const char *s_unknown_args[] = {
	"uARG_00",	"uARG_01",	"uARG_02",	"uARG_03",	"uARG_04",	"uARG_05",	"uARG_06",	"uARG_07",	"uARG_08",	"uARG_09",
	"uARG_10",	"uARG_11",	"uARG_12",	"uARG_13",	"uARG_14",	"uARG_15",	"uARG_16",	"uARG_17",	"uARG_18",	"uARG_19",
	"uARG_20",	"uARG_21",	"uARG_22",	"uARG_23",	"uARG_24",	"uARG_25",	"uARG_26",	"uARG_27",	"uARG_28",	"uARG_29",
};

//------------------------------------------
// 埋込み文字列の取得
//------------------------------------------
static std::vector<const char *> GetArgs(const EVENTLOGRECORD *pBuf) 
{
	//まずダミーで初期化。まあ、30もあれば大丈夫
	std::vector<const char *> pArgs(&s_unknown_args[0], &s_unknown_args[ sizeof(s_unknown_args) / sizeof(s_unknown_args[0])] );

	// 引数リストを取得。ダミーを上書きしていく。
	const char *cp = (const char *)pBuf + (pBuf->StringOffset);
	for(WORD ii=0; ii < min(pBuf->NumStrings, (WORD)pArgs.size()); ii++) {
		pArgs[ii] = cp ;
		cp += strlen(cp) + 1;
	}
	return pArgs;
}


//------------------------------------------
// イベントログから異常状態を判定
//------------------------------------------
bool EvtlogManager::CheckEventLog()
{
	bool				bReturn = true;					// 復帰情報
	HANDLE				hEventLog = NULL;				// イベントログハンドル
	EVENTLOGRECORD*		pBuf = NULL;

	char				cEvLogName[256];				// オープンするイベントログファイル
	bool				bApNg = false;
	bool				bStateNg = false;

	DWORD				dayNewTime = m_dayLastCheckTime;		// 今回取得した最新のログ時間

	DWORD nCount = 0;

	//// メモリ領域割り当て

	for(int ii=0; ii<em_END; ii++) {
		//// ログファイル選択
		memset(cEvLogName, 0x00, sizeof(cEvLogName));
		if		( em_APPLICATION == ii)	{ strcpy(cEvLogName, "Application"); }
		else if	( em_SYSTEM == ii)		{ strcpy(cEvLogName, "System"); }
		else if	( em_SECURITY == ii)	{ strcpy(cEvLogName, "Security"); }


		//// イベントログオープン
		hEventLog = OpenEventLog(NULL, cEvLogName);
		if(NULL == hEventLog) {
			LOG(em_ERR), "[EL] イベントログオープンエラー[%s, %d]", cEvLogName, GetLastError());
			syslog(59, "[%s Open err_code=%d]", cEvLogName, GetLastError());
			bReturn = false;
			break;
		}


		//// イベントログ分ループ
		while(1) {
			//// 開放
			if( NULL != pBuf ) {
				GlobalFree(pBuf);
				pBuf = NULL;
			}

			//// メモリ領域割り当て
			DWORD nBufSize = 1;								// 読込バッファサイズ
			pBuf = (EVENTLOGRECORD*)GlobalAlloc(GMEM_FIXED, nBufSize);

			//// イベントログ仮読み込み (サイズ取得用)
			DWORD nReadBytes;								// 取得サイズ
			DWORD nNextSize;								// 次回取得サイズ
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
					LOG(em_ERR), "[EL] %s イベントログ仮読込 失敗=%d", cEvLogName, api_err);
					syslog(59, "[%s 仮READ err_code=%d]", cEvLogName, GetLastError());
					bReturn = false;
					goto Exit;
				} else {
					// 全部読み込み完了
					break;
				}
			}

			//// メモリ領域再割り当て
			GlobalFree(pBuf);
			pBuf = NULL;
			nBufSize = nNextSize;
			pBuf = (EVENTLOGRECORD *)GlobalAlloc(GMEM_FIXED, nBufSize);

			//// イベントログ読み込み
			bResult = ReadEventLog( hEventLog,
									EVENTLOG_BACKWARDS_READ | EVENTLOG_SEQUENTIAL_READ,
									//EVENTLOG_FORWARDS_READ | EVENTLOG_SEQUENTIAL_READ,
									0,
									pBuf,
									nBufSize,
									&nReadBytes,
									&nNextSize);
			if( !bResult ) {
				LOG(em_ERR), "[EL] %s イベントログ読込 失敗=%d", cEvLogName, GetLastError());
				syslog(59, "[%s READ err_code=%d]", cEvLogName, GetLastError());
				bReturn = false;
				goto Exit;
			}

			//// 日付チェック
			// 新しいものから順に読んでいく。(= 前回時間より前になったらそれでおしまい)
			if (  m_dayLastCheckTime >= pBuf->TimeGenerated ) {
				// 全部読み込んだ
				break;
			}
			// 今回検索の最新時間を決定
			if(dayNewTime < pBuf->TimeGenerated) dayNewTime = pBuf->TimeGenerated;

			//// ソース名・コンピュータ名取得
			const char *cp = (const char *)pBuf;
			cp += sizeof(EVENTLOGRECORD);

			const char *pSourceName = cp;
			cp += strlen(cp)+1;

			const char *pComputerName = cp;
			cp += strlen(cp)+1;


			//// 埋め込み文字列の取得
			{
				std::vector<const char *> pArgs = GetArgs(pBuf);
				// メッセージ取得
				CString sMessage = 
					DispMessage((EM_EVLOGNO)ii, pSourceName, &pArgs[0], pBuf->EventID);


				//// アプリケーションポップアップ チェック
				if( 0 != m_nApId ) {
					CheckApplicationPopup((EM_EVLOGNO)ii, pBuf->TimeWritten, pSourceName, pArgs[0], sMessage);
				}

				//// 特定文字列 チェック
				if(  !sMessage.IsEmpty()  && 0 != m_nStateId) {
					CheckStringInEvent((EM_EVLOGNO)ii, pBuf->TimeWritten, pSourceName, pArgs[0], sMessage);
				}

				//デバッグ用：ファイルダンプ
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
	// 今回チェック時間をセット
	m_dayLastCheckTime = dayNewTime;
	CTime dayWk = dayNewTime;
	LOG(em_INF), "[EL] %04d/%02d/%02d %02d:%02d:%02d までチェック完了[%d件]",
		dayWk.GetYear(), dayWk.GetMonth(), dayWk.GetDay(), dayWk.GetHour(), dayWk.GetMinute(), dayWk.GetSecond(),nCount );
	
	// 開放
	if( NULL != pBuf ) GlobalFree(pBuf);
	if( NULL != hEventLog ) CloseEventLog(hEventLog);
	return bReturn;
}


//------------------------------------------
// イベントログから、アプリケーションポップアップ(アプリ死亡時)が発生しているかチェック。
// EM_EVLOGNO emEvLogNo		ログファイル種類
// , time_t time			イベント発生日時
// const char *SourceName  ソース名
// const char *pArg1	   イベント第一引数
// CString &sMessage		展開したイベントログメッセージ
//------------------------------------------
void EvtlogManager::CheckApplicationPopup(EM_EVLOGNO ii, time_t time, const char *pSourceName, const char *pArg1, const CString &sMessage)
{
	char				cStrAns[256];
	char				cWk[256];					    // 文字列出力用	

	CTime				dayWk;							// 登録時間

	// 自分の起動よりまえのApplication Popupは相手にしない。
	// （直前のログオフ時に死亡したタスクのイベント検出を防止）
	// SECPREEXECは、一般のログを探索するための用途。
	if( time < m_tmRealBoot )
		return ;
	

	//if( (em_SYSTEM == ii) && (0 != m_nApId) ) {
	//	if( 0 == memcmp( pSourceName, "Application Popup", strlen("Application Popup")) ) {
	// Win2008対応
	if( ((em_SYSTEM == ii) && 0 == memcmp( pSourceName, "Application Popup", strlen("Application Popup")))  ||
		((em_APPLICATION == ii) && 0 == memcmp( pSourceName, "Application Error", strlen("Application Error"))) ) {

		memset(cStrAns, 0x00, sizeof(cStrAns));
		

		// .exeがある場合
		const char*				cStrData = strstr(pArg1, ".exe"); 
		if ( cStrData != NULL ) {
			int nResult = (int)(cStrData - pArg1);
			if ( (nResult - 1) <= 0 ) {
				// 処理無し
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
					// 処理無し
				} else {
					memset( cWk, 0x00, sizeof( cWk ));
					memcpy( cWk, pArg1, nResult );
					sprintf( cStrAns, "%s", cWk );
				}
			}
		}
		// シスログ
		if(0 != strlen(cStrAns) ) {
			// ログ出力
			dayWk = time;
			LOG(em_ERR), "[EL] Application Popup発見 [%04d/%02d/%02d %02d:%02d:%02d･･･ %s]",
				dayWk.GetYear(), dayWk.GetMonth(), dayWk.GetDay(), dayWk.GetHour(), dayWk.GetMinute(), dayWk.GetSecond(), cStrAns );
			syslog(SYSNO_EVTLOG_AP_NG, "[%04d/%02d/%02d %02d:%02d:%02d･･･ %s]",
				dayWk.GetYear(), dayWk.GetMonth(), dayWk.GetDay(), dayWk.GetHour(), dayWk.GetMinute(), dayWk.GetSecond(), cStrAns );
			KizuFunction::SetStatus(m_nApId, false, false);
		}
	}
}

//------------------------------------------
// イベントログから、監視対象メッセージが含まれるかチェック。
// EM_EVLOGNO emEvLogNo		ログファイル種類
// time_t time			イベント発生日時
// const char *SourceName  ソース名
// const char *pArg1	   イベント第一引数
// CString &sMessage		展開したイベントログメッセージ
//------------------------------------------
void EvtlogManager::CheckStringInEvent(EM_EVLOGNO emEvLogNo , time_t time, const char *pSourceName, const char *pArg1, const CString &sMessage)
{
	char cEvLogName[256];
	if		( em_APPLICATION == emEvLogNo)	{ strcpy(cEvLogName, "Application"); }
	else if	( em_SYSTEM == emEvLogNo)		{ strcpy(cEvLogName, "System"); }
	else if	( em_SECURITY == emEvLogNo)		{ strcpy(cEvLogName, "Security"); }

	// 特定文字列 確認
	for(int ii=0; ii<m_nMsgCheckCnt; ii++ ) {
		int nPos = sMessage.Find( m_cMsg[ii] );
		if ( nPos >= 0 ) {


			CTime dayWk = time;
			LOG(em_ERR), "[EL] MSG異常発見 [%04d/%02d/%02d %02d:%02d:%02d, %s, %s]",
				dayWk.GetYear(), dayWk.GetMonth(), dayWk.GetDay(), dayWk.GetHour(), dayWk.GetMinute(), dayWk.GetSecond(), cEvLogName, m_cMsg[ii] );
			syslog(SYSNO_EVTLOG_MSG_NG, "[%04d/%02d/%02d %02d:%02d:%02d, %s, %s]",
				dayWk.GetYear(), dayWk.GetMonth(), dayWk.GetDay(), dayWk.GetHour(), dayWk.GetMinute(), dayWk.GetSecond(), cEvLogName, m_cMsg[ii] );
			KizuFunction::SetStatus(m_nStateId, false, false);
		}
	}

}

//------------------------------------------
// レジストリから、メッセージを持つDLLを読み込み返す（ProviderGuid版）
// HKEY hSourceKey				イベントログサービスのレジストリキー(Services\\EventLog）
// out char ModuleName[1024] 	DLL名
// ret 読み込めたときTrue
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
	//WINEVT\Publishersは、64bit側のレジストリにしかない。
	//自分が32bit EXEなら、WOW64の可能性がある
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
// イベントログのソース名から、メッセージの含まれるモジュール(DLL)名を取得
// EM_EVLOGNO emEvLogNo		ログファイル種類
// const char *SourceNamef  ソース名
//  ret: モジュール名の配列
//------------------------------------------
static std::vector<CString> GetModuleNameFromSourceNames(
	EvtlogManager::EM_EVLOGNO emEvLogNo,
	const char *SourceName )
	
{
	std::vector<CString>  sModules;		// 関数戻り値
	CString sExpandName;
	
	HKEY hAppKey = NULL;
	HKEY hSourceKey = NULL;
	

	//// オープンレジストリキー
	char cRegKey[1024];			// オープンレジストリキー
	memset(cRegKey, 0x00, sizeof(cRegKey));
	if		( EvtlogManager::em_APPLICATION == emEvLogNo)	{ strcpy(cRegKey, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application"); }
	else if	( EvtlogManager::em_SYSTEM == emEvLogNo)		{ strcpy(cRegKey, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\System"); }
	else if	( EvtlogManager::em_SECURITY == emEvLogNo)		{ strcpy(cRegKey, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Security"); }


	//// ログ用のレジストリキーをオープン
	DWORD lResult = RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		cRegKey,
		0,
		KEY_READ,
		&hAppKey);
	if( ERROR_SUCCESS != lResult ) goto Exit;

	//// ソースの情報が格納されているレジストリをオープン
	lResult = RegOpenKeyEx(
		hAppKey,
		SourceName,
		0,
		KEY_READ,
		&hSourceKey);
	if( ERROR_SUCCESS != lResult ) goto Exit;

	//// ソースモジュール名を取得
	char ModuleName[1024];			// モジュール名称
	DWORD ModuleNameSize = sizeof(ModuleName);

	//最初は、GUID形式
	if( !ReadEventReg_Guid( hSourceKey, ModuleName ) ){

		//なければ、EventMessageFile
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

	//// 環境変数を展開
	ExpandEnvironmentStrings((const char *)ModuleName, sExpandName.GetBuffer( 65536 ), 65536 );
	sExpandName.ReleaseBuffer();
	

	//セミコロンで複数のDLLが指定されている場合、分ける
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
// メッセージを文字列化する（構造化例外がキャッチされる）
// HMODULE hSourceModule： メッセージの含まれるDLLのハンドル（事前にLoadLibraryすること）
// DWORD MessageId		： メッセージID（イベントから）
// out char **ppMessage	： 結果作成された文字列（LocalAllocで確保されるので、使用後はLocalFreeすること）
// in const char **Args ： メッセージの引数
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

	//構造化例外が出た。二回目リトライはFORMAT_MESSAGE_IGNORE_INSERTSつけとく。
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
// メッセージを文字列化する
// EM_EVLOGNO emEvLogNo		ログファイル種類
// const char *SourceName   イベントのソース名
// const char **Args		イベントの引数（埋め込み文字列）
// DWORD MessageId			イベントのメッセージＩＤ
// ret
//  メッセージ文字列。ｴﾗｰ時はカラ文字列
//------------------------------------------
CString EvtlogManager::DispMessage(
	EM_EVLOGNO emEvLogNo,
	const char *SourceName, 
	const char **Args, 
	DWORD MessageId) const
{

	//// ソースモジュール名を取得
	std::vector<CString> sDllNames = GetModuleNameFromSourceNames(emEvLogNo, SourceName);

	for(int ii=0; ii<(int)sDllNames.size(); ii++) {
		//// ソースモジュールをロード
		HMODULE hSourceModule = LoadLibraryEx(
			sDllNames[ii],
			NULL,
			DONT_RESOLVE_DLL_REFERENCES | LOAD_LIBRARY_AS_DATAFILE);
		
		if( !hSourceModule  ) continue;

		//// メッセージを作成
		char *pMessage = NULL;
		int nRetc = TryFormatMessage( hSourceModule, MessageId, &pMessage, Args);

		FreeLibrary(hSourceModule);
		
		if( nRetc > 0  && pMessage){
			CString sRet( pMessage, nRetc );
			LocalFree( pMessage );
			return sRet;
		}

	}

	//DLLがなかったとき限定：あるけど、変換できなかったときは、やらない。
	//　理由：Kernel-PNPとかは、メッセージDLLをadvapi32.dllとか言うけど、フォーマットはできない
	//　　　　これを、FROM_SYSTEMで文字列化すると、間違ったメッセージが戻ってくる。
	if( sDllNames.empty() ){
		char *pMessage = NULL;
		int nRetc = TryFormatMessage( NULL, MessageId, &pMessage, Args);
		if( nRetc > 0  && pMessage){
			CString sRet( pMessage, nRetc );
			LocalFree( pMessage );
			return sRet;
		}
	}


	//ここまでくると、イベントは解決不能
	//アプリがアンインストールされたとか。
	//何度もエラー出しても仕方ないので、一度で止める
	if( std::find( &m_nUnknownEvent[0], &m_nUnknownEvent[SZUNKNOWN], SourceName ) == &m_nUnknownEvent[SZUNKNOWN]){
		m_nUnknownEvent[ m_nUnknownEventIndex ] =  SourceName;
		m_nUnknownEventIndex = (m_nUnknownEventIndex + 1) % SZUNKNOWN ;
		LOG( em_WAR), "[DispMessage] イベントの変換不能 : SRC=%s, ID=%d",  SourceName, MessageId);
	}
	return "";
}



//------------------------------------------
// 異常状態を解除する
//------------------------------------------
void EvtlogManager::ResetErr()
{
	// 機器状態初期化
	if( 0 != m_nApId    ) KizuFunction::SetStatus( m_nApId,	 true);
	if( 0 != m_nStateId ) KizuFunction::SetStatus( m_nStateId, true);
}
