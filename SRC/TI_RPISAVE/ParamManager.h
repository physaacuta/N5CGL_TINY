#pragma once

// ���ʃw�b�_�[
#include "..\..\Include\KizuCommon.h"									// �V�X�e���p�C���N���[�h

// STL
#include <math.h>

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLib\Class\AutoLock.h"						// �N���e�B�J���Z�N�V��������N���X

#include "..\..\Library\KizuLibMFC\CLASS\CsvIniFunc.h"					// CSV�N���X

// �r�����C�u���� �C���N���[�h
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC����N���X

// ���[�J���̃N���X
#include "MainConst.h"


using namespace KizuLib;
using namespace KizuODBC;

class ParamManager : public ThreadManager
{
//// ���J�\����
public:

	//=======================================
	// ini�t�@�C�� �ݒ�\����
	struct P_MAIN
	{
		// ini ----------
		int			nIns;					// INS_NUM				// ����̃C���X�^���X�� 1�Œ�
		//int			nFrameSizeX;			// FRAME_SIZE_X			// �X�̃t���[���T�C�Y
		//int			nFrameSizeY;			// FRAME_SIZE_Y


		int			nExecCoilStart;						// 0:�R�C������M�Ŏ��������J�n  -1:�����J�n���Ȃ�

		char		cWriteSecPath[64];					// �Z�N�^�[�P�ʂ̏����ݎ��̃Z�N�^�T�C�Y�擾�p�X (�w�肷��ƃZ�N�^�P�ʂ̏����݁B���ݒ蕁�ʂ̏�����)
		char		cSystemName[64];					// �V�X�e������ (RPI�t�@�C���̒��ɖ��ߍ���)
		int			nHddFreeSizeSave;					// �S���摜�ۑ�PC��HDD�Œ�󂫗e�� [GB]
		int			nHddFreeSizeLocal;					// ���[�J��PC��HDD�Œ�󂫗e�� [GB]

		int			nAddFrameInfoType;					// �t���[���t�����ؑ� (0:���� 1:T_RPI_FRAME_INFO_T1)

		// ��
		int			nLoopQueRecvSize;					// LOOP_QUE_RECV_SIZE	// ���肩���RAW�摜�󂯓n���L���[
		int			nLoopQueFrameSize;					// �A���摜�p ��]�L���[�T�C�Y (1�ŁA�t���[���摜�~��������������������)
		int			nThreadUnionNum;					// �A���p�X���b�h�̕�����s��
		int			nThreadCompNum;						// ���k�p�X���b�h�̕�����s��


		// ���
		int			nDispWindScaleX;					// ���摜����̏k�ڔ{��
		int			nDispWindScaleY;
		int			nDispWindPosX;
		int			nDispWindPosY;						// �Y���V�� [pixel]
		int			nDispPrint;							// �摜�f�[�^�Ƀf�o�b�N���t�^ (0:����  1:���[��Fno,Ino�t�^  2:1�{������Iport�V�[�P���XNo�t�^)




		// �Œ�ŕێ����Ă����K�v����
		EM_COLOR_TYPE	emColorType;					// �摜�^�C�v (0:�O���[�摜  1:�J���[�摜  2:�F���摜)
		EM_SYS_TYPE		emSysType;						// �r���^�C�v (0:�A�����C��  1:�o�b�`���C��)
		int				nImgNumTotal;					// 1�摜���琶�������ŏI�I�ȉ摜��	(�F���̏ꍇ4)



		//// �ȍ~ �S�ĊԈ������W�n
		// �v�Z ----------
		//int			nCullSizeX;							// �Ԉ�����P�t���[���T�C�YX
		//int			nCullSizeY;							// �Ԉ�����P�t���[���T�C�YY
		int			nFrameSizeX;			// FRAME_SIZE_X			// �X�̃t���[���T�C�Y(�k�������f)
		int			nFrameSizeY;			// FRAME_SIZE_Y
		int			nGrabSizeX;				// GRAB_SIZE_X			// �X�̎B���t���[���T�C�Y(�k�������f)
		int			nGrabSizeY;				// GRAB_SIZE_Y


		int			nMaxWidth;							// �ő�̃t���[���摜�A����̉摜��		���̈�m�ۂ̂���
		int			nMaxHeight;							// �ő�̃t���[���摜�A����̉摜����
		int			nBmpHeadSize;						// �r�b�g�}�b�v�w�b�_�T�C�Y

		int			nStateId;							// �@��ID


