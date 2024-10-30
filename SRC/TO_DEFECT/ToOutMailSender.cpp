#include "StdAfx.h"
#include "ToOutMailSender.h"

ToOutMailSender::ToOutMailSender(void)
{
}

ToOutMailSender::~ToOutMailSender(void)
{
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PC�}�X�^�[

//------------------------------------------
// PC�}�X�^�[��ԊǗ��� �����ݒ�ʒm
//------------------------------------------
void ToOutMailSender::Send_KsMaster_TimeSet()
{
    SYSTEMTIME time;
    GetLocalTime(&time);	

	COMMON_QUE que;
	que.nEventNo = FACT_KS_MASTER_01;
	que.nLineNo = getlineid();
	memcpy(&que.timeset_que.systemtime, &time, sizeof(time));  

	// ���M
	int iRet = send_mail(KS_MASTER, MASTER_PC_NAME, &que);
}

//------------------------------------------
// �}�X�^�[PC��ԊǗ���PC�����ύX���ꂽ����ʒm
//------------------------------------------
void ToOutMailSender::Send_KsMaster_PcChange()
{
	// �SPC�̑S�^�X�N�ցA�e�@�\�ɂ�����PC���̂��ς��������ʒm����B
	COMMON_QUE que;
	que.nEventNo = FACT_KS_PCNAME_CHANGE; 
	que.nLineNo = getlineid();
	send_mail(KS_MASTER, ".", &que);
	Sleep(50);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �J�����R���g���[���[

//------------------------------------------
// �J�����R���g���[���[�� AGC�ݒ� ��ʒm
// int camerano �J�����ԍ� (0�`15)
// int mode 0:���Βl 1:��Βl 
// int ro �I������ ���Βl ���� ��Βl (-1:���� 0:���̂܂� 1:�グ)
//------------------------------------------
void ToOutMailSender::Send_ToCamera_AGC(int camerano, int mode, int ro)
{
	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;
	que.nLineNo = getlineid();
	que.fl.data[0] = camerano;
	que.fl.data[1] = mode;
	que.fl.data[2] = ro;

	// ���M
	int iRet = send_mail(TO_CAMERA, ".", &que);
// �ُ�ɂȂ�Ƃ��̂������ʂ̃G���[���o��̂Ń���
//	if( 0 != iRet ) syslog(SYSNO_MAIL_SNED_ERR, "[�J�����R���g���[���[��AGC�ݒ� err_code=%d]", iRet);
}

//------------------------------------------
// �J�����R���g���[���[�� �p�����[�^�ύX��ʒm
// int scode �����\�ʏ��No
//------------------------------------------
void ToOutMailSender::Send_ToCamera_Param(int scode)
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_TO_CAMERA_01;
	que.nLineNo = getlineid();
	que.fl.data[0] = scode;
	// ���M
	int iRet = send_mail(TO_CAMERA, ".", &que);
	if( 0 != iRet ) syslog(SYSNO_MAIL_SNED_ERR, "[�J�����R���g���[���[�փp�����[�^�ύX�v�� err_code=%d]", iRet);
}
//------------------------------------------
// �J�����R���g���[���[�� �p���X�o�� ��ʒm
// bool onoff ture:on false:off
//------------------------------------------
void ToOutMailSender::Send_ToCamera_Palse(bool onoff)
{
	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_03;
	que.nLineNo = getlineid();
	que.fl.data[0] = onoff ? 0 : 1;

	// ���M
	int iRet = send_mail(TO_CAMERA, ".", &que);
	if( 0 != iRet ) syslog(SYSNO_MAIL_SNED_ERR, "[�J�����R���g���[���[�փp���X�o�� err_code=%d]", iRet);
}


//------------------------------------------
// �J�����R���g���[���[�� �����J�n ��ʒm
// EM_DIV_PLG em_plg PLG�敪
//------------------------------------------
void ToOutMailSender::Send_ToCamera_Start(EM_DIV_PLG em_plg)
{
	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_02;
	que.nLineNo = getlineid();
	que.fl.data[0] = em_plg;

	// ���M
	int iRet = send_mail(TO_CAMERA, ".", &que);
	if( 0 != iRet ) syslog(SYSNO_MAIL_SNED_ERR, "[�J�����R���g���[���[�֌����J�n err_code=%d]", iRet);
}

//------------------------------------------
// �J�����R���g���[���[�� �Y���R�C���ɍœK�Ȑݒ��ʒm����
// int TorB �\:0 ��:1
// int scode �����\�ʏ�� (1�I���W��)
//------------------------------------------
void ToOutMailSender::Send_ToCamera_CoilChange(int TorB, int scode)
{
	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_05;
	que.nLineNo = getlineid();
	que.fl.data[0] = scode;
	que.fl.data[1] = TorB;

	// ���M
	int iRet = send_mail(TO_CAMERA, ".", &que);
	if( 0 != iRet ) syslog(SYSNO_MAIL_SNED_ERR, "[�J�����R���g���[���[�փR�C���ؑ� err_code=%d]", iRet);
}

//------------------------------------------
// �J�����R���g���[���[�� ���I�����Ԗ₢���킹�ʒm
//------------------------------------------
void ToOutMailSender::Send_ToCamera_AgcVal()
{
	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_06;
	que.nLineNo = getlineid();

	// ���M
	send_mail(TO_CAMERA, ".", &que);
}

//------------------------------------------
// �J���������N�X�C�b�`�ʐM �� �p�����[�^�ύX�ʒm
// int scode �����\�ʏ��No (1�I���W��)
//------------------------------------------
void ToOutMailSender::Send_ToLinksw_Param(int scode)
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));

	que.nEventNo		= FACT_TO_LINKSW_01;
	que.nLineNo			= getlineid();
	que.fl.data[0]		= scode;

	// ���M
	int nRetc = send_mail(TO_LINKSW, ".", &que);
	if( 0 != nRetc ) syslog(SYSNO_MAIL_SNED_ERR, "[�J���������N�X�C�b�`�ʐM�ւ̃p�����[�^�ύX�ʒm err_code=%d]", nRetc);
}

