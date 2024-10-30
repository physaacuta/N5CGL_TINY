// シーケンサ通信 UDP受信専用クラス (1対1) クライアント

#pragma once

// 疵検クラス
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー

// 半固定
#include "..\..\Include\LineCommon.h"									// 疵検インクルード

// ローカルのクラス
#include "MainConst.h"
#include "StatusManager.h"

using namespace KizuLib;

class SeqFrameRecv : public ThreadManager
{
// ローカル定数
protected :
	//static const int PARAM_TIMEOUT_HELCY_RECV		= (10*1000);		// ヘルシー監視タイマアウト [ms]
	static const int PARAM_TIMEOUT_HELCY_RECV		= (20*1000);		// ヘルシー監視タイマアウト [ms]
	static const int PARAM_BUFSIZE					= 256;				// バッファサイズ


public:
	SeqFrameRecv();
	virtual ~SeqFrameRecv();

	//=========================================
	// プロパティ
	void	SetLogSeqMgr(LogFileManager* pCls)	{ mcls_pLogSeq = pCls;}	// シーケンサ受信用ログクラス
	void	SetStatusMgr(StatusManager* pCls)	{ mcls_pStatus = pCls;};// ステータス制御クラスセット


	// 操作
	void	SetRun( bool bMode) {m_bRun = bMode;}						// 検査開始、停止
	bool	GetRun()			{ return m_bRun; }
	bool	GetSt()				{ return m_bSt; }
	void	SetDebugWpd()		{ m_bDebugWpd = true;}


	//=========================================
	// イベント公開
	ThreadQueue<SEQ_TYPE_DATA_FRAME>*	GetQueFrame()	{ return &mque_Frame;}	// フレームデータ受渡用キュー

	// 参照用
	SEQ_TYPE_DATA_FRAME&	GetLastFrame() { return m_typLastFrame; }	


//// メンバー関数
private :
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生

	bool RecvCheck();													// 受信データのチェック
	bool AddNewFrame();													// 新規フレーム情報受信
	void OnErr(bool bManyu);											// 異常通知
	void QueAllDelete();												// すべてのキューを削除

	// TPC/IP 通信制御
	bool Recving();														// 業務伝文の受信を開始


//// メンバー変数
private :

	//=========================================
	// 各インスタンス
	LogFileManager*			mcls_pLogSeq;								// シーケンサ用ログクラス
	StatusManager*			mcls_pStatus;								// ステータス制御クラス

	// スレッドキュー
	ThreadQueue<SEQ_TYPE_DATA_FRAME>	mque_Frame;						// OUT フレームデータ受渡用キュー


	// イベントハンドル
	HANDLE					my_tmHelcy;									// ヘルシー監視タイマイベント


	// ソケット & 送信データ関連
	SOCKET					m_nSock;									// UDP受信ソケット
	OVERLAPPED				m_ovlRecv;									// 非同期受信オーバーラップ	
	BYTE					my_RecvBuf[PARAM_BUFSIZE];					// 受信データ
	bool					my_bHelcyOk;								// 何かしら伝文受信したので今回のヘルシーチェックはヤメ
	struct sockaddr_in		my_fromAddr;								// 受信元アドレス
	int						my_fromLen;									// 受信元アドレス長

	// その他もろもろ
	bool					m_bSt;										// ヘルシー正常時 true
	bool					m_bRun;										// 検査中 true
	SEQ_TYPE_DATA_FRAME		m_typLastFrame;								// 受信した最後のフレーム情報
	bool					m_bDebugWpd;								// デバック。強制WPD認識
};



