#include "StdAfx.h"
#include "TempManager.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// コンストラクタ
// char const* cName 名前 (iniファイルのキーとなる)
//------------------------------------------
TempManager::TempManager(char const* cName) :
ThreadManager(cName),
m_emMainMode(MODE_NONE),
m_nSubMode(0),
m_nUnitMode(0),
m_bIsConnect(DIV_BOOL_NON),
m_bIsInital(DIV_BOOL_NON),
//m_bIsInital(false),
mcls_p232c(NULL)
{
	char	 cWk[64];
	CString  sWk;
	CString  sSession;

	//// 自分の設定キーを決定
	m_cMySession.Format("%s_%s", KT_TEMPMT, cName );
	m_nUnitNum		= GetPrivateProfileInt(m_cMySession, "UNIT_NUM",	0, TASKINI_NAME);
	_ASSERT(0!=m_nUnitNum);
	m_nCycleTime	= GetPrivateProfileInt(m_cMySession, "TIMER_CYCLE",	0, TASKINI_NAME);
	m_nCycleTime	*= 1000;
	m_nTempKeta		= GetPrivateProfileInt(m_cMySession, "TEMP_KETA",	0, TASKINI_NAME);


	//// 機器IDを取得
	m_nIdConnect	= GetPrivateProfileInt(m_cMySession, "CONNECT_ID",	0, TASKINI_NAME);
	for(int ii=0; ii<m_nUnitNum; ii++) {
		sSession.Format("%s_%s_UNIT%d", KT_TEMPMT, cName, ii+1 );

		GetPrivateProfileString(sSession, "NAME", "", cWk, sizeof(cWk), TASKINI_NAME);
		m_cUnitName[ii]		= cWk;
		m_nTempLimit[ii]	= GetPrivateProfileInt(sSession, "TEMP_LIMIT",	0, TASKINI_NAME);	
		m_nIdTemp[ii]		= GetPrivateProfileInt(sSession, "TEMP_ID",		0, TASKINI_NAME);		
		m_nIdTempSts[ii]	= GetPrivateProfileInt(sSession, "TEMP_ST_ID",	0, TASKINI_NAME);	
		m_nIdUnten[ii]		= GetPrivateProfileInt(sSession, "UNTEN_ID",	0, TASKINI_NAME);	
		m_nIdState[ii]		= GetPrivateProfileInt(sSession, "STATE_ID",	0, TASKINI_NAME);	
	}


	//// 初期化
	for(int ii=0; ii<MAX_TEMP_UNIT; ii++) {
		m_emUnten[ii]= DIV_BOOL_NON;
		m_nStatus[ii] = -1;
		m_dTemp[ii] = 0.0;
// --->>> 20180313 BUGFIX Ver(1.0.0.5 → 1.1.0.0)
//		m_bTempSt[ii] = false;
		m_bTempSt[ii] = true;
// <<<--- 20180313 BUGFIX Ver(1.0.0.5 → 1.1.0.0)
	}

	//// ハンドル生成
	m_evRecvEnd = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evSendEnd = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evReTry	= CreateWaitableTimer(NULL, FALSE, NULL);
	m_evAnserTimeOut= CreateWaitableTimer(NULL, FALSE, NULL);
	m_evCycleExec	= CreateWaitableTimer(NULL, FALSE, NULL); 
}

//------------------------------------------
// デストラクタ
//------------------------------------------
TempManager::~TempManager(void)
{
	//// ハンドル解放
	CloseHandle(m_evRecvEnd);
	CloseHandle(m_evSendEnd);
	CloseHandle(m_evReTry);
	CloseHandle(m_evAnserTimeOut);
	CloseHandle(m_evCycleExec);
	// クローズ
	if(NULL != mcls_p232c) {
		SAFE_DELETE(mcls_p232c);
	}
}


