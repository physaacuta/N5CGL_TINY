#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC操作クラス

#include "..\..\Include\KizuSmyu.h"										// GOTインクルード
#include "..\..\Include\ToTbl.h"										// 統括共有メモリ用ヘッダー
#include "..\..\Include\OutInterface.h"									// 外部IF用ヘッダー

#include "PlgControlManager.h"



class PioControlManager :	public ThreadManager
{
private:
	static const int	TIMER_CYCLE = 50;					// チェック周期
//// 公開関数
public:
	PioControlManager(void);
	virtual ~PioControlManager(void);


	//=========================================
	// 画面公開用
	int	GetMaxBood() const { return m_nMaxBood;}
	char const* GetSmyuName(int nIndex) { return m_sSmyuName[nIndex];} 
	char const* GetBitName(int bood, int port, int bit, int pio) const { return m_cBitName[bood][port][bit][pio]; }


#ifndef SEQ_PIO
	void SetValPi(int bood, int port, BYTE onoff) { mtbl_pSmyu[bood]->nDi[port] = onoff; }
	void SetValPi(int bood, int port, int bit, bool onoff) { if (onoff) { mtbl_pSmyu[bood]->nDi[port] |= GetIndexToBit(bit); } else { mtbl_pSmyu[bood]->nDi[port] &= ~GetIndexToBit(bit); } }
	void SetValPo(int bood, int port, int bit, bool onoff) { if (onoff) { mtbl_pSmyu[bood]->nDo[port] |= GetIndexToBit(bit); } else { mtbl_pSmyu[bood]->nDo[port] &= ~GetIndexToBit(bit); } }
	bool GetValPi(int bood, int port, int bit) const { return (mtbl_pSmyu[bood]->nDi[port] & GetIndexToBit(bit)) ? true : false; }
	bool GetValPo(int bood, int port, int bit) const { return (mtbl_pSmyu[bood]->nDo[port] & GetIndexToBit(bit)) ? true : false; }
	//BYTE GetValPo(int bood, int port) const			 { return mtbl_pSmyu[bood]->nDo[port]; }
#else
	void SetValPi(int bood, int port, BYTE onoff);
	void SetValPi(int bood, int port, int bit, bool onoff);
	void SetValPo(int bood, int port, int bit, bool onoff);
	bool GetValPi(int bood, int port, int bit) const;
	bool GetValPo(int bood, int port, int bit) const;
	//BYTE GetValPo(int bood, int port) const			 { return mtbl_pSmyu[bood]->nDo[port]; }
#endif
	

	void SetDebug_Pi(bool flg)	{ m_bDegub[0] = flg; }
	void SetDebug_Po(bool flg)	{ m_bDegub[1] = flg; }
	bool GetDebug_Pi() const	{ return m_bDegub[0]; }
	bool GetDebug_Po() const	{ return m_bDegub[1]; }

	void SetPlgCls(PlgControlManager* pCls) { m_clsPlgCMgr = pCls; };

//// メンバー関数
private:
	//int ThreadFirst() {return 0;};										// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	//int ThreadLast() {return 0;};										// スレッド終了前処理 (終了シグナル後にコールバック)
 //   void ThreadEvent(int nEventNo) {return;};							// スレッドイベント発生
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
	void ThreadEvent(int nEventNo);										// スレッドイベント発生	

	//=========================================
	// 小物
	void GetIniParam();													// Iniファイル取得
	static int	GetBitToIndex(BYTE addr);								// 指定位置ビット を No に変換
	static BYTE GetIndexToBit(int index);								// 指定位置No を ビット に変換
#ifdef SEQ_PIO
	void SetSeqToSmyu(int bood, int port, int bit, bool onoff, int Id) {
		int nWk = m_cBitAddr[bood][port][bit][Id];
		if (-1 == nWk) return;
		mtbl_pSeq->fb_dm[Id].Data[nWk] = onoff ? 1 : 0;
		if(onoff && 0 == Id)WpdCheck(nWk);
	}
	void SetSeqToSmyu(int bood, int port, BYTE onoff, int Id) {
		for (int ii = 0; ii < 8; ii++) {
			int nWk = m_cBitAddr[bood][port][ii][Id];
			if (-1 == nWk) continue;
			mtbl_pSeq->fb_dm[Id].Data[nWk] = (onoff &  (1 << ii)) != 0;
			if ((onoff &  (1 << ii)) != 0 && 0 == Id)WpdCheck(nWk);
		}
	}
	void WpdCheck(int nId) {
		if (-1 != m_nIdPioKen && nId == m_nIdPioKen) m_clsPlgCMgr->Set_WpdKen();
		if (-1 != m_nIdPioSub && nId == m_nIdPioSub) m_clsPlgCMgr->Set_WpdSub();
		if (-1 != m_nIdPioDsp && nId == m_nIdPioDsp) m_clsPlgCMgr->Set_WpdDsp();
	}
	void ResetCheck() {
		if (mtbl_pSeq->fb_dm[DM_SEND_REC].Data[m_nIdPioReset] == m_nStPioReset) return;

		if (1 == mtbl_pSeq->fb_dm[DM_SEND_REC].Data[m_nIdPioReset] && 1 != m_nStPioReset) m_clsPlgCMgr->Set_ResetSignal();
		m_nStPioReset = mtbl_pSeq->fb_dm[DM_SEND_REC].Data[m_nIdPioReset];
	}
#endif

//// メンバー変数
private:

	//=========================================
	HANDLE					m_evTimerCycle;								// チェック周期イベント
	//=========================================
	// 同期制御
	//CRITICAL_SECTION		m_csLock;									// クリティカルセクション



	//=========================================
	// その他もろもろ
	bool					m_bDegub[2];								// デバック時 true [0:PI 1:PO]

	//=========================================
	// 共有メモリ
	int						m_nMaxBood;									// 有効個数
	CString					m_sSmyuName[SMYU_PIO_MAX_BOOD];				// PIO情報 名称
	HANDLE					m_hTblSmyu[SMYU_PIO_MAX_BOOD];				// PIO情報 ハンドル
	SMYU_TBL_PIO*			mtbl_pSmyu[SMYU_PIO_MAX_BOOD];				// PIO情報 データ

	char					m_cBitName[SMYU_PIO_MAX_BOOD][SMYU_PIO_MAX_PORT][8][2][32];	// 個々のボタン名称
	int						m_cBitAddr[SMYU_PIO_MAX_BOOD][SMYU_PIO_MAX_PORT][8][2];		// 個々のボタンに紐づくアドレス

	CString					m_sSeqName;									// シーケンサ情報 名称
	HANDLE					m_hTblSeq;									// シーケンサ情報 ハンドル
	TO_SEQMEM_TBL*			mtbl_pSeq;									// シーケンサ情報 データ

	PlgControlManager*		m_clsPlgCMgr;

	int						m_nIdPioKen;								// 検査用WPDインデックス
	int						m_nIdPioSub;								// 検査比較用WPDインデックス
	int						m_nIdPioDsp;								// 表示用WPDインデックス
	int						m_nIdPioReset;								// リセット信号
	bool					m_nStPioReset;								// リセット信号前回値
};
