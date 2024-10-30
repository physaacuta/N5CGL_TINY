// *********************************************************************************
//	EPC�\�t�g �� ���� �� ���� �@�\ �֘A�w�b�_�[
//	[Ver]
//		Ver.01    2015/06/18  ���ō쐬
//
//	[����]
//		��{�I�ɁA����������
// *********************************************************************************

// �X�̃V�X�e���ŗL�̃C���N���[�h

#ifndef	_TOHTCOMMON_H
#define	_TOHTCOMMON_H

// �C���N���[�h
#include <windows.h>

#include "..\..\Include\LineCommon.h"									// �r���C���N���[�h
#include "..\..\Include\EPC\NCL2DETECT_if.h"							// I/F��`

//=======================================
// ����@�\ �S��
//=======================================
struct CAMPAIR_ITEM {													// �J�����y�A�Ɋ܂܂��J�����Z�b�g
	int				num;													// ���̔���@�\���S������J�����Z�b�g��
	int				camset[MAX_HT1_CAMSET];									// ���̔���@�\���S������J�����Z�b�g(0:����)
};

struct CAMPAIR_INF {													// �S�J�����y�A��`
	CAMPAIR_ITEM	item[NUM_CAMPAIR];										// (�e����@�\���S������J�����Z�b�g�Q) [�J�����y�A��]
	CAMPAIR_ITEM	item_cycle[NUM_CAMPAIR_CYCLE];							// (�e�����@�\���S������J�����Z�b�g�Q) [�J�����y�A��]
	CAMPAIR_ITEM	item_para[NUM_CAMPAIR_PARA];							// (�e���W�b�N�p�������@�\���S������J�����Z�b�g�Q) [�J�����y�A��]
};

