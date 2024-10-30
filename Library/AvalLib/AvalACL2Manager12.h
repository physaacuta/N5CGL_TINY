#pragma once
#include "AvalACL2Manager.h"

namespace AvalLib
{

	class AvalACL2Manager12 : public AvalACL2Manager
	{
		class Gamma_12_8 *m_gamma;
	public:
		AvalACL2Manager12(CString sMyManagerName="AvalMgr12", CString sMyIport="AvalACL2Base");
		~AvalACL2Manager12(void);


		virtual long		GetFramePixelNBit() const { return 8; }
		void SetGamma( double g );
 
		//�t���[���擾��A�F�������A�R���̃t���[���ɂ��ĉ����ɗ���
		void OnFrameGrabbed(void** pImgs, DWORD nFrameNum, int* nSeqNum, __int64 nFastTime);
		void	SetParam( const char *id, const void *param );
	};

}