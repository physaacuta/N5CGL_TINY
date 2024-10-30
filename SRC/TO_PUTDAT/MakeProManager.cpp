#include "StdAfx.h"
#include "MakeProManager.h"
#include "limits.h"

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
MakeProManager::MakeProManager(void) :
ThreadManager("Pro"),
m_bCreateOk(true),
mcls_pDB(NULL)
{
	//// クリティカルセクションオブジェクトを初期化
	InitializeCriticalSection(&m_csLock);

	// 処理中変数 初期化
	ClearMember();
	
	//// INIファイルから定数取込
	// コイル実績書き込み問い合わせ応答待ち時間 [msec]
	m_nMaxWaitTime = GetPrivateProfileInt(TO_PUTDAT, "ADDDB_MAXWAITTIME", 5000, TASKINI_NAME);
	// コイル検索の対象期間
	m_nChkCoilDate = GetPrivateProfileInt(TO_PUTDAT, "CHKCOILDATE", 1, TASKINI_NAME);

	//// シグナル生成
	m_evDefectDbStop	= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evDefectDbAdd		= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_dHosei = 1.0;
}

//------------------------------------------
// デストラクタ
//------------------------------------------
MakeProManager::~MakeProManager(void)
{
	//// クリティカルセクションオブジェクトを解放
	DeleteCriticalSection(&m_csLock);

	//// シグナル解放
	CloseHandle(m_evDefectDbStop);
	CloseHandle(m_evDefectDbAdd);
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int MakeProManager::ThreadFirst()
{
	//// イベント設定
	HANDLE hArray[] =
	{
		gque_Deli.g_evSem 
	};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int MakeProManager::ThreadLast()
{
	DELIVERY_KEY *pDeli;				// キューデータ

	// 未処理のキューを破棄
	while (true)
	{
		pDeli = gque_Deli.GetFromHead(0);
		if( pDeli == NULL ) break;
		
		// 削除
		LOG(em_WAR), "[%s] 未処理キュー削除 [%s]", my_sThreadName, pDeli->cKey);
		SAFE_DELETE(pDeli);
	}
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void MakeProManager::ThreadEvent(int nEventNo)
{
	DELIVERY_KEY*	pDeli;					// 受け渡し依頼データ

	////// シグナルの条件
	enum
	{
		EV_QUE = 0,							// キュー依頼
	};

	////// シグナル条件分け
	switch (nEventNo)
	{
//-----------------------------------------------------------------------------------------------
	case EV_QUE: 							// キュー依頼
		if (true)
		{
			pDeli = gque_Deli.GetFromHead(); 
			bool retc = Exec(pDeli->cKey);
			// 処理中変数 初期化
			ClearMember();
			SAFE_DELETE(pDeli);
			
			// 機器状態
			//if (m_bCreateOk != retc)
			//{
				m_bCreateOk = retc;
				KizuFunction::SetStatus("ZI_CREATE_PRO_ERR_ID", retc, false);	// シスログ表示は出来ない
			//}
		}
		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo);
		return;
	}
}

//------------------------------------------
// 対制御シーケンサデータメモリ通信 に送信依頼
// const char *cKey ID
// 戻り値 : 送信結果
//------------------------------------------
bool MakeProManager::Send_To_Procon(const char *cKey)
{
	bool bRet = true;

	COMMON_QUE que;					// メール構造体
	memset(&que, 0x00, sizeof(que));

	que.nEventNo = FACT_TO_PROCON_01;
	que.nLineNo = getlineid();

	strcpy(que.fc.data, cKey);

	int nRet = send_mail(TO_PROCON, ".", &que);
	if (0 != nRet)
	{
		LOG(em_ERR), "[%s] プロコン用検査実績送信要求(FACT_TO_PROCON_01) 送信エラー [ID=%s][err_code=%d]", my_sThreadName, que.fc.data, nRet);
		syslog(SYSNO_MAIL_SNED_ERR, "[プロコン用検査実績送信要求(FACT_TO_PROCON_01)][err_code=%d]", nRet);
		bRet = false;
	}
	else
	{
		LOG(em_MSG), "[%s] プロコン用検査実績送信要求(FACT_TO_PROCON_01) 送信 [ID=%s]", my_sThreadName, que.fc.data);
		//syslog(, "[ID=%s]", que.fc.data);
		bRet = true;
	}

	return bRet;
}

