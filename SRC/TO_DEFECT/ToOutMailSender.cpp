#include "StdAfx.h"
#include "ToOutMailSender.h"

ToOutMailSender::ToOutMailSender(void)
{
}

ToOutMailSender::~ToOutMailSender(void)
{
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PCマスター

//------------------------------------------
// PCマスター状態管理へ 時刻設定通知
//------------------------------------------
void ToOutMailSender::Send_KsMaster_TimeSet()
{
    SYSTEMTIME time;
    GetLocalTime(&time);	

	COMMON_QUE que;
	que.nEventNo = FACT_KS_MASTER_01;
	que.nLineNo = getlineid();
	memcpy(&que.timeset_que.systemtime, &time, sizeof(time));  

	// 送信
	int iRet = send_mail(KS_MASTER, MASTER_PC_NAME, &que);
}

//------------------------------------------
// マスターPC状態管理にPC名が変更された事を通知
//------------------------------------------
void ToOutMailSender::Send_KsMaster_PcChange()
{
	// 全PCの全タスクへ、各機能におけるPC名称が変わった事を通知する。
	COMMON_QUE que;
	que.nEventNo = FACT_KS_PCNAME_CHANGE; 
	que.nLineNo = getlineid();
	send_mail(KS_MASTER, ".", &que);
	Sleep(50);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// カメラコントローラー

//------------------------------------------
// カメラコントローラーに AGC設定 を通知
// int camerano カメラ番号 (0～15)
// int mode 0:相対値 1:絶対値 
// int ro 露光時間 相対値 又は 絶対値 (-1:下げ 0:そのまま 1:上げ)
//------------------------------------------
void ToOutMailSender::Send_ToCamera_AGC(int camerano, int mode, int ro)
{
	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_04;
	que.nLineNo = getlineid();
	que.fl.data[0] = camerano;
	que.fl.data[1] = mode;
	que.fl.data[2] = ro;

	// 送信
	int iRet = send_mail(TO_CAMERA, ".", &que);
// 異常になるとものすごい量のエラーが出るのでヤメ
//	if( 0 != iRet ) syslog(SYSNO_MAIL_SNED_ERR, "[カメラコントローラーへAGC設定 err_code=%d]", iRet);
}

//------------------------------------------
// カメラコントローラーに パラメータ変更を通知
// int scode 検査表面状態No
//------------------------------------------
void ToOutMailSender::Send_ToCamera_Param(int scode)
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_TO_CAMERA_01;
	que.nLineNo = getlineid();
	que.fl.data[0] = scode;
	// 送信
	int iRet = send_mail(TO_CAMERA, ".", &que);
	if( 0 != iRet ) syslog(SYSNO_MAIL_SNED_ERR, "[カメラコントローラーへパラメータ変更要求 err_code=%d]", iRet);
}
//------------------------------------------
// カメラコントローラーに パルス出力 を通知
// bool onoff ture:on false:off
//------------------------------------------
void ToOutMailSender::Send_ToCamera_Palse(bool onoff)
{
	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_03;
	que.nLineNo = getlineid();
	que.fl.data[0] = onoff ? 0 : 1;

	// 送信
	int iRet = send_mail(TO_CAMERA, ".", &que);
	if( 0 != iRet ) syslog(SYSNO_MAIL_SNED_ERR, "[カメラコントローラーへパルス出力 err_code=%d]", iRet);
}


//------------------------------------------
// カメラコントローラーに 検査開始 を通知
// EM_DIV_PLG em_plg PLG区分
//------------------------------------------
void ToOutMailSender::Send_ToCamera_Start(EM_DIV_PLG em_plg)
{
	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_02;
	que.nLineNo = getlineid();
	que.fl.data[0] = em_plg;

	// 送信
	int iRet = send_mail(TO_CAMERA, ".", &que);
	if( 0 != iRet ) syslog(SYSNO_MAIL_SNED_ERR, "[カメラコントローラーへ検査開始 err_code=%d]", iRet);
}

//------------------------------------------
// カメラコントローラーに 該当コイルに最適な設定を通知する
// int TorB 表:0 裏:1
// int scode 検査表面状態 (1オリジン)
//------------------------------------------
void ToOutMailSender::Send_ToCamera_CoilChange(int TorB, int scode)
{
	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_05;
	que.nLineNo = getlineid();
	que.fl.data[0] = scode;
	que.fl.data[1] = TorB;

	// 送信
	int iRet = send_mail(TO_CAMERA, ".", &que);
	if( 0 != iRet ) syslog(SYSNO_MAIL_SNED_ERR, "[カメラコントローラーへコイル切替 err_code=%d]", iRet);
}

//------------------------------------------
// カメラコントローラーに 現露光時間問い合わせ通知
//------------------------------------------
void ToOutMailSender::Send_ToCamera_AgcVal()
{
	COMMON_QUE que;
	que.nEventNo = FACT_TO_CAMERA_06;
	que.nLineNo = getlineid();

	// 送信
	send_mail(TO_CAMERA, ".", &que);
}

//------------------------------------------
// カメラリンクスイッチ通信 へ パラメータ変更通知
// int scode 検査表面状態No (1オリジン)
//------------------------------------------
void ToOutMailSender::Send_ToLinksw_Param(int scode)
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));

	que.nEventNo		= FACT_TO_LINKSW_01;
	que.nLineNo			= getlineid();
	que.fl.data[0]		= scode;

	// 送信
	int nRetc = send_mail(TO_LINKSW, ".", &que);
	if( 0 != nRetc ) syslog(SYSNO_MAIL_SNED_ERR, "[カメラリンクスイッチ通信へのパラメータ変更通知 err_code=%d]", nRetc);
}

