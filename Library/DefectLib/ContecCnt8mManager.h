// *********************************************************************************
//	Contec CNTボード管理クラス
//	[Ver]
//		Ver.01    2007/03/20  vs2005 対応
//		Ver.02	  2009/09/02  WMD版ドライバに対応
//
//	[メモ]
//		PC98版／WMD版で DEFECTLIB_CONTEC_PIO_WMD のデファインを切り替えること
//		【PC98版】
//			C:\Program Files\CONTEC\API-PAC(W32)\CNT\Samples\inc		APICNT8M.H
//			C:\Program Files\CONTEC\API-PAC(W32)\CNT\Samples\lib		APICNT.lib
//		【WMD x64】
//			C:\Program Files (x86)\CONTEC\API-PAC(W32)\CNTWDM\Samples\Inc		CCnt.h
//			C:\Program Files (x86)\CONTEC\API-PAC(W32)\CNTWDM\Samples\Lib_amd64	CCNT.lib
// *********************************************************************************
#pragma once

#include <winsock2.h>													// UDP送信用

// 半固定
#include "..\..\Include\LibSelect.h"									// ライブラリ切替ヘッダー
#include "..\..\Include\KizuCommon.h"									// 疵検インクルード
#include "..\KizuLib\KizuLib.h"											// 疵検ライブラリ
#include "..\KizuLib\Class\KizuMacro.h"									// マクロ集


#ifndef  DEFECTLIB_CONTEC_CNT_NON		////// 完全無効化
  #include "..\..\Include\KizuSmyu.h"										// シミュレーションヘッダー


// コンテック インクルード
#ifdef DEFECTLIB_CONTEC_CNT_PATH_H
	#include DEFECTLIB_CONTEC_CNT_PATH_H
#else
	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
		#include "..\..\include\Other\APICNT8M.H"							// カウンターボード インクルード
	#else									// WMD版
		#include "..\..\include\Other\CCnt.h"								// CNTボード インクルード
	#endif
#endif

// ライブラリ
#ifndef LIB_MANUAL
#ifdef DEFECTLIB_CONTEC_CNT_PATH_L
	#pragma comment(lib, DEFECTLIB_CONTEC_CNT_PATH_L)
#else
	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
		// VSのバグっぽい。
		#ifdef _DEBUG
			#pragma comment(lib, "C:\\Program Files\\CONTEC\\API-PAC(W32)\\CNT\\Samples\\lib\\APICNT.lib")
		#else
			#pragma comment(lib, "\"C:\\Program Files\\CONTEC\\API-PAC(W32)\\CNT\\Samples\\lib\\APICNT.lib\"")
		#endif
	#else									// WMD版
		// VSのバグっぽい。
		#ifdef _M_X64			// x64 コンパイル
			#ifdef _DEBUG
				#pragma comment(lib, "C:\\Program Files (x86)\\CONTEC\\API-PAC(W32)\\CNTWDM\\Samples\\Lib_amd64\\CCNT.lib")
			#else
				#pragma comment(lib, "\"C:\\Program Files (x86)\\CONTEC\\API-PAC(W32)\\CNTWDM\\Samples\\Lib_amd64\\CCNT.lib\"")
			#endif
		#else
			#ifdef _DEBUG
				#pragma comment(lib, "C:\\Program Files (x86)\\CONTEC\\API-PAC(W32)\\CNTWDM\\Samples\\Lib_i386\\CCNT.lib")
			#else
				#pragma comment(lib, "\"C:\\Program Files (x86)\\CONTEC\\API-PAC(W32)\\CNTWDM\\Samples\\Lib_i386\\CCNT.lib\"")
			#endif
		#endif
	#endif
#endif
#endif

// テストパルス出力
#ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
	extern "C" {		
		DWORD __stdcall CntTestPulse(HANDLE, WORD, WORD);
	}
#endif

class ContecCnt8mManager
{
public:
	//// APIのアクセス時のエラー
	enum ENUM_CNT_ERRCODE {
			CntOpenExErr = 1,
			CntSelectChannelSignalErr,
			CntSetOperationModeErr,
			CCntSetZModeErr,
			CntSetZLogicErr,
			CntSetCountDirectionErr,
			CntSetDigitalFilterErr,
			CntSetSamplingStartTrgErr,
			CntSetSamplingClockTrgErr,
			CntSetSamplingClockErr,
			CntSetSamplingStopTrgErr,
			CntSetSamplingChannelErr,
			CntResetChannelErr,
			CntStartCountErr,
			CntResetSamplingErr,
			CntSetSamplingBuffErr,
			CntStartSamplingErr,
			CntStopSamplingErr,
			CntStopCountErr,
			CntGetSamplingCountErr,
			ContecCnt8mManagerErr,
			CntTestPulseErr,
			CntGetSamplingStatusErr,

