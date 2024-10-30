#include "StdAfx.h"
#include "LinkswManager.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// コンストラクタ
//------------------------------------------
LinkswManager::LinkswManager(void) : 
ThreadManager("LinkswMgr"),
mcls_p232c(NULL),
m_bIsConnect(false),
m_nEventWaitNum(0),
m_emMainMode(MODE_NONE),
m_nSubMode(0),
m_nCamMode(0),
m_nApxMode(0),
m_nAocMode(0),
m_nAocSubMode(0),
m_nRegSizeMode(0),
m_nSwMode(0),
m_bCCMode(FALSE),
m_nSlaveJoinTorB(0),
m_nSlaveJoinFNo(0),
m_nResetFNo(0),
m_bInitialSpeedChange(false),
m_bEnableRapidSerial(false)
{
	// 何も無ければこれ
	m_nUnit = 0;
	my_sThreadName = TO_LINKSW;
	m_cMySession = TO_LINKSW;

	char	cWk[1024];
	CString	sWk;
	CString	sWk2;
	int		nWk;

	// 初期化
	memset( m_cRecvBuf, 0x00, sizeof(m_cRecvBuf) );
	memset( m_cSendBuf, 0x00, sizeof(m_cSendBuf) );

	// イベント生成
	m_evRecvEnd			= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evSendEnd			= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evReTry			= CreateWaitableTimer(NULL, FALSE, NULL);
	m_evAnswerTimeOut	= CreateWaitableTimer(NULL, FALSE, NULL);
	m_evTimerCycleCheck	= CreateWaitableTimer(NULL, FALSE, NULL);
	m_evCounterCheck	= CreateEvent(NULL, FALSE, FALSE, NULL);

	//// INIファイル設定取得
	// 定周期チェックタイマ周期
	m_nCycleTime = GetPrivateProfileInt(m_cMySession, "TIMER_CYCLE", 0, TASKINI_NAME);
	m_nCycleTime *= 1000;
	// 機器状態関連
	for( int ii=0; ii<NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; ii++ )
	{
		sWk.Format("%s_%d", m_cMySession, ii);
		// FPGAダイ温度
		m_nDieTempLimit[ii]	= GetPrivateProfileInt(sWk, "FPGADIE_TEMP_LIMIT",	0, TASKINI_NAME);
		m_nDieTempID[ii]	= GetPrivateProfileInt(sWk, "FPGADIE_TEMP_ID",		0, TASKINI_NAME);
		m_nDieTempStID[ii]	= GetPrivateProfileInt(sWk, "FPGADIE_TEMP_ST_ID",	0, TASKINI_NAME);
		m_nDieTemp[ii]		= 0;
		m_bDieTempSt[ii]	= true;
		// 周囲温度
		m_nAmbTempLimit[ii]	= GetPrivateProfileInt(sWk, "AMBIENT_TEMP_LIMIT",	0, TASKINI_NAME);
		m_nAmbTempID[ii]	= GetPrivateProfileInt(sWk, "AMBIENT_TEMP_ID",		0, TASKINI_NAME);
		m_nAmbTempStID[ii]	= GetPrivateProfileInt(sWk, "AMBIENT_TEMP_ST_ID",	0, TASKINI_NAME);
		m_nAmbTemp[ii]		= 0;
		m_bAmbTempSt[ii]	= true;
		// Opt-C:LinkステータスID
		for( int jj=0; jj<MAX_CAMLINKSW_INPUT; jj++ )
		{
			// B
			sWk2.Format("OPTCLINK%d_B_ST_ID", jj+1);
			m_nOptCLinkStID[ii][jj][0] = GetPrivateProfileInt(sWk, sWk2, 0, TASKINI_NAME);
			m_bOptCLinkSt[ii][jj][0]	= true;
			// M/F
			sWk2.Format("OPTCLINK%d_MF_ST_ID", jj+1);
			m_nOptCLinkStID[ii][jj][1] = GetPrivateProfileInt(sWk, sWk2, 0, TASKINI_NAME);
			m_bOptCLinkSt[ii][jj][1]	= true;
		}
	}

	// iniファイルからハーフ版カメラリンクSWのフラグを取得
	memset(m_bHalfFlg, 0x00, sizeof(m_bHalfFlg));
	for (int jj = 0; jj<NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; jj++)
	{
		sWk.Format("HALF_FLG_%d", (int)jj / MAX_CAMLINKSW_FPGA + 1);
		nWk = GetPrivateProfileInt(m_cMySession, sWk, 0, TASKINI_NAME);
		if (0 == nWk)	m_bHalfFlg[jj] = false;
		else			m_bHalfFlg[jj] = true;
	}

	// iniファイル指定のカメラ入力CH, 表裏区分を取得
	memset( m_nSelectCam, 0x00, sizeof(m_nSelectCam) );
	memset( m_nSelectTorB, 0x00, sizeof(m_nSelectTorB) );
	for( int ii=0; ii<NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; ii++ )
	{
		for( int jj=0; jj<MAX_CAMLINKSW_OUTPUT; jj++ )
		{
			sWk.Format("FPGA_%d_BOARD_%d", ii, jj+1);
			GetPrivateProfileString(m_cMySession, sWk, "", cWk, sizeof(cWk), TASKINI_NAME);

			if( 0 < strlen(cWk) )
			{
				sWk = cWk;

				// トークン分割
				int		rec = 0;
				int		curPos = 0;
				CString	strToken = sWk.Tokenize(",", curPos);
				while( strToken != "" )
				{
					strToken.Trim(_T(" 	"));			// iniファイル設定文字の前後のスペース、タブを取り除く

					if( 0 == rec )						// カメラ入力CH
					{
						// 2017.02.27 Master/SlaveをDB指定 ---------->>>>
						//nWk = atoi(strToken);
				 		//if( 0 >= nWk || MAX_CAMLINKSW_ALLINPUT < nWk ) break;
						//m_nSelectCam[ii][jj] = nWk;
						
						nWk = atoi(strToken);
			 			if( (-1 * MAX_CAMLINKSW_ALLINPUT) > nWk || MAX_CAMLINKSW_ALLINPUT < nWk ) break;

						if (0 != nWk) {
							if (m_bHalfFlg[ii] && !HARF_CC_SOCK_EXIST[jj]) {
								LOG(em_ERR), "[%s] <%d> ハーフ版に存在しないボードがiniに設定されてあるため強制的に未選択に変更(board=%d, cam=%d)", my_sThreadName, ii, jj, nWk);
								break;
							}
						}
						m_nSelectCam[ii][jj] = nWk;
						
						// 2017.02.27 Master/SlaveをDB指定 <<<<----------


					}
					else if( 1 == rec )					// 表裏区分
					{
						nWk = atoi(strToken);
						m_nSelectTorB[ii][jj] = nWk;
					}

					// 次へ
					strToken = sWk.Tokenize(",", curPos);
					rec++;
				}
			}
		}
	}

	// 高速通信切替時のボーレート
	m_bEnableRapidSerial = ( 1 == GetPrivateProfileInt(m_cMySession, "ENABLE_SPEED_CHANGE", 0, TASKINI_NAME) ? true : false );

	// 途中参加レジスタバッファ クリア
	memset(m_nCounterNow, 0x00, sizeof(m_nCounterNow));
	memset(m_nCounterSet, 0x00, sizeof(m_nCounterSet));

	// Opt-C:Linkステータスバッファ クリア
	memset(m_nOptCLinkSt,		0x00, sizeof(m_nOptCLinkSt));
	memset(m_bOptCLink_RxSt,	0x01, sizeof(m_bOptCLink_RxSt));
	memset(m_bOptCLink_LinkSt,	0x01, sizeof(m_bOptCLink_LinkSt));
}

//------------------------------------------
// デストラクタ
//------------------------------------------
LinkswManager::~LinkswManager(void)
{
	// イベント解放
	CloseHandle(m_evRecvEnd);
	CloseHandle(m_evSendEnd);
	CloseHandle(m_evReTry);
	CloseHandle(m_evAnswerTimeOut);
	CloseHandle(m_evTimerCycleCheck);
	CloseHandle(m_evCounterCheck);

	// 232C クローズ
	if( NULL != mcls_p232c )
	{
		SAFE_DELETE(mcls_p232c);
	}
}

//------------------------------------------
// 自クラスセット
// int nUnit			ユニット番号 (0オリジン)
// char const* sSession	Iniセクション名
//------------------------------------------
void LinkswManager::SetClass(int nUnit, char* sSession)
{
	m_nUnit = nUnit;
	m_cMySession = sSession;
	my_sThreadName.Format("LinkSwMgr_%d", m_nUnit); 


	// その他特殊？初期設定項目？読込
	char	cWk[1024];
	CString	sWk;
	CString	sWk2;
	int		nWk;
	//GetPrivateProfileString(m_cMySession, "PARAM_CNT_TBL_NAME", m_cPrmCommonTblName, cWk, sizeof(cWk), TASKINI_NAME);	m_cPrmCommonTblName	= cWk;
	//m_nMyCamNum		= GetPrivateProfileInt(m_cMySession, "CAMID_NUM", m_nMyCamNum, TASKINI_NAME);
	//m_nMyCamIdStart = GetPrivateProfileInt(m_cMySession, "CAMID_START", m_nMyCamIdStart, TASKINI_NAME); 
	//m_nMyCamIdEnd	= m_nMyCamIdStart+m_nMyCamNum;


	
	//// INIファイル設定取得
	// 定周期チェックタイマ周期
	m_nCycleTime = GetPrivateProfileInt(m_cMySession, "TIMER_CYCLE", 0, TASKINI_NAME);
	m_nCycleTime *= 1000;
	// 機器状態関連
	for( int ii=0; ii<NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; ii++ )
	{
		sWk.Format("%s_%d", m_cMySession, ii);
		// FPGAダイ温度
		m_nDieTempLimit[ii]	= GetPrivateProfileInt(sWk, "FPGADIE_TEMP_LIMIT",	0, TASKINI_NAME);
		m_nDieTempID[ii]	= GetPrivateProfileInt(sWk, "FPGADIE_TEMP_ID",		0, TASKINI_NAME);
		m_nDieTempStID[ii]	= GetPrivateProfileInt(sWk, "FPGADIE_TEMP_ST_ID",	0, TASKINI_NAME);
		m_nDieTemp[ii]		= 0;
		m_bDieTempSt[ii]	= true;
		// 周囲温度
		m_nAmbTempLimit[ii]	= GetPrivateProfileInt(sWk, "AMBIENT_TEMP_LIMIT",	0, TASKINI_NAME);
		m_nAmbTempID[ii]	= GetPrivateProfileInt(sWk, "AMBIENT_TEMP_ID",		0, TASKINI_NAME);
		m_nAmbTempStID[ii]	= GetPrivateProfileInt(sWk, "AMBIENT_TEMP_ST_ID",	0, TASKINI_NAME);
		m_nAmbTemp[ii]		= 0;
		m_bAmbTempSt[ii]	= true;
		// Opt-C:LinkステータスID
		for( int jj=0; jj<MAX_CAMLINKSW_INPUT; jj++ )
		{
			// B
			sWk2.Format("OPTCLINK%d_B_ST_ID", jj+1);
			m_nOptCLinkStID[ii][jj][0] = GetPrivateProfileInt(sWk, sWk2, 0, TASKINI_NAME);
			m_bOptCLinkSt[ii][jj][0]	= true;
			// M/F
			sWk2.Format("OPTCLINK%d_MF_ST_ID", jj+1);
			m_nOptCLinkStID[ii][jj][1] = GetPrivateProfileInt(sWk, sWk2, 0, TASKINI_NAME);
			m_bOptCLinkSt[ii][jj][1]	= true;
		}
	}

	// iniファイルからハーフ版カメラリンクSWのフラグを取得
	memset( m_bHalfFlg, 0x00, sizeof(m_bHalfFlg));
	for (int jj = 0; jj<NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; jj++)
	{
		sWk.Format("HALF_FLG_%d", (int)jj / MAX_CAMLINKSW_FPGA + 1);
		nWk = GetPrivateProfileInt(m_cMySession, sWk, 0, TASKINI_NAME);
		if (0 == nWk)	m_bHalfFlg[jj] = false;
		else			m_bHalfFlg[jj] = true;
	}

	// iniファイル指定のカメラ入力CH, 表裏区分を取得
	memset( m_nSelectCam, 0x00, sizeof(m_nSelectCam) );
	memset( m_nSelectTorB, 0x00, sizeof(m_nSelectTorB) );
	for( int ii=0; ii<NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; ii++ )
	{
		for( int jj=0; jj<MAX_CAMLINKSW_OUTPUT; jj++ )
		{
			sWk.Format("FPGA_%d_BOARD_%d", ii, jj+1);
			GetPrivateProfileString(m_cMySession, sWk, "", cWk, sizeof(cWk), TASKINI_NAME);

			if( 0 < strlen(cWk) )
			{
				sWk = cWk;

				// トークン分割
				int		rec = 0;
				int		curPos = 0;
				CString	strToken = sWk.Tokenize(",", curPos);
				while( strToken != "" )
				{
					strToken.Trim(_T(" 	"));			// iniファイル設定文字の前後のスペース、タブを取り除く

					if( 0 == rec )						// カメラ入力CH
					{						
						nWk = atoi(strToken);
			 			if( (-1 * MAX_CAMLINKSW_ALLINPUT) > nWk || MAX_CAMLINKSW_ALLINPUT < nWk ) break;
						if (0 != nWk) {
							if (m_bHalfFlg[ii] && !HARF_CC_SOCK_EXIST[jj]) {
								LOG(em_ERR), "[%s] <%d> ハーフ版に存在しないボードがiniに設定されてあるため強制的に未選択に変更(board=%d, cam=%d)", my_sThreadName, ii, jj, nWk);
								break;
							}
						}
						m_nSelectCam[ii][jj] = nWk;
					}
					else if( 1 == rec )					// 表裏区分
					{
						nWk = atoi(strToken);
						m_nSelectTorB[ii][jj] = nWk;
					}

					// 次へ
					strToken = sWk.Tokenize(",", curPos);
					rec++;
				}
			}
		}
	}

	// 高速通信切替時のボーレート
	m_bEnableRapidSerial = ( 1 == GetPrivateProfileInt(m_cMySession, "ENABLE_SPEED_CHANGE", 0, TASKINI_NAME) ? true : false );

}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// スレッド動作定義


