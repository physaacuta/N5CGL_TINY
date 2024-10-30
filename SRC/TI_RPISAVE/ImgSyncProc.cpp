#include "StdAfx.h"
#include "ImgSyncProc.h"

//前提：フレームNOをそろえれば、絵も揃う。
//＝フレームNOは、カメラ間で基本的に同一。100fもずれたりしない
//同期は飾り。

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

#define LOG2( em, ... ) do { \
	if( mcls_detail ){   mcls_detail->Write(KizuLib::LogFileManager:: em, __VA_ARGS__ ); }\
	if(!mcls_detail || \
		KizuLib::LogFileManager::em == KizuLib::LogFileManager::em_ERR ||\
	    KizuLib::LogFileManager::em == KizuLib::LogFileManager::em_WAR) {\
		if(mcls_pLog){       mcls_pLog->Write(KizuLib::LogFileManager:: em, __VA_ARGS__ ); }\
	}\
}while(0)

#define LOG2BOTH( em, ... ) do { \
	if( mcls_detail){ mcls_detail->Write(KizuLib::LogFileManager:: em, __VA_ARGS__ ); }\
	if(mcls_pLog   ){   mcls_pLog->Write(KizuLib::LogFileManager:: em, __VA_ARGS__ ); }\
}while(0)




//------------------------------------------
// コンストラクタ
// int nIns インスタンスNo
//------------------------------------------
ImgSyncProc::ImgSyncProc(int nIns) :
ThreadManager( GetThreadNameArray("SyncP", nIns) ),
m_nIns(nIns),
m_bIsExec(false),
mcls_detail(NULL)
{
	memset(m_nRecvFrameNo, 0xff, sizeof(m_nRecvFrameNo));
}

//------------------------------------------
// デストラクタ
//------------------------------------------
ImgSyncProc::~ImgSyncProc(void)
{
}


