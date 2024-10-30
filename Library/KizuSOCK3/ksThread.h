// *********************************************************************************
//	SOCKET�ʐM���N���X
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//		�g�p����^�X�N�� �wws2_32.lib�x �������N����K�v�������܂����B
//	[�ǉ����C�u�����t�@�C��]
//		ws2_32.lib
// *********************************************************************************

#pragma once

#include <windows.h>
#include <winsock2.h>

#pragma warning( disable : 4312 )        // �x�����b�Z�[�W 4312 �� 1 �񂾂��o��

namespace KizuSock3
{
	//------------------------------------------
	// �萔
	//------------------------------------------
	#define	KS_MAXCLIENTS	6 //4				// ��̃\�P�b�g�̓����ő�ڑ��� (����PC�������ɂȂ������炱���OK)
	#define	KS_SNDBUF		512*1024		// �\�P�b�g�̃f�t�H���g�̑��M�o�b�t�@�T�C�Y
	#define	KS_RCVBUF		512*1024		// �\�P�b�g�̃f�t�H���g�̎�M�o�b�t�@�T�C�Y

	// �X���b�h�̃��^�[���R�[�h
	#define	THR_NORMAL_END	0
	#define	THR_RECV_ERR	1
	#define	THR_SEND_ERR	2
	#define	THR_CLOSE_END	3

	// ���b�Z�[�W
	// ���ۂɂ�msgbase�����Z����đ�����
	#define	WM_KS_ACCEPT		1					// �A�N�Z�v�g����
	#define	WM_KS_RECV			2					// ��M����
	#define	WM_KS_SEND			3					// ���M����
	#define	WM_KS_DISCONNECT	4					// �ؒf���ꂽ
	#define	WM_KS_RECV_TIMEOUT	5					// ��M�^�C���A�E�g
	#define	WM_KS_SEND_TIMEOUT	6					// ���M�^�C���A�E�g

	// ���b�Z�[�W��wParam�ɂ̓\�P�b�g��ID������
	// lParam�ɂ�windows�̃G���[�R�[�h�܂��͂O������

	// �֐��G���[�R�[�h
	#define	KIZUSOCK_INVALID_HND		(-8)
	#define KIZUSOCK_INVALID_ID			(-9)
	#define	KIZUSOCK_ERROR				(-3)

	// �\�P�b�g�̏I�������敪
	#define KIZUSOCK_CLOSEKIND_NON		0			// �I������������������s��
	#define KIZUSOCK_CLOSEKIND_SOCK		1			// �\�P�b�g���ؒf���ꂽ (= ���肩��̐ؒf)
	#define KIZUSOCK_CLOSEKIND_PROG		2			// �ُ팟�o�����ׁA�v���O�����Őؒf
	#define KIZUSOCK_CLOSEKIND_THEAD	3			// �X���b�h�I��
	#define KIZUSOCK_CLOSEKIND_CANCEL	4			// �L�����Z���Őؒf

	// �X�e�[�^�X�r�b�g
	#define KIZUSOCK_STAT_ACTIVE		0x01L		// �X���b�h�������Ă�����
	#define KIZUSOCK_STAT_WAIT_RECV		0x02L		// �f�[�^�ǂݍ��ݒ�
	#define KIZUSOCK_STAT_WAIT_SEND		0x04L		// �f�[�^���M��
	#define KIZUSOCK_STAT_THREAD_ERR	0x80000000L	// ���炩�̃G���[�ŃX���b�h���I��
	#define KIZUSOCK_STAT_RECV_TIMEOUT	0x100L		// ��M���^�C���A�E�g���Ă���
	#define	KIZUSOCK_STAT_SEND_TIMEOUT	0x200L		// ���M���^�C���A�E�g���Ă���

	// �X�e�[�^�X����
	#define	STAT_ACTIVE(x)	((x) & KIZUSOCK_STAT_ACTIVE)
	#define	SET_ACTIVE(x) ((x) |= KIZUSOCK_STAT_ACTIVE)
	#define	RST_ACTIVE(x) ((x)&= ~KIZUSOCK_STAT_ACTIVE)

	#define	STAT_WAIT_RECV(x)	((x) & KIZUSOCK_STAT_WAIT_RECV)
	#define	SET_WAIT_RECV(x) ((x)|= KIZUSOCK_STAT_WAIT_RECV)
	#define	RST_WAIT_RECV(x) ((x) &= ~KIZUSOCK_STAT_WAIT_RECV)

	#define	STAT_WAIT_SEND(x)	((x) & KIZUSOCK_STAT_WAIT_SEND)
	#define	SET_WAIT_SEND(x) ((x) |= KIZUSOCK_STAT_WAIT_SEND)
	#define	RST_WAIT_SEND(x) ((x) &= ~KIZUSOCK_STAT_WAIT_SEND)

