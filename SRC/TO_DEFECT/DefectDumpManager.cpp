#include "StdAfx.h"
#include "DefectDumpManager.h"


// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


//#include "MainInctance.h"

//------------------------------------------
// コンストラクタ
// int		TorB		0:TOP 1:BOT	
// LPCSTR	filename	ログファイル名 (NULL時 ログ出力しない)
// int		cycleCnt	最大サイクリックファイル件数
// int		backCnt		最大バックアップ履歴数
//------------------------------------------
DefectDumpManager::DefectDumpManager(int TorB, LPCSTR filename, int cycleCnt, int backCnt) :
m_nMaxCycleCnt(cycleCnt),
m_nMaxBackCnt(backCnt),
m_nNowRow(0),
m_nNowFileNo(0),
gque_Deli(DEFECTDUMPMANAGER_MAX_QUE),
ThreadManager()
{
	CString cWk;
	cWk.Format("DefMgr_%d", TorB);
	my_sThreadName = cWk;
	
	//// 初期化
	InitializeCriticalSection(&m_csOutPut);
	InitializeCriticalSection(&m_csANC);

	//// 出力するしない決定
	m_bFileOut = NULL==filename ? false : true;
	
	//// バックアップ (ファイル出力有り時のみ)
	if ( m_bFileOut ) {
		int ii, jj;
		CString cFileName;
		CString cFileNameNew;
		char	cBaseFolder[32];

		// ログフォルダ生成
		GetPrivateProfileString("TO_DEFECT", "DEF_DUMP_PATH", "C:\\CSV", cBaseFolder, 32, TASKINI_NAME);
		_ASSERT( strlen(cBaseFolder) );

		// 表面のみﾌｫﾙﾀﾞ作成
		if (0 == TorB) _mkdir(cBaseFolder);
			
		// ベース名称決定
		//char cwk[256];
		//GetCurrentDirectory( 256, cwk);
		//m_cBaseFolder += cwk;
		//m_cBaseFolder += "\\Csv\\";
		m_cBaseFolder.Format("%s\\", cBaseFolder);
		m_cBaseFile = filename;

		// bk_xxx_@9_0.log ～ bk_xxx_@9_9.logを削除  (最古ファイル削除)
		for( ii=0; ii<=m_nMaxCycleCnt; ii++ ) {
			cFileName.Format("%sbk_%s_@%d_%d.csv", m_cBaseFolder, m_cBaseFile, m_nMaxBackCnt, ii );
			remove(cFileName);
		}

		// bk_xxx_@0_0.csv ～ bk_xxx_@0_9.csv ⇒ bk_xxx_@1_0.csv ～ bk_xxx_@1_9.csv に変更 (バックアップファイルリネーム)
		for( ii=0; ii<=m_nMaxCycleCnt; ii++ ) {
			for( jj=m_nMaxBackCnt; jj>0; jj-- ) {
				cFileName.Format("%sbk_%s_@%d_%d.csv", m_cBaseFolder, m_cBaseFile, jj-1, ii);	// 変換元
				cFileNameNew.Format("%sbk_%s_@%d_%d.csv", m_cBaseFolder, m_cBaseFile, jj, ii);	// 変換先
				rename(cFileName, cFileNameNew);
			}
		}

		// xxx_0.log～xxx_9.csv ⇒ bk_xxx_@0_0.csv ～ bk_xxx_@0_9.csv に変更 (最新ファイルをバックアップファイルにリネーム)
		for( ii=0; ii<=m_nMaxCycleCnt; ii++ ) {
			cFileName.Format("%s%s_%d.csv", m_cBaseFolder, m_cBaseFile, ii);					// 変換元
			cFileNameNew.Format("%sbk_%s_@0_%d.csv", m_cBaseFolder, m_cBaseFile, ii);			// 変換先
			rename(cFileName, cFileNameNew);
		}

		// CSVファイルオープン
		cFileName.Format("%s%s_%d.csv", m_cBaseFolder, m_cBaseFile, m_nNowFileNo);
		m_stream.open(cFileName, std::ios::out | std::ios::trunc);
		m_cNowFPath = cFileName;
	}

}

