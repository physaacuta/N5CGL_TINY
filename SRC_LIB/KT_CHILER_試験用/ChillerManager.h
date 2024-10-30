#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\Kizu232C\Rs232cBase.h"							// RS-232C操作クラス

// ローカルのクラス
#include "MainConst.h"

using namespace KizuLib;
using namespace Kizu232C;
class ChillerManager : public ThreadManager
{

//// ローカル定数
private :
	// RS232C設定
	static const int PARAM_232C_SENDBUF_SIZE		= 256;				// 232Cの送信バッファサイズ
	static const int PARAM_232C_RECVBUF_SIZE		= 256;				// 232Cの受信バッファサイズ
	
	// タイマー
	static const int PARAM_TIMEOUT_RETRY			= 5000;				// リトライ周期
	static const int PARAM_TIMEOUT_ANSER			= 30000;			// 応答アンサータイマー



//// 公開定数
public:
	// チラー送信コマンド種別
	enum EM_CHILER_SEND_COMMAND {
		SEND_CHECK_NOW_TEMP = 1,										// [M1] 液温測定値
		SEND_CHECK_SET_TEMP,											// [S1] 液温設定値
		SEND_CHECK_MODE,												// [JO] 運転状態
		SEND_CHECK_ALM,													// [ER] 警報信号
		SEND_CHECK_NON
	};

	// チラー受信コマンド種別（送信と同じはず）　今後変わるかもしれないので一応準備
	enum EM_CHILER_RECV_COMMAND {
		RECV_CHECK_NOW_TEMP = 1,										// [M1] 液温測定値
		RECV_CHECK_SET_TEMP,											// [S1] 液温設定値
		RECV_CHECK_MODE,												// [JO] 運転状態
		RECV_CHECK_ALM,													// [ER] 警報信号
		RECV_CHECK_NON
	};

	// チラー装置通信状態
	enum EM_CHILER_MAINMODE
	{		
		MODE_NONE = 0,													// 初期状態
		MODE_WAIT,														// 待機	(ホントに何もしていない。or 送信許可待ち)
		MODE_RECV_EXEC,													// 受信コマンド待ち
		MODE_RETRY														// オープンリトライ中
	};

	// チラー装置運転モード
	enum EM_CHILER_UNTENMODE
	{		
		MODE_STOP = 0,													// 全停止中
		MODE_COOL,														// クーラー運転中
		MODE_POMP,														// ポンプ単独運転中
		MODE_NON
	};
	
	//=======================================
	// 構造体定義
	//=======================================
	// 受信管理情報
	struct INF_RECV {
		int				nSize;											// 全部の [byte]
		int				nNow;											// 受信完了したサイズ [byte]
		BYTE			cBuf[PARAM_232C_RECVBUF_SIZE];					// 受信バッファ
	};
	// 送信管理情報
	struct INF_SEND {
		EM_CHILER_SEND_COMMAND	emChilerCommand;						// 要求コマンド
		int				nSize;											// 全部の伝文長 [byte]
		BYTE			cBuf[PARAM_232C_SENDBUF_SIZE];					// 送信バッファ
	};


//// 公開関数
public:
	ChillerManager(char const* cName, int nTempThr, int nTimerCycle);
	virtual ~ChillerManager(void);

	// 送受信処理
	bool AddQue_SendCommand(EM_CHILER_SEND_COMMAND em);					// 送信コマンド


	// 共通処理
	CString GetMyName() { return my_sThreadName;}
	CString GetSendCommandName(EM_CHILER_SEND_COMMAND em);				// 送信コマンド名称
	CString GetRecvCommandName(EM_CHILER_RECV_COMMAND em);				// 受信コマンド名称
	EM_CHILER_SEND_COMMAND GetSendCommand(char* code);					// 送信コードの特定
	EM_CHILER_RECV_COMMAND GetRecvCommand(char* code);					// 受信コードの特定

	CString GetMainModeName(EM_CHILER_MAINMODE em);						// メインモード名称
	CString GetMainModeName() {return GetMainModeName(m_emMainMode);};	// メインモード名称(今の状態)
	CString GetUntenName(EM_CHILER_UNTENMODE em);						// チラー運転状態名称
	CString GetUntenName() {return GetUntenName(m_emUnten);};			// チラー運転状態名称(今の状態)

	// 画面用
	bool	IsOpen()			const {return m_bIsOpen;}
	bool	IsConnect()			const {return m_bIsConnect;}
	double	GetNowTemp()		const {return m_dNowTemp;}
	double	GetSetTemp()		const {return m_dSetTemp;}
	CString	GetAlmNo()			const {return m_sAlmNo;}


//// ローカル関数
private:
	int			ThreadFirst();											// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int			ThreadLast();											// スレッド終了前処理 (終了シグナル後にコールバック)
    void		ThreadEvent(int nEventNo);								// スレッドイベント発生
	void		ThreadWaitFor(int nDefWaitNum, int* nNowWaitNum);		// スレッドのWaitFor待ちの直前処理  


	// RS-232C関連
	bool RS_Open(bool bLogDsp);											// 232C オープン
	void RS_Close();													// 232C クローズ
	bool RS_Recv(int pos = 0, int size = 3, int total = 3);				// 232C 非同期 受信
	bool RS_Send();														// 232C 同期 送信

	// 送受信処理
	void DeleteAllQue();												// キューに溜まっている送信依頼を全て削除
	bool SendStart();													// 送信開始
	int  RecvEnd();														// 受信完了


//// ローカル変数
private:
	// 各インスタンス
	Rs232cBase*				mcls_p232c;									// 232c接続インスタンス

	// 232C関連
	INF_RECV				m_InfRecv;									// 受信管理情報
	INF_SEND				m_InfSend;									// 送信管理情報

	// 同期シグナル
	HANDLE					m_evRecv;									// 受信完了 シグナル
	HANDLE					m_tmReTry;									// リトライ シグナルタイマー
	HANDLE					m_tmCycleExec;								// 定周期チェック シグナルタイマー
	HANDLE					m_tmSendAnser;								// 送信アンサー待ちシグナルタイマー


	// スレッドキュー
	ThreadQueue<INF_SEND>	mque_SendItem;								// 送信依頼データ用キュー


	// 機器ID
	int						m_nIdConnect;								// 回線状態
	int						m_nIdState;									// 運転状態	
	int						m_nIdErr;									// 異常状態
	int						m_nIdTemp;									// 温度
	int						m_nIdTempSt;								// 温度状態


	// チラー装置 状態
	bool					m_bIsOpen;									// 232C回線がオープン時true
	bool					m_bIsConnect;								// チラー装置 接続状態(true:接続中(初めて受信でtrue), false:切断中)

	EM_CHILER_MAINMODE		m_emMainMode;								// 現在のメインモード
	double					m_dNowTemp;									// 液温測定値
	double					m_dSetTemp;									// 液温設定値
	EM_CHILER_UNTENMODE		m_emUnten;									// 運転状態
	CString					m_sAlmNo;									// 警報番号
	bool					m_bLimitTemp;								// チラー上限温度異常 false:異常
	bool					m_bAlmMachine;								// チラー装置異常認識 false:異常

	// その他もろもろ
	CString					m_cMySession;								// 自識別子 (機器状態 等々の切り替えに使用する)
	int						m_nTempThr;									// 温度閾値 [度]
	int						m_nTimerCycle;								// 温度取り込み周期 [秒]
	int						m_nChilerType;								// チラーの型式(0:RKE1500B-VW, 1:RKE3750B-V）※未設定時0

};
