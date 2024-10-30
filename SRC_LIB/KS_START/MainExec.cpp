#include "StdAfx.h"
#include "MainExec.h"

//------------------------------------------
// コンストラクタ
// LogFileManager* pLog ログクラス
// bool* bCancel 強制キャンセルポインタ
//------------------------------------------
MainExec::MainExec(LogFileManager* pLog, bool* bCancel) :
m_nIniNowID(-1),
m_pCancel(bCancel),
mcls_pLog(pLog)
{
}

//------------------------------------------
// デストラクタ
//------------------------------------------
MainExec::~MainExec(void)
{
}


//------------------------------------------
// メイン処理
//------------------------------------------
void MainExec::DoExec()
{
	CString		cKey;				
	int			nReTryCnt;				// 現在のリトライ回数
	int			nReTryMax;				// 最大リトライ回数

	//// フォルダーコピー
	LOG(em_MSG), "フォルダコピー開始");
	nReTryMax = GetPrivateProfileInt("FOLDERCOPY", "RETRY", 0, PCINI_NAME);
	nReTryCnt = 0;
	while(true) {
		if( *m_pCancel ) return;
		if( ExecFolderCopy() ) break;
		
		// リトライチェック
		if( 0 < nReTryMax && nReTryMax <= nReTryCnt) {
			LOG(em_ERR), "指定リトライ回数に到達した為、フォルダコピー中断！");
			break;
		}
		if( 0 < nReTryMax) nReTryCnt += 1;

		LOG(em_ERR), "フォルダコピー待ち・・・");
		Sleep(5000);
	}

	//// ファイルコピー (即時失敗時は、統括が異常のはずだから、成功するまで待つ)
	LOG(em_MSG), "ファイルコピー開始");
	nReTryMax = GetPrivateProfileInt("FILECOPY", "RETRY", 0, PCINI_NAME);
	nReTryCnt = 0;
	while(true) {
		if( *m_pCancel ) return;
		if( ExecFileCopy() ) break;

		// リトライチェック
		if( 0 < nReTryMax && nReTryMax <= nReTryCnt) {
			LOG(em_ERR), "指定リトライ回数に到達した為、フォルダコピー中断！");
			break;
		}
		if( 0 < nReTryMax) nReTryCnt += 1;

		LOG(em_ERR), "ファイルコピー待ち・・・");
		Sleep(5000);
	}

	//// WakeOnLan 実行
	LOG(em_MSG), "Wake On Lan 実行");
	MagicPacket::Send(PCINI_NAME); 


	//// 機能切替
	// 現機能ID取り込み
	LOG(em_MSG), "現機能ID取り込み開始");
	m_nIniNowID = GetPrivateProfileInt("NOWID", "ID", -1, PCINI_NAME);
	if(0 > m_nIniNowID ) {
		LOG(em_ERR), "現機能ID異常 [%d]", m_nIniNowID);
		_ASSERT(0);
		return;
	}
	// 機能切替元パス取得
	LOG(em_MSG), "設定切り替え開始 現機能ID=%d", m_nIniNowID);
	char		cFrom[MAX_PATH];
	cKey.Format("NAME_%d", m_nIniNowID); 
	GetPrivateProfileString("KIZUTASKINI", cKey, "", cFrom, MAX_PATH, PCINI_NAME);
	if(0 == strlen(cFrom) ) {
		LOG(em_WAR), "KizuTask.ini切替元パス 無し [%s]", cFrom);
		//return; 無しの場合もある
	} else {
		// 切替
		if( ! CopyFile(cFrom, TASKINI_NAME, FALSE) ) {
			LOG(em_ERR), "KizuTask.ini切替異常 [%s→%s]", cFrom, TASKINI_NAME);
			//return; 無しの場合もある
		}
	}
	
	//// ODBCの自動切換え
	Change_DB();
	Sleep(5000);	// 切り替えたらちょっと待った方がいい？


	//// タスクの実行
	LOG(em_MSG), "タスク実行開始");
	int cnt = 0;
	while(true) {
		cKey.Format("TASK_%d_%d", m_nIniNowID, cnt); 
		GetPrivateProfileString("EXECUTE", cKey, "", cFrom, MAX_PATH, PCINI_NAME);
		
		// 有無
		if(0 == strlen(cFrom) ) break;

		// 実行
		ProcessManager::WinExec(cFrom);

		// 次
		cnt ++;
	}
}


