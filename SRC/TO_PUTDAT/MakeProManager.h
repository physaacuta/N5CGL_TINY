#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"						// ワーカースレッドマネージャー

#include "..\..\Library\KizuLibMFC\Class\CreateSQL.h"							// SQL文生成補助クラス
#include "..\..\Library\KizuLibMFC\Class\DivNameManager.h"						// DIV定数 名称取得クラス

// 共通インクルード
#include "..\..\Include\OutInterface.h"											// 外部インターフェース用ヘッダー

// 各ワーカースレッド
#include "MainConst.h"															// 自クラス定数ヘッダー

class MakeProManager : public ThreadManager
{
//// 公開構造体
public:
	//=============================
	// 本クラス受け渡し用クラス
	//=============================
	//// メール情報受け渡しキュー
	struct DELIVERY_KEY
	{
		char				cKey[SIZE_SERIAL_FF_NO];							// 主キー
	};

//// メンバー構造体	
private:
	//// 混入率算出用マップ
	struct MIX_RATE_MAP {
		struct GP {
			// 混入率算出用
			int		nMixRateCnt;					// 1m単位の混入件数 (同一セル上にある場合は、無視)
			bool	bFlg[COIL_LEN_MAX];				// 1m単位での疵存在マップ (false:まだ無し true:既に有り)

													// 疵種グループ別の件数
			int		nCnt[MAX_PROGRP_TYPE];			// 件数
		} gp[MAX_PROGRP_GRADE];						// プロコングループ(グレード)
	};

//// 公開関数
public:
	MakeProManager(void);
	virtual ~MakeProManager(void);

	//// 外部アクセス
	void SetEvDefectDbStop()			{ SetEvent(m_evDefectDbStop); }			// 強制キャンセル
	void SetEvDefectDbAddOk(int nDbAns)							// 統括管理 実績書き込み完了
	{
		//AutoLock lock(&m_csLock);

		m_nDefectDbAddAnser = nDbAns;
		SetEvent(m_evDefectDbAdd);
	}

	void SetHosei(double val)													// 補正率をセット
	{
		AutoLock lock(&m_csLock);

		m_dHosei = val;
		LOG(em_INF), "[%s] 補正率変更 %f", my_sThreadName, m_dHosei);
	}
	
//// 公開メンバー変数 
public:
	ThreadQueue<DELIVERY_KEY>	gque_Deli;										// 受け渡しキュー

//// メンバー関数
private:
	int ThreadFirst();															// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();															// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);												// スレッドイベント発生

	bool Exec(char const* cID);													// プロコン検査実績 伝文生成
	void ClearMember();															// 処理中変数 初期化

//------------>>> Execからの一連の処理 (この中の関数だけ privateの処理中変数が使用可能)
	bool GetBufferCut(char const* cID);											// プロコンカット実績受信バッファテーブル取得
	bool CheckEqalCut(char const* cID);											// プロコンカット実績受信バッファテーブル取得
	bool GetCoilInf_KizukenNo();												// コイル情報から管理Noを取得
	bool CheckDefectInsertOk();													// 統括管理がDB書き込み完了した？

	void MakeSendData(int nZiErr, PRO_TYPE_ZISEKI* pTypZi);						// プロコン送信用 実績伝文生成
	bool CheckState_Kensa();													// 対象範囲 の検査状態は正常かチェック
	bool ExecDefectInf(PRO_TYPE_ZISEKI* pTypZi);								// 伝文へ検査実績データセット

	bool InsertZiBuf(const char *cID, const PRO_TYPE_ZISEKI *pTypZi);				// プロコン用検査実績伝文 を バッファテーブルに登録
	bool UpdateProconBuf(const char *cID);										// カット実績テーブル を 更新
// <<<-----------

	bool Send_To_Procon(const char *cKey);										// プロコン用検査実績送信要求
	void Send_To_Defect(const PRO_TYPE_DATA_CUT *pData);						// 統括管理 に実績書き込み状況問い合わせ送信要求

//// メンバー変数
private:
	// シグナル
	HANDLE						m_evDefectDbStop;								// 統括管理 書き込み強制終了シグナル
	HANDLE						m_evDefectDbAdd;								// 統括管理 書き込み完了シグナル
	
	// 処理中 (処理中のみデータが有効)
	OdbcBase					*mcls_pDB;										// DB接続クラス
	char						m_cKizukenNo[SIZE_KIZUKEN_NO];					// 対象の管理No
	PRO_TYPE_CUT				m_typBuf;										// 受信データ (カット実績)

	// その他もろもろ
	CRITICAL_SECTION			m_csLock;										// クリティカルセクション
	bool						m_bCreateOk;									// 処理結果
	double						m_dHosei;										// 補正率 [%]
	int							m_nDefectDbAddAnser;							// 統括管理 書き込み完了結果 (0:正常 1:異常)

	// iniファイル値
	int							m_nMaxWaitTime;									// コイル実績書き込み問い合わせ応答待ち時間 [msec]
	int							m_nChkCoilDate;									// コイル検索の対象期間

};
