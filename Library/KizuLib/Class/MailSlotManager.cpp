// MailSlotManager.cpp: MailSlotManager �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MailSlotManager.h"

#include <stdio.h>

using namespace KizuLib;


//------------------------------------------
//// static �ϐ�
//CRITICAL_SECTION	MailSlotManager::m_csLock;	


//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
MailSlotManager::MailSlotManager() :
m_hSlot(INVALID_HANDLE_VALUE),
m_nLastError(0)
{
//	InitializeCriticalSection(&m_csLock);								// �N���e�B�J���Z�N�V�����I�u�W�F�N�g��������

}
//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
MailSlotManager::~MailSlotManager()
{
	// ���[���X���b�g�j��
	if(INVALID_HANDLE_VALUE != m_hSlot)	CloseHandle(m_hSlot);

//	DeleteCriticalSection(&m_csLock);									// �N���e�B�J���Z�N�V�����I�u�W�F�N�g���J��
}

// ///////////////////////////////////////////////////////////////////
// syslog���X��New���ĂȂ����Ɏg�p����悤
//
//------------------------------------------
// ���M
// LPCSTR SendSlotName ���M���[���X���b�g����
// LPCSTR SendPcName ���MPC����
// LPCVOID pDate ���M���
// DWORD len ���M�o�C�g�� (MAX400byte)
// �߂�l ���A���
//------------------------------------------
int MailSlotManager::SendMail(LPCSTR SendSlotName, LPCSTR SendPcName, LPCVOID pDate, DWORD len)
{
	// �ǂ�����Ă�VB�ŃG���[�ɂȂ�B
//	AutoLock autolock(&m_csLock);		// AutoLock���g������Adll�T�C�Y���������Ȃ��āAVB�œǂ񂾂�̈�j�󂪂ł�悤�ɂȂ����B�Ȃ��H�����S���s��
//	EnterCriticalSection(&m_csLock);	// �߂�ǂ��������ǂ�����


	HANDLE hSend = NULL;
	// ���[���X���b�g�ɐڑ�
	int api_err = Connect(SendSlotName, SendPcName, &hSend);
	if( 0 != api_err ) {
//		LeaveCriticalSection(&m_csLock);
		return api_err;
	}

	// ���[�����M
	DWORD nBytesOccupied = 0;
	BOOL bRet = WriteFile(hSend, pDate, len, &nBytesOccupied, NULL);

//	LeaveCriticalSection(&m_csLock);	// �Ȃ����A�e���̓����ꏊ�ŁA�T�C�Y���ς��B�����킯���킩��Ȃ�


	if (!bRet) {
		Close(&hSend);
		return GetLastError();
	}
	// �ؒf
	Close(&hSend);
	return 0;
}

//------------------------------------------
// ���M�X���b�g�ڑ�
// LPCSTR SendSlotName ���M���[���X���b�g����
// LPCSTR SendPcName ���MPC����
// HANDLE* hSend ���[���X���b�g�n���h��
// �߂�l ���A���
//------------------------------------------
int MailSlotManager::Connect(LPCSTR SendSlotName, LPCSTR SendPcName, HANDLE* hSend)
{
	//CString fullname;
	//fullname.Format("\\\\%s\\mailslot\\%s", SendPcName, SendSlotName);
	char fullname[MAX_PATH];
	sprintf(fullname, "\\\\%s\\mailslot\\%s", SendPcName, SendSlotName);
	// ���[���X���b�g�ɐڑ�
	*hSend = CreateFile(fullname, 
						GENERIC_WRITE, 
						FILE_SHARE_WRITE | FILE_SHARE_READ,
						NULL, 
						OPEN_EXISTING, 
						FILE_ATTRIBUTE_NORMAL,	
						NULL);
	return (*hSend == INVALID_HANDLE_VALUE) ? GetLastError() : 0;
}

//------------------------------------------
// �X���b�g�ؒf
// HANDLE* hSend ���[���X���b�g�n���h��
//------------------------------------------
void MailSlotManager::Close(HANDLE* hSend)
{
	if( *hSend != INVALID_HANDLE_VALUE ) CloseHandle(*hSend);
}

