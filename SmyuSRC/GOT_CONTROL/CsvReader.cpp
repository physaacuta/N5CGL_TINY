#include "StdAfx.h"
#include "CsvReader.h"

//------------------------------------------
// コンストラクタ
//------------------------------------------
CsvReader::CsvReader(void) :
ThreadManager("CsvReader")
{
}

//------------------------------------------
// デストラクタ
//------------------------------------------
CsvReader::~CsvReader(void)
{
}


//------------------------------------------
// シナリオ解析
// CString sPath CSVシナリオフルパス
// std::vector<TYP_SINA_INF*>* pEvent	シナリオリスト
//------------------------------------------
bool CsvReader::CreatEvents(CString sPath, std::vector<TYP_SINA_INF*>* pEvent)
{
	std::vector<CString>	listRow;
	
	//// CSVデータ取得
	if( ! ReadCsv(sPath, &listRow) ) return false;

	//// CSVデータからシナリオデータ生成
	if( ! CsvToEvent(&listRow, pEvent) ) return false;

	return true;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 処理


//------------------------------------------
// CSVファイル読み込み
// CString sPath CSVシナリオフルパス
// std::vector<CString>* pRow	1行単位の文字リスト
//------------------------------------------
bool CsvReader::ReadCsv(CString sPath, std::vector<CString>* pRow)
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
		while(true) {
			nWk ++;

			if( cWk[nWk] == ' ' || cWk[nWk] == '	' ) continue;
			if( cWk[nWk] == '#' || cWk[nWk] == 0x0a) break;

			bAddOk = true;
			break;
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
bool CsvReader::CsvToEvent(std::vector<CString>* pRow, std::vector<TYP_SINA_INF*>* pEvent)
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
bool CsvReader::GetColumn(CString& sVal, int& curPos, CString& sToken)
{
	CString sWk;
	sWk = sVal.Tokenize(",", curPos);
	sWk.Trim(_T(" 	"));					// 前後のスペース、タブを取り除く
	if( 0 == sWk.GetLength() ) return false;

	sToken = sWk;
	return true;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 条件発生部
//// 距離駆動シナリオ
void CsvReader::RaiseData_Plg_Pos(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
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
void CsvReader::RaiseData_Evt_Mail(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
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

void CsvReader::RaiseData_Evt_Po(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
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
void CsvReader::RaiseData_Time_Cycle(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
{
	CString sWk;					// 1列単位の文字列
	TYP_RAISE_TIM_CYCLE* pD = new TYP_RAISE_TIM_CYCLE;

	//// ヘッダー
	pInf->emRaise = DIV_KIND_MAIL;
	pInf->pRaise  = (BYTE*)pD;

	//----------------------------
	// 1回目
	GetColumn(sVal, curPos, sWk);
	pD->nFirst = atoi(sWk);

	// 2回目以降
	GetColumn(sVal, curPos, sWk);
	pD->nCycle = atoi(sWk);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// イベント部
void CsvReader::EventData_Plg(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
{
	CString sWk;					// 1列単位の文字列
	TYP_EVENT_PLG_SPEED* pD = new TYP_EVENT_PLG_SPEED;

	//// ヘッダー
	pInf->emEvent = DIV_KIND_PLG;
	pInf->pEvent  = (BYTE*)pD;

	//----------------------------
	// 対象チャンネル
	GetColumn(sVal, curPos, sWk);
	pD->nCh = atoi(sWk);

	// ライン速度
	GetColumn(sVal, curPos, sWk);
	pD->nMpm = atoi(sWk);
}

void CsvReader::EventData_Pi(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
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
void CsvReader::EventData_Mem(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
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

void CsvReader::EventData_Mail(CString& sVal, int& curPos, TYP_SINA_INF* pInf)
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

		GetColumn(sVal, curPos, sWk);
		strcpy_s(pD->que.fc.data, sizeof(pD->que.fc.data), sWk);

	} else if(0 == strcmp(sSubMode, "MIX")) {
		pD->emMainKind = DIV_MAIL_MIX;

		for(int ii=0; ii<5; ii++) {
			GetColumn(sVal, curPos, sWk);
			strcpy_s(pD->que.mix.cdata[ii], sizeof(pD->que.mix.cdata[ii]), sWk);
		}

		while( GetColumn(sVal, curPos, sWk) ) {
			pD->que.mix.idata[nCnt] = atoi(sWk);
			nCnt++;
		}

	}
}