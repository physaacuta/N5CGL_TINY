// *********************************************************************************
//	iPort�ڑ� ��� �N���X
//	[Ver]
//		Ver.01    2010/01/29  vs2005 �Ή�
//
//	[����]
//		
// *********************************************************************************
#pragma once

#include <Windows.h>

// Pleora �w�b�_�[
	// �K�{�̃w�b�_�[�̂�
	// .c �ŕK�v�ȃw�b�_�[�� *.c�Œ�`
#include <CyTypes.h>
#include <CyGrabber.h>


// �r�����C�u����
#include "..\KizuLib\CLASS\KizuMacro.h"										// �r���}�N��
#include "..\KizuLibMFC\Class\LogFileManager.h"								// ���O�Ǘ��N���X

#include "IportCommon.h"													// iPort�p�萔

// �C���|�[�g
using namespace KizuLib;

namespace IportLib
{
	//===================================================================
	// �萔��`
	//===================================================================

	// /////
    // �A�N�Z�X���[�h
    // /////
    enum ACCESSMODE {	ModeUnknown = 0,
						PerfDrv, 
						WinStackUDP, 
						WinStackTCP, 
						FilterDrv
					};
	// /////
	// �G���[�R�[�h(���ۂ�CyResult)
	// /////
	#define CYEXT_RESULT_BASE	(CY_RESULT_MAX)
	enum ERRCODE {		NoErr = CY_RESULT_OK,				// 0�Œ�B  �v���O��������0=����ŕ����Ă悵
						ApErr = -1,
						// �ȉ�swdetect�ł̊g��
						CYEXT_RESULT_IMAGE_CORRUPTED = CYEXT_RESULT_BASE,
						CYEXT_RESULT_BAD_IMAGE
				};
	#define CAST_CY_ERR(errcode)	static_cast<ERRCODE>(errcode)		// �G���[�R�[�h�ϊ�
	#define RETURN_CY_ERR(errcode)	return CAST_CY_ERR(errcode)			// �G���[�R�[�h�ϊ��� return



	//===================================================================
	// ���N���X
	//===================================================================
	class IportBase
	{
	//// �萔
	public:
		static const int GRAB_DEF_QSIZE = 4;								// �O���u�̃����O�o�b�t�@�����T�C�Y

		struct	TYP_FRAME_INFO {											// �摜��� 
			long				nWidth;											// ��	(XML�Ŏ擾)
			long				nHeight;										// ����	(XML�Ŏ擾)
			long				nNChannel;										// �摜�̃`�����l���� (�O���[�摜=1,RGB�J���[=3 �ȂǂȂ�)
			long				nPixelNbit;										// 1��f�̃r�b�g�� (�ʏ� 8bit)
		};


	//// ���[�J���萔
	protected:
		static const int MC_CONNECT_RETRY_COUNT = 10;						// �R�l�N�g���g���C��

	    struct	TYP_DEVICE_ENTRY {											// �J�����ڑ��ݒ�
            BOOL				valid;											// �L����� (�f�[�^�Z�b�g������ true)
			IportLib::ACCESSMODE emMode;										// �J�����ڑ��^�C�v
            unsigned char		adapter_mac[6];
            char				mac[32];										// ������
            char				ip[32];											// ������
            unsigned char		id;												// Device ID
																					// 0: Gigabit Ethernet IP engines
																					// 1: Fast Ethernet (10/100 Megabits/sec) IP engines
            unsigned char		module_id;										// iPORT  IP �G���W���̃^�C�v
																					// 0x01: Camera Link module
																					// 0x02: Video Decoder module
																					// 0x04: No module (streaming module)
																					// 0x05: Symagery SC3000 module
            unsigned char		smodule_id;										// iPORT  IP �G���W���̃T�u�^�C�v
        };


	//// ���J�֐�
	public:
		IportBase(CString sThreadName ="iPortBase");
		virtual ~IportBase(void);

        // ������
        bool			Init();												// �֐�������
        virtual void	Dispose();											// �֐��J��


