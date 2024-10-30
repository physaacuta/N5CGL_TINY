#include "StdAfx.h"
#include "CoilManager.h"

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
CoilManager::CoilManager(void) :
CoilBaseManager()
{
}

//------------------------------------------
// デストラクタ
//------------------------------------------
CoilManager::~CoilManager(void)
{
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 情報収集 行単位関係

//==========================================
// 現コイル情報に検査状態をセット [←PlgManager]
//------------------------------------------
// int men			
// int rec			
// EM_DIV_KENSA st	
//==========================================
void CoilManager::SetData_StKensa(int men, int rec, EM_DIV_KENSA st)
{
	if( m_pCoilKenR[men]->emKensa[men][rec] < st )
	{
		m_pCoilKenR[men]->emKensa[men][rec] = st;
	}

	if( m_pCoilKenR[men]->nSetRec_St[men] >= rec )	return;
	m_pCoilKenR[men]->nSetRec_St[men] = rec;
	//LOG(em_MSG), "[%s] 現コイル情報に検査状態をセット 疵検No=%s rec=%d 書込行=%d SetRec[0]=%d SetRec[1]=%d SpmUmu=%d men=%d", my_sThreadName, m_pCoilKenV[men]->cKizukenNo, m_pCoilKenV[men]->rec, rec, m_pCoilKenV[men]->nSetRec_St[SpmUmu][0], m_pCoilKenV[men]->nSetRec_St[SpmUmu][1], SpmUmu, men);
}

//==========================================
// 現コイル情報に機器状態をセット [←PlgManager]
//------------------------------------------
// int			men	: 表裏(0:表,1:裏)
// int			rec	: レコード
// EM_DIV_KIKI	st	: 機器状態
//==========================================
void CoilManager::SetData_StKiki(int men, int rec, EM_DIV_KIKI st)
{ 
	if( m_pCoilKenR[men]->emKiki[men][rec] < st ) {
		m_pCoilKenR[men]->emKiki[men][rec] = st;
	}

	if( m_pCoilKenR[men]->nSetRec_St[men] >= rec ) return;
	m_pCoilKenR[men]->nSetRec_St[men] = rec;
	//LOG(em_MSG), "[%s] 現コイル情報に機器状態をセット 疵検No=%s rec=%d 書込行=%d SetRec[0]=%d SetRec[1]=%d SpmUmu=%d men=%d", my_sThreadName, m_pCoilKenV[men]->cKizukenNo, m_pCoilKenV[men]->rec, rec, m_pCoilKenV[men]->nSetRec_St[SpmUmu][0], m_pCoilKenV[men]->nSetRec_St[SpmUmu][1], SpmUmu, men);
}

//==========================================
// 現コイル情報にエッジ位置をセット [←HtRecv]
//------------------------------------------
// int		men		: 表裏(0:表,1:裏)
// int		rec		: レコード
// float*	dEdge	: エッジ位置
//==========================================
void CoilManager::SetData_Edge(int men, int rec, float* dEdge)
{
	for (int ii = m_pCoilKenV[men]->nSetRec_Edge[men] + 1; ii <= rec; ii++) {
		m_pCoilKenV[men]->dEdge[men][ii][0] = dEdge[0];
		m_pCoilKenV[men]->dEdge[men][ii][1] = dEdge[1];
	}

	if (m_pCoilKenV[men]->nSetRec_Edge[men] >= rec) return;
	m_pCoilKenV[men]->nSetRec_Edge[men] = rec;
	//LOG(em_MSG), "[%s] 現コイル情報にエッジ位置をセット 疵検No=%s rec=%d 書込行=%d SetRec[0]=%d SetRec[1]=%d SpmUmu=%d men=%d Edge=%f/%f", my_sThreadName, m_pCoilKenV[men]->cKizukenNo, m_pCoilKenV[men]->rec, rec, m_pCoilKenV[men]->nSetRec_St[SpmUmu][0], m_pCoilKenV[men]->nSetRec_St[SpmUmu][1], SpmUmu, men, dEdge[0], dEdge[1]);
}

//==========================================
// 現コイル情報に行単位の情報をセット [←HtRecv]{ 
//------------------------------------------
// int				men		: 表裏(0:表,1:裏)
// int				rec		: レコード
// int				index	: セット位置
// MARK_ROW_BUF*	buf		: マーキング情報(行単位)
//==========================================
void CoilManager::SetData_Row_V	(int men, int rec, int index, int val)
{
	m_pCoilKenV[men]->nRowVal[men][rec][index] = val;
	//LOG(em_MSG), "[%s] 現コイル情報に行単位の情報をセット  rec=%d SetRec[0]=%d SetRec[1]=%d men=%d", my_sThreadName, m_pCoilKenV[men]->rec, m_pCoilKenV[men]->nSetRec_St[0], m_pCoilKenV[men]->nSetRec_St[1], men);
}

void CoilManager::SetData_Row_R(int men, int rec, int index, int val)
{ 
	m_pCoilKenR[men]->nRowVal[men][rec][index] = val;
}

//==========================================
// 現コイル情報の最終レコード情報更新
//------------------------------------------
// int	men	: 表裏(0:表,1:裏)
// int	rec	: レコード
//==========================================
void CoilManager::SetData_Row_V_Ls(int men, int rec)
{ 
	if( m_pCoilKenV[men]->nSetRec_Last[men] >= rec ) return;
	m_pCoilKenV[men]->nSetRec_Last[men] = rec;
	//LOG(em_MSG), "[%s] 現コイル情報の最終レコード情報更新 rec=%d SetRec[0]=%d SetRec[1]=%d men=%d SPM=%d", my_sThreadName, m_pCoilKenV[men]->rec, m_pCoilKenV[men]->nSetRec_St[SpmUmu][0], m_pCoilKenV[men]->nSetRec_St[SpmUmu][1], men, SpmUmu);
}
void CoilManager::SetData_Row_R_Ls(int men, int rec)
{ 
	if( m_pCoilKenR[men]->nSetRec_Last[men] >= rec ) return;
	m_pCoilKenR[men]->nSetRec_Last[men] = rec;
}


//==========================================
// コイル位置情報を生成
//------------------------------------------
// bool mode false データある分だけ全部登録 true:整合性判断する
// int men 表裏
//==========================================
void CoilManager::SetData_CoilPosition(bool mode, int men)
{
	//LOG(em_MSG), "[%s] コイル位置情報を生成 rec=%d SetRec[0]=%d SetRec[1]=%d men=%d", my_sThreadName, m_pCoilKenV[men]->rec, m_pCoilKenV[men]->nSetRec_St[0], m_pCoilKenV[men]->nSetRec_St[1], men);

	AutoLock autolock(&m_csLock);		//---------------------------->>>

	COIL_INF* pCoil = m_pCoilKenV[men];
	int nMenState = mcls_pStatus->GetMenState();				// 検査面状態
	int nSetRec = -1;

	//================================================
	// データセット位置の決定
	if(! mode) {													// ある分だけ
		// 大きい方
		if( nSetRec < pCoil->nSetRec_Last[0] ) nSetRec = pCoil->nSetRec_Last[0];
		if( nSetRec < pCoil->nSetRec_Last[1] ) nSetRec = pCoil->nSetRec_Last[1];
		if( nSetRec < pCoil->nSetRec_St[0] )   nSetRec = pCoil->nSetRec_St[0];
		if( nSetRec < pCoil->nSetRec_St[1] )   nSetRec = pCoil->nSetRec_St[1];
		if( nSetRec < pCoil->nSetRec_Edge[0] ) nSetRec = pCoil->nSetRec_Edge[0];
		if( nSetRec < pCoil->nSetRec_Edge[1] ) nSetRec = pCoil->nSetRec_Edge[1];

	} else if( -1!=nMenState || ! mode ) {							// 片面検査
		//// 片面検査の場合、上流側のみの検査の場合、機器状態が反映されない為の対策
		// データの流れは大きい方
		nSetRec = pCoil->nSetRec_Last[0] > pCoil->nSetRec_Last[1] ? pCoil->nSetRec_Last[0] : pCoil->nSetRec_Last[1];
		// 実コイルは小さいほう
		if( nSetRec > pCoil->nSetRec_St[0] ) nSetRec = pCoil->nSetRec_St[0];
		if( nSetRec > pCoil->nSetRec_St[1] ) nSetRec = pCoil->nSetRec_St[1];
		if( nSetRec > pCoil->nSetRec_Edge[0] ) nSetRec = pCoil->nSetRec_Edge[0];
		if( nSetRec > pCoil->nSetRec_Edge[1] ) nSetRec = pCoil->nSetRec_Edge[1];
	} else {														// 両面検査
		// 両面検査時は 小さい方
		nSetRec = pCoil->nSetRec_Last[0] < pCoil->nSetRec_Last[1] ? pCoil->nSetRec_Last[0] : pCoil->nSetRec_Last[1];
		if( nSetRec > pCoil->nSetRec_St[0] ) nSetRec = pCoil->nSetRec_St[0];	// たぶん必要ないと思うが念の為
		if( nSetRec > pCoil->nSetRec_St[1] ) nSetRec = pCoil->nSetRec_St[1];
		if( nSetRec > pCoil->nSetRec_Edge[0] ) nSetRec = pCoil->nSetRec_Edge[0];
		if( nSetRec > pCoil->nSetRec_Edge[1] ) nSetRec = pCoil->nSetRec_Edge[1];
	}
	
	//// ここまでの処理
	int nOldRec = pCoil->nLastRec_Postion;
	if( nSetRec > pCoil->nLastRec_Postion) {
		pCoil->nLastRec_Postion = nSetRec;
	}

	autolock.UnLock();		// <<<-------------------------------------

	//================================================
	// 同一位置書き込み対策
	if( nSetRec > nOldRec ) {
	//LOG(em_MSG), "途中 %d, %d, %d-%d,  %d,%d,%d,%d", men, pCoil->rec, pCoil->nLastRec_Postion+1, nSetRec, pCoil->nSetRec_Last[0], pCoil->nSetRec_Last[1], pCoil->nSetRec_St[0], pCoil->nSetRec_St[1]);// ▲

		for(int ii=nOldRec+1; ii<=nSetRec; ii++ ) {
			//// コイル単位で一区間でも異常があれば異常とする
			for(int jj=0; jj<NUM_MEN; jj++) {
				// 検査状態 (正常よりも小さい値がある為の対策)
				if( pCoil->emOneKensa[jj] < pCoil->emKensa[jj][ii] ) pCoil->emOneKensa[jj] = pCoil->emKensa[jj][ii];
				// 機器状態
				if( pCoil->emOneKiki [jj] < pCoil->emKiki[jj][ii]  ) pCoil->emOneKiki[jj]  = pCoil->emKiki[jj][ii];
			}

			//// 登録
			SaveDB_CoilPosition(men, ii);
		}
	}
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 情報収集 コイル単位関係

//==========================================
// 入側論理コイル実績を生成 (実際の物の流れのシメ)
//------------------------------------------
// int endMode 実績締めの状態 (1:通常切替 2:バックアップ切替 3:手動停止 4:強制停止 0:不定またはまだ通板中)
// double dLen 実測長 [mm]
// double dSubLen 検査比較用距離 [mm]
//==========================================
void CoilManager::SetData_CoilResult_Real(int endMode, double dLen, double dLenSub)
{
	AutoLock autolock(&m_csLock);

	//// 締めれるデータは締め
	m_CoilInf[m_nKenRecBase].nEndMode	= endMode;
	m_CoilInf[m_nKenRecBase].nLength	= (long)dLen;
	m_CoilInf[m_nKenRecBase].nLengthSub = (long)dLenSub;


	//// 確定通知を送る
	// コイルの切り替え = 前回のコイルの実測長確定
	OpSendFunc::SendOp_CoilResult(&m_CoilInf[m_nKenRecBase]); 
}


//==========================================
// 入側論理コイル実績をセット (データの流れのシメ)
//	 今基準面じゃない方の通板コイルに対してセット
//==========================================
void CoilManager::SetData_CoilResult_Virtual()
{
	AutoLock autolock(&m_csLock);

	//// 実績強制的に締める可能性のあるので、確定時は全件登録しておく
	SetData_CoilPosition(false, m_nDistNotPos);
	//// コイル実績 登録
	SaveDB_CoilResult();
	//// 前工程検査装置系DB仮管理Noフラグセット
	mcls_pMae->RecvCoilResultWrite(m_pCoilKenV[m_nDistNotPos]);
}

//==========================================
// シャーカット実行	[←MI]
//  ・ファーストカット信号ON時は、出側物理コイル実績の確定
//  ・生カット信号ONやファーストカット信号の連続ON時は、
//    ラストカット位置の更新
//------------------------------------------
// double	dLenIn	: シャーカット位置での入側論理コイル距離[mm]
// int		mode	: 0:通常カット、1:実績確定時
// 戻り値	int		: カット種別 (0:Fカット 1:途中カットorLカット(この時点ではわからない))
//==========================================
void CoilManager::SetData_ShaCut(double dLenIn, int mode)
{
	AutoLock autolock(&m_csLock);

	COIL_INF* pCoil = m_pCoilDsp[POS_DSP_SHR_I];		// 現シャー位置通過中のコイル情報ポインタ
	

	// シャーカット位置通過中の該当コイルのカット回数チェック
	if (MAX_SHACUT_COUNT <= pCoil->nCutCount) {
		if (0 == mode)
		{
			LOG(em_WAR), "[%s] シャーカット管理回数上限到達により、シャーカット情報を破棄", my_sThreadName);
			syslog(297, "[コイルNo=%.12s]", m_pCoilBaseDsp->data.cCoilNoIn);
		}
		return;
	}

	//================================================
	// カット情報格納

	// データセット
	memcpy(m_typCut.cKizukenNo, pCoil->cKizukenNo, SIZE_KIZUKEN_NO);
	m_typCut.dPos = dLenIn;
	
	// シャーカット位置通過中の該当コイルにデータセット
	pCoil->nCutLen[pCoil->nCutCount] = (int)dLenIn;
	pCoil->nCutCount++;

	////// 既に該当コイルの尾端が疵検位置を通過している場合は、コイル実績をDBに格納している為、
	////// コイル実績をupdateする。
	//if (pCoil->bCoilResultAddDb) {
	//	// コイル実績の疵検管理のシャーカット情報を変更
	//	SaveDB_UpdateCoilResult(MODE_RECV_SHRCUT, pCoil);
	//}

	//---------------------------------------
	// 出側関連
	//---------------------------------------
	int nRec=0;
	//// 出側コイル情報有り？
	if (0 != strlen(m_CoilOutInf.cID)) {

		nRec = m_CoilOutInf.nCnt - 1;

		// 最終データ確定
		m_CoilOutInf.incoil[nRec].nLength = (int)dLenIn - m_CoilOutInf.incoil[nRec].nFront;

		// 出側確定した為、Db登録
		SaveDB_CoilCutPos(EM_DIV_CUT::DIV_CUT_FCUT, &m_typCut);
	}

	//実績確定時はここまで
	if (1==mode) return;


	int nWk = m_CoilOutInf.incoil[nRec].nInCutNum;
	nRec = 0;
	//// 出側コイル情報 新規作成
	memset(&m_CoilOutInf, 0x00, sizeof(m_CoilOutInf));
	KizuFunction::GetSerialNoFF(m_CoilOutInf.cID);					// シリアルNoセット
	// 先頭の入側論理コイルを取得 (今シャー位置通過中の入側論理コイル)
	memcpy( m_CoilOutInf.incoil[nRec].cKizukenNo, pCoil->cKizukenNo, SIZE_KIZUKEN_NO);

	m_CoilOutInf.incoil[nRec].nFront = (int)dLenIn;
	m_CoilOutInf.incoil[nRec].nOutPos = 0;
	m_CoilOutInf.incoil[nRec].nInCutNum = nWk + 1;
	m_CoilOutInf.incoil[nRec].nSt_Spm = pCoil->nSt_Spm;
	m_CoilOutInf.incoil[nRec].dSpmHosei = pCoil->dSpmHosei;
	m_CoilOutInf.nCnt += 1;

	//// 表示へ通知
	TO_SO_DATA_CUT  data;
	memset(&data, 0x00, sizeof(data));
	data.nMode = DIV_CUT_FCUT;
	data.nPos = (int)m_typCut.dPos;
	OpSendFunc::SendOp_CutInf(pCoil->cKizukenNo, &data);

	LOG(em_MSG), "[%s] シャーカット信号ON%d回目 <%.10s><%.20s> In=%dmm", my_sThreadName,
		pCoil->nCutCount, pCoil->data.cCoilNoIn, pCoil->cKizukenNo, (long)dLenIn );
}

