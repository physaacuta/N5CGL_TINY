#pragma once

// STL
#include <vector>														// ���X�g�p

// ���Œ�
#include "..\..\Include\LineCommon.h"									// �r���C���N���[�h
#include "..\..\Library\KizuLib\KizuLib.h"								// �r�����C�u����
#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// �}�N���W
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// �X�^�e�B�b�N���C�u�����W
#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ���O�Ǘ��N���X

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLib\Class\WorkThread.h"						// ���[�J�[�X���b�h�N���X
#include "..\..\Library\KizuLib\Class\ThreadQueue.h"					// �X���b�h�L���[�N���X

#include "..\..\Library\KizuODBC\OdbcBase.h"							// DB����N���X
#include "..\..\Library\Kizu232C\Rs232cBase.h"							// RS-232C����N���X

#include "..\..\Include\ToTbl.h"										// �������L�������e�[�u�� �C���N���[�h

#include "MainConst.h"

using namespace KizuLib;
using namespace KizuODBC;
using namespace Kizu232C;



class CameraManager : public WorkThread
{

private :
	//------------------------------------------
	// �Œ�萔
	//------------------------------------------
	// RS232C�ݒ�
	static const int PARAM_232C_SENDBUF_SIZE		= 1024;				// 232C�̑��M�o�b�t�@�T�C�Y
	static const int PARAM_232C_RECVBUF_SIZE		= 1024;				// 232C�̎�M�o�b�t�@�T�C�Y
	
	static const int PARAM_TIMEOUT_RETRY			= 3000;				// ���g���C����
	static const int PARAM_TIMEOUT_ANSER			= 5000;				// �����`���^�C���A�E�g����


#ifdef AGCOFF
	// �I�����ԋ@�\OFF���̃f�t�H���g�l�Z�b�g�p �i��ʂ���̘I�����ԕύX�̂ݑΉ�������ׂ̑Ή��j
	static const int AGCOFF_DEF_InitilaExpTime		= 1800;				// �I�����ԏ����l [25��s]
	static const int AGCOFF_DEF_AGCstep				= 5;				// AGC�X�e�b�v�T�C�Y
#endif


	// �J�����R���g���[���[�����`��
	//#define R_SIZE										2				// ��M�`���T�C�Y
	#define R_NG										"?\r"			// �R�}���h���ߎ��s
	#define R_OK										"#\r"			// �R�}���h����I��
	#define R_NOTHING									">\r"			// �R�}���h�`������
	#define CR											0x0D			// �I�[�R�[�h

	// ���C�����[�h
	enum ENUM_MAINMODE
	{
		MODE_NONE = 0,													// �������
		MODE_OPEN_CHECK,												// �I�[�v���`�F�b�N
		MODE_START_INIT,												// ������
		MODE_CHECK_INIT,												// �Z��������
		MODE_WAIT,														// �ݒ�҂�

		MODE_PULSE_ON,													// �p���X�o�͊J�n
		MODE_PULSE_OFF,													// �p���X�o�͒�~

		MODE_RO,														// �I�����Ԑݒ蒆
		MODE_RETRY,														// �I�[�v�����g���C��
		MODE_MANYU
	};

	// AGC�Z�b�g�R�}���h�p
	struct AGCINF {
		int				nSetCnt;										// ���M����

		// ���M���
		struct AGCINFDATA {
			int			camid;											// ���M����J����ID (0�I���W��)
			int			nExpTime;										// �I������
		} AgcInfData[NUM_CAMERA];
	};
	



public:
	//------------------------------------------
	// �Œ�\����
	//------------------------------------------
	// �p�����[�^
	struct PARAM {
		// �J���������N�X�C�b�`�ݒ� (�� �J�����R���g���[���̏ꍇ�ł��g�p)
		int					nSwitchMasterID[NUM_CAMLINKSW];				// �X�C�b�`�}�X�^��FPGA�{�[�hID
		char				cSwitchMasterID[NUM_CAMLINKSW][4];			// �X�C�b�`�}�X�^��FPGA�{�[�hID ("@+ID"�`���Ŋi�[)

		// ���� (ײ�PLG,�\,��,�e�X�g�p�X���̏�)
		int					nPlgMul[4];									// �J����PLG���{��
		int					nPlgDiv[4];									// �J����PLG������
		int					nPlgPulse[4];								// �J����PLG�M������ (0:���� 1:TTL)
		int					nPlgIsou[4];								// �J����PLG�ʑ� (0:2�� 1:�P��)
		int					nPlgIsouSa[4];								// �J����PLG�ʑ��� (0:�A�b�v�J�E���g 1:�_�E���J�E���g)

