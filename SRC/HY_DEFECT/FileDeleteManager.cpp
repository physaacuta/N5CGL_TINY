#include "StdAfx.h"
#include "FileDeleteManager.h"
#include <io.h>

//------------------------------------------
// コンストラクタ
//------------------------------------------
FileDeleteManager::FileDeleteManager(void):
mque_pFileDel(20),
ThreadManager("FileDelMgr")
{
	//// ハンドル生成
	m_evRuning		= CreateEvent(NULL, TRUE, FALSE, NULL);		// マニュアルリセット
	m_evRunEnding	= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evRunEnd		= CreateEvent(NULL, FALSE, FALSE, NULL);

	// 初期化
	m_bRun = false;
	memset(m_cPath, 0x00, sizeof(m_cPath));
}

//------------------------------------------
// デストラクタ
//------------------------------------------
FileDeleteManager::~FileDeleteManager(void)
{
	CloseHandle(m_evRuning);
	CloseHandle(m_evRunEnding);
	CloseHandle(m_evRunEnd);
}


//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int FileDeleteManager::ThreadFirst()
{
	//// 事前準備

	//// イベント設定
	HANDLE hArray[] = { m_evRunEnding,
						m_evRuning,
						mque_pFileDel.g_evSem
						};

	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	//// 追加
	//this->AddEventNum(1, &mque_pFileDel.g_evSem);
	return 0;
}
//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int FileDeleteManager::ThreadLast()
{
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void FileDeleteManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_RUN_ENDING = 0,				// 実行中断要求シグナル
			EV_RUN,							// ファイル削除
			EV_QUE							// ファイル削除(QUE)
	};

	TYP_DELETE_FILE* pDeli = NULL;				// 受け渡し情報

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_RUN_ENDING:		
		// 削除処理がそもそも動いていない場合
		ResetEvent(m_evRuning);
		SetEvent(m_evRunEnd);
		m_bRun = false;
		break;

//-----------------------------------------------------------------------------------------------
	case EV_RUN:
		DeleteExec();
		break;

//-----------------------------------------------------------------------------------------------
	case EV_QUE:
		pDeli = mque_pFileDel.GetFromHead();
		DeleteExecQue(pDeli->cPath);
		delete pDeli;
		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 処理

//------------------------------------------
// 削除開始
// char const* pPath フォルダパス (.\\temp\\rec0)
//------------------------------------------
bool FileDeleteManager::DeleteStart(char const* pPath)
{
	bool bRetc = true;

	// 前回がまだ動いている？
	if (m_bRun) {
		SetEvent(m_evRunEnding);
		// とまるまで待ち
		if (WAIT_TIMEOUT == WaitForSingleObject(m_evRunEnd, 100)) {
			LOG(em_WAR), "[%s] 実行停止タイムアウト <%s>", my_sThreadName, m_cPath );
			bRetc = false;
		}
	}

	// 削除開始
	memset(m_cPath, 0x00, sizeof(m_cPath));
	strcpy(m_cPath, pPath);
	SetEvent(m_evRuning);
	return bRetc;
}

//------------------------------------------
// 削除開始(QUE)
// char const* pPath フォルダパス (.\\temp\\rec0)
//------------------------------------------
bool FileDeleteManager::DeleteAll(char const* pPath)
{
	bool bRetc = true;
	TYP_DELETE_FILE* DelPath = new TYP_DELETE_FILE;
	memset(DelPath, 0x00, sizeof(TYP_DELETE_FILE));
	strcpy(DelPath->cPath , pPath);

	bRetc=mque_pFileDel.AddToTail(DelPath);
	return bRetc;
}

