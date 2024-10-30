// *********************************************************************************
//	判定PC→統括から受信クラス (サーバー機能のみ)
//	[Ver]
//		Ver.01    2007/03/20  vs2005 対応
//
//	[メモ]
//		主に、判定PC→統括への受信に使用
// *********************************************************************************

#pragma once


// 半固定
#include "..\..\Include\KizuCommon.h"									// 疵検インクルード
#include "..\KizuLib\KizuLib.h"											// 疵検ライブラリ
#include "..\KizuLib\Class\KizuMacro.h"									// マクロ集
#include "..\KizuLibMFC\Class\LogFileManager.h"							// ログ管理クラス

#include "..\KizuSOCK3\SockBase.h"										// ソケット操作クラス

using namespace KizuLib;
using namespace KizuSock3;

class KizuHtRecv 
{
public:
	// 本クラスのソケット状態ステータス
	enum SOCK_STATE {
		STATE_NONE = 0,													// 初期状態 (切断中)
		STATE_IDLE,														// 準備完了 (接続中)
		STATE_HEAD,														// ヘッダ部 受信中
		STATE_DATA														// データ部 受信中
	};

protected :
	// ローカル定数
	static const int PARAM_SOCK_MAX				= 16;					// 最大接続人数 (必ずKS_MAXCLIENTS以下である事)
	static const int PARAM_SOCK_TIMEOUT			= 10000;				// ソケット タイムアウト
	static const int PARAM_SOCK_RETRY_TIME		= 2000;					// ソケット リトライ周期
	static const int PARAM_SOCK_RECV_BUFSIZE	= (4*1024*1024);		// LANボードの受信バッファサイズ (4M) ←同期の送受信しか関係ないとの事
	
	// Ver.1.3.0.0 コンストラクタで引数渡しにした
	//static const int PARAM_SOCK_RECV_DATASIZE	= (32*1024*1024);		// 受信バッファサイズ (32M)  受信データサイズで毎回作り直すか検討の余地有り！！→コストの無駄だから必要無しとの事。by今野さん
	const int PARAM_SOCK_RECV_DATASIZE;									// 受信バッファサイズ (ｺﾝｽﾄﾗｸﾀでsize決定) (デフォルト32M)

//=========================================================================
// ID				= 物理回線ID (クライアントが繋がった順) (0オリジン)
// PCID				= 判定機能ID (機能順) (通常1オリジン)
// RECVID			= 論理回線ID (受信ヘッダ内のsubinfで並び替えた順) (0オリジン)
// クライアントID	= ksThreadのメインスレッドのポインタ
//=========================================================================

public:
	//// メソッド
	KizuHtRecv(const char *cSession, int torb, int accept_num, int recv_num, int recv_max_size = 32*1024*1024);
	virtual ~KizuHtRecv();

	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ログファイルセット
	void SetSockTID(DWORD tid) { my_nThSock = tid; }					// 判定受信スレッドへの送り先

	int GetPcidToId(int pcid)	// pcid(1オリジン) からidに変換
		{
			for(int ii=0; ii<my_nRecvNum; ii++) {
				if(my_PC[ii] == pcid ) return ii;
			}
			return -1;
		}

	void		GetEvDetectWaitStart()	  {SetEvent(my_evDetectWaitStart);}	// 検査開始待ち要求シグナル
	void		SetEvDetectWaitStart()	  {SetEvent(my_evDetectWaitStart);}	// 検査開始待ち要求シグナル
	void		SetEvDetectRun()		  {SetEvent(my_evDetectRun);}	// 検査開始要求シグナル
	void		SetEvDetectStop()		  {SetEvent(my_evDetectStop);}	// 検査停止要求シグナル
	bool		AllConnect();											// 全判定正常?

	SOCK_STATE  GetSockState(int id)const {return my_emSockState[id];};	// 現在のステータス
	CString		GetSockStateName(int id)	const;						// 現在のステータス名称
	bool		IsConnect(int pcid) const		{						// PCID順の接続状態を取得
					for(int ii=0; ii<my_nAcceptNum; ii++) {
						if( pcid == my_PC[ii] ) { 
							if(STATE_NONE != my_emSockState[ii]) return true;
							break;
						}
					}
					return false;
				}

	// ステータス制御
	void SetCliantName(int id, char const* cVal);						// クライアント名称をセット (これで判定PCからのイニシャル伝文を判断してどの判定機能と繋がったか判断する)

	// 外部へ状態通知
	HANDLE	GetEvConnectAll()   const {return my_evConnectAll; };		// 全クライアント接続完了時
	HANDLE	GetEvErrToClose()   const {return my_evErrToClose; };		// 異常発生により クライアント切断時


protected :

