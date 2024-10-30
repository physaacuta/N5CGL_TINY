#include "StdAfx.h"
#include "DataSetManager.h"

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
DataSetManager::DataSetManager(void) :
DataSetBaseManager(),
m_bQueState(false)
{

}

//------------------------------------------
// デストラクタ
//------------------------------------------
DataSetManager::~DataSetManager(void)
{
}

//------------------------------------------
// スレッド準備
//------------------------------------------
void DataSetManager::Allock()
{
	//// iniファイル設定取得
	GetPrivateProfileString(HY_DEFECT, "IMAGE_PATH", "", m_cPath_Img, SIZE_IMAGE_PATH, TASKINI_NAME);
	_ASSERT(strlen(m_cPath_Img));

	GetPrivateProfileString(HY_DEFECT, "MAEIMG_PATH", "", m_cPath_Mae, SIZE_IMAGE_PATH, TASKINI_NAME);
	_ASSERT(strlen(m_cPath_Mae));

	GetPrivateProfileString(HY_DEFECT, "LSTIMG_PATH", "", m_cPath_Lst, SIZE_IMAGE_PATH, TASKINI_NAME);
	_ASSERT(strlen(m_cPath_Lst));

	m_nImgSaveOff = GetPrivateProfileInt(HY_DEFECT, "IMG_SAVE_OFF", 0, TASKINI_NAME);
	m_nMaeImgSaveOff = GetPrivateProfileInt(HY_DEFECT, "MAEIMG_SAVE_OFF", 0, TASKINI_NAME);
	m_nLstImgSaveOff = GetPrivateProfileInt(HY_DEFECT, "LSTIMG_SAVE_OFF", 0, TASKINI_NAME);

	if (1 == m_nImgSaveOff) {
		LOG(em_MSG), "[%s] 探傷画面表示用の画像保存無し <%d>", my_sThreadName, m_nImgSaveOff);
	}
	if (1 == m_nMaeImgSaveOff) {
		LOG(em_MSG), "[%s] 前工程疵画像保存無し <%d>", my_sThreadName, m_nMaeImgSaveOff);
	}
	if (1 == m_nLstImgSaveOff) {
		LOG(em_MSG), "[%s] 重欠陥疵画像保存無し <%d>", my_sThreadName, m_nLstImgSaveOff);
	}

	m_nLstSaveNo[0] = 0;
	m_nLstSaveNo[1] = 0;

	//// 表面検査台位置
	//m_nKensaNum[0] = GetPrivateProfileInt(HY_DEFECT, "KENSA_NUM_O", DIV_LEN_POS_KEN_TOP, TASKINI_NAME);
	//if (1 < NUM_MEN) {
	//	// 裏面検査台位置
	//	m_nKensaNum[1] = GetPrivateProfileInt(HY_DEFECT, "KENSA_NUM_U", DIV_LEN_POS_KEN_BOT, TASKINI_NAME);
	//}

	//// フォルダ生成
	this->CreateFolder(m_cPath_Img, TBL_HY_REC_MAX);

	////// 画像情報削除依頼
	CString sPath;
	//sPath = this->GetFolderPath(m_cPath_Img, 0);
	//mcls_pFDele->DeleteStart(sPath);

	FolderFunc  clsF;
	CString sWk;

	for (int ii = 0; ii < TBL_HY_REC_MAX; ii++) {
		sPath = this->GetFolderPath(m_cPath_Img, ii);
		mcls_pFDele->DeleteAll(sPath);
		LOG(em_MSG), "[%s] 疵画像削除要求 <%s>", my_sThreadName, sPath);
	}

	//// フォルダ生成
	this->CreateFolder(m_cPath_Mae, TBL_HY_REC_MAX);
	for (int ii = 0; ii < TBL_HY_REC_MAX; ii++) {
		sWk = "";
		sWk = GetFolderPath(m_cPath_Mae, ii);
		sWk += "\\*";
		if (clsF.DeleteFileAll(sWk)) {
			LOG(em_MSG), "[%s] 前工程画像削除完了 <%s>", my_sThreadName, sWk);
		}
	}

	//// フォルダ生成
	_mkdir(m_cPath_Lst);
	sWk = "";
	sWk = GetFolderPath(m_cPath_Lst, -1);
	sWk += "\\*";
	if (clsF.DeleteFileAll(sWk)) {
		LOG(em_MSG), "[%s] 重欠陥画像削除完了 <%s>", my_sThreadName, sWk);
	}

	//// 共有メモリ情報を登録
	this->AddMemInf(&m_hStatus,		(void**)&mtbl_pStatus,		sizeof(TBL_HY_STATUS),		TBL_HY_STATUS_NAME, true); 
	this->AddMemInf(&m_hPoint,		(void**)&mtbl_pPoint,		sizeof(TBL_HY_POINT),		TBL_HY_POINT_NAME); 
	this->AddMemInf(&m_hCoilInf,	(void**)&mtbl_pCoilInf,		sizeof(TBL_HY_COILINF),		TBL_HY_COILINF_NAME); 
	this->AddMemInf(&m_hCoilResult, (void**)&mtbl_pCoilResult,	sizeof(TBL_HY_COILRESULT),	TBL_HY_COILRESULT_NAME); 
	this->AddMemInf(&m_hMapRow,		(void**)&mtbl_pMapRow,		sizeof(TBL_HY_MAPROW),		TBL_HY_MAP_ROW_NAME); 
	this->AddMemInf(&m_hDefectInf,	(void**)&mtbl_pDefectInf,	sizeof(TBL_HY_DEFECTINF),	TBL_HY_DEFECTINF_NAME); 
	this->AddMemInf(&m_hAlarm,      (void**)&mtbl_pAlarm,       sizeof(TBL_HY_ALARM),       TBL_HY_ALARM_NAME);
	this->AddMemInf(&m_hMaeInf,		(void**)&mtbl_pMaeInf,		sizeof(TBL_HY_MAE_INF),		TBL_HY_MAE_INF_NAME);
	//this->AddMemInf(&m_hCycleInf,	(void**)&mtbl_pCycleInf,	sizeof(TBL_HY_CYCLE_INF),	TBL_HY_CYCLE_INF_NAME);
	this->AddMemInf(&m_hDefList,	(void**)&mtbl_pDefList,		sizeof(TBL_HY_DEFLIST),		TBL_HY_DEFLIST_NAME, true);
	// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	this->AddMemInf(&ml_hLongPd,	(void**)&mtbl_pLongPd,		sizeof(TBL_HY_LONGPD),		TBL_HY_LONGPD_NAME);
	// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

#ifdef LOCAL
//*/----> ◆自分で共有メモリを作る
	for(int ii=0; ii<m_nMemSetCnt; ii++) {
		CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, m_MemInf[ii].nSize, m_MemInf[ii].cName);

//HANDLE h = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, m_MemInf[ii].nSize, m_MemInf[ii].cName);
//void* pTbl =(LPVOID *) MapViewOfFile(h, FILE_MAP_ALL_ACCESS, 0, 0, 0);
//int a = GetLastError();
//
//_RPT4(_CRT_WARN, "1-%d (%p)(%p) (%d)\n", ii, h, pTbl, a);
	}
//*///<<---
#endif

}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 仮想関数

// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>	
//------------------------------------------
// シグナル待ち行列の追加
//------------------------------------------
void DataSetManager::ThreadFirst_AddHandle()
{
	AddEventNum(1, &mque_DataRequest->g_evSem);
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	AddEventNum(1, &mque_DataRequestLongPd->g_evSem);
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------
}

//------------------------------------------
// シグナルを追加した奴のイベント (追加したものを0始まりとする)
//------------------------------------------
void DataSetManager::OnAddSignal(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_MAE_QUE,							// 前工程情報編集要求
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
			EV_LONGPD_QUE,						// 長周期発生通知
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------
			EV_LAST
	};

	////// シグナル条件分け
	//======================================================
	// 前工程情報編集要求
	if ( nEventNo == EV_MAE_QUE ) 
	{
		TYP_DATASET_REQMAE* pQue;							// キュー情報
		pQue = mque_DataRequest->GetFromHead();				// キュー情報取得
		
		EditMaeDefectOutLen(pQue);

		delete pQue;										// キュー情報開放
	}
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	//======================================================
	// 長周期発生通知
	else if ( nEventNo == EV_LONGPD_QUE ) 
	{
		TYP_DATASET_REQLONGPD* pQue;							// キュー情報
		pQue = mque_DataRequestLongPd->GetFromHead();			// キュー情報取得
		
		EditLongPdInf(pQue);

		delete pQue;										// キュー情報開放
	}
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------
}
// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------

//------------------------------------------
// 共有メモリ初期化
// bMode 初期化モード True:全てのテーブルクリア False:普通の初期化
//------------------------------------------
void DataSetManager::OnClearTbl(bool bMode)
{

	//// 継承元を実行
	DataSetBaseManager::OnClearTbl(bMode);
	
	//// 大量ログ防止対策
	m_nErrCnt_Skip_Defect	= 0;
	m_nErrCnt_Err_Key		= 0;
	m_nErrCnt_Err_Coil		= 0;
	m_nErrCnt_NG_Val		= 0;
	m_nErrCnt_Max_Cnt		= 0;

	//// ステータス、ポインタのみクリア
	//memset(mtbl_pStatus, 0x00, sizeof(TBL_HY_STATUS));
	//memset(mtbl_pPoint->nRecDisp, 0x00, sizeof(mtbl_pPoint->nRecDisp));

	if (!bMode) {
		for (int nMen = 0; nMen<NUM_MEN; nMen++) {
			// 全レコード
			for (int nRec = 0; nRec<HY_LST_DEFLIST_MAX; nRec++) {
				// 強調表示対象フラグのみOFFする。
				mtbl_pDefList->data[nMen].defect[nRec].typ.data.nModeDeflist = 0;
			}
		}
	}
}

//------------------------------------------
// 受信データ共有メモリセット
// TO_SO_HEAD const* pHead 伝文ヘッダ
//------------------------------------------
void DataSetManager::OnSetTbl(TO_SO_HEAD const* pHead)
{
	// 遅延テスト用
	BYTE* pD = &((BYTE*)pHead)[sizeof(TO_SO_HEAD)];	// データ部

	switch (pHead->kind) {

//-----------------------------------------------------------
	case TO_SO_DATA_KIND_STATUS:					// 0:ステータス情報
		SetData_Status(pHead, (TO_SO_DATA_STATUS*)pD);
		break;

//-----------------------------------------------------------
	case TO_SO_DATA_KIND_COIL_KEN:					// 1:コイル情報 (検査用)
		SetData_PointWrite(pHead, mtbl_pPoint);					// 検査用情報受信時 書込ポインタ更新
		SetData_CoilInf(pHead, (TO_SO_DATA_COIL*)pD);
		break;

//-----------------------------------------------------------
	case TO_SO_DATA_KIND_COIL_RESULT:				// 2:コイル実績 (1コイル検査完了時)
		SetData_CoilResult(pHead);
		break;

//-----------------------------------------------------------
	case TO_SO_DATA_KIND_COIL_DSP:					// 3:コイル情報 (表示用)
		SetData_CoilDsp(pHead);
		//DeleteNotDispImg();
		break;

//-----------------------------------------------------------
	case TO_SO_DATA_KIND_DETECT:					// 4:疵情報 (疵情報+疵画像)
		SetData_Detect(pHead, (TO_SO_DATA_DETECT*)pD);
		break;
		//switch (pHead->base_head.subinf) {
		//case DIV_SYSTEM_OUT:
		//	SetData_CycleInf(pHead, (TO_SO_DATA_DETECT*)pD);
		//	break;
		//default:
		//	SetData_Detect(pHead, (TO_SO_DATA_DETECT*)pD);
		//	break;
		//}
		//break;

//-----------------------------------------------------------
	case TO_SO_DATA_KIND_LENGTH:					// 5:通板位置情報(検査装置位置、表示基点位置、各表示コイル通板位置情報)
		SetData_Length(pHead, (TO_SO_DATA_LENGTH*)pD);
		break;
//-----------------------------------------------------------
	case TO_SO_DATA_KIND_CUT:						// 6:カット情報
		SetData_Cut(pHead, (TO_SO_DATA_CUT*)pD);
		break;

//-----------------------------------------------------------
	case TO_SO_DATA_KIND_ALARM:						// 7:警報情報
		SetData_Alarm(pHead, (TO_SO_DATA_ALARM*)pD);
		break;

//-----------------------------------------------------------
	case TO_SO_DATA_KIND_DEFLIST_CANCEL:			// 9:重欠陥疵情報削除要求受信
		SetData_DefLst_Cancel(pHead);
		break;

//-----------------------------------------------------------
	case TO_SO_DATA_KIND_DETECT_NUM:				// 10:疵個数情報 (最小区間内)
		SetData_Detect_Count(pHead);
		break;

//-----------------------------------------------------------

	default:
		LOG(em_ERR), "[%s] データ区分異常 [OnSetTbl] [kind= %d] ", my_sThreadName, pHead->kind );
		break;
	}
}

