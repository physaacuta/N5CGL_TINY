#pragma once

// 半固定
#include "..\..\Include\LineCommon.h"									// φωインクルード

// φωクラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLib\Class\AutoLock.h"						// クリティカルセクション制御クラス

// φωライブラリ インクルード
#include "..\..\Library\KizuODBC\OdbcBase.h"

// ファンクションクラス
#include "ToOutMailSender.h"


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
		// RAS関係
		struct _RAS {
			int	nColorNorm;								// [RAS正常色] RAS表示画面の機器正常時の色 （ARGB形式）
			int	nColorErr;								// [RAS異常色] RAS表示画面の機器異常時の色 （ARGB形式）
			int	nColorWarn;								// [RAS注意色] RAS表示画面の機器注意時の色 （ARGB形式）
		} Ras;

		// LOG関係
		struct _LOG {
			int	nDispNum;								// [LOG表示件数] ログ表示画面の最大ログ表示件数 （あくまでデフォルト値）
			int	nEventColor;							// [LOGイベント色] ログ表示画面のイベント行の表示文字色 （ARGB形式）
			int	nFailColor_Ju;							// [LOG重故障色] ログ表示画面の重故障行の表示文字色 （ARGB形式）
			int	nFailColor_Kei;							// [LOG軽故障色] ログ表示画面の軽故障行の表示文字色 （ARGB形式）
		} Log;

		// 探傷画面表示
		struct _TANSHO {
			int	nDspSectionMin;							// [探傷画面表示区間小] 探傷画面の表示区間設定値 （最小5mの倍数をセットする事）
			int	nDspSectionMax;							// [探傷画面表示区間大] 探傷画面の表示区間設定値 （最小5mの倍数をセットする事）
		} Tansho;

		int		walm;									// [ウォーマ材指定位置] 検査パターンでのウォーマ材設定レコード位置																											
		int		nJpegQuality;							// [Jpeg切り出し画像品質] 切り出し画像のJPEG圧縮品質[%]
		int		nCamTempThr;							// [カメラ温度閾値] カメラ温度異常となる閾値[℃]
		
		// エッジ整合性チェック
		struct _EDGE{
			int		nType;								// [エッジ整合性_実施種別] 0:無し   1：整合性判定のみ  2：整合性判定＋エッジ範囲指定
			float	dDetectRange;						// [エッジ整合性_検出範囲] この値の1/2の位置を基点に、前後範囲とする[mm]
			float	dDetectRangeR;						// [エッジ整合性_リカバリー検出範囲] この値の1/2の位置を基点に、前後範囲とする[mm]
			float	dWidthDiff;							// [エッジ整合性_板幅許容誤差] 上位情報の板幅と検査装置板幅の許容誤差[mm]
			float	dPerEdgeDiff;						// [エッジ整合性_前フレームとの許容誤差] 片エッジ優先時の、前エッジ位置と今回エッジ位置の許容誤差[mm]
			int		nFromSkip;							// [エッジ整合性_溶接後不感帯] コイル切替後、このフレーム数分の間は、エッジ整合性判定を実施しない[フレーム]
			int		nFFErr;								// [エッジ整合性_エッジ検出異常FF送信] この閾値以上の場合、FFにエッジ未検出区間有りで送信。(0：送信無) [TO_DEFECT]
		} Edge;

		// エッジ検出
		struct _EDGE_RESTRICT{
			int		nType;								// [エッジ検出_実施種別] 0:無し   1：統括エッジ範囲指定  2：エッジ検出器範囲指定
			float	dDetectRange;						// [エッジ検出_検出範囲] この値の1/2の位置を基点に、前後範囲とする[mm]
		} Edge_Restrict;

		// 重複疵マップ
		struct _DEF_MAP {
			int		nBGColor_Kei;						// [重複疵マップ背景色_軽] 探傷画面・通板実績画面 重複疵マップ 軽背景色 （ARGB形式）
			int		nBGColor_Tyu;						// [重複疵マップ背景色_中] 探傷画面・通板実績画面 重複疵マップ 中背景色 （ARGB形式）
			int		nBGColor_Ju;						// [重複疵マップ背景色_重] 探傷画面・通板実績画面 重複疵マップ 重背景色 （ARGB形式）
			int		nDefCntThr_Kei;						// [重複疵マップ疵個数閾値_軽] 探傷画面・通板実績画面 重複疵マップの疵個数 軽閾値
			int		nDefCntThr_Tyu;						// [重複疵マップ疵個数閾値_中] 探傷画面・通板実績画面 重複疵マップの疵個数 中閾値
			int		nDefCntThr_Ju;						// [重複疵マップ疵個数閾値_重] 探傷画面・通板実績画面 重複疵マップの疵個数 重閾値
		} DefMap;
			
		// 設備長
		struct _DISTANCE{
			int		nKenBase;							// [設備長_検査基点_上流カメラ]	検査基点～上流カメラまでの距離[mm]
			int		nKenBaseSpm;						// [設備長_検査基点_SPM] 検査基点～SPMまでの距離[mm]
			int		nDspBaseKensaBot;					// [設備長_表示基点_裏検査台] 表示基点～裏検査台位置までの距離[mm]
			int		nDspBaseKensaTop;					// [設備長_表示基点_表検査台] 表示基点～表検査台位置までの距離[mm]
			int		nDspSharCut;						// [設備長_表示基点_シャー] 表示基点～出側シャーカット位置までの距離[mm]
			int		nKenBaseGate;						// [設備長_検査基点ゲート信号ON_検査基点] 検査基点ゲート信号ON～検査基点WPDまでの距離[mm]
			int		nDspBaseGate;						// [設備長_表示基点ゲート信号ON_表示基点] 表示基点ゲート信号ON～表示基点WPDまでの距離[mm]
		} Dist;

		int			nNoLamp[2];							// [疵接近ランプ_不感帯] 溶接点前後の不感帯設定。[先端/尾端]
		int			nKizuLamp[2];						// [疵接近ランプ_出力区間] 疵接近ランプ出力区間。パトライト出力開始位置。[表検査台手前Xm。/表検査台通過後Xm。]
		int			nNoAlarm[2];						// [疵接近アラーム_不感帯] 溶接点前後の不感帯設定。[先端/尾端]
		int			nKizuAlarm[2];						// [疵接近アラーム_出力区間] 疵接近アラーム出力区間。パトライト出力開始位置。[裏検査台手前Xm。/裏検査台通過後Xm。]
		int			nVoicePos;							// [ボイス_出力位置_前] ボイス出力位置。表検査台手前Xm。

		int			nRPISaveColor_R;					// [全長画像_保存_R] 全長画像を保存するか？  0:無し 1:有り  ※無し場合、判定→全長処理間のデータも送らない
		int			nRPISaveColor_G;					// [全長画像_保存_G] 全長画像を保存するか？  0:無し 1:有り  ※無し場合、判定→全長処理間のデータも送らない
		int			nRPISaveColor_B;					// [全長画像_保存_B] 全長画像を保存するか？  0:無し 1:有り  ※無し場合、判定→全長処理間のデータも送らない
		int			nRPISaveColor_Sa;					// [全長画像_保存_差] 全長画像を保存するか？  0:無し 1:有り  ※無し場合、判定→全長処理間のデータも送らない

		float		dPlgDiffLimit[2];					// [PLG距離異常上限値] 検査用PLGと比較用PLGを比較して、異常認識となる最大誤差距離[比較用/表示用]
		float		dProLenDiffLimit;					// [プロコンコイル長比較異常上限値] 中央検査装置とプロコンのコイル長（SPM圧延後）を比較して、異常認識となる最大誤差距離（0ならチェックなし）
		int			nLogiParaCam[2][2];					// [ロジックパララン機能xx_入力カメラxx] 対象のロジックパララン機能に割り付いているカメラNo（0：未選択　1～：カメラNo）
	};


	//=============================
	// 疵種　構造体
	//=============================
	struct P_TYPE_NAME
	{
		int			num;								// 総個数		
		struct _TYPE_ID
		{
			int		nEdasysID;							// [疵種EdasysID]
			int		nPriority;							// [優先度]
			
			bool bEnable[MAX_IMAGE_CNT];				// 画像毎の表示有無
		} Type_Id[MAX_TYPE];
	};


	//=============================
	// グレード　構造体
	//=============================
	struct P_GRADE_NAME
	{
		int			num;								// 総個数
		struct _GRADE_ID
		{
			int		nEdasysID;							// [グレードEdasysID]
			int		nPriority;							// [優先度]
			int		rank;								// [欠陥グレード] (0:無害 1:軽欠陥 2:中欠陥 3:重欠陥)
		} Grade_Id[MAX_GRADE];
	};


	//=============================
	// 検査表面状態　構造体
	//=============================
	struct P_SCODE_OUTPUT
	{
		// 出力条件
		struct OUTPUT									// 出力項目
		{
			int		edasysID;							// [疵種edasysID] (1～:疵種ID  0:未使用  -1:その他項目)
			int		dummy;								// ダミー出力
		};

		// 実体
		struct INF
		{
			// マスター
			BOOL	umu;								// [有効判定] (true:使用 false:未使用)				

			//// 出力設定
			//int		nPutNum;						// outputにセットしている総個数
			//OUTPUT	output[MAX_TYPE];				// 出力条件設定
			//OUTPUT	defput;							// 疵種未設定項目の出力条件

		} rec[MAX_SCODE];								// 最終は、その他項目固定
	};


	//=======================================
	// 検査パターン別 構造体
	//=======================================
	enum EM_OUTPUTFLG									// 出力条件		(優先度があるものは、必ず優先度の高い順に並べておく事)
	{
		OUTPUTFLG_DSP_O = 0,							// 探傷画面表示	表
		OUTPUTFLG_DSP_U,								//				裏

		OUTPUTFLG_DSP_DEFMAP,							// 疵マップ画面表示

		OUTPUTFLG_FF_SEND,								// FF送信有無

		OUTPUTFLG_ALARM_RANP_LOW,						// 疵接近	軽
		OUTPUTFLG_ALARM_RANP_HIGH,						//			重
		
		OUTPUTFLG_PATOLOGHT,							// パトライト出力

		OUTPUTFLG_VOICE_SEQ,							// ボイス連続疵
		OUTPUTFLG_VOICE_HIGH,							// ボイス重大疵
		OUTPUTFLG_VOICE_HEGE,							// ボイスヘゲ
		OUTPUTFLG_VOICE_SURI,							// ボイススリバー
		OUTPUTFLG_VOICE_SAME,							// ボイス表裏同一疵
		OUTPUTFLG_VOICE_CYCLE,							// ボイス周期疵
		OUTPUTFLG_VOICE_OTHER,							// ボイスその他疵
		
		OUTPUTFLG_LOWGR_DB_O,							// DB保存_一般	表
		OUTPUTFLG_LOWGR_DB_U,							//			裏
		OUTPUTFLG_DB_O,									// DB保存_優先	表 
		OUTPUTFLG_DB_U,									//				裏


		OUTPUTFLG_END									//(最後としても使っているので注意)
	};

	struct P_PCODE_OUTPUT
	{
		// 出力条件
		struct OUTPUT							// 出力項目
		{
			int		edasysID;							// 疵種edasysID (1～:疵種ID  0:未使用  -1:その他項目)
			
			int		kind[OUTPUTFLG_END];				// グレードID (0:無し 1～:グレードedasysID) 指定されたグレードID以上の疵を出力対象
														// 配列位置は、[EM_OUTPUTFLG] を使用する事
		};

		// 実体
		struct INF
		{
			// PCODE_MASTERから取得
			BOOL	umu;								// [有効判定] (true:使用 false:未使用)				
			
			// PCODE_OUTPUTから取得
			int		nPutNum;							// outputにセットしている総個数
			OUTPUT	output[MAX_TYPE];					// 出力条件設定
			OUTPUT	defput;								// 疵種未設定項目の出力条件

		} rec[MAX_PCODE];								// 最終は、その他項目固定
	};


	//=======================================
	// 検査パターン共通 構造体 (PALL_OUTPUT)
	//=======================================
	enum EM_OUTPUT_PALLFLG						// 出力条件		(優先度があるものは、必ず優先度の高い順に並べておく事)
	{
		OUTPUTPALLFLG_DEFLIST_DETAIL = 0,				// 重欠陥一覧画面表示_詳細条件
		OUTPUTPALLFLG_DEFLIST_SIMPLE,					// 重欠陥一覧画面表示_簡易条件
		OUTPUTPALLFLG_DEFLIST_SIMPLE_LIST,				// 重欠陥一覧画面表示_簡易項目
	
		OUTPUTPALLFLG_END								//(最後としても使っているので注意)
	};

	struct P_PALL_OUTPUT
	{
		// 出力条件
		struct OUTPUT							// 出力項目
		{
			int		edasysID;							// 疵種edasysID (1～:疵種ID  0:未使用  -1:その他項目)
			
			int		kind[OUTPUTPALLFLG_END];			// グレードID (0:無し 1～:グレードedasysID) 指定されたグレードID以上の疵を出力対象
														// 配列位置は、[EM_OUTPUTFLG] を使用する事
		};

		// 出力設定
		int nPutNum;							// outputにセットしている総個数
		OUTPUT	output[MAX_TYPE];				// 出力条件設定
		OUTPUT	defput;							// 疵種未設定項目の出力条件
	};


	//=======================================
	// 検査パターン固有 構造体
	//=======================================
	struct P_PCOMMON_OUTPUT
	{
		// 出力条件
		struct OUTPUT								// 出力項目
		{
			int		nStopMarkFFSend;					// ストップマークFF送信有無

		} rec[MAX_PCODE];
	};

	//=======================================
	// 共通パラメータ 構造体
	//=======================================
	struct P_PARAM_COMMON
	{
		// PLG情報
		double	dPls;						// PLGパルスレート[mm/pls]
		double	dPlsSub;					// PLGパルスレート比較[mm/pls]
		double	dPlsDsp;					// PLGパルスレート表示[mm/pls]
		//struct _CNTSET{
		//	int		nPlgMul;				// カウンタボードPLG逓倍率
		//	int		nPlgPulse;				// カウンタボードPLG信号特性[0：差動 1：TTL]
		//	int		nPlgPhase;				// カウンタボードPLG位相[0：2相 1：単相]
		//	int		nPlgPhaseDiff;			// カウンタボードPLG位相差[0：ｱｯﾌﾟｶｳﾝﾄ 1：ﾀﾞｳﾝｶｳﾝﾄ]
		//	int		nPlgFilter;				// カウンタボードPLGデジタルフィルタ
		//} typCntSet[1];

		int			nCamTap;				// カメラタップ数

		int			nDistPosition;			// 表裏間基準面[0:表  1:裏]
		double		dDistLen;				// 表裏間距離[mm]

		int			nExSyncIntval;			// EXSYNC最小周期[25ns]
		int			nExSyncMinH;			// EXSYNC最小H時間[25ns]
		int			nExSyncMinL;			// EXSYNC最小L時間[25ns]
		//BOOL		nAGCbyCamcont;			// 統括AGC ON/OFF[TRUE:AGC有効,FALSE:AGC無効]

		double		dLapRate;				// 外接長方形の連なり[%]

#ifndef DISABLE_GAP_CALC
		BOOL	nUseWarpProj;							// 射影変換眼間ズレ補正 True:ズレ補正実施	
#endif

		// カメラコントローラ設定
		struct _CAMCONT
		{
			int		nMul;					// 低倍率
			int		nDiv;					// 分周率
		} typCamCon[4];					// [0:ラインPLG, 1:表校正, 2:裏校正, 3:テストパルス]

		int			nEdgeRestrictPort;		// エッジ制限UDP受信ポート

		int			nShFrameShrinkX;		// 共有メモリ上フレーム画像縮小X
		int			nShFrameShrinkY;		// 共有メモリ上フレーム画像縮小Y
	};
	//=======================================
	// カメラセット別 パラメータ 構造体
	//=======================================
	struct P_PARAM_CAMSET
	{
		struct _CAMSET
		{
			double	dHres;					// 幅方向分解能[mm/pixel]
			double	dXoffset;				// カメラ基準位置[mm]

#ifndef DISABLE_GAP_CALC
			// 複眼ズレ補正
			int		nGapGroup[MAX_CAMERA_GAP_WIDDIV];	// 複眼ズレ補正_幅方向分割 (幅方向分割単位でグループNoをセット) 0時は、対象外。 ※ 各幅方向分割位置が、どのグループに属するか
			int		nGapCount[MAX_CAMERA_GAP_WIDDIV];	// 複眼ズレ補正_集計実施件数 (グループ単位で件数をセット) 0時は、対象外。       ※ 各グループが、何件以上になったら複眼ズレ補正を実施するか
#endif

		} camset[MAX_CAMSET];
	};
	//=======================================
	// 検査表面状態別 パラメータ 構造体
	//=======================================
	struct P_PARAM_SCODE
	{
		struct _SCODE
		{
#ifndef DISABLE_TINY_CALC
			//// 微小欠陥
			union _tiny_weight {
				float	array[8];					// 微小疵重み
				struct {
					float	white_b_white_d;			// 白正_白乱
					float	white_b_black_d;			// 白正_黒乱
					float	black_b_black_d;			// 黒正_黒乱
					float	black_b_white_d;			// 黒正_白乱
					float	white_b;					// 白正
					float	black_b;					// 黒正
					float	white_d;					// 白乱
					float	black_d;					// 黒乱
				};
			} dTinyWeight;

			float	dTinyLimit[2][4][3];				// 微小欠陥閾値レベル(個/m^2) [0:白 1:黒][0:幅4縦4 1:幅4縦1 2:幅1縦4 3:幅1縦1][0:軽 1:中 2:重]
#else
			int ndummey;
#endif
		} scode[MAX_SCODE];
	};
	//=======================================
	// カメラセット・検査表面状態別 パラメータ 構造体
	//=======================================
	struct P_PARAM_CAMSET_SCODE
	{
		struct _SCODE
		{
			struct _CAMSET
			{
				int nEdgeDetectCam;		// エッジ検出カメラ
			} camset[MAX_CAMSET];
		} scode[MAX_SCODE];
	};
	//=======================================
	// カメラセット・カメラ角度別 パラメータ 構造体
	//=======================================
	struct P_PARAM_CAMSET_ANGLE
	{
		struct _CAMSET
		{
			struct _CAMANGLE
			{
#ifndef DISABLE_GAP_CALC
				// カメラ設定 (射影変換に必要)
				int	nXdelay;							// X遅延
				int nXinsert;							// X挿入
				int nYdelayLeft;						// Y遅延左
				int nYdelayRight;						// Y遅延右
				
				// 射影変換
				double	dProjA0;						// 射影パラメータ
				double	dProjA1;
				double	dProjA2;
				double	dProjC1;
				double	dProjC2;
#else
				int ndummey;
#endif
			} camangle[MAX_CAMANGLE];
		} camset[MAX_CAMSET];
	};
	//=======================================
	// 表面状態・カメラセット・カメラ角度別 パラメータ 構造体
	//=======================================
	struct P_PARAM_CAMSET_ANGLE_SCODE
	{
		struct SCODE
		{
			struct CAMSET
			{			
				struct CAMANGLE
				{
					//// カメラ設定
					float dCameraGain;					// カメラゲイン
				} camangle[MAX_CAMANGLE];	
			} camset[MAX_CAMSET];
		} scode[MAX_SCODE];
	};



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// イベント公開
public:
	HANDLE		GetEvPrm_ReadEnd() const { return m_evReadEnd; }		// パラメータ読み込み完了通知



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// パラメータ参照
public:
	inline const P_COMMON_MAST&				GetCommonMast()		const {return (P_COMMON_MAST&)m_CommonMast;}
	inline const P_TYPE_NAME&				GetTypeName()		const {return (P_TYPE_NAME&)m_TypeName;}
	inline const P_GRADE_NAME&				GetGradeName()		const {return (P_GRADE_NAME&)m_GradeName;}
	inline const P_SCODE_OUTPUT&			GetScodeOutput()	const {return (P_SCODE_OUTPUT&)m_ScodeOutput;}
	inline const P_PCODE_OUTPUT&			GetPcodeOutput()	const {return (P_PCODE_OUTPUT&)m_PcodeOutput;}
	inline const P_PALL_OUTPUT&				GetPallOutput()		const {return (P_PALL_OUTPUT&)m_PallOutput;}
	inline const P_PCOMMON_OUTPUT&			GetPcommonOutput()	const {return (P_PCOMMON_OUTPUT&)m_PcommonOutput;}


	inline const P_PARAM_COMMON&		GetParamCommon()		const {return (P_PARAM_COMMON&)m_Param_Common;}
	inline const P_PARAM_CAMSET&		GetParamCamSet()		const {return (P_PARAM_CAMSET&)m_Param_CamSet;}
	inline const P_PARAM_SCODE&			GetParamScode()			const {return (P_PARAM_SCODE&)m_Param_Scode;}
	inline const P_PARAM_CAMSET_SCODE&	GetParamCamSetScode()	const {return (P_PARAM_CAMSET_SCODE&)m_Param_CamSetScode;}
	inline const P_PARAM_CAMSET_ANGLE&	GetParamCamset_Angle()	const {return (P_PARAM_CAMSET_ANGLE&)m_Param_CamsetAngle;}
	inline const P_PARAM_CAMSET_ANGLE_SCODE&	GetParamCamset_Angle_Scode()	const {return (P_PARAM_CAMSET_ANGLE_SCODE&)m_Param_CamsetAngleScode;}

	inline const P_TYPE_NAME::_TYPE_ID*	GetTypeName(int tid)	const
	{
		for(int ii=0; ii<m_TypeName.num; ii++ ) {
			if( m_TypeName.Type_Id[ii].nEdasysID == tid ) return (const P_TYPE_NAME::_TYPE_ID*) &m_TypeName.Type_Id[ii];
		}
		return NULL;
	}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// パラメータ取得関連 (同期)