//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int TempManager::ThreadFirst()
{
	Sleep(50);
	this->SetTid();

	//// RS-232C 初期接続
	if( ! Open(true) ) {
		Close();
	}

	////// 事前準備

	//// イベント設定
	HANDLE hArray[] = { m_evReTry,
						m_evSendEnd,
						m_evRecvEnd,
						m_evAnserTimeOut,
						m_evCycleExec
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int TempManager::ThreadLast()
{
	// 定周期停止
	CancelWaitableTimer(m_evReTry);
	CancelWaitableTimer(m_evAnserTimeOut);
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void TempManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_RETRY = 0,					// 再接続開始
			EV_SENDEND,						// 送信完了
			EV_RECVEND,						// 受信完了
			EV_ANSERTIMEOUT,				// 送信アンサータイムアウト
			EV_CYCLEEXEC					// 定周期実行
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_RETRY: 							// 再接続開始
		if( ! Open(false) ){
			Close();
		}
		return;

//-----------------------------------------------------------------------------------------------
	case EV_SENDEND: 						// 送信完了
		// 特に何もしない
		return;

//-----------------------------------------------------------------------------------------------
	case EV_RECVEND: 						// 受信完了
		if( ! CheckDataRecv() ) {
			Close();
			return;
		}
		// 正常に受信できたので次の非同期受信開始
		if( ! Recv() ) {
			return;
		}
		return;

//-----------------------------------------------------------------------------------------------
	case EV_ANSERTIMEOUT: 					// 送信アンサータイムアウト
		if( DIV_BOOL_TRUE == m_bIsConnect) {
			LOG(em_ERR), "[%s] 応答伝文タイムアウト <%s>:<%s>", my_sThreadName, GetMainModeName(), GetUnitName(m_nUnitMode-1));
			//syslog(0, "[<%s>:<%s>:<%s>]", my_sThreadName, GetMainModeName(), GetUnitName(m_nUnitMode-1));
		}
//		Close();					// 再接続

// --->>> 20180313 BUGFIX Ver(1.0.0.5 → 1.1.0.0)
//		if( ! CheckNextSend() ) return ;
		if (!CheckNextSend()) {
			Close();				// 再接続
		}
// <<<--- 20180313 BUGFIX Ver(1.0.0.5 → 1.1.0.0)
		return;

//-----------------------------------------------------------------------------------------------
	case EV_CYCLEEXEC: 						// 定周期実行
		if( DIV_BOOL_TRUE == m_bIsConnect) {
			m_emMainMode = MODE_READ_STATUS;
			if( ! CheckNextSend() ) {
				Close();			// 再接続		// 既設はNG
			}
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
// 温度パネルメータ制御

//------------------------------------------
// 次送信制御
// 戻り値 false:回線切断処理を実施
//------------------------------------------
bool TempManager::CheckNextSend()
{
	//// 前準備
	memset(m_cSendBuf, 0x00, sizeof(m_cSendBuf));		// 送信バッファ クリア

	//// 処理分け
	switch( m_emMainMode ) {
	
	//======================================================
	// 初期準備確認中
	case MODE_OPEN_CHECK:
		// サブモード変更 (機器毎の設定をしている場合は、サブモードを維持)
		if( m_nUnitNum <= m_nUnitMode ) m_nUnitMode = 0;			// 最後の機器時
		if( 0 == m_nUnitMode ) m_nSubMode++;						// 次のサブモードへ

		//---------------------------------------------
		if( 1==m_nSubMode ) {										// エコーバックテスト
			LOG(em_MSG), "[%s] ---------------------------------------- エコーバックテスト 開始 [UnitNo=%d]", my_sThreadName, m_nUnitMode + 1);

			CMD_TYPE_ECHO_BACK* buf = (CMD_TYPE_ECHO_BACK *)m_cSendBuf;
			memset(buf->cSTX,		STX_CODE,	sizeof(buf->cSTX));
			sprintf(buf->cNodeNo,	"%02d",		m_nUnitMode + 1);
			memcpy(buf->cSubAddr,	"00",		sizeof(buf->cSubAddr));
			memcpy(buf->cServiceID,	"0",		sizeof(buf->cServiceID));
			// FINS-mini ------------------------------------------------>>>
			memcpy(buf->cMRC,		"08",		sizeof(buf->cMRC));
			memcpy(buf->cSRC,		"01",		sizeof(buf->cSRC));
			// FINS-mini <<<------------------------------------------------
			
			memset(buf->cETX,		ETX_CODE,	sizeof(buf->cETX));
			// BCCクリエイト
			CreateBCC((char*)buf, sizeof(CMD_TYPE_ECHO_BACK));

			// 機器毎の設定
			m_nUnitMode++;

		////---------------------------------------------
		//} else if( 2==m_nSubMode ) {
		//	LOG(em_MSG), "[%s] ---------------------------------------- コントローラステータス読出し 開始 [UnitNo=%d]", my_sThreadName, m_nUnitMode + 1);

		//	CMD_TYPE_CONTROLER_STATUS* buf = (CMD_TYPE_CONTROLER_STATUS *)m_cSendBuf;
		//	memset(buf->cSTX,		STX_CODE,	sizeof(buf->cSTX));
		//	sprintf(buf->cNodeNo,	"%02d",		m_nUnitMode + 1);
		//	memcpy(buf->cSubAddr,	"00",		sizeof(buf->cSubAddr));
		//	memcpy(buf->cServiceID,	"0",		sizeof(buf->cServiceID));
		//	// FINS-mini ------------------------------------------------>>>
		//	memcpy(buf->cMRC,		"06",		sizeof(buf->cMRC));
		//	memcpy(buf->cSRC,		"01",		sizeof(buf->cSRC));
		//	// FINS-mini <<<------------------------------------------------
		//	memset(buf->cETX,		ETX_CODE,	sizeof(buf->cETX));
		//	// BCCクリエイト
		//	CreateBCC((char*)buf, sizeof(CMD_TYPE_CONTROLER_STATUS));

		//	// 機器毎の設定
		//	m_nUnitMode++;

		//---------------------------------------------
		} else if( 2==m_nSubMode ) {
			// 準備完了
			m_bIsInital = DIV_BOOL_TRUE;

			// 機器状態変更
			if(0 != m_nIdConnect) KizuFunction::SetStatus(m_nIdConnect, true);

			// 待機
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nUnitMode = 0;

			// 接続完了した為、タイマーセット
			LARGE_INTEGER ts;
			ts.QuadPart = (__int64)1000 * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
			SetWaitableTimer(m_evCycleExec, &ts, 0, NULL, NULL, FALSE);
		}
		break;


	//======================================================
	// コントローラステータス読出し中
	case MODE_READ_STATUS:
		// サブモード変更 (機器毎の設定をしている場合は、サブモードを維持)
		if( m_nUnitNum <= m_nUnitMode ) m_nUnitMode = 0;			// 最後の機器時
		if( 0 == m_nUnitMode ) m_nSubMode++;						// 次のサブモードへ

		//---------------------------------------------
		if( 1==m_nSubMode ) {										// コントローラステータス読出し
/*
			LOG(em_MSG), "[%s] ---------------------------------------- コントローラステータス読出し 開始 [UnitNo=%d]", my_sThreadName, m_nUnitMode + 1);

			CMD_TYPE_CONTROLER_STATUS* buf = (CMD_TYPE_CONTROLER_STATUS *)m_cSendBuf;
			memset(buf->cSTX,		STX_CODE,	sizeof(buf->cSTX));
			sprintf(buf->cNodeNo,	"%02d",		m_nUnitMode + 1);
			memcpy(buf->cSubAddr,	"00",		sizeof(buf->cSubAddr));
			memcpy(buf->cServiceID,	"0",		sizeof(buf->cServiceID));
			// FINS-mini ------------------------------------------------>>>
			memcpy(buf->cMRC,		"06",		sizeof(buf->cMRC));
			memcpy(buf->cSRC,		"01",		sizeof(buf->cSRC));
			// FINS-mini <<<------------------------------------------------
			memset(buf->cETX,		ETX_CODE,	sizeof(buf->cETX));
			// BCCクリエイト
			CreateBCC((char*)buf, sizeof(CMD_TYPE_CONTROLER_STATUS));

			// 機器毎の設定
			m_nUnitMode++;
*/
			LOG(em_MSG), "[%s] ---------------------------------------- コントローラステータス読出し 開始 [UnitNo=%d]", my_sThreadName, m_nUnitMode + 1);

			CMD_TYPE_CONTROLER_STATUS* buf = (CMD_TYPE_CONTROLER_STATUS *)m_cSendBuf;
			memset(buf->cSTX,		STX_CODE,	sizeof(buf->cSTX));
			sprintf(buf->cNodeNo,	"%02d",		m_nUnitMode + 1);
			memcpy(buf->cSubAddr,	"00",		sizeof(buf->cSubAddr));
			memcpy(buf->cServiceID,	"0",		sizeof(buf->cServiceID));
			// FINS-mini ------------------------------------------------>>>
			memcpy(buf->cMRC,		"06",		sizeof(buf->cMRC));
			memcpy(buf->cSRC,		"01",		sizeof(buf->cSRC));
			// FINS-mini <<<------------------------------------------------
			memset(buf->cETX,		ETX_CODE,	sizeof(buf->cETX));
			// BCCクリエイト
			CreateBCC((char*)buf, sizeof(CMD_TYPE_CONTROLER_STATUS));

			// 機器毎の設定
			m_nUnitMode++;


		//---------------------------------------------
		} else if( 2==m_nSubMode ) {
			LOG(em_MSG), "[%s] ---------------------------------------- モニタ値読出し 開始 [UnitNo=%d]", my_sThreadName, m_nUnitMode + 1);

			CMD_TYPE_MONITOR_READ* buf = (CMD_TYPE_MONITOR_READ *)m_cSendBuf;
			memset(buf->cSTX,		STX_CODE,	sizeof(buf->cSTX));
			sprintf(buf->cNodeNo,	"%02d",		m_nUnitMode + 1);
			memcpy(buf->cSubAddr,	"00",		sizeof(buf->cSubAddr));
			memcpy(buf->cServiceID,	"0",		sizeof(buf->cServiceID));
			// FINS-mini ------------------------------------------------>>>
			memcpy(buf->cMRC,		"01",		sizeof(buf->cMRC));
			memcpy(buf->cSRC,		"01",		sizeof(buf->cSRC));
			memcpy(buf->cKind,		"C0",		sizeof(buf->cKind));
			memcpy(buf->cAddr,		"0002",		sizeof(buf->cAddr));
			memcpy(buf->cBitPos,	"00",		sizeof(buf->cBitPos));
			memcpy(buf->cCount,		"0001",		sizeof(buf->cCount));
			// FINS-mini <<<------------------------------------------------
			memset(buf->cETX,		ETX_CODE,	sizeof(buf->cETX));
			// BCCクリエイト
			CreateBCC((char*)buf, sizeof(CMD_TYPE_MONITOR_READ));

			// 機器毎の設定
			m_nUnitMode++;


		//---------------------------------------------
		} else if( 3==m_nSubMode ) {
			//LOG(em_MSG), "[TempManager] コントローラステータス読出し 完了");

			// 待機
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nUnitMode = 0;

			// 接続完了した為、タイマーセット
			LARGE_INTEGER ts;
			ts.QuadPart = (__int64)m_nCycleTime * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
			SetWaitableTimer(m_evCycleExec, &ts, 0, NULL, NULL, FALSE);
		}
		break;

/*
	//======================================================
	// モニタ値読出し中
	case MODE_READ_MONITOR:
		// サブモード変更 (機器毎の設定をしている場合は、サブモードを維持)
		if( m_nUnitNum <= m_nUnitMode ) m_nUnitMode = 0;			// 最後の機器時
		if( 0 == m_nUnitMode ) m_nSubMode++;						// 次のサブモードへ

		//---------------------------------------------
		if( 1==m_nSubMode ) {										// モニタ値読出し
			LOG(em_MSG), "[%s] ---------------------------------------- モニタ値読出し 開始 [UnitNo=%d]", my_sThreadName, m_nUnitMode + 1);

			CMD_TYPE_MONITOR_READ* buf = (CMD_TYPE_MONITOR_READ *)m_cSendBuf;
			memset(buf->cSTX,		STX_CODE,	sizeof(buf->cSTX));
			sprintf(buf->cNodeNo,	"%02d",		m_nUnitMode + 1);
			memcpy(buf->cSubAddr,	"00",		sizeof(buf->cSubAddr));
			memcpy(buf->cServiceID,	"0",		sizeof(buf->cServiceID));
			// FINS-mini ------------------------------------------------>>>
			memcpy(buf->cMRC,		"01",		sizeof(buf->cMRC));
			memcpy(buf->cSRC,		"01",		sizeof(buf->cSRC));
			memcpy(buf->cKind,		"C0",		sizeof(buf->cKind));
			memcpy(buf->cAddr,		"0002",		sizeof(buf->cAddr));
			memcpy(buf->cBitPos,	"00",		sizeof(buf->cBitPos));
			memcpy(buf->cCount,		"0001",		sizeof(buf->cCount));
			// FINS-mini <<<------------------------------------------------
			memset(buf->cETX,		ETX_CODE,	sizeof(buf->cETX));
			// BCCクリエイト
			CreateBCC((char*)buf, sizeof(CMD_TYPE_MONITOR_READ));

			// 機器毎の設定
			m_nUnitMode++;

		//---------------------------------------------
		} else if( 2==m_nSubMode ) {
			//LOG(em_MSG), "[TempManager] モニタ値読出し 完了");

			// 待機
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nUnitMode = 0;

			// 接続完了した為、タイマーセット
			LARGE_INTEGER ts;
			ts.QuadPart = (__int64)m_nCycleTime * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
			SetWaitableTimer(m_evReTry, &ts, 0, NULL, NULL, FALSE);
		}
		break;
*/
	}


	//======================================================
	// 送信
	if( 0==strlen(m_cSendBuf) ) return true;			// 送信データ無しの場合は、正常を返す
	if( ! Send() ) return false;

	// 送信ログ
//◆
	LOG(em_MSG), "[%s] 送信 [%s]", my_sThreadName, m_cSendBuf);
	return true;
}

