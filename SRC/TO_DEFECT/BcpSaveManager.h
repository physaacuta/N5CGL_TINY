//============================================================
//	BCP書き込みクラス
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


class BCPSaveManager : public DbBaseManager
{
/// 公開定数
public:
	BCPSaveManager(void);
	~BCPSaveManager(void);
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 受渡し構造体の番号
public:
	// 
	// (ENUM_DB_KUBUN::em_END からの連番とする
	enum ENUM_KUBUN {
		em_DEFECTINF,																// 疵情報
		em_BCP_BATCH,																// BCP batch強制実行
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
// ローカル定数
private:
	static const int		MAX_BCP_BATCH_COUNT_DEF		= 50;						// この数単位で、疵画像のBCP書込みを行う
	static const int		MAX_BCP_BATCH_COUNT_ATTR	= 100;						// この数単位で、特徴量のBCP書込みを行う
	static const int		MAX_BCP_INS					= NUM_MEN;					// 各BCPのインスタンス数。
	static const int		MAX_BCP_INS_ATTR			= 1+MAX_IMAGE_CNT;			// 基本＋4角度
	
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//公開関数
public:
	double GetAttrSendRowAveTime() const { return mcls_AttrSendRowTime.GetAveTime(); }		// AttrSendRow1回の平均処理時間 [ms]
	double GetAttrSendRowMaxTime()	const { return mcls_AttrSendRowTime.GetMaxTime(); }		// AttrSendRow最大処理時間 [ms]	
	double GetAttrBcpBatchAveTime() const { return mcls_AttrBcpBatchTime.GetAveTime(); }	// AttrBcpBatch1回の平均処理時間 [ms]
	double GetAttrBcpBatchMaxTime()	const { return mcls_AttrBcpBatchTime.GetMaxTime(); }	// AttrBcpBatch最大処理時間 [ms]	
	double GetImgSendRowAveTime() const { return mcls_ImgSendRowTime.GetAveTime(); }		// ImgSendRow1回の平均処理時間 [ms]
	double GetImgSendRowMaxTime()	const { return mcls_ImgSendRowTime.GetMaxTime(); }		// ImgSendRow最大処理時間 [ms]	
	double GetImgBcpBatchAveTime() const { return mcls_ImgBcpBatchTime.GetAveTime(); }		// ImgBcpBatch1回の平均処理時間 [ms]
	double GetImgBcpBatchMaxTime()	const { return mcls_ImgBcpBatchTime.GetMaxTime(); }		// ImgBcpBatch最大処理時間 [ms]	

	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 仮想関数 (継承元からコールバック)
private:
	virtual void OnAlloc(bool bMode=true);											// 関数初期化
	virtual void OnFree();															// 関数開放
	virtual void OnQueDb(int emNo, BYTE* pData);									// DB登録依頼
	virtual void OnBcpAutoCommit(bool bAuto);										// BCP自動コミット用
	virtual void OnBcpStatusCheck();												// BCP状態チェック(定周期タイマーイベント)
	virtual void ThreadFirst_AddHandle() {};										// シグナル待ち行列の追加
	virtual void OnAddSignal(int nEventNo) {};										// シグナルを追加した奴のイベント (追加したものを0始まりとする)
	virtual void OnReSetMaxCount();													// リセット処理追加
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// BCP関連
private:
	void BCP_Bind_Defect_Img();														// BCPクラス テーブル紐付け (疵画像)
	void BCP_Bind_Defect_Attr();													// BCPクラス テーブル紐付け (特徴量)
	//////void BCP_Bind_Coil_Position();													// BCPクラス テーブル紐付け (コイル位置情報)
	void CheckBcpStatus(bool bMode);												// BCP状態を変更

	
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// SQL生成関連
private:
	void CreateSQL_DefectInf(const DBDATA_DEFECT_INF* pWk);							// 疵情報
	void BcpBatchExec(const DBDATA_BCP_BATCH_EXEC* pWk);							// BCP batch 強制書き込み


/// メンバー変数
private:

	//===================================
	// BCP領域
	//// 疵画像
	BCPBase*				mcls_pBcpDefImg[MAX_BCP_INS];							// DB高速書込みクラス
	TYP_BCP_DEFECT_IMG*		m_pBcpDefImgBuf[MAX_BCP_INS];							// BCP書込み用 固定メモリバッファ
	int						m_nBcpDefSendRowCnt[MAX_BCP_INS];						// 表裏別のSendRow件数(Batchしたら0クリア)

	// 特徴量
	BCPBase*				mcls_pBcpAttr[MAX_BCP_INS][MAX_BCP_INS_ATTR];			// DB高速書込みクラス
	TYP_BCP_DEFECT_ATTR*	m_pBcpAttrBuf[MAX_BCP_INS][MAX_BCP_INS_ATTR];			// BCP書込み用 固定メモリバッファ
	int						m_nBcpAttrSendRowCnt[MAX_BCP_INS];						// 表裏別のSendRow件数(Batchしたら0クリア)

	//////// コイル位置情報
	//////BCPBase*					mcls_pBcpCoilPos[EM_DIV_SYSTEM::DIV_SYSTEM_END];	// DB高速書込みクラス[0:中央φω, 1:出側φω, 2:穴検]
	//////TYP_BCP_COIL_POSITION*		m_pBcpCoilPosBuf[EM_DIV_SYSTEM::DIV_SYSTEM_END];	// BCP書込み用 固定メモリバッファ
	//////int							m_nBcpCoilPosSendRowCnt;							// SendRow件数(Batchしたら0クリア)	

	// 時間計測用クラス
	KizuPerformanceCounter	mcls_AttrSendRowTime;									// AttrSendRow時間測定スレッド
	KizuPerformanceCounter	mcls_AttrBcpBatchTime;									// AttrBcpBatch時間測定スレッド
	KizuPerformanceCounter	mcls_ImgSendRowTime;									// ImgSendRow時間測定スレッド
	KizuPerformanceCounter	mcls_ImgBcpBatchTime;									// ImgBcpBatch時間測定スレッド

	// Jpeg画像生成クラス
	KizuLjtJpegManager		mcls_KizuJpegManager;
};

