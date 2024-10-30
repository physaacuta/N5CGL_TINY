#include "StdAfx.h"
#include "LedManager.h"

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
// int nId		     ID (0オリジン) (セクションの何番目)
// int nBlock        配光ブロック数
// int nTempUnit     LED温度計測定点数
// int nTempThr      温度閾値 [度]
// int nTempThrDown  強制OFF温度閾値 [度]
// int nTimerCycle   温度取り込み周期 [秒]
//------------------------------------------
LedManager::LedManager(char const* cName, int nId, int nBlock, int nTempUnit, int nTempThr, int nTempThrDown, int nTimerCycle) :
ThreadManager(cName),
m_nMyID(nId),
m_bIsConnect(false),
m_emMainMode(MODE_NONE),
m_nNowScode(MAX_SCODE),					// 表面状態は、デフォルト表面状態とする
m_bSendStatus(true),

//m_nBlock(nBlock),						/// 2014.10.28 諸藤(高輝度LED対応)
//m_nTempUnit(nTempUnit),				/// 2014.10.28 諸藤(高輝度LED対応)
m_nTempThr(nTempThr),
m_nTempThrDown(nTempThrDown),
m_nTimerCycle(nTimerCycle)
{
	int      nWk;
	CString  sWk;
	CString  sSession;

	//// 自分の設定キーを決定
	m_cMySession.Format("%s_%s", KT_LEDMGR, cName );
	m_nMyMen		= GetPrivateProfileInt(m_cMySession, "MEN",		0, TASKINI_NAME);
	m_nChNum		= GetPrivateProfileInt(m_cMySession, "CH_NUM",	0, TASKINI_NAME);
	/// 2014.10.28 諸藤(高輝度LED対応)------------------------->>>>
	m_nChStart		= GetPrivateProfileInt(m_cMySession, "CH_START", 1, TASKINI_NAME);				// 先頭chの片面単位のID(未定義の場合、1)
	/// 2014.10.28 諸藤(高輝度LED対応)<<<<-------------------------
	_ASSERT(0!=m_nChNum);

	/// 2014.10.28 諸藤(高輝度LED対応)------------------------->>>>
	//// 使用するCHの決定
	memset(&m_bUsedCh, 0x00, sizeof(m_bUsedCh));
	int	nChCnt = 0;
	char cWk[10];
	for(int ii=0; ii<MAX_LED_CH; ii++){
		sSession.Format("%s_%s_CH%d", KT_LEDMGR, cName, ii+1);
		GetPrivateProfileSection(sSession, cWk, 10, TASKINI_NAME);

		if( 0 != strlen(cWk)){
			m_bUsedCh[ii] = true;
			nChCnt++;
		}
	}
	_ASSERT(nChCnt >= m_nChNum);

	/// 2018.12.06 諸藤(不具合修正)------------------------->>>>
	//for(int ii=0; ii<MAX_LED_CH-1; ii++){
	for(int ii=0; ii<MAX_LED_CH; ii++){
	/// 2014.10.28 諸藤(不具合修正)<<<<-------------------------
		// 未使用CHの場合は次へ
		if( !m_bUsedCh[ii] )	continue;

		sSession.Format("%s_%s_CH%d", KT_LEDMGR, cName, ii+1 );
		m_nBlock[ii]    = GetPrivateProfileInt(sSession, "BLOCK",     nBlock,    TASKINI_NAME);		// 配光ブロック数(未定義の場合、共通設定のブロック数)
		m_nMyIndex[ii]  = GetPrivateProfileInt(sSession, "INDEX",     ii+1,      TASKINI_NAME);		// 対応するIndex(未定義の場合、Ch番号)
		m_nTempUnit[ii] = GetPrivateProfileInt(sSession, "TEMP_UNIT", nTempUnit, TASKINI_NAME);		// LED温度計測点数(未定義の場合、共通設定のLED温度計測点数)
		nWk				= GetPrivateProfileInt(sSession, "LR",		  0,		 TASKINI_NAME);		// 照度補正値正展開フラグ(未定義の場合、正展開)
		m_bIsPositiveDeployment[ii] = (0 == nWk ? true : false);
	}
	/// 2014.10.28 諸藤(高輝度LED対応)<<<<-------------------------

	//// 機器IDを取得
	m_nIdConnect	= GetPrivateProfileInt(m_cMySession, "LED_CONNECT_ID",		0, TASKINI_NAME);		// 照明装置 通信状態
	for(int ii=0; ii<MAX_LED_CH; ii++) {
		// 未使用CHの場合は次へ
		if( !m_bUsedCh[ii] )	continue;

		sSession.Format("%s_%s_CH%d", KT_LEDMGR, cName, ii+1 );

		m_nIdBreak[ii]		= GetPrivateProfileInt(sSession, "LED_BREAK_ID",	0, TASKINI_NAME);		// 断線	
		m_nIdHotWar[ii]		= GetPrivateProfileInt(sSession, "LED_HOTWAR_ID",	0, TASKINI_NAME);		// 加熱検出
		m_nIdHotErr[ii]		= GetPrivateProfileInt(sSession, "LED_HOTERR_ID",	0, TASKINI_NAME);		// 加熱シャットダウン
		m_nIdOnOff[ii]		= GetPrivateProfileInt(sSession, "LED_ONOFF_ID",	0, TASKINI_NAME);		// LED点灯消灯状態
		m_nIdValNow[ii]		= GetPrivateProfileInt(sSession, "LED_VALNOW_ID",	0, TASKINI_NAME);		// 現在設定輝度値
		m_nIdTotalTime[ii]	= GetPrivateProfileInt(sSession, "LED_TOTALTIME_ID",0, TASKINI_NAME);		// 稼動累計時間
		for (int jj=0; jj<MAX_TEMP_UNIT; jj++) {
			sWk.Format("LED_TEMP_ST_%d_ID", jj+1);
			m_nIdTempSt[ii][jj]	= GetPrivateProfileInt(sSession, sWk,			0, TASKINI_NAME);		// 温度状態

			sWk.Format("LED_TEMP_%d_ID", jj+1);
			m_nIdTemp[ii][jj]	= GetPrivateProfileInt(sSession, sWk,			0, TASKINI_NAME);		// 温度
		}
	}
	

	//// 初期化
	for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
		mcls_pSock[ii]		= NULL;
		m_eStateMain[ii]	= SOCK_NONE;
		m_nNowEventNo[ii]	= 0;
	}
	for(int ii=0; ii<MAX_LED_CH; ii++) {
		m_nNowLedBase[ii]	= 0;
		for (int jj=0; jj<MAX_LED_BLOCK; jj++) {
			m_nNowLedHosei[ii][jj]	= 0;
		}
		m_nStateOnOff[ii]	= -1;
		m_nStateBreak[ii]	= -1;
		m_nStateTemp[ii]	= -1;

		for (int jj=0; jj<MAX_TEMP_UNIT; jj++) {
			m_nTemp[ii][jj]		= 0;
			m_nTempSts[ii][jj]	= -1;
		}
	}
	memset( m_cSockBuf, 0x00, sizeof(m_cSockBuf));

	//// ハンドル生成
	m_tmSendAnser	= CreateWaitableTimer(NULL, FALSE, NULL);
	m_tmSendOk		= CreateWaitableTimer(NULL, FALSE, NULL);
	m_tmStateCheck	= CreateWaitableTimer(NULL, FALSE, NULL);
	m_tmReCheck		= CreateWaitableTimer(NULL, FALSE, NULL);
	for(int ii=0; ii<MAX_LED_CH; ii++) m_tmAddTime[ii] = CreateWaitableTimer(NULL, FALSE, NULL);
}

//------------------------------------------
// デストラクタ
//------------------------------------------
LedManager::~LedManager(void)
{
	// ハンドル解放
	CloseHandle(m_tmSendAnser);
	CloseHandle(m_tmSendOk);
	CloseHandle(m_tmStateCheck);
	CloseHandle(m_tmReCheck);
	for(int ii=0; ii<MAX_LED_CH; ii++) CloseHandle(m_tmAddTime[ii]);
}


//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int LedManager::ThreadFirst()
{
	// TCP/IP 操作クラス初期化
	SockInit(); 			

	// パラメータ読み込み
	while(TRUE) {
		if( GetParam() ) break;
		Sleep(3000);
	}

	//// イベント設定
	HANDLE hArray[] = { mcls_pSock[PORT_SEND]->gque_Anser.g_evSem,
						m_tmSendAnser,
						m_tmSendOk,
						m_tmStateCheck,
						m_tmReCheck
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);

	//// 追加
	AddEventNum(MAX_LED_CH, m_tmAddTime);
	AddEventNum(1, &mque_SendItem.g_evSem);	
	return 0;
}


