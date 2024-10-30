#pragma once

// ���[�J��
#include "IportManager1p.h"													// iPort1p���N���X
#include "make_planar_color_corr.h"

// �C���|�[�g
using namespace KizuLib;
using namespace ImgLib;

namespace IportLib
{

	//===================================================================
	// �J�����捞�{��
	//===================================================================
	class IportManager1pColor :	public IportManager1p
	{
	//// ���J�\����
	public:


	//// ���J�֐�
	public:
		IportManager1pColor(CString sMyManagerName="iPortMgr1pClr", CString sMyIport="iPortBase");
		virtual ~IportManager1pColor(void);

	//// �����o�[�֐�
	protected:
		virtual void		OnFrameGrabbed(void** pImgs, long nX, long nY, DWORD nFrameNum, DWORD nCamset, WORD* nSeqNum, __int64 nFastTime);	// 1�J�����Z�b�g���̉摜���擾���āA�X���b�h���[�v�L���[�ɃL���[�C���O
//		void Copy_PackedColor8(BYTE* dstImg, void** pImgsrc, int pitch_src);		// �J���[�摜��RGB�摜�ɕ���

	};
};