#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\Kizu232C\Rs232cBase.h"							// RS-232C操作クラス
#include "..\..\Library\KizuLibMFC\CLASS\DivNameManager.h"

// 各ワーカースレッド
#include "MainConst.h"

using namespace KizuLib;
using namespace Kizu232C;

class TempManager :	public ThreadManager
{
//// 固定定数
private:
	//------------------------------------------
	// 固定定数
	//------------------------------------------
	// RS232C設定
	static const int PARAM_232C_SENDBUF_SIZE		= 256;				// 232Cの送信バッファサイズ
	static const int PARAM_232C_RECVBUF_SIZE		= 256;				// 232Cの受信バッファサイズ

	// タイマー
	static const int PARAM_TIMEOUT_RETRY			= 5000;				// 再接続要求
	static const int PARAM_TIMEOUT_ANSER			= 30000;			// 応答伝文受信タイムアウト



//// 公開関数
public:
	TempManager(char const* cName);
	virtual ~TempManager(void);


	//=========================================
	// 外部アクセス
	CString GetMyName() { return my_sThreadName;}
	int		GetUnitNum() { return m_nUnitNum; }

	CString	GetMainModeName(EM_TEMP_MAINMODE em);							// メインモード名称
	CString	GetMainModeName() { return GetMainModeName(m_emMainMode); };	// 今の状態
	CString GetUnitName(int id) { return m_cUnitName[id];};					// ユニット名称

	CString GetConnect() { return DivNameManager::GetDivBool(m_bIsInital);}
	CString GetUnten(int id) { return DivNameManager::GetDivBool(m_emUnten[id]);}
	int		GetStatus(int id){ return m_nStatus[id];}
	double	GetTemp(int id){ return m_dTemp[id];}
	bool	GetTempSt(int id){ return m_bTempSt[id];}


//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	

	// 温度パネルメータ制御
	bool CheckNextSend();												// 次送信制御
	bool CheckDataRecv();												// 受信伝文チェック
	
	bool Check_Echo_Back(RES_TYPE_ECHO_BACK* buf);
	bool Check_Controler_Status(RES_TYPE_CONTROLER_STATUS* buf);
	bool Check_Monitor_Read(RES_TYPE_MONITOR_READ* buf);

	CString GetResCodeName(char* cResCode);								// レスポンスコード名称 取得
	CString GetEXTName(char* cEXT);										// 終了コード名称 取得


	// RS-232C関連
	bool	Open(bool bLogDsp);											// 232C オープン
	void	Close();													// 232C クローズ
	bool	Recv();														// 232C 非同期 受信
	bool	Send();														// 232C 同期 送信

	// BCC関連
	void	CreateBCC(char* cSendBuf, int nBufSize);					// BCCクリエイト
	bool	CheckBCC(char* cRecvBuf, int nBufSize);						// BCCチェック



//// メンバー変数
private:
	// 各インスタンス
	Rs232cBase*				mcls_p232c;									// 232c接続インスタンス


	// ハンドル
	HANDLE					m_evRecvEnd;								// 受信完了イベント認識用
	HANDLE					m_evSendEnd;								// 送信完了イベント認識用
	HANDLE					m_evReTry;									// 再接続要求 イベント
	HANDLE					m_evAnserTimeOut;							// 応答伝文待ちタイムアウト イベント
	HANDLE					m_evCycleExec;								// 定周期確認イベント


	// 232C関連
	char					m_cRecvBuf[PARAM_232C_RECVBUF_SIZE];		// 受信バッファ
	char					m_cSendBuf[PARAM_232C_SENDBUF_SIZE];		// 送信バッファ

	// ステータス関連
	EM_TEMP_MAINMODE		m_emMainMode;								// 現在のメインモード
	int						m_nSubMode;									// 現在のサブモード
	int						m_nUnitMode;								// 現在のユニット台数繰り返しモード

	// 温度パネルメータ 状態
	EM_DIV_BOOL				m_bIsConnect;								// 温度パネルメータ 接続状態 (true:接続中, false:切断中)
	EM_DIV_BOOL				m_bIsInital;								// 温度パネルメータ 初期化状態 (true:オープンチェック完了でTrue)

	EM_DIV_BOOL				m_emUnten[MAX_TEMP_UNIT];					// 運転状態 (true:正常運転中 false:停止中 or 不明)
	int						m_nStatus[MAX_TEMP_UNIT];					// ステータス (0x00:正常 0x01:未計測状態 0x02:表示範囲オーバー 0x04:入力異常A 0x08:入力異常B)
	double					m_dTemp[MAX_TEMP_UNIT];						// 温度
	bool					m_bTempSt[MAX_TEMP_UNIT];					// 温度状態

	// 設定値
	CString					m_cUnitName[MAX_TEMP_UNIT];					// 名称。ログで使用
	int						m_nTempLimit[MAX_TEMP_UNIT];				// 温度閾値 [度] (0未チェック。)この値以上で温度異常にする

	int						m_nIdConnect;								// 接続ID 			(0：機器状態に登録しない)
	int						m_nIdTemp[MAX_TEMP_UNIT];					// 温度ID				〃
	int						m_nIdTempSts[MAX_TEMP_UNIT];				// 温度状態ID			〃
	int						m_nIdUnten[MAX_TEMP_UNIT];					// 運転状態ID			〃
	int						m_nIdState[MAX_TEMP_UNIT];					// ステータス状態ID		〃


	// その他もろもろ
	CString					m_cMySession;								// 自識別子 (機器状態 等々の切り替えに使用する)
	int						m_nUnitNum;									// 管理対象ユニット数
	int						m_nCycleTime;								// 周期 [msec]
	int						m_nTempKeta;								// 取得温度の桁合わせ (1で1/10。2で1/100。0はそのまま。マイナスは未対応)
	
};
