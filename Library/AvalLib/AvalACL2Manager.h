// *********************************************************************************
//	Avalボード操作 クラス
//	[Ver]
//		Ver.01    2014/07/07  vs2005 対応
//
//	[メモ]
//		・Avalへの直接アクセスは、禁止
//		・FC の AvalGrabberACL2Batchに相当
//		・GrabberManagerを継承することで、IportManager関連と同一I/Fで操作可能
//	
//-------------------------------------
// [基本的な使用の考え方]
//		・IportMgr で言う所の Angle にしか対応していない。１インスタンス複数角度
//			・複数Camsetを１インスタンスで取り込む事は考えていない
//				その場合は、複数インスタンスとする。
//
// *********************************************************************************


#pragma once


// 疵検ライブラリ
#include "..\KizuLib\CLASS\KizuMacro.h"											// 疵検マクロ
#include "..\KizuLibMFC\CLASS\ThreadManager.h"									// ワーカースレッドマネージャー
#include "..\KizuMMX\MemCopy.h"												// MMXライブラリ
#include "..\..\Include\LibSelect.h"										// ライブラリ切替ヘッダー


// テスト画像取得用
#include "..\ImgLib\CLASS\KizuAviManager.h"									// AVIファイル
#include "..\KizuLib\CLASS\KizuBitmapManager.h"								// BitMapファイル


// カメラ関係
#include "..\IportLib\IportCommon.h"										// 撮像系定義
#include "..\IportLib\GrabberManager.h"										// 撮像系基底


// ローカル
#include "AvalManagerIF.h"
#include "AvalACL2Base.h"


// インポート
using namespace KizuLib;
using namespace ImgLib;


namespace AvalLib
{


	//===================================================================
	// カメラ取込 共通部本体
	class AvalACL2Manager : public GrabberManager
	{

	//// メンバー定数
	protected:
		static const int	MAX_CHANNEL	= 4;						// 最大４チャンネル
		static const int	MAX_BOARD	= 8;						// 最大８ボード
		static const int	MAX_CAMNUM	= MAX_CHANNEL*MAX_BOARD;	// 最大カメラ接続数




	public:
		AvalACL2Manager(CString sMyManagerName, CString sMyGrabberName);
		virtual ~AvalACL2Manager(void); 

		



		//----------------
		// IF実装

		// 一回のみ
		virtual BOOL Init(int board_id, int board_ch_id, const char* ini_filename, int nbuff, DWORD nFrameW, DWORD nFrameH, BYTE nDummyVal);	// 初期化
		virtual BOOL Init(int nchannel, const char* ini_filename, int nbuff, DWORD nFrameW, DWORD nFrameH, BYTE nDummyVal);						// 初期化
		virtual BOOL Init(int nchannel, int const* board_id, int const* board_ch_id, const char* ini_filename, int nbuff, DWORD nFrameW, DWORD nFrameH, BYTE nDummyVal);	// 初期化
		virtual void Dispose();

		virtual bool		Start();									 	// 実行待機版スレッド開始
		virtual bool		Stop();											// スレッド停止


		// 撮像系
		virtual BOOL		StartGrab();									// 撮像開始
		virtual BOOL		StopGrab();										// 撮像停止
		virtual bool		Reset();										// リセット (Init直後の状態に戻す)


		// 画像情報
        virtual long		GetFrameWidth()     const { return NULL != my_pGrabber ? my_pGrabber->GetWidth() : 0; }
        virtual long		GetFrameHeight()    const { return NULL != my_pGrabber ? my_pGrabber->GetHeight() : 0; }
        virtual long		GetFrameNChannel()  const { return my_nChannel; }
        virtual long		GetFramePixelNBit() const { return NULL != my_pGrabber ? my_pGrabber->GetPixelNBit() : 0; }

		virtual int			GetChannel()	const {return my_nChannel;}


		virtual AvalACL2Base &GetBase( int i ) { return my_pGrabber[i]; }


		// サンプル画像操作系
		virtual bool		OpenBmp(char const* cPathFormat, int nNoFrom, int nNoTo, int nCycleTimer);	// 連続ビットマップ定義
		virtual void		CloseBmp();											// 連続ビットマップ解放
		virtual bool		OpenAvi(char const* cPathFormat, int nCycleTimer);	// AVI定義
		virtual void		CloseAvi();											// AVI解放
		virtual bool		OpenManual();
		virtual void		CloseManual();
		virtual bool		OnManualFrameGrabbed(BYTE* pImgs, DWORD nBytes, DWORD nCamset);
		virtual void		ChangeSmpPathFormat(int nMode = 0) { my_nSmpPathFormatMode = nMode;}	// フォーマット切替
	protected:
		virtual void		OnBmpFrameGrabbed();							// 全カメラセット、カメラ角度に対してビットマップ画像をセット
		virtual void		OnAviFrameGrabbed();							// 全カメラセット、カメラ角度に対してAVI画像をセット


