// ContecCnt8mManager.cpp: ContecCnt8mManager クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "ContecCnt8mManager.h"
#ifndef  DEFECTLIB_CONTEC_CNT_NON		////// 完全無効化

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// コンストラクタ
// WORD nChannelNum			使用チャンネル数
// WORD nSampChannelNum		サンプリングするチャンネル数
// DWORD nSampBuffSize		サンプリングバッファのサイズ
// DWORD nSampBuffNum		サンプリングバッファのチャネル数
//------------------------------------------
ContecCnt8mManager::ContecCnt8mManager(WORD nChannelNum, WORD nSampChannelNum, DWORD nSampBuffSize, DWORD nSampBuffNum) :
m_hDrv(CONTEC_CNT_NOID),
m_nChannelNum(nChannelNum),
m_nSampChannelNum(nSampChannelNum),
m_nSampBufSize(nSampBuffSize),
m_nSampBufNum(nSampBuffNum),
m_errCode(0),
m_pSampBuf(NULL),
m_bStatus(true),
my_nDistPos(0),
my_nDistNotPos(1),
my_nDistLen(0),
my_bSendflg(false),
m_bSmyu(false)
{
	_ASSERT(0 != m_nChannelNum);
	_ASSERT(mc_nMaxCannel >= m_nChannelNum);

	//// パラメータ初期設定 (デフォルト値としてセットしておく)
	// カウンターボード全般の設定パラメータ
	m_parmCntBase.m_bTest = true;
	// カウンターボードチャンネル毎の設定パラメータ
	for(WORD ii=0; ii<m_nChannelNum; ii++ ) {
		m_nChannel[ii] = ii;
		m_parmCntCH[ii].m_nSigType		= CNT_SIGTYPE_LINERECEIVER;
		m_parmCntCH[ii].m_nPhase		= CNT_MODE_1PHASE;
		m_parmCntCH[ii].m_nMul			= CNT_MUL_X1;
		m_parmCntCH[ii].m_nSyncClr		= CNT_CLR_ASYNC;
		m_parmCntCH[ii].m_nZmode		= CNT_ZPHASE_NOT_USE;
		m_parmCntCH[ii].m_nZlogic		= CNT_ZLOGIC_NEGATIVE;
		m_parmCntCH[ii].m_nDir			= CNT_DIR_UP;
		m_parmCntCH[ii].m_nFilterValue	= 0;
	}
	// カウンターボードサンプリング条件設定パラメータ
#ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
	m_parmCntSamp.m_nStart				= CNT8M_START_SOFT;
	m_parmCntSamp.m_nCountStart			= CNT8M_START_CNT_NOT_WITH_SAMP;
	m_parmCntSamp.m_nClock				= CNT8M_CLOCK_EXT_TRG;
	m_parmCntSamp.m_nStop				= CNT8M_STOP_SOFT;
	m_parmCntSamp.m_nCountStop			= CNT8M_STOP_CNT_NOT_WITH_SAMP;
#else									// WMD版
	m_parmCntSamp.m_nStart				= CNTS_START_SOFT;
	m_parmCntSamp.m_nCountStart			= CNTS_START_CNT_NOT_WITH_SAMP;
	m_parmCntSamp.m_nClock				= CNTS_CLOCK_EXT_TRG;
	m_parmCntSamp.m_nStop				= CNTS_STOP_SOFT;
	m_parmCntSamp.m_nCountStop			= CNTS_STOP_CNT_NOT_WITH_SAMP;
#endif	

	//// サンプリングバッファ生成、初期化
	if( m_nSampBufSize*m_nSampBufNum > 0 ) {
		//m_pSampBuf = new SampData[m_nSampBufSize*m_nSampBufNum];
		//memset(m_pSampBuf, 0x00, sizeof(SampData)*(m_nSampBufSize*m_nSampBufNum));
		m_pSampBuf = new DWORD[m_nSampBufSize*m_nSampBufNum*m_nSampChannelNum];
		memset(m_pSampBuf, 0x00, sizeof(DWORD)*(m_nSampBufSize*m_nSampBufNum*m_nSampChannelNum));
	}

	//// UDPソケット初期化
	for( int ii=0; ii<mc_nMaxUdp; ii++) {
		m_sock[ii] = INVALID_SOCKET;
	}

	//// 同期オブジェクト生成
	my_evSamplingStart	= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evSamplingStop	= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evCntError		= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evTimer			= CreateWaitableTimer(NULL, FALSE, NULL);

	InitializeCriticalSection(&my_csLock);						// クリティカルセクションオブジェクトを初期化


	//// シミュレータ用
	m_sSmyuName = "";
	m_hTblSmyu = NULL;
	mtbl_pSmyu = NULL; 
}

