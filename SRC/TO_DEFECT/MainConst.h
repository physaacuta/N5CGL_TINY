// *********************************************************************************
//	���^�X�N�̊�{�萔
//	[Ver]
//		Ver.01    2007/08/01  vs2005 �Ή�
//
//	[����]
//		
// *********************************************************************************

#pragma once

#include "..\..\Include\LineCommon.h"									// �r�����ʃw�b�_�[
#include "..\..\Include\HyTbl.h"										// �\���p���L�������w�b�_�[
#include "TrackingFrame.h"												// FrameNo�Ńg���b�L���O�\�Ƃ���N���X

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �X�C�b�`

//// �d�v�Ȑ؂�ւ��X�C�b�`
//--------
// ���S���p
#define DISABLE_GAP_CALC												// ����Y���␳�@�\������ ���F���̏ꍇ�́A�K����`
#define DISABLE_TINY_CALC												// �������׋@�\������

//--------
// ������ʎd�l
//#define DISABLE_AUTOSTART												// �^�X�N�N�����A�����T���J�n������

//--------
// �����ƍ�����z
//#define DEBUG_TEST													// �e�X�g��
//#define DEBUG_HYPER_V_TEST												// HYPER-V���e�X�g��

//--------
#define ENABLE_GATE_KEN												// �����p�Q�[�g�M��
#define ENABLE_GATE_DSP												// �\���p�Q�[�g�M��

//--------
// �f�o�b�O�m�F�p(�{�Ԏ��A�K���S�Ė����ɂ��邱��)
#define ENABLE_DEBUG_LOG												// ��`���A�e���Ɏd���񂾃f�o�b�O�p�������O���o��
#define	DISABLE_ZZ_EPC													// ��`���A�[���G���W��(ZZ_EPC)���g�p
//#define	DISABLE_LED_DAISYA												// ��`���ALED�_����������ɑ�Ԉʒu�����Ȃ�
#define ENABLE_DEBUG_DLG												// �f�o�b�O�p��DLG�\��

// �f�o�b�O�p���O�o�͐ݒ�
#ifdef ENABLE_DEBUG_LOG
////�����ǉ��\��
	//#define DEBUG_PRINT_SAVEDEFCHK										// ��`���A�r���ۑ��̃t���[������e�����O�o��
	//#define DEBUG_PRINT_SAVEMAXCHK										// ��`���A�r���ۑ��̏���m�F���ʂ����O�o��
	//#define DEBUG_LOG_EDGE_RESTRICT
	//#define DEBUG_LOG_EDGE
	#define DEBUG_LOG_EDGE_UDP
	#define DEBUG_PATO_LOG
	//#define DEBUG_LOG_HT_HEALTHY
#endif

//--------
#define ENABLE_RECORDER_UDP												// �����p���R�[�_�[�ւ̃R�C�����UDP���M�@�������A�R�����g��

#define ADDDBCOUNT_QUE_SIZE_CHANGE										// DB�����݃L���[�T�C�Y��ύX(�L����:200000�A������:5000)

//#define DISABLE_DSP_PLG													// �\��PLG���͖����@���\��PLG���͂���̏ꍇ�A�R�����g��

//--------
// �\�t�g���ʉ��΍�
//#define PLG_DSP_MAP_FIX													// �r�}�b�v�Œ胂�[�h
#define HT_IS_COLOR														// �F��/�P�x�ؑցB  ��`���F�F���@����`���F�P�x
#define RESV_KOTEI														// ���S�Œ蕪��\

#define SENDOP_LENGTH_PLG												// PLG�N���X�ɂĒ�����Œʔʒu��񑗐M�� �A���R�����g

//--------
#define DSP_SOGYO_CHECK												// ���ƊǗ��N���X�ɂĕ\���p�����Ǘ����A���R�����g


//=======================================
// �����p���O��`
// �g�p����ꍇ�́A
//   1:mcls_pLogD �������o�[�ϐ��ɒǉ����Ă��鎖
//   2:�R���X�g���N�^��NULL�������ƃZ�b�g���鎖
// �g�p��:  LOGD(em_MSG), "test_%d_%d", wk, wk );
//=======================================
#define LOGD(t)  if(mcls_pLogD!=NULL)       mcls_pLogD->Write(KizuLib::LogFileManager::t		// ���O�o��
#define pLOGD(t) if(p->mcls_pLogD!=NULL) p->mcls_pLogD->Write(KizuLib::LogFileManager::t		// ���O�o��(�|�C���^��)

#define LOGG(t)  if(mcls_pLogG!=NULL)       mcls_pLogG->Write(KizuLib::LogFileManager::t		// �K�C�_���X�p���O�o��
#define LOGSEQ(t) if(mcls_pLogSeq!=NULL)    mcls_pLogSeq->Write(KizuLib::LogFileManager::t		// �V�[�P���T��M�p���O�o��
#define LOGRN(t) if(mcls_pLogRn!=NULL)   	 mcls_pLogRn->Write(KizuLib::LogFileManager::t		// �r�A�g�p���O�o��
#define LOGRN_ARR(t, id) if(mcls_pLogRn[id-1]!=NULL)   	 mcls_pLogRn[id-1]->Write(KizuLib::LogFileManager::t	// �r�A�g�p���O�o��(�z���)
#define LOGE(t) if(mcls_pLogE!=NULL)    mcls_pLogE->Write(KizuLib::LogFileManager::t	// �G�b�WUDP�p���O�o��

#define LOGA(t)  if(mcls_pLogA!=NULL)       mcls_pLogA->Write(KizuLib::LogFileManager::t		// �x�񃍃O�o��
#define pLOGA(t) if(p->mcls_pLogA!=NULL) p->mcls_pLogA->Write(KizuLib::LogFileManager::t		// �x�񃍃O�o��(�|�C���^��)


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �ŗL�萔

//=======================================
// INI�L�[
//=======================================

#define INI_HT_RECV_TOP		"HT_RECV_TOP"								// �\����@�\�ʐM��` Ini�L�[
#define INI_HT_RECV_BOT		"HT_RECV_BOT"								// ������@�\�ʐM��` Ini�L�[

//#define INI_CY_RECV_TOP		"CY_RECV_TOP"								// �\�����@�\�ʐM��` Ini�L�[
//#define INI_CY_RECV_BOT		"CY_RECV_BOT"								// �������@�\�ʐM��` Ini�L�[


#define INI_OP_SEND			"OP_SEND"									// �I�y���[�^�@�\�ʐM��` Ini�L�[



//=======================================
// �萔��`
//=======================================
//// �ُ�敪
enum EM_ON_ERRWAR {
	// CoilManager
	ON_QUE_COIL_INF = 0,											// �R�C�����o�^���L���[�t��
	ON_QUE_COIL_RESULT,												// �R�C�����ѓo�^���L���[�t��
	ON_QUE_COIL_RESULT_UPDATE,										// �R�C�����эX�V���L���[�t��

	// DBManager
	ON_SAVE_COIL_INF,												// �R�C�����o�^���L���[�t��							
	ON_SAVE_COIL_RESULT,											// �R�C�����ѓo�^���L���[�t��
	ON_SAVE_DEFECT_INF,												// �r���ѓo�^���L���[�t��

	// MainInstance
	ON_QUE_CAMLINKSW,												// �J���������N�X�C�b�`�ݒ�o�^���L���[�t��
	
	// SeqFrameRecv
	ON_SEQ_FRAME_SOCK_CLOSE,										// �ؒf�F�� 

	// PlgManager
	ON_PLG_FRAME_DELETE,											// ��M�t���[������

	// HtRecvBaseManager
	ON_SEQ_FRAME_DELAY,												// �V�[�P���T�t���[����M�x��

	// �O�I�v��
	ON_DB_ACCESS_ERR												// DB�ڑ����s
};

//// GroupID
enum ENUM_GROUP_DIV
{
	DIV_HANTEI01 = 1,												// ����PC1
	DIV_HANTEI02,													// ����PC2
	DIV_HANTEI03,													// ����PC3
	DIV_HANTEI04,													// ����PC4
	DIV_LOGIPARA,													// ���W�b�N�p������PC
	DIV_GROUP_MAX = DIV_LOGIPARA
};
static CString GetHanteiKind(int em)
{
	CString sWk;

	switch (em)
	{
		case DIV_HANTEI01:	sWk = "����O���[�v1";		break;
		case DIV_HANTEI02:	sWk = "����O���[�v2";		break;
		case DIV_HANTEI03:	sWk = "����O���[�v3";		break;
		case DIV_HANTEI04:	sWk = "����O���[�v4";		break;
		case DIV_LOGIPARA:	sWk = "�p�������O���[�v";	break;
		default:			sWk = "�o�b�N�A�b�v";		break;
	}
	return sWk;
}

#define	PLS_RATE_TANI				10000.0						// �p���X���[�g�P��(����V�[�P���T����̓��͂͐����l)


//=======================================
// �r���t�@�C����`
//=======================================
#define	DEF_DUMP_HEAD				"���t,�Ǘ�No,�o���Ǘ�No,�\���敪,�r�A��,�����r��,����O���[�h,�d������,�J�����Z�b�g,����ʒu,�o������ʒu,DS�ʒu,WS�ʒu,DSWS�敪,�r����,�c����\,������\,�t���[��No,�t���[���O��Xmin,�t���[���O��Xmax,�t���[���O��Ymin,�t���[���O��Ymax,�����p�^�[���R�[�h"


