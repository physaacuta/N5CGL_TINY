
#pragma once


// �C���N���[�h
#include <process.h>

// �r�� �C���N���[�h
#include "..\..\include\KizuCommon.h"									// �r�����ʃw�b�_�[
#include "..\..\Library\KizuLib\KizuLib.h"								// �r�����C�u�����w�b�_�[

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLib\CLASS\KizuMacro.h"						// �r���}�N��
#include "..\..\Library\KizuLib\CLASS\WorkThread.h"						// ���[�J�[�X���b�h
#include "..\..\Library\KizuLib\Class\LockManager.h"					// �X���b�h�ԓ�������N���X
#include "..\..\Library\KizuLibMFC\CLASS\LogFileManager.h"				// ���O�}�l�[�W���[

// �r�����C�u���� �C���N���[�h
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC����N���X


// �e���[�J�[�X���b�h
#include "DeleTable.h"													// DB�f�[�^�폜�N���X


using namespace KizuLib;
using namespace KizuODBC;

class ControlManager : public WorkThread
{
public:
	//// �f�B�X�N�e��
	typedef struct {
		double		dTotalSize;											// �f�B�X�N�̑��e�� [Mbyte]
		double		dFreeSize;											// �f�B�X�N�̋󂫗e�� [Mbyte]
		double		dFreeRitu;											// �f�B�X�N�̋󂫗� [%]
	} DISK_SPACE;


private:
	//------------------------------------------
	// �Œ�萔
	//------------------------------------------
	static const int THREAD_MAXCNT				= 9;					// �ő�DB���s�X���b�h��


public:
	ControlManager(char* cSession, LogFileManager* pLog);
	virtual ~ControlManager(void);


	void Start() { WorkThread::Start(MainThread); };					// �X���b�h�J�n
	void Stop()  { WorkThread::Stop(20000); };							// �X���b�h�I��

	void WorkerInit();													// �폜�X���b�h����������
	void WorkerStop();													// �폜�X���b�h��~����
	void WorkerExit();													// �폜�X���b�h�I������


	void SetWaitMode(bool mode);										// �폜/���� �D�惂�[�h (true:�폜�D��)


	// ��ʃA�N�Z�X�p
	char const*	GetSession() const { return m_cSession; };				// �Z�b�V��������
	char const* GetOdbcSession() const { return m_cOdbcSession; };		// �ڑ�����ODBC����`����Ă�Z�b�V����
	const DISK_SPACE& GetDiskSpace() const {return m_DiskSpace;}		// �f�B�X�N�e�ʏ�Ԏ擾
	bool GetWaitMode() const {return m_bWaitMode;}						// �폜/���� �D�惂�[�h (true:�폜�D��)
	int  IsExec(int rec) const	   { if(NULL != mcls_pDele[rec]) { return mcls_pDele[rec]->IsExec()? 1: 0; } else { return -1;} }	// �������s��
	int  IsExec() const	   { if(NULL != mcls_pCycle) { return mcls_pCycle->IsExec()? 1: 0; } else { return -1;} }	// �������s��


private:
	static UINT WINAPI MainThread(LPVOID param);						// ���C���X���b�h

	void CheckDB_Size();												// DB�̎g�p�e�ʃ`�F�b�N
	int  GetDB_Size();													// DB�̎g�p�e�ʎ擾
	void Check_TranSize();												// �g�����U�N�V�������O�T�C�Y�`�F�b�N


	// �e���[�J�[�X���b�h
	LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X
	DeleTable*				mcls_pDele[THREAD_MAXCNT];					// DB�폜�Ǘ��N���X (�e�ʒቺ�p)
	DeleTable*				mcls_pCycle;								// ��������s�p
	LockManager				mcls_Lock;									// ���b�N�}�l�[�W���[

	//// �����V�O�i��
	HANDLE					m_evEndCycle;								// ������`�F�b�N�����V�O�i��
	HANDLE					m_evEndSizeCheck;							// DB�e�ʃ`�F�b�N�����V�O�i��

	//// �N���e�B�J���Z�N�V����
	CRITICAL_SECTION		m_csLock;									// �q�X���b�h�� �������ڍ폜�p

	// ���̑��������
	DISK_SPACE				m_DiskSpace;								// ���݂̃f�B�X�N�e�ʏ��
	bool					m_bWaitMode;								// �폜/���� �D�惂�[�h (true:�폜�D��)
	int						m_nWaitTime;								// ���݂̑҂����� [ms]
	EM_DIV_BOOL				m_bTranStat;								// �g�����U�N�V�����`�F�b�N��� (-1:���� 1:���� 0:�ُ�)
	bool					m_bExecFast;								// ������s��true

	//// ini�t�@�C��
	CString					m_cSession;									// �����̐ݒ肪��`����Ă���ini�t�@�C���̃Z�b�V����
	CString					m_cOdbcSession;								// �ڑ�����ODBC����`����Ă�Z�b�V����
	int						m_nIniTranSizeErrID;						// �g�����U�N�V�������O�ُ�@��ID (0:�`�F�b�N����) 
	int						m_nIniTranErrSize;							// �g�����U�N�V�����G���[�F���T�C�Y

	long					m_nIniTimeCycle;							// ��������s���� [ms]
	long					m_nIniTimeSizeCheck;						// DB�e�ʃ`�F�b�N���� [ms]
	int						m_nIniThreadCount;							// DB�e�ʒቺ���̍폜�X���b�h��
	double					m_dIniDeleteRitu[THREAD_MAXCNT];			// �󂫗e�ʗ�
	int						m_nIniTimeMaxWait;							// �����D�掞�̑҂����� [ms]
	int						m_nIniTimeMinWait;							// �폜�D�掞�̑҂����� [ms]
//	int						m_nIniLogDay;								// ���O�ۑ�����
//	int						m_nIniDelCoilCnt;							// 1��̏����ł̍폜�R�C������ (0�����A���э폜���Ȃ�)

	KizuDeleteManager::PARAM_INF	m_Param;							// �����p�����[�^

};
