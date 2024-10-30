// これは メイン DLL ファイルです。

#include "stdafx.h"
#include "KizuMod.h"
#include <crtdbg.h>

#include "..\KizuLib\Class\KizuFunction.h"								// スタティック関数集

using namespace System;
using namespace System::Runtime::InteropServices;

using namespace KizuLib;
using namespace KizuLibCLI;

//------------------------------------------
// コストラクタ
//------------------------------------------
KizuMod::KizuMod() :
mcls_thread(nullptr),
m_evStop(nullptr)
{
}
//------------------------------------------
// デストラクタ
//------------------------------------------
KizuMod::~KizuMod(void)
{
	// 残骸が残らないように開放
	Task_Exit();
}

//------------------------------------------
// タスク生成
// String^ cTaskName 自タスク名称
// 戻り値 復帰情報
//------------------------------------------
int KizuMod::Task_Init(String^ cTaskName)
{
	IntPtr pTaskName = Marshal::StringToHGlobalAnsi(cTaskName);			// ansicに変換後の文字ポインタ
	int retc = task_init((LPCSTR)pTaskName.ToPointer());
	Marshal::FreeHGlobal(pTaskName);

	if( 0 == retc ) {
		// 読込スレッド生成
		m_evStop = gcnew Threading::AutoResetEvent(false); 
		mcls_thread = gcnew Threading::Thread(gcnew Threading::ParameterizedThreadStart(KizuMod::MainThread)); 
		mcls_thread->Start(this);		// 自ポインタを渡してあげる
	}
	return retc;
}

//------------------------------------------
// タスク生成
// String^ cTaskName 自タスク名称
// int nPCID 自PCID
// 戻り値 復帰情報
//------------------------------------------
int KizuMod::Task_Init(String^ cTaskName, int nPCID)
{
	IntPtr pTaskName = Marshal::StringToHGlobalAnsi(cTaskName);			// ansicに変換後の文字ポインタ
	int retc = task_init((LPCSTR)pTaskName.ToPointer(), nPCID);
	Marshal::FreeHGlobal(pTaskName);

	if( 0 == retc ) {
		// 読込スレッド生成
		m_evStop = gcnew Threading::AutoResetEvent(false);
		mcls_thread = gcnew Threading::Thread(gcnew Threading::ParameterizedThreadStart(KizuMod::MainThread)); 
		mcls_thread->Start(this);		// 自ポインタを渡してあげる
	}
	return retc;
}

//------------------------------------------
// タスク終了
// 戻り値 復帰情報
//------------------------------------------
void KizuMod::Task_Exit()
{
	// スレッド停止
	if (nullptr != mcls_thread) {
		m_evStop->Set();
		mcls_thread->Join();			// スレッドが終了するまで待機
		delete mcls_thread;
		mcls_thread = nullptr;
		delete m_evStop;
		m_evStop = nullptr;
	}

	// メールスロット開放
	task_exit();
}