//=======================================
// ���b�Z�[�W�ʒm
//=======================================
static CString GetDivCamPair(int pair)
{
#ifdef NUM_CAM_POS_1
	if     (1 == pair) {return "�\ C";}
	else if(2 == pair) {return "�� C";}
	else			   {return "�ޯ�����";}
#endif
#ifdef NUM_CAM_POS_2
	if     (1 == pair) {return "�\ DS";}
	else if(2 == pair) {return "�\ WS";}

	else if(3 == pair) {return "�� DS";}
	else if(4 == pair) {return "�� WS";}
	else			   {return "�ޯ�����";}
#endif
#ifdef NUM_CAM_POS_3
	if     (1 == pair) {return "�\ DS";}
	else if(2 == pair) {return "�\ C";}
	else if(3 == pair) {return "�\ WS";}

	else if(4 == pair) {return "�� DS";}
	else if(5 == pair) {return "�� C";}
	else if(6 == pair) {return "�� WS";}
	else			   {return "�ޯ�����";}
#endif
#ifdef NUM_CAM_POS_4
	if     (1 == pair) {return "�\ DS";}
	else if(2 == pair) {return "�\ D1";}
	else if(3 == pair) {return "�\ W1";}
	else if(4 == pair) {return "�\ WS";}

	else if(5 == pair) {return "�� DS";}
	else if(6 == pair) {return "�� D1";}
	else if(7 == pair) {return "�� W1";}
	else if(8 == pair) {return "�� WS";}
	else			   {return "�ޯ�����";}
#endif
#ifdef NUM_CAM_POS_5
	if     (1 == pair) {return "�\ DS";}
	else if(2 == pair) {return "�\ D1";}
	else if(3 == pair) {return "�\ C";}
	else if(4 == pair) {return "�\ W1";}
	else if(5 == pair) {return "�\ WS";}

	else if(6 == pair) {return "�� DS";}
	else if(7 == pair) {return "�� D1";}
	else if(8 == pair) {return "�� C";}
	else if(9 == pair) {return "�� W1";}
	else if(10 == pair) {return "�� WS";}
	else			   {return "�ޯ�����";}
#endif
#ifdef NUM_CAM_POS_6
	if     (1 == pair) {return "�\ DS";}
	else if(2 == pair) {return "�\ D1";}
	else if(3 == pair) {return "�\ D2";}
	else if(4 == pair) {return "�\ W2";}
	else if(5 == pair) {return "�\ W1";}
	else if(6 == pair) {return "�\ WS";}

	else if(7 == pair) {return "�� DS";}
	else if(8 == pair) {return "�� D1";}
	else if(9 == pair) {return "�� D2";}
	else if(10== pair) {return "�� W2";}
	else if(11== pair) {return "�� W1";}
	else if(12== pair) {return "�� WS";}
	else			   {return "�ޯ�����";}
#endif
#ifdef NUM_CAM_POS_7
	if     (1 == pair) {return "�\ DS";}
	else if(2 == pair) {return "�\ D1";}
	else if(3 == pair) {return "�\ D2";}
	else if(4 == pair) {return "�\ C";}
	else if(5 == pair) {return "�\ W2";}
	else if(6 == pair) {return "�\ W1";}
	else if(7 == pair) {return "�\ WS";}

	else if(8 == pair) {return "�� DS";}
	else if(9 == pair) {return "�� D1";}
	else if(10== pair) {return "�� D2";}
	else if(11== pair) {return "�� C";}
	else if(12== pair) {return "�� W2";}
	else if(13== pair) {return "�� W1";}
	else if(14== pair) {return "�� WS";}
	else			   {return "�ޯ�����";}
#endif
#ifdef NUM_CAM_POS_8
	if     (1 == pair) {return "�\ DS";}
	else if(2 == pair) {return "�\ D1";}
	else if(3 == pair) {return "�\ D2";}
	else if(4 == pair) {return "�\ D3";}
	else if(5 == pair) {return "�\ W3";}
	else if(6 == pair) {return "�\ W2";}
	else if(7 == pair) {return "�\ W1";}
	else if(8 == pair) {return "�\ WS";}

	else if(9 == pair) {return "�� DS";}
	else if(10== pair) {return "�� D1";}
	else if(11== pair) {return "�� D2";}
	else if(12== pair) {return "�� D3";}
	else if(13== pair) {return "�� W3";}
	else if(14== pair) {return "�� W2";}
	else if(15== pair) {return "�� W1";}
	else if(16== pair) {return "�� WS";}
	else			   {return "�ޯ�����";}
#endif
#ifdef NUM_CAM_POS_9
	if     (1 == pair) {return "�\ DS";}
	else if(2 == pair) {return "�\ D1";}
	else if(3 == pair) {return "�\ D2";}
	else if(4 == pair) {return "�\ D3";}
	else if(5 == pair) {return "�\ C";}
	else if(6 == pair) {return "�\ W3";}
	else if(7 == pair) {return "�\ W2";}
	else if(8 == pair) {return "�\ W1";}
	else if(9 == pair) {return "�\ WS";}

	else if(10== pair) {return "�� DS";}
	else if(11== pair) {return "�� D1";}
	else if(12== pair) {return "�� D2";}
	else if(13== pair) {return "�� D3";}
	else if(14== pair) {return "�� C";}
	else if(15== pair) {return "�� W3";}
	else if(16== pair) {return "�� W2";}
	else if(17== pair) {return "�� W1";}
	else if(18== pair) {return "�� WS";}
	else			   {return "�ޯ�����";}
#endif
#ifdef NUM_CAM_POS_10
	if     (1 == pair) {return "�\ DS";}
	else if(2 == pair) {return "�\ D1";}
	else if(3 == pair) {return "�\ D2";}
	else if(4 == pair) {return "�\ D3";}
	else if(5 == pair) {return "�\ D4";}
	else if(6 == pair) {return "�\ W4";}
	else if(7 == pair) {return "�\ W3";}
	else if(8 == pair) {return "�\ W2";}
	else if(9 == pair) {return "�\ W1";}
	else if(10== pair) {return "�\ WS";}

	else if(11== pair) {return "�� DS";}
	else if(12== pair) {return "�� D1";}
	else if(13== pair) {return "�� D2";}
	else if(14== pair) {return "�� D3";}
	else if(15== pair) {return "�� D4";}
	else if(16== pair) {return "�� W4";}
	else if(17== pair) {return "�� W3";}
	else if(18== pair) {return "�� W2";}
	else if(19== pair) {return "�� W1";}
	else if(20== pair) {return "�� WS";}
	else			   {return "�ޯ�����";}
#endif
#ifdef NUM_CAM_POS_11
	if     (1 == pair) {return "�\ DS";}
	else if(2 == pair) {return "�\ D1";}
	else if(3 == pair) {return "�\ D2";}
	else if(4 == pair) {return "�\ D3";}
	else if(5 == pair) {return "�\ D4";}
	else if(6 == pair) {return "�\ C";}
	else if(7 == pair) {return "�\ W4";}
	else if(8 == pair) {return "�\ W3";}
	else if(9 == pair) {return "�\ W2";}
	else if(10== pair) {return "�\ W1";}
	else if(11== pair) {return "�\ WS";}

	else if(12== pair) {return "�� DS";}
	else if(13== pair) {return "�� D1";}
	else if(14== pair) {return "�� D2";}
	else if(15== pair) {return "�� D3";}
	else if(16== pair) {return "�� D4";}
	else if(17== pair) {return "�� C";}
	else if(18== pair) {return "�� W4";}
	else if(19== pair) {return "�� W3";}
	else if(20== pair) {return "�� W2";}
	else if(21== pair) {return "�� W1";}
	else if(22== pair) {return "�� WS";}
	else			   {return "�ޯ�����";}
#endif
#ifdef NUM_CAM_POS_12
	if     (1 == pair) {return "�\ DS";}
	else if(2 == pair) {return "�\ D1";}
	else if(3 == pair) {return "�\ D2";}
	else if(4 == pair) {return "�\ D3";}
	else if(5 == pair) {return "�\ D4";}
	else if(6 == pair) {return "�\ D5";}
	else if(7 == pair) {return "�\ W5";}
	else if(8 == pair) {return "�\ W4";}
	else if(9 == pair) {return "�\ W3";}
	else if(10== pair) {return "�\ W2";}
	else if(11== pair) {return "�\ W1";}
	else if(12== pair) {return "�\ WS";}

	else if(13== pair) {return "�� DS";}
	else if(14== pair) {return "�� D1";}
	else if(15== pair) {return "�� D2";}
	else if(16== pair) {return "�� D3";}
	else if(17== pair) {return "�� D4";}
	else if(18== pair) {return "�� D5";}
	else if(19== pair) {return "�� W5";}
	else if(20== pair) {return "�� W4";}
	else if(21== pair) {return "�� W3";}
	else if(22== pair) {return "�� W2";}
	else if(23== pair) {return "�� W1";}
	else if(24== pair) {return "�� WS";}
	else			   {return "�ޯ�����";}
#endif
#ifdef NUM_CAM_POS_13
	if     (1 == pair) {return "�\ DS";}
	else if(2 == pair) {return "�\ D1";}
	else if(3 == pair) {return "�\ D2";}
	else if(4 == pair) {return "�\ D3";}
	else if(5 == pair) {return "�\ D4";}
	else if(6 == pair) {return "�\ D5";}
	else if(7 == pair) {return "�\ C";}
	else if(8 == pair) {return "�\ W5";}
	else if(9 == pair) {return "�\ W4";}
	else if(10== pair) {return "�\ W3";}
	else if(11== pair) {return "�\ W2";}
	else if(12== pair) {return "�\ W1";}
	else if(13== pair) {return "�\ WS";}

	else if(14== pair) {return "�� DS";}
	else if(15== pair) {return "�� D1";}
	else if(16== pair) {return "�� D2";}
	else if(17== pair) {return "�� D3";}
	else if(18== pair) {return "�� D4";}
	else if(19== pair) {return "�� D5";}
	else if(20== pair) {return "�� C";}
	else if(21== pair) {return "�� W5";}
	else if(22== pair) {return "�� W4";}
	else if(23== pair) {return "�� W3";}
	else if(24== pair) {return "�� W2";}
	else if(25== pair) {return "�� W1";}
	else if(26== pair) {return "�� WS";}
	else			   {return "�ޯ�����";}
#endif
#ifdef NUM_CAM_POS_14
	if     (1 == pair) {return "�\ DS";}
	else if(2 == pair) {return "�\ D1";}
	else if(3 == pair) {return "�\ D2";}
	else if(4 == pair) {return "�\ D3";}
	else if(5 == pair) {return "�\ D4";}
	else if(6 == pair) {return "�\ D5";}
	else if(7 == pair) {return "�\ D6";}
	else if(8 == pair) {return "�\ W6";}
	else if(9 == pair) {return "�\ W5";}
	else if(10== pair) {return "�\ W4";}
	else if(11== pair) {return "�\ W3";}
	else if(12== pair) {return "�\ W2";}
	else if(13== pair) {return "�\ W1";}
	else if(14== pair) {return "�\ WS";}

	else if(15== pair) {return "�� DS";}
	else if(16== pair) {return "�� D1";}
	else if(17== pair) {return "�� D2";}
	else if(18== pair) {return "�� D3";}
	else if(19== pair) {return "�� D4";}
	else if(20== pair) {return "�� D5";}
	else if(21== pair) {return "�� D6";}
	else if(22== pair) {return "�� W6";}
	else if(23== pair) {return "�� W5";}
	else if(24== pair) {return "�� W4";}
	else if(25== pair) {return "�� W3";}
	else if(26== pair) {return "�� W2";}
	else if(27== pair) {return "�� W1";}
	else if(28== pair) {return "�� WS";}
	else			   {return "�ޯ�����";}
#endif
#ifdef NUM_CAM_POS_15
	if     (1 == pair) {return "�\ DS";}
	else if(2 == pair) {return "�\ D1";}
	else if(3 == pair) {return "�\ D2";}
	else if(4 == pair) {return "�\ D3";}
	else if(5 == pair) {return "�\ D4";}
	else if(6 == pair) {return "�\ D5";}
	else if(7 == pair) {return "�\ D6";}
	else if(8 == pair) {return "�\ C";}
	else if(9 == pair) {return "�\ W6";}
	else if(10== pair) {return "�\ W5";}
	else if(11== pair) {return "�\ W4";}
	else if(12== pair) {return "�\ W3";}
	else if(13== pair) {return "�\ W2";}
	else if(14== pair) {return "�\ W1";}
	else if(15== pair) {return "�\ WS";}

	else if(16== pair) {return "�� DS";}
	else if(17== pair) {return "�� D1";}
	else if(18== pair) {return "�� D2";}
	else if(19== pair) {return "�� D3";}
	else if(20== pair) {return "�� D4";}
	else if(21== pair) {return "�� D5";}
	else if(22== pair) {return "�� D6";}
	else if(23== pair) {return "�� C";}
	else if(24== pair) {return "�� W6";}
	else if(25== pair) {return "�� W5";}
	else if(26== pair) {return "�� W4";}
	else if(27== pair) {return "�� W3";}
	else if(28== pair) {return "�� W2";}
	else if(29== pair) {return "�� W1";}
	else if(30== pair) {return "�� WS";}
	else			   {return "�ޯ�����";}
#endif
#ifdef NUM_CAM_POS_16
	if     (1 == pair) {return "�\ DS";}
	else if(2 == pair) {return "�\ D1";}
	else if(3 == pair) {return "�\ D2";}
	else if(4 == pair) {return "�\ D3";}
	else if(5 == pair) {return "�\ D4";}
	else if(6 == pair) {return "�\ D5";}
	else if(7 == pair) {return "�\ D6";}
	else if(8 == pair) {return "�\ D7";}
	else if(9 == pair) {return "�\ W7";}
	else if(10== pair) {return "�\ W6";}
	else if(11== pair) {return "�\ W5";}
	else if(12== pair) {return "�\ W4";}
	else if(13== pair) {return "�\ W3";}
	else if(14== pair) {return "�\ W2";}
	else if(15== pair) {return "�\ W1";}
	else if(16== pair) {return "�\ WS";}

	else if(17== pair) {return "�� DS";}
	else if(18== pair) {return "�� D1";}
	else if(19== pair) {return "�� D2";}
	else if(20== pair) {return "�� D3";}
	else if(21== pair) {return "�� D4";}
	else if(22== pair) {return "�� D5";}
	else if(23== pair) {return "�� D6";}
	else if(24== pair) {return "�� D7";}
	else if(25== pair) {return "�� W7";}
	else if(26== pair) {return "�� W6";}
	else if(27== pair) {return "�� W5";}
	else if(28== pair) {return "�� W4";}
	else if(29== pair) {return "�� W3";}
	else if(30== pair) {return "�� W2";}
	else if(31== pair) {return "�� W1";}
	else if(32== pair) {return "�� WS";}
	else			   {return "�ޯ�����";}
#endif

}

