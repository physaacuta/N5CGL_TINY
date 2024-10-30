// *********************************************************************************
//	TCP/IP�̑���M�p���N���X
//	[Ver]
//		Ver.01    2007/07/21  vs2005 �Ή�
//
//	[����]
//		�E��ɁATCP/IP�̊O��I/F�p�ʐM�Ɏg�p (�����܂Ŋ�{�I�ȒʐM�̂ݎ������Ă���)
//		�E�񓯊�/���� �����̑���M�ɑΉ���������B
//		�E���ۂɁAFF��v���R���ƒʐM���������ꍇ�́AFF�p�̌p�������N���X������邱�ƁB �����Ȃ肢���̂��ł�������A�K�v�Ȃ�����
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

class BaseTcpIp : public WorkThread
{
public:
	//=============================
	// �萔
	//=============================
	//// ��ʓI�ȃZ�~�����̃��X�|���X (�{�N���X���ł͓��Ɏg�p���ĂȂ��B�g�������l�́A����Ɏg���Ă�)
	// �T�u�w�b�_�[ [0]
	static const BYTE	gcTCPSEMI_HEAD_H_GYOUMU  = 0x60;				// �Ɩ��`�� ���� �����`��
	static const BYTE	gcTCPSEMI_HEAD_H_RESPONS = 0xE0;				// ���X�|���X�`��
	// �T�u�w�b�_�[ [1]
	static const BYTE	gcTCPSEMI_HEAD_L_OK		 = 0x00;				// ����
	static const BYTE	gcTCPSEMI_HEAD_L_NGCODE  = 0x50;				// Code��ʈُ�
	static const BYTE	gcTCPSEMI_HEAD_L_NGDEN   = 0x51;				// �`����ʈُ�
	static const BYTE	gcTCPSEMI_HEAD_L_NGSIZE  = 0x52;				// �f�[�^���ُ�

	//// ���̑�TCPIP�p
	static const int	PARAM_SOCK_TIMEOUT			= 3000;					// ����M�J�n���Ă���I���܂ł̎���

	//// �{�N���X�̃\�P�b�g��ԃX�e�[�^�X
	enum SOCK_STATE { 
		STATE_NONE = 0,													// ������� (�ؒf��)
		STATE_IDLE,									 					// �������� (�ڑ���)

		STATE_RSING,													// ����M��
		STATE_RECVING,													// ��M��
		STATE_SENDING													// ���M��
	} ; //SockState;

	//// �A���T�[���̃X�e�[�^�X
	enum DELI_ANS_STATE {
		ANS_CONNECT = 0,												// �ڑ������ʒm
		ANS_CLOSE,														// �ؒf�����ʒm
		ANS_RECVEND,													// �񓯊���M�����ʒm
		ANS_SENDEND,													// �񓯊����M�����ʒm
		ANS_ERR_CONNECT													// �R�l�N�g���s�ʒm
	} ; //DeliAnsState;

	//=============================
	// �{�N���X�󂯓n���p�N���X
	//=============================
	//// ��{�ݒ���
	struct DELIVERY_COMMON {
		int		nMyID;													// ������ID   (�N������悤�ɂ����)
		char	cMyName[32];											// �����̖��� (�N������悤�ɂ����)
		char	cSession[32];											// Ini�t�@�C���̃Z�b�V����
		bool	bSorC;													// �T�[�o�[(true) or �N���C�A���g(false)
		bool	bRorS;													// ��M(true) or ���M(false) �����C���̃|�[�g��
		int		nRetryTimer;											// �Đڑ��v���̎��� [ms]
		int		nBufferSize;											// ����M�̍ő�o�b�t�@�T�C�Y (�K���`�������傫�����鎖)
	};

