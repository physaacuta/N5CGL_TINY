// TO_LINKSW.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
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

// CTO_LINKSWApp:
// このクラスの実装については、TO_LINKSW.cpp を参照してください。
//

class CTO_LINKSWApp : public CWinApp
{
public:
	CTO_LINKSWApp();

// オーバーライド
	public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CTO_LINKSWApp theApp;