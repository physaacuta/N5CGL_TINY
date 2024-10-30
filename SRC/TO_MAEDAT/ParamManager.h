// *********************************************************************************
//	パラメータ管理クラス
//	[Ver]
//		Ver.01    2012/09/06
//
//	[メモ]
//		
// *********************************************************************************

#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLibMFC\Class\CreateSQL.h"					// SQL文生成補助クラス
#include "..\..\Library\DefectLib\DBSaveManager.h"						// DB書込み基本クラス
#include "..\..\Library\KizuODBC\OdbcBase.h"							// DB操作クラス

// ローカルのクラス
#include "MainConst.h"

using namespace KizuLib;
using namespace KizuODBC;

class ParamManager : public ThreadManager
{
public:

	//=======================================
	// 疵種 構造体
	//=======================================
	struct PARAM_TYPE
	{
		int num;									// 総個数
		struct TYPE_ID
		{
			char cProc[2];							// 前工程コード
			char cFFName[2];						// FFName
			int	edasysID;							// edasysID

		} type_id[MAX_MAE_TYPE];
	};

	//=======================================
	// グレード 構造体
	//=======================================
	struct PARAM_GRADE
	{
		int num;									// 総個数
		struct GRADE_ID
		{
			char cProc[2];							// 前工程コード
			char cFFName[2];						// FFName
			int	edasysID;							// edasysID
		} grade_id[MAX_MAE_GRADE];
	};

	//=======================================
	// 前工程マスターテーブル 構造体
	//=======================================
	struct PARAM_MASTER
	{
		int num;									// 総個数
		struct MASTER_ID
		{
			char cProc[2];							// 前工程コード
			int  nEnable;							// 有効判定[0:無効、1:有効]
			int  nMode;								// 前工程表示モード
			int  nImgFmt;							// 画像形式[0:JPEG、1:BMP]
            // 2014.07.25 川原  所間連携    >>>>>>>>
			int  nTasyoZai;							// 他所材判定
            // 2014.07.25 川原  所間連携    <<<<<<<<
		} master_id[MAX_MAE_LINE];
	};



//// 公開関数
public:
	ParamManager(void);
	~ParamManager(void);

	void SetReadParam() {	SetEvent(m_evReadParam); }	;				// パラメータ読み込み
	
	bool ReadParam_All();												// 全読み込み
	bool ReadParam_Type();												// 疵種パラメータ読込
	bool ReadParam_Grade();												// グレードパラメータ読込
	bool ReadParam_Master();											// 前工程マスターテーブル読込
	int  GetTyppeEdasysID(char* cProc, char* cFFName) const;			// 疵種EdasysID取得
	int  GetGradeEdasysID(char* cProc, char* cFFName) const;			// グレードEdasysID取得
	int  GetDspMode(char* cProc) const;									// 前工程マスター前工程表示モード取得

    // 2014.07.25 川原  所間連携    >>>>>>>>
	int  GetTasyoZai(CString cProc);									// 他所材判定取得
    // 2014.07.25 川原  所間連携    <<<<<<<<

//// 公開変数
public:



//// メンバー関数
private:		
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent( int nEventNo );									// スレッドイベント発生
	void ParamManager::ExecParam( COMMON_QUE* pQue );					// パラメータ読込制御
	

//// メンバー変数
private:
	//// 各インスタンス
	
	// シグナル
	HANDLE m_evReadParam;												// パラメータ読み込み 


	// パラメータ
	PARAM_TYPE				m_Param_Type;								// 疵種
	PARAM_GRADE				m_Param_Grade;								// グレード
	PARAM_MASTER			m_Param_Master;								// 前工程マスター

};
