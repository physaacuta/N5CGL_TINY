// GOT_CONTROL.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// 手動追加 -------------------->>>
// ヘッダー
#include "MainInctance.h"					// メインインスタンス制御
// <<<-----------------------------

// CGOT_CONTROLApp:
// このクラスの実装については、GOT_CONTROL.cpp を参照してください。
//

class CGOT_CONTROLApp : public CWinApp
{
public:
	CGOT_CONTROLApp();

// オーバーライド
	public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CGOT_CONTROLApp theApp;