// TI_RPISAVE.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
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

// CTI_RPISAVEApp:
// このクラスの実装については、TI_RPISAVE.cpp を参照してください。
//

class CTI_RPISAVEApp : public CWinApp
{
public:
	CTI_RPISAVEApp();

// オーバーライド
	public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CTI_RPISAVEApp theApp;