		//// XML�t�@�C�����g���� (1)�`(4)�܂ł����{
		static ERRCODE	GetXMLFileToIP(const char* filename, CString* ip_addr, CString* mac_addr, CString* mc_addr);		// iPort�ݒ�XML�t�@�C������ IP�A�h���X�֌W���擾
        ERRCODE			SetupFromXMLFile(const char* filename, 
										 bool bSaveIpAddr=true, bool bIsSlave=false, long nAdapterIndex=0, int nGrabQueSize=GRAB_DEF_QSIZE);	// iPort�ݒ�XML�t�@�C�����g�p���āA�J�����ɐڑ�
		ERRCODE			SetupFromXMLFile(const char* filename, 
										 const char* mac_addr, const char* ip_addr, char const* mc_addr, 
										 bool bSaveIpAddr=true, bool bIsSlave=false, long nAdapterIndex=0, int nGrabQueSize=GRAB_DEF_QSIZE);	// iPort�ݒ�XML�t�@�C�����g�p���āA�J�����ɐڑ�

	protected:
		ERRCODE			SetupBuffer(int nGrabQueSize=GRAB_DEF_QSIZE);		// �J�����摜�o�b�t�@ �� iPort��荞�݉摜�̈���m��

	public:
		//// �J��������
		// (1) �f�o�C�X�̑I��
		// (2) �J�����̑I��
		// (3) �f�o�C�X�ւ̐ڑ����J������bind
		bool			IsConnected() const;								// �ڑ��m�F
		bool			IsMulticastSlave() const;							// �}�X�^�[or�X���[�u (FALSE:master or non  TRUE:multicast)
		// (4) grabber�̐ݒ�
		
		// (5) grab�̐���
		ERRCODE			StartGrab();										// �J�����捞�J�n
		ERRCODE			StopGrab();											// �J�����捞��~
		bool			IsStarted() const;									// �J�����捞�m�F
		// Grab();								// One�V���b�g��荞�݂͖�����

		// (6) �f�o�C�X�̐ؒf
		ERRCODE			Disconnect();										// �f�o�C�X�̐ؒf	
		// (7) grab�����摜�̎�舵��
		ERRCODE			LockFrame(void*& p, unsigned long& len, unsigned long& iid, unsigned long& status);		// �摜�ǂݍ���(�摜��荞�݊����܂Ńu���b�L���O)

			// status��舵��
		static const unsigned long MaskFrameOverrun       = 0x00000001;
		static const unsigned long MaskGrabberFIFOOverrun = 0x00000002;
		static const unsigned long MaskImageDropped       = 0x00000004;
		static const unsigned long MaskPartialLineMissing = 0x00000008;
		static const unsigned long MaskFullLineMissing    = 0x00000010;

        static inline bool IsFrameOverrun(unsigned long status)			{ return !!(status & MaskFrameOverrun);	}		// ���̂܂܂��� unsigned long �������I�� bool�ɕϊ��Ń��[�j���O���o�邩��A2�d�ے�ŋ����I��bool�ɕϊ�
        static inline bool IsGrabberFIFOOverrun(unsigned long status)	{ return !!(status & MaskGrabberFIFOOverrun);}
        static inline bool IsImageDropped(unsigned long status)			{ return !!(status & MaskImageDropped); }
        static inline bool IsPartialLineMissing(unsigned long status)	{ return !!(status & MaskPartialLineMissing); }
        static inline bool IsFullLineMissing(unsigned long status)		{ return !!(status & MaskFullLineMissing); }
        static inline bool IsLineMissing(unsigned long status)			{ return !!(status & (MaskPartialLineMissing | MaskFullLineMissing)); }
        

		void			ReleaseFrame() const;								// �摜�ǂݍ��ݗ̈�ԋp
		void			AbortLock() const;									// LockForRead���狭���I�ɔ�����
	
		// (8) Queue�̑���
        void			ClearQueueItemInBuffer();
        unsigned long	GetNQueueItemsInBuffer() const;

