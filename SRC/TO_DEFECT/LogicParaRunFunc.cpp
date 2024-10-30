#include "stdafx.h"
#include "LogicParaRunFunc.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 使用するクラスの定義

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 静的変数の初期化

// ログ用
LogFileManager*					LogicParaRunFunc::mcls_pLog = NULL;
CString							LogicParaRunFunc::my_sThreadName;

// 外部参照
StatusManager*					LogicParaRunFunc::mcls_pStatus = NULL;
PlgManager*						LogicParaRunFunc::mcls_pPlg = NULL;
CoilManager*					LogicParaRunFunc::mcls_pCoil = NULL;
HANDLE							LogicParaRunFunc::m_evTimeCamset = NULL;
HANDLE							LogicParaRunFunc::m_evTimeResumeStart = NULL;
HANDLE							LogicParaRunFunc::m_evTimeResumeWait[NUM_ALL_HTPARA] = { NULL, NULL };

// ロジックパララン処理用
int											LogicParaRunFunc::m_nLogiParaCam[NUM_ALL_HTPARA][NUM_CAMLINK_BOARD];// カメラセット格納領域
LogicParaRunFunc::ENUM_LINKSW_CMD			LogicParaRunFunc::m_LinkswCmdKind;									// TO_LINKSW送信 処理区分
std::vector<LogicParaRunFunc::LINKSW_CMD>	LogicParaRunFunc::mlst_SendCmd;										// TO_LINKSW送信コマンドリスト
int											LogicParaRunFunc::m_nSendCmdIndex;									// TO_LINKSW送信コマンド送信位置
int											LogicParaRunFunc::m_nSelectCamset[NUM_ALL_HANTEI];					// カメラペアID（PCID毎）


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 基本

//==========================================
// コンストラクタ
//==========================================
LogicParaRunFunc::LogicParaRunFunc(LogFileManager* pCls1, StatusManager* pCls2, PlgManager* pCls3)
{
	my_sThreadName = "LogicParaFnc";

	// ロジックパララン用カメラセット情報を初期化
	memset(m_nLogiParaCam, 0x00, sizeof(m_nLogiParaCam));

	// カメラペアIDを初期化
	memset(&m_nSelectCamset, 0x00, sizeof(m_nSelectCamset));

	// 外部参照インスタンスセット
	mcls_pLog	= pCls1;
	mcls_pStatus= pCls2;
	mcls_pPlg	= pCls3;

	// 同期シグナル (タイマー)
	// ※MainInstanceのデストラクタで明示的にCloseHandleすること
	m_evTimeCamset		= CreateWaitableTimer(NULL, FALSE, NULL);		// カメラ選択設定アンサー待ちタイマー
	m_evTimeResumeStart = CreateWaitableTimer(NULL, FALSE, NULL);		// 途中参加設定アンサー待ちタイマー
	for (int ii = 0; ii < NUM_ALL_HTPARA; ii++) {
		m_evTimeResumeWait[ii] = CreateWaitableTimer(NULL, FALSE, NULL);	// 途中開始タイマー判定アンサー待ちタイマー
	}
}

//==========================================
// デストラクタ
//==========================================
LogicParaRunFunc::~LogicParaRunFunc()
{
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 内部関数

//==========================================
// 操作画面に稼動状態通知
// ※MainInstanceの関数を移植
//   MainInstanceの参照を持ちたくなかったので
//==========================================
void LogicParaRunFunc::Send_SoGamenn_MailStatus()
{
	COMMON_QUE que;
	que.nEventNo = FACT_SO_GAMENN_01;
	que.nLineNo = getlineid();

	que.fl.data[0] = mcls_pStatus->GetUnten();
	que.fl.data[1] = mcls_pStatus->GetPlg();
	que.fl.data[2] = mcls_pStatus->GetMenMode() ? 1 : 0;
	que.fl.data[3] = mcls_pStatus->GetKadou();
	que.fl.data[4] = mcls_pStatus->Get_ParaKadou(mcls_pStatus->GetParaSys(0));
	que.fl.data[5] = mcls_pStatus->Get_ParaKadou(mcls_pStatus->GetParaSys(1));

	// 送信
	send_mail(SO_GAMENN, "*", &que);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 公開関数

//==========================================
// カメラセット情報サイズを返す
//==========================================
int LogicParaRunFunc::GetHtParaCamSize()
{
	LOG(em_INF), "[%s] カメラセット領域サイズ:%d", my_sThreadName, sizeof(m_nLogiParaCam));

	return sizeof(m_nLogiParaCam);
}

//==========================================
// 指定したカメラセットを返す
//------------------------------------------
// int pcnoo : パラランID
// int camno : カメラNo
//==========================================
int LogicParaRunFunc::GetHtParaCamNo(int pcno, int camno)
{
	//LOG(em_INF), "[%s] カメラセット取得 pcno:%d camno%d camset:%d", my_sThreadName, pcno, camno, m_nLogiParaCam[pcno][camno]);

	return m_nLogiParaCam[pcno][camno];
}

//==========================================
// 全カメラセットを返す
//==========================================
int* LogicParaRunFunc::GetHtParaCamNo()
{
	LOG(em_INF), "[%s] カメラセット取得 camset:%d/%d %d/%d", my_sThreadName,
		m_nLogiParaCam[0][0], m_nLogiParaCam[0][1],
		m_nLogiParaCam[1][0], m_nLogiParaCam[1][1]);

	return &(m_nLogiParaCam[0][0]);
}

//==========================================
// 全カメラセットを設定する
//------------------------------------------
// int pcnoo : パラランID
// int camno : カメラNo
//==========================================
void LogicParaRunFunc::SetHtParaCamNo(int const* pLogiParaCam)
{
	memcpy(&m_nLogiParaCam, pLogiParaCam, sizeof(m_nLogiParaCam));

	LOG(em_INF), "[%s] カメラセット設定 camset:%d/%d %d/%d", my_sThreadName,
		m_nLogiParaCam[0][0], m_nLogiParaCam[0][1],
		m_nLogiParaCam[1][0], m_nLogiParaCam[1][1]);
}

//==========================================
// 指定したカメラセットを設定する
//------------------------------------------
// int pcnoo  : パラランID
// int camno  : カメラNo
// int camset : カメラセット
//==========================================
void LogicParaRunFunc::SetHtParaCamNo(int pcno, int camno, int camset)
{
	m_nLogiParaCam[pcno][camno] = camset;

	LOG(em_INF), "[%s] カメラセット設定 pcno:%d camno%d camset:%d", my_sThreadName, pcno, camno, camset);
}

