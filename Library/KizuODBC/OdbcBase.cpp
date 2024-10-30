#include "StdAfx.h"
#include "OdbcBase.h"

using namespace KizuODBC;

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// コンストラクタ
//------------------------------------------
OdbcBase::OdbcBase(void) :
mycls_pDB(NULL)
{
	// 生成 (接続ハンドルまで生成)
	mycls_pDB = new OdbcManager();
}

//------------------------------------------
// デストラクタ
//------------------------------------------
OdbcBase::~OdbcBase(void)
{
	// 念の為に切断
	DisConnect();

	// 解放
	if(mycls_pDB) {
		delete mycls_pDB;
		mycls_pDB = NULL;
	}
}

// //////////////////////////////////////////////////////////////////////////////
// OdbcManager の皮被せ関連
// //////////////////////////////////////////////////////////////////////////////

//------------------------------------------
// 接続チェック
// LPCSTR cIniPath iniファイルパス
// LPCSTR cSession セッション
// 戻り値 復帰情報
//------------------------------------------
bool OdbcBase::OpenCheck(LPCSTR cIniPath, LPCSTR cSession)
{
	int retc;
	KizuODBC::OdbcManager * cls_pDB = new OdbcManager();
	if(NULL == cSession) retc = cls_pDB->Connect(cIniPath);
	else				 retc = cls_pDB->Connect(cIniPath, cSession);
	if( 0 == retc ) {
		cls_pDB->DisConnect();
	}
	delete cls_pDB;
	return 0==retc ? true : false;
}

//------------------------------------------
// 任意指定で接続チェック
// char const* dsn  データベース名
// char const* uid  ユーザー名
// char const* pwd  パスワード
// 戻り値 復帰情報
//------------------------------------------
bool OdbcBase::OpenCheck(char const* dsn, char const* uid, char const* pwd)
{
	int retc;
	KizuODBC::OdbcManager * cls_pDB = new OdbcManager();
	retc = cls_pDB->Connect(dsn, uid, pwd);
	if( 0 == retc ) {
		cls_pDB->DisConnect();
	}
	delete cls_pDB;
	return 0==retc ? true : false;
}

//------------------------------------------
// ODBCを未定義で接続
// char const* addr データベースアドレス
// char const* dsn  データベース名
// char const* uid  ユーザー名
// char const* pwd  パスワード
// bool bNative SQL Native Client で接続時 true。通常はfalse
// 戻り値 復帰情報
//------------------------------------------
bool OdbcBase::OpenCheck(char const* addr, char const* dsn, char const* uid, char const* pwd, bool bNative)
{
	int retc;
	KizuODBC::OdbcManager * cls_pDB = new OdbcManager();
	retc = cls_pDB->ConnectDriver(addr, dsn, uid, pwd, bNative);
	if( 0 == retc ) {
		cls_pDB->DisConnect();
	}
	delete cls_pDB;
	return 0==retc ? true : false;
}

//------------------------------------------
// セッション指定での接続
// LPCSTR cIniPath iniファイルパス
// LPCSTR cSession セッション
// 戻り値 復帰情報
//------------------------------------------
bool OdbcBase::Connect(LPCSTR cIniPath, LPCSTR cSession)
{
	if( NULL==mycls_pDB )				return false;
	if( ! mycls_pDB->IsCreateHandle() )	return false;		// ハンドル生成NG
	if( mycls_pDB->IsConnect() )		return true;		// 接続済み

	int retc;
	if(NULL == cSession) retc = mycls_pDB->Connect(cIniPath);
	else				 retc = mycls_pDB->Connect(cIniPath, cSession);
	if( 0 == retc ) {
		return true;
	} else if( 1 == retc) {
		return true;
	} else {
		mycls_pDB->GetErrMsg();
		DisConnect();					// 切断
		return false;
	}
}

//------------------------------------------
// 任意指定での接続
// char const* dsn  データベース名
// char const* uid  ユーザー名
// char const* pwd  パスワード
// 戻り値 復帰情報
//------------------------------------------
bool OdbcBase::Connect(char const* dsn, char const* uid, char const* pwd)
{
	if( NULL==mycls_pDB )				return false;
	if( ! mycls_pDB->IsCreateHandle() )	return false;		// ハンドル生成NG
	if( mycls_pDB->IsConnect() )		return true;		// 接続済み

	int retc;
	retc = mycls_pDB->Connect(dsn, uid, pwd);
	if( 0 == retc ) {
		return true;
	} else if( 1 == retc) {
		return true;
	} else {
		mycls_pDB->GetErrMsg();
		DisConnect();					// 切断
		return false;
	}
}

