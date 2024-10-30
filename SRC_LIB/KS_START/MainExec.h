#pragma once


// �r�� �C���N���[�h
#include "..\..\include\KizuCommon.h"									// �r�����ʃw�b�_�[


// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLib\CLASS\KizuMacro.h"						// �r���}�N��
#include "..\..\Library\KizuLib\CLASS\ProcessManager.h"					// �v���Z�X�Ǘ�
#include "..\..\Library\KizuLibMFC\CLASS\LogFileManager.h"				// ���O�}�l�[�W���[

#include "..\..\Library\KizuLib\CLASS\MagicPacket.h"					// �}�W�b�N�p�P�b�g�Ǘ��N���X

#include <shlwapi.h>	// �t�H���_�`�F�b�N
#include <direct.h>		// �t�H���_����

// ���[�J��
#include "OdbcSetFunc.h"												// �ق��Ŏg�����߃t�@���N�V������


using namespace KizuLib;

class MainExec
{
public:
	MainExec(LogFileManager* pLog, bool* bCancel);
	virtual ~MainExec(void);

	void DoExec();														// ���C������

private:
	bool ExecFolderCopy();												// �t�H���_�[�R�s�[���s
	bool ExecFileCopy();												// �t�@�C���R�s�[���s

	bool CreateFolder(char const* cPath);								// �t�H���_�[����


	// DB��Ini�t�@�C������֘A
	void Change_DB();													// DB�̕ύX
	void ChangeIni_DBSession(char const * Session);						// Ini�t�@�C����DB�̃Z�b�V��������������
	void ChangeIni_DBSession_N(char const * Session, char const * DbSection);	// Ini�t�@�C����DB�̃Z�b�V��������������(�����Z�b�V�����Ή�) 2019.01.23 Add
	// ===>> �⊪ 2018.10.21 ODBC�h���C�o�敪�ʎw��@�\�ǉ�
	//void ChangeODBC_Setting(char const * Session);						// ODBC�̐ݒ�ύX
	void ChangeODBC_Setting(char const * Session, int nId);				// ODBC�̐ݒ�ύX
	// <<===
//	BOOL ODBC_Setting(int nDriverID, LPCSTR cDSN, LPCSTR cDataSource, LPCSTR cDataBase);	// ODBC�����ݒ� (����DNS�̏ꍇ�́A�ύX�ƂȂ�)


	//// �e�C���X�^���X
	LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X

	//// ���̑��������
	int						m_nIniNowID;								// ��ID
	bool*					m_pCancel;									// �����I���� true

};
