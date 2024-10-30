// *********************************************************************************
//	マクロ集
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//
// *********************************************************************************

//#ifndef KIZULIB_KIZUMACRO_H
//#define KIZULIB_KIZUMACRO_H

#pragma once

//#pragma warning(once : 4005)								// マクロの再定義 (SAFE_DELETE とかが、fcでも定義されているための対策) disable で出なくするのは困るから、一回だけ出力しておく

namespace KizuLib
{
	//// キーワード
	// interfaceキーワードの定義
	// (VisualCのBASETYPS.Hに同様の定義あり)
	#ifndef interface
		#define interface struct
	#endif
	// implementsキーワードの定義
	#ifndef implements
		#define implements public
	#endif


	//// マクロ
	// FCにも定義されている対策	
	#ifndef SAFE_DELETE
		#define SAFE_DELETE(x)			if (NULL != x) { delete x; x = NULL; }											// New開放
	#endif
	#ifndef SAFE_DELETE_ARRAY
		#define SAFE_DELETE_ARRAY(x)	if (NULL != x) { delete [] x; x = NULL; }										// New配列開放
	#endif
	#ifndef SAFE_DELETE_HANDLE
		#define SAFE_DELETE_HANDLE(x)	if (INVALID_HANDLE_VALUE != x) { CloseHandle(x); x = INVALID_HANDLE_VALUE; }	// ハンドル開放
	#endif

	#define SAFE_TIMER_SET(t,id,interval)	if(0==t) { t=SetTimer(NULL, id, interval, NULL); }						// タイマー起動
	#define SAFE_TIMER_KILL(t)				if(0!=t) { KillTimer(NULL, t); t = 0; }									// タイマー終了


	//=======================================
	// ログ定義
	// 使用する場合は、
	//   1:mcls_pLog をメンバー変数に追加している事
	//   2:コンストラクタでNULLをちゃんとセットする事
	// 使用例:  LOG(em_MSG), "test_%d_%d", wk, wk );
	//=======================================
	#define LOG(t)  if(mcls_pLog!=NULL)       mcls_pLog->Write(KizuLib::LogFileManager::t							// ログ出力
	#define pLOG(t) if(p->mcls_pLog!=NULL) p->mcls_pLog->Write(KizuLib::LogFileManager::t							// ログ出力(ポインタ版)


	//=======================================
	// ワーニング対策
	//  下のワーニングが、うっとうしいから回避するよ
	//		warning C4805: '|' : 演算中の 'bool' 型と 'const int' 型の混用は安全ではありません
	//		warning C4800: 'const int' : ブール値を 'true' または 'false' に強制的に設定します (警告の処理)
	//		bool b = i;		// C4800
	//		if (i == b)		// C4805
	//	使用方法:  bool b = __BOOL i;	
	//
	//=======================================
	#define __BOOL	!!		// int型を2重否定でbool型にする


	//=======================================
	// もろもろ
	//=======================================
	#define GET_JUST_SIZE(x, l)		((((x-1)/l)+1)*l)				// xサイズをlの倍数に揃える(必ず、xのままか、ちょっと大きくなる方向)
	#define GET_JUST_INDEX(x, l)	((x-1)/l)						// 割り切り処理 ( 5, 5 の場合は、0となる) (この結果を使って配列を定義するときは+1すること)
	#define BYTE_CAST(x) ((x) > 255 ? 255 : (x) < 0 ? 0 : (x))		// BYTE型へキャスト

};

//#endif