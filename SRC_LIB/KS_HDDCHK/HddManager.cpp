#include "StdAfx.h"
#include "HddManager.h"

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
HddManager::HddManager(void) :
ThreadManager("HddManager")
{
	//// ハンドル生成
	m_evManualExec = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evTimerExec  = CreateWaitableTimer(NULL, FALSE, NULL);

	//// 初期化
	m_bState = true;
	m_sNgMsg = "";
}

//------------------------------------------
// デストラクタ
//------------------------------------------
HddManager::~HddManager(void)
{
	//// ハンドル解放
	CloseHandle(m_evManualExec);
	CloseHandle(m_evTimerExec);
}


//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int HddManager::ThreadFirst()
{
	//// 事前準備
	__int64 ts = 3*1000 * -10000; 
	SetWaitableTimer(m_evTimerExec, (LARGE_INTEGER *)&ts, TIME_EXEC_CYCLE, NULL, NULL, FALSE);

	//// イベント設定
	HANDLE hArray[] = { m_evTimerExec,
						m_evManualExec
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int HddManager::ThreadLast()
{
	// 定周期停止
	CancelWaitableTimer(m_evTimerExec);
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void HddManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_TIMER_EXEC = 0,				// 処理実行 定周期タイマ
			EV_MANUAL_EXEC					// 手動実行
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_TIMER_EXEC: 					// 処理実行 定周期タイマ
	case EV_MANUAL_EXEC:					// 手動実行
		// 20181220 Smart Storage Administrator対応 --->>>
		if (EM_HDDCHK_MODE_SSA == m_nMode) {
			ExecSSA();
		}
		else {
			Exec();
		}
		//Exec();
		// 20181220 Smart Storage Administrator対応 ---<<<
		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 基本

//------------------------------------------
// 関数初期化
//------------------------------------------
void HddManager::Alloc()
{
	//// iniファイル取得
	CString  cKey;
	char wk[256];

	m_nChkNum = 0;
	for(int ii=0; ii<MAX_CHK_TXT; ii++) {
		cKey.Format("CHK_TXT%d", ii+1);
		GetPrivateProfileString(KS_HDDCHK, cKey, "", wk, sizeof(wk), PCINI_NAME);
		if( 0 == strlen(wk) ) {
			break;
		} else {
			m_sChkTxt[m_nChkNum] = wk;
			m_nChkNum += 1;
		}
	}

	m_nOkNum = 0;
	for(int ii=0; ii<MAX_CHK_TXT; ii++) {
		cKey.Format("OK_TXT%d", ii+1);
		GetPrivateProfileString(KS_HDDCHK, cKey, "", wk, sizeof(wk), PCINI_NAME);
		if( 0 == strlen(wk) ) {
			break;
		} else {
			m_sOkTxt[m_nOkNum] = wk;
			m_nOkNum += 1;
		}
	}

	m_nStatusID = GetPrivateProfileInt(KS_HDDCHK, "STATEID", 0, PCINI_NAME);

	// 20181220 Smart Storage Administrator対応 --->>>
	m_nMode = GetPrivateProfileInt(KS_HDDCHK, "MODE", 0, PCINI_NAME);
	// 20181220 Smart Storage Administrator対応 ---<<<


	//// 整合性チェック
	_ASSERT(m_nChkNum);
	_ASSERT(m_nOkNum);
	_ASSERT(m_nStatusID);

	//// 機器状態を正常に戻す
	KizuFunction::SetStatus(m_nStatusID, true, false);
}

//------------------------------------------
// 詳細結果表示
//------------------------------------------
void HddManager::ShowEdit()
{
	// メモ帳起動
	char cPrm[MAX_PATH];
	sprintf(cPrm, "notepad.exe %s", CMD_TXT_NAME);
	ProcessManager::ProcessExec(cPrm, SW_SHOWMAXIMIZED);//SW_NORMAL);
}

//------------------------------------------
// チェック
//------------------------------------------
void HddManager::Exec()
{
	CString sRow;
	CString sWk;
	char	cWk[2048];
	int		nWk;
	bool	bAllOk = true;

	//================================================
	// チェックテキスト出力
	sprintf(cWk, "property -tg=all > %s", CMD_TXT_NAME);
	ProcessManager::ShellExec("raidcmd", cWk, SW_HIDE);
//	ProcessManager::ShellExec("""c:\\Program Files (x86)\\Universal RAID Utility\\cli\\raidcmd""", cWk, SW_HIDE);

	// ファイルが完全に出来上がるまでちょっと待ち
	Sleep(2000);


	//================================================
	// テキストの中身をチェック
	//// オープン
	FILE* fp = fopen(CMD_TXT_NAME, "rt"); 
	if(NULL == fp) {
		LOG(em_ERR), "[%s] HDD状態ファイル オープン失敗 [%s]", my_sThreadName, CMD_TXT_NAME);
		return;
	}

	//// 読み込み
	while (!feof(fp)) {
		char* ret = fgets(cWk, sizeof(cWk), fp);	// 1行取得
		if( 0 == ret ) break;

		//// キー　:　値 の行形式となっている
		sRow = cWk;
		nWk  = 0;
		GetColumn(sRow, nWk, sWk);			// キーを取得

		// チェック対象の行検出	
		for(int ii=0; ii<m_nChkNum; ii++) {
			if( 0 == strcmp(sWk, m_sChkTxt[ii]) ) {
					
				GetColumn(sRow, nWk, sWk);			// 値を取得
				
				// OK認識となる文字を検索
				bool bOk = false;
				for(int jj=0; jj<m_nOkNum; jj++) {
					if( 0 == strcmp(sWk, m_sOkTxt[jj]) ) {
						bOk = true;
						break;
					}
				}

				// HDD異常
				if( ! bOk ) {
					if(m_bState) {
						LOG(em_ERR), "[%s] HDD異常検知！[%s]", my_sThreadName, sWk);
						KizuFunction::SetStatus(m_nStatusID, false, true, sWk);
					}
					m_bState = false;
					m_sNgMsg = sWk;
					bAllOk = false;
				}
			}
		}
	}

	//// クローズ
	fclose(fp);

	//// HDDが全部OKの場合、正常復旧
	if( bAllOk && ! m_bState) {
		LOG(em_INF), "[%s] HDD正常復旧", my_sThreadName);
		KizuFunction::SetStatus(m_nStatusID, true, true, sWk);
		m_bState = true;
		m_sNgMsg = "";
	}

	LOG(em_MSG), "[%s] HDD状態チェック完了 [%s]", my_sThreadName, m_bState?"正常":"異常");
}


