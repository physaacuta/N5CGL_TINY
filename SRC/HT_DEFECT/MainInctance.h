// *********************************************************************************
//	���C���Ǘ��X���b�h
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//		
// *********************************************************************************

#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\DefectLib\KizuToSend.h"							// ����˓����ԒʐM �N���X


// EPC�C���N���[�h
//#include "..\..\Include\EPC\NCcommand.h"								// ����R�}���h�̒�`
//#include "..\..\Include\EPC\NCsmemData.h"								// ���L��������̃f�[�^�\��
#include "..\..\Include\EPC\NCL2DETECT_if.h"							// I/F��`


// �e���[�J�[�X���b�h
#include "MainConst.h"													// ���N���X�萔�w�b�_�[
#include "StatusManager.h"												// �X�e�[�^�X�Ǘ��N���X
#include "CommandSender.h"												// �R�}���h�Ǘ��N���X
#include "DataRecver.h"													// EPC��HT�Ǘ��N���X
#include "DataSender.h"													// HT��TO�Ǘ��N���X
#include "CheckLogic.h"													// ���W�b�N�t�@�C���L���`�F�b�N�N���X
#include "ParamManager.h"												// �p�����[�^�N���X


// �S�������
#include "..\..\Library\DefectLib\SockOneToAnySend.h"					// ��]���i�`���̑��M�N���X
#include "..\..\Include\OutIF_HT_RAW.h"									// �S����`
#include "CullManager.h"

using namespace KizuLib;


class MainInctance : public ThreadManager
{
//// �����o�[�萔
private :
	//=============================
	// �{�N���X�󂯓n���p�N���X
	//=============================
	//// ���[�����󂯓n���L���[
	struct DELIVERY_MAIL {
		ENUM_MAININCTANCE	emNo;										// �C�x���gNo
		COMMON_QUE			que;										// �t�����
	};

	//=============================
	// APP �ُ퍀�� �p�����[�^
	//=============================
	#define	APP_PARAM_EXEC				0x01		// EXE���s
	#define	APP_PARAM_PARAM				0x02		// �p�����[�^�ύX
	#define	APP_PARAM_COILINF			0x04		// ��ʐݒ���

	#define	APP_PARAM_SET_POSITION		0x10		// ���������J�E���^
	#define	APP_PARAM_PRELOAD_PARAM		0x20		// �p�����[�^���O�Ǎ�
	#define	APP_PARAM_CLEAR_QUEUE		0x40		// �����s�ݒ�̃L�����Z��


	#define DEFAULT_GCP_CHECK_INTERVAL	1*60*60		// �J����GCP�`�F�b�N�Ԋu[sec] �� INI�t�@�C���ɒ�`�Ȃ��ꍇ�̃f�t�H���g�l
	#define GCP_CHECK_KEY_MAX			20			// �J����GCP�`�F�b�N���ڂ̍ő�

//// ���J�֐�
public:
	MainInctance(void);
	virtual ~MainInctance(void);

	void Start() { SetEvent(m_evThStart); }								// �X���b�h�J�n (MainInctance�̎������̓I�[�o�[���C�h)

	BOOL Init();														// ����������
	void Exit();														// �I������


	// �O���A�N�Z�X
	void SetDeliveryMail(ENUM_MAININCTANCE emNo, COMMON_QUE * que);		// �X���b�h�L���[�Ƀ��[���f�[�^�o�^
	void SetLogMgrCull(LogFileManager* pCls) { mcls_pLogCull = pCls; }	// �S�������p���O�N���X

	void ResumeResponse(int nStartFrameNo, EM_DIV_HANTEI emKind, int nAns);		// �r���Q���A���T�[���M����


	//// ��ʗp
	int			 	GetCamPair()		const { return m_nCamPair;}			// ���݂̃J�����y�A 1�` (0:�o�b�N�A�b�v=���蓖�Ė���)
	int				GetMen()			const { return m_nMen; }
	int				GetNowScode(int id)	const { return m_nNowScode[id];}	// ���݂̌����\�ʏ��No
	EM_DIV_HANTEI	GetKindHt()			const { return m_emKindHt; }		// �o�b�N�A�b�v:0 �P������:1 ��������:2 �p����������:3



//// ���J�ϐ�
public:

	// �e���[�J�[�X���b�h
	ParamManager			gcls_PrmMgr;								// �p�����[�^�N���X
	StatusManager			gcls_Status;								// �X�e�[�^�X����N���X
	CommandSender*			gcls_pCom;									// �R�}���h���M�X���b�h
	DataRecver*				gcls_pData;									// �f�[�^��M�X���b�h
	DataSender*				gcls_pSend;									// �f�[�^���M�X���b�h
	KizuToSend*				gcls_pSock[HT_CONNECT_NUM];					// ����˓����ԒʐM �N���X
	CheckLogic				gcls_Logic;									// ���W�b�N�t�@�C���L���`�F�b�N�N���X



//// �����o�[�֐�
private:		
	// �X���b�h�����`
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	

