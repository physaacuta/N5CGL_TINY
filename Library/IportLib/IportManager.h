#pragma once


//======================================================================
// �������J�����捞�́Apitch��8�����l���Ă��Ȃ�
//   ����Ă�����AConnect �ŃG���[�ɂ���
//======================================================================
#pragma warning(disable : 4819)								// Unicode�ŕۑ�

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


// ���[�J��
#include "GrabberManager.h"													// �B�����N���X
#include "IportBase.h"														// iPort���N���X


// �C���|�[�g
using namespace KizuLib;
using namespace ImgLib;


namespace IportLib
{

	//===================================================================
	// �J�����捞 I/F
	//===================================================================
	interface IportManagerIF
	{

	//// ���J�֐�
	public:

		// ���̂�
		virtual bool		Init(DWORD nFrameW, DWORD nFrameH, DWORD camset=1, DWORD angle=1, BYTE nDummyVal=0x80) = 0;	// ������
	

		// iPort����n
		virtual IportLib::ERRCODE	Connect(DWORD set, DWORD ang, CString sXmlPath, 
										bool bSaveIpAddr=true, bool bIsSlave=false, long nAdapterIndex=0, 
										int nGrabQueSize=IportBase::GRAB_DEF_QSIZE) = 0;	// iPort�ɐڑ� (1p) �ݒ�:XML, Master:XML
		virtual IportLib::ERRCODE	Connect(DWORD set, DWORD ang, CString sXmlPath, 
										const char* mac_addr, const char* ip_addr, char const* mc_addr, 
										bool bSaveIpAddr=true, bool bIsSlave=false, long nAdapterIndex=0, 
										int nGrabQueSize=IportBase::GRAB_DEF_QSIZE) = 0;	// iPort�ɐڑ� (1p) �ݒ�:XML, Master:IP��


		virtual IportLib::ERRCODE	Connect(DWORD set, DWORD ang, CString sXmlPath, long nAdapterIndex1,
										const char* mac_addr2, const char* ip_addr2, long nAdapterIndex2, char const* mc_addr2,
										bool bSaveIpAddr=true, bool bIsSlave=false, int nGrabQueSize=IportBase::GRAB_DEF_QSIZE) = 0;	// iPort�ɐڑ�(2p) �ݒ�:XML, Master:XML, Slave:IP��
		virtual IportLib::ERRCODE	Connect(DWORD set, DWORD ang, CString sXmlPath,
									    const char* mac_addr1, const char* ip_addr1, long nAdapterIndex1, char const* mc_addr1,
										const char* mac_addr2, const char* ip_addr2, long nAdapterIndex2, char const* mc_addr2,
										bool bSaveIpAddr=true, bool bIsSlave=false, int nGrabQueSize=IportBase::GRAB_DEF_QSIZE) = 0;	// iPort�ɐڑ�(2p) �ݒ�:XML, Master:IP��, Slave:IP��


		//virtual void		DisConnect() = 0;									// iPort�Ɛؒf


				// ���̑�����
		virtual bool		SendSerialTemp(DWORD set, DWORD ang, float& temp) = 0;	// �J�������x���擾 (�J�����Ɛڑ��������_��)
		virtual void		SyncFrame(DWORD set, DWORD ang, WORD nSkipCnt) =0;


	//// �v���p�e�B�֌W
	public:
		// �I�u�W�F�N�g�擾

		//(IportBase&)*mycls_pIport[0] ���킩��Ȃ��l����
		//		mycls_pIport[0] �́AIportBase�̃|�C���^�������Ă���
		//		*mycls_pIport[0]�ŁAIportBase�̖{�̂ɃA�N�Z�X
		//		�����(IportBase&)�Ō^�ϊ�
		virtual inline IportBase&		GetClsIpot(DWORD set, DWORD ang, bool isPrimary=true) = 0;
		virtual inline int				GetCamIndex(DWORD set, DWORD ang) const = 0;						// �S��0�I���W��
		virtual inline bool IsPT2000() const = 0;


		// Set
		virtual void		SetResetModuleOnReset(bool on) = 0;
		virtual void		SetAddInfo(DWORD set, DWORD ang, HANDLE h) = 0;	// �t���[�����ɕt�^������
		virtual void		SetImgTurn_LR(bool on) = 0;						// �摜���E���]
		// Get
		virtual void*		GetDummyBuf() = 0;

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
		//virtual bool		IsInit()			 const = 0;
		//virtual bool		IsGrabRun()			 const = 0;
	};