			CntSetOutputHardwareEventErr,
			CntSetCountMatchHardwareEventErr,
			CntNotifyCountUpErr,
			CntNotifySamplingCountErr,
			CntCountUpCallbackProcErr,
			CntSamplingCountCallbackProcErr
	};
	
	//// カウンターボード全般の設定パラメータ
	struct Param_CntBase {
		bool		m_bTest;						// テストパルスを出力するかどうか
	};
	//// カウンターボードチャンネル毎の設定パラメータ
	struct Param_CntCH {
//#ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
//		WORD		m_nSigType;						// 信号源 [CNT_SIGTYPE_LINERECEIVER, CNT_SIGTYPE_TTL](CntSelectChannelSignal)
//		WORD		m_nPhase;						// 相数 [CNT_MODE_2PHASE, CNT_MODE_1PHASE]
//		WORD		m_nMul;							// 逓倍 [CNT_MUL_X4, CNT_MUL_X2 ,CNT_MUL_X1]
//		WORD		m_nSyncClr;						// 同期クリア／非同期クリア [CNT_CLR_ASYNC]
//		WORD		m_nZmode;						// Z相モード [CNT_ZPHASE_NOT_USE]
//		WORD		m_nZlogic;						// Z相論理 [CNT_ZLOGIC_NEGATIVE]
//		WORD		m_nDir;							// カウント方向 [CNT_DIR_UP, CNT_DIR_DOWN]
//		WORD		m_nFilterValue;					// フィルタの時定数
//#else									// WMD版
		short		m_nSigType;						// 信号源 [CNT_SIGTYPE_LINERECEIVER, CNT_SIGTYPE_TTL](CntSelectChannelSignal)
		short		m_nPhase;						// 相数 [CNT_MODE_2PHASE, CNT_MODE_1PHASE]
		short		m_nMul;							// 逓倍 [CNT_MUL_X4, CNT_MUL_X2 ,CNT_MUL_X1]
		short		m_nSyncClr;						// 同期クリア／非同期クリア [CNT_CLR_ASYNC]
		short		m_nZmode;						// Z相モード [CNT_ZPHASE_NOT_USE]
		short		m_nZlogic;						// Z相論理 [CNT_ZLOGIC_NEGATIVE]
		short		m_nDir;							// カウント方向 [CNT_DIR_UP, CNT_DIR_DOWN]
		short		m_nFilterValue;					// フィルタの時定数
//#endif
	};
	//// カウンターボードサンプリング条件設定パラメータ
	struct Param_CntSamp {
#ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
		DWORD		m_nStart;						// サンプリングスタート条件 [CNT8M_START_SOFT] (CntSetSamplingStartTrg)
		WORD		m_nCountStart;					// サンプリング開始時にカウンタをスタートするか [CNT8M_START_CNT_NOT_WITH_SAMP] (CntSetSamplingStartTrg)
		DWORD		m_nClock;						// サンプリングクロック条件 [CNT8M_CLOCK_EXT_TRG, CNT8M_CLOCK_CLOCK]
		DWORD		m_nStop;						// サンプリングストップ条件 [CNT8M_STOP_SOFT]
		WORD		m_nCountStop;					// サンプリング停止時にカウンタをストップするかどうか [CNT8M_STOP_CNT_NOT_WITH_SAMP]
#else									// WMD版
		short		m_nStart;						// サンプリングスタート条件 [CNT8M_START_SOFT] (CntSetSamplingStartTrg)
		short		m_nCountStart;					// サンプリング開始時にカウンタをスタートするか [CNT8M_START_CNT_NOT_WITH_SAMP] (CntSetSamplingStartTrg)
		short		m_nClock;						// サンプリングクロック条件 [CNT8M_CLOCK_EXT_TRG, CNT8M_CLOCK_CLOCK]
		short		m_nStop;						// サンプリングストップ条件 [CNT8M_STOP_SOFT]
		short		m_nCountStop;					// サンプリング停止時にカウンタをストップするかどうか [CNT8M_STOP_CNT_NOT_WITH_SAMP]
#endif	
	};

private :
#ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
	#define CONTEC_CNT_NOID				INVALID_HANDLE_VALUE			 
#else									// WMD版
	#define CONTEC_CNT_NOID				-1
#endif
	static const int mc_nMaxCannel	= 8;			// 本クラスで管理する最大チャンネル数
	static const int mc_nMaxUdp		= 4;			// UDP管理最大数

/*
	// サンプリングバッファ構造体
	struct SampData {	
		long	plg;								// ch0 の値 (= ラインPLGのパルス数)
		DWORD	clk;								// ch1 の値 (= テストパルス10μsec周期の数)
	};
*/
public:
	
