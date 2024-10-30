// *********************************************************************************
//	BCP接続に対応したクラス
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//		使用するタスクで 『odbc32.lib』 をリンクする事
//
// --------------------------------------------
// 使用方法
//		KIZUBCP_BCPMANAGER のコメントを外す
//		ODBCの設定
//				通常 「SQL Server」を選択しているが、「SQL Native Client」を選択する
//
// 環境構築
//	[追加インクルードパス]
//		.\SqlNativeClient\2005\x86
//	[追加ライブラリパス]
//		"C:\Program Files\Microsoft SQL Server\90\SDK\Lib\x86"
//	[追加ライブラリファイル]
//		sqlncli.lib
// *********************************************************************************

#pragma once

#include <windows.h>
#include <sql.h>
#include <sqlext.h>


#include "..\..\Include\LibSelect.h"										// ライブラリ切替ヘッダー


#ifdef DBCHAR							// DBCHARの定義が odbc.h と sqlncli.h でかち合っている為、一旦開放する
	#undef DBCHAR
#endif



//====================================
// バージョン切り替え
//====================================

//// 各バージョンにより変更
//#include "C:\Program Files\Microsoft SQL Server\90\SDK\Include\sqlncli.h"
// 2005用
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
		#ifdef _M_X64			// x64 コンパイル
			//#pragma comment(lib, ".\\SqlNativeClient\\2005\\x64\\sqlncli.lib")
			//#pragma comment(lib, "$(SolutionDir)\\SqlNativeClient\\2005\\x64\\sqlncli.lib")
			//#pragma comment(lib, "C:\\Program Files\\Microsoft SQL Server\\90\\SDK\\Lib\\x64\\sqlncli.lib")

			#ifdef _DEBUG
				#pragma comment(lib, "C:\\Program Files\\Microsoft SQL Server\\90\\SDK\\Lib\\x64\\sqlncli.lib")
			#else
				#pragma comment(lib, "\"C:\\Program Files\\Microsoft SQL Server\\90\\SDK\\Lib\\x64\\sqlncli.lib\"")
			#endif
		#else					// x86 コンパイル
			//#pragma comment(lib, ".\\SqlNativeClient\\2005\\x86\\sqlncli.lib")
			//#pragma comment(lib, "$(SolutionDir)\\SqlNativeClient\\2005\\x86\\sqlncli.lib")	// NG
			//#pragma comment(lib, "C:\\Program Files\\Microsoft SQL Server\\90\\SDK\\Lib\\x86\\sqlncli.lib")	//スペースが入ると Releaseのときになぜかエラーになる

			// VSのバグっぽい。
			#ifdef _DEBUG
				#pragma comment(lib, "C:\\Program Files\\Microsoft SQL Server\\90\\SDK\\Lib\\x86\\sqlncli.lib")
			#else
				#pragma comment(lib, "\"C:\\Program Files\\Microsoft SQL Server\\90\\SDK\\Lib\\x86\\sqlncli.lib\"")
			#endif
		#endif
	#endif
  #endif
#endif
// 2008用
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
		#ifdef _M_X64			// x64 コンパイル
			//#pragma comment(lib, ".\\SqlNativeClient\\2008\\x64\\sqlncli.lib")
			//#pragma comment(lib, "C:\\Program Files\\Microsoft SQL Server\\100\\SDK\\Lib\\x64\\sqlncli10.lib")
			#ifdef _DEBUG
				#pragma comment(lib, "C:\\Program Files\\Microsoft SQL Server\\100\\SDK\\Lib\\x64\\sqlncli10.lib")
			#else
				#pragma comment(lib, "\"C:\\Program Files\\Microsoft SQL Server\\100\\SDK\\Lib\\x64\\sqlncli10.lib\"")
			#endif
		#else					// x86 コンパイル
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

// 2014用
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
		#ifdef _M_X64			// x64 コンパイル
			//#pragma comment(lib, ".\\SqlNativeClient\\2014\\x64\\sqlncli.lib")
			//#pragma comment(lib, "C:\\Program Files\\Microsoft SQL Server\\110\\SDK\\Lib\\x64\\sqlncli11.lib")
			#ifdef _DEBUG
				#pragma comment(lib, "C:\\Program Files\\Microsoft SQL Server\\110\\SDK\\Lib\\x64\\sqlncli11.lib")
			#else
				#pragma comment(lib, "\"C:\\Program Files\\Microsoft SQL Server\\110\\SDK\\Lib\\x64\\sqlncli11.lib\"")
			#endif
		#else					// x86 コンパイル
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







// 疵検 インクルード
#include "..\KizuODBC\OdbcManager.h"											// 親クラス

using namespace KizuODBC;

namespace KizuODBC
{
	class BCPManager : public OdbcManager
	{
	public:
		BCPManager(void);
		~BCPManager(void);

		// 接続
		int Connect(LPCSTR cIniPath);											// デフォルトセッションでの接続
		int Connect(LPCSTR cIniPath, LPCSTR cSession);							// セッション指定での接続
		int Connect(char const* dsn, char const* uid, char const* pwd);			// 任意指定での接続

		// BCP系で必要
		SQLHDBC GetHDBC() { return m_hdbc; }									// 環境変数
		SQLCHAR* GetDSN() { return m_dsn; }										// データベース名
	};
};