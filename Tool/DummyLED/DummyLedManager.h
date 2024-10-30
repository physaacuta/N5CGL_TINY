#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\DefectLib\BaseTcpIp.h"							// TCP/IP操作クラス

// ローカルのクラス
#include "MainConst.h"

using namespace KizuLib;

class DummyLedManager :	public ThreadManager
{

//// ローカル定数
private :
	//=======================================
	// 定数定義
	//=======================================
	// バッファID定義
	static const int BUF_RECV				= 0;						// 受信用バッファ
	static const int BUF_SEND				= 1;						// 送信用バッファ

	// ポートID定義
	static const int PORT_SEND				= 0;						// 送信ポートID 
	//static const int PORT_RECV			= 1;						// 受信ポートID 

	// 回線定義
	static const int ID_SEND				= 0;						// 送信回線用
	//static const int ID_RECV				= 1;						// 受信回線用
	
	// 通信関係定義
	static const int PARAM_SOCK_MAX			= 1;						// ソケット数
	static const int PARAM_SOCK_BUFSIZE		= 2*1024;					// 最大バッファサイズ
	static const int PARAM_RES_TIMEOUT		= 5000;						// 送信完了タイムアウト
	static const int PARAM_COM_BUFSIZE		= 5;						// 共通部伝文サイズ


	// タイマー
	static const int PARAM_SOCK_TIMEOUT		= 5000;						// 再コネクト間隔（ソケット タイムアウト）
	static const int PARAM_ANSER_TIMEOUT	= 15000; //5000;			// 応答伝文タイムアウト
	static const int PARAM_RECHECK_TIMEOUT	= PARAM_SOCK_TIMEOUT+PARAM_ANSER_TIMEOUT+1000;	// 再チェックタイムアウト ※この時間を越えて初めて異常にする
	static const int PARAM_SENDOK_TIMEOUT	= PARAM_ANSER_TIMEOUT+1000;	// 送信許可待ちタイマー
	static const int PARAM_SENDOK_NOWAIT_TIMEOUT = 1000;				// 送信許可待ちタイマー (即時チェック用)
	static const int PARAM_ADDTIME			= 1;						// 点灯累積時間 1分固定!! (変更不可) 

	// その他もろもろ
	static const int SYSLOG_BASE			= 220;						// syslog基底値 (ここから順に加算する)


//// 公開定数
public:
	// クラス内の状態
	enum SOCK_STAT {
		SOCK_NONE = 0,		// 切断中
		SOCK_IDLE,			// 要求待ち
		SOCK_EXEC			// シーケンス実行中
	};
	// LED送信コマンド種別
	enum EM_LED_RECV_COMMAND {
		RECV_CHECK_BREAK = 1,											// [Ra] 断線有無 (ステータス要求)
		RECV_CHECK_HEATING,												// [Rb] LED過熱 (ステータス要求)
		RECV_CHECK_ONOFF,												// [Rl] 点灯消灯 (ステータス要求)
		RECV_CHECK_TEMP,												// [Rt] LED温度 (ステータス要求)
		RECV_EXEC_ON,													// [RX] 点灯指示
		RECV_EXEC_OFF,													// [RY] 消灯指示
		RECV_SEND_MASTER,												// [RT] マスター調光のみ送信 (RAMにセット)
		RECV_SEND_VALUE,												// [RS] 調光データ送信 (RAMにセット)
		RECV_WRITE_EEPROM,												// [RW] RAMの調光データをEEPROM(ファイル)に書込
		RECV_READ_EEPROM,												// [RM] EEPROM(ファイル)の調光データを読込
		RECV_READ_RAM,													// [RR] RAMの調光データを読込
		RECV_GET_BLOCK,													// [RB] ブロック数取得
		RECV_NOTHING,													// [0x00]
		RECV_NON														// 不明
	};
private:
	// LED受信コマンド種別
	enum EM_LED_SEND_COMMAND {
		SEND_NEXT_SEND_OK = 101,										// [AC] 送信許可
		SEND_STATE_ON,													// [Ax] 点灯
		SEND_STATE_OFF,													// [Ay] 消灯
		SEND_STATE_BREAK,												// [AA] 断線
		SEND_STATE_HOT,													// [AB] 温度上昇
		SEND_STATE_SHATDOWN,											// [AD] 加熱ｼｬｯﾄﾀﾞｳﾝ
		SEND_STATE_OK,													// [AN] 正常
		SEND_READ_EEPROM,												// [AE] EEPROM 調光データ 読込
		SEND_READ_RAM,													// [AP] RAM 調光データ 読込
		SEND_LED_TEMP,													// [At] LED温度データ 読込
		SEND_GET_BLOCK,													// [Ab] ブロック数 読込
		SEND_TIME_OUT,													// [AT] 通信タイムアウト
		SEND_NOTHING,													// [0x80] なんか内部で勝手に出力されるらしい意味不明のコード。(メーカーいわく、無視して下さいとのこと)
		SEND_NON														// 不明
	};

