#include "stdafx.h"
#include "LogicParaRunFunc.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �g�p����N���X�̒�`

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �ÓI�ϐ��̏�����

// ���O�p
LogFileManager*					LogicParaRunFunc::mcls_pLog = NULL;
CString							LogicParaRunFunc::my_sThreadName;

// �O���Q��
StatusManager*					LogicParaRunFunc::mcls_pStatus = NULL;
PlgManager*						LogicParaRunFunc::mcls_pPlg = NULL;
CoilManager*					LogicParaRunFunc::mcls_pCoil = NULL;
HANDLE							LogicParaRunFunc::m_evTimeCamset = NULL;
HANDLE							LogicParaRunFunc::m_evTimeResumeStart = NULL;
HANDLE							LogicParaRunFunc::m_evTimeResumeWait[NUM_ALL_HTPARA] = { NULL, NULL };

// ���W�b�N�p�����������p
int											LogicParaRunFunc::m_nLogiParaCam[NUM_ALL_HTPARA][NUM_CAMLINK_BOARD];// �J�����Z�b�g�i�[�̈�
LogicParaRunFunc::ENUM_LINKSW_CMD			LogicParaRunFunc::m_LinkswCmdKind;									// TO_LINKSW���M �����敪
std::vector<LogicParaRunFunc::LINKSW_CMD>	LogicParaRunFunc::mlst_SendCmd;										// TO_LINKSW���M�R�}���h���X�g
int											LogicParaRunFunc::m_nSendCmdIndex;									// TO_LINKSW���M�R�}���h���M�ʒu
int											LogicParaRunFunc::m_nSelectCamset[NUM_ALL_HANTEI];					// �J�����y�AID�iPCID���j


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��{

//==========================================
// �R���X�g���N�^
//==========================================
LogicParaRunFunc::LogicParaRunFunc(LogFileManager* pCls1, StatusManager* pCls2, PlgManager* pCls3)
{
	my_sThreadName = "LogicParaFnc";

	// ���W�b�N�p�������p�J�����Z�b�g����������
	memset(m_nLogiParaCam, 0x00, sizeof(m_nLogiParaCam));

	// �J�����y�AID��������
	memset(&m_nSelectCamset, 0x00, sizeof(m_nSelectCamset));

	// �O���Q�ƃC���X�^���X�Z�b�g
	mcls_pLog	= pCls1;
	mcls_pStatus= pCls2;
	mcls_pPlg	= pCls3;

	// �����V�O�i�� (�^�C�}�[)
	// ��MainInstance�̃f�X�g���N�^�Ŗ����I��CloseHandle���邱��
	m_evTimeCamset		= CreateWaitableTimer(NULL, FALSE, NULL);		// �J�����I��ݒ�A���T�[�҂��^�C�}�[
	m_evTimeResumeStart = CreateWaitableTimer(NULL, FALSE, NULL);		// �r���Q���ݒ�A���T�[�҂��^�C�}�[
	for (int ii = 0; ii < NUM_ALL_HTPARA; ii++) {
		m_evTimeResumeWait[ii] = CreateWaitableTimer(NULL, FALSE, NULL);	// �r���J�n�^�C�}�[����A���T�[�҂��^�C�}�[
	}
}

//==========================================
// �f�X�g���N�^
//==========================================
LogicParaRunFunc::~LogicParaRunFunc()
{
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �����֐�

//==========================================
// �����ʂɉғ���Ԓʒm
// ��MainInstance�̊֐����ڐA
//   MainInstance�̎Q�Ƃ����������Ȃ������̂�
//==========================================
void LogicParaRunFunc::Send_SoGamenn_MailStatus()
{
	COMMON_QUE que;
	que.nEventNo = FACT_SO_GAMENN_01;
	que.nLineNo = getlineid();

	que.fl.data[0] = mcls_pStatus->GetUnten();
	que.fl.data[1] = mcls_pStatus->GetPlg();
	que.fl.data[2] = mcls_pStatus->GetMenMode() ? 1 : 0;
	que.fl.data[3] = mcls_pStatus->GetKadou();
	que.fl.data[4] = mcls_pStatus->Get_ParaKadou(mcls_pStatus->GetParaSys(0));
	que.fl.data[5] = mcls_pStatus->Get_ParaKadou(mcls_pStatus->GetParaSys(1));

	// ���M
	send_mail(SO_GAMENN, "*", &que);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���J�֐�

//==========================================
// �J�����Z�b�g���T�C�Y��Ԃ�
//==========================================
int LogicParaRunFunc::GetHtParaCamSize()
{
	LOG(em_INF), "[%s] �J�����Z�b�g�̈�T�C�Y:%d", my_sThreadName, sizeof(m_nLogiParaCam));

	return sizeof(m_nLogiParaCam);
}

//==========================================
// �w�肵���J�����Z�b�g��Ԃ�
//------------------------------------------
// int pcnoo : �p������ID
// int camno : �J����No
//==========================================
int LogicParaRunFunc::GetHtParaCamNo(int pcno, int camno)
{
	//LOG(em_INF), "[%s] �J�����Z�b�g�擾 pcno:%d camno%d camset:%d", my_sThreadName, pcno, camno, m_nLogiParaCam[pcno][camno]);

	return m_nLogiParaCam[pcno][camno];
}

//==========================================
// �S�J�����Z�b�g��Ԃ�
//==========================================
int* LogicParaRunFunc::GetHtParaCamNo()
{
	LOG(em_INF), "[%s] �J�����Z�b�g�擾 camset:%d/%d %d/%d", my_sThreadName,
		m_nLogiParaCam[0][0], m_nLogiParaCam[0][1],
		m_nLogiParaCam[1][0], m_nLogiParaCam[1][1]);

	return &(m_nLogiParaCam[0][0]);
}

//==========================================
// �S�J�����Z�b�g��ݒ肷��
//------------------------------------------
// int pcnoo : �p������ID
// int camno : �J����No
//==========================================
void LogicParaRunFunc::SetHtParaCamNo(int const* pLogiParaCam)
{
	memcpy(&m_nLogiParaCam, pLogiParaCam, sizeof(m_nLogiParaCam));

	LOG(em_INF), "[%s] �J�����Z�b�g�ݒ� camset:%d/%d %d/%d", my_sThreadName,
		m_nLogiParaCam[0][0], m_nLogiParaCam[0][1],
		m_nLogiParaCam[1][0], m_nLogiParaCam[1][1]);
}

//==========================================
// �w�肵���J�����Z�b�g��ݒ肷��
//------------------------------------------
// int pcnoo  : �p������ID
// int camno  : �J����No
// int camset : �J�����Z�b�g
//==========================================
void LogicParaRunFunc::SetHtParaCamNo(int pcno, int camno, int camset)
{
	m_nLogiParaCam[pcno][camno] = camset;

	LOG(em_INF), "[%s] �J�����Z�b�g�ݒ� pcno:%d camno%d camset:%d", my_sThreadName, pcno, camno, camset);
}

//==========================================
// �J�����y�AID��������
//==========================================
void LogicParaRunFunc::ClearSelectCamset()
{
	// �J�����y�AID��������
	memset(&m_nSelectCamset, 0x00, sizeof(m_nSelectCamset));
}