//------------------------------------------
// ファイルコピー実行
// 戻り値 1回でもコピーできたらtrue。もしくは、コピーファイルが無い場合もtrue
//------------------------------------------
bool MainExec::ExecFileCopy()
{
	char		cBaseFrom[MAX_PATH];	// コピー元ベースパス
	char		cBaseTo[MAX_PATH];		// コピー先ベースパス
	char		cWk[MAX_PATH];

	char		cFrom[MAX_PATH];		// コピー元
	char		cTo[MAX_PATH];			// コピー先
	CString		cKey;

	// ベースパス取得 (未設定許可項目)
	GetPrivateProfileString("FILECOPY", "F_BASE", "", cBaseFrom, MAX_PATH, PCINI_NAME);
	GetPrivateProfileString("FILECOPY", "T_BASE", "", cBaseTo,   MAX_PATH, PCINI_NAME);

	// コピー実行
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

		// 有無
		if(0 == strlen(cFrom) || 0 == strlen(cTo)) {
			// コピーファイルが無し
			//if(1==cnt) retc = true;
			//break;
			return true;
		}

		// コピー
		if( ! CopyFile(cFrom, cTo, FALSE) ) {
			LOG(em_ERR), "[FILECOPY] 失敗 [%s]→[%s]", cFrom, cTo);
			//return retc;
			if(1==cnt) return false;	// まだコピー先とつながっていない
		} else {
			LOG(em_MSG), "[FILECOPY] 実行 [%s]→[%s]", cFrom, cTo);
		}
		//retc = true;
		cnt ++;
	}
	return true;
}

//------------------------------------------
// フォルダーコピー実行
// 戻り値 1回でもコピーできたらtrue。もしくは、コピーフォルダが無い場合もtrue
//------------------------------------------
bool MainExec::ExecFolderCopy()
{

	char		cBaseFrom[MAX_PATH];	// コピー元ベースパス
	char		cBaseTo[MAX_PATH];		// コピー先ベースパス
	char		cWk[MAX_PATH];

	char		cFrom[MAX_PATH];		// コピー元フォルダ (\無し)
	char		cTo[MAX_PATH];			// コピー先フォルダ (\無し)
	CString		cKey;
	CString		cFromFPath;
	CString		cToFPath;

	CString		cSearchFile;			// フォルダ内のファイルを検索するキー
	BOOL		bRet;

	WIN32_FIND_DATA		fd;
	HANDLE				hFind;

//	// 自分自身( PCINI_NAME ) が書き換わっちゃまずいから書き換わらない用にファイルを開いておく
//	FILE* f = fopen(PCINI_NAME, "N");

	// ベースパス取得 (未設定許可項目)
	GetPrivateProfileString("FOLDERCOPY", "F_BASE", "", cBaseFrom, MAX_PATH, PCINI_NAME);
	GetPrivateProfileString("FOLDERCOPY", "T_BASE", "", cBaseTo,   MAX_PATH, PCINI_NAME);


	////// コピー実行
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

		// 有無
		if(0 == strlen(cFrom) || 0 == strlen(cTo)) {
			// コピーフォルダが無し
			if(1==cnt) retc = true;
			break;
		}

		// フォルダチェック ＆ 生成
		if( ! CreateFolder(cTo) ) {
			LOG(em_ERR), "[FOLDERCOPY] [%s]フォルダアクセス、生成失敗", cTo);
			cnt ++;
			continue;
		}

		// サーチするファイル
		cSearchFile.Format( "%s\\*.*", cFrom);
		
		//// ファイルサーチ
		memset(&fd, 0x00, sizeof(fd));
		bRet = TRUE;
		hFind= FindFirstFile(cSearchFile, &fd);
		while(hFind != INVALID_HANDLE_VALUE && bRet) {
			// ファイルのみ
			// △ ExeのKizuPc.iniは書き換えちゃダメ
			if( FILE_ATTRIBUTE_DIRECTORY != (FILE_ATTRIBUTE_DIRECTORY & fd.dwFileAttributes) &&
				0 != strcmp(fd.cFileName, "KizuPc.ini") ) {
				
				// コピー先、元のフルパス決定
				cFromFPath.Format("%s\\%s", cFrom, fd.cFileName );
				cToFPath.Format  ("%s\\%s", cTo,   fd.cFileName );
			
				// コピー
				if( ! CopyFile(cFromFPath, cToFPath, FALSE) ) {
					LOG(em_ERR), "[FOLDERCOPY] 失敗 [%s]→[%s]", cFromFPath, cToFPath);
				} else {
					LOG(em_MSG), "[FOLDERCOPY] 実行 [%s]→[%s]", cFromFPath, cToFPath);
				}
			}

			// 次
			bRet = FindNextFile(hFind, &fd);
			retc = true;		// ここまで来る = 統括にアクセス出来た
		}
		FindClose(hFind);

		// 次のフォルダ
		cnt ++;
	}

	//// 開いていたファイルを閉じる
