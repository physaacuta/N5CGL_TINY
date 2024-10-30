//=====================================================================
// ステータスベース管理クラス
//	【全ライン】
//		・疵検で一般的なステータスを管理
//		・機器状態の読み込み処理を行う
//=====================================================================

#pragma once

// 半固定
#include "..\..\Include\LineCommon.h"									// 疵検インクルード

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLibMFC\Class\DivNameManager.h"				// DIV定数 名称取得クラス
#include "..\..\Library\KizuLib\Class\AutoLock.h"						// クリティカルセクション制御クラス

#include "..\..\Library\DefectLib\KikiManager.h"						// 機器状態管理クラス

// 共通ヘッダー
#include "..\..\Include\ToHtCommon.h"									// 統括判定用インクルード

// ローカル
#include "MainConst.h"													// 自クラス定数ヘッダー
#include "StatusLogFunc.h"												// スレータスログクラス

using namespace KizuLib;

class StatusBaseManager : public ThreadManager
{

public:
	//// 受け渡し構造体
	struct TYP_MY_ERRWAR_INF {
		bool			bErr;											// 大区分 (true:Err false:War) Err時は、検査停止
		int				nKind;											// 区分
		int				nSt[2];
	};

protected :
	//// ローカル定数
	static const int PARAM_DB_READ_INTERVAL_TIME	= 8000;//3000;		// DBチェック周期 [ms]	※ 故障ログチェック時間よりはちょっと短くする
	static const int PARAM_SYSLOG_BASENO			= 300;				// シスログの既定No (ここから10コは、このクラス専用とする)


public:
	StatusBaseManager(void);
	virtual ~StatusBaseManager(void);


	//=========================================
	// プロパティ
	void SetStatusFunc(StatusLogFunc* pCls) { mcls_pStatusFunc = pCls; }	// ステータスログ



	//=========================================
	// 動作定義
	void SetDspSyslog(bool bUmu)	{ m_bDspSyslog = bUmu; }				// 機器状態へシスログ追加 有無
	void SetAddStlog(bool bUmu)		{ m_bAddStlog = bUmu; }					// ステータスログ追加 有無
	



	//=========================================
	// タスクステータス状態取得
	bool			IsDoSequence();																		// シーケンスが行える状態か
	bool			IsRun()						  const { return m_emKadou==DIV_KADOU_START ? true : false; }	// 検査中
	bool			GetMenMode()				  const { return m_bMenMode;}							// 片面検査有無  許可:true / 不許可:false 
	int				GetMenState()				  const { return m_nMenState;}							// 検査面状態 (-1:両面検査 0:表のみで実施 1:裏のみで実施)

	EM_DIV_PLG		GetPlg()					  const { return m_emPlg; }								// PLG区分取得
	EM_DIV_UNTEN	GetUnten()					  const { return m_emUnten; }							// 運転状態取得
	EM_DIV_SOGYO	GetSogyo()					  const { return m_emSogyo; }							// 操業状態取得
#ifdef DSP_SOGYO_CHECK
	EM_DIV_SOGYO	GetSogyoDsp()				  const { return m_emSogyoDsp; }						// 操業状態取得 表示用
#endif
	EM_DIV_KADOU	GetKadou()					  const { return m_emKadou; }							// 稼動状態
	EM_DIV_SYS		GetSys(int men)				  const { return m_emSys[men]; }						// システム状態取得
	EM_DIV_SYS		GetCycleSys(int men)		  const	{ return m_emCycleSys[men];	}					// 周期システム状態取得			
	EM_DIV_SYS		GetParaSys(int pcno)		  const { return m_emParaSys[pcno]; }					// パラランシステム状態取得

	EM_DIV_KENSA 	GetKensa(int men)			  const { return m_emKensa[men]; }						// 検査状態取得
	EM_DIV_KIKI 	GetKiki(int men)			  const { return m_emKiki[men]; }						// 機器状態取得


	CString			GetMenModeName(bool st)		  const { return st?"片面検査許可":"片面検査不許可(両面必須)"; };	// 指定の片面検査有無名称取得 
	CString			GetHanteiName(EM_DIV_HANTEI st)
		{
				if( DIV_HANTEI_HT == st )   return "単発判定";
				if( DIV_HANTEI_CY == st )   return "周期判定";
				if( DIV_HANTEI_PR == st )   return "パララン判定";
				return "不明";
		};
	CString			GetMarkStateName(bool st)	  const { return st?"正常":"異常"; };					// マーキング装置状態

