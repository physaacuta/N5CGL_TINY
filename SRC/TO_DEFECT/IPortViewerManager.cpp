#include "StdAfx.h"
#include "IPortViewerManager.h"
#include "ToLinkswUtil.h"

#include "MainInstance.h"


// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
IPortViewerManager::IPortViewerManager(void) :
ThreadManager("iPortViewerMgr")
{

}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
IPortViewerManager::~IPortViewerManager(void)
{

}

//------------------------------------------
// �X�g�b�v
//------------------------------------------
void IPortViewerManager::Stop()
{
	// ���X���b�h�ɏI���v��
	COMMON_QUE que;
	que.nEventNo = FACT_KS_END;
	que.nLineNo = getlineid();
	send_mail(EPC_MAIL_CAMCONT, ".", &que);
}


//------------------------------------------
// �X���b�h���C�����[�v
//------------------------------------------
int IPortViewerManager::ThreadMainLoop()
{
	COMMON_QUE que;			// ��M�\����


	////////////////////////////////////
	// ���O����
	////////////////////////////////////
	// ���[���X���b�g�N���X
	MailSlotManager		cls_Mail;								
	bool bRetc = cls_Mail.Create(EPC_MAIL_CAMCONT);
	_ASSERT(bRetc);



	////////////////////////////////////
	// �ʏ�̃��C���X���b�h����
	////////////////////////////////////
	//// �N���v����荞�� ���[�v�J�n
	LOG(em_MSG), "[%s] �X���b�h�J�n [0x%X]", my_sThreadName, GetTid());
	while(true) {
		cls_Mail.Recv(&que, SIZE_MSL);		// �N���v����荞��

		switch(que.nEventNo) {

//-----------------------------------------------------------------------------------------------
		case FACT_EPC_IPORTVIEWER:							// iPortViewer����̃��[���X���b�g (��iPortViewer)
			LOG(em_MSG), "[MAIL] iPortViewer����ʒm [cmd=%d]", que.nLineNo);

			// ��������
			switch( que.nLineNo ) {
			case 0:		// �\�Z��
				ToOutMailSender::Send_ToCamera_Palse(false);
				ToOutMailSender::Send_ToCamera_Start(DIV_PLG_TOP);
				Sleep(100);
				ToOutMailSender::Send_ToCamera_Palse(true);
				break;
			case 1:		// ���Z��
				ToOutMailSender::Send_ToCamera_Palse(false);
				ToOutMailSender::Send_ToCamera_Start(DIV_PLG_BOT);
				Sleep(100);
				ToOutMailSender::Send_ToCamera_Palse(true);
				break;
			case 2:		// �e�X�g�p���X
				ToOutMailSender::Send_ToCamera_Palse(false);
				ToOutMailSender::Send_ToCamera_Start(DIV_PLG_TEST);
				Sleep(100);
				ToOutMailSender::Send_ToCamera_Palse(true);
				break;
			case 3:		// �I�����C��
				ToOutMailSender::Send_ToCamera_Palse(false);
				ToOutMailSender::Send_ToCamera_Start(DIV_PLG_LINE);
				Sleep(100);
				ToOutMailSender::Send_ToCamera_Palse(true);
				break;
			case 100:	// ��~
				ToOutMailSender::Send_ToCamera_Palse(false);
				break;

			
			case 1000:	// LED�Ɩ����ʐݒ�
				SetLightVal(que.fc.data[0], que.fc.data[1], que.fc.data[2]);
				break;
			case 1001:	// LED�Ɩ�onoff�ݒ�
				SetLightOnOff(que.fc.data[0], que.fc.data[1], que.fc.data[2]);
				break;
			case 1003:	// �I�����Ԑݒ�
				if(true){
					int nWk;
					memcpy(&nWk, &que.fc.data[1], sizeof(int));
					SetAGC(que.fc.data[0], nWk);
				}
				break;

			//// �J���������N�X�C�b�` ------------------------------------------->>>
			case 1002:	// �J�����ؑ֐ݒ�
				SetCameraChange(que.fc.data[0], que.fc.data[1]);
				break;

			// <<<------------------------------------------------------
			}
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_END:									// �I���v��
			// �e�v���Z�X�I������
			LOG(em_MSG), "[%s] �X���b�h�I�� [0x%X]", my_sThreadName, GetTid());
			return 0;

		default:
			LOG(em_MSG), "[MAIL] �C�x���gNo���� [IPortViewer EventNo=%d]", que.nEventNo);
			syslog(SYSNO_MAILNO_ERR, "[IPortViewer EventNo=%d]", que.nEventNo);
			break;
		}
	}

	return 0;
}

