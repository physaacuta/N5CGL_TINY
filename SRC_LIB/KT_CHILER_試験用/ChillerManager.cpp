#include "StdAfx.h"
#include "ChillerManager.h"

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
// int nTempThr 温度閾値 [度]
// int nTimerCycle 温度取り込み周期 [秒]
//------------------------------------------
ChillerManager::ChillerManager(char const* cName, int nTempThr, int nTimerCycle) :
ThreadManager(cName),
m_nTempThr(nTempThr),
m_nTimerCycle(nTimerCycle),
mcls_p232c(NULL),
m_bIsOpen(false),
m_bIsConnect(false),
m_emMainMode(MODE_NONE),
m_dNowTemp(0.0),
m_dSetTemp(0.0),
m_emUnten(MODE_STOP),
m_sAlmNo(""),
m_bLimitTemp(true),
m_bAlmMachine(true)
{
	//// 自分の設定キーを決定
	m_cMySession.Format("%s_%s", KT_CHILER, cName );

	//// チラーの型式を取得
	m_nChilerType = GetPrivateProfileInt(m_cMySession, "CHILER_TYPE",	0, TASKINI_NAME);			// チラー型式
	
	//// 機器IDを取得
	m_nIdConnect= GetPrivateProfileInt(m_cMySession, "CHILER_CONNECT_ID",	0, TASKINI_NAME);		// 回線状態
	m_nIdState	= GetPrivateProfileInt(m_cMySession, "CHILER_STATE_ID",		0, TASKINI_NAME);		// 運転状態	
	m_nIdErr	= GetPrivateProfileInt(m_cMySession, "CHILER_ERR_ID",		0, TASKINI_NAME);		// 異常状態
	m_nIdTemp	= GetPrivateProfileInt(m_cMySession, "CHILER_TEMP_ID",		0, TASKINI_NAME);		// 温度
	m_nIdTempSt	= GetPrivateProfileInt(m_cMySession, "CHILER_TEMP_ST_ID",	0, TASKINI_NAME);		// 温度状態

	//// ハンドル生成
	m_evRecv	  = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_tmReTry	  = CreateWaitableTimer(NULL, FALSE, NULL);
	m_tmCycleExec = CreateWaitableTimer(NULL, FALSE, NULL);
	m_tmSendAnser = CreateWaitableTimer(NULL, FALSE, NULL);
}