//==========================================
// コイル先頭の切替条件 [←MI]
//------------------------------------------
// int	nMode	: 0:正常切替 1:ゲート信号によるバックアップ	
//==========================================
void CoilManager::SetData_StartWpd(int nMode)
{
	AutoLock autolock(&m_csLock);

	//// 現状の検査基点
	m_CoilInf[m_nKenRecBase].nStartWpdOk = nMode;									
}

//==========================================
// 検査開始からN本目 [←MI]
//==========================================
void CoilManager::SetData_CoilCnt()
{
	AutoLock autolock(&m_csLock);

	//// 一本前のコイルから値を取得
	int nPreCnt = GetCoilIndex(m_nKenRecBase, 1)->nCoilCnt;

	if( INT_MAX == nPreCnt )	m_CoilInf[m_nKenRecBase].nCoilCnt = 1;
	else						m_CoilInf[m_nKenRecBase].nCoilCnt = nPreCnt + 1;
}

//==========================================
// 予定コイル長セット [←MI]
// double dLen 予定コイル長 [mm]
//------------------------------------------
void CoilManager::SetData_CoilLenYotei(double dLen)
{
	AutoLock autolock(&m_csLock);
	
	//// 現状の検査基点
	m_CoilInf[m_nKenRecBase].nLen_Yotei = (long)dLen;
}