//------------------------------------------
// �J���������N�X�C�b�`�ʐM �� �����J�n�ݒ�
//------------------------------------------
void ToOutMailSender::Send_ToLinksw_Start()
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));

	que.nEventNo		= FACT_TO_LINKSW_02;
	que.nLineNo			= getlineid();

	// ���M
	int nRetc = send_mail(TO_LINKSW, ".", &que);
	if( 0 != nRetc ) syslog(SYSNO_MAIL_SNED_ERR, "[�J���������N�X�C�b�`�ʐM�ւ̌����J�n�ݒ� err_code=%d]", nRetc);
}

//------------------------------------------
// �J���������N�X�C�b�`�ʐM �� �J�����I��ݒ�
// int SwitchID �X�C�b�`FPGA�{�[�hID (0�I���W��)
// int BoardNo �t���[���O���o�[�{�[�hNo (1�`15)
// int CamNo �J����No (1�`8�A0:�o�͒�~)
// int nKind Master�^Slave�ݒ�i1�FMaster�A-1�FSlave�j
//------------------------------------------
void ToOutMailSender::Send_ToLinksw_ChangeCam(int SwitchID, int BoardNo, int CamNo, int nKind)
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));

	que.nEventNo		= FACT_TO_LINKSW_03;
	que.nLineNo			= getlineid();
	que.fl.data[0]		= SwitchID;
	que.fl.data[1]		= BoardNo;
	que.fl.data[2]		= CamNo;
	que.fl.data[3]		= nKind;

	// ���M
	int nRetc = send_mail(TO_LINKSW, ".", &que);
	if( 0 != nRetc ) syslog(SYSNO_MAIL_SNED_ERR, "[�J���������N�X�C�b�`�ʐM�ւ̃J�����I��ݒ� err_code=%d]", nRetc);
}
//------------------------------------------
// �J���������N�X�C�b�`�ʐM �� �J�����I��ݒ�
// COMMON_QUE *que ���[���X���b�g�ʐM�p�\����
//------------------------------------------
void ToOutMailSender::Send_ToLinksw_ChangeCam(COMMON_QUE *que)
{
	// ���M
	int nRetc = send_mail(TO_LINKSW, ".", que);
	if (0 != nRetc) syslog(SYSNO_MAIL_SNED_ERR, "[�J���������N�X�C�b�`�ʐM�ւ̃J�����I��ݒ� err_code=%d]", nRetc);
}