	protected:
		virtual void		InitInfo(DWORD nFrameW, DWORD nFrameH, BYTE nDummyVal);		// 初期情報セット
		virtual void		DisConnect() {return;};							// 今回無し



	//// プロパティ関係
	public:
		// Set
		virtual void		SetAddInfo(DWORD idx, HANDLE h) { my_hAddInfo[idx]=h; }	// フレーム情報に付与する情報
		virtual void		SetImgTurn_LR(bool on)	  { my_bImgTurn_LR = on;  }		// 画像左右反転	 ※未実装
		


	//// メンバー関数
	protected:
		virtual int			ThreadMainLoop();								// スレッドメインループをシャドウ
		virtual int			ThreadFirst();									// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
		virtual int			ThreadLast()				{ return 0;};		// スレッド終了前処理 (終了シグナル後にコールバック)
		virtual void		ThreadEvent(int nEventNo)	{};					// スレッドイベント発生
		

		virtual void		OnFrameGrabbed(void** pImgs, DWORD nFrameNum, int* nSeqNum, __int64 nFastTime);	// 1カメラセット分の画像を取得して、スレッドループキューにキューイング
		virtual	void		OnGrabError(unsigned long frame_num, int grabber_index, AvalACL2Base::FrameWaitResult err);		// エラー定義
		virtual void		OnAddInfo(EM_DELIVERY_ID em, DWORD nFrameNum, DWORD set, DWORD ang, DWORD st);	// 付帯情報キューイング



	//// メンバー変数
	protected:

        // grabber base
        int								my_nGrabber;						// カメラ接続数
        AvalACL2Base*					my_pGrabber;						// 撮像クラス [配列]


		// 基本情報
		EM_GRAB_MODE					my_emGrabMode;						// 撮像種別
        DWORD							my_nFrameNum;						// フレームNo 
		DWORD							my_nImgSize;						// フレームサイズ (1画像のみのサイズ)
		DWORD							my_nFrameW;							// 幅   フレーム画像サイズ
		DWORD							my_nFrameH;							// 高さ フレーム画像サイズ
		int								my_nChannel;						// 画像数

		DWORD							my_nBaseBufNum;						// バッファ数
		std::vector<DWORD>				my_nBatchSize;						// 一回の画像取得数
		std::vector<DWORD>				my_nMaxBatchSize;					// 最大画像取得数


		void*							my_pDummyBuf;						// フレーム画像が壊れている場合などのダミーフレーム画像 (Initで生成)

		HANDLE							my_hAddInfo[MAX_CAMNUM];			// フレーム情報に付与する情報
		bool							my_bImgTurn_LR;						// 画像左右反転時 true


		// メインスレッド関係
		HANDLE							my_hHwRun;							// HW撮像実行中(=画像取込中)は常にシグナル状態。 マニュアル操作
		HANDLE							my_hInited;
		bool							my_bThreadLoop;						// メインスレッドのループ時 true

		// テスト画像用
		KizuAviManager*					mycls_Aiv[MAX_COLOR_CHANNEL][MAX_CAMNUM];		// AVIファイル読み込みクラス
		HANDLE							my_evSmpImgTimer;					// AVI画像等定周期で画像を読み込む場合
		int								my_nSmpPathFormatMode;				// 読込みフォーマットタイプ (0:通常orCHで全部同じ画像 1:CH毎に別画像)
		CString							my_sSmpPathFormat;					// 読込みフォーマット
											// 【bmp時】
												// my_nSmpPathFormatMode=0 : カメラセット(0オリジン), カメラ角度(0オリジン), 連番
												// my_nSmpPathFormatMode=1 : カメラセット(0オリジン), カメラ角度(0オリジン), Ch(0オリジン), 連番
											// 【avi時】
												// my_nSmpPathFormatMode=0 : カメラセット(0オリジン), カメラ角度(0オリジン)
												// my_nSmpPathFormatMode=1 : カメラセット(0オリジン), カメラ角度(0オリジン), Ch(0オリジン)
		int								my_nSmpIntervalNo[2];				// 読み込み開始No,終了No
		int								my_nSmpNowNo;						// 現在の読み込み
		int								my_nSmpCycleTimer;					// 読み込み周期 [ms]
		DWORD *m_nSeqNum;
	};
};