//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int LinkswManager::ThreadFirst()
{
	// パラメータ読込み
	while( TRUE )
	{
		if( GetParamStatic() &&
			GetParam() ) break;
		LOG(em_MSG), "[%s] パラメータ読込失敗(DB接続異常？！)。リトライ中・・・", my_sThreadName);
		Sleep(3000);
	}
	// iniファイル指定のカメラ入力CHを設定

	// 2017.02.27 Master/SlaveをDB指定 --------------------------------------------->>>>
	//for( int ii=0; ii<NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; ii++ )
	//{
	//	for( int jj=0; jj<MAX_CAMLINKSW_OUTPUT; jj++ )
	//	{
	//		if( 0 >= m_nSelectCam[ii][jj] || MAX_CAMLINKSW_ALLINPUT < m_nSelectCam[ii][jj] ) continue;
	//		m_Param.linksw[ ii ].nSelectCam[ jj ] = m_nSelectCam[ii][jj];
	//		LOG(em_INF), "[%s] カメラ選択設定(INI) 取得 [ID=%d, ボード%d=%d]", my_sThreadName, ii, jj+1, m_Param.linksw[ ii ].nSelectCam[ jj ]);
	//	}
	//}

	// やる事が煩雑になったので別関数に
	GetParamIni();

	// 2017.02.27 Master/SlaveをDB指定 <<<<---------------------------------------------

	// 算出パラメータ取得
	CalcParam();

	// 232C 初期接続
	if( ! Open(true) )
	{
		Close();
	}

	//// イベント設定
	HANDLE hArray[] =
	{
		m_evReTry,
		m_evSendEnd,
		m_evRecvEnd,
		m_evAnswerTimeOut,
		gque_Deli.g_evSem,
		m_evTimerCycleCheck,
		m_evCounterCheck
	};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	m_nEventWaitNum = sizeof(hArray)/sizeof(HANDLE);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int LinkswManager::ThreadLast()
{
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void LinkswManager::ThreadEvent(int nEventNo)
{
	COMMON_QUE * pQue;

	//// シグナルの条件
	enum
	{
		EV_RETRY,							// 再接続開始
		EV_SENDEND,							// 送信完了
		EV_RECVEND,							// 受信完了
		EV_ANSWERTIMEOUT,					// 送信アンサータイムアウト
		EV_QUE,								// メールスロット受信
		EV_TIMERCYCLECHECK,					// 定周期チェックタイマ
		EV_COUNTERCHECK,					// カウンタチェック
		EV_END
	};

	//// シグナル条件分け
	switch( nEventNo )
	{
//-----------------------------------------------------------------------------------------------
	case EV_RETRY:							// 再接続開始
		if( ! Open(false) )
		{
			Close();
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_SENDEND:						// 送信完了
		// コマンドを送信したので、受信開始
		if( ! Recv() )
		{
			Close();
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_RECVEND:						// 受信完了
		// 受信完了
		CancelWaitableTimer(m_evAnswerTimeOut);

		// 受信データチェック
		if( ! CheckDataRecv() )
		{
			Close();
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_ANSWERTIMEOUT:					// 応答伝文タイムアウト
		if( m_bIsConnect )
		{
			LOG(em_ERR), "[%s] 応答伝文タイムアウト send=%s", my_sThreadName, m_cSendBuf);
			syslog(211, "[応答伝文タイムアウト:send=%s]", m_cSendBuf);
		}
		Close();		// 再接続
		break;

//-----------------------------------------------------------------------------------------------
	case EV_TIMERCYCLECHECK:				// 定周期チェックタイマ
		// キュー生成
		pQue = new COMMON_QUE;
		pQue->nEventNo = FACT_CYCLE_CHECK;
		pQue->nLineNo = getlineid();

		// コマンド解釈
		if( ! SetDataMailCommand(pQue) )
		{
			Close();
		}
		SAFE_DELETE(pQue);
		break;

//-----------------------------------------------------------------------------------------------
	case EV_COUNTERCHECK:					// カウンタチェック
		// キュー生成
		pQue = new COMMON_QUE;
		pQue->nEventNo = FACT_COUNTER_CHECK;
		pQue->nLineNo = getlineid();

		// コマンド解釈
		if( ! SetDataMailCommand(pQue) )
		{
			Close();
		}
		SAFE_DELETE(pQue);
		break;

//-----------------------------------------------------------------------------------------------
	case EV_QUE:							// メールスロット受信
		// キュー取り出し
		pQue = gque_Deli.GetFromHead();

		// コマンド解釈
		if( ! SetDataMailCommand(pQue) )
		{
			Close();
		}
		SAFE_DELETE(pQue);
		break;

//-----------------------------------------------------------------------------------------------
	default:
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo);
		return;
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// シーケンス制御


//------------------------------------------
// コマンド解釈
// COMMON_QUE * pQue メールスロットデータ
// 戻り値 復帰情報
//------------------------------------------
bool LinkswManager::SetDataMailCommand(COMMON_QUE * pQue)
{
	// カメラリンクスイッチ接続中？
	if( ! m_bIsConnect )
	{
		LOG(em_WAR), "[%s] 回線切断状態でコマンド受信", my_sThreadName);
		return true;
	}

	// 20190205 要求に対する応答識別用 --->>>
	memcpy(&m_RecvQue, pQue, sizeof(COMMON_QUE));
	// 20190205 要求に対する応答識別用 ---<<<

	// イベントNoにより処理分け
	switch( pQue->nEventNo )
	{
//---------------------------------------------------------
	case FACT_TO_LINKSW_01:									// パラメータ変更要求
		// 動的パラメータ読込み
		GetParam();

		// 現時点では動的パラメータは無しなので、このまま終了
		return true;

		break;

//---------------------------------------------------------
	case FACT_TO_LINKSW_02:									// 検査用設定コマンド
		// パラメータ読込み
		GetParamStatic();
		GetParam();
		// iniファイル指定のカメラ入力CHを設定
		// 2017.02.27 Master/SlaveをDB指定 ---------->>>>
		//for( int ii=0; ii<NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; ii++ )
		//{
		//	for( int jj=0; jj<MAX_CAMLINKSW_OUTPUT; jj++ )
		//	{
		//		if( 0 >= m_nSelectCam[ii][jj] || MAX_CAMLINKSW_ALLINPUT < m_nSelectCam[ii][jj] ) continue;
		//		m_Param.linksw[ ii ].nSelectCam[ jj ] = m_nSelectCam[ii][jj];
		//	}
		//}

		// Iniパラメータを反映(やる事が煩雑化したので別関数)
		GetParamIni();
		// 2017.02.27 Master/SlaveをDB指定 <<<<----------


		// 算出パラメータ取得
		CalcParam();
		// カウンタリセットフレームNo リセット
		m_nResetFNo = 0;
		// 送信準備
		m_emMainMode = MODE_START_INIT;
		m_nSubMode = 0;
		m_nCamMode = 0;
		m_nApxMode = 0;
		m_nAocMode = 0;
		m_nAocSubMode = 0;
		m_nRegSizeMode = 0;
		m_nSwMode = 0;
		break;

//---------------------------------------------------------
	case FACT_TO_LINKSW_03:									// カメラ選択設定コマンド
		// ハーフ版カメラリンクSWの場合は選択ボードのチェックを実施
		if (m_bHalfFlg[pQue->fl.data[0]]) {
			if (! HARF_CC_SOCK_EXIST[pQue->fl.data[1] - 1]) {
				LOG(em_ERR), "[%s] <%d> ハーフ版に存在しないボードが選択されたため無視(board=%d, cam=%d)", my_sThreadName, pQue->fl.data[0], pQue->fl.data[1], pQue->fl.data[2]);
				// 統括に異常を通知
				// 20190205 要求に対する応答識別用 --->>>
				//Send_ToDefect_StartInit(false);
				Send_ToDefect_SelectInit(false);
				// 20190205 要求に対する応答識別用 ---<<<
				return true;
			}
		}

		// パラメータ設定
		m_Param.linksw[ pQue->fl.data[0] ].nSelectCam[ pQue->fl.data[1]-1 ] = (short)pQue->fl.data[2];

		// 2017.02.27 Master/SlaveをDB指定 ---------->>>>

		// Master/Slave区分 1ならMaster、-1ならSlave それ以外ならMaster(何も指定しなかった場合、0なのでMaster)
		switch(pQue->fl.data[3]){
			case 1:
				m_Param.linksw[ pQue->fl.data[0] ].emChSelMode[ pQue->fl.data[1]-1 ] = CH_SELECT_MASTER;
				break;
			case -1:
				m_Param.linksw[ pQue->fl.data[0] ].emChSelMode[ pQue->fl.data[1]-1 ] = CH_SELECT_SLAVE;
				break;
			default:
				m_Param.linksw[ pQue->fl.data[0] ].emChSelMode[ pQue->fl.data[1]-1 ] = CH_SELECT_MASTER;
				break;
		}

#ifdef ENABLE_MAIL_FORCIBLY_MASTER

		if( CH_SELECT_MASTER == m_Param.linksw[ pQue->fl.data[0] ].emChSelMode[ pQue->fl.data[1]-1 ] ){

			SetForciblyMaster( pQue->fl.data[0], pQue->fl.data[1]-1, pQue->fl.data[2]);
		}

#endif
		// 2017.02.27 Master/SlaveをDB指定 <<<<----------



		CalcParam();
		// 送信準備
		// 2020.05.07 対象カメラのみ設定 ---------->>>>
		if (-1 == pQue->fl.data[3]) {
			m_emMainMode = MODE_START_SELECT_INIT;
			for (int ii = 0; ii<NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; ii++)
			{
				for (int jj = 0; jj<MAX_CAMLINKSW_OUTPUT; jj++)
				{
					m_Param.linksw[ii].bChSelCam[jj] = FALSE;							// カメラ選択設定 OFF
				}
			}
			m_Param.linksw[pQue->fl.data[0]].bChSelCam[pQue->fl.data[1] - 1] = TRUE;
		}
		else {
			m_emMainMode = MODE_START_INIT;
		}
		//m_emMainMode = MODE_START_INIT;
		// 2020.05.07 対象カメラのみ設定 <<<<----------
		m_nSubMode = 0;
		m_nCamMode = 0;
		m_nApxMode = 0;
		m_nAocMode = 0;
		m_nAocSubMode = 0;
		m_nRegSizeMode = 0;
		m_nSwMode = 0;
		break;

//---------------------------------------------------------
	case FACT_TO_LINKSW_04:									// 途中参加設定コマンド
		// 途中参加表裏区分 セット
		m_nSlaveJoinTorB = pQue->fl.data[0];
		// 途中参加フレームNo セット
		m_nSlaveJoinFNo = (DWORD)pQue->fl.data[1];
		// 送信準備
		m_emMainMode = MODE_SLAVE_JOIN;
		m_nSubMode = 0;
		m_nCamMode = 0;
		m_nApxMode = 0;
		m_nAocMode = 0;
		m_nAocSubMode = 0;
		m_nRegSizeMode = 0;
		m_nSwMode = 0;
		break;

//---------------------------------------------------------
	case FACT_TO_LINKSW_05:									// 高速リセット通知
		// カウンタリセットフレームNo セット
		m_nResetFNo = (DWORD)pQue->fl.data[0];
		// 送信準備
		m_emMainMode = MODE_COUNTER_RESET;
		m_nSubMode = 0;
		m_nCamMode = 0;
		m_nApxMode = 0;
		m_nAocMode = 0;
		m_nAocSubMode = 0;
		m_nRegSizeMode = 0;
		m_nSwMode = 0;
		break;

//---------------------------------------------------------
	case FACT_TO_LINKSW_06:									// AOCシリアルスピード変更
		// 送信準備
		{
			// 出力側の番号（複数リンクスイッチがあれば、通番）
			const int out_board_index = pQue->fl.data[0];
			if( pQue->fl.data[0] <= 0 ) {
				LOG(em_ERR), "[%s] 選択ボード異常(board=%d, speed=%d)", my_sThreadName, pQue->fl.data[0], pQue->fl.data[1]);
				return true;
			}
			if (pQue->fl.data[1] <= 0) {
				LOG(em_ERR), "[%s] 選択スピード異常(board=%d, speed=%d)", my_sThreadName, pQue->fl.data[0], pQue->fl.data[1]);
				return true;
			}

			// 出力側番号から、ひもづく入力側番号(camindex)を取得する。
			int nLinkSW = 0;
			int nFPGA = 0;
			int nChannel = -1;


			for( int iLinkSW =0;iLinkSW<NUM_CAMLINKSW; iLinkSW ++){
				for( int iFPGA=0; iFPGA<MAX_CAMLINKSW_FPGA; iFPGA++){
					const short *linksw_clients = m_Param.nCamSelBoardNo[iFPGA + MAX_CAMLINKSW_FPGA*iLinkSW];
					for( int iCh=0; iCh<MAX_CAMLINKSW_ALLINPUT; iCh++){
						const int connected_output = linksw_clients[iCh] + iFPGA*MAX_CAMLINKSW_OUTPUT + iLinkSW * MAX_CAMLINKSW_ALLOUTPUT;
						if( connected_output == out_board_index ){
							//nLinkSW = iLinkSW;
							nLinkSW = linksw_clients[iCh];
							////nFPGA = iFPGA;
							////nChannel = iCh+1;
							nFPGA  = iLinkSW * MAX_CAMLINKSW_FPGA + iCh / MAX_CAMLINKSW_INPUT;
							nChannel  = iCh % MAX_CAMLINKSW_INPUT;
						}
					}
				}
			}


			if( nChannel >= 0 ){
				m_emMainMode = MODE_CHANGE_AOC_SERIALSPEED;
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nAocSubMode = 0;
				m_nRegSizeMode = 0;
				m_nApxMode= nLinkSW;
				m_nSwMode = nFPGA;
				m_nAocMode = nChannel;

				int nAOCNo = m_nAocMode + m_nSwMode *MAX_CAMLINKSW_INPUT;
				LOG(em_MSG), "[%s] 入力通番[%d]のマスターは[%d-%d-%02d] (通番%d)", 
					my_sThreadName, out_board_index, 
					nLinkSW, nFPGA, nChannel,
					nAOCNo
				);
				m_Param.nSerialSpeeds[ nAOCNo ] = pQue->fl.data[1];
			}
			else{
				LOG(em_MSG), "[%s] 入力通番[%d]はスレーブ", 
					my_sThreadName, out_board_index);
				return true;
			}
		}
		break;
//---------------------------------------------------------
	case FACT_CYCLE_CHECK:									// 定周期チェック
		// 送信準備
		m_emMainMode = MODE_CYCLE_CHECK;
		m_nSubMode = 0;
		m_nCamMode = 0;
		m_nApxMode = 0;
		m_nAocMode = 0;
		m_nAocSubMode = 0;
		m_nRegSizeMode = 0;
		m_nSwMode = 0;
		break;

//---------------------------------------------------------
	case FACT_COUNTER_CHECK:								// カウンタチェック
		// バッファクリア
		memset(m_nCounterNow, 0x00, sizeof(m_nCounterNow));
		memset(m_nCounterSet, 0x00, sizeof(m_nCounterSet));
		// 送信準備
		m_emMainMode = MODE_COUNTER_CHECK;
		m_nSubMode = 0;
		m_nCamMode = 0;
		m_nApxMode = 0;
		m_nAocMode = 0;
		m_nAocSubMode = 0;
		m_nRegSizeMode = 0;
		m_nSwMode = 0;
		break;

//---------------------------------------------------------
	default:
		// ありえない！！
		return false;
	}

	// ここまできたら カメラリンクスイッチ へ通知
	if( ! CheckNextSend() ) return false;
	return true;
}

//------------------------------------------
// 次送信制御
// 戻り値 復帰情報
//------------------------------------------
bool LinkswManager::CheckNextSend()
{
	//// 前準備
	memset(m_cSendBuf, 0x00, sizeof(m_cSendBuf));	// 送信バッファ クリア
	//// サブモード変更 (機器毎の設定をしている場合は、サブモードを維持)
	if( 0 == m_nCamMode &&
		0 == m_nApxMode &&
		0 == m_nAocMode &&
		0 == m_nAocSubMode &&
		0 == m_nRegSizeMode ) m_nSubMode++;


	//// 処理分け
	switch( m_emMainMode )
	{
//---------------------------------------------------------
	////////////////////////////////////
	// オープンチェック
	////////////////////////////////////
	case MODE_OPEN_CHECK:
		if( 1 == m_nSubMode &&
			0 == m_nApxMode &&
			0 == m_nAocMode &&
			0 == m_nAocSubMode &&
			0 == m_nRegSizeMode &&
			0 == m_nCamMode )
		{
			LOG(em_MSG), "[%s] <%d> オープンチェック 開始 -------------------------------------------------->>>", my_sThreadName, m_nSwMode);
			ChangeWaitEventNum( true );
		}

		LOG(em_INF), "[%s] <%d> スイッチFPGA ステータスレジスタ", my_sThreadName, m_nSwMode);

		sprintf(m_cSendBuf, "@%02XR002\r", m_nSwMode);																	// スイッチFPGA ステータスレジスタ
		break;

