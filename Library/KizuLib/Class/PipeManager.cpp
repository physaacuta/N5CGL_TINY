// PipeManager.cpp: PipeManager �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PipeManager.h"

using namespace KizuLib;


//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// �R���X�g���N�^
// LPCSTR PipeName �p�C�v����
// bool bIsServer �T�[�o�[��:true  �f�t�H���g�́A�N���C�A���g:false
//------------------------------------------
PipeManager::PipeManager(LPCSTR PipeName, bool bIsServer) :
m_hPipe(INVALID_HANDLE_VALUE),
m_bIsServer(bIsServer),
m_nLastError(0)
{
	if(m_bIsServer) {
		//// �p�C�v�̏��� (�ڑ��҂��ɂ͂Ȃ炢�̂Œ��ӁB)
		// �ڑ��͈�l����
		// �o�́A���̓o�b�t�@�́A256�ɂ��Ă��邪�A�֐����ł͎Q�l���x�ɂ����g���ĂȂ��炵��
		m_hPipe = CreateNamedPipe(	PipeName, //NTCNTRL,
									PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
									PIPE_WAIT | PIPE_READMODE_MESSAGE | PIPE_TYPE_MESSAGE,
									1,	256, 256, 0, NULL);
		//ASSERT(INVALID_HANDLE_VALUE != m_hPipe);
		// �p�C�v�I�[�v�����s
		if (INVALID_HANDLE_VALUE == m_hPipe) throw 2; 

	} else {
		// �p�C�v�ڑ� (�T�[�o�[�ɐڑ���)
	//	m_hPipe = CreateFile((LPCTSTR)PipeName,
		m_hPipe = CreateFile(PipeName,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
			NULL);

		// �p�C�v�I�[�v�����s
		if (INVALID_HANDLE_VALUE == m_hPipe) throw 1; 
		
		// �p�C�v�̐ݒ�
		// �����b�Z�[�W���[�h�ɐݒ�A�u���b�L���O���[�h�ɐݒ�
		DWORD mode = PIPE_READMODE_MESSAGE | PIPE_WAIT;
		SetNamedPipeHandleState(m_hPipe, &mode, NULL, NULL);
	}
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
PipeManager::~PipeManager()
{
	// �p�C�v�N���[�Y
	if(m_bIsServer) {
		DisconnectNamedPipe(m_hPipe);
	}
	CloseHandle(m_hPipe);	
	m_hPipe = INVALID_HANDLE_VALUE;
}

//------------------------------------------
// ��������I/O��������
//------------------------------------------
void PipeManager::Cancel() const
{
	CancelIo(m_hPipe);
}

//------------------------------------------
// �N���C�A���g����̐ڑ����󂯕t����B(��u���b�N)
//------------------------------------------
DWORD PipeManager::Listen(OVERLAPPED *ovl) 
{
	// �ŏ��̐ڑ�
	if ( ! ConnectNamedPipe(m_hPipe, ovl) ) {
		DWORD dwRetCode = GetLastError();
		if (ERROR_IO_PENDING == dwRetCode) return 0;
		//ASSERT(ERROR_IO_PENDING == dwRetCode || ERROR_PIPE_CONNECTED == dwRetCode);
		if (ERROR_PIPE_CONNECTED == dwRetCode) {
			// �ڑ��ς�(�ʏ킠�肦�Ȃ�)
			return dwRetCode;		// �Ăяo�����Ŋm�F����悤�ɂ���
		}
		return dwRetCode;
	}
	return 0;
}

// ///////////////////////////////////////////////////////////////////
// ICommunity�̎���
//
//------------------------------------------
// ��M
// LPVOID pDate ��M���
// DWORD len ��M�o�C�g��
// �߂�l ���A���
//------------------------------------------
int PipeManager::Recv(LPVOID pDate, DWORD len)
{
	DWORD nBytesOccupied = 0;
	BOOL bRet = ReadFile(m_hPipe, pDate, len, &nBytesOccupied, NULL);
	return (!bRet) ? GetLastError() : 0;
}

//------------------------------------------
// ���M
// LPCVOID pDate ���M���
// DWORD len ���M�o�C�g��
// �߂�l ���A���
//------------------------------------------
int PipeManager::Send(LPCVOID pDate, DWORD len)
{
	DWORD nBytesOccupied = 0;
	BOOL bRet = WriteFile(m_hPipe, pDate, len, &nBytesOccupied, NULL);
	return (!bRet) ? GetLastError() : 0;
}


// ///////////////////////////////////////////////////////////////////
// ICommunityAsync�̎���
//
//------------------------------------------
// ��M
// LPVOID pDate ��M���
// DWORD len ��M�o�C�g��
// OVERLAPPED *ovl �񓯊�����̏��
// �߂�l ���A���
//------------------------------------------
bool PipeManager::Recv(LPVOID pDate, DWORD len, OVERLAPPED *ovl)
{
	BOOL bRet = ReadFile(m_hPipe, pDate, len, NULL, ovl);
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
bool PipeManager::Send(LPCVOID pDate, DWORD len, OVERLAPPED *ovl)
{
	BOOL bRet = WriteFile(m_hPipe, pDate, len, NULL, ovl);
	m_nLastError = GetLastError();	
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
bool PipeManager::GetResult(OVERLAPPED *ovl)
{
	DWORD dwTransByte;
	BOOL bRet = GetOverlappedResult(m_hPipe, ovl, &dwTransByte, TRUE);
	if( !bRet ) {
		m_nLastError = GetLastError();
		return false;
	}
	return true;
}