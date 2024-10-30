// *********************************************************************************
//	TCP/IP 通信 のPC間通信クラス １対他 (Send:クライアント   Recv:サーバー)
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//		分割受信は必要なさげなので、未実装とする
// *********************************************************************************

#pragma once
// 半固定
#include "..\..\Include\KizuCommon.h"									// 疵検インクルード
#include "..\KizuLibMFC\CLASS\ThreadManager.h"							// ワーカースレッドマネージャー
#include "..\KizuLibMFC\Class\ThreadLoopQueue.h"
#include "..\KizuLib\Class\KizuPerformanceCounter.h"					// タイマー
#include "..\KizuSOCK3\SockBase.h"										// ソケット操作クラス

#include "SockOneToAny.h"

using namespace KizuLib;
using namespace KizuSock3;

class SockOneToAnyRecv : public ThreadManager
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
		STATE_SUB_HEAD_EX,
		STATE_SUB_ADDR0,
		STATE_SUB_ADDR1
	};


// ローカル定数
protected :
	static const int PARAM_SOCK_MAX					= KS_MAXCLIENTS;	// 最大接続人数なし
	static const int PARAM_SOCK_TIMEOUT				= 10000;			// ソケット タイムアウト
	static const int PARAM_SOCK_RETRY_TIME			= 5000;				// ソケット再接続時間 [ms]
	static const int PARAM_SOCK_RECV_BUFSIZE		= (4*1024*1024);	// LANボードの送信バッファサイズ (4M) 4MがMAX？？ ←同期の送受信しか関係ないとの事

	static const int PARAM_TIMEOUT_HELCY_RECV		= (5*60*1000);		// ヘルシー伝文受信タイマアウト [ms] (5分)
//	static const int PARAM_TIMEOUT_HELCY_RECV		= (10*1000);		// ヘルシー伝文受信タイマアウト [ms] (5分)

public:
	SockOneToAnyRecv(void);
	virtual ~SockOneToAnyRecv(void);


	void	SetSession(const char *cSession ){ my_cSession = cSession; }	// 接続セッションセット 
	void	SetTLQue( ThreadLoopQueue<SOCK_F_HEAD2>* p ) { mque_pDeli = p; }


	//// 外部アクセス
	bool IsConnect(int id)  const { return STATE_NONE == my_emSockState[id] ? false :true; }	// 接続状態
	// Get
	SOCK_STATE  GetSockState(int id)		const {return my_emSockState[id]; };					// 現在のステータス
	CString		GetSockStateName(int id)	const {return GetSockStateName(my_emSockState[id]);};	// 現在のステータス名称
	CString		GetSockStateName(SOCK_STATE st)	const;									// 指定のステータス名称

	void		SetEvCloseReq()		{ SetEvent(my_evCloseReq); }		// 全クライアント切断要求シグナル
	
	// TPC/IP 通信制御
	void Close(int id);													// クライアント切断
	void CloseAll(bool bErr=false);										// 全クライアント切断 & 異常通知



//// メンバー関数
protected:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生

	// TPC/IP 通信制御
	void Reset();														// リセット
	BOOL Listen(bool bLogDsp);											// Listen
	int  Accept();														// Accept (シグナル通知用)
	void ErrCall();														// 異常通知のみ (回線切断はしない)
	BOOL RecvHead(int id);												// 指定のヘッダー受信を開始
	BOOL RecvData(int id);												// 業務伝文のデータ部受信を開始
	int RecvCheck(int id);												// 受信データのチェック

	void Recv_Helcy();													// ヘルシー伝文受信完了 (ヘッダー部のみ)

	// その他
	bool GetFreeQue(int id);	// 新規キュー取得
	void AddPoolQue(int id);	// 受信キューの登録


protected :
	// 各インスタンス
	SockBase*				mcls_pSock;									// ソケット操作クラスインスタンス
	KizuPerformanceCounter	mcls_Time;									// 取得した時間(ストップウォッチ機能を使用)
	
	// 受渡キュー
	ThreadLoopQueue<SOCK_F_HEAD2>*		mque_pDeli;						// 送受データ受け渡しキュー(回転寿司形式)	[LIB⇔AP間]

	// イベントハンドル
	HANDLE					my_tmHelcy;									// ヘルシー送信タイマイベント

	HANDLE					my_evAccept;								// 各PCよりAccept時
	HANDLE					my_evDisConnect[PARAM_SOCK_MAX];			// 切断イベント認識(ID)
	HANDLE					my_evRecvTimeOut[PARAM_SOCK_MAX];			// 受信タイムアウトイベント認識(ID)
	HANDLE					my_evRecv[PARAM_SOCK_MAX];					// 受信イベント認識(ID)

	HANDLE					my_evCloseReq;								// 回線切断要求

	// ソケット & 送信データ関連
	SOCK_STATE				my_emSockState[PARAM_SOCK_MAX];				// ステータス (外部ガイダンス用にしか使わないかなー)
	SOCK_F_HEAD2*			my_pHead[PARAM_SOCK_MAX];					// スレッドキュー受け取り 
	SOCK_F_HEAD2			my_RecvWkHead[PARAM_SOCK_MAX];				// 受信ヘッダ部のバッファ
	DWORD					my_nRecvingDataSize[PARAM_SOCK_MAX];		// 現在のデータ部のみの受信バイト数 (分割送信対応対策) (ID)
	DATA_SUB_MODE			my_emRecvAddId[PARAM_SOCK_MAX];				// データ部のAdd何回目の送信か

	bool					my_bHelcyOk[PARAM_SOCK_MAX];			// 今回のヘルシーチェックはヤメ


	// その他もろもろ
	CString					my_cSession;								// iniファイルのセッション名

};
