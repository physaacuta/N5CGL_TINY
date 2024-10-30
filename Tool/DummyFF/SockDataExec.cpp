#include "StdAfx.h"
#include "SockDataExec.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// コンストラクタ
//------------------------------------------
SockDataExec::SockDataExec(void):
FuncBase("SockDataExec"),
m_bDumpMode(false),
m_bReadDataMode(true)
{
}
//------------------------------------------
// デストラクタ
//------------------------------------------
SockDataExec::~SockDataExec(void)
{
}

//------------------------------------------
// 送信データ取得
// Delivery* deli 送信要求
// ExecItem* item 送受信データ
//------------------------------------------
bool SockDataExec::GetSendData(Delivery* deli, ExecItem* item)
{
	CString	sql;
	CString	cWk;
	SYSTEMTIME STime;
	GetLocalTime(&STime);

	//// セットエリア初期化
	memset(item, 0x00, sizeof(ExecItem));

	//// 各伝文生成
	switch(deli->eStat) {

//-----------------------------------------------------------------------------------------------
	case QUE_SEND_ZISEKI_T:								// 疵実績伝文
	case QUE_SEND_ZISEKI_B:
	{
			//// その他固定 データセット
			// 伝文種類
			item->emStat = deli->eStat;
			// 送信伝文サイズ (検査実績伝文サイズをセット)
			item->nSendSize = sizeof(FF_ANS);
			// 命令No
			memcpy(item->cMeiNo, deli->cMeiNo, sizeof(item->cMeiNo));
			// コイルNo
			memcpy(item->cCoilNo, deli->cCoilNo, sizeof(item->cCoilNo));

			// 伝文コード
			FF_ANS* pBuf;
			pBuf = (FF_ANS*)&item->cSendBuf;
			if (QUE_SEND_ZISEKI_T == deli->eStat) {
				memcpy(pBuf->Hd.Header_Code, FF_TC_CODE_ANS_O, sizeof(pBuf->Hd.Header_Code));
			}
			else {
				memcpy(pBuf->Hd.Header_Code, FF_TC_CODE_ANS_U, sizeof(pBuf->Hd.Header_Code));

			}
			// 伝文発生時刻
			cWk.Format("%04d%02d%02d%02d%02d%02d", STime.wYear, STime.wMonth, STime.wDay, STime.wHour, STime.wMinute, STime.wSecond);
			memcpy(pBuf->Hd.Date, cWk, sizeof(pBuf->Hd.Date));

			// 連番
			memcpy(pBuf->Hd.Rec_Code, "00", sizeof(pBuf->Hd.Rec_Code));

			// 命令No
			memcpy(pBuf->Hd.Meino, deli->cMeiNo, sizeof(pBuf->Hd.Meino));

			// コイルNo
			memcpy(pBuf->Hd.Coilno, deli->cCoilNo, sizeof(pBuf->Hd.Coilno));

			// ステータス
			 memcpy(pBuf->Data.Status, "00", sizeof(pBuf->Data.Status));
	}
	break;

//-----------------------------------------------------------------------------------------------
	case QUE_SEND_STAT_T:								// 検査状態応答要求
	case QUE_SEND_STAT_B:
	{
		//// その他固定 データセット
		// 伝文種類
		item->emStat = deli->eStat;
		// 送信伝文サイズ (検査実績伝文サイズをセット)
		item->nSendSize = sizeof(FF_STSANS);
		// 命令No
		memcpy(item->cMeiNo, deli->cMeiNo, sizeof(item->cMeiNo));
		// コイルNo
		memcpy(item->cCoilNo, deli->cCoilNo, sizeof(item->cCoilNo));

		// 伝文コード
		FF_STSANS* pBuf;
		pBuf = (FF_STSANS*)&item->cSendBuf;
		if (QUE_SEND_STAT_T == deli->eStat) {
			memcpy(pBuf->Hd.Header_Code, FF_TC_CODE_STSANS_O, sizeof(pBuf->Hd.Header_Code));
		}
		else {
			memcpy(pBuf->Hd.Header_Code, FF_TC_CODE_STSANS_U, sizeof(pBuf->Hd.Header_Code));

		}
		// 伝文発生時刻
		cWk.Format("%04d%02d%02d%02d%02d%02d", STime.wYear, STime.wMonth, STime.wDay, STime.wHour, STime.wMinute, STime.wSecond);
		memcpy(pBuf->Hd.Date, cWk, sizeof(pBuf->Hd.Date));

		// 連番
		memcpy(pBuf->Hd.Rec_Code, "00", sizeof(pBuf->Hd.Rec_Code));

		// 命令No
		memcpy(pBuf->Hd.Meino, deli->cMeiNo, sizeof(pBuf->Hd.Meino));

		// コイルNo
		memcpy(pBuf->Hd.Coilno, deli->cCoilNo, sizeof(pBuf->Hd.Coilno));

		// ステータス
		memcpy(pBuf->Data.Status, "00", sizeof(pBuf->Data.Status));
	}
	break;

//-----------------------------------------------------------------------------------------------
	case QUE_SEND_MAE_T:									// 前工程実績要求
	case QUE_SEND_MAE_B:
	{
		// 伝文種類
		item->emStat = deli->eStat;
		// 命令No
		memcpy(item->cMeiNo, deli->cMeiNo, sizeof(item->cMeiNo));
		// コイルNo
		memcpy(item->cCoilNo, deli->cCoilNo, sizeof(item->cCoilNo));

		// 送信伝文サイズ (検査実績伝文サイズをセット)
		item->nSendSize = sizeof(FF_MAE_JISSEKI);
		// 送信伝文ファイルセット
		if (!ReadTestFile(item->cSendBuf, item->nSendSize, deli->cFileName))
		{
			item->emStat = (QUE_SEND_STAT)0;
			return false;
		}
	}
	break;

//-----------------------------------------------------------------------------------------------
	case QUE_SEND_HELCY:								// ヘルシー伝文送信要求
		{
			// 固定データ
			// 伝文種類
			item->emStat = deli->eStat;
			// 送信伝文サイズ (ヘルシー伝文サイズをセット)
			item->nSendSize = sizeof(FF_HELCY);			
			

			// 伝文生成
			FF_HELCY& pBuf = (FF_HELCY&)item->cSendBuf[0];
			memset(&pBuf, 0x20, sizeof(FF_HELCY));


			// 伝文コード
			memcpy( pBuf.Header_Code, FF_TC_CODE_HELCY, sizeof(pBuf.Header_Code));
			// 連番
			memcpy( pBuf.Rec_Code, "00", sizeof(pBuf.Rec_Code));
			// 伝文発生時刻
			cWk.Format("%04d%02d%02d%02d%02d%02d", STime.wYear, STime.wMonth, STime.wDay, STime.wHour, STime.wMinute, STime.wSecond); 
			memcpy( pBuf.DateTime, cWk, sizeof(pBuf.DateTime));
			// 工程コード
			memcpy(pBuf.Line_Code, KOUTEI_CODE, sizeof(pBuf.Line_Code));
			// 伝管コマンドNo
			memcpy(pBuf.Denkan_No, FF_DENKAN_NO, sizeof(pBuf.Denkan_No));
		}
		break;
	}
	return true;
}
//------------------------------------------
// テストデータ読み込み
// void* cSendData		伝文バッファ領域
// int iSendSize		伝文バッファサイズ
// char const* fullname	ファイル名称
//------------------------------------------
bool SockDataExec::ReadTestFile( BYTE* cSendData, int nSendSize, char const* cfullname )
{
	HANDLE		fp;
	DWORD		nNumByte = 0;
	bool 		bRet = false;
	char		cfullpath[MAX_PATH];

	sprintf( cfullpath, ".\\%s", cfullname );						// ファイルPATH生成

	LOG(em_MSG), "[%s] %s読込み開始", my_sThreadName, cfullname );	// 開始ログ出力

	fp = CreateFile( cfullpath,										// ファイルPATH
					 GENERIC_READ,									// アクセスモード（読込み）
					 FILE_SHARE_READ,								// 共有モード（読込みOK）
					 NULL,											// セキュリティ記述子（指定なし）
					 OPEN_EXISTING,									// 作成方法
					 FILE_ATTRIBUTE_NORMAL,							// ファイル属性（普通）
					 NULL );										// テンプレートファイルのハンドル（指定なし）

	if( fp != INVALID_HANDLE_VALUE ) {
		if( ReadFile( fp, cSendData, nSendSize, &nNumByte, NULL) ) {									// 読込み実行
			LOG(em_MSG), "[%s] %s読込み完了", my_sThreadName, cfullname );								// 読込み完了ログ出力
			bRet = true;
		} else {
			LOG(em_ERR), "[%s] %s読込み失敗 (retc=%d)", my_sThreadName, cfullname, GetLastError() );	// 読込み失敗ログ出力
			bRet = false;
		}
		CloseHandle( fp );
	} else {
		LOG(em_ERR), "[%s] %sオープン失敗 (retc=%d)", my_sThreadName, cfullname, GetLastError() );		// ファイルオープン失敗ログ出力
		bRet = false;
	}
	return bRet;
}

