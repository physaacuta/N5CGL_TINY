#include "StdAfx.h"
#include "MaeManager.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// コンストラクタ
//------------------------------------------
MaeManager::MaeManager(void) :
ThreadManager("MaeManager")
{
}

//------------------------------------------
// デストラクタ
//------------------------------------------
MaeManager::~MaeManager(void)
{
}

////////////////////////////////////////////////////////////////////////////////
// スレッド
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int MaeManager::ThreadFirst()
{
	// イベント設定
	HANDLE hArray[] = {
		mque_pMaeRequest->g_evSem
	};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);

	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int MaeManager::ThreadLast()
{
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void MaeManager::ThreadEvent(int nEventNo)
{
	// シグナルの条件
	enum {
		EV_MAE_READ = 0,									// 前工程情報読込
	};

	// シグナル条件分け
	switch(nEventNo){
	//-----------------------------------------------------------------------------------------------
	case EV_MAE_READ: 										// 前工程情報読込
		LOG(em_MSG), "[%s] EventNo=%d OK", my_sThreadName, nEventNo );
		TYP_MAE_REQUEST* pQue;								// キュー情報
		pQue = mque_pMaeRequest->GetFromHead();				// キュー情報取得
		if( 0 == pQue->nMode ){								// 前工程情報読込
			ReadMaeInf(pQue);
		}
		delete pQue;										// キュー情報開放
		return;

	//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] EventNo=%d NG", my_sThreadName, nEventNo );
		return;
	}
}

//------------------------------------------
// 前工程取得状況取得
//------------------------------------------
// TYP_MAE_REQUEST*	pQue	キュー情報
//------------------------------------------
int MaeManager::GetEnableData(TYP_MAE_REQUEST* pQue)
{
	int	nRet;							// リターンコード
	int	nFFRecvFlg;						// FF前工程疵情報受信フラグ
	int	nEnableData;					// 前工程取得状況

	CString	sSql;						// SQL文
	CString	sTbl;						// テーブル名
	CString	sTmp;						// 文字列ワーク
	CString	sTmp2;						// 文字列ワーク

	OdbcBase	clsDB;					// ODBCクラス

	// 初期化
	nEnableData	= 0;					// 前工程取得状況[未取得]

	LOG(em_INF), "②前工程取得状況取得 開始");

	// データベース接続
	if( false == clsDB.Connect(TASKINI_NAME) ){				// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
	} else{
		// テーブル名
		sTbl	= DB_MAE_COIL_INF;							// 前工程コイル情報
		sTmp.Format("'%s'", pQue->cKanriNo);				// 管理№
															// SQL文作成
		sSql	= "SELECT";
		sSql	+= " FF前工程疵情報受信フラグ";
		sSql	+= " FROM ";
		sSql	+= sTbl;									// 前工程コイル情報
		sSql	+= " WHERE";
		sSql	+= " 管理No=";
		sSql	+= sTmp;

		// SQL実行
		if( false == clsDB.ExecuteDirect(sSql) ){			// 失敗
			LOG(em_ERR), "[%s] SQL実行エラー", my_sThreadName);
			LOG(em_NON), "%s", sSql);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", sTbl);
		} else{												// 成功
			nRet	= clsDB.Fetch();						// レコード参照
			if( KIZUODBC_FETCH_OK == nRet ){				// レコード有り
				clsDB.GetData(1, &nFFRecvFlg);				// FF前工程疵情報受信フラグ
				if( 0 == nFFRecvFlg ){						// FF前工程疵情報受信済み
					nEnableData	= 1;						// 前工程取得状況[取得]
				}
			} else if( KIZUODBC_FETCH_NODATE == nRet ){		// レコード無し

			} else {										// 参照エラー
				LOG(em_ERR), "[%s] FETCH ERR SQL", my_sThreadName);
				LOG(em_NON), "%s", sSql);
				syslog(SYSNO_DB_FETCH_ERR, "[%s]", sTbl);
			}
		}
	}

	LOG(em_INF), "②前工程取得状況取得 完了　取得結果=[%d]", nEnableData);

	return nEnableData;
}

