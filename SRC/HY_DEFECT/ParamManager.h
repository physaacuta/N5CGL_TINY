#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[

#include "..\..\Include\LineCommon.h"									// �r���C���N���[�h

// �r�����C�u���� �C���N���[�h
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC����N���X

#include "MainConst.h"

using namespace KizuLib;
using namespace KizuODBC;

class ParamManager :	public ThreadManager
{
public:
	//=======================================
	// �r���萔 �\����
	//=======================================
	struct P_COMMON_MAST
	{
		int nMapDsp[MAP_STAGE_NUM];					// �}�b�v�Ԋu [�V�X�e���ŏ�, ��, ��] [m]
	};

// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	//=======================================
	// �r���萔(�������֌W) �\����
	//=======================================
	struct P_COMMON_MAST_LONGPD
	{
		int nLongPd_GuidanceAvailable;				// �������K�C�_���X �K�C�_���X�L��(0:�Ȃ� 1:����)
	};		
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------


	//=======================================
	// �r�� �\����
	//=======================================
	struct P_TYPE_NAME
	{
		int num;									// ����
		struct TYPE_ID
		{
			int edasysID;							// edasysID
			int priority;							// �\���D��x (1���ŋ�)
		
			bool bEnable[MAX_IMAGE_CNT];			// �摜���̕\���L��
		} type_id[MAX_TYPE];
	};

	//=======================================
	// �O���[�h �\����
	//=======================================
	struct P_GRADE_NAME
	{
		int num;									// ����
		struct GRADE_ID
		{
			int edasysID;							// edasysID
			int priority;							// �\���D��x (1���ŋ�)
		} grade_id[MAX_GRADE];
	};


	//=======================================
	// �����p�^�[���� �\����
	//=======================================
	enum EM_OUTPUTFLG								// �o�͏���		(�D��x��������̂́A�K���D��x�̍������ɕ��ׂĂ�����)
	{
		OUTPUTFLG_DSP_O = 0,						// �T����� �\
		OUTPUTFLG_DSP_U,							// �T����� �� (�K�� �\�̘A�Ԃł��邱��)
		OUTPUTFLG_END								//(�Ō�Ƃ��Ă��g���Ă���̂Œ���)
	};

	struct P_PCODE_OUTPUT
	{
		// �o�͏���
		struct OUTPUT								// �o�͍���
		{
			int		edasysID;						// �r��edasysID (1�`:�r��ID  0:���g�p  -1:���̑�����)
			
			int		kind[OUTPUTFLG_END];			// �O���[�hID (0:���� 1�`:�O���[�hedasysID) �w�肳�ꂽ�O���[�hID�ȏ���r���o�͑Ώ�
													// �z��ʒu�́A[EM_OUTPUTFLG] ���g�p���鎖
		};

		// ����
		struct INF
		{
			// �}�X�^�[
			BOOL	umu;							// �L������ (true:�g�p false:���g�p)				
			
			// �o�͐ݒ�
			int		nPutNum;						// output�ɃZ�b�g���Ă��鑍��
			OUTPUT	output[MAX_TYPE];				// �o�͏����ݒ�
			OUTPUT	defput;							// �r�햢�ݒ荀�ڂ̏o�͏���

		} rec[MAX_PCODE];							// �ŏI�́A���̑����ڌŒ�
	};

	////=======================================
	//// �O�H���}�X�^�[	�\����
	////=======================================
	//struct P_MAE_MASTER
	//{
	//	int	nRec;									// ���R�[�h��
	//	struct REC_ITEM								// ���R�[�h����
	//	{
	//		char	cMaeCode[SIZE_KOUTEI_CODE+1];	// �O�H���R�[�h
	//		int		nMaeDsp;						// �O�H���\�����[�h
	//		int		nImgFmt;						// �摜�`��
	//	}	typRec[MAX_MAE_LINE];
	//};

	////=======================================
	//// �O�H���r��}�X�^�[	�\����
	////=======================================
	//struct P_MAE_TYPE_NAME
	//{
	//	int	nRec;									// ���R�[�h��
	//	struct REC_ITEM								// ���R�[�h����
	//	{
	//		char	cMaeCode[SIZE_KOUTEI_CODE+1];	// �O�H���R�[�h
	//		char	cFFName[SIZE_FFNAME_TYPE+1];	// FFName
	//		int		nTypeID;						// ID
	//		int		nPriority;						// �D��x
	//		int		nDispID;						// �\���L��
	//	}	typRec[MAX_MAE_TYPE];
	//};

	////=======================================
	//// �O�H���r�O���[�h�}�X�^�[	�\����
	////=======================================
	//struct P_MAE_GRADE_NAME
	//{
	//	int	nRec;									// ���R�[�h��
	//	struct REC_ITEM								// ���R�[�h����
	//	{
	//		char	cMaeCode[SIZE_KOUTEI_CODE+1];	// �O�H���R�[�h
	//		char	cFFName[SIZE_FFNAME_GRADE+1];	// FFName
	//		int		nGradeID;						// �O���[�hID
	//		int		nPriority;						// �D��x
	//	}	typRec[MAX_MAE_GRADE];
	//};


//// ���J�֐�
public:
	ParamManager(void);
	virtual ~ParamManager(void);

