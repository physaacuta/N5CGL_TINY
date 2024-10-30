#include "StdAfx.h"
#include "CoilBaseManager.h"

#include "MainInstance.h"

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
CoilBaseManager::CoilBaseManager(void) :
ThreadManager("CoilMgr")
{

//*/----> ◆
#ifdef LOCAL
	m_hCoil = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(TO_COIL_TBL), TBL_TO_COIL_NAME);
	mtbl_pCoil =(TO_COIL_TBL*) MapViewOfFile(m_hCoil, FILE_MAP_ALL_ACCESS, 0, 0, 0);
#ifdef IS_TO_CAMERA_TBL
	m_hCam = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(TO_CAMERA_TBL), TBL_TO_CAMERA_NAME);
	mtbl_pCam =(TO_CAMERA_TBL*) MapViewOfFile(m_hCam, FILE_MAP_ALL_ACCESS, 0, 0, 0);
#endif
#endif
//*///<<---

	//// 共有メモリ
	// コイル情報テーブル
	mem_connect(&m_hCoil, (LPVOID *)&mtbl_pCoil, TBL_TO_COIL_NAME);
	if(m_hCoil == NULL) {
		syslog(SYSNO_MEM_OPEN, "コイル情報テーブル アクセス失敗");
		_ASSERT(FALSE);
	}



	// 初期化
	InitializeCriticalSection(&m_csLock);								// クリティカルセクションオブジェクトを初期化

	//// ハンドル生成
	m_evNextCoilKen  = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evNextCoilDsp = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evNextCoilKen_Recv = CreateEvent(NULL, FALSE, FALSE, NULL);




	// 初期化
	for(int ii=0; ii<NUM_MEN; ii++) {
		m_nInitScode[ii] = MAX_SCODE;
		m_nInitPcode[ii] = MAX_PCODE;	
	}

	AllDelete();

	//// 調整用レコーダー用UDP生成
	SetUdpAllFrame(EPC_UDP_IP, EPC_UDP_PORT_ALLFRAME);

#ifdef ENABLE_RECORDER_UDP
	for (int ii = 0; ii < NUM_MEN; ii++)
	{
		m_nScode[ii]		= MAX_SCODE;
		m_nScode_spmoff[ii] = MAX_SCODE;
	}
	memset(&m_typEpcCoil, 0x00, sizeof(TYPE_EPC_COIL));
#endif

}