//------------------------------------------
// メール送信
// String^ SendSlotName 送信先タスク名
// String^ SendPcName 送信先PC名称
// Object^ pDate 送信データのポインタ
// 戻り値 復帰情報
//------------------------------------------
int KizuMod::Send_Mail(String^ SendSlotName, String^ SendPcName, Object^ pDate)
{
	IntPtr pTaskName = Marshal::StringToHGlobalAnsi(SendSlotName);		// ansicに変換後の文字ポインタ
	IntPtr pPcName = Marshal::StringToHGlobalAnsi(SendPcName);		// ansicに変換後の文字ポインタ

	IntPtr p = Marshal::AllocHGlobal( SIZE_MSL );
	Marshal::StructureToPtr( pDate, p, true);
	int retc = send_mail((const char*)pTaskName.ToPointer(), (const char*)pPcName.ToPointer(), p.ToPointer() );

	Marshal::FreeHGlobal(pTaskName);
	Marshal::FreeHGlobal(pPcName);
	Marshal::FreeHGlobal(p);
	return retc;
}
/*
//------------------------------------------
// メール受信
// Object^% pDate 受信データのポインタ
// Type^ structureType 取り込む構造体
// 戻り値 復帰情報
//------------------------------------------
int KizuMail::Recv_Mail(Object^% pDate, Type^ structureType )
{
	IntPtr p = Marshal::AllocHGlobal( SIZE_MSL );

	int retc =  recv_mail(p.ToPointer());

	pDate = Marshal::PtrToStructure(p, structureType);        //メモリからデータを取得し、構造体へセット
	Marshal::FreeHGlobal(p);
	return retc;
}
*/
//------------------------------------------
// メール転送
// String^ SendSlotName 送信先タスク名
// String^ SendPcName 送信先PC名称
// Object^ pDate 送信データのポインタ
// 戻り値 復帰情報
//------------------------------------------
int KizuMod::Send_Mail_Trans(String^ SendSlotName, String^ SendPcName, Object^ pDate)
{
	IntPtr pTaskName = Marshal::StringToHGlobalAnsi(SendSlotName);		// ansicに変換後の文字ポインタ
	IntPtr pPcName = Marshal::StringToHGlobalAnsi(SendPcName);		// ansicに変換後の文字ポインタ

	IntPtr p = Marshal::AllocHGlobal( SIZE_MSL );
	Marshal::StructureToPtr( pDate, p, true);
	int retc = send_mail_trans((const char*)pTaskName.ToPointer(), (const char*)pPcName.ToPointer(), p.ToPointer() );

	Marshal::FreeHGlobal(pTaskName);
	Marshal::FreeHGlobal(pPcName);
	Marshal::FreeHGlobal(p);
	return retc;
}

//------------------------------------------
// メール転送
// String^ SendUdpAddr 送信先IPアドレス
// String^ SendSlotName 送信先タスク名
// String^ SendPcName 送信先PC名称
// Object^ pDate 送信データのポインタ
// 戻り値 復帰情報
//------------------------------------------
int KizuMod::Send_Mail_Trans(String^ SendUdpAddr, String^ SendSlotName, String^ SendPcName, Object^ pDate)
{
	IntPtr pUdpAddr = Marshal::StringToHGlobalAnsi(SendUdpAddr);		// ansicに変換後の文字ポインタ
	IntPtr pTaskName = Marshal::StringToHGlobalAnsi(SendSlotName);		// ansicに変換後の文字ポインタ
	IntPtr pPcName = Marshal::StringToHGlobalAnsi(SendPcName);		// ansicに変換後の文字ポインタ

	IntPtr p = Marshal::AllocHGlobal( SIZE_MSL );
	Marshal::StructureToPtr( pDate, p, true);
	int retc = send_mail_trans((const char*)pUdpAddr.ToPointer(), (const char*)pTaskName.ToPointer(), (const char*)pPcName.ToPointer(), p.ToPointer() );

	Marshal::FreeHGlobal(pUdpAddr);
	Marshal::FreeHGlobal(pTaskName);
	Marshal::FreeHGlobal(pPcName);
	Marshal::FreeHGlobal(p);
	return retc;
}