//------------------------------------------
// ODBCを未定義で接続
// char const* addr データベースアドレス
// char const* dsn  データベース名
// char const* uid  ユーザー名
// char const* pwd  パスワード
// bool bNative SQL Native Client で接続時 true。通常はfalse
// 戻り値 復帰情報
//------------------------------------------
bool OdbcBase::ConnectDriver(char const* addr, char const* dsn, char const* uid, char const* pwd, bool bNative)
{
	if( NULL==mycls_pDB )				return false;
	if( ! mycls_pDB->IsCreateHandle() )	return false;		// ハンドル生成NG
	if( mycls_pDB->IsConnect() )		return true;		// 接続済み

	int retc;
	retc = mycls_pDB->ConnectDriver(addr, dsn, uid, pwd, bNative);
	if( 0 == retc ) {
		return true;
	} else if( 1 == retc) {
		return true;
	} else {
		mycls_pDB->GetErrMsg();
		DisConnect();					// 切断
		return false;
	}
}



//------------------------------------------
// DB切断
//------------------------------------------
void OdbcBase::DisConnect()
{
	// 切断
	if(NULL != mycls_pDB) {
		mycls_pDB->DisConnect(); 
	}
}

//------------------------------------------
// オートコミット ON/OFF
// bool flg トランザクションと使いたい場合は、falseにする事
// 戻り値 復帰情報
//------------------------------------------
bool OdbcBase::AutoCommitEnable(bool flg)
{
	if( ! mycls_pDB->IsConnect()) return false;			// 未接続

	SQLRETURN sqlrc = mycls_pDB->AutoCommitEnable(flg);
	if( sqlrc == SQL_ERROR || sqlrc == SQL_INVALID_HANDLE) {
		mycls_pDB->GetErrMsg();
		DisConnect();					// 切断
		return false;
	} else {
		return true;
	}
}

//------------------------------------------
// SQL文実行
// LPCSTR cSql SQL文
// bool bErrAutoClose エラー時自動切断機能 (デフォルト true)
//						※ トランザクションを有効としている場合などで、勝手に切断されたら困るよ対応
// 戻り値 復帰情報
//------------------------------------------
bool OdbcBase::ExecuteDirect(LPCSTR cSql, bool bErrAutoClose)
{
	if( ! mycls_pDB->IsConnect()) return false;			// 未接続

	SQLRETURN sqlrc = mycls_pDB->ExecuteDirect(cSql);
	if( sqlrc == SQL_ERROR || sqlrc == SQL_INVALID_HANDLE) {
		mycls_pDB->GetErrMsg();
		if(bErrAutoClose) DisConnect();			// 切断
		return false;
	} else {
		return true;
	}
}

//------------------------------------------
// コミット
// 戻り値 復帰情報
//------------------------------------------
bool OdbcBase::TranCommit()
{
	if( ! mycls_pDB->IsConnect()) return false;			// 未接続

	mycls_pDB->TranCommit();
	return true;
}

//------------------------------------------
// ロールバック
// 戻り値 復帰情報
//------------------------------------------
bool OdbcBase::TranRollBack()
{
	if( ! mycls_pDB->IsConnect()) return false;			// 未接続

	mycls_pDB->TranRollBack();
	return true;
}

//------------------------------------------
// フェッチ
// 戻り値 KIZUODBC_FETCH_xxx
//------------------------------------------
int OdbcBase::Fetch()
{
	if( ! mycls_pDB->IsConnect()) return KIZUODBC_FETCH_ERR;			// 未接続

	SQLRETURN sqlrc = mycls_pDB->Fetch();
	if (sqlrc == SQL_ERROR || sqlrc == SQL_INVALID_HANDLE) {
		mycls_pDB->GetErrMsg();
		DisConnect();					// 切断
		return KIZUODBC_FETCH_ERR;
	} else if(sqlrc == SQL_NO_DATA) {
		return KIZUODBC_FETCH_NODATE;	// データ無し
	} else {
		return KIZUODBC_FETCH_OK;
	}
}

