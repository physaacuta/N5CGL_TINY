#include "StdAfx.h"
#include "IportManager2pColor.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

using namespace IportLib;


////===================================================================
//// マクロ
////===================================================================
//#define LOM(t) mcls_LogMe.Write(KizuLib::LogFileManager::t			// ローカル専用ログ


//------------------------------------------
// コンストラクタ
// CString sMyManagerName 自クラスの名称
// CString sMyIport iPortクラスの名称
//------------------------------------------
IportManager2pColor::IportManager2pColor(CString sMyManagerName, CString sMyIport) :
// 1.名前
IportManager2p(sMyManagerName, sMyIport)
{
	// カラーなので、チャンネル数を3に変更
	my_nChannel = 3;
}

//------------------------------------------
// デストラクタ
//------------------------------------------
IportManager2pColor::~IportManager2pColor(void)
{
}

//------------------------------------------
// 1カメラセット分の画像を取得して、スレッドループキューにキューイング
// void** pImgs 画像(カメラ角度分)
// long* nX 画像サイズ (1コだけ)
// long* nY 画像サイズ (1コだけ)
// DWORD nFrameNum フレームNo (1コだけ)
// DWORD nCamset カメラセット単位の何個目(0オリジン)
// WORD* nSeqNum iPortシーケンス番号
// __int64 nFastTime 最初のカメラ角度を取得した時間
//------------------------------------------
void IportManager2pColor::OnFrameGrabbed(void** pImgs, long nX, long nY, DWORD nFrameNum, DWORD nCamset, WORD* nSeqNum, __int64 nFastTime)
{
	//// 空き器を取得
	FRAME_DATA* p = myque_pFrame->GetFree(0);				// セマフォー待ちバージョン(すぐに結果を返したい)
	if( NULL == p ) {
		// 器空きなし (回復可能なエラー)
		LOG(em_ERR), "[%s] GRABバッファオーバラン(set=%d, No=%d):スレッドループキュー空き無し!", my_sThreadName, nCamset, nFrameNum);
		OnAddInfo(IportLib::DIV_ID_ERROR_NO_IMG_QUEFULL, nFrameNum, nCamset, 0, 0 );
		return;
	}

	//// 画像を格納
	// 時間をセット
	p->cls_Time.PutTimestamp(0, nFastTime);			// ＃０：最初のカメラ角度を取得した時間

	// データコピー
	p->nCamset	= nCamset;
	p->nLen		= (nX*2) * nY;	// 1コだけ
	p->nNo		= nFrameNum;

	for(DWORD ii=0; ii<my_nAngleNum; ii++) {
		//p->nSeqNum[ii] = nSeqNum[ii];
		p->nSeqNum[ii*2+0] = nSeqNum[ii];
		p->nSeqNum[ii*2+1] = nSeqNum[ii+1];	// △かなり暫定
		p->hInfo[ii]   = my_hAddInfo[GetCamIndex(nCamset, ii)];

		if( ! my_bImgTurn_LR ) {
			// カラー画像をRGB画像に分離
			//Copy_PackedColor8(&p->pImgs[ii*my_nImgSize*my_nChannel], &pImgs[2*ii], nX*my_nChannel);
			IportLib::Copy_PackedColor8_2p(&p->pImgs[ii*my_nImgSize*my_nChannel], &pImgs[2*ii], 
				my_nFrameW, nX*my_nChannel, my_nFrameW, my_nFrameH);
		} else {
			_ASSERT( false );	// 未対応
		}
	}

	p->cls_Time.PutTimestamp(1);					// ＃１：全部のカメラ角度が揃った時間 (=今)

	// 登録
	myque_pFrame->AddPool(p); 
}

/*
//------------------------------------------
// カラー画像をRGB画像に分離
// BYTE* dstImg		RGB分離画像 (先)
// void** pImgsrc	カラー画像	(元)
// int pitch_src	iPortあたりの幅(画素数考慮)
//------------------------------------------
void IportManager2pColor::Copy_PackedColor8(BYTE* dstImg, void** pImgsrc, int pitch_src)
{
    const long width  = my_nFrameW;
    const long height = my_nFrameH;
    const long pitch  = my_nFrameW;

    const BYTE*const psrc0 = (BYTE*)pImgsrc[0];
    const BYTE*const psrc1 = (BYTE*)pImgsrc[1];

    BYTE*const pdstR = &dstImg[my_nImgSize*IPORT_COLOR_R];
    BYTE*const pdstG = &dstImg[my_nImgSize*IPORT_COLOR_G];
    BYTE*const pdstB = &dstImg[my_nImgSize*IPORT_COLOR_B];

    const long sz_pack = 3; // RGB

    for(int y=0; y<height; y++)
    {
        const long src_yoffset = y * pitch_src;
        const long dst_yoffset = y * pitch;
        for(int x=0; x<width; x+=2)
        {
			int srcoffset = src_yoffset + sz_pack * x/2;

			// 偶数画素						// 左半分
            pdstG[dst_yoffset + x]     = psrc0[srcoffset + 1];	//G
            pdstB[dst_yoffset + x]     = psrc0[srcoffset    ];	//B
            pdstR[dst_yoffset + x]     = psrc0[srcoffset + 2];	//R
			// 奇数画素						// 右半分
            pdstG[dst_yoffset + x + 1] = psrc1[srcoffset + 1];	//G
            pdstB[dst_yoffset + x + 1] = psrc1[srcoffset    ];	//B
            pdstR[dst_yoffset + x + 1] = psrc1[srcoffset + 2];	//R
        }
    }
}
*/


