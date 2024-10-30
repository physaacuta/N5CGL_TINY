#include "StdAfx.h"
#include "StatusBaseManager.h"


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
StatusBaseManager::StatusBaseManager(void) :
ThreadManager("StatusMgr"),
m_pCoilBase(NULL),
mcls_pStatusFunc(NULL),
mcls_pKiki(NULL)
{
	int ii;

	//// 初期化
	InitializeCriticalSection(&m_csLock);						// クリティカルセクションオブジェクトを初期化

	//// ステータス情報 初期化
	m_bMenMode	= false;
	m_nMenState = -1;
	m_emPlg		= DIV_PLG_LINE;
	m_emUnten	= DIV_UNTEN_STOP;
	m_emSogyo	= DIV_SOGYO_NON;
#ifdef DSP_SOGYO_CHECK
	m_emSogyoDsp = DIV_SOGYO_NON;
#endif
	m_emKadou	= DIV_KADOU_INIT;
	for(ii=0; ii< NUM_MEN; ii++) {
		m_emSys[ii]		 = DIV_SYS_INIT;
		m_emCycleSys[ii] = DIV_SYS_INIT;
		m_emParaSys[ii]	 = DIV_SYS_INIT;
		m_emKensa[ii]	 = DIV_KENSA_NON; //DIV_KENSA_OK;
		m_emKiki[ii]	 = DIV_KIKI_OK;
	}

	//// 機器状態把握
	memset( &m_oldNowSystem, 0x00, sizeof(m_oldNowSystem));
	memset( &m_oldStateRead, 0x00, sizeof(m_oldStateRead));

	//// 機器状態インスタンス生成
	mcls_pKiki = new KikiManager(PARAM_DB_READ_INTERVAL_TIME);
}