//------------------------------------------
// 前工程ＦＦ情報登録
//------------------------------------------
// 前工程ＦＦ情報を共有メモリへ登録する
//------------------------------------------
// TYP_FF_INF*	pFFInf	前工程ＦＦ情報
//------------------------------------------
// 戻り値	登録位置[1:オリジン]
//------------------------------------------
int MaeManager::RegFFInf(TYP_FF_INF* pFFInf, const int nSetPnt, const int nMode)
{
	LOG(em_INF), "④前工程ＦＦ情報登録 開始　FF管理No=[%s] FF疵No=[%d]", pFFInf->cMaeKizukenNo, pFFInf->nMaeKizuNo);

	int	nMenID	= pFFInf->nMenID;							// 表裏区分
	//int	nPnt	= m_pMaeInf->rec[m_nRec].men[nMenID].num;	// 登録位置
	int nPnt = 0;
	if (1 == nMode) {
		nPnt = nSetPnt;
	}
	else {
		nPnt = m_pMaeInf->rec[m_nRec].men[nMenID].num;	// 登録位置
	}
	if( nPnt < TBL_HY_MAE_INF_MAX ){	// 有効登録位置
		// 前工程疵の表示種類 (0：疵無し   1:前工程疵情報、疵画像表示  2:FF情報のみ表示) ※ MAE_MASTERから取得した値
		m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].nMaeDsp			= pFFInf->nMaeDsp;
		// FF管理No (前工程での管理No)
		memcpy( m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].ff.cMaeKizukenNo,	pFFInf->cMaeKizukenNo,	SIZE_KIZUKEN_NO);
		// FF疵No       (前工程での疵No)
		m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].ff.nMaeKizuNo	= pFFInf->nMaeKizuNo;
		// FF疵種
		memcpy( m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].ff.cT, pFFInf->cFF_Type,	2);
		// FFグレード
		memcpy( m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].ff.cG, pFFInf->cFF_Grade,	2);
		// FF疵検出工程
		memcpy( m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].ff.cProc,	pFFInf->cFF_Koutei,	2);
		// FF疵区分
		m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].ff.nDiv = pFFInf->nFF_Kubun;
		// FF疵長さ方向位置 [mm]	※これは使わない
		m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].ff.nPos = pFFInf->nFF_LPos;
		// FF疵巾方向位置 [mm]		(DSからの位置)
		m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].ff.nWidth = pFFInf->nFF_WPos;
		// FF疵長さ [mm]
		m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].ff.nLen = pFFInf->nFF_Length;
		// 最終長手位置 [mm]
		m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].ff.nY = pFFInf->nLastPos;
		// MAE_TYPE_NAME  の ID  (0:不明   1～：前工程の対象疵) ※不明の場合は、FF情報で表示。マスター未定義や
		m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].ff.nTid = pFFInf->nType;
		// MAE_GRADE_NAME の ID  (0:不明)
		m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].ff.nGid = pFFInf->nGrade;
		// <20221030><溝口><2TCM実機化に伴う下工程の改造> --->>>
		m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].ff.nKouteiID = pFFInf->nKouteiID;
		// <20221030><溝口><2TCM実機化に伴う下工程の改造> <<<---

		// 登録位置更新
		if (0 == nMode) {
			m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].nKizuNo = pFFInf->nKizuNo;

			nPnt++;
			m_pMaeInf->rec[m_nRec].men[nMenID].num = nPnt;
		}
	} else{								// 無効登録位置[オーバー]
		nPnt	= 0;
	}

	LOG(em_INF), "④前工程ＦＦ情報登録 完了　登録位置=[%d]", nPnt);

	return	nPnt;
}

static int GetKizuKenNoLen(char *cKizuKenNo)
{
	int ii;
	ii = 0;
	for(ii = SIZE_KIZUKEN_NO - 1; ii >= 0; ii--) {
		if((0x00 != cKizuKenNo[ii]) &&
           (0x20 != cKizuKenNo[ii])){
			// NULL、スペース以外のとき
			break;
		}
	}
	return ii+1;
}

