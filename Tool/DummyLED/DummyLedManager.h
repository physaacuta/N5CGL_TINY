#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\DefectLib\BaseTcpIp.h"							// TCP/IP����N���X

// ���[�J���̃N���X
#include "MainConst.h"

using namespace KizuLib;

class DummyLedManager :	public ThreadManager
{

//// ���[�J���萔
private :
	//=======================================
	// �萔��`
	//=======================================
	// �o�b�t�@ID��`
	static const int BUF_RECV				= 0;						// ��M�p�o�b�t�@
	static const int BUF_SEND				= 1;						// ���M�p�o�b�t�@

	// �|�[�gID��`
	static const int PORT_SEND				= 0;						// ���M�|�[�gID 
	//static const int PORT_RECV			= 1;						// ��M�|�[�gID 

	// �����`
	static const int ID_SEND				= 0;						// ���M����p
	//static const int ID_RECV				= 1;						// ��M����p
	
	// �ʐM�֌W��`
	static const int PARAM_SOCK_MAX			= 1;						// �\�P�b�g��
	static const int PARAM_SOCK_BUFSIZE		= 2*1024;					// �ő�o�b�t�@�T�C�Y
	static const int PARAM_RES_TIMEOUT		= 5000;						// ���M�����^�C���A�E�g
	static const int PARAM_COM_BUFSIZE		= 5;						// ���ʕ��`���T�C�Y


	// �^�C�}�[
	static const int PARAM_SOCK_TIMEOUT		= 5000;						// �ăR�l�N�g�Ԋu�i�\�P�b�g �^�C���A�E�g�j
	static const int PARAM_ANSER_TIMEOUT	= 15000; //5000;			// �����`���^�C���A�E�g
	static const int PARAM_RECHECK_TIMEOUT	= PARAM_SOCK_TIMEOUT+PARAM_ANSER_TIMEOUT+1000;	// �ă`�F�b�N�^�C���A�E�g �����̎��Ԃ��z���ď��߂Ĉُ�ɂ���
	static const int PARAM_SENDOK_TIMEOUT	= PARAM_ANSER_TIMEOUT+1000;	// ���M���҂��^�C�}�[
	static const int PARAM_SENDOK_NOWAIT_TIMEOUT = 1000;				// ���M���҂��^�C�}�[ (�����`�F�b�N�p)
	static const int PARAM_ADDTIME			= 1;						// �_���ݐώ��� 1���Œ�!! (�ύX�s��) 

	// ���̑��������
	static const int SYSLOG_BASE			= 220;						// syslog���l (�������珇�ɉ��Z����)


//// ���J�萔
public:
	// �N���X���̏��
	enum SOCK_STAT {
		SOCK_NONE = 0,		// �ؒf��
		SOCK_IDLE,			// �v���҂�
		SOCK_EXEC			// �V�[�P���X���s��
	};
	// LED���M�R�}���h���
	enum EM_LED_RECV_COMMAND {
		RECV_CHECK_BREAK = 1,											// [Ra] �f���L�� (�X�e�[�^�X�v��)
		RECV_CHECK_HEATING,												// [Rb] LED�ߔM (�X�e�[�^�X�v��)
		RECV_CHECK_ONOFF,												// [Rl] �_������ (�X�e�[�^�X�v��)
		RECV_CHECK_TEMP,												// [Rt] LED���x (�X�e�[�^�X�v��)
		RECV_EXEC_ON,													// [RX] �_���w��
		RECV_EXEC_OFF,													// [RY] �����w��
		RECV_SEND_MASTER,												// [RT] �}�X�^�[�����̂ݑ��M (RAM�ɃZ�b�g)
		RECV_SEND_VALUE,												// [RS] �����f�[�^���M (RAM�ɃZ�b�g)
		RECV_WRITE_EEPROM,												// [RW] RAM�̒����f�[�^��EEPROM(�t�@�C��)�ɏ���
		RECV_READ_EEPROM,												// [RM] EEPROM(�t�@�C��)�̒����f�[�^��Ǎ�
		RECV_READ_RAM,													// [RR] RAM�̒����f�[�^��Ǎ�
		RECV_GET_BLOCK,													// [RB] �u���b�N���擾
		RECV_NOTHING,													// [0x00]
		RECV_NON														// �s��
	};
private:
	// LED��M�R�}���h���
	enum EM_LED_SEND_COMMAND {
		SEND_NEXT_SEND_OK = 101,										// [AC] ���M����
		SEND_STATE_ON,													// [Ax] �_��
		SEND_STATE_OFF,													// [Ay] ����
		SEND_STATE_BREAK,												// [AA] �f��
		SEND_STATE_HOT,													// [AB] ���x�㏸
		SEND_STATE_SHATDOWN,											// [AD] ���M�����޳�
		SEND_STATE_OK,													// [AN] ����
		SEND_READ_EEPROM,												// [AE] EEPROM �����f�[�^ �Ǎ�
		SEND_READ_RAM,													// [AP] RAM �����f�[�^ �Ǎ�
		SEND_LED_TEMP,													// [At] LED���x�f�[�^ �Ǎ�
		SEND_GET_BLOCK,													// [Ab] �u���b�N�� �Ǎ�
		SEND_TIME_OUT,													// [AT] �ʐM�^�C���A�E�g
		SEND_NOTHING,													// [0x80] �Ȃ񂩓����ŏ���ɏo�͂����炵���Ӗ��s���̃R�[�h�B(���[�J�[���킭�A�������ĉ������Ƃ̂���)
		SEND_NON														// �s��
	};