//------------------------------------------
// デストラクタ
//------------------------------------------
ContecCnt8mManager::~ContecCnt8mManager()
{
	// 同期オブジェクト破棄
	CloseHandle(my_evSamplingStart);
	CloseHandle(my_evSamplingStop);
	CloseHandle(my_evCntError);
	CloseHandle(my_evTimer);

	DeleteCriticalSection(&my_csLock);							// クリティカルセクションオブジェクトを開放

	// UDPソケット破棄
	for( int ii=0; ii<mc_nMaxUdp; ii++) {
		if( INVALID_SOCKET != m_sock[ii] ) closesocket(m_sock[ii]);
	}

	// delete Sampling Buffer
	if( NULL != m_pSampBuf ) delete [] m_pSampBuf;

	//// シミュレータ用
	SetSmyuMode(false);
}

//------------------------------------------
// カウンターボード設定
// char* dev_name ボード名称
//------------------------------------------
DWORD ContecCnt8mManager::CountOpen(char* dev_name)
{
	//// シミュレータ用準備
	m_sSmyuName.Format(SMYU_PLG_TBL_NAME, dev_name );


#ifndef NO_HW
	//// CntOpen
#ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
	CNTMESS	lpCntMess;
	DWORD nRet = CntOpenEx(dev_name, &m_hDrv, 0, 0, &lpCntMess);
#else									// WMD版
	long  nRet = CntInit  (dev_name, &m_hDrv);
#endif	
	if( 0 != nRet ) {
		m_errCode	= nRet;		
		return CntOpenExErr;
	}

	//// テストパルス
	if( m_parmCntBase.m_bTest ) {
//#ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
//		nRet = CntTestPulse   (m_hDrv, 0, 1);
//#else									// WMD版
//		nRet = CntSetTestPulse(m_hDrv, 0, 1, 0);
//#endif
		nRet = TestPulse(0);
		if( 0 != nRet ) {
			m_errCode	= nRet;		
			return CntTestPulseErr;
		}
	}

	//// 使用チャンネル分設定を行う
	for(WORD ii=0; ii<m_nChannelNum; ii++ ) {

		// 外部信号源
		nRet = CntSelectChannelSignal(m_hDrv, m_nChannel[ii], m_parmCntCH[ii].m_nSigType);
		if( 0 != nRet ) {
			m_errCode	= nRet;		
			return CntSelectChannelSignalErr;
		}
	
		// 動作モード
		nRet = CntSetOperationMode(m_hDrv, m_nChannel[ii],
							m_parmCntCH[ii].m_nPhase, m_parmCntCH[ii].m_nMul, m_parmCntCH[ii].m_nSyncClr);
		if( 0 != nRet ) {
			m_errCode	= nRet;		
			return CntSetOperationModeErr;
		}

		// Z相使用方法
		nRet = CntSetZMode(m_hDrv, m_nChannel[ii], m_parmCntCH[ii].m_nZmode);
		if( 0 != nRet ) {
			m_errCode	= nRet;		
			return CCntSetZModeErr;
		}

		// Z相論理
		nRet = CntSetZLogic(m_hDrv, m_nChannel[ii], m_parmCntCH[ii].m_nZlogic);
		if( 0 != nRet ) {
			m_errCode	= nRet;	
			return CntSetZLogicErr;
		}

		// カウント方向
		nRet = CntSetCountDirection(m_hDrv, m_nChannel[ii], m_parmCntCH[ii].m_nDir);
		if( 0 != nRet ) {
			m_errCode	= nRet;		
			return CntSetCountDirectionErr;
		}

		// デジタルフィルタ
		nRet = CntSetDigitalFilter(m_hDrv, m_nChannel[ii],	m_parmCntCH[ii].m_nFilterValue);
		if( 0 != nRet ) {
			m_errCode	= nRet;		
			return CntSetDigitalFilterErr;
		}
	}
#endif	
	return 0;
}

