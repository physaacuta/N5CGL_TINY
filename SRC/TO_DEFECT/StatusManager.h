#pragma once

// 半固定
#include "..\..\Include\LineCommon.h"									// φωインクルード
#include "..\..\Include\ToTbl.h"										// 統括用共有メモリテーブルヘッダ

// ローカル
#include "MainConst.h"
#include "StatusBaseManager.h"
#include "ParamManager.h"

#include "OpSendFunc.h"												// 表示管理送信クラス
#include "ToOutMailSender.h"											// 外部メールスロット送信モジュール

// インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー

using namespace KizuLib;

class StatusManager :	public StatusBaseManager
{
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// タイマー用定数
private:
#ifndef LOCAL
	static const int PARAM_ST_WRITE_INTERVAL_TIME	= 30000;			// ステータス書込み周期 [ms]
	static const int PARAM_ST_SOGYO_INTERVAL_TIME   = 2000;				// 操業状態書き込み周期 [ms]
#else
	static const int PARAM_ST_WRITE_INTERVAL_TIME	= 30000;			// ステータス書込み周期 [ms]
	static const int PARAM_ST_SOGYO_INTERVAL_TIME   = 2000;				// 操業状態書き込み周期 [ms]
#endif

	//enum EM_DIV_PROCON_ST {											// プロコンへ送信するステータス値	
	//	DIV_PROCON_ST_NON	= -1,										// 内部的な初期値
	//	DIV_PROCON_ST_OK	= 0,										// 正常
	//	DIV_PROCON_ST_NG,												// 異常 (重故障or縮退)
	//	DIV_PROCON_ST_STOP												// 停止
	//};

//// 公開関数
public:
	StatusManager(void);
	virtual ~StatusManager(void);


/////////////////////////////////////////////////////////////////////////////////////////
//// プロパティ
public:
	void SetParamMgr(ParamManager* pCls)		{ mcls_pParam = pCls; }		// パラメータ管理

/////////////////////////////////////////////////////////////////////////////////////////
//// 機器状態(外部アクセス)
public:

	//=====================================
	// SPM状態
	void	SetSpmStat(EM_DIV_SPM_STAT st)	{ m_emSpmStat = st; };
	int		GetSpmStat(void)	{ return (int)m_emSpmStat; };
	bool	IsSpmStat(void)		{ return DIV_SPM_TRUE == m_emSpmStat;};

		
	//=====================================
	// サーバ盤 開閉状態
	void	SetDoorOpen(bool val)					{ m_bIsDoorOpen = val; }	// サーバ盤 扉 開/閉状態
	void	SetKeyOpen(bool val)					{ m_bIsKeyOpen  = val; }	// サーバ盤 鍵 開/閉状態
	bool	GetDoorOpen() { return m_bIsDoorOpen; }	// True:サーバ盤 扉 開状態
	bool	GetKeyOpen() { return m_bIsKeyOpen; }	// True:サーバ盤 鍵 開状態

	//=====================================
	// コイル情報
	void SetCoilInfStatus(COIL_INF* pCoil)								// 通板中のコイル情報に機器状態をセット
			{ 
				AutoLock lock(&m_csLock); 
				m_pCoil = pCoil;
				if(NULL == pCoil){
					this->m_pCoilBase = NULL;
					memset(this->m_cKizukenNo, 0x00, SIZE_KIZUKEN_NO);
				}else{
					this->m_pCoilBase = &pCoil->data;
					memcpy(this->m_cKizukenNo, pCoil->cKizukenNo, SIZE_KIZUKEN_NO);
				}

				CheckCoilInfStatus();	// 初期状態をセット
				
				//// 機器状態 変更
				// 検査表面状態 // 検査パターン
				if (NULL != mcls_pKiki) {
					mcls_pKiki->SetStatusQue("SCODE_TOP_ID", pCoil->setting.scode[0], false);
					mcls_pKiki->SetStatusQue("SCODE_BOT_ID", pCoil->setting.scode[1], false);
					mcls_pKiki->SetStatusQue("PCODE_TOP_ID", pCoil->setting.pcode[0], false);
					mcls_pKiki->SetStatusQue("PCODE_BOT_ID", pCoil->setting.pcode[1], false);
				}
				else {
					KizuFunction::SetStatus("SCODE_TOP_ID", pCoil->setting.scode[0], false);
					KizuFunction::SetStatus("SCODE_BOT_ID", pCoil->setting.scode[1], false);
					KizuFunction::SetStatus("PCODE_TOP_ID", pCoil->setting.pcode[0], false);
					KizuFunction::SetStatus("PCODE_BOT_ID", pCoil->setting.pcode[1], false);
				}
			}

	
	//=====================================
	// LED照明装置 自動点灯消灯状態
	void SetLedAutoCheckOff(bool bAutoOff) { m_bLedAutoCheckOff = bAutoOff; }

