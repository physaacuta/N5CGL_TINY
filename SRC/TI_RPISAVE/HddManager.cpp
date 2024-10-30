#include "StdAfx.h"
#include "HddManager.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 本来の物
//------------------------------------------
// コンストラクタ
//------------------------------------------
HddManager::HddManager(void) :
ThreadManager("HddMgr")
{
	//// ハンドル生成
	m_evTimerCheck	  = CreateWaitableTimer(NULL, FALSE, NULL);

	//// その他初期化
	for(int ii=0; ii<MAX_INS; ii++) m_bSaveFolderAccess[ii] = false;
}

//------------------------------------------
// デストラクタ
//------------------------------------------
HddManager::~HddManager(void)
{
	//// ハンドル解放
	CloseHandle(m_evTimerCheck);
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int HddManager::ThreadFirst()
{
	ParamManager::P_MAIN		const& prm	= mcls_pParam->GetMain();


	//// フォルダ生成
	for(int ii=0; ii<prm.nIns; ii++) {
		ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[ ii ];
		// 0:OK  EEXIST:既存有り  ENOENT:パス無し	
		if( 0 != _mkdir( prmI.cRawSavePath ) ) {
			int* nErr = _errno();
			if( ENOENT == *nErr) { LOG(em_ERR), "[%s] フォルダ生成失敗(%s)", my_sThreadName, prmI.cRawSavePath); }
		}

		for(int jj=0; jj<prmI.nRpiFolderBufNum; jj++)  { 
			if( 0 != _mkdir( prmI.cRpiFolderBuf[jj] ) ) {
				int* nErr = _errno();
				if( ENOENT == *nErr) { LOG(em_ERR), "[%s] フォルダ生成失敗(%s)", my_sThreadName, prmI.cRpiFolderBuf[jj]); }
			}
		}
		for(int jj=0; jj<prmI.nRpiFolderSaveNum; jj++) { 
			if( 0 != _mkdir( prmI.cRpiFolderSave[jj] ) ) {
				int* nErr = _errno();
				if( ENOENT == *nErr) { LOG(em_ERR), "[%s] フォルダ生成失敗(%s)", my_sThreadName, prmI.cRpiFolderSave[jj]); }
			}
		}
	}


	//// ワーク領域にある 前回の残骸を削除
	FastDelete();

	//// 保存フォルダ アクセスチェック
	if( ! AccessSaveFolder(-1) ) {
		// 復旧チェック
		StartTimer_SaveFolder();
	}

	//// 空き容量チェック
	for(int ii=0; ii<prm.nIns; ii++) {
		ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[ii];

		//// 最古ファイルのフォルダのみ削除
		for(int jj=0; jj<prmI.nRpiFolderBufNum; jj++) {
			FreeSpace( prmI.cRpiFolderBuf[jj], prm.nHddFreeSizeLocal );
		}
	}
	 

	//// イベント設定
	HANDLE hArray[] = { m_evTimerCheck,
						mque_pRpiMove->g_evSem
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int HddManager::ThreadLast()
{
	//// キュー解放
	RPI_MOVE* pDeli;
	while(true) {
		pDeli = mque_pRpiMove->GetFromHead(0);
		if( pDeli == NULL ) break;
		delete pDeli;
	}
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void HddManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_TIEM_CHECK = 0,				// 保存フォルダアクセスチェック
			EV_ADD_QUE,						// 受渡しキュー
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_TIEM_CHECK: 					// 保存フォルダアクセスチェック
		if( ! AccessSaveFolder(-1) ) {
			StartTimer_SaveFolder();
		} else {	
			BackUpCopy();
		}	
		return;

//-----------------------------------------------------------------------------------------------
	case EV_ADD_QUE: 						// 受渡しキュー
		if(true) {
			RPI_MOVE* pDeli = mque_pRpiMove->GetFromHead();
			if( Exec(pDeli) ) {										// 実行
				if( 0 == mque_pRpiMove->GetCount() ) BackUpCopy(pDeli);	// バックアップ移動
			} else {
				m_bSaveFolderAccess[ pDeli->nIns ] = false; 
				StartTimer_SaveFolder();
			}
			delete pDeli;
		}
		return;
	
//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 外部処理

//------------------------------------------
// メイン処理
// RPI_MOVE const* p 受渡し情報
//------------------------------------------
bool HddManager::Exec(RPI_MOVE const* p)
{
	CString sWk;	
	ParamManager::P_MAIN		const& prm	= mcls_pParam->GetMain();
	ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[ p->nIns ];


	//// 未接続状態
	if( ! m_bSaveFolderAccess[p->nIns] ) goto Ending;


	//// 最新ファイルのフォルダを特定
	int nIndex = SearchFolderIndex(p->nIns, true);	


	//// 空き容量チェック
	//		最新ファイルのフォルダに空きがあるか？
	//		空きが無い場合、次のフォルダに空きがあるか？(まだ一周回ってない場合があるための処置)
	//		最古ファイルのフォルダに空きを作る。
	if( ! CheckFreeSpace( prmI.cRpiFolderSave[nIndex], prm.nHddFreeSizeSave) ) {

		if( 1 < prmI.nRpiFolderSaveNum ) {
			// 次のフォルダは空いているか。まだ一周回ってない場合があるための処置
			nIndex ++;
			if(nIndex >= prmI.nRpiFolderSaveNum ) nIndex = 0;
			if( ! CheckFreeSpace( prmI.cRpiFolderSave[nIndex], prm.nHddFreeSizeSave) ) {
				// 既に一周回っているケース。
				// 最古を特定して、最古ファイルのフォルダに対して空き領域を確保
				nIndex = SearchFolderIndex(p->nIns, false);	
				if( ! FreeSpace(prmI.cRpiFolderSave[nIndex], prm.nHddFreeSizeSave) )goto Ending;
			}
		} else {
			// 1個しかないため、そのフォルダの空き領域を確保
			if( ! FreeSpace(prmI.cRpiFolderSave[nIndex], prm.nHddFreeSizeSave) ) goto Ending;
		}
	}

	//// コピー元ファイルがそもそもあるか？ (2回来る可能性があるための処置)
	// ファイル名
	sWk.Format("%s\\%s", p->sPath_F, p->sName );
	if( ! GetFileUmu(sWk) ) {
		LOG(em_WAR), "[%s] 移動前で既にコピー元ファイル無し %s", my_sThreadName, sWk);
		goto Ending;
	}


	// 普通のファイル移動
	if( ! CopyFileExec(p, nIndex) ) {

		//// リトライ処理
		
		// 移動先に同一名称が無いか
		for(int ii=0; ii<prmI.nRpiFolderSaveNum; ii++) {
			sWk.Format("%s\\%s", prmI.cRpiFolderSave[ii], p->sName );
			if( DeleteFile(sWk) ) {
				LOG(em_WAR), "[%s] 移動前の整合性削除完了 %s", my_sThreadName, sWk);
			}
		}
		
		// 移動
		if( ! CopyFileExec(p, nIndex) ) goto Ending;
	}

Ending:
	//// 次の保存のために 自領域もチェック
	FreeSpace(p->sPath_F, prm.nHddFreeSizeLocal);
	return true;
}