//------------------------------------------
// �F���pLED�Ɩ����ʐݒ�
// int camset	�J�����Z�b�g (1�I���W��)
// int index	����F0�Œ�(���g�p) / �F���F�F (0:R  1:G  2:B)
// int val      �P�x (0�`255)
//------------------------------------------
void IPortViewerManager::SetLightVal(int camset, int index, BYTE val)
{
	int ou;		// �\���̔��f�͐F��OR����ňقȂ�
	int ch = index;


#ifdef HT_IS_COLOR		// �F��

	ou = NUM_TorB( (camset-1) );

	LOG(em_MSG), "[MAIL] iPortViewer����ʒm (�Ɩ�����) [camset=%d] [�F=%d] [�l=%d]", camset, index, val);

	ToOutMailSender::Send_KtLedMgr_ValChange(ou, val, 255, ch+1);

#else					// ����

	ou = (camset-1) / (NUM_CAMPAIR / NUM_MEN);

	LOG(em_MSG), "[MAIL] iPortViewer����ʒm (�Ɩ�����) [camset=%d] [�l=%d]", camset, val);

	ToOutMailSender::Send_KtLedMgr_ValChange(ou, val, 255, 0);	// �S�F����

#endif


	// DB�̓o�^�͂Ȃ�
	//CString sql;
	//// ��
	//char cNameM[2][3][64] = {{"�Ɩ��Ɠx_�\", "�Ɩ��Ɠx2_�\", "�Ɩ��Ɠx3_�\"}, {"�Ɩ��Ɠx_��", "�Ɩ��Ɠx2_��", "�Ɩ��Ɠx3_��"} };
	//char cNameH[2][3][64] = {{"�Ɩ��Ɠx�␳�l_�\", "�Ɩ��Ɠx�␳�l2_�\", "�Ɩ��Ɠx�␳�l3_�\"}, {"�Ɩ��Ɠx�␳�l_��", "�Ɩ��Ɠx�␳�l2_��", "�Ɩ��Ɠx�␳�l3_��"} };


	////======================================================
	////// ODBC�ڑ��N���X �C���X�^���X����
	//OdbcBase clsDB;
	//if( ! clsDB.Connect(TASKINI_NAME)) {
	//	// �f�[�^�x�[�X�ڑ��G���[
	//	LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
	//	syslog(SYSNO_DB_CONNECT_ERR, "");
	//	return ;
	//}


	////// SQL����
	//sql = "UPDATE ";
	//sql += DB_PARAM_SCODE;
	//sql += " SET ";	

	//// Where
	//sql += " WHERE ";
	//CreateSQL::sqlColumnUpdate(&sql, "camangle", jj+1);			// �J�����p�x(1�I���W��) 
	//CreateSQL::sqlColumnTrim(&sql);		// �Ō�̃J���}����菜��
	//

	////// ���s
	//if( ! clsDB.ExecuteDirect(sql) ) {
	//	LOG(em_ERR), "[%s] �ˉe�p�����[�^���A�b�v�f�[�g���s [%s] [%s]", my_sThreadName, DB_PARAM_CAMSET_ANGLE, sql);
	//	syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[�ˉe�p�����[�^���A�b�v�f�[�g���s:%s]", DB_PARAM_CAMSET_ANGLE);
	//	return false;
	//}
}