//------------------------------------------
// クラス変換
// Object^ pTo コピー先 (new しておく必要有り。ただし、inisializeはしておく必要無し)
// Object^ pFrom コピー元
//------------------------------------------
void KizuMod::typConv(Object^ pTo, Object^ pFrom)
{
	IntPtr p = Marshal::AllocHGlobal(Marshal::SizeOf(pFrom));
	Marshal::StructureToPtr(pFrom, p, true);					// メモリ上にデータをセット
	Marshal::PtrToStructure(p, pTo);							// メモリからデータを取得し、構造体へセット
	Marshal::FreeHGlobal(p);
}
//------------------------------------------
// 構造体変換
// Object^ pFrom コピー元
// 戻り値 構造体
//------------------------------------------
ValueType^ KizuMod::typConv(Object^ pFrom, Type^ structureType)
{
	// 戻り値で構造体を返すのには 以下の理由
	// ・ホントは引数で ValueType^% で受け渡したい ←トラッキングポインタね
	//   でもVB側で Option Strict On にしていると、Ctype(tbl, ValueType)とすると データが渡らない・・・
	//   Option Strict Off にすると 普通に 構造体の変数 tbl を引数にするだけでデータが渡る・・・
	//   Option Strict Off にはしたくないので、仕方なく 戻り値とする
	IntPtr p;
	try {
		p = Marshal::AllocHGlobal(Marshal::SizeOf(pFrom));
		Marshal::StructureToPtr(pFrom, p, true);                   // メモリ上にデータをセット
		return (ValueType^)Marshal::PtrToStructure(p, structureType);			// メモリからデータを取得し、構造体へセット
	} catch( char * ) {
		return nullptr;
	} finally {
		Marshal::FreeHGlobal(p);
	}
}
//------------------------------------------
// 受信用スレッド
// Object^ arg 自インスタンス
// 戻り値 復帰情報
//------------------------------------------
void KizuMod::MainThread(Object^ arg)
{
	KizuMod ^ p = (KizuMod ^) arg;			// 自インスタンス取得

	// 非同期受信用
	int retc;
	OVERLAPPED ovlRecv;
	memset(&ovlRecv, 0, sizeof(OVERLAPPED));
	ovlRecv.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);		// manual reset

	// イベント作成
	HANDLE evArray[2] = {p->m_evStop->Handle.ToPointer(), ovlRecv.hEvent}; 
	enum { EV_STOP = WAIT_OBJECT_0,			// 終了通知
		   EV_OVL_RECV,						// 非同期受信完了
		   EV_END};

	// 受信用領域
	IntPtr pRecvBuf = Marshal::AllocHGlobal( SIZE_MSL );
	COMMON_QUE_L^ que = gcnew COMMON_QUE_L();

	// 非同期受信開始
	retc = recv_mail_ovl( pRecvBuf.ToPointer(), &ovlRecv);
	if( !retc ) {
		_ASSERTE(GetLastError() == ERROR_IO_PENDING);
	}

	while (true) {
		DWORD dwWait = WaitForMultipleObjects(EV_END, evArray, FALSE, INFINITE);
		
		if( EV_STOP == dwWait ) {								// 終了
			Marshal::FreeHGlobal(pRecvBuf);
			delete que;		// バッファ使い終わったから開放
			return;

		} else if ( EV_OVL_RECV == dwWait ) {					// 非同期受信完了
			
			// 毎回 gcnewしてするのはヤメ
			// とりあえず基本の型をこの形とする (使う方で勝手に型変換してね)
			//COMMON_QUE_L^ que = gcnew COMMON_QUE_L();
			que = (COMMON_QUE_L^)Marshal::PtrToStructure( pRecvBuf, que->GetType() );

			// イベント発行 (ブロッキング)
			p->evRecvMail(que, que->nEventNo);
			//delete que;		// バッファ使い終わったから開放

			// 次非同期受信開始
			retc = recv_mail_ovl( pRecvBuf.ToPointer(), &ovlRecv);
			if( !retc ) {
				//_ASSERTE(GetLastError() == ERROR_IO_PENDING);
				continue;
			}
		} else {
			// ありえない
		}
	}
}




/* 
//------------------------------------------
// PC名称取得
// 戻り値 復帰情報
//------------------------------------------
bool KizuMod::GetPcName()
{
	return getpcname();
}
*/

//------------------------------------------
// システムログ出力
// long nSysNo メッセージNo
//  LPCSTR __cBuffer 可変引数
// 戻り値 復帰情報
//------------------------------------------
int KizuMod::SysLog(int nSysNo, String^ cMsg)
{
	IntPtr p = Marshal::StringToHGlobalAnsi(cMsg);			// ansicに変換後の文字ポインタ
	int retc = syslog(nSysNo, (LPCSTR)p.ToPointer());
	Marshal::FreeHGlobal(p);
	return retc;
}