//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int ImgSyncProc::ThreadFirst()
{
	Reset();

	// イベント設定
	HANDLE hArray[] = { mque_pIn->GetEvQue()
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int ImgSyncProc::ThreadLast()
{
	//// 未返却の器があれば返却
	ResetBufFrame();
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void ImgSyncProc::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_ADD_IMG = 0,					// フレーム画像キューイング
			EV_FREE							// 解放シグナル
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_ADD_IMG: 						// フレーム取込
		AddNewItem();
		return;
	
//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG2(em_ERR, "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 処理

//------------------------------------------
// リセット
//------------------------------------------
void ImgSyncProc::Reset()
{
	ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[m_nIns];
	ParamManager::P_PARAM_CAMSET::INS const& prmCI = mcls_pParam->GetParamCamSet().ins[m_nIns];

	//// ポインタを初期化
	ResetBufFrame();

	//// 同期フラグ
	m_nEndFrameNo = 0;
	m_bIsSync = false;

	for (int ii = 0; ii < NUM_CAM_POS; ii++) {
		m_nRecvFrameNo[ii] = -1;
	}



	//有効無効をコピーしておく。
	for (int ii = 0; ii < NUM_CAM_POS; ii++) {
		m_bEnabled[ii] = prmCI.camset[ii].bIsEnable;
	}


	mcls_Time.InitCycle();
}


//------------------------------------------
// 画像取得
//	※ ちょっと特殊。
//		幅方向分のフレーム画像が揃うまで、器を保持する。
//------------------------------------------
void ImgSyncProc::AddNewItem()
{
	//// フレーム画像キューを取得
	HT_RPI_FDATA* p = mque_pIn->GetFromHead();
	if (NULL == p) {							// 通常ありえないはず
		LOG2(em_ERR, "[%s] スレッドループキュー無し!", my_sThreadName);
		return;
	}
	QueryPerformanceCounter((LARGE_INTEGER *)&p->nTimeFrameRecv);

	CString strSetResult;
	for (int ii = 0; ii < NUM_CAM_POS; ii++) {
		strSetResult.AppendFormat("%02d,", (int)m_pBufFrame[ii].size());
	}


	//TODO：判定が片死にしたときに、片側だけフレームがくるので、
	// そのとき、古いバッファを消さないとだめかも。

	// 受信状況ログ
	LOG2(em_MSG, "[%s.c%d] フレーム受信 [fno=%d] (%d:%s) Size=%d Que:%d PcName[%s] Subinf:%d [%p, %p, %p]", my_sThreadName, p->nCamId, p->nFrameNo,
		m_nEndFrameNo + 1, strSetResult, p->base_head.len, mque_pIn->GetCount(), p->base_head.pname, p->base_head.subinf, p, p->base_head.addr[0], p->base_head.addr[1]);


	//------------------------------ >>>>
	// 各種整合性チェック 。これがミソ

	if (!mcls_pStatus->IsRun()) {
		mque_pFree->AddToTail(p);
		return;
	}

	// １カメラでの送信連番チェック
	//　＋ダミーフレーム挿入
	if (!CheckSeqNo(p)) {
		mque_pFree->AddToTail(p);
		return;
	}

	//列同期バッファにためる
	if (!this->CheckAndAdd(p))
	{
		mque_pFree->AddToTail(p);
		return;
	}



	//// 全カメラセットが揃ったかチェック
	if (!CheckCamset()) return;
	// <<<---------------------------------



	//// 処理
	if (m_bIsExec) {
		mcls_Time.CounterCycle();

		Exec();

	}
	else {
		PoolBack_RecvFreeQue(-1);
	}
}
#include <algorithm>
//------------------------------------------
// 全カメラセットで同期完了？
// FRAME_DATA* p フレーム情報
// 戻り値 : true:同期完了
//------------------------------------------
bool ImgSyncProc::CheckAndAdd(HT_RPI_FDATA* p)
{
	const int cam = p->nCamId;		// 0オリジン


	ParamManager::P_MAIN		const& prm = mcls_pParam->GetMain();

	// =========================================================
	// 調査
	if (2 <= prm.nDispPrint) {
		CString sWk;
		KizuBitmapManager	cls;

		BYTE* pImg = (BYTE*)p->base_head.addr[0];
		if (pImg &&
			p->nTimeFrameRecv != INT_MIN		//ダミーフレームは読み込み専用
			) {
			for (int rec = 0; rec < MAX_IMAGE_CNT; rec++) {
				if (0 == p->nImgSize[rec]) continue;

				int nFrom = p->nImgOffset[rec];
				BYTE* pWk = &pImg[nFrom];

				// シーケンスNo
				sWk.Format("%06d", p->nFrameNo);
				cls.DrawText_Gray(prm.nFrameSizeX, prm.nFrameSizeY, pWk, 1000, 200 + 100 * 0, sWk, 100, 255);
			}
		}
	}

	if (!AddToTail(cam, p))
	{
		if (!m_bIsSync) {
			//幅同期バッファがあふれたので、一番古いやつを返却。
			HT_RPI_FDATA* pFirstInBuf = GetFromHead(cam);



			LOG2(em_WAR, "[%s.c%d] 同期前に幅方向同期機構があふれた！ "
				"[fno=%d]. 最初のQ[fno=%d]を返却",
				my_sThreadName, p->nCamId, p->nFrameNo,
				pFirstInBuf->nFrameNo);
				//Getしたので、Addできるはず。
				AddToTail(cam, p);
				//Getしたフレームは返却
				mque_pFree->AddToTail(pFirstInBuf);
		}
		else {
			LOG2(em_ERR, "[%s.c%d] 受信フレーム画像抜け(幅揃え用バッファあふれ）！！ [FNo=%d,%d]", my_sThreadName, cam, PeekFromHead(cam)->nFrameNo, p->nFrameNo);

			int nCount = OutputIncompleteQueue(p);
			if (nCount > 0) {
				LOG(em_WAR), "[%s] 下流待ち待機... %d ms", my_sThreadName, nCount * 300);
				Sleep(nCount * 300);
			}

			/*
					PoolBack_RecvFreeQue(cam);						// 一番古いやつを返却

					// 2番目に古いやつを取り出し。=最古キュー
					m_SetArray[cam].pData  = GetFromHead(cam);
					m_SetArray[cam].bIsSet = true;

					// １つ空きができたので、再登録
					bRet = AddToTail(cam, p);
					if( ! bRet ) {	// ありえない
						LOG2(em_ERR, "[%s] 受信フレーム画像抜け復旧不能！！ [cam=%d : FNo=%d]", my_sThreadName, cam, p->nFrameNo);
					}
			*/
			mcls_pStatus->SetFrameSyncSt(m_nIns, false);

			// 強制再起動
			mcls_pStatus->ResetAp();
			return false;
		}
	}
	return true;
}

//もってるなかで、最小のフレームNoをもつフレームをかえす。
// bNeedAllCamera : 全カメラからフレームがきている前提か？
// pnCamIndex[opt]: 最少フレームNOは、どのカメラインデクス（camIndex)に属すか？
HT_RPI_FDATA *ImgSyncProc::GetMinimumFrame( bool bNeedAllCamera, int *pnCamIndex )
{
	ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[m_nIns];

	HT_RPI_FDATA *pMinData  = NULL;
	int  nMinFrameNo = INT_MAX;
	for( int ii=0; ii<prmI.nCamsetNum; ii++){
		if( !m_bEnabled[ii] ) continue;

		HT_RPI_FDATA*pi = PeekFromHead(ii);
		if( !pi )
		{
			//データがない＝全部のカメラからきてない。
			if( bNeedAllCamera)
				return NULL;

			continue;
		}

		if( nMinFrameNo > (int)pi->nFrameNo) {
			pMinData   =  pi;
			nMinFrameNo= pMinData->nFrameNo;
			if( pnCamIndex )
				*pnCamIndex = ii;
		}
	}
	return pMinData;

}


//------------------------------------------
// 全カメラセットで同期完了？
// FRAME_DATA* p フレーム情報
// 戻り値 : true:同期完了
//------------------------------------------
bool ImgSyncProc::CheckCamset()
{
	ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[m_nIns];


	HT_RPI_FDATA *pMinData = GetMinimumFrame(true, NULL);
	if( !pMinData ){
		return false;
	}
	int  nMinFrameNo = pMinData->nFrameNo;

	//全部のカメラから何かしらフレームが来た！（ずれているかも）
	bool bWidthsyncWithDummy = false;
	char dummy_stat[MAX_CAMSET+1] = "";
	for( int ii=0; ii<prmI.nCamsetNum; ii++){
		
		HT_RPI_FDATA *pi = PeekFromHead(ii);
		//ずれてるし。。
		if( pi && pi->nFrameNo != nMinFrameNo){
			//とりあえず、ダミーフレームを入れて、FrameNoをそろえる。。
			HT_RPI_FDATA *pDummty = CreateDummyFrame(pMinData, pi->nCamId, ii);

			m_pBufFrame[ii].push_front( pDummty );

			bWidthsyncWithDummy  = true;
			dummy_stat[ii] = 'X';		
		}
		else{
			dummy_stat[ii] = 'O';		
		}
	}
	if( bWidthsyncWithDummy ){
		LOG(em_WAR), "[%s] ダミーをつかって幅同期 [FNo=%d:%s]", my_sThreadName, nMinFrameNo, dummy_stat);
	}
	//これで、m_SetArrayはそろった！(disabledをのぞいて）



	//// 同期フレームか？
	
	if( !m_bIsSync )
	{
		//同期は飾り。
		{
			m_bIsSync = true;
			LOG(em_MSG), "[%s] 同期完了 [fno=%d]==================", my_sThreadName, pMinData->nFrameNo);
			mcls_pStatus->SetFrameSyncSt(m_nIns, true);
			return true;
		}
		//同期まちなので、いま作った分も含めて、幅単位でかえす
		{
			PoolBack_RecvFreeQue(-1);
			return false;
		}
	}
	else{
		return true;
	}
}

