// DummyLED.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
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

// CDummyLEDApp:
// このクラスの実装については、DummyLED.cpp を参照してください。
//

class CDummyLEDApp : public CWinApp
{
public:
	CDummyLEDApp();

// オーバーライド
	public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CDummyLEDApp theApp;