	//// メンバー関数
	// ステータス制御


	// シーケンス制御
	virtual BOOL RecvAllHeader();										// すべてのヘッダー受信を開始
	virtual int  AnsHeader(int id);										// 業務伝文 ヘッダー部受信完了
	virtual BOOL AnsData(int id);										// 業務伝文 受信完了チェック
	virtual BOOL AnsInitializeSocket(int id);							// イニシャル伝文 受信完了
	virtual void DefectStart();											// 探傷開始処理
	virtual void DefectStop();											// 探傷停止処理
	virtual BOOL CheckFrameNo(DWORD framenum, int id, int recvid);		// フレームNoの連番チェック
	virtual BOOL CheckAllReadEnd();										// 全判定PCから受信完了

	// TPC/IP 通信制御
	void Reset();														// リセット
	BOOL Listen(bool bLogDsp);											// Listen
//	int  Accept(MSG* msg);												// Accept
	int  Accept();														// Accept (シグナル通知用)
	void CloseAll(bool bErr=false);										// 全クライアント切断 & 異常通知
	void ErrCall();														// 異常通知のみ (回線切断はしない)
	BOOL RecvHeaderInitialize(int id);									// イニシャル伝文ヘッダー読込開始
	virtual BOOL RecvHeaderDefect(int id);								// 指定のヘッダー受信を開始
	BOOL RecvDataDefect(int id, int recvid);							// 業務伝文のデータ部受信を開始

	// 各インスタンス
protected:
	LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス	
	SockBase*				mcls_pSock;									// ソケット操作クラスインスタンスンス	

	//// 同期シグナル
	HANDLE					my_evConnectAll;							// 全クライアント接続完了時
	HANDLE					my_evErrToClose;							// 異常発生により クライアント切断時
	HANDLE					my_evDetectWaitStart;						// 検査開始待ち要求
	HANDLE					my_evDetectRun;								// 検査開始要求
	HANDLE					my_evDetectStop;							// 検査停止要求
	HANDLE					my_evAccept;								// 各判定PCよりAccept時


	//// 同期シグナル (個々のソケット用)
	HANDLE					my_evDisConnect[PARAM_SOCK_MAX];			// 切断イベント認識(ID)
	HANDLE					my_evRecvTimeOut[PARAM_SOCK_MAX];			// 受信タイムアウトイベント認識(ID)
	HANDLE					my_evRecv[PARAM_SOCK_MAX];					// 受信イベント認識(ID)


	// 受信用
	BOOL					my_bRun;									// 探傷中TRUE
	SOCK_STATE				my_emSockState[PARAM_SOCK_MAX];				// ソケット状態(ID)
	long					my_PC[PARAM_SOCK_MAX];						// pc_id(ID)   // 繋がった順番の配列。判定2が1番につながったら[0]に2をセットする (重要)
	char					my_cClientName[PARAM_SOCK_MAX][SIZE_NAME_PC];	// クライアントのPC名(PCID)
	DWORD					my_nFrameNo[PARAM_SOCK_MAX];				// フレームNo(RECVID) (処理完了したフレームNo)
	bool					my_bDataReady[PARAM_SOCK_MAX];				// データ到着したか(RECVID)
	SOCK_F_HEAD				my_cRecvWkHead[PARAM_SOCK_MAX];				// 並び替え前の受信ヘッダー(内部ワーク用) (ID)
	SOCK_F_HEAD				my_cRecvHead[PARAM_SOCK_MAX];				// 受信ヘッダー(RECVID)
	BYTE*					my_cRecvData[PARAM_SOCK_MAX];				// 受信データ(RECVID)

	long					my_nRecvingDataSize[PARAM_SOCK_MAX];		// 現在のデータ部のみの受信バイト数 (分割送信対応対策) (ID)
	int						my_nRecvMaxCnt[PARAM_SOCK_MAX];				// クライアントから1フレームで何回送られてくるか(ID)
	int						my_nRecvNowCnt[PARAM_SOCK_MAX];				// クライアントから1フレームで何回送られてきたか(ID)


	// その他もろもろ
	DWORD					my_nThSock;									// 判定受信スレッドへの送り先
	int						my_nAcceptNum;								// クライアント数 (物理的な数)
	int						my_nRecvNum;								// フレーム単位での受信数 (論理的な数)
	CString					my_cSession;								// SockBaseServerの接続Session
	int						my_nTorB;									// 表示識別フラグ(0:表, 1:裏)


	// 調査用
public:
	void SetLogMgrD(LogFileManager* pLog) { mcls_pLogD = pLog; }		// 調査用ログファイルセット
protected:
	LogFileManager*			mcls_pLogD;									// 調査用LogFileManagerスレッドインスタ

};
