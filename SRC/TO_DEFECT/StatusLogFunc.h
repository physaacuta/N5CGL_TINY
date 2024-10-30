//=====================================================================
// ステータスログを登録するクラス
//	【全ライン】
//		・基本的に完全流用可能なように作る
//=====================================================================
#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\FuncBase.h"					// モジュールの基底クラス
#include "..\..\Library\KizuLibMFC\Class\DivNameManager.h"				// DIV定数 名称取得クラス

// 半固定
#include "..\..\Include\LineCommon.h"									// 疵検インクルード

// ローカルのクラス
#include "MainConst.h"

using namespace KizuLib;
class StatusLogFunc : public FuncBase
{
	//=========================================
	// ステータス状態ログ
public:
	struct TYP_STATUS_LOG {
		struct CHECK_DATA {
			EM_DIV_KADOU			emKadou;						// 稼動状態
			EM_DIV_KENSA			emKensa[NUM_MEN];				// 検査状態
			EM_DIV_KIKI				emKiki[NUM_MEN];				// 機器状態
			char					cMsg[256];						// ガイダンス
		} Data;
		// 拡張分
		char					cKizukenNo[SIZE_KIZUKEN_NO];			// 疵検管理No
		char					cCoilNo[32];							// コイルNo
	};	

public:
	StatusLogFunc(void);
	virtual ~StatusLogFunc(void);

	void SetQueAddSQL(ThreadQueue<DELIVERY_DB>* pQue) { mque_pAddSQLData=pQue; }	// DB登録用のキュー// 


	void Alloc();												// 初期設定
	void CheckStatusLog(TYP_STATUS_LOG* pTyp);					// ステータス情報チェック

private:
	void InitAutoNo();											// AutoNoに必要なデータを初期セットする
	void RefrashAutoNo(int nDay);								// 日付をチェックして、日が変われば、オートNoをリセットする。


private:
	//// 受け渡しキュー
	ThreadQueue<DELIVERY_DB>*		mque_pAddSQLData;			// DBクラス 受け渡しデータ用キュー(ﾎﾝﾄは<DBManager::DeliveryDB> だが コンパイルが通らんのでDELIVERY_DBに変更)


	//// 処理
	TYP_STATUS_LOG			m_oldStatusLog;					// 前回状態
	

	// 主キー関連
	//  ・主キー１: 登録日付
	//  ・主キー２: m_nAutoNo*10 + m_nODBCID
	int						m_nDay;										// 現在日
	int						m_nAutoNo;									// 主キーの一つとなる連番
																			//・基本的には、日が変わるタイミングでリセット(1)する
																			//・立ち上げたタイミングでは、最新日付のAutoNoの値とする
	int						m_nOdbcID;									// ODBCの設定位置

};
