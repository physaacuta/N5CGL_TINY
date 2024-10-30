#include "StdAfx.h"
#include "DataSetBaseManager.h"

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
DataSetBaseManager::DataSetBaseManager(void) :
mcls_pParam(NULL),
mcls_pFDele(NULL),
mque_pImgSave(NULL),
mque_pImgSaveLow(NULL),
ThreadManager("Mgr")
{
	//// シグナル
	m_evClearTbl  = CreateEvent(NULL, FALSE, FALSE, NULL);			// 共有メモリ初期化イベント

	// 初期化
	m_nMemSetCnt = 0;
	memset(m_MemInf, 0x00, sizeof(m_MemInf));
}

//------------------------------------------
// デストラクタ
//------------------------------------------
DataSetBaseManager::~DataSetBaseManager(void)
{
	//// シグナル開放
	CloseHandle(m_evClearTbl);
}


//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int DataSetBaseManager::ThreadFirst()
{
	//// 事前準備

	// 共有メモリオープン
	OnOpenTbl();
	// 共有メモリ 完全初期化
	OnClearTbl(true);

	//// イベント設定
	AddEventNum(1, &m_evClearTbl);
	AddEventNum(1, &mque_pOpRecv->g_evSem);

	//// イベント追加
	ThreadFirst_AddHandle();
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int DataSetBaseManager::ThreadLast()
{
	//// 共有メモリクローズ
	OnCloseTbl();
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void DataSetBaseManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_CLEAR_TBL = 0,				// 共有メモリ初期化イベント
			EV_QUR,							// 共有メモリ情報受信

			EV_LAST
	};


	////// シグナル条件分け
	//======================================================
	// 共有メモリ初期化イベント
	if(nEventNo == EV_CLEAR_TBL) {
		OnClearTbl(false); 


	//======================================================
	// 共有メモリ情報受信
	} else if(nEventNo == EV_QUR)	{

		TO_SO_HEAD* pHead;											// 受信データヘッダ情報
		pHead = (TO_SO_HEAD*) mque_pOpRecv->GetFromHead(); 

		OnSetTbl( (TO_SO_HEAD const*)pHead);

		QueBufFree(pHead);


	//======================================================
	// それ以外。継承先で追加したシグナル
	} else {
		//// 継承先に通知
		OnAddSignal( nEventNo - EV_LAST );
	}
}


//------------------------------------------
// 現バッファを開放する
// TO_SO_HEAD* pHead  伝文ヘッダ
//------------------------------------------
void DataSetBaseManager::QueBufFree(TO_SO_HEAD* pHead)
{
	if( NULL == pHead) return;
	
	// 開放
	SAFE_DELETE(pHead->base_head.addr[0]);
	SAFE_DELETE(pHead->base_head.addr[1]);
	SAFE_DELETE(pHead);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// その他もろもろ


//------------------------------------------
// 書き込みポインタ更新
// TO_SO_HEAD const* pHead 受信ヘッダ
// TBL_HY_POINT* pTblPoint ポインタ情報テーブル
//------------------------------------------
void DataSetBaseManager::SetData_PointWrite(TO_SO_HEAD const* pHead, TBL_HY_POINT* pTblPoint)
{
	int						rec;					// 0オリジン
	bool					bIsNewRec = false;		// 新規レコード時 true

	LOG(em_MSG),"[書込ポインタ更新処理]");

	//// レコード位置決定
	rec = CheckKanriNo(pHead->KizukenNo, pTblPoint);
	if(0 > rec) {
		// 最新 書き込みレコード
		bIsNewRec = true;
		rec = pTblPoint->nRecWriteBase;
		if(TBL_HY_REC_MAX <= rec) rec = 0;
	}

	//// 書き込みポインタ更新時の処理
	if( ! bIsNewRec) {
		LOG(em_WAR), "[ポインタ情報 <%d>] 管理No重複 管理No=%s", pTblPoint->nRecWriteBase, pHead->KizukenNo);
		return;
	}
	
	// ///////////////////////////////////////////////////////////////////
	// 初期化
	//
	// 継承先にコールバック
	UpDate_PintWrite(pHead, rec);


	// ///////////////////////////////////////////////////////////////////
	// データセット
	//
	// ポインタテーブル セット
	memset( &pTblPoint->pnt[rec], 0x00, sizeof(pTblPoint->pnt[rec]));
	// 管理No
	memcpy( pTblPoint->pnt[rec].cKizukenNo, pHead->KizukenNo, SIZE_KIZUKEN_NO);

	// 書込みレコード
	pTblPoint->nRecWriteBase = rec + 1;

	// 画面表示可否フラグを「表示予定」とする
	pTblPoint->pnt[rec].emDispStatus = DIV_DSP_STANDBY;

	LOG(em_INF), "==========================================================>>>>");
	LOG(em_MSG), "[ポインタ情報 <%d>] 書込ポインタ更新 管理No=%s", pTblPoint->nRecWriteBase, pHead->KizukenNo);
}

