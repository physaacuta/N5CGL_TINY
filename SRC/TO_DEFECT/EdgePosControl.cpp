#include "StdAfx.h"
#include "EdgePosControl.h"


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
EdgePosControl::EdgePosControl(void) :
m_bOldMode(false),
m_bFrameSkipAddDB(false),
FuncBase("EdgePosCont")
{
	Init();
}

//------------------------------------------
// デストラクタ
//------------------------------------------
EdgePosControl::~EdgePosControl(void)
{
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 共通情報
//------------------------------------------
// エッジ検出モード 名称
//------------------------------------------
CString EdgePosControl::GetDivEdgeMode(EM_DIV_EDGE_MODE em) 
{
	CString wk;
	if		(DIV_EDGE_MODE_NON == em)			{ wk = "初期状態";}
	else if (DIV_EDGE_MODE_BACK == em)			{ wk = "ﾊﾞｯｸｱｯﾌﾟ";}
	else if (DIV_EDGE_MODE_SKIP == em)			{ wk = "スキップ";}
	else if (DIV_EDGE_MODE_OK == em)			{ wk = "正常";}
	else if (DIV_EDGE_MODE_ONE == em)			{ wk = "片エッジ優先";}
	else if (DIV_EDGE_MODE_CENTER == em)		{ wk = "ﾗｲﾝｾﾝﾀｰ振分";}
	else										{ wk.Format("ステータス異常 <%d>", em);}
	return wk;
}

//------------------------------------------
// エッジ検出位置決定 (スキャン方向はDS→WS)
// int ou 表裏
// float* dEdgePos 幅方向カメラ分のエッジ位置配列
// int& edgeL エッジ配列位置左（画像左側）DS
// int& edgeR エッジ配列位置右（画像右側）WS
//------------------------------------------
void EdgePosControl::SearchEdgeIndex(int ou, float* dEdgePos, int& edgeL, int& edgeR) 
{
	// ・１台に両エッジは未対応
	// ・内側から外側に検索
	int nHalfNum = NUM_CAM_POS / 2;
	//int nMod = NUM_CAM_POS % 2;

	//// エッジ検出 DS側
	edgeL = -1;
	for(int ii=nHalfNum-1; ii>=0; ii--) {
		if( 0 != dEdgePos[ii] ) {
			edgeL = ii; 
			break;
		}
	}
	//// エッジ検出 WS側 = 右側
	edgeR = -1;
	for(int ii=nHalfNum; ii<NUM_CAM_POS; ii++) {
		if( 0 != dEdgePos[ii] ) {
			edgeR = ii; 
			break;
		}
	}
}
void EdgePosControl::SearchEdgeIndex(int ou, int& edgeL, int& edgeR) 
{
	// 直近のデータを使用
	SearchEdgeIndex(ou, m_EdgeInf[ou].dEdgePos, edgeL, edgeR);
}
//------------------------------------------
// エッジ検出位置決定 (スキャン方向はDS→WS)
// int ou 表裏
// float& edgeL エッジ位置左（画像左側）DS
// float& edgeR エッジ位置右（画像右側）WS
//------------------------------------------
void EdgePosControl::SearchEdgePos(int ou, float& edgeL, float& edgeR, DWORD* pFrameNo) 
{
	// 整合性チェック後の 最終エッジ位置を返却
	edgeL = m_EdgeInf[ou].dEdgeAnsPos[0];
	edgeR = m_EdgeInf[ou].dEdgeAnsPos[1];

	if(NULL != pFrameNo) {
		*pFrameNo = m_EdgeInf[ou].nFrameNo;
	}

}

//------------------------------------------
// 生エッジ検出位置決定 (スキャン方向はDS→WS)
// int ou 表裏
// float& edgeL エッジ位置左（画像左側）DS
// float& edgeR エッジ位置右（画像右側）WS
//------------------------------------------
void EdgePosControl::SearchEdgeRawPos(int ou, float& edgeL, float& edgeR, DWORD* pFrameNo) 
{
	// 整合性チェック後の 最終エッジ位置を返却
	edgeL = m_EdgeInf[ou].dEdgeRawPos[0];
	edgeR = m_EdgeInf[ou].dEdgeRawPos[1];

	if(NULL != pFrameNo) {
		*pFrameNo = m_EdgeInf[ou].nFrameNo;
	}

}



//------------------------------------------
// 生エッジ最大検出位置決定 (スキャン方向はDS→WS)
// int ou 表裏
// float& edgeL エッジ位置左（画像左側）DS
// float& edgeR エッジ位置右（画像右側）WS
//------------------------------------------
void EdgePosControl::SearchEdgeRawPosMax(int ou, float& edgeL, float& edgeR, DWORD* pFrameNo) 
{
	// 整合性チェック後の 最終エッジ位置を返却
	edgeL = m_EdgeInf[ou].dEdgeRawPosMax[0];
	edgeR = m_EdgeInf[ou].dEdgeRawPosMax[1];

	if(NULL != pFrameNo) {
		*pFrameNo = m_EdgeInf[ou].nFrameNo;
	}

}

//------------------------------------------
// 生エッジ最小検出位置決定 (スキャン方向はDS→WS)
// int ou 表裏
// float& edgeL エッジ位置左（画像左側）DS
// float& edgeR エッジ位置右（画像右側）WS
//------------------------------------------
void EdgePosControl::SearchEdgeRawPosMin(int ou, float& edgeL, float& edgeR, DWORD* pFrameNo) 
{
	// 整合性チェック後の 最終エッジ位置を返却
	edgeL = m_EdgeInf[ou].dEdgeRawPosMin[0];
	edgeR = m_EdgeInf[ou].dEdgeRawPosMin[1];

	if(NULL != pFrameNo) {
		*pFrameNo = m_EdgeInf[ou].nFrameNo;
	}

}

//------------------------------------------
// サーチ範囲 (検出範囲、リカバリーモード検出範囲で算出可能)
// int nLR 0:左エッジ側 1:右エッジ側
// float dCoilWidth コイル幅
// float dDetectRange 検出範囲 [mm]
// float* dPos サーチ範囲 (サーチ範囲の左方向側,サーチ範囲の右方向側)
//------------------------------------------
void EdgePosControl::GetSearchPos(int nLR, float dCoilWidth, float dDetectRange, float* dPos)
{
	float dRange	= (float)(dDetectRange * 0.5);	// 範囲

	if(COIL_WID_MAX <= dCoilWidth || 0 >= dCoilWidth) {
		dPos[0]	= 0.0;
		dPos[1]	= 0.0;
	} else {
		if(0==nLR) {
			dPos[0]	= (float)(dCoilWidth*0.5*-1)	- dRange;
			dPos[1]	= (float)(dCoilWidth*0.5*-1)	+ dRange;
		} else {
			dPos[0]	= (float)(dCoilWidth*0.5)		- dRange;
			dPos[1]	= (float)(dCoilWidth*0.5)		+ dRange;
		}
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// メイン処理

//------------------------------------------
// 初期化
//------------------------------------------
void EdgePosControl::Init()
{
	// 初期モードや板幅を初期化する
	memset( m_EdgeInf,	0x00, sizeof(m_EdgeInf));
	memset( m_Zi, 0x00, sizeof(m_Zi));
	for(int ii=0; ii<NUM_MEN; ii++) {
		m_EdgeInf[ii].emEdgeMode = DIV_EDGE_MODE_NON;
	}

	//// その他項目
	for(int ii=0; ii<NUM_MEN; ii++) {
		m_nEdgeSearchSkip[ii]		= 0;
		m_nFrameSaveInterval[ii]	= 0;
	}
}

//------------------------------------------
// 物理的なコイル切替 [←MI]
// int ou 表裏
//------------------------------------------
void EdgePosControl::CoilChange(int ou)
{
	//// 不感帯開始設定 (物理的なコイル切替 から、 論理的なコイル切替+溶接後不感帯 まで)
	// エンジン側では、コイル切り替えで、勝手にサーチ範囲がリセットされるため
	m_nEdgeSearchSkip[ou] = -1;			// 論理的なコイル切替で再度ちゃんと設定するため、ここでは、物理コイル切替～論理的なコイル切替の間、スキップできるようにしておく
	LOG(em_MSG), "[%s] <%s> 物理的なコイル切替認識", my_sThreadName, DivNameManager::GetTorB(ou));
}

//------------------------------------------
// 論理的なコイル切り替え＆上位板幅セット [←HtRecv]
// int ou 表裏
// char const* cKizukenNo 管理No
// float coilWid 上位板幅 [mm] 板幅が不明の場合は、最大板幅
// bool bIsInit 初期化時true
//------------------------------------------
void EdgePosControl::CoilStart(int ou, char const* cKizukenNo, float coilWid, bool bIsInit)
{
	//// 初期化
	memset( &m_Zi[ou], 0x00, sizeof(ZI_COIL_RESULT));

	//// データセット
	memcpy( m_Zi[ou].cKizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);
	m_Zi[ou].dCoilWidMin	= COIL_WID_MAX;							// 最小には、とりあえず最大板幅をセットしておく

	// 有効板幅チェック
	if(COIL_WID_MAX <= coilWid || 0 >= coilWid) {
		m_Zi[ou].dCoilWidth = 0.0;
		m_Zi[ou].bCoilWidth = false;
	} else {
		m_Zi[ou].dCoilWidth = coilWid;
		m_Zi[ou].bCoilWidth = true;
	}

//// ▲
//m_Zi[0].dCoilWidth = 1300;
//m_Zi[0].bCoilWidth = true;

	//// コイル切替時のみ
	if( ! bIsInit ) {
		// 不感帯の設定
		m_nEdgeSearchSkip[ou] = mcls_pParam->GetCommonMast().Edge.nFromSkip; 
		if(0 != m_nEdgeSearchSkip[ou]) m_nEdgeSearchSkip[ou] += 1;		// 直ぐに一回引かれるため、一回分加算しておく

		LOG(em_MSG), "[%s] <%s> 論理的なコイル切替認識 [板幅=%dmm] [管理No: %s]", my_sThreadName, DivNameManager::GetTorB(ou), (int)m_Zi[ou].dCoilWidth, cKizukenNo);
	}
}

//------------------------------------------
// コイル実績締め [←HtRecv]
// int ou 表裏
//------------------------------------------
void EdgePosControl::CoilEnd(int ou)
{
	if(0==strlen(m_Zi[ou].cKizukenNo)) return;

#ifdef EDGE_POS_CONTROL_DBSAVE
	//// 事前準備
	DBManager::DeliveryDB* pDeli = new DBManager::DeliveryDB;
	pDeli->kubun = DBManager::em_EDGE_COIL_INF;
	pDeli->data = (BYTE*)new DBDATA_EDGE_COIL_INF;
	DBDATA_EDGE_COIL_INF* pWk = (DBDATA_EDGE_COIL_INF*)pDeli->data;
	memset(pWk, 0x00, sizeof(DBDATA_EDGE_COIL_INF));


	//// データセット
	memcpy( pWk->cKizukenNo, m_Zi[ou].cKizukenNo, SIZE_KIZUKEN_NO);
	pWk->nMen				= ou;										// 表裏区分 (0:表 1:裏)
	pWk->dCoilWidMin		= m_Zi[ou].dCoilWidMin;						// 板幅最小
	pWk->dCoilWidMax		= m_Zi[ou].dCoilWidMax;						// 板幅最大

	pWk->nLimitEdgeErrNum	= m_Zi[ou].nLimitEdgeErrNum;				// 板幅許容誤差範囲外数
	pWk->nLostEdgeNum[0]	= m_Zi[ou].nLostEdgeNum[0];					// エッジ未検出数 (左エッジ/右エッジ)
	pWk->nLostEdgeNum[1]	= m_Zi[ou].nLostEdgeNum[1];				


	// フレームスキップ個数
	if(m_bFrameSkipAddDB) {
		pWk->nFrameNumCoil		= m_Zi[ou].nFrameNumCoil;
		for(int ii=0; ii<NUM_CAM_POS; ii++)	{
			for(int jj=0; jj<5; jj++) {
				pWk->nFrameNumSkip[ii][jj] = m_Zi[ou].nFrameNumSkip[ii][jj]; 
			}
		}
	}

	//// 登録依頼
	if( ! mque_pAddSQLData->AddToTailFreeCheck(pDeli, 3) ) {
		// キューフル
		delete pDeli->data;
		delete pDeli;

		// DBが異常 (通常ありえない)
		LOG(em_ERR), "[%s] DB登録依頼キューフル [DB_EDGE_COIL_INF]", my_sThreadName);
		syslog(SYSNO_QUEFULL_WAR, "[DB_EDGE_COIL_INF]");
	}
#endif

	//// 多重登録防止処置(念のため)
	memset(&m_Zi[ou], 0x00, sizeof(ZI_COIL_RESULT));
}