//=======================================
// �V�X�e����� ����
//=======================================
static CString GetDivSys(int em)   {return GetDivSys((EM_DIV_SYS)em); };	// �V�X�e�����
static CString GetDivSys(EM_DIV_SYS em) 
{
	CString wk;
	if		(DIV_SYS_INIT == em)				{ wk = "��������"; }
	else if (DIV_SYS_IDLE == em)				{ wk = "��������"; }
	else if (DIV_SYS_INITSTART == em)			{ wk = "�����J�n��������"; }
	else if (DIV_SYS_WAITSTART == em)			{ wk = "�����J�n�҂�";}
	else if (DIV_SYS_RUN == em)					{ wk = "������";}
	else if (DIV_SYS_WAITSTOP == em)			{ wk = "������~�҂�"; }
	else if (DIV_SYS_STOP == em)				{ wk = "������~";}
	else if (DIV_SYS_RETRY == em)				{ wk = "���������ċN����";}
	else										{ wk.Format("�X�e�[�^�X�ُ� <%d>", em);}
	return wk;
}

//=======================================
// ��EPC�� �摜�����^�X�N �p
//=======================================
// �摜�����^�X�N�֘A
//#define EPC_PIPE_COMMAND		"\\\\.\\pipe\\nccntrl"						// �R�}���h�p�p�C�v
//#define EPC_PIPE_DATA			"\\\\.\\pipe\\ncdata"						// �f�[�^�p�p�C�v
//#define EPC_SMEM_DATA			"ncsmem"									// �f�[�^�p���L������
#define EPC_PIPE_COMMAND		name_cpipe									// �R�}���h�p�p�C�v
#define EPC_PIPE_DATA			name_rpipe									// �f�[�^�p�p�C�v
#define EPC_SMEM_DATA			name_shmem									// �f�[�^�p���L������
#define EPC_ODBC_FORMAT			"dsn=%s;uid=%s;pwd=%s;"						// ODBC�ڑ��p�t�H�[�}�b�g(ODBC����, �ڑ����[�U�[��, �ڑ��p�X���[�h)

