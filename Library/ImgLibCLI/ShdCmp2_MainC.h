// *********************************************************************************
//	シェーディング補正の２ クラス (C++用)
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


namespace ImgLibCLI
{

	class ShdCmp2_MainC
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


		ShdCmp2_MainC(int imgW, int imgH, int lpfW, int lpfH, BYTE target);		// コンストラクタ
		ShdCmp2_MainC();
		virtual ~ShdCmp2_MainC();												// デストラクタ. IDispose::Dispose
		//=======================================
		// 公開メソッド
		//=======================================

		BOOL					Alloc();										// ALLOC(ダミー）
		void					Free();											// 開放(ダミー）

		BOOL	CreateLPF(int imgW, int imgH, int lpfW, int lpfH, BYTE taget);	// 画像サイズ可変のLPF生成

		
		// シェーディング補正
		bool				Exec(const BYTE * pSrc, BYTE * pDst);				
		
		// シェーディング補正(範囲限定.エッジ外はLPFサイズが小さくなる)
		bool				ExecEdgeCopy(int lEdge, int rEdge, const BYTE * pSrc, BYTE * pDst );

		// シェーディング補正(範囲限定.エッジ外は黒の扱い)
		bool				ExecEdgeMask(int lEdge, int rEdge, const BYTE * pSrc, BYTE * pDst);


		//=======================================
		// Static 関数
		//=======================================

		//IPP初期化。実行時に最適なCPU毎ルーチンが選択される。
		//実行時mainの頭当たりで、一発呼ぶべき。
		static int  ippStaticInit();

		//シェーディング補正（LPF・TARGETはデフォルト）
		static bool ExecStatic(int imgW, int imgH, const BYTE * pSrc, BYTE * pDst);
		//シェーディング補正（LPF・TARGET指定）
		static bool	ExecStatic(int imgW, int imgH, int lpfW, int lpfH, BYTE taget, const BYTE * pSrc, BYTE * pDst);

	private:
		// メンバークラス
		class CLPFSAT*				mcls_pLPFSAT;								// LPF画像変換（作成）クラス
		class CDevideMMX*			mcls_pDivide;								// 目標ピクセル輝度への変換クラス
		BYTE *						m_pLPFPic;									//　LPF化した画像。 pSrc == pDstのとき用テンポラリ
		BYTE						m_target;									// 目標ピクセル輝度
	};
}
