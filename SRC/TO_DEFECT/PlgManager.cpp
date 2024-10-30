#include "stdafx.h"
#include "PlgManager.h"

// 静的クラス
#include "OpSendFunc.h"													// 表示管理送信クラス

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

LogFileManager*			mcls_pLogPlg = NULL;
#define LOGPLG(t)  if(mcls_pLogPlg!=NULL)       mcls_pLogPlg->Write(KizuLib::LogFileManager::t			// ログ出力
LogFileManager*			mcls_pLogPlgDsp = NULL;
#define LOGPLGDSP(t)  if(mcls_pLogPlgDsp!=NULL)       mcls_pLogPlgDsp->Write(KizuLib::LogFileManager::t			// ログ出力

//------------------------------------------
// コンストラクタ
//------------------------------------------
PlgManager::PlgManager() :
ThreadManager("PlgMgr")
{
	// シグナル
	m_evSamplingStart	= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evSamplingStop	= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evTimer			= CreateWaitableTimer(NULL, FALSE, NULL);
	m_evKenNotPos		= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evTimerDspLen		= CreateWaitableTimer(NULL, FALSE, NULL);

	// UDPソケット初期化
	memset(&m_addr, 0x00, sizeof(m_addr));
	for( int ii=0; ii<MAX_NUM_UDP; ii++) {
		m_sock[ii] = INVALID_SOCKET;
	}

	// その他
	m_emIsLinePLG			= DIV_PLG_LINE;
	m_emUnten				= DIV_UNTEN_NOMAL;
#ifdef DISABLE_DSP_PLG
	m_bIsDisableDspPLG		= true;
#else
	m_bIsDisableDspPLG		= false;
#endif
	m_nDistPos				= 0;
	m_nDistLen = 0;
	m_bRecvInit				= false;
	m_nMin					= 0;
	m_nMax					= 0;
	m_nPreExSync			= 0;

	setlocale(LC_ALL, "Japanese");	// これが無いと日本語パスを含むフォルダの場合、保存できない。(ライブラリの中で実行するべきか迷っているが、変更するとタスク全体に影響するので、とりあえずここで設定)

	mcls_pLogPlg = new LogFileManager("PLGログ_検査", NULL, 20, 3);	// ここをコメント化するだけでログは出ない。
	LOGPLG(em_INF), "スタート");
	mcls_pLogPlgDsp = new LogFileManager("PLGログ_表示", NULL, 20, 3);	// ここをコメント化するだけでログは出ない。
	LOGPLGDSP(em_INF), "スタート");
}

//------------------------------------------
// デストラクタ
//------------------------------------------
PlgManager::~PlgManager()
{
	// シグナル
	CloseHandle(m_evSamplingStart);
	CloseHandle(m_evSamplingStop);
	CloseHandle(m_evTimer);
	CloseHandle(m_evKenNotPos);
	CloseHandle(m_evTimerDspLen);


	// UDPソケット破棄
	for( int ii=0; ii<MAX_NUM_UDP; ii++) {
		if( INVALID_SOCKET != m_sock[ii] ) {
			closesocket(m_sock[ii]);
			m_sock[ii] = INVALID_SOCKET;
		}
	}

	if (NULL != mcls_pLogPlg) delete mcls_pLogPlg;
	if (NULL != mcls_pLogPlgDsp) delete mcls_pLogPlgDsp;
}

