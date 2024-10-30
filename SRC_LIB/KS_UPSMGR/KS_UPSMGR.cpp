// KS_UPSMGR.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <windows.h>

// 疵検 インクルード
#include "..\..\include\KizuCommon.h"									// 疵検共通ヘッダー
#include "..\..\Library\KizuLib\KizuLib.h"								// 疵検ライブラリヘッダー

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// エントリーポイント
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿

int _tmain(int argc, _TCHAR* argv[])
{
	// タスク初期化処理
//	task_init(KS_UPSMGR, 0);
	setlineid(LINE_ID);
	task_init(KS_UPSMGR);

	// UPS異常
	syslog(SYSNO_UPS_NG, "[UPS異常検知]");
	
	// システムシャットダウン通知
	COMMON_QUE que;
	que.nEventNo = FACT_KS_MASTER_06;
	que.nLineNo = getlineid();											// ラインID
	send_mail(KS_MASTER, "*", &que);									// どこかにあるPCマスター管理へ通知

	task_exit();
	return 0;
}

