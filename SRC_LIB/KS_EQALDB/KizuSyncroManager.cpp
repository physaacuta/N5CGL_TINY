#include "StdAfx.h"
#include "KizuSyncroManager.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// コンストラクタ
// CString cSession セッション
// LogFileManager* pLog ログファイルマネージャーポインタ
//------------------------------------------
KizuSyncroManager::KizuSyncroManager(CString cSession, LogFileManager* pLog) :
my_cSession(cSession),
mcls_pLog(pLog),
my_bStopFlg(false),
mcls_pDbA(NULL),
mcls_pDbB(NULL)
{
	//// シグナル生成
	my_evNowExecAtoB	 = CreateEvent(NULL, FALSE, FALSE, NULL);	
	my_evNowExecBtoA	 = CreateEvent(NULL, FALSE, FALSE, NULL);	


	//// 必要な情報をiniから取得
	GetPrivateProfileString(my_cSession, "DB_ODBC_A", "", my_cOdbcSession[0], sizeof(my_cOdbcSession[0]), TASKINI_NAME);
	GetPrivateProfileString(my_cSession, "DB_ODBC_B", "", my_cOdbcSession[1], sizeof(my_cOdbcSession[1]), TASKINI_NAME);
	GetPrivateProfileString(my_cSession, "LINK_PATH_AB", "", my_cLinkPath[0], sizeof(my_cLinkPath[0]), TASKINI_NAME);
	GetPrivateProfileString(my_cSession, "LINK_PATH_BA", "", my_cLinkPath[1], sizeof(my_cLinkPath[1]), TASKINI_NAME);

	my_nTimeFast	 = GetPrivateProfileInt(my_cSession, "TIME_FAST", -1, TASKINI_NAME);
	my_nTimeFast	 = my_nTimeFast * 60 * 1000;								// 分 → ms
	my_nTimeInterval = GetPrivateProfileInt(my_cSession, "TIME_INTERVAL", -1, TASKINI_NAME);
	my_nTimeInterval = my_nTimeInterval * 60 * 1000;							// 分 → ms
	my_nMailKey		 = GetPrivateProfileInt(my_cSession, "MAIL_KEY", 0, TASKINI_NAME);



	// チェック
	_ASSERT(strlen(my_cOdbcSession[0]));
	_ASSERT(strlen(my_cOdbcSession[1]));
	//_ASSERT(strlen(my_cLinkPath[0]));
	//_ASSERT(strlen(my_cLinkPath[1]));
	//_ASSERT(my_nTimeFast);
	//_ASSERT(my_nTimeInterval);
}