// �����@�\
#define USE_PD_EXT_CTRL														// NCspeedPD�̎����@�\�ŕK�v�ׁ̈A��`��������

// iPortViewer �֘A
#define EPC_MAIL_CAMCONT		"camcont"									// iPortViewer����̃��[���X���b�g����
#define EPC_MAIL_IPORTVIEWER	"iPortViewer"								// iPortViewer�̃��[���X���b�g����
#define FACT_EPC_IPORTVIEWER	6000										// iPortViewer����̃��[���X���b�g

//=======================================
// UDP�ݒ�
//		������́AZZ_EPC�p
//=======================================
#ifndef LOCAL
	#define EPC_UDP_IP				"192.9.200.255"							// �}���`�L���X�gIP
#else
	#define EPC_UDP_IP				"192.9.200.255"							// �}���`�L���X�gIP
#endif
#define EPC_UDP_IP_EDGE				"192.9.200.255"								// �}���`�L���X�gIP


#ifndef LOCAL
  //#define EPC_UDP_PORT			8003										// UDP�|�[�g
  #define EPC_UDP_PORT			8143										// UDP�|�[�g
#else
  #define EPC_UDP_PORT			8143										// �J�����Ńe�X�g����Ƃ��ɂ�������Ȃ��悤��
#endif
#define EPC_UDP_PORT_EDGE		8902										// ��ZZ_EPC�p UDP�|�[�g(PARAM_COMMON��EdgeRestrictPort�Ɠ��l�ɂ��Ă�������) 