//		if( ! prmCI.camset[ii].bIsEnable ) continue;	// 無効カメラは、データがありとして処理

// 幅方向バッファがあふれたので、ダミーを使って、たまってる分を結合して下流へ。
int ImgSyncProc::OutputIncompleteQueue(HT_RPI_FDATA* pLast)
{
	ParamManager::P_MAIN		const& prm	= mcls_pParam->GetMain();
	ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[m_nIns];
	
	int ncount = 0;
	while(1)
	{
		int minCamIndex = 0;
		HT_RPI_FDATA *pMinData = GetMinimumFrame(false, &minCamIndex );
	
		//全部吐き終わったってことなので終了
		if( pMinData == NULL ) {
			break;
		}
		
		char dummy_stat[MAX_CAMSET+1] = "";
		
		for(int ii=0; ii<prmI.nCamsetNum; ii++ ) {
			if( !m_bEnabled[ii] ) {
				dummy_stat[ii] = 'D';
				continue;	// 無効カメラは、データがありとして処理
			}
			if( m_pBufFrame[ii].empty()) {
				int mincam = pMinData->nCamId;		// 0オリジン
				
				//// ダミーを先頭に詰める
				HT_RPI_FDATA* pDmy = CreateDummyFrame(pMinData, mincam + (ii - minCamIndex), ii);
				m_pBufFrame[ii].push_front(pDmy);

				dummy_stat[ii] = 'X';		
			}
			else{
				dummy_stat[ii] = 'O';		
			}
		}
		LOG2(em_ERR, "[%s] 幅方向バッファがあふれたので、ダミーを使って下流へ流します。 [fno=%d:%s]", my_sThreadName, pMinData->nFrameNo,dummy_stat);

		//下流へ。
		{
			ncount++;
			Exec();
		}

	}
	return ncount;
}

