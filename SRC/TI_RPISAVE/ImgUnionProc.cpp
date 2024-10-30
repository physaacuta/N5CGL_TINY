#include "StdAfx.h"
#include "ImgUnionProc.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

#define LOG2( em, ... ) do { \
	if(mcls_detail){   mcls_detail->Write(KizuLib::LogFileManager:: em, __VA_ARGS__ ); }\
	if(!mcls_detail || \
		KizuLib::LogFileManager::em == KizuLib::LogFileManager::em_ERR ||\
	    KizuLib::LogFileManager::em == KizuLib::LogFileManager::em_WAR) {\
		if(mcls_pLog){       mcls_pLog->Write(KizuLib::LogFileManager:: em, __VA_ARGS__ ); }\
	}\
}while(0)

//------------------------------------------
// コンストラクタ
// int nId インスタンスNo
//------------------------------------------
ImgUnionProc::ImgUnionProc(int nId) :
ThreadManager( GetThreadNameArray("UnionP", nId) ),
m_bIsExec(false),
mque_pComp(NULL),
mque_pFrame(NULL),
mcls_detail(NULL)
{
	if( InterlockedCompareExchangePointer((PVOID*)&m_BlankData,this, 0) == 0 ){
		m_BlankData = (const BYTE*)VirtualAlloc( 0, SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y*MAX_COLOR_NUM, MEM_COMMIT, PAGE_READONLY);	// ブランクデータ
	}
	Reset();
}

