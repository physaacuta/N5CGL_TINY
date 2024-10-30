// *********************************************************************************
//	���^�X�N�̊�{�萔
//	[Ver]
//		Ver.01    2007/08/01  vs2005 �Ή�
//
//	[����]
//		
// *********************************************************************************

#pragma once


// ��{�I�ɂǂ��ł��g�p����\��������C���N���[�h
#include "..\..\Include\KizuCommon.h"								// �r�����ʃw�b�_�[
#include "..\..\Library\KizuLibMFC\CLASS\ThreadLoopQueue.h"			// ��]���i�`���̃X���b�h�L���[
#include "..\..\Include\OutIF_HT_RAW.h"								// �S���摜�ۑ� �O��I/F�p�w�b�_�[
#include "ProcessQueImpl.h"

//=======================================
// �萔

//// �� (����l�݂����Ȃ���)
#define MAX_INS						4								// ��{�捞�������s����
#define	MAX_THREAD_MULTI_P			32								// �����X���b�h�ő吔

#define MAX_FOLDER_SAVE_NUM			10								// �t�@�C���ۑ��t�H���_�ő吔
#define MAX_COLOR_NUM				4								// 1�J�������琶�������ő�摜�� (���̂Ƃ���3�摜 G,B,G-B)
#define MAX_OVERLAP					100//999						// �ő�I�[�o�[���b�v�w�� (�I�[�o�[���b�v����������Ȃɑ傫���������Ȃ�����)
#define MAX_RESYNC_FNO				100//20								// �ē������邽�߂̃t���[���҂���

//// ������
#define	LOOP_QUE_SOCKRECV_SIZE		16								// GRAB�̃L���[�T�C�Y
#define MAX_BUF_FRAME				30 								// �������̃t���[���Y���ꎞ�o�b�t�@�����O�ő吔
#define MAX_FOLDER_SAVE_NUM			10								// �t�@�C���ۑ��t�H���_�ő吔


#define MAX_COIL_BUFRING_NUM		12  							// �o�b�`���C�����̃R�C��FT����p�o�b�t�@�����O�ő吔


//// ����
#define TIME_DISP_REFRESH			500								// ��ʍX�V���� [ms]

// ���̑�
#define RPI_TYPE					"rpi"							// �g���q (����)
#define RPI_TYPE_WK					"rwk"							// �g���q (�����ݒ�)
#define KIDO_SEC					"KIDO"							// �P�x�Z�N�V����

//// ���J�}�N��
#define GET_SIZE_JUST(x, l)		((((x-1)/l)+1)*l)					// x�T�C�Y��l�̔{���ɑ�����(�K���Ax�̂܂܂��A������Ƒ傫���Ȃ����)

//// FT�����҂�
#define FT_RETRY_CNT				100								// FT_RETRY_CNT * FT_RETRY_SLEEP �����ő�҂�
#define FT_RETRY_SLEEP				200								// ms


//=======================================
// �萔
//// �摜���k�^�C�v
enum EM_COMP_TYPE
{		
	COMP_TYPE_JPEG = 0,												// JPEG���k
	COMP_TYPE_LAC,													// LAC���k (���x���k�Z�p)
	COMP_TYPE_RAW,													// ���摜 (BMPͯ�ް��)
	COMP_TYPE_BMP,													// ���摜 (BMPͯ�ް�L)
	COMP_TYPE_NUC,													// NUC���k (���ǔō��x���k�Z�p)
	COMP_TYPE_LAC5													// LAC5���k (GPU�ō��x���k�Z�p)
};
//// �摜�^�C�v
enum EM_COLOR_TYPE
{
	COLOR_TYPE_GRAY = 0,											// �O���[�摜	(�t���[�������1�摜)
	COLOR_TYPE_COLOR,												// �J���[�摜	(�t���[�������RGB��3�摜)
	COLOR_TYPE_SA													// �F���摜		(�t���[�������RGB��3�摜�B�摜�������RGB,R-B��4�摜)
};
//// �r���^�C�v
enum EM_SYS_TYPE
{
	SYS_TYPE_LINE = 0,												// �A�����C��
	SYS_TYPE_BAT													// �o�b�`���C��
};


//=======================================
// ��M�\����

//-----------------------------
//// �A����̂P�t���[�����̏��
struct FRAME_DATA_COLOR {
	bool					bOk;									// �L���f�[�^��true (Union�ŃZ�b�g�BChain�Ŋm�F)
	void*					pFrameDataMgr[MAX_COLOR_NUM];			// �A����̃t���[����� (�z���g�́AFrameDataManager �̕����悩����)
										// �F���F(Union��3�AColor��1�� �Z�b�g) (�F���̏ꍇ,1��FRAME_DATA����ő�4��FrameDataManager�����o����邽��)
										// �P�x�F��ɂP����Union�ŃZ�b�g
};

//-----------------------------
//// Coil��ImgUnion��Color��imgBuf�Ɏ�n���̈�
struct FRAME_DATA_ARRAY {
	int						nIns;									// �C���X�^���XNo (0�I���W��)
	int						nNum;									// �Z�b�g��
	DWORD					nFno;									// �t���[��No

