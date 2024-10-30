#pragma once

// STL
#include <vector>														// ���X�g�p


// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuODBC\OdbcBase.h"							// DB����N���X

#include "..\..\Library\KizuLib\CLASS\KizuPerformanceCounter.h"			// ���Ԍv��

// �r�����C�u���� �C���N���[�h
#include "MainConst.h"


using namespace KizuLib;
using namespace KizuODBC;


class DefragManager : public ThreadManager
{
private :
	//// �󂯓n���\����
	struct DEF_TBL{
		CString		cDefTable;											// �Ǘ��Ώۂ̎��уe�[�u������
	};


//// ���J�֐�
public:
	DefragManager(int id, char* cSession);
	virtual ~DefragManager(void);



	//// �O���A�N�Z�X
	void SetRunCnacel(bool bMode)	{ m_bRunCancel = bMode; }			// �f�t���O�����L�����Z���� true
	void SetExecDefragPnt(bool* p)	{ m_pExecDefrag = p;}				// �Q�ƃ|�C���^


//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����


	void Exec();														// �C���f�b�N�X �f�t���O���s
	bool GetIndexTables(char const* pName, std::vector<CString>* v);	// �C���f�b�N�X �ꗗ�擾
	int  ExecDefrag(char const* pTblName, char const* pIdxName);		// �C���f�b�N�X �f�t���O���s


	bool IsExec() { 
		return (*m_pExecDefrag && ! m_bRunCancel) ? true : false; 
	}


//// �����o�[�ϐ�
private:

	//// �n���h��
	HANDLE					m_evTimerExec;								// ��������s�C�x���g

	//// �O�����������|�C���^
	bool*					m_pExecDefrag;								// �f�t���O���s�̏ꍇ�Atrue


	//// �Ǘ��e�[�u��
	int						m_nTblNum;									// �Ǘ��e�[�u������
	DEF_TBL*				m_pTblDef;									// �Ǘ��e�[�u�����
	int						m_nTblExecNum;								// ���s�r���̔ԍ� (0�I���W��)
	int						m_nIdxExecNum;								// ���s�r���̔ԍ� (0�I���W��)
	int						m_nDefragIntervalDay;						// �f�t���O���s�Ԋu [��] 0:�����s


	//// ���̑��������
	CString					m_cSession;									// �����̐ݒ肪��`����Ă���ini�t�@�C���̃Z�b�V����
	CString					m_cOdbcSession;								// �ڑ�����ODBC����`����Ă�Z�b�V����
	bool					m_bRunCancel;								// �f�t���O�����L�����Z���� true
	CTime					m_LastDefragTime;							// �ŏI���s����
};
