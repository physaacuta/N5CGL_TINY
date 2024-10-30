#include "StdAfx.h"
#include "TestManager.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//#define DB_WRITE											// 本番時コメント  (テスト時コメントアウト)

//------------------------------------------
// コンストラクタ
//------------------------------------------
TestManager::TestManager(void) :
ThreadManager("TestManager")
{

	//// ハンドル生成
	m_evTimerExec  = CreateWaitableTimer(NULL, FALSE, NULL);

	// 表示基点PLGタイマーハンドル生成
	m_evTimerDisp = CreateWaitableTimer(NULL, FALSE, NULL);

	//// テスト用コイル情報初期化
	m_pTestCoilInf = (TEST_COIL_INF*)malloc(sizeof(TEST_COIL_INF) * MAX_COIL_NUM);
	memset(m_pTestCoilInf, 0x00 ,sizeof(TEST_COIL_INF) * MAX_COIL_NUM);

	//// コイル情報初期化
	m_pCoilInf = (PRO_TYPE_DATA_COILINF*)malloc(sizeof(PRO_TYPE_DATA_COILINF) * MAX_COIL_NUM);
	memset(m_pCoilInf, 0x00 ,sizeof(PRO_TYPE_DATA_COILINF) * MAX_COIL_NUM);

	//// コイルベース初期化
	m_pCoilBase = (COIL_BASEDATA*)malloc(sizeof(COIL_BASEDATA) * MAX_COIL_NUM);
	memset(m_pCoilBase, 0x00 ,sizeof(COIL_BASEDATA) * MAX_COIL_NUM);

	//// 検査台情報初期化
	m_pDspInf = (DSP_INF*)malloc(sizeof(DSP_INF) * DIV_LEN_POS_FINAL);
	memset(m_pDspInf, 0x00 ,sizeof(DSP_INF) * DIV_LEN_POS_FINAL);

	// 検査台位置名称設定
	SetKensadaiName();
	
	// 前工程情報の作成
	m_bMaeInfo = false;			// 前工程情報 デフォルトは作成しない

	// 初期速度
	m_nSpeed = 300;

	// 疵生成 デフォルトは疵生成しない
	m_bDefCreate = false;

	// 状態初期化
	m_nUnten = DIV_UNTEN_NOMAL;	// 運転状態
	m_nKadou = DIV_KADOU_START;	// 稼動状態
	for (int nMen = 0; nMen < NUM_MEN; nMen++) {
		m_nKiki[nMen] = DIV_KIKI_OK;	// 機器状態
		m_nKensa[nMen] = DIV_KENSA_OK;	// 検査状態
	}

	// 表示PLG初期設定
	m_bDispPlg = false;			// 初期状態は停止

	// 通板位置情報送信初期化
	m_bSendPos = true;

	// コイル長範囲設定 最小～最大
	m_nCoilLenMax = (COIL_LEN_MAX *1000);	// 最長コイル長 
	m_nCoilLenMin = (COIL_LEN_MIN*1000);	// 最短コイル長 

	// コイル幅範囲設定
	m_nCoilWidthMin = 650;
	m_nCoilWidthMax = COIL_WID_MAX;

	// 疵発生率
	m_nDefRate = 5;

	// 初期化済み
	m_bInitial = true;

	mcls_pDbs = new DBSaveManager( 0, &mque_AddDB );
	mcls_pDbs->SetLogMgr(mcls_pLog);
	mcls_pDbs->SetSendDbManager(false); 
	mcls_pDbs->Start();
}
//------------------------------------------
// デストラクタ
//------------------------------------------
TestManager::~TestManager(void)
{
	mcls_pDbs->Stop(3000);
	delete mcls_pDbs;

}