HT_RPI_FDATA *new_dummy_frame();

HT_RPI_FDATA* ImgSyncProc::CreateDummyFrame( const HT_RPI_FDATA* p, int nCamId, int nCamIndex)
{
	//ダミーフレームが作れないとか、事態を複雑にする一方なので、
	// Frame自体はnewして共有画像にすることで、容易化を図る


	//HT_RPI_FDATA* pDmy = mque_pFree->GetFromHead();				// 空きから取得
	//if( NULL == pDmy ) {
	//	// 器空きなし (回復可能なエラー)
	//	LOG(em_ERR), "[%s] GRAB空きキュー無し[%d回目][cam=%d]!", my_sThreadName, nCamIndex);
	//	return NULL;
	//}


	HT_RPI_FDATA* pDmy = new_dummy_frame();

	//// ダミーフレーム生成
	pDmy->nCamId	= nCamId;//p->nCamId + (nCamIndex - pCamIndex );
	pDmy->nFrameNo	= p->nFrameNo;
	pDmy->nTimeFrameRecv = INT_MIN;		//ダミーマーク。
	memset(pDmy->dEdgePos, 0x00, sizeof(p->dEdgePos));

	//画像系
	pDmy->nWidth = p->nWidth;
	pDmy->nHeight = p->nHeight;
	pDmy->nImgCnt	= p->nImgCnt;
	for(int jj=0; jj<MAX_IMAGE_CNT; jj++) {
		pDmy->nImgSize[jj]	= p->nImgSize[jj];
		pDmy->nImgOffset[jj]= p->nImgOffset[jj];
	}

	return pDmy;
}
//------------------------------------------
// １カメラでの送信連番チェック
// FRAME_DATA* p フレーム情報
// 戻り値 : false 現フレームを破棄。  true 下流へgo
//------------------------------------------
bool ImgSyncProc::CheckSeqNo(HT_RPI_FDATA* p)
{
	ParamManager::P_MAIN		const& prm = mcls_pParam->GetMain();


	int cam = p->nCamId;		// 0オリジン

	// =========================================================
	// 連番チェック

	//WORD a = -1;
	//WORD b = 0;
	//WORD c = 1;
	//long n;


	//n=b- (WORD)(a+1);		// 0 
	//n=c- (WORD)(b+1);

	//n=c-(WORD)(a+1);		// 1コ抜け
	//n=b-(WORD)(c+1);		// 1コテレコ
	//n=a-(WORD)(c+1);		// 2コテレコ	※これがちゃんと取れない

	//最初のフレームは無条件とする
	if (m_nRecvFrameNo[cam] == -1) {
		m_nRecvFrameNo[cam] = p->nFrameNo - 1;
	}

	long nSa = p->nFrameNo - (m_nRecvFrameNo[cam] + 1);		// 差。(1なら1フレーム抜け。 -1なら重複。 -2以降なら戻り) 0なら連番


//LOG(em_WAR), "[%s] iPortシーケンスNo整合性 [cam=%d : INo=%d->%d]", my_sThreadName, cam, nOld, p->nSeqNum[0]);


	if (0 == nSa) {
		m_nRecvFrameNo[cam] = p->nFrameNo;
		return true;		// 連番チェック
	}

	//	//// フレーム抜け実行有無
	//	if( 1 != prm.nExecDummyInsert ) return true;


	//// テレコ発生
	// 差がすごく大きかった場合、テレコ時( 65,535 - 1+1) などのケースとなる
	// どちらにしてもマイナス値(テレコ)時は、テレコフレームは破棄するため大差なし。
	if (3 < abs(nSa)) {
		LOG2(em_ERR, "[%s.c%d] 連番チェック整合性異常(差:%d) [FNo=%d != %d]", my_sThreadName, cam, nSa, p->nFrameNo, m_nRecvFrameNo[cam] + 1);
		return false;
	}


	// =========================================================
	// これ以降は、フレーム抜け発生
	LOG2(em_ERR, "[%s.c%d] 連番No整合性異常(フレーム抜け発生)(差:%d) [FNo:%d != have:%d]", my_sThreadName, cam, nSa, p->nFrameNo, m_nRecvFrameNo[cam] + 1);
//	調査用ログ--->>>
	//syslog(900, "[%s.c%d] 連番No整合性異常(フレーム抜け発生)(差:%d) [FNo:%d != have:%d]", my_sThreadName, cam, nSa, p->nFrameNo, m_nRecvFrameNo[cam] + 1);
//	調査用ログ---<<<
	m_nRecvFrameNo[cam] = p->nFrameNo;

	// 抜け数分 ダミーフレーム挿入
	for (int ii = 0; ii < nSa; ii++) {

		//// ダミーフレーム生成
		HT_RPI_FDATA* pDmy = CreateDummyFrame(p, p->nCamId, ii);
		pDmy->nFrameNo = p->nFrameNo - (nSa - ii);

		LOG(em_WAR), "[%s.c%d]ダミーフレーム挿入 [FNo=%d]", my_sThreadName, cam, pDmy->nFrameNo);

		//// 挿入
		if (!CheckAndAdd(pDmy)) return true;
	}
	return true;
}