	//=========================================
	//// 判定状態取得
	// PCID
	int				GetHtID(int men, int no)	  const { return m_nHt_ID[men][no]; }					// 検査対象の判定PCのpcid
	EM_DIV_BOOL		GetHtState(int men, int no)	  { return m_bHt_State[men][no]; }
	bool			IsHtState(int men, int no)	  { return DIV_BOOL_TRUE == m_bHt_State[men][no] ? true : false; }	// 判定検査状態
	bool			IsHtState_AllOnOff(int men, EM_DIV_BOOL flg)   { for(int ii=0; ii<NUM_HANTEI; ii++) {if(flg!=m_bHt_State[men][ii]) return false;} return true; }	// 全部flgと同じ値であれば true
	bool			IsHtState_OneOnOff(int men, EM_DIV_BOOL flg)
	{
		for(int ii=0; ii<NUM_HANTEI; ii++)
		{
			if(flg==m_bHt_State[men][ii])
				return true;
		}
		return false;
	}	// 1つでもflgと同じ値があれば true

	//=========================================
	// 判定状態設定
	void			SetHtState_AllOff(int men)		{ for(int ii=0; ii<NUM_HANTEI; ii++) m_bHt_State[men][ii] = DIV_BOOL_NON; }	// 状態リセット
	void			SetHtState(int men, int no, EM_DIV_BOOL flg)	{ m_bHt_State[men][no] = flg; }			// 判定状態をセット

	//=========================================
	//// ロジックパララン状態取得
	// 検査対象のロジックパラランPCのpcid
	int				GetHtParaID(int pcno)	const {	return m_nHt_Para_ID[pcno]; }

	//=========================================
	// ロジックパララン検査状態	
	EM_DIV_BOOL		GetHtParaState(int pcno)	const { return m_bHt_Para_State[pcno];	}
	void			SetHtParaID(int pcno, int value) { m_nHt_Para_ID[pcno] = value; }
	bool			IsHtParaState(int pcno)	{ return DIV_BOOL_TRUE == m_bHt_Para_State[pcno]? true : false;	}
	bool			IsHtParaState_AllOnOff(EM_DIV_BOOL flg)	{ for (int ii = 0; ii < NUM_ALL_HTPARA; ii++) { if (flg != m_bHt_Para_State[ii]) return false; } return true; }	// 全部flgと同じ値であれば true
	bool			IsHtParaState_OneOnOff(EM_DIV_BOOL flg) { for (int ii = 0; ii < NUM_ALL_HTPARA; ii++) { if (flg == m_bHt_Para_State[ii]) return true; }	return false; }	// 1つでもflgと同じ値があれば true
	int				GetGroupTopCamNo(int nFPGABoardNo);	// 指定スイッチFPGAボードIDのグループの先頭カメラNoを返す

	//=========================================
	// ロジックパララン状態設定
	// 状態リセット
	void			SetHtParaState_AllOff()
	{
		for (int ii = 0; ii < NUM_ALL_HTPARA; ii++) m_bHt_Para_State[ii] = DIV_BOOL_NON;
	}
	// ロジックパララン状態をセット
	void			SetHtParaState(int pcno, EM_DIV_BOOL flg)
	{
		m_bHt_Para_State[pcno] = flg;
	}

	//=========================================
	// 判定PC振替
	bool			SelectHanteiPC_SingleGroup(bool* bOk);									// 検査可能な判定PCを判別する [←MainInstance]（今回未使用）
	bool			SelectHanteiPC_MultiGroup(bool* bOk);									// 検査可能な判定PCを判別する [←MainInstance]
	bool			SelectHanteiPC_ResumeGroup();											// 検査可能なロジックパラランPCを判別する [←MainInstance]

