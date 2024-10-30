// *********************************************************************************
//	iPort接続 基底 クラス
//	[Ver]
//		Ver.01    2010/01/29  vs2005 対応
//
//	[メモ]
//		
// *********************************************************************************
#pragma once

#include <Windows.h>

// Pleora ヘッダー
	// 必須のヘッダーのみ
	// .c で必要なヘッダーは *.cで定義
#include <CyTypes.h>
#include <CyGrabber.h>


// 疵検ライブラリ
#include "..\KizuLib\CLASS\KizuMacro.h"										// 疵検マクロ
#include "..\KizuLibMFC\Class\LogFileManager.h"								// ログ管理クラス

#include "IportCommon.h"													// iPort用定数

// インポート
using namespace KizuLib;

namespace IportLib
{
	//===================================================================
	// 定数定義
	//===================================================================

	// /////
    // アクセスモード
    // /////
    enum ACCESSMODE {	ModeUnknown = 0,
						PerfDrv, 
						WinStackUDP, 
						WinStackTCP, 
						FilterDrv
					};
	// /////
	// エラーコード(実際はCyResult)
	// /////
	#define CYEXT_RESULT_BASE	(CY_RESULT_MAX)
	enum ERRCODE {		NoErr = CY_RESULT_OK,				// 0固定。  プログラム中で0=正常で聞いてよし
						ApErr = -1,
						// 以下swdetectでの拡張
						CYEXT_RESULT_IMAGE_CORRUPTED = CYEXT_RESULT_BASE,
						CYEXT_RESULT_BAD_IMAGE
				};
	#define CAST_CY_ERR(errcode)	static_cast<ERRCODE>(errcode)		// エラーコード変換
	#define RETURN_CY_ERR(errcode)	return CAST_CY_ERR(errcode)			// エラーコード変換後 return



	//===================================================================
	// 基底クラス
	//===================================================================
	class IportBase
	{
	//// 定数
	public:
		static const int GRAB_DEF_QSIZE = 4;								// グラブのリングバッファ初期サイズ

		struct	TYP_FRAME_INFO {											// 画像情報 
			long				nWidth;											// 幅	(XMLで取得)
			long				nHeight;										// 高さ	(XMLで取得)
			long				nNChannel;										// 画像のチャンネル数 (グレー画像=1,RGBカラー=3 などなど)
			long				nPixelNbit;										// 1画素のビット数 (通常 8bit)
		};


	//// ローカル定数
	protected:
		static const int MC_CONNECT_RETRY_COUNT = 10;						// コネクトリトライ回数

	    struct	TYP_DEVICE_ENTRY {											// カメラ接続設定
            BOOL				valid;											// 有効状態 (データセット完了時 true)
			IportLib::ACCESSMODE emMode;										// カメラ接続タイプ
            unsigned char		adapter_mac[6];
            char				mac[32];										// 文字列
            char				ip[32];											// 文字列
            unsigned char		id;												// Device ID
																					// 0: Gigabit Ethernet IP engines
																					// 1: Fast Ethernet (10/100 Megabits/sec) IP engines
            unsigned char		module_id;										// iPORT  IP エンジンのタイプ
																					// 0x01: Camera Link module
																					// 0x02: Video Decoder module
																					// 0x04: No module (streaming module)
																					// 0x05: Symagery SC3000 module
            unsigned char		smodule_id;										// iPORT  IP エンジンのサブタイプ
        };


	//// 公開関数
	public:
		IportBase(CString sThreadName ="iPortBase");
		virtual ~IportBase(void);

        // 初期化
        bool			Init();												// 関数初期化
        virtual void	Dispose();											// 関数開放


		//// XMLファイルを使って (1)～(4)までを実施
		static ERRCODE	GetXMLFileToIP(const char* filename, CString* ip_addr, CString* mac_addr, CString* mc_addr);		// iPort設定XMLファイルから IPアドレス関係を取得
        ERRCODE			SetupFromXMLFile(const char* filename, 
										 bool bSaveIpAddr=true, bool bIsSlave=false, long nAdapterIndex=0, int nGrabQueSize=GRAB_DEF_QSIZE);	// iPort設定XMLファイルを使用して、カメラに接続
		ERRCODE			SetupFromXMLFile(const char* filename, 
										 const char* mac_addr, const char* ip_addr, char const* mc_addr, 
										 bool bSaveIpAddr=true, bool bIsSlave=false, long nAdapterIndex=0, int nGrabQueSize=GRAB_DEF_QSIZE);	// iPort設定XMLファイルを使用して、カメラに接続

	protected:
		ERRCODE			SetupBuffer(int nGrabQueSize=GRAB_DEF_QSIZE);		// カメラ画像バッファ に iPort取り込み画像領域を確保

	public:
		//// カメラ操作
		// (1) デバイスの選択
		// (2) カメラの選択
		// (3) デバイスへの接続＆カメラのbind
		bool			IsConnected() const;								// 接続確認
		bool			IsMulticastSlave() const;							// マスターorスレーブ (FALSE:master or non  TRUE:multicast)
		// (4) grabberの設定
		
		// (5) grabの制御
		ERRCODE			StartGrab();										// カメラ取込開始
		ERRCODE			StopGrab();											// カメラ取込停止
		bool			IsStarted() const;									// カメラ取込確認
		// Grab();								// Oneショット取り込みは未実装

