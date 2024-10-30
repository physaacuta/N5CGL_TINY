#pragma once

#include "AvalACL2Manager.h"

namespace AvalLib
{

	class AvalACL2ManagerColor : public AvalACL2Manager
	{
	public:
		AvalACL2ManagerColor(CString sMyManagerName="AvalMgrCol", CString sMyIport="AvalACL2Base");
		~AvalACL2ManagerColor(void);


		// �摜���. INI�����m�N���J�����̐U������Ă���ꍇ������̂ŁA���������ɂ��ǂ�
		// case1�F���������ŁAPixelBit��24
        // case2�F�����{���̂R�{�ŁAPixelBit���W
		virtual long		GetFrameWidth()     const { return NULL != my_pGrabber ? my_pGrabber->GetWidth()*my_pGrabber->GetPixelNBit()/24: 0; }
       virtual long		GetFramePixelNBit() const { return 24; }
 
		//�t���[���擾��A�F�������A�R���̃t���[���ɂ��ĉ����ɗ���
		void OnFrameGrabbed(void** pImgs, DWORD nFrameNum, int* nSeqNum, __int64 nFastTime);
	};

}