		// ///////////////////////////
		// �C���X�^���X�P��
		struct	INS
		{
			// �R�C���P�ʂŃp�����[�^�Ǎ� ----------
			//   �� ��΂ɒ��ڎQ�Ƌ֎~�B�t���[���ƈꏏ�ɓn���Ă����l���g�p���邱��
			//   �� �����I�ɂ�ins�P�ʂŎ��悤�ɕύX
			EM_COMP_TYPE	emCompType;					// �摜���k�^�C�v
			int				nCompQualityBase;			// ���k�� (0�`100%)
			int				nCompQuality[MAX_PCODE];	// PCODE�P�ʂ̈��k���ݒ�l (0�̏ꍇ�̓x�[�X���Q��)



			// ini ----------
			int		nStartCamset;						// �J�����Z�b�g�J�n�ԍ� (1�I���W��)
			int		nCamsetNum;							// �C���X�^���X������̃J�����Z�b�g��
			int		nCamAngle;							// �J�����p�x (1�I���W��)
			int		nMen;								// �\���敪 (0:�\ 1:��)
			bool	bWidthSync;							// �A����摜�������L��
			bool	bImgTorB;							// �摜�㉺���]�@(0:�Ȃ��@1:���]) BMP�w�b�_�[�ŉ��z�I�ɔ��] 

			char	cRawSavePath[128];					// ���摜�ۑ��p�X (�Ō��\�͖���)
			int		nRpiFolderBufNum;
			char	cRpiFolderBuf[MAX_FOLDER_SAVE_NUM][128];	// �ꎟ�����pRPI�t�@�C���ۑ��t�H���_ (�Ō��\�͖���)
			int		nRpiFolderSaveNum;
			char	cRpiFolderSave[MAX_FOLDER_SAVE_NUM][128];	// �����ԕۑ��pRPI�t�@�C���ۑ��t�H���_ (�Ō��\�͖���)

			// �A���摜�֌W
			int		nOverLapMode;						// ���E�t���[���̃I�[�o�[���b�v����������
															// (-1:�A���̂݁B���ްׯ�ߖ���  0:�Ѱ��ݸ�  1:������ڰэ̗p  2:�E����ڰэ̗p)
			int		nImgDelete[2];						// �������t���[���[ �h��ׂ��͈� [pixel] (���E�̒[���� �w���f���̏����폜) (0:�� 1:�E)
			bool	bEdgeDeleteEnabel;					// �G�b�W�O�h��ׂ��L�� (false:���� true:�L��)
			int		nEdgeDelete[2];						// �G�b�W�O�{���� �h��ׂ��͈� [pixel] (���E�̃G�b�W���� �w���f������O�̏����폜) (0:�� 1:�E)



			//// �ȍ~ �S�ĊԈ������W�n
			// �v�Z ----------
			int		nUionWidth;							// �t���[���摜�A����̉摜��
			int		nUionHeight;						// �t���[���摜�A����̉摜����
			BYTE	BmpHead[2048];						// �r�b�g�}�b�v�w�b�_�|�C���^ (���낢���ς�����A�����傫�߂̌Œ�T�C�Y�ɂ��Ă���)

			float	reso_x;								// �ΏۃJ�����̕��ω�����\
			float	reso_y;								// �ΏۃJ�����̏c����\

			int		nLineCenter;						// �摜�A����t���[�����[���烉�C���Z���^�[�ʒu�܂ł̃I�t�Z�b�g�l [pixel]
			int		nInsertRight;						// �E�[��f�}���� [pixel]

		
			struct CAMSET
			{
				// ini ----------
				// �P�x�␳
				bool	bKidoUmu;						// �P�x�␳�L������

				// �v�Z ----------
				double	dResWidth;						// ������\ [mm/pixel]

				//// ���t���[���Ǝ��̃t���[�����d�Ȃ�
				int		nBaseLeft;						// �摜�A����t���[�����[���玩�J�����摜���[�܂ł̃I�t�Z�b�g�l

				int		nOverLap;						// ���J�����Ƃ̉E���̃I�[�o�[���b�v�� [pixel] (0�̂Ƃ��A�I�[�o�[���b�v����)
				int		nLapStartBase;					// ���t���[�����[��ł̃I�[�o�[���b�v�J�n�ʒu
															// nOverLap + nLapStartBase = �t���[���T�C�Y�ƂȂ�

				int		nOverLapL;						// �O�J�����Ƃ̍����̃I�[�o�[���b�v�� [pixel]
				int		nLapStartBaseResult;			// �ŏI�̃I�[�o�[���b�v�J�n�ʒu

				// �ŏI����
				int	nCopyImg[2];						// �P���ɃR�s�[�摜����(���t���[���̊J�npixel�A�R�s�[�T�C�Y)
				int nHokanImg[2];						// �����ۊǂ���摜����(���t���[���̊J�npixel�A�R�s�[�T�C�Y)

			} camset[MAX_CAMSET];						// ���ےl���Z�b�g�����̂́A [m_Ini.nCamsetNum] ��
		} ins[MAX_INS];
	};
	