//------------------------------------------
// 表示ポインタ更新
// TO_SO_HEAD const* pHead 受信ヘッダ
// TBL_HY_POINT* pTblPoint ポインタ情報テーブル
// 戻り値 書き込みレコード位置 (0オリジン)
//------------------------------------------
void DataSetBaseManager::SetData_PointDisp(TO_SO_HEAD const* pHead, TBL_HY_POINT* pTblPoint)
{
	int						rec;					// 0オリジン

	//// レコード位置決定
	rec = CheckKanriNo(pHead->KizukenNo, pTblPoint);
	if(0 > rec) {
		LOG(em_ERR), "[ポインタ情報] 表示ポインタ該当無し 管理No=%s", pHead->KizukenNo);
		return;
	}

	//// 表示基点位置での現在表示ポインタを更新(1オリジン)
	pTblPoint->nRecDispBase = rec + 1;
	LOG(em_MSG), "[ポインタ情報 <%d>] 表示ポインタ更新 管理No=%s", pTblPoint->nRecDispBase, pHead->KizukenNo);


	// ///////////////////////////////////////////////////////////////////
	// その他処理
	//

	// 継承先にコールバック
	UpDate_PintDisp(pHead, rec);
		
	// TBL_HY_POINTの画面表示可否フラグを表示可とする
	pTblPoint->pnt[rec].emDispStatus = DIV_DSP_OK;

	// 過去のTBL_HY_POINTの画面表示可否フラグを走査し、表示予定のコイルは表示しないに更新する
	// 過去のレコードから参照するため、書き込みレコード値を取得
	int nRecIndex = rec;
	for (int ii = 0; ii < TBL_HY_REC_MAX - 1; ii++) {
		// 過去のレコードを走査するため-1する
		nRecIndex -= 1;
		if (0 > nRecIndex) {
			nRecIndex = TBL_HY_REC_MAX - 1;
		}
		// 表示可もしくはデータなしまで達した場合、走査終了
		if ( (DIV_DSP_OK == pTblPoint->pnt[nRecIndex].emDispStatus) 
			|| (DIV_DSP_INITIALIZE == pTblPoint->pnt[nRecIndex].emDispStatus)) {
			break;
		}
		// 表示用意の場合、表示不可に更新
		if (DIV_DSP_STANDBY == pTblPoint->pnt[nRecIndex].emDispStatus) {
			LOG(em_ERR), "[ポインタ情報] 表示不可コイル検出！ 管理No=%s, レコード=%d", pHead->KizukenNo, nRecIndex);
			pTblPoint->pnt[nRecIndex].emDispStatus = DIV_DSP_NG;
		}
	}

}

