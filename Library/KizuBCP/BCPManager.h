// *********************************************************************************
//	BCP�ڑ��ɑΉ������N���X
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//		�g�p����^�X�N�� �wodbc32.lib�x �������N���鎖
//
// --------------------------------------------
// �g�p���@
//		KIZUBCP_BCPMANAGER �̃R�����g���O��
//		ODBC�̐ݒ�
//				�ʏ� �uSQL Server�v��I�����Ă��邪�A�uSQL Native Client�v��I������
//
// ���\�z
//	[�ǉ��C���N���[�h�p�X]
//		.\SqlNativeClient\2005\x86
//	[�ǉ����C�u�����p�X]
//		"C:\Program Files\Microsoft SQL Server\90\SDK\Lib\x86"
//	[�ǉ����C�u�����t�@�C��]
//		sqlncli.lib
// *********************************************************************************

#pragma once

#include <windows.h>
#include <sql.h>
#include <sqlext.h>


#include "..\..\Include\LibSelect.h"										// ���C�u�����ؑփw�b�_�[


#ifdef DBCHAR							// DBCHAR�̒�`�� odbc.h �� sqlncli.h �ł��������Ă���ׁA��U�J������
	#undef DBCHAR
#endif



//====================================
// �o�[�W�����؂�ւ�
//====================================

//// �e�o�[�W�����ɂ��ύX
//#include "C:\Program Files\Microsoft SQL Server\90\SDK\Include\sqlncli.h"
// 2005�p
#ifdef KIZUBCP_SQLSERVER_2005
	#ifdef KIZUBCP_PATH_H
		#include KIZUBCP_PATH_H
	#else
		//#include ".\SqlNativeClient\2005\sqlncli.h"
		#include "C:\\Program Files\\Microsoft SQL Server\\90\\SDK\\Include\\sqlncli.h"
	#endif

  #ifndef LIB_MANUAL
	#ifdef KIZUBCP_PATH_L
		#pragma comment(lib, KIZUBCP_PATH_L)
	#else
		#ifdef _M_X64			// x64 �R���p�C��
			//#pragma comment(lib, ".\\SqlNativeClient\\2005\\x64\\sqlncli.lib")
			//#pragma comment(lib, "$(SolutionDir)\\SqlNativeClient\\2005\\x64\\sqlncli.lib")
			//#pragma comment(lib, "C:\\Program Files\\Microsoft SQL Server\\90\\SDK\\Lib\\x64\\sqlncli.lib")

			#ifdef _DEBUG
				#pragma comment(lib, "C:\\Program Files\\Microsoft SQL Server\\90\\SDK\\Lib\\x64\\sqlncli.lib")
			#else
				#pragma comment(lib, "\"C:\\Program Files\\Microsoft SQL Server\\90\\SDK\\Lib\\x64\\sqlncli.lib\"")
			#endif
		#else					// x86 �R���p�C��
			//#pragma comment(lib, ".\\SqlNativeClient\\2005\\x86\\sqlncli.lib")
			//#pragma comment(lib, "$(SolutionDir)\\SqlNativeClient\\2005\\x86\\sqlncli.lib")	// NG
			//#pragma comment(lib, "C:\\Program Files\\Microsoft SQL Server\\90\\SDK\\Lib\\x86\\sqlncli.lib")	//�X�y�[�X������� Release�̂Ƃ��ɂȂ����G���[�ɂȂ�

			// VS�̃o�O���ۂ��B
			#ifdef _DEBUG
				#pragma comment(lib, "C:\\Program Files\\Microsoft SQL Server\\90\\SDK\\Lib\\x86\\sqlncli.lib")
			#else
				#pragma comment(lib, "\"C:\\Program Files\\Microsoft SQL Server\\90\\SDK\\Lib\\x86\\sqlncli.lib\"")
			#endif
		#endif
	#endif
  #endif