//==========================================
// カメラペアIDを初期化
//==========================================
void LogicParaRunFunc::ClearSelectCamset()
{
	// カメラペアIDを初期化
	memset(&m_nSelectCamset, 0x00, sizeof(m_nSelectCamset));
}

//==========================================
// カメラ選択設定完了処理
//------------------------------------------
// int pcid	: 判定PCID
// int camno: 判定カメラペアID
//==========================================
void LogicParaRunFunc::SetSelectCamset(int pcid, int campair)
{
	m_nSelectCamset[pcid] = campair;

	LOG(em_INF), "[%s] 判定ペアカメラIDセット pcid:%d campair:%d", my_sThreadName, pcid + 1, campair);
}

//==========================================
// カメラ選択設定アンサー待ちタイマーハンドルを返す
//==========================================
HANDLE LogicParaRunFunc::GetEvTimeCamset()
{
	return m_evTimeCamset;
}

//==========================================
// 途中参加設定アンサー待ちタイマーハンドルを返す
//==========================================
HANDLE LogicParaRunFunc::GetEvTimeResumeStart()
{
	return m_evTimeResumeStart;
}

//==========================================
// 途中開始判定アンサー待ちタイマーハンドルを返す
//------------------------------------------
// int nKind				: 検査開始区分（0：ロジックパララン機能1、1：ロジックパララン機能2）
//==========================================
HANDLE LogicParaRunFunc::GetEvTimeResumeWait(int nKind)
{
	return m_evTimeResumeWait[nKind];
}

//==========================================
// 途中開始要求
//------------------------------------------
// COMMON_QUE* que	: 起動要因	
//==========================================
void LogicParaRunFunc::ResumeStart(COMMON_QUE* que)
{
	if (DIV_UNTEN_CAMERA == mcls_pStatus->GetUnten())
	{
		// 「カメラ校正」モード時

		LOG(em_ERR), "[%s] カメラ校正モード時は途中開始要求無効", my_sThreadName);
		// オペレータ機能へ稼動状態を送信
		Send_SoGamenn_MailStatus();
		return;
	}

	// カメラリンクスイッチコマンド通信状態チェック
	// ※検査開始中の途中参加、途中参加中の途中参加を想定してのガードだが、
	// 　検査開始・停止画面でボタン抑制しているので発生しない筈
	if (E_LINKSW_CMD_NOP != m_LinkswCmdKind)
	{
		LOG(em_ERR), "[%s] [%s] 時に途中開始要求受信", my_sThreadName, GetLinkswCmdState(m_LinkswCmdKind));
		return;
	}

	COMMON_QUE* pQue = new COMMON_QUE;
	memcpy(pQue, que, sizeof(COMMON_QUE));

	int nKind = pQue->fl.data[7];
	if (0 <= nKind && NUM_ALL_HTPARA > nKind)
	{
		// 検査開始・停止画面で指定したカメラセットを保存
		SetHtParaCamNo(nKind, 0, pQue->fl.data[8]);
		SetHtParaCamNo(nKind, 1, pQue->fl.data[9]);

		// 途中開始初期化処理へ
		ResumeInitStart(pQue);
	}
	else
	{
		LOG(em_INF), "[%s] 途中開始要求：無効な検査開始区分", my_sThreadName);
	}

	delete pQue;
}

//==========================================
// 途中停止要求
//------------------------------------------
// COMMON_QUE* que	: 起動要因	
//==========================================
void LogicParaRunFunc::ResumeStop(COMMON_QUE* que)
{
	if (DIV_UNTEN_CAMERA == mcls_pStatus->GetUnten())
	{
		// 「カメラ校正」モード時

		LOG(em_ERR), "[%s] カメラ校正モード時は途中停止要求無効", my_sThreadName);
		// オペレータ機能へ稼動状態を送信
		Send_SoGamenn_MailStatus();
		return;
	}

	//// もろもろタイマー破棄
	CancelWaitableTimer(m_evTimeResumeWait[que->fl.data[0]]);

	COMMON_QUE* pQue = new COMMON_QUE;
	memcpy(pQue, que, sizeof(COMMON_QUE));

	Send_HtResume_Stop(pQue->fl.data[0]);

	delete pQue;
}

//==========================================
// 途中開始初期化処理 (途中参加設定完了アンサー待ちへ)
//------------------------------------------
// COMMON_QUE *		que		: 起動情報
//==========================================
void LogicParaRunFunc::ResumeInitStart(COMMON_QUE * que)
{
	//========================================================
	// 下準備
	EM_DIV_UNTEN	nUnten	 = (EM_DIV_UNTEN)que->fl.data[0];			// 運転状態
	EM_DIV_PLG		nPlg	 = (EM_DIV_PLG)que->fl.data[1];				// PLG区分
	bool			bMenMode = (0 == que->fl.data[2] ? false : true);	// 片面検査許可/不許可(true:許可)
	int				scode[2] = { que->fl.data[3], que->fl.data[4] };	// 検査表面状態
	int				pcode[2] = { que->fl.data[5], que->fl.data[6] };	// 検査パターン
	int				nKind = que->fl.data[7];							// 検査開始区分（0：ロジックパララン機能1、1：ロジックパララン機能2）
	int				nPcID = mcls_pStatus->GetHtParaID(nKind);			// ロジックパラランPCID
	LINKSW_CMD		*pCmd = NULL;

	if (0 > nKind || 2 <= nKind)
	{
		LOG(em_ERR), "[%s] 検査開始区分異常(%d)", my_sThreadName, nKind);
		return;
	}

	// ----------------------
	// カメラ選択コマンド事前準備
	ClearLinkswCmd(E_LINKSW_CMD_RESUME_START);	// コマンドリストクリア

	// ----------------------
	// カメラ選択設定コマンド送信
	// ※指定ロジックパラランPC分のみ
	CString sWk;
	LINKSW_CMD	cmd;
	for (int ii = 0; ii < NUM_CAMLINK_BOARD; ii++)
	{
		sWk.Format("CAMLINKSW_PCID%d_%d", nPcID, ii + 1);
		int nWk = GetPrivateProfileInt(TO_DEFECT, sWk, 0, TASKINI_NAME);
		int nFPGABoardNo = nWk / 100;
		int nFPGACh = nWk % 100;
		int nCamPort = GetHtParaCamNo(nKind, ii);

		if (0 == nFPGACh) continue;			// ボード未割当

		// カメラ選択設定コマンド登録（Slave指定）
		memset(&cmd, 0x00, sizeof(LINKSW_CMD));
		cmd.prNo	 = nKind + 1;									// ロジックパラランPC no
		cmd.camNo	 = ii + 1;										// 入力カメラ no
		cmd.topCamNo = mcls_pStatus->GetGroupTopCamNo(nFPGABoardNo);// 先頭カメラNo グループ内の入力カメラの先頭
		cmd.bRecv	 = false;										// 受信フラグクリア
		ToOutMailSender::Create_ToLinksw_ChangeCam(nFPGABoardNo, nFPGACh, nCamPort, -1, &cmd.sendCmd);
		SetLinkswCmd(cmd);
	}
	// 途中参加設定コマンドキューイング（全ロジックパラランPC分）
	// ※送信直前でセットするため、途中参加フレームは０をセット
	memset(&cmd, 0x00, sizeof(LINKSW_CMD));
	cmd.prNo = nKind + 1;											// ロジックパラランPC no
	ToOutMailSender::Create_ToLinksw_JoinReq(2 + nKind, 0, &cmd.sendCmd);
	SetLinkswCmd(cmd);

	// ----------------------
	// 機器状態
	for (int ii = 0; ii < NUM_CAMLINK_BOARD; ii++)
	{
		// 機器状態－ロジックパララン機能 入力カメラ選択結果（正常）
		KizuFunction::SetStatus(mcls_pStatus->GetStatusRead().ParaState[nKind][ii].id, true, false);
		// 機器状態－ロジックパララン機能 入力カメラ（不明）
		KizuFunction::SetStatus(mcls_pStatus->GetStatusRead().ParaCam[nKind][ii].id, -1, false);
	}

	// ----------------------
	// パララン機能システム状態変更
	mcls_pStatus->SetParaSys(nKind, DIV_SYS_INITSTART);
	// ロジックパララン状態 － セット
	mcls_pStatus->SetHtParaState(nKind, DIV_BOOL_NON);

	// カメラ選択設定アンサー待ちタイマー破棄（念のため）
	CancelWaitableTimer(m_evTimeCamset);
	// 途中参加設定アンサー待ちタイマー破棄（念のため）
	CancelWaitableTimer(m_evTimeResumeStart);
	// 途中開始判定アンサー待ちタイマー破棄（念のため）
	CancelWaitableTimer(m_evTimeResumeWait[nKind]);

	// 次のコマンド送信（キュー制御）
	Send_NextCmdSelect();
}

