//=====================================================================
// 操業状態 管理クラス
//	【全ライン】
//		・稼動中、停止中、低速操業の判断を行う。
//=====================================================================

#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLibMFC\Class\DivNameManager.h"				// DIV定数 名称取得クラス
#include "..\..\Library\KizuLib\Class\AutoLock.h"						// クリティカルセクション制御クラス

// 共通ヘッダー
#include "..\..\Include\LineCommon.h"									// 疵検インクルード

// ローカルのクラス
#include "StatusManager.h"												// ステータス制御クラス


using namespace KizuLib;

class SogyoManager : public ThreadManager
{
//// メンバー構造体
private:

	//=======================================
	// 操業条件 閾値 構造体
	//=======================================
	struct SOGYO_SETTING {
		bool	bUmu;													// この条件が有効時 True

		int		nLimitSpeed;											// 閾値 ライン速度 [mpm]
		int		nCheckCnt;												// 閾値 何回続いたら条件に一致するか

		int		nNowCnt;												// 今何回目
	};


//// 公開関数
public:
	SogyoManager(LPCSTR cSession);
	virtual ~SogyoManager(void);


	//=========================================
	// プロパティ
	void SetStatusMgr(StatusManager* pCls) { mcls_pStatus = pCls;};		// ステータス制御クラスセット


	//=========================================
	// 外部操作関数

	// 定周期チェックタイマー 
	void CheckStart(DWORD sampCnt);										// 定周期チェックタイマー開始
	void CheckStop();													// 定周期チェックタイマー停止

	// 外部アクセス
	void SetNowSampCnt(DWORD sampCnt, double dResV);					// 現在の検査用サンプリングクロック値 セット [←PlgManager]
#ifdef DSP_SOGYO_CHECK
	void SetNowSampCntDsp(DWORD sampCnt, double dResV);					// 現在の表示用サンプリングクロック値 セット [←PlgManager]
#endif


	//=========================================
	// デバック
	void GetIniParam();													// 動的 操業判定条件を読み込み [←Dlg]

	// 画面
	int GetSpeed() const	{ return m_nMpm; }
#ifdef DSP_SOGYO_CHECK
	int GetSpeedDsp() const { return m_nMpmDsp; }
#endif



//// メンバー関数
private :
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生


	void CheckSampCntDiff();											// 通板中 定周期チェック
#ifdef DSP_SOGYO_CHECK
	void CheckSampCntDiffDsp();											// 通販中 定周期チェック表示用
#endif


//// メンバー変数
private :


	//// 各インスタンス
	StatusManager*			mcls_pStatus;								// ステータス制御クラス


	//// 同期シグナル
	HANDLE					m_evCheckTimer;								// 定周期タイマーシグナル
	CRITICAL_SECTION		m_csLock;									// クリティカルセクション


	//// サンプリングカウント関連
	/// 検査用
	double					m_dResV;									// 縦分解能 [mm/pixel]
	__int64					m_pf;										// PC周波数

	__int64					m_pcNow, m_pcPre;							// 現在の測定点, 前回の測定点
	DWORD					m_nNowSampCnt, m_nPreSampCnt;				// 現在のクロック値, 前回のクロック値

	int						m_nMpm;										// 現在のライン速度 [mpm]

#ifdef DSP_SOGYO_CHECK
	/// 表示用
	double					m_dResVDsp;									// 縦分解能 [mm/pixel]

	__int64					m_pcNowDsp, m_pcPreDsp;						// 現在の測定点, 前回の測定点
	DWORD					m_nNowSampCntDsp, m_nPreSampCntDsp;			// 現在のクロック値, 前回のクロック値

	int						m_nMpmDsp;									// 現在のライン速度 [mpm]
#endif
	//// 閾値
	union {
		SOGYO_SETTING		array[4];
		struct{
			SOGYO_SETTING	InfUpToDown_Slow;							// 稼動→停止方向 低速操業
			SOGYO_SETTING	InfUpToDown_Stop;							// 稼動→停止方向 ライン停止
			SOGYO_SETTING	InfDownToUp_Slow;							// 停止→稼動方向 低速操業
			SOGYO_SETTING	InfDownToUp_Run;							// 停止→稼動方向 ライン稼動
		};
#ifndef DSP_SOGYO_CHECK
	}						SogyoSetting;
#else
	}						SogyoSetting, SogyoSettingDsp;
#endif

	//// その他もろもろ
	CString					m_cSession;									// iniファイルセッション
};