//------------------------------------------
// 受信伝文チェック
// RES_TYPE_ECHO_BACK* buf 受信伝文
//------------------------------------------
bool TempManager::Check_Echo_Back(RES_TYPE_ECHO_BACK* buf)
{
	int id = m_nUnitMode - 1;

	// MRC
	if( 0 != memcmp(buf->cMRC, "08", sizeof(buf->cMRC)) ) {
		LOG(em_ERR), "[%s] MRC 異常 [%.2s]", my_sThreadName, buf->cMRC);
		//syslog(353, "[<%s> %s ユニットNo=%.2s MRC=%.2s]", GetMainModeName(), my_sThreadName, buf->cNodeNo, buf->cMRC);
		return false;
	}
	// SRC
	if( 0 != memcmp(buf->cSRC, "01", sizeof(buf->cSRC)) ) {
		LOG(em_ERR), "[%s] SRC 異常 [%.2s]", my_sThreadName, buf->cSRC);
		//syslog(353, "[<%s> %s ユニットNo=%.2s SRC=%.2s]", GetMainModeName(), my_sThreadName, buf->cNodeNo, buf->cSRC);
		return false;
	}
	// MRES/SRES
	if( 0 != memcmp(buf->cResCode, RES_OK, sizeof(buf->cResCode)) ) {
		LOG(em_ERR), "[%s] MRES/SRES 異常 [%s]", my_sThreadName, GetResCodeName(buf->cResCode));
		//syslog(353, "[<%s> %s ユニットNo=%.2s MRES/SRES=%.4s(%s)]", GetMainModeName(), my_sThreadName, buf->cNodeNo, buf->cResCode, GetResCodeName(buf->cResCode));
		return false;
	}
	// EXT
	if( 0 != memcmp(buf->cEXT, EXT_OK, sizeof(buf->cEXT)) ) {
		LOG(em_ERR), "[%s] EXT 異常 [%s]", my_sThreadName, GetEXTName(buf->cEXT));
		//syslog(353, "[<%s> %s ユニットNo=%.2s EXT=%.2s(%s)]", GetMainModeName(), my_sThreadName, buf->cNodeNo, buf->cEXT, GetEXTName(buf->cEXT));
		return false;
	}
	return true;
}
bool TempManager::Check_Controler_Status(RES_TYPE_CONTROLER_STATUS* buf)
{
	int id = m_nUnitMode - 1;

	// MRC
	if( 0 != memcmp(buf->cMRC, "06", sizeof(buf->cMRC)) ) {
		LOG(em_ERR), "[%s] MRC 異常 [%.2s]", my_sThreadName, buf->cMRC);
		//syslog(353, "[<%s> %s ユニットNo=%.2s MRC=%.2s]", GetMainModeName(), my_sThreadName, buf->cNodeNo, buf->cMRC);
		return false;
	}
	// SRC
	if( 0 != memcmp(buf->cSRC, "01", sizeof(buf->cSRC)) ) {
		LOG(em_ERR), "[%s] SRC 異常 [%.2s]", my_sThreadName, buf->cSRC);
		//syslog(353, "[<%s> %s ユニットNo=%.2s SRC=%.2s]", GetMainModeName(), my_sThreadName, buf->cNodeNo, buf->cSRC);
		return false;
	}			
	// MRES/SRES
	if( 0 != memcmp(buf->cResCode, RES_OK, sizeof(buf->cResCode)) ) {
		LOG(em_ERR), "[%s] MRES/SRES 異常 [%s]", my_sThreadName, GetResCodeName(buf->cResCode));
		//syslog(353, "[<%s> %s ユニットNo=%.2s MRES/SRES=%.4s(%s)]", GetMainModeName(), my_sThreadName, buf->cNodeNo, buf->cResCode, GetResCodeName(buf->cResCode));
		return false;
	}
	// EXT
    if( 0 != memcmp(buf->cEXT, EXT_OK, sizeof(buf->cEXT)) ) {
		LOG(em_ERR), "[%s] EXT 異常 [%s]", my_sThreadName, GetEXTName(buf->cEXT));
		//syslog(353, "[<%s> %s ユニットNo=%.2s EXT=%.2s(%s)]", GetMainModeName(), my_sThreadName, buf->cNodeNo, buf->cEXT, GetEXTName(buf->cEXT));
		return false;
	}

	//======================================================
	// 運転状態
	if( 0 == memcmp(buf->cData1, "00", sizeof(buf->cData1)) ) {
		if( DIV_BOOL_TRUE != m_emUnten[id] ) {	
			LOG(em_MSG), "[%s] 運転状態 %s [正常運転中(%.2s)]", my_sThreadName, m_cUnitName[id], buf->cData1);		
			m_emUnten[id] = DIV_BOOL_TRUE;
			if(0 != m_nIdUnten[id]) KizuFunction::SetStatus(m_nIdUnten[id], true);

		}
	} else if( 0 == memcmp(buf->cData1, "01", sizeof(buf->cData1)) ) {
		LOG(em_ERR), "[%s] 運転状態 %s [停止中(%.2s)]", my_sThreadName, m_cUnitName[id], buf->cData1);
		if( DIV_BOOL_FALSE != m_emUnten[id] ) {
			//syslog(354, "[<%s> %s 停止]", GetMainModeName(), m_cUnitName[id]);
			m_emUnten[id] = DIV_BOOL_FALSE;
			if(0 != m_nIdUnten[id]) KizuFunction::SetStatus(m_nIdUnten[id], false);
		}
	} else {
		LOG(em_ERR), "[%s] 運転状態 %s [不明(%.2s)]", my_sThreadName, m_cUnitName[id], buf->cData1);
		if( DIV_BOOL_FALSE != m_emUnten[id] ) {
			//syslog(354, "[<%s> %s 不明]", GetMainModeName(), m_cUnitName[id]);
			m_emUnten[id] = DIV_BOOL_FALSE;
			if(0 != m_nIdUnten[id]) KizuFunction::SetStatus(m_nIdUnten[id], false);
		}
	}

	//======================================================
	// ステータス
	if( 0 == memcmp(buf->cData2, "00", sizeof(buf->cData2)) ) {
		if( m_nStatus[id] != MT_STATUS_OK ) {
			LOG(em_MSG), "[%s] ステータス %s [正常(%.2s)]", my_sThreadName, m_cUnitName[id], buf->cData2);
			m_nStatus[id] = MT_STATUS_OK;
			if(0 != m_nIdState[id]) KizuFunction::SetStatus(m_nIdState[id], m_nStatus[id]);
		}
	} else {
		if( buf->cData2[1] & 0x01 ) {
			LOG(em_WAR), "[%s] ステータス %s [未計測状態(%.2s)]", my_sThreadName, m_cUnitName[id], buf->cData2);
			if( ! (m_nStatus[id] & MT_STATUS_ERR_NO_MEASURE) ) {
				//syslog(355, "[<%s> %s 未計測状態]", GetMainModeName(), m_cUnitName[id]);
				m_nStatus[id] = MT_STATUS_ERR_NO_MEASURE;
				if(0 != m_nIdState[id]) KizuFunction::SetStatus(m_nIdState[id], m_nStatus[id]);
			}
			return false;
		}
		if( buf->cData2[1] & 0x02 ) {
			LOG(em_WAR), "[%s] ステータス %s [表示範囲オーバー(%.2s)]", my_sThreadName, m_cUnitName[id], buf->cData2);
			if( ! (m_nStatus[id] & MT_STATUS_ERR_DISPLAY_OVER) ) {
				//syslog(355, "[<%s> %s 表示範囲オーバー]", GetMainModeName(), m_cUnitName[id]);
				m_nStatus[id] = MT_STATUS_ERR_DISPLAY_OVER;
				if(0 != m_nIdState[id]) KizuFunction::SetStatus(m_nIdState[id], m_nStatus[id]);
			}
			return false;
		}
		if( buf->cData2[1] & 0x04 ) {
			LOG(em_ERR), "[%s] ステータス %s [入力異常A(%.2s)]", my_sThreadName, m_cUnitName[id], buf->cData2);
			if( ! (m_nStatus[id] & MT_STATUS_ERR_INPUT_A) ) {
				//syslog(355, "[<%s> %s 入力異常A]", GetMainModeName(), m_cUnitName[id]);
				m_nStatus[id] = MT_STATUS_ERR_INPUT_A;
				if(0 != m_nIdState[id]) KizuFunction::SetStatus(m_nIdState[id], m_nStatus[id]);
			}
			return false;
		}
		if( buf->cData2[1] & 0x08 ) {
			LOG(em_ERR), "[%s] ステータス %s [入力異常B(%.2s)]", my_sThreadName, m_cUnitName[id], buf->cData2);
			if( ! (m_nStatus[id] & MT_STATUS_ERR_INPUT_B) ) {
				//syslog(355, "[<%s> %s 入力異常B]", GetMainModeName(), m_cUnitName[id]);
				m_nStatus[id] = MT_STATUS_ERR_INPUT_B;
				if(0 != m_nIdState[id]) KizuFunction::SetStatus(m_nIdState[id], m_nStatus[id]);
			}
			return false;
		}
	}
	return true;
}

