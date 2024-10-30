#include "StdAfx.h"
#include "CameraManager.h"

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
CameraManager::CameraManager(void) :
m_bIsPulsu(false),
m_bIsConnect(false),
m_bNowSend(false),
m_emMainMode(MODE_NONE),
m_nSubMode(0),
m_nCamMode(0),
m_nSwMode(0),
m_emPLGmode(DIV_PLG_LINE),
#ifdef IS_TO_CAMERA_TBL
m_hCam(NULL),
#endif
mcls_p232c(NULL),
mcls_pLog(NULL)
{
	// 何も無ければこれ
	m_nUnit			= 0;
	my_sThreadName	= TO_CAMERA;
	m_cMySession	= TO_CAMERA;
	m_cPrmCommonTblName	= DB_PARAM_COMMON;
	m_nMyCamNum		= NUM_CAMERA;
	m_nMyCamIdStart	= 0;
	m_nMyCamIdEnd	= m_nMyCamIdStart+m_nMyCamNum;


	memset(m_cRecvBuf, 0x00, sizeof(m_cRecvBuf));
	memset(m_cSendBuf, 0x00, sizeof(m_cSendBuf));
	memset(m_cSendManyu, 0x00, sizeof(m_cSendManyu));
	memset(&m_typAgcInf, 0x00, sizeof(m_typAgcInf));

	for(int ii=0; ii<NUM_CAMERA; ii++){
		m_bAlame[ii] = true;
		m_nNowExpTime[ii] = 0;
	}

	// 表面状態は、デフォルト表面状態とする
	m_nNowScode[0] = MAX_SCODE;
	m_nNowScode[1] = MAX_SCODE;

	// パラメータクリア
	memset(&m_Param, 0x00, sizeof(m_Param));

	// イベント生成
	m_evRecvEnd = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evSendEnd = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evReTry	= CreateWaitableTimer(NULL, FALSE, NULL);
	m_evAnserTimeOut = CreateWaitableTimer(NULL, FALSE, NULL);

#ifndef VER_AVALDATA
	//// 配線ミス対策 (ようは、DS正=[0]がどのカメラIDかを記述)
	// デフォルト
	for(int ii=0; ii<NUM_CAMERA; ii++) {
		m_nCamID[ii] = ii;
	}
	// iniファイルでの指定値を取得
	CString strVal;
	int		nVal;
	for(int ii=0; ii<NUM_CAMERA; ii++) {
		strVal.Format("CAMID_%d", ii+1);
		nVal = GetPrivateProfileInt(m_cMySession, strVal, 0, TASKINI_NAME);
		if( 0 >= nVal || 32 < nVal) continue;

		m_nCamID[ii] = nVal -1 ;
	}
	// 任意で変更
	//m_nCamID[0] = 4 -1;
	//m_nCamID[1] = 8 -1;
	//m_nCamID[2] = 3 -1;
	//m_nCamID[3] = 7 -1;
	//m_nCamID[4] = 2 -1;
	//m_nCamID[5] = 6 -1;
	//m_nCamID[6] = 1 -1;
	//m_nCamID[7] = 5 -1;

	//m_nCamID[8]  = 12 -1;
	//m_nCamID[9]  = 16 -1;
	//m_nCamID[10] = 11 -1;
	//m_nCamID[11] = 15  -1;
	//m_nCamID[12] = 10 -1;
	//m_nCamID[13] = 14 -1;
	//m_nCamID[14] = 9 -1;
	//m_nCamID[15] = 13 -1;
#endif

	// フィルター値取得 [0.1μs]
	m_nFilter = GetPrivateProfileInt(m_cMySession, "FILTER", 0, TASKINI_NAME);
	// # Hコマンド (初期値0) ※ジッター対策。振幅フレ対策。べき乗で移動平均を取る。0～5
	m_nH = GetPrivateProfileInt(m_cMySession, "COMMAND_H", 0, TASKINI_NAME);

	// 任意コマンド
	m_nCmdIniInputNum = 0;
	CString sKey;
	while(true) {
		char cWk[64];
		sKey.Format("CMD%d", m_nCmdIniInputNum);
		GetPrivateProfileString( m_cMySession, sKey, "", cWk, sizeof(cWk), TASKINI_NAME );
		if(0 == strlen(cWk)) break;

		m_lstCmdIniInput.push_back( cWk );
		m_nCmdIniInputNum++;
	}
}

//------------------------------------------
// デストラクタ
//------------------------------------------
CameraManager::~CameraManager(void)
{
	CloseHandle(m_evRecvEnd);
	CloseHandle(m_evSendEnd);
	CloseHandle(m_evReTry);
	CloseHandle(m_evAnserTimeOut);

	// クローズ
	if(NULL != mcls_p232c) {
		SAFE_DELETE(mcls_p232c);
	}

	// カメコン情報テーブル
#ifdef IS_TO_CAMERA_TBL
	if( NULL != m_hCam ) mem_close(&m_hCam, (LPVOID *)&mtbl_pCam);							// カメコン情報
#endif
}

//------------------------------------------
// 自クラスセット
// int nUnit			ユニット番号 (0オリジン)
// char const* sSession	Iniセクション名
//------------------------------------------
void CameraManager::SetClass(int nUnit, char* sSession)
{
	m_nUnit = nUnit;
	m_cMySession = sSession;
	my_sThreadName.Format("CamMgr_%d", m_nUnit); 


	// その他特殊？初期設定項目？読込
	char	cWk[64];
	int		nWk;
	GetPrivateProfileString(m_cMySession, "PARAM_CNT_TBL_NAME", m_cPrmCommonTblName, cWk, sizeof(cWk), TASKINI_NAME);	m_cPrmCommonTblName	= cWk;
	m_nMyCamNum		= GetPrivateProfileInt(m_cMySession, "CAMID_NUM", m_nMyCamNum, TASKINI_NAME);
	m_nMyCamIdStart = GetPrivateProfileInt(m_cMySession, "CAMID_START", m_nMyCamIdStart, TASKINI_NAME); 
	m_nMyCamIdEnd	= m_nMyCamIdStart+m_nMyCamNum;

	// フィルター値取得 [0.1μs]
	m_nFilter = GetPrivateProfileInt(m_cMySession, "FILTER", 0, TASKINI_NAME);
	// # Hコマンド (初期値0) ※ジッター対策。振幅フレ対策。べき乗で移動平均を取る。0～5
	m_nH = GetPrivateProfileInt(m_cMySession, "COMMAND_H", 0, TASKINI_NAME);

	// 任意コマンド
	m_nCmdIniInputNum = 0;
	CString sKey;
	while (true) {
		char cWk[64];
		sKey.Format("CMD%d", m_nCmdIniInputNum);
		GetPrivateProfileString(m_cMySession, sKey, "", cWk, sizeof(cWk), TASKINI_NAME);
		if (0 == strlen(cWk)) break;

		m_lstCmdIniInput.push_back(cWk);
		m_nCmdIniInputNum++;
	}
}

//------------------------------------------
// スレッド
//------------------------------------------
UINT WINAPI CameraManager::MainThread(LPVOID param)
{
	////// メインインスタンス取り出し
	CameraManager *p = (CameraManager *)param;
	p->SetTid();								// 自スレッドIDセット

	// WaitForMultipleObjectsイベント
	HANDLE	hArray[] = {p->my_evStop,
						p->m_evReTry,
						p->m_evSendEnd,
						p->m_evRecvEnd,
						p->m_evAnserTimeOut,
						p->gque_Deli.g_evSem 
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// スレッド終了
			EV_RETRY,						// 再接続開始
			EV_SENDEND,						// 送信完了
			EV_RECVEND,						// 受信完了
			EV_ANSERTIMEOUT,				// 送信アンサータイムアウト
			EV_QUE,							// メールスロット受信
			EV_END	};
	

	//// 共有メモリ
	// カメコン情報テーブル
#ifdef IS_TO_CAMERA_TBL
//*/----> ◆
#ifdef LOCAL
	p->m_hCam = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(TO_CAMERA_TBL), TBL_TO_CAMERA_NAME);
	p->mtbl_pCam =(TO_CAMERA_TBL*) MapViewOfFile(p->m_hCam, FILE_MAP_ALL_ACCESS, 0, 0, 0);
#endif
//*///<<---

	mem_connect(&p->m_hCam, (LPVOID *)&p->mtbl_pCam, TBL_TO_CAMERA_NAME);
	if(p->m_hCam == NULL) {
		pLOG(em_ERR), "[%s] カメコン情報テーブル アクセス失敗", p->my_sThreadName);
		syslog(SYSNO_MEM_OPEN, "カメコン情報テーブル アクセス失敗 [Unit=%d]", p->m_nUnit);
		_ASSERT(FALSE);
	}
	pLOG(em_INF), "[%s] カメコン情報テーブル アクセスモード", p->my_sThreadName);
#endif



	// パラメータ読み込み
	while(TRUE) {
		if( p->GetParamStatic() &&
			p->GetParam() ) break;
		pLOG(em_MSG), "[%s] パラメータ読込失敗(DB接続異常？！)。リトライ中・・・", p->my_sThreadName);
		Sleep(3000);
	}

