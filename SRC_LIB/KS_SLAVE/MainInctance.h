// *********************************************************************************
//	���C���Ǘ��X���b�h
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//		
// *********************************************************************************

#pragma once

// �C���N���[�h
#include <process.h>

// �r�� �C���N���[�h
#include "..\..\include\KizuCommon.h"									// �r�����ʃw�b�_�[
#include "..\..\Library\KizuLib\KizuLib.h"								// �r�����C�u�����w�b�_�[

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLib\CLASS\KizuMacro.h"						// �r���}�N��
#include "..\..\Library\KizuLib\CLASS\WorkThread.h"						// ���[�J�[�X���b�h
#include "..\..\Library\KizuLibMFC\CLASS\LogFileManager.h"				// ���O�}�l�[�W���[

#include "..\..\Library\KizuLib\CLASS\SmemManager.h"					// ���L������
#include "..\..\Library\KizuLib\CLASS\ProcessManager.h"					// PC�E�^�X�N����

// �e���[�J�[�X���b�h
#include "MainConst.h"
#include "..\KS_MASTER\CheckDB.h"										// DB�ڑ��m�F�N���X
#include "..\KS_MASTER\CheckStatus.h"									// �@���ԊǗ��N���X

using namespace KizuLib;


class MainInctance : public WorkThread
{
private:
	// �^�C�}�[
	static const int PARAM_TIMEID_CHECK			= 10;					// ��������s�^�C�}�[ID
	static const int PARAM_TIMEOUT_CHECK		= 3000;					// ��������s����
	static const int PARAM_TIMEOUT_CHECK2		= 5000;					// ��������s���� 2��ڂ̒ʒm

public:
	MainInctance(void);
	virtual ~MainInctance(void);

	void Start() { SetEvent(m_evThStart); };							// �X���b�h�J�n
	void Stop()  { WorkThread::StopMSG(WM_USER_TH_END); };				// �X���b�h�I��
	BOOL Init();														// ����������
	void Exit();														// �I������
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ���O�t�@�C���Z�b�g

	// �e���[�J�[�X���b�h
	CheckStatus*			gcls_pCSta;									// �@���ԊǗ��X���b�h
	CheckDB*				gcls_pCDb;									// �f�[�^�x�[�X�Ǘ��X���b�h

	// �O���A�N�Z�X
	// Set
	void SetStateMaster(bool mode) { m_bStateMaster=mode; }
	// Get
	bool GetStateMaster() const { return m_bStateMaster; }
	bool GetDebug() const { return (0==m_nDebug ? false : true); }

private:
	static UINT WINAPI MainThread(LPVOID param);						// ���C���X���b�h
	
	void Init_Process();												// �v���Z�X �X�^�[�g
	void Exit_Process();												// �v���Z�X �G���h
//	void ExecFileCopy();												// �t�@�C���R�s�[���s
	void Send_MASTER_OK();												// ������ �����グ�����ʒm �A���T�[

	void Exec_Cmd(char const* cIniKey);									// �R�}���h���s



	//// �e�C���X�^���X
	LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X
	KizuLib::SmemManager	mcls_Smem;									// SmemManager�C���X�^���X

	// �����V�O�i��
	HANDLE					m_evThStart;								// MainInctance�X���b�h�̎��s�v��

	// ���̑��������
	int						m_bInitalize;								// ������������true
	bool					m_bStateMaster;								// PC�}�X�^�[�Ǘ�OK�� treu
	int						m_nDebug;									// �f�o�b�N�� (0�F�{�ԗp 1:�f�o�b�N�p(�}�X�^�[�m�F����)  2:�}�X�^�[�m�F+DB�m�F������)
};