//------------------------------------------
// デストラクタ
//------------------------------------------
ChillerManager::~ChillerManager(void)
{
	//// ハンドル解放
	CloseHandle(m_evRecv);
	CloseHandle(m_tmReTry);
	CloseHandle(m_tmCycleExec);
	CloseHandle(m_tmSendAnser);

	// インスタンス解放
	SAFE_DELETE(mcls_p232c);
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int ChillerManager::ThreadFirst()
{
	//// RS-232C 初期接続
	if( ! RS_Open(true) ) {
		RS_Close();
	}

	//// イベント設定
	HANDLE hArray[] = { m_evRecv,
						m_tmReTry,
						m_tmSendAnser,
						m_tmCycleExec,
						mque_SendItem.g_evSem
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int ChillerManager::ThreadLast()
{
	RS_Close();
	return 0;
}

//------------------------------------------
// スレッドのWaitFor待ちの直前処理  
// int nDefWaitNum 元の数
// int* nNowWaitNum 変更後の数
//------------------------------------------
void ChillerManager::ThreadWaitFor(int nDefWaitNum, int* nNowWaitNum)
{
	// 受信中は、送信要求を受け付けないための処置
	if( MODE_RECV_EXEC == m_emMainMode )*nNowWaitNum = nDefWaitNum-1;
	else								*nNowWaitNum = nDefWaitNum;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void ChillerManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_RECV = 0,					// 受信完了
			EV_TIME_RETRY,					// 回線オープンリトライ
			EV_TIME_OUT,					// 応答アンサーNG
			EV_TIME_SATE_CHECK,				// 定周期チェック
			EV_QUE,							// 送信依頼キュー
	};


	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_RECV: 								// 受信完了
		if( -1 == RecvEnd() ) {
			RS_Close();		// 受信異常により切断
		}
		return;

//-----------------------------------------------------------------------------------------------
	case EV_TIME_RETRY:						// 回線オープンリトライ
		LOG(em_ERR), "[%s] 回線オープンリトライ", my_sThreadName);
		if( ! RS_Open(false) ){
			RS_Close();
		}
		return;

//-----------------------------------------------------------------------------------------------
	case EV_TIME_OUT:						// 応答アンサータイムアウト
		LOG(em_ERR), "[%s] アンサータイムアウト [%s]", my_sThreadName, GetSendCommandName(GetSendCommand((char*)&m_InfSend.cBuf[3])));
		RS_Close();
		break;

//-----------------------------------------------------------------------------------------------
	// 一回目のやりとりが正常に行われるまで定周期確認は動作しない
	case EV_TIME_SATE_CHECK:				// ステータス定周期チェック
		AddQue_SendCommand(SEND_CHECK_NOW_TEMP);	// 液温測定値
		AddQue_SendCommand(SEND_CHECK_SET_TEMP);	// 液温設定値
		AddQue_SendCommand(SEND_CHECK_MODE);		// 運転状態
		AddQue_SendCommand(SEND_CHECK_ALM);			// 警報信号

		return;

//-----------------------------------------------------------------------------------------------
	case EV_QUE: 							// 送信依頼キュー
		if(true) {
			// 送信データ取り出し
			INF_SEND* pDeli = mque_SendItem.GetFromHead(); 
			memcpy(&m_InfSend, pDeli, sizeof(m_InfSend));
			delete pDeli;

			// 送信
			if( ! SendStart() ){
				RS_Close();
			}
		}
		break;

//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
	return;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 送受信処理

//------------------------------------------
// キューに溜まっている送信依頼を全て削除
// 戻り値 復帰情報
//------------------------------------------
void ChillerManager::DeleteAllQue()
{
	INF_SEND* pDeli = NULL;

	while(true) {
		pDeli = mque_SendItem.GetFromHead(0);
		if( pDeli == NULL ) break;
		
		// 解放
		delete pDeli;
	}
}

//------------------------------------------
// 送信コマンド
// EM_CHILER_SEND_COMMAND em 送信コマンド
// 戻り値 復帰情報
//------------------------------------------
bool ChillerManager::AddQue_SendCommand(EM_CHILER_SEND_COMMAND em)
{
	// 生成
	INF_SEND* pDeli = new INF_SEND;
	memset(pDeli, 0x00, sizeof(INF_SEND));

	pDeli->emChilerCommand = em;
	pDeli->nSize = 6;

	// コマンドセット

	// デバイスNo
	pDeli->cBuf[0] = 0x04;		// EOT
	pDeli->cBuf[1] = 0x30;		// デバイスアドレス
	pDeli->cBuf[2] = 0x30;		// 〃

	switch( em ) {
	// [M1] 液温測定値
	case SEND_CHECK_NOW_TEMP:
		pDeli->cBuf[3] = 'M';
		pDeli->cBuf[4] = '1';
		break;

	// [S1] 液温設定値
	case SEND_CHECK_SET_TEMP:
		pDeli->cBuf[3] = 'S';
		pDeli->cBuf[4] = '1';
		break;

	// [JO] 運転状態
	case SEND_CHECK_MODE:
		pDeli->cBuf[3] = 'J';
		pDeli->cBuf[4] = 'O';
		break;

	// [ER] 警報信号
	case RECV_CHECK_ALM:
		pDeli->cBuf[3] = 'E';
		pDeli->cBuf[4] = 'R';
		break;
	}

	pDeli->cBuf[5] = 0x05;

	// 追加
	if( ! mque_SendItem.AddToTail(pDeli, 20, 20) ) {		// 絶対登録
		// 絶対登録も無理・・・
		LOG(em_ERR), "[%s] 送信依頼キューフル [%s]", my_sThreadName, GetSendCommandName(em));
		syslog(SYSNO_QUEFULL_ERR, "[%s:%s]", my_sThreadName, GetSendCommandName(em));
	
		// 領域解放
		delete pDeli;
		return false;
	}

	return true;
}