#ifdef VER_AVALDATA
	//// 配線ミス対策
	// もろもろクリア
	for(int ii=0; ii<NUM_CAMLINKSW; ii++) {
		p->m_nCamNum[ii] = 0;
		for(int jj=0; jj<MAX_CAMLINKSW_ALLINPUT; jj++) {
			p->m_nCamID[ii][jj] = 0;
			p->m_nInpCh[ii][jj] = 0;
		}
	}
	// iniファイルでの指定値を取得
	CString strVal;
	int     nCamID;
	int		nInpCh;
	for(int ii=0; ii<NUM_CAMLINKSW; ii++) {
		for(int jj=0; jj<MAX_CAMLINKSW_ALLINPUT; jj++) {
			// カメラID
			strVal.Format("CAMID_%d_%d", p->m_Param.nSwitchMasterID[ii], jj+1);
			nCamID = GetPrivateProfileInt(p->m_cMySession, strVal, 0, TASKINI_NAME);
			if( 0 >= nCamID || NUM_CAMERA < nCamID) continue;

			// 入力CH
			strVal.Format("INPCH_%d_%d", p->m_Param.nSwitchMasterID[ii], jj+1);
			nInpCh = GetPrivateProfileInt(p->m_cMySession, strVal, 0, TASKINI_NAME);
			if( 0 >= nInpCh || MAX_CAMLINKSW_ALLINPUT < nInpCh) continue;

			// セット
			p->m_nCamID[ii][jj] = nCamID -1;
			p->m_nInpCh[ii][jj] = nInpCh -1;
			p->m_nCamNum[ii]++;
		}
	}
	// デフォルト
	bool	bDefault = true;
	for(int ii=0; ii<NUM_CAMLINKSW; ii++) {
		if( 0 != p->m_nCamNum[ii] ) {
			bDefault = false;
			break;
		}
	}
	if( bDefault ) {
		for(int ii=0; ii<NUM_CAMERA; ii++) {


#if LINKSW_CAM_SELECT_MODE == 0		// カメラを各カメラリンクスイッチに前詰めで接続Ver
			p->m_nCamID[ ii / (MAX_CAMLINKSW_ALLINPUT) ][ ii % (MAX_CAMLINKSW_ALLINPUT) ] = ii;
			p->m_nInpCh[ ii / (MAX_CAMLINKSW_ALLINPUT) ][ ii % (MAX_CAMLINKSW_ALLINPUT) ] = ii % MAX_CAMLINKSW_ALLINPUT;
			p->m_nCamNum[ ii / (MAX_CAMLINKSW_ALLINPUT) ]++;
#elif LINKSW_CAM_SELECT_MODE == 1	// カメラを各カメラリンクスイッチに等分配で接続Ver
			p->m_nCamID[ ii / (NUM_CAMERA / NUM_CAMLINKSW) ][ ii % (NUM_CAMERA / NUM_CAMLINKSW) ] = ii;
			p->m_nInpCh[ ii / (NUM_CAMERA / NUM_CAMLINKSW) ][ ii % (NUM_CAMERA / NUM_CAMLINKSW) ] = ii % (NUM_CAMERA / NUM_CAMLINKSW);
			p->m_nCamNum[ ii / (NUM_CAMERA / NUM_CAMLINKSW) ]++;
#else								// 未指定であれば、昔ながらの前詰めで接続Ver
			p->m_nCamID[ ii / (MAX_CAMLINKSW_ALLINPUT) ][ ii % (MAX_CAMLINKSW_ALLINPUT) ] = ii;
			p->m_nInpCh[ ii / (MAX_CAMLINKSW_ALLINPUT) ][ ii % (MAX_CAMLINKSW_ALLINPUT) ] = ii % MAX_CAMLINKSW_ALLINPUT;
			p->m_nCamNum[ ii / (MAX_CAMLINKSW_ALLINPUT) ]++;
#endif
		}
	}
	// 任意で変更
	//p->m_nCamID[0][0] = 4 -1;
	//p->m_nCamID[0][1] = 8 -1;
	//p->m_nCamID[0][2] = 3 -1;
	//p->m_nCamID[0][3] = 7 -1;
	//p->m_nCamID[0][4] = 2 -1;
	//p->m_nCamID[0][5] = 6 -1;
	//p->m_nCamID[0][6] = 1 -1;
	//p->m_nCamID[0][7] = 5 -1;
	
	//p->m_nInpCh[0][0] = 4 -1;
	//p->m_nInpCh[0][1] = 8 -1;
	//p->m_nInpCh[0][2] = 3 -1;
	//p->m_nInpCh[0][3] = 7 -1;
	//p->m_nInpCh[0][4] = 2 -1;
	//p->m_nInpCh[0][5] = 6 -1;
	//p->m_nInpCh[0][6] = 1 -1;
	//p->m_nInpCh[0][7] = 5 -1;

	//p->m_nCamID[1][0] = 12 -1;
	//p->m_nCamID[1][1] = 16 -1;
	//p->m_nCamID[1][2] = 11 -1;
	//p->m_nCamID[1][3] = 15 -1;
	//p->m_nCamID[1][4] = 10 -1;
	//p->m_nCamID[1][5] = 14 -1;
	//p->m_nCamID[1][6] = 9  -1;
	//p->m_nCamID[1][7] = 13 -1;

	//p->m_nInpCh[1][0] = 4 -1;
	//p->m_nInpCh[1][1] = 8 -1;
	//p->m_nInpCh[1][2] = 3 -1;
	//p->m_nInpCh[1][3] = 7 -1;
	//p->m_nInpCh[1][4] = 2 -1;
	//p->m_nInpCh[1][5] = 6 -1;
	//p->m_nInpCh[1][6] = 1 -1;
	//p->m_nInpCh[1][7] = 5 -1;
#endif

	// RS-232C 初期接続
	if( ! p->Open(true) ){
		p->Close();
	}

	// メッセージキュー読み込み
	pLOG(em_MSG), "[%s] スレッド開始 [0x%X]", p->my_sThreadName, p->GetTid());
	while (1) {
		int wait_num = (p->m_bNowSend) ? EV_END-1 : EV_END;
		DWORD ret = WaitForMultipleObjects(wait_num, hArray, FALSE, INFINITE);

		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// スレッド終了
			pLOG(em_MSG), "[%s] スレッド終了 [0x%X]", p->my_sThreadName, p->GetTid());
			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_RETRY:												// 再接続開始
			if( ! p->Open(false) ){
				p->Close();
			}
			break;

//-----------------------------------------------------------------------------------------------	
		case EV_SENDEND:											// 送信完了
			// コマンドを送信したので、受信開始
			if( ! p->Recv() ){
				p->Close();
				break;
			}
			break;

//-----------------------------------------------------------------------------------------------	
		case EV_RECVEND:											// 受信完了
			//// 完了処理
			//p->m_bNowSend = false;
			CancelWaitableTimer(p->m_evAnserTimeOut);

			//// 受信データチェック
			if( ! p->CheckDataRecv() ){
				p->Close();
			}
			break;

//-----------------------------------------------------------------------------------------------	
		case EV_ANSERTIMEOUT:										// 送信アンサータイムアウト
			if(p->m_bIsConnect) {
				pLOG(em_ERR), "[%s] 応答伝文タイムアウト send=%s", p->my_sThreadName, p->m_cSendBuf);
				syslog(201, "[応答伝文タイムアウト:Unit=%d:send=%s]", p->m_nUnit, p->m_cSendBuf);
			}
			p->Close();					// 再接続
			break;

//-----------------------------------------------------------------------------------------------
		case EV_QUE:												// メールスロット受信
			// キー取り出し
			COMMON_QUE *pQue;
			pQue = p->gque_Deli.GetFromHead();

			// 統括からの処理依頼を実行する
			if( ! p->SetDataMailCommand(pQue) ) {
				p->Close();
			}
			SAFE_DELETE(pQue);
			break;

//-----------------------------------------------------------------------------------------------
		default :
			// ありえない！！
			break;
		}
	}
	return 0;
}


//------------------------------------------
// 統括管理からのメールスロットのコマンド解釈
// COMMON_QUE *pQue メールスロットデータ
// 戻り値 復帰情報 (false:異常によりクローズしてね)
//------------------------------------------
bool CameraManager::SetDataMailCommand(COMMON_QUE *pQue)
{
	// カメラコントローラーと通信できる場合のみ、 以降の処理実行
	if( ! m_bIsConnect) {
		LOG(em_WAR), "[%s] 回線切断状態で、コマンド受信", my_sThreadName);
		return true;
	}

	// イベントNoにより処理分け
	switch(pQue->nEventNo) {

//---------------------------------------------------------
		case FACT_TO_CAMERA_01 :							// パラメータ変更要求
			// 動的パラメータ読み込み
			GetParam();
			// 現在の検査表面状態のやつが対象だった場合、カメラコントローラーに通知する
			if( CH_SCODE_ALL   != pQue->fl.data[0] && 
				m_nNowScode[0] != pQue->fl.data[0] &&
				m_nNowScode[1] != pQue->fl.data[0] )   return true;
#ifndef AGCOFF
			// 通知する露光時間を決定
			m_typAgcInf.nSetCnt = 0;
			for(int ii=0; ii<m_nMyCamNum; ii++ ) {
				int camid = GetCamIdxToId( ii );
				m_typAgcInf.AgcInfData[ii].camid = camid;
				m_typAgcInf.AgcInfData[ii].nExpTime = m_Param.nInitialExpTime[ m_nNowScode[NUM_TorB(camid)]-1 ][camid]; 

				m_typAgcInf.nSetCnt++;
			}

			if( 0 == m_typAgcInf.nSetCnt ) return true;		// 一件も設定情報が無かったら止め
			// 送信準備
			m_emMainMode = MODE_RO;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode = 0;
#else
			return true;
#endif
			break;

//---------------------------------------------------------
		case FACT_TO_CAMERA_02 :							// 検査用設定コマンド
			// PLGモード設定
			if( DIV_PLG_LINE != pQue->fl.data[0] &&
				DIV_PLG_TOP  != pQue->fl.data[0] &&
				DIV_PLG_BOT  != pQue->fl.data[0] &&
				DIV_PLG_TEST != pQue->fl.data[0] )  return false;
			// 初期化はパルス停止時のみ
			if(m_bIsPulsu) {
				LOG(em_ERR), "[%s] パルス出力中は無効です。", my_sThreadName);
				return true;
			}

			// パラメータ読込
			GetParamStatic();
			GetParam();
			// PLGモード取得
			m_emPLGmode = (EM_DIV_PLG)pQue->fl.data[0];
			// 送信準備
			m_emMainMode = MODE_START_INIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode = 0;
			break;

//---------------------------------------------------------
		case FACT_TO_CAMERA_03 :							// パルス出力設定
			if( 0 == pQue->fl.data[0] ) {							// ON
				// 送信準備
				m_emMainMode = MODE_PULSE_ON;
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nSwMode = 0;
			} else {												// OFF
				// 送信準備
				m_emMainMode = MODE_PULSE_OFF;
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nSwMode = 0;
			}
			break;

