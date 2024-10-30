// *********************************************************************************
//	���W�b�N�p�������֘A�̃��W���[���Q
//	[����]
//		�e�C���X�^���X�́Astatic�� .cpp�Ɏ�����
// *********************************************************************************
#pragma once

// �W�����C�u����
#include <vector>

// ���Œ�
#include "..\..\Include\LineCommon.h"							// �C���N���[�h

// ���[�J��
#include "MainConst.h"
#include "ToLinkswUtil.h"
#include "StatusManager.h"
#include "PlgManager.h"	

using namespace KizuLib;

class LogicParaRunFunc
{
public:
	LogicParaRunFunc(LogFileManager* pCls1, StatusManager* pCls2, PlgManager* pCls3);
	~LogicParaRunFunc();

public:
	// �����N�X�C�b�`�R�}���h�ʐM���
	struct LINKSW_CMD {
		int					prNo;								// �p�������@�\No�i1�F���W�b�N�p�������@�\1�A2�F���W�b�N�p�������@�\2�j
		int					camNo;								// ���̓J����No  �i1�F�J�����P�A2�F�J�����Q�j
		int					topCamNo;							// �擪�J����No
		COMMON_QUE			sendCmd;							// TO_LINKSW���M �R�}���h
		bool				bRecv;								// ��M�t���O
	};

	// �����N�X�C�b�`�R�}���h�ʐM���
	enum ENUM_LINKSW_CMD {
		E_LINKSW_CMD_NOP = -1,									// ������ԁi�J�n���������j
		E_LINKSW_CMD_START,										// �����J�n
		E_LINKSW_CMD_RESUME_START,								// �r���Q��
	};

	// �����N�X�C�b�`�R�}���h�ʐM���
	static CString GetLinkswCmdState(ENUM_LINKSW_CMD em)
	{
		CString wk;
		if (E_LINKSW_CMD_NOP == em)					{ wk = "�������"; }
		else if (E_LINKSW_CMD_START == em)			{ wk = "�����J�n��"; }
		else if (E_LINKSW_CMD_RESUME_START == em)	{ wk = "�r���Q����"; }
		else                                        { wk.Format("�X�e�[�^�X�ُ� <%d>", em); }
		return wk;
	}

	//// �^�C�}�[�p�萔
#ifndef LOCAL
	static const int PARAM_TIMEOUT_CAMSET		= 120000;		// �J�����I��ݒ�A���T�[�҂��^�C�}�[
	static const int PARAM_TIMEOUT_RESUME_START = 60000;		// �r���Q�� �J���������N�A���T�[�҂��^�C�}�[
	static const int PARAM_TIMEOUT_RESUME_WAIT	= 60000;		// �r���J�n ����A���T�[�҂��^�C�}�[
#else
	static const int PARAM_TIMEOUT_CAMSET		= 120000;		// �J�����I��ݒ�A���T�[�҂��^�C�}�[
	static const int PARAM_TIMEOUT_RESUME_START = 60000;		// �r���Q�� �J���������N�A���T�[�҂��^�C�}�[
	static const int PARAM_TIMEOUT_RESUME_WAIT	= 60000;		// �r���J�n ����A���T�[�҂��^�C�}�[
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////
//// ���J�֐�(���ׂĐÓI�ɂ���)
public:
	static HANDLE GetEvTimeCamset();												// �J�����I��ݒ�A���T�[�҂��^�C�}�[�C�x���g��Ԃ�
	static HANDLE GetEvTimeResumeStart();											// �r���Q���ݒ�A���T�[�҂��C�x���g��Ԃ�
	static HANDLE GetEvTimeResumeWait(int nKind);									// �r���J�n����A���T�[�҂��C�x���g��Ԃ�

	// �J�����Z�b�g�֘A
	static int GetHtParaCamSize();													// �p���������̓J����No�擾
	static int GetHtParaCamNo(int pcno, int camno);									// �p���������̓J����No�擾
	static int* GetHtParaCamNo();													// �p���������̓J����No�擾
	static void SetHtParaCamNo(int const* pLogiParaCam);							// �p���������̓J����No�ݒ�
	static void SetHtParaCamNo(int pcno, int camno, int camset);					// �p���������̓J����No�ݒ�

	// �v���A�����C�x���g�֘A
	static void ResumeStart(COMMON_QUE* que);										// �r���J�n�v��
	static void ResumeStop(COMMON_QUE* que);										// �r����~�v��
	static void Send_HtResume_Start(int nKind, CoilManager const *pCoilManager, int nJoinFrameNo = 0);	// ����PC �� �r���J�n��ʒm
	static void	Send_HtResume_Stop(int nKind);										// ����PC �� �r����~��ʒm
	static void ResumeInitStart(COMMON_QUE * que);									// �r���J�n����������(�r���Q���ݒ芮���A���T�[�҂���)
	static void WaitResumeStart_Answer(COMMON_QUE* que);							// �r���J�n�A���T�[
	static void WaitResumeStop_Answer(COMMON_QUE* que);								// �r����~�A���T�[
	static void WaitResumeStart_Timeout();											// �r���J�n�҂� ���� (�^�C���A�E�g����)
	static void WaitResumeWait_Timeout(int nKind);									// �r���J�n ����A���T�[�҂� (�^�C���A�E�g����)
	static void WaitCamset_Answer(COMMON_QUE* que);									// �J�����I��ݒ芮���ʒm
	static void WaitCamset_TimeOut();												// �J�����I��ݒ�҂� ���� (�^�C���A�E�g����)
	static void WaitLinkswJoin_Answer(COMMON_QUE* que, CoilManager const *pCoilManager);// �r���Q���ݒ芮���ʒm
	static void DebugSend_CamLinksw_CamsetCmpl(int mode);							// �f�o�b�O�p �J�����I��ݒ芮���ʒm���M

	// �J���������N�X�C�b�`����֘A
	static void RegistLinkswCmd();													// �����J�n���̃����N�X�C�b�`�R�}���h�o�^
	static void SetLinkswCmd(LINKSW_CMD cmd);										// �R�}���h���X�g�Ƀ����N�X�C�b�`�R�}���h�o�^
	static void ClearLinkswCmd(ENUM_LINKSW_CMD state = E_LINKSW_CMD_NOP);			// �R�}���h���X�g�N���A
	static bool IsTargetCamNo(long nFPGABoardNo, long nCamno);						// �p�������J�����̗L�����`�F�b�N
	static void Send_NextCmdSelect();												// ���̃R�}���h���M�i�L���[����j
	static bool IsEqualCamsetCMD(COMMON_QUE* que);									// ���M���̃J�����ݒ�R�}���h�̉������`�F�b�N����
	static bool IsRecvCamsetCMD(int nPrNo);											// �w��p������PC�̃J�����ݒ芮����M�ς݂��`�F�b�N����

	static void LogicParaRunFunc::ClearSelectCamset();								// ����PC �J�����y�AID������
	static void LogicParaRunFunc::SetSelectCamset(int pcid, int camno);				// ����PC �J�����y�AID�ݒ�

private:
	static void	Send_SoGamenn_MailStatus();											// PC�}�X�^�[�Ǘ��ɍċN���v���ʒm

///////////////////////////////////////////////////////////////////////////////////////////////////////
//// �����o�[ �ϐ�
private:
	//=========================================
	// ���W�b�N�p�������p��`
	static int						m_nLogiParaCam[NUM_ALL_HTPARA][NUM_CAMLINK_BOARD];// �J�����Z�b�g
	static ENUM_LINKSW_CMD			m_LinkswCmdKind;								// TO_LINKSW���M �����敪
	static std::vector<LINKSW_CMD>	mlst_SendCmd;									// TO_LINKSW���M�R�}���h���X�g
	static int						m_nSendCmdIndex;								// TO_LINKSW���M�R�}���h���M�ʒu
	static int						m_nSelectCamset[NUM_ALL_HANTEI];				// �J�����y�AID�iPCID���j
																			
	//=========================================
	// ���O�p
	static LogFileManager*			mcls_pLog;										// LogFileManager�X���b�h�C���X�^���X
	static CString					my_sThreadName;									// ���X���b�h��

	//=========================================
	// �O���Q��
	static StatusManager*			mcls_pStatus;									// �X�e�[�^�X������N���X
	static PlgManager*				mcls_pPlg;										// PLG�Ǘ��N���X
	static CoilManager*				mcls_pCoil;										// �R�C�����Ǘ��N���X
	static HANDLE					m_evTimeCamset;									// �J�����I��ݒ�A���T�[�҂��^�C�}�[
	static HANDLE					m_evTimeResumeStart;							// �r���Q���ݒ�A���T�[�҂��^�C�}�[
	static HANDLE					m_evTimeResumeWait[NUM_ALL_HTPARA];				// �r���J�n����A���T�[�҂��^�C�}�[
};