//==========================================
// �J�����I��ݒ芮������
//------------------------------------------
// int pcid	: ����PCID
// int camno: ����J�����y�AID
//==========================================
void LogicParaRunFunc::SetSelectCamset(int pcid, int campair)
{
	m_nSelectCamset[pcid] = campair;

	LOG(em_INF), "[%s] ����y�A�J����ID�Z�b�g pcid:%d campair:%d", my_sThreadName, pcid + 1, campair);
}

//==========================================
// �J�����I��ݒ�A���T�[�҂��^�C�}�[�n���h����Ԃ�
//==========================================
HANDLE LogicParaRunFunc::GetEvTimeCamset()
{
	return m_evTimeCamset;
}

//==========================================
// �r���Q���ݒ�A���T�[�҂��^�C�}�[�n���h����Ԃ�
//==========================================
HANDLE LogicParaRunFunc::GetEvTimeResumeStart()
{
	return m_evTimeResumeStart;
}

//==========================================
// �r���J�n����A���T�[�҂��^�C�}�[�n���h����Ԃ�
//------------------------------------------
// int nKind				: �����J�n�敪�i0�F���W�b�N�p�������@�\1�A1�F���W�b�N�p�������@�\2�j
//==========================================
HANDLE LogicParaRunFunc::GetEvTimeResumeWait(int nKind)
{
	return m_evTimeResumeWait[nKind];
}

//==========================================
// �r���J�n�v��
//------------------------------------------
// COMMON_QUE* que	: �N���v��	
//==========================================
void LogicParaRunFunc::ResumeStart(COMMON_QUE* que)
{
	if (DIV_UNTEN_CAMERA == mcls_pStatus->GetUnten())
	{
		// �u�J�����Z���v���[�h��

		LOG(em_ERR), "[%s] �J�����Z�����[�h���͓r���J�n�v������", my_sThreadName);
		// �I�y���[�^�@�\�։ғ���Ԃ𑗐M
		Send_SoGamenn_MailStatus();
		return;
	}

	// �J���������N�X�C�b�`�R�}���h�ʐM��ԃ`�F�b�N
	// �������J�n���̓r���Q���A�r���Q�����̓r���Q����z�肵�ẴK�[�h�����A
	// �@�����J�n�E��~��ʂŃ{�^���}�����Ă���̂Ŕ������Ȃ���
	if (E_LINKSW_CMD_NOP != m_LinkswCmdKind)
	{
		LOG(em_ERR), "[%s] [%s] ���ɓr���J�n�v����M", my_sThreadName, GetLinkswCmdState(m_LinkswCmdKind));
		return;
	}

	COMMON_QUE* pQue = new COMMON_QUE;
	memcpy(pQue, que, sizeof(COMMON_QUE));

	int nKind = pQue->fl.data[7];
	if (0 <= nKind && NUM_ALL_HTPARA > nKind)
	{
		// �����J�n�E��~��ʂŎw�肵���J�����Z�b�g��ۑ�
		SetHtParaCamNo(nKind, 0, pQue->fl.data[8]);
		SetHtParaCamNo(nKind, 1, pQue->fl.data[9]);

		// �r���J�n������������
		ResumeInitStart(pQue);
	}
	else
	{
		LOG(em_INF), "[%s] �r���J�n�v���F�����Ȍ����J�n�敪", my_sThreadName);
	}

	delete pQue;
}

//==========================================
// �r����~�v��
//------------------------------------------
// COMMON_QUE* que	: �N���v��	
//==========================================
void LogicParaRunFunc::ResumeStop(COMMON_QUE* que)
{
	if (DIV_UNTEN_CAMERA == mcls_pStatus->GetUnten())
	{
		// �u�J�����Z���v���[�h��

		LOG(em_ERR), "[%s] �J�����Z�����[�h���͓r����~�v������", my_sThreadName);
		// �I�y���[�^�@�\�։ғ���Ԃ𑗐M
		Send_SoGamenn_MailStatus();
		return;
	}

	//// �������^�C�}�[�j��
	CancelWaitableTimer(m_evTimeResumeWait[que->fl.data[0]]);

	COMMON_QUE* pQue = new COMMON_QUE;
	memcpy(pQue, que, sizeof(COMMON_QUE));

	Send_HtResume_Stop(pQue->fl.data[0]);

	delete pQue;
}

//==========================================
// �r���J�n���������� (�r���Q���ݒ芮���A���T�[�҂���)
//------------------------------------------
// COMMON_QUE *		que		: �N�����
//==========================================
void LogicParaRunFunc::ResumeInitStart(COMMON_QUE * que)
{
	//========================================================
	// ������
	EM_DIV_UNTEN	nUnten	 = (EM_DIV_UNTEN)que->fl.data[0];			// �^�]���
	EM_DIV_PLG		nPlg	 = (EM_DIV_PLG)que->fl.data[1];				// PLG�敪
	bool			bMenMode = (0 == que->fl.data[2] ? false : true);	// �Жʌ�������/�s����(true:����)
	int				scode[2] = { que->fl.data[3], que->fl.data[4] };	// �����\�ʏ��
	int				pcode[2] = { que->fl.data[5], que->fl.data[6] };	// �����p�^�[��
	int				nKind = que->fl.data[7];							// �����J�n�敪�i0�F���W�b�N�p�������@�\1�A1�F���W�b�N�p�������@�\2�j
	int				nPcID = mcls_pStatus->GetHtParaID(nKind);			// ���W�b�N�p������PCID
	LINKSW_CMD		*pCmd = NULL;

	if (0 > nKind || 2 <= nKind)
	{
		LOG(em_ERR), "[%s] �����J�n�敪�ُ�(%d)", my_sThreadName, nKind);
		return;
	}

	// ----------------------
	// �J�����I���R�}���h���O����
	ClearLinkswCmd(E_LINKSW_CMD_RESUME_START);	// �R�}���h���X�g�N���A

	// ----------------------
	// �J�����I��ݒ�R�}���h���M
	// ���w�胍�W�b�N�p������PC���̂�
	CString sWk;
	LINKSW_CMD	cmd;
	for (int ii = 0; ii < NUM_CAMLINK_BOARD; ii++)
	{
		sWk.Format("CAMLINKSW_PCID%d_%d", nPcID, ii + 1);
		int nWk = GetPrivateProfileInt(TO_DEFECT, sWk, 0, TASKINI_NAME);
		int nFPGABoardNo = nWk / 100;
		int nFPGACh = nWk % 100;
		int nCamPort = GetHtParaCamNo(nKind, ii);

		if (0 == nFPGACh) continue;			// �{�[�h������

		// �J�����I��ݒ�R�}���h�o�^�iSlave�w��j
		memset(&cmd, 0x00, sizeof(LINKSW_CMD));
		cmd.prNo	 = nKind + 1;									// ���W�b�N�p������PC no
		cmd.camNo	 = ii + 1;										// ���̓J���� no
		cmd.topCamNo = mcls_pStatus->GetGroupTopCamNo(nFPGABoardNo);// �擪�J����No �O���[�v���̓��̓J�����̐擪
		cmd.bRecv	 = false;										// ��M�t���O�N���A
		ToOutMailSender::Create_ToLinksw_ChangeCam(nFPGABoardNo, nFPGACh, nCamPort, -1, &cmd.sendCmd);
		SetLinkswCmd(cmd);
	}
	// �r���Q���ݒ�R�}���h�L���[�C���O�i�S���W�b�N�p������PC���j
	// �����M���O�ŃZ�b�g���邽�߁A�r���Q���t���[���͂O���Z�b�g
	memset(&cmd, 0x00, sizeof(LINKSW_CMD));
	cmd.prNo = nKind + 1;											// ���W�b�N�p������PC no
	ToOutMailSender::Create_ToLinksw_JoinReq(2 + nKind, 0, &cmd.sendCmd);
	SetLinkswCmd(cmd);

	// ----------------------
	// �@����
	for (int ii = 0; ii < NUM_CAMLINK_BOARD; ii++)
	{
		// �@���ԁ|���W�b�N�p�������@�\ ���̓J�����I�����ʁi����j
		KizuFunction::SetStatus(mcls_pStatus->GetStatusRead().ParaState[nKind][ii].id, true, false);
		// �@���ԁ|���W�b�N�p�������@�\ ���̓J�����i�s���j
		KizuFunction::SetStatus(mcls_pStatus->GetStatusRead().ParaCam[nKind][ii].id, -1, false);
	}

	// ----------------------
	// �p�������@�\�V�X�e����ԕύX
	mcls_pStatus->SetParaSys(nKind, DIV_SYS_INITSTART);
	// ���W�b�N�p��������� �| �Z�b�g
	mcls_pStatus->SetHtParaState(nKind, DIV_BOOL_NON);

	// �J�����I��ݒ�A���T�[�҂��^�C�}�[�j���i�O�̂��߁j
	CancelWaitableTimer(m_evTimeCamset);
	// �r���Q���ݒ�A���T�[�҂��^�C�}�[�j���i�O�̂��߁j
	CancelWaitableTimer(m_evTimeResumeStart);
	// �r���J�n����A���T�[�҂��^�C�}�[�j���i�O�̂��߁j
	CancelWaitableTimer(m_evTimeResumeWait[nKind]);

	// ���̃R�}���h���M�i�L���[����j
	Send_NextCmdSelect();
}

