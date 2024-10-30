

#pragma once

// 疵検ライブラリ インクルード
#include "..\..\Library\DefectLib\KizuOpSend.h"	
#include "..\..\Library\DefectLib\DBSaveManager.h"						// DB書込み基本クラス
#include "..\..\Library\KizuLibMFC\Class\CreateSQL.h"					// SQL文生成補助クラス

// テスト画像取得用
#include "..\..\Library\ImgLib\CLASS\KizuAviManager.h"					// AVIファイル
#include "..\..\Library\KizuLib\CLASS\KizuBitmapManager.h"				// BitMapファイル


#include "ParamManager.h"	

// 全長がらみ
#include "CullManager.h"

// インポート
using namespace ImgLib;

class TestManager : public ThreadManager
{

////　公開関数
public:
	//=============================
	// 本クラス受け渡し用クラス
	//=============================
	//// メール情報受け渡しキュー
	struct DELIVERY_KEY {
		int		nMode;							// 選択モード (0:手動 1:BMP 2:AVI)
		char	cPath[256];						// ファイルパス
		int		nCycle;							// 周期[ms] 0:停止
		int		nStart;							// 開始No
		int		nEnd;							// 終了No
	};
	ThreadQueue<DELIVERY_KEY>	gque_Deli;								// テストマネージャー受け渡しキュー

	TestManager(int nNumCam=10, int nCamStart=1);
	virtual ~TestManager(void);


	//=========================================
	enum DELIVERY_ID {
		EM_TIMER = 0,							// タイマ起動
		EM_FRAME_SEND,							// フレーム情報送信
		EM_END
	};

	// 撮像種別
	enum EM_GRAB_MODE {
		GRAB_MODE_INIT = -1,											// 初期値
		GRAB_MODE_MAN = 0,												// マニュアルセット
		GRAB_MODE_BMP,													// Bitmapファイル
		GRAB_MODE_AVI													// AVIファイル
	};


	//=========================================
	// セット
	void SetParamMgr(ParamManager* pCls)		{ mcls_pParam = pCls; };	// パラメータ管理
	void SetCullMgr(CullManager** pCls)			{ mcls_pCull = pCls; }		// 全長
	void Alloc();														// 確保
	void Free();														// 開放

	void SetFrameNo(int nFrameNo)				{ my_nFrameNum = nFrameNo; }	// フレームNoセット
	void SetChangeFrm(int nChangeFrm)			{ my_nChangeFrm = nChangeFrm; }	// 連続稼動切替フレームNoセット

	//=========================================
	// ゲット
	virtual inline int				GetCamIndex(DWORD set, DWORD ang) const						 // 全部0オリジン (カメラ単位)
	{
		_ASSERT(set < my_nCamsetNum);
		_ASSERT(ang < my_nAngleNum);
		return set*my_nAngleNum + ang;
	}

	int GetFrameNo()							{ return(my_nFrameNum); }		// フレームNoゲット
	int GetChangeFrm()							{ return(my_nChangeFrm); }		// 連続稼働切替フレームNoゲット

	//=========================================

	//// 変数
	BYTE*			pImgs;												// 画像				(カメラ角度分)

//// メンバ関数
private:


	int ThreadFirst();													// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int ThreadLast();													// スレッド終了前処理 (終了シグナル後にコールバック)
    void ThreadEvent(int nEventNo);				

	void Exec(DELIVERY_KEY* pDeli);										// メイン処理実行
	void ExecCycle();													// 定周期処理実行


	// サンプル画像操作系
	bool		OpenBmp(char const* cPathFormat, int nNoFrom, int nNoTo, int nCycleTimer);	// 連続ビットマップ定義
	void		CloseBmp();											// 連続ビットマップ解放
	bool		OpenAvi(char const* cPathFormat, int nCycleTimer);	// AVI定義
	void		CloseAvi();											// AVI解放
	bool		OpenManual();
	void		CloseManual();
	bool		OnManualFrameGrabbed(BYTE* pImgs, DWORD nBytes);

	void		OnAddCull(int nCamId);								// 全長画像送信要求

	void		Send_TiRpisave_NextCoil(int nFrmNo);				// 次コイル情報事前通知

private:
//// メンバ変数

	//=========================================
	//// ハンドル
	HANDLE					m_evTimerExec;								// 定周期実行イベント

	//=========================================
	//// 各インスタンス
	ParamManager*			mcls_pParam;	
	//// 全長
	CullManager**			mcls_pCull;									// 間引きクラス

	//// 変数
	int						m_nNumCam;									// カメラ台数
	int						m_nCamStart;								// カメラ開始位置



	//// 定数


protected:

	//// 変数
	DWORD							my_nCamsetNum;						// 幅方向数
	DWORD							my_nAngleNum;						// 角度数
	DWORD							my_nImgSize;						// フレームサイズ (1画像のみのサイズ)
	DWORD							my_nFrameW;							// 幅   フレーム画像サイズ
	DWORD							my_nFrameH;							// 高さ フレーム画像サイズ
	DWORD							my_nFrameNum;						// フレームNo 
	int								my_nChannel;						// 画像数
	DWORD							my_nChangeFrm;						// 切替フレームNo 



	// テスト画像用
	KizuAviManager*					mycls_Aiv[NUM_CAMERA];				// AVIファイル読み込みクラス
	HANDLE							my_evSmpImgTimer;					// AVI画像等定周期で画像を読み込む場合
	CString							my_sSmpPathFormat;					// 読み込みフォーマット
																		// bmp時、カメラセット(0オリジン), カメラ角度(0オリジン), 連番
																		// avi時、カメラセット(0オリジン), カメラ角度(0オリジン)
	int								my_nSmpIntervalNo[2];				// 読み込み開始No,終了No
	int								my_nSmpNowNo;						// 現在の読み込み
	int								my_nSmpCycleTimer;					// 読み込み周期 [ms]

	int								my_nSelectMode;						// 選択モード


	//// 関数
	virtual void		OnBmpFrameGrabbed();							// 全カメラセット、カメラ角度に対してビットマップ画像をセット
	virtual void		OnAviFrameGrabbed();							// 全カメラセット、カメラ角度に対してAVI画像をセット

};
