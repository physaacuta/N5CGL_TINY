// *********************************************************************************
//	UDP���M�N���X
//	[Ver]
//		Ver.01    2014/06/11  ���ō쐬�J�n
//		Ver.02	  2015/01/16�@4PL�W�J�p�ɃN���X����ʉ�
//
//	[����]
//   �Ӄ֊�{�@�\�̂݃I�����C���@�`�@4PL�Ǝ��@�\���I�����C���܂ł̊ԁA
//   �����p���R�[�_�[�ɃR�C�����𑗐M����N���X�B
//   �ŏI�I�ɂ́A���̋@�\��OFF�ɂȂ�B
// *********************************************************************************
#pragma once

#include <winsock2.h>														// UDP���M�p

// ���Œ�
#include "..\..\Include\LineCommon.h"										// �r���C���N���[�h
#include "..\..\Include\ToHtCommon.h"										// EPC�\�t�g �� ���� �� ���� �@�\ �֘A�w�b�_�[
//#include "..\..\Include\EPC\NCcommand.h"									// �摜����\�t�gI/F�w�b�_
#include "..\..\Include\EPC\NCL2DETECT_if.h"								// I/F��`

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"					// ���[�J�[�X���b�h�}�l�[�W���[

// ���[�J���̃N���X
#include "MainConst.h"

using namespace KizuLib;

class CmdSender : public ThreadManager
{



////=========================================
//// ���J�\����
public:
	struct	SEND_UDP{
		int		nTorB;														// ���M��̃|�[�g�i0:�\�p�A1:���p�A-1:���ʁj
		int		nPcId;														// ���M���PCID(0�I���W��)�@-1���A�u���[�h�L���X�g
		int		nSize;														// ���M�f�[�^�̃T�C�Y
		int		nCommand;													// ���M�R�}���h
		char	data[1024];													// ���M�f�[�^
	};

////=========================================
//// ���J�֐�
public:
	// �R���X�g���N�^�E�f�X�g���N�^
	CmdSender(void);
	virtual ~CmdSender(void);

	// �X���b�h�L���[
	ThreadQueue<SEND_UDP> gque_SendUdp;										// UDP���M�f�[�^

	// PFC�\�t�g�ɃR�}���h���M(���N���X����N��)
	void SendCommand_Start(int TorB = -1);									// ���R�[�_�[�֌����J�n�R�}���h�ʒm
	void SendCommand_Stop(int TorB = -1);									// ���R�[�_�[�֌�����~�R�}���h�ʒm
	void SendCommand_Param(int TorB, int nscode);							// ���R�[�_�[�֌����\�ʏ�ԕύX�R�}���h�ʒm
	void SendCommand_CoilInf(int TorB, TYPE_EPC_COIL typData);				// ���R�[�_�[�փR�C������ʒm

////=========================================
//// �����o�[�֐�
private:
	void	ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	int		ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int		ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)

	// UDP�ʐM�֌W
	void	HtUdp_Send(int TorB, char* cData, int nSize, int nCommand);		// �u���[�h�L���X�g���M 
	void	HtUdp_Send(int TorB, int nPcNo, char* cData, int nSize, int nCommand);	// ���j�L���X�g���M

	// �P�i����
	CString	GetCmdName(int nCommand);										// �R�}���h���̂��擾


////=========================================
//// �����o�[�ϐ�
private :

	// ���R�[�_�[ UDP���M�p
	SOCKET			m_SockHtSend;											// UDP���M�\�P�b�g
	sockaddr_in		m_AddrHtSend[NUM_MEN];									// ���M���A�h���X(0:�\�p�A1:���p)

	//// ���̑�
	char			m_cUniIp[NUM_SENDER][32];								// ���R�[�_�[ ���j�L���X�g���M�p��IP�A�h���X
};