bool TempManager::Check_Monitor_Read(RES_TYPE_MONITOR_READ* buf)
{
	int id = m_nUnitMode - 1;
	// MRC
	if( 0 != memcmp(buf->cMRC, "01", sizeof(buf->cMRC)) ) {
		LOG(em_ERR), "[%s] MRC 異常 [%.2s]", my_sThreadName, buf->cMRC);
		//syslog(353, "[<%s> %s ユニットNo=%.2s MRC=%.2s]", GetMainModeName(), my_sThreadName, buf->cNodeNo, buf->cMRC);
		return false;
	}
	// SRC
	if( 0 != memcmp(buf->cSRC, "01", sizeof(buf->cSRC)) ) {
		LOG(em_ERR), "[%s] SRC 異常 [%.2s]", my_sThreadName, buf->cSRC);
		//syslog(353, "[<%s> %s ユニットNo=%.2s SRC=%.2s]", GetMainModeName(), my_sThreadName, buf->cNodeNo, buf->cSRC);
		return false;
	}			
	// MRES/SRES
	if( 0 != memcmp(buf->cResCode, RES_OK, sizeof(buf->cResCode)) ) {
		LOG(em_ERR), "[%s] MRES/SRES 異常 [%s]", my_sThreadName, GetResCodeName(buf->cResCode));
		//syslog(353, "[<%s> %s ユニットNo=%.2s MRES/SRES=%.4s(%s)]", GetMainModeName(), my_sThreadName, buf->cNodeNo, buf->cResCode, GetResCodeName(buf->cResCode));
		return false;
	}
	// EXT
	if( 0 != memcmp(buf->cEXT, EXT_OK, sizeof(buf->cEXT)) ) {
		LOG(em_ERR), "[%s] EXT 異常 [%s]", my_sThreadName, GetEXTName(buf->cEXT));
		//syslog(353, "[<%s> %s ユニットNo=%.2s EXT=%.2s(%s)]", GetMainModeName(), my_sThreadName, buf->cNodeNo, buf->cEXT, GetEXTName(buf->cEXT));
		return false;
	}


	//======================================================
	// モニタ値
	CString cwk;
