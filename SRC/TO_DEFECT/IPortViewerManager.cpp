#include "StdAfx.h"
#include "IPortViewerManager.h"
#include "ToLinkswUtil.h"

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
IPortViewerManager::IPortViewerManager(void) :
ThreadManager("iPortViewerMgr")
{

}

//------------------------------------------
// デストラクタ
//------------------------------------------
IPortViewerManager::~IPortViewerManager(void)
{

}

//------------------------------------------
// ストップ
//------------------------------------------
void IPortViewerManager::Stop()
{
	// 自スレッドに終了要求
	COMMON_QUE que;
	que.nEventNo = FACT_KS_END;
	que.nLineNo = getlineid();
	send_mail(EPC_MAIL_CAMCONT, ".", &que);
}


//------------------------------------------
// スレッドメインループ
//------------------------------------------
int IPortViewerManager::ThreadMainLoop()
{
	COMMON_QUE que;			// 受信構造体


	////////////////////////////////////
	// 事前準備
	////////////////////////////////////
	// メールスロットクラス
	MailSlotManager		cls_Mail;								
	bool bRetc = cls_Mail.Create(EPC_MAIL_CAMCONT);
	_ASSERT(bRetc);



	////////////////////////////////////
	// 通常のメインスレッド処理
	////////////////////////////////////
	//// 起動要因取り込み ループ開始
	LOG(em_MSG), "[%s] スレッド開始 [0x%X]", my_sThreadName, GetTid());
	while(true) {
		cls_Mail.Recv(&que, SIZE_MSL);		// 起動要因取り込み

		switch(que.nEventNo) {

//-----------------------------------------------------------------------------------------------
		case FACT_EPC_IPORTVIEWER:							// iPortViewerからのメールスロット (←iPortViewer)
			LOG(em_MSG), "[MAIL] iPortViewerから通知 [cmd=%d]", que.nLineNo);

			// 処理分け
			switch( que.nLineNo ) {
			case 0:		// 表校正
				ToOutMailSender::Send_ToCamera_Palse(false);
				ToOutMailSender::Send_ToCamera_Start(DIV_PLG_TOP);
				Sleep(100);
				ToOutMailSender::Send_ToCamera_Palse(true);
				break;
			case 1:		// 裏校正
				ToOutMailSender::Send_ToCamera_Palse(false);
				ToOutMailSender::Send_ToCamera_Start(DIV_PLG_BOT);
				Sleep(100);
				ToOutMailSender::Send_ToCamera_Palse(true);
				break;
			case 2:		// テストパルス
				ToOutMailSender::Send_ToCamera_Palse(false);
				ToOutMailSender::Send_ToCamera_Start(DIV_PLG_TEST);
				Sleep(100);
				ToOutMailSender::Send_ToCamera_Palse(true);
				break;
			case 3:		// オンライン
				ToOutMailSender::Send_ToCamera_Palse(false);
				ToOutMailSender::Send_ToCamera_Start(DIV_PLG_LINE);
				Sleep(100);
				ToOutMailSender::Send_ToCamera_Palse(true);
				break;
			case 100:	// 停止
				ToOutMailSender::Send_ToCamera_Palse(false);
				break;

			
			case 1000:	// LED照明光量設定
				SetLightVal(que.fc.data[0], que.fc.data[1], que.fc.data[2]);
				break;
			case 1001:	// LED照明onoff設定
				SetLightOnOff(que.fc.data[0], que.fc.data[1], que.fc.data[2]);
				break;
			case 1003:	// 露光時間設定
				if(true){
					int nWk;
					memcpy(&nWk, &que.fc.data[1], sizeof(int));
					SetAGC(que.fc.data[0], nWk);
				}
				break;

			//// カメラリンクスイッチ ------------------------------------------->>>
			case 1002:	// カメラ切替設定
				SetCameraChange(que.fc.data[0], que.fc.data[1]);
				break;

			// <<<------------------------------------------------------
			}
			break;

//-----------------------------------------------------------------------------------------------
		case FACT_KS_END:									// 終了要求
			// 各プロセス終了処理
			LOG(em_MSG), "[%s] スレッド終了 [0x%X]", my_sThreadName, GetTid());
			return 0;

		default:
			LOG(em_MSG), "[MAIL] イベントNo無し [IPortViewer EventNo=%d]", que.nEventNo);
			syslog(SYSNO_MAILNO_ERR, "[IPortViewer EventNo=%d]", que.nEventNo);
			break;
		}
	}

	return 0;
}

//------------------------------------------
// 色差用LED照明光量設定
// int camset	カメラセット (1オリジン)
// int index	複眼：0固定(未使用) / 色差：色 (0:R  1:G  2:B)
// int val      輝度 (0～255)
//------------------------------------------
void IPortViewerManager::SetLightVal(int camset, int index, BYTE val)
{
	int ou;		// 表裏の判断は色差OR複眼で異なる
	int ch = index;


#ifdef HT_IS_COLOR		// 色差

	ou = NUM_TorB( (camset-1) );

	LOG(em_MSG), "[MAIL] iPortViewerから通知 (照明光量) [camset=%d] [色=%d] [値=%d]", camset, index, val);

	ToOutMailSender::Send_KtLedMgr_ValChange(ou, val, 255, ch+1);