//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int LedManager::ThreadLast()
{
	// TCP/IP 操作クラス開放
	SockExit();
	return 0;
}
//------------------------------------------
// スレッドのWaitFor待ちの直前処理  
// int nDefWaitNum 元の数
// int* nNowWaitNum 変更後の数
//------------------------------------------
void LedManager::ThreadWaitFor(int nDefWaitNum, int* nNowWaitNum)
{
	// 受信中は、送信要求を受け付けないための処置
	if( SOCK_EXEC == m_eStateMain[ID_SEND] )		*nNowWaitNum = nDefWaitNum-1;
	else											*nNowWaitNum = nDefWaitNum;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void LedManager::ThreadEvent(int nEventNo)
{
	BaseTcpIp::DELIVERY_ANS*	queAns  = NULL;			// TCP/IPクラス 操作結果

	////// シグナルの条件
	enum {	EV_SOCK_SEND_ANS = 0,			// 送信ポート 操作結果
			EV_TIME_SEND_ANSER,				// 送信伝文待ちタイマー タイムアップ
			EV_TIME_SEND_OK,				// 送信許可待ちタイマー タイムアップ
			EV_TIME_SATE_CHECK,				// ステータス定周期チェック
			EV_RECHECK,						// 再チェック タイムアップ
			EV_ADDTIME						// 点灯累計時間加算
	};


	////// シグナル条件分け
	switch (nEventNo) {

	//-----------------------------------------------------------------------------------------------
	case EV_SOCK_SEND_ANS:											// 送信ポート 操作結果

		queAns = mcls_pSock[PORT_SEND]->gque_Anser.GetFromHead(); 

		// 操作結果の処理
		ExecAnsQue(queAns->nMyID, queAns);

		// 取り出した操作結果を解放
		SAFE_DELETE(queAns->Data);
		SAFE_DELETE(queAns);
		break;

//-----------------------------------------------------------------------------------------------
	case EV_TIME_SEND_ANSER:				// 送信伝文待ちタイマー タイムアップ
		LOG(em_ERR), "[%s] 送信に対するアンサー伝文タイムアウト [前回送信=%s]", my_sThreadName, GetSendCommandName(m_InfSend.emLedCommand) );
//		syslog(SYSNO_SOCK_RECV_TIMEOUT, "[%s:%s]", my_sThreadName, GetSendCommandName(m_InfSend.emLedCommand));

		// 強制切断
		LOG(em_ERR), "[%s] UserClose 強制切断", my_sThreadName);
		if(m_bIsConnect) UserClose(-1);		// 繋がっている状態でタイムアウト
		else			 UserClose(0);
		return;

//-----------------------------------------------------------------------------------------------
	case EV_TIME_SEND_OK:				// 送信許可待ちタイマー タイムアップ
		if( SEND_READ_EEPROM == m_InfSend.emLedCommand ||
			SEND_WRITE_EEPROM == m_InfSend.emLedCommand ) {
			// 調光EEPROM読込 時には、送信許可待ち が帰ってこないため、正常認識とする
		} else {
			LOG(em_WAR), "[%s] 送信許可 未受信によりタイムアウト [前回送信=%s]", my_sThreadName, GetSendCommandName(m_InfSend.emLedCommand) );

			//// 110ms以上経過したら送信許可も待たなくてよいらしいため、異常にしない
			//syslog(SYSNO_SOCK_RECV_TIMEOUT, "[%s:%s]", my_sThreadName, GetSendCommandName(m_InfSend.emLedCommand));

			//// 強制切断
			//LOG(em_ERR), "[%s] UserClose 強制切断", my_sThreadName);
			//UserClose();
		}

		//// 次のコマンドを送信してもOK
		m_eStateMain[ID_SEND] = SOCK_IDLE;
		return;

//-----------------------------------------------------------------------------------------------
	// 一回目のやりとりが正常に行われるまで定周期確認は動作しない
	case EV_TIME_SATE_CHECK:				// ステータス定周期チェック
		LOG(em_MSG), "[%s] 定周期確認（断線確認、LED加熱確認、LED温度確認）", my_sThreadName);
		AddQue_SendCommand(SEND_CHECK_BREAK, 0);	// 断線確認
		AddQue_SendCommand(SEND_CHECK_HEATING, 0);	// LED加熱確認
		AddQue_SendCommand(SEND_CHECK_TEMP, 0);		// LED温度確認
		return;

//-----------------------------------------------------------------------------------------------
	case EV_RECHECK:						// 再チェック タイムアップ
		KizuFunction::SetStatus(m_nIdConnect, false);	// 機器状態に通知
		LOG(em_ERR), "[%s] 回線異常 ===================================>>>>", my_sThreadName);
		return;

//-----------------------------------------------------------------------------------------------
	default :
		if(EV_ADDTIME <= nEventNo && EV_ADDTIME+MAX_LED_CH-1 >= nEventNo) {		// 点灯累計時間加算
			int nCh = nEventNo - EV_ADDTIME + 1;
			AddTime_Exec(nCh);

//-----------------------------------------------------------------------------------------------
		} else {																// 送信依頼キュー

			if(true) {
				// 送信データ取り出し
				INF_SEND* pDeli = mque_SendItem.GetFromHead(); 
				memcpy(&m_InfSend, pDeli, sizeof(m_InfSend));
				delete pDeli;

				// 状態確認
				if( MODE_NONE == m_emMainMode) break;

				// 送信
				int retc;
				if( ! SendStart(&retc) ){
					LOG(em_ERR), "[%s] UserClose SendStart Err", my_sThreadName);
					UserClose();
				}
			}
		}
		break;
	}
}

//------------------------------------------
// TCP/IPクラス 操作結果の処理
// int id ポートID (よく使うから引数かしておく)
// BaseTcpIp::DELIVERY_ANS* que 操作結果
//------------------------------------------
void LedManager::ExecAnsQue(int id, BaseTcpIp::DELIVERY_ANS* que)
{
	switch(que->state) {

	//-----------------------------------------------------------------------------------------------
	case BaseTcpIp::ANS_CLOSE :										// クローズ通知
		if( KIZUSOCK_CLOSEKIND_SOCK == mcls_pSock[id]->GetCloseKind() ) {
			// 相手から切断
			LOG(em_ERR), "[%s][%s] 相手から切断要求！！", my_sThreadName, mcls_pSock[id]->GetMyName());
		}
		// 切断処理実行
		LOG(em_ERR), "[%s] UserClose 切断処理実行", my_sThreadName);
		UserClose();
		break;

	//-----------------------------------------------------------------------------------------------
	case BaseTcpIp::ANS_CONNECT :									// コネクト完了通知
		if( BaseTcpIp::STATE_NONE != mcls_pSock[PORT_SEND]->GetSockState() ) {

			// 整合性チェック (コネクト通知が来る前に、上記IF文で、両ポート正常になる可能性がある対策)
			if(m_bIsConnect) break;

			for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
				m_eStateMain[ii] = SOCK_IDLE;
				m_nNowEventNo[ii] = 0;
			}
			
			// 接続したので総合モードを待ち状態にする
			m_emMainMode = MODE_WAIT;
			AddQue_SendCommand(SEND_GET_BLOCK, 0);		// BLOCK数取得  ※1電源複数灯タイプ and 24ブロック以上の時には、必ず照明装置に対してブロック数取得を行うこと。これをしないと照明装置内部のブロック数が正常に認識されないため、配光取得とかするとおかしくなる

			AddQue_SendCommand(SEND_CHECK_BREAK, 0);	// 断線確認
			AddQue_SendCommand(SEND_CHECK_HEATING, 0);	// LED加熱確認
			AddQue_SendCommand(SEND_CHECK_TEMP, 0);		// LED温度確認
			AddQue_SendCommand(SEND_CHECK_ONOFF, 0);	// 点灯消灯確認
			AddQue_SendCommand(SEND_READ_EEPROM, 0);	// EEPROM読込

			// 送信ポートを 受信待ちにしておく。
			if( ! RecvStart( PORT_SEND, PARAM_COM_BUFSIZE, m_cSockBuf[id][BUF_RECV] )) {
				mcls_pSock[PORT_SEND]->UserClose();
				break;
			}
		}
		break;

	//-----------------------------------------------------------------------------------------------
	case BaseTcpIp::ANS_RECVEND :									// 非同期 受信完了通知
		if( 0 != que->Data->nAns) {
			// 正常受信完了じゃないので切断しよう
			LOG(em_ERR), "[%s] UserClose 正常受信完了じゃない Ans:%d", my_sThreadName, que->Data->nAns);
			UserClose();
		} else {
			// 受信完了時の処理を行う
			int mode = RecvEnd(id, que);

			// 処理わけ
			if(-1 == mode) {										// 異常
				// 異常だから切断
				LOG(em_ERR), "[%s] UserClose 異常だから切断", my_sThreadName);
				UserClose();

			} else if(9 == mode) {									// 送信許可を受信
				Sleep(110);											// すぐに送ると異常になるため、ちょっと待ってから送信
				m_eStateMain[ID_SEND] = SOCK_IDLE;

			} else if(0 == mode) {									// 引き続き受信
				// 引き続き受信の為、何もしない

			} else if(1 == mode) {									// 受信コマンド受信
				// コマンドに対する受信
				memset(&m_InfSend, 0x00, sizeof(m_InfSend));		// 送信コマンドを初期化しておく
			}
		}
		break;

	//-----------------------------------------------------------------------------------------------
	case BaseTcpIp::ANS_SENDEND :									// 送信完了通知
		// 非同期送信は 今回は、行わない (処理簡略化の為)
		break;
	
	//-----------------------------------------------------------------------------------------------
	default:			// ありえない
		LOG(em_ERR), "[%s] モード異常 [%d]", my_sThreadName, que->state);
		break;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 送受信処理