//------------------------------------------
// 関数初期化
//------------------------------------------
void PlgManager::Alloc()
{
	//======================================================
	// 通常の初期化関連


	// 初期化
	m_bMaxLen_Ken = false;								// コイル長上限到達
	m_bMaxLen_Dsp = false;
	m_emIsLinePLG = DIV_PLG_LINE;
	m_emUnten = DIV_UNTEN_NOMAL;

	ClearMember();
	
	// パルスレート
	m_dPlsR_Ken = 0.0;
	m_dPlsR_Dsp = 0.0;
	m_dPlsR_Sub = 0.0;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 初期化

//------------------------------------------
// 初期化
//------------------------------------------
void PlgManager::ClearMember()
{
	//================================================
	// PLG関連
	memset(&m_trFrm, 0x00, sizeof(m_trFrm));
	for (int ii = 0; ii<NUM_MEN; ii++) {
		m_nDly[ii] = 0;
		m_nChgFrameNo[ii] = 0;
	}
	m_nTime = 0;
	m_dDistance = 0.0;

	m_nFrameNo = 0;
	m_nSpeed = 0;
	m_nSpeedDsp = 0;
	m_dResV = mcls_pParam->CalcResV(m_emIsLinePLG);
	m_dResVCycle = 0.0;
	m_nPreExSync = 0;
	m_preLen_KenSub = 0;

#ifndef PLG_DSP_KEN_EQAL
		m_preCountDsp = 0;
#endif

	m_bCoilOutChange = false;

	//// 初期化
	CoilInit();
}

//------------------------------------------
// 初期化
//------------------------------------------
void PlgManager::CoilInit()
{
	ParamManager::P_COMMON_MAST	prm	= mcls_pParam->GetCommonMast();

	//================================================
	// 距離管理
	// 前回値用 (検査基点用)
	m_dCoilOldLen_Ken				= 0.0;
	m_bCoilChange_Ken				= false;

	// 前回値用 (表示基点用)
	m_dCoilOldLen_BaseDsp			= 0.0;
	for(int ii=0; ii<MAX_POS_INDEX; ii++) {
		m_dLen_Dsp[ii]				= 0.0;
		m_bCoilChange_Dsp[ii]		= false;
	}

	//================================================
	// 距離管理
	//// 最下流の表示位置決定

	double dDspLen[NUM_DSP_WPD];
	dDspLen[0] = prm.Dist.nDspSharCut;										// 管理対象の一番下流※の距離。今回の基準とする ※今回はシャーカット位置

	// 検査
	m_dLen_Ken[m_nDistPos]			= m_nDistLen;							// 基準面の方が先に進んでいる事とする。
	m_dLen_Ken[m_nDistNotPos]		= 0.0;
	m_dLen_WpdKenPer				= -1 * m_nDistLen;
	m_dLen_KenSub					= m_nDistLen;

	// 表示	(距離管理の一番下流にコイル先端が来ている状態からスタート)
	m_dLen_BaseDsp				= 0.0;
	m_dLen_Dsp[POS_DSP_DSP1_P]	= - prm.Dist.nDspBaseKensaTop;				// 表面検査台
	m_dLen_Dsp[POS_DSP_DSP2_P]	= - prm.Dist.nDspBaseKensaBot;				// 裏面検査台
	m_dLen_Dsp[POS_DSP_SHR_I]	= - dDspLen[0];								// シャーカット位置
	m_dLen_OutCut				= m_dLen_Dsp[POS_DSP_SHR_I];				// シャー位置と同じため	
	
	//// 検査位置 (各システム個別)
	// 今回は無し

	//// 表示位置 (各システム個別)
	// 今回は無し
	

	//================================================
	// コイル長異常状態チェック
	mcls_pStatus->SetStatusQue("MAXLEN_KEN_ID", true, false);
	mcls_pStatus->SetStatusQue("MAXLEN_DSP_ID", true, false);
	m_bMaxLen_Ken = false;							
	m_bMaxLen_Dsp = false;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// メインスレッド
//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int PlgManager::ThreadFirst()
{
	//================================================
	// UDP生成
	while( TRUE ) {
		if( SetUdp(0, EPC_UDP_IP, EPC_UDP_PORT ) ) break;
		LOG(em_ERR), "[%s] UDP 準備中・・・ [%d]", my_sThreadName, EPC_UDP_PORT);
		Sleep(2000);
	}

	//================================================
	// 表示位置送信タイマー
	__int64 ts = TIME_INTERVAL_DSP_LEN * -10000; ;	// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evTimerDspLen, (LARGE_INTEGER *)&ts, TIME_INTERVAL_DSP_LEN, NULL, NULL, FALSE);



	//================================================
	// 通常のスレッド操作
	//// イベント設定
	AddEventNum(1, &m_evSamplingStart);
	AddEventNum(1, &m_evSamplingStop);
	AddEventNum(1, &mcls_pSeqFrameR->GetQueFrame()->g_evSem);
	AddEventNum(1, &m_evTimer);
	AddEventNum(1, &m_evTimerDspLen);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int PlgManager::ThreadLast()
{
	//// タイマー操作
	CancelWaitableTimer(m_evTimer);
	CancelWaitableTimer(m_evTimerDspLen);
	return 0;
}


//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void PlgManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_SAMP_START = 0,				// 定周期 スタート
			EV_SAMP_STOP,					// 定周期 ストップ
			EV_QUE_FRAME,					// フレーム情報受信
			EV_TIMER,						// 定周期チェック用
			EV_DSP_LEN_CHECK,				// 表示用 通板距離送信
	};
	
	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_SAMP_START: 							// 定周期スタート
	{
		//====================================
		// 選択されているPLG状態をチェック
		m_emIsLinePLG = mcls_pStatus->GetPlg();
		m_emUnten	  = mcls_pStatus->GetUnten();

		LOGPLG(em_WAR), "===========================================================");
		LOGPLG(em_INF), ",UDP FNo,表[m],裏[m],表残[m],裏残[m],ﾌﾗｸﾞ1,2,3,速度[mpm],時間差[ms],時間周期[ms],エッジ位置負側_表[mm],エッジ位置正側_表[mm],エッジ位置負側_裏[mm],エッジ位置正側_裏[mm],エッジ位置フレームNo_表,エッジ位置フレームNo_裏,都度分解能");
		LOGPLG(em_INF), "%s認識, %s認識", DivNameManager::GetDivPlg(m_emIsLinePLG), DivNameManager::GetDivUnten(m_emUnten));

		LOGPLGDSP(em_WAR), "===========================================================");
		LOGPLGDSP(em_INF), ",表示基点[m],表検査台[m],裏検査台[m],ｼｬｰ位置[m],切替ﾌﾗｸﾞ1,2,3,速度[mpm],処理差[ms],時間周期[ms]");
		LOGPLGDSP(em_INF), "%s認識, %s認識", DivNameManager::GetDivPlg(m_emIsLinePLG), DivNameManager::GetDivUnten(m_emUnten));

		LOG(em_INF), "[%s] %s認識, %s認識, 表示PLG：%s", my_sThreadName, DivNameManager::GetDivPlg(m_emIsLinePLG), DivNameManager::GetDivUnten(m_emUnten), (m_bIsDisableDspPLG ? "無効" : "有効"));

		// 強制テスト時
		if( DIV_UNTEN_SMYU == m_emUnten)	m_bDebugMode = true;
		else														m_bDebugMode = false;

		//================================================	
		// 初期化
		ClearMember();

		//================================================
		// 開始
		// 初期カウント値取得
		m_preCountDsp = mcls_pDmem->GetDmRecv().dsp.nPlgAdd;
		LOG(em_INF), "[%s] カウント値初期値取得　[%d]", my_sThreadName, m_preCountDsp);

		//================================================
		// パルスレート
		if (true) {
			m_dPlsR_Ken = mcls_pParam->GetParamCommon().dPls;
			m_dPlsR_Dsp = mcls_pParam->GetParamCommon().dPlsDsp;
			m_dPlsR_Sub = mcls_pParam->GetParamCommon().dPlsSub;
		}
		
		//================================================
		// 操業状態監視 開始
		if (true) {
			DWORD nLvalAdd = mcls_pDmem->GetDmRecv().ken.nLvalAdd;
			//if (mcls_pParam->IsLedCycle()) nLvalAdd /= 2;			// 交互点灯なら半分
			mcls_pSogyo->CheckStart(nLvalAdd);						// 今回はサンプリングカウントの代わりに、ラインPLGカウントを使用する
		}

		//================================================
		// タイマ
		SetEvWaitableTimer(m_evTimer, TIME_INTERVAL_DSP_PLG, TIME_INTERVAL_DSP_PLG);
		SetEvWaitableTimer(m_evTimerDspLen, TIME_INTERVAL_DSP_LEN, TIME_INTERVAL_DSP_LEN);

		mcls_Time.InitAve();
		mcls_Time.CounterStart();

		mcls_TimeDspSpeed.InitAve();
		mcls_TimeDspSpeed.CounterStart();
	}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_SAMP_STOP:								// 定周期停止	
		CancelWaitableTimer(m_evTimer);
		CancelWaitableTimer(m_evTimerDspLen);

		//// 操業状態監視 停止
		mcls_pSogyo->CheckStop();
		mcls_pStatus->SetLength( 0 );

		break;

