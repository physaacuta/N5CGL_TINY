// KS_EVTLOG.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
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

// CKS_EVTLOGApp:
// このクラスの実装については、KS_EVTLOG.cpp を参照してください。
//

class CKS_EVTLOGApp : public CWinApp
{
public:
	CKS_EVTLOGApp();

// オーバーライド
	public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CKS_EVTLOGApp theApp;