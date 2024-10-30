// *********************************************************************************
//	���C���Ǘ��X���b�h
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//		
// *********************************************************************************

#pragma once

// �W�����C�u����
#include <vector>

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[

// �r�����C�u���� �C���N���[�h
#include "..\..\Library\DefectLib\Tracking.h"							// �g���b�L���O�Ǘ��N���X
#include "..\..\Library\DefectLib\KizuOpSend.h"							// �������\���E����PC�ւ̑��M�N���X
#include "..\..\Library\KizuLibMFC\Class\DivNameManager.h"				// �r�����C�u�����w�b�_�[

// �e���[�J�[�X���b�h
#include "MainConst.h"
#include "ParamManager.h"												// �p�����[�^�Ǘ��N���X
#include "StatusManager.h"												// �X�e�[�^�X����N���X
#include "DBManager.h"													// DB�o�^����N���X
#include "CoilManager.h"												// �R�C�����Ǘ��N���X
#include "PlgManager.h"													// PLG�Ǘ��N���X
#include "HtRecv.h"														// ����˓����ԒʐM�Ǘ��N���X
#include "SogyoManager.h"												// ���Ə�ԊǗ��N���X
#include "SeqFrameRecv.h"												// �V�[�P���T�ʐM��M�N���X
#include "AlarmeManager.h"												// �x��o�͊Ǘ��N���X
#include "DmemManager.h"												// �f�[�^�������ʐM�Ǘ��N���X

// �P�@�\�N���X
#include "EdgePosControl.h"												// �G�b�W���o�������Ǘ��N���X
#include "StatusLogFunc.h"												// �X�e�[�^�X���O�N���X
#include "CamLinkSpeedManager.h"										// �J���������N�X�C�b�`�p �V���A���ʐM���x�ύX�N���X
#include "EdgeRestrictSender.h"											// �G�b�W���o�͈͎w�� UDP���M�N���X

// �t�@���N�V�����N���X
#include "OpSendFunc.h"													// �������\�� ���M��񐶐��c�[��
#include "IPortViewerManager.h"											// iPortViewer�R�}���h��t�N���X
#include "CmdSender.h"													// �����p���R�[�_�[ UDP�ʐM�N���X
#include "ToLinkswUtil.h"												// �J���������N�X�C�b�`�ݒ胆�[�e�B���e�B
#include "LogicParaRunFunc.h"											// ���W�b�N�p�������֘A�������W���[��

using namespace KizuLib;


class MainInstance : public ThreadManager
{
public:
	MainInstance(void);
	virtual ~MainInstance(void);

	void Start() { SetEvent(m_evThStart); }								// �X���b�h�J�n (MainInstance�̎������̓I�[�o�[���C�h)
	BOOL Init();														// ����������
	void Exit();														// �I������
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ���O�t�@�C���Z�b�g
	
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast() { return 0; }										// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
	void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����


////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �{�N���X�󂯓n���p�N���X
//// ���[�����󂯓n���L���[
private :
	struct DELIVERY_MAIL {
		ENUM_MAININSTANCE	emNo;										// �C�x���gNo
		COMMON_QUE			que;										// �t�����
	};
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �^�C�}�[�p�萔
private:
#ifndef LOCAL
	static const int PARAM_TIMEOUT_RESTART				= 30000;//15000//3000;	// ���������ċN�� �҂��^�C�}�[
	static const int PARAM_TIMEOUT_DETECT_INIT_START	= 120000;		// �J���������N�X�C�b�`�ݒ�A���T�[�҂��^�C�}�[
	static const int PARAM_TIMEOUT_WAIT_START			= 60000;//120000;		// �����J�n ����A���T�[�҂��^�C�}�[
	static const int PARAM_TIMEOUT_WAIT_STOP			= 7000;			// ������~ ����A���T�[�҂��^�C�}�[
#else
	static const int PARAM_TIMEOUT_RESTART				= 15000;		// ���������ċN�� �҂��^�C�}�[
	static const int PARAM_TIMEOUT_DETECT_INIT_START	= 120000;		// �J���������N�X�C�b�`�ݒ�A���T�[�҂��^�C�}�[
	static const int PARAM_TIMEOUT_WAIT_START			= 60000;		// �����J�n ����A���T�[�҂��^�C�}�[ (HT_DEFECT��"PROCESS_INIT_WAIT_TIME"+"PROCESS_EXIT_WAIT_TIME"��蒷���ݒ肷�邱��)
	static const int PARAM_TIMEOUT_WAIT_STOP			= 7000;			// ������~ ����A���T�[�҂��^�C�}�[
#endif
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �O���A�N�Z�X
public:
	void SetDeliveryMail(ENUM_MAININSTANCE emNo, COMMON_QUE * que);		// �X���b�h�L���[�Ƀ��[���f�[�^�o�^
	void SetLogMgrSeq(LogFileManager* pCls) { mcls_pLogSeq = pCls; }	// �V�[�P���T����M�p���O�N���X
	void SetLogMgrE(LogFileManager* pCls) {  mcls_pLogE = pCls; }		// �G�b�WUDP�p���O�N���X
	void SetLogMgrG(LogFileManager* pCls) { mcls_pLogG = pCls; }		// �K�C�_���X�p���O�N���X
	void SetLogMgrD(LogFileManager* pCls) { mcls_pLogD = pCls; }		// �����p���O�N���X
	void SetLogMgrA(LogFileManager* pCls) { mcls_pLogA = pCls; }		// �x��p���O�N���X
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �V�[�P���X����
public:
	void AllDetectStop(bool bMode, bool bRetry);						// �S��~
	void DetectReStart();												// ���������ċN�� �v�� (���������ċN������)
	void DetectInitStart(COMMON_QUE * que);								// ��������������			(�J���������N�X�C�b�`�ݒ芮���A���T�[�҂���)
	void DetectWaitStart();												// �����J�n�҂��J�n���� (���茟���J�n�A���T�[�҂���)
	void DetectRun();													// �������J�n���� (��������)
	void DetectStop(bool bMode);										// ������~ ���� (���茟����~�A���T�[�҂���)
	void WaitInit_TimeOut();											// �����������҂�����		(�^�C���A�E�g����)
	void WaitInit_Answer(COMMON_QUE* que);								// �����������҂�����		(�A���T�[��M)
	void WaitStart_Answer(COMMON_QUE* que);								// �����J�n�҂� ���� (�A���T�[��M)
	void WaitStop_Answer(COMMON_QUE* que);								// ������~�҂� ���� (�A���T�[��M)
	void WaitStart_TimeOut();											// �����J�n�҂� ���� (�^�C���A�E�g����)
	void DetectRun_Cancel(int ou);										// �w��ʂ݂̂̌������f
	void HT_StatAnswer(COMMON_QUE* que);								// ����₢���킹�A���T�[

	void ChangeCoil_Ken(int ou);										// �e�������u�ʒu�ł̃R�C���؂�ւ�

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// WPD����
private:
	void WPD_KenBase(int bIsWpd, double *pSpmDiff);						// �����pWPD�F��
	void WPD_DspBase();													// �\���pWPD�F��



////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// ���̑�
private:
	//void Cut_PiOn();													// PI �V���[�J�b�g�M��ON
	void SetPosition_CoilChange(DWORD nNowFrame, double dKenDist, double dKenDistNot, double dSpmDiff);	// �R�C���ؑ֎��̓��������J�E���^�ݒ�v��
	void SetPosition_ForceCoilChange(DWORD nNowFrame, double dKenDist, double dKenDistNot);				// �R�C�������ؑ֎��̓��������J�E���^�ݒ�v��
	void SetParam_NextCoilInfRecv();									// ���R�C������M���̃p�����[�^�ݒ�v��
	void Add_Synclock(COMMON_QUE* que);									// DB�o�^�󋵖₢���킹

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �O���^�X�N�ʒm
public:
	// HT_DEFECT
	static void Send_HtDefect_Param(int frame, int id, int scode, EM_DIV_HANTEI hantei = DIV_HANTEI_NON);	// ����PC �� �p�����[�^�ύX��ʒm [��ParamManager, GapInfManger]
	void		Send_HtDefect_Start();																		// ����PC �� �����J�n��ʒm [��MI]
	static void Send_HtDefect_Pcinfo(TYPE_EPC_COIL* pData);													// ����PC �� ��ʏ�񑗐M
	static void Send_HtDefect_JpegQuality(int nJpegQuality);												// ����PC �� JPEG�摜���k���ݒ著�M  [��MI, ParamManager]
	void		Send_HtDefect_PcCanOk();																	// ����PC �� ���s�s�\����	[��Dlg, MI]
	static void Send_HtDefect_ClearParam(int id, EM_DIV_HANTEI hantei);										// ����PC �� �����s�ݒ�̃L�����Z���˗� [��MI]
	static void Send_HtDefect_SetPosition(int id, EM_DIV_HANTEI hantei, NCL_CMD_SET_POSITION* cmd);			// ����PC �� ���������J�E���^�ݒ�v�� [��PlgManager]
	static void Send_HtDefect_PreLoadParam(int id, EM_DIV_HANTEI hantei, int scode);						// ����PC �� �p�����[�^���O�Ǎ� [��CoilManager]
	// HY_DEFECT
	static void	Send_HyDefect_Param(int id);																// �\���Ǘ��� �p�����[�^�ύX��ʒm [��ParamManager]
	void		Send_HyDefect_MemDataInit();																// �\���Ǘ��ɕ\����񏉊����ʒm
	static void Send_HyDefect_MaeRead(char const* cKizukenNo);												// �\���Ǘ��� �O�H���r���Ǎ��݂�ʒm [��MaeManager, CoilBaseManager]
	// TO_GETDAT
	//static void Send_ToGetdat_Param(int id);																// ���̓f�[�^�ҏW�Ǘ��� �p�����[�^�ύX��ʒm [��ParamManager]
	// TO_PUTDAT
	//static void Send_ToPutdat_Param(int id);																// ���ѕҏW�Ǘ��� �p�����[�^�ύX��ʒm [��ParamManager]
	//static void Send_ToPutDat_Synclock(int okng);															// ���ѕҏW�^�X�N�� �o�^�����ʒm [��DBManager]
	// TO_MAEDAT
	//static void Send_ToMaedat_Param(int id);																// �O�H�����ѕҏW�Ǘ��� �p�����[�^�ύX��ʒm [��ParamManager]
	static void Send_ToMaedat_CoilRecv(char* KizukenNo, int rec);											// �O�H�����ѕҏW�� �����p�R�C������M���� ��ʒm [��CoilManager]
	// SO_GAMENN
	void		Send_SoGamenn_MailStatus();																	// �I�y���[�^��ʂɌ��݂̏�Ԓʒm [��Dlg]
	// KS_SLAVE
	static void	Send_KsMaster_RebootPc(char* cPcName);														// PC�}�X�^�[�Ǘ��ɍċN���v���ʒm [��StatusManager]
	// SO_DEFMAP
	static void Send_SoDefMap_Param(int id);																// �r�_�}�b�v��ʂɃp�����[�^�ύX��ʒm [��ParamManager]

private:
	// HT_DEFECT
	void		Send_HtDefect_Status();																		// ����PC �� ��Ԗ⍇��
	void		Send_HtDefect_Stop();																		// �S����PC �� ������~��ʒm
	void		Send_HtDefect_Stop(int men);																// �Y���ʂ̔���PC�A����PC �� ������~��ʒm
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// ���J�ϐ�
public:
	// �e���[�J�[�X���b�h
	LockManager					gcls_Lock;									// �X���b�h�ԓ�������N���X
	ParamManager*				gcls_pParam;								// �p�����[�^�Ǘ��N���X
	StatusManager				gcls_Status;								// �X�e�[�^�X����N���X
	DBManager					gcls_DB;									// DB�o�^����N���X
	CoilManager*				gcls_pCoil;									// �R�C�����Ǘ��N���X
	HtRecv*						gcls_pHtRecv[NUM_MEN];						// ����˓����ԒʐM�Ǘ��N���X (0:�\ 1:��)

	PlgManager					gcls_Plg;									// PLG�Ǘ��N���X
	AlarmeManager				gcls_Arm;									// �x��o�͊Ǘ��N���X
	DmemManager					gcls_Dmem;									// �f�[�^�������ʐM�Ǘ��N���X
	SeqFrameRecv*				gcls_pSeqFrameR;							// �V�[�P���T�ʐM��M�N���X
	KizuOpSend*					gcls_pOpS[HY_SOCK_NUM];						// �������\�� ���M�N���X
	OpSendFunc					gcls_OpSend;								// �������\�� ���M��񐶐����W���[��
	SogyoManager*				gcls_pSogyo;								// ���Ə�ԊǗ��N���X
	MaeManager					gcls_Mae;									// �O�H�����Ǘ��N���X

	DefectDumpManager*			gcls_pDefDump[NUM_MEN];						// �r���t�@�C���Ǘ��N���X
	
	CamLinkSpeedManager			gcls_CamLinkSpdMgr;							// �J���������N�X�C�b�`�p �V���A���ʐM���x�ύX�N���X
		
	// �g���b�L���O�֘A
	Tracking*					gcls_pTrKenDistPos;							// ������_�`��ʌ������u�ʒu�܂ł� �g���b�L���O
	TrackingList*				gcls_pTlCoilOutSt;							// ������_�`�V���[�ʒu�܂ł� �g���b�L���O (�o�������R�C���P�ʂ̏�ԊǗ��p�g���b�L���O)

	// ���S���p
	EdgePosControl				gcls_EdgePosCont;							// �G�b�W���o�������Ǘ��N���X
	StatusLogFunc*				gcls_pStatusFunc;							// �X�e�[�^�X���O�N���X
	IPortViewerManager			gcls_IPort;									// IPortViewerManager�R�}���h��t�N���X

	TrackingFrame<TR_FRAME>		gcls_TrFrm;									// FrameNo�Ńg���b�L���O�\�Ƃ���N���X
#ifdef ENABLE_RECORDER_UDP
	CmdSender					gcls_CmdSender;								// �����p���R�[�_�[ UDP�ʐM�N���X
#endif
#ifdef ENABLE_RPI_SAVE
	IfRpiSave*					mcls_pIfRpiSave;
#endif
	EdgeRestrictSender			gcls_EdgeRestSd;							// �G�b�W���o�͈͎w�� UDP�ʐM�N���X

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �����o�[�ϐ�
private:

	// �e���[�J�[�X���b�h
	LogFileManager*				mcls_pLogSeq;								// �V�[�P���T��M�p���O�o��
	LogFileManager*				mcls_pLogE;									// �G�b�WUDP�p���O�o��
	LogFileManager*				mcls_pLogG;									// �K�C�_���X�p���O�o��
	LogFileManager*				mcls_pLogD;									// �����p���O�N���X
	LogFileManager*				mcls_pLogA;									// �x��p���O�N���X


	// ��������
	ThreadQueue<DELIVERY_MAIL>	mque_DeliMail;								// ���[�����󂯓n���L���[
	HANDLE						m_evThStart;								// MainInstance�X���b�h�̎��s�v��

	// �����V�O�i���i�^�C�}�[�j
	HANDLE						m_evTimeReStart;							// ���������ċN���҂��^�C�}�[
	HANDLE						m_evTimeInitStart;							// �J���������N�X�C�b�`�ݒ�A���T�[�҂��^�C�}�[
	HANDLE						m_evTimeWaitStart;							// �����J�n ����A���T�[�҂��^�C�}�[


	// �����V�O�i�� (�g���b�L���O)
	HANDLE								m_evTr_Ken;							// Tr ������_WPD�`��ʌ������u�ʒu�܂ł̃g���b�L���O
	HANDLE								m_evTr_KenGate;						// Tr �Q�[�g�M��ON�`������_�܂ł̃g���b�L���O
	HANDLE								m_evTr_DspGate;						// Tr �Q�[�g�M��ON�`�\����_�܂ł̃g���b�L���O

	NCL_CMD_SET_POSITION		m_strCmd[NUM_MEN];							// �O��WPD�M����M���̓��������J�E���^�\����
	
	bool						m_bIsWpdGateTrKen;							// �����pWPD�Q�[�g�M��ON�`�����pWPD�M��ON�̊�true�ɂȂ�t���O
	bool						m_bIsWpdGateTrDsp;							// �\���pWPD�Q�[�g�M��ON�`�\���pWPD�M��ON�̊�true�ɂȂ�t���O

	// �S���摜�ۑ��֘A
#ifdef ENABLE_RPI_SAVE
	
	IfRpiSave							gcls_IfRpiSave;
#endif

	DWORD						m_nFrameNo_V[NUM_MEN][NUM_CAM_POS];			// �_���t���[��No (WPD ON���̃t���[��No��ێ�)

};