//------------------------------------------
// ポインターテーブルに保持している疵検管理Noと一致するかチェック
// char const* pKizukenNo 疵検管理No
// TBL_HY_POINT const* pTblPoint ポインタ情報テーブル
// 戻り値 -1:該当疵検管理No無し  0～:一致レコード位置(0オリジン)
//------------------------------------------
int DataSetBaseManager::CheckKanriNo(char const* pKizukenNo, TBL_HY_POINT const* pTblPoint)
{
	// 書き込みポインタチェック
	if(0 >= pTblPoint->nRecWriteBase) return -1;

	//// ポインターテーブルをチェック
	// 今の位置から前に遡って行く
	int rec = pTblPoint->nRecWriteBase -1;
	for(int ii=0; ii<TBL_HY_REC_MAX; ii++) {
		if(0 == memcmp(pTblPoint->pnt[rec].cKizukenNo, pKizukenNo, SIZE_KIZUKEN_NO ) ) return rec;

		rec -= 1;
		if( 0 > rec ) rec = TBL_HY_REC_MAX-1;
	}

	// ここまで来たら該当無し
	return -1;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// こまごました処理


//------------------------------------------
// レコード位置取得(テーブル)
// int rec 基準レコード (0オリジン)
// int index 0:現在位置 1:一個前 2:二個前 3:三個前 -1:次
// 戻り値:レコード位置 (0オリジン)
//------------------------------------------
int DataSetBaseManager::GetRecIndex(int rec, int index)
{
	if(0 == index) {
		// 現在位置
		return rec;			
	}
	// 指定前
	int recval = rec - index;
	if( 0 > recval) recval = TBL_HY_REC_MAX + recval;
	if( TBL_HY_REC_MAX <= recval) recval = recval - TBL_HY_REC_MAX;
	return recval;
}

//------------------------------------------
// フォルダパス生成
// CString sBasePath 生成ベースパス (最後の\はいらない)
// int rec レコード (0オリジン) -1:レコードは無し
//------------------------------------------
CString DataSetBaseManager::GetFolderPath(CString sBasePath, int rec)
{
	CString sWk;
	if(-1==rec) {
		sWk.Format("%s", sBasePath);
	} else {
		sWk.Format("%s\\rec%d", sBasePath, rec);
	}
	return sWk;
}

//------------------------------------------
// 画像格納フォルダ作成
// CString sPath 生成ベースパス (最後の\はいらない)
// int nRec サブフォルダ数 (0:レコード作らない)
//------------------------------------------
void DataSetBaseManager::CreateFolder(CString sPath, int nRec)
{
	// ベースフォルダ作成
	_mkdir(sPath);

	// サブフォルダ
	CString sWk;
	for(int ii=0; ii<nRec; ii++) {
		//sWk.Format("%s\\rec%d", sPath, ii);
		sWk = GetFolderPath(sPath, ii);
		_mkdir(sWk);
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 基本 (完全流用に入れても良いくらいの奴)

//------------------------------------------
// 基準レコードの行から、各ステージの行を算出
// int stage	ステージ (0オリジン)
// int nRow		基準レコード (0オリジン)
// 戻り値		対象行位置 (0オリジン)
//------------------------------------------
int DataSetBaseManager::GetStageIndex(int nStage, int nRowIndex)				
{
	int nIntVal   = mcls_pParam->GetCommonMast().nMapDsp[nStage];		// マップ間隔 [m]
	int nMaxIndex = GET_JUST_INDEX( COIL_LEN_MAX, nIntVal );			// 最大レコード位置

	int index = nRowIndex / ( nIntVal / MAP_REPRES_MIN );	//(0オリジン) 5,5のときは0
	
	// 整合性チェック
		// 既設からおかしい。 たぶん、最大コイル長が 6m で マップ間隔が(2,4,6)となっていると、最大値がおかしくなっているはず
	if(0 > index)			index = 0;
	if(nMaxIndex < index)	index = nMaxIndex;
	return index;
}
//------------------------------------------
// 指定距離から、各ステージの行位置を算出
// int nStage	ステージ (0オリジン)
// int nLen		指定距離 [m]
// 戻り値		対象行位置 (0オリジン)
//------------------------------------------
int DataSetBaseManager::GetStageLenToIndex(int nStage, int nLen)				
{
	int nIntVal   = mcls_pParam->GetCommonMast().nMapDsp[nStage];		// マップ間隔 [m]
	int nMaxIndex = GET_JUST_INDEX( COIL_LEN_MAX, nIntVal );			// 最大レコード位置

	int index = nLen / nIntVal;	//(0オリジン) 5,5のときは0

	// 整合性チェック
		// 既設からおかしい。 たぶん、最大コイル長が 6m で マップ間隔が(2,4,6)となっていると、最大値がおかしくなっているはず
	if(0 > index)			index = 0;
	if(nMaxIndex < index)	index = nMaxIndex;
	return index;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 共有メモリ操作
//------------------------------------------
// 使いやすいように共有メモリ登録
// HANDLE* h			ハンドル
// void** pTbl			テーブル
// int nSize			テーブルサイズ
// char const* cName	名称
// bool bNomalCrearCancel 初期化モード (True:通常初期化は実施しない)
//------------------------------------------
bool DataSetBaseManager::AddMemInf(HANDLE* h, void** pTbl, int nSize, char const* cName, bool bNomalCrearCancel)
{
	if( MAX_MEM_COUNT <= m_nMemSetCnt ) return false;

	// 登録
	m_MemInf[m_nMemSetCnt].h	= h;
	m_MemInf[m_nMemSetCnt].pTbl = pTbl;		
	m_MemInf[m_nMemSetCnt].nSize= nSize;
	m_MemInf[m_nMemSetCnt].bNomalCrearCancel = bNomalCrearCancel;
	strcpy(m_MemInf[m_nMemSetCnt].cName, cName);

	*m_MemInf[m_nMemSetCnt].pTbl = 0;		//cdcdcdはいやなので、初期化しておく
	m_nMemSetCnt++;
	return true;
}

//------------------------------------------
// 共有メモリオープン
//------------------------------------------
void DataSetBaseManager::OnOpenTbl()
{

	// どうも64bitOS 上で、32bitAP を動かして、共有メモリ確保すると、既にメモリをいっぱい使用している場合に、巨大な共有メモリを確保しようとしたら失敗する
	// GetLastError(); で 8 : ERROR_NOT_ENOUGH_MEMORY
	for(int ii=0; ii<m_nMemSetCnt; ii++) {
		MEM_INF* pInf = &m_MemInf[ii];
		mem_connect(pInf->h, (LPVOID *)pInf->pTbl, pInf->cName);

		if(NULL == pInf->h || NULL == *pInf->pTbl) {
			int nErr = GetLastError();

			syslog(SYSNO_MEM_OPEN, "[%s:Err=%d]", pInf->cName, nErr);
			LOG(em_ERR), "[%s] 共有メモリ 接続失敗 (%d:%s=%dbyte:Err=%d)", my_sThreadName, ii, pInf->cName, pInf->nSize, nErr);
			_ASSERT(FALSE);
			continue;
		}
		LOG(em_MSG), "[%s] 共有メモリ 接続完了 (%d:%s=%dbyte)", my_sThreadName, ii, pInf->cName, pInf->nSize);
	}

}

//------------------------------------------
// 共有メモリクローズ
//------------------------------------------
void DataSetBaseManager::OnCloseTbl()
{
	// 共有メモリを開放する
	for(int ii=0; ii<m_nMemSetCnt; ii++) {
		MEM_INF* pInf = &m_MemInf[ii];

		mem_close(pInf->h, (LPVOID *)pInf->pTbl);
	}

	LOG(em_MSG), "[%s] 共有メモリ 切断完了", my_sThreadName);
}

//------------------------------------------
// 共有メモリ初期化
// bMode 初期化モード True:全てのテーブルクリア False:普通の初期化
//------------------------------------------
void DataSetBaseManager::OnClearTbl(bool bMode)
{
	LOG(em_MSG), "[%s] 共有メモリ 初期化 モード=%d(0:通常初期化 1:完全初期化)", my_sThreadName, bMode);

	// 初期化
	for(int ii=0; ii<m_nMemSetCnt; ii++) {
		MEM_INF* pInf = &m_MemInf[ii];
		
		if( ! bMode && pInf->bNomalCrearCancel ) continue;

_RPT4(_CRT_WARN, "3-%d[%s] (%d)(%p)\n", ii, pInf->cName, pInf->nSize, *pInf->pTbl);
		memset(*pInf->pTbl,	0x00, pInf->nSize);
	}
}

