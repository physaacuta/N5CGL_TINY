#pragma once

// 標準
#include <direct.h>

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー


// 疵検共通ヘッダー
#include "..\..\include\LineCommon.h"									// 疵検共通ヘッダー
#include "..\..\include\HyTbl.h"										// 操作PC共有メモリヘッダー

#include "ParamManager.h"												// パラメータ管理クラス
#include "FileDeleteManager.h"											// ファイル削除管理クラス
#include "FileSaveManager.h"											// ファイル保存管理クラス

class DataSetBaseManager :	public ThreadManager
{
//// 公開定数
public:
	static const int		MAX_MEM_COUNT = 64;							// 共有メモリ 最大管理数

	// 公開構造体
	struct MEM_INF
	{
		HANDLE*				h;											// ハンドル
		void**				pTbl;										// テーブル
		int					nSize;										// サイズ
		char				cName[64];									// 名称
		bool				bNomalCrearCancel;							// 初期化モード (True:通常初期化は実施しない)
	};

//// 公開関数
public:
	DataSetBaseManager(void);
	virtual ~DataSetBaseManager(void);

	//=========================================
	// セット
	void SetParamMgr(ParamManager* pCls)		{ mcls_pParam = pCls; }	// パラメータ管理
	void SetFDeleMgr(FileDeleteManager* pCls)	{ mcls_pFDele = pCls; }	// ファイル削除管理
	void SetQueImgSave(ThreadQueue<FileSaveManager::TYP_SAVE_IMG>* pQue) { mque_pImgSave = pQue; }			// 受け渡しデータ用キュー
	void SetQueImgSaveLow(ThreadQueue<FileSaveManager::TYP_SAVE_IMG>* pQue) { mque_pImgSaveLow = pQue; }	// 受け渡しデータ用キュー

	void SetQueOpRecv(ThreadQueue<SOCK_BASE_HEAD>* pQue) { mque_pOpRecv = pQue; }	// 統括→表示送信通知のキュー


	// キュー取得
	ThreadQueue<SOCK_BASE_HEAD>* GetQueOpRecv() { return mque_pOpRecv; }// 統括→表示送信通知のキューのポインタ取得
	int GetToHyQueCount() { return mque_pOpRecv->GetCount(); }			// 統括→表示送信通知のキューの件数を取得

	//=========================================
	// 基本関係
	int   GetStageIndex(int nStage, int nRow );							// 基準レコードの行から、各ステージの行を算出
	int	  GetStageLenToIndex(int nStage, int nLen );					// 指定距離から、各ステージの行位置を算出


	//=========================================
	// その他もろもろ
	static void		CreateFolder(CString sPath, int nRec);				// 格納フォルダ作成
	static int		GetRecIndex(int rec, int index);					// レコード位置取得
	static CString	GetFolderPath(CString sBasePath, int rec);			// フォルダパス生成
	
	int	CheckKanriNo(char const* pKizukenNo, TBL_HY_POINT const* pTblPoint);		// ポインターテーブルに保持している疵検管理Noと一致するかチェック


	//=========================================
	// 外部アクセス
	void SetEvClearTbl()	{ SetEvent(m_evClearTbl); }



	//=========================================
	// デバック関係 (仮想関数)
	virtual void OnOutputTbl();											// 全共有メモリ バイナリ出力
	virtual void OnInputTbl();											// 全共有メモリ バイナリ読込



//// メンバー関数
protected:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生

	// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
	//virtual void ThreadFirst_AddHandle() {};							// シグナル待ち行列の追加
	virtual void ThreadFirst_AddHandle(){ThreadFirst_AddHandle();};		// シグナル待ち行列の追加
	// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------

	//=========================================
	// 共有メモリ操作 (仮想関数)
	bool		 AddMemInf(HANDLE* h, void** pTbl, int nSize, char const* cName, bool bNomalCrearCancel=false);	// 共有メモリ情報を登録
	virtual void OnOpenTbl();											// 共有メモリオープン
	virtual void OnCloseTbl();											// 共有メモリクローズ
	virtual void OnClearTbl(bool bMode);								// 共有メモリ初期化
	virtual void OnSetTbl(TO_SO_HEAD const* pHead) {};					// 受信データ共有メモリセット
	// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
	//virtual void OnAddSignal(int nEventNo) {};							// シグナルを追加した奴のイベント (追加したものを0始まりとする)
	virtual void OnAddSignal(int nEventNo){ OnAddSignal(nEventNo); };		// シグナルを追加した奴のイベント (追加したものを0始まりとする)
	// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------

	//=========================================
	// 実際の書き込み関係
	void SetData_PointWrite(TO_SO_HEAD const* pHead, TBL_HY_POINT* pTblPoint);		// 書き込みポインタ更新
	void SetData_PointDisp (TO_SO_HEAD const* pHead, TBL_HY_POINT* pTblPoint);		// 表示ポインタ更新

	virtual void UpDate_PintWrite(TO_SO_HEAD const* pHead, int rec) {};				// 書き込みポインタ更新処理
	virtual void UpDate_PintDisp (TO_SO_HEAD const* pHead, int rec) {};				// 表示ポインタ更新処理
	


	//=========================================
	// その他もろもろ
	void QueBufFree(TO_SO_HEAD* pHead);									// 現バッファを開放する


//// メンバー変数
protected:

	//=========================================
	//// 各インスタンス
	ParamManager*					mcls_pParam;						// パラメータ管理
	FileDeleteManager*				mcls_pFDele;						// ファイル削除管理クラス

	// 受け渡しキュー
	ThreadQueue<FileSaveManager::TYP_SAVE_IMG>*	mque_pImgSave;			// 受け渡しデータ用キュー (代表疵)
	ThreadQueue<FileSaveManager::TYP_SAVE_IMG>*	mque_pImgSaveLow;		// 受け渡しデータ用キュー (リスト部で隠れる疵)


	//=========================================
	//// 共有メモリ情報
	int								m_nMemSetCnt;						// セット数
	MEM_INF							m_MemInf[MAX_MEM_COUNT];			// 使いやすいようにループ用


//// メンバー変数
private:

	// 受け渡しキュー
	ThreadQueue<SOCK_BASE_HEAD>*	mque_pOpRecv;						// 統括→表示送信通知のキュー

	//=========================================
	// シグナル
	HANDLE							m_evClearTbl;						// 共有メモリ初期化イベント
};
