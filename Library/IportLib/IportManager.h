#pragma once


//======================================================================
// 太平製カメラ取込は、pitchは8しか考えていない
//   違っていたら、Connect でエラーにする
//======================================================================
#pragma warning(disable : 4819)								// Unicodeで保存

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


// ローカル
#include "GrabberManager.h"													// 撮像基底クラス
#include "IportBase.h"														// iPort基底クラス


// インポート
using namespace KizuLib;
using namespace ImgLib;


namespace IportLib
{

	//===================================================================
	// カメラ取込 I/F
	//===================================================================
	interface IportManagerIF
	{

	//// 公開関数
	public:

		// 一回のみ
		virtual bool		Init(DWORD nFrameW, DWORD nFrameH, DWORD camset=1, DWORD angle=1, BYTE nDummyVal=0x80) = 0;	// 初期化
	

		// iPort操作系
		virtual IportLib::ERRCODE	Connect(DWORD set, DWORD ang, CString sXmlPath, 
										bool bSaveIpAddr=true, bool bIsSlave=false, long nAdapterIndex=0, 
										int nGrabQueSize=IportBase::GRAB_DEF_QSIZE) = 0;	// iPortに接続 (1p) 設定:XML, Master:XML
		virtual IportLib::ERRCODE	Connect(DWORD set, DWORD ang, CString sXmlPath, 
										const char* mac_addr, const char* ip_addr, char const* mc_addr, 
										bool bSaveIpAddr=true, bool bIsSlave=false, long nAdapterIndex=0, 
										int nGrabQueSize=IportBase::GRAB_DEF_QSIZE) = 0;	// iPortに接続 (1p) 設定:XML, Master:IP直


		virtual IportLib::ERRCODE	Connect(DWORD set, DWORD ang, CString sXmlPath, long nAdapterIndex1,
										const char* mac_addr2, const char* ip_addr2, long nAdapterIndex2, char const* mc_addr2,
										bool bSaveIpAddr=true, bool bIsSlave=false, int nGrabQueSize=IportBase::GRAB_DEF_QSIZE) = 0;	// iPortに接続(2p) 設定:XML, Master:XML, Slave:IP直
		virtual IportLib::ERRCODE	Connect(DWORD set, DWORD ang, CString sXmlPath,
									    const char* mac_addr1, const char* ip_addr1, long nAdapterIndex1, char const* mc_addr1,
										const char* mac_addr2, const char* ip_addr2, long nAdapterIndex2, char const* mc_addr2,
										bool bSaveIpAddr=true, bool bIsSlave=false, int nGrabQueSize=IportBase::GRAB_DEF_QSIZE) = 0;	// iPortに接続(2p) 設定:XML, Master:IP直, Slave:IP直


		//virtual void		DisConnect() = 0;									// iPortと切断


				// その他小物
		virtual bool		SendSerialTemp(DWORD set, DWORD ang, float& temp) = 0;	// カメラ温度を取得 (カメラと接続時しかダメ)
		virtual void		SyncFrame(DWORD set, DWORD ang, WORD nSkipCnt) =0;


	//// プロパティ関係
	public:
		// オブジェクト取得

		//(IportBase&)*mycls_pIport[0] がわからない人向け
		//		mycls_pIport[0] は、IportBaseのポインタが入っている
		//		*mycls_pIport[0]で、IportBaseの本体にアクセス
		//		それを(IportBase&)で型変換
		virtual inline IportBase&		GetClsIpot(DWORD set, DWORD ang, bool isPrimary=true) = 0;
		virtual inline int				GetCamIndex(DWORD set, DWORD ang) const = 0;						// 全部0オリジン
		virtual inline bool IsPT2000() const = 0;


		// Set
		virtual void		SetResetModuleOnReset(bool on) = 0;
		virtual void		SetAddInfo(DWORD set, DWORD ang, HANDLE h) = 0;	// フレーム情報に付与する情報
		virtual void		SetImgTurn_LR(bool on) = 0;						// 画像左右反転
		// Get
		virtual void*		GetDummyBuf() = 0;

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
		//virtual bool		IsInit()			 const = 0;
		//virtual bool		IsGrabRun()			 const = 0;
	};