//------------------------------------------
// 統括管理に実績書き込み状況問い合わせ送信依頼
// const PRO_TYPE_DATA_CUT *pData プロコンカット実績生データ
//------------------------------------------
void MakeProManager::Send_To_Defect(const PRO_TYPE_DATA_CUT *pData)
{
	COMMON_QUE que;					// メール構造体
	memset(&que, 0x00, sizeof(que));

	que.nEventNo = FACT_TO_DEFECT_13;
	que.nLineNo = getlineid();

	memcpy(que.mix.cdata[0], pData->KeyInf.cCoilNo, sizeof(pData->KeyInf.cCoilNo));
	memcpy(que.mix.cdata[1], m_cKizukenNo, sizeof(m_cKizukenNo));

	int nRet = send_mail(TO_DEFECT, ".", &que);
	if (0 != nRet)
	{
		LOG(em_ERR), "[%s] 実績書込み状況問合せ(FACT_TO_DEFECT_13) 送信エラー [コイルNo=%s][管理No=%s][err_code=%d]", my_sThreadName, que.mix.cdata[0], que.mix.cdata[1], nRet);
		syslog(SYSNO_MAIL_SNED_ERR, "[実績書込み状況問合せ(FACT_TO_DEFECT_13)][err_code=%d]", nRet);
	}
	else
	{
		LOG(em_MSG), "[%s] 実績書込み状況問合せ(FACT_TO_DEFECT_13) 送信 [コイルNo=%s][管理No=%s]", my_sThreadName, que.mix.cdata[0], que.mix.cdata[1]);
		//syslog(, "[管理No=%s; コイルNo=%s]", que.mix.cdata[1], que.mix.cdata[0]);
	}
}

//------------------------------------------
// 処理中変数 初期化
//------------------------------------------
void MakeProManager::ClearMember()
{
	// 処理中変数 初期化
	SAFE_DELETE(mcls_pDB);
	memset(m_cKizukenNo, 0x00, sizeof(m_cKizukenNo));
	memset(&m_typBuf, 0x00, sizeof(m_typBuf));
}

//------------------------------------------
// プロコン検査実績 伝文生成
// char const* cID BUF_PRO_R_CUTのキー
// 戻り値 復帰情報
//------------------------------------------
bool MakeProManager::Exec(char const* cID)
{
	AutoLock lock(&m_csLock);

	PRO_TYPE_ZISEKI		typZi;							// プロコン用 穴検実績伝文
	int					nZiErr = 0;						// 実績作成処理エラー(0:正常 1:異常)

	//// 処理中変数 初期化
	ClearMember();

	//// ODBC接続クラス インスタンス生成
	mcls_pDB = new OdbcBase();
	if (!mcls_pDB->Connect(TASKINI_NAME))
	{
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "[%s]", my_sThreadName);
		syslog(359, "[ID=%s DB接続エラー]", cID);
		return false;
	}

	//// プロコンカット実績受信バッファテーブル取得
	if (!GetBufferCut(cID))
	{
		// 取得失敗
		LOG(em_ERR), "[%s] カット実績受信バッファテーブルデータなし [ID=%s]", my_sThreadName, cID);
		syslog(359, "[ID=%s カット実績該当無し]", cID);
		return false;
	}

// 同一カット実績かチェックは、今回無しとする！
#if 0		// 1,5CGLは同一カット実績伝文が送られてくる場合有り。ただし、実績区分以外は同一内容であり、再生成しても問題なしのはず！
	//// 同一カット実績かチェック
	// カット後コイル抜き取りでも 同一のカット実績伝文が送られてくる為の処置
	if (CheckEqalCut(cID))
	{
		// 対プロコン通信に送信要求した為、処理終わり
		return true;
	}