//==========================================
// �r���J�n�A���T�[ �����@(��HT_DEFECT)
//------------------------------------------
// COMMON_QUE* que	: �N�����
//==========================================
void LogicParaRunFunc::WaitResumeStart_Answer(COMMON_QUE* que)
{
	//======================================================
	// ������
	int campair = que->fl.data[0];							// �J�����y�A (1�I���W��)
	int pcid = que->fl.data[1];							// PCID (1�I���W��)
	int ans = que->fl.data[2];							// ���� (0:���� 1:�y�̏� -1:�d�̏�)
	int frameNo = que->fl.data[3];							// �r���Q���t���[��
	EM_DIV_HANTEI emKind = (EM_DIV_HANTEI)que->fl.data[4];	// �S������
	CString wk;

	// [�S������] �p����������
	if (DIV_HANTEI_PR == emKind)
	{
		int nPcIndex = -1;

		for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
		{
			// �����������W�b�N�p������PC�̂ݏ�ԕύX
			if (mcls_pStatus->GetHtParaID(ii) == pcid)
			{
				nPcIndex = ii;
				break;
			}
		}

		if (-1 == nPcIndex)
		{
			// �ُ퉞��
			LOG(em_ERR), "[%s] �r���J�n�A���T�[ �ُ팟�o(PCID�ُ�) pcid=%d", my_sThreadName, pcid);
			// �������f
			return;
		}

		//// �������^�C�}�[�j��
		CancelWaitableTimer(m_evTimeResumeWait[nPcIndex]);

		if (mcls_pStatus->GetParaSys(nPcIndex) != DIV_SYS_WAITSTART) {
			// �ُ퉞��
			LOG(em_ERR), "[%s] [%s]���ɓr���J�n�A���T�[�ʒm��M", my_sThreadName, GetDivSys(mcls_pStatus->GetParaSys(nPcIndex)));
			// �������f
			return;
		}

		// �ُ퉞��
		if (0 != ans)
		{
			LOG(em_ERR), "[%s] �r���J�n�A���T�[ �ُ팟�o(���ʈُ�) pcid=%d", my_sThreadName, pcid);

			// ----------------------
			// �@���ԁ|���W�b�N�p�������@�\xx_�J����xx�I�����ʁi�ُ�j
			for (int ii = 0; ii < NUM_CAMLINK_BOARD; ii++)
			{
				KizuFunction::SetStatus(mcls_pStatus->GetStatusRead().ParaState[nPcIndex][ii].id, false);
			}
		}

		// ----------------------
		// ���W�b�N�p��������� �| �Z�b�g
		// �i���W�b�N�p��������ԗ��\���p�j
		// �����������W�b�N�p������PC�̂ݏ�ԕύX
		mcls_pStatus->SetHtParaState(nPcIndex, (0 == ans ? DIV_BOOL_TRUE : DIV_BOOL_FALSE));

		// ----------------------
		// �p�������@�\�V�X�e����ԕύX
		if (0 == ans) {
			mcls_pStatus->SetParaSys(nPcIndex, DIV_SYS_RUN);
		} else {
			mcls_pStatus->SetParaSys(nPcIndex, DIV_SYS_STOP);
		}

		LOG(em_MSG), "[%s] ���W�b�N�p������%d �ݒ芮���I�I", my_sThreadName, nPcIndex + 1);

		// �I�y���[�^�@�\�֌���Ԃ𑗐M
		Send_SoGamenn_MailStatus();
		OpSendFunc::SendOp_Status();
	}
	// [�S������] �P������
	else
	{
		// �ُ퉞��
		LOG(em_ERR), "[%s] �r���J�n�A���T�[ �ُ팟�o(�P������) pcid=%d", my_sThreadName, pcid);
	}
}

