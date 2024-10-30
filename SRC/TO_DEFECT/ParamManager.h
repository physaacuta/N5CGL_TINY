#pragma once

// ���Œ�
#include "..\..\Include\LineCommon.h"									// �ӃփC���N���[�h

// �ӃփN���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLib\Class\AutoLock.h"						// �N���e�B�J���Z�N�V��������N���X

// �Ӄփ��C�u���� �C���N���[�h
#include "..\..\Library\KizuODBC\OdbcBase.h"

// �t�@���N�V�����N���X
#include "ToOutMailSender.h"


#include "MainConst.h"



using namespace KizuLib;
using namespace KizuODBC;

class ParamManager :	public ThreadManager
{
public:
	ParamManager(void);
	virtual ~ParamManager(void);

private:
	int		ThreadFirst();												// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int		ThreadLast();												// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void	ThreadEvent(int nEventNo);									// �X���b�h�C�x���g����



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �p�����[�^�p�\����
public:
	//=============================
	// �Ӄ֒萔�@�\����
	//=============================
	struct P_COMMON_MAST
	{
		// RAS�֌W
		struct _RAS {
			int	nColorNorm;								// [RAS����F] RAS�\����ʂ̋@�퐳�펞�̐F �iARGB�`���j
			int	nColorErr;								// [RAS�ُ�F] RAS�\����ʂ̋@��ُ펞�̐F �iARGB�`���j
			int	nColorWarn;								// [RAS���ӐF] RAS�\����ʂ̋@�풍�ӎ��̐F �iARGB�`���j
		} Ras;

		// LOG�֌W
		struct _LOG {
			int	nDispNum;								// [LOG�\������] ���O�\����ʂ̍ő働�O�\������ �i�����܂Ńf�t�H���g�l�j
			int	nEventColor;							// [LOG�C�x���g�F] ���O�\����ʂ̃C�x���g�s�̕\�������F �iARGB�`���j
			int	nFailColor_Ju;							// [LOG�d�̏�F] ���O�\����ʂ̏d�̏�s�̕\�������F �iARGB�`���j
			int	nFailColor_Kei;							// [LOG�y�̏�F] ���O�\����ʂ̌y�̏�s�̕\�������F �iARGB�`���j
		} Log;

		// �T����ʕ\��
		struct _TANSHO {
			int	nDspSectionMin;							// [�T����ʕ\����ԏ�] �T����ʂ̕\����Ԑݒ�l �i�ŏ�5m�̔{�����Z�b�g���鎖�j
			int	nDspSectionMax;							// [�T����ʕ\����ԑ�] �T����ʂ̕\����Ԑݒ�l �i�ŏ�5m�̔{�����Z�b�g���鎖�j
		} Tansho;

		int		walm;									// [�E�H�[�}�ގw��ʒu] �����p�^�[���ł̃E�H�[�}�ސݒ背�R�[�h�ʒu																											
		int		nJpegQuality;							// [Jpeg�؂�o���摜�i��] �؂�o���摜��JPEG���k�i��[%]
		int		nCamTempThr;							// [�J�������x臒l] �J�������x�ُ�ƂȂ�臒l[��]
		
		// �G�b�W�������`�F�b�N
		struct _EDGE{
			int		nType;								// [�G�b�W������_���{���] 0:����   1�F����������̂�  2�F����������{�G�b�W�͈͎w��
			float	dDetectRange;						// [�G�b�W������_���o�͈�] ���̒l��1/2�̈ʒu����_�ɁA�O��͈͂Ƃ���[mm]
			float	dDetectRangeR;						// [�G�b�W������_���J�o���[���o�͈�] ���̒l��1/2�̈ʒu����_�ɁA�O��͈͂Ƃ���[mm]
			float	dWidthDiff;							// [�G�b�W������_�����e�덷] ��ʏ��̔��ƌ������u���̋��e�덷[mm]
			float	dPerEdgeDiff;						// [�G�b�W������_�O�t���[���Ƃ̋��e�덷] �ЃG�b�W�D�掞�́A�O�G�b�W�ʒu�ƍ���G�b�W�ʒu�̋��e�덷[mm]
			int		nFromSkip;							// [�G�b�W������_�n�ڌ�s����] �R�C���ؑ֌�A���̃t���[�������̊Ԃ́A�G�b�W��������������{���Ȃ�[�t���[��]
			int		nFFErr;								// [�G�b�W������_�G�b�W���o�ُ�FF���M] ����臒l�ȏ�̏ꍇ�AFF�ɃG�b�W�����o��ԗL��ő��M�B(0�F���M��) [TO_DEFECT]
		} Edge;

		// �G�b�W���o
		struct _EDGE_RESTRICT{
			int		nType;								// [�G�b�W���o_���{���] 0:����   1�F�����G�b�W�͈͎w��  2�F�G�b�W���o��͈͎w��
			float	dDetectRange;						// [�G�b�W���o_���o�͈�] ���̒l��1/2�̈ʒu����_�ɁA�O��͈͂Ƃ���[mm]
		} Edge_Restrict;

		// �d���r�}�b�v
		struct _DEF_MAP {
			int		nBGColor_Kei;						// [�d���r�}�b�v�w�i�F_�y] �T����ʁE�ʔ��щ�� �d���r�}�b�v �y�w�i�F �iARGB�`���j
			int		nBGColor_Tyu;						// [�d���r�}�b�v�w�i�F_��] �T����ʁE�ʔ��щ�� �d���r�}�b�v ���w�i�F �iARGB�`���j
			int		nBGColor_Ju;						// [�d���r�}�b�v�w�i�F_�d] �T����ʁE�ʔ��щ�� �d���r�}�b�v �d�w�i�F �iARGB�`���j
			int		nDefCntThr_Kei;						// [�d���r�}�b�v�r��臒l_�y] �T����ʁE�ʔ��щ�� �d���r�}�b�v���r�� �y臒l
			int		nDefCntThr_Tyu;						// [�d���r�}�b�v�r��臒l_��] �T����ʁE�ʔ��щ�� �d���r�}�b�v���r�� ��臒l
			int		nDefCntThr_Ju;						// [�d���r�}�b�v�r��臒l_�d] �T����ʁE�ʔ��щ�� �d���r�}�b�v���r�� �d臒l
		} DefMap;
			
		// �ݔ���
		struct _DISTANCE{
			int		nKenBase;							// [�ݔ���_������__�㗬�J����]	������_�`�㗬�J�����܂ł̋���[mm]
			int		nKenBaseSpm;						// [�ݔ���_������__SPM] ������_�`SPM�܂ł̋���[mm]
			int		nDspBaseKensaBot;					// [�ݔ���_�\����__��������] �\����_�`��������ʒu�܂ł̋���[mm]
			int		nDspBaseKensaTop;					// [�ݔ���_�\����__�\������] �\����_�`�\������ʒu�܂ł̋���[mm]
			int		nDspSharCut;						// [�ݔ���_�\����__�V���[] �\����_�`�o���V���[�J�b�g�ʒu�܂ł̋���[mm]
			int		nKenBaseGate;						// [�ݔ���_������_�Q�[�g�M��ON_������_] ������_�Q�[�g�M��ON�`������_WPD�܂ł̋���[mm]
			int		nDspBaseGate;						// [�ݔ���_�\����_�Q�[�g�M��ON_�\����_] �\����_�Q�[�g�M��ON�`�\����_WPD�܂ł̋���[mm]
		} Dist;

		int			nNoLamp[2];							// [�r�ڋ߃����v_�s����] �n�ړ_�O��̕s���ѐݒ�B[��[/���[]
		int			nKizuLamp[2];						// [�r�ڋ߃����v_�o�͋��] �r�ڋ߃����v�o�͋�ԁB�p�g���C�g�o�͊J�n�ʒu�B[�\�������OXm�B/�\������ʉߌ�Xm�B]
		int			nNoAlarm[2];						// [�r�ڋ߃A���[��_�s����] �n�ړ_�O��̕s���ѐݒ�B[��[/���[]
		int			nKizuAlarm[2];						// [�r�ڋ߃A���[��_�o�͋��] �r�ڋ߃A���[���o�͋�ԁB�p�g���C�g�o�͊J�n�ʒu�B[���������OXm�B/��������ʉߌ�Xm�B]
		int			nVoicePos;							// [�{�C�X_�o�͈ʒu_�O] �{�C�X�o�͈ʒu�B�\�������OXm�B

		int			nRPISaveColor_R;					// [�S���摜_�ۑ�_R] �S���摜��ۑ����邩�H  0:���� 1:�L��  �������ꍇ�A���聨�S�������Ԃ̃f�[�^������Ȃ�
		int			nRPISaveColor_G;					// [�S���摜_�ۑ�_G] �S���摜��ۑ����邩�H  0:���� 1:�L��  �������ꍇ�A���聨�S�������Ԃ̃f�[�^������Ȃ�
		int			nRPISaveColor_B;					// [�S���摜_�ۑ�_B] �S���摜��ۑ����邩�H  0:���� 1:�L��  �������ꍇ�A���聨�S�������Ԃ̃f�[�^������Ȃ�
		int			nRPISaveColor_Sa;					// [�S���摜_�ۑ�_��] �S���摜��ۑ����邩�H  0:���� 1:�L��  �������ꍇ�A���聨�S�������Ԃ̃f�[�^������Ȃ�

		float		dPlgDiffLimit[2];					// [PLG�����ُ����l] �����pPLG�Ɣ�r�pPLG���r���āA�ُ�F���ƂȂ�ő�덷����[��r�p/�\���p]
		float		dProLenDiffLimit;					// [�v���R���R�C������r�ُ����l] �����������u�ƃv���R���̃R�C�����iSPM������j���r���āA�ُ�F���ƂȂ�ő�덷�����i0�Ȃ�`�F�b�N�Ȃ��j
		int			nLogiParaCam[2][2];					// [���W�b�N�p�������@�\xx_���̓J����xx] �Ώۂ̃��W�b�N�p�������@�\�Ɋ���t���Ă���J����No�i0�F���I���@1�`�F�J����No�j
	};


	//=============================
	// �r��@�\����
	//=============================
	struct P_TYPE_NAME
	{
		int			num;								// ����		
		struct _TYPE_ID
		{
			int		nEdasysID;							// [�r��EdasysID]
			int		nPriority;							// [�D��x]
			
			bool bEnable[MAX_IMAGE_CNT];				// �摜���̕\���L��
		} Type_Id[MAX_TYPE];
	};


	//=============================
	// �O���[�h�@�\����
	//=============================
	struct P_GRADE_NAME
	{
		int			num;								// ����
		struct _GRADE_ID
		{
			int		nEdasysID;							// [�O���[�hEdasysID]
			int		nPriority;							// [�D��x]
			int		rank;								// [���׃O���[�h] (0:���Q 1:�y���� 2:������ 3:�d����)
		} Grade_Id[MAX_GRADE];
	};


	//=============================
	// �����\�ʏ�ԁ@�\����
	//=============================
	struct P_SCODE_OUTPUT
	{
		// �o�͏���
		struct OUTPUT									// �o�͍���
		{
			int		edasysID;							// [�r��edasysID] (1�`:�r��ID  0:���g�p  -1:���̑�����)
			int		dummy;								// �_�~�[�o��
		};

		// ����
		struct INF
		{
			// �}�X�^�[
			BOOL	umu;								// [�L������] (true:�g�p false:���g�p)				

			//// �o�͐ݒ�
			//int		nPutNum;						// output�ɃZ�b�g���Ă��鑍��
			//OUTPUT	output[MAX_TYPE];				// �o�͏����ݒ�
			//OUTPUT	defput;							// �r�햢�ݒ荀�ڂ̏o�͏���

		} rec[MAX_SCODE];								// �ŏI�́A���̑����ڌŒ�
	};


	//=======================================
	// �����p�^�[���� �\����
	//=======================================
	enum EM_OUTPUTFLG									// �o�͏���		(�D��x��������̂́A�K���D��x�̍������ɕ��ׂĂ�����)
	{
		OUTPUTFLG_DSP_O = 0,							// �T����ʕ\��	�\
		OUTPUTFLG_DSP_U,								//				��

		OUTPUTFLG_DSP_DEFMAP,							// �r�}�b�v��ʕ\��

		OUTPUTFLG_FF_SEND,								// FF���M�L��

		OUTPUTFLG_ALARM_RANP_LOW,						// �r�ڋ�	�y
		OUTPUTFLG_ALARM_RANP_HIGH,						//			�d
		
		OUTPUTFLG_PATOLOGHT,							// �p�g���C�g�o��

		OUTPUTFLG_VOICE_SEQ,							// �{�C�X�A���r
		OUTPUTFLG_VOICE_HIGH,							// �{�C�X�d���r
		OUTPUTFLG_VOICE_HEGE,							// �{�C�X�w�Q
		OUTPUTFLG_VOICE_SURI,							// �{�C�X�X���o�[
		OUTPUTFLG_VOICE_SAME,							// �{�C�X�\�������r
		OUTPUTFLG_VOICE_CYCLE,							// �{�C�X�����r
		OUTPUTFLG_VOICE_OTHER,							// �{�C�X���̑��r
		
		OUTPUTFLG_LOWGR_DB_O,							// DB�ۑ�_���	�\
		OUTPUTFLG_LOWGR_DB_U,							//			��
		OUTPUTFLG_DB_O,									// DB�ۑ�_�D��	�\ 
		OUTPUTFLG_DB_U,									//				��


		OUTPUTFLG_END									//(�Ō�Ƃ��Ă��g���Ă���̂Œ���)
	};

	struct P_PCODE_OUTPUT
	{
		// �o�͏���
		struct OUTPUT							// �o�͍���
		{
			int		edasysID;							// �r��edasysID (1�`:�r��ID  0:���g�p  -1:���̑�����)
			
			int		kind[OUTPUTFLG_END];				// �O���[�hID (0:���� 1�`:�O���[�hedasysID) �w�肳�ꂽ�O���[�hID�ȏ���r���o�͑Ώ�
														// �z��ʒu�́A[EM_OUTPUTFLG] ���g�p���鎖
		};

		// ����
		struct INF
		{
			// PCODE_MASTER����擾
			BOOL	umu;								// [�L������] (true:�g�p false:���g�p)				
			
			// PCODE_OUTPUT����擾
			int		nPutNum;							// output�ɃZ�b�g���Ă��鑍��
			OUTPUT	output[MAX_TYPE];					// �o�͏����ݒ�
			OUTPUT	defput;								// �r�햢�ݒ荀�ڂ̏o�͏���

		} rec[MAX_PCODE];								// �ŏI�́A���̑����ڌŒ�
	};


	//=======================================
	// �����p�^�[������ �\���� (PALL_OUTPUT)
	//=======================================
	enum EM_OUTPUT_PALLFLG						// �o�͏���		(�D��x��������̂́A�K���D��x�̍������ɕ��ׂĂ�����)
	{
		OUTPUTPALLFLG_DEFLIST_DETAIL = 0,				// �d���׈ꗗ��ʕ\��_�ڍ׏���
		OUTPUTPALLFLG_DEFLIST_SIMPLE,					// �d���׈ꗗ��ʕ\��_�ȈՏ���
		OUTPUTPALLFLG_DEFLIST_SIMPLE_LIST,				// �d���׈ꗗ��ʕ\��_�ȈՍ���
	
		OUTPUTPALLFLG_END								//(�Ō�Ƃ��Ă��g���Ă���̂Œ���)
	};

	struct P_PALL_OUTPUT
	{
		// �o�͏���
		struct OUTPUT							// �o�͍���
		{
			int		edasysID;							// �r��edasysID (1�`:�r��ID  0:���g�p  -1:���̑�����)
			
			int		kind[OUTPUTPALLFLG_END];			// �O���[�hID (0:���� 1�`:�O���[�hedasysID) �w�肳�ꂽ�O���[�hID�ȏ���r���o�͑Ώ�
														// �z��ʒu�́A[EM_OUTPUTFLG] ���g�p���鎖
		};

		// �o�͐ݒ�
		int nPutNum;							// output�ɃZ�b�g���Ă��鑍��
		OUTPUT	output[MAX_TYPE];				// �o�͏����ݒ�
		OUTPUT	defput;							// �r�햢�ݒ荀�ڂ̏o�͏���
	};


	//=======================================
	// �����p�^�[���ŗL �\����
	//=======================================
	struct P_PCOMMON_OUTPUT
	{
		// �o�͏���
		struct OUTPUT								// �o�͍���
		{
			int		nStopMarkFFSend;					// �X�g�b�v�}�[�NFF���M�L��

		} rec[MAX_PCODE];
	};

	//=======================================
	// ���ʃp�����[�^ �\����
	//=======================================
	struct P_PARAM_COMMON
	{
		// PLG���
		double	dPls;						// PLG�p���X���[�g[mm/pls]
		double	dPlsSub;					// PLG�p���X���[�g��r[mm/pls]
		double	dPlsDsp;					// PLG�p���X���[�g�\��[mm/pls]
		//struct _CNTSET{
		//	int		nPlgMul;				// �J�E���^�{�[�hPLG���{��
		//	int		nPlgPulse;				// �J�E���^�{�[�hPLG�M������[0�F���� 1�FTTL]
		//	int		nPlgPhase;				// �J�E���^�{�[�hPLG�ʑ�[0�F2�� 1�F�P��]
		//	int		nPlgPhaseDiff;			// �J�E���^�{�[�hPLG�ʑ���[0�F���߶��� 1�F�޳ݶ���]
		//	int		nPlgFilter;				// �J�E���^�{�[�hPLG�f�W�^���t�B���^
		//} typCntSet[1];

		int			nCamTap;				// �J�����^�b�v��

		int			nDistPosition;			// �\���Ԋ��[0:�\  1:��]
		double		dDistLen;				// �\���ԋ���[mm]

		int			nExSyncIntval;			// EXSYNC�ŏ�����[25ns]
		int			nExSyncMinH;			// EXSYNC�ŏ�H����[25ns]
		int			nExSyncMinL;			// EXSYNC�ŏ�L����[25ns]
		//BOOL		nAGCbyCamcont;			// ����AGC ON/OFF[TRUE:AGC�L��,FALSE:AGC����]

		double		dLapRate;				// �O�ڒ����`�̘A�Ȃ�[%]

#ifndef DISABLE_GAP_CALC
		BOOL	nUseWarpProj;							// �ˉe�ϊ���ԃY���␳ True:�Y���␳���{	
#endif

		// �J�����R���g���[���ݒ�
		struct _CAMCONT
		{
			int		nMul;					// ��{��
			int		nDiv;					// ������
		} typCamCon[4];					// [0:���C��PLG, 1:�\�Z��, 2:���Z��, 3:�e�X�g�p���X]

		int			nEdgeRestrictPort;		// �G�b�W����UDP��M�|�[�g

		int			nShFrameShrinkX;		// ���L��������t���[���摜�k��X
		int			nShFrameShrinkY;		// ���L��������t���[���摜�k��Y
	};
	//=======================================
	// �J�����Z�b�g�� �p�����[�^ �\����
	//=======================================
	struct P_PARAM_CAMSET
	{
		struct _CAMSET
		{
			double	dHres;					// ����������\[mm/pixel]
			double	dXoffset;				// �J������ʒu[mm]

#ifndef DISABLE_GAP_CALC
			// ����Y���␳
			int		nGapGroup[MAX_CAMERA_GAP_WIDDIV];	// ����Y���␳_���������� (�����������P�ʂŃO���[�vNo���Z�b�g) 0���́A�ΏۊO�B �� �e�����������ʒu���A�ǂ̃O���[�v�ɑ����邩
			int		nGapCount[MAX_CAMERA_GAP_WIDDIV];	// ����Y���␳_�W�v���{���� (�O���[�v�P�ʂŌ������Z�b�g) 0���́A�ΏۊO�B       �� �e�O���[�v���A�����ȏ�ɂȂ����畡��Y���␳�����{���邩
#endif

		} camset[MAX_CAMSET];
	};
	//=======================================
	// �����\�ʏ�ԕ� �p�����[�^ �\����
	//=======================================
	struct P_PARAM_SCODE
	{
		struct _SCODE
		{
#ifndef DISABLE_TINY_CALC
			//// ��������
			union _tiny_weight {
				float	array[8];					// �����r�d��
				struct {
					float	white_b_white_d;			// ����_����
					float	white_b_black_d;			// ����_����
					float	black_b_black_d;			// ����_����
					float	black_b_white_d;			// ����_����
					float	white_b;					// ����
					float	black_b;					// ����
					float	white_d;					// ����
					float	black_d;					// ����
				};
			} dTinyWeight;

			float	dTinyLimit[2][4][3];				// ��������臒l���x��(��/m^2) [0:�� 1:��][0:��4�c4 1:��4�c1 2:��1�c4 3:��1�c1][0:�y 1:�� 2:�d]
#else
			int ndummey;
#endif
		} scode[MAX_SCODE];
	};
	//=======================================
	// �J�����Z�b�g�E�����\�ʏ�ԕ� �p�����[�^ �\����
	//=======================================
	struct P_PARAM_CAMSET_SCODE
	{
		struct _SCODE
		{
			struct _CAMSET
			{
				int nEdgeDetectCam;		// �G�b�W���o�J����
			} camset[MAX_CAMSET];
		} scode[MAX_SCODE];
	};
	//=======================================
	// �J�����Z�b�g�E�J�����p�x�� �p�����[�^ �\����
	//=======================================
	struct P_PARAM_CAMSET_ANGLE
	{
		struct _CAMSET
		{
			struct _CAMANGLE
			{
#ifndef DISABLE_GAP_CALC
				// �J�����ݒ� (�ˉe�ϊ��ɕK�v)
				int	nXdelay;							// X�x��
				int nXinsert;							// X�}��
				int nYdelayLeft;						// Y�x����
				int nYdelayRight;						// Y�x���E
				
				// �ˉe�ϊ�
				double	dProjA0;						// �ˉe�p�����[�^
				double	dProjA1;
				double	dProjA2;
				double	dProjC1;
				double	dProjC2;
#else
				int ndummey;
#endif
			} camangle[MAX_CAMANGLE];
		} camset[MAX_CAMSET];
	};
	//=======================================
	// �\�ʏ�ԁE�J�����Z�b�g�E�J�����p�x�� �p�����[�^ �\����
	//=======================================
	struct P_PARAM_CAMSET_ANGLE_SCODE
	{
		struct SCODE
		{
			struct CAMSET
			{			
				struct CAMANGLE
				{
					//// �J�����ݒ�
					float dCameraGain;					// �J�����Q�C��
				} camangle[MAX_CAMANGLE];	
			} camset[MAX_CAMSET];
		} scode[MAX_SCODE];
	};



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �C�x���g���J
public:
	HANDLE		GetEvPrm_ReadEnd() const { return m_evReadEnd; }		// �p�����[�^�ǂݍ��݊����ʒm



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �p�����[�^�Q��
public:
	inline const P_COMMON_MAST&				GetCommonMast()		const {return (P_COMMON_MAST&)m_CommonMast;}
	inline const P_TYPE_NAME&				GetTypeName()		const {return (P_TYPE_NAME&)m_TypeName;}
	inline const P_GRADE_NAME&				GetGradeName()		const {return (P_GRADE_NAME&)m_GradeName;}
	inline const P_SCODE_OUTPUT&			GetScodeOutput()	const {return (P_SCODE_OUTPUT&)m_ScodeOutput;}
	inline const P_PCODE_OUTPUT&			GetPcodeOutput()	const {return (P_PCODE_OUTPUT&)m_PcodeOutput;}
	inline const P_PALL_OUTPUT&				GetPallOutput()		const {return (P_PALL_OUTPUT&)m_PallOutput;}
	inline const P_PCOMMON_OUTPUT&			GetPcommonOutput()	const {return (P_PCOMMON_OUTPUT&)m_PcommonOutput;}


	inline const P_PARAM_COMMON&		GetParamCommon()		const {return (P_PARAM_COMMON&)m_Param_Common;}
	inline const P_PARAM_CAMSET&		GetParamCamSet()		const {return (P_PARAM_CAMSET&)m_Param_CamSet;}
	inline const P_PARAM_SCODE&			GetParamScode()			const {return (P_PARAM_SCODE&)m_Param_Scode;}
	inline const P_PARAM_CAMSET_SCODE&	GetParamCamSetScode()	const {return (P_PARAM_CAMSET_SCODE&)m_Param_CamSetScode;}
	inline const P_PARAM_CAMSET_ANGLE&	GetParamCamset_Angle()	const {return (P_PARAM_CAMSET_ANGLE&)m_Param_CamsetAngle;}
	inline const P_PARAM_CAMSET_ANGLE_SCODE&	GetParamCamset_Angle_Scode()	const {return (P_PARAM_CAMSET_ANGLE_SCODE&)m_Param_CamsetAngleScode;}

	inline const P_TYPE_NAME::_TYPE_ID*	GetTypeName(int tid)	const
	{
		for(int ii=0; ii<m_TypeName.num; ii++ ) {
			if( m_TypeName.Type_Id[ii].nEdasysID == tid ) return (const P_TYPE_NAME::_TYPE_ID*) &m_TypeName.Type_Id[ii];
		}
		return NULL;
	}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �p�����[�^�擾�֘A (����)
public:
	bool ReadParam_All();												// �S�p�����[�^�ǂݍ���
	
	bool Read_CommonMast();												// �萔�p�����[�^�Ǎ�
	bool Read_TypeName();												// �r��p�����[�^�Ǎ�
	bool Read_GradeName();												// �O���[�h�p�����[�^�Ǎ�
	bool Read_ScodeOutput();											// �����\�ʏ�ԃp�����[�^�Ǎ�
	bool Read_PcodeOutput();											// �����p�^�[���ʃp�����[�^�Ǎ�
	bool Read_PallOutput();												// �����p�^�[�����ʃp�����[�^�Ǎ�
//	bool Read_PCommonOutput();											// �����p�^�[���ŗL�p�����[�^�Ǎ�

	bool Read_Param_Common();											// ���ʃp�����[�^�Ǎ�
	bool Read_Param_CamSet();											// �J�����Z�b�g�� �p�����[�^ �Ǎ�
	bool Read_Param_Scode();											// �\�ʏ�ԕ� �p�����[�^ �Ǎ�
	bool Read_Param_Camset_Scode();										// �J�����Z�b�g�E�����\�ʏ�ԕ� �p�����[�^ �Ǎ�
	bool Read_Param_Camset_Angle();										// �J�����Z�b�g�E�J�����p�x�� �p�����[�^ �Ǎ�
	bool Read_Param_Camset_Angle_Scode();								// �\�ʏ�ԁE�J�����Z�b�g�E�J�����p�x�� �p�����[�^ �Ǎ�

	bool Read_HtGroupNum(int groupmax);									// GroupID���̕K�vPC���擾


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �o�͏����`�F�b�N
public:
	bool CheckEdasysID_T(long tid) const;								// �r��EdasysID �̗L��
	bool CheckEdasysID_G(long gid) const;								// �O���[�hEdasysID �̗L��

	int  GetYusenT(long tid) const;										// �r��D��x�擾
	int  GetYusenG(long gid) const;										// �O���[�h�D��x�擾
	int  GetRankG(long gid) const;										// ���׃O���[�h�擾
	bool Check_Output(EM_OUTPUTFLG output, long pcode, long tid, long gid) const;					// �����p�^�[���o�͑Ώۂ̌��ׂ�����
	int  Check_OutputEx(EM_OUTPUTFLG output, long pcode, long tid, long gid) const;					// �����p�^�[���o�͑Ώۂ̌��ׂ�����
	bool Check_Pall_Output(EM_OUTPUT_PALLFLG output, long tid, long gid, bool bUp = true) const;	// �����p�^�[�����ʏo�͑Ώۂ̌��ׂ�����
	bool Check_ImgDsp(long tid, int index) const;													// �r��ɑ΂����r�摜�̕\���Ώۂ��`�F�b�N

	// �ǂ��g������
	double	CalcResV(int idx);											// �v�Z�l�c����\
	double	CalcSampBuff(int idx);										// �v�Z�l�J�E���^�l(1�t���[��)

	// �O���[�v���̕K�v����PC�䐔��Ԃ�
	void	GetHtGroupNum(int *pHtGroupNum) { memcpy(pHtGroupNum, m_nHtGroupNum, sizeof(m_nHtGroupNum)); }
	// �O���[�v���̔���PC������Ԃ�
	void	GetHtGroupMax(int *pHtGroupMax) { memcpy(pHtGroupMax, m_nHtGroupMax, sizeof(m_nHtGroupMax)); }


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// ����ŗL�̌��J�֐�
public:
	EM_OUTPUTFLG	GetOutputFlg(ENUM_VOICE_PRI_TYPE emViceDefType);	// �{�C�X�o�͏����̗D��x����DB���ڂ��擾


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �X���b�h�L���[
public:
	ThreadQueue<COMMON_QUE>	gque_Deli;									// �p�����[�^�ύX�ʒm�󂯓n���L���[


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �����o�[�֐�
private:
	void	ExecParam(COMMON_QUE* pQue);								// �p�����[�^�Ǎ�����



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// �����o�[�ϐ�
private:

	//// �n���h��
	HANDLE						m_evReadEnd;							// �p�����[�^�Ǎ�����

	//// �p�����[�^
	P_COMMON_MAST				m_CommonMast;							// �Ӄ֒萔
	P_TYPE_NAME					m_TypeName;								// �r��
	P_GRADE_NAME				m_GradeName;							// �O���[�h
	P_SCODE_OUTPUT				m_ScodeOutput;							// �����\�ʏ��
	P_PCODE_OUTPUT				m_PcodeOutput;							// �����p�^�[����
	P_PALL_OUTPUT				m_PallOutput;							// �����p�^�[������
	P_PCOMMON_OUTPUT			m_PcommonOutput;						// �����p�^�[���ŗL

	P_PARAM_COMMON				m_Param_Common;							// ���ʃp�����[�^
	P_PARAM_CAMSET				m_Param_CamSet;							// �J�����Z�b�g�� �p�����[�^	
	P_PARAM_SCODE				m_Param_Scode;							// �\�ʏ�ԕ� �p�����[�^	
	P_PARAM_CAMSET_SCODE		m_Param_CamSetScode;					// �J�����Z�b�g�E�����\�ʏ�ԕ� �p�����[�^
	P_PARAM_CAMSET_ANGLE		m_Param_CamsetAngle;					// �J�����Z�b�g�E�J�����p�x�� �p�����[�^
	P_PARAM_CAMSET_ANGLE_SCODE	m_Param_CamsetAngleScode;				// �\�ʏ�ԁE�J�����Z�b�g�E�J�����p�x�� �p�����[�^


	int							m_nHtGroupNum[DIV_GROUP_MAX - 1];		// Group���̔���PC�K�v��
	int							m_nHtGroupMax[DIV_GROUP_MAX - 1];		// Group���̔���PC����
};
