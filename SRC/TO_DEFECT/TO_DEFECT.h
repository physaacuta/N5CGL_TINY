
// TO_DEFECT.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル

// 手動追加 -------------------->>>
// ヘッダー
#include "MainInstance.h"					// メインインスタンス制御
// <<<-----------------------------

// CTO_DEFECTApp:
// このクラスの実装については、TO_DEFECT.cpp を参照してください。
//

class CTO_DEFECTApp : public CWinApp
{
public:
	CTO_DEFECTApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CTO_DEFECTApp theApp;