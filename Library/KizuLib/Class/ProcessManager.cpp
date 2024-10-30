#include "StdAfx.h"
#include "ProcessManager.h"

#include <stdio.h>
#include <stdlib.h>
#include "..\..\..\Include\KizuCommon.h"					// �r�����ʃC���N���[�h
#include "..\KizuLib.h"										// �r�����C�u�����C���N���[�h

using namespace KizuLib;
/*
//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
ProcessManager::ProcessManager(void)
{
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
ProcessManager::~ProcessManager(void)
{
}
*/

//------------------------------------------
// �풓�v���Z�X�S���Ƀ��[���𑗐M
// COMMON_QUE* que ���M���[�����e
//------------------------------------------
void ProcessManager::AllProcessSend(COMMON_QUE* que)
{
	char cKey[64];
	char cWk[256];
	int nNum = 0;
	while(true) {
		nNum++;
		// �^�X�N���̎擾
		sprintf( cKey, "TASK%d", nNum);			// Key����
		GetPrivateProfileString(KIZULIB_PROCESSMANAGER_SESSION, cKey, "", cWk, sizeof(cWk), TASKINI_NAME);
		
		// �^�X�N�I���ʒm
		if( 0 == strlen(cWk) ) {
			break;
		} else {
			char* taskName;
			char const strDelimit[]   = ",\n";			// �f���~�^�����Q
			//char * next_token;
			//taskName = strtok_s( cWk, strDelimit, &next_token);
			taskName = strtok( cWk, strDelimit);
			send_mail(taskName, ".", que);
		}
	}
}

//------------------------------------------
// �풓�v���Z�X�N���֐�
//------------------------------------------
void ProcessManager::ExecProcess()
{
	char cKey[64];
	char cWk[256];
	int nNum = 0;
	while(true) {
		nNum++;
		// �^�X�N���̎擾
		sprintf( cKey, "TASK%d", nNum);			// Key����
		GetPrivateProfileString(KIZULIB_PROCESSMANAGER_SESSION, cKey, "", cWk, sizeof(cWk), TASKINI_NAME);
		
		// �^�X�N���s
		if( 0 == strlen(cWk) ) break;
		else				   WinExec(cWk);
	}
}

//------------------------------------------
// �풓�v���Z�X�I���֐�
//------------------------------------------
void ProcessManager::ExitProcess()
{
	COMMON_QUE que;
	que.nEventNo = FACT_KS_END;
	que.nLineNo = LINE_ID;

	AllProcessSend(&que);
/*
	char cKey[64];
	char cWk[256];
	int nNum = 0;
	while(true) {
		nNum++;
		// �^�X�N���̎擾
		sprintf( cKey, "TASK%d", nNum);			// Key����
		GetPrivateProfileString(KIZULIB_PROCESSMANAGER_SESSION, cKey, "", cWk, sizeof(cWk), TASKINI_NAME);
		
		// �^�X�N�I���ʒm
		if( 0 == strlen(cWk) ) {
			break;
		} else {
			char* taskName;
			char const strDelimit[]   = ",\n";			// �f���~�^�����Q
			char * next_token;
			taskName = strtok( cWk, strDelimit, &next_token);
			send_mail(taskName, ".", &que);
		}
	}
*/
}