        // (9) Reset
        ERRCODE			ResetModule();
		

		// (10) �`���n
		ERRCODE			SendSerialCommand(char const* sSend, CString* sRecv, int timeout=40, bool bDsp=true );	// �V���A���R�}���h ���M��A�A���T�[��M
		ERRCODE			SendSerialCommand(char const* sSend, int timeout=40, bool bDsp=true);					// �V���A���R�}���h ���M�̂�


	//// �v���p�e�B�֌W
	public:
		// �O������̃C���X�^���X
		void			SetLogMgr(LogFileManager* p) { mcls_pLog = p; };						// ���O�t�@�C���Z�b�g
		char const*		GetThreadName()				 {return my_sThreadName.GetBuffer();}		// ���X���b�h��

		// Cy�I�u�W�F�N�g�擾
		inline CyGrabber*         GetCyGrabber()	const { return my_pCyGrabber; }
		inline CyImageBuffer*     GetCyImageBuffer()const { return my_pCyBuffer; }
		inline CyCameraInterface* GetCyCamera()		const { return my_pCyCamera; }


		// �J�����ڑ����
        inline BOOL IsDeviceEntryValid()	const { return my_typDevEntry.valid; }
        inline const char* GetDeviceIP()	const { return my_typDevEntry.ip; }
        inline const char* GetDeviceMAC()	const { return my_typDevEntry.mac; }

		// �摜���
		TYP_FRAME_INFO const& GetFrameInfo()const { return (TYP_FRAME_INFO&)my_typFrameInfo;}
        inline long		GetFrameWidth()     const { return my_typFrameInfo.nWidth; }
        inline long		GetFrameHeight()    const { return my_typFrameInfo.nHeight; }
        inline long		GetFrameNChannel()  const { return my_typFrameInfo.nNChannel; }
        inline long		GetFramePixelNBit() const { return my_typFrameInfo.nPixelNbit; }

		// grab�X�e�[�^�X
		unsigned long	GetResendCount()	const { return my_nResendCount;}
		unsigned long	GetLostCount()		const { return my_nLostCount;}
		bool			IsWaitingForLock()	const { return my_bLockWaiting; }



	//// �����o�[�ϐ�
	protected:

		// Pleora�N���X
		CyGrabber*				my_pCyGrabber;								// �O���o�ڑ� �C���X�^���X
        CyImageBuffer*			my_pCyBuffer;								// �J�����摜�o�b�t�@ �C���X�^���X
        CyCameraInterface*		my_pCyCamera;								// �J������� �C���X�^���X (���̒���CyGrabber�������Ă���)

		// �e�C���X�^���X
		LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X

		// �J�����ڑ����
		TYP_DEVICE_ENTRY		my_typDevEntry;								// �J�����ڑ��ݒ�
		char					my_cCameraName[128];						// �J�����@�� (Standard CameraLink Camera �Ȃ�)

		// �摜���
		TYP_FRAME_INFO			my_typFrameInfo;							// �摜���

		// grab���
		unsigned long			my_nResendCount;							// �p�P�b�g�đ��� (�����I�Ȃ�Ȃ̂ŁA�đ����Đ�������ꍇ������)
		unsigned long			my_nLostCount;								// �p�P�b�g������ (LockForRead�Œx��Ď�荞�߂Ȃ�������ł͂Ȃ��̂Œ���)
		bool					my_bLockWaiting;							// LockForRead��:true
		bool					my_bIsSlave;								// �X���[�u���s�� true
		///////bool					my_bIsLock;									// LockForRead���튮����`SignalReadEnd������܂ł̊�
		//bool					my_bIsFreeRun;								// �t���[������:true (SetupFromXMLFile�O�ɃZ�b�g���Ă�������)
		//int						my_nFreeRunHz;								// ��荞�ގ��g�� [Hz]

		// ���̑��������
		CString					my_sThreadName;								// ���X���b�h��
		
	};
};