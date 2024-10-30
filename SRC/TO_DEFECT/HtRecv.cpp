#include "StdAfx.h"
#include "HtRecv.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

#include "MainInstance.h"




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// よく使うマクロ
#ifdef LOG_DEF_OUTPUT
	#define LOGDEF(t)	if(mcls_pLogDef!=NULL)	mcls_pLogDef->Write(KizuLib::LogFileManager::t		// ログ出力
#endif

#define KIZUFILE(t)  if(mcls_pKizuFile!=NULL)		mcls_pKizuFile->Write(t									// 疵情報ファイル出力

// offsetは、 HT_TO_DATA のオフセット
#define GET_PTR_HEADER( cam, header_offset)			my_cRecvData[cam][ header_offset ]
#define GET_PTR_RESULT( cam, result_offset, num )	my_cRecvData[cam][ result_offset + (num*sizeof(NCL_DEFECT_RESULT)) ]
#define GET_PTR_ATTR( cam, attr_offset, num)		my_cRecvData[cam][ attr_offset + (num*sizeof(double)*(MAX_ATTR_BASE+(MAX_ATTR_ANGLE*MAX_IMAGE_CNT))) ]


#ifdef DEBUG_LOG_EDGE_UDP
#define LOGUDP(t)	if(mcls_pLogUdp!=NULL)	mcls_pLogUdp->Write(KizuLib::LogFileManager::t		// ログ出力
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// メイン

//==========================================
// コンストラクタ
//------------------------------------------
// const char *	cSession		: セッション文字列
// int			TorB			: 0:TOP 1:BOT	
// bool			bDistPosition	: 基準面の場合true	
//==========================================
HtRecv::HtRecv(const char *cSession, int TorB, bool bDistPosition) : 
HtRecvBaseManager(cSession, TorB, bDistPosition, NUM_HANTEI, NUM_CAM_POS, DIV_HANTEI01),
mcls_pCoil(NULL),
#ifndef DISABLE_GAP_CALC
mcls_pGap(NULL),
#endif
#ifdef LOG_DEF_OUTPUT
  mcls_pLogDef(NULL),
#endif
#ifdef DEBUG_LOG_EDGE_UDP
  mcls_pLogUdp(NULL),
#endif
mcls_pDefDump(NULL),
mque_pAddSQLData(NULL)
{
	//===========================================================
	//// メンバ変数を初期化
	//////memset( m_nAGCSetFrameNo, 0x00, sizeof(m_nAGCSetFrameNo));
	//////memset( m_nAGCLastCommand, 0x00, sizeof(m_nAGCLastCommand));
	//////memset( m_nAGCBrCenter, 0x00, sizeof(m_nAGCBrCenter));
	//////m_bEnableAGC = true;
	memset( m_nBr, 0x00, sizeof(m_nBr));

	m_nNowGridTrandRow = 0;


	//===========================================================
	//// 機器IDセット
	CString sWk;
	sWk.Format("FRAME_SKIP_%d_ID", TorB+1);
	this->SetFrameSkipId( GetPrivateProfileInt(INI_STATUS, sWk, 0, TASKINI_NAME) );
	KizuFunction::SetStatus(sWk, true, false);		// 正常に戻し

	sWk.Format("EDGE_NG_%d_ID", TorB+1);
	this->SetEdgeNgId( GetPrivateProfileInt(INI_STATUS, sWk, 0, TASKINI_NAME) );
	KizuFunction::SetStatus(sWk, true, false);		// 正常に戻し
	
	sWk.Format("FRAME_UNMATCH_%d_ID", TorB + 1);
	this->SetnFrameUnmatchId(GetPrivateProfileInt(INI_STATUS, sWk, 0, TASKINI_NAME));
	KizuFunction::SetStatus(sWk, true, false);		// 正常に戻し

	// 溶接点
	m_nPantiTypeId = GetPrivateProfileInt(TO_DEFECT, "PANCH_TYPE_ID", 0, TASKINI_NAME);

#ifdef LOG_DEF_OUTPUT
	//===========================================================
	// デバックログ
	setlocale(LC_ALL, "Japanese");	// これが無いと日本語パスを含むフォルダの場合、保存できない。(ライブラリの中で実行するべきか迷っているが、変更するとタスク全体に影響するので、とりあえずここで設定)
	CString sName;
	sName.Format("Def_%s", DivNameManager::GetTorB(TorB) );
	//mcls_pLogDef = new LogFileManager(sName, NULL, 20, 2);	// ここをコメント化するだけでログは出ない。
	mcls_pLogDef = new LogFileManager(sName, NULL, 100, 2);	// ここをコメント化するだけでログは出ない。
	//LOGDEF(em_NON), "日付,管理No,疵連番,tid,gid,最終保存,保存対象,無害,最大,マージ,長手位置,SQLQUE,DBQUE,BCPQUE,無害削除,有害削除,疵ﾘｽﾄFLG,ﾊﾟﾄﾗｲﾄFLG");
	//LOGDEF(em_NON), "日付,管理No,疵連番,tid,gid,最終保存,保存対象,無害,最大,マージ,長手位置,DS,WS,FRM,CAM,LABEL,SQLQUE,DBQUE,BCPQUE,無害削除,有害削除,疵ﾘｽﾄFLG,ﾊﾟﾄﾗｲﾄFLG");
	// 20210312
	LOGDEF(em_NON), "日付,管理No,疵連番,tid,gid,最終保存,保存対象,無害,最大,マージ,長手位置,DS,WS,FRM,CAM,LABEL,SQLQUE,DBQUE,BCPQUE,無害削除,有害削除,疵ﾘｽﾄFLG,ﾊﾟﾄﾗｲﾄFLG,疵保存ｽｷｯﾌﾟ");

#endif

#ifdef DEBUG_LOG_EDGE_UDP
	//===========================================================
	// デバックログ
	setlocale(LC_ALL, "Japanese");	// これが無いと日本語パスを含むフォルダの場合、保存できない。(ライブラリの中で実行するべきか迷っているが、変更するとタスク全体に影響するので、とりあえずここで設定)
	CString sName2;
	sName2.Format("EdgeUdp_%s", DivNameManager::GetTorB(TorB));
	mcls_pLogUdp = new LogFileManager(sName2, NULL, 20, 2);	// ここをコメント化するだけでログは出ない。
	LOGUDP(em_NON), ", 表裏, 管理No, fno, 長手距離, 生エッジ左, 生エッジ右, 穴検エッジ左, 穴検エッジ右, 不信頼フラグ, 伝文サイズ ");
#endif

	// 表示管理送信用疵バッファクリア
	memset(&m_OpSendDefInfo, 0x00, sizeof(m_OpSendDefInfo));
}

//------------------------------------------
// デストラクタ
//------------------------------------------
HtRecv::~HtRecv(void)
{
#ifdef LOG_DEF_OUTPUT
	SAFE_DELETE(mcls_pLogDef);
#endif
#ifdef DEBUG_LOG_EDGE_UDP
	SAFE_DELETE(mcls_pLogUdp);
#endif

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 仮想関数

//==========================================
// 検査開始待ち開始処理
//==========================================
void HtRecv::OnExecDetectWaitStart()
{
	////// このタイミングでは、長手位置が前回値のため、グリッド位置が初期位置にならない
	//m_nNowGridRow = (long)(mcls_pPlg->GetLen_Ken( my_nTorB ) / (MAP_REPRES_MIN*1000));

	//// 代表疵バッファ (この時点で残っている事は無いはず)
	ReSetOp_DetectData(0);

	//// データ初期化
	//////memset( m_nAGCSetFrameNo, 0x00, sizeof(m_nAGCSetFrameNo));
	//////memset( m_nAGCLastCommand, 0x00, sizeof(m_nAGCLastCommand));
	//////memset( m_nAGCBrCenter, 0x00, sizeof(m_nAGCBrCenter));
	memset( m_nBr, 0x00, sizeof(m_nBr));
	m_nNowGridTrandRow = 0;

	mcls_pDefDump->FileNoClear();	// 疵情報ファイル初期化

#ifndef DISABLE_TINY_CALC
	//// 微小欠陥用クラス初期化
	mcls_pTiny->Init(true);
#endif
}

//==========================================
// 検査開始処理
//==========================================
void HtRecv::OnExecDetectRun()
{

	//// 検査開始時に、手前の部分が異常とかなる対策
	////// 長手位置を検査開始時にセットしているため、このタイミングでグリッド位置を算出
	m_nPerFramePos = (int)mcls_pPlg->GetLen_Ken( my_nTorB );
	m_nNowGridRow = (long)(mcls_pPlg->GetLen_Ken( my_nTorB ) / (MAP_REPRES_MIN*1000));

	m_nNowOutGridRow = m_nNowGridRow;
	m_dNowOutLength  = m_nNowGridRow;

	//// 開始時に一発行情報を送る。 (無いと表示の書込recが更新されないため)
	OnOpNewConnect();


	// 両面検査の場合は、自分の面のみ
	// 片面検査の場合は、両方
	int nMenState   = mcls_pStatus->GetMenState();				// 検査面状態 (-1:両面検査 0:表のみで実施 1:裏のみで実施)
	for(int ii=0; ii<NUM_MEN; ii++) {
		if( (-1==nMenState && ii==my_nTorB) ||
			(-1!=nMenState) ) {

			//// エッジ検出整合性準備
			mcls_pEdgePosCont->CoilStart(ii, mcls_pCoil->GetCoilKenV(my_nTorB)->cKizukenNo, (float)mcls_pCoil->GetCoilKenV(my_nTorB)->data.nWid, true);
		}
	}

	// 疵情報ファイル切替　ヘッダー部付与
	int nCoilRec = mcls_pCoil->GetRecKizukenNo(mcls_pCoil->GetCoilKenV(my_nTorB)->cKizukenNo);		// 現在レコード位置
	mcls_pDefDump->Write(nCoilRec, DEF_DUMP_HEAD);												// 疵情報ファイルヘッダー部


}

//==========================================
// 検査停止処理
//==========================================
void HtRecv::OnExecDetectStop()
{
	//// エッジ検出整合性停止
	mcls_pEdgePosCont->CoilEnd(my_nTorB);

#ifndef DISABLE_TINY_CALC
	//// 微小欠陥関係実績締め (前回が動いていた場合のみ)
	if( mcls_pStatus->IsRun() )	{
		mcls_pTiny->CoilEnd(mcls_pCoil->GetCoilKenV(my_nTorB), NULL);
	}
#endif

	mcls_pDefDump->FileNoClear();		// 疵情報ファイル初期化

	// 代表疵バッファクリア
	ReSetOp_DetectData(0);
}

//==========================================
// 新規表示機能新規接続
//==========================================
void HtRecv::OnOpNewConnect()
{
	if( ! this->my_bRun ) return;

	// 今たまっている 行列情報、行情報を送信
	OpSendFunc::SendOp_ColRowData(
					my_nTorB,
					mcls_pCoil->GetCoilKenV(my_nTorB),
					m_nNowGridRow,
					m_OpSendDefInfo.ColnowBuf_Now.BufDefCol,
					m_OpSendDefInfo.ColnowBuf_Now.BufDefAry,
					mcls_pCoil->GetCoilKenV(my_nTorB)->nDefCnt[my_nTorB][m_nNowGridRow]);
	// 後始末
	ClearDefectBuffer();
}

//==========================================
// 業務伝文 受信後のメイン処理
//==========================================
void HtRecv::OnMainDefect()
{
	//==================================================================
	// 初期情報取得
	HT_TO_HEAD*	pHth;
	HT_TO_HEAD& hth = (HT_TO_HEAD&)this->my_cRecvHead[0];			// とりあえず 先頭のヘッダー部取得
	HT_TO_DATA& htd = (HT_TO_DATA&)this->my_cRecvData[0][0];		// とりあえず 先頭のデータ部取得
	int nMenMode	= mcls_pStatus->GetMenState();					// 検査面状態(-1:両面検査 0:表のみで検査 1:裏のみで検査)


	//==================================================================
	// 画像取込～判定→統括伝送完了までの平均、最大所要時間[ms]を算出
	__int64 pc_pre, pc_now;
	//memcpy(&pc_pre, &hth.toVal[TOVAL_REC_PROCTIME], sizeof(__int64));
	memcpy(&pc_pre, &m_trFrm.nRecProcTime, sizeof(__int64));
	QueryPerformanceCounter((LARGE_INTEGER*)&pc_now);
	m_dHtProcNowTime = (double)(pc_now - pc_pre) / m_pf * 1000.0;		// [s] → [ms]
	m_dHtProcAveTime = m_dHtProcAveTime*0.8 + m_dHtProcNowTime*0.2;
	if( hth.nFrameNo > PARAM_FRAME_SKIP_CNT ) {
		if( m_dHtProcMaxTime < m_dHtProcNowTime ) {
			m_dHtProcMaxTime = m_dHtProcNowTime;
		}
	}


	//==================================================================
	// 各判定PCから疵情報受信時、最初の受信～1フレーム揃うまでの平均、最大所要時間[ms]を算出
	//
	m_dRecvProcTotalNowTime = (double)(pc_now - m_pc_RecvFirst) / m_pf * 1000.0;		// [s] → [ms]
	m_dRecvProcTotalAveTime = m_dRecvProcTotalAveTime*0.8 + m_dRecvProcTotalNowTime*0.2;
	if( hth.nFrameNo > PARAM_FRAME_SKIP_CNT ) {
		if( m_dRecvProcTotalMaxTime < m_dRecvProcTotalNowTime ) {
			m_dRecvProcTotalMaxTime = m_dRecvProcTotalNowTime;
		}
	}



	//==================================================================
	// 切替把握 (片面全部に送っている為、最初の1台のみチェックすればOK)
	// ここでは、把握するだけ。処理するのは下のほう
	bool bGridChange = false;										// グリッド切替把握
	bool bCoilChange[NUM_MEN] = {{false}};							// コイル切替把握
	bool bCoilOutChange = false;									// 出側物理単位のコイル切替把握[0:Fカット 1:Lカット]

	//// コイル切り替わり(入側論理単位)
	//if( 1 == hth.toVal[TOVAL_TOP_COILCHANGE] )	bCoilChange[0] = true;
	//if( 1 == hth.toVal[TOVAL_BOT_COILCHANGE] )	bCoilChange[1] = true;
	if(1 == m_trFrm.nCoilChg[my_nTorB]) bCoilChange[my_nTorB] = true;


	//// コイル切り替わり(出側物理単位)
	//if( 1 == hth.toVal[TOVAL_FCUT_COILOUTCHANGE] )	bCoilOutChange[0] = true;
	//if( 1 == hth.toVal[TOVAL_LCUT_COILOUTCHANGE] )	bCoilOutChange[1] = true;
	if(1 == m_trFrm.nFCut) bCoilOutChange = true;

	long nNowGridRow = m_nNowGridRow;
	long nFramePos = hth.frame_pos;

	//// グリッド切り替わり
	if (nNowGridRow + 1 <= (nFramePos / (MAP_REPRES_MIN * 1000)))
	{
		bGridChange = true;
	}


	//==================================================================
	// 生のエッジ位置を算出
	float	dEdge[2];												// 生エッジ位置[0:DS 1:WS]
	for(int ii=0; ii<2; ii++){
		pHth			= (HT_TO_HEAD*)&my_cRecvHead[0];
		dEdge[ii]		= pHth->edge_pos[ii];
	}


	//==================================================================
	// 各判定より 計算したエッジ内平均輝度をセット
	for(int ii=0; ii<NUM_CAM_POS; ii++){
		for(int jj=0; jj<(NUM_CAM_ANGLE-1); jj++){
			m_nBr[ii][jj] = ((HT_TO_HEAD*)&my_cRecvHead[ii])->br[jj];
		}
	}

	//==================================================================
	// ・コイルが切り替わったよ
	// ・最小区間が変わったよ = 代表疵決定
	if (bCoilChange[my_nTorB] || bGridChange)
	{
//LOG(em_INF), "[%s] <%s> 最小区間切替 GridRow:%d nFramePos:%d", my_sThreadName, DivNameManager::GetTorB(my_nTorB), m_nNowGridRow, nFramePos);

		if(MAP_ROW_NUM <= nNowGridRow) nNowGridRow = MAP_ROW_NUM-1;		// 最大コイル長を超えた場合は、最大コイル長の格納位置を更新
		if (0 <= nNowGridRow && MAP_ROW_NUM > nNowGridRow)
		{
			// エッジ位置セット
			mcls_pCoil->SetData_Edge(my_nTorB, nNowGridRow, dEdge);
		}

		//================================================
		// 代表疵＋疵リスト送信
		OpSendFunc::SendOp_ColRowData(
						my_nTorB,
						mcls_pCoil->GetCoilKenV(my_nTorB),
						nNowGridRow,
						m_OpSendDefInfo.ColnowBuf_Now.BufDefCol,
						m_OpSendDefInfo.ColnowBuf_Now.BufDefAry,
						mcls_pCoil->GetCoilKenV(my_nTorB)->nDefCnt[my_nTorB][nNowGridRow]);
		// 代表疵＋疵リスト管理領域クリア
		ReSetOp_DetectData(nFramePos);

#ifndef SENDOP_LENGTH_PLG
		// 通板距離情報 を送信
		OpSendFunc::SendOp_Length();
#endif

		//// 行単位専用
		if (0 <= nNowGridRow && MAP_ROW_NUM > nNowGridRow)
		{
			// 行単位の付加情報 (5mピッチ) 実際は[MAP_ROW_NUM]分のデータがある
			mcls_pCoil->SetData_Row_V_Ls(my_nTorB, nNowGridRow);
		}

		//// コイル単位
		////// 最小区間毎の行情報をセット (データ収集した結果をセット)
		////// 実績が5001mmなどとなると、グリッドは＋１されるが 行情報が送れないので、最後のマップ１行が未検査区間みたいになるを防ぐ対応
		if( bCoilChange[my_nTorB] ) {
			long nLen = mcls_pCoil->GetCoilKenV(my_nTorB)->nLength;
			int  nNewGridRow = nLen / (MAP_REPRES_MIN*1000);
			if(MAP_ROW_NUM <= nNewGridRow) nNewGridRow = MAP_ROW_NUM-1;		// 最大コイル長を超えた場合は、最大コイル長の格納位置を更新

			if(true) {
				// エッジ位置セット
				mcls_pCoil->SetData_Edge(my_nTorB, nNewGridRow, dEdge);
			}

			// 実測長でグリッド位置を計算してもう一回 行情報を送る。
			LOG(em_INF), "[%s] <%s> コイル切替 [%3.3fm] GridRow(%d→%d) fno:%d", my_sThreadName, DivNameManager::GetTorB(my_nTorB), nLen/1000.0, nNowGridRow, nNewGridRow, hth.nFrameNo);

#ifndef SENDOP_LENGTH_PLG
			// 通板距離情報 を送信
			OpSendFunc::SendOp_Length( my_nTorB, nNewGridRow );
#endif
			// 20210604 Add --->
			// コイル切り替え時に未送信疵の有無をチェックし、未送信があれば送信
			bool bLeftovers = false;
			for (int ii = 0; ii < MAP_COL_NUM; ii++) {
				if (m_OpSendDefInfo.ColnowBuf_Now.BufDefCol[ii].umu) {
					LOG(em_INF), "[%s] <%s> ☆☆☆ 次区間 未送信代表疵送信 [fno:%d][No:%d][Pos:%.1f(m)]",
						my_sThreadName, DivNameManager::GetTorB(my_nTorB),
						m_OpSendDefInfo.ColnowBuf_Now.BufDefCol[ii].Data.nFrameNo,
						m_OpSendDefInfo.ColnowBuf_Now.BufDefCol[ii].Data.nKizuNo,
						m_OpSendDefInfo.ColnowBuf_Now.BufDefCol[ii].Data.y / 1000.0);
						bLeftovers = true;
				}
			}

			if (bLeftovers) {
				//================================================
				// 代表疵＋疵リスト送信 ※次最小区間に疵の有無
				OpSendFunc::SendOp_ColRowData(
					my_nTorB,
					mcls_pCoil->GetCoilKenV(my_nTorB),
					nNowGridRow,
					m_OpSendDefInfo.ColnowBuf_Now.BufDefCol,
					m_OpSendDefInfo.ColnowBuf_Now.BufDefAry,
					mcls_pCoil->GetCoilKenV(my_nTorB)->nDefCnt[my_nTorB][nNowGridRow]);
				// 代表疵＋疵リスト管理領域クリア
				ReSetOp_DetectData(nFramePos);
			}
			// <---
		}
	}

	//// バッファに保持している位置情報を DBに登録
	mcls_pCoil->SetData_CoilPosition(true, my_nTorB);


	//==================================================================
	// コイルトレンド
	// とりあえず 5m間隔
	if(true){
		bool bCoilTrand = false;
		int  nNewTrandRow = (hth.frame_pos / (5*1000));
		if( m_nNowGridTrandRow + 1 <= nNewTrandRow ) {
			SetCoilTrand();		// 一定周期で登録
		}
		m_nNowGridTrandRow = nNewTrandRow;
	}


