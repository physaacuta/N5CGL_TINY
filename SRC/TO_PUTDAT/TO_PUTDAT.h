
// TO_PUTDAT.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
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


// CTO_PUTDAT:
// このクラスの実装については、TO_PUTDAT.cpp を参照してください。
//

class CTO_PUTDAT : public CWinApp
{
public:
	CTO_PUTDAT();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CTO_PUTDAT theApp;