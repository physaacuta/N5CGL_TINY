// TO_PROCON.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
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

// CTO_PROCONApp:
// このクラスの実装については、TO_PROCON.cpp を参照してください。
//

class CTO_PROCONApp : public CWinApp
{
public:
	CTO_PROCONApp();

// オーバーライド
	public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CTO_PROCONApp theApp;