//------------------------------------------
// DB_SYSTEM_STATEに機器状態をセットする
// int id 機器ID
// int state 機器状態 (0:正常  1～:異常)
// 戻り値 復帰情報
//------------------------------------------
void KizuMod::SetStatus(int id, int state)
{
	KizuFunction::SetStatus(id, state, true); 
	return;
}
void KizuMod::SetStatus(int id, bool state)
{
	KizuFunction::SetStatus(id, state, true); 
	return;
}
// 規定引数の指定がC++/CLIでは出来ない
void KizuMod::SetStatus(int id, int state, bool bLogDsp)
{
	KizuFunction::SetStatus(id, state, bLogDsp); 
	return;
}
void KizuMod::SetStatus(int id, bool state, bool bLogDsp)
{
	KizuFunction::SetStatus(id, state, bLogDsp); 
	return;
}

//------------------------------------------
// 共有メモリ生成
// String^ cTblName 生成テーブル名
// int size 生成サイズ
// 戻り値 復帰情報
//------------------------------------------
bool KizuMod::ksMemCreate(String^ cTblName, int size)
{
	HANDLE hMapFile;

	IntPtr pname = Marshal::StringToHGlobalAnsi(cTblName);						// ansicに変換後の文字ポインタ
	hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, (LPCSTR)pname.ToPointer());
	Marshal::FreeHGlobal(pname);

	if(NULL == hMapFile) return false;
	return true;
}

//------------------------------------------
// 共有メモリ開放
// String^ cTblName 生成テーブル名
// 戻り値 復帰情報
//------------------------------------------
void KizuMod::ksMemClose(String^ cTblName)
{
	HANDLE hMapFile;

	IntPtr pname = Marshal::StringToHGlobalAnsi(cTblName);						// ansicに変換後の文字ポインタ
	hMapFile =OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, (LPCSTR)pname.ToPointer());
	Marshal::FreeHGlobal(pname);

	if(NULL != hMapFile) CloseHandle(hMapFile);
	return;
}

//------------------------------------------
// 共有メモリ上のデータ取得
// String^ cTblName 接続するテーブル名
// Type^ structureType 構造体の型 GetType(COMMON_QUE) とかでもOK
// 戻り値 読込構造体
//------------------------------------------
ValueType^ KizuMod::ksMemRead(String^ cTblName, Type^ structureType)
{
	HANDLE hMapFile;
	IntPtr iMap;
	IntPtr pname = Marshal::StringToHGlobalAnsi(cTblName);						// ansicに変換後の文字ポインタ
	mem_connect( &hMapFile, (LPVOID*)&iMap, (LPCSTR)pname.ToPointer() );		// 共有メモリにアクセス
	
	try {
		if( iMap != IntPtr::Zero) {
			return (ValueType^)Marshal::PtrToStructure(iMap, structureType);	// データをマーネジドにコピー
		} else {
			//_ASSERTE(false);
			return nullptr;
		}
	} catch( char * ) {
		return nullptr;
	} finally {
		Marshal::FreeHGlobal(pname);
		mem_close( &hMapFile, (LPVOID*)&iMap );									// 共有メモリ開放
	}
}