// //////////////////////////////////////////////////////////////////////////////
// バイナリデータ登録関連
// //////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// 複数件バイナリ登録
// LPCSTR cSql ? 付きのSQL文
// int paramCnt パラメータの数
// DWORD size パラメータ数分のバイナリデータサイズの配列 (NULLセット時は、0を指定)
// LPVOID pVal パラメータ数分のバイナリデータの先頭ポインタの配列 (NULLセット時は、NULLを指定)
// bool bErrAutoClose エラー時自動切断機能 (デフォルト true)
// 戻り値 復帰情報
//------------------------------------------
bool OdbcBase::SqlBinalyWrite(LPCSTR cSql, int paramCnt, DWORD *size, LPVOID *pVal, bool bErrAutoClose)
{
	if( ! mycls_pDB->IsConnect()) return false;			// 未接続

	int ii;
	SQLRETURN sqlrc;
	SQLPOINTER pToken;

	//// パラメータ付きSQL文セット
	sqlrc = mycls_pDB->Prepare(cSql);
	if( sqlrc == SQL_ERROR || sqlrc == SQL_INVALID_HANDLE) {
		mycls_pDB->GetErrMsg();
		DisConnect();					// 切断
		return false;
	}

	//// 複数件バインド登録
	// これ以降の return はダメ
	SQLLEN *cbPhotoParam = new SQLLEN[paramCnt];
	memset( cbPhotoParam, 0x00, sizeof(SQLLEN)*paramCnt);
	// セット
	for( ii=0; ii<paramCnt; ii++) {
		//cbPhotoParam[ii] = SQL_LEN_DATA_AT_EXEC(size[ii]);

		// バインド
		if( NULL == pVal[ii] ) {					// NULLデータ
			sqlrc = mycls_pDB->BindParameter(ii+1, 1, &cbPhotoParam[ii]);
		} else {									// 実データ
			sqlrc = mycls_pDB->BindParameter(ii+1, size[ii], &cbPhotoParam[ii]);
		}
		if( sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO ) {
			// 正常
		} else {
			goto NG;
		}

		// パラメータの 1 のデータは実行時に指定する。    
		// 注意として、ここではパラメータの長さに       
		// SQL_LEN_DATA_AT_EXEC マクロで、0 にしている。
		// これは、SQLGetInfo の SQL_NEED_LONG_DATA_LEN 情報が 
		// "N" を返すドライバで許される。
		cbPhotoParam[ii] = (SQLLEN)SQL_LEN_DATA_AT_EXEC(0);
	}

	//// 実行
	sqlrc = mycls_pDB->Execute();
	if( sqlrc == SQL_ERROR || sqlrc == SQL_INVALID_HANDLE) {
		goto NG;
	}

	//// パラメータ登録
	for( ii=0; ii<paramCnt; ii++) {
		// パラメータ番号取得
		if( SQL_NEED_DATA == mycls_pDB->ParamData(&pToken)) {
			// バインドがまだの為OK
		} else {
			goto NG;
		}

		// データセット 
		if( NULL == pVal[ii] ) {					// NULLデータ
			sqlrc = mycls_pDB->PutData(0, 0);
		} else {									// 実データ
			sqlrc = mycls_pDB->PutData(pVal[ii], size[ii]);
		}
		if (sqlrc == SQL_ERROR || sqlrc == SQL_INVALID_HANDLE) {
			goto NG;
		}
	}

	//// パラメータ番号取得
	if( SQL_SUCCESS == mycls_pDB->ParamData(&pToken) ) {
		// 正常
	} else {
		goto NG;
	}

	//// 終了処理
	if (NULL != cbPhotoParam) {
		delete [] cbPhotoParam; 
		cbPhotoParam = NULL;
	}
	return true;
NG:
	mycls_pDB->GetErrMsg();
	if(bErrAutoClose) DisConnect();					// 切断
	if (NULL != cbPhotoParam) {
		delete [] cbPhotoParam; 
		cbPhotoParam = NULL;
	}
	return false;
}

//------------------------------------------
// 1件バイナリ登録 (簡易版)
// LPCSTR cSql SQL文
// DWORD size バイナリデータサイズ
// LPVOID pVal バイナリデータ
// bool bErrAutoClose エラー時自動切断機能 (デフォルト true)
// 戻り値 復帰情報
//------------------------------------------
bool OdbcBase::SqlBinalyWrite(LPCSTR cSql, DWORD size, LPVOID pVal, bool bErrAutoClose)
{
	return SqlBinalyWrite(cSql, 1, &size, &pVal, bErrAutoClose);
}
//------------------------------------------
// 2件バイナリ登録 (簡易版)
// LPCSTR cSql SQL文
// DWORD size1 バイナリデータサイズ
// DWORD size2 バイナリデータサイズ
// LPVOID pVal1 バイナリデータ
// LPVOID pVal2 バイナリデータ
// bool bErrAutoClose エラー時自動切断機能 (デフォルト true)
// 戻り値 復帰情報
//------------------------------------------
bool OdbcBase::SqlBinalyWrite(LPCSTR cSql, DWORD size1, DWORD size2, LPVOID pVal1, LPVOID pVal2, bool bErrAutoClose)
{
	DWORD wksize[2];
	LPVOID wkpVal[2];
	// データセット
	wksize[0] = size1;
	wksize[1] = size2;
	wkpVal[0] = pVal1;
	wkpVal[1] = pVal2;
	// 登録
	return SqlBinalyWrite(cSql, 2, wksize, wkpVal, bErrAutoClose);
}

