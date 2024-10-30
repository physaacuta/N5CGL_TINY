
#include "stdafx.h"
#include "KizuLib.h"

#include <crtdbg.h>
#include <stdio.h>

#include "Class\KizuMacro.h"												// マクロ集
#include "Class\MailSlotManager.h"											// メールスロットクラス
#include "Class\AutoLock.h"													// CRITICAL_SECTION のロック・アンロック クラス
#include "Class\KizuPcInfManager.h"											// PC管理
#include "Class\MailTransManager.h"											// メール転送クラス

//#include <odbcinst.h>														// ODBC自動設定用

using namespace KizuLib;

// ///////////////////////////////////////////////////////////////////
// グローバル変数
//
char PCNAME[MAX_PCCOUNT][SIZE_NAME_PC];										// KizuCommon.hにあるexertn宣言の実体

// ///////////////////////////////////////////////////////////////////
// スタティック メンバ変数
//
//#define KIZULIB_KIZULIB_SESSION				"PC"						// iniファイルセッション
static MailSlotManager*		mcls_pMail = NULL;								// メールスロットクラス
static KizuPcInfManager*	mcls_pPcInf = NULL;								// PC名称管理クラス

static int					g_nPCID = 0;									// 自PCID (1オリジン)
static int					g_nLineID = LINE_ID;							// 自ラインID (通常0固定。同一PCに複数ライン存在する場合に使用)
static char					g_cMyTaskName[SIZE_NAME_TASK];					// 自タスク名称


// ///////////////////////////////////////////////////////////////////
// メソッド
//
//------------------------------------------
// タスク生成
// LPCSTR cTaskName 自タスク名称
// 戻り値 復帰情報
//------------------------------------------
int	task_init(LPCSTR cTaskName) 
{
	// ini ファイルより取得
	int nPCID = GetPrivateProfileInt(INI_COMMON, "PCID", -99, PCINI_NAME);
	_ASSERT(-99 != nPCID);
	if(-99 == nPCID) return -99;

	return task_init(cTaskName, nPCID);
}

//------------------------------------------
// タスク生成
// LPCSTR cTaskName 自タスク名称
// int nPCID 自PCID (0:DB接続無し(PC名は.)、 -1:DB接続無し(PC名はコンピュータ名を取得)、 -9:DB接続あり(PC名はコンピュータ名を取得))
// 戻り値 復帰情報
//------------------------------------------
int task_init(LPCSTR cTaskName, int nPCID)
{
	// PC名称管理クラス作成
	if(NULL != mcls_pPcInf) return -9;
	mcls_pPcInf = new KizuPcInfManager(nPCID);
	getpcname();												// PC名称取得

	// 自タスクメールスロット作成
	if(NULL != mcls_pMail) return -9;
	mcls_pMail = new MailSlotManager();
	if( !mcls_pMail->Create(cTaskName) ) return -1;

	// スタティック変数保持
	g_nPCID = nPCID;
	strcpy(g_cMyTaskName, cTaskName);
	return 0;
}

//------------------------------------------
// タスク生成 (同一PC複数タスク実行用)
// LPCSTR cTaskName 自タスク名称 (サイズの関係上、16-4=12文字くらいにしておく事)
// int nLineID ラインID (通常は、getlineid() から取ってきた値)
// 戻り値 復帰情報
//------------------------------------------
int task_init_multi(LPCSTR cTaskName, int nLineID)
{
	// 複数タスク名称決定
	char cMultiTaskName[SIZE_NAME_TASK];
	memset(cMultiTaskName, 0x00, sizeof(cMultiTaskName));
	sprintf(cMultiTaskName, "%s_%d", cTaskName, nLineID );

	// タクス生成
	return task_init(cMultiTaskName);
}

//------------------------------------------
// タスク終了
// LPCSTR cTaskName タスク名称
// LPCSTR cPcName PC名称
// 戻り値 復帰情報
//------------------------------------------
void task_exit()
{
	SAFE_DELETE(mcls_pMail);
	SAFE_DELETE(mcls_pPcInf);
}

//------------------------------------------
// メール送信
// LPCSTR SendSlotName 送信メールスロット名称
// LPCSTR SendPcName 送信PC名称
// LPCVOID pDate 送信情報
// 戻り値 復帰情報
//------------------------------------------
int	send_mail(LPCSTR SendSlotName, LPCSTR SendPcName, LPCVOID pDate)
{
	return MailSlotManager::SendMail(SendSlotName, SendPcName, pDate, SIZE_MSL);
}
//------------------------------------------
// メール転送
// LPCSTR SendSlotName 送信メールスロット名称
// LPCSTR SendPcName 送信PC名称
// LPCVOID pDate 送信情報
// 戻り値 復帰情報
//------------------------------------------
int	send_mail_trans(LPCSTR SendSlotName, LPCSTR SendPcName, LPCVOID pDate)
{
	return MailTransManager::SendMailTrans(SendSlotName, SendPcName, pDate, SIZE_MSL);
}
//------------------------------------------
// メール転送
// LPCSTR SendUdpAddr 送信先IPアドレス
// LPCSTR SendSlotName 送信メールスロット名称
// LPCSTR SendPcName 送信PC名称
// LPCVOID pDate 送信情報
// 戻り値 復帰情報
//------------------------------------------
int	send_mail_trans(LPCSTR SendUdpAddr, LPCSTR SendSlotName, LPCSTR SendPcName, LPCVOID pDate)
{
	return MailTransManager::SendMailTrans(SendUdpAddr, SendSlotName, SendPcName, pDate, SIZE_MSL);
}

