// ����� ���C�� DLL �t�@�C���ł��B

#include "stdafx.h"
#include "KizuMod.h"
#include <crtdbg.h>

#include "..\KizuLib\Class\KizuFunction.h"								// �X�^�e�B�b�N�֐��W

using namespace System;
using namespace System::Runtime::InteropServices;

using namespace KizuLib;
using namespace KizuLibCLI;

//------------------------------------------
// �R�X�g���N�^
//------------------------------------------
KizuMod::KizuMod() :
mcls_thread(nullptr),
m_evStop(nullptr)
{
}
//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
KizuMod::~KizuMod(void)
{
	// �c�[���c��Ȃ��悤�ɊJ��
	Task_Exit();
}

//------------------------------------------
// �^�X�N����
// String^ cTaskName ���^�X�N����
// �߂�l ���A���
//------------------------------------------
int KizuMod::Task_Init(String^ cTaskName)
{
	IntPtr pTaskName = Marshal::StringToHGlobalAnsi(cTaskName);			// ansic�ɕϊ���̕����|�C���^
	int retc = task_init((LPCSTR)pTaskName.ToPointer());
	Marshal::FreeHGlobal(pTaskName);

	if( 0 == retc ) {
		// �Ǎ��X���b�h����
		m_evStop = gcnew Threading::AutoResetEvent(false); 
		mcls_thread = gcnew Threading::Thread(gcnew Threading::ParameterizedThreadStart(KizuMod::MainThread)); 
		mcls_thread->Start(this);		// ���|�C���^��n���Ă�����
	}
	return retc;
}

//------------------------------------------
// �^�X�N����
// String^ cTaskName ���^�X�N����
// int nPCID ��PCID
// �߂�l ���A���
//------------------------------------------
int KizuMod::Task_Init(String^ cTaskName, int nPCID)
{
	IntPtr pTaskName = Marshal::StringToHGlobalAnsi(cTaskName);			// ansic�ɕϊ���̕����|�C���^
	int retc = task_init((LPCSTR)pTaskName.ToPointer(), nPCID);
	Marshal::FreeHGlobal(pTaskName);

	if( 0 == retc ) {
		// �Ǎ��X���b�h����
		m_evStop = gcnew Threading::AutoResetEvent(false);
		mcls_thread = gcnew Threading::Thread(gcnew Threading::ParameterizedThreadStart(KizuMod::MainThread)); 
		mcls_thread->Start(this);		// ���|�C���^��n���Ă�����
	}
	return retc;
}