//------------------------------------------
// 書き込みポインタ更新処理
// TO_SO_HEAD const* pH 検査用コイル情報
// int rec 今回のレコード (0オリジン)
//------------------------------------------
void DataSetManager::UpDate_PintWrite(TO_SO_HEAD const* pH, int rec)
{

	//// 対象テーブルレコードを初期化
	memset(&mtbl_pPoint->pnt[rec], 0x00, sizeof(mtbl_pPoint->pnt[rec]));
	memset(&mtbl_pCoilInf->rec[rec], 0x00, sizeof(mtbl_pCoilInf->rec[rec]));
	memset(&mtbl_pCoilResult->rec[rec], 0x00, sizeof(mtbl_pCoilResult->rec[rec]));
	memset(&mtbl_pMapRow->rec[rec], 0x00, sizeof(mtbl_pMapRow->rec[rec]));
	memset(&mtbl_pDefectInf->rec[rec], 0x00, sizeof(mtbl_pDefectInf->rec[rec]));
	//memset(&mtbl_pCycleInf->rec[rec], 0x00, sizeof(mtbl_pCycleInf->rec[rec]));
	memset(&mtbl_pMaeInf->rec[rec], 0x00, sizeof(mtbl_pMaeInf->rec[rec]));
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	memset(&mtbl_pLongPd->rec[rec], 0x00, sizeof(mtbl_pLongPd->rec[rec]));
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

	////対象レコードの次レコードも初期化
	// ポインタ情報削除
	int nNextRec = GetRecIndex(rec, -1);	// (0オリジン)
	memset(&mtbl_pPoint->pnt[nNextRec], 0x00, sizeof(mtbl_pPoint->pnt[nNextRec]));

	//// 画像情報削除依頼(対象レコードの次レコード)
	CString sPath;
	sPath = this->GetFolderPath(m_cPath_Img, nNextRec);
	mcls_pFDele->DeleteStart(sPath);

}
////------------------------------------------
//// 表示範囲外疵画像削除処理
////------------------------------------------
//void DataSetManager::DeleteNotDispImg()
//{
//	CString sPath;
//	//// 表示対象外画像削除依頼
//	int nDelrec = GetDeleteRec();
//	if (0 < nDelrec) {
//		LOG(em_MSG), "[%s] 表示範囲外レコード画像削除 [rec=%d]", my_sThreadName, nDelrec);
//		sPath = this->GetFolderPath(m_cPath_Img, nDelrec-1);
//		mcls_pFDele->DeleteStart(sPath);
//	}
//	else {
//		LOG(em_MSG), "[%s] 表示範囲外レコード画像削除無し [rec=%d]", my_sThreadName, nDelrec);
//	}
//}
//
////------------------------------------------
//// 表示範囲外レコード取得処理
//// Out 0:無し、1～ レコード番号（1オリジン）
////------------------------------------------
//int DataSetManager::GetDeleteRec()
//{
//	// 表示基点レコード位置取得(0オリジンにする)
//	int nRec = mtbl_pPoint->nRecDispBase - 1;
//	// 表示基点に無い場合は終了
//	if (0 > nRec) return 0;
//
//	int nCnt = 0;
//	for (int ii = 0; ii < TBL_HY_REC_MAX; ii++) {
//		// 表示可能または表示予定
//		if (mtbl_pPoint->pnt[nRec].emDispStatus == DIV_DSP_OK || mtbl_pPoint->pnt[nRec].emDispStatus == DIV_DSP_STANDBY) {
//			nCnt = nCnt + 1;
//		}
//		if (nCnt > DSP_COIL_MAX) {
//			return nRec + 1;
//		}
//		// 前回表示レコード位置取得(0オリジン)
//		nRec = GetRecIndex(nRec, 1);
//	}
//	// 表示コイルMAXに到達しなかったので無し
//	return 0;
//}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 実際の書き込み関係

//------------------------------------------
// 0:ステータス情報
// TO_SO_HEAD const* pH 受信ヘッダ
//  const* pD 付加情報部
//------------------------------------------
void DataSetManager::SetData_Status(TO_SO_HEAD const* pH, TO_SO_DATA_STATUS const* pD)
{

	LOG(em_MSG), "[%s][0:ｽﾃｰﾀｽ] 運転=(%s), 稼動=(%s), 検査=(%s/%s), 機器=(%s/%s)", my_sThreadName,
		DivNameManager::GetDivUnten(pD->nUnten), DivNameManager::GetDivKadou(pD->nKadou), 
		DivNameManager::GetDivKensa(pD->status.kensa[0]), DivNameManager::GetDivKensa(pD->status.kensa[1]), 
		DivNameManager::GetDivKiki(pD->status.kiki[0]), DivNameManager::GetDivKiki(pD->status.kiki[1])
		);

	////// 中央以外は無視
	//if (DIV_SYSTEM_SPM != pH->base_head.subinf) return;

	//// データセット
	mtbl_pStatus->emUnten = (EM_DIV_UNTEN)pD->nUnten;
	mtbl_pStatus->emKadou = (EM_DIV_KADOU)pD->nKadou;

	memcpy( &mtbl_pStatus->status, &pD->status, sizeof(TYP_STATUS_NOWSYSTEM));

	//// コイル単位の検査異常をセット
	for(int nMen=0; nMen<NUM_MEN; nMen++) {
		int pos = (0 == nMen) ? DIV_LEN_POS_KEN_TOP: DIV_LEN_POS_KEN_BOT;
		int nRec = mtbl_pPoint->nRecDisp[pos] - 1;				// 各検査台位置でのレコード特定
		if( 0 > nRec ) continue;								// まだレコードが未割り当て

		// 各検査台位置の通板距離[m]を取得
		int nLenDisp = mtbl_pPoint->pnt[nRec].typ.nLenDisp[pos];

		if (DIV_KENSA_OK != mtbl_pStatus->status.kensa[nMen]) {
			// ステータスの検査異常度が正常以外は、各検査台位置レコードのコイル情報.検査状態に1(そのコイルで一区間でも縮退時)をセット
			mtbl_pCoilInf->rec[nRec].coilinf.nCoilKensa = 1;
		}
		// 行情報に現在状態をセット
		SetRow_Inf(nRec, nMen, nLenDisp);
	}

	// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	// 長周期ガイダンス表示判定
	SetLongPdGuidance();
	// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------
}

//------------------------------------------
// 1:コイル情報（検査用）更新
// TO_SO_HEAD const* pH 受信ヘッダ
//  * pD			付加情報部
//------------------------------------------
void DataSetManager::SetData_CoilInf(TO_SO_HEAD const* pH, TO_SO_DATA_COIL const* pD)
{
	LOG(em_MSG), "[%s][1:検査用ｺｲﾙ] <%s> %s, p=%d/%d, s=%d/%d, map=%d", my_sThreadName,
		pH->KizukenNo, pD->coilbase.cCoilNoIn, pD->pcode[0], pD->pcode[1], pD->scode[0], pD->scode[1], pD->nMapColNum );

	////// 中央以外は無視
	//if (DIV_SYSTEM_SPM != pH->base_head.subinf) return;

	//================================================
	// 整合性判定

	//// レコード特定
	int nRec = CheckKanriNo(pH->KizukenNo, mtbl_pPoint);
	if (0 > nRec) {
		LOG(em_ERR), "[%s] 該当レコード無し", my_sThreadName);
		syslog(400, "[検査用コイル情報:管理No=%s]", pH->KizukenNo);		// 該当管理情報無しのため、受信情報を処理できません。
		return;
	}

	if (DIV_DSP_STANDBY != mtbl_pPoint->pnt[nRec].emDispStatus) {
		// コイル情報書込み不可
		LOG(em_ERR), "[%s][1:検査用ｺｲﾙ] コイル情報書込み不可(%d)", my_sThreadName, mtbl_pPoint->pnt[nRec].emDispStatus);
		syslog(406, "[管理No=%s]", pH->KizukenNo);	// 検査用コイル情報を処理することができませんでした。
		return;
	}

	LOG(em_MSG), "[%s] レコード確定 rec=%d", my_sThreadName, nRec);

	//================================================
	// 検査用コイル情報 セット

	COINFO_BASE& coilinf = mtbl_pCoilInf->rec[nRec].coilinf;
	memcpy(coilinf.cKizukenNo, pH->KizukenNo, SIZE_KIZUKEN_NO);
	for(int nMen=0; nMen<NUM_MEN; nMen++) {
		coilinf.scode[nMen] = pD->scode[nMen];	// 検査表面状態
		coilinf.pcode[nMen] = pD->pcode[nMen];	// 検査パターン
	}
	coilinf.nMapColNum = pD->nMapColNum;	// マップ列数
	coilinf.nCoilKensa = 0;					// 検査状態(まずは正常)
	memcpy(&coilinf.base, &pD->coilbase, sizeof(pD->coilbase));		// システム共通のコイル情報
	memcpy(&coilinf.data, &pD->coilinf,  sizeof(pD->coilinf));		// コイル情報

	// 入側距離、出側距離セット
	int	nOutOffset[MAP_STAGE_NUM];
	GetOffsetOutLen(nRec, nOutOffset);		// 出側オフセット位置取得

	//================================================
	// その他テーブル更新
	//// 疵情報
	memcpy(mtbl_pDefectInf->rec[nRec].KizukenNo, pH->KizukenNo, SIZE_KIZUKEN_NO);
	memcpy(mtbl_pDefectInf->rec[nRec].cMeiNo, pD->coilbase.cMeiNo, SIZE_MEI_NO+1);

	for (int nStage = 0; nStage<MAP_STAGE_NUM; nStage++) {
		for (int nMen = 0; nMen<NUM_MEN; nMen++) {
			int nMaxIndex = GET_JUST_INDEX(COIL_LEN_MAX, mcls_pParam->GetCommonMast().nMapDsp[nStage]);	// 最大レコード位置
			int nInLen = 0;						// 入側開始は0
			int nOutLen = nOutOffset[nStage];	// 出側開始は出側オフセット位置。
			for (int ii = 0; ii <= nMaxIndex; ii++) {
				nInLen += mcls_pParam->GetCommonMast().nMapDsp[nStage];
				nOutLen += mcls_pParam->GetCommonMast().nMapDsp[nStage];
				mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[ii].typ.nLen_i = nInLen;
				mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[ii].typ.nLen_o = nOutLen;
				mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[ii].typ.nKizuCnt = -1;
			}
		}
	}

	// 出側距離の再設定（次（上流側)のレコードから）
	int nNextRec = GetRecIndex(nRec, -1);
	// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
	//RecalOutLen(nRec);
	RecalOutLen(nRec, true);
	// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------

	return;
}