//==========================================
// �r����~�A���T�[ �����@(��HT_DEFECT)
// ���҂����킹�����͎��{���Ȃ�
// �@�ˈُ퉞�����̃��O�o�͂̂�
//------------------------------------------
// COMMON_QUE* que	: �N�����
//==========================================
void LogicParaRunFunc::WaitResumeStop_Answer(COMMON_QUE* que)
{
	//======================================================
	// ������
	int campair	= que->fl.data[0];							// �J�����y�A (1�I���W��)
	int pcid	= que->fl.data[1];							// PCID (1�I���W��)
	int ans		= que->fl.data[2];							// ���� (0:���� 1:�y�̏� -1:�d�̏�)
	int mode	= que->fl.data[3];							// �敪 (0:���� 1:�����Ȃ�)
	EM_DIV_HANTEI emKind = (EM_DIV_HANTEI)que->fl.data[4];	// �S������
	CString wk;

	// [�S������] �p����������
	if (DIV_HANTEI_PR == emKind)
	{
		// �ُ퉞��
		if (0 != ans || 0 != mode)
		{
			LOG(em_ERR), "[%s] �r����~�A���T�[ �ُ팟�o(���W�b�N�p������) pcid=%d", my_sThreadName, pcid);
		}

		int nPcIndex = -1;

		for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
		{
			// �����������W�b�N�p������PC�̂ݏ�ԕύX
			if (mcls_pStatus->GetHtParaID(ii) == pcid)
			{
				nPcIndex = ii;
				break;
			}
		}

		if (-1 == nPcIndex)
		{
			// �ُ퉞��
			LOG(em_ERR), "[%s] �r����~�A���T�[ �ُ팟�o(PCID�ُ�) pcid=%d", my_sThreadName, pcid);
			// �������f
			return;
		}

		// ���W�b�N�p��������� �| �N���A
		mcls_pStatus->SetHtParaState(nPcIndex, DIV_BOOL_FALSE);

		// �p�������@�\�V�X�e����ԕύX
		mcls_pStatus->SetParaSys(nPcIndex, DIV_SYS_STOP);

		// �I�y���[�^�@�\�֌���Ԃ𑗐M
		Send_SoGamenn_MailStatus();
		OpSendFunc::SendOp_Status();

		LOG(em_MSG), "[%s] ���W�b�N�p������%d ��~�I�I", my_sThreadName, nPcIndex + 1);
	}
	// [�S������] �P������
	else
	{
		// �ُ퉞��
		// ����ɐ���ŕԂ���
		if (0 != ans || 0 != mode)
		{
			LOG(em_ERR), "[%s] �r����~�A���T�[ �ُ팟�o(�P������) pcid=%d", my_sThreadName, pcid);
		}
	}
}

//==========================================
// ����PC �� �r���J�n��ʒm
//------------------------------------------
// int nKind				: �����J�n�敪�i0�F���W�b�N�p�������@�\1�A1�F���W�b�N�p�������@�\2�j
// int nJoinFrameNo			: �r���Q���t���[��No
// CoilManager *pCoilManager: 
//==========================================
void LogicParaRunFunc::Send_HtResume_Start(int nKind, CoilManager const *pCoilManager, int nJoinFrameNo)
{
	EM_DIV_UNTEN  emUnten = mcls_pStatus->GetUnten();

	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_HT_DEFECT_22;
	que.nLineNo = getlineid();

	// �����\�ʏ��No(�\�E��)
	que.fl.data[0] = emUnten;
	que.fl.data[1] = pCoilManager->GetInitScode(0);
	que.fl.data[2] = pCoilManager->GetInitScode(1);

	// ��������(�\�E��)
	int nDistLen = mcls_pPlg->GetDistLen();
	int nDistPos = mcls_pPlg->GetDistPos();
	int nDistNotPos = mcls_pPlg->GetDistNotPos();
	que.fl.data[3 + nDistPos] = nDistLen;			// ��ʂ���ɐi��ł���
	que.fl.data[3 + nDistNotPos] = 0;

	// �r���Q���t���[��No
	que.fl.data[5] = nJoinFrameNo;

	//// �o�b�N�A�b�v�ŏ���������
	for (int ii = 0; ii < NUM_ALL_HANTEI; ii++)
	{
		que.fl.data[6 + 2 * ii + 0] = 0;
		que.fl.data[6 + 2 * ii + 1] = 0;
		que.fl.data[6 + 2 * ii + 2] = DIV_HANTEI_BK;
	}

	// �w�胍�W�b�N�p�������@�\�̂�
	int targetPCID = mcls_pStatus->GetHtParaID(nKind);

	//// PCID���� �J�����y�A���Z�b�g
	// �����W�b�N�p�������̂݃Z�b�g����
	if (targetPCID > 0 && targetPCID <= NUM_ALL_HANTEI)
	{
		for (int ii = 0; ii < NUM_MEN; ii++)
		{
			// �P��
			for (int jj = 0; jj < NUM_HANTEI; jj++)
			{
				int pcid = mcls_pStatus->GetHtID(ii, jj);

				if (pcid > 0 && pcid <= NUM_ALL_HANTEI)
				{
					que.fl.data[6 + 3 * (pcid - 1) + 0] = 0;			// �J�����y�A�h�c�i�N���A�j
					que.fl.data[6 + 3 * (pcid - 1) + 1] = 0;			// ���g�p
					que.fl.data[6 + 3 * (pcid - 1) + 2] = DIV_HANTEI_HT;// �P���@�\
				}
			}
#ifdef ENABLE_CYCLE
			// ����
			for (int jj = 0; jj < NUM_HANTEI_CYCLE; jj++)
			{
				int pcid = gcls_Status.GetCyID(ii, jj);

				if (pcid > 0 && pcid <= NUM_ALL_HANTEI)
				{
					que.fl.data[6 + 3 * (pcid - 1) + 0] = 0;			// �J�����y�A�h�c�i�N���A�j
					que.fl.data[6 + 3 * (pcid - 1) + 1] = 0;			// ���g�p
					que.fl.data[6 + 3 * (pcid - 1) + 2] = DIV_HANTEI_CY;// �����@�\
				}
			}
#endif
		}
		// �N���A���Ă���Z�b�g
		for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
		{
			int pcid = mcls_pStatus->GetHtParaID(ii);

			if (pcid > 0 && pcid <= NUM_ALL_HANTEI) {

				que.fl.data[6 + 3 * (pcid - 1) + 0] = 0;				// �J�����Z�b�g�P�i�N���A�j
				que.fl.data[6 + 3 * (pcid - 1) + 1] = 0;				// �J�����Z�b�g�Q�i�N���A�j
				que.fl.data[6 + 3 * (pcid - 1) + 2] = DIV_HANTEI_PR;	// �p������
			}
		}
		int camset1 = GetHtParaCamNo(nKind, 0);
		int camset2 = GetHtParaCamNo(nKind, 1);


		// 0�̂܂ܑ���Ɖ����ɑ΂��Ă̗v���Ȃ̂�������Ȃ����߁A
		// -1�ő��M���A���葤��0�ɖ߂�
		que.fl.data[6 + 3 * (targetPCID - 1) + 0] = (0 == camset1 ? -1 : camset1);
		que.fl.data[6 + 3 * (targetPCID - 1) + 1] = (0 == camset2 ? -1 : camset2);
		que.fl.data[6 + 3 * (targetPCID - 1) + 2] = DIV_HANTEI_PR;

		// �J�����Z�b�g����ʂ𔻒�
		int nMen = 0;
		if (0 != camset1) nMen = (0 == ((camset1 - 1) / (MAX_CAMSET / NUM_MEN)) ? 0 : 1);
		if (0 != camset2) nMen = (0 == ((camset2 - 1) / (MAX_CAMSET / NUM_MEN)) ? 0 : 1);

		// ���R�C���̌����\�ʏ�Ԃ�ݒ�
		// ������N�����̓R�C���̃Z�b�g���ǂ��t���Ȃ��\��������̂ŁA���̏ꍇ�͏����l
		if (NULL != pCoilManager && NULL != pCoilManager->GetCoilBaseKen())
		{
			for (int ii = 0; ii < NUM_MEN; ii++)
			{
				if (0 != pCoilManager->GetCoilKenR(nMen)->nSt_Spm)
				{
					// �������
					que.fl.data[1 + ii] = pCoilManager->GetCoilBaseKen()->setting.scode[ii];
				}
				else
				{
					// �J�����
					que.fl.data[1 + ii] = pCoilManager->GetCoilBaseKen()->setting.scode_spmoff[ii];
				}
			}
		}
	}

	//// ���M
	int iRet = send_mail(HT_DEFECT, "*", &que);
	LOG(em_MSG), "[%s] �r���J�n�v�� (FACT_HT_DEFECT_22) [PCID=%d] %d", my_sThreadName, targetPCID, que.fl.data[0]);
	if (0 != iRet) syslog(SYSNO_MAIL_SNED_ERR, "[�r���J�n�v�� err_code=%d]", iRet);

	// ----------------------
	// �p�������@�\�V�X�e����ԕύX
	mcls_pStatus->SetParaSys(nKind, DIV_SYS_WAITSTART);

	//======================================================
	// ����PC����̉����҂��^�C�}�[�N��
	__int64 ts = (__int64)PARAM_TIMEOUT_RESUME_WAIT * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evTimeResumeWait[nKind], (LARGE_INTEGER*)&ts, 0, NULL, NULL, FALSE);

}