//------------------------------------------
// カメラリンクスイッチ通信 へ 検査開始設定
//------------------------------------------
void ToOutMailSender::Send_ToLinksw_Start()
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));

	que.nEventNo		= FACT_TO_LINKSW_02;
	que.nLineNo			= getlineid();

	// 送信
	int nRetc = send_mail(TO_LINKSW, ".", &que);
	if( 0 != nRetc ) syslog(SYSNO_MAIL_SNED_ERR, "[カメラリンクスイッチ通信への検査開始設定 err_code=%d]", nRetc);
}

//------------------------------------------
// カメラリンクスイッチ通信 へ カメラ選択設定
// int SwitchID スイッチFPGAボードID (0オリジン)
// int BoardNo フレームグラバーボードNo (1～15)
// int CamNo カメラNo (1～8、0:出力停止)
// int nKind Master／Slave設定（1：Master、-1：Slave）
//------------------------------------------
void ToOutMailSender::Send_ToLinksw_ChangeCam(int SwitchID, int BoardNo, int CamNo, int nKind)
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));

	que.nEventNo		= FACT_TO_LINKSW_03;
	que.nLineNo			= getlineid();
	que.fl.data[0]		= SwitchID;
	que.fl.data[1]		= BoardNo;
	que.fl.data[2]		= CamNo;
	que.fl.data[3]		= nKind;

	// 送信
	int nRetc = send_mail(TO_LINKSW, ".", &que);
	if( 0 != nRetc ) syslog(SYSNO_MAIL_SNED_ERR, "[カメラリンクスイッチ通信へのカメラ選択設定 err_code=%d]", nRetc);
}
//------------------------------------------
// カメラリンクスイッチ通信 へ カメラ選択設定
// COMMON_QUE *que メールスロット通信用構造体
//------------------------------------------
void ToOutMailSender::Send_ToLinksw_ChangeCam(COMMON_QUE *que)
{
	// 送信
	int nRetc = send_mail(TO_LINKSW, ".", que);
	if (0 != nRetc) syslog(SYSNO_MAIL_SNED_ERR, "[カメラリンクスイッチ通信へのカメラ選択設定 err_code=%d]", nRetc);
}

