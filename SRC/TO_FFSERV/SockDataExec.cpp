#include "StdAfx.h"
#include "SockDataExec.h"

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
SockDataExec::SockDataExec(void):
FuncBase("SockDataExec"),
m_bDumpMode(false),
m_bReadDataMode(true)
{
}
//------------------------------------------
// デストラクタ
//------------------------------------------
SockDataExec::~SockDataExec(void)
{
}

//------------------------------------------
// 送信データ取得
// Delivery* deli 送信要求
// ExecItem* item 送受信データ
//------------------------------------------
bool SockDataExec::GetSendData(Delivery* deli, ExecItem* item)
{
	CString	sql;
	CString	cWk;
	SYSTEMTIME STime;
	GetLocalTime(&STime);

	//// セットエリア初期化
	memset(item, 0x00, sizeof(ExecItem));

	//// 各伝文生成
	switch(deli->eStat) {

//-----------------------------------------------------------------------------------------------
	case QUE_SEND_ZISEKI:								// 疵実績伝文
		{
			if( ( 0 == strlen( deli->cOmtFileName ) )
				|| ( GetReadDataMode() ) ) {
				// 通常モードの場合 と
				// テストモードの DB指定の場合


				CString cDbName;
				cDbName = DB_BUF_FF_S_ZISEKI;

				//// SQL文作成
				sql =  "SELECT";
				sql += " TOP 1 ID";
				sql += ", キー情報, 生伝文1, 生伝文2, 生伝文3, 生伝文4";
				sql += " FROM ";
				sql += cDbName;
				sql += " WHERE ";
				sql += " 伝文区分 IN (";
				CreateSQL::sqlColumnInsert(&sql, FF_SENDKIND_NON);			// 伝文区分(未送信)
				CreateSQL::sqlColumnInsert(&sql, FF_SENDKIND_ERR);			// 伝文区分(送信失敗（再送信あり）)
				CreateSQL::sqlColumnTrim(&sql);								// 最後のカンマを取り除く
				sql += ") ";

				cWk.Format(" AND 登録日付+1>'%d/%d/%d %d:%d:%d' ",STime.wYear, STime.wMonth, STime.wDay, STime.wHour, STime.wMinute, STime.wSecond);
				sql += cWk;
				sql += " ORDER BY 登録日付 ";


				//// ODBC接続クラス インスタンス生成
				OdbcBase clsDB;
				if( ! clsDB.Connect(TASKINI_NAME)) {
					// データベース接続エラー
					LOG(em_ERR), "[%s] DB接続エラー[%s]", my_sThreadName, cDbName);
					syslog(SYSNO_DB_CONNECT_ERR, "[%s]", cDbName);
					return false;
				}


				//// 実行
				if( ! clsDB.ExecuteDirect(sql)) {
					LOG(em_ERR), "[%s] DB読取SQL文実行エラー[%s]", my_sThreadName, cDbName);
					syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
					return false;
				}

				//// 取得
				int sqlrc = clsDB.Fetch();
				if(sqlrc == KIZUODBC_FETCH_OK) {					// データ有り
					
					//// その他固定 データセット
					// 伝文種類
					item->emStat = deli->eStat;
					// 何個セットしているか
					item->nSendMaxNo = 2;
					// 送信伝文サイズ (検査実績伝文サイズをセット)
					item->nSendSize[0] = sizeof(FF_JISSEKI);
					item->nSendSize[1] = sizeof(FF_JISSEKI);
					item->nSendSize[2] = sizeof(FF_STS_JISSEKI);
					item->nSendSize[3] = sizeof(FF_STS_JISSEKI);
					// 何個受信するべきか
					item->nRecvMaxNo = 4;
					//受信伝文サイズ (検査実績 応答伝文サイズをセット)
					item->nRecvSize[0] = sizeof(FF_ANS);
					item->nRecvSize[1] = sizeof(FF_ANS);
					item->nRecvSize[2] = sizeof(FF_STSANS);
					item->nRecvSize[3] = sizeof(FF_STSANS);


					//// DB検索結果より データセット
					// DBのID
					clsDB.GetData(1, item->cKeyId, SIZE_SERIAL_FF_NO);
					// キー情報 命令No
					//clsDB.GetData(2, item->cMeiNo, SIZE_MEI_NO);
					PRO_TYPE_KEY	typKey;
					clsDB.GetData(2, (char*)&typKey, sizeof(typKey));
					memcpy(item->cMeiNo, typKey.cMeiNo, sizeof(typKey.cMeiNo));
					
					// 送信データ0 ← 生伝文1
					clsDB.GetDataImage(3, item->cSendBuf[0], item->nSendSize[0]);
					// 送信データ1 ← 生伝文2
					clsDB.GetDataImage(4, item->cSendBuf[1], item->nSendSize[1]);
					// 送信データ0 ← 生伝文3
					clsDB.GetDataImage(5, item->cSendBuf[2], item->nSendSize[2]);
					// 送信データ1 ← 生伝文4
					clsDB.GetDataImage(6, item->cSendBuf[3], item->nSendSize[3]);

					LOG(em_MSG), "[%s] 疵実績伝文 取得完了 [ID=%s]", my_sThreadName, item->cKeyId);
					break;

				} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
					LOG(em_INF), "[%s] 疵実績情報 未送信データ無し", my_sThreadName);
					return false;
				} else {								// エラー
					LOG(em_ERR), "[%s] DB読取結果取得エラー[%s]", my_sThreadName, cDbName);
					syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
					return false;
				}
			} else {
				// テストモードの ファイル指定の場合
				
				LOG(em_INF), "[%s] デバッグ 実績伝文送信要求=====", my_sThreadName);
				LOG(em_INF), "[%s]  検査実績伝文(表) [%s]", my_sThreadName, deli->cOmtFileName);
				LOG(em_INF), "[%s]  検査実績伝文(裏) [%s]", my_sThreadName, deli->cUraFileName);
				LOG(em_INF), "[%s]  検査状態伝文(表) [%s]", my_sThreadName, deli->cStOmtFileName);
				LOG(em_INF), "[%s]  検査状態伝文(裏) [%s]", my_sThreadName, deli->cStUraFileName);

				//// その他固定 データセット
				// 伝文種類
				item->emStat = deli->eStat;
				// 何個セットしているか
				item->nSendMaxNo = 2;
				// 送信伝文サイズ (検査実績伝文サイズをセット)
				item->nSendSize[0] = sizeof(FF_JISSEKI);
				item->nSendSize[1] = sizeof(FF_JISSEKI);
				item->nSendSize[2] = sizeof(FF_STS_JISSEKI);
				item->nSendSize[3] = sizeof(FF_STS_JISSEKI);
				// 何個受信するべきか
				item->nRecvMaxNo = 4;
				//受信伝文サイズ (検査実績 応答伝文サイズをセット)
				item->nRecvSize[0] = sizeof(FF_ANS);
				item->nRecvSize[1] = sizeof(FF_ANS);
				item->nRecvSize[2] = sizeof(FF_STSANS);
				item->nRecvSize[3] = sizeof(FF_STSANS);

				sprintf( item->cKeyId, "%d%d%d %d%d%d", STime.wYear, STime.wMonth, STime.wDay, STime.wHour, STime.wMinute, STime.wSecond );

				// 疵実績情報テスト送信伝文（表）ファイルセット
				if( ! ReadTestFile( item->cSendBuf[0], item->nSendSize[0], deli->cOmtFileName ) )
				{
					item->emStat = (QUE_SEND_STAT)0;
					return false;
				}
				// 疵実績情報テスト送信伝文（裏）ファイルセット
				if( ! ReadTestFile( item->cSendBuf[1], item->nSendSize[1], deli->cUraFileName ) )
				{
					item->emStat = (QUE_SEND_STAT)0;
					return false;
				}
				// 疵状態テスト送信伝文（表）ファイルセット
				if (!ReadTestFile(item->cSendBuf[2], item->nSendSize[2], deli->cStOmtFileName))
				{
					item->emStat = (QUE_SEND_STAT)0;
					return false;
				}
				// 疵状態テスト送信伝文（裏）ファイルセット
				if (!ReadTestFile(item->cSendBuf[3], item->nSendSize[3], deli->cStUraFileName))
				{
					item->emStat = (QUE_SEND_STAT)0;
					return false;
				}
				FF_JISSEKI* pJsk;
				pJsk = (FF_JISSEKI*)&item->cSendBuf[0];
				memcpy( item->cMeiNo, pJsk->Hd.Meino, sizeof(pJsk->Hd.Meino) );
			}
		}
		break;

