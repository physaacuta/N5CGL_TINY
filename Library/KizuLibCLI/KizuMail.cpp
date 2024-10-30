// これは メイン DLL ファイルです。

#include "stdafx.h"
#include "KizuMail.h"
#include <crtdbg.h>

using namespace System;
using namespace System::Runtime::InteropServices;

using namespace KizuLibCLI;

//------------------------------------------
// コストラクタ
//------------------------------------------
KizuMail::KizuMail() :
mcls_thread(nullptr),
m_evStop(nullptr)
{
}
//------------------------------------------
// デストラクタ
//------------------------------------------
KizuMail::~KizuMail(void)
{
	// 残骸が残らないように開放
	Task_Exit();
}


//------------------------------------------
// タスク生成
// String^ cTaskName 自タスク名称
// 戻り値 復帰情報
//------------------------------------------
int KizuMail::Task_Init(String^ cTaskName)
{
	IntPtr pTaskName = Marshal::StringToHGlobalAnsi(cTaskName);			// ansicに変換後の文字ポインタ
	int retc = task_init((LPCSTR)pTaskName.ToPointer());
	Marshal::FreeHGlobal(pTaskName);

	if( 0 == retc ) {
		// 読込スレッド生成
		m_evStop = gcnew Threading::AutoResetEvent(false); 
		mcls_thread = gcnew Threading::Thread(gcnew Threading::ParameterizedThreadStart(KizuMail::MainThread)); 
		mcls_thread->Start(this);		// 自ポインタを渡してあげる
	}
	return retc;
}

//------------------------------------------
// タスク生成
// String^ cTaskName 自タスク名称
// String^ cPcName 自PC名称
// 戻り値 復帰情報
//------------------------------------------
int KizuMail::Task_Init(String^ cTaskName, String^ cPcName)
{
	IntPtr pTaskName = Marshal::StringToHGlobalAnsi(cTaskName);			// ansicに変換後の文字ポインタ
	IntPtr pPcName = Marshal::StringToHGlobalAnsi(cPcName);			// ansicに変換後の文字ポインタ
	int retc = task_init((LPCSTR)pTaskName.ToPointer(), (LPCSTR)pPcName.ToPointer());
	Marshal::FreeHGlobal(pTaskName);
	Marshal::FreeHGlobal(pPcName);

	if( 0 == retc ) {
		// 読込スレッド生成
		m_evStop = gcnew Threading::AutoResetEvent(false); 
		mcls_thread = gcnew Threading::Thread(gcnew Threading::ParameterizedThreadStart(KizuMail::MainThread)); 
		mcls_thread->Start(this);		// 自ポインタを渡してあげる
	}
	return retc;
}

//------------------------------------------
// タスク終了
// 戻り値 復帰情報
//------------------------------------------
void KizuMail::Task_Exit()
{
	// スレッド停止
	if (nullptr != mcls_thread) {
		m_evStop->Set();
		mcls_thread = nullptr;
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
int KizuMail::Send_Mail(String^ SendSlotName, String^ SendPcName, Object^ pDate)
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
// クラス変換
// Object^ pTo コピー先 (new しておく必要有り。ただし、inisializeはしておく必要無し)
// Object^ pFrom コピー元
//------------------------------------------
void KizuMail::typConv(Object^ pTo, Object^ pFrom)
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
Object^ KizuMail::typConv(Object^ pFrom, Type^ structureType)
{
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
void KizuMail::MainThread(Object^ arg)
{
	KizuMail ^ p = (KizuMail ^) arg;			// 自インスタンス取得

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
	
	// 非同期受信開始
	retc = recv_mail_ovl( pRecvBuf.ToPointer(), &ovlRecv);
	if( !retc ) {
		_ASSERTE(GetLastError() == ERROR_IO_PENDING);
	}

	while (true) {
		DWORD dwWait = WaitForMultipleObjects(EV_END, evArray, FALSE, INFINITE);
		
		if( EV_STOP == dwWait ) {								// 終了
			Marshal::FreeHGlobal(pRecvBuf);
			return;

		} else if ( EV_OVL_RECV == dwWait ) {					// 非同期受信完了
			
			// とりあえず基本の型をこの形とする (使う方で勝手に型変換してね)
			COMMON_QUE_L^ que = gcnew COMMON_QUE_L();
			que = (COMMON_QUE_L^)Marshal::PtrToStructure( pRecvBuf, que->GetType() );

			// イベント発行 (ブロッキング)
			p->evRecvMail(que, que->nEventNo);
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