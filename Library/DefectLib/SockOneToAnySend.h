// *********************************************************************************
//	TCP/IP 通信 のPC間通信クラス １対他 (Send:クライアント   Recv:サーバー)
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//		・高速通信、大量データ通信を考慮した作りとする
//			※とはいっても KizuToSend とほとんど同じになった・・・
//		・Addr部のみ分割送信(2M以上)に対応
// *********************************************************************************

#pragma once
// 半固定
#include "..\..\Include\KizuCommon.h"									// 疵検インクルード
#include "..\KizuLibMFC\CLASS\ThreadManager.h"							// ワーカースレッドマネージャー
#include "..\KizuLibMFC\Class\ThreadLoopQueue.h"
#include "..\KizuLib\Class\KizuPerformanceCounter.h"					// タイマー
#include "..\KizuLib\Class\AutoLock.h"									// クリティカルセクション制御クラス
#include "..\KizuSOCK3\SockBase.h"										// ソケット操作クラス

#include "SockOneToAny.h"

using namespace KizuLib;
using namespace KizuSock3;

class SockOneToAnySend : public ThreadManager
{
//// 定数
public:
	// 本クラスのソケット状態ステータス
	enum SOCK_STATE { 
		STATE_NONE = 0,													// 初期状態 (切断中)
		STATE_IDLE,														// 準備完了 (接続中)
		STATE_HEAD,														// ヘッダ部 送信中
		STATE_DATA														// データ部 送信中
	};

	enum DATA_SUB_MODE {
		STATE_SUB_NONE = 0,
		STATE_SUB_ADDR0,
		STATE_SUB_ADDR1
	};


// ローカル定数
protected :
	static const int PARAM_SOCK_MAX				= 1;					// 最大接続人数
	static const int PARAM_SOCK_TIMEOUT			= 10000;				// ソケット タイムアウト
	static const int PARAM_SOCK_RETRY_TIME		= 2000;					// ソケット リトライ周期
	static const int PARAM_SOCK_SEND_BUFSIZE	= (4*1024*1024);		// LANボードの送信バッファサイズ (4M) 4MがMAX？？

	static const int PARAM_TIME_HELCY_SEND		= (10*1000);			// ヘルシー送信。次回何か送ってから本秒数経過

public:
	SockOneToAnySend();
	virtual ~SockOneToAnySend(void);

	void	SetSession(const char *cSession ){ my_cSession = cSession; }	// 接続セッションセット 
	void	SetRetryMode (bool bRetry=true) { my_bRetryMode=bRetry; }
	void	SetTLQue( ThreadLoopQueue<SOCK_F_HEAD2>* p ) { mque_pDeli = p; }


	//// ソケット操作
	void SetConnect	  () { SetEvent(my_evConnect);}						// 回線接続要求 (非同期)
	void SetDisConnect() { SetEvent(my_evDisConnect);}					// 回線切断要求 (非同期)

	HANDLE		GetEvClose()   const {return my_evAnserClose; };		// 外部への切断通知イベント
	HANDLE		GetEvConnect() const {return my_evAnserConnect; };		// 外部への接続通知イベント


	//// 外部アクセス
	bool IsConnect()  const { return STATE_NONE == my_emSockState ? false :true; }	// 接続状態
	// Get
	SOCK_STATE  GetSockState() const {return my_emSockState; };			// 現在のステータス
	CString		GetSockStateName()	const {return GetSockStateName(my_emSockState);};	// 現在のステータス名称
	CString		GetSockStateName(SOCK_STATE st)	const;									// 指定のステータス名称

//// メンバー関数
protected:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	void ThreadWaitFor(int nDefWaitNum, int* nNowWaitNum);				// スレッドのWaitFor待ちの直前処理
	

	// バッファ系
	void QueBufSendBack();												// 現バッファを返却する
	void QueAllSendBack();												// すべてのキューを返却する

	// 通信操作
	void SendStart();													// 疵情報伝文送信 開始処理
	void SendEnd();														// 疵情報伝文送信 完了処理
	void SendHeader();													// ヘッダー部送信開始
	void SendData();													// データ部送信開始
	void SendHelcy();													// ヘルシー伝文送信 処理

	// ソケット操作
	void Connect(bool bLogDsp);											// 接続
	void Close();														// 切断 & リトライ設定


protected :
	// 各インスタンス
	SockBase*				mcls_pSock;									// ソケット操作クラスインスタンス
	KizuPerformanceCounter	mcls_Time;									// 取得した時間(ストップウォッチ機能を使用)

	// 受渡キュー
	ThreadLoopQueue<SOCK_F_HEAD2>*		mque_pDeli;						// 送受データ受け渡しキュー(回転寿司形式)	[LIB⇔AP間]
	

	// イベントハンドル
	HANDLE					my_tmReTryConnect;							// 再接続要求タイマイベント
	HANDLE					my_tmHelcy;									// ヘルシー送信タイマイベント

	HANDLE					my_evConnect;								// 接続要求イベント
	HANDLE					my_evDisConnect;							// 切断 イベント (手動でも切れるように公開もして置く)
	HANDLE					my_evSendTimeout;							// 送信タイムアウト イベント
	HANDLE					my_evSendEnd;								// 送信完了 イベント
	HANDLE					my_evAnserClose;							// 外部への切断通知イベント
	HANDLE					my_evAnserConnect;							// 外部への接続完了通知イベント

	// 同期制御
	CRITICAL_SECTION		m_csLock;									// クリティカルセクション

	// ソケット & 送信データ関連
	SOCK_STATE				my_emSockState;								// ステータス (外部ガイダンス用にしか使わないかなー)
	SOCK_F_HEAD2*			my_pHead;									// スレッドキュー受け取り && 送信データバッファ
	long					my_nSendingDataSize;						// 現在のデータ部のみの送信バイト数 (分割送信対応対策)
//	int						my_nSendAddId;								// データ部のAdd何回目の送信か
	DATA_SUB_MODE			my_emSendAddId;								// データ部のAdd何回目の送信か

	bool					my_bHelcyCancel;							// 今回のヘルシー送信はヤメ

	// その他もろもろ
	CString					my_cSession;								// iniファイルのセッション名
	bool					my_bRetryMode;								// リトライ接続の有無 (true:リトライする false:リトライしない)
};
