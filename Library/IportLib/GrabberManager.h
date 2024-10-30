#pragma once

//======================================================================
// �B�����N���X
//======================================================================


// �r�����C�u����
#include "..\KizuLib\CLASS\KizuMacro.h"										// �r���}�N��
#include "..\KizuLib\Class\KizuPerformanceCounter.h"						// �^�C�}�[

#include "..\KizuLibMFC\CLASS\ThreadManager.h"								// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\KizuLibMFC\Class\LogFileManager.h"								// ���O�Ǘ��N���X
#include "..\KizuLibMFC\CLASS\ThreadLoopQueue.h"							// ��]���i�`���̃X���b�h�L���[


#include "..\KizuMMX\MemCopy.h"												// MMX���C�u����

#include "..\..\Include\LibSelect.h"										// ���C�u�����ؑփw�b�_�[



// �e�X�g�摜�擾�p
#include "..\ImgLib\CLASS\KizuAviManager.h"									// AVI�t�@�C��
#include "..\KizuLib\CLASS\KizuBitmapManager.h"								// BitMap�t�@�C��

//
#include "IportCommon.h"													// iPort�p�萔
#include "IGrabber.h"														// �B��I/F



// �C���|�[�g
using namespace KizuLib;




//===================================================================
// �}�N��
#define LOM(t) mcls_LogMe.Write(KizuLib::LogFileManager::t			// ���[�J����p���O



//===================================================================
// �{��
namespace IportLib
{
	class GrabberManager :	public ThreadManager,
							implements IGrabber,
							implements IGrabberParam,
							implements IGrabberDummy
	{

	//===================================================================
	// �Œ�萔
	public:
		static const int	MAX_COLOR_CHANNEL	= 3;						// �ő�S�F�`�����l��


		//// �B�����
		enum EM_GRAB_MODE {
			GRAB_MODE_HW = 0,												// �n�[�h���g�����I�����C�����[�h
			GRAB_MODE_IPORT = GRAB_MODE_HW,									// iPort (�n�[�h���g�����I�����C��) ���̂̌݊��̂��߂Ɏc���Ă���

			// �f�o�b�N���
			GRAB_MODE_BMP,													// Bitmap�t�@�C��
			GRAB_MODE_AVI,													// AVI�t�@�C��
			GRAB_MODE_MAN													// �}�j���A���Z�b�g
		};

		//// Gragger���
		enum EM_GRAB_TYPE {
			GRAB_TYPE_IPORT = 0,											// iPort
			GRAB_TYPE_AVAL													// AvalACL2
		};

		//// �ڑ��p�����[�^
		struct CONNECT_PARAM {
		};

	protected:
		enum EM_SMPPATHFROMAT {
			SMPPATHFROMAT_NOM = 0,											// 0:�ʏ�orCH�őS�������摜 
			SMPPATHFROMAT_CHN												// 1:CH���ɕʉ摜
		};


	//===================================================================
	// ���J
	public:
		GrabberManager(EM_GRAB_TYPE emType, CString sMyManagerName="GrabberMgr");
		virtual ~GrabberManager(void);


		//----------------------------------------
		// ����
		virtual EM_GRAB_TYPE	IsGrabType() const { return my_emGrabType; }


		//----------------------------------------
		// �Ǝ�I/F
		// Grabber����
//		virtual BOOL		Connect( CONNECT_PARAM* prm );
		virtual void		DisConnect() = 0;								// iPort�Ɛؒf

		//----------------------------------------
		//�������ł͂��Ȃ��B����Ɍp����Ŏ����B  ��������Ă����Ȃ��ƁAGrabberManager�^�Ƀ|�C���^���ꂽ�Ƃ��ɁA�����Ȃ�ThreadManager��Start()�ɂ����Ă��܂�
		virtual bool		Start() = 0;									// �X���b�h�J�n �� ���̃X���b�h�́A�z���g�ɃX���b�h�����s�����܂Ń��b�N�����
		virtual bool		Stop() = 0;										// �X���b�h��~
	


		// �I�u�W�F�N�g�ݒ�
		virtual void		SetQueFrame(ThreadLoopQueue<FRAME_DATA>* pQue) { myque_pFrame = pQue;}
		virtual void		SetCamsetNo( int camset ){ m_ncamset = camset ; }


		// �I�u�W�F�N�g�擾
		virtual inline ThreadLoopQueue<FRAME_DATA>*	GetQueFrame()	{ return myque_pFrame; }
		virtual inline ThreadQueue<DELIVERY_INFO>&	GetQueInfo()	{ return myque_Info; }

		// �X�e�[�^�X
		virtual bool		IsInit()			const { return my_bInit;}
		virtual bool		IsGrabRun()			const { return my_bGrabRun;}
		virtual int			GetChannel()		const = 0;
		virtual int			GetCamset()			const { return m_ncamset; } 
		virtual void		SetParam( const char* id, const void *param ) {}

	//// �����o�[�ϐ�
	protected:

		// �N���X
		LogFileManager					mcls_LogMe;							// ���[�J����p
	
		// Que
		ThreadLoopQueue<FRAME_DATA>*	myque_pFrame;						// �t���[���摜�󂯓n���L���[(��]���i�`��) (�O������C���X�^���X�����炤)
		ThreadQueue<DELIVERY_INFO>		myque_Info;							// �t�����󂯓n���L���[ (�����ŃC���X�^���X�����)


		// ��{���
		EM_GRAB_TYPE					my_emGrabType;						// Grabber �̎�� (iPort,Aval)
		EM_GRAB_MODE					my_emGrabMode;						// �B�����

		int								m_ncamset;

		// �X�e�[�^�X
		bool							my_bInit;							// Init�ς݂�true
		bool							my_bGrabRun;						// Grabbing��true

	};
};