	//=========================================
	// 機器状態書き込み
	void			SetCamTemp(int camset, BYTE *temp)
						{
							AutoLock lock(&m_csLock); 
							for(int ii=0; ii<MAX_PHYSICAL_CAMANGLE; ii++) {
								if( 0==temp[ii] ) continue;
								m_nCamTemp[camset][ii] = (int)temp[ii];
							}
						}
	void			SetCamTemp(int camset, int angle, float temp)
						{
							AutoLock lock(&m_csLock);
							m_nCamTemp[camset][angle] = (int)temp;
						}
	void			SetExpTime(int camset, int color, int exptime)
						{
							AutoLock lock(&m_csLock);
							m_nExpTime[camset][color] = exptime;
						}
	void			SetCamGain(int camset, int color, int gain)
						{
							AutoLock lock(&m_csLock);
							m_nCamGain[camset][color] = gain;
						}
	void			SetCamVolt(int camset, int volt)
						{
							AutoLock lock(&m_csLock);
							m_nCamVolt[camset] = volt;
						}
	void			SetBrValue(int camset, int color, int br)
						{
							AutoLock lock(&m_csLock);
							m_nBrValue[camset][color] = br;
						}
	//void			SetCamGain(int camset, float *gain)
	//					{
	//						AutoLock lock(&m_csLock);
	//						for(int ii=0; ii<MAX_PHYSICAL_CAMANGLE; ii++){
	//							if( 0==gain[ii] ) continue;
	//							m_nCamGain[camset-1][ii] = (int)(gain[ii] * 100);
	//						}
	//					}
	//void			SetBrValue(int camset, BYTE *br)
	//					{
	//						AutoLock lock(&m_csLock);
	//						for(int ii=0; ii<MAX_PHYSICAL_CAMANGLE; ii++){
	//							if( 0== br[ii] ) continue;
	//							m_nBrValue[camset-1][ii] = (int)(br[ii]);
	//						}
	//					}
	void			SetLineSpeed(int val) { m_nLineSpeed = val; }		// 検査用ライン速度	[←SogyoMgr]
#ifdef DSP_SOGYO_CHECK
	void			SetLineSpeedDsp(int val) { m_nLineSpeedDsp = val; }	// 表示用ライン速度	[←SogyoMgr]
#endif
	void			SetLength(int val)		{ m_nLength = val; }		// 通板距離 [m]	[←PlgMgr]
	void			SetLedValue(int men, int color, int value)			// 照明装置調光値
						{m_nLedValue[men][color] = value;}

	//=========================================
	// 機器状態読み込み
	int				GetExpTime(int camset, int color) { return m_nExpTime[camset][color]; }			// カメラ露光時間取得
	int				GetCamGain(int camset, int color) { return m_nCamGain[camset][color]; }			// カメラゲイン値取得
	int				GetLedValue(int men, int color)   { return m_nLedValue[men][color]; }			// 照明装置調光値取得
	int				GetBrValue(int men, int color)	  { return m_nBrValue[men][color]; }			// カメラ輝度取得

	//=========================================
	// 検査中異常把握
	bool			OnMyErrWar(TYP_MY_ERRWAR_INF const* pInf);			// 検査中の状態把握 (自タスク内の異常把握)

	bool	GetEdgeUdpHealthySt(int men) {
		AutoLock lock(&m_csLock);
		return m_bEdgeUdpHealthySt[men]; 
	}

	bool	GetEdgeUdpTrackingSt(int men) {
		AutoLock lock(&m_csLock);
		return m_bEdgeUdpTrackingSt[men]; 
	}

//// メンバー関数
private:
	void			CheckCoilInfStatus();								// 現在の機器状態をコイル情報にセット
	void			WriteStatus();										// ステータス書き込み処理
	void			SogyoStatus_LedOnOff();								// 操業状態チェック処理_LED照明点灯/消灯制御

	//=========================================
	// 仮想関数
	virtual void	OnReadKikiStatusEnd(bool bChange);					// 機器状態取得 & 状態チェック 完了時
	virtual void	ThreadFirst_AddHandle();							// シグナル待ち行列の追加
	virtual void	OnAddSignal(int nEventNo);							// シグナルを追加した奴のイベント (追加したものを0始まりとする)

	
//// メンバー変数
private:

	//=========================================
	// 各インスタンス
	ParamManager*			mcls_pParam;								// パラメータ管理
	COIL_INF*				m_pCoil;									// 現在の基準面通板中のコイル情報	

	// シグナル
	HANDLE					m_evTimerWriteSt;							// ステータス書き込みタイマーハンドル
	HANDLE					m_evTimerSogyoSt;							// 操業状態書き込みタイマーハンドル