#ifndef DISABLE_TINY_CALC
	//==================================================================
	// 微小欠陥関連
	// ・コイル切替り時
	if( bCoilChange[my_nTorB] ) {
		mcls_pTiny->CoilEnd( mcls_pCoil->GetCoilKenV(my_nTorB), NULL );
	}
#endif


	//==================================================================
	// 把握したコイル切替を実行
	//		片面でしか検査していない場合にも認識させる必要がある為、どっちの面でも同じ処理をさせる

	//// 実績締め
	////// 両面検査の場合は、基準面で無い方のインスタンスで、基準面で無い方のコイル切り替わりが来た
	////// 片面検査の場合は、基準面で無い方のコイル切り替わりが来た
	for(int ii=0; ii<NUM_MEN; ii++){
		if( bCoilChange[ii] ){
			if( (-1==nMenMode && ! m_bDistPostion && bCoilChange[my_nTorB] ) ||
				(-1!=nMenMode && bCoilChange[m_nDistNotPos]) ) {
				
				mcls_pCoil->SetData_CoilResult_Virtual();

			}

	//// 参照ポインタ切替
	// 両面検査の場合は、自分の面のみ
	// 片面検査の場合は、両方
			if( (-1==nMenMode && ii==my_nTorB) ||
				(-1!=nMenMode) ) {

				LOG(em_MSG), "[%s] <%s> %sコイル切り替え把握 [%.3fm]", my_sThreadName, DivNameManager::GetTorB(my_nTorB), DivNameManager::GetTorB(ii), (m_nPerFramePos/1000.0));
				mcls_pCoil->ChangeCoilKenPointer(ii);
			
				// エッジ情報 締め & 次の処理
				mcls_pEdgePosCont->CoilEnd(ii); 
				mcls_pEdgePosCont->CoilStart(ii, mcls_pCoil->GetCoilKenV(ii)->cKizukenNo, (float)mcls_pCoil->GetCoilKenV(ii)->data.nWid, false);	// 開始
			
				// 疵連番初期化
				//m_nNowKizuSeq = 0;

			}
		}
	}

	//// コイル切替完了後
	const COIL_INF*  pCoilInf = mcls_pCoil->GetCoilKenV(my_nTorB);	// 現検査装置位置通過中のコイル情報ポインタ

	//// 自分の面のコイル切り替えの場合、先頭情報としてトレンド登録
	if( bCoilChange[my_nTorB] ) {
		SetCoilTrand();	
		
		// 重要。キューが多くて、まだCoilInfが登録出来ていないで、そのコイルの疵情報が来たときのトラブル対応
		mcls_pDbMgr->ChangeSQLInf( pCoilInf->cKizukenNo );

	}

	//==================================================================
	// 出側物理単位のコイル切替把握
	for(int ii=0; ii<NUM_MEN; ii++){
		if(bCoilOutChange){				// ラストカットタイミングで実施

			//// 参照ポインタ切替
			// 両面検査の場合は、自分の面のみ
			// 片面検査の場合は、両方
			if( (-1==nMenMode && ii==my_nTorB) ||
				(-1!=nMenMode) ){
				LOG(em_MSG), "[%s] 出側物理コイル切替把握 [%.3fm]", my_sThreadName, (m_nPerFramePos/1000.0));
				mcls_pCoil->ChangeCoilOutKenPointer(ii);

			}
		}
	}


	//==================================================================
	// コイル実績用 エッジ内平均輝度をセット
	for(int ii=0; ii<NUM_CAM_POS; ii++){
		HT_TO_HEAD& pWk = (HT_TO_HEAD&)this->my_cRecvHead[ii];
		int camset = pWk.camset - (my_nTorB * NUM_CAM_POS);		// 両面合わせて1～20 →片面単位で1～10
		StatusManager* cls_pS = (StatusManager*) mcls_pStatus;
		for(int jj=0; jj<(MAX_CAMANGLE-1); jj++){
			// データセット
			mcls_pCoil->SetBrCenter(my_nTorB, camset-1, jj, pWk.br[jj]);
			cls_pS->SetBrValue(pWk.camset-1, jj, pWk.br[jj]);
		}
	}


	//==================================================================
	// AGC関連
	//	(コイル切り替え後に実施する事)
	// ※CTLφωでは、探傷エンジンでAGCを実行するため、統括では未実施
	//////if(DIV_UNTEN_NOMAL == mcls_pStatus->GetUnten() || DIV_UNTEN_SMYU == mcls_pStatus->GetUnten()) { 
	//////	if( mcls_pParam->GetParamCommon().nAGCbyCamcont ) {
	//////		// 統括でAGC制御する場合のみ実行	
	//////		if(m_bEnableAGC){
	//////			Check_AGC(hth.nFrameNo);
	//////		}
	//////	}
	//////}


	//==================================================================
	// フレームスキップ
	//	・コイル切り替え後であること
	for(int ii=0; ii<NUM_CAM_POS; ii++) {
		pHth = (HT_TO_HEAD*)&my_cRecvHead[ii];	
		if( 0 != pHth->nFrameSkip || 
			STATUS_PROCSKIP == pHth->nFrameSt ||
			STATUS_TOOMANY  == pHth->nFrameSt ||
			STATUS_QFULL    == pHth->nFrameSt) {

			if( ! this->IsFrameSkipState() ) {
				LOG(em_INF), "[%s] フレームスキップ (fno=%d, camset=%d, ans=%d)", my_sThreadName, pHth->nFrameNo, pHth->camset, pHth->nFrameSkip );
			}
			this->SetEvFrameSkip();
			break;
		}
	}


	//==================================================================
	// エッジ位置の把握
	//	・コイル切り替え後であること
	//////float	dEdgePos[NUM_CAM_POS];				// 各カメラセットの検出エッジ位置
	//////int		nFrameSkip[NUM_CAM_POS];			// 各カメラセットのフレームスキップ
	//////int		nEdgeNg[NUM_CAM_POS];				// 各カメラセットのエッジ検出失敗
	//////int		nFrameSt[NUM_CAM_POS];				// 各カメラセットの探傷状態定義
	//////for(int ii=0; ii<NUM_CAM_POS; ii++) {
	//////	pHth	= (HT_TO_HEAD*)&my_cRecvHead[ii];	
	//////	dEdgePos[ii]	= pHth->edge_pos[0]; 
	//////	nFrameSkip[ii]	= pHth->nFrameSkip;
	//////	nEdgeNg[ii]		= pHth->nEdgeNg;
	//////	nFrameSt[ii]	= pHth->nFrameSt;
	//////

	//////	//if(0 != nFrameSkip[ii] || 0 != nEdgeNg[ii]) {
	//////	//	LOG(em_INF), "[%s] %s (%d)<%s> フレーム状態異常 Frame=%d, Edge=%d", my_sThreadName, DivNameManager::GetTorB(my_nTorB),
	//////	//		ii, pCoilInf->cKizukenNo, nFrameSkip[ii], nEdgeNg[ii]);
	//////	//}

	//////}

	int		nFrameSkip[NUM_CAM_POS];			// 各カメラセットのフレームスキップ
	int		nEdgeNg[NUM_CAM_POS];				// 各カメラセットのエッジ検出失敗
	int		nFrameSt[NUM_CAM_POS];				// 各カメラセットの探傷状態定義
	for(int ii=0; ii<NUM_CAM_POS; ii++) {
		pHth	= (HT_TO_HEAD*)&my_cRecvHead[ii];	
		nFrameSkip[ii]	= pHth->nFrameSkip;
		nEdgeNg[ii]		= pHth->nEdgeNg;
		nFrameSt[ii]	= pHth->nFrameSt;
	}
	// フレーム数、未検査情報をセット
	mcls_pEdgePosCont->SetFrameSkip(my_nTorB, nFrameSkip, nEdgeNg, nFrameSt);

	float	dEdgePos[2];						// 検出エッジ位置[0:DS 1:WS]
	float	dEdgePosMax[2];						// 検出最大エッジ位置[0:DS 1:WS]
	float	dEdgePosMin[2];						// 検出最小エッジ位置[0:DS 1:WS]
	pHth	= (HT_TO_HEAD*)&my_cRecvHead[0];	
	for(int ii=0; ii<2; ii++) {
		dEdgePos[ii]	= pHth->edge_pos[ii]; 
		dEdgePosMax[ii]	= pHth->edge_pos_max[ii];
		dEdgePosMin[ii]	= pHth->edge_pos_min[ii];
	}
	// エッジ位置セット & エッジ整合性判定
	if( ! mcls_pEdgePosCont->SetEdgePos(my_nTorB, hth.nFrameNo, hth.frame_pos, mcls_pPlg->GetResV(), dEdgePos, dEdgePosMax, dEdgePosMin, pCoilInf->dSpmHosei) ) {
		// 先端3mはエッジ不正のチェックは無し。ノッチャー、コイル幅変更でどうしても異常が出るため。
		// 2024.02.22 mizoguchi --->>>
		//if (3000 < hth.frame_pos) {
		int nMaskFront = (long)(SIZE_GRAB_IMG_Y * mcls_pParam->GetCommonMast().Edge.nFromSkip * mcls_pPlg->GetResV());
		if (nMaskFront < hth.frame_pos) {
			// 2024.02.22 mizoguchi <<<---
			if( ! this->IsEdgeNgState() ) {
				LOG(em_INF), "[%s] <%s> エッジ不整合 [%s] (検出:%.3fm, 現:%.3fm, fno=%d)", my_sThreadName, DivNameManager::GetTorB(my_nTorB),
					pCoilInf->cKizukenNo, hth.frame_pos/1000.0, mcls_pPlg->GetLen_Ken(my_nTorB)/1000.0, hth.nFrameNo);
			}
			this->SetEvEdgeNg();
		}
	}


	//==================================================================
	// マージ
	//	・特徴量の最後のほうに、マージ結果を追加
	ExecMerge();


	//==================================================================
	// 画像取込～統括処理完了までの平均、最大所要時間[ms]を算出
	QueryPerformanceCounter((LARGE_INTEGER*)&pc_now);
	m_dToProcNowTime = (double)(pc_now - pc_pre) / m_pf * 1000.0;		// [s] → [ms]
	m_dToProcAveTime = m_dToProcAveTime*0.8 + m_dToProcNowTime*0.2;
	if( hth.nFrameNo > PARAM_FRAME_SKIP_CNT ) {
		if( m_dToProcMaxTime < m_dToProcNowTime ) {
			m_dToProcMaxTime = m_dToProcNowTime;
		}
	}



	//==================================================================
	// DS(WS)の疵情報受信開始～WS(DS)の疵情報受信開始までの平均、最大所要時間[ms]を算出
	// 各判定PCから疵情報受信時、最初の受信～次の受信までの平均、最大所要時間[ms]を算出

	m_dRecvProcNowTime = (double)(m_pc_RecvSecond - m_pc_RecvFirst) / m_pf * 1000.0;		// [s] → [ms]
	m_dRecvProcAveTime = m_dRecvProcAveTime*0.8 + m_dRecvProcNowTime*0.2;
	if( hth.nFrameNo > PARAM_FRAME_SKIP_CNT ) {
		if( m_dRecvProcMaxTime < m_dRecvProcNowTime ) {
			m_dRecvProcMaxTime = m_dRecvProcNowTime;
		}
	}


	//==================================================================
	// 画像処理エンジンの平均、最大所要時間[ms]を取得
	__int64 ht_start, ht_end;
	__int64 nc_freq;
	DWORD nCamSet;
	for(int ii=0; ii<NUM_CAM_POS; ii++) {
		nCamSet = NUM_CamPos( ((HT_TO_HEAD*)&my_cRecvHead[ii])->camset );
		HT_TO_HEAD& hth  = (HT_TO_HEAD&)this->my_cRecvHead[ii];
		memcpy(&ht_start, &hth.ht_timestamp_start, sizeof(__int64));
		memcpy(&ht_end, &hth.ht_timestamp_end, sizeof(__int64));
		memcpy(&nc_freq, &hth.ht_freq, sizeof(__int64));

		// 画像処理エンジン処理完了～統括伝送開始までの所用時間 算出
		m_dNcHtProcNowTime[ii] = (double)( ht_end - ht_start ) / nc_freq * 1000.0;	// [s] → [ms]

		// 平均＆最大時間セット
		m_dNcHtProcAveTime[ii] = m_dNcHtProcAveTime[ii]*0.8 + m_dNcHtProcNowTime[ii]*0.2;
		if( hth.nFrameNo > PARAM_FRAME_SKIP_CNT ) {
			if( m_dNcHtProcMaxTime[ii] < m_dNcHtProcNowTime[ii] ) {
				m_dNcHtProcMaxTime[ii] = m_dNcHtProcNowTime[ii];
			}
		}
	}

	//==================================================================
	// 欠陥発生時処理
	ExecDefect((double)nFramePos);