//------------------------------------------
// デストラクタ
//------------------------------------------
StatusBaseManager::~StatusBaseManager(void)
{
	//// 機器状態スレッド開放
	SAFE_DELETE(mcls_pKiki);

	//// 開放
	DeleteCriticalSection(&m_csLock);							// クリティカルセクションオブジェクトを開放
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int StatusBaseManager::ThreadFirst()
{
	//// 機器状態開始
	mcls_pKiki->SetLogMgr(mcls_pLog);
	if(m_bDspSyslog) mcls_pKiki->SetOutPutLog();						// 探傷画面へ故障ログ表示
	mcls_pKiki->CheckBitSysArea(0);
	mcls_pKiki->Start();	// 最初はログが出なくてもいい

	

	//// イベント設定
	HANDLE hWk = mcls_pKiki->GetEvReadEnd();
	AddEventNum(1, &hWk);

	//// イベント追加
	ThreadFirst_AddHandle();
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int StatusBaseManager::ThreadLast()
{
	//// 機器状態停止
	mcls_pKiki->Stop();
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void StatusBaseManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件		
	enum {	EV_READEND = 0,								// 機器状態チェック完了
			EV_LAST
			};

	////// シグナル条件分け
	//======================================================
	// 機器状態チェック完了
	if(nEventNo == EV_READEND) {
		ReadKikiStatus();

	//======================================================
	// それ以外。継承先で追加したシグナル
	} else {
		//// 継承先に通知
		OnAddSignal( nEventNo - EV_LAST );
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 機器状態把握

//------------------------------------------
// 機器状態取得 & 状態チェック
//------------------------------------------
void StatusBaseManager::ReadKikiStatus()
{
	int ii;
	TYP_STATUS_NOWSYSTEM		typStatus;						// ステータス情報

	// 機器状態クラスより機器状態情報を取得
	mcls_pKiki->GetStatusNow( &typStatus );


	////////////////////////////////////
	// 現状把握
	////////////////////////////////////
	// 現機器状態と比較
	for(ii=0; ii< NUM_MEN; ii++) {
		//// 検査状態
		if( m_emKensa[ii] != typStatus.kensa[ii] ) {

			if( DIV_KENSA_OK == typStatus.kensa[ii]) {					// 正常になった
				if( DIV_KENSA_SKIP == m_emKensa[ii] || DIV_KENSA_DBNON == m_emKensa[ii] ) {
				} else {
					LOG(em_MSG), "[%s] %s 検査状態変更 [%s → %s]", my_sThreadName, 
						DivNameManager::GetTorB(ii), DivNameManager::GetDivKensa(m_emKensa[ii]), DivNameManager::GetDivKensa(typStatus.kensa[ii]) );
				}
			} else {													// 異常になった
				if( DIV_KENSA_SKIP == typStatus.kensa[ii]) {
				} else if( DIV_KENSA_DBNON == typStatus.kensa[ii]) {
				} else if( DIV_KENSA_EDGENG == typStatus.kensa[ii]) {
				} else {
					if( DIV_KENSA_SKIP == m_emKensa[ii] || DIV_KENSA_DBNON == m_emKensa[ii] || DIV_KENSA_EDGENG == m_emKensa[ii] ) {
					} else {
						LOG(em_ERR), "[%s] %s 検査状態変更 [%s → %s]", my_sThreadName, 
							DivNameManager::GetTorB(ii), DivNameManager::GetDivKensa(m_emKensa[ii]), DivNameManager::GetDivKensa(typStatus.kensa[ii]) );
					}
				}
			}


			//// シスログ出力
			CString		sLog1;
			CString		sLog2;
			sLog1.Format("%s 前回<%s>", DivNameManager::GetTorB(ii), DivNameManager::GetDivKensa(m_emKensa[ii]) );
			if(NULL != m_pCoilBase) {
				sLog2.Format(" (%s)(%s)", m_cKizukenNo, m_pCoilBase->cCoilNoIn);
			}
			if(DIV_KENSA_OK == typStatus.kensa[ii])		{ syslog(PARAM_SYSLOG_BASENO+10, "%s%s", sLog1, sLog2); }	// 310
			if(DIV_KENSA_NG == typStatus.kensa[ii])		{ syslog(PARAM_SYSLOG_BASENO+11, "%s%s", sLog1, sLog2); }	// 311
			if(DIV_KENSA_EDGENG == typStatus.kensa[ii]) { syslog(PARAM_SYSLOG_BASENO+14, "%s%s", sLog1, sLog2); }	// 314
			if(DIV_KENSA_DBNON == typStatus.kensa[ii])	{ syslog(PARAM_SYSLOG_BASENO+15, "%s%s", sLog1, sLog2); }	// 315
			if(DIV_KENSA_SKIP == typStatus.kensa[ii])	{ syslog(PARAM_SYSLOG_BASENO+16, "%s%s", sLog1, sLog2); }	// 316
			if(DIV_KENSA_STOP == typStatus.kensa[ii])	{ syslog(PARAM_SYSLOG_BASENO+19, "%s%s", sLog1, sLog2); }	// 319


			// 反映
			m_emKensa[ii] = (EM_DIV_KENSA)typStatus.kensa[ii];
		}

		//// 機器状態
		if( m_emKiki[ii] != typStatus.kiki[ii] ) {
			// 状態変更
			if( DIV_KIKI_OK == typStatus.kiki[ii]) {					// 正常になった
				LOG(em_MSG), "[%s] %s 機器状態変更 [%s → %s]", my_sThreadName,
					DivNameManager::GetTorB(ii), DivNameManager::GetDivKiki(m_emKiki[ii]), DivNameManager::GetDivKiki(typStatus.kiki[ii]) );


			} else {													// 異常になった
				LOG(em_ERR), "[%s] %s 機器状態変更 [%s → %s]", my_sThreadName, 
					DivNameManager::GetTorB(ii), DivNameManager::GetDivKiki(m_emKiki[ii]), DivNameManager::GetDivKiki(typStatus.kiki[ii]) );
			}
			// 反映
			m_emKiki[ii] = (EM_DIV_KIKI)typStatus.kiki[ii];
		}
	}

	////////////////////////////////////
	// 変更分を格納
	////////////////////////////////////
	bool bChange = false;		// 前回と比べて変更有り？

	//// 前回値と状態が違う？ (異常の項目のみしかチェックできないよ)	
	for(ii=0; ii< NUM_MEN; ii++) {
		// 前回と違っている？
		if( !bChange && m_oldNowSystem.kensa[ii] != typStatus.kensa[ii] ) bChange = true;
		if( !bChange && m_oldNowSystem.kiki [ii] != typStatus.kiki [ii] ) bChange = true;
		if( !bChange && m_oldNowSystem.nCnt		 != typStatus.nCnt )	  bChange = true;		// 優先度900番代対応
		for(int jj=0; jj<typStatus.nCnt; jj++ ) {
			if( ! bChange && m_oldNowSystem.ngInf[jj].id != typStatus.ngInf[jj].id && 
				( 0 == typStatus.ngInf[jj].area || ii+1 == typStatus.ngInf[jj].area )) bChange = true;
		}	
	}
	// 先頭メッセージのみ変更対応
	if( ! bChange && m_oldNowSystem.ngInf[0].id != typStatus.ngInf[0].id ) bChange = true;


	//// 今回値セット
	EnterCriticalSection(&m_csLock);
	memcpy( &m_oldNowSystem, &typStatus, sizeof(typStatus));
	mcls_pKiki->GetStatusInf( (TYP_STATUS_INF*)&m_oldStateRead, sizeof(m_oldStateRead));
	LeaveCriticalSection(&m_csLock);


	////////////////////////////////////
	// エンド処理
	////////////////////////////////////
	//// ステータス情報チェック
	if(m_bAddStlog) CheckStatusLog();

	//// 継承先へ
	OnReadKikiStatusEnd(bChange);
}