//---------------------------------------------------------
		case FACT_TO_CAMERA_04 :							// AGC設定 
			memset( &m_typAgcInf, 0x00, sizeof(m_typAgcInf));
			if( -1 == pQue->fl.data[0] ) {							// 全カメラ
				for(int ii=0; ii<m_nMyCamNum; ii++ ) {
					SetExposureTime( GetCamIdxToId(ii), pQue->fl.data[1], pQue->fl.data[2+ii] );
				}

			} else {
				SetExposureTime( pQue->fl.data[0], pQue->fl.data[1], pQue->fl.data[2] );
			}

			if( 0 == m_typAgcInf.nSetCnt ) return true;		// 一件も設定情報が無かったら止め
			// 送信準備
			m_emMainMode = MODE_RO;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode = 0;
			break;


//---------------------------------------------------------
		case FACT_TO_CAMERA_05 :							// コイル切替
#ifndef AGCOFF
			int nPreScode;									// 前回の検査表面状態No

			// 表面状態変更
			if( 0 == pQue->fl.data[1] ) {							// 表
				nPreScode      = m_nNowScode[0];
				m_nNowScode[0] = pQue->fl.data[0];
				if( 0 > m_nNowScode[0] || MAX_SCODE <= m_nNowScode[0]) m_nNowScode[0] = MAX_SCODE;
			} else {												// 裏
				nPreScode      = m_nNowScode[1];
				m_nNowScode[1] = pQue->fl.data[0];
				if( 0 > m_nNowScode[1] || MAX_SCODE <= m_nNowScode[1]) m_nNowScode[1] = MAX_SCODE;
			}

#ifdef VER_SCODE_CHG
			// 表面状態の変化判断(0:なし、1:あり)
			if(m_Param.bScodeChg && nPreScode == pQue->fl.data[0]) {
				LOG(em_INF), "[%s] 表面状態の変化無しのため、露光時間のセット無し Scode:%d", my_sThreadName, pQue->fl.data[0]);
				return true;		// 表面状態が変化無しであれば、露光時間の初期値はセットしない
			}
#endif

			// 通知する露光時間を決定
			m_typAgcInf.nSetCnt = 0;
			for(int ii=0; ii<m_nMyCamNum; ii++ ) {
				int camid = GetCamIdxToId( ii );

				if( NUM_TorB(camid) != pQue->fl.data[1] ) continue;	// 該当面じゃなかったら無視
				m_typAgcInf.AgcInfData[ m_typAgcInf.nSetCnt ].camid = camid;
				m_typAgcInf.AgcInfData[ m_typAgcInf.nSetCnt ].nExpTime = m_Param.nInitialExpTime[ m_nNowScode[NUM_TorB(camid)]-1 ][camid]; 

				m_typAgcInf.nSetCnt++;
			}

			if( 0 == m_typAgcInf.nSetCnt ) return true;		// 一件も設定情報が無かったら止め
			// 送信準備
			m_emMainMode = MODE_RO;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode = 0;
#else
			return true;
#endif
			break;
		
//---------------------------------------------------------
		case FACT_TO_CAMERA_06 :							// 現露光時間問い合わせ
			// 現露光時間を統括管理へ通知
			Send_ToDefect_AgcVal();
			return true;
			break;

//---------------------------------------------------------
		case FACT_TO_CAMERA_07:								// 校正用設定コマンド
			// 初期化はパルス停止時のみ
			if(m_bIsPulsu) {
				LOG(em_ERR), "[CameraManager] パルス出力中は無効です。");
				return true;
			}

			// パラメータ読込
			GetParamStatic();
			GetParam();
			// PLGモード取得
			m_emPLGmode = DIV_PLG_TEST;								// テストパルス固定
			// テストパルス周期取得
			m_nTestTime = pQue->fl.data[0]							// テストパルス周期 [μs単位]
							/ m_Param.nPlgDiv[m_emPLGmode-1]		// ÷ 分周率
							* m_Param.nPlgMul[m_emPLGmode-1];		// × 逓倍率
			// 露光時間取得
			m_nExpTime = pQue->fl.data[1]							// 露光時間 [μs単位]
							* 1000 / 25;							// [25ｎs単位] へ変換
			// 送信準備
			m_emMainMode = MODE_CHECK_INIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode = 0;
			break;
		
//---------------------------------------------------------
		case FACT_TO_CAMERA_09 :							// 任意
			memcpy(m_cSendManyu, pQue->fc.data, sizeof(pQue->fc.data));
			// 送信準備
			m_emMainMode = MODE_MANYU;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode = 0;
			break;

//---------------------------------------------------------
		default :
			return false;		// 有り得ない
	}

	// ここまできたら カメラコントローラーに通知
	if( ! CheckNextSend() ) return false;
	return true;
}

//------------------------------------------
// 露光時間決定
// int cam カメラ番号 (0オリジン)
// int mode モード (0:相対値 1:絶対値)
// int div 値 (相対位置の場合は、 -1:下げ 1:上げ 0:そのまま。  絶対値の場合は、指定値  0:そのまま)
// 戻り値 true:有効データをセットした場合
//------------------------------------------
bool CameraManager::SetExposureTime(int cam, int mode, int div )
{
	int iExp = 0;				// 露光時間計算用
	int scode;					// 指定カメラの表面状態 (0オリジン)

	//// 要求のあったカメラ番号をチェック
//	if( cam < 0 || NUM_CAMERA <= cam ) {
	if( ! IsCheckMyCamId(cam) ) { 
		LOG(em_ERR), "[%s] AGC要求のカメラ番号が不正 [camera = %d]", my_sThreadName, cam+1);
		syslog(202, "[Unit=%d ID=%d]", m_nUnit, cam+1);
		return false;
	}
	
	//// データセット
	scode = m_nNowScode[ NUM_TorB(cam) ] - 1;

	//// 露光時間計算
	if(0 == mode) {					// 相対値
		// 上げ 下げ
#ifndef VER_IDULE_CAMERA
		if(1 == div)	   iExp = (int)(m_nNowExpTime[cam] * (1 + m_Param.dAGCstep[ scode ]) + 0.9);	// 切捨てで1未満になる可能性がある為、絶対 変化量は1以上になるようにする
		else if(-1 == div) iExp = (int)(m_nNowExpTime[cam] * (1 - m_Param.dAGCstep[ scode ]) );
#else
		if(1 == div)	   iExp = (int)(m_nNowExpTime[cam] * (1 + m_Param.dAGCstep[ scode ][cam]) + 0.9);	// 切捨てで1未満になる可能性がある為、絶対 変化量は1以上になるようにする
		else if(-1 == div) iExp = (int)(m_nNowExpTime[cam] * (1 - m_Param.dAGCstep[ scode ][cam]) );
#endif

	} else if(1==mode) {			// 絶対値
		iExp = div;
	}

	//// 値が0の時は、無視
	if( 0 == div ) return false;

	//// 整合性チェック(計算した露光時間が範囲内？)
	if( iExp < m_Param.nExSyncMinH ) {
		if( m_bAlame[cam] ) {
			m_bAlame[cam] = false;
			LOG(em_ERR), "[%s] 露光時間 下限値到達 [ID=%d] 露[%d]", my_sThreadName, cam+1, iExp);
//			syslog(203, "[Unit=%d ID=%d 露光時間=%d]", m_nUnit, cam+1, iExp);	// 大量に表示される為、ヤメ？△
		}
#ifdef IS_TO_CAMERA_TBL
		mtbl_pCam->caminf[cam].nMode = -1; 
#endif
		return false;

	} else if( iExp > m_Param.nExSyncLT - m_Param.nExSyncMinL ) {
		if( m_bAlame[cam] ) {
			m_bAlame[cam] = false;
			LOG(em_ERR), "[%s] 露光時間 上限値到達 [ID=%d] 露[%d]", my_sThreadName, cam+1, iExp);
//			syslog(204, "[Unit=%d ID=%d 露光時間=%d]", m_nUnit, cam+1, iExp);	// 大量に表示される為、ヤメ？△
		}
#ifdef IS_TO_CAMERA_TBL
		mtbl_pCam->caminf[cam].nMode = 1; 
#endif
		return false;
	}

	// 正常範囲内よ
	m_bAlame[cam] = true;


	//// 露光時間セット
	int cnt = m_typAgcInf.nSetCnt;
	m_typAgcInf.nSetCnt += 1;
	m_typAgcInf.AgcInfData[cnt].camid = cam;
	m_typAgcInf.AgcInfData[cnt].nExpTime = iExp;
	return true;
}