//------------------------------------------
// 検査状況セット
// int ou 表裏
// int* nFrameSkip  片面カメラセット分の処理スキップ
// int* nEdgeNg		片面カメラセット分のエッジ検出失敗
// int* nFrameSts	片面カメラセット分の探傷状態定義
//------------------------------------------
void EdgePosControl::SetFrameSkip(int ou, int* nFrameSkip, int* nEdgeNg, int* nFrameSt)
{
	m_Zi[ou].nFrameNumCoil ++;
	for(int ii=0; ii<NUM_CAM_POS; ii++) {
		if(0 != nEdgeNg[ii])						m_Zi[ou].nFrameNumSkip[ii][0] ++;	// エッジだけは、画像処理でNGが別系統 
		if(STATUS_PROCSKIP == nFrameSt[ii])			m_Zi[ou].nFrameNumSkip[ii][1] ++; 
		if(NCL_SMEMPIPE_SMEMFULL == nFrameSkip[ii]) m_Zi[ou].nFrameNumSkip[ii][2] ++; 
		if(STATUS_TOOMANY == nFrameSt[ii])			m_Zi[ou].nFrameNumSkip[ii][3] ++; 
		if(STATUS_QFULL == nFrameSt[ii])			m_Zi[ou].nFrameNumSkip[ii][4] ++; 
	}
}

