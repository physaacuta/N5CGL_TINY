#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\DefectLib\BaseTcpIp.h"							// TCP/IP����N���X

// ���[�J���̃N���X
#include "MainConst.h"

using namespace KizuLib;

class LedManager :	public ThreadManager
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
	enum EM_LED_SEND_COMMAND {
		SEND_CHECK_BREAK = 1,											// [Ra] �f���L�� (�X�e�[�^�X�v��)
		SEND_CHECK_HEATING,												// [Rb] LED�ߔM (�X�e�[�^�X�v��)
		SEND_CHECK_ONOFF,												// [Rl] �_������ (�X�e�[�^�X�v��)
		SEND_CHECK_TEMP,												// [Rt] LED���x (�X�e�[�^�X�v��)
		SEND_EXEC_ON,													// [RX] �_���w��
		SEND_EXEC_OFF,													// [RY] �����w��
		SEND_SEND_MASTER,												// [RT] �}�X�^�[�����̂ݑ��M (RAM�ɃZ�b�g)
		SEND_SEND_VALUE,												// [RS] �����f�[�^���M (RAM�ɃZ�b�g)
		SEND_WRITE_EEPROM,												// [RW] RAM�̒����f�[�^��EEPROM(�t�@�C��)�ɏ���
		SEND_READ_EEPROM,												// [RM] EEPROM(�t�@�C��)�̒����f�[�^��Ǎ�
		SEND_READ_RAM,													// [RR] RAM�̒����f�[�^��Ǎ�
		SEND_GET_BLOCK,													// [RB] �u���b�N���擾
		SEND_NOTHING,													// [0x00]
		SEND_NON														// �s��
	};
private:
	// LED��M�R�}���h���
	enum EM_LED_RECV_COMMAND {
		RECV_NEXT_SEND_OK = 101,										// [AC] ���M����
		RECV_STATE_ON,													// [Ax] �_��
		RECV_STATE_OFF,													// [Ay] ����
		RECV_STATE_BREAK,												// [AA] �f��
		RECV_STATE_HOT,													// [AB] ���x�㏸
		RECV_STATE_SHATDOWN,											// [AD] ���M�����޳�
		RECV_STATE_OK,													// [AN] ����
		RECV_READ_EEPROM,												// [AE] EEPROM �����f�[�^ �Ǎ�
		RECV_READ_RAM,													// [AP] RAM �����f�[�^ �Ǎ�
		RECV_LED_TEMP,													// [At] LED���x�f�[�^ �Ǎ�
		RECV_GET_BLOCK,													// [Ab] �u���b�N�� �Ǎ�
		RECV_TIME_OUT,													// [AT] �ʐM�^�C���A�E�g
		RECV_CMD_ERROR,													// [Ae] �R�}���h�G���[
		RECV_NOTHING,													// [0x80] �Ȃ񂩓����ŏ���ɏo�͂����炵���Ӗ��s���̃R�[�h�B(���[�J�[���킭�A�������ĉ������Ƃ̂���)
		RECV_NON														// �s��
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
		bool			bRecvUmu;										// �Ή������M�R�}���h�̗L�� (true:�L��)
		int				nSize;											// �S���̓`���� [byte]
		int				nCh;											// �Ώۃ`�����l�� (1�I���W��)
		BYTE			cBuf[PARAM_SOCK_BUFSIZE];						// ���M�o�b�t�@
	};

	// �p�����[�^
	struct PARAM {
		int				nLedBase[MAX_SCODE][MAX_LED_CH];				// �S�̒����f�[�^ (0�`255)
		BYTE			nLedHosei[MAX_SCODE][MAX_LED_CH][MAX_LED_BLOCK];// �z���f�[�^[MAX256�R] (0�`255)
	};


