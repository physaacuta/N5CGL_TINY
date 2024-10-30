// PipeManager.cpp: PipeManager クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PipeManager.h"

using namespace KizuLib;


//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// コンストラクタ
// LPCSTR PipeName パイプ名称
// bool bIsServer サーバー時:true  デフォルトは、クライアント:false
//------------------------------------------
PipeManager::PipeManager(LPCSTR PipeName, bool bIsServer) :
m_hPipe(INVALID_HANDLE_VALUE),
m_bIsServer(bIsServer),
m_nLastError(0)
{
	if(m_bIsServer) {
		//// パイプの準備 (接続待ちにはならいので注意。)
		// 接続は一人だけ
		// 出力、入力バッファは、256にしているが、関数内では参考程度にしか使ってないらしい
		m_hPipe = CreateNamedPipe(	PipeName, //NTCNTRL,
									PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
									PIPE_WAIT | PIPE_READMODE_MESSAGE | PIPE_TYPE_MESSAGE,
									1,	256, 256, 0, NULL);
		//ASSERT(INVALID_HANDLE_VALUE != m_hPipe);
		// パイプオープン失敗
		if (INVALID_HANDLE_VALUE == m_hPipe) throw 2; 

	} else {
		// パイプ接続 (サーバーに接続ね)
	//	m_hPipe = CreateFile((LPCTSTR)PipeName,
		m_hPipe = CreateFile(PipeName,
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
			NULL);

		// パイプオープン失敗
		if (INVALID_HANDLE_VALUE == m_hPipe) throw 1; 
		
		// パイプの設定
		// ※メッセージモードに設定、ブロッキングモードに設定
		DWORD mode = PIPE_READMODE_MESSAGE | PIPE_WAIT;
		SetNamedPipeHandleState(m_hPipe, &mode, NULL, NULL);
	}
}

//------------------------------------------
// デストラクタ
//------------------------------------------
PipeManager::~PipeManager()
{
	// パイプクローズ
	if(m_bIsServer) {
		DisconnectNamedPipe(m_hPipe);
	}
	CloseHandle(m_hPipe);	
	m_hPipe = INVALID_HANDLE_VALUE;
}

//------------------------------------------
// 未処理のI/Oを取り消す
//------------------------------------------
void PipeManager::Cancel() const
{
	CancelIo(m_hPipe);
}

//------------------------------------------
// クライアントからの接続を受け付ける。(非ブロック)
//------------------------------------------
DWORD PipeManager::Listen(OVERLAPPED *ovl) 
{
	// 最初の接続
	if ( ! ConnectNamedPipe(m_hPipe, ovl) ) {
		DWORD dwRetCode = GetLastError();
		if (ERROR_IO_PENDING == dwRetCode) return 0;
		//ASSERT(ERROR_IO_PENDING == dwRetCode || ERROR_PIPE_CONNECTED == dwRetCode);
		if (ERROR_PIPE_CONNECTED == dwRetCode) {
			// 接続済み(通常ありえない)
			return dwRetCode;		// 呼び出し元で確認するようにする
		}
		return dwRetCode;
	}
	return 0;
}

// ///////////////////////////////////////////////////////////////////
// ICommunityの実装
//
//------------------------------------------
// 受信
// LPVOID pDate 受信情報
// DWORD len 受信バイト数
// 戻り値 復帰情報
//------------------------------------------
int PipeManager::Recv(LPVOID pDate, DWORD len)
{
	DWORD nBytesOccupied = 0;
	BOOL bRet = ReadFile(m_hPipe, pDate, len, &nBytesOccupied, NULL);
	return (!bRet) ? GetLastError() : 0;
}

//------------------------------------------
// 送信
// LPCVOID pDate 送信情報
// DWORD len 送信バイト数
// 戻り値 復帰情報
//------------------------------------------
int PipeManager::Send(LPCVOID pDate, DWORD len)
{
	DWORD nBytesOccupied = 0;
	BOOL bRet = WriteFile(m_hPipe, pDate, len, &nBytesOccupied, NULL);
	return (!bRet) ? GetLastError() : 0;
}


// ///////////////////////////////////////////////////////////////////
// ICommunityAsyncの実装
//
//------------------------------------------
// 受信
// LPVOID pDate 受信情報
// DWORD len 受信バイト数
// OVERLAPPED *ovl 非同期操作の情報
// 戻り値 復帰情報
//------------------------------------------
bool PipeManager::Recv(LPVOID pDate, DWORD len, OVERLAPPED *ovl)
{
	BOOL bRet = ReadFile(m_hPipe, pDate, len, NULL, ovl);
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
bool PipeManager::Send(LPCVOID pDate, DWORD len, OVERLAPPED *ovl)
{
	BOOL bRet = WriteFile(m_hPipe, pDate, len, NULL, ovl);
	m_nLastError = GetLastError();	
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
bool PipeManager::GetResult(OVERLAPPED *ovl)
{
	DWORD dwTransByte;
	BOOL bRet = GetOverlappedResult(m_hPipe, ovl, &dwTransByte, TRUE);
	if( !bRet ) {
		m_nLastError = GetLastError();
		return false;
	}
	return true;
}