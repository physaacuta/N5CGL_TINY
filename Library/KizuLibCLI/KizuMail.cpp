// ����� ���C�� DLL �t�@�C���ł��B

#include "stdafx.h"
#include "KizuMail.h"
#include <crtdbg.h>

using namespace System;
using namespace System::Runtime::InteropServices;

using namespace KizuLibCLI;

//------------------------------------------
// �R�X�g���N�^
//------------------------------------------
KizuMail::KizuMail() :
mcls_thread(nullptr),
m_evStop(nullptr)
{
}
//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
KizuMail::~KizuMail(void)
{
	// �c�[���c��Ȃ��悤�ɊJ��
	Task_Exit();
}


//------------------------------------------
// �^�X�N����
// String^ cTaskName ���^�X�N����
// �߂�l ���A���
//------------------------------------------
int KizuMail::Task_Init(String^ cTaskName)
{
	IntPtr pTaskName = Marshal::StringToHGlobalAnsi(cTaskName);			// ansic�ɕϊ���̕����|�C���^
	int retc = task_init((LPCSTR)pTaskName.ToPointer());
	Marshal::FreeHGlobal(pTaskName);

	if( 0 == retc ) {
		// �Ǎ��X���b�h����
		m_evStop = gcnew Threading::AutoResetEvent(false); 
		mcls_thread = gcnew Threading::Thread(gcnew Threading::ParameterizedThreadStart(KizuMail::MainThread)); 
		mcls_thread->Start(this);		// ���|�C���^��n���Ă�����
	}
	return retc;
}

//------------------------------------------
// �^�X�N����
// String^ cTaskName ���^�X�N����
// String^ cPcName ��PC����
// �߂�l ���A���
//------------------------------------------
int KizuMail::Task_Init(String^ cTaskName, String^ cPcName)
{
	IntPtr pTaskName = Marshal::StringToHGlobalAnsi(cTaskName);			// ansic�ɕϊ���̕����|�C���^
	IntPtr pPcName = Marshal::StringToHGlobalAnsi(cPcName);			// ansic�ɕϊ���̕����|�C���^
	int retc = task_init((LPCSTR)pTaskName.ToPointer(), (LPCSTR)pPcName.ToPointer());
	Marshal::FreeHGlobal(pTaskName);
	Marshal::FreeHGlobal(pPcName);

	if( 0 == retc ) {
		// �Ǎ��X���b�h����
		m_evStop = gcnew Threading::AutoResetEvent(false); 
		mcls_thread = gcnew Threading::Thread(gcnew Threading::ParameterizedThreadStart(KizuMail::MainThread)); 
		mcls_thread->Start(this);		// ���|�C���^��n���Ă�����
	}
	return retc;
}

//------------------------------------------
// �^�X�N�I��
// �߂�l ���A���
//------------------------------------------
void KizuMail::Task_Exit()
{
	// �X���b�h��~
	if (nullptr != mcls_thread) {
		m_evStop->Set();
		mcls_thread = nullptr;
	}

	// ���[���X���b�g�J��
	task_exit();
}

//------------------------------------------
// ���[�����M
// String^ SendSlotName ���M��^�X�N��
// String^ SendPcName ���M��PC����
// Object^ pDate ���M�f�[�^�̃|�C���^
// �߂�l ���A���
//------------------------------------------
int KizuMail::Send_Mail(String^ SendSlotName, String^ SendPcName, Object^ pDate)
{
	IntPtr pTaskName = Marshal::StringToHGlobalAnsi(SendSlotName);		// ansic�ɕϊ���̕����|�C���^
	IntPtr pPcName = Marshal::StringToHGlobalAnsi(SendPcName);		// ansic�ɕϊ���̕����|�C���^

	IntPtr p = Marshal::AllocHGlobal( SIZE_MSL );
	Marshal::StructureToPtr( pDate, p, true);
	int retc = send_mail((const char*)pTaskName.ToPointer(), (const char*)pPcName.ToPointer(), p.ToPointer() );

	Marshal::FreeHGlobal(pTaskName);
	Marshal::FreeHGlobal(pPcName);
	Marshal::FreeHGlobal(p);
	return retc;
}
/*
//------------------------------------------
// ���[����M
// Object^% pDate ��M�f�[�^�̃|�C���^
// Type^ structureType ��荞�ލ\����
// �߂�l ���A���
//------------------------------------------
int KizuMail::Recv_Mail(Object^% pDate, Type^ structureType )
{
	IntPtr p = Marshal::AllocHGlobal( SIZE_MSL );

	int retc =  recv_mail(p.ToPointer());

	pDate = Marshal::PtrToStructure(p, structureType);        //����������f�[�^���擾���A�\���̂փZ�b�g
	Marshal::FreeHGlobal(p);
	return retc;
}
*/