//------------------------------------------
// デストラクタ
//------------------------------------------
KizuSyncroManager::~KizuSyncroManager(void)
{
	// 開放
	CloseHandle(my_evNowExecAtoB);
	CloseHandle(my_evNowExecBtoA);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 共通メソッド

//------------------------------------------
// 即時実行シグナルコール
// int nAtoB  0:A→B    1:B→A
// int key 任意の値
// 戻り値：実行対象時true
//------------------------------------------
bool KizuSyncroManager::SetEvNowExec(int nAtoB, int nMailKey)
{
	// 自分のインスタンスが対象？
	if(0 == nMailKey) return false;
	if(nMailKey != my_nMailKey) return false;

	// 0:A→B    1:B→A
	if(0 == nAtoB) {
		SetEvent(my_evNowExecAtoB);
	} else if(1 == nAtoB) {
		SetEvent(my_evNowExecBtoA);
	}
	return true;
}

//------------------------------------------
// A系とB系のDBが生きているかチェックする
// 戻り値： trur:OK   false:NG
//------------------------------------------
bool KizuSyncroManager::CheckDB_Connect()
{
	CString		sql;
	int			nTbl;

	////// サーバーアクセス A系確認
	// ODBCを経由してアクセス可能か確認
	OdbcBase cls_DB;
	if( ! cls_DB.Connect( TASKINI_NAME, my_cOdbcSession[0]) ) return false;
	// A系のリンクサーバーを経由して、B系にアクセス可能か確認
	sql.Format("SELECT COUNT(*) FROM %s%s", my_cLinkPath[0], "SysObjects");
	if( ! cls_DB.GetSelectKey( sql, &nTbl ) ) return false;
	cls_DB.DisConnect();


	////// サーバーアクセス B系確認
	// ODBCを経由してアクセス可能か確認
	if( ! cls_DB.Connect( TASKINI_NAME, my_cOdbcSession[1]) ) return false;
	// B系のリンクサーバーを経由して、A系にアクセス可能か確認
	sql.Format("SELECT COUNT(*) FROM %s%s", my_cLinkPath[1], "SysObjects");
	if( ! cls_DB.GetSelectKey( sql, &nTbl ) ) return false;
	cls_DB.DisConnect();

	// ここまで来たらA系・B系ともにOK
	return true;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// マスター移動関係


//------------------------------------------
// 2DB間のテーブルを同じにする
// bool bAtoB    true:A→B  false:B→A
// char const* tbl		テーブル名称
//------------------------------------------
bool KizuSyncroManager::Synclock_Master(bool bAtoB, char const* tbl)
{
	bool		bAns = true;
	CString		sql;
	if(NULL == mcls_pDbA) return false; 
	if(NULL == mcls_pDbB) return false; 


	//// DB接続チェック (通常接続されているはずだが、念の為)
	// A系
	if( ! mcls_pDbA->Connect(TASKINI_NAME, my_cOdbcSession[0] ) ) {
		// データベース接続エラー
		LOG(em_ERR), "[Synclock_Master] DB接続エラー [%s:%s]", my_cSession, tbl);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}
	// B系
	if( ! mcls_pDbB->Connect(TASKINI_NAME, my_cOdbcSession[1] ) ) {
		// データベース接続エラー
		LOG(em_ERR), "[Synclock_Master] DB接続エラー [%s:%s]", my_cSession, tbl);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	// これ以降で 異常で処理抜けする場合は、ロールバック等を必ずする事
	mcls_pDbA->AutoCommitEnable(false);				// トランザクションを有効化
	mcls_pDbB->AutoCommitEnable(false);				// トランザクションを有効化	


	// -------------------------------------------------------
	// 【主な流れ】
	// ・トランザクション開始
	// ・コピー先のテーブルを全件削除
	// ・コピー元からコピー先へ、一括で全部登録
	// ・コミット
	// 【注意】
	// ・Toが基準となる。
	// -------------------------------------------------------

	//// アクセス文字列生成
	CString		sFromTblAccessPass;		// コピー先DBからリンクサーバー経由で元DBにアクセスする時の 元テーブルパス
	OdbcBase*	cls_pDbFrom;			// 元DB
	OdbcBase*	cls_pDbTo;				// 先DB
	if(bAtoB) {
		sFromTblAccessPass.Format("%s%s", my_cLinkPath[1], tbl);
		cls_pDbFrom = mcls_pDbA;
		cls_pDbTo	= mcls_pDbB;
	} else {
		sFromTblAccessPass.Format("%s%s", my_cLinkPath[0], tbl);
		cls_pDbFrom = mcls_pDbB;
		cls_pDbTo	= mcls_pDbA;
	}


	//// コピー先のテーブルを全件削除
	sql.Format("DELETE FROM %s", tbl);
	if( !cls_pDbTo->ExecuteDirect(sql, false)) {	// トランザクション有効中なので、異常時の自動切断はしない
		LOG(em_ERR), "[Synclock_Master] 削除 [%s:%s]", my_cSession, sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s:%s]", my_cSession, tbl);

		bAns = false;
		goto Ending;
	}


	//// コピー元からコピー先へ、一括で全部登録
	sql.Format("INSERT INTO %s SELECT * FROM %s",
		tbl, sFromTblAccessPass);
	if( !cls_pDbTo->ExecuteDirect(sql, false)) {	// トランザクション有効中なので、異常時の自動切断はしない
		LOG(em_ERR), "[Synclock_Master] マスター同期異常 [%s:%s]", my_cSession, sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s:%s]", my_cSession, tbl);
		
		bAns = false;
		goto Ending;
	}


	//// 終了処理 (必須)
Ending:
	if(bAns) {
		// コミット
		cls_pDbTo->TranCommit();
		LOG(em_INF), "[Synclock_Master] マスター同期完了 [%s:%s]", my_cSession, tbl);
	} else {
		// ロールバック
		cls_pDbTo->TranRollBack();
	}

	return bAns;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 実績移動関係


