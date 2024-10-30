#include "StdAfx.h"
#include "HtRecvBaseManager.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// コンストラクタ
// const char *cSession セッション文字列
// int TorB 表示識別フラグ(0:表, 1:裏)
// bool bDistPostion 基準面の場合true
// int accept_num クライアント数 (物理回線数)
// int recv_num フレーム単位での受信数 (論理回線数)
//------------------------------------------
HtRecvBaseManager::HtRecvBaseManager(const char *cSession, int TorB, bool bDistPostion, int accept_num, int recv_num, int HtCy) :
KizuHtRecv( cSession, TorB, accept_num, recv_num),
m_bDistPostion(bDistPostion),
mcls_pLog(NULL),
mcls_pLock(NULL),
mcls_pStatus(NULL),
mcls_pParam(NULL),
m_nHtCy(HtCy),
ThreadManager(cSession, HT_RECV_EVENT_MAX)
{
	//// 所要時間関連 初期化

	//// 画像取込～判定→統括伝送完了までの平均、最大所要時間クリア
	m_dHtProcNowTime = 0.0;
	m_dHtProcAveTime = 0.0;
	m_dHtProcMaxTime = 0.0;

	//// 画像取込～統括処理完了までの平均、最大所要時間クリア
	m_dToProcNowTime = 0.0;
	m_dToProcAveTime = 0.0;
	m_dToProcMaxTime = 0.0;


	//// 画像処理エンジン処理完了～統括伝送開始までの平均、最大所要時間クリア
	for(int ii=0; ii<NUM_CAM_POS; ii++) {
		m_dNcHtProcNowTime[ii] = 0.0;
		m_dNcHtProcAveTime[ii] = 0.0;
		m_dNcHtProcMaxTime[ii] = 0.0;
	}


	//// DS/WSの疵情報受信時のタイムスタンプ
	m_pc_RecvFirst = 0;
	m_pc_RecvSecond = 0;


	//// DS(WS)の疵情報受信開始～WS(DS)の疵情報受信開始までの平均、最大所要時間クリア
	m_dRecvProcNowTime = 0.0;
	m_dRecvProcAveTime = 0.0;
	m_dRecvProcMaxTime = 0.0;


	//// DS(WS)の疵情報受信開始～WS(DS)の疵情報受信完了までの平均、最大所要時間クリア
	m_dRecvProcTotalNowTime = 0.0;
	m_dRecvProcTotalAveTime = 0.0;
	m_dRecvProcTotalMaxTime = 0.0;


	// パフォーマンスカウンタの周波数を取得
	QueryPerformanceFrequency((LARGE_INTEGER *)&m_pf);



	//// その他もろもろ初期化
	m_nPerFramePos = 0;
	m_nNowGridRow  = 0;
	m_nNowMinRow   = 0;

	m_nNowOutGridRow = 0;
	m_dNowOutLength  = 0.0;
	m_nFrameNo       = 0;

	m_nTrId = GetTrId(m_nHtCy, TorB);						// フレームトラッキングID
	m_nTrOtherId = GetTrOtherId(m_nTrId);					// 他方面のフレームトラッキングID
	memset(&m_trFrm, 0x00, sizeof(m_trFrm));				// フレームトラッキング情報

	// 基準面の把握
	if(bDistPostion) {
		m_nDistPos = TorB;
		m_nDistNotPos = TorB == 0 ? 1 : 0;
	} else {
		m_nDistNotPos = TorB;
		m_nDistPos = TorB == 0 ? 1 : 0;
	}

	// フレームスキップ & エッジ不正
	m_nFrameSkipId		= 0;
	m_bFrameSkipState	= false;
	m_evNowFrameSkip	= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evTimeFrameSkip	= CreateWaitableTimer(NULL, FALSE, NULL);

	m_nEdgeNgId			= 0;
	m_bEdgeNgState		= false;
	m_evNowEdgeNg		= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evTimeEdgeNg		= CreateWaitableTimer(NULL, FALSE, NULL);

	// その他シグナル
	m_evOpNewConnect	= CreateEvent(NULL, FALSE, FALSE, NULL);
}

