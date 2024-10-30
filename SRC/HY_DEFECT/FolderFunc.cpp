#include "StdAfx.h"
#include "FolderFunc.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


using namespace KizuLib;

//------------------------------------------
// コンストラクタ
//------------------------------------------
FolderFunc::FolderFunc(void) :
FuncBase("FolderFunc")
{
}

//------------------------------------------
// デストラクタ
//------------------------------------------
FolderFunc::~FolderFunc(void)
{
}


//------------------------------------------
// サブディレクトリまで一気に作成
// char const* cPath フルパス  ※最後の\ 必須
// 戻り値 復帰情報  ※繰り返し作っても異常にならない
//------------------------------------------
bool FolderFunc::CreateFolder(char const* cPath)
{
	if( MakeSureDirectoryPathExists(cPath) ) return true;
	else return false;
}


//------------------------------------------
// フォルダ有無チェック
// char const* cPath チェックフォルダのフルパス
//------------------------------------------
bool FolderFunc::CheckFolderUmu(char const* cPath)
{
	CFileFind fFind;
	//検索用のワイルドカード付きパスを用意
	CString sPath = cPath;
	sPath.TrimRight(_T("\\"));
	sPath += _T("\\*.*");

	if( ! fFind.FindFile( sPath, 0)) return false;
	
	fFind.Close();
	return true;
}

//------------------------------------------
// ファイル有無チェック
// char const* cFPath 取得したいファイルのフルパス
// 戻り値: false:異常  true:ファイル有り
//------------------------------------------
bool FolderFunc::CheckFileUmu(char const* cFPath)
{
	CFileFind fFind;
	CString sPath = cFPath;

	if( ! fFind.FindFile( sPath, 0)) return false;
	
	fFind.Close();
	return true;
}

//------------------------------------------
// ファイルアクセスチェック
// char const* cFPath 取得したいファイルのフルパス
// 戻り値: false:異常  true:ファイルアクセス可
//------------------------------------------
bool FolderFunc::CheckFileAccess(char const* cFPath)
{
	//// ファイル関係 のデータ事前準備
	FILE*	fp = NULL;			// ファイルポインタ

	// ファイルオープン
	fp = fopen(cFPath, "rb");
	if(fp == NULL){        // オープン失敗
		return false;
	}
	// ファイルサイズ取得
	fclose(fp);
	
	return true;
}

//------------------------------------------
// フォルダ以下を削除する関数
// char const* cPath 削除するフォルダのフルパス
//------------------------------------------
bool FolderFunc::DeleteFolder(char const* cPath)
{
	CFileFind fFind;

	//検索用のワイルドカード付きパスを用意
	CString sPath = cPath;
	sPath.TrimRight(_T("\\"));
	sPath += _T("\\*.*");

	if( ! fFind.FindFile( sPath )) return false;
	BOOL bRet=true;
	while(bRet){
		bRet=fFind.FindNextFile();
		if(fFind.IsDots()) continue;

		// GetFilePath を使うと
		// 正常にパスが取得できない場合があるので
		// フルパスを作成する処理を入れる
		CString sWk;
		sWk.Format(_T("%s\\%s"), cPath, fFind.GetFileName());
		// フォルダがあれば再帰
		// ファイルなら削除
		if(fFind.IsDirectory()){
			DeleteFolder(sWk);
		}else{
			DeleteFile(sWk);
		}
	}
	fFind.Close();

	return RemoveDirectory(cPath);
}


//------------------------------------------
// *指定 or フルパス指定 ファイルを 削除する関数
// char const* cPath 削除するファイルのフルパス
//------------------------------------------
bool FolderFunc::DeleteFileAll(char const* cPath)
{
	CFileFind fFind;
	CString sPath = cPath;

	if( ! fFind.FindFile( sPath, 0)) return false;
	BOOL bRet=true;
	while(bRet){
		bRet=fFind.FindNextFile();
		if(fFind.IsDots()) continue;

		// ファイルなら削除
		if(fFind.IsDirectory()){

		}else{
			DeleteFile( fFind.GetFilePath());
		}
	}
	fFind.Close();
	return true;
}