//------------------------------------------
// 共有メモリ上のデータ取得
// String^ cTblName 接続するテーブル名
// int startIndex   コピー開始アドレス
// Type^ structureType 構造体の型 GetType(COMMON_QUE) とかでもOK
// int^ nRetc 復帰情報 (1:正常 0:異常)
// 戻り値 読込構造体
//------------------------------------------
ValueType^ KizuMod::ksMemRead(String^ cTblName, int startIndex, Type^ structureType, int^% nRetc)
{
	HANDLE hMapFile;
	IntPtr iMap;
	IntPtr pname = Marshal::StringToHGlobalAnsi(cTblName);						// ansicに変換後の文字ポインタ
	mem_connect( &hMapFile, (LPVOID*)&iMap, (LPCSTR)pname.ToPointer() );		// 共有メモリにアクセス
	
	nRetc = 1;
	try {
		if( iMap != IntPtr::Zero) {
			return (ValueType^)Marshal::PtrToStructure((iMap + startIndex), structureType);	// データをマーネジドにコピー
		} else {
			//_ASSERTE(false);
			nRetc = 0;
			return nullptr;
		}
	} catch( char * ) {
		nRetc = 0;
		return nullptr;
	} finally {
		Marshal::FreeHGlobal(pname);
		mem_close( &hMapFile, (LPVOID*)&iMap );									// 共有メモリ開放
	}
}

//------------------------------------------
// 共有メモリ上のデータ取得
// String^ cTblName 接続するテーブル名
// int startIndex   コピー開始アドレス
// ValueType^% p	取得する構造体
// 戻り値 復帰情報
//------------------------------------------
bool KizuMod::ksMemRead(String^ cTblName, int startIndex, ValueType^% p)
{
	HANDLE hMapFile;
	IntPtr iMap;
	IntPtr pname = Marshal::StringToHGlobalAnsi(cTblName);						// ansicに変換後の文字ポインタ
	mem_connect( &hMapFile, (LPVOID*)&iMap, (LPCSTR)pname.ToPointer() );		// 共有メモリにアクセス
	
	try {
		if( iMap != IntPtr::Zero) {
			p = (ValueType^)Marshal::PtrToStructure((iMap + startIndex), p->GetType());	// データをマーネジドにコピー
			return true;
		} else {
			//_ASSERTE(false);
			return false;
		}
	} catch( char * ) {
		return false;
	} finally {
		Marshal::FreeHGlobal(pname);
		mem_close( &hMapFile, (LPVOID*)&iMap );									// 共有メモリ開放
	}
}

//------------------------------------------
// 共有メモリ上のデータ取得
// String^ cTblName 接続するテーブル名
// int size 共有メモリサイズ
// array<unsigned char>^ byteVal 読込バイト配列
// 戻り値 復帰情報
//------------------------------------------
bool KizuMod::ksMemRead(String^ cTblName, int size, array<unsigned char>^ byteVal)
{
	HANDLE hMapFile;
	IntPtr iMap;
	IntPtr pname = Marshal::StringToHGlobalAnsi(cTblName);						// ansicに変換後の文字ポインタ
	mem_connect( &hMapFile, (LPVOID*)&iMap, (LPCSTR)pname.ToPointer() );		// 共有メモリにアクセス
	
	try {
		if( iMap != IntPtr::Zero) {
			Marshal::Copy(iMap, byteVal, 0, size );	// データをマーネジドにコピー
			return true;
		} else {
			//_ASSERTE(false);
			return false;
		}
	} catch( char * ) {
		return false;
	} finally {
		Marshal::FreeHGlobal(pname);
		mem_close( &hMapFile, (LPVOID*)&iMap );									// 共有メモリ開放
	}
}

//------------------------------------------
// 共有メモリ上のデータ取得
// String^ cTblName 接続するテーブル名
// int startIndex   コピー開始アドレス
// int size 　　　　共有メモリのサイズ
// array<unsigned char>^ byteVal 読込バイト配列
// 戻り値 復帰情報
//------------------------------------------
bool KizuMod::ksMemRead(String^ cTblName, int startIndex, int size, array<unsigned char>^ byteVal)
{
	
	HANDLE hMapFile;
	IntPtr iMap;
	IntPtr pname = Marshal::StringToHGlobalAnsi(cTblName);						// ansicに変換後の文字ポインタ
	mem_connect( &hMapFile, (LPVOID*)&iMap, (LPCSTR)pname.ToPointer() );		// 共有メモリにアクセス
	
	try {
		if( iMap != IntPtr::Zero) {
			Marshal::Copy((iMap + startIndex), byteVal, 0, size );	// データをマーネジドにコピー
			return true;
		} else {
			//_ASSERTE(false);
			return false;
		}
	} catch( char * ) {
		return false;
	} finally {
		Marshal::FreeHGlobal(pname);
		mem_close( &hMapFile, (LPVOID*)&iMap );									// 共有メモリ開放
	}
}