	// 照明装置通信状態
	enum EM_LED_MAINMODE
	{		
		MODE_NONE = 0,													// 初期状態
		MODE_WAIT,														// 待機	(ホントに何もしていない。or 送信許可待ち)
		MODE_RECV_EXEC													// 受信コマンド待ち
	};


private:
	//=======================================
	// 構造体定義
	//=======================================
	// 受信管理情報
	struct INF_RECV {
		int				nSize;											// 全部の [byte]
		int				nCh;											// 対象チャンネル (1オリジン)
		//int				nNow;										// 受信完了したサイズ [byte]
		BYTE			cBuf[PARAM_SOCK_BUFSIZE];						// 受信バッファ
	};

	// 送信管理情報
	struct INF_SEND {
		EM_LED_SEND_COMMAND	emLedCommand;								// 要求コマンド
		bool			bSendOkUmu;										// 対応する受信コマンドの有無 (true:有り)
		int				nSize;											// 全部の伝文長 [byte]
		int				nCh;											// 対象チャンネル (1オリジン)
		BYTE			cBuf[PARAM_SOCK_BUFSIZE];						// 送信バッファ
	};

//// 公開関数
public:
	DummyLedManager(char const* cName, int nId, int nBlock, int nTempUnit);
	virtual ~DummyLedManager(void);

	// 送受信処理
	bool AddQue_SendCommand(EM_LED_SEND_COMMAND em, int nCh);				// 1バイトだけの送信コマンド専用

	// 共通処理
	CString GetMyName()			{ return my_sThreadName;}
	int		GetID()				{ return m_nMyID;}
	int		GetMen()			{ return m_nMyMen;}
	int		GetChNum()			{ return m_nChNum;}
	int		GetBlock(int ch)		{ return m_nBlock[ch];}
	int		GetMyIndex(int ch)		{ return m_nMyIndex[ch];}
	int		GetTempUnit(int ch)		{ return m_nTempUnit[ch];}
	void	SetTemp(int ch, int uni, int temp)			{ m_nTemp[ch][uni] = temp;}
	int		SetLedTempSts(int nCh, int nUnit)			{
															m_nTempSts[nCh][nUnit]++;
															m_nTempSts[nCh][nUnit] = (2 < m_nTempSts[nCh][nUnit] ? 0 : m_nTempSts[nCh][nUnit]);
															return m_nTempSts[nCh][nUnit];
														}
	void	SetBreak(int ch)							{
															m_nStateBreak[ch]= (1==m_nStateBreak[ch]?0:1);
															if(m_nStateBreak[ch]== 1) AddQue_SendCommand(SEND_STATE_SHATDOWN,ch);
														}
	int		SetStateTemp(int nCh)						{
															m_nStateTemp[nCh]++;
															m_nStateTemp[nCh] = (2 < m_nStateTemp[nCh] ? 0 : m_nStateTemp[nCh]);
															return m_nStateTemp[nCh];
														}
	void	SetLedOnOff(int ch, int nOnOff)				{ m_nLedOnOff[ch] = nOnOff; }

	CString GetSendCommandName(EM_LED_SEND_COMMAND em);					// 送信コマンド名称
	CString GetRecvCommandName(EM_LED_RECV_COMMAND em);					// 受信コマンド名称
	DummyLedManager::EM_LED_RECV_COMMAND		GetRecvCommand(char code);		// 受信コードの特定
	DummyLedManager::EM_LED_SEND_COMMAND		GetSendCommand(char code);		// 送信コードの特定
	CString GetMainModeName(EM_LED_MAINMODE em);						// メインモード名称
	CString GetMainModeName() {return GetMainModeName(m_emMainMode);};	// 今の状態
	CString GetSendStatName(int id) const	{return mcls_pSock[id]->GetSockStateName(); }		// 送信

	// 画面用
	bool	IsConnect()						const {return m_bIsConnect;}
	int		GetNowScode()					const {return m_nNowScode;}
	BYTE	GetNowLedBase(int nCh)			const {return m_nNowLedBase[nCh];}
	BYTE	GetNowLedHosei(int nCh, int nBlock)	const {return m_nNowLedHosei[nCh][nBlock];}

