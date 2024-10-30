// *********************************************************************************
//	幅方向カメラの紐付けクラス
//	[Ver]
//		Ver.01    2010/03/04	初版
//
//	[メモ]
//		・
// *********************************************************************************

#pragma once
// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLib\Class\KizuPerformanceCounter.h"			// タイマー
#include "..\..\Library\KizuLib\Class\KizuBitmapManager.h"

// ローカル
#include "MainConst.h"													// 自クラス定数ヘッダー
#include "ProcessQueThread.h"											// ProcThread基底

#include "ParamManager.h"												// パラメータ管理クラス
#include "StatusManager.h"												// ステータス制御クラス

using namespace KizuLib;

class ImgSyncProc :	public ThreadManager,
					public ProcessQueThread<HT_RPI_FDATA, FRAME_DATA_ARRAY>
{
public:
	ImgSyncProc(int nIns);
	virtual ~ImgSyncProc(void);

	void Reset();														// リセット
	void SetIsExec(bool bMode) { m_bIsExec = bMode; }					// 処理実行有無


	// プロパティ
	void SetStatusMgr(StatusManager* pCls) { mcls_pStatus = pCls; }		// ステータス管理
	void SetParamMgr(ParamManager* pCls) { mcls_pParam = pCls; }		// パラメータ管理	
	
	void SetQueFree( IQueue<HT_RPI_FDATA >  *pQue ) { mque_pFree  = pQue; }		// 返却Queセット


	// 画面
	DWORD GetRecvFrameNo(int id) const {return m_nRecvFrameNo[id];}
	DWORD GetNowExecFrameNo(int id) const {return m_nRecvFrameNo[id];}
	void SetDetailLog( KizuLib::LogFileManager *log) { mcls_detail = log; }


//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	

	// 処理
	virtual FRAME_DATA_ARRAY* OnAddNewItem( HT_RPI_FDATA* pIn ) {return NULL;};	// 実処理 カスタマイズするので、デフォルト無し
	void AddNewItem();													// 画像取得
	void Exec();														// 処理実行

	void PoolBack_RecvFreeQue(int nIndex);								// 受信キューバッファ器に入っている皿を返却
	void ItemReset();													// 受信キューバッファ器の初期化だけ

	bool CheckAndAdd(HT_RPI_FDATA* p);									// 全カメラセットで同期完了？
	bool CheckCamset();									// 全カメラセットが揃ったかチェック
	bool CheckSeqNo(HT_RPI_FDATA* p);									// １カメラでの送信連番チェック
	int OutputIncompleteQueue(HT_RPI_FDATA* p);


	// バッファリング管理
	bool AddToTail(int cam, HT_RPI_FDATA* p);							// バッファリング
	HT_RPI_FDATA* GetFromHead(int cam);									// バッファ取り出し
	HT_RPI_FDATA* PeekFromHead(int cam);								// バッファみるだけ
	void ResetBufFrame();												// バッファクリア
	void ResetBufFrame(int nIndex);


	HT_RPI_FDATA* CreateDummyFrame(const HT_RPI_FDATA* p, int nCamId, int nCamIndex);

public:	
	int GetMaxBufFrameNum();											// バッファ最大数
private:

	KizuLib::LogFileManager *			mcls_detail;
	HT_RPI_FDATA *GetMinimumFrame( bool bNeedAllCamera, int *pnCamIndex );

//// メンバー変数
private:

	// 受け渡しキュー
	IQueue<HT_RPI_FDATA>*				mque_pFree;					// フレーム画像受信キュー 返却場所


	// 各インスタンス
	StatusManager*						mcls_pStatus;					// ステータス管理
	ParamManager*						mcls_pParam;					// パラメータ管理
	KizuPerformanceCounter				mcls_Time;						// フレーム取得時間計測用


	// 共通的な情報
	int									m_nIns;							// インスタンス番号 (0オリジン)
	bool								m_bIsExec;						// 実行許可時 true (撮像開始時true)
	bool								m_bIsSync;						// 初回同期完了時 true
	DWORD								m_nRecvFrameNo[NUM_CAM_POS];	// 受信フレームNo (0オリジン)
	DWORD								m_nEndFrameNo;					// 処理完了フレームNo

	std::deque<HT_RPI_FDATA*>			m_pBufFrame[NUM_CAM_POS];		// 受信キューバッファ器(同一カメラが複数回来た対策)
	bool m_bEnabled[NUM_CAM_POS];

};