//------------------------------------------
// �����pLED�Ɩ��_�������ݒ�
// int cam		�F���̏ꍇ�J�����Z�b�g (1�I���W��)/����̏ꍇ�J�����y�A(1�I���W��)
// int index	����F0�Œ�(���g�p) / �F���F�F (0:R  1:G  2:B)
// int onoff	1:on 0:off
//------------------------------------------
void IPortViewerManager::SetLightOnOff(int cam, int index, int onoff)
{
	int ou;		// �\���̔��f�͐F��OR����ňقȂ�


	int ch = index;

#ifdef HT_IS_COLOR

	ou = NUM_TorB( (cam-1) );

	LOG(em_MSG), "[MAIL] iPortViewer����ʒm (�Ɩ�%s) [camset=%d] [�F=%d] [�l=%d]", (1==onoff?"ON":"OFF"), cam, index, onoff);

	ToOutMailSender::Send_KtLedMgr_OnOff(ou, (BOOL)onoff, ch+1);

#else

	ou = (cam-1) / (NUM_CAMPAIR / NUM_MEN);

	LOG(em_MSG), "[MAIL] iPortViewer����ʒm (�Ɩ�%s) [campair=%d] [�l=%d]", (1==onoff?"ON":"OFF"), cam, onoff);

	ToOutMailSender::Send_KtLedMgr_OnOff(ou, (BOOL)onoff, 0);

#endif




}

//------------------------------------------
// �J�����ؑ֐ݒ�
// int hantei	�e����PC�̌ŗL�ԍ� (1�I���W��)
// int cam		�F���̏ꍇ�J�����Z�b�g (1�I���W��)/����̏ꍇ�J�����y�A(1�I���W��)
//------------------------------------------
void IPortViewerManager::SetCameraChange(int hantei, int cam)
{
#ifdef HT_IS_COLOR		// �F��

	CString			sWk;

	LOG(em_MSG), "[MAIL] iPortViewer����ʒm (�J�����ؑ�) [hantei=%d] [camset=%d]", hantei, cam);

	// �t���[���O���o�[�{�[�hNo
	int nFPGABoardNo= 0;
	int nFPGACh		= 0;
	ToLinkswUtil::GetFPGAInfo(hantei, 1, &nFPGABoardNo, &nFPGACh);

	// Board 0 - 7
	if (0 > nFPGABoardNo || (NUM_CAMLINKSW * MAX_CAMLINKSW_FPGA) <= nFPGABoardNo)
	{
		LOG(em_ERR), "[%s] �J���������ؑ� �t���[���O���o�[�{�[�hNo �ُ� [%s]", my_sThreadName, sWk);
		return;
	}
	// Ch 1 - 15
	if (0 >= nFPGACh || MAX_CAMLINKSW_OUTPUT < nFPGACh)
	{
		LOG(em_ERR), "[%s] �J���������ؑ� �t���[���O���o�[�{�[�hCh �ُ� [%s]", my_sThreadName, sWk);
		return;
	}

	// �J����No
	ToLinkswUtil::TO_LINKSW_OUTPUT info;
	memset(&info, 0x00, sizeof(info));
	ToLinkswUtil::GetDeterminedHanteiPC(mcls_pStatus, &info);

	// �ΏۂƂȂ������FPGA�ԍ����擾
	int nFPGABoard[MAX_CAMLINKSW_FPGA];
	memset(&nFPGABoard, 0x00, sizeof(nFPGABoard));
	nFPGABoard[0] = (nFPGABoardNo / MAX_CAMLINKSW_FPGA) * MAX_CAMLINKSW_FPGA;
	nFPGABoard[1] = nFPGABoard[0] + 1;

	LOG(em_MSG), "[%s] �J���������ؑ� [FPGA:%d/%d]", my_sThreadName, nFPGABoard[0], nFPGABoard[1]);

	int nCamNo = -1;
	for (int ii = 0; ii < MAX_CAMLINKSW_FPGA; ii++)
	{
		int nWk = ToLinkswUtil::GetTargetCamNo(&info, nFPGABoard[ii], cam);
		if (-1 != nWk)
		{
			// �ڑ����Ă���J������I������
			nCamNo = nWk;
			break;
		}
	}
	if (-1 != nCamNo)
	{
		LOG(em_MSG), "[%s] �J���������ؑ� [�w��J����No:%d �o�̓J����No:%d]", my_sThreadName, cam, nCamNo);
	}
	else
	{
		LOG(em_ERR), "[%s] �J���������ؑ� �J����No �ُ� [�w��J����No:%d]", my_sThreadName, cam);

		//// �J���������N�X�C�b�`�ʐM �� �J�����I��ݒ�i�o�͒�~�j
		ToOutMailSender::Send_ToLinksw_ChangeCam(nFPGABoardNo, nFPGACh, 0);
		return;
	}

	//// �J���������N�X�C�b�`�ʐM �� �J�����I��ݒ�
	ToOutMailSender::Send_ToLinksw_ChangeCam(nFPGABoardNo, nFPGACh, nCamNo);