//==========================================
// 現エッジ内平均輝度をコイル実績にセット
//------------------------------------------
// int*		pBr	: カメラ毎のエッジ内平均輝度	
//==========================================
void CoilManager::SetData_CoilResult_BrVal(int* pBr)
{
	AutoLock autolock(&m_csLock);

	for(int ii=0; ii<NUM_CAMERA; ii++){
		m_pCoilKenV[NUM_TorB(ii)]->nBrVal[ii] = pBr[ii];
	}
}

//==========================================
// 現露光時間をコイル実績にセット [←MI]
//------------------------------------------
// int*		pExp	: カメラ毎の露光時間
//==========================================
void CoilManager::SetData_CoilResult_AgcVal(int* pExp)
{
	AutoLock autolock(&m_csLock);

	for(int ii=0; ii<NUM_CAMERA; ii++){
		m_pCoilKenV[NUM_TorB(ii)]->nAgcVal[ii] = pExp[ii];
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 情報収集 その他

//==========================================
// 欠陥検出警報解除 	[←MI]
// 通知されたタイミングで、表面検査台位置を通板中のコイルが対象となる
//------------------------------------------
// int		pos				: 表示位置 (0オリジン)
//==========================================
void CoilManager::SetData_AlarmCancel(int pos)
{
	AutoLock autolock(&m_csLock);

	// 表示位置を通板中コイルが対象
	if( 0 > pos || pos >= MAX_DISP_POS ) return;
	COIL_INF const* pCoil = GetCoilAll(pos);
	if( NULL == pCoil ) return;

	// 欠陥検出警報解除のフラグセット
	int rec = pCoil->rec;
	if( m_CoilInf[rec].bAlarmCancel ) {
		m_CoilInf[rec].bAlarmCancel = false;
		LOGA(em_MSG), "[%s] 欠陥検出警報出力 受付完了！<%s>", my_sThreadName, m_CoilInf[rec].cKizukenNo );
		//LOG(em_WAR), "[%s] 欠陥検出警報解除中に さらに 警報解除を受け付けた <%s>", my_sThreadName, m_CoilInf[rec].cKizukenNo );
	} else {
		m_CoilInf[rec].bAlarmCancel = true;
		LOGA(em_MSG), "[%s] 欠陥検出警報解除 受付完了！<%s>", my_sThreadName, m_CoilInf[rec].cKizukenNo );
	}
}

