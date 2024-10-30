// *********************************************************************************
//	Contec リモートPIOボード管理クラス
//		オンボードPIOとリモートPIO を 同一EXEで同居させる場合は、
//		オンボードPIOの方のドライバを 従来版にする必要あり！
//		WMD だと スタティックリンクが競合してNGとなる
// *********************************************************************************
#pragma once

#include "ContecPioBaseManager.h"										// 基底クラス

#ifndef  DEFECTLIB_CONTEC_PIO_NON		////// 完全無効化

// コンテック インクルード
//	オンボードでいう WMD版しか無い
#include "..\..\include\Other\CCAPDIO.h"								// PIOボード インクルード

// VSのバグっぽい。
#ifdef _DEBUG
	#pragma comment(lib, "C:\\Program Files\\CONTEC\\API-CAP(W32)\\Samples\\Lib\\ccapdio.lib")
#else
	#pragma comment(lib, "\"C:\\Program Files\\CONTEC\\API-CAP(W32)\\Samples\\Lib\\ccapdio.lib\"")
#endif

using namespace KizuLib;


class ContecPioNetManager : public ContecPioBaseManager
{
private:
	//static const int CONTEC_PIO_NOID	= -1;

public:
	ContecPioNetManager();
	virtual ~ContecPioNetManager();

	void Start() { WorkThread::Start(MainThread); };					// スレッド開始
	void Stop()  { WorkThread::Stop(10000); };							// スレッド終了


	//=========================================
	// 操作
	bool Alloc(WORD port_num, WORD kid, char const* dev_name="DIO000");	// 領域確保
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
	short					m_hDrv;										// デバイスID
	char					m_dev_name[64];								// ボード名称

};
#endif