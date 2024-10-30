// *********************************************************************************
//	メイン管理スレッド
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//		
//
// *********************************************************************************

#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネジャー

// ローカル
#include "MainConst.h"													// 自クラス定数ヘッダー
#include "LinkswManager.h"

using namespace KizuLib;

class MainInstance : public ThreadManager
{
//// 公開関数
public:
	MainInstance(void);
	virtual ~MainInstance(void);

	void Start()														// スレッド開始 (MainInstanceの時だけはオーバーライド)
	{
		SetEvent(m_evThStart);
	}

	BOOL Init();														// 初期化処理
	void Exit();														// 終了処理

//// 公開変数
public:
	// 各ワーカースレッド
	LinkswManager *				gcls_pLinksw[MAX_THREAD];				// カメラリンクスイッチ通信クラス

//// メンバー関数
private:
	// スレッド動作定義
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生

//// メンバー変数
private:
	// 同期制御
	HANDLE						m_evThStart;							// MainInstanceスレッドの実行要求

	// その他もろもろ
	int							m_nUnitNum;								// 管理対象機器数

// ======================================================================
// 小物
public:
	// ループ算出
	void GetChLoop(int nUnit, int& ii_s, int& ii_e)						// ユニット指定時の対象算出
	{
		if (0 == nUnit) { ii_s = 0; ii_e = m_nUnitNum; }
		else { ii_s = nUnit - 1; ii_e = nUnit; }
	}
};