//-----------------------------------------------------------------------------------------------
	case EV_QUE_FRAME:								// フレーム情報受信
	{
		mcls_pLock->LockTh();	//// <<<<<--------------

		mcls_Time.CounterCycle();

		SEQ_TYPE_DATA_FRAME* p = mcls_pSeqFrameR->GetQueFrame()->GetFromHead();

		if (NULL != p)
		{
			// 1フレーム通板完了
			NextLength_Ken(p);

			SAFE_DELETE(p);
		}

		mcls_pLock->UnlockTh();	//// <<<<<--------------
		break;
	}

//-----------------------------------------------------------------------------------------------
	case EV_TIMER:
		mcls_pLock->LockTh();	//// <<<<<--------------

		//================================================
		// 検査比較用PLG 測長距離を格納
		{
			DWORD nowLength = mcls_pDmem->GetDmRecv().nLengthSub;
			if (m_preLen_KenSub != nowLength)
			{
				DWORD diff = nowLength - m_preLen_KenSub;
				m_preLen_KenSub = nowLength;
				m_dLen_KenSub += (double)diff;
			}
		}

		//================================================
		// 表示位置 での 通板距離管理
		NextLength_Dsp();

		ExecCycle();

		//================================================
		// 最小区間での機器状態把握
		CheckIntervalStatus();

		mcls_pLock->UnlockTh();	//// <<<<<--------------
		break;

//-----------------------------------------------------------------------------------------------
	case EV_DSP_LEN_CHECK:										// 表示用 通板距離送信
		mcls_pLock->LockTh();	//// <<<<<--------------
		OpSendFunc::SendOp_Length( );
		mcls_pLock->UnlockTh();	//// <<<<<--------------
		break;



//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

//////////////////////////////////////////////////////////////////////
// UDP

//------------------------------------------
// UDP SOCKET生成
// int no 生成No(0オリジン)
// const char* ip IPアドレス (例：192.9.200.255)
// WORD port ポートNo (例：8003)
// 戻り値 復帰情報
//------------------------------------------
BOOL PlgManager::SetUdp(int no, const char* ip, WORD port)
{
	_ASSERT(0 <= no);
	_ASSERT(MAX_NUM_UDP > no);


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

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 通板距離管理関連

//------------------------------------------
// 検査基点での距離管理 (1フレーム取り込み完了)
// SEQ_TYPE_DATA_FRAME const* p シーケンサからの1フレーム伝文
//------------------------------------------
void PlgManager::NextLength_Ken(SEQ_TYPE_DATA_FRAME const* p)
{
	double	dLen_Ken[2];								// 各検査装置位置での通板距離 [mm] (表/裏 別)
	DWORD	nFrameNo;									// フレームNo (0オリジン)
	
	// 現在値保持
	for(int ii=0; ii<NUM_MEN; ii++) dLen_Ken[ii] = m_dLen_Ken[ii];
	nFrameNo = m_nFrameNo;

	//================================================
	// シーケンサ情報 整合性チェック
	if( m_nFrameNo != p->nFrameNo ) {
		LOG(em_ERR), "[%s] シーケンサ フレーム伝文異常 Fno(自:seq)%d!=%d", my_sThreadName, m_nFrameNo, p->nFrameNo);
		syslog(246, "[Fno(自:seq)%d!=%d]", m_nFrameNo, p->nFrameNo);

		mcls_pStatus->AddQueMyErrWar(true, ON_PLG_FRAME_DELETE);		// フレーム消失。検査停止
	}

	//================================================
	// 通板距離管理
	double tm = p->nTimeDiff;														// 10μs
	long frame_diff;
	if (DIV_PLG_TEST == m_emIsLinePLG) {
		double rate = (double)mcls_pParam->GetParamCommon().typCamCon[DIV_PLG_TEST - 1].nDiv / (double)mcls_pParam->GetParamCommon().typCamCon[DIV_PLG_TEST - 1].nMul;
		frame_diff = (long)((double)((long)p->nExSyncAdd - (long)m_nPreExSync) * rate);
		//LOG(em_INF), "検査基点 PLG ExSyncAdd:%d PreExSync:%d rate(nDiv:%d nMul:%d):%f diff:%d", (long)p->nExSyncAdd, (long)m_nPreExSync, mcls_pParam->GetParamCommon().typCamCon[DIV_PLG_TEST - 1].nDiv, mcls_pParam->GetParamCommon().typCamCon[DIV_PLG_TEST - 1].nMul, rate, frame_diff);
		m_nPreExSync = p->nExSyncAdd;
	}
	else {
		frame_diff = p->nPlgDiff;
	}
//LOG(em_INF), "PLG SampCnt:%d GetSampBuffPlg(now:%d,%d, next:%d,%d) diff:%d", m_nCurrentSampCnt, buff_pos, GetSampBuffPlg(buff_pos), next_buff_pos, GetSampBuffPlg(next_buff_pos), frame_diff);


	//================================================
	// 今回の情報決定
	m_nTime = (int)(tm / 100.0 + 0.5F);					// 10us → ms
	m_dDistance = m_dPlsR_Ken * (double)(frame_diff);				// １フレームで進んだ距離
	if( m_dDistance < 0 ) m_dDistance = 0;							// ０クリアの場合は無視

	m_nSpeed	= (DWORD)(m_dDistance / ((double)(m_nTime/1000.0) ) + 0.5F);	// [mm/sec]

#ifndef RESV_KOTEI
	m_dResV		= (double)(m_dDistance / (SAMPBUFFSIZE));			// 分解能決定
#else
	m_dResVCycle= (double)(m_dDistance / (SIZE_FRAME_IMG_Y));			// 分解能決定 
#endif

	
	////====================================
	//// UDP情報管理(ZZ_EPC用)

#ifdef DISABLE_ZZ_EPC
	//// 共通情報
	m_udp.framenum	= m_nFrameNo;
	m_udp.vRes		= (float)m_dResV;

	// UDPパケット送信
#ifdef DEBUG_LOG_UDP
	LOG(em_INF), "[%s] <frm:%d> vRes:%.3f", my_sThreadName, m_udp.framenum, m_udp.vRes);
