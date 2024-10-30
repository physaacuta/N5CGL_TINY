// *********************************************************************************
//	疵検PC間 TCP/IP受信基本クラス (クライアント機能のみ)
//	[Ver]
//		Ver.01    2007/03/19  vs2005 対応
//
//	[メモ]
//		主に、疵検PC間の受信に使用する (KizuOpSendの受けに使用)
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

class KizuDefaultRecv : public WorkThread  
{
public:
	static const int PARAM_SOCK_RETRY_TIME			= 5000;				// ソケット再接続時間 [ms]
	static const int PARAM_SOCK_RECV_BUFSIZE		= (4*1024*1024);	// LANボードの受信バッファサイズ (4M)
	static const int PARAM_TIMEOUT_HELCY_RECV		= (5*60*1000);		// ヘルシー伝文受信タイマアウト [ms] (5分)


	// 本クラスのソケット状態ステータス
	enum SOCK_STATE { 
		STATE_NONE = 0,													// 初期状態 (切断中)
		STATE_IDLE,														// 準備完了 (接続中)
		STATE_HEAD,														// ヘッダ部 受信中
		STATE_DATA														// データ部 受信中
	};

public:
	KizuDefaultRecv(const char *cSession, ThreadQueue<SOCK_BASE_HEAD>* que_pRecv);
	virtual ~KizuDefaultRecv(void);

	void Start() { WorkThread::Start(MainThread); };					// スレッド開始
	void Stop()  { WorkThread::Stop(10000); };							// スレッド終了
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ログファイルセット

	//// 外部アクセス
	// Get
	SOCK_STATE  GetSockState()		const {return my_emSockState; };					// 現在のステータス
	CString		GetSockStateName()	const {return GetSockStateName(my_emSockState);};	// 現在のステータス名称
	CString		GetSockStateName(SOCK_STATE st)	const;									// 指定のステータス名称

protected:
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド

	// ソケット操作
	void Connect(bool bLogDsp);											// 接続
	void Close();														// 切断 & リトライ設定
	bool Recv_Head();													// ヘッダ部受信開始
	bool Recv_Data();													// データ部受信開始
	void Recv_Helcy();													// ヘルシー伝文受信完了 (ヘッダー部のみ)

	// 受け渡しキュー
	ThreadQueue<SOCK_BASE_HEAD>* mque_pRecv;							// 受信データ構造体受け渡し キュー

	// 各インスタンス
	LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス
	SockBase*				mcls_pSock;									// ソケット操作クラスインスタンス
	
	// ソケット
	SOCK_STATE				my_emSockState;								// ステータス (外部ガイダンス用にしか使わないかなー)
	SOCK_BASE_HEAD		 	my_RecvHead;								// 受信ヘッダ部のバッファ (実際は、SOCK_F_HEADとなるが、ヘッダー部のみの伝文を受信出来る様にする為、SOCK_BASE_HEADとする事)
	BYTE *					my_pRecvData;								// 受信データ部のバッファ
	long					my_nRecvingDataSize;						// 現在のデータ部のみの受信バイト数 (分割送信対応対策) (ID)

	// シグナル
	HANDLE					my_evReTryConnect;							// 再接続要求タイマイベント
	HANDLE					my_evHelcy;									// ヘルシー送信イベント
	HANDLE					my_evDisConnect;							// 切断イベント
	HANDLE					my_evRecvEnd;								// 受信完了イベント

	// その他もろもろ
	CString					m_cSession;									// iniファイルのセッション名
	
};