//------------------------------------------
// 送信開始
// 戻り値 復帰情報
//------------------------------------------
bool ChillerManager::SendStart()
{
	// 対応する受信コマンドがある場合のみ 
	m_emMainMode = MODE_RECV_EXEC;			// 受信コマンド待ちにする

	// 送信
	return RS_Send();
}

//------------------------------------------
// 受信完了
// 戻り値 0:引き続き受信  -1:異常(切断してね) 1:コマンドに対するアンサーを受信
//------------------------------------------
int ChillerManager::RecvEnd()
{
	bool retc;

	// コマンド特定
	EM_CHILER_RECV_COMMAND  emRecv = GetRecvCommand((char*)&m_InfRecv.cBuf[1]);
	EM_CHILER_SEND_COMMAND  emSend = GetSendCommand((char*)&m_InfSend.cBuf[3]);

//*/ ◆
	LOG(em_INF), "[%s] 受信 [前回送信=%s] [%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X]", 
		my_sThreadName,
		GetSendCommandName(emSend),
		m_InfRecv.cBuf[0],
		m_InfRecv.cBuf[1],
		m_InfRecv.cBuf[2],
		m_InfRecv.cBuf[3],
		m_InfRecv.cBuf[4],
		m_InfRecv.cBuf[5],
		m_InfRecv.cBuf[6],
		m_InfRecv.cBuf[7],
		m_InfRecv.cBuf[8],
		m_InfRecv.cBuf[9],
		m_InfRecv.cBuf[10],
		m_InfRecv.cBuf[11],
		m_InfRecv.cBuf[12],
		m_InfRecv.cBuf[13],
		m_InfRecv.cBuf[14]);
