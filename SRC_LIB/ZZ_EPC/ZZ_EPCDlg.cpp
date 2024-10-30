// ZZ_EPCDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "ZZ_EPC.h"
#include "ZZ_EPCDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CZZ_EPCDlg ダイアログ




CZZ_EPCDlg::CZZ_EPCDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CZZ_EPCDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CZZ_EPCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Control(pDX, IDC_CAM_ID, m_cmbCamId);
	DDX_Control(pDX, IDC_SHRINKRATIO, m_cmbShrinkRatio);
}

BEGIN_MESSAGE_MAP(CZZ_EPCDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CZZ_EPCDlg::OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_LIST1, &CZZ_EPCDlg::OnLbnDblclkList1)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_UDPRECV, &CZZ_EPCDlg::OnBnClickedUdprecv)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHANGE_BRIGHTNESS, &CZZ_EPCDlg::OnBnClickedChangeBrightness)
	ON_BN_CLICKED(IDC_CHANGE_CAMGAIN, &CZZ_EPCDlg::OnBnClickedChangeCamgain)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHANGE_EDGE, &CZZ_EPCDlg::OnBnClickedChangeEdge)
	ON_BN_CLICKED(IDC_PUTEDGE, &CZZ_EPCDlg::OnBnClickedPutedge)
	ON_BN_CLICKED(IDC_CNG_KIZU1, &CZZ_EPCDlg::OnBnClickedCngKizu1)
	ON_BN_CLICKED(IDC_STOP_KIZU1, &CZZ_EPCDlg::OnBnClickedStopKizu1)
	ON_BN_CLICKED(IDC_CNG_KIZU2, &CZZ_EPCDlg::OnBnClickedCngKizu2)
	ON_BN_CLICKED(IDC_CNG_KIZU3, &CZZ_EPCDlg::OnBnClickedCngKizu3)
	ON_BN_CLICKED(IDC_CNG_KIZU4, &CZZ_EPCDlg::OnBnClickedCngKizu4)
	ON_BN_CLICKED(IDC_CNG_KIZU5, &CZZ_EPCDlg::OnBnClickedCngKizu5)
	ON_BN_CLICKED(IDC_STOP_KIZU2, &CZZ_EPCDlg::OnBnClickedStopKizu2)
	ON_BN_CLICKED(IDC_STOP_KIZU3, &CZZ_EPCDlg::OnBnClickedStopKizu3)
	ON_BN_CLICKED(IDC_STOP_KIZU4, &CZZ_EPCDlg::OnBnClickedStopKizu4)
	ON_BN_CLICKED(IDC_STOP_KIZU5, &CZZ_EPCDlg::OnBnClickedStopKizu5)
	ON_BN_CLICKED(IDC_UPDATE, &CZZ_EPCDlg::OnBnClickedUpdate)
	ON_BN_CLICKED(IDC_SKIP, &CZZ_EPCDlg::OnBnClickedSkip)
	ON_BN_CLICKED(IDC_CHECK2, &CZZ_EPCDlg::OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_CHANGE_CAM_GAIN, &CZZ_EPCDlg::OnBnClickedChangeCamGain)
	ON_BN_CLICKED(IDC_CHANGE_CAM_EXP, &CZZ_EPCDlg::OnBnClickedChangeCamExp)
	ON_BN_CLICKED(IDC_CHANGE_CAM_VOLT, &CZZ_EPCDlg::OnBnClickedChangeCamVolt)
	ON_BN_CLICKED(IDC_CHANGE_CAM_TEMP, &CZZ_EPCDlg::OnBnClickedChangeCamTemp)
	ON_CBN_SELCHANGE(IDC_CAM_ID, &CZZ_EPCDlg::OnCbnSelchangeCamId)
	ON_BN_CLICKED(IDC_USE_SMEM, &CZZ_EPCDlg::OnBnClickedUseSmem)
	ON_CBN_SELCHANGE(IDC_SHRINKRATIO, &CZZ_EPCDlg::OnCbnSelchangeShrinkratio)
END_MESSAGE_MAP()


// CZZ_EPCDlg メッセージ ハンドラ

