#pragma once

#include "AvalACL2Manager.h"
#include "CGrabberCorrMatImpl.h"

namespace AvalLib
{
	

	class AvalACL2ManagerColorCorr : public AvalACL2Manager
								   , public CGrabberCorrMatImpl
	{
			KizuPerformanceCounter kc;

	public:
		AvalACL2ManagerColorCorr(CString sMyManagerName="AvalMgrCorr", CString sMyIport="AvalACL2Base");
		~AvalACL2ManagerColorCorr(void);

		// 画像情報. INIがモノクロカメラの振りをしている場合があるので、正しい幅にもどす
		// case1：幅が同じで、PixelBitが24
        // case2：幅が本来の３倍で、PixelBitが８
		virtual long		GetFrameWidth()     const { return NULL != my_pGrabber ? my_pGrabber->GetWidth()*my_pGrabber->GetPixelNBit()/24: 0; }
       virtual long		GetFramePixelNBit() const { return 24; }
 
		//フレーム取得後、色分離し、３枚のフレームにして下流に流す
		void OnFrameGrabbed(void** pImgs, DWORD nFrameNum, int* nSeqNum, __int64 nFastTime);

	};

}