//	fclose(f);
	return retc;
}

//------------------------------------------
// フォルダー生成
// char const* cPath 生成フォルダパス
// 戻り値 false:生成もできなかった
//------------------------------------------
bool MainExec::CreateFolder(char const* cPath)
{
	if( PathIsDirectory( cPath ) ) return true;

	// 生成
	LOG(em_WAR), "[FOLDERCOPY] [%s]が無いためフォルダ生成開始", cPath);
	_mkdir(cPath);
	return PathIsDirectory( cPath );

}

//===========================================================================
// DBの操作関連
//===========================================================================
//------------------------------------------
// DBの変更
// char* Session DBのセッション (該当無しの場合はNULL)
//------------------------------------------
void MainExec::Change_DB()
{
	// 必要な情報取得
	char Session[128];
	GetPrivateProfileString("DB", "SESSION", "", (char *)Session, sizeof(Session), PCINI_NAME);
	if(0 == strlen(Session)) {
		LOG(em_ERR), "DBのセッション情報無し");
		return;
	}

	//// 現在設定されているセッションにKizuTask.iniを書き換える
	ChangeIni_DBSession(Session);
	
	// 2019.01.23 DBのセッション切り替えを複数指定対応 --->>>
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

			//// 現在設定されているセッションにKizuTask.iniを書き換える
			ChangeIni_DBSession_N(Session, cDbSection);
			cnt++;
		}
	}
	// 2019.01.23 DBのセッション切り替えを複数指定対応 ---<<<

	//// KizuTask.iniのDB情報でODBCの自動設定を行う
	CString		cKey;
	int			cnt = 1;
	while(true) {
		cKey.Format("ODBCSET_%d", cnt); 
		memset(Session, 0x00, sizeof(Session));
		GetPrivateProfileString("DB", cKey, "", Session, sizeof(Session), PCINI_NAME);
		if(0 == strlen(Session)) break;
		
		// 自動登録
		ChangeODBC_Setting(Session, cnt);
		cnt ++;
	}
}

//------------------------------------------
// IniファイルでDBのセッションを書き換え
// char* Session DBのセッション (該当無しの場合はNULL)
//------------------------------------------
void MainExec::ChangeIni_DBSession(char const * Session)
{
	char wk_addr[128];
	char wk_odbc[128];
	char wk_dsna[128];
	char wk_user[128];
	char wk_pass[128];

	// NULLなら変更無し
	if ( 0==strlen(Session) ) return;

	// A系かB系のデータを取得
	GetPrivateProfileString(Session, "DBADDLESS", "", (char *)wk_addr, sizeof(wk_addr), TASKINI_NAME);
	GetPrivateProfileString(Session, "ODBCNAME",  "", (char *)wk_odbc, sizeof(wk_odbc), TASKINI_NAME);
	GetPrivateProfileString(Session, "DSNAME",    "", (char *)wk_dsna, sizeof(wk_dsna), TASKINI_NAME);
	GetPrivateProfileString(Session, "USERID",    "", (char *)wk_user, sizeof(wk_user), TASKINI_NAME);
	GetPrivateProfileString(Session, "PASSWORD",  "", (char *)wk_pass, sizeof(wk_pass), TASKINI_NAME);
	// データが全部あるかチェック
	if( 0 == strlen(wk_addr) ||
		0 == strlen(wk_odbc) ||
		0 == strlen(wk_dsna) ||
		0 == strlen(wk_user) ||
		0 == strlen(wk_pass) ) {
		LOG(em_MSG), "[CheckDB] DB ini Session書き換え情報無し");
		return;
	}

	// A系かB系のデータをオンラインにコピー
	WritePrivateProfileString( INI_DB, "DBADDLESS", wk_addr, TASKINI_NAME);
	WritePrivateProfileString( INI_DB, "ODBCNAME",	wk_odbc, TASKINI_NAME);
	WritePrivateProfileString( INI_DB, "DSNAME",	wk_dsna, TASKINI_NAME);
	WritePrivateProfileString( INI_DB, "USERID",	wk_user, TASKINI_NAME);
	WritePrivateProfileString( INI_DB, "PASSWORD",	wk_pass, TASKINI_NAME);
	LOG(em_MSG), "[CheckDB] DB ini Session書き換え [%s]→[%s]", Session, INI_DB);

}