#ifndef DISABLE_TINY_CALC
	//==================================================================
	// 微小欠陥関連
	// ・通常時
	//
	if(true) {
		TinyManager::ANSER_INF	Tiny;			// 微小欠陥検出結果
		bool bWk = mcls_pTiny->CheckTiny( (HT_TO_HEAD const*)my_cRecvHead, &Tiny );
		if( bWk ) {
			//// 微小欠陥有り。なので、行情報にセット
			// 表示管理へ		
			OpSendFunc::SendOp_Tiny(mcls_pCoil->GetCoilKenV(my_nTorB)->cKizukenNo, my_nTorB, Tiny.emGr, Tiny.nPos);
		}
	}
#endif
}

//==========================================
// 業務伝文 (個別)メイン処理
// ※この処理間はブロックされているので注意
//------------------------------------------
// SOCK_F_HEAD* p	受信データ
//==========================================
void HtRecv::OnMainHls(SOCK_F_HEAD* p)
{
	HT_TO_HEAD_HLS* pH = (HT_TO_HEAD_HLS*) p;
	ENG_STAT* pD = (ENG_STAT*) &pH->EngStat;
	//if( ! pH->nEngStatSet ) return;		// 今回無視データ？

	// データセット
	StatusManager* cls_pS = (StatusManager*) mcls_pStatus;

#ifdef DEBUG_LOG_HT_HEALTHY
	LOG(em_FIL), "[%s] <%s> カメラ台数：%d", my_sThreadName, DivNameManager::GetTorB(my_nTorB), pD->camnum);
#endif

	int camnum = pD->camnum;

	if (0 > camnum || NCL_ncam_max <= camnum) return;

	for(int ii=0; ii<camnum; ii++) {
		//==================================================================
		// カメラ温度セット
		for(int jj=0; jj<MAX_PHYSICAL_CAMANGLE; jj++){
			int camset = pD->camid[ii] - 1;
			if (0 > camset || MAX_CAMSET <= camset) continue;
			cls_pS->SetCamTemp(camset, jj, pD->temp[ii]); 
		}

		//==================================================================
		// 露光時間セット
		for(int jj=0; jj<(MAX_CAMANGLE-1); jj++){
			int camset = pD->camid[ii] - 1;
			int nWk = (int)(pD->exptime[jj][ii]);
			if (0 > camset || NUM_CAMERA <= camset) continue;
			cls_pS->SetExpTime(camset, jj, nWk);
		}

		//==================================================================
		// カメラゲインセット
		for(int jj=0; jj<(MAX_CAMANGLE-1); jj++){
			int camset = pD->camid[ii] - 1;
			int nWk = (int)(pD->gain[jj][ii] * 100);
			if (0 > camset || MAX_CAMSET <= camset) continue;
			cls_pS->SetCamGain(camset, jj, nWk);
		}

		//==================================================================
		// カメラ電圧セット
		for(int jj=0; jj<MAX_PHYSICAL_CAMANGLE; jj++){
			int camset = pD->camid[ii] - 1;
			if (0 > camset || NUM_CAMERA <= camset) continue;
			cls_pS->SetCamVolt(camset, (int)(pD->volt[ii] * 10.0));
		}

#ifdef DEBUG_LOG_HT_HEALTHY
	LOG(em_FIL), "[%s] <%s> カメラ状態セット[ID:%d][温度:%.3f][露光:%.3f %.3f %.3f][ゲイン:%.3f %.3f %.3f][電圧:%.3f]"
																				, my_sThreadName
																				, DivNameManager::GetTorB(my_nTorB)
																				, pD->camid[ii]
																				, pD->temp[ii]
																				, pD->exptime[0][ii]
																				, pD->exptime[1][ii]
																				, pD->exptime[2][ii]
																				, pD->gain[0][ii]
																				, pD->gain[1][ii]
																				, pD->gain[2][ii]
																				, pD->volt[ii]
																				);
#endif
	}

}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 個別処理