public:
	bool ReadParam_All();												// 全パラメータ読み込み
	
	bool Read_CommonMast();												// 定数パラメータ読込
	bool Read_TypeName();												// 疵種パラメータ読込
	bool Read_GradeName();												// グレードパラメータ読込
	bool Read_ScodeOutput();											// 検査表面状態パラメータ読込
	bool Read_PcodeOutput();											// 検査パターン別パラメータ読込
	bool Read_PallOutput();												// 検査パターン共通パラメータ読込
//	bool Read_PCommonOutput();											// 検査パターン固有パラメータ読込

	bool Read_Param_Common();											// 共通パラメータ読込
	bool Read_Param_CamSet();											// カメラセット別 パラメータ 読込
	bool Read_Param_Scode();											// 表面状態別 パラメータ 読込
	bool Read_Param_Camset_Scode();										// カメラセット・検査表面状態別 パラメータ 読込
	bool Read_Param_Camset_Angle();										// カメラセット・カメラ角度別 パラメータ 読込
	bool Read_Param_Camset_Angle_Scode();								// 表面状態・カメラセット・カメラ角度別 パラメータ 読込

	bool Read_HtGroupNum(int groupmax);									// GroupID毎の必要PC数取得


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// 出力条件チェック
public:
	bool CheckEdasysID_T(long tid) const;								// 疵種EdasysID の有無
	bool CheckEdasysID_G(long gid) const;								// グレードEdasysID の有無

	int  GetYusenT(long tid) const;										// 疵種優先度取得
	int  GetYusenG(long gid) const;										// グレード優先度取得
	int  GetRankG(long gid) const;										// 欠陥グレード取得
	bool Check_Output(EM_OUTPUTFLG output, long pcode, long tid, long gid) const;					// 検査パターン出力対象の欠陥か判定
	int  Check_OutputEx(EM_OUTPUTFLG output, long pcode, long tid, long gid) const;					// 検査パターン出力対象の欠陥か判定
	bool Check_Pall_Output(EM_OUTPUT_PALLFLG output, long tid, long gid, bool bUp = true) const;	// 検査パターン共通出力対象の欠陥か判定
	bool Check_ImgDsp(long tid, int index) const;													// 疵種に対して疵画像の表示対象かチェック

	// 良く使うもの
	double	CalcResV(int idx);											// 計算値縦分解能
	double	CalcSampBuff(int idx);										// 計算値カウンタ値(1フレーム)

	// グループ毎の必要判定PC台数を返す
	void	GetHtGroupNum(int *pHtGroupNum) { memcpy(pHtGroupNum, m_nHtGroupNum, sizeof(m_nHtGroupNum)); }
	// グループ毎の判定PC総数を返す
	void	GetHtGroupMax(int *pHtGroupMax) { memcpy(pHtGroupMax, m_nHtGroupMax, sizeof(m_nHtGroupMax)); }


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// 今回固有の公開関数
public:
	EM_OUTPUTFLG	GetOutputFlg(ENUM_VOICE_PRI_TYPE emViceDefType);	// ボイス出力条件の優先度からDB項目を取得


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// スレッドキュー
public:
	ThreadQueue<COMMON_QUE>	gque_Deli;									// パラメータ変更通知受け渡しキュー


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// メンバー関数
private:
	void	ExecParam(COMMON_QUE* pQue);								// パラメータ読込制御



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// メンバー変数
private:

	//// ハンドル
	HANDLE						m_evReadEnd;							// パラメータ読込完了

	//// パラメータ
	P_COMMON_MAST				m_CommonMast;							// φω定数
	P_TYPE_NAME					m_TypeName;								// 疵種
	P_GRADE_NAME				m_GradeName;							// グレード
	P_SCODE_OUTPUT				m_ScodeOutput;							// 検査表面状態
	P_PCODE_OUTPUT				m_PcodeOutput;							// 検査パターン別
	P_PALL_OUTPUT				m_PallOutput;							// 検査パターン共通
	P_PCOMMON_OUTPUT			m_PcommonOutput;						// 検査パターン固有

	P_PARAM_COMMON				m_Param_Common;							// 共通パラメータ
	P_PARAM_CAMSET				m_Param_CamSet;							// カメラセット別 パラメータ	
	P_PARAM_SCODE				m_Param_Scode;							// 表面状態別 パラメータ	
	P_PARAM_CAMSET_SCODE		m_Param_CamSetScode;					// カメラセット・検査表面状態別 パラメータ
	P_PARAM_CAMSET_ANGLE		m_Param_CamsetAngle;					// カメラセット・カメラ角度別 パラメータ
	P_PARAM_CAMSET_ANGLE_SCODE	m_Param_CamsetAngleScode;				// 表面状態・カメラセット・カメラ角度別 パラメータ


	int							m_nHtGroupNum[DIV_GROUP_MAX - 1];		// Group毎の判定PC必要数
	int							m_nHtGroupMax[DIV_GROUP_MAX - 1];		// Group毎の判定PC総数
};