//==========================================
// 途中開始アンサー 処理　(←HT_DEFECT)
//------------------------------------------
// COMMON_QUE* que	: 起動情報
//==========================================
void LogicParaRunFunc::WaitResumeStart_Answer(COMMON_QUE* que)
{
	//======================================================
	// 下準備
	int campair = que->fl.data[0];							// カメラペア (1オリジン)
	int pcid = que->fl.data[1];							// PCID (1オリジン)
	int ans = que->fl.data[2];							// 結果 (0:正常 1:軽故障 -1:重故障)
	int frameNo = que->fl.data[3];							// 途中参加フレーム
	EM_DIV_HANTEI emKind = (EM_DIV_HANTEI)que->fl.data[4];	// 担当判定
	CString wk;

	// [担当判定] パララン判定
	if (DIV_HANTEI_PR == emKind)
	{
		int nPcIndex = -1;

		for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
		{
			// 応答したロジックパラランPCのみ状態変更
			if (mcls_pStatus->GetHtParaID(ii) == pcid)
			{
				nPcIndex = ii;
				break;
			}
		}

		if (-1 == nPcIndex)
		{
			// 異常応答
			LOG(em_ERR), "[%s] 途中開始アンサー 異常検出(PCID異常) pcid=%d", my_sThreadName, pcid);
			// 処理中断
			return;
		}

		//// もろもろタイマー破棄
		CancelWaitableTimer(m_evTimeResumeWait[nPcIndex]);

		if (mcls_pStatus->GetParaSys(nPcIndex) != DIV_SYS_WAITSTART) {
			// 異常応答
			LOG(em_ERR), "[%s] [%s]時に途中開始アンサー通知受信", my_sThreadName, GetDivSys(mcls_pStatus->GetParaSys(nPcIndex)));
			// 処理中断
			return;
		}

		// 異常応答
		if (0 != ans)
		{
			LOG(em_ERR), "[%s] 途中開始アンサー 異常検出(結果異常) pcid=%d", my_sThreadName, pcid);

			// ----------------------
			// 機器状態－ロジックパララン機能xx_カメラxx選択結果（異常）
			for (int ii = 0; ii < NUM_CAMLINK_BOARD; ii++)
			{
				KizuFunction::SetStatus(mcls_pStatus->GetStatusRead().ParaState[nPcIndex][ii].id, false);
			}
		}

		// ----------------------
		// ロジックパララン状態 － セット
		// （ロジックパララン状態欄表示用）
		// 応答したロジックパラランPCのみ状態変更
		mcls_pStatus->SetHtParaState(nPcIndex, (0 == ans ? DIV_BOOL_TRUE : DIV_BOOL_FALSE));

		// ----------------------
		// パララン機能システム状態変更
		if (0 == ans) {
			mcls_pStatus->SetParaSys(nPcIndex, DIV_SYS_RUN);
		} else {
			mcls_pStatus->SetParaSys(nPcIndex, DIV_SYS_STOP);
		}

		LOG(em_MSG), "[%s] ロジックパララン%d 設定完了！！", my_sThreadName, nPcIndex + 1);

		// オペレータ機能へ現状態を送信
		Send_SoGamenn_MailStatus();
		OpSendFunc::SendOp_Status();
	}
	// [担当判定] 単発判定
	else
	{
		// 異常応答
		LOG(em_ERR), "[%s] 途中開始アンサー 異常検出(単発判定) pcid=%d", my_sThreadName, pcid);
	}
}

