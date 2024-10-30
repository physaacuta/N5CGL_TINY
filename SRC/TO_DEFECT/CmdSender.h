// *********************************************************************************
//	UDP送信クラス
//	[Ver]
//		Ver.01    2014/06/11  初版作成開始
//		Ver.02	  2015/01/16　4PL展開用にクラスを一般化
//
//	[メモ]
//   φω基本機能のみオンライン　～　4PL独自機能をオンラインまでの間、
//   調整用レコーダーにコイル情報を送信するクラス。
//   最終的には、この機能はOFFになる。
// *********************************************************************************
#pragma once

#include <winsock2.h>														// UDP送信用

// 半固定
#include "..\..\Include\LineCommon.h"										// 疵検インクルード
#include "..\..\Include\ToHtCommon.h"										// EPCソフト ＆ 統括 ＆ 判定 機能 関連ヘッダー
//#include "..\..\Include\EPC\NCcommand.h"									// 画像判定ソフトI/Fヘッダ
#include "..\..\Include\EPC\NCL2DETECT_if.h"								// I/F定義

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"					// ワーカースレッドマネージャー

// ローカルのクラス
#include "MainConst.h"

using namespace KizuLib;

class CmdSender : public ThreadManager
{



////=========================================
//// 公開構造体
public:
	struct	SEND_UDP{
		int		nTorB;														// 送信先のポート（0:表用、1:裏用、-1:両面）
		int		nPcId;														// 送信先のPCID(0オリジン)　-1時、ブロードキャスト
		int		nSize;														// 送信データのサイズ
		int		nCommand;													// 送信コマンド
		char	data[1024];													// 送信データ
	};

////=========================================
//// 公開関数
public:
	// コンストラクタ・デストラクタ
	CmdSender(void);
	virtual ~CmdSender(void);

	// スレッドキュー
	ThreadQueue<SEND_UDP> gque_SendUdp;										// UDP送信データ

	// PFCソフトにコマンド送信(他クラスから起動)
	void SendCommand_Start(int TorB = -1);									// レコーダーへ検査開始コマンド通知
	void SendCommand_Stop(int TorB = -1);									// レコーダーへ検査停止コマンド通知
	void SendCommand_Param(int TorB, int nscode);							// レコーダーへ検査表面状態変更コマンド通知
	void SendCommand_CoilInf(int TorB, TYPE_EPC_COIL typData);				// レコーダーへコイル情報を通知

////=========================================
//// メンバー関数
private:
	void	ThreadEvent(int nEventNo);										// スレッドイベント発生
	int		ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int		ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)

	// UDP通信関係
	void	HtUdp_Send(int TorB, char* cData, int nSize, int nCommand);		// ブロードキャスト送信 
	void	HtUdp_Send(int TorB, int nPcNo, char* cData, int nSize, int nCommand);	// ユニキャスト送信

	// 単品もの
	CString	GetCmdName(int nCommand);										// コマンド名称を取得


////=========================================
//// メンバー変数
private :

	// レコーダー UDP送信用
	SOCKET			m_SockHtSend;											// UDP送信ソケット
	sockaddr_in		m_AddrHtSend[NUM_MEN];									// 送信元アドレス(0:表用、1:裏用)

	//// その他
	char			m_cUniIp[NUM_SENDER][32];								// レコーダー ユニキャスト送信用のIPアドレス
};