//------------------------------------------
// メール受信
// LPVOID pDate 受信情報
// 戻り値 復帰情報
//------------------------------------------
int	recv_mail(LPVOID pDate)
{
	return mcls_pMail->Recv(pDate, SIZE_MSL);
}
//------------------------------------------
// メール受信 (非同期)
// LPVOID pDate 受信情報
// OVERLAPPED* ovlRecv オーバーラップ
// 戻り値 復帰情報
//------------------------------------------
bool recv_mail_ovl(LPVOID pDate, OVERLAPPED* ovlRecv)
{
	return mcls_pMail->Recv(pDate, SIZE_MSL, ovlRecv);
}
//------------------------------------------
// 全PC名称をDBより取得
//------------------------------------------
bool getpcname()
{
	if(NULL == mcls_pPcInf) return false;
	
	return mcls_pPcInf->SetPcName(PCNAME[0], TASKINI_NAME);			// DBより PC名称を取得
}

//------------------------------------------
// 自PC名称取得
//------------------------------------------
char const* getmypcname()
{
	if(NULL == mcls_pPcInf) return NULL;
	
	return mcls_pPcInf->GetMyPcName();
}

//------------------------------------------
// システムログ出力
// long nSysNo メッセージNo
// LPCSTR __cBuffer 可変引数
// 戻り値 復帰情報
//------------------------------------------
int syslog(long nSysNo, LPCSTR __cBuffer, ... )
{
	char	cMsg[2048];
    SYSTEMTIME time;
    GetLocalTime(&time);
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));

	//// 可変引数より、メッセージ作成
	memset( cMsg, 0x00, sizeof(cMsg));
	va_list	vmarker;
	va_start( vmarker, __cBuffer ); 				// 可変個の引数の初期化
	//_vsnprintf( (char*)cMsg, _TRUNCATE, __cBuffer, vmarker );
	_vsnprintf( (char*)cMsg, sizeof(que.syslog_que.cMsg)-1, __cBuffer, vmarker );
	va_end( vmarker );								// 可変個の引数のリセット

	//// メッセージ生成
	que.nEventNo = FACT_KS_SYSLOG_01;												// イベントコード
	que.nLineNo = getlineid();														// ラインID
	que.syslog_que.nSysNo = nSysNo;													// メッセージNo
	que.syslog_que.nPcKind = NULL == mcls_pPcInf ? 0 : mcls_pPcInf->GetPcKind();	// PCKIND 
	que.syslog_que.date[0] = time.wYear;											// 自PC時刻
	que.syslog_que.date[1] = time.wMonth;
	que.syslog_que.date[2] = time.wDay;
	que.syslog_que.date[3] = time.wHour;
	que.syslog_que.date[4] = time.wMinute;
	que.syslog_que.date[5] = time.wSecond;
	strcpy( que.syslog_que.cTaskName, g_cMyTaskName);				// タスク名
	if(NULL != mcls_pPcInf) strcpy( que.syslog_que.cPcName, mcls_pPcInf->GetMyPcName() );	// PC名
	strcpy( que.syslog_que.cMsg, cMsg);				// メッセージ内容

	//// メール送信
	return send_mail(KS_SYSLOG, SYSLOG_PC_NAME, &que );
}

