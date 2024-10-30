// *********************************************************************************
//	当タスクの基本定数
//	[Ver]
//		Ver.01    2007/08/01  vs2005 対応
//
//	[メモ]
//		
// *********************************************************************************

#pragma once


// 基本的にどこでも使用する可能性があるインクルード
#include "..\..\Include\KizuCommon.h"								// 疵検共通ヘッダー
#include "..\..\Library\KizuLibMFC\CLASS\ThreadLoopQueue.h"			// 回転寿司形式のスレッドキュー
#include "..\..\Include\OutIF_HT_RAW.h"								// 全長画像保存 外部I/F用ヘッダー
#include "ProcessQueImpl.h"

//=======================================
// 定数

//// 数 (上限値みたいなもの)
#define MAX_INS						4								// 基本取込処理を行う数
#define	MAX_THREAD_MULTI_P			32								// 処理スレッド最大数

#define MAX_FOLDER_SAVE_NUM			10								// ファイル保存フォルダ最大数
#define MAX_COLOR_NUM				4								// 1カメラから生成される最大画像数 (今のところ3画像 G,B,G-B)
#define MAX_OVERLAP					100//999						// 最大オーバーラップ指定 (オーバーラップ部分をそんなに大きくしたくないため)
#define MAX_RESYNC_FNO				100//20								// 再同期するためのフレーム待ち数

//// メモリ
#define	LOOP_QUE_SOCKRECV_SIZE		16								// GRABのキューサイズ
#define MAX_BUF_FRAME				30 								// 幅方向のフレームズレ一時バッファリング最大数
#define MAX_FOLDER_SAVE_NUM			10								// ファイル保存フォルダ最大数


#define MAX_COIL_BUFRING_NUM		12  							// バッチライン時のコイルFT特定用バッファリング最大数


//// 時間
#define TIME_DISP_REFRESH			500								// 画面更新周期 [ms]

// その他
#define RPI_TYPE					"rpi"							// 拡張子 (完成)
#define RPI_TYPE_WK					"rwk"							// 拡張子 (書込み中)
#define KIDO_SEC					"KIDO"							// 輝度セクション

//// 公開マクロ
#define GET_SIZE_JUST(x, l)		((((x-1)/l)+1)*l)					// xサイズをlの倍数に揃える(必ず、xのままか、ちょっと大きくなる方向)

//// FT同期待ち
#define FT_RETRY_CNT				100								// FT_RETRY_CNT * FT_RETRY_SLEEP だけ最大待ち
#define FT_RETRY_SLEEP				200								// ms


//=======================================
// 定数
//// 画像圧縮タイプ
enum EM_COMP_TYPE
{		
	COMP_TYPE_JPEG = 0,												// JPEG圧縮
	COMP_TYPE_LAC,													// LAC圧縮 (高度圧縮技術)
	COMP_TYPE_RAW,													// 生画像 (BMPﾍｯﾀﾞｰ無)
	COMP_TYPE_BMP,													// 生画像 (BMPﾍｯﾀﾞｰ有)
	COMP_TYPE_NUC,													// NUC圧縮 (改良版高度圧縮技術)
	COMP_TYPE_LAC5													// LAC5圧縮 (GPU版高度圧縮技術)
};
//// 画像タイプ
enum EM_COLOR_TYPE
{
	COLOR_TYPE_GRAY = 0,											// グレー画像	(フレームからは1画像)
	COLOR_TYPE_COLOR,												// カラー画像	(フレームからはRGBの3画像)
	COLOR_TYPE_SA													// 色差画像		(フレームからはRGBの3画像。画像処理後はRGB,R-Bの4画像)
};
//// 疵検タイプ
enum EM_SYS_TYPE
{
	SYS_TYPE_LINE = 0,												// 連続ライン
	SYS_TYPE_BAT													// バッチライン
};


//=======================================
// 受信構造体

//-----------------------------
//// 連結後の１フレーム分の情報
struct FRAME_DATA_COLOR {
	bool					bOk;									// 有効データ時true (Unionでセット。Chainで確認)
	void*					pFrameDataMgr[MAX_COLOR_NUM];			// 連結後のフレーム情報 (ホントは、FrameDataManager の方がよかった)
										// 色差：(Unionで3つ、Colorで1つ セット) (色差の場合,1つのFRAME_DATAから最大4つのFrameDataManagerが作り出されるため)
										// 輝度：常に１つだけUnionでセット
};

//-----------------------------
//// Coil→ImgUnion→Color→imgBufに受渡し領域
struct FRAME_DATA_ARRAY {
	int						nIns;									// インスタンスNo (0オリジン)
	int						nNum;									// セット数
	DWORD					nFno;									// フレームNo