//------------------------------------------
// エッジ検出位置セット
// int ou 表裏
// long nFrameNo フレームNo
// long nPos 長手位置 [mm]
// double dResY 縦分解能
// float* edge 表裏毎のエッジ位置配列ポインタ (左、右の順番に格納)
// float* edgemax 表裏毎の最大エッジ位置配列ポインタ (左、右の順番に格納)
// float* edgemin 表裏毎の最小エッジ位置配列ポインタ (左、右の順番に格納)
// double dSpmNobiHosei SPM伸び補正率
// 戻り値： true:エッジ正常   false:エッジ未検出、片エッジ、センター振分 ※ 不感帯を含む
//------------------------------------------
bool EdgePosControl::SetEdgePos(int ou, long nFrameNo, long nPos, double dResY, float* edge, float* edgemax, float* edgemin, double dSpmNobiHosei)
{
	bool		bAns = true;				// 戻り値
	EDGE_INF	NewEdge;					// 今回の新しい情報
	float		dNewSearchPos[2][2];		// 片エッジ優先時のサーチ範囲 (左/右)(左側ｻｰﾁ範囲/右側ｻｰﾁ範囲) (0で制限無し)
	float		dCoilWid[2];				// 上位板幅 [mm] (左, 右)
	float		dWk[2];
	bool		bWk;
	ParamManager::P_COMMON_MAST const& prm = mcls_pParam->GetCommonMast(); 
	int			nEdgeErrReason = EDGE_NORMAL;	// エッジ不正理由の論理和


	//================================================
	// 前準備
	// 前回値を初期値としてセット
	memcpy(&NewEdge, &m_EdgeInf[ou], sizeof(NewEdge));
	
	// 今回のエッジ位置をセット
	NewEdge.nFrameNo = nFrameNo;
	//memcpy(NewEdge.dEdgePos, edge, sizeof(NewEdge.dEdgePos));
	memcpy(NewEdge.dEdgeRawPos, edge, sizeof(NewEdge.dEdgeRawPos));
	memcpy(NewEdge.dEdgeRawPosMax, edgemax, sizeof(NewEdge.dEdgeRawPosMax));
	memcpy(NewEdge.dEdgeRawPosMin, edgemin, sizeof(NewEdge.dEdgeRawPosMin));
	for(int ii=0; ii<2; ii++) NewEdge.bEdgeEnable[ii] = false;
	if(0.0 > NewEdge.dEdgeRawPos[0]) NewEdge.bEdgeEnable[0] = true;
	if(0.0 < NewEdge.dEdgeRawPos[1]) NewEdge.bEdgeEnable[1] = true;

	// 生のエッジ位置を算出
	for(int ii=0; ii<2; ii++) {
		if(!NewEdge.bEdgeEnable[ii]) {
			NewEdge.dEdgeRawPos[ii] = 0;
			bAns = false;
		}
		NewEdge.dEdgeAnsPos[ii]	= NewEdge.dEdgeRawPos[ii];	
	}

	//SearchEdgeIndex(ou, NewEdge.dEdgePos, nWk[0], nWk[1]);
	//for(int ii=0; ii<2; ii++) {
	//	if(-1 == nWk[ii]) {
	//		NewEdge.bEdgeEnable[ii] = false;
	//		NewEdge.dEdgeRawPos[ii] = 0;
	//		bAns = false;

	//	} else {
	//		NewEdge.bEdgeEnable[ii] = true;
	//		NewEdge.dEdgeRawPos[ii] = NewEdge.dEdgePos[ nWk[ii] ];
	//		m_nEdgeDetectCamSet[ou][ii]	= ou*NUM_CAM_POS + nWk[ii] + 1;	// 検出カメラセット
	//	}
	//	NewEdge.dEdgeAnsPos[ii]	= NewEdge.dEdgeRawPos[ii];	
	//}

	//// いろいろ減算
	if( 0 < m_nEdgeSearchSkip[ou] )		m_nEdgeSearchSkip[ou]	 -= 1;	// スキップ中
	if( 0 < m_nFrameSaveInterval[ou] )	m_nFrameSaveInterval[ou] -= 1;	// 画像要求中


	//// ここまでが、以前のまま
	if(0 == prm.Edge.nType) {
		NewEdge.bEdgeAns = false;		// 最終エッジ位置 使用不可
		// 未加工で、判定エッジをそのまま最終エッジとして使用
		memcpy(&m_EdgeInf[ou], &NewEdge, sizeof(EDGE_INF));
		
		bAns = true;		// 整合性判定なしのため、正常
		return bAns;
	}



	// ▲
	//NewEdge.dEdgeRawPos[0] = -900;
	//NewEdge.dEdgeRawPos[1] = 900;

	//================================================
	// エッジ位置整合性判定
	//   ・エッジ検出モードの決定
	//	 ・最終エッジ位置の決定

	// よく使う情報を準備
	if( m_Zi[ou].bCoilWidth ) {
		dCoilWid[0] = (float)(m_Zi[ou].dCoilWidth*0.5) * -1;
		dCoilWid[1] = (float)(m_Zi[ou].dCoilWidth*0.5);
	}

	for(int ww=0; ww<1; ww++) {	
		//--------------------------
		// 0 カメラ調整時は、無視
		if(m_bOldMode) {
			NewEdge.emEdgeMode				= DIV_EDGE_MODE_NON;		// 無条件に何もしない
			for(int ii=0; ii<2; ii++) {
				if( NewEdge.bEdgeEnable[ii] ) {
					NewEdge.dEdgeAnsPos[ii]	= NewEdge.dEdgeRawPos[ii];			// 今回値を使用
				} else {
					NewEdge.dEdgeAnsPos[ii]	= m_EdgeInf[ou].dEdgeAnsPos[ii];	// 前回値を使用
				}
			}
			break;
		}


		//--------------------------
		// ① 初回？ または、上位コイル情報の板幅無し
		//		・最初からコイル情報があるラインでも対応可能な様にしておく
		if( ! m_Zi[ou].bCoilWidth ) {

			//// 最初
			if( DIV_EDGE_MODE_NON == m_EdgeInf[ou].emEdgeMode ) {
				//// 初回で エッジ検出が成功
				if(NewEdge.bEdgeEnable[0] && NewEdge.bEdgeEnable[1]) {
					NewEdge.emEdgeMode			= DIV_EDGE_MODE_BACK;		// バックアップに遷移	
					for(int ii=0; ii<2; ii++) {
						NewEdge.dEdgeAnsPos[ii]	= NewEdge.dEdgeRawPos[ii];
					}

				//// 初回で、エッジも未検出。 何もしない。
				} else {
					NewEdge.emEdgeMode			= DIV_EDGE_MODE_NON;
					for(int ii=0; ii<2; ii++) {
						if(NewEdge.bEdgeEnable[ii])	NewEdge.dEdgeAnsPos[ii]	= NewEdge.dEdgeRawPos[ii];
						else						NewEdge.dEdgeAnsPos[ii]	= 0.0;
					}
				}

			//// 一回でもエッジが見つかった後
			} else {
				NewEdge.emEdgeMode				= DIV_EDGE_MODE_BACK;		// 無条件にバックアップに遷移
				for(int ii=0; ii<2; ii++) {
					if( NewEdge.bEdgeEnable[ii] ) {
						NewEdge.dEdgeAnsPos[ii]	= NewEdge.dEdgeRawPos[ii];			// 今回値を使用
					} else {
						NewEdge.dEdgeAnsPos[ii]	= m_EdgeInf[ou].dEdgeAnsPos[ii];	// 前回値を使用
					}
				}
			}
			break;
		}
		
		//--------------------------
		// ② スキップ中？
		if( 0 != m_nEdgeSearchSkip[ou] ) {
			NewEdge.emEdgeMode					= DIV_EDGE_MODE_SKIP;

			for(int ii=0; ii<2; ii++) {
				if( NewEdge.bEdgeEnable[ii] ) {
					NewEdge.dEdgeAnsPos[ii]		= NewEdge.dEdgeRawPos[ii];			// 今回値を使用
				} else {
					NewEdge.dEdgeAnsPos[ii]		= m_EdgeInf[ou].dEdgeAnsPos[ii];	// 前回値を使用
				}
			}

#ifdef DEBUG_LOG_EDGE
			LOG(em_WAR), "[%s] <%s> %d エッジ検出スキップ 管理No:%s 発生位置:%d (%.3f/%.3f) OldEdgeMode:%d", my_sThreadName, DivNameManager::GetTorB(ou), nFrameNo,
				m_Zi[ou].cKizukenNo, nPos, NewEdge.dEdgeRawPos[0], NewEdge.dEdgeRawPos[1], m_EdgeInf[ou].emEdgeMode);
#endif
			break;
		}

		//--------------------------
		// ③ 検出範囲内にエッジ位置がちゃんとある？
		bWk = false;
		for(int ii=0; ii<2; ii++) {
			dWk[0] = abs( dCoilWid[ii] - NewEdge.dEdgeRawPos[ii] );
			if( ! NewEdge.bEdgeEnable[ii] ||
				dWk[0] > prm.Edge.dDetectRange*0.5 ) {
				
				NewEdge.emEdgeMode				= DIV_EDGE_MODE_ONE;		// 少なくとも片エッジ優先
				bWk = true;
				nEdgeErrReason |= ii == 0 ? EDGE_ERR_LEFT : EDGE_ERR_RIGHT;	// エッジ不正理由
			}
		}
		if(bWk) {
			m_Zi[ou].nLimitEdgeErrNum += 1;
			if(DIV_EDGE_MODE_OK == m_EdgeInf[ou].emEdgeMode) {
#ifdef DEBUG_LOG_EDGE
				//▲
				LOG(em_WAR), "[%s] <%s> %d エッジ検出範囲外 (%.3f/%.3f)", my_sThreadName, DivNameManager::GetTorB(ou), nFrameNo, NewEdge.dEdgeRawPos[0], NewEdge.dEdgeRawPos[1]);
#endif
			}
			break;
		}

		//--------------------------
		// ④ 板幅許容誤差範囲内？
		dWk[0] = (abs(NewEdge.dEdgeRawPos[0])+abs(NewEdge.dEdgeRawPos[1])) ;	// 板幅
		if( dWk[0] < (m_Zi[ou].dCoilWidth-prm.Edge.dWidthDiff) || 
			dWk[0] > (m_Zi[ou].dCoilWidth+prm.Edge.dWidthDiff) ) {
			NewEdge.emEdgeMode					= DIV_EDGE_MODE_ONE;		// 少なくとも片エッジ優先
			m_Zi[ou].nLimitEdgeErrNum += 1;
			nEdgeErrReason |= EDGE_ERR_WIDTH;								// エッジ不正理由

			if(DIV_EDGE_MODE_OK == m_EdgeInf[ou].emEdgeMode) {
#ifdef DEBUG_LOG_EDGE
				//▲
				LOG(em_WAR), "[%s] <%s> %d 板幅許容範囲外=%.3fmm (上位板幅=%.3f)", my_sThreadName, DivNameManager::GetTorB(ou), nFrameNo, dWk[0], m_Zi[ou].dCoilWidth);
#endif
			}
			break;
		}

		//--------------------------
		// ⑤ ここまで来たら、正常
		NewEdge.emEdgeMode						= DIV_EDGE_MODE_OK;
		for(int ii=0; ii<2; ii++) {
			NewEdge.dEdgeAnsPos[ii]				= NewEdge.dEdgeRawPos[ii];		// 今回検出のエッジ位置をそのまま使用
			
			dWk[0] = (abs(NewEdge.dEdgeRawPos[0])+abs(NewEdge.dEdgeRawPos[1]));		// 板幅
			if(m_Zi[ou].dCoilWidMin > dWk[0]) m_Zi[ou].dCoilWidMin = dWk[0];
			if(m_Zi[ou].dCoilWidMax < dWk[0]) m_Zi[ou].dCoilWidMax = dWk[0];
		}
		break;
	}


	//================================================
	// バックアップモードの決定
	if(DIV_EDGE_MODE_ONE == NewEdge.emEdgeMode) {
		// この時点で、エッジ不整合 確定
		bAns = false;

		// 前フレームとの誤差
		dWk[0] = abs( m_EdgeInf[ou].dEdgeAnsPos[0] - NewEdge.dEdgeRawPos[0] );		// 左
		dWk[1] = abs( m_EdgeInf[ou].dEdgeAnsPos[1] - NewEdge.dEdgeRawPos[1] );		// 右

#ifdef DEBUG_LOG_EDGE
		LOG(em_WAR), "[%s] <%s> %d, 前ﾌﾚｰﾑとの誤差 左(%.3f<%.3f) 右(%.3f<%.3f)", my_sThreadName, DivNameManager::GetTorB(ou), nFrameNo, 
			dWk[0], prm.Edge.dPerEdgeDiff*0.5, dWk[1], prm.Edge.dPerEdgeDiff*0.5 ); // ▲