//------------------------------------------
// 前工程疵情報登録
//------------------------------------------
// 前工程疵情報を共有メモリへ登録する
//------------------------------------------
// TYP_FF_INF*		pFFInf	ＦＦ疵情報
// TYP_MAE_DEFECT*	pMaeDef	前工程疵情報
//------------------------------------------
void MaeManager::RegMaeDefect(TYP_FF_INF* pFFInf, TYP_MAE_DEFECT* pMaeDef)
{
	int	nMenID	= pFFInf->nMenID;							// 表裏区分
	int	nPnt	= m_pMaeInf->rec[m_nRec].men[nMenID].num;	// 登録位置[1:オリジン]

	// 画像形式 (0:Jpeg 1:BMP)
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.nImageFlg	= pFFInf->nImgFmt;
	// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 ----->>>
	if (0 == memcmp(pFFInf->cMwk01, KOUTEI_CODE_2TCM, 2)) {
		if (0 == strncmp(pFFInf->cMaeKizukenNo, KIZUKEN_CODE_2TCM, SIZE_KIZUKEN_CODE)) {
			m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt - 1].inf.data.nImageFlg = 1;
		}
		else {
			m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt - 1].inf.data.nImageFlg = pFFInf->nImgFmt;
		}
	}
	// 20200830 冷延　検査装置試験機　ハード・ソフト（2TCM穴検更新） 溝口 <<<-----

	// 事前編集フラグ (-1:非表示  0:通常表示  1:強調表示) ※プロビジョン
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.nDspMode		= pFFInf->nPreEdit;
	// 表裏区分  (0:表 1:裏)
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.men			= pMaeDef->nMenID;
	// 周期疵区分(0:一般疵, 1:簡易周期疵,連続疵 2:周期欠陥)
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.cycle_div	= pMaeDef->nCycle;
	// DSWS区分  (0:DS 1:WS)
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.dsws			= pMaeDef->nDSWS;

	// 速度[mpm]
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.speed		= pMaeDef->nLSpeed;
	// ピッチ[mm]
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.pitch		= pMaeDef->nPitch;
	// ロール径[mm]
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.roolkei		= pMaeDef->fKei;
	// 疵幅 [mm]
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.box_wid		= pMaeDef->nKizuW;
	// 疵長さ [mm]
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.box_len		= pMaeDef->nKizuL;

	// 縦分解能[mm/pixel]
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.h_res		= pMaeDef->fVDenc;
	// 横分解能[mm/pixel]
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.w_res		= pMaeDef->fHDenc;
	// 外接Xmin[pixel]		(切り出し画像の左上からの位置) (マイナス値ありえる)
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.box_x_min	= pMaeDef->nEXmin;
	// 外接Xmax[pixel]
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.box_x_max	= pMaeDef->nEXmax;
	// 外接Ymin[pixel]
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.box_y_min	= pMaeDef->nEYmin;
	// 外接Ymax[pixel]
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.box_y_max	= pMaeDef->nEYmax;
	// 疵中心位置X [pixel]  (切り出し画像の左上からの位置)
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.center_x		= pMaeDef->nCPosX;
	// 疵中心位置Y [pixel]
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.center_y		= pMaeDef->nCPosY;
	// 縦画素数[pixel]
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.h_size		= pMaeDef->nVPixel;
	// 横画素数[pixel]
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.w_size		= pMaeDef->nHPixel;

	//// 最終長手位置 [mm]
	//m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data[nSpm].nY			= pMaeDef->nLPos;
	//// MAE_TYPE_NAME  の ID  (0:不明   1～：前工程の対象疵)
	//m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data[nSpm].nTid			= pFFInf->nType;
	//// MAE_GRADE_NAME の ID  (0:不明)
	//m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data[nSpm].nGid			= pFFInf->nGrade;
}



