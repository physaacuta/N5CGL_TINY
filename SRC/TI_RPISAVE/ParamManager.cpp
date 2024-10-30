#include "StdAfx.h"
#include "ParamManager.h"

#include "..\..\Library\KizuLib\CLASS\KizuBitmapManager.h"		// 他にはリンクしたくない(=するほどじゃない)から、cppに入れておく。

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
ParamManager::ParamManager(void) :
ThreadManager("ParamMgr")
{
	m_evReadEnd	= CreateEvent(NULL, FALSE, FALSE, NULL);	
}

//------------------------------------------
// デストラクタ
//------------------------------------------
ParamManager::~ParamManager(void)
{
	CloseHandle(m_evReadEnd);
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int ParamManager::ThreadFirst()
{
	//// イベント設定
	HANDLE hArray[] = { gque_Deli.g_evSem
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void ParamManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件		
	enum {	EV_QUE = 0,								// パラメータ変更通知
			EV_END	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_QUE:									// パラメータ変更通知
		COMMON_QUE* pDeli;				// スレッドキュー受け渡し
		pDeli = gque_Deli.GetFromHead();
		ExecParam(pDeli);
		delete pDeli;

		// パラメータ読み込み完了通知
		SetEvent(m_evReadEnd);
		break;


//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		break;
	}
}

//------------------------------------------
// パラメータ読込制御
// COMMON_QUE* pQue キュー受け渡し
//------------------------------------------
void ParamManager::ExecParam(COMMON_QUE* pQue)
{	
	// 今回は、全パラメータを読み込んで終わり？
	ReadParam_All(false);
}

//------------------------------------------
// 計算値縦分解能 (完全固定分解能としたいため)
// int idx	チャンネル (0:ﾗｲﾝPLG)  EM_DIV_PLG
//------------------------------------------
double ParamManager::CalcResV()
{
	double dResV = 1.0;		// 縦分解能
	dResV = m_Param_Common.dPls * m_Param_Common.nDiv / m_Param_Common.nMul;
	return dResV;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// パラメータ読込

//------------------------------------------
// 全パラメータ読み込み
// bool bStatic staticパラメータも取得する場合 true
//------------------------------------------
bool ParamManager::ReadParam_All(bool bStatic)
{

	// =========================================================
	// パラメータ取得
	P_MAIN	buf;
	memset(&buf, 0x00, sizeof(buf));

	if( ! Read_Ini(buf, false) )	return false;
	if( ! Read_Param_CamSet(buf) )	return false;
	if( ! Read_Param_CamEnable(buf))return false;
	if( ! Read_Param_Common() )		return false;

	// 計算
	CalcParam(buf);
	memcpy( &m_Main, &buf, sizeof(buf));

	// コイル単位での読み込み
	for(int ii=0; ii<buf.nIns; ii++) {
		Read_IniCoilChange(ii);
		Read_IniKidoChange(ii);
	}
	return true;
}

//------------------------------------------
// iniファイル読み込み
// P_MAIN& buf セット用エリア
// bool bStatic staticパラメータも取得する場合 true
//------------------------------------------
bool ParamManager::Read_Ini(P_MAIN& buf, bool bStatic)
{
	CString	sWk, sWk2;
//	char	cWk[256];

	//// iniファイル取得
	// 共通部
	sWk.Format("%s", TI_RPISAVE);
	buf.nIns		= GetPrivateProfileInt(sWk, "INS_NUM", 0, TASKINI_NAME );
	buf.nFrameSizeX = GetPrivateProfileInt(sWk, "FRAME_SIZE_X", 0, TASKINI_NAME );
	buf.nFrameSizeY	= GetPrivateProfileInt(sWk, "FRAME_SIZE_Y", 0, TASKINI_NAME );
	buf.nGrabSizeX  = GetPrivateProfileInt(sWk, "GRAB_SIZE_X", 0, TASKINI_NAME );
	buf.nGrabSizeY	= GetPrivateProfileInt(sWk, "GRAB_SIZE_Y", 0, TASKINI_NAME );

	buf.nExecCoilStart	= GetPrivateProfileInt(sWk, "EXEC_COIL_START", 0, TASKINI_NAME );

	GetPrivateProfileString( sWk, "WRITE_SEC_PATH", "", buf.cWriteSecPath, sizeof(buf.cWriteSecPath), TASKINI_NAME );
	GetPrivateProfileString( sWk, "SYSTEM_NAME", "", buf.cSystemName, sizeof(buf.cSystemName), TASKINI_NAME );
	buf.nHddFreeSizeSave  = GetPrivateProfileInt(sWk, "HDD_FREE_SIZE", 10, TASKINI_NAME);
	buf.nHddFreeSizeLocal = GetPrivateProfileInt(sWk, "HDD_FREE_SIZE_LOCAL", buf.nHddFreeSizeSave, TASKINI_NAME);
	
	buf.nAddFrameInfoType = GetPrivateProfileInt(sWk, "ADD_FRAMEINFO_TYPE", 0, TASKINI_NAME);		// フレーム付加情報切替 (0:無し 1:T_RPI_FRAME_INFO_T1)


	// 処理
	buf.nLoopQueRecvSize	= GetPrivateProfileInt(sWk, "LOOP_QUE_RECV_SIZE", 10, TASKINI_NAME);
	buf.nLoopQueFrameSize	= GetPrivateProfileInt(sWk, "LOOP_QUE_FRAME_SIZE", 20, TASKINI_NAME);
	buf.nThreadUnionNum		= GetPrivateProfileInt(sWk, "THREAD_UNION_NUM", 2, TASKINI_NAME);
	buf.nThreadCompNum		= GetPrivateProfileInt(sWk, "THREAD_COMP_NUM", 8, TASKINI_NAME);

	// 画面
	buf.nDispWindScaleX		= GetPrivateProfileInt(sWk, "DEBUG_DISP_SCALE_X", 8, TASKINI_NAME);
	buf.nDispWindScaleY		= GetPrivateProfileInt(sWk, "DEBUG_DISP_SCALE_Y", 8, TASKINI_NAME);
	buf.nDispWindPosX		= GetPrivateProfileInt(sWk, "DEBUG_DISP_POS_X", 0, TASKINI_NAME);
	buf.nDispWindPosY		= GetPrivateProfileInt(sWk, "DEBUG_DISP_POS_Y", 0, TASKINI_NAME);
	buf.nDispPrint			= GetPrivateProfileInt(sWk, "DEBUG_PRINT", 0, TASKINI_NAME);

	// 機器ID
	buf.nStateId			= GetPrivateProfileInt(sWk, "ERR_ID", 0, TASKINI_NAME);

	// 固定
	buf.emColorType = COLOR_TYPE_COLOR;
	buf.emSysType	= SYS_TYPE_BAT;
	buf.nImgNumTotal= MAX_COLOR_NUM;

	// インスタンス別
	for(int ii=0; ii<buf.nIns; ii++) {
		sWk.Format("%s_INS_%d", TI_RPISAVE, ii+1);
	
		// 基本
		buf.ins[ii].nStartCamset	= GetPrivateProfileInt(sWk, "START_CAMSET", 0, TASKINI_NAME );
		buf.ins[ii].nCamsetNum		= GetPrivateProfileInt(sWk, "CAMSET_NUM", 0, TASKINI_NAME );
		buf.ins[ii].nMen			= GetPrivateProfileInt(sWk, "MEN", 0, TASKINI_NAME );
		buf.ins[ii].bWidthSync		= __BOOL GetPrivateProfileInt(sWk, "WIDTH_SYNC", 0, TASKINI_NAME );
		buf.ins[ii].bImgTorB		= __BOOL GetPrivateProfileInt(sWk, "IMG_TorB", 0, TASKINI_NAME );
		GetPrivateProfileString(sWk, "RAW_SAVE_PATH", "", buf.ins[ii].cRawSavePath, sizeof(buf.ins[ii].cRawSavePath), TASKINI_NAME );

		buf.ins[ii].nCamAngle = 1;

		// 一時バッファ
		for(int jj=0; jj<MAX_FOLDER_SAVE_NUM; jj++) {
			sWk2.Format("RPI_FOLDER_BUF_%d", jj+1);
			GetPrivateProfileString(sWk, sWk2, "", buf.ins[ii].cRpiFolderBuf[jj], sizeof(buf.ins[ii].cRpiFolderBuf[jj]), TASKINI_NAME );
			if( 0 == strlen(buf.ins[ii].cRpiFolderBuf[jj])) break;
			buf.ins[ii].nRpiFolderBufNum ++;
		}
		_ASSERT( 0 != buf.ins[ii].nRpiFolderBufNum );

		// 長期間保存パス
		for(int jj=0; jj<MAX_FOLDER_SAVE_NUM; jj++) {
			sWk2.Format("RPI_FOLDER_SAVE_%d", jj+1);
			GetPrivateProfileString(sWk, sWk2, "", buf.ins[ii].cRpiFolderSave[jj], sizeof(buf.ins[ii].cRpiFolderSave[jj]), TASKINI_NAME );
			if( 0 == strlen(buf.ins[ii].cRpiFolderSave[jj])) break;
			buf.ins[ii].nRpiFolderSaveNum ++;
		}			
		_ASSERT( 0 != buf.ins[ii].nRpiFolderSaveNum );

		//// 連結関係
		buf.ins[ii].nOverLapMode	= GetPrivateProfileInt(sWk, "OVERLAP_MODE", 0, TASKINI_NAME );
		buf.ins[ii].nImgDelete[0]	= GetPrivateProfileInt(sWk, "IMG_DELETE_L", 0, TASKINI_NAME );
		buf.ins[ii].nImgDelete[1]	= GetPrivateProfileInt(sWk, "IMG_DELETE_R", 0, TASKINI_NAME );
		buf.ins[ii].bEdgeDeleteEnabel= __BOOL GetPrivateProfileInt(sWk, "EDGE_DELETE_ENABLE", 0, TASKINI_NAME );
		buf.ins[ii].nEdgeDelete[0]	= GetPrivateProfileInt(sWk, "EDGE_DELETE_L", 0, TASKINI_NAME );
		buf.ins[ii].nEdgeDelete[1]	= GetPrivateProfileInt(sWk, "EDGE_DELETE_R", 0, TASKINI_NAME );
	}


	//// 取得データをメンバー変数にセット
	LOG(em_MSG), "[%s] Iniファイル 取得完了", my_sThreadName);
	return true;
}

//------------------------------------------
// カメラセット別 パラメータ 読込
// P_MAIN& buf セット用エリア
//------------------------------------------
bool ParamManager::Read_Param_CamSet(P_MAIN& pm)
{
	CString sql;					// SQL文
	CString	sWk;
	int camset;

	//// パラメータ初期化
	CString cDbName;
	cDbName = DB_PARAM_CAMSET;
	P_PARAM_CAMSET&	p = m_Param_CamSet;
	P_PARAM_CAMSET	buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}


	for(int ii=0; ii<pm.nIns; ii++) {
		//// SQL文作成
		sql =  "SELECT";
		sql += " camset";
		sql += ", Hres, Xoffset";

		sql += " FROM ";
		sql += cDbName;
		sWk.Format(" WHERE camset>=%d and camset<%d", pm.ins[ii].nStartCamset, pm.ins[ii].nStartCamset+pm.ins[ii].nCamsetNum );
		sql += sWk;


		//// 実行
		if( !clsDB.ExecuteDirect(sql)) {
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
			return false;
		}

		//// 取得
		while (TRUE) {
			int sqlrc = clsDB.Fetch();
			if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り
			
				// 主キー
				clsDB.GetData(1, &camset);
				if( MAX_CAMSET < camset || 0 >= camset ) {
					// 主キー不正
					LOG(em_ERR), "[%s] [%s 主キー不正 camset=%d]", my_sThreadName, cDbName, camset);
					syslog(SYSNO_DB_DATA_ERR, "[%s camset=%d]", cDbName, camset);					
					continue;
				}
				camset = camset - pm.ins[ii].nStartCamset;		// 使いやすいように0オリジンに変更

				// データ部
				// 正・乱別
				clsDB.GetData(2, &buf.ins[ii].camset[camset].dResWidth );
				clsDB.GetData(3, &buf.ins[ii].camset[camset].dXoffset );

				buf.ins[ii].camset[camset].bIsEnable = true;

			} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
				break;	
			} else {								// エラー
				syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
				return false;
			}
		}
	}

	//// 取得データをメンバー変数にセット
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, cDbName);
	return true;
}

