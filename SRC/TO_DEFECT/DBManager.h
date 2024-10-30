//============================================================
//	DB管理クラス
//------------------------------------------------------------
// [メモ]		2015/07/15		初版作成開始
//		
//============================================================
#pragma once

// 半固定
#include "..\..\Include\LineCommon.h"												// φωインクルード

// φωクラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"							// ワーカースレッドマネージャー

// ローカル
#include "MainConst.h"
#include "DbBaseManager.h"
#include "BcpSaveManager.h"
#include "ParamManager.h"


class DBManager :	public DbBaseManager
{
public:
	DBManager(void);
	virtual ~DBManager(void);

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 受渡し構造体の番号
public:
	// (ENUM_DB_KUBUN::em_END からの連番とする
	enum ENUM_KUBUN {
		em_COILINF = em_DB_KUBUN_END,												// コイル情報
		em_COILRESULT,																// コイル実績
		em_COILRESULT_UPDATE,														// コイル実績アップデート
		em_COILRESULT_UPDATE_CUT,													// コイル実績アップデート （実績確定後のUpDate用）(シャーカット)

		em_COILPOSITION,															// コイル位置
		em_COILTRAND,																// コイルトレンド
		em_COILCUTPOS,																// コイルカット位置

		em_DEFECTINF,																// 疵情報

		em_SEQ_COIL_BUF,															// コイル情報伝文バッファ保存

		em_EDGE_COIL_INF,															// エッジ検出実績テーブル
		em_EDGE_CHANGE_INF,															// エッジ検出モード変化管理テーブル

#ifndef DISABLE_TINY_CALC
		em_TINY_RESULT,																// 微小欠陥実績
		em_TINY_INF,																// 微小欠陥情報
#endif

		em_FRAME_INF,																// フレーム情報テーブル
		em_FRAME_IMG,																// フレーム画像＋フレーム情報アップデート分

		em_CAMLINKSW,																// カメラリンクスイッチ設定(←MainInstance)

		em_PULSE_RATE_UPDATE,														// シーケンサよりパルスレート変更

		em_DUMMY_INF,																// ダミー疵(1CAPLのみ。ストップマーク疵)	

		em_DB_ADD_SYNC,																// 登録状況問い合わせ

		em_DB_COUNT_RESET															// 疵画像登録数リセット
	};



/////////////////////////////////////////////////////////////////////////////////////////////////////////
// BCP用構造体
private:
	//// 疵画像保存用テーブル
	struct TYP_BCP_DEFECT_IMG
	{
		char					cKizukenNo[SIZE_KIZUKEN_NO];						// 管理No
		int						nKizuNo;											// 疵No	
		BCPBase::typBindDataEx	img[MAX_HT1_CAMANGLE];								// 画像データ 正/乱
	};


	//// 特徴量 (基本、角度 同一構造体)
	struct TYP_BCP_DEFECT_ATTR
	{
		char					cKizukenNo[SIZE_KIZUKEN_NO];						// 管理No
		int						nKizuNo;											// 疵No
		double					attr[MAX_ATTR_BASE];								// 特徴量
	};


	//// コイル位置情報
	struct TYP_BCP_COIL_POSITION
	{
		DBDATA_COIL_POSITION	CoilPos;											// コイル位置情報
	};



/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 公開関数
public:
	BCPSaveManager&		GetBcpSaveMgr(void) {return mcls_Bcps;};				// BCP書込みクラス取得


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 仮想関数 (継承元からコールバック)
private:
	virtual void OnAlloc(bool bMode=true);											// 関数初期化
	virtual void OnFree();															// 関数開放
	virtual void OnQueDb(int emNo, BYTE* pData);									// DB登録依頼
	virtual void OnBcpAutoCommit(bool bAuto) {};									// BCP自動コミット用
	virtual void OnBcpStatusCheck() {};												// BCP状態チェック(定周期タイマーイベント)
	virtual void ThreadFirst_AddHandle() {};										// シグナル待ち行列の追加
	virtual void OnAddSignal(int nEventNo) {};										// シグナルを追加した奴のイベント (追加したものを0始まりとする)
	virtual void OnReSetMaxCount();													// リセット処理追加
	
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// SQL生成関連
private:
	void CreateSQL_CoilInf(const DBDATA_COIL_INF* pWk);								// コイル情報
	void CreateSQL_CoilResult(const DBDATA_COIL_RESULT* pWk);						// コイル実績
	void CreateSQL_CoilResultUpdate(const DBDATA_COIL_RESULT_UPDATE* pWk);			// コイル実績アップデート （実績確定後のUpDate用）
	void CreateSQL_CameraResult(const DBDATA_COIL_RESULT* pWk);						// カメラ実績

	void CreateSQL_CoilPosition(const DBDATA_COIL_POSITION* pWk);					// コイル位置
	void CreateSQL_CoilTrand(const DBDATA_COIL_TRAND* pWk);							// コイルトレンド
	void CreateSQL_CoilCutPos(const DBDATA_COIL_CUT_POS* pWk);						// コイルカット位置

	BOOL CreateSQL_DefectInf(const DBDATA_DEFECT_INF* pWk);							// 疵情報


	void CreateSQL_EdgeCoilInf(const DBDATA_EDGE_COIL_INF* pWk);					// エッジ検出実績
	void CreateSQL_EdgeChangeInf(const DBDATA_EDGE_CHANGE_INF* pWk);				// エッジ検出モード変化管理

	void CreateSQL_CamLinkSW(const DBDATA_CAMERA_LINKSWITCH* pWk);					// カメラリンクスイッチ設定

	void CreateSQL_PulseRateUpdate(const DBDATA_PULSE_RATE_UPDATE* pWk);			// シーケンサからパルスレート変更

	void Add_Synclock(const DBDATA_ADD_SYNC* pWk);									// 登録状況問い合わせ

	void Reset_ImgCount(const DBDATA_COUNT_RESET* pWk);								// 疵画像登録数リセット

	void CreateSQL_StatusLog(const DBDATA_STATUS_LOG* pWk);							// ステータスログ




/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// メンバー変数
private:

	//// 各インスタンス
	BCPSaveManager			mcls_Bcps;												// BCP書き込みクラス

};
