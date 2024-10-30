// *********************************************************************************
//	SOCKET����E�ڑ��N���X
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//		�g�p����^�X�N�� �wws2_32.lib�x �������N����K�v�������܂����B
//	[�ǉ����C�u�����t�@�C��]
//		ws2_32.lib
// *********************************************************************************

#pragma once

#pragma warning(disable : 4311)							// �x�����b�Z�[�W 4311 �� 1 �񂾂��o��
#pragma warning(disable : 4996)							// ***���Â��`���Ƃ��Đ錾����܂����B���o���Ȃ��΍�

#include "ksThread.h"

namespace KizuSock3
{
	class ksControl  
	{
	public:

		ksControl(DWORD tid, DWORD msgbase);								// �R���X�g���N�^
		virtual ~ksControl();												// �f�X�g���N�^

		// ����
		int SockOpen(DWORD rcvBufSz, DWORD sndBufSz);						// �\�P�b�g���J��
		int	Connect(ksThread **newInst);									// �ڑ����Ďq�X���b�h���쐬����i���C���X���b�h�͂Ȃ��j
		int Listen(int backlog);											// �񓯊��ł����ɖ߂�B���ۂɂȂ���ƃ��b�Z�[�W��������
		int CloseChild(ksThread *pChild);									// �ؒf���ĉ��

		// GET
		int IsValidChild(UINT id) const;									// �q�X���b�h���L�邩
		int	GetActive() const;												// �������Ă���q�X���b�h�̐���Ԃ�
		int GetInstCount() const {return m_nInstCount;};					// ���������q�X���b�h�̐�
		void GetAcceptEventMsg(MSG* msg);									// �A�N�Z�v�g�V�O�i���ʒm����MSG�����擾 (�A�N�Z�v�g�V�O�i���ʒm���̂ݎg�p)

		// SET
		void SetAddr(const char *ip, short port);							// IP�A�|�[�g�ݒ�
		void SetTimeout(DWORD timeout);										// �^�C���A�E�g�Đݒ�
		void SetEvRecvTimeout(HANDLE evRecvTimeout);						// ��M�^�C���A�E�g �C�x���g�ݒ�
		void SetEvSendTimeout(HANDLE evSendTimeout);						// ���M�^�C���A�E�g �C�x���g�ݒ�
		void SetEvDisConnect(HANDLE evDisConnect);							// �ؒf�ʒm �C�x���g�ݒ�
		void SetEvAccept(HANDLE evAccept);									// �A�N�Z�v�g�ʒm �C�x���g�ݒ� (KizuSock3�Ŏg����悤�ɂ����B�V�O�i���ʒm��K��GetAcceptEventMsg�R�[�����鎖)
		void SetKsThPriority(int nPriority) { m_nKsThPriority=nPriority; }	// ksThread�̗D��x��ݒ�

	private:
		static unsigned int __stdcall MainThread(LPVOID arg);				// Listen�p���C���X���b�h
		void	sendMsg(UINT msg, WPARAM wParam, LPARAM lParam);			// ���b�Z�[�W�E�C�x���g �ʒm
		//friend static void sendMsg(ksControl *p, UINT msg, WPARAM wParam, LPARAM lParam);


		DWORD	m_nTid;						// �ʒm�� (PostThreadMessage�̑����)
		DWORD	m_nMsgBase;					// ���b�Z�[�W�x�[�X�ԍ�
		SOCKET	m_sock;						// �ŏ��ɍ�����\�P�b�g

		HANDLE	m_hThread;					// ���C���X���b�h�n���h��
		DWORD	m_nTimeout;					// �^�C���A�E�g�̒l
		CRITICAL_SECTION	m_cs;			// m_nInstCount�̂��߂̃N���e�B�J���Z�N�V����

		// �\�P�b�g����N���X
		ksThread*	mcls_pInst[KS_MAXCLIENTS];		// �q�X���b�h�������e����z��
		DWORD		m_nInstCount;					// �������ꂽ�q�X���b�h�̐�
		char		m_Ip[20];				// IP�A�h���X
		short		m_nPort;				// �|�[�g�ԍ�
		MSG			m_LastAcceptMsg;		// �Ō�ɃA�N�Z�v�g���ꂽ��� (�A�N�Z�v�g�V�O�i���ʒm���̂ݎg�p)
		int			m_nKsThPriority;		// �q�X���b�h(���ۂ̑���M�X���b�h)�̗D��x

		// ���X���b�h����p�V�O�i��
		HANDLE	m_evStopEvent;				// ��~�C�x���g
		HANDLE	m_evAccepting;				// accept�Ŏq�X���b�h���ǉ����ꂽ�Ƃ��̃C�x���g
		HANDLE  m_evAcceptEndCollBack;		// �A�N�Z�v�g�ʐM������A�����������ɃV�O�i�� (�A�N�Z�v�g�V�O�i���ʒm���̂ݎg�p)

		// �C�x���g�ʒm�p�V�O�i��
		HANDLE m_evRecvTimeout;				// ��M�^�C���A�E�g �C�x���g
		HANDLE m_evSendTimeout;				// ���M�^�C���A�E�g �C�x���g
		HANDLE m_evDisConnect;				// �ؒf�ʒm �C�x���g
		HANDLE m_evAccept;					// �A�N�Z�v�g�ʒm �C�x���g
	};
};