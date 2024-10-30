#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー

#include "..\..\Include\LineCommon.h"									// 疵検インクルード

// 疵検ライブラリ インクルード
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC操作クラス

#include "MainConst.h"

using namespace KizuLib;
using namespace KizuODBC;

class ParamManager :	public ThreadManager
{
public:
	//=======================================
	// 疵検定数 構造体
	//=======================================
	struct P_COMMON_MAST
	{
		int nMapDsp[MAP_STAGE_NUM];		// マップ間隔 [システム最小, 小, 大] [m]

		// 設備長_SPM_裏検査台
		int nSpmToUraCamera;

		// 設備長_裏検査位置_表検査位置
		int nCameraToCamera;

		// 設備長_検査基点_上流カメラ
		int nBaseToCamera;

		// 設備長_検査基点_SPM
		int nBaseToSpm;

		// 設備長_検査位置～表示基点[mm]
		int nCameraToDsp;

		// 設備長_表示基点_裏検査台
		int nDspToUraCamera;

		// 設備長_表示基点_表検査台
		int nDspToOmoteCamera;

		// 設備長_表示基点_シャー
		int nDspToCut;

		// 重欠陥一覧画面_フィルタ（0：詳細フィルタ  1:簡易フィルタ）
		int nDeflistFilter;
	};

	//=======================================
	// 疵種 構造体
	//=======================================
	struct P_TYPE_NAME
	{
		int num;						// 総個数
		struct TYPE_ID
		{
			int edasysID;				// edasysID
			int priority;				// 表示優先度 (1が最強)
		
			bool bEnable[MAX_IMAGE_CNT];	// 画像毎の表示有無
		} type_id[MAX_TYPE];
	};

	//=======================================
	// グレード 構造体
	//=======================================
	struct P_GRADE_NAME
	{
		int num;						// 総個数
		struct GRADE_ID
		{
			int edasysID;				// edasysID
			int priority;				// 表示優先度 (1が最強)
			int nDefDiv;				// 欠陥グレード 0:無し 1:軽欠陥 2:中欠陥 3:重欠陥
		} grade_id[MAX_GRADE];
	};


	//=======================================
	// 検査パターン別 構造体
	//=======================================
	enum EM_OUTPUTFLG								// 出力条件		(優先度があるものは、必ず優先度の高い順に並べておく事)
	{
		OUTPUTFLG_DSP_O = 0,							// 探傷画面 表
		OUTPUTFLG_DSP_U,								// 探傷画面 裏 (必ず 表の連番であること)
		OUTPUTFLG_MAP,									// 疵マップ画面表示
		//OUTPUTFLG_KEI_O,								// マーキング指示 軽欠陥 表
		//OUTPUTFLG_KEI_U,								// マーキング指示 軽欠陥 裏
		//OUTPUTFLG_JUU_O,								// マーキング指示 重欠陥 表
		//OUTPUTFLG_JUU_U,								// マーキング指示 重欠陥 裏
		//OUTPUTFLG_LST_O,								// マーキング指示 重欠陥 表
		//OUTPUTFLG_LST_U,								// マーキング指示 重欠陥 裏

		OUTPUTFLG_END									//(最後としても使っているので注意)
	};

	struct P_PCODE_OUTPUT
	{
		// 出力条件
		struct OUTPUT								// 出力項目
		{
			int		edasysID;							// 疵種edasysID (1～:疵種ID  0:未使用  -1:その他項目)
			
			int		kind[OUTPUTFLG_END];				// グレードID (0:無し 1～:グレードedasysID) 指定されたグレードID以上の疵を出力対象
																// 配列位置は、[EM_OUTPUTFLG] を使用する事
		};

		// 実体
		struct INF
		{
			// マスター
			BOOL	umu;							// 有効判定 (true:使用 false:未使用)				
			
			// 出力設定
			int		nPutNum;						// outputにセットしている総個数
			OUTPUT	output[MAX_TYPE];				// 出力条件設定
			OUTPUT	defput;							// 疵種未設定項目の出力条件

		} rec[MAX_PCODE];							// 最終は、その他項目固定
	};

	//=======================================
	// 検査パターン共通 構造体
	//=======================================
	enum EM_PALLOUT								// 出力条件		(優先度があるものは、必ず優先度の高い順に並べておく事)
	{
		PALLOUT_DETAIL = 0,							// 重欠陥一覧画面表示_詳細条件
		PALLOUT_SIMPLE,								// 重欠陥一覧画面表示_簡易条件