//------------------------------------------
// ステータス情報チェック
//------------------------------------------
void StatusBaseManager::CheckStatusLog()
{
	if( NULL == mcls_pStatusFunc) return;
	StatusLogFunc::TYP_STATUS_LOG	typStatus;
	memset(&typStatus, 0x00, sizeof(typStatus));

	//--------------------------------
	// 今回情報セット
	typStatus.Data.emKadou = m_emKadou;
	for(int ii=0; ii<NUM_MEN; ii++) {
		typStatus.Data.emKensa[ii]	= m_oldNowSystem.kensa[ii];
		typStatus.Data.emKiki[ii]	= m_oldNowSystem.kiki[ii];
	}
	memcpy(typStatus.Data.cMsg, m_oldNowSystem.ngInf[0].msg, sizeof(typStatus.Data.cMsg)); 

	// 拡張分
	if(NULL != m_pCoilBase) {
		memcpy(typStatus.cKizukenNo, m_cKizukenNo, SIZE_KIZUKEN_NO );
		memcpy(typStatus.cCoilNo, m_pCoilBase->cCoilNoIn, sizeof(m_pCoilBase->cCoilNoIn));
	}

	//// ステータスログクラスに渡す
	mcls_pStatusFunc->CheckStatusLog(&typStatus);
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ステータス制御

//------------------------------------------
// シーケンスが行える状態か
// 戻り値 true:シーケンス可能 false:シーケンス不能
//------------------------------------------
bool StatusBaseManager::IsDoSequence()
{
	// 稼動状態：稼働中 & (運転状態：通常運転 or シミュレーション)
	if( IsRun() &&
		(	DIV_UNTEN_NOMAL == GetUnten() ||
			DIV_UNTEN_SMYU  == GetUnten()	)
	  ) return true;
	return false;
}

//------------------------------------------
// 片面検査有無変更
// bool st 許可:true / 不許可:false 
//------------------------------------------
bool StatusBaseManager::SetMenMode(bool st)
{
	////// メインインスタンス取り出し
	bool	old;
	old = GetMenMode();
	if( old == st ) return false;

	//// クリティカルセクション突入
	AutoLock lock(&m_csLock);

	//// 変更ステータスにより処理分け
	m_bMenMode = st;
	syslog(PARAM_SYSLOG_BASENO+0, "[<%s> ⇒ <%s>]", GetMenModeName(old), GetMenModeName(st));	// 300
	LOG(em_MSG), "<<片面検査有無変更>> [<%s> ⇒ <%s>]", GetMenModeName(old), GetMenModeName(st)); 
	return true;

}
//------------------------------------------
// 検査面状態変更
// int st 実施状態 (-1:両面検査 0:表のみで実施 1:裏のみで実施)
//------------------------------------------
bool StatusBaseManager::SetMenState(int st)
{
	////// メインインスタンス取り出し
	int	old;
	old = GetMenState();
	if( old == st ) return false;

	//// クリティカルセクション突入
	AutoLock lock(&m_csLock);

	//// 変更ステータスにより処理分け
	m_nMenState = st;
	if( -1 == st ) {
		syslog(PARAM_SYSLOG_BASENO+1, "[両面検査]"); // 301
		LOG(em_MSG), "<<検査面状態変更>> [両面検査]"); 	
	} else if(0==st) {
		syslog(PARAM_SYSLOG_BASENO+1, "[表面のみ検査]");  // 301
		LOG(em_MSG), "<<検査面状態変更>> [表面のみ検査]"); 	
	} else if(1==st) {
		syslog(PARAM_SYSLOG_BASENO+1, "[裏面のみ検査]");  // 301
		LOG(em_MSG), "<<検査面状態変更>> [裏面のみ検査]"); 	
	}
	return true;
}


