#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�W���[

#include "..\..\Library\KizuODBC\OdbcBase.h"							// DB����N���X
#include "..\..\Library\Kizu232C\Rs232cBase.h"							// RS-232C����N���X

// ���[�J��
#include "MainConst.h"													// ���N���X�萔�w�b�_�[

using namespace KizuLib;
using namespace KizuODBC;
using namespace Kizu232C;

class LinkswManager : public ThreadManager
{
//// �����o�[�萔
private:
	//------------------------------------------
	// �Œ�萔
	//------------------------------------------
	// RS232C�ݒ�
	static const int PARAM_232C_SENDBUF_SIZE		= 1024;				// 232C�̑��M�o�b�t�@�T�C�Y
	static const int PARAM_232C_RECVBUF_SIZE		= 1024;				// 232C�̎�M�o�b�t�@�T�C�Y
	
	// �^�C�}�ݒ�
	static const int PARAM_TIMEOUT_RETRY			= 3000;				// ���g���C����
	static const int PARAM_TIMEOUT_ANSWER			= 5000;				// �����`���^�C���A�E�g����

	// �N���v��
	#define	FACT_CYCLE_CHECK						-1					// ������`�F�b�N
	#define	FACT_COUNTER_CHECK						-2					// �J�E���^�`�F�b�N

	// �J���������N�X�C�b�`�����`��
	#define	R_OK									"OK"				// �����敪
	#define	CR										0x0D				// �I�[�R�[�h

	// ���C�����[�h
	enum ENUM_MAINMODE
	{
		MODE_NONE = 0,													// �������
		MODE_OPEN_CHECK,												// �I�[�v���`�F�b�N
		MODE_START_UP,													// ������
		MODE_START_INIT,												// �����J�n�ݒ�
		MODE_SLAVE_JOIN,												// �r���Q���ݒ�
		MODE_COUNTER_RESET,												// �J�E���^���Z�b�g
		MODE_COUNTER_CHECK,												// �J�E���^�`�F�b�N
		MODE_CYCLE_CHECK,												// ������`�F�b�N
		MODE_WAIT,														// �ݒ�҂�
		MODE_RETRY,														// �I�[�v�����g���C��
		MODE_CHANGE_AOC_SERIALSPEED,									// AOC�̃V���A�����x�ύX
		MODE_START_SELECT_INIT,											// �����J�n�ݒ�(�J�����I��ݒ�)
	};

//// ���J�萔
public:

	// 2017.02.27 Master/Slave��DB�w�� ---------->>>>

	// CH�I�����[�h
	enum ENUM_CH_SELECT_MODE
	{
		CH_SELECT_NON = 0,												// ���I��
		CH_SELECT_MASTER,												// Master�w��
		CH_SELECT_SLAVE													// Slave�w��
	};
	// 2017.02.27 Master/Slave��DB�w�� <<<<----------


	//------------------------------------------
	// �Œ�\����
	//------------------------------------------
	// �p�����[�^
	struct PARAM
	{
		// ����
		short				nLineBuffer;								// �i�[�o�X�����W�X�^ (1,2,3,4,6,8,10)	�� �J�����^�b�v��
		short				nYSize;										// Ysize���W�X�^						�� �c�t���[���摜�T�C�Y [pixel]

		// �J���������N�X�C�b�`�ݒ�
		struct LINKSW
		{
			BOOL			bStatus;									// �X�e�[�^�X (TRUE:�X�C�b�`�}�X�^ FALSE:�X�C�b�`�X���[�u)
			short			nSelectCam[MAX_CAMLINKSW_OUTPUT];			// �J�����I�� (0:�o�͒�~ 1�`8:����CH)

			// 2017.02.27 Master/Slave��DB�w�� ---------->>>>
			ENUM_CH_SELECT_MODE emChSelMode[MAX_CAMLINKSW_OUTPUT];
			// 2017.02.27 Master/Slave��DB�w�� <<<<----------