//------------------------------------------
// デストラクタ
//------------------------------------------
CoilBaseManager::~CoilBaseManager(void)
{
	//// 共有メモリ開放
	mem_close(&m_hCoil, (LPVOID *)&mtbl_pCoil);							// コイル情報
#ifdef IS_TO_CAMERA_TBL
	mem_close(&m_hCam,  (LPVOID *)&mtbl_pCam);							// カメコン情報
#endif
	//// 開放
	DeleteCriticalSection(&m_csLock);									// クリティカルセクションオブジェクトを開放

	//// ハンドル解放
	CloseHandle(m_evNextCoilKen);
	CloseHandle(m_evNextCoilDsp);
	CloseHandle(m_evNextCoilKen_Recv);

	//// 全長保存UDP破棄
	ReSetUdpAllFrame();
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int CoilBaseManager::ThreadFirst()
{
	//// 事前準備

	//// イベント設定
	this->AddEventNum(1, &m_evNextCoilKen);
	this->AddEventNum(1, &m_evNextCoilDsp);
	this->AddEventNum(1, &mcls_pTlCoilOutSt->gque_Deli.g_evSem);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int CoilBaseManager::ThreadLast()
{
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void CoilBaseManager::ThreadEvent(int nEventNo)
{
	//// シグナルの条件
	enum {	EV_NEXT_COIL_KEN = 0,				// 検査用 コイル情報受信通知 (次コイル情報受信)
			EV_NEXT_COIL_DSP					// 表示用 コイル情報受信通知 (次コイル情報受信)
			//EV_QUE,								// 出側物理コイル単位の状態変更通知
			};

		//// シグナル条件分け
		switch (nEventNo) {

	//-----------------------------------------------------------------------------------------------
		case EV_NEXT_COIL_KEN:										// 検査用 コイル情報受信通知 (次コイル情報受信)
			RecvNextCoil_Ken();
			break;
			
	//-----------------------------------------------------------------------------------------------
		case EV_NEXT_COIL_DSP:										// 表示用 コイル情報受信通知 (次コイル情報受信)
			RecvNextCoil_Dsp();
			break;

	//-----------------------------------------------------------------------------------------------
		default :
			// ありえない！！
			LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
			return;
		}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// シーケンス制御
//==========================================
// 全コイル情報きれいさっぱり初期化
//------------------------------------------
// 【呼び出し元】	
//	MainInstance:
//		←DetectWaitStart
//		←DetectStop
//  自:
//		←コンストラクタ
//		←Init
//==========================================
void CoilBaseManager::AllDelete()
{
	AutoLock autolock(&m_csLock);

	// レコード等
	m_nKenRecBase = -1;					// 上流側の検査基点位置での入側論理コイルバッファの配列位置 (-1:無し) (現在値から-1が一個前のコイル情報となる)
	m_nDspRecBase = -1;
	m_dLastSpm = 0.0f;

	// 実バッファ
#ifndef TRACKING_NON_DSP_COIL
	memset( &m_CoilNext_Dsp[0], 0x00, sizeof(m_CoilNext_Dsp) );
#endif
	CoilClear(&m_CoilNext_Ken);

	for(int ii=0; ii<SIZE_COIL_BUF; ii++) {
		CoilClear(&m_CoilInf[ii]);
		m_CoilInf[ii].rec = ii;
	}

	// 各位置 通過中のコイル情報ポインタ
	for(int ii=0; ii<NUM_MEN; ii++) {
		m_pCoilKenV[ii]  = &m_CoilInf[0];
		m_pCoilKenR[ii]  = &m_CoilInf[0];
	}

#ifndef DSP_NO_TRACKING		// トラッキング表示
	m_pCoilBaseDsp = NULL;
	for(int ii=0; ii<MAX_POS_INDEX; ii++) {
		m_pCoilDsp[ii]  = NULL;
	}
#else						// リアルタイム表示
	m_pCoilBaseDsp = &m_CoilInf[0];
	for(int ii=0; ii<MAX_POS_INDEX; ii++) {
		m_pCoilDsp[ii]		= &m_CoilInf[0];
		m_pCoilKenUD[0][ii]	= &m_CoilInf[0];
		m_pCoilKenUD[1][ii]	= &m_CoilInf[0];
	}
#endif

	// 出側コイル情報
	for(int ii=0; ii<SIZE_COIL_OUT_BUF; ii++){
		OutCoilClear(&m_CoilOutInf);
	}

	// シャーカット位置
	memset(&m_typCut, 0x00, sizeof(m_typCut));


	// 検査台位置を通板中の出側物理コイル情報
	m_pCoilOutKen[0] = &m_CoilOutInf;
	m_pCoilOutKen[1] = &m_CoilOutInf;


	// 輝度
	memset(m_nBrCenter, 0x00, sizeof(m_nBrCenter));

}

//==========================================
// コイル情報の初期化
//------------------------------------------
// 【呼び出し元】
//	自:
//		AllDelete（対象：全コイル）
//		RecvNextCoil_Ken(対象：次コイル)
//		ChangeCoilKenBase(対象：次コイル)
//------------------------------------------
// COIL_INF*	pCoil	: 初期化するコイル情報
//==========================================
void CoilBaseManager::CoilClear(COIL_INF* pCoil)
{
	//// 通常の初期化
	memset( pCoil, 0x00, sizeof(COIL_INF) );

	// 0以外の初期値の奴
	
	for(int ii=0; ii<NUM_MEN; ii++) {
		for(int jj=0; jj<MAP_ROW_NUM; jj++) {
			pCoil->emKensa[ii][jj] = DIV_KENSA_NON;
			pCoil->emKiki [ii][jj] = DIV_KIKI_OK;
			for (int kk = 0; kk<SIZE_COIL_ROWVAL; kk++) pCoil->nRowVal[ii][jj][kk] = 0;

		}
		pCoil->nSetRec_St[ii]   = -1;
		pCoil->nSetRec_Last[ii] = -1;
		pCoil->nSetRec_Edge[ii] = -1;
	}
	pCoil->nLastRec_Postion		= -1;

	pCoil->nLen_Yotei			= COIL_LEN_MAX * 1000;
	pCoil->dSpmHosei			= 1.0;
	
}

//==========================================
// 出側物理コイル情報の初期化
// 【呼び出し元】
//  自：
//			AllDelete（対象：全コイル）
//------------------------------------------
// COIL_OUT_INF* PCoil	: 初期化するコイル情報	
//==========================================
void CoilBaseManager::OutCoilClear(COIL_OUT_INF* pCoil)
{
	// 通常の初期化
	memset(pCoil, 0x00, sizeof(COIL_OUT_INF));

}

//==========================================
// 検査開始時の初期化処理
//------------------------------------------
// 【呼び出し元】
//	MainInstance::DetectRun
//------------------------------------------
// bool			restart	: 強制検査再起動フラグ (true:強制 false:システム開始時)
// const char*	cID		: キー情報(BUF_PRO_R_COILINFのID)
//==========================================
void CoilBaseManager::Init(bool restart)
{
	AutoLock autolock(&m_csLock);

	//// コイル情報開放
	AllDelete();

	//// 必要な情報を生成
#ifndef DSP_NO_TRACKING							// 遅れて表示
	m_nKenRecBase = 0;
	m_nDspRecBase = -1;
#else											// 即時表示
	m_nKenRecBase = 0;
	m_nDspRecBase = m_nKenRecBase;		// 検査後即時表示のため
	m_nShaRecBase = m_nKenRecBase;
#endif

	// 一発目から画面表示のため
	// 検査用
	m_nKenRecBase = 0;
	m_nDspRecBase = m_nKenRecBase;
	
	// 表示用
	m_pCoilBaseDsp = &m_CoilInf[m_nDspRecBase];
	for(int ii=0; ii<POS_DSP_END; ii++) {
		m_pCoilDsp[ii]  = m_pCoilBaseDsp;
	}

	//// 入側論理コイルNoを生成
	DummyCoilInf(m_nKenRecBase, true);

#ifdef DEBUG_LOG_OUT_CHANGE
	LOG(em_FIL), "[%s] Init()で1に初期化[%d]", my_sThreadName, m_CoilOutInf[m_nShaRecBase].nCnt);
#endif

	//// 機器状態変更
	KizuFunction::SetStatus("COIL_KEN_ID", true, false);	// 検査用コイル情報は関係なしの為、復旧させておく
#ifndef TRACKING_NON_DSP_COIL
	KizuFunction::SetStatus("COIL_DSP_ID", true, false);	// 表示用コイル情報は関係なしの為、復旧させておく
#endif
	KizuFunction::SetStatus("COIL_TIMING_ERR_ID", true, false);		// 検査開始2本目のコイル情報なし
	if( ! restart ) {
		KizuFunction::SetStatus("COIL_INIT_ID",     false, false);	// 検査開始１本目
		KizuFunction::SetStatus("COIL_RESTART_ID",  true,  false);	// 強制検査再起動１本目
	} else {
		KizuFunction::SetStatus("COIL_INIT_ID",     true,  false);	// 検査開始１本目
		KizuFunction::SetStatus("COIL_RESTART_ID",  false, false);	// 強制検査再起動１本目
	}


	//// ステータス管理クラスにコイル情報をセット
	mcls_pStatus->SetCoilInfStatus(&m_CoilInf[m_nKenRecBase]); 

	//// 今流れ始めるコイルの コイル情報をDBへ保存 (1本目コイルとして登録)
	SaveDB_CoilInf();

	//// 検査開始一本目から表示
	m_pCoilBaseDsp->bDspOk = true;

	// 管理Noをセットするタイミングは以下の通り。
	// 1本目コイル　：　検査開始時
	// 2本目コイル　：　コイル情報受信時
	// よって、管理Noは 1本目 ＞ 2本目。
	// そこで、1本目と2本目のコイルの管理Noを逆に変更する。
	//// 検査条件初期値がコイル情報要求の場合のみ、2本目コイルの管理Noをリセット
	if( 0!=strlen(m_CoilNext_Ken.cKizukenNo) ) {
		do {
			memset(m_CoilNext_Ken.cKizukenNo, 0x00, sizeof(m_CoilNext_Ken.cKizukenNo));
			KizuFunction::GetKizukenNo( m_CoilNext_Ken.cKizukenNo );
			Sleep(100);
		} while( 0==strcmp(m_CoilInf[m_nKenRecBase].cKizukenNo, m_CoilNext_Ken.cKizukenNo) );
	}

}

//==========================================
// ダミーの入側論理コイル情報を生成
//------------------------------------------
// 【呼び出し元】
//  自
//		ChangeCoilKenBase
//		Init
//------------------------------------------
// int	rec		: セットするレコード位置 (0オリジン)
// bool init	: true:初期時  false:コイル情報等が無い場合
// bool isWpd	: false: コイル長上限到達
//==========================================
void CoilBaseManager::DummyCoilInf(int rec, bool init, bool isWpd)
{
    SYSTEMTIME time;
    GetLocalTime(&time);

	//====================================================================================
	// 命令時に確定する情報
	
	if(isWpd) KizuFunction::GetKizukenNo( m_CoilInf[rec].cKizukenNo );			// 管理No
	else {
		if( 0!=strlen(m_CoilNext_Ken.cKizukenNo) ) {
			// 次コイル情報受信済みであれば、同じ管理Noとする
			memcpy(m_CoilInf[rec].cKizukenNo, m_CoilNext_Ken.cKizukenNo, SIZE_KIZUKEN_NO);
		} else {
			KizuFunction::GetKizukenNo( m_CoilInf[rec].cKizukenNo );			// 管理No
		}
	}

	char cWk[SIZE_SERIAL_NO];
	int size = sprintf(cWk, "%02d%02d%02d%02d%02d", 
								time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

	memset(m_CoilInf[rec].data_u.CoilInf.KeyInf.cCoilNo, 0x20, sizeof(m_CoilInf[rec].data_u.CoilInf.KeyInf.cCoilNo));
	memcpy(m_CoilInf[rec].data_u.CoilInf.KeyInf.cCoilNo, cWk, size);			// コイルNo
	strcpy(m_CoilInf[rec].cCoilKey, "DummyCoil");								// キー情報

	m_CoilInf[rec].setting.bWalm = false;
	for (int ii = 0; ii<NUM_MEN; ii++) {
		m_CoilInf[rec].setting.bDefScode[ii] = true;
		m_CoilInf[rec].setting.bDefPcode[ii] = true;
		m_CoilInf[rec].setting.scode[ii]		= (init ? m_nInitScode[ii] : MAX_SCODE);
		m_CoilInf[rec].setting.pcode[ii]		= (init ? m_nInitPcode[ii] : MAX_PCODE);
		m_CoilInf[rec].setting.scode_spmoff[ii] = (init ? m_nInitScode[ii] : MAX_SCODE);
		m_CoilInf[rec].setting.pcode_spmoff[ii] = (init ? m_nInitPcode[ii] : MAX_PCODE);
	}
	m_CoilInf[rec].bDummeyCoil		= true;										// ダミーコイル設定
	m_CoilInf[rec].bCoilInit		= init;										// コイル検査開始１本目(true:１本目)
	m_CoilInf[rec].nMapCol			= MAP_COL_NUM;								// マップの幅数
	m_CoilInf[rec].nStartWpdOk		= 2;										// バックアップにて溶接切替(0:正常WPD  1:異種点  2:ゲート信号)

	// 基本コイル情報部
	m_CoilInf[rec].data.nLenIn = COIL_LEN_MAX;							// コイル長[mm]
	m_CoilInf[rec].data.nWid = COIL_WID_MAX;
	m_CoilInf[rec].data.nAtu = COIL_THICK_MAX;


	//====================================================================================
	// プロコンコイル情報を擬似的にセット　コイル長上限到達バージョン(一個前のコイル情報から引継ぎ)
	if( ! isWpd ){

		int nPre = (0 != rec ? rec - 1 : SIZE_COIL_BUF-1);
		//// 前コイルから引き継ぐ情報
		memcpy(&m_CoilInf[rec].data_u.CoilInf, &m_CoilInf[nPre].data_u.CoilInf, sizeof(m_CoilInf[rec].data_u.CoilInf));	// コイル情報
		memcpy(&m_CoilInf[rec].setting, &m_CoilInf[nPre].setting, sizeof(m_CoilInf[nPre].setting));	// φωで使うコイル基本情報

		memcpy(m_CoilInf[rec].data_u.CoilInf.KeyInf.cCoilNo, cWk, size);							// コイルNo戻し

																								//// その他コイル情報
		m_CoilInf[rec].nMapCol		= m_CoilInf[nPre].nMapCol;										// マップの幅数
																							
	//====================================================================================
	// プロコンコイル情報を擬似的にセット　次コイル情報無しバージョン
	}else{

		int nCoilLen = 2500;			// 今この場でのコイル長
		// 擬似プロコンコイル情報をセット
		memset(&m_CoilInf[rec].data_u.CoilInf.KeyInf.cMeiNo,	0x00,	sizeof(m_CoilInf[rec].data_u.CoilInf.KeyInf.cMeiNo));	// 命令No
		memcpy(&m_CoilInf[rec].data_u.CoilInf.KeyInf.cMeiNo,	"0123",	sizeof(m_CoilInf[rec].data_u.CoilInf.KeyInf.cMeiNo));

		m_CoilInf[rec].data_u.CoilInf.nWidZai= m_CoilInf[rec].data.nWid;							// 材料巾(入側) [mm]
		m_CoilInf[rec].data_u.CoilInf.nWidTrim = (long)((double)m_CoilInf[rec].data.nWid * 0.9);	// トリム巾 [mm] (トリム無し時は、0)
		m_CoilInf[rec].data_u.CoilInf.nAtuSei=m_CoilInf[rec].data.nAtu;								// 注文厚 [μm]
		m_CoilInf[rec].data_u.CoilInf.nLenIn= nCoilLen;													// コイル長 [m]
#if 0		// 試験用データ
		m_CoilInf[rec].data_u.CoilInf.nAtuZai = m_CoilInf[rec].data.nAtu;							// 材料板厚
		m_CoilInf[rec].data_u.CoilInf.nWidSei = m_CoilInf[rec].data.nWid;							// 材料板幅
		m_CoilInf[rec].data_u.CoilInf.nNobiSPM = 100;												// SPM伸び率
		m_CoilInf[rec].data_u.CoilInf.nNobiTLV = 100;												// TLV伸び率
		m_CoilInf[rec].data_u.CoilInf.nCutF = nCoilLen * 0.01;										// 入側におけるフロント端板カットの実測長
		m_CoilInf[rec].data_u.CoilInf.nCutT = nCoilLen * 0.02;										// 入側におけるテール端板カットの実測長
		m_CoilInf[rec].data_u.CoilInf.nSPMRoolKei_Up = 100;											// SPMワークロール径（上）
		m_CoilInf[rec].data_u.CoilInf.nSPMRoolKei_Down = 200;										// SPMワークロール径（下）
		memcpy(m_CoilInf[rec].data_u.CoilInf.cKenKijun_O, "    ", 4);								// 検査基準 表
		memcpy(m_CoilInf[rec].data_u.CoilInf.cKenKijun_U, "    ", 4);								// 検査基準 裏
		memcpy(m_CoilInf[rec].data_u.CoilInf.cKurometo_Kind, "  ", 2);								// クロメート 実績種類
		memcpy(m_CoilInf[rec].data_u.CoilInf.cKurometo_Umu, "  ", 2);								// クロメート 実績有無
		memcpy(m_CoilInf[rec].data_u.CoilInf.cHinCode, " ", 1);										// 品種
		memcpy(m_CoilInf[rec].data_u.CoilInf.cMekkiKind, " ", 1);									// メッキ種類
		memcpy(m_CoilInf[rec].data_u.CoilInf.cSyuZai, "  ", 2);										// 主材質
		memcpy(m_CoilInf[rec].data_u.CoilInf.cDifMaker, " ", 1);									// ディファレンシャルマーカー
		memcpy(m_CoilInf[rec].data_u.CoilInf.cKenban, " ", 1);										// 原板区分
		memcpy(m_CoilInf[rec].data_u.CoilInf.cMekkiCycle, " ", 1);									// メッキサイクル
		memcpy(m_CoilInf[rec].data_u.CoilInf.cInLineSPM, " ", 1);									// レベラー インラインSPM
		memcpy(m_CoilInf[rec].data_u.CoilInf.cKagakuToyu, " ", 1);									// 化学処理塗油
		memcpy(m_CoilInf[rec].data_u.CoilInf.cAtukkaSPM, " ", 1);									// オフラインSPM圧下率
		memcpy(m_CoilInf[rec].data_u.CoilInf.cMekkiKubun, " ", 1);									// メッキ区分
		memcpy(m_CoilInf[rec].data_u.CoilInf.cOmoteSyori1, " ", 1);									// 表面処理1桁目
		memcpy(m_CoilInf[rec].data_u.CoilInf.cOmoteSyori2, " ", 1);									// 表面処理2桁目
		memcpy(m_CoilInf[rec].data_u.CoilInf.cToyu, " ", 1);										// 塗油
		memcpy(m_CoilInf[rec].data_u.CoilInf.cKouteiYobi, " ", 1);									// 工程記号予備
		memcpy(m_CoilInf[rec].data_u.CoilInf.cCaplSyuZai, "          ", 10);						// CAPL 主材質
		memcpy(m_CoilInf[rec].data_u.CoilInf.cMetuke, "        ", 8);								// 目付規格
		memcpy(m_CoilInf[rec].data_u.CoilInf.cNi, "  ", 2);											// Ni付着量
		memcpy(m_CoilInf[rec].data_u.CoilInf.cSodoTCM, "  ", 2);									// TCMロール粗度
		memcpy(m_CoilInf[rec].data_u.CoilInf.cSpmUmu, "  ", 2);										// SPM圧下有無
		memcpy(m_CoilInf[rec].data_u.CoilInf.cNextLine, "  ", 2);									// 計画次工程コード
		memcpy(m_CoilInf[rec].data_u.CoilInf.cMuke, "ABCD                            ", 32);		// 向先
		memcpy(m_CoilInf[rec].data_u.CoilInf.cYouto, "    ", 4);									// 用途
		memcpy(m_CoilInf[rec].data_u.CoilInf.cCaplSiage, "    ", 4);								// CAPL表面仕上げ
		memcpy(m_CoilInf[rec].data_u.CoilInf.cCaplSanUmu, "  ", 2);									// CAPL酸洗有無
		memcpy(m_CoilInf[rec].data_u.CoilInf.cHinMode, " ", 1);										// 品種区分
		memcpy(m_CoilInf[rec].data_u.CoilInf.cGaCrKubun, " ", 1);									// GA/CR区分
		memcpy(m_CoilInf[rec].data_u.CoilInf.cSpmJisekiUmu, "  ", 2);								// SPM圧下実績有無
		memcpy(m_CoilInf[rec].data_u.CoilInf.cKouteiCode, "  ", 2);									// 工程コード
#endif
	}

	//====================================================================================
	// 擬似プロコンコイル情報から、改めてシステム共通コイル情報をセット
	memset(m_CoilInf[rec].data.cMeiNo, 0x00, sizeof(m_CoilInf[rec].data.cMeiNo));
	memcpy(m_CoilInf[rec].data.cMeiNo, m_CoilInf[rec].data_u.CoilInf.KeyInf.cMeiNo, SIZE_MEI_NO);				// 命令No
	memset(m_CoilInf[rec].data.cCoilNoIn, 0x00, sizeof(m_CoilInf[rec].data.cCoilNoIn));
	memcpy(m_CoilInf[rec].data.cCoilNoIn, m_CoilInf[rec].data_u.CoilInf.KeyInf.cCoilNo, SIZE_COIL_NO);			// コイルNo
	memset(m_CoilInf[rec].data.cCoilKey, 0x00, sizeof(m_CoilInf[rec].data.cCoilKey));
	memcpy(m_CoilInf[rec].data.cCoilKey, &m_CoilInf[rec].data_u.CoilInf.KeyInf.cMeiNo[0], (SIZE_MEI_NO+ SIZE_COIL_NO));		// キー情報
	m_CoilInf[rec].data.nWid = COIL_WID_MAX;					// 板幅

	// その他共通のコイル情報
	Create_CoilInf(&m_CoilInf[rec]);
}

//==========================================
// 入側論理コイル情報を生成
//------------------------------------------
// 【呼び出し元】
//  自
//		DummyCoilInf
//		RecvNextCoil_Ken
//------------------------------------------
// COIL_INF* pCoil セットするポインタ
//==========================================
void CoilBaseManager::Create_CoilInf(COIL_INF* pCoil)
{
	//// 命令時に確定
	for(int ii=0; ii<NUM_MEN; ii++) {
		for(int jj=0; jj<NUM_HANTEI; jj++) {
			pCoil->nHtID[ii][jj] = mcls_pStatus->GetHtID(ii, jj);
		}
	}
	pCoil->emUnten = mcls_pStatus->GetUnten();

	//// 随時更新
	// この時点では、最弱の正常をセット
	for(int ii=0; ii<NUM_MEN; ii++) {
		pCoil->emOneKensa[ii] = DIV_KENSA_OK;
		pCoil->emOneKiki [ii] = DIV_KIKI_OK;
	}

	//// コイル通板完了時に確定
	pCoil->bCoilResultAddDb = false;

	//// データセット (実績時に確定)
	pCoil->bDspOk = false;
}


//==========================================
// 検査基点のコイル情報を 次コイル情報エリアにセット
//==========================================
void CoilBaseManager::RecvNextCoil_Ken()
{
	AutoLock autolock(&m_csLock);
	//double dNobi = 1.0;					// 伸び率

	//================================================
	// 共有メモリデータアクセス
	int nRec = 0;	// 検査用のコイル情報にアクセス
 	COIL_SETTING& setting		= (COIL_SETTING&)mtbl_pCoil->rec[nRec].setting;
	COIL_BASEDATA& basedata		= (COIL_BASEDATA&)mtbl_pCoil->rec[nRec].basedata;
	PRO_TYPE_COILINF& coil		= (PRO_TYPE_COILINF&)mtbl_pCoil->rec[nRec].val;
	PRO_TYPE_DATA_COILINF *pData = (PRO_TYPE_DATA_COILINF *)&coil.data;

	//================================================
	// 事前チェック
	// 無効データ？（グラントフラグの値をチェック）
	if (0 != pData->Common.nGrandFlg) {
		LOG(em_ERR), "[CoilManager] 検査用受信コイル情報 異常 [区分=%d コイルNo=%.12s]", pData->Common.nGrandFlg, coil.data.CoilInf.KeyInf.cCoilNo);
		syslog(263, "[検査用コイル情報 区分=%d コイルNo=%.12s]", pData->Common.nGrandFlg, coil.data.CoilInf.KeyInf.cCoilNo);
		return;
	}

	// 次コイルバッファに既にデータあり
	//bool bOldCoil = false;
	char cOldKizukenNo[SIZE_KIZUKEN_NO];			// コイル情報受信で、表示PCにコイル情報を送るようにしたため、別管理Noをつけちゃうと表示レコードが切替ってしまう対策
	if( 0 != strlen(m_CoilNext_Ken.cKizukenNo) ) {
		LOG(em_ERR), "[%s] 検査用次コイルバッファに既にデータ有り[コイルNo=%s ID=%s]", my_sThreadName, m_CoilNext_Ken.data.cCoilNoIn, m_CoilNext_Ken.cKizukenNo);
		syslog(252, "[検査用：コイルNo=%s ID=%s]", m_CoilNext_Ken.data.cCoilNoIn, m_CoilNext_Ken.cKizukenNo);
		
		//bOldCoil = true;
		memcpy(cOldKizukenNo, m_CoilNext_Ken.cKizukenNo, SIZE_KIZUKEN_NO);
	}

	// 現コイルの次コイル情報と今受信した次コイル情報が不一致？
	if (0 != strlen(m_pCoilKenV[m_nDistPos]->data_u.next_coil[0].KeyInf.cCoilNo)) {
		if (0 != memcmp(m_pCoilKenV[m_nDistPos]->data_u.next_coil[0].KeyInf.cCoilNo, coil.data.CoilInf.KeyInf.cCoilNo, sizeof(coil.data.CoilInf.KeyInf.cCoilNo))) {
			LOG(em_WAR), "[CoilManager] 現コイルの次コイルNoと次コイルNoが不一致 [現の次コイルNo=%.12s 次コイルNo=%.12s]", m_pCoilKenV[m_nDistPos]->data_u.next_coil[0].KeyInf.cCoilNo, coil.data.CoilInf.KeyInf.cCoilNo);
			syslog(262, "[現コイルの次コイルNo=%.12s 次コイルNo=%.12s]", m_pCoilKenV[m_nDistPos]->data_u.next_coil[0].KeyInf.cCoilNo, coil.data.CoilInf.KeyInf.cCoilNo);
		}
	}

	//================================================
	// データセット (命令時に確定)

	//// 基本
	CoilClear(&m_CoilNext_Ken);
	memcpy( &m_CoilNext_Ken.setting, &setting, sizeof(COIL_SETTING));			// φωで使うコイル基本情報
	memcpy( &m_CoilNext_Ken.data,    &basedata, sizeof(COIL_BASEDATA));			// コイル情報 (システム共通)
	memcpy( &m_CoilNext_Ken.data_u,  &coil.data, sizeof(PRO_TYPE_DATA_COILINF));		// コイル情報 (ライン固有)

	m_CoilNext_Ken.dSpmHosei = 1.0 + (double)( coil.data.CoilInf.nNobiSPM ) / 10000;

	//if(!bOldCoil) {
	KizuFunction::GetKizukenNo( m_CoilNext_Ken.cKizukenNo );						// 管理No
	//} else {
	//	memcpy(m_CoilNext_Ken.cKizukenNo, cOldKizukenNo, SIZE_KIZUKEN_NO);				// 初回つけたものを引き続き使用
	//}
	m_CoilNext_Ken.bDummeyCoil = false;
	m_CoilNext_Ken.bCoilInit = false;											// 検査開始１本目以外

	////// 拡張部
	//memcpy(m_CoilNext_Ken.data.cKizukenNo, m_CoilNext_Ken.cKizukenNo, SIZE_KIZUKEN_NO);

	// ウォーマ材？
	//// ↓もう既にTO_GETDATがsetting.bWalmにフラグを入れたんじゃないの？
	//if( mcls_pParam->GetCommonMast().walm == m_CoilNext_Ken.setting.pcode[0] ) {			// ウォーマ材
	//	m_CoilNext_Ken.setting.bWalm = true;
	//}
	if( m_CoilNext_Ken.setting.bWalm ) {			// ウォーマ材
		LOG(em_WAR), "[%s] ウォーマ材 認識！！ [コイルNo=%s]", my_sThreadName, basedata.cCoilNoIn);
	}

	// 板幅に対応するマップ列数は？
	if (0 >= m_CoilNext_Ken.data.nWid)			m_CoilNext_Ken.data.nWid = COIL_WID_MAX;
	if (COIL_WID_MAX < m_CoilNext_Ken.data.nWid) m_CoilNext_Ken.data.nWid = COIL_WID_MAX;
#ifndef PLG_DSP_MAP_FIX
	m_CoilNext_Ken.nMapCol = KizuFunction::SelectGridCol2(m_CoilNext_Ken.data.nWid, MAP_COL_INTERVAL, MAP_COL_NUM);
#else
	m_CoilNext_Ken.nMapCol = MAP_COL_NUM;
#endif
	LOG(em_MSG), "[%s] 検査基点コイル情報受信 [%s][%s] MAP列数=%d", my_sThreadName, basedata.cCoilNoIn, m_CoilNext_Ken.cKizukenNo, m_CoilNext_Ken.nMapCol);
	syslog(250, "[検査用 コイルNo=%s%s]", m_CoilNext_Ken.data.cCoilNoIn, m_CoilNext_Ken.setting.bWalm ? " <ウォーマ材>" : "");

	//// その他共通のコイル情報
	Create_CoilInf(&m_CoilNext_Ken);

	//================================================
	// 通知

	// 検査用 次コイル情報受信完了通知イベント
	SetEvent(m_evNextCoilKen_Recv);

	// 上位情報を各判定PCへ送信
	TYPE_EPC_COIL data;
	SetTypeEpcCoil(data, m_CoilNext_Ken.data_u);
	MainInstance::Send_HtDefect_Pcinfo(&data);
