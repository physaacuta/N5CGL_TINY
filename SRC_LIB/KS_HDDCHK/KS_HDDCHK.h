// KS_HDDCHK.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
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

// CKS_HDDCHKApp:
// このクラスの実装については、KS_HDDCHK.cpp を参照してください。
//

class CKS_HDDCHKApp : public CWinApp
{
public:
	CKS_HDDCHKApp();

// オーバーライド
	public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CKS_HDDCHKApp theApp;