// *********************************************************************************
//	前工程メイン管理クラス
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
#include "..\..\Library\KizuODBC\OdbcBase.h"							// DB操作クラス
#include "..\..\include\ToTbl.h"										// コイル情報

// ローカルのクラス
#include "MainConst.h"
#include "FFSendManager.h"
#include "PreDefectManager.h"
#include "ParamManager.h"

using namespace KizuLib;
using namespace KizuODBC;

class PreProcManager : public ThreadManager
{
private :
	//=============================
	// 本クラス受け渡し用クラス
	//=============================
	//// メール情報受け渡しキュー
	struct DELIVERY_MAIL {
		ENUM_MAEDATOPERATION	emNo;									// イベントNo
		COMMON_QUE				que;									// 付加情報
	};



//// 公開関数
public:
	PreProcManager(void);
	~PreProcManager(void);

	void Alloc();																		// 関数初期化
	
	// 外部アクセス用
	ThreadQueue<COMMON_QUE>	gque_Deli;													// 受け渡しキュー

	// 外部アクセス
	void SetFFSendManager(FFSendManager* pCls) {mcls_pFFSend = pCls;}					// FF送信管理クラスセット
	void SetParamManager(ParamManager* pCls) {mcls_pParam = pCls;}						// パラメータ管理クラスセット
	void SetPreDefectManager(PreDefectManager* pCls) {mcls_pPreDefect = pCls;}			// 前工程編集クラスセット
	void SetPreDBManager(PreDBManager* pCls) {mcls_pPreDefect->SetPreDBManager(pCls);}	// 前工程実績情報DB登録管理クラスセット


//// 公開変数
public:
	// 各ワーカースレッド



//// メンバー関数
private:		
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent( int nEventNo );									// スレッドイベント発生

	// シーケンス制御
	bool MakeMaeCoilInf( COMMON_QUE* pQue );																				// 前工程コイル情報メイン処理
	bool SelectCountMaeCoilInf( OdbcBase* clspDB, PRO_TYPE_KEY* keyinf, int* nDataCnt );									// 前工程コイル情報ＤＢ 件数取得
	bool SelectMaeCoilInf( OdbcBase* clspDB, PRO_TYPE_KEY* keyinf, char* cKizuKenNo, int* nFFFlg,int* nKariFlg );			// 前工程コイル情報取得
	bool InsertCoilOrder( OdbcBase* clspDB, char* cKizukenNo, int nTujun, PRO_TYPE_DATA_COILINF::NEXT_COIL*	pnext_coil );	// コイル通板順位テーブル登録
	bool InsertMaeCoilInf( OdbcBase* clspDB, char* cKizukenNo, int nTujun, PRO_TYPE_DATA_COILINF::NEXT_COIL* pnext_coil );					// 前工程コイル情報登録
	void MakeMaeDefectInf( COMMON_QUE* pQue );															// 前工程疵情報登録メイン処理
	bool SelectBufFfRMaeinf( OdbcBase* clspDB, char* cId, PRO_TYPE_KEY* keyinf,  FF_MAE_INF* typData );	// FF前工程疵情報 取得
	bool UpdateCoilInf( OdbcBase* clsInsDB,char* cKizukenNo, char* cId,  FF_MAE_INF* typData );			// 前工程コイル情報更新
	bool UpdateMaeCoilInf( OdbcBase* clspDB, char* cCoilKey, int nFrontLen );							// 入側フロント端板長さ 更新
	bool InsertMaeCoilLine( OdbcBase* clsInsDB, char* cKizuKenNO, int nOuDiv, int nProcKind, int nDspMode, FF_MAE_TYPE_INF* pKizuHead );	// 前工程ライン別コイル情報登録
	bool InsertMaeFfInf( OdbcBase* clsInsDB, char* cKizuKenNO, int nKizuNo, int nProcKind, int nOuDiv, int nDspMode, FF_MAE_TYPE_INF* pKizuHead, FF_MAE_TYPE_DATA* pKizuBody );	// 前工程FF疵情報テーブル登録
	bool InsertDefectInfMain( OdbcBase* clsInsDB, char* cKizuKenNO, FF_MAE_INF* typData );				// 前工程疵情報登録メイン処理
	

//// メンバー変数
	//// 各インスタンス
	FFSendManager*			mcls_pFFSend;								// FF送信管理
	PreDefectManager*       mcls_pPreDefect;							// 前工程編集クラス
	ParamManager*			mcls_pParam;								// パラメータ管理クラス

	//// 共有メモリ情報
	HANDLE					m_hCoil;									// コイル情報 ハンドル
	TO_COIL_TBL*			mtbl_pCoil;									// コイル情報 データ
};