//---------------------------------------------------------
	////////////////////////////////////
	// 初期化
	////////////////////////////////////
	case MODE_START_UP:
		if( 1 == m_nSubMode &&
			0 == m_nApxMode &&
			0 == m_nAocMode &&
			0 == m_nAocSubMode &&
			0 == m_nRegSizeMode &&
			0 == m_nCamMode )
		{
			LOG(em_MSG), "[%s] <%d> 初期化 開始 -------------------------------------------------->>>", my_sThreadName, m_nSwMode);
			ChangeWaitEventNum( true );
		}

		if( 1 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> カメラ%.2d Opt-C:Link Line Buffer 格納バス幅レジスタ [%d]", my_sThreadName, m_nSwMode, m_nCamMode+1, m_Param.nLineBuffer);

			int nAddr = 0x102 + (0x010 * m_nCamMode);
			sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, m_Param.nLineBuffer);							// Opt-C:Link Line Buffer 格納バス幅レジスタ

			// カメラ毎のモード設定
			if( m_nCamMode == MAX_CAMLINKSW_ALLINPUT-1 )	m_nCamMode = 0;					// 最後のカメラ時
			else											m_nCamMode++;
		}
		else if( 2 == m_nSubMode )
		{
			//// レジスタサイズ繰り返しモード設定
			if( 0 == m_nRegSizeMode )
			{
				LOG(em_MSG), "[%s] <%d> カメラ%.2d Opt-C:Link YSize レジスタ [%d]", my_sThreadName, m_nSwMode, m_nCamMode+1, m_Param.nYSize);
				m_nRegSizeMode = 2;
			}

			if( 2 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> カメラ%.2d Opt-C:Link YSize レジスタ (7:0)", my_sThreadName, m_nSwMode, m_nCamMode+1);

				int nAddr = 0x104 + (0x010 * m_nCamMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, LOBYTE(m_Param.nYSize));						// Opt-C:Link YSize レジスタ (7:0)
			}
			else if( 1 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> カメラ%.2d Opt-C:Link YSize レジスタ (15:8)", my_sThreadName, m_nSwMode, m_nCamMode+1);

				int nAddr = 0x105 + (0x010 * m_nCamMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, HIBYTE(m_Param.nYSize));						// Opt-C:Link YSize レジスタ (15:8)

				// カメラ毎のモード設定
				if( m_nCamMode == MAX_CAMLINKSW_ALLINPUT-1 )	m_nCamMode = 0;				// 最後のカメラ時
				else											m_nCamMode++;
			}
		}
		else if( 3 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> ボード%.2d モードレジスタ (途中参加カウンタリード選択, 遅延カウント信号選択) [%d]", my_sThreadName, m_nSwMode, m_nApxMode+1, 64 + 8 * COUNTER_MODE);

			int nAddr = 0x010 + (0x010 * m_nApxMode);
			sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 64 + 8 * COUNTER_MODE);							// ボード モードレジスタ (途中参加カウンタリード選択, 遅延カウント信号選択)

			// ボード毎のモード設定
			if( m_nApxMode == MAX_CAMLINKSW_OUTPUT-1 )		m_nApxMode = 0;					// 最後のボード時
			else											m_nApxMode++;
		}
		else if( 4 == m_nSubMode )
		{
			//// レジスタサイズ繰り返しモード設定
			if( 0 == m_nRegSizeMode )
			{
				LOG(em_MSG), "[%s] <%d> ボード%.2d 途中参加レジスタ [%d]", my_sThreadName, m_nSwMode, m_nApxMode+1, 0);
				m_nRegSizeMode = 8;
			}

			if( 8 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (7:0)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x018 + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (7:0) Write
			}
			else if( 7 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (15:8)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x019 + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (15:8) Write
			}
			else if( 6 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (23:16)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01A + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (23:16) Write
			}
			else if( 5 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (31:24)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01B + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (31:24) Write
			}
			else if( 4 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (39:32)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01C + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (39:32) Write
			}
			else if( 3 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (47:40)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01D + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (47:40) Write
			}
			else if( 2 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (55:48)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01E + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (55:48) Write
			}
			else if( 1 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (63:56)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01F + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (63:56) Write

				// ボード毎のモード設定
				if( m_nApxMode == MAX_CAMLINKSW_OUTPUT-1 )		m_nApxMode = 0;				// 最後のボード時
				else											m_nApxMode++;
			}
		}
		else if( 5 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> ボード%.2d カメラ選択レジスタ [%d(0:出力停止 1～8:入力CH)]", my_sThreadName, m_nSwMode, m_nApxMode+1, m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]);

			int nAddr = 0x011 + (0x010 * m_nApxMode);
			sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]);	// ボード カメラ選択レジスタ

			// ボード毎のモード設定
			if( m_nApxMode == MAX_CAMLINKSW_OUTPUT-1 )		m_nApxMode = 0;					// 最後のボード時
			else											m_nApxMode++;
		}
		else if( 6 == m_nSubMode )
		{
			// ボード番号
			int nBoard = m_Param.nCamSelBoardNo[m_nSwMode][m_nCamMode];

			// ボード番号 初期値 の場合
			if( 0 == nBoard )
			{
				// スイッチマスタ and スイッチマスタ側のカメラ
				if( m_Param.linksw[m_nSwMode].bStatus && MAX_CAMLINKSW_INPUT >= m_nCamMode+1 )
				{
					// スイッチスレーブ側のボートと接続するカメラを検索
					for( int ii=0; ii<MAX_CAMLINKSW_OUTPUT; ii++ )
					{
						if( m_nCamMode+1 == m_Param.linksw[m_nSwMode + 1].nSelectCam[ii] )
						{
							nBoard = 16;	// Arria5
							break;
						}
					}
				}
				// スイッチスレーブ and スイッチスレーブ側のカメラ
				if( ! m_Param.linksw[m_nSwMode].bStatus && MAX_CAMLINKSW_INPUT < m_nCamMode+1 )
				{
					// スイッチマスタ側のボートと接続するカメラを検索
					for( int ii=0; ii<MAX_CAMLINKSW_OUTPUT; ii++ )
					{
						if( m_nCamMode+1 == m_Param.linksw[m_nSwMode - 1].nSelectCam[ii] )
						{
							nBoard = 16;	// Arria5
							break;
						}
					}
				}
			}

			LOG(em_INF), "[%s] <%d> カメラ%.2d ボード選択レジスタ [%d(1～15:ボードNo 16:Arria5)]", my_sThreadName, m_nSwMode, m_nCamMode+1, nBoard);

			int nAddr = 0x108 + (0x010 * m_nCamMode);
			sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, nBoard);											// カメラ ボード選択レジスタ

			// カメラ毎のモード設定
			if( m_nCamMode == MAX_CAMLINKSW_ALLINPUT-1 )	m_nCamMode = 0;					// 最後のカメラ時
			else											m_nCamMode++;
		}
		else if( 7 == m_nSubMode )
		{
			//// AOCサブモード変更
			if( 0 == m_nRegSizeMode )	m_nAocSubMode++;

			if( 1 == m_nAocSubMode &&
				0 == m_nRegSizeMode )
			{
				LOG(em_MSG), "[%s] <%d> AOC%.2d Line Buffer 格納バス幅レジスタ [%d]", my_sThreadName, m_nSwMode, m_nAocMode+1, m_Param.nLineBuffer);
			}

			if( 1 == m_nAocSubMode )
			{
				//// レジスタサイズ繰り返しモード設定
				if( 0 == m_nRegSizeMode )	m_nRegSizeMode = 2;

				if( 2 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Address レジスタ (7:0)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x180 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x70);									// AOC Address モードレジスタ (7:0)
				}
				else if( 1 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Address レジスタ (15:8)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x181 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x00);									// AOC Address モードレジスタ (15:8)
				}
			}
			else if( 2 == m_nAocSubMode )
			{
				//// レジスタサイズ繰り返しモード設定
				if( 0 == m_nRegSizeMode )	m_nRegSizeMode = 4;

				if( 4 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data レジスタ (7:0)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x182 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x00);									// AOC Write Data レジスタ (7:0)
				}
				else if( 3 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data レジスタ (15:8)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x183 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, m_Param.nLineBuffer);					// AOC Write Data レジスタ (15:8)
				}
				else if( 2 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data レジスタ (23:16)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x184 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x00);									// AOC Write Data レジスタ (23:16)
				}
				else if( 1 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data レジスタ (31:24)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x185 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x00);									// AOC Write Data レジスタ (31:24)
				}
			}
			else if( 3 == m_nAocSubMode )
			{
				LOG(em_INF), "[%s] <%d> AOC%.2d Ctrl レジスタ (Write Access)", my_sThreadName, m_nSwMode, m_nAocMode+1);

				int nAddr = 0x18A + (0x010 * m_nAocMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x11);										// AOC Ctrl レジスタ (Write Access)
			}
			else if( 4 == m_nAocSubMode )
			{
				LOG(em_INF), "[%s] <%d> AOC%.2d Ctrl レジスタ (Read Access)", my_sThreadName, m_nSwMode, m_nAocMode+1);

				int nAddr = 0x18A + (0x010 * m_nAocMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);													// AOC Ctrl レジスタ (Read Access)
			}
			else if( 5 == m_nAocSubMode )
			{
				LOG(em_INF), "[%s] <%d> AOC%.2d Ctrl レジスタ (送信完了リセット)", my_sThreadName, m_nSwMode, m_nAocMode+1);

				int nAddr = 0x18A + (0x010 * m_nAocMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x04);										// AOC Ctrl レジスタ (送信完了リセット)

				// AOC毎のモード設定
				if( m_nAocMode == MAX_CAMLINKSW_INPUT-1 )	m_nAocMode = 0;					// 最後のAOC時
				else										m_nAocMode++;
			}
		}
		else if( 8 == m_nSubMode )
		{
			//// AOCサブモード変更
			if( 0 == m_nRegSizeMode )	m_nAocSubMode++;

			if( 1 == m_nAocSubMode &&
				0 == m_nRegSizeMode )
			{
				LOG(em_MSG), "[%s] <%d> AOC%.2d YSize レジスタ [%d]", my_sThreadName, m_nSwMode, m_nAocMode+1, m_Param.nYSize);
			}

			if( 1 == m_nAocSubMode )
			{
				//// レジスタサイズ繰り返しモード設定
				if( 0 == m_nRegSizeMode )	m_nRegSizeMode = 2;

				if( 2 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Address レジスタ (7:0)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x180 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x78);									// AOC Address モードレジスタ (7:0)
				}
				else if( 1 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Address レジスタ (15:8)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x181 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x00);									// AOC Address モードレジスタ (15:8)
				}
			}
			else if( 2 == m_nAocSubMode )
			{
				//// レジスタサイズ繰り返しモード設定
				if( 0 == m_nRegSizeMode )	m_nRegSizeMode = 4;

				if( 4 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data レジスタ (7:0)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x182 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, LOBYTE(m_Param.nYSize));					// AOC Write Data レジスタ (7:0)
				}
				else if( 3 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data レジスタ (15:8)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x183 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, HIBYTE(m_Param.nYSize));					// AOC Write Data レジスタ (15:8)
				}
				else if( 2 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data レジスタ (23:16)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x184 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x00);									// AOC Write Data レジスタ (23:16)
				}
				else if( 1 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data レジスタ (31:24)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x185 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x00);									// AOC Write Data レジスタ (31:24)
				}
			}
			else if( 3 == m_nAocSubMode )
			{
				LOG(em_INF), "[%s] <%d> AOC%.2d Ctrl レジスタ (Write Access)", my_sThreadName, m_nSwMode, m_nAocMode+1);

				int nAddr = 0x18A + (0x010 * m_nAocMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x11);										// AOC Ctrl レジスタ (Write Access)
			}
			else if( 4 == m_nAocSubMode )
			{
				LOG(em_INF), "[%s] <%d> AOC%.2d Ctrl レジスタ (Read Access)", my_sThreadName, m_nSwMode, m_nAocMode+1);

				int nAddr = 0x18A + (0x010 * m_nAocMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);													// AOC Ctrl レジスタ (Read Access)
			}
			else if( 5 == m_nAocSubMode )
			{
				LOG(em_INF), "[%s] <%d> AOC%.2d Ctrl レジスタ (送信完了リセット)", my_sThreadName, m_nSwMode, m_nAocMode+1);

				int nAddr = 0x18A + (0x010 * m_nAocMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x04);										// AOC Ctrl レジスタ (送信完了リセット)

				// AOC毎のモード設定
				if( m_nAocMode == MAX_CAMLINKSW_INPUT-1 )	m_nAocMode = 0;					// 最後のAOC時
				else										m_nAocMode++;
			}
		}
		else if( 10 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> AOC%.2d CCモードレジスタ [%d(0:FPGA 1:Board)]", my_sThreadName, m_nSwMode, m_nAocMode+1, m_bCCMode);

			int nAddr = 0x18B + (0x010 * m_nAocMode);
			sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, m_bCCMode);										// AOC CCモードレジスタ

			// AOC毎のモード設定
			if( m_nAocMode == MAX_CAMLINKSW_INPUT-1 )		m_nAocMode = 0;					// 最後のAOC時
			else											m_nAocMode++;
		}
		else if( 9 == m_nSubMode )
		{
			//// AOCサブモード変更
			if( 0 == m_nRegSizeMode )	m_nAocSubMode++;

			if( 1 == m_nAocSubMode &&
				0 == m_nRegSizeMode )
			{
				LOG(em_MSG), "[%s] <%d> AOC%.2d Serial レジスタ [%d]", my_sThreadName, m_nSwMode, m_nAocMode+1, 9600);
			}

			if( 1 == m_nAocSubMode )
			{
				//// レジスタサイズ繰り返しモード設定
				if( 0 == m_nRegSizeMode )	m_nRegSizeMode = 2;

				if( 2 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Address レジスタ (7:0)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x180 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x7C);									// AOC Address モードレジスタ (7:0)
				}
				else if( 1 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Address レジスタ (15:8)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x181 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x00);									// AOC Address モードレジスタ (15:8)
				}
			}
			else if( 2 == m_nAocSubMode )
			{
				//// レジスタサイズ繰り返しモード設定
				if( 0 == m_nRegSizeMode )	m_nRegSizeMode = 4;

				if( 4 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data レジスタ (7:0)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x182 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, LOBYTE(m_Param.nSamplingTiming()));					// AOC Write Data レジスタ (7:0)
				}
				else if( 3 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data レジスタ (15:8)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x183 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, HIBYTE(m_Param.nSamplingTiming()));					// AOC Write Data レジスタ (15:8)
				}
				else if( 2 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data レジスタ (23:16)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x184 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, LOBYTE(m_Param.nSamplingDelay()));									// AOC Write Data レジスタ (23:16)
				}
				else if( 1 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data レジスタ (31:24)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x185 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, HIBYTE(m_Param.nSamplingDelay()));									// AOC Write Data レジスタ (31:24)
				}
			}
			else if( 3 == m_nAocSubMode )
			{
				LOG(em_INF), "[%s] <%d> AOC%.2d Ctrl レジスタ (Write Access)", my_sThreadName, m_nSwMode, m_nAocMode+1);

				int nAddr = 0x18A + (0x010 * m_nAocMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x11);										// AOC Ctrl レジスタ (Write Access)
			}
			else if( 4 == m_nAocSubMode )
			{
				LOG(em_INF), "[%s] <%d> AOC%.2d Ctrl レジスタ (Read Access)", my_sThreadName, m_nSwMode, m_nAocMode+1);

				int nAddr = 0x18A + (0x010 * m_nAocMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);													// AOC Ctrl レジスタ (Read Access)
			}
			else if( 5 == m_nAocSubMode )
			{
				LOG(em_INF), "[%s] <%d> AOC%.2d Ctrl レジスタ (送信完了リセット)", my_sThreadName, m_nSwMode, m_nAocMode+1);

				int nAddr = 0x18A + (0x010 * m_nAocMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x04);										// AOC Ctrl レジスタ (送信完了リセット)

				// AOC毎のモード設定
				if( m_nAocMode == MAX_CAMLINKSW_INPUT-1 )	m_nAocMode = 0;					// 最後のAOC時
				else										m_nAocMode++;
			}
		}
		else if( 11 == m_nSubMode )
		{
			LOG(em_MSG), "[%s] <%d> 初期化 終了 --------------------------------------------------<<<" , my_sThreadName, m_nSwMode);

			// 初期化 → 設定待ち
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nApxMode = 0;
			m_nAocMode = 0;
			m_nAocSubMode = 0;
			m_nRegSizeMode = 0;
			m_nSwMode = 0;
			ChangeWaitEventNum( false );													// 1シーケンス正常完了
		}
		else
		{
			LOG(em_ERR), "[%s] <%d> サブモード異常 [mode = 初期化, sub_mode = %d]", my_sThreadName, m_nSwMode, m_nSubMode);
			syslog(210, "[<%d> mode = 初期化 sub_mode = %d]", m_nSwMode, m_nSubMode);
			ChangeWaitEventNum( false );
			return false;
		}
		break;

//---------------------------------------------------------
	////////////////////////////////////
	// 検査開始設定
	////////////////////////////////////
	case MODE_START_INIT:
		if( 1 == m_nSubMode &&
			0 == m_nApxMode &&
			0 == m_nAocMode &&
			0 == m_nAocSubMode &&
			0 == m_nRegSizeMode &&
			0 == m_nCamMode )
		{
			LOG(em_MSG), "[%s] <%d> 検査開始設定 開始 -------------------------------------------------->>>", my_sThreadName, m_nSwMode);
			ChangeWaitEventNum( true );
		}

		if( 1 == m_nSubMode )
		{
			//// レジスタサイズ繰り返しモード設定
			if( 0 == m_nRegSizeMode )
			{
				LOG(em_MSG), "[%s] <%d> ボード%.2d 途中参加レジスタ [%d]", my_sThreadName, m_nSwMode, m_nApxMode+1, 0);
				m_nRegSizeMode = 8;
			}

			if( 8 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (7:0)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x018 + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (7:0) Write
			}
			else if( 7 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (15:8)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x019 + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (15:8) Write
			}
			else if( 6 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (23:16)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01A + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (23:16) Write
			}
			else if( 5 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (31:24)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01B + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (31:24) Write
			}
			else if( 4 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (39:32)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01C + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (39:32) Write
			}
			else if( 3 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (47:40)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01D + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (47:40) Write
			}
			else if( 2 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (55:48)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01E + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (55:48) Write
			}
			else if( 1 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (63:56)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01F + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (63:56) Write

				// ボード毎のモード設定
				if( m_nApxMode == MAX_CAMLINKSW_OUTPUT-1 )		m_nApxMode = 0;				// 最後のボード時
				else											m_nApxMode++;
			}
		}
		else if( 2 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> ボード%.2d カメラ選択レジスタ [%d(0:出力停止 1～8:入力CH)]", my_sThreadName, m_nSwMode, m_nApxMode+1, m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]);

			int nAddr = 0x011 + (0x010 * m_nApxMode);
			sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]);	// ボード カメラ選択レジスタ

			// ボード毎のモード設定
			if( m_nApxMode == MAX_CAMLINKSW_OUTPUT-1 )		m_nApxMode = 0;					// 最後のボード時
			else											m_nApxMode++;
		}
		else if( 3 == m_nSubMode )
		{
			// ボード番号
			int nBoard = m_Param.nCamSelBoardNo[m_nSwMode][m_nCamMode];

			// ボード番号 初期値 の場合
			if( 0 == nBoard )
			{
				// スイッチマスタ and スイッチマスタ側のカメラ
				if( m_Param.linksw[m_nSwMode].bStatus && MAX_CAMLINKSW_INPUT >= m_nCamMode+1 )
				{
					// スイッチスレーブ側のボートと接続するカメラを検索
					for( int ii=0; ii<MAX_CAMLINKSW_OUTPUT; ii++ )
					{
						if( m_nCamMode+1 == m_Param.linksw[m_nSwMode + 1].nSelectCam[ii] )
						{
							nBoard = 16;	// Arria5
							break;
						}
					}
				}
				// スイッチスレーブ and スイッチスレーブ側のカメラ
				if( ! m_Param.linksw[m_nSwMode].bStatus && MAX_CAMLINKSW_INPUT < m_nCamMode+1 )
				{
					// スイッチマスタ側のボートと接続するカメラを検索
					for( int ii=0; ii<MAX_CAMLINKSW_OUTPUT; ii++ )
					{
						if( m_nCamMode+1 == m_Param.linksw[m_nSwMode - 1].nSelectCam[ii] )
						{
							nBoard = 16;	// Arria5
							break;
						}
					}
				}
			}

			LOG(em_INF), "[%s] <%d> カメラ%.2d ボード選択レジスタ [%d(1～15:ボードNo 16:Arria5)]", my_sThreadName, m_nSwMode, m_nCamMode+1, nBoard);

			int nAddr = 0x108 + (0x010 * m_nCamMode);
			sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, nBoard);											// カメラ ボード選択レジスタ

			// カメラ毎のモード設定
			if( m_nCamMode == MAX_CAMLINKSW_ALLINPUT-1 )	m_nCamMode = 0;					// 最後のカメラ時
			else											m_nCamMode++;
		}
		else if( 4 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> AOC%.2d CCモードレジスタ [%d(0:FPGA 1:Board)]", my_sThreadName, m_nSwMode, m_nAocMode+1, m_bCCMode);

			int nAddr = 0x18B + (0x010 * m_nAocMode);
			sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, m_bCCMode);										// AOC CCモードレジスタ

			// AOC毎のモード設定
			if( m_nAocMode == MAX_CAMLINKSW_INPUT-1 )		m_nAocMode = 0;					// 最後のAOC時
			else											m_nAocMode++;
		}
		else if( 5 == m_nSubMode )
		{
			LOG(em_MSG), "[%s] <%d> 検査開始設定 終了 --------------------------------------------------<<<" , my_sThreadName, m_nSwMode);

			// 検査開始設定 → 設定待ち
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nApxMode = 0;
			m_nAocMode = 0;
			m_nAocSubMode = 0;
			m_nRegSizeMode = 0;
			m_nSwMode = 0;
			ChangeWaitEventNum( false );													// 1シーケンス正常完了

			// 統括管理 へ カメラリンクスイッチ設定完了通知
			// 20190205 要求に対する応答識別用 --->>>
			//Send_ToDefect_StartInit();
			if (FACT_TO_LINKSW_02 == m_RecvQue.nEventNo) {
				Send_ToDefect_StartInit();
			}
			else {
				Send_ToDefect_SelectInit();
			}
			// 20190205 要求に対する応答識別用 ---<<<
		}
		else
		{
			LOG(em_ERR), "[%s] <%d> サブモード異常 [mode = 検査開始設定, sub_mode = %d]", my_sThreadName, m_nSwMode, m_nSubMode);
			syslog(210, "[<%d> mode = 検査開始設定 sub_mode = %d]", m_nSwMode, m_nSubMode);
			ChangeWaitEventNum( false );
			return false;
		}
		break;

// 2020.05.07 対象カメラのみ設定 ---------->>>>
//---------------------------------------------------------
////////////////////////////////////
// 検査開始設定(カメラ選択設定)
////////////////////////////////////
	case MODE_START_SELECT_INIT:
		if (1 == m_nSubMode &&
			0 == m_nApxMode &&
			0 == m_nAocMode &&
			0 == m_nAocSubMode &&
			0 == m_nRegSizeMode &&
			0 == m_nCamMode)
		{
			LOG(em_MSG), "[%s] <%d> 検査開始設定(カメラ選択設定) 開始 -------------------------------------------------->>>", my_sThreadName, m_nSwMode);
			ChangeWaitEventNum(true);
		}

		if (1 == m_nSubMode)
		{
			//// レジスタサイズ繰り返しモード設定
			if (0 == m_nRegSizeMode)
			{
				if(TRUE == m_Param.linksw[m_nSwMode].bChSelCam[m_nApxMode]) {
					LOG(em_MSG), "[%s] <%d> ボード%.2d 途中参加レジスタ [%d]", my_sThreadName, m_nSwMode, m_nApxMode + 1, 0);
				}
				else
				{
					LOG(em_MSG), "[%s] <%d> ボード%.2d 途中参加レジスタ [-]", my_sThreadName, m_nSwMode, m_nApxMode + 1);
				}
				m_nRegSizeMode = 8;
			}

			if (8 == m_nRegSizeMode)
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (7:0)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x018 + (0x010 * m_nApxMode);
				if (TRUE == m_Param.linksw[m_nSwMode].bChSelCam[m_nApxMode]) {
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (7:0) Write
				}
				else
				{
					// 途中参加しないボードはReadしておく！
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (15:8) Read
				}
			}
			else if (7 == m_nRegSizeMode)
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (15:8)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x019 + (0x010 * m_nApxMode);
				if (TRUE == m_Param.linksw[m_nSwMode].bChSelCam[m_nApxMode]) {
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (7:0) Write
				}
				else
				{
					// 途中参加しないボードはReadしておく！
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (15:8) Read
				}
			}
			else if (6 == m_nRegSizeMode)
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (23:16)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01A + (0x010 * m_nApxMode);
				if (TRUE == m_Param.linksw[m_nSwMode].bChSelCam[m_nApxMode]) {
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (7:0) Write
				}
				else
				{
					// 途中参加しないボードはReadしておく！
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (15:8) Read
				}
			}
			else if (5 == m_nRegSizeMode)
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (31:24)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01B + (0x010 * m_nApxMode);
				if (TRUE == m_Param.linksw[m_nSwMode].bChSelCam[m_nApxMode]) {
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (7:0) Write
				}
				else
				{
					// 途中参加しないボードはReadしておく！
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (15:8) Read
				}
			}
			else if (4 == m_nRegSizeMode)
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (39:32)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01C + (0x010 * m_nApxMode);
				if (TRUE == m_Param.linksw[m_nSwMode].bChSelCam[m_nApxMode]) {
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (7:0) Write
				}
				else
				{
					// 途中参加しないボードはReadしておく！
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (15:8) Read
				}
			}
			else if (3 == m_nRegSizeMode)
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (47:40)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01D + (0x010 * m_nApxMode);
				if (TRUE == m_Param.linksw[m_nSwMode].bChSelCam[m_nApxMode]) {
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (7:0) Write
				}
				else
				{
					// 途中参加しないボードはReadしておく！
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (15:8) Read
				}
			}
			else if (2 == m_nRegSizeMode)
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (55:48)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01E + (0x010 * m_nApxMode);
				if (TRUE == m_Param.linksw[m_nSwMode].bChSelCam[m_nApxMode]) {
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (7:0) Write
				}
				else
				{
					// 途中参加しないボードはReadしておく！
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (15:8) Read
				}
			}
			else if (1 == m_nRegSizeMode)
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (63:56)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01F + (0x010 * m_nApxMode);
				if (TRUE == m_Param.linksw[m_nSwMode].bChSelCam[m_nApxMode]) {
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (7:0) Write
				}
				else
				{
					// 途中参加しないボードはReadしておく！
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (15:8) Read
				}

																														// ボード毎のモード設定
				if (m_nApxMode == MAX_CAMLINKSW_OUTPUT - 1)		m_nApxMode = 0;				// 最後のボード時
				else											m_nApxMode++;
			}
		}
		else if (2 == m_nSubMode)
		{
			int nAddr = 0x011 + (0x010 * m_nApxMode);

			if (TRUE == m_Param.linksw[m_nSwMode].bChSelCam[m_nApxMode]) {
				LOG(em_INF), "[%s] <%d> ボード%.2d カメラ選択レジスタ [%d(0:出力停止 1～8:入力CH)]", my_sThreadName, m_nSwMode, m_nApxMode + 1, m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]);	// ボード カメラ選択レジスタ
			}
			else
			{
				// 途中参加しないボードはReadしておく！
				LOG(em_INF), "[%s] <%d> ボード%.2d カメラ選択レジスタ [%d(0:出力停止 1～8:入力CH)] Read", my_sThreadName, m_nSwMode, m_nApxMode + 1, m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (15:8) Read
			}


																															// ボード毎のモード設定
			if (m_nApxMode == MAX_CAMLINKSW_OUTPUT - 1)		m_nApxMode = 0;					// 最後のボード時
			else											m_nApxMode++;
		}
		else if (3 == m_nSubMode)
		{
			LOG(em_MSG), "[%s] <%d> 検査開始設定(カメラ選択設定) 終了 --------------------------------------------------<<<", my_sThreadName, m_nSwMode);

			// 検査開始設定 → 設定待ち
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nApxMode = 0;
			m_nAocMode = 0;
			m_nAocSubMode = 0;
			m_nRegSizeMode = 0;
			m_nSwMode = 0;
			ChangeWaitEventNum(false);													// 1シーケンス正常完了

			// 統括管理 へ カメラリンクスイッチ設定完了通知
			Send_ToDefect_SelectInit();
		}
		else
		{
			LOG(em_ERR), "[%s] <%d> サブモード異常 [mode = 検査開始設定, sub_mode = %d]", my_sThreadName, m_nSwMode, m_nSubMode);
			syslog(210, "[<%d> mode = 検査開始設定 sub_mode = %d]", m_nSwMode, m_nSubMode);
			ChangeWaitEventNum(false);
			return false;
		}
		break;


