// KT_TEMPMT.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
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

// CKT_TEMPMTApp:
// このクラスの実装については、KT_TEMPMT.cpp を参照してください。
//

class CKT_TEMPMTApp : public CWinApp
{
public:
	CKT_TEMPMTApp();

// オーバーライド
	public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CKT_TEMPMTApp theApp;