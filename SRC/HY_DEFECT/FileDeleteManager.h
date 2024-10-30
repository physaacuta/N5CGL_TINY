//=====================================================================
// 過去の画像データを削除
//	【メモ】
//		・次レコード切り替え時に、指定レコードを対象に削除する
//		・4日以上経過した古いJPEG画像を削除
//=====================================================================

#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー


// 疵検共通ヘッダー
#include "..\..\include\KizuCommon.h"									// 疵検共通ヘッダー


using namespace KizuLib;

class FileDeleteManager : public ThreadManager
{
	//// 公開定数
public:

	// 疵画像保存用テーブル
	struct TYP_DELETE_FILE
	{
		char					cPath[128];								// パス
	};

	//// 公開関数
public:
	FileDeleteManager(void);
	virtual ~FileDeleteManager(void);


	bool	DeleteStart(char const* pPath);								// 削除開始
	bool	DeleteAll(char const* pPath);								// 削除開始(QUE)
	bool	IsRun() const { return m_bRun; }							// 削除中

	ThreadQueue<TYP_DELETE_FILE>	mque_pFileDel;						// 受け渡しデータ用キュー


//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	

	void DeleteExec();													// 削除処理
	void DeleteExecQue(char const* pPath);								// 削除処理(QUE)


//// メンバー 変数
private:
	

	//=========================================
	// シグナル
	HANDLE					m_evRuning;									// 実行中シグナル (手動)
	HANDLE					m_evRunEnding;								// 実行中断要求シグナル
	HANDLE					m_evRunEnd;									// 実行中断完了シグナル


	//=========================================
	// 状態
	bool					m_bRun;										// 削除中 true
	char					m_cPath[128];								// 削除対象フォルダ

};