	// �v���Z�X����
	bool ExecuteProcess_DetectProc();									// EPC�摜�����\�t�g�N��
	bool ExecuteProcess_iPortViewer();									// �J���������\�t�g �N��

	bool DisposeProcess_DetectProc();									// EPC�摜�����\�t�g�I��
	bool DisposeProcess_iPortViewer();									// �J���������\�t�g �I��


	// �O�`�Z�o�v���Z�X
	bool ExecuteProcess_RectProc();										// �O�`�Z�o�v���Z�X �N��
	bool DisposeProcess_RectProc();										// �O�`�Z�o�v���Z�X �I��


	// �V�[�P���X����
	void DetectStart(COMMON_QUE* que);									// �����J�n����
	void DetectStop(int mode, int ansRetc);								// ������~����
	void HelcyStat();													// ��Ԏ擾
	void ResumeStart(COMMON_QUE* que);									// �r���Q������

	bool DoSeqenc(int nMen, int nKind);									// ���s�\��Ԃ��`�F�b�N




	// ��������
	int CheckCommandPipe_Err(CommandSender::EM_PIPE_ERR emErrCode, int nDetail, BYTE const* pData); // �R�}���h�p�C�v�N���X���G���[
	void SendState_App(DWORD state);									// APP�ُ퍀�ڃp�����[�^ �؂�ւ�
	void SendState_App(DWORD addr, bool onoff);							// APP�ُ퍀�ڃp�����[�^ �؂�ւ�
	void SendState_PcCanOk(bool reboot);								// ���s�s�\����
	void SendState_PcCanNot();											// ������PC �� ���s�s�\
	void SendState_Grab(int nCamId, bool state);						// �摜���ُ͈�
	void Send_ToDefect_State();											// ��Ԃ�ʒm
	//void Send_ToDefect_FrameReqFail(DWORD camset, DWORD FrameNo);		// �t���[���摜�擾���s��ʒm
	void Send_CoilInfDiv(const COMMON_QUE* pQue);						// ��ʏ��(������)�擾&���M���C������

	// �R�}���h����
	int SendCommand_Init();												// �摜�����^�X�N �������R�}���h
	int SendCommand_Dispose();											// �摜�����^�X�N �����J���R�}���h
	int SendCommand_Exit();												// �摜�����^�X�N �I���R�}���h
	int SendCommand_Start();											// �摜�����^�X�N �����J�n�R�}���h
	int SendCommand_Stop();												// �摜�����^�X�N ������~�R�}���h
	int SendCommand_Param(DWORD framenum, DWORD scode);					// �摜�����^�X�N �p���[���[�^�ύX�R�}���h
	int SendCommand_CoilInf(DWORD size, BYTE const* pData);				// �摜�����^�X�N ��ʏ��ݒ�R�}���h    ���������M��
	int SendCommand_EdgeSearch(int ll, int lr, int rl, int rr);			// �摜�����^�X�N �G�b�W���o�T�[�`�͈͕ύX�R�}���h

	int SendCommand_PreLoad_Param(DWORD scode);							// �摜�����^�X�N DB���烁�����ւ̃p�����[�^�ǂݍ��ݐݒ�R�}���h
	int SendCommand_SetPosition(NCL_CMD_SET_POSITION* pVal);			// �摜�����^�X�N ���������J�E���^�ݒ�R�}���h
	int SendCommand_ClearQueue();										// �摜�����^�X�N �����s�ݒ�̃L�����Z���R�}���h
	int SendCommand_Query_State(NCL_ANSWER_QUERY_STATE* pVal);			// �摜�����^�X�N ��Ԗ₢���킹

	int SendCommand_Resume(int nStartFrameNo);							// �摜�����^�X�N �����r���J�n
	void ResumeStop(int mode, int ansRetc);								// �摜�����^�X�N �����r����~

	void CommandAnswerProc(int retc, DWORD addr);						// �摜�����^�X�N �A���T�[������

	// �J����GCP�`�F�b�N�֘A
	void GcpCheck_StartTimer();											// �J����GCP���ʂ̒�����`�F�b�N���J�n
	void GcpCheck();													// �J����GCP���ʂ̒�����`�F�b�N
	bool FindLine(const char* src, const char* target, char* retStr, const char* delimiter = "\r\n");	// GCP���ʂ���w��̍s�𒊏o


//// �����o�[�ϐ�
private:
	// �e���[�J�[�X���b�h
	LogFileManager*				mcls_pLogCull;							// �S�������p���O�N���X

	// ��������
	ThreadQueue<DELIVERY_MAIL>	mque_DeliMail;							// ���[�����󂯓n���L���[
	HANDLE						m_evThStart;							// MainInctance�X���b�h�̎��s�v��
	HANDLE						m_evHelcy;								// �w���V�[�擾�^�C�}�[
	HANDLE						m_evGcpCheck;							// �J����GCP�`�F�b�N�^�C�}�[