#define EPC_UDP_PORT_ALLFRAME	8100										// UDP�|�[�g(���R�[�_PC�p)


// ZZ_EPC�p�̑��xUDP�B�������M������A1�t���[���̌��ʂ�Ԃ�
//   ���G���W�����ɂ͑��M���Ȃ�����
struct DEBUG_SPEED_INFO {
	DWORD	framenum;		// �t���[���ԍ�
	float	vRes;			// �c����\(mm/pixel)
};



////=======================================
//// ���xUDP�ɂ̂��� ���������p�t���O (ToVal)
////=======================================
//#define TOVAL_TOP_COILCHANGE		0										// �\�� �R�C���؂�ւ��t���O
//#define TOVAL_BOT_COILCHANGE		1 										// ���� �R�C���؂�ւ��t���O
//#define TOVAL_FCUT_COILOUTCHANGE	2										// �o�������R�C�� �t�@�[�X�g�J�b�g �؂�ւ��t���O
//#define TOVAL_LCUT_COILOUTCHANGE	3										// �o�������R�C�� ���X�g�J�b�g �؂�ւ��t���O
//
//#define TOVAL_REC_PROCTIME			4										// 1�t���[���捞��������QueryPerformanceCounter
//
//#define TOVAL_SET_FRAMENO			6 										// �G�b�W���o���ʎ擾�t���[��No (�\/����2�g�p)

