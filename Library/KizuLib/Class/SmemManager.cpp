// SmemManager.cpp: SmemManager �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SmemManager.h"

#include <stdio.h>
#include "..\..\..\Include\KizuCommon.h"					// �r�����ʃC���N���[�h

using namespace KizuLib;

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
SmemManager::SmemManager() :
m_nNum(0)
{
	memset(m_hMapFile, 0x00, sizeof(m_hMapFile));
	memset(m_pMapped, 0x00, sizeof(m_pMapped));
}
//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
SmemManager::~SmemManager()
{

}

//------------------------------------------
// ���L����������
// bool clear �����㏉�������s���ꍇ true
//------------------------------------------
void SmemManager::CreateMem(bool clear)
{
	char cMemName[256];
	DWORD nMemSize;		
	char cKey[64];		// INI�L�[�o�b�t�@

	m_nNum = 0;
	while(1) 
	{
		// ���L���������擾
		sprintf( cKey, "MEM%d_NAME", m_nNum+1);			// Key����
		GetPrivateProfileString(KIZULIB_SMEMMANAGER_SESSION, cKey, "" , cMemName, sizeof(cKey), TASKINI_NAME);

		// ���L�������T�C�Y
		sprintf( cKey, "MEM%d_SIZE", m_nNum+1 );
		nMemSize = GetPrivateProfileInt(KIZULIB_SMEMMANAGER_SESSION, cKey, 0, TASKINI_NAME);

		if( 0 == strlen(cMemName) || 0 == nMemSize ) break;


		// ���L���������� (CreateFileMapping�� ���������́A�V�K�쐬�n���h���B �����ς݂̏ꍇ�́A���̃n���h�����Ԃ�)
		m_hMapFile[m_nNum] = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, nMemSize, cMemName);
		m_pMapped[m_nNum] = MapViewOfFile(m_hMapFile[m_nNum], FILE_MAP_ALL_ACCESS, 0, 0, 0);

		// ������
		if( clear ) memset(m_pMapped[m_nNum], 0x00, nMemSize);

		m_nNum++;
		if( KIZULIB_SMEMMANAGER_MAX_NUM == m_nNum ) break;			// ������B
	}
}

//------------------------------------------
// ���L�������j��
//------------------------------------------
void SmemManager::DeleteMem()
{
	for( int ii=0; ii<m_nNum; ii++ ) {
		UnmapViewOfFile(m_pMapped[ii]);
		CloseHandle(m_hMapFile[ii]);
	}
}