#else					// 複眼

	ou = (camset-1) / (NUM_CAMPAIR / NUM_MEN);

	LOG(em_MSG), "[MAIL] iPortViewerから通知 (照明光量) [camset=%d] [値=%d]", camset, val);

	ToOutMailSender::Send_KtLedMgr_ValChange(ou, val, 255, 0);	// 全色消灯

#endif


	// DBの登録はなし
	//CString sql;
	//// 列名
	//char cNameM[2][3][64] = {{"照明照度_表", "照明照度2_表", "照明照度3_表"}, {"照明照度_裏", "照明照度2_裏", "照明照度3_裏"} };
	//char cNameH[2][3][64] = {{"照明照度補正値_表", "照明照度補正値2_表", "照明照度補正値3_表"}, {"照明照度補正値_裏", "照明照度補正値2_裏", "照明照度補正値3_裏"} };


	////======================================================
	////// ODBC接続クラス インスタンス生成
	//OdbcBase clsDB;
	//if( ! clsDB.Connect(TASKINI_NAME)) {
	//	// データベース接続エラー
	//	LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
	//	syslog(SYSNO_DB_CONNECT_ERR, "");
	//	return ;
	//}


	////// SQL生成
	//sql = "UPDATE ";
	//sql += DB_PARAM_SCODE;
	//sql += " SET ";	

	//// Where
	//sql += " WHERE ";
	//CreateSQL::sqlColumnUpdate(&sql, "camangle", jj+1);			// カメラ角度(1オリジン) 
	//CreateSQL::sqlColumnTrim(&sql);		// 最後のカンマを取り除く
	//

	////// 実行
	//if( ! clsDB.ExecuteDirect(sql) ) {
	//	LOG(em_ERR), "[%s] 射影パラメータをアップデート失敗 [%s] [%s]", my_sThreadName, DB_PARAM_CAMSET_ANGLE, sql);
	//	syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[射影パラメータをアップデート失敗:%s]", DB_PARAM_CAMSET_ANGLE);
	//	return false;
	//}
}


//------------------------------------------
// 式差用LED照明点灯消灯設定
// int cam		色差の場合カメラセット (1オリジン)/複眼の場合カメラペア(1オリジン)
// int index	複眼：0固定(未使用) / 色差：色 (0:R  1:G  2:B)
// int onoff	1:on 0:off
//------------------------------------------
void IPortViewerManager::SetLightOnOff(int cam, int index, int onoff)
{
	int ou;		// 表裏の判断は色差OR複眼で異なる


	int ch = index;

#ifdef HT_IS_COLOR

	ou = NUM_TorB( (cam-1) );

	LOG(em_MSG), "[MAIL] iPortViewerから通知 (照明%s) [camset=%d] [色=%d] [値=%d]", (1==onoff?"ON":"OFF"), cam, index, onoff);

	ToOutMailSender::Send_KtLedMgr_OnOff(ou, (BOOL)onoff, ch+1);

#else

	ou = (cam-1) / (NUM_CAMPAIR / NUM_MEN);

	LOG(em_MSG), "[MAIL] iPortViewerから通知 (照明%s) [campair=%d] [値=%d]", (1==onoff?"ON":"OFF"), cam, onoff);

	ToOutMailSender::Send_KtLedMgr_OnOff(ou, (BOOL)onoff, 0);

#endif




}