//------------------------------------------
// 2:コイル実績 (1コイル検査完了時)
// TO_SO_HEAD const* pH 受信ヘッダ
//------------------------------------------
void DataSetManager::SetData_CoilResult(TO_SO_HEAD const* pH)
{
	LOG(em_MSG), "[%s][2:ｺｲﾙ実績] <%s> %dmm", my_sThreadName, pH->KizukenNo, pH->data_coilresult.len );

	////// 中央以外は無視
	//if (DIV_SYSTEM_SPM != pH->base_head.subinf) return;

	//================================================
	// 整合性判定
	// コイルレコードの取得
	int nRec = CheckKanriNo(pH->KizukenNo, mtbl_pPoint);
	if( 0 > nRec) {
		LOG(em_ERR), "[%s][2:ｺｲﾙ実績] 該当レコード無し [%s]", my_sThreadName, pH->KizukenNo );
		syslog(400, "[コイル実績:管理No=%s]", pH->KizukenNo);	// 該当コイル無しのため、受信情報を処理できません。
		return;
	}

	//// 実測長セット
	int	nRowLast[MAP_STAGE_NUM];

	mtbl_pCoilResult->rec[nRec].nLen = pH->data_coilresult.len;

	for (int nStage = 0; nStage < MAP_STAGE_NUM; nStage++) {
		// 受信した実測長にあたる行位置を取得
		nRowLast[nStage] = GetStageLenToIndex(nStage, (pH->data_coilresult.len / 1000));
	}
	

	//// 行補正
	for (int nMen = 0; nMen < NUM_MEN; nMen++) {
		for (int nStage = 0; nStage < MAP_STAGE_NUM; nStage++) {
			// 現在書き込み行
			int nRowWrite = mtbl_pPoint->pnt[nRec].typ.nRowWrite[nMen][nStage];
			if (nRowLast[nStage] > nRowWrite) {
				// 現在書き込み行よりも受信した実測長にあたる行位置が大きいので、その間の行情報を埋める
				RefRowStatus(nRec, nStage, nMen, nRowWrite, nRowLast[nStage]);
				// 現在書き込み行を更新(0オリジン)
				mtbl_pPoint->pnt[nRec].typ.nRowWrite[nMen][nStage] = nRowLast[nStage];
			}
			//else if (nRowLast[nSpm][nStage] < nRowWrite) {

			// 現在書き込み行よりも受信した実測長にあたる行位置が小さい場合は、その間の疵情報ポインタを
			// 実測長にあたる行位置に再セットする。
			int nMin = 0;
			int nMax = 0;
			int nKizuCnt = 0;
			// 疵情報
			//for (int nRow = nRowWrite; nRow > nRowLast[nSpm][nStage]; nRow--) {
			for (int nRow = MAP_ROW_NUM-1; nRow > nRowLast[nStage]; nRow--) {
				DSP_TYP_ROW& pWk = mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRow].typ;
				
				// 疵個数
				if (-1 < pWk.nKizuCnt) nKizuCnt += pWk.nKizuCnt;
				// 疵個数初期化
				pWk.nKizuCnt = -1;

				// ポインタ情報
				if ((0 == pWk.pnt[0]) || (0 == pWk.pnt[1]))	continue;
				if (0 == nMin || pWk.pnt[0] < nMin) nMin = pWk.pnt[0];			// 最小疵情報ポインタ
				if (0 == nMax || pWk.pnt[1] > nMax) nMax = pWk.pnt[1];			// 最大疵情報ポインタ
			
				// ポインタ情報初期化
				pWk.pnt[0] = 0;
				pWk.pnt[1] = 0;
				
			}
			for (int ii = nMin; ii <= nMax; ii++) {
				if (1 > ii) continue;
				DSP_TYP_DEFECTINF& pDef = mtbl_pDefectInf->rec[nRec].pnt[ii - 1].typ;
				// 疵の長手位置が実測長より短い場合は対象外
				if (pDef.data.y <= pH->data_coilresult.len) continue;
				DSP_TYP_ROW& pWk = mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRowLast[nStage]].typ;
				if (0 == pWk.pnt[0] || ii < pWk.pnt[0]) pWk.pnt[0] = ii;		// 最小疵情報ポインタ
				if (0 == pWk.pnt[1] || ii > pWk.pnt[1]) pWk.pnt[1] = ii;		// 最大疵情報ポインタ
			}
			// 疵個数の設定			
			if (0 < nKizuCnt) {
				DSP_TYP_ROW& pWk = mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRowLast[nStage]].typ;
				pWk.nKizuCnt = nKizuCnt;
			}


			nMin = 0;
			nMax = 0;
			// 前工程疵情報
			//for (int nRow = nRowWrite; nRow > nRowLast[nSpm][nStage]; nRow--) {
			for (int nRow = MAP_ROW_NUM - 1; nRow > nRowLast[nStage]; nRow--) {
					DSP_TYP_ROW& pWk = mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRow].typ;
				if ((0 == pWk.pntMae[0]) || (0 == pWk.pntMae[1]))	continue;
				if (0 == nMin || pWk.pntMae[0] < nMin) nMin = pWk.pntMae[0];			// 最小疵情報ポインタ
				if (0 == nMax || pWk.pntMae[1] > nMax) nMax = pWk.pntMae[1];			// 最大疵情報ポインタ

				// ポインタ情報初期化
				pWk.pntMae[0] = 0;
				pWk.pntMae[1] = 0;
			}
			for (int ii = nMin; ii <= nMax; ii++) {
				if (1 > ii) continue;
				DSP_TYP_MAE_FFINF& pDef = mtbl_pMaeInf->rec[nRec].men[nMen].maeinf[ii - 1].ff;
				// 疵の長手位置が実測長より短い場合は対象外
				if (pDef.nY <= pH->data_coilresult.len) continue;
				DSP_TYP_ROW& pWk = mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRowLast[nStage]].typ;
				if (0 == pWk.pntMae[0] || ii < pWk.pntMae[0]) pWk.pntMae[0] = ii;		// 最小疵情報ポインタ
				if (0 == pWk.pntMae[1] || ii > pWk.pntMae[1]) pWk.pntMae[1] = ii;		// 最大疵情報ポインタ
			}
			//}
		}
	}
	

	//// ポインタ情報更新
	for (int nStage = 0; nStage < MAP_STAGE_NUM; nStage++) {
		// 受信した実測長にあたる行位置を検査完了行とする。(1オリジン)
		mtbl_pPoint->pnt[nRec].typ.nRowLast[nStage] = nRowLast[nStage] + 1;
	}

	//// 出側距離の再設定（次（上流側)のレコードから）
	int nNextRec = GetRecIndex(nRec, -1);
	// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
	//RecalOutLen(nNextRec);
	RecalOutLen(nNextRec, true);
	// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------

	//================================================
	// コイルエンド処理
	//// 大量ログ出力
	// 結果ログ出力
	if( m_nErrCnt_Skip_Defect ) { 
		LOG(em_ERR), "[%s] <大量ログ結果> 疵情報スキップ(%s:%d件)", my_sThreadName, pH->KizukenNo, m_nErrCnt_Skip_Defect); 
		syslog(404, "[疵情報スキップ:%s:%d件]", pH->KizukenNo, m_nErrCnt_Skip_Defect); 
	}
	if( m_nErrCnt_Err_Key ) { 
		LOG(em_ERR), "[%s] <大量ログ結果> 該当レコード無し(%s:%d件)", my_sThreadName, pH->KizukenNo, m_nErrCnt_Err_Key); 
		syslog(404, "[該当レコード無し:%s:%d件]", pH->KizukenNo, m_nErrCnt_Err_Key); 
	}
	if( m_nErrCnt_Err_Coil ) {
		LOG(em_ERR), "[%s] <大量ログ結果> 該当論理コイルなし(%s:%d件)", my_sThreadName, pH->KizukenNo, m_nErrCnt_Err_Coil); 
		syslog(404, "[該当レコード無し:%s:%d件]", pH->KizukenNo, m_nErrCnt_Err_Coil); 
	}
	if( m_nErrCnt_NG_Val ) { 
		LOG(em_ERR), "[%s] <大量ログ結果> 整合性異常(%s:%d件)", my_sThreadName, pH->KizukenNo, m_nErrCnt_NG_Val); 
		syslog(404, "[整合性異常:%s:%d件]", pH->KizukenNo, m_nErrCnt_NG_Val); 
	}
	if( m_nErrCnt_Max_Cnt ) { 
		LOG(em_ERR), "[%s] <大量ログ結果> 格納件数上限到達(%s:%d件)", my_sThreadName, pH->KizukenNo, m_nErrCnt_Max_Cnt); 
		syslog(404, "[格納件数上限到達:%s:%d件]", pH->KizukenNo, m_nErrCnt_Max_Cnt); 
	}

	// 初期化
	m_nErrCnt_Skip_Defect	= 0;
	m_nErrCnt_Err_Key		= 0;
	m_nErrCnt_Err_Coil		= 0;
	m_nErrCnt_NG_Val		= 0;
	m_nErrCnt_Max_Cnt		= 0;

	LOG(em_MSG), "--------------------------- 入側コイル切り替わり ---------------------------");
}

//------------------------------------------
// 3:コイル情報（表示用）更新
// TO_SO_HEAD const* pH 受信ヘッダ
//------------------------------------------
void DataSetManager::SetData_CoilDsp(TO_SO_HEAD const* pH)
{
	bool	bRecal = false;				// 出側距離再計算フラグ

	LOG(em_MSG), "[%s][3:表示用ｺｲﾙ] <%s>", my_sThreadName, pH->KizukenNo);

	// レコードの取得
	int nRec = CheckKanriNo(pH->KizukenNo, mtbl_pPoint);
	if( 0 > nRec) {
		LOG(em_ERR), "[%s][3:表示用ｺｲﾙ] 該当レコード無し [%s]", my_sThreadName, pH->KizukenNo );
		syslog(400, "[表示用コイル情報:管理No=%s]", pH->KizukenNo);	// 該当コイル無しのため、受信情報を処理できません。
		return;
	}

	// 表示状態を確認
	if (DIV_DSP_STANDBY != mtbl_pPoint->pnt[nRec].emDispStatus) {
		// 待機中ではない
		LOG(em_ERR), "[%s][3:表示用ｺｲﾙ] 状態エラーのため受付不可 [%s].(%d)", my_sThreadName, pH->KizukenNo, mtbl_pPoint->pnt[nRec].emDispStatus );
		if (DIV_DSP_OK != mtbl_pPoint->pnt[nRec].emDispStatus) {
			// 重複受信以外（このコイルの後に検査受信したコイルが先に表示受信をした？）
			syslog(408, "[管理No=%s]", pH->KizukenNo);	// 表示用コイル情報を処理することができませんでした。
		}
		return;
	}

	mtbl_pPoint->nRecDispBase = nRec + 1;				// 表示基点位置での現在表示レコードを更新(1オリジン)

	mtbl_pPoint->pnt[nRec].emDispStatus = DIV_DSP_OK;	// 表示状態を表示可にする。

	// このコイルよりも先に検査受信しているのに、表示受信をしていないコイルの画面表示状態を変更
	int ii = 1;
	while (1) {
		int nWk = GetRecIndex(nRec, ii);
		if (nWk == nRec) {
			// 周回したので終了
			break;
		}
		if ((0 == mtbl_pPoint->pnt[nWk].emDispStatus)||(DIV_DSP_OK == mtbl_pPoint->pnt[nWk].emDispStatus)) {
			//　表示状態が表示可、もしくは、0(データなし)のコイルが見つかったら終了
			break;
		}
		if (DIV_DSP_STANDBY == mtbl_pPoint->pnt[nWk].emDispStatus) {
			// このコイルが先に検査受信しているのに、表示受信を受信していない
			LOG(em_ERR), "[%s][3:表示用ｺｲﾙ] 表示未受信のため状態NG [%s]", my_sThreadName, mtbl_pPoint->pnt[nWk].cKizukenNo);
			syslog(409, "[管理No=%s]", mtbl_pPoint->pnt[nWk].cKizukenNo);		// 表示用コイル情報を受信していません。
			mtbl_pPoint->pnt[nWk].emDispStatus = DIV_DSP_NG;
			bRecal = true;
		}
		ii++;
	}

	if (bRecal) {
		// 出側距離の再設定
		// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
		//RecalOutLen(nRec);
		RecalOutLen(nRec, true);
		// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
	}

	return;
}

