#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC����N���X

// ���[�J��
#include "MainConst.h"													// ���N���X�萔�w�b�_�[


class FileCheckManager :	public ThreadManager
{

//// ���J�֐�
public:
	FileCheckManager(int nId);
	virtual ~FileCheckManager(void);


	void		SetEv_CheckStart() { SetEvent(m_evCheckStart);}
	EM_DIV_BOOL	GetRpiFile() { return m_emRpiFile; }
	char*		GetKizukenNo() { return m_cKizukenNo; }


//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	

	EM_DIV_BOOL CheckRpiFile();											// RPI�t�@�C���`�F�b�N


//// �����o�[�ϐ�
private:

	//// �n���h��
	HANDLE					m_evTimerExec;								// ��������s�C�x���g
	HANDLE					m_evCheckStart;								// ���s�C�x���g

	//// Ini�t�@�C��
	int						m_nTimeFirst;								// �������s�܂ł̎��� [ms]
	int						m_nCheckTime;								// �`�F�b�N���� [ms]
	int						m_nCheckOkNum;								// �ߋ�x�R�C�������āA�P��RPI�t�@�C�����������NG���� (1�`)
	int						m_nStateId;									// �@��ID

	int						m_nRpiCheckMode;							// �`�F�b�N���[�h (0:Ini�t�@�C���p�X�w��  1:���ڎw��)
	char					m_cRpiCheckSql[1024];						// ���ь����pSQL��

	// ini�t�@�C���w�莞�̂�
	char					m_cRpiSaveIniPath[256];						// TI_RPISAVE��INI�t�@�C����΃p�X(����IniSetting�̒�)
	char					m_cRpiSaveInsSec[256];						// TI_RPISAVE�Z�N�V������ ([TI_RPISAVE_INS_1])

	char					m_cRebootPcname[SIZE_NAME_PC];				// �ċN��PC����


	//// ���̑��������
	int						m_nId;										// ID (1�I���W��)
	EM_DIV_BOOL				m_emRpiFile;								// RPI�t�@�C�� ����
	char					m_cKizukenNo[SIZE_KIZUKEN_NO];				// �O��`�F�b�N���̍ŐV�r���Ǘ�No
};
