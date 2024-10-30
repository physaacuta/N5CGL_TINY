// *********************************************************************************
//	PC�E�^�X�N����N���X
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//
// *********************************************************************************

#pragma once

#include <windows.h>
#include <shellapi.h>
#include "..\..\..\include\KizuCommon.h"									// �r�����ʃw�b�_�[
#include "KizuFunction.h"													// �T�u���[�`���W

namespace KizuLib
{
	class ProcessManager
	{
	public :
		enum { P_POWEROFF=1, P_REBOOT, P_LOGOFF, P_NOTHING };

	private:
		#define KIZULIB_PROCESSMANAGER_SESSION				"EXECUTE"			// ini�t�@�C���Z�b�V����
		 //const char * KIZULIB_PROCESSMANAGER_SESSION = "EXECUTE";
	
	public:
	//	ProcessManager(void);
	//	virtual ~ProcessManager(void);

		static void AllProcessSend(COMMON_QUE* que);							// �풓�v���Z�X�S���Ƀ��[���𑗐M
		static void ExecProcess();												// �풓�v���Z�X�N���֐�
		static void ExitProcess();												// �풓�v���Z�X�I���֐�
		static void WinExec(char* strInit);										// ���v���Z�X�쐬����

		static int ExitWin(int status);											// Windows�I������
		static BOOL ksAdToken(LPCSTR lpName);									// �w�肵���A�N�Z�X�g�[�N�����̓�����L���ɂ���
		static void ProcessExec(char const* cCmd, int flg=SW_HIDE);				// �R�}���h�����s����
		static int  ProcessExecWait(char const* cCmd, int flg=SW_HIDE);			// �R�}���h�����s����A�ŏI���܂ő҂�
		static int  ProcessExecWaitAns(char const* cCmd, char* cAns, int flg=SW_HIDE);		// �R�}���h�����s����A ���ʂ𒊏o�A�ŏI���܂ő҂�
		static void ShellExec(char const* cCmd, char const* cPrm, int flg=SW_HIDE, char const* cExe="open"); // �t�@�C�������w�肵�Ď��s �Ɠ��ꓮ��

	};
};