// //////////////////////////////////////////////////////////////////////////////
// バイナリファイルデータ登録関連
// //////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// 複数件バイナリファイル登録 (MAX10列)
// LPCSTR cSql ? 付きのSQL文
// int paramCnt パラメータの数
// int pathsize バイナリファイルパス の配列サイズ 通常 MAX_PATH
// const char* cFPath バイナリファイルのフルパス
// 戻り値 復帰情報
//------------------------------------------
bool OdbcBase::SqlBinalyFileWrite(LPCSTR cSql, int paramCnt, int pathsize, const char* cFPath, bool bErrAutoClose)
{
	if( ! mycls_pDB->IsConnect()) return false;			// 未接続

	int ii;
	SQLRETURN sqlrc;
	SQLPOINTER pToken;


	//// ファイル関係 のデータ事前準備
	FILE*		fp;											// ファイルポインタ
	fpos_t*		size = new fpos_t[paramCnt];				// ファイルサイズ
	BYTE*		data = new BYTE[BinalyFileOneWriteSize];	// 一回の書き込みバッファエリア
	for( ii=0; ii<paramCnt; ii++) {
		fp = fopen(&cFPath[ii*pathsize], "rb");
		if(fp == NULL){        // オープン失敗
			size[ii] = 0;
			continue;
		}
		// ファイルサイズ取得
		fseek(fp, 0, SEEK_END);						// 尾端に移動
		fgetpos(fp, &size[ii]);						// ファイルポインタ位置取得 = 最終位置なのでファイルサイズが分かる
		fclose(fp);
	}
	

	//// パラメータ付きSQL文セット
	sqlrc = mycls_pDB->Prepare(cSql);
	if( sqlrc == SQL_ERROR || sqlrc == SQL_INVALID_HANDLE) {
		mycls_pDB->GetErrMsg();
		DisConnect();					// 切断
		return false;
	}

	//// 複数件バインド登録
	// これ以降の return はダメ
	SQLLEN *cbPhotoParam = new SQLLEN[paramCnt];
	memset( cbPhotoParam, 0x00, sizeof(SQLLEN)*paramCnt);
	// セット
	for( ii=0; ii<paramCnt; ii++) {
		// ここでちゃんとサイズを指定しないとダメ
		// トータル30Mを超えるようなバイナリデータの場合、SQL_LEN_DATA_AT_EXEC(0); これだとエラーになる。
//		cbPhotoParam[ii] = (SQLLEN)SQL_LEN_DATA_AT_EXEC(size[ii]);


		// バインド
		if( 0 == size[ii] ) {						// NULLデータ
			sqlrc = mycls_pDB->BindParameter(ii+1, 1, &cbPhotoParam[ii]);
		} else {									// 実データ
			sqlrc = mycls_pDB->BindParameter(ii+1, (SQLUINTEGER)size[ii], &cbPhotoParam[ii]);
		}
		if( sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO ) {
			// 正常
		} else {
			goto NG;
		}


		// パラメータの 1 のデータは実行時に指定する。    
		// 注意として、ここではパラメータの長さに       
		// SQL_LEN_DATA_AT_EXEC マクロで、0 にしている。
		// これは、SQLGetInfo の SQL_NEED_LONG_DATA_LEN 情報が 
		// "N" を返すドライバで許される。

		//varbinary(MAX)はここ
		cbPhotoParam[ii] = SQL_LEN_DATA_AT_EXEC(0);	

	}

	//// 実行
	sqlrc = mycls_pDB->Execute();
	if( sqlrc == SQL_ERROR || sqlrc == SQL_INVALID_HANDLE) {
		goto NG;
	}

	//// パラメータ登録
	for( ii=0; ii<paramCnt; ii++) {
		// パラメータ番号取得
		if( SQL_NEED_DATA == mycls_pDB->ParamData(&pToken)) {
			// バインドがまだの為OK
		} else {
			goto NG;
		}

		// データセット 
		if( NULL == size[ii] ) {					// NULLデータ
			sqlrc = mycls_pDB->PutData(0, 0);
		} else {									// 実データ
			long	nReadSize;
			__int64 nTotalSize = 0;
			// バイナリファイル書き込み (ここで失敗するはずが無い)
			fp = fopen(&cFPath[ii*pathsize], "rb");
			while( nReadSize = (long)fread( data, sizeof(BYTE), BinalyFileOneWriteSize, fp ) ) {

				sqlrc =mycls_pDB->PutData(data, nReadSize);
				if (sqlrc == SQL_ERROR || sqlrc == SQL_INVALID_HANDLE) {
					goto NG;
				}
				nTotalSize += nReadSize;
			}
			fclose(fp);
		}
	}

	//// パラメータ番号取得
	// ここで実際に書き込むはず。 むちゃむちゃ時間がかかっている
	if( SQL_SUCCESS == mycls_pDB->ParamData(&pToken) ) {
		// 正常
	} else {
		goto NG;
	}

	//// 終了処理
	if (NULL != cbPhotoParam) {
		delete [] cbPhotoParam; 
		cbPhotoParam = NULL;
	}
	delete size;
	delete data;
	return true;
NG:
	mycls_pDB->GetErrMsg();
	if(bErrAutoClose) DisConnect();					// 切断
	if (NULL != cbPhotoParam) {
		delete [] cbPhotoParam; 
		cbPhotoParam = NULL;
	}
	delete size;
	delete data;
	return false;
}


//------------------------------------------
// データ取得
// WORD n カラム位置 (1オリジン)
// LPVOID pVal バッファポインタ
// DWORD size バッファサイズ
// 戻り値 復帰情報
//------------------------------------------
// 文字
long OdbcBase::GetData(WORD n, LPVOID pVal, DWORD size)
{
	if( ! mycls_pDB->IsConnect()) return -1;			// 未接続
	return mycls_pDB->GetData(n, pVal, size);
}
// 整数
long OdbcBase::GetData(WORD n, short *pVal)
{
	if( ! mycls_pDB->IsConnect()) return -1;			// 未接続
	return mycls_pDB->GetData(n, pVal);
}
long OdbcBase::GetData(WORD n, int *pVal)
{
	if( ! mycls_pDB->IsConnect()) return -1;			// 未接続
	return mycls_pDB->GetData(n, pVal);
}
long OdbcBase::GetData(WORD n, long *pVal)
{
	if( ! mycls_pDB->IsConnect()) return -1;			// 未接続
	return mycls_pDB->GetData(n, pVal);
}
// 実数
long OdbcBase::GetData(WORD n, float *pVal)
{
	if( ! mycls_pDB->IsConnect()) return -1;			// 未接続
	return mycls_pDB->GetData(n, pVal);
}
long OdbcBase::GetData(WORD n, double *pVal)
{
	if( ! mycls_pDB->IsConnect()) return -1;			// 未接続
	return mycls_pDB->GetData(n, pVal);
}