//==========================================
// 欠陥発生時処理
//==========================================
void HtRecv::ExecDefect(double dFramePos)
{
	//================================================
	// 欠陥情報取得
	HT_TO_HEAD*				pHth = (HT_TO_HEAD*)&my_cRecvHead[0];		
	HT_TO_DATA*				pHtd = (HT_TO_DATA*)&my_cRecvData[0][0];		
	int						nJpegCmp;			// JPEG圧縮モード (-1:JPEG圧縮無し 0:疵無し 1～:圧縮したサイズ。=Img以降のオフセット値を参照する場合は、この値分減算すること)

	NCL_SMEM_FRAME_HEADER*	pHead;
	NCL_DEFECT_RESULT*		pResu;
	double*					pAttr;
	NCL_IMAGE_HEADER*		pImgBmp;
	DEFECT_HEAD_IMAGE*		pImgJpg;
	DEFECT_DATA_IMAGE		typImg;				// 太平 独自の画像構造体
	int						nDetectCnt;			// 欠陥個数
	int						tid;
	int						gid;


	const COIL_INF*  pCoilInf = mcls_pCoil->GetCoilKenV(my_nTorB);		// 現検査装置位置通過中のコイル情報ポインタ
	int pcode	= pCoilInf->setting.pcode[my_nTorB]-1;					// 指定された検査装置位置のコイルの検査パターン (0オリジンにする)


	//================================================
	// 1フレーム全体の結果取得
	//		・疵No付与
	for(int ii=0; ii<NUM_CAM_POS; ii++) {
		//// 事前チェック
		pHth	= (HT_TO_HEAD*)&my_cRecvHead[ii];							// ヘッダー部取得
		pHtd	= (HT_TO_DATA*)&this->my_cRecvData[ii][0];					// とりあえず 先頭のデータ部取得

		pHead	= (NCL_SMEM_FRAME_HEADER*) &GET_PTR_HEADER(ii, pHtd->header_offset);	// エンジンのヘッダー

		if (FRONT == (pHead->executed_position_mask & FRONT))
		{
			//LOG(em_INF), "[%s] 画像エンジンからのコイル切替認識 Cam:%d frame_num:%d", my_sThreadName, ii + 1, pHead->frame_num );
		}

		if( sizeof(HT_TO_HEAD) == pHth->base_head.len ) continue;			// 疵無し時は、無処理


		//================================================
		// 基本特徴量後端 に 独自情報を付与
		nDetectCnt	= pHead->defect_num;
		for(int jj=0; jj<nDetectCnt; jj++) {
			pResu = (NCL_DEFECT_RESULT*) &GET_PTR_RESULT(ii, pHtd->result_offset, jj);
			pAttr = (double*)	&GET_PTR_ATTR(ii, pHtd->attr_offset, jj);	

			// ------------------------
			// 疵No付与
			pAttr[EPC_ATTR_BASE_KIZUNO] = mcls_pCoil->GetKizuNo((COIL_INF*)pCoilInf);
		}
	}

	//================================================
	// 1フレーム全体の結果取得
	//		・疵連番付与
	//		・基本特徴量後端 に 独自情報を付与
	//		・1フレーム内でもっとも優先度の高い疵を特定する
	for(int ii=0; ii<NUM_CAM_POS; ii++) {
		//// 事前チェック
		pHth	= (HT_TO_HEAD*)&my_cRecvHead[ii];							// ヘッダー部取得
		pHtd	= (HT_TO_DATA*)&this->my_cRecvData[ii][0];					// とりあえず 先頭のデータ部取得

		pHead	= (NCL_SMEM_FRAME_HEADER*) &GET_PTR_HEADER(ii, pHtd->header_offset);	// エンジンのヘッダー
		if( sizeof(HT_TO_HEAD) == pHth->base_head.len ) continue;			// 疵無し時は、無処理


		//================================================
		// 基本特徴量後端 に 独自情報を付与
		nDetectCnt	= pHead->defect_num;
		for(int jj=0; jj<nDetectCnt; jj++) {
			pResu = (NCL_DEFECT_RESULT*) &GET_PTR_RESULT(ii, pHtd->result_offset, jj);
			pAttr = (double*)	&GET_PTR_ATTR(ii, pHtd->attr_offset, jj);	
			tid	  = pResu->type;											// 疵種EdasysID
			gid	  = pResu->grade;											// グレードEdasysID

			// ------------------------
			// 欠陥グレード 付与
			int nRank = mcls_pParam->GetRankG(gid);
			if(0 > nRank) {		// 登録外の場合は、強制的に、先頭の情報に書き換え
				int nGidWk = mcls_pParam->GetGradeName().Grade_Id[0].nEdasysID;
				nRank = mcls_pParam->GetRankG(nGidWk);
			}
			pAttr[EPC_ATTR_BASE_RANK_G] = nRank;

		}

		//================================================
		// 1フレーム内でもっとも優先度の高い疵を特定する
		//   ＋ 基本特徴量後端 に 独自情報を付与
		static const int CHECKNUM = 4;									// 比較用個数
		bool   bBig[CHECKNUM] = {false, false, true, false};		// 比較用情報 (Gr[小]、疵種[小]、面積[大]、長手位置[小])
		double dNow[CHECKNUM];
		double dPre[CHECKNUM];
		int nDefMaxIndex = -1;								// １フレーム内でもっとも優先度の高い疵インデックス (-1:対象無し)

		for(int jj=0; jj<nDetectCnt; jj++) {
			pHead = (NCL_SMEM_FRAME_HEADER*) &GET_PTR_HEADER(ii, pHtd->header_offset);	// エンジンのヘッダー
			pResu = (NCL_DEFECT_RESULT*) &GET_PTR_RESULT(ii, pHtd->result_offset, jj);
			pAttr = (double*)	&GET_PTR_ATTR(ii, pHtd->attr_offset, jj);	

#ifdef LOCAL
/** △2回毎にグレードIDを変更(ID+1)
pResu->grade = (my_nFrameNo[0] % 2) +1;
//**/
#endif

			tid	  = pResu->type;											// 疵種EdasysID
			gid	  = pResu->grade;											// グレードEdasysID


			// ------------------------
			// 自分がマージ対象疵なら関係無し
			if( 0 != pAttr[EPC_ATTR_BASE_MERGE] ) continue;

			// ------------------------
			// 最初の疵を初期
			if( -1 == nDefMaxIndex) {
				nDefMaxIndex = jj;
				dPre[0] = pAttr[EPC_ATTR_BASE_YU_G];
				dPre[1] = pAttr[EPC_ATTR_BASE_YU_T];
				dPre[2] = pAttr[AREA];
				dPre[3] = pAttr[MERGED_CENTER_Y];
			}

			// 比較用情報
			dNow[0] = pAttr[EPC_ATTR_BASE_YU_G];
			dNow[1] = pAttr[EPC_ATTR_BASE_YU_T];
			dNow[2] = pAttr[AREA];
			dNow[3] = pAttr[MERGED_CENTER_Y];

			// ------------------------
			// 比較
			if( KizuFunction::SelectPriorityDetect(CHECKNUM, dNow, dPre, bBig) ) { 
				// 新旧入替
				nDefMaxIndex = jj;
				memcpy(dPre, dNow, sizeof(dPre));
			}
		}

		// ------------------------
		// 最大優先度
		if(-1 != nDefMaxIndex) {
			pAttr = (double*)	&GET_PTR_ATTR(ii, pHtd->attr_offset, nDefMaxIndex);	
			pAttr[EPC_ATTR_BASE_FRAME_DEFECT_MAX] = 1;
		}

	}

	int nShrinkRatio = 1;

	//================================================
	// 個々の疵の結果取得
	for(int ii=0; ii<NUM_CAM_POS; ii++) {
		//// 事前チェック
		pHth	= (HT_TO_HEAD*)&my_cRecvHead[ii];	
		pHtd	= (HT_TO_DATA*)&this->my_cRecvData[ii][0];					// とりあえず 先頭のデータ部取得

		pHead	= (NCL_SMEM_FRAME_HEADER*) &GET_PTR_HEADER(ii, pHtd->header_offset);	// エンジンのヘッダー

		nJpegCmp	= pHth->nJpegCmp;
		if( sizeof(HT_TO_HEAD) == pHth->base_head.len ) continue;			// 疵無し時は、無処理

		//// 欠陥個数分
		nDetectCnt	= pHead->defect_num;
//LOG(em_INF), "[%s] 疵画像 Cam:%d num:%d", my_sThreadName, ii + 1, nDetectCnt );
		for(int jj=0; jj<nDetectCnt; jj++) {


			//================================================
			//// 事前準備 として 各アドレスをセット
			pResu = (NCL_DEFECT_RESULT*) &GET_PTR_RESULT(ii, pHtd->result_offset, jj);
			pAttr = (double*)	&GET_PTR_ATTR(ii, pHtd->attr_offset, jj);	
			tid	  = pResu->type;											// 疵種EdasysID
			gid	  = pResu->grade;											// グレードEdasysID


			//================================================
			// 画像はちょっと特殊 (複数疵で同じ画像の場合がある)
			// 画像配列分オフセット位置をずらしていき、取得したい画像情報のオフセット位置を取得する

			//// BMP画像
			if( -1 == nJpegCmp) {		
				// 今回の画像配列位置を特定
				int imgRecOff =	pHtd->image_offset;									// 画像情報の先頭位置
				for(int kk=0; kk<pHtd->image_num; kk++) {
					if(kk == pResu->im_idx) break;
					pImgBmp = (NCL_IMAGE_HEADER*) &my_cRecvData[ii][imgRecOff];		// 本配列の画像情報取得
					imgRecOff += sizeof(DWORD)*2 + (pImgBmp->height * pImgBmp->width * MAX_CAMANGLE);			// 本配列の画像情報サイズを加算 = 次画像情報の位置
				}
				pImgBmp  = (NCL_IMAGE_HEADER*) &my_cRecvData[ii][imgRecOff];
				
				// 太平用画像構造体へデータセット
				typImg.bIsBmp	= true;
				typImg.nWidth	= pImgBmp->width;
				typImg.nHeight	= pImgBmp->height;
				for(int kk=0; kk<MAX_CAMANGLE; kk++) {
					typImg.nSize[kk] = pImgBmp->height * pImgBmp->width;
					typImg.pImg[kk]  = &pImgBmp->image[pImgBmp->height * pImgBmp->width * kk];
				}
//LOG(em_INF), "[%s] 疵画像 [BMP kizuno=%d tid=%d, gid=%d][v:%d h:%d size:%d]", my_sThreadName, (long)pAttr[EPC_ATTR_BASE_KIZUNO], tid, gid, typImg.nHeight, typImg.nWidth, typImg.nSize );

				// 切り出し画像の縮小率
				nShrinkRatio = (int)pImgBmp->shrinkratio;

			//// Jepg画像
			} else {
				// 今回の画像配列位置を特定
				int imgRecOff =	pHtd->image_offset;									// 画像情報の先頭位置
				int kk;
				for(kk=0; kk<pHtd->image_num; kk++) {
					if(kk == pResu->im_idx) break;
					pImgJpg = (DEFECT_HEAD_IMAGE*) &my_cRecvData[ii][imgRecOff];		// 本配列の画像情報取得
					imgRecOff += pImgJpg->nNextImgOffset;								// 次画像情報の位置を加算
//LOG(em_INF), "[%s] 疵画像 [JPG kizuno=%d no=%d tid=%d, gid=%d][imgnum:%d v:%d h:%d size:%d]", my_sThreadName, (long)pAttr[EPC_ATTR_BASE_KIZUNO], kk + 1, tid, gid, pHtd->image_num, pImgJpg->nHeight, pImgJpg->nWidth, pImgJpg->nSize );
				}
				pImgJpg  = (DEFECT_HEAD_IMAGE*) &my_cRecvData[ii][imgRecOff];
//LOG(em_INF), "[%s] 疵画像 [JPG kizuno=%d no=%d tid=%d, gid=%d][imgnum:%d v:%d h:%d size:%d]", my_sThreadName, (long)pAttr[EPC_ATTR_BASE_KIZUNO], kk + 1, tid, gid, pHtd->image_num, pImgJpg->nHeight, pImgJpg->nWidth, pImgJpg->nSize );

				// 太平用画像構造体へデータセット
				typImg.bIsBmp	= false;
				typImg.nWidth	= pImgJpg->nWidth;
				typImg.nHeight	= pImgJpg->nHeight;
				for(int kk=0; kk<MAX_CAMANGLE; kk++) {
					typImg.nSize[kk] = pImgJpg->nSize[kk];
//					typImg.pImg[kk]  = (BYTE*)((int)pImgJpg + pImgJpg->nImgOffset[kk]);
					typImg.pImg[kk] = &reinterpret_cast<BYTE*>(pImgJpg)[ pImgJpg->nImgOffset[kk] ];

/*/◆
CString s;
s.Format("to_men%d_cam%d_%d_%d.jpg", my_nTorB, ii, jj, kk);
std::ofstream   os;
os.open(s, std::ios::binary);
os.write((const char*)typImg.pImg[kk], typImg.nSize[kk] );
os.close();
//*/
				}
//LOG(em_INF), "[%s] 疵画像 [JPG kizuno=%d tid=%d, gid=%d][imgnum:%d v:%d h:%d size:%d]", my_sThreadName, (long)pAttr[EPC_ATTR_BASE_KIZUNO], tid, gid, pHtd->image_num, typImg.nHeight, typImg.nWidth, typImg.nSize );
/*/◆				
//				if (pHtd->defect_num != pHtd->image_num) {
//					LOG(em_ERR), "[%s]  [y=%dmm kizuNo=%d defect_num=%d image_num=%d, im_idx=%d, size=%d/%d/%d/%d]"
//						, my_sThreadName, (long)pAttr[MERGED_CENTER_Y], (int)pAttr[EPC_ATTR_BASE_KIZUNO]
//						, pHtd->defect_num, pHtd->image_num, pResu->im_idx 
//						, typImg.nSize[0], typImg.nSize[1], typImg.nSize[2], typImg.nSize[3]);
//				}
//*/
				// 切り出し画像の縮小率
				nShrinkRatio = (int)pImgJpg->fShrinkRatio;
			}

			//================================================
			////// 全長画像間引き率を特徴量にセット
			////pAttr[EPC_ATTR_BASE_IMG_CULL_X]			= mcls_pParam->GetCommonMast().nImgCullX;	// 全長画像_間引き率横
			////pAttr[EPC_ATTR_BASE_IMG_CULL_Y]			= mcls_pParam->GetCommonMast().nImgCullY;	// 全長画像_間引き率縦
			pAttr[EPC_ATTR_BASE_IMG_CULL_X] = mcls_pParam->GetParamCommon().nShFrameShrinkX;	// 共有メモリ上フレーム画像縮小X
			pAttr[EPC_ATTR_BASE_IMG_CULL_Y] = mcls_pParam->GetParamCommon().nShFrameShrinkY;	// 共有メモリ上フレーム画像縮小Y
			if(1 > pAttr[EPC_ATTR_BASE_IMG_CULL_X]) pAttr[EPC_ATTR_BASE_IMG_CULL_X] = 1.0;
			if(1 > pAttr[EPC_ATTR_BASE_IMG_CULL_Y]) pAttr[EPC_ATTR_BASE_IMG_CULL_Y] = 1.0;

#ifdef DEBUG_LOG_KIZU
			// ▲
			LOG(em_INF), "[%s] 疵検出 [FNo=%d, kizuNo=%d, cam=%d, tid=%d, gid=%d, max=%d, y=%dmm]", my_sThreadName, pHth->nFrameNo, 
				(int)pAttr[EPC_ATTR_BASE_KIZUNO], pHth->camset, tid, gid, (int)pAttr[EPC_ATTR_BASE_FRAME_DEFECT_MAX], (long)pAttr[MERGED_CENTER_Y] );
#endif


			//================================================
			// マーキング所要時間を特徴量にセット
			pAttr[EPC_ATTR_BASE_HTTO_PROC_TIME]		= m_dHtProcNowTime;						// 画像取込～判定→統括伝送完了までの所要時間 [msec]
			pAttr[EPC_ATTR_BASE_TOTAL_PROC_TIME]	= m_dToProcNowTime;						// 画像取込～統括処理完了までの所要時間 [msec]
			pAttr[EPC_ATTR_BASE_HT_PROC_TIME]		= m_dNcHtProcNowTime[pHth->camset];		// 画像処理エンジン処理完了～判定→統括伝送開始までの所要時間 [msec]
			pAttr[EPC_ATTR_BASE_TR1_PROC_TIME]		= m_dRecvProcNowTime;					// DS(WS)の疵情報受信開始～DS(WS)の疵情報受信開始までの所要時間 [msec]
			pAttr[EPC_ATTR_BASE_TR2_PROC_TIME]		= m_dRecvProcTotalNowTime;				// DS(WS)の疵情報受信開始～WS(DS)の疵情報受信完了までの所要時間 [msec]
			pAttr[EPC_ATTR_BASE_NC_PROC_TIME]		= 0;									// 画像取込～画像処理エンジン処理完了までの所要時間 [msec]

			//================================================
			// 個々の疵情報の処理 (ちょっとでっかいし、簡略化する為に別メソッドとする)
			SetDetectInf(pResu, pAttr, dFramePos, nShrinkRatio, &typImg);
		}
	}
}

