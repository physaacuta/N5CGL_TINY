// PLGReceive.h: PLGReceive クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLGRECEIVE_H__1D3BDA8B_1C93_4A46_90A6_0641DA079BFF__INCLUDED_)
#define AFX_PLGRECEIVE_H__1D3BDA8B_1C93_4A46_90A6_0641DA079BFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// インクルード
#include <winsock2.h>

// 半固定
#include "..\..\Include\LineCommon.h"									// 疵検インクルード
#include "..\..\Library\KizuLib\KizuLib.h"								// 疵検ライブラリ
#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// マクロ集
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// スタティックライブラリ集
#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ログ管理クラス

// 疵検クラス インクルード
#include "..\..\Library\KizuLib\Class\WorkThread.h"						// ワーカースレッドクラス


// EPCインクルード
//#include "..\..\Include\EPC\NCspeed.h"
#include "..\..\Include\EPC\NCL2DETECT_if.h"							// I/F定義
#include "..\..\Include\ToHtCommon.h"

#include "DataSender.h"


using namespace KizuLib;

class PLGReceive : public WorkThread  
{
public:
	PLGReceive();
	virtual ~PLGReceive();

	void Start() { WorkThread::Start(MainThread); };					// スレッド開始
	void Stop()  { WorkThread::Stop(); };								// スレッド終了
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; };			// ログファイルセット

	void SetDataSender(DataSender* pCls) { mcls_pData = pCls; };			// ログファイルセット


private :
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド

	// その他もろもろ
	int		m_nSock;								// UDP受信ソケット
	
	// シグナル
	HANDLE	m_evReset;								// 受信待ち状態にリセットする
	
	//// 各インスタンス
	LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス
	DataSender*				mcls_pData;									// データ送信

};

#endif // !defined(AFX_PLGRECEIVE_H__1D3BDA8B_1C93_4A46_90A6_0641DA079BFF__INCLUDED_)