//------------------------------------------
// 処理実行
//------------------------------------------
void ImgSyncProc::Exec()
{
	// =========================================================
	//// 次のキュー情報を生成
	FRAME_DATA_ARRAY* pDeli = new FRAME_DATA_ARRAY;
	memset(pDeli, 0x00, sizeof(FRAME_DATA_ARRAY));


	//// データセット
	pDeli->nIns = m_nIns;
	pDeli->nNum = mcls_pParam->GetMain().ins[m_nIns].nCamsetNum;

	QueryPerformanceCounter((LARGE_INTEGER *)&pDeli->nTimePlgSyncEnd);

	pDeli->emImgSaveStat = IMG_SAVE_STAT_SAVE;

	for (int ii = 0; ii < pDeli->nNum; ii++) {
		pDeli->nIsOk[ii] = false;
		pDeli->pData[ii] = GetFromHead(ii);

		if (pDeli->pData[ii]) {
			pDeli->nIsOk[ii] = true;
			pDeli->nFno = pDeli->pData[ii]->nFrameNo;
			pDeli->nIdx = ii;
			//ダミーフレームなので、描画用のフレームNOを-1にする。
			if (pDeli->pData[ii]->nTimeFrameRecv == INT_MIN) {
				pDeli->pData[ii]->nFrameNo = -1;
			}
		}


		/*
		//// BMP
		if(true) {
			if( ! m_SetArray[ii].bIsSet) continue;
			for(int jj=0; jj<m_SetArray[ii].pData->nImgCnt; jj++) {

				int off = m_SetArray[ii].pData->nImgOffset[jj];
				BYTE* p = (BYTE*)m_SetArray[ii].pData->base_head.addr[0];

				CString s;
				s.Format("to_raw_%d_%d_%d.bmp", m_SetArray[ii].pData->nFCNo, ii, jj);
				KizuBitmapManager kb;
				kb.DataToBMPfile_Gray(SIZE_FRAME_IMG_X, SIZE_FRAME_IMG_Y, &p[off], s);
			}
		}
		*/
	}




	m_nEndFrameNo = pDeli->nFno;
	LOG2(em_MSG, "[%s] 下流へ [fno=%d]", my_sThreadName, pDeli->nFno);

	////// 次へ
	if (!mque_pOut->AddToTail(pDeli)) {
		// 確保した器は全部開放
		for (int ii = 0; ii < pDeli->nNum; ii++) {
			mque_pFree->AddToTail(pDeli->pData[ii]);
		}
		delete pDeli;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// バッファリング管理

//------------------------------------------
// バッファリング
//------------------------------------------
bool ImgSyncProc::AddToTail(int cam, HT_RPI_FDATA* p)
{
	//全部いったんバッファする前提なので、わざわざログに出さなくてもいい
	//if(! m_pBufFrame[cam].empty() ){
	//	LOG(em_MSG), "[%s.c%d] バッファリング [FCno=%d] (%d, MAX=%d)", my_sThreadName, cam, p->nFCNo, m_pBufFrame[cam].size()+1, MAX_BUF_FRAME);	// +1は今回タメる分
	//}


	if(MAX_BUF_FRAME <= m_pBufFrame[cam].size() ) return false;
	m_pBufFrame[cam].push_back(p);
	return true;
}

//------------------------------------------
// バッファ取り出し
//------------------------------------------
HT_RPI_FDATA* ImgSyncProc::GetFromHead(int cam)
{
	_ASSERT( cam < NUM_CAM_POS);
	//if(0 >= m_pBufFrame[cam].size() ) return NULL;
	if( m_pBufFrame[cam].empty() ) return NULL;

	HT_RPI_FDATA* p = m_pBufFrame[cam].front();
	m_pBufFrame[cam].pop_front();
	return p;
}	


//------------------------------------------
// バッファみるだけ
//------------------------------------------
HT_RPI_FDATA* ImgSyncProc::PeekFromHead(int cam)
{
	_ASSERT( cam < NUM_CAM_POS);
	//if(0 >= m_pBufFrame[cam].size() ) return NULL;
	if( m_pBufFrame[cam].empty() ) return NULL;

	return m_pBufFrame[cam].front();
}	


//------------------------------------------
// バッファクリア
//------------------------------------------
void ImgSyncProc::ResetBufFrame()
{
	// カメラ単位のバッファリング
	for(int ii=0; ii<NUM_CAM_POS; ii++) {
		ResetBufFrame(ii);
	}
}
void ImgSyncProc::ResetBufFrame(int nIndex)
{
	while(HT_RPI_FDATA* p = GetFromHead(nIndex)) {
		mque_pFree->AddToTail(p);
	}
}

//------------------------------------------
// 受信キュー幅方向バッファを1列分返却。
// int nIndex -1:全部 0～:指定したインデックスのみ
//------------------------------------------
void ImgSyncProc::PoolBack_RecvFreeQue(int nIndex)
{
	int s, e;
	if (-1 == nIndex) { s = 0; e = NUM_CAM_POS - 1; }
	else { s = e = nIndex; }

	LOG2(em_INF, "[%s] バッファクリア(%d～%d)", my_sThreadName, s, e);


	for (int ii = s; ii <= e; ii++) {
		HT_RPI_FDATA* p = GetFromHead(ii);
		if (NULL == p) break;
		mque_pFree->AddToTail(p);
	}

}


//------------------------------------------
// バッファ最大数
//------------------------------------------
int ImgSyncProc::GetMaxBufFrameNum()
{
	int nMax = 0;
	// カメラ単位のバッファリング
	for(int ii=0; ii<NUM_CAM_POS; ii++) {
		if( nMax < (int)m_pBufFrame[ii].size() ) 
			nMax = (int)m_pBufFrame[ii].size();
	}
	return nMax;
}

