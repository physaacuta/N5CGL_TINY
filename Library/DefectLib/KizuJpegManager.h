// *********************************************************************************
//	Jpeg画像生成クラス
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//		Ver.02	  2010/11/30  ijl20.dll を binの中に入れる必要なしとなった
//
//	[メモ]
//		開発環境には、ippをインストールしておく必要有り
//
////// 使用にあたり
// ・プロジェクト/プロパティを選択
// ・追加のインクルードディレクトリで『C:\Program Files\Intel\IPP\5.0\ia32\include』を追加
//     (ipp5.0のインクルードフォルダ)
// ・追加のライブラリディレクトリで『..\..\include\Other\Ipp』を追加
//
// 環境構築
//	[追加インクルードパス]
//		C:\Program Files\Intel\IPP\5.0\ia32\include
//	[追加ライブラリパス]
//		..\Include\Other\Ipp
//	[追加ライブラリファイル]
/*
ijl20l.lib
ippimerged.lib
ippsmerged.lib
ippjmerged.lib
ippcvmerged.lib
ippsemerged.lib
ippiemerged.lib
ippjemerged.lib
ippcvemerged.lib
ippcorel.lib
*/
/*
ijl20l.lib;ippimergedem64t.lib;ippsmergedem64t.lib;ippjmergedem64t.lib;ippcvmergedem64t.lib;ippsemergedem64t.lib;ippiemergedem64t.lib;ippjemergedem64t.lib;ippcvemergedem64t.lib;ippcoreem64tl.lib
*/
// *********************************************************************************



#pragma once

#include <windows.h>

#include "..\..\Include\LibSelect.h"											// ライブラリ切替ヘッダー

#ifdef DEFECTLIB_KIZUJPEGMANAGER
	// 市販インクルード
	#include "..\..\include\Other\Ipp\ijl.h"									// JPEG変換のインクルード
#endif


namespace KizuLib
{
	class KizuJpegManager
	{
	public:
		KizuJpegManager(void);
		virtual ~KizuJpegManager(void);

		static int BmpToJpeg(int nW, int nH, const BYTE* bP, int jquality, int* jSize, BYTE* jP, bool nTB = false);		// BMP→JPEG 変換（グレー）
		static int BmpToJpegColor(int nW, int nH, const BYTE* bP, int jquality, int* jSize, BYTE* jP, bool nTB = false);// BMP→JPEG 変換（カラー）
		static int BmpToJPEGfile(int nW, int nH, const BYTE* bP, int jquality, const char* Path, bool nTB = false);	// BMP→JPEG 変換後ファイル書き込み（グレー）


		static int JpegToBmp(const BYTE* jP, int jSize, int nW, int nH, BYTE* bP, bool nTB = false );					// JPEG→BMP 変換（グレー）
		static int JpegToBmpColor(const BYTE* jP, int jSize, int nW, int nH, BYTE* bP, bool nTB = false );				// JPEG→BMP 変換（カラー）

	};
};