////////////////////////////////////////////////////////////////////////////////
// スレッド
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int TestManager::ThreadFirst()
{
//// 事前準備
	__int64 ts = 3000 * -1000; 
	int nCycleTime = 1*500;	// 0.5秒周期
	ts = -1;
	SetWaitableTimer(m_evTimerExec, (LARGE_INTEGER *)&ts, nCycleTime, NULL, NULL, FALSE);


	SetWaitableTimer(m_evTimerDisp, (LARGE_INTEGER *)&ts, nCycleTime, NULL, NULL, FALSE);

	//// イベント設定
	HANDLE hArray[] = { gque_Deli.g_evSem,
						m_evTimerExec,
						m_evTimerDisp
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);

	//mcls_pImgSend->SetDbQue(&mque_AddDB);

	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int TestManager::ThreadLast()
{
		DELIVERY_KEY *pDeli;				// キューデータ

	// 未処理のキューを破棄
	while(true) {
		pDeli = gque_Deli.GetFromHead(0);
		if( pDeli == NULL ) break;
		
		// 削除
		LOG(em_WAR), "[%s] 未処理キュー削除 [%s]", my_sThreadName, pDeli->cKizukenNo);
		delete pDeli;
	}

	// 定周期停止
	CancelWaitableTimer(m_evTimerExec);
	CancelWaitableTimer(m_evTimerDisp);

	SAFE_DELETE(m_pTestCoilInf);
	SAFE_DELETE(m_pCoilInf);
	SAFE_DELETE(m_pCoilBase);
	SAFE_DELETE(m_pDspInf);

	SAFE_DELETE_HANDLE(m_evTimerExec);
	SAFE_DELETE_HANDLE(m_evTimerDisp);


	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void TestManager::ThreadEvent(int nEventNo)
{
	DELIVERY_KEY*	pDeli;			// 受け渡し依頼データ

	////// シグナルの条件
	enum {	EV_QUE = 0,						// キュー依頼
			EV_TIMER_EXEC,					// 処理実行 定周期タイマー
			EV_TIMER_DISP					// 表示PLGイベント
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_QUE:
		pDeli = gque_Deli.GetFromHead(); 
		Exec( pDeli->id , pDeli);
		SAFE_DELETE(pDeli);
		break;
	
	case EV_TIMER_EXEC: 				// 処理実行 定周期タイマー
		if(true) {
			//LOG(em_MSG), "[%s] 定周期タイマー", my_sThreadName);
			Exec( EM_TIMER , NULL );
		}
		break;

	case EV_TIMER_DISP:					// 表示PLGイベント
		if(true) {
			//LOG(em_MSG), "[%s] 表示PLG", my_sThreadName );
			// 1FLL では表示用PLGは存在しない
			//DispPlgEvent();
		}
		break;
//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 0.ステータス情報セット
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void TestManager::SendStatusInfo()
{
	//// 送信データ生成 (送信先で開放する事) 
	// 送信データサイズを求める
	int nSendSize = sizeof(TO_SO_HEAD) + sizeof(TO_SO_DATA_STATUS);

	// BYTE配列を作成する
	BYTE* pSend = new BYTE[nSendSize];
	memset( pSend,0,nSendSize );

	// ヘッダ部先頭アドレスをセットする
	TO_SO_HEAD& head = (TO_SO_HEAD&)pSend[0];

	// データ部先頭アドレスをセットする
	TO_SO_DATA_STATUS& data = (TO_SO_DATA_STATUS&)pSend[sizeof(TO_SO_HEAD)];

	// 初期値をセット
	data.status.kensa[0] = DIV_KENSA_OK;
	data.status.kensa[1] = DIV_KENSA_OK;
	data.status.kiki[0] = DIV_KIKI_OK;
	data.status.kiki[1] = DIV_KIKI_OK;

	// ベースヘッダ部
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len = nSendSize;
	head.base_head.addr[0] = NULL;
	head.base_head.addr[1] = NULL;

	// 拡張ヘッダ部
	memcpy( head.KizukenNo, "0020141223 091215", sizeof(head.KizukenNo)); 
	head.kind =TO_SO_DATA_KIND_STATUS;

	// データ部
	data.nUnten = m_nUnten;			// 運転モード (DIV_UNTEN_STOP･･･)
	data.nKadou = m_nKadou;			//★ 稼動状態 (DIV_KADOU_INIT･･･)


	for (int nMen=0; nMen<NUM_MEN; nMen++) {

		//★ 検査異常度 DIV_KENSA
		switch (m_nKensa[nMen]) {
			case DIV_KENSA_OK:
				data.status.kensa[nMen]=DIV_KENSA_OK;
				break;
			case DIV_KENSA_NG:
				data.status.kensa[nMen]=DIV_KENSA_NG;
				break;
			case DIV_KENSA_MENTE: 
				data.status.kensa[nMen] = DIV_KENSA_MENTE;
				break;
			case DIV_KENSA_STOP:
				data.status.kensa[nMen] = DIV_KENSA_STOP;
				break;
			case DIV_KENSA_EDGENG:
				data.status.kensa[nMen] = DIV_KENSA_EDGENG;
				break;
			case DIV_KENSA_DBNON:
				data.status.kensa[nMen] = DIV_KENSA_DBNON;
				break;
			case DIV_KENSA_SKIP:
				data.status.kensa[nMen] = DIV_KENSA_SKIP;
				break;
			case DIV_KENSA_NON:
				data.status.kensa[nMen] = DIV_KENSA_NON;
				break;
		}

		//★ 機器異常度 DIV_KIKI
		switch (m_nKiki[nMen]) {
			case DIV_KIKI_OK:	
				data.status.kiki[nMen] = DIV_KIKI_OK;
				break;
			case DIV_KIKI_KEI:
				data.status.kiki[nMen] = DIV_KIKI_KEI;
				break;
			default:
				data.status.kiki[nMen] = DIV_KIKI_JYU;
				break;
		}
	}



	if(((DIV_KIKI_OK == data.status.kiki[0])&&(DIV_KIKI_OK == data.status.kiki[1])) && 
	   ((DIV_KENSA_OK == data.status.kensa[0]) && (DIV_KENSA_OK == data.status.kensa[1]))){
		data.status.nCnt = 0;					// 異常セット件数
	} else {
		data.status.nCnt = 5;					// 異常セット件数

		// 異常の機器情報上位5件
		//★ ①
		data.status.ngInf[0].id = 1;		// 機器ID
		data.status.ngInf[0].pcid = 1;		// PCID (0:該当無し。機能に割り付き   1～：PCに割り付き)
		data.status.ngInf[0].area = 1;		// 異常範囲 (0:全体 1:表面 2:裏面)
		data.status.ngInf[0].kensa = DIV_KENSA_OK;		// 検査異常度 (0:無害 1:縮退)
		data.status.ngInf[0].kiki = DIV_KIKI_OK;		// 機器異常度 (0:無害 1:軽故障 2:中故障 3:重故障)
		data.status.ngInf[0].yusen = 1;		// 優先度 (1:最強 NULL=99999：最弱)
		memcpy(data.status.ngInf[0].msg, "---------1---------2---------3---------4---------5---------6---------7---------8---------9--------10--------11--------12--------13--------14--------15--------16--------17--------18--------19--------20--------21--------22--------23--------24--------25-----6", sizeof(data.status.ngInf[0].msg)); // 異常時のメッセージ
		data.status.ngInf[0].pcmode = 1;	// PC区分 (0:オンライン  1:オフライン)
		data.status.ngInf[0].stat = 1;		// 現状態

		// ②
		data.status.ngInf[1].id = 2;		// 機器ID
		data.status.ngInf[1].pcid = 2;		// PCID (0:該当無し。機能に割り付き   1～：PCに割り付き)
		data.status.ngInf[1].area = 2;		// 異常範囲 (0:全体 1:表面 2:裏面)
		data.status.ngInf[1].kensa = DIV_KENSA_NG;		// 検査異常度 (0:無害 1:縮退)
		data.status.ngInf[1].kiki = DIV_KIKI_KEI;		// 機器異常度 (0:無害 1:軽故障 2:中故障 3:重故障)
		data.status.ngInf[1].yusen = 2;		// 優先度 (1:最強 NULL=99999：最弱)
		memcpy(data.status.ngInf[1].msg, "*********1*********2*********3*********4*********5*********6*********7*********8*********9********10********11********12********13********14********15********16********17********18********19********20********21********22********23********24********25*****6", sizeof(data.status.ngInf[1].msg)); // 異常時のメッセージ
		data.status.ngInf[1].pcmode = 2;	// PC区分 (0:オンライン  1:オフライン)
		data.status.ngInf[1].stat = 2;		// 現状態

		// ③
		data.status.ngInf[2].id = 3;		// 機器ID
		data.status.ngInf[2].pcid = 3;		// PCID (0:該当無し。機能に割り付き   1～：PCに割り付き)
		data.status.ngInf[2].area = 3;		// 異常範囲 (0:全体 1:表面 2:裏面)
		data.status.ngInf[2].kensa = DIV_KENSA_OK;		// 検査異常度 (0:無害 1:縮退)
		data.status.ngInf[2].kiki = DIV_KIKI_JYU;		// 機器異常度 (0:無害 1:軽故障 2:中故障 3:重故障)
		data.status.ngInf[2].yusen = 3;		// 優先度 (1:最強 NULL=99999：最弱)
		memcpy(data.status.ngInf[2].msg, "#########1#########2#########3#########4#########5#########6#########7#########8#########9########10########11########12########13########14########15########16########17########18########19########20########21########22########23########24########25#####6", sizeof(data.status.ngInf[2].msg)); // 異常時のメッセージ
		data.status.ngInf[2].pcmode = 3;	// PC区分 (0:オンライン  1:オフライン)
		data.status.ngInf[2].stat = 3;		// 現状態

		// ④
		data.status.ngInf[3].id = 4;		// 機器ID
		data.status.ngInf[3].pcid = 4;		// PCID (0:該当無し。機能に割り付き   1～：PCに割り付き)
		data.status.ngInf[3].area = 4;		// 異常範囲 (0:全体 1:表面 2:裏面)
		data.status.ngInf[3].kensa = DIV_KENSA_NG;		// 検査異常度 (0:無害 1:縮退)
		data.status.ngInf[3].kiki = DIV_KIKI_OK;		// 機器異常度 (0:無害 1:軽故障 2:中故障 3:重故障)
		data.status.ngInf[3].yusen = 4;		// 優先度 (1:最強 NULL=99999：最弱)
		memcpy(data.status.ngInf[3].msg, "+++++++++1+++++++++2+++++++++3+++++++++4+++++++++5+++++++++6+++++++++7+++++++++8+++++++++9++++++++10++++++++11++++++++12++++++++13++++++++14++++++++15++++++++16++++++++17++++++++18++++++++19++++++++20++++++++21++++++++22++++++++23++++++++24++++++++25+++++6", sizeof(data.status.ngInf[3].msg)); // 異常時のメッセージ
		data.status.ngInf[3].pcmode = 4;	// PC区分 (0:オンライン  1:オフライン)
		data.status.ngInf[3].stat = 4;		// 現状態

		// ⑤
		data.status.ngInf[4].id = 5;		// 機器ID
		data.status.ngInf[4].pcid = 5;		// PCID (0:該当無し。機能に割り付き   1～：PCに割り付き)
		data.status.ngInf[4].area = 5;		// 異常範囲 (0:全体 1:表面 2:裏面)
		data.status.ngInf[4].kensa = DIV_KENSA_OK;		// 検査異常度 (0:無害 1:縮退)
		data.status.ngInf[4].kiki = DIV_KIKI_KEI;		// 機器異常度 (0:無害 1:軽故障 2:中故障 3:重故障)
		data.status.ngInf[4].yusen = 5;		// 優先度 (1:最強 NULL=99999：最弱)
		memcpy(data.status.ngInf[4].msg, "=========1=========2=========3=========4=========5=========6=========7=========8=========9========10========11========12========13========14========15========16========17========18========19========20========21========22========23========24========25=====6", sizeof(data.status.ngInf[4].msg)); // 異常時のメッセージ
		data.status.ngInf[4].pcmode = 5;	// PC区分 (0:オンライン  1:オフライン)
		data.status.ngInf[4].stat = 5;		// 現状態
	}

	// 送信
	mcls_pOpSend->gque_Deli.AddToTail((SOCK_BASE_HEAD*)pSend);
}

//// 指定された疵情報を送信
void TestManager::MakeDefectManual(char const* cKizukenNoSet, int nMen, int nY , int nDSdiv , int Tid, int Gid , float nArea, bool bNotChkDsWs)
{
	int nRec;
	int pcode;

	// 検査中のコイルレコード番号取得
	int pos;
	switch (nMen) {
	case 0:	pos = DIV_LEN_POS_KEN_TOP;	break;
	case 1:	pos = DIV_LEN_POS_KEN_BOT;	break;
	default:	return;
	}

	if (m_pDspInf[pos].cKizukenNo[0]==0x00) {
		// 検査中コイルは無し
		return;
	}
	for (nRec=0; nRec<MAX_COIL_NUM; nRec++) {
		if (0==memcmp(m_pDspInf[pos].cKizukenNo, (&m_pTestCoilInf[nRec])->cKizukenNo, sizeof(m_pDspInf[pos].cKizukenNo))) {
			// 現在検査装置通板中のコイル
			break;
		}
	}
	if (MAX_COIL_NUM==nRec) {
		// 検査中のコイルは存在しない
		return;
	}

	// 疵検管理No取得
	char cKizukenNo[SIZE_KIZUKEN_NO];
	if (strlen(cKizukenNoSet) > 0) {
		memcpy(cKizukenNo, cKizukenNoSet, SIZE_KIZUKEN_NO);
	}
	else {
		memcpy(cKizukenNo, m_pTestCoilInf[nRec].cKizukenNo, SIZE_KIZUKEN_NO);
	}

	// pcodeの取得
	pcode = m_pTestCoilInf[nRec].pcode[nMen];

	// 命令No 取得
	char cMeiNo[SIZE_MEI_NO];
	memcpy( cMeiNo , m_pTestCoilInf[nRec].cMeiNo , SIZE_MEI_NO );

	char cCoilNo[SIZE_COIL_NO];
	memcpy( cCoilNo , m_pTestCoilInf[nRec].cCoilNo , SIZE_COIL_NO);

	// コイル幅取得
	int nWidth = (&m_pCoilBase[nRec])->nWid;
	int nMapColNum = m_pTestCoilInf[nRec].nMapColNum;
	int nCol = nDSdiv;
	if (nCol >= MAP_COL_NUM) nCol = MAP_COL_NUM-1;
	else if (nCol < 0) nCol = 0;
	int nCNum;
	int nCenter;
	float dDS;
	float dWS;

	// 指定された幅位置からDS/WSからの距離を算出
	if (nCol >= (MAP_COL_NUM/2)) {
		// WS側
		nCNum = nCol - (MAP_COL_NUM / 2);
		if (!bNotChkDsWs && ((nMapColNum / 2)-1) < nCNum) {
			// 指定された幅位置がマップ列数範囲に含まれない
			return;
		}
		// センターからの距離
		nCenter = (MAP_COL_INTERVAL*nCNum) + (rand() % MAP_COL_INTERVAL);
		if ((nWidth / 2) <= nCenter) nCenter = (nWidth / 2) - 1;
		// DSからの距離決定
		dDS = (float)(nWidth / 2) + nCenter;
		// WSからの距離決定
		dWS = (float)nWidth - dDS;
	} else {
		// DS側
		nCNum = ((MAP_COL_NUM / 2)-1) - nCol;
		if (!bNotChkDsWs && ((nMapColNum / 2) - 1) < nCNum) {
			// 指定された幅位置がマップ列数範囲に含まれない
			return;
		}
		// センターからの距離
		nCenter = (MAP_COL_INTERVAL*nCNum) + (rand() % MAP_COL_INTERVAL);
		if ((nWidth / 2) <= nCenter) nCenter = (nWidth / 2) - 1;
		// WSからの距離決定
		dWS = (float)(nWidth / 2) + nCenter;
		// DSからの距離決定
		dDS = (float)nWidth - dWS;
	}

	// 長手分割位置決定
	int nRow = int(nY / 1000 / MAP_REPRES_MIN);

	// 疵情報送信要求
	ImgSendManager::DELIVERY_KEY* pQue = new ImgSendManager::DELIVERY_KEY;
	memset(pQue, 0x00, sizeof(ImgSendManager::DELIVERY_KEY));
	pQue->id = 1;					// 疵情報送信要求
	memcpy(pQue->cKizukenNo , cKizukenNo, SIZE_KIZUKEN_NO  );
	memcpy(pQue->cCoilNo , cCoilNo,SIZE_COIL_NO);
	memcpy(pQue->cMeiNo , cMeiNo , SIZE_MEI_NO );
	pQue->nMen = nMen;				// 表面
	pQue->x = nCol;					// 分割位置
	pQue->nTid = Tid;				// 疵種EdasysID
	pQue->nGid = Gid;				// グレードEdasysID
	pQue->nInY = nY;				// 疵長手位置[mm]
	pQue->dDS = dDS;				// DS側エッジからの距離
	pQue->dWS = dWS;				// WS側エッジからの距離
	pQue->nKizuSeq = m_pTestCoilInf[nRec].nKizuSeq;	// 疵連番
	pQue->nSpeed = m_nSpeed;		// 速度
	pQue->nArea = nArea;			// 面積

	bool output_dsp = false;
	int yuusenT = mcls_pParam->GetYusenT(pQue->nTid);
	int yuusenG = mcls_pParam->GetYusenG(pQue->nGid);

	// 出力対象の疵か判定(探傷画面)
	if( 0 == nMen ){
		// 探傷画面出力対象か	
		if(mcls_pParam->Check_Output(ParamManager::OUTPUTFLG_DSP_O,pcode-1,pQue->nTid,pQue->nGid) ) output_dsp = true;
	} else {
		// 探傷画面出力対象か	
		if(mcls_pParam->Check_Output(ParamManager::OUTPUTFLG_DSP_U,pcode-1,pQue->nTid,pQue->nGid) ) output_dsp = true;
	}
	pQue->nObjTansho = true;

	// 出力対象の疵か判定(重欠陥一覧画面)
	bool output_lst = false;
	
	if (1 == mcls_pParam->GetCommonMast().nDeflistFilter) {
		if (mcls_pParam->Check_PallOutput(ParamManager::PALLOUT_SIMPLE, pQue->nTid, pQue->nGid)) output_lst = true;
	} else{
		if (mcls_pParam->Check_PallOutput(ParamManager::PALLOUT_DETAIL, pQue->nTid, pQue->nGid)) output_lst = true;
	}
	pQue->nObjDeflist = output_lst;


	LOG(em_MSG),"[%s] 手動設定 送信疵送信(%d) <%s> men[%d] Seq[%d] y[%d] x[%d] DS[%f] WS[%f] TID[%d] GID[%d] yuusenG[%d] yuusenT[%d] dsp[%d] lst[%d]", my_sThreadName,0,cKizukenNo,nMen,pQue->nKizuSeq , nY,nCol,dDS,dWS , pQue->nTid , pQue->nGid , yuusenG , yuusenT , output_dsp, output_lst);
	mcls_pImgSend->gque_Deli.AddToTail(pQue);
	m_pTestCoilInf[nRec].nKizuSeq++;

	return;

}

//// 疵情報決定＆送信
void TestManager::MakeDefect(int nRec , int nMen, int from , int to )
{

	if ((from<0)||(from>=to)) {
		// fromが負数、またはfromよりもtoの方が小さいということはありえない
		LOG(em_MSG),"[%s] 疵情報送信決定＆送信 パラメータ異常: from[%d] to[%d]", my_sThreadName , from , to );
		return;
	}

	for(int row = int(from/1000/MAP_REPRES_MIN); row <= int(to/1000/MAP_REPRES_MIN); row++ ){
		// 疵個数決定
		int nDef = (rand() % (m_nDefRate+1) ) ;
		if( nDef < 0 ) nDef = 0;

		if( m_bDefCreate == false ){
			nDef = 0;
		} else {
			//LOG(em_MSG),"[%s] 疵個数仮決定 行[%d] 仮決定疵個数[%d]", my_sThreadName , row , nDef );
		}

		// 管理No取得
		char cKizukenNo[SIZE_KIZUKEN_NO];
		memcpy( cKizukenNo, m_pTestCoilInf[nRec].cKizukenNo , SIZE_KIZUKEN_NO );

		// pcodeの取得
		int pcode = m_pTestCoilInf[nRec].pcode[nMen];

		// 命令No 取得
		char cMeiNo[SIZE_MEI_NO];
		memcpy( cMeiNo , m_pTestCoilInf[nRec].cMeiNo , SIZE_MEI_NO );

		// コイルNo 取得
		char cCoilNo[SIZE_COIL_NO];
		memcpy( cCoilNo , m_pTestCoilInf[nRec].cCoilNo , SIZE_COIL_NO);

		// マップ列数取得
		int nMapColNum = m_pTestCoilInf[nRec].nMapColNum;

		// コイル幅取得
		int nWidth = (&m_pCoilBase[nRec])->nWid;

		// 実際に送信する疵個数
		int nNum = 0;

		// 長手位置は行位置の範囲、かつ、from～toの範囲であるべき
		int	nMinY = (row*MAP_REPRES_MIN*1000);
		int nMaxY = ((row+1)*MAP_REPRES_MIN*1000)-1;
		if (nMinY < from) nMinY = from;
		if (nMaxY > to) nMaxY = to;

		// 疵情報送信要求
		for(int jj = 0;jj<nDef;jj++){
			// 長手位置決定
			int nY = nMinY+(rand() % ((nMaxY-nMinY)+1));
			float dDS;
			float dWS;
			int nCol;
			int nCenter = (rand() % ((nWidth) / 2));		// センターからの距離[mm]
			if (rand() % 2) {
				// WS側
				nCol = (MAP_COL_NUM / 2) + (nCenter / MAP_COL_INTERVAL);
				dDS = (float)((nWidth / 2) + nCenter);
				dWS = (float)nWidth - dDS;
			} else {
				// DS側
				nCol = (MAP_COL_NUM / 2) - (nCenter / MAP_COL_INTERVAL) - 1;
				dWS = (float)((nWidth / 2) + nCenter);
				dDS = (float)nWidth - dWS;
			}
			if (0 > nCol)	nCol = 0;
			if (MAP_COL_NUM <= nCol)	nCol = MAP_COL_NUM-1;

			// 疵情報送信要求
			ImgSendManager::DELIVERY_KEY* pQue = new ImgSendManager::DELIVERY_KEY;
			memset(pQue, 0x00, sizeof(ImgSendManager::DELIVERY_KEY));
			pQue->id = 1;												// 疵情報送信要求
			memcpy(pQue->cKizukenNo , cKizukenNo, SIZE_KIZUKEN_NO  );	// 管理No
			memcpy(pQue->cCoilNo , cCoilNo,SIZE_COIL_NO);				// コイルNo
			memcpy(pQue->cMeiNo , cMeiNo , SIZE_MEI_NO );				// 命令No
			pQue->nMen = nMen;											// 面
			pQue->x = nCol;												// 分割位置
			pQue->nTid = mcls_pParam->GetRandTid(64);					// 疵種EdasysID（ランダム)
			pQue->nSpeed = m_nSpeed;									// 速度

			int nGid = rand() % 100;	// グレードEdasysIDは、0 ～ 99のランダムな値で決定
			//if (20 > nGid) {			// 0 ～ 19	20%
			//	pQue->nGid = 1;	// EdasysID 1
			//}
			//else if (50 > nGid) {	// 19 ～ 49	30%
			//	pQue->nGid = 2;	// EdasysID 2
			//}
			//else if (82 > nGid) {	// 50 ～ 81	32%
			//	pQue->nGid = 3;	// EdasysID 3
			//}
			//else if (92 > nGid) {	// 82 ～ 91 10%
			//	pQue->nGid = 4;	// EdasysID 4
			//}
			//else if (97 > nGid) {	// 92 ～ 96 5%
			//	pQue->nGid = 5;	// EdasysID 5
			//}
			//else {					// 97 ～ 99 3%
			//	pQue->nGid = 6;	// EdasysID 6
			//}
			if( 20 > nGid ){			// 0 ～ 19	20%
				pQue->nGid = 1;	// EdasysID 1
			} else if ( 50 > nGid ) {	// 19 ～ 49	30%
				pQue->nGid = 12;	// EdasysID 2
			} else if ( 82 > nGid ) {	// 50 ～ 81	32%
				pQue->nGid = 13;	// EdasysID 3
			} else if ( 92 > nGid ) {	// 82 ～ 91 10%
				pQue->nGid = 14;	// EdasysID 4
			} else if ( 97 > nGid ) {	// 92 ～ 96 5%
				pQue->nGid = 15;	// EdasysID 5
			} else {					// 97 ～ 99 3%
				pQue->nGid = 16;	// EdasysID 6
			}

			pQue->nInY = nY;											// 長手位置[mm]
			pQue->dDS = dDS;											// 幅方向位置 DS～
			pQue->dWS = dWS;											// 幅方向位置 WS～
			pQue->nKizuSeq = m_pTestCoilInf[nRec].nKizuSeq;				// 疵連番

			bool output_dsp = false;
			bool output_lst = false;
			bool mark_kei = false;
			bool mark_jyu = false;
			int yuusenT = mcls_pParam->GetYusenT(pQue->nTid);
			int yuusenG = mcls_pParam->GetYusenG(pQue->nGid);
			int kekkanG = mcls_pParam->GetDefDivG(pQue->nGid);
			// 出力対象の疵か判定
			if( 0 == nMen ){
				// 探傷画面出力対象か	
				if(mcls_pParam->Check_Output(ParamManager::OUTPUTFLG_DSP_O,pcode-1,pQue->nTid,pQue->nGid) ) output_dsp = true;
			} else {
				// 探傷画面出力対象か	
				if(mcls_pParam->Check_Output(ParamManager::OUTPUTFLG_DSP_U,pcode-1,pQue->nTid,pQue->nGid) ) output_dsp = true;
			}
			pQue->nObjTansho = output_dsp;

			// 出力対象の疵か判定(重欠陥一覧画面)
			if (1 == mcls_pParam->GetCommonMast().nDeflistFilter) {
				if (mcls_pParam->Check_PallOutput(ParamManager::PALLOUT_SIMPLE, pQue->nTid, pQue->nGid)) output_lst = true;
			}
			else {
				if (mcls_pParam->Check_PallOutput(ParamManager::PALLOUT_DETAIL, pQue->nTid, pQue->nGid)) output_lst = true;
			}
			pQue->nObjDeflist = output_lst;
			pQue->dspmode = 0;
			if (1 == pQue->nObjDeflist) {
				int nlst = rand() % 100;	// グレードEdasysIDは、0 ～ 99のランダムな値で決定
				if (10 > nlst) {			// 0 ～ 9	10%
					pQue->dspmode = 1;		// 強調表示
				}
			}
			pQue->nSysKbn = 0;		// 検出システム（中央）

			// 最終的に表示管理に送信する疵か判定する
			if( (output_dsp || output_lst ) && (-1 != yuusenG)) {
				LOG(em_MSG),"[%s] 送信疵決定(%d) <%s> men[%d] Seq[%d] y[%d] x[%d] DS[%f] WS[%f] TID[%d] GID[%d] dsp[%d] lst[%d]", my_sThreadName,0,cKizukenNo,nMen,pQue->nKizuSeq , pQue->nInY,nCol,dDS,dWS , pQue->nTid , pQue->nGid , output_dsp, output_lst);
				// 疵連番カウントアップ
				m_pTestCoilInf[nRec].nKizuSeq++;
				// 疵個数
				nNum++;
				// 疵情報送信要求
				mcls_pImgSend->gque_Deli.AddToTail(pQue);
			} else {
				// 指示された疵は送信対象ではない
				//LOG(em_INF),"[%s] 送信未対象疵(%d) <%s> men[%d] Seq[%d] y[%d/%d] x[%d] DS[%f] WS[%f] TID[%d] GID[%d] dsp[%d] lst[%d] map[%d]", my_sThreadName,0,cKizukenNo,nMen,pQue->nKizuSeq , pQue->nInY[0],pQue->nInY[1], nCol,dDS,dWS , pQue->nTid , pQue->nGid , output_dsp, output_lst, output_map);
				// 送信しないので、キューは削除する
				SAFE_DELETE(pQue);
			}
		}
	}
}