BOOL CZZ_EPCDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
// ------------------------------->>>
	// ×ボタン無効
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL) pSysMenu->EnableMenuItem(SC_CLOSE, MF_GRAYED);
	// メインスレッド開始
	InitProcess();
// <<<-------------------------------
	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CZZ_EPCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CZZ_EPCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}







// これ以降 手動追加 -------------------->>>

void CZZ_EPCDlg::OnDestroy()
{
	ExitProcess();

	CDialog::OnDestroy();

	// TODO: ここにメッセージ ハンドラ コードを追加します。
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// ×ボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CZZ_EPCDlg::OnClose()
{
	//CDialog::OnClose();
}
//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 閉じるボタン
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CZZ_EPCDlg::OnBnClickedOk()
{
	//ExitProcess();
	// 画面の終了要求
	PostMessage(WM_QUIT);
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// メモ帳起動
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CZZ_EPCDlg::OnLbnDblclkList1()
{
	// ListBoxコントロールのプロパティは、ソート禁止にしておく事
	// ListBoxコントロールのダブルクリックイベントを追加
	mcls_pLog->ShowEdit();
}

//------------------------------------------
// 各プロセス初期化処理
//------------------------------------------
void CZZ_EPCDlg::InitProcess()
{
	// タスク初期化処理
	if( 0 != task_init("ZZ_EPC", 0) ) _ASSERT(0);

	// LogFileManagerインスタンス生成 (これ以降 ログ出力可能)
	setlocale(LC_ALL, "Japanese");	// これが無いと日本語パスを含むフォルダの場合、保存できない。(ライブラリの中で実行するべきか迷っているが、変更するとタスク全体に影響するので、とりあえずここで設定)
	mcls_pLog = new LogFileManager("ZZ_EPC", &m_ListBox);
	LOG(em_MSG), "タスク起動");
	LOG(em_MSG), "各プロセス開始処理 実行中・・・");

	// メインインスタンス 開始処理
	mcls_MI.SetLogMgr(mcls_pLog);
	mcls_MI.Init();


	//// 画面更新用データセット
	for(int ii=0;ii<8;ii++) {
		CString strWk;
		strWk.Format("%d" ,ii+1);
		m_cmbCamId.AddString(strWk);
	}
	m_cmbCamId.SetCurSel(0);

	//// 画面更新用タイマ生成
	SetTimer(1, 500, NULL);

	((CButton *)GetDlgItem(IDC_USE_SMEM))->SetCheck(true);
	OnCbnSelchangeCamId();

	// 切り出し画像の縮小率
	CString strWk;
	strWk.Format("%0.2f", mcls_MI.gcls_pData->GetShrinkRatio());
	SetDlgItemText(IDC_SHRINKRATIO, strWk);
	
	OnBnClickedUseSmem();

	m_cmbShrinkRatio.AddString("1");
	m_cmbShrinkRatio.AddString("2");
	m_cmbShrinkRatio.AddString("4");
	m_cmbShrinkRatio.SetCurSel(0);

	// 疵情報設定の初期値Ini反映
	CString sWk;
	for (int ii = 0; ii < 5; ii++) {
		// 疵個数
		sWk.Format("PUT_KIZU_%d", ii + 1);
		mcls_MI.gcls_pData->g_nPutKizu[ii] = GetPrivateProfileInt("ZZ_EPC", sWk, 0, TASKINI_NAME);
		// 疵種
		sWk.Format("PUT_TYPE_%d", ii + 1);
		mcls_MI.gcls_pData->g_nPutType[ii] = GetPrivateProfileInt("ZZ_EPC", sWk, 0, TASKINI_NAME);
		// グレード
		sWk.Format("PUT_GRADE_%d", ii + 1);
		mcls_MI.gcls_pData->g_nPutGrade[ii] = GetPrivateProfileInt("ZZ_EPC", sWk, 0, TASKINI_NAME);
		// フレーム間隔
		sWk.Format("PUT_FRAME_%d", ii + 1);
		mcls_MI.gcls_pData->g_nPutFrm[ii] = GetPrivateProfileInt("ZZ_EPC", sWk, 0, TASKINI_NAME);
		// フレーム間隔
		sWk.Format("PUT_CYCLE_%d", ii + 1);
		mcls_MI.gcls_pData->g_nPutCycle[ii] = GetPrivateProfileInt("ZZ_EPC", sWk, 0, TASKINI_NAME);

	}
	OnBnClickedUpdate();		// 更新ボタン押下

	// メインスレッドの実行を行う
	LOG(em_MSG), "各プロセス開始処理 完了");
	mcls_MI.Start();	
	return;
}

//￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
// 各プロセス開放処理
//＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿＿
void CZZ_EPCDlg::ExitProcess()
{
	//// ログ リストボックス表示処理終了
	mcls_pLog->SetListOut(false);					// これ以降 リストボックスへのログ表示禁止

	// メインインスタンス 終了処理
	mcls_MI.Stop();
	mcls_MI.Exit();

	//// ログクラス開放
	LOG(em_MSG), "タスク終了");
	if(NULL != mcls_pLog){
		delete mcls_pLog;
		mcls_pLog = NULL;
	}
	
	// タスク終了処理
	KillTimer(1);			// 画面更新タイマ
	task_exit();

	// 画面の終了要求
//	PostMessage(WM_QUIT);
}

//====================================
// UDP受信したことにする
//====================================
void CZZ_EPCDlg::OnBnClickedUdprecv()
{
	DEBUG_SPEED_INFO readInf;
	memset(&readInf, 0x00, sizeof(readInf));

	readInf.framenum = mcls_MI.gcls_pData->m_nFrameNo;
	readInf.vRes = 0.1f;

	mcls_MI.gcls_pData->SetFrameData(&readInf);
}




void CZZ_EPCDlg::OnBnClickedChangeBrightness()
{
	const int nID[4][3] = {	{IDC_BR_CENTER_ANGLE1, IDC_BR_OV_R_ANGLE1, IDC_BR_OV_L_ANGLE1},
							{IDC_BR_CENTER_ANGLE2, IDC_BR_OV_R_ANGLE2, IDC_BR_OV_L_ANGLE2},
							{IDC_BR_CENTER_ANGLE3, IDC_BR_OV_R_ANGLE3, IDC_BR_OV_L_ANGLE3},
							{IDC_BR_CENTER_ANGLE4, IDC_BR_OV_R_ANGLE4, IDC_BR_OV_L_ANGLE4}};

	int nBrightness[4][3];
	for(int ii=0; ii<4; ii++){
		for(int jj=0; jj<3; jj++){
			nBrightness[ii][jj] = GetDlgItemInt(nID[ii][jj]);
			if(0 > nBrightness[ii][jj])		nBrightness[ii][jj] = 0;
			if(255 < nBrightness[ii][jj])	nBrightness[ii][jj] = 255;
		}
	}

	mcls_MI.gcls_pData->SetBrightness(&nBrightness[0][0]);

}

void CZZ_EPCDlg::OnBnClickedChangeCamgain()
{
	const int nID[3] = {IDC_GAIN_R, IDC_GAIN_G, IDC_GAIN_B  };
	double    dGain[3];

	for(int ii=0; ii<3; ii++){
		dGain[ii] = (double)GetDlgItemInt(nID[ii]);
		if(1 > dGain[ii])	dGain[ii] = 1;
		if(10 < dGain[ii])	dGain[ii] = 10;
	}

	mcls_MI.gcls_pData->SetCamGain(&dGain[0]);
}

void CZZ_EPCDlg::OnBnClickedChangeEdge()
{
	double    dEdge[2];
	CString	  sWk;

	GetDlgItemText(IDC_EDGE, sWk);
	dEdge[0] = (double)KizuFunction::AtoFex(sWk, strlen(sWk));
	GetDlgItemText(IDC_EDGE3, sWk);
	dEdge[1] = (double)KizuFunction::AtoFex(sWk, strlen(sWk));
	mcls_MI.gcls_pData->SetEdge(0, &dEdge[0]);
	mcls_MI.gcls_pData->SetEdge(1, &dEdge[1]);
}

