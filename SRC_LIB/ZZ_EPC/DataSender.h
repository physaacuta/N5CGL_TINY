#pragma once

// 半固定
#include "..\..\Include\LineCommon.h"									// 疵検インクルード
#include "..\..\Library\KizuLib\KizuLib.h"								// 疵検ライブラリ
#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// マクロ集
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// スタティックライブラリ集
#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ログ管理クラス
#include "..\..\Library\KizuLib\Class\KizuPerformanceCounter.h"			// タイマー

// 疵検クラス インクルード
#include "..\..\Library\KizuLib\Class\WorkThread.h"						// ワーカースレッドクラス
#include "..\..\include\ToHtCommon.h"									// 統括判定用の定義ヘッダ

// EPCインクルード
//#include "..\..\Include\EPC\NCsmemData.h"
//#include "..\..\Include\EPC\NCspeed.h"
//#include "..\..\Include\EPC\NCAttr.h"
#include "..\..\Include\EPC\NCL2DETECT_if.h"							// I/F定義
#include "..\..\Include\EPC\NCL2attr.h"

using namespace KizuLib;

class DataSender : public WorkThread
{
private:
	static const int IMG_SIZE_X = 512;
	static const int IMG_SIZE_Y = 512;
	//static const int IMG_SIZE_X = 768;
	//static const int IMG_SIZE_Y = 768;
	//static const int IMG_SIZE_X = 1024;
	//static const int IMG_SIZE_Y = 1024;
	
	static const int FILE_IMG_NUM = 1;		// 切り出し画像枚数
	static const int FILE_FRM_NUM = 1;		// フレーム画像枚数
#define		FILE_FORMAT_IMG	 "sample_%02d.bmp"		// 切り出し画像パス
#define		FILE_FORMAT_FRM	 "frm_%02d.bmp"			// フレーム画像パス



	static const int MAX_MEMSIZE = 1024*1024*400;
	//static const int MAX_MEMSIZE = 1024*1024*200;
	static const int MAX_DEFCNT	 = 10;
	// ざっくりな１回のサイズ
	static const int ONE_MEMSIZE = 2048
									+ (SIZE_FRAME_IMG_X * SIZE_FRAME_IMG_Y * 4)
									+ sizeof(float)*SIZE_FRAME_IMG_Y*2
									+ sizeof(double)*(NCL_nattr_common+NCL_nattr_angle*4) * MAX_DEFCNT
									+ IMG_SIZE_X*IMG_SIZE_Y*4  * MAX_DEFCNT;




	static const int MAX_NEXT_POS_CMD = 3;		// 最大コマンド蓄積数
	static const int FRM_SHRINK_X = 2;		// フレーム画像X縮小
	static const int FRM_SHRINK_Y = 2;		// フレーム画像Y縮小


public:
	struct EPC_POS {
		float			front_pos;		// 加算カウンタ
		float			tail_pos;		// 減算カウンタ
		float			shear_cut_pos;	// 加算カウンタ
		float			univ_cnt_up_pos[4];		// 汎用加算カウンタ
		float			univ_cnt_dn_pos[4];		// 汎用減算カウンタ
	};




public:
	DataSender(void);
	virtual ~DataSender(void);

