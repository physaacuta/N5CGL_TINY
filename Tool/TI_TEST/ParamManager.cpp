#include "StdAfx.h"
#include "ParamManager.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


//#include "MainInctance.h"

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 基準

//------------------------------------------
// コンストラクタ
//------------------------------------------
ParamManager::ParamManager(void):
ThreadManager("ParamMgr")
{
	m_evRead	= CreateEvent(NULL, FALSE, FALSE, NULL);	
}

//------------------------------------------
// デストラクタ
//------------------------------------------
ParamManager::~ParamManager(void)
{
	CloseHandle(m_evRead);
}


//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int ParamManager::ThreadFirst()
{
	//// イベント設定
	HANDLE hArray[] = { m_evRead
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
	enum {	EV_READ = 0,								// 非同期読込
			EV_END	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_READ:										// 非同期読込
		LOG(em_MSG), "[%s] DBパラメータ 非同期読込依頼", my_sThreadName);
		ReadParam_All();
		break;

//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		break;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// パラメータ取得関連

//------------------------------------------
// 全パラメータ読込み
//------------------------------------------
bool ParamManager::ReadParam_All()
{

	//// 一時バッファに
	if( ! Read_CommonMast() )		return false;


	//// 一時バッファから本番に
	memcpy(&m_CommonMast, &m_CommonMastTemp, sizeof(P_COMMON_MAST));

	LOG(em_MSG), "[%s] 読込パラメータに結果反映", my_sThreadName);
	return true;
}

//------------------------------------------
// 疵検定数パラメータ読込み
//------------------------------------------
bool ParamManager::Read_CommonMast()
{
	CString	sql;					// SQL文

	//// パラメータ初期化
	CString	cDbName;
	//cDbName = DB_COMMON_MASTER;
	cDbName.Format("%s CROSS JOIN %s",  DB_PARAM_COMMON, DB_COMMON_MASTER) ;
	P_COMMON_MAST & p = m_CommonMastTemp;
	P_COMMON_MAST   buf;
	memset(&buf, 0x00, sizeof(buf));

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME) )
	{
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL文作成
	//sql = "SELECT";
	//sql += " 全長画像_間引き率横, 全長画像_間引き率縦";
	//sql += ", 全長画像_保存_R, 全長画像_保存_G, 全長画像_保存_B, 全長画像_保存_差";
	sql = "SELECT TOP 1 ";
	sql += " [ShFrameShrinkX], [ShFrameShrinkY]";
	sql += ", 全長画像_保存_R, 全長画像_保存_G, 全長画像_保存_B, 全長画像_保存_差";

	sql += " FROM ";
	sql += cDbName;

	//// 実行
	if( ! clsDB.ExecuteDirect(sql) )
	{
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	//// 取得
	while( TRUE )
	{
		int sqlrc = clsDB.Fetch();
		if( sqlrc == KIZUODBC_FETCH_OK )					// データ有り
		{
			// データ部
			clsDB.GetData(1, &buf.nImgCullX);
			clsDB.GetData(2, &buf.nImgCullY);

			clsDB.GetData(3, &buf.nImgSave[0]);
			clsDB.GetData(4, &buf.nImgSave[1]);
			clsDB.GetData(5, &buf.nImgSave[2]);
			clsDB.GetData(6, &buf.nImgSave[3]);


			// 自分で算出
			for(int ii=0; ii<MAX_IMAGE_CNT; ii++) {	
				if(TRUE == buf.nImgSave[ii]) buf.nImgSaveNum ++;
			}
		}
		else if( sqlrc == KIZUODBC_FETCH_NODATE )			// データ無し
		{
			break;
		}
		else												// エラー
		{
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	//// 取得データをメンバー変数にセット
	memcpy(&p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, cDbName);
	return true;
}
