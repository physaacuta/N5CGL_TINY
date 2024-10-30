//======================================================================================
//
// カメラリンクスイッチ用 シリアル通信速度変更クラス
//
//--------------------------------------------------------------------------------------
// 画像処理エンジンからのUDPを受けて、TO_LINKSWに速度変更通知を送信する。
// UDPは、カメラセット単位で送信されてくる前提
// （色差・複眼ともに1カメラセットあたり1回。複眼の場合は、1回の通知で正・乱のカメラを速度変更する）
//======================================================================================


#pragma once


#include <winsock2.h>


// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Include\EPC\NCL2DETECT_if.h"

#include "StatusManager.h"
#include "MainConst.h"

#include "ToOutMailSender.h"


class CamLinkSpeedManager :	public ThreadManager
{

//// 当クラス内切替スイッチ
#define		ENABLE_DEBUG_CAMLINK			// 定義時、デバッグ用ログ出力(本番時コメント)

//// 当クラス内専用の定数
private:

	static const int DEFAULT_BAUD_RATE = 9600;							// デフォルトのシリアル通信速度[bps]



//// メンバー構造体
private:

	// シリアル通信速度 変更要求用構造体(エンジンから、このデータセットでやってくる)
	struct TYP_SER_CHANGE_REQ{
	
		int PacketId;					// パケットID
		int CamId;						// カメラID(1オリジン)
		int nSpeed;						// シリアル通信速度[bps]
	};


//// 公開関数
public:
	CamLinkSpeedManager(void);
	virtual ~CamLinkSpeedManager(void);


	// 外部セットプロパティ
	void SetStatusMgr(StatusManager* p) { mcls_pStatus = p; }			// ステータス管理クラス[←MI]

	void InitCamLinkSpeed(int nBaudrate = DEFAULT_BAUD_RATE);			// シリアル速度を初期設定に戻す[←MI]




//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生

	void OpenUdpPort();													// UDPポート オープン
	void CloseUdpPort();												// UDPポート クローズ

	void GetIniSetting();												// Iniファイルから接続CHを取得して配列にセット

	int  SelectBoardID(const int nCamId, int* nBoardID);				// カメラIDから、フレームグラバーボードNoを特定


	
//// メンバー変数
private:

	// ワーカースレッド
	StatusManager*		mcls_pStatus;									// ステータス管理クラス

	// PCIDに紐付くカメラリンクスイッチの出力CH
	// [PCID][接続ボード] の二次元配列に、出力CHをセット(FPGAのID0からの連番　／　未接続時、0)
	int					m_nBoardID[NUM_ALL_HANTEI][NUM_CAMLINK_BOARD];

	SOCKET				m_udp;											// UDP受信ソケット
	HANDLE				m_hEventRecv;									// 受信完了ハンドル

};
