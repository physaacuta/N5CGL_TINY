#include "StdAfx.h"
#include "PlgControlManager.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// コンストラクタ
//------------------------------------------
PlgControlManager::PlgControlManager(void) :
ThreadManager("PlgCntMgr")
{
	//// INIファイル読み込み
	GetIniParam();

	//// 共有メモリ生成
	m_hTblSmyu = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SMYU_TBL_PLG), m_sSmyuName);
	mtbl_pSmyu =(SMYU_TBL_PLG*) MapViewOfFile(m_hTblSmyu, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if(m_hTblSmyu == NULL) {
		_ASSERT(FALSE);
	}
	memset(mtbl_pSmyu, 0x00, sizeof(SMYU_TBL_PLG));

#ifdef SEQ_PLG
	m_hTblSeq = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(TO_SEQMEM_TBL), m_sSeqName);
	mtbl_pSeq = (TO_SEQMEM_TBL*)MapViewOfFile(m_hTblSeq, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (m_hTblSeq == NULL) {
		_ASSERT(FALSE);
	}
	SetSeqMem_Clear(mtbl_pSeq);
#endif


	//// ハンドル生成
	m_evTimerFrame = CreateWaitableTimer(NULL, FALSE, NULL);
	m_evTimerCnt = CreateWaitableTimer(NULL, FALSE, NULL);
	for(int ii=0; ii<SMYU_PLG_MAX_CHANNEL; ii++) {
		m_evTimerSpeed[ii] = CreateWaitableTimer(NULL, FALSE, NULL);
	}
	m_evTimerHelcy = CreateWaitableTimer(NULL, FALSE, NULL);

#ifdef SEQ_PLG
	// 初期値代入
	m_dKenPlgDiff = 0;
	m_dKenPlgAdd = 0;
	m_dKenExsync = 0;
	m_dDspPlgAdd = 0;

	SetUdpSocket();

	SEQ_DM_RECV_BASE* typWk = (SEQ_DM_RECV_BASE*)&mtbl_pSeq->fb_dm[0];
	typWk->nPls = (WORD)(m_Setting.dPlsY[m_nIdPlgKen] * 10000.0);
	typWk->nPlsSub = (WORD)(m_Setting.dPlsY[m_nIdPlgSub] * 10000.0);
	typWk->nPlsDsp = (WORD)(m_Setting.dPlsY[m_nIdPlgDsp] * 10000.0);
#endif
}

//------------------------------------------
// デストラクタ
//------------------------------------------
PlgControlManager::~PlgControlManager(void)
{
	//// 共有メモリ開放
	UnmapViewOfFile(mtbl_pSmyu);
#ifndef SEQ_PLG
	CloseHandle(m_hTblSmyu);
#endif

#ifdef SEQ_PLG
	UnmapViewOfFile(mtbl_pSeq);
	CloseHandle(m_hTblSeq);
#endif


	//// ハンドル解放
	CloseHandle(m_evTimerFrame);
	CloseHandle(m_evTimerCnt);
	for(int ii=0; ii<SMYU_PLG_MAX_CHANNEL; ii++) {
		CloseHandle(m_evTimerSpeed[ii]);
	}
	CloseHandle(m_evTimerHelcy);

#ifdef SEQ_PLG
	ReSetUdpSocket();
#endif
}


