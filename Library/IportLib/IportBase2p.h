#pragma once

// ���[�J��
#include "IportBase.h"														// iPort���N���X


// �C���|�[�g
using namespace KizuLib;
using namespace IportLib;

namespace IportLib
{
	class IportBase2p
	{
	//// ���J�֐�
	public:
		IportBase2p(CString sThreadName ="iPortBase2p");
		virtual ~IportBase2p(void);

        // ������
        bool			Init();												// �֐�������
        virtual void	Dispose();											// �֐��J��

		inline IportBase&  GetBase(int n) { return mycls_Iport[n]; }		// Base�擾

		//// iPort����n
        ERRCODE			SetupFromXMLFile(const char* filename1, const char* filename2, 
										 long nAdapterIndex1=0, long nAdapterIndex2=0, 
										 bool bSaveIpAddr=true, bool bIsSlave=false, int nGrabQueSize=IportBase::GRAB_DEF_QSIZE);
		ERRCODE			SetupFromXMLFile(const char* filename, 
										 const char* mac_addr1, const char* ip_addr1, long nAdapterIndex1, char const* mc_addr1,
										 const char* mac_addr2, const char* ip_addr2, long nAdapterIndex2, char const* mc_addr2,
										 bool bSaveIpAddr=true, bool bIsSlave=false, int nGrabQueSize=IportBase::GRAB_DEF_QSIZE);
		ERRCODE			SetupFromXMLFile(const char* filename, long nAdapterIndex1,
										 const char* mac_addr2, const char* ip_addr2, long nAdapterIndex2, char const* mc_addr2,
										 bool bSaveIpAddr=true, bool bIsSlave=false, int nGrabQueSize=IportBase::GRAB_DEF_QSIZE);


		//// �J��������
		// (1) �f�o�C�X�̑I��
		// (2) �J�����̑I��
		// (3) �f�o�C�X�ւ̐ڑ����J������bind
		bool			IsConnected() const { return mycls_Iport[0].IsConnected(); }			// �ڑ��m�F
		bool			IsMulticastSlave() const { return mycls_Iport[0].IsMulticastSlave(); }	// �}�X�^�[or�X���[�u (FALSE:master or non  TRUE:multicast)
		// (4) grabber�̐ݒ�
		
		// (5) grab�̐���
		ERRCODE			StartGrab();										// �J�����捞�J�n
		ERRCODE			StopGrab();											// �J�����捞��~
		bool			IsStarted() const { return mycls_Iport[0].IsStarted(); }				// �J�����捞�m�F

		// (6) �f�o�C�X�̐ؒf
		ERRCODE			Disconnect();										// �f�o�C�X�̐ؒf	
		// (7) grab�����摜�̎�舵��
		ERRCODE			LockFrame(void*& p0, unsigned long& len0, unsigned long& iid0, unsigned long& stat0,
								  void*& p1, unsigned long& len1, unsigned long& iid1, unsigned long& stat1);		// �摜�ǂݍ���(�摜��荞�݊����܂Ńu���b�L���O)

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
		void			SetSyncPort(int nSkipCnt) { my_nSkipPort=nSkipCnt;}	// �Y���␳����

		// (8) Queue�̑���
		void			ClearQueueItemInBuffer() { mycls_Iport[0].ClearQueueItemInBuffer(); mycls_Iport[1].ClearQueueItemInBuffer(); }
		unsigned long	GetNQueueItemsInBuffer(bool isPrimary=true) const { return mycls_Iport[ (isPrimary?0:1) ].GetNQueueItemsInBuffer(); }

		// (9) Reset
        ERRCODE			ResetModule();
		

		// (10) �`���n (���ׂă}�X�^�[�ɑ΂��Ď��{)
		ERRCODE			SendSerialCommand(char const* sSend, CString* sRecv, int timeout=40, bool bDsp=true) { return mycls_Iport[0].SendSerialCommand(sSend, sRecv, timeout, bDsp); }	// �V���A���R�}���h ���M��A�A���T�[��M
		ERRCODE			SendSerialCommand(char const* sSend, int timeout=40, bool bDsp=true) { return mycls_Iport[0].SendSerialCommand(sSend, timeout, bDsp); }							// �V���A���R�}���h ���M�̂�


	//// �v���p�e�B�֌W
	public:
		// �O������̃C���X�^���X
		void			SetLogMgr(LogFileManager* p) { mycls_Iport[0].SetLogMgr(p); mycls_Iport[1].SetLogMgr(p); };	// ���O�t�@�C���Z�b�g
		char const*		GetThreadName()				 {return mycls_Iport[0].GetThreadName();}		// ���X���b�h��


		// �摜���
        inline long		GetFrameWidth()     const { return mycls_Iport[0].GetFrameWidth()+mycls_Iport[1].GetFrameWidth(); }
        inline long		GetFrameHeight()    const { return mycls_Iport[0].GetFrameHeight(); }
        inline long		GetFrameNChannel()  const { return mycls_Iport[0].GetFrameNChannel(); }
        inline long		GetFramePixelNBit() const { return mycls_Iport[0].GetFramePixelNBit(); }


		// grab�X�e�[�^�X
		unsigned long	GetResendCount()	const { return mycls_Iport[0].GetResendCount() + mycls_Iport[1].GetResendCount();}
		unsigned long	GetLostCount()		const { return mycls_Iport[0].GetLostCount() + mycls_Iport[1].GetLostCount();}
	//	bool			IsWaitingForLock()	const { return my_bLockWaiting; }	


	//// ���[�J���萔
	protected:
        IportBase	mycls_Iport[2];											// iPort�x�[�X�N���X�{�́B (0:�}�X�^�[  1:�X���[�u�|�[�g)
        ERRCODE     my_lock_err[2];											// LockFrame�̃G���[
	
		// �t���[����������
		int			my_nSkipPort;											// 1�J��������iPort�V�[�P���XNo�Y���␳�p (0:���� 1:�}�X�^�[��1��X�L�b�v -1:�X���[�u��1��X�L�b�v)
	};
};