#endif

	sendto( GetSock(0), (char *)&m_udp, sizeof(m_udp), 0, (sockaddr *)GetAddr(0), sizeof(sockaddr_in));
#endif

	////====================================
	//// エッジ位置情報 UDP送信
	int		nUseLimit[NUM_MEN];
	float	dEdge_Left[NUM_MEN];
	float	dEdge_Right[NUM_MEN];
	for(int ii=0; ii<NUM_MEN; ii++) {
		nUseLimit[ii] = (mcls_pEdgePosCont->GetEdgeAns(ii) ? 1 : 0);		// 最終エッジ位置が正常時、エッジ範囲指定で検出
		dEdge_Left[ii] = mcls_pEdgePosCont->GetEdgeAnsPos(ii, 0);			// 左エッジ
		dEdge_Right[ii] = mcls_pEdgePosCont->GetEdgeAnsPos(ii, 1);			// 右エッジ
	}
	mcls_pEdgeRestSd->SendEdgeRestrict(m_nFrameNo, nUseLimit, dEdge_Left, dEdge_Right);

	////====================================
	//// フレームNoトラッキング情報管理
	memset(&m_trFrm, 0x00, sizeof(m_trFrm));
	for(int ii=0; ii<NUM_MEN; ii++) {
		if (0 != m_nDly[ii]) {
			LOG(em_MSG), "[%s] <%s> 旧遅延実行キー実行=%d FrameNo=%d ChgFNo:%d", my_sThreadName, DivNameManager::GetTorB(ii), m_nDly[ii], m_nFrameNo, m_nChgFrameNo[ii]);
			m_nDly[ii] = 0;
		}
		if(m_nFrameNo >= m_nChgFrameNo[ii] && 0 < m_nChgFrameNo[ii]) {
			m_trFrm.nCoilChg[ii] = 1;

			LOG(em_MSG), "[%s] <%s> 遅延実行キー実行=%d FrameNo=%d ChgFNo:%d", my_sThreadName, DivNameManager::GetTorB(ii), m_nDly[ii], m_nFrameNo, m_nChgFrameNo[ii]);
			syslog(271, "[%s]", DivNameManager::GetTorB(ii));
			m_nChgFrameNo[ii] = 0;
		}
	}
	m_trFrm.nFrameNo = m_nFrameNo;
	m_trFrm.nSpeed = m_nSpeed;
	QueryPerformanceCounter((LARGE_INTEGER*)&m_trFrm.nRecProcTime[0]);		// 1フレーム取り込み完了時のQueryPerformanceCounter値
	//// 出側物理コイル切り替え
	if( m_bCoilOutChange ) m_trFrm.nFCut = 1;							// ファーストカット切り替え

	mcls_pTrFrm->AddItem(-1, &m_trFrm);										// 全部に登録

#ifdef ENABLE_CYCLE
	for(int ii=0; ii<NUM_MEN; ii++) {
		if( DIV_SYS_RUN != mcls_pStatus->GetCycleSys(ii) ) {
			// 周期機能が止まっている場合は、登録済み情報を削除
			int nId = GetTrId(DIV_CYCLE, ii);
			mcls_pTrFrm->DelItem(nId, m_nFrameNo);
		}
	}
