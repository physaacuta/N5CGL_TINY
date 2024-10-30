// XX_CPPSMPEX.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
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

// CXX_CPPSMPEXApp:
// このクラスの実装については、XX_CPPSMPEX.cpp を参照してください。
//

class CXX_CPPSMPEXApp : public CWinApp
{
public:
	CXX_CPPSMPEXApp();

// オーバーライド
	public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CXX_CPPSMPEXApp theApp;