//------------------------------------------
// 共有メモリ上のデータ取得
// String^ cTblName 接続するテーブル名
// int startIndex   コピー開始アドレス
// int size 　　　　共有メモリのサイズ
// Type^ structureType 構造体の型 GetType(COMMON_QUE) とかでもOK
// int^ nRetc 復帰情報 (1:正常 0:異常)
// 戻り値 読込構造体
//------------------------------------------
ValueType^ KizuMod::ksMemReadMap(String^ cTblName, int startIndex, int size, Type^ structureType, int^% nRetc)
{
	HANDLE hMapFile;
	IntPtr iMap;											// 元
	IntPtr iAnsMap;											// 最終的に使いたい位置
	int nAli = 0;											// ページサイズにアライメントしたオフセット量 
	IntPtr pname = Marshal::StringToHGlobalAnsi(cTblName);						// ansicに変換後の文字ポインタ
	mem_connect( &hMapFile, (LPVOID*)&iMap, (LPCSTR)pname.ToPointer(), startIndex, size, &nAli );		// 共有メモリにアクセス
	
	nRetc = 1;
	try {
		if( iMap != IntPtr::Zero ) {
			//iAnsMap = iMap + nAli;
			iAnsMap = IntPtr( iMap.ToInt64() + nAli);
			

			return (ValueType^)Marshal::PtrToStructure(iAnsMap, structureType);	// データをマーネジドにコピー
		} else {
			//_ASSERTE(false);
			nRetc = 0;
			return nullptr;
		}
	} catch( char * ) {
		nRetc = 0;
		return nullptr;
	} finally {
		Marshal::FreeHGlobal(pname);
		mem_close( &hMapFile, (LPVOID*)&iMap );									// 共有メモリ開放
	}
}

//------------------------------------------
// 共有メモリ上のデータ取得
// String^ cTblName 接続するテーブル名
// int startIndex   コピー開始アドレス
// int size 　　　　共有メモリのサイズ
// ValueType^% p	取得する構造体
// 戻り値 復帰情報
//------------------------------------------
bool KizuMod::ksMemReadMap(String^ cTblName, int startIndex, int size, ValueType^% p)
{
	HANDLE hMapFile;
	IntPtr iMap;											// 元
	IntPtr iAnsMap;											// 最終的に使いたい位置
	int nAli = 0;											// ページサイズにアライメントしたオフセット量 
	IntPtr pname = Marshal::StringToHGlobalAnsi(cTblName);						// ansicに変換後の文字ポインタ
	mem_connect( &hMapFile, (LPVOID*)&iMap, (LPCSTR)pname.ToPointer(), startIndex, size, &nAli );		// 共有メモリにアクセス
	
	try {
		if( iMap != IntPtr::Zero ) {
			//iAnsMap = iMap + nAli;
			iAnsMap = IntPtr( iMap.ToInt64() + nAli);
			p = (ValueType^)Marshal::PtrToStructure(iAnsMap, p->GetType());	// データをマーネジドにコピー
			return true;
		} else {
			//_ASSERTE(false);
			return false;
		}
	} catch( char * ) {
		return false;
	} finally {
		Marshal::FreeHGlobal(pname);
		mem_close( &hMapFile, (LPVOID*)&iMap );									// 共有メモリ開放
	}
}