#endif
	//// 出側物理コイル切り替え
	// ファーストカット
	if( m_bCoilOutChange ) {
		LOG(em_MSG), "[%s] <%s> 出側物理コイル ファーストカット実行[出:%.2f(m)]", my_sThreadName, DivNameManager::GetTorB(0), m_dLen_OutCut/1000.0);
		//syslog();
		m_bCoilOutChange = false;
	}
	//// イニシャル伝文受信フラグによる切り替え
	if(m_bRecvInit) {
		LOG(em_MSG), "[%s] <%s> イニシャル伝文受信によるコイル切り替え[裏:%.2f(m),表:%.2f(m)]", my_sThreadName, DivNameManager::GetTorB(0), m_dLen_Ken[0]/1000.0, m_dLen_Ken[1]/1000.0);
		//syslog();
		m_bRecvInit = false;
	}

	//================================================
	// 情報 更新

	//// フレームNo加算
	m_nFrameNo = p->nFrameNo + 1;

	//// 通板距離加算
	m_dLen_Ken[m_nDistPos]		+= m_dDistance;							// 基準面
	m_dLen_Ken[m_nDistNotPos]	+= m_dDistance;							// もう一方
	m_dLen_WpdKenPer			-= m_dDistance;

	// もう一方の方にも溶接点到達を把握
	if (m_bCoilChange_Ken && m_dCoilOldLen_Ken <= m_dLen_Ken[m_nDistNotPos]) {
		// メインインスタンスに通知
		//  (同期して他クラスとかと一斉に処理させたい為、一度MainInctanceに通知する)
		// つまり、ここでは把握するだけで、実際切り替えは、MainInctanceから処理が呼び出される
		SetEvent(m_evKenNotPos);
	}

	//// 上限到達？
	if(COIL_LEN_MAX *1000 < m_dLen_Ken[m_nDistPos] ) {
		if(!m_bMaxLen_Ken) {
			// コイル長上限到達
			LOG(em_ERR), "[%s] 検査用コイル長上限到達 [コイルNo=%s]", my_sThreadName, mcls_pCoil->GetCoilKenR(m_nDistPos)->data.cCoilNoIn );
			syslog(275, "[検査用コイル: コイルNo=%s]", mcls_pCoil->GetCoilKenR(m_nDistPos)->data.cCoilNoIn);
			mcls_pStatus->SetStatusQue("MAXLEN_KEN_ID", false, false);
			m_bMaxLen_Ken = true;
		
			//// 強制コイル切替 
			//SetEvent(m_evCoilLenMaxKen);
		}
	}

	mcls_Time.PutTimestamp(1);

	//// 調査用
	if (true) {			// PLGのログ出力
		// コイル残長
		long nLenZan[NUM_MEN];		// コイル残長[mm] = 入側コイル長 - 通板距離 [mm] ※ダミーコイルはＭＡＸコイル長
		for (int ii = 0; ii<NUM_MEN; ii++) {
			if (mcls_pCoil->GetCoilKenR(ii)->bDummeyCoil) nLenZan[ii] = (COIL_LEN_MAX * 1000) - (long)dLen_Ken[ii];	// ダミーコイル
			else nLenZan[ii] = mcls_pCoil->GetCoilKenR(ii)->data.nLenIn * 1000 - (long)dLen_Ken[ii];							// プロコンコイル
		}
		//// 出側コイル長
		// エッジ位置 & 反映フレームNo
		DWORD	nFNoWk[NUM_MEN];
		float	edgePosL[NUM_MEN];		// エッジ位置負側(mm)
		float	edgePosR[NUM_MEN];		// エッジ位置正側(mm)
		for (int ii = 0; ii<NUM_MEN; ii++) mcls_pEdgePosCont->SearchEdgePos(ii, edgePosL[ii], edgePosR[ii], &nFNoWk[ii]); 	// エッジ位置決定
		
		//LOGPLG(em_INF), ",UDP FNo,表[m],裏[m],表残[m],裏残[m],出表[m],出裏[m],速度[mpm],処理時間[ms],時間差[ms],エッジ位置負側_表[mm],エッジ位置正側_表[mm],エッジ位置負側_裏[mm],エッジ位置正側_裏[mm],エッジ位置フレームNo_表,エッジ位置フレームNo_裏");	
		//LOGPLG(em_NON), ",%06d, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f,%d,%d,%d,%d, %d, %d, %d, %.3f, %.3f, %.3f, %.3f, %d,%d",
		LOGPLG(em_NON), ",%06d, %.3f, %.3f, %.3f, %.3f,%d,%d,%d, %d, %d, %d, %.3f, %.3f, %.3f, %.3f, %d,%d, %lf",
			nFrameNo, dLen_Ken[0]/1000.0, dLen_Ken[1]/1000.0, nLenZan[0]/1000.0, nLenZan[1]/1000.0,
			m_trFrm.nCoilChg[0], m_trFrm.nCoilChg[1], m_trFrm.nFCut,
			GetSpeed(),	(int)mcls_Time.GetTimestamp(1,0), (int)mcls_Time.GetCycleTime(),
			edgePosL[0], edgePosR[0], edgePosL[1], edgePosR[1], nFNoWk[0], nFNoWk[1],
			m_dResVCycle);
	}

	////================================================
	//// WPD認識
	//if( -1 != p->nWpdFramePos ) {
	//	WpdTrKenStart();
	//}
}

//------------------------------------------
// 定周期処理
//------------------------------------------
void PlgManager::ExecCycle()
{
	// 操業状態関係
	SEQ_DM_RECV_BASE dm = mcls_pDmem->GetDmRecv();			// コピーだから注意
	DWORD nLvalAdd = dm.ken.nLvalAdd;
	//if (mcls_pParam->IsLedCycle()) nLvalAdd /= 2;			// 交互点灯なら半分	 ここは半分にする必要なし。渡す分解能がパルスレートのため

	mcls_pSogyo->SetNowSampCnt(nLvalAdd, m_dResV);		// 現在のサンプリングクロック値 取得
#ifdef DSP_SOGYO_CHECK
	if (DIV_PLG_TEST == mcls_pStatus->GetPlg()) {
		DWORD nPlgAddDsp = dm.ken.nPlgAdd;
		mcls_pSogyo->SetNowSampCntDsp(nPlgAddDsp, m_dResV);		// 現在のサンプリングクロック値 取得
	}
	else {
		DWORD nPlgAddDsp = dm.dsp.nPlgAdd;
		mcls_pSogyo->SetNowSampCntDsp(nPlgAddDsp, m_dPlsR_Dsp);	// 現在のサンプリングクロック値 取得
	}
#endif


	// 距離
	mcls_pStatus->SetLength( (int)(m_dLen_Ken[m_nDistPos]/1000.0) );
}