//------------------------------------------
// 4:疵情報 (疵情報+疵画像)
// TO_SO_HEAD const* pH 受信ヘッダ
//  * pD			付加情報部
//------------------------------------------
void DataSetManager::SetData_Detect(TO_SO_HEAD const* pH, TO_SO_DATA_DETECT const* pD)
{
	int   nMen = pH->men;
	int	  nRow[MAP_STAGE_NUM];	// マップ格納行位置 (0オリジン)
	int   nKizuLen;
	//================================================
	// 整合性判定

	//// レコード特定
	// レコードの取得
	int nRec = CheckKanriNo(pH->KizukenNo, mtbl_pPoint);
	if (0 > nRec) {
		if (0 == m_nErrCnt_Err_Key) {
			LOG(em_ERR), "[%s][4:疵情報] 該当レコード無し [%s]", my_sThreadName, pH->KizukenNo );
			syslog(400, "[疵情報:管理No=%s]", pH->KizukenNo);	// 該当コイル無しのため、受信情報を処理できません。
		}
		m_nErrCnt_Err_Key++;
		return;
	}

	LOG(em_MSG), "[%s][4:疵情報] <%s:%d:%d> Y=%dmm, T=%d, G=%d 探傷=%d 重欠陥=%d ﾊﾟﾄﾗｲﾄ疵=%d", my_sThreadName, 
		pH->KizukenNo, pH->men, pD->datas.nKizuNo, (int)pD->datas.y, pD->datas.nTid, pD->datas.nGid,
		pD->datas.nObjTansho, pD->datas.nObjDeflist, pD->datas.nModeDeflist);

	//================================================
	// 疵の長手位置から行位置を算出する。
	nKizuLen = (int)pD->datas.y;
	// 検査実測長設定済の過去コイル かつ 疵の長手位置が検査実測長を超える場合
	if ((0 < mtbl_pCoilResult->rec[nRec].nLen) && (nKizuLen > mtbl_pCoilResult->rec[nRec].nLen)) {
		// 行位置を検査実測長の位置に補正
		nKizuLen = mtbl_pCoilResult->rec[nRec].nLen;
	}

	for (int nStage = 0; nStage<MAP_STAGE_NUM; nStage++) {
		nRow[nStage] = GetStageLenToIndex(nStage, int(nKizuLen / 1000));
	}
	

	//// 探傷、重欠陥一覧画面出力の可否を確認
	if( 0 == pD->datas.nObjTansho && 0 == pD->datas.nObjDeflist){
		// 疵情報データにて、全画面出力対象疵ではない
		LOG(em_MSG), "[%s][4:疵情報] 表示対象外1 <%s:%d> Y=%d[mm]:%d/%d/%d  X=%d KizuNo=%d T=%d G=%d objT=%d objL=%d",
			my_sThreadName,
			pH->KizukenNo, pH->men, (int)pD->datas.y, nRow[0], nRow[1], nRow[2], pD->datas.nWidDiv,
			pD->datas.nKizuNo, pD->datas.nTid, pD->datas.nGid,
			pD->datas.nObjTansho, pD->datas.nObjDeflist );
		return;
	}

	//// コイル情報の検査パターンにより、疵種、グレードが探傷画面出力対象であるかを確認
	//int tid = pD->data[0].nTid;
	//int gid = pD->data[0].nGid;
	//int	p = mtbl_pCoilInf->rec[nRec].coilinf.pcode[nMen];
	//bool bDspTansho = mcls_pParam->Check_Output( (ParamManager::EM_OUTPUTFLG)(ParamManager::OUTPUTFLG_DSP_O+nMen), p-1, tid, gid );
	//if( false == bDspTansho ){
	//	// 疵種、グレードが表示対象ではない
	//	LOG(em_MSG), "[%s][4:疵情報] 表示対象外2 <%s:%d> Y=%dmm:%d/%d/%d X=%d, KizuNo=%d, T=%d, G=%d objT=%d", my_sThreadName, 
	//		pH->KizukenNo, pH->men, nRow[0][0], nRow[0][1], nRow[0][2], pD->data[0].nWidDiv,
	//		pD->data[0].nKizuNo, pD->data[0].nTid, pD->data[0].nGid, pD->data[0].nObjTansho );
	//	return;
	//}

	//// 面チェック
	if( (0 > nMen) || (NUM_MEN <= nMen)) {
		if( 0 == m_nErrCnt_NG_Val ) {
			LOG(em_ERR), "[%s][4:疵情報] 該当面情報異常 面=%d", my_sThreadName, nMen);
			syslog(401, "[疵情報:管理No=%s:面=%d]", pH->KizukenNo, nMen);	// 整合性異常のため、受信情報を処理できません。
		}
		m_nErrCnt_NG_Val++;
		return;
	}

#ifdef LOG_DEBUG
	LOG(em_MSG), "[%s][4:疵情報] <%s><Men:%d> Y=%dmm:%d/%d/%d X=%d, KizuNo=%d, T=%d, G=%d objT=%d objL=%d objM=%d", my_sThreadName, 
	pH->KizukenNo, pH->men, nRow[0][0], nRow[0][1], nRow[0][2], pD->data[0].nWidDiv,
	pD->data[0].nKizuNo, pD->data[0].nTid, pD->data[0].nGid, pD->data[0].nObjTansho, pD->datas[0].nObjDeflist, pD->datas[0].nObjDefmap );
#endif

	//// 優先度
	if(0 == pD->datas.nTYuu || 0 == pD->datas.nGYuu ) {
		if( 0 == m_nErrCnt_NG_Val ) {
			LOG(em_ERR), "[%s][4:疵情報] 該当疵情報異常 優先度T=%d 優先度G=%d", my_sThreadName, pD->datas.nTYuu, pD->datas.nGYuu);
			syslog(401, "[疵情報:管理No=%s:優先度T=%d 優先度G=%d]", pH->KizukenNo, pD->datas.nTYuu, pD->datas.nGYuu);	// 整合性異常のため、受信情報を処理できません。
		}
		m_nErrCnt_NG_Val++;
		return;
	}

	//// 上限チェック
	int nNum = mtbl_pDefectInf->rec[nRec].num;				// 上限到達？
	if (MAX_MAP_DEFECT_NUM <= nNum) {
		if( 0 == m_nErrCnt_Max_Cnt) {
			LOG(em_ERR), "[%s][4:疵情報] 格納件数上限到達により破棄", my_sThreadName);
			syslog(402, "[管理No=%s]", pH->KizukenNo);	// 疵格納件数上限到達により、以降の疵情報を破棄します。
		}
		m_nErrCnt_Max_Cnt++;

		// スキップしたことを疵マップに通知
		SetRecvStatus(nRec, nMen, nRow[0], DIV_HY_RECVST_SKIP);

		return;
	}

	//================================================
	// 負荷対策
	// 未処理キューが指定件数を超えたら、強制スキップ実施
	if(QUE_SKIP_COUNT < GetToHyQueCount()) {
		// スキップしたことを疵マップに通知
		SetRecvStatus(nRec, nMen, nRow[0], DIV_HY_RECVST_SKIP);
		
		if( 0 == m_nErrCnt_Skip_Defect ) {
			LOG(em_ERR), "[%s][4:疵情報] 該当疵情報スキップ 未処理Que=%d", my_sThreadName, GetToHyQueCount());
			syslog(403, "[管理No=%s]", pH->KizukenNo);		// 疵情報大量受信により、処理できません。疵情報を一部スキップします。
		}
		m_nErrCnt_Skip_Defect++;
		return;
	}

	// 疵情報セット
	SetDefectInfo(pD, nRec, nMen, nNum);

	if (1 == pD->datas.nObjDeflist) {
		SetDefLst(pD, pH->KizukenNo, nRec, nMen);
	}


	// 疵ポインタを登録（最小、最大を更新）
	for (int nStage = 0; nStage<MAP_STAGE_NUM; nStage++) {
		DSP_TYP_ROW& pWk = mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRow[nStage]].typ;
		if (0 == pWk.pnt[0] || pWk.pnt[0] > nNum + 1) pWk.pnt[0] = nNum + 1;			// 最小疵情報ポインタ(1オリジン)
		if (0 == pWk.pnt[1] || pWk.pnt[1] < nNum + 1) pWk.pnt[1] = nNum + 1;			// 最大疵情報ポインタ(1オリジン)
	}

			//// 行情報更新
			//SetRow_Inf(nRec, nMen, nSpm, (int)(pD->datas[nSpm].y / 1000));

			//for (int nStage = 0; nStage<MAP_STAGE_NUM; nStage++) {

			//	int nRowWrite = mtbl_pPoint->pnt[nRec].typ[nSpm].nRowWrite[nMen][nStage];
			//	if (nRowWrite >= nRow[nSpm][nStage]) {
			//		// 格納されているデータと等しい、もしくは、大きい場合は、現在の書込み行は更新しない
			//		continue;
			//	}

			//	// 現在書込み行～新たな行までの間の行情報を埋める
			//	DSP_TYP_ROW& pBaseRow = mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRowWrite].typ[nSpm];
			//	for (nRowWrite = nRowWrite + 1; nRowWrite<nRow[nSpm][nStage]; nRowWrite++) {
			//		DSP_TYP_ROW& pNewRow = mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRowWrite].typ[nSpm];
			//		// 詳細情報セット
			//		pNewRow.emKadou = pBaseRow.emKadou;										// 稼動状態
			//		pNewRow.emKensa[DIV_SYSTEM_SPM] = pBaseRow.emKensa[DIV_SYSTEM_SPM];		// 検査状態
			//		pNewRow.emKiki[DIV_SYSTEM_SPM] = pBaseRow.emKiki[DIV_SYSTEM_SPM];		// 機器状態
			//		SetDataStatusMerge(nRec, nStage, nMen, nSpm, nRowWrite, DIV_SYSTEM_SPM);
			//	}
			//	// 現在書き込み行を更新(0オリジン)
			//	mtbl_pPoint->pnt[nRec].typ[nSpm].nRowWrite[nMen][nStage] = nRow[nSpm][nStage];
			//}


	return;
}



//------------------------------------------
// 5:通板位置情報(各通板ポイント位置情報)
// TO_SO_HEAD const* pH 受信ヘッダ
//  * pD			付加情報部
//------------------------------------------
void DataSetManager::SetData_Length(TO_SO_HEAD const* pH, TO_SO_DATA_LENGTH const* pD)
{
	/*
	LOG(em_MSG), "[%s][5:通板位置情報] (カメラ位置(表)=%s:%d) (表示位置(表)=%s:%d)", my_sThreadName,
	pD->dsp[DIV_LEN_POS_KEN_TOP].KizukenNo, pD->dsp[DIV_LEN_POS_KEN_TOP].len, pD->dsp[DIV_LEN_POS_DSP_TOP].KizukenNo, pD->dsp[DIV_LEN_POS_DSP_TOP].len);
	LOG(em_MSG), "[%s][5:通板位置情報] (カメラ位置(裏)=%s:%d) (表示位置(裏)=%s:%d)", my_sThreadName, 
	pD->dsp[DIV_LEN_POS_KEN_BOT].KizukenNo, pD->dsp[DIV_LEN_POS_KEN_BOT].len, pD->dsp[DIV_LEN_POS_DSP_BOT].KizukenNo, pD->dsp[DIV_LEN_POS_DSP_BOT].len);
*/

	bool	bKensa;
	int		nMen;
	bool	bRecal = false;				// 出側距離再計算フラグ
	int		nRecWk = 0;

	// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	SetLongPdGuidance();
	// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

	//================================================
	// 各通板ポイント
	for(int pos=0; pos<MAX_DISP_POS; pos++) {
		// レコード不明時は無処理
		if( NULL == pD->dsp[pos].KizukenNo[0] ) continue;

		// レコードの取得
		int nRec = CheckKanriNo(pD->dsp[pos].KizukenNo, mtbl_pPoint);
		if (0 > nRec) {
			LOG(em_ERR), "[%s][5:通板位置情報] 各通板ポイント該当レコード無し。[%s] pos<%d>", my_sThreadName, pD->dsp[pos].KizukenNo, pos);
			continue;
		}

		// 通板距離の行位置を取得
		int nRow[MAP_STAGE_NUM];
		for (int nStage = 0; nStage < MAP_STAGE_NUM; nStage++) {
			nRow[nStage] = GetStageLenToIndex(nStage, pD->dsp[pos].len);
		}

		// 各通板ポイント位置での現在レコードを更新
		mtbl_pPoint->nRecDisp[pos] = nRec + 1;

		switch (pos) {
		case DIV_LEN_POS_KEN_TOP:					// カメラ位置(表面)
			mtbl_pPoint->nRecWrite[0] = nRec + 1;	// 検査装置位置での現在レコードを更新(1オリジン)
			bKensa = true;
			nMen = 0;
			break;
		case DIV_LEN_POS_KEN_BOT:					// カメラ位置(裏面)
			mtbl_pPoint->nRecWrite[1] = nRec + 1;	// 検査装置位置での現在レコードを更新(1オリジン)
			bKensa = true;
			nMen = 1;
			break;
		case DIV_LEN_POS_DSP_TOP:					// 表示基点(表面)
			bKensa = false;
			nMen = 0;
			break;
		case DIV_LEN_POS_DSP_BOT:					// 表示基点(裏面)
			bKensa = false;
			nMen = 1;
			break;
		default:
			bKensa = false;
		}
		if (bKensa) {
			// 行情報に現在状態をセット
			SetRow_Inf(nRec, nMen, pD->dsp[pos].len);
			for (int nStage = 0; nStage<MAP_STAGE_NUM; nStage++) {
				int nRowWrite = mtbl_pPoint->pnt[nRec].typ.nRowWrite[nMen][nStage];
				if (nRowWrite >= nRow[nStage]) {
					// 格納されているデータと等しい、もしくは、大きい場合は、現在の書込み行は更新しない
					continue;
				}

				// 現在書き込み行から新たな書込み行-1まで、行情報ステータスをコピー
				RefRowStatus(nRec, nStage, nMen, nRowWrite, nRow[nStage] - 1);

				// 現在書き込み行を更新(0オリジン)
				mtbl_pPoint->pnt[nRec].typ.nRowWrite[nMen][nStage] = nRow[nStage];
			}
			if (DIV_KENSA_OK != mtbl_pStatus->status.kensa[nMen]) {
				// 検査装置位置レコードのコイル情報.検査状態に1(そのコイルで一区間でも縮退時)をセット
				mtbl_pCoilInf->rec[nRec].coilinf.nCoilKensa = 1;
			}
		}
						
		// 各検査台位置の通板距離
		mtbl_pPoint->pnt[nRec].typ.nLenDisp[pos] = pD->dsp[pos].len;
		for (int nStage = 0; nStage<MAP_STAGE_NUM; nStage++) {
			// 各検査台位置での現在表示行を更新(1オリジン)
			mtbl_pPoint->pnt[nRec].typ.nRowDisp[pos][nStage] = nRow[nStage] + 1;
			//----------->>>
			if (0 < mtbl_pPoint->pnt[nRec].typ.nRowLast[nStage]) {
				if (nRow[nStage] + 1 > mtbl_pPoint->pnt[nRec].typ.nRowLast[nStage]) {
					mtbl_pPoint->pnt[nRec].typ.nRowLast[nStage] = nRow[nStage] + 1;
					DSP_TYP_ROW& pWk = mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRow[nStage]].typ;
					pWk.emDspKensa = DIV_KENSA_OK;
					pWk.emDspKiki = DIV_KIKI_OK;
					bRecal = true;
					if (0 != nRecWk) nRecWk = nRec;
					LOG(em_MSG), "[%s] 検査完了行変更[%s] nRec=%d Stage=%d men=%d row=%d", 
						my_sThreadName, mtbl_pPoint->pnt[nRec].cKizukenNo, nRec, nStage, nMen, nRow[nStage]);
				}
			}
			//<<<-----------
		}
	}
	
	if (bRecal) {
		int nNextRec = GetRecIndex(nRecWk, -1);
		// 出側距離の再設定
		// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
		//RecalOutLen(nNextRec);
		RecalOutLen(nNextRec, true);
		// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
	}

	return;
}