//// 指定された重欠陥疵情報削除要求を送信
void TestManager::SendDefLstCancel(char const* cKizukenNo, int nMen, int nCancelNo, int nSystem)
{
	//// 送信データ生成 (送信先で開放する事) 
	// 送信データサイズを求める(※+1:仮データ部)
	int nSendSize = sizeof(TO_SO_HEAD) ;

	// BYTE配列を作成する
	BYTE* pSend = new BYTE[nSendSize];
	memset(pSend, 0, nSendSize);

	// ヘッダ部先頭アドレスをセットする
	TO_SO_HEAD& head = (TO_SO_HEAD&)pSend[0];

	// ベースヘッダ部
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	head.base_head.subinf = nSystem;
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len = nSendSize;
	head.base_head.addr[0] = NULL;
	head.base_head.addr[1] = NULL;

	// 拡張ヘッダ部
	// 疵検管理Noセット
	memcpy(head.KizukenNo, cKizukenNo, sizeof(head.KizukenNo));
	// 表・裏 セット（未使用）
	head.men = nMen;
	// データＩＤセット
	head.kind = TO_SO_DATA_KIND_DEFLIST_CANCEL;

	head.deflist_cancel.nKizuSeq = nCancelNo;

	// 送信
	mcls_pOpSend->gque_Deli.AddToTail((SOCK_BASE_HEAD*)pSend);
	LOG(em_MSG), "[%s] 重欠陥疵情報削除要求 送信 <%s> men[%d] kind[%d] Seq[%d] System[%d]", 
		my_sThreadName, head.KizukenNo, head.men, head.kind, head.deflist_cancel.nKizuSeq, head.base_head.subinf);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 3.コイル情報 (検査用) / 	5.コイル情報 (表示用) セット
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void TestManager::SendCoilInfo(int nRec,int nDiv)
{
	//// 送信データ生成 (送信先で開放する事) 
	// 送信データサイズを求める
	int nSendSize = sizeof(TO_SO_HEAD) + sizeof(TO_SO_DATA_COIL);
	
	// BYTE配列を作成する
	BYTE* pSend = new BYTE[nSendSize];
	memset( pSend,0,nSendSize );

	// ヘッダ部先頭アドレスをセットする
	TO_SO_HEAD& head = (TO_SO_HEAD&)pSend[0];

	// データ部先頭アドレスをセットする
	TO_SO_DATA_COIL& data = (TO_SO_DATA_COIL&)pSend[sizeof(TO_SO_HEAD)];

	// ベースヘッダ部
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len = nSendSize;
	head.base_head.addr[0] = NULL;
	head.base_head.addr[1] = NULL;

	// 拡張ヘッダ部
	// 管理Noセット
	memcpy( head.KizukenNo, m_pTestCoilInf[nRec].cKizukenNo , sizeof(head.KizukenNo)); 
	//// 表・裏 セット（未使用）
	//head.men = -1;

	if( 0 == nDiv ){
		// 検査用コイル情報セット
		head.kind = TO_SO_DATA_KIND_COIL_KEN;
	} else {
		// 表示用コイル情報セット
		head.kind = TO_SO_DATA_KIND_COIL_DSP;
		head.data_coilinf_dsp.nSpmUmu = m_pTestCoilInf[nRec].nSpmUmu;
	}

	// コイルベース部セット
	memcpy( &data.coilbase  , &m_pCoilBase[nRec] , sizeof(data.coilbase) );

	// コイル情報部セット(プロコン受信情報)
	memcpy( &data.coilinf , &m_pCoilInf[nRec] , sizeof(data.coilinf) );

	// 検査表面状態  (1オリジン)
	data.scode[0] = m_pTestCoilInf[nRec].scode[0];
	data.scode[1] = m_pTestCoilInf[nRec].scode[1];
	// 検査パターン  (1オリジン)
	data.pcode[0] = m_pTestCoilInf[nRec].pcode[0];
	data.pcode[1] = m_pTestCoilInf[nRec].pcode[1];
	// マップ列数 
	data.nMapColNum = m_pTestCoilInf[nRec].nMapColNum;

	// 送信
	mcls_pOpSend->gque_Deli.AddToTail((SOCK_BASE_HEAD*)pSend);

	// 前工程情報管理にDB作成要求
	if (m_bMaeInfo && 0 == nDiv) {
		SendMaeInf(nRec);
	}
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 4.コイル実績 (1コイル検査完了時)	送信
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void TestManager::SendCoilResult(int nRec)
{
	//// 送信データ生成 (送信先で開放する事) 
	// 送信データサイズを求める
	int nSendSize = sizeof(TO_SO_HEAD);
	
	// BYTE配列を作成する
	BYTE* pSend = new BYTE[nSendSize];
	memset( pSend,0,nSendSize );

	// ヘッダ部先頭アドレスをセットする
	TO_SO_HEAD& head = (TO_SO_HEAD&)pSend[0];

	// ベースヘッダ部
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len = nSendSize;
	head.base_head.addr[0] = NULL;
	head.base_head.addr[1] = NULL;

	// 拡張ヘッダ部
	// 疵検管理Noセット
	memcpy( head.KizukenNo,  m_pTestCoilInf[nRec].cKizukenNo , sizeof(head.KizukenNo)); 
	// データＩＤセット
	head.kind = TO_SO_DATA_KIND_COIL_RESULT;
	//// 表・裏 セット（未使用）
	//head.men = -1;

	// 実測長 [mm] セット
	head.data_coilresult.len = m_pTestCoilInf[nRec].nCoilLen;

	// 送信
	mcls_pOpSend->gque_Deli.AddToTail((SOCK_BASE_HEAD*)pSend);

}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 5.通板位置情報(検査装置位置、表示基点位置、各表示コイル通板位置情報)
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void TestManager::SendPosInfo(int nRec)
{
	//// 送信データ生成 (送信先で開放する事) 
	// 送信データサイズを求める(※+1:仮データ部)
	int nSendSize = sizeof(TO_SO_HEAD) + sizeof(TO_SO_DATA_LENGTH);

	// BYTE配列を作成する
	BYTE* pSend = new BYTE[nSendSize];
	memset( pSend,0,nSendSize );

	// ヘッダ部先頭アドレスをセットする
	TO_SO_HEAD& head = (TO_SO_HEAD&)pSend[0];
	TO_SO_DATA_LENGTH& data = (TO_SO_DATA_LENGTH&)pSend[sizeof(TO_SO_HEAD)];

	// ベースヘッダ部
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len = nSendSize;
	head.base_head.addr[0] = NULL;
	head.base_head.addr[1] = NULL;

	// 拡張ヘッダ部
	// 疵検管理Noセット
	memcpy( head.KizukenNo,  m_pTestCoilInf[nRec].cKizukenNo , sizeof(head.KizukenNo)); 
	// 表・裏 セット（未使用）
	head.men = -1;
	// データＩＤセット
	head.kind = TO_SO_DATA_KIND_LENGTH;

	//// データ部セット
	// 各検査台距離情報セット
	for(int pos = 0; pos < MAX_DISP_POS; pos++ ){
		memcpy( data.dsp[pos].KizukenNo  , m_pDspInf[pos].cKizukenNo  ,  SIZE_KIZUKEN_NO );
		data.dsp[pos].len = int( m_pDspInf[pos].nDistance / 1000);
		//for(int nRec=0;nRec<MAX_COIL_NUM;nRec++){
		//	TEST_COIL_INF *pTestCoilInf = &m_pTestCoilInf[nRec];
		//	if (0 == pTestCoilInf->cKizukenNo[0]) {
		//		continue;
		//	}
		//	if (memcmp(pTestCoilInf->cKizukenNo, m_pDspInf[pos].cKizukenNo, sizeof(pTestCoilInf->cKizukenNo))) {
		//		continue;
		//	}
		//	//// 溶接点残長(検査台位置での入側単位の残長)
		//	//data.nLenZan[pos] = (pTestCoilInf->nCoilLen[0] - m_pDspInf[pos].nDistance)/1000;
		//	//break;
		//}
	}

	//LOG(em_MSG),"[%s] 通板位置情報 検査台：カメラ位置[%s][%d/%d][%d]表示基点[%s][%d/%d][%d]",
	//	my_sThreadName,
	//	data.dsp[0].KizukenNo, data.dsp[0].len[0], data.dsp[0].len[1], data.nLenZan[0],
	//	data.dsp[1].KizukenNo, data.dsp[1].len[0], data.dsp[1].len[1], data.nLenZan[1] );

	// 送信
	mcls_pOpSend->gque_Deli.AddToTail((SOCK_BASE_HEAD*)pSend);

}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 6.カット情報等
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void TestManager::SendCutInfo(char const* cKizukenNo , int nPos, int nMode )
{
	int	nRec;
	for (nRec=0; nRec<MAX_COIL_NUM; nRec++) {
		if( 0 == memcmp((&m_pTestCoilInf[nRec])->cKizukenNo, cKizukenNo, sizeof((&m_pTestCoilInf[nRec])->cKizukenNo))) {
			break;
		}
	}
	if (nRec==MAX_COIL_NUM) {
		return;
	}

	//// 送信データ生成 (送信先で開放する事) 
	// 送信データサイズを求める(※+1:仮データ部)
	int nSendSize = sizeof(TO_SO_HEAD) + sizeof(TO_SO_DATA_CUT);

	// BYTE配列を作成する
	BYTE* pSend = new BYTE[nSendSize];
	memset( pSend,0,nSendSize );

	// ヘッダ部先頭アドレスをセットする
	TO_SO_HEAD& head = (TO_SO_HEAD&)pSend[0];
	TO_SO_DATA_CUT& data = (TO_SO_DATA_CUT&)pSend[sizeof(TO_SO_HEAD)];

	// ベースヘッダ部
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len = nSendSize;
	head.base_head.addr[0] = NULL;
	head.base_head.addr[1] = NULL;

	// 拡張ヘッダ部
	// 管理Noセット
	memcpy( head.KizukenNo,  cKizukenNo , sizeof(head.KizukenNo)); 
	// 表・裏 セット（未使用）
	head.men = -1;
	// データＩＤセット
	head.kind = TO_SO_DATA_KIND_CUT;
	// データ部セット
	if (0 == nMode) {
		data.nMode =  1;
	} else {
		data.nMode = (EM_DIV_CUT)nMode;
	}
	// カット位置
	data.nPos = nPos;

	// 送信
	mcls_pOpSend->gque_Deli.AddToTail((SOCK_BASE_HEAD*)pSend);

	LOG(em_MSG),"[%s] カット情報送信 <%s> nMode = %d nPos = %d", my_sThreadName , cKizukenNo, data.nMode, data.nPos);

	(&m_pTestCoilInf[nRec])->nCutNum++;

}

