// KS_START.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CKS_STARTApp:
// このクラスの実装については、KS_START.cpp を参照してください。
//

class CKS_STARTApp : public CWinApp
{
public:
	CKS_STARTApp();

// オーバーライド
	public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CKS_STARTApp theApp;