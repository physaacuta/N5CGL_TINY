#pragma once

// 疵検共通ヘッダー
#include "..\..\Include\LineCommon.h"									// ラインインクルード
#include "..\..\Include\HyTbl.h"										// 表示用共有メモリヘッダー

#include "..\..\Library\KizuLibMFC\Class\DivNameManager.h"				// DIV定数 名称取得クラス

// ローカル
#include "MainConst.h"
#include "DataSetBaseManager.h"											// 共有メモリデータセットクラス
#include "FileDeleteManager.h"											// ファイル削除管理クラス
#include "FileSaveManager.h"											// ファイル保存管理クラス
#include "FolderFunc.h"


//#define	LOG_DEBUG													// デバッグログ

#define LST_SAVE_NO_MAX		100											// 重欠陥画像保存MAX

class DataSetManager : public DataSetBaseManager
{
// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
	//// 公開変数
public:
	//// 構造体
	struct TYP_DATASET_REQMAE								// 前工程疵情報編集要求キュー
	{
		int nRec;											// 対象レコード(0～)
	};
// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------

// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	struct TYP_DATASET_REQLONGPD							// 長周期欠陥発生情報編集要求キュー
	{
		char	cKanriNo[SIZE_KIZUKEN_NO];					// 管理No
	};
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

//// 公開関数
public:
	DataSetManager(void);
	virtual ~DataSetManager(void);

	void Allock();														// スレッド準備


	//=========================================
	// デバック関係 (仮想関数)
	//virtual void OnOutputTbl();										// 全共有メモリ バイナリ出力
	//virtual void OnInputTbl();										// 全共有メモリ バイナリ読込

	//=========================================
	// 画面関係
	TBL_HY_POINT const* GetPointTbl() { return mtbl_pPoint; };				
	TBL_HY_COILINF const* GetCoilInfTbl() { return mtbl_pCoilInf; };
	TBL_HY_COILRESULT const* GetCoilResultTbl() { return mtbl_pCoilResult; };
	TBL_HY_DEFECTINF  const* GetDefectInfTbl() { return mtbl_pDefectInf; };
	TBL_HY_MAPROW* GetMapRowTbl() { return mtbl_pMapRow; };
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	TBL_HY_LONGPD const* GetCoilLongPdTbl() { return mtbl_pLongPd; };
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

	//=========================================
	// 前工程関係
	TBL_HY_MAE_INF*	GetMaeInf() { return mtbl_pMaeInf; }	// 前工程疵情報ポインタ取得
	char const*	GetMaeImgPath() { return m_cPath_Mae; }		// 前工程画像パス取得
	int GetnMaeImgSaveOff() { return m_nMaeImgSaveOff; }	// 前工程画像保存有無を取得（1:保存しない、以外：保存する）

// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
															// 前工程情報読込要求キュー
	void SetQue_MaeRequest(ThreadQueue<TYP_DATASET_REQMAE>* pQue){mque_DataRequest = pQue;}
// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
															// 長周期情報発生要求キュー
	void SetQue_LongPdRequest(ThreadQueue<TYP_DATASET_REQLONGPD>* pQue){mque_DataRequestLongPd = pQue;}
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

//// メンバー関数
protected:
// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>	
	virtual void ThreadFirst_AddHandle();								// シグナル待ち行列の追加
// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------

	//=========================================
	// 共有メモリ操作 (仮想関数)
	//virtual void OnOpenTbl();											// 共有メモリオープン
	//virtual void OnCloseTbl();										// 共有メモリクローズ
	virtual void OnClearTbl(bool bMode);								// 共有メモリ初期化
	virtual void OnSetTbl(TO_SO_HEAD const* pHead);						// 受信データ共有メモリセット
// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>	
//	//virtual void OnAddSignal(int nEventNo);							// シグナルを追加した奴のイベント (追加したものを0始まりとする)
	virtual void OnAddSignal(int nEventNo);							// シグナルを追加した奴のイベント (追加したものを0始まりとする)
// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
	virtual void UpDate_PintWrite(TO_SO_HEAD const* pHead, int rec);	// 書き込みポインタ更新処理
	//virtual void UpDate_PintDisp (TO_SO_HEAD const* pHead, int rec);	// 表示ポインタ更新処理