//// 指定された警報情報を送信
void TestManager::SendAlarm(DELIVERY_KEY* pDeli)
{
	//// 送信データ生成 (送信先で開放する事) 
	// 送信データサイズを求める(※+1:仮データ部)
	int nSendSize = sizeof(TO_SO_HEAD) + sizeof(TO_SO_DATA_ALARM);

	// BYTE配列を作成する
	BYTE* pSend = new BYTE[nSendSize];
	memset(pSend, 0, nSendSize);

	// ヘッダ部先頭アドレスをセットする
	TO_SO_HEAD& head = (TO_SO_HEAD&)pSend[0];
	TO_SO_DATA_ALARM& data = (TO_SO_DATA_ALARM&)pSend[sizeof(TO_SO_HEAD)];

	// ベースヘッダ部
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	head.base_head.subinf = 0;
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len = nSendSize;
	head.base_head.addr[0] = NULL;
	head.base_head.addr[1] = NULL;

	// 拡張ヘッダ部
	// 疵検管理Noセット
	//  = NULL
	// 表・裏 セット（未使用）
	head.men = -1;
	// データＩＤセット
	head.kind = TO_SO_DATA_KIND_ALARM;
	for (int kk = 0; kk < MAX_ALARME_POS; kk++) {
		for (int ii = 0; ii < 2; ii++) {
			for (int jj = 0; jj < 8; jj++) {
				if (1 == pDeli->nLanp[kk][ii][jj] || 2 == pDeli->nLanp[kk][ii][jj]) {
					data.a_mode[kk][ii][jj] = pDeli->nLanp[kk][ii][jj];
				}
				else {
					data.a_mode[kk][ii][jj] = 0;
				}
			}
		}
	}
	// 送信
	mcls_pOpSend->gque_Deli.AddToTail((SOCK_BASE_HEAD*)pSend);
	LOG(em_MSG), "[%s] 警報情報 送信", my_sThreadName);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 9.疵個数情報 (最小区間内)
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void TestManager::SendDefCount(DELIVERY_KEY* pDeli)
{
	//// 送信データ生成 (送信先で開放する事) 
	// 送信データサイズを求める(※+1:仮データ部)
	int nSendSize = sizeof(TO_SO_HEAD);

	// BYTE配列を作成する
	BYTE* pSend = new BYTE[nSendSize];
	memset(pSend, 0, nSendSize);

	// ヘッダ部先頭アドレスをセットする
	TO_SO_HEAD& head = (TO_SO_HEAD&)pSend[0];
	TO_SO_DATA_ALARM& data = (TO_SO_DATA_ALARM&)pSend[sizeof(TO_SO_HEAD)];

	// ベースヘッダ部
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	head.base_head.subinf = 0;
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len = nSendSize;
	head.base_head.addr[0] = NULL;
	head.base_head.addr[1] = NULL;

	// 拡張ヘッダ部
	// 疵検管理Noセット
	memcpy(head.KizukenNo, pDeli->cKizukenNo, sizeof(head.KizukenNo));
	// 表・裏 セット
	head.men = pDeli->nMen;
	// データＩＤセット
	head.kind = TO_SO_DATA_KIND_DETECT_NUM;
	// 実測長セット
	head.data_defnum.len = pDeli->nY;
	// 疵個数セット
	head.data_defnum.num = pDeli->nCnt;

	// 送信
	mcls_pOpSend->gque_Deli.AddToTail((SOCK_BASE_HEAD*)pSend);
	LOG(em_MSG), "[%s] 疵個数情報 送信", my_sThreadName);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 新規コイル情報作成
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void TestManager::AddNewCoil()
{
	static int SeqNo = 0;
	char	buff[32];
	char	cKanriNo[SIZE_KIZUKEN_NO + 1];

	SeqNo = ((SeqNo + 1) % 100) + 1;

	// 最古のコイルの通板距離がコイル長に満たない（検査未完）の場合は新規追加は出来ない
	if ((&m_pTestCoilInf[MAX_COIL_NUM - 1])->nCoilLen) {
		if ((&m_pTestCoilInf[MAX_COIL_NUM - 1])->nCoilLen > (&m_pTestCoilInf[MAX_COIL_NUM - 1])->nLen) {
			LOG(em_ERR), "[%s] 新規コイル追加は出来ません", my_sThreadName);
			return;
		}
		// 検査台情報に管理Noが存在する(通板処理中)場合も新規追加は不可
		for (int pos = 0; pos < DIV_LEN_POS_FINAL; pos++) {
			if (0 == memcmp(m_pDspInf[pos].cKizukenNo, (&m_pTestCoilInf[MAX_COIL_NUM - 1])->cKizukenNo, sizeof(m_pDspInf[pos].cKizukenNo))) {
				LOG(em_ERR), "[%s] 新規コイル追加は出来ません。", my_sThreadName);
				return;
			}
		}
	}

	// 管理No取得
	KizuFunction::GetKizukenNo(cKanriNo);
	// 重複を確認
	for (int ii = 0; ii < MAX_COIL_NUM; ii++) {
		if (0 == memcmp((&m_pTestCoilInf[ii])->cKizukenNo, cKanriNo, sizeof((&m_pTestCoilInf[ii])->cKizukenNo))) {
			// 作成した管理Noが重複してしまった
			LOG(em_ERR), "[%s] 新規コイル追加は出来ません。", my_sThreadName);
			return;
		}
	}

	// 各コイルデータテーブルの先頭を空ける
	for (int ii = MAX_COIL_NUM - 1; ii >= 1; ii--) {
		memmove(&m_pTestCoilInf[ii], &m_pTestCoilInf[ii - 1], sizeof(m_pTestCoilInf[ii]));
		memmove(&m_pCoilBase[ii], &m_pCoilBase[ii - 1], sizeof(m_pCoilBase[ii]));
		memmove(&m_pCoilInf[ii], &m_pCoilInf[ii - 1], sizeof(m_pCoilInf[ii]));
	}

	// ----------------------------------------------------
	// TestCoil 情報セット
	// ----------------------------------------------------
	TEST_COIL_INF* pTestCoilInf = &m_pTestCoilInf[0];
	memset(pTestCoilInf, 0x00, sizeof(TEST_COIL_INF));

	// 管理No
	memmove(pTestCoilInf->cKizukenNo, cKanriNo, sizeof(pTestCoilInf->cKizukenNo));

	// SPM圧下有無
	if (rand() % 2) {
		pTestCoilInf->nSpmUmu = DIV_SPM_ON;
		// SPM伸び率
		int nNobiSPM = 100 + (rand() % 50);
		pTestCoilInf->dNobiSPM = (double)nNobiSPM/100;
	} else {
		pTestCoilInf->nSpmUmu = DIV_SPM_OFF;
		pTestCoilInf->dNobiSPM = 1.00;
	}

	// コイル長セット（最小長～最大長の範囲で決定する)
	pTestCoilInf->nCoilLen = ((rand() % ((m_nCoilLenMax / 1000 - m_nCoilLenMin / 1000) + 1)) + m_nCoilLenMin / 1000) * 1000;

	// コイル幅決定[mm]
	pTestCoilInf->nCoilWidth = ((rand() % ((m_nCoilWidthMax - m_nCoilWidthMin) + 1))) + m_nCoilWidthMin;

	// MAP分割数決定
	int col = (int)(pTestCoilInf->nCoilWidth / 2) / MAP_COL_INTERVAL;
	if ((pTestCoilInf->nCoilWidth / 2) % MAP_COL_INTERVAL) {
		col += 1;
	}
	pTestCoilInf->nMapColNum = col * 2;
	if (pTestCoilInf->nMapColNum > MAP_COL_NUM) {
		pTestCoilInf->nMapColNum = MAP_COL_NUM;
	}

	// 通板距離初期化
	pTestCoilInf->nLen = 0 ;

	// SCODEセット
	if( 0 < m_scode && m_scode < MAX_SCODE ){
		pTestCoilInf->scode[0] = m_scode;
		pTestCoilInf->scode[1] = m_scode;
	} else {
		pTestCoilInf->scode[0] = MAX_SCODE;
		pTestCoilInf->scode[1] = MAX_SCODE;
	}

	// PCODEセット
	if( 0 < m_pcode && m_pcode < MAX_PCODE ){
		pTestCoilInf->pcode[0] = m_pcode;
		pTestCoilInf->pcode[1] = m_pcode;
	} else {
		pTestCoilInf->pcode[0] = MAX_PCODE;
		pTestCoilInf->pcode[1] = MAX_PCODE;
	}

	// コイルNo
	memset(pTestCoilInf->cCoilNo, 0, sizeof(pTestCoilInf->cCoilNo));
	sprintf(pTestCoilInf->cCoilNo, "T%0*d", (sizeof(pTestCoilInf->cCoilNo)-2), SeqNo);

	// 命令No
	memset(pTestCoilInf->cMeiNo, 0, sizeof(pTestCoilInf->cMeiNo));
	sprintf(pTestCoilInf->cMeiNo,"%0*d ", (sizeof(pTestCoilInf->cMeiNo)-2), SeqNo);

	// 現在検査対象の疵連番を1から開始
	pTestCoilInf->nKizuSeq = 1;

	// シャーカット回数は0
	pTestCoilInf->nCutNum = 0;

	// ----------------------------------------------------
	// CoilBase 情報セット
	// ----------------------------------------------------
	COIL_BASEDATA* pCoilBase = &m_pCoilBase[0];
	memset(pCoilBase, 0x00 , sizeof(COIL_BASEDATA));

	// コイルNo
	memcpy( pCoilBase->cCoilNoIn , pTestCoilInf->cCoilNo , sizeof(pTestCoilInf->cCoilNo)-1);

	// 命令No
	memcpy( pCoilBase->cMeiNo , pTestCoilInf->cMeiNo , sizeof(pTestCoilInf->cMeiNo)-1);

	// キー情報(コイルNo+命令No)
	memset(buff,0,sizeof(buff));
	strcat(buff,pCoilBase->cCoilNoIn);
	strcat(buff,pCoilBase->cMeiNo);
	memcpy( pCoilBase->cCoilKey , buff, sizeof(pCoilBase->cCoilKey)-1);

	// 予備1
	memset(pCoilBase->cYobi1,0,sizeof(pCoilBase->cYobi1));

	// 入側コイル長[mm]
	pCoilBase->nLenIn = pTestCoilInf->nCoilLen;

	// 板厚[μm] とりあえず1FLL仕様のまま 0.26 ～ 0.30[mm]　⇒ 260[μm] ～ 300[μm]
	pCoilBase->nAtu = (rand() % ( 300 - 260 )) + 260;

	// 板幅[mm]
	pCoilBase->nWid = pTestCoilInf->nCoilWidth;

	// 入側フロント端板長さ [mm] (上位情報の長さ)
	pCoilBase->nCutLenF = 0;
	// 入側テール端板長さ　[mm] (上位情報の長さ)
	pCoilBase->nCutLenT = 0;
	// 出側コイル長 [m]
	pCoilBase->nLenOut = (pTestCoilInf->nCoilLen/1000);

	// 予備
	memset(pCoilBase->cYobi2,0,sizeof(pCoilBase->cYobi2));
	memset(pCoilBase->cYobi3, 0, sizeof(pCoilBase->cYobi3));

	// ----------------------------------------------------
	// CoilInf 情報セット(プロコンコイル情報)
	// ----------------------------------------------------
	PRO_TYPE_DATA_COILINF* pCoilInf = &m_pCoilInf[0];
	// プロコンデータを初期化
	memset(pCoilInf , 0, sizeof(PRO_TYPE_DATA_COILINF));

	// コイルNo
	memcpy(pCoilInf->CoilInf.KeyInf.cCoilNo, pTestCoilInf->cCoilNo, sizeof(pCoilInf->CoilInf.KeyInf.cCoilNo));
	// 命令No
	memcpy(pCoilInf->CoilInf.KeyInf.cMeiNo, pTestCoilInf->cMeiNo, sizeof(pCoilInf->CoilInf.KeyInf.cMeiNo));

	pCoilInf->CoilInf.nSendPoint = 0;					// 送信位置
	pCoilInf->CoilInf.nLenIn = 0;						// コイル長[m]
	pCoilInf->CoilInf.nAtuZai = 0;					// 材料板厚[10-3mm]
	pCoilInf->CoilInf.nWidZai = 0;					// 材料板幅[mm]
	pCoilInf->CoilInf.nAtuSei = 0;					// 注文板厚[10-3mm]
	pCoilInf->CoilInf.nWidSei = 0;					// 注文板幅[mm]
	pCoilInf->CoilInf.nWidTrim = 0;					// 現工程トリム幅[mm]
	pCoilInf->CoilInf.nNobiSPM = 0;					// SPM伸び率[10-2%]
	pCoilInf->CoilInf.nNobiTLV = 0;					// TVL伸び率[10-2%]
	pCoilInf->CoilInf.nCutF = 0;				// 入側フロント端板長さ[mm]
	pCoilInf->CoilInf.nCutT = 0;					// 入側テール端板長さ[mm]
	pCoilInf->CoilInf.nSPMRoolKei_Up = 0;				// SPMワークロール径(上)[10-1mm]
	pCoilInf->CoilInf.nSPMRoolKei_Down = 0;				// SPMワークロール径(下)[10-1mm]
	memset(pCoilInf->CoilInf.nYobi, 0, sizeof(pCoilInf->CoilInf.nYobi));	// 予備
	memset(pCoilInf->CoilInf.cKenKijun_O, ' ', sizeof(pCoilInf->CoilInf.cKenKijun_O));			// 検査基準(表)
	memset(pCoilInf->CoilInf.cKenKijun_U, ' ', sizeof(pCoilInf->CoilInf.cKenKijun_U));			// 検査基準(裏)
	memset(pCoilInf->CoilInf.cKurometo_Kind, ' ', sizeof(pCoilInf->CoilInf.cKurometo_Kind));	// クロメート命令_種類
	memset(pCoilInf->CoilInf.cKurometo_Umu, ' ', sizeof(pCoilInf->CoilInf.cKurometo_Umu));		// クロメート命令_有無
	memset(pCoilInf->CoilInf.cHinCode, ' ', sizeof(pCoilInf->CoilInf.cHinCode));				// 品種
	memset(pCoilInf->CoilInf.cMekkiKind, ' ', sizeof(pCoilInf->CoilInf.cMekkiKind));			// メッキ種類
	memset(pCoilInf->CoilInf.cSyuZai, ' ', sizeof(pCoilInf->CoilInf.cSyuZai));				// 主材質
	memset(pCoilInf->CoilInf.cDifMaker, ' ', sizeof(pCoilInf->CoilInf.cDifMaker));				// ディファレンシャルマーク
	memset(pCoilInf->CoilInf.cKenban, ' ', sizeof(pCoilInf->CoilInf.cKenban));					// 原板区分
	memset(pCoilInf->CoilInf.cMekkiCycle, ' ', sizeof(pCoilInf->CoilInf.cMekkiCycle));			// メッキサイクル
	memset(pCoilInf->CoilInf.cInLineSPM, ' ', sizeof(pCoilInf->CoilInf.cInLineSPM));		// レベラー_インラインSPM
	memset(pCoilInf->CoilInf.cKagakuToyu, ' ', sizeof(pCoilInf->CoilInf.cKagakuToyu));				// 化学処理塗油
	memset(pCoilInf->CoilInf.cAtukkaSPM, ' ', sizeof(pCoilInf->CoilInf.cAtukkaSPM));				// オフラインSPM圧下率
	memset(pCoilInf->CoilInf.cMekkiKubun, ' ', sizeof(pCoilInf->CoilInf.cMekkiKubun));			// メッキ区分
	memset(pCoilInf->CoilInf.cOmoteSyori1, ' ', sizeof(pCoilInf->CoilInf.cOmoteSyori1));				// 表面処理1桁目
	memset(pCoilInf->CoilInf.cOmoteSyori2, ' ', sizeof(pCoilInf->CoilInf.cOmoteSyori2));				// 表面処理2桁目
	memset(pCoilInf->CoilInf.cToyu, ' ', sizeof(pCoilInf->CoilInf.cToyu));					// 塗油
	memset(pCoilInf->CoilInf.cKouteiYobi, ' ', sizeof(pCoilInf->CoilInf.cKouteiYobi));					// 工程記号予備
	memset(pCoilInf->CoilInf.cBaund, ' ', sizeof(pCoilInf->CoilInf.cBaund));					// 予備
	memset(pCoilInf->CoilInf.cCaplSyuZai, ' ', sizeof(pCoilInf->CoilInf.cCaplSyuZai));				// CAPL主材質
	memset(pCoilInf->CoilInf.cMetuke, ' ', sizeof(pCoilInf->CoilInf.cMetuke));	// 目付規格
	memset(pCoilInf->CoilInf.cNi, ' ', sizeof(pCoilInf->CoilInf.cNi));							// Ni付着量
	memset(pCoilInf->CoilInf.cSodoTCM, ' ', sizeof(pCoilInf->CoilInf.cSodoTCM));						// TCMロール粗度区分
	memset(pCoilInf->CoilInf.cSpmUmu, ' ', sizeof(pCoilInf->CoilInf.cSpmUmu));					// SPM圧下有無
	memset(pCoilInf->CoilInf.cNextLine, ' ', sizeof(pCoilInf->CoilInf.cNextLine));				// 計画次工程コード
	memset(pCoilInf->CoilInf.cMuke, ' ', sizeof(pCoilInf->CoilInf.cMuke));						// 向先
	memset(pCoilInf->CoilInf.cYouto, ' ', sizeof(pCoilInf->CoilInf.cYouto));						// 用途
	memset(pCoilInf->CoilInf.cCaplSiage, ' ', sizeof(pCoilInf->CoilInf.cCaplSiage));		// CAPL表面仕上げ
	memset(pCoilInf->CoilInf.cCaplSanUmu, ' ', sizeof(pCoilInf->CoilInf.cCaplSanUmu));				// CAPL酸洗有無
	memset(pCoilInf->CoilInf.cHinMode, ' ', sizeof(pCoilInf->CoilInf.cHinMode));				// 品種区分
	memset(pCoilInf->CoilInf.cGaCrKubun, ' ', sizeof(pCoilInf->CoilInf.cGaCrKubun));					// GA/CR区分
	memset(pCoilInf->CoilInf.cSpmJisekiUmu, ' ', sizeof(pCoilInf->CoilInf.cSpmJisekiUmu));			// SPM圧下実績有無
	memset(pCoilInf->CoilInf.cKouteiCode, ' ', sizeof(pCoilInf->CoilInf.cKouteiCode));						// 工程コード
	memset(pCoilInf->CoilInf.cYobi1, ' ', sizeof(pCoilInf->CoilInf.cYobi1));					// 予備

	// 主材質
	pCoilInf->CoilInf.cSyuZai[0] = 0x31 + (rand() % 7);
	pCoilInf->CoilInf.cSyuZai[1] = 0x41 + (rand() % 26);

	// 材料板厚[10-3mm]
	pCoilInf->CoilInf.nAtuZai = pCoilBase->nAtu;
	// 材料板幅[mm]
	pCoilInf->CoilInf.nWidZai = pCoilBase->nWid;

	// 注文板厚[10-3mm]
	pCoilInf->CoilInf.nAtuSei = pCoilBase->nAtu;
	// 注文板幅[mm]
	pCoilInf->CoilInf.nWidSei = pCoilBase->nWid;

	// 向先コード ランダムで5パターン程度から選ぶ
	switch( rand() % 5 ){
		case 0:
			memcpy( pCoilInf->CoilInf.cMuke, "Mukesaki 01 & TEST   " , sizeof(pCoilInf->CoilInf.cMuke));
			break;
		case 1:
			memcpy( pCoilInf->CoilInf.cMuke, "Muke 02 @ 12345      " , sizeof(pCoilInf->CoilInf.cMuke));
			break;
		case 2:
			memcpy( pCoilInf->CoilInf.cMuke, "<-   Max length    ->" , sizeof(pCoilInf->CoilInf.cMuke));
			break;
		case 3:
			memcpy( pCoilInf->CoilInf.cMuke, "<-  ->              >" , sizeof(pCoilInf->CoilInf.cMuke));
			break;
		case 4:
			memcpy( pCoilInf->CoilInf.cMuke, "test TEST test TEST  " , sizeof(pCoilInf->CoilInf.cMuke));
			break;
		default:
			memcpy( pCoilInf->CoilInf.cMuke, "                     " , sizeof(pCoilInf->CoilInf.cMuke));
			break;
	}
	// 品種区分
	pCoilInf->CoilInf.cHinMode[0] = 0x41 + (rand() % 26);

	// 品種
	char cHinCode[] = { 'H', 'Z', 'G', 'A', 'B', 'S', 'U', 'Q' };
	pCoilInf->CoilInf.cHinCode[0] = cHinCode[(rand() % 6)];

	// 用途
	memset(buff, 0, sizeof(buff));
	sprintf(buff,"%0*d", sizeof(sizeof(pCoilInf->CoilInf.cYouto)), (1000*((rand() % 9)+1)+(rand()%1000)));
	memcpy(pCoilInf->CoilInf.cYouto, buff, sizeof(pCoilInf->CoilInf.cYouto));

	// 計画次工程コード
	memset(buff, 0, sizeof(buff));
	sprintf(buff, "%0*d", sizeof(pCoilInf->CoilInf.cNextLine), (rand() % 100));
	memcpy(pCoilInf->CoilInf.cNextLine, buff, sizeof(pCoilInf->CoilInf.cNextLine));

	// 検査基準(表)
	memset(buff, 0, sizeof(buff));
	sprintf(buff, "%0*d", sizeof(pCoilInf->CoilInf.cKenKijun_O), (rand() % 1000));
	memcpy(pCoilInf->CoilInf.cKenKijun_O, buff, sizeof(pCoilInf->CoilInf.cKenKijun_O));

	// 検査基準(裏)
	memset(buff, 0, sizeof(buff));
	sprintf(buff, "%0*d", sizeof(pCoilInf->CoilInf.cKenKijun_U), (rand() % 1000));
	memcpy(pCoilInf->CoilInf.cKenKijun_U, buff, sizeof(pCoilInf->CoilInf.cKenKijun_U));

	// 本来コイル情報はここで送信すべきではない。うまく動作しないので、今はここにしている
	// コイル情報送信(検査用)
	LOG(em_INF),"[%s] 検査用コイル情報送信[%s]", my_sThreadName,(&m_pTestCoilInf[0])->cKizukenNo);
	SendCoilInfo(0,0);

	if (true == m_bInitial) {

		// イニシャルのみ、通板距離をカメラ位置にする。（検査基点からカメラ位置までの通板を省略）
		pTestCoilInf->nLen = mcls_pParam->GetCommonMast().nBaseToCamera - 1;

		m_bInitial = false;
	}

	//// テスト的にステータス情報を送信する
	//SendStatusInfo();
}