//------------------------------------------
// カメラ選択設定データ生成
// int SwitchID スイッチFPGAボードID (0オリジン)
// int BoardNo フレームグラバーボードNo (1～15)
// int CamNo カメラNo (1～8、0:出力停止)
// int nKind Master／Slave設定（1：Master、-1：Slave）
// COMMON_QUE *que メールスロット通信用構造体
//------------------------------------------
void ToOutMailSender::Create_ToLinksw_ChangeCam(int SwitchID, int BoardNo, int CamNo, int nKind, COMMON_QUE *que)
{
	que->nEventNo = FACT_TO_LINKSW_03;
	que->nLineNo = getlineid();
	que->fl.data[0] = SwitchID;
	que->fl.data[1] = BoardNo;
	que->fl.data[2] = CamNo;
	que->fl.data[3] = nKind;
}

//------------------------------------------
// カメラリンクスイッチ通信 へ 途中参加設定通知
// int ou 表裏区分(0:表 1:裏)
// int JoinFrameNo 途中参加フレームNo(0オリジン)
//------------------------------------------
void ToOutMailSender::Send_ToLinksw_JoinReq(int ou, int JoinFrameNo)
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));

	que.nEventNo		= FACT_TO_LINKSW_04;
	que.nLineNo			= getlineid();
	que.fl.data[0]		= ou;
	que.fl.data[1]		= JoinFrameNo;

	// 送信
	int nRetc = send_mail(TO_LINKSW, ".", &que);
	if( 0 != nRetc ) syslog(SYSNO_MAIL_SNED_ERR, "[カメラリンクスイッチ通信への途中参加設定通知 err_code=%d]", nRetc);
}

//------------------------------------------
// カメラリンクスイッチ通信 へ 途中参加設定通知
// COMMON_QUE *que メールスロット通信用構造体
//------------------------------------------
void ToOutMailSender::Send_ToLinksw_JoinReq(COMMON_QUE *que)
{
	// 送信
	int nRetc = send_mail(TO_LINKSW, ".", que);
	if (0 != nRetc) syslog(SYSNO_MAIL_SNED_ERR, "[カメラリンクスイッチ通信への途中参加設定通知 err_code=%d]", nRetc);
}

//------------------------------------------
// カメラリンクスイッチ通信 へ 途中参加設定通知
// int ou 表裏区分(0:表 1:裏 2：ロジックパララン１ 3：ロジックパララン2)
// int JoinFrameNo 途中参加フレームNo(0オリジン)
// COMMON_QUE *que メールスロット通信用構造体
//------------------------------------------
void ToOutMailSender::Create_ToLinksw_JoinReq(int ou, int JoinFrameNo, COMMON_QUE *que)
{
	que->nEventNo		= FACT_TO_LINKSW_04;
	que->nLineNo		= getlineid();
	que->fl.data[0]		= ou;
	que->fl.data[1]		= JoinFrameNo;
}

//------------------------------------------
// カメラリンクスイッチ通信 へ シリアル通信速度変更要求
// int nBoardID	フレームグラバーボードID(1オリジン)
// int nSpeed	シリアル通信速度[bps]
//------------------------------------------
void ToOutMailSender::Send_ToLinksw_ChangeLinkSpeed(int nBoardID, int nSpeed)
{
	COMMON_QUE que;

	que.nEventNo	= FACT_TO_LINKSW_06;
	que.nLineNo		= getlineid();
	que.fl.data[0]	= nBoardID;
	que.fl.data[1]	= nSpeed;
	
	// 送信
	int nRetc = send_mail(TO_LINKSW, ".", &que);
	if( 0 != nRetc ) syslog(SYSNO_MAIL_SNED_ERR, "[カメラリンクスイッチ通信へのシリアル通信速度変更通知 err_code=%d]", nRetc);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 照明装置

#ifndef KT_LEDMGR
//------------------------------------------
// メタハラ照明装置に パラメータ変更を通知
// int scode 検査表面状態No
//------------------------------------------
void ToOutMailSender::Send_ToLights_Param(int scode)
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_TO_LIGHTS_01;
	que.nLineNo = getlineid();
	que.fl.data[0] = scode;
	// 送信
	int iRet = send_mail(TO_LIGHTS, ".", &que);
	if( 0 != iRet ) syslog(SYSNO_MAIL_SNED_ERR, "[メタハラ照明装置へパラメータ変更要求 err_code=%d]", iRet);
}