//------------------------------------------
// �J�����I��ݒ�f�[�^����
// int SwitchID �X�C�b�`FPGA�{�[�hID (0�I���W��)
// int BoardNo �t���[���O���o�[�{�[�hNo (1�`15)
// int CamNo �J����No (1�`8�A0:�o�͒�~)
// int nKind Master�^Slave�ݒ�i1�FMaster�A-1�FSlave�j
// COMMON_QUE *que ���[���X���b�g�ʐM�p�\����
//------------------------------------------
void ToOutMailSender::Create_ToLinksw_ChangeCam(int SwitchID, int BoardNo, int CamNo, int nKind, COMMON_QUE *que)
{
	que->nEventNo = FACT_TO_LINKSW_03;
	que->nLineNo = getlineid();
	que->fl.data[0] = SwitchID;
	que->fl.data[1] = BoardNo;
	que->fl.data[2] = CamNo;
	que->fl.data[3] = nKind;
}

//------------------------------------------
// �J���������N�X�C�b�`�ʐM �� �r���Q���ݒ�ʒm
// int ou �\���敪(0:�\ 1:��)
// int JoinFrameNo �r���Q���t���[��No(0�I���W��)
//------------------------------------------
void ToOutMailSender::Send_ToLinksw_JoinReq(int ou, int JoinFrameNo)
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));

	que.nEventNo		= FACT_TO_LINKSW_04;
	que.nLineNo			= getlineid();
	que.fl.data[0]		= ou;
	que.fl.data[1]		= JoinFrameNo;

	// ���M
	int nRetc = send_mail(TO_LINKSW, ".", &que);
	if( 0 != nRetc ) syslog(SYSNO_MAIL_SNED_ERR, "[�J���������N�X�C�b�`�ʐM�ւ̓r���Q���ݒ�ʒm err_code=%d]", nRetc);
}

//------------------------------------------
// �J���������N�X�C�b�`�ʐM �� �r���Q���ݒ�ʒm
// COMMON_QUE *que ���[���X���b�g�ʐM�p�\����
//------------------------------------------
void ToOutMailSender::Send_ToLinksw_JoinReq(COMMON_QUE *que)
{
	// ���M
	int nRetc = send_mail(TO_LINKSW, ".", que);
	if (0 != nRetc) syslog(SYSNO_MAIL_SNED_ERR, "[�J���������N�X�C�b�`�ʐM�ւ̓r���Q���ݒ�ʒm err_code=%d]", nRetc);
}

//------------------------------------------
// �J���������N�X�C�b�`�ʐM �� �r���Q���ݒ�ʒm
// int ou �\���敪(0:�\ 1:�� 2�F���W�b�N�p�������P 3�F���W�b�N�p������2)
// int JoinFrameNo �r���Q���t���[��No(0�I���W��)
// COMMON_QUE *que ���[���X���b�g�ʐM�p�\����
//------------------------------------------
void ToOutMailSender::Create_ToLinksw_JoinReq(int ou, int JoinFrameNo, COMMON_QUE *que)
{
	que->nEventNo		= FACT_TO_LINKSW_04;
	que->nLineNo		= getlineid();
	que->fl.data[0]		= ou;
	que->fl.data[1]		= JoinFrameNo;
}

//------------------------------------------
// �J���������N�X�C�b�`�ʐM �� �V���A���ʐM���x�ύX�v��
// int nBoardID	�t���[���O���o�[�{�[�hID(1�I���W��)
// int nSpeed	�V���A���ʐM���x[bps]
//------------------------------------------
void ToOutMailSender::Send_ToLinksw_ChangeLinkSpeed(int nBoardID, int nSpeed)
{
	COMMON_QUE que;

	que.nEventNo	= FACT_TO_LINKSW_06;
	que.nLineNo		= getlineid();
	que.fl.data[0]	= nBoardID;
	que.fl.data[1]	= nSpeed;
	
	// ���M
	int nRetc = send_mail(TO_LINKSW, ".", &que);
	if( 0 != nRetc ) syslog(SYSNO_MAIL_SNED_ERR, "[�J���������N�X�C�b�`�ʐM�ւ̃V���A���ʐM���x�ύX�ʒm err_code=%d]", nRetc);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �Ɩ����u

#ifndef KT_LEDMGR
//------------------------------------------
// ���^�n���Ɩ����u�� �p�����[�^�ύX��ʒm
// int scode �����\�ʏ��No
//------------------------------------------
void ToOutMailSender::Send_ToLights_Param(int scode)
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_TO_LIGHTS_01;
	que.nLineNo = getlineid();
	que.fl.data[0] = scode;
	// ���M
	int iRet = send_mail(TO_LIGHTS, ".", &que);
	if( 0 != iRet ) syslog(SYSNO_MAIL_SNED_ERR, "[���^�n���Ɩ����u�փp�����[�^�ύX�v�� err_code=%d]", iRet);
}