// 2020.05.07 対象カメラのみ設定 <<<<----------

//---------------------------------------------------------
	////////////////////////////////////
	// 途中参加設定
	////////////////////////////////////
	case MODE_SLAVE_JOIN:
		if( 1 == m_nSubMode &&
			0 == m_nApxMode &&
			0 == m_nAocMode &&
			0 == m_nAocSubMode &&
			0 == m_nRegSizeMode &&
			0 == m_nCamMode )
		{
			LOG(em_MSG), "[%s] <%d> 途中参加設定 開始 -------------------------------------------------->>>", my_sThreadName, m_nSwMode);
			ChangeWaitEventNum( true );
		}

		if( 1 == m_nSubMode )
		{
			//// レジスタサイズ繰り返しモード設定
			if( 0 == m_nRegSizeMode )
			{
				if( (0 != m_nSelectCam[m_nSwMode][m_nApxMode] ||
					 0 != m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]) &&		// カメラ入力CH設定有り かつ
					m_nSlaveJoinTorB == m_nSelectTorB[m_nSwMode][m_nApxMode] )		// 途中参加表裏区分一致
				{
					LOG(em_MSG), "[%s] <%d> ボード%.2d 途中参加レジスタ [%I64d]", my_sThreadName, m_nSwMode, m_nApxMode+1, (__int64)(((__int64)m_nSlaveJoinFNo - (__int64)m_nResetFNo) * SIZE_FRAME_IMG_Y));
				}
				else
				{
					LOG(em_MSG), "[%s] <%d> ボード%.2d 途中参加レジスタ [-]", my_sThreadName, m_nSwMode, m_nApxMode+1);
				}
				m_nRegSizeMode = 8;
			}

			if( 8 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (7:0)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x018 + (0x010 * m_nApxMode);
				if( (0 != m_nSelectCam[m_nSwMode][m_nApxMode] ||
					 0 != m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]) &&		// カメラ入力CH設定有り かつ
					m_nSlaveJoinTorB == m_nSelectTorB[m_nSwMode][m_nApxMode] )		// 途中参加表裏区分一致
				{
					__int64 nVal = (((__int64)m_nSlaveJoinFNo - (__int64)m_nResetFNo) * SIZE_FRAME_IMG_Y);
					nVal >>= (64 - 8 * 8);
					nVal &= 0xFF;
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, nVal);									// ボード 途中参加レジスタ (7:0) Write
				}
				else
				{
					// 途中参加しないボードはReadしておく！
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (7:0) Read
				}
			}
			else if( 7 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (15:8)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x019 + (0x010 * m_nApxMode);
				if( (0 != m_nSelectCam[m_nSwMode][m_nApxMode] ||
					 0 != m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]) &&		// カメラ入力CH設定有り かつ
					m_nSlaveJoinTorB == m_nSelectTorB[m_nSwMode][m_nApxMode] )		// 途中参加表裏区分一致
				{
					__int64 nVal = (((__int64)m_nSlaveJoinFNo - (__int64)m_nResetFNo) * SIZE_FRAME_IMG_Y);
					nVal >>= (64 - 8 * 7);
					nVal &= 0xFF;
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, nVal);									// ボード 途中参加レジスタ (15:8) Write
				}
				else
				{
					// 途中参加しないボードはReadしておく！
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (15:8) Read
				}
			}
			else if( 6 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (23:16)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01A + (0x010 * m_nApxMode);
				if( (0 != m_nSelectCam[m_nSwMode][m_nApxMode] ||
					 0 != m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]) &&		// カメラ入力CH設定有り かつ
					m_nSlaveJoinTorB == m_nSelectTorB[m_nSwMode][m_nApxMode] )		// 途中参加表裏区分一致
				{
					__int64 nVal = (((__int64)m_nSlaveJoinFNo - (__int64)m_nResetFNo) * SIZE_FRAME_IMG_Y);
					nVal >>= (64 - 8 * 6);
					nVal &= 0xFF;
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, nVal);									// ボード 途中参加レジスタ (23:16) Write
				}
				else
				{
					// 途中参加しないボードはReadしておく！
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (23:16) Read
				}
			}
			else if( 5 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (31:24)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01B + (0x010 * m_nApxMode);
				if( (0 != m_nSelectCam[m_nSwMode][m_nApxMode] ||
					 0 != m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]) &&		// カメラ入力CH設定有り かつ
					m_nSlaveJoinTorB == m_nSelectTorB[m_nSwMode][m_nApxMode] )		// 途中参加表裏区分一致
				{
					__int64 nVal = (((__int64)m_nSlaveJoinFNo - (__int64)m_nResetFNo) * SIZE_FRAME_IMG_Y);
					nVal >>= (64 - 8 * 5);
					nVal &= 0xFF;
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, nVal);									// ボード 途中参加レジスタ (31:24) Write
				}
				else
				{
					// 途中参加しないボードはReadしておく！
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (31:24) Read
				}
			}
			else if( 4 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (39:32)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01C + (0x010 * m_nApxMode);
				if( (0 != m_nSelectCam[m_nSwMode][m_nApxMode] ||
					 0 != m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]) &&		// カメラ入力CH設定有り かつ
					m_nSlaveJoinTorB == m_nSelectTorB[m_nSwMode][m_nApxMode] )		// 途中参加表裏区分一致
				{
					__int64 nVal = (((__int64)m_nSlaveJoinFNo - (__int64)m_nResetFNo) * SIZE_FRAME_IMG_Y);
					nVal >>= (64 - 8 * 4);
					nVal &= 0xFF;
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, nVal);									// ボード 途中参加レジスタ (39:32) Write
				}
				else
				{
					// 途中参加しないボードはReadしておく！
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (39:32) Read
				}
			}
			else if( 3 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (47:40)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01D + (0x010 * m_nApxMode);
				if( (0 != m_nSelectCam[m_nSwMode][m_nApxMode] ||
					 0 != m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]) &&		// カメラ入力CH設定有り かつ
					m_nSlaveJoinTorB == m_nSelectTorB[m_nSwMode][m_nApxMode] )		// 途中参加表裏区分一致
				{
					__int64 nVal = (((__int64)m_nSlaveJoinFNo - (__int64)m_nResetFNo) * SIZE_FRAME_IMG_Y);
					nVal >>= (64 - 8 * 3);
					nVal &= 0xFF;
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, nVal);									// ボード 途中参加レジスタ (47:40) Write
				}
				else
				{
					// 途中参加しないボードはReadしておく！
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (47:40) Read
				}
			}
			else if( 2 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (55:48)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01E + (0x010 * m_nApxMode);
				if( (0 != m_nSelectCam[m_nSwMode][m_nApxMode] ||
					 0 != m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]) &&		// カメラ入力CH設定有り かつ
					m_nSlaveJoinTorB == m_nSelectTorB[m_nSwMode][m_nApxMode] )		// 途中参加表裏区分一致
				{
					__int64 nVal = (((__int64)m_nSlaveJoinFNo - (__int64)m_nResetFNo) * SIZE_FRAME_IMG_Y);
					nVal >>= (64 - 8 * 2);
					nVal &= 0xFF;
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, nVal);									// ボード 途中参加レジスタ (55:48) Write
				}
				else
				{
					// 途中参加しないボードはReadしておく！
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (55:48) Read
				}
			}
			else if( 1 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (63:56)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01F + (0x010 * m_nApxMode);
				if( (0 != m_nSelectCam[m_nSwMode][m_nApxMode] ||
					 0 != m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]) &&		// カメラ入力CH設定有り かつ
					m_nSlaveJoinTorB == m_nSelectTorB[m_nSwMode][m_nApxMode] )		// 途中参加表裏区分一致
				{
					__int64 nVal = (((__int64)m_nSlaveJoinFNo - (__int64)m_nResetFNo) * SIZE_FRAME_IMG_Y);
					nVal >>= (64 - 8 * 1);
					nVal &= 0xFF;
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, nVal);									// ボード 途中参加レジスタ (63:56) Write
				}
				else
				{
					// 途中参加しないボードはReadしておく！
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (63:56) Read
				}

				// ボード毎のモード設定
				if( m_nApxMode == MAX_CAMLINKSW_OUTPUT-1 )		m_nApxMode = 0;				// 最後のボード時
				else											m_nApxMode++;
			}
		}
		else if( 2 == m_nSubMode )
		{
			LOG(em_MSG), "[%s] <%d> 途中参加設定 終了 --------------------------------------------------<<<" , my_sThreadName, m_nSwMode);

			// 初期化 → 設定待ち
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nApxMode = 0;
			m_nAocMode = 0;
			m_nAocSubMode = 0;
			m_nRegSizeMode = 0;
			m_nSwMode = 0;
			ChangeWaitEventNum( false );													// 1シーケンス正常完了

			// 統括管理 へ カメラリンクスイッチ設定完了通知
			Send_ToDefect_SlaveJoin();
		}
		else
		{
			LOG(em_ERR), "[%s] <%d> サブモード異常 [mode = 途中参加設定, sub_mode = %d]", my_sThreadName, m_nSwMode, m_nSubMode);
			syslog(210, "[<%d> mode = 途中参加設定 sub_mode = %d]", m_nSwMode, m_nSubMode);
			ChangeWaitEventNum( false );
			return false;
		}
		break;

//---------------------------------------------------------
	////////////////////////////////////
	// カウンタリセット
	////////////////////////////////////
	case MODE_COUNTER_RESET:
		if( 1 == m_nSubMode &&
			0 == m_nApxMode &&
			0 == m_nAocMode &&
			0 == m_nAocSubMode &&
			0 == m_nRegSizeMode &&
			0 == m_nCamMode )
		{
			LOG(em_MSG), "[%s] <%d> カウンタリセット 開始 -------------------------------------------------->>>", my_sThreadName, m_nSwMode);
			ChangeWaitEventNum( true );
		}

#if 0	// 途中参加カウンタマニュアルリセットの動作が不明のためヤメ
		if( 1 == m_nSubMode )
		{
			//// レジスタサイズ繰り返しモード設定
			if( 0 == m_nRegSizeMode )
			{
				LOG(em_MSG), "[%s] <%d> ボード%.2d 途中参加レジスタ [%d]", my_sThreadName, m_nSwMode, m_nApxMode+1, 0);
				m_nRegSizeMode = 8;
			}

			if( 8 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (7:0)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x018 + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (7:0) Write
			}
			else if( 7 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (15:8)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x019 + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (15:8) Write
			}
			else if( 6 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (23:16)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01A + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (23:16) Write
			}
			else if( 5 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (31:24)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01B + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (31:24) Write
			}
			else if( 4 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (39:32)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01C + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (39:32) Write
			}
			else if( 3 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (47:40)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01D + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (47:40) Write
			}
			else if( 2 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (55:48)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01E + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (55:48) Write
			}
			else if( 1 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (63:56)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01F + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (63:56) Write

				// ボード毎のモード設定
				if( m_nApxMode == MAX_CAMLINKSW_OUTPUT-1 )		m_nApxMode = 0;				// 最後のボード時
				else											m_nApxMode++;
			}
		}
		else if( 2 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> ボード%.2d モードレジスタ (途中参加カウンタマニュアルリセット) [%d]", my_sThreadName, m_nSwMode, m_nApxMode+1, 64 + 16 + 8 * COUNTER_MODE);

			int nAddr = 0x010 + (0x010 * m_nApxMode);
			sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 64 + 16 + 8 * COUNTER_MODE);						// ボード モードレジスタ (途中参加カウンタマニュアルリセット)

			// ボード毎のモード設定
			if( m_nApxMode == MAX_CAMLINKSW_OUTPUT-1 )		m_nApxMode = 0;					// 最後のボード時
			else											m_nApxMode++;
		}
		else if( 3 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> ボード%.2d モードレジスタ (途中参加カウンタマニュアルリセット解除) [%d]", my_sThreadName, m_nSwMode, m_nApxMode+1, 64 + 0 + 8 * COUNTER_MODE);

			int nAddr = 0x010 + (0x010 * m_nApxMode);
			sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 64 + 0 + 8 * COUNTER_MODE);						// ボード モードレジスタ (途中参加カウンタマニュアルリセット解除)

			// ボード毎のモード設定
			if( m_nApxMode == MAX_CAMLINKSW_OUTPUT-1 )		m_nApxMode = 0;					// 最後のボード時
			else											m_nApxMode++;
		}
		else if( 4 == m_nSubMode )
#else
		if( 1 == m_nSubMode )
		{
			//// レジスタサイズ繰り返しモード設定
			if( 0 == m_nRegSizeMode )
			{
				LOG(em_MSG), "[%s] <%d> ボード%.2d 途中参加レジスタ [%d]", my_sThreadName, m_nSwMode, m_nApxMode+1, 0);
				m_nRegSizeMode = 8;
			}

			if( 8 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (7:0)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x018 + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (7:0) Write
			}
			else if( 7 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (15:8)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x019 + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (15:8) Write
			}
			else if( 6 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (23:16)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01A + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (23:16) Write
			}
			else if( 5 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (31:24)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01B + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (31:24) Write
			}
			else if( 4 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (39:32)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01C + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (39:32) Write
			}
			else if( 3 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (47:40)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01D + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (47:40) Write
			}
			else if( 2 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (55:48)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01E + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (55:48) Write
			}
			else if( 1 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (63:56)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01F + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// ボード 途中参加レジスタ (63:56) Write

				// ボード毎のモード設定
				if( m_nApxMode == MAX_CAMLINKSW_OUTPUT-1 )		m_nApxMode = 0;				// 最後のボード時
				else											m_nApxMode++;
			}
		}
		else if( 2 == m_nSubMode )
#endif
		{
			LOG(em_MSG), "[%s] <%d> カウンタリセット 終了 --------------------------------------------------<<<" , my_sThreadName, m_nSwMode);

			// 初期化 → 設定待ち
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nApxMode = 0;
			m_nAocMode = 0;
			m_nAocSubMode = 0;
			m_nRegSizeMode = 0;
			m_nSwMode = 0;
			ChangeWaitEventNum( false );													// 1シーケンス正常完了
		}
		else
		{
			LOG(em_ERR), "[%s] <%d> サブモード異常 [mode = カウンタリセット, sub_mode = %d]", my_sThreadName, m_nSwMode, m_nSubMode);
			syslog(210, "[<%d> mode = カウンタリセット sub_mode = %d]", m_nSwMode, m_nSubMode);
			ChangeWaitEventNum( false );
			return false;
		}
		break;

