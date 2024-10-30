// *********************************************************************************
//	BCP クラス (超高速DB書込み制御クラス)
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//		使用するタスクで 『odbc32.lib』 をリンクする事
//		OdbcManager を使いやすいように皮被せ
//
// --------------------------------------------
// 使用方法
//		KIZUBCP_BCPMANAGER のコメントを外す
//		ODBCの設定
//				通常 「SQL Server」を選択しているが、「SQL Native Client」を選択する
//
// 環境構築
//	[追加インクルードパス]
//		無し
//	[追加ライブラリパス]
//		"C:\Program Files\Microsoft SQL Server\90\SDK\Lib\x86"
//	[追加ライブラリファイル]
//		sqlncli.lib
// *********************************************************************************

#pragma once

#include <stdio.h>
#include "BCPManager.h"														// ODBC接続基底クラス


// 疵検 インクルード
#include "..\KizuODBC\OdbcBase.h"											// 親クラス
#include "..\KizuODBC\OdbcManager.h"										

using namespace KizuODBC;

namespace KizuODBC
{

	class BCPBase// :	public OdbcBase
	{
		public:

		//------------------------------------------
		// BCP系受け渡し構造体
		//------------------------------------------

		//-------------
		// 受け渡しデータタイプ
		typedef enum enumBCP_BINDTYPE {
			em_VarChar		= SQLVARCHAR,
			em_Int			= SQLINT4,
			em_Float		= SQLFLT4,
			em_Double		= SQLFLT8,
			em_Day			= SQLDATETIME, //SQLDATETIMN,	// 使用する型は、CTime 
			em_Binary		= SQLIMAGE,			// image型
			em_VerBinary	= SQLVARBINARY
		} emBcpBindType;

		//-------------
		// バイナリ受け渡し領域 (固定長)
		/*typedef*/ struct typBindData {
			// 領域が連続で続いていないといけない為、バイナリデータはnewしない
			long    nSize;														// サイズ (バイナリデータ以外は必要無し。バインド時は、1をセット)
			BYTE	bData[KIZUBCP_MAX_IMGSIZE];									// データ
								// (intデータとかの場合は、(BYTE*)&(ntest) こんな感じでセット)
								// (バイナリデータは、(BYTE*)&(typBindData構造体)をセット )
		};

		//-------------
		// バイナリ受け渡し領域 (可変長対応)
			// 全体に対しての 初期化は絶対に禁止。Init()を使うこと
			//// 使い方
			//BCPBase::typBindDataEx  p(3);
			//memset(p.pData, 0x01, 3);
			//p.SetSize(0xF1F2F3F4);
		struct typBindDataEx {
		  private:
			BYTE *		pBase;				// ポインタ本体
		  public:
			__int64		nMaxSize;			// 画像最大サイズ
			BYTE*		pData;				// データ部ポインタ

			// コンストラクタ
			typBindDataEx() {
				nMaxSize = 0;
				pData = NULL;
				pBase = NULL;
			};

			// デストラクタ
			~typBindDataEx(void) {
				if(NULL != pBase) delete [] pBase;
			};

			// 領域確保
			void Alloc(int nMaxSize)
			{
				if(0 > nMaxSize) return;			
				this->nMaxSize	= nMaxSize;

				// 領域確保
				this->pBase		= new BYTE[nMaxSize + sizeof(__int64)];

				// 使いやすいように
				if(0 != nMaxSize) {
					this->pData	= &pBase[(sizeof(__int64))];
					Init();
				} else {
					// サイズが0のときは、固定でNULLデータをセットしたいときのための処置
					SetNullData();
				}
			};


			//// メンバー関数
			__int64 GetSize()				{ return (__int64)((__int64*)pBase)[0]; }
			void	SetSize(__int64 nSize)	{ (__int64)((__int64*)pBase)[0] = nSize; }	// サイズをセット (先頭4バイトがサイズ領域) (varbinaryは8なので注意) -1のときはNULL登録となる
			void	Init()					{ SetSize(1); }							// エリア初期化
			void	SetNullData()			{ SetSize(SQL_NULL_DATA); }				// NULL 登録は、(先頭4バイトがALL FF) (=-1のこと)

			BYTE*	GetBindPointer() const	{ return pBase; }						// バインドポイント取得
		};

	public:
		BCPBase(void);
		~BCPBase(void);
		
	//------------------------------------------
	// OdbcManager の皮被せ関連
	//------------------------------------------
		// 接続
		bool Connect(LPCSTR cIniPath, LPCSTR cSession = NULL);					// セッション指定での接続
		void DisConnect();														// 切断
		bool IsConnect() const {return mycls_pDB->IsConnect();}					// 接続中か確認


	//------------------------------------------
	// 超高速書込み用 (ホントにTO_DEFECTしか使う必要無し) (今までのバインド実行よりも10倍程度高速になるが使い方が難しいので注意)
	//      BCP系 (ライブラリに odbc32.lib とは別に odbcbcp.lib を追加する必要有り)
	//------------------------------------------
		bool BCP_Init(LPCSTR cTableName);													// BCP系 初期化
		bool BCP_Init();																	// BCP系 初期化
		bool BCP_Bind_Value (int coulmn, BYTE* pData, emBcpBindType typ);					// BCP系 バインド (値型)
		bool BCP_Bind_Binary (int coulmn, typBindData* pData, emBcpBindType typ=em_Binary);		// BCP系 バインド (バイナリ配列型)
		bool BCP_Bind_Binary (int coulmn, typBindDataEx* pData, emBcpBindType typ=em_VerBinary);// BCP系 バインド (バイナリ配列型 可変長対応)
		bool BCP_Bind_VerBinary(int coulmn, BYTE* pData);
		bool BCP_Bind_BinaryNull(int coulmn, typBindData* pData, emBcpBindType typ=em_Binary);
		bool BCP_Bind_String(int coulmn, char* pData);										// BCP系 バインド (文字列型 終端NULL必須)
		
		bool BCP_SendRow();																	// BCP系 DBへデータ転送
		int  BCP_Batch();																	// BCP系 データ転送したデータのコミットみたいな奴
		bool BCP_Down();																	// BCP系 開放
		static void Get_BPC_Time(char* cTime);												// BCP系 日付登録情報を取得

		bool BCP_ColLen(int coulmn, long nSize);											// BCP系 バインドのバイナリデータサイズ指定 (varbinary(MAX) だと 0xFFFF 以上のサイズがうまいこと登録できない暫定対応策)

	//------------------------------------------
	// 外部アクセス
	//------------------------------------------
		const char* GetTableName() const { return m_cTableName; }				// BCPで接続しているテーブル名

	protected:
		BCPManager* mycls_pDB;													// ODBC接続基底クラス (クラスを直で使う)
		SQLHDBC		m_hdbc;														// 接続ハンドル
		char		m_cTableName[256];											// 接続するテーブル名称
	};
};