//------------------------------------------
// キューに溜まっている送信依頼を全て削除
// 戻り値 復帰情報
//------------------------------------------
void LedManager::DeleteAllQue()
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
// EM_LED_SEND_COMMAND em 送信コマンド
// int	nCh 対象チャンネル (0:全チャンネル  1～:指定チャンネルのみ)
// 戻り値 復帰情報
//------------------------------------------
bool LedManager::AddQue_SendCommand(EM_LED_SEND_COMMAND em, int nCh)
{
	int ii_s, ii_e;
	GetChLoop(nCh, ii_s, ii_e);

	//// 対象チャンネル数分 送信コマンド発行
	for(int ii=ii_s; ii<=ii_e; ii++) {
		//======================================================
		// 事前チェック

		if( !m_bUsedCh[ii-1] ) continue;	// 未使用CHの場合、送信しない

		if(SEND_EXEC_ON == em) {
			// 点灯指示のときに、温度異常の場合は点灯させない
			bool bTempStsNg = false;
			// ===>> 坂巻 2018.04.10 LED加熱シャットダウン時に同色異ChのLEDが消灯にならない不具合修正
			//for(int jj=0; jj<m_nTempUnit[ii-1]; jj++) { if(2 == m_nTempSts[ii-1][jj]) bTempStsNg = true; }
			//if( 2 == m_nStateTemp[ii-1] || bTempStsNg) {
			//	LOG(em_ERR), "[%s] 温度異常により点灯禁止 [CH=%d]", my_sThreadName, ii);
			//	continue;
			//}
			bool bHotErr = false;
			for(int kk=0; kk<MAX_LED_CH;kk++) {
				// 違うIndexの場合は無視する
				if(m_nMyIndex[ii-1] != m_nMyIndex[kk]) continue;
				for(int jj=0; jj<m_nTempUnit[kk]; jj++) { if(2 == m_nTempSts[kk][jj]) bTempStsNg = true; }

				if( 2 == m_nStateTemp[kk] || bTempStsNg) {
				LOG(em_ERR), "[%s] 温度異常により点灯禁止 [CH=%d]", my_sThreadName, ii);
					bHotErr = true;
					break;
				}
			}
			if(bHotErr) continue;
			// <<===
		}

		/// 2014.10.28 諸藤(高輝度LED対応)------------------------------------------------>>>>
		if( (SEND_CHECK_BREAK == em && 0 == m_nIdBreak[ii-1]) ||
			(SEND_CHECK_ONOFF == em && 0 == m_nIdOnOff[ii-1]) ||
			(SEND_CHECK_HEATING == em && 0 == m_nIdHotWar[ii-1]) ||
			(SEND_CHECK_HEATING == em && 0 == m_nIdHotErr[ii-1]) ) {
			LOG(em_INF), "[%s] 機器ID未定義の為ステータス要求停止 [CH=%d][%s]", my_sThreadName, ii, GetSendCommandName(em));
			continue;
		}
		if(SEND_CHECK_TEMP == em && 0 == m_nTempUnit[ii-1] ) {
			// 温度ユニットがついていない場合があるので、そのときにはログは出したくない
			continue;
		}
		/// 2014.10.28 諸藤(高輝度LED対応)<<<<------------------------------------------------


		//======================================================
		//// 生成
		INF_SEND* pDeli = new INF_SEND;
		memset(pDeli, 0x00, sizeof(INF_SEND));

		pDeli->emLedCommand = em;
		pDeli->nSize = PARAM_COM_BUFSIZE;
		pDeli->nCh	 = ii;

		//// コマンドセット

		// 送信コマンドのつぼ
		//   ・送信許可伝文を受け取らなければ、次シーケンスへ推移できない
		//   ・送信許可伝文の前に受信しなければならない伝文がある。次シーケンスへ推移できない
		//   　(bRecvUmuが、falseであれば許可伝文のみとなる)

		pDeli->cBuf[0] = 'R';
		switch( em ) {
			// 断線有無 (ステータス要求)
			case SEND_CHECK_BREAK:
				pDeli->cBuf[1] = 'a';
				pDeli->bRecvUmu = true;
				break;
			// 点灯消灯状態 (ステータス要求)
			case SEND_CHECK_ONOFF:
				pDeli->cBuf[1] = 'l';
				pDeli->bRecvUmu = true;
				break;
			// LED加熱状態 (ステータス要求)
			case SEND_CHECK_HEATING:
				pDeli->cBuf[1] = 'b';
				pDeli->bRecvUmu = true;
				break;
			// LED温度状態 (ステータス要求)
			case SEND_CHECK_TEMP:
				pDeli->cBuf[1] = 't';
				pDeli->bRecvUmu = true;
				break;
			// 点灯指示
			case SEND_EXEC_ON:
				pDeli->cBuf[1] = 'X';
				pDeli->bRecvUmu = true;
				break;
			// 消灯指示
			case SEND_EXEC_OFF:
				pDeli->cBuf[1] = 'Y';
				pDeli->bRecvUmu = true;
				break;
			// 調光データ送信 (RAMにセット)
			case SEND_SEND_VALUE:
				pDeli->cBuf[1] = 'S';
				pDeli->bRecvUmu = false;
				break;
			// RAMの調光データをEEPROM(ファイル)に書込
			case SEND_WRITE_EEPROM:
				pDeli->cBuf[1] = 'W';
				pDeli->bRecvUmu = false;
				break;
			// EEPROM(ファイル)の調光データを読込
			case SEND_READ_EEPROM:
				pDeli->cBuf[1] = 'M';
				pDeli->bRecvUmu = true;
				break;
			// RAMの調光データを読込
			case SEND_READ_RAM:
				pDeli->cBuf[1] = 'R';
				pDeli->bRecvUmu = true;
				break;
			// ブロック数取得
			case SEND_GET_BLOCK:
				pDeli->cBuf[1] = 'B';
				pDeli->bRecvUmu = true;
				break;
		}
		pDeli->cBuf[2] = 0x01;
		pDeli->cBuf[3] = 0x01 + ii-1;		// チャンネル
		pDeli->cBuf[4] = 0x00;

		// 追加
		if( ! mque_SendItem.AddToTail(pDeli, 20, 20) ) {		// 絶対登録
			// 絶対登録も無理・・・
			LOG(em_ERR), "[%s] 送信依頼キューフル [CH=%d] [%s]", my_sThreadName, ii, GetSendCommandName(em));
			syslog(SYSNO_QUEFULL_ERR, "[%s:CH=%d:%s]", my_sThreadName, ii, GetSendCommandName(em));
		
			// 領域解放
			delete pDeli;
			return false;
		}
	}
	return true;
}





//------------------------------------------
// マスター＋配光データセット
// int nCh 対象チャンネル (0:全チャンネル  1～:指定チャンネルのみ)
// int nLedBase 全体調光
// BYTE* nLedHosei 配光データ
// 戻り値 復帰情報
//------------------------------------------
bool LedManager::AddQue_SetLightMastVal(int nCh, BYTE nLedBase, BYTE* nLedHosei)
{
	bool retc = true;
	int ii_s, ii_e;
	GetChLoop(nCh, ii_s, ii_e);
	
	//// 送信
	for(int ii=ii_s; ii<=ii_e; ii++) {

		if( !m_bUsedCh[ii-1] ) continue;	// 未使用CHの場合、送信しない

		// 生成
		INF_SEND* pDeli = new INF_SEND;
		memset(pDeli, 0x00, sizeof(INF_SEND));

		pDeli->emLedCommand = SEND_SEND_VALUE;
		pDeli->nSize = PARAM_COM_BUFSIZE + 1 + m_nBlock[ii-1];	// 送信コマンド + 全体調光データ ＋ 補正データブロック数
		pDeli->nCh	 = ii;

		pDeli->bRecvUmu = false;
		pDeli->cBuf[0] = 'R';
		pDeli->cBuf[1] = 'S';
		pDeli->cBuf[2] = 0x01;
		pDeli->cBuf[3] = 0x01 + ii-1;		// チャンネル
		pDeli->cBuf[4] = 0x00;

		pDeli->cBuf[5] = nLedBase;
		memcpy(&pDeli->cBuf[6], nLedHosei, m_nBlock[ii-1]);

		// 追加
		if( ! mque_SendItem.AddToTail(pDeli, 20, 20) ) {		// 絶対登録
			// 絶対登録も無理・・・
			LOG(em_ERR), "[%s] 送信依頼キューフル [CH=%d] [%s]", my_sThreadName, ii, GetSendCommandName(SEND_SEND_VALUE));
			syslog(SYSNO_QUEFULL_ERR, "[%s:CH=%d:%s]", my_sThreadName, ii, GetSendCommandName(SEND_SEND_VALUE));
		
			// 領域解放
			delete pDeli;
			return false;
		}

		// 調光データを送信時は、必ずEPPROMに書き込む
		// 書き込んだのち、EPPROMのデータを取得してみる。で画面等に結果を反映させる
		retc &= AddQue_SendCommand(SEND_WRITE_EEPROM, ii);
		retc &= AddQue_SendCommand(SEND_READ_EEPROM, ii);
	}
	return retc;
}

//------------------------------------------
// マスター＋配光データセット (scode)
// int	nCh 対象チャンネル (0:全チャンネル  1～:指定チャンネルのみ)
// int scode (1オリジン)
// 戻り値 復帰情報
//------------------------------------------
bool LedManager::AddQue_SetLightMastVal(int nCh, int scode)
{
	// 全SCODEで無い場合は更新
	if( CH_SCODE_ALL != scode )	m_nNowScode = scode;	

	bool retc = true;
	int ii_s, ii_e;
	GetChLoop(nCh, ii_s, ii_e);
	
	//// 送信
	for(int ii=ii_s; ii<=ii_e; ii++) {

		if( !m_bUsedCh[ii-1] ) continue;	// 未使用CHの場合、送信しない

		retc &= AddQue_SetLightMastVal( ii, m_Param.nLedBase[m_nNowScode-1][ii-1], m_Param.nLedHosei[m_nNowScode-1][ii-1] );
	}
	return retc;
}

//------------------------------------------
// マスター＋配光データセット  (配光部は全部同一値)
// int	nCh 対象チャンネル (0:全チャンネル  1～:指定チャンネルのみ)
// int nLedBase 全体調光
// BYTE nLedHosei 配光データ (配光部は全部同一値)
// 戻り値 復帰情報
//------------------------------------------
bool LedManager::AddQue_SetLightMastVal(int nCh, BYTE nLedBase, BYTE nLedHosei)
{
	bool	retc = true;
	int ii_s, ii_e;
	GetChLoop(nCh, ii_s, ii_e);

	//// 対象チャンネル数分 送信コマンド発行
	for(int ii=ii_s; ii<=ii_e; ii++) {

		if( !m_bUsedCh[ii-1] ) continue;	// 未使用CHの場合、送信しない

		//// 配光データ
		BYTE  nWk[MAX_LED_BLOCK];
		for(int jj=0; jj<m_nBlock[ii-1]; jj++) nWk[jj] = nLedHosei;
		retc &= AddQue_SetLightMastVal( ii, nLedBase, nWk );
	}

	return retc;
}