//==========================================
// 個々の疵情報の処理
//------------------------------------------
// nt_defect_result*	pResu		: 検査結果情報
// double*				pAttr		: 特徴量リスト (基本特徴量+角度1特徴量+角度2特徴量･･･)
// double				nFramePos	: フレーム[mm]
// int					nShrinkRatio: 切り出し画像の縮小率
// DEFECT_DATA_IMAGE*	pImg		: 太平用画像情報ポインタ
//==========================================
void HtRecv::SetDetectInf(NCL_DEFECT_RESULT const* pResu, double const* pAttr, double dFramePos, int nShrinkRatio, DEFECT_DATA_IMAGE const* pImg)
{
	//================================================
	// 事前準備
	const COIL_INF*  pCoilInf = mcls_pCoil->GetCoilKenV(my_nTorB);			// 現検査装置位置通過中のコイル情報ポインタ
	int nWidth  = pCoilInf->data.nWid;										// コイル板幅[mm]

	int pcode	= pCoilInf->setting.pcode[my_nTorB]-1;					// 指定された検査装置位置のコイルの検査パターン (0オリジンにする)
	int tid		= pResu->type;											// 疵種EdasysID
	int gid		= pResu->grade;											// グレードEdasysID
	long nPos	= (long)pAttr[MERGED_CENTER_Y];							// フレーム単位のコイル先端からの距離 [mm]
	int nKizuNo = (long)pAttr[EPC_ATTR_BASE_KIZUNO];					// 疵No
	char cKanriNo[SIZE_KIZUKEN_NO];
	float dLen;															// 距離情報 
	memcpy(cKanriNo, pCoilInf->cKizukenNo, SIZE_KIZUKEN_NO);

	if (0 == pImg->nHeight || 0 == pImg->nWidth || 0 == pImg->nSize || NULL == pImg->pImg)
	{
		LOG(em_ERR), "[%s] 疵画像異常 [%s kizuno=%d tid=%d, gid=%d][v:%d h:%d size:%d]", my_sThreadName, cKanriNo, nKizuNo, tid, gid, pImg->nHeight, pImg->nWidth, pImg->nSize );
	}

	//================================================
	// EdasysIDが登録されている奴かどうかチェックする必要がある？
	if( ! mcls_pParam->CheckEdasysID_T(tid) || ! mcls_pParam->CheckEdasysID_G(gid) ) {
		LOG(em_ERR), "[%s] 登録外のEdasysIDを検出。先頭EdasysIDに強制振替 [%s kizuno=%d tid=%d, gid=%d]", my_sThreadName, cKanriNo, nKizuNo, tid, gid );
		syslog(285, "[%s kizuno=%d tid=%d gid=%d]", cKanriNo, nKizuNo, tid, gid);

		// 強制的に、先頭の情報に書き換え
		if( ! mcls_pParam->CheckEdasysID_T(tid)) {
			tid = mcls_pParam->GetTypeName().Type_Id[0].nEdasysID; 
			((NCL_DEFECT_RESULT*)pResu)->type = tid; 
		}
		if( ! mcls_pParam->CheckEdasysID_G(gid)) {
			gid = mcls_pParam->GetGradeName().Grade_Id[0].nEdasysID;
			((NCL_DEFECT_RESULT*)pResu)->grade = gid; 
		}
	}

	// デバック用 ◆
	if( 0 != m_nPantiTypeId && m_nPantiTypeId == tid ) {
		LOG(em_INF), "[%s] <%s> パンチ穴検出 [%0.3fm]", my_sThreadName, DivNameManager::GetTorB(my_nTorB), nPos/1000.0);
	}


	//LOG(em_INF), "[%s] <%s> 疵検出 [%0.3fm]", my_sThreadName, DivNameManager::GetTorB(my_nTorB), nPos/1000.0);


	//================================================
	// 基本情報 生成
	DETECT_BASE_DATA data;
	Create_DetectBaseData( my_nTorB, nWidth, pCoilInf->nMapCol, pResu, pAttr, nShrinkRatio, &data );
	//LOG(em_INF), "[%s] <%s> 外接Xmin[pixel]:%d", my_sThreadName, DivNameManager::GetTorB(my_nTorB), data.frm_x_min);
	//LOG(em_INF), "[%s] <%s> 外接Xmax[pixel]:%d", my_sThreadName, DivNameManager::GetTorB(my_nTorB), data.frm_x_max);
	//LOG(em_INF), "[%s] <%s> 外接Ymin[pixel]:%d", my_sThreadName, DivNameManager::GetTorB(my_nTorB), data.frm_y_min);
	//LOG(em_INF), "[%s] <%s> 外接Ymax[pixel]:%d", my_sThreadName, DivNameManager::GetTorB(my_nTorB), data.frm_y_max);
	//LOG(em_INF), "--------------------------------");
	data.w_size		 = (WORD)pImg->nWidth;
	data.h_size		 = (WORD)pImg->nHeight;
	data.base_w_size = (WORD)(pImg->nWidth * nShrinkRatio);
	data.base_h_size = (WORD)(pImg->nHeight * nShrinkRatio);
	//if (1 != nShrinkRatio)
	//{
	//	LOG(em_INF), "[%s] <%s> 切り出し画像の縮小率：%d", my_sThreadName, DivNameManager::GetTorB(my_nTorB), nShrinkRatio);
	//	LOG(em_INF), "[%s] <%s> 元の縦分解能：%.4f　横分解能：%.4f", my_sThreadName, DivNameManager::GetTorB(my_nTorB), data.base_h_res, data.base_w_res);
	//	LOG(em_INF), "[%s] <%s> 　　縦分解能：%.4f　横分解能：%.4f", my_sThreadName, DivNameManager::GetTorB(my_nTorB), data.h_res, data.w_res);
	//	LOG(em_INF), "[%s] <%s> 元の切り出し画像 left:%d top:%d right:%d bottom:%d center:(%d, %d)", my_sThreadName, DivNameManager::GetTorB(my_nTorB),
	//		data.base_box_x_min,
	//		data.base_box_y_min,
	//		data.base_box_x_max,
	//		data.base_box_y_max,
	//		data.base_center_x,
	//		data.base_center_y);
	//	LOG(em_INF), "[%s] <%s> 　　切り出し画像 left:%d top:%d right:%d bottom:%d center:(%d, %d)", my_sThreadName, DivNameManager::GetTorB(my_nTorB),
	//		data.box_x_min,
	//		data.box_y_min,
	//		data.box_x_max,
	//		data.box_y_max,
	//		data.center_x,
	//		data.center_y);
	//	LOG(em_INF), "--------------------------------");
	//}

	// 20210312 --->>>
	char cKizukenNo[SIZE_KIZUKEN_NO];
	memcpy(cKizukenNo, mcls_pCoil->GetCoilKenV(my_nTorB)->cKizukenNo, SIZE_KIZUKEN_NO);

	// 疵DB保存・表示を制御（台車がオンライン位置でない、かつ、機器マスターの検査異常度が正常以外を判定）
	if (1 == mcls_pStatus->GetStatusRead().DaisyaEx_St[my_nTorB].stat && DIV_KENSA_OK != mcls_pStatus->GetStatusRead().DaisyaEx_St[my_nTorB].kensa) {

		LOGDEF(em_NON), ",%s,%d,%d,%d,%s(%d),%s(%d),%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,〇",
			cKizukenNo, (int)pAttr[EPC_ATTR_BASE_KIZUNO], tid, gid,
			"×", -1,
			(mcls_pParam->Check_Output((ParamManager::EM_OUTPUTFLG)(ParamManager::OUTPUTFLG_LOWGR_DB_O + my_nTorB), pcode, tid, gid) ? "○" : "×"), mcls_pParam->Check_Output((ParamManager::EM_OUTPUTFLG)(ParamManager::OUTPUTFLG_LOWGR_DB_O + my_nTorB), pcode, tid, gid),
			0, (int)pAttr[EPC_ATTR_BASE_FRAME_DEFECT_MAX], (int)pAttr[EPC_ATTR_BASE_MERGE], nPos,
			(int)pAttr[CENTER_X_FROM_L_EDGE], (int)pAttr[CENTER_X_FROM_R_EDGE], (int)pAttr[FRAME_NUM], (int)pAttr[CAMSET], (int)pAttr[LABEL_VALUE],

			mcls_pDbMgr->GetQueData_Count(), mcls_pDbMgr->GetQueDB_Count(), mcls_pDbMgr->GetBcpSaveMgr().GetQueData_Count(),
			mcls_pDbMgr->GetInf_LowGrDeleCnt(0, my_nTorB),
			mcls_pDbMgr->GetInf_DeleCnt(0, my_nTorB), false, false
			);

		return;
	}
	// 20210312 <<<---

	//================================================
	// 疵発生位置をセット
	dLen = data.y;

	//================================================
	// 表示出力対象の場合、代表疵決定用バッファにセット
	//// 表示出力対象 選定のみ。 探傷画面表示結果をDBに残したいため
	bool bAddOp = false;
	bool bObjTansho = mcls_pParam->Check_Output((ParamManager::EM_OUTPUTFLG)(ParamManager::OUTPUTFLG_DSP_O + my_nTorB), pcode, tid, gid);		// 探傷画面表示対象
	bool bObjDeflist = mcls_pParam->Check_Pall_Output((ParamManager::EM_OUTPUT_PALLFLG)(ParamManager::OUTPUTPALLFLG_DEFLIST_DETAIL), tid, gid); // 詳細フィルタ
	bool bObjPatoOutput = mcls_pParam->Check_Output((ParamManager::EM_OUTPUTFLG)(ParamManager::OUTPUTFLG_PATOLOGHT), pcode, tid, gid);				// パトライト出力
	bool bDspMode = false;
	if (!pCoilInf->bAlarmCancel && bObjPatoOutput & !pCoilInf->bDummeyCoil)
	{
		// ・欠陥警報禁止指定無し
		// ・パトライト出力
		// ・ダミーコイル以外
		if( 0 == pAttr[EPC_ATTR_BASE_MERGE] ) bDspMode = mcls_pArm->Add_Pato(pCoilInf->cKizukenNo, my_nTorB, nKizuNo, nPos);
	}

	// 表示機能送信用にデータセット
	data.nObjTansho = (bObjTansho ? 1 : 0);
	data.nObjDeflist = (bObjDeflist ? 1 : 0);
	data.nModeDeflist = (bDspMode ? 1 : 0);

	SYSTEMTIME time;
	GetLocalTime(&time);

	data.time[0] = time.wYear;
	data.time[1] = time.wMonth;
	data.time[2] = time.wDay;
	data.time[3] = time.wHour;
	data.time[4] = time.wMinute;
	data.time[5] = time.wSecond;

	bAddOp |= bObjTansho;												// 探傷画面出力対象なら送る			OR
	bAddOp |= bObjDeflist;												// 重欠陥一覧画面出力対象なら送る	OR
	bAddOp &= (0 == pAttr[EPC_ATTR_BASE_MERGE] ? true : false);			// マージ対象疵なら送らない			AND

	//// 探傷画面の疵個数のみをカウント対象とする(探傷画面以外の疵も送信するため)
	bool bRowKizuCnt = false;
	bRowKizuCnt |= mcls_pParam->Check_Output((ParamManager::EM_OUTPUTFLG)(ParamManager::OUTPUTFLG_DSP_O + my_nTorB), pcode, tid, gid);	// 通常の表示対象

	if (bAddOp)
	{
		// 表示管理送信用疵情報編集
		SetBuf_DetectData(mcls_pCoil->GetCoilKenV(my_nTorB), &data, pImg, dLen, dFramePos);
	}


	//================================================
	// DB保存対象？
	int	nAddDb = -1;						// -1:保存NG   0:保存OK   9:画像保存無しバージョン

	// 無害Gr の特定
	bool bUpGrFlg  = mcls_pParam->Check_Output((ParamManager::EM_OUTPUTFLG)(ParamManager::OUTPUTFLG_DB_O + my_nTorB), pcode, tid, gid);	
	bool bLowGrFlg = mcls_pParam->Check_Output((ParamManager::EM_OUTPUTFLG)(ParamManager::OUTPUTFLG_LOWGR_DB_O + my_nTorB), pcode, tid, gid);	
	int  nLowGrFlg = ( bLowGrFlg && ! bUpGrFlg ) ? 1 : 0;	// 無害疵一時保存フラグ (低Gr <=x< 高Gr の範囲内の場合、1)	
	if( bUpGrFlg || bLowGrFlg ) {
		int  nLowGrFlgWk = nLowGrFlg;
		if( 0 != pAttr[EPC_ATTR_BASE_MERGE] ) nLowGrFlgWk = 1;	// マージ対象疵なら低Gr認識 (これ重要)		
			
		// フロー制御は、登録前に行うように変更 (BCPが遅くなると、どうしようも無くなる為)
		nAddDb = mcls_pDbMgr->SaveDefectCheck( cKizukenNo, my_nTorB, nLowGrFlgWk, (int)pAttr[EPC_ATTR_BASE_FRAME_DEFECT_MAX]);
	}


#ifdef LOG_DEF_OUTPUT
	// 調査用ログ	20210312 --->>>
	LOGDEF(em_NON), ",%s,%d,%d,%d,%s(%d),%s(%d),%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,×",
		cKizukenNo, (int)pAttr[EPC_ATTR_BASE_KIZUNO],tid,gid,
		(-1==nAddDb?"×": (0==nAddDb?"○":"△")), nAddDb,
		(mcls_pParam->Check_Output((ParamManager::EM_OUTPUTFLG)(ParamManager::OUTPUTFLG_LOWGR_DB_O + my_nTorB), pcode, tid, gid)?"○":"×"),mcls_pParam->Check_Output((ParamManager::EM_OUTPUTFLG)(ParamManager::OUTPUTFLG_LOWGR_DB_O + my_nTorB), pcode, tid, gid),
		nLowGrFlg, (int)pAttr[EPC_ATTR_BASE_FRAME_DEFECT_MAX], (int)pAttr[EPC_ATTR_BASE_MERGE], nPos,
		(int)pAttr[CENTER_X_FROM_L_EDGE], (int)pAttr[CENTER_X_FROM_R_EDGE], (int)pAttr[FRAME_NUM], (int)pAttr[CAMSET], (int)pAttr[LABEL_VALUE],
	
		mcls_pDbMgr->GetQueData_Count(), mcls_pDbMgr->GetQueDB_Count(), mcls_pDbMgr->GetBcpSaveMgr().GetQueData_Count(),
		mcls_pDbMgr->GetInf_LowGrDeleCnt(0, my_nTorB),
		mcls_pDbMgr->GetInf_DeleCnt(0, my_nTorB), bObjDeflist, bDspMode
	);
	// <<<---
#endif

	if(-1 != nAddDb) {
		//// 事前準備
		DBManager::DeliveryDB* pDeli = new DBManager::DeliveryDB;
		pDeli->kubun = DBManager::em_DEFECTINF;
		pDeli->data  = (BYTE*)new DBDATA_DEFECT_INF;
		DBDATA_DEFECT_INF* pWk = (DBDATA_DEFECT_INF*)pDeli->data;
		memset(pWk, 0x00, sizeof(DBDATA_DEFECT_INF));

		//// データセット
		memcpy(pWk->cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);
		// 基本特徴量セット
		memcpy(pWk->attr_base, pAttr, sizeof(pWk->attr_base));
		// 角度別特徴量セット
		memcpy(pWk->attr_angle, &pAttr[MAX_ATTR_BASE], sizeof(pWk->attr_angle));	// 角度特徴量は、基本の後ろにある為

		// 疵基本情報セット
		memcpy(&pWk->data, &data, sizeof(DETECT_BASE_DATA) );

		// 疵発生位置
		pWk->dLen = dLen;

		// 拡張部分疵情報セット
		pWk->nLowGrFlg			= nLowGrFlg;												// 無害疵一時保存フラグ (低Gr <=x< 高Gr の範囲内の場合、1)	
		pWk->nIsDspFlg			= (bObjTansho ? 1 : 0);								// 探傷画面表示フラグ
		pWk->bImgSkip	  = 9 == nAddDb ? true : false;								// 疵画像スキップ

		// 表示機能用 情報セット(上でセットしてる情報と被ってる物もある 要整理)
		pWk->data.nObjTansho = pWk->nIsDspFlg;

		// 疵基本情報セット
		pWk->dSpmHosei = pCoilInf->dSpmHosei;										// SPM補正率

		// 幅方向絶対位置セット
		pWk->data.nWidAbsPos = (int)pAttr[ABS_X_POS];

		// 画像データセット
		pWk->isBmp = pImg->bIsBmp;
		for(int ii=0; ii<MAX_CAMANGLE; ii++) {
			pWk->nImgsize[ii] = pImg->nSize[ii];
			pWk->pImg[ii]	  = new BYTE[ pImg->nSize[ii] ];
			memcpy(pWk->pImg[ii], pImg->pImg[ii], pImg->nSize[ii] );
/*/◆
CString s;
s.Format("to%d.jpg", ii);
std::ofstream   os;
os.open(s, std::ios::binary);
os.write((const char*)pWk->pImg[ii], pImg->nSize[ii] );
os.close();
//*/
		}

		//// 登録依頼
		// ※ 上でフローチェックをするようにしたので、必要無いはずだが、とりあえず残しておく。
		if( ! mque_pAddSQLData->AddToTailFreeCheck(pDeli, 50) ) {
			// DB管理クラスに通知
			if(0 == pWk->nLowGrFlg ) mcls_pDbMgr->SetEvSaveSkip(my_nTorB);					// 有害疵Grの場合 DB未保存通知
			mcls_pDbMgr->SetSaveSkipAdd(pWk->cKizukenNo, my_nTorB, pWk->nLowGrFlg );		// 削除件数を加算

			// DBが非常に遅い (通常、あまり無いはず)
			if( 0 == pWk->nLowGrFlg && ! mcls_pDbMgr->IsSaveSkipState(my_nTorB) ) {
				LOG(em_ERR), "[%s] DB登録依頼キューフル [DBDATA_DEFECT_INF]", my_sThreadName);
				syslog(SYSNO_QUEFULL_WAR, "[DBDATA_DEFECT_INF]");
			}

			// キューフル
			for(int ii=0; ii<MAX_CAMANGLE; ii++) { SAFE_DELETE_ARRAY(pWk->pImg[ii]); }
			delete pDeli->data;
			delete pDeli;
		}
	}