//------------------------------------------
// デストラクタ
//------------------------------------------
HtRecvBaseManager::~HtRecvBaseManager(void)
{
	// フレームスキップ & エッジ不正
	CloseHandle(m_evNowFrameSkip);
	CloseHandle(m_evTimeFrameSkip);
	CloseHandle(m_evNowEdgeNg);
	CloseHandle(m_evTimeEdgeNg);

	CloseHandle(m_evOpNewConnect);
}


//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int HtRecvBaseManager::ThreadFirst()
{
	//// イベント設定 (動的追加)
	// 接続数が 最大8までは変更する必要なし
	AddEventNum(PARAM_SOCK_MAX, my_evRecv);
	AddEventNum(1, &my_evDetectWaitStart);
	AddEventNum(1, &my_evDetectRun);
	AddEventNum(1, &my_evDetectStop);
	AddEventNum(1, &my_evAccept);
	AddEventNum(PARAM_SOCK_MAX, my_evDisConnect);
	AddEventNum(PARAM_SOCK_MAX, my_evRecvTimeOut);
	
	AddEventNum(1, &m_evTimeFrameSkip);
	AddEventNum(1, &m_evNowFrameSkip);
	AddEventNum(1, &m_evTimeEdgeNg);
	AddEventNum(1, &m_evNowEdgeNg);

	AddEventNum(1, &m_evOpNewConnect);

	//// イベント追加
	ThreadFirst_AddHandle();


	//HANDLE hArray[] = { my_evRecv[0],				// ここの配列関係だけ、台数分調整する必要あり
	//					my_evDetectWaitStart,
	//					my_evDetectRun,
	//					my_evDetectStop,
	//					my_evAccept,
	//					my_evDisConnect[0],
	//					my_evRecvTimeOut[0]
	//					};
	//SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);


	//// ソケット生成 + Listen (成功するまでブロック)
	SetSockTID( GetTid() );
	while( TRUE ) {
		if( Listen(true) ) break;
		Sleep(2000);
	}
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int HtRecvBaseManager::ThreadLast()
{
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void HtRecvBaseManager::ThreadEvent(int nEventNo)
{
	int id = 0;										// 物理回線ID (0オリジン)
	int pcid = 0;									// 判定機能ID (1オリジン)
	int recvid = 0;									// 論理回線ID (0オリジン)

	////// シグナルの条件
	enum {	EV_RECV				= 0,								// 受信完了通知			(PARAM_SOCK_MAX分)
			EV_DETECT_WAITSTART	= EV_RECV + PARAM_SOCK_MAX,			// 検査開始待ち要求
			EV_DETECT_RUN,											// 検査開始要求
			EV_DETECT_STOP,											// 検査停止要求
			EV_ACCEPT,												// アクセプト通知
			EV_DISCONNECT,											// 切断通知				(PARAM_SOCK_MAX分)
			EV_RECVTIMEOUT		= EV_DISCONNECT+PARAM_SOCK_MAX,		// 受信タイムアウト		(PARAM_SOCK_MAX分)
			EV_TIME_FRAME_SKIP	= EV_RECVTIMEOUT+PARAM_SOCK_MAX,	// フレームスキップ終了
			EV_NOW_FRAME_SKIP,										// フレームスキップ発生

			EV_TIME_EDGE_NG,										// エッジ不正終了
			EV_NOW_EDGE_NG,											// エッジ不正発生

			EV_OP_NEW_CONNECT,										// 新規表示機能新規接続

			EV_LAST
	};

	////// シグナル条件分け
	
	//======================================================
	// 受信完了通知 (PARAM_SOCK_MAX分)
	if(nEventNo >= EV_RECV && nEventNo < EV_RECV+PARAM_SOCK_MAX) {
		id = nEventNo - EV_RECV;
		OnRecvEnd(id);
	

	//======================================================
	// 検査開始待ち要求
	} else if(nEventNo == EV_DETECT_WAITSTART)	{
		LOG(em_MSG), "[%s] <%s> 検査開始待ち要求", my_sThreadName, DivNameManager::GetTorB(my_nTorB) );
		ExecDetectWaitStart();


	//======================================================
	// 検査開始要求
	} else if(nEventNo == EV_DETECT_RUN)	{
		LOG(em_MSG), "[%s] <%s> 検査開始要求", my_sThreadName, DivNameManager::GetTorB(my_nTorB) );
		ExecDetectRun();


	//======================================================
	// 検査停止要求
	} else if(nEventNo == EV_DETECT_STOP)	{
		LOG(em_MSG), "[%s] <%s> 検査停止要求", my_sThreadName, DivNameManager::GetTorB(my_nTorB) );
		ExecDetectStop();
		// 正常切断
		this->CloseAll(true); 


	//======================================================
	// アクセプト通知
	} else if(nEventNo == EV_ACCEPT)	{
		LOG(em_MSG), "[%s] <%s> アクセプト シグナル", my_sThreadName, DivNameManager::GetTorB(my_nTorB) );

		id = this->Accept();
		if( id < 0 ) return;

		// 受信開始 (イニシャル伝文)
		if( ! this->RecvHeaderInitialize(id) ) {
			LOG(em_ERR), "[%s] <%s> イニシャル伝文 受信開始異常", my_sThreadName, DivNameManager::GetTorB(my_nTorB));
			this->CloseAll();
			return;
		}


	//======================================================
	// 切断通知 (PARAM_SOCK_MAX分)
	} else if(nEventNo >= EV_DISCONNECT && nEventNo < EV_DISCONNECT+PARAM_SOCK_MAX) {
		id = nEventNo - EV_DISCONNECT;
		pcid = my_PC[ id ] - 1;

		if( DIV_SYS_RUN == mcls_pStatus->GetSys( my_nTorB ) ) {
			LOG(em_ERR), "[%s] <%s> 検査中にいきなり回線切断！！ <%s> mode=%d ID=%d PCID=%d", my_sThreadName, DivNameManager::GetTorB(my_nTorB), my_cClientName[pcid], mcls_pSock->GetCloseKindID(id), id, pcid+1 );
			syslog(244, "[%s %s mode=%d]", DivNameManager::GetTorB(my_nTorB), my_cClientName[pcid], mcls_pSock->GetCloseKindID(id) );
			
		} else {
			LOG(em_MSG), "[%s] <%s> 回線切断 <%s>", my_sThreadName, DivNameManager::GetTorB(my_nTorB), my_cClientName[pcid] );
		}
		this->CloseAll();


	//======================================================
	// 受信タイムアウト (PARAM_SOCK_MAX分)
	} else if(nEventNo >= EV_RECVTIMEOUT && nEventNo < EV_RECVTIMEOUT+PARAM_SOCK_MAX) {
		// 何もしない

	//======================================================
	// フレームスキップ
	} else if(nEventNo == EV_TIME_FRAME_SKIP )	{		// 終了
		FrameSkipEnd();

	} else if(nEventNo == EV_NOW_FRAME_SKIP)	{		// 開始
		FrameSkipStart();


	//======================================================
	// エッジ不正
	} else if(nEventNo == EV_TIME_EDGE_NG )	{		// 終了
		EdgeNgEnd();

	} else if(nEventNo == EV_NOW_EDGE_NG)	{		// 開始
		EdgeNgStart();

	//======================================================
	// 新規表示機能新規接続
	} else if(nEventNo == EV_OP_NEW_CONNECT)	{
		OnOpNewConnect();

	//======================================================
	// それ以外。継承先で追加したシグナル
	} else {
		// 継承先に通知
		OnAddSignal( nEventNo - EV_LAST );
	}
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 標準処理

