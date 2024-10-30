#pragma once

//======================================================================
// 撮像基底クラス
//======================================================================


// 疵検ライブラリ
#include "..\KizuLib\CLASS\KizuMacro.h"										// 疵検マクロ
#include "..\KizuLib\Class\KizuPerformanceCounter.h"						// タイマー

#include "..\KizuLibMFC\CLASS\ThreadManager.h"								// ワーカースレッドマネージャー
#include "..\KizuLibMFC\Class\LogFileManager.h"								// ログ管理クラス
#include "..\KizuLibMFC\CLASS\ThreadLoopQueue.h"							// 回転寿司形式のスレッドキュー


#include "..\KizuMMX\MemCopy.h"												// MMXライブラリ

#include "..\..\Include\LibSelect.h"										// ライブラリ切替ヘッダー



// テスト画像取得用
#include "..\ImgLib\CLASS\KizuAviManager.h"									// AVIファイル
#include "..\KizuLib\CLASS\KizuBitmapManager.h"								// BitMapファイル

//
#include "IportCommon.h"													// iPort用定数
#include "IGrabber.h"														// 撮像I/F



// インポート
using namespace KizuLib;




//===================================================================
// マクロ
#define LOM(t) mcls_LogMe.Write(KizuLib::LogFileManager::t			// ローカル専用ログ



//===================================================================
// 本体
namespace IportLib
{
	class GrabberManager :	public ThreadManager,
							implements IGrabber,
							implements IGrabberParam,
							implements IGrabberDummy
	{

	//===================================================================
	// 固定定数
	public:
		static const int	MAX_COLOR_CHANNEL	= 3;						// 最大４色チャンネル


		//// 撮像種別
		enum EM_GRAB_MODE {
			GRAB_MODE_HW = 0,												// ハードを使ったオンラインモード
			GRAB_MODE_IPORT = GRAB_MODE_HW,									// iPort (ハードを使ったオンライン) ※昔の互換のために残しておく

			// デバック状態
			GRAB_MODE_BMP,													// Bitmapファイル
			GRAB_MODE_AVI,													// AVIファイル
			GRAB_MODE_MAN													// マニュアルセット
		};

		//// Gragger種類
		enum EM_GRAB_TYPE {
			GRAB_TYPE_IPORT = 0,											// iPort
			GRAB_TYPE_AVAL													// AvalACL2
		};

		//// 接続パラメータ
		struct CONNECT_PARAM {
		};

	protected:
		enum EM_SMPPATHFROMAT {
			SMPPATHFROMAT_NOM = 0,											// 0:通常orCHで全部同じ画像 
			SMPPATHFROMAT_CHN												// 1:CH毎に別画像
		};


	//===================================================================
	// 公開
	public:
		GrabberManager(EM_GRAB_TYPE emType, CString sMyManagerName="GrabberMgr");
		virtual ~GrabberManager(void);


		//----------------------------------------
		// 実装
		virtual EM_GRAB_TYPE	IsGrabType() const { return my_emGrabType; }


		//----------------------------------------
		// 独自I/F
		// Grabber操作
//		virtual BOOL		Connect( CONNECT_PARAM* prm );
		virtual void		DisConnect() = 0;								// iPortと切断

		//----------------------------------------
		//※ここではしない。さらに継承先で実装。  これをしておかないと、GrabberManager型にポインタ入れたときに、いきなりThreadManagerのStart()にいってしまう
		virtual bool		Start() = 0;									// スレッド開始 ※ このスレッドは、ホントにスレッドが実行されるまでロックされる
		virtual bool		Stop() = 0;										// スレッド停止
	


		// オブジェクト設定
		virtual void		SetQueFrame(ThreadLoopQueue<FRAME_DATA>* pQue) { myque_pFrame = pQue;}
		virtual void		SetCamsetNo( int camset ){ m_ncamset = camset ; }


		// オブジェクト取得
		virtual inline ThreadLoopQueue<FRAME_DATA>*	GetQueFrame()	{ return myque_pFrame; }
		virtual inline ThreadQueue<DELIVERY_INFO>&	GetQueInfo()	{ return myque_Info; }

		// ステータス
		virtual bool		IsInit()			const { return my_bInit;}
		virtual bool		IsGrabRun()			const { return my_bGrabRun;}
		virtual int			GetChannel()		const = 0;
		virtual int			GetCamset()			const { return m_ncamset; } 
		virtual void		SetParam( const char* id, const void *param ) {}

	//// メンバー変数
	protected:

		// クラス
		LogFileManager					mcls_LogMe;							// ローカル専用
	
		// Que
		ThreadLoopQueue<FRAME_DATA>*	myque_pFrame;						// フレーム画像受け渡しキュー(回転寿司形式) (外部からインスタンスをもらう)
		ThreadQueue<DELIVERY_INFO>		myque_Info;							// 付加情報受け渡しキュー (自分でインスタンスを作る)


		// 基本情報
		EM_GRAB_TYPE					my_emGrabType;						// Grabber の種類 (iPort,Aval)
		EM_GRAB_MODE					my_emGrabMode;						// 撮像種別

		int								m_ncamset;

		// ステータス
		bool							my_bInit;							// Init済みでtrue
		bool							my_bGrabRun;						// Grabbing中true

	};
};