//------------------------------------------
// マスター調光データセット
// int	nCh 対象チャンネル (0:全チャンネル  1～:指定チャンネルのみ)
// int nLedBase 全体調光
// 戻り値 復帰情報
//------------------------------------------
bool LedManager::AddQue_SetLightMaster(int nCh, int nLedBase)
{
	bool	retc = true;
	int ii_s, ii_e;
	GetChLoop(nCh, ii_s, ii_e);

	//// 対象チャンネル数分 送信コマンド発行
	for(int ii=ii_s; ii<=ii_e; ii++) {

		if( !m_bUsedCh[ii-1] ) continue;	// 未使用CHの場合、送信しない

		// 生成
		INF_SEND* pDeli = new INF_SEND;
		memset(pDeli, 0x00, sizeof(INF_SEND));

/*** 		// 3.2.1 修正 (なぜか RT は受け付けてくれないので、RSで送信して代用する)
		pDeli->emLedCommand = SEND_SEND_MASTER;
		pDeli->nSize = PARAM_COM_BUFSIZE + 1;	// 送信コマンド + 全体調光データ
		pDeli->nCh	 = ii;

		pDeli->bRecvUmu = false;
		pDeli->cBuf[0] = 'R';
		pDeli->cBuf[1] = 'T';
		pDeli->cBuf[2] = 0x01;
		pDeli->cBuf[3] = 0x01 + ii-1;		// チャンネル
		pDeli->cBuf[4] = 0x00;

		pDeli->cBuf[5] = nLedBase;
***/
		pDeli->emLedCommand = SEND_SEND_MASTER;
		pDeli->nSize = PARAM_COM_BUFSIZE + 1 + m_nBlock[ii-1];	// 送信コマンド + 全体調光データ ＋ 補正データブロック数
		pDeli->nCh	 = ii;

		pDeli->bRecvUmu = false;
		pDeli->cBuf[0] = 'R';
		pDeli->cBuf[1] = 'S';
		pDeli->cBuf[2] = 0x01;
		pDeli->cBuf[3] = 0x01 + ii-1;		// チャンネル
		pDeli->cBuf[4] = 0x00;

		pDeli->cBuf[5] = nLedBase;
		memcpy(&pDeli->cBuf[6], m_nNowLedHosei[ii-1], m_nBlock[ii-1]);


		// 追加
		if( ! mque_SendItem.AddToTail(pDeli, 20, 20) ) {		// 絶対登録
			// 絶対登録も無理・・・
			LOG(em_ERR), "[%s] 送信依頼キューフル [CH=%d] [%s]", my_sThreadName, ii, GetSendCommandName(SEND_SEND_MASTER));
			syslog(SYSNO_QUEFULL_ERR, "[%s:CH=%d:%s]", my_sThreadName, ii, GetSendCommandName(SEND_SEND_MASTER));
		
			// 領域解放
			delete pDeli;
			return false;
		}

		// 調光データを送信時は、必ずEPPROMに書き込む
		// 書き込んだのち、EPPROMのデータを取得してみる。で画面等に結果を反映させる
		retc &= AddQue_SendCommand(SEND_WRITE_EEPROM, ii);
		retc &= AddQue_SendCommand(SEND_READ_EEPROM, ii);
	}
	return retc;
}

//------------------------------------------
// 調光データセット (配光部は一括指定)
// int nCh 対象チャンネル (0:全チャンネル  1～:指定チャンネルのみ)
// BYTE* nLedHosei 配光データ
// 戻り値 復帰情報
//------------------------------------------
bool LedManager::AddQue_SetLightValue(int nCh, BYTE* nLedHosei)
{
	bool	retc = true;
	int ii_s, ii_e;
	GetChLoop(nCh, ii_s, ii_e);

	//// 対象チャンネル数分 送信コマンド発行
	for(int ii=ii_s; ii<=ii_e; ii++) {

		if( !m_bUsedCh[ii-1] ) continue;	// 未使用CHの場合、送信しない

		retc &= AddQue_SetLightMastVal(nCh, m_nNowLedBase[ii-1], nLedHosei);
	}

	return retc;
}

//------------------------------------------
// 配光データセット (配光部は全部同一値)
// int nCh 対象チャンネル (0:全チャンネル  1～:指定チャンネルのみ)
// BYTE nLedHosei 配光データ
// 戻り値 復帰情報
//------------------------------------------
bool LedManager::AddQue_SetLightValue(int nCh, BYTE nLedHosei)
{
	bool	retc = true;
	int ii_s, ii_e;
	GetChLoop(nCh, ii_s, ii_e);

	//// 対象チャンネル数分 送信コマンド発行
	for(int ii=ii_s; ii<=ii_e; ii++) {

		if( !m_bUsedCh[ii-1] ) continue;	// 未使用CHの場合、送信しない

		retc &= AddQue_SetLightMastVal(nCh, m_nNowLedBase[ii-1], nLedHosei);
	}

	return retc;
}

//------------------------------------------
// 調光データセット (配光部は個別指定)
// int nCh 対象チャンネル (0:全チャンネル  1～:指定チャンネルのみ)
// BYTE nLedHosei 配光データ
// 戻り値 復帰情報
//------------------------------------------
bool LedManager::AddQue_SetLightValue(int nCh, int nBlock, BYTE nLedHosei)
{
	bool	retc = true;
	int ii_s, ii_e;
	GetChLoop(nCh, ii_s, ii_e);

	BYTE  nWk[MAX_LED_BLOCK];		// 配光データ

	//// 対象チャンネル数分 送信コマンド発行
	for(int ii=ii_s; ii<=ii_e; ii++) {

		if( !m_bUsedCh[ii-1] ) continue;	// 未使用CHの場合、送信しない

		for(int jj=0; jj<m_nBlock[ii-1]; jj++) nWk[jj] = m_nNowLedHosei[ii-1][jj];	// まずは現在の値を変化なし項目としてセット
		nWk[nBlock] = nLedHosei;	// 変更有り部分のみ再セット


		retc &= AddQue_SetLightMastVal(nCh, m_nNowLedBase[ii-1], nWk);
	}

	return retc;
}

//------------------------------------------
// 送信開始
// int* retStat 送信依頼モード(戻り値)
// 戻り値 false:切断してね要求
//------------------------------------------
bool LedManager::SendStart(int* retStat)
{
	// 状態確認
	if( MODE_NONE == m_emMainMode) {
		//LOG(em_ERR), "[%s] ソケット切断中に送信依頼有り。依頼破棄[CH=%d][%d]", my_sThreadName, m_InfSend.nCh, m_InfSend.emLedCommand);
		return false;
	}
	
	
	// データコピー
	memcpy(&m_cSockBuf[PORT_SEND][BUF_SEND], m_InfSend.cBuf, m_InfSend.nSize);
	*retStat = m_InfSend.emLedCommand;	


	//// 同期送信
	LOG(em_INF), "[%s] コマンド送信 [CH=%d][%s] [%.2s]", my_sThreadName, m_InfSend.nCh, GetSendCommandName(m_InfSend.emLedCommand), m_InfSend.cBuf );