//------------------------------------------
// サンプリング設定
//------------------------------------------
DWORD ContecCnt8mManager::SampOpen()
{
#ifdef NO_HW
	return 0;
#endif

	_ASSERT(CONTEC_CNT_NOID != m_hDrv);
	_ASSERT(0 != m_nSampChannelNum);
	
	// サンプリングスタート条件
#ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
	DWORD nRet = CntSetSamplingStartTrg    (m_hDrv, m_parmCntSamp.m_nStart, m_parmCntSamp.m_nCountStart);
#else									// WMD版
	long  nRet = CntSetSamplingStartTrigger(m_hDrv, m_parmCntSamp.m_nStart, m_parmCntSamp.m_nCountStart, m_nSampChannelNum);
#endif	
	if( 0 != nRet ) {
		m_errCode	= nRet;		
		return CntSetSamplingStartTrgErr;
	}

	// サンプリングクロック条件
#ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
	nRet = CntSetSamplingClockTrg    (m_hDrv, m_parmCntSamp.m_nClock);
#else									// WMD版
	nRet = CntSetSamplingClockTrigger(m_hDrv, m_parmCntSamp.m_nClock);
#endif	
	if( 0 != nRet ) {
		m_errCode	= nRet;		
		return CntSetSamplingClockTrgErr;
	}

	// 内部クロック
#ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
	nRet = CntSetSamplingClock        (m_hDrv, 1, CNT8M_TIM_UNIT_US);
#else									// WMD版
	nRet = CntSetSamplingInternalClock(m_hDrv, 1, CNTS_TIM_UNIT_US);
#endif	
	if( 0 != nRet ) {
		m_errCode	= nRet;	
		return CntSetSamplingClockErr;
	}
	
	// サンプリングストップ条件
#ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
	nRet = CntSetSamplingStopTrg    (m_hDrv, m_parmCntSamp.m_nStop, m_parmCntSamp.m_nCountStop);
#else									// WMD版
	nRet = CntSetSamplingStopTrigger(m_hDrv, m_parmCntSamp.m_nStop, m_parmCntSamp.m_nCountStop, m_nSampChannelNum);
#endif	
	if( 0 != nRet ) {
		m_errCode	= nRet;	
		return CntSetSamplingStopTrgErr;
	}

	// サンプリングを行うCH数
#ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
	nRet = CntSetSamplingChannel(m_hDrv, m_nSampChannelNum);
	if( nRet != 0 ) {
		m_errCode	= nRet;	
		return CntSetSamplingChannelErr;
	}
#else									// WMD版
		// スタート時に設定するようになった為、必要なし
#endif	
	return 0;
}

//------------------------------------------
// カウンタースタート
//------------------------------------------
DWORD ContecCnt8mManager::CountStart()
{
#ifndef NO_HW
	_ASSERT(CONTEC_CNT_NOID != m_hDrv);

	// カウント値をゼロクリア
  #ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
	DWORD nRet = CntResetChannel(m_hDrv, m_nChannel, m_nChannelNum);
  #else									// WMD版
	DWORD nPresetData[mc_nMaxCannel];
	memset(nPresetData, 0x00, sizeof(nPresetData));
	long  nRet = CntPreset(m_hDrv, m_nChannel, m_nChannelNum, nPresetData);
  #endif	
	if( 0 != nRet ) {
		m_errCode	= nRet;	
		return CntResetChannelErr;
	}

	// カウンタをスタート
  #ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
	nRet = CntStartCount(m_hDrv, m_nChannel, m_nChannelNum, 0);
  #else									// WMD版
	nRet = CntStartCount(m_hDrv, m_nChannel, m_nChannelNum);
  #endif	
	if( 0 != nRet ) {
		m_errCode	= nRet;	
		return CntStartCountErr;
	}
#endif
	return 0;
}

