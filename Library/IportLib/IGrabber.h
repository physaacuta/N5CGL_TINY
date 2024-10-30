//======================================================================
// �������J�����捞 I/F
//======================================================================

#pragma once

// �C���|�[�g
using namespace KizuLib;

namespace KizuLib
{
	//===================================================================
	// �B���n��`
	interface IGrabber
	{
		// �P��̂�
			// Init  �͈������Ⴄ���߁A�p����Ŏ���
		virtual void		Dispose() = 0;									// �J��

		//virtual bool		Start() = 0;									// �J�n
		//virtual bool		Stop() = 0;										// ��~

		// �B���n
		virtual int			StartGrab() = 0;								// �B���J�n
		virtual int			StopGrab() = 0;									// �B����~
		virtual bool		Reset() = 0;									// ���Z�b�g (Init����̏�Ԃɖ߂�)

		protected: IGrabber(){  }
	};


	//===================================================================
	// �B���p�����[�^
	interface IGrabberParam
	{
        virtual long		GetFrameWidth()	const		= 0;
        virtual long		GetFrameHeight() const		= 0;
        virtual long		GetFrameNChannel() const	= 0;
        virtual long		GetFramePixelNBit() const	= 0;

		protected: ~IGrabberParam() {}
	};


	//===================================================================
	// �_�~�[�B���n��`
	interface IGrabberDummy
	{
		// �_�~�[����
		virtual bool		OpenBmp(char const* cPathFormat, int nNoFrom, int nNoTo, int nCycleTimer) = 0;	// �A���r�b�g�}�b�v��`
		virtual void		CloseBmp() = 0;														// �A���r�b�g�}�b�v���
		virtual bool		OpenAvi(char const* cPathFormat, int nCycleTimer) = 0;				// AVI��`
		virtual void		CloseAvi() = 0;														// AVI���
		virtual bool		OpenManual() = 0;
		virtual void		CloseManual() = 0;

		// ����
		virtual void		ChangeSmpPathFormat(int nMode = 0) = 0;								// �t�H�[�}�b�g�ؑ�

		virtual void		OnBmpFrameGrabbed() = 0;;											// �S�J�����Z�b�g�A�J�����p�x�ɑ΂��ăr�b�g�}�b�v�摜���Z�b�g
		virtual void		OnAviFrameGrabbed() = 0;;											// �S�J�����Z�b�g�A�J�����p�x�ɑ΂���AVI�摜���Z�b�g
		virtual bool		OnManualFrameGrabbed(BYTE* pImgs, DWORD nBytes, DWORD nCamset) = 0;
		
		protected: ~IGrabberDummy() {}
	};


	//===================================================================
	//���F����
	interface IGrabberCorrMat{
		enum { NCOEF  = 12 };
		virtual void		SetCorrMat(int , const float* mat) = 0;							// ���F�s��l�Z�b�g
		virtual void		ResetCorrMat() = 0;	
		
		protected: ~IGrabberCorrMat() {}
	};
};