//------------------------------------------
// ���v���Z�X�쐬����
// char* strInit �����^�X�N�p�X (�^�X�N�p�X ����(�����ł�OK), �E�B���h�E�t���O)�̌`��
//------------------------------------------
void ProcessManager::WinExec(char* strInit)
{
	STARTUPINFO	si;
	PROCESS_INFORMATION pi;
	char * taskPath;							// �N���^�X�N�p�X
	char * show_window_flg;						// �E�B���h�E�t���O 
	int	flg;									// �E�B���h�E�t���O 
	char const strDelimit[]   = ",\n";			// �f���~�^�����Q
	char * wkPath;								// ���[�N�p�X


	//// ����
//	taskPath = strtok( strInit, strDelimit, &next_token);
//	show_window_flg = strtok( NULL, strDelimit, &next_token);
	taskPath = strtok( strInit, strDelimit);
	show_window_flg = strtok( NULL, strDelimit);
	flg = atoi(show_window_flg);

	// �z���g�̎��sExe������Ȃ�ύX����
	wkPath = strtok( NULL, strDelimit);
	if( NULL != wkPath) {
		KizuFunction::Trim(wkPath, 0);	// �g����������Ɛ���Ƀ^�X�N���N���ł��Ȃ��΍�
		taskPath = wkPath;
	}


	//// �^�X�N�N������
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;		// �E�C���h�E�쐬�t���O
	if( show_window_flg == NULL ){
		si.wShowWindow = SW_SHOWNORMAL;		//�ʏ�
								
	}else{

		// �E�C���h�E�\���X�^�C��
		switch( flg )
		{
			case 0:			//�ʏ�
				si.wShowWindow = SW_SHOWNORMAL;  										
				break;

			case 1:			//�ŏ���
				si.wShowWindow = SW_SHOWMINIMIZED;  										
				break;

			case 2:			//�ő剻
				si.wShowWindow = SW_SHOWMAXIMIZED;  										
				break;

			case 3:			//��\��
				si.wShowWindow = SW_HIDE;  										
				break;

			default:			//�ʏ�
				si.wShowWindow = SW_SHOWNORMAL;  										
				break;
		}
	}
	si.lpReserved = NULL;		// �\��ς݁ANULL�łȂ���΂Ȃ�Ȃ�
	si.lpDesktop = NULL;		// �f�X�N�g�b�v��
	si.cbReserved2 = 0;			// �\��ς݁A0�łȂ���΂Ȃ�Ȃ�
	si.lpReserved2 = NULL;		// �\��ς݁ANULL�łȂ���΂Ȃ�Ȃ�
	si.lpTitle = NULL;			// �E�C���h�E�X�^�C��

    //// �v���Z�X�N��
	CreateProcess( NULL, taskPath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi );
	CloseHandle(pi.hProcess);	// �v���Z�X�n���h���N���[�Y
	CloseHandle(pi.hThread);	// �v���Z�X�E�X���b�h�E�n���h���N���[�Y

	return;
}


//------------------------------------------
// Windows�I������
// int status P_POWEROFF:�V���b�g�_�E��(1)  P_REBOOT:�ċN��(2)
//------------------------------------------
int ProcessManager::ExitWin(int status)
{
	// �w�肵���A�N�Z�X�g�[�N�����̓�����L���ɂ���
	ksAdToken(SE_SHUTDOWN_NAME);
	ksAdToken(SE_SYSTEMTIME_NAME);

	Sleep(3000);

	// �ċN���A�V���b�g�_�E��
	switch(status) {
	case P_POWEROFF:

		if( !ExitWindowsEx((EWX_POWEROFF | EWX_FORCE), 0) )
			// �V���b�g�_�E�����s
			return -1;
		break;

	case P_REBOOT:
		if( !ExitWindowsEx((EWX_REBOOT | EWX_FORCE), 0) )
			// �ċN�����s
			return -2;
		break;

	case P_LOGOFF:
		if( !ExitWindowsEx((EWX_LOGOFF | EWX_FORCE), 0) )
			// ���O�I�t���s
			return -3;
		break;

	default:
		break;
	}
	return 0;
}

//------------------------------------------
// �w�肵���A�N�Z�X�g�[�N�����̓�����L���ɂ���
// LPCSTR lpName ����
// �߂�l ���A���
//------------------------------------------
BOOL ProcessManager::ksAdToken(LPCSTR lpName)
{
	// �v���Z�X�̃A�N�Z�X�g�[�N���� 
	// TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES ���w�肵�� Open ����B
	HANDLE hToken;			// �n���h��
	TOKEN_PRIVILEGES tp;	// ��������tp�ɂ���

	if(!OpenProcessToken( GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken)) {
		return FALSE;
	}

	// SE_SYSTEM_NAME �� LUID ���擾����B
	LookupPrivilegeValue(NULL, lpName, &tp.Privileges[0].Luid);

	// SE_SYSTEM_NAME ������ SE_PRIVILEGE_ENABLED ������ ENABLE �Ƃ���B
	tp.PrivilegeCount = 1;	//�������[�U�[�͂P�ł���
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tp, 0, NULL, 0);
	return TRUE;
}

//------------------------------------------
// �R�}���h�����s����
// char const* cCmd �R�}���h
// int flg �N����� (0:��\�� 1:�ʏ� 2:�ŏ��� 3:�ő剻)
//------------------------------------------
void ProcessManager::ProcessExec(char const* cCmd, int flg)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	GetStartupInfo(&si);
	si.dwFlags = flg;			// SW_HIDE

	DWORD mode = NORMAL_PRIORITY_CLASS;
	if(0 == flg) mode |= CREATE_NO_WINDOW;

	// �v���Z�X�N��
	CreateProcess(NULL, (LPSTR)cCmd, NULL, NULL, FALSE, mode, NULL, NULL, &si, &pi);
	CloseHandle(pi.hProcess);	// �v���Z�X�n���h���N���[�Y
	CloseHandle(pi.hThread);	// �v���Z�X�E�X���b�h�E�n���h���N���[�Y
}

