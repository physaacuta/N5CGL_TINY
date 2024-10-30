#pragma once


// 疵検 インクルード
#include "..\..\include\KizuCommon.h"									// 疵検共通ヘッダー


// 疵検クラス インクルード
#include "..\..\Library\KizuLib\CLASS\KizuMacro.h"						// 疵検マクロ
#include "..\..\Library\KizuLib\CLASS\ProcessManager.h"					// プロセス管理
#include "..\..\Library\KizuLibMFC\CLASS\LogFileManager.h"				// ログマネージャー

#include "..\..\Library\KizuLib\CLASS\MagicPacket.h"					// マジックパケット管理クラス

#include <shlwapi.h>	// フォルダチェック
#include <direct.h>		// フォルダ生成

// ローカル
#include "OdbcSetFunc.h"												// ほかで使うためファンクション化


using namespace KizuLib;

class MainExec
{
public:
	MainExec(LogFileManager* pLog, bool* bCancel);
	virtual ~MainExec(void);

	void DoExec();														// メイン処理

private:
	bool ExecFolderCopy();												// フォルダーコピー実行
	bool ExecFileCopy();												// ファイルコピー実行

	bool CreateFolder(char const* cPath);								// フォルダー生成


	// DBのIniファイル操作関連
	void Change_DB();													// DBの変更
	void ChangeIni_DBSession(char const * Session);						// IniファイルでDBのセッションを書き換え
	void ChangeIni_DBSession_N(char const * Session, char const * DbSection);	// IniファイルでDBのセッションを書き換え(複数セッション対応) 2019.01.23 Add
	// ===>> 坂巻 2018.10.21 ODBCドライバ区分個別指定機能追加
	//void ChangeODBC_Setting(char const * Session);						// ODBCの設定変更
	void ChangeODBC_Setting(char const * Session, int nId);				// ODBCの設定変更
	// <<===
//	BOOL ODBC_Setting(int nDriverID, LPCSTR cDSN, LPCSTR cDataSource, LPCSTR cDataBase);	// ODBC自動設定 (同一DNSの場合は、変更となる)


	//// 各インスタンス
	LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス

	//// その他もろもろ
	int						m_nIniNowID;								// 自ID
	bool*					m_pCancel;									// 強制終了時 true

};