/*
//------------------------------------------
// 共有メモリ上のデータ取得
// String^ cTblName 接続するテーブル名
// int startIndex   コピー開始アドレス
// int size 　　　　共有メモリのサイズ
// IntPtr p			読込みポインタ		(GCHandle.Alloc  した後に、 GCHandle.AddrOfPinnedObject で アドレスを固定しておくこと)
// 戻り値 復帰情報	
//------------------------------------------
bool KizuMod::ksMemRead(String^ cTblName, int startIndex, int size, IntPtr p)
{
	
	HANDLE hMapFile;
	int iMap;
	IntPtr pname = Marshal::StringToHGlobalAnsi(cTblName);						// ansicに変換後の文字ポインタ
	mem_connect( &hMapFile, (LPVOID*)&iMap, (LPCSTR)pname.ToPointer() );		// 共有メモリにアクセス
	
	try {
		if( iMap != 0 ) {
			memcpy( p.ToPointer(), (LPVOID*)(iMap + startIndex), size);
			//Marshal::Copy((IntPtr)(iMap + startIndex), byteVal, 0, size );	// データをマーネジドにコピー
			return true;
		} else {
			//_ASSERTE(false);
			return false;
		}
	} catch( char * ) {
		return false;
	} finally {
		Marshal::FreeHGlobal(pname);
		mem_close( &hMapFile, (LPVOID*)&iMap );									// 共有メモリ開放
	}
}
*/

//------------------------------------------
// 共有メモリ上のデータ書込み
// String^ cTblName 接続するテーブル名
// ValueType^ tbl 書込みデータ
// 戻り値 復帰情報
//------------------------------------------
bool KizuMod::ksMemWrite(String^ cTblName, ValueType^ tbl)
{
	HANDLE hMapFile;
	IntPtr iMap;
	IntPtr pname = Marshal::StringToHGlobalAnsi(cTblName);						// ansicに変換後の文字ポインタ
	mem_connect( &hMapFile, (LPVOID*)&iMap, (LPCSTR)pname.ToPointer() );		// 共有メモリにアクセス
	
	try {
		if( iMap != IntPtr::Zero) {
			Marshal::StructureToPtr(tbl, iMap, true);					// メモリ上にデータをセット (用は共有メモリに書込み)
			return true;
		} else {
			return false;
		}
	} catch( char * ) {
		return false;
	} finally {
		Marshal::FreeHGlobal(pname);
		mem_close( &hMapFile, (LPVOID*)&iMap );									// 共有メモリ開放
	}
}

//------------------------------------------
// 共有メモリ上のデータ書込み
// String^ cTblName 接続するテーブル名
// int startIndex   コピー開始アドレス
// int size 　　　　共有メモリのサイズ
// ValueType^ tbl 書込みデータ
// 戻り値 復帰情報
//------------------------------------------
bool KizuMod::ksMemWriteMap(String^ cTblName, int startIndex, int size, ValueType^ tbl)
{
	HANDLE hMapFile;
	IntPtr iMap;											// 元
	IntPtr iAnsMap;											// 最終的に使いたい位置
	int nAli = 0;											// ページサイズにアライメントしたオフセット量 
	IntPtr pname = Marshal::StringToHGlobalAnsi(cTblName);						// ansicに変換後の文字ポインタ
	mem_connect( &hMapFile, (LPVOID*)&iMap, (LPCSTR)pname.ToPointer(), startIndex, size, &nAli );		// 共有メモリにアクセス
	
	try {
		if( iMap != IntPtr::Zero ) {
			iAnsMap = IntPtr( iMap.ToInt64() + nAli);
			Marshal::StructureToPtr(tbl, iAnsMap, true);					// メモリ上にデータをセット (用は共有メモリに書込み)
			return true;
		} else {
			return false;
		}
	} catch( char * ) {
		return false;
	} finally {
		Marshal::FreeHGlobal(pname);
		mem_close( &hMapFile, (LPVOID*)&iMap );									// 共有メモリ開放
	}
}