#else					// ����

	CString			sWk;

	LOG(em_MSG), "[MAIL] iPortViewer����ʒm (�J�����ؑ�) [hantei=%d] [campair=%d]", hantei, cam);

	// �t���[���O���o�[�{�[�hNo_��
	sWk.Format("CAMLINKSW_PCID%d_1", hantei);
	int nBoardNo_S = GetPrivateProfileInt(TO_DEFECT, sWk, 0, TASKINI_NAME);
	
	// �t���[���O���o�[�{�[�hNo_��
	sWk.Format("CAMLINKSW_PCID%d_2", hantei);
	int nBoardNo_R = GetPrivateProfileInt(TO_DEFECT, sWk, 0, TASKINI_NAME);

	if( 0 >= nBoardNo_S || MAX_CAMLINKSW_OUTPUT*MAX_CAMLINKSW_FPGA < nBoardNo_S ||
		0 >= nBoardNo_R || MAX_CAMLINKSW_OUTPUT*MAX_CAMLINKSW_FPGA < nBoardNo_R)
	{
		LOG(em_ERR), "[%s] �J���������ؑ� �t���[���O���o�[�{�[�hNo �ُ� [%s]", my_sThreadName, sWk);
		return;
	}

	// �J����No
	int nCamNo_S = ((cam - 1) * MAX_PHYSICAL_CAMANGLE) + 1;
	int nCamNo_R = ((cam - 1) * MAX_PHYSICAL_CAMANGLE) + 2;
	if( 0 >= nCamNo_S || NUM_CAMERA < nCamNo_S || 0 >= nCamNo_R || NUM_CAMERA < nCamNo_R )
	{
		LOG(em_ERR), "[%s] �J���������ؑ� �J����No �ُ� [���F%d/���F%d]", my_sThreadName, nCamNo_S, nCamNo_R);
		return;
	}

	//// �J���������N�X�C�b�`�ʐM �� �J�����I��ݒ�
	ToOutMailSender::Send_ToLinksw_ChangeCam( nBoardNo_S / MAX_CAMLINKSW_OUTPUT, nBoardNo_S, nCamNo_S);
	ToOutMailSender::Send_ToLinksw_ChangeCam( nBoardNo_R / MAX_CAMLINKSW_OUTPUT, nBoardNo_R, nCamNo_R);
#endif

}

//------------------------------------------
// AGC�ݒ�
// int camset	�J�����Z�b�g (1�I���W��)
// int ro		�I������	 (��Βl)
//------------------------------------------
void IPortViewerManager::SetAGC(int camset, int ro)
{
	LOG(em_MSG), "[MAIL] iPortViewer����ʒm (�I�����ԕύX) [camset=%d] [ExpTime=%d]", camset, ro);

	ToOutMailSender::Send_ToCamera_AGC(camset-1, 1, ro);

	
}