#endif // 0

	//// コイル情報取得
	if (!GetCoilInf_KizukenNo())
	{
		LOG(em_ERR), "[%s] 該当コイルなしにより 不信頼実績伝文を生成 [コイルNo=%.12s]", my_sThreadName, m_typBuf.data.KeyInf.cCoilNo);
		nZiErr = 1;
	}

	//// 統括管理側で、対象範囲の疵実績が全部DB登録完了しているかチェック
	// ブロックされるので注意
	if (0 == nZiErr)			// コイル情報も無いなら無視
	{
		if (!CheckDefectInsertOk())
		{
			LOG(em_ERR), "[%s] 統括管理 実績書込み遅れにより 不信頼実績伝文を生成 [コイルNo=%.12s]", my_sThreadName, m_typBuf.data.KeyInf.cCoilNo);
			syslog(354, "[コイルNo=%.12s][管理No=%s]", m_typBuf.data.KeyInf.cCoilNo, m_cKizukenNo);
			nZiErr = 1;
		}
	}

	//// そもそも受信したカット実績のグラントフラグが異常
	if (PRO_GRAND_OK != m_typBuf.data.Common.nGrandFlg)
	{
		LOG(em_WAR), "[%s] カット実績 グラントフラグ異常 [%d]", my_sThreadName, m_typBuf.data.Common.nGrandFlg);
		nZiErr = 1;
	}
	
	//// プロコン送信用 実績伝文生成
	MakeSendData(nZiErr, &typZi);
	

	//// プロコン用検査実績伝文 を バッファテーブルに登録
	if (InsertZiBuf(cID, &typZi))
	{
		LOG(em_MSG), "[%s] プロコン検査実績送信バッファテーブル(BUF_PRO_S_ZISEKI) 登録完了", my_sThreadName);

		//// 対プロコン通信にプロコン用検査実績送信要求
		if (!Send_To_Procon(cID))
		{
			LOG(em_ERR), "[%s]  [ID=%s 通信タスクへの送信要求通知エラー]", my_sThreadName, cID);
			syslog(359, "[ID=%s 通信タスクへの送信要求通知エラー]", cID);
		}
		else
		{
			syslog(358, "[コイルNo=%.12s][子番=%d][管理No=%s][ID=%s; 実績区分=%s; 収集区分=%s]",
				m_typBuf.data.KeyInf.cCoilNo, m_typBuf.data.nKoban, m_cKizukenNo, cID,
				(0 == typZi.data.nZiMode) ? "正常" : "異常",
				(0 == typZi.data.nKikiMode) ? "正常" : "異常");
		}

	}
	else
	{
		LOG(em_ERR), "[%s] プロコン検査実績送信バッファテーブル(BUF_PRO_S_ZISEKI) 登録失敗", my_sThreadName);
		syslog(359, "[ID=%s 伝文DB登録エラー]", cID);
		return false;
	}

	//// プロコンカット実績受信バッファテーブル 更新
	if (UpdateProconBuf(cID))
	{
		LOG(em_MSG), "[%s] プロコンカット実績受信バッファテーブル(BUF_PRO_R_CUT) 更新完了", my_sThreadName);
	}
	else
	{
		LOG(em_ERR), "[%s] プロコンカット実績受信バッファテーブル(BUF_PRO_R_CUT) 更新失敗", my_sThreadName);
	}
	
	return true;
}

//------------------------------------------
// プロコンカット実績受信バッファテーブル取得
// char const* cID キー
//------------------------------------------
bool MakeProManager::GetBufferCut(char const* cID)
{
	//// SQL文作成
	CString				cSql;
	cSql.Format("SELECT 生伝文 FROM %s WHERE ID='%s'", DB_BUF_PRO_R_CUT, cID);

	// データ取得(Binary)
	//LOG(em_NON), "%s", cSql);
	return mcls_pDB->GetSelectImage(cSql, sizeof(PRO_TYPE_CUT), (char*)&m_typBuf);
}

//------------------------------------------
// 同一カット実績かチェック
// char const* cID キー
// 戻り値 true:重複有り  false:重複無し
//------------------------------------------
bool MakeProManager::CheckEqalCut(char const* cID)
{
	CString				cSql;
	CString				cWk;
	char				cSendKey[SIZE_SERIAL_FF_NO];				// 同一キー

	//// 現在日時取得
	SYSTEMTIME st;
	GetLocalTime(&st);

	//// 同一のカット実績伝文を取得
	cSql = "SELECT TOP 1 ID FROM ";
	cSql += DB_BUF_PRO_R_CUT;
	cSql += " WHERE ";
	cWk.Format("ID<>'%s' AND ", cID); 																		cSql += cWk;
	cWk.Format("キー情報='%.4s%.12s' AND ", m_typBuf.data.KeyInf.cMeiNo, m_typBuf.data.KeyInf.cCoilNo); 	cSql += cWk;
	cWk.Format("開始位置=%d AND ", m_typBuf.data.nStart);													cSql += cWk;
	cWk.Format("終了位置=%d AND ", m_typBuf.data.nEnd);														cSql += cWk;
	cWk.Format("登録日付>DATEADD(d,-%d,'%d/%d/%d') AND ", m_nChkCoilDate, st.wYear, st.wMonth, st.wDay);	cSql += cWk;
	cSql += "編集日付 IS NOT NULL ";
	cSql += "ORDER BY 登録日付 DESC";

	if (!mcls_pDB->GetSelectKey(cSql, sizeof(cSendKey), cSendKey))
	{
		// 同一キーは無し
		LOG(em_NON), "%s", cSql);
		return false;
	}

	//// 送信するプロコン実績伝文がホントにある？
	int nCnt;
	cSql.Format("SELECT COUNT(*) FROM %s WHERE ID='%s'", DB_BUF_PRO_S_ZISEKI, cSendKey);
	if (!mcls_pDB->GetSelectKey(cSql, &nCnt))
	{
		LOG(em_NON), "%s", cSql);
		return false;
	}
	if (1 != nCnt)
	{
		LOG(em_WAR), "[%s] 同一カット実績はあったが、実績情報伝文が未生成 [ID=%s]", my_sThreadName, cSendKey);
		return false;
	}

	//// 既に生成した実績伝文を送信
	LOG(em_INF), "[%s] 同一カット実績があるので前回の情報を送信 [ID=%s]", my_sThreadName, cSendKey);
	if (!Send_To_Procon(cSendKey))
	{
		syslog(359, "[ID=%s 通信タスクへの送信要求通知エラー]", cSendKey);
	}
	else
	{
		syslog(358, " (前回情報送信) [コイルNo=%.12s][子番=%d][ID=%s]",
			m_typBuf.data.KeyInf.cCoilNo,
			m_typBuf.data.nKoban,
			cSendKey);
	}
	return true;
}