		// ����
		int					nPlgTestInterval;							// �J����PLG�e�X�g�p���X���� (�P�x���胂�[�h���̎���) [��s]	
		int					nExSyncLT;									// LT�ŏ����� [25��s]
		int					nExSyncMinH;								// LT�����̍ŏ��l [25��s]
		int					nExSyncMinL;								// LT�����̍ő�l�̕� �ő�l=(nExSyncLT-nExSyncMinL) [25��s]

		BOOL				bScodeChg;									// �\�ʏ�ԕω����f (false:OFF true:ON)

// AGCOFF ���ɂ́A�f�t�H���g�l���Z�b�g���� ------>>
		// �J�����Z�b�g�E�J�����p�x�E�\�ʏ�ԕ�
		int					nInitialExpTime[MAX_SCODE][NUM_CAMERA];		// �I�����ԏ����l [�����\�ʏ��][�J�����䐔] [25��s]
#ifndef VER_IDULE_CAMERA
		// �\�ʏ�ԕ�
		double				dAGCstep[MAX_SCODE];						// AGC�X�e�b�v�T�C�Y[�����\�ʏ��]
#else
		// �\�ʏ�ԁE�J�����Z�b�g��
		double				dAGCstep[MAX_SCODE][NUM_CAMERA];			// AGC�X�e�b�v�T�C�Y[�����\�ʏ��][�J�����䐔]
#endif
//<<-----------------------------------------------
	};


public:
	CameraManager(void);
	virtual ~CameraManager(void);

	void Start() { WorkThread::Start(MainThread); };					// �X���b�h�J�n
	void Stop()  { WorkThread::Stop(); };								// �X���b�h�I��
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; };			// ���O�t�@�C���Z�b�g
	void SetClass(int nUnit, char* sSession);							// ���N���X�Z�b�g


	// �X���b�h�L���[
	ThreadQueue<COMMON_QUE>	gque_Deli;									// �󂯓n���f�[�^�p�L���[

	//// �O���A�N�Z�X
	bool  IsPulsu() const { return m_bIsPulsu; }						// �p���X�o�͒�?
	// Get
	const PARAM& GetParamInf() const { return m_Param; }				// �p�����[�^
	int   GetExpTime(int id)   const { return m_nNowExpTime[id]; }		// �I������ [25��s]
	int   GetNowScode(int ou)  const { return m_nNowScode[ou]; }		// ���݂̌����\�ʏ��No (�\/��)
	EM_DIV_PLG GetPlgMode()	   const { return m_emPLGmode; }			// PLG�敪
	
	CString GetMainModeName(ENUM_MAINMODE emMainMode);					// �w�胂�[�h���̎擾
	CString GetMainModeName();											// �����[�h���̎擾

	// Set


	// ������Ή��p
	CString GetMyName()			{ return my_sThreadName;}
	CString GetMySession()		{ return m_cMySession;}
	int		GetMyCamIdStart()	{ return m_nMyCamIdStart;}
	int		GetMyCamNum()		{ return m_nMyCamNum;}

	bool	IsCheckMyCamId(int id) {									// ���N���X�Ώۂ̃J�����ł����true
				if( id < m_nMyCamIdStart) return false;
				if( id >=m_nMyCamIdStart + m_nMyCamNum ) return false;
				return true;
	}
	int		GetCamIdxToId(int idx) { return m_nMyCamIdStart + idx;}	// ���N���X�̘A�Ԃ���J����ID��
	int		GetCamIdToIdx(int id)  {									// �J����ID���玩�N���X�̘A�Ԃɕϊ��B�ΏۊO�ł����-1
				if( ! IsCheckMyCamId(id) ) return -1;
				return id - m_nMyCamIdStart;
	}	