//------------------------------------------
// サンプリングスタート
//------------------------------------------
DWORD ContecCnt8mManager::SampStart()
{
	if( ! m_bSmyu) {	
#ifndef NO_HW
		_ASSERT(CONTEC_CNT_NOID != m_hDrv);
		_ASSERT(0 != m_nSampChannelNum);
		_ASSERT(m_nSampBufNum*m_nSampBufSize> 0);

		// FIFOをリセット
	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
		DWORD nRet = CntResetSampling(m_hDrv, CNT8M_RESET_FIFO);
	#else									// WMD版
		long nRet  = CntSamplingFifoReset(m_hDrv);
	#endif	
		if( 0 != nRet ) {
			m_errCode	= nRet;	
			return CntResetSamplingErr;
		}
			
		// サンプリングバッファの設定
		//memset(m_pSampBuf, 0x00, sizeof(SampData)*(m_nSampBufSize*m_nSampBufNum));
		//nRet = CntSetSamplingBuff(m_hDrv, (DWORD *)m_pSampBuf, m_nSampChannelNum, m_nSampBufNum*m_nSampBufSize, CNT8M_WRITE_RING);
		memset(m_pSampBuf, 0x00, sizeof(DWORD)*(m_nSampBufSize*m_nSampBufNum*m_nSampChannelNum));
	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
		nRet = CntSetSamplingBuff  (m_hDrv, m_pSampBuf, m_nSampChannelNum, m_nSampBufNum*m_nSampBufSize, CNT8M_WRITE_RING);
	#else									// WMD版
		nRet = CntSamplingSetBuffer(m_hDrv, m_pSampBuf,	m_nSampChannelNum, m_nSampBufNum*m_nSampBufSize, CNTS_WRITE_RING);
	#endif	
		if( 0 != nRet ) {
			m_errCode	= nRet;	
			return CntSetSamplingBuffErr;
		}

		// サンプリングをスタート
	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
		nRet = CntStartSampling(m_hDrv);
	#else									// WMD版
		nRet = CntSamplingStart(m_hDrv);
	#endif	
		if( 0 != nRet ) {
			m_errCode	= nRet;	
			return CntStartSamplingErr;
		}
#endif
		return 0;
	} else {
		SmyuCntResetSampling();
		return 0;
	}
}
//------------------------------------------
// サンプリングストップ
//------------------------------------------
DWORD ContecCnt8mManager::SampStop()
{
#ifndef NO_HW
	_ASSERT(CONTEC_CNT_NOID != m_hDrv);

	// サンプリングを強制停止します。
  #ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
	DWORD nRet = CntStopSampling(m_hDrv);
  #else									// WMD版
	long nRet  = CntSamplingStop(m_hDrv);
  #endif	
	if( 0 != nRet ) {
		m_errCode	= nRet;	
		return CntStopSamplingErr;
	}
#endif
	return 0;
}
//------------------------------------------
// カウンターストップ
//------------------------------------------
DWORD ContecCnt8mManager::CountStop()
{
#ifndef NO_HW
	_ASSERT(CONTEC_CNT_NOID != m_hDrv);

	// カウンタストップ
  #ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
	CntStopCount(m_hDrv, m_nChannel, m_nChannelNum, 0);
  #else									// WMD版
	CntStopCount(m_hDrv, m_nChannel, m_nChannelNum);
  #endif
#endif
	return 0;
}
//------------------------------------------
// サンプリングストップ
//------------------------------------------
DWORD ContecCnt8mManager::CountClose()
{
#ifndef NO_HW
	_ASSERT(CONTEC_CNT_NOID != m_hDrv);

	// CntClose
  #ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
	CntClose(m_hDrv);
  #else									// WMD版
	CntExit(m_hDrv);
  #endif
#endif
	return 0;
}