//====================================
// エッジ変更停止
//====================================
void CZZ_EPCDlg::OnBnClickedPutedge()
{
	bool bVal = (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_PUTEDGE) ? true : false );
	mcls_MI.gcls_pData->g_bPutEdge = bVal;
}
//====================================
// 未検査区間出力
//====================================
void CZZ_EPCDlg::OnBnClickedSkip()
{
	bool bVal = (BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_SKIP) ? true : false );
	mcls_MI.gcls_pData->g_bPutSkip = bVal;
}

//====================================
// 画面更新
//====================================
void CZZ_EPCDlg::OnTimer(UINT_PTR nIDEvent)
{
}

//====================================
// 疵出力設定を変更
//------------------------------------
// int n : 疵種n個目(0オリジン)
//====================================
void CZZ_EPCDlg::ChangeKizuInfo(int n)
{
	// 出力条件を取得するオブジェクトID
	int KizuCntArray[5]    = { IDC_KIZUCNT1,   IDC_KIZUCNT2,   IDC_KIZUCNT3,   IDC_KIZUCNT4,   IDC_KIZUCNT5};		// 疵個数
	int KizuTypeArray[5]   = { IDC_KIZUTYPE1,  IDC_KIZUTYPE2,  IDC_KIZUTYPE3,  IDC_KIZUTYPE4,  IDC_KIZUTYPE5};		// 疵種
	int KizuGrArray[5]     = { IDC_KIZUGRADE1, IDC_KIZUGRADE2, IDC_KIZUGRADE3, IDC_KIZUGRADE4, IDC_KIZUGRADE5};		// 疵グレード
	int KizuFrmCntArray[5] = { IDC_FRMCNT1,    IDC_FRMCNT2,    IDC_FRMCNT3,    IDC_FRMCNT4,    IDC_FRMCNT5};		// フレーム間隔
	int KizuCycleArray[5]  = { IDC_CYCLE1,     IDC_CYCLE2,     IDC_CYCLE3,     IDC_CYCLE4,     IDC_CYCLE5};			// 周期
	int nWk;

	// 疵個数
	nWk = GetDlgItemInt(KizuCntArray[n]);
	if(0 > nWk) nWk = 0;
	if(10 < nWk) nWk = 10;
	mcls_MI.gcls_pData->g_nPutKizu[n] = nWk;

	// 疵種
	nWk = GetDlgItemInt(KizuTypeArray[n]);
	if(0 > nWk) nWk = 1;
	mcls_MI.gcls_pData->g_nPutType[n] = nWk;

	// 疵Gr
	nWk = GetDlgItemInt(KizuGrArray[n]);
	if(0 > nWk) nWk = 1;
	mcls_MI.gcls_pData->g_nPutGrade[n] = nWk;

	// フレーム間隔
	nWk = GetDlgItemInt(KizuFrmCntArray[n]);
	if(0 > nWk) nWk = 0;
	mcls_MI.gcls_pData->g_nPutFrm[n] = nWk;

	// 周期
	nWk = GetDlgItemInt(KizuCycleArray[n]);
	if(0 > nWk) nWk = 0;
	mcls_MI.gcls_pData->g_nPutCycle[n] = nWk;

	// 幅座標
	nWk = GetDlgItemInt(IDC_POS_X);
	if (0 > nWk) nWk = 1;
	mcls_MI.gcls_pData->g_nPosX = nWk;
}

//====================================
// 疵出力を停止/開始
//------------------------------------
// int n : 疵種n個目(0オリジン)
//====================================
void CZZ_EPCDlg::StopKizuOutPut(int n)
{
	int StopKizuArray[5]    = { IDC_STOP_KIZU1, IDC_STOP_KIZU2, IDC_STOP_KIZU3, IDC_STOP_KIZU4, IDC_STOP_KIZU5};

	bool bVal = (BST_CHECKED == CWnd::IsDlgButtonChecked(StopKizuArray[n]) ? false : true );
	mcls_MI.gcls_pData->g_bPutKizu[n] = bVal;
}