// 位置情報を更新する
void TestManager::UpdateDispInf(int nRec)
{
	int	from;
	int	to;
	int	men;
	int	n;

	// 各位置の距離計算
	static int distance[DIV_LEN_POS_FINAL];

	if( distance[0] == 0){
		distance[DIV_LEN_POS_KEN_BOT] = mcls_pParam->GetCommonMast().nBaseToSpm + mcls_pParam->GetCommonMast().nSpmToUraCamera;	// 検査基点WPD～カメラ位置距離(裏検査位置)
		distance[DIV_LEN_POS_KEN_TOP] = distance[DIV_LEN_POS_KEN_BOT] + mcls_pParam->GetCommonMast().nCameraToCamera;	// 検査基点WPD～カメラ位置距離(表検査位置)
		//distance[DIV_LEN_POS_DSP_TOP] = distance[DIV_LEN_POS_KEN_BOT] + mcls_pParam->GetCommonMast().nCameraToDsp + mcls_pParam->GetCommonMast().nDspBaseToDsp;	// カメラ位置～表示基点距離
		distance[DIV_LEN_POS_DSP_BOT] = distance[DIV_LEN_POS_KEN_BOT] + mcls_pParam->GetCommonMast().nCameraToDsp + mcls_pParam->GetCommonMast().nDspToUraCamera;	// カメラ位置～表示基点距離
		distance[DIV_LEN_POS_DSP_TOP] = distance[DIV_LEN_POS_KEN_BOT] + mcls_pParam->GetCommonMast().nCameraToDsp + mcls_pParam->GetCommonMast().nDspToOmoteCamera;												// カメラ位置～表示基点距離

		distance[DIV_LEN_POS_SHR_I] = distance[DIV_LEN_POS_KEN_BOT] + mcls_pParam->GetCommonMast().nCameraToDsp + mcls_pParam->GetCommonMast().nDspToCut;		// 表示基点～シャーカット位置距離

		for(int pos=0;pos<DIV_LEN_POS_FINAL;pos++){
			LOG(em_INF),"[%s] distance[%d] = [%d]", my_sThreadName, pos , distance[pos]);
		}
	}

	// 各位置の距離計算
	for( int pos = DIV_LEN_POS_FINAL -1 ; pos >= 0; pos-- ) {

		switch (pos) {
		case DIV_LEN_POS_KEN_TOP:					// カメラ位置
		case DIV_LEN_POS_KEN_BOT:					// カメラ位置

			// コイル先端から検査基点位置までの距離[mm]を算出する
			n =  (&m_pTestCoilInf[nRec])->nLen - distance[pos];
			men = (pos == DIV_LEN_POS_KEN_TOP)?0:1;

			if( n < 0 ) {
				// 値がマイナスということは、カメラ位置には１つ前のコイルが通過中となる
				// １つ前のコイル通板距離(=コイル長) から計算する。
				// １つ前のコイル情報が存在するか
				if (MAX_COIL_NUM == nRec+1) {
					// １つ前のコイル情報は存在しない
					m_pDspInf[pos].nDistance = 0;
					m_pDspInf[pos].nRec = 0;
					memset( m_pDspInf[pos].cKizukenNo ,0x00 , sizeof(m_pDspInf[pos].cKizukenNo));
				} else if( (&m_pTestCoilInf[nRec + 1])->cKizukenNo[0] == 0x00 ){
					// １つ前のコイル情報は存在しない
					m_pDspInf[pos].nDistance = 0;
					m_pDspInf[pos].nRec = 0;
					memset( m_pDspInf[pos].cKizukenNo ,0x00 , sizeof(m_pDspInf[pos].cKizukenNo));
				} else {
					// １つ前のコイル情報が存在する
					from = m_pDspInf[pos].nDistance;
					to   = (&m_pTestCoilInf[nRec + 1])->nLen + n;

					// from～to で 行列情報・行情報送信
					MakeDefect(nRec+1, men, from, to);

					m_pDspInf[pos].nDistance = to;
					m_pDspInf[pos].nRec = (nRec + 1)+1;		
					memcpy( m_pDspInf[pos].cKizukenNo , (&m_pTestCoilInf[nRec + 1])->cKizukenNo , sizeof(m_pDspInf[pos].cKizukenNo));
				}
				break;
			}
			if( memcmp( m_pDspInf[pos].cKizukenNo , (&m_pTestCoilInf[nRec])->cKizukenNo, sizeof(m_pDspInf[pos].cKizukenNo) ) != 0 ){
				// カメラ位置で新しいコイルに切り替わったので、先に１つ前のコイルの処理を行う。
				if (MAX_COIL_NUM == nRec+1) {
					// １つ前のコイル情報は存在しない
				} else if( (&m_pTestCoilInf[nRec + 1])->cKizukenNo[0] == 0x00 ){
					// １つ前のコイル情報は存在しない
				} else {
					// １つ前のコイルの from～to で行列情報・行情報送信
					from = m_pDspInf[pos].nDistance;
					to   = (&m_pTestCoilInf[nRec + 1])->nLen;
					MakeDefect(nRec+1, men, from , to);
					if( DIV_LEN_POS_KEN_TOP == pos ){
						// 検査基点(上流)＝基準面を通過したことになるので、コイル実績を送信する
						LOG(em_INF),"[%s] コイル実績送信[%s]", my_sThreadName,(&m_pTestCoilInf[nRec+1])->cKizukenNo);
						SendCoilResult(nRec+1);
					}
				}
				m_pDspInf[pos].nDistance = 0;
				m_pDspInf[pos].nRec = 0;
				memset( m_pDspInf[pos].cKizukenNo ,0x00 , sizeof(m_pDspInf[pos].cKizukenNo));
				//if (pos == DIV_LEN_POS_KEN_BOT) {
				//	// 表示基点(検査基点(下流))の切り替わりなので、コイル情報(表示)を送信
				//	LOG(em_INF),"[%s] 表示用コイル情報送信[%s]", my_sThreadName,(&m_pTestCoilInf[nRec])->cKizukenNo);
				//	SendCoilInfo(nRec,1);
				//}
			}
			// from～to で行列情報・行情報送信
			from = m_pDspInf[pos].nDistance;
			to   = n;
			MakeDefect(nRec, men, from, to);
			m_pDspInf[pos].nDistance = n;
			m_pDspInf[pos].nRec = nRec+1;
			memcpy( m_pDspInf[pos].cKizukenNo , (&m_pTestCoilInf[nRec])->cKizukenNo , sizeof(m_pDspInf[pos].cKizukenNo));
			break;

		case DIV_LEN_POS_DSP_TOP:						// 表示位置
		case DIV_LEN_POS_DSP_BOT:						// 表示位置
			char cKizukenNo[SIZE_KIZUKEN_NO];
			memcpy(cKizukenNo, m_pDspInf[pos].cKizukenNo, sizeof(cKizukenNo));
			m_pDspInf[pos].nDistance = 0;
			m_pDspInf[pos].nRec = 0;
			memset(m_pDspInf[pos].cKizukenNo, 0x00, sizeof(m_pDspInf[pos].cKizukenNo));
			n = distance[pos];
			for (int i = 0; (nRec + i) < MAX_COIL_NUM; i++) {
				if ((&m_pTestCoilInf[nRec + i])->cKizukenNo[0] == 0x00) {
					// コイル情報は存在しない
					break;
				}
				if (n >= (&m_pTestCoilInf[nRec + i])->nLen) {
					n -= (&m_pTestCoilInf[nRec + i])->nLen;
					continue;
				}
				if (memcmp(cKizukenNo, (&m_pTestCoilInf[nRec + i])->cKizukenNo, sizeof(cKizukenNo))) {
					if (pos == DIV_LEN_POS_DSP_TOP) {
						// 表示基点の切り替わりなので、コイル情報(表示)を送信
						LOG(em_INF), "[%s] 表示用コイル情報送信[%s]", my_sThreadName, (&m_pTestCoilInf[nRec + i])->cKizukenNo);
						SendCoilInfo(nRec + i, 1);
					}
				}
				// １つ前のコイル情報が存在する
				m_pDspInf[pos].nDistance = (&m_pTestCoilInf[nRec + i])->nLen - n;
				m_pDspInf[pos].nRec = (nRec + i) + 1;
				memcpy(m_pDspInf[pos].cKizukenNo, (&m_pTestCoilInf[nRec + i])->cKizukenNo, sizeof(m_pDspInf[pos].cKizukenNo));
				break;
			}
			break;

		case DIV_LEN_POS_SHR_I:						// シャーカット位置
			memcpy(cKizukenNo, m_pDspInf[pos].cKizukenNo, sizeof(cKizukenNo));
			m_pDspInf[pos].nDistance = 0;
			m_pDspInf[pos].nRec = 0;
			memset( m_pDspInf[pos].cKizukenNo ,0x00 , sizeof(m_pDspInf[pos].cKizukenNo));
			n = distance[pos];
			for (int i=0; (nRec+i)<MAX_COIL_NUM; i++) {
				if( (&m_pTestCoilInf[nRec + i])->cKizukenNo[0] == 0x00 ){
					// コイル情報は存在しない
					break;
				}
				if (n >= (&m_pTestCoilInf[nRec + i])->nLen) {
					n -= (&m_pTestCoilInf[nRec + i])->nLen;
					continue;
				}
				if (true == m_bYousetuCut) {

					if (memcmp(cKizukenNo, (&m_pTestCoilInf[nRec + i])->cKizukenNo, sizeof(cKizukenNo))) {
						if (MAX_COIL_NUM == nRec + 1) {
							// １つ前のコイル情報は存在しない
						}
						else if ((&m_pTestCoilInf[nRec + 1])->cKizukenNo[0] == 0x00) {
							// １つ前のコイル情報は存在しない
						}
						else {
							SendCutInfo((&m_pTestCoilInf[nRec + 1])->cKizukenNo, (&m_pTestCoilInf[nRec + 1])->nLen - 500, DIV_CUT_WPD_OTHER);
							//LOG(em_INF), "[%s] シャーカット通知[%s] nLen=%d", my_sThreadName, (&m_pTestCoilInf[nRec + 1])->cKizukenNo, (&m_pTestCoilInf[nRec + 1])->nLen - 500);
						}
					}
				}
				m_pDspInf[pos].nDistance = (&m_pTestCoilInf[nRec + i])->nLen-n;
				m_pDspInf[pos].nRec = (nRec+i)+1;
				memcpy( m_pDspInf[pos].cKizukenNo , (&m_pTestCoilInf[nRec + i])->cKizukenNo , sizeof(m_pDspInf[pos].cKizukenNo));
				break;
			}
		}
	}

}

