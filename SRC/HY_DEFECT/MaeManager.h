#pragma once

// �W��
#include <direct.h>

// �r���t�@���N�V����
#include "..\..\Library\KizuLib\CLASS\KizuBitmapManager.h"	// Bitmap�摜�����N���X

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"	// ���[�J�[�X���b�h�}�l�[�W���[

// �r�����ʃw�b�_�[
#include "..\..\include\LineCommon.h"						// �r�����C�����ʃw�b�_�[
#include "..\..\include\HyTbl.h"							// �\��PC���L�������w�b�_�[

// �ʃw�b�_�[
#include "ParamManager.h"									// �p�����[�^�Ǘ��N���X
#include "DataSetManager.h"									// �f�[�^�Z�b�g�N���X

#define LEN_KIZUKEN_NO_HOT      15                          // �M���d�グ�A�Ǘ�No����

class MaeManager :	public ThreadManager
{
//// ���J�ϐ�
public:
	//// �\����
	struct TYP_MAE_REQUEST									// �O�H�����Ǎ��v���L���[
	{
		int			nMode;									// ��� (0:�O�H���r���Ǎ�)
		char		cKanriNo[SIZE_KIZUKEN_NO];				// �����Ώۂ̊Ǘ�No
		void*		pData;									// �t����� (�������NULL)
	};

//// ���J�֐�
public:
	MaeManager(void);										// �R���X�g���N�^
	virtual ~MaeManager(void);								// �f�X�g���N�^

	//// �v���p�e�B
															// �p�����[�^�Ǘ��N���X
	void SetParamMgr(ParamManager* pCls)	{mcls_pParam   = pCls;}
															// �f�[�^�Z�b�g�N���X
	void SetDataSetMgr(DataSetManager* pCls){mcls_pDataSet = pCls;}
															// �O�H�����Ǎ��v���L���[
	void SetQue_MaeRequest(ThreadQueue<TYP_MAE_REQUEST>* pQue){mque_pMaeRequest = pQue;}

//// �����o�[�ϐ�
private:
	//==================================
	// �\����
	// �O�H���e�e���
	struct TYP_FF_INF
	{
		// �O�H���e�e�r���
		char		cKizukenNo[SIZE_KIZUKEN_NO];	// �Ǘ���
		int			nKizuNo;						// �r��
		int			nKouteiID;						// �H�����
		int			nMenID;							// �\���敪[0:�\�A1:��]
		int			nPreEdit;						// ���O�ҏW�t���O
		char		cMaeKizukenNo[SIZE_KIZUKEN_NO];	// �O�H���Ǘ���
		int			nMaeKizuNo;						// �O�H���r��
		char		cFF_Type[4];					// �e�e�r��
		char		cFF_Grade[4];					// �e�e�r�O���[�h
		char		cFF_Koutei[4];					// �e�e�r���o�H��
		int			nFF_Kubun;						// �e�e�r�敪
		int			nFF_LPos;						// �e�e�r���������ʒu[mm]
		int			nFF_WPos;						// �e�e�r�Е����ʒu[mm]
		int			nFF_Length;						// �e�e�r����[mm]
		int			nType;							// �����r��
		int			nGrade;							// �����r�O���[�h
		int			nLastPos;						// �ŏI����ʒu[mm]
		// �O�H�����C���ʃR�C�����
		char		cLwk01[SIZE_KIZUKEN_NO];		// �Ǘ�No
		int			nLwk02;							// �H�����
		int			nLwk03;							// �\���敪
		int			nStatus;						// �����X�e�[�^�X
		// �O�H���}�X�^�[
		char		cMwk01[4];						// �O�H���R�[�h
		int			nCheck;							// �L������
		int			nMaeDsp;						// �O�H���\�����[�h
		int			nImgFmt;						// �摜�`��
	};

	// �O�H���r���
	struct TYP_MAE_DEFECT
	{
		char		cKizukenNo[SIZE_KIZUKEN_NO];	// �Ǘ���
		int			nKizuNo;						// �r��
		int			nKouteiID;						// �H�����
		int			nMenID;							// �\���敪[0:�\�A1:��]
		int			nCycle;							// �����t���O
		int			nLPos;							// ����ʒu[mm]
		int			nDSWS;							// DSWS�敪[0:DS�A1:WS]
		float		fDSPos;							// DS�ʒu[mm]
		float		fWSPos;							// WS�ʒu[mm]
		int			nLSpeed;						// ���C�����x[mpm]
		int			nPitch;							// �s�b�`[mm]
		float		fKei;							// �a[mm]
		int			nKizuW;							// �r��[mm]
		int			nKizuL;							// �r����[mm]
		float		fVDenc;							// �c����\[mm/pixel]
		float		fHDenc;							// ������\[mm/pixel]
		int			nEXmin;							// �O��Xmin[pixel]
		int			nEXmax;							// �O��Xmax[pixel]
		int			nEYmin;							// �O��Ymin[pixel]
		int			nEYmax;							// �O��Ymax[pixel]
		int			nCPosX;							// �r���S�ʒuX[pixel]
		int			nCPosY;							// �r���S�ʒuY[pixel]
		int			nVPixel;						// �c��f��[pixel]
		int			nHPixel;						// ����f��[pixel]
		int			nPSize1;						// �摜�T�C�Y1[byte]
		int			nPSize2;						// �摜�T�C�Y2[byte]
		int			nPSize3;						// �摜�T�C�Y3[byte]
		int			nPSize4;						// �摜�T�C�Y4[byte]
	};

	//==================================
	// ���L������
	int	m_nRec;							// �i�[���R�[�h
	TBL_HY_MAPROW*	m_pMaeMap;			// �O�H���}�b�v���e�[�u��
	TBL_HY_MAE_INF*	m_pMaeInf;			// �O�H���r���e�[�u��

	//==================================
	// �e�C���X�^���X
	ParamManager*		mcls_pParam;	// �p�����[�^�Ǘ��N���X
	DataSetManager*		mcls_pDataSet;	// �f�[�^�Z�b�g�N���X
	// �X���b�h�L���[
	ThreadQueue<TYP_MAE_REQUEST>*	mque_pMaeRequest;		// �O�H�����Ǎ��v���L���[

//// �����o�[�֐�
private:
	//=========================================
	// �X���b�h
	int		ThreadFirst();				// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int		ThreadLast();				// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void	ThreadEvent(int nEventNo);	// �X���b�h�C�x���g����

	//=========================================
	// �O�H��
	int		GetEnableData(TYP_MAE_REQUEST* pQue);			// �O�H���擾�󋵎擾
	void	RegMaeMap();									// �O�H���}�b�v���o�^
	int		RegFFInf(TYP_FF_INF* pFFInf, const int nPnt, const int nMode);	// �O�H���e�e���o�^
															// �O�H���r���o�^
	void	RegMaeDefect(TYP_FF_INF* pFFInf, TYP_MAE_DEFECT* pMaeDef);
															// �O�H���r�摜�Ǎ�
	bool	ReadMaeImg(TYP_FF_INF* pFFInf, TYP_MAE_DEFECT* pMaeDef);
	bool	ReadMaeDefect(TYP_FF_INF* pFFInf);				// �O�H���r���Ǎ�
	bool	ReadFFInf(TYP_MAE_REQUEST* pQue);				// �O�H���e�e���Ǎ�
	void	ReadMaeInf(TYP_MAE_REQUEST* pQue);				// �O�H�����Ǎ�
	int		GetMaeInfSetPos(TYP_FF_INF* pFFInf, int* nMode);
};