	//=========================================
	// 実際の書き込み関係
	void SetData_Status(TO_SO_HEAD const* pH, TO_SO_DATA_STATUS const* pD);		// ステータス情報
	void SetData_CoilInf(TO_SO_HEAD const* pH, TO_SO_DATA_COIL const* pD);		// コイル情報（検査用）更新
	void SetData_CoilResult(TO_SO_HEAD const* pH);								// コイル実績 (1コイル検査完了時)
	void SetData_CoilDsp(TO_SO_HEAD const* pH);									// コイル情報（表示用）更新
	void SetData_Detect(TO_SO_HEAD const* pH, TO_SO_DATA_DETECT const* pD);		// 疵情報 (疵情報+疵画像)
	//void SetData_CycleInf(TO_SO_HEAD const* pH, TO_SO_DATA_DETECT const* pD);		// 疵情報 (疵情報+疵画像)
	void SetData_Length(TO_SO_HEAD const* pH, TO_SO_DATA_LENGTH const* pD);		// 表示基点位置＋各表示コイル通板位置情報
	void SetData_Cut(TO_SO_HEAD const* pH, TO_SO_DATA_CUT const* pD);			// カット情報
	void SetData_Alarm(TO_SO_HEAD const* pH, TO_SO_DATA_ALARM const* pD);		// 警報情報
	void SetData_DefLst_Cancel(TO_SO_HEAD const* pH);							// 重欠陥疵情報削除要求
	void SetData_Detect_Count(TO_SO_HEAD const* pH);							// 疵個数情報 (最小区間内)

	//=========================================
	// テーブル操作
	void SetDefectInfo(TO_SO_DATA_DETECT const* pD, const int nRec, const int nMen, const int nIndex);
																				// 疵情報セット
	//void SetCycleInfo(TO_SO_DATA_DETECT const* pD, const int nRec, const int nMen, const int nIndex);
																				// 周期疵情報セット
	void SetDefLst(TO_SO_DATA_DETECT const* pD, char const* pKizukenNo, const int nRec, const int nMen);
	void SetRecvStatus(const int nRec, const int nMen, const int row, const int st);
																				// 疵マップの情報受信状態にデータセット
	void SetRow_Inf(const int nRec, const int nMen, const int nLen);
																				// 行情報にステータスをセット
	void GetOffsetOutLen(const int nRec, int * pOutLen);						// 出側オフセット位置取得
// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
//	void RecalOutLen(const int nRec);											// 出側距離の再設定

	void RecalOutLen(const int nRec, bool bDefect = false);						// 出側距離の再設定
	void GetDeliveryPosY(const int nRec, float dPosY, float* pdDeliveryPosY);	// 出側位置を取得
	void GetDeliveryPosY(const int nRec, int   nPosY, int* pnDeliveryPosY)
	{
		float dPosY =  nPosY;
		float dDeliveryPosY = *pnDeliveryPosY;
		GetDeliveryPosY(nRec, dPosY, &dDeliveryPosY);
		*pnDeliveryPosY = (int)dDeliveryPosY;
	};

	void EditMaeDefectOutLen(TYP_DATASET_REQMAE* pQue);							// 前工程疵情報 出側位置編集

// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------

// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	void EditLongPdInf(TYP_DATASET_REQLONGPD* pQue);							// 長周期情報編集
	void SetLongPdGuidance();													// 長周期ガイダンスセット
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

	void RefRowStatus(const int nRec, const int nStage, const int nMen, const int nBaseRow, const int nEndRow);
																				// 行位置のステータス(稼動状態/検査状態/機器状態)を指定行まで反映
	void DataShiftDefList(int nO_U, int nStartPos, int nEndPos);				// 重欠陥一覧表示疵情報データシフト処理
	void GetDefListFileNo(int nO_U, int nRec, int* nFileNo);					// 重欠陥一覧画像ファイル保存用ファイルNo取得