//==========================================
// ����PC �� �r����~��ʒm
//------------------------------------------
// int nKind	: �����J�n�敪�i0�F���W�b�N�p�������@�\1�A1�F���W�b�N�p�������@�\2�j
//==========================================
void LogicParaRunFunc::Send_HtResume_Stop(int nKind)
{
	EM_DIV_UNTEN  emUnten = mcls_pStatus->GetUnten();

	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_HT_DEFECT_23;
	que.nLineNo	 = getlineid();

	//// �Ƃ肠���� �������Z�b�g
	for (int ii = 0; ii < NUM_ALL_HANTEI; ii++)
	{
		que.fl.data[ii] = -1;
	}

	// �w�胍�W�b�N�p�������@�\�̂�
	// �@���W�b�N�p�������@�\1�F�\�̂P���
	// �@���W�b�N�p�������@�\2�F���̂P���
	int targetPCID = mcls_pStatus->GetHtParaID(nKind);

	//// PCID���� �J�����y�A���Z�b�g
	// �����W�b�N�p�������̂݃Z�b�g����
	if (targetPCID > 0 && targetPCID <= NUM_ALL_HANTEI)
	{
		que.fl.data[targetPCID - 1] = 0;
	}

	//// ���M
	int iRet = send_mail(HT_DEFECT, "*", &que);
	LOG(em_MSG), "[%s] �r����~�v�� (FACT_HT_DEFECT_23) PCID=%d", my_sThreadName, targetPCID);
	if (0 != iRet) syslog(SYSNO_MAIL_SNED_ERR, "[�r����~�v�� err_code=%d]", iRet);

	// ----------------------
	// �p�������@�\�V�X�e����ԕύX
	mcls_pStatus->SetParaSys(nKind, DIV_SYS_WAITSTOP);
}

//==========================================
// �f�o�b�O�p �J�����I��ݒ芮���ʒm���M
//==========================================
void LogicParaRunFunc::DebugSend_CamLinksw_CamsetCmpl(int mode)
{
	if (mlst_SendCmd.size() <= m_nSendCmdIndex)
	{
		// TO_LINKSW���M�R�}���h���X�gEmpty

		// �����J�n�A�r���Q���ȊO�ŃJ�����I��ݒ芮�����ʒm�����P�[�X
		COMMON_QUE que;
		que.nEventNo = FACT_TO_DEFECT_32;
		que.nLineNo = getlineid();
		que.fl.data[0] = 1;
		que.fl.data[1] = 2;
		que.fl.data[2] = 3;
		que.fl.data[3] = 4;
		que.fl.data[4] = 1;
		send_mail(TO_DEFECT, ".", &que);
	}
	else
	{
		// �����J�n�A�r���Q����

		if (0 == mode || 1 == mode)
		{
			// ���퉞���A�ُ퉞��

			// �J�����I��ݒ���{���Ɍ���ʒm
			if (FACT_TO_LINKSW_03 == mlst_SendCmd[m_nSendCmdIndex].sendCmd.nEventNo)
			{
				// ���M���̃J�����I��ݒ�R�}���h���牞���`���𐶐��E���g�ɕԑ�
				COMMON_QUE que;
				que.nEventNo = FACT_TO_DEFECT_32;
				que.nLineNo = getlineid();
				que.fl.data[0] = mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[0];
				que.fl.data[1] = mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[1];
				que.fl.data[2] = mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[2];
				que.fl.data[3] = mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[3];
				que.fl.data[4] = (0 == mode ? 1 : 0);
				send_mail(TO_DEFECT, ".", &que);
			}
		}
		else
		{
			// �`���s��v�i�J����No�ŕs��v�ɂ���j
			COMMON_QUE que;
			que.nEventNo = FACT_TO_DEFECT_32;
			que.nLineNo = getlineid();
			que.fl.data[0] = mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[0];
			que.fl.data[1] = mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[1];
			que.fl.data[2] = 99;
			que.fl.data[3] = mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[3];
			que.fl.data[4] = 1;
			send_mail(TO_DEFECT, ".", &que);
		}
	}
}