//------------------------------------------
// 日付型
// WORD n カラム位置 (0オリジン)
//------------------------------------------
// int [3]
long OdbcBase::GetDataDate(WORD n, int *pVal)
{
	if( ! mycls_pDB->IsConnect()) return -1;			// 未接続
	return mycls_pDB->GetDataDate(n, pVal);
}
// int [6]
long OdbcBase::GetDataDateTime(WORD n, int *pVal)
{
	if( ! mycls_pDB->IsConnect()) return -1;			// 未接続
	return mycls_pDB->GetDataDateTime(n, pVal);
}

//------------------------------------------
// バイナリデータ取得
// WORD n カラム位置 (0オリジン)
// LPVOID pVal バッファポインタ
// DWORD size バッファサイズ
// 戻り値 取得バイト数
//------------------------------------------
long OdbcBase::GetDataImage(WORD n, LPVOID pVal, DWORD size )
{
	if( ! mycls_pDB->IsConnect()) return -1;			// 未接続
	return mycls_pDB->GetDataImage(n, pVal, size);
}



// //////////////////////////////////////////////////////////////////////////////
// SQL文 文字操作関連
// //////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// insert
// LPSTR pSql 編集するSQL文
// int bufsize 編集するSQL文のバッファ
// const char *val 追加する文字列
// int size 文字列の長さ (0の場合、NULLを自動セット)
// 戻り値 取得バイト数
//------------------------------------------
// 文字列
int OdbcBase::sqlColumnInsert(LPSTR pSql, const char *val, int size)
{
	// データがNULL
	if( NULL == val || 0 == size ) {
		//return sprintf(pSql+strlen(pSql), bufsize-strlen(pSql), "NULL," );
		return sprintf(pSql+strlen(pSql), "NULL," );
	}
	
	// ワークセット
	int iwk = size+8;
	char *cpwk = new char[iwk];
	memset(cpwk, 0x00, iwk);
	memcpy(cpwk, val, size);

	// データセット
	int len;
	//len = sprintf_s( pSql+strlen(pSql), bufsize-strlen(pSql), "'%s',", cpwk );
	len = sprintf( pSql+strlen(pSql), "'%s',", cpwk );
	delete [] cpwk;
	return len;
}
// 整数
int OdbcBase::sqlColumnInsert(LPSTR pSql, short val)
{
	//return sprintf_s(pSql+strlen(pSql), bufsize-strlen(pSql), "%d,", val);
	return sprintf(pSql+strlen(pSql), "%d,", val);
}
int OdbcBase::sqlColumnInsert(LPSTR pSql, int val)
{
	//return sprintf_s(pSql+strlen(pSql), bufsize-strlen(pSql), "%d,", val);
	return sprintf(pSql+strlen(pSql), "%d,", val);
}
int OdbcBase::sqlColumnInsert(LPSTR pSql, long val)
{
	//return sprintf_s(pSql+strlen(pSql), bufsize-strlen(pSql), "%d,", val);
	return sprintf(pSql+strlen(pSql), "%d,", val);
}
int OdbcBase::sqlColumnInsert(LPSTR pSql, DWORD val)
{
	//return sprintf_s(pSql+strlen(pSql), bufsize-strlen(pSql), "%d,", val);
	return sprintf(pSql+strlen(pSql), "%d,", val);
}
// 実数
int OdbcBase::sqlColumnInsert(LPSTR pSql, float val)
{
	//return sprintf_s(pSql+strlen(pSql), bufsize-strlen(pSql), "%0.6f,", val);
	return sprintf(pSql+strlen(pSql), "%0.6f,", val);
}
int OdbcBase::sqlColumnInsert(LPSTR pSql, double val)
{
	//return sprintf_s(pSql+strlen(pSql), bufsize-strlen(pSql), "%0.15lf,", val);
	return sprintf(pSql+strlen(pSql), "%0.15lf,", val);
}