#ifdef ENABLE_RECORDER_UDP
	// 調整用レコーダーに送信するコイル情報を保持(実際に送信するのはコイル切替時)
	memset(&m_typEpcCoil, 0x00, sizeof(TYPE_EPC_COIL));
	memcpy(&m_typEpcCoil, &data, sizeof(TYPE_EPC_COIL));
	for (int ii = 0; ii < NUM_MEN; ii++)
	{
		m_nScode[ii]		= m_CoilNext_Ken.setting.scode[ii];
		m_nScode_spmoff[ii] = m_CoilNext_Ken.setting.scode_spmoff[ii];
	}
#endif


	// パラメータ変更通知 (検査表面状態変更、毎回送っても良いとの事)
	for(int ii=0; ii<NUM_MEN; ii++) {
		// SPM圧下状態
		LOG(em_MSG), "[%s] 検査基点 パラメータ事前読込要求(FACT_HT_DEFECT_21) [%s][%s] SPM圧下 scode=%d",
			my_sThreadName, m_CoilNext_Ken.data.cCoilNoIn, DivNameManager::GetTorB(ii), m_CoilNext_Ken.setting.scode[ii]);
		MainInstance::Send_HtDefect_PreLoadParam(ii, DIV_HANTEI_NON, m_CoilNext_Ken.setting.scode[ii]);
		// SPM開放状態
		LOG(em_MSG), "[%s] 検査基点 パラメータ事前読込要求(FACT_HT_DEFECT_21) [%s][%s] SPM開放 scode=%d",
			my_sThreadName, m_CoilNext_Ken.data.cCoilNoIn, DivNameManager::GetTorB(ii), m_CoilNext_Ken.setting.scode_spmoff[ii]);
		MainInstance::Send_HtDefect_PreLoadParam(ii, DIV_HANTEI_NON, m_CoilNext_Ken.setting.scode_spmoff[ii]);
	}
	//	
	////// コイル情報 を表示PCに送る 
	//// ※ 従来は、上流カメラ位置のコイル切り替えで送っていたが、前工程情報を前もって取得したい
	////     それで、表示PCのレコードを次に切り替えるために送る。
	//OpSendFunc::SendOp_CoilInf_Ken( &m_CoilNext_Ken );

	//// 通板開始コイルの前工程情報を取得 ＆ 通知
	if( ! m_CoilNext_Ken.bDummeyCoil ) {
		// 検査用コイル情報を受信した & 管理Noが確定した事を前工程実績編集へ通知
		MainInstance::Send_ToMaedat_CoilRecv(m_CoilNext_Ken.cKizukenNo, nRec);

		//// 仮管理Noから管理Noに振りなおし
		//mcls_pMae->AddQue_MaeRequest(&m_CoilNext_Ken, dNobi);
	}else{
		//// 表示PCに通知(表示PC側での共有メモリ初期化の為、全コイル通知が必要)x
		//MainInstance::Send_HyDefect_MaeRead(m_CoilNext_Ken.cKizukenNo);
	}

}