//==========================================
// �p�������J�����̗L�����`�F�b�N
//------------------------------------------
// long nFPGABoardNo	: �X�C�b�`FPGA�{�[�hID (0 - 7)
// long nCamno			: �J����No (1 - 20)
//==========================================
bool LogicParaRunFunc::IsTargetCamNo(long nFPGABoardNo, long nCamno)
{
	bool bTargetCam = false;
	CString sWk;
	bool bTargetPC[NUM_ALL_HANTEI];

	memset(&bTargetPC, false, sizeof(bTargetPC));

	// �@�X�C�b�`FPGA�{�[�hID����O���[�vID���Z�o�i0-7 �� 0-3�j
	int nTargetGroupID = nFPGABoardNo / 2;

	// �A�p������PC�Ɠ����O���[�v�̔���PC����������
	for (int ii = 0; ii < NUM_MEN; ii++)
	{
		for (int jj = 0; jj < NUM_HANTEI; jj++)
		{
			int nPcID = mcls_pStatus->GetHtID(ii, jj);
			if (0 == nPcID) continue;
			for (int kk = 0; kk < NUM_CAMLINK_BOARD; kk++)
			{
				int nGroupID = ToLinkswUtil::GetHanteiGroup(ii, jj, kk + 1);
				if (nGroupID == nTargetGroupID) bTargetPC[nPcID - 1] = true;
			}
		}
	}

	for (int ii = 0; ii < NUM_ALL_HANTEI; ii++)
	{
		if (bTargetPC[ii])
		{
			// �B����PC�Ɋ�����Ă���J�����Z�b�g���擾����
			int nCamset = m_nSelectCamset[ii];

			// �C�J�����Z�b�g���J�����ԍ��ɕϊ�����
			int nCam1 = (nCamset * 2) - 1;
			int nCam2 = nCamset * 2;

			// �D�w��J�������J�����ԍ��Ɋ܂܂�邩�`�F�b�N
			if (nCam1 == nCamno || nCam2 == nCamno)
			{
				bTargetCam = true;
				break;
			}
		}
	}

	return bTargetCam;
}
//------------------------------------------
// ���̃J�����I��ݒ�R�}���h���M�i�L���[����j
// ���\��m_nSendCamIndex�����Z���Ă���Ăяo������
//------------------------------------------
void LogicParaRunFunc::Send_NextCmdSelect()
{
	// �J�����ݒ�R�}���h�L���[�`�F�b�N
	if (mlst_SendCmd.empty())
	{
		// �S�J�����ݒ�R�}���h���M��ɃA���T�[�����������ꍇ�ɁA���̃P�[�X�ɓ��肤��
		// �������^�C���A�E�g���J��Ԃ��A�S�R�}���h���M����������ɃA���T�[�����������ꍇ

		LOG(em_ERR), "[%s] ���M�R�}���h�����I�I", my_sThreadName);
		// �R�}���h���X�g�N���A�i�O�ׁ̈j
		ClearLinkswCmd();
	}
	else
	{
		// �J�����ݒ�R�}���h���Ƒ��M�ʒu���疢���M�R�}���h�̗L�����`�F�b�N����
		if (mlst_SendCmd.size() > m_nSendCmdIndex)
		{
			// �����M�J�����ݒ�R�}���h����i���M���j

			switch (mlst_SendCmd[m_nSendCmdIndex].sendCmd.nEventNo)
			{
				// �J�����I��ݒ�R�}���h (��TO_LINKSW)
				case FACT_TO_LINKSW_03:
					{
						int nCamSet = mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[2];

						// �����M�J�����ݒ�R�}���h���M
						if (IsTargetCamNo(mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[0], nCamSet))
						{
							// �J����No��ϊ��i1-20 �� 1-8�j
							mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[2] = nCamSet - mlst_SendCmd[m_nSendCmdIndex].topCamNo;
						}
						else
						{
							// �ΏۊO�̃J�������I�����ꂽ
							LOG(em_ERR), "[%s] �ΏۊO�̃J�������I������Ă���[�X�C�b�`FPGA�{�[�hID=%d, �t���[���O���o�[�{�[�hNo=%d, �J����No=%d Master/Slave�ݒ�=%d]",
								my_sThreadName,
								mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[0],
								mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[1],
								mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[2],
								mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[3]);
							syslog(235, "�ΏۊO�̃J�������I������Ă���[�X�C�b�`FPGA�{�[�hID=%d, �t���[���O���o�[�{�[�hNo=%d, �J����No=%d Master/Slave�ݒ�=%d]",
								mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[0],
								mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[1],
								mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[2],
								mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[3]);
							// �o�͒�~
							mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[2] = 0;
							// �@���ԁ|���W�b�N�p�������@�\xx_�J����xx�I�����ʁi�ُ�j
							KizuFunction::SetStatus(mcls_pStatus->GetStatusRead().ParaState[mlst_SendCmd[m_nSendCmdIndex].prNo - 1][mlst_SendCmd[m_nSendCmdIndex].camNo - 1].id, false);

							// �w��J�����N���A
							SetHtParaCamNo(mlst_SendCmd[m_nSendCmdIndex].prNo - 1, mlst_SendCmd[m_nSendCmdIndex].camNo - 1, 0);
						}

						// �R�}���h���M
						LOG(em_MSG), "[%s] �J�����I��ݒ�R�}���h���M(FACT_TO_LINKSW_03) [�X�C�b�`FPGA�{�[�hID=%d, �t���[���O���o�[�{�[�hNo=%d, �J����No=%d(%d) Master/Slave�ݒ�=%d]",
							my_sThreadName,
							mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[0],
							mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[1],
							mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[2],
							nCamSet,
							mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[3]);
						ToOutMailSender::Send_ToLinksw_ChangeCam(&mlst_SendCmd[m_nSendCmdIndex].sendCmd);

						// �J���������N�X�C�b�`�ݒ�A���T�[�҂��^�C�}�[�N��
						__int64 ts = (__int64)PARAM_TIMEOUT_CAMSET * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
						SetWaitableTimer(m_evTimeCamset, (LARGE_INTEGER*)&ts, 0, NULL, NULL, FALSE);
					}
					break;

				// �r���Q���ݒ�R�}���h	(��TO_LINKSW)
				case FACT_TO_LINKSW_04:
					{
						// �I�y���[�^�@�\�֌���Ԃ𑗐M
						// ���ғ����ɑJ�ڊ������Ă���Ƃ��̂ݑ��M
						if (DIV_KADOU_START == mcls_pStatus->GetKadou())
						{
							Send_SoGamenn_MailStatus();
							OpSendFunc::SendOp_Status();
						}

						// �r���Q���ݒ�A���T�[�҂��^�C�}�[�j���i�O�̂��߁j
						CancelWaitableTimer(m_evTimeResumeStart);

						// ���݂̃t���[��No�ɓr���Q���t���[�������Z
						mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[1] = mcls_pPlg->GetFrameNo() + NUM_JOIN_FRAME;

						LOG(em_MSG), "[%s] �r���Q���ݒ�R�}���h���M(FACT_TO_LINKSW_04)[�\���敪=%d �r���Q���t���[��No=%d]", my_sThreadName, mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[0], mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[1]);
						ToOutMailSender::Send_ToLinksw_JoinReq(&mlst_SendCmd[m_nSendCmdIndex].sendCmd);

						// �r���Q���ݒ�A���T�[�҂��^�C�}�[�N��
						__int64 ts = (__int64)PARAM_TIMEOUT_RESUME_START * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
						SetWaitableTimer(m_evTimeResumeStart, (LARGE_INTEGER*)&ts, 0, NULL, NULL, FALSE);
					}
					break;

				default:
					LOG(em_ERR), "[%s] �z��O�̃R�}���h[%d]", my_sThreadName, mlst_SendCmd[m_nSendCmdIndex].sendCmd.nEventNo);
					// �R�}���h���X�g�N���A
					ClearLinkswCmd();
					break;
			}
		}
		else
		{
			// �����M�J�����ݒ�R�}���h�Ȃ��i�S�đ��M�ς݁j
			// ���o�^���ăR�}���h���𑗐M�ʒu���������ꍇ�A
			// �@�S�R�}���h���M�ς݂Ƃ�������

			LOG(em_MSG), "[%s] �S�J���������N�X�C�b�`�R�}���h���M�����I�I", my_sThreadName);

			// �J�����I��ݒ�A���T�[�҂��^�C�}�[�j���i�O�̂��߁j
			CancelWaitableTimer(m_evTimeCamset);
			// �r���Q���ݒ�A���T�[�҂��^�C�}�[�j���i�O�̂��߁j
			CancelWaitableTimer(m_evTimeResumeStart);
			// �R�}���h���X�g�N���A
			ClearLinkswCmd();

			// �I�y���[�^�@�\�֌���Ԃ𑗐M
			// ���ғ����ɑJ�ڊ������Ă���Ƃ��̂ݑ��M
			if (DIV_KADOU_START == mcls_pStatus->GetKadou())
			{
				Send_SoGamenn_MailStatus();
				OpSendFunc::SendOp_Status();
			}
		}
	}
}