//------------------------------------------
// 前工程疵画像読込
//------------------------------------------
// 前工程疵画像を読み込みローカルファイルへ格納する
//------------------------------------------
// TYP_FF_INF*		pFFInf	ＦＦ疵情報
// TYP_MAE_DEFECT*	pMaeDef	前工程疵情報
//------------------------------------------
bool MaeManager::ReadMaeImg(TYP_FF_INF* pFFInf, TYP_MAE_DEFECT* pMaeDef)
{
	int	nImgFmt;						// 画像形式
	int	nMenID;							// 表裏区分
	int	nPnt;							// 登録位置[1:オリジン]
	int	nRet;							// リターンコード
	int	nSize[MAX_IMAGE_CNT];			// 画像サイズ

	char		cPath[SIZE_IMAGE_PATH];	// 画像パス
	char*		pPath[MAX_IMAGE_CNT];	// 画像パスポインタ
	char const*	pFolder;				// 前工程画像フォルダー

	CString		sPath;					// 画像パス
	CString		sSql;					// SQL文
	CString		sTbl;					// テーブル名

	OdbcBase	clsDB;					// ODBCクラス

	static const char	cImgExt[2][4]={
		"jpg", "bmp"
	};

	LOG(em_INF), "⑥前工程疵画像読込 開始　FF管理No=[%s] FF疵No=[%d]", pFFInf->cMaeKizukenNo, pFFInf->nMaeKizuNo);

	nMenID	= pFFInf->nMenID;							// 表裏区分
	nPnt	= m_pMaeInf->rec[m_nRec].men[nMenID].num;	// 登録位置[1:オリジン]

	// 前工程画像フォルダー
	pFolder	= mcls_pDataSet->GetMaeImgPath();

	// 画像形式
	nImgFmt	= (int)m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.nImageFlg;

	// 画像パス
	sPath.Format("%s\\rec%d\\%d_%d", pFolder, m_nRec, pMaeDef->nMenID, pMaeDef->nKizuNo);

	// 画像サイズ
	nSize[0]	= pMaeDef->nPSize1;
	nSize[1]	= pMaeDef->nPSize2;
	nSize[2]	= pMaeDef->nPSize3;
	nSize[3]	= pMaeDef->nPSize4;

	// 画像パスポインタ
	pPath[0]	= m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.image_path_1;
	pPath[1]	= m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.image_path_2;
	pPath[2]	= m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.image_path_3;
	pPath[3]	= m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.image_path_4;

	// データベース接続
	if( false == clsDB.Connect(TASKINI_NAME) ){				// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	// テーブル名
	sTbl	= DB_MAE_DEFECT_IMG;							// 前工程疵画像

	// SQL文作成
	sSql.Format(
		"SELECT 画像1, 画像2, 画像3, 画像4 FROM %s WHERE 管理No='%s' AND 疵No=%d",
		sTbl, pMaeDef->cKizukenNo, pMaeDef->nKizuNo
	);

	// SQL実行
	if( false == clsDB.ExecuteDirect(sSql) ){
		LOG(em_ERR), "[%s] SQL実行エラー ", my_sThreadName);
		LOG(em_NON), "%s", sSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", sTbl);
		return false;
	}

	// テーブルデータ取得
	while( TRUE ){											// 有効レコード分
		nRet	= clsDB.Fetch();							// レコード参照
		if( KIZUODBC_FETCH_OK == nRet ){					// レコード有り
			// レコードデータ取得
			for(int ii=0; ii<MAX_IMAGE_CNT; ii++){			// 画像数分
				if( nSize[ii] > 0 ){						// 画像有り
					// 初期化
					memset(cPath, 0x00, sizeof(cPath));
					// 画像エリア確保
					BYTE*	pImg	= new BYTE[nSize[ii]];
					// 画像取得
					clsDB.GetDataImage(ii + 1, pImg, nSize[ii]);
					// 画像パス
					sprintf(cPath, "%s_%d.%s", (LPCTSTR)sPath, ii, cImgExt[nImgFmt]);
					// 画像ファイル出力
					if (0 == nImgFmt){						// JPEG
						CFile	clsFile;					// ファイルアクセスクラス
						clsFile.Open(cPath, CFile::modeCreate | CFile::modeWrite);
						clsFile.Write(pImg, nSize[ii]);
						clsFile.Close();
						//std::ofstream	clsStream;
						//clsStream.open(cPath, std::ios::out | std::ios::trunc | std::ios::binary);
						//clsStream.write((char const*)pImg, nSize[ii]);
						//clsStream.close();
					} else{									// BMP

						KizuBitmapManager	clsBmp;			// Bitmap画像生成クラス
						clsBmp.DataToBMPfile_Gray(pMaeDef->nHPixel, pMaeDef->nVPixel, pImg, cPath);
					}
					// 画像パス登録
					strcpy(pPath[ii], cPath);
					// 画像エリア開放
					SAFE_DELETE(pImg);
				}
			}
		} else if( KIZUODBC_FETCH_NODATE == nRet ){			// レコード無し
			break;
		} else {											// 参照エラー
			LOG(em_ERR), "[%s] FETCH ERR SQL", my_sThreadName);
			LOG(em_NON), "%s", sSql);
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", sTbl);
			return false;
		}
	}

	LOG(em_INF), "⑥前工程疵画像読込 完了");

	return true;
}

