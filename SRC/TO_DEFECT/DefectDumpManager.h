// *********************************************************************************
//	疵情報非同期書込み クラス
//
//	[メモ]
//		
// *********************************************************************************
#pragma once

#pragma warning(disable : 4996)							// ***が古い形式として宣言されました。を出さない対策

#include <fstream>	
#include "..\..\Include\LineCommon.h"									// 疵検インクルード
#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// マクロ集
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// スタティックライブラリ集
#include "..\..\Library\KizuLibMFC\Class\CreateSQL.h"					// SQL文生成補助クラス


// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ワーカースレッドマネージャー


// EPCインクルード
//#include "..\..\Include\EPC\NCattr.h"
#include "..\..\Include\EPC\NCL2attr.h"									// 特徴量定義


// ローカルのクラス
#include "WarpMatchImage.h"												// 複眼ズレ補正処理
#include "MainConst.h"
#include "ParamManager.h"												// パラメータ管理クラス

#include <math.h>
#include <fstream>
#include <direct.h>

using namespace KizuLib;


class DefectDumpManager : public ThreadManager
{
public:
	DefectDumpManager(int TorB, LPCSTR filename="DEF_", int cycleCnt=5, int backCnt=4);
	virtual ~DefectDumpManager();


private:
	void	ThreadEvent(int nEventNo);									// スレッドイベント発生
	int		ThreadFirst();												// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int		ThreadLast();												// スレッド終了前処理 (終了シグナル後にコールバック)


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// ローカル定数
private:
	static const int DEFECTDUMPMANAGER_MAX_QUE		= 512;				// ログファイル最大数


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// スレッドキュー
public:

	// スレッドキュー セット用構造体
	struct	DEFDUMP_QUE {
		int		nRecord;												// 登録レコードNo
		LPSTR	pStr;													// 疵情報
	};

	ThreadQueue<DEFDUMP_QUE>	gque_Deli;								// スレッドキュー
	

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// 公開関数
public:

	// ログ出力
	void	Write( int nRecord, LPCSTR __cBuffer, ... );				// 非同期書き込みエントリー (可変引数対応)[←HtRecv]
	void	WriteLock( int nRecord, LPCSTR __cBuffer, ... );			// 同期書込み (可変引数対応)[★誰も使ってない]
	int		AllGetQueue();												// 全キュー強制出力[★誰も使ってない]
	int		AllDeleteQueue();											// 全キュー強制削除[←DefectDumpMgr]

	// その他もろもろ
	void	SetFileOut(bool umu) {m_bFileOut = umu;}					// ファイル出力有無[★誰も使ってない]
	void	FileNoClear() {m_nNowFileNo = -1;}							// ファイルNoの初期化(検査開始時に実行)[←HtRecv]
	CString	GetNowLogPath() {return m_cNowFPath;}						// 現在のログファイルの絶対パスを返す[★誰も使ってない]
	void	WriteFile(int nRecord, LPCSTR msg);							// ファイルに書き込み[←DefectDumpMgr]
	void	FlushFile();												// ファイルに強制出力[←DefectDumpMgr]
	int		Get_NowFileNo() {return m_nNowFileNo;}						// 現在ﾌｧｲﾙNo取り出し[←HtRecv, RealCntMgr]


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// メンバー変数
private:

	// ログ出力制御
	//ofstream				m_stream;									// ストリーム	// VS6
	std::ofstream			m_stream;									// ストリーム	// VS2003以降

	CRITICAL_SECTION		m_csOutPut;									// クリティカルセクション
	CRITICAL_SECTION		m_csANC;									// 非同期用クリティカルセクション

	bool					m_bFileOut;									// ファイル出力 (true:ファイル出力有り)

	// ログファイル関連
	CString					m_cBaseFolder;								// ベースのフォルダ
	CString					m_cBaseFile;								// ベースのファイル名
	CString					m_cNowFPath;								// 現在出力中のファイルフルパス
	int						m_nNowFileNo;								// ファイル数
	DWORD					m_nNowRow;									// 行数
	int						m_nMaxCycleCnt;								// 最大サイクリックファイル件数
	int						m_nMaxBackCnt;								// 最大バックアップ履歴数
};
