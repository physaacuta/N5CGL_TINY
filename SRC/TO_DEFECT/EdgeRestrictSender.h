// *********************************************************************************
//	エッジ検出範囲指定 UDP送信クラス
//	[Ver]
//		Ver.01    2017/08/01  初版作成開始
//
// *********************************************************************************
#pragma once

#include <winsock2.h>														// UDP送信用

// 半固定
#include "..\..\Include\LineCommon.h"										// 疵検インクルード
#include "..\..\Include\ToHtCommon.h"										// EPCソフト ＆ 統括 ＆ 判定 機能 関連ヘッダー
#include "..\..\Include\EPC\NCL2DETECT_if.h"								// I/F定義

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"					// ワーカースレッドマネージャー

// ローカルのクラス
#include "MainConst.h"
#include "ParamManager.h"													// パラメータ管理
#include "EdgePosControl.h"													// エッジ検出整合性管理クラス

using namespace KizuLib;

class EdgeRestrictSender : public ThreadManager
{

////=========================================
//// プロパティ
public:
	//// ワークスレッド
	void SetParamMgr(ParamManager* pCls)		{ mcls_pParam = pCls; }					// パラメータ管理クラスセット
	void SetStatusMgr(StatusManager* pCls)		{ mcls_pStatus = pCls ;}				// ステータス情報管理クラス

////=========================================
//// 公開関数
public:
	// コンストラクタ・デストラクタ
	EdgeRestrictSender(void);
	virtual ~EdgeRestrictSender(void);

	// スレッドキュー
	ThreadQueue<NCL_EDGE_RESTRICT_DATA> gque_SendUdp;									// UDP送信データ

	// PFCソフトにコマンド送信(他クラスから起動)
	void SendEdgeRestrict(int nFrame, int* nUseLimit, float* dEdge_Left, float* dEdge_Right);	// 画像処理エンジンへエッジ位置情報送信

	NCL_EDGE_RESTRICT_DATA&	GetLastSendData() { return m_strLastSendData; };// 最終送信データ取得

////=========================================
//// メンバー関数
private:
	void	ThreadEvent(int nEventNo);										// スレッドイベント発生
	int		ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int		ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)

////=========================================
//// メンバー変数
private :

	//=========================================
	// 各インスタンス
	ParamManager*			mcls_pParam;									// パラメータ管理クラス
	StatusManager*			mcls_pStatus;									// ステータス情報管理クラス

	// エッジ検出範囲指定 UDP送信用
	SOCKET			m_SockHtSend;											// UDP送信ソケット
	sockaddr_in		m_AddrHtSend;											// 送信元アドレス

	NCL_EDGE_RESTRICT_DATA	m_strLastSendData;								// 最終送信データ
};