	// �p�����[�^
	int			 				m_nCamPair;								// ���݂̃J�����y�A 1�` (0:�o�b�N�A�b�v=���蓖�Ė���)
	int							m_nMen;									// ���݂̖� (-1:�s��  0:�\  1:��)
	int							m_nNowScode[NUM_MEN];					// ���݂̌����\�ʏ��No(0:�Ώ�Scode)
	EM_DIV_HANTEI				m_emKindHt;								// ����@�\
	DWORD						m_nAppParam;							// APP�ُ퍀�ڃp�����[�^
	bool						m_bNgCam;								// �ُ�J�����S�� true


	// �摜�����G���W��
	PROCESS_INFORMATION			m_pInfo;								// ���s���̃v���Z�X�X�e�[�^�X
	bool						m_bDetectProc;							// �T���v���Z�X���N������Ă��邩(true:�N����)
	bool						m_bDetectProc_pd;						// �T���v���Z�X(����)���N������Ă��邩(true:�N����)
	bool						m_bDetectProc_pr;						// �T���v���Z�X(�p������)���N������Ă��邩(true:�N����)
	bool						m_bAdjustProc;							// �J���������v���Z�X���N������Ă��邩(true:�N����)


	// �O�`�Z�o�^�X�N
	PROCESS_INFORMATION			m_pInfoRect;							// ���s���̃v���Z�X�X�e�[�^�X
	bool						m_bRectProc;							// �O�`�Z�o�v���Z�X�����s����Ă��邩(true:�N����)
	//DWORD						m_nReSendMailSeqNo;						// �đ���M�V�[�P���XNo�i�[�o�b�t�@�����[���X���b�g�����b��Ή�


	// �R�C����񕪊���M�Ή�
	DIVCOIL_MERGED_PCINFO		m_typMergedInfo;						// ��������M�p�\����


//// �S���摜�p
public:
	SockOneToAnySend*				gcls_pSockRpi[FRAME_HT_SEND_PORT];	// ����ˑS�������ԒʐM �N���X
	ThreadLoopQueue<HT_RPI_FDATA>*	gque_pSockRpi[FRAME_HT_SEND_PORT];	// �t���[���摜�󂯓n���L���[(��]���i�`��)	[LIB��AP��] �� �L���[�T�C�Y��AP����w�肵�������߁A�|�C���^�ɂ��Ă���
	CullManager*					gcls_pCull[FRAME_HT_SEND_PORT];		// �Ԉ����X���b�h

private:
	void QueueAlloc(int idx, int n, int const* p);						// �S���������M�p�̉�]���i�L���[�m��
	void QueueFree();													// �S���������M�p�̉�]���i�L���[���

	// GCP���ʃ`�F�b�N�̃e�X�g�p
	const char test_gcp_result[2048] =
		"Model          P4_CC_04K07T_00_R\r\n"
		"Microcode      03 - 081 - 20282 - 18\r\n"
		"CCI            03 - 110 - 20277 - 05\r\n"
		"FPGA           03 - 056 - 20444 - 05\r\n"
		"Serial #       12069009\r\n"
		"User ID\r\n"
		"BiST :			Good\r\n"
		"\r\n"
		"DefaultSet     1\r\n"
		"Ext Trig       On\r\n"
		"Meas L.R.      7[Hz]\r\n"
		"Max  L.R.      31250[Hz]\r\n"
		"Exp.Mode      TriggerWidth\r\n"
		"Meas E.T.      14500[ns]\r\n"
		"Max  E.T.      3000000[ns]\r\n"
		"\r\n"
		"Test Pat.      0:Off\r\n"
		"Direction      Internal, Forward\r\n"
		"Line Delay     3.00\r\n"
		"Hor Alig Mode  Off\r\n"
		"Hor.Bin		1\r\n"
		"Flat Field     Off\r\n"
		"Color Plane Selector : All\r\n"
		"Offset         0\r\n"
		"Gain           6.00\r\n"
		"Red Gain       4.51\r\n"
		"Green Gain     5.00\r\n"
		"Blue Gain      1.00\r\n"
		"Color Matrix Selector : Factory 2\r\n"
		"Color Correction Matrix : 1.000000, 0.000000, 0.000000, 0\r\n"
		"						   0.000000, 1.000000, 0.000000, 0\r\n"
		"						   0.000000, 0.000000, 1.000000, 0\r\n"
		"Mirror         Off\r\n"
		"AOI Mode :		Off\r\n"
		"CL Speed       85MHz\r\n"
		"CL Config      Full\r\n"
		"Pixel Fmt      RGB8\r\n"
		"CPA ROI        1 - 4096\r\n"
		"USER>	\r\n";
};