//------------------------------------------
// 表示基点のコイル情報を 次コイル情報エリアにセット
//------------------------------------------
void CoilBaseManager::RecvNextCoil_Dsp()
{
#ifndef TRACKING_NON_DSP_COIL
	// ※
	AutoLock autolock(&m_csLock);

	//================================================
	// 共有メモリデータアクセス
	int nRec = 1;
	PRO_TYPE_COILINF& coil = (PRO_TYPE_COILINF&)mtbl_pCoil->rec[nRec].val;
	PRO_TYPE_DATA_COILINF *pData = (PRO_TYPE_DATA_COILINF *)&coil.data;

	for (int ii = 0; ii<NUM_DSP_WPD; ii++) {
		//================================================
		// 事前チェック
		// 無効データ？
		if (0 != pData->Common.nGrandFlg) {
			LOG(em_ERR), "[CoilManager] 表示用受信コイル情報 異常 [区分=%d コイルNo=%.12s]", pData->Common.nGrandFlg, coil.data.CoilInf.KeyInf.cCoilNo);
			syslog(263, "[表示用コイル情報 区分=%d コイルNo=%.12s]", pData->Common.nGrandFlg, coil.data.CoilInf.KeyInf.cCoilNo);
			return;
		}
		if( 0!= coil.data.CoilInf.nAtuSei)
		// 次コイルバッファに既にデータあり
		if (0 != strlen(m_CoilNext_Dsp[ii].CoilInf.KeyInf.cCoilNo)) {
			LOG(em_ERR), "[%s] 表示用次コイルバッファに既にデータ有り[コイルNo=%12.12s]", my_sThreadName, m_CoilNext_Dsp[ii].CoilInf.KeyInf.cCoilNo);
			syslog(253, "[表示用：コイルNo=%12.12s]", m_CoilNext_Dsp[ii].CoilInf.KeyInf.cCoilNo);
			memset(&m_CoilNext_Dsp[ii], 0x00, sizeof(PRO_TYPE_DATA_COILDSP));
		}


		//================================================
		// データセット
		memcpy(&m_CoilNext_Dsp[ii], &coil.data, sizeof(PRO_TYPE_DATA_COILDSP));		// プロコンデータセット
	}

	char cWk[SIZE_COIL_NO + 1];
	memset(cWk, 0x00, sizeof(cWk));
	memcpy(cWk, m_CoilNext_Dsp[0].CoilInf.KeyInf.cCoilNo, SIZE_COIL_NO);
	LOG(em_MSG), "[%s] 表示基点コイル情報受信 [%s]", my_sThreadName, cWk);
	syslog(250, "[表示用 コイルNo=%s]", cWk);
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// コイル切り替え
//==========================================================================
// 検査対象コイル切り替え(基準面) [←MI]
//--------------------------------------------------------------------------
// bool isWpd : true:通常切替 false:コイル長上限到達によるバックアップ切替
// bool isNowCoil : true:現コイル反映の一発目切替 false:それ以外
//==========================================================================
void CoilBaseManager::ChangeCoilKenBase(bool isWpd, bool isNowCoil)
{
	AutoLock autolock(&m_csLock);
	bool bDummyFlg = false;					// ダミーコイル生成時はtrue
	int rec = m_nKenRecBase + 1;
	if(rec >= SIZE_COIL_BUF) rec = 0;

	if(isWpd){
		//================================================
		//// 入側関連
		// 次コイル情報がちゃんとあった？
		if( 0 == strlen(m_CoilNext_Ken.cKizukenNo) ) { 
			LOG(em_ERR), "[%s] 検査用次コイル情報無し", my_sThreadName);
			syslog(254, "[検査用]");
			bDummyFlg = true;
		}

		//================================================
		//// コイル切り替え
		memcpy( &m_CoilInf[rec], &m_CoilNext_Ken, sizeof(COIL_INF));	// 次コイル から 現コイルに情報セット
		
	}
	m_CoilInf[rec].rec = rec;
	
	// コイル情報にSPM状態セット
	// ※検査位置の状態で決定
	m_CoilInf[rec].nSt_Spm = mcls_pStatus->IsSpmStat() ? 1 : 0;
	LOG(em_INF), "[%s] [%s] 検査位置 SPM状態：%s", my_sThreadName, m_CoilInf[rec].data.cCoilNoIn, GetSpmStat((EM_DIV_SPM_STAT)mcls_pStatus->GetSpmStat()));

	// SPMが開放されていた場合、パラメータ変更要求を送信する
	if (0 == m_CoilInf[rec].nSt_Spm)
	{
		// 基準面のみ変更
		// ※既に検査台に到達しているので即時反映
		LOG(em_INF), "[%s] 検査位置 パラメータ変更要求(FACT_HT_DEFECT_01) <SPM%s> [%s][%s] scode=%d",
			my_sThreadName, GetSpmStat((EM_DIV_SPM_STAT)mcls_pStatus->GetSpmStat()), m_CoilInf[rec].data.cCoilNoIn, DivNameManager::GetTorB(m_nDistPos), m_CoilInf[rec].setting.scode_spmoff[m_nDistPos]);
		MainInstance::Send_HtDefect_Param(0, m_nDistPos, m_CoilInf[rec].setting.scode_spmoff[m_nDistPos], DIV_HANTEI_NON);
	}

	// ステータス管理クラスにコイル情報をセット
	mcls_pStatus->SetCoilInfStatus(&m_CoilInf[rec]); 

	// 次エリア初期化
	int nextrec = rec + 1;
	if(nextrec >= SIZE_COIL_BUF) nextrec = 0;
	CoilClear(&m_CoilInf[nextrec]);			// 次エリア領域初期化
	m_CoilInf[nextrec].rec = nextrec;

	// 仮管理Noから管理Noに振りなおし←コイル切り替え時にタイミング変更
	mcls_pMae->RecvCoilInf(&m_CoilNext_Ken);

	//================================================
	//// コイル情報有無
	if(bDummyFlg || ! isWpd) {
		// コイル情報が無かった為、ダミーコイル生成
		DummyCoilInf(rec, false, isWpd);

#ifdef ENABLE_RECORDER_UDP
		// ダミーコイルのレコーダーPC送信用コイル情報を生成
		// 調整用レコーダーに送信するコイル情報を保持(実際に送信するのはコイル切替時)
		TYPE_EPC_COIL data;
		SetTypeEpcCoil(data, m_CoilInf[rec].data_u);
		memset(&m_typEpcCoil, 0x00, sizeof(TYPE_EPC_COIL));
		memcpy(&m_typEpcCoil, &data, sizeof(TYPE_EPC_COIL));
		for (int ii = 0; ii < NUM_MEN; ii++)
		{
			m_nScode[ii]		= m_CoilInf[rec].setting.scode[ii];
			m_nScode_spmoff[ii] = m_CoilInf[rec].setting.scode_spmoff[ii];
		}
#endif

		// 機器状態変更
		// タイミング不良時
		if( (0 != mcls_pStatus->GetStatusRead().Coil_Init.stat || 0 != mcls_pStatus->GetStatusRead().Coil_ReStart.stat) && isWpd ) {
			mcls_pStatus->SetStatusQue("COIL_TIMING_ERR_ID", false);
		}
		// 検査用コイル情報未受信時
		else {
			if(0 == mcls_pStatus->GetStatusRead().MaxLen_Ken.stat) {
				mcls_pStatus->SetStatusQue("COIL_TIMING_ERR_ID", true, false);
				if( 0 == mcls_pStatus->GetStatusRead().Coil_Ken.stat && isWpd ) mcls_pStatus->SetStatusQue("COIL_KEN_ID", false);
			}
		}
	} else {
		// 機器状態変更
		if( 0 != mcls_pStatus->GetStatusRead().Coil_Ken.stat && isWpd  ) mcls_pStatus->SetStatusQue("COIL_KEN_ID", true);
		if( 0 != mcls_pStatus->GetStatusRead().Coil_Timing_Err.stat && isWpd ) mcls_pStatus->SetStatusQue("COIL_TIMING_ERR_ID", true);
	}
	// 検査開始１本目以降(現コイル反映の一発目は実施しない)
	if( ! isNowCoil ){
		if( 0 != mcls_pStatus->GetStatusRead().Coil_Init.stat )    mcls_pStatus->SetStatusQue("COIL_INIT_ID", true);
		if( 0 != mcls_pStatus->GetStatusRead().Coil_ReStart.stat ) mcls_pStatus->SetStatusQue("COIL_RESTART_ID", true);
	}
	//// 検査表面状態 / 検査パターン 保存 (強制検査再起動用)
	for(int ii=0; ii<NUM_MEN; ii++) {
		m_nInitScode[ii] = m_CoilInf[rec].setting.scode[ii];
		m_nInitPcode[ii] = m_CoilInf[rec].setting.pcode[ii];
	}

	// 検査基点でのコイルのSPM伸び率を保持しておく
	if (bDummyFlg)
	{
		// SPM伸び率0%固定
		m_dLastSpm = 0.0;
	}
	else
	{
		// [10^2%]
		m_dLastSpm = m_CoilInf[rec].data.dNobiSPM;
	}
	LOG(em_INF), "[%s] [%s] 検査位置 SPM伸び率=%.4f%%", my_sThreadName, m_CoilInf[rec].data.cCoilNoIn, m_dLastSpm);

	//================================================
	//// レコード更新
	m_nKenRecBase = rec;
	m_pCoilKenR[m_nDistPos] = &m_CoilInf[rec];

	//// 今流れ始めるコイルの コイル情報をDBへ保存
	SaveDB_CoilInf();

	//// 次コイル領域初期化
	CoilClear(&m_CoilNext_Ken);				// 次コイル領域初期化

	//// 他機能へ通知
	// コイル情報
	OpSendFunc::SendOp_CoilInf_Ken( &m_CoilInf[m_nKenRecBase] );
	// ステータス情報
	OpSendFunc::SendOp_Status();
}

//==========================================
// 検査対象コイル切り替え(他方面) [←MainInstance]
//==========================================
void CoilBaseManager::ChangeCoilKenBase_NotPos()
{
	AutoLock autolock(&m_csLock);

	// 参照ポインタを変更
	m_pCoilKenR[m_nDistNotPos]	= &m_CoilInf[m_nKenRecBase];

	// SPMが開放されていた場合、パラメータ変更要求を送信する
	if (0 == m_pCoilKenR[m_nDistNotPos]->nSt_Spm)
	{
		// 基準面と逆の面のみ変更
		// ※既に検査台に到達しているので即時反映
		LOG(em_INF), "[%s] 検査位置 パラメータ変更要求(FACT_HT_DEFECT_01) <SPM%s> [%s][%s] scode=%d",
			my_sThreadName, GetSpmStat((EM_DIV_SPM_STAT)mcls_pStatus->GetSpmStat()), m_CoilInf[m_nKenRecBase].data.cCoilNoIn, DivNameManager::GetTorB(m_nDistNotPos), m_CoilInf[m_nKenRecBase].setting.scode_spmoff[m_nDistNotPos]);
		MainInstance::Send_HtDefect_Param(0, m_nDistNotPos, m_CoilInf[m_nKenRecBase].setting.scode_spmoff[m_nDistNotPos], DIV_HANTEI_NON);
	}
}


//==========================================
// 表示対象コイル切り替え [←MainInstance]
//------------------------------------------
// double	dLen	: 表示基点のコイル長
// int		nDspWpd	: 表示基点（1CAPL中央では0のみ）
// 戻り値	bool	: 切り替え可否
//==========================================
bool CoilBaseManager::ChangeCoilDspBase(double dLen, int nDspWpd)
{
	AutoLock autolock(&m_csLock);
	int rec = -1;							// 内部保持レコードとの紐付け不能時は、-1
	int wkRec = 0;							// 次レコード検索用

#ifndef TRACKING_NON_DSP_COIL			// トラッキング表示
	char cCoilKey[SIZE_COIL_KEY + 1];					// 使いやすいようにキーとなる情報だけにする
	char cCoilNo[SIZE_COIL_NO+1];						// ログ表示用にコイルNoも
	char cMeiNo[SIZE_MEI_NO + 1];						// ログ表示用に命令Noも
	memset(cCoilKey, 0x00, sizeof(cCoilKey));
	memset(cCoilNo, 0x00, sizeof(cCoilNo));
	memset(cMeiNo, 0x00, sizeof(cMeiNo));
	memcpy(cCoilKey, &m_CoilNext_Dsp[nDspWpd].CoilInf.KeyInf, sizeof(PRO_TYPE_KEY));
	memcpy(cCoilNo, &m_CoilNext_Dsp[nDspWpd].CoilInf.KeyInf.cCoilNo, SIZE_COIL_NO);
	memcpy(cMeiNo, &m_CoilNext_Dsp[nDspWpd].CoilInf.KeyInf.cMeiNo, SIZE_MEI_NO);


	//// 次コイル情報がちゃんとあった？
	if( 0 == strlen(cCoilKey) ) { 
		LOG(em_ERR), "[%s] 表示用次コイル情報無し", my_sThreadName);
		syslog(255, "[表示用]");
		if( 0 == mcls_pStatus->GetStatusRead().Coil_Dsp.stat ) mcls_pStatus->SetStatusQue("COIL_DSP_ID", false);
		rec = -1;

	} else {
		// 検査用コイルとの紐付け
		for(int ii=1; ii<SIZE_COIL_BUF-1; ii++) {
			wkRec = GetRecIndex(m_nDspRecBase, -ii);		// 1個つぎ 2個つぎと順番に探していく
			if( 0 == memcmp(cCoilKey, m_CoilInf[wkRec].data.cCoilKey, SIZE_COIL_KEY) ) {
				// 紐付け完了
				if( 0 != mcls_pStatus->GetStatusRead().Coil_Dsp.stat ) mcls_pStatus->SetStatusQue("COIL_DSP_ID", true);
				rec = wkRec;
				break;
			}
		}

		if(-1 == rec) {
			LOG(em_WAR), "[%s] 表示用コイル情報切り替えだけど、まだ検査したコイルが無い。コイルNo=%s, 命令No=%s", my_sThreadName, cCoilNo, cMeiNo);
			syslog(256, "[コイルNo=%s, 命令No=%s]", cCoilNo, cMeiNo);
			memset( &m_CoilNext_Dsp, 0x00, sizeof(m_CoilNext_Dsp));
			return false;
		}
	}

	//// 検査開始一本目の場合のみ、WPDが来たら、今検査している奴を出力してあげる  (かなりデバック用)
	if(NULL == m_pCoilBaseDsp) {
		if(-1 == rec) rec=0;
	}

	//// 紐付け失敗時のフォロー
	if(-1 == rec) {
		// 順番通りに通板するはずなので、今の次の奴が流れているとする
		wkRec = GetRecIndex(m_nDspRecBase, -1);
		if( -1 != m_nDspRecBase && 0 > strcmp( m_CoilInf[m_nDspRecBase].cKizukenNo, m_CoilInf[wkRec].cKizukenNo) ) {	// 管理Noが 今 < 次 にならないとおかしい
			rec = wkRec;	// 1こ次のレコード

			LOG(em_WAR), "[%s] 表示用コイル 仮紐付け コイルNo=%s, 命令No=%s", my_sThreadName, m_CoilInf[rec].data.cCoilNoIn, m_CoilInf[rec].data.cMeiNo);
			syslog(257, "[コイルNo=%s, 命令No=%s]", m_CoilInf[rec].data.cCoilNoIn, m_CoilInf[rec].data.cMeiNo);


			// ホントに何もわかんない (検査開始直後とかしかありえないはず)
		}else {
			LOG(em_ERR), "[%s] 表示用コイル情報 紐付け失敗！！", my_sThreadName);
			syslog(258, "");
		}
	}
	memset( &m_CoilNext_Dsp, 0x00, sizeof(m_CoilNext_Dsp));			// 表示用次コイルバッファ初期化


