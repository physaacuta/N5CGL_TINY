// *********************************************************************************
//	Contec PIOボード管理クラス
// *********************************************************************************
#pragma once

#include "ContecPioBaseManager.h"										// 基底クラス

#ifndef  DEFECTLIB_CONTEC_PIO_NON		////// 完全無効化
// コンテック インクルード
#ifdef DEFECTLIB_CONTEC_PIO_PATH_H
	#include DEFECTLIB_CONTEC_PIO_PATH_H
#else
	#ifndef DEFECTLIB_CONTEC_PIO_WMD		// 従来版
		#include "..\..\include\Other\APIDIO.H"								// PIOボード インクルード
	#else									// WMD版
		#include "..\..\include\Other\CDio.h"								// PIOボード インクルード
	#endif
#endif

// コンテック ライブラリ
#ifndef LIB_MANUAL
#ifdef DEFECTLIB_CONTEC_PIO_PATH_L
	#pragma comment(lib, DEFECTLIB_CONTEC_PIO_PATH_L)
#else
	#ifndef DEFECTLIB_CONTEC_PIO_WMD		// 従来版
		// VSのバグっぽい。
		#ifdef _DEBUG
			#pragma comment(lib, "C:\\Program Files\\CONTEC\\API-PAC(W32)\\Dio\\Samples\\Lib\\APIDIO.lib")
		#else
			#pragma comment(lib, "\"C:\\Program Files\\CONTEC\\API-PAC(W32)\\Dio\\Samples\\Lib\\APIDIO.lib\"")
		#endif

	#else									// WMD版
		// VSのバグっぽい。
		#ifdef _M_X64			// x64 コンパイル
			#ifdef _DEBUG			 
				#pragma comment(lib, "C:\\Program Files (x86)\\CONTEC\\API-PAC(W32)\\DIOWDM\\Samples\\Lib_amd64\\CDio.lib")
			#else
				#pragma comment(lib, "\"C:\\Program Files (x86)\\CONTEC\\API-PAC(W32)\\DIOWDM\\Samples\\Lib_amd64\\CDio.lib\"")
			#endif
		#else
			#ifdef _DEBUG			 
				#pragma comment(lib, "C:\\Program Files (x86)\\CONTEC\\API-PAC(W32)\\DIOWDM\\Samples\\Lib_i386\\CDio.lib")
			#else
				#pragma comment(lib, "\"C:\\Program Files (x86)\\CONTEC\\API-PAC(W32)\\DIOWDM\\Samples\\Lib_i386\\CDio.lib\"")
			#endif
		#endif
	#endif
#endif
#endif

using namespace KizuLib;


class ContecPioManager : public ContecPioBaseManager
{
private:
#ifndef DEFECTLIB_CONTEC_PIO_WMD		// 従来版
	#define CONTEC_PIO_NOID				INVALID_HANDLE_VALUE			 
#else									// WMD版
	#define CONTEC_PIO_NOID				-1
#endif

	struct CHECK_CALLBACK_ARRAY {									// 割り込みチェック設定情報
		PIO_BIT		addr;												// チェックするアドレス
		PIO_PORT	port;												// チェックするポート
		HANDLE		evSignOn;											// 割り込み認識時シグナルにするハンドル
		bool		bMode;												// 割り込み (true:0→1時  false:1→0時)
	};

public:
	ContecPioManager();
	virtual ~ContecPioManager();

	void Start() { WorkThread::Start(MainThread); };					// スレッド開始
	void Stop()  { WorkThread::Stop(10000); };							// スレッド終了


	//=========================================
	// 操作
#ifndef DEFECTLIB_CONTEC_PIO_WMD		// 従来版
	bool Alloc(WORD drvno, WORD grpno, WORD port_num, WORD kid);		// 領域確保
#else									// WMD版
	bool Alloc(WORD port_num, WORD kid, char const* dev_name="DIO000");	// 領域確保
#endif
	void Free();														// 領域開放
	

private:
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド

	BOOL Input();														// デジタルデータの入力
	BOOL Output();														// デジタルデータの出力


	//=========================================
	// 接続処理
	bool	Open(bool bDsp);											// オープン
	bool	Close();													// クローズ
	bool	m_bIsOpen;													// アクセス可能時 true


	//=========================================
	// Contecで必要

#ifndef DEFECTLIB_CONTEC_PIO_WMD		// 従来版
	HANDLE					m_hDrv;										// デバイスハンドル
	DINP					m_lpDInp;									// デジタル入力の設定状態を格納するDINP構造体
	DOUT					m_lpDOut;									// デジタル出力の設定状態を格納するDOUT構造体
	WORD					m_drvno;									// PIOボード ドライバNo
	WORD					m_grpno;									// PIOボード グループNo

#else									// WMD版
	short					m_hDrv;										// デバイスID
	char					m_dev_name[64];								// ボード名称

#endif


#ifdef DEFECTLIB_CONTEC_PIO_WMD			// WMD版
	//=========================================
	//// 割り込み用
public:
	void SetCheckCallBackArray(PIO_PORT port, PIO_BIT addr, HANDLE evSignOn, bool bMode=true);	// 割り込みチェック情報 設定
	BOOL InitCallBackInp();												// 割り込み入力準備 (Alloc、SetCheckCallBackArrayをした後)

private:
	static void _stdcall CallBackProc(short hDrv, WPARAM wParam, LPARAM lParam, void *Param);	// 割り込みコールバック


	DWORD					m_nCheckCallBackNum;						// 割り込みチェックする点数
	CHECK_CALLBACK_ARRAY 	m_CheckCallBackArray[mc_nMaxCheckCount];	// IO割り込みチェックするポート・ビットの情報
#endif
};

#endif