//------------------------------------------
// �R�}���h�����s����A�ŏI���܂ő҂�
// char const* cCmd �R�}���h
// int flg �N����� (0:��\�� 1:�ʏ� 2:�ŏ��� 3:�ő剻)
//------------------------------------------
int ProcessManager::ProcessExecWait(char const* cCmd, int flg)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	GetStartupInfo(&si);
	si.dwFlags = flg;			// SW_HIDE

	DWORD mode = NORMAL_PRIORITY_CLASS;
	if(0 == flg) mode |= CREATE_NO_WINDOW;

	// �v���Z�X�N��
	if( !CreateProcess(NULL, (LPSTR)cCmd, NULL, NULL, FALSE, mode, NULL, NULL, &si, &pi) ) {
		return -99;
	}

	// �v���Z�X�̏I����҂�
	WaitForSingleObject( pi.hProcess, INFINITE );

	// �I���R�[�h�̎擾
	DWORD  exitcode = 0;
	GetExitCodeProcess( pi.hProcess, &exitcode);

	// ��n��
	CloseHandle(pi.hProcess);	// �v���Z�X�n���h���N���[�Y
	CloseHandle(pi.hThread);	// �v���Z�X�E�X���b�h�E�n���h���N���[�Y
	return (int)exitcode;
}

//------------------------------------------
// �R�}���h�����s����A ���ʂ𒊏o�A�ŏI���܂ő҂�
// char const* cCmd �R�}���h
// int flg �N����� (0:��\�� 1:�ʏ� 2:�ŏ��� 3:�ő剻)
// char* cAns ���ʕ�����
//------------------------------------------
int ProcessManager::ProcessExecWaitAns(char const* cCmd, char* cAns, int flg)
{
	int er = 0;
	// �擾�p
	HANDLE hReadPipe = 0;
	HANDLE hWritePipe = 0;
	SECURITY_ATTRIBUTES saInherit;
	memset(&saInherit, 0x00, sizeof(saInherit));
	saInherit.bInheritHandle = TRUE;

	if( 0 == CreatePipe(&hReadPipe, &hWritePipe, &saInherit, 0) ) return -2;


	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	GetStartupInfo(&si);
	si.dwFlags = flg;			// SW_HIDE
	si.dwFlags = si.dwFlags | STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	si.hStdOutput = hWritePipe;
	//si.hStdError= hWritePipe;			// �G���[���b�Z�[�W���o�͂��Ē�������ꍇ�͗L����

	DWORD mode = NORMAL_PRIORITY_CLASS;
	if(0 == flg) mode |= CREATE_NO_WINDOW;


	// �v���Z�X�N��
	BOOL bRet = CreateProcess(NULL, (LPSTR)cCmd, NULL, NULL, TRUE, mode, NULL, NULL, &si, &pi);
	er = GetLastError();
	CloseHandle(hWritePipe);

	if( ! bRet ) {
		CloseHandle(hReadPipe);
		return -er;
	}


	// ���ʒ��o
	int nLength = 0;
	while (1) {
		DWORD nRead=0;

		if( ::ReadFile(hReadPipe, &cAns[nLength], 2048, &nRead, 0) == 0 ) break;
		nLength += nRead;
	}
	CloseHandle(hReadPipe);


	// �v���Z�X�̏I����҂�
	WaitForSingleObject( pi.hProcess, INFINITE );

	// �I���R�[�h�̎擾
	DWORD  exitcode = 0;
	GetExitCodeProcess( pi.hProcess, &exitcode);

	// ��n��
	CloseHandle(pi.hProcess);	// �v���Z�X�n���h���N���[�Y
	CloseHandle(pi.hThread);	// �v���Z�X�E�X���b�h�E�n���h���N���[�Y
	return (int)exitcode;
}



//------------------------------------------
// �t�@�C�������w�肵�Ď��s �Ɠ��ꓮ��
// char const* cCmd �R�}���h
// char const* cPrm �p�����[�^����
// int flg �N����� (0:��\�� 1:�ʏ� 2:�ŏ��� 3:�ő剻)
// char const* cExe ���s�`��
//------------------------------------------
void ProcessManager::ShellExec(char const* cCmd, char const* cPrm, int flg, char const* cExe)
{
	ShellExecute( NULL, cExe, cCmd, cPrm, NULL, flg);
}

