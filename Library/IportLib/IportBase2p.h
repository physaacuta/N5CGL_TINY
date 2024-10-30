#pragma once

// ローカル
#include "IportBase.h"														// iPort基底クラス


// インポート
using namespace KizuLib;
using namespace IportLib;

namespace IportLib
{
	class IportBase2p
	{
	//// 公開関数
	public:
		IportBase2p(CString sThreadName ="iPortBase2p");
		virtual ~IportBase2p(void);

        // 初期化
        bool			Init();												// 関数初期化
        virtual void	Dispose();											// 関数開放

		inline IportBase&  GetBase(int n) { return mycls_Iport[n]; }		// Base取得

		//// iPort操作系
        ERRCODE			SetupFromXMLFile(const char* filename1, const char* filename2, 
										 long nAdapterIndex1=0, long nAdapterIndex2=0, 
										 bool bSaveIpAddr=true, bool bIsSlave=false, int nGrabQueSize=IportBase::GRAB_DEF_QSIZE);
		ERRCODE			SetupFromXMLFile(const char* filename, 
										 const char* mac_addr1, const char* ip_addr1, long nAdapterIndex1, char const* mc_addr1,
										 const char* mac_addr2, const char* ip_addr2, long nAdapterIndex2, char const* mc_addr2,
										 bool bSaveIpAddr=true, bool bIsSlave=false, int nGrabQueSize=IportBase::GRAB_DEF_QSIZE);
		ERRCODE			SetupFromXMLFile(const char* filename, long nAdapterIndex1,
										 const char* mac_addr2, const char* ip_addr2, long nAdapterIndex2, char const* mc_addr2,
										 bool bSaveIpAddr=true, bool bIsSlave=false, int nGrabQueSize=IportBase::GRAB_DEF_QSIZE);


		//// カメラ操作
		// (1) デバイスの選択
		// (2) カメラの選択
		// (3) デバイスへの接続＆カメラのbind
		bool			IsConnected() const { return mycls_Iport[0].IsConnected(); }			// 接続確認
		bool			IsMulticastSlave() const { return mycls_Iport[0].IsMulticastSlave(); }	// マスターorスレーブ (FALSE:master or non  TRUE:multicast)
		// (4) grabberの設定
		
		// (5) grabの制御
		ERRCODE			StartGrab();										// カメラ取込開始
		ERRCODE			StopGrab();											// カメラ取込停止
		bool			IsStarted() const { return mycls_Iport[0].IsStarted(); }				// カメラ取込確認

		// (6) デバイスの切断
		ERRCODE			Disconnect();										// デバイスの切断	
		// (7) grabした画像の取り扱い
		ERRCODE			LockFrame(void*& p0, unsigned long& len0, unsigned long& iid0, unsigned long& stat0,
								  void*& p1, unsigned long& len1, unsigned long& iid1, unsigned long& stat1);		// 画像読み込み(画像取り込み完了までブロッキング)

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
		void			SetSyncPort(int nSkipCnt) { my_nSkipPort=nSkipCnt;}	// ズレ補正処理

		// (8) Queueの操作
		void			ClearQueueItemInBuffer() { mycls_Iport[0].ClearQueueItemInBuffer(); mycls_Iport[1].ClearQueueItemInBuffer(); }
		unsigned long	GetNQueueItemsInBuffer(bool isPrimary=true) const { return mycls_Iport[ (isPrimary?0:1) ].GetNQueueItemsInBuffer(); }

		// (9) Reset
        ERRCODE			ResetModule();
		

		// (10) 伝送系 (すべてマスターに対して実施)
		ERRCODE			SendSerialCommand(char const* sSend, CString* sRecv, int timeout=40, bool bDsp=true) { return mycls_Iport[0].SendSerialCommand(sSend, sRecv, timeout, bDsp); }	// シルアルコマンド 送信後、アンサー受信
		ERRCODE			SendSerialCommand(char const* sSend, int timeout=40, bool bDsp=true) { return mycls_Iport[0].SendSerialCommand(sSend, timeout, bDsp); }							// シルアルコマンド 送信のみ


	//// プロパティ関係
	public:
		// 外部からのインスタンス
		void			SetLogMgr(LogFileManager* p) { mycls_Iport[0].SetLogMgr(p); mycls_Iport[1].SetLogMgr(p); };	// ログファイルセット
		char const*		GetThreadName()				 {return mycls_Iport[0].GetThreadName();}		// 自スレッド名


		// 画像情報
        inline long		GetFrameWidth()     const { return mycls_Iport[0].GetFrameWidth()+mycls_Iport[1].GetFrameWidth(); }
        inline long		GetFrameHeight()    const { return mycls_Iport[0].GetFrameHeight(); }
        inline long		GetFrameNChannel()  const { return mycls_Iport[0].GetFrameNChannel(); }
        inline long		GetFramePixelNBit() const { return mycls_Iport[0].GetFramePixelNBit(); }


		// grabステータス
		unsigned long	GetResendCount()	const { return mycls_Iport[0].GetResendCount() + mycls_Iport[1].GetResendCount();}
		unsigned long	GetLostCount()		const { return mycls_Iport[0].GetLostCount() + mycls_Iport[1].GetLostCount();}
	//	bool			IsWaitingForLock()	const { return my_bLockWaiting; }	


	//// ローカル定数
	protected:
        IportBase	mycls_Iport[2];											// iPortベースクラス本体。 (0:マスター  1:スレーブポート)
        ERRCODE     my_lock_err[2];											// LockFrameのエラー
	
		// フレーム同期操作
		int			my_nSkipPort;											// 1カメラ内でiPortシーケンスNoズレ補正用 (0:無し 1:マスターを1回スキップ -1:スレーブを1回スキップ)
	};
};