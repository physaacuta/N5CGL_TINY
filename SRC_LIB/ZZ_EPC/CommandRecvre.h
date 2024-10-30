#pragma once

// 半固定
#include "..\..\Include\LineCommon.h"									// 疵検インクルード
#include "..\..\Library\KizuLib\KizuLib.h"								// 疵検ライブラリ
#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// マクロ集
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// スタティックライブラリ集
#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ログ管理クラス

// 疵検クラス インクルード
#include "..\..\Library\KizuLib\Class\WorkThread.h"						// ワーカースレッドクラス
#include "..\..\include\ToHtCommon.h"									// 統括判定用の定義ヘッダ

// EPCインクルード
//#include "..\..\Include\EPC\NCcommand.h"								// 制御コマンドの定義
#include "..\..\Include\EPC\NCL2DETECT_if.h"							// I/F定義


#include "DataSender.h"

using namespace KizuLib;


class CommandRecvre : public WorkThread
{
public:
	CommandRecvre(void);
	virtual ~CommandRecvre(void);


	void Start() { WorkThread::Start(MainThread); };					// スレッド開始
	void Stop()  { WorkThread::Stop(); };								// スレッド終了
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; };			// ログファイルセット
	
	void SetDataSender(DataSender* pCls) { mcls_pData = pCls; };			//


	int SendCommand(LPVOID senddata, DWORD sendsize);

	void SetExp(int cam, float* exp) { for (int ii = 0; ii<3; ii++)m_exptime[ii][cam] = exp[ii]; }
	void SetGain(int cam, float* gain) { for (int ii = 0; ii<3; ii++)m_gain[ii][cam] = gain[ii]; }
	void SetTemp(int cam, float* temp) { m_temp[cam] = temp[cam]; }
	void SetVolt(int cam, float* volt) { m_volt[cam] = volt[cam]; }

	float GetExp(int color, int cam) { return m_exptime[color][cam]; }
	float GetGain(int color, int cam) { return m_gain[color][cam]; }
	float GetTemp(int cam) { return m_temp[cam]; }
	float GetVolt(int cam) { return m_volt[cam]; }

private :
	static UINT WINAPI MainThread(LPVOID param);						// メインスレッド

	void GetCommandExec(int command, BYTE* pBuf);									// コマンド解釈

	//// 各インスタンス
	LogFileManager*			mcls_pLog;									// LogFileManagerスレッドインスタンス
	DataSender*				mcls_pData;									// データ送信

	HANDLE					m_hPipe;									// パイプ本体
	OVERLAPPED				m_ovlSend;									// 受信用非同期IO
	int						m_RetCode;									// リターンコード構造体
	NCL_ANSWER_QUERY_STATE	m_RetState;									// リターンコード構造体(NCL_QUERY_STATUS)

	float					m_exptime[3][NCL_ncam_max];					// 色別露光時間
	float					m_gain[3][NCL_ncam_max];					// 色別ゲイン
	float					m_temp[NCL_ncam_max];						// カメラ温度
	float					m_volt[NCL_ncam_max];						// カメラ電圧
};