//------------------------------------------
// 表示基点での 距離管理
//------------------------------------------
void PlgManager::NextLength_Dsp()
{

	//================================================
	// 距離加算

	// データ取得
	SEQ_DM_RECV_BASE dm = mcls_pDmem->GetDmRecv();		// コピーだから注意


	//// 表示距離 算出
	int sa;											// 今回の進んだPLGカウント
	DWORD nOldCount = m_preCountDsp;
	double dDistance;									// 今回の進んだ距離
	// テストパルス時は検査距離を使用
	if (DIV_PLG_TEST == m_emIsLinePLG) {
		double rate = (double)mcls_pParam->GetParamCommon().typCamCon[DIV_PLG_TEST - 1].nDiv / (double)mcls_pParam->GetParamCommon().typCamCon[DIV_PLG_TEST - 1].nMul;
		sa = (int)((int)(dm.ken.nLvalAdd - m_preCountDsp) * rate);
		dDistance = sa * m_dPlsR_Ken;
		//LOG(em_INF), "表示基点 PLG FrameNo:%d nLvalAdd:%d preCountDsp:%d rate(nDiv:%d nMul:%d):%f diff:%d", dm.ken.nFrameNo, dm.ken.nLvalAdd, m_preCountDsp, mcls_pParam->GetParamCommon().typCamCon[DIV_PLG_TEST - 1].nDiv, mcls_pParam->GetParamCommon().typCamCon[DIV_PLG_TEST - 1].nMul, rate, sa);
		m_preCountDsp = dm.ken.nLvalAdd;					// リセット
	}
	// 表示PLG無効時は検査距離を使用
	// ※現地調整時の期間限定
	else if (m_bIsDisableDspPLG) {
		sa = dm.ken.nPlgAdd - m_preCountDsp;
		dDistance = sa * m_dPlsR_Ken;
		m_preCountDsp = dm.ken.nPlgAdd;						// リセット
	}
	// それ以外では表示距離を使用
	else {
		sa = dm.dsp.nPlgAdd - m_preCountDsp;
		dDistance = sa * m_dPlsR_Dsp;
		m_preCountDsp = dm.dsp.nPlgAdd;						// リセット
	}
	if (m_nMin > m_preCountDsp) { m_nMin = m_preCountDsp; }
	if (m_nMax < m_preCountDsp) { m_nMax = m_preCountDsp; }
	
	// ソフト対策
	if (COIL_LEN_MAX * 1000 < dDistance) {
		LOG(em_ERR), "[PlgManager] 表示距離差異常[差：%d][DM:%lu→%lu][%.3lfmm][min:%lu][max:%lu]", sa, nOldCount, m_preCountDsp, dDistance, m_nMin, m_nMax);
		//★↓デバッグ用（いずれ削除予定）
		syslog(901, "[差：%d][DM:%lu→%lu][%.3lfmm][min:%lu][max:%lu]", sa, nOldCount, m_preCountDsp, dDistance, m_nMin, m_nMax);
		m_preCountDsp = nOldCount;		// 逆転操業の時は、前回値は保持しない(逆転時の距離追従はしない)
		return;
	}

	if( 0 != sa) {
		if (0>sa) {
			if (-20>sa)LOG(em_MSG), "[PlgManager] 逆転操業[差：%d][DM:%lu→%lu][%.3lfmm][min:%lu][max:%lu]",	sa, nOldCount, m_preCountDsp, dDistance, m_nMin, m_nMax);
			m_preCountDsp = nOldCount;		// 逆転操業の時は、前回値は保持しない(逆転時の距離追従はしない)
			return;
		}
		mcls_TimeDspSpeed.CounterCycle();

		NextLength_Dsp(dDistance);
	}

}

