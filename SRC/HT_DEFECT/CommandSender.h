#pragma once

// EPCインクルード
//#include "..\..\Include\EPC\NCcommand.h"								// 制御コマンドの定義
#include "..\..\Include\EPC\NCL2DETECT_if.h"							// I/F定義

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLib\Class\PipeManager.h"					// PIPEクラス

// ローカル
#include "MainConst.h"													// 自クラス定数ヘッダー

class CommandSender : public ThreadManager
{
//// 公開定数
public :
	//------------------------------------------
	// 本クラスのエラー
	//------------------------------------------
	enum EM_PIPE_ERR {
		CPIPE_NON = 0,							// 初期状態
		CPIPE_ERR_COMMAND,						// 画像処理タスクから非同期で重大異常が送られてきた

		CPIPE_OPEN_ERR,							// オープンで失敗			
		CPIPE_READ_ERR,							// 読込みで失敗
		CPIPE_DISCONNECT_ERR					// パイプが勝手に切断された
	};

//// メンバー定数
private :
	//------------------------------------------
	// 固定定数
	//------------------------------------------
	static const int PIPE_ANSER_TIMEOUT				= 18000;//8000;		// パイプの応答タイムアウト時間
	static const int PIPE_RECVBUF_SIZE				= 512;				// パイプの最大受信サイズ
	static const int PIPE_SENDBUF_SIZE				= 1024;				// パイプの最大送信サイズ
	static const int PIPE_OPEN_INTERVAL				= 1000;				// パイプオープンチェック待ち時間
	static const int PIPE_OPEN_RETRY				= 5;				// パイプオープンチェック回数



//// 公開関数
public:
	CommandSender(LPCSTR cPipeName);
	virtual ~CommandSender(void);


	//// 外部アクセス
	int SendCommand(BYTE *pSendData, DWORD nSendSize, void *pRecvRetc, int nRecvSize=sizeof(NCL_ANSWER), DWORD nWaitTime=PIPE_ANSER_TIMEOUT); // コマンド受付
	//int SendCommand(BYTE *pSendData, DWORD nSendSize, void *pRecvRetc, int nRecvSize=sizeof(int), DWORD nWaitTime=PIPE_ANSER_TIMEOUT); // コマンド受付
	void ReadPerData(int nOffset, void* pRecvRetc, int nRecvSize );

	bool		IsConnect()		   const { return m_bConnect; }			// 接続状態
	// Set
	void		SetEvPipeOpen()	   { SetEvent(m_evOpenPipe); }			// PIPEの接続要求シグナル
	void		SetEvPipeClose()   { SetEvent(m_evClosePipe); }			// PIPEの切断要求シグナル
	void		SetRunExit()	   { m_bRunExit = true; }				// 終了処理中フラグセット
	void		SetPipeName(LPCSTR pipe_name) {m_cPipeName=pipe_name;}	// PIPE名称変更 
	void		SetNgCam(bool bMode){ m_bNgCam = bMode;}

//// 公開変数
public : 
	// スレッドキュー
	ThreadQueue<CPIPE_ERROR>	gque_CPipeErr;							// 受け渡しコマンドパイプエラー用キュー


//// メンバー関数
private:		
	// スレッド動作定義
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	

//// メンバー変数
private:

	// 各インスタンス
	PipeManager*			mcls_pPipe;									// パイプクラス


	// シグナル (PIPE操作)
	HANDLE					m_evOpenPipe;								// PIPEの接続用イベント
	HANDLE					m_evClosePipe;								// PIPEの切断用イベント
	HANDLE					m_evSendStart;								// 受信開始イベント認識用
	HANDLE					m_evSendEnd;								// 送信完了インベト認識用


	// PIPE関連
	bool					m_bConnect;									// 接続状態
	CString					m_cPipeName;								// 接続するPIPEパス
	char					m_cRecvWk[PIPE_RECVBUF_SIZE];				// 受信バッファ
	char					m_cRecvBuf[PIPE_RECVBUF_SIZE];				// 受信バッファ
	char					m_cSendBuf[PIPE_SENDBUF_SIZE];				// 送信バッファ
	DWORD					m_nSendSize;								// 送信データ長
	OVERLAPPED				m_ovlRecv;									// 受信用非同期IO

	// その他もろもろ
	bool					m_bRunExit;									// 終了処理中
	bool					m_bNgCam;									// 異常カメラ担当 true

};