//=======================================
// TEX�g�p ������ID
//=======================================
#define EPC_ATTR_BASE_MERGE				255									// �}�[�W���� (0:�}�[�W�ΏۊO 1:�}�[�W�Ώ�)
#define EPC_ATTR_BASE_KIZUNO			254									// �rNO���[�N�@(�R�C���P�ʁA�\����1�`�̘A��)

#define EPC_ATTR_BASE_FRAME_DEFECT_MAX	253									// 1�t���[�����ōő��r (0:�ő��r����Ȃ� 1:�ő��r)
#define EPC_ATTR_BASE_YU_G				252									// �O���[�h�D��x (�}�[�W�ŕK�{)
#define EPC_ATTR_BASE_YU_T				251									// �r��D��x (�}�[�W�ŕK�{)
#define EPC_ATTR_BASE_RANK_G			250									// ���׃O���[�h (0:���Q 1:�y���� 2:������ 3:�d����)

#define EPC_ATTR_BASE_HTTO_PROC_TIME	249									// �摜�捞�`���聨�����`�������܂ł̏��v���� [msec]
#define EPC_ATTR_BASE_TOTAL_PROC_TIME	248									// �摜�捞�`�������������܂ł̏��v���� [msec]
#define EPC_ATTR_BASE_HT_PROC_TIME		247									// �摜�����G���W�����������`���聨�����`���J�n�܂ł̏��v���� [msec]
#define EPC_ATTR_BASE_TR1_PROC_TIME		246									// DS(WS)���r����M�J�n�`DS(WS)���r����M�J�n�܂ł̏��v���� [msec]
#define EPC_ATTR_BASE_TR2_PROC_TIME		245									// DS(WS)���r����M�J�n�`WS(DS)���r����M�����܂ł̏��v���� [msec]
#define	EPC_ATTR_BASE_NC_PROC_TIME		244									// �摜�捞�`�摜�����G���W�����������܂ł̏��v���� [msec]

#define EPC_ATTR_BASE_IMG_CULL_X		243									// �S���摜�Ԉ�������
#define EPC_ATTR_BASE_IMG_CULL_Y		242									// �S���摜�Ԉ������c


//=======================================
// ���聨���� �؂�o���摜JPEG���̏ꍇ�̍\����
//=======================================
// �؂�o���摜�z��̗v�f(����Ǘ��Ŏg�p)
struct DEFECT_HEAD_IMAGE {
	DWORD					nWidth;											// �؂�o���摜��
	DWORD					nHeight;										// �؂�o���摜����
	float					fShrinkRatio;									// �摜�̈��k��
	DWORD					nSize[MAX_IMAGE_CNT];							// Jpeg�T�C�Y
	DWORD					nImgOffset[MAX_IMAGE_CNT];						// �摜�̐擪�ʒu(�{�\���̐擪����̉摜�擪�I�t�Z�b�g)
	DWORD					nNextImgOffset;									// �����̉摜+�s�b�`���܂߂��T�C�Y(=�{�\���̐擪���玟�̉摜�z��܂ł̃I�t�Z�b�g�ƂȂ�) (jepg��8�̔{���łȂ��ƃ_��������) (�Ō��0)
	// ���ۂɂ́A���̌�� nSize���̉摜������
};

// �؂�o���摜�̎���(�����Ǘ��Ŏg�p)
struct DEFECT_DATA_IMAGE {
	bool					bIsBmp;											// �摜��Bmp��True
	DWORD					nWidth;											// �؂�o���摜��
	DWORD					nHeight;										// �؂�o���摜����
	DWORD					nSize[MAX_IMAGE_CNT];							// Jpeg�T�C�Y
	BYTE*					pImg[MAX_IMAGE_CNT];							// �摜�̐擪�ʒu
};