//------------------------------------------
// 表示基点での 距離管理
// double dDistance 今回進んだ距離
//------------------------------------------
void PlgManager::NextLength_Dsp(double dDistance)
{
	m_dLen_BaseDsp += dDistance;
	for(int ii=0; ii<POS_DSP_END; ii++) m_dLen_Dsp[ii] += dDistance;
	m_dLen_OutCut += dDistance;


	//================================================
	//// 各表示位置にコイル先端が到達
	for(int ii=0; ii<POS_DSP_END; ii++) {
	
		if( m_bCoilChange_Dsp[ii] && m_dCoilOldLen_BaseDsp <= m_dLen_Dsp[ii] ) {
			//double	dInitLen	= 0.0;											// コイル切替時の位置

			// 各ポイントで送る (そうしないと、タイミングによっては、最終行が送れない)
			OpSendFunc::SendOp_Length();

			// 切り替え処理
			if (POS_DSP_SHR_I > ii) {
				LOG(em_MSG), "[%s] 表示距離管理[%d] コイル切り替え 検査実測長=%0.3fm [%s]", my_sThreadName, ii, m_dLen_Dsp[ii] / 1000.0, mcls_pCoil->GetCoilDsp(ii)->cKizukenNo );
				mcls_pCoil->ChangeCoilDspPointer((EM_POS_DSP)ii, m_dLen_Dsp[ii], m_dLen_OutCut);
				m_dLen_Dsp[ii] = 0.0;
				m_bCoilChange_Dsp[ii] = false;
			}

			// シャー位置通過時独自処理
			else if (POS_DSP_SHR_I == ii) {
				LOG(em_MSG), "[PlgManager] シャー位置 コイル切り替え 検査実測長=%0.3fm", m_dLen_Dsp[ii] / 1000.0);

				//double dLen_OutSha = m_dLen_OutCut - dInitLen;		// シャー位置をコイル先端が通過時の出側距離
				//mcls_pCoil->ChangeCoilShaPointer(m_dLen_Dsp[ii], dLen_OutSha);
				mcls_pCoil->ChangeCoilShaPointer(m_dLen_Dsp[ii], m_dLen_OutCut);
				m_dLen_Dsp[ii] = 0.0;
				m_bCoilChange_Dsp[ii] = false;
			}
		}
	}


	//// 上限到達？
	if(COIL_LEN_MAX *1000 < m_dLen_BaseDsp ) {
		if(!m_bMaxLen_Dsp) {
			// コイル長上限到達
			if(NULL != mcls_pCoil->GetCoilBaseDsp()) {
				LOG(em_ERR), "[%s] 表示用コイル長上限到達 [コイルNo=%s]", my_sThreadName, mcls_pCoil->GetCoilBaseDsp()->data.cCoilNoIn );
				syslog(275, "[表示用コイル: コイルNo=%s]", mcls_pCoil->GetCoilBaseDsp()->data.cCoilNoIn);
//#ifndef PLG_DSP_KEN_EQAL		
				mcls_pStatus->SetStatusQue("MAXLEN_DSP_ID", false, false);
//#endif
				m_bMaxLen_Dsp = true;
				//// 強制コイル切替 
				//SetEvent(m_evCoilLenMaxDsp);

			}
		}
	}

	mcls_TimeDspSpeed.PutTimestamp(1);

	// 前回との時間差
	double dWk = mcls_TimeDspSpeed.GetTimestamp(1, 0);

	m_nSpeedDsp = (DWORD)(dDistance / ((double)(dWk / 1000.0)) + 0.5F);	// [mm/sec]

	LOGPLGDSP(em_NON), ", %.3f, %.3f, %.3f, %.3f, %.3f,%d,%d,%d, %d, %d, %d",
		m_dLen_BaseDsp/1000.0, m_dLen_Dsp[0]/1000.0, m_dLen_Dsp[1]/1000.0, m_dLen_Dsp[2]/1000.0, m_dLen_OutCut/1000.0,
		m_bCoilChange_Dsp[0]?1:0, m_bCoilChange_Dsp[1]?1:0, m_bCoilChange_Dsp[2]?1:0,
		GetSpeed(),	(int)dWk, (int)mcls_TimeDspSpeed.GetCycleTime());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// 共通

//==========================================
// 全部入り距離
//------------------------------------------
// int pos	
//==========================================
double PlgManager::GetLen_All(int pos)
{
	// 検査基点　検査位置(表面/裏面)
	if (pos == DIV_LEN_POS_KEN_TOP || pos == DIV_LEN_POS_KEN_BOT) {
		int nMen = pos - DIV_LEN_POS_KEN_TOP;
		return GetLen_Ken(nMen);
	}
	// 表示基点　表面検査台位置
	else if( pos == DIV_LEN_POS_DSP_TOP) {
		return GetLen_BaseDsp();

	}
	// 表示基点　裏面検査台位置
	else if (pos == DIV_LEN_POS_DSP_BOT) {
		return GetLen_BaseDsp();

	}
	// 検査台位置
	else {
		return GetLen_Dsp((pos - DIV_DISP_POS_DSP_P1));
	}
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 切り替え

//------------------------------------------
// 検査基点でコイル切り替え
//------------------------------------------
void PlgManager::CoilChange_KenBase()
{
	m_dLen_WpdKenPer = mcls_pParam->GetCommonMast().Dist.nKenBase;
}

//------------------------------------------
// 検査装置位置でコイル切り替え
// int men 0～
//------------------------------------------
void PlgManager::CoilChange_Ken(int men)
{


	//// 最後の長さを送っておく
	OpSendFunc::SendOp_Length();

	//================================================
	// 基準面の切り替え
	if( m_nDistPos == men ) {

		//// 情報を変更
		m_dCoilOldLen_Ken = m_dLen_Ken[m_nDistPos];		// 前回値 保持


		//// 基準面の情報
		m_nDly[m_nDistPos]				= 1;								// 遅延実行キーセット
		m_dLen_Ken[m_nDistPos]			= 0.0;								// 距離初期化
		m_dLen_KenSub					= 0.0;

		m_bCoilChange_Ken	= true;											// 切り替え中
		LOG(em_MSG), "[%s] 基準面検査位置 コイル切り替え 検査実測長=%0.3fm", my_sThreadName, m_dCoilOldLen_Ken/1000.0);

		//// パルスレート変更
		int idx = 0;
		if (m_dPlsR_Ken != mcls_pParam->GetParamCommon().dPls) {
			LOG(em_MSG), "[%s] 検査用パルスレート変更 %f→%f", my_sThreadName, m_dPlsR_Ken, mcls_pParam->GetParamCommon().dPls);
			syslog(276, "[検査用=%f→%f]", m_dPlsR_Ken, mcls_pParam->GetParamCommon().dPls);

			m_dPlsR_Ken	 = mcls_pParam->GetParamCommon().dPls;
			m_dResV		 = mcls_pParam->CalcResV( m_emIsLinePLG );
		}
		if (m_dPlsR_Sub != mcls_pParam->GetParamCommon().dPlsSub) {
			LOG(em_MSG), "[%s] 検査比較用パルスレート変更 %f→%f", my_sThreadName, m_dPlsR_Sub, mcls_pParam->GetParamCommon().dPlsSub);
			syslog(276, "[検査比較用=%f→%f]", m_dPlsR_Sub, mcls_pParam->GetParamCommon().dPlsSub);

			m_dPlsR_Sub = mcls_pParam->GetParamCommon().dPlsSub;
		}

		//// コイル長状態チェック
		if( m_bMaxLen_Ken ) mcls_pStatus->SetStatusQue("MAXLEN_KEN_ID", true, false);
		m_bMaxLen_Ken = false;	
	} 
	
	//================================================
	// もう一方の面の切り替え
	else {
		double wk = m_dLen_Ken[m_nDistNotPos]/1000.0;				// 前回値

		// もう一方が前回値まで到達 = もう一方の位置までコイル切り替わりが来た
		m_nDly[m_nDistNotPos]		= 1;							// 遅延実行キーセット
		m_dLen_Ken[m_nDistNotPos]	= m_dLen_Ken[m_nDistPos] - m_nDistLen;	// 他方面のコイル先頭フレームは、0mとならない。表裏間距離がフレーム単位で割り切れない分の誤差をここで吸収。(表裏間距離を厳密に考慮する)
		
		m_bCoilChange_Ken			= false;
		LOG(em_MSG), "[%s] 他方面(%d) 検査位置 コイル切り替え 検査実測長=%0.3fm, 基準面実測長=%0.3fm, 今回のフレーム先頭位置=%0.3fm", my_sThreadName, men, wk, m_dCoilOldLen_Ken/1000.0, m_dLen_Ken[men]/1000.0);
	}
	
	syslog(259, "[%s]", DivNameManager::GetTorB(men));
}