//------------------------------------------
// 削除処理
//------------------------------------------
void FileDeleteManager::DeleteExec()
{
	BOOL			bRun		= TRUE;
	int				nStopMode	= 0;			// 0:正常 1:キャンセル -1:異常
	int				ii=0;
	//CTime			tBase= CTime::GetCurrentTime() - CTimeSpan(1,0,0,0);			// 1日前
	CTime			tBase = CTime::GetCurrentTime() - CTimeSpan(0, 12, 0, 0);		// 12時間前
	//CTime			tBase = CTime::GetCurrentTime() - CTimeSpan(0, 0, 0, 0);		// 0時間前
	CString			cPath;						// 受け渡しパス
	CString			cFileName;					// 検索したファイルのフルパス
	CString			cSearchFile;				// フォルダ内のファイルを検索するキー
	CString			sWk;
	std::deque<CString> queDeleteFile;			// 削除ファイル



	//===================================
	// 事前準備
	m_bRun = true;
	cPath  = m_cPath;
	cSearchFile.Format( "%s\\*.*", cPath );


	//===================================
	// 削除対象ファイルを特定
	WIN32_FIND_DATA	fd;
	HANDLE			hFind;
	memset(&fd, 0x00, sizeof(fd));
	hFind = FindFirstFile(cSearchFile, &fd);
	while(hFind != INVALID_HANDLE_VALUE) {
		//// ファイルのみ
		if( FILE_ATTRIBUTE_DIRECTORY != fd.dwFileAttributes ) {
			// 指定日以上経過したファイルを消す
			if( tBase > CTime(fd.ftLastWriteTime) ) {

				queDeleteFile.push_back( fd.cFileName );
			}
		}

		//// 次
		bRun = FindNextFile(hFind, &fd);
		if(!bRun) {
			if(ERROR_NO_MORE_FILES != GetLastError()) {
				nStopMode = -1;
				LOG(em_MSG), "[%s] ファイル削除失敗<%s>", my_sThreadName, fd.cFileName);
			}
			break;
		}

		//// 停止要求を認識する
		if(WAIT_TIMEOUT != WaitForSingleObject(m_evRunEnding, 0)) {
			// 停止要求有りなので中断
			nStopMode = 1;
			break;
		}
	}
	FindClose(hFind);



	//===================================
	// 削除対象ファイルを削除
	int nCnt = (int)queDeleteFile.size();
	for(ii=0; ii<nCnt; ii++ ) {
		sWk.Format("%s\\%s", m_cPath, queDeleteFile[ii]);
		DeleteFile(sWk);

		//// 停止要求を認識する
		if(WAIT_TIMEOUT != WaitForSingleObject(m_evRunEnding, 0)) {
			// 停止要求有りなので中断
			nStopMode = 1;
			break;
		}
	}

	LOG(em_MSG), "[%s] ファイル削除完了 <%s=%d><%d><%d>", my_sThreadName, cPath, nCnt, nStopMode, ii);



	//===================================
	// 削除停止
	ResetEvent(m_evRuning);
	m_bRun = false;
	if(1 == nStopMode) SetEvent(m_evRunEnd);
}
//------------------------------------------
// 削除処理(QUE)
//------------------------------------------
void FileDeleteManager::DeleteExecQue(char const* pPath)
{
	BOOL			bRun = TRUE;
	//int				nStopMode = 0;			// 0:正常 1:キャンセル -1:異常
	int				ii = 0;
	//CTime			tBase= CTime::GetCurrentTime() - CTimeSpan(1,0,0,0);			// 1日前
	CTime			tBase = CTime::GetCurrentTime() - CTimeSpan(0, 12, 0, 0);		// 12時間前
	CString			cPath;						// 受け渡しパス
	CString			cFileName;					// 検索したファイルのフルパス
	CString			cSearchFile;				// フォルダ内のファイルを検索するキー
	CString			sWk;
	std::deque<CString> queDeleteFile;			// 削除ファイル



												//===================================
												// 事前準備
	cPath = pPath;
	cSearchFile.Format("%s\\*.*", cPath);
	LOG(em_MSG), "[%s] ファイル削除開始(QUE) <%s>", my_sThreadName, cSearchFile);


	//===================================
	// 削除対象ファイルを特定
	WIN32_FIND_DATA	fd;
	HANDLE			hFind;
	memset(&fd, 0x00, sizeof(fd));
	hFind = FindFirstFile(cSearchFile, &fd);
	while (hFind != INVALID_HANDLE_VALUE) {
		//// ファイルのみ
		if (FILE_ATTRIBUTE_DIRECTORY != fd.dwFileAttributes) {
			// 指定日以上経過したファイルを消す
			if (tBase > CTime(fd.ftLastWriteTime)) {

				queDeleteFile.push_back(fd.cFileName);
			}
		}

		//// 次
		bRun = FindNextFile(hFind, &fd);
		if (!bRun) {
			if (ERROR_NO_MORE_FILES != GetLastError()) {
				LOG(em_MSG), "[%s] ファイル削除失敗<%s>", my_sThreadName, fd.cFileName);
			}
			break;
		}
	}
	FindClose(hFind);



	//===================================
	// 削除対象ファイルを削除
	int nCnt = (int)queDeleteFile.size();
	for (ii = 0; ii<nCnt; ii++) {
		sWk.Format("%s\\%s", cPath, queDeleteFile[ii]);
		DeleteFile(sWk);
	}

	LOG(em_MSG), "[%s] ファイル削除完了(QUE) <%s=%d><%d>", my_sThreadName, cPath, nCnt,ii);
}
