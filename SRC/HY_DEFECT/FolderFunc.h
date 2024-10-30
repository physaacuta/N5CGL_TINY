//=====================================================================
// フォルダ、ファイル操作系のクラス群
//	【全ライン】
//		・基本的に完全流用可能なように作る
//=====================================================================
#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\FuncBase.h"					// モジュールの基底クラス
#include "..\..\Library\KizuLib\CLASS\PingManager.h"				


// 標準
#include <direct.h>
// STL
#include <vector>														// リスト用
#include <algorithm>													// ソート用

// 追加
#include <imagehlp.h>
#pragma comment(lib, "imagehlp.lib")					// サブディレクトリまで一気に作る

namespace KizuLib
{
	class FolderFunc  : public FuncBase
	{

	public:
		struct F_ITEM {
			CString			sName;			// 名前 (フルパスじゃ無い)
			CTime			tTime;			// 書込み日付
		};

	public:
		FolderFunc(void);
		virtual ~FolderFunc(void);

		// 自PC内操作
		bool CreateFolder(char const* cPath);								// サブディレクトリまで一気に作成
		bool CheckFolderUmu(char const* cPath);								// フォルダ有無チェック
		bool CheckFileUmu(char const* cFPath);								// ファイル有無チェック
		bool CheckFileAccess(char const* cFPath);							// ファイルアクセスチェック
		bool DeleteFolder(char const* cPath);								// フォルダ以下を削除する関数
		bool DeleteFileAll(char const* cPath);								// *指定 or フルパス指定 ファイルを 削除する関数

		double CheckFreeSpace(char const* sFolder);							// 指定フォルダの空きエリアチェック
		void GetList(bool bFolder, char const* cRoot, std::vector<F_ITEM>* vItem, int nSort);	// フォルダ一覧を取得
		
		
		// 付帯
		static bool PingCheck(char const* srv, bool bIpAddr = true);					// Pingチェック


	private:
		static inline bool sort_fd_old(const F_ITEM &a, const F_ITEM &b);	// ソート関数
		static inline bool sort_fd_new(const F_ITEM &a, const F_ITEM &b);	// ソート関数


	};
};