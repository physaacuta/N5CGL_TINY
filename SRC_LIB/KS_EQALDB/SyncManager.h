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

#include "KizuSyncroManager.h"											// DB同期管理クラス

using namespace KizuLib;


class SyncManager : public WorkThread,
					public KizuSyncroManager
{

private :
	// 同期テーブル定義
	struct TABLE {
		bool				bEnable;									// このレコードが有効時true
		char				cTbl[PARAM_MAX_STRING];						// 対象テーブル
	};


public:
	SyncManager(CString cSession, LogFileManager* pLog);
	virtual ~SyncManager(void);

	void Start() { WorkThread::Start(MainThread); };					// スレッド開始
	void Stop()  { WorkThread::Stop(10000); };							// スレッド終了

	char const*	GetType() const	{return (char const*)&"マスター同期";};		// 動作タイプ (固定名称)

private :
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド
	bool Exec(bool bAtoB);												// 2DB間のテーブルを同じにする


	//// 同期対象テーブル
	TABLE					m_Tbl[PARAM_MAX_TABLE];						// 同期対象テーブル
};
