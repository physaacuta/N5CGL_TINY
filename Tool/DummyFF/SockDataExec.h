// *********************************************************************************
//	FF通信データの受渡し
//	[Ver]
//		Ver.01    2012/08/30
//
//	[メモ]
//		
// *********************************************************************************

#pragma once

// 疵検クラス インクルード
#include "..\..\Library\KizuLibMFC\CLASS\FuncBase.h"					// ファンクション用のベースクラス
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// スタティックライブラリ集
#include "..\..\Library\DefectLib\BaseTcpIp.h"							// TCP/IP操作クラス
#include "..\..\Library\KizuLibMFC\Class\CreateSQL.h"					// SQL文生成補助クラス

// メンバー
#include "MainConst.h"

using namespace KizuLib; 
using namespace KizuODBC;

class SockDataExec : public FuncBase
{
public:
	enum FF_SENDKIND_NO {												// 伝文区分
		FF_SENDKIND_OK = 0, 											// 正常送信完了時
		FF_SENDKIND_NON = 1, 											// 未送信
		FF_SENDKIND_ERR = -1, 											// 送信失敗（再送信あり）
		FF_SENDKIND_NOTSEND = -9 										// 再送信なし
	};

//// 公開関数
public:
	SockDataExec(void);
	virtual ~SockDataExec(void);

	//// 外部アクセス
	// 送信関係
	bool GetSendData(Delivery* deli, ExecItem* item);					// 送信データ取得


	//// Dump出力関係
	void SetDumpMode(bool bmode)	{m_bDumpMode = bmode; }				// ダンプ出力モードセット
	bool GetDumpMode() const		{return m_bDumpMode; }				// ダンプ出力モード取得

	//// テストデータ読み込みモード関係
	void SetReadDataMode(bool bmode)	{m_bReadDataMode = bmode; }				// テストデータ読み込みモードセット
	bool GetReadDataMode() const		{return m_bReadDataMode; }				// テストデータ読み込みモード取得

	bool ReadTestFile( BYTE* cSendData, int nSendSize, char const* cfullname );		// テストデータ読み込み

//// 公開変数
public:


//// メンバー変数
private:
	//// ハンドル

	//// 各インスタンス

	//// ステータス
	bool                    m_bDumpMode;								// ダンプ出力モード(True:全件出力,False:1件出力)
	bool                    m_bReadDataMode;							// テストデータ読み込みモード(True:DB,False:ファイル)


};