#ifndef DISABLE_GAP_CALC
	//================================================
	// 複眼ズレ集計対象？
	//	・集計対象の下限～上限の間に入っている場合、集計対象
	//
	if( mcls_pParam->Check_Pall_Output(ParamManager::OUTPUTPALLFLG_DEFLIST_L, tid, gid, true) &&
		mcls_pParam->Check_Pall_Output(ParamManager::OUTPUTPALLFLG_DEFLIST_H, tid, gid, false) ) {
		
		mcls_pGap->AddDetect(pAttr);
	}
#endif


	//================================================
	//// 警報出力用情報セット
	bool bAdd = true;
	bAdd &= (0 != pAttr[EPC_ATTR_BASE_MERGE]) ? false : true;
	if (bAdd) {
		SetAlameInf(&data, pAttr);
	}


}


//==========================================
// マージの実行
//==========================================
void HtRecv::ExecMerge()
{
	//================================================
	// 個々のデータ個数を取得
	HT_TO_DATA*		pHtd[NUM_CAM_POS];				// 全カメラのフレームヘッダーポインタ (疵無しの場合は、NULLがセットされている)
	int				nDetectCnt[NUM_CAM_POS];		// 欠陥個数 (PCID)=カメラセット順
	int				nDetectTotalCnt = 0;

	for(int ii=0; ii<NUM_CAM_POS; ii++) {
		// データ部あり？
		HT_TO_DATA& htd  = (HT_TO_DATA&)this->my_cRecvData[ii][0];

		if( sizeof(HT_TO_HEAD) == ((HT_TO_HEAD*)&my_cRecvHead[ii])->base_head.len ) {
			pHtd[ii]		= NULL;
			nDetectCnt[ii]	= 0;							// データ無し (ヘッダー部のみ)
		} else {
			pHtd[ii]		= &htd;
			nDetectCnt[ii]	= htd.defect_num;
			nDetectTotalCnt+= htd.defect_num;
		}
	}

	//----------------------------
	// 優先度セット (マージ機能強化対応)
	NCL_DEFECT_RESULT*		pResu;
	double*					pAttr;							// 特徴量
	for(int ii=0; ii<NUM_CAM_POS; ii++) {
		for( int jj=0; jj<nDetectCnt[ii]; jj++) {
			pResu = (NCL_DEFECT_RESULT*) &GET_PTR_RESULT(ii, pHtd[ii]->result_offset, jj);
			pAttr = (double*)			 &GET_PTR_ATTR(ii, pHtd[ii]->attr_offset, jj);


			// 太平オリジナルエリアにセット
			pAttr[EPC_ATTR_BASE_YU_G] = mcls_pParam->GetYusenG(pResu->grade);
			pAttr[EPC_ATTR_BASE_YU_T] = mcls_pParam->GetYusenT(pResu->type);			
		}
	}


	//================================================
	// マージの実行
	if( 0 != nDetectTotalCnt &&
		(DIV_UNTEN_NOMAL == mcls_pStatus->GetUnten() || DIV_UNTEN_SMYU == mcls_pStatus->GetUnten()) ) { 
		// パラメータセット
		MergeManager::PARAM_MERGE prmMerge;				// マージに必要なパラメータセット
		prmMerge.dLapRitu = mcls_pParam->GetParamCommon().dLapRate;
		for(int ii=0; ii<NUM_CAM_POS; ii++) {
			int camset = NUM_CAM_POS*my_nTorB + ii;
			prmMerge.dOffset_x[ii] = mcls_pParam->GetParamCamSet().camset[camset].dXoffset; 
			//LOG(em_INF), "[%s] [%02d] offset=%.4f", my_sThreadName, camset, prmMerge.dOffset_x[ii]);
		}

		// 下準備
		MergeManager  cls_Merge;
		cls_Merge.SetLogMgr(mcls_pLog);
		cls_Merge.SetParam(&prmMerge);

		//----------------------------
		// 特殊なので、AP側でセット
		int cnt = 0;
		MergeManager::MergeInfo* lst = new MergeManager::MergeInfo[nDetectTotalCnt];			// 疵リスト

		for(int ii=0; ii<NUM_CAM_POS; ii++) {
			for( int jj=0; jj<nDetectCnt[ii]; jj++) {
				lst[cnt].pAttr		= (double*) &GET_PTR_ATTR(ii, pHtd[ii]->attr_offset, jj);
				//
				lst[cnt].nCamset	= (int)lst[cnt].pAttr[CAMSET];
				//
				lst[cnt].yusenG		= (int)lst[cnt].pAttr[EPC_ATTR_BASE_YU_G];
				lst[cnt].yusenT		= (int)lst[cnt].pAttr[EPC_ATTR_BASE_YU_T];
				lst[cnt].area		= lst[cnt].pAttr[AREA];
				lst[cnt].len		= lst[cnt].pAttr[CENTER_Y];
				//
				lst[cnt].xmin		= lst[cnt].pAttr[BOX_X_MIN] * lst[cnt].pAttr[HORZ_RES] + prmMerge.dOffset_x[ii];
				lst[cnt].xmax		= lst[cnt].pAttr[BOX_X_MAX] * lst[cnt].pAttr[HORZ_RES] + prmMerge.dOffset_x[ii];
				lst[cnt].ymin		= lst[cnt].pAttr[BOX_Y_MIN] * lst[cnt].pAttr[VERT_RES];
				lst[cnt].ymax		= lst[cnt].pAttr[BOX_Y_MAX] * lst[cnt].pAttr[VERT_RES];
				//
				lst[cnt].lstLap		= NULL; 
				lst[cnt].isChecked	= false;
				cnt++;
			}
		}

		//// 実行
		cls_Merge.DoMerge( nDetectCnt, lst);

		//// 解放
		SAFE_DELETE_ARRAY(lst);
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AGC 
//#define DEBUG_PRINT_AGC			// 定義時 ログ出力

//==========================================
// AGC実行
//------------------------------------------
// DWORD	framenum	: 現在のフレームNo	
//==========================================
void HtRecv::Check_AGC(DWORD framenum)
{
	// 探傷エンジンでAGCを実行するため、統括では未実施
}

//==========================================
// コイルトレンド情報を登録
//==========================================
void HtRecv::SetCoilTrand()
{
	float dWk1, dWk2;

	//// 事前準備
	DBManager::DeliveryDB* pDeli = new DBManager::DeliveryDB;
	pDeli->kubun = DBManager::em_COILTRAND;
	pDeli->data  = (BYTE*)new DBDATA_COIL_TRAND;
	DBDATA_COIL_TRAND* pWk = (DBDATA_COIL_TRAND*)pDeli->data;
	memset(pWk, 0x00, sizeof(DBDATA_COIL_TRAND));

	//================================================
	//// データセット
	memcpy(pWk->cKizukenNo, mcls_pCoil->GetCoilKenV(my_nTorB)->cKizukenNo, SIZE_KIZUKEN_NO);
	pWk->nMen		= my_nTorB; 
	pWk->nLen		= ((HT_TO_HEAD*)&my_cRecvHead[0])->frame_pos;
	pWk->nSpeed		= mcls_pPlg->GetSpeed();		// ライン速度
	mcls_pEdgePosCont->SearchEdgeRawPos(my_nTorB, dWk1, dWk2);
	//dWk1			= mcls_pCoil->GetCoilKenV(my_nTorB)->dEdge[mcls_pParam->GetCommonMast().nWalkMen][m_nNowGridRow][0];	// 生エッジ位置
	//dWk2			= mcls_pCoil->GetCoilKenV(my_nTorB)->dEdge[mcls_pParam->GetCommonMast().nWalkMen][m_nNowGridRow][1];	// 生エッジ位置
	pWk->dCoilWid	= abs(dWk1) + abs(dWk2);		// 板幅(生エッジ位置から取得)
	pWk->dWalk      = (dWk1 + dWk2) / 2.0f;			// ウォーク量(ラインセンターからの距離)

	mcls_pEdgePosCont->SearchEdgeRawPosMax(my_nTorB, dWk1, dWk2);
	pWk->dCoilWidMax	= abs(dWk1) + abs(dWk2);	// 板幅最大(生エッジ位置から取得)
	mcls_pEdgePosCont->SearchEdgeRawPosMin(my_nTorB, dWk1, dWk2);
	pWk->dCoilWidMin	= abs(dWk1) + abs(dWk2);	// 板幅最小(生エッジ位置から取得)

	pWk->dSpmHosei	= mcls_pCoil->GetCoilKenV(my_nTorB)->dSpmHosei;		// SPM補正率
//LOG(em_INF), "[%s] [DBDATA_COIL_TRAND] '%s',%d,%d (%d %.4f)", my_sThreadName, pWk->cKizukenNo, my_nTorB, (int)((double)pWk->nLen * pWk->dSpmHosei), pWk->nLen, pWk->dSpmHosei);

	for(int ii=0; ii<NUM_CAM_POS; ii++){
		HT_TO_HEAD* pHth = (HT_TO_HEAD*)&my_cRecvHead[ii];	
		HT_TO_DATA* pDth = (HT_TO_DATA*)&my_cRecvData[ii][0];	

#ifndef HT_IS_COLOR				// 輝度
		for(int jj=0; jj<NUM_CAM_ANGLE; jj++) {
			pWk->nBr[ii][jj][0]	= pHth->br[jj].br_center;
			pWk->nBr[ii][jj][1]	= pHth->br[jj].br_l_ovl;
			pWk->nBr[ii][jj][2]	= pHth->br[jj].br_r_ovl;

			pWk->nExp[ii][jj]	= mcls_pCoil->GetCamExpTime(my_nTorB, ii, jj);
		}
#else							// 色差
		for(int jj=0; jj<(NUM_CAM_ANGLE -1); jj++) {
			pWk->nBr[ii][jj][0]	= pHth->br[jj];						// 輝度

		}
		for(int jj=0; jj<(NUM_CAM_ANGLE -1); jj++) {
			pWk->nExp[ii][jj] = mcls_pCoil->GetCamExpTime(my_nTorB, ii, jj);	// 露光時間
		}
		for (int jj = 0; jj<(NUM_CAM_ANGLE -1); jj++) {
			pWk->dGain[ii][jj] = mcls_pCoil->GetCamGain(my_nTorB, ii, jj);		// カメラゲイン値
		}
#endif
	}
	for(int ii=0; ii<DIV_LED_COLOR_END; ii++) {
		pWk->nLightValue[ii] = mcls_pCoil->GetLedValue(my_nTorB,ii);
	}

	//================================================
	//// 登録依頼
	if( ! mque_pAddSQLData->AddToTailFreeCheck(pDeli, 100) ) {	// 登録
		// 絶対登録も無理・・・
		LOG(em_ERR), "[%s] DB登録依頼キューフル [DBDATA_COIL_TRAND]", my_sThreadName);
		//syslog(SYSNO_QUEFULL_WAR, "[DBDATA_COIL_TRAND]");
		delete pDeli->data;
		delete pDeli;
	}
}

//==========================================
// 基本疵情報 生成
//------------------------------------------
// int					men		: 0:表 1:裏
// int					coilwid	: コイル幅 [mm]
// int					nMapCol	: コイル分割数
// NCL_DEFECT_RESULT*	pResu	: 結果
// double*				attr	: 基本特徴量
// int				nShrinkRatio: 縮小率
// DETECT_BASE_DATA*	data	: 基本疵情報
//==========================================
void HtRecv::Create_DetectBaseData(int men, int coilwid, int nMapCol, NCL_DEFECT_RESULT const* pResu, double const* attr, int nShrinkRatio, DETECT_BASE_DATA* data)
{
	// 事前準備
	double dkiriX		= attr[IMAGE_X] / attr[HORZ_RES];						// 切り出し画像左上のPixel位置
	double dkiriY		= attr[IMAGE_Y] / attr[VERT_RES];						// 切り出し画像左上のPixel位置

	// データセット
	data->TorB			=			men;
	//data->nTid			= (WORD)	attr[DEFECT_TYPE];
	//data->nGid			= (WORD)	attr[DEFECT_GRADE];
	data->nTid			= (WORD)	pResu->type;			// 振り替え後にする
	data->nGid			= (WORD)	pResu->grade;			// 振り替え後にする
		

	// フラグ (簡易周期の場合は、1固定)
	if(      ((DWORD)attr[DEFECT_FLAG] & NCL_DEFECT_FLAGS::PERIODIC)  == NCL_DEFECT_FLAGS::PERIODIC ) {	// 簡易周期での周期疵
		data->cycle_div	= 1;
	} else if( ((DWORD)attr[DEFECT_FLAG] & NCL_DEFECT_FLAGS::SERIES) == NCL_DEFECT_FLAGS::SERIES ) {	// 簡易周期での連続疵
		data->cycle_div	= 1;
	} else {
		data->cycle_div	= 0;
	}

	data->camset		= (WORD)	attr[CAMSET];

	data->nKizuNo		= (long)	attr[EPC_ATTR_BASE_KIZUNO];				// 疵No (表裏共通で1～の連番)

	data->y				= (float)	attr[MERGED_CENTER_Y];
	data->y_out			= data->y;												// ここでは同値をセット
	if( 0 == CAMLEFT_IS_DS(men) ) {												// カメラ左側がWS対応
		data->ds		= (float)	attr[CENTER_X_FROM_L_EDGE];
		data->ws		= (float)	attr[CENTER_X_FROM_R_EDGE];
	} else {
		data->ds		= (float)	attr[CENTER_X_FROM_R_EDGE];
		data->ws		= (float)	attr[CENTER_X_FROM_L_EDGE];
	}
	// 基準エッジは、単純に近いほうを採用する
	data->dsws	= ( data->ds <= data->ws ) ? 0 : 1;
	

	data->dsc			= (float)	((coilwid/2.0) - data->ds);
	data->wsc			= (float)	((coilwid/2.0) - data->ws);
	data->speed			= (int)		( attr[LINE_SPEED] * 60.0f / 1000.0f + 0.5f );
	data->pitch			= (int)		attr[CYCLE_LENGTH];
	data->roolkei		= (float)	( attr[CYCLE_LENGTH] / M_PI );

	data->aria			= (float)	attr[AREA];
	data->kizu_wid		= (int)		attr[FERET_X];
	data->kizu_len		= (int)		attr[FERET_Y];

	data->h_res			= (float)	(attr[VERT_RES] * (double)nShrinkRatio);	// 縦分解能[mm/pixel] ※縮小率を考慮
	data->w_res			= (float)	(attr[HORZ_RES] * (double)nShrinkRatio);	// 横分解能[mm/pixel] ※縮小率を考慮
	data->nFrameNo		= (long)	attr[FRAME_NUM];	

	data->frm_x_min		= (short)	attr[BOX_X_MIN];
	data->frm_x_max		= (short)	attr[BOX_X_MAX];
	data->frm_y_min		= (short)	attr[BOX_Y_MIN];
	data->frm_y_max		= (short)	attr[BOX_Y_MAX];

	data->cull_frm_x_min		= (short)	(attr[BOX_X_MIN] / attr[EPC_ATTR_BASE_IMG_CULL_X]);
	data->cull_frm_x_max		= (short)	(attr[BOX_X_MAX] / attr[EPC_ATTR_BASE_IMG_CULL_X]);
	data->cull_frm_y_min		= (short)	(attr[BOX_Y_MIN] / attr[EPC_ATTR_BASE_IMG_CULL_Y]);
	data->cull_frm_y_max		= (short)	(attr[BOX_Y_MAX] / attr[EPC_ATTR_BASE_IMG_CULL_Y]);

	// 切り出し画像セット (原点は、切り出し画像の左上からの位置)
	// ※縮小率を考慮
	data->box_x_min = (short)((attr[BOX_X_MIN] - dkiriX) / (double)nShrinkRatio);
	data->box_x_max = (short)((attr[BOX_X_MAX] - dkiriX) / (double)nShrinkRatio);
	data->box_y_min = (short)((attr[BOX_Y_MIN] - dkiriY) / (double)nShrinkRatio);
	data->box_y_max = (short)((attr[BOX_Y_MAX] - dkiriY) / (double)nShrinkRatio);
	data->center_x = (short)(((attr[CENTER_X] / attr[HORZ_RES]) - dkiriX) / (double)nShrinkRatio);
	data->center_y = (short)(((attr[CENTER_Y] / attr[VERT_RES]) - dkiriY) / (double)nShrinkRatio);
//	data->h_size		= // この時点では不明
//	data->w_size		= 

// 元画像の切り出し画像セット (原点は、切り出し画像の左上からの位置)
// ※縮小前の座標系で送られてくるので、そのままセット
	data->base_box_x_min = (short)(attr[BOX_X_MIN] - dkiriX);
	data->base_box_x_max = (short)(attr[BOX_X_MAX] - dkiriX);
	data->base_box_y_min = (short)(attr[BOX_Y_MIN] - dkiriY);
	data->base_box_y_max = (short)(attr[BOX_Y_MAX] - dkiriY);
	data->base_center_x = (short)((attr[CENTER_X] / attr[HORZ_RES]) - dkiriX);
	data->base_center_y = (short)((attr[CENTER_Y] / attr[VERT_RES]) - dkiriY);
//	data->base_h_size	 = // この時点では不明
//	data->base_w_size	 =
	data->base_h_res	 = (float)attr[VERT_RES];	// 元の縦分解能[mm/pixel]
	data->base_w_res	 = (float)attr[HORZ_RES];	// 元の横分解能[mm/pixel]
	data->nShrinkRatio	 = nShrinkRatio;			// 縮小率

	//data->nFrameDefMax	= (WORD)	attr[EPC_ATTR_BASE_FRAME_DEFECT_MAX];
	data->nTYuu			= (WORD)	attr[EPC_ATTR_BASE_YU_T];
	data->nGYuu			= (WORD)	attr[EPC_ATTR_BASE_YU_G];

// 2023.09.14 画像処理装置 H・Sソフト改造 -------------------->>>>>
	data->nIFMergeFlg	= (short)		attr[IFMERGE_FLAG];
// 2023.09.14 画像処理装置 H・Sソフト改造 <<<<<--------------------

	//================================================
	// マップ横グリッド位置算出
	int col;													// 内部メッシュの列位置 (0オリジン)
#ifndef PLG_DSP_MAP_FIX
	col		= KizuFunction::SelectColPosition2(data->dsws, data->ds, data->ws, coilwid, nMapCol, MAP_COL_NUM, MAP_COL_INTERVAL );
#else
	col		= KizuFunction::SelectColPositionFix(data->dsws, data->ds, data->ws, coilwid, MAP_COL_NUM);
#endif

	// 整合性確認
	if( 0 > col || col >= MAP_COL_NUM ) {
		if( col < 0 )	col = 0;
		else			col = MAP_COL_NUM - 1;
	}
	data->nWidDiv		= col;


	//// 付帯情報 ------------------------------------------
	//data->nCycleNo		= (int) attr[CYCLE_SERIAL];					// 周期疵連番


}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 警報
//------------------------------------------
// 警報出力対象欠陥の判定を行う
//------------------------------------------
// DETECT_BASE_DATA const* pD 基本情報
//------------------------------------------
void HtRecv::SetAlameInf(DETECT_BASE_DATA const* pD, double const* pAttr)
{
	// ★警報判定処理はコイル情報管理に移動
	mcls_pCoil->SetAlameInf(pD, my_nTorB, mcls_pCoil->GetCoilKen_DataSet(my_nTorB), pAttr);
}

//------------------------------------------
// 代表疵、疵リストクリア
//------------------------------------------
void HtRecv::ClearDefectBuffer()
{
	for (int ii = 0; ii<MAP_COL_NUM; ii++)
	{
		for (int jj = 0; jj<MAX_CAMANGLE; jj++)
		{
			if (NULL != m_OpSendDefInfo.ColnowBuf_Now.BufDefCol[ii].pImg[jj])
			{
//LOG(em_INF), "[%s] 代表疵画像解放 kizuNo= %d men=%d col:%d addr:0x%x", my_sThreadName,
//	m_OpSendDefInfo.ColnowBuf_Now.BufDefCol[ii].Data.nKizuNo, my_nTorB, ii, m_OpSendDefInfo.ColnowBuf_Now.BufDefCol[ii].pImg[jj]);
				SAFE_DELETE_ARRAY(m_OpSendDefInfo.ColnowBuf_Now.BufDefCol[ii].pImg[jj]);
			}
		}
	}
	for (int ii = 0; ii<MAP_COL_NUM; ii++)
	{
		for (int jj = 0; jj < MAP_NEST_NUM; jj++)
		{
			for (int kk = 0; kk<MAX_CAMANGLE; kk++)
			{
				if (NULL != m_OpSendDefInfo.ColnowBuf_Now.BufDefAry[ii][jj].pImg[kk])
				{
//LOG(em_INF), "[%s] 疵リスト画像解放 kizuNo= %d men=%d col:%d no:%d addr:0x%x", my_sThreadName,
//	m_OpSendDefInfo.ColnowBuf_Now.BufDefAry[ii][jj].Data.nKizuNo, my_nTorB, ii, jj, m_OpSendDefInfo.ColnowBuf_Now.BufDefAry[ii][jj].pImg[kk]);
					SAFE_DELETE_ARRAY(m_OpSendDefInfo.ColnowBuf_Now.BufDefAry[ii][jj].pImg[kk]);
				}
			}
		}
	}
}

//------------------------------------------
// 代表疵選定バッファ初期化
// int nFramePos	フレーム位置[mm]
//------------------------------------------
void HtRecv::ReSetOp_DetectData(int nFramePos)
{
	//// 代表疵バッファ (この時点で残っている事は無いはず)
	// インスタンス開放
	ClearDefectBuffer();

	// 次の最小区間バッファをカレントにコピー
	memcpy(&m_OpSendDefInfo.ColnowBuf_Now, &m_OpSendDefInfo.ColnowBuf_Next, sizeof(m_OpSendDefInfo.ColnowBuf_Next));
	memset(&m_OpSendDefInfo.ColnowBuf_Next, 0x00, sizeof(m_OpSendDefInfo.ColnowBuf_Next));
}

//------------------------------------------
// 疵情報を表示管理送信用構造体に変換
// TYP_COLROW_BUF *	pBuf 疵が含まれるコイル情報
// double dLen 疵発生位置
// DETECT_BASE_DATA const* pData 検査結果マトメ
// DEFECT_DATA_IMAGE* pImg 太平用画像情報ポインタ
//------------------------------------------
void HtRecv::DetectToColRow(TYP_COLROW_BUF *pBuf, double dLen, DETECT_BASE_DATA const *pData, DEFECT_DATA_IMAGE const *pImg)
{
	pBuf->umu = true;
	pBuf->bIsBmp = pImg->bIsBmp;
	pBuf->dLen = dLen;

	// 疵情報コピー
	memcpy(&pBuf->Data, pData, sizeof(DETECT_BASE_DATA));

	// 画像コピー
	int nSize;
	for (int ii = 0; ii < MAX_CAMANGLE; ii++)
	{
		nSize = pImg->nSize[ii];
		pBuf->nImgsize[ii] = nSize;
		if (0 < nSize)
		{
			pBuf->pImg[ii] = new BYTE[nSize];
			memcpy(pBuf->pImg[ii], pImg->pImg[ii], nSize);
		}
	}
}

//------------------------------------------
// 表示管理送信用疵情報編集
// COIL_INF const*	pCoilInf 疵が含まれるコイル情報
// DETECT_BASE_DATA const* pData 検査結果マトメ
// DEFECT_DATA_IMAGE const* pImg 太平用画像情報ポインタ
// double dLen 疵発生位置（入側先端からの位置[mm]）
// double dFramePos 現在のフレーム位置[mm]
//------------------------------------------
void HtRecv::SetBuf_DetectData(COIL_INF const* pCoilInf, DETECT_BASE_DATA const* pData, DEFECT_DATA_IMAGE const* pImg, double dLen, double dFramePos)
{
	int col			= pData->nWidDiv;		// 内部メッシュの列位置 (0オリジン)
	int nRange		= (MAP_REPRES_MIN * 1000);
	int nStartPos	= (int)(dFramePos / (double)nRange) * nRange;
	int nEndPos		= nStartPos + nRange;

#ifdef DEF_DETECT_LOG
	LOG(em_INF), "[%s] 現在の最小区間：[st:%d ed:%d]", my_sThreadName, nStartPos, nEndPos);
#endif

	int nGridRow = (int)(0 <= dLen ? dLen / (double)nRange : 0);	// 仮に疵発生位置が０未満の場合は０行目とする
	int nDefCnt	 = 1;												// 行位置が範囲外（最大コイル長超え）でも今回の疵数１を保証

	// マップ最大行数（最大コイル長）を超えていたら、最終行の個数に加算
	if (MAP_ROW_NUM <= nGridRow) nGridRow = MAP_ROW_NUM - 1;
	// 長手位置単位の疵数を加算し、加算後の値を返す
	if (0 != pData->nObjTansho)
	{
		// 探傷画面出力対象
		// ※現在の疵数に＋１して返す
		nDefCnt = mcls_pCoil->SetDefectCount(my_nTorB, pCoilInf->cKizukenNo, nGridRow);
	}
	else
	{
		// 探傷画面出力対象外
		// ※現在の疵数を返す
		nDefCnt = mcls_pCoil->GetDefectCount(my_nTorB, pCoilInf->cKizukenNo, nGridRow);
	}
	// 加算先が見つからなかった場合、今回の疵数１を保証
	if (-1 == nDefCnt) nDefCnt = 1;

	// 前の最小区間の疵
	if (dLen < (double)nStartPos)
	{
#ifdef DEF_DETECT_LOG
		LOG(em_WAR), "[%s] 疵検知(通板済の最小区間の疵) kizuNo=%d 疵種:%d グレード:%d col:%d len=%.2f", my_sThreadName, pData->nKizuNo, pData->nTid, pData->nGid, col, dLen);
#endif

		// 即座に送信する

		// 疵情報変換
		TYP_COLROW_BUF	wkBuf;
		memset(&wkBuf, 0x00, sizeof(wkBuf));
		DetectToColRow(&wkBuf, dLen, pData, pImg);

		// １疵情報送信
		OpSendFunc::SendOp_ColRowData(my_nTorB, pCoilInf, &wkBuf);

		// 疵数送信
#ifdef DEF_DETECT_LOG
		LOG(em_INF), "[%s] 統括⇒表示間 疵数送信 [長手位置=%d 疵数=%d]", my_sThreadName,
			nGridRow * nRange, nDefCnt);
