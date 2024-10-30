// *********************************************************************************
//	DB書込み基本クラス
//	[Ver]
//		Ver.01    2007/03/14  vs2005 対応
//
//	[メモ]
//		複数インスタンスによる同時書込みに対応した作りとする
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
//#include "..\KizuLib\Class\AutoLock.h"									// クリティカルセクションクラス
#include "..\KizuLib\Class\KizuPerformanceCounter.h"					// 時間測定クラス

#include "..\KizuODBC\OdbcBase.h"										// DB操作クラス

using namespace KizuLib;
using namespace KizuODBC;

class DBSaveManager : public WorkThread  
{
public:
	static const int PARAM_DB_MAX_BAINARY		= 10;					// DB登録 バイナリデータ最大件数
	static const int PARAM_DB_RETRY_TIME		= 1000;					// DB再接続時間 [ms]

	//// 受け渡し構造体
	typedef struct DELIVERYSQL{
		// 登録データ
		CString strSql;													// SQL文 (0文字の場合、状況問い合わせにより、渡されたシグナルを叩く)
		bool	addmode;												// 最終時にも絶対登録時true
		bool	bCalcTime;												// 時間計測するSQL文時true
	
		bool	bIsLock;												// 順所保障で実行する場合true
		DWORD	nLockNo;												// 順番保障用(連番であること。)

		int		cnt;													// バイナイデータ件数 (最大10件) (0時、バイナリ登録無し=通常SQL文)
		DWORD	size[PARAM_DB_MAX_BAINARY];								// 画像サイズ
		BYTE*	pImg[PARAM_DB_MAX_BAINARY];								// 画像データ

		// 登録完了時にメールスロット送信用
		MAIL_ITEM*	SendMailItem;										// 送信アイテム一式
		
	public:
		DELIVERYSQL() {													// デフォルトコンストラクタ
			strSql		= "";
			addmode		= false;
			bCalcTime	= true;

			bIsLock		= false;
			nLockNo		= 0;

			cnt			= 0;
			memset(size, 0x00, sizeof(size));
			memset(pImg, 0x00, sizeof(pImg));
			SendMailItem= NULL;	
			
		};
	} DeliverySQL;


public:
	DBSaveManager(int myID, ThreadQueue<DeliverySQL>* que_pDeli);
	virtual ~DBSaveManager();


	void Start() { WorkThread::Start(MainThread); };					// スレッド開始
	//void Stop()  { WorkThread::Stop(10000); };							// スレッド終了
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ログファイルセット
	void SetSession(char const* pSession) { m_cSession = pSession; }	// DB接続セッション指定
	void SetReTry(bool bMode) { m_bReTry = m_bReTryExecOk = bMode; }	// DB異常時に即時再度挑戦するか (true:再挑戦)

	//// 外部アクセス用
	double GetAveTime() const { return mcls_Counter.GetAveTime(); }		// 1回の平均処理時間 [ms]
	double GetMaxTime()	const { return mcls_Counter.GetMaxTime(); }		// 最大処理時間 [ms]
	void InitTime()			  { mcls_Counter.InitAve(); }				// 初期化

	void SetSendDbManager(bool flg) { m_bSendDbManager = flg; }			// DB管理に送信するか (true:送信)
	void SetEvSynclock(HANDLE evSynclock) { m_evSynclock = evSynclock;}	// 登録状況問い合わせイベントをセット

	//// 外部シグナル
	void SetEvReSet()	{ SetEvent(m_evReSet);}							// DB接続リセット



private:
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド
	void Send_To_DbManage();											// DB削除管理に通知
	void sqlExecute(DeliverySQL * pDeli);								// SQL実行
	bool ConnectDB(bool bLogDsp);										// DB接続
	void CloseDB();														// DB切断


	// 受け渡しキュー
	ThreadQueue<DeliverySQL>*	mque_pDeli;								// SQL情報 キュー

	// 各インスタンス
	LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス
	KizuPerformanceCounter	mcls_Counter;								// 時間測定スレッド
	OdbcBase*				mcls_pDB;									// ODBC操作クラス
	
	// シグナル
	HANDLE					m_evReTryConnect;							// 再接続要求タイマイベント
	HANDLE					m_evSynclock;								// 登録状況問い合わせイベント (外部から渡される)
	HANDLE					m_evReSet;									// DB接続リセットイベント
	

	// その他もろもろ
	int						m_nMyID;									// 自分のID (0始まり)
	bool					m_bDBConnect;								// DB接続状態 (true:接続中)
	bool					m_bDBDelete;								// DB管理状態 (ture:削除優先)
	bool					m_bSendDbManager;							// DB管理に送信するか (true:送信)
	bool					m_bReTry;									// DB異常時に即時再度挑戦するか (true:再挑戦)
	bool					m_bReTryExecOk;								// 一回リトライしたら、再びリトライするのは、１回で正常になってからとする。(= SQL異常とかで、毎回再接続するのを止めたい)

	CString					m_cSession;									// DB接続セッション


	// 順序保障関連 (必ず CS を通して、変数にアクセス)
private:
	static CRITICAL_SECTION	m_csLock;									// 自クラスの他インスタンス間の排他制御
	static DWORD			m_nEndLockNo;								// 今処理完了した nLockNo
	static DWORD			m_nPushLockNo;								// APが取り出しが nLockNo

	DWORD	ReadEndLockNo() const { AutoLock  lock(&m_csLock); return m_nEndLockNo; }
public:
	static void SetLockSeqens(DELIVERYSQL* p) {							// 順序保障をセット
		AutoLock  lock(&m_csLock); 
		m_nPushLockNo ++;

		p->bIsLock = true;
		p->nLockNo = m_nPushLockNo;
	}
	static void ResetLockNo () {										// 検査開始時などに。しなくてもOK。(検査停止時はNG。まだ登録Queが残っているかもだし)
		AutoLock  lock(&m_csLock); 
		m_nEndLockNo = 0;
		m_nPushLockNo= 0;
	}

};
