// *********************************************************************************
//	Aval�{�[�h���� �N���X
//	[Ver]
//		Ver.01    2014/07/07  vs2005 �Ή�
//
//	[����]
//		�EAval�ւ̒��ڃA�N�Z�X�́A�֎~
//		�EFC �� AvalGrabberACL2Batch�ɑ���
//		�EGrabberManager���p�����邱�ƂŁAIportManager�֘A�Ɠ���I/F�ő���\
//	
//-------------------------------------
// [��{�I�Ȏg�p�̍l����]
//		�EIportMgr �Ō������� Angle �ɂ����Ή����Ă��Ȃ��B�P�C���X�^���X�����p�x
//			�E����Camset���P�C���X�^���X�Ŏ�荞�ގ��͍l���Ă��Ȃ�
//				���̏ꍇ�́A�����C���X�^���X�Ƃ���B
//
// *********************************************************************************


#pragma once


// �r�����C�u����
#include "..\KizuLib\CLASS\KizuMacro.h"											// �r���}�N��
#include "..\KizuLibMFC\CLASS\ThreadManager.h"									// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\KizuMMX\MemCopy.h"												// MMX���C�u����
#include "..\..\Include\LibSelect.h"										// ���C�u�����ؑփw�b�_�[


// �e�X�g�摜�擾�p
#include "..\ImgLib\CLASS\KizuAviManager.h"									// AVI�t�@�C��
#include "..\KizuLib\CLASS\KizuBitmapManager.h"								// BitMap�t�@�C��


// �J�����֌W
#include "..\IportLib\IportCommon.h"										// �B���n��`
#include "..\IportLib\GrabberManager.h"										// �B���n���


// ���[�J��
#include "AvalManagerIF.h"
#include "AvalACL2Base.h"


// �C���|�[�g
using namespace KizuLib;
using namespace ImgLib;


namespace AvalLib
{


	//===================================================================
	// �J�����捞 ���ʕ��{��
	class AvalACL2Manager : public GrabberManager
	{

	//// �����o�[�萔
	protected:
		static const int	MAX_CHANNEL	= 4;						// �ő�S�`�����l��
		static const int	MAX_BOARD	= 8;						// �ő�W�{�[�h
		static const int	MAX_CAMNUM	= MAX_CHANNEL*MAX_BOARD;	// �ő�J�����ڑ���




	public:
		AvalACL2Manager(CString sMyManagerName, CString sMyGrabberName);
		virtual ~AvalACL2Manager(void); 

		



		//----------------
		// IF����

		// ���̂�
		virtual BOOL Init(int board_id, int board_ch_id, const char* ini_filename, int nbuff, DWORD nFrameW, DWORD nFrameH, BYTE nDummyVal);	// ������
		virtual BOOL Init(int nchannel, const char* ini_filename, int nbuff, DWORD nFrameW, DWORD nFrameH, BYTE nDummyVal);						// ������
		virtual BOOL Init(int nchannel, int const* board_id, int const* board_ch_id, const char* ini_filename, int nbuff, DWORD nFrameW, DWORD nFrameH, BYTE nDummyVal);	// ������
		virtual void Dispose();

		virtual bool		Start();									 	// ���s�ҋ@�ŃX���b�h�J�n
		virtual bool		Stop();											// �X���b�h��~


		// �B���n
		virtual BOOL		StartGrab();									// �B���J�n
		virtual BOOL		StopGrab();										// �B����~
		virtual bool		Reset();										// ���Z�b�g (Init����̏�Ԃɖ߂�)


		// �摜���
        virtual long		GetFrameWidth()     const { return NULL != my_pGrabber ? my_pGrabber->GetWidth() : 0; }
        virtual long		GetFrameHeight()    const { return NULL != my_pGrabber ? my_pGrabber->GetHeight() : 0; }
        virtual long		GetFrameNChannel()  const { return my_nChannel; }
        virtual long		GetFramePixelNBit() const { return NULL != my_pGrabber ? my_pGrabber->GetPixelNBit() : 0; }

		virtual int			GetChannel()	const {return my_nChannel;}


		virtual AvalACL2Base &GetBase( int i ) { return my_pGrabber[i]; }


		// �T���v���摜����n
		virtual bool		OpenBmp(char const* cPathFormat, int nNoFrom, int nNoTo, int nCycleTimer);	// �A���r�b�g�}�b�v��`
		virtual void		CloseBmp();											// �A���r�b�g�}�b�v���
		virtual bool		OpenAvi(char const* cPathFormat, int nCycleTimer);	// AVI��`
		virtual void		CloseAvi();											// AVI���
		virtual bool		OpenManual();
		virtual void		CloseManual();
		virtual bool		OnManualFrameGrabbed(BYTE* pImgs, DWORD nBytes, DWORD nCamset);
		virtual void		ChangeSmpPathFormat(int nMode = 0) { my_nSmpPathFormatMode = nMode;}	// �t�H�[�}�b�g�ؑ�
	protected:
		virtual void		OnBmpFrameGrabbed();							// �S�J�����Z�b�g�A�J�����p�x�ɑ΂��ăr�b�g�}�b�v�摜���Z�b�g
		virtual void		OnAviFrameGrabbed();							// �S�J�����Z�b�g�A�J�����p�x�ɑ΂���AVI�摜���Z�b�g


