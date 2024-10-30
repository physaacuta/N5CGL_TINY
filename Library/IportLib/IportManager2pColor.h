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
	class IportManager2pColor :	public IportManager2p
	{
	//// 公開構造体
	public:


	//// 公開関数
	public:
		IportManager2pColor(CString sMyManagerName="iPortMgr2pClr", CString sMyIport="iPortBase");
		virtual ~IportManager2pColor(void);

	//// メンバー関数
	protected:
		virtual void		OnFrameGrabbed(void** pImgs, long nX, long nY, DWORD nFrameNum, DWORD nCamset, WORD* nSeqNum, __int64 nFastTime);	// 1カメラセット分の画像を取得して、スレッドループキューにキューイング
//		void Copy_PackedColor8(BYTE* dstImg, void** pImgsrc, int pitch_src);		// カラー画像をRGB画像に分離

	};
};