	int						nIdx;									// 1つだけ見れた良い場合のレコード。使いやすいよう
	bool					nIsOk[NUM_CAM_POS];						// 幅方向分の有効セット
	HT_RPI_FDATA*			pData[NUM_CAM_POS];						// 幅方向分のフレーム情報  (PlgSyncでセット)

	FRAME_DATA_COLOR		Fdc;									// 連結後のフレーム情報 (Unionでセット)			

	
	// コイル切替判定認識 (CoilControlProcでセット)
	// 基本
	char					cKanriNo[SIZE_KIZUKEN_NO];				// 管理No
	char					cCoilNo[32];							// コイルNo
	int						mmFromHeadAtCoilStart;					// コイル先頭時点での溶接点からの距離[mm]
	DWORD					nCFno;									// コイル先頭からのフレームNo (0オリジン)
	EM_COMP_TYPE			emCompType;								// 画像圧縮タイプ
	int						nCompQuality;							// 圧縮率 (0～100%)
	bool					bDummyCoil;								// true:ダミーコイル


	// 個別
	EM_IMG_SAVE_STAT		emImgSaveStat;							// 画像状態認識 (バッチラインの場合は、CoilControlでも上書き)
	int						nEdgeIdx[2];							// フレーム画像連結後左端からのエッジ位置 [pixel]
	float					nEdgePos[2];							// [mm]
	//int						nFTset;									// 先後端位置セット時  (先端1  後端2 無し0)
	//long					nFTposCfNo;								// 先後端位置フレームNo (このファイル先頭からのフレームNo (0オリジン))
	//long					nFTposPixel;							// 先後端位置フレーム内pixel (そのフレーム上端からのピクセル位置 (0オリジン))


	__int64					nTimePlgSyncEnd;						// Plg情報紐付け完了時間 
};

//-----------------------------
//// RPIファイルの移動
struct RPI_MOVE {
	CString					sPath_F;								// 移動元フォルダパス (最後の\は無し)
//	CString					sPath_T;								// 移動先フォルダパス (最後の\は無し)
	CString					sName;									// RPIファイル名
	int						nIns;									// インスタンスNo (0オリジン)
};


//-----------------------------
// RpiWriteProcへ
// 受け渡し構造体の番号 
enum ENUM_KUBUN_RPIWRITE {
	em_FRAME_INFO = 0,												// フレーム情報追加
	em_WRITE_END													// 終了指示
};
// 受け渡しアイテム
struct DELIVERY_RPIWRITE {
	ENUM_KUBUN_RPIWRITE	emNo;										// どの構造体か
	BYTE*				data;										// 構造体のポインタ (NULLはあり得る)
};


//=======================================
// 簡単な共通関数
static CString GetThreadNameArray(char* c, int nNo) {
	CString s;
	s.Format("%s_%d", c, nNo);
	return s;
}
static CString GetThreadNameArray2(char* c, int nNo, int nNo2) {
	CString s;
	s.Format("%s_%d_%d", c, nNo, nNo2);
	return s;
}

// 複数単位のフォーマット
static CString GetArrayFormat(int const* p, int nCnt=1, char* sprit=",")
{
	CString cWk;
	cWk.Format("%d", p[0]);
	for(int ii=1; ii<nCnt; ii++) {
		cWk.AppendFormat("%s%d", sprit, p[ii]);
	}
	return cWk;
}



//------------------------------------------
// 受信キュー用の横取りした器を返却
// ThreadLoopQueue<FRAME_DATA>** pQue 返却元のスレッドループキューインスタンス
// FRAME_DATA_ARRAY* p キュー情報
//------------------------------------------
static void PoolBack_RecvFreeQue(IQueue<HT_RPI_FDATA>* pQue, FRAME_DATA_ARRAY* p)
{
	//// 横取りしておいた器を返却
	for(int ii=0; ii<p->nNum; ii++) {
		if( ! p->nIsOk[ii] ) continue; 
		pQue->AddToTail( p->pData[ii] );
	}

	//// 受け渡しキューを開放
	SAFE_DELETE( p );
}

static void PoolBack_RecvFreeQue(IQueue<HT_RPI_FDATA>** pQue, FRAME_DATA_ARRAY* p)
{
	int nIns = p->nIns;
	PoolBack_RecvFreeQue(pQue[nIns], p);
}




//=======================================
// メッセージ通知
//=======================================
//// 全てMainInctanceで受ける
//       場合によっては、MainInctanceが各ワーカースレッドにたれ流しする
enum ENUM_MAININCTANCE {
	E_DEF_ML_PARAM = WM_USER + 100,									// パラメータ変更通知
	E_DEF_ML_START,													// 検査開始要求
	E_DEF_ML_STOP,													// 検査停止要求
	E_DEF_ML_NEXTCOIL,												// 次コイル情報

	E_DEF_ML_STATUS,												// 状態問い合わせ

	E_DEF_END														// 終了
};