//------------------------------------------
// 受信完了通知
// int id 物理回線ID (0オリジン)
//------------------------------------------
void HtRecvBaseManager::OnRecvEnd(int id) 
{
	int recvid = 0;									// 論理回線ID (0オリジン)


	switch( my_emSockState[id] ) {
	
	case KizuHtRecv::STATE_HEAD:								// ヘッダー受信

		//// イニシャル伝文？
		if( -1 == my_PC[id] ) {									// イニシャル伝文
			if( ! this->AnsInitializeSocket(id) ) {
				LOG(em_ERR), "[%s] <%s> イニシャル伝文 受信異常", my_sThreadName, DivNameManager::GetTorB(my_nTorB));
				CloseAll();
			}
			return;
		
		//// 業務伝文 (1フレームのヘッダー部受信)
		} else {

			// ヘルシー伝文?  ※ヘルシー伝文といいつつも、実体はフレーム単位でなく単発の伝文受信
			if( 0 == memcmp( my_cRecvWkHead[ id ].base_head.code, T_CODE_HELCY, SIZE_SOCK_TC) ) {

#ifdef DEBUG_LOG_HT_HEALTHY
				LOG(em_FIL), "[%s] <%s> 業務伝文ヘルシー", my_sThreadName, DivNameManager::GetTorB(my_nTorB));
#endif
				// 継承先で処理
				OnMainHls( &my_cRecvWkHead[ id ]);

				// 次受信開始
				if( ! this->RecvHeaderDefect(id) ) {	
					LOG(em_ERR), "[%s] <%s> ヘルシー受信後の次受信開始失敗", my_sThreadName, DivNameManager::GetTorB(my_nTorB));
					CloseAll();	
				}
				return;
			}

			// ヘッダー部受信完了処理
			recvid = this->AnsHeader(id);
			if( 0 > recvid ) {
				LOG(em_ERR), "[%s] <%s> 業務伝文ヘッダー部 受信異常", my_sThreadName, DivNameManager::GetTorB(my_nTorB));
				return;
			}
			
			// フレームNoチェック
			if( ! this->CheckFrameNo( ((HT_TO_HEAD*)&my_cRecvHead[ recvid ])->nFrameNo, id, recvid ) ) {
				LOG(em_ERR), "[%s] <%s> 業務伝文ヘッダー部のフレームNo異常", my_sThreadName, DivNameManager::GetTorB(my_nTorB));
				syslog(243, "[%s: recvid=%d pre=%d now=%d]", my_cClientName[my_PC[id] - 1], recvid, my_nFrameNo[recvid], ((HT_TO_HEAD *)&my_cRecvHead[recvid])->nFrameNo);
				return;
			}

			// 先行受信からの時間計測
			int nRecvCnt	= 0;
			for(int ii=0; ii<my_nRecvNum; ii++) {
				if( my_nFrameNo[ii] == ((HT_TO_HEAD*)&my_cRecvHead[ recvid ])->nFrameNo ) nRecvCnt++;
			}
			if(1 == nRecvCnt)		QueryPerformanceCounter( (LARGE_INTEGER*)&m_pc_RecvFirst );
			else					QueryPerformanceCounter( (LARGE_INTEGER*)&m_pc_RecvSecond );


			// 業務伝文 受信後のメイン処理
			if( this->CheckAllReadEnd() )  {
				MainDefect();
			}
		}
		break;

	case KizuHtRecv::STATE_DATA:								// データ部受信
		// 業務伝文 受信完了チェック
		this->AnsData(id);
		
		// 業務伝文 受信後のメイン処理
		if( this->CheckAllReadEnd() )  {
			MainDefect();
		}
		break;
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// シーケンス制御