	//=========================================
	// �p�����[�^�Q��
	inline const P_COMMON_MAST&			GetCommonMast()			const {return (P_COMMON_MAST&)m_CommonMast;}
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	inline const P_COMMON_MAST_LONGPD&	GetCommonMast_LongPd()	const {return (P_COMMON_MAST_LONGPD&)m_CommonMast_LongPd;}
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

	inline const P_TYPE_NAME&			GetTypeName()			const {return (P_TYPE_NAME&)m_TypeName;}
	inline const P_GRADE_NAME&			GetGradeName()			const {return (P_GRADE_NAME&)m_GradeName;}
	inline const P_PCODE_OUTPUT&		GetPcodeOutpu()			const {return (P_PCODE_OUTPUT&)m_PcodeOutput;}

	//inline const P_MAE_MASTER&			GetMaeMaster()			const {return (P_MAE_MASTER&)m_MaeMaster;}
	//inline const P_MAE_TYPE_NAME&		GetMaeTypeName()		const {return (P_MAE_TYPE_NAME&)m_MaeTypeName;}
	//inline const P_MAE_GRADE_NAME&		GetMaeGradeName()		const {return (P_MAE_GRADE_NAME&)m_MaeGradeName;}

	//=========================================
	// �p�����[�^�擾�֘A (����)
	bool ReadParam_All(bool init=false);								// �S�p�����[�^�ǂݍ���

	bool Read_CommonMast();												// �r���萔�p�����[�^�Ǎ�
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	bool Read_CommonMast_LongPd();										// �r���萔�p�����[�^�Ǎ�(�������֌W)
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------
	bool Read_TypeName();												// �r��p�����[�^�Ǎ�
	bool Read_GradeName();												// �O���[�h�p�����[�^�Ǎ�
	bool Read_PcodeOutput();											// �����p�^�[���ʃp�����[�^�Ǎ�

	bool Read_MaeMaster();												// �O�H���}�X�^�[�e�[�u���Ǎ�
	bool Read_MaeTypeName();											// �O�H���r��}�X�^�[�e�[�u���Ǎ�
	bool Read_MaeGradeName();											// �O�H���r�O���[�h�}�X�^�[�e�[�u���Ǎ�

	//=========================================
	// �o�͏����`�F�b�N
	bool CheckEdasysID_T(long tid) const;								// �r��EdasysID �̗L��
	bool CheckEdasysID_G(long gid) const;								// �O���[�hEdasysID �̗L��

	int GetYusenT(long tid) const;										// �r��D��x�擾
	int GetYusenG(long gid) const;										// �O���[�h�D��x�擾

	int GetMaeYusenT(char* cProc, int nTid) const;						// �O�H���r��D��x�擾
	int GetMaeYusenG(char* cProc, int nGid) const;						// �O�H���O���[�h�D��x�擾

	int	GetMaeTID(char* cProc, char* cType);
	int	GetMaeGID(char* cProc, char* cGrade);

	bool CheckDispDef(char* cProc, char* cType, char* cGrade);

	bool Check_Output(EM_OUTPUTFLG output, long pcode, long tid, long gid) const;		// �����p�^�[���o�͑Ώۂ̌��ׂ�����
	bool Check_ImgDsp(long tid, int index) const;						// �r��ɑ΂��Ċe�p�x���\�����`�F�b�N


//// ���J�ϐ�
public:

	// �󂯓n���L���[
	ThreadQueue<COMMON_QUE>	gque_Deli;									// �p�����[�^�ύX�ʒm�󂯓n���L���[


//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast()	{return 0;};									// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	

	void ExecParam(COMMON_QUE* pQue);									// �p�����[�^�Ǎ�����



//// �����o�[�ϐ�
private:

	//=========================================
	// �p�����[�^
	P_COMMON_MAST				m_CommonMast;							// �r���萔
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	P_COMMON_MAST_LONGPD		m_CommonMast_LongPd;					// �r���萔(�������֌W)
// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------
	P_TYPE_NAME					m_TypeName;								// �r��
	P_GRADE_NAME				m_GradeName;							// �O���[�h
	P_PCODE_OUTPUT				m_PcodeOutput;							// �����p�^�[����

	//P_MAE_MASTER				m_MaeMaster;							// �O�H���}�X�^�[�e�[�u��
	//P_MAE_TYPE_NAME				m_MaeTypeName;							// �O�H���r��}�X�^�[�e�[�u��
	//P_MAE_GRADE_NAME			m_MaeGradeName;							// �O�H���r�O���[�h�}�X�^�[�e�[�u��

};