#else			// リアルタイム表示
	// 表示レコードと検査レコードは一緒
	rec = m_nKenRecBase;
#endif

	//// 整合性チェック
	if(-1 == rec) return false;					// レコード検索失敗
	if( rec == m_nDspRecBase) return false;		// 今表示中コイルと同じ (ありえない)


	//// 今通板中のコイルの実績セット
	//// (上流側の表示基点のみ保存実施)
	if(NULL != m_pCoilBaseDsp) {
		m_pCoilBaseDsp->nDspLength = (long)dLen; 
		SaveDB_UpdateCoilResult(MODE_DSP_WPD_ON, m_pCoilBaseDsp);

	}

	//// 表示用コイル切り替え
	m_nDspRecBase				= rec;
	m_pCoilBaseDsp				= &m_CoilInf[rec];
	m_pCoilBaseDsp->bDspOk		= true;
	LOG(em_MSG), "[%s] 表示用コイル切り替え完了 rec=%d コイルNo=%s", my_sThreadName, rec, m_CoilInf[rec].data.cCoilNoIn);
	syslog(260, "[コイルNo=%s]", m_CoilInf[rec].data.cCoilNoIn);

	//// 他機能へ通知
	OpSendFunc::SendOp_CoilInf_Dsp(m_pCoilBaseDsp);
	return true;
}

//==========================================
// 検査装置位置 対象コイルの参照位置変更 (今検査装置位置のレコードの奴にする) [←HtRecv]
//------------------------------------------
// int	men	: 表裏
//==========================================
void CoilBaseManager::ChangeCoilKenPointer(int men)
{
	AutoLock autolock(&m_csLock);
	
	//// 検査装置位置 の参照ポインタを変更
	m_pCoilKenV[men] = &m_CoilInf[m_nKenRecBase];
	//LOG(em_MSG), "[%s] 検査装置位置 対象コイルの参照位置変更完了 管理No=%s ID=%d rec=%d SetRec[0][0]=%d SetRec[0][1]=%d SetRec[1][0]=%d SetRec[1][1]=%d men=%d"
	//	, my_sThreadName, m_pCoilKenV[men]->cKizukenNo, m_nKenRecBase, m_pCoilKenV[men]->rec
	//	, m_pCoilKenV[men]->nSetRec_St[0][0], m_pCoilKenV[men]->nSetRec_St[0][1]
	//	, m_pCoilKenV[men]->nSetRec_St[1][0], m_pCoilKenV[men]->nSetRec_St[1][1]
	//	, men);
}

//==========================================
// 表示用 各位置 対象コイルの参照位置変更 (今各位置のレコードの奴にする) [←PlgManager]
//------------------------------------------
// EM_POS_DSP	pos		: 対象位置 (上位3項目は、こない)
// double		dLenIn	: 現在の通過位置での入側単位の距離 [mm]
// double		dLenOut	: シャー位置での出側単位の距離 [mm]
//==========================================
void CoilBaseManager::ChangeCoilDspPointer(EM_POS_DSP pos, double dLenIn, double dLenOut)
{
	AutoLock autolock(&m_csLock);

	//// 参照ポインタを変更 (表示基点～一番下流の位置までの間に一本コイルが入らない前提)
	// 各位置のポインタにセットする
	m_pCoilDsp[pos]    = m_pCoilBaseDsp;

	//// 次のコイルの先端が今のシャーカット単位の距離となる
	//m_pCoilDsp[pos]->nOutCoilPos = (long)dLenOut;
}

//==========================================
// 表示用 シャーカット位置 対象コイルの参照位置変更
// (今各位置のレコードの奴にする) [←PlgManager]
//------------------------------------------
// double dLen_In	
// double dLen_Out	
//==========================================
void CoilBaseManager::ChangeCoilShaPointer(double dLen_In, double dLen_Out)
{
	AutoLock autolock(&m_csLock);

	//// 今までシャーカット位置を通過していたコイルが対象
	if( NULL != m_pCoilDsp[POS_DSP_SHR_I]) {
		//// 入側単位のホントの一番最後の実績の為、毎回updeteする事にする
		// コイル実績の疵検管理の出側距離情報を変更
		SaveDB_UpdateCoilResult(MODE_RECV_SHRCUT, m_pCoilDsp[POS_DSP_SHR_I]);
	}

	//---------------------------------------
	// 出側関連
	//---------------------------------------
	if (0 != strlen(m_CoilOutInf.cID))
	{
		// 前回の溶接点からシャーカットされてない
		// ※現出側コイル内に溶接点が２点目
		if ((MAX_SHALINK_COUNT - 1) <= m_CoilOutInf.nCnt)
		{
			LOG(em_MSG), "[%s] 現出側コイル内における%d点目の溶接点", my_sThreadName, m_CoilOutInf.nCnt);
		}
		// １出側コイルに３入側コイル以上は許容できない
		if (MAX_SHALINK_COUNT <= m_CoilOutInf.nCnt)
		{
			LOG(em_ERR), "[%s] シャーカット管理回数上限到達により、シャーカット情報を破棄", my_sThreadName);
			syslog(297, "[コイルNo=%.12s]", m_pCoilBaseDsp->data.cCoilNoIn);
		}
		else
		{
			// 前入側の実績を確定させる
			SetInCoilInf_End(&m_CoilOutInf, dLen_In, dLen_Out);

				// 次入側の情報をセットする(シャーカット中以外)
			SetInCoilInf_Init(m_pCoilBaseDsp, &m_CoilOutInf, dLen_Out);
#ifdef DEBUG_LOG_OUT_CHANGE
			LOG(em_FIL), "[%s] ChangeCoilShaPointer()で合成コイル認識[%d]", my_sThreadName, m_CoilOutInf[m_nShaRecBase].nCnt);
#endif
		}
	}

	//// 参照ポインタを変更 (表示基点～一番下流の位置までの間に一本コイルが入らない前提)
	int pos = POS_DSP_SHR_I;
	// 各位置のポインタにセットする
	m_pCoilDsp[pos]    = m_pCoilBaseDsp;

	// 次のコイルの先端が今のシャーカット単位の距離となる
	m_pCoilDsp[pos]->nOutCoilPos = (long)dLen_Out;
	LOG(em_MSG), "[%s] シャーカット位置 コイル切り替え 入側コイル(%.10s)%.20s In=%dmm Out=%dmm", my_sThreadName, m_pCoilDsp[pos]->data.cCoilNoIn, m_pCoilDsp[pos]->cKizukenNo, (long)dLen_In, (long)dLen_Out );

}

//==========================================
// 検査装置位置 対象出側物理コイルの参照位置変更
// (今検査装置位置のレコードの奴にする) [←HtRecv]
//------------------------------------------
// int	ou	: 表裏(0:表 1:裏)
//==========================================
void CoilBaseManager::ChangeCoilOutKenPointer(int ou)
{
	AutoLock autolock(&m_csLock);

	//// 参照ポインタを変更 (表示基点～一番下流の位置までの間に一本コイルが入らない前提)
	// 各位置のポインタにセットする
	m_pCoilOutKen[ou] = &m_CoilOutInf;


	// 各位置の参照ポイントが変更された？
	if( m_pCoilOutKen[0] != &m_CoilOutInf ||
		m_pCoilOutKen[1] != &m_CoilOutInf ) return;

	// 出側コイル情報有り？
	if( 0 == strlen( m_CoilOutInf.cID) ) return;

}

//==========================================
// 検査装置位置 出側物理コイルの入側コイル情報を更新
// ※現コイル反映の検査開始時に使用
//------------------------------------------
// 【呼び出し元】
//  自
//		MainInstance
//==========================================
void CoilBaseManager::ReloadInCoil()
{
	AutoLock autolock(&m_csLock);
	
	memset(&m_CoilOutInf.incoil[0], 0x00, sizeof(m_CoilOutInf.incoil[0]));
	m_CoilOutInf.nCnt = 0;

#ifdef DEBUG_LOG_OUT_CHANGE
	LOG(em_FIL), "[%s] 現コイル反映にあわせて更新[%d]", my_sThreadName, m_CoilOutInf[m_nShaRecBase].nCnt);
#endif
}

//==========================================
// 出側コイルの次の入側コイル情報をセット
//------------------------------------------
// COIL_INF*		pCoilIn		: 入側物理コイル情報
// COIL_OUT_INF*	pCoilOut	: 出側物理コイル情報
// double			dOutLen		: 出側通板距離
//==========================================
void CoilBaseManager::SetInCoilInf_Init(COIL_INF* pCoilIn, COIL_OUT_INF* pCoilOut, double dOutLen)
{
	// 次入側の情報をセットする(シャーカット中以外)
	int nRec = pCoilOut->nCnt;

	memcpy( pCoilOut->incoil[nRec].cKizukenNo, pCoilIn->cKizukenNo, SIZE_KIZUKEN_NO);
	pCoilOut->incoil[nRec].nInCutNum = 1;
	pCoilOut->incoil[nRec].nFront = 0;
	pCoilOut->incoil[nRec].nOutPos = (int)dOutLen;
	pCoilOut->incoil[nRec].nSt_Spm = pCoilIn->nSt_Spm;
	pCoilOut->incoil[nRec].dSpmHosei = pCoilIn->dSpmHosei;

	pCoilOut->nCnt += 1;
}

