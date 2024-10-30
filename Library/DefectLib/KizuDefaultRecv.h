// *********************************************************************************
//	�r��PC�� TCP/IP��M��{�N���X (�N���C�A���g�@�\�̂�)
//	[Ver]
//		Ver.01    2007/03/19  vs2005 �Ή�
//
//	[����]
//		��ɁA�r��PC�Ԃ̎�M�Ɏg�p���� (KizuOpSend�̎󂯂Ɏg�p)
// *********************************************************************************

#pragma once

// ���Œ�
#include "..\..\Include\KizuCommon.h"									// �r���C���N���[�h
#include "..\KizuLib\KizuLib.h"											// �r�����C�u����
#include "..\KizuLib\Class\KizuMacro.h"									// �}�N���W
#include "..\KizuLibMFC\Class\LogFileManager.h"							// ���O�Ǘ��N���X

// �r���N���X �C���N���[�h
#include "..\KizuLib\Class\WorkThread.h"								// ���[�J�[�X���b�h�N���X
#include "..\KizuLib\Class\ThreadQueue.h"								// �X���b�h�L���[�N���X

#include "..\KizuSOCK3\SockBase.h"										// �\�P�b�g����N���X

using namespace KizuLib;
using namespace KizuSock3;

class KizuDefaultRecv : public WorkThread  
{
public:
	static const int PARAM_SOCK_RETRY_TIME			= 5000;				// �\�P�b�g�Đڑ����� [ms]
	static const int PARAM_SOCK_RECV_BUFSIZE		= (4*1024*1024);	// LAN�{�[�h�̎�M�o�b�t�@�T�C�Y (4M)
	static const int PARAM_TIMEOUT_HELCY_RECV		= (5*60*1000);		// �w���V�[�`����M�^�C�}�A�E�g [ms] (5��)


	// �{�N���X�̃\�P�b�g��ԃX�e�[�^�X
	enum SOCK_STATE { 
		STATE_NONE = 0,													// ������� (�ؒf��)
		STATE_IDLE,														// �������� (�ڑ���)
		STATE_HEAD,														// �w�b�_�� ��M��
		STATE_DATA														// �f�[�^�� ��M��
	};

public:
	KizuDefaultRecv(const char *cSession, ThreadQueue<SOCK_BASE_HEAD>* que_pRecv);
	virtual ~KizuDefaultRecv(void);

	void Start() { WorkThread::Start(MainThread); };					// �X���b�h�J�n
	void Stop()  { WorkThread::Stop(10000); };							// �X���b�h�I��
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ���O�t�@�C���Z�b�g

	//// �O���A�N�Z�X
	// Get
	SOCK_STATE  GetSockState()		const {return my_emSockState; };					// ���݂̃X�e�[�^�X
	CString		GetSockStateName()	const {return GetSockStateName(my_emSockState);};	// ���݂̃X�e�[�^�X����
	CString		GetSockStateName(SOCK_STATE st)	const;									// �w��̃X�e�[�^�X����

protected:
	static UINT WINAPI MainThread(LPVOID param);						// ���C���X���b�h

	// �\�P�b�g����
	void Connect(bool bLogDsp);											// �ڑ�
	void Close();														// �ؒf & ���g���C�ݒ�
	bool Recv_Head();													// �w�b�_����M�J�n
	bool Recv_Data();													// �f�[�^����M�J�n
	void Recv_Helcy();													// �w���V�[�`����M���� (�w�b�_�[���̂�)

	// �󂯓n���L���[
	ThreadQueue<SOCK_BASE_HEAD>* mque_pRecv;							// ��M�f�[�^�\���̎󂯓n�� �L���[

	// �e�C���X�^���X
	LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X
	SockBase*				mcls_pSock;									// �\�P�b�g����N���X�C���X�^���X
	
	// �\�P�b�g
	SOCK_STATE				my_emSockState;								// �X�e�[�^�X (�O���K�C�_���X�p�ɂ����g��Ȃ����ȁ[)
	SOCK_BASE_HEAD		 	my_RecvHead;								// ��M�w�b�_���̃o�b�t�@ (���ۂ́ASOCK_F_HEAD�ƂȂ邪�A�w�b�_�[���݂̂̓`������M�o����l�ɂ���ׁASOCK_BASE_HEAD�Ƃ��鎖)
	BYTE *					my_pRecvData;								// ��M�f�[�^���̃o�b�t�@
	long					my_nRecvingDataSize;						// ���݂̃f�[�^���݂̂̎�M�o�C�g�� (�������M�Ή��΍�) (ID)

	// �V�O�i��
	HANDLE					my_evReTryConnect;							// �Đڑ��v���^�C�}�C�x���g
	HANDLE					my_evHelcy;									// �w���V�[���M�C�x���g
	HANDLE					my_evDisConnect;							// �ؒf�C�x���g
	HANDLE					my_evRecvEnd;								// ��M�����C�x���g

	// ���̑��������
	CString					m_cSession;									// ini�t�@�C���̃Z�b�V������
	
};
