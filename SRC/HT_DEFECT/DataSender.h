#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLib\Class\PipeManager.h"					// PIPEクラス
#include "..\..\Library\KizuLib\Class\KizuPerformanceCounter.h"			// タイマー

#include "..\..\Library\DefectLib\KizuToSend.h"							// 判定⇒統括間通信 クラス
#ifdef JPEGLIB_IPP
#include "..\..\Library\DefectLib\KizuJpegManager.h"					// Bitmap⇔Jpeg変換クラス
#else
#include "..\..\Library\DefectLib\KizuLjtJpegManager.h"					// Bitmap⇔Jpeg変換クラス
#endif
#include "..\..\Library\KizuMMX\MemCopy.h"								// メモリコピークラス
//#include "..\..\Library\KizuMMX\asm_x64\MemCopy_x64.h"								// メモリコピークラス

// 疵検 インクルード
#include "..\..\include\ToHtCommon.h"									// 統括判定用の定義ヘッダ

// EPCインクルード
//#include "..\..\Include\EPC\NCsmemData.h"								// 共有メモリ上のデータ構造
#include "..\..\Include\EPC\NCL2DETECT_if.h"							// I/F定義

// 各ワーカースレッド
#include "StatusManager.h"
#include "DataRecver.h"
#include "ParamManager.h"

// 全長がらみ
#include "CullManager.h"

using namespace KizuLib;

class DataSender : public ThreadManager
{

public:
	static const int MC_TIME_JPEG_S = 2;		// JPEG圧縮 (切り出し画像)
	static const int MC_TIME_JPEG_E = 3;

	static const int MC_TIME_RPI_S = 4;			// 全長処理PC関係
	static const int MC_TIME_RPI_E = 5;

	static const int MC_TIME_JF_S = 6;			// JPEG圧縮 (フレーム画像)
	static const int MC_TIME_JF_E = 7;

	static const int MC_TIME_ALL_S = 8;			// フレーム受信時処理
	static const int MC_TIME_ALL_E = 9;

//// 公開関数
public:
	DataSender(void);
	virtual ~DataSender(void);

	void SetParamMgr(ParamManager* pCls)	   { mcls_pPrmMgr = pCls;}
	void SetStatusMgr(StatusManager* pCls)	   { mcls_pStatus = pCls;}
	void SetDataRecver(DataRecver* pCls)	   { mcls_pData = pCls;}
	void SetKizuToSend(KizuToSend** pCls)	   { mcls_pSock = pCls;}

	// 全長
	void SetCullMgr(CullManager** pCls)		   { mcls_pCull = pCls; }


	void SetJpegQuality(int n)		{ m_nJpegQuality = n; if(0==n) {LOG(em_MSG), "[%s] BMP画像送信", my_sThreadName);} else {LOG(em_INF), "[%s] JPEG画像送信(品質=%d)", my_sThreadName, n);} }
	void SetCamPair(int n)			{ m_nCamPair = n;}
	void SetKindHt(EM_DIV_HANTEI n)	{ m_emKindHt = n;}

	void StartHls();
	void StopHls();

	void SetFrameSend(bool bFrameSend = true)	{ m_bFrameSend = bFrameSend; }
	bool IsFrameSend()							{ return m_bFrameSend; }

	// その他
	void			ClearFrameNo();												// FrameNo関係初期化
	int				GetDspFrameNo()		const { return m_nDspFrameNo;}
	int				GetDspRecvFrameNo()	const { return m_nDspRecvFrameNo;}
	int				GetJpegQuality()	const { return m_nJpegQuality;}

	// 時間
	int				GetTimeIntval() { return (int)mcls_TimeCycle.GetCycleTime(); }
	int				GetTimeAllNow() { return (int)mcls_Time.GetAveTime(); }
	int				GetTimeAllMax() { return (int)mcls_Time.GetMaxTime(); }

	int				g_nTime_All[2];			// フレーム処理	[ms](現,Max)
	int				g_nTime_Rpi[2];			// 全長画像送信依頼	[ms](現,Max)
	int				g_nTime_To[2];			// 切出画像	[ms](現,Max)
	int				g_nTime_ToFrame[2];		// Jpegフレーム


//// メンバー関数
private:		
	// スレッド動作定義
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	


	// メイン処理
	void NewDataExec(BYTE* pNewData);									// データパイプより 1フレーム情報を処理
	void NewDataSendBack(int idx);										// NewDataExec で Newした送信データの開放 および 共有メモリのオフセット値開放
	void SendHls();														// 定周期情報

	// Jpeg変換
	//int CreateJpegData(HT_TO_HEAD* pData, nc_smem_pipe_data* pSmem, HANDLE* pSendAdder);	// 判定管理でJPEG変換
	//int CreateJpegData(nc_smem_frame_header* pSmem, BYTE* pImgAddr);		// 判定管理でJPEG変換
	int CreateJpegData(NCL_SMEM_FRAME_HEADER* pSmem, BYTE* pImgAddr, int freelen);		// 判定管理でJPEG変換 (切り出し画像)
	int CreateJpegFrame(NCL_SMEM_FRAME_HEADER* pSmem, BYTE* pImgAddr);		// 判定管理でJPEG変換 (フレーム画像)


//// メンバー変数
private:

	//// 各インスタンス
	ParamManager*			mcls_pPrmMgr;								// パラメータクラス
	StatusManager*			mcls_pStatus;								// ステータス制御クラス
	DataRecver*				mcls_pData;									// データ受信スレッド
	KizuToSend**			mcls_pSock;									// 判定⇒統括間通信 クラス

	KizuPerformanceCounter	mcls_Time;									// 時間
	KizuPerformanceCounter	mcls_TimeCycle;								// 時間

	LogFileManager*			mcls_pLogSock;								// ソケット用ログクラス


	// シグナル
	HANDLE					m_evTimeHls;								// ヘルシー定周期出力


	// その他もろもろ
	DWORD					m_nFrameNo[MAX_HT1_CAMSET];					// 統括へ送信済みフレームNo [カメラセット順]
	DWORD					m_nRecvFrameNo[MAX_HT1_CAMSET];				// 画像処理タスクのデータパイプから受信したフレームNo [カメラセット順]
	
	DWORD					m_nDspFrameNo;
	DWORD					m_nDspRecvFrameNo;

	int						m_nJpegQuality;								// Jpeg圧縮品質 (0:BMPのまま 1～:Jpeg変換)

	int			 			m_nCamPair;									// 現在のカメラペア 1～ (0:バックアップ=割り当て無し)
	EM_DIV_HANTEI			m_emKindHt;									// 判定機能時:1 周期機能時:2
	bool					m_bSendHls;									// 統括ヘルシー送信開始

//// 全長
	CullManager**			mcls_pCull;									// 間引きクラス

#ifdef JPEGLIB_LJT
	KizuLjtJpegManager		mcls_KizuJpegManager;						// Jpeg画像生成クラス
#endif

	bool					m_bFrameSend;								// フレーム送信許可
	
};