#endif


		//--------------------------
		// ・片エッジ優先モード	
		if( DIV_EDGE_MODE_SKIP != m_EdgeInf[ou].emEdgeMode &&
			( ( NewEdge.bEdgeEnable[0] && dWk[0] <= prm.Edge.dPerEdgeDiff*0.5 ) || 
			  ( NewEdge.bEdgeEnable[1] && dWk[1] <= prm.Edge.dPerEdgeDiff*0.5 ) ) ) {

			//// 基準エッジ側を決定
			int nBase;						// 基準側のエッジIndex
			int	nOther;						// 他方側のエッジIndex
			if( dWk[0] <= dWk[1] )	{ nBase = 0; nOther = 1; } 
			else					{ nBase	= 1; nOther	= 0; }
			

			//// サーチ範囲
			// 基準側
			dNewSearchPos[nBase][0]		= NewEdge.dEdgeRawPos[nBase] - (float)(prm.Edge.dDetectRange*0.5);	// 左方向のサーチ範囲
			dNewSearchPos[nBase][1]		= NewEdge.dEdgeRawPos[nBase] + (float)(prm.Edge.dDetectRange*0.5);	// 右方向のサーチ範囲
			// 他方側
			float	dOtherEdgePos		= NewEdge.dEdgeRawPos[nBase] + (0==nBase ? m_Zi[ou].dCoilWidth : -m_Zi[ou].dCoilWidth);		// 他方のエッジ位置
			dNewSearchPos[nOther][0]	= dOtherEdgePos - (float)(prm.Edge.dDetectRangeR*0.5);				// 左方向のサーチ範囲
			dNewSearchPos[nOther][1]	= dOtherEdgePos + (float)(prm.Edge.dDetectRangeR*0.5);				// 右方向のサーチ範囲


			//// 今回のエッジ位置を決定
			NewEdge.emEdgeMode				= DIV_EDGE_MODE_ONE;
			NewEdge.dEdgeAnsPos[nBase]		= NewEdge.dEdgeRawPos[nBase];
			if( 0 == nOther	)	NewEdge.dEdgeAnsPos[nOther]	= dOtherEdgePos + (float)(prm.Edge.dDetectRange*0.2);
			else 				NewEdge.dEdgeAnsPos[nOther]	= dOtherEdgePos - (float)(prm.Edge.dDetectRange*0.2);

		//--------------------------
		// ・ラインセンター振り分けモード
		} else {
			//// 今回のエッジ位置を決定
			NewEdge.emEdgeMode				= DIV_EDGE_MODE_CENTER;
			GetSearchPos(0, m_Zi[ou].dCoilWidth, (float)(prm.Edge.dDetectRangeR*0.8), dWk);
			NewEdge.dEdgeAnsPos[0]			= dWk[1];		// 板側は、左
			GetSearchPos(1, m_Zi[ou].dCoilWidth, (float)(prm.Edge.dDetectRangeR*0.8), dWk);
			NewEdge.dEdgeAnsPos[1]			= dWk[0];		// 板側は、右
			nEdgeErrReason |= EDGE_ERR_PREFRAME;			// エッジ不正理由
		}

	}