	//// ����M���
	struct DELIVERY_DATA {
		bool	bRorS;													// ��M(true) or ���M(false) ��
		bool	bAnsQueing;												// ���쌋�ʂ��L���[�ɓo�^����ꍇ true�B �N���X���ŏ���������ꍇ�ɂ̂�false
		int		nSize;													// ����M�f�[�^�̒���
		int		nStartPoint;											// Data���̎��ۂ̑��삷��ʒu�B�i�ʏ�0�Œ�B��M���ɁA�w�b�_�[���A�f�[�^���𕪂��Ď�M����ꍇ���ɁA�����L���B�j
		BYTE	*Data;													// ����M�f�[�^�̃|�C���^ (�J�n����New�B��������Delete���鎖)
		int		nUserWk[4];												// ����Ɏg�p���Ă��������B(�{�N���X�ł͈�؂Ȃɂ����܂���)

		// �{�N���X���Z�b�g ------->>>
		int		nAns;													// �������
																			// 0:  ����ɑ���M�o�����ꍇ
																			// 1:  ���s (�^�C���A�E�g��)
																			// -1: �O�̃C���X�^���X�����s��
																			// -2: �q�X���b�h���Ƃ܂��Ă���
																			// -3: �񓯊�����^�C���A�E�g
																			// -7: ����񓯊�����p����
																			// -8: �񓯊����쒆�ɐؒf���ꂽ
																			// -9: �\�P�b�g�I�[�v�����ĂȂ�
	};

	//// �A���T�[���
	struct DELIVERY_ANS {
		int					nMyID;										// ������ID (�N������悤�ɂ����)
		DELI_ANS_STATE		state;										// �A���T�[���̃X�e�[�^�X
		DELIVERY_DATA*		Data;										// ��� (����M���̂݁B����ȊO��NULL)
	};


public:
	BaseTcpIp(DELIVERY_COMMON* del);
	virtual ~BaseTcpIp(void);

	void Start() { WorkThread::Start(MainThread); };					// �X���b�h�J�n
	void Stop()  { WorkThread::StopMSG(WM_USER_TH_END); };				// �X���b�h�I��
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ���O�t�@�C���Z�b�g
	void SetPostTID(DWORD tid) { this->my_PostTid=tid;}					// PostThreadMessage�̑���� (���C���C���X�^���X)
	void SetErrConnectQueing(bool bFlg) {my_bIsErrConnectQueing=bFlg;};	// �R�l�N�g���s�� �󂯓n���L���[�ɃL���[�C���O���邵�Ȃ�
	

	//// �񓯊� ����p
	bool SetRunningData( DELIVERY_DATA* );								// �񓯊� ����M�f�[�^�󂯓n��	
	int DeleteEndData(DELIVERY_DATA* pData);							// �񓯊� ����M�f�[�^�̊��������̂������J�� (����M�̌��ʏ���߂�l�Ƃ��ĕԂ�)

	//// ���� ����p
	int LockRecv(BYTE* data, int size, int timeout);					// ���� ��M
	int LockSend(BYTE* data, int size, int timeout);					// ���� ���M



	//// �O���A�N�Z�X
	void		UserClose() { SetEvent(my_evUserClose); };					// ���[�U�[����̋����ؒf�w��

	// Get
	int			GetCloseKind()		const {return my_nCloseKind; };			// �I���敪���擾
										//	#define KIZUSOCK_CLOSEKIND_NON		0			// �I������������������s��
										//	#define KIZUSOCK_CLOSEKIND_SOCK		1			// �\�P�b�g���ؒf���ꂽ (= ���肩��̐ؒf)
										//	#define KIZUSOCK_CLOSEKIND_PROG		2			// �ُ팟�o�����ׁA�v���O�����Őؒf
										//	#define KIZUSOCK_CLOSEKIND_THEAD	3			// �X���b�h�I��
	SOCK_STATE  GetSockState()		const {return my_emSockState; };					// ���݂̃X�e�[�^�X
	CString		GetSockStateName()	const {return GetSockStateName(my_emSockState);};	// ���݂̃X�e�[�^�X����
	CString		GetSockStateName(SOCK_STATE st)	const;									// �w��̃X�e�[�^�X����
	CString		GetAnsStateName(DELI_ANS_STATE st) const;					// �A���T�[����
	const char* GetMyName()			const {return my_Del_Common.cMyName; }	// �����̖��O �ݒ�


	// �󂯓n���L���[
	ThreadQueue<DELIVERY_ANS>  gque_Anser;								// �A���T�[�L���[ (�擾�� �K���J�����鎖)


protected :
	static UINT WINAPI MainThread(LPVOID param);						// ���C���X���b�h

	// Sock�֘A
	BOOL SockNew(bool bLogDsp);											// �C���X�^���X����
	BOOL SockDelete(bool bLogDsp);										// �C���X�^���X�J��
	BOOL Listen(bool bLogDsp);											// Listen
	BOOL Connect(bool bLogDsp);											// �ڑ�(�u���b�L���O)
	BOOL Accept();														// Accept(�V�O�i���ʒm�p) 
	BOOL Close(bool bLogDsp);											// �ؒf
	BOOL IsConnect() const { return (NULL == mcls_pSock || 0==mcls_pSock->GetActive() ) ? FALSE : TRUE; };		// �\�P�b�g���L����TRUE

	// ���ʑ���
	bool SetAnserQue(DELI_ANS_STATE state, DELIVERY_DATA* data=NULL);	// �O���ʒm�p�ɃL���[�C���O

	// �񓯊�����
	void RunningStart();												// �񓯊� ����M����˗�
	virtual void CheckRecvEnd();										// �񓯊� ��M����
	virtual void CheckSendEnd();										// �񓯊� ���M����


	// �e�C���X�^���X
	LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X
	SockBase*				mcls_pSock;									// �\�P�b�g����N���X�C���X�^���X

	// �����V�O�i��
	HANDLE					my_evUserClose;								// ���[�U�[����̋����ؒf�ʒm
	HANDLE					my_evRunningEnd;							// ���[�U�[����̑����Ԋ���������ʒm

	// �����V�O�i�� (�\�P�b�g�֘A)
	HANDLE					my_evReTryConnect;							// �Đڑ��v���^�C�}�C�x���g
	HANDLE					my_evAccept;								// Accept �C�x���g
	HANDLE					my_evDisConnect;							// �ؒf �C�x���g
	HANDLE					my_evSendTimeout;							// ���M�^�C���A�E�g �C�x���g
	HANDLE					my_evRecvEnd;								// ��M���� �C�x���g
	HANDLE					my_evSendEnd;								// ���M���� �C�x���g

	HANDLE					my_evLockRecvEnd;							// ���� �u���b�L���O��M�����C�x���g
	HANDLE					my_evLockSendEnd;							// ���� �u���b�L���O���M�����C�x���g

	// �󂯓n���L���[
	ThreadQueue<DELIVERY_DATA> mque_Running;							// ����M�f�[�^�󂯓n���L���[

	// �^�C�}�[�֘A
	UINT					my_tId_ReTry;								// �Đڑ��v���^�C�}ID

	// �\�P�b�g�֘A
	SOCK_STATE				my_emSockState;								// �X�e�[�^�X (�O���K�C�_���X�p�ɂ����g��Ȃ����ȁ[)
	DELIVERY_COMMON			my_Del_Common;								// ��{���
	DELIVERY_DATA*			my_pNowDataR;								// ���݂� �񓯊� ��M���̃f�[�^
	DELIVERY_DATA*			my_pNowDataS;								// ���݂� �񓯊� ���M���̃f�[�^

	// ���̑��������
	int						my_nCloseKind;								// �I�����(-1:�{�v���O���� 0�`:KizuSock3�Ǘ�)
	bool					my_bIsErrConnectQueing;						// �R�l�N�g���s�� �󂯓n���L���[�ɃL���[�C���O���邵�Ȃ� (true:����) �f�t�H���gfalse ���ʌ݊��p
};