// --->>> 20141212 MOD マイナス値変換不具合修正
//	cwk.Format("0x%.8s", buf->cData);
//	double monitor = (double)strtol(cwk, NULL, 0);
//	//monitor /= 10;
//	for(int ii=0; ii<m_nTempKeta; ii++) monitor /= 10;			// ものによって桁が異なるための対応	
	cwk.Format("%.8s", buf->cData);
	long nwk;
	sscanf(cwk, "%X", &nwk);
	double monitor = (double)nwk;
	for(int ii=0; ii<m_nTempKeta; ii++) monitor /= 10.0;		// ものによって桁が異なるための対応	
// <<<--- 20141212 MOD マイナス値変換不具合修正

	// 運転状態チェック
	if( DIV_BOOL_TRUE != m_emUnten[id] ) monitor = 0.0;

	LOG(em_INF), "[%s] %s 温度=%lf [℃]", my_sThreadName, m_cUnitName[id], monitor);

	// モニタ値 保存
	if(m_dTemp[id] != monitor) {
		if(0 != m_nIdTemp[id]) KizuFunction::SetStatus(m_nIdTemp[id], (int)(monitor+0.5), false);	// 四捨五入
	}
	m_dTemp[id] = monitor;

	// 温度チェック
	bool bLimitOk = true;
	if( 0 != m_nTempLimit[id] && m_nTempLimit[id] <= (int)monitor ) bLimitOk = false;
	if( ! bLimitOk && m_bTempSt[id] ) {	// 正常→異常
		if(0 != m_nIdTempSts[id]) KizuFunction::SetStatus(m_nIdTempSts[id], false);

	} else if( bLimitOk && ! m_bTempSt[id]){ // 異常→正常
		if(0 != m_nIdTempSts[id]) KizuFunction::SetStatus(m_nIdTempSts[id], true);

	}
	m_bTempSt[id] = bLimitOk; 

	return true;
}


