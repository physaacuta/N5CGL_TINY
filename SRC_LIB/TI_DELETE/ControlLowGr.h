#pragma once

// �r�� �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLib\CLASS\KizuMacro.h"						// �r���}�N��

// �r�����C�u���� �C���N���[�h
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC����N���X

// �e���[�J�[�X���b�h
#include "MainConst.h"
#include "DeleLowGr.h"													// ���QGr�폜�N���X


using namespace KizuLib;
using namespace KizuODBC;


class ControlLowGr : public ThreadManager
{
private:
	//------------------------------------------
	// �Œ�萔
	//------------------------------------------
	static const int THREAD_MAXCNT				= 9;					// �ő�DB���s�X���b�h��

//// ���J�֐�
public:
	ControlLowGr();
	virtual ~ControlLowGr(void);

	bool	IsEnable() {return m_bEnable;}

	void WorkerInit();													// �폜�X���b�h����������
	void WorkerStop();													// �폜�X���b�h��~����
	void WorkerExit();													// �폜�X���b�h�I������


//// �����o�[�֐�
private :
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����


	int CheckExecItem(char const* cKizukenNo);							// �폜�Ώۂ��r���Ǘ�No���`�F�b�N
	
	bool CheckDeleListItem();											// ���QGr�폜�Ώۃ`�F�b�N
	bool BackUpDeleListItem();											// ���QGr�폜�Ώۃ`�F�b�N


//// �����o�[�ϐ�
private :

	//=========================================
	// �e�C���X�^���X
	DeleLowGr*				mcls_pLowGr[THREAD_MAXCNT];					// ���QGr�폜�N���X


	//// �����V�O�i��
	HANDLE					m_evEndDelete;								// �폜�����V�O�i��
	HANDLE					m_evTimeCycle;								// ������V�O�i��

	//// ini�t�@�C��
	int						m_nIniThreadCount;							// �������s�폜�X���b�h��
	int						m_nIniCycle;								// DELETE_DEFECT_LIST�����������Ń`�F�b�N���邩 [ms]
	int						m_nIniDeleDay;								// �����ȑO�̃f�[�^�������폜���邩 [��] (0�̂Ƃ��͋����폜���Ȃ�)
	CString					m_cIniSqlSelectKey;							// �ꗗ�e�[�u������폜�ΏۃL�[�擾SQL (�폜�Ώێ�L�[�������ňꗗ�擾)
	SETTING_TBL				m_typSet;									// �ݒ�e�[�u�����

	// ���̑��������
	bool					m_bEnable;									// ���s�Ώۂ̏ꍇ true
};