	EM_DIV_PARA_KADOU Get_ParaKadou(EM_DIV_SYS nParaSys)												// パラランPC システム状態を稼働状態に変換（RAS表示、SO_GAMENN送信用）
	{
		EM_DIV_PARA_KADOU nKadou = DIV_PARA_KADOU_STOP;

		switch (nParaSys)
		{
			// [システム状態] 検査停止待ち
		case DIV_SYS_WAITSTOP:
			nKadou = DIV_PARA_KADOU_WAITSTOP;
			break;

			// [システム状態] 検査開始初期化中
		case DIV_SYS_INITSTART:
			// [システム状態] 検査開始待ち
		case DIV_SYS_WAITSTART:
			// [システム状態] 強制検査再起動中
		case DIV_SYS_RETRY:
			nKadou = DIV_PARA_KADOU_WAITSTART;
			break;

			// [システム状態] 検査中
		case DIV_SYS_RUN:
			nKadou = DIV_PARA_KADOU_START;
			break;

			// [システム状態] 停止モード
		case DIV_SYS_INIT:
			// [システム状態] 準備完了
		case DIV_SYS_IDLE:
			// [システム状態] 検査停止
		case DIV_SYS_STOP:
		default:
			nKadou = DIV_PARA_KADOU_STOP;
			break;
		}

		return nKadou;
	};

	//=========================================
	// 状態設定
	bool			SetMenMode(bool st);																// 片面検査有無
	bool			SetMenState(int st);																// 検査面状態変更
	bool			SetPlg(EM_DIV_PLG st);																// PLG区分変更
	bool			SetUnten(EM_DIV_UNTEN st);															// 運転状態変更
	bool			SetSogyo(EM_DIV_SOGYO st, int nMpm);												// 操業状態変更
#ifdef DSP_SOGYO_CHECK
	bool			SetSogyoDsp(EM_DIV_SOGYO st, int nMpm);												// 操業状態変更 表示用
#endif
	bool			SetKadou(EM_DIV_KADOU st);															// 稼動状態変更
	bool			SetSys(int men, EM_DIV_SYS st);														// システム状態変更
	bool			SetCycleSys(int men, EM_DIV_SYS st);												// 周期システム状態変更
	bool			SetParaSys(int pcno, EM_DIV_SYS st);												// パラランシステム状態変更

	//=========================================
	// 検査中異常把握
	inline ThreadQueue<TYP_MY_ERRWAR_INF>&	GetQueMyErrWar()	{ return mque_MyErrWar; }
	virtual void	AddQueMyErrWar(bool bErr, int nKind, int nSt1=0, int nSt2=0);						// 検査中異常発生
	virtual bool	OnMyErrWar(TYP_MY_ERRWAR_INF const* pInf) { return false;};							// 検査中の状態把握 (自タスク内の異常把握)


	//=========================================
	// 機器状態取得
	const TYP_STATUS_READ& GetStatusRead()								// 現在の機器状態一覧を返す
		{ 
			AutoLock lock(&m_csLock);
			return m_oldStateRead;
		}
	const TYP_STATUS_NOWSYSTEM& GetStatusNowSystem()					// 現在の機器状態一覧を返す
		{ 
			AutoLock lock(&m_csLock); 
			return m_oldNowSystem;
		} 


	//=========================================
	// ステータス制御
	inline KikiManager const& GetKikiMgr()	const	{ return (KikiManager&)mcls_pKiki; } 


	//// 機器状態 非同期書き込み
	void SetStatusQue(int id, int state, bool bLogDsp =true, char const * cMsg = "" )   {mcls_pKiki->SetStatusQue(id,state,bLogDsp,cMsg);}
	void SetStatusQue(int id, bool state, bool bLogDsp =true, char const * cMsg  = "" ) {mcls_pKiki->SetStatusQue(id,state,bLogDsp,cMsg);}	
	void SetStatusQue(char const * cKey, bool state, bool bLogDsp =true, char const * cMsg  = "" ) {mcls_pKiki->SetStatusQue(cKey,state,bLogDsp,cMsg);}
	void SetStatusQue(char const * cKey, int state, bool bLogDsp =true, char const * cMsg  = "" )  {mcls_pKiki->SetStatusQue(cKey,state,bLogDsp,cMsg);}
	void SetStatusQue(char const * cKeyFormat, int id, int state, bool bLogDsp =true, char const * cMsg = "" )  
			{	char cKey[256]; memset(cKey, 0x00, sizeof(cKey));
				sprintf_s(cKey, sizeof(cKey), cKeyFormat, id);
				mcls_pKiki->SetStatusQue(cKey,state,bLogDsp,cMsg);
			}

