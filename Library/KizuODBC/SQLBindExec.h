#pragma once
// *********************************************************************************
//	VB.NET側のSQLCommandみたいなクラスのODBC版。
//	[Ver]
//		Ver.01    2015/09/07  
//
//	[メモ]
//		ひとつのOdbcManagerから複数のSQLBindExecが生成できる。なるべく使いまわすのがよい。
//		なお、大本のOdbcManagerの切断時には、子供のSQLBindExecは全滅する。メンバ変数で持つときは注意。
//
//		Bindはポインタにするので、Executeするときまで実体が存在すること。（特に自動変数の寿命に注意）。
//		CStringをBindするときは、Bind～Execute間では中身を変更しないこと（CString内部のアドレスが変更されるとNG）。
//	[SQL]
//		INSERT文は、"INSERT INTO [TABLE]"でOK. VALUES (?,?...)は自動付与。BindのColNameは不使用なので自由。
//		UPDATE文は、"UPDATE [TABLE] WHERE ..." でOK. SET a=?,b=?...は自動付与。
//			ただし、SET側とWHERE側のパラメタはBindのColNameで区別。SET側は列名、Where側はNULLにすること。
//			SET側にSQL関数を使いたい場合は、BindSQLFunctionで"列名=関数名"で与えること。
//
//		SELECT文はSQLBindRecvColumn側をつかうこと。
// *********************************************************************************

#include "SQLBindBase.h"


namespace KizuODBC
{
	class OdbcManager;
	class OdbcBase;

	typedef  SQLRETURN RequestBlobDataType(class SQLBindExec *that, SQLPOINTER pToken);

	//ODBCのステートメントを担当するクラス. パラメタライズドクエリ；
	//何度もExecuteを実行するとき用に。
	class SQLBindExec : public SQLBindBase
	{
	protected:
		SQLHSTMT m_hDBC;		//基本的には参照
		SQLHSTMT m_hStmt;		//所有物

		const char *m_original_sql;
		char *m_replacedSQL;
		const char *m_sQuery;	//クエリ文字列
		std::deque<SQLLEN> m_szHolder;		//Bind変数のサイズホルダー.[1]が第一列。[0]は未使用。
		std::deque<const char *>m_sColNames;

		SQLLEN* set_holder(const char *sColName, int len);

		bool m_bPrep;			//プリペアードならTRUE.

		SQLCHAR					m_cLastErrMsgMain[256];

		//m_hStmtからエラーメッセージを取得する。
		int RecordError();

		//実行部分。仮SQLをVALUES(?,?,?)など足して本番に切り替える
		virtual void SQLBindExec::CreateSQL();

	public:
		SQLHSTMT GetStmt() const { return m_hStmt; }
		const char *QueryString() const { return m_sQuery; }
		//最後に失敗した際のエラーメッセージを返す。
		const char *GetErrMsg() const { return (const char*)m_cLastErrMsgMain; }


		//Manager: 接続済みのODBCManager。
		// sQeury: 構築するクエリ。INSERTとUPDATEには特例がある
		//			INSERT文は、"INSERT INTO [TABLE]"でOK. VALUES (?,?...)は自動付与。BindのColNameは不使用なので自由。
		//			UPDATE文は、"UPDATE [TABLE] WHERE ..." でOK. SET a=?,b=?...は自動付与。
		//				ただし、SET側とWHERE側のパラメタはBindのColNameで区別。SET側は列名、Where側はNULLにすること。
		//				SET側にSQL関数を使いたい場合は、BindSQLFunctionで"列名=関数名"で与えること。
		//  bPrep: Prepareするか。同一インスタンスで５回以上Executeする予定ならTrue.
		// 　  　　ただし、ストアドを呼び出す場合は、falseのほうがよい。
		SQLBindExec(KizuODBC::OdbcManager &manager, const char *sQuery, bool bPrep = false);
		//SQLBindExec(KizuODBC::OdbcBase &manager, const char *sQuery, bool bPrep = false);
		~SQLBindExec(void);

		//BIND関連部分
			//ダミーパラメータ。
		class NoParameter {};
		static NoParameter *NoParam() { return 0; };
		int BindSQLFunction(const char *sColName)
		{
			set_holder(sColName, 0);
			return SQL_SUCCESS;
		}