#endif
		OpSendFunc::SendOp_DetectNum(pCoilInf->cKizukenNo, my_nTorB, nGridRow * nRange, nDefCnt);

		// 画像領域解放
		for (int ii = 0; ii < MAX_CAMANGLE; ii++)
		{
			if (0 < wkBuf.nImgsize[ii])
			{
				SAFE_DELETE_ARRAY(wkBuf.pImg[ii]);
				wkBuf.pImg[ii] = NULL;
			}
		}
	}
	else if (dLen >= (double)nStartPos && dLen < (double)nEndPos)
	{
		// カレントバッファに登録して、最小区間切替、コイル切替時にまとめて送信
		SetOp_DetectData(mcls_pCoil->GetCoilKenV(my_nTorB), pData, pImg, dLen, &m_OpSendDefInfo.ColnowBuf_Now);
	}
	else
	{
		if (dLen < (double)(nEndPos + nRange))
		{
			// 次最小区間バッファに登録する
#ifdef DEF_DETECT_LOG
			LOG(em_WAR), "[%s] 疵検知(次の最小区間の疵) kizuNo=%d 疵種:%d グレード:%d col:%d len=%.2f", my_sThreadName, pData->nKizuNo, pData->nTid, pData->nGid, col, dLen);
#endif
			// 次最小区間バッファに登録して、最小区間切替、コイル切替時にまとめて送信
			SetOp_DetectData(mcls_pCoil->GetCoilKenV(my_nTorB), pData, pImg, dLen, &m_OpSendDefInfo.ColnowBuf_Next);
		}
		else
		{
			// 次最小区間より先の疵
#ifdef DEF_DETECT_LOG
			LOG(em_WAR), "[%s] 疵検知(次の最小区間より先の疵) kizuNo=%d 疵種:%d グレード:%d col:%d len=%.2f", my_sThreadName, pData->nKizuNo, pData->nTid, pData->nGid, col, dLen);