//◆	LOG(em_INF), "[%s] コマンド送信 [CH=%d][%s] [%.2s]", my_sThreadName, m_InfSend.nCh, GetSendCommandName(m_InfSend.emLedCommand), m_InfSend.cBuf );
	//syslog(213,"[%s:%s]", my_sThreadName, GetSendCommandName(m_InfSend.emLedCommand) );
	LOG(em_MSG), "   送信[%d][%s][%02X,%02X,%02X,%02X,%02X,%02X]", m_InfSend.nSize, m_InfSend.cBuf, m_InfSend.cBuf[0], m_InfSend.cBuf[1], m_InfSend.cBuf[2], m_InfSend.cBuf[3], m_InfSend.cBuf[4], m_InfSend.cBuf[5]);
 

	int iRetc = mcls_pSock[PORT_SEND]->LockSend( m_cSockBuf[PORT_SEND][BUF_SEND], m_InfSend.nSize, PARAM_RES_TIMEOUT );
	if(0 != iRetc) {
		LOG(em_ERR), "[%s][CH=%d][%s] TCP送信エラー(err_code = %d)", my_sThreadName, m_InfSend.nCh, GetSendCommandName(m_InfSend.emLedCommand), iRetc);
		syslog(SYSNO_SOCK_SEND_ERR, "[%s CH=%d err_code = %d]", my_sThreadName, m_InfSend.nCh, iRetc);
		return false;
	}


	// -------------------------------------
	__int64 ts;		// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)

	//// アンサーがあるもののみ、応答伝文待ちタイマーON	
	if (m_InfSend.bRecvUmu) {
		m_emMainMode		  = MODE_RECV_EXEC;				// 送信したので受信コマンド待ちにする
		ts = PARAM_ANSER_TIMEOUT * -10000;	// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
		SetWaitableTimer(m_tmSendAnser, (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE);
	} else {
		m_emMainMode		  = MODE_WAIT;					// アンサーなしなので、いきなり送信許可待ちにする
	}

	//// 送信許可待ちタイマー
	// 全伝文に送信許可待ちタイマーをセット
	if (m_InfSend.bRecvUmu)	ts = PARAM_SENDOK_TIMEOUT * -10000;
	else					ts = PARAM_SENDOK_NOWAIT_TIMEOUT * -10000;
	SetWaitableTimer(m_tmSendOk,  (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE);
	m_eStateMain[ID_SEND] = SOCK_EXEC;
	return true;
}

//------------------------------------------
// 受信完了
// int id ポートID
// BaseTcpIp::DELIVERY_ANS* que 操作結果
// 戻り値 0:引き続き受信  -1:異常(切断してね) 1:コマンドに対するアンサーを受信  9:送信許可を受信
//------------------------------------------
int LedManager::RecvEnd(int id, BaseTcpIp::DELIVERY_ANS* que)
{
	// コマンド特定
	EM_LED_RECV_COMMAND  emRecv = GetRecvCommand(m_cSockBuf[PORT_SEND][BUF_RECV][1]);
	EM_LED_SEND_COMMAND  emSend = GetSendCommand(m_cSockBuf[PORT_SEND][BUF_SEND][1]);


	// ///////////////////////////////////////////////////////////////////
	// 前処理
	//

	// 受信コマンドの長さに関係する為、先に特定
	m_InfRecv.nCh	= m_cSockBuf[PORT_SEND][BUF_RECV][3];	// チャンネル

	// データ部がある受信コマンドを受信
	// 受信コマンドの種別で、データ部の長さが初めて分かるので、全体長さを再定義
	if( RECV_LED_TEMP == emRecv) {
		if (PARAM_COM_BUFSIZE == que->Data->nSize) {
			// 残りを受信
			m_InfRecv.nSize = PARAM_COM_BUFSIZE + m_nTempUnit[m_InfRecv.nCh-1];		// 受信コマンド + LED温度格納点数
			if( ! RecvStart( id, m_nTempUnit[m_InfRecv.nCh-1], m_cSockBuf[id][BUF_RECV], PARAM_COM_BUFSIZE)) return -1;
			return 0;
		}
	} else 	if( RECV_READ_EEPROM == emRecv || RECV_READ_RAM == emRecv) {
		if (PARAM_COM_BUFSIZE == que->Data->nSize) {
			// 残りを受信
			m_InfRecv.nSize = PARAM_COM_BUFSIZE + 1 + m_nBlock[m_InfRecv.nCh-1]; // 受信コマンド + 全体調光データ ＋ 補正データブロック数
			if( ! RecvStart( id, m_InfRecv.nSize, m_cSockBuf[id][BUF_RECV], PARAM_COM_BUFSIZE)) return -1;
			return 0;
		}

	} else {
		m_InfRecv.nSize = que->Data->nSize;
	}

	memcpy(&m_InfRecv.cBuf, &m_cSockBuf[PORT_SEND][BUF_RECV], m_InfRecv.nSize);


	// ///////////////////////////////////////////////////////////////////
	// １コマンド受信完了時
	//
	LOG(em_INF), "[%s][CH=%d] コマンド受信 [%s] [size=%d,%.2s] [前回送信=%s]",
		my_sThreadName, m_InfRecv.nCh, GetRecvCommandName(emRecv), m_InfRecv.nSize, m_InfRecv.cBuf, GetSendCommandName(emSend) );
//◆	LOG(em_INF), "[%s][CH=%d] コマンド受信 [%s] [size=%d,%.2s] [前回送信=%s]",
//◆		my_sThreadName, m_InfRecv.nCh, GetRecvCommandName(emRecv), m_InfRecv.nSize, m_InfRecv.cBuf, GetSendCommandName(emSend) );
	LOG(em_MSG), "   受信[%d][%.5s][%02X,%02X,%02X,%02X,%02X,%02X]", m_InfRecv.nSize, m_InfRecv.cBuf, m_InfRecv.cBuf[0], m_InfRecv.cBuf[1], m_InfRecv.cBuf[2], m_InfRecv.cBuf[3], m_InfRecv.cBuf[4], m_InfRecv.cBuf[5]);

	// 回線接続認識 (何か受信したらOK)
	if( ! m_bIsConnect) {
		CancelWaitableTimer(m_tmReCheck);

		LOG(em_MSG), "[%s] 照明装置 準備完了", my_sThreadName);
		syslog(SYSNO_SOCK_CONNECT, "[%s]", my_sThreadName);

		m_bIsConnect = true;
		KizuFunction::SetStatus(m_nIdConnect, true);

		// 回線接続したら定周期開始
		__int64 ts = (__int64)(m_nTimerCycle*1000)* -10000;	// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
		SetWaitableTimer(m_tmStateCheck, (LARGE_INTEGER *)&ts, m_nTimerCycle*1000, NULL, NULL, FALSE);
	}

	// コマンドにより振り分け
	if( RECV_NON == emRecv) {								// 不明なら異常としておく
		LOG(em_ERR), "[%s] 不明コマンド [CH=%d,size=%d,%s]", my_sThreadName, m_InfRecv.nCh, m_InfRecv.nSize, m_InfRecv.cBuf);
		syslog(SYSNO_SOCK_RECV_ERR, "[%s:CH=%d:size=%d:%s]", my_sThreadName, m_InfRecv.nCh, m_InfRecv.nSize, m_InfRecv.cBuf);
		// ★ﾊﾞｲﾊﾟｽ 20131101
		//return -1;
		// ★やっぱり有効化 20160524  ※ホントに異常の場合、回線を切りたい
		return -1;

	} else if(RECV_NOTHING == emRecv) {						// 完全に無視するコマンド達
		return 0;

	} else if(RECV_NEXT_SEND_OK == emRecv) {				// 送信許可ならタイマーキャンセルして終わり
		// 送信許可タイマーキャンセル
		CancelWaitableTimer(m_tmSendOk);
		
		// 受信コマンド受付中にいきなり送信許可を受信。 (= シーケンスが狂っている)
		if(MODE_WAIT != m_emMainMode) {
			LOG(em_ERR), "[%s][CH=%d] 受信コマンド受付中にいきなり送信許可を受信", my_sThreadName, m_InfRecv.nCh);
			syslog(SYSNO_SOCK_RECV_ERR, "[%s:CH=%d]", my_sThreadName, m_InfRecv.nCh);
			//return -1;
		} else {
			LOG(em_MSG), "[%s:CH=%d] 送信許可受信", my_sThreadName, m_InfRecv.nCh);
