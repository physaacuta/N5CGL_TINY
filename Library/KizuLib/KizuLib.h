// *********************************************************************************
//	C�����C�u�����֐�
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//		MFC�̎g�p�͐�΋֎~�B�g���Ă��郉�C�u�����́AKizuLibMFC��
// *********************************************************************************
#pragma once
//#ifndef KIZLIB_H
//#define KIZULIB_H

#include <windows.h>

#include "..\..\Include\KizuCommon.h"										// �r�����ʃC���N���[�h
#include "Class\KizuPcInfManager.h"											// PC�Ǘ�

// ///////////////////////////////////////////////////////////////////
// ���ʊ֐�
//
int task_init(LPCSTR cTaskName);										// �^�X�N����
int task_init(LPCSTR cTaskName, int nPCID);								// �^�X�N����
int task_init_multi(LPCSTR cTaskName, int nLineID);						// �^�X�N���� (����PC�����^�X�N���s�p)
void task_exit();														// �^�X�N�I��
int	send_mail(LPCSTR SendSlotName, LPCSTR SendPcName, LPCVOID pDate);	// ���[�����M
int	send_mail_trans(LPCSTR SendSlotName, LPCSTR SendPcName, LPCVOID pDate);	// ���[���]�����M
int	send_mail_trans(LPCSTR SendUdpAddr, LPCSTR SendSlotName, LPCSTR SendPcName, LPCVOID pDate);	// ���[���]�����M
int	recv_mail(LPVOID pDate);											// ���[����M
bool recv_mail_ovl(LPVOID pDate, OVERLAPPED* ovlRecv);					// ���[����M (�񓯊�)

bool getpcname();														// �SPC���̂�DB���擾
char const* getmypcname();												// ��PC���̎擾
int syslog(long nSysNo, LPCSTR __cBuffer, ... );						// �V�X�e�����O�o��
int statelog(long nSysNo, long nStateId, long nStateVal, LPCSTR __cBuffer, ... );	// �@���ԕύX���O�o��

void mem_connect(HANDLE *hMapFile, LPVOID *pMapped, const char* name);	// ���L�������A�N�Z�X
void mem_connect(HANDLE *hMapFile, LPVOID *pMapped, const char* name, DWORD offset, SIZE_T s, int* nAddrAli);	// ���L�������A�N�Z�X (�����勤�L�������Q�ƂɑΉ� 20160826)
void mem_close(HANDLE *hMapFile, LPVOID *pMapped);						// ���L�������g�p�I��

// ���܂���
int  getpcid();															// ��PCID (1�I���W��)
void setlineid();														// �����C��ID�Z�b�g
void setlineid(int nLineID);											// �����C��ID�Z�b�g
int  getlineid();														// �����C��ID�擾
char const* getmytaskname();											// ���^�X�N����
LPCVOID getcls_pcinfmgr();												// KizuPcInfManager�N���X�ԋp

//bool ksODBC_Setting(LPCSTR cDSN, LPCSTR cDataSource, LPCSTR cDataBase);	// ODBC�����ݒ�





//#endif