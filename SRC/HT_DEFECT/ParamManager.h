#pragma once

#include "..\..\Library\KizuLibMFC\Class\FuncBase.h"
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC操作クラス

// ローカル
#include "MainConst.h"													// 自クラス定数ヘッダー

class ParamManager : public ThreadManager //public FuncBase
{

//// 公開構造体
public:
	//=======================================
	// 疵検定数 構造体
	//=======================================
	struct P_COMMON_MAST
	{
		int			nImgCullX;											// 画像間引き数。静的。 ※ 1/x を指定
		int			nImgCullY;
		BOOL		nImgSave[MAX_IMAGE_CNT];							// 全長画像保存有無。= 全長処理への伝送有無
	
		// 自分で算出
		int			nImgSaveNum;										// 全長画像保存数。 ※自分で算出
	};

//// 公開関数
public:
	ParamManager(void);
	virtual ~ParamManager(void);

	void	SetEv_PrmRead() { SetEvent(m_evRead);}						// 非同期パラメータ読込


	//=========================================
	// パラメータ参照
	inline const P_COMMON_MAST&			GetCommonMast()			const {return (P_COMMON_MAST&)m_CommonMast;}


	//=========================================
	// パラメータ取得関連 (同期)
	bool ReadParam_All();												// 全パラメータ読み込み
	

//// メンバー関数
private :
	int				ThreadFirst();										// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int				ThreadLast()	{return 0;};						// スレッド終了前処理 (終了シグナル後にコールバック)
    void			ThreadEvent(int nEventNo);							// スレッドイベント発生

	// DB読込
	bool			Read_CommonMast();									// 疵検定数パラメータ読込


//// メンバー変数
private :
	//=========================================
	// シグナル (外部操作)
	HANDLE						m_evRead;								// パラメータ読み込み


	//=========================================
	// パラメータ
	P_COMMON_MAST				m_CommonMast;							// 疵検定数

	// 非同期読込
	P_COMMON_MAST				m_CommonMastTemp;						// 疵検定数(非同期読み込んだバッファ)
	
};
