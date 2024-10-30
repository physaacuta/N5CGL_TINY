// *********************************************************************************
//	����PC���������瑗�M�N���X (�N���C�A���g�@�\�̂�)
//	[Ver]
//		Ver.01    2007/08/09  vs2005 �Ή�
//
//	[����]
//		��ɁA����PC�������ւ̑��M�Ɏg�p
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


class KizuToSend : public WorkThread
{
public:
	// �{�N���X�̃\�P�b�g��ԃX�e�[�^�X
	enum SOCK_STATE { 
		STATE_NONE = 0,													// ������� (�ؒf��)
		STATE_IDLE,														// �������� (�ڑ���)
		STATE_HEAD,														// �w�b�_�� ���M��
		STATE_DATA														// �f�[�^�� ���M��
	};

protected :
	// ���[�J���萔
	static const int PARAM_SOCK_MAX				= 1;					// �ő�ڑ��l��
	static const int PARAM_SOCK_TIMEOUT			= 10000;				// �\�P�b�g �^�C���A�E�g
	static const int PARAM_SOCK_RETRY_TIME		= 2000;					// �\�P�b�g ���g���C����
	static const int PARAM_SOCK_SEND_BUFSIZE	= (4*1024*1024);		// LAN�{�[�h�̑��M�o�b�t�@�T�C�Y (4M)


public:
	KizuToSend();
	virtual ~KizuToSend(void);

	void Start() { WorkThread::Start(MainThread); };					// �X���b�h�J�n
	void Stop()  { WorkThread::Stop(10000); };							// �X���b�h�I��
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ���O�t�@�C���Z�b�g
	void SetPostTID(DWORD tid) { this->my_PostTid=tid;}					// PostThreadMessage�̑���� (���C���C���X�^���X)

	// �󂯓n���L���[
	ThreadQueue<SOCK_F_HEAD> gque_Deli;									// ���M�f�[�^�󂯓n���L���[
	ThreadQueue<SOCK_F_HEAD> gque_SendBack;								// ���M�����f�[�^�󂯓n���L���[ (���̂܂ܕԂ�)


	// �\�P�b�g����
	void SetConnect	  (bool bRetry=true)  { my_bRetryMode=bRetry; SetEvent(my_evConnect);}		// ����ڑ��v�� (�񓯊�)
	void SetDisConnect(bool bRetry=false) { my_bRetryMode=bRetry; SetEvent(my_evDisConnect);}	// ����ؒf�v�� (�񓯊�)


	//// �O���A�N�Z�X
	bool IsConnect()  const { return STATE_NONE == my_emSockState ? false :true; }	// �ڑ����
	// Get
	HANDLE		GetEvClose()   const {return my_evAnserClose; };		// �O���ւ̐ؒf�ʒm�C�x���g
	int			GetQueCount()  const {return gque_Deli.GetCount(); };	// ���M�҂�
	int			GetMaxCount()  const {return gque_Deli.GetMaxCount();};	// �ő�o�^�҂�����
	SOCK_STATE  GetSockState() const {return my_emSockState; };			// ���݂̃X�e�[�^�X
	CString		GetSockStateName()	const;								// ���݂̃X�e�[�^�X����

	// Set
	void SetSession(const char *cSession ){ my_cSession = cSession; }	// �ڑ��Z�b�V�����Z�b�g 
	void SetPcName(const char *cName ){ memcpy(my_cPCname, cName, sizeof(my_cPCname)); }	// PC���̃Z�b�g
	void SetSendNum(int sendnum){ my_nSendNum = sendnum; }				// ���M��

protected :
	static UINT WINAPI MainThread(LPVOID param);						// ���C���X���b�h
	
	void SendStart();													// �r���`�����M �J�n����
	void SendEnd();														// �r���`�����M ��������

	void QueBufSendBack();												// ���o�b�t�@��ԋp����
	void QueAllSendBack();												// ���ׂẴL���[��ԋp����

	// �\�P�b�g����
	void Connect(bool bLogDsp);											// �ڑ�
	void Close();														// �ؒf & ���g���C�ݒ�
	void SendHeader();													// �w�b�_�[�����M�J�n
	void SendData();													// �f�[�^�����M�J�n

	// �e�C���X�^���X
	LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X
	SockBase*				mcls_pSock;									// �\�P�b�g����N���X�C���X�^���X


	// �C�x���g�n���h��
	HANDLE					my_evReTryConnect;							// �Đڑ��v���^�C�}�C�x���g
	HANDLE					my_evConnect;								// �ڑ��v���C�x���g
	HANDLE					my_evDisConnect;							// �ؒf �C�x���g (�蓮�ł��؂��悤�Ɍ��J�����Ēu��)
	HANDLE					my_evSendTimeout;							// ���M�^�C���A�E�g �C�x���g
	HANDLE					my_evSendEnd;								// ���M���� �C�x���g
	HANDLE					my_evAnserClose;							// �O���ւ̐ؒf�ʒm�C�x���g


	// �\�P�b�g & ���M�f�[�^�֘A
	SOCK_STATE				my_emSockState;								// �X�e�[�^�X (�O���K�C�_���X�p�ɂ����g��Ȃ����ȁ[)
	SOCK_F_HEAD*			my_pHead;									// �X���b�h�L���[�󂯎�� && ���M�f�[�^�o�b�t�@
	long					my_nSendingDataSize;						// ���݂̃f�[�^���݂̂̑��M�o�C�g�� (�������M�Ή��΍�)
	int						my_nSendNum;								// �t���[���P�ʂł̑��M�� (�_���I�Ȑ�)


	// ���̑��������
	char					my_cPCname[SIZE_NAME_PC];					// ���M��PC
//	CString					my_cPCname;
	CString					my_cSession;								// ini�t�@�C���̃Z�b�V������
	bool					my_bRetryMode;								// ���g���C�̗L�� (true:���g���C���� false:���g���C���Ȃ�)

};
