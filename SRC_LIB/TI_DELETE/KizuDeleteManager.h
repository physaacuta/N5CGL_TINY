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
#include "..\..\Library\KizuLib\Class\LockManager.h"					// �X���b�h�ԓ�������N���X

#include "..\..\Library\KizuODBC\OdbcBase.h"							// DB����N���X

using namespace KizuLib;
using namespace KizuODBC;

class KizuDeleteManager : public WorkThread
{
public:
//	//// ���b�Z�[�W��`
//	static const int WM_SIZEMIN_END					= WM_USER+100;		// �e�ʒቺ�폜���s�����ʒm
	
	//// �󂯓n���\����
	struct DEL_TBL{
		CString		cDelTable;											// �폜�Ώۂ̎��уe�[�u������
		CString		cKeyName;											// ��L�[���ږ���
		CString		cSubName;											// �T�u�L�[���ږ��� (�ꊇ�폜���A�w�薳��)
		int			nDelRow;											// 1��̍폜�s��	(�ꊇ�폜���A0)			
	};
	// �w�茏���ێ��폜�p
	struct DEL_TBL_CNT : implements DEL_TBL{
		int			nMaxCnt;											// �ێ������		
	};


	//// �K�v�ȃp�����[�^�\����
	struct PARAM_INF {
		int			nLogSaveDay;										// ���O�ۑ�����

		int			nDelCoilCnt;										// 1��̏����ł̍폜�R�C������
		char		sCoilResultTbl[64];									// �R�C�����уe�[�u��
		char		sCoilResultKey[64];									// �R�C�����т̎�L�[
		char		sCoilResultSort[64];								// �R�C�����т̃\�[�g�L�[

	};

public:
	KizuDeleteManager(int id, int* nWaitTime, HANDLE evEndCycle, HANDLE evEndSizeMin, CString cSession, CString cOdbcSession);
	virtual ~KizuDeleteManager(void);

	void Start() { WorkThread::Start(MainThread); };					// �X���b�h�J�n
	void Stop()  { WorkThread::Stop(10000); };							// �X���b�h�I��
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ���O�t�@�C���Z�b�g
	void SetLockMgr(LockManager* pLock)	 { mcls_pLock = pLock;}			// �X���b�h�ԓ�������N���X

	void SetParamInf(PARAM_INF* prm) { memcpy(&my_Param, prm, sizeof(my_Param)); }	// �p�����[�^�ݒ�

	//// �O���A�N�Z�X
	void SetStop()				{ my_bStopFlg = true; SetEvent(my_evStop);}	// �I���v��
	void SetEvCycle()			{ SetEvent(my_evCycle); }				// ��������s�C�x���g
	void SetEvSizeMin() 		{ SetEvent(my_evSizeMin); }				// DB�e�ʒቺ���s�C�x���g
	void SetEvExecFast() 		{ SetEvent(my_evExecFast); }			// ������s�C�x���g
	bool IsExec_Cycle() const	{ return my_bIsExec_Cycle; }			// �������s�� (����������Ŏ��s)
	
	int	 GetTranSizeMin() const { return my_nTranSizeMin;}				// �g�����U�N�V�������O�T�C�Y �ŏ���`
	int	 GetTranSizeNow() const { return my_nTranSizeNow;}				// �g�����U�N�V�������O�T�C�Y ����


	//// ��ʗp
	bool IsExec() const			{ return (my_bIsExec_Cycle||my_bIsExec_SizeMin) ? true : false ;}	// �������s��


protected :
	static UINT WINAPI MainThread(LPVOID param);						// ���C���X���b�h

    // //////////////////////////////////////////////////
    // �������z�֐� (�K�� �p����Ŏ������鎖)
    // //////////////////////////////////////////////////
	virtual void ExecCycle()   = 0;										// ��������s���\�b�h
	virtual void ExecSizeMin() = 0;										// DB�e�ʒቺ���s���\�b�h
	virtual void ExecFast() = 0;										// ������s���\�b�h