//------------------------------------------
// 機器状態変更ログ出力 (基本的には、ライブラリからのみしかコールしないはず)
// long nSysNo メッセージNo
// long nStateId 機器状態ID
// long nStateVal 機器状態値 (0:正常  1～:異常)
// LPCSTR __cBuffer 可変引数
// 戻り値 復帰情報
//------------------------------------------
int statelog(long nSysNo, long nStateId, long nStateVal, LPCSTR __cBuffer, ... )
{
	char	cMsg[2048];
    SYSTEMTIME time;
    GetLocalTime(&time);
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));

	//// 可変引数より、メッセージ作成
	memset( cMsg, 0x00, sizeof(cMsg));
	va_list	vmarker;
	va_start( vmarker, __cBuffer ); 				// 可変個の引数の初期化
	//_vsnprintf( (char*)cMsg, _TRUNCATE, __cBuffer, vmarker );
	_vsnprintf( (char*)cMsg, sizeof(que.statelog_que.cMsg)-1, __cBuffer, vmarker );
	va_end( vmarker );								// 可変個の引数のリセット

	//// メッセージ生成
	que.nEventNo = FACT_KS_SYSLOG_02;												// イベントコード
	que.nLineNo = getlineid();														// ラインID
	que.statelog_que.nSysNo = nSysNo;												// メッセージNo
	que.statelog_que.nPcKind = NULL == mcls_pPcInf ? 0 : mcls_pPcInf->GetPcKind();	// PCKIND 
	que.statelog_que.nStateId = nStateId;											// 機器状態ID
	que.statelog_que.nStateVal = nStateVal;											// 機器状態値
	que.statelog_que.date[0] = time.wYear;											// 自PC時刻
	que.statelog_que.date[1] = time.wMonth;
	que.statelog_que.date[2] = time.wDay;
	que.statelog_que.date[3] = time.wHour;
	que.statelog_que.date[4] = time.wMinute;
	que.statelog_que.date[5] = time.wSecond;
	strcpy( que.statelog_que.cTaskName, g_cMyTaskName);			// タスク名
	if(NULL != mcls_pPcInf) strcpy( que.statelog_que.cPcName, mcls_pPcInf->GetMyPcName() );	// PC名
	strcpy( que.statelog_que.cMsg, cMsg);			// メッセージ内容

	//// メール送信
	return send_mail(KS_SYSLOG, SYSLOG_PC_NAME, &que );
}

//------------------------------------------
// 共有メモリアクセス
// HANDLE *hMapFile ハンドル(out)
// LPVOID *pMapped マッピングアドレス (out)
// const char* name 接続共有メモリ名称 (in)
//------------------------------------------
void mem_connect(HANDLE *hMapFile, LPVOID *pMapped, const char* name)
{
	*hMapFile =OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, name);
	*pMapped = MapViewOfFile(*hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
}

//------------------------------------------
// 共有メモリアクセス
// HANDLE *hMapFile ハンドル(out)
// LPVOID *pMapped マッピングアドレス (out)		※実際に使う場合は *pWk = (char*)*pMapped+nAli; でずらす必要有り
// const char* name 接続共有メモリ名称 (in)
// DWORD offset	マッピング オフセット位置 (in)
// SIZE_T s マッピング サイズ (in)
// int	 nAddrAli	アライメントして移動させたサイズ。  -1 ずらした場合は、1がセットされる
//---------------
// ※	オフセットを使用する場合、オフセットはページサイズの倍数である必要がある
//		つまり、指定されたoffsetがページサイズの倍数でない場合は、
//		アドレスを戻して、アライメントを合わせる。
//		その戻し量を nAddrAli にセットする
//		注意として、戻した量分 サイズは増加させる
//------------------------------------------
void mem_connect(HANDLE *hMapFile, LPVOID *pMapped, const char* name, DWORD offset, SIZE_T s, int* nAddrAli)
{
	// ページサイズでアライメント
	SYSTEM_INFO si;
	GetSystemInfo(&si);			// ページサイズを取得
	int nAli = offset % si.dwAllocationGranularity;		// ずらし量
	int nAnsOffset = offset - nAli;
	int nAnsSize = s + nAli;


	*hMapFile =OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, name);
	*pMapped = MapViewOfFile(*hMapFile, FILE_MAP_ALL_ACCESS, 0, nAnsOffset, nAnsSize);

	//	*pMapped = (char*)*pMapped+nAli;	// テスト。これで構造体に直接アクセル可能。ただし、UnmapViewOfFile時にズラシちゃっているので怒られる
	
	*nAddrAli = nAli;
}

//------------------------------------------
// 共有メモリ使用終了
// HANDLE *hMapFile ハンドル(out)
// LPVOID *pMapped マッピングアドレス (out)
//------------------------------------------
void mem_close(HANDLE *hMapFile, LPVOID *pMapped)
{
	BOOL r = UnmapViewOfFile(*pMapped);
	CloseHandle(*hMapFile);
}

//------------------------------------------
// 自PCID (1オリジン)
//------------------------------------------
int getpcid()
{
	return g_nPCID;
}
//------------------------------------------
// 自ラインIDセット
//------------------------------------------
void setlineid()
{
	int nLineID = GetPrivateProfileInt(INI_COMMON, "LINEID", -1, TASKINI_NAME);
	_ASSERT(-1 != nLineID);
	g_nLineID = nLineID;
}
void setlineid(int nLineID)
{
	g_nLineID = nLineID;
}
//------------------------------------------
// 自ラインID取得
//------------------------------------------
int getlineid()
{
	return g_nLineID;
}
//------------------------------------------
// 自タスク名称
//------------------------------------------
char const* getmytaskname()
{
	return g_cMyTaskName;
}
//------------------------------------------
// KizuPcInfManagerクラス返却
//------------------------------------------
LPCVOID getcls_pcinfmgr()
{
	return (LPCVOID)mcls_pPcInf;
}