//------------------------------------------
// 前工程疵情報読込
//------------------------------------------
// 前工程疵情報テーブルから該当管理№の疵情報を読み込み共有メモリへ登録する
//------------------------------------------
// TYP_FF_INF*	pFFInf	ＦＦ情報
//------------------------------------------
bool MaeManager::ReadMaeDefect(TYP_FF_INF* pFFInf)
{
	int				nRet;				// リターンコード

	CString			sSql;				// SQL文
	CString			sTbl;				// テーブル名
	CString			sTmp;				// 文字列ワーク

	TYP_MAE_DEFECT	typMaeDef;			// 前工程疵情報

	OdbcBase	clsDB;					// ODBCクラス

	LOG(em_INF), "⑤前工程疵情報登録 開始　FF管理No=[%s] FF疵No=[%d]", pFFInf->cMaeKizukenNo, pFFInf->nMaeKizuNo);

	// データベース接続
	if( false == clsDB.Connect(TASKINI_NAME) ){				// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	// テーブル名
	sTbl	= DB_MAE_DEFECT_INF;							// 前工程疵情報

	// SQL文作成
	sSql	= "SELECT";
	sSql	+= " 管理No";
	sSql	+= ",疵No";
	sSql	+= ",工程種別";
	sSql	+= ",表裏区分";
	sSql	+= ",周期フラグ";
	sSql	+= ",長手位置";
	sSql	+= ",DSWS区分";
	sSql	+= ",DS位置";
	sSql	+= ",WS位置";
	sSql	+= ",ライン速度";
	sSql	+= ",ピッチ";
	sSql	+= ",径";
	sSql	+= ",疵幅";
	sSql	+= ",疵長さ";
	sSql	+= ",縦分解能";
	sSql	+= ",横分解能";
	sSql	+= ",外接Xmin";
	sSql	+= ",外接Xmax";
	sSql	+= ",外接Ymin";
	sSql	+= ",外接Ymax";
	sSql	+= ",疵中心位置X";
	sSql	+= ",疵中心位置Y";
	sSql	+= ",縦画素数";
	sSql	+= ",横画素数";
	sSql	+= ",画像サイズ1";
	sSql	+= ",画像サイズ2";
	sSql	+= ",画像サイズ3";
	sSql	+= ",画像サイズ4";
	sTmp.Format(
		" FROM %s WHERE 管理No='%s' AND 疵No=%d AND 工程種別=%d AND 表裏区分=%d",
		sTbl, pFFInf->cKizukenNo, pFFInf->nKizuNo, pFFInf->nKouteiID, pFFInf->nMenID
	);
	sSql	+= sTmp;

	// SQL実行
	if( false == clsDB.ExecuteDirect(sSql) ){
		LOG(em_ERR), "[%s] SQL実行エラー ", my_sThreadName);
		LOG(em_NON), "%s", sSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", sTbl);
		return false;
	}

	// テーブルデータ取得
	while( TRUE ){											// 有効レコード分
		nRet	= clsDB.Fetch();							// レコード参照
		if( KIZUODBC_FETCH_OK == nRet ){					// レコード有り
			// レコードデータ取得
															// 管理№
			clsDB.GetData( 1, typMaeDef.cKizukenNo, SIZE_KIZUKEN_NO);
			clsDB.GetData( 2, &typMaeDef.nKizuNo);			// 疵№
			clsDB.GetData( 3, &typMaeDef.nKouteiID);		// 工程種別
			clsDB.GetData( 4, &typMaeDef.nMenID);			// 表裏区分[0:表、1:裏]
			clsDB.GetData( 5, &typMaeDef.nCycle);			// 周期フラグ
			clsDB.GetData( 6, &typMaeDef.nLPos);			// 長手位置[mm]
			clsDB.GetData( 7, &typMaeDef.nDSWS);			// DSWS区分[0:DS、1:WS]
			clsDB.GetData( 8, &typMaeDef.fDSPos);			// DS位置[mm]
			clsDB.GetData( 9, &typMaeDef.fWSPos);			// WS位置[mm]
			clsDB.GetData(10, &typMaeDef.nLSpeed);			// ライン速度[mpm]
			clsDB.GetData(11, &typMaeDef.nPitch);			// ピッチ[mm]
			clsDB.GetData(12, &typMaeDef.fKei);				// 径[mm]
			clsDB.GetData(13, &typMaeDef.nKizuW);			// 疵幅[mm]
			clsDB.GetData(14, &typMaeDef.nKizuL);			// 疵長さ[mm]
			clsDB.GetData(15, &typMaeDef.fVDenc);			// 縦分解能[mm/pixel]
			clsDB.GetData(16, &typMaeDef.fHDenc);			// 横分解能[mm/pixel]
			clsDB.GetData(17, &typMaeDef.nEXmin);			// 外接Xmin[pixel]
			clsDB.GetData(18, &typMaeDef.nEXmax);			// 外接Xmax[pixel]
			clsDB.GetData(19, &typMaeDef.nEYmin);			// 外接Ymin[pixel]
			clsDB.GetData(20, &typMaeDef.nEYmax);			// 外接Ymax[pixel]
			clsDB.GetData(21, &typMaeDef.nCPosX);			// 疵中心位置X[pixel]
			clsDB.GetData(22, &typMaeDef.nCPosY);			// 疵中心位置Y[pixel]
			clsDB.GetData(23, &typMaeDef.nVPixel);			// 縦画素数[pixel]
			clsDB.GetData(24, &typMaeDef.nHPixel);			// 横画素数[pixel]
			clsDB.GetData(25, &typMaeDef.nPSize1);			// 画像サイズ1[byte]
			clsDB.GetData(26, &typMaeDef.nPSize2);			// 画像サイズ2[byte]
			clsDB.GetData(27, &typMaeDef.nPSize3);			// 画像サイズ3[byte]
			clsDB.GetData(28, &typMaeDef.nPSize4);			// 画像サイズ4[byte]

			RegMaeDefect(pFFInf, &typMaeDef);				// 前工程疵情報登録[共有メモリ]

			// 画像有無(=1は保存しない)
			if (1 != mcls_pDataSet->GetnMaeImgSaveOff())
			{
				ReadMaeImg(pFFInf, &typMaeDef);				// 前工程疵画像読込
			}

		} else if( KIZUODBC_FETCH_NODATE == nRet ){			// レコード無し
			break;
		} else {											// 参照エラー
			LOG(em_ERR), "[%s] FETCH ERR SQL", my_sThreadName);
			LOG(em_NON), "%s", sSql);
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", sTbl);
			return false;
		}
	}

	LOG(em_INF), "⑤前工程疵情報登録 完了");

	return true;
}

