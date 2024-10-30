// LogFileManager.cpp: LogFileManager クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogFileManager.h"

#include <direct.h>
#include "..\..\KizuLib\Class\KizuMacro.h"												// マクロ集

using namespace KizuLib;

//------------------------------------------
// コンストラクタ
// LPCSTR filename ログファイル名 (NULL時 ログ出力しない)
// CListBox *list リストボックスコントロール (NULL時 リストボックス表示しない)
// int cycleCnt 最大サイクリックファイル件数
// int backCnt 最大バックアップ履歴数
//------------------------------------------
//LogFileManager::LogFileManager()
LogFileManager::LogFileManager(LPCSTR filename, CListBox *list, int cycleCnt, int backCnt) :
m_cntList(list),
m_nMaxCycleCnt(cycleCnt),
m_nMaxBackCnt(backCnt),
m_nNowRow(0),
m_nNowFileNo(0),
mque_Log(KIZULIB_LOGFILEMANAGER_MAX_QUE)
{
	
	//// 初期化
	InitializeCriticalSection(&m_csOutPut);
	InitializeCriticalSection(&m_csANC);

	//// 出力するしない決定
	m_bFileOut = NULL==filename ? false : true;
	m_bListOut = NULL==list ? false : true;

	//// バックアップ (ファイル出力有り時のみ)
	if ( m_bFileOut ) {
		int ii, jj;
		CString cFileName;
		CString cFileNameNew;

		// ログフォルダ生成
		_mkdir("Log");
		
		// ベース名称決定
		//_getdcwd( 0, m_cBaseFolder.GetBuffer(256) , 256 );	// 現在のフォルダを取得
		char cwk[256];
		GetCurrentDirectory( 256, cwk);
		m_cBaseFolder += cwk;
		m_cBaseFolder += "\\Log\\";
		m_cBaseFile = filename;

		// bk_xxx_@9_0.log ～ bk_xxx_@9_9.logを削除  (最古ファイル削除)
		for( ii=0; ii<=m_nMaxCycleCnt; ii++ ) {
			cFileName.Format("%sbk_%s_@%d_%d.log", m_cBaseFolder, m_cBaseFile, m_nMaxBackCnt, ii );
			remove(cFileName);
		}

		// bk_xxx_@0_0.log ～ bk_xxx_@0_9.log ⇒ bk_xxx_@1_0.log ～ bk_xxx_@1_9.log に変更 (バックアップファイルリネーム)
		for( ii=0; ii<=m_nMaxCycleCnt; ii++ ) {
			for( jj=m_nMaxBackCnt; jj>0; jj-- ) {
				cFileName.Format("%sbk_%s_@%d_%d.log", m_cBaseFolder, m_cBaseFile, jj-1, ii);	// 変換元
				cFileNameNew.Format("%sbk_%s_@%d_%d.log", m_cBaseFolder, m_cBaseFile, jj, ii);	// 変換先
				rename(cFileName, cFileNameNew);
			}
		}

		// xxx_0.log～xxx_9.log ⇒ bk_xxx_@0_0.log ～ bk_xxx_@0_9.log に変更 (最新ファイルをバックアップファイルにリネーム)
		for( ii=0; ii<=m_nMaxCycleCnt; ii++ ) {
			cFileName.Format("%s%s_%d.log", m_cBaseFolder, m_cBaseFile, ii);						// 変換元
			cFileNameNew.Format("%sbk_%s_@0_%d.log", m_cBaseFolder, m_cBaseFile, ii);			// 変換先
			rename(cFileName, cFileNameNew);
		}

		// ログファイルオープン
		cFileName.Format("%s%s_%d.log", m_cBaseFolder, m_cBaseFile, m_nNowFileNo);
		m_stream.open(cFileName, std::ios::out | std::ios::trunc);
		m_cNowFPath = cFileName;
	}

	//// メインスレッド起動
	Start();
}

//------------------------------------------
// デストラクタ
//------------------------------------------
LogFileManager::~LogFileManager()
{
	// メインスレッド停止
	Stop();				// スレッド終了
	mque_Log.Free();	// スレッドキュー破棄
	// ファイルクローズ
	//if (m_stream != NULL) {			// そもそもポインタじゃない
	if ( m_stream.is_open() ) {
		// クローズ
		m_stream.close();
	}
	// 開放
	DeleteCriticalSection(&m_csOutPut);
	DeleteCriticalSection(&m_csANC);
	
}

