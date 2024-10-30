// *********************************************************************************
//	共有メモリ クラス
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//		
// *********************************************************************************

#pragma once

#pragma warning(disable : 4996)							// ***が古い形式として宣言されました。を出さない対策

#include <Windows.h>

//#include "..\..\..\Include\KsCommon.h"		// 疵検共通ヘッダー


namespace KizuLib
{
	class SmemManager  
	{
	private:
		#define KIZULIB_SMEMMANAGER_MAX_NUM				50			// 管理可能共有メモリ数
		#define KIZULIB_SMEMMANAGER_SESSION				"MEM"		// iniファイルセッション

	public:
		SmemManager();
		virtual ~SmemManager();

		void CreateMem(bool clear = true);					// 共有メモリ生成
		void DeleteMem();									// 共有メモリ破棄

		LPVOID GetAddress(int id) const { return m_pMapped[id]; }		// 指定されたマッピングアドレス

	private:
		int		m_nNum;										// 共有メモリを生成した数
		HANDLE	m_hMapFile[KIZULIB_SMEMMANAGER_MAX_NUM];	// ハンドル
		LPVOID	m_pMapped[KIZULIB_SMEMMANAGER_MAX_NUM];		// マッピングアドレス
	};
};