//------------------------------------------
// 前工程ＦＦ情報読込
//------------------------------------------
// 前工程ＦＦ疵情報テーブルから該当管理№のＦＦ情報を読み込み共有メモリへ登録する
//------------------------------------------
// TYP_MAE_REQUEST*	pQue		キュー情報
//------------------------------------------
bool MaeManager::ReadFFInf(TYP_MAE_REQUEST* pQue)
{
	int	nPnt;							// 登録位置[1:オリジン]
	int	nRet;							// リターンコード

	CString	sSql;						// SQL文
	CString	sTbl;						// テーブル名
	CString	sTmp;						// 文字列ワーク

	TYP_FF_INF	typFFInf;				// 前工程ＦＦ情報

	OdbcBase	clsDB;					// ODBCクラス

	LOG(em_INF), "③前工程ＦＦ情報読込 開始");

	// データベース接続
	if( false == clsDB.Connect(TASKINI_NAME) ){				// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	// テーブル名
	sTbl	= DB_MAE_FF_INF;								// 前工程ＦＦ疵情報

	sTmp.Format("'%s'", pQue->cKanriNo);					// 管理№

	// SQL文作成
	sSql	= "SELECT";
	sSql	+= " i.管理No";
	sSql	+= ",i.疵No";
	sSql	+= ",i.工程種別";
	sSql	+= ",i.表裏区分";
	sSql	+= ",i.事前編集フラグ";
	sSql	+= ",i.前工程管理No";
	sSql	+= ",i.前工程疵No";
	sSql	+= ",i.FF疵種";
	sSql	+= ",i.FFグレード";
	sSql	+= ",i.FF疵検出工程";
	sSql	+= ",i.FF疵区分";
	sSql	+= ",i.FF疵長さ方向位置";
	sSql	+= ",i.FF疵巾方向位置";
	sSql	+= ",i.FF疵長さ";
	sSql	+= ",i.判定疵種";
	sSql	+= ",i.判定グレード";
	sSql	+= ",i.最終長手位置";
	sSql	+= ",c.管理No";
	sSql	+= ",c.工程種別";
	sSql	+= ",c.表裏区分";
	sSql	+= ",c.処理ステータス";
	sSql	+= ",m.前工程コード";
	sSql	+= ",m.有効判定";
	sSql	+= ",m.前工程表示モード";
	sSql	+= ",m.画像形式";
	sSql	+= " FROM (";
	sSql	+= sTbl;										// 前工程ＦＦ疵情報[i]
	sSql	+= " i";
	sSql	+= " INNER JOIN ";
	sSql	+= DB_MAE_COIL_LINE;							// 前工程ライン別コイル情報[c]
	sSql	+=" c";
	sSql	+= " ON";
	sSql	+= " i.管理No=c.管理No";
	sSql	+= " AND i.工程種別=c.工程種別";
	sSql	+= " AND i.表裏区分=c.表裏区分";
	sSql	+= ")";
	sSql	+= " INNER JOIN ";
	sSql	+= DB_MAE_MASTER;								// 前工程マスター[m]
	sSql	+= " m";
	sSql	+= " ON";
	sSql	+= " i.FF疵検出工程=m.前工程コード";
	sSql	+= " WHERE";
	sSql	+= " i.管理No=";
	sSql	+= sTmp;
	sSql	+= " ORDER BY";
	sSql	+= " i.工程種別 ASC, i.疵No ASC";

	// SQL実行
	if( false == clsDB.ExecuteDirect(sSql) ){
		LOG(em_ERR), "[%s] SQL実行エラー ", my_sThreadName);
		LOG(em_NON), "%s", sSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", sTbl);
		return false;
	}

	// テーブルデータ取得
	while( TRUE ){											// 有効レコード分
		nRet	= clsDB.Fetch();							// レコード参照
		if( KIZUODBC_FETCH_OK == nRet ){					// レコード有り
			// 前工程ＦＦ疵情報
															// 管理№
			clsDB.GetData( 1, typFFInf.cKizukenNo, SIZE_KIZUKEN_NO);
			clsDB.GetData( 2, &typFFInf.nKizuNo);			// 疵№
			clsDB.GetData( 3, &typFFInf.nKouteiID);			// 工程種別
			clsDB.GetData( 4, &typFFInf.nMenID);			// 表裏区分[0:表、1:裏]
			clsDB.GetData( 5, &typFFInf.nPreEdit);			// 事前編集フラグ
															// 前工程管理№
			clsDB.GetData( 6, typFFInf.cMaeKizukenNo, SIZE_KIZUKEN_NO);
			clsDB.GetData( 7, &typFFInf.nMaeKizuNo);		// 前工程疵№
			clsDB.GetData( 8, typFFInf.cFF_Type, 4);		// ＦＦ疵種
			clsDB.GetData( 9, typFFInf.cFF_Grade, 4);		// ＦＦ疵グレード
			clsDB.GetData(10, typFFInf.cFF_Koutei, 4);		// ＦＦ疵検出工程
			clsDB.GetData(11, &typFFInf.nFF_Kubun);			// ＦＦ疵区分
			clsDB.GetData(12, &typFFInf.nFF_LPos);			// ＦＦ疵長さ方向位置[mm]
			clsDB.GetData(13, &typFFInf.nFF_WPos);			// ＦＦ疵巾方向位置[mm]
			clsDB.GetData(14, &typFFInf.nFF_Length);		// ＦＦ疵長さ[mm]
			clsDB.GetData(15, &typFFInf.nType);				// 判定疵種
			clsDB.GetData(16, &typFFInf.nGrade);			// 判定疵グレード
			clsDB.GetData(17, &typFFInf.nLastPos);			// 最終長手位置[mm]
			// 前工程ライン別コイル情報
															// 管理No
			clsDB.GetData(18, typFFInf.cLwk01, SIZE_KIZUKEN_NO);
			clsDB.GetData(19, &typFFInf.nLwk02);			// 工程種別
			clsDB.GetData(20, &typFFInf.nLwk03);			// 表裏区分
			clsDB.GetData(21, &typFFInf.nStatus);			// 処理ステータス
			// 前工程マスター
			clsDB.GetData(22, typFFInf.cMwk01, 4);			// 前工程コード
			clsDB.GetData(23, &typFFInf.nCheck);			// 有効判定
			clsDB.GetData(24, &typFFInf.nMaeDsp);			// 前工程表示モード
			clsDB.GetData(25, &typFFInf.nImgFmt);			// 画像形式

			if( 1 == typFFInf.nCheck ){						// 有効工程
				// 前工程表示対象
				if( 1 == typFFInf.nMaeDsp || 2 == typFFInf.nMaeDsp ){
					int nMode = 0;
					int nSetPos = GetMaeInfSetPos(&typFFInf, &nMode);
					nPnt	= RegFFInf(&typFFInf, nSetPos, nMode);			// 前工程ＦＦ情報登録[共有メモリ]
					if( nPnt > 0 && 0 == nMode){							// 登録正常
						if( 1 == typFFInf.nMaeDsp ){		// 前工程疵情報表示
							if( 0 == typFFInf.nStatus ){	// 前工程疵情報[取得済み]
								ReadMaeDefect(&typFFInf);	// 前工程疵情報読込正常
							}
						}
					}
				}
			}
		} else if( KIZUODBC_FETCH_NODATE == nRet ){			// レコード無し
			break;
		} else {											// 参照エラー
			LOG(em_ERR), "[%s] FETCH ERR SQL", my_sThreadName);
			LOG(em_NON), "%s", sSql);
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", sTbl);
			return false;
		}
	}

	LOG(em_INF), "③前工程ＦＦ情報読込 完了");

	return true;
}

