#include "StdAfx.h"
#include "KizuPcInfManager.h"

using namespace KizuLib;

//------------------------------------------
// コンストラクタ
// int nPCID PCID (0:DB接続無し(PC名は.)、 -1:DB接続無し(PC名はコンピュータ名を取得)、 -9:DB接続あり(PC名はコンピュータ名を取得))
//------------------------------------------
KizuPcInfManager::KizuPcInfManager(int nPCID)
{
	// 初期化
	m_nPcId = nPCID;
	memset(m_cMyPcName, 0x00, sizeof(m_cMyPcName));
	if( 0 == nPCID ) m_cMyPcName[0] = '.';
	else			 m_cMyPcName[0] = '*';
	
	// コンピュータ名を取得
	if( -1 == nPCID || -9 == nPCID) {
		char  cName[256];
		DWORD len = sizeof(cName);
		if( GetComputerName(cName, &len) ) {
			memset(m_cMyPcName, 0x00, sizeof(m_cMyPcName));
			strcpy(m_cMyPcName, cName);
		}
	}

	// その他初期化
	m_nPcMode = 1;
	m_nPcKind = 0;
	memset(m_cPcKindName, 0x00, sizeof(m_cPcKindName));
}

//------------------------------------------
// デストラクタ
//------------------------------------------
KizuPcInfManager::~KizuPcInfManager(void)
{

}

//------------------------------------------
// PC名称取得
//------------------------------------------
bool KizuPcInfManager::SetPcName(char *pcname, LPCSTR cIniPath, LPCSTR cSession )
{
	int ii;
	char cWk[MAX_PCCOUNT][SIZE_NAME_PC];

	// ワークに初期値をセット
	memset(cWk, 0x00, sizeof(cWk));
	// ローカルのみ？
	if( 0 == m_nPcId || -1 == m_nPcId ) {
		for( ii=0; ii<MAX_PCCOUNT; ii++) cWk[ii][0] = '.';
		memcpy(pcname, cWk[0], sizeof(cWk));
		return true;
	}

	// DB接続有り
	for( ii=0; ii<MAX_PCCOUNT; ii++) cWk[ii][0] = '*';

	// DB接続チェック
	OdbcBase cls_Db;
	if( ! cls_Db.Connect(cIniPath, cSession ) ) {
		// 接続失敗の為 * をセット
		memcpy(pcname, cWk[0], sizeof(cWk));
		return false;
	}

	////// DBよりPC種別を取得する
	//// SQL文 生成
	int kind;
	int id;
	char sql[256];
	memset(sql, 0x00, sizeof(sql));
	sprintf(sql, "SELECT i.PCKIND, m.PC名, m.PCID, i.PC区分, i.PC種別 FROM %s m INNER JOIN %s i ON m.PCID = i.PCID ORDER BY i.PCKIND ASC",
								 DB_PC_MASTER, DB_PC_INF );


	//// 実行
	if( !cls_Db.ExecuteDirect(sql)) {
		//syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", DB_PC_INF);
		return false;
	}

	//// 取得
	while (TRUE) {
		int sqlrc = cls_Db.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り
			cls_Db.GetData(1, &kind );
			if(1<kind && MAX_PCCOUNT<kind) {
				// kind ng 	
				continue;
			}

			cls_Db.GetData(2, cWk[kind-1], SIZE_NAME_PC );
			cls_Db.GetData(3, &id );

			// 自PCIDと同じPCID？
			if(m_nPcId == id) {
				// 自PCの役割見つけた
				m_nPcKind = kind;
				memcpy(m_cMyPcName, cWk[kind-1], SIZE_NAME_PC);
				cls_Db.GetData(4, &m_nPcMode );
				cls_Db.GetData(5, m_cPcKindName, sizeof(m_cPcKindName) );
			}

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			//syslog(SYSNO_DB_FETCH_ERR, "[%s]", DB_PC_INF);
			return false;
		}
	}
	// 一括セット
	memcpy(pcname, cWk[0], sizeof(cWk));
	return true;
}