//------------------------------------------
// 受信伝文チェック
// 戻り値 false:回線切断処理を実施
//------------------------------------------
bool TempManager::CheckDataRecv()
{
	char	cWk[64];
	CString	strWk;

	//======================================================
	// 事前準備

	// 現在のm_nUnitModeから機器の配列位置を算出
	int id = m_nUnitMode - 1;
//◆
	LOG(em_MSG), "[%s] 受信 [%s]", my_sThreadName, m_cRecvBuf);

	// 応答伝文受信タイムアウト タイマー停止
	CancelWaitableTimer(m_evAnserTimeOut);

	// BCC チェック
	int nRecvBufSize = (int)strlen(m_cRecvBuf);
	if( ! CheckBCC(m_cRecvBuf, nRecvBufSize) ) {
		LOG(em_ERR), "[%s] 応答伝文 BCC異常 [受信=%.1s]", my_sThreadName, &m_cRecvBuf[nRecvBufSize-1]);
		//syslog(0, "[%s:ユニットNo=%d]", my_sThreadName, id);	// BCCチェック異常
		return false;
	}

	// ユニットNo チェック
	sprintf(cWk, "%02d", id+1);
	if( 0 != memcmp(&m_cRecvBuf[1], cWk, 2) ) {
		LOG(em_ERR), "[%s] 応答伝文 ユニットNo異常 [送信=%s][受信=%.2s]", my_sThreadName, cWk, &m_cRecvBuf[1]);
		//syslog(0, "[<%s> %s 送信ユニットNo=%s 受信ユニットNo=%.2s]", GetMainModeName(), my_sThreadName, cWk, &m_cRecvBuf[1]);
		return false;
	}

	//// 処理分け
	switch( m_emMainMode ) {
	

	//======================================================
	// 初期準備確認中
	case MODE_OPEN_CHECK:

		//---------------------------------------------
		////// エコーバックテスト
		if( 1==m_nSubMode ) {
			// 伝文内容チェック
			RES_TYPE_ECHO_BACK* buf = (RES_TYPE_ECHO_BACK *)m_cRecvBuf;
			if( ! Check_Echo_Back(buf) ) return false;
			if( ! CheckNextSend() ) return false;
		}
		////---------------------------------------------
		//} else if( 2==m_nSubMode ) {
		//	RES_TYPE_CONTROLER_STATUS* buf = (RES_TYPE_CONTROLER_STATUS *)m_cRecvBuf;
		//	if( ! Check_Controler_Status(buf) ) return false;
		//	if( ! CheckNextSend() ) return false;
		//}
		break;

	//======================================================
	// コントローラステータス読出し中
	case MODE_READ_STATUS:
		////// コントローラステータス読出し
		if( 1==m_nSubMode ) {
			RES_TYPE_CONTROLER_STATUS* buf = (RES_TYPE_CONTROLER_STATUS *)m_cRecvBuf;
			if( ! Check_Controler_Status(buf) ) return false;
			if( ! CheckNextSend() ) return false;
		//---------------------------------------------
		} else if( 2==m_nSubMode ) {
			RES_TYPE_MONITOR_READ* buf = (RES_TYPE_MONITOR_READ *)m_cRecvBuf;
			if( ! Check_Monitor_Read(buf) ) return false;
			if( ! CheckNextSend() ) return false;
		}
	}


	return true;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 232C通信関連

