#pragma once

// 共通ヘッダー
#include "..\..\Include\KizuCommon.h"									// システム用インクルード

// STL
#include <math.h>

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuLib\Class\AutoLock.h"						// クリティカルセクション制御クラス

#include "..\..\Library\KizuLibMFC\CLASS\CsvIniFunc.h"					// CSVクラス

// 疵検ライブラリ インクルード
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC操作クラス

// ローカルのクラス
#include "MainConst.h"


using namespace KizuLib;
using namespace KizuODBC;

class ParamManager : public ThreadManager
{
//// 公開構造体
public:

	//=======================================
	// iniファイル 設定構造体
	struct P_MAIN
	{
		// ini ----------
		int			nIns;					// INS_NUM				// 今回のインスタンス数 1固定
		//int			nFrameSizeX;			// FRAME_SIZE_X			// 個々のフレームサイズ
		//int			nFrameSizeY;			// FRAME_SIZE_Y


		int			nExecCoilStart;						// 0:コイル情報受信で自動検査開始  -1:自動開始しない

		char		cWriteSecPath[64];					// セクター単位の書込み時のセクタサイズ取得パス (指定するとセクタ単位の書込み。未設定普通の書込み)
		char		cSystemName[64];					// システム略称 (RPIファイルの中に埋め込む)
		int			nHddFreeSizeSave;					// 全長画像保存PCのHDD最低空き容量 [GB]
		int			nHddFreeSizeLocal;					// ローカルPCのHDD最低空き容量 [GB]

		int			nAddFrameInfoType;					// フレーム付加情報切替 (0:無し 1:T_RPI_FRAME_INFO_T1)

		// 数
		int			nLoopQueRecvSize;					// LOOP_QUE_RECV_SIZE	// 判定からのRAW画像受け渡しキュー
		int			nLoopQueFrameSize;					// 連結画像用 回転キューサイズ (1つで、フレーム画像×幅方向分メモリを消費)
		int			nThreadUnionNum;					// 連結用スレッドの並列実行数
		int			nThreadCompNum;						// 圧縮用スレッドの並列実行数


		// 画面
		int			nDispWindScaleX;					// 元画像からの縮尺倍率
		int			nDispWindScaleY;
		int			nDispWindPosX;
		int			nDispWindPosY;						// ズラシ量 [pixel]
		int			nDispPrint;							// 画像データにデバック情報付与 (0:無し  1:左端にFno,Ino付与  2:1＋中央にIportシーケンスNo付与)




		// 固定で保持しておく必要あり
		EM_COLOR_TYPE	emColorType;					// 画像タイプ (0:グレー画像  1:カラー画像  2:色差画像)
		EM_SYS_TYPE		emSysType;						// 疵検タイプ (0:連続ライン  1:バッチライン)
		int				nImgNumTotal;					// 1画像から生成される最終的な画像数	(色差の場合4)



		//// 以降 全て間引き座標系
		// 計算 ----------
		//int			nCullSizeX;							// 間引き後１フレームサイズX
		//int			nCullSizeY;							// 間引き後１フレームサイズY
		int			nFrameSizeX;			// FRAME_SIZE_X			// 個々のフレームサイズ(縮小率反映)
		int			nFrameSizeY;			// FRAME_SIZE_Y
		int			nGrabSizeX;				// GRAB_SIZE_X			// 個々の撮像フレームサイズ(縮小率反映)
		int			nGrabSizeY;				// GRAB_SIZE_Y


		int			nMaxWidth;							// 最大のフレーム画像連結後の画像幅		※領域確保のため
		int			nMaxHeight;							// 最大のフレーム画像連結後の画像高さ
		int			nBmpHeadSize;						// ビットマップヘッダサイズ

		int			nStateId;							// 機器ID


		// ///////////////////////////
		// インスタンス単位
		struct	INS
		{
			// コイル単位でパラメータ読込 ----------
			//   ※ 絶対に直接参照禁止。フレームと一緒に渡ってきた値を使用すること
			//   ※ 内部的にはins単位で持つように変更
			EM_COMP_TYPE	emCompType;					// 画像圧縮タイプ
			int				nCompQualityBase;			// 圧縮率 (0～100%)
			int				nCompQuality[MAX_PCODE];	// PCODE単位の圧縮率設定値 (0の場合はベースを参照)