// 定周期タイマー受信処理
void TestManager::TimerEvent( void )
{
	// タイマーの周期(0.5秒)と現在速度から距離の増分を求める
	// m_nSpeed (mpm)
	// nDiff : 1周期(0.5秒)あたりの通板距離[mm]
	static int nOldDiff = 0;
	int nDiff = (m_nSpeed * 1000 / 60 / 2) ;

	if( nDiff != nOldDiff ) {
		LOG(em_INF),"[%s] Speed[%d]mpm 1周期に移動する距離[%d]mm ", my_sThreadName,m_nSpeed ,nDiff);
		nOldDiff = nDiff;
	}

	// 初期状態の場合はコイル情報を追加する
	if( (&m_pTestCoilInf[0])->cKizukenNo[0] == 0x00 ) AddNewCoil();

	for(int nRec=MAX_COIL_NUM-1;nRec>=0;nRec--){
		TEST_COIL_INF *pTestCoilInf = &m_pTestCoilInf[nRec];
		// コイル情報を持っているか
		if( pTestCoilInf->cKizukenNo[0] == 0x00 ) continue;
		// 検査基点WPDを通過済み		
		if(pTestCoilInf->nLen == pTestCoilInf->nCoilLen) continue;

		// 現在検査装置通板中のコイルとなる
		m_nRecNo = nRec;

		//if (0 == pTestCoilInf->nLen) {
		//	LOG(em_INF), "[%s] 検査用コイル情報送信[%s]", my_sThreadName, (&m_pTestCoilInf[0])->cKizukenNo);
		//	SendCoilInfo(nRec, 0);
		//}
		// 現在速度とタイマー周期から１周期辺りの距離を通板距離に加算する
		pTestCoilInf->nLen += nDiff;

		if(pTestCoilInf->nLen >= pTestCoilInf->nCoilLen){
			// 通板距離がコイル長を超えた＝検査基点WPDを通過(WPDがONしたタイミング)
			// 通板距離はコイル長をセットする。
			pTestCoilInf->nLen = pTestCoilInf->nCoilLen;
		}

		// 各検査台の位置情報を更新する
		UpdateDispInf(nRec);

		// 6.通板位置情報(検査装置位置、表示基点位置、各表示コイル通板位置情報)
		if (true == m_bSendPos) {
			SendPosInfo(nRec);
		}

		// 通板距離が残り30m未満で 未検査コイル情報が１つも無い状態の場合は、新規コイルを追加する
		if( 0 == nRec && (pTestCoilInf->nLen + 30000 > pTestCoilInf->nCoilLen )) AddNewCoil();

		// for loopを抜ける
		break;
	}
}