//------------------------------------------
// 現露光時間を統括管理へ送信
//------------------------------------------
void CameraManager::Send_ToDefect_AgcVal()
{
	COMMON_QUE que;
	que.nEventNo = FACT_TO_DEFECT_14;
	//que.nLineNo = getlineid();
	que.nLineNo = m_nUnit+1;

	for(int ii=0; ii<NUM_CAMERA; ii++ ) {
		que.fl.data[ii] = m_nNowExpTime[ii];
	}
		
	// 送信
	send_mail(TO_DEFECT, ".", &que);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// カメラコントロール制御
//------------------------------------------
// 現モード名称取得
// 戻り値 状態
//------------------------------------------
// 現モード名取得
CString CameraManager::GetMainModeName()
{
	return GetMainModeName(m_emMainMode);
}
// 指定モード名取得
CString CameraManager::GetMainModeName(ENUM_MAINMODE emMainMode)
{
	CString wk;
	switch( emMainMode ) {
		case MODE_NONE:
			wk = "初期状態";
			break;
		case MODE_OPEN_CHECK:
			wk = "オープンチェック";
			break;
		case MODE_START_INIT:
			wk = "初期化";
			break;
		case MODE_CHECK_INIT:
			wk = "校正初期化";
			break;
		case MODE_WAIT:
			wk = "設定待ち";
			break;

		case MODE_PULSE_ON:
			wk = "パルス出力開始";
			break;
		case MODE_PULSE_OFF:
			wk = "パルス出力停止";
			break;

		case MODE_RO:
			wk = "露光時間設定中";
			break;
		case MODE_RETRY:
			wk = "オープンリトライ中";
			break;
		case MODE_MANYU:
			wk = "手動送信中";
			break;
		default :
			wk = "モード異常";
			break;
	}
	return wk;
}
//------------------------------------------
// 次送信制御
// 戻り値 false:回線切断処理を実施
//------------------------------------------
bool CameraManager::CheckNextSend() 
{
	//// 前準備
	memset(m_cSendBuf, 0x00, sizeof(m_cSendBuf));	// 送信バッファ クリア
	//// サブモード変更 (機器毎の設定をしている場合は、サブモードを維持)
	if(0 == m_nCamMode) m_nSubMode++;

	//// 処理分け
	switch( m_emMainMode ) {

//---------------------------------------------------------
	////////////////////////////////////
	// オープンチェック
	////////////////////////////////////
	case MODE_OPEN_CHECK:
		if( 1==m_nSubMode &&
			0==m_nCamMode ) {
			LOG(em_MSG), "[%s] <%d> オープンチェック 開始", my_sThreadName, m_Param.nSwitchMasterID[m_nSwMode]);
			m_bNowSend = true;												// 1シーケンス開始
		}

		sprintf(m_cSendBuf, "%s\r", m_Param.cSwitchMasterID[m_nSwMode]);
		break;


//---------------------------------------------------------
	////////////////////////////////////
	// 初期化
	////////////////////////////////////
	case MODE_START_INIT:
		if( 1==m_nSubMode &&
			0==m_nCamMode ) {
			LOG(em_MSG), "[%s] <%d> 初期化 開始", my_sThreadName, m_Param.nSwitchMasterID[m_nSwMode]);
			m_bNowSend = true;												// 1シーケンス開始
		}

		if		 (1==m_nSubMode) {
//			sprintf(m_cSendBuf, "%sE0\r", m_Param.cSwitchMasterID[m_nSwMode]);											// パルス出力停止
//			m_bIsPulsu = false;
//
//		} else if(2==m_nSubMode) {
			sprintf(m_cSendBuf, "%sS\r", m_Param.cSwitchMasterID[m_nSwMode]);											// カウンタリセット

		} else if(2==m_nSubMode) {
			sprintf(m_cSendBuf, "%sL%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nExSyncLT);						// LT最小周期

		} else if(3==m_nSubMode) {
			sprintf(m_cSendBuf, "%sM%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgMul[m_emPLGmode-1]);			// カメラPLG逓倍率

		} else if(4==m_nSubMode) {
			sprintf(m_cSendBuf, "%sD%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgDiv[m_emPLGmode-1]);			// カメラPLG分周率

		} else if(5==m_nSubMode) {
#if ONLINE_INPUT_C == 1
			sprintf(m_cSendBuf, "%sC0\r", m_Param.cSwitchMasterID[m_nSwMode]);											// オンライン１ (トラッキング)
#elif ONLINE_INPUT_C == 2
			sprintf(m_cSendBuf, "%sC1\r", m_Param.cSwitchMasterID[m_nSwMode]);											// オンライン２ (カメラ)
#else
			sprintf(m_cSendBuf, "%sC0\r", m_Param.cSwitchMasterID[m_nSwMode]);											// オンライン１ (トラッキング)
#endif

		} else if(6==m_nSubMode) {											// Iコマンドは入力ch
			sprintf(m_cSendBuf, "%sI1%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgPulse[0]);					// オンライン[トラッキング] (差動・TTL)  (4モードは、カメラコントローラーに入力されるラインPLGの信号特性)

		} else if(7==m_nSubMode) {
			sprintf(m_cSendBuf, "%sI2%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgPulse[1]);					// 上面校正 (差動・TTL)

		} else if(8==m_nSubMode) {
			sprintf(m_cSendBuf, "%sI3%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgPulse[2]);					// 下面校正 (差動・TTL)

		} else if(9==m_nSubMode) {
			sprintf(m_cSendBuf, "%sI0%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgPulse[3]);					// オンライン[カメラ] (差動・TTL)

		} else if(10==m_nSubMode) {
			sprintf(m_cSendBuf, "%sN%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgTestInterval);				// テストパルス周期

		} else if(11==m_nSubMode) {
			sprintf(m_cSendBuf, "%sU%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgIsou[m_emPLGmode-1]);		// 位相設定 (２相・単相)

		} else if(12==m_nSubMode) {
			sprintf(m_cSendBuf, "%sR%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgIsouSa[m_emPLGmode-1]);		// 位相差設定 (アップカウント・ダウンカウント)

			// 次のサブモードでカメラ毎の設定をするための準備
			m_nCamMode = 0;

		} else if(13==m_nSubMode) {											// 露光時間変更
#ifndef AGCOFF
#ifndef VER_AVALDATA
			sprintf(m_cSendBuf, "%sA%c%d\r",
				m_Param.cSwitchMasterID[m_nSwMode],
				GetChannelNo( m_nCamID[m_nCamMode] ),
				//m_Param.nInitialExpTime[ m_nNowScode[NUM_TorB(ii)]-1 ][ m_nCamMode ] );	// 今の検査表面状態を使う
				m_Param.nInitialExpTime[ MAX_SCODE-1 ][ m_nCamMode ] );	// デフォルト検査表面状態を使う

			// 現在の露光時間を変更
			m_nNowExpTime[m_nCamMode] = m_Param.nInitialExpTime[ MAX_SCODE-1 ][ m_nCamMode ];
#ifdef IS_TO_CAMERA_TBL
			mtbl_pCam->caminf[m_nCamMode].nExpTime = m_nNowExpTime[m_nCamMode];
			mtbl_pCam->caminf[m_nCamMode].nMode = 0; 
#endif

			// カメラ毎のモード設定
			if( m_nCamMode == NUM_CAMERA-1 ) m_nCamMode = 0;				// 最後のカメラ時
			else							 m_nCamMode++;
#else
			sprintf(m_cSendBuf, "%sA%c%d\r",
				m_Param.cSwitchMasterID[m_nSwMode],
				GetChannelNo( m_nInpCh[m_nSwMode][m_nCamMode] ),
				//m_Param.nInitialExpTime[ m_nNowScode[NUM_TorB(ii)]-1 ][ m_nCamID[m_nSwMode][m_nCamMode] ] );			// 今の検査表面状態を使う
				m_Param.nInitialExpTime[ MAX_SCODE-1 ][ m_nCamID[m_nSwMode][m_nCamMode] ] );							// デフォルト検査表面状態を使う

			// 現在の露光時間を変更
			m_nNowExpTime[ m_nCamID[m_nSwMode][m_nCamMode] ] = m_Param.nInitialExpTime[ MAX_SCODE-1 ][ m_nCamID[m_nSwMode][m_nCamMode] ];
#ifdef IS_TO_CAMERA_TBL
			mtbl_pCam->caminf[ m_nCamID[m_nSwMode][m_nCamMode] ].nExpTime = m_nNowExpTime[ m_nCamID[m_nSwMode][m_nCamMode] ];
			mtbl_pCam->caminf[ m_nCamID[m_nSwMode][m_nCamMode] ].nMode = 0; 
#endif

			// カメラ毎のモード設定
			if( m_nCamMode == m_nCamNum[m_nSwMode]-1 )	m_nCamMode = 0;		// 最後のカメラ時
			else										m_nCamMode++;
#endif
#else
			// 露光時間の初期値を変更 ----->>>
#ifndef VER_AVALDATA
			sprintf(m_cSendBuf, "%sA%c%d\r",
				m_Param.cSwitchMasterID[m_nSwMode],
				GetChannelNo(m_nCamID[m_nCamMode]),
				m_Param.nExSyncMinH);							// 露光時間の最小値を設定
			// カメラ毎のモード設定
			if (m_nCamMode == NUM_CAMERA - 1) m_nCamMode = 0;				// 最後のカメラ時
			else							 m_nCamMode++;
#else
			sprintf(m_cSendBuf, "%sA%c%d\r",
				m_Param.cSwitchMasterID[m_nSwMode],
				GetChannelNo(m_nInpCh[m_nSwMode][m_nCamMode]),
				m_Param.nExSyncMinH);							// 露光時間の最小値を設定
			// カメラ毎のモード設定
			if (m_nCamMode == m_nCamNum[m_nSwMode] - 1)	m_nCamMode = 0;		// 最後のカメラ時
			else										m_nCamMode++;
#endif
			//sprintf(m_cSendBuf, "%s\r", m_Param.cSwitchMasterID[m_nSwMode]);											// サブモードを変えない為、何も無しコマンドを送信しておく
			// 露光時間の初期値を変更 -----<<<
#endif

		} else if(14==m_nSubMode) {											// PLG区分変更 (Tコマンドは出力ch)
			if(0==m_nSwMode) {												// デイジーチェーン接続の1台目
				if( DIV_PLG_LINE == m_emPLGmode ) {							// ライン
					sprintf(m_cSendBuf, "%sT0\r", m_Param.cSwitchMasterID[m_nSwMode]);
				} else if( DIV_PLG_TOP == m_emPLGmode ) {					// 表校正
					sprintf(m_cSendBuf, "%sT1\r", m_Param.cSwitchMasterID[m_nSwMode]);
				} else if( DIV_PLG_BOT == m_emPLGmode ) {					// 裏校正
					sprintf(m_cSendBuf, "%sT2\r", m_Param.cSwitchMasterID[m_nSwMode]);
				} else {													// テスト
					sprintf(m_cSendBuf, "%sT3\r", m_Param.cSwitchMasterID[m_nSwMode]);
				}
			} else {														// デイジーチェーン接続の2台目以降 (※ 1台目のPLG入力を使用)
				sprintf(m_cSendBuf, "%sT4\r", m_Param.cSwitchMasterID[m_nSwMode]);
			}

		} else if(15==m_nSubMode) {
			if(0 >= m_nFilter || 99999 < m_nFilter) {
				sprintf(m_cSendBuf, "%s\r", m_Param.cSwitchMasterID[m_nSwMode]);
			} else {
				sprintf(m_cSendBuf, "%sF%05d\r", m_Param.cSwitchMasterID[m_nSwMode], m_nFilter);						// フィルター[0.1μs] (カウンターボードと同じフィルタ時間 0.8μs をセット)
			}

		} else if(16==m_nSubMode) {
			if(0 >= m_nH) {
				sprintf(m_cSendBuf, "%s\r", m_Param.cSwitchMasterID[m_nSwMode]);
			} else {
				sprintf(m_cSendBuf, "%sH%01d\r", m_Param.cSwitchMasterID[m_nSwMode], m_nH);
			}


		} else if(17==m_nSubMode) {					// これからは、ｶﾒﾗﾘﾝｸｽｲｯﾁに関係無くの設定。
			if(0 >= m_nCmdIniInputNum) {
				sprintf(m_cSendBuf, "%s\r", m_Param.cSwitchMasterID[m_nSwMode]);
			} else {
				sprintf(m_cSendBuf, "%s\r", m_lstCmdIniInput[m_nCamMode], m_nH);
			}
			// カメラモードを使って、任意コマンドの送信個数分を送る
			if( m_nCamMode >= m_nCmdIniInputNum-1 ) m_nCamMode = 0;		// 最後のカメラ時
			else								    m_nCamMode++;

		} else if(18==m_nSubMode) {

			// 初期化中 → 設定待ち	
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode = 0;
			m_bNowSend = false;												// 1シーケンス正常完了

		} else {
			LOG(em_ERR), "[%s] <%d> サブモード異常 [mode = 初期化, sub_mode = %d]", my_sThreadName, m_Param.nSwitchMasterID[m_nSwMode], m_nSubMode);
			syslog(200, "Unit=%d [<%d> mode = 初期化 sub_mode = %d]", m_nUnit, m_Param.nSwitchMasterID[m_nSwMode], m_nSubMode);
			m_bNowSend = false;												// 1シーケンスは強制終了
			return false;
		}
		break;
		
//---------------------------------------------------------
	////////////////////////////////////
	// 校正初期化
	////////////////////////////////////
	case MODE_CHECK_INIT:
		if( 1==m_nSubMode &&
			0==m_nCamMode ) {
			LOG(em_MSG), "[CameraManager] <%d> 校正初期化 開始", m_Param.nSwitchMasterID[m_nSwMode]);
			m_bNowSend = true;												// 1シーケンス開始
		}

		if		 (1==m_nSubMode) {
			sprintf(m_cSendBuf, "%sS\r", m_Param.cSwitchMasterID[m_nSwMode]);											// カウンタリセット

		} else if(2==m_nSubMode) {
			sprintf(m_cSendBuf, "%sL%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nExSyncLT);						// LT最小周期
		
		} else if(3==m_nSubMode) {
			sprintf(m_cSendBuf, "%sM%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgMul[m_emPLGmode-1]);			// カメラPLG逓倍率
		
		} else if(4==m_nSubMode) {
			sprintf(m_cSendBuf, "%sD%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgDiv[m_emPLGmode-1]);			// カメラPLG分周率
		
		} else if(5==m_nSubMode) {
#if ONLINE_INPUT_C == 1
			sprintf(m_cSendBuf, "%sC0\r", m_Param.cSwitchMasterID[m_nSwMode]);											// オンライン１ (トラッキング)
#elif ONLINE_INPUT_C == 2
			sprintf(m_cSendBuf, "%sC1\r", m_Param.cSwitchMasterID[m_nSwMode]);											// オンライン２ (カメラ)
#else
			sprintf(m_cSendBuf, "%sC0\r", m_Param.cSwitchMasterID[m_nSwMode]);											// オンライン１ (トラッキング)
#endif

		} else if(6==m_nSubMode) {											// Iコマンドは入力ch
			sprintf(m_cSendBuf, "%sI1%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgPulse[0]);					// オンライン[トラッキング] (差動・TTL)  (4モードは、カメラコントローラーに入力されるラインPLGの信号特性)

		} else if(7==m_nSubMode) {
			sprintf(m_cSendBuf, "%sI2%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgPulse[1]);					// 上面校正 (差動・TTL)

		} else if(8==m_nSubMode) {
			sprintf(m_cSendBuf, "%sI3%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgPulse[2]);					// 下面校正 (差動・TTL)

		} else if(9==m_nSubMode) {
			sprintf(m_cSendBuf, "%sI0%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgPulse[3]);					// オンライン[カメラ] (差動・TTL)

		} else if(10==m_nSubMode) {											// テストパルス周期
			sprintf(m_cSendBuf, "%sN%d\r",
				m_Param.cSwitchMasterID[m_nSwMode],
				m_nTestTime );

			// 現在のテストパルス周期を変更 (※ パラメータを変更)
			m_Param.nPlgTestInterval = m_nTestTime;

		} else if(11==m_nSubMode) {
			sprintf(m_cSendBuf, "%sU%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgIsou[m_emPLGmode-1]);		// 位相設定 (２相・単相)

		} else if(12==m_nSubMode) {
			sprintf(m_cSendBuf, "%sR%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgIsouSa[m_emPLGmode-1]);		// 位相差設定 (アップカウント・ダウンカウント)

			// 次のサブモードでカメラ毎の設定をするための準備
			m_nCamMode = 0;

		} else if(13==m_nSubMode) {											// 露光時間変更
			sprintf(m_cSendBuf, "%sA%c%d\r",
				m_Param.cSwitchMasterID[m_nSwMode],
				GetChannelNo( m_nInpCh[m_nSwMode][m_nCamMode] ),
				m_nExpTime );

			// 現在の露光時間を変更
			m_nNowExpTime[ m_nCamID[m_nSwMode][m_nCamMode] ] = m_nExpTime;
#ifdef IS_TO_CAMERA_TBL
			mtbl_pCam->caminf[m_nCamMode].nExpTime = m_nNowExpTime[m_nCamMode];
			mtbl_pCam->caminf[m_nCamMode].nMode = 0; 
#endif

			// カメラ毎のモード設定
			if( m_nCamMode == m_nCamNum[m_nSwMode]-1 )	m_nCamMode = 0;		// 最後のカメラ時
			else										m_nCamMode++;

		} else if(14==m_nSubMode) {											// PLG区分変更 (Tコマンドは出力ch)
			if( 0 == m_nSwMode ) {											// デイジーチェーン接続の1台目
				if( DIV_PLG_LINE == m_emPLGmode ) {							// ライン
					sprintf(m_cSendBuf, "%sT0\r", m_Param.cSwitchMasterID[m_nSwMode]);
				} else if( DIV_PLG_TOP == m_emPLGmode ) {					// 表校正
					sprintf(m_cSendBuf, "%sT1\r", m_Param.cSwitchMasterID[m_nSwMode]);
				} else if( DIV_PLG_BOT == m_emPLGmode ) {					// 裏校正
					sprintf(m_cSendBuf, "%sT2\r", m_Param.cSwitchMasterID[m_nSwMode]);
				} else {													// テスト
					sprintf(m_cSendBuf, "%sT3\r", m_Param.cSwitchMasterID[m_nSwMode]);
				}
			} else {														// デイジーチェーン接続の2台目以降 (※ 1台目のPLG入力を使用)
				sprintf(m_cSendBuf, "%sT4\r", m_Param.cSwitchMasterID[m_nSwMode]);
			}

		} else if(15==m_nSubMode) {
			if(0 >= m_nFilter || 99999 < m_nFilter) {
				sprintf(m_cSendBuf, "%s\r", m_Param.cSwitchMasterID[m_nSwMode]);
			} else {
				sprintf(m_cSendBuf, "%sF%05d\r", m_Param.cSwitchMasterID[m_nSwMode], m_nFilter);						// フィルター[0.1μs] (カウンターボードと同じフィルタ時間 0.8μs をセット)
			}

		} else if(16==m_nSubMode) {
			if(0 >= m_nH) {
				sprintf(m_cSendBuf, "%s\r", m_Param.cSwitchMasterID[m_nSwMode]);
			} else {
				sprintf(m_cSendBuf, "%sH%01d\r", m_Param.cSwitchMasterID[m_nSwMode], m_nH);
			}


		} else if(17==m_nSubMode) {					// これからは、ｶﾒﾗﾘﾝｸｽｲｯﾁに関係無くの設定。
			if(0 >= m_nCmdIniInputNum) {
				sprintf(m_cSendBuf, "%s\r", m_Param.cSwitchMasterID[m_nSwMode]);
			} else {
				sprintf(m_cSendBuf, "%s\r", m_lstCmdIniInput[m_nCamMode], m_nH);
			}
			// カメラモードを使って、任意コマンドの送信個数分を送る
			if( m_nCamMode >= m_nCmdIniInputNum-1 ) m_nCamMode = 0;		// 最後のカメラ時
			else								    m_nCamMode++;

			// カメラリンクスイッチ毎のモード設定
			if( m_nSwMode != NUM_CAMLINKSW-1 ) {							// 最後のカメラリンクスイッチ以外時
				m_nSubMode = 0;
				m_nCamMode = 0;
			}
			m_nSwMode++;

		} else if(18==m_nSubMode) {

			// 初期化中 → 設定待ち	
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode = 0;
			m_bNowSend = false;												// 1シーケンス正常完了

		} else {
			LOG(em_ERR), "[CameraManager] <%d> サブモード異常 [mode = 校正初期化, sub_mode = %d]", m_Param.nSwitchMasterID[m_nSwMode], m_nSubMode);
			syslog(200, "[<%d> mode = 校正初期化 sub_mode = %d]", m_Param.nSwitchMasterID[m_nSwMode], m_nSubMode);
			m_bNowSend = false;												// 1シーケンスは強制終了
			return false;
		}
		break;

//---------------------------------------------------------
	////////////////////////////////////
	// パルス出力開始
	////////////////////////////////////
	case MODE_PULSE_ON:
		if( 1==m_nSubMode &&
			0==m_nCamMode ) {
			LOG(em_MSG), "[%s] パルス出力 開始", my_sThreadName);
			m_bNowSend = true;												// 1シーケンス開始
		}

		if		 (1==m_nSubMode) {
			if(0==m_nSwMode) {												// デイジーチェーン接続の1台目
				sprintf(m_cSendBuf, "%sS\r", m_Param.cSwitchMasterID[m_nSwMode]);										// カウンタリセット
			} else {														// デイジーチェーン接続の2台目以降
				sprintf(m_cSendBuf, "%s\r", m_Param.cSwitchMasterID[m_nSwMode]);
			}

		} else if(2==m_nSubMode) {
			if(0==m_nSwMode) {												// デイジーチェーン接続の1台目
				sprintf(m_cSendBuf, "%sE1\r", m_Param.cSwitchMasterID[m_nSwMode]);										// パルス出力開始
			} else {														// デイジーチェーン接続の2台目以降
				sprintf(m_cSendBuf, "%s\r", m_Param.cSwitchMasterID[m_nSwMode]);
			}

		} else if(3==m_nSubMode) {

			// パルス出力開始中 → 設定待ち	
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode = 0;
			m_bIsPulsu = true;
			m_bNowSend = false;												// 1シーケンス正常完了

		} else {
			LOG(em_ERR), "[%s] <%d> サブモード異常 [mode = パルス出力開始, sub_mode = %d]", my_sThreadName, m_Param.nSwitchMasterID[m_nSwMode], m_nSubMode);
			syslog(200, "Unit=%d [<%d> mode = パルス出力開始 sub_mode = %d]", m_nUnit, m_Param.nSwitchMasterID[m_nSwMode], m_nSubMode);
			m_bNowSend = false;												// 1シーケンスは強制終了
			return false;
		}
		break;

//---------------------------------------------------------
	////////////////////////////////////
	// パルス出力停止
	////////////////////////////////////
	case MODE_PULSE_OFF:
		if( 1==m_nSubMode &&
			0==m_nCamMode ) {
			LOG(em_MSG), "[%s] パルス停止 開始", my_sThreadName);
			m_bNowSend = true;												// 1シーケンス開始
		}

		if		 (1==m_nSubMode) {
			if(0==m_nSwMode) {												// デイジーチェーン接続の1台目
				sprintf(m_cSendBuf, "%sE0\r", m_Param.cSwitchMasterID[m_nSwMode]);										// パルス出力停止
			} else {														// デイジーチェーン接続の2台目以降
				sprintf(m_cSendBuf, "%s\r", m_Param.cSwitchMasterID[m_nSwMode]);
			}

		} else if(2==m_nSubMode) {

			// パルス出力停止中 → 設定待ち	
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode = 0;
			m_bIsPulsu = false;
			m_bNowSend = false;												// 1シーケンス正常完了

		} else {
			LOG(em_ERR), "[%s] <%d> サブモード異常 [mode = パルス出力停止, sub_mode = %d]", my_sThreadName, m_Param.nSwitchMasterID[m_nSwMode], m_nSubMode);
			syslog(200, "Unit=%d [<%d> mode = パルス出力停止 sub_mode = %d]", m_nUnit, m_Param.nSwitchMasterID[m_nSwMode], m_nSubMode);
			m_bNowSend = false;												// 1シーケンスは強制終了
			return false;
		}
		break;

//---------------------------------------------------------
	////////////////////////////////////
	// 露光時間設定中
	////////////////////////////////////
	case MODE_RO:
		if( 1==m_nSubMode &&
			0==m_nCamMode ) {
			LOG(em_MSG), "[%s] <%d> 露光時間変更 開始", my_sThreadName, m_Param.nSwitchMasterID[m_nSwMode]); 
			m_bNowSend = true;												// 1シーケンス開始
		}

		if		 (1==m_nSubMode) {											// 露光時間変更
			int id = m_typAgcInf.AgcInfData[ m_nCamMode ].camid;			// カメラID
			int val= m_typAgcInf.AgcInfData[ m_nCamMode ].nExpTime;			// 露光時間

#ifndef VER_AVALDATA
			sprintf(m_cSendBuf, "%sA%c%d\r", m_Param.cSwitchMasterID[m_nSwMode], GetChannelNo( m_nCamID[ id ] ), val );

			// 現在の露光時間を変更
			m_nNowExpTime[id] = val;
#ifdef IS_TO_CAMERA_TBL
			mtbl_pCam->caminf[id].nExpTime = val;
			mtbl_pCam->caminf[id].nMode = 0; 
#endif
#else
			//// 当該カメラIDがカメラリンクスイッチに接続されているかチェック
			for(int ii=0; ii<m_nCamNum[m_nSwMode]; ii++) {
				if( m_nCamID[m_nSwMode][ii] == id ) {
					sprintf(m_cSendBuf, "%sA%c%d\r",
						m_Param.cSwitchMasterID[m_nSwMode],
						GetChannelNo( m_nInpCh[m_nSwMode][ ii ] ),
						val);

					// 現在の露光時間を変更
					m_nNowExpTime[ ii ] = val;
				}
			}
			if( 0 == strlen(m_cSendBuf) ) {
				sprintf(m_cSendBuf, "%s\r",
					m_Param.cSwitchMasterID[m_nSwMode]);
			}
#ifdef IS_TO_CAMERA_TBL
			mtbl_pCam->caminf[id].nExpTime = val;
			mtbl_pCam->caminf[id].nMode = 0; 
#endif
#endif

			// カメラ毎のモード設定
			if( m_nCamMode == m_typAgcInf.nSetCnt-1 ) m_nCamMode = 0;		// 最後のカメラ時
			else									  m_nCamMode++;

		} else if(2==m_nSubMode) {

			// 露光時間設定中 → 設定待ち	
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode = 0;
			m_bNowSend = false;												// 1シーケンス正常完了

		} else {
			LOG(em_ERR), "[%s] <%d> サブモード異常 [mode = 露光時間設定中, sub_mode = %d]", my_sThreadName, m_Param.nSwitchMasterID[m_nSwMode], m_nSubMode);
			syslog(200, "Unit=%d [<%d> mode = 露光時間設定中 sub_mode = %d]", m_nUnit, m_Param.nSwitchMasterID[m_nSwMode], m_nSubMode);
			m_bNowSend = false;												// 1シーケンスは強制終了
			return false;
		}
		break;

//---------------------------------------------------------
	////////////////////////////////////
	// 任意コマンド
	case MODE_MANYU:
		if		 (1==m_nSubMode) {
			m_bNowSend = true;
			// ホントに全部自分で手打ち
			//sprintf(m_cSendBuf, "%s%s\r", m_Param.cSwitchMasterID[m_nSwMode], m_cSendManyu);
			sprintf(m_cSendBuf, "%s\r", m_cSendManyu);

		} else {
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode = 0;
			m_bNowSend = false;
		}
		break;

//---------------------------------------------------------
	default:
		LOG(em_ERR), "[%s] <%d> モード異常 [MainMode = %d]", my_sThreadName, m_Param.nSwitchMasterID[m_nSwMode], m_emMainMode);
		syslog(200, "Unit=%d [<%d> mode = %d]", m_nUnit, m_Param.nSwitchMasterID[m_nSwMode], m_emMainMode);
		m_bNowSend = false;													// 1シーケンスは強制終了
		return false;
	}

	//// 送信
	if( 0==strlen(m_cSendBuf) ) return true;								// 送信データ無しの場合は、正常を返す。
	if( !Send() ) return false;
	return true;
}

//------------------------------------------
// 受信伝文チェック
// 戻り値 false:回線切断処理を実施
//------------------------------------------
bool CameraManager::CheckDataRecv() 
{
	//LOG(em_INF), "[%s] 応答伝文受信 [%s]", my_sThreadName, m_cRecvBuf);

	////////////////////////////////////
	// コマンド解釈失敗
	////////////////////////////////////
	if ( 0 == memcmp(m_cRecvBuf, R_NG, strlen(m_cRecvBuf)) ) {
		LOG(em_ERR), "[%s] 受信伝文 異常 [解釈失敗:send=%s]", my_sThreadName, m_cSendBuf);
		syslog(201, "Unit=%d [解釈失敗:send=%s]", m_nUnit, m_cSendBuf);
		m_bNowSend = false;													// 1シーケンスは強制終了
		return false;
	

	////////////////////////////////////
	// コマンド無し
	////////////////////////////////////
	} else if ( 0 == memcmp(m_cRecvBuf, R_NOTHING, strlen(m_cRecvBuf)) ) {
		// オープンチェック時のみ 正常認識とする
		if( CameraManager::MODE_OPEN_CHECK == m_emMainMode ) {
#ifdef VER_AVALDATA
			//// カメラリンクスイッチ毎のモード設定
			// オープンチェック時
			if( 1 == m_nSubMode &&
				m_nSwMode != NUM_CAMLINKSW-1 )	{							// 最後のカメラリンクスイッチ以外時
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nSwMode++;

				// 次の送信データ決定 & 送信
				if( ! CheckNextSend() ) {
					return false;
				}
				return true;
			}
#endif

			// 接続完了
			LOG(em_MSG), "[%s] カメラコントロール回路 接続確認完了", my_sThreadName);
			syslog(205, "Unit=%d", m_nUnit);
			m_bIsConnect = true;

			CString sWk;
			sWk.Format("CAMCONTROL_ID_%d", m_nUnit + 1);
			int id = GetPrivateProfileInt(INI_STATUS, sWk, -1, TASKINI_NAME);
			if( -1 == id ) {
				KizuFunction::SetStatus("CAMCONTROL_ID", true);
			}
			else {
				KizuFunction::SetStatus(sWk, true);
			}


			// 初期化処理 開始
			m_emMainMode = CameraManager::MODE_START_INIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode = 0;
			if( ! CheckNextSend() ) {
				return false;
			}
		
		// そもそも送信が何も無かったら、確認の為だけなので無条件にＯＫとする
		} else if(strcmp("\r", m_cSendBuf)) {
#ifdef VER_AVALDATA
			//// カメラリンクスイッチ毎のモード設定
			// 初期化時
			if( CameraManager::MODE_START_INIT == m_emMainMode &&
				16 == m_nSubMode &&
				m_nSwMode != NUM_CAMLINKSW-1 ) {							// 最後のカメラリンクスイッチ以外時
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nSwMode++;
			}
			// 露光時間設定中時
			if( CameraManager::MODE_RO == m_emMainMode &&
				1 == m_nSubMode &&
				0 == m_nCamMode &&											// 最後のカメラ時
				m_nSwMode != NUM_CAMLINKSW-1 ) {							// 最後のカメラリンクスイッチ以外時
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nSwMode++;
			}
			// 校正初期化時
			if( CameraManager::MODE_CHECK_INIT == m_emMainMode &&
				16 == m_nSubMode &&
				m_nSwMode != NUM_CAMLINKSW-1 ) {							// 最後のカメラリンクスイッチ以外時
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nSwMode++;
			}
#endif

			// 次の送信データ決定 & 送信
			if( ! CheckNextSend() ) {
				return false;
			}
		

		// 通常は、異常認識
		} else {
			LOG(em_ERR), "[%s] 受信伝文 異常 [コマンド無し:send=%s]", my_sThreadName, m_cSendBuf);
			syslog(201, "Unit=%d [コマンド無し:send=%s]", m_nUnit, m_cSendBuf);
			m_bNowSend = false;												// 1シーケンスは強制終了
			return false;
		}


	////////////////////////////////////
	// コマンド正常
	////////////////////////////////////
	} else if ( 0 == memcmp(m_cRecvBuf, R_OK, strlen(m_cRecvBuf)) ) {
#ifdef VER_AVALDATA
		//// カメラリンクスイッチ毎のモード設定
		// 初期化時
		if( CameraManager::MODE_START_INIT == m_emMainMode &&
			16 == m_nSubMode &&
			m_nSwMode != NUM_CAMLINKSW-1 ) {								// 最後のカメラリンクスイッチ以外時
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode++;
		}
		// 露光時間設定中時
		if( CameraManager::MODE_RO == m_emMainMode &&
			1 == m_nSubMode &&
			0 == m_nCamMode &&												// 最後のカメラ時
			m_nSwMode != NUM_CAMLINKSW-1 ) {								// 最後のカメラリンクスイッチ以外時
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode++;
		}
		// 校正初期化時
		if( CameraManager::MODE_CHECK_INIT == m_emMainMode &&
			16 == m_nSubMode &&
			m_nSwMode != NUM_CAMLINKSW-1 ) {								// 最後のカメラリンクスイッチ以外時
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode++;
		}
#endif
		
		// 次の送信データ決定 & 送信
		if( ! CheckNextSend() ) {
			return false;
		}


	////////////////////////////////////
	// コマンド不明
	////////////////////////////////////
	} else {																// 不明
		LOG(em_ERR), "[%s] 受信伝文 異常 [不明:recv=%s]", my_sThreadName, m_cRecvBuf);
		syslog(201, "Unit=%d [不明:recv=%s]", m_nUnit, m_cRecvBuf);
		m_bNowSend = false;													// 1シーケンスは強制終了	
		return false; 
	}
	
	return true;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 232C通信関連

//------------------------------------------
// 232C オープン
// bool bLogDsp false:ログ出力無し
//------------------------------------------
bool CameraManager::Open(bool bLogDsp) 
{

	// Rs232cBaseインスタンス生成
	try{
		// カメラコントローラとPC間は、制御線が繋がっていない為、ソフトフロー制御を無効とする
		mcls_p232c = new Rs232cBase(m_cMySession, TASKINI_NAME, GetTid(), PARAM_232C_RECVBUF_SIZE, PARAM_232C_SENDBUF_SIZE, WM_USER, true);
		mcls_p232c->SetRecvEndCR(CR);  // 終端コードチェックに可変長に読込みとする

	} catch(int code) {
		mcls_p232c = NULL;
		if(bLogDsp) {
			LOG(em_ERR), "[%s] 232Cオープンエラー(err_code = %d)", my_sThreadName, code);
			syslog(SYSNO_232C_NEW_ERR, "Unit=%d [err_code = %d]", m_nUnit, code);
		}
		return false;
	}
	
	// 接続完了
	memset(m_cRecvBuf, 0x00, sizeof(m_cRecvBuf));
	memset(m_cSendBuf, 0x00, sizeof(m_cSendBuf));
//	syslog(SYSNO_232C_OPEN, "Unit=%d", m_nUnit);

	// カメラコントローラーと通信できるか確認
	m_emMainMode = MODE_OPEN_CHECK;
	m_nSubMode = 0;
	m_nCamMode = 0;
	m_nSwMode = 0;
	if( ! CheckNextSend() ) return false;
	return true;
}

//------------------------------------------
// 232C クローズ
//------------------------------------------
void CameraManager::Close() 
{
	// 全タイマ停止
	CancelWaitableTimer(m_evReTry);
	CancelWaitableTimer(m_evAnserTimeOut);

	// クローズ
	if(NULL != mcls_p232c) {
		SAFE_DELETE(mcls_p232c);
	}

	//// リトライのたびにログ出力されるの禁止
	// 一度繋がった以降からの出力
	if(m_bIsConnect) {
		LOG(em_MSG), "[%s] 232Cクローズ", my_sThreadName);
		syslog(SYSNO_232C_CLOSE, "Unit=%d", m_nUnit);
		CString sWk;
		sWk.Format("CAMCONTROL_ID_%d", m_nUnit + 1);
		KizuFunction::SetStatus(sWk, true);
		int id = GetPrivateProfileInt(INI_STATUS, sWk, -1, TASKINI_NAME);
		if( -1 == id ) {
			KizuFunction::SetStatus("CAMCONTROL_ID", false);
		}
		else {
			KizuFunction::SetStatus(sWk, false);
		}
	}

	// ステータス初期化
	m_bIsConnect = false;
	m_bNowSend = false;
	m_emMainMode = MODE_RETRY;
	m_nSubMode = 0;
	m_nCamMode = 0;
	m_nSwMode = 0;

	// リトライタイマ セット
	LARGE_INTEGER ts;
	ts.QuadPart = (__int64)PARAM_TIMEOUT_RETRY * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evReTry, &ts, 0, NULL, NULL, FALSE);
}

//------------------------------------------
// 232C 非同期 受信
//------------------------------------------
bool CameraManager::Recv() 
{
	if(NULL == mcls_p232c) return true;

	// 受信バッファクリア
	memset(m_cRecvBuf, 0x00, sizeof(m_cRecvBuf));

	// 終端コードを見て シグナルを返すタイプ
	int iRetc = mcls_p232c->Recv(m_cRecvBuf, 1, m_evRecvEnd );		// 可変長読み込みの為、1バイト固定
	if(0 != iRetc) {
		// 受信エラー
		LOG(em_ERR), "[%s] 232C受信エラー(err_code = %d)", my_sThreadName, iRetc);
		syslog(SYSNO_232C_RECV_ERR, "Unit=%d [err_code = %d]", m_nUnit, iRetc);
		return false;
	}
	return true;
}

//------------------------------------------
// 232C 非同期 送信
//------------------------------------------
bool CameraManager::Send() 
{
	if(NULL == mcls_p232c) return true;

	//// 送信
	LOG(em_INF), "[%s] コマンド送信 [%s]", my_sThreadName, m_cSendBuf);
	int iRetc = mcls_p232c->Send(m_cSendBuf, (DWORD)strlen(m_cSendBuf), m_evSendEnd);
	if(0 != iRetc) {
		// 受信エラー
		LOG(em_ERR), "[%s] 232C送信エラー(err_code = %d)", my_sThreadName, iRetc);
		syslog(SYSNO_232C_SEND_ERR, "Unit=%d [err_code = %d]", m_nUnit, iRetc);
		return false;
	}
	
	//// 送信中
	// 応答タイマ セット
	LARGE_INTEGER ts;
	ts.QuadPart = (__int64)PARAM_TIMEOUT_ANSER * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evAnserTimeOut, &ts, 0, NULL, NULL, FALSE);

	//// ステータス変更
	//m_bNowSend = true;
	return true;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DB関連