//// ���J�֐�
public:
	LedManager(char const* cName, int nId, int nBlock, int nTempUnit, int nTempThr, int nTempThrDown, int nTimerCycle);
	virtual ~LedManager(void);

	// ����M����
	bool AddQue_SendCommand(EM_LED_SEND_COMMAND em, int nCh);				// 1�o�C�g�����̑��M�R�}���h��p

	bool AddQue_SetLightMastVal(int nCh, BYTE nLedBase, BYTE* nLedHosei);	// �}�X�^�[�{�z���f�[�^�Z�b�g
	bool AddQue_SetLightMastVal(int nCh, int scode);						// �}�X�^�[�{�z���f�[�^�Z�b�g (scode)
	bool AddQue_SetLightMastVal(int nCh, BYTE nLedBase, BYTE nLedHosei);	// �}�X�^�[�{�z���f�[�^�Z�b�g (�z�����͑S������l)

	bool AddQue_SetLightMaster(int nCh, int nLedBase);						// �S�̒����f�[�^�Z�b�g




	bool AddQue_SetLightValue(int nCh, BYTE* nLedHosei);				// �����f�[�^�Z�b�g (�z�����͈ꊇ�w��)

	bool AddQue_SetLightValue(int nCh, BYTE nLedHosei);					// �z���f�[�^�Z�b�g (�z�����͑S������l)
	bool AddQue_SetLightValue(int nCh, int nBlock, BYTE nLedHosei);		// �����f�[�^�Z�b�g (�z�����͌ʎw��)

	/// 2014.10.28 ����(���P�xLED�Ή�) ------------------------------------------------>>>>

	// ����M����(INDEX�w���)
	void AddQue_SendCommand_Index(EM_LED_SEND_COMMAND em, int nIndex);				// 1�o�C�g�����̑��M�R�}���h��p
	void AddQue_SetLightMastVal_Index(int nIndex, BYTE nLedBase, BYTE* nLedHosei);	// �}�X�^�[�{�z���f�[�^�Z�b�g
	void AddQue_SetLightMastVal_Index(int nIndex, int scode);						// �}�X�^�[�{�z���f�[�^�Z�b�g (scode)
	void AddQue_SetLightMastVal_Index(int nIndex, BYTE nLedBase, BYTE nLedHosei);	// �}�X�^�[�{�z���f�[�^�Z�b�g (�z�����͑S������l)
	void AddQue_SetLightMaster_Index(int nIndex, int nLedBase);						// �S�̒����f�[�^�Z�b�g
	void AddQue_SetLightValue_Index(int nIndex, BYTE* nLedHosei);					// �����f�[�^�Z�b�g (�z�����͈ꊇ�w��)
	void AddQue_SetLightValue_Index(int nIndex, BYTE nLedHosei);					// �z���f�[�^�Z�b�g (�z�����͑S������l)
	void AddQue_SetLightValue_Index(int nIndex, int nBlock, BYTE nLedHosei);		// �����f�[�^�Z�b�g (�z�����͌ʎw��)
	/// 2014.10.28 ����(���P�xLED�Ή�) <<<<------------------------------------------------
		


	// DB�擾�֘A
	bool	GetParam();													// ���I�p�����[�^�擾

	// ���ʏ���
	CString GetMyName()			{ return my_sThreadName;}
	int		GetID()				{ return m_nMyID;}
	int		GetMen()			{ return m_nMyMen;}
	int		GetChNum()			{ return m_nChNum;}
	/// 2014.10.28 ����(���P�xLED�Ή�)------------------------->>>>
	//int		GetBlock()			{ return m_nBlock;}
	int		GetBlock(int ch)		{ return m_nBlock[ch];}
	int		GetMyIndex(int ch)		{ return m_nMyIndex[ch];}
	int		GetTempUnit(int ch)		{ return m_nTempUnit[ch];}
	bool	GetDeployment(int ch)	{ return m_bIsPositiveDeployment[ch]; }
	/// 2014.10.28 ����(���P�xLED�Ή�)<<<<-------------------------

	CString GetSendCommandName(EM_LED_SEND_COMMAND em);					// ���M�R�}���h����
	CString GetRecvCommandName(EM_LED_RECV_COMMAND em);					// ��M�R�}���h����
	LedManager::EM_LED_RECV_COMMAND		GetRecvCommand(char code);		// ��M�R�[�h�̓���
	LedManager::EM_LED_SEND_COMMAND		GetSendCommand(char code);		// ���M�R�[�h�̓���
	CString GetMainModeName(EM_LED_MAINMODE em);						// ���C�����[�h����
	CString GetMainModeName() {return GetMainModeName(m_emMainMode);};	// ���̏��
	CString GetSendStatName(int id) const	{return mcls_pSock[id]->GetSockStateName(); }		// ���M

	// ��ʗp
	bool	IsConnect()						const {return m_bIsConnect;}
	int		GetNowScode()					const {return m_nNowScode;}
	BYTE	GetNowLedBase(int nCh)			const {return m_nNowLedBase[nCh];}
	BYTE	GetNowLedHosei(int nCh, int nBlock)	const {return m_nNowLedHosei[nCh][nBlock];}
	BYTE	GetNowLedValue(int nCh, int nBlock)	const {return GET_LED_VAL(m_nNowLedBase[nCh],m_nNowLedHosei[nCh][nBlock]);}

	int		GetStateOnOff(int nCh)			const {return m_nStateOnOff[nCh];}
	int		GetStateBreak(int nCh)			const {return m_nStateBreak[nCh];}
	int		GetStateTemp(int nCh)			const {return m_nStateTemp[nCh];}
	int		GetLedTemp(int nCh, int nUnit)	const {return m_nTemp[nCh][nUnit];}


	// ���[�v�Z�o
	void GetChLoop(int nCh, int& ii_s, int& ii_e)					// Ch�w�莞�̑ΏێZ�o
	{
		if(0==nCh) { ii_s=1; ii_e=MAX_LED_CH; }
		else	   { ii_s=ii_e=nCh;}
	}

	//// 2014.10.28 ����(���P�xLED�Ή�)----------------------->>>
	bool IsTargetIndex(int nCh, int nIndex){
		// ���g�pCH�̏ꍇ��False
		if( !m_bUsedCh[nCh-1] )		return false;

		if(DEF_ALL_INDEX==nIndex || m_nMyIndex[nCh-1] == nIndex)	return true;
		else														return false;
	}
	//// 2014.10.28 ����(���P�xLED�Ή�)<<<-----------------------


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
	
	// �_���ݐώ��ԁA���x�֌W
	void AddTime_Start(int nCh);										// �ݐϊJ�n
	void AddTime_Stop(int nCh);											// �ݐϒ�~
	void AddTime_Exec(int nCh);											// �ݐω��Z