//-----------------------------------------------------------------------------------------------
	case QUE_SEND_MAE:									// 前工程実績要求
		{
			// 固定データ
			// 伝文種類
			item->emStat = deli->eStat;
			// 何個セットしているか
			item->nSendMaxNo = 1;
			// 送信伝文サイズ (前工程疵実績要求伝文サイズをセット)
			item->nSendSize[0] = sizeof(FF_MAE_REQUEST);
			// 何個受信するべきか
			item->nRecvMaxNo = 2;
			// 受信伝文サイズ (前工程疵情報伝文サイズをセット)
			item->nRecvSize[0] = sizeof(FF_MAE_JISSEKI);				
			item->nRecvSize[1] = sizeof(FF_MAE_JISSEKI);


			//// 起動引数より データセット
			// 命令No
			cWk.Format("%04s", deli->cMeiNo);
			memcpy(item->cMeiNo, cWk, SIZE_MEI_NO);
			// コイルNo
			memcpy(item->cCoilNo, deli->cCoilNo, sizeof(deli->cCoilNo));
			// コイル特定するためのキー情報
			memcpy(&item->keyinf, &deli->keyinf, sizeof(deli->keyinf));

			// 伝文生成
			FF_MAE_REQUEST& pBuf = (FF_MAE_REQUEST&)item->cSendBuf[0];
			memset(&pBuf, 0x20, sizeof(FF_MAE_REQUEST));


			// 伝文コード
			memcpy( pBuf.HeaderCode, FF_TC_CODE_MAEREQ, sizeof(pBuf.HeaderCode));
			// 連番
			memcpy( pBuf.Rec_Code, "00", sizeof(pBuf.Rec_Code));
			// 伝文発生時刻
			cWk.Format("%04d%02d%02d%02d%02d%02d", STime.wYear, STime.wMonth, STime.wDay, STime.wHour, STime.wMinute, STime.wSecond); 
			memcpy( pBuf.DateTime, cWk, sizeof(pBuf.DateTime));
			// 工程コード
			memcpy( pBuf.Line_Code, deli->cLine_Code, SIZE_KOUTEI_CODE);
			// 命令番号
			memcpy( pBuf.MeireiNo, item->cMeiNo, SIZE_MEI_NO);
			
			LOG(em_MSG), "[%s] 前工程実績要求伝文 生成完了 [コイルNo=%s][命令No=%s]", my_sThreadName, item->cCoilNo, item->cMeiNo);
		}
		break;