	// �Ɩ����u�ʐM���
	enum EM_LED_MAINMODE
	{		
		MODE_NONE = 0,													// �������
		MODE_WAIT,														// �ҋ@	(�z���g�ɉ������Ă��Ȃ��Bor ���M���҂�)
		MODE_RECV_EXEC													// ��M�R�}���h�҂�
	};


private:
	//=======================================
	// �\���̒�`
	//=======================================
	// ��M�Ǘ����
	struct INF_RECV {
		int				nSize;											// �S���� [byte]
		int				nCh;											// �Ώۃ`�����l�� (1�I���W��)
		//int				nNow;										// ��M���������T�C�Y [byte]
		BYTE			cBuf[PARAM_SOCK_BUFSIZE];						// ��M�o�b�t�@
	};

	// ���M�Ǘ����
	struct INF_SEND {
		EM_LED_SEND_COMMAND	emLedCommand;								// �v���R�}���h
		bool			bSendOkUmu;										// �Ή������M�R�}���h�̗L�� (true:�L��)
		int				nSize;											// �S���̓`���� [byte]
		int				nCh;											// �Ώۃ`�����l�� (1�I���W��)
		BYTE			cBuf[PARAM_SOCK_BUFSIZE];						// ���M�o�b�t�@
	};

//// ���J�֐�
public:
	DummyLedManager(char const* cName, int nId, int nBlock, int nTempUnit);
	virtual ~DummyLedManager(void);

	// ����M����
	bool AddQue_SendCommand(EM_LED_SEND_COMMAND em, int nCh);				// 1�o�C�g�����̑��M�R�}���h��p

	// ���ʏ���
	CString GetMyName()			{ return my_sThreadName;}
	int		GetID()				{ return m_nMyID;}
	int		GetMen()			{ return m_nMyMen;}
	int		GetChNum()			{ return m_nChNum;}
	int		GetBlock(int ch)		{ return m_nBlock[ch];}
	int		GetMyIndex(int ch)		{ return m_nMyIndex[ch];}
	int		GetTempUnit(int ch)		{ return m_nTempUnit[ch];}
	void	SetTemp(int ch, int uni, int temp)			{ m_nTemp[ch][uni] = temp;}
	int		SetLedTempSts(int nCh, int nUnit)			{
															m_nTempSts[nCh][nUnit]++;
															m_nTempSts[nCh][nUnit] = (2 < m_nTempSts[nCh][nUnit] ? 0 : m_nTempSts[nCh][nUnit]);
															return m_nTempSts[nCh][nUnit];
														}
	void	SetBreak(int ch)							{
															m_nStateBreak[ch]= (1==m_nStateBreak[ch]?0:1);
															if(m_nStateBreak[ch]== 1) AddQue_SendCommand(SEND_STATE_SHATDOWN,ch);
														}
	int		SetStateTemp(int nCh)						{
															m_nStateTemp[nCh]++;
															m_nStateTemp[nCh] = (2 < m_nStateTemp[nCh] ? 0 : m_nStateTemp[nCh]);
															return m_nStateTemp[nCh];
														}
	void	SetLedOnOff(int ch, int nOnOff)				{ m_nLedOnOff[ch] = nOnOff; }

	CString GetSendCommandName(EM_LED_SEND_COMMAND em);					// ���M�R�}���h����
	CString GetRecvCommandName(EM_LED_RECV_COMMAND em);					// ��M�R�}���h����
	DummyLedManager::EM_LED_RECV_COMMAND		GetRecvCommand(char code);		// ��M�R�[�h�̓���
	DummyLedManager::EM_LED_SEND_COMMAND		GetSendCommand(char code);		// ���M�R�[�h�̓���
	CString GetMainModeName(EM_LED_MAINMODE em);						// ���C�����[�h����
	CString GetMainModeName() {return GetMainModeName(m_emMainMode);};	// ���̏��
	CString GetSendStatName(int id) const	{return mcls_pSock[id]->GetSockStateName(); }		// ���M

	// ��ʗp
	bool	IsConnect()						const {return m_bIsConnect;}
	int		GetNowScode()					const {return m_nNowScode;}
	BYTE	GetNowLedBase(int nCh)			const {return m_nNowLedBase[nCh];}
	BYTE	GetNowLedHosei(int nCh, int nBlock)	const {return m_nNowLedHosei[nCh][nBlock];}

