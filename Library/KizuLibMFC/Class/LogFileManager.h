// *********************************************************************************
//	LOG非同期書込み クラス
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応 (コンストラクタでサイズ指定に変更)
//
//	[メモ]
//		MFCのListBoxにも ログを表示する事が可能
// *********************************************************************************

#if !defined(AFX_LOGFILEMANAGER_H__CA3C4798_AC15_40E3_9772_335FAA1A0EF5__INCLUDED_)
#define AFX_LOGFILEMANAGER_H__CA3C4798_AC15_40E3_9772_335FAA1A0EF5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable : 4996)							// ***が古い形式として宣言されました。を出さない対策

#include <fstream>	
#include "..\..\KizuLib\Class\ThreadQueue.h"					// キュークラス
#include "..\..\KizuLib\Class\WorkThread.h"						// ワーカースレッドクラス
#include "..\..\KizuLib\Class\AutoLock.h"						// クリティカルセクションクラス

namespace KizuLib
{
	class LogFileManager : public WorkThread
	{
	private:
		
		static const int KIZULIB_LOGFILEMANAGER_MAX_ROW_NUM	= 20000;		// 1ファイルあたりの行数
		static const int KIZULIB_LOGFILEMANAGER_MAX_LISTBOX	= 200;			// リストボックス表示行数
		static const int KIZULIB_LOGFILEMANAGER_MAX_QUE		= 512;			// ログファイル最大数

	public:
		
		// //////////////////////////////////////////////////////////////////////////////
		// 定数定義
		// //////////////////////////////////////////////////////////////////////////////
		//// 受け渡し構造体の番号
		typedef enum LNO {
			em_NON = 0,							// 無し
			em_MSG,								// ☆ 
			em_ERR,								// ★
			em_INF,								// ○ (通常 共通クラス等で出力)
			em_WAR,								// ● (通常 共通クラス等で出力)
			em_FIL								// ■ 
		}lNo;


		// //////////////////////////////////////////////////////////////////////////////
		// メソッド
		// //////////////////////////////////////////////////////////////////////////////
		LogFileManager(LPCSTR filename, CListBox *list = NULL, int cycleCnt=3, int backCnt=4);		// コンストラクタ
		virtual ~LogFileManager();											// デストラクタ

		// ログ出力
		void Write( LogFileManager::LNO no, LPCSTR __cBuffer, ... );		// 非同期書き込みエントリー (可変引数対応)
		void WriteLock( LogFileManager::LNO no, LPCSTR __cBuffer, ... );	// 同期書込み (可変引数対応)
		int AllGetQueue();													// 全キュー強制出力
		int AllDeleteQueue();												// 全キュー強制削除

		// その他もろもろ
		void SetListOut(bool umu) {m_bListOut = umu; AllGetQueue(); Sleep(100); };	// リストボックスコントロール出力有無
		void SetFileOut(bool umu) {m_bFileOut = umu;}								// ファイル出力有無
		CString GetNowLogPath() {return m_cNowFPath;}						// 現在のログファイルの絶対パスを返す
		void ShowEdit();													// メモ帳起動
		void WriteFile(LPCSTR msg);											// ファイルに書き込み

	private:
		// 非同期関連
		static UINT WINAPI MainThread(LPVOID param);						// メインスレッド
		void Start() { WorkThread::Start(MainThread); };					// スレッド開始
		void Stop()  { WorkThread::Stop(10000); };							// スレッド終了


		// ログ出力関連
		void WriteList(LPCSTR msg);											// リストボックスに書き込み

		
		// ログ出力制御
		//ofstream				m_stream;									// ストリーム	// VS6
		std::ofstream			m_stream;									// ストリーム	// VS2003以降
		CListBox*				m_cntList;									// リストボックス

		CRITICAL_SECTION		m_csOutPut;									// クリティカルセクション
		CRITICAL_SECTION		m_csANC;									// 非同期用クリティカルセクション

		ThreadQueue<char>		mque_Log;									// ログキュー
		bool					m_bFileOut;									// ファイル出力 (true:ファイル出力有り)
		bool					m_bListOut;									// リストボックスコントロール出力 (true:リスト出力有り)

		// ログファイル関連
		CString					m_cBaseFolder;								// ベースのフォルダ
		CString					m_cBaseFile;								// ベースのファイル名
		CString					m_cNowFPath;								// 現在出力中のファイルフルパス

		DWORD					m_nNowFileNo;								// ファイル数
		DWORD					m_nNowRow;									// 行数
		int						m_nMaxCycleCnt;								// 最大サイクリックファイル件数
		int						m_nMaxBackCnt;								// 最大バックアップ履歴数
	
	};
};
#endif // !defined(AFX_LOGFILEMANAGER_H__CA3C4798_AC15_40E3_9772_335FAA1A0EF5__INCLUDED_)