	int						nIdx;									// 1�������ꂽ�ǂ��ꍇ�̃��R�[�h�B�g���₷���悤
	bool					nIsOk[NUM_CAM_POS];						// ���������̗L���Z�b�g
	HT_RPI_FDATA*			pData[NUM_CAM_POS];						// ���������̃t���[�����  (PlgSync�ŃZ�b�g)

	FRAME_DATA_COLOR		Fdc;									// �A����̃t���[����� (Union�ŃZ�b�g)			

	
	// �R�C���ؑ֔���F�� (CoilControlProc�ŃZ�b�g)
	// ��{
	char					cKanriNo[SIZE_KIZUKEN_NO];				// �Ǘ�No
	char					cCoilNo[32];							// �R�C��No
	int						mmFromHeadAtCoilStart;					// �R�C���擪���_�ł̗n�ړ_����̋���[mm]
	DWORD					nCFno;									// �R�C���擪����̃t���[��No (0�I���W��)
	EM_COMP_TYPE			emCompType;								// �摜���k�^�C�v
	int						nCompQuality;							// ���k�� (0�`100%)
	bool					bDummyCoil;								// true:�_�~�[�R�C��


	// ��
	EM_IMG_SAVE_STAT		emImgSaveStat;							// �摜��ԔF�� (�o�b�`���C���̏ꍇ�́ACoilControl�ł��㏑��)
	int						nEdgeIdx[2];							// �t���[���摜�A���㍶�[����̃G�b�W�ʒu [pixel]
	float					nEdgePos[2];							// [mm]
	//int						nFTset;									// ���[�ʒu�Z�b�g��  (��[1  ��[2 ����0)
	//long					nFTposCfNo;								// ���[�ʒu�t���[��No (���̃t�@�C���擪����̃t���[��No (0�I���W��))
	//long					nFTposPixel;							// ���[�ʒu�t���[����pixel (���̃t���[����[����̃s�N�Z���ʒu (0�I���W��))


	__int64					nTimePlgSyncEnd;						// Plg���R�t���������� 
};

//-----------------------------
//// RPI�t�@�C���̈ړ�
struct RPI_MOVE {
	CString					sPath_F;								// �ړ����t�H���_�p�X (�Ō��\�͖���)
//	CString					sPath_T;								// �ړ���t�H���_�p�X (�Ō��\�͖���)
	CString					sName;									// RPI�t�@�C����
	int						nIns;									// �C���X�^���XNo (0�I���W��)
};


//-----------------------------
// RpiWriteProc��
// �󂯓n���\���̂̔ԍ� 
enum ENUM_KUBUN_RPIWRITE {
	em_FRAME_INFO = 0,												// �t���[�����ǉ�
	em_WRITE_END													// �I���w��
};
// �󂯓n���A�C�e��
struct DELIVERY_RPIWRITE {
	ENUM_KUBUN_RPIWRITE	emNo;										// �ǂ̍\���̂�
	BYTE*				data;										// �\���̂̃|�C���^ (NULL�͂��蓾��)
};


//=======================================
// �ȒP�ȋ��ʊ֐�
static CString GetThreadNameArray(char* c, int nNo) {
	CString s;
	s.Format("%s_%d", c, nNo);
	return s;
}
static CString GetThreadNameArray2(char* c, int nNo, int nNo2) {
	CString s;
	s.Format("%s_%d_%d", c, nNo, nNo2);
	return s;
}

// �����P�ʂ̃t�H�[�}�b�g
static CString GetArrayFormat(int const* p, int nCnt=1, char* sprit=",")
{
	CString cWk;
	cWk.Format("%d", p[0]);
	for(int ii=1; ii<nCnt; ii++) {
		cWk.AppendFormat("%s%d", sprit, p[ii]);
	}
	return cWk;
}



//------------------------------------------
// ��M�L���[�p�̉���肵�����ԋp
// ThreadLoopQueue<FRAME_DATA>** pQue �ԋp���̃X���b�h���[�v�L���[�C���X�^���X
// FRAME_DATA_ARRAY* p �L���[���
//------------------------------------------
static void PoolBack_RecvFreeQue(IQueue<HT_RPI_FDATA>* pQue, FRAME_DATA_ARRAY* p)
{
	//// ����肵�Ă��������ԋp
	for(int ii=0; ii<p->nNum; ii++) {
		if( ! p->nIsOk[ii] ) continue; 
		pQue->AddToTail( p->pData[ii] );
	}

	//// �󂯓n���L���[���J��
	SAFE_DELETE( p );
}

static void PoolBack_RecvFreeQue(IQueue<HT_RPI_FDATA>** pQue, FRAME_DATA_ARRAY* p)
{
	int nIns = p->nIns;
	PoolBack_RecvFreeQue(pQue[nIns], p);
}




//=======================================
// ���b�Z�[�W�ʒm
//=======================================
//// �S��MainInctance�Ŏ󂯂�
//       �ꍇ�ɂ���ẮAMainInctance���e���[�J�[�X���b�h�ɂ��ꗬ������
enum ENUM_MAININCTANCE {
	E_DEF_ML_PARAM = WM_USER + 100,									// �p�����[�^�ύX�ʒm
	E_DEF_ML_START,													// �����J�n�v��
	E_DEF_ML_STOP,													// ������~�v��
	E_DEF_ML_NEXTCOIL,												// ���R�C�����

	E_DEF_ML_STATUS,												// ��Ԗ₢���킹

	E_DEF_END														// �I��
};