//==========================================
// 現コイル反映時、検査開始1本目をバッファからクリア[←MI]
//==========================================
void CoilManager::ClearFirstCoilInf()
{
	CoilClear(&m_CoilInf[GetRecIndex(0,0)]);
}

//==========================================
// 疵情報ファイル出力先ファイル名称を返却
//------------------------------------------
// 【呼び出し元】
//  HtRecv
//		OnMainDefect
//  CoilManager
//      SetData_CoilResult_Virtual
//------------------------------------------
// int	men		: 表裏(0:表,1:裏)
// int	nRec	: 入側論理コイル情報のバッファのレコードNo
//==========================================
CString CoilManager::GetDefectDumpPath(int nMen, int nRec) {
	CString s;
	char	cBaseFolder[32];

	GetPrivateProfileString("TO_DEFECT", "DEF_DUMP_PATH", "C:\\CSV", cBaseFolder, 32, TASKINI_NAME);
	s.Format("%s\\Def_Men=%d_%d.csv", cBaseFolder, nMen, nRec);		// 疵情報ファイル名称

	return s;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Get

int	CoilManager::GetRecKey(char const* cMeiNo, char const* cCoilNo)
{
	if (0 == strlen(cMeiNo) || 0 == strlen(cCoilNo)) {
		LOG(em_ERR), "[CoilManager] [コイル情報伝文] 連携用コイル情報紐づけ失敗（命令No,コイルNo NULL値）[命令No:%.4s][コイルNo:%.12s]", cMeiNo, cCoilNo );
		return -1;
	}

	COIL_INF const* pWk;
	if (-1 == m_nKenRecBase) return -1;

	for (int ii = 0; ii<SIZE_COIL_BUF; ii++) {
		pWk = GetCoilIndex(m_nKenRecBase, ii);

		if (0 == memcmp(pWk->data.cCoilNoIn, cCoilNo, SIZE_COIL_NO) && 
			0 == memcmp(pWk->data.cMeiNo, cMeiNo, SIZE_MEI_NO))			return pWk->rec;
	}
	// ここまで来たら該当無し
	return -1;
}

