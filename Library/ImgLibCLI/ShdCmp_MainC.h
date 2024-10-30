// *********************************************************************************
//	シェーディング補正 クラス (C++用)
//	[Ver]
//		Ver.01    2012/10/29  初版
//
//	[メモ]
//		・C++用。CLI用としても使うため、プログラム本体は、ImgLibCLIにおいておく
// *********************************************************************************
#pragma once

#include "Common\ShdCmp_DeShade.h"

using namespace ImgLibCLI;

namespace ImgLibCLI
{
	class ShdCmp_MainC
	{
	public:
		//=======================================
		// 定数定義
		//=======================================
		static const long DEF_LPF_WIDTH		= 32;								// LPFブロック幅  （0～画像の幅）  8の倍数
		static const long DEF_LPF_HEIGHT	= 32;								// LPFブロック高さ（0～画像の高さ）8の倍数
		static const long DEF_LPF_TARGET	= 128;								// シェーリング補正の目標輝度値 （0～255）


	public:
		ShdCmp_MainC(void);
		~ShdCmp_MainC(void);

		//=======================================
		// 公開メソッド
		//=======================================
		
		BOOL					Alloc();										// ロック
		void					Free();											// 開放

		BOOL	CreateLPF(long imgW, long imgH, long lpfW, long lpfH, BYTE taget);	// 画像サイズ可変のLPF生成
		BOOL	Exec(BYTE const* pSrc, BYTE* pDst);									// シェーディング補正
		BOOL	ExecEdgeMask(long nPosL, long nPosR, BYTE const* pSrc, BYTE* pDst);	// シェーディング補正(エッジ外黒埋め)
		BOOL	ExecEdgeCopy(long nPosL, long nPosR, BYTE const* pSrc, BYTE* pDst);	// シェーディング補正(エッジ外は元画像)


		//=======================================
		// Static 関数
		//=======================================
		static BOOL	ExecStatic(long imgW, long imgH, BYTE const* pSrc, BYTE* pDst);
		static BOOL	ExecStatic(long imgW, long imgH, long lpfW, long lpfH, BYTE taget, BYTE const* pSrc, BYTE* pDst);


	private:
		// メンバークラス
		CDeShadeMMX*			mcls_pDeshade;									// 実際のシェーリング補正実施用クラス
		CDeShadeMMX::Params*	mcls_pParams;									// シェーリング補正パラメータクラス
		

		// メンバー変数
		long					m_nImgW;										// 対象画像の幅
		long					m_nImgH;										// 対象画像の高さ
		bool					m_bIsLpf;										// LPF生成済み時 true
	
	};
};