//------------------------------------------
// 6:カット情報
// TO_SO_HEAD const* pH 受信ヘッダ
//  * pD			付加情報部
//------------------------------------------
void DataSetManager::SetData_Cut(TO_SO_HEAD const* pH, TO_SO_DATA_CUT const* pD)
{

	LOG(em_INF), "[%s][6:カット情報] (管理No=%s) (区分=%d 位置=%dmm)", my_sThreadName, pH->KizukenNo, pD->nMode, pD->nPos);

	//================================================
	// 整合性判定

	// コイルレコードの取得
	int nRec = CheckKanriNo(pH->KizukenNo, mtbl_pPoint);
	if( 0 > nRec) {
		LOG(em_ERR), "[%s][6:カット情報] 該当レコード無し", my_sThreadName);
		syslog(400, "[カット情報:管理No=%s]", pH->KizukenNo);	// 該当コイル無しのため、受信情報を処理できません。
		return;
	}

	//// カット数チェック
	int nCutCnt = mtbl_pCoilResult->rec[nRec].nCutCnt;
	if( MAX_SHACUT_COUNT <= nCutCnt ) {
		LOG(em_WAR), "[%s][6:カット情報] カット数異常 %d (%dmm)", my_sThreadName, pD->nMode, pD->nPos);
		return;
	}

	//================================================
	// カット情報セット
	// 長手順にソートするため、挿入位置を検索
	int nInsert = 0;
// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
	bool bSamePos = false;
// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
	for(nInsert=0; nInsert<mtbl_pCoilResult->rec[nRec].nCutCnt; nInsert++) {
		if( mtbl_pCoilResult->rec[nRec].cut[nInsert].nPos > pD->nPos ) break;
// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
		if( mtbl_pCoilResult->rec[nRec].cut[nInsert].nPos == pD->nPos ) 
		{
			bSamePos = true;
			break;
		}
// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
	}
	// 挿入
// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
	if( true == bSamePos ) 		
	{
		LOG(em_MSG), "[%s][6:カット情報] 同一位置受信のため保存せず %d (%dmm)", my_sThreadName, pD->nMode, pD->nPos);
	}
	else
	{
// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------	

		for(int ii=MAX_SHACUT_COUNT-1; ii>=nInsert; ii-- ) {
			if(MAX_SHACUT_COUNT-1 == ii) continue;
			memcpy( &mtbl_pCoilResult->rec[nRec].cut[ii+1], &mtbl_pCoilResult->rec[nRec].cut[ii], sizeof(TO_SO_DATA_CUT) );
		}
		// セット	
		TO_SO_DATA_CUT& pWk = mtbl_pCoilResult->rec[nRec].cut[nInsert];
		pWk.nMode = pD->nMode;
		pWk.nPos = pD->nPos;
	mtbl_pCoilResult->rec[nRec].nCutCnt ++;

// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
	}
// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------

	//================================================
	// シャーカット区分設定
	BYTE nAdd = 0;
	if (DIV_CUT_FCUT == pD->nMode) nAdd = ROW_KIND_BOTTOM;					// 成品尾端
	if (DIV_CUT_LCUT == pD->nMode) nAdd = ROW_KIND_UPPER;					// 成品先端
	if (DIV_CUT_DIV == pD->nMode) nAdd = (ROW_KIND_UPPER | ROW_KIND_BOTTOM);
	if (DIV_CUT_WPD_OTHER == pD->nMode) nAdd = ROW_KIND_YOSETU;				// 溶接点
	for (int nStage = 0; nStage<MAP_STAGE_NUM; nStage++) {
		int nRow = GetStageLenToIndex(nStage, pD->nPos / 1000);
		for (int nMen = 0; nMen<NUM_MEN; nMen++) {
			// 付加情報にカット区分によるフラグをセット。
			mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRow].typ.nKind |= nAdd;
			if (ROW_KIND_BOTTOM & mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRow].typ.nKind) {
				// 成品尾端の場合は、シャーカット区分を1にする。
				mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRow].typ.nCut = 1;
			}
		}
	}
	

	if(!(ROW_KIND_BOTTOM & nAdd)){
		// 成品尾端以外は以降の処理は行わない
		return;
	}

	// 出側距離の再設定
	// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
	//RecalOutLen(nRec);
	RecalOutLen(nRec, true);
	// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
	return;
}

//------------------------------------------
// 7:警報情報
// TO_SO_HEAD const* pH 受信ヘッダ
//  * pD			付加情報部
//------------------------------------------
void DataSetManager::SetData_Alarm(TO_SO_HEAD const* pH, TO_SO_DATA_ALARM const* pD)
{
	LOG(em_MSG), "[%s][7:警報情報]", my_sThreadName );
	memcpy(mtbl_pAlarm, pD, sizeof(TO_SO_DATA_ALARM));
}


//------------------------------------------
// 9:重欠陥疵情報削除要求
// TO_SO_HEAD const* pH 受信ヘッダ
//------------------------------------------
void DataSetManager::SetData_DefLst_Cancel(TO_SO_HEAD const* pH)
{
	LOG(em_MSG), "[%s][9:重欠陥疵情報削除要求] (管理No=%s) 検査面=%d キャンセル対象の疵連番=%d",
		my_sThreadName, pH->KizukenNo, pH->men, pH->deflist_cancel.nKizuSeq );

	//// コイルレコードの取得
	//int nRec = CheckKanriNo(pH->KizukenNo, mtbl_pPoint);
	//if (0 > nRec) {
	//	LOG(em_ERR), "[%s][9:重欠陥疵情報削除要求] 該当レコード無し", my_sThreadName);
	//	syslog(400, "[重欠陥疵情報削除要求:管理No=%s]", pH->KizukenNo);	// 該当コイル無しのため、受信情報を処理できません。
	//	return;
	//}

	//int nDefCnt;
	int nKizuSeq = pH->deflist_cancel.nKizuSeq;
	int nMen = pH->men;
	bool bFlg = false;


	// 重欠陥テーブル更新
	for (int ii = 0; ii<HY_LST_DEFLIST_MAX; ii++) {
		DSP_TYP_DEFECTINF& pWk = mtbl_pDefList->data[nMen].defect[ii].typ;
		if (0 == memcmp(mtbl_pDefList->data[nMen].defect[ii].KizuKenNo, pH->KizukenNo, SIZE_KIZUKEN_NO)) {
			// 対象疵か？
			if (nKizuSeq == pWk.data.nKizuNo) {
				if (1 == pWk.data.nModeDeflist) {
					pWk.data.nModeDeflist = 0;
					LOG(em_MSG), "[%s][重欠陥疵情報削除要求] 強調表示フラグ解除 <%s> 面=%d 疵連番=%d Rec=%d",
						my_sThreadName, pH->KizukenNo, pH->men, pH->deflist_cancel.nKizuSeq, ii);
				}
				else {
					LOG(em_WAR), "[%s][重欠陥疵情報削除要求] 強調表示非対象 <%s> 面=%d 疵連番=%d Rec=%d",
						my_sThreadName, pH->KizukenNo, pH->men, pH->deflist_cancel.nKizuSeq, ii);
				}
				
				bFlg = true;
				break;
			}
		}
	}

	if (!bFlg) {
		LOG(em_WAR), "[%s][重欠陥疵情報削除要求] 該当疵連番無し <%s> 面=%d 疵No=%d",
			my_sThreadName, pH->KizukenNo, pH->men, pH->deflist_cancel.nKizuSeq);
	}
}

//------------------------------------------
// 10:疵個数情報 (最小区間内)
// TO_SO_HEAD const* pH 受信ヘッダ
//------------------------------------------
void DataSetManager::SetData_Detect_Count(TO_SO_HEAD const* pH)
{
	int nMen = pH->men;
	int nKizuCnt = 0;
	int nRow = 0;

	LOG(em_INF), "[%s][10:疵個数情報 (最小区間内)] (管理No=%s) 検査面=%d 実測長=%d 疵個数=%d", my_sThreadName, pH->KizukenNo, pH->men, pH->data_defnum.len, pH->data_defnum.num);

	//================================================
	// 整合性判定

	// コイルレコードの取得
	int nRec = CheckKanriNo(pH->KizukenNo, mtbl_pPoint);
	if (0 > nRec) {
		LOG(em_ERR), "[%s][10:疵個数情報 (最小区間内)] 該当レコード無し", my_sThreadName);
		syslog(400, "[疵個数情報 (最小区間内):管理No=%s]", pH->KizukenNo);	// 該当コイル無しのため、受信情報を処理できません。
		return;
	}

	// 面チェック
	if ((0 > nMen) || (NUM_MEN <= nMen)) {
		LOG(em_ERR), "[%s][10:疵個数情報 (最小区間内)] 該当面情報異常 面=%d", my_sThreadName, nMen);
		syslog(401, "[10:疵個数情報 (最小区間内):管理No=%s:面=%d]", pH->KizukenNo, nMen);	// 整合性異常のため、受信情報を処理できません。
		return;
	}

	//================================================
	// 疵個数の長手位置から行位置を算出する。
	int nKizuLen = (int)pH->data_defnum.len;
	// 検査実測長設定済の過去コイル かつ 疵の長手位置が検査実測長を超える場合
	if ((0 < mtbl_pCoilResult->rec[nRec].nLen) && (nKizuLen > mtbl_pCoilResult->rec[nRec].nLen)) {
		// 行位置を検査実測長の位置に補正
		nKizuLen = mtbl_pCoilResult->rec[nRec].nLen;
	}

	//================================================
	// 最小区間の疵個数を設定
	nKizuCnt = pH->data_defnum.num;		// 表示エリア小、表示エリア大に加算する疵個数
	// 対象疵個数の行位置を取得
	nRow = GetStageLenToIndex(0, (nKizuLen / 1000));
	DSP_TYP_ROW& pWk = mtbl_pMapRow->rec[nRec].stage[0].men[nMen].row[nRow].typ;
	
	// 最小区間の疵個数再取得時
	// 共有メモリの疵個数が-1の場合、統括から対象行の疵個数を取得していない
	if (-1 < pWk.nKizuCnt) {
		// 最小区間の疵個数を取得していた場合、
		// 前回の疵個数を表示エリア小、表示エリア大から除いて、新規に受信した疵個数を加算する
		nKizuCnt -= pWk.nKizuCnt;
	}

	// 最小区間の疵個数を共有メモリに反映（上書き）
	pWk.nKizuCnt = pH->data_defnum.num;

	//================================================
	// 表示エリア小、表示エリア大の疵個数を設定
	for (int nStage = 1; nStage < MAP_STAGE_NUM; nStage++) {
		// 対象疵個数の行位置を取得
		nRow = GetStageLenToIndex(nStage, (nKizuLen / 1000));
		DSP_TYP_ROW& pWk = mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRow].typ;
		
		// 疵個数未受信(-1)の場合は初期化
		if (0 > pWk.nKizuCnt) pWk.nKizuCnt = 0;

		// 最小区間の疵個数を表示エリア小 or 表示エリア大に反映（加算）
		pWk.nKizuCnt += nKizuCnt;
	}

	return;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// テーブル操作