//------------------------------------------
// 非同期書き込みエントリー
// LogFileManager::LNO no シンボル
// LPCSTR __cBuffer 可変配列メッセージ
//------------------------------------------
void LogFileManager::Write( LogFileManager::LNO no, LPCSTR __cBuffer, ... )
{
	char	cTyp[8];
	char	cMsg[20480];
    SYSTEMTIME time;
    GetLocalTime(&time);

	AutoLock autolock(&m_csANC);

	//// 可変引数より、メッセージ作成
	memset( cMsg, 0x00, sizeof(cMsg));
	va_list	vmarker;
	va_start( vmarker, __cBuffer ); 				// 可変個の引数の初期化
//	_vsnprintf( (char*)cMsg, _TRUNCATE, __cBuffer, vmarker );
	_vsnprintf( (char*)cMsg, sizeof(cMsg)-1, __cBuffer, vmarker );
	va_end( vmarker );								// 可変個の引数のリセット
	
	//// シンボル付加
	memset( cTyp, 0x00, sizeof(cTyp));
	if ( no==em_NON ) {				// 無し
	}else if ( no==em_MSG ) {
		strcpy(cTyp, "☆ "); 
	}else if ( no==em_ERR ) {
		strcpy(cTyp, "★ "); 
	}else if ( no==em_INF ) {
		strcpy(cTyp, "○ "); 
	}else if ( no==em_WAR ) {
		strcpy(cTyp, "● "); 
	}else if ( no==em_FIL ) {
		strcpy(cTyp, "■ "); 
	}

	//// 書き込み用バッファ作成 (使い終わったら必ず開放する事)
	int		nSize;
	nSize = (int)strlen(cMsg) + 256;			// 256は 時刻 や キュー無し時にメッセージ 等で使用する為
	LPSTR pStr = new char [nSize];							// CString を char*に変更 (連続で書き込むと 0.2ms から 0.1msに短縮)
	// メッセージ生成
	sprintf(pStr, "%04d/%02d/%02d %02d:%02d:%02d.%03d %s%s",
		time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, 
		cTyp, cMsg);


	//// キューに登録
	// キュー残容量チェック
	if( 0 == mque_Log.GetFreeCount() ) {			// 空きなし
		// 全ログ開放
		int cnt;
		cnt = AllDeleteQueue();
		// 非同期書き込みログ削除
		memset(pStr, 0x00, nSize);
		sprintf(pStr, "%04d/%02d/%02d %02d:%02d:%02d.%03d %s%s%d", 
			time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, 
			"● ", "ログキューフル!! ログ削除件数=", cnt);
	}
	// キューに登録
	mque_Log.AddToTail( pStr );		// strは取得後に開放
}


//------------------------------------------
// 同期書込み (可変引数対応)
// LogFileManager::LNO no シンボル
// LPCSTR __cBuffer 可変配列メッセージ
//------------------------------------------
void LogFileManager::WriteLock( LogFileManager::LNO no, LPCSTR __cBuffer, ... )
{
	char	cTyp[8];
	char	cMsg[20480];
    SYSTEMTIME time;
    GetLocalTime(&time);

	AutoLock autolock(&m_csANC);

	//// 可変引数より、メッセージ作成
	memset( cMsg, 0x00, sizeof(cMsg));
	va_list	vmarker;
	va_start( vmarker, __cBuffer ); 				// 可変個の引数の初期化
	_vsnprintf( (char*)cMsg, sizeof(cMsg)-1, __cBuffer, vmarker );
	va_end( vmarker );								// 可変個の引数のリセット
	
	//// シンボル付加
	memset( cTyp, 0x00, sizeof(cTyp));
	if ( no==em_NON ) {				// 無し
	}else if ( no==em_MSG ) {
		strcpy(cTyp, "☆ "); 
	}else if ( no==em_ERR ) {
		strcpy(cTyp, "★ "); 
	}else if ( no==em_INF ) {
		strcpy(cTyp, "○ "); 
	}else if ( no==em_WAR ) {
		strcpy(cTyp, "● "); 
	}else if ( no==em_FIL ) {
		strcpy(cTyp, "■ "); 
	}

	//// 書き込み用バッファ作成 (使い終わったら必ず開放する事)
	int		nSize;
	nSize = (int)strlen(cMsg) + 256;			// 256は 時刻 や キュー無し時にメッセージ 等で使用する為
	LPSTR pStr = new char [nSize];							// CString を char*に変更 (連続で書き込むと 0.2ms から 0.1msに短縮)
	// メッセージ生成
	sprintf(pStr, "%04d/%02d/%02d %02d:%02d:%02d.%03d %s%s",
		time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, 
		cTyp, cMsg);


	// リストボックスに追加
	if( m_bListOut) WriteList(pStr);
	// ファイル出力
	if( m_bFileOut) WriteFile(pStr);


	SAFE_DELETE_ARRAY( pStr );
}


