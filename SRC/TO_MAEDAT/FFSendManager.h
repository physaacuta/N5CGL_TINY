// *********************************************************************************
//	FF送信管理クラス
//	[Ver]
//		Ver.01    2012/09/06
//
//	[メモ]
//		
// *********************************************************************************

#pragma once


// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー

// ローカルのクラス
#include "MainConst.h"


using namespace KizuLib;
using namespace KizuODBC;

class FFSendManager : public ThreadManager
{


//// 公開関数
public:
	FFSendManager(void);
	~FFSendManager(void);

	void SetFFSend() {	SetEvent(m_evFFSend); }	;						// FFデータ送信要求


//// 公開変数
public:
	// 各ワーカースレッド



//// メンバー関数
private:	
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent( int nEventNo );									// スレッドイベント発生

	// シーケンス制御
	void FFSendMain();													// 前工程実績要求メイン処理
	void SendMailFFReq(char* cKeyinf, char* cMeiNo, char* cLine_Code, char* cCoilNo);	// 前工程実績要求

	static const int FFSEND_TIMER = 60*60*1000;							// 定周期処理タイマーインターバル [ms]

//// メンバー変数
private:
	// シグナル
	HANDLE m_evFFSend;													// FFデータ送信要求シグナル 
	HANDLE m_evTimeCycle;												// ヘルシー
};