int MaeManager::GetMaeInfSetPos(TYP_FF_INF* pFFInf, int* nMode )
{
	int	nMenID = pFFInf->nMenID;							// 表裏区分
	int	nPnt = m_pMaeInf->rec[m_nRec].men[nMenID].num;		// 登録位置

	// 5CGLの DB 前工程ＦＦ疵情報 のキー値は管理Noと疵Noのみである為、
	// 管理Noと疵Noが重複する事はないので以下の判定は実施されない。
	// 流用元の1CAPLでは DB 前工程ＦＦ疵情報 のキー値に「SPM圧下有無」が存在していたので
	// 管理Noと疵Noが重複する可能性があった。
	*nMode = 0;
	for (int ii = 0; ii < (int)m_pMaeInf->rec[m_nRec].men[nMenID].num; ii++) {
		if (pFFInf->nKizuNo == m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[ii].nKizuNo) {
			nPnt = ii;
			*nMode = 1;
		}
	}
	return nPnt;
}
//------------------------------------------
// 前工程マップ情報登録
//------------------------------------------
// 前工程ＦＦ疵情報を共有メモリへ登録する
//------------------------------------------
void MaeManager::RegMaeMap()
{
	LOG(em_INF), "⑦前工程マップ情報登録 開始");

	TBL_HY_MAPROW*	pTblMapRow = mcls_pDataSet->GetMapRowTbl();
	for (int nMen = 0; nMen < NUM_MEN; nMen++) {
		int	nCnt = m_pMaeInf->rec[m_nRec].men[nMen].num;	// 疵情報登録数
		for (int nNum = 0; nNum < nCnt; nNum++) {			// 疵情報登録数分

			int	nLen = m_pMaeInf->rec[m_nRec].men[nMen].maeinf[nNum].ff.nY / 1000;	// 最終長手位置[mm]⇒[m]
	
			// 疵ポインタを登録（最小、最大を更新）
			for (int nStage = 0; nStage<MAP_STAGE_NUM; nStage++) {
				// 行数[0:オリジン]
				int	nRow = mcls_pDataSet->GetStageLenToIndex(nStage, nLen);
				DSP_TYP_ROW& pWk = pTblMapRow->rec[m_nRec].stage[nStage].men[nMen].row[nRow].typ;
				if (0 == pWk.pntMae[0] || pWk.pntMae[0] > nNum + 1) pWk.pntMae[0] = nNum + 1;			// 最小疵情報ポインタ(1オリジン)
				if (0 == pWk.pntMae[1] || pWk.pntMae[1] < nNum + 1) pWk.pntMae[1] = nNum + 1;			// 最大疵情報ポインタ(1オリジン)
			}
			
		}
	}

	LOG(em_INF), "⑦前工程マップ情報登録 完了");
}