//			LOG(em_MSG), "[%s:CH=%d] 送信許可受信", my_sThreadName, m_InfRecv.nCh);
		}

		// また最初から受信開始
		if( ! RecvStart( id, PARAM_COM_BUFSIZE, m_cSockBuf[id][BUF_RECV] )) {
			return -1;
		}

		return 9;
	}

	// ///////////////////////////////////////////////////////////////////
	// ここまで来たら、受信コマンド
	//
	
	//// アンサー待ちタイマーキャンセル
	CancelWaitableTimer(m_tmSendAnser);
	CancelWaitableTimer(m_tmAddTime);

	//// 処理わけ
	CString  wk;
	int nCh = m_InfRecv.nCh-1;

	if(MODE_RECV_EXEC != m_emMainMode) { LOG(em_INF), "いきなり受信コマンドが送られてきた！");}

	// コマンドにより処理分け
	switch( emRecv ) {
		// -------------------------------------------------------------
		case RECV_STATE_ON:												// [Ax] 点灯
			if(0 == m_nStateOnOff[nCh]) break;										// 前回から点灯なら無視
			m_nStateOnOff[nCh] = 0;
			LOG(em_MSG), "[%s][CH=%d] 点灯検出", my_sThreadName, nCh+1);

			KizuFunction::SetStatus(m_nIdOnOff[nCh], 1, false);
			syslog(SYSLOG_BASE+0, "[%s:CH=%d]", my_sThreadName, nCh+1);

			// 点灯累積時間集計開始
			AddTime_Start(nCh+1);
			break;

		// -------------------------------------------------------------
		case RECV_STATE_OFF:											// [Ay] 消灯
			if(1 == m_nStateOnOff[nCh]) break;										// 前回から消灯なら無視
			m_nStateOnOff[nCh] = 1;
			LOG(em_WAR), "[%s][CH=%d] 消灯検出", my_sThreadName, nCh+1);

			KizuFunction::SetStatus(m_nIdOnOff[nCh], 0, false);
			syslog(SYSLOG_BASE+1, "[%s:CH=%d]", my_sThreadName, nCh+1);

			// 点灯累積時間集計停止
			AddTime_Stop(nCh+1);
			break;

		// -------------------------------------------------------------
		case RECV_STATE_BREAK:											// [AA] 断線
			if(1 == m_nStateBreak[nCh]) break;								// 前回から異常なら無視
			m_nStateBreak[nCh] = 1;
			LOG(em_WAR), "[%s][CH=%d] 断線検出", my_sThreadName, nCh+1);

			KizuFunction::SetStatus(m_nIdBreak[nCh], false);
			break;

		// -------------------------------------------------------------
		case RECV_STATE_HOT:											// [AB] 温度上昇
			if(1 == m_nStateTemp[nCh]) break;								// 前回から異常なら無視
			m_nStateTemp[nCh] = 1;
			LOG(em_WAR), "[%s][CH=%d] LED加熱検出", my_sThreadName, nCh+1);

			// 加熱とシャットダウンは排他の関係
			KizuFunction::SetStatus(m_nIdHotWar[nCh], false);
			KizuFunction::SetStatus(m_nIdHotErr[nCh], true, false);
			break;

		// -------------------------------------------------------------
		case RECV_STATE_SHATDOWN:										// [AD] 加熱ｼｬｯﾄﾀﾞｳﾝ
			if(2 == m_nStateTemp[nCh]) break;								// 前回から異常なら無視
			m_nStateTemp[nCh] = 2;
			LOG(em_WAR), "[%s][CH=%d] LED加熱シャットダウン検出", my_sThreadName, nCh+1);

			// 加熱とシャットダウンは排他の関係
			KizuFunction::SetStatus(m_nIdHotWar[nCh], true, false);
			KizuFunction::SetStatus(m_nIdHotErr[nCh], false);

			// 20160524 加熱シャットダウン時には、照明電源側は消灯。φωソフト的には点灯状態となる。
			// そこで、消灯指示をだして、状態を一致させる
			LOG(em_WAR), "[%s] 強制消灯指示", my_sThreadName);
			// ===>> 坂巻 2018.04.10 LED加熱シャットダウン時に同色異ChのLEDが消灯にならない不具合修正
			//AddQue_SendCommand(SEND_EXEC_OFF, 0);
			AddQue_SendCommand_Index(SEND_EXEC_OFF, m_nMyIndex[nCh-1]);
			// <<===
			break;

		// -------------------------------------------------------------
		case RECV_STATE_OK:												// [AN] 正常
			//// 何の送信コマンドに対するアンサーか？
			if( SEND_CHECK_BREAK == emSend ) {
				LOG(em_MSG), "[%s][CH=%d] 断線復旧アンサー受信", my_sThreadName, nCh+1);
				if(0 == m_nStateBreak[nCh]) break;										// 前回から正常なら無視
				
				m_nStateBreak[nCh] = 0;
				LOG(em_MSG), "[%s][CH=%d] 断線復旧検出", my_sThreadName, nCh+1);
				KizuFunction::SetStatus(m_nIdBreak[nCh], true);

			} else if( SEND_CHECK_HEATING == emSend ) {
				LOG(em_MSG), "[%s][CH=%d] 過熱復旧アンサー受信", my_sThreadName, nCh+1);
				if(0 == m_nStateTemp[nCh]) break;										// 前回から正常なら無視

				m_nStateTemp[nCh] = 0;
				LOG(em_MSG), "[%s][CH=%d] 過熱復旧検出", my_sThreadName, nCh+1);
				KizuFunction::SetStatus(m_nIdHotWar[nCh], true);
				KizuFunction::SetStatus(m_nIdHotErr[nCh], true);

			} else {
				// 送信コマンドに対応する正常アンサーでは無い
				LOG(em_WAR), "[%s][CH=%d] 送信コマンドに対応する正常アンサーでは無い。[%s]",  my_sThreadName, nCh+1, GetSendCommandName(m_InfSend.emLedCommand));
				return -1;
			}
			break;

		// -------------------------------------------------------------
		case RECV_READ_EEPROM:											// [AE] EEPROM 調光データ 読込
			if(true) {
				m_nNowLedBase[nCh]  = m_InfRecv.cBuf[5];
				for(int ii=0; ii<m_nBlock[nCh]; ii++) m_nNowLedHosei[nCh][ii] = m_InfRecv.cBuf[6+ii];

				int nVal = (int)((double)(GetNowLedValue(nCh, 0) / 255.0)*100.0);
				LOG(em_MSG), "[%s][CH=%d] 調光データ取得 [全体調光=%d;配光=%d;光量=%d:%d%%]", my_sThreadName, nCh+1, m_nNowLedBase[nCh], m_nNowLedHosei[nCh][0], GetNowLedValue(nCh,0), nVal);
				syslog(SYSLOG_BASE+2, "[%s:CH=%d:調光=%d:配光=%d:光量=%d:%d%%]", my_sThreadName, nCh+1, m_nNowLedBase[nCh], m_nNowLedHosei[nCh][0], GetNowLedValue(nCh,0), nVal);

				KizuFunction::SetStatus(m_nIdValNow[nCh], nVal, false);	
			}
			break;

		// -------------------------------------------------------------
		case RECV_READ_RAM:												// [AP] RAM 調光データ 読込
			// 疵検では、未使用のはず
			m_nNowLedBase[nCh]  = m_InfRecv.cBuf[5];
			for(int ii=0; ii<m_nBlock[nCh]; ii++) m_nNowLedHosei[nCh][ii] = m_InfRecv.cBuf[6+ii];
			break;

		// -------------------------------------------------------------
		case RECV_LED_TEMP:												// [At] LED温度データ 読込
			if( SEND_CHECK_TEMP == emSend ) {
				// LED温度データセット
				for(int ii=0; ii<m_nTempUnit[nCh]; ii++) {
					m_nTemp[nCh][ii] = (int)m_InfRecv.cBuf[5+ii];
				}
				LOG(em_MSG), "[%s] 温度[%d][%d][%d]", my_sThreadName, m_nTemp[nCh][0], m_nTemp[nCh][1], m_nTemp[nCh][2]);

				// LED温度セット
				SetTemp(nCh+1);
			} else {
				// 送信コマンドに対応する正常アンサーでは無い
				LOG(em_WAR), "[%s][CH=%d] 送信コマンドに対応する正常アンサーでは無い。[%s]",  my_sThreadName, nCh+1, GetSendCommandName(m_InfSend.emLedCommand));
				return -1;
			}
			break;

		// -------------------------------------------------------------
		case RECV_GET_BLOCK:											// [Ab] ブロック数 読込
			if(true) {
				// 受信してログを出すだけ
				BYTE nCh	= m_InfRecv.cBuf[3];
				BYTE nBlock = m_InfRecv.cBuf[4];
				LOG(em_MSG), "[%s][CH=%d] ブロック数取得 = %d",  my_sThreadName, nCh, nBlock);
			}
			break;
		// -------------------------------------------------------------
		case RECV_CMD_ERROR:											// [Ae] コマンドエラー
			if(true) {
				// 受信してログを出すだけ
				BYTE nCh	= m_InfRecv.cBuf[3];
				LOG(em_ERR), "[%s][CH=%d] コマンドエラー",  my_sThreadName, nCh);
			}
			break;
	}

	// また最初から受信開始
	if( ! RecvStart( id, PARAM_COM_BUFSIZE, m_cSockBuf[id][BUF_RECV] )) {
		return -1;
	}

	// 後始末
	m_emMainMode = MODE_WAIT;

	//2015014
	//
	if(RECV_READ_EEPROM == emRecv)	return 9;
	else							return 0;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TPC/IP 通信制御

//------------------------------------------
// 初期化処理 (一度のみ起動)
//------------------------------------------
void LedManager::SockInit()
{
	BaseTcpIp::DELIVERY_COMMON	deli;

	//// 送信用スレッド 生成
	// 定義
	deli.nMyID = PORT_SEND;
	deli.bSorC = false;
	deli.bRorS = false;
	sprintf(deli.cMyName,  "%s", my_sThreadName);
	sprintf(deli.cSession, "%s", m_cMySession);
	deli.nBufferSize = PARAM_SOCK_BUFSIZE;
	deli.nRetryTimer = PARAM_SOCK_TIMEOUT;

	// 生成
	mcls_pSock[PORT_SEND] = new BaseTcpIp(&deli);
	mcls_pSock[PORT_SEND]->SetLogMgr(mcls_pLog);
	mcls_pSock[PORT_SEND]->SetMainPostTid(GetTid());

	// ワーカースレッド開始
	mcls_pSock[PORT_SEND]->Start();
}

//------------------------------------------
// 終了処理 (一度のみ起動)
//------------------------------------------
void LedManager::SockExit()
{
	// ワーカースレッド停止
	mcls_pSock[PORT_SEND]->Stop();
	// ワーカースレッド解放
	SAFE_DELETE( mcls_pSock[PORT_SEND] );
}


//------------------------------------------
// 強制切断
// int nClose 区分 (-1:切断するけどまだ異常にしない  0:切断&異常  1:初期化のみ  )
//------------------------------------------
void LedManager::UserClose(int nClose)
{
	if( MODE_NONE != m_emMainMode && 0 == nClose ) {		// 今接続中？
		// どのポートからでも クローズ通知が来たら、全ポート強制終了させる
		mcls_pSock[PORT_SEND]->UserClose();
		KizuFunction::SetStatus(m_nIdConnect, false);	// 機器状態に通知
		LOG(em_ERR), "[%s] 回線異常 ===================================>>>>", my_sThreadName);
	}

	if( MODE_NONE != m_emMainMode && -1 == nClose ) {		// 今接続中？
		// どのポートからでも クローズ通知が来たら、全ポート強制終了させる
		mcls_pSock[PORT_SEND]->UserClose();
		LOG(em_WAR), "[%s] 回線異常 保留中･･･", my_sThreadName);

		// ホントに異常か再チェックを開始
		__int64 ts = PARAM_RECHECK_TIMEOUT * -10000;
		SetWaitableTimer(m_tmReCheck,  (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE);
	}

	if( 1 != nClose ) 
	{
		for(int ii=0; ii<PARAM_SOCK_MAX; ii++) {
			m_eStateMain[ii]  = SOCK_NONE;
			m_nNowEventNo[ii] = 0;
		}
		// ステータス関連初期化
		m_bIsConnect = false;
		CancelWaitableTimer(m_tmAddTime);
	} else {
		m_eStateMain[ID_SEND]  = SOCK_IDLE;
		m_nNowEventNo[ID_SEND] = 0;
	}

	// 送信許可タイマーキャンセル
	CancelWaitableTimer(m_tmSendOk);
	CancelWaitableTimer(m_tmStateCheck);

	// 切断したので総合モードを待ち状態にする
	m_emMainMode = MODE_NONE;

	// 送信依頼のキューがあれば削除しておく
	DeleteAllQue();
}