//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int PlgControlManager::ThreadFirst()
{
	//// 定周期処理開始
	int ii_s = ( 1==m_Setting.nSamplingEnable, 2, 0 );		// 通常の疵検時は、0,1chは固定のため
	for(int ii=ii_s; ii<SMYU_PLG_MAX_CHANNEL; ii++) {
		SetTimer_SpeedCycle(ii, true);
	}

	//// イベント設定
	//HANDLE hArray[] = { m_evTimerFrame,
	//					m_evTimerSpeed[]
	//					};
	//SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);

	AddEventNum(1, &m_evTimerFrame);
#ifdef SEQ_PLG
	__int64 ts;
	ts= TIME_CNT_INTERVAL * -10000;
	SetWaitableTimer(m_evTimerCnt, (LARGE_INTEGER *)&ts, TIME_CNT_INTERVAL, NULL, NULL, FALSE);

	ts = TIME_HELCY_INTERVAL * -10000;
	SetWaitableTimer(m_evTimerHelcy, (LARGE_INTEGER *)&ts, TIME_CNT_INTERVAL, NULL, NULL, FALSE);
#endif
	AddEventNum(1, &m_evTimerCnt);
	AddEventNum(1, &m_evTimerHelcy);
	AddEventNum(SMYU_PLG_MAX_CHANNEL, m_evTimerSpeed);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int PlgControlManager::ThreadLast()
{
	// 定周期停止
//	SetTimer_FrameCycle(false);
	for(int ii=0; ii<SMYU_PLG_MAX_CHANNEL; ii++) {
		SetTimer_SpeedCycle(ii, false);
	}
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void PlgControlManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_TIMER_FRAME = 0,				// フレーム出力定周期実行イベント
			EV_TIMER_CNT,					// ライブカウンタ
			EV_TIMER_HELCY,					// ヘルシー送信
			EV_TIMER_SPEED					// 通板距離関係
	};

	////// シグナル条件分け
//-----------------------------------------------------------------------------------------------
	// フレーム出力定周期実行イベント
	if (EV_TIMER_FRAME == nEventNo) {
		SetMem_Sampling();
//-----------------------------------------------------------------------------------------------
	// ライブカウンタイベント
	} else if (EV_TIMER_CNT == nEventNo) {
		SetMem_Cnt();
//-----------------------------------------------------------------------------------------------
	// ヘルシー送信
	} else if (EV_TIMER_HELCY == nEventNo) {
		SendHelcy();

//-----------------------------------------------------------------------------------------------
	// 通板距離関係
	} else {
		SetMem_Speed(nEventNo - EV_TIMER_SPEED);
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 共有メモリ 操作

//------------------------------------------
// サンプリングクロック用
//------------------------------------------
void PlgControlManager::SetMem_Sampling()
{
	//// 今回のサンプリング位置を取得
	DWORD nCnt   = (m_Setting.nFrameSizeY/SMYU_PLG_BUNSYU);

	DWORD nSamp	 = mtbl_pSmyu->nSamplingCount;				// 今回の先頭
	DWORD nSampE = nSamp + nCnt;	

	DWORD nIndex = nSamp % (SMYU_PLG_MAX_SAMPBUFNUM*m_Setting.nFrameSizeY);
	DWORD nIndexE= nIndex + nCnt;							// 今回の最後
	long nPre = nIndex -1;									// 前回の最後
	if( 0 > nPre ) nPre = SMYU_PLG_MAX_SAMPBUFNUM*m_Setting.nFrameSizeY - 1;

	//// ベース値を決定
	// 前回の最後の値
	DWORD nPlgCnt  = mtbl_pSmyu->nSampBuf[nPre][0];
	DWORD nTimeCnt = mtbl_pSmyu->nSampBuf[nPre][1];

//	LOG(em_MSG), "[%s] ｻﾝﾌﾟﾘﾝｸﾞ %d→%d ch0=%d, ch1=%d", my_sThreadName, nSamp, nSampE , nPlgCnt, nTimeCnt);


	// 10nsecテストパルスは、
	//	1pixel辺りの時間カウントを算出
	//		[mm/sec]    = [mpm] / 60 * 1000.0
	//		[sec/pixel] = [mm/pixel] / [mm/sec]
	//		[sec] → 10nsec 単位に変換
	//	これが、1ライン辺りのテストパルス数となるはず	
	double dWk = m_SaveData.nSpeed[0] / 60.0 * 1000.0;	// [mm/sec] 
	dWk = m_Setting.dResY[0] / dWk;						// [sec/pixel] 
	DWORD nTimeAdd = (DWORD)(dWk * 100000);

	//// サンプリングバッファにデータセット
	for(DWORD ii=nIndex; ii<nIndexE; ii++) {
		nPlgCnt++;							// PLG値は、前回の1加算でよい
		nTimeCnt += nTimeAdd;				// 時間カウント値は、1ライン辺りの10nsec数をセット
		mtbl_pSmyu->nSampBuf[ii][0] = nPlgCnt;	
		mtbl_pSmyu->nSampBuf[ii][1] = nTimeCnt;
	}

	//// 全部セットし終わったあとに、最新レコード位置を更新
	mtbl_pSmyu->nCount[0] = nPlgCnt;
	mtbl_pSmyu->nCount[1] = nTimeCnt;
	mtbl_pSmyu->nSamplingCount = nSampE;

	mtbl_pSmyu->dLen[0] += (nCnt * m_Setting.dResY[0]);

//	LOG(em_MSG), "[%s] ch0=%d, ch1=%d", my_sThreadName, nPlgCnt, nTimeCnt );

#ifdef SEQ_PLG
	long tm = m_SaveData.nFrameCycle / SMYU_PLG_BUNSYU;
	dWk = (tm / 1000.0) / dWk;				// 30ms周期のため
	SEQ_DM_RECV_BASE* typWk = (SEQ_DM_RECV_BASE*)&mtbl_pSeq->fb_dm[0];
	m_dKenPlgDiff += dWk * m_Setting.dResY[0] / m_Setting.dPlsY[0];
	typWk->ken.nPlgDiff = (DWORD)m_dKenPlgDiff;				// 検査用 PLG 差分
	typWk->ken.nTimeDiff += tm * 100;						// 検査用 時間 差分 [10μs]
	m_dKenExsync += dWk;
	typWk->ken.nLvalAdd = (DWORD)m_dKenExsync;				// 検査用 ExSync　積算値
	m_dKenPlgAdd += dWk* m_Setting.dResY[0] / m_Setting.dPlsY[0];
	typWk->ken.nPlgAdd = (DWORD)m_dKenPlgAdd;				// 検査用 PLG 積算
	typWk->ken.nTimeAdd += tm * 100;						// 検査用 時間 積算 [10μs]
															// フレームが1増えた
	typWk->nLength += (DWORD)(nCnt * m_Setting.dResY[0]);

	if (m_Setting.nFrameSizeY <= typWk->ken.nPlgDiff* m_Setting.dPlsY[0] / m_Setting.dResY[0]) {
		SendUpd();
		typWk->ken.nFrameNo += 1;
		m_dKenPlgDiff -= m_Setting.nFrameSizeY * m_Setting.dResY[0] / m_Setting.dPlsY[0];
		typWk->ken.nPlgDiff = (DWORD)m_dKenPlgDiff;
		typWk->ken.nTimeDiff = 0;
	}
	//m_dDspPlgAdd += dWk;
	//typWk->dsp.nPlgAdd = (DWORD)m_dDspPlgAdd;				// 検査用 PLG 積算
#endif
}

//------------------------------------------
// サンプリングクロック用
//------------------------------------------
void PlgControlManager::SetMem_Cnt()
{
#ifdef SEQ_PLG
	SEQ_DM_RECV_BASE* typWk = (SEQ_DM_RECV_BASE*)&mtbl_pSeq->fb_dm[0];
	typWk->nHBCnt += 1;
	if(9999 < typWk->nHBCnt)typWk->nHBCnt = 0;
#endif
}

//------------------------------------------
// 通板距離用
// int nIndex 配列位置 (0オリジン)
//------------------------------------------
void PlgControlManager::SetMem_Speed(int nIndex)
{
	//		[mm/sec]    = [mpm] / 60 * 1000.0
	//		[pixel/sec] = [mm/sec] / [mm/pixel]
	double dWk = m_SaveData.nSpeed[nIndex] / 60.0 * 1000.0;	// [mm/sec] 
	dWk = dWk / m_Setting.dResY[nIndex];					// [pixel/sec] 
	dWk = dWk / (1000.0/TIME_SPEED_INTERVAL);				// 30ms周期のため

	mtbl_pSmyu->nCount[nIndex] += (DWORD)dWk;
	mtbl_pSmyu->dLen[nIndex]   +=  (DWORD)dWk * m_Setting.dResY[nIndex];

#ifdef SEQ_PLG
	SEQ_DM_RECV_BASE* typWk = (SEQ_DM_RECV_BASE*)&mtbl_pSeq->fb_dm[0];
	if (nIndex == m_nIdPlgKen) {
		typWk->nLength += (DWORD)(dWk * m_Setting.dResY[nIndex]);
		m_dKenPlgDiff += dWk* m_Setting.dResY[0] / m_Setting.dPlsY[0];
		typWk->ken.nPlgDiff = (DWORD)m_dKenPlgDiff;			// 検査用 PLG 差分
		typWk->ken.nTimeDiff += TIME_SPEED_INTERVAL * 100;		// 検査用 時間 差分 [10μs]
		m_dKenExsync += dWk;
		typWk->ken.nLvalAdd = (DWORD)m_dKenExsync;				// 検査用 ExSync　積算値
		m_dKenPlgAdd += dWk* m_Setting.dResY[0] / m_Setting.dPlsY[0];
		typWk->ken.nPlgAdd = (DWORD)m_dKenPlgAdd;				// 検査用 PLG 積算
		typWk->ken.nTimeAdd += TIME_SPEED_INTERVAL * 100;		// 検査用 時間 積算 [10μs]
																// フレームが1増えた

		if (m_Setting.nFrameSizeY <= typWk->ken.nPlgDiff* m_Setting.dPlsY[0] / m_Setting.dResY[0]) {
			SendUpd();
			typWk->ken.nFrameNo += 1;
			m_dKenPlgDiff -= m_Setting.nFrameSizeY * m_Setting.dResY[0] / m_Setting.dPlsY[0];
			typWk->ken.nPlgDiff = (DWORD)m_dKenPlgDiff;
			typWk->ken.nTimeDiff = 0;
		}
	}
	if (nIndex == m_nIdPlgDsp) {
		typWk->nLengthDsp += (DWORD)(dWk * m_Setting.dResY[nIndex]);
		m_dDspPlgAdd += dWk* m_Setting.dResY[0] / m_Setting.dPlsY[0];
		typWk->dsp.nPlgAdd = (DWORD)m_dDspPlgAdd;				// 検査用 PLG 積算
	}
	if (nIndex == m_nIdPlgSub) {
		typWk->nLengthSub += (DWORD)(dWk * m_Setting.dResY[nIndex]);
	}
#endif
}

//------------------------------------------
// シーケンサ共有メモリクリア
// int nIndex 配列位置 (0オリジン)
//------------------------------------------
void PlgControlManager::SetSeqMem_Clear(TO_SEQMEM_TBL* tbl)
{
	SEQ_DM_RECV_BASE* typWk = (SEQ_DM_RECV_BASE*)&tbl->fb_dm[0];
	memset(&typWk->ken, 0x00, sizeof(typWk->ken));
	memset(&typWk->dsp, 0x00, sizeof(typWk->dsp));
	typWk->nLength = 0;
	typWk->nLengthSub = 0;
	typWk->nLengthDsp = 0;
	typWk->nWpdOnLength = 0;
	typWk->nWpdOnLengthSub = 0;
	typWk->nWpdOnLengthDsp = 0;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 小物

//------------------------------------------
// シミュレータ開始
//------------------------------------------
void PlgControlManager::SmyuStart()
{
	//// 初期化
	memset(mtbl_pSmyu, 0x00, sizeof(SMYU_TBL_PLG));
}
/*
//------------------------------------------
// フレーム出力定周期タイマー 制御
// bool bMode true:開始 false:停止
//------------------------------------------
void PlgControlManager::SetTimer_FrameCycle(bool bMode)
{
	//// 事前チェック
	if( 0 >= m_SaveData.nFrameCycle ) bMode=false;

	//// 処理
	if(bMode) {
		//// 事前準備
		__int64 ts = (m_SaveData.nFrameCycle /SMYU_PLG_BUNSYU) * -10000; 
		SetWaitableTimer(m_evTimerFrame, (LARGE_INTEGER *)&ts, m_SaveData.nFrameCycle/SMYU_PLG_BUNSYU, NULL, NULL, FALSE);

	} else {
		CancelWaitableTimer(m_evTimerFrame);

	}
}
*/
//------------------------------------------
// ライン速度出力定周期タイマー 制御
// int nIndex 配列位置 (0オリジン)
// bool bMode true:開始 false:停止
//------------------------------------------
void PlgControlManager::SetTimer_SpeedCycle(int nIndex, bool bMode)
{
	if(1==m_Setting.nSamplingEnable && 0 == nIndex) {
		//======================================================
		//// 通常の疵検時
		if( 0 >= m_SaveData.nFrameCycle ) bMode=false;

		//// 処理
		if(bMode) {
			//// 事前準備
			__int64 ts = (m_SaveData.nFrameCycle /SMYU_PLG_BUNSYU) * -10000; 
			SetWaitableTimer(m_evTimerFrame, (LARGE_INTEGER *)&ts, m_SaveData.nFrameCycle/SMYU_PLG_BUNSYU, NULL, NULL, FALSE);
		} else {
			CancelWaitableTimer(m_evTimerFrame);
		}

	} else {
		//======================================================
		//// その他
		if( 0 >= m_SaveData.nSpeed[nIndex] ) bMode=false;

		//// 処理
		if(bMode) {
			//// 事前準備
			__int64 ts = TIME_SPEED_INTERVAL * -10000; 	// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
			SetWaitableTimer(m_evTimerSpeed[nIndex], (LARGE_INTEGER *)&ts, TIME_SPEED_INTERVAL, NULL, NULL, FALSE);

		} else {
			CancelWaitableTimer(m_evTimerSpeed[nIndex]);
		}
	}


	//// 画面に結果反映
	int chkSpeedSelect[SMYU_PLG_MAX_CHANNEL]= { IDC_PLG_SPEED_S0,IDC_PLG_SPEED_S1,IDC_PLG_SPEED_S2,IDC_PLG_SPEED_S3,	IDC_PLG_SPEED_S4,IDC_PLG_SPEED_S5,IDC_PLG_SPEED_S6,IDC_PLG_SPEED_S7 };
	AfxGetApp()->GetMainWnd()->CheckDlgButton(chkSpeedSelect[nIndex], bMode);//(bMode?1:0)); 
}

//------------------------------------------
// 検査用WPD信号ON
//------------------------------------------
void PlgControlManager::Set_WpdKen(void)
{
#ifdef SEQ_PLG
	SEQ_DM_RECV_BASE* typWk = (SEQ_DM_RECV_BASE*)&mtbl_pSeq->fb_dm[0];
	typWk->ken.nWpdOnFrameNo = typWk->ken.nFrameNo;
	typWk->ken.nWpdOnLvalAdd = typWk->ken.nLvalAdd;
	typWk->ken.nWpdOnYCnt = (DWORD)(typWk->ken.nPlgDiff* m_Setting.dPlsY[0] / m_Setting.dResY[0]);

	typWk->nWpdOnLength = typWk->nLength;
	typWk->nLength = 0;
#endif
}

//------------------------------------------
// 検査比較用WPD信号ON
//------------------------------------------
void PlgControlManager::Set_WpdSub(void)
{
#ifdef SEQ_PLG
	SEQ_DM_RECV_BASE* typWk = (SEQ_DM_RECV_BASE*)&mtbl_pSeq->fb_dm[0];
	typWk->nWpdOnLengthSub = typWk->nLengthSub;
	typWk->nLengthSub = 0;
#endif
}

//------------------------------------------
// 検査表示用WPD信号ON
//------------------------------------------
void PlgControlManager::Set_WpdDsp(void)
{
#ifdef SEQ_PLG
	SEQ_DM_RECV_BASE* typWk = (SEQ_DM_RECV_BASE*)&mtbl_pSeq->fb_dm[0];
	typWk->dsp.nWpdOnPlgAdd = typWk->dsp.nPlgAdd;

	typWk->nWpdOnLengthDsp = typWk->nLengthDsp;
	typWk->nLengthDsp = 0;
#endif
}

//------------------------------------------
// リセット信号ON
//------------------------------------------
void PlgControlManager::Set_ResetSignal(void)
{
#ifdef SEQ_PLG
	SEQ_DM_RECV_BASE* typWk = (SEQ_DM_RECV_BASE*)&mtbl_pSeq->fb_dm[0];

	memset(&typWk->ken, 0x00, sizeof(typWk->ken));
	memset(&typWk->dsp, 0x00, sizeof(typWk->dsp));

	typWk->nWpdOnLength = 0;
	typWk->nWpdOnLengthSub = 0;
	typWk->nWpdOnLengthDsp = 0;

	typWk->nLength = 0;
	typWk->nLengthSub = 0;
	typWk->nLengthDsp = 0;
	typWk->nWpdOnLength = 0;
	typWk->nWpdOnLengthSub = 0;
	typWk->nWpdOnLengthDsp = 0;

	m_dKenPlgDiff = 0;
	m_dKenPlgAdd = 0;
	m_dKenExsync = 0;
	m_dDspPlgAdd = 0;
#endif
}

//------------------------------------------
// Iniファイル取得
//------------------------------------------
void PlgControlManager::GetIniParam()
{
	CString	sWk;
	int		nWk;
	char	cWk[1024];

	//// ODBC接続クラス インスタンス生成
	//OdbcBase	clsDb;
	//if( ! clsDb.Connect(TASKINI_NAME)) {
	//	_ASSERT(FALSE);
	//	return;
	//}

	memset(&m_Setting, 0x00, sizeof(m_Setting));
	memset(&m_SaveData,0x00, sizeof(m_SaveData));


	//================================================
	// 基本データ
	GetPrivateProfileString(GOT_CONTROL, "TBL_NAME_PLG", "", cWk, sizeof(cWk), TASKINI_NAME);	m_sSmyuName = cWk;
	GetPrivateProfileString(GOT_CONTROL, "TBL_NAME_SEQ", TBL_TO_SEQMEM_NAME, cWk, sizeof(cWk), TASKINI_NAME);	m_sSeqName = cWk;
	m_nIdPlgKen = GetPrivateProfileInt(GOT_CONTROL, "INDEX_PLG_KEN", 0, TASKINI_NAME);
	m_nIdPlgSub = GetPrivateProfileInt(GOT_CONTROL, "INDEX_PLG_SUB", 1, TASKINI_NAME);
	m_nIdPlgDsp = GetPrivateProfileInt(GOT_CONTROL, "INDEX_PLG_DSP", 2, TASKINI_NAME);
	//================================================
	// 設定データ

	// サンプリング有効 有無
	m_Setting.nSamplingEnable = GetPrivateProfileInt(GOT_CONTROL, "SAMPLING_ENABLE", 0, TASKINI_NAME);
	
	
	//if(1==m_Setting.nSamplingEnable) {
		// 縦フレームサイズ
		GetPrivateProfileString(GOT_CONTROL, "FRAMESIZE_Y_SQL", "", cWk, sizeof(cWk), TASKINI_NAME);
		if(0!=strlen(cWk)) {
			if( ! OdbcBase::GetSelectKey_Static(cWk, &m_Setting.nFrameSizeY, TASKINI_NAME)) {
				_ASSERT(FALSE);
			}
		} else {
			m_Setting.nFrameSizeY = GetPrivateProfileInt(GOT_CONTROL, "FRAMESIZE_Y_VAL", 0, TASKINI_NAME);
		}
	//}

	// 有効Ch数
	m_Setting.nChNum = GetPrivateProfileInt(GOT_CONTROL, "PLG_CH_NUM", 0, TASKINI_NAME);
	
	// 分解能
	for(int ii=0; ii<m_Setting.nChNum; ii++) {
		sWk.Format("PLG_RES_SQL_%d", ii); 
		GetPrivateProfileString(GOT_CONTROL, sWk, "", cWk, sizeof(cWk), TASKINI_NAME);
		if(0!=strlen(cWk)) {
			if( ! OdbcBase::GetSelectKey_Static(cWk, &m_Setting.dResY[ii], TASKINI_NAME)) {
				_ASSERT(FALSE);
			}
		} else {
			sWk.Format("PLG_RES_VAL_%d", ii); 
			GetPrivateProfileString(GOT_CONTROL, sWk, "", cWk, sizeof(cWk), TASKINI_NAME);
			m_Setting.dResY[ii] = atof(cWk);
		}
	}

	// パルスレート
	for(int ii=0; ii<m_Setting.nChNum; ii++) {
		sWk.Format("PLG_PLS_SQL_%d", ii); 
		GetPrivateProfileString(GOT_CONTROL, sWk, "", cWk, sizeof(cWk), TASKINI_NAME);
		if(0!=strlen(cWk)) {
			if( ! OdbcBase::GetSelectKey_Static(cWk, &m_Setting.dPlsY[ii], TASKINI_NAME)) {
				_ASSERT(FALSE);
			}
		} else {
			sWk.Format("PLG_PLS_VAL_%d", ii); 
			GetPrivateProfileString(GOT_CONTROL, sWk, "", cWk, sizeof(cWk), TASKINI_NAME);
			m_Setting.dPlsY[ii] = atof(cWk);
		}
	}

	//================================================
	// 画面用データ
	
	CWnd*   cwnd		= AfxGetApp()->GetMainWnd();	// 使いやすいようにエイリアス

	// フレーム出力周期 (初期値) [ms]
	if(1==m_Setting.nSamplingEnable) {
		nWk = GetPrivateProfileInt(GOT_CONTROL, "INI_FRAME_OUTPUT_INTERVAL", 0, TASKINI_NAME);
		//cwnd->SetDlgItemInt(IDC_PLG_FRAMECYCLE_D, nWk, TRUE);
		m_SaveData.nFrameCycle = nWk;
	} else {
		cwnd->GetDlgItem(IDC_PLG_FRAMECYCLE_D)->EnableWindow(false); 
		cwnd->GetDlgItem(IDC_PLG_FRAMECYCLE_W)->EnableWindow(false); 
		cwnd->GetDlgItem(IDC_PLG_FRAMECYCLE_C)->EnableWindow(false); 
		//cwnd->GetDlgItem(IDC_PLG_FRAMECYCLE_S)->EnableWindow(false); 
	}

	// ライン速度
	int txtSpeedDsp[SMYU_PLG_MAX_CHANNEL]	= { IDC_PLG_SPEED_D0,IDC_PLG_SPEED_D1,IDC_PLG_SPEED_D2,IDC_PLG_SPEED_D3,	IDC_PLG_SPEED_D4,IDC_PLG_SPEED_D5,IDC_PLG_SPEED_D6,IDC_PLG_SPEED_D7 };
	int txtSpeedWrite[SMYU_PLG_MAX_CHANNEL]	= { IDC_PLG_SPEED_W0,IDC_PLG_SPEED_W1,IDC_PLG_SPEED_W2,IDC_PLG_SPEED_W3,	IDC_PLG_SPEED_W4,IDC_PLG_SPEED_W5,IDC_PLG_SPEED_W6,IDC_PLG_SPEED_W7 };
	int btnSpeedChange[SMYU_PLG_MAX_CHANNEL]= { IDC_PLG_SPEED_C0,IDC_PLG_SPEED_C1,IDC_PLG_SPEED_C2,IDC_PLG_SPEED_C3,	IDC_PLG_SPEED_C4,IDC_PLG_SPEED_C5,IDC_PLG_SPEED_C6,IDC_PLG_SPEED_C7 };
	int chkSpeedSelect[SMYU_PLG_MAX_CHANNEL]= { IDC_PLG_SPEED_S0,IDC_PLG_SPEED_S1,IDC_PLG_SPEED_S2,IDC_PLG_SPEED_S3,	IDC_PLG_SPEED_S4,IDC_PLG_SPEED_S5,IDC_PLG_SPEED_S6,IDC_PLG_SPEED_S7 };

	if(1==m_Setting.nSamplingEnable) {
		//cwnd->GetDlgItem(chkSpeedSelect[0])->EnableWindow(false); 

		cwnd->GetDlgItem(txtSpeedDsp[1])->EnableWindow(false); 
		cwnd->GetDlgItem(txtSpeedWrite[1])->EnableWindow(false); 
		cwnd->GetDlgItem(btnSpeedChange[1])->EnableWindow(false); 
		cwnd->GetDlgItem(chkSpeedSelect[1])->EnableWindow(false); 
	}

	for(int ii=0; ii<m_Setting.nChNum; ii++) {
		sWk.Format("INI_PLG_SPEED_%d", ii); 
		nWk = GetPrivateProfileInt(GOT_CONTROL, sWk, 0, TASKINI_NAME);
		//cwnd->SetDlgItemInt(txtSpeedDsp[ii], nWk, TRUE);
		m_SaveData.nSpeed[ii] = nWk;
	}
	for(int ii=m_Setting.nChNum; ii<SMYU_PLG_MAX_CHANNEL; ii++) {
		cwnd->GetDlgItem(txtSpeedDsp[ii])->EnableWindow(false); 
		cwnd->GetDlgItem(txtSpeedWrite[ii])->EnableWindow(false); 
		cwnd->GetDlgItem(btnSpeedChange[ii])->EnableWindow(false); 
		cwnd->GetDlgItem(chkSpeedSelect[ii])->EnableWindow(false); 
	}


	// 周期変更
	if(1==m_Setting.nSamplingEnable) {
		double dWk =  m_SaveData.nSpeed[0] / 60.0 * 1000.0;
		dWk  = dWk / (m_Setting.dResY[0]*m_Setting.nFrameSizeY);
		nWk = (int)(1.0 / dWk * 1000.0);
		m_SaveData.nFrameCycle = nWk;
	}

#ifdef SEQ_PLG
	GetPrivateProfileString(GOT_CONTROL, "UDP_IP", "192.9.200.255", cWk, sizeof(cWk), TASKINI_NAME);		m_SeqUdpIp = cWk;
	m_SeqUdpPort = GetPrivateProfileInt(GOT_CONTROL, "UDP_PORT", SEQ_UDP_PORT, TASKINI_NAME);
#endif

}

//------------------------------------------
// UDP SOCKET生成
//------------------------------------------
BOOL PlgControlManager::SetUdpSocket()
{
	// 初期化
	m_SeqUdpsock = INVALID_SOCKET;
	memset(&m_SeqUdpaddr, 0x00, sizeof(m_SeqUdpaddr));

	// ソケット生成
	WSADATA wsaData;
	WORD wVersionRequired = MAKEWORD(2, 2);
	WSAStartup(wVersionRequired, &wsaData);
	m_SeqUdpsock = socket(AF_INET, SOCK_DGRAM, 0);
	if (INVALID_SOCKET == m_SeqUdpsock) return FALSE;
	m_SeqUdpaddr.sin_family = AF_INET;
	m_SeqUdpaddr.sin_addr.s_addr = inet_addr(m_SeqUdpIp);
	m_SeqUdpaddr.sin_port = htons(m_SeqUdpPort);
	return TRUE;
}

//------------------------------------------
//// UDP SOCKET破棄
//------------------------------------------
void PlgControlManager::ReSetUdpSocket()
{
	if (INVALID_SOCKET != m_SeqUdpsock) closesocket(m_SeqUdpsock);
	m_SeqUdpsock = INVALID_SOCKET;
}

//------------------------------------------
// UDP送信
//------------------------------------------
void PlgControlManager::SendUpd(void)
{
#ifdef SEQ_PLG
	// 定周期タイマーリセット
	CancelWaitableTimer(m_evTimerHelcy);

	SEQ_DM_RECV_BASE* typWk = (SEQ_DM_RECV_BASE*)&mtbl_pSeq->fb_dm[0];
	SEQ_TYPE_FRAME *p = new SEQ_TYPE_FRAME;
	memset(p, 0x00, sizeof(SEQ_TYPE_FRAME));
	p->head.nTC = SEQ_TC_FRAME;
	p->head.nSeqLen = sizeof(SEQ_TYPE_DATA_FRAME);
	p->data.nFrameNo = typWk->ken.nFrameNo;
	p->data.nPlgDiff = typWk->ken.nPlgDiff;
	p->data.nTimeDiff = typWk->ken.nTimeDiff;

	p->data.nExSyncAdd = typWk->ken.nLvalAdd;
	p->data.nPlgAdd = typWk->ken.nPlgAdd;
	p->data.nTimeAdd = typWk->ken.nTimeAdd;

	p->data.nWpdFramePos = typWk->ken.nWpdOnYCnt;
	sendto(m_SeqUdpsock, (char*)p, sizeof(SEQ_TYPE_DATA_FRAME), 0, (sockaddr*)&m_SeqUdpaddr, sizeof(sockaddr_in));
	delete p;

	// 定周期タイマー再セット
	__int64 ts = TIME_HELCY_INTERVAL * -10000;
	SetWaitableTimer(m_evTimerHelcy, (LARGE_INTEGER *)&ts, TIME_CNT_INTERVAL, NULL, NULL, FALSE);
#endif

}
//------------------------------------------
// ヘルシー送信
//------------------------------------------
void PlgControlManager::SendHelcy(void)
{
#ifdef SEQ_PLG
	SEQ_DM_RECV_BASE* typWk = (SEQ_DM_RECV_BASE*)&mtbl_pSeq->fb_dm[0];
	SEQ_TYPE_HEAD *p = new SEQ_TYPE_HEAD;
	memset(p, 0x00, sizeof(SEQ_TYPE_HEAD));
	p->nTC = SEQ_TC_HELCY;
	p->nSeqLen = sizeof(SEQ_TYPE_HEAD);
	sendto(m_SeqUdpsock, (char*)p, sizeof(SEQ_TYPE_HEAD), 0, (sockaddr*)&m_SeqUdpaddr, sizeof(sockaddr_in));
	delete p;
#endif

}
//------------------------------------------
// シーケンサ共有メモリクリア
//------------------------------------------
void PlgControlManager::Clear_SeqMem(void)
{
#ifdef SEQ_PLG
	SetSeqMem_Clear(mtbl_pSeq);
#endif
}
