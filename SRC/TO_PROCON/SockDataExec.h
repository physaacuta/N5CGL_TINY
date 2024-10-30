// *********************************************************************************
//	プロコン通信 の 送受信データ受け渡し
//	[Ver]
//		Ver.01    2007/08/03  vs2005 対応
//
//	[メモ]
//		
// *********************************************************************************

#pragma once

// 半固定
#include "..\..\Include\LineCommon.h"									// 疵検インクルード
#include "..\..\Library\KizuLib\KizuLib.h"								// 疵検ライブラリ
#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// マクロ集
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// スタティックライブラリ集
#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ログ管理クラス

// 疵検クラス インクルード
#include "..\..\Library\KizuODBC\OdbcBase.h"							// DB操作クラス
#include "..\..\Library\KizuLibMFC\Class\CreateSQL.h"					// SQL文生成補助クラス
#include "..\..\Library\DefectLib\BaseTcpIp.h"							// TCP/IP操作クラス
#include "..\..\Library\DefectLib\DBSaveManager.h"						// DB書込み基本クラス

// メンバー
#include "MainConst.h"
#include "SockManager.h"
#include "..\..\Include\ToTbl.h"										// 統括共有メモリテーブル

using namespace KizuLib;
using namespace KizuODBC;

class SockDataExec : public SockManager
{
//// 公開関数
public:
	SockDataExec(void);
	virtual ~SockDataExec(void);

	void Alloc();														// 関数初期化
	void Free();														// 関数開放

	void SetLogMgr(LogFileManager* pCls) { mcls_pLog = pCls; SockManager::SetLogMgr(pCls); };	// ログファイルセット 


	//// 外部アクセス
	// 送信関係
	bool SetSendDataAnser(Delivery* deli, bool bAns);					// 送信データ 事後処理
	bool SetRecvData(int tc, int size, BYTE * data);					// 受信データ 事後処理
	bool AddQue_Ziseki(COMMON_QUE const* pQue);							// 疵検査実績伝文 送信依頼 [←Dlg]

	// 外部操作
	bool ReadBufFile(int nTc, BYTE * data, char* cFileName, int nMode = 0);		// バッファファイルを読込


//// メンバー関数
private:

	//// 対プロコンI/F関連
	void	ByteHantenProDataRecv(int nTc, BYTE * data);				// プロコン受信伝文 バイト反転
	void	ByteHantenProDataSend(int nTc, BYTE * data);				// プロコン送信伝文 バイト反転

	//// その他
	bool	InsertBufProRData(int nTc, BYTE * data);					// 受信伝文のBUFテーブル登録



//// メンバー変数
private:
	//// 各インスタンス
	LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス
	DBSaveManager*			mcls_pDbs;									// DB書込みクラス	
	ThreadQueue<DBSaveManager::DeliverySQL> mque_AddDB;					// 受け渡しデータ用キュー (ここに入ったキューは全て登録する)


	// 共有メモリ情報
	HANDLE					m_hCoil;									// コイル情報 ハンドル
	TO_COIL_TBL*			mtbl_pCoil;									// コイル情報 データ
	
	// デバック用
	PRO_TYPE_CUT            mtyp_Cut;									// カット情報保存
};
