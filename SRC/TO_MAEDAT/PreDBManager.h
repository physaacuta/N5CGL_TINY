// *********************************************************************************
//	前工程実績情報DB登録管理クラス
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
#include "..\..\include\ToTbl.h"										// コイル情報

// ローカルのクラス
#include "MainConst.h"
#include "ParamManager.h"
// 2013.10.29 諸藤 統合サーバー参照 ------->>>
#include "PreDBSelectManager.h"
// 2013.10.29 諸藤 統合サーバー参照 <<<-------

using namespace KizuLib;
using namespace KizuODBC;
class PreDBManager : public ThreadManager
{

public:

	//// 受け渡し構造体
	struct DeliveryPreDB {
		char cKizuKenNo[SIZE_KIZUKEN_NO+1];				// 管理No
		char cPreKizuKenNo[SIZE_KIZUKEN_NO+1];			// 前工程管理No
		char cLineCode[2+1];							// 工程コード(疵検出工程)	2013.10.30 諸藤 広畑材の表示
		int nOuDiv;										// 表裏区分
		char cMeiNo[SIZE_MEI_NO+1];						// 命令No
		char cCoilNo[SIZE_COIL_NO+1];					// コイルNo
	};

//// 公開関数
	PreDBManager();
	~PreDBManager(void);

    // 2014.07.25 川原  所間連携    >>>>>>>>
    void SetParamManager(ParamManager* pCls) {mcls_pParam = pCls;}  // パラメータ管理クラスセット
    // 2014.07.25 川原  所間連携    <<<<<<<<

// 2013.10.29 諸藤 統合サーバー参照 ----->>>
private:

	//// 各種パラメータ
	enum EM_PRE_LINE{			    // 前工程ライン
		PRE_LINE_2TCM	    = 0,    // 2TCM
		PRE_LINE_3TCM       = 1,    // 3TCM
									// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 ----->>>
		PRE_LINE_N2TCM_ANA	= 2,	// 2TCM_ANA
									// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 <<<-----
		// 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
		PRE_LINE_N2TCM_KIZU = 3		// 2TCM_KIZU
		// 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<

	};

    enum EM_PRE_MILL{			// 自所材、他所材
		NAGOYA_MILL		= 0,	// 自所材
		ANOTHER_MILL	= 1     // 他所材
	};

#define SYSLOG_BASE		PARAM_SYSLOG_BASENO + 2
    enum EM_ALERT_NO{			// 接続先DBに関するアラームNo
		ALERT_OK = SYSLOG_BASE, 
		ALERT_NG_PING,
		ALERT_NG_CONNECT,
		ALERT_NG_SEARCH,
		NON_ALERT				// 前回と同じ結果の為、シスログ出力なし
	};

// 2013.10.29 諸藤 統合サーバー参照 <<<-----

//// 公開変数
public:
	//// スレッドキュー
	ThreadQueue<DeliveryPreDB> gque_AddSQLData;							// 受け渡しデータ用キュー



//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent( int nEventNo );									// スレッドイベント発生

	// シーケンス制御
	void DBInsMain( DeliveryPreDB* pDeli );																// DB登録メイン処理
	bool CheckInsert( OdbcBase* clspDB, DeliveryPreDB* pDeli );											// 登録対象かチェック
	bool DBIns( EM_PRE_LINE PreLine, DeliveryPreDB* pDeli, OdbcBase* clsInsDB, OdbcBase* clsPreDB );	// DB登録処理
	
    bool GetNew2TCMInf( OdbcBase* clsPreDB, MAE_DEFECT_IMAGE_INF* typData );							// 2TCM情報取得
	bool GetNew3TCMInf( OdbcBase* clsPreDB, MAE_DEFECT_IMAGE_INF* typData );							// 3TCM情報取得
																										// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 ----->>>
	bool GetNewN2TCM_ANAInf(OdbcBase* clsPreDB, MAE_DEFECT_IMAGE_INF* typData);							// N2TCM_ANA情報取得
																										// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 <<<-----
	// 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
	bool GetNewN2TCM_KIZUInf(OdbcBase* clsPreDB, MAE_DEFECT_IMAGE_INF* typData);						// N2TCM_KIZU情報取得
	// 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<

	bool InsertDefectInf( OdbcBase* clsDB, MAE_DEFECT_IMAGE_INF* typData );								// 前工程疵情報登録
	bool InsertDefectImg( OdbcBase* clsDB, MAE_DEFECT_IMAGE_INF* typData );								// 前工程疵画像登録
	void UpdateCoilLine( OdbcBase* clsDB, DeliveryPreDB* pDeli );										// 前工程ライン別コイル情報更新

	// 2013.10.30 諸藤 広畑材の表示 ------->>>
	int CheckAnotherMill(char* LineCode);														        // 自所材、他所材の判定
	int GetAlertNo(PreDBSelectManager::EM_SELECT_DB nSelect,EM_PRE_LINE nPreLine);						// 接続結果からアラームNoを決定
	// 2013.10.30 諸藤 広畑材の表示 <<<-------

	void SendMailDefect(char* cMeiNo, char* cCoilNo);													// 前工程疵情報書込み完了通知送信

//// メンバー変数
private:
	//// 各インスタンス
	
	PreDBSelectManager* mcls_pSelect2TCM;						// 前工程DB選択クラス(2TCM)
	PreDBSelectManager* mcls_pSelect3TCM;						// 前工程DB選択クラス(3TCM)
	PreDBSelectManager::EM_SELECT_DB	m_emLastResult_2TCM;	// 前回の選択結果
	PreDBSelectManager::EM_SELECT_DB	m_emLastResult_3TCM;	// 前回の選択結果
																// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 ----->>>
	PreDBSelectManager* mcls_pSelectN2TCM_ANA;					// 前工程DB選択クラス(2TCM_ANA)
	PreDBSelectManager::EM_SELECT_DB	m_emLastResult_N2TCM_ANA;// 前回の選択結果
																 // 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 <<<-----
	// 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
	PreDBSelectManager* mcls_pSelectN2TCM_KIZU;					// 前工程DB選択クラス(2TCM_ANA)
	PreDBSelectManager::EM_SELECT_DB	m_emLastResult_N2TCM_KIZU;// 前回の選択結果
	// 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<

    ParamManager*			mcls_pParam;				    // パラメータ管理クラス

	//// 共有メモリ情報
	HANDLE					m_hCoil;									// コイル情報 ハンドル
	TO_COIL_TBL*			mtbl_pCoil;									// コイル情報 データ
};