// 2019.01.23 DBのセッション切り替えを複数指定対応 --->>>
//------------------------------------------
// IniファイルでDBのセッションを書き換え
// char* Session DBのセッション (該当無しの場合はNULL)
// char* DbSection DBのセクション名
//------------------------------------------
void MainExec::ChangeIni_DBSession_N(char const * Session, char const * DbSection)
{
	char wk_addr[128];
	char wk_odbc[128];
	char wk_dsna[128];
	char wk_user[128];
	char wk_pass[128];

	// NULLなら変更無し
	if (0 == strlen(Session)) return;
	if (0 == strlen(DbSection)) return;

	// A系かB系のデータを取得
	GetPrivateProfileString(Session, "DBADDLESS", "", (char *)wk_addr, sizeof(wk_addr), TASKINI_NAME);
	GetPrivateProfileString(Session, "ODBCNAME", "", (char *)wk_odbc, sizeof(wk_odbc), TASKINI_NAME);
	GetPrivateProfileString(Session, "DSNAME", "", (char *)wk_dsna, sizeof(wk_dsna), TASKINI_NAME);
	GetPrivateProfileString(Session, "USERID", "", (char *)wk_user, sizeof(wk_user), TASKINI_NAME);
	GetPrivateProfileString(Session, "PASSWORD", "", (char *)wk_pass, sizeof(wk_pass), TASKINI_NAME);
	// データが全部あるかチェック
	if (0 == strlen(wk_addr) ||
		0 == strlen(wk_odbc) ||
		0 == strlen(wk_dsna) ||
		0 == strlen(wk_user) ||
		0 == strlen(wk_pass)) {
		LOG(em_MSG), "[CheckDB] DB ini Session書き換え情報無し(%s)", DbSection);
		return;
	}

	// A系かB系のデータをオンラインにコピー
	WritePrivateProfileString(DbSection, "DBADDLESS", wk_addr, TASKINI_NAME);
	WritePrivateProfileString(DbSection, "ODBCNAME", wk_odbc, TASKINI_NAME);
	WritePrivateProfileString(DbSection, "DSNAME", wk_dsna, TASKINI_NAME);
	WritePrivateProfileString(DbSection, "USERID", wk_user, TASKINI_NAME);
	WritePrivateProfileString(DbSection, "PASSWORD", wk_pass, TASKINI_NAME);
	LOG(em_MSG), "[CheckDB] DB ini Session書き換え [%s]→[%s]", Session, DbSection);

}
// 2019.01.23 DBのセッション切り替えを複数指定対応 ---<<<

