//=====================================================================
// 画像データを非同期に保存する
//	【メモ】
//		・受信タイミングで保存していると、時間が掛かるための対策
//=====================================================================

#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー


// 疵検共通ヘッダー
#include "..\..\include\KizuCommon.h"									// 疵検共通ヘッダー

using namespace KizuLib;

class FileSaveManager :	public ThreadManager
{
//// 公開定数
public:

	// 疵画像保存用テーブル
	struct TYP_SAVE_IMG
	{
		int						nSetCnt;								// データセット数
		char					cPath[MAX_CAMANGLE][32];				// 保存パス
		int						nSize[MAX_CAMANGLE];					// 画像サイズ
		BYTE*					pImg [MAX_CAMANGLE];					// 画像データ

		char*					pMemPath[MAX_CAMANGLE];					// 共有メモリパス	(NULL以外の場合、画像保存後、保存パスをここに書き込む)
	};


//// 公開関数
public:
	FileSaveManager(void);
	virtual ~FileSaveManager(void);

	// プロパティ
	void SetQueImgSave	 (ThreadQueue<TYP_SAVE_IMG>* pQue) { mque_pImgSave = pQue; }		// 受け渡しデータ用キュー
	void SetQueImgSaveLow(ThreadQueue<TYP_SAVE_IMG>* pQue) { mque_pImgSaveLow = pQue; }		// 受け渡しデータ用キュー

	// 外部アクセス
	void SetEvClearQue()		{ SetEvent(m_evClearQue); }				// 未処理キューを全削除


//// 公開変数
public : 


//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	
	void AllQueFree(ThreadQueue<TYP_SAVE_IMG>* pQue);					// 全キュー開放
	void SaveImg(TYP_SAVE_IMG* p);										// 画像ファイル書き込み


//// メンバー変数
private:
	// スレッドキュー
	ThreadQueue<TYP_SAVE_IMG>*	mque_pImgSave;							// 受け渡しデータ用キュー (代表疵)
	ThreadQueue<TYP_SAVE_IMG>*	mque_pImgSaveLow;						// 受け渡しデータ用キュー (リスト部で隠れる疵)


	// シグナル
	HANDLE						m_evClearQue;							// 初期化イベント

};