void CoilManager::SetAlameInf(DETECT_BASE_DATA const* pD, int men, COIL_INF* pCoil, double const* pAttr)
{
	if (NULL == pCoil) return;
	//================================================
	// 事前準備
	//
	bool bAdd = false;												// 前回より強い疵受信したよフラグ
																	//COIL_INF* pCoil = mcls_pCoil->GetCoilKen_DataSet(men);	// 現検査装置位置通過中のコイル情報ポインタ
	int nWidth = pCoil->data.nWid;								// コイル板幅
	int nMapCol = pCoil->nMapCol;								// 現コイルのマップグリッド数
	int pcode = pCoil->setting.pcode[men] - 1;				// 指定された検査装置位置のコイルの検査パターン (0オリジンにする)
	int rank = 0;												// グレードランク

	int row = (int)(pD->y / OUTPUT_MESYU_SEKKIN_ROW);				// 内部メッシュの行位置 (0オリジン)
	int col = pD->nWidDiv;											// 内部メッシュの列位置 (0オリジン)
	if (0 > row || row >= OUTPUT_MESYU_SEKKIN_MAX) return;

	//======================================================
	// 前準備
	if (!mcls_pStatus->IsRun()) return;		// 検査停止中は警報出力なし
	if (DIV_UNTEN_NOMAL != mcls_pStatus->GetUnten()) return;


	if (pCoil->bAlarmCancel)		return;		// 警報キャンセル時

	ParamManager::P_TYPE_NAME::_TYPE_ID const * prmT = mcls_pParam->GetTypeName(pD->nTid);
	if (NULL == prmT) return;


	//================================================
	// 条件セット
	OUTPUT_ITEM	inf;
	inf.bUmu = true;
	inf.nMen = men;
	inf.nKizuNo = pD->nKizuNo;
	inf.nTid = pD->nTid;
	inf.nGid = pD->nGid;
	inf.nTYuu = pD->nTYuu;
	inf.nGYuu = pD->nGYuu;
	inf.dAria = pD->aria;
	inf.nLen = pD->y;
	inf.nWidDiv = pD->nWidDiv;
	inf.nDs = (int)pD->ds;
	inf.nWs = (int)pD->ws;
	inf.nKind = em_RAMP_NON;
	memset(inf.bPatoFlg, 0x00, sizeof(bool)*MAX_DISP_POS);
	inf.cycle_div = (int)pD->cycle_div;
	//inf.nKind	= prmT->nKizuVoic;

	LOGA(em_MSG), "[%s] 疵発生 [%s][men=%d row=%d col=%d No=%d, len=%.3f, nWidDiv=%d, t=%d, g=%d, width=%d]",
		my_sThreadName, pCoil->cKizukenNo,
		inf.nMen, row, col, inf.nKizuNo, inf.nLen / 1000.0, inf.nWidDiv, inf.nTid, inf.nGid, nWidth);

	//================================================
	// 警報出力セット
	//================================================
	//// 疵接近ランプ チェック
	//-----------------------------
	// 重欠陥
	if (mcls_pParam->Check_Output(ParamManager::OUTPUTFLG_ALARM_RANP_HIGH, pcode, inf.nTid, inf.nGid))
	{
		if ((!pCoil->alarme.ranp[men][row][col].bUmu ||
			 pCoil->alarme.ranp[men][row][col].nKind < em_RAMP_JYU) &&
			inf.nKind < em_RAMP_JYU) {
			inf.nKind = em_RAMP_JYU;
		}
	}
	//-----------------------------
	// 軽欠陥
	if (mcls_pParam->Check_Output(ParamManager::OUTPUTFLG_ALARM_RANP_LOW, pcode, inf.nTid, inf.nGid))
	{
		if ((!pCoil->alarme.ranp[men][row][col].bUmu ||
			pCoil->alarme.ranp[men][row][col].nKind < em_RAMP_KEI) &&
			inf.nKind < em_RAMP_KEI) {
			inf.nKind = em_RAMP_KEI;
		}
	}
	//-----------------------------
	if (em_RAMP_NON != inf.nKind)
	{	// 今回疵が強い
		LOGA(em_MSG), "[%s] 疵接近ランプ出力対象疵発生", my_sThreadName);
		memcpy(&pCoil->alarme.ranp[men][row][col], &inf, sizeof(inf));
	}

	// ボイス用の行数に変更
	row = (int)(pD->y / OUTPUT_MESYU_VOIC_ROW);				// 内部メッシュの行位置 (0オリジン)
	if (0 > row || row >= OUTPUT_MESYU_ROW) return;
	int nVoiceMapCol = KizuFunction::SelectGridCol2(nWidth, OUTPUT_MESYU_VOIC_COL, OUTPUT_MESYU_COL);
	int nVoiceCol = KizuFunction::SelectColPosition2(pD->dsws, pD->ds, pD->ws, nWidth, nVoiceMapCol, OUTPUT_MESYU_COL, OUTPUT_MESYU_VOIC_COL);

	LOGA(em_MSG), "[%s] ボイス用位置 [%s][men=%d row=%d col=%d edge_l=%.0f edge_r=%.0f]",
		my_sThreadName, pCoil->cKizukenNo, inf.nMen, row, nVoiceCol, pAttr[CENTER_X_FROM_L_EDGE], pAttr[CENTER_X_FROM_R_EDGE]);

	//================================================
	//// 欠陥検出ボイス チェック

	// ここから欠陥検出ボイス処理なので初期化
	inf.nKind = em_VOICE_ERR_NON;

	// 優先順位
	//-----------------------------
	// (重)連続疵接近
	//     重大疵接近
	//     ヘゲ接近
	//     スリバー接近
	//     表裏面疵
	//     周期疵注意
	//     微小疵接近
	// (軽)疵接近

	//-----------------------------
	// 連続疵判断用 ちょっと特殊。
	// 今回の疵の連続性を判定
	int ii = 0;
	if (!pCoil->alarme.voic_re[men][row][nVoiceCol])
	{
		// 疵が一定数連続で発生していたため、いつも通りの手順で警報ボイス種類セット
		if (mcls_pParam->Check_Output(ParamManager::OUTPUTFLG_VOICE_SEQ, pcode, inf.nTid, inf.nGid))
		{
			pCoil->alarme.voic_re[men][row][nVoiceCol] = true;

			// 連続疵判定
			for (ii = 1; ii < OUTPUT_VOICE_RE_ROW; ii++)
			{
				if (0 > row - ii) break;
				if (!pCoil->alarme.voic_re[men][row - ii][nVoiceCol]) break;
			}
			if (OUTPUT_VOICE_RE_ROW == ii)
			{
				LOGA(em_MSG), "[%s] ボイス：連続疵検出", my_sThreadName);
				inf.nKind = em_VOICE_DEF_RE;
			}
		}
	}
	//-----------------------------
	// 重大疵
	if (mcls_pParam->Check_Output(ParamManager::OUTPUTFLG_VOICE_HIGH, pcode, inf.nTid, inf.nGid))
	{
		if ((!pCoil->alarme.voice[men][row].bUmu ||
			 GetVoicePriType((ENUM_VOICE_ERR_TYPE)pCoil->alarme.voice[men][row].nKind) < GetVoicePriType(em_VOICE_DEF_JU)) &&
			 GetVoicePriType((ENUM_VOICE_ERR_TYPE)inf.nKind) < GetVoicePriType(em_VOICE_DEF_JU))
		{
			inf.nKind = em_VOICE_DEF_JU;
		}
	}

	//-----------------------------
	// ヘゲ
	if (mcls_pParam->Check_Output(ParamManager::OUTPUTFLG_VOICE_HEGE, pcode, inf.nTid, inf.nGid))
	{
		if ((!pCoil->alarme.voice[men][row].bUmu ||
			GetVoicePriType((ENUM_VOICE_ERR_TYPE)pCoil->alarme.voice[men][row].nKind) < GetVoicePriType(em_VOICE_DEF_HE)) &&
			GetVoicePriType((ENUM_VOICE_ERR_TYPE)inf.nKind) < GetVoicePriType(em_VOICE_DEF_HE))
		{
			inf.nKind = em_VOICE_DEF_HE;
		}
	}

	//-----------------------------
	// スリバ
	if (mcls_pParam->Check_Output(ParamManager::OUTPUTFLG_VOICE_SURI, pcode, inf.nTid, inf.nGid))
	{
		if ((!pCoil->alarme.voice[men][row].bUmu ||
			GetVoicePriType((ENUM_VOICE_ERR_TYPE)pCoil->alarme.voice[men][row].nKind) < GetVoicePriType(em_VOICE_DEF_SU)) &&
			GetVoicePriType((ENUM_VOICE_ERR_TYPE)inf.nKind) < GetVoicePriType(em_VOICE_DEF_SU))
		{
			inf.nKind = em_VOICE_DEF_SU;
		}
	}

	//-----------------------------
	// 表裏面疵 ちょっと特殊。
	// 疵が表裏面で発生していたため、いつも通りの手順で警報ボイス種類セット
	if (mcls_pParam->Check_Output(ParamManager::OUTPUTFLG_VOICE_SAME, pcode, inf.nTid, inf.nGid))
	{
		pCoil->alarme.voic_tb[men][row][nVoiceCol] = true;

		// 表裏疵判定
		//// この位置を中心に逆面の4方向に欠陥があるか探す
		bool bWk = false;
		int nNotMen = 0 == men ? 1 : 0;
		if (pCoil->alarme.voic_tb[nNotMen][row][nVoiceCol]) bWk = true;												// 中心 
		if (0 <= row - 1)				if (pCoil->alarme.voic_tb[nNotMen][row - 1][nVoiceCol]) bWk = true;			// 上
		if (OUTPUT_MESYU_ROW > row + 1)	if (pCoil->alarme.voic_tb[nNotMen][row + 1][nVoiceCol]) bWk = true;			// 下
		if (0 <= nVoiceCol - 1)		 		if (pCoil->alarme.voic_tb[nNotMen][row][nVoiceCol - 1]) bWk = true;		// 左
		if (OUTPUT_MESYU_COL > nVoiceCol + 1)	if (pCoil->alarme.voic_tb[nNotMen][row][nVoiceCol + 1]) bWk = true;	// 右
		if (bWk)
		{
			if ((!pCoil->alarme.voice[men][row].bUmu ||
				GetVoicePriType((ENUM_VOICE_ERR_TYPE)pCoil->alarme.voice[men][row].nKind) < GetVoicePriType(em_VOICE_DEF_TB)) &&
				GetVoicePriType((ENUM_VOICE_ERR_TYPE)inf.nKind) < GetVoicePriType(em_VOICE_DEF_TB))
			{
				inf.nKind = em_VOICE_DEF_TB;
				LOGA(em_MSG), "[%s] ボイス：表裏面疵検出", my_sThreadName);
			}
		}
	}

	//-----------------------------
	// 周期疵
	if (mcls_pParam->Check_Output(ParamManager::OUTPUTFLG_VOICE_CYCLE, pcode, inf.nTid, inf.nGid))
	{
		if ((!pCoil->alarme.voice[men][row].bUmu ||
			GetVoicePriType((ENUM_VOICE_ERR_TYPE)pCoil->alarme.voice[men][row].nKind) < GetVoicePriType(em_VOICE_DEF_SY)) &&
			GetVoicePriType((ENUM_VOICE_ERR_TYPE)inf.nKind) < GetVoicePriType(em_VOICE_DEF_SY))
		{
			inf.nKind = em_VOICE_DEF_SY;
		}
	}

	//-----------------------------
	// その他疵
	if (mcls_pParam->Check_Output(ParamManager::OUTPUTFLG_VOICE_OTHER, pcode, inf.nTid, inf.nGid))
	{
		if ((!pCoil->alarme.voice[men][row].bUmu ||
			GetVoicePriType((ENUM_VOICE_ERR_TYPE)pCoil->alarme.voice[men][row].nKind) < GetVoicePriType(em_VOICE_DEF_OT)) &&
			GetVoicePriType((ENUM_VOICE_ERR_TYPE)inf.nKind) < GetVoicePriType(em_VOICE_DEF_OT))
		{
			inf.nKind = em_VOICE_DEF_OT;
		}
	}

	//-----------------------------
	if (em_VOICE_ERR_NON != inf.nKind)
	{	// 今回疵が強い
		if (GetVoicePriType((ENUM_VOICE_ERR_TYPE)pCoil->alarme.voice[men][row].nKind) < GetVoicePriType((ENUM_VOICE_ERR_TYPE)inf.nKind)) {
			//LOG(em_MSG), "[%s] ボイス出力対象疵発生 [%s][men=%d id=%d row= %d No=%d, len=%.3f, dsws=%d, t=%d, g=%d]",
			//	my_sThreadName, pCoil->cKizukenNo,
			//	inf.nMen, inf.nKind, row, inf.nKizuNo, inf.nLen / 1000.0, inf.nWidDiv, inf.nTid, inf.nGid );
			memcpy(&pCoil->alarme.voice[men][row], &inf, sizeof(inf));
		}

		//LOGG(em_MSG), "[%s] 警報出力対象[疵発生時] [%s][men=%d, No=%d, len=%.3f, dsws=%d, t=%d, g=%d]", my_sThreadName, pCoil->cKizukenNo,
		//	inf.nMen, inf.nSeqNo, inf.nLen / 1000.0, inf.nWidDiv, inf.nTid, inf.nGid );
		LOGA(em_MSG), "[%s] 警報出力対象[疵発生時] [%s][men=%d id=%d No=%d, len=%.3f, dsws=%d, t=%d, g=%d rank=%d]", my_sThreadName, pCoil->cKizukenNo,
			inf.nMen, inf.nKind, inf.nKizuNo, inf.nLen / 1000.0, inf.nWidDiv, inf.nTid, inf.nGid, rank );
	}
}