		//パラメタをバインド。
		// 注意。ODBCのバインドは、「変数のアドレス」と「？」をヒモづける。
		// バインドされている間(≒インスタンスが生存している間)は、変数は有効なアドレスである必要がある。
		// ポインタを登録しているので、memcpyなどで実体側のメモリ内容を変更してExecuteすれば、パラメタも変わっている。

		//数値
		int BindNum(const char *sColName, const int *nPrepKey);
		int BindNum(const char *sColName, const unsigned int *nPrepKey) { return BindNum(sColName, (const int*)nPrepKey); }
		int BindNum(const char *sColName, const long *nPrepKey) { return BindNum(sColName, (const int*)nPrepKey); }
		int BindNum(const char *sColName, const unsigned long *nPrepKey) { return BindNum(sColName, (const int*)nPrepKey); }
		int BindNum(const char *sColName, const float *nPrepKey);
		int BindNum(const char *sColName, const double *nPrepKey);

		//文字列。（nMaxColumnWidthは、DB側の幅。クエリの組立て(Cacheの当たり具合)に効いてくる。）
		int BindStr(const char *sColName, const char *pPrepKey, int nMaxColumnWidth = -1);
		//固定長文字列（C側文字列がNUL終端せず、常にnColumnWidthだけのサイズを登録する。NUL文字があるとDBにもNUL文字が入るので注意）
		int BindFix(const char *sColName, const char *pPrepKey, int nMaxColumnWidth);
		//UUID。File Streamのときに出てくるかも。
		int Bind(const char *sColName, const UUID *pPrepKey);
		//SQL日時型。
		int Bind(const char *sColName, const SQL_TIMESTAMP_STRUCT *pTimeStamp);

		//blob( ImageとかVarBinary MAXとか) のバインド。
		// 実際のデータは後回し。
		//  提供は、BlobDataRequestedに関数を入れてコールバックを待つか
		//  このクラスを継承してOnRequestBlobDataをオーバーライドするか。
		//　Executeの直後にコールバックが呼ばれる。
		// pToken : トークン。「実際のデータ」に紐づけるためのID。 
		//　　　　　たとえば、OnBlobData_SendFileなら、ファイル名へのポインタを入れる。
		//nMaxSize: ほとんどダミー。実データより大きくて問題ないが、小さいとエラーになる。

		// image, varbinary(max) 
		int BindBlobBin(const char *sColName, const void *pToken, int nMaxSize = (1 << 30));

		// varchar(max)
		int BindBlobChar(const char *sColName, const void *pToken, int nMaxSize = (1 << 30));

		//ユーティリティ的メソッド。


		//単純に実行する。
		bool Execute();

		//実行する。実行後、HSTMTをOdbcManagerに転送する。
		//なので、OdbcManager側のFetchとかGetDataをかが使える
		SQLRETURN SelectExecute(KizuODBC::OdbcManager &manager);

		//Blobデータ送信部分
			//Executeの際、bindBlobを処理する
		virtual SQLRETURN OnRequestBlobData(SQLPOINTER pToken);
		//デフォルトでは、OnRequestBlobDataは BlobDataRequestedを呼ぶ
		RequestBlobDataType *BlobDataRequested;

		//Blobとしてファイルを送信。pToken＝ファイルパス文字列を意味するとして、そのファイルを送信する。
		static SQLRETURN OnBlobData_SendFile(SQLBindExec *that, SQLPOINTER pToken);

		//ファイルサイズをとる。本質的には不要だがSendFileするときにファイルサイズがほしくなるのはよくある。
		static long long GetFileSize(const char *filename);

		struct SendMemoryType {
			long long size;
			const void *data;
		};

		//Blobとしてメモリを送信。pToken＝SendMemoryType型を意味するとして、そのメモリを送信する。
		static SQLRETURN OnBlobData_SendMemory(SQLBindExec *that, SQLPOINTER pToken);

		// Callbackの際、bidnBlob内で要求されたデータを送出する部分。
		SQLRETURN PutData(SQLPOINTER pData, SQLINTEGER nLen);
		SQLRETURN PutNULL() { return PutData(0, 0); }
		SQLRETURN PutEmpty() { return PutData("", 0); }

	};

};