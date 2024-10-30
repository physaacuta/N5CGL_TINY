// *********************************************************************************
//	結果表示クラス
//	[Ver]
//		Ver.01    2010/03/04	初版
//
//	[メモ]
//		・
// *********************************************************************************

#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLibMFC\Class\ImageWindow.h"					// 画像表示制御クラス

#include "..\..\Library\KizuLib\CLASS\KizuBitmapManager.h"

// ローカル
#include "MainConst.h"													// 自クラス定数ヘッダー
#include "ParamManager.h"												// パラメータ管理クラス
#include "FrameDataManager.h"											// フレームデータ管理クラス

#include "ProcessQueThread.h"											// ProcThread基底

using namespace KizuLib;

class DispProc : public ThreadManager,
				 public ProcessQueThread<FrameDataManager, FrameDataManager>
{
//// 公開関数
public:
	DispProc(void);
	virtual ~DispProc(void);

	void Reset();														// リセット
	void SetIsExec(bool bMode) { m_bIsExec = bMode; }					// 処理実行有無


	// プロパティ
	void SetParamMgr(ParamManager* pCls) { mcls_pParam = pCls; }		// パラメータ管理	
	void SetImgWin(ImageWindow* pCls) { mcls_pImgWin = pCls; }			// 画像表示クラス

	// 画面表示非表示
	void SetEv_ShowWin()	{ SetEvent(m_evShowWin); }
	void SetEv_CloseWin()	{ SetEvent(m_evCloseWin);}
	void WindowIsOnOff(bool bOnOff);					// 画面描画On/Off



//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	
	// 処理
	virtual FrameDataManager* OnAddNewItem( FrameDataManager* pIn );	// 実処理
	void Exec(FrameDataManager* p);										// 処理実行

	// 画面表示
	void ShowWindow();
	void CloseWindow();

//// メンバー変数
private:
	//// 各インスタンス
	ParamManager*						mcls_pParam;					// パラメータ管理
	ImageWindow*						mcls_pImgWin;					// 画像表示クラス

	//// 共通的な情報
	int									m_nIns;							// インスタンス番号 (0オリジン)
	bool								m_bIsExec;						// 実行許可時 true (撮像開始時true)

	//// ウィンドウ表示用
	HANDLE								m_evShowWin;					// 画面表示要求
	HANDLE								m_evCloseWin;					// 画面終了要求
	bool								m_bIsDisp;						// ImgWindow表示中 true
	DWORD								m_nDspInterval[MAX_INS][MAX_COLOR_NUM];		// 前回表示時間 [ms]
	CRITICAL_SECTION					my_cs;							// クリティカルセクション


//// 画面への結果表示用だから、めんどくさいのでグローバル
public:
	//// 時間関係
	double								g_dTime_Iport[2];				// 最初の画像取得から最後の画像取得までに掛かった時間 (平均,最大)
	double								g_dTime_PlgSync[2];				// PLGの紐付けに掛かった時間
//	double								g_dTime_CoilControl[2];			// コイル紐付け時間
	double								g_dTime_ImgUnion[2];			// 画像連結時間
//	double								g_dTime_ImgColor[2];			// 連結後画像処理時間
	double								g_dTime_ImgBuf[2];				// 画像Buf時間
	double								g_dTime_ImgComp[2];				// 画像圧縮時間
	double								g_dTime_RpiWrite[MAX_INS][2];	// RPIファイル書込み時間
};
