// *********************************************************************************
//	Contec PIOボード管理クラス
//	[Ver]
//		Ver.01    2007/03/20  vs2005 対応
//		Ver.02	  2009/09/02  WMD版ドライバに対応
//		Ver.03	  2010/10/29  シミュレータに対応
//		Ver.03	  2010/11/10  リモートPIOに対応
//
//	[メモ]
//		PC98版／WMD版で DEFECTLIB_CONTEC_PIO_WMD のデファインを切り替えること
//		【PC98版】
//			C:\Program Files\CONTEC\API-PAC(W32)\Dio\Samples\Inc		APIDIO.H
//			C:\Program Files\CONTEC\API-PAC(W32)\Dio\Samples\Lib		APIDIO.lib
//
//		【WMD x86】
//			C:\Program Files\CONTEC\API-PAC(W32)\DIOWDM\Samples\Inc		CDio.h
//			C:\Program Files\CONTEC\API-PAC(W32)\DIOWDM\Samples\Lib		CDIO.lib
//
//		【WMD x64】
//			C:\Program Files (x86)\CONTEC\API-PAC(W32)\DIOWDM\Samples\Inc		CDio.h
//			C:\Program Files (x86)\CONTEC\API-PAC(W32)\DIOWDM\Samples\Lib_amd64	CDIO.lib
//
//		【リモートPIO】
//			C:\Program Files\CONTEC\API-CAP(W32)\Samples\Inc	ccapdio.h
//			C:\Program Files\CONTEC\API-CAP(W32)\Samples\Lib	ccapdio.lib
// *********************************************************************************
#pragma once

// 半固定
#include "..\..\Include\LibSelect.h"									// ライブラリ切替ヘッダー
#include "..\..\Include\KizuCommon.h"									// 疵検インクルード


#include "..\KizuLib\KizuLib.h"											// 疵検ライブラリ
#include "..\KizuLib\Class\KizuMacro.h"									// マクロ集
#include "..\KizuLib\Class\KizuFunction.h"								// スタティックライブラリ集
#include "..\KizuLibMFC\Class\LogFileManager.h"							// ログ管理クラス

// 疵検クラス インクルード
#include "..\KizuLib\Class\WorkThread.h"								// ワーカースレッドクラス
		
using namespace KizuLib;

#ifndef  DEFECTLIB_CONTEC_PIO_NON		////// 完全無効化
  #include "..\..\Include\KizuSmyu.h"										// シミュレーションヘッダー

class ContecPioBaseManager : public WorkThread
{
public:	
	//// 受け渡し構造体の番号
	typedef enum PIO_BIT {
		BIT_0 = (0x01),
		BIT_1 = (0x02),
		BIT_2 = (0x04),
		BIT_3 = (0x08),
		BIT_4 = (0x10),
		BIT_5 = (0x20),
		BIT_6 = (0x40),
		BIT_7 = (0x80)
	};

	//// 受け渡し構造体の番号
	typedef enum PIO_PORT {
		PORT_0 = 0,
		PORT_1 = 1,
		PORT_2 = 2,
		PORT_3 = 3,
		PORT_4 = 4,
		PORT_5 = 5,
		PORT_6 = 6,
		PORT_7 = 7
	};

protected:
//#ifndef DEFECTLIB_CONTEC_PIO_WMD		// 従来版
//	#define CONTEC_PIO_NOID				INVALID_HANDLE_VALUE			 
//#else									// WMD版
//	#define CONTEC_PIO_NOID				-1
//#endif
	static const int mc_nReadInterval	= 25;							// PIO読み込み周期 [ms]
	static const int mc_nMaxCheckCount	= 64;							// 最大チェックDI点数
	static const int mc_nMaxDoPulsCount	= 16;							// 最大DOパルス出力制御点数


	struct CHECK_ARRAY {											// チェック設定情報
		PIO_BIT		addr;												// チェックするアドレス
		PIO_PORT	port;												// チェックするポート
		HANDLE		evSignOn;											// ONの時シグナルにするハンドル
		HANDLE		evSignOff;											// ONの時シグナルにするハンドル
	};

	struct CHECK_DOPULS_ARRAY {										// DOパルス出力設定情報
		PIO_BIT		addr;												// チェックするアドレス
		PIO_PORT	port;												// チェックするポート
		bool		bOnOff;												// セット時の上体 (true:OFF→ON→OFF  false:ON→OFF→ON)

		bool		bExec;												// 実行中の場合 True
		HANDLE		evTimer;											// セット時間用タイマー
	};


public:
	ContecPioBaseManager();
	virtual ~ContecPioBaseManager();

//	void Start() { WorkThread::Start(MainThread); };					// スレッド開始
//	void Stop()  { WorkThread::Stop(10000); };							// スレッド終了
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ログファイルセット
	void SetPostTID(DWORD tid) { this->my_PostTid=tid;}					// PostThreadMessageの送り先 (メインインスタンス)


	//=========================================
	// 操作
	virtual bool Alloc(WORD drvno, WORD grpno, WORD port_num, WORD kid) {return true;};			// 領域確保
	virtual bool Alloc(WORD port_num, WORD kid, char const* dev_name="DIO000") {return true;};	// 領域確保
	virtual void Free() {};																		// 領域開放
	

