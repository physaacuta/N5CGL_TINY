#pragma once

// 半固定
#include "..\..\Include\KizuCommon.h"									// 疵検インクルード
#include "..\..\Library\KizuLib\KizuLib.h"								// 疵検ライブラリ
#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// マクロ集
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// スタティックライブラリ集
#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ログ管理クラス

// 疵検クラス インクルード
#include "..\..\Library\KizuLib\Class\WorkThread.h"						// ワーカースレッドクラス
#include "..\..\Library\KizuLib\Class\ThreadQueue.h"					// スレッドキュークラス

#include "..\..\Library\KizuODBC\OdbcBase.h"							// DB操作クラス

using namespace KizuLib;
using namespace KizuODBC;


class KizuSyncroManager
{
public:
	// 同期テーブル定義
	static const int PARAM_MAX_THREAD		= 10;						// 同一インスタンスの最大のインスタンス数
	static const int PARAM_MAX_TABLE		= 100;						// 最大同期対象テーブル

	static const int PARAM_MAX_KEYCNT		= 3;						// 主キー最大指定数 (変更するとSQL文を見直す必要有り)
	static const int PARAM_MAX_STRING		= 128;						// 文字列

	//=======================================
	// 定数定義
	//=======================================
/*	// DBの選択状態
	enum EM_SELECT_DB {
		SELECT_NON	= -1,												// 無し
		SELECT_A	= 0,												// A系
		SELECT_B	= 1													// B系
	};
*/

public:
	KizuSyncroManager(CString cSession, LogFileManager* pLog);
	virtual ~KizuSyncroManager(void);

	virtual void Start() {};											// 純粋仮想関数
	virtual void Stop() {};												// 純粋仮想関数
	virtual char const*	GetType() const	{ return (char const*)& "";};	// 動作タイプ (固定名称)

//	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; };			// ログファイルセット
	void SetStop()			{ my_bStopFlg = true; }						// 終了要求

	bool SetEvNowExec(int nAtoB, int nMailKey);							// 即時実行シグナルコール
	


	// 画面アクセス用
	char const*	GetSession() const { return my_cSession; };				// セッション名称
	char const* GetOdbc_A() const  { return (const char *)my_cOdbcSession[0];}
	char const* GetOdbc_B() const  { return (const char *)my_cOdbcSession[1];}
	char const* GetLink_A() const  { return (const char *)my_cLinkPath[0];}
	char const* GetLink_B() const  { return (const char *)my_cLinkPath[1];}

	int			GetTimeFast() const { return my_nTimeFast/(60 *1000);}
	int			GetTimeInterval() const { return my_nTimeInterval/(60 *1000);}
	int			GetMailkey() const { return my_nMailKey;}



protected :
	void SetOdbcBaseA(OdbcBase* p) { mcls_pDbA = p; };					// A系操作 DB接続クラス
	void SetOdbcBaseB(OdbcBase* p) { mcls_pDbB = p; };					// B系操作 DB接続クラス

	// 共通メソッド
//	static int		CheckIni_Select_DB();								// IniファイルでDBのセッションがA系かB系かチェックする
//	static int		CheckIni_Select_DB(char* Session);					// IniファイルでDBのセッションがA系かB系かチェックする
	bool			CheckDB_Connect();									// A系とB系のDBが生きているかチェックする

	// 実際の処理関係
	bool			Synclock_Master(bool bAtoB, char const* tbl);			// 2DB間のマスターテーブルの同期
//	bool			Copy_DB(bool bAtoB, char const* tbl, int nExecCnt, char const* key1, char const* key2);	// 少量コピー
//	bool			Move_DB(bool bAtoB, char const* tbl, int nExecCnt, char const* key1, char const* key2);	// 少量移動
//	bool			Syncopy_DB(bool bAtoB, char const* tbl, int nExecCnt, char const* key1, char const* key2);	// 2DB間の実績テーブルの同期

	bool			Copy_DB   (bool bAtoB, char const* tbl, int nExecCnt, int nKeyCnt, char const* key);	// 少量コピー
	bool			Move_DB   (bool bAtoB, char const* tbl, int nExecCnt, int nKeyCnt, char const* key);	// 少量移動
	bool			Syncopy_DB(bool bAtoB, char const* tbl, int nExecCnt, int nKeyCnt, char const* key);	// 2DB間の実績テーブルの同期


//	bool Move_DB(bool bBtoA, char const* tbl, int delCnt, char const* keyName);		// 指定項目キーの 少量移動


	//// 各インスタンス
	LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス
	OdbcBase*				mcls_pDbA;									// A系操作 ODBC操作クラス
	OdbcBase*				mcls_pDbB;									// B系操作 ODBC操作クラス

	//// 同期シグナル
	HANDLE					my_evNowExecAtoB;							// A→B 実行シグナル
	HANDLE					my_evNowExecBtoA;							// B→A 実行シグナル

	//// iniファイル関連
	CString					my_cSession;								// 自分の設定が定義されているiniファイルのセッション
	char					my_cOdbcSession[2][128];					// 接続するODBCが定義されてるセッション
	char					my_cLinkPath[2][128];						// リンクサーバー経由でDB接続するパス [0:A→B系の接続パス  1:B→A系の接続パス]	// 形式は、 [pc名].DATABASE_5CGL.dbo.  こんな感じで入っている
//	bool					my_bLinkServerB;							// B系のリンクサーバー有無 (有:true)
	int						my_nTimeFast;								// 初期実行までの時間 [ms]
	int						my_nTimeInterval;							// 2回目以降の実行周期 [ms]
	int						my_nMailKey;								// メールスロット受信時に第2引数と一致した場合、強制的に実行される。 (0:未実施)

	//// その他もろもろ
	bool					my_bStopFlg;								// 終了要求時 true
};