			// ini ----------
			int		nStartCamset;						// カメラセット開始番号 (1オリジン)
			int		nCamsetNum;							// インスタンスあたりのカメラセット数
			int		nCamAngle;							// カメラ角度 (1オリジン)
			int		nMen;								// 表裏区分 (0:表 1:裏)
			bool	bWidthSync;							// 連結後画像幅同期有無
			bool	bImgTorB;							// 画像上下反転　(0:なし　1:反転) BMPヘッダーで仮想的に反転 

			char	cRawSavePath[128];					// 生画像保存パス (最後の\は無し)
			int		nRpiFolderBufNum;
			char	cRpiFolderBuf[MAX_FOLDER_SAVE_NUM][128];	// 一次生成用RPIファイル保存フォルダ (最後の\は無し)
			int		nRpiFolderSaveNum;
			char	cRpiFolderSave[MAX_FOLDER_SAVE_NUM][128];	// 長期間保存用RPIファイル保存フォルダ (最後の\は無し)

			// 連結画像関係
			int		nOverLapMode;						// 左右フレームのオーバーラップ部結合条件
															// (-1:連結のみ。ｵｰﾊﾞｰﾗｯﾌﾟ無し  0:ｽﾑｰｼﾞﾝｸﾞ  1:左ｶﾒﾗﾌﾚｰﾑ採用  2:右ｶﾒﾗﾌﾚｰﾑ採用)
			int		nImgDelete[2];						// 無条件フレーム端 塗り潰し範囲 [pixel] (左右の端から 指定画素分の情報を削除) (0:左 1:右)
			bool	bEdgeDeleteEnabel;					// エッジ外塗り潰し有無 (false:無し true:有り)
			int		nEdgeDelete[2];						// エッジ外＋α分 塗り潰し範囲 [pixel] (左右のエッジから 指定画素分から外の情報を削除) (0:左 1:右)



			//// 以降 全て間引き座標系
			// 計算 ----------
			int		nUionWidth;							// フレーム画像連結後の画像幅
			int		nUionHeight;						// フレーム画像連結後の画像高さ
			BYTE	BmpHead[2048];						// ビットマップヘッダポインタ (いろいろ大変だから、少し大きめの固定サイズにしておく)

			float	reso_x;								// 対象カメラの平均横分解能
			float	reso_y;								// 対象カメラの縦分解能

			int		nLineCenter;						// 画像連結後フレーム左端からラインセンター位置までのオフセット値 [pixel]
			int		nInsertRight;						// 右端画素挿入量 [pixel]

		
			struct CAMSET
			{
				// ini ----------
				// 輝度補正
				bool	bKidoUmu;						// 輝度補正有効無効

				// 計算 ----------
				double	dResWidth;						// 横分解能 [mm/pixel]

				//// 自フレームと次のフレームが重なる
				int		nBaseLeft;						// 画像連結後フレーム左端から自カメラ画像左端までのオフセット値

				int		nOverLap;						// 次カメラとの右側のオーバーラップ量 [pixel] (0のとき、オーバーラップ無し)
				int		nLapStartBase;					// 自フレーム左端基準でのオーバーラップ開始位置
															// nOverLap + nLapStartBase = フレームサイズとなる

				int		nOverLapL;						// 前カメラとの左側のオーバーラップ量 [pixel]
				int		nLapStartBaseResult;			// 最終のオーバーラップ開始位置

				// 最終結果
				int	nCopyImg[2];						// 単純にコピー画像部分(自フレームの開始pixel、コピーサイズ)
				int nHokanImg[2];						// 直線保管する画像部分(自フレームの開始pixel、コピーサイズ)

			} camset[MAX_CAMSET];						// 実際値がセットされるのは、 [m_Ini.nCamsetNum] 分
		} ins[MAX_INS];
	};
	