//------------------------------------------
// 現 複数カウンター値取得
// WORD ChNum   読み込みたいチャンネル数
// short* ChNo   読み込みたいチャンネル番号の配列 (0オリジン)
// DWORD* count 読み込んだカウント値の配列
// 戻り値 復帰情報
//------------------------------------------
DWORD ContecCnt8mManager::GetCount(WORD ChNum, short* ChNo, DWORD* count)
{
	// カウント値の取得
	long  nRet = 0;
	if( ! m_bSmyu) {	
#ifndef NO_HW
		_ASSERT(CONTEC_CNT_NOID != m_hDrv);

	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
		nRet = CntReadCnt(m_hDrv, (WORD*)ChNo, ChNum, count);
	#else									// WMD版
		nRet = CntReadCount(m_hDrv, ChNo, ChNum, count);
	#endif	
#endif
	} else {
		nRet = SmyuGetReadCount(ChNum, ChNo, count);
	}

	if( nRet != 0 ) {
		m_errCode	= nRet;	
		return ContecCnt8mManagerErr;
	}
	return 0;
}

//------------------------------------------
// 現 単一カウンター値取得
// DWORD* count 読み込んだカウント値
// WORD channel 読み込みたいチャンネルレコード (0オリジン)
// 戻り値 復帰情報
//------------------------------------------
DWORD ContecCnt8mManager::GetCount(DWORD* count, WORD channel)
{
	//// ホントは、GetCount(WORD ChNum, WORD* ChNo, DWORD* count) を内部でコールするだけでよいが、あまりテストができない為、こうしておく

	// カウント値の取得
	long  nRet = 0;
	if( ! m_bSmyu) {	
#ifndef NO_HW
	_ASSERT(CONTEC_CNT_NOID != m_hDrv);
	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
		nRet = CntReadCnt(m_hDrv, &m_nChannel[channel], 1, count);
	#else									// WMD版
		nRet = CntReadCount(m_hDrv, &m_nChannel[channel], 1, count);
	#endif
#endif
	} else {
		nRet = SmyuGetReadCount(1, (short*)&m_nChannel[channel], count);
	}

	if( nRet != 0 ) {
		m_errCode	= nRet;	
		return ContecCnt8mManagerErr;
	}
	return 0;
}

//------------------------------------------
// 現サンプリングカウンター値取得
// DWORD* sampCnt 読み込んだカウント値   下位32bit
// DWORD* sampCarry 読み込んだカウント値 上位32bit？
// 戻り値 復帰情報
//------------------------------------------
DWORD ContecCnt8mManager::GetSamplingCount(DWORD* sampCnt, DWORD* sampCarry)
{
	// サンプリング回数の取得
	DWORD nRet = 0;
	if(!m_bSmyu) {
#ifndef NO_HW		// 本番
		_ASSERT(CONTEC_CNT_NOID != m_hDrv);
		nRet = CntGetSamplingCount(m_hDrv, sampCnt, sampCarry);
#endif
	} else {
		nRet = SmyuGetSamplingCount(sampCnt);
	}

	if( nRet != 0 ) {
		m_errCode	= nRet;
		return CntGetSamplingCountErr;
	}
	return 0;
}
//------------------------------------------
// 現サンプリングカウンター値取得
// DWORD* sampCnt 読み込んだカウント値
// 戻り値 復帰情報
//------------------------------------------
DWORD ContecCnt8mManager::GetSamplingCount(DWORD* sampCnt)
{
/*
	//// ホントは、GetSamplingCount(DWORD* sampCnt, DWORD* sampCarry) を内部でコールするだけでよいが、あまりテストができない為、こうしておく
	_ASSERT(CONTEC_CNT_NOID != m_hDrv);

	// サンプリング回数の取得
	DWORD sampCarry;
	DWORD nRet = CntGetSamplingCount(m_hDrv, sampCnt, &sampCarry);
	if( nRet != 0 ) {
		m_errCode	= nRet;
		return CntGetSamplingCountErr;
	}
	return 0;
	*/

	//// ホントは、GetSamplingCount(DWORD* sampCnt, DWORD* sampCarry) を内部でコールするだけでよいが、あまりテストができない為、こうしておく
	// サンプリング回数の取得
	DWORD sampCarry;
	return GetSamplingCount(sampCnt, &sampCarry);
}

