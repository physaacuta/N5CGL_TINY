#pragma once


// 疵検クラス インクルード
#include "..\..\Library\KizuLib\CLASS\MailSlotManager.h"				// ワーカースレッドマネージャー


#include "SmyuBaseManager.h"

class SmyuEvtManager :	public SmyuBaseManager
{
//// メンバー定数
private :
	static const int	TIME_EXEC_INTERVAL		= 30;					// PLGチェック実行周期
	static const int	MAX_MAIL_WAIT_NUM		= 8;					// メールスロット待ち対象タスク最大数


//// 公開関数
public:
	SmyuEvtManager(void);
	virtual ~SmyuEvtManager(void);

	virtual void SmyuStart();
	virtual void SmyuStop();


//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	

	void MailRecvStart(int nIndex);										// メールスロット 受信開始
	void GetPoNowData();												// Po現在値取得
	
	void SmyuExec_Po();													// シミュレータ 実行 (PO変化)
	void SmyuExec_Mail(int nIndex);										// シミュレータ 実行 (メールスロット受信)



//// メンバー変数
private:

	//// ハンドル
	HANDLE					m_evTimerExec;								// 定周期実行イベント
	HANDLE					m_evRecvMail[MAX_MAIL_WAIT_NUM];

	//// メールスロット受信用
	int						m_nMailCnt;									// メールスロット 接続数
	// 以下の配列は、全部同じIndexを使用
	MailSlotManager*		mcls_pMail[MAX_MAIL_WAIT_NUM];				// 受信メールスロットクラス
	CString					m_RecvName[MAX_MAIL_WAIT_NUM];				// 受信メールスロット名称
	COMMON_QUE				m_RecvBuf[MAX_MAIL_WAIT_NUM];				// 受信バッファ (非同期のため必要)


	//// PO 変化
	bool					m_nDoBuf[SMYU_PIO_MAX_BOOD][SMYU_PIO_MAX_PORT][8];		// PO前回値
};