//------------------------------------------
// update
// LPSTR pSql 編集するSQL文
// const char *name 項目名称
// const char *val 追加する文字列
// int size 文字列の長さ (0の場合、NULLを自動セット)
// 戻り値 取得バイト数
//------------------------------------------
// 文字列
int OdbcBase::sqlColumnUpdate(LPSTR pSql, const char *name, const char *val, int size)
{
	// データがNULL
	if( NULL == val || 0 == size ) {
		//return sprintf_s(pSql+strlen(pSql), bufsize-strlen(pSql), "%s=NULL,", name );
		return sprintf(pSql+strlen(pSql), "%s=NULL,", name );
	}
	
	// ワークセット
	int iwk = size+8;
	char *cpwk = new char[iwk];
	memset(cpwk, 0x00, iwk);
	memcpy(cpwk, val, size);

	// データセット
	int len;
	//len = sprintf_s(pSql+strlen(pSql), bufsize-strlen(pSql), "%s='%s',", name, cpwk );
	len = sprintf(pSql+strlen(pSql), "%s='%s',", name, cpwk );
	delete [] cpwk;
	return len;
}
// 整数
int OdbcBase::sqlColumnUpdate(LPSTR pSql, const char *name, int val)
{
	return sprintf(pSql+strlen(pSql), "%s=%d,", name, val);
}
int OdbcBase::sqlColumnUpdate(LPSTR pSql, const char *name, long val)
{
	return sprintf(pSql+strlen(pSql), "%s=%d,", name, val);
}
// 実数
int OdbcBase::sqlColumnUpdate(LPSTR pSql, const char *name, float val)
{
	return sprintf(pSql+strlen(pSql), "%s=%0.6f,", name, val);
}
int OdbcBase::sqlColumnUpdate(LPSTR pSql, const char *name, double val)
{
	return sprintf(pSql+strlen(pSql), "%s=%0.15lf,", name, val);
}


//------------------------------------------
// SQL文の最後の "," を取り除く
// LPSTR pSql 編集するSQL文
//------------------------------------------
int OdbcBase::sqlColumnTrim(LPSTR pSql)
{
	int len = (int)strlen(pSql);
	if( ',' == pSql[len-1] ) {
		pSql[len-1] = NULL;
		return len-1;
	}
	return len;
}
//------------------------------------------
// SQL文の登録日付を生成 (文字列)
// CString *cSql 編集するSQL文
//------------------------------------------
int OdbcBase::sqlColumnAddTime(LPSTR pSql)
{
	SYSTEMTIME time;
    GetLocalTime(&time);

//	return sprintf_s(pSql+strlen(pSql), bufsize-strlen(pSql), "'%d/%d/%d %d:%d:%d'", 
//				time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

	return sprintf(pSql+strlen(pSql), "'%d/%d/%d %d:%d:%d'", 
				time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
}

// //////////////////////////////////////////////////////////////////////////////
// 標準的なSelect文の実行取得
// //////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// 単一行1項目のintデータのみ取得 (おもにcount(*)のSQL文実行に使用)
// char const *cSql 実行するSQL文
// int size バッファサイズ
// char *pVal 取得データ
// 戻り値 復帰情報
//------------------------------------------
bool OdbcBase::GetSelectKey(char const *cSql, int size, char *pVal)
{
	if( ! mycls_pDB->IsConnect()) return false;			// 未接続

	// SQL実行
	if(! this->ExecuteDirect( cSql ) ){
		this->DisConnect();
		return false;
	}

	// フェッチ
	int sqlrc = this->Fetch();	
	if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り	(切断しない)
		this->GetData(1, pVal, size);
		return true;
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し	(切断しない)
		*pVal = 0;
		return false;					
	} else {											// エラー		(切断される)
		*pVal = 0;
		return false;
	}
}
bool OdbcBase::GetSelectKey(char const *cSql, int *pVal)
{
	if( ! mycls_pDB->IsConnect()) return false;			// 未接続
	// SQL実行
	if(! this->ExecuteDirect( cSql ) ){
		this->DisConnect();
		return false;
	}

	// フェッチ
	int sqlrc = this->Fetch();	
	if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り	(切断しない)
		this->GetData(1, pVal);
		return true;
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し	(切断しない)
		*pVal = 0;
		return false;					
	} else {											// エラー		(切断される)
		*pVal = 0;
		return false;
	}
}
bool OdbcBase::GetSelectKey(char const *cSql, float *pVal)
{
	if( ! mycls_pDB->IsConnect()) return false;			// 未接続
	// SQL実行
	if(! this->ExecuteDirect( cSql ) ){
		this->DisConnect();
		return false;
	}

	// フェッチ
	int sqlrc = this->Fetch();	
	if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り	(切断しない)
		this->GetData(1, pVal);
		return true;
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し	(切断しない)
		*pVal = 0;
		return false;					
	} else {											// エラー		(切断される)
		*pVal = 0;
		return false;
	}
}
bool OdbcBase::GetSelectKey(char const *cSql, double *pVal)
{
	if( ! mycls_pDB->IsConnect()) return false;			// 未接続
	// SQL実行
	if(! this->ExecuteDirect( cSql ) ){
		this->DisConnect();
		return false;
	}

	// フェッチ
	int sqlrc = this->Fetch();	
	if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り	(切断しない)
		this->GetData(1, pVal);
		return true;
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し	(切断しない)
		*pVal = 0;
		return false;					
	} else {											// エラー		(切断される)
		*pVal = 0;
		return false;
	}
}
// 日付取得 int[6]
bool OdbcBase::GetSelectDateTime(char const *cSql, int *pVal)
{
	if( ! mycls_pDB->IsConnect()) return false;			// 未接続
	// SQL実行
	if(! this->ExecuteDirect( cSql ) ){
		this->DisConnect();
		return false;
	}

	// フェッチ
	int sqlrc = this->Fetch();	
	if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り	(切断しない)
		this->GetDataDateTime(1, pVal);
		return true;
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し	(切断しない)
		return false;					
	} else {											// エラー		(切断される)
		return false;
	}
}
// バイナリデータ
bool OdbcBase::GetSelectImage(char const *cSql, int size, char *pVal)
{
	if( ! mycls_pDB->IsConnect()) return false;			// 未接続
	// SQL実行
	if(! this->ExecuteDirect( cSql ) ){
		this->DisConnect();
		return false;
	}

	// フェッチ
	int sqlrc = this->Fetch();	
	if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り	(切断しない)
		this->GetDataImage(1, pVal, size);
		return true;
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し	(切断しない)
		*pVal = 0;
		return false;					
	} else {											// エラー		(切断される)
		*pVal = 0;
		return false;
	}
}