	//=======================================
	// �J�����Z�b�g�� �p�����[�^ �\����
	struct P_PARAM_CAMSET
	{
		// �ʒP�ʂœ����Ă���
		struct	INS {
			struct CAMSET
			{		
				int		nCamset;							// �Z�b�g����Ă���J�����Z�b�g�ԍ� (1�I���W��)
				double	dResWidth;							// ������\ [mm/pixel]
				double	dXoffset;							// �J�������_ X [mm]
				

				bool	bIsEnable;							// �L���J������ true�B�� �������J���������ł������΍�
			} camset[MAX_CAMSET];					// ���ےl���Z�b�g�����̂́A [m_Ini.nCamsetNum] ��
		} ins[MAX_INS];								// ���ےl���Z�b�g�����̂́A [m_Ini.nIns] ��
	};


	//=======================================
	// ���ʃp�����[�^ �\����
	struct P_PARAM_COMMON
	{
		double	dPls;									// �p���X���[�g	
													// �J�����R���g���[���ݒ� (0:ײ�PLG)
		int		nMul;									// ���{
		int		nDiv;									// ����


		// �Ԉ����֌W
		int			nImgCullX;							// �摜�Ԉ������B�ÓI�B �� 1/x ���w��
		int			nImgCullY;

	};


//// ���J�֐�
public:
	ParamManager(void);
	virtual ~ParamManager(void);


	//=========================================
	// �C�x���g���J
	HANDLE		GetEvPrm_ReadEnd() const { return m_evReadEnd; }		// �p�����[�^�ǂݍ��݊����ʒm



	//=========================================
	// �p�����[�^�Q��
	inline const P_MAIN&				GetMain()				const {return (P_MAIN&)m_Main;}

	inline const P_PARAM_CAMSET&		GetParamCamSet()		const {return (P_PARAM_CAMSET&)m_Param_CamSet;}
	inline const P_PARAM_COMMON&		GetParamCommon()		const {return (P_PARAM_COMMON&)m_Param_Common;}

	inline const std::vector<short>&	GetKido(int nIns, int nCam)	const { return m_vKido[nIns][nCam]; }

	double	CalcResV();													// �v�Z�l�c����\


	//=========================================
	// �p�����[�^�擾�֘A (����)
	bool ReadParam_All(bool bStatic = false);							// �S�p�����[�^�ǂݍ���

	void Read_IniCoilChange(int nIns);									// �R�C���P�ʂŃp�����[�^�Ǎ�����
	void Read_IniKidoChange(int nIns);									// �P�x�l�X�V




	//=========================================

	// ���W�ϊ�
	int CalcCullX(int nFX) { return nFX/m_Param_Common.nImgCullX; }			// �t���[�����W�n���Ԉ������W�n��
	int CalcCullY(int nFY) { return nFY/m_Param_Common.nImgCullY; }





private:
	bool Read_Ini(P_MAIN& buf, bool bStatic = false);					// ini�t�@�C���ǂݍ���
	bool Read_Param_CamSet(P_MAIN& buf);								// �J�����Z�b�g�� �p�����[�^ �Ǎ�
	bool Read_Param_Common();											// ���ʃp�����[�^�Ǎ�
	bool Read_Param_CamEnable(P_MAIN& pm);								// �L���J�����擾


//// ���J�ϐ�
public:
	// �󂯓n���L���[
	ThreadQueue<COMMON_QUE>	gque_Deli;									// �p�����[�^�ύX�ʒm�󂯓n���L���[


//// �����o�[�֐�
private :
	int				ThreadFirst();										// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int				ThreadLast()	{return 0;};						// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void			ThreadEvent(int nEventNo);							// �X���b�h�C�x���g����

	void			ExecParam(COMMON_QUE* pQue);						// �p�����[�^�Ǎ�����
	void			CalcParam(P_MAIN& buf);								// �擾�����p�����[�^���� �K�v�ȏ����Z�o����


//// �����o�[�ϐ�
private :
	//=========================================
	// �V�O�i�� (�O������)
	HANDLE						m_evReadEnd;							// �p�����[�^�ǂݍ��݊���

	//=========================================
	// �p�����[�^
	P_MAIN						m_Main;									// Ini�t�@�C�����̐ݒ���
	P_PARAM_CAMSET				m_Param_CamSet;							// �J�����Z�b�g�� �p�����[�^
	P_PARAM_COMMON				m_Param_Common;							// ���ʃp�����[�^

	std::vector<short>			m_vKido[MAX_INS][MAX_CAMSET];			// �P�x�␳�l [1�J���� ��pixel��] ���o�C�g�ɂ��Ȃ��̂̓}�C�i�X�␳�Ή��̂���

};