//------------------------------------------
// 全長画像保存PCの空きエリアを確保
// char const* sFolder 対象フォルダ (最後の\は無し)
//------------------------------------------
bool HddManager::FreeSpace(char const* sFolder, const int nFreeSpaceGB)
{
	double					dNowFreeSize;	// 現在の空き容量 [GB]
	std::vector<RPI_ITEM>	vItem;			// RPIファイル一覧
	CString	sWk;

	ParamManager::P_MAIN	 const& prm	 = mcls_pParam->GetMain();


	// =========================================================
	// 空き容量をチェック
	// なんかネットワーク経由だと最後に\を付けないとダメっぽい
	dNowFreeSize = CheckFreeSpace( sFolder );
	if( 0 > (int)dNowFreeSize ) return true;					// サイズ取得異常なので、無視
	if(nFreeSpaceGB <= (int)dNowFreeSize ) return true;	// 最低空き容量よりも大きい？


	// =========================================================
	// フォルダ内の古いもの順のRPIファイル一覧を生成
	
	//// フォルダ内のRPIファイル一覧を取得
	sWk.Format("%s\\*.%s", sFolder, RPI_TYPE );
	WIN32_FIND_DATA	fd;
	HANDLE			hFind;
	memset(&fd, 0x00, sizeof(fd));
	hFind = FindFirstFile(sWk, &fd);
	while(hFind != INVALID_HANDLE_VALUE) {
		// ファイルのみ
		if( FILE_ATTRIBUTE_DIRECTORY != fd.dwFileAttributes ) {
			// リストに追加
			RPI_ITEM t;
			t.sName				= fd.cFileName;
			t.tTime				= fd.ftLastAccessTime;
			t.nSize.HighPart	= fd.nFileSizeHigh;
			t.nSize.LowPart		= fd.nFileSizeLow;
			vItem.push_back(t);
		}
		// 次
		if(! FindNextFile(hFind, &fd)) {
			if(ERROR_NO_MORE_FILES != GetLastError()) {
				LOG(em_ERR), "[%s] ファイル一覧取得失敗 %d", my_sThreadName, GetLastError());
			}
			break;
		}
	}
	FindClose(hFind);

	//// 古い順にソート
	std::sort(vItem.begin(), vItem.end(), sort_fd_old);


	// =========================================================
	// 最低空き容量 以上になるまで 古いRPIファイルを削除
	double dWk;
	double dTotalDelSize = 0.0;				// 削除ファイルサイズ [GB]	
	for(int ii=0; ii<(int)vItem.size()-prm.nIns; ii++) {				// 直近の数ファイルは絶対に消さないこと。もしかしたら自分かもしれないし・・・

		dWk = vItem[ii].nSize.QuadPart  /1024.0/1024.0/1024.0;	// [B→GB]
		sWk.Format("%s\\%s", sFolder, vItem[ii].sName );
		DeleteFile(sWk);
		int nWk = GetLastError();
		LOG(em_INF), "[%s] RPIﾌｧｲﾙ削除 [%.3fGB](アクセス日付=%s)(%s) retc=%d", my_sThreadName, dWk, vItem[ii].tTime.Format("'%Y/%m/%d %H:%M:%S'"), sWk, nWk);

		// 次のため
		dTotalDelSize += dWk;
		if( 2<=ii ) {   //少なくとも3ファイルくらいは消す
			if(nFreeSpaceGB <= (int)(dNowFreeSize+dTotalDelSize) ) break;
		}
	}

	// 整合性チェック
	if(nFreeSpaceGB> (int)(dNowFreeSize+dTotalDelSize) ) {
		LOG(em_ERR), "[%s] 全部RPIファイル削除したのに、最低空き容量[%dGB]を下回る！", my_sThreadName, nFreeSpaceGB, GetLastError());
		return false;
	}

	return true;
}