//------------------------------------------
// 単一行1項目のintデータのみ取得 (おもにcount(*)のSQL文実行に使用)
// char const *cSql 実行するSQL文
// int size バッファサイズ
// char *pVal 取得データ
// LPCSTR cIniPath iniファイルパス
// LPCSTR cSession セッション
// 戻り値 復帰情報
//------------------------------------------
bool OdbcBase::GetSelectKey_Static(char const *cSql, int size, char *pVal, LPCSTR cIniPath, LPCSTR cSession)
{
	OdbcBase cls_DB;
	if( ! cls_DB.Connect(cIniPath, cSession) ) return false;
	return cls_DB.GetSelectKey(cSql, size, pVal);
}

bool OdbcBase::GetSelectKey_Static(char const *cSql, int *pVal, LPCSTR cIniPath, LPCSTR cSession)
{
	OdbcBase cls_DB;
	if( ! cls_DB.Connect(cIniPath, cSession) ) return false;
	return cls_DB.GetSelectKey(cSql, pVal);
}
bool OdbcBase::GetSelectKey_Static(char const *cSql, float *pVal, LPCSTR cIniPath, LPCSTR cSession)
{
	OdbcBase cls_DB;
	if( ! cls_DB.Connect(cIniPath, cSession) ) return false;
	return cls_DB.GetSelectKey(cSql, pVal);
}
bool OdbcBase::GetSelectKey_Static(char const *cSql, double *pVal, LPCSTR cIniPath, LPCSTR cSession)
{
	OdbcBase cls_DB;
	if( ! cls_DB.Connect(cIniPath, cSession) ) return false;
	return cls_DB.GetSelectKey(cSql, pVal);
}
bool OdbcBase::GetSelectDateTime_Static(char const *cSql, int *pVal, LPCSTR cIniPath, LPCSTR cSession)
{
	OdbcBase cls_DB;
	if( ! cls_DB.Connect(cIniPath, cSession) ) return false;
	return cls_DB.GetSelectDateTime(cSql, pVal);
}
bool OdbcBase::GetSelectImage_Static(char const *cSql, int size, char *pVal, LPCSTR cIniPath, LPCSTR cSession)
{
	OdbcBase cls_DB;
	if( ! cls_DB.Connect(cIniPath, cSession) ) return false;
	return cls_DB.GetSelectImage(cSql, size, pVal);
}


// //////////////////////////////////////////////////////////////////////////////
// 便利SQL
// //////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// リンクサーバーを追加
// bool bDelete true:既存のリンクサーバーを一度消す
// char const *cLinkName リンクサーバー名
// char const* addr データベースアドレス
// char const* dsn  データベース名
// char const* uid  ユーザー名
// char const* pwd  パスワード
// bool bDelete  true:既存のリンクサーバーを一度消す  (デフォルト：true)
// char const* srvproduct 製品名 (デフォルト:mySQLServer)
// char const* provider プロバイダ (デフォルト:SQLNCLI) ※ GUIのプロバイダ値をそのままセットしたらNGだった。専用の文字列が有りそうなので注意
// 戻り値 復帰情報
//------------------------------------------
bool OdbcBase::ExecuteDirect_AddLinkServer(char const *cLinkName, char const* addr, char const* dsn, char const* uid, char const* pwd, bool bDelete, char const* srvproduct, char const* provider)
{
	if( ! mycls_pDB->IsConnect()) return false;			// 未接続

	char cSql[256];

	// 既存のリンクサーバーがあれば削除
	if( bDelete ) {
		memset(cSql, 0x00, sizeof(cSql));
		sprintf(cSql, "sp_dropserver '%s', droplogins", cLinkName);

		if(! this->ExecuteDirect( cSql, false ) ){
			// 既存が無い場合はエラーとなるが、別に問題ない為、何もしない
			//this->DisConnect();
			//return false;
		}
	}

	// リンクサーバー追加
	memset(cSql, 0x00, sizeof(cSql));
	sprintf(cSql, "sp_addlinkedserver @server='%s',@srvproduct='%s',@provider='%s',@datasrc='%s',@catalog='%s'",
		cLinkName, srvproduct, provider, addr, dsn);

	if(! this->ExecuteDirect( cSql ) ){
		this->DisConnect();
		return false;
	}

	// ログインユーザーの設定
	memset(cSql, 0x00, sizeof(cSql));
	sprintf(cSql, "sp_addlinkedsrvlogin @rmtsrvname='%s',@useself=false,@rmtuser='%s',@rmtpassword='%s'",
		cLinkName, uid, pwd);

	if(! this->ExecuteDirect( cSql ) ){
		this->DisConnect();
		return false;
	}

	return true;
}

