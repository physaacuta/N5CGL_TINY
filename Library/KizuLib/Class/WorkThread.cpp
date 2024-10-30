#include "Stdafx.h"
#include "WorkThread.h"

#include <process.h>

using namespace KizuLib;

/*
//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
WorkThread::WorkThread(void) :
my_pThread(NULL),
my_Tid(0),
my_PostTid(0)
{
	my_evStop = CreateEvent(NULL, FALSE, FALSE, NULL);
}
*/
//------------------------------------------
// �R���X�g���N�^
// DWORD postTid PostThreadMessage�Œʒm����� (�悭MainInctance�Ƀ��b�Z�[�W���΂����Ɏg�p����)	
//------------------------------------------
WorkThread::WorkThread(DWORD postTid) :
my_hThread(NULL),
my_hThreadReady(INVALID_HANDLE_VALUE),
my_Tid(0),
my_bThreadEvOn(false),
my_PostTid(postTid)
{
	my_evStop = CreateEvent(NULL, FALSE, FALSE, NULL);
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
WorkThread::~WorkThread(void)
{
	CloseHandle(my_evStop);
}

//------------------------------------------
// �X���b�h�����s�����`�F�b�N (Stop���W���[�����R�[�����ĂȂ��Ă����m�ɔ��f�\)
// �߂�l : true:���s��  false:�X���b�h�������Ă��Ȃ�
//------------------------------------------
bool WorkThread::IsThreadRun()
{
	//// �������� �X���b�h�n���h��������H
	if( NULL==my_hThread ) return false;
	
	//// ���s�����`�F�b�N
	// my_hThread ��NULL�����Ń`�F�b�N����ƁAStop()���R�[�������ɁA����my_evStop�V�O�i���𔭍s�����ꍇ�A����ɔc���ł��Ȃ�
	if( WAIT_TIMEOUT == WaitForSingleObject(my_hThread, 0))	return true;	// �^�C���A�E�g = �X���b�h�������Ă���
	else													return false;
}

//------------------------------------------
// �X���b�h���s (p�L��)
// UINT (__stdcall threadFunc(void*)) �t�����h���\�b�h�̃A�h���X
// int nPriority �X���b�h�D��x
//------------------------------------------
BOOL WorkThread::Start(UINT (__stdcall threadFunc(void*)), int nPriority)
{
	ResetEvent(my_evStop);
	unsigned int tid;
	my_hThread = (HANDLE)_beginthreadex(NULL, 0, threadFunc, (LPVOID)this, 0, &tid);
	if(NULL == my_hThread) return FALSE;
	my_bThreadEvOn = true;

	// �X���b�h�D��x�w��
	SetThreadPriority(my_hThread, nPriority);
	return TRUE;
}

//------------------------------------------
// �X���b�h���s (p����)
// int nPriority �X���b�h�D��x
//------------------------------------------
BOOL WorkThread::Start(int nPriority)
{
	ResetEvent(my_evStop);
	unsigned int tid;
	my_hThread = (HANDLE)_beginthreadex(NULL, 0, m_thread_entry, (LPVOID)this, 0, &tid);
	if(NULL == my_hThread) return FALSE;
	my_bThreadEvOn = true;

	// �X���b�h�D��x�w��
	SetThreadPriority(my_hThread, nPriority);
	return TRUE;
}




//------------------------------------------
// �X���b�h���s (p�L��)
// UINT (__stdcall threadFunc(void*)) �t�����h���\�b�h�̃A�h���X
// int nPriority �X���b�h�D��x
// int nWaitTime �ő�҂�����
//------------------------------------------
BOOL WorkThread::StartLock(UINT (__stdcall threadFunc(void*)), int nPriority, int nWaitTime)
{
	// ���s�҂��V�O�i��
	my_hThreadReady = CreateEvent(NULL, FALSE, FALSE, NULL);

	// �ʏ�̃X���b�h�@�\
	if( ! Start(threadFunc, nPriority) ) {
		CloseHandle(my_hThreadReady);	my_hThreadReady=INVALID_HANDLE_VALUE;
		return FALSE;
	}

	// �ҋ@
    DWORD wait = WaitForSingleObject(my_hThreadReady, nWaitTime);
	BOOL ret = WAIT_OBJECT_0 == wait ? TRUE : FALSE;
	CloseHandle(my_hThreadReady);	my_hThreadReady=INVALID_HANDLE_VALUE;
	return ret;
}
//------------------------------------------
// ���s�҂��ŃX���b�h���s (p����)
// int nPriority �X���b�h�D��x
// int nWaitTime �ő�҂�����
//------------------------------------------
BOOL WorkThread::StartLock(int nPriority, int nWaitTime)
{
	// ���s�҂��V�O�i��
	my_hThreadReady = CreateEvent(NULL, FALSE, FALSE, NULL);

	// �ʏ�̃X���b�h�@�\
	if( ! Start(nPriority) ) {
		CloseHandle(my_hThreadReady);	my_hThreadReady=INVALID_HANDLE_VALUE;
		return FALSE;
	}

	// �ҋ@
    DWORD wait = WaitForSingleObject(my_hThreadReady, nWaitTime);
	BOOL ret = WAIT_OBJECT_0 == wait ? TRUE : FALSE;
	CloseHandle(my_hThreadReady);	my_hThreadReady=INVALID_HANDLE_VALUE;
	return ret;
}





//------------------------------------------
// p�����p �X���b�h�ɓn���֐�
// int nPriority �X���b�h�D��x
//------------------------------------------
UINT WINAPI WorkThread::m_thread_entry(LPVOID parg)
{
    WorkThread* p = (WorkThread*)parg;
 	//return p->MainThread();			// �p����̎��̂����s
	UINT ret = p->MainThread();			// �p����̎��̂����s
	
	// �X���b�h���s����
	if (INVALID_HANDLE_VALUE != p->my_hThreadReady) { SetEvent(p->my_hThreadReady); }		// ���s�ҋ@��

	return ret;
}

//------------------------------------------
// �X���b�h (�p����Ŏ������鎖)
//------------------------------------------
UINT WorkThread::MainThread()
{
	// ����ŗ�O�ƂȂ�ꍇ�́AStartLock���Ή��Ȃ̂ŁAStart()�ɕύX���邱�ƁB
	throw;				// ��O����������悤�ɕύX�Bp�L��Ő̃��C�u���������Ղ�StartLock�ɂ���ƃ_���Ȃ̂�
	return 0;
}

//------------------------------------------
// �X���b�h��~
// int interval �����I���܂ł̑҂����� [ms]
//------------------------------------------
void WorkThread::Stop(int interval)
{
	if (my_hThread == NULL) return;		// �X���b�h����
	SetEvent(my_evStop);				// ��~�V�O�i��
	my_bThreadEvOn = false;
	if (WAIT_TIMEOUT == WaitForSingleObject(my_hThread, interval)) {
		// �ُ�ׁ̈A�����I�ɃX���b�h�I��
		TerminateThread(my_hThread, -1);
	} else {
		// ����ׁ̈A�n���h�����
		CloseHandle(my_hThread);
	}
	my_hThread = NULL;
}

//------------------------------------------
// �X���b�h��~
//------------------------------------------
void WorkThread::Stop()
{
	// �f�t�H���g�҂�����
	Stop(10000);
}

//------------------------------------------
// �X���b�h��~ (PostThreadMessage�ŏI���ʒm)
// int msgNo �ʒm���b�Z�[�WNo
// int interval = 10000 �����I���܂ł̑҂����� [ms]
//------------------------------------------
void WorkThread::StopMSG(int msgNo, int interval)
{
	// �f�t�H���g�҂�����
	PostThreadMessage(my_Tid, msgNo, 0, 0);
	Stop(interval);
}

//------------------------------------------
// VC�f�o�b�K�E�C���h�E�́A�u�X���b�h���v��ύX����
// const char *name �f�o�b�J�ɕ\�����閼��
// DWORD tid �ΏۃX���b�hID �����Ȃ�-1
//------------------------------------------
void WorkThread::SetThreadName(const char *name, DWORD tid)
{
	struct ThreadName{
		DWORD dwType;
		LPCSTR szName;
		DWORD dwThreadID;
		DWORD dwFlags;
	};
	ThreadName threadname = { 0x1000, name, tid, 0 };
	__try{
		RaiseException( 0x406d1388, 0, sizeof( threadname ) / sizeof(DWORD) , (ULONG_PTR*)&threadname );
	}__except( EXCEPTION_CONTINUE_EXECUTION )
	{}
}