//------------------------------------------
// 普通のファイル移動
// RPI_MOVE const* p 受渡し情報
// int nHddIndex HDDIndex
//------------------------------------------
bool HddManager::CopyFileExec(RPI_MOVE const* p, int nHddIndex)
{
	CString			sPathSrc,	sPathDst;				// フルパス
	ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[ p->nIns ];

	//// パス決定
	sPathSrc.Format("%s\\%s", p->sPath_F, p->sName );
	sPathDst.Format("%s\\%s", prmI.cRpiFolderSave[nHddIndex], p->sName );


	//// 移動
	if( MoveFile(sPathSrc, sPathDst) ) {
	//if( MoveFileEx(sPathSrc, sPathDst, MOVEFILE_REPLACE_EXISTING) ) {
		LOG(em_MSG), "[%s] RPIﾌｧｲﾙ移動完了 [%s]", my_sThreadName, sPathSrc);
		return true;
	} else {
		LOG(em_ERR), "[%s] RPIﾌｧｲﾙ移動失敗 [%s][%d]", my_sThreadName, sPathSrc, GetLastError());
		return false;
	}
}

//------------------------------------------
// 初期起動時の ゴミ削除
//------------------------------------------
void HddManager::FastDelete()
{
	//// 通常無いはずだが、一次ワーク領域に *.rpiwk が残骸として残っている場合、削除する
	std::vector<CString>		vItem;			// RPIファイル一覧
	CString						sWk;
	WIN32_FIND_DATA				fd;
	HANDLE						hFind;
	ParamManager::P_MAIN		const& prm	= mcls_pParam->GetMain();


	for(int ii=0; ii<prm.nIns; ii++) {
		ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[ii];

		//// フォルダ内のRPIワークファイル一覧を取得
		sWk.Format("%s\\*.%s", prmI.cRpiFolderBuf, RPI_TYPE_WK );
		memset(&fd, 0x00, sizeof(fd));
		hFind = FindFirstFile(sWk, &fd);
		while(hFind != INVALID_HANDLE_VALUE) {
			// ファイルのみ
			if( FILE_ATTRIBUTE_DIRECTORY != fd.dwFileAttributes ) {
				// リストに追加
				vItem.push_back(fd.cFileName);
			}
			// 次
			if(! FindNextFile(hFind, &fd)) {
				if(ERROR_NO_MORE_FILES != GetLastError()) {
					LOG(em_ERR), "[%s] ファイル一覧取得失敗 %d", my_sThreadName, GetLastError());
				}
				break;
			}
		}
		FindClose(hFind);

		//// 削除
		for(int ii=0; ii<(int)vItem.size(); ii++) {
			sWk.Format("%s\\%s", prmI.cRpiFolderBuf, vItem[ii] );
			DeleteFile(sWk);
			LOG(em_WAR), "[%s] RPIﾜｰｸﾌｧｲﾙ削除 (%s)", my_sThreadName, sWk);
		}

		//// 次へ
		vItem.clear(); 
	}
}