//==========================================
// 途中停止アンサー 処理　(←HT_DEFECT)
// ※待ち合わせ処理は実施しない
// 　⇒異常応答時のログ出力のみ
//------------------------------------------
// COMMON_QUE* que	: 起動情報
//==========================================
void LogicParaRunFunc::WaitResumeStop_Answer(COMMON_QUE* que)
{
	//======================================================
	// 下準備
	int campair	= que->fl.data[0];							// カメラペア (1オリジン)
	int pcid	= que->fl.data[1];							// PCID (1オリジン)
	int ans		= que->fl.data[2];							// 結果 (0:正常 1:軽故障 -1:重故障)
	int mode	= que->fl.data[3];							// 区分 (0:応答 1:いきなり)
	EM_DIV_HANTEI emKind = (EM_DIV_HANTEI)que->fl.data[4];	// 担当判定
	CString wk;

	// [担当判定] パララン判定
	if (DIV_HANTEI_PR == emKind)
	{
		// 異常応答
		if (0 != ans || 0 != mode)
		{
			LOG(em_ERR), "[%s] 途中停止アンサー 異常検出(ロジックパララン) pcid=%d", my_sThreadName, pcid);
		}

		int nPcIndex = -1;

		for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
		{
			// 応答したロジックパラランPCのみ状態変更
			if (mcls_pStatus->GetHtParaID(ii) == pcid)
			{
				nPcIndex = ii;
				break;
			}
		}

		if (-1 == nPcIndex)
		{
			// 異常応答
			LOG(em_ERR), "[%s] 途中停止アンサー 異常検出(PCID異常) pcid=%d", my_sThreadName, pcid);
			// 処理中断
			return;
		}

		// ロジックパララン状態 － クリア
		mcls_pStatus->SetHtParaState(nPcIndex, DIV_BOOL_FALSE);

		// パララン機能システム状態変更
		mcls_pStatus->SetParaSys(nPcIndex, DIV_SYS_STOP);

		// オペレータ機能へ現状態を送信
		Send_SoGamenn_MailStatus();
		OpSendFunc::SendOp_Status();

		LOG(em_MSG), "[%s] ロジックパララン%d 停止！！", my_sThreadName, nPcIndex + 1);
	}
	// [担当判定] 単発判定
	else
	{
		// 異常応答
		// ※常に正常で返す筈
		if (0 != ans || 0 != mode)
		{
			LOG(em_ERR), "[%s] 途中停止アンサー 異常検出(単発判定) pcid=%d", my_sThreadName, pcid);
		}
	}
}

//==========================================
// 判定PC へ 途中開始を通知
//------------------------------------------
// int nKind				: 検査開始区分（0：ロジックパララン機能1、1：ロジックパララン機能2）
// int nJoinFrameNo			: 途中参加フレームNo
// CoilManager *pCoilManager: 
//==========================================
void LogicParaRunFunc::Send_HtResume_Start(int nKind, CoilManager const *pCoilManager, int nJoinFrameNo)
{
	EM_DIV_UNTEN  emUnten = mcls_pStatus->GetUnten();

	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_HT_DEFECT_22;
	que.nLineNo = getlineid();

	// 検査表面状態No(表・裏)
	que.fl.data[0] = emUnten;
	que.fl.data[1] = pCoilManager->GetInitScode(0);
	que.fl.data[2] = pCoilManager->GetInitScode(1);

	// 初期距離(表・裏)
	int nDistLen = mcls_pPlg->GetDistLen();
	int nDistPos = mcls_pPlg->GetDistPos();
	int nDistNotPos = mcls_pPlg->GetDistNotPos();
	que.fl.data[3 + nDistPos] = nDistLen;			// 基準面が先に進んでいる
	que.fl.data[3 + nDistNotPos] = 0;

	// 途中参加フレームNo
	que.fl.data[5] = nJoinFrameNo;

	//// バックアップで初期化する
	for (int ii = 0; ii < NUM_ALL_HANTEI; ii++)
	{
		que.fl.data[6 + 2 * ii + 0] = 0;
		que.fl.data[6 + 2 * ii + 1] = 0;
		que.fl.data[6 + 2 * ii + 2] = DIV_HANTEI_BK;
	}

	// 指定ロジックパララン機能のみ
	int targetPCID = mcls_pStatus->GetHtParaID(nKind);

	//// PCID順に カメラペアをセット
	// ※ロジックパラランのみセットする
	if (targetPCID > 0 && targetPCID <= NUM_ALL_HANTEI)
	{
		for (int ii = 0; ii < NUM_MEN; ii++)
		{
			// 単発
			for (int jj = 0; jj < NUM_HANTEI; jj++)
			{
				int pcid = mcls_pStatus->GetHtID(ii, jj);

				if (pcid > 0 && pcid <= NUM_ALL_HANTEI)
				{
					que.fl.data[6 + 3 * (pcid - 1) + 0] = 0;			// カメラペアＩＤ（クリア）
					que.fl.data[6 + 3 * (pcid - 1) + 1] = 0;			// 未使用
					que.fl.data[6 + 3 * (pcid - 1) + 2] = DIV_HANTEI_HT;// 単発機能
				}
			}
#ifdef ENABLE_CYCLE
			// 周期
			for (int jj = 0; jj < NUM_HANTEI_CYCLE; jj++)
			{
				int pcid = gcls_Status.GetCyID(ii, jj);

				if (pcid > 0 && pcid <= NUM_ALL_HANTEI)
				{
					que.fl.data[6 + 3 * (pcid - 1) + 0] = 0;			// カメラペアＩＤ（クリア）
					que.fl.data[6 + 3 * (pcid - 1) + 1] = 0;			// 未使用
					que.fl.data[6 + 3 * (pcid - 1) + 2] = DIV_HANTEI_CY;// 周期機能
				}
			}
#endif
		}
		// クリアしてからセット
		for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
		{
			int pcid = mcls_pStatus->GetHtParaID(ii);

			if (pcid > 0 && pcid <= NUM_ALL_HANTEI) {

				que.fl.data[6 + 3 * (pcid - 1) + 0] = 0;				// カメラセット１（クリア）
				que.fl.data[6 + 3 * (pcid - 1) + 1] = 0;				// カメラセット２（クリア）
				que.fl.data[6 + 3 * (pcid - 1) + 2] = DIV_HANTEI_PR;	// パララン
			}
		}
		int camset1 = GetHtParaCamNo(nKind, 0);
		int camset2 = GetHtParaCamNo(nKind, 1);


		// 0のまま送ると何処に対しての要求なのか分からないため、
		// -1で送信し、判定側で0に戻す
		que.fl.data[6 + 3 * (targetPCID - 1) + 0] = (0 == camset1 ? -1 : camset1);
		que.fl.data[6 + 3 * (targetPCID - 1) + 1] = (0 == camset2 ? -1 : camset2);
		que.fl.data[6 + 3 * (targetPCID - 1) + 2] = DIV_HANTEI_PR;

		// カメラセットから面を判定
		int nMen = 0;
		if (0 != camset1) nMen = (0 == ((camset1 - 1) / (MAX_CAMSET / NUM_MEN)) ? 0 : 1);
		if (0 != camset2) nMen = (0 == ((camset2 - 1) / (MAX_CAMSET / NUM_MEN)) ? 0 : 1);

		// 現コイルの検査表面状態を設定
		// ※初回起動時はコイルのセットが追い付かない可能性があるので、その場合は初期値
		if (NULL != pCoilManager && NULL != pCoilManager->GetCoilBaseKen())
		{
			for (int ii = 0; ii < NUM_MEN; ii++)
			{
				if (0 != pCoilManager->GetCoilKenR(nMen)->nSt_Spm)
				{
					// 圧下状態
					que.fl.data[1 + ii] = pCoilManager->GetCoilBaseKen()->setting.scode[ii];
				}
				else
				{
					// 開放状態
					que.fl.data[1 + ii] = pCoilManager->GetCoilBaseKen()->setting.scode_spmoff[ii];
				}
			}
		}
	}

	//// 送信
	int iRet = send_mail(HT_DEFECT, "*", &que);
	LOG(em_MSG), "[%s] 途中開始要求 (FACT_HT_DEFECT_22) [PCID=%d] %d", my_sThreadName, targetPCID, que.fl.data[0]);
	if (0 != iRet) syslog(SYSNO_MAIL_SNED_ERR, "[途中開始要求 err_code=%d]", iRet);

	// ----------------------
	// パララン機能システム状態変更
	mcls_pStatus->SetParaSys(nKind, DIV_SYS_WAITSTART);

	//======================================================
	// 判定PCからの応答待ちタイマー起動
	__int64 ts = (__int64)PARAM_TIMEOUT_RESUME_WAIT * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evTimeResumeWait[nKind], (LARGE_INTEGER*)&ts, 0, NULL, NULL, FALSE);

}

