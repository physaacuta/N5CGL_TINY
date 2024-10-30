#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[

#include "..\..\Include\LineCommon.h"									// �r���C���N���[�h

// ���[�J���̃N���X
#include "MainConst.h"
#include "ParamManager.h"												// �p�����[�^�Ǘ��N���X

using namespace KizuLib;

class LongPdCheckManager :	public ThreadManager
{
//// �\����
public:
	//=============================
	// �Ď���� �\����
	//=============================
	struct CEHCK_INF
	{
		char		cManagementNo[SIZE_KIZUKEN_NO];						// �Ǘ�No
		BOOL		bLongPdExist;										// ���������חL��(true:�L�� false:����)
		int			nCount[NUM_MEN];									// ���@�K�C�_���X�p
		char		cCoilNoIn[SIZE_COIL_NO+1];							// �����R�C��No+NULL  (�R�C��No) �K�C�_���X�p
	};

//// ���[�J���萔
private :

	//=========================================
	// �萔

	//static const int TIME_INTERVAL_DEFAULT_EXEC	= 5;					// ������������s �^�C�}���� [ms] �f�t�H���g�l�@���p�����[�^�Ǎ����܂ł̒l


//// ���J�֐�
public:
	LongPdCheckManager(void);
	virtual ~LongPdCheckManager(void);

	//=========================================
	// �v���p�e�B
	void SetParamMgr(ParamManager* pCls)	{ mcls_pParam = pCls; }		// �p�����[�^�Ǘ�

	////=========================================
	//// �O������֐�


	//=========================================
	// �O���A�N�Z�X

	//=========================================
	// ��ʐ�p
	CEHCK_INF const*  GetCheckInf(int index)	{ return &m_CheckInf[index]; }


//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	
	void CheckLongPd();													// ���������׃`�F�b�N

//// �����o�[�ϐ�
private:
	CEHCK_INF			m_CheckInf[CHECK_COIL_NUM];						// �Ď���� ��������Œ��߃R�C���̏����擾
	int					m_nParamCycleTime;								// �p�����[�^����Ǎ������������� ( 0�ȉ��͔��f�����Ƀf�t�H���g )

	//=========================================
	// �e�C���X�^���X
	ParamManager*			mcls_pParam;								// �p�����[�^�Ǘ�

	//// �L���[

	//// �n���h��
	HANDLE					m_evTimerExec;								// ��������s�C�x���g	
};
