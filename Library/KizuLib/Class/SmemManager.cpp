// SmemManager.cpp: SmemManager クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SmemManager.h"

#include <stdio.h>
#include "..\..\..\Include\KizuCommon.h"					// 疵検共通インクルード

using namespace KizuLib;

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// コンストラクタ
//------------------------------------------
SmemManager::SmemManager() :
m_nNum(0)
{
	memset(m_hMapFile, 0x00, sizeof(m_hMapFile));
	memset(m_pMapped, 0x00, sizeof(m_pMapped));
}
//------------------------------------------
// デストラクタ
//------------------------------------------
SmemManager::~SmemManager()
{

}

//------------------------------------------
// 共有メモリ生成
// bool clear 生成後初期化を行う場合 true
//------------------------------------------
void SmemManager::CreateMem(bool clear)
{
	char cMemName[256];
	DWORD nMemSize;		
	char cKey[64];		// INIキーバッファ

	m_nNum = 0;
	while(1) 
	{
		// 共有メモリ名取得
		sprintf( cKey, "MEM%d_NAME", m_nNum+1);			// Key生成
		GetPrivateProfileString(KIZULIB_SMEMMANAGER_SESSION, cKey, "" , cMemName, sizeof(cKey), TASKINI_NAME);

		// 共有メモリサイズ
		sprintf( cKey, "MEM%d_SIZE", m_nNum+1 );
		nMemSize = GetPrivateProfileInt(KIZULIB_SMEMMANAGER_SESSION, cKey, 0, TASKINI_NAME);

		if( 0 == strlen(cMemName) || 0 == nMemSize ) break;


		// 共有メモリ生成 (CreateFileMappingは 未生成時は、新規作成ハンドル。 生成済みの場合は、そのハンドルが返る)
		m_hMapFile[m_nNum] = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, nMemSize, cMemName);
		m_pMapped[m_nNum] = MapViewOfFile(m_hMapFile[m_nNum], FILE_MAP_ALL_ACCESS, 0, 0, 0);

		// 初期化
		if( clear ) memset(m_pMapped[m_nNum], 0x00, nMemSize);

		m_nNum++;
		if( KIZULIB_SMEMMANAGER_MAX_NUM == m_nNum ) break;			// 上限到達
	}
}

//------------------------------------------
// 共有メモリ破棄
//------------------------------------------
void SmemManager::DeleteMem()
{
	for( int ii=0; ii<m_nNum; ii++ ) {
		UnmapViewOfFile(m_pMapped[ii]);
		CloseHandle(m_hMapFile[ii]);
	}
}