			// 2020.05.07 �ΏۃJ�����̂ݐݒ� ---------->>>>
			BOOL			bChSelCam[MAX_CAMLINKSW_OUTPUT];			// �J�����I��ݒ� (TRUE:�Ώ� FALSE:�ΏۊO) ��SLAVE��荞�ݎ��̂�
			// 2020.05.07 �ΏۃJ�����̂ݐݒ� <<<<----------

		} linksw[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];

		// �Z�o
		BOOL				bCamInputCh[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA][MAX_CAMLINKSW_INPUT];			// �J�������́^������Ch (TRUE:���� FALSE:������)
		short				nCamSelBoardNo[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA][MAX_CAMLINKSW_ALLINPUT];	// �J�����I���{�[�hNo (1�`15:�{�[�hNo 16:Arria5)

		//AOC�V���A���֌W
		int					nSerialSpeeds[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA*MAX_CAMLINKSW_INPUT];			// AOC�ɐݒ肷��V���A�����x

		int GetSerialSpeed( int nInput = -1) const 
		{
			int speed = 9600;
			if(0 <= nInput && nInput < _countof(nSerialSpeeds)){
				speed = nSerialSpeeds[nInput];
			}
			return speed;
		}
		int nSamplingDelay(int nInput=-1) const {
			return 0x7ff/(GetSerialSpeed(nInput)/9600);
		}
		int nSamplingTiming(int nInput=-1) const {
			if( nInput < 0 ) return 0x1046;
			return 0x1046/(GetSerialSpeed(nInput)/9600);
		}
	};


//// ���J�֐�
public:
	LinkswManager(void);
	virtual ~LinkswManager(void);

	void SetClass(int nUnit, char* sSession);							// ���N���X�Z�b�g

	// �X���b�h�L���[
	ThreadQueue<COMMON_QUE>	gque_Deli;									// �󂯓n���f�[�^�p�L���[

	// �O���A�N�Z�X
	bool IsConnect()			const { return m_bIsConnect; }			// �ڑ����H

	// Get
	const PARAM & GetParamInf()	const { return m_Param; }				// �p�����[�^

	CString GetMainModeName(ENUM_MAINMODE emMainMode);					// �w�胂�[�h���̎擾
	CString GetMainModeName();											// �����[�h���̎擾

	int GetDieTemp(int board)	const { return m_nDieTemp[board]; }		// FPGA�_�C���x
	int GetAmbTemp(int board)	const { return m_nAmbTemp[board]; }		// ���͉��x
	__int64 GetCounterNow(int Fpga, int board) const { return m_nCounterNow[Fpga][board]; } // ���݃J�E���^�l
	__int64 GetCounterSet(int Fpga, int board) const { return m_nCounterSet[Fpga][board]; } // �ݒ�J�E���^�l

	// Set
	void SetCounterCheck()												// �J�E���^�`�F�b�N�C�x���g�ʒm
	{
		SetEvent(m_evCounterCheck);
	}
	void SetCCMode(BOOL bMode)											// CC���[�h�ݒ�
	{
		m_bCCMode = bMode;
	}

	// ������Ή��p
	CString GetMyName() { return my_sThreadName; }
	CString GetMySession() { return m_cMySession; }

	// 2017.02.27 Master/Slave��DB�w�� ---------->>>>

	CString GetChSelectModeName(ENUM_CH_SELECT_MODE em)					// CH�I�����[�h����
	{
		if( CH_SELECT_NON    == em) { return "���I��"; }
		if( CH_SELECT_MASTER == em) { return "Master�w��"; }
		if( CH_SELECT_SLAVE	 == em) { return "Slave�w��"; }

		return "�s��";
	}
	// 2017.02.27 Master/Slave��DB�w�� <<<<----------

	bool IsEnableRapidSerial() { return m_bEnableRapidSerial; }			// �����ʐM�̗L��/����


//// �����o�[�֐�
private:
	// �X���b�h�����`
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����

	// �V�[�P���X����
	bool SetDataMailCommand(COMMON_QUE * pQue);							// �R�}���h����
	bool CheckNextSend();												// �����M����
	bool CheckDataRecv();												// ��M�`���`�F�b�N
	bool CheckDataRecv_AOC();											// ��M�`���`�F�b�N(AOC�ݒ�j

	void ChangeWaitEventNum(bool bNowSend)								// �҂��C�x���g���ύX
	{
		if( bNowSend )	ChangeWaitNum( m_nEventWaitNum - 3 );
		else			ChangeWaitNum( m_nEventWaitNum );
	}

	// �O���ʒm
	void Send_ToDefect_StartInit(bool bFlg=true);						// �����Ǘ� �� �J���������N�X�C�b�`�ݒ芮���ʒm
	void Send_ToDefect_SlaveJoin();										// �����Ǘ� �� �r���Q���ݒ芮���ʒm
	void Send_ToDefect_SelectInit(bool bFlg=true);						// �����Ǘ� �� �J�����I��ݒ芮���ʒm 20190205 Add

	// 232C�֘A
	bool Open(bool bLogDsp);											// 232C �I�[�v��
	void Close();														// 232C �N���[�Y
	bool Recv();														// 232C �񓯊� ��M
	bool Send();														// 232C �񓯊� ���M


	bool DirectSendSpeedChange();										// LinkSwitch���x�ύX�B

	// �p�����[�^�֘A
	bool GetParam();													// ���I�p�����[�^�擾 (�A���A�����_�ł͓��I�p�����[�^�͖����I�I)
	bool GetParamStatic();												// �ÓI�p�����[�^�擾

	// 2017.02.27 Master/Slave��DB�w�� ---------->>>>
	void GetParamIni();																// Ini�p�����[�^�𔽉f
	void SetForciblyMaster(const int nId, const int nCh, const int nSelectCam);		// �����I��Master�ݒ�
	// 2017.02.27 Master/Slave��DB�w�� <<<<----------

	bool CalcParam();													// �Z�o�p�����[�^�擾

//// �����o�[�ϐ�
private:
	// �e�C���X�^���X
	Rs232cBase *			mcls_p232c;									// 232c�ڑ��C���X�^���X

	// �V�O�i��
	HANDLE					m_evRecvEnd;								// ��M�����C�x���g�F���p
	HANDLE					m_evSendEnd;								// ���M�����C�x���g�F���p
	HANDLE					m_evReTry;									// �Đڑ��v���C�x���g
	HANDLE					m_evAnswerTimeOut;							// �����`���҂��^�C���A�E�g�C�x���g
	HANDLE					m_evTimerCycleCheck;						// ������`�F�b�N�^�C�}�C�x���g
	HANDLE					m_evCounterCheck;							// �J�E���^�`�F�b�N�C�x���g

	// 232C�֘A
	char					m_cRecvBuf[PARAM_232C_RECVBUF_SIZE];		// ��M�o�b�t�@
	char					m_cSendBuf[PARAM_232C_SENDBUF_SIZE];		// ���M�o�b�t�@

	// �X�e�[�^�X�֘A
	ENUM_MAINMODE			m_emMainMode;								// ���݂̃��C�����[�h
	int						m_nSubMode;									// ���݂̃T�u���[�h
	int						m_nCamMode;									// ���݂̃J�����䐔�J��Ԃ����[�h
	int						m_nApxMode;									// ���݂̃{�[�h�䐔�J��Ԃ����[�h
	int						m_nAocMode;									// ���݂�AOC�䐔�J��Ԃ����[�h
	int						m_nAocSubMode;								// ���݂�AOC�T�u���[�h
	int						m_nRegSizeMode;								// ���݂̃��W�X�^�T�C�Y�J��Ԃ����[�h (�� 1byte���傫���T�C�Y�̃��W�X�^�A�N�Z�X�Ɏg�p)
	int						m_nSwMode;									// ���݂̃X�C�b�`FPGA�䐔�J��Ԃ����[�h