//------------------------------------------
// ���M���̃J�����ݒ�R�}���h�̉������`�F�b�N����
// ���^�C���A�E�g��̎�M���l�����A�R�}���h�L���[��
//   ���ꂩ�ƈ�v���邩������
//------------------------------------------
// COMMON_QUE* que	: ���[���X���b�g�ʐM�p�\����
//------------------------------------------
bool LogicParaRunFunc::IsEqualCamsetCMD(COMMON_QUE* que)
{
	bool bResp = false;
	std::vector<LINKSW_CMD>::iterator itr;

	for (itr = mlst_SendCmd.begin(); itr != mlst_SendCmd.end(); itr++)
	{
		if (itr->sendCmd.fl.data[0] == que->fl.data[0] &&
			itr->sendCmd.fl.data[1] == que->fl.data[1] &&
			itr->sendCmd.fl.data[2] == que->fl.data[2] &&
			itr->sendCmd.fl.data[3] == que->fl.data[3])
		{
			// ���M�R�}���h�ƈ�v
			bResp = true;
			itr->bRecv = true;
			break;
		}
	}

	return bResp;
}

//------------------------------------------
// �w��p�������̃J�����ݒ芮���ʒm�`�F�b�N
// ���P���ł������M�������true��Ԃ�
//------------------------------------------
// int nPrNo	: 1�F���W�b�N�p�������P�A2�F���W�b�N�p������2
//------------------------------------------
bool LogicParaRunFunc::IsRecvCamsetCMD(int nPrNo)
{
	bool bResp = false;
	std::vector<LINKSW_CMD>::iterator itr;

	for (itr = mlst_SendCmd.begin(); itr != mlst_SendCmd.end(); itr++)
	{
		if ((itr->prNo == nPrNo) && (itr->bRecv))
		{
			// ��M����
			bResp = true;
			break;
		}
	}

	return bResp;
}

//------------------------------------------
// �X���b�h�L���[�Ƀ����N�X�C�b�`�R�}���h�o�^
//------------------------------------------
// LINKSW_CMD* cmd �����N�X�C�b�`�R�}���h�\����
//------------------------------------------
void LogicParaRunFunc::SetLinkswCmd(LINKSW_CMD cmd)
{
	mlst_SendCmd.push_back(cmd);
}

//------------------------------------------
// �R�}���h���X�g�N���A
//------------------------------------------
// ENUM_LINKSW_CMD state �ʐM���
//------------------------------------------
void LogicParaRunFunc::ClearLinkswCmd(ENUM_LINKSW_CMD state)
{
	// �J���������N�X�C�b�`�R�}���h�ʐM��Ԑݒ�
	m_LinkswCmdKind = state;
	// �J���������N�X�C�b�`�R�}���h���M�ʒu������
	m_nSendCmdIndex = 0;
	// �R�}���h���X�g�N���A
	mlst_SendCmd.clear();
}

//------------------------------------------
// �R�}���h���X�g�N���A
//------------------------------------------
void LogicParaRunFunc::RegistLinkswCmd()
{
	// �J�����I��ݒ�R�}���h�L���[�C���O�i�S���W�b�N�p������PC���j
	CString sWk;
	LINKSW_CMD cmd;
	for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
	{
		int pcid = mcls_pStatus->GetHtParaID(ii);
		if (0 == pcid) continue;				// ���W�b�N�p������PC������

		for (int jj = 0; jj < NUM_CAMLINK_BOARD; jj++)
		{
			sWk.Format("CAMLINKSW_PCID%d_%d", pcid, jj + 1);
			int nWk = GetPrivateProfileInt(TO_DEFECT, sWk, 0, TASKINI_NAME);
			int nFPGABoardNo = nWk / 100;
			int nFPGACh = nWk % 100;
			int nCamNo = GetHtParaCamNo(ii, jj);

			if (0 == nFPGACh) continue;			// �{�[�h������

												// �J�����I��ݒ�R�}���h�o�^�iSlave�w��j
			memset(&cmd, 0x00, sizeof(LINKSW_CMD));
			cmd.prNo = ii + 1;										// ���W�b�N�p������PC no
			cmd.camNo = jj + 1;										// ���W�b�N�p�������J���� no
			cmd.topCamNo = mcls_pStatus->GetGroupTopCamNo(nFPGABoardNo);	// �擪�J����No �O���[�v���̓��̓J�����̐擪
			cmd.bRecv = false;										// ��M�t���O�N���A
			ToOutMailSender::Create_ToLinksw_ChangeCam(nFPGABoardNo, nFPGACh, nCamNo, -1, &cmd.sendCmd);
			SetLinkswCmd(cmd);
		}
	}
	// �r���Q���ݒ�R�}���h�L���[�C���O�i�S���W�b�N�p������PC���j
	for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
	{
		// ���M���O�ŃZ�b�g���邽�߁A�r���Q���t���[���͂O���Z�b�g
		memset(&cmd, 0x00, sizeof(LINKSW_CMD));
		cmd.prNo = ii + 1;												// ���W�b�N�p������PC no
		ToOutMailSender::Create_ToLinksw_JoinReq(2 + ii, 0, &cmd.sendCmd);
		SetLinkswCmd(cmd);
	}
}

//==========================================
// �J�����I��ݒ�҂�����(�^�C���A�E�g����)
//==========================================
void LogicParaRunFunc::WaitCamset_TimeOut()
{
	if (E_LINKSW_CMD_NOP != m_LinkswCmdKind)
	{
		// ----------------------
		// �@���ԁ|���W�b�N�p�������@�\xx_�J����xx�I�����ʁi�ُ�j
		KizuFunction::SetStatus(mcls_pStatus->GetStatusRead().ParaState[mlst_SendCmd[m_nSendCmdIndex].prNo - 1][mlst_SendCmd[m_nSendCmdIndex].camNo - 1].id, false, false);
		syslog(236, "[�J�����I��ݒ�A���T�[�^�C���A�E�g ���W�b�N�p������%d �J����%d]", mlst_SendCmd[m_nSendCmdIndex].prNo, mlst_SendCmd[m_nSendCmdIndex].camNo);

		// ���R�}���h���M�Ɉڍs
		m_nSendCmdIndex++;

		// ���̃R�}���h���M�i�L���[����j
		Send_NextCmdSelect();
	}
	else
	{
		LOG(em_ERR), "[%s] �����J�n(�r���Q��)���{���ȊO�ŃJ�����ݒ芮���ʒm����M", my_sThreadName);
		// �R�}���h���X�g�N���A
		ClearLinkswCmd();
	}
}

//==========================================
// �r���J�n�ݒ�҂�����(�^�C���A�E�g����)
//==========================================
void LogicParaRunFunc::WaitResumeStart_Timeout()
{
	if (E_LINKSW_CMD_NOP != m_LinkswCmdKind)
	{
		// ���W�b�N�p��������� �| �Z�b�g
		mcls_pStatus->SetHtParaState(mlst_SendCmd[m_nSendCmdIndex].prNo - 1, DIV_BOOL_FALSE);
		// �p�������@�\�V�X�e����ԕύX
		mcls_pStatus->SetParaSys(mlst_SendCmd[m_nSendCmdIndex].prNo - 1, DIV_SYS_STOP);
		syslog(236, "[�r���J�n�ݒ�A���T�[�^�C���A�E�g ���W�b�N�p������%d]", mlst_SendCmd[m_nSendCmdIndex].prNo);

		// ���R�}���h���M�Ɉڍs
		m_nSendCmdIndex++;

		// ���̃R�}���h���M�i�L���[����j
		Send_NextCmdSelect();
	}
	else
	{
		LOG(em_ERR), "[%s] �����J�n(�r���Q��)���{���ȊO�œr���Q�������ʒm����M", my_sThreadName);
		// �R�}���h���X�g�N���A
		ClearLinkswCmd();
	}
}