	// �擾�֘A
	bool Select_OldKey(char const *tbl, char const *keyName, char const *keySort, int size, char *keyVal); // �w��e�[�u���̍ŌẪL�[���擾
	bool Select_OldKey(char const *tbl, char const *keyName, int size, char *keyVal);
	bool Select_OldKey(char const *tbl, char const *keyName, int *keyVal);
	bool Select_OldKey(char const *tbl, char const *keyName, char const *keySort, int *keyVal);
	bool Select_SubKey(char const *tbl, char const *keyName, char const *subName, char const *keyVal, int size, char *subVal);	// �w��e�[�u���̃T�u�L�[���擾
	bool Select_SubKey(char const *tbl, char const *keyName, char const *subName, char const *keyVal, int *subVal);

	// �f�[�^�e�ʊǗ�
	bool Exec_Shrink();													// �g�����U�N�V�������O ���k
	bool Delete_Log(int nDelLogDay);									// Log�폜����
	bool Delete_Day(char const *tbl, char const *keyDayName, int day);	// �w������ȑO�f�[�^���ꊇ�폜
	bool Delete_Day2(char const *tbl, char const *keyDayName, int day);	// �w������ȑO�f�[�^���P���P�ʂō폜
	bool Delete_Cnt(char const *tbl, char const *keyName, char const *subName, int delCnt, int nMaxNum);	// �w�茏���ȏ�̍폜

	// Old
	bool Delete_Inf(DEL_TBL const *dtbl, char const *keyVal);			// �w���L�[�� �ꊇ�폜/���ʍ폜 �G���g���[
	bool Delete_Inf(char const *tbl, char const *keyName, char const *keyVal);									// �w���L�[�� �ꊇ�폜 (1�R�C����)
	bool Delete_Inf(char const *tbl, char const *keyName, char const *subName, int delCnt, char const *keyVal); // �w���L�[�� ���ʍ폜

	// Inf
	bool Delete_OldInf(DEL_TBL const *dtbl, char const *keyVal);								// �ꊇ�폜/���ʍ폜 �G���g���[ (�w��L�[���Â����т��폜����)
	bool Delete_OldInf(char const *tbl, char const *keyName, char const *keyVal);				// �w��L�[���Â����т��ꊇ�폜
	bool Delete_OldInf(char const *tbl, char const *keyName, char const *subName, int delCnt, char const *keyVal); // �w��L�[���Â����т����ʍ폜



	//// �e�C���X�^���X
	LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X
	LockManager*			mcls_pLock;									// ���b�N�}�l�[�W���[


	//// �����V�O�i��
	HANDLE					my_evCycle;									// ��������s�C�x���g
	HANDLE					my_evSizeMin;								// DB�e�ʒቺ���s�C�x���g
	HANDLE					my_evExecFast;								// ������s�C�x���g

	// �O������Z�b�g
	HANDLE					my_evEndCycle;								// ����������C�x���g
	HANDLE					my_evEndSizeMin;							// DB�e�ʒቺ�����C�x���g


	//// ���̑��������
	int						my_nID;										// ������ID (1�I���W��)
	int*					my_pnWaitTime;								// �ꎞ��~����
	bool					my_bIsExec_Cycle;							// �������s�� (����������Ŏ��s)
	bool					my_bIsExec_SizeMin;							// �������s�� (DB�e�ʒቺ�����Ŏ��s)
	bool					my_bStopFlg;								// �I���v���� true

	//// ini�t�@�C�� & �ݒ�l
	CString					my_cSession;								// �����̐ݒ肪��`����Ă���ini�t�@�C���̃Z�b�V����
	CString					my_cOdbcSession;							// �ڑ�����ODBC����`����Ă�Z�b�V����
	PARAM_INF				my_Param;									// �����p�����[�^
	/*
	int						my_nIniLogDay;								// ���O�ۑ�����
	int						my_nIniDelCoilCnt;							// 1��̏����ł̍폜�R�C������
	CString					m_cIniCoilResultTbl;						// �R�C�����уe�[�u��
	CString					m_cIniCoilResultKey;						// �R�C�����т̎�L�[
*/


	//// �g�����U�N�V�������O�`�F�b�N�֌W
	int						my_nTranSizeMin;							// �g�����U�N�V�������O�T�C�Y �ŏ���`
	int						my_nTranSizeNow;							// �g�����U�N�V�������O�T�C�Y ����
};