	ContecCnt8mManager(WORD nChannelNum, WORD nSampChannelNum, DWORD nSampBuffSize, DWORD nSampBuffNum);
	virtual ~ContecCnt8mManager();


	//// パラメータアクセス (オープン前に設定を完了しておく事)
	Param_CntBase&	SetParamCntBase()		{return m_parmCntBase;  }	// カウンターボード全般の設定パラメータ
	Param_CntCH&	SetParamCntCH(WORD ch)	{return m_parmCntCH[ch];}	// カウンターボードチャンネル毎の設定パラメータ
	Param_CntSamp&	SetParamCntSamp()		{return m_parmCntSamp;  }	// カウンターボードサンプリング条件設定パラメータ

	// Set (必須設定)
	void SetEvSampStart()			{ SetEvent(my_evSamplingStart);	}	// シグナルセット サンプリングスタート
	void SetEvSampStop()			{ SetEvent(my_evSamplingStop);	}	// シグナルセット サンプリングストップ
	void SetEvTimerCancel()			{ CancelWaitableTimer(my_evTimer);}	// タイマシグナルキャンセル
	void SetDistPostion(int torb)	{ my_nDistPos = torb; my_nDistNotPos = 0==torb ? 1 : 0;}	// 基準面(より上流の方)が裏か表か(0:表,1:裏)
	void SetDistLen(DWORD len)		{ my_nDistLen = len;	}			// 表裏間距離 [mm]
	void SetEvTimerStart(int interval) {								// タイマシグナルセット
		__int64 ts = 0;
		SetWaitableTimer(my_evTimer, (LARGE_INTEGER *)&ts, interval, NULL, NULL, FALSE);
	}

	//// 任意に設定可能
	DWORD TestPulse(short int_ext=0);									// テストパルスの内部／外部切替制御
	// 一致出力
	DWORD OutputHardwareEvent(WORD ChNum, short EventType, short PulseWidth);	// 制御出力信号のハードウェアイベント設定
	DWORD CountMatchHardwareEvent(WORD ChNo, short RegisterNo, short eventType);// カウント一致のハードウェアイベント設定
	DWORD ResetChannel(int nChIndex, int ChNum);						// カウント値をゼロクリアします。
	DWORD NotifyCountUp(WORD ChNo, short RegNo, long plsCnt);			// カウント一致による通知の指定設定
	DWORD NotifySamplingCount(long Count);								// 指定個数サンプリング完了の通知設定
	DWORD CountUpCallbackProc(void* CallBackProc, void* param);			// CntNotifyCountUp関数で設定したカウント一致によりイベントが発生した場合に呼び出されるコールバック関数を登録
	DWORD SamplingCountCallbackProc(void* CallBackProc, void* param);


	// Get
	HANDLE	GetEvCntError() const			{ return my_evCntError;  }	// エラー発生シグナル
	DWORD	GetDistLen()	const			{ return my_nDistLen; }		// 表裏間距離 [mm]
	int		GetDistPos()	const			{ return my_nDistPos; }		// 基準面(より上流の方)が裏か表か(0:表,1:裏)
	int		GetDistNotPos()	const			{ return my_nDistNotPos; }	// 基準面と逆の方(0:表,1:裏)
	bool	GetStatus()     const			{ return m_bStatus;		 }	// カウンターボード状態 取得

	// 小物
	int		GetCntMul(int nWk) { 
		int mul = CNT_MUL_X1;
		if( 1 == nWk )		mul = CNT_MUL_X1;
		else if( 2 == nWk )	mul = CNT_MUL_X2;
		else if( 4 == nWk ) mul = CNT_MUL_X4;
		return mul;
	}

protected:
	DWORD CountOpen(char* dev_name = "CNT00");							// カウンターボード設定
	DWORD SampOpen();													// サンプリング設定
	DWORD CountStart();													// カウンタースタート
	DWORD SampStart();													// サンプリングスタート
	DWORD CountStop();													// カウンターストップ
	DWORD CountClose();													// カウンターボード開放
	DWORD SampStop();													// サンプリングストップ
	BOOL  SetUdp(int no, const char* ip, WORD port);					// UDP設定
	virtual void ContecError(DWORD dwRet) = 0;							// 上記関数エラー時の処理

