#include "stdafx.h"
#include "ToLinkswUtil.h"

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
LogFileManager*					ToLinkswUtil::mcls_pLog = NULL;
CString							ToLinkswUtil::my_sThreadName;

// �X�e�[�^�X������N���X
StatusManager*					ToLinkswUtil::mcls_pStatus = NULL;

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��{

//==========================================
// �R���X�g���N�^
//------------------------------------------
// LogFileManager* pCls1	: ���O�N���X
// StatusManager* pCls2		: �X�e�[�^�X����N���X
//==========================================
ToLinkswUtil::ToLinkswUtil(LogFileManager* pCls1, StatusManager* pCls2)
{
	my_sThreadName = "ToLinkswUtil";

	// ���O�N���X�Z�b�g
	mcls_pLog = pCls1;

	// �X�e�[�^�X����N���X�Z�b�g
	mcls_pStatus = pCls2;
}

//==========================================
// �f�X�g���N�^
//==========================================
ToLinkswUtil::~ToLinkswUtil()
{
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���J�֐�

//==========================================
// �w�蔻��PC��FPGA�����擾����
//------------------------------------------
// int pcid			: PCID
// int camno		: �J����No
// int *pFPGABoardNo: FPGA�{�[�hNo
// int *pFPGACh		: FPGA�`���l��No
//==========================================
void ToLinkswUtil::GetFPGAInfo(int pcid, int camno, int *pFPGABoardNo, int *pFPGACh)
{
	CString sWk;

	sWk.Format("CAMLINKSW_PCID%d_%d", pcid, camno);
	int nWk = GetPrivateProfileInt(TO_DEFECT, sWk, 0, TASKINI_NAME);
	*pFPGABoardNo	= nWk / 100;
	*pFPGACh		= nWk % 100;
}

//==========================================
// �J���������N�X�C�b�` �o�͏����擾����
//------------------------------------------
// StatusManager *pStatus	: �X�e�[�^�X���
// TO_LINKSW_OUTPUT *pInfo	: �J���������N�X�C�b�`���
//==========================================
void ToLinkswUtil::GetOutputInfo(StatusManager *pStatus, TO_LINKSW_OUTPUT *pInfo)
{
	CString sWk;

	for (int ii = 0; ii<NUM_ALL_HT; ii++) {
		for (int jj = 0; jj<NUM_CAMLINK_BOARD; jj++) {
			int nFPGABoardNo = 0;
			int nFPGACh		 = 0;

			GetFPGAInfo(ii + 1, jj + 1, &nFPGABoardNo, &nFPGACh);
			pInfo->output[ii][jj].nFPGABoardNo	= nFPGABoardNo;
			pInfo->output[ii][jj].nFPGACh		= nFPGACh;
			pInfo->output[ii][jj].nLinkswNo		= pInfo->output[ii][jj].nFPGABoardNo / 2;
		}
	}

	// ----------------------
	// �ǂ̃{�[�h�ɂǂ̃J�����摜���o�͂��邩����
	// ��FPGA�{�[�hNo����1����̔�
	int nCamset[NUM_CAMLINKSW];		// �J�����Z�b�g�J�E���^�iCameraLinkSwitch���j
	memset(nCamset, 0x00, sizeof(nCamset));
	int nCamNo = 0;
	for (int ii = 0; ii < NUM_MEN; ii++)
	{
		for (int jj = 0; jj < NUM_HANTEI; jj++)
		{
			int pcid = pStatus->GetHtID(ii, jj);
			if (0 >= pcid) continue;
			if (NUM_ALL_HT < pcid) continue;

			for (int kk = 0; kk < NUM_CAMLINK_BOARD; kk++)
			{
				int nCamsetIndex = pInfo->output[pcid - 1][kk].nFPGABoardNo;

				// FPGA�{�[�hNo�̏���`�F�b�N
				if ((NUM_CAMLINKSW * NUM_CAMLINK_BOARD) <= nCamsetIndex) continue;

				// CameraLinkSwitch(FPGA:0,1)���Œʔ�(1�`8)
				nCamsetIndex = nCamsetIndex / 2;

				// ����l�F�W
				// ��������ꍇ�͋����I�ɂW��ݒ�
				nCamset[nCamsetIndex] = (NUM_CAMLINKSW_CAM_MAX <= nCamset[nCamsetIndex] ? NUM_CAMLINKSW_CAM_MAX : nCamset[nCamsetIndex] + 1);
				pInfo->output[pcid - 1][kk].nCamPort= nCamset[nCamsetIndex];
				pInfo->output[pcid - 1][kk].nCamset	= nCamNo + 1;
				nCamNo++;
			}
		}
	}
}

//==========================================
// �ڑ��ς݂̃J���������N�X�C�b�` �o�͏����擾����
//------------------------------------------
// StatusManager *pStatus	: �X�e�[�^�X���
// TO_LINKSW_OUTPUT *pInfo	: �J���������N�X�C�b�`���
//==========================================
void ToLinkswUtil::GetDeterminedHanteiPC(StatusManager *pStatus, TO_LINKSW_OUTPUT *pInfo)
{
	GetOutputInfo(pStatus, pInfo);
		
	// ----------------------
	// ���ڑ��̏��i�o�b�N�A�b�vPC�j���N���A����
	// ��DB�o�^�ΏۊO
	bool bEnablePC[NUM_ALL_HT];
	for (int ii = 0; ii < NUM_ALL_HT; ii++) bEnablePC[ii] = false;
	for (int ii = 0; ii < NUM_MEN; ii++)
	{
		for (int jj = 0; jj < NUM_HANTEI; jj++)
		{
			int pcid = pStatus->GetHtID(ii, jj);
			if (0 >= pcid) continue;
			if (NUM_ALL_HT < pcid) continue;
			bEnablePC[pcid - 1] = true;
		}
	}
	for (int ii = 0; ii < NUM_ALL_HT; ii++)
	{
		if (!bEnablePC[ii])
		{
			for (int jj = 0; jj < NUM_CAMLINK_BOARD; jj++)
			{
				pInfo->output[ii][jj].nFPGABoardNo	= 0;
				pInfo->output[ii][jj].nFPGACh		= 0;
			}
		}
	}
}

//==========================================
// FPGA�ԍ��A�J�����Z�b�g����o�̓|�[�g������o��
//------------------------------------------
// TO_LINKSW_OUTPUT *pInfo	: �J���������N�X�C�b�`���
// long nFPGABoardNo		: FPGA�ԍ�
// long nCamno				: �J�����Z�b�gNo
//==========================================
int ToLinkswUtil::GetTargetCamNo(const TO_LINKSW_OUTPUT *pInfo, long nFPGABoardNo, long nCamno)
{
	for (int ii = 0; ii < NUM_ALL_HT; ii++)
	{
		if (0 == pInfo->output[ii][0].nFPGACh) continue;

		for (int jj = 0; jj < NUM_CAMLINK_BOARD; jj++)
		{
			if (nFPGABoardNo == pInfo->output[ii][jj].nFPGABoardNo &&
				nCamno == pInfo->output[ii][jj].nCamset)
			{
				return pInfo->output[ii][jj].nCamPort;
			}
		}
	}

	return -1;
}

//==========================================
// �ʁA����PCNo����J���������N�X�C�b�`�O���[�v������o��
//------------------------------------------
// int nMen	   : �\��
// int nHantei : ����No
// int nCam    : �J����No
//==========================================
int ToLinkswUtil::GetHanteiGroup(int nMen, int nHantei, int nCam)
{
	CString sWk;
	int nPcID = mcls_pStatus->GetHtID(nMen, nHantei);
	int nGroupID = -1;

	if (0 != nPcID)
	{
		sWk.Format("CAMLINKSW_PCID%d_%d", nPcID, nCam);
		int nWk = GetPrivateProfileInt(TO_DEFECT, sWk, 0, TASKINI_NAME);
		nGroupID = (nWk / 100) / 2;
	}

	return nGroupID;
}