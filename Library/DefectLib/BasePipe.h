// *********************************************************************************
//	名前付きパイプの送受信用基底クラス
//	[Ver]
//		Ver.01    2010/03/02  vs2005 対応
//
//	[メモ]
//		・同一コンピュータ同士のタスク間通信時は、高速
//			・\\\\.\\pipe\aaa		だと 25ns～50ns で通信可能
//			・\\\\tokatu\\			だと 180ns～280ns とかなり遅くなり (同一PCでも、コンピュータ名指定は普通くらいの速度になる)
// *********************************************************************************

#pragma once

// 半固定
#include "..\..\Include\KizuCommon.h"									// 疵検インクルード
#include "..\KizuLib\KizuLib.h"											// 疵検ライブラリ
#include "..\KizuLib\Class\KizuMacro.h"									// マクロ集
#include "..\KizuLibMFC\Class\LogFileManager.h"							// ログ管理クラス

#include "..\KizuLib\Class\PipeManager.h"								// 名前付きパイプクラス

using namespace KizuLib;

class BasePipe
{
//// 公開
public:

	//// アンサー情報のステータス
	enum DELI_ANS_STATE {
		ANS_ACCEPT = 0,													// アクセプト接続完了通知
		ANS_RECVEND,													// 非同期受信完了通知
		ANS_ERR_CLOSE,													// 切断通知
		ANS_ERR_CONNECT													// コネクト失敗通知
	};

	//// 送受信情報
	struct PIPE_BASE_HEAD {
		int		nSize;													// 送受信データの長さ(自分を含めた長さ)
		int		nKind;													// 伝文区分
		int		nId;													// 複数接続対応 (誰かわかるようにIDセット) 基本0オリジン
	};


	//// アンサー情報
	struct DELIVERY_ANS {
		DELI_ANS_STATE		state;										// アンサー情報のステータス
		PIPE_BASE_HEAD*		pData;										// 情報 (受信時のみ。それ以外はNULL)
	};



//// 公開
public:
	BasePipe(void);
	virtual ~BasePipe(void);

	// パラメータ
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; };			// ログファイルセット
	void SetIsServer(bool bMode)		{ my_bIsServer = bMode; }		// サーバーorクライアント 設定
	void SetPipeName(LPCSTR sName)		{ my_sPipeName = sName; }		// 名前付きパイプ名称


	//// パイプ操作
	bool	Connect();													// コネクト (接続されるまでブロックされる)
	bool	Listen();													// リスニング (ノンブロック)
	void	Accepting();												// アクセプト完了
	void	Close();													// クローズ

	bool	StartRecvHeader();											// ヘッダー部受信開始
	DWORD	EndRecvHeader();											// ヘッダー部受信完了時
	DWORD	GetRecvBody(BYTE* buf, DWORD len );							// データ部受信 (同期)

	bool	SendLock(BYTE* buf, DWORD len, int nMaxWait);				// データ送信 (同期)
	bool	SendUnLock(BYTE* buf, DWORD len);							// データ送信 (非同期)

	//// シグナル公開
	HANDLE	GetEvSendEnd() { return my_ovlSend.hEvent;}					// 送信完了時
	HANDLE	GetEvRecvEnd() { return my_ovlRecv.hEvent;}					// 受信完了時
	HANDLE	GetEvAcceptEnd() { return my_ovlAccept.hEvent;}				// アクセプト完了時


	//// Get
	bool	IsConnect() const			{ return my_nConnect==0?false:true;}
	int		GetConnectCnt() const		{ return my_nConnect;}
	CString GetPipeName() const			{ return my_sPipeName;}
	PIPE_BASE_HEAD&	GetRecvHeadBuf()	{ return my_cRecvHeadBuf;}		// ヘッダー部データにアクセス
	int GetError()						{ return mcls_pPipe->GetError(); }	// 最終エラー取得


protected :

	//// 各インスタンス
	LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス
	PipeManager*			mcls_pPipe;									// Pipeクラス


	// 非同期IO
	OVERLAPPED				my_ovlSend;									// 送信完了
	OVERLAPPED				my_ovlRecv;									// 受信完了
	OVERLAPPED				my_ovlAccept;								// アクセプト完了
	CRITICAL_SECTION		my_cs;										// ロック


	// 状態
	int						my_nConnect;								// 接続人数
	bool					my_bRecvHead;								// ヘッダー部分受信中時 true
	PIPE_BASE_HEAD			my_cRecvHeadBuf;							// ヘッダー受信時


	// その他もろもろ
	bool					my_bIsServer;								// Server時true
	CString					my_sPipeName;								// 名前付きパイプ名称
	CString					my_sThreadName;								// スレッド名称
};
