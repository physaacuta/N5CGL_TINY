// *********************************************************************************
//	シェーディング補正の２ クラス (CLI用)
//	[Ver]
//		Ver.01    2013/6/22  初版
//
//	[メモ]
//		・CLI上から インラインアセンブラ等は、使用できない為、以下の設定を行うこと
//			・MakeSATasm.cpp, LPFSAT.cpp
//				プロパティ / 構成プロパティ / C/C++ / 全般 / 「共通言語ランタイム サポートでコンパイル」を「サポートしない」に変更
// *********************************************************************************
#pragma once

#include <windows.h>



#include "../../../include/LibSelect.h"									// ライブラリ切替ヘッダー
//LPFSATの実装にIPPを使っているので、IPP無では、クラス毎消える。
#ifdef DEFECTLIB_KIZUJPEGMANAGER										

using namespace System;
using namespace System::Runtime::InteropServices;


namespace ImgLibCLI
{
	public ref class ShadingComp2
	{
	public:
		//=======================================
		// 定数定義
		//=======================================
		static const int SHRINK_WIDTH	= 16;								// LPF用の処理単位幅 8の倍数
		static const int SHRINK_HEIGHT	= 16;								// LPF用の処理単位高 8の倍数

		static const int DEF_LPF_WIDTH	= 64;								// デフォルトLPFブロック幅  （0～画像の幅）  SHINK_WIDTHの倍数
		static const int DEF_LPF_HEIGHT	= 64;								// デフォルトLPFブロック高さ（0～画像の高さ）SHINK_HEIGHTの倍数

		static const int DEF_LPF_TARGET	= 128;								// デフォルトシェーリング補正の目標輝度値 （0～255）


		//=======================================
		// 公開メソッド
		//=======================================
		ShadingComp2(int imgW, int imgH, int lpfW, int lpfH, BYTE target);		// コンストラクタ
		virtual ~ShadingComp2();												// デストラクタ. IDispose::Dispose

		//互換インターフェース群
		ShadingComp2();		// コンストラクタ
		bool					Alloc(long imgW, long imgH);					// ロック
		void					Free();											// 開放
		bool					CreateLPF();									// LPF生成
		bool					CreateLPF(long lpfW, long lpfH, BYTE taget);	// LPF生成
		
		// シェーディング補正
		bool				Exec(array<BYTE>^ pSrc, array<BYTE>^ pDst);	
		bool				Exec(IntPtr pSrc, IntPtr pDst);				
		
		// シェーディング補正(範囲限定.エッジ外は元画像のまま)
		bool				ExecEdgeCopy(array<BYTE>^ pSrc, array<BYTE>^ pDst, int lEdge, int rEdge);
		bool				ExecEdgeCopy(IntPtr pSrc, IntPtr pDst, int lEdge, int rEdge);			 

		// シェーディング補正(範囲限定.エッジ外は黒の扱い)
		bool				ExecEdgeMask(array<BYTE>^ pSrc, array<BYTE>^ pDst, int lEdge, int rEdge);
		bool				ExecEdgeMask(IntPtr pSrc, IntPtr pDst, int lEdge, int rEdge);			   


		//=======================================
		// Static 関数
		//=======================================
		//シェーディング補正（LPF・TARGETはデフォルト）
		static bool ExecStatic(long imgW, long imgH, array<BYTE>^ pSrc, array<BYTE>^ pDst);
		static bool ExecStatic(long imgW, long imgH, IntPtr pSrc, IntPtr pDst);
		//シェーディング補正（LPF・TARGET指定）
		static bool	ExecStatic(long imgW, long imgH, long lpfW, long lpfH, BYTE taget, array<BYTE>^ pSrc, array<BYTE>^ pDst);
		static bool	ExecStatic(long imgW, long imgH, long lpfW, long lpfH, BYTE taget, IntPtr pSrc,IntPtr pDst);

	private:
		// メンバークラス
		class CLPFSAT*				mcls_pLPFSAT;								// LPF画像変換（作成）クラス
		class CDevideMMX*			mcls_pDivide;								// 目標ピクセル輝度への変換クラス
		IntPtr						m_pLPFPic;									//　LPF化した画像。 pSrc == pDstのとき用テンポラリ
		BYTE						m_target;
		
		int							m_nImgWidth;
		int							m_nImgHeight;
	};
}
#endif