#endif
			// 即座に送信する

			// 疵情報変換
			TYP_COLROW_BUF	wkBuf;
			memset(&wkBuf, 0x00, sizeof(wkBuf));
			DetectToColRow(&wkBuf, dLen, pData, pImg);

			// １疵情報送信
			OpSendFunc::SendOp_ColRowData(my_nTorB, pCoilInf, &wkBuf);

			// 疵数送信
#ifdef DEF_DETECT_LOG
			LOG(em_INF), "[%s] 統括⇒表示間 疵数送信 [長手位置=%d 疵数=%d]", my_sThreadName,
				nGridRow * nRange, nDefCnt);
#endif
			OpSendFunc::SendOp_DetectNum(pCoilInf->cKizukenNo, my_nTorB, nGridRow * nRange, nDefCnt);

			// 画像領域解放
			for (int ii = 0; ii < MAX_CAMANGLE; ii++)
			{
				if (0 < wkBuf.nImgsize[ii])
				{
					SAFE_DELETE_ARRAY(wkBuf.pImg[ii]);
					wkBuf.pImg[ii] = NULL;
				}
			}
		}
	}
}

//------------------------------------------
// 代表疵選定＆疵リストまとめ
// COIL_INF const*	pCoilInf 疵が含まれるコイル情報
// DETECT_BASE_DATA const* pData 検査結果マトメ
// DEFECT_DATA_IMAGE* pImg 太平用画像情報ポインタ
// double dLen 疵発生位置
// 戻り値：0:正常, -1:マップ位置不正
//------------------------------------------
int HtRecv::SetOp_DetectData(COIL_INF const* pCoilInf, DETECT_BASE_DATA const* pData, DEFECT_DATA_IMAGE const* pImg, double dLen, TYP_COLROW_INFO *pColRow_Info)
{
	int nDataSet	= 0;					// バッファにデータセットする？しない？ (0:無し  1:新規  2:新が強い 3:新が弱い)
	int col			= pData->nWidDiv;		// 内部メッシュの列位置 (0オリジン)
	TYP_COLROW_BUF	wkBuf;					// ワーク

	if (MAP_COL_NUM <= col)
	{
		LOG(em_ERR), "[%s] 幅分割区分 異常 kizuNo= %d col:%d", my_sThreadName, pData->nKizuNo, col);
		return -1;
	}

#ifdef DEF_DETECT_LOG
	LOG(em_INF), "[%s] 疵検知 kizuNo=%d 疵種:%d グレード:%d col:%d len=%.2f", my_sThreadName, pData->nKizuNo, pData->nTid, pData->nGid, col, dLen);
#endif

	memset(&wkBuf, 0x00, sizeof(wkBuf));

	////////////////////////////////////////////////////////////////////////////
	// １段目。列単位で代表疵を算出

	// 同列内に代表疵有無
	if (!pColRow_Info->BufDefCol[col].umu)
	{
		// 新規
		nDataSet = 1;
	}
	else
	{
		// 既存データ有り

		// バッファ内と優先度比較
		if (SelectPriorityDetect(pData, &pColRow_Info->BufDefCol[col].Data))
		{
			// 新が強い
			nDataSet = 2;

			// 古い奴はワークに移動
			memcpy(&wkBuf, &pColRow_Info->BufDefCol[col], sizeof(wkBuf));
			// 画像領域は再利用するか下で解放
			memset(&pColRow_Info->BufDefCol[col], 0x00, sizeof(pColRow_Info->BufDefCol[col]));
		}
		else
		{
			// 新が弱かった
			nDataSet = 3;
		}
	}

	// 今回の疵情報をバッファにセット
	// ＃新規 or 新が強い
	if (1 == nDataSet || 2 == nDataSet)
	{
		// 今回のデータをセット
		DetectToColRow(&pColRow_Info->BufDefCol[col], dLen, pData, pImg);

#ifdef DEF_DETECT_LOG
		LOG(em_INF), "[%s] 代表疵登録 kizuNo= %d 疵種:%d グレード:%d col:%d", my_sThreadName,
			pColRow_Info->BufDefCol[col].Data.nKizuNo, pData->nTid, pData->nGid, col);
#endif

		if (1 == nDataSet)
		{
			// 新規登録の場合はここまで
			return 0;
		}
	}

	////////////////////////////////////////////////////////////////////////////
	// ２段目。列単位でリスト疵を算出

	// 今回のセット位置
	// ＃デフォルト：対象列の最後の位置
	int nIndex = pColRow_Info->nSetCnt[col];

	DETECT_BASE_DATA const* pNewWk;
	if (2 == nDataSet)
	{
		// 新が強い

		// 代表から追い出された疵と比較
		pNewWk = &wkBuf.Data;
	}
	else if (3 == nDataSet)
	{
		// 新が弱い

		// 今回疵と比較
		// ＃代表疵には入らなかった
		pNewWk = pData;
	}

	for (int ii = 0; ii < pColRow_Info->nSetCnt[col]; ii++)
	{
		// バッファ内と優先度比較
		if (SelectPriorityDetect(pNewWk, &pColRow_Info->BufDefAry[col][ii].Data))
		{
			// 今回の疵が優先度高
			nIndex = ii;
			break;
		}
	}

	// セット対象外
	// ＃リストMAXでかつ、今回疵が最も弱い
	if (MAP_NEST_NUM == nIndex)
	{
		nIndex = -1;

		if (2 == nDataSet)
		{
			// 新が強い

			// 払い出された代表疵の強制送信
			// 重欠陥一覧強調表示対象疵の場合は即時送信
			// ※重欠陥対象疵が連続発生時に、まとめ送信機能により
			// 　重欠陥一覧強調表示対象疵が破棄されてしまう問題対策
			if (1 == wkBuf.Data.nModeDeflist)
			{
				// 払いだされる重欠陥一覧強調表示対象疵情報を送信
				OpSendFunc::SendOp_ColRowData(my_nTorB, pCoilInf, &wkBuf);

//#ifdef DEF_DETECT_LOG
				LOG(em_INF), "[%s] 疵送信(払い出し重欠陥一覧強調表示対象疵：代表疵) kizuNo=%d 疵種:%d グレード:%d col:%d", my_sThreadName,
					wkBuf.Data.nKizuNo, wkBuf.Data.nTid, wkBuf.Data.nGid, col);
//#endif
			}

			// 代表から追い出された疵を削除
			for (int ii = 0; ii < MAX_CAMANGLE; ii++)
			{
				if (0 < wkBuf.nImgsize[ii])
				{
					SAFE_DELETE_ARRAY(wkBuf.pImg[ii]);
					wkBuf.pImg[ii] = NULL;
				}
			}
#ifdef DEF_DETECT_LOG
			LOG(em_INF), "[%s] 代表疵削除 kizuNo=%d 疵種:%d グレード:%d col:%d", my_sThreadName,
				wkBuf.Data.nKizuNo, wkBuf.Data.nTid, wkBuf.Data.nGid, col);
#endif
			memset(&wkBuf, 0x00, sizeof(wkBuf));
		}
		else if (3 == nDataSet)
		{
#ifdef DEF_DETECT_LOG
			LOG(em_INF), "[%s] 登録対象外 kizuNo=%d 疵種:%d グレード:%d col:%d", my_sThreadName,
				pData->nKizuNo, pData->nTid, pData->nGid, col);
#endif
			// 一時バッファに格納
			DetectToColRow(&wkBuf, dLen, pData, pImg);

			// 代表疵にもリスト疵にも入らない疵の強制送信
			// 重欠陥一覧強調表示対象疵の場合は即時送信
			// ※重欠陥対象疵が連続発生時に、まとめ送信機能により
			// 　重欠陥一覧強調表示対象疵が破棄されてしまう問題対策
			if (1 == wkBuf.Data.nModeDeflist)
			{
				// 払いだされる重欠陥一覧強調表示対象疵情報を送信
				OpSendFunc::SendOp_ColRowData(my_nTorB, pCoilInf, &wkBuf);

//#ifdef DEF_DETECT_LOG
				LOG(em_INF), "[%s] 疵送信(払い出し重欠陥一覧強調表示対象疵：今回疵) kizuNo=%d 疵種:%d グレード:%d col:%d", my_sThreadName,
					wkBuf.Data.nKizuNo, wkBuf.Data.nTid, wkBuf.Data.nGid, col);
//#endif
			}

			// 一時領域解放
			for (int ii = 0; ii < MAX_CAMANGLE; ii++)
			{
				if (0 < wkBuf.nImgsize[ii])
				{
					SAFE_DELETE_ARRAY(wkBuf.pImg[ii]);
					wkBuf.pImg[ii] = NULL;
				}
			}
			memset(&wkBuf, 0x00, sizeof(wkBuf));
		}
	}

	//================================================
	// 挿入
	if (-1 != nIndex)
	{
		if (MAP_NEST_NUM == pColRow_Info->nSetCnt[col])
		{
			// リストMAXの場合

			// リストから払い出される最弱疵の強制送信
			// 重欠陥一覧強調表示対象疵の場合は即時送信
			// ※重欠陥対象疵が連続発生時に、まとめ送信機能により
			// 　重欠陥一覧強調表示対象疵が破棄されてしまう問題対策
			if (1 == pColRow_Info->BufDefAry[col][MAP_NEST_NUM - 1].Data.nModeDeflist)
			{
				// 払いだされる重欠陥一覧強調表示対象疵情報を送信
				OpSendFunc::SendOp_ColRowData(my_nTorB, pCoilInf, &pColRow_Info->BufDefAry[col][MAP_NEST_NUM - 1]);
//#ifdef DEF_DETECT_LOG
				LOG(em_INF), "[%s] 疵送信(払い出し重欠陥一覧強調表示対象疵：リスト疵) kizuNo=%d 疵種:%d グレード:%d col:%d", my_sThreadName,
					pColRow_Info->BufDefAry[col][MAP_NEST_NUM - 1].Data.nKizuNo,
					pColRow_Info->BufDefAry[col][MAP_NEST_NUM - 1].Data.nTid,
					pColRow_Info->BufDefAry[col][MAP_NEST_NUM - 1].Data.nGid, col);
//#endif
			}

			// 最弱疵を開放
			pColRow_Info->nSetCnt[col]--;
			for (int ii = 0; ii < MAX_CAMANGLE; ii++)
			{
				if (0 < pColRow_Info->BufDefAry[col][MAP_NEST_NUM - 1].nImgsize[ii])
				{
					SAFE_DELETE_ARRAY(pColRow_Info->BufDefAry[col][MAP_NEST_NUM - 1].pImg[ii]);
					pColRow_Info->BufDefAry[col][MAP_NEST_NUM - 1].pImg[ii] = NULL;
				}
			}
#ifdef DEF_DETECT_LOG
			LOG(em_INF), "[%s] リスト疵削除 kizuNo=%d 疵種:%d グレード:%d col:%d", my_sThreadName,
				pColRow_Info->BufDefAry[col][MAP_NEST_NUM - 1].Data.nKizuNo, pColRow_Info->BufDefAry[col][MAP_NEST_NUM - 1].Data.nTid, pColRow_Info->BufDefAry[col][MAP_NEST_NUM - 1].Data.nGid, col);
#endif
			memset(&pColRow_Info->BufDefAry[col][MAP_NEST_NUM - 1], 0x00, sizeof(TYP_COLROW_BUF));
		}

		// 新データを差し込む位置より下位データを移動
		// ＃同領域内なので、下から順に
		for (int ii = MAP_NEST_NUM - 1; ii >= nIndex; ii--)
		{
			if (MAP_NEST_NUM - 1 == ii) continue;
			memcpy(&pColRow_Info->BufDefAry[col][ii + 1], &pColRow_Info->BufDefAry[col][ii], sizeof(TYP_COLROW_BUF));
		}

		if (2 == nDataSet)
		{
			// 新が強い

			// 代表から追い出された疵を登録
			memcpy(&pColRow_Info->BufDefAry[col][nIndex], &wkBuf, sizeof(TYP_COLROW_BUF));
#ifdef DEF_DETECT_LOG
			LOG(em_INF), "[%s] リスト疵登録(代表疵) kizuNo=%d 疵種:%d グレード:%d col:%d", my_sThreadName,
				pColRow_Info->BufDefAry[col][nIndex].Data.nKizuNo, pData->nTid, pData->nGid, col);
#endif
		}
		else if (3 == nDataSet)
		{
			// 新が弱い

			// 今回疵を登録
			DetectToColRow(&pColRow_Info->BufDefAry[col][nIndex], dLen, pData, pImg);

#ifdef DEF_DETECT_LOG
			LOG(em_INF), "[%s] リスト疵登録(今回疵) kizuNo=%d 疵種:%d グレード:%d col:%d", my_sThreadName,
				pColRow_Info->BufDefAry[col][nIndex].Data.nKizuNo, pData->nTid, pData->nGid, col);
#endif
		}
	}

	//================================================
	//// 表示対象の欠陥候補が有ったので、件数加算
	if (-1 != nIndex) pColRow_Info->nSetCnt[col]++;
	if (MAP_NEST_NUM < pColRow_Info->nSetCnt[col]) pColRow_Info->nSetCnt[col] = MAP_NEST_NUM;

	return 0;
}

//------------------------------------------
// 代表疵選定
// DETECT_BASE_DATA* pNow	今回疵
// DETECT_BASE_DATA* pPre	比較対象疵
//------------------------------------------
bool HtRecv::SelectPriorityDetect(DETECT_BASE_DATA const* pNow, DETECT_BASE_DATA const* pPre)
{
	const int cnCnt = 4;
	bool   bBig[cnCnt] = { false,		false,			true,		false };	// 比較用情報 (Gr[小]、疵種[小]、面積[大]、長手位置[小])
	double dNow[cnCnt] = { (double)pNow->nGYuu, (double)pNow->nTYuu, pNow->aria, pNow->y };
	double dPre[cnCnt] = { (double)pPre->nGYuu,	(double)pPre->nTYuu, pPre->aria, pPre->y };

	return KizuFunction::SelectPriorityDetect(cnCnt, dNow, dPre, bBig);
}