	int		GetStateOnOff(int nCh)			const {return m_nStateOnOff[nCh];}
	int		GetStateBreak(int nCh)			const {return m_nStateBreak[nCh];}
	int		GetStateTemp(int nCh)			const {return m_nStateTemp[nCh];}
	int		GetLedTemp(int nCh, int nUnit)	const {return m_nTemp[nCh][nUnit];}
	int		GetLedTempSts(int nCh, int nUnit)	const {return m_nTempSts[nCh][nUnit];}
	int		GetLedOnOff(int nCh)			const { return m_nLedOnOff[nCh]; }

//// メンバー関数
private:
	int			ThreadFirst();											// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int			ThreadLast();											// スレッド終了前処理 (終了シグナル後にコールバック)
    void		ThreadEvent(int nEventNo);								// スレッドイベント発生
	void		ThreadWaitFor(int nDefWaitNum, int* nNowWaitNum);		// スレッドのWaitFor待ちの直前処理  

	// 送受信処理
	void DeleteAllQue();												// キューに溜まっている送信依頼を全て削除
	bool SendStart(int* retc);											// 送信開始
	int  RecvEnd(int id, BaseTcpIp::DELIVERY_ANS* que);					// 受信完了

	// TCP/IP 通信制御
	void SockInit();													// 初期化処理 (一度のみ起動)
	void SockExit();													// 終了処理 (一度のみ起動)
	void UserClose(int nClose = 0);										// 強制切断
	bool RecvStart(int id, int size, BYTE* pBuf, int point=0);			// 非同期 受信処理開始
	void ExecAnsQue(int id, BaseTcpIp::DELIVERY_ANS* que);				// TCP/IPクラス 操作結果の処理

	EM_LED_SEND_COMMAND CheckBreak(int nCh);
	EM_LED_SEND_COMMAND CheckOnOff(int nCh);
	EM_LED_SEND_COMMAND CheckHeating(int nCh, int nUnit);
	EM_LED_SEND_COMMAND CheckHeating(int nCh);

	void GetMasterData(int nCh);
	

//// メンバー変数
private:
	// 各インスタンス
	BaseTcpIp*				mcls_pSock[PARAM_SOCK_MAX];					// 受信ポート[0] 送信ポート[1] 用操作クラス
	INF_RECV				m_InfRecv;									// 受信管理情報
	INF_SEND				m_InfSend;									// 送信管理情報

	// スレッドキュー
	ThreadQueue<INF_SEND>	mque_SendItem;								// 送信依頼データ用キュー

	// 照明装置 状態
	bool					m_bIsConnect;								// 照明装置 接続状態(true:接続中(初めて受信でtrue), false:切断中)
	EM_LED_MAINMODE			m_emMainMode;								// 現在のメインモード

	int						m_nNowScode;								// 現在の検査表面状態No (1オリジン)
	BYTE					m_nNowLedBase[MAX_LED_CH];					// 現在の調光データ
	BYTE					m_nNowLedHosei[MAX_LED_CH][MAX_LED_BLOCK];	// 現在の補正データ
	int						m_nStateOnOff[MAX_LED_CH];					// 点灯消灯 (-1:不明 0:点灯 1:消灯(正常) 2:消灯(異常))
	int						m_nStateBreak[MAX_LED_CH];					// 断線有無 (-1:不明 0:正常 1:断線)
	int						m_nStateTemp[MAX_LED_CH];					// LED加熱  (-1:不明 0:正常 1:加熱 2:シャットダウン)
	int						m_nTemp[MAX_LED_CH][MAX_TEMP_UNIT];			// LED温度
	int						m_nTempSts[MAX_LED_CH][MAX_TEMP_UNIT];		// LED温度状態 (-1:不明 0:正常 1:加熱 2:シャットダウン)
	int						m_nLedOnOff[MAX_LED_CH];					// 点灯消灯制御 (0:要求に従う 1:消灯固定)


	// ソケット関連
	BYTE					m_cSockBuf[PARAM_SOCK_MAX][2][PARAM_SOCK_BUFSIZE];	// 送受信用バッファ [0:受信ポート 1:送信ポート] [0:受信バッファ 1:送信バッファ]

	SOCK_STAT				m_eStateMain[PARAM_SOCK_MAX];				// メインステータス

	// その他もろもろ
	CString					m_cYourSession;								// 自識別子 (機器状態 等々の切り替えに使用する)
	CString					m_cMySession;								// 自識別子 (機器状態 等々の切り替えに使用する)
	int						m_nMyID;									// ID (0オリジン) (セクションの何番目)
	int						m_nMyMen;									// 表:0 裏:1
	int						m_nChNum;									// 管理対象チャンネル数

	bool					m_bUsedCh[MAX_LED_CH];						// CHの使用/未使用
	int						m_nBlock[MAX_LED_CH];						// 配光ブロック数
	int						m_nTempUnit[MAX_LED_CH];					// LED温度計測点数
	int						m_nMyIndex[MAX_LED_CH];						// CHが対応するIndex
};