//------------------------------------------
// 全共有メモリ バイナリ出力
//------------------------------------------
void DataSetBaseManager::OnOutputTbl()
{
	std::ofstream os;
	CString sWk;

	// 出力
	for(int ii=0; ii<m_nMemSetCnt; ii++) {
		MEM_INF* pInf = &m_MemInf[ii];
		
		sWk.Format(".\\%s.bin", pInf->cName);
		os.open(sWk, std::ios::out | std::ios::binary);
		os.write((char const*)*pInf->pTbl, pInf->nSize );
		os.close();

	}

	LOG(em_MSG), "[%s] 全共有メモリ デバック出力完了", my_sThreadName);
}

//------------------------------------------
// 全共有メモリ バイナリ読込
//------------------------------------------
void DataSetBaseManager::OnInputTbl()
{
	std::ifstream is;
	CString sWk;

	for(int ii=0; ii<m_nMemSetCnt; ii++) {
		MEM_INF* pInf = &m_MemInf[ii];

		sWk.Format(".\\%s.bin", pInf->cName);
		is.open(sWk, std::ios::in | std::ios::binary);
		is.read((char *)*pInf->pTbl, pInf->nSize  );
		is.close();
	}
	LOG(em_MSG), "[%s] 全共有メモリ デバック読込完了", my_sThreadName);
}