//------------------------------------------
// 232C オープン
// bool bLogDsp false:ログ出力無し
//------------------------------------------
bool TempManager::Open(bool bLogDsp)
{
	// Rs232cBaseインスタンス生成
	try {
		mcls_p232c = new Rs232cBase(m_cMySession, TASKINI_NAME, GetTid(), PARAM_232C_RECVBUF_SIZE, PARAM_232C_SENDBUF_SIZE, WM_USER);
		//mcls_p232c->SetRecvEndCR(ETX_CODE);	// 終端コードチェックに可変長に読込みとする
	} catch(int code) {
		mcls_p232c = NULL;
		if( bLogDsp ) {
			LOG(em_ERR), "[%s] 232Cオープンエラー (err_code = %d)", my_sThreadName, code);
			syslog(SYSNO_232C_NEW_ERR, "[温度パネルメータ:err_code = %d]", code);
		}
		return false;
	}

	// 接続完了
	memset(m_cRecvBuf, 0x00, sizeof(m_cRecvBuf));
	memset(m_cSendBuf, 0x00, sizeof(m_cSendBuf));
	//syslog(SYSNO_232C_OPEN, "[温度パネルメータ]");

	// 受信開始
	if( ! Recv() ) return false;

	// 初期通信チェック
	m_bIsConnect = DIV_BOOL_TRUE;
	m_emMainMode = MODE_OPEN_CHECK;
	m_nSubMode = 0;
	m_nUnitMode = 0;

	// 温度パネルメータと通信できるか確認
	if( ! CheckNextSend() ) return false;
	return true;
}

//------------------------------------------
// 232C クローズ
//------------------------------------------
void TempManager::Close()
{
	// リトライタイマ以外は全て停止
	CancelWaitableTimer(m_evReTry);
	CancelWaitableTimer(m_evAnserTimeOut);

	// クローズ
	if( NULL != mcls_p232c ) {
		SAFE_DELETE(mcls_p232c);
	}

	//// リトライのたびにログ出力されるの禁止
	// 正常から異常に変わった場合のみ
	if( DIV_BOOL_TRUE == m_bIsConnect ) {
		LOG(em_ERR), "[%s] 232Cクローズ", my_sThreadName);
		syslog(SYSNO_232C_CLOSE, "[温度パネルメータ]");
		// 機器状態変更
		if(0 != m_nIdConnect) KizuFunction::SetStatus(m_nIdConnect, false);
	}

	// ステータス初期化
	m_bIsConnect = DIV_BOOL_FALSE;
	m_bIsInital  = DIV_BOOL_FALSE;
	m_emMainMode = MODE_RETRY;
	m_nSubMode = 0;
	m_nUnitMode = 0;

	// リトライタイマ セット
	LARGE_INTEGER ts;
	ts.QuadPart = (__int64)PARAM_TIMEOUT_RETRY * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evReTry, &ts, 0, NULL, NULL, FALSE);
}

//------------------------------------------
// 232C 非同期 受信
//------------------------------------------
bool TempManager::Recv()
{
	if( NULL == mcls_p232c ) return true;

	// 受信バッファクリア
	memset(m_cRecvBuf, 0x00, sizeof(m_cRecvBuf));

	//受信バッファサイズをモードごとに決定
	DWORD nRecvBufSize = 0;
	switch( m_emMainMode ) {
		case MODE_OPEN_CHECK:
			// 初期準備確認中
			if(1==m_nSubMode)
			{
				nRecvBufSize = sizeof(RES_TYPE_ECHO_BACK);
			}
			//if(2==m_nSubMode)
			//{
			//	nRecvBufSize = sizeof(RES_TYPE_CONTROLER_STATUS);
			//}
			break;

		case MODE_WAIT:
			nRecvBufSize = sizeof(RES_TYPE_CONTROLER_STATUS);	// WAIT後の初回は、コントロール読み込み
			break;
				
		case MODE_READ_STATUS:
			if( 1==m_nSubMode )
			{
				// コントローラステータス読出し中
				nRecvBufSize = sizeof(RES_TYPE_CONTROLER_STATUS);
			}
			if(2==m_nSubMode)
			{
				nRecvBufSize = sizeof(RES_TYPE_MONITOR_READ);
			}
			break;

		default:		// 基本
			nRecvBufSize = sizeof(RES_TYPE_ECHO_BACK);
			//LOG(em_WAR), "[%s] メインモード不明 (MainMode = %d)", my_sThreadName, m_emMainMode);
			break;
	}

	//// 受信
	int iRetc = mcls_p232c->Recv(m_cRecvBuf, nRecvBufSize, m_evRecvEnd);
	if(0 != iRetc) {
		// 受信エラー
		LOG(em_ERR), "[%s] 232C受信エラー (err_code = %d)", my_sThreadName, iRetc);
		syslog(SYSNO_232C_RECV_ERR, "[温度パネルメータ:err_code = %d]", iRetc);
		return false;
	}
	return true;
}