//*/

	// ///////////////////////////////////////////////////////////////////
	// 前処理
	//
	if( RECV_CHECK_NOW_TEMP == emRecv || 
		RECV_CHECK_SET_TEMP == emRecv || 
		RECV_CHECK_MODE		== emRecv || 
		RECV_CHECK_ALM		== emRecv) {

		// データ部がある受信コマンドを受信
		if( 3 == m_InfRecv.nNow ) {
			//受信コマンドの種別で、データ部の長さが初めて分かるので、全体長さを再定義
			//まだ受信が全部完了していない為、引き続き受信
			if (RECV_CHECK_MODE == emRecv) {
				m_InfRecv.nSize = 6;
			} else if (RECV_CHECK_NOW_TEMP == emRecv || RECV_CHECK_SET_TEMP == emRecv) {
				m_InfRecv.nSize = 13;
			} else if (RECV_CHECK_ALM == emRecv) {
				if(0 == m_nChilerType)	m_InfRecv.nSize = 8;
				else					m_InfRecv.nSize = 9;
			}

			retc = RS_Recv(m_InfRecv.nNow, m_InfRecv.nSize - m_InfRecv.nNow);
			return retc ? 0 : -1;
		}
	}

	// ///////////////////////////////////////////////////////////////////
	// １コマンド受信完了時
	//
	//LOG(em_INF), "[%s] コマンド受信 [%s] [size=%d,%.2s] [前回送信=%s]",
	//	my_sThreadName, GetRecvCommandName(emRecv), m_InfRecv.nSize, &m_InfRecv.cBuf[1], GetSendCommandName(emSend) );

	// 回線接続認識 (何か受信したらOK)
	if( ! m_bIsConnect) {
		LOG(em_MSG), "[%s] チラー装置装置 準備完了", my_sThreadName);
		syslog(SYSNO_232C_OPEN, "[%s]", my_sThreadName);

		m_bIsConnect = true;
		KizuFunction::SetStatus(m_nIdConnect, true);		

		// 次回からの周期タイマースタート
		__int64 ts = (__int64)(m_nTimerCycle*1000)* -10000;	// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
		SetWaitableTimer(m_tmCycleExec, (LARGE_INTEGER *)&ts, m_nTimerCycle*1000, NULL, NULL, FALSE);
	}

	// コマンドにより振り分け
	if( RECV_CHECK_NON == emRecv) {							// 不明なら異常としておく
		LOG(em_ERR), "[%s] 不明コマンド [size=%d,%s]", my_sThreadName, m_InfRecv.nSize, m_InfRecv.cBuf);
		syslog(SYSNO_232C_SEND_ERR, "[%s:size=%d:%s]", my_sThreadName, m_InfRecv.nSize, m_InfRecv.cBuf);
		return -1;
	}
	
	// ///////////////////////////////////////////////////////////////////
	// ここまで来たら、受信コマンド
	//
	CString  wk;
	if(MODE_RECV_EXEC != m_emMainMode) { LOG(em_INF), "いきなり受信コマンドが送られてきた！");}

	// コマンドにより処理分け
	switch( emRecv ) {
		// -------------------------------------------------------------
		case RECV_CHECK_NOW_TEMP:												// 液温測定値
			if (true) {
				char* sValue;
				if( 0 == m_nChilerType ) {
					sValue = new char[6];
					sprintf(sValue, "%.5s", (char*)&m_InfRecv.cBuf[6]);
					m_dNowTemp = atof(sValue);
				}
				else {
					sValue = new char[7];
					sprintf(sValue, "%.6s", (char*)&m_InfRecv.cBuf[5]);
					m_dNowTemp = atof(sValue);
				}
				delete sValue;

				// 液温測定値格納
				if( (double)m_nTempThr <= m_dNowTemp) {
					if( m_bLimitTemp ) {
						LOG(em_ERR), "[%s] チラー温度上限オーバー %2.2f℃", my_sThreadName, m_dNowTemp);
						if(0!=m_nIdTempSt) KizuFunction::SetStatus(m_nIdTempSt, false);
					}
					m_bLimitTemp = false;
				} else {
					if( ! m_bLimitTemp ) {
						LOG(em_MSG), "[%s] チラー温度正常復帰 %2.2f℃", my_sThreadName, m_dNowTemp);
						if(0!=m_nIdTempSt) KizuFunction::SetStatus(m_nIdTempSt, true);
					}
					m_bLimitTemp = true;
				}
				if(0!=m_nIdTemp) KizuFunction::SetStatus(m_nIdTemp, (int)(m_dNowTemp), false);
			}
			break;

		// -------------------------------------------------------------
		case RECV_CHECK_SET_TEMP:
			if (true) {
				char* sValue;
				if( 0 == m_nChilerType ) {
					sValue = new char[6];
					sprintf(sValue, "%.5s", (char*)&m_InfRecv.cBuf[6]);
					m_dSetTemp = atof(sValue);
				}
				else {
					sValue = new char[7];
					sprintf(sValue, "%.6s", (char*)&m_InfRecv.cBuf[5]);
					m_dSetTemp = atof(sValue);
				}
				delete sValue;
			}
			break;

			// -------------------------------------------------------------
		case RECV_CHECK_MODE:
			if (true) {
				CString  wk;
				int nVal = atoi((char*)&m_InfRecv.cBuf[3]);

				if (m_emUnten != MODE_COOL && (EM_CHILER_UNTENMODE)nVal == MODE_COOL) {
					LOG(em_MSG), "[%s] 運転中復旧", my_sThreadName);
					if(0!=m_nIdState) KizuFunction::SetStatus(m_nIdState, true);
				} else if (m_emUnten == MODE_COOL && (EM_CHILER_UNTENMODE)nVal != MODE_COOL) {
					LOG(em_MSG), "[%s] ポンプ停止", my_sThreadName);
					if(0!=m_nIdState) KizuFunction::SetStatus(m_nIdState, false);
				}

				m_emUnten = (EM_CHILER_UNTENMODE)nVal;
			}
			break;

			// -------------------------------------------------------------
		case RECV_CHECK_ALM:
			if (true) {
				if( 0 == m_nChilerType ) {
					m_sAlmNo.Format("%.3s", (char*)&m_InfRecv.cBuf[3]);
				}
				else {
					m_sAlmNo.Format("%.4s", (char*)&m_InfRecv.cBuf[3]);
				}

				if (m_sAlmNo != "    ") {
					if (m_bAlmMachine) {
						CString wk;
						LOG(em_ERR), "[%s] チラー装置異常 Alarm[%s]", my_sThreadName, m_sAlmNo);
						wk.Format("[ErrID:%s]", m_sAlmNo);
						if(0!=m_nIdErr) KizuFunction::SetStatus(m_nIdErr, false, true, wk);
					}
					m_bAlmMachine = false;
				} else {
					if (!m_bAlmMachine) {
						LOG(em_MSG), "[%s] チラー装置異常復帰", my_sThreadName);
						if(0!=m_nIdErr) KizuFunction::SetStatus(m_nIdErr, true);
					}
					m_bAlmMachine = true;
				}
			}

		default :
			break;
	}

	// 後始末
	m_emMainMode = MODE_WAIT;
	CancelWaitableTimer(m_tmSendAnser);

	// 受信完了後少し待つ(チラー装置の仕様) (用は、受信したら速攻で次伝文を送らない)
	Sleep(200);

	// 次受信
	retc = RS_Recv();
	return retc ? 1 : -1;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RS-232C制御