//------------------------------------------
// 検査開始待ち開始処理
//------------------------------------------
void HtRecvBaseManager::ExecDetectWaitStart()
{
	//// 繋がってないはずだが、念の為に 切断処理
	this->CloseAll(true);

	//// もろもろ初期化
	m_nPerFramePos = 0;
	m_nNowGridRow  = 0;
	m_nNowMinRow   = 0;
	//m_nNowKizuSeq  = 0;


	m_nNowOutGridRow = 0;
	m_dNowOutLength  = 0.0;
	m_nFrameNo       = 0;

	// 基準面の方が先に進んでいる事とする。	// OnExecDetectWaitStart() ここで ちゃんとした値で初期化すること
	//if( m_bDistPostion ) {
	//	m_nNowGridRow = m_nDistLen / (MAP_REPRES_MIN*1000);
	//} else {
	//	m_nNowGridRow = 0;
	//}


	//// 機器状態初期化
	if( 0 != m_nFrameSkipId ) {
		mcls_pStatus->SetStatusQue(m_nFrameSkipId, true, false);
	}

	//// 検査する判定PCのクライアント名を、機能の順番に セットする
	for(int ii=0; ii<my_nAcceptNum; ii++) {
		// コンピューター名をセット
		int index = ii + this->my_nTorB*my_nAcceptNum + ((m_nHtCy-1)*NUM_MEN*NUM_HANTEI);
		this->SetCliantName(ii, OnGetPcName(index) );
		LOG(em_MSG), "[%s] <%s> 判定PCクライアント名登録[%s]", my_sThreadName, DivNameManager::GetTorB(my_nTorB), OnGetPcName(index) );
	}

	//// 継承先に通知
	OnExecDetectWaitStart();

	// この後、各判定よりコネクトされてくるはず…
}

