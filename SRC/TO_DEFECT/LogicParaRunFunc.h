// *********************************************************************************
//	ロジックパララン関連のモジュール群
//	[メモ]
//		各インスタンスは、staticで .cppに持たす
// *********************************************************************************
#pragma once

// 標準ライブラリ
#include <vector>

// 半固定
#include "..\..\Include\LineCommon.h"							// インクルード

// ローカル
#include "MainConst.h"
#include "ToLinkswUtil.h"
#include "StatusManager.h"
#include "PlgManager.h"	

using namespace KizuLib;

class LogicParaRunFunc
{
public:
	LogicParaRunFunc(LogFileManager* pCls1, StatusManager* pCls2, PlgManager* pCls3);
	~LogicParaRunFunc();

public:
	// リンクスイッチコマンド通信情報
	struct LINKSW_CMD {
		int					prNo;								// パララン機能No（1：ロジックパララン機能1、2：ロジックパララン機能2）
		int					camNo;								// 入力カメラNo  （1：カメラ１、2：カメラ２）
		int					topCamNo;							// 先頭カメラNo
		COMMON_QUE			sendCmd;							// TO_LINKSW送信 コマンド
		bool				bRecv;								// 受信フラグ
	};

	// リンクスイッチコマンド通信状態
	enum ENUM_LINKSW_CMD {
		E_LINKSW_CMD_NOP = -1,									// 初期状態（開始処理無し）
		E_LINKSW_CMD_START,										// 検査開始
		E_LINKSW_CMD_RESUME_START,								// 途中参加
	};

	// リンクスイッチコマンド通信状態
	static CString GetLinkswCmdState(ENUM_LINKSW_CMD em)
	{
		CString wk;
		if (E_LINKSW_CMD_NOP == em)					{ wk = "初期状態"; }
		else if (E_LINKSW_CMD_START == em)			{ wk = "検査開始中"; }
		else if (E_LINKSW_CMD_RESUME_START == em)	{ wk = "途中参加中"; }
		else                                        { wk.Format("ステータス異常 <%d>", em); }
		return wk;
	}

	//// タイマー用定数
#ifndef LOCAL
	static const int PARAM_TIMEOUT_CAMSET		= 120000;		// カメラ選択設定アンサー待ちタイマー
	static const int PARAM_TIMEOUT_RESUME_START = 60000;		// 途中参加 カメラリンクアンサー待ちタイマー
	static const int PARAM_TIMEOUT_RESUME_WAIT	= 60000;		// 途中開始 判定アンサー待ちタイマー
#else
	static const int PARAM_TIMEOUT_CAMSET		= 120000;		// カメラ選択設定アンサー待ちタイマー
	static const int PARAM_TIMEOUT_RESUME_START = 60000;		// 途中参加 カメラリンクアンサー待ちタイマー
	static const int PARAM_TIMEOUT_RESUME_WAIT	= 60000;		// 途中開始 判定アンサー待ちタイマー
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////
//// 公開関数(すべて静的にする)
public:
	static HANDLE GetEvTimeCamset();												// カメラ選択設定アンサー待ちタイマーイベントを返す
	static HANDLE GetEvTimeResumeStart();											// 途中参加設定アンサー待ちイベントを返す
	static HANDLE GetEvTimeResumeWait(int nKind);									// 途中開始判定アンサー待ちイベントを返す

	// カメラセット関連
	static int GetHtParaCamSize();													// パララン入力カメラNo取得
	static int GetHtParaCamNo(int pcno, int camno);									// パララン入力カメラNo取得
	static int* GetHtParaCamNo();													// パララン入力カメラNo取得
	static void SetHtParaCamNo(int const* pLogiParaCam);							// パララン入力カメラNo設定
	static void SetHtParaCamNo(int pcno, int camno, int camset);					// パララン入力カメラNo設定

