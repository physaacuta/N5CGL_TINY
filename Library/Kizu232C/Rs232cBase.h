// *********************************************************************************
//	RS-232C�ʐM �N���X
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//		
// *********************************************************************************

#pragma once

#include <windows.h>

#pragma warning(disable : 4996)							// ***���Â��`���Ƃ��Đ錾����܂����B���o���Ȃ��΍�

namespace Kizu232C
{
	//------------------------------------------
	// �萔
	//------------------------------------------
	// ���b�Z�[�W
	// ���ۂɂ�msgbase�����Z����đ�����
	#define	WM_KS232C_RECV			1
	#define	WM_KS232C_SEND			2
	#define	WM_KS232C_ERR			3
	#define	WM_KS232C_RECV_TIMEOUT	4
	#define	WM_KS232C_SEND_TIMEOUT	5

	// �X�e�[�^�X�r�b�g
	#define KIZU232C_STAT_ACTIVE		0x01L			// �X���b�h�������Ă�����
	#define KIZU232C_STAT_WAIT_RECV		0x02L			// �f�[�^�ǂݍ��ݒ�
	#define KIZU232C_STAT_WAIT_SEND		0x04L			// �f�[�^���M��
	#define KIZU232C_STAT_THREAD_ERR	0x80000000L		// ���炩�̃G���[�ŃX���b�h���I��
	#define KIZU232C_STAT_RECV_TIMEOUT	0x100L			// ��M���^�C���A�E�g���Ă���
	#define	KIZU232C_STAT_SEND_TIMEOUT	0x200L			// ���M���^�C���A�E�g���Ă���

	// �X�e�[�^�X����
	#define	STAT_ACTIVE(x)	((x) & KIZU232C_STAT_ACTIVE)
	#define	SET_ACTIVE(x) ((x) |= KIZU232C_STAT_ACTIVE)
	#define	RST_ACTIVE(x) ((x)&= ~KIZU232C_STAT_ACTIVE)

	#define	STAT_WAIT_RECV(x)	((x) & KIZU232C_STAT_WAIT_RECV)
	#define	SET_WAIT_RECV(x) ((x)|= KIZU232C_STAT_WAIT_RECV)
	#define	RST_WAIT_RECV(x) ((x) &= ~KIZU232C_STAT_WAIT_RECV)

	#define	STAT_WAIT_SEND(x)	((x) & KIZU232C_STAT_WAIT_SEND)
	#define	SET_WAIT_SEND(x) ((x) |= KIZU232C_STAT_WAIT_SEND)
	#define	RST_WAIT_SEND(x) ((x) &= ~KIZU232C_STAT_WAIT_SEND)

	#define	STAT_THREAD_ERR(x)	((x) & KIZU232C_STAT_THREAD_ERR)
	#define	SET_THREAD_ERR(x) ((x) |= KIZU232C_STAT_THREAD_ERR)
	#define	RST_THREAD_ERR(x) ((x) &= ~KIZU232C_STAT_THREAD_ERR)

	#define	STAT_RECV_TIMEOUT(x)	((x) & KIZU232C_STAT_RECV_TIMEOUT)
	#define	SET_RECV_TIMEOUT(x) ((x) |= KIZU232C_STAT_RECV_TIMEOUT)
	#define	RST_RECV_TIMEOUT(x) ((x) &= ~KIZU232C_STAT_RECV_TIMEOUT)

	#define	STAT_SEND_TIMEOUT(x)	(x & KIZU232C_STAT_SEND_TIMEOUT)
	#define	SET_SEND_TIMEOUT(x) (x |= KIZU232C_STAT_SEND_TIMEOUT)
	#define	RST_SEND_TIMEOUT(x) (x &= ~KIZU232C_STAT_SEND_TIMEOUT)


	//------------------------------------------
	// �N���X
	//------------------------------------------
	class Rs232cBase
	{
	public:
		
		Rs232cBase(const char *cSession, const char *cIniPath, DWORD tid,
				   DWORD r_size = 0, DWORD s_size = 0, DWORD msgbase = WM_USER, bool bSoftFolwOff = false);		// �R���X�g���N�^
		virtual ~Rs232cBase(void);														// �f�X�g���N�^

		// ����
		int Recv(LPVOID pData, DWORD len);												// ��M�J�n���� (���C���X���b�h�̎�M�J�n��������������܂Ńu���b�N)
		int Recv(LPVOID pData, DWORD len, HANDLE evEvent);
		int Send(LPCVOID pData, DWORD len);												// ���M�J�n���� (���C���X���b�h�̑��M�J�n��������������܂Ńu���b�N)
		int Send(LPCVOID pData, DWORD len, HANDLE evEvent);	
		void Cancel();																	// ����M�҂��������

		// SET
		void SetTimeout(DWORD timeout);													// �^�C���A�E�g�Đݒ�
		void SetRecvEndCR(char cr) { m_cRecvCR = cr; }									// ��M�������fCR (1�o�C�g�Âǂݍ��݁ACR�Ɠ����������� ��M�����Ƃ���)

		// GET
		DWORD GetQueueLen() const;														// �ʐM�G���[�̏��m�F

	private:
		static unsigned int __stdcall MainThread(LPVOID arg);							// ���C���X���b�h
		void	sendMsg(UINT msg, WPARAM wParam, LPARAM lParam);						// ���b�Z�[�W�E�C�x���g �ʒm


		DWORD	m_nTid;						// �ʒm�� (PostThreadMessage�̑����)
		DWORD	m_nMsgBase;					// ���b�Z�[�W�x�[�X�ԍ�

		HANDLE	m_hCom;						// RS-232C �ʐM����n���h��
		HANDLE	m_hThread;					// ���C���X���b�h�n���h��
		DWORD	m_nTimeout;					// �^�C���A�E�g�̒l
		DWORD	m_stat;						// �X�e�[�^�X
		DWORD	m_apierr;					// GetLastError�̒l
		char	m_cRecvCR;					// ��M�������fCR

		// ����M�p �o�b�t�@
		char	*m_pRecvData;				// IN:��M�w�b�_�̃|�C���^
		DWORD	m_nRecvSz;					// IN:��M�f�[�^�̃T�C�Y
		const char	*m_pSendData;			// IN:���M�w�b�_�̃|�C���^
		DWORD	m_nSendSz;					// IN:���M�f�[�^�̃T�C�Y

		// ���X���b�h����p�V�O�i��
		HANDLE	m_evStopEvent;				// IN:��~�C�x���g
		HANDLE	m_evSetTimeout;				// IN:�^�C���A�E�g�Đݒ�
		HANDLE	m_evCancelEvent;			// IN:CancelIo���s						(����M�҂��������)
		HANDLE	m_evRecvStart;				// IN:���[�h���s����					(��M�J�n�������Ă˗v��)
		HANDLE	m_evSendStart;				// IN:�f�[�^���M						(���M�J�n�������Ă˗v��)
		HANDLE	m_evRecvAck;				// OUT:��M���X���b�h�Ɏ󂯕t����ꂽ	(��M�J�n����������ʒm)
		HANDLE	m_evSendAck;				// OUT:���M���X���b�h�Ɏ󂯕t����ꂽ	(���M�J�n����������ʒm)

		// �C�x���g�ʒm�p�V�O�i��
		HANDLE	m_evRecvEnd;				// ��M�����ʒm �C�x���g				
		HANDLE	m_evSendEnd;				// ���M�����ʒm �C�x���g
	};
};