#endif
// 2008�p
#ifdef KIZUBCP_SQLSERVER_2008
	#ifdef KIZUBCP_PATH_H
		#include KIZUBCP_PATH_H
	#else
		//#include ".\SqlNativeClient\2008\sqlncli.h"
		#include "C:\\Program Files\\Microsoft SQL Server\\100\\SDK\\Include\\sqlncli.h"
	#endif

  #ifndef LIB_MANUAL
	#ifdef KIZUBCP_PATH_L
		#pragma comment(lib, KIZUBCP_PATH_L)
	#else
		#ifdef _M_X64			// x64 �R���p�C��
			//#pragma comment(lib, ".\\SqlNativeClient\\2008\\x64\\sqlncli.lib")
			//#pragma comment(lib, "C:\\Program Files\\Microsoft SQL Server\\100\\SDK\\Lib\\x64\\sqlncli10.lib")
			#ifdef _DEBUG
				#pragma comment(lib, "C:\\Program Files\\Microsoft SQL Server\\100\\SDK\\Lib\\x64\\sqlncli10.lib")
			#else
				#pragma comment(lib, "\"C:\\Program Files\\Microsoft SQL Server\\100\\SDK\\Lib\\x64\\sqlncli10.lib\"")
			#endif
		#else					// x86 �R���p�C��
			//#pragma comment(lib, ".\\SqlNativeClient\\2008\\x86\\sqlncli.lib")
			//#pragma comment(lib, "C:\\Program Files\\Microsoft SQL Server\\100\\SDK\\Lib\\x86\\sqlncli10.lib")
			#ifdef _DEBUG
				#pragma comment(lib, "C:\\Program Files\\Microsoft SQL Server\\100\\SDK\\Lib\\x86\\sqlncli10.lib")
			#else
				#pragma comment(lib, "\"C:\\Program Files\\Microsoft SQL Server\\100\\SDK\\Lib\\x86\\sqlncli10.lib\"")
			#endif
		#endif
	#endif
  #endif
#endif

// 2014�p
#ifdef KIZUBCP_SQLSERVER_2014
	#ifdef KIZUBCP_PATH_H
		#include KIZUBCP_PATH_H
	#else
		//#include ".\SqlNativeClient\2014\sqlncli.h"
		#include "C:\\Program Files\\Microsoft SQL Server\\110\\SDK\\Include\\sqlncli.h"
	#endif

  #ifndef LIB_MANUAL
	#ifdef KIZUBCP_PATH_L
		#pragma comment(lib, KIZUBCP_PATH_L)
	#else
		#ifdef _M_X64			// x64 �R���p�C��
			//#pragma comment(lib, ".\\SqlNativeClient\\2014\\x64\\sqlncli.lib")
			//#pragma comment(lib, "C:\\Program Files\\Microsoft SQL Server\\110\\SDK\\Lib\\x64\\sqlncli11.lib")
			#ifdef _DEBUG
				#pragma comment(lib, "C:\\Program Files\\Microsoft SQL Server\\110\\SDK\\Lib\\x64\\sqlncli11.lib")
			#else
				#pragma comment(lib, "\"C:\\Program Files\\Microsoft SQL Server\\110\\SDK\\Lib\\x64\\sqlncli11.lib\"")
			#endif
		#else					// x86 �R���p�C��
			//#pragma comment(lib, ".\\SqlNativeClient\\2014\\x86\\sqlncli.lib")
			//#pragma comment(lib, "C:\\Program Files\\Microsoft SQL Server\\110\\SDK\\Lib\\x86\\sqlncli11.lib")
			#ifdef _DEBUG
				#pragma comment(lib, "C:\\Program Files\\Microsoft SQL Server\\110\\SDK\\Lib\\x86\\sqlncli11.lib")
			#else
				#pragma comment(lib, "\"C:\\Program Files\\Microsoft SQL Server\\110\\SDK\\Lib\\x86\\sqlncli11.lib\"")
			#endif
		#endif
	#endif
  #endif
#endif







// �r�� �C���N���[�h
#include "..\KizuODBC\OdbcManager.h"											// �e�N���X

using namespace KizuODBC;

namespace KizuODBC
{
	class BCPManager : public OdbcManager
	{
	public:
		BCPManager(void);
		~BCPManager(void);

		// �ڑ�
		int Connect(LPCSTR cIniPath);											// �f�t�H���g�Z�b�V�����ł̐ڑ�
		int Connect(LPCSTR cIniPath, LPCSTR cSession);							// �Z�b�V�����w��ł̐ڑ�
		int Connect(char const* dsn, char const* uid, char const* pwd);			// �C�ӎw��ł̐ڑ�

		// BCP�n�ŕK�v
		SQLHDBC GetHDBC() { return m_hdbc; }									// ���ϐ�
		SQLCHAR* GetDSN() { return m_dsn; }										// �f�[�^�x�[�X��
	};
};