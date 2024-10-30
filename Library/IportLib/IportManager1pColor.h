#pragma once

// ローカル
#include "IportManager1p.h"													// iPort1p基底クラス
#include "make_planar_color_corr.h"

// インポート
using namespace KizuLib;
using namespace ImgLib;

namespace IportLib
{

	//===================================================================
	// カメラ取込本体
	//===================================================================
	class IportManager1pColor :	public IportManager1p
	{
	//// 公開構造体
	public:


	//// 公開関数
	public:
		IportManager1pColor(CString sMyManagerName="iPortMgr1pClr", CString sMyIport="iPortBase");
		virtual ~IportManager1pColor(void);

	//// メンバー関数
	protected:
		virtual void		OnFrameGrabbed(void** pImgs, long nX, long nY, DWORD nFrameNum, DWORD nCamset, WORD* nSeqNum, __int64 nFastTime);	// 1カメラセット分の画像を取得して、スレッドループキューにキューイング
//		void Copy_PackedColor8(BYTE* dstImg, void** pImgsrc, int pitch_src);		// カラー画像をRGB画像に分離

	};
};