//------------------------------------------
// 疵情報セット
// TO_SO_DATA_DETECT* pD	疵情報
// int nRec					テーブルレコードNo (0オリジン)
// int nMen					表裏区分
// int nIndex				疵情報レコード位置 (0オリジン)
//------------------------------------------
void DataSetManager::SetDefectInfo(TO_SO_DATA_DETECT const* pD, const int nRec, const int nMen, const int nIndex)
{
	//================================================
	// 前準備
	DSP_TYP_DEFECTINF& pSet = mtbl_pDefectInf->rec[nRec].pnt[ nIndex ].typ;

	char  cPath[MAX_HT1_CAMANGLE][32];
	char* pPath[MAX_HT1_CAMANGLE];						// 共有メモリパス
	pPath[0] = pSet.image_path_s;
	pPath[1] = pSet.image_path_r;
	pPath[2] = pSet.image_path_r2;
	pPath[3] = pSet.image_path_r3;


	//================================================
	// 疵情報セット
	memcpy(&pSet.data, &pD->datas, sizeof(DETECT_BASE_DATA));
// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
	// 疵単位の出側距離 を付け直し
	GetDeliveryPosY(nRec, pD->datas.y, &pSet.data.y_out);
// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------

	for(int ii=0; ii<MAX_CAMANGLE; ii++) {
		sprintf(cPath[ii], "%s\\%d_%d_%d.jpg", (LPCTSTR)this->GetFolderPath(m_cPath_Img, nRec), nMen, pSet.data.nKizuNo, ii);	// 画像は疵連番にする
	}

	//================================================
	// 疵画像登録
	int	nImgAdd = 0;				// 0:画像登録可  -9:代表疵破棄  -1:リストに隠れる疵破棄  -99:該当PC画像保存無し

	//--------------------------------
	//// 事前チェック
	//if( 0 != pD->data[0].nFrameDefMax) {
	if (1) {
		if( 0 == mque_pImgSave->GetFreeCount() ) nImgAdd = -9;
	} else {
		if( 0 == mque_pImgSaveLow->GetFreeCount() ) nImgAdd = -1;
	}
	// 画像登録有無
	if (1 == m_nImgSaveOff) nImgAdd = -99;

	//--------------------------------
	//// 画像登録
	if( 0 == nImgAdd ) {
		// 生成
		int nOffset = 0;

		FileSaveManager::TYP_SAVE_IMG* pImg = new FileSaveManager::TYP_SAVE_IMG;
		memset(pImg, 0x00, sizeof(FileSaveManager::TYP_SAVE_IMG));

		//// セット
		pImg->nSetCnt = MAX_IMAGE_CNT;
		for(int ii=0; ii<MAX_IMAGE_CNT; ii++) {
			// 表示対象疵か？
			if( mcls_pParam->Check_ImgDsp( pD->datas.nTid, ii ) ) {
				if (0 < pD->image_len[ii]) {
					int nBufSize = GET_JUST_SIZE(pD->image_len[ii], 512);
					pImg->nSize[ii] = nBufSize;
					pImg->pImg[ii] = (BYTE*)VirtualAlloc(NULL, nBufSize, MEM_COMMIT, PAGE_READWRITE);		// 高速書込み用にセクタ単位で領域を確保

					memcpy(pImg->pImg[ii], &pD->image[nOffset], pD->image_len[ii]);
					strcpy(pImg->cPath[ii], cPath[ii]);

					pImg->pMemPath[ii] = pPath[ii];				// 共有メモリパス
				}
			}
			nOffset += pD->image_len[ii];
		}

		//// 登録
		BOOL bRetc;							// 登録結果

//		if(0 != pD->data[0].nFrameDefMax) {		
		if (1) {
				if( 10 < mque_pImgSave->GetCount() ) {
				bRetc = mque_pImgSave->AddToHead(pImg);			// たまっていると リアルタイム表示のときに、毎回画像表示が間に合わない対策
			} else {
				bRetc = mque_pImgSave->AddToTail(pImg);
			}
		} else {
			if( 10 < mque_pImgSaveLow->GetCount() ) {
				bRetc = mque_pImgSaveLow->AddToHead(pImg);
			} else {
				bRetc = mque_pImgSaveLow->AddToTail(pImg);
			}
		}

		if( ! bRetc ) {
			for(int ii=0; ii<pImg->nSetCnt; ii++ ) { SAFE_DELETE(pImg->pImg[ii]); }
			delete pImg;
			nImgAdd = -9;
		}

	}

	//// NGの場合
	if( 0 != nImgAdd ) {
		// 画像取り消し
		for(int ii=0; ii<MAX_HT1_CAMANGLE; ii++) {
			pPath[ii][0] = 0x00;		// 念のために共有メモリの画像パスをクリア
		}
		// ガイダンス
		if( -9 == nImgAdd) {
			LOG(em_ERR), "[%s] 疵画像登録破棄 [%s] index=%d, No=%d, T=%d, G=%d", 
				my_sThreadName, mtbl_pDefectInf->rec[nRec].KizukenNo, nIndex, pD->datas.nKizuNo, pD->datas.nTid, pD->datas.nGid);
		} else if( -1 == nImgAdd) {
			LOG(em_WAR), "[%s] 疵画像登録スキップ [%s] index=%d, No=%d", 
				my_sThreadName, mtbl_pDefectInf->rec[nRec].KizukenNo, nIndex, pD->datas.nKizuNo);
		} else if (-99 == nImgAdd) {
			//LOG(em_WAR), "[%s] 疵画像登録無し [%s] index=%d, No=%d",
			//	my_sThreadName, mtbl_pDefectInf->rec[nRec].KizukenNo, nIndex, pD->datas[0].nKizuNo);
		}
	}


	//================================================
	// ポインタ更新
	mtbl_pDefectInf->rec[nRec].num = nIndex+1;	
}


//------------------------------------------
// 重欠陥情報セット
// TO_SO_DATA_DETECT* pD	疵情報
// char* pKizukenNo			管理No
// int nRec					テーブルレコードNo (0オリジン)
// int nMen					表裏区分
//------------------------------------------
void DataSetManager::SetDefLst(TO_SO_DATA_DETECT const* pD, char const* pKizukenNo, const int nRec, const int nMen)
{
	int nNewRec = mtbl_pDefList->head.nPos[nMen] + 1;

	if (MAX_DEFLIST_BUF < nNewRec) {
		nNewRec = 1;
	}

	int ick;			// 上書き可能データ検索用
	int iDelRec;		// 最古の普通疵のレコード位置格納用(0オリジン) & データシフト実施フラグ兼用
	iDelRec = -1;
	if (0 < mtbl_pDefList->data[nMen].defect[nNewRec - 1].typ.data.nKizuNo ) {
		if (1 == mtbl_pDefList->data[nMen].defect[nNewRec - 1].typ.data.nModeDeflist) {
			// 最古の普通疵を探す
			ick = nNewRec-1;
			for (int ii = 0; ii < MAX_DEFLIST_BUF; ii++) {
				if (0 == mtbl_pDefList->data[nMen].defect[ick].typ.data.nModeDeflist) {
					// 最古の普通疵レコード位置セット
					iDelRec = ick;
					break;
				}
				ick = ick + 1;
				if (MAX_DEFLIST_BUF <= ick) ick = 0;
			}
			if (-1 == iDelRec) {
				LOG(em_ERR), "上書き可能データ無し[管理No = %s] 面=%d No=%d", pKizukenNo, nMen, mtbl_pDefList->data[nMen].defect[nNewRec - 1].typ.data.nKizuNo);
				// syslog(???, "[疵検管理No = %s]", pHead->KizukenNo);
				return;
			}
		}
	}

	// データシフト実施(iDelRec=-1の時は不要)
	if (-1 != iDelRec) {
		DataShiftDefList(nMen, iDelRec, nNewRec - 1);
	}
	// 初期化
	memset(&mtbl_pDefList->data[nMen].defect[nNewRec - 1], 0x00, sizeof(mtbl_pDefList->data[nMen].defect[nNewRec - 1]));
	// データセット
	memcpy(mtbl_pDefList->data[nMen].defect[nNewRec - 1].KizuKenNo, pKizukenNo, SIZE_KIZUKEN_NO);
	memcpy(mtbl_pDefList->data[nMen].defect[nNewRec - 1].CoilNo, mtbl_pCoilInf->rec[nRec].coilinf.base.cCoilNoIn, SIZE_COIL_NO + 1);
	memcpy(mtbl_pDefList->data[nMen].defect[nNewRec - 1].MeiNo, mtbl_pCoilInf->rec[nRec].coilinf.base.cMeiNo, SIZE_MEI_NO + 1);

	//================================================
	// 前準備
	DSP_TYP_DEFECTINF& pSet = mtbl_pDefList->data[nMen].defect[nNewRec - 1].typ;

	char  cPath[MAX_HT1_CAMANGLE][32];
	char* pPath[MAX_HT1_CAMANGLE];						// 共有メモリパス
	pPath[0] = pSet.image_path_s;
	pPath[1] = pSet.image_path_r;
	pPath[2] = pSet.image_path_r2;
	pPath[3] = pSet.image_path_r3;

	//================================================
	// 疵情報セット
	memcpy(&pSet.data, &pD->datas, sizeof(DETECT_BASE_DATA));

	int nFileNo = 0;
	// 重欠陥一覧画像ファイル保存用ファイルNo取得
	int nNoWk;
	nNoWk = m_nLstSaveNo[nMen] + 1;
	if (nNoWk > LST_SAVE_NO_MAX) {
		nNoWk = 1;
	}
	GetDefListFileNo(nMen, nNoWk, &nFileNo);
	m_nLstSaveNo[nMen] = nFileNo;

	for (int ii = 0; ii<MAX_CAMANGLE; ii++) {
		sprintf(cPath[ii], "%s\\%d_%d_%d.jpg", (LPCTSTR)this->GetFolderPath(m_cPath_Lst, -1), nMen, nFileNo, ii);	// 画像は疵連番にする
	}
	LOG(em_MSG), "[%s] 重欠陥情報 men=%d rec=%d FileNo=%d", my_sThreadName, nMen, nNewRec, nFileNo);

	//================================================
	// 疵画像登録
	int	nImgAdd = 0;				// 0:画像登録可  -9:代表疵破棄  -1:リストに隠れる疵破棄  -99:該当PC画像保存無し

									//--------------------------------
									//// 事前チェック
									//if( 0 != pD->data[0].nFrameDefMax) {
	if (1) {
		if (0 == mque_pImgSave->GetFreeCount()) nImgAdd = -9;
	}
	else {
		if (0 == mque_pImgSaveLow->GetFreeCount()) nImgAdd = -1;
	}
	// 画像登録有無
	if (1 == m_nLstImgSaveOff) nImgAdd = -99;

	//--------------------------------
	//// 画像登録
	if (0 == nImgAdd) {
		// 生成
		int nOffset = 0;

		FileSaveManager::TYP_SAVE_IMG* pImg = new FileSaveManager::TYP_SAVE_IMG;
		memset(pImg, 0x00, sizeof(FileSaveManager::TYP_SAVE_IMG));

		//// セット
		pImg->nSetCnt = MAX_IMAGE_CNT;
		for (int ii = 0; ii<MAX_IMAGE_CNT; ii++) {
			// 表示対象疵か？
			if (mcls_pParam->Check_ImgDsp(pD->datas.nTid, ii)) {
				if (0 < pD->image_len[ii]) {
					int nBufSize = GET_JUST_SIZE(pD->image_len[ii], 512);
					pImg->nSize[ii] = nBufSize;
					pImg->pImg[ii] = (BYTE*)VirtualAlloc(NULL, nBufSize, MEM_COMMIT, PAGE_READWRITE);		// 高速書込み用にセクタ単位で領域を確保

					memcpy(pImg->pImg[ii], &pD->image[nOffset], pD->image_len[ii]);
					strcpy(pImg->cPath[ii], cPath[ii]);

					pImg->pMemPath[ii] = pPath[ii];				// 共有メモリパス
				}
			}
			nOffset += pD->image_len[ii];
		}

		//// 登録
		BOOL bRetc;							// 登録結果

											//		if(0 != pD->data[0].nFrameDefMax) {		
		if (1) {
			if (10 < mque_pImgSave->GetCount()) {
				bRetc = mque_pImgSave->AddToHead(pImg);			// たまっていると リアルタイム表示のときに、毎回画像表示が間に合わない対策
			}
			else {
				bRetc = mque_pImgSave->AddToTail(pImg);
			}
		}
		else {
			if (10 < mque_pImgSaveLow->GetCount()) {
				bRetc = mque_pImgSaveLow->AddToHead(pImg);
			}
			else {
				bRetc = mque_pImgSaveLow->AddToTail(pImg);
			}
		}

		if (!bRetc) {
			for (int ii = 0; ii<pImg->nSetCnt; ii++) { SAFE_DELETE(pImg->pImg[ii]); }
			delete pImg;
			nImgAdd = -9;
		}

	}

	//// NGの場合
	if (0 != nImgAdd) {
		// 画像取り消し
		for (int ii = 0; ii<MAX_HT1_CAMANGLE; ii++) {
			pPath[ii][0] = 0x00;		// 念のために共有メモリの画像パスをクリア
		}
		// ガイダンス
		if (-9 == nImgAdd) {
			LOG(em_ERR), "[%s] 重欠陥疵画像登録破棄 [%s] Rec=%d, No=%d, T=%d, G=%d",
				my_sThreadName, mtbl_pDefList->data[nMen].defect[nNewRec - 1].KizuKenNo, nNewRec, pD->datas.nKizuNo, pD->datas.nTid, pD->datas.nGid);
		}
		else if (-1 == nImgAdd) {
			LOG(em_WAR), "[%s] 重欠陥疵画像登録スキップ [%s] Rec=%d, No=%d",
				my_sThreadName, mtbl_pDefList->data[nMen].defect[nNewRec - 1].KizuKenNo, nNewRec, pD->datas.nKizuNo);
		}
		else if (-99 == nImgAdd) {
			//LOG(em_WAR), "[%s] 重欠陥疵画像登録無し [%s] Rec=%d, No=%d",
			//	my_sThreadName, mtbl_pDefList->data[nMen].defect[nNewRec - 1].KizuKenNo, nNewRec, pD->datas[0].nKizuNo);
		}
	}

	//================================================
	// ポインタ更新
	mtbl_pDefList->head.nPos[nMen] = nNewRec;
}
////------------------------------------------
//// 重欠陥一覧画像ファイル保存用ファイルNo取得
//// int nO_U					表裏区分
//// int nRec					テーブルレコードNo (1オリジン)
//// int* nFileNo			    重欠陥一覧画像ファイル保存用ファイルNo(1オリジン)
////------------------------------------------
//void DataSetManager::GetDefListFileNo(int nO_U, int nRec, int* nFileNo)
//{
//	//================================================
//	// 前準備
//
//	char  cPath[MAX_HT1_CAMANGLE][32];
//	char* pPath[MAX_HT1_CAMANGLE];						// 共有メモリパス
//
//	*nFileNo = nRec;
//
//	for (int ii = 0; ii<MAX_CAMANGLE; ii++) {
//		sprintf(cPath[ii], "%s\\%d_%d_%d.jpg", this->GetFolderPath(m_cPath_Lst, -1), nO_U, nRec, ii);	// 画像は疵連番にする
//	}
//
//	bool bCk = false;
//
//	// 該当レコード番号は使用中か？
//	for (int ii = 0; ii < HY_LST_DEFLIST_MAX; ii++) {
//		if (nRec - 1 == ii) continue;
//		DSP_TYP_DEFECTINF& pSet = mtbl_pDefList->data[nO_U].defect[ii].typ;
//		pPath[0] = pSet.image_path_s;
//		pPath[1] = pSet.image_path_r;
//		pPath[2] = pSet.image_path_r2;
//		pPath[3] = pSet.image_path_r3;
//
//		for (int jj = 0; jj < MAX_CAMANGLE; jj++) {
//			if (0 == strcmp(cPath[jj], pPath[jj])) {
//				// 使用中
//				bCk = true;
//				break;
//			}
//		}
//		// 使用中だったので検索終了
//		if (bCk) {
//			break;
//		}
//	}
//	// 未使用だった。
//	if (!bCk) {
//		*nFileNo = nRec;
//		//LOG(em_MSG), "[%s] レコード番号使用 rec=%d FileNo=%d", my_sThreadName, nRec, *nFileNo);
//		return;
//	}
//
//	// 空きを探す
//	for (int ii = 0; ii < HY_LST_DEFLIST_MAX; ii++) {
//		if (nRec - 1 == ii) continue;
//		for (int jj = 0; jj<MAX_CAMANGLE; jj++) {
//			sprintf(cPath[jj], "%s\\%d_%d_%d.jpg", this->GetFolderPath(m_cPath_Lst, -1), nO_U, ii+1, jj);	// 画像は疵連番にする
//		}
//		bCk = false;
//		for (int jj = 0; jj < HY_LST_DEFLIST_MAX; jj++) {
//			DSP_TYP_DEFECTINF& pSet = mtbl_pDefList->data[nO_U].defect[jj].typ;
//			pPath[0] = pSet.image_path_s;
//			pPath[1] = pSet.image_path_r;
//			pPath[2] = pSet.image_path_r2;
//			pPath[3] = pSet.image_path_r3;
//			for (int kk = 0; kk < MAX_CAMANGLE; kk++) {
//				if (0 == strcmp(cPath[kk], pPath[kk])) {
//					// 使用中
//					bCk = true;
//					break;
//				}
//			}
//
//			if (bCk) {
//				// 使用中なので次を検索
//				break;
//			}
//		}
//		if ( !bCk) {
//			// 未使用なのでこの番号を使用
//			*nFileNo = ii + 1;
//			//LOG(em_MSG), "[%s] 空き番号使用 rec=%d FileNo=%d", my_sThreadName, nRec, *nFileNo);
//			return;
//		}
//	}
//}