//------------------------------------------
// 232C オープン
// bool bLogDsp false:ログ出力無し
//------------------------------------------
bool ChillerManager::RS_Open(bool bLogDsp) 
{
	// Rs232cBaseインスタンス生成
	try{
		mcls_p232c = new Rs232cBase(m_cMySession, TASKINI_NAME, GetTid(), PARAM_232C_RECVBUF_SIZE, PARAM_232C_SENDBUF_SIZE, WM_USER, true);
	} catch(int code) {
		mcls_p232c = NULL;
		if(bLogDsp) {
			LOG(em_ERR), "[%s] 232Cオープンエラー(err_code = %d)", my_sThreadName, code);
			syslog(SYSNO_232C_NEW_ERR, "[%s err_code = %d]", my_sThreadName, code);
		}
		return false;
	}
	
	// ステータス初期化
	memset(&m_InfRecv, 0x00, sizeof(m_InfRecv));
	memset(&m_InfSend, 0x00, sizeof(m_InfSend));
	m_bIsOpen = true;			// とりあえずオープンは出来ている
	m_bIsConnect = false;		// まだ、チラー装置と接続できるかは不明
	m_emMainMode = MODE_NONE;

	// 受信開始
	//if( ! RS_Recv(0, 3, 13) ) return false;
	if( ! RS_Recv() ) return false;

	AddQue_SendCommand(SEND_CHECK_NOW_TEMP);	// 液温測定値
	AddQue_SendCommand(SEND_CHECK_SET_TEMP);	// 液温設定値
	AddQue_SendCommand(SEND_CHECK_MODE);		// 運転状態

	return true;
}

