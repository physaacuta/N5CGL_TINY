#pragma once

// インクルード
#include <windows.h>

#include "LineCommon.h"
#include "..\Library\DefectLib\SockOneToAny.h"




//***************************************************************
// 起動要因

//=======================================
// 蓄積機能
//=======================================
// TI_RPISAVE
#define FACT_TI_RPISAVE_01			9421					// 検査開始要求 (←TO_DEFECT) 
#define FACT_TI_RPISAVE_02			9422					// 検査停止要求 (←TO_DEFECT)

#define FACT_TI_RPISAVE_04			9424					// 次コイル情報事前通知 (←TO_DEFECT)
																// ML[0] 面(-1:全部 0:表 1:裏)
																// ML[1,2] 検査パターン (1～)
																// ML[3,4] 検査表面状態 (1～)
																// ML[5] 切替フレームNo  ※ 重要。 今回これで切替
																// MC[0] 管理No
																// MC[1] コイルNo

#define FACT_TI_RPISAVE_05			9425					// 外部コイル切替情報 (←TO_DEFECT) ※バッチラインのみ
																// FL[0] 表裏区分 (0:表 1:裏)
																// FL[1] 先端尾端区分 (0:先端 1:尾端)
																// FL[2] 対象フレームNo
																// FL[3] 対象フレーム内Y位置 [pixel] （画像上が0pixelとして指定）

//***************************************************************
// 判定管理 RAW画像送信


//=======================================
// タスク名称
//=======================================

// 蓄積機能
#define TI_RPISAVE					"TI_RPISAVE"			// 全長画像保存管理 (=画像処理エンジン)




//=======================================
// 判定管理 RAW画像送信 間引き設定
#define FRAME_RAW_SEND														// コメントにすることで機能停止
#define FRAME_HT_SEND_PORT				2									// 判定側の送信ポート数	
#define MAX_BUF_QUE_RPISOCK				8									// 8MB×4画像×MAX_BUF_QUE_RPISOCK 分のメモリが確保される。8キューで256MB



//=======================================
// 判定→全長画像処理 フレームデータ
struct HT_RPI_FDATA : public SOCK_F_HEAD2
{
	//// 送信元でセット
	int					nCamId;				// カメラセット 
	DWORD				nFrameNo;			// 送信連番 (0オリジン)
	float				dEdgePos[2];		// エッジ位置（カメラ座標系)mm単位


	// RAWデータ、固定サイズ前提
	//		※	addr[0] の中に、画像領域最大サイズがセットされている。
	//			で送りたい画像のみを前詰めにセット
	int		nImgCnt;						// 画像セット数
	int		nWidth;							// 画像幅
	int		nHeight;						// 画像高さ
	DWORD	nImgSize[MAX_IMAGE_CNT];		// 画像サイズ	※ 0 は画像無し。
	DWORD	nImgOffset[MAX_IMAGE_CNT];		// 画像の先頭位置(Addrからの画像先頭オフセット)	 つまり最初の画像は0となるはず

	// 受け取り側でセット
	__int64				nTimeFrameRecv;		// 受信完了時間

	//========================
	// 器生成時
	// int const* p=0 受け渡しカスタム情報(int 配列) 
	// [0] 画像サイズ最大サイズ
	void	Alloc(int const* p)
	{
		int nSize = p[0];

		// データ部初期化
		memset(this, 0x00, sizeof(HT_RPI_FDATA));		// 自分自身を初期化
														// 領域確保
		this->base_head.addr[0] = (long*)_aligned_malloc(nSize, 32);
	};

	//========================
	// 器解放時
	void	Free()
	{
		if (NULL != this->base_head.addr[0]) {
			_aligned_free(this->base_head.addr[0]);
			this->base_head.addr[0] = NULL;
		}
	};

};

//***************************************************************
// 定義
//// 画像状態
enum EM_IMG_SAVE_STAT
{				
	// 保存対象画像																 連続ライン、	バッチライン
	IMG_SAVE_STAT_SAVE = 0,											// 保存対象		○				○	
	IMG_SAVE_STAT_CHANGE = 9,										// コイル切替	○				×

	IMG_SAVE_STAT_F = 1,											// 先端画像		×				○
	IMG_SAVE_STAT_T = 2,											// 尾端画像		×				○

};

//// 本タスクで使用する速度UDP情報 
//		(対象システム全部が対象としたときの、本タスクが必要な項目の最大公約数)
struct UDP_DATA {
	DWORD	framenum;												// フレーム番号			framenum
	DWORD	speed;													// ライン速度(mm/sec)		※0
	float	vRes;													// 縦分解能(mm/pixel)
//	int		toVal[8];												// 統括で勝手に使える領域（パイプにそのまま帰る）

	struct _pos {
		long	posFromHead;										// 溶接点からの距離(mm)		※0
		float	edgePosL;											// エッジ位置負側(mm)
		float	edgePosR;											// エッジ位置正側(mm)
	} pos[2];
};