//==========================================
// 指定最小区間の疵数を＋１する [←HtRecv]
//------------------------------------------
// int	men				  : 表裏(0:表,1:裏)
// const char* pKizukenNo : 管理No
// int	row				  : 指定最小区間行数
//==========================================
int CoilManager::SetDefectCount(int men, const char* pKizukenNo, int row)
{
	for (int ii = 0; ii < SIZE_COIL_BUF; ii++)
	{
		if (0 == memcmp(pKizukenNo, m_CoilInf[ii].cKizukenNo, sizeof(m_CoilInf[ii].cKizukenNo)))
		{
			m_CoilInf[ii].nDefCnt[men][row]++;
#ifdef DEF_DETECT_LOG
			LOG(em_MSG), "[%s] 疵数更新 管理No=%s 面=%d 行=%d 疵数=%d->%d", my_sThreadName, m_CoilInf[ii].cKizukenNo, men, row, m_CoilInf[ii].nDefCnt[men][row] - 1, m_CoilInf[ii].nDefCnt[men][row]);
#endif
			return m_CoilInf[ii].nDefCnt[men][row];
		}
	}
	LOG(em_WAR), "[%s] 疵数更新（対象コイル無し）管理No=%s 面=%d 行=%d", my_sThreadName, pKizukenNo, men, row);
	return -1;
}

//==========================================
// 指定最小区間の疵数を取得する [←HtRecv]
//------------------------------------------
// int	men				  : 表裏(0:表,1:裏)
// const char* pKizukenNo : 管理No
// int	row				  : 指定最小区間行数
//==========================================
int CoilManager::GetDefectCount(int men, const char* pKizukenNo, int row)
{
	for (int ii = 0; ii < SIZE_COIL_BUF; ii++)
	{
		if (0 == memcmp(pKizukenNo, m_CoilInf[ii].cKizukenNo, sizeof(m_CoilInf[ii].cKizukenNo)))
		{
#ifdef DEF_DETECT_LOG
			LOG(em_MSG), "[%s] 疵数取得 管理No=%s 面=%d 行=%d 疵数=%d->%d", my_sThreadName, m_CoilInf[ii].cKizukenNo, men, row, m_CoilInf[ii].nDefCnt[men][row] - 1, m_CoilInf[ii].nDefCnt[men][row]);
#endif
			return m_CoilInf[ii].nDefCnt[men][row];
		}
	}
	LOG(em_WAR), "[%s] 疵数取得（対象コイル無し）管理No=%s 面=%d 行=%d", my_sThreadName, pKizukenNo, men, row);
	return 0;
}