//------------------------------------------
// 動的パラメータ取得
//------------------------------------------
bool CameraManager::GetParam()
{
#ifndef AGCOFF
	CString sql;					// SQL文
	int scode;
	int camangle;
	int camset;
	int wk;

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "Unit %d", m_nUnit);
		return false;
	}
	

	//// SQL文作成
#ifndef VER_P4_CAMERA
#ifndef VER_IDULE_CAMERA
	////// 表面状態×カメラセット×カメラ角度別
	CString sWk;
	sWk.Format("%d", MAX_PHYSICAL_CAMANGLE);
	sql =  "SELECT";
	sql += " scode, camangle, camset, InitialExpTime";
	sql += " FROM ";
	sql += DB_PARAM_CAMSET_ANGLE_SCODE;
	sql += " WHERE camangle>=1 and camangle<=";
	sql += sWk;
#else
	sql =  "SELECT";
	sql += " scode, 1, camnum, ExpTime, AGCstep";
	sql += " FROM ";
	sql += DB_PARAM_CAM_SCODE;
#endif
#else
	////// 表面状態×カメラセット
	// P4カメラ
	sql =  "SELECT";
	sql += " scode, 1, camset, InitialExpTime";
	sql += " FROM ";
	sql += DB_PARAM_CAMSET_SCODE;