	// 要求、応答イベント関連
	static void ResumeStart(COMMON_QUE* que);										// 途中開始要求
	static void ResumeStop(COMMON_QUE* que);										// 途中停止要求
	static void Send_HtResume_Start(int nKind, CoilManager const *pCoilManager, int nJoinFrameNo = 0);	// 判定PC へ 途中開始を通知
	static void	Send_HtResume_Stop(int nKind);										// 判定PC へ 途中停止を通知
	static void ResumeInitStart(COMMON_QUE * que);									// 途中開始初期化処理(途中参加設定完了アンサー待ちへ)
	static void WaitResumeStart_Answer(COMMON_QUE* que);							// 途中開始アンサー
	static void WaitResumeStop_Answer(COMMON_QUE* que);								// 途中停止アンサー
	static void WaitResumeStart_Timeout();											// 途中開始待ち 処理 (タイムアウト発生)
	static void WaitResumeWait_Timeout(int nKind);									// 途中開始 判定アンサー待ち (タイムアウト発生)
	static void WaitCamset_Answer(COMMON_QUE* que);									// カメラ選択設定完了通知
	static void WaitCamset_TimeOut();												// カメラ選択設定待ち 処理 (タイムアウト発生)
	static void WaitLinkswJoin_Answer(COMMON_QUE* que, CoilManager const *pCoilManager);// 途中参加設定完了通知
	static void DebugSend_CamLinksw_CamsetCmpl(int mode);							// デバッグ用 カメラ選択設定完了通知送信

	// カメラリンクスイッチ制御関連
	static void RegistLinkswCmd();													// 検査開始時のリンクスイッチコマンド登録
	static void SetLinkswCmd(LINKSW_CMD cmd);										// コマンドリストにリンクスイッチコマンド登録
	static void ClearLinkswCmd(ENUM_LINKSW_CMD state = E_LINKSW_CMD_NOP);			// コマンドリストクリア
	static bool IsTargetCamNo(long nFPGABoardNo, long nCamno);						// パラランカメラの有効性チェック
	static void Send_NextCmdSelect();												// 次のコマンド送信（キュー制御）
	static bool IsEqualCamsetCMD(COMMON_QUE* que);									// 送信中のカメラ設定コマンドの応答かチェックする
	static bool IsRecvCamsetCMD(int nPrNo);											// 指定パラランPCのカメラ設定完了受信済みかチェックする

	static void LogicParaRunFunc::ClearSelectCamset();								// 判定PC カメラペアID初期化
	static void LogicParaRunFunc::SetSelectCamset(int pcid, int camno);				// 判定PC カメラペアID設定

private:
	static void	Send_SoGamenn_MailStatus();											// PCマスター管理に再起動要求通知

///////////////////////////////////////////////////////////////////////////////////////////////////////
//// メンバー 変数
private:
	//=========================================
	// ロジックパララン用定義
	static int						m_nLogiParaCam[NUM_ALL_HTPARA][NUM_CAMLINK_BOARD];// カメラセット
	static ENUM_LINKSW_CMD			m_LinkswCmdKind;								// TO_LINKSW送信 処理区分
	static std::vector<LINKSW_CMD>	mlst_SendCmd;									// TO_LINKSW送信コマンドリスト
	static int						m_nSendCmdIndex;								// TO_LINKSW送信コマンド送信位置
	static int						m_nSelectCamset[NUM_ALL_HANTEI];				// カメラペアID（PCID毎）
																			
	//=========================================
	// ログ用
	static LogFileManager*			mcls_pLog;										// LogFileManagerスレッドインスタンス
	static CString					my_sThreadName;									// 自スレッド名

	//=========================================
	// 外部参照
	static StatusManager*			mcls_pStatus;									// ステータス制御基底クラス
	static PlgManager*				mcls_pPlg;										// PLG管理クラス
	static CoilManager*				mcls_pCoil;										// コイル情報管理クラス
	static HANDLE					m_evTimeCamset;									// カメラ選択設定アンサー待ちタイマー
	static HANDLE					m_evTimeResumeStart;							// 途中参加設定アンサー待ちタイマー
	static HANDLE					m_evTimeResumeWait[NUM_ALL_HTPARA];				// 途中開始判定アンサー待ちタイマー
};