//==========================================
// 判定PC へ 途中停止を通知
//------------------------------------------
// int nKind	: 検査開始区分（0：ロジックパララン機能1、1：ロジックパララン機能2）
//==========================================
void LogicParaRunFunc::Send_HtResume_Stop(int nKind)
{
	EM_DIV_UNTEN  emUnten = mcls_pStatus->GetUnten();

	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_HT_DEFECT_23;
	que.nLineNo	 = getlineid();

	//// とりあえず 無視をセット
	for (int ii = 0; ii < NUM_ALL_HANTEI; ii++)
	{
		que.fl.data[ii] = -1;
	}

	// 指定ロジックパララン機能のみ
	// 　ロジックパララン機能1：表の１台目
	// 　ロジックパララン機能2：裏の１台目
	int targetPCID = mcls_pStatus->GetHtParaID(nKind);

	//// PCID順に カメラペアをセット
	// ※ロジックパラランのみセットする
	if (targetPCID > 0 && targetPCID <= NUM_ALL_HANTEI)
	{
		que.fl.data[targetPCID - 1] = 0;
	}

	//// 送信
	int iRet = send_mail(HT_DEFECT, "*", &que);
	LOG(em_MSG), "[%s] 途中停止要求 (FACT_HT_DEFECT_23) PCID=%d", my_sThreadName, targetPCID);
	if (0 != iRet) syslog(SYSNO_MAIL_SNED_ERR, "[途中停止要求 err_code=%d]", iRet);

	// ----------------------
	// パララン機能システム状態変更
	mcls_pStatus->SetParaSys(nKind, DIV_SYS_WAITSTOP);
}

//==========================================
// デバッグ用 カメラ選択設定完了通知送信
//==========================================
void LogicParaRunFunc::DebugSend_CamLinksw_CamsetCmpl(int mode)
{
	if (mlst_SendCmd.size() <= m_nSendCmdIndex)
	{
		// TO_LINKSW送信コマンドリストEmpty

		// 検査開始、途中参加以外でカメラ選択設定完了が通知されるケース
		COMMON_QUE que;
		que.nEventNo = FACT_TO_DEFECT_32;
		que.nLineNo = getlineid();
		que.fl.data[0] = 1;
		que.fl.data[1] = 2;
		que.fl.data[2] = 3;
		que.fl.data[3] = 4;
		que.fl.data[4] = 1;
		send_mail(TO_DEFECT, ".", &que);
	}
	else
	{
		// 検査開始、途中参加時

		if (0 == mode || 1 == mode)
		{
			// 正常応答、異常応答

			// カメラ選択設定実施中に限り通知
			if (FACT_TO_LINKSW_03 == mlst_SendCmd[m_nSendCmdIndex].sendCmd.nEventNo)
			{
				// 送信中のカメラ選択設定コマンドから応答伝文を生成・自身に返送
				COMMON_QUE que;
				que.nEventNo = FACT_TO_DEFECT_32;
				que.nLineNo = getlineid();
				que.fl.data[0] = mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[0];
				que.fl.data[1] = mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[1];
				que.fl.data[2] = mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[2];
				que.fl.data[3] = mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[3];
				que.fl.data[4] = (0 == mode ? 1 : 0);
				send_mail(TO_DEFECT, ".", &que);
			}
		}
		else
		{
			// 伝文不一致（カメラNoで不一致にする）
			COMMON_QUE que;
			que.nEventNo = FACT_TO_DEFECT_32;
			que.nLineNo = getlineid();
			que.fl.data[0] = mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[0];
			que.fl.data[1] = mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[1];
			que.fl.data[2] = 99;
			que.fl.data[3] = mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[3];
			que.fl.data[4] = 1;
			send_mail(TO_DEFECT, ".", &que);
		}
	}
}

