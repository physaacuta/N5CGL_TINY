// *********************************************************************************
//	判定PC→統括から送信クラス (クライアント機能のみ)
//	[Ver]
//		Ver.01    2007/08/09  vs2005 対応
//
//	[メモ]
//		主に、判定PC→統括への送信に使用
// *********************************************************************************

#pragma once

// 半固定
#include "..\..\Include\KizuCommon.h"									// 疵検インクルード
#include "..\KizuLib\KizuLib.h"											// 疵検ライブラリ
#include "..\KizuLib\Class\KizuMacro.h"									// マクロ集
#include "..\KizuLibMFC\Class\LogFileManager.h"							// ログ管理クラス

// 疵検クラス インクルード
#include "..\KizuLib\Class\WorkThread.h"								// ワーカースレッドクラス
#include "..\KizuLib\Class\ThreadQueue.h"								// スレッドキュークラス

#include "..\KizuSOCK3\SockBase.h"										// ソケット操作クラス

using namespace KizuLib;
using namespace KizuSock3;


class KizuToSend : public WorkThread
{
public:
	// 本クラスのソケット状態ステータス
	enum SOCK_STATE { 
		STATE_NONE = 0,													// 初期状態 (切断中)
		STATE_IDLE,														// 準備完了 (接続中)
		STATE_HEAD,														// ヘッダ部 送信中
		STATE_DATA														// データ部 送信中
	};

protected :
	// ローカル定数
	static const int PARAM_SOCK_MAX				= 1;					// 最大接続人数
	static const int PARAM_SOCK_TIMEOUT			= 10000;				// ソケット タイムアウト
	static const int PARAM_SOCK_RETRY_TIME		= 2000;					// ソケット リトライ周期
	static const int PARAM_SOCK_SEND_BUFSIZE	= (4*1024*1024);		// LANボードの送信バッファサイズ (4M)


public:
	KizuToSend();
	virtual ~KizuToSend(void);

	void Start() { WorkThread::Start(MainThread); };					// スレッド開始
	void Stop()  { WorkThread::Stop(10000); };							// スレッド終了
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ログファイルセット
	void SetPostTID(DWORD tid) { this->my_PostTid=tid;}					// PostThreadMessageの送り先 (メインインスタンス)

	// 受け渡しキュー
	ThreadQueue<SOCK_F_HEAD> gque_Deli;									// 送信データ受け渡しキュー
	ThreadQueue<SOCK_F_HEAD> gque_SendBack;								// 送信完了データ受け渡しキュー (そのまま返す)


	// ソケット操作
	void SetConnect	  (bool bRetry=true)  { my_bRetryMode=bRetry; SetEvent(my_evConnect);}		// 回線接続要求 (非同期)
	void SetDisConnect(bool bRetry=false) { my_bRetryMode=bRetry; SetEvent(my_evDisConnect);}	// 回線切断要求 (非同期)


	//// 外部アクセス
	bool IsConnect()  const { return STATE_NONE == my_emSockState ? false :true; }	// 接続状態
	// Get
	HANDLE		GetEvClose()   const {return my_evAnserClose; };		// 外部への切断通知イベント
	int			GetQueCount()  const {return gque_Deli.GetCount(); };	// 送信待ち
	int			GetMaxCount()  const {return gque_Deli.GetMaxCount();};	// 最大登録待ち件数
	SOCK_STATE  GetSockState() const {return my_emSockState; };			// 現在のステータス
	CString		GetSockStateName()	const;								// 現在のステータス名称

	// Set
	void SetSession(const char *cSession ){ my_cSession = cSession; }	// 接続セッションセット 
	void SetPcName(const char *cName ){ memcpy(my_cPCname, cName, sizeof(my_cPCname)); }	// PC名称セット
	void SetSendNum(int sendnum){ my_nSendNum = sendnum; }				// 送信回数

protected :
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド
	
	void SendStart();													// 疵情報伝文送信 開始処理
	void SendEnd();														// 疵情報伝文送信 完了処理

	void QueBufSendBack();												// 現バッファを返却する
	void QueAllSendBack();												// すべてのキューを返却する

	// ソケット操作
	void Connect(bool bLogDsp);											// 接続
	void Close();														// 切断 & リトライ設定
	void SendHeader();													// ヘッダー部送信開始
	void SendData();													// データ部送信開始

	// 各インスタンス
	LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス
	SockBase*				mcls_pSock;									// ソケット操作クラスインスタンス


	// イベントハンドル
	HANDLE					my_evReTryConnect;							// 再接続要求タイマイベント
	HANDLE					my_evConnect;								// 接続要求イベント
	HANDLE					my_evDisConnect;							// 切断 イベント (手動でも切れるように公開もして置く)
	HANDLE					my_evSendTimeout;							// 送信タイムアウト イベント
	HANDLE					my_evSendEnd;								// 送信完了 イベント
	HANDLE					my_evAnserClose;							// 外部への切断通知イベント


	// ソケット & 送信データ関連
	SOCK_STATE				my_emSockState;								// ステータス (外部ガイダンス用にしか使わないかなー)
	SOCK_F_HEAD*			my_pHead;									// スレッドキュー受け取り && 送信データバッファ
	long					my_nSendingDataSize;						// 現在のデータ部のみの送信バイト数 (分割送信対応対策)
	int						my_nSendNum;								// フレーム単位での送信数 (論理的な数)


	// その他もろもろ
	char					my_cPCname[SIZE_NAME_PC];					// 送信元PC
//	CString					my_cPCname;
	CString					my_cSession;								// iniファイルのセッション名
	bool					my_bRetryMode;								// リトライの有無 (true:リトライする false:リトライしない)

};
