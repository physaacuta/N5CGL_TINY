// *********************************************************************************
//	C風ライブラリ関数
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//		MFCの使用は絶対禁止。使っているライブラリは、KizuLibMFCへ
// *********************************************************************************
#pragma once
//#ifndef KIZLIB_H
//#define KIZULIB_H

#include <windows.h>

#include "..\..\Include\KizuCommon.h"										// 疵検共通インクルード
#include "Class\KizuPcInfManager.h"											// PC管理

// ///////////////////////////////////////////////////////////////////
// 共通関数
//
int task_init(LPCSTR cTaskName);										// タスク生成
int task_init(LPCSTR cTaskName, int nPCID);								// タスク生成
int task_init_multi(LPCSTR cTaskName, int nLineID);						// タスク生成 (同一PC複数タスク実行用)
void task_exit();														// タスク終了
int	send_mail(LPCSTR SendSlotName, LPCSTR SendPcName, LPCVOID pDate);	// メール送信
int	send_mail_trans(LPCSTR SendSlotName, LPCSTR SendPcName, LPCVOID pDate);	// メール転送送信
int	send_mail_trans(LPCSTR SendUdpAddr, LPCSTR SendSlotName, LPCSTR SendPcName, LPCVOID pDate);	// メール転送送信
int	recv_mail(LPVOID pDate);											// メール受信
bool recv_mail_ovl(LPVOID pDate, OVERLAPPED* ovlRecv);					// メール受信 (非同期)

bool getpcname();														// 全PC名称をDBより取得
char const* getmypcname();												// 自PC名称取得
int syslog(long nSysNo, LPCSTR __cBuffer, ... );						// システムログ出力
int statelog(long nSysNo, long nStateId, long nStateVal, LPCSTR __cBuffer, ... );	// 機器状態変更ログ出力

void mem_connect(HANDLE *hMapFile, LPVOID *pMapped, const char* name);	// 共有メモリアクセス
void mem_connect(HANDLE *hMapFile, LPVOID *pMapped, const char* name, DWORD offset, SIZE_T s, int* nAddrAli);	// 共有メモリアクセス (※巨大共有メモリ参照に対応 20160826)
void mem_close(HANDLE *hMapFile, LPVOID *pMapped);						// 共有メモリ使用終了

// こまごま
int  getpcid();															// 自PCID (1オリジン)
void setlineid();														// 自ラインIDセット
void setlineid(int nLineID);											// 自ラインIDセット
int  getlineid();														// 自ラインID取得
char const* getmytaskname();											// 自タスク名称
LPCVOID getcls_pcinfmgr();												// KizuPcInfManagerクラス返却

//bool ksODBC_Setting(LPCSTR cDSN, LPCSTR cDataSource, LPCSTR cDataBase);	// ODBC自動設定





//#endif