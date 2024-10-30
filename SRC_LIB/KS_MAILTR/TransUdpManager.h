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

#include "MainConst.h"


class TransUdpManager :	public ThreadManager
{

//// 当クラス内切替スイッチ
#define		ENABLE_DEBUG_MAILTR			// 定義時、デバッグ用ログ出力(本番時コメント)

//// 当クラス内専用の定数
private:


//// メンバー構造体
private:


//// 公開関数
public:
	TransUdpManager(void);
	virtual ~TransUdpManager(void);

	void Send_KsMailtr_MailTrans(LPCSTR SendSlotName, LPCSTR SendPcName, LPCVOID pDate, DWORD len);		// メールスロット転送受信からメール送信

	UdpMail*		GetRecvUdp() { return &m_RecvUdp; }					// UDP受信情報取得 [←Dlg]
	void			ClearRecvUdp() { memset(&m_RecvUdp, 0x00, sizeof(m_RecvUdp)); };	// UDP受信情報初期化 [←Dlg]

//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生

	void OpenUdpPort();													// UDPポート オープン
	void CloseUdpPort();												// UDPポート クローズ


	
//// メンバー変数
private:

	SOCKET				m_udp;											// UDP受信ソケット
	HANDLE				m_hEventRecv;									// 受信完了ハンドル

	UdpMail				m_RecvUdp;										// UDP受信情報

};
