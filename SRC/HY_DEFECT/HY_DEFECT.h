
// HY_DEFECT.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
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


// CHY_DEFECTApp:
// このクラスの実装については、HY_DEFECT.cpp を参照してください。
//

class CHY_DEFECTApp : public CWinApp
{
public:
	CHY_DEFECTApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CHY_DEFECTApp theApp;