//==========================================
// 最後の入側コイル情報の確定情報をセット
//------------------------------------------
// COIL_OUT_INF*	pCoilOut	: 出側物理コイル情報
// double			dInLen		: 入側通板距離
// double			dOutLen		: 出側通板距離
//==========================================
void CoilBaseManager::SetInCoilInf_End(COIL_OUT_INF* pCoilOut, double dInLen, double dOutLen)
{
	// 最後の入側の実績を確定させる
	int nRec = pCoilOut->nCnt;
	if(0 <= nRec-1) {
		pCoilOut->incoil[nRec - 1].nLength = (int)dInLen - pCoilOut->incoil[nRec - 1].nFront;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 実績保存関連

//==========================================
// コイル情報部 DB保存
// ※ 実際の物の流れで 基準面の検査装置位置通過時
//------------------------------------------
// 【呼び出し元】
//  自
//		Init
//		ChangeCoilKenBase
//==========================================
void CoilBaseManager::SaveDB_CoilInf()
{
	if(-1 == m_nKenRecBase ) {
		LOG(em_ERR), "[%s] DB保存時 コイル情報異常", my_sThreadName);
		syslog(261, "[コイル情報管理で現在レコード位置が不正=%d]", m_nKenRecBase);
		return;
	}

	//================================================
	//// 事前準備
	DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
	deliDB->kubun = DBManager::em_COILINF;
	deliDB->data = (BYTE*)new DBDATA_COIL_INF;
	DBDATA_COIL_INF* pWk   = (DBDATA_COIL_INF*)deliDB->data;			// エイリアス
	memset(pWk, 0x00, sizeof(DBDATA_COIL_INF));
	
	const COIL_INF*	 pCoil = &m_CoilInf[m_nKenRecBase];					// エイリアス


	//================================================
	//// データセット
	memcpy( pWk->cKizukenNo, pCoil->cKizukenNo, SIZE_KIZUKEN_NO);
	for(int ii=0; ii<NUM_MEN; ii++) {
		pWk->scode[ii] = pCoil->setting.scode[ii];
		pWk->pcode[ii] = pCoil->setting.pcode[ii];
	}

	SetTypeCoilInf(*pWk, pCoil->data_u);

	//================================================
	//// 登録依頼
	if( ! mque_pAddSQLData->AddToTail( deliDB, 20, 20) ) {			// 絶対登録 
		// 絶対登録も無理・・・
		LOG(em_ERR), "[%s] DB登録依頼キューフル [DBDATA_COIL_INF]", my_sThreadName);
		syslog(SYSNO_QUEFULL_ERR, "[DBDATA_COIL_INF]");
		delete deliDB->data;
		delete deliDB;
		// DBが異常 (通常ありえない)
		mcls_pStatus->AddQueMyErrWar( true, ON_QUE_COIL_INF );
	}
}

//==========================================
// コイル位置情報 DB保存
// ※ データの流れで 基準面でない方の面で 最小間隔が更新されたタイミング
//------------------------------------------
// int	ou	: 表裏 (両面検査時は、基準面でない方の面)
// int	row	: セット位置 (最小間隔=5m) (0オリジン)
//==========================================
void CoilBaseManager::SaveDB_CoilPosition(int ou, int row)
{
	//if(DIV_UNTEN_SMYU == mcls_pStatus->GetUnten()) return;
	if( 0>row || MAP_ROW_NUM <=row) return;				// 位置異常

	//// 事前準備
	DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
	deliDB->kubun = DBManager::em_COILPOSITION;
	deliDB->data = (BYTE*)new DBDATA_COIL_POSITION;
	DBDATA_COIL_POSITION* pWk = (DBDATA_COIL_POSITION*)deliDB->data;			// エイリアス
	
	const COIL_INF*		  pCoil = m_pCoilKenV[ou];								// エイリアス

	//// データセット
	// ヘッダ部
	memcpy( pWk->cKizukenNo, pCoil->cKizukenNo, SIZE_KIZUKEN_NO);
	pWk->nID = row;
	// データ部
	for(int ii=0; ii<NUM_MEN; ii++) {
		pWk->emKensa[ii]	= pCoil->emKensa[ii][row];
		pWk->emKiki[ii]		= pCoil->emKiki[ii][row];
	}
	for(int ii=0; ii<2; ii++) {
		pWk->dEdge[ii] = pCoil->dEdge[m_nDistPos][row][ii];						// エッジ位置
	}
	pWk->dWalk      = (pWk->dEdge[0] + pWk->dEdge[1]) / 2.0f;			// ウォーク量(ラインセンターからの距離)

	//// 登録依頼
	if( ! mque_pAddSQLData->AddToTailFreeCheck(deliDB, 50) ) {	// 登録
		LOG(em_ERR), "[%s] DB登録依頼キューフル [DB_COIL_POSITION] <%s> [%s] row:%d", my_sThreadName, DivNameManager::GetTorB(ou), pCoil->cKizukenNo, row);
		syslog(SYSNO_QUEFULL_WAR, "[DB_COIL_POSITION]");
		delete deliDB->data;
		delete deliDB;
	}
}

//==========================================
// コイル実績部 DB保存
// ※ データの流れで 基準面でない方の検査装置位置通過時
//------------------------------------------
// 【呼び出し元】
//  自スレッド
//		SetData_CoilResult_Virtual
//==========================================
void CoilBaseManager::SaveDB_CoilResult()
{
	//if(DIV_UNTEN_SMYU == mcls_pStatus->GetUnten()) return;
	if(-1 == m_nKenRecBase ) {
		LOG(em_ERR), "[%s] DB保存時 コイル実績異常", my_sThreadName);
		return;
	}

	//// ここまで来たら 実績登録済みとするよ
	m_pCoilKenV[m_nDistNotPos]->bCoilResultAddDb	= true;


	//================================================
	//// 事前準備
	DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
	deliDB->kubun = DBManager::em_COILRESULT;
	deliDB->data = (BYTE*)new DBDATA_COIL_RESULT;
	DBDATA_COIL_RESULT* pWk = (DBDATA_COIL_RESULT*)deliDB->data;			// エイリアス

	const COIL_INF*		pCoil = m_pCoilKenV[m_nDistNotPos];					// エイリアス


	//================================================
	//// データセット
	memcpy( pWk->cKizukenNo, pCoil->cKizukenNo, SIZE_KIZUKEN_NO);

	pWk->nEndMode					= pCoil->nEndMode;
	for(int ii=0; ii<NUM_MEN; ii++) pWk->nSt_Hantei[ii] = pCoil->nSt_Hantei[ii];
	for(int ii=0; ii<NUM_MEN; ii++) pWk->nSt_Light[ii]  = pCoil->nSt_Light[ii];
	for(int ii=0; ii<NUM_MEN; ii++) pWk->nSt_Cam_Inline[ii] = pCoil->nSt_Daisya[ii];
	pWk->nSt_CamControl				= pCoil->nSt_CamControl;
	pWk->nSt_CamSwitch				= pCoil->nSt_CamSwitch;
	pWk->nSt_Procon					= pCoil->nSt_Procon;
	pWk->nSt_FF						= pCoil->nSt_FF;
	pWk->nSt_Sqence					= pCoil->nSt_Sqence;
	for(int ii=0; ii<NUM_MEN; ii++) pWk->nSt_Cam_Inline[ii]	= pCoil->nSt_Daisya[ii];
	for(int ii=0; ii<NUM_MEN; ii++) pWk->nSt_DbSkip[ii]		= pCoil->nSt_DbSkip[ii];
	for(int ii=0; ii<NUM_MEN; ii++) pWk->nSt_FrameSkip[ii]  = pCoil->nSt_FrameSkip[ii];
	for(int ii=0; ii<NUM_MEN; ii++) {
		for(int jj=0; jj<NUM_HANTEI; jj++) {
			pWk->nHtID[ii][jj] = pCoil->nHtID[ii][jj];
		}
	}
	for(int ii=0; ii<NUM_MEN; ii++) {
		for(int jj=0; jj<NUM_RPIEXEC; jj++) {
			pWk->nRpiID[ii][jj] = pCoil->nRpiID[ii][jj];
		}
	}
	memcpy( &pWk->setting, &pCoil->setting, sizeof(COIL_SETTING));
	
	pWk->nSpmStat					= pCoil->nSt_Spm;
	pWk->nLength					= pCoil->nLength;
	pWk->nLengthSub					= pCoil->nLengthSub;
	pWk->nLengthDsp					= pCoil->nDspLength;
	pWk->nOutCoilPos				= pCoil->nOutCoilPos;

	pWk->bStartWpdOk				= (0 == pCoil->nStartWpdOk ? true:false);
	pWk->nCoilCnt					= pCoil->nCoilCnt;

	pWk->emUnten					= pCoil->emUnten;
	for(int ii=0; ii<NUM_MEN; ii++) pWk->emOneKensa[ii]  = pCoil->emOneKensa[ii]; 
	for(int ii=0; ii<NUM_MEN; ii++) pWk->emOneKiki[ii]   = pCoil->emOneKiki[ii]; 

	pWk->emPlg						= pCoil->emPlg;
	pWk->nToyuSply					= pCoil->nSt_ToyuSplr;

	// そもそも実績送信可能か判定
	bool bPutOk = true;			// 送信許可
	if( pCoil->bDummeyCoil )				bPutOk = false;		// コイル情報が割り付いていない
	if( 1 != pCoil->nEndMode )				bPutOk = false;		// 途中コイル終わりは、対象外
	if( DIV_PLG_TEST == pCoil->emPlg )		bPutOk = false;		// テストパルス時は、対象外
	if( DIV_UNTEN_NOMAL != pCoil->emUnten )	bPutOk = false;		// 運転状態が正常以外は時は、対象外
	if( pCoil->setting.bWalm )				bPutOk = false;		// ウォーマ材の場合は、対象外
	
	pWk->nResultSendOk		= bPutOk ? 0 : 1;

	// 輝度(カメラ - 角度 - 位置(CLR))
	for(int men=0; men<NUM_MEN; men++){
		for(int cam=0; cam<NUM_CAM_POS; cam++){
			for(int angle=0; angle<MAX_CAMANGLE; angle++){
				int camset = men * NUM_CAM_POS + cam;
				pWk->nBr[camset][angle][0] = m_nBrCenter[men][cam][angle];
			}
		}
	}

	// 露光時間(表裏 - カメラ - アングル)
	for(int men=0; men<NUM_MEN; men++) {
		for(int cam=0; cam<NUM_CAM_POS; cam++) {
			for(int angle=0; angle<MAX_CAMANGLE; angle++) {
				int camset = men * NUM_CAM_POS + cam;
				pWk->nExp[camset][angle] = GetCamExpTime(men, cam, angle);	// 露光時間
			}
		}
	}

	// ゲイン(表裏 - カメラ - アングル)
	for(int men=0; men<NUM_MEN; men++) {
		for(int cam=0; cam<NUM_CAM_POS; cam++) {
			for(int angle=0; angle<MAX_CAMANGLE; angle++) {
				int camset = men * NUM_CAM_POS + cam;
				pWk->nGain[camset][angle] = (int)GetCamGain(men, cam, angle);	// ゲイン
			}
		}
	}

	LOG(em_MSG), "[%s] DB COIL_RESULT 登録 [%s][%s] 外部送信[%s]", my_sThreadName, pCoil->cKizukenNo, pCoil->data.cCoilNoIn, (bPutOk?"○":"×"));
	LOG(em_MSG), "<<<----------------------------------------------");



	//================================================
	//// 登録依頼
	if( ! mque_pAddSQLData->AddToTail( deliDB, 20, 20) ) {			// 絶対登録 
		// 絶対登録も無理・・・
		LOG(em_ERR), "[%s] DB登録依頼キューフル [DBDATA_COIL_RESULT]", my_sThreadName);
		syslog(SYSNO_QUEFULL_ERR, "[DBDATA_COIL_RESULT]");
		delete deliDB->data;
		delete deliDB;
		// DBが異常 (通常ありえない)
		mcls_pStatus->AddQueMyErrWar( true, ON_QUE_COIL_RESULT );
	}
}

//==========================================
// カット位置 DB保存
// ※ シャーカット確定時
//------------------------------------------
// int					nMode	: カット区分
// SHRCUT_ITEM const*	cut		: カット情報
//==========================================
void CoilBaseManager::SaveDB_CoilCutPos(int nMode, SHRCUT_ITEM const* cut)
{
	//if(DIV_UNTEN_SMYU == mcls_pStatus->GetUnten()) return;
	COIL_INF* pCoil = m_pCoilDsp[POS_DSP_SHR_I];		// 現シャー位置通過中のコイル情報ポインタ

		//================================================
		//// 事前準備
		DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
		deliDB->kubun = DBManager::em_COILCUTPOS;
		deliDB->data = (BYTE*)new DBDATA_COIL_CUT_POS;
		DBDATA_COIL_CUT_POS* pWk   = (DBDATA_COIL_CUT_POS*)deliDB->data;		// エイリアス
	
		//================================================
		//// データセット
		memcpy( pWk->cKizukenNo, cut->cKizukenNo, SIZE_KIZUKEN_NO);
		pWk->nMode		= nMode;
		memcpy(&pWk->data, &m_CoilOutInf, sizeof(m_CoilOutInf));
		
		//================================================
		//// 登録依頼
		if( ! mque_pAddSQLData->AddToTailFreeCheck(deliDB, 30) ) {	// 登録
			LOG(em_ERR), "[%s] DB登録依頼キューフル [DB_COIL_CUT_POS]", my_sThreadName);
			syslog(SYSNO_QUEFULL_WAR, "[DB_COIL_CUT_POS]");
			delete deliDB->data;
			delete deliDB;
	}
}


