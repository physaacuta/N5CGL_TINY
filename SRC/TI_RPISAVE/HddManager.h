// *********************************************************************************
//	HDD制御クラス
//	[Ver]
//		Ver.01    2010/03/04	初版
//
//	[メモ]
//		・全長保存PCの空き容量をチェックして、最古から消す
//		・RPIファイルが出来上がったタイミングで 全長保存PCに移動
//		・移動し終わって、自フォルダ内に 今コピーした奴より１日以上古い日付のファイルが有れば、バックアップ移動
// *********************************************************************************

#pragma once
// 共通ヘッダー
#include "..\..\Include\KizuCommon.h"									// システム用インクルード

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー

#include <direct.h>														// フォルダ生成
#include <shlwapi.h>													// パス文字操作関係

// STL
#include <vector>														// リスト用
#include <algorithm>													// ソート用

// ローカルのクラス
#include "MainConst.h"
#include "ParamManager.h"												// パラメータ管理クラス
#include "RpiWriteProc.h"												// RPIファイル書込クラス

using namespace KizuLib;

class HddManager : public ThreadManager
{


//// メンバー定数
private:
	struct RPI_ITEM {
		CString			sName;			// RPIファイル名
		CTime			tTime;			// アクセス日付
		LARGE_INTEGER	nSize;			// ファイルサイズ
	};

	// 定数
	static const int COPY_VIEW_SIZE		= 1*1024*1024;					// RPIファイルを一回にコピーするサイズ[MB] このくらいが最速。大きすぎても、小さすぎても逆に遅くなる
	static const int FOLDER_CHECK_TIME	= 2 * 60 * 1000;				// フォルダチェック周期 [ms]


//// 公開関数
public:
	HddManager(void);
	virtual ~HddManager(void);


	// プロパティ
	void SetParamMgr(ParamManager* pCls) { mcls_pParam = pCls; }		// パラメータ管理	
	void SetQueRpiMove(ThreadQueue<RPI_MOVE>* pQue) { mque_pRpiMove = pQue; }	//
	void SetQueRpiWrite(ThreadQueue<DELIVERY_RPIWRITE>* pQue[][MAX_COLOR_NUM]){ for(int ii=0; ii<MAX_INS; ii++) for(int jj=0; jj<MAX_COLOR_NUM; jj++) mque_pRpiWrite[ii][jj] = pQue[ii][jj]; }	// RpiProcに渡すキュー

	// 小物
	int SearchFolderIndex(int nIns, bool bIsNew);						// 新旧ファイルのフォルダIndexを算出


//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	

	bool Exec(RPI_MOVE const* p);										// メイン処理
	bool FreeSpace(char const* sFolder,const int nFreeSpaceGB);								// 全長画像保存PCの空きエリアを確保
	bool CopyFileExec(RPI_MOVE const* p, int nHddIndex);				// 普通のファイル移動
	void FastDelete();													// 初期起動時の ゴミ削除
	void BackUpCopy(RPI_MOVE const* p=NULL);							// 一次フォルダにあるRPIファイルを移動 (正常時では対象ファイルは無いはず)

	// 小物
	static inline bool sort_fd_old(const RPI_ITEM &a, const RPI_ITEM &b);	// ソート関数
	static inline bool sort_fd_new(const RPI_ITEM &a, const RPI_ITEM &b);	// ソート関数
	bool AccessSaveFolder(int nIns);									// 長期保存用フォルダにアクセス可能かチェック
	void StartTimer_SaveFolder();										// 長期保存用フォルダにアクセス可能チェックタイマー

	bool RootFolderCheck(char const* cPath1, char const* cPath2);		// 同一ドライブかチェック
	double CheckFreeSpace(char const* sFolder);							// 全長画像保存PCの空きエリアチェック
	bool   CheckFreeSpace(char const* sFolder, double dLimit);			// 全長画像保存PCの空きエリアチェック
	bool GetFileUmu(char const* cFPath);								// ファイル有無をチェック



//// メンバー変数
private:

	// 受け渡しキュー
	ThreadQueue<RPI_MOVE>*				mque_pRpiMove;					// RPIファイル移動要求受け渡しキュー [in]
	ThreadQueue<DELIVERY_RPIWRITE>*	mque_pRpiWrite[MAX_INS][MAX_COLOR_NUM]; // RPIファイル書き込みへ受け渡しキュー	[out]


	// 各インスタンス
	ParamManager*						mcls_pParam;					// パラメータ管理

	// ハンドル
	HANDLE								m_evTimerCheck;					// 保存先フォルダアクセスチェック タイマー [ms]

	// その他もろもろ
	bool								m_bSaveFolderAccess[MAX_INS];	// 保存先フォルダにアクセス可能か (true:アクセス可能 false:不可)
};
