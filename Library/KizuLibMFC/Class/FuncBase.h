// *********************************************************************************
//	ファンクション用のベースクラス
//	[メモ]
//		ThreadManagerは継承しないけど、LogManagerとかだけは使いたいよ
// *********************************************************************************

#pragma once

// 半固定
#include "..\..\..\Include\KizuCommon.h"								// 疵検インクルード
#include "..\..\KizuLib\KizuLib.h"										// 疵検ライブラリ
#include "..\..\KizuLib\Class\KizuMacro.h"								// マクロ集
#include "..\..\KizuLibMFC\Class\LogFileManager.h"						// ログ管理クラス

using namespace KizuLib;
class FuncBase
{
public:
	FuncBase(const char* pThreadName="");
	~FuncBase(void);

	void SetLogMgr(LogFileManager* pCls) {mcls_pLog = pCls;}	// ログクラスセット
	char const*		GetThreadName()		 {return my_sThreadName.GetBuffer();}	// 自スレッド名


protected:
	LogFileManager*			mcls_pLog;							// LogFileManagerスレッドインスタンス
	CString					my_sThreadName;						// 自スレッド名
};