//==========================================
// 疵画像保存数リセット要求
// ※ ラストカット時
//------------------------------------------
// char const* cKizukenNo	管理No
//==========================================
void CoilBaseManager::ResetDB_DefectImg(char const* cKizukenNo)
{

	//================================================
	//// 事前準備
	DBManager::DeliveryDB *deliDB = new DBManager::DeliveryDB;
	deliDB->kubun = DBManager::em_DB_COUNT_RESET;
	deliDB->data = (BYTE*)new DBDATA_COUNT_RESET;
	DBDATA_COUNT_RESET* pWk   = (DBDATA_COUNT_RESET*)deliDB->data;		// エイリアス
	
	//================================================
	//// データセット
	memcpy( pWk->cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);
	pWk->nTorB	= 2;					// 両面
		
	//================================================
	//// 登録依頼
	if( ! mque_pAddSQLData->AddToTail( deliDB, 20, 20 ) ) {			// 絶対登録
		// 絶対登録も無理・・・
		LOG(em_ERR), "[%s] DB登録依頼キューフル [DBDATA_COUNT_RESET]", my_sThreadName);
		syslog(SYSNO_QUEFULL_ERR, "[DBDATA_COUNT_RESET]");
		delete deliDB->data;
		delete deliDB;
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// カメラコントローラ露光時間

//==========================================
// 指定カメラの露光時間を取得
//------------------------------------------
// int	nMen	: 0…表, 1…裏	
// int	nCamPos	: 0…DS, 1…WS
// int	nAngId	: 0…正, 1…乱 ※色差は"0"固定
//==========================================
int	CoilBaseManager::GetCamExpTime(int nMen, int nCamPos, int nAngId)
{
	if (NULL == mcls_pStatus) return 0;
	int nCam = (nMen * NUM_CAM_POS) + nCamPos;
	return mcls_pStatus->GetExpTime(nCam, nAngId);
}
#ifdef IS_TO_CAMERA_TBL
//==========================================
// 指定カメラの状態を取得
//------------------------------------------
// int	nMen	: 0…表, 1…裏	
// int	nCamPos	: 0…DS, 1…WS
// int	nAngId	: 0…正, 1…乱 ※色差は"0"固定
//==========================================
int	CoilBaseManager::GetCamMode(int nMen, int nCamPos, int nAngId)
{
	if(NULL == mtbl_pCam) return 0;
	int nCam = (nMen * NUM_CAM_POS * MAX_PHYSICAL_CAMANGLE) + (nCamPos * MAX_PHYSICAL_CAMANGLE) + nAngId;
	return mtbl_pCam->caminf[nCam].nMode; 
}
#endif
//==========================================
// 指定カメラのカメラゲインを取得
//------------------------------------------
// int	nMen	: 0…表, 1…裏	
// int	nCamPos	: 0…DS ～ 3…WS
// int	nColor	: 0…R, 1…G, 2…B
//==========================================
float	CoilBaseManager::GetCamGain(int nMen, int nCamPos, int nColor)
{
	if (NULL == mcls_pStatus) return 0;
	int nCam = (nMen * NUM_CAM_POS) + nCamPos;
	return (float)(mcls_pStatus->GetCamGain(nCam, nColor) / 100.0);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 照明装置
//==========================================
// 照明装置調光値を取得
//------------------------------------------
// int	nMen	: 0…表, 1…裏	
// int	nColor	: 0…R, 1,2…G, 3,4…B
//==========================================
int	CoilBaseManager::GetLedValue(int nMen, int nColor)
{
	if (NULL == mcls_pStatus) return 0;
	return mcls_pStatus->GetLedValue(nMen, nColor);
}



//////////////////////////////////////////////////////////////////////////////////
// 小物

#define SET_DATA_COIL_S(prm1, prm2)			memcpy(prm1, prm2, sizeof(prm1));	
#define SET_DATA_COIL_V(prm1, prm2, size)	prm1 = KizuFunction::AtoIex(prm2, size);	

//==========================================
// TYPE_EPC_COIL作成
//------------------------------------------
// TYPE_EPC_COIL&	data			: 変換後のデータ		
// const PRO_TYPE_DATA_COILINF&	p	: コピー元コイル情報
//==========================================
void CoilBaseManager::SetTypeEpcCoil(TYPE_EPC_COIL & data, const PRO_TYPE_DATA_COILINF & p)
{

	// 初期化
	memset(&data, 0x00, sizeof(data));

	// データセット
	SET_DATA_COIL_S(data.cMeiNo,			p.CoilInf.KeyInf.cMeiNo);										// [01] 命令No
	SET_DATA_COIL_S(data.cCoilNo,			p.CoilInf.KeyInf.cCoilNo);										// [02] コイルNo
	data.nSendPoint = p.CoilInf.nSendPoint;																	// [03] 送信位置 (0:WPD3 1:WPD4 2:WPD6-50m)
	data.nLenIn = p.CoilInf.nLenIn;																			// [04] 入側論理コイル 長さ [m]	
	data.nAtuZai = p.CoilInf.nAtuZai;																		// [05] 材料板厚
	data.nWidZai = p.CoilInf.nWidZai;																		// [06] 材料板幅
	data.nAtuSei = p.CoilInf.nAtuSei;																		// [07] 注文板厚
	data.nWidSei = p.CoilInf.nWidSei;																		// [08] 注文板幅
	data.nWidTrim = p.CoilInf.nWidTrim;																		// [09] 現工程トリム幅
	data.nNobiSPM = p.CoilInf.nNobiSPM;																		// [10] SPM伸び率
	data.nNobiTLV = p.CoilInf.nNobiTLV;																		// [11] TLV伸び率
	data.nCutF = p.CoilInf.nCutF;																			// [12] 入側におけるフロント端板カットの実測長
	data.nCutT = p.CoilInf.nCutT;																			// [13] 入側におけるテール端板カットの実測長
	data.nSPMRoolKei_Up = p.CoilInf.nSPMRoolKei_Up;															// [14] SPMワークロール径（上）
	data.nSPMRoolKei_Down = p.CoilInf.nSPMRoolKei_Down;														// [15] SPMワークロール径（下）

	data.nYobi[0] = 0;																						// [16] int予備10
	data.nYobi[1] = 0;																						// [17] int予備9
	data.nYobi[2] = 0;																						// [18] int予備8
	data.nYobi[3] = 0;																						// [19] int予備7
	data.nYobi[4] = 0;																						// [20] int予備6
	data.nYobi[5] = 0;																						// [21] int予備5
	data.nYobi[6] = 0;																						// [22] int予備4
	data.nYobi[7] = 0;																						// [23] int予備3
	data.nYobi[8] = 0;																						// [24] int予備2
	data.nYobi[9] = 0;																						// [25] int予備1

	SET_DATA_COIL_S(data.cKenKijun_O, p.CoilInf.cKenKijun_O);												// [26] 検査基準　表
	SET_DATA_COIL_S(data.cKenKijun_U, p.CoilInf.cKenKijun_U);												// [27] 検査基準　裏
	SET_DATA_COIL_S(data.cKurometo_Kind, p.CoilInf.cKurometo_Kind);											// [28] クロメート 実績種類
	SET_DATA_COIL_S(data.cKurometo_Umu, p.CoilInf.cKurometo_Umu);											// [29] クロメート 実績有無
	SET_DATA_COIL_S(data.cHinCode, p.CoilInf.cHinCode);														// [30] 品種
	SET_DATA_COIL_S(data.cMekkiKind, p.CoilInf.cMekkiCycle);												// [31] メッキ種類
	SET_DATA_COIL_S(data.cSyuZai, p.CoilInf.cSyuZai);														// [32] 主材質
	SET_DATA_COIL_S(data.cDifMaker, p.CoilInf.cDifMaker);													// [33] ディファレンシャルマーカー
	SET_DATA_COIL_S(data.cKenban, p.CoilInf.cKenban);														// [34] 原板区分
	SET_DATA_COIL_S(data.cMekkiCycle, p.CoilInf.cMekkiCycle);												// [35] メッキサイクル
	SET_DATA_COIL_S(data.cInLineSPM, p.CoilInf.cInLineSPM);													// [36] レベラー　インラインSPM
	SET_DATA_COIL_S(data.cKagakuToyu, p.CoilInf.cKagakuToyu);												// [37] 化学処理塗油
	SET_DATA_COIL_S(data.cAtukkaSPM, p.CoilInf.cAtukkaSPM);													// [38] オフラインSPM圧下率
	SET_DATA_COIL_S(data.cMekkiKubun, p.CoilInf.cMekkiKubun);												// [39] メッキ区分
	SET_DATA_COIL_S(data.cOmoteSyori1, p.CoilInf.cOmoteSyori1);												// [40] 表面処理1桁目
	SET_DATA_COIL_S(data.cOmoteSyori2, p.CoilInf.cOmoteSyori2);												// [41] 表面処理2桁目
	SET_DATA_COIL_S(data.cToyu, p.CoilInf.cToyu);															// [42] 塗油
	SET_DATA_COIL_S(data.cKouteiYobi, p.CoilInf.cKouteiYobi);												// [43] 工程記号予備
	SET_DATA_COIL_S(data.cBaund, p.CoilInf.cBaund);															// [44] バウンダリ合わせ
	SET_DATA_COIL_S(data.cCaplSyuZai, p.CoilInf.cCaplSyuZai);												// [45] CAPL 主材質
	SET_DATA_COIL_S(data.cMetuke, p.CoilInf.cMetuke);														// [46] 目付規格
	SET_DATA_COIL_S(data.cNi, p.CoilInf.cNi);																// [47] Ni付着量
	SET_DATA_COIL_S(data.cSodoTCM, p.CoilInf.cSodoTCM);														// [48] TCMロール粗度
	SET_DATA_COIL_S(data.cSpmUmu, p.CoilInf.cSpmUmu);														// [49] SPM圧下有無
	SET_DATA_COIL_S(data.cNextLine, p.CoilInf.cNextLine);													// [50] 計画次工程コード
	SET_DATA_COIL_S(data.cMuke, p.CoilInf.cMuke);															// [51] 向先
	SET_DATA_COIL_S(data.cYouto, p.CoilInf.cYouto);															// [52] 用途
	SET_DATA_COIL_S(data.cCaplSiage, p.CoilInf.cCaplSiage);													// [53] CAPL表面仕上げ
	SET_DATA_COIL_S(data.cCaplSanUmu, p.CoilInf.cCaplSanUmu);												// [54] CAPL酸洗有無
	SET_DATA_COIL_S(data.cHinMode, p.CoilInf.cHinMode);														// [55] 品種区分
	SET_DATA_COIL_S(data.cGaCrKbn, p.CoilInf.cGaCrKubun);													// [56] GA/CR区分
	SET_DATA_COIL_S(data.cSpmJiseki, p.CoilInf.cSpmJisekiUmu);												// [57] SPM実績有無
	SET_DATA_COIL_S(data.cKouteiCode, p.CoilInf.cKouteiCode);												// [58] 工程コード
	SET_DATA_COIL_S(data.cYobi, p.CoilInf.cYobi1);															// [59] 
}

//==========================================
// DBDATA_COIL_INF作成
//------------------------------------------
// DBDATA_COIL_INF&			data	: 変換後のデータ
// PRO_TYPE_DATA_COILINF&	coil	: コピー元コイル情報
//==========================================
void CoilBaseManager::SetTypeCoilInf(DBDATA_COIL_INF& data, const PRO_TYPE_DATA_COILINF& p)
{

	//// データセット
	memcpy(data.cCoilKey, &p.CoilInf.KeyInf, sizeof(p.CoilInf.KeyInf));

	SET_DATA_COIL_S(data.cCoilNo,					p.CoilInf.KeyInf.cCoilNo);			// コイルNo
	SET_DATA_COIL_S(data.cMeiNo,					p.CoilInf.KeyInf.cMeiNo);			// 命令No
	
	data.nSendPoint = p.CoilInf.nSendPoint;												// 送信位置
	data.nLenIn = p.CoilInf.nLenIn;														// 入側論理コイル 長さ
	data.nAtuZai = p.CoilInf.nAtuZai;													// 材料板厚
	data.nWidZai = p.CoilInf.nWidZai;													// 材料板幅
	data.nAtuSei = p.CoilInf.nAtuSei;													// 材料板厚
	data.nWidSei = p.CoilInf.nWidSei;													// 材料板幅
	data.nWidTrim = p.CoilInf.nWidTrim;													// 現工程トリム幅
	data.nNobiSPM = p.CoilInf.nNobiSPM;													// SPM伸び率
	data.nNobiTLV = p.CoilInf.nNobiTLV;													// TLV伸び率
	data.nCutF = p.CoilInf.nCutF;														// 入側におけるフロント端板カットの実測長
	data.nCutT = p.CoilInf.nCutT;														// 入側におけるテール端板カットの実測長
	data.nSPMRoolKei_Up = p.CoilInf.nSPMRoolKei_Up;										// PSMワークロール径（上）
	data.nSPMRoolKei_Down = p.CoilInf.nSPMRoolKei_Down;									// SPMワークロール径（下）

	SET_DATA_COIL_S(data.cKenKijun_O, p.CoilInf.cKenKijun_O);							// 検査基準 表
	SET_DATA_COIL_S(data.cKenKijun_U, p.CoilInf.cKenKijun_U);							// 検査基準 裏
	SET_DATA_COIL_S(data.cKurometo_Kind, p.CoilInf.cKurometo_Kind);						// クロメート 実績種類
	SET_DATA_COIL_S(data.cKurometo_Umu, p.CoilInf.cKurometo_Umu);						// クロメート 実績有無
	SET_DATA_COIL_S(data.cHinCode, p.CoilInf.cHinCode);									// 品種
	SET_DATA_COIL_S(data.cMekkiKind, p.CoilInf.cMekkiKind);								// メッキ種類
	SET_DATA_COIL_S(data.cSyuZai, p.CoilInf.cSyuZai);									// 主材質
	SET_DATA_COIL_S(data.cDifMaker, p.CoilInf.cDifMaker);								// ディファレンシャルマーカー
	SET_DATA_COIL_S(data.cKenban, p.CoilInf.cKenban);									// 原板区分
	SET_DATA_COIL_S(data.cMekkiCycle, p.CoilInf.cMekkiCycle);							// メッキサイクル
	SET_DATA_COIL_S(data.cInLineSPM, p.CoilInf.cInLineSPM);								// レベラー インラインSPM
	SET_DATA_COIL_S(data.cKagakuToyu, p.CoilInf.cKagakuToyu);							// 化学処理塗油
	SET_DATA_COIL_S(data.cAtukkaSPM, p.CoilInf.cAtukkaSPM);								// オフラインSPM圧下率
	SET_DATA_COIL_S(data.cMekkiKubun, p.CoilInf.cMekkiKubun);							// メッキ区分
	SET_DATA_COIL_S(data.cOmoteSyori1, p.CoilInf.cOmoteSyori1);							// 表面処理1桁目
	SET_DATA_COIL_S(data.cOmoteSyori2, p.CoilInf.cOmoteSyori2);							// 表面処理2桁目
	SET_DATA_COIL_S(data.cToyu, p.CoilInf.cToyu);										// 塗油
	SET_DATA_COIL_S(data.cKouteiYobi, p.CoilInf.cKouteiYobi);							// 工程記号予備
	SET_DATA_COIL_S(data.cBaund, p.CoilInf.cBaund);										// バウンダリ合わせ
	SET_DATA_COIL_S(data.cCaplSyuZai, p.CoilInf.cCaplSyuZai);							// CAPL 主材質
	SET_DATA_COIL_S(data.cMetuke, p.CoilInf.cMetuke);									// 目付規格
	SET_DATA_COIL_S(data.cNi, p.CoilInf.cNi);											// Ni付着量
	SET_DATA_COIL_S(data.cSodoTCM, p.CoilInf.cSodoTCM);									// TCMロール粗度
	SET_DATA_COIL_S(data.cSpmUmu, p.CoilInf.cSpmUmu);									// SPM圧下有無
	SET_DATA_COIL_S(data.cNextLine, p.CoilInf.cNextLine);								// 計画次工程コード
	SET_DATA_COIL_S(data.cMuke, p.CoilInf.cMuke);										// 向先
	SET_DATA_COIL_S(data.cYouto, p.CoilInf.cYouto);										// 用途
	SET_DATA_COIL_S(data.cCaplSiage, p.CoilInf.cCaplSiage);								// CAPL表面仕上げ
	SET_DATA_COIL_S(data.cCaplSanUmu, p.CoilInf.cCaplSanUmu);							// CAPL酸洗有無
	SET_DATA_COIL_S(data.cHinMode, p.CoilInf.cHinMode);									// 品種区分
	SET_DATA_COIL_S(data.cGaCrKubun, p.CoilInf.cGaCrKubun);								// GA/CR区分
	SET_DATA_COIL_S(data.cSpmJisekiUmu, p.CoilInf.cSpmJisekiUmu);						// SPM圧下実績有無
	SET_DATA_COIL_S(data.cKouteiCode, p.CoilInf.cKouteiCode);							// 工程コード
}

#undef SET_DATA_COIL_S
#undef SET_DATA_COIL_V



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Get

//==========================================
// レコード位置取得
//------------------------------------------
// int		rec		: 基準レコード
// int		index	: 0:現在位置 1:一個前 2:二個前 3:三個前 -1:次
// 戻り値	int		: レコード位置
//==========================================
int CoilBaseManager::GetRecIndex(int rec, int index)
{
	if(0 == index) {					// 現在位置
		return rec;			
	} else {							// 指定前
		int recval = rec - index;
		if( 0 > recval) recval = SIZE_COIL_BUF + recval;
		if( SIZE_COIL_BUF <= recval) recval = recval - SIZE_COIL_BUF;
		return recval;
	}
}

//==========================================
// レコード位置取得
//------------------------------------------
// char const*	pKizukenNo	: 管理No
// 戻り値		int			: レコード位置
//==========================================
int CoilBaseManager::GetRecKizukenNo(char const* pKizukenNo)
{
	COIL_INF const* pWk;
	if(-1 == m_nKenRecBase) return -1;

	for(int ii=0; ii<SIZE_COIL_BUF; ii++) {
		pWk = GetCoilIndex(m_nKenRecBase, ii);

		if(0 == memcmp( pWk->cKizukenNo, pKizukenNo, SIZE_KIZUKEN_NO ) ) return pWk->rec;
	}
	// ここまで来たら該当無し
	return -1;
}

//==========================================
// 指定された管理Noのコイル情報
//------------------------------------------
// char const*	pKizukenNo	: 管理No
// 戻り値		COIL_INF*	: コイル情報
//==========================================
const COIL_INF* CoilBaseManager::GetCoilKizukenNo(char const* pKizukenNo)
{
	COIL_INF const* pWk;
	if(-1 == m_nKenRecBase) return NULL;

	for(int ii=0; ii<SIZE_COIL_BUF; ii++) {
		pWk = GetCoilIndex(m_nKenRecBase, ii);

		if(0 == memcmp( pWk->cKizukenNo, pKizukenNo, SIZE_KIZUKEN_NO ) ) return pWk;
	}
	// ここまで来たら該当無し
	return NULL;
}

//------------------------------------------
// 指定された管理Noのコイル情報  (呼ばれるたびにインクリンメントされるため注意)
// COIL_INF* pCoil 対象のコイル情報
// 戻り値:疵No(1オリジン)
//------------------------------------------
int CoilBaseManager::GetKizuNo(COIL_INF* pCoil)
{
	AutoLock autolock(&m_csLock);

	pCoil->nKizuNoNum ++;
	return pCoil->nKizuNoNum;
}

//==========================================
// 全部入り距離
//------------------------------------------
// int		pos			: 
// 戻り値	COIL_INF*	: コイル情報	
//==========================================
const COIL_INF* CoilBaseManager::GetCoilAll(int pos) const
{
	if (pos == DIV_LEN_POS_KEN_TOP || pos == DIV_LEN_POS_KEN_BOT) {
		int nMen = pos - DIV_LEN_POS_KEN_TOP;
		return GetCoilKenR(nMen);
	} else if( pos == DIV_LEN_POS_DSP_TOP) {
		return GetCoilBaseDsp();

	} else if( pos == DIV_LEN_POS_DSP_BOT) {
		return GetCoilBaseDsp();

	} else {
		return GetCoilDsp( (pos- DIV_DISP_POS_DSP_P1) );
	}
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EPC製 全長保存PC用

//==========================================
// UDP SOCKET生成
//------------------------------------------
// const char*	ip		: IPアドレス (例：192.9.200.255)
// WORD			port	: ポートNo (例：8100)
// 戻り値		BOOL	: 復帰情報
//==========================================
BOOL CoilBaseManager::SetUdpAllFrame(const char* ip, WORD port)
{
	// 初期化
	m_sockAllFrame = INVALID_SOCKET;
	memset(&m_addrAllFrame, 0x00, sizeof(m_addrAllFrame));

	// ソケット生成
	WSADATA wsaData;
	WORD wVersionRequired = MAKEWORD(2, 2);
	WSAStartup(wVersionRequired, &wsaData);
	m_sockAllFrame = socket(AF_INET, SOCK_DGRAM, 0);
	if (INVALID_SOCKET == m_sockAllFrame) return FALSE;
	m_addrAllFrame.sin_family = AF_INET;
	m_addrAllFrame.sin_addr.s_addr = inet_addr(ip);
    m_addrAllFrame.sin_port = htons(port);
	return TRUE;
}

//==========================================
//// UDP SOCKET破棄
//==========================================
void CoilBaseManager::ReSetUdpAllFrame()
{
	if( INVALID_SOCKET != m_sockAllFrame ) closesocket(m_sockAllFrame);
	m_sockAllFrame = INVALID_SOCKET;
}

//==========================================
// コイル情報 を 生画像PCに送信
//------------------------------------------
// TYPE_EPC_COIL*	pData	: コイル情報
//==========================================
void CoilBaseManager::SendUdpAllFrame(TYPE_EPC_COIL* pData)
{
	// UDPパケット送信
	sendto( m_sockAllFrame, (char *)pData, sizeof(TYPE_EPC_COIL), 0, (sockaddr *)&m_addrAllFrame, sizeof(sockaddr_in));
}

//==========================================
// レコード位置取得
//------------------------------------------
// int		rec		: 基準レコード
// int		index	: 0:現在位置 1:一個前 2:二個前 3:三個前 -1:次
// 戻り値	int		: レコード位置
//==========================================
int CoilBaseManager::GetShaRecIndex(int rec, int index)
{
	if(0 == index) {					// 現在位置
		return rec;			
	} else {							// 指定前
		int recval = rec - index;
		if( 0 > recval) recval = SIZE_COIL_OUT_BUF + recval;
		if( SIZE_COIL_BUF <= recval) recval = SIZE_COIL_OUT_BUF - recval;
		return recval;
	}
}



//------------------------------------------
// コイル実績の更新
// COIL_INF const*	pCoilInf 疵が含まれるコイル情報
//------------------------------------------
void CoilBaseManager::SaveDB_UpdateCoilResult(EM_MODE_COILRESULT_UPDATE emMode, COIL_INF const* pCoilInf)
{

	// 事前準備
	DBManager::DeliveryDB* pDeli = new DBManager::DeliveryDB;
	pDeli->kubun = DBManager::em_COILRESULT_UPDATE;
	pDeli->data = (BYTE*)new DBDATA_COIL_RESULT_UPDATE;
	DBDATA_COIL_RESULT_UPDATE* pWk = (DBDATA_COIL_RESULT_UPDATE*)pDeli->data;
	memset(pWk, 0x00, sizeof(DBDATA_COIL_RESULT_UPDATE));

	// データセット
	memcpy(pWk->cKizukenNo, pCoilInf->cKizukenNo, SIZE_KIZUKEN_NO);
	pWk->emMode = emMode;

	switch (emMode) {
	case MODE_RECV_SHRCUT:
		pWk->nOutCoilPos = pCoilInf->nOutCoilPos;
		break;
	case MODE_DSP_WPD_ON:
		pWk->nDspLength = pCoilInf->nDspLength;
		pWk->nSpmUmu = pCoilInf->nSt_Spm;
		break;
	}


	// 登録依頼
	if (!mque_pAddSQLData->AddToTailFreeCheck(pDeli, 20)) {
		// キューフル
		delete pDeli->data;
		delete pDeli;

		// DBが異常 (通常ありえない)
		LOG(em_ERR), "[%s] DB登録依頼キューフル [DBDATA_COIL_RESULT_UPDATE]", my_sThreadName);
		syslog(SYSNO_QUEFULL_WAR, "[DBDATA_COIL_RESULT_UPDATE]");
		mcls_pStatus->AddQueMyErrWar( true, ON_QUE_COIL_RESULT );
	}
}