//------------------------------------------
// 有効カメラ取得
//------------------------------------------
bool ParamManager::Read_Param_CamEnable(P_MAIN& pm)
{
/***
	CString sql;					// SQL文
	CString	sWk;
	int camset;

	//// パラメータ初期化
	CString cDbName;
	cDbName = DB_HT_ENABLE;
	P_PARAM_CAMSET&	p = m_Param_CamSet;


	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//-------------------------
	// カメラ不使用設定
	for(int ii=0; ii<pm.nIns; ii++) {
		//// SQL文作成
		sql =  "SELECT";
		sql += " camset";
		sql += ", 動作可否";

		sql += " FROM ";
		sql += cDbName;
		sWk.Format(" WHERE camset>=%d and camset<%d", pm.ins[ii].nStartCamset, pm.ins[ii].nStartCamset+pm.ins[ii].nCamsetNum );
		sql += sWk;


		//// 実行
		if( !clsDB.ExecuteDirect(sql)) {
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
			return false;
		}

		//// 取得
		while (TRUE) {
			int sqlrc = clsDB.Fetch();
			if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り
			
				// 主キー
				clsDB.GetData(1, &camset);
				if( MAX_CAMSET < camset || 0 >= camset ) {
					// 主キー不正
					LOG(em_ERR), "[%s] [%s 主キー不正 camset=%d]", my_sThreadName, cDbName, camset);
					syslog(SYSNO_DB_DATA_ERR, "[%s camset=%d]", cDbName, camset);					
					continue;
				}
				camset = camset - pm.ins[ii].nStartCamset;		// 使いやすいように0オリジンに変更

				// データ部
				int wk;
				clsDB.GetData(2, &wk);
				p.ins[ii].camset[camset].bIsEnable = 1 == wk ? FALSE : TRUE;


			} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
				break;	
			} else {								// エラー
				syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
				return false;
			}
		}
	}
	//// 取得データをメンバー変数にセット
	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, cDbName);
***/
	return true;
}