//------------------------------------------
// 一次フォルダにあるRPIファイルを移動 (正常時では対象ファイルは無いはず)
//------------------------------------------
void HddManager::BackUpCopy(RPI_MOVE const* p)
{

	//// 通常無いはずだが、一次ワーク領域に *.rpi が残っている場合、移動する
	std::vector<CString>		vItem;			// RPIファイル一覧
	CString						sWk;
	WIN32_FIND_DATA				fd;
	HANDLE						hFind;
	ParamManager::P_MAIN		const& prm	= mcls_pParam->GetMain();

	CString sMyFile;
	if(NULL != p) sMyFile = p->sName;

	for(int ii=0; ii<prm.nIns; ii++) {
		if( ! m_bSaveFolderAccess[ii] ) continue;

		ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[ii];

		for(int rec=0; rec<prmI.nRpiFolderBufNum; rec++) {

			//// フォルダ内のRPIワークファイル一覧を取得
			sWk.Format("%s\\*.%s", prmI.cRpiFolderBuf[rec], RPI_TYPE );
			memset(&fd, 0x00, sizeof(fd));
			hFind = FindFirstFile(sWk, &fd);
			while(hFind != INVALID_HANDLE_VALUE) {
				// ファイルのみ
				if( FILE_ATTRIBUTE_DIRECTORY != fd.dwFileAttributes ) {
	//				if( 0 < strcmp(sMyFile, fd.cFileName ) ) {		// 自分よりも小さいやつだけ対象
					if( 0 != strcmp(sMyFile, fd.cFileName ) ) {		// 自分は対象外
						// リストに追加
						LOG(em_WAR), "[%s] RPIﾌｧｲﾙ ﾊﾞｯｸｱｯﾌﾟ移動対象有り (%s)", my_sThreadName, fd.cFileName);
						vItem.push_back(fd.cFileName);
					}
				}
				// 次
				if(! FindNextFile(hFind, &fd)) {
					if(ERROR_NO_MORE_FILES != GetLastError()) {
						LOG(em_ERR), "[%s] ファイル一覧取得失敗 %d", my_sThreadName, GetLastError());
					}
					break;
				}
			}
			FindClose(hFind);

			//// 長期保存場所に同一ファイルがあれば、一度消す
			// 中途半端なRPIファイルができている場合がある対策
			for(int jj=0; jj<(int)vItem.size(); jj++) {
				for(int kk=0; kk<prmI.nRpiFolderSaveNum; kk++) {
					sWk.Format("%s\\%s", prmI.cRpiFolderSave[kk], vItem[jj] );
					if( DeleteFile(sWk) ) {
						LOG(em_WAR), "[%s] バックアップ削除完了 %s", my_sThreadName, sWk);
						//break;	// もしかしたら複数個所に存在するかも知れないので、一応全部チェック
					}
				}
			}

			//// 移動
			for(int jj=0; jj<(int)vItem.size(); jj++) {
				if( 0 != mque_pRpiMove->GetCount() ) return;	// 複数実行されるのを防止?なぜここにこれがあるのかいまいち覚えていない。

				// 受渡しデータ生成
				RPI_MOVE* p = new RPI_MOVE;
				p->nIns		= ii;	
				p->sName	= vItem[jj];
				p->sPath_F	= prmI.cRpiFolderBuf[rec];

				// 実行
				Exec(p);

				// 後始末
				delete p;
			}
			vItem.clear(); 
		}
	}
}
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 小物