	#define	STAT_THREAD_ERR(x)	((x) & KIZUSOCK_STAT_THREAD_ERR)
	#define	SET_THREAD_ERR(x) ((x) |= KIZUSOCK_STAT_THREAD_ERR)
	#define	RST_THREAD_ERR(x) ((x) &= ~KIZUSOCK_STAT_THREAD_ERR)

	#define	STAT_RECV_TIMEOUT(x)	((x) & KIZUSOCK_STAT_RECV_TIMEOUT)
	#define	SET_RECV_TIMEOUT(x) ((x) |= KIZUSOCK_STAT_RECV_TIMEOUT)
	#define	RST_RECV_TIMEOUT(x) ((x) &= ~KIZUSOCK_STAT_RECV_TIMEOUT)

	#define	STAT_SEND_TIMEOUT(x)	(x & KIZUSOCK_STAT_SEND_TIMEOUT)
	#define	SET_SEND_TIMEOUT(x) (x |= KIZUSOCK_STAT_SEND_TIMEOUT)
	#define	RST_SEND_TIMEOUT(x) (x &= ~KIZUSOCK_STAT_SEND_TIMEOUT)

	//------------------------------------------
	// �N���X
	//------------------------------------------
	class ksThread  
	{
	public:
		ksThread(SOCKET sock, DWORD timeout, DWORD tid, DWORD msgbase, 
				HANDLE evRecvTimeout, HANDLE evSendTimeout, HANDLE evDisconnect);			// �R���X�g���N�^
		virtual ~ksThread();																// �f�X�g���N�^
		int Start(int nPriority = THREAD_PRIORITY_NORMAL);									// �X���b�h�J�n
		void Stop();																		// �X���b�h��~

		// ����
		int Recv(LPVOID pData, DWORD len);													// ��M�J�n���� (���C���X���b�h�̎�M�J�n��������������܂Ńu���b�N)			
		int Recv(LPVOID pData, DWORD len, HANDLE evEvent);									// ��M�J�n���� (���C���X���b�h�̎�M�J�n��������������܂Ńu���b�N)
		int Send(LPCVOID pData, DWORD len);													// ���M�J�n���� (���C���X���b�h�̑��M�J�n��������������܂Ńu���b�N)
		int Send(LPCVOID pData, DWORD len, HANDLE evEvent);									// ���M�J�n���� (���C���X���b�h�̑��M�J�n��������������܂Ńu���b�N)
		void Cancel();																		// ����M�҂��������

		// SET
		void SetTimeout(DWORD timeout);														// �^�C���A�E�g�Đݒ�
		void SetEvRecvTimeout(HANDLE evRecvTimeout) {m_evRecvTimeout = evRecvTimeout; };	// ��M�^�C���A�E�g �C�x���g�ݒ�
		void SetEvSendTimeout(HANDLE evSendTimeout) {m_evSendTimeout = evSendTimeout; };	// ���M�^�C���A�E�g �C�x���g�ݒ�
		void SetEvDisConnect(HANDLE evDisConnect)   {m_evDisConnect  = evDisConnect; };		// �ؒf�ʒm �C�x���g�ݒ�

		// GET
		HANDLE GetThreadHandle() const {return m_hThread;};									// �X���b�h�n���h��
		DWORD  GetStat()		 const {return m_stat;};									// �X�e�[�^�X
		DWORD  GetCloseKind()	 const {return m_nCloseKind;};								// �\�P�b�g�̏I���敪

	private:
		static unsigned int __stdcall MainThread(LPVOID arg); 								// ���C���X���b�h
		void	sendMsg(UINT msg, WPARAM wParam, LPARAM lParam);							// ���b�Z�[�W�E�C�x���g �ʒm


		DWORD	m_nTid;						// �ʒm�� (PostThreadMessage�̑����)
		DWORD	m_nMsgBase;					// ���b�Z�[�W�x�[�X�ԍ�
		HANDLE	m_sock;						// accept�œ���ꂽ�\�P�b�g�܂��͎�\�P�b�g

		HANDLE	m_hThread;					// ���C���X���b�h�n���h��
		DWORD	m_nTimeout;					// �^�C���A�E�g�̒l
		DWORD	m_stat;						// �X�e�[�^�X
		DWORD	m_apierr;					// GetLastError�̒l
		DWORD	m_nCloseKind;				// �\�P�b�g�̏I���敪

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
		HANDLE m_evRecvEnd;					// ��M�����ʒm �C�x���g				
		HANDLE m_evSendEnd;					// ���M�����ʒm �C�x���g
		HANDLE m_evRecvTimeout;				// ��M�^�C���A�E�g �C�x���g
		HANDLE m_evSendTimeout;				// ���M�^�C���A�E�g �C�x���g
		HANDLE m_evDisConnect;				// �ؒf�ʒm �C�x���g
	};
};