//------------------------------------------
// 検査開始処理
//------------------------------------------
void HtRecvBaseManager::ExecDetectRun()
{
	//// パラメータをセット
	EM_DIV_PLG emIsLinePLG = mcls_pStatus->GetPlg();
	m_dResV	= mcls_pParam->CalcResV( (int)emIsLinePLG );

	//// 時間計測準備
	mcls_Time.InitAve();

	//// 画像取込～判定→統括伝送完了までの平均、最大所要時間クリア
	m_dHtProcNowTime = 0.0;
	m_dHtProcAveTime = 0.0;
	m_dHtProcMaxTime = 0.0;

	//// 画像取込～統括処理完了までの平均、最大所要時間クリア
	m_dToProcNowTime = 0.0;
	m_dToProcAveTime = 0.0;
	m_dToProcMaxTime = 0.0;

	//// 画像処理エンジン処理完了～統括伝送開始までの平均、最大所要時間クリア
	for(int ii=0; ii<NUM_CAM_POS; ii++) {
		m_dNcHtProcNowTime[ii] = 0.0;
		m_dNcHtProcAveTime[ii] = 0.0;
		m_dNcHtProcMaxTime[ii] = 0.0;
	}

	//// DS/WSの疵情報受信時のタイムスタンプ
	m_pc_RecvFirst = 0;
	m_pc_RecvSecond = 0;

	//// DS(WS)の疵情報受信開始～WS(DS)の疵情報受信開始までの平均、最大所要時間クリア
	m_dRecvProcNowTime = 0.0;
	m_dRecvProcAveTime = 0.0;
	m_dRecvProcMaxTime = 0.0;
	//// DS(WS)の疵情報受信開始～WS(DS)の疵情報受信完了までの平均、最大所要時間クリア
	m_dRecvProcTotalNowTime = 0.0;
	m_dRecvProcTotalAveTime = 0.0;
	m_dRecvProcTotalMaxTime = 0.0;


	//// 探傷開始
	this->DefectStart(); 
	
	//// 継承先に通知
	OnExecDetectRun();

	//// 受信開始
	if( ! this->RecvAllHeader() ) {
		LOG(em_ERR), "[%s] <%s> 探傷開始失敗！！", my_sThreadName, DivNameManager::GetTorB(my_nTorB));
		return;
	}
}

