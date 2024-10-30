#include "stdafx.h"
#include "AllUserLogOff.h"

//------------------------------------------
// コンストラクタ
//------------------------------------------
AllUserLogOff::AllUserLogOff(void) :
FuncBase("LogOff")
{
}

//------------------------------------------
// デストラクタ
//------------------------------------------
AllUserLogOff::~AllUserLogOff(void)
{
}

//------------------------------------------
// リモートユーザーログオフ実行処理
//------------------------------------------
// 戻り値
// 復帰情報(bool):true …正常終了(自分とサーバ以外はログオフ完了)
//                false…異常終了(異常の為、ログオフ未完了)
//------------------------------------------
bool AllUserLogOff::Exec()
{
	// 変数の宣言_初期化
	DWORD nMyProcessID = GetCurrentProcessId();		// 自プロセスIDを取得する
	DWORD nMySessionID = 0;							// 自セッションIDを格納する
	WTS_SESSION_INFO* psessions = 0;				// セッション情報を格納する
	DWORD nScount = 0;								// セッション数を格納する

	// 自プロセスIDから自セッションIDを取得する
	ProcessIdToSessionId( nMyProcessID, &nMySessionID );

	// セッション情報を取得する
	if( ! WTSEnumerateSessions( WTS_CURRENT_SERVER_HANDLE, 0, 1, &psessions, &nScount ) ) return false;

	// セッション名とセッション数が取得できた場合
	// セッション数分処理を行う
	for( DWORD ii = 0; ii < nScount; ii++ )
	{
		// 自分のセッションID以外とサービス用セッション(ID:0)以外とリッスンセッション65536のユーザーを除外する
		if( ( nMySessionID != psessions[ii].SessionId ) && 
			( ( 0 != psessions[ii].SessionId ) && ( 0 != lstrcmp( psessions[ii].pWinStationName, "Services") ) ) &&
			( WTSListen != psessions[ii].State ) )
		{
			// 自分のユーザ名以外とサービス用セッション以外とリッスンセッション65536のユーザーをログオフする
			BOOL bRet = WTSLogoffSession( WTS_CURRENT_SERVER_HANDLE, psessions[ii].SessionId, TRUE );
			LOG(em_MSG), "[%s] ログオフセッション名:%s ログオフセッションID:%d ログオフ関数戻り値:%d ", my_sThreadName, psessions[ii].pWinStationName, psessions[ii].SessionId, bRet );		// リストボックスにログオフしたセッションIDを表示させる
			continue;									// 次のセッションID確認
		}
		LOG(em_MSG), "[%s] ログオンのまま (ログオフセッション名:%s セッションID:%d)", my_sThreadName, psessions[ii].pWinStationName, psessions[ii].SessionId );		// リストボックスにログオフしたセッションIDを表示させる
	}
	// メモリを開放する
	WTSFreeMemory( psessions );
	return true;
}

/***
typedef BOOL
WINAPI
WTSConnectSessionA_Type(
    IN ULONG LogonId,
    IN ULONG TargetLogonId,
    __in PSTR pPassword,
    IN BOOL bWait
    );
***/


//------------------------------------------
// コンソール側が 表でない時には、コンソールを表に変更する
//   ※Cでは今のところ使わない。サンプル程度。ちゃんと動く
//------------------------------------------
bool AllUserLogOff::Change()
{
	bool bRetc = false; 
/*** VS2010以降じゃないとコンパイル通らない。 → WTSConnectSessionA_Type 対応でVS2005でも動くはずだが。
	HMODULE hDll = LoadLibrary("wtsapi32.dll");
	WTSConnectSessionA_Type *pWTSConnectSessionA = (WTSConnectSessionA_Type*)GetProcAddress( hDll, "WTSConnectSessionA");
	if( NULL == pWTSConnectSessionA)
		return false;


	// 変数の宣言_初期化
	DWORD nMyProcessID = GetCurrentProcessId();		// 自プロセスIDを取得する
	DWORD nMySessionID = 0;							// 自セッションIDを格納する
	WTS_SESSION_INFO* psessions = 0;				// セッション情報を格納する
	DWORD nScount = 0;								// セッション数を格納する

	// 自プロセスIDから自セッションIDを取得する
	ProcessIdToSessionId( nMyProcessID, &nMySessionID );

	// セッション情報を取得する
	if( ! WTSEnumerateSessions( WTS_CURRENT_SERVER_HANDLE, 0, 1, &psessions, &nScount ) ) return false;

	// コンソールセッションを特定
	DWORD	nConsoleId = 0;
	for( DWORD ii = 0; ii < nScount; ii++) {
		if(0 == lstrcmp( psessions[ii].pWinStationName, "Console") ) {
			nConsoleId = psessions[ii].SessionId;
			break;
		}
	}

	// 切り替え
	if( 0 != nConsoleId && nMySessionID != nConsoleId ) {
		// 【重要】第３引数はパスワード。 同じであれば空白でよいはず。
		if( pWTSConnectSessionA( nMySessionID, nConsoleId, "", true) )  bRetc = true;
	}

	// メモリを開放する
	WTSFreeMemory( psessions );
***/
	return bRetc;
}