//------------------------------------------
// デストラクタ
//------------------------------------------
DefectDumpManager::~DefectDumpManager()
{
	// ファイルクローズ
	//if ( m_stream != NULL ) {
	if ( m_stream.is_open() ) {
		// クローズ
		m_stream.close();
		//m_stream = NULL; //←これがダメ。代わりが分からない
	}
	// 開放
	DeleteCriticalSection(&m_csOutPut);
	DeleteCriticalSection(&m_csANC);
	
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int DefectDumpManager::ThreadFirst()
{
	//// イベント設定
	HANDLE hArray[] = { gque_Deli.g_evSem
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (スレッド終了直前にコールバック)
//------------------------------------------
int DefectDumpManager::ThreadLast()
{
	//// 全疵情報開放		
	AllDeleteQueue();

	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void DefectDumpManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件		
	enum {	EV_QUE = 0,								// パラメータ変更通知
			EV_END	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_QUE:									// パラメータ変更通知
		if(true) {
			DEFDUMP_QUE* pDeli = gque_Deli.GetFromHead();

			// ファイル出力
			if( m_bFileOut) {
				if(0 > pDeli->nRecord) FlushFile();
				else				   WriteFile(pDeli->nRecord, pDeli->pStr);
			}
			SAFE_DELETE_ARRAY(pDeli->pStr);
			SAFE_DELETE(pDeli);
		}
		break;

//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		break;
	}
}


//------------------------------------------
// 非同期書き込みエントリー
// int nRecord コイル情報レコードNo
// LPCSTR __cBuffer 可変配列メッセージ
//------------------------------------------
void DefectDumpManager::Write( int nRecord, LPCSTR __cBuffer, ... )
{
	char	cMsg[20480];
    SYSTEMTIME time;
    GetLocalTime(&time);

	AutoLock autolock(&m_csANC);

	//// 可変引数より、メッセージ作成
	memset( cMsg, 0x00, sizeof(cMsg));
	va_list	vmarker;
	va_start( vmarker, __cBuffer ); 				// 可変個の引数の初期化
//	_vsnprintf( (char*)cMsg, _TRUNCATE, __cBuffer, vmarker );
	_vsnprintf( (char*)cMsg, sizeof(cMsg)-1, __cBuffer, vmarker );
	va_end( vmarker );								// 可変個の引数のリセット
	
	//// 書き込み用バッファ作成 (使い終わったら必ず開放する事)
	int		nSize;
	nSize = (int)strlen(cMsg) + 256;			// 256は 時刻 や キュー無し時にメッセージ 等で使用する為
	LPSTR pStr = new char [nSize];							// CString を char*に変更 (連続で書き込むと 0.2ms から 0.1msに短縮)
	// メッセージ生成
	sprintf(pStr, "%04d/%02d/%02d %02d:%02d:%02d.%03d%s",
		time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds,
		cMsg
		);

	//// キューに登録
	// キュー残容量チェック
	if( 0 == gque_Deli.GetFreeCount() ) {			// 空きなし
		// 全ログ開放
		int cnt;
		cnt = AllDeleteQueue();
		// 非同期書き込みログ削除
		memset(pStr, 0x00, nSize);
		//sprintf(pStr, "%04d/%02d/%02d %02d:%02d:%02d.%03d%s%d ", 
		//	"疵情報キューフル!! 疵情報削除件数=", cnt,
		//	time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds
		//	);
		//return;
	}

	DEFDUMP_QUE* pQue = new DEFDUMP_QUE;
	pQue->nRecord = nRecord;
	pQue->pStr = pStr;
	// キューに登録
	gque_Deli.AddToTail( pQue );		// strは取得後に開放
}


//------------------------------------------
// 同期書込み (可変引数対応)
// int nRecord コイル情報レコードNo
// LPCSTR __cBuffer 可変配列メッセージ
//------------------------------------------
void DefectDumpManager::WriteLock( int nRecord, LPCSTR __cBuffer, ... )
{
	char	cMsg[20480];
    SYSTEMTIME time;
    GetLocalTime(&time);

	AutoLock autolock(&m_csANC);

	//// 可変引数より、メッセージ作成
	memset( cMsg, 0x00, sizeof(cMsg));
	va_list	vmarker;
	va_start( vmarker, __cBuffer ); 				// 可変個の引数の初期化
	_vsnprintf( (char*)cMsg, sizeof(cMsg)-1, __cBuffer, vmarker );
	va_end( vmarker );								// 可変個の引数のリセット
	
	//// 書き込み用バッファ作成 (使い終わったら必ず開放する事)
	int		nSize;
	nSize = (int)strlen(cMsg) + 256;			// 256は 時刻 や キュー無し時にメッセージ 等で使用する為
	LPSTR pStr = new char [nSize];							// CString を char*に変更 (連続で書き込むと 0.2ms から 0.1msに短縮)
	// メッセージ生成
	sprintf(pStr, "%04d/%02d/%02d_%02d:%02d:%02d.%03d%s",
		time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds,
		cMsg
		);


	// ファイル出力
	if( m_bFileOut) WriteFile(nRecord, pStr);


	SAFE_DELETE_ARRAY( pStr );
}


//------------------------------------------
// 制御書き込み
// int nRecord コイル情報レコードNo
// LPCSTR __cBuffer 可変配列メッセージ
//------------------------------------------
void DefectDumpManager::WriteFile(int nRecord, LPCSTR pStr)
{
	AutoLock autolock(&m_csOutPut);

	if(nRecord != m_nNowFileNo) {
		m_stream.close();

		m_nNowRow = 0;
		m_nNowFileNo = nRecord;
		if( (int)m_nNowFileNo < 0 ) m_nNowFileNo = 0;
		if( (int)m_nNowFileNo > m_nMaxCycleCnt ) m_nNowFileNo = m_nMaxCycleCnt;
		CString cFileName;
		cFileName.Format("%s%s_%d.csv", m_cBaseFolder, m_cBaseFile, m_nNowFileNo);
		m_stream.open(cFileName, std::ios::out | std::ios::trunc);
		m_cNowFPath = cFileName;

	}
	////// ファイル出力
	//// 疵情報ファイルに書き込み
    m_stream << pStr << std::endl;
	m_nNowRow++;
}


//------------------------------------------
// 制御強制出力
//------------------------------------------
void DefectDumpManager::FlushFile()
{
	AutoLock autolock(&m_csOutPut);

	m_stream.flush();

}

//------------------------------------------
// 全キュー強制出力
// 戻り値 出力件数
//------------------------------------------
int DefectDumpManager::AllGetQueue()
{
	DEFDUMP_QUE*	pDeli;

	int cnt = 0;
	// 全キュー取り出し
	while (true) { 
		pDeli = gque_Deli.GetFromHead(0);
		if(pDeli) {
			// ファイル出力
			if( m_bFileOut) WriteFile(pDeli->nRecord, pDeli->pStr);
			SAFE_DELETE_ARRAY( pDeli ) ;
			cnt++;
		} else {
			break;
		}
	}
	return cnt;
}

//------------------------------------------
// 全キュー強制削除
// 戻り値 削除件数
//------------------------------------------
int DefectDumpManager::AllDeleteQueue()
{
	DEFDUMP_QUE*	pDeli;

	int cnt = 0;
	// 全キュー取り出し
	while (true) { 
		pDeli = gque_Deli.GetFromHead(0);
		if(pDeli) {
			SAFE_DELETE_ARRAY(pDeli->pStr);
			SAFE_DELETE(pDeli);
			cnt++;
		} else {
			break;
		}
	}
	return cnt;
}