// HY_DEFECTに初期化通知を送る
void TestManager::Send_Hy_Init()
{
	COMMON_QUE  que;
	memset(&que, 0x00, sizeof(que));
	que.nLineNo = getlineid();

	// HY_DEFECT
	que.nEventNo = FACT_HY_DEFECT_02;	
	send_mail(HY_DEFECT,".", &que);

	// ステータスを送信する
	SendStatusInfo();
}

// HY_DEFECTにパラメータ変更通知を送る
void TestManager::Send_Hy_Param()
{
	COMMON_QUE  que;
	memset(&que, 0x00, sizeof(que));
	que.nLineNo = getlineid();

	// HY_DEFECT
	que.nEventNo = FACT_HY_DEFECT_01;	
	send_mail(HY_DEFECT,".", &que);

}

// 前工程管理スレッドに登録要求を送信する
void TestManager::SendMaeInf(int nRec)
{
	char cCoilKey[SIZE_COIL_KEY + 1];

	// CoilKeyセット
	memset(cCoilKey, 0x00, sizeof(cCoilKey));
	memcpy(cCoilKey, (&m_pTestCoilInf[nRec])->cMeiNo, SIZE_MEI_NO);

	// 疵情報送信要求
	TestMaeManager::DELIVERY_KEY* pQue = new TestMaeManager::DELIVERY_KEY;
	memset(pQue, 0x00, sizeof(TestMaeManager::DELIVERY_KEY));

	memcpy(pQue->cKizukenNo, (&m_pTestCoilInf[nRec])->cKizukenNo, SIZE_KIZUKEN_NO);
	memcpy(pQue->cKeyInf, cCoilKey, SIZE_COIL_KEY);
	memcpy(pQue->cMeiNo, (&m_pTestCoilInf[nRec])->cMeiNo, SIZE_MEI_NO);
	memcpy(pQue->cCoilNo, (&m_pTestCoilInf[nRec])->cCoilNo, SIZE_COIL_NO);
	pQue->nCoilLen = (&m_pTestCoilInf[nRec])->nCoilLen;
	pQue->dNobiSPM = (&m_pTestCoilInf[nRec])->dNobiSPM;

	//LOG(em_MSG),"[%s] 送信疵決定(%d) <%s> men[%d] Seq[%d] y[%d] x[%d] DS[%f] WS[%f] TID[%d] GID[%d]", my_sThreadName,jj,cKizukenNo,nMen,pQue->nKizuSeq , nY,nX,nDS,nWS , pQue->nTid , pQue->nGid );

	mcls_pMaeMgr->gque_Deli.AddToTail(pQue);

}

