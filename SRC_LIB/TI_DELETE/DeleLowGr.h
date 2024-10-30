#pragma once

// �r�� �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLib\CLASS\KizuMacro.h"						// �r���}�N��
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC����N���X

// �r�����C�u���� �C���N���[�h
#include "MainConst.h"


using namespace KizuLib;
using namespace KizuODBC;


class DeleLowGr : public ThreadManager
{
private:
	//------------------------------------------
	// �Œ�萔
	//------------------------------------------
	static const int DELETE_MAXCNT				= 200;					// 1��̍폜����
	static const int SLEEP_TIME					= 2000;					// 1��폜��̑҂����� [ms]

//// ���J�֐�
public:
	DeleLowGr(int id, HANDLE evEndExec, SETTING_TBL const* pTyp);
	virtual ~DeleLowGr(void);


	//// �O���A�N�Z�X
	void SetStop()				{ m_bStopFlg = true; SetEvent(my_evStop);}	// �I���v��
	void SetExec(char* cNo)		{ memcpy(m_cKey, cNo, sizeof(m_cKey)); SetEvent(m_evExec); m_bIsExec=true;};	// ���s�˗�
	bool IsExec() const			{ return m_bIsExec; }
	const char* ExecKey()		{ return m_cKey; }


//// �����o�[�֐�
private :
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����


	bool StartExec();													// ���QGr�폜�J�n
	bool DeleteInf(OdbcBase* pClsDB, char const* cKeyTbl, char const* cKeyWhereSql, char const* cDeleTbl, char const* cKizukenNo, int nKizuNo);	// ���QGr�폜���s
	bool SetDeleteOkFlg(OdbcBase* pClsDB, char const* cKizukenNo);		// ���QGr�폜�Ǘ��e�[�u�����A�b�v�f�[�g


//// �����o�[�ϐ�
private :

	//// �����V�O�i��
	HANDLE					m_evExec;									// �폜���s�V�O�i��
	HANDLE					m_evEndExec;								// �폜�����C�x���g

	//// ���s�����
	bool					m_bIsExec;									// �������s��
	char					m_cKey[64];									// ���s�Ώێ�L�[

	//// ini�t�@�C��
	CString					m_cIniSqlSelectEnd;							// ���QGr�폜��̈ꗗ�e�[�u���X�VSQL (WHERE��ɂ͍폜������L�[���Z�b�g�����)

	//// ���̑��������
	int						m_nID;										// ������ID (1�I���W��)
	bool					m_bStopFlg;									// �I���v���� true
	SETTING_TBL const *		m_pTypSet;									// �폜�Ώۃe�[�u�����

};