//------------------------------------------
// メタハラ照明装置に 該当コイルに最適な設定を通知する
// int TorB 表:0 裏:1
// int scode 検査表面状態 (1オリジン)
//------------------------------------------
void ToOutMailSender::Send_ToLights_CoilChange(int TorB, int scode)
{
	COMMON_QUE que;
	que.nEventNo = FACT_TO_LIGHTS_02;
	que.nLineNo = getlineid();
	que.fl.data[0] = scode;
	que.fl.data[1] = TorB;

	// 送信
	int iRet = send_mail(TO_LIGHTS, ".", &que);
	if( 0 != iRet ) syslog(SYSNO_MAIL_SNED_ERR, "[メタハラ照明装置へコイル切替通知 err_code=%d]", iRet);
}

#else

//------------------------------------------
// LED照明装置に パラメータ変更を通知
// int scode 検査表面状態No
//------------------------------------------
void ToOutMailSender::Send_KtLedMgr_Param(int scode)
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_KT_LEDMGR_01;
	que.nLineNo = getlineid();
	que.fl.data[0] = scode;
	// 送信
	int iRet = send_mail(KT_LEDMGR, ".", &que);
	if( 0 != iRet ) syslog(SYSNO_MAIL_SNED_ERR, "[LED照明装置へパラメータ変更要求 err_code=%d]", iRet);
}

//------------------------------------------
// LED照明装置に 該当コイルに最適な設定を通知する
// int TorB 表:0 裏:1
// int scode 検査表面状態 (1オリジン)
// int ch  CH(0:全部  1～:対象CHのみ)
//------------------------------------------
void ToOutMailSender::Send_KtLedMgr_CoilChange(int TorB, int scode, int ch)
{
	COMMON_QUE que;
	que.nEventNo = FACT_KT_LEDMGR_02;
	que.nLineNo = getlineid();
	que.fl.data[0] = TorB;			// 引数が逆なので注意
	que.fl.data[1] = ch;
	que.fl.data[2] = scode;

	// 送信
	int iRet = send_mail(KT_LEDMGR, ".", &que);
	if( 0 != iRet ) syslog(SYSNO_MAIL_SNED_ERR, "[LED照明装置へコイル切替通知 err_code=%d]", iRet);
}
//------------------------------------------
// LED照明装置に 最適な輝度設定を通知する
// int TorB 表:0 裏:1
// int valM 光量値 (全体調光データ  0～255)
// int valH 光量値 (配光調整データ  0～255)
// int ch  CH(0:全部  1～:対象CHのみ)
//------------------------------------------
void ToOutMailSender::Send_KtLedMgr_ValChange(int TorB, int valM, int valH, int ch)
{
	COMMON_QUE que;
	que.nEventNo = FACT_KT_LEDMGR_03;
	que.nLineNo = getlineid();
	que.fl.data[0] = TorB;		
	que.fl.data[1] = ch;
	que.fl.data[2] = valM;
	que.fl.data[3] = valH;

	// 送信
	int iRet = send_mail(KT_LEDMGR, ".", &que);
	if( 0 != iRet ) syslog(SYSNO_MAIL_SNED_ERR, "[LED照明装置へ輝度設定 err_code=%d]", iRet);
}

//------------------------------------------
// LED照明装置通信へ 点灯・消灯指示
// int TorB 表:0 裏:1
// bool onoff 点灯消灯区分
// int ch  CH(0:全部  1～:対象CHのみ)
// 戻り値 復帰情報
//------------------------------------------
void ToOutMailSender::Send_KtLedMgr_OnOff(int TorB, BOOL onoff, int ch)
{
	COMMON_QUE que;
	que.nEventNo = FACT_KT_LEDMGR_04;
	que.nLineNo = getlineid();
	que.fl.data[0] = TorB;
	que.fl.data[1] = ch;
	que.fl.data[2] = onoff;
	// 送信
	int iRet = send_mail(KT_LEDMGR, ".", &que);
  #ifndef LOCAL
	if( 0 != iRet ) syslog(SYSNO_MAIL_SNED_ERR, "[LED照明装置へ点灯・消灯指示 err_code=%d]", iRet);
  #endif
}
#endif