//------------------------------------------
// 検査停止処理
//------------------------------------------
void HtRecvBaseManager::ExecDetectStop()
{
	//// 探傷停止
	this->DefectStop();

	//// 継承先に通知
	OnExecDetectStop();
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// メイン処理
//------------------------------------------
// 業務伝文 受信後のメイン処理
//------------------------------------------
void HtRecvBaseManager::MainDefect()
{
	/////////////////////////////////////////////////////////////////////
	// 事前準備

	// 処理時間計測開始
	mcls_Time.CounterStart();

	// ブロック
	// ※ コイル切り替え等している時は、ちょっと待ち)
	mcls_pLock->BlockTh();

	// フレームトラッキング情報 取り出し & 削除
	HT_TO_HEAD& hth = (HT_TO_HEAD&)this->my_cRecvHead[0];	
	bool bRet;
	for(int ii=0; ii<TRACKING_ITEM_REPEAT_NUM; ii++) {
		bRet = mcls_pTrFrm->GetDelItem(m_nTrId, hth.nFrameNo, &m_trFrm);
		// もし成功なら次へ進む
		if(bRet) break;
		// 検査停止後に受信したフレームについては紐づけを無視する
		if (DIV_KADOU_STOP == mcls_pStatus->GetKadou()) {
			bRet = !m_bFrameUnmatchState;				// 前回値をセット
			break;
		}
		// 失敗なら待機後再チャレンジ
		LOG(em_ERR), "[%s] <%s> フレームトラッキングずれ発生中のため待機・・・[Fno=%d]", my_sThreadName, DivNameManager::GetTorB(my_nTorB), hth.nFrameNo);
		if(ii >= TRACKING_ITEM_REPEAT_NUM) break;
		Sleep(TRACKING_ITEM_SLEEP);
	}

	// フレームずれ機器状態変更
	if(0 != m_nFrameUnmatchId) {
		if (!bRet && !m_bFrameUnmatchState) {
			// 機器状態セット
			m_bFrameUnmatchState = true;
			mcls_pStatus->SetStatusQue(m_nFrameUnmatchId, false, false);	// フレームNoずれ
			mcls_pStatus->SetStatusRead();									// 非同期強制読み込みシグナルセット (すぐに状態を通知したいため)
			LOG(em_ERR), "[%s] <%s> フレームトラッキングずれ異常[Fno=%d]", my_sThreadName, DivNameManager::GetTorB(my_nTorB), hth.nFrameNo);
			syslog(245, "[%s, Fno=%d]", DivNameManager::GetTorB(my_nTorB), hth.nFrameNo);

			// シーケンサフレーム受信遅れ
			mcls_pStatus->AddQueMyErrWar(true, ON_SEQ_FRAME_DELAY);
		}
		else if(bRet && m_bFrameUnmatchState){
			// 機器状態セット
			m_bFrameUnmatchState = false;
			mcls_pStatus->SetStatusQue(m_nFrameUnmatchId, true, false);		// フレームNoずれ
			mcls_pStatus->SetStatusRead();									// 非同期強制読み込みシグナルセット (すぐに状態を通知したいため)
			LOG(em_MSG), "[%s] <%s> フレームトラッキングずれ正常[Fno=%d]", my_sThreadName, DivNameManager::GetTorB(my_nTorB), hth.nFrameNo);
		}
	}

	if(!bRet) memset(&m_trFrm, 0x00, sizeof(m_trFrm));
	int nMenMode	= mcls_pStatus->GetMenState();					// 検査面状態(-1:両面検査 0:表のみで検査 1:裏のみで検査)
	if(-1 != nMenMode) mcls_pTrFrm->DelItem(m_nTrOtherId, hth.nFrameNo);	// 片面検査時は、他方面の削除も実施

	/////////////////////////////////////////////////////////////////////
	// 前処理
	//

	/////////////////////////////////////////////////////////////////////
	// 継承先で実行させる処理
	OnMainDefect();

	/////////////////////////////////////////////////////////////////////
	// 後処理
	//

	// 距離更新
	//m_nPerFramePos = hth.frame_pos;								// 前回フレームの長手位置 [mm]
	m_nNowGridRow  = hth.frame_pos / (MAP_REPRES_MIN*1000);		// 次回の送信マップ位置
	m_nNowMinRow   = hth.frame_pos / (1*1000);					// 最小距離

	int nLen = (int)(hth.frame_pos + m_dOneFrameLen);
	m_nPerFramePos = nLen;								// フレームの長手位置 [mm]

	bool bCoilChange	= false;
	//if( 1 == hth.toVal[TOVAL_TOP_COILCHANGE] && 0 == my_nTorB )	bCoilChange = true;
	//if( 1 == hth.toVal[TOVAL_BOT_COILCHANGE] && 1 == my_nTorB )	bCoilChange = true;
	if(1 == m_trFrm.nCoilChg[my_nTorB]) bCoilChange = true;

	if(bCoilChange) {
		// コイル切替時、縦分解能を再設定(パルスレート変更を考慮)
		EM_DIV_PLG emIsLinePLG = mcls_pStatus->GetPlg();
		m_dResV	= mcls_pParam->CalcResV( (int)emIsLinePLG );
		SetOneFrameLen( mcls_pParam->CalcResV((int)emIsLinePLG)*SIZE_FRAME_IMG_Y );
	}

	if( hth.nFrameNo != m_nFrameNo ){
		m_nFrameNo = hth.nFrameNo;
		//if( 1 == hth.toVal[TOVAL_LCUT_COILOUTCHANGE] ){
		if(1 == m_trFrm.nFCut) {
			m_nNowOutGridRow = 0;
			m_dNowOutLength  = 0.0;
		} else {
			m_dNowOutLength += (double)(SIZE_FRAME_IMG_Y) * m_dResV / 1000.0;
			m_nNowOutGridRow = (int)m_dNowOutLength;		// 出側物理単位の通板距離[m]
		}
	}

	// 次のフレームを読込
	if( ! this->RecvAllHeader() ) {
		LOG(em_ERR), "[%s] <%s> 次フレーム読込異常", my_sThreadName, DivNameManager::GetTorB(my_nTorB));
		// 強制リトライ発動
		CloseAll();
	}

	// 処理時間計測終了
	mcls_Time.CounterEnd();
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// フレームスキップ
//------------------------------------------
// フレームスキップ 開始
//------------------------------------------
void HtRecvBaseManager::FrameSkipStart()
{
	// 既にスキップ中であれば 機器状態に登録はしない
	if( ! m_bFrameSkipState ) {
		// 機器状態セット
		m_bFrameSkipState = true;
		mcls_pStatus->SetStatusQue(m_nFrameSkipId, false, false);		// スキップ
		mcls_pStatus->SetStatusRead();							// 非同期強制読み込みシグナルセット (すぐに状態を通知したいため)
	}

	// 外部公開シグナル
	__int64 ts = (__int64)TIME_FRAME_SKIP_WAIT * -10000;
	SetWaitableTimer(m_evTimeFrameSkip, (LARGE_INTEGER*)&ts, 0, NULL, NULL, FALSE);
}