//------------------------------------------
// コイル情報から管理Noを取得
//------------------------------------------
bool MakeProManager::GetCoilInf_KizukenNo()
{
	CString				cSql;
	CString				cWk;

	//// 現在日時取得
	SYSTEMTIME st;
	GetLocalTime(&st);

	//// コイル情報から管理Noを取得
	cSql  = "SELECT TOP 1 管理No FROM ";
	cSql += DB_COIL_INF;
	cSql += " WHERE ";
	cWk.Format("生産No='%.12s' AND ", m_typBuf.data.KeyInf.cCoilNo);									cSql += cWk;
	cWk.Format("登録日付>DATEADD(d,-%d,'%d/%d/%d') ", m_nChkCoilDate, st.wYear, st.wMonth, st.wDay);	cSql += cWk;
	cSql += "ORDER BY 登録日付 DESC";

	if (!mcls_pDB->GetSelectKey(cSql, SIZE_KIZUKEN_NO, m_cKizukenNo))
	{
		// 1件も無い
		LOG(em_NON), "%s", cSql);
		return false;
	}
	
	LOG(em_MSG), "[%s] コイルNo=%.12s をT_COIL_INFから特定 [管理No=%s]", my_sThreadName, m_typBuf.data.KeyInf.cCoilNo, m_cKizukenNo);
	return true;
}

//------------------------------------------
// 統括管理がDB書き込み完了した？
// 戻り値 true:統括側で書き込み完了 false:まだ書き込んでいる最中なので、もう不信頼にする
//------------------------------------------
bool MakeProManager::CheckDefectInsertOk()
{
	bool				bAnser = true;
	CString				cSql;

	//// そもそもコイル実績があれば、DB登録は完了している
	int nCnt;
	cSql.Format("SELECT COUNT(*) FROM %s WHERE 管理No='%s'", DB_COIL_RESULT, m_cKizukenNo);
	if (!mcls_pDB->GetSelectKey(cSql, &nCnt))
	{
		LOG(em_NON), "%s", cSql);
		return false;
	}
	if (1 == nCnt)
	{
		LOG(em_INF), "[%s] COIL_RESULTが既に有り。(通板完了済み) [管理No=%s]", my_sThreadName, m_cKizukenNo);
		//LOG(em_NON), "%s", cSql);
		return true;
	}

	//// 統括管理の書き込み状態問い合わせ待ち
	ResetEvent( m_evDefectDbAdd	);
	m_nDefectDbAddAnser = 0;

	// 統括管理に書き込み状態問い合わせ
	Send_To_Defect(&m_typBuf.data);

	// 統括からの結果待ち
	HANDLE evCheckTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	__int64 ts = m_nMaxWaitTime * -10000;	// 1μ秒→100ナノ秒 (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
	SetWaitableTimer(evCheckTimer,   (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE);

	HANDLE hArray[] =
	{
		m_evDefectDbStop,
		m_evDefectDbAdd,
		evCheckTimer
	};
	enum
	{
		EV_STOP = WAIT_OBJECT_0,			// スレッド終了
		EV_ANSER,							// 統括管理よりアンサー
		EV_CHECK_TIMER,						// 定周期監視タイマー
		EV_END
	};

	LOG(em_MSG), "[%s] 実績書込み完了通知 (FACT_TO_PUTDAT_04) 受信待ち･･･", my_sThreadName);

	bool wEnd = true;
	while (wEnd)
	{
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);

		switch (ret)
		{
//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// スレッド終了
			wEnd = false;
			bAnser = false;
			LOG(em_ERR), "[%s] 統括管理側 実績書込み待ち 強制中断", my_sThreadName);
			break;

//-----------------------------------------------------------------------------------------------
		case EV_ANSER:												// 統括管理よりアンサー
			wEnd = false;
			bAnser = 0==m_nDefectDbAddAnser ? true : false;
			LOG(em_MSG), "[%s] 統括管理側 実績書込み待ち 完了 [結果=%d]", my_sThreadName, m_nDefectDbAddAnser);
			break;

