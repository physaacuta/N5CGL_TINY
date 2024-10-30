// *********************************************************************************
//	DB�ڑ��m�F�N���X
//	[Ver]
//		Ver.01    2007/04/29  vs2005 �Ή�
//
//	[����]
//		�E�^�X�N�N������DB�ڑ��\���`�F�b�N
// *********************************************************************************

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
//#include "..\KizuLib\Class\AutoLock.h"								// �N���e�B�J���Z�N�V�����N���X
#include "..\..\Library\KizuLib\Class\KizuPerformanceCounter.h"			// ���ԑ���N���X

#include "..\..\Library\KizuODBC\OdbcBase.h"							// DB����N���X

using namespace KizuLib;
using namespace KizuODBC;

class CheckDB :	public WorkThread
{
public :
#define	 E_DEF_DB_FAST_CONNECT				 WM_USER+200				// PostThreadMassage��ID
#define	 E_DEF_DB_NG_TO_OK					 WM_USER+201				// PostThreadMassage��ID


private :
	static const int HEART_BEAT_DBCHECK_CYCLE		= 30000;			// DB��Ԋm�F����
	static const int MAX_SEND_TASK					= 50;				// ���M�^�X�N�ő�o�^����

public:
	CheckDB(bool isMaster);
	virtual ~CheckDB(void);


	void Start() { WorkThread::Start(MainThread); };					// �X���b�h�J�n
	void Stop()  { WorkThread::Stop(10000); };							// �X���b�h�I��
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ���O�t�@�C���Z�b�g

	// �O���A�N�Z�X
	bool GetStateDB()  const {return m_bStateDB; }						// DB�ڑ��\�� true
	bool GetFastExec() const {return m_bFastExec; }						// �����N���� true ��x�ł�DB�ɂȂ����false
	void SetFastExecDebug()  { m_bFastExec=false; }						// �f�o�b�N�p


private :
	static UINT WINAPI MainThread(LPVOID param);						// ���C���X���b�h

	void Get_IniData();													// ini�t�@�C���f�[�^�擾
	void Check_DataBase();												// DB�`�F�b�N
	bool OpenCheckEx();													// DB�ڑ��m�F


	//// �e�C���X�^���X
	LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X

	// �V�O�i��
	HANDLE					m_evDBCheck;								// DB�Ď��^�C�}�C�x���g

	// ���̑��������
	bool					m_bIsMaster;								// PC�}�X�^�[�Ǘ���true
	bool					m_bStateDB;									// DB�̌��݂̏��
	bool					m_bFastExec;								// �����N���� (true:�����N���� false:��x�ł�DB�Ɍq������)
	bool					m_bCheckDB;									// DB�`�F�b�N (true:����)
	int						m_nSendTaskCnt;								// ���M�^�X�N��
	char					m_cSendTaskName[MAX_SEND_TASK][SIZE_NAME_TASK];	// ���M�^�X�N����

};
