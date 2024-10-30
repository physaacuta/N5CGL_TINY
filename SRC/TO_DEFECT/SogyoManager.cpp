#include "StdAfx.h"
#include "SogyoManager.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// コンストラクタ
// LPCSTR cSession iniファイルセッション
//------------------------------------------
SogyoManager::SogyoManager(LPCSTR cSession) :
m_cSession(cSession),
mcls_pStatus(NULL),
ThreadManager("SogyolMgr")
{
	// 初期化
	m_pcNow = m_pcPre = 0;
	m_nNowSampCnt = m_nPreSampCnt = 0;
	m_nMpm = 0;
#ifdef DSP_SOGYO_CHECK
	m_pcNowDsp = m_pcPreDsp = 0;
	m_nNowSampCntDsp = m_nPreSampCntDsp = 0;
	m_nMpmDsp = 0;
#endif


	// 定周期チェックタイマー 生成
	m_evCheckTimer = CreateWaitableTimer(NULL, FALSE, NULL);

	// クリティカルセクション
	InitializeCriticalSection(&m_csLock);
}

//------------------------------------------
// デストラクタ
//------------------------------------------
SogyoManager::~SogyoManager(void)
{
	// 定周期チェックタイマー 開放
	CancelWaitableTimer(m_evCheckTimer);
	CloseHandle(m_evCheckTimer);

	// クリティカルセクション
	DeleteCriticalSection(&m_csLock);
}