//-----------------------------------------------------------------------------------------------
		case EV_CHECK_TIMER:										// 定周期監視タイマー
			wEnd = false;
			bAnser = false;
			LOG(em_ERR), "[%s] 統括管理側 実績書込み待ち 受信タイムアウト", my_sThreadName);
			break;

//-----------------------------------------------------------------------------------------------
		default :
			// ありえない！！
			break;
		}

	}

	// 解放
	CancelWaitableTimer(evCheckTimer);
	CloseHandle(evCheckTimer);

	return bAnser;
}

//------------------------------------------
// プロコン送信用 実績伝文生成
// int nZiErr 実績作成処理エラー (0:正常 1:異常)
// PRO_TYPE_ZISEKI *pTypZi 実績伝文
//------------------------------------------
void MakeProManager::MakeSendData(int nZiErr, PRO_TYPE_ZISEKI *pTypZi)
{
	bool			bWk;

	//// 初期化
	memset(pTypZi, 0x00, sizeof(PRO_TYPE_ZISEKI));
	// 0x00で無い 予備エリアは予めその型で初期化しておく
	memset(pTypZi->data.Common.cYobi, 0x20, sizeof(pTypZi->data.Common.cYobi));
	memset(pTypZi->data.cYobi1, 0x20, sizeof(pTypZi->data.cYobi1));
	memset(pTypZi->data.cYobi, 0x20, sizeof(pTypZi->data.cYobi));


	//// 伝文へ検査実績データセット
	if (!ExecDefectInf(pTypZi))
	{
		LOG(em_WAR), "[%s] 検査実績データセット異常", my_sThreadName);
	}

	//// ヘッダー部
	pTypZi->head.bHead[0] = PRO_HEAD_H_GYOUMU;
	pTypZi->head.bHead[1] = PRO_HEAD_L_OK;
	//pTypZi->head.nLen = sizeof(PRO_TYPE_ZISEKI);
	pTypZi->head.nLen = PRO_TYPE_LENGTH(sizeof(PRO_TYPE_ZISEKI));	// Csemi対応
	pTypZi->head.nTC = PRO_TC_ZISEKI;
	pTypZi->data.Common.nKubun = PRO_KUBUN_NOMAL;

	// 送信時刻
	SYSTEMTIME STime;
	GetLocalTime(&STime);
	pTypZi->data.Common.nDate[0] = STime.wYear % 100;
	pTypZi->data.Common.nDate[1] = STime.wMonth;
	pTypZi->data.Common.nDate[2] = STime.wDay;
	pTypZi->data.Common.nDate[3] = STime.wHour;
	pTypZi->data.Common.nDate[4] = STime.wMinute;
	pTypZi->data.Common.nDate[5] = STime.wSecond;

	// グラントフラグ (0:正常 1:異常)
	if (1 == nZiErr) pTypZi->data.Common.nGrandFlg = PRO_GRAND_NG;	// 異常
	else			 pTypZi->data.Common.nGrandFlg = PRO_GRAND_OK;	// 正常
	
	//// データ部
	pTypZi->data.nZiMode = pTypZi->data.Common.nGrandFlg;			// 実績区分 = グラントフラグのデータと同じ

	// 収集区分
	bWk = CheckState_Kensa();								// 対象範囲内の検査状態をチェック
	pTypZi->data.nKikiMode = bWk ? 0 : 1;					// (0:正常 1:異常)

	memcpy(pTypZi->data.KeyInf.cMeiNo, m_typBuf.data.KeyInf.cMeiNo, sizeof(m_typBuf.data.KeyInf.cMeiNo));		// 命令No
	memcpy(pTypZi->data.KeyInf.cCoilNo, m_typBuf.data.KeyInf.cCoilNo, sizeof(m_typBuf.data.KeyInf.cCoilNo));	// コイルNo
	pTypZi->data.nKoban = m_typBuf.data.nKoban;				// 小番 (1CAPLのみデータが入っているはず。CGLは空白)

	return;
}

//------------------------------------------
// 対象範囲 の検査状態は正常かチェック
//------------------------------------------
bool MakeProManager::CheckState_Kensa()
{
	CString			cSql;
	int				nCnt;
	int				nBetween[2];			// 開始、終了範囲 COIL_POSITIONのインデックス位置 (IDは5m間隔で振られる(例:ID=0:0m-4.9m...))

	//// そもそも異常停止している？
	// この時点では、実績が出来ていない場合も普通にある。この場合は、関係なし
	// 実績が出来上がっている場合に、実績格納区分 が 異常停止の場合は 不信頼にする
	cSql.Format("SELECT ISNULL(実績格納区分,0) FROM %s WHERE 管理No='%s' ", DB_COIL_RESULT, m_cKizukenNo );
	if (mcls_pDB->GetSelectKey(cSql, &nCnt))
	{
		if (DIV_END_STOP_MANUAL == nCnt || DIV_END_STOP_AUTO == nCnt)
		{
			LOG(em_WAR), "[%s] 実績格納区分が強制停止のため、不信頼フラグセット", my_sThreadName);
			LOG(em_NON), "%s", cSql);
			return false;
		}
	}

	//// 今回の対象範囲