//---------------------------------------------------------
	////////////////////////////////////
	// 定周期チェック
	////////////////////////////////////
	case MODE_CYCLE_CHECK:
		if( 1 == m_nSubMode &&
			0 == m_nApxMode &&
			0 == m_nAocMode &&
			0 == m_nAocSubMode &&
			0 == m_nRegSizeMode &&
			0 == m_nCamMode )
		{
			LOG(em_MSG), "[%s] <%d> 定周期チェック 開始 -------------------------------------------------->>>", my_sThreadName, m_nSwMode);
			ChangeWaitEventNum( true );
		}

		if( 1 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> 周囲温度レジスタ", my_sThreadName, m_nSwMode);

			sprintf(m_cSendBuf, "@%02XR003\r", m_nSwMode);																// 周囲温度レジスタ
		}
		else if( 2 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> FPGAダイ温度レジスタ", my_sThreadName, m_nSwMode);

			sprintf(m_cSendBuf, "@%02XR004\r", m_nSwMode);																// FPGAダイ温度レジスタ
		}
		else if( 3 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> カメラ%.2d Opt-C:Link B   Ctrl/Status レジスタ", my_sThreadName, m_nSwMode, m_nCamMode+1);

			int nAddr = 0x100 + (0x010 * m_nCamMode);
			sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);														// Opt-C:Link B   Ctrl/Status レジスタ

			// カメラ毎のモード設定
			if( m_nCamMode == MAX_CAMLINKSW_ALLINPUT-1 )	m_nCamMode = 0;					// 最後のカメラ時
			else											m_nCamMode++;
		}
		else if( 4 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> カメラ%.2d Opt-C:Link M/F Ctrl/Status レジスタ", my_sThreadName, m_nSwMode, m_nCamMode+1);

			int nAddr = 0x101 + (0x010 * m_nCamMode);
			sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);														// Opt-C:Link M/F Ctrl/Status レジスタ

			// カメラ毎のモード設定
			if( m_nCamMode == MAX_CAMLINKSW_ALLINPUT-1 )	m_nCamMode = 0;					// 最後のカメラ時
			else											m_nCamMode++;
		}
		else if( 5 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> カメラ%.2d Opt-C:Link B   Ctrl/Status レジスタ CRCエラーリセット", my_sThreadName, m_nSwMode, m_nCamMode+1);

			int nAddr = 0x100 + (0x010 * m_nCamMode);
			sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 2);

			// カメラ毎のモード設定
			if( m_nCamMode == MAX_CAMLINKSW_ALLINPUT-1 )	m_nCamMode = 0;					// 最後のカメラ時
			else											m_nCamMode++;
		}
		else if( 6 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> カメラ%.2d Opt-C:Link M/F Ctrl/Status レジスタ CRCエラーリセット", my_sThreadName, m_nSwMode, m_nCamMode+1);

			int nAddr = 0x101 + (0x010 * m_nCamMode);
			sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 2);

			// カメラ毎のモード設定
			if( m_nCamMode == MAX_CAMLINKSW_ALLINPUT-1 )	m_nCamMode = 0;					// 最後のカメラ時
			else											m_nCamMode++;
		}
		else if( 7 == m_nSubMode )
		{
			for( int ii=0; ii<NUM_CAMLINKSW; ii++ )
			{
				//// マスター側 (IDが偶数)
				for( int jj=0; jj<MAX_CAMLINKSW_INPUT; jj++ )
				{
					//// B
					LOG(em_MSG), "[%s] <%d> カメラ%.2d [Opt-C:Link B   Status=%d]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA, jj+1, m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0]);
					// Opt-C;Linkステータスチェック
					if( 0 != m_nOptCLinkStID[ii*MAX_CAMLINKSW_FPGA][jj][0] )
					{
						if( (    0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] & 8)			// RxStatusエラー
							  || 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] & 4)			// LinkStatusエラー
							  || 0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] & 2)			// CRCエラー
							) && m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] )
						{
							// 正常 → 異常
							KizuFunction::SetStatus(m_nOptCLinkStID[ii*MAX_CAMLINKSW_FPGA][jj][0], false);
							m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] = false;
						}
						else if( (    0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] & 8)	// RxStatusエラー
								   && 0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] & 4)	// LinkStatusエラー
								   && 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] & 2)	// CRCエラー
								 ) && ! m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] )
						{
							// 異常 → 正常
							KizuFunction::SetStatus(m_nOptCLinkStID[ii*MAX_CAMLINKSW_FPGA][jj][0], true);
							m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] = true;
						}

						// RxStatusエラー (受信側光モジュールの受光状態)
						if( 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] & 8) &&
							m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA][jj][0] )
						{
							LOG(em_ERR), "[%s] <%d> カメラ%.2d [Opt-C:Link B   RxStatusエラー]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA, jj+1);
							syslog(213, "[<%d> Ch%d B]", ii*MAX_CAMLINKSW_FPGA, jj+1);
							m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA][jj][0] = FALSE;
						}
						else if( ! m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA][jj][0] )
						{
							m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA][jj][0] = TRUE;
						}

						// LinkStatusエラー (リンク状態)
						if( 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] & 4) &&
							m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] )
						{
							LOG(em_ERR), "[%s] <%d> カメラ%.2d [Opt-C:Link B   LinkStatusエラー]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA, jj+1);
							syslog(214, "[<%d> Ch%d B]", ii*MAX_CAMLINKSW_FPGA, jj+1);
							m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] = FALSE;
						}
						else if( ! m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] )
						{
							m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] = TRUE;
						}

						// CRCエラー
						if( 0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] & 2) )
						{
							LOG(em_WAR), "[%s] <%d> カメラ%.2d [Opt-C:Link B   CRCエラー]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA, jj+1);
							syslog(215, "[<%d> Ch%d B]", ii*MAX_CAMLINKSW_FPGA, jj+1);
						}
					}

					//// M/F
					LOG(em_MSG), "[%s] <%d> カメラ%.2d [Opt-C:Link M/F Status=%d]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA, jj+1, m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1]);
					// Opt-C;Linkステータスチェック
					if( 0 != m_nOptCLinkStID[ii*MAX_CAMLINKSW_FPGA][jj][1] )
					{
						if( (    0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] & 8)			// RxStatusエラー
							  || 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] & 4)			// LinkStatusエラー
							  || 0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] & 2)			// CRCエラー
							) && m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] )
						{
							// 正常 → 異常
							KizuFunction::SetStatus(m_nOptCLinkStID[ii*MAX_CAMLINKSW_FPGA][jj][1], false);
							m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] = false;
						}
						else if( (    0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] & 8)	// RxStatusエラー
								   && 0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] & 4)	// LinkStatusエラー
								   && 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] & 2)	// CRCエラー
								 ) && ! m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] )
						{
							// 異常 → 正常
							KizuFunction::SetStatus(m_nOptCLinkStID[ii*MAX_CAMLINKSW_FPGA][jj][1], true);
							m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] = true;
						}

						// RxStatusエラー (受信側光モジュールの受光状態)
						if( 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] & 8) &&
							m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA][jj][1] )
						{
							LOG(em_ERR), "[%s] <%d> カメラ%.2d [Opt-C:Link M/F RxStatusエラー]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA, jj+1);
							syslog(213, "[<%d> Ch%d M/F]", ii*MAX_CAMLINKSW_FPGA, jj+1);
							m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA][jj][1] = FALSE;
						}
						else if( ! m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA][jj][1] )
						{
							m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA][jj][1] = TRUE;
						}

						// LinkStatusエラー (リンク状態)
						if( 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] & 4) &&
							m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] )
						{
							LOG(em_ERR), "[%s] <%d> カメラ%.2d [Opt-C:Link M/F LinkStatusエラー]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA, jj+1);
							syslog(214, "[<%d> Ch%d M/F]", ii*MAX_CAMLINKSW_FPGA, jj+1);
							m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] = FALSE;
						}
						else if( ! m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] )
						{
							m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] = TRUE;
						}

						// CRCエラー
						if( 0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] & 2) )
						{
							LOG(em_WAR), "[%s] <%d> カメラ%.2d [Opt-C:Link M/F CRCエラー]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA, jj+1);
							syslog(215, "[<%d> Ch%d M/F]", ii*MAX_CAMLINKSW_FPGA, jj+1);
						}
					}
				}

				//// スレーブ側 (IDが奇数)
				for( int jj=MAX_CAMLINKSW_INPUT; jj<MAX_CAMLINKSW_ALLINPUT; jj++ )
				{
					//// B
					LOG(em_MSG), "[%s] <%d> カメラ%.2d [Opt-C:Link B   Status=%d]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA+1, jj+1, m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0]);
					// Opt-C;Linkステータスチェック
					if( 0 != m_nOptCLinkStID[ii*MAX_CAMLINKSW_FPGA+1][jj-MAX_CAMLINKSW_INPUT][0] )
					{
						if( (    0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] & 8)		// RxStatusエラー
							  || 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] & 4)		// LinkStatusエラー
							  || 0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] & 2)		// CRCエラー
							) && m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj-MAX_CAMLINKSW_INPUT][0] )
						{
							// 正常 → 異常
							KizuFunction::SetStatus(m_nOptCLinkStID[ii*MAX_CAMLINKSW_FPGA+1][jj-MAX_CAMLINKSW_INPUT][0], false);
							m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj-MAX_CAMLINKSW_INPUT][0] = false;
						}
						else if( (    0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] & 8)	// RxStatusエラー
								   && 0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] & 4)	// LinkStatusエラー
								   && 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] & 2)	// CRCエラー
								 ) && ! m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj-MAX_CAMLINKSW_INPUT][0] )
						{
							// 異常 → 正常
							KizuFunction::SetStatus(m_nOptCLinkStID[ii*MAX_CAMLINKSW_FPGA+1][jj-MAX_CAMLINKSW_INPUT][0], true);
							m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj-MAX_CAMLINKSW_INPUT][0] = true;
						}

						// RxStatusエラー (受信側光モジュールの受光状態)
						if( 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] & 8) &&
							m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] )
						{
							LOG(em_ERR), "[%s] <%d> カメラ%.2d [Opt-C:Link B   RxStatusエラー]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA+1, jj+1);
							syslog(213, "[<%d> Ch%d B]", ii*MAX_CAMLINKSW_FPGA+1, jj+1);
							m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] = FALSE;
						}
						else if( ! m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] )
						{
							m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] = TRUE;
						}

						// LinkStatusエラー (リンク状態)
						if( 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] & 4) &&
							m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] )
						{
							LOG(em_ERR), "[%s] <%d> カメラ%.2d [Opt-C:Link B   LinkStatusエラー]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA+1, jj+1);
							syslog(214, "[<%d> Ch%d B]", ii*MAX_CAMLINKSW_FPGA+1, jj+1);
							m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] = FALSE;
						}
						else if( ! m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] )
						{
							m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] = TRUE;
						}

						// CRCエラー
						if( 0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] & 2) )
						{
							LOG(em_WAR), "[%s] <%d> カメラ%.2d [Opt-C:Link B   CRCエラー]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA+1, jj+1);
							syslog(215, "[<%d> Ch%d B]", ii*MAX_CAMLINKSW_FPGA+1, jj+1);
						}
					}

					//// M/F
					LOG(em_MSG), "[%s] <%d> カメラ%.2d [Opt-C:Link M/F Status=%d]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA+1, jj+1, m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1]);
					// Opt-C;Linkステータスチェック
					if( 0 != m_nOptCLinkStID[ii*MAX_CAMLINKSW_FPGA+1][jj-MAX_CAMLINKSW_INPUT][1] )
					{
						if( (    0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] & 8)		// RxStatusエラー
							  || 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] & 4)		// LinkStatusエラー
							  || 0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] & 2)		// CRCエラー
							) && m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj-MAX_CAMLINKSW_INPUT][1] )
						{
							// 正常 → 異常
							KizuFunction::SetStatus(m_nOptCLinkStID[ii*MAX_CAMLINKSW_FPGA+1][jj-MAX_CAMLINKSW_INPUT][1], false);
							m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj-MAX_CAMLINKSW_INPUT][1] = false;
						}
						else if( (    0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] & 8)	// RxStatusエラー
								   && 0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] & 4)	// LinkStatusエラー
								   && 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] & 2)	// CRCエラー
								 ) && ! m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj-MAX_CAMLINKSW_INPUT][1] )
						{
							// 異常 → 正常
							KizuFunction::SetStatus(m_nOptCLinkStID[ii*MAX_CAMLINKSW_FPGA+1][jj-MAX_CAMLINKSW_INPUT][1], true);
							m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj-MAX_CAMLINKSW_INPUT][1] = true;
						}

						// RxStatusエラー (受信側光モジュールの受光状態)
						if( 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] & 8) &&
							m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] )
						{
							LOG(em_ERR), "[%s] <%d> カメラ%.2d [Opt-C:Link M/F RxStatusエラー]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA+1, jj+1);
							syslog(213, "[<%d> Ch%d M/F]", ii*MAX_CAMLINKSW_FPGA+1, jj+1);
							m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] = FALSE;
						}
						else if( ! m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] )
						{
							m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] = TRUE;
						}

						// LinkStatusエラー (リンク状態)
						if( 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] & 4) &&
							m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] )
						{
							LOG(em_ERR), "[%s] <%d> カメラ%.2d [Opt-C:Link M/F LinkStatusエラー]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA+1, jj+1);
							syslog(214, "[<%d> Ch%d M/F]", ii*MAX_CAMLINKSW_FPGA+1, jj+1);
							m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] = FALSE;
						}
						else if( ! m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] )
						{
							m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] = TRUE;
						}

						// CRCエラー
						if( 0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] & 2) )
						{
							LOG(em_WAR), "[%s] <%d> カメラ%.2d [Opt-C:Link M/F CRCエラー]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA+1, jj+1);
							syslog(215, "[<%d> Ch%d M/F]", ii*MAX_CAMLINKSW_FPGA+1, jj+1);
						}
					}
				}
			}

			LOG(em_MSG), "[%s] <%d> 定周期チェック 終了 --------------------------------------------------<<<" , my_sThreadName, m_nSwMode);

			// 定周期チェック → 設定待ち
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nApxMode = 0;
			m_nAocMode = 0;
			m_nAocSubMode = 0;
			m_nRegSizeMode = 0;
			m_nSwMode = 0;
			ChangeWaitEventNum( false );													// 1シーケンス正常完了

			// 定周期チェックタイマ 再開
			LARGE_INTEGER ts;
			ts.QuadPart = (__int64)m_nCycleTime * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
			SetWaitableTimer(m_evTimerCycleCheck, &ts, 0, NULL, NULL, FALSE);
		}
		else
		{
			LOG(em_ERR), "[%s] <%d> サブモード異常 [mode = 定周期チェック, sub_mode = %d]", my_sThreadName, m_nSwMode, m_nSubMode);
			syslog(210, "[<%d> mode = 定周期チェック sub_mode = %d]", m_nSwMode, m_nSubMode);
			ChangeWaitEventNum( false );
			return false;
		}
		break;

//---------------------------------------------------------
	////////////////////////////////////
	// カウンタチェック
	////////////////////////////////////
	case MODE_COUNTER_CHECK:
		if( 1 == m_nSubMode &&
			0 == m_nApxMode &&
			0 == m_nAocMode &&
			0 == m_nAocSubMode &&
			0 == m_nRegSizeMode &&
			0 == m_nCamMode )
		{
			LOG(em_MSG), "[%s] <%d> カウンタチェック 開始 -------------------------------------------------->>>", my_sThreadName, m_nSwMode);
			ChangeWaitEventNum( true );
		}

		if( 1 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> ボード%.2d モードレジスタ (途中参加カウンタリード選択)(現在値Read) [%d]", my_sThreadName, m_nSwMode, m_nApxMode+1, 0 + 8 * COUNTER_MODE);

			int nAddr = 0x010 + (0x010 * m_nApxMode);
			sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0 + 8 * COUNTER_MODE);							// ボード モードレジスタ (途中参加カウンタリード選択, 遅延カウント信号選択)

			// ボード毎のモード設定
			if( m_nApxMode == MAX_CAMLINKSW_OUTPUT-1 )		m_nApxMode = 0;					// 最後のボード時
			else											m_nApxMode++;
		}
		else if( 2 == m_nSubMode )
		{
			//// レジスタサイズ繰り返しモード設定
			if( 0 == m_nRegSizeMode )
			{
				LOG(em_MSG), "[%s] <%d> ボード%.2d 途中参加レジスタ [現在値Read]", my_sThreadName, m_nSwMode, m_nApxMode+1);
				m_nRegSizeMode = 8;
			}

			if( 8 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (7:0)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x018 + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (7:0) Read
			}
			else if( 7 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (15:8)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x019 + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (15:8) Read
			}
			else if( 6 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (23:16)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01A + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (23:16) Read
			}
			else if( 5 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (31:24)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01B + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (31:24) Read
			}
			else if( 4 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (39:32)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01C + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (39:32) Read
			}
			else if( 3 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (47:40)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01D + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (47:40) Read
			}
			else if( 2 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (55:48)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01E + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (55:48) Read
			}
			else if( 1 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (63:56)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01F + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (63:56) Read

				// ボード毎のモード設定
				if( m_nApxMode == MAX_CAMLINKSW_OUTPUT-1 )		m_nApxMode = 0;				// 最後のボード時
				else											m_nApxMode++;
			}
		}
		else if( 3 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> ボード%.2d モードレジスタ (途中参加カウンタリード選択)(設定値Read) [%d]", my_sThreadName, m_nSwMode, m_nApxMode+1, 64 + 8 * COUNTER_MODE);

			int nAddr = 0x010 + (0x010 * m_nApxMode);
			sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 64 + 8 * COUNTER_MODE);							// ボード モードレジスタ (途中参加カウンタリード選択, 遅延カウント信号選択)

			// ボード毎のモード設定
			if( m_nApxMode == MAX_CAMLINKSW_OUTPUT-1 )			m_nApxMode = 0;				// 最後のボード時
			else												m_nApxMode++;
		}
		else if( 4 == m_nSubMode )
		{
			//// レジスタサイズ繰り返しモード設定
			if( 0 == m_nRegSizeMode )
			{
				LOG(em_MSG), "[%s] <%d> ボード%.2d 途中参加レジスタ [設定値Read]", my_sThreadName, m_nSwMode, m_nApxMode+1);
				m_nRegSizeMode = 8;
			}

			if( 8 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (7:0)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x018 + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (7:0) Read
			}
			else if( 7 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (15:8)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x019 + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (15:8) Read
			}
			else if( 6 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (23:16)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01A + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (23:16) Read
			}
			else if( 5 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (31:24)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01B + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (31:24) Read
			}
			else if( 4 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (39:32)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01C + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (39:32) Read
			}
			else if( 3 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (47:40)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01D + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (47:40) Read
			}
			else if( 2 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (55:48)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01E + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (55:48) Read
			}
			else if( 1 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> ボード%.2d 途中参加レジスタ (63:56)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01F + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// ボード 途中参加レジスタ (63:56) Read

				// ボード毎のモード設定
				if( m_nApxMode == MAX_CAMLINKSW_OUTPUT-1 )		m_nApxMode = 0;				// 最後のボード時
				else											m_nApxMode++;
			}
		}
		else if( 5 == m_nSubMode )
		{
			for( int ii=0; ii<NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; ii++ )
			{
				for( int jj=0; jj<MAX_CAMLINKSW_OUTPUT; jj++ )
				{
					LOG(em_MSG), "[%s] <%d> ボード%.2d [現在値=%I64d 設定値=%I64d]", my_sThreadName, ii, jj+1, m_nCounterNow[ii][jj], m_nCounterSet[ii][jj]);
				}
			}

			LOG(em_MSG), "[%s] <%d> カウンタチェック 終了 --------------------------------------------------<<<" , my_sThreadName, m_nSwMode);

			// 初期化 → 設定待ち
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nApxMode = 0;
			m_nAocMode = 0;
			m_nAocSubMode = 0;
			m_nRegSizeMode = 0;
			m_nSwMode = 0;
			ChangeWaitEventNum( false );													// 1シーケンス正常完了
		}
		else
		{
			LOG(em_ERR), "[%s] <%d> サブモード異常 [mode = カウンタチェック, sub_mode = %d]", my_sThreadName, m_nSwMode, m_nSubMode);
			syslog(210, "[<%d> mode = カウンタチェック sub_mode = %d]", m_nSwMode, m_nSubMode);
			ChangeWaitEventNum( false );
			return false;
		}
		break;

//---------------------------------------------------------
	case MODE_CHANGE_AOC_SERIALSPEED:
			//// AOCサブモード変更
		//AOCレジスタを個別に編集するので、位置によっては 頭にあるsubmode++ が効かないことがあるので。
		if( (0 == m_nSubMode||1 == m_nSubMode  ) &&
			0 == m_nAocSubMode &&
			0 == m_nRegSizeMode &&
			0 == m_nCamMode )
		{
			//int nAOCNo = m_nAocMode * MAX_CAMLINKSW_ALLINPUT + m_nSwMode *MAX_CAMLINKSW_INPUT + m_nApxMode;
			int nAOCNo = m_nAocMode + m_nSwMode *MAX_CAMLINKSW_INPUT;
			LOG(em_MSG), "[%s] <%d> AOCシリアル速度変更 開始[%d.%d.%d <= %d] -------------------------------------------------->>>",
				my_sThreadName, m_nSwMode, 
				m_nApxMode,				m_nSwMode ,				m_nAocMode,
				m_Param.GetSerialSpeed(nAOCNo) );
			ChangeWaitEventNum( true );
			m_nSubMode = 1;
		}

		if(1 == m_nSubMode )
		{
			if( 0 == m_nRegSizeMode )	m_nAocSubMode++;

			if( 1 == m_nAocSubMode )
			{
				//// レジスタサイズ繰り返しモード設定
				if( 0 == m_nRegSizeMode )	m_nRegSizeMode = 2;

				if( 2 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Address レジスタ (7:0)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x180 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x7C);									// AOC Address モードレジスタ (7:0)
				}
				else if( 1 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Address レジスタ (15:8)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x181 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x00);									// AOC Address モードレジスタ (15:8)
				}
			}
			else if( 2 == m_nAocSubMode )
			{
				//int nAOCNo =  m_nAocMode * MAX_CAMLINKSW_ALLINPUT + m_nSwMode *MAX_CAMLINKSW_INPUT + m_nApxMode;
				int nAOCNo = m_nAocMode + m_nSwMode *MAX_CAMLINKSW_INPUT;
				//// レジスタサイズ繰り返しモード設定
				if( 0 == m_nRegSizeMode )	m_nRegSizeMode = 4;

				if( 4 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data レジスタ (7:0)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x182 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, LOBYTE(m_Param.nSamplingTiming(nAOCNo )));					// AOC Write Data レジスタ (7:0)
				}
				else if( 3 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data レジスタ (15:8)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x183 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, HIBYTE(m_Param.nSamplingTiming(nAOCNo)));					// AOC Write Data レジスタ (15:8)
				}
				else if( 2 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data レジスタ (23:16)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x184 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, LOBYTE(m_Param.nSamplingDelay(nAOCNo)));									// AOC Write Data レジスタ (23:16)
				}
				else if( 1 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data レジスタ (31:24)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x185 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, HIBYTE(m_Param.nSamplingDelay(nAOCNo)));									// AOC Write Data レジスタ (31:24)
				}
			}
			else if( 3 == m_nAocSubMode )
			{
				LOG(em_INF), "[%s] <%d> AOC%.2d Ctrl レジスタ (Write Access)", my_sThreadName, m_nSwMode, m_nAocMode+1);

				int nAddr = 0x18A + (0x010 * m_nAocMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x11);										// AOC Ctrl レジスタ (Write Access)
			}
			else if( 4 == m_nAocSubMode )
			{
				LOG(em_INF), "[%s] <%d> AOC%.2d Ctrl レジスタ (Read Access)", my_sThreadName, m_nSwMode, m_nAocMode+1);

				int nAddr = 0x18A + (0x010 * m_nAocMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);													// AOC Ctrl レジスタ (Read Access)
			}
			else if( 5 == m_nAocSubMode )
			{
				LOG(em_INF), "[%s] <%d> AOC%.2d Ctrl レジスタ (送信完了リセット)", my_sThreadName, m_nSwMode, m_nAocMode+1);

				int nAddr = 0x18A + (0x010 * m_nAocMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x04);										// AOC Ctrl レジスタ (送信完了リセット)
				
				m_nApxMode = 0;
				m_nAocMode = 0;
			}
		}

		else if( 2 == m_nSubMode )
			{
				LOG(em_MSG), "[%s] <%d> AOCシリアル速度変更  終了 --------------------------------------------------<<<" , my_sThreadName, m_nSwMode);

				// 初期化 → 設定待ち
				m_emMainMode = MODE_WAIT;
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nApxMode = 0;
				m_nAocMode = 0;
				m_nAocSubMode = 0;
				m_nRegSizeMode = 0;
				m_nSwMode = 0;
				ChangeWaitEventNum( false );													// 1シーケンス正常完了
		}
		else
		{
			LOG(em_ERR), "[%s] <%d> サブモード異常 [mode = AOCシリアル速度変更 , sub_mode = %d]", my_sThreadName, m_nSwMode, m_nSubMode);
			syslog(210, "[<%d> mode =  AOCシリアル速度変更 sub_mode = %d]", m_nSwMode, m_nSubMode);
			ChangeWaitEventNum( false );
			return false;
		}
		break;
	break;