//------------------------------------------
// �X���b�g�\�z
// LPCSTR SlotName ���[���X���b�g����
// DWORD lReadTimeout �^�C���A�E�g [ms]
// �߂�l ���A���
//------------------------------------------
bool MailSlotManager::Create(LPCSTR SlotName, DWORD lReadTimeout)
{
	//CString fullname;
	//fullname.Format("\\\\.\\mailslot\\%s", SlotName);
	char fullname[MAX_PATH];
	sprintf(fullname, "\\\\.\\mailslot\\%s", SlotName);
	// ���[���X���b�g�쐬
	m_hSlot = CreateMailslot(fullname, 
		0,							// ���b�Z�[�W�T�C�Y
		lReadTimeout,				// �^�C���A�E�g [ms]
		NULL);

	// ���[���X���b�g�쐬���s
	return (INVALID_HANDLE_VALUE == m_hSlot) ? false : true; 
}

// ///////////////////////////////////////////////////////////////////
// ICommunity�̎���
//
// 20170321 ҰٽۯĻ��ނ��قȂ�����ԒʐM�̑Ή� --->
//------------------------------------------
// ��M
// LPVOID pDate ��M���
// DWORD len ��M�o�C�g�� (MAX400byte)
// �߂�l ���A���
//------------------------------------------
int MailSlotManager::Recv(LPVOID pDate, DWORD len)
{
	char cWk[1024] = "";
	DWORD nBytesOccupied = 0;
	// ���[����M�i����1024 Byte�œǂݍ��݁j
	BOOL bRet = ReadFile(m_hSlot, cWk, sizeof(cWk), &nBytesOccupied, NULL);
	int nRetCode = ( !bRet ) ? GetLastError() : 0;

	memcpy( pDate, cWk, min( len, nBytesOccupied) );
	if ( len < nBytesOccupied ) {
		return ERROR_MORE_DATA;
	}

	return nRetCode;
}
// 20170321 ҰٽۯĻ��ނ��قȂ�����ԒʐM�̑Ή� <---

//------------------------------------------
// ���M
// LPCVOID pDate ���M���
// DWORD len ���M�o�C�g�� (MAX400byte)
// �߂�l ���A���
//------------------------------------------
int MailSlotManager::Send(LPCVOID pDate, DWORD len)
{
//	AutoLock autolock(&m_csLock);
//	EnterCriticalSection(&m_csLock);

	// ���[�����M
	DWORD nBytesOccupied = 0;
	BOOL bRet = WriteFile(m_hSlot, pDate, len, &nBytesOccupied, NULL);

//	LeaveCriticalSection(&m_csLock);

	return (!bRet) ? GetLastError() : 0;
}


// ///////////////////////////////////////////////////////////////////
// ICommunityAsync�̎���
//
//------------------------------------------
// ��M
// LPVOID pDate ��M���
// DWORD len ��M�o�C�g�� (MAX400byte)
// OVERLAPPED *ovl �񓯊�����̏��
// �߂�l ���A���
//------------------------------------------
bool MailSlotManager::Recv(LPVOID pDate, DWORD len, OVERLAPPED *ovl)
{
	BOOL bRet = ReadFile(m_hSlot, pDate, len, NULL, ovl);
	m_nLastError = GetLastError();	
	if( !bRet ) {		// �����s�̏ꍇ
		if (m_nLastError != ERROR_IO_PENDING) return false;
	}
	return true;
}

//------------------------------------------
// ���M
// LPCVOID pDate ���M���
// DWORD len ���M�o�C�g�� (MAX400byte)
// OVERLAPPED *ovl �񓯊�����̏��
// �߂�l ���A���
//------------------------------------------
bool MailSlotManager::Send(LPCVOID pDate, DWORD len, OVERLAPPED *ovl)
{
//	AutoLock autolock(&m_csLock);
//	EnterCriticalSection(&m_csLock);


	// ���[�����M
	BOOL bRet = WriteFile(m_hSlot, pDate, len, NULL, ovl);
	m_nLastError = GetLastError();	

//	LeaveCriticalSection(&m_csLock);
	if( !bRet ) {		// �����s�̏ꍇ
		if (m_nLastError != ERROR_IO_PENDING) return false;
	}
	return true;
}

//------------------------------------------
// �񓯊����ʖ₢���킹
// OVERLAPPED *ovl �񓯊�����̏��
// �߂�l ���A���
//------------------------------------------
bool MailSlotManager::GetResult(OVERLAPPED *ovl)
{
	DWORD dwTransByte;
	BOOL bRet = GetOverlappedResult(m_hSlot, ovl, &dwTransByte, TRUE);
	if( !bRet ) {
		m_nLastError = GetLastError();
		return false;
	}
	return true;
}
