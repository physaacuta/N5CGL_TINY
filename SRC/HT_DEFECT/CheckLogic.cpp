#include "stdafx.h"
#include "CheckLogic.h"

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
CheckLogic::CheckLogic() :
ThreadManager("CheckLogic"),
m_bLogicUmu(DIV_BOOL_NON)
{
}

//------------------------------------------
// デストラクタ
//------------------------------------------
CheckLogic::~CheckLogic()
{
}


//------------------------------------------
// scode のチェック処理
// int scode	表面状態 (0:全て 1～:該当scodeのみ)
//------------------------------------------
void CheckLogic::Check(int scode)
{
	TYP_DELEV* p = new TYP_DELEV;
	p->scode = scode;
	if( ! gque_Check.AddToTailFreeCheck(p) ) {
		delete p;
		LOG(em_ERR), "[%s] gque_Check キューフル!!", my_sThreadName);
	}
}


//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int CheckLogic::ThreadFirst()
{
	//// イベント設定
	AddEventNum(1, &gque_Check.g_evSem );
	return 0;
}


//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void CheckLogic::ThreadEvent(int nEventNo)
{
	////// シグナルの条件		
	enum {	EV_QUE = 0,									// キュー
			EV_END	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_QUE:										// チェック
		{
			TYP_DELEV* pType = gque_Check.GetFromHead();
			LOG(em_MSG), "[%s] Logicチェック依頼 <%d>", my_sThreadName, pType->scode );
			m_bLogicUmu = ExecLogic(pType->scode);
			delete pType;
		}
		break;

//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		break;
	}
}

//------------------------------------------
// ロジックファイル有無チェック
// int scode	表面状態 (0:全て 1～:該当scodeのみ)
//------------------------------------------
EM_DIV_BOOL CheckLogic::ExecLogic(int scode)
{
	CString	sql;					// SQL文
	EM_DIV_BOOL	ans = DIV_BOOL_TRUE;
	
	int  val;
	char path[512];

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME) )	{
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return DIV_BOOL_NON;
	}


	//// SQL文作成
	CString sWhere = "";
	if( 0 != scode ) { sWhere.Format("AND s.scode=%d", scode); }

	sql.Format("SELECT MIN(s.scode), s.DCDname FROM %s AS s INNER JOIN %s AS m ON s.scode=m.scode WHERE m.有効判定=1 %s GROUP BY s.DCDname",
		DB_PARAM_SCODE, DB_SCODE_MASTER, sWhere);

	//// 実行
	if( ! clsDB.ExecuteDirect(sql) )
	{
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", sql);
		return DIV_BOOL_NON;
	}

	//// 取得
	while( TRUE ) {
		int sqlrc = clsDB.Fetch();
		if( sqlrc == KIZUODBC_FETCH_OK )					// データ有り
		{
			clsDB.GetData(1, &val);
			clsDB.GetData(2, path, sizeof(path));

			// ファイルチェック
			{
				CFileFind fFind;
				if( !fFind.FindFile(path, 0) ) {
					ans = DIV_BOOL_FALSE;
					LOG(em_ERR), "[%s] ロジックファイル無し (%d)(%s)", my_sThreadName, val, path);
					syslog(120, "(%d)(%s)", val, path);
				} else {
					LOG(em_MSG), "[%s] ロジックファイルOK (%d)(%s)", my_sThreadName, val, path);
				}
				fFind.Close();
			}

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) { break; }		// データ無し
		else {											// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", sql);
			return DIV_BOOL_NON;
		}
	}

	return ans;
}