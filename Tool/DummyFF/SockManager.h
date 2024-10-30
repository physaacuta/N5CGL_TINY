// *********************************************************************************
//	�ʐM����Ǘ��X���b�h
//	[Ver]
//		Ver.01    2012/08/30
//
//	[����]
//		
// *********************************************************************************

#pragma once


// ���Œ�
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Include\LineCommon.h"									// �r���C���N���[�h

#include "..\..\Library\DefectLib\BaseTcpIp.h"							// TCP/IP����N���X

// �����o�[
#include "MainConst.h"
#include "SockDataExec.h"												// ����M�f�[�^�󂯓n���N���X

using namespace KizuLib;

class SockManager : public ThreadManager
{
private :

	// �o�b�t�@ID��`
	static const int BUF_RECV				= 0;						// ��M�p�o�b�t�@
	static const int BUF_SEND				= 1;						// ���M�p�o�b�t�@

	// �|�[�gID��`
	static const int PORT_RECV				= 0;						// ��M�|�[�gID 
	static const int PORT_SEND				= 1;						// ���M�|�[�gID 

	// �ʐM�֌W��`
	static const int PARAM_SOCK_MAX			= 2;						// �\�P�b�g��
	static const int PARAM_SOCK_BUFSIZE		= 65000;					// �ő�o�b�t�@�T�C�Y (65000)
	//static const int PARAM_SOCKANSER_TIMEOUT= 5*60*1000;				// �����`�� �^�C���A�E�g(T1)
	static const int PARAM_SOCK_TIMEOUT		= 30*1000;					// �\�P�b�g �^�C���A�E�g(T2)
	//static const int PARAM_HELCYSEND_TIMER	= 15*60*1000;				// �w���V�[�`�� ��������M �^�C��(T3)
	//static const int PARAM_JUKOSYO_TIMER	= 10;						// �d�̏� �^�C��[��]

	//// ��M���ʒ�`
	//static const int RECV_RET_ERR_NONE		= -9;						// ���Ȃ�ُ�(�ؒf�����K�v)
	//static const int RECV_RET_ERR_IDLE		= -1;						// �ُ�(�ؒf�͂��Ȃ�)
	//static const int RECV_RET_OK			= 0;						// 1�V�[�P���X���ׂĊ���
	//static const int RECV_RET_NEXT			= 1;						// �����M���������{���邱��
	//static const int RECV_RET_NONEXT		= 9;						// ����M���������{���邱��(=�������Ȃ�)

public :
	//// �N���X���̏��
	enum SOCK_STAT {
		SOCK_NONE = 0,		// �ؒf��
		SOCK_IDLE,			// �v���҂�
		SOCK_EXEC			// �V�[�P���X���s��
	};

//// ���J�֐�
public:
	SockManager(void);
	virtual ~SockManager(void);
	
	//// �O���A�N�Z�X
	bool	IsConnect() const				{return m_bIsConnect;}
	void    SetDumpMode(bool bmode)         { mcls_pData->SetDumpMode(bmode); }					// �_���v�o�̓��[�h�Z�b�g
	bool    GetDumpMode()                   {return mcls_pData->GetDumpMode(); }				// �_���v�o�̓��[�h�擾

	CString GetSockStatName() const			{return GetSockStatName(m_eStateMain); }			// �\�P�b�g�X�e�[�^�X����
	CString GetSendStatName() const			{return GetSendStatName(m_ExecItem.emStat); }		// ���M
	CString GetSockStatName(SOCK_STAT st) const;						// ���݂̃X�e�[�^�X����
	CString GetSendStatName(QUE_SEND_STAT st) const;					// ���݂̑��M�X�e�[�^�X����
	void SetSockDataExec(SockDataExec* pCls) {mcls_pData = pCls;}			// ����M�f�[�^�󂯓n���N���X


//// ���J�ϐ�
public:
	// �󂯓n���L���[
	ThreadQueue<Delivery>	gque_Deli;									// ���M�˗��p�L���[ 


//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	void ThreadWaitFor(int nDefWaitNum, int* nNowWaitNum);				// �X���b�h��WaitFor�҂��̒��O����  
	
	void ExecAnsQue(int id, BaseTcpIp::DELIVERY_ANS* que);				// TCP/IP�N���X ���쌋�ʂ̏���
	void QueDelete();													// �L���[���폜����



	// TPC/IP �ʐM����
	void SockInit();													// ����������
	void SockExit();													// �I������
	void UserClose();													// �����ؒf
	bool RecvStart(int id, int size, BYTE* pBuf, int point=0);			// �񓯊� ��M�����J�n

	// ����M�V�[�P���X
	bool SendLockStart(Delivery* deli);									// �Ɩ��`�� ���M (����)
	bool SendExec();													// �Ɩ��`�����M ���ʏ���
	bool RecvEnd(int id, BaseTcpIp::DELIVERY_ANS* que);					// ��M �������� (�񓯊�)

	// �ŗL����
	int SetRecvData( BYTE const* data, char* cTc ); // ��M�f�[�^����


//// �����o�[�ϐ�
private:
	//// �e�C���X�^���X
	BaseTcpIp*				mcls_pSock[PARAM_SOCK_MAX];					// ��M�|�[�g[0] ���M�|�[�g[1] �p����N���X
	SockDataExec*			mcls_pData;									// ����M�f�[�^�󂯓n���N���X
	
	//// �\�P�b�g�֘A
	BYTE					m_cSockBuf[PARAM_SOCK_MAX][2][PARAM_SOCK_BUFSIZE];	// ����M�p�o�b�t�@ [0:��M�|�[�g 1:���M�|�[�g] [0:��M�o�b�t�@ 1:���M�o�b�t�@]
	ExecItem				m_ExecItem;									// ���M���̃f�[�^


	//// �X�e�[�^�X
	bool					m_bIsConnect;								// ��������
	bool					m_bIsFfConnectErr;							// FF�T�[�o�[�������� true
	SOCK_STAT				m_eStateMain;								// ���C���X�e�[�^�X

	int						m_nSt;										// �Z�b�g����X�e�[�^�X
};
