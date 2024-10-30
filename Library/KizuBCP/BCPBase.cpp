#include "StdAfx.h"
#include "BCPBase.h"

using namespace KizuODBC;

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// コンストラクタ
//------------------------------------------
BCPBase::BCPBase(void) : 
m_hdbc(NULL),
mycls_pDB(NULL)
{
	memset(m_cTableName, 0x00, sizeof(m_cTableName) );

	// 生成 (接続ハンドルまで生成)
	mycls_pDB = new BCPManager();
}

//------------------------------------------
// デストラクタ
//------------------------------------------
BCPBase::~BCPBase(void)
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
// セッション指定での接続
// LPCSTR cIniPath iniファイルパス
// LPCSTR cSession セッション
// 戻り値 復帰情報
//------------------------------------------
bool BCPBase::Connect(LPCSTR cIniPath, LPCSTR cSession)
{
	if( NULL==mycls_pDB )				return false;
	if( ! mycls_pDB->IsCreateHandle() )	return false;		// ハンドル生成NG
	if( mycls_pDB->IsConnect() )		return true;		// 接続済み

	int retc;
	if(NULL == cSession) retc = mycls_pDB->Connect(cIniPath);
	else				 retc = mycls_pDB->Connect(cIniPath, cSession);

	// 基本をセット
	m_hdbc = mycls_pDB->GetHDBC();				// BCP系で必要な為 コネクト成功時に保持しておく
	
	if( 0 == retc ) {					// 新規接続
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
void BCPBase::DisConnect()
{
	// 切断
	if(NULL != mycls_pDB) {
		mycls_pDB->DisConnect(); 
	}
}


// //////////////////////////////////////////////////////////////////////////////
// BCP系
// //////////////////////////////////////////////////////////////////////////////

//------------------------------------------
// BCP系 初期化
// LPCSTR cTableName 接続テーブル (正式には、「ODBCの名称..DB名称」だが、[DB名称]のみでもOK)
// 戻り値 復帰情報
//
// ※ bcp_init は、同一テーブルに対し、複数やったらダメ。(= 書き込む人が複数だとNG)
//------------------------------------------
bool BCPBase::BCP_Init()
{
	return BCP_Init( (LPCSTR)mycls_pDB->GetDSN() );
}
bool BCPBase::BCP_Init(LPCSTR cTableName)
{
	// テーブル名格納
	strcpy(m_cTableName, cTableName);

	if( SUCCEED == bcp_init(m_hdbc, cTableName, NULL, NULL, DB_IN ) ) {
//	if( SUCCEED == bcp_init(m_hdbc, "test_tbl", NULL, NULL, DB_IN ) ) {
		return true;
	} else {
		return false;
	}
}
//------------------------------------------
// BCP系 バインド (値型)
// int coulmn カラム位置 (1オリジン)
// BYTE* pData バインドする先頭アドレス (このアドレスにセットされているデータがそのままＤＢへ転送される)
// emBcpBindType typ バインドする型
// 戻り値 復帰情報
// -------------------------------
// 呼び元で、バインドするアドレスを固定しておく事
//------------------------------------------
bool BCPBase::BCP_Bind_Value(int coulmn, BYTE* pData, emBcpBindType typ)
{
	if( SUCCEED == bcp_bind(m_hdbc, pData, 0, SQL_VARLEN_DATA, NULL, 0, typ, coulmn) ) {
		return true;
	} else {
		return false;
	}
}

//------------------------------------------
// BCP系 バインド (バイナリ配列型)
// int coulmn カラム位置 (1オリジン)
// typBindData* pData バインドするバイナリデータの先頭アドレス (データサイズと実データは連続しておく必要がある。)
// emBcpBindType typ バインドする型 (em_Binary,em_VerCharがほぼ固定)
// 戻り値 復帰情報
// -------------------------------
// 呼び元で、バインドするアドレスを固定しておく事
//------------------------------------------
bool BCPBase::BCP_Bind_Binary(int coulmn, typBindData* pData, emBcpBindType typ)
{
	if( SUCCEED == bcp_bind(m_hdbc, (BYTE*)pData, 4, SQL_VARLEN_DATA, NULL, 0, typ, coulmn) ) {
//	if( SUCCEED == bcp_bind(m_hdbc, (BYTE*)pData, 4, SQL_NULL_DATA, NULL, 0, typ, coulmn) ) {
		return true;
	} else {
		return false;
	}
}
bool BCPBase::BCP_Bind_Binary(int coulmn, typBindDataEx* pData, emBcpBindType typ)
{
	if( SUCCEED == bcp_bind(m_hdbc, pData->GetBindPointer(), 8, SQL_VARLEN_DATA, NULL, 0, typ, coulmn) ) {
		return true;
	} else {
		return false;
	}
}

// 直接登録。先頭部分8バイトがデータサイズな連続データ。使うときに毎回再バインドする必要有り
bool BCPBase::BCP_Bind_VerBinary(int coulmn, BYTE* pData)
{
	if( SUCCEED == bcp_bind(m_hdbc, pData, 8, SQL_VARLEN_DATA, NULL, 0, SQLVARBINARY, coulmn) ) {
	//if( SUCCEED == bcp_bind(m_hdbc, pData, 8, SQL_VARLEN_DATA, NULL, 0, SQLBIGVARBINARY, coulmn) ) {
	//if( SUCCEED == bcp_bind(m_hdbc, pData, 4, SQL_VARLEN_DATA, NULL, 0, SQLVARBINARY, coulmn) ) {
		return true;
	} else {
		return false;
	}
}

////// BCPでNULLデータのセット方法がわからない為 暫定
// pDataの先頭4バイトをFFで埋めるとNULLが登録されることを発見。
//	無くすと影響が大きいため、一応このまま暫定として残しておく。
bool BCPBase::BCP_Bind_BinaryNull(int coulmn, typBindData* pData, emBcpBindType typ)
{
//	if( SUCCEED == bcp_bind(m_hdbc, (BYTE*)pData, 4, SQL_VARLEN_DATA, NULL, 0, typ, coulmn) ) {
	if( SUCCEED == bcp_bind(m_hdbc, (BYTE*)pData, 4, SQL_NULL_DATA, NULL, 0, typ, coulmn) ) {
		return true;
	} else {
		return false;
	}
}

//------------------------------------------
// BCP系 バインド (文字列型 終端NULL必須)
// int coulmn カラム位置 (1オリジン)
// BYTE* pData バインドする先頭アドレス (終端のNULL必須)
// 戻り値 復帰情報
// -------------------------------
// 呼び元で、バインドするアドレスを固定しておく事
//------------------------------------------
bool BCPBase::BCP_Bind_String(int coulmn, char* pData)
{
	if( SUCCEED == bcp_bind(m_hdbc, (BYTE*)pData, 0, SQL_VARLEN_DATA, (UCHAR*)"", 1, SQLCHARACTER, coulmn) ) {
		return true;
	} else {
		return false;
	}
}

//------------------------------------------
// BCP系 DBへデータ転送
// 戻り値 復帰情報
//------------------------------------------
bool BCPBase::BCP_SendRow()
{
	RETCODE rec = bcp_sendrow(m_hdbc);
	if( SUCCEED == rec ) {
		return true;
	} else {
		return false;
	}
}
//------------------------------------------
// BCP系 データ転送したデータのコミットみたいな奴
// 戻り値 コミットした件数
//------------------------------------------
int BCPBase::BCP_Batch()
{
	return bcp_batch(m_hdbc);
}

//------------------------------------------
// BCP系 開放 (Batchされてない奴も勝手にBatchされる？)
// 戻り値 復帰情報
//------------------------------------------
bool BCPBase::BCP_Down()
{
	// 本来は、保存された件数を返す。また、-1なら異常
	if( -1 != bcp_done(m_hdbc) ) {
		return true;
	} else {
		return false;
	}
}


//------------------------------------------
// BCP系 日付登録情報を取得
// char* cTime セットする日付エリア (20byte以上必要)
//------------------------------------------
void BCPBase::Get_BPC_Time(char* cTime)
{
	SYSTEMTIME time;
    GetLocalTime(&time);

	sprintf(cTime, "%04d-%02d-%02d %02d:%02d:%02d", 
				time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
}

//------------------------------------------
// BCP系 バインドのバイナリデータサイズ指定 ()
// int coulmn テーブルのカラム位置 (1オリジン)
// long nSize サイズ
// 戻り値 復帰情報
// -------------------------------
// varbinary(MAX) だと 0xFFFF 以上のサイズがうまいこと登録できない暫定対応策
//------------------------------------------
bool BCPBase::BCP_ColLen(int coulmn, long nSize)
{
	if( 0xFFFF >= nSize ) {
		// 必要無し。というか、指定しても反映されない。インジケータが優先される
		return true;
	} else {
		// 0xFFFF 以上だと インジケータが優先されない。なぜ？
		RETCODE rec = bcp_collen(m_hdbc, nSize, coulmn);
		if( SUCCEED == rec ) {
			return true;
		} else {
			return false;
		}
	}
}