	//===================================================================
	// カメラ取込 共通部本体
	//===================================================================
	class IportManager :	public		GrabberManager,
							implements	IportManagerIF
	{

	//// メンバー定数
	protected:
		static const int	LOCK_QUE_RETRY_CNT	= 4;						// ロック中キューを取得した場合のリトライ回数
		static const int	MAX_IPORT_CHANNEL	= MAX_COLOR_CHANNEL;	// 3		// 最大４色チャンネル
		static const int	MAX_IPORT_CAMANGLE	= 4;						// 最大４角度
		static const int	MAX_IPORT_CAMSET	= 8;						// 最大８カメラセット
		static const int	MAX_IPORT_CAMNUM	= MAX_IPORT_CAMSET*MAX_IPORT_CAMANGLE;	// 最大カメラ接続数
		

	//// 公開関数
	public:
		IportManager(CString sMyManagerName);
		virtual ~IportManager(void);

		//----------------------------------------
		// grab 実装 ※ここではしない。さらに継承先で実装
		virtual bool		Start() = 0;									// スレッド開始 ※ このスレッドは、ホントにスレッドが実行されるまでロックされる
		virtual bool		Stop() = 0;										// スレッド停止
	

		//----------------------------------------
		// サンプル画像操作系 実装
	public:	
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





		// その他小物
		virtual void		SyncFrame(DWORD set, DWORD ang, WORD nSkipCnt) { int ch=GetCamIndex(set, ang); my_nSkipCnt[ch]+=nSkipCnt;}


	//// プロパティ関係
	public:
		//virtual inline		ThreadLoopQueue<FRAME_DATA>* GetQueFrame()	   { return myque_pFrame; }
		//virtual inline		ThreadQueue<DELIVERY_INFO>&	 GetQueInfo()	   { return myque_Info; }


		// Set
		virtual void		SetResetModuleOnReset(bool on) { my_reset_module = on; }
		virtual void		SetAddInfo(DWORD set, DWORD ang, HANDLE h) { DWORD ch=GetCamIndex(set,ang); my_hAddInfo[ch]=h; }	// フレーム情報に付与する情報
		virtual void		SetImgTurn_LR(bool on)	  { my_bImgTurn_LR = on; }			// 画像左右反転	
//		virtual void		SetCamsetNo(DWORD set, DWORD ang, DWORD no){ DWORD ch=GetCamIndex(set,ang); my_nCamsetNo[ch]=no; }	// カメラセットNo(0オリジン)
		// Get
		virtual void*		GetDummyBuf()			  { return my_pDummyBuf; }
//		virtual int			GetCamsetNo(DWORD set, DWORD ang) { DWORD ch=GetCamIndex(set,ang); return my_nCamsetNo[ch]; }

		// ステータス
		virtual DWORD		GetResendTotal()	const { return my_nResendTotal; }
		virtual DWORD		GetLostTotal()		const { return my_nLostTotal;}
		virtual DWORD		GetQueueCount()		const { return my_nQueueCount;}
		virtual DWORD		GetMaxQueueCount()	const { return my_nMaxQueueCount;}
		virtual DWORD		GetFrameNo()		const { return my_nFrameNum;}
		virtual int			GetChannel()		const { return my_nChannel;}
		//virtual bool		IsInit()			const { return my_bInit;}
		//virtual bool		IsGrabRun()			const { return my_bGrabRun;}

	//// メンバー関数
	protected:
		virtual void		OnAddInfo(EM_DELIVERY_ID em, DWORD nFrameNum, DWORD set, DWORD ang, DWORD st);	// 付帯情報キューイング


	//// メンバー変数
	protected:

		//// クラス
		//LogFileManager					mcls_LogMe;							// ローカル専用
	
		//// Que
		//ThreadLoopQueue<FRAME_DATA>*	myque_pFrame;						// フレーム画像受け渡しキュー(回転寿司形式) (外部からインスタンスをもらう)
		//ThreadQueue<DELIVERY_INFO>		myque_Info;							// 付加情報受け渡しキュー (自分でインスタンスを作る)

		// 基本情報
		//EM_GRAB_MODE					my_emGrabMode;						// 撮像種別
		DWORD							my_nCamsetNum;						// 幅方向数
		DWORD							my_nAngleNum;						// 角度数
		DWORD							my_nFrameNum;						// フレームNo 
		DWORD							my_nResendTotal;					// パケット再送回数
		DWORD							my_nLostTotal;						// パケット損失個数
		DWORD							my_nQueueCount;						// 取込待ちしているフレーム数
		DWORD							my_nMaxQueueCount;					// 取込待ちしているフレーム数
		DWORD							my_nImgSize;						// フレームサイズ (1画像のみのサイズ)
		DWORD							my_nFrameW;							// 幅   フレーム画像サイズ
		DWORD							my_nFrameH;							// 高さ フレーム画像サイズ
		int								my_nChannel;						// 画像数
		WORD							my_nSeqNum[MAX_IPORT_CAMNUM];		// 自分で持っているシーケンス番号 (iPort番号と一致するようにしている) (start時のシーケンス番号で初期化する)
		//bool							my_bInit;							// Init済みでtrue
		//bool							my_bGrabRun;						// Grabbing中true

		bool							my_reset_module;					// Reset()時にResetModuleを呼ぶかどうか (SpyderGigEの時に使用。Piranha2で使用しちゃダメっぽい)
		void*							my_pDummyBuf;						// フレーム画像が壊れている場合などのダミーフレーム画像 (Initで生成)

		DWORD							my_nCamsetNo[MAX_IPORT_CAMNUM];		// カメラセット(0オリジン)
		HANDLE							my_hAddInfo[MAX_IPORT_CAMNUM];		// フレーム情報に付与する情報
		WORD							my_nSkipCnt[MAX_IPORT_CAMNUM];		// 指定枚数強制スキップ

		bool							my_bImgTurn_LR;						// 画像左右反転時 true

		// メインスレッド関係
		HANDLE							my_hThreadReady;					// ホントにスレッドが実行するまで待機用
		HANDLE							my_hThreadGoStop;					// 実行可能中(=画像取込中)は常にシグナル状態。 マニュアル操作
		bool							my_bThreadLoop;						// メインスレッドのループ時 true

		// テスト画像用
		KizuAviManager*					mycls_Aiv[MAX_IPORT_CHANNEL][MAX_IPORT_CAMNUM];		// AVIファイル読み込みクラス
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
	};
};
