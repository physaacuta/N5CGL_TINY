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

		// �摜���. INI�����m�N���J�����̐U������Ă���ꍇ������̂ŁA���������ɂ��ǂ�
		// case1�F���������ŁAPixelBit��24
        // case2�F�����{���̂R�{�ŁAPixelBit���W
		virtual long		GetFrameWidth()     const { return NULL != my_pGrabber ? my_pGrabber->GetWidth()*my_pGrabber->GetPixelNBit()/24: 0; }
       virtual long		GetFramePixelNBit() const { return 24; }
 
		//�t���[���擾��A�F�������A�R���̃t���[���ɂ��ĉ����ɗ���
		void OnFrameGrabbed(void** pImgs, DWORD nFrameNum, int* nSeqNum, __int64 nFastTime);

	};

}