	void Start() { WorkThread::Start(MainThread); };					// スレッド開始
	void Stop()  { WorkThread::Stop(); };								// スレッド終了
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; };			// ログファイルセット


	int SendData(LPVOID senddata, DWORD sendsize);
	int SetFrameData(DEBUG_SPEED_INFO* readInf);
	void InitFrameNo() { m_nFrameNo=0; }
	void SetCamset(int num, DWORD* set) { m_camsetnum = num; for (int ii = 0; ii < 8; ii++) m_camset[ii] = abs((int)set[ii]); }
	//void SetCamset(int num, DWORD* set ) {m_camsetnum=num; memcpy(m_camset, set, 4*8); }
	void GetCamset(int* num, DWORD* set) {*num = m_camsetnum; memcpy(set, m_camset, 4*8); }

	void SetBrightness(int* set)		 {memcpy(&m_nBrightness, set, sizeof(int)*4*3); }
	void SetCamGain(double* set)		 {memcpy(&m_dCamGain, set, sizeof(double)*3); }
	void SetEdge(int ii, double* set)		{if(ii>=0 && ii<2) m_dEdge[ii] = *set;}
	int  GetBrightness(int angle, int pos)	{ return m_nBrightness[angle][pos]; }
	double SetCamGain(int color)			{ return m_dCamGain[color]; }
	void SetRandomParam(bool set)			{ m_bRand = set; }

	void SetEdgeRestrict(NCL_EDGE_RESTRICT_DATA* readInf) { memcpy(&m_EdgeRestrict, readInf, sizeof(NCL_EDGE_RESTRICT_DATA)); }	// エッジ範囲制限

	void SetShrinkRatio(float value) { m_shrinkratio = value; }
	float GetShrinkRatio() { return m_shrinkratio; }

	BYTE*					gtbl_pFrame;								// フレーム情報受け渡し用共有メモリ データ

	//bool					g_bPutKizu;									// 疵出力
	//int						g_nPutKizu;									// 疵出力個数

	bool					g_bPutKizu[5];								// 疵出力
	int						g_nPutKizu[5];								// 疵出力個数
	int						g_nPutFrm[5];								// 疵出力フレーム間隔

	int						g_nPutType[5];								// 出力対象の疵種
	int						g_nPutGrade[5];								// 出力対象のグレード
	int						g_nPutCycle[5];								// 出力対象の周期欠陥

	bool					g_bPutEdge;									// エッジ変更出力
	bool					g_bPutSkip;									// 未検査区間出力

	int						m_nFrameNo;									// フレームNo

	int						g_nPosX;

private :
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド


	//// 各インスタンス
	LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス
	KizuPerformanceCounter	mcls_Time;									// 取得した時間(ストップウォッチ機能を使用)


	HANDLE					m_hFrame;									// フレーム情報受け渡し用共有メモリ ハンドル
	HANDLE					m_hPipe;									// パイプ本体
	OVERLAPPED				m_ovlSend;									// 受信用非同期IO

	int						m_camsetnum;								// カメラセット数
	int						m_camset[8];								// カメラセット

	int						m_nSmemBase;								// 共有メモリ 書込みポインタ

	int						m_nBrightness[4][3];						// 輝度情報[カメラ角度][ｴｯｼﾞ内平均明るさ/右オーバーラップ輝度/左オーバーラップ輝度]
	double					m_dCamGain[3];								// カメラゲイン現在値[R/G/B]
	double					m_dEdge[2];									// エッジ位置
	bool					m_bRand;									// 色々乱数をセットするか否かフラグ

	BYTE                    m_imgTemp[( IMG_SIZE_X * IMG_SIZE_Y * MAX_IMAGE_CNT ) * FILE_IMG_NUM ];
    BYTE                    m_frmTemp[( SIZE_FRAME_IMG_X * SIZE_FRAME_IMG_Y ) * FILE_FRM_NUM ];

	float					m_shrinkratio;								// 切り出し画像の縮小率
	bool					m_bSHMemSet;								// 共有メモリ設定フラグ

//// エンジン速度UDP無し対応
private:
	EPC_POS					m_NowPos;									// 現在距離
	NCL_CMD_SET_POSITION	m_NextPosCmd[MAX_NEXT_POS_CMD];				// NCL_SET_POSITION 要求。
	NCL_EDGE_RESTRICT_DATA	m_EdgeRestrict;								// NCL_EDGE_RESTRICT_DATA 受信

public:
	void	InitPosCmd() { 
		memset(m_NextPosCmd, 0x00, sizeof(m_NextPosCmd));
		memset( &m_NowPos, 0x00, sizeof(m_NowPos));
	}
	void	ClearPosCmd() { 
		memset(m_NextPosCmd, 0x00, sizeof(m_NextPosCmd));
	}
	void	SetNextPosCmd(NCL_CMD_SET_POSITION* p);
	void	SetFirstSHMem(bool bSHMemSet, FIRST_SHMEM_INFOv1* p);
	void	SetFirstSHMem(FIRST_SHMEM_INFOv1* p);
};