//==========================================
// �r���J�n����A���T�[�҂�����(�^�C���A�E�g����)
//------------------------------------------
// int nKind				: �����J�n�敪�i0�F���W�b�N�p�������@�\1�A1�F���W�b�N�p�������@�\2�j
//==========================================
void LogicParaRunFunc::WaitResumeWait_Timeout(int nKind)
{
	// ���W�b�N�p��������� �| �N���A
	mcls_pStatus->SetHtParaState(nKind, DIV_BOOL_FALSE);

	// �p�������@�\�V�X�e����ԕύX
	mcls_pStatus->SetParaSys(nKind, DIV_SYS_STOP);

	// �r����~�v��
	COMMON_QUE que;
	que.fl.data[0] = nKind;
	ResumeStop(&que);

	// �I�y���[�^�@�\�֌���Ԃ𑗐M
	Send_SoGamenn_MailStatus();
	OpSendFunc::SendOp_Status();

	syslog(236, "[�r���J�n����A���T�[�^�C���A�E�g ���W�b�N�p������%d]", nKind + 1);

}

//==========================================
// �J�����I��ݒ芮������
//------------------------------------------
// COMMON_QUE* que	: �N���v��	
//==========================================
void LogicParaRunFunc::WaitCamset_Answer(COMMON_QUE* que)
{
	if (mlst_SendCmd.size() <= m_nSendCmdIndex)
	{
		// TO_LINKSW���M�R�}���h���X�gEmpty

		if (EM_DIV_UNTEN::DIV_UNTEN_CAMERA != mcls_pStatus->GetUnten())
		{
			LOG(em_ERR), "[%s] �J�����I��ݒ�A���T�[ �ُ팟�o(����M�������ُ�)", my_sThreadName);
		}
		else
		{
			LOG(em_INF), "[%s] iPortViewer����̃J�����I��ݒ�ɂ�鉞��", my_sThreadName);
		}

		// �R�}���h���X�g�N���A
		ClearLinkswCmd();
		return;
	}

	// �ݒ茋�ʃ`�F�b�N
	if (1 != que->fl.data[4])
	{
		// �ݒ茋�ʁF���s
		LOG(em_ERR), "[%s] �J�����I��ݒ�A���T�[ �ُ팟�o(���ʈُ�)", my_sThreadName);

		// �@���ԁ|���W�b�N�p�������@�\xx_�J����xx�I�����ʁi�ُ�j
		KizuFunction::SetStatus(mcls_pStatus->GetStatusRead().ParaState[mlst_SendCmd[m_nSendCmdIndex].prNo - 1][mlst_SendCmd[m_nSendCmdIndex].camNo - 1].id, false);
	}
	else
	{
		// �ݒ茋�ʁF����

		// �`���`�F�b�N
		// �@�X�C�b�`FPGA�{�[�hID
		// �A�t���[���O���o�[�{�[�hNo
		// �B�J����No
		// �CMaster�^Slave�ݒ�
		if (mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[0] != que->fl.data[0] ||
			mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[1] != que->fl.data[1] ||
			mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[2] != que->fl.data[2] ||
			mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[3] != que->fl.data[3])
		{
			// �ُ퉞���i���M�����`���ƍ��v���Ȃ��j

			// �O�ɑ��M�����������x��Ă����H
			if (IsEqualCamsetCMD(que))
			{
				LOG(em_MSG), "[%s] �O�̃J�����I��ݒ�̃A���T�[���x��Ď�M���ꂽ�I�I", my_sThreadName);
				return;	// �{���̉���������͂��Ȃ̂ŁA���R�}���h�͑��M�����ɏI��
			}
			else
			{
				LOG(em_INF), "[%s] �O��̌����J�n�A�܂��͓r���Q���ł̎d�|������M���ꂽ�I�I", my_sThreadName);
				return;	// �{���̉���������͂��Ȃ̂ŁA���R�}���h�͑��M�����ɏI��
			}
		}
		else
		{
			// �����M
			mlst_SendCmd[m_nSendCmdIndex].bRecv = true;

			// �@���ԁ|���W�b�N�p�������@�\ ���̓J����
			int nCamno = GetHtParaCamNo(mlst_SendCmd[m_nSendCmdIndex].prNo - 1, mlst_SendCmd[m_nSendCmdIndex].camNo - 1);
			nCamno = (0 != que->fl.data[2] ? nCamno : que->fl.data[2]);
			KizuFunction::SetStatus(mcls_pStatus->GetStatusRead().ParaCam[mlst_SendCmd[m_nSendCmdIndex].prNo - 1][mlst_SendCmd[m_nSendCmdIndex].camNo - 1].id, nCamno, false);
		}
	}

	// ���R�}���h���M�Ɉڍs
	m_nSendCmdIndex++;

	// ���̃R�}���h���M�i�L���[����j
	Send_NextCmdSelect();
}

//==========================================
// �r���Q���ݒ芮������
//------------------------------------------
// COMMON_QUE* que				   : �N���v��	
// CoilManager const *pCoilManager : �R�C�����
//==========================================
void LogicParaRunFunc::WaitLinkswJoin_Answer(COMMON_QUE* que, CoilManager const *pCoilManager)
{
	if (DIV_UNTEN_CAMERA != mcls_pStatus->GetUnten())
	{
		// �u�J�����Z���v���[�h�ȊO

		COMMON_QUE* pQue = new COMMON_QUE;
		memcpy(pQue, que, sizeof(COMMON_QUE));

		// L[0]  �\���敪 �i0�F�\�A1�F���A2�F���W�b�N�p�������P�A3�F���W�b�N�p������2�j
		// L[1]  �r���Q���t���[��No �i0�I���W���j
		int nTorB = pQue->fl.data[0];
		int nJoinFrameNo = pQue->fl.data[1];
		if (2 == nTorB || 3 == nTorB)
		{
			// �r���J�n�v��
			Send_HtResume_Start(nTorB - 2, pCoilManager, nJoinFrameNo);
		}
		else
		{
			LOG(em_INF), "[%s] �r���Q���ݒ芮���ʒm�F�����ȕ\���敪", my_sThreadName);
		}

		delete pQue;

		// ���R�}���h���M�Ɉڍs
		m_nSendCmdIndex++;

		// ���̃R�}���h���M�i�L���[����j
		Send_NextCmdSelect();
	}
	else
	{
		// �u�J�����Z���v���[�h��

		LOG(em_ERR), "[%s] �J�����Z�����[�h���͓r���Q���ݒ芮���ʒm����", my_sThreadName);
		// �I�y���[�^�@�\�։ғ���Ԃ𑗐M
		Send_SoGamenn_MailStatus();
	}
}