//---------------------------------------------------------
	default:
		LOG(em_ERR), "[%s] モード異常 [MainMode = %d]", my_sThreadName, m_emMainMode);
		syslog(210, "[mode = %d]", m_emMainMode);
		ChangeWaitEventNum( false );
		return false;
	}

	//// 送信
	if( 0 == strlen(m_cSendBuf) ) return true;												// 送信データ無しの場合は、正常を返す。
	if( ! Send() ) return false;
	return true;
}

//------------------------------------------
// 受信伝文チェック(AOC MODE)
// 戻り値 復帰情報
//------------------------------------------
bool LinkswManager::CheckDataRecv_AOC()
{
	// AOC Ctrl レジスタ (Write Access)
	if( 3 == m_nAocSubMode )
	{
		//// 特に無し
	}
	// AOC Ctrl レジスタ (Read Access)
	else if( 4 == m_nAocSubMode )
	{
		char cWk[256];
		int nCtrl;
		memset(cWk, 0x00, sizeof(cWk));
		memcpy(cWk, &m_cRecvBuf[10], 2);
		sscanf(cWk, "%X", &nCtrl);
		//// リードデータ チェック
		if( 0x04 != (nCtrl & 0x06) )		// 1bit目が0、2bit目ば1となっているかチェック
		{
			// カメラ入力Ch
			if( m_Param.bCamInputCh[ m_nSwMode ][ m_nAocMode ] )
			{
				LOG(em_ERR), "[%s] <%d> AOC%.2d Ctrl レジスタ データ送信 失敗 (%d)", my_sThreadName, m_nSwMode, m_nAocMode+1, m_nAocSubMode);
// --->>>
// カメラ選択設定コマンドで、存在しないカメラ or 存在するが未接続のカメラを選択した場合、
// 初期化(MODE_START_UP)シーケンス、検査開始設定(MODE_START_INIT)シーケンスを無限ループしてしまうための対策
//						ChangeWaitEventNum( false );										// 1シーケンス強制終了
//						return false;
// <<<---
			}
			// カメラ未入力Ch
			else
			{
				LOG(em_WAR), "[%s] <%d> AOC%.2d Ctrl レジスタ データ送信 失敗 (カメラ未入力チャンネルのため無視) (%d)", my_sThreadName, m_nSwMode, m_nAocMode+1, m_nAocSubMode);
			}
		}
	}
	// AOC Ctrl レジスタ (送信完了リセット)
	else if( 5 == m_nAocSubMode )
	{
		char cWk[256];
		int nCtrl;
		memset(cWk, 0x00, sizeof(cWk));
		memcpy(cWk, &m_cRecvBuf[10], 2);
		sscanf(cWk, "%X", &nCtrl);
		//// ライトデータ チェック
		if( 0x00 != (nCtrl & 0x04) )		// 2bit目が0となっているかチェック
		{
			// カメラ入力Ch
			if( m_Param.bCamInputCh[ m_nSwMode ][ m_nAocMode ] )
			{
				LOG(em_ERR), "[%s] <%d> AOC%.2d Ctrl レジスタ 送信完了リセット 失敗 (%d)", my_sThreadName, m_nSwMode, m_nAocMode+1, m_nAocSubMode);
				ChangeWaitEventNum( false );										// 1シーケンス強制終了
				return false;
			}
			// カメラ未入力Ch
			else
			{
				LOG(em_WAR), "[%s] <%d> AOC%.2d Ctrl レジスタ 送信完了リセット 失敗 (カメラ未入力チャンネルのため無視) (%d)", my_sThreadName, m_nSwMode, m_nAocMode+1, m_nAocSubMode);
			}
		}

		//// AOCサブモードクリア
		m_nAocSubMode = 0;
	}
	return true;
}

//------------------------------------------
// 受信伝文チェック
// 戻り値 復帰情報
//------------------------------------------
bool LinkswManager::CheckDataRecv()
{
	CString sWk;
	//LOG(em_INF), "[%s] 応答伝文受信 [%s]", my_sThreadName, m_cRecvBuf);

	//// スイッチFPGAボードID チェック
	if( 0 != memcmp(&m_cRecvBuf[0], &m_cSendBuf[1], 2) )
	{
		LOG(em_ERR), "[%s] 受信伝文 異常 [ボードID相違:send=%s][%s]", my_sThreadName, m_cSendBuf, m_cRecvBuf);
		syslog(211, "[ボードID相違:send=%s]", m_cSendBuf);
		ChangeWaitEventNum( false );														// 1シーケンス強制終了
		return false;
	}

	//// 応答区分 チェック
	if( 0 != memcmp(&m_cRecvBuf[3], R_OK, 2) )
	{
		LOG(em_ERR), "[%s] 受信伝文 異常 [応答区分異常:send=%s]", my_sThreadName, m_cSendBuf);
		syslog(211, "[応答区分異常:send=%s]", m_cSendBuf);
		ChangeWaitEventNum( false );														// 1シーケンス強制終了
		return false;
	}

	//// アドレス チェック
	if( 0 != memcmp(&m_cRecvBuf[6], &m_cSendBuf[4], 3) )
	{
		LOG(em_ERR), "[%s] 受信伝文 異常 [アドレス相違:send=%s]", my_sThreadName, m_cSendBuf);
		syslog(211, "[アドレス相違:send=%s]", m_cSendBuf);
		ChangeWaitEventNum( false );														// 1シーケンス強制終了
		return false;
	}

	//// レジスタサイズ繰り返しモード変更
	if( 0 != m_nRegSizeMode )	m_nRegSizeMode--;

	//// 処理分け
	switch( m_emMainMode )
	{
//---------------------------------------------------------
	////////////////////////////////////
	// オープンチェック
	////////////////////////////////////
	case MODE_OPEN_CHECK:
		//// リードデータ チェック
		// スイッチFPGA ステータスレジスタ
		if( m_Param.linksw[ m_nSwMode ].bStatus
			? 0 != memcmp(&m_cRecvBuf[10], "01", 2)			// スイッチマスタ
			: 0 != memcmp(&m_cRecvBuf[10], "00", 2) )		// スイッチスレーブ
		{
			LOG(em_ERR), "[%s] <%d> 受信伝文 異常 [ステータスレジスタ相違:DB(%s)≠REG(%s)]",
				my_sThreadName, m_nSwMode,
				m_Param.linksw[ m_nSwMode ].bStatus ? "スイッチマスタ" : "スイッチスレーブ",
				m_Param.linksw[ m_nSwMode ].bStatus ? "スイッチスレーブ" : "スイッチマスタ");
			ChangeWaitEventNum( false );													// 1シーケンス強制終了
			return false;
		}

		// スイッチFPGA毎のモード設定
		if( m_nSwMode != NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA-1 )								// 最後のスイッチFPGA以外時
		{
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nApxMode = 0;
			m_nAocMode = 0;
			m_nAocSubMode = 0;
			m_nRegSizeMode = 0;
			m_nSwMode++;

			// 次の送信データ決定 & 送信
			if( ! CheckNextSend() )
			{
				return false;
			}
			break;
		}

		// 接続完了
		LOG(em_MSG), "[%s] カメラリンクスイッチ回路 接続確認完了 --------------------------------------------------<<<", my_sThreadName);
		syslog(212, "");
		m_bIsConnect = true;
		//sWk.Format("CAMSWITCH_ID_%d",m_nUnit+1);
		//KizuFunction::SetStatus(sWk, true);
		if (true) {
			CString sWk;
			sWk.Format("CAMSWITCH_ID_%d", m_nUnit + 1);
			int id = GetPrivateProfileInt(INI_STATUS, sWk, -1, TASKINI_NAME);
			if (-1 == id) {
				KizuFunction::SetStatus("CAMSWITCH_ID", true);
			}
			else {
				KizuFunction::SetStatus(sWk, true);
			}
		}

		// 定周期チェックタイマ 開始
		LARGE_INTEGER ts;
		ts.QuadPart = (__int64)m_nCycleTime * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
		SetWaitableTimer(m_evTimerCycleCheck, &ts, 0, NULL, NULL, FALSE);

		// オープンチェック → 初期化
		m_emMainMode = MODE_START_UP;
		m_nSubMode = 0;
		m_nCamMode = 0;
		m_nApxMode = 0;
		m_nAocMode = 0;
		m_nAocSubMode = 0;
		m_nRegSizeMode = 0;
		m_nSwMode = 0;
		if( ! CheckNextSend() )
		{
			return false;
		}
		break;

//---------------------------------------------------------
	////////////////////////////////////
	// 初期化
	////////////////////////////////////
	case MODE_START_UP:
		if( 7 == m_nSubMode ||
			8 == m_nSubMode ||
			9 == m_nSubMode )
		{
			if( !CheckDataRecv_AOC() )
						return false;
					}
		else
		{
			//// ライトデータ チェック
			if( 0 != memcmp(&m_cRecvBuf[10], &m_cSendBuf[8], 2) )
			{
				LOG(em_ERR), "[%s] <%d> 受信伝文 異常 [ライトデータ相違:送信(%.2s)≠受信(%.2s)]",
					my_sThreadName, m_nSwMode, &m_cSendBuf[8], &m_cRecvBuf[10]);
				ChangeWaitEventNum( false );												// 1シーケンス強制終了
				return false;
			}
		}

		// スイッチFPGA毎のモード設定
		if( 10 == m_nSubMode &&
			0 == m_nAocMode )																// 最後のAOC時
		{
			if( m_nSwMode != NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA-1 )							// 最後のスイッチFPGA以外時
			{
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nApxMode = 0;
				m_nAocMode = 0;
				m_nAocSubMode = 0;
				m_nRegSizeMode = 0;
				m_nSwMode++;
			}
		}

		// 次の送信データ決定 & 送信
		if( ! CheckNextSend() )
		{
			return false;
		}
		break;

//---------------------------------------------------------
	////////////////////////////////////
	// 検査開始設定
	////////////////////////////////////
	case MODE_START_INIT:
		//// ライトデータ チェック
		if( 0 != memcmp(&m_cRecvBuf[10], &m_cSendBuf[8], 2) )
		{
			LOG(em_ERR), "[%s] <%d> 受信伝文 異常 [ライトデータ相違:送信(%.2s)≠受信(%.2s)]",
				my_sThreadName, m_nSwMode, &m_cSendBuf[8], &m_cRecvBuf[10]);
			ChangeWaitEventNum( false );													// 1シーケンス強制終了
			return false;
		}

		// スイッチFPGA毎のモード設定
		if( 4 == m_nSubMode &&
			0 == m_nAocMode )																// 最後のAOC時
		{
			if( m_nSwMode != NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA-1 )							// 最後のスイッチFPGA以外時
			{
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nApxMode = 0;
				m_nAocMode = 0;
				m_nAocSubMode = 0;
				m_nRegSizeMode = 0;
				m_nSwMode++;
			}	
		}

		// 次の送信データ決定 & 送信
		if( ! CheckNextSend() )
		{
			return false;
		}
		break;

// 2020.05.07 対象カメラのみ設定 ---------->>>>
//---------------------------------------------------------
////////////////////////////////////
// 検査開始設定(カメラ選択設定)
////////////////////////////////////
	case MODE_START_SELECT_INIT:
		//// ライトデータ チェック
		if (0 == memcmp("=", &m_cSendBuf[7], 1) &&
			0 != memcmp(&m_cRecvBuf[10], &m_cSendBuf[8], 2))
		{
			LOG(em_ERR), "[%s] <%d> 受信伝文 異常 [ライトデータ相違:送信(%.2s)≠受信(%.2s)]",
				my_sThreadName, m_nSwMode, &m_cSendBuf[8], &m_cRecvBuf[10]);
				ChangeWaitEventNum(false);													// 1シーケンス強制終了
				return false;
		}

		// スイッチFPGA毎のモード設定
		if (2 == m_nSubMode &&
			0 == m_nRegSizeMode &&
			0 == m_nApxMode)																// 最後のボード時
		{
			if (m_nSwMode != NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA - 1)							// 最後のスイッチFPGA以外時
			{
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nApxMode = 0;
				m_nAocMode = 0;
				m_nAocSubMode = 0;
				m_nRegSizeMode = 0;
				m_nSwMode++;
			}
		}

		// 次の送信データ決定 & 送信
		if (!CheckNextSend())
		{
			return false;
		}
		break;

// 2020.05.07 対象カメラのみ設定 <<<<----------

//---------------------------------------------------------
	////////////////////////////////////
	// 途中参加設定
	////////////////////////////////////
	case MODE_SLAVE_JOIN:
		//// ライトデータ チェック
		if( 0 == memcmp("=",             &m_cSendBuf[7], 1) &&
			0 != memcmp(&m_cRecvBuf[10], &m_cSendBuf[8], 2) )
		{
			LOG(em_ERR), "[%s] <%d> 受信伝文 異常 [ライトデータ相違:送信(%.2s)≠受信(%.2s)]",
				my_sThreadName, m_nSwMode, &m_cSendBuf[8], &m_cRecvBuf[10]);
			ChangeWaitEventNum( false );													// 1シーケンス強制終了
			return false;
		}

		// スイッチFPGA毎のモード設定
		if( 1 == m_nSubMode &&
			0 == m_nRegSizeMode &&
			0 == m_nApxMode )																// 最後のボード時
		{
			if( m_nSwMode != NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA-1 )							// 最後のスイッチFPGA以外時
			{
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nApxMode = 0;
				m_nAocMode = 0;
				m_nAocSubMode = 0;
				m_nRegSizeMode = 0;
				m_nSwMode++;
			}
		}

		// 次の送信データ決定 & 送信
		if( ! CheckNextSend() )
		{
			return false;
		}
		break;

//---------------------------------------------------------
	////////////////////////////////////
	// カウンタリセット
	////////////////////////////////////
	case MODE_COUNTER_RESET:
		//// ライトデータ チェック
		if( 0 != memcmp(&m_cRecvBuf[10], &m_cSendBuf[8], 2) )
		{
			LOG(em_ERR), "[%s] <%d> 受信伝文 異常 [ライトデータ相違:送信(%.2s)≠受信(%.2s)]",
				my_sThreadName, m_nSwMode, &m_cSendBuf[8], &m_cRecvBuf[10]);
			ChangeWaitEventNum( false );													// 1シーケンス強制終了
			return false;
		}

#if 0	// 途中参加カウンタマニュアルリセットの動作が不明のためヤメ
		// スイッチFPGA毎のモード設定
		if( 3 == m_nSubMode &&
			0 == m_nApxMode )																// 最後のボード時
		{
			if( m_nSwMode != NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA-1 )							// 最後のスイッチFPGA以外時
			{
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nApxMode = 0;
				m_nAocMode = 0;
				m_nAocSubMode = 0;
				m_nRegSizeMode = 0;
				m_nSwMode++;
			}
		}
#else
		// スイッチFPGA毎のモード設定
		if( 1 == m_nSubMode &&
			0 == m_nRegSizeMode &&
			0 == m_nApxMode )																// 最後のモード時
		{
			if( m_nSwMode != NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA-1 )							// 最後のスイッチFPGA以外時
			{
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nApxMode = 0;
				m_nAocMode = 0;
				m_nAocSubMode = 0;
				m_nRegSizeMode = 0;
				m_nSwMode++;
			}
		}
#endif

		// 次の送信データ決定 & 送信
		if( ! CheckNextSend() )
		{
			return false;
		}
		break;

//---------------------------------------------------------
	////////////////////////////////////
	// 定周期チェック
	////////////////////////////////////
	case MODE_CYCLE_CHECK:
		if( 1 == m_nSubMode )
		{
			//// 周囲温度 チェック
			char cWk[256];
			int nTemp;		// 温度 [℃]
			memset(cWk, 0x00, sizeof(cWk));
			memcpy(cWk, &m_cRecvBuf[10], 2);
			sscanf(cWk, "%X", &nTemp);
			LOG(em_INF), "[%s] <%d> 周囲温度 [%d℃]", my_sThreadName, m_nSwMode, nTemp);

			// 温度保存
			if( nTemp != m_nAmbTemp[m_nSwMode] )
			{
				// 温度変化時のみ
				m_nAmbTemp[m_nSwMode] = nTemp;
				if( 0 != m_nAmbTempID[m_nSwMode] )		KizuFunction::SetStatus(m_nAmbTempID[m_nSwMode], nTemp, false);
			}
			// 温度チェック
			bool bLimitOk = true;
			if( 0 != m_nAmbTempLimit[m_nSwMode] && m_nAmbTempLimit[m_nSwMode] <= nTemp ) bLimitOk = false;
			if( ! bLimitOk && m_bAmbTempSt[m_nSwMode] )
			{
				// 正常 → 異常
				if( 0 != m_nAmbTempStID[m_nSwMode] )	KizuFunction::SetStatus(m_nAmbTempStID[m_nSwMode], false);
			}
			else if( bLimitOk && ! m_bAmbTempSt[m_nSwMode] )
			{
				// 異常 → 正常
				if( 0 != m_nAmbTempStID[m_nSwMode] )	KizuFunction::SetStatus(m_nAmbTempStID[m_nSwMode], true);
			}
			m_bAmbTempSt[m_nSwMode] = bLimitOk;
		}
		else if( 2 == m_nSubMode )
		{
			//// FPGAダイ温度 チェック
			char cWk[256];
			int nTemp;		// 温度 [℃]
			memset(cWk, 0x00, sizeof(cWk));
			memcpy(cWk, &m_cRecvBuf[10], 2);
			sscanf(cWk, "%X", &nTemp);
			LOG(em_INF), "[%s] <%d> FPGAダイ温度 [%d℃]", my_sThreadName, m_nSwMode, nTemp);

			// 温度保存
			if( nTemp != m_nDieTemp[m_nSwMode] )
			{
				// 温度変化時のみ
				m_nDieTemp[m_nSwMode] = nTemp;
				if( 0 != m_nDieTempID[m_nSwMode] )		KizuFunction::SetStatus(m_nDieTempID[m_nSwMode], nTemp, false);
			}
			// 温度チェック
			bool bLimitOk = true;
			if( 0 != m_nDieTempLimit[m_nSwMode] && m_nDieTempLimit[m_nSwMode] <= nTemp ) bLimitOk = false;
			if( ! bLimitOk && m_bDieTempSt[m_nSwMode] )
			{
				// 正常 → 異常
				if( 0 != m_nDieTempStID[m_nSwMode] )	KizuFunction::SetStatus(m_nDieTempStID[m_nSwMode], false);
			}
			else if( bLimitOk && ! m_bDieTempSt[m_nSwMode] )
			{
				// 異常 → 正常
				if( 0 != m_nDieTempStID[m_nSwMode] )	KizuFunction::SetStatus(m_nDieTempStID[m_nSwMode], true);
			}
			m_bDieTempSt[m_nSwMode] = bLimitOk;
		}
		else if( 3 == m_nSubMode ||
				 4 == m_nSubMode )
		{
			//// Opt-C:Linkステータス チェック
			char cWk[256];
			BYTE nVal;
			memset(cWk, 0x00, sizeof(cWk));
			memcpy(cWk, &m_cRecvBuf[10], 2);
			nVal = (BYTE)strtol(cWk, NULL, 16);

			// レジスタ値保存
			if( 3 == m_nSubMode )	m_nOptCLinkSt[m_nSwMode][0 != m_nCamMode ? m_nCamMode - 1 : MAX_CAMLINKSW_ALLINPUT - 1][0] = nVal;	// B
			else					m_nOptCLinkSt[m_nSwMode][0 != m_nCamMode ? m_nCamMode - 1 : MAX_CAMLINKSW_ALLINPUT - 1][1] = nVal;	// M/F
		}
		//else
		//{
		//	//// ライトデータ チェック
		//	if( 0 != memcmp(&m_cRecvBuf[10], &m_cSendBuf[8], 2) )
		//	{
		//		LOG(em_ERR), "[%s] <%d> 受信伝文 異常 [ライトデータ相違:送信(%.2s)≠受信(%.2s)]",
		//			my_sThreadName, m_nSwMode, &m_cSendBuf[8], &m_cRecvBuf[10]);
		//		ChangeWaitEventNum( false );												// 1シーケンス強制終了
		//		return false;
		//	}
		//}

		// スイッチFPGA毎のモード設定
		if( 6 == m_nSubMode &&
			0 == m_nCamMode )																// 最後のカメラ時
		{
			if( m_nSwMode != NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA-1 )							// 最後のスイッチFPGA以外時
			{
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nApxMode = 0;
				m_nAocMode = 0;
				m_nAocSubMode = 0;
				m_nRegSizeMode = 0;
				m_nSwMode++;
			}
		}

		// 次の送信データ決定 & 送信
		if( ! CheckNextSend() )
		{
			return false;
		}
		break;