//イベント受信処理
void TestManager::Exec( int id , DELIVERY_KEY* pDeli )
{
	static int initFlg = 1;
	static int pauseFlg = 1;

	if(EM_PLG_STOP == id){		// PLG停止
		LOG(em_MSG), "[%s] 一時停止", my_sThreadName);
		pauseFlg = 1;
	}

	if(EM_PLG_START == id){		// PLG起動
		LOG(em_MSG), "[%s] 一時停止解除", my_sThreadName);
		pauseFlg = 0;
	}

	// HY_DEFECTと接続を開始したら処理を行う。切れている間はとりあえず更新を行わない
	if( mcls_pOpSend->IsConnect(0) == false ) return;


	if( initFlg == 1 ) {
		// 初めての接続後処理を行う前に初期化通知を送信する
		Send_Hy_Init();
		initFlg = 0;
	}

	switch(id){
		case EM_TIMER:			// タイマー受信
			if( 0 == pauseFlg ) {
				// 一時停止中で無ければタイマー処理を行う	
				TimerEvent();
			}
			break;

		case EM_COIL_ADD:		// コイル情報追加
			AddNewCoil();
			break;

		case EM_PLG_STOP:		// PLG停止
			break;
		case EM_PLG_START:		// PLG起動
			break;

		case EM_PLG_SLOW:		// PLG１周期
			if( pauseFlg == 1 ) {
				//一時停止中に１周期進める
				TimerEvent();
			}
			break;

		case EM_SPEED_SETTING:	// スピード設定
			LOG(em_MSG), "[%s] 速度設定 [%d] mpm ", my_sThreadName, pDeli->speed );
			m_nSpeed = pDeli->speed ;
			break;

		case EM_INIT_NOTIFY:	// 初期化通知
			//// テスト用コイル情報初期化
			memset(m_pTestCoilInf, 0x00 ,sizeof(TEST_COIL_INF) * MAX_COIL_NUM);
			//// コイル情報初期化
			memset(m_pCoilInf, 0x00 ,sizeof(PRO_TYPE_DATA_COILINF) * MAX_COIL_NUM);
			//// コイルベース初期化
			memset(m_pCoilBase, 0x00 ,sizeof(COIL_BASEDATA) * MAX_COIL_NUM);
			//// 検査台情報初期化
			memset(m_pDspInf, 0x00 ,sizeof(DSP_INF) * DIV_LEN_POS_FINAL);
			// 検査台位置名称設定
			SetKensadaiName();
			// 初期化通知送信
			Send_Hy_Init();
			// 初期化済み
			m_bInitial = true;
			break;

		case EM_PARAM_NOTIFY:	// パラメータ変更通知
			// パラメータ変更通知送信
			Send_Hy_Param();
			break;

		case EM_STATUS_NOTIFY:	// 状態通知
			m_nUnten = pDeli->nUnten;
			m_nKadou = pDeli->nKadou;
			for (int nMen = 0; nMen<NUM_MEN; nMen++) {
				m_nKiki[nMen] = pDeli->nKiki[nMen];
				m_nKensa[nMen] = pDeli->nKensa[nMen];
			}
			// 状態更新
			SendStatusInfo();
			break;

		case EM_COIL_CUT:		// カット情報送信
			if( strlen(m_pDspInf[DIV_LEN_POS_SHR_I].cKizukenNo) != 0){
				SendCutInfo(m_pDspInf[DIV_LEN_POS_SHR_I].cKizukenNo, m_pDspInf[DIV_LEN_POS_SHR_I].nDistance);
			} else {
				LOG(em_MSG), "[%s] 現在シャーカット位置にコイルが無いので、カット情報は送信できません ", my_sThreadName );
			}
			break;
		case EM_DEFECT: // 疵情報 手動生成
			MakeDefectManual(pDeli->cKizukenNo, pDeli->nMen, pDeli->nY, pDeli->dDS, pDeli->Tid, pDeli->Gid, pDeli->dArea, pDeli->bNotChkDsWs);
			break;

		case EM_DEFLST_CANCEL: // 重欠陥疵情報削除要求
			SendDefLstCancel(pDeli->cKizukenNo, pDeli->nMen, pDeli->nCancelNo, pDeli->nSystem);
			break;
		case EM_ALARM: // 警報情報送信
			SendAlarm(pDeli);
			break;
		case EM_DEFECT_NUM:	// 状態通知
			SendDefCount(pDeli);
			break;
		default:
			break;
	}

}

// 検査台名称設定
void TestManager::SetKensadaiName( void )
{
	// 検査台位置名称設定
	sprintf(m_pDspInf[DIV_LEN_POS_KEN_TOP].Name, "カメラ位置_表");
	sprintf(m_pDspInf[DIV_LEN_POS_KEN_BOT].Name, "カメラ位置_裏");
	sprintf(m_pDspInf[DIV_LEN_POS_DSP_TOP].Name, "表示基点_表");
	sprintf(m_pDspInf[DIV_LEN_POS_DSP_BOT].Name, "表示基点_裏");
	sprintf(m_pDspInf[DIV_LEN_POS_SHR_I].Name ,  "シャーカット位置");
}



