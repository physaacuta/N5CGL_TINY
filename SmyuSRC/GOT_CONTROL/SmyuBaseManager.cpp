#include "StdAfx.h"
#include "SmyuBaseManager.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


CString CreatThreadName(const char* pThreadName, int id) { CString s; s.Format("%s_%d", pThreadName, id); return s;}

//------------------------------------------
// コンストラクタ
//------------------------------------------
SmyuBaseManager::SmyuBaseManager(const char* pThreadName, int nMyID) :
m_nMyID(nMyID),
ThreadManager(CreatThreadName(pThreadName, nMyID))
{
	
}

//------------------------------------------
// デストラクタ
//------------------------------------------
SmyuBaseManager::~SmyuBaseManager(void)
{
	// 開放
	for(DWORD ii=0; ii<m_lstStory.size(); ii++) {
		SAFE_DELETE( m_lstStory[ii]->pRaise );
		SAFE_DELETE( m_lstStory[ii]->pEvent );
		SAFE_DELETE( m_lstStory[ii] );
	}
}


//------------------------------------------
// シナリオ読み込み
// LPCSTR sPath CSVファイルパス
//------------------------------------------
bool SmyuBaseManager::SetCsvFilePath(LPCSTR sPath)
{
	std::vector<CString>	listRow;
	
	LOG(em_MSG), "[%s] シナリオ読み込み開始 [%s]----->>>", my_sThreadName, sPath);

	//// CSVデータ取得
	if( ! ReadCsv(sPath, &listRow) ) return false;

	//// CSVデータからシナリオデータ生成
	if( ! CsvToEvent(&listRow, &m_lstStory) ) return false;

	LOG(em_MSG), "[%s] シナリオ読み込み完了", my_sThreadName);
	return true;
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// シナリオ生成部

//------------------------------------------
// CSVファイル読み込み
// CString LPCSTR CSVシナリオフルパス
// std::vector<CString>* pRow	1行単位の文字リスト
//------------------------------------------
bool SmyuBaseManager::ReadCsv(LPCSTR sPath, std::vector<CString>* pRow)
{
	char cWk[2048];
	int nWk;

	//================================================
	// オープン
	FILE* fp = fopen(sPath, "rt"); 
	if(NULL == fp) {
		LOG(em_ERR), "[%s] CSVファイル オープン失敗 [%s]", my_sThreadName, sPath);
		return false;
	}

	//================================================
	// 読み込み
	while (!feof(fp)) {
		char* ret = fgets(cWk, sizeof(cWk), fp);	// 1行取得
		if( 0 == ret ) break;

		//// 空白行, コメント行は 対象外
		bool bAddOk = false;
		nWk = -1;
		int nCnt = strlen(cWk);
		for(int ii=0; ii<nCnt; ii++) {
			nWk ++;

			if( cWk[nWk] == ' ' || cWk[nWk] == '	' ) continue;
			if( cWk[nWk] == '#' || cWk[nWk] == 0x0a) {
				// ここで文字列が終わりの認識にする
				cWk[nWk] = 0x00;
				break;
			}

			bAddOk = true;
		}

		//// 追加対象
		if(bAddOk) {
			CString s;
			s = cWk;
			s.Trim(_T(" 	"));
			s.Trim(_T(0x0a));
			pRow->push_back(s);
		}
	}

	//================================================
	// クローズ
	fclose(fp);
	return true;
}