//------------------------------------------
// 指定フォルダの空きエリアチェック
// char const* sFolder 対象フォルダ (最後の\は無し)
// 戻り値	0～：現在の空き容量 [GB]	0>:失敗
//------------------------------------------
double FolderFunc::CheckFreeSpace(char const* sFolder)
{
	CString	sWk;

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
// フォルダ一覧を取得
// bool					bFolder	true:フォルダ  false:ファイル
// char const*			cRoot	検索パス (フォルダ時[aa\\*]  ファイル時[aa\\*.csv])
// std::vector<F_ITEM>* vItem	取得アイテム
// int nSort					並び替えオプション (-1:古い順 0:無し 1:新しい順) ※書込み日付でソート
//------------------------------------------
void FolderFunc::GetList(bool bFolder, char const* cRoot, std::vector<F_ITEM>* vItem, int nSort)
{
	// =========================================================
	// フォルダ内の古いもの順のRPIファイル一覧を生成
	WIN32_FIND_DATA	fd;
	HANDLE			hFind;
	memset(&fd, 0x00, sizeof(fd));
	hFind = FindFirstFile(cRoot, &fd);
	while(hFind != INVALID_HANDLE_VALUE) {
		// 特殊は対象外
		if( 0==strcmp(".", fd.cFileName ) ) goto NEXT;
		if( 0==strcmp("..", fd.cFileName ) ) goto NEXT;

		// 対象のみ
		//if( ( bFolder && 0 != (FILE_ATTRIBUTE_DIRECTORY & fd.dwFileAttributes) ) ||			// フォルダ
		//	(!bFolder && 0 == (FILE_ATTRIBUTE_DIRECTORY & fd.dwFileAttributes) ) ) {		// ファイル
		if( ( bFolder && (FILE_ATTRIBUTE_DIRECTORY == fd.dwFileAttributes) ) ||			// フォルダ
			(!bFolder && (FILE_ATTRIBUTE_DIRECTORY != fd.dwFileAttributes) ) ) {		// ファイル

			// リストに追加
			F_ITEM t;
			t.sName				= fd.cFileName;
			t.tTime				= fd.ftLastWriteTime;
			vItem->push_back(t);
		}


		// 次
NEXT:
		if(! FindNextFile(hFind, &fd)) {
			if(ERROR_NO_MORE_FILES != GetLastError()) {
				LOG(em_ERR), "[%s] ファイル一覧取得失敗 %d", my_sThreadName, GetLastError());
			}
			break;
		}
	}
	FindClose(hFind);

	// =========================================================
	// ソート
	if(-1 == nSort)		{ std::sort(vItem->begin(), vItem->end(), sort_fd_old); }
	else if(1 == nSort) { std::sort(vItem->begin(), vItem->end(), sort_fd_new); }

}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 小物

//// ソートの比較関数
//------------------------------------------
// 古いやつを見つける。Aが古い場合True
//------------------------------------------
inline bool FolderFunc::sort_fd_old(const F_ITEM &a, const F_ITEM &b)
{
	return a.tTime < b.tTime;
}
//------------------------------------------
// 新しいやつを見つける。Aが新しい場合True
//------------------------------------------
inline bool FolderFunc::sort_fd_new(const F_ITEM &a, const F_ITEM &b)
{
	return a.tTime > b.tTime;
}


//------------------------------------------
// Pingチェック
// char const* srv  サーバーアドレス
// bool bIpAddr true:IPアドレス   false:ホスト名
// 戻り値 -:異常 1:接続済み 0:正常接続
//------------------------------------------
bool FolderFunc::PingCheck(char const* srv, bool bIpAddr)
{
	PingManager clsPing;

	char cIp[256];
	memset(cIp, 0x00, sizeof(cIp));

	// IPアドレスに変換
	if(bIpAddr) {
		memcpy(cIp, srv, sizeof(cIp));
	} else {
		if( ! clsPing.HostToIp( srv, cIp )) return false;
	}

	return clsPing.Ping(cIp);
}
