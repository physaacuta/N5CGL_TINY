#pragma once

// *********************************************************************************
//	ODBC�o�^�p
//	[Ver]
//		Ver.01    2012/03/23  vs2005 �Ή�
//
//	[����]
//		x86,x64�ɑΉ������邽�߁A�t�@���N�V������
// *********************************************************************************

// #include <odbcinst.h>	// ���K�v
// ODBCCP32.lib ���K�v

#include <windows.h>
#include <odbcinst.h>													// ODBC�����ݒ�p



// DB��Ini�t�@�C������֘A
BOOL ODBC_Setting(int nDriverID, LPCSTR cDSN, LPCSTR cDataSource, LPCSTR cDataBase);	// ODBC�����ݒ� (����DNS�̏ꍇ�́A�ύX�ƂȂ�)