//====================================
// 疵出力条件 画面表示内容を更新
//====================================
void CZZ_EPCDlg::OnBnClickedUpdate()
{
	int KizuCntArray[5]    = { IDC_KIZUCNT1,   IDC_KIZUCNT2,   IDC_KIZUCNT3,   IDC_KIZUCNT4,   IDC_KIZUCNT5};		// 疵個数
	int KizuTypeArray[5]   = { IDC_KIZUTYPE1,  IDC_KIZUTYPE2,  IDC_KIZUTYPE3,  IDC_KIZUTYPE4,  IDC_KIZUTYPE5};		// 疵種
	int KizuGrArray[5]     = { IDC_KIZUGRADE1, IDC_KIZUGRADE2, IDC_KIZUGRADE3, IDC_KIZUGRADE4, IDC_KIZUGRADE5};		// 疵グレード
	int KizuFrmCntArray[5] = { IDC_FRMCNT1,    IDC_FRMCNT2,    IDC_FRMCNT3,    IDC_FRMCNT4,    IDC_FRMCNT5};		// フレーム間隔
	int KizuCycleArray[5]  = { IDC_CYCLE1,     IDC_CYCLE2,     IDC_CYCLE3,     IDC_CYCLE4,     IDC_CYCLE5};			// 周期
	int StopKizuArray[5]    = { IDC_STOP_KIZU1, IDC_STOP_KIZU2, IDC_STOP_KIZU3, IDC_STOP_KIZU4, IDC_STOP_KIZU5};	// 出力中/停止中

	for(int ii=0; ii<5; ii++){
		SetDlgItemInt(KizuCntArray[ii],    mcls_MI.gcls_pData->g_nPutKizu[ii]);		// 疵個数
		SetDlgItemInt(KizuTypeArray[ii],   mcls_MI.gcls_pData->g_nPutType[ii]);		// 疵種
		SetDlgItemInt(KizuGrArray[ii],     mcls_MI.gcls_pData->g_nPutGrade[ii]);	// 疵グレード
		SetDlgItemInt(KizuFrmCntArray[ii], mcls_MI.gcls_pData->g_nPutFrm[ii]);		// フレーム間隔
		SetDlgItemInt(KizuCycleArray[ii],  mcls_MI.gcls_pData->g_nPutCycle[ii]);	// 周期
	}
}



void CZZ_EPCDlg::OnBnClickedCheck2()
{
	bool bVal = ((BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_CHECK2)) ? true : false);
	LOG(em_MSG), "乱数フラグ変更(%d)", bVal);
	mcls_MI.gcls_pData->SetRandomParam(bVal);
	return;
}


void CZZ_EPCDlg::OnBnClickedChangeCamGain()
{
	int cam = m_cmbCamId.GetCurSel();
	int id[3] = {IDC_CAM_GAIN_1, IDC_CAM_GAIN_2, IDC_CAM_GAIN_3};
	float value[3];
	CString sWk;

	for (int ii = 0; ii < 3; ii++)
	{
		GetDlgItemText(id[ii], sWk);
		value[ii] = KizuFunction::AtoFex(sWk, strlen(sWk));
	}

	LOG(em_MSG), "GAIN %.2f, %.2f, %.2f", value[0], value[1], value[2] );
	mcls_MI.gcls_pCommand->SetGain(cam, value);

	OnBnClickedUseSmem();
}


void CZZ_EPCDlg::OnBnClickedChangeCamExp()
{
	int cam = m_cmbCamId.GetCurSel();
	int id[3] = { IDC_CAM_EXP_1, IDC_CAM_EXP_2, IDC_CAM_EXP_3 };
	float value[3];
	for (int ii = 0; ii < 3; ii++) {
		value[ii] = (float)GetDlgItemInt(id[ii]);
	}

	LOG(em_MSG), "EXP %lf, %lf, %lf", value[0], value[1], value[2] );
	mcls_MI.gcls_pCommand->SetExp(cam, value);

	OnBnClickedUseSmem();
}