//------------------------------------------
// 共通パラメータ 読込
//------------------------------------------
bool ParamManager::Read_Param_Common()
{
	CString sql;					// SQL文
	double wk;

	//// パラメータ初期化
	CString cDbName;
	cDbName.Format("%s CROSS JOIN %s",  DB_PARAM_COMMON, DB_COMMON_MASTER) ;


	P_PARAM_COMMON	buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL文作成
	//sql =  "SELECT TOP 1 [PLGパルスレート], [カメラPLG逓倍率], [カメラPLG分周率], [全長画像_間引き率横], [全長画像_間引き率縦]";
	sql =  "SELECT TOP 1 [PLGパルスレート], [カメラPLG逓倍率], [カメラPLG分周率], [ShFrameShrinkX], [ShFrameShrinkY]";
	sql += " FROM ";
	sql += cDbName;
		
	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り

			// データ部
			clsDB.GetData(1, &buf.dPls );
			clsDB.GetData(2, &buf.nMul );
			clsDB.GetData(3, &buf.nDiv);

			clsDB.GetData(4, &buf.nImgCullX);
			clsDB.GetData(5, &buf.nImgCullY);

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	//// 取得データをメンバー変数にセット
	memcpy( &m_Param_Common, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, cDbName);
	return true;
}