//==========================================
// パラランカメラの有効性チェック
//------------------------------------------
// long nFPGABoardNo	: スイッチFPGAボードID (0 - 7)
// long nCamno			: カメラNo (1 - 20)
//==========================================
bool LogicParaRunFunc::IsTargetCamNo(long nFPGABoardNo, long nCamno)
{
	bool bTargetCam = false;
	CString sWk;
	bool bTargetPC[NUM_ALL_HANTEI];

	memset(&bTargetPC, false, sizeof(bTargetPC));

	// ①スイッチFPGAボードIDからグループIDを算出（0-7 ⇒ 0-3）
	int nTargetGroupID = nFPGABoardNo / 2;

	// ②パラランPCと同じグループの判定PCを検索する
	for (int ii = 0; ii < NUM_MEN; ii++)
	{
		for (int jj = 0; jj < NUM_HANTEI; jj++)
		{
			int nPcID = mcls_pStatus->GetHtID(ii, jj);
			if (0 == nPcID) continue;
			for (int kk = 0; kk < NUM_CAMLINK_BOARD; kk++)
			{
				int nGroupID = ToLinkswUtil::GetHanteiGroup(ii, jj, kk + 1);
				if (nGroupID == nTargetGroupID) bTargetPC[nPcID - 1] = true;
			}
		}
	}

	for (int ii = 0; ii < NUM_ALL_HANTEI; ii++)
	{
		if (bTargetPC[ii])
		{
			// ③判定PCに割りついているカメラセットを取得する
			int nCamset = m_nSelectCamset[ii];

			// ④カメラセットをカメラ番号に変換する
			int nCam1 = (nCamset * 2) - 1;
			int nCam2 = nCamset * 2;

			// ⑤指定カメラがカメラ番号に含まれるかチェック
			if (nCam1 == nCamno || nCam2 == nCamno)
			{
				bTargetCam = true;
				break;
			}
		}
	}

	return bTargetCam;
}
//------------------------------------------
// 次のカメラ選択設定コマンド送信（キュー制御）
// ※予めm_nSendCamIndexを加算してから呼び出すこと
//------------------------------------------
void LogicParaRunFunc::Send_NextCmdSelect()
{
	// カメラ設定コマンドキューチェック
	if (mlst_SendCmd.empty())
	{
		// 全カメラ設定コマンド送信後にアンサーが到着した場合に、このケースに入りうる
		// ※応答タイムアウトを繰り返し、全コマンド送信完了した後にアンサーが到着した場合

		LOG(em_ERR), "[%s] 送信コマンド無し！！", my_sThreadName);
		// コマンドリストクリア（念の為）
		ClearLinkswCmd();
	}
	else
	{
		// カメラ設定コマンド数と送信位置から未送信コマンドの有無をチェックする
		if (mlst_SendCmd.size() > m_nSendCmdIndex)
		{
			// 未送信カメラ設定コマンドあり（送信中）

			switch (mlst_SendCmd[m_nSendCmdIndex].sendCmd.nEventNo)
			{
				// カメラ選択設定コマンド (←TO_LINKSW)
				case FACT_TO_LINKSW_03:
					{
						int nCamSet = mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[2];

						// 次送信カメラ設定コマンド送信
						if (IsTargetCamNo(mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[0], nCamSet))
						{
							// カメラNoを変換（1-20 ⇒ 1-8）
							mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[2] = nCamSet - mlst_SendCmd[m_nSendCmdIndex].topCamNo;
						}
						else
						{
							// 対象外のカメラが選択された
							LOG(em_ERR), "[%s] 対象外のカメラが選択されている[スイッチFPGAボードID=%d, フレームグラバーボードNo=%d, カメラNo=%d Master/Slave設定=%d]",
								my_sThreadName,
								mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[0],
								mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[1],
								mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[2],
								mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[3]);
							syslog(235, "対象外のカメラが選択されている[スイッチFPGAボードID=%d, フレームグラバーボードNo=%d, カメラNo=%d Master/Slave設定=%d]",
								mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[0],
								mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[1],
								mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[2],
								mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[3]);
							// 出力停止
							mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[2] = 0;
							// 機器状態－ロジックパララン機能xx_カメラxx選択結果（異常）
							KizuFunction::SetStatus(mcls_pStatus->GetStatusRead().ParaState[mlst_SendCmd[m_nSendCmdIndex].prNo - 1][mlst_SendCmd[m_nSendCmdIndex].camNo - 1].id, false);

							// 指定カメラクリア
							SetHtParaCamNo(mlst_SendCmd[m_nSendCmdIndex].prNo - 1, mlst_SendCmd[m_nSendCmdIndex].camNo - 1, 0);
						}

						// コマンド送信
						LOG(em_MSG), "[%s] カメラ選択設定コマンド送信(FACT_TO_LINKSW_03) [スイッチFPGAボードID=%d, フレームグラバーボードNo=%d, カメラNo=%d(%d) Master/Slave設定=%d]",
							my_sThreadName,
							mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[0],
							mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[1],
							mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[2],
							nCamSet,
							mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[3]);
						ToOutMailSender::Send_ToLinksw_ChangeCam(&mlst_SendCmd[m_nSendCmdIndex].sendCmd);

						// カメラリンクスイッチ設定アンサー待ちタイマー起動
						__int64 ts = (__int64)PARAM_TIMEOUT_CAMSET * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
						SetWaitableTimer(m_evTimeCamset, (LARGE_INTEGER*)&ts, 0, NULL, NULL, FALSE);
					}
					break;

				// 途中参加設定コマンド	(←TO_LINKSW)
				case FACT_TO_LINKSW_04:
					{
						// オペレータ機能へ現状態を送信
						// ※稼働中に遷移完了しているときのみ送信
						if (DIV_KADOU_START == mcls_pStatus->GetKadou())
						{
							Send_SoGamenn_MailStatus();
							OpSendFunc::SendOp_Status();
						}

						// 途中参加設定アンサー待ちタイマー破棄（念のため）
						CancelWaitableTimer(m_evTimeResumeStart);

						// 現在のフレームNoに途中参加フレームを加算
						mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[1] = mcls_pPlg->GetFrameNo() + NUM_JOIN_FRAME;

						LOG(em_MSG), "[%s] 途中参加設定コマンド送信(FACT_TO_LINKSW_04)[表裏区分=%d 途中参加フレームNo=%d]", my_sThreadName, mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[0], mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[1]);
						ToOutMailSender::Send_ToLinksw_JoinReq(&mlst_SendCmd[m_nSendCmdIndex].sendCmd);

						// 途中参加設定アンサー待ちタイマー起動
						__int64 ts = (__int64)PARAM_TIMEOUT_RESUME_START * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
						SetWaitableTimer(m_evTimeResumeStart, (LARGE_INTEGER*)&ts, 0, NULL, NULL, FALSE);
					}
					break;

				default:
					LOG(em_ERR), "[%s] 想定外のコマンド[%d]", my_sThreadName, mlst_SendCmd[m_nSendCmdIndex].sendCmd.nEventNo);
					// コマンドリストクリア
					ClearLinkswCmd();
					break;
			}
		}
		else
		{
			// 未送信カメラ設定コマンドなし（全て送信済み）
			// ※登録してコマンド数を送信位置が超えた場合、
			// 　全コマンド送信済みということ

			LOG(em_MSG), "[%s] 全カメラリンクスイッチコマンド送信完了！！", my_sThreadName);

			// カメラ選択設定アンサー待ちタイマー破棄（念のため）
			CancelWaitableTimer(m_evTimeCamset);
			// 途中参加設定アンサー待ちタイマー破棄（念のため）
			CancelWaitableTimer(m_evTimeResumeStart);
			// コマンドリストクリア
			ClearLinkswCmd();

			// オペレータ機能へ現状態を送信
			// ※稼働中に遷移完了しているときのみ送信
			if (DIV_KADOU_START == mcls_pStatus->GetKadou())
			{
				Send_SoGamenn_MailStatus();
				OpSendFunc::SendOp_Status();
			}
		}
	}
}