//---------------------------------------------------------
	////////////////////////////////////
	// カウンタチェック
	////////////////////////////////////
	case MODE_COUNTER_CHECK:
		if( 2 == m_nSubMode ||
			4 == m_nSubMode )
		{
			//// 途中参加カウンタ チェック
			char cWk[256];
			__int64	nVal;
			memset(cWk, 0x00, sizeof(cWk));
			memcpy(cWk, &m_cRecvBuf[10], 2);
			nVal = strtol(cWk, NULL, 16);
			nVal <<= (64 - 8 * (m_nRegSizeMode + 1));

			// カウンタ値保存
			if( 2 == m_nSubMode )	m_nCounterNow[m_nSwMode][1 != (m_nRegSizeMode + 1) ? m_nApxMode : m_nApxMode - 1] += nVal;			// 現在値
			else					m_nCounterSet[m_nSwMode][1 != (m_nRegSizeMode + 1) ? m_nApxMode : m_nApxMode - 1] += nVal;			// 設定値
		}

		// スイッチFPGA毎のモード設定
		if( 4 == m_nSubMode &&
			0 == m_nRegSizeMode &&
			0 == m_nApxMode )																// 最後のボード時
		{
			if( m_nSwMode != NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA-1 )							// 最後のスイッチFPGA以外時
			{
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nApxMode = 0;
				m_nAocMode = 0;
				m_nAocSubMode = 0;
				m_nRegSizeMode = 0;
				m_nSwMode++;
			}
		}

		// 次の送信データ決定 & 送信
		if( ! CheckNextSend() )
		{
			return false;
		}
		break;
	case MODE_CHANGE_AOC_SERIALSPEED:
		if( 1 == m_nSubMode ){
			if( !CheckDataRecv_AOC() )
				return false;
		}
		else
		{
			//// ライトデータ チェック
			if( 0 != memcmp(&m_cRecvBuf[10], &m_cSendBuf[8], 2) )
			{
				LOG(em_ERR), "[%s] <%d> 受信伝文 異常 [ライトデータ相違:送信(%.2s)≠受信(%.2s)]",
					my_sThreadName, m_nSwMode, &m_cSendBuf[8], &m_cRecvBuf[10]);
				ChangeWaitEventNum( false );												// 1シーケンス強制終了
				return false;
			}
		}

		// スイッチFPGA毎のモード設定
		if( 10 == m_nSubMode &&
			0 == m_nAocMode )																// 最後のAOC時
		{
			if( m_nSwMode != NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA-1 )							// 最後のスイッチFPGA以外時
			{
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nApxMode = 0;
				m_nAocMode = 0;
				m_nAocSubMode = 0;
				m_nRegSizeMode = 0;
				m_nSwMode++;
			}
		}

		// 次の送信データ決定 & 送信
		if( ! CheckNextSend() )
		{
			return false;
		}
		break;
//---------------------------------------------------------
	default:
		LOG(em_ERR), "[%s] モード異常 [MainMode = %d]", my_sThreadName, m_emMainMode);
		syslog(210, "[mode = %d]", m_emMainMode);
		ChangeWaitEventNum( false );
		return false;
	}

	return true;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 外部通知


//------------------------------------------
// 統括管理 へ カメラリンクスイッチ設定完了通知
//------------------------------------------
void LinkswManager::Send_ToDefect_StartInit(bool bFlg)
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));

	que.nEventNo	= FACT_TO_DEFECT_30;
	//que.nLineNo = getlineid();
	que.nLineNo		= m_nUnit+1;
	que.fl.data[0]	= bFlg;				// 接続正常異常フラグ

	// 送信
	int nRetc = send_mail(TO_DEFECT, ".", &que);
	if( 0 != nRetc ) syslog(SYSNO_MAIL_SNED_ERR, "[統括管理へのカメラリンクスイッチ設定完了通知 err_code=%d]", nRetc);
	LOG(em_MSG), "[%s] 統括管理へのカメラリンクスイッチ設定完了通知 [Fact = %d][Flg:%d]", my_sThreadName, que.nEventNo, bFlg);
}

//------------------------------------------
// 統括管理 へ 途中参加設定完了通知
//------------------------------------------
void LinkswManager::Send_ToDefect_SlaveJoin()
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));

	que.nEventNo	= FACT_TO_DEFECT_31;
	//que.nLineNo		= getlineid();
	que.nLineNo		= m_nUnit+1;
	que.fl.data[0]	= m_nSlaveJoinTorB;		// 途中参加表裏区分
	que.fl.data[1]	= m_nSlaveJoinFNo;		// 途中参加フレームNo

	// 送信
	int nRetc = send_mail(TO_DEFECT, ".", &que);
	if( 0 != nRetc ) syslog(SYSNO_MAIL_SNED_ERR, "[統括管理への途中参加設定完了通知 err_code=%d]", nRetc);
	LOG(em_MSG), "[%s] 統括管理への途中参加設定完了通知 [Fact = %d]", my_sThreadName, que.nEventNo);
}

//------------------------------------------
// 統括管理 へ カメラ選択設定完了通知
//------------------------------------------
void LinkswManager::Send_ToDefect_SelectInit(bool bFlg)
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));

	que.nEventNo = FACT_TO_DEFECT_32;
	//que.nLineNo = getlineid();
	que.nLineNo = m_nUnit + 1;
	memcpy(que.fl.data, m_RecvQue.fl.data, sizeof(que.fl.data));
	que.fl.data[4] = bFlg;				// 接続正常異常フラグ

										// 送信
	int nRetc = send_mail(TO_DEFECT, ".", &que);
	if (0 != nRetc) syslog(SYSNO_MAIL_SNED_ERR, "[統括管理へのカメラ選択設定完了通知 err_code=%d]", nRetc);
	LOG(em_MSG), "[%s] 統括管理へのカメラ選択設定完了通知 [Fact = %d][Flg:%d]", my_sThreadName, que.nEventNo, bFlg);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 外部アクセス


//------------------------------------------
// 現モード名称取得
// 戻り値 モード名称
//------------------------------------------
CString LinkswManager::GetMainModeName()
{
	return GetMainModeName(m_emMainMode);
}

//------------------------------------------
// 指定モード名称取得
// ENUM_MAINMODE emMainMode メインモード
// 戻り値 モード名称
//------------------------------------------
CString LinkswManager::GetMainModeName(ENUM_MAINMODE emMainMode)
{
	CString wk;
	switch( emMainMode )
	{
	case MODE_NONE:
		wk = "初期状態";
		break;
	case MODE_OPEN_CHECK:
		wk = "オープンチェック";
		break;
	case MODE_START_UP:
		wk = "初期化";
		break;
	case MODE_START_INIT:
		wk = "検査開始設定";
		break;
	case MODE_SLAVE_JOIN:
		wk = "途中参加設定";
		break;
	case MODE_COUNTER_RESET:
		wk = "カウンタリセット";
		break;
	case MODE_COUNTER_CHECK:
		wk = "カウンタチェック";
		break;
	case MODE_CYCLE_CHECK:
		wk = "定周期チェック";
		break;
	case MODE_WAIT:
		wk = "設定待ち";
		break;
	case MODE_RETRY:
		wk = "オープンリトライ中";
		break;
	case MODE_CHANGE_AOC_SERIALSPEED:
		wk = "AOCシリアル速度設定";
		break;
// 2020.05.07 対象カメラのみ設定 ---------->>>>
	case MODE_START_SELECT_INIT:
		wk = "検査開始設定(カメラ選択設定)";
		break;
// 2020.05.07 対象カメラのみ設定 <<<<----------
	}
	return wk;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 232C関連

//LINKSWを高速モード(115200bps)に変更する。
//Rs232cBaseのモード（KizuTask.iniの記載）によらず、9600bpsで
//  送信する必要があるので、直接シリアルをたたく。
//また、ここを変更すると、グラバーボードのシリアルも115200bpsになるので注意
bool LinkswManager::DirectSendSpeedChange()
{
	// 変更する必要なし。LINKSWをリセットすれば9600になる。
	if( LINKW_SERIAL_SPEED == 9600 )
		return true;

	char cComName[256];
	GetPrivateProfileString(m_cMySession, "com", "",	cComName, sizeof(cComName), TASKINI_NAME);
	if (strlen(cComName) == 0) {
		return false;				// COMポート指定がない
	}

	// 通信ポートをオープンする
	const HANDLE hCom = CreateFile(cComName, GENERIC_READ|GENERIC_WRITE , NULL,
						NULL, OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if( hCom == INVALID_HANDLE_VALUE ) 
		return false;
	{
		DCB dcb = {sizeof(DCB)};
		GetCommState( hCom, &dcb );
		dcb.BaudRate = CBR_9600;
		dcb.fBinary = 1;
		dcb.fDtrControl = DTR_CONTROL_ENABLE;
		dcb.fRtsControl = RTS_CONTROL_ENABLE;
		SetCommState( hCom, &dcb );
	}
	{
		COMMTIMEOUTS timeout = {};
		timeout.WriteTotalTimeoutMultiplier = 1;
		SetCommTimeouts( hCom, &timeout);
	}
	PurgeComm(hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

	// TODO. デイジーチェーンの場合、カメラリンクの台数だけ、送信しないとだめかも。
	// そのときは、おそらく、チェーンの遠いほうから送信する必要がある。。
#if 1
	static const char sCmd[] = "@FFW1D0=04\r" "@FFW1D1=01\r";
	DWORD nSent = 0;
	int r = WriteFile(hCom, &sCmd, sizeof( sCmd ) -1, &nSent, 0 );
	LOG(em_MSG), "[%s] CamLinkSW SpeedUp.(%d) => %d(%d)", my_sThreadName, nSent, r, GetLastError());
#else
	// ホストアクセスシリアル通信ボーレート選択レジスタ
	// 設定変更後、1D1h番地のレジスタ有効時に変更内容が反映
	for (int ii = NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; ii > 0; ii--) {
		char sCmd[1024];
		sprintf(sCmd, "@%02XW1D0=04\r", ii-1);
		DWORD nSent = 0;
		int r = WriteFile(hCom, &sCmd, sizeof(sCmd) - 1, &nSent, 0);
		LOG(em_MSG), "[%s] CamLinkSW SpeedUp. Board:%d Select[%s] (%d) => %d(%d)", my_sThreadName, (ii - 1), sCmd, nSent, r, GetLastError());
	}
	// ホストアクセスシリアル通信ボーレート設定レジスタ
	for (int ii = NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; ii > 0; ii--) {
		{
			char sCmd[1024];
			sprintf(sCmd, "@%02XW1D1=01\r", ii - 1);
			DWORD nSent = 0;
			int r = WriteFile(hCom, &sCmd, sizeof(sCmd) - 1, &nSent, 0);
			LOG(em_MSG), "[%s] CamLinkSW SpeedUp. Board:%d Set[%s] (%d) => %d(%d)", my_sThreadName, (ii - 1), sCmd, nSent, r, GetLastError());
		}
	}
#endif


	//戻りはこない可能性が高いので読まない。
	// (要は、すでに115200bpsになっていたとき。)
	CloseHandle( hCom );

	return true;

}
//------------------------------------------
// 232C オープン
// bool bDspLog false:ログ出力無し
//------------------------------------------
bool LinkswManager::Open(bool bDspLog)
{
	// Rs232cBase インスタンス生成
	try
	{
		if( ! m_bInitialSpeedChange && m_bEnableRapidSerial  )
		{
			if (DirectSendSpeedChange() )
			{
				m_bInitialSpeedChange = true;
			}

		}

		// カメラコントローラとPC間は、制御線が繋がっていない為、ソフトフロー制御を無効とする
		mcls_p232c = new Rs232cBase(m_cMySession, TASKINI_NAME, GetTid(), PARAM_232C_RECVBUF_SIZE, PARAM_232C_SENDBUF_SIZE, WM_USER, true);
		mcls_p232c->SetRecvEndCR(CR);	// 終端コードチェックに可変長に読込みとする
	}
	catch( int code )
	{
		mcls_p232c = NULL;
		if( bDspLog )
		{
			LOG(em_ERR), "[%s] 232Cオープンエラー(err_code = %d)", my_sThreadName, code);
			syslog(SYSNO_232C_NEW_ERR, "[err_code = %d]", code);
		}
		return false;
	}

	// 接続完了
	memset(m_cRecvBuf, 0x00, sizeof(m_cRecvBuf));
	memset(m_cSendBuf, 0x00, sizeof(m_cSendBuf));
	//syslog(SYSNO_232C_OPEN, "");

	// カメラリンクスイッチと通信できるか確認
	m_emMainMode = MODE_OPEN_CHECK;
	m_nSubMode = 0;
	m_nCamMode = 0;
	m_nApxMode = 0;
	m_nAocMode = 0;
	m_nAocSubMode = 0;
	m_nRegSizeMode = 0;
	m_nSwMode = 0;
	if( ! CheckNextSend() ) return false;
	return true;
}

//------------------------------------------
// 232C クローズ
//------------------------------------------
void LinkswManager::Close()
{
	// 全タイマ停止
	CancelWaitableTimer(m_evReTry);
	CancelWaitableTimer(m_evAnswerTimeOut);
	CancelWaitableTimer(m_evTimerCycleCheck);

	// クローズ
	if( NULL != mcls_p232c )
	{
		SAFE_DELETE(mcls_p232c);
	}

	//// リトライのたびにログ出力されるの禁止
	// 一度繋がった以降からの出力
	if( m_bIsConnect )
	{
		LOG(em_MSG), "[%s] 232Cクローズ", my_sThreadName);
		syslog(SYSNO_232C_CLOSE, "");
		KizuFunction::SetStatus("CAMSWITCH_ID", false);
	}

	// ステータス初期化
	m_bIsConnect = false;
	m_emMainMode = MODE_RETRY;
	m_nSubMode = 0;
	m_nCamMode = 0;
	m_nApxMode = 0;
	m_nAocMode = 0;
	m_nAocSubMode = 0;
	m_nRegSizeMode = 0;
	m_nSwMode = 0;
	ChangeWaitEventNum( false );

	// リトライタイマ セット
	LARGE_INTEGER ts;
	ts.QuadPart = (__int64)PARAM_TIMEOUT_RETRY * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evReTry, &ts, 0, NULL, NULL, FALSE);
}

//------------------------------------------
// 232C 非同期 受信
//------------------------------------------
bool LinkswManager::Recv()
{
	if( NULL == mcls_p232c ) return true;

	// 受信バッファクリア
	memset(m_cRecvBuf, 0x00, sizeof(m_cRecvBuf));

	// 終端コードを見てシグナルを返すタイプ
	int iRetc = mcls_p232c->Recv(m_cRecvBuf, 1, m_evRecvEnd);		// 可変長読込みの為、1バイト固定
	if( 0 != iRetc )
	{
		// 受信エラー
		LOG(em_ERR), "[%s] 232C受信エラー(err_code = %d)", my_sThreadName, iRetc);
		syslog(SYSNO_232C_RECV_ERR, "[err_code = %d]", iRetc);
		return false;
	}
	return true;
}

//------------------------------------------
// 232C 非同期 送信
//------------------------------------------
bool LinkswManager::Send()
{
	if( NULL == mcls_p232c ) return true;

	//// 送信
	LOG(em_INF), "[%s] コマンド送信 [%s]", my_sThreadName, m_cSendBuf);
	int iRetc = mcls_p232c->Send(m_cSendBuf, (DWORD)strlen(m_cSendBuf), m_evSendEnd);
	if( 0 != iRetc )
	{
		// 受信エラー
		LOG(em_ERR), "[%s] 232C送信エラー(err_code = %d)", my_sThreadName, iRetc);
		syslog(SYSNO_232C_SEND_ERR, "[err_code = %d]", iRetc);
		return false;
	}

	//// 送信中
	// 応答伝文待ちタイマ セット
	LARGE_INTEGER ts;
	ts.QuadPart = (__int64)PARAM_TIMEOUT_ANSWER * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_evAnswerTimeOut, &ts, 0, NULL, NULL, FALSE);
	return true;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// パラメータ関連


//------------------------------------------
// 動的パラメータ取得
//------------------------------------------
bool LinkswManager::GetParam()
{
	// 現時点では動的パラメータは無し！！
	return true;
}