//// ソートの比較関数
//------------------------------------------
// 古いやつを見つける。Aが古い場合True
//------------------------------------------
inline bool HddManager::sort_fd_old(const RPI_ITEM &a, const RPI_ITEM &b)
{
	return a.tTime < b.tTime;
}
//------------------------------------------
// 新しいやつを見つける。Aが新しい場合True
//------------------------------------------
inline bool HddManager::sort_fd_new(const RPI_ITEM &a, const RPI_ITEM &b)
{
	return a.tTime > b.tTime;
}

//------------------------------------------
// 長期保存用フォルダにアクセス可能かチェック
// int nIns 対象インスタンス -1:全部  0～:対象インスタンスのみ
// 戻り値 -1時：全部OK時true  0～時：対象インスタンスの状態
//------------------------------------------
bool HddManager::AccessSaveFolder(int nIns)
{
	int ii_s, ii_e;
	bool bRetc = true;	// 全体を通して1つでもNGあり
	bool bAccess;

	ParamManager::P_MAIN		const& prm	= mcls_pParam->GetMain();

	//// 対象決定
	if(-1==nIns) { ii_s=0; ii_e=prm.nIns-1; }
	else		 { ii_s=ii_e=nIns;}

	//// アクセスチェック
	for(int ii=ii_s; ii<=ii_e; ii++) {
		ParamManager::P_MAIN::INS    const& prmI = mcls_pParam->GetMain().ins[ ii ];
		bAccess = true;
		// フォルダチェック
		// D:\ でも D: でもどちらでもOK
		for(int jj=0; jj<prmI.nRpiFolderSaveNum; jj++) {
			if( ! PathIsDirectory( prmI.cRpiFolderSave[jj] ) ) {
				LOG(em_WAR), "[%s] 長期保存用フォルダアクセス失敗 [%s]", my_sThreadName, prmI.cRpiFolderSave[jj] );	
				bAccess = false;
				bRetc = false;
			}
		}
		// 結果セット
		m_bSaveFolderAccess[ii] = bAccess;
	}
	return bRetc;
}

//------------------------------------------
// 長期保存用フォルダにアクセス可能チェックタイマー
//------------------------------------------
void HddManager::StartTimer_SaveFolder()
{
	__int64 ts = FOLDER_CHECK_TIME * -10000;		// 1ミリ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evTimerCheck, (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE);
}


//------------------------------------------
// 同一ドライブかチェック
// char const* cPath1
// char const* cPath2
// 戻り値：同一ドライブ時 ture
//------------------------------------------
bool HddManager::RootFolderCheck(char const* cPath1, char const* cPath2)
{
	char cWk1[MAX_PATH];
	char cWk2[MAX_PATH];
	
	memset(cWk1, 0x00, sizeof(cWk1));
	memset(cWk2, 0x00, sizeof(cWk2));
	strcpy(cWk1, cPath1);
	strcpy(cWk2, cPath2);

	// 最後の\マークを付与。(d: の場合正常に取れない為)
	PathAddBackslash(cWk1);
	PathAddBackslash(cWk2);

	// 比較
	return PathIsSameRoot(cWk1, cWk2);
}


