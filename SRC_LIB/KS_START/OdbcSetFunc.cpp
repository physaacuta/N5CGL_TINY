#include "StdAfx.h"
#include "OdbcSetFunc.h"


//===========================================================================
// DB�̑���֘A
//===========================================================================


//------------------------------------------
// ODBC�����ݒ� (����DNS�̏ꍇ�́A�ύX�ƂȂ�)
// int nDriverID �h���C�o�敪 (0:SQL Server, 1:SQL Native Client, 2:SQL Native Client 10.0, 3:SQL Native Client 11.0)  
// LPCSTR cDNS ����ɕt����閼�O(ODBC�̖��O)
// LPCSTR cDataSource �f�[�^�x�[�X�ڑ��A�h���X
// LPCSTR cDatabase �f�[�^�x�[�X�� (NULL����)
// ���s����^�X�N�� ODBCCP32.lib ��ǉ����鎖
//------------------------------------------
BOOL ODBC_Setting(int nDriverID, LPCSTR cDSN, LPCSTR cDataSource, LPCSTR cDataBase)
{
	// ODBC�N���X�ւ̒ǉ��́A��� ( = KizuLibCLI�ŃG���[����������)

// #include <odbcinst.h>	// ���K�v
// ODBCCP32.lib ���K�v
	// VS2015 ����́Alegacy_stdio_definitions.lib ���K�v�B


//DSN = ����ɕt����閼�O
//DESCRIPTION = ��������
//TRUSTED_CONNECTION=No �����SQL�F�؂ɏo����͂�

// �e���ڂ̊Ԃ�NULL���K�v
// ���X�g��NULL2�ȏ�K�v


	//// �ڑ�������
	int len;
	char wk[256];
	memset(wk, 0x00, sizeof(wk));


	//// �ǉ�����h���C�o���w��
	char deriver[256];
	memset(deriver, 0x00, sizeof(deriver));
	if(0 == nDriverID) {
		strcpy(deriver, "SQL Server");
	} else if(1 == nDriverID) {
		strcpy(deriver, "SQL Native Client");
	} else if(2 == nDriverID) {
		strcpy(deriver, "SQL Server Native Client 10.0");
	} else if(3 == nDriverID) {
		strcpy(deriver, "SQL Server Native Client 11.0");
	} else {
		return false;
	}

	// �폜�p������
	len  = sprintf(wk		, "Trusted_Connection=no");		len += 1;
	len += sprintf(wk + len , "DSN=%s", cDSN);				
	
	//// ODBC�����폜 (�h���C�o���͉��ł��悢�B�ł��󔒂�NG������)
	//  �� �����ɂ́ASQL Native Client ���g�p����ׁA�h���C�o���قȂ�B���ׁ̈A��x�����Ă���A�ēx�쐬����
	SQLConfigDataSource( NULL, ODBC_REMOVE_SYS_DSN, "SQL Server", wk );

	// �ǉ��p������
	len  = sprintf(wk		, "Trusted_Connection=no");		len += 1;
	len += sprintf(wk + len , "DSN=%s", cDSN);				len += 1;
	len += sprintf(wk + len	, "SERVER=%s", cDataSource);	len += 1;
	len += sprintf(wk + len	, "DataBase=%s", cDataBase);

	//// ODBC�����o�^ (���[�U�[�E�p�X�͐ݒ�s�\�B�ǂ����ɂ��Ă��v���O�����Ŏg���Ƃ��Ɏw�肷��וK�v�Ȃ��H)
	return SQLConfigDataSource( NULL, ODBC_ADD_SYS_DSN, deriver, wk );
}