//------------------------------------------
// ODBCの設定変更
// char* Session DBのセッション
//------------------------------------------
void MainExec::ChangeODBC_Setting(char const * Session, int nId)
{
	// 今iniファイルに設定されているDBセッションでODBCを設定する
	char wk_addr[128];
	char wk_odbc[128];
	char wk_dsna[128];
	GetPrivateProfileString(Session, "DBADDLESS", "", (char *)wk_addr, sizeof(wk_addr), TASKINI_NAME);
	GetPrivateProfileString(Session, "ODBCNAME",  "", (char *)wk_odbc, sizeof(wk_odbc), TASKINI_NAME);
	GetPrivateProfileString(Session, "DSNAME",    "", (char *)wk_dsna, sizeof(wk_dsna), TASKINI_NAME);
	
	// データが全部あるかチェック
	if( 0 == strlen(wk_addr) ||
		0 == strlen(wk_odbc) ||
		0 == strlen(wk_dsna) ) {
		LOG(em_MSG), "[CheckDB] DB Session情報無し");
		return;
	}
	
	// ドライバ
	int driverid = GetPrivateProfileInt("DB", "DRIVERID", -1, PCINI_NAME);
	// ===>> 坂巻 2018.10.21 ODBCドライバ区分個別指定機能追加
	CString sWk;
	sWk.Format("DRIVERID_%d", nId);
	driverid = GetPrivateProfileInt("DB", sWk, driverid, PCINI_NAME);
	// <<===
	if(0 > driverid ) {
		LOG(em_ERR), "ODBCのドライバ区分異常 [%d]", driverid);
		_ASSERT(0);
		return;
	}


	//// 動作モードにより処理分け
		// ODBC追加モード (0:KS_STARTと同じ  1:32bit版ODBCのみ  2:64bit版ODBCのみ  -1:全部(32bit,64bit))
	int addmode = GetPrivateProfileInt("DB", "ADDMODE", 0, PCINI_NAME);
	if( 0 == addmode ) {

		//// 書き換え
		if( ODBC_Setting(driverid, wk_odbc, wk_addr, wk_dsna) ) {
			LOG(em_MSG), "[CheckDB] ODBC設定書換完了 [%s] ODBC=%s, DB=%s, IP=%s", Session, wk_odbc, wk_dsna, wk_addr);

		} else {
			LOG(em_ERR), "[CheckDB] ODBC設定書換失敗 [%s] ODBC=%s, DB=%s, IP=%s", Session, wk_odbc, wk_dsna, wk_addr);
		}

	} else {
		//// 設定用の小さなEXEに任せる
		int nRec;				// EXEの戻り値
		CString sCmd;			// EXEコマンド

		// 32bit版登録
		if(-1 == addmode || 1 == addmode) {
			// ===>> 坂巻 2018.10.21 ODBCドライバ区分個別指定機能追加
			//sCmd.Format("%s_ODBC.exe %s", KS_START, Session);
			sCmd.Format("%s_ODBC.exe %s %d", KS_START, Session, nId);
			// <<===
			nRec = ProcessManager::ProcessExecWait( sCmd, 0);

			if( 0 == nRec ) {
				LOG(em_MSG), "[CheckDB] ODBC設定書換完了 [%s]", sCmd);
			} else {
				LOG(em_ERR), "[CheckDB] ODBC設定書換失敗 [%s], rec=%d", sCmd, nRec);
			}
		}

		// 64bit版登録
		if(-1 == addmode || 2 == addmode) {
			// ===>> 坂巻 2018.10.21 ODBCドライバ区分個別指定機能追加
			//sCmd.Format("%s_ODBCx64.exe %s", KS_START, Session);
			sCmd.Format("%s_ODBCx64.exe %s %d", KS_START, Session, nId);
			// <<===
			nRec = ProcessManager::ProcessExecWait( sCmd, 0);

			if( 0 == nRec ) {
				LOG(em_MSG), "[CheckDB] ODBC設定書換完了 [%s]", sCmd);
			} else {
				LOG(em_ERR), "[CheckDB] ODBC設定書換失敗 [%s], rec=%d", sCmd, nRec);
			}
		}
	}
}
/*
//------------------------------------------
// ODBC自動設定 (同一DNSの場合は、変更となる)
// int nDriverID ドライバ区分 (0:SQL Server, 1:SQL Native Client, 2:SQL Native Client 10.0)  
// LPCSTR cDNS 勝手に付けれる名前(ODBCの名前)
// LPCSTR cDataSource データベース接続アドレス
// LPCSTR cDatabase データベース名 (NULL許可)
// 実行するタスクに ODBCCP32.lib を追加する事
//------------------------------------------
BOOL MainExec::ODBC_Setting(int nDriverID, LPCSTR cDSN, LPCSTR cDataSource, LPCSTR cDataBase)
{
	// ODBCクラスへの追加は、難しい ( = KizuLibCLIでエラーが発生する)

// #include <odbcinst.h>	// が必要
// ODBCCP32.lib が必要

//DSN = 勝手に付けれる名前
//DESCRIPTION = 説明書き
//TRUSTED_CONNECTION=No これでSQL認証に出来るはず

// 各項目の間にNULLが必要
// ラストはNULL2つ以上必要


	//// 接続文字列
	int len;
	char wk[256];
	memset(wk, 0x00, sizeof(wk));


	//// 追加するドライバを指定
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

	// 削除用文字列
	len  = sprintf(wk		, "Trusted_Connection=no");		len += 1;
	len += sprintf(wk + len , "DSN=%s", cDSN);				
	
	//// ODBC自動削除 (ドライバ名は何でもよい。でも空白はNGだった)
	//  ※ 統括には、SQL Native Client を使用する為、ドライバが異なる。その為、一度消してから、再度作成する
	SQLConfigDataSource( NULL, ODBC_REMOVE_SYS_DSN, "SQL Server", wk );

	// 追加用文字列
	len  = sprintf(wk		, "Trusted_Connection=no");		len += 1;
	len += sprintf(wk + len , "DSN=%s", cDSN);				len += 1;
	len += sprintf(wk + len	, "SERVER=%s", cDataSource);	len += 1;
	len += sprintf(wk + len	, "DataBase=%s", cDataBase);

	//// ODBC自動登録 (ユーザー・パスは設定不可能。どっちにしてもプログラムで使うときに指定する為必要ない？)
	return SQLConfigDataSource( NULL, ODBC_ADD_SYS_DSN, deriver, wk );
}
*/