#endif

	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "Unit=%d [%s]", m_nUnit, DB_PARAM_CAMSET_ANGLE_SCODE);
		return false;
	}
	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り
			// 主キー
			clsDB.GetData(1, &scode);
			clsDB.GetData(2, &camangle);
			clsDB.GetData(3, &camset);

			if( scode > MAX_SCODE || scode <= 0 || camset > MAX_CAMSET || camset <= 0 || camangle > MAX_PHYSICAL_CAMANGLE || camangle <= 0) {
				// 主キー不正
				LOG(em_ERR), "[%s] [%s 主キー不正 scode=%d camset=%d camangle=%d]", my_sThreadName, DB_PARAM_CAMSET_ANGLE_SCODE, scode, camset, camangle);
				syslog(SYSNO_DB_DATA_ERR, "Unit=%d [%s scode=%d camset=%d camangle=%d]", m_nUnit, DB_PARAM_CAMSET_ANGLE_SCODE, scode, camset, camangle);	
				continue;
			}

			// データ部
			clsDB.GetData(4, &m_Param.nInitialExpTime[scode-1][((camset-1)*MAX_PHYSICAL_CAMANGLE) + (camangle-1)] );
#ifdef VER_IDULE_CAMERA
			clsDB.GetData(5, &m_Param.dAGCstep[scode-1][((camset-1)*MAX_PHYSICAL_CAMANGLE) + (camangle-1)]);