//------------------------------------------
// CSV から イベントに生成
// std::vector<CString>* pRow	1行単位の文字リスト
// std::vector<CString>* pEvent	イベントデータに生成
//------------------------------------------
bool SmyuBaseManager::CsvToEvent(std::vector<CString>* pRow, std::vector<TYP_SINA_INF*>* pEvent)
{
	//// 1行づつのデータを、イベントデータに変換
	for(DWORD ii=0; ii<pRow->size(); ii++) {
		CString			sVal	= pRow->at(ii);		// 1行分の文字列
		CString			sWk;						// 1列単位の文字列
		int				curPos	= 0;				// 区切り位置
		TYP_SINA_INF*	inf		= NULL;				// シナリオ
		//================================================
		// 発生条件部  イベント種別特定
		
		if( GetColumn(sVal, curPos, sWk) ) {
			
			//// 距離駆動シナリオ
			if(			0 == strcmp(sWk, STR_KIND_PLG)) {
				inf	= new TYP_SINA_INF;
				RaiseData_Plg_Pos(sVal, curPos, inf);


			//// イベント駆動シナリオ
			} else if(	0 == strcmp(sWk, STR_KIND_MAIL)) {
				inf	= new TYP_SINA_INF;
				RaiseData_Evt_Mail(sVal, curPos, inf);
			
			} else if(	0 == strcmp(sWk, STR_KIND_PO)) {
				inf	= new TYP_SINA_INF;
				RaiseData_Evt_Po(sVal, curPos, inf);
			

			//// イベント駆動シナリオ
			} else if(	0 == strcmp(sWk, STR_KIND_TIME)) {
				inf	= new TYP_SINA_INF;
				RaiseData_Time_Cycle(sVal, curPos, inf);


			} else {
				LOG(em_ERR), "[%s] 発生条件部 イベント種別異常 [%s]", my_sThreadName, sVal);
				break;
			}
		} else {
			LOG(em_ERR), "[%s] 発生条件部 イベント種別無し [%s]", my_sThreadName, sVal);
			SAFE_DELETE(inf);
		}

		//================================================
		// イベント部  イベント種別特定
		if( GetColumn(sVal, curPos, sWk) ) {
			if(			0 == strcmp(sWk, STR_KIND_PLG)) {
				EventData_Plg(sVal, curPos, inf);
			
			} else if(	0 == strcmp(sWk, STR_KIND_PI)) {
				EventData_Pi(sVal, curPos, inf);

			} else if(	0 == strcmp(sWk, STR_KIND_MEM)) {
				EventData_Mem(sVal, curPos, inf);

			} else if(	0 == strcmp(sWk, STR_KIND_MAIL)) {
				EventData_Mail(sVal, curPos, inf);
			
			} else if(	0 == strcmp(sWk, STR_KIND_TIME)) {
				EventData_Time(sVal, curPos, inf);

			} else if(	0 == strcmp(sWk, STR_KIND_ZZ)) {
				EventData_Zz(sVal, curPos, inf);

			} else {
				LOG(em_ERR), "[%s] イベント部 イベント種別異常 [%s]", my_sThreadName, sVal);
				SAFE_DELETE(inf->pRaise);
				SAFE_DELETE(inf);
				break;
			}
		} else {
			LOG(em_ERR), "[%s] イベント部 イベント種別無し [%s]", my_sThreadName, sVal);
			SAFE_DELETE(inf->pRaise);
			SAFE_DELETE(inf);
		}


		//================================================
		// 登録
		pEvent->push_back(inf);
	}
	return true;
}

//------------------------------------------
// １行のデータから , 単位の文字列を取得
// CString& sVal 1行単位の情報
// int& curPos 区切り位置
// CString& sToken 取得した列の文字列
// 戻り値 復帰情報
//------------------------------------------
bool SmyuBaseManager::GetColumn(CString& sVal, int& curPos, CString& sToken)
{
	CString sWk;
	sWk = sVal.Tokenize(",", curPos);
	sWk.Trim(_T(" 	"));					// 前後のスペース、タブを取り除く
	if( 0 == sWk.GetLength() ) return false;

	sToken = sWk;
	return true;
}