// //////////////////////////////////////////////////////////////////////////////
// 完全な補助機能関連
// //////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// iniファイルのセッションが同一か判定
// LPCSTR cIniPath1 iniファイルパス
// LPCSTR cSession1 セッション
// LPCSTR cIniPath2 iniファイルパス
// LPCSTR cSession2 セッション
// 戻り値 復帰情報
//------------------------------------------
bool OdbcBase::CheckIniSession(LPCSTR cIniPath1, LPCSTR cSession1, LPCSTR cIniPath2, LPCSTR cSession2)
{
	//  Cで使用する項目のみで判定
//	char wk_addr[2][128];
	char wk_odbc[2][128];
//	char wk_dsna[2][128];
	char wk_user[2][128];
	char wk_pass[2][128];

//	GetPrivateProfileString(cSession1, "DBADDLESS", "", (char *)wk_addr[0], sizeof(wk_addr), cIniPath1);
	GetPrivateProfileString(cSession1, "ODBCNAME",  "", (char *)wk_odbc[0], sizeof(wk_odbc), cIniPath1);
//	GetPrivateProfileString(cSession1, "DSNAME",    "", (char *)wk_dsna[0], sizeof(wk_dsna), cIniPath1);
	GetPrivateProfileString(cSession1, "USERID",    "", (char *)wk_user[0], sizeof(wk_user), cIniPath1);
	GetPrivateProfileString(cSession1, "PASSWORD",  "", (char *)wk_pass[0], sizeof(wk_pass), cIniPath1);

//	GetPrivateProfileString(cSession2, "DBADDLESS", "", (char *)wk_addr[1], sizeof(wk_addr), cIniPath2);
	GetPrivateProfileString(cSession2, "ODBCNAME",  "", (char *)wk_odbc[1], sizeof(wk_odbc), cIniPath2);
//	GetPrivateProfileString(cSession2, "DSNAME",    "", (char *)wk_dsna[1], sizeof(wk_dsna), cIniPath2);
	GetPrivateProfileString(cSession2, "USERID",    "", (char *)wk_user[1], sizeof(wk_user), cIniPath2);
	GetPrivateProfileString(cSession2, "PASSWORD",  "", (char *)wk_pass[1], sizeof(wk_pass), cIniPath2);

	if(
//		0 ==strcmp(wk_addr[0], wk_addr[1]) &&
		0 ==strcmp(wk_odbc[0], wk_odbc[1]) &&
//		0 ==strcmp(wk_dsna[0], wk_dsna[1]) &&
		0 ==strcmp(wk_user[0], wk_user[1]) &&
		0 ==strcmp(wk_pass[0], wk_pass[1]) 
		) return true;
	else  return false;
}

// //////////////////////////////////////////////////////////////////////////////
// 非同期SQL実行
// //////////////////////////////////////////////////////////////////////////////

//------------------------------------------
// 非同期実行
// LPCSTR cSql SQL文
// bool bFast  true:初回実行  false:2回目以降
// bool bErrAutoClose エラー時自動切断機能 (デフォルト true)
// 戻り値：-1:異常  0:完了  1:SQL実行中
//------------------------------------------
int OdbcBase::ExecuteDirectAsync(LPCSTR cSql, bool bFast, bool bErrAutoClose)
{
	if( ! mycls_pDB->IsConnect()) return false;			// 未接続
	SQLRETURN sqlrc;

	//// 実行
	sqlrc = mycls_pDB->ExecuteDirectAsync(cSql, bFast);

	//// 結果チェック
	if( sqlrc == SQL_ERROR || sqlrc == SQL_INVALID_HANDLE) {
		mycls_pDB->GetErrMsg();
		if(bErrAutoClose) DisConnect();			// 切断
		return -1;
	} else if( sqlrc == SQL_STILL_EXECUTING ) {
		return 1;
	} else {
		return 0;
	}
}
//------------------------------------------
// 非同期実行キャンセル
// 戻り値：復帰情報
//------------------------------------------
bool OdbcBase::ExecuteDirectAsyncCancel()
{
	if( ! mycls_pDB->IsConnect()) return false;			// 未接続
	SQLRETURN sqlrc;
	//// 実行
	sqlrc = mycls_pDB->ExecuteDirectAsyncCancel();
	if( sqlrc == SQL_ERROR || sqlrc == SQL_INVALID_HANDLE) {
		mycls_pDB->GetErrMsg();
		return false;
	} else {
		return true;
	}
}