#endif

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "Unit=%d [%s]", m_nUnit, DB_PARAM_CAMSET_ANGLE_SCODE);
			return false;
		}
	}
	
#ifndef VER_IDULE_CAMERA
	////// 表面状態
	//// SQL文作成
	sql =  "SELECT";
	sql += " scode, AGCstep";
	sql += " FROM ";
	sql += DB_PARAM_SCODE;
	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "Unit=%d [%s]", m_nUnit, DB_PARAM_SCODE);
		return false;
	}
	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り
			// 主キー
			clsDB.GetData(1, &scode);

			if( scode > MAX_SCODE || scode <= 0 ) {
				// 主キー不正
				LOG(em_ERR), "[%s] [%s 主キー不正 scode=%d]", my_sThreadName, DB_PARAM_SCODE, scode);
				syslog(SYSNO_DB_DATA_ERR, "Unit=%d [%s scode=%d]", m_nUnit, DB_PARAM_SCODE, scode);	
				continue;
			}

			// データ部
			clsDB.GetData(2, &wk ); m_Param.dAGCstep[scode-1] = (double)(wk / 100.0);

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "Unit=%d [%s]", m_nUnit, DB_PARAM_SCODE);
			return false;
		}
	}
#endif
	
#ifdef VER_SCODE_CHG
	////// 共通パラメータ
	//// SQL文作成
	sql =  "SELECT";
	sql += " EnableInterCoilAGC";
	
	sql += " FROM ";
	//sql += DB_PARAM_COMMON;
	sql += m_cPrmCommonTblName;
	
	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "Unit=%d [%s]", m_nUnit, DB_PARAM_COMMON);
		return false;
	}
	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {						// データ有り
			clsDB.GetData(1, &m_Param.bScodeChg);				// 有効時は、DBの設定値を反映

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// データ無し
			break;	
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "Unit=%d [%s]", m_nUnit, DB_PARAM_COMMON);
			return false;
		}
	}
