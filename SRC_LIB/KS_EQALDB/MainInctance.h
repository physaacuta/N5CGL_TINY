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
#include <winsock2.h>
#include <process.h>

// �r�� �C���N���[�h
#include "..\..\include\KizuCommon.h"									// �r�����ʃw�b�_�[
#include "..\..\Library\KizuLib\KizuLib.h"								// �r�����C�u�����w�b�_�[

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLib\CLASS\KizuMacro.h"						// �r���}�N��
#include "..\..\Library\KizuLib\CLASS\WorkThread.h"						// ���[�J�[�X���b�h
#include "..\..\Library\KizuLibMFC\CLASS\LogFileManager.h"				// ���O�}�l�[�W���[

// �r�����C�u���� �C���N���[�h
//#include "..\..\Library\KizuSOCK3\SockBase.h"							// �\�P�b�g����N���X
//#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC����N���X
//#include "..\..\Library\Kizu232C\Rs232cBase.h"							// 232C����N���X

#include "..\..\Library\DefectLib\BaseTcpIp.h"							// TCP/IP����N���X

// �e���[�J�[�X���b�h
#include "SyncManager.h"
#include "MoveManager.h"
#include "CopyManager.h"
#include "SyncopyManager.h"

using namespace KizuLib;

class MainInctance : public WorkThread
{
private:

public:
	enum EM_TYPE_CLASS {
		TYPE_DB_SYNC	= 0,											// �}�X�^�[����
		TYPE_DB_COPY,													// ���ђǉ��R�s�[
		TYPE_DB_MOVE,													// ���шړ�
		TYPE_DB_SYNCOPY	,												// ���ъ��S����
		TYPE_DB_END
	};



public:
	MainInctance(void);
	virtual ~MainInctance(void);

	void Start() { SetEvent(m_evThStart); };							// �X���b�h�J�n
	void Stop()  { WorkThread::StopMSG(WM_USER_TH_END); };				// �X���b�h�I��
	BOOL Init();														// ����������
	void Exit();														// �I������
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ���O�t�@�C���Z�b�g


	void AddLinkServer(char const* cKey);								// �����N�T�[�o�[��ݒ�
	void SelectNowExec(int nAtoB, int nMailKey);						// �������s��U�蕪��

	// �e���[�J�[�X���b�h
	int					g_nClassCnt[TYPE_DB_END];							// �e�C���X�^���X�̐�����
	KizuSyncroManager*	gcls_pDb[TYPE_DB_END][KizuSyncroManager::PARAM_MAX_THREAD];	// �e�N���X�̎Q�Ɨp

	SyncManager*		gcls_pSync[KizuSyncroManager::PARAM_MAX_THREAD];	// �}�X�^�[�����N���X
	CopyManager*		gcls_pCopy[KizuSyncroManager::PARAM_MAX_THREAD];	// ���ђǉ��R�s�[�N���X		
	MoveManager*		gcls_pMove[KizuSyncroManager::PARAM_MAX_THREAD];	// ���шړ��N���X
	SyncopyManager*		gcls_pSynCopy[KizuSyncroManager::PARAM_MAX_THREAD];	// ���ѓ����N���X



private:
	static UINT WINAPI MainThread(LPVOID param);						// ���C���X���b�h

	//// �e�C���X�^���X
	LogFileManager*		mcls_pLog;										// LogFileManager�X���b�h�C���X�^���X
		

	// �����V�O�i��
	HANDLE	m_evThStart;												// MainInctance�X���b�h�̎��s�v��

	// ���̑��������
};