//------------------------------------------
// 重欠陥一覧画像ファイル保存用ファイルNo取得
// int nO_U					表裏区分
// int nNo					保存用ファイルNo 候補(1オリジン)
// int* nFileNo			    重欠陥一覧画像ファイル保存用ファイルNo(1オリジン)
//------------------------------------------
void DataSetManager::GetDefListFileNo(int nO_U, int nNo, int* nFileNo)
{
	//================================================
	// 前準備

	char  cPath[MAX_HT1_CAMANGLE][32];
	char* pPath[MAX_HT1_CAMANGLE];						// 共有メモリパス
	bool bCk = false;
	int nWk;
	
	nWk = nNo;
	if (1 > nWk || LST_SAVE_NO_MAX < nWk) {
		nWk = 1;
	}

	// 空きを探す
	for (int ii = 0; ii < LST_SAVE_NO_MAX; ii++) {

		for (int jj = 0; jj<MAX_CAMANGLE; jj++) {
			sprintf(cPath[jj], "%s\\%d_%d_%d.jpg", (LPCTSTR)this->GetFolderPath(m_cPath_Lst, -1), nO_U, nWk, jj);	// 画像は疵連番にする
		}
		bCk = false;
		for (int jj = 0; jj < HY_LST_DEFLIST_MAX; jj++) {
			DSP_TYP_DEFECTINF& pSet = mtbl_pDefList->data[nO_U].defect[jj].typ;
			pPath[0] = pSet.image_path_s;
			pPath[1] = pSet.image_path_r;
			pPath[2] = pSet.image_path_r2;
			pPath[3] = pSet.image_path_r3;
			for (int kk = 0; kk < MAX_CAMANGLE; kk++) {
				if (0 == strcmp(cPath[kk], pPath[kk])) {
					// 使用中
					bCk = true;
					break;
				}
			}

			if (bCk) {
				// 使用中なので次を検索
				break;
			}
		}
		if (!bCk) {
			// 未使用なのでこの番号を使用
			*nFileNo = nWk;
			//LOG(em_MSG), "[%s] 空き番号使用 rec=%d FileNo=%d", my_sThreadName, nRec, *nFileNo);
			return;
		}
		nWk = nWk + 1;
		if (LST_SAVE_NO_MAX < nWk) {
			nWk = 1;
		}
	}
}

//------------------------------------------
// 重欠陥一覧表示疵情報データシフト処理
// int nO_U					表裏区分(0:表, 1:裏)
// int nStartPos            シフト開始レコード位置(0オリジン)
// int nEndPos              シフト終了レコード位置(0オリジン)
//------------------------------------------
void DataSetManager::DataShiftDefList(int nO_U, int nStartPos, int nEndPos)
{
	int iNewRec;	// コピー先レコード位置(0オリジン)
	int iOldRec;	// コピー元レコード位置(0オリジン)
	int	ii;			// ループカウンタ

					// シフト不要
	if (nStartPos == nEndPos) {
		// 最終レコード初期化
		memset(&mtbl_pDefList->data[nO_U].defect[nEndPos], 0x00, sizeof(mtbl_pDefList->data[nO_U].defect[nEndPos]));
		return;
	}

	iNewRec = nStartPos;
	iOldRec = iNewRec - 1;
	if (0 > iOldRec) iOldRec = MAX_DEFLIST_BUF - 1;

	// １レコードシフト
	for (ii = 0; ii < MAX_DEFLIST_BUF; ii++) {
		// データコピー
		memcpy(&mtbl_pDefList->data[nO_U].defect[iNewRec], &mtbl_pDefList->data[nO_U].defect[iOldRec], sizeof(mtbl_pDefList->data[nO_U].defect[iNewRec]));

		// 次編集レコードセット
		iNewRec = iNewRec - 1;
		if (0 > iNewRec) iNewRec = MAX_DEFLIST_BUF - 1;
		iOldRec = iNewRec - 1;
		if (0 > iOldRec) iOldRec = MAX_DEFLIST_BUF - 1;
		if (iNewRec == nEndPos) break;
	}
	// 最終レコード初期化
	memset(&mtbl_pDefList->data[nO_U].defect[nEndPos], 0x00, sizeof(mtbl_pDefList->data[nO_U].defect[nEndPos]));

}

//------------------------------------------
// 疵マップの情報受信状態にデータセット
// int nRec	該当コイルのレコード位置 (0オリジン)
// int nMen	面
// int row  最小行レコード位置 (0オリジン)
// int st	ステータス状態
//------------------------------------------
void DataSetManager::SetRecvStatus(const int nRec, const int nMen, const int row, const int st)
{
	int	  y;				// マップ格納行位置 (0オリジン)
	for(int nStage=0; nStage<MAP_STAGE_NUM; nStage++) {
		// 対象位置算出
		y = GetStageIndex(nStage, row);

		// 判定
		bool bOk = false;
		int nWk = mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[y].typ.nEnable;
		if( 0 == nWk ) {					// 未セット
			bOk = true;							// 無条件にセットOK
		} else if(  0 < nWk ){				// 正常の方
			if(st>0 && nWk < st) bOk = true;		// より正常
			if(st<0) bOk = true;					// 異常

		} else if(  0 > nWk ){				// 異常の方
			if(st<0 && nWk > st) bOk = true;		// より異常
		}
		
		// セット
		if(bOk) mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[y].typ.nEnable = st;
	}
}


//------------------------------------------
// 行情報にデータセット
// int nRec	該当コイルのレコード位置 (0オリジン)
// int nMen	面
// int nLen	現在距離 [m]
//------------------------------------------
void DataSetManager::SetRow_Inf(const int nRec, const int nMen,const int nLen)
{
	if(0 > nRec) return;
	bool nSkip = false;						// 行抜け有り時 true

	for(int nStage =0; nStage<MAP_STAGE_NUM; nStage++) {
		int nRow = GetStageLenToIndex(nStage, nLen);			// 対象位置算出

		if (0 == nStage) {
			SetRecvStatus(nRec, nMen, nRow, DIV_HY_RECVST_OK);	// 情報受信状態 正常
		}

		DSP_TYP_ROW& pWk = mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRow].typ;

		// 詳細情報セット
		if(pWk.emKadou < mtbl_pStatus->emKadou )			pWk.emKadou = mtbl_pStatus->emKadou;				// 稼動状態
		if (pWk.emDspKensa < mtbl_pStatus->status.kensa[nMen]) pWk.emDspKensa = mtbl_pStatus->status.kensa[nMen];		// 検査状態
		if (pWk.emDspKiki  < mtbl_pStatus->status.kiki[nMen]) pWk.emDspKiki = mtbl_pStatus->status.kiki[nMen];		// 機器状態

	}
}