	//// 機器情報 非同期読み込み要求
	void SetStatusRead() { mcls_pKiki->SetStatusRead(); }				// (非同期)強制読込み開始イベント。完了したら通常通り m_evReadEnd がシグナルとなる
	void RefreshStatus() { mcls_pKiki->Refresh(); ReadKikiStatus(); }	// 機器状態 強制更新



//// メンバー関数
protected:
	int				ThreadFirst();										// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int				ThreadLast();										// スレッド終了前処理 (終了シグナル後にコールバック)
    void			ThreadEvent(int nEventNo);							// スレッドイベント発生

	void			ReadKikiStatus();									// 機器状態取得 & 状態チェック
	void			CheckStatusLog();									// ステータス情報チェック

	//=========================================
	// 仮想関数
	virtual void	OnReadKikiStatusEnd(bool bChange) {};				// 機器状態取得 & 状態チェック 完了時
	virtual void	ThreadFirst_AddHandle() {};							// シグナル待ち行列の追加
	virtual void	OnAddSignal(int nEventNo) {};						// シグナルを追加した奴のイベント (追加したものを0始まりとする)



//// その他もろもろ
protected :

	//=========================================
	// 各インスタンス
	KikiManager*			mcls_pKiki;									// 機器状態管理クラス
	StatusLogFunc*			mcls_pStatusFunc;							// ステータスログクラス		

	//=========================================
	// 受け渡しキュー
	ThreadQueue<SOCK_BASE_HEAD>*	mque_pOpSend;						// 統括→表示送信通知のキュー
	ThreadQueue<TYP_MY_ERRWAR_INF>	mque_MyErrWar;						// 検査中の状態把握


	//=========================================
	// 同期制御
	CRITICAL_SECTION		m_csLock;									// クリティカルセクション


	//=========================================
	// 管理ステータス
	bool					m_bMenMode;									// 片面検査許可:true / 不許可:false 
	int						m_nMenState;								// 検査面状態 (-1:両面検査 0:表のみで実施 1:裏のみで実施)
	EM_DIV_PLG				m_emPlg;									// PLG区分
	EM_DIV_UNTEN			m_emUnten;									// 運転状態
	EM_DIV_SOGYO			m_emSogyo;									// 操業状態
#ifdef DSP_SOGYO_CHECK
	EM_DIV_SOGYO			m_emSogyoDsp;								// 操業状態 表示用
#endif
	EM_DIV_KADOU			m_emKadou;									// 稼動状態
	EM_DIV_SYS				m_emSys[NUM_MEN];							// システム状態 [面]
	EM_DIV_SYS				m_emCycleSys[NUM_MEN];						// 周期判定機能単独のシステム状態 [面]
	EM_DIV_SYS				m_emParaSys[NUM_ALL_HTPARA];				// パララン判定機能単独のシステム状態 [面]
	EM_DIV_KENSA			m_emKensa[NUM_MEN];							// 検査状態 [面]
	EM_DIV_KIKI				m_emKiki[NUM_MEN];							// 機器状態 [面]


	//=========================================
	// ステータス
	TYP_STATUS_NOWSYSTEM	m_oldNowSystem;								// 前回チェック時の機器状態
	TYP_STATUS_READ			m_oldStateRead;								// 前回チェック時の全機器状態


	//=========================================
	// 情報
	char					m_cKizukenNo[SIZE_KIZUKEN_NO];				// 管理No
	COIL_BASEDATA*			m_pCoilBase;								// 基本情報

	//=========================================
	// 動作定義
	bool					m_bDspSyslog;								// 機器状態へシスログ追加 有無
	bool					m_bAddStlog;								// ステータスログ追加 有無
};