		PALLOUT_END									//(最後としても使っているので注意)
	};

	struct P_PALL_OUTPUT
	{
		// 出力条件
		struct OUTPUT								// 出力項目
		{
			int		edasysID;							// 疵種edasysID (1～:疵種ID  0:未使用  -1:その他項目)

			int		kind[PALLOUT_END];					// グレードID (0:無し 1～:グレードedasysID) 指定されたグレードID以上の疵を出力対象
														// 配列位置は、[EM_PALLOUT] を使用する事
		};

		// 実体
		// マスター
		// 実体
		struct INF
		{
			// 出力設定
			int		nPutNum;						// outputにセットしている総個数
			OUTPUT	output[MAX_TYPE];				// 出力条件設定
			OUTPUT	defput;							// 疵種未設定項目の出力条件
		} inf;
	};

//// 公開関数
public:
	ParamManager(void);
	virtual ~ParamManager(void);

	//=========================================
	// パラメータ参照
	inline const P_COMMON_MAST&			GetCommonMast()			const {return (P_COMMON_MAST&)m_CommonMast;}
	inline const P_TYPE_NAME&			GetTypeName()			const {return (P_TYPE_NAME&)m_TypeName;}
	inline const P_GRADE_NAME&			GetGradeName()			const {return (P_GRADE_NAME&)m_GradeName;}
	inline const P_PCODE_OUTPUT&		GetPcodeOutpu()			const {return (P_PCODE_OUTPUT&)m_PcodeOutput;}

	//=========================================
	// パラメータ取得関連 (同期)
	bool ReadParam_All(bool init=false);								// 全パラメータ読み込み

	bool Read_CommonMast();												// 疵検定数パラメータ読込
	bool Read_TypeName();												// 疵種パラメータ読込
	bool Read_GradeName();												// グレードパラメータ読込
	bool Read_PcodeOutput();											// 検査パターン別パラメータ読込
	bool Read_PallOutput();												// 検査パターン共通パラメータ読込

	//bool Read_MaeMaster();												// 前工程マスターテーブル読込
	//bool Read_MaeTypeName();											// 前工程疵種マスターテーブル読込
	//bool Read_MaeGradeName();											// 前工程疵グレードマスターテーブル読込

	//=========================================
	// 出力条件チェック
	bool CheckEdasysID_T(long tid) const;								// 疵種EdasysID の有無
	bool CheckEdasysID_G(long gid) const;								// グレードEdasysID の有無

	int GetYusenT(long tid) const;										// 疵種優先度取得
	int GetYusenG(long gid) const;										// グレード優先度取得
	int GetDefDivG(long gid) const;

	//int GetMaeYusenT(char* cProc, int nTid) const;						// 前工程疵種優先度取得
	//int GetMaeYusenG(char* cProc, int nGid) const;						// 前工程グレード優先度取得

	//int	GetMaeTID(char* cProc, char* cType);
	//int	GetMaeGID(char* cProc, char* cGrade);
	//bool CheckDispDef(char* cProc, char* cType, char* cGrade);

	bool Check_Output(EM_OUTPUTFLG output, long pcode, long tid, long gid) const;		// 検査パターン出力対象の欠陥か判定
	bool Check_ImgDsp(long tid, int index) const;						// 疵種に対して各角度が表示かチェック
	bool Check_PallOutput(EM_PALLOUT output, long tid, long gid) const;


	// テスト用
	// ランダムで疵EdasysIDを取得する
	int ParamManager::GetRandTid(int pcode);


//// 公開変数
public:

	// 受け渡しキュー
	ThreadQueue<COMMON_QUE>	gque_Deli;									// パラメータ変更通知受け渡しキュー


//// メンバー関数
private:
	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast()	{return 0;};									// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);										// スレッドイベント発生
	

	void ExecParam(COMMON_QUE* pQue);									// パラメータ読込制御



//// メンバー変数
private:

	//=========================================
	// パラメータ
	P_COMMON_MAST				m_CommonMast;							// 疵検定数
	P_TYPE_NAME					m_TypeName;								// 疵種
	P_GRADE_NAME				m_GradeName;							// グレード
	P_PCODE_OUTPUT				m_PcodeOutput;							// 検査パターン別
	P_PALL_OUTPUT				m_PallOutput;							// 検査パターン共通

};
