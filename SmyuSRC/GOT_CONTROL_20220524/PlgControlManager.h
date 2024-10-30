#pragma once

#include "resource.h"		// 画面コントロールにアクセス用
#include <winsock2.h>													// UDP送信用

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC操作クラス

#include "..\..\Include\KizuSmyu.h"										// GOTインクルード
#include "..\..\Include\ToTbl.h"										// 統括共有メモリ用ヘッダー
#include "..\..\Include\OutInterface.h"									// 外部IF用ヘッダー


class PlgControlManager :	public ThreadManager
{
//// メンバー構造体
public:

	//=======================================
	// 閾値 構造体
	//=======================================
	struct TYP_SETTING {										// 設定データ
		int		nSamplingEnable;										// 1:通常の疵検  0:PLGのみ

		int		nFrameSizeY;											// 縦フレーム数 [pixel]

		int		nChNum;													// 有効Ch数
		double	dResY[SMYU_PLG_MAX_CHANNEL];							// 縦分解能 [mm/pixel]
		double	dPlsY[SMYU_PLG_MAX_CHANNEL];							// パルスレート [mm/pls]
	};
	struct TYP_SAVEDATA {										// 保持データ
		int		nFrameCycle;											// フレーム出力周期 [ms]
		int		nSpeed[SMYU_PLG_MAX_CHANNEL];							// ライン速度 [mpm]
	};

//// メンバー定数
private :
	static const int	TIME_CNT_INTERVAL	= 1000;					// ライブカウンタ実行周期
	static const int	TIME_SPEED_INTERVAL = 100;					// 通板距離速度処理実行周期
	static const int	TIME_HELCY_INTERVAL	= 5000;					// ヘルシー送信周期


//// 公開関数
public:
	PlgControlManager(void);
	virtual ~PlgControlManager(void);

	void SmyuStart();													// シミュレータ開始


	//=========================================
	// 制御用
	TYP_SAVEDATA&		SetSaveData() const { return (TYP_SAVEDATA&)m_SaveData; }
	//void	SetTimer_FrameCycle(bool bMode);
	void	SetTimer_SpeedCycle(int nIndex, bool bMode);
	
	// 通板距離用
	double	GetLength(int nIndex)	const	{ return mtbl_pSmyu->dLen[nIndex]; }
	void	ResetLength(int nIndex)			{ mtbl_pSmyu->dLen[nIndex] = 0.0; }
	DWORD	GetFrameNo()	const
	{
		SEQ_DM_RECV_BASE* typWk = (SEQ_DM_RECV_BASE*)&mtbl_pSeq->fb_dm[0];
		return typWk->ken.nFrameNo;
	}


	//=========================================
	// 画面公開用
	TYP_SAVEDATA const&		GetSaveData() const { return (TYP_SAVEDATA const&) m_SaveData; }
	TYP_SETTING const&		GetSetting() const  { return (TYP_SETTING const&) m_Setting; }

	void	Set_WpdKen(void);											// 検査用WPD信号ON
	void	Set_WpdSub(void);											// 検査比較用WPD信号ON
	void	Set_WpdDsp(void);											// 表示用WPD信号ON
	void	Set_ResetSignal(void);										// リセット信号ON

	void	Clear_SeqMem(void);											// シーケンサ共有メモリクリア

//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	

	//=========================================
	// 共有メモリ操作
	void SetMem_Sampling();												// サンプリングクロック用
	void SetMem_Cnt();													// ライブカウンタ用
	void SetMem_Speed(int nIndex);										// 通板距離用

	void SetSeqMem_Clear(TO_SEQMEM_TBL* tbl);							// シーケンサ共有メモリクリア


	//=========================================
	// 小物
	void GetIniParam();													// Iniファイル取得
	void SendUpd();														// UDP送信
	void SendHelcy();													// ヘルシー送信
	

	//=========================================
	// UDP送信
	BOOL SetUdpSocket();												// UDP SOCKET生成
	void ReSetUdpSocket();												// UDP SOCKET破棄

//// メンバー変数
private:

	//=========================================
	// ハンドル
	HANDLE					m_evTimerFrame;								// フレーム出力定周期実行イベント (nFrameCycle が0以上の場合のみ)
	HANDLE					m_evTimerCnt;								// ライブカウンタイベント
	HANDLE					m_evTimerSpeed[SMYU_PLG_MAX_CHANNEL];		// その他通板距離定周期実行イベント
	HANDLE					m_evTimerHelcy;								// ヘルシー送信イベント


	//=========================================
	// 保持データ
	TYP_SETTING				m_Setting;									// 初期設定定数 (立ち上げ時のみ読み直し)
	TYP_SAVEDATA			m_SaveData;									// 保持データ


	//=========================================
	// 共有メモリ
	CString					m_sSmyuName;								// PLG情報 名称
	HANDLE					m_hTblSmyu;									// PLG情報 ハンドル
	SMYU_TBL_PLG*			mtbl_pSmyu;									// PLG情報 データ

	CString					m_sSeqName;									// シーケンサ情報 名称
	HANDLE					m_hTblSeq;									// シーケンサ情報 ハンドル
	TO_SEQMEM_TBL*			mtbl_pSeq;									// シーケンサ情報 データ
	int						m_nIdPlgKen;									// 検査用PLGインデックス
	int						m_nIdPlgSub;									// 検査比較用PLGインデックス
	int						m_nIdPlgDsp;									// 表示用PLGインデックス

	double					m_dKenPlgDiff;								// PLG差分
	double					m_dKenPlgAdd;								// PLG積算
	double					m_dKenExsync;								// ExSync積算値
	double					m_dDspPlgAdd;								// PLG積算

	//=========================================
	// UDP送信
	SOCKET					m_SeqUdpsock;								// UDP用 ソケット本体
	sockaddr_in				m_SeqUdpaddr;								// UDP用 sockaddr_in構造体
	CString					m_SeqUdpIp;									// UDP用 IPアドレス
	WORD					m_SeqUdpPort;								// UDP用 Port番号
};