// ##########################################################################
// 条件発生部
//// 距離駆動シナリオ
void SmyuBaseManager::RaiseData_Plg_Pos(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
{
	CString sWk;					// 1列単位の文字列
	TYP_RAISE_PLG_POS* pD = new TYP_RAISE_PLG_POS;

	//// ヘッダー
	pInf->emRaise = DIV_KIND_PLG;
	pInf->pRaise  = (BYTE*)pD;


	//----------------------------
	// 距離参照チャンネル
	GetColumn(sVal, curPos, sWk);
	pD->nCh = atoi(sWk);

	// 事象発生距離 [mm]
	GetColumn(sVal, curPos, sWk);
	pD->nLen = atoi(sWk);
}

//// イベント駆動シナリオ
void SmyuBaseManager::RaiseData_Evt_Mail(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
{
	CString sWk;					// 1列単位の文字列
	TYP_RAISE_EVT_MAIL* pD = new TYP_RAISE_EVT_MAIL;

	//// ヘッダー
	pInf->emRaise = DIV_KIND_MAIL;
	pInf->pRaise  = (BYTE*)pD;

	//----------------------------
	// タスク名称
	GetColumn(sVal, curPos, sWk);
	strcpy_s(pD->cTaskName, sizeof(pD->cTaskName), sWk);

	// イベントNo
	GetColumn(sVal, curPos, sWk);
	pD->nEventNo = atoi(sWk);
}

void SmyuBaseManager::RaiseData_Evt_Po(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
{
	CString sWk;					// 1列単位の文字列
	TYP_RAISE_EVT_PO* pD = new TYP_RAISE_EVT_PO;

	//// ヘッダー
	pInf->emRaise = DIV_KIND_PO;
	pInf->pRaise  = (BYTE*)pD;

	//----------------------------
	// ボードNo
	GetColumn(sVal, curPos, sWk);
	pD->nBood = atoi(sWk);

	// PortNo
	GetColumn(sVal, curPos, sWk);
	pD->nPort = atoi(sWk);

	// BitNo
	GetColumn(sVal, curPos, sWk);
	pD->nBit = atoi(sWk);

	// 認識タイミング
	GetColumn(sVal, curPos, sWk);
	pD->nMode = atoi(sWk);
}

//// 時間駆動シナリオ
void SmyuBaseManager::RaiseData_Time_Cycle(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
{
	CString sWk;					// 1列単位の文字列
	TYP_RAISE_TIM_CYCLE* pD = new TYP_RAISE_TIM_CYCLE;

	//// ヘッダー
	pInf->emRaise = DIV_KIND_TIME;
	pInf->pRaise  = (BYTE*)pD;

	//----------------------------
	// 配列位置
	GetColumn(sVal, curPos, sWk);
	pD->nID = atoi(sWk);

	// 1回目
	GetColumn(sVal, curPos, sWk);
	pD->nFirst = atoi(sWk);

	// 2回目以降
	GetColumn(sVal, curPos, sWk);
	pD->nCycle = atoi(sWk);
}

// ##########################################################################
// イベント部
void SmyuBaseManager::EventData_Plg(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
{
	CString sWk;					// 1列単位の文字列

	//----------------------------
	// サブ種別の特定
	GetColumn(sVal, curPos, sWk);
	int nSubKind = atoi(sWk);

	// サブ種別特定
	if(0 == nSubKind) {
		pInf->emEvent	= DIV_KIND_PLG;
		pInf->nEventSub = nSubKind;
		TYP_EVENT_PLG_SPEED* pD = new TYP_EVENT_PLG_SPEED;
		pInf->pEvent    = (BYTE*)pD;

		//----------------------------
		// 対象チャンネル
		GetColumn(sVal, curPos, sWk);
		pD->nCh = atoi(sWk);

		// ライン速度
		GetColumn(sVal, curPos, sWk);
		pD->nMpm = atoi(sWk);

	} else if(1 == nSubKind) {
		pInf->emEvent	= DIV_KIND_PLG;
		pInf->nEventSub = nSubKind;
		TYP_EVENT_PLG_FRAMECYCLE* pD = new TYP_EVENT_PLG_FRAMECYCLE;
		pInf->pEvent    = (BYTE*)pD;

		//----------------------------
		// ライン速度
		GetColumn(sVal, curPos, sWk);
		pD->nCycle = atoi(sWk);

	}
}

void SmyuBaseManager::EventData_Pi(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
{
	CString sWk;					// 1列単位の文字列

	//----------------------------
	// サブ種別の特定
	GetColumn(sVal, curPos, sWk);
	int nSubKind = atoi(sWk);


	// サブ種別特定
	if(0 == nSubKind) {
		pInf->emEvent   = DIV_KIND_PI;
		pInf->nEventSub = nSubKind;
		TYP_EVENT_PI_BIT* pD = new TYP_EVENT_PI_BIT;
		pInf->pEvent    = (BYTE*)pD;


		//----------------------------
		// ボードNo
		GetColumn(sVal, curPos, sWk);
		pD->nBood = atoi(sWk);

		// PortNo
		GetColumn(sVal, curPos, sWk);
		pD->nPort = atoi(sWk);

		// BitNo
		GetColumn(sVal, curPos, sWk);
		pD->nBit = atoi(sWk);

		// セット値
		GetColumn(sVal, curPos, sWk);
		pD->nMode = atoi(sWk);


	} else if(1 == nSubKind) {
		pInf->emEvent   = DIV_KIND_PI;
		pInf->nEventSub = nSubKind;
		TYP_EVENT_PI_PORT* pD = new TYP_EVENT_PI_PORT;
		pInf->pEvent  = (BYTE*)pD;


		//----------------------------
		// ボードNo
		GetColumn(sVal, curPos, sWk);
		pD->nBood = atoi(sWk);

		// PortNo
		GetColumn(sVal, curPos, sWk);
		pD->nPort = atoi(sWk);

		// セット値
		GetColumn(sVal, curPos, sWk);
		pD->nBitSet = (BYTE) atoi(sWk);


	} else if(2 == nSubKind) {
		pInf->emEvent   = DIV_KIND_PI;
		pInf->nEventSub = nSubKind;
		TYP_EVENT_PI_BITPULS* pD = new TYP_EVENT_PI_BITPULS;
		pInf->pEvent    = (BYTE*)pD;


		//----------------------------
		// ボードNo
		GetColumn(sVal, curPos, sWk);
		pD->nBood = atoi(sWk);

		// PortNo
		GetColumn(sVal, curPos, sWk);
		pD->nPort = atoi(sWk);

		// BitNo
		GetColumn(sVal, curPos, sWk);
		pD->nBit = atoi(sWk);

		// セット値
		GetColumn(sVal, curPos, sWk);
		pD->nMode = atoi(sWk);

		// ON時間
		GetColumn(sVal, curPos, sWk);
		pD->nTime = atoi(sWk);

	} else {
		pInf->emEvent = DIV_KIND_NON;
		LOG(em_ERR), "[%s] PI サブ種別異常 [%s]", my_sThreadName, sVal);
		return;
	}
}
void SmyuBaseManager::EventData_Mem(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
{
	CString sWk;					// 1列単位の文字列
	TYP_EVENT_MEM_WRITE* pD = new TYP_EVENT_MEM_WRITE;

	//// ヘッダー
	pInf->emEvent = DIV_KIND_MEM;
	pInf->pEvent  = (BYTE*)pD;

	//----------------------------
	// 対象テーブル
	GetColumn(sVal, curPos, sWk);
	strcpy_s(pD->cTblName, sizeof(pD->cTblName), sWk);

	// 書き込み開始位置
	GetColumn(sVal, curPos, sWk);
	pD->nIndex = atoi(sWk);

	// バイナリデータ相対パス
	GetColumn(sVal, curPos, sWk);
	strcpy_s(pD->cWriteDataPath, sizeof(pD->cWriteDataPath), sWk);
}

void SmyuBaseManager::EventData_Mail(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
{
	CString sWk;					// 1列単位の文字列
	TYP_EVENT_MAIL_SEND* pD = new TYP_EVENT_MAIL_SEND;

	//// ヘッダー
	pInf->emEvent = DIV_KIND_MAIL;
	pInf->pEvent  = (BYTE*)pD;

	//----------------------------
	// 送信PC
	GetColumn(sVal, curPos, sWk);
	strcpy_s(pD->cPcName, sizeof(pD->cPcName), sWk);

	// 送信タスク
	GetColumn(sVal, curPos, sWk);
	strcpy_s(pD->cTaskName, sizeof(pD->cTaskName), sWk);

	// イベントNo
	GetColumn(sVal, curPos, sWk);
	pD->que.nEventNo = atoi(sWk);


	//----------------------------
	// サブ種別の特定
	int nCnt =0;
	CString sSubMode;		
	GetColumn(sVal, curPos, sSubMode);
	

	// サブ種別特定
	if(0 == strcmp(sSubMode, "FL")) {
		pD->emMainKind = DIV_MAIL_FL;

		while( GetColumn(sVal, curPos, sWk) ) {
			pD->que.fl.data[nCnt] = atoi(sWk);
			nCnt++;
		}

	} else if(0 == strcmp(sSubMode, "FC")) {
		pD->emMainKind = DIV_MAIL_FC;

		sWk = "";
		GetColumn(sVal, curPos, sWk);
		strcpy_s(pD->que.fc.data, sizeof(pD->que.fc.data), sWk);

	} else if(0 == strcmp(sSubMode, "MIX")) {
		pD->emMainKind = DIV_MAIL_MIX;

		for(int ii=0; ii<5; ii++) {
			sWk = "";
			GetColumn(sVal, curPos, sWk);
			strcpy_s(pD->que.mix.cdata[ii], sizeof(pD->que.mix.cdata[ii]), sWk);
		}

		while( GetColumn(sVal, curPos, sWk) ) {
			pD->que.mix.idata[nCnt] = atoi(sWk);
			nCnt++;
		}

	}
}
void SmyuBaseManager::EventData_Time(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
{
	CString sWk;					// 1列単位の文字列
	TYP_EVENT_TIM_MODE* pD = new TYP_EVENT_TIM_MODE;

	//// ヘッダー
	pInf->emRaise = DIV_KIND_TIME;
	pInf->pRaise  = (BYTE*)pD;

	//----------------------------
	// 配列位置
	GetColumn(sVal, curPos, sWk);
	pD->nIndex = atoi(sWk);

	// 配列位置
	GetColumn(sVal, curPos, sWk);
	pD->nID = atoi(sWk);

	// 0:停止指示  1:開始指示
	GetColumn(sVal, curPos, sWk);
	pD->nMode = atoi(sWk);
}

void SmyuBaseManager::EventData_Zz(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
{
	CString sWk;					// 1列単位の文字列

	//----------------------------
	// サブ種別の特定
	GetColumn(sVal, curPos, sWk);
	int nSubKind = atoi(sWk);


	// サブ種別特定
	if(0 == nSubKind) {
		pInf->emEvent   = DIV_KIND_ZZ;
		pInf->nEventSub = nSubKind;
		TYP_EVENT_ZZ_RESET* pD = new TYP_EVENT_ZZ_RESET;
		pInf->pEvent    = (BYTE*)pD;

		//----------------------------
		// 距離参照チャンネル (0オリジン) 0～7
		GetColumn(sVal, curPos, sWk);
		pD->nCh = atoi(sWk);

		// シナリオ終了区分 (0:何もしない  1:最初に戻る)
		GetColumn(sVal, curPos, sWk);
		pD->nMode = atoi(sWk);
	} else {
		pInf->emEvent = DIV_KIND_NON;
		LOG(em_ERR), "[%s] ZZ サブ種別異常 [%s]", my_sThreadName, sVal);
		return;
	}
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// イベント発生

//------------------------------------------
// イベント発生
// EM_DIV_KIND emEvent イベント種別
// int nEventSub サブ種別
// BYTE* pEvent 構造体ポインタ
//------------------------------------------
void SmyuBaseManager::RaiseEvents(EM_DIV_KIND emEvent, int nEventSub, BYTE* pEvent)
{
	////// シナリオ条件分け
	switch (emEvent) {
//-----------------------------------------------------------------------------------------------
	case DIV_KIND_PLG:										
		if(0==nEventSub) {
			TYP_EVENT_PLG_SPEED* p = (TYP_EVENT_PLG_SPEED*)pEvent;
			
			if(0 == p->nMpm) {
				mcls_pPlg->SetTimer_SpeedCycle(p->nCh, false);
				LOG(em_INF), "[%s] シナリオ実行<速度変更> Ch=%d; 停止", my_sThreadName, p->nCh);
			} else {
				mcls_pPlg->SetSaveData().nSpeed[p->nCh] = p->nMpm;

				// 周期変更
				double dWk =  mcls_pPlg->SetSaveData().nSpeed[0] / 60.0 * 1000.0;
				dWk  = dWk / ( mcls_pPlg->GetSetting().dResY[0]*mcls_pPlg->GetSetting().nFrameSizeY);
				int nWk = (int)(1.0 / dWk * 1000.0);
				mcls_pPlg->SetSaveData().nFrameCycle = nWk;


				// 変更通知。 	
				mcls_pPlg->SetTimer_SpeedCycle(p->nCh, true);
				LOG(em_INF), "[%s] シナリオ実行<速度変更> Ch=%d; mpm=%d", my_sThreadName, p->nCh, p->nMpm);
			}
		} else if(1==nEventSub) {
			TYP_EVENT_PLG_FRAMECYCLE* p = (TYP_EVENT_PLG_FRAMECYCLE*)pEvent;

			if(0 == p->nCycle) {
				// 停止だけは、速度を残しておく
				mcls_pPlg->SetTimer_SpeedCycle(0, false);
				LOG(em_INF), "[%s] シナリオ実行<ﾌﾚｰﾑ周期変更> 停止", my_sThreadName);
			} else {
				mcls_pPlg->SetSaveData().nFrameCycle = p->nCycle;
				// 変更通知。 (Trueを渡しておけば、関数内で勝手に停止開始を判断してくれる)			
				mcls_pPlg->SetTimer_SpeedCycle(0, true);
				LOG(em_INF), "[%s] シナリオ実行<ﾌﾚｰﾑ周期変更> speed=%d", my_sThreadName, p->nCycle);
			}
		}
		return;

//-----------------------------------------------------------------------------------------------
	case DIV_KIND_PI:										
		if(0==nEventSub) {
			TYP_EVENT_PI_BIT* p = (TYP_EVENT_PI_BIT*)pEvent;
			
			mcls_pPio->SetValPi( p->nBood, p->nPort, p->nBit, __BOOL p->nMode); 

			//LOG(em_INF), "[%s] シナリオ実行<PI BIT単位> Bood=%d; Port=%d; Bit=%d; set=%d", my_sThreadName, p->nBood, p->nPort, p->nBit, p->nMode);


		} else if(1==nEventSub) {
			TYP_EVENT_PI_PORT* p = (TYP_EVENT_PI_PORT*)pEvent;

			mcls_pPio->SetValPi( p->nBood, p->nPort, p->nBitSet); 

			LOG(em_INF), "[%s] シナリオ実行<PI PORT単位> Bood=%d; Port=%d; set=%02X", my_sThreadName, p->nBood, p->nPort, p->nBitSet );


		} else if(2==nEventSub) {
			TYP_EVENT_PI_BITPULS* p = (TYP_EVENT_PI_BITPULS*)pEvent;

			// △
		
		}
		return;

////-----------------------------------------------------------------------------------------------
//	case DIV_KIND_PO:										
//		return;

//-----------------------------------------------------------------------------------------------
	case DIV_KIND_MEM:										
		if(0==nEventSub) {
			TYP_EVENT_MEM_WRITE* p = (TYP_EVENT_MEM_WRITE*)pEvent;
			
			// 共有メモリアクセス
			HANDLE h;
			BYTE*  t = NULL;
			mem_connect(&h, (LPVOID *)&t, p->cTblName);
			if(h == NULL) {
				LOG(em_ERR), "[%s] 共有メモリ アクセス失敗 [%s]", my_sThreadName, p->cTblName);
				return;
			}

			// ファイルサイズを取得
			int nSize;
			nSize = KizuFunction::GetFileSize(p->cWriteDataPath);

			// バイナリファイルセット
			std::ifstream   is;
			is.open(p->cWriteDataPath, std::ios::binary);
			is.read( (char*)&t[p->nIndex], nSize);//1024*1024*1024);
			is.close();

			// 共有メモリ開放
			mem_close(&h, (LPVOID *)&t);

			LOG(em_INF), "[%s] シナリオ実行<共有メモリ書込> Name=%s; Start=%d; Path=%s;", my_sThreadName, p->cTblName, p->nIndex, p->cWriteDataPath );
		}
		return;

//-----------------------------------------------------------------------------------------------
	case DIV_KIND_MAIL:				
		if(0==nEventSub) {
			TYP_EVENT_MAIL_SEND* p = (TYP_EVENT_MAIL_SEND*)pEvent;

			send_mail(p->cTaskName, p->cPcName, &p->que);

			LOG(em_INF), "[%s] シナリオ実行<Mail送信> PC=%s; Task=%s; EvtNo=%d; Type=%d", my_sThreadName, p->cPcName, p->cTaskName, p->que.nEventNo, p->emMainKind);
			if( DIV_MAIL_FL == p->emMainKind) {
				LOG(em_INF), "   引数 L0=%d, L1=%d, L2=%d", p->que.fl.data[0], p->que.fl.data[1], p->que.fl.data[2]);

			} else if( DIV_MAIL_FC == p->emMainKind) {
				LOG(em_INF), "   引数 C=%s", p->que.fc.data);
			} else if( DIV_MAIL_MIX == p->emMainKind) {
				LOG(em_INF), "   引数 ML0=%d, ML1=%d, ML2=%d, C0=%s, C1=%s, C2=%s", p->que.mix.idata[0], p->que.mix.idata[1], p->que.mix.idata[2], p->que.mix.cdata[0], p->que.mix.cdata[1],  p->que.mix.cdata[2]);
			}
		}
		return;

//-----------------------------------------------------------------------------------------------
	case DIV_KIND_TIME:	
		if(true) {

		}
		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] 不明=%d", my_sThreadName, emEvent );
		return;
	}
}

