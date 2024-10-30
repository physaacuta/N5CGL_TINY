#pragma once

// ���[�J��
#include "IportManager2p.h"													// iPort2p���N���X
#include "make_planar_color_corr.h"

// �C���|�[�g
using namespace KizuLib;
using namespace ImgLib;

namespace IportLib
{

	//===================================================================
	// �J�����捞�{��
	//===================================================================
	class IportManager2pColor :	public IportManager2p
	{
	//// ���J�\����
	public:


	//// ���J�֐�
	public:
		IportManager2pColor(CString sMyManagerName="iPortMgr2pClr", CString sMyIport="iPortBase");
		virtual ~IportManager2pColor(void);

	//// �����o�[�֐�
	protected:
		virtual void		OnFrameGrabbed(void** pImgs, long nX, long nY, DWORD nFrameNum, DWORD nCamset, WORD* nSeqNum, __int64 nFastTime);	// 1�J�����Z�b�g���̉摜���擾���āA�X���b�h���[�v�L���[�ɃL���[�C���O
//		void Copy_PackedColor8(BYTE* dstImg, void** pImgsrc, int pitch_src);		// �J���[�摜��RGB�摜�ɕ���

	};
};