//------------------------------------------
// 少量コピー
// bool bAtoB    true:A→B  false:B→A
// char const* tbl		テーブル名称
// int nExecCnt １回の処理件数
// int nKeyCnt 有効主キー数
// char const* key 主キー配列
//------------------------------------------
bool KizuSyncroManager::Copy_DB(bool bAtoB, char const* tbl, int nExecCnt, int nKeyCnt, char const* key)
{
	bool		bAns = true;
	if(NULL == mcls_pDbA) return false; 
	if(NULL == mcls_pDbB) return false; 

	char const* key1 = &key[PARAM_MAX_STRING*0];
	char const* key2 = &key[PARAM_MAX_STRING*1];
	char const* key3 = &key[PARAM_MAX_STRING*2];

	//// DB接続チェック (通常接続されているはずだが、念の為)
	// A系
	if( ! mcls_pDbA->Connect(TASKINI_NAME, my_cOdbcSession[0] ) ) {
		// データベース接続エラー
		LOG(em_ERR), "[Copy_DB] DB接続エラー [%s:%s]", my_cSession, tbl);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}
	// B系
	if( ! mcls_pDbB->Connect(TASKINI_NAME, my_cOdbcSession[1] ) ) {
		// データベース接続エラー
		LOG(em_ERR), "[Copy_DB] DB接続エラー [%s:%s]", my_cSession, tbl);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	// -------------------------------------------------------
	// 【主な流れ】
	// ・トランザクション開始  // コピーだけなので、トランザクションヤメ
	// ・コピー先の最新位置Aを特定
	// ・コピー元のA以降のデータを、コピー先へ指定件数コピー (古いやつから順番にコピーとなる)
	// ・コミット
	// ・待ち
	// ・繰り返し 最初へ
	// 【注意】
	// ・Toが基準となる。
	// -------------------------------------------------------

	//// アクセス文字列生成
	CString		sFromTblAccessPass;		// コピー先DBからリンクサーバー経由で元DBにアクセスする時の 元テーブルパス
