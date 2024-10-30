// *********************************************************************************
//	���[�J�[�X���b�h��p�����X���b�h��{�N���X
//	[Ver]
//		Ver.01    2009/04/10  vs2005 �Ή�
//
//	[����]
//		�V�O�i�����ɃR�[���o�b�N�����
// *********************************************************************************
#pragma once

#include <process.h>

// ���Œ�
#include "..\..\..\Include\KizuCommon.h"								// �r���C���N���[�h
#include "..\..\KizuLib\KizuLib.h"										// �r�����C�u����
#include "..\..\KizuLib\Class\KizuMacro.h"								// �}�N���W
#include "..\..\KizuLib\Class\KizuFunction.h"							// �X�^�e�B�b�N���C�u�����W
#include "..\..\KizuLibMFC\Class\LogFileManager.h"						// ���O�Ǘ��N���X

// �r���N���X �C���N���[�h
#include "..\..\KizuLib\Class\WorkThread.h"								// ���[�J�[�X���b�h�N���X


using namespace KizuLib;

class ThreadManager : public WorkThread
{
protected :
	const int PARAM_MAX_WAIT_NUM;										// WaitFor�ő�҂���


public:
	ThreadManager(const char* pThreadName="", int nMaxWaitNum = 50);	// �R���X�g���N�^
	virtual ~ThreadManager(void);										// �f�X�g���N�^

	void Start(int nPriority = THREAD_PRIORITY_NORMAL)					// �X���b�h�J�n
		{ 
			WorkThread::Start(MainThread, nPriority); 
		};
	bool StartLock(int nPriority = THREAD_PRIORITY_NORMAL, int nWaitTime=3000)	// ���s�ҋ@�ŃX���b�h�J�n
		{
			return  !! WorkThread::StartLock(MainThread, nPriority, nWaitTime); 
		};
	void Stop()							 {WorkThread::Stop(); };		// �X���b�h�I��
	void Stop(int interval)				 {WorkThread::Stop(interval);};	// �X���b�h�I��
	void SetLogMgr(LogFileManager* pCls) {mcls_pLog = pCls;}			// ���O�N���X�Z�b�g

	char const*		GetThreadName()		 {return my_sThreadName.GetBuffer();}		// ���X���b�h��

	// �V�O�i���ǉ�����
	bool SetEventNum(int num, LPHANDLE pWait);							// �C�x���g�Z�b�g
	bool AddEventNum(HANDLE pWait);										// �C�x���g�̒ǉ� (1�R����)
	bool AddEventNum(int num, LPHANDLE pWait);							// �C�x���g�̒ǉ� (�z��)
	// �V�O�i������
	bool ChangeWaitNum(int num);										// WaitFor�̑҂�����ύX (�ʐM�^�X�N���Ŏg�p�����)
//	int  GetDefaultWaitNum() const {return my_nWaitNum;}				// �{����WaitFor�̑҂���

protected:
	static UINT WINAPI MainThread(LPVOID param);						// ���C���X���b�h

	virtual int ThreadFirst() = 0;										// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	virtual int ThreadLast() = 0;										// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
	virtual void ThreadWaitFor(int nDefWaitNum, int* nNowWaitNum) {};	// �X���b�h��WaitFor�҂��̒��O����
	virtual void ThreadEvent(int nEventNo) = 0;							// �X���b�h�C�x���g���� (nEventNo��0�I���W��)
	virtual int	ThreadMainLoop();										// �X���b�h���C�����[�v


	// �e�C���X�^���X
	LogFileManager*				mcls_pLog;								// LogFileManager�X���b�h�C���X�^���X

	// �V�O�i��
	HANDLE*						my_pEvWait;								// WaitFor�҂��V�O�i��
	int							my_nWaitNum;							// WaitFor�҂��� (Stop�p�̃V�O�i���͉��Z���ĂȂ�)
	int							my_nWaitNumNow;							// �����WaitFor�҂���

	// ���̑��������
	CString						my_sThreadName;							// ���X���b�h��
};