private :
	static UINT WINAPI MainThread(LPVOID param);						// ���C���X���b�h
	bool SetDataMailCommand(COMMON_QUE *pQue);							// �����Ǘ�����̃��[���X���b�g�̃R�}���h����
	bool SetExposureTime(int cam, int mode, int div );					// �I�����Ԍ���
	void Send_ToDefect_AgcVal();										// ���I�����Ԃ𓝊��Ǘ��֑��M

	// �V�[�P���X����
	bool CheckNextSend();												// �����M����
	bool CheckDataRecv();												// ��M�`���`�F�b�N

	// 232C�֘A
	bool Open(bool bLogDsp);											// 232C �I�[�v��
	void Close();														// 232C �N���[�Y
	bool Recv();														// 232C �񓯊� ��M
	bool Send();														// 232C �񓯊� ���M

	// DB�擾�֘A
	bool GetParam();													// ���I�p�����[�^�擾
	bool GetParamStatic();												// �ÓI�p�����[�^�擾

	// �R�}���h�֘A
	char GetChannelNo(int nCamID);										// �J����ID����`�����l���ԍ��擾


	//// �e�C���X�^���X
	LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X
	Rs232cBase*				mcls_p232c;									// 232c�ڑ��C���X�^���X


	// �V�O�i��
	HANDLE					m_evRecvEnd;								// ��M�����C�x���g�F���p
	HANDLE					m_evSendEnd;								// ���M�����C�x���g�F���p
	HANDLE					m_evReTry;									// �Đڑ��v�� �C�x���g
	HANDLE					m_evAnserTimeOut;							// �����`���҂��^�C���A�E�g �C�x���g


	// 232C�֘A
	char					m_cRecvBuf[PARAM_232C_RECVBUF_SIZE];		// ��M�o�b�t�@
	char					m_cSendBuf[PARAM_232C_SENDBUF_SIZE];		// ���M�o�b�t�@
	char					m_cSendManyu[PARAM_232C_SENDBUF_SIZE];		// �蓮���M�o�b�t�@

	// �X�e�[�^�X�֘A
	ENUM_MAINMODE			m_emMainMode;								// ���݂̃��C�����[�h
	int						m_nSubMode;									// ���݂̃T�u���[�h
	int						m_nCamMode;									// ���݂̃J�����䐔�J��Ԃ����[�h
	int						m_nSwMode;									// ���݂̃J���������N�X�C�b�`�䐔�J��Ԃ����[�h (�� �J�����R���g���[���̏ꍇ�ł��g�p)
	
	// �J�����R���g���[���[ ���
	bool					m_bIsConnect;								// �J�����R���g���[���[ �ڑ����(true:�ڑ���, false:�ؒf��)
	bool					m_bIsPulsu;									// �p���X�o�͒�
	bool					m_bNowSend;									// ���ݑ��M���H (1�V�[�P���X�������܂ł̊�true)
	int						m_nNowExpTime[NUM_CAMERA];					// ���݂̘I������ [25��s]


	// ���̑�������� (������Ή�)
	int						m_nUnit;									// �@��ԍ�(0�I���W��)
	CString					my_sThreadName;								// �X���b�h��
	CString					m_cMySession;								// �����ʎq (Ini�ݒ荀��)
	CString					m_cPrmCommonTblName;						// �p�����[�^�e�[�u����
	int						m_nMyCamNum;								// ���N���X�S�� �J�����䐔
	int						m_nMyCamIdStart;							// ���N���X�S�� �J����ID�J�n�ʒu (0�I���W��)
	int						m_nMyCamIdEnd;

#ifndef VER_AVALDATA
	int						m_nCamID[NUM_CAMERA];						// �J����ID (�z���~�X�ׂ̈̐U��ւ��p)
#else
	int						m_nCamNum[NUM_CAMLINKSW];							// �ڑ��J�����䐔
	int						m_nCamID[NUM_CAMLINKSW][MAX_CAMLINKSW_ALLINPUT];	// �J����ID
	int						m_nInpCh[NUM_CAMLINKSW][MAX_CAMLINKSW_ALLINPUT];	// ����CH (�z���~�X�ׂ̈̐U��ւ��p)
#endif
	bool					m_bAlame[NUM_CAMERA];						// �A���[���o�͋敪 (true:�o��OK)
	int						m_nNowScode[2];								// ���݂̌����\�ʏ��No (�\/��)
	EM_DIV_PLG				m_emPLGmode;								// PLG�敪 (1:���C�� 2:�\�Z�� 3:���Z�� 4:�e�X�g)
	int						m_nTestTime;								// �e�X�g�p���X���� [��s]
	int						m_nExpTime;									// �I������ [25��s]

	// �p�����[�^
	PARAM					m_Param;									// �p�����[�^
	AGCINF					m_typAgcInf;								// �I�����ԕύX���̏��
	int						m_nFilter;									// �J�����R���g���[���[�t�B���^�[�l [0.1��s]	
	int						m_nH;										// H�R�}���h�l (0�`)

	int						m_nCmdIniInputNum;							// ini�t�@�C������w�肵�����M�R�}���h��
	std::vector<CString>	m_lstCmdIniInput;							// ini�t�@�C������w�肵�����M�R�}���h

	// ���L������
#ifdef IS_TO_CAMERA_TBL
	HANDLE					m_hCam;										// �J���R����� �n���h��
	TO_CAMERA_TBL*			mtbl_pCam;									// �J���R����� �f�[�^
#endif
};