//------------------------------------------
// フレームスキップ 完了
//------------------------------------------
void HtRecvBaseManager::FrameSkipEnd()
{
	m_bFrameSkipState = false;
	mcls_pStatus->SetStatusQue(m_nFrameSkipId, true, false);		// スキップ
	mcls_pStatus->SetStatusRead();							// 非同期強制読み込みシグナルセット (すぐに状態を通知したいため)
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// エッジ不正
//------------------------------------------
// エッジ不正 開始
//------------------------------------------
void HtRecvBaseManager::EdgeNgStart()
{
	// 既にスキップ中であれば 機器状態に登録はしない
	if( ! m_bEdgeNgState ) {
		// 機器状態セット
		m_bEdgeNgState = true;
		mcls_pStatus->SetStatusQue(m_nEdgeNgId, false, false);		// エッジ不正
		mcls_pStatus->SetStatusRead();								// 非同期強制読み込みシグナルセット (すぐに状態を通知したいため)
	}

	// 外部公開シグナル
	__int64 ts = (__int64)TIME_EDGE_NG_WAIT * -10000;
	SetWaitableTimer(m_evTimeEdgeNg, (LARGE_INTEGER*)&ts, 0, NULL, NULL, FALSE);
}

//------------------------------------------
// エッジ不正 完了
//------------------------------------------
void HtRecvBaseManager::EdgeNgEnd()
{
	m_bEdgeNgState = false;
	mcls_pStatus->SetStatusQue(m_nEdgeNgId, true, false);		// エッジ不正
	mcls_pStatus->SetStatusRead();								// 非同期強制読み込みシグナルセット (すぐに状態を通知したいため)
}
