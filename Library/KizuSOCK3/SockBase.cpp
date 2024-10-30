// SockBase.cpp: SockBase クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <crtdbg.h>
#include <winsock2.h>
#include <windows.h>

#include "SockBase.h"

using namespace KizuSock3;

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

//------------------------------------------
// コンストラクタ
// const char *cSession iniファイル セッション
// const char *cIniPath iniファイル パス
// DWORD tid PostThreadMessageの送り先 (ThreadIDを渡せば良い)
// DWORD timeout タイムアウト間隔 [ms]
// DWORD client_num 接続上限数 (クライアント時1固定、サーバー時接続に来る人数分を指定)
// DWORD r_size ソケットの受信バッファサイズ
// DWORD s_size ソケットの送信バッファサイズ
// DWORD msgbase メッセージベース番号
//------------------------------------------
SockBase::SockBase(const char *cSession, const char *cIniPath, DWORD tid, DWORD timeout, DWORD client_num,
				   DWORD r_size, DWORD s_size, DWORD msgbase) :
my_nClientNum(client_num),
my_nNewAcceptNgMode(0),
mycls_pKc(NULL)
{
	// 引数チェック
	_ASSERT(client_num > 0 && client_num <= KS_MAXCLIENTS);
	_ASSERT(0 != strlen(cSession) && 0 != strlen(cIniPath));
	_ASSERT(0 != tid);

	// まずは全部初期化
	for( int ii=0; ii<KS_MAXCLIENTS; ii++ ) {
		mycls_pKt[ii] = NULL;
		my_nClientAddr[ii] = NULL;
	}

	// INIファイルよりIP,ポート取得
	char cIp[20];
	long iPort;
	GetPrivateProfileString(cSession, "SAVE_IP", "", cIp, sizeof(cIp), cIniPath);
	iPort = GetPrivateProfileInt(cSession, "SAVE_PORT", 0, cIniPath);
	if( 0 == strlen(cIp) || 0 == iPort ) _ASSERT(0);

	// ksControlインスタンス生成
	mycls_pKc = new ksControl(tid, msgbase);

	//// ソケット生成
	// 0だったら デフォルトサイズをセット
	if(!r_size) r_size = KS_RCVBUF;
	if(!s_size) s_size = KS_SNDBUF;
	if (mycls_pKc->SockOpen(r_size, s_size)) {
		delete mycls_pKc;
		mycls_pKc = NULL;
		throw 1;
	}

	// IP,ポート 操作クラスに格納 (実際に使用されるのは、Connect,Listenした時ね)
	mycls_pKc->SetAddr(cIp, (short)iPort);

	// タイムアウトの設定
	if( timeout > 0 ) mycls_pKc->SetTimeout(timeout);
}

//------------------------------------------
// デストラクタ
//------------------------------------------
SockBase::~SockBase()
{
	// ソケット破棄
	if(NULL != mycls_pKc) {
		delete mycls_pKc;
		mycls_pKc = NULL;
		WSACleanup();
	}
	// クライアント用ソケットポインタを未割り当て (delete は ksControlの中でやってくれている)
	for( int ii=0; ii<KS_MAXCLIENTS; ii++ ) {
		mycls_pKt[ii] = NULL;
	}
}

//------------------------------------------
// 指定されたポインタと一致する配列位置を返す
// ksThread* ksT ksThreadのポインタ
// 戻り値 配列位置
//------------------------------------------
int SockBase::GetSockID(WPARAM ksT) const	// UINTは64bitAPの場合NG
{
	return GetSockID((ksThread *)ksT);
}
int SockBase::GetSockID(ksThread* ksT) const
{
	// Activeなものの中を探す
	for( DWORD ii=0; ii<my_nClientNum; ii++ ) {
		if( mycls_pKt[ii] == ksT ) return ii;
	}
	return -1;
}
//------------------------------------------
// 未割り当ての配列位置を返す
// 戻り値 配列位置
//------------------------------------------
int SockBase::GetFreeID() const
{
	// Activeなものの中から新規に振る
	for( DWORD ii=0; ii<my_nClientNum; ii++ ) {
		if( NULL == mycls_pKt[ii] ) return ii;
	}
	return -1;
}
//------------------------------------------
// 今生きている子スレッドの数を返す
// 戻り値 生きている数
//------------------------------------------
int SockBase::GetActive() const
{
	int num = 0;
	for( DWORD ii=0; ii<my_nClientNum; ii++ ) {
		if( NULL != mycls_pKt[ii] ) num++;
	}
	return num;
}
//------------------------------------------
// ソケットの終了区分を返す
// int id 配列アドレス
// 戻り値 終了区分
//------------------------------------------
int SockBase::GetCloseKindID(int id) const
{
	if( NULL == mycls_pKt[id]) {
		return 0;
	} else {
		return mycls_pKt[id]->GetCloseKind();
	}
}