//------------------------------------------
// 取得したパラメータから 必要な情報を算出する
// P_MAIN& buf セット用エリア
//------------------------------------------
void ParamManager::CalcParam(P_MAIN& buf)
{
	int ii, jj;

	// 整合性チェック
	if( buf.nIns < 1 )			_ASSERT(false);
	if( buf.nIns > MAX_INS )	_ASSERT(false);


	////// 共通
	//if( COLOR_TYPE_GRAY == buf.emColorType ) { buf.nImgNumTotal = 1; buf.nImgNumRaw = 1; }
	//if( COLOR_TYPE_COLOR== buf.emColorType ) { buf.nImgNumTotal = 3; buf.nImgNumRaw = 3; }	// RGBの3画像、RGBの3画像
	//if( COLOR_TYPE_SA== buf.emColorType )	 { buf.nImgNumTotal = 4; buf.nImgNumRaw = 3; }	// RGB,R-Bの4画像、RGBの3画像


	//// 前段で単純に計算できる間引き座標関係
	buf.nFrameSizeX = CalcCullX(buf.nFrameSizeX);
	buf.nFrameSizeY = CalcCullY(buf.nFrameSizeY);
	buf.nGrabSizeX = CalcCullX(buf.nGrabSizeX);
	buf.nGrabSizeY = CalcCullY(buf.nGrabSizeY);

	for(ii=0; ii<buf.nIns; ii++) {
		for(jj=0; jj<buf.ins[ii].nCamsetNum; jj++) {

			buf.ins[ii].camset[jj].dResWidth = m_Param_CamSet.ins[ii].camset[jj].dResWidth * m_Param_Common.nImgCullX;
		}
	}





	//// １段目
	int			nMaxWidth = 0;									// 最大の画像幅 [pixel]
	int			nSyncWidth = 0;									// 幅方向同期する画像幅 [pixel]
	int			nRawWidth[MAX_INS];								// 純粋な画像幅 [pixel]
	for(ii=0; ii<buf.nIns; ii++) {
		double	dResXSum = 0.0;									// 平均横分解能計算用ワーク
		int		nLetfStart = 0;									// 1つ前のカメラの左開始位置
		int		nOldLap = 0;									// 1つ前のカメラの右オーバーラップ量
		double	dWk = 0.0;

		for(jj=0; jj<buf.ins[ii].nCamsetNum; jj++) {
			//ParamManager::P_PARAM_CAMSET::INS::CAMSET& prm = m_Param_CamSet.ins[ii].camset[jj];
			P_MAIN::INS::CAMSET& set = buf.ins[ii].camset[jj];

			// ---------------------------------------
			// 連結無し
			if( -1 == buf.ins[ii].nOverLapMode ) {	
				set.nOverLap		= 0;
				set.nOverLapL	= 0;
				set.nBaseLeft	= buf.nFrameSizeX * jj;
				set.nLapStartBase= buf.nFrameSizeX;

			// ---------------------------------------
			// 連結時
			} else {

				if( jj != buf.ins[ii].nCamsetNum-1 ) {
					dWk = m_Param_CamSet.ins[ii].camset[jj+1].dXoffset - m_Param_CamSet.ins[ii].camset[jj].dXoffset;					// 自カメラ内と次カメラが重なっている範囲 (mm系)
					//set.nOverLap = buf.nFrameSizeX - (int)(dWk / set.dResWidth);		// 重なっている範囲 (pixel系)
					set.nOverLap = buf.nGrabSizeX - (int)(dWk / set.dResWidth);		// 重なっている範囲 (pixel系)
					int nOverLapResult = (set.nOverLap < MAX_OVERLAP ? set.nOverLap : MAX_OVERLAP);
		
					if(0 < set.nOverLap) {
						//set.nLapStartBase = buf.nFrameSizeX - set.nOverLap;
						//set.nLapStartBaseResult = buf.nFrameSizeX - nOverLapResult;
						set.nLapStartBase = buf.nGrabSizeX - set.nOverLap;
						set.nLapStartBaseResult = buf.nGrabSizeX - nOverLapResult;
					} else {
						// 右オーバーラップ部が無い
						set.nOverLap = 0;
						//set.nLapStartBase = buf.nFrameSizeX;
						//set.nLapStartBaseResult = buf.nFrameSizeX;
						set.nLapStartBase = buf.nGrabSizeX;
						set.nLapStartBaseResult = buf.nGrabSizeX;
					}

				} else {
					// 一番最後のカメラは無条件で右オーバーラップは無い
					set.nOverLap = 0;
					//set.nLapStartBase = buf.nFrameSizeX;
					//set.nLapStartBaseResult = buf.nFrameSizeX;
					set.nLapStartBase = buf.nGrabSizeX;
					set.nLapStartBaseResult = buf.nGrabSizeX;
				}
				set.nOverLapL = nOldLap;

				// 左端からのオフセット値算出
				if(0 != jj) {
					set.nBaseLeft = nLetfStart + buf.ins[ii].camset[jj-1].nLapStartBase;
				} else {
					set.nBaseLeft = 0;					// 左端カメラは常に0
				}
			}

			// 次のため
			dResXSum   += set.dResWidth;
			nLetfStart	= set.nBaseLeft;
			nOldLap		= set.nOverLap;
		}


		buf.ins[ii].reso_x = (float)(dResXSum / buf.ins[ii].nCamsetNum); 	// 横分解能
		buf.ins[ii].reso_y = (float) this->CalcResV() * m_Param_Common.nImgCullY;
		buf.ins[ii].nLineCenter = (int)(abs( m_Param_CamSet.ins[ii].camset[0].dXoffset ) / buf.ins[ii].reso_x);		// ラインセンター [pixel]


		// 画像サイズ (幅は、仮決め)
		//nRawWidth[ii]	= nLetfStart + buf.nFrameSizeX;	
		nRawWidth[ii]	= nLetfStart + buf.nGrabSizeX;	
		nMaxWidth		= max( nMaxWidth, nRawWidth[ii]);
		if(buf.ins[ii].bWidthSync ) nSyncWidth = max( nSyncWidth, nRawWidth[ii]);
	}


	//// ２段目
	// 画像幅を決定
	buf.nMaxHeight = buf.nFrameSizeY;
	buf.nMaxWidth  = GET_SIZE_JUST(nMaxWidth, 16); // (((nMaxWidth-1) / 8) + 1)*8;						// 8の倍数に変更

	for(int ii=0; ii<buf.nIns; ii++) {
		P_MAIN::INS& set = buf.ins[ii];

		set.nUionHeight = buf.nFrameSizeY;

		// 画像幅を決定
		if(set.bWidthSync ) {
			set.nUionWidth   = GET_SIZE_JUST(nSyncWidth, 16); //(((nSyncWidth-1) / 8) + 1)*8;						// 8の倍数に変更
			set.nInsertRight = set.nUionWidth - nRawWidth[ii];			// 右端黒挿入量 [pixel]
		} else {
			set.nUionWidth   = GET_SIZE_JUST(nRawWidth[ii], 16); //(((nRawWidth[ii]-1) / 8) + 1)*8;					// 8の倍数に変更
			set.nInsertRight = set.nUionWidth - nRawWidth[ii];			// 右端黒挿入量 [pixel]
		}
	}


	// ------------------------
	// オーバーラップ量
	for(int ii=0; ii<buf.nIns; ii++) {
		for(jj=0; jj<buf.ins[ii].nCamsetNum; jj++) {

			P_MAIN::INS::CAMSET& set = buf.ins[ii].camset[jj];

			//// 左右オーバーラップ (何も無し。連結のみ)
			if( -1 == buf.ins[ii].nOverLapMode ) {
				
				set.nCopyImg[0] = 0;
				//set.nCopyImg[1] = buf.nFrameSizeX;
				set.nCopyImg[1] = buf.nGrabSizeX;

				set.nHokanImg[0] = 0;
				set.nHokanImg[1] = 0;
			}

			//// 左右オーバーラップ (スムージング)
			if( 0 == buf.ins[ii].nOverLapMode ) {
				set.nCopyImg[0] = set.nOverLapL;
				set.nCopyImg[1] = set.nLapStartBaseResult - set.nCopyImg[0];
				if( 0 > set.nCopyImg[1]) set.nCopyImg[1] = 0;

				set.nHokanImg[0] = set.nLapStartBaseResult;
				//set.nHokanImg[1] = buf.nFrameSizeX - set.nLapStartBaseResult;
				set.nHokanImg[1] = buf.nGrabSizeX - set.nLapStartBaseResult;
			}

			//// 左右オーバーラップ (左フレーム優先)
			if( 1 == buf.ins[ii].nOverLapMode ) {
				set.nCopyImg[0] = set.nOverLapL;
				set.nCopyImg[1] = set.nLapStartBase + set.nOverLap - set.nOverLapL;

				set.nHokanImg[0] = 0;
				set.nHokanImg[1] = 0;
			}

			//// 左右オーバーラップ (右フレーム優先)
			if( 2 == buf.ins[ii].nOverLapMode ) {
				set.nCopyImg[0] = 0;
				set.nCopyImg[1] = set.nLapStartBase;

				set.nHokanImg[0] = 0;
				set.nHokanImg[1] = 0;
			}
		}
	}

	
	//// ビットマップヘッダ生成
	KizuBitmapManager	kb;
	buf.nBmpHeadSize = kb.GetBitmapHeaderSize_Gray(); 
	for(int ii=0; ii<buf.nIns; ii++) {
		//kb.CreateBitmapHeader_Gray( buf.ins[ii].nUionWidth, buf.ins[ii].nUionHeight, buf.ins[ii].BmpHead, false); // LAC時に使用するため、画像を反転させておく。(=用は普通の疵検のBMP画像の通り)
		kb.CreateBitmapHeader_Gray( buf.ins[ii].nUionWidth, buf.ins[ii].nUionHeight, buf.ins[ii].BmpHead, buf.ins[ii].bImgTorB); // LAC時に使用するため、画像を反転させておく。(=用は普通の疵検のBMP画像の通り)
	}



	//// 取得データをメンバー変数にセット
	LOG(em_MSG), "[%s] 計算完了 [画像サイズ x=%d/Y=%d]", my_sThreadName, buf.nMaxWidth, buf.nMaxHeight );
}



