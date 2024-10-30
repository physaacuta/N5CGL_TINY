#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLib\Class\KizuPerformanceCounter.h"			// タイマー


// 全長がらみ
#include "..\..\Library\DefectLib\SockOneToAnySend.h"					// 回転寿司形式の送信クラス
#include "..\..\Include\OutIF_HT_RAW.h"									// 全長定義

// 各ワーカースレッド
#include "ParamManager.h"


class CullManager :	public ThreadManager
{
public:
	static const int MC_TIME_RPI_S = 4;			// 全長処理PC関係
	static const int MC_TIME_RPI_E = 5;


	//struct DELI {
	__declspec(align(16)) struct DELI {			// SSE使用のため、アラインを16Byte単位とする
		int					nFrameSkip;			// 0:以外  NG
		int					nCamId;				// カメラセット 
		DWORD				nFrameNo;			// 送信連番 (0オリジン)
		float				dEdgePos[2];		// エッジ位置（カメラ座標系)mm単位

		DWORD				nWidth;				// フレーム画像X(pixel)
		DWORD				nHeight;			// フレーム画像Y(pixel)
		DWORD				nStride;			// フレーム画像X(Byte)
		DWORD				nChannel;			// 画像数

		DWORD				nImgSize[MAX_IMAGE_CNT];		// 画像サイズ	※ 0 は画像無し。

		DWORD				nYobi[3];			// アライン調整用予備

		BYTE				pImg[1];			// RAW画像配列。未セット色の場合は、そのまま前詰め			
	};


public:
	ThreadQueue<DELI>		gque_Deli;			// 受け渡しキュー

	// 時間
	int				g_nTime_Rpi[2];			// 間引フレーム全長


//// 公開関数
public:
	CullManager(void);
	virtual ~CullManager(void);

	void SetParamMgr(ParamManager* pCls)	   { mcls_pPrmMgr = pCls;}
	void SetLogMgrCull(LogFileManager* pCls)	{ mcls_pLogCull = pCls; }		// 全長処理関連ログ出力クラス


	// 全長
	void SetRpiToSend(SockOneToAnySend* pCls) { mcls_pSockRpi = pCls; }
	void SetQueSockRpi(ThreadLoopQueue<HT_RPI_FDATA>* pQue) { mque_pSockRpi = pQue; }


//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	
	void Exec(DELI* pDeli);												// 処理


	// 完全個別
	bool ResizeRaw(BYTE* const pSrcRaw, long nSrcWidth, long nSrcHeight, long nCullX, long nCullY, BYTE* pDstRaw, int mode);	// RAW画像 間引き
	bool ResizeRaw(BYTE* const pSrcRaw, long nSrcWidth, long nSrcHeight, BYTE* pDstRaw, long nDstWidth, long nDstHeight, int mode);

	bool ResizeRaw(BYTE* const pSrcRaw, int nSrcWidth, int nSrcHeight, int nSrcRoiX, int nSrcRoiY, int nSrcRoiW, int nSrcRoiH,
		int nDstWidth, int nDstHeight, double dXF, double dYF, BYTE* pDstRaw, int mode);


//// メンバー変数
private:
	//// 各インスタンス
	ParamManager*			mcls_pPrmMgr;								// パラメータクラス
	LogFileManager*			mcls_pLogCull;								// 全長処理関連ログ出力クラス

	KizuPerformanceCounter	mcls_Time;									// 時間

//// 全長
	SockOneToAnySend*		mcls_pSockRpi;								// 判定⇒全長処理間通信 クラス
	ThreadLoopQueue<HT_RPI_FDATA>*	mque_pSockRpi;						// フレーム画像受け渡しキュー(回転寿司形式)	[LIB⇔AP間] ※ キューサイズをAPから指定したいため、ポインタにしておく


};