//------------------------------------------
// ���^�n���Ɩ����u�� �Y���R�C���ɍœK�Ȑݒ��ʒm����
// int TorB �\:0 ��:1
// int scode �����\�ʏ�� (1�I���W��)
//------------------------------------------
void ToOutMailSender::Send_ToLights_CoilChange(int TorB, int scode)
{
	COMMON_QUE que;
	que.nEventNo = FACT_TO_LIGHTS_02;
	que.nLineNo = getlineid();
	que.fl.data[0] = scode;
	que.fl.data[1] = TorB;

	// ���M
	int iRet = send_mail(TO_LIGHTS, ".", &que);
	if( 0 != iRet ) syslog(SYSNO_MAIL_SNED_ERR, "[���^�n���Ɩ����u�փR�C���ؑ֒ʒm err_code=%d]", iRet);
}

#else

//------------------------------------------
// LED�Ɩ����u�� �p�����[�^�ύX��ʒm
// int scode �����\�ʏ��No
//------------------------------------------
void ToOutMailSender::Send_KtLedMgr_Param(int scode)
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_KT_LEDMGR_01;
	que.nLineNo = getlineid();
	que.fl.data[0] = scode;
	// ���M
	int iRet = send_mail(KT_LEDMGR, ".", &que);
	if( 0 != iRet ) syslog(SYSNO_MAIL_SNED_ERR, "[LED�Ɩ����u�փp�����[�^�ύX�v�� err_code=%d]", iRet);
}

//------------------------------------------
// LED�Ɩ����u�� �Y���R�C���ɍœK�Ȑݒ��ʒm����
// int TorB �\:0 ��:1
// int scode �����\�ʏ�� (1�I���W��)
// int ch  CH(0:�S��  1�`:�Ώ�CH�̂�)
//------------------------------------------
void ToOutMailSender::Send_KtLedMgr_CoilChange(int TorB, int scode, int ch)
{
	COMMON_QUE que;
	que.nEventNo = FACT_KT_LEDMGR_02;
	que.nLineNo = getlineid();
	que.fl.data[0] = TorB;			// �������t�Ȃ̂Œ���
	que.fl.data[1] = ch;
	que.fl.data[2] = scode;

	// ���M
	int iRet = send_mail(KT_LEDMGR, ".", &que);
	if( 0 != iRet ) syslog(SYSNO_MAIL_SNED_ERR, "[LED�Ɩ����u�փR�C���ؑ֒ʒm err_code=%d]", iRet);
}
//------------------------------------------
// LED�Ɩ����u�� �œK�ȋP�x�ݒ��ʒm����
// int TorB �\:0 ��:1
// int valM ���ʒl (�S�̒����f�[�^  0�`255)
// int valH ���ʒl (�z�������f�[�^  0�`255)
// int ch  CH(0:�S��  1�`:�Ώ�CH�̂�)
//------------------------------------------
void ToOutMailSender::Send_KtLedMgr_ValChange(int TorB, int valM, int valH, int ch)
{
	COMMON_QUE que;
	que.nEventNo = FACT_KT_LEDMGR_03;
	que.nLineNo = getlineid();
	que.fl.data[0] = TorB;		
	que.fl.data[1] = ch;
	que.fl.data[2] = valM;
	que.fl.data[3] = valH;

	// ���M
	int iRet = send_mail(KT_LEDMGR, ".", &que);
	if( 0 != iRet ) syslog(SYSNO_MAIL_SNED_ERR, "[LED�Ɩ����u�֋P�x�ݒ� err_code=%d]", iRet);
}

//------------------------------------------
// LED�Ɩ����u�ʐM�� �_���E�����w��
// int TorB �\:0 ��:1
// bool onoff �_�������敪
// int ch  CH(0:�S��  1�`:�Ώ�CH�̂�)
// �߂�l ���A���
//------------------------------------------
void ToOutMailSender::Send_KtLedMgr_OnOff(int TorB, BOOL onoff, int ch)
{
	COMMON_QUE que;
	que.nEventNo = FACT_KT_LEDMGR_04;
	que.nLineNo = getlineid();
	que.fl.data[0] = TorB;
	que.fl.data[1] = ch;
	que.fl.data[2] = onoff;
	// ���M
	int iRet = send_mail(KT_LEDMGR, ".", &que);
  #ifndef LOCAL
	if( 0 != iRet ) syslog(SYSNO_MAIL_SNED_ERR, "[LED�Ɩ����u�֓_���E�����w�� err_code=%d]", iRet);
  #endif
}
#endif