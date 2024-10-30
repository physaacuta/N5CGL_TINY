// *********************************************************************************
//	画像圧縮クラス (並列処理)
//	[Ver]
//		Ver.01    2010/03/04	初版
//
//	[メモ]
//		・連結後画像フレーム単位で処理
// *********************************************************************************

#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLibMFC\Class\ThreadSyncManager.h"			// 並列スレッド間順序保障制御クラス

///#include "..\..\Library\DefectLib\KizuJpegManager.h"					// Bitmap⇔Jpeg変換クラス
#include "..\..\Library\KizuLib\Class\KizuBitmapManager.h"


// ローカル
#include "MainConst.h"													// 自クラス定数ヘッダー
#include "ParamManager.h"												// パラメータ管理クラス
#include "FrameDataManager.h"											// フレームデータ管理クラス
#include "ProcessQueThread.h"											// ProcThread基底

//#include "LacManager.h"												// 高度圧縮クラス
#include "../../LibPG/lac/LacManager.h"									// 高度圧縮クラス
#include "RpiFileManager.h"												// 太平製RPIファイルクラス

using namespace KizuLib;

class ImgCompProc :	public ThreadManager,
					public ProcessQueThread<FrameDataManager, FrameDataManager>
{
//// 公開関数
public:
	ImgCompProc(int nId);
	virtual ~ImgCompProc(void);

	void Reset();														// リセット
	void SetIsExec(bool bMode) { m_bIsExec = bMode; }					// 処理実行有無

	
	// プロパティ
	void SetParamMgr(ParamManager* pCls) { mcls_pParam = pCls; }		// パラメータ管理	

	// 画面
	bool	IsComp() const{return m_bIsComp;}


//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	
	// 処理
	virtual FrameDataManager* OnAddNewItem( FrameDataManager* pIn );	// 実処理
	void Exec(FrameDataManager* p);										// 処理実行



//// メンバー変数
private:
	//// 受け渡しキュー

	//// 各インスタンス
	ParamManager*						mcls_pParam;					// パラメータ管理
	
///	KizuJpegManager						mcls_Jpeg;						// JPEG圧縮クラス
	LacManager							mcls_Lac;						// 高度圧縮クラス

	// 共通的な情報
	bool								m_bIsExec;						// 実行許可時 true (撮像開始時true)
	bool								m_bIsComp;						// 圧縮中true

	BYTE*								m_imgwk;

	int									m_nCompQuality;					// 圧縮率
	int									m_nUionWidth;					// フレーム画像連結後の画像幅
	int									m_nUionHeight;					// フレーム画像連結後の画像高さ

};