//------------------------------------------
// コイル単位でパラメータ読込直し	※ ちゃんと先頭フレームから反映
// int nIns InsNo (0オリジン)
//------------------------------------------
void ParamManager::Read_IniCoilChange(int nIns)
{
	CString	sWk, sWk2, sWk3;
	sWk.Format("%s", TI_RPISAVE);
	sWk2.Format("%s_INS_%d", TI_RPISAVE, nIns+1);

	// まずはローカルに読込
	EM_COMP_TYPE emCompType = (EM_COMP_TYPE) GetPrivateProfileInt(sWk, "COMP_TYPE", 0, TASKINI_NAME );
	int nCompQualityBase = GetPrivateProfileInt(sWk2, "COMP_QUALITY", 1, TASKINI_NAME );

	int nComp[MAX_PCODE];
	for (int ii = 0; ii<MAX_PCODE; ii++) {
		sWk3.Format("COMP_%d", ii + 1);
		nComp[ii] = GetPrivateProfileInt(sWk, sWk3, 0, TASKINI_NAME);
	}


	// 反映
	m_Main.ins[nIns].emCompType = emCompType;
	m_Main.ins[nIns].nCompQualityBase = nCompQualityBase;
	memcpy(m_Main.ins[nIns].nCompQuality, nComp, sizeof(nComp));
}

