//======================================================================
// 太平製カメラ取込は、pitchは8しか考えていない
//   違っていたら、Connect でエラーにする
//======================================================================

#pragma once

// カメラ関係
#include "..\IportLib\IportCommon.h"											// 疵検マクロ

// インポート
using namespace KizuLib;
using namespace IportLib;

namespace AvalLib
{

	//===================================================================
	// カメラ取込 I/F
	//===================================================================
	class AvalManagerIF
	{

	//// メンバー定数
	protected:
		static const int	MAX_CHANNEL	= 4;						// 最大４チャンネル
		static const int	MAX_BOARD	= 8;						// 最大８ボード
		static const int	MAX_CAMNUM	= MAX_CHANNEL*MAX_BOARD;	// 最大カメラ接続数


		// 撮像種別
		enum EM_GRAB_MODE {
			GRAB_MODE_HW = 0,												// ハードウェア。ホンモノ
			GRAB_MODE_BMP,													// Bitmapファイル
			GRAB_MODE_AVI,													// AVIファイル
			GRAB_MODE_MAN													// マニュアルセット
		};



	public:

		virtual void		SetQueFrame(ThreadLoopQueue<FRAME_DATA>* pQue) = 0;


		//// 一回のみ
		//virtual void		Dispose() = 0;									// 解放

		//virtual bool		Start() = 0; 									// スレッド開始 ※ このスレッドは、ホントにスレッドが実行されるまでロックされる
		//virtual bool		Stop() = 0;										// スレッド停止
		//
		//// 撮像系
		//virtual BOOL		StartGrab() = 0;								// 撮像開始
		//virtual BOOL		StopGrab() = 0;									// 撮像停止
		//virtual bool		Reset() = 0;									// リセット


		// サンプル画像操作系
		virtual bool		OpenBmp(char const* cPathFormat, int nNoFrom, int nNoTo, int nCycleTimer) = 0;	// 連続ビットマップ定義
		virtual void		CloseBmp() = 0;											// 連続ビットマップ解放
		virtual bool		OpenAvi(char const* cPathFormat, int nCycleTimer) = 0;	// AVI定義
		virtual void		CloseAvi() = 0;											// AVI解放
		virtual bool		OpenManual() = 0;
		virtual void		CloseManual() = 0;
		virtual bool		OnManualFrameGrabbed(BYTE* pImgs, DWORD nBytes, DWORD nCamset) = 0;

		// その他小物
		virtual bool		SendSerialTemp(DWORD set, DWORD ang, float& temp) = 0;	// カメラ温度を取得 (カメラと接続時しかダメ)
		virtual void		SyncFrame(DWORD set, DWORD ang, WORD nSkipCnt) =0;


	//// プロパティ関係
	public:
		// オブジェクト取得


		virtual inline ThreadLoopQueue<FRAME_DATA>*	GetQueFrame() = 0; 
		virtual inline ThreadQueue<DELIVERY_INFO>&	GetQueInfo() = 0;

		// Set
		virtual void		SetResetModuleOnReset(bool on) = 0;
		virtual void		SetAddInfo(DWORD set, DWORD ang, HANDLE h) = 0;	// フレーム情報に付与する情報
		virtual void		SetImgTurn_LR(bool on) = 0;						// 画像左右反転
		// Get
		virtual void*		GetDummyBuf() = 0;

		// 画像情報
		//virtual IportBase::TYP_FRAME_INFO const& GetFrameInfo() const = 0;
        virtual long		GetFrameWidth()	const		= 0;
        virtual long		GetFrameHeight() const		= 0;
        virtual long		GetFrameNChannel() const	= 0;
        virtual long		GetFramePixelNBit() const	= 0;

		// ステータス
		virtual DWORD		GetResendCount(DWORD set, DWORD ang) const = 0;	// パケット再送回数
		virtual DWORD		GetLostCount  (DWORD set, DWORD ang) const = 0;	// パケット損失個数
		virtual DWORD		GetResendTotal()	 const = 0;
		virtual DWORD		GetLostTotal()		 const = 0;
		virtual DWORD		GetQueueCount()		 const = 0;
		virtual DWORD		GetMaxQueueCount()	 const = 0;
		virtual DWORD		GetChDiff	  (DWORD set, DWORD ang) const = 0;
		virtual DWORD		GetChDiffMax()		 const = 0;
		virtual DWORD		GetFrameNo()		 const = 0;
		virtual bool		IsInit()			 const = 0;
		virtual bool		IsGrabRun()			 const = 0;

	};

};