//------------------------------------------
// デストラクタ
//------------------------------------------
ImgUnionProc::~ImgUnionProc(void)
{
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int ImgUnionProc::ThreadFirst()
{
	//// イベント設定
	HANDLE hArray[] = { this->mque_pIn->GetEvQue() 
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int ImgUnionProc::ThreadLast()
{
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void ImgUnionProc::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_ADD_IMG = 0,					// フレーム画像キューイング
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_ADD_IMG: 						// フレーム取込
		this->ExecQue();
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
void ImgUnionProc::Reset()
{
}
//------------------------------------------
// 実処理
// Tin* pIn		入力Queデータ
// 戻り値		NULL:返却しない 出力Queデータ:次のスレッドへ返却する場合  
//------------------------------------------
FRAME_DATA_ARRAY* ImgUnionProc::OnAddNewItem( FRAME_DATA_ARRAY* p )
{
	if( NULL == p ) {	
		LOG2(em_ERR, "[%s] スレッドループキュー無し!", my_sThreadName);
		return p;
	}

	
//LOG(em_WAR), "[%s]<%d> %d Fno=%d, CNo=%d", my_sThreadName, p->nIns, p->emImgSaveStat, p->Udp.udp.framenum, p->nCFno);

	//// 処理
	if(m_bIsExec) {
		Exec(p);
	} else {
		LOG2(em_INF, "[%s]<%d> キャンセル", my_sThreadName, p->nIns);
	}

	return p;
}


//------------------------------------------
// 処理実行
// FRAME_DATA_ARRAY* p 幅方向分のフレーム情報
//------------------------------------------
void ImgUnionProc::Exec(FRAME_DATA_ARRAY* p)
{
	int ii, jj;
	int nIns = p->nIns;
	
	__int64		nTimeStart;					// 処理開始時刻
	QueryPerformanceCounter((LARGE_INTEGER *)&nTimeStart);


	// =========================================================
	// よく使うパラメータをエイリアス
	ParamManager::P_MAIN		const& prm	= mcls_pParam->GetMain();
	ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[nIns];
	int nMen = prmI.nMen;


	// =========================================================
	// 幅方向連結後のフレーム領域を 空き器を取得
	bool bGetQue = true;						// キューを取得
	HT_RPI_FDATA* hrf = p->pData[ p->nIdx ];	// よく使う情報

	//---------------------->>> ロック
	mque_pFrame->EnterCS();
	if( hrf->nImgCnt <= mque_pFrame->GetCountFree() ) {				// ここで空きがなければ、ホントにNG
		for(int ii=0; ii<MAX_COLOR_NUM; ii++) {
			if( 0 == hrf->nImgSize[ii] ) continue;

			p->Fdc.pFrameDataMgr[ii] = mque_pFrame->GetFree(100);			// セマフォー待ちバージョン(すぐに結果を返したい)
			if( NULL == p->Fdc.pFrameDataMgr[ii] ) break;
			((FrameDataManager*)p->Fdc.pFrameDataMgr[ii])->Clear();			// 初期化
		
//LOG2(em_ERR, "[%s] 連結キュー 残り=%d", my_sThreadName, mque_pFrame->GetCountFree());
		}
		p->Fdc.bOk = true;
	} else {
		bGetQue = false;		// 既に空き無し
	}
	mque_pFrame->LeaveCS();
	//<<<---------------------- アンロック

	if( ! bGetQue ) {
		// 器空きなし (回復可能なエラー)
		LOG2(em_ERR, "[%s] 連結キュー 器無し!(これが発生すること自体、かなり異常)", my_sThreadName);
		mcls_pStatus->SetFrameSyncSt( nIns, false );


		// キューを中途半端に取得していたらいけないので 返却
		for(int ii=0; ii<MAX_COLOR_NUM; ii++) {
			if( NULL == p->Fdc.pFrameDataMgr[ii] ) continue;
			mque_pFrame->AddFree( (FrameDataManager*)p->Fdc.pFrameDataMgr[ii] ); 
			p->Fdc.pFrameDataMgr[ii] = NULL;
			p->Fdc.bOk = false;
		}
		return;
	}

/*
	CString sWk;
	sWk.Format("c:\\testt1_%d.bmp", m_nIns); 
	KizuBitmapManager cls;
	cls.DataToBMPfile_Gray(2048, 2048, p->pData[0]->pImgs, sWk);
//*/

	// =========================================================
	// フロー制御
	//		仮決め
	bool bSkip		 = false;									// 画像連結スキップ有無
	EM_COMP_TYPE emCompType = p->emCompType;					// 圧縮タイプ
	int nCompQuality = p->nCompQuality;							// 圧縮率
	int nExecQueCnt  = mque_pComp->GetSyncQueCount();
	

	if (COMP_TYPE_LAC == emCompType) {
		// そろそろ間に合わない可能性があるので、圧縮率を高めて処理時間を早くする
		if(nExecQueCnt > (prm.nLoopQueFrameSize * 0.3) ) {
			nCompQuality = p->nCompQuality*2;
			if(nCompQuality >= 80 ) nCompQuality = 80;
		}
		if(nExecQueCnt > (prm.nLoopQueFrameSize * 0.5) ) {
			if(nCompQuality < 80 ) nCompQuality = 80;
		}
		if(nExecQueCnt > (prm.nLoopQueFrameSize * 0.6) ) {
			if(nCompQuality < 100 ) nCompQuality = 100;
		}
	} else if (COMP_TYPE_NUC == emCompType) {
		// そろそろ間に合わない可能性があるので、圧縮率を高めて処理時間を早くする
		if(nExecQueCnt > (prm.nLoopQueFrameSize * 0.3) ) {
			nCompQuality = p->nCompQuality*2;
			if(nCompQuality <= -80 ) nCompQuality = -80;
		}
		if(nExecQueCnt > (prm.nLoopQueFrameSize * 0.35) ) {
			if(nCompQuality > -80 ) nCompQuality = -80;
		}
		if(nExecQueCnt > (prm.nLoopQueFrameSize * 0.4) ) {
			if(nCompQuality > -100 ) nCompQuality = -100;
		}


	} else if (COMP_TYPE_JPEG == emCompType) {
		// そろそろ間に合わない可能性があるので、圧縮率を高めて処理時間を早くする
		if(nExecQueCnt > (prm.nLoopQueFrameSize * 0.3) ) {
			nCompQuality = p->nCompQuality/2;
			if(nCompQuality <= 30 ) nCompQuality = 30;
		}
		if(nExecQueCnt > (prm.nLoopQueFrameSize * 0.5) ) {
			if(nCompQuality > 30 ) nCompQuality = 30;
		}
		if(nExecQueCnt > (prm.nLoopQueFrameSize * 0.6) ) {
			if(nCompQuality > 20 ) nCompQuality = 20;
		}
	}


	// ----------------
	// もうだめ
	// バッチラインは、尾端検出で一気に流れてくるため、フロー制御をちょっと甘くする
	if (COMP_TYPE_LAC == emCompType) {

		// 間に合わないので黒埋めして処理を軽くして長さだけはあわせる
		if( nExecQueCnt > (prm.nLoopQueFrameSize * 0.5) ) { 
			LOG2(em_ERR, "[%s]<%d> 圧縮スキップ", my_sThreadName, nIns);
			nCompQuality = 200;
			bSkip		 = true;
		}

	} else if (COMP_TYPE_NUC == emCompType) {
		// 間に合わないので黒埋めして処理を軽くして長さだけはあわせる
		if( nExecQueCnt > (prm.nLoopQueFrameSize * 0.7) ) { 
			LOG2(em_ERR, "[%s]<%d> 圧縮スキップ", my_sThreadName, nIns);
			nCompQuality = -100;
			bSkip		 = true;
		}

	} else if (COMP_TYPE_JPEG == emCompType) {
		// 間に合わないので黒埋めして処理を軽くして長さだけはあわせる
		if( nExecQueCnt > (prm.nLoopQueFrameSize * 0.7) ) { 
			LOG2(em_ERR, "[%s]<%d> 圧縮スキップ", my_sThreadName, nIns);
			nCompQuality = 1;
			bSkip		 = true;
		}
	}


	// =========================================================
	// 画像処理

	// 輝度補正
	Exec_Kido(p);

	// 画像連結
	Exec_Union(p, bSkip);



	// =========================================================
	// その他の受け渡しデータセット
	for(int rec=0; rec<MAX_COLOR_NUM; rec++) {
		if( NULL == p->Fdc.pFrameDataMgr[rec] ) continue;	// 保存色そもそもこない対応
		FrameDataManager* pDeli = (FrameDataManager*)p->Fdc.pFrameDataMgr[rec];

		// 元
		pDeli->nLen			= prmI.nUionHeight * prmI.nUionWidth + prm.nBmpHeadSize; //nTo[rec];
		pDeli->nNo			= hrf->nFrameNo;			// カメラリンクスイッチなのでiPortNoがない。フレームNoをセットしておく
		for(jj=0; jj<prmI.nCamsetNum; jj++ ) {
			if( ! p->nIsOk[jj] ) continue;
			pDeli->nSeqNum[jj] = p->pData[jj]->nFrameNo;
		}
		//// フレーム基本情報
		pDeli->nIns			= nIns;
		pDeli->nStartCamset	= prmI.nStartCamset;
		pDeli->nAngle		= prmI.nCamAngle;
		pDeli->nColor		= rec;
		pDeli->nMen			= prmI.nMen;
		pDeli->nUnionWidth	= prmI.nUionWidth;
		pDeli->nUnionHeight	= prmI.nUionHeight;


		//// フレーム毎の情報
		pDeli->nCFno		= p->nCFno;
		pDeli->nEdgeIdx[0]	= p->nEdgeIdx[0]; 
		pDeli->nEdgeIdx[1]	= p->nEdgeIdx[1];
		pDeli->bRpiSave		= true;
		pDeli->emImgSaveStat= p->emImgSaveStat;

	
		//// 付帯的な情報




		//// コイル情報 
		memcpy(pDeli->cKizukenNo, p->cKanriNo, SIZE_KIZUKEN_NO); 
		strcpy(pDeli->cCoilNo, p->cCoilNo);
		pDeli->emCompType	= p->emCompType;
		pDeli->nCompQuality = nCompQuality;
		pDeli->bDummyCoil   = p->bDummyCoil;

		//// UDP情報 ※今回実際にはUDP情報は無いが、改造を少なくするために、使っている情報をセットしておく
		//memcpy( &pDeli->UdpData, &p->Udp.udp, sizeof(UDP_DATA)); 
		memset(&pDeli->UdpData, 0x00, sizeof(pDeli->UdpData));
		pDeli->UdpData.framenum = p->nFno;
		pDeli->UdpData.vRes = prmI.reso_y;
		pDeli->UdpData.pos[prmI.nMen].edgePosL  = p->nEdgePos[0];
		pDeli->UdpData.pos[prmI.nMen].edgePosR  = p->nEdgePos[1];
		pDeli->UdpData.pos[prmI.nMen].posFromHead  
			  = p->mmFromHeadAtCoilStart 
			  + p->nCFno * prmI.reso_y * prm.nFrameSizeY;
		//(間引き後のフレームサイズに、間引き後の分解能をかけているのでよい。)




		//// 時間データセット
		// カメラ取得時間
		__int64 nMin, nMax;
		nMin = nMax = hrf->nTimeFrameRecv;
		for(ii=0; ii<prmI.nCamsetNum; ii++) {
			if( ! p->nIsOk[ii] ) continue;
			nMin = __min(nMin, p->pData[ii]->nTimeFrameRecv);
			nMax = __max(nMax, p->pData[ii]->nTimeFrameRecv);
		}
		pDeli->cls_Time.PutTimestamp( FrameDataManager::TIME_GetCamFast, nMin );	// 最初のカメラ
		pDeli->cls_Time.PutTimestamp( FrameDataManager::TIME_GetCamLast, nMax );	// 最後のカメラ
		// その他
		pDeli->cls_Time.PutTimestamp( FrameDataManager::TIME_PlgSync_END, p->nTimePlgSyncEnd);
		pDeli->cls_Time.PutTimestamp( FrameDataManager::TIME_ImgUnion_START, nTimeStart);
		pDeli->cls_Time.PutTimestamp( FrameDataManager::TIME_ImgUnion_END);		// 最後


		// ==========================
		// デバック情報
		if( 1 <= prm.nDispPrint ) {
			CString sWk;
			KizuBitmapManager	cls;			
			FrameDataManager* pDeli = (FrameDataManager*)p->Fdc.pFrameDataMgr[rec];
			BYTE* pWk = &pDeli->pImgs[prm.nBmpHeadSize];

			int nF = 50; //100;
			int nH = 50; //100;	// フォント100のときに100
			sWk.Format("CFno=%06d", pDeli->nCFno+1 );
			cls.DrawText_Gray( pDeli->nUnionWidth, pDeli->nUnionHeight, pWk, 20, nH, sWk, nF, 255);
			
			sWk.Format("SeqNo =%06d", pDeli->nSeqNum[0] );
			cls.DrawText_Gray( pDeli->nUnionWidth, pDeli->nUnionHeight, pWk, 20, nH*2, sWk, nF, 255);
			for(int ii=1; ii<prmI.nCamsetNum; ii++) {
				sWk.Format("       %06d", pDeli->nSeqNum[ii] );
				cls.DrawText_Gray( pDeli->nUnionWidth, pDeli->nUnionHeight, pWk, 20, (nH*2)+nH*ii, sWk, nF, 255);
			}
		}
	}


//◆
	LOG2(em_INF, "[%s] 下流へ (Fno=%d/%d) st=%d", my_sThreadName, p->nFno, p->nCFno, p->emImgSaveStat );


	////// テスト的に画像出力
/*/
	CString sWk;
	sWk.Format("c:\\compgo%d.bmp", m_nIns); 

	std::ofstream   os;
	os.open(sWk, std::ios::binary);
	os.write((const char*)pDeli->pImgs, pDeli->nLen );
	os.close();
//*/
}

//------------------------------------------
// 輝度補正
// FRAME_DATA_ARRAY* p 幅方向分のフレーム情報
//------------------------------------------
void ImgUnionProc::Exec_Kido(FRAME_DATA_ARRAY* p)
{
	// =========================================================
	// よく使うパラメータをエイリアス
	int nIns = p->nIns;
	ParamManager::P_MAIN		const& prm = mcls_pParam->GetMain();
	ParamManager::P_MAIN::INS   const& prmI = mcls_pParam->GetMain().ins[nIns];
	int nMen = prmI.nMen;


	//// そもそも実施なし？
	bool bNon = false;
	for (int cam = 0; cam<prmI.nCamsetNum; cam++) {
		if (prmI.camset[cam].bKidoUmu) { bNon = true; break; }
	}
	if (!bNon) return;


	//////////////////////////////////////////////////////////////////////////////////
	// 各画像ごとの処理

	short nWk;
	short*  vKido = new short[prm.nFrameSizeX];				// 補正値

	for (int cam = 0; cam<prmI.nCamsetNum; cam++) {
		for (int rec = 0; rec<MAX_COLOR_NUM; rec++) {
			if (NULL == p->Fdc.pFrameDataMgr[rec]) continue;	// 保存色そもそもこない対応

			if (!prmI.camset[cam].bKidoUmu) continue;

			//int nFrom = prm.nFrameSizeX * prm.nFrameSizeY * rec;				// コピー元の配列位置 (色分並んでいるため)
			//int idx = nFrom;
			int idx = 0;
			BYTE* pImg = (BYTE*)p->pData[cam]->base_head.addr[0];	// 今回対象画像の先頭アドレス
			if (NULL == pImg[idx]) continue;

														// 補正値セット。 直接 vector[] でアクセスするとすごく時間が掛かるため
			for (int xx = 0; xx < prm.nFrameSizeX; xx++) vKido[xx] = mcls_pParam->GetKido(nIns, cam)[xx];

			// 補正
			for (int yy = 0; yy < prm.nFrameSizeY; yy++) {
				for (int xx = 0; xx < prm.nFrameSizeX; xx++) {
					nWk = (short)pImg[idx] + vKido[xx];
					pImg[idx] = BYTE_CAST(nWk);

					idx++;
				}
			}
		}
	}

	// 後始末
	delete[] vKido;
}

//------------------------------------------
// 画像連結
// FRAME_DATA_ARRAY* p 幅方向分のフレーム情報
// bool bSkip スキップ
//------------------------------------------
void ImgUnionProc::Exec_Union(FRAME_DATA_ARRAY* p, bool bSkip)
{
	int jj, kk;
	int nWk;
	const BYTE bMask	= 0x00;				// エッジ外のマスク色


	// =========================================================
	// よく使うパラメータをエイリアス
	int nIns = p->nIns;
	ParamManager::P_MAIN		const& prm	= mcls_pParam->GetMain();
	ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[nIns];
	int nMen = prmI.nMen;

	// =========================================================
	// エッジ外 塗り潰し範囲を決定
	int nBlackL=0, nBlackR=0;							// 塗り潰し量 [pixel]

	if (prmI.bEdgeDeleteEnabel && 0 < p->nEdgeIdx[0]) {
		nBlackL = p->nEdgeIdx[0] - prmI.nEdgeDelete[0];						// 左端からの削除位置
	}

	// 右エッジ
	if (prmI.bEdgeDeleteEnabel && 0 < p->nEdgeIdx[1]) {
		nBlackR = prmI.nUionWidth - (p->nEdgeIdx[1] + prmI.nEdgeDelete[1]);	// 右端からの削除位置
	}

	nBlackL = max( nBlackL, prmI.nImgDelete[0]);					// 左エッジ
	nBlackR = max( nBlackR, prmI.nImgDelete[1]);					// 右エッジ

//LOG(em_MSG), "ID=%d   img=%d, C=%d, E=%d/%d, ED=%d/%d R=%d/%d",m_nIns, prmI.nUionWidth, prmI.nLineCenter, 
//(int)p->Udp.udp.pos[nMen].edgePosL, (int)p->Udp.udp.pos[nMen].edgePosR, prmI.nEdgeDelete[0], prmI.nEdgeDelete[1], nBlackL, nBlackR);




	//////////////////////////////////////////////////////////////////////////////////
	// 各画像ごとの処理
	int nTo[MAX_COLOR_NUM];				// コピー先の配列位置
	int nColorNum = 0;
	for(int rec=0; rec<MAX_COLOR_NUM; rec++) {
		if( NULL == p->Fdc.pFrameDataMgr[rec] ) continue;	// 保存色そもそもこない対応

		int nBase	= 0;													// コピー先のベース
		//int nFrom	= prm.nCullSizeX * prm.nCullSizeY * nColorNum;		// コピー元の配列位置 (色分並んでいるため)
		int nFrom	= prm.nFrameSizeX * prm.nFrameSizeY * nColorNum;		// コピー元の配列位置 (色分並んでいるため)
		nColorNum ++;
		FrameDataManager* pDeli = (FrameDataManager*)p->Fdc.pFrameDataMgr[rec];
		

		// =========================================================
		// BMPヘッダーを付与
		nBase = prm.nBmpHeadSize;
		memcpy( pDeli->pImgs, prmI.BmpHead, nBase);
		nTo[rec] = nBase;


		// =========================================================
		// スキップ制御
		if( bSkip ) {
			// 圧縮処理が間に合わないので黒埋めして圧縮時間を短くして長さだけはあわせる
			int nSize = prmI.nUionWidth*prmI.nUionHeight;
			memset(&pDeli->pImgs[ nTo[rec] ], 0x00, nSize);
			nTo[rec] += nSize;
		}

		// =========================================================
		// 画像連結
		if( ! bSkip ) {

			for(int yy=0; yy<prmI.nUionHeight; yy++) {

				//------------------------------
				//// 真ん中データ部
				for(jj=0; jj<prmI.nCamsetNum; jj++ ) {
					
					// 使いやすいように画像ポインタを取り出し
					const BYTE* pImg  = m_BlankData;	// 基準
					const BYTE* pImg2 = m_BlankData;	// 右となり
					if(	p->nIsOk[jj] ) {
						int off = p->pData[jj]->nImgOffset[rec];
						pImg = (BYTE*)p->pData[jj]->base_head.addr[0];
					} 
					if( jj != prmI.nCamsetNum-1 && p->nIsOk[jj+1] ) {
						int off = p->pData[jj+1]->nImgOffset[rec];
						pImg2 = (BYTE*)p->pData[jj+1]->base_head.addr[0];
					} 


					// 単純なコピー
					volatile int src_index = nFrom + prmI.camset[jj].nCopyImg[0];
					volatile int src_len   =         prmI.camset[jj].nCopyImg[1];
					memcpy( &pDeli->pImgs[ nTo[rec] ], &pImg[ src_index ], src_len);
					nTo[rec] += src_len;

					// 直線補完
					if(0 < prmI.camset[jj].nHokanImg[0]) {
						for(kk=0; kk<prmI.camset[jj].nHokanImg[1]; kk++) {
							nWk = (int)( (pImg [ kk + nFrom + prmI.camset[jj].nHokanImg[0] ] * ( prmI.camset[jj].nHokanImg[1] -1 - kk ) +
										  pImg2[ kk + nFrom + (prmI.camset[jj].nOverLap-prmI.camset[jj].nHokanImg[1])] * ( kk )) / ( prmI.camset[jj].nHokanImg[1]));
							pDeli->pImgs[ nTo[rec] ] = BYTE_CAST(nWk); //(BYTE)(nWk<=255 ? nWk : 255);
							nTo[rec]++;
						}
					}
				}

				//------------------------------
				//// 左塗り潰し
				if(0 != nBlackL) {
					memset( &pDeli->pImgs[ nBase + prmI.nUionWidth * yy ], bMask, nBlackL);
				}
				
				//------------------------------
				//// 右塗り潰し ＆ 右挿入
				if(0 != nBlackR || 0 != prmI.nInsertRight ) {
					memset( &pDeli->pImgs[ nBase + prmI.nUionWidth * (yy+1) - (nBlackR+prmI.nInsertRight)], bMask, nBlackR+prmI.nInsertRight);
				
					nTo[rec] +=  prmI.nInsertRight;
				}

				//------------------------------
				//// 次のため
				nFrom += prm.nFrameSizeX;


				////// ※ とりあえず確認用
				//if( 0 != nTo[rec] % prm.nUionWidth ) {
				//	LOG(em_MSG), "画像幅異常=%d/%d", nTo[rec], prm.nUionWidth);
				//}
			}

/*
//// BMP
if(true) {
		//int off = nBase + rec*(prmI.nUionWidth*prmI.nUionHeight);
		BYTE* pImg = (BYTE*)&pDeli->pImgs[ nBase ];

		CString s;
		s.Format("union_%d_%d.bmp", p->nFno, rec);
		KizuBitmapManager kb;
		kb.DataToBMPfile_Gray(prmI.nUionWidth, prmI.nUionHeight, &pImg[0], s);
}
//*/


		}
	}


	// =========================================================
	// その他の受け渡しデータセット
	for(int rec=0; rec<MAX_COLOR_NUM; rec++) {
		if( NULL == p->Fdc.pFrameDataMgr[rec] ) continue;	// データ無し対応

		FrameDataManager* pDeli = (FrameDataManager*)p->Fdc.pFrameDataMgr[rec];

		// 画面用
		nWk = 0;
		for(jj=0; jj<prmI.nCamsetNum; jj++ ) {
			// 通常フレーム画像境界部
			nWk += prmI.camset[jj].nCopyImg[1];
			pDeli->nImgLapPos[jj][0] = nWk;
			// オーバーラップ境界部
			if(0 < prmI.camset[jj].nHokanImg[0]) {
				nWk += prmI.camset[jj].nHokanImg[1];
				pDeli->nImgLapPos[jj][1] = nWk;
			}
		}
		pDeli->nImgClipPos[0] = nBlackL;
		pDeli->nImgClipPos[1] = prmI.nUionWidth - nBlackR;
	}
}


const BYTE *ImgUnionProc::m_BlankData;