//------------------------------------------
// コネクト (接続されるまでブロックされる)
//  クライアント時の接続関数
//------------------------------------------
int SockBase::Connect()
{
	// コネクト時は、0番目に固定
	if (NULL != mycls_pKt[0]) return -9;
	return mycls_pKc->Connect(&mycls_pKt[0]);
}

//------------------------------------------
// リスニング (ノンブロック)
//  サーバー時の接続関数
//------------------------------------------
int SockBase::Listen()
{
	return mycls_pKc->Listen(my_nClientNum);
}

//------------------------------------------
// アクセプト (アクセプト通知が来た場合に本関数をコールする) (シグナル用)
//------------------------------------------
int SockBase::Accept()
{
	// 擬似的にメッセージを受け取った事にする
	MSG msg;
	mycls_pKc->GetAcceptEventMsg(&msg);
	return Accept(&msg);
}
//------------------------------------------
// アクセプト (アクセプト通知が来た場合に本関数をコールする) (メッセージ時用)
// MSG *msg アクセプトされたときのポインタ
// 戻り値 セットした配列位置を返す(クライアントIDの事)
//------------------------------------------
int SockBase::Accept(MSG *msg)
{
	// 未割り当ての配列位置を取得
	int id = GetFreeID();

	//// 未割り当て無し 又は 既に登録済み？のインスタンスなら失敗とする
	if( -1 != id && -1 == GetSockID((ksThread *)msg->wParam) ) {
		my_nClientAddr[id] = (long)msg->lParam;
		

	} else {
		// 異常取得
		//
		//  ※ OSダウンして、自動再起動した場合には、
		// ソケットがクローズされないので、クライアントオーバーとなる
		// このような場合にはlParamを見て、同じIPであれば前のソケットを閉じる
		
		//// つながっているもので同じIPを持つものを探す
		DWORD ii;
		for( ii=0; ii<my_nClientNum; ii++ ) {
			if( NULL != mycls_pKt[ii] && my_nClientAddr[ii] == msg->lParam ) break;
		}

		//if( ii >= 0 && ii < my_nClientNum ) {		// 同じアドレスのソケットが見つかった
		if( 0==my_nNewAcceptNgMode &&
			ii >= 0 && ii < my_nClientNum ) {		// 同じアドレスのソケットが見つかった
			// 前のソケットを閉じる
			if( 0 != mycls_pKc->CloseChild( mycls_pKt[ii] ) ) _ASSERT(0);
			id = ii;	// 開放した配列位置にする

		} else {									// 未知のクライアント発生
			if( 0 != mycls_pKc->CloseChild( (ksThread *)msg->wParam ) ) _ASSERT(0);
			return -1;
		}
	}

	// クライアントID,ソケットの状態決定
	mycls_pKt[id] = (ksThread *)msg->wParam;
	return id;
}

//------------------------------------------
// 特定のクライアントを切り離す (ポインタ指定)
// ksThread* ksT 切断したい ksThread のポインタ
//------------------------------------------
void SockBase::Close(WPARAM ksT) 
{
	Close((ksThread *)ksT);
}
void SockBase::Close(ksThread* ksT) 
{
	int id = GetSockID(ksT);
	if( -1 != id ) {
		mycls_pKc->CloseChild( ksT );
		my_nClientAddr[id] = 0;
		mycls_pKt[id] = NULL;
	}
}

