#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC操作クラス

// ローカルのクラス
#include "MainConst.h"

using namespace KizuLib;
using namespace KizuODBC;

class MaeManager :	public ThreadManager
{
//// 公開定数
public:

	enum em_EXEC_MODE
	{
		UPDATE_MAE_REQUEST = 0,		// 全前工程情報更新
		UPDATE_FRAG_ONLY,			// フラグのみ更新
		END
	};

	// 未確定管理Noを 管理Noに UpDate
	struct TYP_MAE_REQUEST
	{
		em_EXEC_MODE			emMode;									// モード
		char					cMeiNo[SIZE_MEI_NO+1];					// 変更対象の命令No
		char					cCoilNo[SIZE_COIL_NO + 1];					// 変更対象のコイルNo

		// 更新情報
		char					cKizukenNo[SIZE_KIZUKEN_NO];			// 処理後の管理No
		long					nNobiSPM;								// SPM伸び率 [10-2%]
		long					nCutF;									// 入側におけるフロント端板カットの実績長[mm]
	};


//// 公開関数
public:
	MaeManager(void);
	virtual ~MaeManager(void);


	//=========================================
	// 外部受け渡し
	void RecvCoilInf(COIL_INF const* pCoil);					//  コイル情報受信
	void RecvMaeWrite(char const* cMeiNo, char const* cCoilNo);	//  前工程疵情報書込み完了通知受信
	void RecvCoilResultWrite(COIL_INF const* pCoil);			//  コイル実績書込み完了通知受信

private:
	void AddQue_MaeRequest(COIL_INF const* pCoil, em_EXEC_MODE emMode);	// 仮管理No更新 依頼	//以前は公開関数であったが、メンバー化


//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	

	void Exec_MaeRequest(TYP_MAE_REQUEST const* p);						// 仮管理No更新
	void Exec_FragOnly(TYP_MAE_REQUEST const* p);						// フラグのみ更新


//// メンバー変数
private:

	//=========================================
	// 各インスタンス

	//// キュー
	ThreadQueue<TYP_MAE_REQUEST> mque_MaeRequest;						// 受け渡しデータ用キュー

	COIL_INF*					m_pCoilInf;								// 最後に受信したコイル情報

};