void CZZ_EPCDlg::OnBnClickedChangeCamVolt()
{
	int cam = m_cmbCamId.GetCurSel();
	float value[NCL_ncam_max];
	value[cam] = (float)GetDlgItemInt(IDC_CAM_VOLT_1);

	LOG(em_MSG), "VOLT %lf", value[cam] );
	mcls_MI.gcls_pCommand->SetVolt(cam, value);

	OnBnClickedUseSmem();
}


void CZZ_EPCDlg::OnBnClickedChangeCamTemp()
{
	int cam = m_cmbCamId.GetCurSel();
	float value[NCL_ncam_max];
	value[cam] = (float)GetDlgItemInt(IDC_CAM_TEMP_1);

	LOG(em_MSG), "TEMP %lf", value[cam] );
	mcls_MI.gcls_pCommand->SetTemp(cam, value);

	OnBnClickedUseSmem();
}


void CZZ_EPCDlg::OnCbnSelchangeCamId()
{
	int cam = m_cmbCamId.GetCurSel();

	int Gain[3] = { IDC_CAM_GAIN_1, IDC_CAM_GAIN_2, IDC_CAM_GAIN_3 };
	int Exp[3] = { IDC_CAM_EXP_1, IDC_CAM_EXP_2, IDC_CAM_EXP_3 };
	int Volt[1] = { IDC_CAM_VOLT_1 };
	int Temp[1] = { IDC_CAM_TEMP_1 };
	CString sWk;

	for (int jj = 0; jj < 3; jj++)
	{
		sWk.Format("%0.2f", mcls_MI.gcls_pCommand->GetGain(jj, cam));
		SetDlgItemText(Gain[jj], sWk);
		SetDlgItemInt(Exp[jj], (UINT)mcls_MI.gcls_pCommand->GetExp(jj, cam));
	};
	SetDlgItemInt(Volt[0], (UINT)mcls_MI.gcls_pCommand->GetVolt(cam));
	SetDlgItemInt(Temp[0], (UINT)mcls_MI.gcls_pCommand->GetTemp(cam));
}


void CZZ_EPCDlg::OnBnClickedUseSmem()
{
	bool bSHMemSet = ((BST_CHECKED == CWnd::IsDlgButtonChecked(IDC_USE_SMEM)) ? true : false);
	LOG(em_MSG), "共有メモリへのセット：%s", (bSHMemSet ? "設定" : "解除"));

	FIRST_SHMEM_INFOv1 info;
	memset(&info, 0, sizeof(FIRST_SHMEM_INFOv1));

	if (bSHMemSet)
	{
		memcpy(&info.first_smem_req_id, &ID_FIRST_SHMEM_INFO_IDv1, sizeof(GUID));
		mcls_MI.gcls_pData->GetCamset((int *)&info.camnum, &info.camid[0]);
		for (DWORD ii = 0; ii < info.camnum; ii++)
		{
			for (int jj = 0; jj < 3; jj++)
			{
				info.gain[ii][jj] = mcls_MI.gcls_pCommand->GetGain(jj, ii);
				info.exptime[ii][jj] = mcls_MI.gcls_pCommand->GetExp(jj, ii);
			}
			info.volt[ii] = mcls_MI.gcls_pCommand->GetVolt(ii);
			info.temp[ii] = mcls_MI.gcls_pCommand->GetTemp(ii);

			LOG(em_MSG), " カメラ状態 [ID:%d][温度:%.3f][露光:%.3f %.3f %.3f][ゲイン:%.3f %.3f %.3f][電圧:%.3f]"
				, info.camid[ii]
				, info.temp[ii]
				, info.exptime[ii][0]
				, info.exptime[ii][1]
				, info.exptime[ii][2]
				, info.gain[ii][0]
				, info.gain[ii][1]
				, info.gain[ii][2]
				, info.volt[ii]
				);
		}
	}

	mcls_MI.gcls_pData->SetFirstSHMem(bSHMemSet, &info);
}


void CZZ_EPCDlg::OnCbnSelchangeShrinkratio()
{
	float fShrinkRatio[] = {1.0, 2.0, 4.0};
	mcls_MI.gcls_pData->SetShrinkRatio(fShrinkRatio[m_cmbShrinkRatio.GetCurSel()]);
}
