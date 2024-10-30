// *********************************************************************************
//	TCP/IP �ʐM ��PC�ԒʐM�N���X �P�Α� (Send:�N���C�A���g   Recv:�T�[�o�[)
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//		�E�����ʐM�A��ʃf�[�^�ʐM���l���������Ƃ���
//			���Ƃ͂����Ă� KizuToSend �ƂقƂ�Ǔ����ɂȂ����E�E�E
//		�EAddr���̂ݕ������M(2M�ȏ�)�ɑΉ�
// *********************************************************************************

#pragma once
// ���Œ�
#include "..\..\Include\KizuCommon.h"									// �r���C���N���[�h
#include "..\KizuLibMFC\CLASS\ThreadManager.h"							// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\KizuLibMFC\Class\ThreadLoopQueue.h"
#include "..\KizuLib\Class\KizuPerformanceCounter.h"					// �^�C�}�[
#include "..\KizuLib\Class\AutoLock.h"									// �N���e�B�J���Z�N�V��������N���X
#include "..\KizuSOCK3\SockBase.h"										// �\�P�b�g����N���X

#include "SockOneToAny.h"

using namespace KizuLib;
using namespace KizuSock3;

class SockOneToAnySend : public ThreadManager
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
		STATE_SUB_ADDR0,
		STATE_SUB_ADDR1
	};


// ���[�J���萔
protected :
	static const int PARAM_SOCK_MAX				= 1;					// �ő�ڑ��l��
	static const int PARAM_SOCK_TIMEOUT			= 10000;				// �\�P�b�g �^�C���A�E�g
	static const int PARAM_SOCK_RETRY_TIME		= 2000;					// �\�P�b�g ���g���C����
	static const int PARAM_SOCK_SEND_BUFSIZE	= (4*1024*1024);		// LAN�{�[�h�̑��M�o�b�t�@�T�C�Y (4M) 4M��MAX�H�H

	static const int PARAM_TIME_HELCY_SEND		= (10*1000);			// �w���V�[���M�B���񉽂������Ă���{�b���o��

public:
	SockOneToAnySend();
	virtual ~SockOneToAnySend(void);

	void	SetSession(const char *cSession ){ my_cSession = cSession; }	// �ڑ��Z�b�V�����Z�b�g 
	void	SetRetryMode (bool bRetry=true) { my_bRetryMode=bRetry; }
	void	SetTLQue( ThreadLoopQueue<SOCK_F_HEAD2>* p ) { mque_pDeli = p; }


	//// �\�P�b�g����
	void SetConnect	  () { SetEvent(my_evConnect);}						// ����ڑ��v�� (�񓯊�)
	void SetDisConnect() { SetEvent(my_evDisConnect);}					// ����ؒf�v�� (�񓯊�)

	HANDLE		GetEvClose()   const {return my_evAnserClose; };		// �O���ւ̐ؒf�ʒm�C�x���g
	HANDLE		GetEvConnect() const {return my_evAnserConnect; };		// �O���ւ̐ڑ��ʒm�C�x���g


	//// �O���A�N�Z�X
	bool IsConnect()  const { return STATE_NONE == my_emSockState ? false :true; }	// �ڑ����
	// Get
	SOCK_STATE  GetSockState() const {return my_emSockState; };			// ���݂̃X�e�[�^�X
	CString		GetSockStateName()	const {return GetSockStateName(my_emSockState);};	// ���݂̃X�e�[�^�X����
	CString		GetSockStateName(SOCK_STATE st)	const;									// �w��̃X�e�[�^�X����

//// �����o�[�֐�
protected:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	void ThreadWaitFor(int nDefWaitNum, int* nNowWaitNum);				// �X���b�h��WaitFor�҂��̒��O����
	

	// �o�b�t�@�n
	void QueBufSendBack();												// ���o�b�t�@��ԋp����
	void QueAllSendBack();												// ���ׂẴL���[��ԋp����

	// �ʐM����
	void SendStart();													// �r���`�����M �J�n����
	void SendEnd();														// �r���`�����M ��������
	void SendHeader();													// �w�b�_�[�����M�J�n
	void SendData();													// �f�[�^�����M�J�n
	void SendHelcy();													// �w���V�[�`�����M ����

	// �\�P�b�g����
	void Connect(bool bLogDsp);											// �ڑ�
	void Close();														// �ؒf & ���g���C�ݒ�


protected :
	// �e�C���X�^���X
	SockBase*				mcls_pSock;									// �\�P�b�g����N���X�C���X�^���X
	KizuPerformanceCounter	mcls_Time;									// �擾��������(�X�g�b�v�E�H�b�`�@�\���g�p)

	// ��n�L���[
	ThreadLoopQueue<SOCK_F_HEAD2>*		mque_pDeli;						// ����f�[�^�󂯓n���L���[(��]���i�`��)	[LIB��AP��]
	

	// �C�x���g�n���h��
	HANDLE					my_tmReTryConnect;							// �Đڑ��v���^�C�}�C�x���g
	HANDLE					my_tmHelcy;									// �w���V�[���M�^�C�}�C�x���g

	HANDLE					my_evConnect;								// �ڑ��v���C�x���g
	HANDLE					my_evDisConnect;							// �ؒf �C�x���g (�蓮�ł��؂��悤�Ɍ��J�����Ēu��)
	HANDLE					my_evSendTimeout;							// ���M�^�C���A�E�g �C�x���g
	HANDLE					my_evSendEnd;								// ���M���� �C�x���g
	HANDLE					my_evAnserClose;							// �O���ւ̐ؒf�ʒm�C�x���g
	HANDLE					my_evAnserConnect;							// �O���ւ̐ڑ������ʒm�C�x���g

	// ��������
	CRITICAL_SECTION		m_csLock;									// �N���e�B�J���Z�N�V����

	// �\�P�b�g & ���M�f�[�^�֘A
	SOCK_STATE				my_emSockState;								// �X�e�[�^�X (�O���K�C�_���X�p�ɂ����g��Ȃ����ȁ[)
	SOCK_F_HEAD2*			my_pHead;									// �X���b�h�L���[�󂯎�� && ���M�f�[�^�o�b�t�@
	long					my_nSendingDataSize;						// ���݂̃f�[�^���݂̂̑��M�o�C�g�� (�������M�Ή��΍�)
//	int						my_nSendAddId;								// �f�[�^����Add����ڂ̑��M��
	DATA_SUB_MODE			my_emSendAddId;								// �f�[�^����Add����ڂ̑��M��

	bool					my_bHelcyCancel;							// ����̃w���V�[���M�̓���

	// ���̑��������
	CString					my_cSession;								// ini�t�@�C���̃Z�b�V������
	bool					my_bRetryMode;								// ���g���C�ڑ��̗L�� (true:���g���C���� false:���g���C���Ȃ�)
};
