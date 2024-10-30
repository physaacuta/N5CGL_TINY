#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\Kizu232C\Rs232cBase.h"							// RS-232C����N���X

// ���[�J���̃N���X
#include "MainConst.h"

using namespace KizuLib;
using namespace Kizu232C;
class ChillerManager : public ThreadManager
{

//// ���[�J���萔
private :
	// RS232C�ݒ�
	static const int PARAM_232C_SENDBUF_SIZE		= 256;				// 232C�̑��M�o�b�t�@�T�C�Y
	static const int PARAM_232C_RECVBUF_SIZE		= 256;				// 232C�̎�M�o�b�t�@�T�C�Y
	
	// �^�C�}�[
	static const int PARAM_TIMEOUT_RETRY			= 5000;				// ���g���C����
	static const int PARAM_TIMEOUT_ANSER			= 30000;			// �����A���T�[�^�C�}�[



//// ���J�萔
public:
	// �`���[���M�R�}���h���
	enum EM_CHILER_SEND_COMMAND {
		SEND_CHECK_NOW_TEMP = 1,										// [M1] �t������l
		SEND_CHECK_SET_TEMP,											// [S1] �t���ݒ�l
		SEND_CHECK_MODE,												// [JO] �^�]���
		SEND_CHECK_ALM,													// [ER] �x��M��
		SEND_CHECK_NON
	};

	// �`���[��M�R�}���h��ʁi���M�Ɠ����͂��j�@����ς�邩������Ȃ��̂ňꉞ����
	enum EM_CHILER_RECV_COMMAND {
		RECV_CHECK_NOW_TEMP = 1,										// [M1] �t������l
		RECV_CHECK_SET_TEMP,											// [S1] �t���ݒ�l
		RECV_CHECK_MODE,												// [JO] �^�]���
		RECV_CHECK_ALM,													// [ER] �x��M��
		RECV_CHECK_NON
	};

	// �`���[���u�ʐM���
	enum EM_CHILER_MAINMODE
	{		
		MODE_NONE = 0,													// �������
		MODE_WAIT,														// �ҋ@	(�z���g�ɉ������Ă��Ȃ��Bor ���M���҂�)
		MODE_RECV_EXEC,													// ��M�R�}���h�҂�
		MODE_RETRY														// �I�[�v�����g���C��
	};

	// �`���[���u�^�]���[�h
	enum EM_CHILER_UNTENMODE
	{		
		MODE_STOP = 0,													// �S��~��
		MODE_COOL,														// �N�[���[�^�]��
		MODE_POMP,														// �|���v�P�Ɖ^�]��
		MODE_NON
	};
	
	//=======================================
	// �\���̒�`
	//=======================================
	// ��M�Ǘ����
	struct INF_RECV {
		int				nSize;											// �S���� [byte]
		int				nNow;											// ��M���������T�C�Y [byte]
		BYTE			cBuf[PARAM_232C_RECVBUF_SIZE];					// ��M�o�b�t�@
	};
	// ���M�Ǘ����
	struct INF_SEND {
		EM_CHILER_SEND_COMMAND	emChilerCommand;						// �v���R�}���h
		int				nSize;											// �S���̓`���� [byte]
		BYTE			cBuf[PARAM_232C_SENDBUF_SIZE];					// ���M�o�b�t�@
	};


//// ���J�֐�
public:
	ChillerManager(char const* cName, int nTempThr, int nTimerCycle);
	virtual ~ChillerManager(void);

	// ����M����
	bool AddQue_SendCommand(EM_CHILER_SEND_COMMAND em);					// ���M�R�}���h


	// ���ʏ���
	CString GetMyName() { return my_sThreadName;}
	CString GetSendCommandName(EM_CHILER_SEND_COMMAND em);				// ���M�R�}���h����
	CString GetRecvCommandName(EM_CHILER_RECV_COMMAND em);				// ��M�R�}���h����
	EM_CHILER_SEND_COMMAND GetSendCommand(char* code);					// ���M�R�[�h�̓���
	EM_CHILER_RECV_COMMAND GetRecvCommand(char* code);					// ��M�R�[�h�̓���

	CString GetMainModeName(EM_CHILER_MAINMODE em);						// ���C�����[�h����
	CString GetMainModeName() {return GetMainModeName(m_emMainMode);};	// ���C�����[�h����(���̏��)
	CString GetUntenName(EM_CHILER_UNTENMODE em);						// �`���[�^�]��Ԗ���
	CString GetUntenName() {return GetUntenName(m_emUnten);};			// �`���[�^�]��Ԗ���(���̏��)

	// ��ʗp
	bool	IsOpen()			const {return m_bIsOpen;}
	bool	IsConnect()			const {return m_bIsConnect;}
	double	GetNowTemp()		const {return m_dNowTemp;}
	double	GetSetTemp()		const {return m_dSetTemp;}
	CString	GetAlmNo()			const {return m_sAlmNo;}


//// ���[�J���֐�
private:
	int			ThreadFirst();											// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int			ThreadLast();											// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void		ThreadEvent(int nEventNo);								// �X���b�h�C�x���g����
	void		ThreadWaitFor(int nDefWaitNum, int* nNowWaitNum);		// �X���b�h��WaitFor�҂��̒��O����  


	// RS-232C�֘A
	bool RS_Open(bool bLogDsp);											// 232C �I�[�v��
	void RS_Close();													// 232C �N���[�Y
	bool RS_Recv(int pos = 0, int size = 3, int total = 3);				// 232C �񓯊� ��M
	bool RS_Send();														// 232C ���� ���M

	// ����M����
	void DeleteAllQue();												// �L���[�ɗ��܂��Ă��鑗�M�˗���S�č폜
	bool SendStart();													// ���M�J�n
	int  RecvEnd();														// ��M����


//// ���[�J���ϐ�
private:
	// �e�C���X�^���X
	Rs232cBase*				mcls_p232c;									// 232c�ڑ��C���X�^���X

	// 232C�֘A
	INF_RECV				m_InfRecv;									// ��M�Ǘ����
	INF_SEND				m_InfSend;									// ���M�Ǘ����

	// �����V�O�i��
	HANDLE					m_evRecv;									// ��M���� �V�O�i��
	HANDLE					m_tmReTry;									// ���g���C �V�O�i���^�C�}�[
	HANDLE					m_tmCycleExec;								// ������`�F�b�N �V�O�i���^�C�}�[
	HANDLE					m_tmSendAnser;								// ���M�A���T�[�҂��V�O�i���^�C�}�[


	// �X���b�h�L���[
	ThreadQueue<INF_SEND>	mque_SendItem;								// ���M�˗��f�[�^�p�L���[


	// �@��ID
	int						m_nIdConnect;								// ������
	int						m_nIdState;									// �^�]���	
	int						m_nIdErr;									// �ُ���
	int						m_nIdTemp;									// ���x
	int						m_nIdTempSt;								// ���x���


	// �`���[���u ���
	bool					m_bIsOpen;									// 232C������I�[�v����true
	bool					m_bIsConnect;								// �`���[���u �ڑ����(true:�ڑ���(���߂Ď�M��true), false:�ؒf��)

	EM_CHILER_MAINMODE		m_emMainMode;								// ���݂̃��C�����[�h
	double					m_dNowTemp;									// �t������l
	double					m_dSetTemp;									// �t���ݒ�l
	EM_CHILER_UNTENMODE		m_emUnten;									// �^�]���
	CString					m_sAlmNo;									// �x��ԍ�
	bool					m_bLimitTemp;								// �`���[������x�ُ� false:�ُ�
	bool					m_bAlmMachine;								// �`���[���u�ُ�F�� false:�ُ�

	// ���̑��������
	CString					m_cMySession;								// �����ʎq (�@���� ���X�̐؂�ւ��Ɏg�p����)
	int						m_nTempThr;									// ���x臒l [�x]
	int						m_nTimerCycle;								// ���x��荞�ݎ��� [�b]
	int						m_nChilerType;								// �`���[�̌^��(0:RKE1500B-VW, 1:RKE3750B-V�j�����ݒ莞0

};
