#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLib\Class\PipeManager.h"					// PIPEクラス

// EPCインクルード
#include "..\..\Include\EPC\NCL2DETECT_if.h"							// I/F定義

// ローカル
#include "MainConst.h"													// 自クラス定数ヘッダー

class DataRecver : public ThreadManager
{
//// 公開定数
public :
	//------------------------------------------
	// 本クラスのエラー
	//------------------------------------------
	enum EM_PIPE_ERR {
		CPIPE_NON = 0,							// 初期状態
		CPIPE_OPEN_ERR,							// オープンで失敗			
		CPIPE_READ_ERR,							// 読込みで失敗
		CPIPE_ERR_SMEM,							// 共有メモリオープン失敗
		CPIPE_DISCONNECT_ERR					// パイプが勝手に切断された
	};

//// メンバー定数
private :
	//------------------------------------------
	// 固定定数
	//------------------------------------------
	static const int PIPE_ANSER_TIMEOUT				= 3000;				// パイプの応答タイムアウト時間
	static const int PIPE_RECVBUF_SIZE				= 256;				// パイプの最大受信サイズ
	static const int PIPE_OPEN_INTERVAL				= 1000;				// パイプオープンチェック待ち時間
	static const int PIPE_OPEN_RETRY				= 5;				// パイプオープンチェック回数


//// 公開関数
public:
	DataRecver(LPCSTR cPipeName, LPCSTR cSmemName);
	virtual ~DataRecver(void);


	//// 外部アクセス
	void		SendBack(DWORD ofs);									// オフセット領域の返却関数
	bool		IsConnect()		   const { return m_bConnect;}			// 接続状態
	bool		IsFirstInfoExists();									// 共有メモリ内のカメラ情報有無
	void		HelcyStat(ENG_STAT *st);								// 共有メモリ内のカメラ情報取得
	// Get
	BYTE*		GetMapPointer()	   const { return mtbl_pMap; }			// 共有メモリの先頭アドレス

	// Set
	void		SetEvPipeOpen()	   { SetEvent(m_evOpenPipe); }			// PIPEの接続要求シグナル
	void		SetEvPipeClose()   { SetEvent(m_evClosePipe); }			// PIPEの切断要求シグナル
	void		SetRunExit()	   { m_bRunExit = true; }				// 終了処理中フラグセット
	void		SetPipeName(LPCSTR pipe_name) {m_cPipeName=pipe_name;}	// PIPE名称変更 
	void		SetSmemName(LPCSTR smem_name) {m_cSmemName=smem_name;}	// 共有メモリ名称変更 



//// 公開変数
public:
	// 受け渡しキュー
	ThreadQueue<BYTE>			gque_Deli;								// 共有メモリ制御パイプ送信データ受け渡し
	ThreadQueue<CPIPE_ERROR>	gque_CPipeErr;							// 受け渡しコマンドパイプエラー用キュー


//// メンバー関数
private:		
	// スレッド動作定義
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	

//// メンバー変数
private:


	//// 各インスタンス
	PipeManager*			mcls_pPipe;									// パイプ管理クラス

	//// 同期制御
	CRITICAL_SECTION		m_csLock;									// クリティカルセクション


	// シグナル (PIPE操作)
	HANDLE					m_evOpenPipe;								// PIPEの接続用イベント
	HANDLE					m_evClosePipe;								// PIPEの切断用イベント


	// PIPE関連
	bool					m_bConnect;									// 接続状態
	CString					m_cPipeName;								// 接続するPIPEパス
	BYTE					m_cRecvBuf[PIPE_RECVBUF_SIZE];				// 受信バッファ
	OVERLAPPED				m_ovlSend;									// 非同期送信用のオーバーラップ構造体
	OVERLAPPED				m_ovlRecv;									// 受信用非同期IO

	// 共有メモリ関連
	CString					m_cSmemName;								// 共有メモリ名称
	HANDLE					m_hMap;										// フレーム情報受け渡し用共有メモリ ハンドル
	BYTE*					mtbl_pMap;									// フレーム情報受け渡し用共有メモリ データ

	// その他もろもろ
	bool					m_bRunExit;									// 終了処理中
	bool					m_bFirstInfoExists;							// 共有メモリ内のカメラ情報有無

};