	//=========================================
	// 小物関係
	//void DeleteNotDispImg();													// 表示範囲外疵画像削除処理
	//int GetDeleteRec();															// 表示範囲外レコード取得処理




//// メンバー変数
private:
	//=========================================
	// 共有メモリ情報
	
	// テーブルハンドル
	HANDLE					m_hStatus;									// ステータス情報テーブル
	HANDLE					m_hPoint;									// ポインタ情報テーブル
	HANDLE					m_hCoilInf;									// コイル情報テーブル
	HANDLE					m_hCoilResult;								// コイル実績テーブル
	HANDLE					m_hMapRow;									// マップ位置テーブル
	HANDLE					m_hDefectInf;								// 疵情報テーブル
	HANDLE					m_hAlarm;									// 警報出力中テーブル
	HANDLE					m_hMaeInf;									// 前工程疵情報テーブル
	HANDLE					m_hDefmap;									// 疵マップ情報テーブル
	//HANDLE					m_hCycleInf;								// 周期疵情報テーブル
	HANDLE					m_hDefList;									// 重欠陥一覧表示ポインタ情報テーブル
	// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	HANDLE					ml_hLongPd;									// 長周期情報テーブル
	// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------	

	// 共有メモリポインタ
	TBL_HY_STATUS*			mtbl_pStatus;								// ステータス情報テーブル
	TBL_HY_POINT*			mtbl_pPoint;								// ポインタ情報テーブル
	TBL_HY_COILINF*			mtbl_pCoilInf;								// コイル情報テーブル
	TBL_HY_COILRESULT*		mtbl_pCoilResult;							// コイル実績テーブル
	TBL_HY_MAPROW*			mtbl_pMapRow;								// マップ位置テーブル
	TBL_HY_DEFECTINF*		mtbl_pDefectInf;							// 疵情報テーブル
	TBL_HY_ALARM*			mtbl_pAlarm;								// 警報出力中テーブル
	TBL_HY_MAE_INF*			mtbl_pMaeInf;								// 前工程疵情報テーブル
	//TBL_HY_CYCLE_INF*		mtbl_pCycleInf;								// 周期疵情報テーブル
	TBL_HY_DEFLIST*			mtbl_pDefList;								// 重欠陥一覧表示ポインタ情報テーブル
	// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	TBL_HY_LONGPD*			mtbl_pLongPd;								// 長周期情報テーブル
	// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------	

	//=========================================
	// その他もろもろ
	char					m_cPath_Img[SIZE_IMAGE_PATH];				// 画像保存ベースパス
	char					m_cPath_Mae[SIZE_IMAGE_PATH];				// 画像保存ベースパス[前工程]
	char					m_cPath_Lst[SIZE_IMAGE_PATH];				// 画像保存ベースパス[重欠陥一覧]
	bool					m_bQueState;								// キュー空き状態
	int						m_nKensaNum[NUM_MEN];						// 検査台番号

	int						m_nImgSaveOff;								// 探傷画面表示用の画像保存有無 1:保存しない、以外：保存する
	int						m_nMaeImgSaveOff;							// 前工程疵画像保存有無         1:保存しない、以外：保存する
	int						m_nLstImgSaveOff;							// 重欠陥疵画像保存有無         1:保存しない、以外：保存する

	int						m_nLstSaveNo[NUM_MEN];						// 重欠陥疵画像保存番号（最終保存番号 ※＋１して次は使う）

// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
	// スレッドキュー
	ThreadQueue<TYP_DATASET_REQMAE>*	mque_DataRequest;				// データセットへの前工程疵編集依頼キュー
// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	ThreadQueue<TYP_DATASET_REQLONGPD>*	mque_DataRequestLongPd;			// データセットへの長周期情報編集依頼キュー
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

	// 大量ログ出力防止対策
	int						m_nErrCnt_Skip_Defect;						// スキップ 行列情報
	int						m_nErrCnt_Err_Key;							// レコード無し 行列情報
	int						m_nErrCnt_Err_Coil;							// 論理コイル無し 行列情報
	int						m_nErrCnt_NG_Val;							// 整合性異常 行列情報
	int						m_nErrCnt_Max_Cnt;							// 上限到達 行列情報
};