//------------------------------------------
// 送信中のカメラ設定コマンドの応答かチェックする
// ※タイムアウト後の受信を考慮し、コマンドキューの
//   何れかと一致するかを検索
//------------------------------------------
// COMMON_QUE* que	: メールスロット通信用構造体
//------------------------------------------
bool LogicParaRunFunc::IsEqualCamsetCMD(COMMON_QUE* que)
{
	bool bResp = false;
	std::vector<LINKSW_CMD>::iterator itr;

	for (itr = mlst_SendCmd.begin(); itr != mlst_SendCmd.end(); itr++)
	{
		if (itr->sendCmd.fl.data[0] == que->fl.data[0] &&
			itr->sendCmd.fl.data[1] == que->fl.data[1] &&
			itr->sendCmd.fl.data[2] == que->fl.data[2] &&
			itr->sendCmd.fl.data[3] == que->fl.data[3])
		{
			// 送信コマンドと一致
			bResp = true;
			itr->bRecv = true;
			break;
		}
	}

	return bResp;
}

//------------------------------------------
// 指定パラランのカメラ設定完了通知チェック
// ※１件でも正常受信があればtrueを返す
//------------------------------------------
// int nPrNo	: 1：ロジックパララン１、2：ロジックパララン2
//------------------------------------------
bool LogicParaRunFunc::IsRecvCamsetCMD(int nPrNo)
{
	bool bResp = false;
	std::vector<LINKSW_CMD>::iterator itr;

	for (itr = mlst_SendCmd.begin(); itr != mlst_SendCmd.end(); itr++)
	{
		if ((itr->prNo == nPrNo) && (itr->bRecv))
		{
			// 受信あり
			bResp = true;
			break;
		}
	}

	return bResp;
}

//------------------------------------------
// スレッドキューにリンクスイッチコマンド登録
//------------------------------------------
// LINKSW_CMD* cmd リンクスイッチコマンド構造体
//------------------------------------------
void LogicParaRunFunc::SetLinkswCmd(LINKSW_CMD cmd)
{
	mlst_SendCmd.push_back(cmd);
}

//------------------------------------------
// コマンドリストクリア
//------------------------------------------
// ENUM_LINKSW_CMD state 通信状態
//------------------------------------------
void LogicParaRunFunc::ClearLinkswCmd(ENUM_LINKSW_CMD state)
{
	// カメラリンクスイッチコマンド通信状態設定
	m_LinkswCmdKind = state;
	// カメラリンクスイッチコマンド送信位置初期化
	m_nSendCmdIndex = 0;
	// コマンドリストクリア
	mlst_SendCmd.clear();
}

//------------------------------------------
// コマンドリストクリア
//------------------------------------------
void LogicParaRunFunc::RegistLinkswCmd()
{
	// カメラ選択設定コマンドキューイング（全ロジックパラランPC分）
	CString sWk;
	LINKSW_CMD cmd;
	for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
	{
		int pcid = mcls_pStatus->GetHtParaID(ii);
		if (0 == pcid) continue;				// ロジックパラランPC未割当

		for (int jj = 0; jj < NUM_CAMLINK_BOARD; jj++)
		{
			sWk.Format("CAMLINKSW_PCID%d_%d", pcid, jj + 1);
			int nWk = GetPrivateProfileInt(TO_DEFECT, sWk, 0, TASKINI_NAME);
			int nFPGABoardNo = nWk / 100;
			int nFPGACh = nWk % 100;
			int nCamNo = GetHtParaCamNo(ii, jj);

			if (0 == nFPGACh) continue;			// ボード未割当

												// カメラ選択設定コマンド登録（Slave指定）
			memset(&cmd, 0x00, sizeof(LINKSW_CMD));
			cmd.prNo = ii + 1;										// ロジックパラランPC no
			cmd.camNo = jj + 1;										// ロジックパラランカメラ no
			cmd.topCamNo = mcls_pStatus->GetGroupTopCamNo(nFPGABoardNo);	// 先頭カメラNo グループ内の入力カメラの先頭
			cmd.bRecv = false;										// 受信フラグクリア
			ToOutMailSender::Create_ToLinksw_ChangeCam(nFPGABoardNo, nFPGACh, nCamNo, -1, &cmd.sendCmd);
			SetLinkswCmd(cmd);
		}
	}
	// 途中参加設定コマンドキューイング（全ロジックパラランPC分）
	for (int ii = 0; ii < NUM_ALL_HTPARA; ii++)
	{
		// 送信直前でセットするため、途中参加フレームは０をセット
		memset(&cmd, 0x00, sizeof(LINKSW_CMD));
		cmd.prNo = ii + 1;												// ロジックパラランPC no
		ToOutMailSender::Create_ToLinksw_JoinReq(2 + ii, 0, &cmd.sendCmd);
		SetLinkswCmd(cmd);
	}
}

//==========================================
// カメラ選択設定待ち処理(タイムアウト発生)
//==========================================
void LogicParaRunFunc::WaitCamset_TimeOut()
{
	if (E_LINKSW_CMD_NOP != m_LinkswCmdKind)
	{
		// ----------------------
		// 機器状態－ロジックパララン機能xx_カメラxx選択結果（異常）
		KizuFunction::SetStatus(mcls_pStatus->GetStatusRead().ParaState[mlst_SendCmd[m_nSendCmdIndex].prNo - 1][mlst_SendCmd[m_nSendCmdIndex].camNo - 1].id, false, false);
		syslog(236, "[カメラ選択設定アンサータイムアウト ロジックパララン%d カメラ%d]", mlst_SendCmd[m_nSendCmdIndex].prNo, mlst_SendCmd[m_nSendCmdIndex].camNo);

		// 次コマンド送信に移行
		m_nSendCmdIndex++;

		// 次のコマンド送信（キュー制御）
		Send_NextCmdSelect();
	}
	else
	{
		LOG(em_ERR), "[%s] 検査開始(途中参加)実施中以外でカメラ設定完了通知を受信", my_sThreadName);
		// コマンドリストクリア
		ClearLinkswCmd();
	}
}