public:
	void AddTime_Reset(int nCh);										// �ݐσ��Z�b�g
	void AddTime_Reset_Index(int nIndex);								// �ݐσ��Z�b�g(Index�w���)
private:	
	void SetTemp(int nCh);												// LED���݉��x

	void TurnHosei(BYTE * nHosei, int nCnt);							// �Ɠx�␳�l�̃o�C�i����𔽓]			// 2014.12.01 ����(���P�xLED�Ή�)


//// �����o�[�ϐ�
private:
	// �e�C���X�^���X
	BaseTcpIp*				mcls_pSock[PARAM_SOCK_MAX];					// ��M�|�[�g[0] ���M�|�[�g[1] �p����N���X
	INF_RECV				m_InfRecv;									// ��M�Ǘ����
	INF_SEND				m_InfSend;									// ���M�Ǘ����

	// �����V�O�i��
	HANDLE					m_tmSendAnser;								// �����`���҂� �V�O�i���^�C�}�[
	HANDLE					m_tmSendOk;									// ���M���҂� �V�O�i���^�C�}�[
	HANDLE					m_tmStateCheck;								// �Ɩ���Ԓ�����`�F�b�N �V�O�i���^�C�}�[
	HANDLE					m_tmReCheck;								// �ă`�F�b�N�^�C���A�b�v �V�O�i���^�C�}�[ (�����Ńz���g��NG�����f)
	HANDLE					m_tmAddTime[MAX_LED_CH];					// �_���ݐώ���
	HANDLE					m_evUserClose;								// �����ؒf�V�O�i��

	// �X���b�h�L���[
	ThreadQueue<INF_SEND>	mque_SendItem;								// ���M�˗��f�[�^�p�L���[


	// �@��ID
	int						m_nIdConnect;								// �Ɩ����u �ʐM���
	int						m_nIdBreak[MAX_LED_CH];						// �f��	
	int						m_nIdHotWar[MAX_LED_CH];					// ���M���o
	int						m_nIdHotErr[MAX_LED_CH];					// ���M�V���b�g�_�E��
	int						m_nIdOnOff[MAX_LED_CH];						// LED�_���������
	int						m_nIdValNow[MAX_LED_CH];					// ���ݐݒ�P�x�l
	int						m_nIdTotalTime[MAX_LED_CH];					// �ғ��݌v����
	int						m_nIdTemp[MAX_LED_CH][MAX_TEMP_UNIT];		// ���x
	int						m_nIdTempSt[MAX_LED_CH][MAX_TEMP_UNIT];		// ���x���

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

	PARAM					m_Param;									// �p�����[�^


	// �\�P�b�g�֘A
	BYTE					m_cSockBuf[PARAM_SOCK_MAX][2][PARAM_SOCK_BUFSIZE];	// ����M�p�o�b�t�@ [0:��M�|�[�g 1:���M�|�[�g] [0:��M�o�b�t�@ 1:���M�o�b�t�@]

	SOCK_STAT				m_eStateMain[PARAM_SOCK_MAX];				// ���C���X�e�[�^�X
	long					m_nNowEventNo[PARAM_SOCK_MAX];				// ���� �V�[�P���X�������̃C�x���gNo
	bool					m_bSendStatus;								// ���M�����X�e�[�^�X(True:����,False:���s)
	int						m_nNowCh;									// ���M
	int						m_nEndCh;									// ���M

	// ���̑��������
	CString					m_cMySession;								// �����ʎq (�@���� ���X�̐؂�ւ��Ɏg�p����)
	int						m_nMyID;									// ID (0�I���W��) (�Z�N�V�����̉��Ԗ�)
	int						m_nMyMen;									// �\:0 ��:1
	int						m_nChNum;									// �Ǘ��Ώۃ`�����l����

	/// 2014.10.28 ����(���P�xLED�Ή�)------------------------->>>>
	//int					m_nBlock;									// �z���u���b�N��
	//int					m_nTempUnit;								// LED���x�v���_��

	int						m_nChStart;									// �擪�`�����l���̕ЖʒP�ʂ�ID(1�I���W��)
	bool					m_bUsedCh[MAX_LED_CH];						// CH�̎g�p/���g�p
	int						m_nBlock[MAX_LED_CH];						// �z���u���b�N��
	int						m_nTempUnit[MAX_LED_CH];					// LED���x�v���_��
	int						m_nMyIndex[MAX_LED_CH];						// CH���Ή�����INDEX(�P��ɕ���CH�g���ꍇ�A����INDEX���Z�b�g)
	bool					m_bIsPositiveDeployment[MAX_LED_CH];		// �Ɠx�␳�l���W�J�t���O(True:DB����擾�����o�C�i����DS��WS�����ɓW�J�AFalse:�t�W�J)
	/// 2014.10.28 ����(���P�xLED�Ή�)<<<<-------------------------
	int						m_nTempThr;									// ���x臒l [�x]
	int						m_nTempThrDown;								// ����OFF���x臒l [�x]
	int						m_nTimerCycle;								// ���x��荞�ݎ��� [�b]

};