//------------------------------------------
// 輝度値更新			※ 読込時即時反映なので、注意
// int nIns InsNo (0オリジン)
//------------------------------------------
void ParamManager::Read_IniKidoChange(int nIns)
{
	char	cKidoPath[256];					// 輝度補正パス
	CString	sWk, sWk2;
	sWk.Format("%s_INS_%d", TI_RPISAVE, nIns + 1);


	ParamManager::P_MAIN::INS& prmI = m_Main.ins[nIns];


	for (int jj = 0; jj<m_Main.ins[nIns].nCamsetNum; jj++) {
		// 輝度補正
		sWk2.Format("KIDO_PATH_%d", jj + 1);
		GetPrivateProfileString(sWk, sWk2, "", cKidoPath, sizeof(cKidoPath), TASKINI_NAME);
		if (0 == strlen(cKidoPath)) {
			prmI.camset[jj].bKidoUmu = false;
			m_vKido[nIns][jj].clear();
			continue;
		}

		// 読込	
		CsvIniFunc	cls;
		std::vector<CString>	v;
		std::vector<short>		vB;
		if (!cls.ReadCsv(KIDO_SEC, cKidoPath, &v)) {
			LOG(em_ERR), "[%s] 輝度補正ファイルオープンエラー[%s]", my_sThreadName, cKidoPath);
			prmI.camset[jj].bKidoUmu = false;
			m_vKido[nIns][jj].clear();
			continue;
		}
		for (DWORD ii = 0; ii<v.size(); ii++) {
			if (ii > m_Main.nFrameSizeX) break;

			vB.push_back((short)atoi(v[ii]));
		}

		// 反映
		m_vKido[nIns][jj] = vB;
		prmI.camset[jj].bKidoUmu = true;

		LOG(em_INF), "[%s] 輝度補正ファイル取得[%s][%s] Path[%s]", my_sThreadName, sWk, sWk2, cKidoPath);
	}
}