//-----------------------------------------------------------------------------------------------
	case QUE_SEND_HELCY:								// ヘルシー伝文送信要求
		{
			// 固定データ
			// 伝文種類
			item->emStat = deli->eStat;
			// 何個セットしているか
			item->nSendMaxNo = 1;
			// 送信伝文サイズ (ヘルシー伝文サイズをセット)
			item->nSendSize[0] = sizeof(FF_HELCY);			
			item->nRecvMaxNo = 1;
			// 受信伝文サイズ (ヘルシー応答伝文サイズをセット)
			item->nRecvSize[0] = sizeof(FF_HELCY);	
			

			// 伝文生成
			FF_HELCY& pBuf = (FF_HELCY&)item->cSendBuf[0];
			memset(&pBuf, 0x20, sizeof(FF_HELCY));


			// 伝文コード
			memcpy( pBuf.Header_Code, FF_TC_CODE_HELCY, sizeof(pBuf.Header_Code));
			// 連番
			memcpy( pBuf.Rec_Code, "00", sizeof(pBuf.Rec_Code));
			// 伝文発生時刻
			cWk.Format("%04d%02d%02d%02d%02d%02d", STime.wYear, STime.wMonth, STime.wDay, STime.wHour, STime.wMinute, STime.wSecond); 
			memcpy( pBuf.DateTime, cWk, sizeof(pBuf.DateTime));
			// 工程コード
			memcpy(pBuf.Line_Code, KOUTEI_CODE, sizeof(pBuf.Line_Code));
			// 伝管コマンドNo
			memcpy(pBuf.Denkan_No, FF_DENKAN_NO, sizeof(pBuf.Denkan_No));
		}
		break;
	}
	return true;
}