#ifdef PRO_SEND_ZI_HOSEI
	nBetween[0] = (int)((double)m_typBuf.data.nStart / 10.0 / m_dHosei) / MAP_REPRES_MIN;
	nBetween[1] = (int)((double)m_typBuf.data.nEnd / 10.0 / m_dHosei + 0.9) / MAP_REPRES_MIN;
	LOG(em_MSG), "[%s] 検査状態判定 ﾌﾟﾛｺﾝ採寸開始,終了位置[%d～%d](10-1m) 子番[%d] 補正率[%f%%]", my_sThreadName, m_typBuf.data.nStart, m_typBuf.data.nEnd, m_typBuf.data.nKoban, m_dHosei);
#else
	nBetween[0] = (int)((double)m_typBuf.data.nStart / 10.0) / MAP_REPRES_MIN;
	nBetween[1] = (int)((double)m_typBuf.data.nEnd / 10.0 + 0.9) / MAP_REPRES_MIN;
	LOG(em_MSG), "[%s] 検査状態判定 ﾌﾟﾛｺﾝ採寸開始,終了位置[%d～%d](10-1m) 子番[%d]", my_sThreadName, m_typBuf.data.nStart, m_typBuf.data.nEnd, m_typBuf.data.nSubNo);
#endif // PRO_SEND_ZI_HOSEI
	LOG(em_MSG), "[%s] 検査状態判定 最小代表疵間隔換算 開始,終了位置[%d～%d](%dm) ", my_sThreadName, nBetween[0], nBetween[1], MAP_REPRES_MIN);

	//// 対象範囲内にデータはあるか？ (下の条件だけでは、チェックできない為)
	cSql.Format("SELECT COUNT(*) FROM %s WHERE 管理No='%s' AND (ID BETWEEN %d AND %d)",
		DB_COIL_POSITION, m_cKizukenNo, nBetween[0], nBetween[1]);
	if (!mcls_pDB->GetSelectKey(cSql, &nCnt))
	{
		LOG(em_ERR), "[%s] 対象コイルのコイル位置情報取得失敗 [%d～%d]", my_sThreadName, nBetween[0], nBetween[1]);
		LOG(em_NON), "%s", cSql);
		return false;
	}
	if (0 == nCnt)
	{
		LOG(em_WAR), "[%s] 対象コイルの範囲内に 一件もデータ無し [%d～%d]", my_sThreadName, nBetween[0], nBetween[1]);
		LOG(em_NON), "%s", cSql);
		return false;
	}

	//// 対象範囲内で、正常以外があるか？
	for (int ii = 0; ii < NUM_MEN; ii++) {
		if (0 == ii) {
			cSql.Format("SELECT COUNT(*) FROM %s WHERE 管理No='%s' AND 検査状態_表<>%d AND (ID BETWEEN %d AND %d)",
				DB_COIL_POSITION, m_cKizukenNo, DIV_KENSA_OK, nBetween[0], nBetween[1]);
		}
		else {
			cSql.Format("SELECT COUNT(*) FROM %s WHERE 管理No='%s' AND 検査状態_裏<>%d AND (ID BETWEEN %d AND %d)",
				DB_COIL_POSITION, m_cKizukenNo, DIV_KENSA_OK, nBetween[0], nBetween[1]);
		}
		if (!mcls_pDB->GetSelectKey(cSql, &nCnt))
		{
			LOG(em_ERR), "[%s] 対象コイルのコイル位置情報取得失敗 [%d～%d]", my_sThreadName, nBetween[0], nBetween[1]);
			LOG(em_NON), "%s", cSql);
			return false;
		}
		if (0 != nCnt)
		{
			LOG(em_WAR), "[%s] 対象コイルの範囲内に 検査異常有り [%d～%d] %d個", my_sThreadName, nBetween[0], nBetween[1], nCnt);
			return false;
		}
	}

	// ここまで来たら、両面とも全部正常だった
	return true;
}