//------------------------------------------
// PLG区分変更
// EM_DIV_PLG st PLG区分
//------------------------------------------
bool StatusBaseManager::SetPlg(EM_DIV_PLG st)
{
	SetStatusQue("DIV_PLGMODE_ID", st, false);

	////// メインインスタンス取り出し
	EM_DIV_PLG	old;
	old = GetPlg();
	if( old == st ) return false;

	//// クリティカルセクション突入
	AutoLock lock(&m_csLock);

	//// 変更ステータスにより処理分け
	m_emPlg = st;
	syslog(PARAM_SYSLOG_BASENO+2, "[<%s> ⇒ <%s>]", DivNameManager::GetDivPlg(old), DivNameManager::GetDivPlg(st));  // 302
	LOG(em_MSG), "<<PLG区分変更>> [<%s> ⇒ <%s>]", DivNameManager::GetDivPlg(old), DivNameManager::GetDivPlg(st)); 
	return true;
}

//------------------------------------------
// 運転状態変更
// M_DIV_UNTEN st 運転状態
//------------------------------------------
bool StatusBaseManager::SetUnten(EM_DIV_UNTEN st)
{
	SetStatusQue("DIV_UNTEN_ID", st, false);

	////// メインインスタンス取り出し
	EM_DIV_UNTEN	old;
	old = GetUnten();
	if( old == st ) return false;

	//// クリティカルセクション突入
	AutoLock lock(&m_csLock);

	//// 変更ステータスにより処理分け
	m_emUnten = st;
	syslog(PARAM_SYSLOG_BASENO+3, "[<%s> ⇒ <%s>]", DivNameManager::GetDivUnten(old), DivNameManager::GetDivUnten(st));  // 303
	LOG(em_MSG), "<<運転状態変更>> [<%s> ⇒ <%s>]", DivNameManager::GetDivUnten(old), DivNameManager::GetDivUnten(st)); 
	return true;
}

//------------------------------------------
// 操業状態変更
// EM_DIV_SOGYO st 操業状態
// int nMpm ライン速度 [mpm]
//------------------------------------------
bool StatusBaseManager::SetSogyo(EM_DIV_SOGYO st, int nMpm)
{
	SetStatusQue("DIV_SOGYO_ID", st, false);

	////// メインインスタンス取り出し
	EM_DIV_SOGYO	old;
	old = GetSogyo();
	if( old == st ) return false;

	//// クリティカルセクション突入
	AutoLock lock(&m_csLock);

	//// 変更ステータスにより処理分け
	m_emSogyo = st;
	syslog(PARAM_SYSLOG_BASENO+4, "[<%s> ⇒ <%s>] (ライン速度=%dmpm)", DivNameManager::GetDivSogyo(old), DivNameManager::GetDivSogyo(st), nMpm); // 304
	LOG(em_MSG), "<<操業状態変更>> [<%s> ⇒ <%s>] (ライン速度=%dmpm)", DivNameManager::GetDivSogyo(old), DivNameManager::GetDivSogyo(st), nMpm);
	return true;
}

#ifdef DSP_SOGYO_CHECK
//------------------------------------------
// 操業状態変更 表示用
// EM_DIV_SOGYO st 操業状態
// int nMpm ライン速度 [mpm]
//------------------------------------------
bool StatusBaseManager::SetSogyoDsp(EM_DIV_SOGYO st, int nMpm)
{
	//KizuFunction::SetStatus("DIV_SOGYO_ID", st, false);

	////// メインインスタンス取り出し
	EM_DIV_SOGYO	old;
	old = GetSogyoDsp();
	if( old == st ) return false;

	//// クリティカルセクション突入
	AutoLock lock(&m_csLock);

	//// 変更ステータスにより処理分け
	m_emSogyoDsp = st;
	syslog(PARAM_SYSLOG_BASENO+8, "[<%s> ⇒ <%s>] (ライン速度=%dmpm)", DivNameManager::GetDivSogyo(old), DivNameManager::GetDivSogyo(st), nMpm); // 308
	LOG(em_MSG), "<<表示用操業状態変更>> [<%s> ⇒ <%s>] (ライン速度=%dmpm)", DivNameManager::GetDivSogyo(old), DivNameManager::GetDivSogyo(st), nMpm);
	return true;
}
#endif

//------------------------------------------
// 稼動状態変更
// EM_DIV_KADOU st 稼動状態変更
//------------------------------------------
bool StatusBaseManager::SetKadou(EM_DIV_KADOU st)
{
	SetStatusQue("DIV_KADOU_ID", st, false);
	
	// 稼動状態が変わった事を機器管理へ通知
	if(NULL != mcls_pKiki ) {
		if( DIV_KADOU_START == st)  mcls_pKiki->SetKadou(true);
		else						mcls_pKiki->SetKadou(false);

		// 即時反映	
		if( mcls_pKiki->IsThreadRun() ) {
			mcls_pKiki->Refresh();
			ReadKikiStatus();
		}
	}

	////// メインインスタンス取り出し
	EM_DIV_KADOU	old;
	old = GetKadou();
	if( old == st ) return false;

	//// クリティカルセクション突入
	AutoLock lock(&m_csLock);

	//// 変更ステータスにより処理分け
	m_emKadou = st;
	syslog(PARAM_SYSLOG_BASENO+5, "[<%s> ⇒ <%s>]", DivNameManager::GetDivKadou(old), DivNameManager::GetDivKadou(st));  // 305
	LOG(em_MSG), "<<稼動状態変更>> [<%s> ⇒ <%s>]", DivNameManager::GetDivKadou(old), DivNameManager::GetDivKadou(st)); 
	return true;
}