//------------------------------------------
// 指定グループのPCIDを取得
// (通常判定PCの候補となるPCIDを全部とってくる等の使い方をする)
// LPCSTR cIniPath iniファイルパス
// int groupno 取得するGroupNo
// int maxcnt 取得する最大件数
// int* count 取得した件数
// int* nPcid 取得したPCIDの配列 (昇順)
// bool bTablePcinf 読込先テーブルの切替(デフォルトfalse) false:PC_INF=手動設定でPCIDを変更した場合の状態が取得できる。true:PC_MASTER=正規のPCIDが取得できる。
// 戻り値 復帰情報
//------------------------------------------
bool KizuPcInfManager::GetGroupPcID(LPCSTR cIniPath, int groupno, int maxcnt, int* count, int* nPcid, bool bTablePcinf)
{
	*count = 0;

	// DB接続チェック
	OdbcBase cls_Db;
	if( ! cls_Db.Connect(cIniPath) ) {
		return false;
	}

	char sql[256];
	memset(sql, 0x00, sizeof(sql));
	/*
	// 20081125 巻き戻しで対応
	//sprintf(sql, "SELECT PCID FROM %s WHERE GroupNo = %d ORDER BY PCID ASC", DB_PC_MASTER, groupno );
	sprintf(sql, "SELECT PCID FROM %s WHERE GroupNo = %d ORDER BY PCID ASC", DB_PC_INF, groupno );
	*/
	// 20090225 両方のテーブルを読むケースがあるので、引数を追加
	if(bTablePcinf)	sprintf(sql, "SELECT PCID FROM %s WHERE GroupNo = %d ORDER BY PCID ASC", DB_PC_MASTER, groupno );
	else			sprintf(sql, "SELECT PCID FROM %s WHERE GroupNo = %d ORDER BY PCID ASC", DB_PC_INF, groupno );

	//// 実行
	if( !cls_Db.ExecuteDirect(sql)) {
		return false;
	}

	//// 取得
	while (TRUE) {
		int sqlrc = cls_Db.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り

			cls_Db.GetData(1, &nPcid[*count] );
			*count += 1;
			if( maxcnt<= *count) break;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			return false;
		}
	}
	return true;
}
//------------------------------------------
// 指定グループのPCIDをセットする  (PC_INF)
// (通常判定PCのバックアップ切り替え等でPCIDを振り直す)
// LPCSTR cIniPath iniファイルパス
// int groupno セットするGroupNo
// int count セットする件数
// int const* nPcid セットしたいPCIDの配列 (昇順)
// 戻り値 復帰情報
//------------------------------------------
bool KizuPcInfManager::SetGroupPcID(LPCSTR cIniPath, int groupno, int count, int const* nPcid )
{
	int cnt = 0;
	int wk;

	// DB接続チェック
	OdbcBase cls_Db_Select;
	if( ! cls_Db_Select.Connect(cIniPath) ) {
		return false;
	}
	// DB接続チェック
	OdbcBase cls_Db_UpDate;
	if( ! cls_Db_UpDate.Connect(cIniPath) ) {
		return false;
	}

	char sqlSelect[256];
	char sqlUpDate[256];
	memset(sqlSelect, 0x00, sizeof(sqlSelect));
	sprintf(sqlSelect, "SELECT PCKIND FROM %s WHERE GroupNo = %d ORDER BY PCKIND ASC", DB_PC_INF, groupno );

	//// 実行
	if( !cls_Db_Select.ExecuteDirect(sqlSelect)) {
		return false;
	}

	//// 取得 および セット
	while (TRUE) {
		int sqlrc = cls_Db_Select.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り

			//// 取得した順に PCIDを変更していく
			// 取得
			cls_Db_Select.GetData(1, &wk );
			
			// 変更
			memset(sqlUpDate, 0x00, sizeof(sqlUpDate));
			sprintf(sqlUpDate, "UPDATE %s SET PCID=%d WHERE PCKIND=%d", DB_PC_INF, nPcid[cnt], wk );
			cnt ++;
			if( !cls_Db_UpDate.ExecuteDirect(sqlUpDate)) {
				return false;
			}
			if( count <= cnt) break;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			return false;
		}
	}

	cls_Db_UpDate.TranCommit();
	return true;
}