//------------------------------------------
// 伝文へ検査実績データセット
// PRO_TYPE_ZISEKI* pTypZi 実績伝文
//------------------------------------------
bool MakeProManager::ExecDefectInf(PRO_TYPE_ZISEKI* pTypZi)
{
	CString			cSql;
	CString			cWk;
	double			dBetween[2];			// 開始、終了範囲 [10-1m]
	MIX_RATE_MAP	typMixRate;				// 混入率算出用マップ
	int				nWk;
	double			dWk;
	int				yId;
	int				gId;
	int				tId;
	int				nKensys;				// 検出システム 0:中央φω 1:出側φω 2:穴検

	double			dHosei;					// 途中で補正率が別スレッドから変更されるのを防止する為。 
	
	memset(&typMixRate, 0x00, sizeof(typMixRate));

	// 今回の対象範囲 (補正率を反映)
#ifdef PRO_SEND_ZI_HOSEI
	dHosei = m_dHosei;		// 途中で補正率が別スレッドから変更されるのを防止する為。 
	dBetween[0] = m_typBuf.data.nStart * 0.1 / dHosei;
	dBetween[1] = m_typBuf.data.nEnd   * 0.1 / dHosei;
	LOG(em_MSG), "[%s] 混入率計算 ﾌﾟﾛｺﾝ採寸開始,終了位置[%d～%d](10-1m) 補正率[%f] ", my_sThreadName, m_typBuf.data.nStart, m_typBuf.data.nEnd, dHosei);
#else
	dBetween[0] = (double)m_typBuf.data.nStart;
	dBetween[1] = (double)m_typBuf.data.nEnd;
	LOG(em_MSG), "[%s] 検査実績データ取得 ﾌﾟﾛｺﾝ採寸開始,終了位置[%d～%d](10-1m) 子番[%d]", my_sThreadName, m_typBuf.data.nStart, m_typBuf.data.nEnd, m_typBuf.data.nSubNo);
#endif // PRO_SEND_ZI_HOSEI

	// 検査実績データ 取得
	for (int ii = 0; ii<NUM_MEN; ii++) {
		cSql.Format("SELECT d.長手位置, t.プロコングループ, g.プロコングループ \
					FROM %s AS d, %s AS t, %s AS g \
					WHERE d.判定疵種=t.EdasysID AND d.判定グレード=g.EdasysID AND d.重複結果=0 AND \
					d.管理No='%s' AND t.プロコングループ<>0 AND g.プロコングループ<>0 ",
			(0 == ii ? DB_DEFECT_INF_T : DB_DEFECT_INF_B), DB_TYPE_NAME, DB_GRADE_NAME, m_cKizukenNo);

		// 実行	
		if (!mcls_pDB->ExecuteDirect(cSql)) {
			LOG(em_ERR), "[%s] 疵情報取得エラー [%s]", my_sThreadName, m_cKizukenNo);
			LOG(em_NON), "%s", cSql);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", m_cKizukenNo);
			return false;
		}

		// 取得
		while (true) {
			int sqlrc = mcls_pDB->Fetch();
			if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り

				mcls_pDB->GetData(1, &nWk);
				mcls_pDB->GetData(2, &tId);
				mcls_pDB->GetData(3, &gId);

				// データ反映
				dWk = nWk / 1000.0;			// 長手位置 [mm]→[m]
				if (dBetween[0] > dWk || dBetween[1] < dWk) continue;	// 今回の対象範囲外の疵

				yId = (int)dWk;				// [m]でのインデックス位置
				tId = tId - 1;
				gId = gId - 1;
				if (0 > yId || COIL_LEN_MAX <= yId) continue;
				if (0 > tId || MAX_PROGRP_TYPE <= tId) continue;
				if (0 > gId || MAX_PROGRP_GRADE <= gId) continue;

				// Short上限オーバーの個数は加算しない
				if (SHRT_MAX <= typMixRate.gp[gId].nCnt[tId]) continue;

				// 集計
				if (!typMixRate.gp[gId].bFlg[yId]) {	// まだ無い場合のみ
					typMixRate.gp[gId].bFlg[yId] = true;
					typMixRate.gp[gId].nMixRateCnt++;			// 1件で1mの疵混入とする
				}
				typMixRate.gp[gId].nCnt[tId] ++;

			}
			else if (sqlrc == KIZUODBC_FETCH_NODATE) {		// データ無し
				break;
			}
			else {								// エラー
				LOG(em_ERR), "[%s] FETCHエラー [%s]", my_sThreadName, m_cKizukenNo);
				syslog(SYSNO_DB_FETCH_ERR, "[%s]", m_cKizukenNo);
				return false;
			}
		}
	}

	//// 混入率を算出
	//int nYouso = (int)((m_typBuf.data.nEnd * 0.1) - (m_typBuf.data.nStart * 0.1));
	int nYouso = (int)(dBetween[1] - dBetween[0]);
	LOG(em_MSG), "[%s] 混入率計算 検査長換算値 距離[%d](m) 開始,終了位置[%f～%f](m) ", my_sThreadName, nYouso, dBetween[0], dBetween[1]);
	// 検査実績データ 実績伝文セット
	for (int ii=0; ii<MAX_PROGRP_GRADE; ii++)
	{
		// 混入率
		pTypZi->data.grade_inf[ii].nMixedRate = (short)((double)typMixRate.gp[ii].nMixRateCnt / nYouso * 100 + 0.5f);	// 混入率 [%]→[10-2%](四捨五入)
		// 件数
		for (int jj = 0; jj<MAX_PROGRP_TYPE; jj++)	pTypZi->data.grade_inf[ii].nTypeCnt[jj] = typMixRate.gp[ii].nCnt[jj];

		LOG(em_MSG), "[%s] Gr=%d, 混入率=%.03f%%, 集約個数=%d, 件数=%d,%d,%d,%d,%d %d,%d,%d,%d,%d",
			my_sThreadName, ii + 1, ((double)typMixRate.gp[ii].nMixRateCnt / nYouso), typMixRate.gp[ii].nMixRateCnt,
			typMixRate.gp[ii].nCnt[0], typMixRate.gp[ii].nCnt[1], typMixRate.gp[ii].nCnt[2], typMixRate.gp[ii].nCnt[3], typMixRate.gp[ii].nCnt[4],
			typMixRate.gp[ii].nCnt[5], typMixRate.gp[ii].nCnt[6], typMixRate.gp[ii].nCnt[7], typMixRate.gp[ii].nCnt[8], typMixRate.gp[ii].nCnt[9]);
	}
	
	return true;
}