//	OdbcBase*	cls_pDbFrom;			// 元DB。FromにはTOのLINKSERVER経由でアクセスするのでコメント化。
	OdbcBase*	cls_pDbTo;				// 先DB
	if(bAtoB) {
		sFromTblAccessPass.Format("%s%s", my_cLinkPath[1], tbl);
//		cls_pDbFrom = mcls_pDbA;
		cls_pDbTo	= mcls_pDbB;
	} else {
		sFromTblAccessPass.Format("%s%s", my_cLinkPath[0], tbl);
//		cls_pDbFrom = mcls_pDbB;
		cls_pDbTo	= mcls_pDbA;
	}

	////// コピー (ちょっとづつコピー)
	int nCntwk = 1;
	//コピーの方針
	//PKの型を考えだすと面倒なので、全部文字列で扱う！
	//WHERE [数値項目] > '300'なども、実行パフォーマンス上、問題ないはず。


	while(true) {
		CString sValPK[4];		//TO側にある最新のレコードのPK
		bool bToEmpty = false;	//TO側のテーブルは空だった、、、

		//自分の持っているなかで、一番新しいレコードのPKを取得
		{	
			CString sSqlFillMyPK = "SELECT TOP(1) ";
			for( int ii=0; ii<nKeyCnt; ii++){
				const char *key_ii = &key[PARAM_MAX_STRING*ii];
				sSqlFillMyPK += key_ii;
				sSqlFillMyPK += ",";
            }
			sSqlFillMyPK.TrimRight(',');
			sSqlFillMyPK.AppendFormat(" FROM %s ORDER BY ", tbl);
			for( int ii=0; ii<nKeyCnt; ii++){
				const char *key_ii = &key[PARAM_MAX_STRING*ii];
				sSqlFillMyPK += key_ii;
				sSqlFillMyPK += " desc,";
			}
			sSqlFillMyPK.TrimRight(',');
            
			if( cls_pDbTo->ExecuteDirect(sSqlFillMyPK) )
			{
				int nFetchResult = cls_pDbTo->Fetch();
				if( KIZUODBC_FETCH_OK == nFetchResult  ){
					for( int nCol=1; nCol<=nKeyCnt; nCol++){
						CString sPKwk;
						int len = cls_pDbTo->GetData(nCol, sPKwk.GetBuffer( 64 ), 64 );
						sPKwk.ReleaseBuffer();
                        //データが取れた
                        sValPK[nCol-1] =  sPKwk ;
                    }
                }
                else if( KIZUODBC_FETCH_NODATE == nFetchResult )
                {
                    //データが取れないとき用
                    for( int nCol=1; nCol<=nKeyCnt; nCol++){
                        sValPK[nCol-1] = "<NUL>";	//WHERE自体あとで吹っ飛ぶので、適当。
					}
					bToEmpty = true;
                }
                else{
                    LOG(em_ERR), "[Copy_DB] 実績コピーF [%s:%s]", my_cSession, sSqlFillMyPK);
					syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s:%s]", my_cSession, tbl);
					bAns = false;
					goto Ending;
				}
			}
			else{
				LOG(em_ERR), "[Copy_DB] 実績コピーE [%s:%s]", my_cSession, sSqlFillMyPK);
				syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s:%s]", my_cSession, tbl);
				bAns = false;
				goto Ending;
			}
		}

		
		//// コピー対象となるSQL文（WHERE句、ORDER BY句）の作成
		CString strSqlWhere;     
		CString strSqlOrderBy;
		
		if(1 == nKeyCnt) {
            CString	strSqlExecWk;
			strSqlExecWk += "WHERE ";
			strSqlExecWk += "(%s > '%s') ";
			
			strSqlWhere.Format(strSqlExecWk,
				key1, sValPK[0]);
			strSqlOrderBy.Format("ORDER BY %s asc", key1 );

		} else if(2 == nKeyCnt){
            CString	strSqlExecWk;
			strSqlExecWk += "WHERE ";
			strSqlExecWk +=  "(%s > '%s') OR ";
			strSqlExecWk += "((%s = '%s') AND (%s > '%s') ) ";
		
			strSqlWhere.Format(strSqlExecWk,
				key1, sValPK[0],
				key1, sValPK[0],     key2, sValPK[1] );
            strSqlOrderBy.Format( "ORDER BY %s asc, %s asc", key1, key2);
            
		} else if(3 == nKeyCnt){
            CString	strSqlExecWk;
			strSqlExecWk += "WHERE ";
			strSqlExecWk += "  (%s > '%s') OR ";
			strSqlExecWk += " ((%s = '%s') AND (%s > '%s') ) OR ";
			strSqlExecWk += " ((%s = '%s') AND (%s = '%s') AND (%s > '%s') ) ";
		
			strSqlWhere.Format(strSqlExecWk,
				key1, sValPK[0],
				key1, sValPK[0],     key2, sValPK[1],
				key1, sValPK[0],     key2, sValPK[1],     key3, sValPK[2] );

            strSqlOrderBy.Format("ORDER BY %s asc, %s asc, %s asc",	key1, key2, key3);
		
		} else {
			_ASSERT(false);	
		}

		//TOのテーブルは空なので、WHEREは削除。
		//削除するのは、VARCHAR(N), DATETIME, INTに共通で通用する初期値がない為。
		//  '0'だと、DATETIME時に、「0はDATETIMEに変換できません」とか言い出す。
		//　Intの0 ならいけるように見えるが、VARCHAR(N)のレコードが'6000000000'みたいに
		//　数値のみで構成されてると、「VARCHAR->INT変換 対INT」の比較になり
		//　「変換でオーバーフローした」とか言い出す。
		//　「VARCHAR対INT->VARCHAR変換」で比較するかと思ったのに。
		if( bToEmpty ){
			strSqlWhere = "";
		}

		//// Fromには、TOより新しいレコードはいくつあるの？
		int nToCopy = 0;
        {
            CString		sqlCnt;
            sqlCnt.Format("SELECT COUNT(*) FROM %s ", sFromTblAccessPass );
            sqlCnt.Append(strSqlWhere);
            
            cls_pDbTo->GetSelectKey(sqlCnt, &nToCopy);
		}

        //コピーしないといけないのかな？
        if(0 == nToCopy)  { goto Ending;}
        else			{ LOG(em_INF), "[Copy_DB] 実績コピー ･･･ %d回目(残%d)  %s, %s, %s", nCntwk, nToCopy,  sValPK[0], sValPK[1], sValPK[2]);}
      	nCntwk ++;

		// コピー開始
		CString		sqlExec;
		sqlExec.AppendFormat("INSERT INTO %s ", tbl );
		sqlExec.AppendFormat("SELECT TOP %d * FROM %s ", nExecCnt, sFromTblAccessPass);
		sqlExec.Append(strSqlWhere);
		sqlExec.Append(strSqlOrderBy);

		if( !cls_pDbTo->ExecuteDirect(sqlExec)) { //, false)) {		// トランザクション有効中なので、異常時の自動切断はしない
			LOG(em_ERR), "[Copy_DB] 実績コピーI [%s:%s]", my_cSession, sqlExec);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s:%s]", my_cSession, tbl);

			bAns = false;
			goto Ending;
		}

		//// 待ち
		Sleep(100);
		if(my_bStopFlg) goto Ending; // 終了？
	}

	//// 終了処理 (必須)
