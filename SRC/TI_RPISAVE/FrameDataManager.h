#pragma once

// 半固定
#include "..\..\Include\KizuCommon.h"									// 疵検インクルード
#include "..\..\Library\KizuLib\KizuLib.h"								// 疵検ライブラリ
#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// マクロ集
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// スタティックライブラリ集
#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ログ管理クラス


// 画像処理用ライブラリ インクルード
#include "..\..\Library\IportLib\IportCommon.h"							// iPort用クラス定数


// ローカル
#include "MainConst.h"													// 自クラス定数ヘッダー

using namespace KizuLib;
using namespace IportLib;

class FrameDataManager : implements FRAME_DATA
{
	//// 既に これは、継承元で定義されている
	//DWORD			nCamset;										// カメラセット		※ (1オリジン) (set:PlgSyncProc)
	//BYTE*			pImgs;											// 画像				※ 連結後の生画像情報 (先頭にはビットマップヘッダー付与済み)(set:ImgUnionProc)
	//DWORD			nLen;											// 画像サイズ		※ (ビットマップヘッダーサイズも付与) (set:ImgUnionProc) 


	// 未使用 -----------
	//DWORD			nNo;											// iPortフレームNo	(0オリジン) (1コだけ)
	//WORD			nSeqNum[32];									// iPortシーケンス番号

	//HANDLE		hInfo[4];										// SetAddInfo で付与した情報
	
	// 都度セット -----------
	//KizuPerformanceCounter	cls_Time;							// 取得した時間(ストップウォッチ機能を使用)
																			// ライブラリ内でセットするタイムスタンプ
																			// ＃０：最初のカメラ角度を取得した時間
																			// ＃１：全部のカメラ角度が揃って、画像をコピー完了した時間

public:
	// ストップウォッチ
	//KizuPerformanceCounter	cls_Time;							// 取得した時間(ストップウォッチ機能を使用)
																		// ライブラリ内でセットするタイムスタンプ

	static const int		TIME_GetCamFast			= 10;				// ＃10：最初のカメラを取得した時間
	static const int		TIME_GetCamLast			= 11;				// ＃11：最後のカメラを取得した時間

	static const int		TIME_PlgSync_END		= 15;				// ＃19：PlgSyncProcが処理完了時

	//static const int		TIME_Coil_START			= 16;				// ＃16：CoilControlProcが処理開始時
	//static const int		TIME_Coil_END			= 17;				// ＃17：CoilControlProcが処理完了時 

	static const int		TIME_ImgUnion_START		= 20;				// ＃20：ImgUnionProcが処理開始時
	static const int		TIME_ImgUnion_END		= 21;				// ＃21：ImgUnionProcが処理完了時  (↑までは、ImgUnionProcでセット)

//	static const int		TIME_ImgColor_START		= 22;				// ＃20：ImgCompProcが処理開始時
//	static const int		TIME_ImgColor_END		= 23;				// ＃21：ImgCompProcが処理完了時

	static const int		TIME_ImgBuf_START		= 24;				// ＃20：ImgBufProcが処理開始時
	static const int		TIME_ImgBuf_END			= 25;				// ＃21：ImgBufProcが処理完了時

	static const int		TIME_ImgComp_START		= 26;				// ＃20：ImgCompProcが処理開始時
	static const int		TIME_ImgComp_END		= 27;				// ＃21：ImgCompProcが処理完了時

	static const int		TIME_RpiWrite_START		= 30;				// ＃20：RpiWriteProcが処理開始時
	static const int		TIME_RpiWrite_END		= 31;				// ＃21：RpiWriteProcが処理完了時



//// ＋αの公開変数
public:
	// 1フレームのデータ領域
	int						nBufSize;									// 圧縮後領域のバッファサイズ(セクタを考慮したサイズ) (set:構築時)
	BYTE*					pBuf;										// 圧縮後の領域本体 (先頭にはRpiFileManager::LINK分を付与) (領域確保:構築時) (set:ImgCompProc)
																			// このまま nSetSize分 RPIファイルに書込み
	int						nCompSize;									// 圧縮後サイズ (set:ImgCompProc)
	int						nSetSize;									// データのセットサイズ (set:ImgCompProc) (更新:RpiWriteProc)
	
	BYTE*					pFdata;										// 付加情報部の先頭ポインタ。無しの場合NULL
	int						nFdataSize;									// 付加情報部のサイズ。無しの場合0
	

	//// 通常のデータ (set:ImgUnionProc)
	// フレーム単位で持っておく 基本情報 
	DWORD					nIns;										// インスタンス番号 (0オリジン)
	DWORD					nStartCamset;								// 一番左端のカメラセット番号 (1オリジン)
	DWORD					nAngle;										// カメラ角度 (0オリジン)
	DWORD					nColor;										// 画像色番号 (0オリジン)
	DWORD					nMen;										// 表裏区分 (0:表 1:裏)
	DWORD					nUnionWidth;								// フレーム画像連結後の画像幅
	DWORD					nUnionHeight;								// フレーム画像連結後の画像高さ


	// フレーム毎の情報
	DWORD					nCFno;										// コイル先頭からのフレームNo (0オリジン)
	int						nEdgeIdx[2];								// 左端からの左右エッジPixel位置 [pixel]
	bool					bRpiSave;									// 画像保存選択
	EM_IMG_SAVE_STAT		emImgSaveStat;								// 画像状態認識


	//// 付帯的な情報
	//int						nFTset;										// 先後端位置セット時  (先端1  後端2 無し0)
	//long					nFTposCfNo;									// 先後端位置フレームNo (このファイル先頭からのフレームNo (0オリジン))			※ IMG_SAVE_STAT_F,IMG_SAVE_STAT_Tの時のみ
	//long					nFTposPixel;								// 先後端位置フレーム内pixel (そのフレーム上端からのピクセル位置 (0オリジン))	※ IMG_SAVE_STAT_F,IMG_SAVE_STAT_Tの時のみ



	// コイル情報
	char					cKizukenNo[SIZE_KIZUKEN_NO];				// 管理No
	char					cCoilNo[32];								// コイルNo
	EM_COMP_TYPE			emCompType;									// 圧縮形式
	int						nCompQuality;								// 圧縮率
	bool					bDummyCoil;									// true:ダミーコイル

	// UDP情報
	UDP_DATA				UdpData;									// UDP情報
//	int						nUnionEdgePos[2];							// フレーム画像連結後のエッジ位置 [pixel] (左からのエッジ)


	//// 画面表示用 (set:ImgUnionProc)
	int						nImgLapPos[MAX_CAMSET][2];					// フレーム間画像の重なり位置 [結合後画像の左からのpixel]
	int						nImgClipPos[2];								// 左右の削除画素位置 [結合後画像の左からのpixel]


	//// その他
	int						nWriteMode;									// 書込みモード (0:同期書込みモード 1:セクタ書込みモード) (set:構築時)
	
//// 公開関数
public:
	FrameDataManager(void);
	virtual ~FrameDataManager(void);

	void			Clear();											// 初期化

//// メンバー関数
protected:
	virtual void	Alloc(int const* p=0);								// 器生成時 (IportManagerでInitされた時コールバック)
	virtual void	Free();												// 器解放時 (IportManagerでDisposeされた時コールバック)
};
