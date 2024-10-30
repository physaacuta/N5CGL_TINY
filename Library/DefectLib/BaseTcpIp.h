// *********************************************************************************
//	TCP/IPの送受信用基底クラス
//	[Ver]
//		Ver.01    2007/07/21  vs2005 対応
//
//	[メモ]
//		・主に、TCP/IPの外部I/F用通信に使用 (あくまで基本的な通信のみ実装している)
//		・非同期/同期 両方の送受信に対応させたよ。
//		・実際に、FFやプロコンと通信させたい場合は、FF用の継承したクラス等を作ること。 ←かなりいいのができたから、必要ないかも
// *********************************************************************************

#pragma once

// 半固定
#include "..\..\Include\KizuCommon.h"									// 疵検インクルード
#include "..\KizuLib\KizuLib.h"											// 疵検ライブラリ
#include "..\KizuLib\Class\KizuMacro.h"									// マクロ集
#include "..\KizuLibMFC\Class\LogFileManager.h"							// ログ管理クラス

// 疵検クラス インクルード
#include "..\KizuLib\Class\WorkThread.h"								// ワーカースレッドクラス
#include "..\KizuLib\Class\ThreadQueue.h"								// スレッドキュークラス

#include "..\KizuSOCK3\SockBase.h"										// ソケット操作クラス

using namespace KizuLib;
using namespace KizuSock3;

class BaseTcpIp : public WorkThread
{
public:
	//=============================
	// 定数
	//=============================
	//// 一般的なセミ部分のレスポンス (本クラス内では特に使用してない。使いたい人は、勝手に使ってね)
	// サブヘッダー [0]
	static const BYTE	gcTCPSEMI_HEAD_H_GYOUMU  = 0x60;				// 業務伝文 又は 応答伝文
	static const BYTE	gcTCPSEMI_HEAD_H_RESPONS = 0xE0;				// レスポンス伝文
	// サブヘッダー [1]
	static const BYTE	gcTCPSEMI_HEAD_L_OK		 = 0x00;				// 正常
	static const BYTE	gcTCPSEMI_HEAD_L_NGCODE  = 0x50;				// Code種別異常
	static const BYTE	gcTCPSEMI_HEAD_L_NGDEN   = 0x51;				// 伝文種別異常
	static const BYTE	gcTCPSEMI_HEAD_L_NGSIZE  = 0x52;				// データ長異常

	//// その他TCPIP用
	static const int	PARAM_SOCK_TIMEOUT			= 3000;					// 送受信開始してから終わるまでの時間

	//// 本クラスのソケット状態ステータス
	enum SOCK_STATE { 
		STATE_NONE = 0,													// 初期状態 (切断中)
		STATE_IDLE,									 					// 準備完了 (接続中)

		STATE_RSING,													// 送受信中
		STATE_RECVING,													// 受信中
		STATE_SENDING													// 送信中
	} ; //SockState;

	//// アンサー情報のステータス
	enum DELI_ANS_STATE {
		ANS_CONNECT = 0,												// 接続完了通知
		ANS_CLOSE,														// 切断完了通知
		ANS_RECVEND,													// 非同期受信完了通知
		ANS_SENDEND,													// 非同期送信完了通知
		ANS_ERR_CONNECT													// コネクト失敗通知
	} ; //DeliAnsState;

	//=============================
	// 本クラス受け渡し用クラス
	//=============================
	//// 基本設定情報
	struct DELIVERY_COMMON {
		int		nMyID;													// 自分のID   (誰か分るようにする為)
		char	cMyName[32];											// 自分の名称 (誰か分るようにする為)
		char	cSession[32];											// Iniファイルのセッション
		bool	bSorC;													// サーバー(true) or クライアント(false)
		bool	bRorS;													// 受信(true) or 送信(false) がメインのポートか
		int		nRetryTimer;											// 再接続要求の周期 [ms]
		int		nBufferSize;											// 送受信の最大バッファサイズ (必ず伝文よりも大きくする事)
	};

	//// 送受信情報
	struct DELIVERY_DATA {
		bool	bRorS;													// 受信(true) or 送信(false) か
		bool	bAnsQueing;												// 操作結果をキューに登録する場合 true。 クラス内で処理させる場合にのみfalse
		int		nSize;													// 送受信データの長さ
		int		nStartPoint;											// Data部の実際の操作する位置。（通常0固定。受信時に、ヘッダー部、データ部を分けて受信する場合等に、凄く有効。）
		BYTE	*Data;													// 送受信データのポインタ (開始時にNew。完了時にDeleteする事)
		int		nUserWk[4];												// 勝手に使用してください。(本クラスでは一切なにもしません)

		// 本クラスがセット ------->>>
		int		nAns;													// 処理状態
																			// 0:  正常に送受信出来た場合
																			// 1:  失敗 (タイムアウト等)
																			// -1: 前のインスタンスが実行中
																			// -2: 子スレッドがとまっている
																			// -3: 非同期操作タイムアウト
																			// -7: 同一非同期操作継続中
																			// -8: 非同期操作中に切断された
																			// -9: ソケットオープンしてない
	};

	//// アンサー情報
	struct DELIVERY_ANS {
		int					nMyID;										// 自分のID (誰か分るようにする為)
		DELI_ANS_STATE		state;										// アンサー情報のステータス
		DELIVERY_DATA*		Data;										// 情報 (送受信時のみ。それ以外はNULL)
	};


public:
	BaseTcpIp(DELIVERY_COMMON* del);
	virtual ~BaseTcpIp(void);

