#pragma once

// ローカル
#include "IportManager2p.h"													// iPort2p基底クラス
#include "make_planar_color_corr.h"

// インポート
using namespace KizuLib;
using namespace ImgLib;

namespace IportLib
{

	//===================================================================
	// カメラ取込本体
	//===================================================================
	class IportManager2pColorCorr :	public IportManager2p
								  , implements IGrabberCorrMat
	{
	//// 公開構造体
	public:


	//// 公開関数
	public:
		IportManager2pColorCorr(CString sMyManagerName="iPortMgr2pClr", CString sMyIport="iPortBase");
		virtual ~IportManager2pColorCorr(void);

		virtual bool		Init(DWORD nFrameW, DWORD nFrameH, DWORD camset=1, DWORD angle=1, BYTE nDummyVal=0x80);	// 初期化
		virtual void		Dispose();															// 解放

		virtual void		SetCorrMat(int camset, const float* mat);							// 混色行列値セット
		virtual void		ResetCorrMat();														// 混色行列値リセット


	//// メンバー関数
	protected:
		virtual void		OnFrameGrabbed(void** pImgs, long nX, long nY, DWORD nFrameNum, DWORD nCamset, WORD* nSeqNum, __int64 nFastTime);	// 1カメラセット分の画像を取得して、スレッドループキューにキューイング


	//// メンバー変数
    private:
		float***			my_mat_corr;					// 混色行列値 [0or1][カメラセット][混色行列値 (r,g,b,offset)]
        int					my_mat_fore;					// ダブルバッファーの考え方。my_mat_corrのどっちの配列を使うか

	};
};

