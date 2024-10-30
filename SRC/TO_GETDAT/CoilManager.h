#pragma once
// ���Œ�
#include "..\..\Include\LineCommon.h"									// �r���C���N���[�h

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[

#include "..\..\Library\AtlRegExpLib\AtlRegManager.h"					// ���K�\�� �N���X
#include "..\..\Library\KizuLibMFC\Class\DivNameManager.h"				// DIV�萔 ���̎擾�N���X

// ���[�J��
#include "..\..\Include\ToHtCommon.h"									// ��������p�C���N���[�h
#include "..\..\Include\ToTbl.h"										// �������L�������e�[�u��

#include "MainConst.h"
#include "ParamManager.h"

using namespace KizuLib;

// �t�@���N�V�����̎g���������AThreadManager���p�����Ă����B���[�J�[�X���b�h�͋N�����Ȃ�
class CoilManager :	public ThreadManager
{
//// ���J�\����
public: 
	struct TYP_NEWCOIL {
		COIL_SETTING 	setting;
		COIL_BASEDATA	basedata;
	};



//// ���J�֐�
public:
	CoilManager(LogFileManager* pLog);
	virtual ~CoilManager(void);
	
	// �������u���ʏ�����
	void Check_CoilInf(int nRec, bool bRevFlg=false, bool bHandSetFlg = false);			// �R�C��������� [��Dlg]

	// ��ʗp
	TYP_NEWCOIL const*	GetNewCoil()				{ return &m_typNewCoil; }	

	// �_�C�A���O���猟������������� �Ή�
	void SetHandSetScode(int men, int val)		{ m_nHandSetScode[men] = val; }			// ����͂��ꂽSCODE [��Dlg]
	void SetHandSetPcode(int men, int val)		{ m_nHandSetPcode[men] = val; }			// ����͂��ꂽPCODE [��Dlg]

	int  GetHandSetScode(int men)				{ return m_nHandSetScode[men]; }		// ����͂��ꂽSCODE [��Dlg]
	int  GetHandSetPcode(int men)				{ return m_nHandSetPcode[men]; }		// ����͂��ꂽPCODE [��Dlg]

	CString			GetSpmModeName(bool bSpm)	  const { return bSpm ? "SPM������" : "SPM�J����"; };	// SPM��Ԃ̖��̎擾 

//// �����o�[�֐�
private:
	// ���[�J�[�X���b�h�Ƃ��ē��삳���Ȃ����߁A�֌W�֐������J�ɕύX
	int				ThreadFirst()				{return 0;}				// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int				ThreadLast()				{return 0;}				// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void			ThreadEvent(int nEventNo)	{;}						// �X���b�h�C�x���g����

	void	SelectScode(PRO_TYPE_DATA_COILINF const* c, int ou, bool bSpm, int* scode);	// �����\�ʏ�Ԃ�����
	void	SelectPcode(PRO_TYPE_DATA_COILINF const* c, int ou, bool bSpm, int* pcode);	// �����p�^�[��������

//// �����o�[ �ϐ�
private:

	//// �e�C���X�^���X 
	ParamManager			mcls_Param;									// �p�����[�^�Ǘ�

	// ���L���������
	HANDLE					m_hCoil;									// �R�C����� �n���h��
	TO_COIL_TBL*			mtbl_pCoil;									// �R�C����� �f�[�^

	// ���̑��������
	TYP_NEWCOIL				m_typNewCoil;								// �ŐV�̃R�C�����

	int						m_nHandSetScode[NUM_MEN];					// �_�C�A���O�Ɏ���͂��ꂽ SCODE
	int						m_nHandSetPcode[NUM_MEN];					// �_�C�A���O�Ɏ���͂��ꂽ PCODE

};