//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int SogyoManager::ThreadFirst()
{
	//// イベント設定
	HANDLE hArray[] = { m_evCheckTimer
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int SogyoManager::ThreadLast()
{
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void SogyoManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_CHECK_TIMER = 0,				// 定周期監視タイマー
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_CHECK_TIMER:					// 定周期監視タイマー
		// 定周期チェック
		CheckSampCntDiff();
#ifdef DSP_SOGYO_CHECK
		CheckSampCntDiffDsp();
#endif
		break;

//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}

}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 外部操作関数

//------------------------------------------
// 動的 操業判定条件を読み込み
//   ※ 現調中に設定変更で毎回探傷をとめるのがイヤだったので暫定としてこのようにした
//------------------------------------------
void SogyoManager::GetIniParam()
{
	SogyoSetting.InfUpToDown_Slow.nLimitSpeed = GetPrivateProfileInt(m_cSession, "UpToDown_SLOW_MPM", 0, TASKINI_NAME);
	SogyoSetting.InfUpToDown_Slow.nCheckCnt   = GetPrivateProfileInt(m_cSession, "UpToDown_SLOW_CNT", 0, TASKINI_NAME);
	SogyoSetting.InfUpToDown_Stop.nLimitSpeed = GetPrivateProfileInt(m_cSession, "UpToDown_STOP_MPM", 0, TASKINI_NAME);
	SogyoSetting.InfUpToDown_Stop.nCheckCnt   = GetPrivateProfileInt(m_cSession, "UpToDown_STOP_CNT", 0, TASKINI_NAME);
	SogyoSetting.InfDownToUp_Slow.nLimitSpeed = GetPrivateProfileInt(m_cSession, "DownToUp_SLOW_MPM", 0, TASKINI_NAME);
	SogyoSetting.InfDownToUp_Slow.nCheckCnt   = GetPrivateProfileInt(m_cSession, "DownToUp_SLOW_CNT", 0, TASKINI_NAME);
	SogyoSetting.InfDownToUp_Run.nLimitSpeed  = GetPrivateProfileInt(m_cSession, "DownToUp_RUN_MPM", 0, TASKINI_NAME);
	SogyoSetting.InfDownToUp_Run.nCheckCnt    = GetPrivateProfileInt(m_cSession, "DownToUp_RUN_CNT", 0, TASKINI_NAME);
	// この条件が有効無効かチェック
	for(int ii=0; ii<4; ii++) {
		if(0 <= SogyoSetting.array[ii].nLimitSpeed && 0 < SogyoSetting.array[ii].nCheckCnt) SogyoSetting.array[ii].bUmu = true;
	}
#ifdef DSP_SOGYO_CHECK
	// 表示用セット（とりあえず検査用と同じ閾値）
	SogyoSettingDsp.InfUpToDown_Slow.nLimitSpeed = GetPrivateProfileInt(m_cSession, "UpToDown_SLOW_MPM", 0, TASKINI_NAME);
	SogyoSettingDsp.InfUpToDown_Slow.nCheckCnt   = GetPrivateProfileInt(m_cSession, "UpToDown_SLOW_CNT", 0, TASKINI_NAME);
	SogyoSettingDsp.InfUpToDown_Stop.nLimitSpeed = GetPrivateProfileInt(m_cSession, "UpToDown_STOP_MPM", 0, TASKINI_NAME);
	SogyoSettingDsp.InfUpToDown_Stop.nCheckCnt   = GetPrivateProfileInt(m_cSession, "UpToDown_STOP_CNT", 0, TASKINI_NAME);
	SogyoSettingDsp.InfDownToUp_Slow.nLimitSpeed = GetPrivateProfileInt(m_cSession, "DownToUp_SLOW_MPM", 0, TASKINI_NAME);
	SogyoSettingDsp.InfDownToUp_Slow.nCheckCnt   = GetPrivateProfileInt(m_cSession, "DownToUp_SLOW_CNT", 0, TASKINI_NAME);
	SogyoSettingDsp.InfDownToUp_Run.nLimitSpeed  = GetPrivateProfileInt(m_cSession, "DownToUp_RUN_MPM", 0, TASKINI_NAME);
	SogyoSettingDsp.InfDownToUp_Run.nCheckCnt    = GetPrivateProfileInt(m_cSession, "DownToUp_RUN_CNT", 0, TASKINI_NAME);
	// この条件が有効無効かチェック
	for(int ii=0; ii<4; ii++) {
		if(0 <= SogyoSettingDsp.array[ii].nLimitSpeed && 0 < SogyoSettingDsp.array[ii].nCheckCnt) SogyoSettingDsp.array[ii].bUmu = true;
	}
#endif
}

//------------------------------------------
// 定周期チェックタイマー 開始
// DWORD sampCnt 初回値 (-1なら値を変えない)
//------------------------------------------
void SogyoManager::CheckStart(DWORD sampCnt)
{
	AutoLock autolock(&m_csLock);

	// 時間
	QueryPerformanceFrequency((LARGE_INTEGER *)&m_pf);
	
	// 初回値セット
	if( -1 != sampCnt ) m_nPreSampCnt = sampCnt;
	QueryPerformanceCounter((LARGE_INTEGER *)&m_pcPre);

	// iniファイル取得
	memset(&SogyoSetting, 0x00, sizeof(SogyoSetting));

#ifdef DSP_SOGYO_CHECK
	// 初回値セット
	if (-1 != sampCnt) m_nPreSampCntDsp = sampCnt;
	QueryPerformanceCounter((LARGE_INTEGER *)&m_pcPreDsp);

	// iniファイル取得
	memset(&SogyoSettingDsp, 0x00, sizeof(SogyoSettingDsp));
#endif

	GetIniParam();										// 動的パラメータをiniファイルから読込
	int nCheckInterval = GetPrivateProfileInt(m_cSession, "CHECK_INTERVAL", 1000, TASKINI_NAME);

	// 周期
	__int64 ts = (__int64)nCheckInterval * -10000; //0;	// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evCheckTimer,   (LARGE_INTEGER *)&ts, nCheckInterval, NULL, NULL, FALSE);
}

//------------------------------------------
// 定周期チェックタイマー 停止
//------------------------------------------
void SogyoManager::CheckStop()
{
	AutoLock autolock(&m_csLock);

	// タイマー停止
	CancelWaitableTimer(m_evCheckTimer);

	// 操業状態 不明
	m_nMpm = 0;
	mcls_pStatus->SetSogyo( DIV_SOGYO_NON, m_nMpm );
	mcls_pStatus->SetLineSpeed(0);
#ifdef DSP_SOGYO_CHECK
	m_nMpmDsp = 0;
	mcls_pStatus->SetSogyoDsp(DIV_SOGYO_NON, m_nMpmDsp);
	mcls_pStatus->SetLineSpeedDsp(0);
#endif
}

//------------------------------------------
// 現在の検査用サンプリングクロック値 取得
// DWORD sampCnt 現在のサンプリングクロック値
// double dResV 直近の縦分解能 [mm/pixel]
//------------------------------------------
void SogyoManager::SetNowSampCnt(DWORD sampCnt, double dResV)
{
	AutoLock autolock(&m_csLock);

	m_nNowSampCnt = sampCnt;
	m_dResV = dResV;
	QueryPerformanceCounter((LARGE_INTEGER *)&m_pcNow);	// 現在時間を取得
}


#ifdef DSP_SOGYO_CHECK
//------------------------------------------
// 現在の表示用サンプリングクロック値 取得
// DWORD sampCnt 現在のサンプリングクロック値
// double dResV 直近の縦分解能 [mm/pixel]
//------------------------------------------
void SogyoManager::SetNowSampCntDsp(DWORD sampCnt, double dResV)
{
	AutoLock autolock(&m_csLock);

	m_nNowSampCntDsp = sampCnt;
	m_dResVDsp = dResV;
	QueryPerformanceCounter((LARGE_INTEGER *)&m_pcNowDsp);	// 現在時間を取得
}
#endif

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 判定

//------------------------------------------
// 通板中 定周期チェック
//------------------------------------------
void SogyoManager::CheckSampCntDiff()
{
	//// 必要なメンバーをローカルに保持
	// 下部でStatusMgrのステータス変更するとDB Updateなので、PlgMgrがロックされてしまう対策
	////AutoLock autolock(&m_csLock);
	//EnterCriticalSection(&m_csLock);			// ---------------->>>
	//	double	pcDiff		= (double)(m_pcNow - m_pcPre) / m_pf;		// ２点間の時間[s]
	//	long	sampCntDiff = m_nNowSampCnt - m_nPreSampCnt;			// ２点間のカウント

	//	double	dResV		= m_dResV;
	//	DWORD	nNowSampCnt = m_nNowSampCnt;
	//	__int64	pcNow		= m_pcNow;
	//LeaveCriticalSection(&m_csLock);			// <<<------------------
	//	
	AutoLock autolock(&m_csLock);			// ---------------->>>
		double	pcDiff		= (double)(m_pcNow - m_pcPre) / m_pf;		// ２点間の時間[s]
		long	sampCntDiff = m_nNowSampCnt - m_nPreSampCnt;			// ２点間のカウント

		double	dResV		= m_dResV;
		DWORD	nNowSampCnt = m_nNowSampCnt;
		__int64	pcNow		= m_pcNow;
	autolock.UnLock();						// <<<------------------



	////////////////////////////////////
	// ライン速度を算出
	////////////////////////////////////
	//// 前回値として保存
	m_nPreSampCnt = nNowSampCnt;
	m_pcPre = pcNow;

	if(0 > sampCntDiff) {
		return;	// 1周した (通常ありえない)
	}


	//// 今回のライン速度
	//   あまり正確では無いが、そんなに精度が必要な訳ではないので、OKとする
	int nMpm;
	if( 0 == sampCntDiff ) {
		nMpm = 0;
	} else {
		nMpm = (int)(((sampCntDiff*dResV) / pcDiff) * 60.0 / 1000.0);	// [mm/s]→[mpm]
		if( 0 > nMpm || 3000 < nMpm ) {		// 何かしらの異常
			LOG(em_ERR), "[%s] 検査速度=%d, samp=%d/%d, time=%lf", my_sThreadName, nMpm, m_nNowSampCnt, m_nPreSampCnt, pcDiff );
			return;
		}
	}
	//// 前回値として保存
	m_nMpm = nMpm;

	////////////////////////////////////
	// 操業状態 判定
	////////////////////////////////////
	// この１回だけを見た時にどうか
	EM_DIV_SOGYO	emNowSogyo   = mcls_pStatus->GetSogyo();	// 現在の操業状態
	EM_DIV_SOGYO	emTotalSogyo = DIV_SOGYO_NON;				// 全ての条件を見た上での操業状態
	EM_DIV_SOGYO	emOneSogyo   = DIV_SOGYO_NON;				// この1回だけを見た時の操業状態
	// 低速操業 (稼動 → 停止)
	if(SogyoSetting.InfUpToDown_Slow.bUmu && nMpm <= SogyoSetting.InfUpToDown_Slow.nLimitSpeed ) {
		emOneSogyo = DIV_SOGYO_SLOW;
		
		if( DIV_SOGYO_STOP != emNowSogyo ) {
			SogyoSetting.InfUpToDown_Slow.nNowCnt ++;
			if( SogyoSetting.InfUpToDown_Slow.nNowCnt >= SogyoSetting.InfUpToDown_Slow.nCheckCnt ) emTotalSogyo = DIV_SOGYO_SLOW;
		}
	}
	// ライン停止 (稼動 → 停止)
	if(SogyoSetting.InfUpToDown_Stop.bUmu && nMpm <= SogyoSetting.InfUpToDown_Stop.nLimitSpeed ) {
		emOneSogyo = DIV_SOGYO_STOP;
	
		if( DIV_SOGYO_STOP != emNowSogyo ) {
			SogyoSetting.InfUpToDown_Stop.nNowCnt ++;
			if( SogyoSetting.InfUpToDown_Stop.nNowCnt >= SogyoSetting.InfUpToDown_Stop.nCheckCnt ) emTotalSogyo = DIV_SOGYO_STOP;
		}
	}
	// 低速操業 (停止 → 稼動)
	if(SogyoSetting.InfDownToUp_Slow.bUmu && nMpm > SogyoSetting.InfDownToUp_Slow.nLimitSpeed ) {
		emOneSogyo = DIV_SOGYO_SLOW;

		if( (DIV_SOGYO_SLOW != emNowSogyo && DIV_SOGYO_RUN != emNowSogyo) ) {
			SogyoSetting.InfDownToUp_Slow.nNowCnt ++;
			if( SogyoSetting.InfDownToUp_Slow.nNowCnt >= SogyoSetting.InfDownToUp_Slow.nCheckCnt ) emTotalSogyo = DIV_SOGYO_SLOW;
		}
	}
	// ライン操業 (停止 → 稼動)
	if(SogyoSetting.InfDownToUp_Run.bUmu  && nMpm > SogyoSetting.InfDownToUp_Run.nLimitSpeed ) {
		emOneSogyo = DIV_SOGYO_RUN;

		if( (DIV_SOGYO_RUN != emNowSogyo) ) {
			SogyoSetting.InfDownToUp_Run.nNowCnt ++;
			if( SogyoSetting.InfDownToUp_Run.nNowCnt >= SogyoSetting.InfDownToUp_Run.nCheckCnt ) emTotalSogyo = DIV_SOGYO_RUN;
		}
	}
//	LOG(em_INF), "今回は=%s, %s", DivNameManager::GetDivSogyo(emOneSogyo), DivNameManager::GetDivSogyo(emTotalSogyo));
//	LOG(em_INF), "今回は=%s, %s, %d,%d,%d,%d", DivNameManager::GetDivSogyo(emOneSogyo), DivNameManager::GetDivSogyo(emTotalSogyo), SogyoSetting.array[0].nNowCnt, SogyoSetting.array[1].nNowCnt, SogyoSetting.array[2].nNowCnt, SogyoSetting.array[3].nNowCnt);


	//// 現状と違う？
	EM_DIV_SOGYO  emAns = DIV_SOGYO_NON;				// 最終判定
	if( emOneSogyo == emNowSogyo ) {
		// 状態に変化が無い為、カウント値をリセット
		for(int ii=0; ii<4; ii++) SogyoSetting.array[ii].nNowCnt = 0;

	} else {
		// 操業状態が変化した
		if( DIV_SOGYO_NON ==emNowSogyo ) {		// 今現在 操業状態が不明の為、今回の結果のみで判定した操業状態をセット
			emAns = emOneSogyo;
		} else if( emNowSogyo != emTotalSogyo && DIV_SOGYO_NON != emTotalSogyo ) {
			emAns = emTotalSogyo;
		}
	}


	////////////////////////////////////
	// 最終
	////////////////////////////////////
	//// 操業状態を変更
	if(DIV_SOGYO_NON != emAns) {
		mcls_pStatus->SetSogyo( emAns, nMpm );
		for(int ii=0; ii<4; ii++) SogyoSetting.array[ii].nNowCnt = 0;
	}
	mcls_pStatus->SetLineSpeed(nMpm);
}

#ifdef DSP_SOGYO_CHECK
//------------------------------------------
// 通板中 定周期チェック
//------------------------------------------
void SogyoManager::CheckSampCntDiffDsp()
{
	//// 必要なメンバーをローカルに保持
	// 下部でStatusMgrのステータス変更するとDB Updateなので、PlgMgrがロックされてしまう対策
	////AutoLock autolock(&m_csLock);
	//EnterCriticalSection(&m_csLock);			// ---------------->>>
	//	double	pcDiff		= (double)(m_pcNow - m_pcPre) / m_pf;		// ２点間の時間[s]
	//	long	sampCntDiff = m_nNowSampCnt - m_nPreSampCnt;			// ２点間のカウント

	//	double	dResV		= m_dResV;
	//	DWORD	nNowSampCnt = m_nNowSampCnt;
	//	__int64	pcNow		= m_pcNow;
	//LeaveCriticalSection(&m_csLock);			// <<<------------------
	//	
	AutoLock autolock(&m_csLock);			// ---------------->>>
	double	pcDiff = (double)(m_pcNowDsp - m_pcPreDsp) / m_pf;			// ２点間の時間[s]
	long	sampCntDiff = m_nNowSampCntDsp - m_nPreSampCntDsp;			// ２点間のカウント

	double	dResV = m_dResVDsp;
	DWORD	nNowSampCnt = m_nNowSampCntDsp;
	__int64	pcNow		= m_pcNowDsp;
	autolock.UnLock();						// <<<------------------



	////////////////////////////////////
	// ライン速度を算出
	////////////////////////////////////
	//// 前回値として保存
	m_nPreSampCntDsp = nNowSampCnt;
	m_pcPreDsp = pcNow;

	if(0 > sampCntDiff) {
		return;	// 1周した (通常ありえない)
	}


	//// 今回のライン速度
	//   あまり正確では無いが、そんなに精度が必要な訳ではないので、OKとする
	int nMpm;
	if( 0 == sampCntDiff ) {
		nMpm = 0;
	} else {
		nMpm = (int)(((sampCntDiff*dResV) / pcDiff) * 60.0 / 1000.0);	// [mm/s]→[mpm]
		if( 0 > nMpm || 3000 < nMpm ) {		// 何かしらの異常
			LOG(em_ERR), "[%s] 表示速度=%d, samp=%d/%d, time=%lf", my_sThreadName, nMpm, m_nNowSampCntDsp, m_nPreSampCntDsp, pcDiff );
			return;
		}
	}
	//// 前回値として保存
	m_nMpmDsp = nMpm;

	////////////////////////////////////
	// 操業状態 判定
	////////////////////////////////////
	// この１回だけを見た時にどうか
	EM_DIV_SOGYO	emNowSogyo   = mcls_pStatus->GetSogyoDsp();	// 現在の操業状態
	EM_DIV_SOGYO	emTotalSogyo = DIV_SOGYO_NON;				// 全ての条件を見た上での操業状態
	EM_DIV_SOGYO	emOneSogyo   = DIV_SOGYO_NON;				// この1回だけを見た時の操業状態
	// 低速操業 (稼動 → 停止)
	if(SogyoSettingDsp.InfUpToDown_Slow.bUmu && nMpm <= SogyoSettingDsp.InfUpToDown_Slow.nLimitSpeed ) {
		emOneSogyo = DIV_SOGYO_SLOW;
		
		if( DIV_SOGYO_STOP != emNowSogyo ) {
			SogyoSettingDsp.InfUpToDown_Slow.nNowCnt ++;
			if( SogyoSettingDsp.InfUpToDown_Slow.nNowCnt >= SogyoSettingDsp.InfUpToDown_Slow.nCheckCnt ) emTotalSogyo = DIV_SOGYO_SLOW;
		}
	}
	// ライン停止 (稼動 → 停止)
	if(SogyoSettingDsp.InfUpToDown_Stop.bUmu && nMpm <= SogyoSettingDsp.InfUpToDown_Stop.nLimitSpeed ) {
		emOneSogyo = DIV_SOGYO_STOP;
	
		if( DIV_SOGYO_STOP != emNowSogyo ) {
			SogyoSettingDsp.InfUpToDown_Stop.nNowCnt ++;
			if( SogyoSettingDsp.InfUpToDown_Stop.nNowCnt >= SogyoSettingDsp.InfUpToDown_Stop.nCheckCnt ) emTotalSogyo = DIV_SOGYO_STOP;
		}
	}
	// 低速操業 (停止 → 稼動)
	if(SogyoSettingDsp.InfDownToUp_Slow.bUmu && nMpm > SogyoSettingDsp.InfDownToUp_Slow.nLimitSpeed ) {
		emOneSogyo = DIV_SOGYO_SLOW;

		if( (DIV_SOGYO_SLOW != emNowSogyo && DIV_SOGYO_RUN != emNowSogyo) ) {
			SogyoSettingDsp.InfDownToUp_Slow.nNowCnt ++;
			if( SogyoSettingDsp.InfDownToUp_Slow.nNowCnt >= SogyoSettingDsp.InfDownToUp_Slow.nCheckCnt ) emTotalSogyo = DIV_SOGYO_SLOW;
		}
	}
	// ライン操業 (停止 → 稼動)
	if(SogyoSettingDsp.InfDownToUp_Run.bUmu  && nMpm > SogyoSettingDsp.InfDownToUp_Run.nLimitSpeed ) {
		emOneSogyo = DIV_SOGYO_RUN;

		if( (DIV_SOGYO_RUN != emNowSogyo) ) {
			SogyoSettingDsp.InfDownToUp_Run.nNowCnt ++;
			if( SogyoSettingDsp.InfDownToUp_Run.nNowCnt >= SogyoSettingDsp.InfDownToUp_Run.nCheckCnt ) emTotalSogyo = DIV_SOGYO_RUN;
		}
	}
//	LOG(em_INF), "今回は=%s, %s", DivNameManager::GetDivSogyo(emOneSogyo), DivNameManager::GetDivSogyo(emTotalSogyo));
//	LOG(em_INF), "今回は=%s, %s, %d,%d,%d,%d", DivNameManager::GetDivSogyo(emOneSogyo), DivNameManager::GetDivSogyo(emTotalSogyo), SogyoSettingDsp.array[0].nNowCnt, SogyoSettingDsp.array[1].nNowCnt, SogyoSettingDsp.array[2].nNowCnt, SogyoSettingDsp.array[3].nNowCnt);


	//// 現状と違う？
	EM_DIV_SOGYO  emAns = DIV_SOGYO_NON;				// 最終判定
	if( emOneSogyo == emNowSogyo ) {
		// 状態に変化が無い為、カウント値をリセット
		for(int ii=0; ii<4; ii++) SogyoSettingDsp.array[ii].nNowCnt = 0;

	} else {
		// 操業状態が変化した
		if( DIV_SOGYO_NON ==emNowSogyo ) {		// 今現在 操業状態が不明の為、今回の結果のみで判定した操業状態をセット
			emAns = emOneSogyo;
		} else if( emNowSogyo != emTotalSogyo && DIV_SOGYO_NON != emTotalSogyo ) {
			emAns = emTotalSogyo;
		}
	}


	////////////////////////////////////
	// 最終
	////////////////////////////////////
	//// 操業状態を変更
	if(DIV_SOGYO_NON != emAns) {
		mcls_pStatus->SetSogyoDsp( emAns, nMpm );
		for(int ii=0; ii<4; ii++) SogyoSettingDsp.array[ii].nNowCnt = 0;
	}
	mcls_pStatus->SetLineSpeedDsp(nMpm);
}
#endif