	//=======================================
	// カメラセット別 パラメータ 構造体
	struct P_PARAM_CAMSET
	{
		// 面単位で入っている
		struct	INS {
			struct CAMSET
			{		
				int		nCamset;							// セットされているカメラセット番号 (1オリジン)
				double	dResWidth;							// 横分解能 [mm/pixel]
				double	dXoffset;							// カメラ原点 X [mm]
				

				bool	bIsEnable;							// 有効カメラ時 true。※ 幅方向カメラ無しでも動く対策
			} camset[MAX_CAMSET];					// 実際値がセットされるのは、 [m_Ini.nCamsetNum] 分
		} ins[MAX_INS];								// 実際値がセットされるのは、 [m_Ini.nIns] 分
	};


	//=======================================
	// 共通パラメータ 構造体
	struct P_PARAM_COMMON
	{
		double	dPls;									// パルスレート	
													// カメラコントローラ設定 (0:ﾗｲﾝPLG)
		int		nMul;									// 逓倍
		int		nDiv;									// 分周


		// 間引き関係
		int			nImgCullX;							// 画像間引き数。静的。 ※ 1/x を指定
		int			nImgCullY;

	};


//// 公開関数
public:
	ParamManager(void);
	virtual ~ParamManager(void);


	//=========================================
	// イベント公開
	HANDLE		GetEvPrm_ReadEnd() const { return m_evReadEnd; }		// パラメータ読み込み完了通知



	//=========================================
	// パラメータ参照
	inline const P_MAIN&				GetMain()				const {return (P_MAIN&)m_Main;}

	inline const P_PARAM_CAMSET&		GetParamCamSet()		const {return (P_PARAM_CAMSET&)m_Param_CamSet;}
	inline const P_PARAM_COMMON&		GetParamCommon()		const {return (P_PARAM_COMMON&)m_Param_Common;}

	inline const std::vector<short>&	GetKido(int nIns, int nCam)	const { return m_vKido[nIns][nCam]; }

	double	CalcResV();													// 計算値縦分解能


	//=========================================
	// パラメータ取得関連 (同期)
	bool ReadParam_All(bool bStatic = false);							// 全パラメータ読み込み

	void Read_IniCoilChange(int nIns);									// コイル単位でパラメータ読込直し
	void Read_IniKidoChange(int nIns);									// 輝度値更新




	//=========================================

	// 座標変換
	int CalcCullX(int nFX) { return nFX/m_Param_Common.nImgCullX; }			// フレーム座標系を間引き座標系に
	int CalcCullY(int nFY) { return nFY/m_Param_Common.nImgCullY; }





private:
	bool Read_Ini(P_MAIN& buf, bool bStatic = false);					// iniファイル読み込み
	bool Read_Param_CamSet(P_MAIN& buf);								// カメラセット別 パラメータ 読込
	bool Read_Param_Common();											// 共通パラメータ読込
	bool Read_Param_CamEnable(P_MAIN& pm);								// 有効カメラ取得


//// 公開変数
public:
	// 受け渡しキュー
	ThreadQueue<COMMON_QUE>	gque_Deli;									// パラメータ変更通知受け渡しキュー


//// メンバー関数
private :
	int				ThreadFirst();										// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int				ThreadLast()	{return 0;};						// スレッド終了前処理 (終了シグナル後にコールバック)
    void			ThreadEvent(int nEventNo);							// スレッドイベント発生

	void			ExecParam(COMMON_QUE* pQue);						// パラメータ読込制御
	void			CalcParam(P_MAIN& buf);								// 取得したパラメータから 必要な情報を算出する


//// メンバー変数
private :
	//=========================================
	// シグナル (外部操作)
	HANDLE						m_evReadEnd;							// パラメータ読み込み完了

	//=========================================
	// パラメータ
	P_MAIN						m_Main;									// Iniファイル等の設定情報
	P_PARAM_CAMSET				m_Param_CamSet;							// カメラセット別 パラメータ
	P_PARAM_COMMON				m_Param_Common;							// 共通パラメータ

	std::vector<short>			m_vKido[MAX_INS][MAX_CAMSET];			// 輝度補正値 [1カメラ 幅pixel分] ※バイトにしないのはマイナス補正対応のため

};
