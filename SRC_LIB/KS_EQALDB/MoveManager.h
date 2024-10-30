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


class MoveManager : public WorkThread,
					public KizuSyncroManager
{

private :
	// 移動テーブル定義
	struct TABLE {
		bool				bEnable;									// このレコードが有効時true
		char				cTbl[PARAM_MAX_STRING];						// 対象テーブル
		int					nExecCnt;									// 1回の処理件数	

		int					nKeyCnt;									// 指定された有効な主キー数
		char				cKey[PARAM_MAX_KEYCNT][PARAM_MAX_STRING];	// 主キー
	};

public:
	MoveManager(CString cSession, LogFileManager* pLog);
	virtual ~MoveManager(void);


	void Start() { WorkThread::Start(MainThread); };					// スレッド開始
	void Stop()  { WorkThread::Stop(10000); };							// スレッド終了

	char const*	GetType() const	{return (char const*)&"実績移動";};		// 動作タイプ (固定名称)

private :
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド
	bool Exec(bool bAtoB);												// A系の実績をB系に移動させる

	//// 同期対象テーブル
	TABLE					m_Tbl[PARAM_MAX_TABLE];						// 同期対象テーブル

};
