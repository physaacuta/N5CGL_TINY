// *********************************************************************************
//	����PC�����������M�N���X (�T�[�o�[�@�\�̂�)
//	[Ver]
//		Ver.01    2007/03/20  vs2005 �Ή�
//
//	[����]
//		��ɁA����PC�������ւ̎�M�Ɏg�p
// *********************************************************************************

#pragma once


// ���Œ�
#include "..\..\Include\KizuCommon.h"									// �r���C���N���[�h
#include "..\KizuLib\KizuLib.h"											// �r�����C�u����
#include "..\KizuLib\Class\KizuMacro.h"									// �}�N���W
#include "..\KizuLibMFC\Class\LogFileManager.h"							// ���O�Ǘ��N���X

#include "..\KizuSOCK3\SockBase.h"										// �\�P�b�g����N���X

using namespace KizuLib;
using namespace KizuSock3;

class KizuHtRecv 
{
public:
	// �{�N���X�̃\�P�b�g��ԃX�e�[�^�X
	enum SOCK_STATE {
		STATE_NONE = 0,													// ������� (�ؒf��)
		STATE_IDLE,														// �������� (�ڑ���)
		STATE_HEAD,														// �w�b�_�� ��M��
		STATE_DATA														// �f�[�^�� ��M��
	};

protected :
	// ���[�J���萔
	static const int PARAM_SOCK_MAX				= 16;					// �ő�ڑ��l�� (�K��KS_MAXCLIENTS�ȉ��ł��鎖)
	static const int PARAM_SOCK_TIMEOUT			= 10000;				// �\�P�b�g �^�C���A�E�g
	static const int PARAM_SOCK_RETRY_TIME		= 2000;					// �\�P�b�g ���g���C����
	static const int PARAM_SOCK_RECV_BUFSIZE	= (4*1024*1024);		// LAN�{�[�h�̎�M�o�b�t�@�T�C�Y (4M) �������̑���M�����֌W�Ȃ��Ƃ̎�
	
	// Ver.1.3.0.0 �R���X�g���N�^�ň����n���ɂ���
	//static const int PARAM_SOCK_RECV_DATASIZE	= (32*1024*1024);		// ��M�o�b�t�@�T�C�Y (32M)  ��M�f�[�^�T�C�Y�Ŗ����蒼���������̗]�n�L��I�I���R�X�g�̖��ʂ�����K�v�����Ƃ̎��Bby���삳��
	const int PARAM_SOCK_RECV_DATASIZE;									// ��M�o�b�t�@�T�C�Y (�ݽ�׸���size����) (�f�t�H���g32M)

//=========================================================================
// ID				= �������ID (�N���C�A���g���q��������) (0�I���W��)
// PCID				= ����@�\ID (�@�\��) (�ʏ�1�I���W��)
// RECVID			= �_�����ID (��M�w�b�_����subinf�ŕ��ёւ�����) (0�I���W��)
// �N���C�A���gID	= ksThread�̃��C���X���b�h�̃|�C���^
//=========================================================================

public:
	//// ���\�b�h
	KizuHtRecv(const char *cSession, int torb, int accept_num, int recv_num, int recv_max_size = 32*1024*1024);
	virtual ~KizuHtRecv();

	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ���O�t�@�C���Z�b�g
	void SetSockTID(DWORD tid) { my_nThSock = tid; }					// �����M�X���b�h�ւ̑����

	int GetPcidToId(int pcid)	// pcid(1�I���W��) ����id�ɕϊ�
		{
			for(int ii=0; ii<my_nRecvNum; ii++) {
				if(my_PC[ii] == pcid ) return ii;
			}
			return -1;
		}

	void		GetEvDetectWaitStart()	  {SetEvent(my_evDetectWaitStart);}	// �����J�n�҂��v���V�O�i��
	void		SetEvDetectWaitStart()	  {SetEvent(my_evDetectWaitStart);}	// �����J�n�҂��v���V�O�i��
	void		SetEvDetectRun()		  {SetEvent(my_evDetectRun);}	// �����J�n�v���V�O�i��
	void		SetEvDetectStop()		  {SetEvent(my_evDetectStop);}	// ������~�v���V�O�i��
	bool		AllConnect();											// �S���萳��?

	SOCK_STATE  GetSockState(int id)const {return my_emSockState[id];};	// ���݂̃X�e�[�^�X
	CString		GetSockStateName(int id)	const;						// ���݂̃X�e�[�^�X����
	bool		IsConnect(int pcid) const		{						// PCID���̐ڑ���Ԃ��擾
					for(int ii=0; ii<my_nAcceptNum; ii++) {
						if( pcid == my_PC[ii] ) { 
							if(STATE_NONE != my_emSockState[ii]) return true;
							break;
						}
					}
					return false;
				}

	// �X�e�[�^�X����
	void SetCliantName(int id, char const* cVal);						// �N���C�A���g���̂��Z�b�g (����Ŕ���PC����̃C�j�V�����`���𔻒f���Ăǂ̔���@�\�ƌq�����������f����)

	// �O���֏�Ԓʒm
	HANDLE	GetEvConnectAll()   const {return my_evConnectAll; };		// �S�N���C�A���g�ڑ�������
	HANDLE	GetEvErrToClose()   const {return my_evErrToClose; };		// �ُ픭���ɂ�� �N���C�A���g�ؒf��


protected :