//=======================================
// �����ʗp �R�C�����
//=======================================
#pragma pack(1)
struct TYPE_EPC_COIL			// 212 byte  (max240byte�܂�) �� ����ȏ�̏ꍇ�́A�s�v�ȍ��ڂ��폜
{
	char			cMeiNo[4];												// [01] �����_���R�C�� ����No
	char			cCoilNo[12];											// [02] �����_���R�C�� ���YNo
	long			nSendPoint;												// [03] ���M�ʒu (0:WPD3 1:WPD4 2:WPD6-50m)
	long			nLenIn;													// [04] �����_���R�C�� ���� [m]	
	long			nAtuZai;												// [05] �ޗ��� [��m] (�o��)
	long			nWidZai;												// [06] �ޗ��� [mm]
	long			nAtuSei;												// [07] ������ [��m] (�o��)
	long			nWidSei;												// [08] ������ [mm]
	long			nWidTrim;												// [09] ���H���g������ [mm] (�m�[�g�������́A0)
	long			nNobiSPM;												// [10] SPM�L�ї� [10-2%]
	long			nNobiTLV;												// [11] TLV�L�ї� [10-2%]
	long			nCutF;													// [12] �����ɂ�����t�����g�[�J�b�g�̎��ђ�
	long			nCutT;													// [13] �����ɂ�����e�[���[�J�b�g�̎��ђ�
	long			nSPMRoolKei_Up;											// [14] SPM���[�N���[���a (��) [10-1mm]
	long			nSPMRoolKei_Down;										// [15] SPM���[�N���[���a (��) [10-1mm]
	long			nYobi[10];												// [16-25] int�\��10�`1

	char			cKenKijun_O[4];											// [26] ����� �\
	char			cKenKijun_U[4];											// [27] ����� ��
	char			cKurometo_Kind[2];										// [28] �N�����[�g ���ю��
	char			cKurometo_Umu[2];										// [29] �N�����[�g ���їL��
	char			cHinCode[1];											// [30] �i��
	char			cMekkiKind[1];											// [31] ���b�L���
	char			cSyuZai[2];												// [32] ��ގ�
	char			cDifMaker[1];											// [33] �f�B�t�@�����V�����}�[�J�[
	char			cKenban[1];												// [34] ���敪
	char			cMekkiCycle[1];											// [35] ���b�L�T�C�N��
	char			cInLineSPM[1];											// [36] ���x���[ �C�����C��SPM
	char			cKagakuToyu[1];											// [37] ���w�����h��
	char			cAtukkaSPM[1];											// [38] �I�t���C��SPM������
	char			cMekkiKubun[1];											// [39] ���b�L�敪
	char			cOmoteSyori1[1];										// [40] �\�ʏ���1����
	char			cOmoteSyori2[1];										// [41] �\�ʏ���2����
	char			cToyu[1];												// [42] �h��
	char			cKouteiYobi[1];											// [43] �H���L���\��
	char			cBaund[1];												// [44] �o�E���_�����킹
	char			cCaplSyuZai[10];										// [45] CAPL ��ގ�
	char			cMetuke[8];												// [46] �ڕt�K�i
	char			cNi[2];													// [47] Ni�t����
	char			cSodoTCM[2];											// [48] TCM���[���e�x
	char			cSpmUmu[2];												// [49] SPM�����L�� (�L��:O  ����:X)
	char			cNextLine[2];											// [50] �v�掟�H���R�[�h
	char			cMuke[32];												// [51] ����
	char			cYouto[4];												// [52] �p�r
	char			cCaplSiage[4];											// [53] CAPL�\�ʎd�グ
	char			cCaplSanUmu[2];											// [54] CAPL�_���L��
	char			cHinMode[1];											// [55] �i��敪
	char			cGaCrKbn[1];											// [56] GA/CR�敪
	char			cSpmJiseki[2];											// [57] SPM���їL��
	char			cKouteiCode[2];											// [58] �H���R�[�h
	char			cYobi[2];												// [59] �\��
};

//=======================================
// �����R�C����񑗐M�Ή�
//		�������M���́AFACT_HT_DEFECT_14 ���g������

#define DIVCOIL_ONE_SIZE_SEND		220												// �������M���A��x�ɑ���o�C�g��
#define DIVCOIL_MAX_DIV_CNT			10												// �ő啪����
#define DIVCOIL_MAX_SEND_SIZE		(DIVCOIL_ONE_SIZE_SEND * DIVCOIL_MAX_DIV_CNT)	// �ő呗�M�T�C�Y

//// ��������M�p�\����
struct DIVCOIL_MERGED_PCINFO {
//	int		nDlyKey;																// �x�����s�L�[
	int		nDivCnt;																// ������
	bool	bGetFlg[DIVCOIL_ONE_SIZE_SEND];											// �����f�[�^�̎�M�t���O
	int		nSize;																	// ���v�̃T�C�Y(�f�[�^����M���邲�Ƃɉ��Z)
	BYTE	bPcInfo[DIVCOIL_MAX_SEND_SIZE];											// ��ʏ��
};


#pragma pack()

#endif
