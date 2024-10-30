// *********************************************************************************
//	シェーディング補正 クラス (CLI用)
//	[Ver]
//		Ver.01    2008/10/28  初版
//
//	[メモ]
//		・CLI上から インラインアセンブラ等は、使用できない為、以下の設定を行うこと
//			・ShdCmp_Main.cpp以外のシェーリング補正用の*.cppが対象として、
//				プロパティ / 構成プロパティ / C/C++ / 全般 / 「共通言語ランタイム サポートでコンパイル」を「サポートしない」に変更
// *********************************************************************************
#pragma once

#include <windows.h>
#include "ShdCmp_DeShade.h"

using namespace System;
using namespace System::Runtime::InteropServices;

using namespace ImgLibCLI;

namespace ImgLibCLI
{

	public ref class ShadingComp
	{
	public:
		//=======================================
		// 定数定義
		//=======================================
		static const long DEF_LPF_WIDTH		= 32;								// LPFブロック幅  （0～画像の幅）  8の倍数
		static const long DEF_LPF_HEIGHT	= 32;								// LPFブロック高さ（0～画像の高さ）8の倍数
		static const long DEF_LPF_TARGET	= 128;								// シェーリング補正の目標輝度値 （0～255）


		//=======================================
		// 公開メソッド
		//=======================================
		ShadingComp();															// コンストラクタ
		virtual ~ShadingComp();													// デストラクタ
		
		BOOL					Alloc(long imgW, long imgH);					// ロック
		void					Free();											// 開放
		BOOL					CreateLPF();									// LPF生成
		BOOL					CreateLPF(long lpfW, long lpfH, BYTE taget);	// LPF生成
		BOOL					Exec(array<BYTE>^ pSrc, array<BYTE>^ pDst);		// シェーディング補正
		BOOL					ExecEdgeMask(array<BYTE>^ pSrc, array<BYTE>^ pDst, long lEdge, long rEdge);// シェーディング補正(エッジ外黒埋め)
		BOOL					ExecEdgeCopy(array<BYTE>^ pSrc, array<BYTE>^ pDst, long lEdge, long rEdge);// シェーディング補正(エッジ外生画像)

		//=======================================
		// Static 関数
		//=======================================
		static BOOL	ExecStatic(long imgW, long imgH, array<BYTE>^ pSrc, array<BYTE>^ pDst);
		static BOOL	ExecStatic(long imgW, long imgH, long lpfW, long lpfH, BYTE target, array<BYTE>^ pSrc, array<BYTE>^ pDst);


	private:
		// メンバークラス
		CDeShadeMMX*			mcls_pDeshade;									// 実際のシェーリング補正実施用クラス
		CDeShadeMMX::Params*	mcls_pParams;									// シェーリング補正パラメータクラス
		//BYTE*					m_pImgBuf;										// シェーリング補正変換画像格納領域
		IntPtr					m_pImgBuf;										// シェーリング補正変換画像格納領域
		

		// メンバー変数
		long					m_nImgW;										// 対象画像の幅
		long					m_nImgH;										// 対象画像の高さ
		bool					m_bIsLpf;										// LPF生成済み時 true
	
	};
}