//------------------------------------------
// 前工程情報読込
//------------------------------------------
// TYP_MAE_REQUEST*	pQue	キュー情報
//------------------------------------------
void MaeManager::ReadMaeInf(TYP_MAE_REQUEST* pQue)
{
	LOG(em_INF), "①前工程情報読込 開始　管理No=[%s]", pQue->cKanriNo);

	// 格納レコード検索
/***
	TBL_HY_POINT const*	pTblPoint	= mcls_pDataSet->GetPointTbl();
	m_nRec	= mcls_pDataSet->CheckKizukenNo(pQue->cKizukenNo, pTblPoint);
	if( m_nRec < 0 ){										// 該当レコード無し
		LOG(em_ERR), "[%s] 該当レコード無し 管理№=%s", my_sThreadName, pQue->cKizukenNo);
		return;
	}
***/
	//// TCP/IPとメールスロットの逆転到着対応
	TBL_HY_POINT const*	pTblPoint	= mcls_pDataSet->GetPointTbl();
	int ii;
	const int cCNT = 3;
	for(ii=0; ii<cCNT; ii++) {
		m_nRec	= mcls_pDataSet->CheckKanriNo(pQue->cKanriNo, pTblPoint);
		if( m_nRec < 0 ){										// 該当レコード無し
			LOG(em_ERR), "[%s] 該当レコード無し 管理№=%s のため、ちょっと待ち", my_sThreadName, pQue->cKanriNo);
			Sleep(500);
		} else {
			break;
		}
	}
	if( ii == cCNT ) {										// 該当レコード無し
		LOG(em_ERR), "[%s] 該当レコード無し 管理№=%s", my_sThreadName, pQue->cKanriNo);
		return;
	}

	// 共有メモリ格納レコードを初期化する
	m_pMaeInf	= mcls_pDataSet->GetMaeInf();				// 前工程マップ情報ポインタ取得
	// 前工程疵情報
	memset(&m_pMaeInf->rec[m_nRec], 0x00, sizeof(m_pMaeInf->rec[m_nRec]));

	// 前工程取得状況
	if (1 == GetEnableData(pQue)) {			// 取得済み
											// 前工程ＦＦ情報読込
		if (true == ReadFFInf(pQue)) {		// 読込正常
		}
	}
// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
	// 前工程疵情報の取得完了をメインに通知
	COMMON_QUE que;
	que.nEventNo = FACT_HY_DEFECT_04;
	que.nLineNo = getlineid();
	que.fl.data[0] = m_nRec;
	send_mail(getmytaskname(), ".", &que);
// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
	
	RegMaeMap();									// 前工程マップ情報登録

	LOG(em_INF), "①前工程情報読込 完了");
}