//------------------------------------------
// 非同期 受信処理開始
// int id ポートID
// int size サイズ
// BYTE* pBuf バッファポインタ
// int point 受信データ開始位置
// 戻り値 false:回線切断処理を実施
//------------------------------------------
bool LedManager::RecvStart(int id, int size, BYTE* pBuf, int point)
{
	//// 非同期 受信
	// 受信情報セット
	BaseTcpIp::DELIVERY_DATA* pDeleData = new BaseTcpIp::DELIVERY_DATA;
	pDeleData->bAnsQueing = true;	// 固定
	pDeleData->bRorS = true;		// 受信
	pDeleData->nStartPoint = point;
	pDeleData->nSize = size;
	pDeleData->Data = pBuf;
	pDeleData->nUserWk[0] = size+point;			// 合計受信バイト数としてセットしておく
	
	// 受信要求
	if( mcls_pSock[id]->SetRunningData(pDeleData)) {
		return true;	
	} else {
		LOG(em_ERR), "[%s] 非同期 受信要求失敗", my_sThreadName );
		SAFE_DELETE(pDeleData);
		return false;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DB関連
//------------------------------------------
// 動的パラメータ取得
//------------------------------------------
bool LedManager::GetParam()
{
	CString sql;					// SQL文
	CString sWk;
	int scode;
	PARAM buf;
	memset(&buf, 0x00, sizeof(buf));

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}
/// 2014.10.28 諸藤(高輝度LED対応)------------------------->>>>
#if 0
	//// 共通パラメータ
	// SQL文作成
	sql =  "SELECT";
	if(0 == m_nMyMen) {
		sql += " scode, 照明照度_表, 照明照度補正値_表";

		for(int ii=2; ii<=m_nChNum; ii++) {
			sWk.Format(", 照明照度%d_表", ii);			sql += sWk;
			sWk.Format(", 照明照度補正値%d_表", ii);	sql += sWk;
		}

	} else {
		sql += " scode, 照明照度_裏, 照明照度補正値_裏";

		for(int ii=2; ii<=m_nChNum; ii++) {
			sWk.Format(", 照明照度%d_裏", ii);			sql += sWk;
			sWk.Format(", 照明照度補正値%d_裏", ii);	sql += sWk;
		}
	}
#else
	//// 共通パラメータ
	// SQL文作成
	sql =  "SELECT";
	if(0 == m_nMyMen) {
		if( 1 == m_nChStart )	{ sWk.Format(" scode, 照明照度_表, 照明照度補正値_表"); }
		else					{ sWk.Format(" scode, 照明照度%d_表, 照明照度補正値%d_表", m_nChStart, m_nChStart); }
		sql += sWk;

		for(int ii=m_nChStart+1; ii<=m_nChNum+m_nChStart-1; ii++) {
			sWk.Format(", 照明照度%d_表", ii);			sql += sWk;
			sWk.Format(", 照明照度補正値%d_表", ii);	sql += sWk;
		}

	} else {
		if( 1 == m_nChStart )	{ sWk.Format(" scode, 照明照度_裏, 照明照度補正値_裏", m_nChStart, m_nChStart); }
		else					{ sWk.Format(" scode, 照明照度%d_裏, 照明照度補正値%d_裏", m_nChStart, m_nChStart); }
		sql += sWk;

		for(int ii=m_nChStart+1; ii<=m_nChNum+m_nChStart-1; ii++) {
			sWk.Format(", 照明照度%d_裏", ii);			sql += sWk;
			sWk.Format(", 照明照度補正値%d_裏", ii);	sql += sWk;
		}
	}
#endif
/// 2014.10.28 諸藤(高輝度LED対応)<<<<-------------------------	

	sql += " FROM ";
	sql += DB_PARAM_SCODE;

	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", DB_PARAM_SCODE);
		return false;
	}

	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り
			// 主キー
			clsDB.GetData(1, &scode);
			if( scode > MAX_SCODE || scode <= 0 ) {
				// 主キー不正
				LOG(em_ERR), "[%s] [%s 主キー不正 scode=%d ]", my_sThreadName, DB_PARAM_SCODE, scode);
				syslog(SYSNO_DB_DATA_ERR, "[%s scode=%d]", DB_PARAM_SCODE, scode);					
				continue;
			}

			// データ部
			int nCh = 0;
			for(int ii=0; ii<m_nChNum; ii++) {
				
				while( !m_bUsedCh[nCh] ) nCh++;

				clsDB.GetData(2 + (2*ii), &buf.nLedBase[scode-1][nCh] );
				int nCnt = clsDB.GetDataImage(3 + (2*ii), &buf.nLedHosei[scode-1][nCh], MAX_LED_BLOCK );	// VarBinary

				// 足りない分は、先頭のブロックと同じデータをセット
				for(int jj=nCnt; jj<m_nBlock[nCh]; jj++) {
					buf.nLedHosei[scode-1][nCh][jj] = buf.nLedHosei[scode-1][nCh][0];
				}

				// 2014.12.01 諸藤(高輝度LED対応)-------->>>>>
				//左右逆展開の場合、ここで照度補正値を反転
				if( !GetDeployment(nCh) ) {
					TurnHosei(buf.nLedHosei[scode-1][nCh], m_nBlock[nCh]);
				}
				// 2014.12.01 諸藤(高輝度LED対応)<<<<<--------
				nCh++;
			}


		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", DB_PARAM_SCODE);
			return false;
		}
	}
	
	memcpy(&m_Param, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] 動的パラメータ取得完了", my_sThreadName);
	return true;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 共通処理

//------------------------------------------
// 送信コマンド名称
// EM_LED_SEND_COMMAND em コマンド
// 戻り値 名称
//------------------------------------------
CString LedManager::GetSendCommandName(EM_LED_SEND_COMMAND em)
{
	CString wk;
	switch( em ) {
		case SEND_CHECK_BREAK:
			wk = "断線有無";
			break;
		case SEND_CHECK_HEATING:
			wk = "LED加熱ステータス要求";
			break;
		case SEND_CHECK_ONOFF:
			wk = "点灯消灯状態";
			break;
		case SEND_CHECK_TEMP:
			wk = "LED温度状態";
			break;
		case SEND_EXEC_ON:
			wk = "点灯指示";
			break;
		case SEND_EXEC_OFF:
			wk = "消灯指示";
			break;
		case SEND_SEND_MASTER:
			wk = "全体調光送信";
			break;
		case SEND_SEND_VALUE:
			wk = "調光データ送信";
			break;
		case SEND_WRITE_EEPROM:
			wk = "調光EEPROM書込";
			break;
		case SEND_READ_EEPROM:
			wk = "調光EEPROM読込";
			break;
		case SEND_READ_RAM:
			wk = "調光RAM読込";
			break;
		case SEND_GET_BLOCK:
			wk = "ブロック数取得";
			break;
		case SEND_NOTHING:
			wk = "";
			break;
		default :
			wk = "モード異常";
			break;
	}
	return wk;
}
CString LedManager::GetRecvCommandName(EM_LED_RECV_COMMAND em)
{
	CString wk;
	switch( em ) {
		case RECV_NEXT_SEND_OK:
			wk = "送信許可";
			break;
		case RECV_STATE_ON:
			wk = "LED点灯";
			break;
		case RECV_STATE_OFF:
			wk = "LED消灯";
			break;
		case RECV_STATE_BREAK:
			wk = "断線";
			break;
		case RECV_STATE_HOT:
			wk = "温度上昇";
			break;
		case RECV_STATE_SHATDOWN:
			wk = "加熱ｼｬｯﾄﾀﾞｳﾝ";
			break;
		case RECV_STATE_OK:
			wk = "正常";
			break;
		case RECV_READ_EEPROM:
			wk = "調光EEPROM読込";
			break;
		case RECV_READ_RAM:
			wk = "調光RAM読込";
			break;
		case RECV_LED_TEMP:
			wk = "LED温度読込";
			break;
		case RECV_GET_BLOCK:
			wk = "ブロック数読込";
			break;
		case RECV_NOTHING:
			wk = "無視コマンド";
			break;
		case RECV_CMD_ERROR:
			wk = "コマンドエラー";
			break;
		default :
			wk = "モード異常";
			break;
	}
	return wk;
}

//------------------------------------------
// 受信コードの特定
// char code 先頭1文字
// 戻り値 受信コマンド
//------------------------------------------
LedManager::EM_LED_RECV_COMMAND LedManager::GetRecvCommand(char code)
{
	switch( code ) {
		case 'C': return RECV_NEXT_SEND_OK;
		case 'x': return RECV_STATE_ON;
		case 'y': return RECV_STATE_OFF;
		case 'A': return RECV_STATE_BREAK;
		case 'B': return RECV_STATE_HOT;
		case 'D': return RECV_STATE_SHATDOWN;
		case 'N': return RECV_STATE_OK;
		case 'E': return RECV_READ_EEPROM;
		case 'P': return RECV_READ_RAM;
		case 't': return RECV_LED_TEMP;
		case 'b': return RECV_GET_BLOCK;
		case 'T': return RECV_TIME_OUT;
		case 'e': return RECV_CMD_ERROR;
		case 0x80: return RECV_NOTHING;
		default : return RECV_NON;
	}
}

LedManager::EM_LED_SEND_COMMAND LedManager::GetSendCommand(char code)
{
	switch( code ) {
		case 'a': return SEND_CHECK_BREAK;
		case 'b': return SEND_CHECK_HEATING;
		case 'l': return SEND_CHECK_ONOFF;
		case 't': return SEND_CHECK_TEMP;
		case 'X': return SEND_EXEC_ON;
		case 'Y': return SEND_EXEC_OFF;
		case 'T': return SEND_SEND_MASTER;
		case 'S': return SEND_SEND_VALUE;
		case 'W': return SEND_WRITE_EEPROM;
		case 'M': return SEND_READ_EEPROM;
		case 'R': return SEND_READ_RAM;
		case 'B': return SEND_GET_BLOCK;
		case 0x00: return SEND_NOTHING;
		default : return SEND_NON;
	}
}

//------------------------------------------
// メインモード名称
// EM_LED_MAINMODE em モード
// 戻り値 名称
//------------------------------------------
CString LedManager::GetMainModeName(EM_LED_MAINMODE em)
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
			wk = "受信コマンド待ち";
			break;
		default :
			wk = "モード異常";
			break;
	}
	return wk;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 点灯累積時間関係