//------------------------------------------
// 表示基点位置でコイル切り替え
//------------------------------------------
void PlgManager::CoilChange_BaseDsp()
{
	ParamManager::P_COMMON_MAST	const& prm	= mcls_pParam->GetCommonMast();

	//// 表示基点のWPDの距離を基準とする
	m_dCoilOldLen_BaseDsp	= m_dLen_BaseDsp;							// 前回値 保持
	m_dLen_BaseDsp			= 0.0;										// 距離初期化

	//// 各検査位置の情報更新
	double dWkLen[POS_DSP_END];
	dWkLen[POS_DSP_DSP1_P] = (double)prm.Dist.nDspBaseKensaTop;			// 検査台位置1（表検査台）
	dWkLen[POS_DSP_DSP2_P] = (double)prm.Dist.nDspBaseKensaBot;			// 検査台位置2（裏検査台）
	dWkLen[POS_DSP_SHR_I] = (double)prm.Dist.nDspSharCut;				// シャー位置

	for(int ii=0; ii<POS_DSP_END; ii++) {
		// 各表示位置
		double wk = m_dCoilOldLen_BaseDsp - dWkLen[ii];
		if(POS_DSP_DSP2_P !=ii) // 裏面検査台の時はログ表示なし
			LOG(em_MSG), "[%s] 表示位置[%d] 位置の距離変更 [%0.3fm]→[%0.3fm]", my_sThreadName, ii, m_dLen_Dsp[ii]/1000.0, wk/1000.0);	// 基本的に同じ値となるはず
		m_dLen_Dsp[ii]			= wk;										// 基点からどれだけずれているか
		m_bCoilChange_Dsp[ii]	= true;
	}
	
	//// パルスレート変更
	if (m_dPlsR_Dsp != mcls_pParam->GetParamCommon().dPlsDsp) {
		LOG(em_MSG), "[%s] 表示用パルスレート変更 %f→%f", my_sThreadName, m_dPlsR_Dsp, mcls_pParam->GetParamCommon().dPlsDsp);
		syslog(276, "[表示用=%f→%f]", m_dPlsR_Dsp, mcls_pParam->GetParamCommon().dPlsDsp);

		m_dPlsR_Dsp	 = mcls_pParam->GetParamCommon().dPlsDsp;
	}
  
	//// コイル長状態チェック
	if( m_bMaxLen_Dsp ) mcls_pStatus->SetStatusQue("MAXLEN_DSP_ID", true, false);
	m_bMaxLen_Dsp = false;
}

//------------------------------------------
// シャーカット位置で出側物理コイル切り替え (PIOとかで即時通知時)
// double dNow 現在の通板距離 [mm]
//------------------------------------------
void PlgManager::CoilChange_OutCut()
{
	m_dLen_OutCut = 0.0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//// 実績収集関連
//==========================================
// 最小区間の状態把握
//==========================================
void PlgManager::CheckIntervalStatus()
{
	//==============================================
	//// 最小区間ピッチで機器状態をサンプリングする
	int				row;		// セット位置
	EM_DIV_KENSA	emKensa;	// 検査状態ワーク
	EM_DIV_KIKI		emKiki;		// 機器状態ワーク

	for (int ii = 0; ii<NUM_MEN; ii++) {
		row = (int)(m_dLen_Ken[ii] / (MAP_REPRES_MIN * 1000));
		if (0>row || MAP_ROW_NUM <= row) continue;				// 位置異常


																// 検査状態セット
		emKensa = mcls_pStatus->GetKensa(ii);
		mcls_pCoil->SetData_StKensa(ii, row, emKensa);
		// 機器状態セット
		emKiki = mcls_pStatus->GetKiki(ii);
		mcls_pCoil->SetData_StKiki(ii, row, emKiki);

#ifdef ENABLE_CYCLE			// 周期機能の状態をセット
			//// 周期機能の状態をチェック
			EM_DIV_CYCLESTAT  emCycStat;
			if (DIV_SYS_RUN != mcls_pStatus->GetCycleSys(ii)) {
				// 周期機能が止まっている場合は、対象外
				emCycStat = DIV_CYCLESTAT_STOP;
			}
			else {
				emCycStat = DIV_CYCLESTAT_OK;
			}

			// 周期機能状態セット
			mcls_pCoil->SetData_Row_R(ii, row, 0, emCycStat);
#endif
	}
}