//=======================================
// �\����_�ȍ~������P�ʂŃg���b�L���O�Ǘ�
//=======================================
static const int MAX_POS_INDEX = 8;									// �e�ʒu�ł̃g���b�L���O�p

// �����P�ʂ̑����ʒu (�\���ʒu���ɃZ�b�g)
enum EM_POS_DSP {

	//// EM_DIV_DISP �̌ʕ����ƍ��킹�Ă�������

	// �V�X�e���S�̂ł̊Ǘ��Ώ� (MAX_DISP_POS��)
	POS_DSP_P1,														// ������1�ʒu
	POS_DSP_P2,														// ������2�ʒu�i������1�ʒu�Ɠ�����ʂ�\���j
//	POS_DSP_P3,														// ������3�ʒu
//	POS_DSP_P4,														// ������4�ʒu
//	POS_DSP_P5,
	POS_DSP_P_END,													// ������ �ʒu�I�[

	// �����Ǘ�������
	POS_DSP_DSP1_P = POS_DSP_P1,									// [�\���ʒu] �\����_1 ��̊e������J�n�ʒu
	POS_DSP_DSP2_P = POS_DSP_P2,									// [�\���ʒu] �\����_2 ��̊e������J�n�ʒu
	POS_DSP_SHR_I = POS_DSP_P_END,									// �V���[�ʒu

	//----- �����܂Ŕ��Œ�
	POS_DSP_END,													// �Ō�Ɏg�p
	
};
//
//// �����P�ʂ̑����ʒu (�㗬��) (�o���o���ɃZ�b�gOK)
//enum EM_POS_KEN_U {
//	POS_KEN_U_END = 0												// �Ō�Ɏg�p
//};
//// ����
//enum EM_POS_KEN_D {
//	POS_KEN_D_END = 0												// �Ō�Ɏg�p
//};
//
//// �����P�ʂ̑����ʒu (�㗬��) (�o���o���ɃZ�b�gOK)
//enum EM_POS_DSP_U {
//	POS_DSP_U_SPM = 0,												// SPM
//	POS_DSP_U_END													// �Ō�Ɏg�p
//};
//// ����
//enum EM_POS_DSP_D {
//	POS_DSP_D_SPM = 0,												// SPM
//	POS_DSP_D_END													// �Ō�Ɏg�p
//};

// �V�[�P���T���
enum EM_SEQ_STAT {
	SEQ_STAT_INIT=-1,				// �����l
	SEQ_STAT_OK=0,					// ����
	SRQ_STAT_WAR,					// ����
	SEQ_STAT_ERR,					// �ُ�
};

// �V�[�P���T�ғ����
enum EM_SEQ_KADOU {
	SRQ_KADOU_INI=-1,				// �����l
	SEQ_KADOU_RESET=0,				// ���Z�b�g�ς� 
	SEQ_KADOU_INIT,					// �������ς�
	SEQ_KADOU_YOBI,					// �\��
	SEQ_KADOU_START					// �B��
};

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �ȈՋ��ʊ֐�


// �^�C�}�[�N��		nTime=[ms]
static void SetEvWaitableTimer(HANDLE evTimer, int nTime, int nCycle = 0)
{
	LARGE_INTEGER ts;
	ts.QuadPart = (__int64)nTime * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(evTimer, &ts, nCycle, NULL, NULL, FALSE);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �t���[��No�g���b�L���O �\����

// �t���[��No�g���b�L���O���
struct TR_FRAME : public TR_FRAME_BASE {
	int			nCoilChg[NUM_MEN];									// �R�C���ؑփt���O
	int			nSpeed;												// ���C�����x[mm/sec]
	int			nFCut;												// �o�������R�C�� �t�@�[�X�g�J�b�g �؂�ւ��t���O
	int			nRecProcTime[2];									// 1�t���[���捞��������QueryPerformanceCounter
};
#define	MAX_TR_FRAME		2										// �g���b�L���O��(�\�� �P���̂�)

static int	GetTrId(int nHtCy, int nTorB) { return (((nHtCy - DIV_HANTEI01) * NUM_MEN) + nTorB); }	// �t���[���g���b�L���OID�擾
static int	GetTrOtherId(int nTrId) { return (0 == (nTrId % NUM_MEN) ? nTrId + 1 : nTrId - 1); }	// �����ʂ̃t���[���g���b�L���OID�擾

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �x��o�� �\����

// �`�F�b�N�p���
struct OUTPUT_ITEM {
	bool		bUmu;												// �Z�b�g�L��
	int			nMen;												// �Ώۖ� (0:�\ 1:��)
	int			nKizuNo;											// �rNo
	int			nTid;												// �r��
	int			nGid;												// �O���[�h
	int			nTYuu;												// �r��D��x
	int			nGYuu;												// �O���[�h�D��x
	float		dAria;												// �ʐ�
	long		nLen;												// ����ʒu [mm]
	int			nWidDiv;											// �������ʒu (0�I���W��)
	int			nDs;												// DS����̋��� [mm]
	int			nWs;												// WS����̋��� [mm]
	bool		bPatoFlg[MAX_DISP_POS];								// �r�ڋ߃{�C�X�o�͍σt���O(True:�o�͍�)[�e������ʒu����]
	int			cycle_div;											// �����r�敪(0:����r, 1:�ȈՎ����r,�A���r )

	int			nKind;												// �x����	
};


//=======================================
// ���׌x��o�͗p �������b�V�� �\����
//=======================================
// ���׌x��o�͔���p
#define OUTPUT_MESYU_SEKKIN_ROW		500								// �r�ڋ� �̃��b�V�� [mm]
#define OUTPUT_MESYU_SEKKIN_MAX		(COIL_LEN_MAX*1000/OUTPUT_MESYU_SEKKIN_ROW)		// �r�ڋ� �̃��b�V�� �s��
#define OUTPUT_MESYU_SEKKIN_COL		LAMP_COL_NUM					// �r�ڋ߃����v������
#define OUTPUT_MESYU_VOIC_ROW		500								// �����r��\�����r ����{�C�X�̃��b�V�� [mm]
#define OUTPUT_MESYU_VOIC_COL		50								// [mm]
#define OUTPUT_MESYU_ROW			COIL_LEN_MAX*1000/OUTPUT_MESYU_VOIC_ROW	// �{�C�X�o�͔���p �������b�V���s�� (500mm�Ԋu)
#define OUTPUT_MESYU_COL			COIL_WID_MAX/OUTPUT_MESYU_VOIC_COL		// �{�C�X�o�͔���p �������b�V���� (50mm�Ԋu)
#define OUTPUT_VOICE_RE_ROW			5								// �A������ �A������������Y�����ׂƂ��邩
#define OUTPUT_ALARM_COL			ALARM_COL_NUM					// �r�ڋ߃A���[��������


////=========================================
//// �{�C�X�o�͏���
#define OUTPUT_VOICE_ERR_NUM		em_VOICE_ERR_END			 		// �ݔ��ُ�{�C�X��
#define OUTPUT_VOICE_DEF_NUM		em_VOICE_DEF_END - em_VOICE_ERR_END	// ���׌��o�{�C�X��
#define OUTPUT_VOICE_NUM			em_VOICE_DEF_END					// �{�C�X�o�͏�����

enum ENUM_VOICE_ERR_TYPE {
	em_VOICE_ERR_NON = -1,											// ����

	em_VOICE_ERR_KEN,												// �u�r���ُ�ł��B�v
	em_VOICE_ERR_RECV_COILINF,										// �u�R�C����񂪎�M�ł��܂���B�v
	em_VOICE_ERR_SEND_ZISEKI,										// �u�r�����ё��M�ُ�ł��B�v
	em_VOICE_ERR_LIGHT,												// �u�Ɩ����u�ُ�ł��B�v
	em_VOICE_ERR_CAM,												// �u�J�����ُ�ł��B�v
	em_VOICE_ERR_TEMP,												// �u���x�ُ�ł��B�v
	em_VOICE_ERR_END,

	em_VOICE_DEF_TOP = em_VOICE_ERR_END,							// �\
	em_VOICE_DEF_BOT,												// ��
	em_VOICE_DEF_HE,												// �w�Q
	em_VOICE_DEF_SU,												// �X���o
	em_VOICE_DEF_RE,												// �A���r
	em_VOICE_DEF_JU,												// �d���r
	em_VOICE_DEF_SY,												// �����r
	em_VOICE_DEF_BI,												// �����r
	em_VOICE_DEF_OT,												// �r�ڋ�
	em_VOICE_DEF_TB,												// �\�������r
	em_VOICE_DEF_END
};

//=========================================
// ���׌��o�{�C�X (�D�揇�ʏ��B��ԉ����ŋ�)
#define OUTPUT_VOICE_PRI_NUM		em_VOICE_PRI_END				// �D��Ώۂ̌��׌��o�{�C�X��
enum ENUM_VOICE_PRI_TYPE {
	em_VOICE_PRI_NON = 0,											// ����
	em_VOICE_PRI_OT,												// �r�ڋ�
	em_VOICE_PRI_BI,												// �����r �P��
	em_VOICE_PRI_SY,												// �����r
	em_VOICE_PRI_TB,												// �\�������r
	em_VOICE_PRI_SU,												// �X���o
	em_VOICE_PRI_HE,												// �w�Q
	em_VOICE_PRI_JU,												// �d���r
	em_VOICE_PRI_RE,												// �A���r
	em_VOICE_PRI_END
};

// �D��x�����׌��o�{�C�X��������DM�A�h���X���{�C�X�������擾
static ENUM_VOICE_ERR_TYPE GetVoiceDefType(ENUM_VOICE_PRI_TYPE emVoiceDefType) {
	switch (emVoiceDefType) {
		case em_VOICE_PRI_NON:	return em_VOICE_ERR_NON;		// ����
		case em_VOICE_PRI_OT:	return em_VOICE_DEF_OT;			// �r�ڋ�
		case em_VOICE_PRI_BI:	return em_VOICE_DEF_BI;			// �����r
		case em_VOICE_PRI_SY:	return em_VOICE_DEF_SY;			// �����r
		case em_VOICE_PRI_TB:	return em_VOICE_DEF_TB;			// �\�������r
		case em_VOICE_PRI_SU:	return em_VOICE_DEF_SU;			// �X���o
		case em_VOICE_PRI_HE:	return em_VOICE_DEF_HE;			// �w�Q
		case em_VOICE_PRI_JU:	return em_VOICE_DEF_JU;			// �d���r
		case em_VOICE_PRI_RE:	return em_VOICE_DEF_RE;			// �A���r
		default:				return em_VOICE_DEF_END;
	}
};

// DM�A�h���X���{�C�X��������D��x�����׌��o�{�C�X�������擾
static ENUM_VOICE_PRI_TYPE GetVoicePriType(ENUM_VOICE_ERR_TYPE emVoiceDefType) {
	switch (emVoiceDefType) {
		case em_VOICE_ERR_NON:	return em_VOICE_PRI_NON;		// ����
		case em_VOICE_DEF_OT:	return em_VOICE_PRI_OT;			// �r�ڋ�
		case em_VOICE_DEF_BI:	return em_VOICE_PRI_BI;			// �����r
		case em_VOICE_DEF_SY:	return em_VOICE_PRI_SY;			// �����r
		case em_VOICE_DEF_TB:	return em_VOICE_PRI_TB;			// �\�������r
		case em_VOICE_DEF_SU:	return em_VOICE_PRI_SU;			// �X���o
		case em_VOICE_DEF_HE:	return em_VOICE_PRI_HE;			// �w�Q
		case em_VOICE_DEF_JU:	return em_VOICE_PRI_JU;			// �d���r
		case em_VOICE_DEF_RE:	return em_VOICE_PRI_RE ;		// �A���r
		default:				return em_VOICE_PRI_NON;
	}
};

static CString GetVoiceTypeName(int emVoiceDefType)
{
	switch (emVoiceDefType) {
	case em_VOICE_ERR_KEN:				return "�Ӄ�ERR";		// �u�r���ُ�ł��B�v
	case em_VOICE_ERR_RECV_COILINF:		return "��ُ��ERR";	// �u�R�C����񂪎�M�ł��܂���B�v
	case em_VOICE_ERR_SEND_ZISEKI:		return "���ё��MERR";	// �u�r�����ё��M�ُ�ł��B�v
	case em_VOICE_ERR_LIGHT:			return "�Ɩ�ERR";		// �u�Ɩ����u�ُ�ł��B�v
	case em_VOICE_ERR_CAM:				return "���ERR";		// �u�J�����ُ�ł��B�v
	case em_VOICE_ERR_TEMP:				return "���xERR";		// �u���x�ُ�ł��B�v
	case em_VOICE_ERR_NON:				return "";				// ����
	case em_VOICE_DEF_OT:				return "�r�ڋ�";		// �r�ڋ�
	case em_VOICE_DEF_BI:				return "����";			// �����r
	case em_VOICE_DEF_SY:				return "����";			// �����r
	case em_VOICE_DEF_TB:				return "�\��";			// �\�������r
	case em_VOICE_DEF_SU:				return "����";			// �X���o
	case em_VOICE_DEF_HE:				return "͹�";			// �w�Q
	case em_VOICE_DEF_JU:				return "�d��";			// �d���r
	case em_VOICE_DEF_RE:				return "�A��";			// �A���r
	default:							return "";
	}

}

//=========================================
// �{�C�X�o�� ���
enum ENUM_VOICE_STAT {
	em_VOICE_STAT_OFF = 0,											// ��~
	em_VOICE_STAT_ON,												// �o��
};

//=========================================
// �p�g���C�g�ݒu�ʒu
#define OUTPUT_PATO_POS_NUM			em_PATO_POS_END					// �p�g���C�g�ݒu�ʒu�̐�
enum ENUM_PATO_POS_TYPE {
	em_PATO_POS_KENSA = 0,											// ������
	em_PATO_POS_UNTEN,												// �^�]��
	em_PATO_POS_END													// �Ō�
};

//=========================================
// �p�g���C�g���
#define OUTPUT_PATO_KIND_NUM		em_PATO_KIND_END				// �p�g���C�g��ނ̐�
enum ENUM_PATO_KIND_TYPE {
	em_PATO_KIND_RAMP = 0,											// �����v
	em_PATO_KIND_BUZZ,												// �u�U�[
	em_PATO_KIND_END												// �Ō�
};

//=========================================
// �r�ڋ߃����v���
#define OUTPUT_RAMP_NUM		em_RAMP_END								// �r�ڋ߃����v�̐�
enum ENUM_RAMP_TYPE {
	em_RAMP_NON = -1,												// ��Ώ�(=�����v�o�͖���)
	em_RAMP_KEI,													// �y���׌��o
	em_RAMP_JYU,													// �d���׌��o
	em_RAMP_END
};

//=========================================
// �r�ڋ߃����v���
enum ENUM_RAMP_STAT {
	em_RAMP_STAT_OFF = 0,											// ��~
	em_RAMP_STAT_ON,												// �o��
};

//=========================================
//// ���׌x����
// ���׌x��o�͏���
struct ALARME_INF
{
	// �\����
	OUTPUT_ITEM		ranp[NUM_MEN][OUTPUT_MESYU_SEKKIN_MAX][OUTPUT_MESYU_SEKKIN_COL];			// �r�ڋ߃����v�̏��
	OUTPUT_ITEM		voice[NUM_MEN][OUTPUT_MESYU_ROW];											// �{�C�X�o��
	bool			voic_re[NUM_MEN][OUTPUT_MESYU_ROW][OUTPUT_MESYU_COL];						// �A���r���f�p		(�Z���^�[�) (�R�C���P�ʂŃX���C�h�����Ȃ���`�F�b�N���Ă���)
	bool			voic_tb[NUM_MEN][OUTPUT_MESYU_ROW][OUTPUT_MESYU_COL];						// �\�������r���f�p	(�Z���^�[�) (�R�C���P��)
};

// �x��o�͌��ʎ󂯓n���p
struct ALARME_OUTPUT {
	ENUM_RAMP_STAT	ranp[NUM_MEN][OUTPUT_MESYU_SEKKIN_COL][OUTPUT_RAMP_NUM];	// �r�ڋ߃����v�̏��
	ENUM_RAMP_STAT	alarm[NUM_MEN][OUTPUT_MESYU_SEKKIN_COL][OUTPUT_RAMP_NUM];	// �r�ڋ߃A���[���̏��
	int				pato[OUTPUT_PATO_POS_NUM][OUTPUT_PATO_KIND_NUM];			// �p�g���C�g�̏��
	ENUM_VOICE_STAT	voice[OUTPUT_VOICE_NUM];									// �{�C�X�̏��
};

// �p�g���C�g�p��� (���Z�b�gPB�Ή�)
struct PATO_INF
{
	// �r����L�[
	char					cKizukenNo[SIZE_KIZUKEN_NO];			// �r���Ǘ�No
	int						nMen;									// �\��
	int						nKizuSeq;								// �r�A��
	int						rec;									// 1m�Ԋu�̃��R�[�h (�x��o�͗p�̓������b�V���̔z��ʒu)

	long					nY;										// ����ʒu (�r���S�ʒu) [mm] 

																	// ���
	bool					bRun;									// �p�g���C�g���s�� (true:���s�Ώ�(�p�g���C�g�o�͊J�n�ʒu���߂����r)  false:�ΏۊO )
};

//=======================================
// �o�������R�C���P�ʂ̏�Ԋ֘A
//=======================================
//// �g���b�L���O�C�x���g�敪
enum EM_TR_KEY_COILOUT_ST {
	TR_KEY_KENSA_TOP = 1,											// ������� �\
	TR_KEY_KENSA_BOT,												// ������� ��
	TR_KEY_KIKI_TOP,												// �@���� �\
	TR_KEY_KIKI_BOT,												// �@���� ��
};


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �S�R�C����� �\����


//======================================================
// �����_���R�C���p
#define SIZE_COIL_ROWVAL			1								// �s���  0:���ɂȂ�


#define SIZE_COIL_BUF				11								// ������/���������p�̓����_���R�C�����̃o�b�t�@
struct	COIL_INF
{
	//// ��{
	int						rec;									// �������g�̔z��ʒu (0�I���W��)

	//// ��ʏ��
	COIL_BASEDATA			data;									// �V�X�e�����ʃR�C�����
	PRO_TYPE_DATA_COILINF	data_u;									// �R�C�����
	char					cCoilKey[SIZE_COIL_KEY];				// �L�[���
	double					dSpmHosei;								// SPM�␳���i1.0 + SPM�L�ї�[10^2%]�j
	
	//// ���ߎ��Ɋm��
	char					cKizukenNo[SIZE_KIZUKEN_NO];			// �Ǘ�No
	bool					bDummeyCoil;							// �_�~�[�R�C���� (true:�_�~�[�R�C��)
	bool					bCoilInit;								// �����J�n�P�{�ڃR�C���� (true:�����J�n�P�{��)
	COIL_SETTING			setting;								// �ӃւŎg���R�C����{���
	int						nMapCol;								// �}�b�v�̕��� (�R�C�������Z�o�B�s�\���́A�ő�B�悭�g�����玝�����Ă���)
	int						nHtID[NUM_MEN][NUM_HANTEI];				// �g�p����PC (���̌����Ŏg�p��������PCID)
	int						nRpiID[NUM_MEN][NUM_RPIEXEC];			// �g�p�S������PC (���̌����Ŏg�p�����S������PCID)
		
	//// �R�C���ؑ֎��Ɋm��
	int						nStartWpdOk;							// �R�C����[��������_�M��(0:����WPD  1:�َ�_  2:�Q�[�g�M��)
	int						nCoilCnt;								// �����J�n����x�{�ڂ̃R�C���i�����J�n1�{�ڂ�0�B�ȍ~�_���R�C���ؑւ̃^�C�~���O��+1�j
	// ===>> �⊪ 2021.06.03 ���R�C�����ޒʔ̎��A�R�C������r������ɂł��Ȃ��s��C��
	double					dWpdOnLength;							// �����pWPDON���̃V�[�P���T�Ǘ����R�C����
	// <<===
	
	//// �������ɐ����X�V
	int						nSt_Hantei[NUM_MEN];					// ������ �\��	 (0:���� 1:�ُ�)
	int						nSt_Light[NUM_MEN];						// �Ɩ����u��� �\��
	int						nSt_CamControl;							// �J�����R���g���[����H
	int						nSt_CamSwitch;							// �J���������N�X�C�b�`��H
	int						nSt_Procon;								// �v���R�����
	int						nSt_FF;									// FF���
	int						nSt_Sqence;								// �V�[�P���T���
	int						nSt_SqeDm;								// �f�[�^���������
	int						nSt_SqeUdp;								// �V�[�P���TUDP���
	int						nSt_Daisya[NUM_MEN];					// �J������ԏ��[0:�\ 1:��]
	int						nSt_DbSkip[NUM_MEN];					// DB���ۑ�
	int						nSt_FrameSkip[NUM_MEN];					// �t���[���X�L�b�v
	int						nKizuNoNum;								// �rNo�̃x�[�X�ƂȂ�B
	int						nSt_RpiSave[NUM_MEN];					// �S���摜�ۑ����
	int						nSt_ToyuSplr;							// �h���X�v���[���u���


	// �ʒu���֘A
	EM_DIV_KENSA 			emKensa[NUM_MEN][MAP_ROW_NUM];			// ������� (5m�s�b�`) [��PlgManager]
	EM_DIV_KIKI				emKiki[NUM_MEN][MAP_ROW_NUM];			// �@���� (5m�s�b�`) [��PlgManager]
	//int						nDstyCycle[NUM_MEN][MAP_ROW_NUM];		// ���x����
	int						nRowVal[NUM_MEN][MAP_ROW_NUM][SIZE_COIL_ROWVAL];	// �t����� (5m�s�b�`) 
	float					dEdge[NUM_MEN][MAP_ROW_NUM][2];			// �G�b�W�ʒu (5m�s�b�`) [��EdgePosControl]

	int						nSetRec_St[NUM_MEN];					// ������ԁE�@���Ԃ̍ŏI�Z�b�g�ʒu (1�I���W��)
	int						nSetRec_Edge[NUM_MEN];					// �G�b�W�ʒu�̍ŏI�Z�b�g�ʒu(1�I���W��)
	int						nSetRec_Last[NUM_MEN];					// �ʒu���̕t���f�[�^(��Ԓx���Z�b�g����f�[�^)�̍ŏI�Z�b�g�ʒu (1�I���W��)
	int						nLastRec_Postion;						// �ŏI�������݈ʒu (1�I���W��)
	
	// ���׌x��֘A
	bool					bAlarmCancel;							// �Y���R�C���̂݌��׌x��֎~ (true:�֎~)
	ALARME_INF				alarme;									// �x��o�͗p���׏��

		
	// �����ʒu�ʔ������Ɋm��
	int						nEndMode;								// ���ъi�[�敪		 (1:�ʏ�ؑ� 2:�o�b�N�A�b�v�ؑ� 3:�蓮��~ 4:������~ 0:�s��܂��͂܂��ʔ�)
	long					nLength;								// �Ӄ֎����� �imm�j
	long					nLengthSub;								// ������r������ (mm)
	bool					bCoilResultAddDb;						// �R�C�����т�DB�ɓo�^�����ςݎ� true

	EM_DIV_UNTEN 			emUnten;								// �^�]���
	EM_DIV_KENSA			emOneKensa[NUM_MEN];					// ������� (���Ԃł����̒l���L�����ꍇ)
	EM_DIV_KIKI				emOneKiki[NUM_MEN];						// �@���� (���Ԃł����̒l���L�����ꍇ)
	EM_DIV_PLG				emPlg;									// PLG���

	int						nAgcVal[NUM_CAMERA];					// �I������ (���̃R�C���̗������������̘I�����ԁB���R�C���p�̌����pWPD ON���Ɋm�� = ���ʔ��R�C�����[�̕��Ȃ�I�����Ԃ����������Ă���͂�)
	int						nBrVal[NUM_CAMERA];						// �P�x (���̃R�C���̗������������̋P�x�B���R�C���p�̌����pWPD ON���Ɋm�� = ���ʔ��R�C�����[�̕��Ȃ�P�x�����������Ă���͂�)
	int						nGain[NUM_CAMERA];						// �Q�C���i���̃R�C���̗������������̋P�x�B���R�C���p�̌����pWPD ON���Ɋm�� = ���ʔ��R�C�����[�̕��Ȃ�P�x�����������Ă���͂��j
	int						nValue[NUM_MEN];						// �����l�i���̃R�C���̗������������̋P�x�B���R�C���p�̌����pWPD ON���Ɋm�� = ���ʔ��R�C�����[�̕��Ȃ�P�x�����������Ă���͂��j
	
	// �o���Œ��߂��B(=�A�b�v�f�[�g�̉\���L��)
	long					nDspLength;								// �\�������� [mm]
	long					nOutCoilPos;							// �o���P�ʐ�[�ʒu [mm]
	int						nCutCount;								// �J�b�g��
	int						nCutLen[MAX_SHACUT_COUNT];				// �r���ŊǗ�����J�b�g�ʒu [mm]

	// �\�����Ɋm��
	int						nSt_Spm;								// SPM�����L���@0�F�J����� 1�F�������
	bool					bDspOk;									// �\����_��ʉߎ� true

	// ������_WPD ON���̏㗬�J�����ʒu�ʉ߃R�C��
	long					nLen_Yotei;								// �\��R�C���� [mm]

	int						nDefCnt[NUM_MEN][MAP_ROW_NUM];			// �ŏ���Ԗ����r���o��
};

//======================================================
//// �o���R�C���p
#define SIZE_COIL_OUT_BUF			1								// �o�������R�C�����̃o�b�t�@

struct	COIL_OUT_INF
{
	//// ��{
	//int						rec;									// �������g�̔z��ʒu�i0�I���W���j


	//char					cOutKizukenNo[SIZE_KIZUKEN_NO];			// �o���Ǘ�No
	//int						nDivResult;								// ���ъi�[�敪(1:�ʏ�ؑ�,2:�o�b�N�A�b�v�ؑ�,3:�蓮��~,4:������~)
	char					cID[SIZE_SERIAL_FF_NO];					// ID
	int						nCnt;									// ��������H
	//char					cKizukenNo[SIZE_KIZUKEN_NO];			// �œ��������Ǘ�No(��{��[0]���œ����Ǘ�No�Ƃ��č̗p)
	//char					cOutCoilNo[PRO_SIZE_COIL_NO+2];			// �o���R�C��No(�������͎q�ԕt�^)
	
	//int						nLength;								// ���������� [mm]
	//int						nFCutLen;								// F���[�J�b�g�� [mm]
	//int						nTCutLen;								// T���[�J�b�g�� [mm]
	//int						nCutLen;								// �[�J�b�g�� [mm]
	
	//EM_DIV_UNTEN			emUnten;								// �^�]���
	//EM_DIV_KENSA			emOneKensa[NUM_MEN];					// ������� (���Ԃł����̒l���L�����ꍇ)
	//EM_DIV_KIKI				emOneKiki[NUM_MEN];						// �@���� (���Ԃł����̒l���L�����ꍇ)


	// �����_���P�ʂ̏��
	struct INCOIL {
		char				cKizukenNo[SIZE_KIZUKEN_NO];			// �����Ǘ�No
		int					nInCutNum;								// �������� (1�`)	�� �����_���R�C�����̉������ڂ�
		int					nFront;									// �擪�ʒu [mm]	�� �����_���R�C���擪���牽mm��
		int					nLength;								// ���� [mm]
		//int					nWidth;									// �ޗ���[0.1mm]
		int					nOutPos;								// �o�������ʒu [mm]�� �o�������R�C���擪���牽mm��
		//int					nEnable;								// �q�Ԋ��t�L��(0:�L�� 0�ȊO:����(999:���[��, 998:��[��))
		//char				cInCoilNo[PRO_SIZE_COIL_NO+2];			// �����R�C��No(�������͎q�ԕt�^)
		//COIL_SETTING		setting;								// �R�C����{�ݒ�

		//COIL_INF*			pCoilIn;								// �����_���R�C�����̃|�C���^
		
		int						nSt_Spm;							// SPM�����L��
		double				dSpmHosei;								// SPM�����␳��
	} incoil[MAX_SHALINK_COUNT];									// MAX�o������4�R�C��(�O�̂��߁A�{�̃G���A��ێ�����)
};


//// �V���[�J�b�g
struct SHRCUT_ITEM {
	char					cKizukenNo[SIZE_KIZUKEN_NO];			// �Ǘ�No
	double					dPos;									// �����P�ʂ̒���ʒu [mm]
	//double					dPosOut;								// �o���P�ʂ̒���ʒu [mm]
};


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DB�ۑ��p �\����

//======================================================
// DB�ۑ��p ��`

//---------------------->>
// ���p�N���X���Ŏg���ꍇ�́A������ɒ�`
enum ENUM_DB_KUBUN {
	em_STATUS_LOG = 0,												// �X�e�[�^�X���O
	em_DB_KUBUN_END													// �I��� (�g���Ă���)
};
// StatusMgr �� DBMgr �ŏz�ˑ��֌W�ɂȂ����΍�
//// �󂯓n���\����
struct DELIVERY_DB {
	ENUM_DB_KUBUN		kubun;										// �ǂ̍\���̂�
	BYTE*				data;										// �\���̂̃|�C���^
};
// ---------------------

//======================================================
// ���S���p �֌W

//// �@���ԃ��O�p
struct DBDATA_STATUS_LOG
{
	EM_DIV_KADOU			emKadou;								// �ғ����
	EM_DIV_KENSA			emKensa[NUM_MEN];						// �������
	EM_DIV_KIKI				emKiki[NUM_MEN];						// �@����
	char					cMsg[256];								// �K�C�_���X

																	// �g����
	char					cKizukenNo[SIZE_KIZUKEN_NO];			// �r���Ǘ�No
	char					cCoilNo[32];							// �R�C��No

																	// ��L�[ (���ʃN���X�ŃZ�b�g)
	CTime					time;									// �o�^���t
	int						nAutoNo;								// �A��
};


//======================================================
// �R�C����� (�����R�C��)
struct DBDATA_COIL_INF
{
	char			cKizukenNo[SIZE_KIZUKEN_NO];		// �Ǘ�No
	int				scode[NUM_MEN];						// �����\�ʏ�� (�\/��)(1�I���W��)
	int				pcode[NUM_MEN];						// �����p�^�[�� (�\/��)(1�I���W��)

	//-------------------- �v���R���R�C�����
	char			cCoilKey[SIZE_COIL_KEY];			// �L�[���
	char			cMeiNo[SIZE_MEI_NO];				// ����No
	char			cCoilNo[PRO_SIZE_COIL_NO];			// �R�C��No
	long			nSendPoint;							// ���M�ʒu (0:WPD3 1:WPD4 2:WPD6-50m)
	long			nLenIn;								// �����_���R�C�� ���� [m]
	long			nAtuZai;							// �ޗ��� [��m] (����)
	long			nWidZai;							// �ޗ��� [mm]
	long			nAtuSei;							// ������ [��m] (�o��)
	long			nWidSei;							// ������ [mm]
	long			nWidTrim;							// ���H���g������ [mm] (�m�[�g�������́A0)
	long			nNobiSPM;							// SPM�L�ї� [10-2%]
	long			nNobiTLV;							// TLV�L�ї� [10-2%]
	long			nCutF;								// �����ɂ�����t�����g�[�J�b�g�̎��ђ�[mm]
	long			nCutT;								// �����ɂ�����e�[���[�J�b�g�̎��ђ�[mm]
	long			nSPMRoolKei_Up;						// SPM���[�N���[���a (��) [10-1mm]
	long			nSPMRoolKei_Down;					// SPM���[�N���[���a (��) [10-1mm]

	char			cKenKijun_O[4];						// ����� �\
	char			cKenKijun_U[4];						// ����� ��
	char			cKurometo_Kind[2];					// �N�����[�g ���ю��
	char			cKurometo_Umu[2];					// �N�����[�g ���їL��
	char			cHinCode[1];						// �i��
	char			cMekkiKind[1];						// ���b�L���
	char			cSyuZai[2];							// ��ގ�
	char			cDifMaker[1];						// �f�B�t�@�����V�����}�[�J�[
	char			cKenban[1];							// ���敪
	char			cMekkiCycle[1];						// ���b�L�T�C�N��
	char			cInLineSPM[1];						// ���x���[ �C�����C��SPM
	char			cKagakuToyu[1];						// ���w�����h��
	char			cAtukkaSPM[1];						// �I�t���C��SPM������
	char			cMekkiKubun[1];						// ���b�L�敪
	char			cOmoteSyori1[1];					// �\�ʏ���1����
	char			cOmoteSyori2[1];					// �\�ʏ���2����
	char			cToyu[1];							// �h��
	char			cKouteiYobi[1];						// �H���L���\��
	char			cBaund[1];							// �o�E���_�����킹
	char			cCaplSyuZai[10];					// CAPL ��ގ�
	char			cMetuke[8];							// �ڕt�K�i
	char			cNi[2];								// Ni�t����
	char			cSodoTCM[2];						// TCM���[���e�x
	char			cSpmUmu[2];							// SPM�����L�� (�L��:O  ����:X)
	char			cNextLine[2];						// �v�掟�H���R�[�h
	char			cMuke[32];							// ����
	char			cYouto[4];							// �p�r
	char			cCaplSiage[4];						// CAPL�\�ʎd�グ
	char			cCaplSanUmu[2];						// CAPL�_���L��
	char			cHinMode[1];						// �i��敪

	char			cGaCrKubun[1];						// GA/CR�敪
	char			cSpmJisekiUmu[2];					// SPM�������їL���i�L��F0 ����:X�j
	char			cKouteiCode[2];						// �H���R�[�h

};

//======================================================
// �R�C������
struct DBDATA_COIL_RESULT
{
	char			cKizukenNo[SIZE_KIZUKEN_NO];		// �Ǘ�No
	int				nEndMode;							// ���ъi�[�敪		 (1:�ʏ�ؑ� 2:�o�b�N�A�b�v�ؑ� 3:�蓮��~ 4:������~ 0:�s��)
	int				nSt_Hantei[NUM_MEN];				// ������[�\��]	 (0:���� 1:�ُ�)
	int				nSt_Light[NUM_MEN];					// �Ɩ����u���[�\��]
	int				nSt_CamControl;						// �J�����R���g���[����H���
	int				nSt_CamSwitch;						// �J���������N�X�C�b�`��H���
	int				nSt_Procon;							// �v���R�����
	int				nSt_FF;								// FF���
	int				nSt_Sqence;							// �V�[�P���T���
	int				nSt_Cam_Inline[NUM_MEN];			// �J������ԏ��[�\��]
	int				nSt_DbSkip[NUM_MEN];				// DB���ۑ�[�\��]
	int				nSt_FrameSkip[NUM_MEN];				// �t���[���X�L�b�v[�\��]
	int				nHtID[NUM_MEN][NUM_HANTEI];			// �g�p����PC[�\��] (���̌����Ŏg�p��������PCID)
	int				nRpiID[NUM_MEN][NUM_RPIEXEC];		// �g�p�S��PC[�\��] (���̌����Ŏg�p�����S������PCID)
	int				bStartWpdOk;						// ������_�M�����o��

	COIL_SETTING	setting;							// �ӃւŎg���R�C����{���

	int				nSpmStat;							// SPM�����L��[0:SPM��������,1:SPM�����L��]
	long			nLength;							// �Ӄ֎����� [SPM��������/SPM�����L��][mm]
	long			nLengthSub;							// ������r������ [mm]
	long			nLengthDsp;							// �\�������� [mm]
	int				nOutCoilPos;						// �o���P�ʐ�[�ʒu [mm] �� ���킩���Ă��镪����

	EM_DIV_UNTEN 	emUnten;							// �^�]���
	EM_DIV_KENSA	emOneKensa[NUM_MEN];				// ������� (���Ԃł����̒l���L�����ꍇ)
	EM_DIV_KIKI		emOneKiki[NUM_MEN];					// �@���� (���Ԃł����̒l���L�����ꍇ)
	EM_DIV_PLG		emPlg;								// PLG�敪
	int				nResultSendOk;						// �O�����ё��M���� (0�F���M  1:���M���Ȃ�)
	int				nBr[MAX_CAMSET][MAX_CAMANGLE][3];	// �P�x[�J�����Z�b�g][�J�����p�x][C,L,R]		// C����DB�ɂ̓Z�b�g���Ȃ�
	int				nExp[MAX_CAMSET][MAX_CAMANGLE];		// �I������[�J�����Z�b�g][�J�����p�x]
	int				nGain[MAX_CAMSET][MAX_CAMANGLE];	// �Q�C��[�J�����Z�b�g][�J�����p�x]
	int				nCoilCnt;							// �����J�n����x�{�ڂ̃R�C���i�����J�n1�{�ڂ�0�B�ȍ~�_���R�C���ؑւ̃^�C�~���O��+1�j
	int				nToyuSply;							// �h���X�v���[���u��� (0�F�����t  1:1��Ԃł����t���L��)
};

////======================================================
//// �o���R�C��(�����E�_������)
//struct	DBDATA_COIL_OUT_INF
//{
//	COIL_OUT_INF	data;											// �o���R�C�����
//};



//======================================================
// �R�C�����уA�b�v�f�[�g �i���ъm����UpDate�p�j
enum EM_MODE_COILRESULT_UPDATE
{
	MODE_RECV_SHRCUT = 0,						// �V���[�J�b�g�M����M
	MODE_DSP_WPD_ON								// �\��WPD�M��ON
};

struct DBDATA_COIL_RESULT_UPDATE
{
	EM_MODE_COILRESULT_UPDATE	emMode;								// ���s���
	char						cKizukenNo[SIZE_KIZUKEN_NO];		// �Ǘ�No

	// ---- �V���[�J�b�g�M����M --------------------
	int							nOutCoilPos;						// �o���P�ʐ�[�ʒu [mm]

	// ---- �\��WPD�M��ON --------------------
	int							nDspLength;							// �\��������
	int							nSpmUmu;							// SPM�����L��
};


//======================================================
// �R�C���ʒu���
struct DBDATA_COIL_POSITION
{
	char					cKizukenNo[SIZE_KIZUKEN_NO];			// �Ǘ�No
	int						nID;									// ID (1�`�A��)

	EM_DIV_KENSA			emKensa[NUM_MEN];						// �������
	EM_DIV_KIKI				emKiki[NUM_MEN];						// �@����

	float					dEdge[2];								// �G�b�W�ʒu [0]�� [1]�E
	float					dWalk;									// �E�H�[�N��
};


//======================================================
// �R�C���g�����h���
struct DBDATA_COIL_TRAND
{
	char					cKizukenNo[SIZE_KIZUKEN_NO];			// �Ǘ�No
	int						nMen;									// �\���敪 (0:�\ 1:��)
	int						nLen;									// ����ʒu [mm] (����ʒu)
	
	// �g�����h�f�[�^
	int						nSpeed;									// ���C�����x [mpm]
	float					dCoilWid;								// �R�C���� [mm]
	float					dCoilWidMax;							// �R�C�����ő� [mm]
	float					dCoilWidMin;							// �R�C�����ŏ� [mm]
	float					dWalk;									// �E�H�[�N�� [mm]
#ifndef HT_IS_COLOR
	int						nBr[NUM_CAM_POS][NUM_CAM_ANGLE][3];		// ���ϋP�x [0:DS/1:WS][0:��/1:��][0:C/1:L/2:R]
	int						nExp[NUM_CAM_POS][NUM_CAM_ANGLE];		// �I������ [clk] (��/��)
#else
	int						nBr[NUM_CAM_POS][MAX_CAMANGLE][3];		// ���ϋP�x (R, G, B)(C, L, R �̏�)	// C����DB�ɂ̓Z�b�g���Ȃ�
	int						nExp[NUM_CAM_POS][MAX_CAMANGLE];		// �I������ [clk]
	float					dGain[NUM_CAM_POS][3];					// �J�����Q�C���l(R, G, B�̏�)
#endif
	int						nLightValue[DIV_LED_COLOR_END];			// �Ɩ����u_�����l(�\,��)(R,G1,G2,B1,B2)

	double					dSpmHosei;								// SPM�␳���i1.0 + SPM�L�ї�[10^2%]�j
};

//======================================================
// �J�b�g�ʒu
struct DBDATA_COIL_CUT_POS
{
	char					cKizukenNo[SIZE_KIZUKEN_NO];			// �Ǘ�No
	int						nCutNo;									// ����No
	int						nMode;									// �敪
																		// 1:�V���[�J�b�g �i�t�@�[�X�g�J�b�g�j
																		// 2:�V���[�J�b�g �i���X�g�J�b�g�j
																		// 3:����         �i�� �����́A�P�_�̂݁B�J�b�g�́A�͈́j
																		// 4:�َ�_
	//int						nPos[NUM_SPM_STAT];						// �擪�ʒu [0:SPM��������,1:SPM�����L��][mm]
	//int						nOutLen[NUM_SPM_STAT];					// �ۗL���� [0:SPM��������,1:SPM�����L��]
	//int						nOutPos[NUM_SPM_STAT];					// �o�������ʒu [0:SPM��������,1:SPM�����L��]
	COIL_OUT_INF			data;									// �o���R�C�����
};


//======================================================
// �r���

struct DBDATA_DEFECT_INF
{
	char					cKizukenNo[SIZE_KIZUKEN_NO];			// �Ǘ�No
	DETECT_BASE_DATA		data;									// ��{�r���
	float					dLen;									// �r�����ʒu

	double					attr_base[MAX_ATTR_BASE];				// ��{������
	double					attr_angle[MAX_HT1_CAMANGLE][MAX_ATTR_ANGLE];	// �p�x�ʓ�����

	// �g������
	int						nLowGrFlg;								// ���Q�r�ꎞ�ۑ��t���O (��DB�p�ۑ� <=x< �D��DB�ۑ� �͈͓̔��̏ꍇ�A1)
	int						nIsDspFlg;								// �\���Ώۋ敪_�T����� (1:�T����ʕ\���Ώ�)
	bool					bImgSkip;								// �r�摜�X�L�b�v�t���O(true:�X�L�b�v)
	//int						nRequiredTime[MAX_REQUIRED_TIME];		// �摜�捞�J�n�`�������������܂ł̊e���v����

	// �摜�f�[�^��JPEG�̏ꍇ
	bool					isBmp;									// true�̏ꍇBMP�摜�Ɣ��肷��
	int						nImgsize[MAX_HT1_CAMANGLE];				// �摜�T�C�Y
	BYTE*					pImg[MAX_HT1_CAMANGLE];					// ��/���摜 �|�C���^

	double					dSpmHosei;								// SPM�␳���i1.0 + SPM�L�ї�[10^2%]�j

	// BCP�����݃N���X��n���p
	int						nKizuNo;								// ������rNo

};

//======================================================
//// �G�b�W
// �G�b�W���o����
struct DBDATA_EDGE_COIL_INF
{
	char					cKizukenNo[SIZE_KIZUKEN_NO];			// �r���Ǘ�No
	int						nMen;									// �\���敪

	float					dCoilWidMin;							// ���ŏ�
	float					dCoilWidMax;							// ���ő�

	int						nLimitEdgeErrNum;						// �����e�덷�͈͊O��
	int						nLostEdgeNum[2];						// �G�b�W�����o�� (���G�b�W/�E�G�b�W)

	int						nFrameNumCoil;							// �R�C���P�ʃt���[����
	int						nFrameNumSkip[NUM_CAM_POS][5];			// �J�����Z�b�g (1�`10) (���������1�`�T)
};

// �G�b�W���o���[�h�ω��Ǘ�
struct DBDATA_EDGE_CHANGE_INF
{
	char					cKizukenNo[SIZE_KIZUKEN_NO];			// �Ǘ�No
	int						nPos;									// �����ʒu [mm]
	int						nMen;									// �\���敪 (0:�\ 1:��)
	int						nFrameNo;								// �t���[��No
	int						nEdgeMode;								// ���o�敪 (0:����  1:�д��ޗD��  2:����ײݐU�蕪��  -1:�X�L�b�v   -9:�ΏۊO   -99:������ԁj
	float					dEdgeRawPos[2];							// ���G�b�W�ʒu [mm] (���G�b�W/�E�G�b�W)
	float					dEdgeAnsPos[2];							// �ŏI�G�b�W�ʒu [mm] (���G�b�W/�E�G�b�W)
	int						nEdgeErrReason;							// �G�b�W�s�����R�̘_���a
																	//	0x00:����
																	//	0x01:���G�b�W���o�͈͊O
																	//	0x02:�E�G�b�W���o�͈͊O
																	//	0x04:�����e�덷�͈͊O
																	//	0x08:�O�t���[���Ƃ̋��e�͈͊O
};

//======================================================
//// �J���������N�X�C�b�`�ݒ�
struct DBDATA_CAMERA_LINKSWITCH
{
	int						nPCID[NUM_MEN][NUM_HANTEI];						// ����PCID
	int						nParaPCID[NUM_ALL_HTPARA];						// ���W�b�N�p������PCID
	int						nLogiParaCam[NUM_ALL_HTPARA][NUM_CAMLINK_BOARD];// ���W�b�N�p�������p�J�����Z�b�g
};

//======================================================
// ���̑�

//�V�[�P���T����p���X���[�g�ύX
struct DBDATA_PULSE_RATE_UPDATE
{
	double					dPls[3];								// �ύX��̃p���X���[�g[0:�����pPLG, 1:������r�pPLG, 2:�\���pPLG][mm/pls]
	double					dVres[3];								// �ύX��̒����������\[0:�����pPLG, 1:������r�pPLG, 2:�\���pPLG][mm/pixel]
																	// ���Ԃ�[0](�����pPLG)�ȊO�g��Ȃ�^^;
};


//DB�₢���킹�󋵊m�F
struct DBDATA_ADD_SYNC
{
	MAIL_ITEM				sendItem;								// ���ʕԋp��
};


//BCP batch �������s
struct DBDATA_BCP_BATCH_EXEC
{
	bool					bMode;									// ���[�h
};

//�r�摜�o�^�����Z�b�g
struct DBDATA_COUNT_RESET
{
	char					cKizukenNo[SIZE_KIZUKEN_NO];			// �Ǘ�No
	int						nTorB;									// �\���敪(0:�\ 1:�� 2:����)
};



/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �Ǝ��̕��ނȂ�

// POR����
enum DIV_POR_DIRECTION
{
	DIV_POR_DOWN = 0,	// ���o��
	DIV_POR_UP			// ��o��
};


static CString GetPorDirection(DIV_POR_DIRECTION dir)
{
	CString sWk;

	switch (dir) {
	case DIV_POR_DOWN:	sWk = "���o��";		break;
	case DIV_POR_UP:	sWk = "��o��";		break;
	default:			sWk = "�s��";		break;
	}
	return sWk;
}


// ��Ԉʒu
enum DIV_DAISYA_POS
{
	DIV_DAISYA_IN = 0,			// �C�����C���ʒu
	DIV_DAISYA_OFF				// �I�t���C���ʒu

};
static CString GetDaisyaPos(DIV_DAISYA_POS pos)
{
	CString	sWk;
	switch (pos) {
	case DIV_DAISYA_IN:		sWk = "�C�����C���ʒu";		break;
	case DIV_DAISYA_OFF:	sWk = "�I�t���C���ʒu";		break;
	default:				sWk = "�ʒu�s��";			break;
	}
	return sWk;
}

static CString GetSpmStat(EM_DIV_SPM_STAT spm)
{
	CString	sWk;
	switch (spm) {
	case DIV_SPM_FALSE:		sWk = "�J����";		break;
	case DIV_SPM_TRUE:		sWk = "������";		break;
	default:				sWk = "����";		break;
	}
	return sWk;
}

enum EM_DIV_SEQ_STAT
{
	DIV_SEQ_NORMAL = 0,			// ����
	DIV_SEQ_WARNING,			// ����
	DIV_SEQ_ABNORMAL			// �ُ�
};
static CString GetSeqStat(EM_DIV_SEQ_STAT stat)
{
	CString	sWk;
	switch (stat) {
	case DIV_SEQ_NORMAL:	sWk = "����";		break;
	case DIV_SEQ_WARNING:	sWk = "����";		break;
	case DIV_SEQ_ABNORMAL:	sWk = "�ُ�";		break;
	default:				sWk = "�s��";		break;
	}
	return sWk;
}

enum EM_DIV_SEQ_KADOU
{
	DIV_SEQ_RESET = 0,			// ���Z�b�g�ς�
	DIV_SEQ_INIT,				// �������ς�
	DIV_SEQ_RESERVE,			// �\��
	DIV_SEQ_IMAGING				// �B����
};
static CString GetSeqKadou(EM_DIV_SEQ_KADOU stat)
{
	CString	sWk;
	switch (stat) {
	case DIV_SEQ_RESET:		sWk = "ؾ�čς�";	break;
	case DIV_SEQ_INIT:		sWk = "�������ς�";	break;
	case DIV_SEQ_RESERVE:	sWk = "�\��";		break;
	case DIV_SEQ_IMAGING:	sWk = "�B����";		break;
	default:				sWk = "�s��";		break;
	}
	return sWk;
}

enum DIV_CHILER_STOP
{
	DIV_CHILER_RUN = 0,			// �^�]��
	DIV_CHILER_STOPPED			// ��~��
};
static CString GetChilerStop(DIV_CHILER_STOP stat)
{
	CString	sWk;
	switch (stat) {
	case DIV_CHILER_RUN:	sWk = "�^�]��";		break;
	case DIV_CHILER_STOPPED:sWk = "��~��";		break;
	default:				sWk = "�s��";		break;
	}
	return sWk;
}

enum DIV_CHILER_ERR
{
	DIV_CHILER_NORMAL = 0,		// ����
	DIV_CHILER_ABNORMAL			// �ُ�
};
static CString GetChilerErr(DIV_CHILER_ERR stat)
{
	CString	sWk;
	switch (stat) {
	case DIV_CHILER_NORMAL:	sWk = "����";		break;
	case DIV_CHILER_ABNORMAL:sWk = "�ُ�";		break;
	default:				sWk = "�s��";		break;
	}
	return sWk;
}

enum DIV_DRYER_STOP
{
	DIV_DRYER_RUN = 0,			// �^�]��
	DIV_DRYER_STOPPED			// ��~��
};
static CString GetDryerStop(DIV_DRYER_STOP stat)
{
	CString	sWk;
	switch (stat) {
	case DIV_DRYER_RUN:		sWk = "�^�]��";		break;
	case DIV_DRYER_STOPPED:	sWk = "��~��";		break;
	default:				sWk = "�s��";		break;
	}
	return sWk;
}

enum DIV_DRYER_ERR
{
	DIV_DRYER_NORMAL = 0,		// ����
	DIV_DRYER_ABNORMAL			// �ُ�
};
static CString GetDryerErr(DIV_DRYER_ERR stat)
{
	CString	sWk;
	switch (stat) {
	case DIV_DRYER_NORMAL:	sWk = "����";		break;
	case DIV_DRYER_ABNORMAL:sWk = "�ُ�";		break;
	default:				sWk = "�s��";		break;
	}
	return sWk;
}

enum DIV_COOLER_STOP
{
	DIV_COOLER_RUN = 0,			// �^�]��
	DIV_COOLER_STOPPED			// ��~��
};
static CString GetCoolerStop(DIV_COOLER_STOP stat)
{
	CString	sWk;
	switch (stat) {
	case DIV_COOLER_RUN:	sWk = "�^�]��";		break;
	case DIV_COOLER_STOPPED:sWk = "��~��";		break;
	default:				sWk = "�s��";		break;
	}
	return sWk;
}

enum DIV_COOLER_ERR
{
	DIV_COOLER_NORMAL = 0,		// ����
	DIV_COOLER_ABNORMAL			// �ُ�
};
static CString GetCoolerErr(DIV_COOLER_ERR stat)
{
	CString	sWk;
	switch (stat) {
	case DIV_COOLER_NORMAL:	sWk = "����";		break;
	case DIV_COOLER_ABNORMAL:sWk= "�ُ�";		break;
	default:				sWk = "�s��";		break;
	}
	return sWk;
}

enum DIV_DOOR_STAT
{
	DIV_DOOR_CLOSE = 0,			// ���
	DIV_DOOR_OPEN				// �J���
};
static CString GetDoorStat(DIV_DOOR_STAT stat)
{
	CString	sWk;
	switch (stat) {
	case DIV_DOOR_CLOSE:	sWk = "���";		break;
	case DIV_DOOR_OPEN:		sWk = "�J���";		break;
	default:				sWk = "�s��";		break;
	}
	return sWk;
}

enum DIV_KEY_STAT
{
	DIV_KEY_LOCK = 0,			// ���
	DIV_KEY_UNLOCK				// �J���
};
static CString GetKeyStat(DIV_KEY_STAT stat)
{
	CString	sWk;
	switch (stat) {
	case DIV_KEY_LOCK:		sWk = "���";		break;
	case DIV_KEY_UNLOCK:	sWk = "�J���";		break;
	default:				sWk = "�s��";		break;
	}
	return sWk;
}

enum DIV_SPRAY_STAT
{
	DIV_NOT_BLOWING = 0,		// �����t
	DIV_BLOWING					// ���t��
};
static CString GetSprayStat(DIV_SPRAY_STAT stat)
{
	CString	sWk;
	switch (stat) {
	case DIV_NOT_BLOWING:	sWk = "�����t";		break;
	case DIV_BLOWING:		sWk = "���t��";		break;
	default:				sWk = "�s��";		break;
	}
	return sWk;
}

// �X�g�b�v�}�[�N�֌W
#define STOPMARK_INTERVAL_LEN		10000							// �X�g�b�v�}�[�N�A�����͖h�~���� [mm]

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// �ʐM��� �X�e�[�^�X��`
//=======================================
// ���b�Z�[�W�ʒm
//=======================================
//// �S��MainInstance�Ŏ󂯂�
//       �ꍇ�ɂ���ẮAMainInstance���e���[�J�[�X���b�h�ɂ��ꗬ������
enum ENUM_MAININSTANCE {
	E_DEF_ML_PARAM = WM_USER + 100,									// �p�����[�^�ύX�ʒm
	E_DEF_ML_START,													// �����J�n�v��
	E_DEF_ML_STOP,													// ������~�v��
	E_DEF_ML_WAIT_START_ANSWER,										// ����J�n�A���T�[
	E_DEF_ML_WAIT_STOP_ANSWER,										// �����~�A���T�[
	E_DEF_ML_STATE_ANSWER,											// �����Ԗ⍇���A���T�[
	E_DEF_ML_INITIAL,												// �C�j�V�����`����M
	E_DEF_ML_RECV_CUTAREA,											// �J�b�g�͈͏��ʒm
	E_DEF_ML_ALAMECANCEL,											// �x��o�͉����ʒm
	E_DEF_ML_MAE_WRITE,												// �O�H�������݊����ʒm
	E_DEF_ML_DB_ADD_SYNC,											// DB���ѕۑ��󋵖₢���킹�ʒm
	E_DEF_ML_LINKSW_ANSWER,											// �J���������N�X�C�b�`�ݒ芮���ʒm
	E_DEF_ML_LINKSW_JOIN_ANSWER,									// �r���Q���ݒ芮���ʒm
	E_DEF_ML_LINKSW_CAMSET_ANSWER,									// �J�����I��ݒ芮���ʒm
	E_DEF_ML_RESUME_START_ANSWER,									// �r���J�n�A���T�[
	E_DEF_ML_RESUME_STOP_ANSWER,									// �r����~�A���T�[
	E_DEF_ML_RESUME_START,											// �r���J�n�v��
	E_DEF_ML_RESUME_STOP,											// �r����~�v��
	E_DEF_END														// �I��
};

//=======================================
// �G�b�W�s�����R
//=======================================
enum EM_EDGE_ERR_REASON
{
	EDGE_NORMAL = 0x00,												// ����
	EDGE_ERR_LEFT = 0x01,											// ���G�b�W���o�͈͊O
	EDGE_ERR_RIGHT = 0x02,											// �E�G�b�W���o�͈͊O
	EDGE_ERR_WIDTH = 0x04,											// �����e�덷�͈͊O
	EDGE_ERR_PREFRAME = 0x08										// �O�t���[���Ƃ̋��e�덷�͈͊O
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �f�o�b�O�{�^�� ���j���[���X�g
// �����ɒ�`��ǉ�������AXX_SAMPEXDlg::OnNMClickListCmd2()����Switch���ɏ����ǉ�����
enum	ENUM_DEBUG_CMD {
	E_DEBUG_START,						// �����J�n
	E_DEBUG_STOP,						// ������~
	E_DEBUG_RESUME_START,				// �r���J�n
	E_DEBUG_RESUME_STOP,				// �r����~
	E_DEBUG_HT_CANNOT_CLEAR,			// ���葱�s�s�\����
	E_DEBUG_PARAM,						// �p�����[�^�擾
	E_DEBUG_CAMLINKSW_READY,			// �J���������N�X�C�b�`�ݒ芮��
	E_DEBUG_CAMLINKSW_CAMSET,			// �J�����I��ݒ芮��
	E_DEBUG_RESUME_START_CMPL,			// �r���Q���ݒ芮��
	E_DEBUG_CMD_END						// �I�[
};
static	CString	DebugCmd[E_DEBUG_CMD_END] = {
	"�����J�n"
	,"������~"
	,"�r���J�n"
	,"�r����~"
	,"���s�s�\����"
	,"�p�����[�^�擾"
	,"LINKSW�ݒ芮��"
	,"�J�����I��ݒ芮��"
	,"�r���Q���ݒ芮��"
};

// �����ɒ�`��ǉ�������AXX_SAMPEXDlg::OnNMClickListInput()����Switch���ɏ����ǉ�����
enum	ENUM_DEBUG_INPUT {
	E_DEBUG_RECV_COILINF_KEN,			// �[�������R�C����M
	E_DEBUG_RECV_COILINF_DSP,			// �[���\���R�C����M
	E_DEBUG_REGET_COILINF_KEN,			// �����p��� �Ď�M
	E_DEBUG_REGET_COILINF_DSP,			// �\���p��� �Ď�M
	E_DEBUG_PI_WPD_KEN_DM,				// PI ������_WPD���oDM�̂�		(WPD3)
	E_DEBUG_PI_WPD_DSP_DM,				// PI �\����_WPD���oDM�̂�		(WPD5)
	E_DEBUG_PI_SHAR_CUT,				// PI �V���[�J�b�g�M��
	E_DEBUG_PATO_RESET,					// PI �p�g���C�g���Z�b�g
	E_DEBUG_SEQ_CNT_RESET,				// PO ����V�[�P���T�J�E���^���Z�b�g
	E_DEBUG_INITIAL,					// �C�j�V�����`����M�ʒm
	E_DEBUG_VOICE_RESET_SETU,			// �{�C�X�x��o�͉����v���i�ݔ��ُ�x������j
	E_DEBUG_VOICE_RESET_DEF,			// �{�C�X�x��o�͉����v���i���׌��o�x������j
	E_DEBUG_PI_WPD_KEN,					// PI ������_WPD���o			(WPD3)
	E_DEBUG_PI_WPD_DSP,					// PI �\����_WPD���o			(WPD5)
	E_DEBUG_INPUT_END					// �I�[
};
static	CString	DebugInput[E_DEBUG_INPUT_END] = {
	"�[�������R�C����M"
	,"�[���\���R�C����M"
	,"�����p��� �Ď�M"
	,"�\���p��� �Ď�M"
	,"PI ������_WPD���oDM"
	,"PI �\����_WPD���oDM"
	,"PI �����ĐM��"
	,"PI ���ײ�ؾ������"
	,"PO SEQ����ؾ������"
	,"�Ƽ�ٓ`����M"
	,"�ݔ��ُ�x�����"
	,"���׌��o�x�����"
	,"PI ������_WPD���o"
	,"PI �\����_WPD���o"
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �ȒP�ȋ��ʊ֐�

//=======================================
// �r�o�̓t�@�C�����̎擾
//=======================================
static CString GetDefectDumpName(int nMen) {
	CString s;
	s.Format("Def_Men=%d", nMen);
	return s;
}

//==========================================
// ���l��������ϊ�
//------------------------------------------
// const long	val			: �ϊ��O�̐��l
// char*		result		: �ϊ���̕�����
// const long	size		: �ϊ���̕�����
// const bool	isLeftPad	: True=���l�� False=�E�l��
// const char	padding		: �󗓂𖄂߂镶��
// �߂�l		int			: 0=���� 
//==========================================
static int ItoA_Size(const long val, char* result, const long size, const bool isLeftPad, const char padding)
{
	char wk[256];					// �ϊ���̕�����
	char wkNum[256];				// �ϊ���̐��l
	int  nStartPos;					// ���l���Z�b�g����J�n�ʒu(0�I���W��)

	memset(wk, padding, 256);
	memset(wkNum, 0x00, 256);
	_itoa(val, wkNum, 10);

	// �������ڂ��琔�l���Z�b�g����H	
	if (isLeftPad)	nStartPos = 0;
	else			nStartPos = size - (int)strlen(wkNum);

	memcpy(&wk[nStartPos], wkNum, strlen(wkNum));

	memcpy(result, wk, size);

	return 0;
}


//==========================================
// �V�X�e�����擾
//==========================================
static const char* GetSystemName()
{
	return "5CGL�ɔ����Ӄ�";
}

//=======================================
// �ŏ��Ԋu ��\�r��� �\����
//=======================================
// �r���
struct TYP_COLROW_BUF
{
	bool					umu;									// �f�[�^�L��
	DETECT_BASE_DATA		Data;									// ���ʃ}�g��
	double					dLen;									// �r�����ʒu

	bool					bIsBmp;									// �摜��Bmp��True
	int						nImgsize[MAX_CAMANGLE];					// �摜�T�C�Y
	BYTE*					pImg[MAX_CAMANGLE];						// �摜�̈�
};
struct TYP_COLROW_INFO
{
	TYP_COLROW_BUF			BufDefCol[MAP_COL_NUM];					// ��\�r���
	TYP_COLROW_BUF			BufDefAry[MAP_COL_NUM][MAP_NEST_NUM];	// ���X�g�r���
	int						nSetCnt[MAP_COL_NUM];					// ���ۂ̃��X�g�̈�ɃZ�b�g�����r��
};
struct TYP_OP_SEND_DEFECT_INFO
{
	TYP_COLROW_INFO			ColnowBuf_Now;							// �J�����g�ŏ���Ԃ��r���
	TYP_COLROW_INFO			ColnowBuf_Next;							// ���̍ŏ���Ԃ��r���
};
