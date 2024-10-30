//======================================================================
// �������J�����捞�́Apitch��8�����l���Ă��Ȃ�
//   ����Ă�����AConnect �ŃG���[�ɂ���
//======================================================================

#pragma once

// �J�����֌W
#include "..\IportLib\IportCommon.h"											// �r���}�N��

// �C���|�[�g
using namespace KizuLib;
using namespace IportLib;

namespace AvalLib
{

	//===================================================================
	// �J�����捞 I/F
	//===================================================================
	class AvalManagerIF
	{

	//// �����o�[�萔
	protected:
		static const int	MAX_CHANNEL	= 4;						// �ő�S�`�����l��
		static const int	MAX_BOARD	= 8;						// �ő�W�{�[�h
		static const int	MAX_CAMNUM	= MAX_CHANNEL*MAX_BOARD;	// �ő�J�����ڑ���


		// �B�����
		enum EM_GRAB_MODE {
			GRAB_MODE_HW = 0,												// �n�[�h�E�F�A�B�z�����m
			GRAB_MODE_BMP,													// Bitmap�t�@�C��
			GRAB_MODE_AVI,													// AVI�t�@�C��
			GRAB_MODE_MAN													// �}�j���A���Z�b�g
		};



	public:

		virtual void		SetQueFrame(ThreadLoopQueue<FRAME_DATA>* pQue) = 0;


		//// ���̂�
		//virtual void		Dispose() = 0;									// ���

		//virtual bool		Start() = 0; 									// �X���b�h�J�n �� ���̃X���b�h�́A�z���g�ɃX���b�h�����s�����܂Ń��b�N�����
		//virtual bool		Stop() = 0;										// �X���b�h��~
		//
		//// �B���n
		//virtual BOOL		StartGrab() = 0;								// �B���J�n
		//virtual BOOL		StopGrab() = 0;									// �B����~
		//virtual bool		Reset() = 0;									// ���Z�b�g


		// �T���v���摜����n
		virtual bool		OpenBmp(char const* cPathFormat, int nNoFrom, int nNoTo, int nCycleTimer) = 0;	// �A���r�b�g�}�b�v��`
		virtual void		CloseBmp() = 0;											// �A���r�b�g�}�b�v���
		virtual bool		OpenAvi(char const* cPathFormat, int nCycleTimer) = 0;	// AVI��`
		virtual void		CloseAvi() = 0;											// AVI���
		virtual bool		OpenManual() = 0;
		virtual void		CloseManual() = 0;
		virtual bool		OnManualFrameGrabbed(BYTE* pImgs, DWORD nBytes, DWORD nCamset) = 0;

		// ���̑�����
		virtual bool		SendSerialTemp(DWORD set, DWORD ang, float& temp) = 0;	// �J�������x���擾 (�J�����Ɛڑ��������_��)
		virtual void		SyncFrame(DWORD set, DWORD ang, WORD nSkipCnt) =0;


	//// �v���p�e�B�֌W
	public:
		// �I�u�W�F�N�g�擾


		virtual inline ThreadLoopQueue<FRAME_DATA>*	GetQueFrame() = 0; 
		virtual inline ThreadQueue<DELIVERY_INFO>&	GetQueInfo() = 0;

		// Set
		virtual void		SetResetModuleOnReset(bool on) = 0;
		virtual void		SetAddInfo(DWORD set, DWORD ang, HANDLE h) = 0;	// �t���[�����ɕt�^������
		virtual void		SetImgTurn_LR(bool on) = 0;						// �摜���E���]
		// Get
		virtual void*		GetDummyBuf() = 0;

		// �摜���
		//virtual IportBase::TYP_FRAME_INFO const& GetFrameInfo() const = 0;
        virtual long		GetFrameWidth()	const		= 0;
        virtual long		GetFrameHeight() const		= 0;
        virtual long		GetFrameNChannel() const	= 0;
        virtual long		GetFramePixelNBit() const	= 0;

		// �X�e�[�^�X
		virtual DWORD		GetResendCount(DWORD set, DWORD ang) const = 0;	// �p�P�b�g�đ���
		virtual DWORD		GetLostCount  (DWORD set, DWORD ang) const = 0;	// �p�P�b�g������
		virtual DWORD		GetResendTotal()	 const = 0;
		virtual DWORD		GetLostTotal()		 const = 0;
		virtual DWORD		GetQueueCount()		 const = 0;
		virtual DWORD		GetMaxQueueCount()	 const = 0;
		virtual DWORD		GetChDiff	  (DWORD set, DWORD ang) const = 0;
		virtual DWORD		GetChDiffMax()		 const = 0;
		virtual DWORD		GetFrameNo()		 const = 0;
		virtual bool		IsInit()			 const = 0;
		virtual bool		IsGrabRun()			 const = 0;

	};

};