	//=========================================
	// 判定状態
	int						m_nHt_ID   [NUM_MEN][NUM_HANTEI];			// 検査対象の判定PCのpcidをセット  (PCKind順。ここに入っているのが正常な判定PCID) PCID1オリジン
	EM_DIV_BOOL				m_bHt_State[NUM_MEN][NUM_HANTEI];			// 検査対象の判定PCの状態 (PCKind順) (検査準備完了時にtrue。停止準備完了時にfalse)

	//// 今回はプロビジョン
	//int						m_nCy_ID   [NUM_MEN][NUM_HANTEI_CYCLE];		// 検査対象の判定PCのpcidをセット  (PCKind順。ここに入っているのが正常な判定PCID) PCID1オリジン
	//EM_DIV_BOOL				m_bCy_State[NUM_MEN][NUM_HANTEI_CYCLE];		// 検査対象の判定PCの状態 (PCKind順) (検査準備完了時にtrue。停止準備完了時にfalse)

	//=========================================
	// ロジックパララン状態
	int						m_nHt_Para_ID[NUM_ALL_HTPARA];				// 検査対象のロジックパラランPCのpcidをセット  (PCKind順。ここに入っているのが正常な判定PCID) PCID1オリジン
	EM_DIV_BOOL				m_bHt_Para_State[NUM_ALL_HTPARA];			// 検査対象のロジックパラランPCの状態 (PCKind順) (検査準備完了時にtrue。停止準備完了時にfalse)
	int						m_nHt_TopCamno[DIV_GROUP_MAX - 1];			// 各カメラリンクスイッチグループの先頭カメラNo

	//=========================================
	// 機器状態
	int						m_nCamTemp[MAX_CAMSET][MAX_CAMANGLE];		// カメラ温度
	int						m_nCamGain[MAX_CAMSET][MAX_CAMANGLE];		// カメラゲイン(10^2表記 1.00→100)
	int						m_nExpTime[NUM_CAMERA][MAX_CAMANGLE];		// カメラ露光時間[ns]
	int						m_nBrValue[NUM_CAMERA][MAX_CAMANGLE];		// カメラ輝度(0-255)
	int						m_nCamVolt[NUM_CAMERA];						// カメラ電圧
	int						m_nLineSpeed;								// 検査用ライン速度 (操業状態管理の検査用ライン速度を使用)
#ifdef DSP_SOGYO_CHECK
	int						m_nLineSpeedDsp;							// 表示用ライン速度 (操業状態管理の表示用ライン速度を使用)
#endif
	int						m_nLength;									// 通板距離 [m]
	EM_DIV_SPM_STAT			m_emSpmStat;								// SPM状態(0:SPM圧下無し, 1:SPM圧下有り)
	

	// LED照明関係
	bool					m_bLedAutoCheckOff;							// LED照明 自動点灯消灯状態チェック無効化時 true 
	int						m_nLedLineStopOffTime;						// LED照明 ライン休止OFF時間 [分]
	int						m_nLedDaisyaOffLineTime;					// LED照明 台車オフラインOFF時間[秒] ※単位が秒であることに注意！！
																		// マイナス値指定で処理無効
	int						m_nLedLineStopCnt[NUM_MEN];					// ライン停止時の経過回数
	int						m_nLedLineRunOffCnt[NUM_MEN];				// ライン稼動時に消灯している経過回数
	int						m_nLedDaisyaOffLineCnt[NUM_MEN];			// 台車オフライン時の経過回数
	int						m_nLedValue[NUM_MEN][DIV_LED_COLOR_END];	// 照明装置の調光値[面][色](単位:%)


	// サーバ盤 開閉状態
	bool					m_bIsDoorOpen;								// True:開状態 False:閉状態
	bool					m_bIsKeyOpen;								// True:開状態 False:閉状態

	// エッジUDP状態
	bool					m_bEdgeUdpHealthySt[NUM_MEN];				// UDPヘルシー状態（一定時間内に何かしらの伝文を受信時True）
	bool					m_bEdgeUdpTrackingSt[NUM_MEN];				// UDPトラッキング状態（一定距離内に何かしらの伝文を受信時True）


#ifdef IS_TO_CAMERA_TBL
	HANDLE					m_hCam;										// カメラコントローラ情報テーブル　ハンドル
	TO_CAMERA_TBL*			mtbl_pCam;									// カメラコントローラ情報テーブル	
#endif

public:

	int  GetCamSetPerPc() { return CAMSET_PER_PC; }						// 1PCあたりのカメラセット数[←CamLinkSpdMgr]

private:

	static const int CAMSET_PER_PC = (MAX_CAMSET / NUM_MEN) / (NUM_HANTEI);		// 1PCあたりのカメラセット数
};