		// (6) デバイスの切断
		ERRCODE			Disconnect();										// デバイスの切断	
		// (7) grabした画像の取り扱い
		ERRCODE			LockFrame(void*& p, unsigned long& len, unsigned long& iid, unsigned long& status);		// 画像読み込み(画像取り込み完了までブロッキング)

			// status取り扱い
		static const unsigned long MaskFrameOverrun       = 0x00000001;
		static const unsigned long MaskGrabberFIFOOverrun = 0x00000002;
		static const unsigned long MaskImageDropped       = 0x00000004;
		static const unsigned long MaskPartialLineMissing = 0x00000008;
		static const unsigned long MaskFullLineMissing    = 0x00000010;

        static inline bool IsFrameOverrun(unsigned long status)			{ return !!(status & MaskFrameOverrun);	}		// そのままだと unsigned long を強制的に boolに変換でワーニングが出るから、2重否定で強制的にboolに変換
        static inline bool IsGrabberFIFOOverrun(unsigned long status)	{ return !!(status & MaskGrabberFIFOOverrun);}
        static inline bool IsImageDropped(unsigned long status)			{ return !!(status & MaskImageDropped); }
        static inline bool IsPartialLineMissing(unsigned long status)	{ return !!(status & MaskPartialLineMissing); }
        static inline bool IsFullLineMissing(unsigned long status)		{ return !!(status & MaskFullLineMissing); }
        static inline bool IsLineMissing(unsigned long status)			{ return !!(status & (MaskPartialLineMissing | MaskFullLineMissing)); }
        

		void			ReleaseFrame() const;								// 画像読み込み領域返却
		void			AbortLock() const;									// LockForReadから強制的に抜ける
	
		// (8) Queueの操作
        void			ClearQueueItemInBuffer();
        unsigned long	GetNQueueItemsInBuffer() const;

        // (9) Reset
        ERRCODE			ResetModule();
		

		// (10) 伝送系
		ERRCODE			SendSerialCommand(char const* sSend, CString* sRecv, int timeout=40, bool bDsp=true );	// シルアルコマンド 送信後、アンサー受信
		ERRCODE			SendSerialCommand(char const* sSend, int timeout=40, bool bDsp=true);					// シルアルコマンド 送信のみ


	//// プロパティ関係
	public:
		// 外部からのインスタンス
		void			SetLogMgr(LogFileManager* p) { mcls_pLog = p; };						// ログファイルセット
		char const*		GetThreadName()				 {return my_sThreadName.GetBuffer();}		// 自スレッド名

		// Cyオブジェクト取得
		inline CyGrabber*         GetCyGrabber()	const { return my_pCyGrabber; }
		inline CyImageBuffer*     GetCyImageBuffer()const { return my_pCyBuffer; }
		inline CyCameraInterface* GetCyCamera()		const { return my_pCyCamera; }


		// カメラ接続情報
        inline BOOL IsDeviceEntryValid()	const { return my_typDevEntry.valid; }
        inline const char* GetDeviceIP()	const { return my_typDevEntry.ip; }
        inline const char* GetDeviceMAC()	const { return my_typDevEntry.mac; }

		// 画像情報
		TYP_FRAME_INFO const& GetFrameInfo()const { return (TYP_FRAME_INFO&)my_typFrameInfo;}
        inline long		GetFrameWidth()     const { return my_typFrameInfo.nWidth; }
        inline long		GetFrameHeight()    const { return my_typFrameInfo.nHeight; }
        inline long		GetFrameNChannel()  const { return my_typFrameInfo.nNChannel; }
        inline long		GetFramePixelNBit() const { return my_typFrameInfo.nPixelNbit; }

		// grabステータス
		unsigned long	GetResendCount()	const { return my_nResendCount;}
		unsigned long	GetLostCount()		const { return my_nLostCount;}
		bool			IsWaitingForLock()	const { return my_bLockWaiting; }



	//// メンバー変数
	protected:

		// Pleoraクラス
		CyGrabber*				my_pCyGrabber;								// グラバ接続 インスタンス
        CyImageBuffer*			my_pCyBuffer;								// カメラ画像バッファ インスタンス
        CyCameraInterface*		my_pCyCamera;								// カメラ情報 インスタンス (この中にCyGrabberも入っている)

		// 各インスタンス
		LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス

		// カメラ接続情報
		TYP_DEVICE_ENTRY		my_typDevEntry;								// カメラ接続設定
		char					my_cCameraName[128];						// カメラ機種 (Standard CameraLink Camera など)

		// 画像情報
		TYP_FRAME_INFO			my_typFrameInfo;							// 画像情報

		// grab情報
		unsigned long			my_nResendCount;							// パケット再送回数 (内部的なやつなので、再送して成功する場合もある)
		unsigned long			my_nLostCount;								// パケット損失個数 (LockForReadで遅れて取り込めなかったやつではないので注意)
		bool					my_bLockWaiting;							// LockForRead中:true
		bool					my_bIsSlave;								// スレーブ実行時 true
		///////bool					my_bIsLock;									// LockForRead正常完了後～SignalReadEndをするまでの間
		//bool					my_bIsFreeRun;								// フリーラン時:true (SetupFromXMLFile前にセットしておくこと)
		//int						my_nFreeRunHz;								// 取り込む周波数 [Hz]

		// その他もろもろ
		CString					my_sThreadName;								// 自スレッド名
		
	};
};