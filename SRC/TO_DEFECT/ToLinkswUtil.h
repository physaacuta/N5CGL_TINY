// *********************************************************************************
//	�J���������N�X�C�b�`�ݒ胆�[�e�B���e�B
//	[����]
//		�e�C���X�^���X�́Astatic�� .cpp�Ɏ�����
// *********************************************************************************

#pragma once

// ���Œ�
#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ���O�Ǘ��N���X

// ���[�J��
#include "MainConst.h"
#include "StatusManager.h"												// �X�e�[�^�X������N���X

using namespace KizuLib;

class ToLinkswUtil
{
public:
	ToLinkswUtil(LogFileManager* pCls1, StatusManager* pCls2);
	~ToLinkswUtil();

public:
	//=========================================
	// �O���󂯓n�����
	struct LINKSW_OUTPUT_INFO
	{
		int nCamPort;												// �J�����|�[�g (1�I���W��) 0:�o�͒�~
		int nFPGABoardNo;											// FPGA�{�[�hNo (0�`7)
		int nFPGACh;												// FPGA�{�[�hCh (1�`15)
		int nLinkswNo;												// �����N�X�C�b�`No (0�`3)
		int nCamset;												// �J�����Z�b�g (1�I���W��) 0:�o�͒�~
	};
	struct TO_LINKSW_OUTPUT
	{
		LINKSW_OUTPUT_INFO	output[NUM_ALL_HT][NUM_CAMLINK_BOARD];	// [PC�䐔][PC1�䓖����̐ڑ�ch��]
	};
	
///////////////////////////////////////////////////////////////////////////////////////////////////////
//// �����o�[�萔
private:

	//=========================================
	// ���O�p
	static LogFileManager*		mcls_pLog;											// LogFileManager�X���b�h�C���X�^���X
	static CString				my_sThreadName;										// ���X���b�h��

	static StatusManager*		mcls_pStatus;										// �X�e�[�^�X������N���X

public:

///////////////////////////////////////////////////////////////////////////////////////////////////////
//// �v���p�e�B
public:

///////////////////////////////////////////////////////////////////////////////////////////////////////
//// �����֐�
private:

///////////////////////////////////////////////////////////////////////////////////////////////////////
//// ���J�֐�
public:
	static void GetFPGAInfo(int pcid, int camno, int *pFPGABoardNo, int *pFPGACh);
	static void GetOutputInfo(StatusManager *pStatus, TO_LINKSW_OUTPUT *pOutputInfo);
	static void GetDeterminedHanteiPC(StatusManager *pStatus, TO_LINKSW_OUTPUT *pInfo);
	static int GetTargetCamNo(const TO_LINKSW_OUTPUT *pInfo, long nFPGABoardNo, long nCamno);
	static int GetHanteiGroup(int nMen, int nHantei, int nCam);

///////////////////////////////////////////////////////////////////////////////////////////////////////
//// �����o�[ �ϐ�
private:
};

