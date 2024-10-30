// *********************************************************************************
//	TCP/IP �ʐM ��PC�ԒʐM�N���X �P�Α� (Send:�N���C�A���g   Recv:�T�[�o�[)
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//		������M�͕K�v�Ȃ����Ȃ̂ŁA�������Ƃ���
// *********************************************************************************

#pragma once
// ���Œ�
#include "..\..\Include\KizuCommon.h"									// �r���C���N���[�h
#include "..\KizuLibMFC\CLASS\ThreadManager.h"							// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\KizuLibMFC\Class\ThreadLoopQueue.h"
#include "..\KizuLib\Class\KizuPerformanceCounter.h"					// �^�C�}�[
#include "..\KizuSOCK3\SockBase.h"										// �\�P�b�g����N���X

#include "SockOneToAny.h"

using namespace KizuLib;
using namespace KizuSock3;

class SockOneToAnyRecv : public ThreadManager
{

//// �萔
public:
	// �{�N���X�̃\�P�b�g��ԃX�e�[�^�X
	enum SOCK_STATE { 
		STATE_NONE = 0,													// ������� (�ؒf��)
		STATE_IDLE,														// �������� (�ڑ���)
		STATE_HEAD,														// �w�b�_�� ���M��
		STATE_DATA														// �f�[�^�� ���M��
	};

	enum DATA_SUB_MODE {
		STATE_SUB_NONE = 0,
		STATE_SUB_HEAD_EX,
		STATE_SUB_ADDR0,
		STATE_SUB_ADDR1
	};


// ���[�J���萔
protected :
	static const int PARAM_SOCK_MAX					= KS_MAXCLIENTS;	// �ő�ڑ��l���Ȃ�
	static const int PARAM_SOCK_TIMEOUT				= 10000;			// �\�P�b�g �^�C���A�E�g
	static const int PARAM_SOCK_RETRY_TIME			= 5000;				// �\�P�b�g�Đڑ����� [ms]
	static const int PARAM_SOCK_RECV_BUFSIZE		= (4*1024*1024);	// LAN�{�[�h�̑��M�o�b�t�@�T�C�Y (4M) 4M��MAX�H�H �������̑���M�����֌W�Ȃ��Ƃ̎�

	static const int PARAM_TIMEOUT_HELCY_RECV		= (5*60*1000);		// �w���V�[�`����M�^�C�}�A�E�g [ms] (5��)
//	static const int PARAM_TIMEOUT_HELCY_RECV		= (10*1000);		// �w���V�[�`����M�^�C�}�A�E�g [ms] (5��)

public:
	SockOneToAnyRecv(void);
	virtual ~SockOneToAnyRecv(void);


	void	SetSession(const char *cSession ){ my_cSession = cSession; }	// �ڑ��Z�b�V�����Z�b�g 
	void	SetTLQue( ThreadLoopQueue<SOCK_F_HEAD2>* p ) { mque_pDeli = p; }


	//// �O���A�N�Z�X
	bool IsConnect(int id)  const { return STATE_NONE == my_emSockState[id] ? false :true; }	// �ڑ����
	// Get
	SOCK_STATE  GetSockState(int id)		const {return my_emSockState[id]; };					// ���݂̃X�e�[�^�X
	CString		GetSockStateName(int id)	const {return GetSockStateName(my_emSockState[id]);};	// ���݂̃X�e�[�^�X����
	CString		GetSockStateName(SOCK_STATE st)	const;									// �w��̃X�e�[�^�X����

	void		SetEvCloseReq()		{ SetEvent(my_evCloseReq); }		// �S�N���C�A���g�ؒf�v���V�O�i��
	
	// TPC/IP �ʐM����
	void Close(int id);													// �N���C�A���g�ؒf
	void CloseAll(bool bErr=false);										// �S�N���C�A���g�ؒf & �ُ�ʒm



//// �����o�[�֐�
protected:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����

	// TPC/IP �ʐM����
	void Reset();														// ���Z�b�g
	BOOL Listen(bool bLogDsp);											// Listen
	int  Accept();														// Accept (�V�O�i���ʒm�p)
	void ErrCall();														// �ُ�ʒm�̂� (����ؒf�͂��Ȃ�)
	BOOL RecvHead(int id);												// �w��̃w�b�_�[��M���J�n
	BOOL RecvData(int id);												// �Ɩ��`���̃f�[�^����M���J�n
	int RecvCheck(int id);												// ��M�f�[�^�̃`�F�b�N

	void Recv_Helcy();													// �w���V�[�`����M���� (�w�b�_�[���̂�)

	// ���̑�
	bool GetFreeQue(int id);	// �V�K�L���[�擾
	void AddPoolQue(int id);	// ��M�L���[�̓o�^


protected :
	// �e�C���X�^���X
	SockBase*				mcls_pSock;									// �\�P�b�g����N���X�C���X�^���X
	KizuPerformanceCounter	mcls_Time;									// �擾��������(�X�g�b�v�E�H�b�`�@�\���g�p)
	
	// ��n�L���[
	ThreadLoopQueue<SOCK_F_HEAD2>*		mque_pDeli;						// ����f�[�^�󂯓n���L���[(��]���i�`��)	[LIB��AP��]

	// �C�x���g�n���h��
	HANDLE					my_tmHelcy;									// �w���V�[���M�^�C�}�C�x���g

	HANDLE					my_evAccept;								// �ePC���Accept��
	HANDLE					my_evDisConnect[PARAM_SOCK_MAX];			// �ؒf�C�x���g�F��(ID)
	HANDLE					my_evRecvTimeOut[PARAM_SOCK_MAX];			// ��M�^�C���A�E�g�C�x���g�F��(ID)
	HANDLE					my_evRecv[PARAM_SOCK_MAX];					// ��M�C�x���g�F��(ID)

	HANDLE					my_evCloseReq;								// ����ؒf�v��

	// �\�P�b�g & ���M�f�[�^�֘A
	SOCK_STATE				my_emSockState[PARAM_SOCK_MAX];				// �X�e�[�^�X (�O���K�C�_���X�p�ɂ����g��Ȃ����ȁ[)
	SOCK_F_HEAD2*			my_pHead[PARAM_SOCK_MAX];					// �X���b�h�L���[�󂯎�� 
	SOCK_F_HEAD2			my_RecvWkHead[PARAM_SOCK_MAX];				// ��M�w�b�_���̃o�b�t�@
	DWORD					my_nRecvingDataSize[PARAM_SOCK_MAX];		// ���݂̃f�[�^���݂̂̎�M�o�C�g�� (�������M�Ή��΍�) (ID)
	DATA_SUB_MODE			my_emRecvAddId[PARAM_SOCK_MAX];				// �f�[�^����Add����ڂ̑��M��

	bool					my_bHelcyOk[PARAM_SOCK_MAX];			// ����̃w���V�[�`�F�b�N�̓���


	// ���̑��������
	CString					my_cSession;								// ini�t�@�C���̃Z�b�V������

};
