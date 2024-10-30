// *********************************************************************************
//	プロコン通信 スレッド
//	[Ver]
//		Ver.01    2007/08/03  vs2005 対応
//
//	[メモ]
//		
// *********************************************************************************
#pragma once


// 半固定
#include "..\..\Include\LineCommon.h"									// 検査インクルード
#include "..\..\Library\KizuLib\KizuLib.h"								// 検査ライブラリ
#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// マクロ集
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// スタティックライブラリ集
#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ログ管理クラス

// 検査クラス インクルード
#include "..\..\library\kizulibmfc\class\threadmanager.h"				// ワーカースレッドマネージャー
//#include "..\..\Library\KizuLib\Class\WorkThread.h"						// ワーカースレッドクラス
#include "..\..\Library\DefectLib\BaseTcpIp.h"							// TCP/IP操作クラス

// メンバー
#include "MainConst.h"
//#include "SockDataExec.h"												// 送受信データ受け渡しクラス

using namespace KizuLib;

class SockManager :	public ThreadManager
{
private :

	// バッファID定義
	static const int BUF_RECV				= 0;						// 受信用バッファ
	static const int BUF_SEND				= 1;						// 送信用バッファ

	// ポートID定義
	static const int PORT_RECV				= 0;						// 受信ポートID 
	static const int PORT_SEND				= 1;						// 送信ポートID 

	// 回線定義
	static const int ID_RECV				= 0;						// 受信回線用
	static const int ID_SEND				= 1;						// 送信回線用 


	// 通信関係定義
	static const int PARAM_SOCK_MAX			= 2;						// ソケット数
	static const int PARAM_SOCK_BUFSIZE		= 2*1024;					// 最大バッファサイズ (2K)
	static const int PARAM_SOCK_TIMEOUT		= 30*1000;					// ソケット タイムアウト[ms]
	static const int PARAM_SOCK_RETRY_TIMER = 10*1000;					// 再コネクト時間 (再アクセプト間隔時間)[ms]
#ifdef LINE_1CGL
	static const int PARAM_SOCKANSER_TIMEOUT= 90*1000;					// 応答伝文 タイムアウト (1CGLだけは、プロコンCPUが低いためタイムアウト時間を延ばす)
#else
	static const int PARAM_SOCKANSER_TIMEOUT= 30*1000;					// 応答伝文 タイムアウト[ms]
#endif
	static const int PARAM_RESPANSER_TIMEOUT= 5*1000;					// レスポンス タイムアウト[ms]
	static const int PARAM_HELCY_TIMEOUT	= 90*1000;					// ヘルシー伝文受信 タイムアウト[ms]

public :
	//// クラス内の状態
	enum SOCK_STAT {
		SOCK_NONE = 0,		// 切断中
		SOCK_IDLE,			// 要求待ち
		SOCK_EXEC			// シーケンス実行中

//		SOCK_EXEC_RECV,		// 受信シーケンス実行中
//		SOCK_EXEC_SEND		// 送信シーケンス実行中
	};
	//// 受信中状態
	enum SOCK_NOW_RECV {
		SOCK_NR_RES = 0,	// レス部受信中
		SOCK_NR_HEAD,		// ヘッダー部受信中
		SOCK_NR_DATA		// データ部受信中

	};



public:
	SockManager(void);
	virtual ~SockManager(void);

	void Start() { WorkThread::Start(MainThread);  };					// スレッド開始
	void Stop()  { WorkThread::Stop(10000); };							// スレッド終了 (シグナルイベントでの終了)
	//=========================================
	// セットクラス
	//void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ログファイルセット
	void SetQueDeli(ThreadQueue<Delivery>* pQue) { mque_pDeli = pQue; }	// 受け渡しキューセット


	// 受け渡しキュー
	ThreadQueue<Delivery>*	mque_pDeli;									// 送信依頼用キュー

	
	//// 外部アクセス
	CString GetStateName(int id) const		{return mcls_pSock[id]->GetSockStateName(); }		// ソケットステータス名称
	bool	IsConnect(int id)	 const		{ return m_eStateMain[id] == SOCK_NONE ? false : true; }	// 回線接続状態
	void	UserClose(bool bMode=true);															// 強制切断
	CString GetSockStatName(int id) const { return GetSockStatName(m_eStateMain[id]); }			// ソケットステータス名称
	CString GetSockStatName(SOCK_STAT st) const;												// 現在のステータス名称


	// Dump出力関係
	void SetDumpMode(bool bmode) { m_bDumpMode = bmode; }				// ダンプ出力モードセット
	bool GetDumpMode() { return m_bDumpMode; }							// ダンプ出力モード取得

protected:
	//=========================================
	// 仮想関数
	virtual bool SetSendDataAnser(Delivery* deli, bool bAns) = 0;		// 送信データ 事後処理
	virtual bool SetRecvData(int tc, int size, BYTE * data) = 0;		// 受信データ 事後処理

//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
	void ThreadEvent(int nEventNo);										// スレッドイベント発生
	void ThreadWaitFor(int nDefWaitNum, int* nNowWaitNum);				// スレッドのWaitFor待ちの直前処理

	void ExecAnsQue(int id, BaseTcpIp::DELIVERY_ANS* que);				// TCP/IPクラス 操作結果の処理


	// TCP/IP 通信制御
	void SockInit();													// 初期化処理
	void SockExit();													// 終了処理
	bool RecvStart(int id, int size, BYTE* pBuf, int point=0);			// 非同期 受信処理開始
//	bool SendStart(int id, int size, BYTE* pBuf, int point=0);			// 非同期 送信処理開始
	bool SendLock_Respons(int id, BYTE mode);							// レスポンス伝文の送信 (同期)

	// 送受信シーケンス
	bool SendLockStart();												// 業務伝文 送信 (同期)
	bool RecvEnd(int id, BaseTcpIp::DELIVERY_ANS* que);					// 受信 完了処理 (非同期)


	//// 各インスタンス
	//LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス
	BaseTcpIp*				mcls_pSock[PARAM_SOCK_MAX];					// 受信ポート[0] 送信ポート[1] 用操作クラス
	//SockDataExec*			mcls_pData;									// 送受信データ受け渡しクラス

	// シグナル
	HANDLE					m_evAnserTimeOut;							// 応答伝文タイムアウトシグナル
	HANDLE					m_evHelcyTimeOut;							// ヘルシー伝文受信タイムアウトシグナル

	//// ソケット関連
	BYTE					m_cSockBuf[PARAM_SOCK_MAX][2][PARAM_SOCK_BUFSIZE];	// 送受信用バッファ [0:受信ポート 1:送信ポート] [0:受信バッファ 1:送信バッファ]

	//// ステータス
	bool					m_bIsConnect;								// 両回線状態
	SOCK_STAT				m_eStateMain[PARAM_SOCK_MAX];				// メインステータス
	SOCK_NOW_RECV			m_eStateNowRecv[PARAM_SOCK_MAX];			// 受信伝文のどの部分を受信しているかフラグ (0:レス部 1:ヘッダー部 2:データ部)
	long					m_nNowEventNo[PARAM_SOCK_MAX];				// 現在 シーケンス処理中のイベントNo
	//	int						m_nStateSub;								// サブステータス


//// メンバー関数
protected:
	//// ステータス
	void SetHelcy();													// ヘルシー伝文 受信タイムアウト設定

//// メンバー変数
protected:
	//// ステータス
	bool                    m_bDumpMode;								// ダンプ出力モード(True:全件出力,False:1件出力)

};
