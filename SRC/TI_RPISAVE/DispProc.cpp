#include "StdAfx.h"
#include "DispProc.h"

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
DispProc::DispProc(void) :
ThreadManager("DispP"),
m_bIsExec(false),
m_bIsDisp(false)
{
	Reset();
	InitializeCriticalSection(&my_cs);

	//// シグナル準備
	m_evShowWin				= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evCloseWin			= CreateEvent(NULL, FALSE, FALSE, NULL);
}

//------------------------------------------
// デストラクタ
//------------------------------------------
DispProc::~DispProc(void)
{
	DeleteCriticalSection(&my_cs);
	//// シグナル解放
	CloseHandle(m_evShowWin);
	CloseHandle(m_evCloseWin);
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int DispProc::ThreadFirst()
{
	//// イベント設定
	HANDLE hArray[] = { m_evShowWin,
						m_evCloseWin,
						this->mque_pIn->GetEvQue() 
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int DispProc::ThreadLast()
{
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void DispProc::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_SHOW_WIN = 0,				// 画面表示要求
			EV_CLOSE_WIN,					// 画面終了要求
			EV_ADD_IMG						// フレーム画像キューイング
	};

	////// シグナル条件分け
	switch (nEventNo) {
//-----------------------------------------------------------------------------------------------
	case EV_SHOW_WIN:
		if( ! m_bIsDisp) ShowWindow();
		break;

	case EV_CLOSE_WIN:
		if(m_bIsDisp) CloseWindow();
		break;

//-----------------------------------------------------------------------------------------------
	case EV_ADD_IMG: 						// フレーム取込
		this->ExecQue();
		return;
	
//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 処理

//------------------------------------------
// 実処理
// Tin* pIn		入力Queデータ
// 戻り値		NULL:返却しない 出力Queデータ:次のスレッドへ返却する場合  
//------------------------------------------
FrameDataManager* DispProc::OnAddNewItem( FrameDataManager* p )
{
	if( NULL == p ) {							// 通常ありえないはず
		LOG(em_ERR), "[%s] スレッドループキュー無し!", my_sThreadName);
		return NULL;
	}

	//// 処理
	if(m_bIsExec) {
		Exec(p);

	} else {
		LOG(em_INF), "[%s] キャンセル %d", my_sThreadName, mque_pIn->GetCount());
	}

	//// 器に返却
	return p;
}


//------------------------------------------
// 処理実行
// FrameDataManager* p 情報
//------------------------------------------
void DispProc::Exec(FrameDataManager* p)
{
	int	ii;
	double	dWk;
	int	nIns = p->nIns;
	int nColor = p->nColor;
	ParamManager::P_MAIN		const& prm	= mcls_pParam->GetMain();
	ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[nIns];




	// =========================================================
	// 時間計測結果 表示
	dWk = p->cls_Time.GetTimestamp( FrameDataManager::TIME_GetCamLast, FrameDataManager::TIME_GetCamFast );
	g_dTime_Iport[0] = g_dTime_Iport[0]*0.8 + dWk*0.2;
	g_dTime_Iport[1] = __max(g_dTime_Iport[1], dWk);
	

	dWk = p->cls_Time.GetTimestamp( FrameDataManager::TIME_PlgSync_END, FrameDataManager::TIME_GetCamLast );
	g_dTime_PlgSync[0] = g_dTime_PlgSync[0]*0.8 + dWk*0.2;
	g_dTime_PlgSync[1] = __max(g_dTime_PlgSync[1], dWk);

	//dWk = p->cls_Time.GetTimestamp( FrameDataManager::TIME_Coil_END, FrameDataManager::TIME_Coil_START );
	//g_dTime_CoilControl[0] = g_dTime_CoilControl[0]*0.8 + dWk*0.2;
	//g_dTime_CoilControl[1] = __max(g_dTime_CoilControl[1], dWk);

	dWk = p->cls_Time.GetTimestamp( FrameDataManager::TIME_ImgUnion_END, FrameDataManager::TIME_ImgUnion_START );
	g_dTime_ImgUnion[0] = g_dTime_ImgUnion[0]*0.8 + dWk*0.2;
	g_dTime_ImgUnion[1] = __max(g_dTime_ImgUnion[1], dWk);

	//dWk = p->cls_Time.GetTimestamp( FrameDataManager::TIME_ImgColor_END, FrameDataManager::TIME_ImgColor_START );
	//g_dTime_ImgColor[0] = g_dTime_ImgColor[0]*0.8 + dWk*0.2;
	//g_dTime_ImgColor[1] = __max(g_dTime_ImgColor[1], dWk);

	dWk = p->cls_Time.GetTimestamp( FrameDataManager::TIME_ImgBuf_END, FrameDataManager::TIME_ImgBuf_START );
	g_dTime_ImgBuf[0] = g_dTime_ImgBuf[0]*0.8 + dWk*0.2;
	g_dTime_ImgBuf[1] = __max(g_dTime_ImgBuf[1], dWk);

	dWk = p->cls_Time.GetTimestamp( FrameDataManager::TIME_ImgComp_END, FrameDataManager::TIME_ImgComp_START );
	g_dTime_ImgComp[0] = g_dTime_ImgComp[0]*0.8 + dWk*0.2;
	g_dTime_ImgComp[1] = __max(g_dTime_ImgComp[1], dWk);

	dWk = p->cls_Time.GetTimestamp( FrameDataManager::TIME_RpiWrite_END, FrameDataManager::TIME_RpiWrite_START );
	g_dTime_RpiWrite[nIns][0] = g_dTime_RpiWrite[nIns][0]*0.8 + dWk*0.2;
	g_dTime_RpiWrite[nIns][1] = __max(g_dTime_RpiWrite[nIns][1], dWk);



/*/
	CString s;
	s.Format("c:\\comp%d.bmp", p->nIns);
	std::ofstream   os;
	os.open(s, std::ios::binary);
	os.write((const char*)p->pImgs, p->nLen );
	os.close();
//*/


	// =========================================================
	// 画面表示
	bool bDisp = false;
	if( m_bIsDisp &&
		GetTickCount() > (m_nDspInterval[nIns][nColor]+TIME_DISP_REFRESH) ) {

		bDisp = true;
	}



	//// ウィンドウ表示
	// 2フレーム分連結されている画像を表示
	if( bDisp ) {
		EnterCriticalSection(&my_cs);
		int ch = prm.nImgNumTotal*nIns + nColor;

		// 画像データ
		mcls_pImgWin->Display(ch, (ImageWindow::BITMAPHEADERS*)prmI.BmpHead, &p->pImgs[ prm.nBmpHeadSize ], false);

		// オーバーラップ等 境界部明示
		mcls_pImgWin->SetColor(RGB(255, 128, 0));			// オレンジ
		for(ii=0; ii<prm.ins[nIns].nCamsetNum; ii++ ) {
			mcls_pImgWin->DrawLine(ch, p->nImgLapPos[ii][0], 0, p->nImgLapPos[ii][0], 25, false);
			mcls_pImgWin->DrawLine(ch, p->nImgLapPos[ii][1], 0, p->nImgLapPos[ii][1], 25, false);
		}
		mcls_pImgWin->SetColor(RGB(255, 0, 0));				// 赤
		mcls_pImgWin->DrawLine(ch, p->nImgClipPos[0], 0, p->nImgClipPos[0], 50, false);
		mcls_pImgWin->DrawLine(ch, p->nImgClipPos[1], 0, p->nImgClipPos[1], 50, true);

		// 次の表示用
		m_nDspInterval[nIns][nColor] = GetTickCount();

		LeaveCriticalSection(&my_cs);
	}
}

//------------------------------------------
// リセット
//------------------------------------------
void DispProc::Reset()
{
	for(int ii=0; ii<2; ii++) {
		g_dTime_Iport[ii] = 0.0;
		//g_dTime_Corr[ii] = 0.0;
		g_dTime_PlgSync[ii] = 0.0;
		//g_dTime_CoilControl[ii] = 0.0;
		g_dTime_ImgUnion[ii] = 0.0;
		//g_dTime_ImgColor[ii] = 0.0;
		g_dTime_ImgBuf[ii] = 0.0;
		g_dTime_ImgComp[ii] = 0.0;
		
		for(int jj=0; jj<MAX_INS; jj++) {
			g_dTime_RpiWrite[jj][ii] = 0.0;
		}
	}

	for(int ii=0; ii<MAX_INS; ii++) {
		for(int jj=0; jj<MAX_COLOR_NUM; jj++) {
			m_nDspInterval[ii][jj] = 0;
		}
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// フレーム画像表示画面操作

//------------------------------------------
// 画面表示
//------------------------------------------
void DispProc::ShowWindow()
{
	const ParamManager::P_MAIN& prm = mcls_pParam->GetMain();	// 使いやすいようにエイリアス
	int nInsNum = prm.nIns;		
	int nImgNum = prm.nImgNumTotal;
	int nTotalCnt = prm.nIns * nImgNum;

	// 画面枚数
	mcls_pImgWin->Init( nTotalCnt, AfxGetApp()->m_hInstance );
	for(int ii=0; ii<nInsNum; ii++ ) {
		for(int jj=0; jj<nImgNum; jj++) {
			int ch = nImgNum*ii+jj;

			if(0 != jj) {
				// 低品質
				mcls_pImgWin->SetZoom(ch, 1.0f, 1.0f);
				mcls_pImgWin->SetScale(ch, 1.0f/prm.nDispWindScaleX, 1.0f/prm.nDispWindScaleY);
				mcls_pImgWin->SetSize(ch, (long)(prm.nMaxWidth/prm.nDispWindScaleX), (long)(prm.nMaxHeight/prm.nDispWindScaleY) );
			} else {
				// 高品質
				mcls_pImgWin->SetZoom(ch, 1.0f/prm.nDispWindScaleX, 1.0f/prm.nDispWindScaleY);
				mcls_pImgWin->SetScale(ch, 1.0f, 1.0f);
				mcls_pImgWin->SetSize(ch, (long)(prm.nMaxWidth/prm.nDispWindScaleX), (long)(prm.nMaxHeight/prm.nDispWindScaleY) );
			}
			Sleep(100);
			
			
			mcls_pImgWin->SetPos(ch, prm.nDispWindPosX*jj, prm.nDispWindPosY*ii + prm.nDispWindPosY/4*jj );


			CString str;
			str.Format("連結画像 Inst=%d Img=%d", ii, jj);
			mcls_pImgWin->SetTitle(ch, str);
		}
	}
	//mcls_pImgWin->Init( nInsNum, AfxGetApp()->m_hInstance );

	// すぐに画像表示させようとすると落ちるため、ちょっとだけ表示させるのをヤメ
	for(int ii=0; ii<nInsNum; ii++ ) {
		for(int jj=0; jj<nImgNum; jj++) {
			m_nDspInterval[ii][jj] = GetTickCount()+TIME_DISP_REFRESH;
		}	
	}

	//// 初期表示は最小化しておく
	m_bIsDisp = false;
	for(int ii=0; ii<nInsNum; ii++ ) {
		for(int jj=0; jj<nImgNum; jj++) {
			int ch = nImgNum*ii+jj;
			mcls_pImgWin->ShowWindow(ch, SW_MINIMIZE);
		}
	}
}
//------------------------------------------
// 画面非表示
//------------------------------------------
void DispProc::CloseWindow()
{
	EnterCriticalSection(&my_cs);
	m_bIsDisp = false;
	mcls_pImgWin->Dispose(); 
	LeaveCriticalSection(&my_cs);
}
//------------------------------------------
// 画面描画On/Off
//------------------------------------------
void DispProc::WindowIsOnOff(bool bOnOff)
{
	if(m_bIsDisp == bOnOff) return;

	const ParamManager::P_MAIN& prm = mcls_pParam->GetMain();	// 使いやすいようにエイリアス
	int nInsNum = prm.nIns;		
	int nImgNum = prm.nImgNumTotal;

	for(int ii=0; ii<nInsNum; ii++ ) {
		for(int jj=0; jj<nImgNum; jj++) {
			int ch = nImgNum*ii+jj;

			if(bOnOff)	mcls_pImgWin->ShowWindow(ch, SW_NORMAL);
			else 		mcls_pImgWin->ShowWindow(ch, SW_MINIMIZE);

		}
	}

	m_bIsDisp = bOnOff;
}
