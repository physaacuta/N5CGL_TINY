#pragma once
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー

#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// マクロ集
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// スタティックライブラリ集
#include "..\..\Library\KizuLibMFC\Class\DivNameManager.h"				// DIV定数 名称取得クラス
#include "..\..\Library\KizuLib\Class\AutoLock.h"						// クリティカルセクション制御クラス

// 疵検 インクルード
#include "..\..\include\ToHtCommon.h"									// 統括判定用の定義ヘッダ


// 各ワーカースレッド
#include "MainConst.h"													// 自クラス定数ヘッダー


// ファンクションの使い方だが、ThreadManagerを継承しておく。ワーカースレッドは起動しない
class StatusManager : public ThreadManager
{
//// 公開定数
public:
	// タスク ステータス
	enum MAIN_STATE
	{		
		STATE_NON = 0,			// 初期化中
		STATE_INIT,				// 初期状態 (資源開放状態)
		STATE_WAIT,				// 準備完了 (資源占有状態)
		STATE_RUN,				// 検査中   (オンライン)
		STATE_CANNOT			// 続行不可能
	};


//// 公開関数
public:
	StatusManager(void);
	virtual ~StatusManager(void);


	// タスクステータス制御
	bool			IsRunning()			const { return STATE_RUN == m_emMainState ? true : false;}	// 検査中:true
	MAIN_STATE		GetMainState()		const { return m_emMainState; }								// 現在のステータス取得
	CString			GetMainStateName()	const { return GetMainStateName(m_emMainState); };			// 現在のステータス名称取得
	CString			GetMainStateName(MAIN_STATE st) const;											// 指定のステータス名称取得
	bool			SetMainState(MAIN_STATE st);													// ステータス変更

	// 運転モード
	EM_DIV_UNTEN	GetUnten()			const { return m_emUnten; }											// 現在の運転モード取得
	CString			GetUntenName()		const { return GetUntenName(m_emUnten); };							// 現在の運転モード名称取得
	CString			GetUntenName(EM_DIV_UNTEN unten) const { return DivNameManager::GetDivUnten(unten); };	// 指定の運転モード名称取得
	bool			SetUnten(EM_DIV_UNTEN unten);													// 運転モード変更


	// カメラペア関係
	bool			GetParam_CamPairInf();											// パラメータ取得
	int				GetCamPairItemNum(int nCamPair, EM_DIV_HANTEI em) {
							if(em == DIV_HANTEI_HT)		return m_CamPairInf.item[nCamPair-1].num;
							else if(em == DIV_HANTEI_CY)return m_CamPairInf.item_cycle[nCamPair-1].num;
							else if(em == DIV_HANTEI_PR)return m_CamPairInf.item_para[0].num;
							else return 0;
	}
	int				GetCamPairItemCamset(int nCamPair, int index, EM_DIV_HANTEI em) {
							if(em == DIV_HANTEI_HT)		return m_CamPairInf.item[nCamPair-1].camset[index];
							else if(em == DIV_HANTEI_CY)return m_CamPairInf.item_cycle[nCamPair - 1].camset[index];
							else if(em == DIV_HANTEI_PR)return m_CamPairInf.item_para[0].camset[index];
							else return 0;
	}
	// パラランはカメラセット指定なので外部からセットする必要がある
	void			SetParaCamset(int nCamSet1, int nCamSet2) {
							m_CamPairInf.item_para[0].camset[0] = nCamSet1;
							m_CamPairInf.item_para[0].camset[1] = nCamSet2;
							m_CamPairInf.item_para[0].num		= 2;
	}

//// メンバー関数
private:
	// ワーカースレッドとして動作させないため、関係関数を非公開に変更
	int				ThreadFirst()				{return 0;}				// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int				ThreadLast()				{return 0;}				// スレッド終了前処理 (終了シグナル後にコールバック)
    void			ThreadEvent(int nEventNo)	{;}						// スレッドイベント発生


//// メンバー変数
private:

	//// 同期制御
	CRITICAL_SECTION		m_csLock;									// クリティカルセクション

	//// ステータス情報
	MAIN_STATE				m_emMainState;								// タスクステータス
	EM_DIV_UNTEN			m_emUnten;									// 運転モード

	CAMPAIR_INF				m_CamPairInf;								// 全カメラペア定義 (各判定機能が担当するカメラセット群) 




//// 各種状態保持
public :
	bool IsEngStatNew() const { return m_bGetEngStatNew; }
	void SetEngStat(ENG_STAT const*  pTyp) { AutoLock lock(&m_csLock); m_bGetEngStatNew=true; memcpy( &m_EngStat, pTyp, sizeof(m_EngStat)); }
	void GetEngStat(ENG_STAT*  pTyp) { AutoLock lock(&m_csLock); m_bGetEngStatNew=false; memcpy( pTyp, &m_EngStat, sizeof(m_EngStat)); }


private:
	bool					m_bGetEngStatNew;
	ENG_STAT				m_EngStat;									// エンジン状態 (最新)


};
