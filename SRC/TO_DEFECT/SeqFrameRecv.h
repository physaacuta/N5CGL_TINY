// �V�[�P���T�ʐM UDP��M��p�N���X (1��1) �N���C�A���g

#pragma once

// �r���N���X
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[

// ���Œ�
#include "..\..\Include\LineCommon.h"									// �r���C���N���[�h

// ���[�J���̃N���X
#include "MainConst.h"
#include "StatusManager.h"

using namespace KizuLib;

class SeqFrameRecv : public ThreadManager
{
// ���[�J���萔
protected :
	//static const int PARAM_TIMEOUT_HELCY_RECV		= (10*1000);		// �w���V�[�Ď��^�C�}�A�E�g [ms]
	static const int PARAM_TIMEOUT_HELCY_RECV		= (20*1000);		// �w���V�[�Ď��^�C�}�A�E�g [ms]
	static const int PARAM_BUFSIZE					= 256;				// �o�b�t�@�T�C�Y


public:
	SeqFrameRecv();
	virtual ~SeqFrameRecv();

	//=========================================
	// �v���p�e�B
	void	SetLogSeqMgr(LogFileManager* pCls)	{ mcls_pLogSeq = pCls;}	// �V�[�P���T��M�p���O�N���X
	void	SetStatusMgr(StatusManager* pCls)	{ mcls_pStatus = pCls;};// �X�e�[�^�X����N���X�Z�b�g


	// ����
	void	SetRun( bool bMode) {m_bRun = bMode;}						// �����J�n�A��~
	bool	GetRun()			{ return m_bRun; }
	bool	GetSt()				{ return m_bSt; }
	void	SetDebugWpd()		{ m_bDebugWpd = true;}


	//=========================================
	// �C�x���g���J
	ThreadQueue<SEQ_TYPE_DATA_FRAME>*	GetQueFrame()	{ return &mque_Frame;}	// �t���[���f�[�^��n�p�L���[

	// �Q�Ɨp
	SEQ_TYPE_DATA_FRAME&	GetLastFrame() { return m_typLastFrame; }	


//// �����o�[�֐�
private :
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����

	bool RecvCheck();													// ��M�f�[�^�̃`�F�b�N
	bool AddNewFrame();													// �V�K�t���[������M
	void OnErr(bool bManyu);											// �ُ�ʒm
	void QueAllDelete();												// ���ׂẴL���[���폜

	// TPC/IP �ʐM����
	bool Recving();														// �Ɩ��`���̎�M���J�n


//// �����o�[�ϐ�
private :

	//=========================================
	// �e�C���X�^���X
	LogFileManager*			mcls_pLogSeq;								// �V�[�P���T�p���O�N���X
	StatusManager*			mcls_pStatus;								// �X�e�[�^�X����N���X

	// �X���b�h�L���[
	ThreadQueue<SEQ_TYPE_DATA_FRAME>	mque_Frame;						// OUT �t���[���f�[�^��n�p�L���[


	// �C�x���g�n���h��
	HANDLE					my_tmHelcy;									// �w���V�[�Ď��^�C�}�C�x���g


	// �\�P�b�g & ���M�f�[�^�֘A
	SOCKET					m_nSock;									// UDP��M�\�P�b�g
	OVERLAPPED				m_ovlRecv;									// �񓯊���M�I�[�o�[���b�v	
	BYTE					my_RecvBuf[PARAM_BUFSIZE];					// ��M�f�[�^
	bool					my_bHelcyOk;								// ��������`����M�����̂ō���̃w���V�[�`�F�b�N�̓���
	struct sockaddr_in		my_fromAddr;								// ��M���A�h���X
	int						my_fromLen;									// ��M���A�h���X��

	// ���̑��������
	bool					m_bSt;										// �w���V�[���펞 true
	bool					m_bRun;										// ������ true
	SEQ_TYPE_DATA_FRAME		m_typLastFrame;								// ��M�����Ō�̃t���[�����
	bool					m_bDebugWpd;								// �f�o�b�N�B����WPD�F��
};