Ending:
	if(bAns) {
		LOG(em_INF), "[Copy_DB] 実績コピー完了 [%s:%s]", my_cSession, tbl);
	} else {

	}
	return bAns;
}

//------------------------------------------
// 少量移動
// bool bAtoB    true:A→B  false:B→A
// char const* tbl		テーブル名称
// int nExecCnt １回の処理件数
// int nKeyCnt 有効主キー数
// char const* key 主キー配列
//------------------------------------------
bool KizuSyncroManager::Move_DB(bool bAtoB, char const* tbl, int nExecCnt, int nKeyCnt, char const* key)
{
	bool		bAns = true;
	CString		sql;
	int			nowCnt;				// 現在の削除位置
	if(NULL == mcls_pDbA) return false; 
	if(NULL == mcls_pDbB) return false; 

	char const* key1 = &key[PARAM_MAX_STRING*0];
	char const* key2 = &key[PARAM_MAX_STRING*1];
//	char const* key3 = &key[PARAM_MAX_STRING*2];	// 3個目は未対応

	//// DB接続チェック (通常接続されているはずだが、念の為)
	// A系
	if( ! mcls_pDbA->Connect(TASKINI_NAME, my_cOdbcSession[0] ) ) {
		// データベース接続エラー
		LOG(em_ERR), "[Move_DB] DB接続エラー [%s:%s]", my_cSession, tbl);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}
	// B系
	if( ! mcls_pDbB->Connect(TASKINI_NAME, my_cOdbcSession[1] ) ) {
		// データベース接続エラー
		LOG(em_ERR), "[Move_DB] DB接続エラー [%s:%s]", my_cSession, tbl);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	// これ以降で 異常で処理抜けする場合は、ロールバック等を必ずする事
	mcls_pDbA->AutoCommitEnable(false);				// トランザクションを有効化
	mcls_pDbB->AutoCommitEnable(false);				// トランザクションを有効化	

	// -------------------------------------------------------
	// 【主な流れ】
	// ・トランザクション開始
	// ・コピー元からコピー先へ、指定件数コピー (新しいやつから順番に移動となる)
	// ・コピー元から指定件数削除
	// ・コミット
	// ・待ち
	// ・繰り返し 最初へ
	// 【注意】
	// ・Toが基準となる。
	// -------------------------------------------------------

	//// アクセス文字列生成
	CString		sFromTblAccessPass;		// コピー先DBからリンクサーバー経由で元DBにアクセスする時の 元テーブルパス
	OdbcBase*	cls_pDbFrom;			// 元DB
	OdbcBase*	cls_pDbTo;				// 先DB
	if(bAtoB) {
		sFromTblAccessPass.Format("%s%s", my_cLinkPath[1], tbl);
		cls_pDbFrom = mcls_pDbA;
		cls_pDbTo	= mcls_pDbB;
	} else {
		sFromTblAccessPass.Format("%s%s", my_cLinkPath[0], tbl);
		cls_pDbFrom = mcls_pDbB;
		cls_pDbTo	= mcls_pDbA;
	}


	////// 実績移動 (ちょっとづつ移動)
	while(true) {
		//// 移動位置決定 (ORDER BYは付けない。deleteで付けれなかった為。 でも主キーで登録、削除するので順番は変わらないはず)
		sql.Format("SELECT COUNT(*) FROM %s", tbl);
		cls_pDbFrom->GetSelectKey(sql, &nowCnt);
		if( 0 >= nowCnt ) break;					// データ無し 
		LOG(em_INF), "[Move_DB] 移動 [%s:%s] ･･･ 残り%d件", my_cSession, tbl, nowCnt);

		//// コピーするSQL文 (SQL文が難しくなるので、フォーマットのみ定義)
		sql.Format("INSERT INTO %s SELECT TOP %d * FROM %s ORDER BY %s desc",
			tbl, nExecCnt, sFromTblAccessPass, key1);
		if(2 <= nKeyCnt) {
			sql += ", ";
			sql += key2;
			sql += " desc";
		}

		//// コピー
		if( !cls_pDbTo->ExecuteDirect(sql, false)) {	// トランザクション有効中なので、異常時の自動切断はしない
			LOG(em_ERR), "[Move_DB] コピー [%s:%s]", my_cSession, sql);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s:%s]", my_cSession, tbl);

			bAns = false;
			goto Ending;
		}

		//// 削除するSQL文 (SQL文が難しくなるので、フォーマットのみ定義)
		CString		strSqlCntWk;
		if(1 == nKeyCnt) {
			strSqlCntWk  = "DELETE FROM %s WHERE ";
			strSqlCntWk += "%s >= (SELECT TOP 1 A.%s FROM ";
			strSqlCntWk +=				"(select TOP %d * from %s order by %s desc) AS A ORDER BY %s asc)";
			
			sql.Format( strSqlCntWk,
						tbl, 
						key1, key1,
						nExecCnt, tbl, key1, key1);
		} else if(2 == nKeyCnt){
			// 主キーが２こ有るやつは、第一項目の該当より前全部と該当項目の第2のキー全部を消す
			strSqlCntWk  = "DELETE FROM %s WHERE ";
			strSqlCntWk += "%s > (SELECT TOP 1 A.%s FROM ";
			strSqlCntWk +=				"(select TOP %d * from %s order by %s desc, %s desc) AS A ORDER BY A.%s asc, A.%s asc) OR ";
			
			strSqlCntWk += "(%s = (SELECT TOP 1 B.%s FROM ";
			strSqlCntWk +=				"(select TOP %d * from %s order by %s desc, %s desc) AS B ORDER BY B.%s asc, B.%s asc) AND ";
			
			strSqlCntWk += "%s >= (SELECT TOP 1 C.%s FROM ";
			strSqlCntWk +=				"(select TOP %d * from %s order by %s desc, %s desc) AS C ORDER BY C.%s asc, C.%s asc) )";
			
			sql.Format( strSqlCntWk,
						tbl, 
						key1, key1,
						nExecCnt, tbl, key1, key2, key1, key2,
						key1, key1,
						nExecCnt, tbl, key1, key2, key1, key2,
						key2, key2,
						nExecCnt, tbl, key1, key2, key1, key2 );
		}

		//// 削除
		if( !cls_pDbFrom->ExecuteDirect(sql, false)) {	// トランザクション有効中なので、異常時の自動切断はしない
			LOG(em_ERR), "[Move_DB] 削除 [%s:%s]", my_cSession, sql);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s:%s]", my_cSession, tbl);

			bAns = false;
			goto Ending;
		}

		//// コミット
		cls_pDbFrom->TranCommit();
		cls_pDbTo->TranCommit();
		//LOG(em_INF), "[Move_DB] 実績移動中・・・ [%s:%s]", my_cSession, tbl);

		//// 待ち
		Sleep(1000);
		if(my_bStopFlg) goto Ending; // 終了？
	}


	//// 終了処理 (必須)