//------------------------------------------
// 全長画像保存PCの空きエリアチェック
// char const* sFolder 対象フォルダ (最後の\は無し)
// 戻り値	0～：現在の空き容量 [GB]	0>:失敗
//------------------------------------------
double HddManager::CheckFreeSpace(char const* sFolder)
{
	CString	sWk;
	ParamManager::P_MAIN	 const& prm	 = mcls_pParam->GetMain();

	// =========================================================
	// 空き容量をチェック
	// なんかネットワーク経由だと最後に\を付けないとダメっぽい
	sWk.Format("%s\\", sFolder );

	// 空きサイズ取得
	ULARGE_INTEGER n1;		// 使用可能バイト数
	ULARGE_INTEGER n2;		// 全バイト数
	ULARGE_INTEGER n3;		// 空きバイト数
	if( ! GetDiskFreeSpaceEx( sWk, &n1, &n2, &n3 )) {
		int nRetc = GetLastError();
		LOG(em_ERR), "[%s] 空きサイズ取得失敗 =%d [%s]", my_sThreadName, nRetc, sWk);
		return -1;
	}

	double dNowFreeSize = n3.QuadPart /1024.0/1024.0/1024.0;	// 現在の空き容量 [GB] [B→GB]
	return dNowFreeSize;
}

//------------------------------------------
// 全長画像保存PCの空きエリアチェック
// char const* sFolder 対象フォルダ (最後の\は無し)
// double dLimit 空き容量 [GB]
// 戻り値	true:正常
//------------------------------------------
bool HddManager::CheckFreeSpace(char const* sFolder, double dLimit)
{
	double	dNowFreeSize = CheckFreeSpace(sFolder);		// 現在の空き容量 [GB]
	if( 0 > dNowFreeSize ) return true;					// 取得失敗の場合は、正常認識。

	if( dLimit <= (int)dNowFreeSize ) return true;		// 最低空き容量よりも大きい？
	return false;
}

//------------------------------------------
// 新旧ファイルのフォルダIndexを算出
// int nIns 対象インスタンス 0～:対象インスタンスのみ
// bool bIsNew		ture:最新算出 false:最古算出
// 戻り値：フォルダIndex
//------------------------------------------
int HddManager::SearchFolderIndex(int nIns, bool bIsNew)
{
	int		nIndex = 0;
	CTime	tm;
	CString sWk;

	//// 前準備
	ParamManager::P_MAIN::INS    const& prmI = mcls_pParam->GetMain().ins[ nIns ];
	if( 1 >= prmI.nRpiFolderSaveNum ) return 0;			// 1つしかない場合は、無条件に確定
	if(bIsNew)	tm = 0;
	else		tm = CTime::GetCurrentTime();

	//// 対象フォルダサーチ
	for(int ii=0; ii<prmI.nRpiFolderSaveNum; ii++) {
	
		// フォルダ内のRPIファイル一覧を取得
		sWk.Format("%s\\*.%s", prmI.cRpiFolderSave[ii], RPI_TYPE );
		WIN32_FIND_DATA	fd;
		HANDLE			hFind;
		memset(&fd, 0x00, sizeof(fd));
		hFind = FindFirstFile(sWk, &fd);
		while(hFind != INVALID_HANDLE_VALUE) {
			// ファイルのみ
			if( FILE_ATTRIBUTE_DIRECTORY != fd.dwFileAttributes ) {
				// どれ
				if( ( bIsNew && (tm < fd.ftLastAccessTime) ) ||
					(!bIsNew && (tm > fd.ftLastAccessTime) ) ) {
					
					tm		= fd.ftLastAccessTime;
					nIndex	= ii; 
				}
			}
			// 次
			if(! FindNextFile(hFind, &fd)) {
				if(ERROR_NO_MORE_FILES != GetLastError()) {
					LOG(em_ERR), "[%s] ファイル一覧取得失敗 %d", my_sThreadName, GetLastError());
				}
				break;
			}
		}
		FindClose(hFind);
	}
	
	LOG(em_INF), "[%s] Index=%dが対象。アクセス日付(%s) ", my_sThreadName, nIndex, tm.Format("'%Y/%m/%d %H:%M:%S'"));
	return nIndex;
}



//------------------------------------------
// ファイル有無をチェック
// char const* cFPath 取得したいファイルのフルパス
// 戻り値: false:異常  true:ファイル有り
//------------------------------------------
bool HddManager::GetFileUmu(char const* cFPath)
{
	//// ファイル関係 のデータ事前準備
	FILE*	fp = NULL;			// ファイルポインタ
	fpos_t	size;				// ファイルサイズ

	// ファイルオープン
	fp = fopen(cFPath, "rb");
	if(fp == NULL){        // オープン失敗
		return false;
	}
	// ファイルサイズ取得
	fclose(fp);
	
	return true;
}