//------------------------------------------
// 232C クローズ
//------------------------------------------
void ChillerManager::RS_Close() 
{
	// リトライタイマ以外はすべて停止
	CancelWaitableTimer(m_evRecv);
	CancelWaitableTimer(m_tmCycleExec);
	CancelWaitableTimer(m_tmSendAnser);

	// クローズ
	if(NULL != mcls_p232c) {
		SAFE_DELETE(mcls_p232c);
	}

	//// リトライのたびにログ出力されるの禁止
	// 正常から異常に変わった場合のみ
	if(m_bIsConnect) {
		LOG(em_MSG), "[%s] 232Cクローズ", my_sThreadName);
		syslog(SYSNO_232C_CLOSE, "[%s]", my_sThreadName);
		KizuFunction::SetStatus(m_nIdConnect, false);		
	}

	// ステータス初期化
	memset(&m_InfRecv, 0x00, sizeof(m_InfRecv));
	memset(&m_InfSend, 0x00, sizeof(m_InfSend));
	m_bIsOpen = false;
	m_bIsConnect = false;
	m_emMainMode = MODE_NONE;

	// 送信依頼のキューがあれば削除しておく
	DeleteAllQue();

	// リトライタイマ セット
	__int64 ts = PARAM_TIMEOUT_RETRY * -10000; ;	// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_tmReTry,  (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE);
}

//------------------------------------------
// 232C 非同期 受信
// int pos セットするバッファ開始位置 (0オリジン)
// int size 受信サイズ
// int total 全部のサイズ
//------------------------------------------
bool ChillerManager::RS_Recv(int pos, int size, int total) 
{
	if(NULL == mcls_p232c) return true;

	// 受信開始処理
	if(0 == pos) {
		memset(&m_InfRecv, 0x00, sizeof(m_InfRecv));
		m_InfRecv.nNow = 0;
		m_InfRecv.nSize = total;
	}

	// 途中での変更も受付る為
	if(1 != total) {
		m_InfRecv.nSize = total;
	}

	// 指定バイトづつ読込
	int iRetc = mcls_p232c->Recv(&m_InfRecv.cBuf[pos], size, m_evRecv );
	if(0 != iRetc) {
		// 受信エラー
		LOG(em_ERR), "[%s] 232C受信エラー(err_code = %d)", my_sThreadName, iRetc);
		syslog(SYSNO_232C_RECV_ERR, "[%s err_code = %d]", my_sThreadName, iRetc);
		return false;
	}

	// 事後処理
	m_InfRecv.nNow += size;			// ここまでは、受信させる
	return true;
}

//------------------------------------------
// 232C 同期 送信
//------------------------------------------
bool ChillerManager::RS_Send() 
{
	if(NULL == mcls_p232c) return true;

	LOG(em_INF), "[%s] コマンド送信 [%s] [%02X%02X%02X%02X%02X%02X]", my_sThreadName, GetSendCommandName(m_InfSend.emChilerCommand),
		m_InfSend.cBuf[0], m_InfSend.cBuf[1], m_InfSend.cBuf[2], m_InfSend.cBuf[3], m_InfSend.cBuf[4], m_InfSend.cBuf[5]);
	
	//	syslog(217,"[%s:%s]", my_sThreadName, GetSendCommandName(m_InfSend.emChilerCommand) );

	int iRetc = mcls_p232c->Send(m_InfSend.cBuf, m_InfSend.nSize);
	if(0 != iRetc) {
		// 送信エラー
		LOG(em_ERR), "[%s] [%s] 232C送信エラー(err_code = %d)", my_sThreadName, GetSendCommandName(m_InfSend.emChilerCommand), iRetc);
		syslog(SYSNO_232C_SEND_ERR, "[%s err_code = %d]", my_sThreadName, iRetc);
		return false;
	}

	// 応答アンサータイマー
	__int64 ts = PARAM_TIMEOUT_ANSER * -10000;	// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_tmSendAnser,  (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE);
	return true;
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 共通処理