//------------------------------------------
// 特定のクライアントを切り離す (配列位置指定)
// int id 配列アドレス
//------------------------------------------
void SockBase::CloseID(int id)
{
	if( NULL == mycls_pKt[id]) return;

	if( -1 != id ) {
		mycls_pKc->CloseChild( mycls_pKt[id] );
		my_nClientAddr[id] = 0;
		mycls_pKt[id] = NULL;
	}
}

//------------------------------------------
// 全てのクライアントを切り離す
//------------------------------------------
void SockBase::CloseAll()
{
	for( DWORD ii=0; ii<my_nClientNum; ii++ ) CloseID(ii);
}

//------------------------------------------
// 現在の読み込みキャンセルする関数
//------------------------------------------
void SockBase::Cancel()
{
	for( DWORD ii=0; ii<my_nClientNum; ii++ ) {
		if( NULL == mycls_pKt[ii] ) continue;
		mycls_pKt[ii]->Cancel();
	}
}

//------------------------------------------
// 受信開始依頼 (ポインタ指定)
// LPVOID pData 受信バッファ
// DWORD len 受信サイズ
// ksThread* ksT ksThread のポインタ
// HANDLE evRecvEnd 受信完了イベント (NULL時はメッセージで渡される)
//------------------------------------------
int SockBase::Recv(LPVOID pData, DWORD len, UINT ksT, HANDLE evRecvEnd)
{
	return Recv(pData, len, (ksThread*)ksT, evRecvEnd);
}
int SockBase::Recv(LPVOID pData, DWORD len, ksThread* ksT, HANDLE evRecvEnd)
{
	int id = GetSockID(ksT);
	if( -1 == id ) return -9;

	return Recv(id, pData, len, evRecvEnd);
}

//------------------------------------------
// 受信開始依頼 (配列位置指定)
// int id 配列位置 (クライアント時は、0固定)
// LPVOID pData 受信バッファ
// DWORD len 受信サイズ
// HANDLE evRecvEnd 受信完了イベント (NULL時はメッセージで通知される)
//------------------------------------------
int SockBase::Recv(int id, LPVOID pData, DWORD len, HANDLE evRecvEnd)
{
	if( -1 == id ) return -9;				// 未割り当て
	if( NULL == mycls_pKt[id] ) return -1;	// 実は切断されていた

	// 受信開始依頼
	int iRet = mycls_pKt[id]->Recv( pData, len, evRecvEnd);
	
	// 受信開始依頼が失敗の為 切り離し
	if(0 != iRet) {
		mycls_pKc->CloseChild( mycls_pKt[id] );
		my_nClientAddr[id] = 0;
		mycls_pKt[id] = NULL;
	}
	return iRet;
}

//------------------------------------------
// 送信開始依頼 (ポインタ指定)
// LPCVOID pData 送信バッファ
// DWORD size 送信サイズ
// ksThread* ksT ksThread のポインタ
// HANDLE evSendEnd 受信完了イベント (NULL時はメッセージで通知される)
//------------------------------------------
int SockBase::Send(LPCVOID pData, DWORD len, UINT ksT, HANDLE evSendEnd)
{
	return Send(pData, len, (ksThread*)ksT, evSendEnd);
}
int SockBase::Send(LPCVOID pData, DWORD len, ksThread* ksT, HANDLE evSendEnd)
{
	int id = GetSockID(ksT);
	if( -1 == id ) return -9;

	return Send(id, pData, len, evSendEnd);
}
//------------------------------------------
// 送信開始依頼 (配列位置指定)
// int id 配列位置 (クライアント時は、0固定)
// LPCVOID senddata 送信バッファ
// DWORD size 送信サイズ
// HANDLE evSendEnd 送信完了イベント (NULL時はメッセージで通知される)
//------------------------------------------
int SockBase::Send(int id, LPCVOID pData, DWORD len, HANDLE evSendEnd)
{
	if( -1 == id ) return -9;				// 未割り当て
	if( NULL == mycls_pKt[id] ) return -1;	// 実は切断されていた

	// 送信開始依頼
	int iRet = mycls_pKt[id]->Send( pData, len, evSendEnd);
	
	// 送信開始依頼が失敗の為 切り離し
	if(0 != iRet) {
		mycls_pKc->CloseChild( mycls_pKt[id] );
		my_nClientAddr[id] = 0;
		mycls_pKt[id] = NULL;
	}
	return iRet;
}