//------------------------------------------
// 出側オフセット位置取得
// int nRec			該当コイルのレコード位置 (0オリジン)
// int * pOutLen	出側オフセット位置(最小/小/大)
//------------------------------------------
void DataSetManager::GetOffsetOutLen(const int nRec, int * pOutLen)
{

	// 初期値をセットしておく。
	for (int nStage = 0; nStage<MAP_STAGE_NUM; nStage++) {
		pOutLen[nStage] = 0;
	}

	//// ポインタ情報テーブルをチェック（上流→下流）
	int		ii = 1;		// 次のレコードから
	while (1) {
		int nWk = GetRecIndex(nRec, ii);
		if (ii && (nWk == nRec)) {
			// 周回した(ここまで来ることは無いと思うが。。。）
			break;
		}
		int nSpm = 0;
		switch (mtbl_pPoint->pnt[nWk].emDispStatus) {
		case DIV_DSP_OK:
		case DIV_DSP_STANDBY:
			for (int nStage=0; nStage<MAP_STAGE_NUM; nStage++) {
				// 検査終了行を取得
				int nRowLast = mtbl_pPoint->pnt[nWk].typ.nRowLast[nStage];
				if (0 == nRowLast) {
					// 検査完了前なので、コイル情報の出側コイル長から終端を算出する。
					COIL_BASEDATA &pWk = mtbl_pCoilInf->rec[nWk].coilinf.base;
					nRowLast = GetStageLenToIndex(nStage, pWk.nLenOut);
				} else {
					nRowLast -= 1;	// 検査終了行は1オリジンのため、1を引いておく。
				}
				pOutLen[nStage] = mtbl_pMapRow->rec[nWk].stage[nStage].men[0].row[nRowLast].typ.nLen_o;
				// 対象行でカットされていたら０にする。
				if (0 != mtbl_pMapRow->rec[nWk].stage[nStage].men[0].row[nRowLast].typ.nCut) {
					pOutLen[nStage] = 0;
				}
			}
			return;
			break;

		case DIV_DSP_NG:
			// 次(下流)のレコードへ
			ii+=1;
			break;

		case DIV_DSP_INITIALIZE:
		default:
			return;
			break;
		}
	}

	return;
}

//------------------------------------------
// 出側距離の再設定
// int nRec			出側距離を再設定するレコード開始位置 (0オリジン)
//------------------------------------------
// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
//void DataSetManager::RecalOutLen(const int nRec)
//{

void DataSetManager::RecalOutLen(const int nRec, bool bDefect)
{
// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------

	// 更新開始
	mtbl_pPoint->nRefresh = true;

	int nWriteRec = mtbl_pPoint->nRecWriteBase;
	if (TBL_HY_REC_MAX <= nWriteRec) nWriteRec = 0;

	//// ポインタ情報テーブルをチェック (下流→上流）
	int		ii = 0;
	while (1) {
		int nWk = GetRecIndex(nRec, ii);

		if (nWriteRec == nWk) {
			// 書き込みレコードになったら終了
			break;
		}

		if (ii && (nWk == nRec)) {
			// 周回した(ここまで来ることは無いと思うが。。。）
			break;
		}

		int	nOutOffset[MAP_STAGE_NUM];
		GetOffsetOutLen(nWk, nOutOffset);		// 出側オフセット位置取得

		for (int nStage = 0; nStage < MAP_STAGE_NUM; nStage++) {
			for (int nMen = 0; nMen < NUM_MEN; nMen++) {
				int nMaxIndex = GET_JUST_INDEX(COIL_LEN_MAX, mcls_pParam->GetCommonMast().nMapDsp[nStage]);	// 最大レコード位置
				int nOutLen = nOutOffset[nStage];	// 出側開始は出側オフセット位置。
				for (int ii = 0; ii <= nMaxIndex; ii++) {
					DSP_TYP_ROW & pWk = mtbl_pMapRow->rec[nWk].stage[nStage].men[nMen].row[ii].typ;
					// 出側距離セット
					nOutLen += mcls_pParam->GetCommonMast().nMapDsp[nStage];
					pWk.nLen_o = nOutLen;
					if (pWk.nCut) {
						// シャーカットされた行なので、次の行から出側距離をリセット。
						nOutLen = 0;
					}
				}
			}
		}

// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>	
		if (true == bDefect)
		{
			// 疵情報の再設定 
			for (int nDefIndex = 0; nDefIndex < mtbl_pDefectInf->rec[nWk].num; nDefIndex++)
			{			
				GetDeliveryPosY(nWk, mtbl_pDefectInf->rec[nWk].pnt[nDefIndex].typ.data.y, &mtbl_pDefectInf->rec[nWk].pnt[nDefIndex].typ.data.y_out);
			}

			// 前工程疵情報の再設定
			for (int nMen = 0; nMen < NUM_MEN; nMen++ )
			{
				for (int nDefIndex = 0; nDefIndex < mtbl_pMaeInf->rec[nWk].men[nMen].num; nDefIndex++)
				{
					GetDeliveryPosY(nWk, mtbl_pMaeInf->rec[nWk].men[nMen].maeinf[nDefIndex].ff.nY, &mtbl_pMaeInf->rec[nWk].men[nMen].maeinf[nDefIndex].ff.nY_Out);
				}
			}			
		}
// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------

		// 次(上流)のレコードへ
		ii -= 1;
	}

	// 更新終了
	mtbl_pPoint->nRefresh = false;
	return;
}

// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
/// <summary>
/// 疵情報の出側位置を取得
/// </summary>
/// <param name="nRec">該当コイルのレコード位置 (0オリジン)</param>
/// <param name="dPosY">入側位置</param>
/// <param name="pdDeliveryPosY">算出した出側位置</param>
void DataSetManager::GetDeliveryPosY(const int nRec, float dPosY, float* pdDeliveryPosY)
{
	// 初期値
	*pdDeliveryPosY = dPosY;

	// 自コイルから上流に戻り、直近のカット位置から出側距離を算出する。	

	// 自コイル
	for (int ii = 0; ii < mtbl_pCoilResult->rec[nRec].nCutCnt; ii++)
	{
		int nCutIndex = mtbl_pCoilResult->rec[nRec].nCutCnt - ii - 1;
		if (DIV_CUT_FCUT == mtbl_pCoilResult->rec[nRec].cut[nCutIndex].nMode)
		{
			bool bCut = false;
			int nCutPos = mtbl_pCoilResult->rec[nRec].cut[nCutIndex].nPos;
			if (dPosY >= nCutPos)
			{
				*pdDeliveryPosY = dPosY - nCutPos;				
				bCut = true;
			}

			if ( true == bCut )
			{
				return;
			}
		}
	}

	// 上流
	for (int ii = 1; ii < TBL_HY_REC_MAX; ii++)
	{
		int nTargetRec = GetRecIndex(nRec, ii);
		if (DIV_DSP_OK == mtbl_pPoint->pnt[nTargetRec].emDispStatus || 
			DIV_DSP_STANDBY == mtbl_pPoint->pnt[nTargetRec].emDispStatus )
		{

			// コイル長の取得
			int nCoilLen = 0;
			if (0 == mtbl_pCoilResult->rec[nTargetRec].nLen)
			{
				// 実測長が無い場合、通板距離から算出する。※通常、コイル切替をしているから実測長が有る筈。
				int nWk = 0;
				for (int jj = 0; jj < MAX_DISP_POS; jj++)
				{
					if (nWk < mtbl_pPoint->pnt[nTargetRec].typ.nLenDisp[jj])
					{
						nWk = mtbl_pPoint->pnt[nTargetRec].typ.nLenDisp[jj];
					}
				}
				nCoilLen = nWk * 1000;		// ※[m]->[mm]
			}
			else
			{
				nCoilLen = mtbl_pCoilResult->rec[nTargetRec].nLen;
			}

			// 該当コイルにカットは有るか？
			int nTargetCutPos = 0;
			if ( 0 < mtbl_pCoilResult->rec[nTargetRec].nCutCnt )
			{
				int nCutIndex = mtbl_pCoilResult->rec[nTargetRec].nCutCnt - 1;
				if (DIV_CUT_FCUT == mtbl_pCoilResult->rec[nTargetRec].cut[nCutIndex].nMode )
				{
					nTargetCutPos = mtbl_pCoilResult->rec[nTargetRec].cut[nCutIndex].nPos;
				}
			}

			if (0 < nTargetCutPos)
			{
				// カット有なら差分を加算
				*pdDeliveryPosY = *pdDeliveryPosY + (nCoilLen - nTargetCutPos < 0 ? 0 : nCoilLen - nTargetCutPos);
				return;
			}
			else
			{
				// カット無ならコイル長を加算
				*pdDeliveryPosY = *pdDeliveryPosY + nCoilLen;
			}
		}
	}

	// ここまで来たら上流コイルもカットも無いので入側距離にする。
	*pdDeliveryPosY = dPosY;
}

/// <summary>
/// 前工程疵情報 出側位置編集 
/// </summary>
/// <param name="pQue"></param>
void  DataSetManager::EditMaeDefectOutLen(TYP_DATASET_REQMAE* pQue)
{
	int nRec = pQue->nRec;

	LOG(em_MSG), "[%s] 前工程疵情報 出側位置編集 Rec=%d num=%d/%d",
		my_sThreadName, nRec, mtbl_pMaeInf->rec[nRec].men[0].num, mtbl_pMaeInf->rec[nRec].men[1].num);

	for ( int nMen = 0; nMen < NUM_MEN; nMen++ )
	{
		for ( int nDefIdx = 0; nDefIdx < mtbl_pMaeInf->rec[nRec].men[nMen].num; nDefIdx++ ) 			
		{
			DSP_TYP_MAE_FFINF* pDef = &mtbl_pMaeInf->rec[nRec].men[nMen].maeinf[nDefIdx].ff;

			// 出側距離セット
			GetDeliveryPosY(nRec, pDef->nY, &pDef->nY_Out);
		}
	}
}
// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------

// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
/// <summary>
/// 長周期情報編集
/// </summary>
/// <param name="pQue"></param>
void DataSetManager::EditLongPdInf(TYP_DATASET_REQLONGPD* pQue)
{
	LOG(em_INF), "[%s][長周期発生通知] (管理No=%s)", my_sThreadName, pQue->cKanriNo);

	//================================================
	// 整合性判定

	// コイルレコードの取得
	int nRec = CheckKanriNo(pQue->cKanriNo, mtbl_pPoint);
	if( 0 > nRec) {
		LOG(em_ERR), "[%s][長周期発生通知] 該当レコード無し", my_sThreadName);
		return;
	}

	mtbl_pLongPd->rec[nRec].nLongPdOccurrenceFlag = 1;
	LOG(em_INF), "[%s][長周期発生通知] (rec=%d) 長周期発生 ", my_sThreadName, nRec);

	// ガイダンス判定
	SetLongPdGuidance();
}

/// <summary>
/// 長周期ガイダンスセット
/// </summary>
void DataSetManager::SetLongPdGuidance()
{
	if( 0 == mcls_pParam->GetCommonMast_LongPd().nLongPd_GuidanceAvailable ) 		
	{
		// ガイダンスなし
		mtbl_pLongPd->nGuidanceFlag = 0;
		return;
	}

	if( 0 < mtbl_pStatus->status.nCnt ) 		
	{
		// 何かしらの異常が有るならガイダンス表示なし
		mtbl_pLongPd->nGuidanceFlag = 0;
		return;
	}

	int nRec = mtbl_pPoint->nRecDispBase;
	if( 0 >= nRec ) 		
	{
		// 表示コイル無し
		mtbl_pLongPd->nGuidanceFlag = 0;
		return;
	}

	bool bExist = false;
	for( int ii = 0; ii < 2; ii++ ) 		
	{
		int nCheckRec = GetRecIndex(nRec - 1, ii);

		if( 1 == mtbl_pLongPd->rec[nCheckRec].nLongPdOccurrenceFlag ) 		
		{
			// 長周期欠陥あり
			bExist = true;
			break;
		}
	}

	if  ( true == bExist )
	{
		// ガイダンス表示
		mtbl_pLongPd->nGuidanceFlag = 1;
	}
	else 		
	{
		mtbl_pLongPd->nGuidanceFlag = 0;
	}
}
// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

//------------------------------------------
// 行位置のステータス(稼動状態/検査状態/機器状態)を指定行まで反映
// int nRec     テーブルレコード位置 (0オリジン)
// int nStage   行ステージ (0:最小/1:小/2:大)
// int nMen	    面 (0:表/1:裏)
// int nBaseRow 開始行 (0オリジン)
// int nBaseRow 終了行 (0オリジン)
// 戻り値 -1:算出エラー  0～:出側コイル先端からの距離 [mm]
//------------------------------------------
void DataSetManager::RefRowStatus(const int nRec, const int nStage, const int nMen, const int nBaseRow, const int nEndRow)
{
	// 現在書き込み行の行情報（稼動状態/検査状態/機器状態)をコピー
	for (int nRow=nBaseRow+1; nRow<=nEndRow; nRow++) {
		DSP_TYP_ROW& pSrc = mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRow-1].typ;
		DSP_TYP_ROW& pTag = mtbl_pMapRow->rec[nRec].stage[nStage].men[nMen].row[nRow].typ;
		if(pTag.emKadou < pSrc.emKadou)	pTag.emKadou = pSrc.emKadou;					// 稼動状態
		if(pTag.emDspKensa < pSrc.emDspKensa)	pTag.emDspKensa = pSrc.emDspKensa;		// 検査状態
		if(pTag.emDspKiki  < pSrc.emDspKiki)	pTag.emDspKiki = pSrc.emDspKiki;		// 機器状態

	}

	return;
}