//------------------------------------------
// 制御書き込み
//------------------------------------------
void LogFileManager::WriteFile(LPCSTR pStr)
{
	AutoLock autolock(&m_csOutPut);

	////// ファイル出力
	//// ログファイルに書き込み
    m_stream << pStr << std::endl;
	m_nNowRow++;

	if( m_nNowRow > KIZULIB_LOGFILEMANAGER_MAX_ROW_NUM ) {
		m_stream.close();
		m_nNowRow = 0;
		m_nNowFileNo++;
		if( (int)m_nNowFileNo > m_nMaxCycleCnt ) m_nNowFileNo = 0;
		CString cFileName;
		cFileName.Format("%s%s_%d.log", m_cBaseFolder, m_cBaseFile, m_nNowFileNo);
		m_stream.open(cFileName, std::ios::out | std::ios::trunc);
		m_cNowFPath = cFileName;
	}
}

//------------------------------------------
// リストボックスに書き込み
//------------------------------------------
void LogFileManager::WriteList(LPCSTR pStr)
{
	AutoLock autolock(&m_csOutPut);

	///// リストボックスに追加
	try{
		if(m_cntList) {
			m_cntList->AddString(pStr);
			if( m_cntList->GetCount() > KIZULIB_LOGFILEMANAGER_MAX_LISTBOX ) m_cntList->DeleteString(0);
			m_cntList->SetCurSel( m_cntList->GetCount()-1);
		}
	} catch(int code) {
		code = code;
	}
}

//------------------------------------------
// 全キュー強制出力
// 戻り値 出力件数
//------------------------------------------
int LogFileManager::AllGetQueue()
{
	LPSTR pStr = 0;
	int cnt = 0;
	// 全キュー取り出し
	while (true) { 
		pStr = mque_Log.GetFromHead(0);
		if(pStr) {
			// リストボックスに追加
			if( m_bListOut) WriteList(pStr);
			// ファイル出力
			if( m_bFileOut) WriteFile(pStr);
			SAFE_DELETE_ARRAY( pStr ) ;
			cnt++;
		} else {
			break;
		}
	}
	return cnt;
}

//------------------------------------------
// 全キュー強制削除
// 戻り値 削除件数
//------------------------------------------
int LogFileManager::AllDeleteQueue()
{
	LPSTR pStr = 0;
	int cnt = 0;
	// 全キュー取り出し
	while (true) { 
		pStr = mque_Log.GetFromHead(0);
		if(pStr) {
			SAFE_DELETE_ARRAY( pStr );
			cnt++;
		} else {
			break;
		}
	}
	return cnt;
}

//------------------------------------------
// メモ帳起動
//------------------------------------------
void LogFileManager::ShowEdit()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	GetStartupInfo(&si);
	si.dwFlags = 0;

	// メモ帳起動
	char path[MAX_PATH];
	sprintf(path, "notepad.exe %s", GetNowLogPath());
	CreateProcess(NULL, path, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
	CloseHandle(pi.hProcess);	// プロセスハンドルクローズ
	CloseHandle(pi.hThread);	// プロセス・スレッド・ハンドルクローズ
}


//------------------------------------------
// スレッド
//------------------------------------------
UINT WINAPI LogFileManager::MainThread(LPVOID param)
{
	LogFileManager *p = (LogFileManager *)param;
	p->SetTid();
//	p->my_Tid = GetCurrentThreadId();

	// 出力フラグ
	bool blnList = p->m_bListOut;
	bool blnFile = p->m_bFileOut;

	////// イベント作成
	// 停止要求, DB書き込み情報スレッドキュー
	HANDLE	hArray[2] = { p->my_evStop,
						  p->mque_Log.g_evSem};	// WaitForMultipleObjectsイベント

	enum { EV_STOP = WAIT_OBJECT_0,			// 終了通知
		   EV_QUE,							// キュー
		   EV_END};
	
	// メッセージキュー読み込み
	while( TRUE ) {
		LPSTR pStr = 0;
		// シグナル待機
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);
		
		// 出力フラグ制御
		blnList = p->m_bListOut;
		blnFile = p->m_bFileOut;

		switch (ret) {
		case EV_STOP :				// スレッド終了
			//// 全ログ開放		
			p->AllDeleteQueue();
			return 0;

		case EV_QUE:				// ログ書き込み
			pStr = p->mque_Log.GetFromHead();
			if(!pStr) break;

			// リストボックスに追加
			if( blnList) p->WriteList(pStr);
			// ファイル出力
			if( blnFile) p->WriteFile(pStr);
			
			SAFE_DELETE_ARRAY( pStr );
			break;
		default:
			break;
		}
	}
}