	// �J���������N�X�C�b�`���
	bool					m_bIsConnect;								// �ڑ���� (true:�ڑ��� false:�ؒf��)
	bool					m_bInitialSpeedChange;

	// ���̑��������
	int						m_nEventWaitNum;							// �C�x���g�ݒ萔
	BOOL					m_bCCMode;									// CC���[�h 
																		// FALSE : �J�����R���g���[��FPGA��EXSYNC
																		// TRUE  : �t���[���O���o��CC1,CC2
	int						m_nCycleTime;								// ������`�F�b�N�^�C�}���� [msec]

	short					m_nSelectCam[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA][MAX_CAMLINKSW_OUTPUT];
	short					m_nSelectTorB[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA][MAX_CAMLINKSW_OUTPUT];

	// �n�[�t�őΉ�
	bool					m_bHalfFlg[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];

	// ������Ή�
	int						m_nUnit;									// �@��ԍ�(0�I���W��)
	CString					m_cMySession;								// �����ʎq (Ini�ݒ荀��)
	//CString					m_cPrmCommonTblName;						// �p�����[�^�e�[�u����
	//int						m_nMyCamNum;								// ���N���X�S�� �J�����䐔
	//int						m_nMyCamIdStart;							// ���N���X�S�� �J����ID�J�n�ʒu (0�I���W��)

	//// �@���Ԋ֘A
	// FPGA�_�C���x
	int						m_nDieTempLimit[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];	// ���x臒l [��]
	int						m_nDieTempID[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];		// ���xID
	int						m_nDieTempStID[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];	// ���x���ID
	int						m_nDieTemp[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];		// ���x [��]
	bool					m_bDieTempSt[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];		// ���x���
	// ���͉��x
	int						m_nAmbTempLimit[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];	// ���x臒l [��]
	int						m_nAmbTempID[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];		// ���xID
	int						m_nAmbTempStID[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];	// ���x���ID
	int						m_nAmbTemp[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];		// ���x [��]
	bool					m_bAmbTempSt[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA];		// ���x���
	// Opt-C:Link�X�e�[�^�X
	int						m_nOptCLinkStID[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA][MAX_CAMLINKSW_INPUT][2];	// Opt-C:Link�X�e�[�^�XID
	bool					m_bOptCLinkSt[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA][MAX_CAMLINKSW_INPUT][2];	// Opt-C:Link�X�e�[�^�X

	// �p�����[�^�֘A
	PARAM					m_Param;									// �p�����[�^

	// �r���Q���֘A
	int						m_nSlaveJoinTorB;							// �r���Q���\���敪
	DWORD					m_nSlaveJoinFNo;							// �r���Q���t���[��No (0�I���W��)
	DWORD					m_nResetFNo;								// �J�E���^���Z�b�g�t���[��No (0�I���W��)
	__int64					m_nCounterNow[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA][MAX_CAMLINKSW_OUTPUT];	// �r���Q�����W�X�^���ݒl
	__int64					m_nCounterSet[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA][MAX_CAMLINKSW_OUTPUT];	// �r���Q�����W�X�^�ݒ�l

	// Opt-C:Link�X�e�[�^�X�֘A
	BYTE					m_nOptCLinkSt[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA][MAX_CAMLINKSW_ALLINPUT][2];			// Opt-C:Link�X�e�[�^�X���ݒl
	BOOL					m_bOptCLink_RxSt[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA][MAX_CAMLINKSW_ALLINPUT][2];		// Opt-C:Link RxStatus���
	BOOL					m_bOptCLink_LinkSt[NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA][MAX_CAMLINKSW_ALLINPUT][2];	// Opt-C:Link LinkStatus���

	// �������[�h�ւ̐ؑ�
	bool					m_bEnableRapidSerial;						// �����ʐM(115200bps)�̗L��/����

	// �v���ɑ΂��鉞�����ʗp
	COMMON_QUE				m_RecvQue;									// ���[���X���b�g�ʒm 20190205 Add


};