	//=========================================
	// Get
	BOOL GetStatusDI() const { return m_bStatusDi;}						// ボード状態 (DI)
	BOOL GetStatusDO() const { return m_bStatusDo;}						// ボード状態 (DO)
	bool GetDI(PIO_PORT port, PIO_BIT addr) const	{ return ( m_pDI[port] & addr ) ? true : false; }		// DI 指定位置の BIT ON/OFF 状態 (アクセスする変数が違うので注意)
	bool GetDO(PIO_PORT port, PIO_BIT addr) const	{ return ( m_bufOut[port] & addr ) ? true : false; }	// DO 指定位置の BIT ON/OFF 状態 (アクセスする変数が違うので注意)
	BYTE const* GetDI() const { return m_pDI;}							// DI状態
	BYTE const* GetDO() const { return m_bufOut;}						// DO状態
	HANDLE GetDiErrEvt() const { return m_evDiErr; }					// エラー認識用イベント
	HANDLE GetDoErrEvt() const { return m_evDoErr; }					// エラー認識用イベント
	void GetDIRefresh();												// 強制更新 (読込み完了するまでブロックされる)


	//=========================================
	// Set
	void SetCheckArray(PIO_PORT port, PIO_BIT addr, HANDLE evSignOn=NULL, HANDLE evSignOff=NULL);	// チェック情報 設定
	void SetDO(PIO_PORT port, PIO_BIT addr, bool onoff, bool evt = false);		// DO 指定位置ビット ON/OFF
	void WriteDO()			 { SetEvent(m_evOutputEvt); };				// 内部バッファ値を全部書き込み


	//=========================================
	// DEBUG用
	void SetAutoRead(bool mode) { m_bAuto = mode; }						// DI読み込み可(自動true)・不可(手動false)
	void SetDI(PIO_PORT port, PIO_BIT addr, bool onoff);				// DO 指定位置ビット ON/OFF

	//=========================================
	// その他Static変換
	static int	GetBitToIndex(BYTE addr);								// 指定位置ビット を No に変換
	static BYTE GetIndexToBit(int index);								// 指定位置No を ビット に変換
	static int PioNowStateCheck(PIO_PORT port, PIO_BIT addr, BYTE const* now, BYTE const* old ); // PIO変化状態チェック

protected:
//	virtual static UINT WINAPI MainThread(LPVOID param) {};				// メインスレッド

	int PioNowState(PIO_PORT port, PIO_BIT addr, BYTE* now, BYTE* old);	// デジタル値更新
	virtual BOOL Input()  {return TRUE;};								// デジタルデータの入力
	virtual BOOL Output() {return TRUE;};								// デジタルデータの出力

	//=========================================
	// 各インスタンス	
	LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス


	//=========================================
	// Contecで必要
	WORD					m_nPortNum;									// ポート数
//#ifndef DEFECTLIB_CONTEC_PIO_WMD		// 従来版
//	HANDLE					m_hDrv;										// デバイスハンドル
//	DINP					m_lpDInp;									// デジタル入力の設定状態を格納するDINP構造体
//	DOUT					m_lpDOut;									// デジタル出力の設定状態を格納するDOUT構造体			
//#else									// WMD版
//	short					m_hDrv;										// デバイスID
//#endif
	short*					m_inpPortNo;								// デジタル入力を行う論理ポート番号が格納されている配列								
	short*					m_outPortNo;								// デジタル出力を行う論理ポート番号が格納されている配列
	BYTE*					m_bufInp;									// デジタル入力データを格納するバッファ
	BYTE*					m_bufOut;									// デジタル出力データを格納するバッファ
	BYTE*					m_bufWk;									// デジタル入力データを一時格納


	//=========================================
	// 入出力チェック用
	BYTE*					m_pDI;										// 信号状態保持
	BYTE*					m_pDO;										// 信号状態保持
	BOOL					m_bStatusDi;								// DIステータス(TRUE:正常, FALSE:異常)
	BOOL					m_bStatusDo;								// DOステータス(TRUE:正常, FALSE:異常)
	DWORD					m_nCheckNum;								// チェックする点数
	CHECK_ARRAY				m_CheckArray[mc_nMaxCheckCount];			// IOチェックするポート・ビットの情報

	//=========================================
	// シグナル
	HANDLE					m_evDiErr;									// エラー認識用イベント
	HANDLE					m_evDoErr;									// エラー認識用イベント
	HANDLE					m_evOutputEvt;								// 出力要求イベント
	HANDLE					m_evLockReadStart;							// 強制読込み開始イベント
	HANDLE					m_evLockReadEnd;							// 強制読込み完了イベント

	// その他もろもろ
	WORD					m_nKID;										// 機器ID

	// DEBUG用
	bool					m_bAuto;									// 手動設定時 false




	//=========================================
	//// DOパルス出力用
public:
	void SetPulsDoSetting(PIO_PORT port, PIO_BIT addr, bool bOnOff = true);	// DOパルス設定(Start前に実行すること)
	bool SetPulsDO(PIO_PORT port, PIO_BIT addr, int nTime);					// DO 指定位置ビットを 指定時間パルス出力
	bool IsPulsDO(PIO_PORT port, PIO_BIT addr);								// DOパルス出力実行中？

protected:
	int						m_nPulsDoNum;								// 最大セット数
	CHECK_DOPULS_ARRAY		m_DoPulsArray[mc_nMaxDoPulsCount];			// パルス出力アイテム
	


	//=========================================
	//// シミュレータ関係
public:
	bool	SetSmyuMode(bool bMode); 									// シミュレータモードの設定
	bool	GetSmyuMode() { return m_bSmyu; }

protected:
	void	SmyuInput();												// デジタルデータの入力
	void	SmyuOutput();												// デジタルデータの出力

protected:
	bool					m_bSmyu;									// シミュレータ時 true; 通常時 false;
	CString					m_sSmyuName;								// PIO情報 名称
	HANDLE					m_hTblSmyu;									// PIO情報 ハンドル
	SMYU_TBL_PIO*			mtbl_pSmyu;									// PIO情報 データ

};

#endif