	// Get
	long  GetErrorCode()				 const { return m_errCode;	}				// 異常コード
	DWORD GetSampStat(DWORD* status, DWORD* err);									// サンプリングステータスを取得
	DWORD GetSamplingCount(DWORD* sampCnt, DWORD* sampCarry);						// 現サンプリングカウンター値取得
	DWORD GetSamplingCount(DWORD* sampCnt);											// 現サンプリングカウンター値取得
	DWORD GetCount(WORD ChNum, short* ChNo, DWORD* count);							// 複数のカウント値取得
	DWORD GetCount(DWORD* count, WORD channel);										// 単一のカウント値取得
//	DWORD GetSampBuffClk(DWORD buff_pos) const { return m_pSampBuf[buff_pos].clk;}	// サンプリングクロック値
//	long  GetSampBuffPlg(DWORD buff_pos) const { return m_pSampBuf[buff_pos].plg;}	// サンプリングPLG値
//	DWORD GetSampBuff(DWORD buff_pos, int ch) const { return m_pSampBuf[buff_pos*m_nSampChannelNum+ch]; } // サンプリングクロック値
	DWORD GetSampBuff(DWORD buff_pos, int ch) const									// サンプリングクロック値
			{ 
				if(!m_bSmyu) return m_pSampBuf[buff_pos*m_nSampChannelNum+ch];
				else		 return mtbl_pSmyu->nSampBuf[buff_pos][ch];
			} 
	DWORD GetSampBuffClk(DWORD buff_pos) const { return GetSampBuff(buff_pos, 1);}			// サンプリングクロック値 (ch1固定)
	long  GetSampBuffPlg(DWORD buff_pos) const { return (long)GetSampBuff(buff_pos, 0);}	// サンプリングPLG値 (ch0固定)

	// Set
	void  SetStatus(bool mode)				   { m_bStatus = mode; }	// カウンターボード状態 セット

	// ソケット
	SOCKET GetSock(int no)				 const { return m_sock[no];	}				// UDPソケットインスタンス
	sockaddr_in* GetAddr(int no) 			   { return &m_addr[no]; }				// UDPソケットsockaddr_in構造体


	// 同期オブジェクト
	HANDLE			my_evSamplingStart;									// サンプリングスタート
	HANDLE			my_evSamplingStop;									// サンプリングストップ
	HANDLE			my_evCntError;										// エラー
	HANDLE			my_evTimer;											// 定周期タイマー用

	//// 同期制御
	CRITICAL_SECTION	my_csLock;										// クリティカルセクション (使うならご自由に)

	// その他もろもろ
	int				my_nDistPos;										// 基準面(より上流の方)が裏か表か(0:表,1:裏)
	int				my_nDistNotPos;										// 基準面と逆の方(0:表,1:裏)
	DWORD			my_nDistLen;										// 表裏間距離 [mm]
	bool			my_bSendflg;										// 送信有無(TRUE:送信有, FALSE:送信無)


private:
	// カウンターボード関連
#ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
	HANDLE			m_hDrv;												// デバイスハンドル
	WORD			m_nChannel[mc_nMaxCannel];							// チャンネル番号
#else									// WMD版
	short			m_hDrv;												// デバイスID
	short			m_nChannel[mc_nMaxCannel];							// チャンネル番号
#endif
	long			m_errCode;											// エラーコード
	Param_CntBase	m_parmCntBase;										// カウンターボード全般の設定パラメータ
	WORD			m_nChannelNum;										// 使用チャンネル数
	Param_CntCH		m_parmCntCH[mc_nMaxCannel];							// カウンターボードチャンネル毎の設定パラメータ
	Param_CntSamp	m_parmCntSamp;										// カウンターボードサンプリング条件設定パラメータ

	// サンプリング関連
	//SampData*		m_pSampBuf;											// サンプリングバッファ
	DWORD*			m_pSampBuf;											// サンプリングバッファ
	WORD			m_nSampChannelNum;									// サンプリングを行うチャンネル数 (Ch0～指定した数(=ch数分) の連続したChを同時にサンプリングする )
	DWORD			m_nSampBufSize;										// サンプリングバッファサイズ
	DWORD			m_nSampBufNum;										// サンプリング数

	// UDP関連
	//int				m_sock[mc_nMaxUdp];									// ソケット本体
	SOCKET			m_sock[mc_nMaxUdp];									// ソケット本体		
	sockaddr_in		m_addr[mc_nMaxUdp];									// sockaddr_in構造体

	// その他もろもろ
	bool			m_bStatus;											// カウンターボード状態



	//=========================================
	//// シミュレータ関係
public:
	bool	SetSmyuMode(bool bMode); 									// シミュレータモードの設定
	bool	GetSmyuMode() { return m_bSmyu; }

private:
	void	SmyuCntResetSampling();										// サンプリングバッファの初期化
	DWORD	SmyuGetSamplingCount(DWORD* sampCnt);						// 最新のサンプリングカウント値 取得
	DWORD	SmyuGetReadCount(WORD ChNum, short* ChNo, DWORD* count);	// 最新のカウント値 取得

private:
	bool					m_bSmyu;									// シミュレータ時 true;
	CString					m_sSmyuName;								// PLG情報 名称
	HANDLE					m_hTblSmyu;									// PLG情報 ハンドル
	SMYU_TBL_PLG*			mtbl_pSmyu;									// PLG情報 データ
};

#endif