//------------------------------------------
// 送信コマンド名称
// EM_CHILER_SEND_COMMAND em コマンド
// 戻り値 名称
//------------------------------------------
CString ChillerManager::GetSendCommandName(EM_CHILER_SEND_COMMAND em)
{
	CString wk;
	switch( em ) {
		case SEND_CHECK_NOW_TEMP:
			wk = "液温測定値";
			break;
		case SEND_CHECK_SET_TEMP:
			wk = "液温設定値";
			break;
		case SEND_CHECK_MODE:
			wk = "運転状態";
			break;
		case SEND_CHECK_ALM:
			wk = "警報信号";
			break;
		default :
			wk = "モード異常";
			break;
	}
	return wk;
}

//------------------------------------------
// 受信コマンド名称
// EM_CHILER_SEND_COMMAND em コマンド
// 戻り値 名称
//------------------------------------------
CString ChillerManager::GetRecvCommandName(EM_CHILER_RECV_COMMAND em)
{
	CString wk;
	switch( em ) {
		case RECV_CHECK_NOW_TEMP:
			wk = "液温測定値";
			break;
		case RECV_CHECK_SET_TEMP:
			wk = "液温設定値";
			break;
		case RECV_CHECK_MODE:
			wk = "運転状態";
			break;
		case RECV_CHECK_ALM:
			wk = "警報信号";
			break;
		default :
			wk = "モード異常";
			break;
	}
	return wk;
}

//------------------------------------------
// 送信コードの特定
// char code 先頭1文字
// 戻り値 受信コマンド
//------------------------------------------
ChillerManager::EM_CHILER_SEND_COMMAND ChillerManager::GetSendCommand(char* code)
{
	if (0 == memcmp(code, "M1", 2)) return SEND_CHECK_NOW_TEMP;
	if (0 == memcmp(code, "S1", 2)) return SEND_CHECK_SET_TEMP;
	if (0 == memcmp(code, "JO", 2)) return SEND_CHECK_MODE;
	if (0 == memcmp(code, "ER", 2)) return SEND_CHECK_ALM;

	return SEND_CHECK_NON;
}

//------------------------------------------
// 受信コードの特定
// char code 先頭1文字
// 戻り値 受信コマンド
//------------------------------------------
ChillerManager::EM_CHILER_RECV_COMMAND ChillerManager::GetRecvCommand(char* code)
{
	if (0 == memcmp(code, "M1", 2)) return RECV_CHECK_NOW_TEMP;
	if (0 == memcmp(code, "S1", 2)) return RECV_CHECK_SET_TEMP;
	if (0 == memcmp(code, "JO", 2)) return RECV_CHECK_MODE;
	if (0 == memcmp(code, "ER", 2)) return RECV_CHECK_ALM;

	return RECV_CHECK_NON;
}

//------------------------------------------
// メインモード名称
// EM_LED_MAINMODE em モード
// 戻り値 名称
//------------------------------------------
CString ChillerManager::GetMainModeName(EM_CHILER_MAINMODE em)
{
	CString wk;
	switch( em ) {
		case MODE_NONE:
			wk = "切断中";
			break;
		case MODE_WAIT:
			wk = "待機";
			break;
		case MODE_RECV_EXEC:
			wk = "応答アンサー待ち";
			break;
		case MODE_RETRY:
			wk = "オープンリトライ中";
			break;
		default :
			wk = "モード異常";
			break;
	}
	return wk;
}

//------------------------------------------
// チラー装置運転状態
// EM_CHILER_UNTENMODE em モード
// 戻り値 名称
//------------------------------------------
CString ChillerManager::GetUntenName(EM_CHILER_UNTENMODE em)
{
	CString wk;
	switch( em ) {
		case MODE_STOP:
			wk = "停止";
			break;
		case MODE_COOL:
			wk = "運転";
			break;
		case MODE_POMP:
			wk = "ポンプ単独運転";
			break;
		default :
			wk = "モード異常";
			break;
	}
	return wk;
}