	//// �����o�[�֐�
	// �X�e�[�^�X����


	// �V�[�P���X����
	virtual BOOL RecvAllHeader();										// ���ׂẴw�b�_�[��M���J�n
	virtual int  AnsHeader(int id);										// �Ɩ��`�� �w�b�_�[����M����
	virtual BOOL AnsData(int id);										// �Ɩ��`�� ��M�����`�F�b�N
	virtual BOOL AnsInitializeSocket(int id);							// �C�j�V�����`�� ��M����
	virtual void DefectStart();											// �T���J�n����
	virtual void DefectStop();											// �T����~����
	virtual BOOL CheckFrameNo(DWORD framenum, int id, int recvid);		// �t���[��No�̘A�ԃ`�F�b�N
	virtual BOOL CheckAllReadEnd();										// �S����PC�����M����

	// TPC/IP �ʐM����
	void Reset();														// ���Z�b�g
	BOOL Listen(bool bLogDsp);											// Listen
//	int  Accept(MSG* msg);												// Accept
	int  Accept();														// Accept (�V�O�i���ʒm�p)
	void CloseAll(bool bErr=false);										// �S�N���C�A���g�ؒf & �ُ�ʒm
	void ErrCall();														// �ُ�ʒm�̂� (����ؒf�͂��Ȃ�)
	BOOL RecvHeaderInitialize(int id);									// �C�j�V�����`���w�b�_�[�Ǎ��J�n
	virtual BOOL RecvHeaderDefect(int id);								// �w��̃w�b�_�[��M���J�n
	BOOL RecvDataDefect(int id, int recvid);							// �Ɩ��`���̃f�[�^����M���J�n

	// �e�C���X�^���X
protected:
	LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X	
	SockBase*				mcls_pSock;									// �\�P�b�g����N���X�C���X�^���X���X	

	//// �����V�O�i��
	HANDLE					my_evConnectAll;							// �S�N���C�A���g�ڑ�������
	HANDLE					my_evErrToClose;							// �ُ픭���ɂ�� �N���C�A���g�ؒf��
	HANDLE					my_evDetectWaitStart;						// �����J�n�҂��v��
	HANDLE					my_evDetectRun;								// �����J�n�v��
	HANDLE					my_evDetectStop;							// ������~�v��
	HANDLE					my_evAccept;								// �e����PC���Accept��


	//// �����V�O�i�� (�X�̃\�P�b�g�p)
	HANDLE					my_evDisConnect[PARAM_SOCK_MAX];			// �ؒf�C�x���g�F��(ID)
	HANDLE					my_evRecvTimeOut[PARAM_SOCK_MAX];			// ��M�^�C���A�E�g�C�x���g�F��(ID)
	HANDLE					my_evRecv[PARAM_SOCK_MAX];					// ��M�C�x���g�F��(ID)


	// ��M�p
	BOOL					my_bRun;									// �T����TRUE
	SOCK_STATE				my_emSockState[PARAM_SOCK_MAX];				// �\�P�b�g���(ID)
	long					my_PC[PARAM_SOCK_MAX];						// pc_id(ID)   // �q���������Ԃ̔z��B����2��1�ԂɂȂ�������[0]��2���Z�b�g���� (�d�v)
	char					my_cClientName[PARAM_SOCK_MAX][SIZE_NAME_PC];	// �N���C�A���g��PC��(PCID)
	DWORD					my_nFrameNo[PARAM_SOCK_MAX];				// �t���[��No(RECVID) (�������������t���[��No)
	bool					my_bDataReady[PARAM_SOCK_MAX];				// �f�[�^����������(RECVID)
	SOCK_F_HEAD				my_cRecvWkHead[PARAM_SOCK_MAX];				// ���ёւ��O�̎�M�w�b�_�[(�������[�N�p) (ID)
	SOCK_F_HEAD				my_cRecvHead[PARAM_SOCK_MAX];				// ��M�w�b�_�[(RECVID)
	BYTE*					my_cRecvData[PARAM_SOCK_MAX];				// ��M�f�[�^(RECVID)

	long					my_nRecvingDataSize[PARAM_SOCK_MAX];		// ���݂̃f�[�^���݂̂̎�M�o�C�g�� (�������M�Ή��΍�) (ID)
	int						my_nRecvMaxCnt[PARAM_SOCK_MAX];				// �N���C�A���g����1�t���[���ŉ��񑗂��Ă��邩(ID)
	int						my_nRecvNowCnt[PARAM_SOCK_MAX];				// �N���C�A���g����1�t���[���ŉ��񑗂��Ă�����(ID)


	// ���̑��������
	DWORD					my_nThSock;									// �����M�X���b�h�ւ̑����
	int						my_nAcceptNum;								// �N���C�A���g�� (�����I�Ȑ�)
	int						my_nRecvNum;								// �t���[���P�ʂł̎�M�� (�_���I�Ȑ�)
	CString					my_cSession;								// SockBaseServer�̐ڑ�Session
	int						my_nTorB;									// �\�����ʃt���O(0:�\, 1:��)


	// �����p
public:
	void SetLogMgrD(LogFileManager* pLog) { mcls_pLogD = pLog; }		// �����p���O�t�@�C���Z�b�g
protected:
	LogFileManager*			mcls_pLogD;									// �����pLogFileManager�X���b�h�C���X�^

};
