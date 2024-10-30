#pragma once

// 半固定
#include "..\..\Include\LineCommon.h"									// φωインクルード

// φωクラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLib\Class\AutoLock.h"						// クリティカルセクション制御クラス

// φωライブラリ インクルード
#include "..\..\Library\KizuODBC\OdbcBase.h"

#include "MainConst.h"

using namespace KizuLib;
using namespace KizuODBC;

class ParamManager :	public ThreadManager
{
public:
	ParamManager(void);
	virtual ~ParamManager(void);

private:
	int		ThreadFirst();												// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int		ThreadLast();												// スレッド終了前処理 (終了シグナル後にコールバック)
    void	ThreadEvent(int nEventNo);									// スレッドイベント発生



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// パラメータ用構造体
public:
	//=============================
	// φω定数　構造体
	//=============================
	struct P_COMMON_MAST
	{
		int		nLongPd_GuidanceAvailable;								// [長周期欠陥ガイダンス有無] 探傷画面への長周期のガイダンス有無(0:なし 1:あり)
		int		nLongPd_GuidanceCheckInterval;							// [長周期欠陥ガイダンス確認実施間隔] 長周期実績のチェック間隔 [秒]
		int		nLongPd_NearFurnaceRangeFrom;							// [長周期欠陥炉チカ判定範囲From] 周期ピッチが炉チカと判定される範囲のFrom [mm]
		int		nLongPd_NearFurnaceRangeTo;								// [長周期欠陥炉チカ判定範囲To] 周期ピッチが炉チカと判定される範囲のTo [mm]
	};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// イベント公開
public:


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// パラメータ参照
public:
	inline const P_COMMON_MAST&				GetCommonMast()		const {return (P_COMMON_MAST&)m_CommonMast;}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// パラメータ取得関連 (同期)
public:
	bool ReadParam_All();												// 全パラメータ読み込み
	
	bool Read_CommonMast();												// 定数パラメータ読込

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// スレッドキュー
public:
	ThreadQueue<COMMON_QUE>	gque_Deli;									// パラメータ変更通知受け渡しキュー


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// メンバー関数
private:

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// メンバー変数
private:
	//// パラメータ
	P_COMMON_MAST				m_CommonMast;							// φω定数
};
