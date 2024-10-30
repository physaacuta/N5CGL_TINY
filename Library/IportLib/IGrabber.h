//======================================================================
// 太平製カメラ取込 I/F
//======================================================================

#pragma once

// インポート
using namespace KizuLib;

namespace KizuLib
{
	//===================================================================
	// 撮像系定義
	interface IGrabber
	{
		// １回のみ
			// Init  は引数が違うため、継承先で実装
		virtual void		Dispose() = 0;									// 開放

		//virtual bool		Start() = 0;									// 開始
		//virtual bool		Stop() = 0;										// 停止

		// 撮像系
		virtual int			StartGrab() = 0;								// 撮像開始
		virtual int			StopGrab() = 0;									// 撮像停止
		virtual bool		Reset() = 0;									// リセット (Init直後の状態に戻す)

		protected: IGrabber(){  }
	};


	//===================================================================
	// 撮像パラメータ
	interface IGrabberParam
	{
        virtual long		GetFrameWidth()	const		= 0;
        virtual long		GetFrameHeight() const		= 0;
        virtual long		GetFrameNChannel() const	= 0;
        virtual long		GetFramePixelNBit() const	= 0;

		protected: ~IGrabberParam() {}
	};


	//===================================================================
	// ダミー撮像系定義
	interface IGrabberDummy
	{
		// ダミー初期
		virtual bool		OpenBmp(char const* cPathFormat, int nNoFrom, int nNoTo, int nCycleTimer) = 0;	// 連続ビットマップ定義
		virtual void		CloseBmp() = 0;														// 連続ビットマップ解放
		virtual bool		OpenAvi(char const* cPathFormat, int nCycleTimer) = 0;				// AVI定義
		virtual void		CloseAvi() = 0;														// AVI解放
		virtual bool		OpenManual() = 0;
		virtual void		CloseManual() = 0;

		// 処理
		virtual void		ChangeSmpPathFormat(int nMode = 0) = 0;								// フォーマット切替

		virtual void		OnBmpFrameGrabbed() = 0;;											// 全カメラセット、カメラ角度に対してビットマップ画像をセット
		virtual void		OnAviFrameGrabbed() = 0;;											// 全カメラセット、カメラ角度に対してAVI画像をセット
		virtual bool		OnManualFrameGrabbed(BYTE* pImgs, DWORD nBytes, DWORD nCamset) = 0;
		
		protected: ~IGrabberDummy() {}
	};


	//===================================================================
	//混色分離
	interface IGrabberCorrMat{
		enum { NCOEF  = 12 };
		virtual void		SetCorrMat(int , const float* mat) = 0;							// 混色行列値セット
		virtual void		ResetCorrMat() = 0;	
		
		protected: ~IGrabberCorrMat() {}
	};
};