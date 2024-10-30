#pragma once

// ���Œ�
#include "..\..\Include\KizuCommon.h"									// �r���C���N���[�h
#include "..\..\Library\KizuLib\KizuLib.h"								// �r�����C�u����
#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// �}�N���W
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// �X�^�e�B�b�N���C�u�����W
#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ���O�Ǘ��N���X

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLib\Class\WorkThread.h"						// ���[�J�[�X���b�h�N���X
#include "..\..\Library\KizuLib\Class\ThreadQueue.h"					// �X���b�h�L���[�N���X

#include "..\..\Library\KizuODBC\OdbcBase.h"							// DB����N���X

using namespace KizuLib;
using namespace KizuODBC;


class KizuSyncroManager
{
public:
	// �����e�[�u����`
	static const int PARAM_MAX_THREAD		= 10;						// ����C���X�^���X�̍ő�̃C���X�^���X��
	static const int PARAM_MAX_TABLE		= 100;						// �ő哯���Ώۃe�[�u��

	static const int PARAM_MAX_KEYCNT		= 3;						// ��L�[�ő�w�萔 (�ύX�����SQL�����������K�v�L��)
	static const int PARAM_MAX_STRING		= 128;						// ������

	//=======================================
	// �萔��`
	//=======================================
/*	// DB�̑I�����
	enum EM_SELECT_DB {
		SELECT_NON	= -1,												// ����
		SELECT_A	= 0,												// A�n
		SELECT_B	= 1													// B�n
	};
*/

public:
	KizuSyncroManager(CString cSession, LogFileManager* pLog);
	virtual ~KizuSyncroManager(void);

	virtual void Start() {};											// �������z�֐�
	virtual void Stop() {};												// �������z�֐�
	virtual char const*	GetType() const	{ return (char const*)& "";};	// ����^�C�v (�Œ薼��)

//	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; };			// ���O�t�@�C���Z�b�g
	void SetStop()			{ my_bStopFlg = true; }						// �I���v��

	bool SetEvNowExec(int nAtoB, int nMailKey);							// �������s�V�O�i���R�[��
	


	// ��ʃA�N�Z�X�p
	char const*	GetSession() const { return my_cSession; };				// �Z�b�V��������
	char const* GetOdbc_A() const  { return (const char *)my_cOdbcSession[0];}
	char const* GetOdbc_B() const  { return (const char *)my_cOdbcSession[1];}
	char const* GetLink_A() const  { return (const char *)my_cLinkPath[0];}
	char const* GetLink_B() const  { return (const char *)my_cLinkPath[1];}

	int			GetTimeFast() const { return my_nTimeFast/(60 *1000);}
	int			GetTimeInterval() const { return my_nTimeInterval/(60 *1000);}
	int			GetMailkey() const { return my_nMailKey;}



protected :
	void SetOdbcBaseA(OdbcBase* p) { mcls_pDbA = p; };					// A�n���� DB�ڑ��N���X
	void SetOdbcBaseB(OdbcBase* p) { mcls_pDbB = p; };					// B�n���� DB�ڑ��N���X

	// ���ʃ��\�b�h
//	static int		CheckIni_Select_DB();								// Ini�t�@�C����DB�̃Z�b�V������A�n��B�n���`�F�b�N����
//	static int		CheckIni_Select_DB(char* Session);					// Ini�t�@�C����DB�̃Z�b�V������A�n��B�n���`�F�b�N����
	bool			CheckDB_Connect();									// A�n��B�n��DB�������Ă��邩�`�F�b�N����

	// ���ۂ̏����֌W
	bool			Synclock_Master(bool bAtoB, char const* tbl);			// 2DB�Ԃ̃}�X�^�[�e�[�u���̓���
//	bool			Copy_DB(bool bAtoB, char const* tbl, int nExecCnt, char const* key1, char const* key2);	// ���ʃR�s�[
//	bool			Move_DB(bool bAtoB, char const* tbl, int nExecCnt, char const* key1, char const* key2);	// ���ʈړ�
//	bool			Syncopy_DB(bool bAtoB, char const* tbl, int nExecCnt, char const* key1, char const* key2);	// 2DB�Ԃ̎��уe�[�u���̓���

	bool			Copy_DB   (bool bAtoB, char const* tbl, int nExecCnt, int nKeyCnt, char const* key);	// ���ʃR�s�[
	bool			Move_DB   (bool bAtoB, char const* tbl, int nExecCnt, int nKeyCnt, char const* key);	// ���ʈړ�
	bool			Syncopy_DB(bool bAtoB, char const* tbl, int nExecCnt, int nKeyCnt, char const* key);	// 2DB�Ԃ̎��уe�[�u���̓���


//	bool Move_DB(bool bBtoA, char const* tbl, int delCnt, char const* keyName);		// �w�荀�ڃL�[�� ���ʈړ�


	//// �e�C���X�^���X
	LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X
	OdbcBase*				mcls_pDbA;									// A�n���� ODBC����N���X
	OdbcBase*				mcls_pDbB;									// B�n���� ODBC����N���X

	//// �����V�O�i��
	HANDLE					my_evNowExecAtoB;							// A��B ���s�V�O�i��
	HANDLE					my_evNowExecBtoA;							// B��A ���s�V�O�i��

	//// ini�t�@�C���֘A
	CString					my_cSession;								// �����̐ݒ肪��`����Ă���ini�t�@�C���̃Z�b�V����
	char					my_cOdbcSession[2][128];					// �ڑ�����ODBC����`����Ă�Z�b�V����
	char					my_cLinkPath[2][128];						// �����N�T�[�o�[�o�R��DB�ڑ�����p�X [0:A��B�n�̐ڑ��p�X  1:B��A�n�̐ڑ��p�X]	// �`���́A [pc��].DATABASE_5CGL.dbo.  ����Ȋ����œ����Ă���
//	bool					my_bLinkServerB;							// B�n�̃����N�T�[�o�[�L�� (�L:true)
	int						my_nTimeFast;								// �������s�܂ł̎��� [ms]
	int						my_nTimeInterval;							// 2��ڈȍ~�̎��s���� [ms]
	int						my_nMailKey;								// ���[���X���b�g��M���ɑ�2�����ƈ�v�����ꍇ�A�����I�Ɏ��s�����B (0:�����{)

	//// ���̑��������
	bool					my_bStopFlg;								// �I���v���� true
};