//------------------------------------------
// 実績伝文送信結果
// char const* cKey 主キー
// FF_SENDKIND_NO emKind 伝文区分の設定値
//------------------------------------------
bool SockDataExec::SetZisekiSendAnser(char const* cKey, FF_SENDKIND_NO emKind)
{
	//// チェック
	if(0 == strlen(cKey)) {
		LOG(em_ERR), "[%s] 実績伝文送信結果反映異常。主キー無し [%s]", my_sThreadName, cKey);
	}

	//// SQL生成
	CString	sql;
	CString cDbName;
	cDbName = DB_BUF_FF_S_ZISEKI;

	sql = "UPDATE ";
	sql += cDbName;
	sql += " SET ";
	CreateSQL::sqlColumnUpdate(&sql, "伝文区分", emKind); 
	sql += "送信日付=";	CreateSQL::sqlColumnAddTime(&sql);
	sql += " WHERE ID=";
	CreateSQL::sqlColumnInsert(&sql, cKey, SIZE_SERIAL_FF_NO);		// ID
	CreateSQL::sqlColumnTrim(&sql);									// 最後のカンマを取り除く

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー[%s]", my_sThreadName, cDbName);
		syslog(SYSNO_DB_CONNECT_ERR, "[%s]", cDbName);
		return false;
	}


	//// 実行
	if( ! clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] 実績伝文送信結果 UPDATEエラー [%s]", my_sThreadName, sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}
	return true;
}
//------------------------------------------
// 前工程実績バッファ登録
// ExecItem const* item 送受信データ
//------------------------------------------
bool SockDataExec::InsertMaeBuf(ExecItem const* item)
{
	char cId[SIZE_SERIAL_FF_NO];
	KizuFunction::GetSerialNoFF(cId);		// ID作成

	//// SQL生成
	CString	sql;
	CString cDbName;
	cDbName = DB_BUF_FF_R_MAEINF;

	//======================================================	
	//// FF前工程疵情報受信バッファテーブル
	// ヘッダー部
	sql = "INSERT INTO ";
	sql += cDbName;
	sql += " VALUES(";	


	// データ部
	CreateSQL::sqlColumnInsert(&sql, cId, SIZE_SERIAL_FF_NO);						// ID
	CreateSQL::sqlColumnAddTime(&sql, true);										// 登録日付
	CreateSQL::sqlColumnInsert(&sql, (char*)&item->keyinf, sizeof(item->keyinf));	// キー情報
	sql += "?, ?";																	// 生伝文
	sql += ")";

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー[%s]", my_sThreadName, cDbName);
		syslog(SYSNO_DB_CONNECT_ERR, "[%s]", cDbName);
		return false;
	}
	if( ! clsDB.SqlBinalyWrite(sql, item->nRecvSize[0], item->nRecvSize[1], (LPVOID)item->cRecvBuf[0], (LPVOID)item->cRecvBuf[1] )) {
		LOG(em_ERR), "[%s] 前工程伝文結果 INSERTエラー [%s]", my_sThreadName, sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}


	//// 前工程が登録できた為、TO_MAEDATに通知
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));

	que.nEventNo = FACT_TO_MAEDAT_03;
	que.nLineNo = getlineid();
	memcpy(que.fc.data , cId, SIZE_SERIAL_FF_NO);
	send_mail( TO_MAEDAT, ".", &que);

	LOG(em_MSG), "[%s] 前工程実績登録完了 [ID=%s]", my_sThreadName, cId);

	return true;
}

//------------------------------------------
// テストデータ読み込み
// void* cSendData		伝文バッファ領域
// int iSendSize		伝文バッファサイズ
// char const* fullname	ファイル名称
//------------------------------------------
bool SockDataExec::ReadTestFile( BYTE* cSendData, int nSendSize, char const* cfullname )
{
	HANDLE		fp;
	DWORD		nNumByte = 0;
	bool 		bRet = false;
	char		cfullpath[MAX_PATH];

	sprintf( cfullpath, ".\\%s", cfullname );						// ファイルPATH生成

	LOG(em_MSG), "[%s] %s読込み開始", my_sThreadName, cfullname );	// 開始ログ出力

	fp = CreateFile( cfullpath,										// ファイルPATH
					 GENERIC_READ,									// アクセスモード（読込み）
					 FILE_SHARE_READ,								// 共有モード（読込みOK）
					 NULL,											// セキュリティ記述子（指定なし）
					 OPEN_EXISTING,									// 作成方法
					 FILE_ATTRIBUTE_NORMAL,							// ファイル属性（普通）
					 NULL );										// テンプレートファイルのハンドル（指定なし）

	if( fp != INVALID_HANDLE_VALUE ) {
		if( ReadFile( fp, cSendData, nSendSize, &nNumByte, NULL) ) {									// 読込み実行
			LOG(em_MSG), "[%s] %s読込み完了", my_sThreadName, cfullname );								// 読込み完了ログ出力
			bRet = true;
		} else {
			LOG(em_ERR), "[%s] %s読込み失敗 (retc=%d)", my_sThreadName, cfullname, GetLastError() );	// 読込み失敗ログ出力
			bRet = false;
		}
		CloseHandle( fp );
	} else {
		LOG(em_ERR), "[%s] %sオープン失敗 (retc=%d)", my_sThreadName, cfullname, GetLastError() );		// ファイルオープン失敗ログ出力
		bRet = false;
	}
	return bRet;
}