	int		GetStateOnOff(int nCh)			const {return m_nStateOnOff[nCh];}
	int		GetStateBreak(int nCh)			const {return m_nStateBreak[nCh];}
	int		GetStateTemp(int nCh)			const {return m_nStateTemp[nCh];}
	int		GetLedTemp(int nCh, int nUnit)	const {return m_nTemp[nCh][nUnit];}
	int		GetLedTempSts(int nCh, int nUnit)	const {return m_nTempSts[nCh][nUnit];}
	int		GetLedOnOff(int nCh)			const { return m_nLedOnOff[nCh]; }

//// �����o�[�֐�
private:
	int			ThreadFirst();											// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int			ThreadLast();											// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void		ThreadEvent(int nEventNo);								// �X���b�h�C�x���g����
	void		ThreadWaitFor(int nDefWaitNum, int* nNowWaitNum);		// �X���b�h��WaitFor�҂��̒��O����  

	// ����M����
	void DeleteAllQue();												// �L���[�ɗ��܂��Ă��鑗�M�˗���S�č폜
	bool SendStart(int* retc);											// ���M�J�n
	int  RecvEnd(int id, BaseTcpIp::DELIVERY_ANS* que);					// ��M����

	// TCP/IP �ʐM����
	void SockInit();													// ���������� (��x�̂݋N��)
	void SockExit();													// �I������ (��x�̂݋N��)
	void UserClose(int nClose = 0);										// �����ؒf
	bool RecvStart(int id, int size, BYTE* pBuf, int point=0);			// �񓯊� ��M�����J�n
	void ExecAnsQue(int id, BaseTcpIp::DELIVERY_ANS* que);				// TCP/IP�N���X ���쌋�ʂ̏���

	EM_LED_SEND_COMMAND CheckBreak(int nCh);
	EM_LED_SEND_COMMAND CheckOnOff(int nCh);
	EM_LED_SEND_COMMAND CheckHeating(int nCh, int nUnit);
	EM_LED_SEND_COMMAND CheckHeating(int nCh);

	void GetMasterData(int nCh);
	

//// �����o�[�ϐ�
private:
	// �e�C���X�^���X
	BaseTcpIp*				mcls_pSock[PARAM_SOCK_MAX];					// ��M�|�[�g[0] ���M�|�[�g[1] �p����N���X
	INF_RECV				m_InfRecv;									// ��M�Ǘ����
	INF_SEND				m_InfSend;									// ���M�Ǘ����

	// �X���b�h�L���[
	ThreadQueue<INF_SEND>	mque_SendItem;								// ���M�˗��f�[�^�p�L���[

	// �Ɩ����u ���
	bool					m_bIsConnect;								// �Ɩ����u �ڑ����(true:�ڑ���(���߂Ď�M��true), false:�ؒf��)
	EM_LED_MAINMODE			m_emMainMode;								// ���݂̃��C�����[�h

	int						m_nNowScode;								// ���݂̌����\�ʏ��No (1�I���W��)
	BYTE					m_nNowLedBase[MAX_LED_CH];					// ���݂̒����f�[�^
	BYTE					m_nNowLedHosei[MAX_LED_CH][MAX_LED_BLOCK];	// ���݂̕␳�f�[�^
	int						m_nStateOnOff[MAX_LED_CH];					// �_������ (-1:�s�� 0:�_�� 1:����(����) 2:����(�ُ�))
	int						m_nStateBreak[MAX_LED_CH];					// �f���L�� (-1:�s�� 0:���� 1:�f��)
	int						m_nStateTemp[MAX_LED_CH];					// LED���M  (-1:�s�� 0:���� 1:���M 2:�V���b�g�_�E��)
	int						m_nTemp[MAX_LED_CH][MAX_TEMP_UNIT];			// LED���x
	int						m_nTempSts[MAX_LED_CH][MAX_TEMP_UNIT];		// LED���x��� (-1:�s�� 0:���� 1:���M 2:�V���b�g�_�E��)
	int						m_nLedOnOff[MAX_LED_CH];					// �_���������� (0:�v���ɏ]�� 1:�����Œ�)


	// �\�P�b�g�֘A
	BYTE					m_cSockBuf[PARAM_SOCK_MAX][2][PARAM_SOCK_BUFSIZE];	// ����M�p�o�b�t�@ [0:��M�|�[�g 1:���M�|�[�g] [0:��M�o�b�t�@ 1:���M�o�b�t�@]

	SOCK_STAT				m_eStateMain[PARAM_SOCK_MAX];				// ���C���X�e�[�^�X

	// ���̑��������
	CString					m_cYourSession;								// �����ʎq (�@���� ���X�̐؂�ւ��Ɏg�p����)
	CString					m_cMySession;								// �����ʎq (�@���� ���X�̐؂�ւ��Ɏg�p����)
	int						m_nMyID;									// ID (0�I���W��) (�Z�N�V�����̉��Ԗ�)
	int						m_nMyMen;									// �\:0 ��:1
	int						m_nChNum;									// �Ǘ��Ώۃ`�����l����

	bool					m_bUsedCh[MAX_LED_CH];						// CH�̎g�p/���g�p
	int						m_nBlock[MAX_LED_CH];						// �z���u���b�N��
	int						m_nTempUnit[MAX_LED_CH];					// LED���x�v���_��
	int						m_nMyIndex[MAX_LED_CH];						// CH���Ή�����Index
};