	void Start() { WorkThread::Start(MainThread); };					// スレッド開始
	void Stop()  { WorkThread::StopMSG(WM_USER_TH_END); };				// スレッド終了
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ログファイルセット
	void SetPostTID(DWORD tid) { this->my_PostTid=tid;}					// PostThreadMessageの送り先 (メインインスタンス)
	void SetErrConnectQueing(bool bFlg) {my_bIsErrConnectQueing=bFlg;};	// コネクト失敗時 受け渡しキューにキューイングするしない
	

	//// 非同期 操作用
	bool SetRunningData( DELIVERY_DATA* );								// 非同期 送受信データ受け渡し	
	int DeleteEndData(DELIVERY_DATA* pData);							// 非同期 送受信データの完了したのを自動開放 (送受信の結果情報を戻り値として返す)

	//// 同期 操作用
	int LockRecv(BYTE* data, int size, int timeout);					// 同期 受信
	int LockSend(BYTE* data, int size, int timeout);					// 同期 送信



	//// 外部アクセス
	void		UserClose() { SetEvent(my_evUserClose); };					// ユーザーからの強制切断指示

	// Get
	int			GetCloseKind()		const {return my_nCloseKind; };			// 終了区分を取得
										//	#define KIZUSOCK_CLOSEKIND_NON		0			// 終了した事が無いから不明
										//	#define KIZUSOCK_CLOSEKIND_SOCK		1			// ソケットが切断された (= 相手からの切断)
										//	#define KIZUSOCK_CLOSEKIND_PROG		2			// 異常検出した為、プログラムで切断
										//	#define KIZUSOCK_CLOSEKIND_THEAD	3			// スレッド終了
	SOCK_STATE  GetSockState()		const {return my_emSockState; };					// 現在のステータス
	CString		GetSockStateName()	const {return GetSockStateName(my_emSockState);};	// 現在のステータス名称
	CString		GetSockStateName(SOCK_STATE st)	const;									// 指定のステータス名称
	CString		GetAnsStateName(DELI_ANS_STATE st) const;					// アンサー名称
	const char* GetMyName()			const {return my_Del_Common.cMyName; }	// 自分の名前 設定


	// 受け渡しキュー
	ThreadQueue<DELIVERY_ANS>  gque_Anser;								// アンサーキュー (取得後 必ず開放する事)


protected :
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド

	// Sock関連
	BOOL SockNew(bool bLogDsp);											// インスタンス生成
	BOOL SockDelete(bool bLogDsp);										// インスタンス開放
	BOOL Listen(bool bLogDsp);											// Listen
	BOOL Connect(bool bLogDsp);											// 接続(ブロッキング)
	BOOL Accept();														// Accept(シグナル通知用) 
	BOOL Close(bool bLogDsp);											// 切断
	BOOL IsConnect() const { return (NULL == mcls_pSock || 0==mcls_pSock->GetActive() ) ? FALSE : TRUE; };		// ソケットが有効時TRUE

	// 共通操作
	bool SetAnserQue(DELI_ANS_STATE state, DELIVERY_DATA* data=NULL);	// 外部通知用にキューイング

	// 非同期操作
	void RunningStart();												// 非同期 送受信操作依頼
	virtual void CheckRecvEnd();										// 非同期 受信完了
	virtual void CheckSendEnd();										// 非同期 送信完了


	// 各インスタンス
	LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス
	SockBase*				mcls_pSock;									// ソケット操作クラスインスタンス

	// 同期シグナル
	HANDLE					my_evUserClose;								// ユーザーからの強制切断通知
	HANDLE					my_evRunningEnd;							// ユーザーからの操作状態完了したよ通知

	// 同期シグナル (ソケット関連)
	HANDLE					my_evReTryConnect;							// 再接続要求タイマイベント
	HANDLE					my_evAccept;								// Accept イベント
	HANDLE					my_evDisConnect;							// 切断 イベント
	HANDLE					my_evSendTimeout;							// 送信タイムアウト イベント
	HANDLE					my_evRecvEnd;								// 受信完了 イベント
	HANDLE					my_evSendEnd;								// 送信完了 イベント

	HANDLE					my_evLockRecvEnd;							// 同期 ブロッキング受信完了イベント
	HANDLE					my_evLockSendEnd;							// 同期 ブロッキング送信完了イベント

	// 受け渡しキュー
	ThreadQueue<DELIVERY_DATA> mque_Running;							// 送受信データ受け渡しキュー

	// タイマー関連
	UINT					my_tId_ReTry;								// 再接続要求タイマID

	// ソケット関連
	SOCK_STATE				my_emSockState;								// ステータス (外部ガイダンス用にしか使わないかなー)
	DELIVERY_COMMON			my_Del_Common;								// 基本情報
	DELIVERY_DATA*			my_pNowDataR;								// 現在の 非同期 受信中のデータ
	DELIVERY_DATA*			my_pNowDataS;								// 現在の 非同期 送信中のデータ

	// その他もろもろ
	int						my_nCloseKind;								// 終了状態(-1:本プログラム 0～:KizuSock3管理)
	bool					my_bIsErrConnectQueing;						// コネクト失敗時 受け渡しキューにキューイングするしない (true:する) デフォルトfalse 下位互換用
};
