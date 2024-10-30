// *********************************************************************************
//	TCP/IP �ʐM �̊��N���X
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//		�g�p����^�X�N�� �wws2_32.lib�x �������N����K�v�������܂����B
//	[�ǉ����C�u�����t�@�C��]
//		ws2_32.lib
// *********************************************************************************

#pragma once

#include "ksControl.h"

namespace KizuSock3
{
	class SockBase  
	{
	public:
		SockBase(const char *cSession, const char *cIniPath, DWORD tid, DWORD timeout, DWORD client_num = 1,
				 DWORD r_size = 0, DWORD s_size = 0, DWORD msgbase = WM_USER);						// �R���X�g���N�^

		virtual ~SockBase();																		// �f�X�g���N�^

		void SetOpt_NewAcceptNgMode(int nMode)		{my_nNewAcceptNgMode=nMode;}

		//// ���b�Z�[�W�ʒm����C�x���g�ʒm�ɕύX
		void SetEvAccept(HANDLE evAccept)			{mycls_pKc->SetEvAccept(evAccept);};			// �A�N�Z�v�g�ʒm �C�x���g�ݒ� (ksThread�ɂ͊֌W�Ȃ�)
		// �S���̃\�P�b�g�ɑ΂��ē����V�O�i��
		void SetEvRecvTimeout(HANDLE evRecvTimeout) {mycls_pKc->SetEvRecvTimeout(evRecvTimeout);};	// ��M�^�C���A�E�g �C�x���g�ݒ�
		void SetEvSendTimeout(HANDLE evSendTimeout) {mycls_pKc->SetEvSendTimeout(evSendTimeout);};	// ���M�^�C���A�E�g �C�x���g�ݒ�
		void SetEvDisConnect(HANDLE evDisConnect)	{mycls_pKc->SetEvDisConnect(evDisConnect);};	// �ؒf�ʒm �C�x���g�ݒ�

		// �w��̃\�P�b�g�̂�
		void SetEvRecvTimeout(int id, HANDLE evRecvTimeout) { if(NULL != mycls_pKt[id]) mycls_pKt[id]->SetEvRecvTimeout(evRecvTimeout); }	// ��M�^�C���A�E�g �C�x���g�ݒ�
		void SetEvSendTimeout(int id, HANDLE evSendTimeout) { if(NULL != mycls_pKt[id]) mycls_pKt[id]->SetEvSendTimeout(evSendTimeout); }	// ���M�^�C���A�E�g �C�x���g�ݒ�
		void SetEvDisConnect(int id, HANDLE evDisConnect)   { if(NULL != mycls_pKt[id]) mycls_pKt[id]->SetEvDisConnect(evDisConnect); }	// �ؒf�ʒm �C�x���g�ݒ�


		// ����
		int Connect();									// �R�l�N�g (�ڑ������܂Ńu���b�N�����)
		int Listen();									// ���X�j���O (�m���u���b�N)
		int Accept();									// �A�N�Z�v�g (�V�O�i���p)
		int Accept(MSG *msg);							// �A�N�Z�v�g (���b�Z�[�W�p)
		void Close(WPARAM ksT); 							// ����̃N���C�A���g��؂藣�� (�|�C���^�w��)
		void Close(ksThread* ksT);
		void CloseID(int id);							// ����̃N���C�A���g��؂藣�� (�z��ʒu�w��)
		void CloseAll();								// �S�ẴN���C�A���g��؂藣��
		void Cancel();									// ���݂̓ǂݍ��݃L�����Z������֐�

		int Recv(LPVOID pData, DWORD len, UINT ksT, HANDLE evRecvEnd = NULL);				// ��M�J�n�˗� (�|�C���^�w��)
		int Recv(LPVOID pData, DWORD len, ksThread* ksT, HANDLE evRecvEnd = NULL);		
		int Recv(int id, LPVOID pData, DWORD len, HANDLE evRecvEnd = NULL);					// ��M�J�n�˗� (�z��ʒu�w��)

		int Send(LPCVOID pData, DWORD len, UINT ksT, HANDLE evSendEnd = NULL);				// ���M�J�n�˗� (�|�C���^�w��)
		int Send(LPCVOID pData, DWORD len, ksThread* ksT, HANDLE evSendEnd = NULL);		
		int Send(int id, LPCVOID pData, DWORD len, HANDLE evSendEnd = NULL);				// ���M�J�n�˗� (�z��ʒu�w��)

		void SetKsThPriority(int nPriority) { if(NULL!=mycls_pKc) mycls_pKc->SetKsThPriority(nPriority); }	// ksThread�̗D��x��ݒ�

		// GET
		int GetSockID(WPARAM ksT) const;				// �w�肳�ꂽ�|�C���^�ƈ�v����z��ʒu��Ԃ�
		int GetSockID(ksThread* ksT) const;
		int GetFreeID() const;							// �����蓖�Ă̔z��ʒu��Ԃ�
		int GetActive() const;							// �������Ă���q�X���b�h�̐���Ԃ�
		int GetCloseKindID(int id) const;				// �\�P�b�g�̏I���敪��Ԃ�

	protected:
		ksControl *mycls_pKc;							// �\�P�b�g����N���X
		ksThread  *mycls_pKt[KS_MAXCLIENTS];			// �e�N���C�A���g�p�\�P�b�g�|�C���^ (id)

		long	  my_nClientAddr[KS_MAXCLIENTS];		// �N���C�A���g��IP�A�h���X(id)
		DWORD	  my_nClientNum;						// �N���C�A���gMAX�� (�T�[�o�[�Ȃ�ڑ����萔���B�N���C�A���g�Ȃ�ʏ�1)

		int		  my_nNewAcceptNgMode;					// �V�KAccept��t�ُ펞�̋����𑀍�
														//   �n�[�t�R�l�N�V�������̕ێ�IP�ƐV�K�R�l�N�gIP�������Ƃ��̋���
															// 0:�]���ǂ���B���C�u�������ŐV���Z�b�V����������������
															// 1:�̂̃Z�b�V����������I�ɐؒf�B�ŁA����̂�V�K�Ŏ󂯕t����B
	};
};