//------------------------------------------
// １行のデータから : 単位の文字列を取得
// CString& sVal 1行単位の情報
// int& curPos 区切り位置
// CString& sToken 取得した列の文字列
// 戻り値 復帰情報
//------------------------------------------
bool HddManager::GetColumn(CString& sVal, int& curPos, CString& sToken)
{
	CString sWk;
	sWk = sVal.Tokenize(":", curPos);
	sWk.Trim(_T(" 	\r\n"));					// 前後のスペース、タブを取り除く
	if( 0 == sWk.GetLength() ) return false;

	sToken = sWk;
	return true;
}

// 20181220 Smart Storage Administrator対応 --->>>
//------------------------------------------
// チェック
//------------------------------------------
void HddManager::ExecSSA()
{
	CString sWk;
	char	cWk[2048];
	bool	bAllOk = true;

	static const char cCmd[] = "\"%PROGRAMFILES%\\Smart Storage Administrator\\ssacli\\bin\\ssacli.exe\" ctrl all show config";
	CString sExpcmd;
	int nLen = ExpandEnvironmentStrings(cCmd, sExpcmd.GetBuffer(4096), 4096);		// 環境変数文字列を文字列に展開
	sExpcmd.ReleaseBuffer(nLen);

	//================================================
	// チェックテキスト出力
	CString sResult;
	int res = ProcessManager::ProcessExecWaitAns(sExpcmd, sResult.GetBuffer(4 * 1024 * 1024), SW_HIDE);		// コマンド実行(結果返却)
	sResult.ReleaseBuffer();

	FILE* fw = fopen(CMD_TXT_NAME, "w");
	if (NULL != fw) {
		fwrite(sResult, sResult.GetLength(), 1, fw);
		fclose(fw);
	}
	if (0 == sResult.GetLength()) {
		// 結果なしは異常
		sWk.Format("HDD状態取得失敗[%d]", res);
		if (m_bState) {
			KizuFunction::SetStatus(m_nStatusID, false, true, sWk);
		}
		LOG(em_ERR), "[%s] HDD状態取得失敗！[%s]", my_sThreadName, sWk);

		m_bState = false;
		m_sNgMsg = sWk;
		bAllOk = false;
	}

	LOG(em_INF), "[%s] HDD状態ファイル出力 [%s]", my_sThreadName, sExpcmd);

	//================================================
	// テキストの中身をチェック
	//// オープン
	FILE* fp = fopen(CMD_TXT_NAME, "rt");
	if (NULL == fp) {
		LOG(em_ERR), "[%s] HDD状態ファイル オープン失敗 [%s]", my_sThreadName, CMD_TXT_NAME);
		return;
	}

	//// 読み込み
	while (!feof(fp)) {
		char* ret = fgets(cWk, sizeof(cWk), fp);	// 1行取得
		if (0 == ret) break;

		sWk = cWk;
		sWk.Trim(_T(" 	\r\n"));					// 前後のスペース、タブを取り除く

		// チェック対象の行検出	
		for (int ii = 0; ii < m_nChkNum; ii++) {
			//// チェックする文字列をサーチ。
			if (NULL != strstr(sWk, m_sChkTxt[ii])) {
				bool bOk = false;
				// OK認識となる文字を検索
				for (int jj = 0; jj<m_nOkNum; jj++) {
					if (NULL != strstr(sWk, m_sOkTxt[jj])) {
						bOk = true;
						break;
					}
				}

				// HDD異常
				if (!bOk) {
					if (m_bState) {
						KizuFunction::SetStatus(m_nStatusID, false, true, sWk);
					}
					LOG(em_ERR), "[%s] HDD異常検知！[%s]", my_sThreadName, sWk);

					m_bState = false;
					m_sNgMsg = sWk;
					bAllOk = false;
				}
			}
		}

	}

	//// クローズ
	fclose(fp);

	//// HDDが全部OKの場合、正常復旧
	if (bAllOk && !m_bState) {
		LOG(em_INF), "[%s] HDD正常復旧", my_sThreadName);
		KizuFunction::SetStatus(m_nStatusID, true, true, sWk);
		m_bState = true;
		m_sNgMsg = "";
	}

	LOG(em_MSG), "[%s] HDD状態チェック完了 [%s]", my_sThreadName, m_bState ? "正常" : "異常");
}
// 20181220 Smart Storage Administrator対応 ---<<<