	//===================================================================
	// �J�����捞 ���ʕ��{��
	//===================================================================
	class IportManager :	public		GrabberManager,
							implements	IportManagerIF
	{

	//// �����o�[�萔
	protected:
		static const int	LOCK_QUE_RETRY_CNT	= 4;						// ���b�N���L���[���擾�����ꍇ�̃��g���C��
		static const int	MAX_IPORT_CHANNEL	= MAX_COLOR_CHANNEL;	// 3		// �ő�S�F�`�����l��
		static const int	MAX_IPORT_CAMANGLE	= 4;						// �ő�S�p�x
		static const int	MAX_IPORT_CAMSET	= 8;						// �ő�W�J�����Z�b�g
		static const int	MAX_IPORT_CAMNUM	= MAX_IPORT_CAMSET*MAX_IPORT_CAMANGLE;	// �ő�J�����ڑ���
		

	//// ���J�֐�
	public:
		IportManager(CString sMyManagerName);
		virtual ~IportManager(void);

		//----------------------------------------
		// grab ���� �������ł͂��Ȃ��B����Ɍp����Ŏ���
		virtual bool		Start() = 0;									// �X���b�h�J�n �� ���̃X���b�h�́A�z���g�ɃX���b�h�����s�����܂Ń��b�N�����
		virtual bool		Stop() = 0;										// �X���b�h��~
	

		//----------------------------------------
		// �T���v���摜����n ����
	public:	
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





		// ���̑�����
		virtual void		SyncFrame(DWORD set, DWORD ang, WORD nSkipCnt) { int ch=GetCamIndex(set, ang); my_nSkipCnt[ch]+=nSkipCnt;}


	//// �v���p�e�B�֌W
	public:
		//virtual inline		ThreadLoopQueue<FRAME_DATA>* GetQueFrame()	   { return myque_pFrame; }
		//virtual inline		ThreadQueue<DELIVERY_INFO>&	 GetQueInfo()	   { return myque_Info; }


		// Set
		virtual void		SetResetModuleOnReset(bool on) { my_reset_module = on; }
		virtual void		SetAddInfo(DWORD set, DWORD ang, HANDLE h) { DWORD ch=GetCamIndex(set,ang); my_hAddInfo[ch]=h; }	// �t���[�����ɕt�^������
		virtual void		SetImgTurn_LR(bool on)	  { my_bImgTurn_LR = on; }			// �摜���E���]	
//		virtual void		SetCamsetNo(DWORD set, DWORD ang, DWORD no){ DWORD ch=GetCamIndex(set,ang); my_nCamsetNo[ch]=no; }	// �J�����Z�b�gNo(0�I���W��)
		// Get
		virtual void*		GetDummyBuf()			  { return my_pDummyBuf; }
//		virtual int			GetCamsetNo(DWORD set, DWORD ang) { DWORD ch=GetCamIndex(set,ang); return my_nCamsetNo[ch]; }

		// �X�e�[�^�X
		virtual DWORD		GetResendTotal()	const { return my_nResendTotal; }
		virtual DWORD		GetLostTotal()		const { return my_nLostTotal;}
		virtual DWORD		GetQueueCount()		const { return my_nQueueCount;}
		virtual DWORD		GetMaxQueueCount()	const { return my_nMaxQueueCount;}
		virtual DWORD		GetFrameNo()		const { return my_nFrameNum;}
		virtual int			GetChannel()		const { return my_nChannel;}
		//virtual bool		IsInit()			const { return my_bInit;}
		//virtual bool		IsGrabRun()			const { return my_bGrabRun;}

	//// �����o�[�֐�
	protected:
		virtual void		OnAddInfo(EM_DELIVERY_ID em, DWORD nFrameNum, DWORD set, DWORD ang, DWORD st);	// �t�я��L���[�C���O


	//// �����o�[�ϐ�
	protected:

		//// �N���X
		//LogFileManager					mcls_LogMe;							// ���[�J����p
	
		//// Que
		//ThreadLoopQueue<FRAME_DATA>*	myque_pFrame;						// �t���[���摜�󂯓n���L���[(��]���i�`��) (�O������C���X�^���X�����炤)
		//ThreadQueue<DELIVERY_INFO>		myque_Info;							// �t�����󂯓n���L���[ (�����ŃC���X�^���X�����)

		// ��{���
		//EM_GRAB_MODE					my_emGrabMode;						// �B�����
		DWORD							my_nCamsetNum;						// ��������
		DWORD							my_nAngleNum;						// �p�x��
		DWORD							my_nFrameNum;						// �t���[��No 
		DWORD							my_nResendTotal;					// �p�P�b�g�đ���
		DWORD							my_nLostTotal;						// �p�P�b�g������
		DWORD							my_nQueueCount;						// �捞�҂����Ă���t���[����
		DWORD							my_nMaxQueueCount;					// �捞�҂����Ă���t���[����
		DWORD							my_nImgSize;						// �t���[���T�C�Y (1�摜�݂̂̃T�C�Y)
		DWORD							my_nFrameW;							// ��   �t���[���摜�T�C�Y
		DWORD							my_nFrameH;							// ���� �t���[���摜�T�C�Y
		int								my_nChannel;						// �摜��
		WORD							my_nSeqNum[MAX_IPORT_CAMNUM];		// �����Ŏ����Ă���V�[�P���X�ԍ� (iPort�ԍ��ƈ�v����悤�ɂ��Ă���) (start���̃V�[�P���X�ԍ��ŏ���������)
		//bool							my_bInit;							// Init�ς݂�true
		//bool							my_bGrabRun;						// Grabbing��true

		bool							my_reset_module;					// Reset()����ResetModule���ĂԂ��ǂ��� (SpyderGigE�̎��Ɏg�p�BPiranha2�Ŏg�p������_�����ۂ�)
		void*							my_pDummyBuf;						// �t���[���摜�����Ă���ꍇ�Ȃǂ̃_�~�[�t���[���摜 (Init�Ő���)

		DWORD							my_nCamsetNo[MAX_IPORT_CAMNUM];		// �J�����Z�b�g(0�I���W��)
		HANDLE							my_hAddInfo[MAX_IPORT_CAMNUM];		// �t���[�����ɕt�^������
		WORD							my_nSkipCnt[MAX_IPORT_CAMNUM];		// �w�薇�������X�L�b�v

		bool							my_bImgTurn_LR;						// �摜���E���]�� true

		// ���C���X���b�h�֌W
		HANDLE							my_hThreadReady;					// �z���g�ɃX���b�h�����s����܂őҋ@�p
		HANDLE							my_hThreadGoStop;					// ���s�\��(=�摜�捞��)�͏�ɃV�O�i����ԁB �}�j���A������
		bool							my_bThreadLoop;						// ���C���X���b�h�̃��[�v�� true

		// �e�X�g�摜�p
		KizuAviManager*					mycls_Aiv[MAX_IPORT_CHANNEL][MAX_IPORT_CAMNUM];		// AVI�t�@�C���ǂݍ��݃N���X
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
	};
};