//------------------------------------------
// カメラ切替設定
// int hantei	各判定PCの固有番号 (1オリジン)
// int cam		色差の場合カメラセット (1オリジン)/複眼の場合カメラペア(1オリジン)
//------------------------------------------
void IPortViewerManager::SetCameraChange(int hantei, int cam)
{
#ifdef HT_IS_COLOR		// 色差

	CString			sWk;

	LOG(em_MSG), "[MAIL] iPortViewerから通知 (カメラ切替) [hantei=%d] [camset=%d]", hantei, cam);

	// フレームグラバーボードNo
	int nFPGABoardNo= 0;
	int nFPGACh		= 0;
	ToLinkswUtil::GetFPGAInfo(hantei, 1, &nFPGABoardNo, &nFPGACh);

	// Board 0 - 7
	if (0 > nFPGABoardNo || (NUM_CAMLINKSW * MAX_CAMLINKSW_FPGA) <= nFPGABoardNo)
	{
		LOG(em_ERR), "[%s] カメラ調整切替 フレームグラバーボードNo 異常 [%s]", my_sThreadName, sWk);
		return;
	}
	// Ch 1 - 15
	if (0 >= nFPGACh || MAX_CAMLINKSW_OUTPUT < nFPGACh)
	{
		LOG(em_ERR), "[%s] カメラ調整切替 フレームグラバーボードCh 異常 [%s]", my_sThreadName, sWk);
		return;
	}

	// カメラNo
	ToLinkswUtil::TO_LINKSW_OUTPUT info;
	memset(&info, 0x00, sizeof(info));
	ToLinkswUtil::GetDeterminedHanteiPC(mcls_pStatus, &info);

	// 対象となる内部のFPGA番号を取得
	int nFPGABoard[MAX_CAMLINKSW_FPGA];
	memset(&nFPGABoard, 0x00, sizeof(nFPGABoard));
	nFPGABoard[0] = (nFPGABoardNo / MAX_CAMLINKSW_FPGA) * MAX_CAMLINKSW_FPGA;
	nFPGABoard[1] = nFPGABoard[0] + 1;

	LOG(em_MSG), "[%s] カメラ調整切替 [FPGA:%d/%d]", my_sThreadName, nFPGABoard[0], nFPGABoard[1]);

	int nCamNo = -1;
	for (int ii = 0; ii < MAX_CAMLINKSW_FPGA; ii++)
	{
		int nWk = ToLinkswUtil::GetTargetCamNo(&info, nFPGABoard[ii], cam);
		if (-1 != nWk)
		{
			// 接続しているカメラを選択した
			nCamNo = nWk;
			break;
		}
	}
	if (-1 != nCamNo)
	{
		LOG(em_MSG), "[%s] カメラ調整切替 [指定カメラNo:%d 出力カメラNo:%d]", my_sThreadName, cam, nCamNo);
	}
	else
	{
		LOG(em_ERR), "[%s] カメラ調整切替 カメラNo 異常 [指定カメラNo:%d]", my_sThreadName, cam);

		//// カメラリンクスイッチ通信 へ カメラ選択設定（出力停止）
		ToOutMailSender::Send_ToLinksw_ChangeCam(nFPGABoardNo, nFPGACh, 0);
		return;
	}

	//// カメラリンクスイッチ通信 へ カメラ選択設定
	ToOutMailSender::Send_ToLinksw_ChangeCam(nFPGABoardNo, nFPGACh, nCamNo);

#else					// 複眼

	CString			sWk;

	LOG(em_MSG), "[MAIL] iPortViewerから通知 (カメラ切替) [hantei=%d] [campair=%d]", hantei, cam);

	// フレームグラバーボードNo_正
	sWk.Format("CAMLINKSW_PCID%d_1", hantei);
	int nBoardNo_S = GetPrivateProfileInt(TO_DEFECT, sWk, 0, TASKINI_NAME);
	
	// フレームグラバーボードNo_乱
	sWk.Format("CAMLINKSW_PCID%d_2", hantei);
	int nBoardNo_R = GetPrivateProfileInt(TO_DEFECT, sWk, 0, TASKINI_NAME);

	if( 0 >= nBoardNo_S || MAX_CAMLINKSW_OUTPUT*MAX_CAMLINKSW_FPGA < nBoardNo_S ||
		0 >= nBoardNo_R || MAX_CAMLINKSW_OUTPUT*MAX_CAMLINKSW_FPGA < nBoardNo_R)
	{
		LOG(em_ERR), "[%s] カメラ調整切替 フレームグラバーボードNo 異常 [%s]", my_sThreadName, sWk);
		return;
	}

	// カメラNo
	int nCamNo_S = ((cam - 1) * MAX_PHYSICAL_CAMANGLE) + 1;
	int nCamNo_R = ((cam - 1) * MAX_PHYSICAL_CAMANGLE) + 2;
	if( 0 >= nCamNo_S || NUM_CAMERA < nCamNo_S || 0 >= nCamNo_R || NUM_CAMERA < nCamNo_R )
	{
		LOG(em_ERR), "[%s] カメラ調整切替 カメラNo 異常 [正：%d/乱：%d]", my_sThreadName, nCamNo_S, nCamNo_R);
		return;
	}

	//// カメラリンクスイッチ通信 へ カメラ選択設定
	ToOutMailSender::Send_ToLinksw_ChangeCam( nBoardNo_S / MAX_CAMLINKSW_OUTPUT, nBoardNo_S, nCamNo_S);
	ToOutMailSender::Send_ToLinksw_ChangeCam( nBoardNo_R / MAX_CAMLINKSW_OUTPUT, nBoardNo_R, nCamNo_R);
#endif

}

//------------------------------------------
// AGC設定
// int camset	カメラセット (1オリジン)
// int ro		露光時間	 (絶対値)
//------------------------------------------
void IPortViewerManager::SetAGC(int camset, int ro)
{
	LOG(em_MSG), "[MAIL] iPortViewerから通知 (露光時間変更) [camset=%d] [ExpTime=%d]", camset, ro);

	ToOutMailSender::Send_ToCamera_AGC(camset-1, 1, ro);

	
}