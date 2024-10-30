// *********************************************************************************
//	�v���R���ʐM �X���b�h
//	[Ver]
//		Ver.01    2007/08/03  vs2005 �Ή�
//
//	[����]
//		
// *********************************************************************************
#pragma once


// ���Œ�
#include "..\..\Include\LineCommon.h"									// �����C���N���[�h
#include "..\..\Library\KizuLib\KizuLib.h"								// �������C�u����
#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// �}�N���W
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// �X�^�e�B�b�N���C�u�����W
#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ���O�Ǘ��N���X

// �����N���X �C���N���[�h
#include "..\..\library\kizulibmfc\class\threadmanager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
//#include "..\..\Library\KizuLib\Class\WorkThread.h"						// ���[�J�[�X���b�h�N���X
#include "..\..\Library\DefectLib\BaseTcpIp.h"							// TCP/IP����N���X

// �����o�[
#include "MainConst.h"
//#include "SockDataExec.h"												// ����M�f�[�^�󂯓n���N���X

using namespace KizuLib;

class SockManager :	public ThreadManager
{
private :

	// �o�b�t�@ID��`
	static const int BUF_RECV				= 0;						// ��M�p�o�b�t�@
	static const int BUF_SEND				= 1;						// ���M�p�o�b�t�@

	// �|�[�gID��`
	static const int PORT_RECV				= 0;						// ��M�|�[�gID 
	static const int PORT_SEND				= 1;						// ���M�|�[�gID 

	// �����`
	static const int ID_RECV				= 0;						// ��M����p
	static const int ID_SEND				= 1;						// ���M����p 


	// �ʐM�֌W��`
	static const int PARAM_SOCK_MAX			= 2;						// �\�P�b�g��
	static const int PARAM_SOCK_BUFSIZE		= 2*1024;					// �ő�o�b�t�@�T�C�Y (2K)
	static const int PARAM_SOCK_TIMEOUT		= 30*1000;					// �\�P�b�g �^�C���A�E�g[ms]
	static const int PARAM_SOCK_RETRY_TIMER = 10*1000;					// �ăR�l�N�g���� (�ăA�N�Z�v�g�Ԋu����)[ms]
#ifdef LINE_1CGL
	static const int PARAM_SOCKANSER_TIMEOUT= 90*1000;					// �����`�� �^�C���A�E�g (1CGL�����́A�v���R��CPU���Ⴂ���߃^�C���A�E�g���Ԃ����΂�)
#else
	static const int PARAM_SOCKANSER_TIMEOUT= 30*1000;					// �����`�� �^�C���A�E�g[ms]
#endif
	static const int PARAM_RESPANSER_TIMEOUT= 5*1000;					// ���X�|���X �^�C���A�E�g[ms]
	static const int PARAM_HELCY_TIMEOUT	= 90*1000;					// �w���V�[�`����M �^�C���A�E�g[ms]

public :
	//// �N���X���̏��
	enum SOCK_STAT {
		SOCK_NONE = 0,		// �ؒf��
		SOCK_IDLE,			// �v���҂�
		SOCK_EXEC			// �V�[�P���X���s��

//		SOCK_EXEC_RECV,		// ��M�V�[�P���X���s��
//		SOCK_EXEC_SEND		// ���M�V�[�P���X���s��
	};
	//// ��M�����
	enum SOCK_NOW_RECV {
		SOCK_NR_RES = 0,	// ���X����M��
		SOCK_NR_HEAD,		// �w�b�_�[����M��
		SOCK_NR_DATA		// �f�[�^����M��

	};



public:
	SockManager(void);
	virtual ~SockManager(void);

	void Start() { WorkThread::Start(MainThread);  };					// �X���b�h�J�n
	void Stop()  { WorkThread::Stop(10000); };							// �X���b�h�I�� (�V�O�i���C�x���g�ł̏I��)
	//=========================================
	// �Z�b�g�N���X
	//void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ���O�t�@�C���Z�b�g
	void SetQueDeli(ThreadQueue<Delivery>* pQue) { mque_pDeli = pQue; }	// �󂯓n���L���[�Z�b�g