//------------------------------------------
// �^�X�N�I��
// �߂�l ���A���
//------------------------------------------
void KizuMod::Task_Exit()
{
	// �X���b�h��~
	if (nullptr != mcls_thread) {
		m_evStop->Set();
		mcls_thread->Join();			// �X���b�h���I������܂őҋ@
		delete mcls_thread;
		mcls_thread = nullptr;
		delete m_evStop;
		m_evStop = nullptr;
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
int KizuMod::Send_Mail(String^ SendSlotName, String^ SendPcName, Object^ pDate)
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
// ���[���]��
// String^ SendSlotName ���M��^�X�N��
// String^ SendPcName ���M��PC����
// Object^ pDate ���M�f�[�^�̃|�C���^
// �߂�l ���A���
//------------------------------------------
int KizuMod::Send_Mail_Trans(String^ SendSlotName, String^ SendPcName, Object^ pDate)
{
	IntPtr pTaskName = Marshal::StringToHGlobalAnsi(SendSlotName);		// ansic�ɕϊ���̕����|�C���^
	IntPtr pPcName = Marshal::StringToHGlobalAnsi(SendPcName);		// ansic�ɕϊ���̕����|�C���^

	IntPtr p = Marshal::AllocHGlobal( SIZE_MSL );
	Marshal::StructureToPtr( pDate, p, true);
	int retc = send_mail_trans((const char*)pTaskName.ToPointer(), (const char*)pPcName.ToPointer(), p.ToPointer() );

	Marshal::FreeHGlobal(pTaskName);
	Marshal::FreeHGlobal(pPcName);
	Marshal::FreeHGlobal(p);
	return retc;
}

//------------------------------------------
// ���[���]��
// String^ SendUdpAddr ���M��IP�A�h���X
// String^ SendSlotName ���M��^�X�N��
// String^ SendPcName ���M��PC����
// Object^ pDate ���M�f�[�^�̃|�C���^
// �߂�l ���A���
//------------------------------------------
int KizuMod::Send_Mail_Trans(String^ SendUdpAddr, String^ SendSlotName, String^ SendPcName, Object^ pDate)
{
	IntPtr pUdpAddr = Marshal::StringToHGlobalAnsi(SendUdpAddr);		// ansic�ɕϊ���̕����|�C���^
	IntPtr pTaskName = Marshal::StringToHGlobalAnsi(SendSlotName);		// ansic�ɕϊ���̕����|�C���^
	IntPtr pPcName = Marshal::StringToHGlobalAnsi(SendPcName);		// ansic�ɕϊ���̕����|�C���^

	IntPtr p = Marshal::AllocHGlobal( SIZE_MSL );
	Marshal::StructureToPtr( pDate, p, true);
	int retc = send_mail_trans((const char*)pUdpAddr.ToPointer(), (const char*)pTaskName.ToPointer(), (const char*)pPcName.ToPointer(), p.ToPointer() );

	Marshal::FreeHGlobal(pUdpAddr);
	Marshal::FreeHGlobal(pTaskName);
	Marshal::FreeHGlobal(pPcName);
	Marshal::FreeHGlobal(p);
	return retc;
}

//------------------------------------------
// �N���X�ϊ�
// Object^ pTo �R�s�[�� (new ���Ă����K�v�L��B�������Ainisialize�͂��Ă����K�v����)
// Object^ pFrom �R�s�[��
//------------------------------------------
void KizuMod::typConv(Object^ pTo, Object^ pFrom)
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
ValueType^ KizuMod::typConv(Object^ pFrom, Type^ structureType)
{
	// �߂�l�ō\���̂�Ԃ��̂ɂ� �ȉ��̗��R
	// �E�z���g�͈����� ValueType^% �Ŏ󂯓n������ ���g���b�L���O�|�C���^��
	//   �ł�VB���� Option Strict On �ɂ��Ă���ƁACtype(tbl, ValueType)�Ƃ���� �f�[�^���n��Ȃ��E�E�E
	//   Option Strict Off �ɂ���� ���ʂ� �\���̂̕ϐ� tbl �������ɂ��邾���Ńf�[�^���n��E�E�E
	//   Option Strict Off �ɂ͂������Ȃ��̂ŁA�d���Ȃ� �߂�l�Ƃ���
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
void KizuMod::MainThread(Object^ arg)
{
	KizuMod ^ p = (KizuMod ^) arg;			// ���C���X�^���X�擾

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
	COMMON_QUE_L^ que = gcnew COMMON_QUE_L();

	// �񓯊���M�J�n
	retc = recv_mail_ovl( pRecvBuf.ToPointer(), &ovlRecv);
	if( !retc ) {
		_ASSERTE(GetLastError() == ERROR_IO_PENDING);
	}

	while (true) {
		DWORD dwWait = WaitForMultipleObjects(EV_END, evArray, FALSE, INFINITE);
		
		if( EV_STOP == dwWait ) {								// �I��
			Marshal::FreeHGlobal(pRecvBuf);
			delete que;		// �o�b�t�@�g���I���������J��
			return;

		} else if ( EV_OVL_RECV == dwWait ) {					// �񓯊���M����
			
			// ���� gcnew���Ă���̂̓���
			// �Ƃ肠������{�̌^�����̌`�Ƃ��� (�g�����ŏ���Ɍ^�ϊ����Ă�)
			//COMMON_QUE_L^ que = gcnew COMMON_QUE_L();
			que = (COMMON_QUE_L^)Marshal::PtrToStructure( pRecvBuf, que->GetType() );

			// �C�x���g���s (�u���b�L���O)
			p->evRecvMail(que, que->nEventNo);
			//delete que;		// �o�b�t�@�g���I���������J��

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




/* 
//------------------------------------------
// PC���̎擾
// �߂�l ���A���
//------------------------------------------
bool KizuMod::GetPcName()
{
	return getpcname();
}
*/

//------------------------------------------
// �V�X�e�����O�o��
// long nSysNo ���b�Z�[�WNo
//  LPCSTR __cBuffer �ψ���
// �߂�l ���A���
//------------------------------------------
int KizuMod::SysLog(int nSysNo, String^ cMsg)
{
	IntPtr p = Marshal::StringToHGlobalAnsi(cMsg);			// ansic�ɕϊ���̕����|�C���^
	int retc = syslog(nSysNo, (LPCSTR)p.ToPointer());
	Marshal::FreeHGlobal(p);
	return retc;
}

//------------------------------------------
// DB_SYSTEM_STATE�ɋ@���Ԃ��Z�b�g����
// int id �@��ID
// int state �@���� (0:����  1�`:�ُ�)
// �߂�l ���A���
//------------------------------------------
void KizuMod::SetStatus(int id, int state)
{
	KizuFunction::SetStatus(id, state, true); 
	return;
}
void KizuMod::SetStatus(int id, bool state)
{
	KizuFunction::SetStatus(id, state, true); 
	return;
}
// �K������̎w�肪C++/CLI�ł͏o���Ȃ�
void KizuMod::SetStatus(int id, int state, bool bLogDsp)
{
	KizuFunction::SetStatus(id, state, bLogDsp); 
	return;
}
void KizuMod::SetStatus(int id, bool state, bool bLogDsp)
{
	KizuFunction::SetStatus(id, state, bLogDsp); 
	return;
}

//------------------------------------------
// ���L����������
// String^ cTblName �����e�[�u����
// int size �����T�C�Y
// �߂�l ���A���
//------------------------------------------
bool KizuMod::ksMemCreate(String^ cTblName, int size)
{
	HANDLE hMapFile;

	IntPtr pname = Marshal::StringToHGlobalAnsi(cTblName);						// ansic�ɕϊ���̕����|�C���^
	hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, (LPCSTR)pname.ToPointer());
	Marshal::FreeHGlobal(pname);

	if(NULL == hMapFile) return false;
	return true;
}

//------------------------------------------
// ���L�������J��
// String^ cTblName �����e�[�u����
// �߂�l ���A���
//------------------------------------------
void KizuMod::ksMemClose(String^ cTblName)
{
	HANDLE hMapFile;

	IntPtr pname = Marshal::StringToHGlobalAnsi(cTblName);						// ansic�ɕϊ���̕����|�C���^
	hMapFile =OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, (LPCSTR)pname.ToPointer());
	Marshal::FreeHGlobal(pname);

	if(NULL != hMapFile) CloseHandle(hMapFile);
	return;
}

//------------------------------------------
// ���L��������̃f�[�^�擾
// String^ cTblName �ڑ�����e�[�u����
// Type^ structureType �\���̂̌^ GetType(COMMON_QUE) �Ƃ��ł�OK
// �߂�l �Ǎ��\����
//------------------------------------------
ValueType^ KizuMod::ksMemRead(String^ cTblName, Type^ structureType)
{
	HANDLE hMapFile;
	IntPtr iMap;
	IntPtr pname = Marshal::StringToHGlobalAnsi(cTblName);						// ansic�ɕϊ���̕����|�C���^
	mem_connect( &hMapFile, (LPVOID*)&iMap, (LPCSTR)pname.ToPointer() );		// ���L�������ɃA�N�Z�X
	
	try {
		if( iMap != IntPtr::Zero) {
			return (ValueType^)Marshal::PtrToStructure(iMap, structureType);	// �f�[�^���}�[�l�W�h�ɃR�s�[
		} else {
			//_ASSERTE(false);
			return nullptr;
		}
	} catch( char * ) {
		return nullptr;
	} finally {
		Marshal::FreeHGlobal(pname);
		mem_close( &hMapFile, (LPVOID*)&iMap );									// ���L�������J��
	}
}

//------------------------------------------
// ���L��������̃f�[�^�擾
// String^ cTblName �ڑ�����e�[�u����
// int startIndex   �R�s�[�J�n�A�h���X
// Type^ structureType �\���̂̌^ GetType(COMMON_QUE) �Ƃ��ł�OK
// int^ nRetc ���A��� (1:���� 0:�ُ�)
// �߂�l �Ǎ��\����
//------------------------------------------
ValueType^ KizuMod::ksMemRead(String^ cTblName, int startIndex, Type^ structureType, int^% nRetc)
{
	HANDLE hMapFile;
	IntPtr iMap;
	IntPtr pname = Marshal::StringToHGlobalAnsi(cTblName);						// ansic�ɕϊ���̕����|�C���^
	mem_connect( &hMapFile, (LPVOID*)&iMap, (LPCSTR)pname.ToPointer() );		// ���L�������ɃA�N�Z�X
	
	nRetc = 1;
	try {
		if( iMap != IntPtr::Zero) {
			return (ValueType^)Marshal::PtrToStructure((iMap + startIndex), structureType);	// �f�[�^���}�[�l�W�h�ɃR�s�[
		} else {
			//_ASSERTE(false);
			nRetc = 0;
			return nullptr;
		}
	} catch( char * ) {
		nRetc = 0;
		return nullptr;
	} finally {
		Marshal::FreeHGlobal(pname);
		mem_close( &hMapFile, (LPVOID*)&iMap );									// ���L�������J��
	}
}

//------------------------------------------
// ���L��������̃f�[�^�擾
// String^ cTblName �ڑ�����e�[�u����
// int startIndex   �R�s�[�J�n�A�h���X
// ValueType^% p	�擾����\����
// �߂�l ���A���
//------------------------------------------
bool KizuMod::ksMemRead(String^ cTblName, int startIndex, ValueType^% p)
{
	HANDLE hMapFile;
	IntPtr iMap;
	IntPtr pname = Marshal::StringToHGlobalAnsi(cTblName);						// ansic�ɕϊ���̕����|�C���^
	mem_connect( &hMapFile, (LPVOID*)&iMap, (LPCSTR)pname.ToPointer() );		// ���L�������ɃA�N�Z�X
	
	try {
		if( iMap != IntPtr::Zero) {
			p = (ValueType^)Marshal::PtrToStructure((iMap + startIndex), p->GetType());	// �f�[�^���}�[�l�W�h�ɃR�s�[
			return true;
		} else {
			//_ASSERTE(false);
			return false;
		}
	} catch( char * ) {
		return false;
	} finally {
		Marshal::FreeHGlobal(pname);
		mem_close( &hMapFile, (LPVOID*)&iMap );									// ���L�������J��
	}
}

//------------------------------------------
// ���L��������̃f�[�^�擾
// String^ cTblName �ڑ�����e�[�u����
// int size ���L�������T�C�Y
// array<unsigned char>^ byteVal �Ǎ��o�C�g�z��
// �߂�l ���A���
//------------------------------------------
bool KizuMod::ksMemRead(String^ cTblName, int size, array<unsigned char>^ byteVal)
{
	HANDLE hMapFile;
	IntPtr iMap;
	IntPtr pname = Marshal::StringToHGlobalAnsi(cTblName);						// ansic�ɕϊ���̕����|�C���^
	mem_connect( &hMapFile, (LPVOID*)&iMap, (LPCSTR)pname.ToPointer() );		// ���L�������ɃA�N�Z�X
	
	try {
		if( iMap != IntPtr::Zero) {
			Marshal::Copy(iMap, byteVal, 0, size );	// �f�[�^���}�[�l�W�h�ɃR�s�[
			return true;
		} else {
			//_ASSERTE(false);
			return false;
		}
	} catch( char * ) {
		return false;
	} finally {
		Marshal::FreeHGlobal(pname);
		mem_close( &hMapFile, (LPVOID*)&iMap );									// ���L�������J��
	}
}

//------------------------------------------
// ���L��������̃f�[�^�擾
// String^ cTblName �ڑ�����e�[�u����
// int startIndex   �R�s�[�J�n�A�h���X
// int size �@�@�@�@���L�������̃T�C�Y
// array<unsigned char>^ byteVal �Ǎ��o�C�g�z��
// �߂�l ���A���
//------------------------------------------
bool KizuMod::ksMemRead(String^ cTblName, int startIndex, int size, array<unsigned char>^ byteVal)
{
	
	HANDLE hMapFile;
	IntPtr iMap;
	IntPtr pname = Marshal::StringToHGlobalAnsi(cTblName);						// ansic�ɕϊ���̕����|�C���^
	mem_connect( &hMapFile, (LPVOID*)&iMap, (LPCSTR)pname.ToPointer() );		// ���L�������ɃA�N�Z�X
	
	try {
		if( iMap != IntPtr::Zero) {
			Marshal::Copy((iMap + startIndex), byteVal, 0, size );	// �f�[�^���}�[�l�W�h�ɃR�s�[
			return true;
		} else {
			//_ASSERTE(false);
			return false;
		}
	} catch( char * ) {
		return false;
	} finally {
		Marshal::FreeHGlobal(pname);
		mem_close( &hMapFile, (LPVOID*)&iMap );									// ���L�������J��
	}
}

//------------------------------------------
// ���L��������̃f�[�^�擾
// String^ cTblName �ڑ�����e�[�u����
// int startIndex   �R�s�[�J�n�A�h���X
// int size �@�@�@�@���L�������̃T�C�Y
// Type^ structureType �\���̂̌^ GetType(COMMON_QUE) �Ƃ��ł�OK
// int^ nRetc ���A��� (1:���� 0:�ُ�)
// �߂�l �Ǎ��\����
//------------------------------------------
ValueType^ KizuMod::ksMemReadMap(String^ cTblName, int startIndex, int size, Type^ structureType, int^% nRetc)
{
	HANDLE hMapFile;
	IntPtr iMap;											// ��
	IntPtr iAnsMap;											// �ŏI�I�Ɏg�������ʒu
	int nAli = 0;											// �y�[�W�T�C�Y�ɃA���C�����g�����I�t�Z�b�g�� 
	IntPtr pname = Marshal::StringToHGlobalAnsi(cTblName);						// ansic�ɕϊ���̕����|�C���^
	mem_connect( &hMapFile, (LPVOID*)&iMap, (LPCSTR)pname.ToPointer(), startIndex, size, &nAli );		// ���L�������ɃA�N�Z�X
	
	nRetc = 1;
	try {
		if( iMap != IntPtr::Zero ) {
			//iAnsMap = iMap + nAli;
			iAnsMap = IntPtr( iMap.ToInt64() + nAli);
			

			return (ValueType^)Marshal::PtrToStructure(iAnsMap, structureType);	// �f�[�^���}�[�l�W�h�ɃR�s�[
		} else {
			//_ASSERTE(false);
			nRetc = 0;
			return nullptr;
		}
	} catch( char * ) {
		nRetc = 0;
		return nullptr;
	} finally {
		Marshal::FreeHGlobal(pname);
		mem_close( &hMapFile, (LPVOID*)&iMap );									// ���L�������J��
	}
}

//------------------------------------------
// ���L��������̃f�[�^�擾
// String^ cTblName �ڑ�����e�[�u����
// int startIndex   �R�s�[�J�n�A�h���X
// int size �@�@�@�@���L�������̃T�C�Y
// ValueType^% p	�擾����\����
// �߂�l ���A���
//------------------------------------------
bool KizuMod::ksMemReadMap(String^ cTblName, int startIndex, int size, ValueType^% p)
{
	HANDLE hMapFile;
	IntPtr iMap;											// ��
	IntPtr iAnsMap;											// �ŏI�I�Ɏg�������ʒu
	int nAli = 0;											// �y�[�W�T�C�Y�ɃA���C�����g�����I�t�Z�b�g�� 
	IntPtr pname = Marshal::StringToHGlobalAnsi(cTblName);						// ansic�ɕϊ���̕����|�C���^
	mem_connect( &hMapFile, (LPVOID*)&iMap, (LPCSTR)pname.ToPointer(), startIndex, size, &nAli );		// ���L�������ɃA�N�Z�X
	
	try {
		if( iMap != IntPtr::Zero ) {
			//iAnsMap = iMap + nAli;
			iAnsMap = IntPtr( iMap.ToInt64() + nAli);
			p = (ValueType^)Marshal::PtrToStructure(iAnsMap, p->GetType());	// �f�[�^���}�[�l�W�h�ɃR�s�[
			return true;
		} else {
			//_ASSERTE(false);
			return false;
		}
	} catch( char * ) {
		return false;
	} finally {
		Marshal::FreeHGlobal(pname);
		mem_close( &hMapFile, (LPVOID*)&iMap );									// ���L�������J��
	}
}

/*
//------------------------------------------
// ���L��������̃f�[�^�擾
// String^ cTblName �ڑ�����e�[�u����
// int startIndex   �R�s�[�J�n�A�h���X
// int size �@�@�@�@���L�������̃T�C�Y
// IntPtr p			�Ǎ��݃|�C���^		(GCHandle.Alloc  ������ɁA GCHandle.AddrOfPinnedObject �� �A�h���X���Œ肵�Ă�������)
// �߂�l ���A���	
//------------------------------------------
bool KizuMod::ksMemRead(String^ cTblName, int startIndex, int size, IntPtr p)
{
	
	HANDLE hMapFile;
	int iMap;
	IntPtr pname = Marshal::StringToHGlobalAnsi(cTblName);						// ansic�ɕϊ���̕����|�C���^
	mem_connect( &hMapFile, (LPVOID*)&iMap, (LPCSTR)pname.ToPointer() );		// ���L�������ɃA�N�Z�X
	
	try {
		if( iMap != 0 ) {
			memcpy( p.ToPointer(), (LPVOID*)(iMap + startIndex), size);
			//Marshal::Copy((IntPtr)(iMap + startIndex), byteVal, 0, size );	// �f�[�^���}�[�l�W�h�ɃR�s�[
			return true;
		} else {
			//_ASSERTE(false);
			return false;
		}
	} catch( char * ) {
		return false;
	} finally {
		Marshal::FreeHGlobal(pname);
		mem_close( &hMapFile, (LPVOID*)&iMap );									// ���L�������J��
	}
}
*/

//------------------------------------------
// ���L��������̃f�[�^������
// String^ cTblName �ڑ�����e�[�u����
// ValueType^ tbl �����݃f�[�^
// �߂�l ���A���
//------------------------------------------
bool KizuMod::ksMemWrite(String^ cTblName, ValueType^ tbl)
{
	HANDLE hMapFile;
	IntPtr iMap;
	IntPtr pname = Marshal::StringToHGlobalAnsi(cTblName);						// ansic�ɕϊ���̕����|�C���^
	mem_connect( &hMapFile, (LPVOID*)&iMap, (LPCSTR)pname.ToPointer() );		// ���L�������ɃA�N�Z�X
	
	try {
		if( iMap != IntPtr::Zero) {
			Marshal::StructureToPtr(tbl, iMap, true);					// ��������Ƀf�[�^���Z�b�g (�p�͋��L�������ɏ�����)
			return true;
		} else {
			return false;
		}
	} catch( char * ) {
		return false;
	} finally {
		Marshal::FreeHGlobal(pname);
		mem_close( &hMapFile, (LPVOID*)&iMap );									// ���L�������J��
	}
}

//------------------------------------------
// ���L��������̃f�[�^������
// String^ cTblName �ڑ�����e�[�u����
// int startIndex   �R�s�[�J�n�A�h���X
// int size �@�@�@�@���L�������̃T�C�Y
// ValueType^ tbl �����݃f�[�^
// �߂�l ���A���
//------------------------------------------
bool KizuMod::ksMemWriteMap(String^ cTblName, int startIndex, int size, ValueType^ tbl)
{
	HANDLE hMapFile;
	IntPtr iMap;											// ��
	IntPtr iAnsMap;											// �ŏI�I�Ɏg�������ʒu
	int nAli = 0;											// �y�[�W�T�C�Y�ɃA���C�����g�����I�t�Z�b�g�� 
	IntPtr pname = Marshal::StringToHGlobalAnsi(cTblName);						// ansic�ɕϊ���̕����|�C���^
	mem_connect( &hMapFile, (LPVOID*)&iMap, (LPCSTR)pname.ToPointer(), startIndex, size, &nAli );		// ���L�������ɃA�N�Z�X
	
	try {
		if( iMap != IntPtr::Zero ) {
			iAnsMap = IntPtr( iMap.ToInt64() + nAli);
			Marshal::StructureToPtr(tbl, iAnsMap, true);					// ��������Ƀf�[�^���Z�b�g (�p�͋��L�������ɏ�����)
			return true;
		} else {
			return false;
		}
	} catch( char * ) {
		return false;
	} finally {
		Marshal::FreeHGlobal(pname);
		mem_close( &hMapFile, (LPVOID*)&iMap );									// ���L�������J��
	}
}