#ifdef DEBUG_LOG_EDGE
//	
	LOG(em_MSG), "[%s] <%s> %d, 生エッジ(%d/%d)", my_sThreadName, DivNameManager::GetTorB(ou), nFrameNo, (int)NewEdge.dEdgeRawPos[0], (int)NewEdge.dEdgeRawPos[1] ); // ▲
#endif

	//================================================
	// モード別処理
	bool bDbAdd = false;							// 今回の状態をDBに登録
	//////bool bSearchChange = false;						// サーチ範囲変更(内部が変わるじゃなくて、判定に変更通知を送るフラグ)


	//// モード変化による処理判定？
	if( m_EdgeInf[ou].emEdgeMode != NewEdge.emEdgeMode ) {
		if( DIV_EDGE_MODE_NON != NewEdge.emEdgeMode ) bDbAdd = true;
	}

	// 最終エッジ位置 使用可否
	if( DIV_EDGE_MODE_OK == NewEdge.emEdgeMode ||
	    DIV_EDGE_MODE_ONE == NewEdge.emEdgeMode ||
		DIV_EDGE_MODE_CENTER == NewEdge.emEdgeMode) {
		NewEdge.bEdgeAns = true;		// 最終エッジ位置 使用可
	} else {
		NewEdge.bEdgeAns = false;		// 最終エッジ位置 使用不可
	}

	//// エッジ未検出個数セット
	if( DIV_EDGE_MODE_SKIP != NewEdge.emEdgeMode ) {
		for(int ii=0; ii<2; ii++) {
			if( ! NewEdge.bEdgeEnable[ii])	m_Zi[ou].nLostEdgeNum[ii] += 1;					// エッジ未検出数		
		}
	}

	//// 今回の結果を反映
	memcpy(&m_EdgeInf[ou], &NewEdge, sizeof(EDGE_INF));



	//================================================
	// 外部、他クラス やり取り