//------------------------------------------
// 静的パラメータ取得
//------------------------------------------
bool LinkswManager::GetParamStatic()
{
	CString sql;					// SQL文
	int nId;
	int nCnt;

	//// 初期化
	memset(&m_Param, 0x00, sizeof(m_Param));

	//serialの初期値は9600.
	std::fill_n( m_Param.nSerialSpeeds, _countof(m_Param.nSerialSpeeds), 9600 );

#if 1
	//// ODBC接続クラス インスンタス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME) )
	{
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	////// 共通パラメータ
	//// SQL文作成
	sql = "SELECT";
	sql += " カメラタップ数";
#ifdef AOI_HEIGHT
	sql += ", AOIheight";
#endif
	sql += " FROM ";
	sql += DB_PARAM_COMMON;
	//// 実行
	if( ! clsDB.ExecuteDirect(sql) )
	{
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", DB_PARAM_COMMON);
		return false;
	}
	//// 取得
	while( TRUE )
	{
		int sqlrc = clsDB.Fetch();
		if( sqlrc == KIZUODBC_FETCH_OK )						// データ有り
		{
			int si = 1;
			clsDB.GetData(si++, &m_Param.nLineBuffer);
#ifdef AOI_HEIGHT
			clsDB.GetData(si++, &m_Param.nYSize);	// AOIheight
#else
			m_Param.nYSize = SIZE_FRAME_IMG_Y;	// 縦フレーム画像サイズ
#endif
		}
		else if( sqlrc == KIZUODBC_FETCH_NODATE )				// データ無し
		{
			break;
		}
		else													// エラー
		{
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", DB_PARAM_COMMON);
			return false;
		}
	}
#else
	m_Param.nLineBuffer	= 8;
	m_Param.nYSize		= SIZE_FRAME_IMG_Y;
#endif

#if 1		// DBから取得
	////// カメラリンクスイッチ設定パラメータ
	//// SQL文作成
	sql = "SELECT";
	sql += " ボードID";
	sql += ", ステータス";
	sql += ", ボード1_カメラ選択";
	sql += ", ボード2_カメラ選択";
	sql += ", ボード3_カメラ選択";
	sql += ", ボード4_カメラ選択";
	sql += ", ボード5_カメラ選択";
	sql += ", ボード6_カメラ選択";
	sql += ", ボード7_カメラ選択";
	sql += ", ボード8_カメラ選択";
	sql += ", ボード9_カメラ選択";
	sql += ", ボード10_カメラ選択";
	sql += ", ボード11_カメラ選択";
	sql += ", ボード12_カメラ選択";
	sql += ", ボード13_カメラ選択";
	sql += ", ボード14_カメラ選択";
	sql += ", ボード15_カメラ選択";
	sql += " FROM ";
	sql += DB_CAMERA_LINKSWITCH;
	sql += " WHERE ";
	sql.AppendFormat(" ID =  %d ", m_nUnit+1);
	sql += " ORDER BY ID ASC";		// IDの昇順
	//// 実行
	if( ! clsDB.ExecuteDirect(sql) )
	{
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", DB_CAMERA_LINKSWITCH);
		return false;
	}
	//// 取得
	nCnt = 0;
	while( TRUE )
	{
		int sqlrc = clsDB.Fetch();
		if( sqlrc == KIZUODBC_FETCH_OK )						// データ有り
		{
			// ID
			clsDB.GetData(1, &nId);
			if( 0 > nId || NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA <= nId )
			{
				// スイッチFPGAボードID 異常
				LOG(em_ERR), "[%s] スイッチFPGAボードID 異常 [ID=%d(ID:0～%d)]", my_sThreadName, nId, NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA-1);
				return false;
			}

			// ステータス
			clsDB.GetData(2, &m_Param.linksw[nId].bStatus);
			LOG(em_INF), "[%s] スイッチFPGAボードID 取得 [ID=%d, スイッチ%s]", my_sThreadName, nId, m_Param.linksw[nId].bStatus ? "マスタ" : "スレーブ");

			// ボード1～15_カメラ選択
			for( int ii=0; ii<MAX_CAMLINKSW_OUTPUT; ii++ )
			{
				// 2017.02.27 Master/SlaveをDB指定 ------------------------------------------------>>>>
				//clsDB.GetData(3+ii, &m_Param.linksw[nId].nSelectCam[ii]);
				//if( 0 > m_Param.linksw[nId].nSelectCam[ii] || MAX_CAMLINKSW_ALLINPUT < m_Param.linksw[nId].nSelectCam[ii] )
				//{
				//	// カメラ選択設定 異常
				//	LOG(em_ERR), "[%s] カメラ選択設定(DB) 異常 [ID=%d, ボード%d=%d]", my_sThreadName, nId, ii+1, m_Param.linksw[nId].nSelectCam[ii]);
				//	return false;
				//}
				//LOG(em_INF), "[%s] カメラ選択設定(DB) 取得 [ID=%d, ボード%d=%d]", my_sThreadName, nId, ii+1, m_Param.linksw[nId].nSelectCam[ii]);

				int                 nSelectCh   = 0;					// 選択ch
				ENUM_CH_SELECT_MODE emChSelMode = CH_SELECT_NON;		// Master/Slave/未選択

				clsDB.GetData(3+ii, &nSelectCh);
				if (m_bHalfFlg[nId] && !HARF_CC_SOCK_EXIST[ii]) {
					// ハーフ版カメラリンクSWの存在しないボードIDの場合は強制的に未選択
					if(0 != nSelectCh){
						LOG(em_ERR), "[%s] <%d> ハーフ版に存在しないボードが初期値されていたため強制的に未選択に変更(board=%d, cam=%d)", my_sThreadName, nId, ii+1, nSelectCh);
						nSelectCh = 0;
					}
				}

				// 選択chが負数ならばSlave指定
				if( -1 >= nSelectCh )	{ emChSelMode = CH_SELECT_SLAVE; }
				if( 0  <  nSelectCh )	{ emChSelMode = CH_SELECT_MASTER; }

				m_Param.linksw[nId].nSelectCam[ii] = abs(nSelectCh);			// これ以降chとして使う為に、正数値にする
				m_Param.linksw[nId].emChSelMode[ii] = emChSelMode;
				// 2020.05.07 対象カメラのみ設定 ---------->>>>
				m_Param.linksw[nId].bChSelCam[ii] = FALSE;
				// 2020.05.07 対象カメラのみ設定 <<<<----------

				if( 0 > m_Param.linksw[nId].nSelectCam[ii] || MAX_CAMLINKSW_ALLINPUT < m_Param.linksw[nId].nSelectCam[ii] )
				{
					// カメラ選択設定 異常
					LOG(em_ERR), "[%s] カメラ選択設定(DB) 異常 [ID=%d, ボード%d=%d, %s]", my_sThreadName, nId, ii+1, m_Param.linksw[nId].nSelectCam[ii], GetChSelectModeName(m_Param.linksw[nId].emChSelMode[ii]));
					return false;
				}
				LOG(em_INF), "[%s] カメラ選択設定(DB) 取得 [ID=%d, ボード%d=%d, %s]", my_sThreadName, nId, ii+1, m_Param.linksw[nId].nSelectCam[ii], GetChSelectModeName(m_Param.linksw[nId].emChSelMode[ii]));
				// 2017.02.27 Master/SlaveをDB指定 <<<<------------------------------------------------

			}

			nCnt++;
		}
		else if( sqlrc == KIZUODBC_FETCH_NODATE )				// データ無し
		{
			break;
		}
		else													// エラー
		{
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", DB_CAMERA_LINKSWITCH);
			return false;
		}
	}
	// 取得レコード数チェック
	if( nCnt != NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA )
	{
		LOG(em_ERR), "[%s] カメラリンクスイッチ設定 件数異常 [CNT=%d(CNT:%d)]", my_sThreadName, nCnt, NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA);
		return false;
	}
#else
	m_Param.linksw[0].bStatus			= TRUE;
	m_Param.linksw[0].nSelectCam[0]		= 0;
	m_Param.linksw[0].nSelectCam[1]		= 1;
	m_Param.linksw[0].nSelectCam[2]		= 2;
	m_Param.linksw[0].nSelectCam[3]		= 3;
	m_Param.linksw[0].nSelectCam[4]		= 4;
	m_Param.linksw[0].nSelectCam[5]		= 0;
	m_Param.linksw[0].nSelectCam[6]		= 0;
	m_Param.linksw[0].nSelectCam[7]		= 0;
	m_Param.linksw[0].nSelectCam[8]		= 0;
	m_Param.linksw[0].nSelectCam[9]		= 0;
	m_Param.linksw[0].nSelectCam[10]	= 0;
	m_Param.linksw[0].nSelectCam[11]	= 0;
	m_Param.linksw[0].nSelectCam[12]	= 0;
	m_Param.linksw[0].nSelectCam[14]	= 0;

	m_Param.linksw[1].bStatus			= FALSE;
	m_Param.linksw[1].nSelectCam[0]		= 1;
	m_Param.linksw[1].nSelectCam[1]		= 2;
	m_Param.linksw[1].nSelectCam[2]		= 3;
	m_Param.linksw[1].nSelectCam[3]		= 4;
	m_Param.linksw[1].nSelectCam[4]		= 0;
	m_Param.linksw[1].nSelectCam[5]		= 0;
	m_Param.linksw[1].nSelectCam[6]		= 0;
	m_Param.linksw[1].nSelectCam[7]		= 0;
	m_Param.linksw[1].nSelectCam[8]		= 0;
	m_Param.linksw[1].nSelectCam[9]		= 0;
	m_Param.linksw[1].nSelectCam[10]	= 0;
	m_Param.linksw[1].nSelectCam[11]	= 0;
	m_Param.linksw[1].nSelectCam[12]	= 0;
	m_Param.linksw[1].nSelectCam[14]	= 0;

	//m_Param.linksw[2].bStatus			= TRUE;
	//m_Param.linksw[2].nSelectCam[0]		= 0;
	//m_Param.linksw[2].nSelectCam[1]		= 0;
	//m_Param.linksw[2].nSelectCam[2]		= 0;
	//m_Param.linksw[2].nSelectCam[3]		= 0;
	//m_Param.linksw[2].nSelectCam[4]		= 0;
	//m_Param.linksw[2].nSelectCam[5]		= 0;
	//m_Param.linksw[2].nSelectCam[6]		= 0;
	//m_Param.linksw[2].nSelectCam[7]		= 0;
	//m_Param.linksw[2].nSelectCam[8]		= 0;
	//m_Param.linksw[2].nSelectCam[9]		= 0;
	//m_Param.linksw[2].nSelectCam[10]	= 0;
	//m_Param.linksw[2].nSelectCam[11]	= 0;
	//m_Param.linksw[2].nSelectCam[12]	= 0;
	//m_Param.linksw[2].nSelectCam[14]	= 0;

	//m_Param.linksw[3].bStatus			= FALSE;
	//m_Param.linksw[3].nSelectCam[0]		= 0;
	//m_Param.linksw[3].nSelectCam[1]		= 0;
	//m_Param.linksw[3].nSelectCam[2]		= 0;
	//m_Param.linksw[3].nSelectCam[3]		= 0;
	//m_Param.linksw[3].nSelectCam[4]		= 0;
	//m_Param.linksw[3].nSelectCam[5]		= 0;
	//m_Param.linksw[3].nSelectCam[6]		= 0;
	//m_Param.linksw[3].nSelectCam[7]		= 0;
	//m_Param.linksw[3].nSelectCam[8]		= 0;
	//m_Param.linksw[3].nSelectCam[9]		= 0;
	//m_Param.linksw[3].nSelectCam[10]	= 0;
	//m_Param.linksw[3].nSelectCam[11]	= 0;
	//m_Param.linksw[3].nSelectCam[12]	= 0;
	//m_Param.linksw[3].nSelectCam[14]	= 0;
#endif

	LOG(em_MSG), "[%s] 静的パラメータ取得完了", my_sThreadName);
	return true;
}

//------------------------------------------
// 算出パラメータ取得
//------------------------------------------
bool LinkswManager::CalcParam()
{
	//// 初期化
	memset(m_Param.bCamInputCh, 0x00, sizeof(m_Param.bCamInputCh));
	memset(m_Param.nCamSelBoardNo, 0x00, sizeof(m_Param.nCamSelBoardNo));


	//// 2017.02.27 Master/SlaveをDB指定 ------------------------------------------------->>>>
	//for( int ii=0; ii<NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; ii++ )
	//{
	//	for( int jj=0; jj<MAX_CAMLINKSW_OUTPUT; jj++ )
	//	{
	//		//// 算出
	//		int nFpgaMasterNo	= ii / MAX_CAMLINKSW_FPGA * MAX_CAMLINKSW_FPGA;
	//		int nFpgaNo			= (m_Param.linksw[ii].nSelectCam[jj] - 1) / MAX_CAMLINKSW_INPUT;	// 0～1
	//		int nCamNo			= (m_Param.linksw[ii].nSelectCam[jj] - 1) % MAX_CAMLINKSW_INPUT;	// 0～3
	//		int nCamNoAll		=  m_Param.linksw[ii].nSelectCam[jj] - 1;							// 0～7
	//		// チェック
	//		if( 0 > nFpgaMasterNo + nFpgaNo	|| NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA	<= nFpgaMasterNo + nFpgaNo )	continue;
	//		if( 0 > nCamNo					|| MAX_CAMLINKSW_INPUT				<  nCamNo )						continue;
	//		if( 0 > nCamNoAll				|| MAX_CAMLINKSW_ALLINPUT			<  nCamNoAll )					continue;

	//		//// カメラ入力Ch
	//		m_Param.bCamInputCh[ nFpgaMasterNo + nFpgaNo ][ nCamNo ] = TRUE;

	//		//// カメラ選択ボードNo
	//		if( 0 == m_Param.nCamSelBoardNo[ nFpgaMasterNo + 0 ][ nCamNoAll ] &&
	//			0 == m_Param.nCamSelBoardNo[ nFpgaMasterNo + 1 ][ nCamNoAll ] )						// 若番優先
	//		{
	//			m_Param.nCamSelBoardNo[ ii ][ nCamNoAll ] = jj+1;
	//		}
	//	}
	//}

	int nMasterCheck[NUM_CAMLINKSW][MAX_CAMLINKSW_ALLINPUT] = {{0}};		// Master設定チェック[カメラリンクスイッチ][入力ch] (0:未選択 -1:Slaveのみ割りつき 1:Master割りつき)

	for( int ii=0; ii<NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; ii++ )
	{
		for( int jj=0; jj<MAX_CAMLINKSW_OUTPUT; jj++ )
		{
			//// 算出
			int nCamLinkSwNo    = ii / MAX_CAMLINKSW_FPGA;											// このボードが属するカメラリンクスイッチ
			int nFpgaMasterNo	= ii - (ii % MAX_CAMLINKSW_FPGA);									// このボードが属するカメラリンクスイッチのMasterボードNo
			int nFpgaNo			= (m_Param.linksw[ii].nSelectCam[jj] - 1) / MAX_CAMLINKSW_INPUT;	// このInputChはMasterボード／Slaveボードのどちらについているか(0～1)
			int nCamNo			= (m_Param.linksw[ii].nSelectCam[jj] - 1) % MAX_CAMLINKSW_INPUT;	// 選択したchのカメラNo(0～3)
			int nCamNoAll		=  m_Param.linksw[ii].nSelectCam[jj] - 1;							// 選択したch(0～7)

			// チェック
			if( 0 > nFpgaMasterNo + nFpgaNo	|| NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA	<= nFpgaMasterNo + nFpgaNo )	continue;
			if( 0 > nCamNo					|| MAX_CAMLINKSW_INPUT				<  nCamNo )						continue;
			if( 0 > nCamNoAll				|| MAX_CAMLINKSW_ALLINPUT			<  nCamNoAll )					continue;

			//// カメラ入力Ch
			m_Param.bCamInputCh[ nFpgaMasterNo + nFpgaNo ][ nCamNo ] = TRUE;

			//// カメラ選択ボードNo(=Master指定)
			bool bIsMaster = true;
			bIsMaster &= (CH_SELECT_MASTER == m_Param.linksw[ii].emChSelMode[jj]);				// Master指定

			for(int kk = 0; kk < MAX_CAMLINKSW_FPGA; kk++){
				bIsMaster &= ( 0 == m_Param.nCamSelBoardNo[ nFpgaMasterNo + kk ][ nCamNoAll ] );	// 同じカメラリンクスイッチの中に、既にMaster指定済みの出力chが在る
			}

			if( bIsMaster )
			{
				m_Param.nCamSelBoardNo[ ii ][ nCamNoAll ] = jj+1;
				nMasterCheck[nCamLinkSwNo][nCamNoAll] = 1;				// Master割りつき
			}

			// Slave設定？
			if( CH_SELECT_SLAVE == m_Param.linksw[ii].emChSelMode[jj] &&
				1 != nMasterCheck[nCamLinkSwNo][nCamNoAll])
			{
				nMasterCheck[nCamLinkSwNo][nCamNoAll] = -1;				// Slave割りつき
			}
		}
	}

	//// Masterが設定されていないchはいないか？
	CString sErrMsg = "";
	CString sWk;
	for(int ii=0; ii<NUM_CAMLINKSW; ii++){
		for(int jj=0; jj<MAX_CAMLINKSW_ALLINPUT; jj++){

			if( -1 == nMasterCheck[ii][jj] ){
				if( 0 != strlen(sErrMsg) )	sErrMsg+=",";
				sWk.Format("[%d台目 入力ch%d]", ii+1, jj+1);
				sErrMsg += sWk;
	}
		}
	}

	if( 0 != strlen(sErrMsg) )	LOG(em_ERR), "[%s] Master設定が割りついていないchあり！ %s", my_sThreadName, sErrMsg);

	//// 2017.02.27 Master/SlaveをDB指定 <<<<-------------------------------------------------
	return true;
}

// 2017.02.27 Master/SlaveをDB指定 ------------------------------------------------->>>>

//--------------------------------------
// Iniパラメータを反映
//--------------------------------------
void LinkswManager::GetParamIni()
{
	for( int ii=0; ii<NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; ii++ )
	{
		for( int jj=0; jj<MAX_CAMLINKSW_OUTPUT; jj++ )
		{
			int					nSelectCh   = m_nSelectCam[ ii ][ jj ];
			ENUM_CH_SELECT_MODE emChSelMode = CH_SELECT_NON;		// Master/Slave/未選択

			// 選択Chが0ならば、Iniでは設定してないので無視（DBが設定した値を上書きしないように）
			if( 0 == nSelectCh ) continue;

			
			// 選択chが負数ならばSlave指定
			if( -1 >= nSelectCh )	{ emChSelMode = CH_SELECT_SLAVE; }
			if( 0  <  nSelectCh )	{ emChSelMode = CH_SELECT_MASTER; }

			// これ以降chとして使う為に、正数値にする
			nSelectCh = abs(nSelectCh);
			if( 0 >= nSelectCh || MAX_CAMLINKSW_ALLINPUT < nSelectCh ) continue;


			m_Param.linksw[ ii ].nSelectCam[ jj ]  = nSelectCh;
			m_Param.linksw[ ii ].emChSelMode[ jj ] = emChSelMode;
			// 2020.05.07 対象カメラのみ設定 ---------->>>>
			m_Param.linksw[ ii ].bChSelCam[ jj ] = FALSE;
			// 2020.05.07 対象カメラのみ設定 <<<<----------

			LOG(em_INF), "[%s] カメラ選択設定(INI) 取得 [ID=%d, ボード%d=%d, %s]", my_sThreadName, ii, jj+1, m_Param.linksw[ ii ].nSelectCam[ jj ], GetChSelectModeName(m_Param.linksw[ ii ].emChSelMode[ jj ]));
		}
	}
									
}

//---------------------------------------
// 強制的にMaster設定
// ここでは、パラメータのMaster/Slave設定を
// 変更するのみ。実際に切り替えるには、
// この後にCalcParam()をコールすること
//---------------------------------------
// int	nId			: Master設定するchのボードID(0オリジン)
// int	nCh			: Master設定するch(0オリジン)
// int	nSelectCam	: Master設定するchのカメラ選択(1オリジン)
//---------------------------------------
void LinkswManager::SetForciblyMaster(const int nId, const int nCh, const int nSelectCam)
{
	int nFirstBoardNo = nId - (nId % MAX_CAMLINKSW_FPGA);	// 対象ボードがあるカメラリンクスイッチの先頭ボードID(0オリジン)

	for(int ii=nFirstBoardNo; ii < (nFirstBoardNo + MAX_CAMLINKSW_FPGA); ii++){

		for(int jj=0; jj < MAX_CAMLINKSW_OUTPUT; jj++){

			// Master設定の対象chならば、次へ
			if( nId == ii && nCh == jj)	continue;

			// Master設定するchとカメラ選択が同じならば、Slave設定
			if( nSelectCam == m_Param.linksw[ii].nSelectCam[jj] ){
				m_Param.linksw[ii].emChSelMode[jj] = CH_SELECT_SLAVE;
			}
		}
	}
}

// 2017.02.27 Master/SlaveをDB指定 <<<<-------------------------------------------------