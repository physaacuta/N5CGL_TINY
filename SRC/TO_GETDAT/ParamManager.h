#pragma once
// 半固定
#include "..\..\Include\LineCommon.h"									// 疵検インクルード

// 疵検クラス インクルード
//#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLibMFC\CLASS\FuncBase.h"

// 疵検ライブラリ インクルード
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC操作クラス

// ローカル
#include "MainConst.h"

using namespace KizuLib;
using namespace KizuODBC;

// ファンクションの使い方だが、ThreadManagerを継承しておく。ワーカースレッドは起動しない
class ParamManager : public FuncBase
{

//// 公開構造体
public:
	//=======================================
	// 疵検定数 構造体
	struct P_COMMON_MAST
	{
		int nWalm;									// ウォーマ材の検査パターンレコード位置 (1オリジン)
	};


	//=======================================
	// 検査表面状態 構造体
	struct P_SCODE_OUTPUT
	{
		// 条件設定
		struct JUDGE
		{
			BOOL	bUmu;				// 有効判定 (true:有効 false:無効)
			int		nSpm;				// SPM圧下有無 (-1:無し 0:×(閉:圧下) 1:○(開:開放))
			char	cMeiNo[32];			// 命令No
			char	cCoilNo[32];		// 生産No
			int		nAtuSei[2];			// 注文板厚[μm] 0:下限 1:上限
			char	cKenKijun[32];		// 検査基準
			char	cMuke[32];			// 向先
			char	cYouto[32];			// 用途
			char	cSyuZai[32];		// 主材質
			char	cOmoteSyori1[32];	// 表面処理1桁目
			char	cHinCode[32];		// 品種
		};

		// 実体
		struct INF
		{
			// マスター
			BOOL	bUmu;							// 有効判定 (true:使用 false:未使用)				

			// 条件設定
			JUDGE	typJudge[MAX_CODE_JUDGE];			// 条件設定
		} rec[MAX_SCODE];							// 最終は、その他項目固定

		// 優先度と配列位置の紐付け
		int		nPriority[MAX_SCODE];				// 優先度順にscode(=配列Index)が入っている
		int		nMasterNum;							// 有効なマスター数
	};


	//=======================================
	// 検査パターン別 構造体
	struct P_PCODE_OUTPUT
	{
		// 条件設定
		struct JUDGE
		{
			BOOL	bUmu;				// 有効判定 (true:有効 false:無効)
			char	cMeiNo[32];			// 命令No
			char	cCoilNo[32];		// 生産No
			int		nAtuSei[2];			// 注文板厚[μm] 0:下限 1:上限
			char	cKenKijun[32];		// 検査基準
			char	cMuke[32];			// 向先
			char	cYouto[32];			// 用途
			char	cSyuZai[32];		// 主材質
			char	cOmoteSyori1[32];	// 表面処理1桁目
			char	cHinCode[32];		// 品種
		};

		// 実体
		struct INF
		{
			// マスター
			BOOL	bUmu;							// 有効判定 (true:使用 false:未使用)				

			// 条件設定
			JUDGE	typJudge[MAX_CODE_JUDGE];		// 条件設定
		} rec[MAX_PCODE];							// 最終は、その他項目固定

		// 優先度と配列位置の紐付け
		int		nPriority[MAX_PCODE];				// 優先度順にpcode(=配列Index)が入っている
		int		nMasterNum;							// 有効なマスター数
	};

//// 公開関数
public:
	ParamManager(void);
	virtual ~ParamManager(void);

	//=========================================
	// パラメータ参照
	inline const P_COMMON_MAST&		GetCommonMast()			const {return (P_COMMON_MAST&)m_CommonMast;}
	inline const P_SCODE_OUTPUT&	GetScodeOutput()		const {return (P_SCODE_OUTPUT&)m_ScodeOutput;}
	inline const P_PCODE_OUTPUT&	GetPcodeOutput()		const {return (P_PCODE_OUTPUT&)m_PcodeOutput;}


	//=========================================
	// パラメータ取得関連 (同期)
	bool ReadParam_All();												// 全パラメータ読み込み
	bool Read_CommonMast();												// 疵検定数パラメータ読み込み
	bool Read_ScodeOutput();											// 検査表面状態パラメータ読込
	bool Read_PcodeOutput();											// 検査パターン別パラメータ読込


//// メンバー関数
private:
	//// ワーカースレッドとして動作させないため、関係関数を非公開に変更
	//int						ThreadFirst()				{return 0;}		// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	//int						ThreadLast()				{return 0;}		// スレッド終了前処理 (終了シグナル後にコールバック)
	//void					ThreadEvent(int nEventNo)	{;}				// スレッドイベント発生

	//=========================================
	// パラメータ
	P_COMMON_MAST			m_CommonMast;								// 疵検定数
	P_SCODE_OUTPUT			m_ScodeOutput;								// 検査表面状態	
	P_PCODE_OUTPUT			m_PcodeOutput;								// 検査パターン別
};