	// �󂯓n���L���[
	ThreadQueue<Delivery>*	mque_pDeli;									// ���M�˗��p�L���[

	
	//// �O���A�N�Z�X
	CString GetStateName(int id) const		{return mcls_pSock[id]->GetSockStateName(); }		// �\�P�b�g�X�e�[�^�X����
	bool	IsConnect(int id)	 const		{ return m_eStateMain[id] == SOCK_NONE ? false : true; }	// ����ڑ����
	void	UserClose(bool bMode=true);															// �����ؒf
	CString GetSockStatName(int id) const { return GetSockStatName(m_eStateMain[id]); }			// �\�P�b�g�X�e�[�^�X����
	CString GetSockStatName(SOCK_STAT st) const;												// ���݂̃X�e�[�^�X����


	// Dump�o�͊֌W
	void SetDumpMode(bool bmode) { m_bDumpMode = bmode; }				// �_���v�o�̓��[�h�Z�b�g
	bool GetDumpMode() { return m_bDumpMode; }							// �_���v�o�̓��[�h�擾

protected:
	//=========================================
	// ���z�֐�
	virtual bool SetSendDataAnser(Delivery* deli, bool bAns) = 0;		// ���M�f�[�^ ���㏈��
	virtual bool SetRecvData(int tc, int size, BYTE * data) = 0;		// ��M�f�[�^ ���㏈��

//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
	void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	void ThreadWaitFor(int nDefWaitNum, int* nNowWaitNum);				// �X���b�h��WaitFor�҂��̒��O����

	void ExecAnsQue(int id, BaseTcpIp::DELIVERY_ANS* que);				// TCP/IP�N���X ���쌋�ʂ̏���


	// TCP/IP �ʐM����
	void SockInit();													// ����������
	void SockExit();													// �I������
	bool RecvStart(int id, int size, BYTE* pBuf, int point=0);			// �񓯊� ��M�����J�n
//	bool SendStart(int id, int size, BYTE* pBuf, int point=0);			// �񓯊� ���M�����J�n
	bool SendLock_Respons(int id, BYTE mode);							// ���X�|���X�`���̑��M (����)

	// ����M�V�[�P���X
	bool SendLockStart();												// �Ɩ��`�� ���M (����)
	bool RecvEnd(int id, BaseTcpIp::DELIVERY_ANS* que);					// ��M �������� (�񓯊�)


	//// �e�C���X�^���X
	//LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X
	BaseTcpIp*				mcls_pSock[PARAM_SOCK_MAX];					// ��M�|�[�g[0] ���M�|�[�g[1] �p����N���X
	//SockDataExec*			mcls_pData;									// ����M�f�[�^�󂯓n���N���X

	// �V�O�i��
	HANDLE					m_evAnserTimeOut;							// �����`���^�C���A�E�g�V�O�i��
	HANDLE					m_evHelcyTimeOut;							// �w���V�[�`����M�^�C���A�E�g�V�O�i��

	//// �\�P�b�g�֘A
	BYTE					m_cSockBuf[PARAM_SOCK_MAX][2][PARAM_SOCK_BUFSIZE];	// ����M�p�o�b�t�@ [0:��M�|�[�g 1:���M�|�[�g] [0:��M�o�b�t�@ 1:���M�o�b�t�@]

	//// �X�e�[�^�X
	bool					m_bIsConnect;								// ��������
	SOCK_STAT				m_eStateMain[PARAM_SOCK_MAX];				// ���C���X�e�[�^�X
	SOCK_NOW_RECV			m_eStateNowRecv[PARAM_SOCK_MAX];			// ��M�`���̂ǂ̕�������M���Ă��邩�t���O (0:���X�� 1:�w�b�_�[�� 2:�f�[�^��)
	long					m_nNowEventNo[PARAM_SOCK_MAX];				// ���� �V�[�P���X�������̃C�x���gNo
	//	int						m_nStateSub;								// �T�u�X�e�[�^�X


//// �����o�[�֐�
protected:
	//// �X�e�[�^�X
	void SetHelcy();													// �w���V�[�`�� ��M�^�C���A�E�g�ݒ�

//// �����o�[�ϐ�
protected:
	//// �X�e�[�^�X
	bool                    m_bDumpMode;								// �_���v�o�̓��[�h(True:�S���o��,False:1���o��)

};
