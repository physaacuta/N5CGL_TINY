// *********************************************************************************
//	統括→表示・操作PCへの送信クラス (サーバー機能のみ)
//	[Ver]
//		Ver.01    2007/03/20  vs2005 対応
//
//	[メモ]
//		主に、統括→操作PC・表示PCへの送信に使用
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


class KizuOpSend : public WorkThread  
{
protected :
	// ローカル定数
	static const int PARAM_SOCK_MAX				= 6;					// 最大接続人数
	static const int PARAM_SOCK_TIMEOUT			= 60000; //20161031 //10000; //5000;					// ソケット タイムアウト
	static const int PARAM_SOCK_SEND_BUFSIZE	= (4*1024*1024);		// LANボードの送信バッファサイズ (4M)
	static const int PARAM_TIME_HELCY_SEND		= (2*60*1000);			// ヘルシー伝文送信タイマー周期 [ms] (2分)

public:
	KizuOpSend(const char *cSession);
	virtual ~KizuOpSend();

	void Start() { WorkThread::Start(MainThread); };					// スレッド開始
	void Stop()  { WorkThread::Stop(10000); };							// スレッド終了
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ログファイルセット
	void SetPostTID(DWORD tid) { this->my_PostTid=tid;}					// PostThreadMessageの送り先 (メインインスタンス)
	
	// 受け渡しキュー
	ThreadQueue<SOCK_BASE_HEAD> gque_Deli;								// 送信データ受け渡しキュー

	//// 外部アクセス
	BOOL IsConnect(int id) const {return my_bIsConnect[id]; }			// 接続状態
	int	 GetConnectCount() const { if(NULL!=mcls_pSock) {return mcls_pSock->GetActive();} else {return 0;} }	// 接続件数取得
	
	HANDLE	GetEvConnect() const {return my_evConnect; };				// 外部への接続イベント
	HANDLE	GetEvClose()   const {return my_evClose; };					// 外部への切断イベント

protected :
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド
	void SendStart();													// 疵情報伝文送信 開始処理
	void SendEnd();														// 疵情報伝文送信 完了処理
	void SendHelcy();													// ヘルシー伝文送信 処理
	void QueBufFree();													// 現バッファを開放する
	void QueAllFree();													// すべてのキュー開放

	// ソケット操作
	BOOL Listen(bool bLogDsp);											// Listen
	void Close(int id);													// 切断処理
	BOOL Send();														// 疵情報伝文送信

	// 各インスタンス
	LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス
	SockBase*				mcls_pSock;									// ソケット操作クラスインスタンス

	// 送信用
	BOOL					my_bIsConnect[PARAM_SOCK_MAX];				// 接続状態保持(TRUE:接続中, FALSE:切断中)
	BOOL					my_bIsNowSend;								// 現在送信中識別(TRUE:送信中, FALSE:送信中でない)
	SOCK_BASE_HEAD*			my_pHead;									// スレッドキュー受け取り && 送信データバッファ
	int						my_nNowSendCount;							// 送信回数
	BOOL					my_bIsAllSend;								// 全てのClientへ送信完了?(TRUE:完, FALSE:未)
	long					my_nSendingDataSize;						// 現在のデータ部のみの送信バイト数 (分割送信対応対策)

	// イベントハンドル
	HANDLE					my_evConnect;								// 接続イベント
	HANDLE					my_evClose;									// 切断イベント

	// その他もろもろ
	CString					my_cSession;								// iniファイルのセッション名
};