//------------------------------------------
// �N���X�ϊ�
// Object^ pTo �R�s�[�� (new ���Ă����K�v�L��B�������Ainisialize�͂��Ă����K�v����)
// Object^ pFrom �R�s�[��
//------------------------------------------
void KizuMail::typConv(Object^ pTo, Object^ pFrom)
{
	IntPtr p = Marshal::AllocHGlobal(Marshal::SizeOf(pFrom));
	Marshal::StructureToPtr(pFrom, p, true);					// ��������Ƀf�[�^���Z�b�g
	Marshal::PtrToStructure(p, pTo);							// ����������f�[�^���擾���A�\���̂փZ�b�g
	Marshal::FreeHGlobal(p);
}
//------------------------------------------
// �\���̕ϊ�
// Object^ pFrom �R�s�[��
// �߂�l �\����
//------------------------------------------
Object^ KizuMail::typConv(Object^ pFrom, Type^ structureType)
{
	IntPtr p;
	try {
		p = Marshal::AllocHGlobal(Marshal::SizeOf(pFrom));
		Marshal::StructureToPtr(pFrom, p, true);                   // ��������Ƀf�[�^���Z�b�g
		return (ValueType^)Marshal::PtrToStructure(p, structureType);			// ����������f�[�^���擾���A�\���̂փZ�b�g
	} catch( char * ) {
		return nullptr;
	} finally {
		Marshal::FreeHGlobal(p);
	}
}
//------------------------------------------
// ��M�p�X���b�h
// Object^ arg ���C���X�^���X
// �߂�l ���A���
//------------------------------------------
void KizuMail::MainThread(Object^ arg)
{
	KizuMail ^ p = (KizuMail ^) arg;			// ���C���X�^���X�擾

	// �񓯊���M�p
	int retc;
	OVERLAPPED ovlRecv;
	memset(&ovlRecv, 0, sizeof(OVERLAPPED));
	ovlRecv.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);		// manual reset

	// �C�x���g�쐬
	HANDLE evArray[2] = {p->m_evStop->Handle.ToPointer(), ovlRecv.hEvent}; 
	enum { EV_STOP = WAIT_OBJECT_0,			// �I���ʒm
		   EV_OVL_RECV,						// �񓯊���M����
		   EV_END};

	// ��M�p�̈�
	IntPtr pRecvBuf = Marshal::AllocHGlobal( SIZE_MSL );
	
	// �񓯊���M�J�n
	retc = recv_mail_ovl( pRecvBuf.ToPointer(), &ovlRecv);
	if( !retc ) {
		_ASSERTE(GetLastError() == ERROR_IO_PENDING);
	}

	while (true) {
		DWORD dwWait = WaitForMultipleObjects(EV_END, evArray, FALSE, INFINITE);
		
		if( EV_STOP == dwWait ) {								// �I��
			Marshal::FreeHGlobal(pRecvBuf);
			return;

		} else if ( EV_OVL_RECV == dwWait ) {					// �񓯊���M����
			
			// �Ƃ肠������{�̌^�����̌`�Ƃ��� (�g�����ŏ���Ɍ^�ϊ����Ă�)
			COMMON_QUE_L^ que = gcnew COMMON_QUE_L();
			que = (COMMON_QUE_L^)Marshal::PtrToStructure( pRecvBuf, que->GetType() );

			// �C�x���g���s (�u���b�L���O)
			p->evRecvMail(que, que->nEventNo);
			// ���񓯊���M�J�n
			retc = recv_mail_ovl( pRecvBuf.ToPointer(), &ovlRecv);
			if( !retc ) {
				//_ASSERTE(GetLastError() == ERROR_IO_PENDING);
				continue;
			}
		} else {
			// ���肦�Ȃ�
		}
	}
}