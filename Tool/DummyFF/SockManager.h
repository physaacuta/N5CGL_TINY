// *********************************************************************************
//	通信制御管理スレッド
//	[Ver]
//		Ver.01    2012/08/30
//
//	[メモ]
//		
// *********************************************************************************

#pragma once


// 半固定
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Include\LineCommon.h"									// 疵検インクルード

#include "..\..\Library\DefectLib\BaseTcpIp.h"							// TCP/IP操作クラス

// メンバー
#include "MainConst.h"
#include "SockDataExec.h"												// 送受信データ受け渡しクラス

using namespace KizuLib;

class SockManager : public ThreadManager
{
private :

	// バッファID定義
	static const int BUF_RECV				= 0;						// 受信用バッファ
	static const int BUF_SEND				= 1;						// 送信用バッファ

	// ポートID定義
	static const int PORT_RECV				= 0;						// 受信ポートID 
	static const int PORT_SEND				= 1;						// 送信ポートID 

	// 通信関係定義
	static const int PARAM_SOCK_MAX			= 2;						// ソケット数
	static const int PARAM_SOCK_BUFSIZE		= 65000;					// 最大バッファサイズ (65000)
	//static const int PARAM_SOCKANSER_TIMEOUT= 5*60*1000;				// 応答伝文 タイムアウト(T1)
	static const int PARAM_SOCK_TIMEOUT		= 30*1000;					// ソケット タイムアウト(T2)
	//static const int PARAM_HELCYSEND_TIMER	= 15*60*1000;				// ヘルシー伝文 定周期送信 タイム(T3)
	//static const int PARAM_JUKOSYO_TIMER	= 10;						// 重故障 タイム[分]

	//// 受信結果定義
	//static const int RECV_RET_ERR_NONE		= -9;						// かなり異常(切断処理必要)
	//static const int RECV_RET_ERR_IDLE		= -1;						// 異常(切断はしない)
	//static const int RECV_RET_OK			= 0;						// 1シーケンスすべて完了
	//static const int RECV_RET_NEXT			= 1;						// 次送信処理を実施すること
	//static const int RECV_RET_NONEXT		= 9;						// 次受信処理を実施すること(=何もしない)

public :
	//// クラス内の状態
	enum SOCK_STAT {
		SOCK_NONE = 0,		// 切断中
		SOCK_IDLE,			// 要求待ち
		SOCK_EXEC			// シーケンス実行中
	};

//// 公開関数
public:
	SockManager(void);
	virtual ~SockManager(void);
	
	//// 外部アクセス
	bool	IsConnect() const				{return m_bIsConnect;}
	void    SetDumpMode(bool bmode)         { mcls_pData->SetDumpMode(bmode); }					// ダンプ出力モードセット
	bool    GetDumpMode()                   {return mcls_pData->GetDumpMode(); }				// ダンプ出力モード取得

	CString GetSockStatName() const			{return GetSockStatName(m_eStateMain); }			// ソケットステータス名称
	CString GetSendStatName() const			{return GetSendStatName(m_ExecItem.emStat); }		// 送信
	CString GetSockStatName(SOCK_STAT st) const;						// 現在のステータス名称
	CString GetSendStatName(QUE_SEND_STAT st) const;					// 現在の送信ステータス名称
	void SetSockDataExec(SockDataExec* pCls) {mcls_pData = pCls;}			// 送受信データ受け渡しクラス


//// 公開変数
public:
	// 受け渡しキュー
	ThreadQueue<Delivery>	gque_Deli;									// 送信依頼用キュー 


//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	void ThreadWaitFor(int nDefWaitNum, int* nNowWaitNum);				// スレッドのWaitFor待ちの直前処理  
	
	void ExecAnsQue(int id, BaseTcpIp::DELIVERY_ANS* que);				// TCP/IPクラス 操作結果の処理
	void QueDelete();													// キューを削除する



	// TPC/IP 通信制御
	void SockInit();													// 初期化処理
	void SockExit();													// 終了処理
	void UserClose();													// 強制切断
	bool RecvStart(int id, int size, BYTE* pBuf, int point=0);			// 非同期 受信処理開始

	// 送受信シーケンス
	bool SendLockStart(Delivery* deli);									// 業務伝文 送信 (同期)
	bool SendExec();													// 業務伝文送信 共通処理
	bool RecvEnd(int id, BaseTcpIp::DELIVERY_ANS* que);					// 受信 完了処理 (非同期)

	// 固有処理
	int SetRecvData( BYTE const* data, char* cTc ); // 受信データ処理


//// メンバー変数
private:
	//// 各インスタンス
	BaseTcpIp*				mcls_pSock[PARAM_SOCK_MAX];					// 受信ポート[0] 送信ポート[1] 用操作クラス
	SockDataExec*			mcls_pData;									// 送受信データ受け渡しクラス
	
	//// ソケット関連
	BYTE					m_cSockBuf[PARAM_SOCK_MAX][2][PARAM_SOCK_BUFSIZE];	// 送受信用バッファ [0:受信ポート 1:送信ポート] [0:受信バッファ 1:送信バッファ]
	ExecItem				m_ExecItem;									// 送信中のデータ


	//// ステータス
	bool					m_bIsConnect;								// 両回線状態
	bool					m_bIsFfConnectErr;							// FFサーバー準備中時 true
	SOCK_STAT				m_eStateMain;								// メインステータス

	int						m_nSt;										// セットするステータス
};
