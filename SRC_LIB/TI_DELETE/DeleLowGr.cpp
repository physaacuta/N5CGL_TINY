#include "StdAfx.h"
#include "DeleLowGr.h"

//------------------------------------------
// コンストラクタ
// int id 自分のID (1オリジン)
// HANDLE evEndExec 実行完了シグナル
// SETTING_TBL const* pTyp 削除テーブル情報
//------------------------------------------
DeleLowGr::DeleLowGr(int id, HANDLE evEndExec, SETTING_TBL const* pTyp) :
ThreadManager( GetThreadNameArray("LowGr", id) ),
m_nID(id),
m_evEndExec(evEndExec),
m_pTypSet(pTyp),
m_bIsExec(false),
m_bStopFlg(false)
{
	char		cWk[1024];
	GetPrivateProfileString(TI_DELETE, "SQL_SELECT_END", "", cWk, sizeof(cWk), TASKINI_NAME );
	m_cIniSqlSelectEnd = cWk;


	// もろもろ初期化
	memset(m_cKey, 0x00, sizeof(m_cKey));

	// イベント生成
	m_evExec = CreateEvent(NULL, FALSE, FALSE, NULL);

}


//------------------------------------------
// デストラクタ
//------------------------------------------
DeleLowGr::~DeleLowGr(void)
{
	// イベント破棄
	CloseHandle(m_evExec);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// スレッド操作

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int DeleLowGr::ThreadFirst()
{
	//// イベント設定
	HANDLE hArray[] = { m_evExec			// 削除実行依頼
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int DeleLowGr::ThreadLast()
{
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void DeleLowGr::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_EXEC = 0,						// 削除実行依頼
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_EXEC:											// 削除実行依頼
		LOG(em_MSG), "[%s] 無害Gr削除 実行依頼 [%s]", my_sThreadName, m_cKey);
		if(m_bStopFlg) break;		// 終了？

		// テーブルに削除した結果を反映
		if( ! StartExec() ) {
			// 終わり
			m_bIsExec = false;// 念の為
			memset(m_cKey, 0x00, sizeof(m_cKey));
		}
		break;
//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// メイン処理

//------------------------------------------
// 無害Gr削除開始
//------------------------------------------
bool DeleLowGr::StartExec()
{
	CString sql;
	int		nSubNo;

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME, INI_DB)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] [%s] DB接続エラー", my_sThreadName, INI_DB);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// 実績削除
	for(int ii=0; ii<m_pTypSet->nMainCnt; ii++) {
		while(true) {
			//// 今回の削除対象の疵Noを取得
			sql.Format("SELECT ISNULL(MAX(a.%s),0) FROM (SELECT TOP %d %s FROM %s \
					   WHERE %s='%s' %s ORDER BY %s DESC, %s ASC) AS a",
				m_pTypSet->cKeySub, DELETE_MAXCNT, m_pTypSet->cKeySub, m_pTypSet->main[ii].cTblMain, 
				m_pTypSet->cKeyMain, m_cKey, m_pTypSet->main[ii].cSqlWhere, m_pTypSet->cKeyMain, m_pTypSet->cKeySub );
			clsDB.GetSelectKey(sql, &nSubNo);
			if(m_bStopFlg) return false;		// 終了？

			//// 終わり？
			if(0 == nSubNo) break;


			//// 削除テーブルの実績を削除
			for(int jj=0; jj<m_pTypSet->main[ii].nSubCnt; jj++) {
				if( ! DeleteInf( &clsDB, m_pTypSet->main[ii].cTblMain, m_pTypSet->main[ii].cSqlWhere, m_pTypSet->main[ii].cTblSub[jj], m_cKey, nSubNo) ) return false;
				if(m_bStopFlg) return false; // 終了？
				Sleep(SLEEP_TIME);
				if(m_bStopFlg) return false; // 終了？
			}

			//// キーテーブルの実績を削除
			if( ! DeleteInf( &clsDB, m_pTypSet->main[ii].cTblMain, m_pTypSet->main[ii].cSqlWhere, m_pTypSet->main[ii].cTblMain, m_cKey, nSubNo) ) return false;
			if(m_bStopFlg) return false; // 終了？

			LOG(em_MSG), "[%s] 削除中 ･･･ [%s] [%s] 疵No=%d まで完了", my_sThreadName, m_pTypSet->main[ii].cTblMain, m_cKey, nSubNo);
		}
	}

	//// 管理テーブル更新
	SetDeleteOkFlg(&clsDB, m_cKey);
	clsDB.ExecuteDirect_ShrinkFile();				// DB圧縮 
	return true;
}

//------------------------------------------
// 無害Gr削除実行
// OdbcBase* pClsDB DB操作クラス
// char const* cKeyTbl 基準となるテーブル (DEFECT_INFのみ)
// char const* cKeyWhereSql 抽出SQL
// char const* cDeleTbl 削除対象となる実績テーブル
// char const* cKey 対象の疵検管理No
// int nSubNo 対象の最大疵No
//------------------------------------------
bool DeleLowGr::DeleteInf(OdbcBase* pClsDB, char const* cKeyTbl, char const* cKeyWhereSql, char const* cDeleTbl, char const* cKey, int nSubNo)
{
	CString sql;

	// DEFECT_INFの無害一時保存フラグに紐付く 疵No を削除する
	sql.Format("DELETE FROM %s WHERE %s='%s' AND %s IN \
			   (SELECT %s FROM %s WHERE %s='%s' AND %s<=%d %s)", 
		cDeleTbl, m_pTypSet->cKeyMain, cKey, m_pTypSet->cKeySub, 
		m_pTypSet->cKeySub, cKeyTbl, m_pTypSet->cKeyMain, cKey, m_pTypSet->cKeySub, nSubNo, cKeyWhereSql);

	if( !pClsDB->ExecuteDirect(sql) ) {
		LOG(em_ERR), "[%s] %s削除失敗", my_sThreadName, cDeleTbl);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDeleTbl);
		return false;
	}
	return true;
}

//------------------------------------------
// 無害Gr削除管理テーブルをアップデート
// OdbcBase* pClsDB DB操作クラス
// char const* cKey 対象の疵検管理No
//------------------------------------------
bool DeleLowGr::SetDeleteOkFlg(OdbcBase* pClsDB, char const* cKey)
{
	CString sql;

	// 管理テーブルを更新
	if( 0 != m_cIniSqlSelectEnd.GetLength() ) {
		sql.Format(m_cIniSqlSelectEnd, cKey);
		if( !pClsDB->ExecuteDirect(sql) ) {
			LOG(em_ERR), "[%s] 更新失敗 (%s)", my_sThreadName, m_cIniSqlSelectEnd);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", m_cIniSqlSelectEnd);
			return false;
		}
	}

	// 正常完了時のみ
	LOG(em_MSG), "[%s] 無害Gr削除完了[%s]", my_sThreadName, cKey);
	m_bIsExec = false;
	memset(m_cKey, 0x00, sizeof(m_cKey));
	Sleep(SLEEP_TIME);

	SetEvent(m_evEndExec);
	return true;
}