//------------------------------------------
// システム状態変更
// int ou 表:0 裏:1
// EM_DIV_SYS st システム状態
//------------------------------------------
bool StatusBaseManager::SetSys(int ou, EM_DIV_SYS st)
{
	////// メインインスタンス取り出し
	EM_DIV_SYS	old;
	old = GetSys(ou);
	if( old == st ) return false;

	//// クリティカルセクション突入
	AutoLock lock(&m_csLock);

	//// 変更ステータスにより処理分け
	m_emSys[ou] = st;
	syslog(PARAM_SYSLOG_BASENO+6, "[%s:<%s> ⇒ <%s>]", DivNameManager::GetTorB(ou), GetDivSys(old), GetDivSys(st));  // 306
	LOG(em_MSG), "<<システム状態変更>> [%s:<%s> ⇒ <%s>]", DivNameManager::GetTorB(ou), GetDivSys(old), GetDivSys(st)); 
	return true;
}
//------------------------------------------
// 周期機能システム状態変更
// int ou 表:0 裏:1
// EM_DIV_SYS st システム状態
//------------------------------------------
bool StatusBaseManager::SetCycleSys(int ou, EM_DIV_SYS st)
{
	////// メインインスタンス取り出し
	EM_DIV_SYS	old;
	old = GetCycleSys(ou);
	if( old == st ) return false;

	//// クリティカルセクション突入
	AutoLock lock(&m_csLock);

	//// 変更ステータスにより処理分け
	m_emCycleSys[ou] = st;
	syslog(PARAM_SYSLOG_BASENO+7, "[%s:<%s> ⇒ <%s>]", DivNameManager::GetTorB(ou), GetDivSys(old), GetDivSys(st));  // 307
	LOG(em_MSG), "<<周期機能システム状態変更>> [%s:<%s> ⇒ <%s>]", DivNameManager::GetTorB(ou), GetDivSys(old), GetDivSys(st)); 
	return true;
}
//------------------------------------------
// パララン機能システム状態変更
// int pcno パラランPC1:0 パラランPC2:1
// EM_DIV_SYS st システム状態
//------------------------------------------
bool StatusBaseManager::SetParaSys(int pcno, EM_DIV_SYS st)
{
	KizuFunction::SetStatus(GetStatusRead().ParaKadou[pcno].id, Get_ParaKadou(st), false);

	////// メインインスタンス取り出し
	EM_DIV_SYS	old;
	old = GetParaSys(pcno);
	if (old == st) return false;

	//// クリティカルセクション突入
	AutoLock lock(&m_csLock);

	//// 変更ステータスにより処理分け
	m_emParaSys[pcno] = st;
	syslog(PARAM_SYSLOG_BASENO + 9, "[パラランPC%d:<%s> ⇒ <%s>]", pcno + 1, GetDivSys(old), GetDivSys(st));  // 309
	LOG(em_MSG), "<<パララン機能システム状態変更>> [パラランPC%d:<%s> ⇒ <%s>]", pcno + 1, GetDivSys(old), GetDivSys(st));
	return true;
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 検査中異常把握

//------------------------------------------
// 検査中異常発生
// bool bErr 大区分 (true:Err false:War) Err時は、検査停止
// int nKind 区分
// int nSt1 付加情報
// int nSt2 付加情報
//------------------------------------------
void StatusBaseManager::AddQueMyErrWar(bool bErr, int nKind, int nSt1, int nSt2)
{
	//// 受け渡し構造体
	TYP_MY_ERRWAR_INF* p = new TYP_MY_ERRWAR_INF;
	p->bErr		= bErr;
	p->nKind	= nKind;
	p->nSt[0]	= nSt1;
	p->nSt[1]	= nSt2;

	if( ! mque_MyErrWar.AddToTailFreeCheck(p, 0) ) {
		LOG(em_ERR), "[%s] 検査中異常キューフル", my_sThreadName);
		syslog(SYSNO_QUEFULL_WAR, "[検査中異常]");
		delete p;
	}
}