#ifdef EDGE_POS_CONTROL_DBSAVE
	//------------------------
	// DBに エッジ検出モード変化管理テーブル を登録
	if(bDbAdd) {
#ifdef DEBUG_LOG_EDGE
			LOG(em_MSG), "[%s] <%s> %d, エッジ検出モード変化登録 管理No:%s 発生位置:%d 検出区分:%d", my_sThreadName, DivNameManager::GetTorB(ou),
				nFrameNo, m_Zi[ou].cKizukenNo, nPos, NewEdge.emEdgeMode);
#endif
			//// 事前準備
			DBManager::DeliveryDB* pDeli = new DBManager::DeliveryDB;
			pDeli->kubun = DBManager::em_EDGE_CHANGE_INF;
			pDeli->data = (BYTE*)new DBDATA_EDGE_CHANGE_INF;
			DBDATA_EDGE_CHANGE_INF* pWk = (DBDATA_EDGE_CHANGE_INF*)pDeli->data;
			memset(pWk, 0x00, sizeof(DBDATA_EDGE_CHANGE_INF));


			//// データセット
			memcpy( pWk->cKizukenNo, m_Zi[ou].cKizukenNo, SIZE_KIZUKEN_NO);
			pWk->nPos = nPos;								// 発生位置 [mm]
			//GetSpmLen(KBN_KEN, DIV_SPM_TRUE, nPos, dSpmNobiHosei, pWk->nPos);// 発生位置 [mm]
			pWk->nMen			= ou;						// 表裏区分 (0:表 1:裏)
			pWk->nFrameNo		= nFrameNo;					// フレームNo
			pWk->nEdgeMode		= NewEdge.emEdgeMode;		// 検出区分
			pWk->dEdgeRawPos[0]	= NewEdge.dEdgeRawPos[0];	// 生エッジ位置 [mm]
			pWk->dEdgeRawPos[1]	= NewEdge.dEdgeRawPos[1]; 		
			pWk->dEdgeAnsPos[0]	= NewEdge.dEdgeAnsPos[0];	// 最終エッジ位置 [mm]
			pWk->dEdgeAnsPos[1]	= NewEdge.dEdgeAnsPos[1]; 
			pWk->nEdgeErrReason = nEdgeErrReason;			// エッジ不正理由の論理和


			//// 登録依頼
			if( ! mque_pAddSQLData->AddToTailFreeCheck(pDeli, 10) ) {
				// キューフル
				delete pDeli->data;
				delete pDeli;

				// DBが異常 (通常ありえない)
				LOG(em_ERR), "[%s] DB登録依頼キューフル [DBDATA_DEFECT_INF]", my_sThreadName);
				syslog(SYSNO_QUEFULL_WAR, "[DBDATA_DEFECT_INF]");
				// ここでのキューフルでは、停止させない。
		}
	}

#endif

	return bAns;
}