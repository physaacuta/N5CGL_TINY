// MailSlotManager.cpp: MailSlotManager クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MailSlotManager.h"

#include <stdio.h>

using namespace KizuLib;


//------------------------------------------
//// static 変数
//CRITICAL_SECTION	MailSlotManager::m_csLock;	


//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// コンストラクタ
//------------------------------------------
MailSlotManager::MailSlotManager() :
m_hSlot(INVALID_HANDLE_VALUE),
m_nLastError(0)
{
//	InitializeCriticalSection(&m_csLock);								// クリティカルセクションオブジェクトを初期化

}
//------------------------------------------
// デストラクタ
//------------------------------------------
MailSlotManager::~MailSlotManager()
{
	// メールスロット破棄
	if(INVALID_HANDLE_VALUE != m_hSlot)	CloseHandle(m_hSlot);

//	DeleteCriticalSection(&m_csLock);									// クリティカルセクションオブジェクトを開放
}

// ///////////////////////////////////////////////////////////////////
// syslog等々でNewしてない時に使用するよう
//
//------------------------------------------
// 送信
// LPCSTR SendSlotName 送信メールスロット名称
// LPCSTR SendPcName 送信PC名称
// LPCVOID pDate 送信情報
// DWORD len 送信バイト数 (MAX400byte)
// 戻り値 復帰情報
//------------------------------------------
int MailSlotManager::SendMail(LPCSTR SendSlotName, LPCSTR SendPcName, LPCVOID pDate, DWORD len)
{
	// どうやってもVBでエラーになる。
//	AutoLock autolock(&m_csLock);		// AutoLockを使ったら、dllサイズが小さくなって、VBで読んだら領域破壊がでるようになった。なぜ？原因全く不明
//	EnterCriticalSection(&m_csLock);	// めんどくさいけども手作業


	HANDLE hSend = NULL;
	// メールスロットに接続
	int api_err = Connect(SendSlotName, SendPcName, &hSend);
	if( 0 != api_err ) {
//		LeaveCriticalSection(&m_csLock);
		return api_err;
	}

	// メール送信
	DWORD nBytesOccupied = 0;
	BOOL bRet = WriteFile(hSend, pDate, len, &nBytesOccupied, NULL);

//	LeaveCriticalSection(&m_csLock);	// なぜか、各所の入れる場所で、サイズが変わる。もうわけがわからない


	if (!bRet) {
		Close(&hSend);
		return GetLastError();
	}
	// 切断
	Close(&hSend);
	return 0;
}

//------------------------------------------
// 送信スロット接続
// LPCSTR SendSlotName 送信メールスロット名称
// LPCSTR SendPcName 送信PC名称
// HANDLE* hSend メールスロットハンドル
// 戻り値 復帰情報
//------------------------------------------
int MailSlotManager::Connect(LPCSTR SendSlotName, LPCSTR SendPcName, HANDLE* hSend)
{
	//CString fullname;
	//fullname.Format("\\\\%s\\mailslot\\%s", SendPcName, SendSlotName);
	char fullname[MAX_PATH];
	sprintf(fullname, "\\\\%s\\mailslot\\%s", SendPcName, SendSlotName);
	// メールスロットに接続
	*hSend = CreateFile(fullname, 
						GENERIC_WRITE, 
						FILE_SHARE_WRITE | FILE_SHARE_READ,
						NULL, 
						OPEN_EXISTING, 
						FILE_ATTRIBUTE_NORMAL,	
						NULL);
	return (*hSend == INVALID_HANDLE_VALUE) ? GetLastError() : 0;
}

//------------------------------------------
// スロット切断
// HANDLE* hSend メールスロットハンドル
//------------------------------------------
void MailSlotManager::Close(HANDLE* hSend)
{
	if( *hSend != INVALID_HANDLE_VALUE ) CloseHandle(*hSend);
}

//------------------------------------------
// スロット構築
// LPCSTR SlotName メールスロット名称
// DWORD lReadTimeout タイムアウト [ms]
// 戻り値 復帰情報
//------------------------------------------
bool MailSlotManager::Create(LPCSTR SlotName, DWORD lReadTimeout)
{
	//CString fullname;
	//fullname.Format("\\\\.\\mailslot\\%s", SlotName);
	char fullname[MAX_PATH];
	sprintf(fullname, "\\\\.\\mailslot\\%s", SlotName);
	// メールスロット作成
	m_hSlot = CreateMailslot(fullname, 
		0,							// メッセージサイズ
		lReadTimeout,				// タイムアウト [ms]
		NULL);

	// メールスロット作成失敗
	return (INVALID_HANDLE_VALUE == m_hSlot) ? false : true; 
}