	protected:
		virtual void		InitInfo(DWORD nFrameW, DWORD nFrameH, BYTE nDummyVal);		// �������Z�b�g
		virtual void		DisConnect() {return;};							// ���񖳂�



	//// �v���p�e�B�֌W
	public:
		// Set
		virtual void		SetAddInfo(DWORD idx, HANDLE h) { my_hAddInfo[idx]=h; }	// �t���[�����ɕt�^������
		virtual void		SetImgTurn_LR(bool on)	  { my_bImgTurn_LR = on;  }		// �摜���E���]	 ��������
		


	//// �����o�[�֐�
	protected:
		virtual int			ThreadMainLoop();								// �X���b�h���C�����[�v���V���h�E
		virtual int			ThreadFirst();									// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
		virtual int			ThreadLast()				{ return 0;};		// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
		virtual void		ThreadEvent(int nEventNo)	{};					// �X���b�h�C�x���g����
		

		virtual void		OnFrameGrabbed(void** pImgs, DWORD nFrameNum, int* nSeqNum, __int64 nFastTime);	// 1�J�����Z�b�g���̉摜���擾���āA�X���b�h���[�v�L���[�ɃL���[�C���O
		virtual	void		OnGrabError(unsigned long frame_num, int grabber_index, AvalACL2Base::FrameWaitResult err);		// �G���[��`
		virtual void		OnAddInfo(EM_DELIVERY_ID em, DWORD nFrameNum, DWORD set, DWORD ang, DWORD st);	// �t�я��L���[�C���O



	//// �����o�[�ϐ�
	protected:

        // grabber base
        int								my_nGrabber;						// �J�����ڑ���
        AvalACL2Base*					my_pGrabber;						// �B���N���X [�z��]


		// ��{���
		EM_GRAB_MODE					my_emGrabMode;						// �B�����
        DWORD							my_nFrameNum;						// �t���[��No 
		DWORD							my_nImgSize;						// �t���[���T�C�Y (1�摜�݂̂̃T�C�Y)
		DWORD							my_nFrameW;							// ��   �t���[���摜�T�C�Y
		DWORD							my_nFrameH;							// ���� �t���[���摜�T�C�Y
		int								my_nChannel;						// �摜��

		DWORD							my_nBaseBufNum;						// �o�b�t�@��
		std::vector<DWORD>				my_nBatchSize;						// ���̉摜�擾��
		std::vector<DWORD>				my_nMaxBatchSize;					// �ő�摜�擾��


		void*							my_pDummyBuf;						// �t���[���摜�����Ă���ꍇ�Ȃǂ̃_�~�[�t���[���摜 (Init�Ő���)

		HANDLE							my_hAddInfo[MAX_CAMNUM];			// �t���[�����ɕt�^������
		bool							my_bImgTurn_LR;						// �摜���E���]�� true


		// ���C���X���b�h�֌W
		HANDLE							my_hHwRun;							// HW�B�����s��(=�摜�捞��)�͏�ɃV�O�i����ԁB �}�j���A������
		HANDLE							my_hInited;
		bool							my_bThreadLoop;						// ���C���X���b�h�̃��[�v�� true

		// �e�X�g�摜�p
		KizuAviManager*					mycls_Aiv[MAX_COLOR_CHANNEL][MAX_CAMNUM];		// AVI�t�@�C���ǂݍ��݃N���X
		HANDLE							my_evSmpImgTimer;					// AVI�摜��������ŉ摜��ǂݍ��ޏꍇ
		int								my_nSmpPathFormatMode;				// �Ǎ��݃t�H�[�}�b�g�^�C�v (0:�ʏ�orCH�őS�������摜 1:CH���ɕʉ摜)
		CString							my_sSmpPathFormat;					// �Ǎ��݃t�H�[�}�b�g
											// �ybmp���z
												// my_nSmpPathFormatMode=0 : �J�����Z�b�g(0�I���W��), �J�����p�x(0�I���W��), �A��
												// my_nSmpPathFormatMode=1 : �J�����Z�b�g(0�I���W��), �J�����p�x(0�I���W��), Ch(0�I���W��), �A��
											// �yavi���z
												// my_nSmpPathFormatMode=0 : �J�����Z�b�g(0�I���W��), �J�����p�x(0�I���W��)
												// my_nSmpPathFormatMode=1 : �J�����Z�b�g(0�I���W��), �J�����p�x(0�I���W��), Ch(0�I���W��)
		int								my_nSmpIntervalNo[2];				// �ǂݍ��݊J�nNo,�I��No
		int								my_nSmpNowNo;						// ���݂̓ǂݍ���
		int								my_nSmpCycleTimer;					// �ǂݍ��ݎ��� [ms]
		DWORD *m_nSeqNum;
	};
};