#else
	m_Param.bScodeChg = false;									// 無効時は、判断無し
#endif

#else
//	static const int AGCOFF_DEF_InitilaExpTime		= 1800;				// 露光時間初期値 [25ｎs]
//	static const int AGCOFF_DEF_AGCstep				= 5;				// AGCステップサイズ

	for(int ii=0; ii<MAX_SCODE; ii++) {
		m_Param.dAGCstep[ii] = (double)(AGCOFF_DEF_AGCstep / 100.0);

		for(int jj=0; jj<NUM_CAMERA; jj++) {
			m_Param.nInitialExpTime[ii][jj] = AGCOFF_DEF_InitilaExpTime;
		}
	}
	m_Param.bScodeChg = false;									// 無効時は、判断無し
#endif

	LOG(em_MSG), "[%s] 動的パラメータ取得完了", my_sThreadName);
	return true;
}

//------------------------------------------
// 静的パラメータ取得
//------------------------------------------
bool CameraManager::GetParamStatic()
{
	CString sql;					// SQL文
	int nId;

	//// パラメータ有無
	bool bAri[3];
	bAri[0] = 1 == GetPrivateProfileInt(m_cMySession, "PARAM_O", 0, TASKINI_NAME) ? true : false;	// 表校正
	bAri[1] = 1 == GetPrivateProfileInt(m_cMySession, "PARAM_U", 0, TASKINI_NAME) ? true : false;	// 裏校正
	bAri[2] = 1 == GetPrivateProfileInt(m_cMySession, "PARAM_T", 0, TASKINI_NAME) ? true : false;	// テストPLG
	
#if 1
	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "Unit=%d", m_nUnit);
		return false;
	}
	
	////// 共通パラメータ
	//// SQL文作成
	sql =  "SELECT";
#ifndef VER_IDULE_CAMERA
	sql += " カメラPLG逓倍率, カメラPLG分周率, カメラPLG信号特性, カメラPLG位相, カメラPLG位相差, ";
	sql += " DummyIntval, ExSyncIntval, ExSyncMinH, ExSyncMinL";
#else
	sql += " PLGmag, PLGdiv, カメラPLG信号特性, カメラPLG位相, カメラPLG位相差, ";
	sql += " DummyIntval, MinFrameInterval, MinExpTime, MinFrameInterval - MaxExpTime";
#endif
	
	if(bAri[0]) sql += ", カメラPLG逓倍率_O, カメラPLG分周率_O, カメラPLG信号特性_O, カメラPLG位相_O, カメラPLG位相差_O";
	if(bAri[1]) sql += ", カメラPLG逓倍率_U, カメラPLG分周率_U, カメラPLG信号特性_U, カメラPLG位相_U, カメラPLG位相差_U";
	if(bAri[2]) sql += ", カメラPLG逓倍率_T, カメラPLG分周率_T, カメラPLG信号特性_T, カメラPLG位相_T, カメラPLG位相差_T";

	sql += " FROM ";
	//sql += DB_PARAM_COMMON;
	sql += m_cPrmCommonTblName;
	
	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "Unit=%d [%s]", m_nUnit, DB_PARAM_COMMON);
		return false;
	}
	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {						// データ有り
			clsDB.GetData(1, &m_Param.nPlgMul[0]);
			clsDB.GetData(2, &m_Param.nPlgDiv[0]);
			clsDB.GetData(3, &m_Param.nPlgPulse[0]);
			clsDB.GetData(4, &m_Param.nPlgIsou[0]);
			clsDB.GetData(5, &m_Param.nPlgIsouSa[0]);
			clsDB.GetData(6, &m_Param.nPlgTestInterval);
			clsDB.GetData(7, &m_Param.nExSyncLT);
			clsDB.GetData(8, &m_Param.nExSyncMinH);
			clsDB.GetData(9, &m_Param.nExSyncMinL);
			nId = 10;

			for(int ii=0; ii<3; ii++) {
				if(bAri[ii]) {
					clsDB.GetData(nId+0, &m_Param.nPlgMul[ii+1]);
					clsDB.GetData(nId+1, &m_Param.nPlgDiv[ii+1]);
					clsDB.GetData(nId+2, &m_Param.nPlgPulse[ii+1]);
					clsDB.GetData(nId+3, &m_Param.nPlgIsou[ii+1]);
					clsDB.GetData(nId+4, &m_Param.nPlgIsouSa[ii+1]);

					nId += 5;
				} else {
					m_Param.nPlgMul[ii+1] = m_Param.nPlgMul[0];
					m_Param.nPlgDiv[ii+1] = m_Param.nPlgDiv[0];
					m_Param.nPlgPulse[ii+1] = m_Param.nPlgPulse[0];
					m_Param.nPlgIsou[ii+1] = m_Param.nPlgIsou[0];
					m_Param.nPlgIsouSa[ii+1] = m_Param.nPlgIsouSa[0];
				}
			}

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// データ無し
			break;	
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "Unit=%d [%s]", m_nUnit, DB_PARAM_COMMON);
			return false;
		}
	}
#else
	for( int ii=0; ii<4; ii++ )
	{
		m_Param.nPlgMul[ii]		= 3;		// カメラPLG逓倍率
		m_Param.nPlgDiv[ii]		= 2;		// カメラPLG分周率
		m_Param.nPlgPulse[ii]	= 0;		// カメラPLG信号特性 (0:差動 1:TTL)
		m_Param.nPlgIsou[ii]	= 0;		// カメラPLG位相 (0:2相 1:単相)
		m_Param.nPlgIsouSa[ii]	= 0;		// カメラPLG位相差 (0:アップカウント 1:ダウンカウント)
	}
	m_Param.nPlgTestInterval	= 45;		// カメラPLGテストパルス周期 (輝度測定モード時の周期) [μs]	
	m_Param.nExSyncLT			= 775;		// LT最小周期 [25ｎs]
	m_Param.nExSyncMinH			= 80;		// LT周期の最小値 [25ｎs]
	m_Param.nExSyncMinL			= 80;		// LT周期の最大値の幅 最大値=(nExSyncLT-nExSyncMinL) [25ｎs]
#endif

	// 特殊設定
	if(! bAri[2]) {						// テストパルスのパラメータ指定が無い場合のみ
		m_Param.nPlgIsouSa[3] = 0;		// テストパルスはアップカウント固定
	}

#ifdef VER_AVALDATA
#if 1
	////// カメラリンクスイッチ設定パラメータ
	//// SQL文作成
	sql =  "SELECT";
	sql += " ボードID";
	sql += " FROM ";
	sql += DB_CAMERA_LINKSWITCH;
	sql += " WHERE ";
	sql += " ステータス = 1";
	sql.AppendFormat( " AND ID = %d",  m_nUnit+1);
	sql += " ORDER BY ID ASC";		// IDの昇順
	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "Unit=%d [%s]", m_nUnit, DB_CAMERA_LINKSWITCH);
		return false;
	}
	//// 取得
	nId = 0;
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {						// データ有り
			if( NUM_CAMLINKSW <= nId ) {
				// スイッチマスタ FPGAボードID件数オーバー
				LOG(em_ERR), "[%s] スイッチマスタ FPGAボードID 件数オーバー [%d件]", my_sThreadName, nId);
				return false;
			}
			clsDB.GetData(1, &m_Param.nSwitchMasterID[nId]);
			LOG(em_INF), "[%s] スイッチマスタ FPGAボードID 取得 [ID=%d]", my_sThreadName, m_Param.nSwitchMasterID[nId]);
			sprintf(m_Param.cSwitchMasterID[nId], "@%02X", m_Param.nSwitchMasterID[nId]);
			nId++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// データ無し
			break;	
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "Unit=%d [%s]", m_nUnit, DB_CAMERA_LINKSWITCH);
			return false;
		}
	}
#else
	m_Param.nSwitchMasterID[0]	= 0;
	sprintf(m_Param.cSwitchMasterID[0], "@%02X", m_Param.nSwitchMasterID[0]);
	m_Param.nSwitchMasterID[1]	= 2;
	sprintf(m_Param.cSwitchMasterID[1], "@%02X", m_Param.nSwitchMasterID[1]);
#endif
#endif

	LOG(em_MSG), "[%s] 静的パラメータ取得完了", my_sThreadName);
	return true;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// コマンド関連


//------------------------------------------
// カメラIDからチャンネル番号取得
// int nCamID カメラID (0オリジン)
//------------------------------------------
char CameraManager::GetChannelNo(int nCamID)
{
	char cChNo;

#ifdef VER_AVALDATA
	// カメラリンクスイッチのチャンネル番号は4単位
	// 理由：カメラコントロールFPGAのExSyncが以下のように接続されているため！
	// <チャンネル番号>		<接続先>
	// 0					カメラ1のCC1
	// 1					カメラ1のCC2
	// 2					カメラ1のCC3
	// 3					カメラ1のCC4
	// 4					カメラ2のCC1
	// 5					カメラ2のCC2
	// 6					カメラ2のCC3
	// 7					カメラ2のCC4
	nCamID *= 4;
#endif

	if( 0 <= nCamID && 10 > nCamID ) {
		cChNo = '0' + nCamID;
	} else if( 10 <= nCamID && 32 > nCamID ) {
		cChNo = 'a' + nCamID - 10;
	} else {
		cChNo = NULL;
	}

	return cChNo;
}