// ///////////////////////////////////////////////////////////////////
// ICommunityの実装
//
// 20170321 ﾒｰﾙｽﾛｯﾄｻｲｽﾞが異なるﾀｽｸ間通信の対応 --->
//------------------------------------------
// 受信
// LPVOID pDate 受信情報
// DWORD len 受信バイト数 (MAX400byte)
// 戻り値 復帰情報
//------------------------------------------
int MailSlotManager::Recv(LPVOID pDate, DWORD len)
{
	char cWk[1024] = "";
	DWORD nBytesOccupied = 0;
	// メール受信（ｻｲｽﾞ1024 Byteで読み込み）
	BOOL bRet = ReadFile(m_hSlot, cWk, sizeof(cWk), &nBytesOccupied, NULL);
	int nRetCode = ( !bRet ) ? GetLastError() : 0;

	memcpy( pDate, cWk, min( len, nBytesOccupied) );
	if ( len < nBytesOccupied ) {
		return ERROR_MORE_DATA;
	}

	return nRetCode;
}
// 20170321 ﾒｰﾙｽﾛｯﾄｻｲｽﾞが異なるﾀｽｸ間通信の対応 <---

//------------------------------------------
// 送信
// LPCVOID pDate 送信情報
// DWORD len 送信バイト数 (MAX400byte)
// 戻り値 復帰情報
//------------------------------------------
int MailSlotManager::Send(LPCVOID pDate, DWORD len)
{
//	AutoLock autolock(&m_csLock);
//	EnterCriticalSection(&m_csLock);

	// メール送信
	DWORD nBytesOccupied = 0;
	BOOL bRet = WriteFile(m_hSlot, pDate, len, &nBytesOccupied, NULL);

//	LeaveCriticalSection(&m_csLock);

	return (!bRet) ? GetLastError() : 0;
}


// ///////////////////////////////////////////////////////////////////
// ICommunityAsyncの実装
//
//------------------------------------------
// 受信
// LPVOID pDate 受信情報
// DWORD len 受信バイト数 (MAX400byte)
// OVERLAPPED *ovl 非同期操作の情報
// 戻り値 復帰情報
//------------------------------------------
bool MailSlotManager::Recv(LPVOID pDate, DWORD len, OVERLAPPED *ovl)
{
	BOOL bRet = ReadFile(m_hSlot, pDate, len, NULL, ovl);
	m_nLastError = GetLastError();	
	if( !bRet ) {		// 即失敗の場合
		if (m_nLastError != ERROR_IO_PENDING) return false;
	}
	return true;
}

//------------------------------------------
// 送信
// LPCVOID pDate 送信情報
// DWORD len 送信バイト数 (MAX400byte)
// OVERLAPPED *ovl 非同期操作の情報
// 戻り値 復帰情報
//------------------------------------------
bool MailSlotManager::Send(LPCVOID pDate, DWORD len, OVERLAPPED *ovl)
{
//	AutoLock autolock(&m_csLock);
//	EnterCriticalSection(&m_csLock);


	// メール送信
	BOOL bRet = WriteFile(m_hSlot, pDate, len, NULL, ovl);
	m_nLastError = GetLastError();	

//	LeaveCriticalSection(&m_csLock);
	if( !bRet ) {		// 即失敗の場合
		if (m_nLastError != ERROR_IO_PENDING) return false;
	}
	return true;
}

//------------------------------------------
// 非同期結果問い合わせ
// OVERLAPPED *ovl 非同期操作の情報
// 戻り値 復帰情報
//------------------------------------------
bool MailSlotManager::GetResult(OVERLAPPED *ovl)
{
	DWORD dwTransByte;
	BOOL bRet = GetOverlappedResult(m_hSlot, ovl, &dwTransByte, TRUE);
	if( !bRet ) {
		m_nLastError = GetLastError();
		return false;
	}
	return true;
}
