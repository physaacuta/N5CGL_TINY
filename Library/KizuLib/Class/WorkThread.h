// *********************************************************************************
//	�X���b�h �N���X
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//
// *********************************************************************************

//#ifndef KIZULIB_WORKTHREAD_H
//#define KIZULIB_WORKTHREAD_H

#pragma once

#include <windows.h>

namespace KizuLib
{
	class WorkThread
	{
	public:
		WorkThread(DWORD postTid = 0);						// �R���X�g���N�^
		virtual ~WorkThread(void);							// �f�X�g���N�^


		BOOL Start(UINT (__stdcall threadFunc(void*)), int nPriority = THREAD_PRIORITY_NORMAL);		// �X���b�h���s (p�L��)
		BOOL Start(int nPriority = THREAD_PRIORITY_NORMAL);											// �X���b�h���s (p����)
		void Stop();																		// �X���b�h��~
		void Stop(int interval);															// �X���b�h��~
		void StopMSG(int msgNo, int interval = 10000);										// �X���b�h��~ (PostThreadMessage�ŏI���ʒm)

		void StopSignal() { SetEvent(my_evStop); };											// �X���b�h��~�V�O�i���̂ݎ��s


		// �v���p�e�B
		bool  IsThreadRun();								// �X���b�h���s�����`�F�b�N (���s��true)
		bool  IsThreadEvOn() { return my_bThreadEvOn;}		// Start���s�� true, Stop���s�� false

		DWORD GetTid() const { return my_Tid;}				// ���X���b�hID
		void  SetTid() { my_Tid = GetCurrentThreadId(); }	// ���X���b�hID�Z�b�g
		static void  SetThreadName(const char *name, DWORD tid=-1);	// VC�f�o�b�K�E�C���h�E�́A�u�X���b�h���v��ύX����

		DWORD GetMainPostTid() const { return my_PostTid;}	// PostThreadMessage�Œʒm�����
		void  SetMainPostTid(int tid) {my_PostTid = tid; }	// PostThreadMessage�Œʒm����� (�悭MainInctance�Ƀ��b�Z�[�W���΂����Ɏg�p����)	


	protected :
	
		virtual UINT MainThread(void);// = 0;				// �X���b�h�����s���郁���o�֐� (�p����ɓ��ꖼ�̊֐����K�v)
		static  UINT WINAPI m_thread_entry(LPVOID p);		// AfxBeginThread�ɓn���֐�


		//CWinThread* my_pThread;							// �X���b�h�C���X�^���X
		HANDLE		my_hThread;								// �X���b�h�n���h��
		HANDLE		my_evStop;								// �I���V�O�i��
		DWORD		my_Tid;									// ���X���b�hID
		DWORD		my_PostTid;								// PostThreadMessage�Œʒm����� (�悭MainInctance�Ƀ��b�Z�[�W���΂����Ɏg�p����)	
		bool		my_bThreadEvOn;							// Start���s�� true, Stop���s�� false
	



		// ���C���X���b�h���s�ҋ@
	public:
		BOOL StartLock(UINT (__stdcall threadFunc(void*)), int nPriority = THREAD_PRIORITY_NORMAL, int nWaitTime=3000);		// ���s�҂��ŃX���b�h���s (p�L��)
		BOOL StartLock(int nPriority = THREAD_PRIORITY_NORMAL, int nWaitTime=3000);		// ���s�҂��ŃX���b�h���s (p����)
	protected:
		HANDLE		my_hThreadReady;						// �z���g�ɃX���b�h�����s����܂őҋ@�p

	};
};
//#endif