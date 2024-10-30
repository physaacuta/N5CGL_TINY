// *********************************************************************************
//	�G�b�W���o�͈͎w�� UDP���M�N���X
//	[Ver]
//		Ver.01    2017/08/01  ���ō쐬�J�n
//
// *********************************************************************************
#pragma once

#include <winsock2.h>														// UDP���M�p

// ���Œ�
#include "..\..\Include\LineCommon.h"										// �r���C���N���[�h
#include "..\..\Include\ToHtCommon.h"										// EPC�\�t�g �� ���� �� ���� �@�\ �֘A�w�b�_�[
#include "..\..\Include\EPC\NCL2DETECT_if.h"								// I/F��`

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"					// ���[�J�[�X���b�h�}�l�[�W���[

// ���[�J���̃N���X
#include "MainConst.h"
#include "ParamManager.h"													// �p�����[�^�Ǘ�
#include "EdgePosControl.h"													// �G�b�W���o�������Ǘ��N���X

using namespace KizuLib;

class EdgeRestrictSender : public ThreadManager
{

////=========================================
//// �v���p�e�B
public:
	//// ���[�N�X���b�h
	void SetParamMgr(ParamManager* pCls)		{ mcls_pParam = pCls; }					// �p�����[�^�Ǘ��N���X�Z�b�g
	void SetStatusMgr(StatusManager* pCls)		{ mcls_pStatus = pCls ;}				// �X�e�[�^�X���Ǘ��N���X

////=========================================
//// ���J�֐�
public:
	// �R���X�g���N�^�E�f�X�g���N�^
	EdgeRestrictSender(void);
	virtual ~EdgeRestrictSender(void);

	// �X���b�h�L���[
	ThreadQueue<NCL_EDGE_RESTRICT_DATA> gque_SendUdp;									// UDP���M�f�[�^

	// PFC�\�t�g�ɃR�}���h���M(���N���X����N��)
	void SendEdgeRestrict(int nFrame, int* nUseLimit, float* dEdge_Left, float* dEdge_Right);	// �摜�����G���W���փG�b�W�ʒu��񑗐M

	NCL_EDGE_RESTRICT_DATA&	GetLastSendData() { return m_strLastSendData; };// �ŏI���M�f�[�^�擾

////=========================================
//// �����o�[�֐�
private:
	void	ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	int		ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int		ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)

////=========================================
//// �����o�[�ϐ�
private :

	//=========================================
	// �e�C���X�^���X
	ParamManager*			mcls_pParam;									// �p�����[�^�Ǘ��N���X
	StatusManager*			mcls_pStatus;									// �X�e�[�^�X���Ǘ��N���X

	// �G�b�W���o�͈͎w�� UDP���M�p
	SOCKET			m_SockHtSend;											// UDP���M�\�P�b�g
	sockaddr_in		m_AddrHtSend;											// ���M���A�h���X

	NCL_EDGE_RESTRICT_DATA	m_strLastSendData;								// �ŏI���M�f�[�^
};