//------------------------------------------
// 累積開始
// int	nCh 対象チャンネル (1～:指定チャンネルのみ)
//------------------------------------------
void LedManager::AddTime_Start(int nCh)
{
	if(0 == m_nIdTotalTime[nCh-1]) return;

	__int64 ts = (__int64)PARAM_ADDTIME * 60 * 1000 -10000;	// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(m_tmAddTime[nCh-1],  (LARGE_INTEGER *)&ts, (PARAM_ADDTIME * 60 * 1000), NULL, NULL, FALSE);
}
//------------------------------------------
// 累積停止
// int	nCh 対象チャンネル (1～:指定チャンネルのみ)
//------------------------------------------
void LedManager::AddTime_Stop(int nCh)
{
	CancelWaitableTimer(m_tmAddTime[nCh-1]);
}

//------------------------------------------
// 累積加算
// int	nCh 対象チャンネル (1～:指定チャンネルのみ)
//------------------------------------------
void LedManager::AddTime_Exec(int nCh)
{
	CString sql;					// SQL文

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return;
	}

	sql.Format("UPDATE %s SET 機器状態=機器状態+%d WHERE 機器ID=%d", DB_STATUS_INF, PARAM_ADDTIME, m_nIdTotalTime[nCh-1]);
	if( !clsDB.ExecuteDirect(sql)) {
		LOG(em_ERR), "[%s] LED照明稼動累計時間 加算失敗 [CH=%d]", my_sThreadName, nCh);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s:CH=%d]", DB_STATUS_INF, nCh);
		return;
	}
}
//------------------------------------------
// 累積リセット
// int	nCh 対象チャンネル (0:全チャンネル  1～:指定チャンネルのみ)
//------------------------------------------
void LedManager::AddTime_Reset(int nCh)
{
	int ii_s, ii_e;
	GetChLoop(nCh, ii_s, ii_e);

	for(int ii=ii_s; ii<=ii_e; ii++) {

		if( !m_bUsedCh[ii-1] ) continue;	// 未使用CHの場合、リセットしない
		KizuFunction::SetStatus(m_nIdTotalTime[ii-1], 0, false);
	}
}

//------------------------------------------
// LED温度セット（現在状態）
// int	nCh 対象チャンネル (1～:指定チャンネルのみ)
//------------------------------------------
void LedManager::SetTemp(int nCh)
{
	for(int ii=0; ii<m_nTempUnit[nCh-1]; ii++) {
		if ( m_nTempThrDown <= m_nTemp[nCh-1][ii] ) {		// 強制消灯+温度異常

			AddQue_SendCommand_Index(SEND_EXEC_OFF, m_nMyIndex[nCh-1]);	// 強制消灯

			if (2 != m_nTempSts[nCh-1][ii]) {
				LOG(em_ERR), "[%s][CH=%d] LED照明温度異常+強制消灯(%d) %d℃", my_sThreadName, nCh, ii+1, m_nTemp[nCh-1][ii]);
				if(0!=m_nIdTempSt[nCh-1][ii]) KizuFunction::SetStatus(m_nIdTempSt[nCh-1][ii], false);
			}
			m_nTempSts[nCh-1][ii] = 2;

		} else if ( m_nTempThr <= m_nTemp[nCh-1][ii] ) {	// 温度異常
			if (1 != m_nTempSts[nCh-1][ii]) {
				LOG(em_ERR), "[%s][CH=%d] LED照明温度異常(%d) %d℃", my_sThreadName, nCh, ii+1, m_nTemp[nCh-1][ii]);
				if(0!=m_nIdTempSt[nCh-1][ii]) KizuFunction::SetStatus(m_nIdTempSt[nCh-1][ii], false);
			}
			m_nTempSts[nCh-1][ii] = 1;
		} else {
			if ( 0 != m_nTempSts[nCh-1][ii]) {
				LOG(em_MSG), "[%s][CH=%d] LED照明温度復旧(%d) %d℃", my_sThreadName, nCh, ii+1, m_nTemp[nCh-1][ii]);
				if(0!=m_nIdTempSt[nCh-1][ii]) KizuFunction::SetStatus(m_nIdTempSt[nCh-1][ii], true);
			}
			m_nTempSts[nCh-1][ii] = 0;
		}

		if(0!=m_nIdTemp[nCh-1][ii]) KizuFunction::SetStatus(m_nIdTemp[nCh-1][ii], m_nTemp[nCh-1][ii], false);
	}
}

/// 2014.12.01 諸藤(高輝度LED対応) ------------------------------------------------>>>>
//==========================================
// 照度補正値のバイナリ列を反転
//------------------------------------------
// BYTE *	nHosei		: 照度補正値のバイナリ列(先頭アドレス)	
// int		nCnt		: 照度補正値のブロック数	
//==========================================
void LedManager::TurnHosei(BYTE * nHosei, int nCnt)
{
	BYTE Wk[MAX_LED_BLOCK];
	
	for(int ii=0; ii<nCnt; ii++){
		Wk[ii] = nHosei[nCnt-ii-1];
	}

	memcpy(nHosei, &Wk, nCnt);
}

//====================================================
// 送受信処理(INDEX指定版)
//----------------------------------------------------
// Indexに該当するCHを指定して従来の送受信処理をコール
//====================================================

//==========================================
// 1バイトだけの送信コマンド専用
//------------------------------------------
// EM_LED_SEND_COMMAND	em		：送信コマンド	
// int					nIndex	：Index
//==========================================
void LedManager::AddQue_SendCommand_Index(EM_LED_SEND_COMMAND em, int nIndex)
{
	for(int ii=0; ii<MAX_LED_CH; ii++){
		if( IsTargetIndex(ii+1, nIndex) ){
			AddQue_SendCommand(em, ii+1);
		}
	}
}

//==========================================
// マスター＋配光データセット
//------------------------------------------
// int		nIndex		: Index		
// BYTE		nLedBase	: 全体調光
// BYTE*	nLedHosei	: 配光データ
//==========================================
void LedManager::AddQue_SetLightMastVal_Index(int nIndex, BYTE nLedBase, BYTE* nLedHosei)
{
	for(int ii=0; ii<MAX_LED_CH; ii++){
		if( IsTargetIndex(ii+1, nIndex) ){
			AddQue_SetLightMastVal(ii+1, nLedBase, nLedHosei);
		}
	}
}

//==========================================
// マスター＋配光データセット (scode)
//------------------------------------------
// int		nIndex		: Index	
// int		scode		: 検査表面状態
//==========================================
void LedManager::AddQue_SetLightMastVal_Index(int nIndex, int scode)
{
	for(int ii=0; ii<MAX_LED_CH; ii++){
		if( IsTargetIndex(ii+1, nIndex) ){
			AddQue_SetLightMastVal(ii+1, scode);
		}
	}
}

//=================================================
// マスター＋配光データセット (配光部は全部同一値)
//-------------------------------------------------
// int		nIndex		: Index
// BYTE		nLedBase	: 全体調光
// BYTE		nLedHosei	: 配光データ
//=================================================
void LedManager::AddQue_SetLightMastVal_Index(int nIndex, BYTE nLedBase, BYTE nLedHosei)
{
	for(int ii=0; ii<MAX_LED_CH; ii++){
		if( IsTargetIndex(ii+1, nIndex) ){
			AddQue_SetLightMastVal(ii+1, nLedBase, nLedHosei);
		}
	}
}

//==========================================
// 全体調光データセット
//------------------------------------------
// int	nIndex		: Index
// int	nLedBase	: 全体調光
//==========================================
void LedManager::AddQue_SetLightMaster_Index(int nIndex, int nLedBase)
{
	for(int ii=0; ii<MAX_LED_CH; ii++){
		if( IsTargetIndex(ii+1, nIndex) ){
			AddQue_SetLightMaster(ii+1, nLedBase);
		}
	}
}

//==========================================
// 調光データセット (配光部は一括指定)
//------------------------------------------
// int		nIndex		: Index
// BYTE*	nLedHosei	: 配光データ
//==========================================
void LedManager::AddQue_SetLightValue_Index(int nIndex, BYTE* nLedHosei)
{
	for(int ii=0; ii<MAX_LED_CH; ii++){
		if( IsTargetIndex(ii+1, nIndex) ){
			AddQue_SetLightValue(ii+1, nLedHosei);
		}
	}
}

//==========================================
// 配光データセット (配光部は全部同一値)
//------------------------------------------
// int		nIndex		: Index
// BYTE		nLedHosei	: 配光データ	
//==========================================
void LedManager::AddQue_SetLightValue_Index(int nIndex, BYTE nLedHosei)
{
	for(int ii=0; ii<MAX_LED_CH; ii++){
		if( IsTargetIndex(ii+1, nIndex) ){
			AddQue_SetLightValue(ii+1, nLedHosei);
		}
	}
}

//==========================================
// 調光データセット (配光部は個別指定)
//------------------------------------------
// int	nIndex		: Index		
// int	nBlock		: ブロック
// BYTE	nLedHosei	: 配光データ
//==========================================
void LedManager::AddQue_SetLightValue_Index(int nIndex, int nBlock, BYTE nLedHosei)
{
	for(int ii=0; ii<MAX_LED_CH; ii++){
		if( IsTargetIndex(ii+1, nIndex) ){
			AddQue_SetLightValue(ii+1, nBlock, nLedHosei);
		}
	}
}

//==========================================
// 累積点灯時間リセット 
//------------------------------------------
// int	nIndex		: Index	
//==========================================
void LedManager::AddTime_Reset_Index(int nIndex)
{
	for(int ii=0; ii<MAX_LED_CH; ii++){
		if( IsTargetIndex(ii+1, nIndex) ){
			AddTime_Reset(ii+1);
		}
	}
}
/// 2014.12.01 諸藤(高輝度LED対応) <<<<------------------------------------------------