//------------------------------------------
// プロコン用検査実績伝文 を バッファテーブルに登録
// char const* cID キー
// PRO_TYPE_ZISEKI const* pTypZi 実績伝文
//------------------------------------------
bool MakeProManager::InsertZiBuf(const char *cID, const PRO_TYPE_ZISEKI *pTypZi)
{
	CString		cSql;

	cSql.Format("INSERT INTO %s VALUES(", DB_BUF_PRO_S_ZISEKI);

	CreateSQL::sqlColumnInsert(&cSql, cID, SIZE_SERIAL_FF_NO);				// ID
	CreateSQL::sqlColumnAddTime(&cSql, true);								// 登録日付
	CreateSQL::sqlColumnInsert(&cSql, 1);									// 伝文区分(1:未送信)
	CreateSQL::sqlColumnInsert(&cSql, (char*)&m_typBuf.data.KeyInf.cMeiNo, sizeof(m_typBuf.data.KeyInf));	// キー情報
	CreateSQL::sqlColumnInsert(&cSql, m_typBuf.data.nKoban);				// 分割No
	cSql += "?, ";															// 生伝文
	CreateSQL::sqlColumnInsert(&cSql, m_cKizukenNo, SIZE_KIZUKEN_NO);		// 管理No
	CreateSQL::sqlColumnInsert(&cSql, NULL, 0);								// 送信日付
	CreateSQL::sqlColumnTrim(&cSql);
	cSql += ")";

#ifndef TEST_ONLINE_DATA
	// 登録
	if (!mcls_pDB->SqlBinalyWrite(cSql, sizeof(PRO_TYPE_ZISEKI), (BYTE*)pTypZi)) {
		LOG(em_ERR), "[%s] 検査実績登録失敗 [%s]", my_sThreadName, cID);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", DB_BUF_PRO_S_ZISEKI, cID);
		return false;
	}
#else
	// バイナリダンプ
	KizuFunction::DumpData("debag プロコン検査実績.txt", sizeof(PRO_TYPE_ZISEKI), (BYTE*)pTypZi);
	LOG(em_WAR), "[%s] テストモードにより、プロコン検査実績をダンプ出力", my_sThreadName);

#endif

	return true;
}

//------------------------------------------
// カット実績テーブル を 更新
// char const* cID キー
//------------------------------------------
bool MakeProManager::UpdateProconBuf(char const* cID)
{
	CString		cSql;

	cSql.Format("UPDATE %s SET ", DB_BUF_PRO_R_CUT);
	CreateSQL::sqlColumnUpdate(&cSql, "管理No", m_cKizukenNo, SIZE_KIZUKEN_NO); 
	cSql += "編集日付=";
	CreateSQL::sqlColumnAddTime(&cSql);							// 編集日付
	cSql += " WHERE ID='";
	cSql += cID;
	cSql += "'";

	// 登録
	if (!mcls_pDB->ExecuteDirect(cSql))
	{
		LOG(em_ERR), "[%s] カット実績 UPDATEエラー [ID=%s]", my_sThreadName, cID);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", DB_BUF_PRO_R_CUT, cID);
		return false;
	}
	return true;
}