//------------------------------------------
// 232C 同期 送信
//------------------------------------------
bool TempManager::Send()
{
	if( NULL == mcls_p232c) return true;

	//// 送信
	int iRetc = mcls_p232c->Send(m_cSendBuf, (DWORD)strlen(m_cSendBuf), m_evSendEnd);
	if(0 != iRetc) {
		// 送信エラー
		LOG(em_ERR), "[%s] 232C送信エラー (err_code = %d)", my_sThreadName, iRetc);
		syslog(SYSNO_232C_SEND_ERR, "[温度パネルメータ err_code = %d]", iRetc);
		return false;
	}

	//// 送信中
	// 応答タイマ セット
	LARGE_INTEGER ts;
	ts.QuadPart = (__int64)PARAM_TIMEOUT_ANSER * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evAnserTimeOut, &ts, 0, NULL, NULL, FALSE);
	return true;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 小物関連

//------------------------------------------
// メインモード名称
// EM_TEMP_MAINMODE em モード
// 戻り値 名称
//------------------------------------------
CString TempManager::GetMainModeName(EM_TEMP_MAINMODE em)
{
	CString	wk;
	if( MODE_NONE == em ) {
		wk = "初期状態";
	} else if( MODE_OPEN_CHECK == em ) {
		wk = "初期準備確認中";
	} else if( MODE_WAIT == em ) {
		wk = "待機";
	} else if( MODE_READ_STATUS == em ) {
		wk = "読出し中";
	//} else if(  MODE_READ_MONITOR == em ) {
	//	wk = "モニタ値読出し中";
	} else if( MODE_RETRY == em ) {
		wk = "オープンリトライ中";
	} else {
		wk = "モード異常";
	}
	return wk;
}

//------------------------------------------
// レスポンスコード名称
// char* cResCode レスポンスコード
// 戻り値 名称
//------------------------------------------
CString TempManager::GetResCodeName(char* cResCode)
{
	CString wk;

	if( 0==memcmp(cResCode, RES_OK, RES_SIZE) ) {
		wk = "正常終了";
	} else if( 0==memcmp(cResCode, RES_ERR_CMDLEN_OVER, RES_SIZE) ) {
		wk = "コマンド長オーバー";
	} else if( 0==memcmp(cResCode, RES_ERR_CMDLEN_SHORT, RES_SIZE) ) {
		wk = "コマンド長不足";
	} else if( 0==memcmp(cResCode, RES_ERR_AREAKIND, RES_SIZE) ) {
		wk = "エリア種別エラー";
	} else if( 0==memcmp(cResCode, RES_ERR_RESLEN_OVER, RES_SIZE) ) {
		wk = "レスポンス長オーバー";
	} else if( 0==memcmp(cResCode, RES_ERR_PARAM, RES_SIZE) ) {
		wk = "パラメータエラー";
	} else if( 0==memcmp(cResCode, RES_ERR_SYSTEM, RES_SIZE) ) {
		wk = "動作エラー";
	} else {
		wk = "レスポンスコード異常";
	}

	return wk;
}


//------------------------------------------
// 終了コード名称
// char* cEXT 終了コード
// 戻り値 名称
//------------------------------------------
CString TempManager::GetEXTName(char* cEXT)
{
	CString	wk;

	if( 0==memcmp(cEXT, EXT_OK, EXT_SIZE) ) {
		wk = "正常終了";
	} else if( 0==memcmp(cEXT, EXT_ERR_FINS, EXT_SIZE) ) {
		wk = "FINSコマンドエラー";
	} else if( 0==memcmp(cEXT, EXT_ERR_PARITY, EXT_SIZE) ) {
		wk = "パリティエラー";
	} else if( 0==memcmp(cEXT, EXT_ERR_FRAMING, EXT_SIZE) ) {
		wk = "フレーミングエラー";
	} else if( 0==memcmp(cEXT, EXT_ERR_OVERRUN, EXT_SIZE) ) {
		wk = "オーバーランエラー";
	} else if( 0==memcmp(cEXT, EXT_ERR_BCC, EXT_SIZE) ) {
		wk = "BCCエラー";
	} else if( 0==memcmp(cEXT, EXT_ERR_FORMAT, EXT_SIZE) ) {
		wk = "フォーマットエラー";
	} else if( 0==memcmp(cEXT, EXT_ERR_SUBADDRESS, EXT_SIZE) ) {
		wk = "サブアドレスエラー";
	} else if( 0==memcmp(cEXT, EXT_ERR_FRAMELEN, EXT_SIZE) ) {
		wk = "フレーム長エラー";
	} else {
		wk = "終了コード異常";
	}

	return wk;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BCC関連

//------------------------------------------
// BCCクリエイト
// char* cSendBuf 送信バッファ
// int nBufSize 送信データサイズ
//------------------------------------------
void TempManager::CreateBCC(char* cSendBuf, int nBufSize)
{
	char			cBCC = 0x00;

	// ユニットNo～ETXまでのBCCを計算
	for(int ii=1; ii<nBufSize-1; ii++) {
		cBCC ^= cSendBuf[ii];
	}

	// BCCを格納
	cSendBuf[nBufSize-1] = cBCC;
}

//------------------------------------------
// BCCチェック
// char* cRecvBuf 受信バッファ
// int nBufSize 受信データサイズ
//------------------------------------------
bool TempManager::CheckBCC(char* cRecvBuf, int nBufSize)
{
	char			cBCC = 0x00;
	bool			bRet = true;

	// ユニットNo～ETXまでのBCCを計算
	for(int ii=1; ii<nBufSize-1; ii++) {
		cBCC ^= cRecvBuf[ii];
	}
	if( cBCC != cRecvBuf[nBufSize-1] ) {
		bRet = false;
	}

	return bRet;
}
