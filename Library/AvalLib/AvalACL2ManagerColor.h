#pragma once

#include "AvalACL2Manager.h"

namespace AvalLib
{

	class AvalACL2ManagerColor : public AvalACL2Manager
	{
	public:
		AvalACL2ManagerColor(CString sMyManagerName="AvalMgrCol", CString sMyIport="AvalACL2Base");
		~AvalACL2ManagerColor(void);


		// 画像情報. INIがモノクロカメラの振りをしている場合があるので、正しい幅にもどす
		// case1：幅が同じで、PixelBitが24
        // case2：幅が本来の３倍で、PixelBitが８
		virtual long		GetFrameWidth()     const { return NULL != my_pGrabber ? my_pGrabber->GetWidth()*my_pGrabber->GetPixelNBit()/24: 0; }
       virtual long		GetFramePixelNBit() const { return 24; }
 
		//フレーム取得後、色分離し、３枚のフレームにして下流に流す
		void OnFrameGrabbed(void** pImgs, DWORD nFrameNum, int* nSeqNum, __int64 nFastTime);
	};

}