//=====================================================================
// 疵検機器 I/Fクラス
//	【全ライン】
//		・疵検の共通支援、共通タスクとのI/Fを提供する
//=====================================================================

#pragma once

#include "..\..\Include\LineCommon.h"						// 疵検インクルード
#include "..\..\Library\KizuLib\KizuLib.h"					// 疵検ライブラリ

class ToOutMailSender
{
public:
	ToOutMailSender(void);
	virtual ~ToOutMailSender(void);


	//// PCマスター
	static void Send_KsMaster_TimeSet();								// PCマスター状態管理へ 時刻設定通知 [←Dlg]
	static void Send_KsMaster_PcChange();								// マスターPC状態管理にPC名が変更された事を通知


	//// 対カメラコントローラ
	static void Send_ToCamera_AGC(int camerano, int mode, int ro);		// カメラコントローラーに AGC設定 を通知 [←HY_RECV]
	static void Send_ToCamera_Param(int scode);							// カメラコントローラー へ パラメータ変更を通知 [←ParamManager]
	static void Send_ToCamera_Palse(bool onoff);						// カメラコントローラーに パルス出力 を通知 [←PioManager, IPortViewer]
	static void Send_ToCamera_Start(EM_DIV_PLG em_plg);					// カメラコントローラーに 検査開始 を通知 [←Dlg, IPortViewer]
	static void Send_ToCamera_CoilChange(int TorB, int scode);			// カメラコントローラーに 該当コイルに最適な設定を通知する
	static void Send_ToCamera_AgcVal();									// カメラコントローラーに 現露光時間問い合わせ通知

	// カメラリンクスイッチ通信
	static void Send_ToLinksw_Param(int scode);										// カメラリンクスイッチ通信 へ パラメータ変更通知 [←ParamManager]
	static void Send_ToLinksw_Start();												// カメラリンクスイッチ通信 へ 検査開始通知 [←MainInstance]
	static void Send_ToLinksw_ChangeCam(int SwitchID, int BoardNo, int CamID, int nKind=1);	// カメラリンクスイッチ通信 へ カメラ選択設定 [←？]
	static void Send_ToLinksw_JoinReq(int ou, int JoinFrameNo);						// カメラリンクスイッチ通信 へ 途中参加設定 [←MainInstance]
	static void Send_ToLinksw_ChangeLinkSpeed(int nBoardID, int nSpeed);			// カメラリンクスイッチ通信 へ シリアル通信速度変更要求 [←CamLinkSpeedManager]
	static void Create_ToLinksw_ChangeCam(int SwitchID, int BoardNo, int CamNo, int nKind, COMMON_QUE *que);	// カメラリンクスイッチ通信 へ カメラ選択設定データ生成
	static void Create_ToLinksw_JoinReq(int ou, int JoinFrameNo, COMMON_QUE *que);	// カメラリンクスイッチ通信 へ 途中参加設定データ生成
	static void Send_ToLinksw_ChangeCam(COMMON_QUE *que);							// カメラリンクスイッチ通信 へ カメラ選択設定 [←？]
	static void Send_ToLinksw_JoinReq(COMMON_QUE *que);								// カメラリンクスイッチ通信 へ 途中参加設定 [←MainInstance]


	//// 対照明装置
#ifndef KT_LEDMGR
	static void Send_ToLights_Param(int scode);							// 照明装置に パラメータ変更を通知 [←ParamManager]
	static void Send_ToLights_CoilChange(int TorB, int scode);			// 照明装置に 該当コイルに最適な設定を通知
#else
	static void Send_KtLedMgr_Param(int scode);										// 照明装置に パラメータ変更を通知 [←ParamManager]
	static void Send_KtLedMgr_CoilChange(int TorB, int scode, int ch=0);			// 照明装置に 該当コイルに最適な設定を通知
	static void Send_KtLedMgr_ValChange(int TorB, int valM, int valH, int ch=0);	// LED照明装置に 最適な輝度設定を通知する [←IportViewer]
	static void Send_KtLedMgr_OnOff(int TorB, BOOL onoff, int ch=0);				// LED照明装置通信に 点灯・消灯指示 を通知 [←MainInstance, ?] (2CGLのみ)
#endif
};