//==========================================
// 途中開始設定待ち処理(タイムアウト発生)
//==========================================
void LogicParaRunFunc::WaitResumeStart_Timeout()
{
	if (E_LINKSW_CMD_NOP != m_LinkswCmdKind)
	{
		// ロジックパララン状態 － セット
		mcls_pStatus->SetHtParaState(mlst_SendCmd[m_nSendCmdIndex].prNo - 1, DIV_BOOL_FALSE);
		// パララン機能システム状態変更
		mcls_pStatus->SetParaSys(mlst_SendCmd[m_nSendCmdIndex].prNo - 1, DIV_SYS_STOP);
		syslog(236, "[途中開始設定アンサータイムアウト ロジックパララン%d]", mlst_SendCmd[m_nSendCmdIndex].prNo);

		// 次コマンド送信に移行
		m_nSendCmdIndex++;

		// 次のコマンド送信（キュー制御）
		Send_NextCmdSelect();
	}
	else
	{
		LOG(em_ERR), "[%s] 検査開始(途中参加)実施中以外で途中参加完了通知を受信", my_sThreadName);
		// コマンドリストクリア
		ClearLinkswCmd();
	}
}

//==========================================
// 途中開始判定アンサー待ち処理(タイムアウト発生)
//------------------------------------------
// int nKind				: 検査開始区分（0：ロジックパララン機能1、1：ロジックパララン機能2）
//==========================================
void LogicParaRunFunc::WaitResumeWait_Timeout(int nKind)
{
	// ロジックパララン状態 － クリア
	mcls_pStatus->SetHtParaState(nKind, DIV_BOOL_FALSE);

	// パララン機能システム状態変更
	mcls_pStatus->SetParaSys(nKind, DIV_SYS_STOP);

	// 途中停止要求
	COMMON_QUE que;
	que.fl.data[0] = nKind;
	ResumeStop(&que);

	// オペレータ機能へ現状態を送信
	Send_SoGamenn_MailStatus();
	OpSendFunc::SendOp_Status();

	syslog(236, "[途中開始判定アンサータイムアウト ロジックパララン%d]", nKind + 1);

}

//==========================================
// カメラ選択設定完了処理
//------------------------------------------
// COMMON_QUE* que	: 起動要因	
//==========================================
void LogicParaRunFunc::WaitCamset_Answer(COMMON_QUE* que)
{
	if (mlst_SendCmd.size() <= m_nSendCmdIndex)
	{
		// TO_LINKSW送信コマンドリストEmpty

		if (EM_DIV_UNTEN::DIV_UNTEN_CAMERA != mcls_pStatus->GetUnten())
		{
			LOG(em_ERR), "[%s] カメラ選択設定アンサー 異常検出(送受信整合性異常)", my_sThreadName);
		}
		else
		{
			LOG(em_INF), "[%s] iPortViewerからのカメラ選択設定による応答", my_sThreadName);
		}

		// コマンドリストクリア
		ClearLinkswCmd();
		return;
	}

	// 設定結果チェック
	if (1 != que->fl.data[4])
	{
		// 設定結果：失敗
		LOG(em_ERR), "[%s] カメラ選択設定アンサー 異常検出(結果異常)", my_sThreadName);

		// 機器状態－ロジックパララン機能xx_カメラxx選択結果（異常）
		KizuFunction::SetStatus(mcls_pStatus->GetStatusRead().ParaState[mlst_SendCmd[m_nSendCmdIndex].prNo - 1][mlst_SendCmd[m_nSendCmdIndex].camNo - 1].id, false);
	}
	else
	{
		// 設定結果：成功

		// 伝文チェック
		// ①スイッチFPGAボードID
		// ②フレームグラバーボードNo
		// ③カメラNo
		// ④Master／Slave設定
		if (mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[0] != que->fl.data[0] ||
			mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[1] != que->fl.data[1] ||
			mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[2] != que->fl.data[2] ||
			mlst_SendCmd[m_nSendCmdIndex].sendCmd.fl.data[3] != que->fl.data[3])
		{
			// 異常応答（送信した伝文と合致しない）

			// 前に送信した応答が遅れてきた？
			if (IsEqualCamsetCMD(que))
			{
				LOG(em_MSG), "[%s] 前のカメラ選択設定のアンサーが遅れて受信された！！", my_sThreadName);
				return;	// 本来の応答が来るはずなので、次コマンドは送信せずに終了
			}
			else
			{
				LOG(em_INF), "[%s] 前回の検査開始、または途中参加での仕掛分が受信された！！", my_sThreadName);
				return;	// 本来の応答が来るはずなので、次コマンドは送信せずに終了
			}
		}
		else
		{
			// 正常受信
			mlst_SendCmd[m_nSendCmdIndex].bRecv = true;

			// 機器状態－ロジックパララン機能 入力カメラ
			int nCamno = GetHtParaCamNo(mlst_SendCmd[m_nSendCmdIndex].prNo - 1, mlst_SendCmd[m_nSendCmdIndex].camNo - 1);
			nCamno = (0 != que->fl.data[2] ? nCamno : que->fl.data[2]);
			KizuFunction::SetStatus(mcls_pStatus->GetStatusRead().ParaCam[mlst_SendCmd[m_nSendCmdIndex].prNo - 1][mlst_SendCmd[m_nSendCmdIndex].camNo - 1].id, nCamno, false);
		}
	}

	// 次コマンド送信に移行
	m_nSendCmdIndex++;

	// 次のコマンド送信（キュー制御）
	Send_NextCmdSelect();
}

//==========================================
// 途中参加設定完了処理
//------------------------------------------
// COMMON_QUE* que				   : 起動要因	
// CoilManager const *pCoilManager : コイル情報
//==========================================
void LogicParaRunFunc::WaitLinkswJoin_Answer(COMMON_QUE* que, CoilManager const *pCoilManager)
{
	if (DIV_UNTEN_CAMERA != mcls_pStatus->GetUnten())
	{
		// 「カメラ校正」モード以外

		COMMON_QUE* pQue = new COMMON_QUE;
		memcpy(pQue, que, sizeof(COMMON_QUE));

		// L[0]  表裏区分 （0：表、1：裏、2：ロジックパララン１、3：ロジックパララン2）
		// L[1]  途中参加フレームNo （0オリジン）
		int nTorB = pQue->fl.data[0];
		int nJoinFrameNo = pQue->fl.data[1];
		if (2 == nTorB || 3 == nTorB)
		{
			// 途中開始要求
			Send_HtResume_Start(nTorB - 2, pCoilManager, nJoinFrameNo);
		}
		else
		{
			LOG(em_INF), "[%s] 途中参加設定完了通知：無効な表裏区分", my_sThreadName);
		}

		delete pQue;

		// 次コマンド送信に移行
		m_nSendCmdIndex++;

		// 次のコマンド送信（キュー制御）
		Send_NextCmdSelect();
	}
	else
	{
		// 「カメラ校正」モード時

		LOG(em_ERR), "[%s] カメラ校正モード時は途中参加設定完了通知無効", my_sThreadName);
		// オペレータ機能へ稼動状態を送信
		Send_SoGamenn_MailStatus();
	}
}