//------------------------------------------
// サンプリングステータスを取得
// DWORD* status	ステータス状態
// DWORD* err		エラー状態
// 戻り値 復帰情報
//------------------------------------------
DWORD ContecCnt8mManager::GetSampStat(DWORD* status, DWORD* err)
{
#ifndef NO_HW		// 本番
	_ASSERT(CONTEC_CNT_NOID != m_hDrv);

	DWORD nRet = CntGetSamplingStatus(m_hDrv, status, err);
	if( 0 != nRet ) {
		m_errCode	= nRet;
		return CntGetSamplingStatusErr;
	}
#endif
	return 0;
}

//------------------------------------------
// テストパルスの内部／外部切替制御
// short int_ext	on_off 1:内部、0:外部
// 戻り値 復帰情報
//------------------------------------------
DWORD ContecCnt8mManager::TestPulse(short int_ext)
{
	long nRet = 0;
	if( ! m_bSmyu) {	
#ifndef NO_HW
		_ASSERT(CONTEC_CNT_NOID != m_hDrv);
	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
		nRet = CntTestPulse   (m_hDrv, int_ext, 1);
	#else									// WMD版
		nRet = CntSetTestPulse(m_hDrv, int_ext, 1, 0);
	#endif	
#endif
	}

	if( nRet != 0 ) {
		m_errCode	= nRet;	
		return CntTestPulseErr;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////
// ソケット

//------------------------------------------
// UDP SOCKET生成
// int no 生成No(0オリジン)
// const char* ip IPアドレス (例：192.9.200.255)
// WORD port ポートNo (例：8003)
// 戻り値 復帰情報
//------------------------------------------
BOOL ContecCnt8mManager::SetUdp(int no, const char* ip, WORD port)
{
	_ASSERT(0 <= no);
	_ASSERT(mc_nMaxUdp > no);


	WSADATA wsaData;
	WORD wVersionRequired = MAKEWORD(2, 2);
	WSAStartup(wVersionRequired, &wsaData);
	m_sock[no] = (int)socket(AF_INET, SOCK_DGRAM, 0);
	if (INVALID_SOCKET == m_sock[no]) return FALSE;
	m_addr[no].sin_family = AF_INET;
	m_addr[no].sin_addr.s_addr = inet_addr(ip);
    m_addr[no].sin_port = htons(port);
	return TRUE;
}



//////////////////////////////////////////////////////////////////////
// 一致出力


//------------------------------------------
// 制御出力信号のハードウェアイベント設定
// WORD ChNo		設定するチャネル番号 (0オリジン)
// short EventType	カウント一致のハードウェアイベントの種類 (CNT_OEVENT_GENOUT:汎用出力 CNT_OEVENT_MATCH0:カウント一致レジスタ0)
// short PulseWidth	ワンショットパルスのパルス幅の係数 (0:10us  1:100us  2:1ms  3:10ms  4:100ms)
// 戻り値 復帰情報
//------------------------------------------
DWORD ContecCnt8mManager::OutputHardwareEvent(WORD ChNo, short EventType, short PulseWidth)
{
	// 制御出力にパルス発生
	long nRet = 0;
	if( ! m_bSmyu) {	
#ifndef NO_HW
		_ASSERT(CONTEC_CNT_NOID != m_hDrv);
	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
		nRet = CntSetOutputSignalEvent(m_hDrv, ChNo, CNT_OEVENT_NEGATIVE, EventType, PulseWidth);
	#else									// WMD版
		nRet = CntSetOutputHardwareEvent(m_hDrv, ChNo, CNT_OEVENT_NEGATIVE, EventType, PulseWidth);
	#endif	
#endif
	}

	if( nRet != 0 ) {
		m_errCode	= nRet;	
		return CntSetOutputHardwareEventErr;
	}
	return 0;
}

//------------------------------------------
// カウント一致のハードウェアイベント設定
// WORD ChNo		設定するチャネル番号 (0オリジン)
// short RegisterNo	比較レジスタNo
// DWORD カウント一致のハードウェアイベントの種類
// 戻り値 復帰情報
//------------------------------------------
DWORD ContecCnt8mManager::CountMatchHardwareEvent(WORD ChNo, short RegisterNo, short eventType)
{
	// カウント一致のハードウェアイベント設定 (カウント値ゼロクリア )
	long nRet = 0;
	if( ! m_bSmyu) {	
#ifndef NO_HW
		_ASSERT(CONTEC_CNT_NOID != m_hDrv);
	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
		nRet = CntSetCountUpEvent(m_hDrv, ChNo, RegisterNo, eventType, 0);
	#else									// WMD版
		nRet = CntSetCountMatchHardwareEvent(m_hDrv, ChNo, RegisterNo, eventType, 0);
	#endif	
#endif
	}

	if( nRet != 0 ) {
		m_errCode	= nRet;	
		return CntSetCountMatchHardwareEventErr;
	}
	return 0;
}

//------------------------------------------
// カウント値をゼロクリアします。
// int nChIndex		設定するチャネル番号 (0オリジン)
// int ChNum		チャンネル数
// 戻り値 復帰情報
//------------------------------------------
DWORD ContecCnt8mManager::ResetChannel(int nChIndex, int ChNum)
{
	// カウント一致のハードウェアイベント設定 (カウント値ゼロクリア )
	long nRet = 0;
	if( ! m_bSmyu) {	
#ifndef NO_HW
		_ASSERT(CONTEC_CNT_NOID != m_hDrv);
	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
		nRet = CntResetChannel(m_hDrv, &m_nChannel[ nChIndex ], ChNum);
	#else									// WMD版
		nRet = CntZeroClearCount(m_hDrv, &m_nChannel[ nChIndex ], ChNum);
	#endif	
#endif
	}

	if( nRet != 0 ) {
		m_errCode	= nRet;	
		return CntResetChannelErr;
	}
	return 0;
}

//------------------------------------------
// カウント一致による通知の指定設定
// WORD ChNo		設定するチャネル番号 (0オリジン)
// short RegNo 比較レジスタのレジスタ番号 (基本的に0でOK)
// DWORD Count 比較レジスタに設定する比較値
// 戻り値 復帰情報
//------------------------------------------
DWORD ContecCnt8mManager::NotifyCountUp(WORD ChNo, short RegNo, long plsCnt)
{
	// カウント一致のハードウェアイベント設定 (カウント値ゼロクリア )
	long nRet = 0;
	if( ! m_bSmyu) {	
#ifndef NO_HW
		_ASSERT(CONTEC_CNT_NOID != m_hDrv);
	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
		nRet = CntNotifyCountUp(m_hDrv, ChNo, RegNo, plsCnt, 0, 0);
	#else									// WMD版
		nRet = CntNotifyCountUp(m_hDrv, ChNo, RegNo, plsCnt, 0);
	#endif	
#endif
	}

	if( nRet != 0 ) {
		m_errCode	= nRet;	
		return CntNotifyCountUpErr;
	}
	return 0;
}

//------------------------------------------
// 指定個数サンプリング完了の通知設定
// long Count	通知するサンプリング値
// 戻り値 復帰情報
//------------------------------------------
DWORD ContecCnt8mManager::NotifySamplingCount(long Count)
{
	// カウント一致のハードウェアイベント設定 (カウント値ゼロクリア )
	long nRet = 0;
	if( ! m_bSmyu) {	
#ifndef NO_HW
		_ASSERT(CONTEC_CNT_NOID != m_hDrv);
	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
		nRet = CntNotifySamplingCount(m_hDrv, Count, 0, 0);
	#else									// WMD版
		nRet = CntNotifySamplingCount(m_hDrv, Count, 0);
	#endif	
#endif
	}

	if( nRet != 0 ) {
		m_errCode	= nRet;	
		return CntNotifySamplingCountErr;
	}
	return 0;
}

//------------------------------------------
// CntNotifyCountUp関数で設定したカウント一致によりイベントが発生した場合に呼び出されるコールバック関数を登録
// void* CallBackProc コールバック関数のアドレスを指定します。
// void* param コールバックに渡すパラメータのアドレスを指定します。パラメータが必要ない場合は、NULLを指定してください。
// 戻り値 復帰情報
//------------------------------------------
DWORD ContecCnt8mManager::CountUpCallbackProc(void* CallBackProc, void* param)
{
	long nRet = 0;
	if( ! m_bSmyu) {	
#ifndef NO_HW
		_ASSERT(CONTEC_CNT_NOID != m_hDrv);
	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
		// 無し
	#else									// WMD版
		nRet = CntCountUpCallbackProc(m_hDrv, CallBackProc, param);
	#endif	
#endif
	}

	if( nRet != 0 ) {
		m_errCode	= nRet;	
		return CntCountUpCallbackProcErr;
	}
	return 0;
}

DWORD ContecCnt8mManager::SamplingCountCallbackProc(void* CallBackProc, void* param)
{
	long nRet = 0;
	if( ! m_bSmyu) {	
#ifndef NO_HW
		_ASSERT(CONTEC_CNT_NOID != m_hDrv);
	#ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
		// 無し
	#else									// WMD版
		nRet = CntSamplingCountCallbackProc(m_hDrv, CallBackProc, param);
	#endif	
#endif
	}

	if( nRet != 0 ) {
		m_errCode	= nRet;	
		return CntSamplingCountCallbackProcErr;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////
// シミュレーション

//------------------------------------------
// シミュレータモードの設定
// bool bMode True:シミュレーション時 false:通常
//------------------------------------------
bool ContecCnt8mManager::SetSmyuMode(bool bMode) 
{
	bool bRetc=true;
	//// シミュレーション時 
	if(bMode) {
		mem_connect(&m_hTblSmyu, (LPVOID *)&mtbl_pSmyu, m_sSmyuName);
		if(m_hTblSmyu == NULL) {
			m_hTblSmyu = NULL;
			mtbl_pSmyu = NULL; 
			bMode = false;
			bRetc = false;
			goto ending;
		}

	} else {
		if(NULL != mtbl_pSmyu) {
			mem_close(&m_hTblSmyu, (LPVOID *)&mtbl_pSmyu);
		}
		m_hTblSmyu = NULL;
		mtbl_pSmyu = NULL; 
	}

	//// フラグの変更
ending:
	m_bSmyu = bMode;
	return bRetc;
}
//------------------------------------------
// サンプリングバッファの初期化
//------------------------------------------
void ContecCnt8mManager::SmyuCntResetSampling()
{
	memset(mtbl_pSmyu, 0x00, sizeof(SMYU_TBL_PLG));
}
//------------------------------------------
// 最新のサンプリングカウント値 取得
// DWORD* sampCnt 最新のサンプリングカウント値
//------------------------------------------
DWORD ContecCnt8mManager::SmyuGetSamplingCount(DWORD* sampCnt) 
{
	*sampCnt = mtbl_pSmyu->nSamplingCount;
	return 0;
}

//------------------------------------------
// 最新のカウント値 取得
// WORD ChNum   読み込みたいチャンネル数
// short* ChNo   読み込みたいチャンネル番号の配列 (0オリジン)
// DWORD* count 読み込んだカウント値の配列
//------------------------------------------
DWORD ContecCnt8mManager::SmyuGetReadCount(WORD ChNum, short* ChNo, DWORD* count) 
{
	for(int ii=0; ii<ChNum; ii++) {
		count[ii] = mtbl_pSmyu->nCount[ ChNo[ii] ];
	}

	return 0;
}



#endif