Ending:
	if(bAns) {
		// コミット
		cls_pDbFrom->TranCommit();
		cls_pDbTo->TranCommit();
		LOG(em_INF), "[Move_DB] 実績移動完了 [%s:%s]", my_cSession, tbl);
	} else {
		// ロールバック
		cls_pDbFrom->TranRollBack();
		cls_pDbTo->TranRollBack();
	}
	return bAns;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 実績同期関係

//------------------------------------------
// 実績完全同期
// bool bAtoB    true:A→B  false:B→A
// char const* tbl		テーブル名称
// int nExecCnt １回の処理件数
// int nKeyCnt 有効主キー数
// char const* key 主キー配列
//------------------------------------------
bool KizuSyncroManager::Syncopy_DB(bool bAtoB, char const* tbl, int nExecCnt, int nKeyCnt, char const* key)
{
	bool		bAns = true;
	CString		sqlCnt;
	CString		sqlExec;
	if(NULL == mcls_pDbA) return false; 
	if(NULL == mcls_pDbB) return false; 

	char const* key1 = &key[PARAM_MAX_STRING*0];
	char const* key2 = &key[PARAM_MAX_STRING*1];
	char const* key3 = &key[PARAM_MAX_STRING*2];

	//// DB接続チェック (通常接続されているはずだが、念の為)
	// A系
	if( ! mcls_pDbA->Connect(TASKINI_NAME, my_cOdbcSession[0] ) ) {
		// データベース接続エラー
		LOG(em_ERR), "[Syncopy_DB] DB接続エラー [%s:%s]", my_cSession, tbl);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}
	// B系
	if( ! mcls_pDbB->Connect(TASKINI_NAME, my_cOdbcSession[1] ) ) {
		// データベース接続エラー
		LOG(em_ERR), "[Syncopy_DB] DB接続エラー [%s:%s]", my_cSession, tbl);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	// -------------------------------------------------------
	// 【主な流れ】
	// ・元、先のキーを比較して、片一方にしかない主キーを指定件数コピー (新しいやつから順番に移動となる)
	// ・待ち
	// ・繰り返し 最初へ
	// 【注意】
	// ・Toが基準となる。
	// -------------------------------------------------------

	//// アクセス文字列生成
	CString		sFromTblAccessPass;		// コピー先DBからリンクサーバー経由で元DBにアクセスする時の 元テーブルパス
	OdbcBase*	cls_pDbFrom;			// 元DB
	OdbcBase*	cls_pDbTo;				// 先DB
	if(bAtoB) {
		sFromTblAccessPass.Format("%s%s", my_cLinkPath[1], tbl);
		cls_pDbFrom = mcls_pDbA;
		cls_pDbTo	= mcls_pDbB;
	} else {
		sFromTblAccessPass.Format("%s%s", my_cLinkPath[0], tbl);
		cls_pDbFrom = mcls_pDbB;
		cls_pDbTo	= mcls_pDbA;
	}


	//// コピーするSQL文 (SQL文が難しくなるので、フォーマットのみ定義)
	CString		strSqlExecWk;
	if(1 == nKeyCnt) {
		// 元と先を外部結合して、元がNULLで先にデータがあるもののみインサート
		strSqlExecWk  = "INSERT INTO %s ";
		strSqlExecWk += "SELECT TOP %d f.* FROM %s f ";
		strSqlExecWk += "LEFT OUTER JOIN %s t ON f.%s=t.%s ";
		strSqlExecWk += "WHERE t.%s IS NULL ";
		strSqlExecWk += "ORDER BY t.%s asc";
		
		sqlExec.Format(strSqlExecWk,
			tbl, 
			nExecCnt, sFromTblAccessPass,
			tbl, key1, key1,
			key1,
			key1);

	} else if(2 == nKeyCnt){
		strSqlExecWk  = "INSERT INTO %s ";
		strSqlExecWk += "SELECT TOP %d f.* FROM %s f ";
		strSqlExecWk += "LEFT OUTER JOIN %s t ON f.%s=t.%s AND f.%s=t.%s ";
		strSqlExecWk += "WHERE t.%s IS NULL AND t.%s IS NULL ";
		strSqlExecWk += "ORDER BY t.%s asc, t.%s asc";
		
		sqlExec.Format(strSqlExecWk,
			tbl, 
			nExecCnt, sFromTblAccessPass,
			tbl, key1, key1, key2, key2,
			key1, key2,
			key1, key2);

	} else if(3 == nKeyCnt){
		strSqlExecWk  = "INSERT INTO %s ";
		strSqlExecWk += "SELECT TOP %d f.* FROM %s f ";
		strSqlExecWk += "LEFT OUTER JOIN %s t ON f.%s=t.%s AND f.%s=t.%s AND f.%s=t.%s ";
		strSqlExecWk += "WHERE t.%s IS NULL AND t.%s IS NULL AND t.%s IS NULL ";
		strSqlExecWk += "ORDER BY t.%s asc, t.%s asc, t.%s asc";
		
		sqlExec.Format(strSqlExecWk,
			tbl, 
			nExecCnt, sFromTblAccessPass,
			tbl, key1, key1, key2, key2, key3, key3,
			key1, key2, key3,
			key1, key2, key3);

	} else {
		_ASSERT(false);	
	}

	//// 最新をチェックするSQL文 (SQL文が難しくなるので、フォーマットのみ定義)
	CString		strSqlCntWk;

	if(1 == nKeyCnt) {
		//strSqlCntWk  = "SELECT COUNT(*) FROM %s f ";
		//strSqlCntWk += "LEFT OUTER JOIN %s t ON f.%s=t.%s ";
		//strSqlCntWk += "WHERE t.%s IS NULL ";
		// これだと全レコード検索になりものすごく時間が掛かる為、対策

		strSqlCntWk  = "SELECT COUNT(*) FROM (";

		strSqlCntWk += "SELECT TOP 1 f.* FROM %s f ";
		strSqlCntWk += "LEFT OUTER JOIN %s t ON f.%s=t.%s ";
		strSqlCntWk += "WHERE t.%s IS NULL ";
		strSqlCntWk += "ORDER BY t.%s asc";
		
		strSqlCntWk += ") as bb";

		sqlCnt.Format(strSqlCntWk,
			sFromTblAccessPass,
			tbl, key1, key1,
			key1,
			key1);

	} else if(2 == nKeyCnt){

		strSqlCntWk  = "SELECT COUNT(*) FROM (";

		strSqlCntWk += "SELECT TOP 1 f.* FROM %s f ";
		strSqlCntWk += "LEFT OUTER JOIN %s t ON f.%s=t.%s AND f.%s=t.%s ";
		strSqlCntWk += "WHERE t.%s IS NULL AND t.%s IS NULL ";
		strSqlCntWk += "ORDER BY t.%s asc, t.%s asc";

		strSqlCntWk += ") as bb";

		sqlCnt.Format(strSqlCntWk,
			sFromTblAccessPass,
			tbl, key1, key1, key2, key2,
			key1, key2,
			key1, key2);

	} else if(3 == nKeyCnt){
		strSqlCntWk  = "SELECT COUNT(*) FROM (";

		strSqlCntWk += "SELECT TOP 1 f.* FROM %s f ";
		strSqlCntWk += "LEFT OUTER JOIN %s t ON f.%s=t.%s AND f.%s=t.%s AND f.%s=t.%s ";
		strSqlCntWk += "WHERE t.%s IS NULL AND t.%s IS NULL AND t.%s IS NULL ";
		strSqlCntWk += "ORDER BY t.%s asc, t.%s asc, t.%s asc";

		strSqlCntWk += ") as bb";

		sqlCnt.Format(strSqlCntWk,
			sFromTblAccessPass,
			tbl, key1, key1, key2, key2, key3, key3,
			key1, key2, key3,
			key1, key2, key3);
	}


	////// 実績完全同期 (ちょっとづつコピー)
	int nCntwk = 1;
	while(true) {
		// コピー開始
		if( !cls_pDbTo->ExecuteDirect(sqlExec)) {	
			LOG(em_ERR), "[Syncopy_DB] 実績同期 [%s:%s]", my_cSession, sqlExec);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s:%s]", my_cSession, tbl);

			bAns = false;
			goto Ending;
		}

		//// もうNULLが無いかチェック
		int cntWk;
		cls_pDbTo->GetSelectKey(sqlCnt, &cntWk);
		if(0 == cntWk)  { goto Ending;}
		else			{ LOG(em_INF), "[Syncopy_DB] 実績同期 ･･･ %d回目", nCntwk);}
		nCntwk ++;

		//// 待ち
		Sleep(100);
		if(my_bStopFlg) goto Ending; // 終了？
	}


	//// 終了処理 (必須)
Ending:
	if(bAns) {
		LOG(em_INF), "[Syncopy_DB] 実績同期完了 [%s:%s]", my_cSession, tbl);
	} else {
		LOG(em_ERR), "[Syncopy_DB] 実績同期失敗 [%s:%s]", my_cSession, tbl);
	}
	
	return bAns;
}