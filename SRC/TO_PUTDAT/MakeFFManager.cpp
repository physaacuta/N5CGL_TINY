#include "StdAfx.h"
#include "MakeFfManager.h"

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
MakeFfManager::MakeFfManager(void) :
ThreadManager("FF"),
m_bCreateOk(true),
mcls_pProMgr(NULL),
mcls_pDB(NULL)
{
	//// INIファイルから定数取込
	// コイル検索の対象期間
	m_nChkCoilDate = GetPrivateProfileInt(TO_PUTDAT, "CHKCOILDATE", 1, TASKINI_NAME);
	// コイル実績確認のリトライ回数
	m_nRetryCount = GetPrivateProfileInt(TO_PUTDAT, "RETRYCOUNT", 15, TASKINI_NAME);
	// コイル実績確認のリトライ時間
	m_nRetryWaitTime = GetPrivateProfileInt(TO_PUTDAT, "RETRYWAITTIME", 20000, TASKINI_NAME);
	// 採寸実績未受信の実績検索期間
	m_nChkUnSentDate = GetPrivateProfileInt(TO_PUTDAT, "CHKUNSENTDATE", 1, TASKINI_NAME);
	m_dHosei = 1.0;
}

//------------------------------------------
// デストラクタ
//------------------------------------------
MakeFfManager::~MakeFfManager(void)
{
}

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int MakeFfManager::ThreadFirst()
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
int MakeFfManager::ThreadLast()
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
void MakeFfManager::ThreadEvent(int nEventNo)
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
			//if(m_bCreateOk != retc)
			//{
				m_bCreateOk = retc;
				KizuFunction::SetStatus("ZI_CREATE_FF_ERR_ID", retc, false);	// シスログ表示は出来ない
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
// 処理中変数 初期化
//------------------------------------------
void MakeFfManager::ClearMember()
{
	// 処理中変数 初期化
	SAFE_DELETE(mcls_pDB);
	memset(m_cKizukenNo, 0x00, sizeof(m_cKizukenNo));
	memset(&m_typBuf, 0x00, sizeof(m_typBuf));
}

//------------------------------------------
// ＦＦサーバー検査実績 伝文生成
// char const* cID BUF_PRO_R_SAISUNのキー
// 戻り値 復帰情報
//------------------------------------------
bool MakeFfManager::Exec(char const* cID)
{
	FF_JISSEKI			typZi[2];						// FF用 疵検査実績伝文
	FF_INF				typDefect[2];					// FFに送信する可能性がある疵情報
	memset(typDefect, 0x00, sizeof(typDefect));
	int					nZiErr = 0;						// 実績作成処理エラー(0:正常 1:異常)
	int					nIsWarm = 0;					// ウォーマ材区分 (0:一般材 1:ウォーマ材)
	int					nIsResult = 0;					// コイル実績有無 (0:有り 1:無し)
	int					nIsSendOk = 0;					// 外部実績送信許可 (0:送信 1:送信しない)	※4PL対応
	int					pcode[NUM_MEN];					// 検査パターン (0オリジン)

	FF_STS_JISSEKI		typSts[2];						// FF用 検査状態伝文
	FF_STS				typPosition[2];					// コイル位置情報
	memset(typPosition, 0x00, sizeof(typPosition));

	//// 処理中変数 初期化
	ClearMember();

	//// ODBC接続クラス インスタンス生成
	mcls_pDB = new OdbcBase();
	if (!mcls_pDB->Connect(TASKINI_NAME))
	{
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "[%s]", my_sThreadName);
		syslog(356, "[ID=%s DB接続エラー]", cID);
		return false;
	}

	//// プロコン採寸実績受信バッファテーブル取得
	if (!GetBufferSai(cID))
	{
		// 取得失敗
		LOG(em_ERR), "[%s] 採寸実績受信バッファテーブルデータなし [ID=%s]", my_sThreadName, cID);
		syslog(356, "[ID=%s 採寸実績該当無し]", cID);
		return false;
	}

	//// そもそも受信した採寸実績のグラントフラグが異常
	if (PRO_GRAND_OK != m_typBuf.data.Common.nGrandFlg)
	{
		LOG(em_WAR), "[%s] 採寸実績 グラントフラグ異常 [%d]", my_sThreadName, m_typBuf.data.Common.nGrandFlg);
		syslog(356, "[ID=%s 採寸実績グラントフラグ異常]", cID);
		//nZiErr = 1;		// グラントフラグが異常時も検査実績を異常として送信する
	}

	//// コイル情報取得
	if (!GetCoilInf_KizukenNo())
	{
		LOG(em_ERR), "[%s] 該当コイルなし [コイルNo=%.12s][ID=%s]", my_sThreadName, m_typBuf.data.KeyInf.cCoilNo, cID);
		syslog(356, "[ID=%s コイル情報該当無し]", cID);
		nZiErr = 1;
	}

	//// FF送信疵情報と特定する為にパラメータを取得
	if (!ReadParam()) {
		LOG(em_ERR), "[%s] パラメータ読み込み失敗により 実績伝文作成処理なし [キー情報=%.14s]", my_sThreadName, &m_typBuf.data.KeyInf);
		syslog(356, "[管理No=%s:パラメータ取得失敗]", m_cKizukenNo);	// 8:検査実績送信伝文作成で異常。
		return false;
	}

	// この時点 nZiErr = 0 であればで対象の管理Noは決定している。⇒ m_KizukenNo にセットされているので
	// バックアップ送信処理が可能
	if( 0 == nZiErr ) {
		// 未送信のコイル実績のバックアップ送信処理
		bool retc2 = ExecUnsentResult(m_cKizukenNo);
	}


	if (0 == nZiErr)
	{
		m_nLen = 0;
		//// コイル実績が出来上がっているかチェック & 補正率算出
		// ブロックされるので注意
		if (!CheckCoilResultInsertOk(&nIsWarm, &nIsSendOk, pcode))
		{
			LOG(em_ERR), "[%s] コイル実績該当なし [コイルNo=%.12s][ID=%s]", my_sThreadName, m_typBuf.data.KeyInf.cCoilNo, cID);
			//syslog(356, "[ID=%s コイル実績該当無し]", cID);
			nIsResult = 1;
		}
	}


	//// コイル実績テーブル 更新
	if (0 == nIsResult && 0 == nZiErr)
	{
		if (!UpdateCoilResult())
		{
			LOG(em_ERR), "[%s] コイル実績テーブル(T_COIL_RESULT) 更新失敗", my_sThreadName);
		}
		else
		{
			LOG(em_MSG), "[%s] コイル実績テーブル(T_COIL_RESULT) 更新完了", my_sThreadName);
		}
	}

	//// カット実績テーブル 登録
	if (0 == nIsResult && 0 == nZiErr)
	{
		if (!InsertCutResult(cID))
		{
			LOG(em_ERR), "[%s] カット実績テーブル(T_COIL_CUT_RESULT) 登録失敗", my_sThreadName);
		}
		else
		{
			LOG(em_MSG), "[%s] カット実績テーブル(T_COIL_CUT_RESULT) 登録完了", my_sThreadName);
		}
	}

	//// 採寸実績テーブル 登録
	if (0 == nIsResult && 0 == nZiErr)
	{
		if (!InsertSaisunResult(cID))
		{
			LOG(em_ERR), "[%s] 採寸実績テーブル(T_COIL_SAISUN_RESULT) 登録失敗", my_sThreadName);
		}
		else
		{
			LOG(em_MSG), "[%s] 採寸実績テーブル(T_COIL_SAISUN_RESULT) 登録完了", my_sThreadName);
		}
	}


	//// プロコンカット実績受信バッファテーブル 更新
	if (!UpdateProconBuf(cID))
	{
		LOG(em_ERR), "[%s] プロコン採寸実績受信バッファテーブル(BUF_FF_R_SAISUN) 更新失敗", my_sThreadName);
	}
	else
	{
		LOG(em_MSG), "[%s] プロコン採寸実績受信バッファテーブル(BUF_FF_R_SAISUN) 更新完了", my_sThreadName);
	}

	//// FF送信用 実績情報の生成可能？
	if (1 == nZiErr || 1 == nIsWarm || 1 == nIsSendOk) {
		if (1 == nIsSendOk){	// 4PL対応
			LOG(em_WAR), "[%s] 外部実績送信許可が[1:送信しない]のため実績作成処理なし [管理No=%s, キー情報=%.14s]", my_sThreadName, m_cKizukenNo, &m_typBuf.data.KeyInf);
			syslog(356, "[管理No=%s 外部実績送信許可NG]", m_cKizukenNo);	// 4:検査実績伝文送信対象外です。
		} else if (1 == nIsWarm) {
			LOG(em_WAR), "[%s] ウォーマー材のため実績作成処理なし [管理No=%s, キー情報=%.14s]", my_sThreadName, m_cKizukenNo, &m_typBuf.data.KeyInf);
			syslog(356, "[管理No=%s ウォーマー材]", m_cKizukenNo);	// 4:検査実績伝文送信対象外です。
		} else {
			LOG(em_ERR), "[%s] 整合性異常のため実績作成処理なし [管理No=%s, キー情報=%.14s]", my_sThreadName, m_cKizukenNo, &m_typBuf.data.KeyInf);
		}

		return true;
	}

	//// FF送信用 実績情報生成
	for(int ii=0; ii<NUM_MEN; ii++) {
		if( ! MakeSendData( ii, pcode[ii], nIsResult, &typDefect[ii], &typZi[ii]) ) {
			//// ※実際には MakeSendData で False となる条件は存在しない
			LOG(em_ERR), "[%s] <%s> FF検査実績伝文作成失敗", my_sThreadName, DivNameManager::GetTorB(ii));
			syslog(356, "[管理No=%s:伝文作成失敗]", m_cKizukenNo);	// 8:検査実績送信伝文作成で異常。

			return false;
		}
	}
	//// FF送信用 検査状態生成
	for (int ii = 0; ii<NUM_MEN; ii++) {
		if (!MakeSendStsData(ii, pcode[ii], nIsResult, &typPosition[ii], &typSts[ii])) {
			LOG(em_ERR), "[%s] <%s> FF検査状態伝文作成失敗", my_sThreadName, DivNameManager::GetTorB(ii));
			syslog(356, "[管理No = %s]", m_cKizukenNo);				// 8:検査実績送信伝文作成で異常
			return false;
		}
	}

	// 基本となるIDNoを生成       (yyyymmdd hhnnssff\0)
	char cID_Send[SIZE_SERIAL_FF_NO];
	KizuLib::KizuFunction::GetSerialNoFF(cID_Send);	

	//// FF用検査実績伝文 を バッファテーブルに登録
	if( InsertZiBuf(cID_Send, typZi, typSts) ) {
		LOG(em_MSG), "[%s] FF検査実績送信バッファテーブル(BUF_FF_S_ZISEKI)登録完了", my_sThreadName);

	} else {
		LOG(em_ERR), "[%s] FF検査実績送信バッファテーブル(BUF_FF_S_ZISEKI)登録失敗", my_sThreadName);
		syslog(356, "[管理No=%s:登録失敗]", m_cKizukenNo);	// 8: 検査実績送信伝文作成で異常。

		return false;
	}

	//// FF送信疵情報 を次工程情報テーブルに登録
	////  次工程コイル情報テーブル(ACCESS_NEXT_COIL_INF)
	////  次工程疵情報テーブル    (ACCESS_NEXT_DEFECT_INF)
	if (InsertAccessNextInf(&typDefect[0], &typZi[0])) {
		LOG(em_MSG), "[%s] 次工程情報テーブル 登録完了", my_sThreadName);
	} else {
		LOG(em_ERR), "[%s] 次工程情報テーブル 登録失敗", my_sThreadName);
	}

	//// ＦＦサーバに実績送信要求
	Send_To_FF(cID);


	return true;
}

//------------------------------------------
// プロコン採寸実績受信バッファテーブル取得
// const char *cID キー
//------------------------------------------
bool MakeFfManager::GetBufferSai(const char *cID)
{
	//// SQL文作成
	CString				cSql;
	cSql.Format("SELECT 生伝文 FROM %s WHERE ID='%s'", DB_BUF_PRO_R_SAISUN, cID);

	// データ取得
	return mcls_pDB->GetSelectImage(cSql, sizeof(PRO_TYPE_SAISUN), (char*)&m_typBuf);
}

//------------------------------------------
// コイル情報から管理Noを取得
//------------------------------------------
bool MakeFfManager::GetCoilInf_KizukenNo()
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
		// 1件も無し
		return false;
	}
	
	LOG(em_MSG), "[%s] コイルNo=%.12s をT_COIL_INFから特定 [管理No=%s]", my_sThreadName, m_typBuf.data.KeyInf.cCoilNo, m_cKizukenNo);
	return true;
}

//------------------------------------------
// コイル実績が登録されるまで待ち。＆補正率確定
// int *nIsWarm     ウォーマ材区分 (0:一般材 1:ウォーマ材)
// 戻り値 : false:存在しない 
//------------------------------------------
bool MakeFfManager::CheckCoilResultInsertOk(int *nIsWarm, int* nIsSendOk, int* pcode)
{
	CString				cSql;
	ENUM_END_MODE		emEndMode;			// 実績格納区分
	int					nLen;				// 検査実測長 [mm]
	int					nWk;
	
	cSql.Format("SELECT 実績格納区分, ウォーマー材区分, 外部実績送信許可, \
				検査実測長, \
				検査パターンコード_表, 検査パターンコード_裏 \
				FROM %s \
				WHERE 管理No='%s'", DB_COIL_RESULT, m_cKizukenNo);

	//// コイル実績のデータ有無確認
	for (int ii=0; ii<m_nRetryCount; ii++)
	{
		if (!mcls_pDB->ExecuteDirect(cSql))
		{
			LOG(em_ERR), "[%s] コイル実績 取得エラー [管理No=%s]", my_sThreadName, m_cKizukenNo);
			LOG(em_NON), "%s", cSql);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", DB_COIL_RESULT, m_cKizukenNo);
			return false;
		}
		int sqlrc = mcls_pDB->Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK)						// データ有り
		{
			mcls_pDB->GetData(1, (int *)&emEndMode);		// 実績格納区分
			mcls_pDB->GetData(2, nIsWarm);					// ウォーマ材区分
			mcls_pDB->GetData(3, nIsSendOk);				// 外部実績送信許可
			mcls_pDB->GetData(4, &nLen);					// 検査実測長
			mcls_pDB->GetData(5, &nWk);						// 検査パターンコード_表
			pcode[0] = nWk - 1;								// (0オリジン)
			mcls_pDB->GetData(6, &nWk);						// 検査パターンコード_裏
			pcode[1] = nWk - 1;								// (0オリジン)

			m_nLen = nLen;									// 検査実績、検査状態伝文作成時に使用。異常時は前回値使用
			break;
		}
		else if (sqlrc == KIZUODBC_FETCH_NODATE)			// データ無し
		{
			// 最後まで コイル実績が見つからない
			if (ii >= m_nRetryCount - 1)
			{
				LOG(em_ERR), "[%s] コイル実績 無し [管理No=%s]", my_sThreadName, m_cKizukenNo);
				syslog(356, "コイル実績無し [コイルNo=%.12s][管理No=%s]", m_typBuf.data.KeyInf.cCoilNo, m_cKizukenNo);
				return false;
			}
			LOG(em_WAR), "[%s] コイル実績 検索中 回数: %d", my_sThreadName, ii + 1);
			Sleep(m_nRetryWaitTime);
		}
		else												// エラー
		{
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", m_cKizukenNo);
			return false;
		}
	}

	//------------------------------------------
	// 補正率計算
	//------------------------------------------
	double	dWk;
	double 	dHani;						// プロコンコイル長比較異常上限値 [%]
	////m_dHosei = 1.0;						// 補正値デフォルトをセット ※補正率異常時は前回値を使用
	double	dHosei;						// 補正率		

	cSql.Format("SELECT プロコンコイル長比較異常上限値 FROM %s", DB_COMMON_MASTER);
	if( ! mcls_pDB->ExecuteDirect(cSql)) {
		LOG(em_ERR), "[%s] 比較異常上限値 取得エラー [管理No=%s]", my_sThreadName, m_cKizukenNo);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", DB_COMMON_MASTER, m_cKizukenNo);
		return true;
	}
	int sqlrc = mcls_pDB->Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り

		mcls_pDB->GetData( 1, &dHani);					// プロコンコイル長比較異常上限値

	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
		LOG(em_ERR), "[%s] 比較異常上限値 無し [%.20s]", my_sThreadName, m_cKizukenNo);
		LOG(em_NON), "%s", cSql);
		return true;
	} else {								// エラー
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", m_cKizukenNo);
		return true;
	}

	//------------------------------------------
	// PLG距離比較異常上限値[%] 0の場合、補正率計算を行わない
	//------------------------------------------
	if (0 == dHani) {
		LOG(em_MSG), "[%s] 補正率計算 未実施 [PLG距離比較異常上限値=0]", my_sThreadName);
		syslog(353, "[管理No=%s]", m_cKizukenNo);
		// 機器状態を正常へ更新する
		KizuFunction::SetStatus("HOSEIRITU_ID", true, false);
		return true;
	}

	//------------------------------------------
	// プロコン実測長チェック
	//------------------------------------------
	if (0 >= m_typBuf.data.nLen)
	{
		LOG(em_ERR), "[%s] プロコン実測長 整合性異常 [%.1fm][管理No=%s]", my_sThreadName, (double)m_typBuf.data.nLen / 10.0, m_cKizukenNo);
		syslog(351, "[プロコン長=%.1fm][コイルNo=%.12s][管理No=%s]", (double)m_typBuf.data.nLen / 10.0, m_typBuf.data.KeyInf.cCoilNo, m_cKizukenNo);
		// 機器状態を異常へ更新する
		KizuFunction::SetStatus("HOSEIRITU_ID", false, false);
		return true;
	}

	//------------------------------------------
	// 補正率計算
	//------------------------------------------
	// 検査実測長チェック
	if (0 >= m_nLen)
	{
		LOG(em_ERR), "[%s] 検査実測長 整合性異常 [%.3fm][管理No=%s]", my_sThreadName, (double)m_nLen / 1000.0, m_cKizukenNo);
		syslog(352, "[検査実測長=%.3fm][コイルNo=%.12s][管理No=%s]", (double)m_nLen / 1000.0, m_typBuf.data.KeyInf.cCoilNo, m_cKizukenNo);
		// 実績格納区分が 通常切替 の場合のみ
		if (DIV_END_NORMAL == emEndMode)
		{
			// 機器状態を異常へ更新する
			KizuFunction::SetStatus("HOSEIRITU_ID", false, false);
		}
		//return true;
	}
	else
	{
		// 補正率計算
		dHosei = (double)m_typBuf.data.nLen * 100.0 / (double)m_nLen;

		// 補正率計算結果差分の絶対値(%)を取得
		if (1.0 < dHosei)
		{
			dWk = (dHosei - 1.0) * 100.0;
		}
		else
		{
			dWk = (1.0 - dHosei) * 100.0;
		}

		//------------------------------------------
		// 補正率範囲判定
		//------------------------------------------
		if (0 != dHani && dHani < dWk)
		{
			// 範囲外
			LOG(em_ERR), "[%s] 補正率計算 異常 [補正率(%f) = プロコン長=%.1fm / 検査実測長=%.3fm]", my_sThreadName, dHosei, (double)m_typBuf.data.nLen / 10.0, (double)m_nLen / 1000.0);
			syslog(351, "[%lf%% = プロコン長=%.1fm / 検査実測長=%.3fm][コイルNo=%.12s][管理No=%s]", dHosei*100, (double)m_typBuf.data.nLen / 10.0, (double)m_nLen / 1000.0, m_typBuf.data.KeyInf.cCoilNo, m_cKizukenNo);
			// 実績格納区分が 通常切替 の場合のみ
			if (DIV_END_NORMAL == emEndMode)
			{
				// 機器状態を異常へ更新する
				KizuFunction::SetStatus("HOSEIRITU_ID", false, false);
			}
		}
		else
		{
			// 範囲内
			m_dHosei = dHosei;
			LOG(em_MSG), "[%s] 補正率計算 [補正率(%f) = プロコン長=%.1fm / 検査実測長=%.3fm]", my_sThreadName, m_dHosei, (double)m_typBuf.data.nLen / 10.0, (double)m_nLen / 1000.0);
			syslog(350, "[%lf%% = プロコン長=%.1fm / 検査実測長=%.3fm][コイルNo=%.12s][管理No=%s]", m_dHosei*100, (double)m_typBuf.data.nLen / 10.0, (double)m_nLen / 1000.0, m_typBuf.data.KeyInf.cCoilNo, m_cKizukenNo);
			// プロコン用のクラスにもセットしてあげる
			mcls_pProMgr->SetHosei(m_dHosei);
			// 機器状態を正常へ更新する
			KizuFunction::SetStatus("HOSEIRITU_ID", true, false);
		}
	}

	return true;
}



//------------------------------------------
// パラメータ読込
//------------------------------------------
bool MakeFfManager::ReadParam()
{
	int id;
	int typid;
	int no;
	CString				cSql;


	//===========================
	// PARAM_PCODE & PCOMMON_OUTPUT
	//===========================
	PARAM_PCODE	buf;
	memset(&buf, 0x00, sizeof(buf));

	//// PARAM_PCODE
	cSql = "SELECT pcode, 疵種EdasysID, FF送信有無 ";
	cSql += "FROM ";
	cSql += DB_PCODE_OUTPUT;
	cSql += " ORDER BY pcode ASC, 疵種EdasysID ASC";

	if (!mcls_pDB->ExecuteDirect(cSql)) {
		LOG(em_ERR), "[%s] [%s SELECTエラー]", my_sThreadName, DB_PCODE_OUTPUT);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", DB_PCODE_OUTPUT);
		return false;
	}
	while (true) {
		int sqlrc = mcls_pDB->Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り

			// 主キー
			mcls_pDB->GetData(1, &id);						// pcode
			
			if (MAX_PCODE < id || 0 >= id) {
				// 主キー不正
				LOG(em_ERR), "[%s] [%s 主キー不正 id=%d]", my_sThreadName, DB_PCODE_OUTPUT, id);
				syslog(SYSNO_DB_DATA_ERR, "[%s id=%d]", DB_PCODE_OUTPUT, id);
				continue;
			}
			mcls_pDB->GetData(2, &typid);					// 疵種EdasysID
			if (-1 == typid) {
				// デフォルト
				buf.rec[id-1].typDefput.nEdasysID = typid;
				mcls_pDB->GetData(3, (int*)&buf.rec[id-1].typDefput.nKind);	// FF送信有無
			} else {
				no = buf.rec[id-1].nPutNum;
				buf.rec[id-1].typOutput[no].nEdasysID = typid;
				mcls_pDB->GetData(3, (int*)&buf.rec[id-1].typOutput[no].nKind);	// FF送信有無
				buf.rec[id-1].nPutNum = no + 1;
			}
		} else if (sqlrc == KIZUODBC_FETCH_NODATE) {		// データ無し
			break;
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", m_cKizukenNo);
			return false;
		}
	}

	// 取得データをメンバー変数にセット
	memcpy(&m_Param_Pcode, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, DB_PCODE_OUTPUT);

	//===========================
	// PARAM_GRADE
	//===========================
	PARAM_GRADE	grade;
	memset(&grade, 0x00, sizeof(grade));

	cSql =  "SELECT";
	cSql += " EdasysID, 優先度, 欠陥グレード";
	cSql += " FROM ";
	cSql += DB_GRADE_NAME;
	cSql += " ORDER BY 欠陥グレード DESC, 優先度 ASC";

	if( ! mcls_pDB->ExecuteDirect(cSql)) {
		LOG(em_ERR), "[%s] [%s SELECTエラー]", my_sThreadName, DB_GRADE_NAME);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", DB_GRADE_NAME);
		return false;
	}
	for(int ii = 0; ii < MAX_GRADE; ii++){
		int sqlrc = mcls_pDB->Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り

			// 主キー
			mcls_pDB->GetData(1, &id);						// EdasysID
			if( 0 >= id ) {
				// 主キー不正
				LOG(em_ERR), "[%s] [%s 主キー不正 EdasysID=%d]", my_sThreadName, DB_GRADE_NAME, id);
				syslog(SYSNO_DB_DATA_ERR, "[%s EdasysID=%d]", DB_GRADE_NAME, id);					
				continue;
			}
			
			// データ部
			grade.typGrade_id[ii].nEdasysID = id;							// EdasysID
			mcls_pDB->GetData(2, &grade.typGrade_id[ii].nPriority );		// 優先度
			mcls_pDB->GetData(3, &grade.typGrade_id[ii].nRank);				// 欠陥グレード

			grade.nNum++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", DB_GRADE_NAME);
			return false;
		}
	}

	// 取得データをメンバー変数にセット
	memcpy( &m_Param_Grade, &grade, sizeof(grade));
	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, DB_GRADE_NAME);

	//===========================
	// PARAM_COMMON_MAST
	//===========================
	PARAM_COMMON_MAST	common;
	memset ( &common, 0x00, sizeof(common));

	common.nEdgeNgCount = 0;
	cSql =  "SELECT";
	cSql += " エッジ整合性_エッジ検出異常FF送信";
	cSql += " FROM ";
	cSql += DB_COMMON_MASTER;

	if( ! mcls_pDB->ExecuteDirect(cSql)) {
		LOG(em_ERR), "[%s] [%s SELECTエラー]", my_sThreadName, DB_COMMON_MASTER);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", DB_COMMON_MASTER);
		return false;
	}

	int sqlrc = mcls_pDB->Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り
	
		// データ部
		mcls_pDB->GetData(1, &common.nEdgeNgCount);

	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
		return false;	
	} else {								// エラー
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", DB_COMMON_MASTER);
		return false;
	}

	// 取得データをメンバー変数にセット
	memcpy( &m_Param_CommonMast, &common, sizeof(common));
	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, DB_COMMON_MASTER);
	return true;
}



//------------------------------------------
// FF送信用 実績伝文生成
// int ou 0:表
// int nIsResult コイル実績有無 (0:有り 1:無し)
// FF_INF * pInf 編集ワーク
// FF_JISSEKI* pTypZi 各面のみの実績伝文
// 戻り値 : 復帰情報
//------------------------------------------
bool MakeFfManager::MakeSendData(int ou, int pcode, int nIsResult, FF_INF* pInf, FF_JISSEKI* pTypZi)
{
	CString cWk;
	int		nWk;

	//// 初期化
	memset( pTypZi, 0x20, sizeof(FF_JISSEKI));


	//===========================
	// ヘッダー部
	//===========================
	if(0 == ou) memcpy( pTypZi->Hd.Header_Code, FF_TC_CODE_JI_O, sizeof(pTypZi->Hd.Header_Code) );
	else		memcpy( pTypZi->Hd.Header_Code, FF_TC_CODE_JI_U, sizeof(pTypZi->Hd.Header_Code) );
	memcpy(pTypZi->Hd.Rec_Code, Z_FF_RENBAN, sizeof(pTypZi->Hd.Rec_Code));

	// 送信時刻
	SYSTEMTIME STime;
	GetLocalTime(&STime);
	cWk.Format("%04d%02d%02d%02d%02d%02d", STime.wYear, STime.wMonth, STime.wDay, STime.wHour, STime.wMinute, STime.wSecond); 
	memcpy(pTypZi->Hd.Date, cWk, sizeof(pTypZi->Hd.Date));

	// 工程コード ( 5CGL=85 )
	memcpy(pTypZi->Hd.Line_Code, KOUTEI_CODE, sizeof(pTypZi->Hd.Line_Code));

	// 命令番号
	memcpy(pTypZi->Hd.Meino, &m_typBuf.data.KeyInf.cMeiNo, sizeof(pTypZi->Hd.Meino));

	// 冷延コイル番号
	//memcpy(pTypZi->Hd.Coilno, &m_typBuf.data.KeyInf.cCoilNo, sizeof(pTypZi->Hd.Coilno));
	if ('-' == m_typBuf.data.KeyInf.cCoilNo[1]) {
		// コイルNoの'-'は削除してFF送信
		pTypZi->Hd.Coilno[0] = m_typBuf.data.KeyInf.cCoilNo[0];
		memcpy(&pTypZi->Hd.Coilno[1], &m_typBuf.data.KeyInf.cCoilNo[2], sizeof(pTypZi->Hd.Coilno) - 2);
	}
	else {
		memcpy(pTypZi->Hd.Coilno, &m_typBuf.data.KeyInf.cCoilNo, sizeof(pTypZi->Hd.Coilno));
	}


 	//===========================
	// コイル情報部
	//===========================
	// コイル長さ(右詰, 0パディング)

	// コイル長上限チェック
	nWk = m_typBuf.data.nLen;
	if ( (COIL_LEN_MAX * 10) < m_typBuf.data.nLen) {
		// 入側コイル長MAX[10-1m] にて設定
		nWk = COIL_LEN_MAX * 10;
		LOG(em_ERR), "[%s] <%s> コイル長上限範囲外 プロコン長=%d[m]", my_sThreadName, DivNameManager::GetTorB(ou), nWk);
	}
	cWk.Format("%06d", nWk);
	memcpy(pTypZi->CoilLen, cWk, sizeof(pTypZi->CoilLen));


	// 疵検処理モード(2桁目スペース)
	if (0 != m_typBuf.data.Common.nGrandFlg)
	{
		// 不信頼フラグがNGの場合もNGとして送信する。
		nWk = Z_FF_KMODE_NG;
	}
	else
	{
		if (0 != nIsResult) {			// コイル実績無し =疵検非稼動+疵検停止←自動で無いことは多分無いので、手動にしておく
			nWk = Z_FF_KMODE_STOP;
		}
		else {
			// 状態取得
			nWk = GetStatus_Kensa();				// coil_result
			nWk = GetStatus_EdgeDetect(ou, nWk);	// EdgeCoilInf
		}
	}
	cWk.Format("%01d  ", nWk);
	memcpy(pTypZi->Mode, cWk, sizeof(pTypZi->Mode));
	LOG(em_INF), "[%s] <%s> 疵検処理モード=%d", my_sThreadName, DivNameManager::GetTorB(ou), nWk);

	// 管理No
	if(0 == nIsResult) memcpy(pTypZi->Id, m_cKizukenNo, strlen(m_cKizukenNo));		// 管理Noの最後のNULLは、コピーしたくないため

	// これ以外のコイル情報部は、これ以降の処理で順次確定させていく


 	//===========================
	// 疵情報部
	//===========================
	if(0 == nIsResult) {			// コイル実績がある場合のみ (検査パターンが不明となるため、送信すべき疵実績が特定できない)
		// 疵情報を取得する
		if( ! GetData_DefectInf(ou, pcode, pInf) ) {
			LOG(em_ERR), "[%s] <%s> 疵情報取得失敗", my_sThreadName, DivNameManager::GetTorB(ou));
			pInf->nDefCnt = 0;		// 念の為に無かったことにしておく
		}
	}

	//// カット情報は最優先の為、一番最初に実績付与しておく
	MakeCutData(ou, pInf, pTypZi);

	//// 疵情報エリアにデータセット
	MakeDefectData(ou, pInf, pTypZi);

	return true;
}
//==========================================
// 対象コイルの疵情報を取得する
//------------------------------------------
// int		ou		: 表裏 (0:表 1:裏)
// int      pcode   : 検査パターン (0オリジン)
// FF_INF*	pInf	: 編集ワーク
// 戻値		bool	: 復帰情報
//==========================================
bool MakeFfManager::GetData_DefectInf(int ou, int pcode, FF_INF* pInf)
{
	int					rec;				// 疵情報レコード位置(0オリジン)
	int					gid;				// 判定グレード
	int					tid;				// 判定疵種
	int					nPos;				// 長手位置
	int					no;					// 疵No
	int					ws;					// WS位置
	int					gFF;				// FF伝送名称(グレード)
	char				tFF[3];				// FF伝送名称(疵種)
	int					len;				// 疵長さ
	int					nRank;				// 欠陥グレード
	int					nGPriority;			// グレード優先度
	int					nTPriority;			// 疵種優先度
	int					nKensys;			// 検出システム 0:中央φω 1:出側φω 2:穴検


	int					nSendCnt = 0;		// 送信対象の個数

	int					ii;
	CString				cSql;
	CString				cWk;

	cSql = "SELECT d.判定グレード, d.判定疵種, d.長手位置, ";				// 必須
	cSql += "g.欠陥グレード, g.優先度, t.優先度, d.疵No, d.WS位置, g.FF伝送名称, t.FF伝送名称 ";
	cSql += ", d.疵長さ ";
	cSql += "FROM ";
	cSql += DB_COIL_RESULT;
	cSql += " r, ";
	cSql += (0 == ou) ? DB_DEFECT_INF_T : DB_DEFECT_INF_B;
	cSql += " d, ";
	cSql += DB_TYPE_NAME;
	cSql += " t, ";
	cSql += DB_GRADE_NAME;
	cSql += " g ";
	cSql += "WHERE ";
	cSql += "r.管理No = d.管理No AND d.判定疵種=t.EdasysID AND d.判定グレード=g.EdasysID AND d.重複結果=0 AND ";
	cSql += "r.管理No = '";
	cSql += m_cKizukenNo;
	cSql += "' ";
	cSql += "ORDER BY g.欠陥グレード DESC, g.優先度 ASC, t.優先度 ASC, d.長手位置 ASC";

	//LOG(em_NON), "%s", cSql);
	if( ! mcls_pDB->ExecuteDirect(cSql)) {
		LOG(em_ERR), "[%s] 疵情報取得エラー [%s]", my_sThreadName, m_cKizukenNo);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", m_cKizukenNo);
		return false;
	}
	while(true) {
		int sqlrc = mcls_pDB->Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り
				
			mcls_pDB->GetData(1,  &gid);				// DB_DEFECT_INF_T/B : 判定グレード
			mcls_pDB->GetData(2,  &tid);				// DB_DEFECT_INF_T/B : 判定疵種
			mcls_pDB->GetData(3,  &nPos);				// DB_DEFECT_INF_T/B : 長手位置
			mcls_pDB->GetData(4,  &nRank);				// DB_GRADE_NAME     : 欠陥グレード
			mcls_pDB->GetData(5,  &nGPriority);			// DB_GRADE_NAME	 : グレード優先度
			mcls_pDB->GetData(6,  &nTPriority);			// DB_TYPE_NAME		 : 疵種優先度
			mcls_pDB->GetData(7,  &no);					// DB_DEFECT_INF_T/B : 疵No
			mcls_pDB->GetData(8,  &ws);					// DB_DEFECT_INF_T/B : WS位置
			mcls_pDB->GetData(9,  &gFF);				// DB_GRADE_NAME     : FF伝送名称
			mcls_pDB->GetData(10, &tFF, sizeof(tFF));	// DB_TYPE_NAME      : FF伝送名称
			mcls_pDB->GetData(11, &len);				// DB_DEFECT_INF_T/B : 疵長さ

			// FF送信対象?
			if (!Check_Output(pcode, tid, gid)) continue;

			// 成品の範囲内？
			nPos = (int)(nPos * m_dHosei);					// 補正率を反映させる
			for (ii = 0; ii<m_typBuf.data.nSetCnt; ii++) {
				if (m_typBuf.data.saisun[ii].nStart * 100 <= nPos && m_typBuf.data.saisun[ii].nEnd * 100 >= nPos) break;		// [m] → [0.1m]対応
			}
			if (ii == m_typBuf.data.nSetCnt) {
				//LOG(em_MSG), "対象外の疵 %dmm", nPos);			
				continue;	// 範囲外(=カットされた部分)	
			}

			if (MAX_FF_DFCT_CNT <= pInf->nDefCnt) {
				pInf->bOverStatus = true;		// 疵情報だけでFF送信のMAXに到達。後は削除していくだけ
				break;
			}

			// 編集ワークにセット
			rec = pInf->nDefCnt;

			pInf->typDef[rec].nGid       = gid;
			pInf->typDef[rec].nTid       = tid;
			pInf->typDef[rec].nYpos      = nPos;
			pInf->typDef[rec].nRank		 = nRank;
			pInf->typDef[rec].nGPriority = nGPriority;
			pInf->typDef[rec].nTPriority = nTPriority;
			pInf->typDef[rec].nNo        = no;
			pInf->typDef[rec].nWS        = ws;
			pInf->typDef[rec].nGFF       = gFF;
			memcpy(&pInf->typDef[rec].tFF, tFF, sizeof(pInf->typDef[rec].tFF));
			pInf->typDef[rec].nLen       = len;

			pInf->nDefCnt ++;
			nSendCnt ++;


		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", m_cKizukenNo);
			return false;
		}
	}

	LOG(em_MSG), "[%s] <%s> FF送信対象 送信対象数(%d) pcode=%d", my_sThreadName, DivNameManager::GetTorB(ou), nSendCnt, pcode + 1);
	return true;
}
//------------------------------------------
// 対象コイルの検査状態(疵検の稼動状況)を取得する
// 戻り値 : 対象コイルの検査状態
//------------------------------------------
int MakeFfManager::GetStatus_Kensa()
{
	CString				cSql;
	int					nKensa[2];		// 検査状態
	int					nEndMode;		// 実績格納区分

	cSql.Format("SELECT 検査状態_表, 検査状態_裏, 実績格納区分 FROM %s WHERE 管理No='%s'", DB_COIL_RESULT, m_cKizukenNo); 
	// 実行	
	if( ! mcls_pDB->ExecuteDirect(cSql)) {
		LOG(em_ERR), "[%s] 検査状態取得失敗 [%s]", my_sThreadName, m_cKizukenNo);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s: キー情報=%.16s]", DB_COIL_RESULT, m_cKizukenNo);
		return Z_FF_KMODE_NG;
	}
	// 取得
	int sqlrc = mcls_pDB->Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り

		mcls_pDB->GetData(1, &nKensa[0]);				// 検査状態_表
		mcls_pDB->GetData(2, &nKensa[1]);				// 検査状態_裏
		mcls_pDB->GetData(3, &nEndMode);				// 実績格納区分

	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
		LOG(em_ERR), "[%s] 検査状態取得失敗", my_sThreadName);
		return Z_FF_KMODE_NG;				// 有り得ない
	} else {								// エラー
		LOG(em_ERR), "[%s] 検査状態取得失敗", my_sThreadName);
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", m_cKizukenNo);
		return Z_FF_KMODE_NG;				// 有り得ない
	}


	//// 処理モード判別
	int		nAns = 0;
	if( DIV_KENSA_OK == nKensa[0] && DIV_KENSA_OK == nKensa[1]) {
		nAns = Z_FF_KMODE_OK;						// 両面とも正常 (= 両面正常)

	} else if( DIV_KENSA_OK == nKensa[0] ) {	
		nAns = Z_FF_KMODE_OK_O;						// 表面のみ正常 (= 裏面縮退)

	} else if( DIV_KENSA_OK == nKensa[1] ) {
		nAns = Z_FF_KMODE_OK_U;						// 裏面のみ正常 (= 表面縮退)

	} else {
		nAns = Z_FF_KMODE_NG;						// 両面とも正常じゃない (= 両面縮退)
	}

	// 停止は最優先
	if( DIV_END_STOP_MANUAL == nEndMode || DIV_END_STOP_AUTO == nEndMode ) {			// 自動停止 or 手動疵検停止
		nAns = Z_FF_KMODE_STOP;
	}
	return nAns;
}
//------------------------------------------
// 対象コイルのエッジ検出状況を取得する
// int ou 0:表
// int nState 先の状態
// 戻り値 : 対象コイルの検査状態
//------------------------------------------
int MakeFfManager::GetStatus_EdgeDetect(int ou, int nState)
{
	//// 正常の時にしか「エッジ未検出区間有り」のチェックをしない
	if(Z_FF_KMODE_OK != nState) return nState;
	if( 0 == m_Param_CommonMast.nEdgeNgCount ) return nState;

	//// リカバリーモードになった件数を取得
	CString				cSql;
	int					nNgCount;		// リカバリーモードになった件数
	int					nLostCnt[2];	// エッジ検出失敗個数 (左,右)
	cSql.Format("SELECT 板幅許容範囲外数, エッジ検出失敗個数_左, エッジ検出失敗個数_右 FROM %s WHERE 管理No='%s' AND 表裏区分=%d", DB_EDGE_COIL_INF, m_cKizukenNo, ou); 

	if( ! mcls_pDB->ExecuteDirect(cSql)) {
		LOG(em_ERR), "[%s] エッジ検出状態取得失敗(%s) [%s]", my_sThreadName, DivNameManager::GetTorB(ou), m_cKizukenNo);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", m_cKizukenNo);
		return nState;
	}
	// 取得
	int sqlrc = mcls_pDB->Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り

		mcls_pDB->GetData(1, &nNgCount);				// 板幅許容範囲外数
		mcls_pDB->GetData(2, &nLostCnt[0]);				// エッジ検出失敗個数_左
		mcls_pDB->GetData(3, &nLostCnt[1]);				// エッジ検出失敗個数_右

	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
		LOG(em_ERR), "[%s] エッジ検出状態取得無し(%s) [%s]", my_sThreadName, DivNameManager::GetTorB(ou), m_cKizukenNo);
		return nState;			// 有り得ない
	} else {								// エラー
		LOG(em_ERR), "[%s] エッジ検出状態取得失敗(%s) [%s]", my_sThreadName, DivNameManager::GetTorB(ou), m_cKizukenNo);
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", m_cKizukenNo);
		return nState;			// 有り得ない
	}

	//// 総合判定
	if(m_Param_CommonMast.nEdgeNgCount <= nNgCount ||
	   m_Param_CommonMast.nEdgeNgCount <= nLostCnt[0] ||
	   m_Param_CommonMast.nEdgeNgCount <= nLostCnt[1]) {
		nState = Z_FF_KMODE_EDGENG;
	}
	return nState;
}
//------------------------------------------
// 出力対象の欠陥か判定
// long pcode	検査パターン (0オリジン)
// long tid		疵種EdasysID
// long gid		グレードEdasysID
// 戻り値 true:対象
//------------------------------------------
bool MakeFfManager::Check_Output(long pcode, long tid, long gid) const
{
	// 引数チェック
	if (0 > pcode || MAX_PCODE <= pcode) {
		LOG(em_ERR), "[%s] pcodeNG=%d", my_sThreadName, pcode+1);
		return false;
	}

	//// 疵種EdasysID 指定有り？
	for (int ii = 0; ii<m_Param_Pcode.rec[pcode].nPutNum; ii++) {
		if (tid == m_Param_Pcode.rec[pcode].typOutput[ii].nEdasysID) {
			// Gr設定無しの疵種の為 全部対象外
			if (0 == m_Param_Pcode.rec[pcode].typOutput[ii].nKind) return false;

			// Gr設定有りの疵種の為 優先度比較
			if (GetYusenG(gid) <= GetYusenG(m_Param_Pcode.rec[pcode].typOutput[ii].nKind))	return true;
			else																			return false;
		}
	}

	//// 無いときは デフォルト指定 (ココまで来た場合は、疵種設定無しの時)
	// Gr設定無しの疵種の為 全部対象外
	if (0 == m_Param_Pcode.rec[pcode].typDefput.nKind) return false;

	// Gr設定有りの疵種の為 優先度比較
	if (GetYusenG(gid) <= GetYusenG(m_Param_Pcode.rec[pcode].typDefput.nKind))	return true;
	else																	return false;
}
//------------------------------------------
// グレード優先度取得
// long gid グレードEdasysID
// 戻り値 優先度 (未検出-1)
//------------------------------------------
int MakeFfManager::GetYusenG(long gid) const
{
	for(int ii=0; ii<m_Param_Grade.nNum; ii++) {
		if( m_Param_Grade.typGrade_id[ii].nEdasysID == gid ) return m_Param_Grade.typGrade_id[ii].nPriority;
	}
	return -1;
}
//------------------------------------------
// カット情報を FF実績にセットする
// int ou 0:表
// FF_INF * pInf 編集ワーク
// FF_JISSEKI* pTypZi FF実績伝文
//------------------------------------------
void MakeFfManager::MakeCutData(int ou, FF_INF * pInf, FF_JISSEKI* pTypZi)
{
	int nCutCount = 0;				// カット回数
	int cnt;
	CString cWk;


	//// 溶接巻き込まれがある場合は、カット情報を作成
	// 溶接巻き込まれ時は、プロコン側は、カット情報として作ってくれてない為の処置
	if (0 < m_typBuf.data.nMakiLenF) {
		LOG(em_INF), "[%s] <%s> 溶接巻き込まれ有り %.1fm", my_sThreadName, DivNameManager::GetTorB(ou), (m_typBuf.data.nMakiLenF/10.0));

		// 溶接巻き込まれをセット
		cnt = pInf->nFfCnt;
		memcpy(pTypZi->Dt[cnt].Kizukbn, Z_FF_DFCT_MAKI, sizeof(pTypZi->Dt[cnt].Kizukbn));	// 疵検出区分  (溶接巻き込まれ)
		memset(pTypZi->Dt[cnt].Kind, ' ', sizeof(pTypZi->Dt[cnt].Kind));					// 疵種(2桁スペース)
		memset(pTypZi->Dt[cnt].Grade, '0', sizeof(pTypZi->Dt[cnt].Grade));					// 疵グレード(2桁0設定)
		memset(pTypZi->Dt[cnt].WidPos, '0', sizeof(pTypZi->Dt[cnt].WidPos));				// 疵巾方向位置(4桁0設定)
		memset(pTypZi->Dt[cnt].Len, '0', sizeof(pTypZi->Dt[cnt].Len));						// 疵長さ(4桁0設定)
		cWk.Format("%06d", m_typBuf.data.nMakiLenF);										// 溶接巻き込まれ長さをセット [m] → [0.1m]対応
		memcpy(pTypZi->Dt[cnt].DefectPos, cWk, sizeof(pTypZi->Dt[cnt].DefectPos));			// 疵総長さ方向位置(右詰, 0パディング)
		memset(pTypZi->Dt[cnt].kbn, ' ', sizeof(pTypZi->Dt[cnt].kbn));						// 疵区分(スペース)
		memset(pTypZi->Dt[cnt].No, '0', sizeof(pTypZi->Dt[cnt].No));						// 疵No (0)
		memset(pTypZi->Dt[cnt].OpeDsp, ' ', sizeof(pTypZi->Dt[cnt].OpeDsp));				// オペガイ(スペース)
		memset(pTypZi->Dt[cnt].SysKbn, ' ', sizeof(pTypZi->Dt[cnt].SysKbn));				// システム区分(スペース)

		pInf->nFfCnt++;


		// カット情報をセット
		cnt = pInf->nFfCnt;
		memcpy(pTypZi->Dt[cnt].Kizukbn, Z_FF_DFCT_CUT, sizeof(pTypZi->Dt[cnt].Kizukbn));	// 疵検出区分  (カット)
		memset(pTypZi->Dt[cnt].Kind, ' ', sizeof(pTypZi->Dt[cnt].Kind));					// 疵種(2桁スペース)
		memset(pTypZi->Dt[cnt].Grade, '0', sizeof(pTypZi->Dt[cnt].Grade));					// 疵グレード(2桁0設定)
		memset(pTypZi->Dt[cnt].WidPos, '0', sizeof(pTypZi->Dt[cnt].WidPos));				// 疵巾方向位置(4桁0設定)
		memset(pTypZi->Dt[cnt].Len, '0', sizeof(pTypZi->Dt[cnt].Len));						// 疵長さ(4桁0設定)
		cWk.Format("%06d", m_typBuf.data.nMakiLenF);										// 溶接巻き込まれ長さをセット [m] → [0.1m]対応
		memcpy(pTypZi->Dt[cnt].DefectPos, cWk, sizeof(pTypZi->Dt[cnt].DefectPos));			// 疵総長さ方向位置(右詰, 0パディング)
		memset(pTypZi->Dt[cnt].kbn, ' ', sizeof(pTypZi->Dt[cnt].kbn));						// 疵区分(スペース)
		memset(pTypZi->Dt[cnt].No, '0', sizeof(pTypZi->Dt[cnt].No));						// 疵No (0)
		memset(pTypZi->Dt[cnt].OpeDsp, ' ', sizeof(pTypZi->Dt[cnt].OpeDsp));				// オペガイ(スペース)
		memset(pTypZi->Dt[cnt].SysKbn, ' ', sizeof(pTypZi->Dt[cnt].SysKbn));				// システム区分(スペース)

		pInf->nFfCnt++;
		nCutCount++;
	}

	//// 通常のカット情報を生成
	for (int ii = 0; ii<m_typBuf.data.nSetCnt; ii++) {
		// End(=ファーストカット)位置とコイル長が同じ場合は、カットでは無く、まだ次コイルへと続いている
		if (m_typBuf.data.saisun[ii].nEnd == m_typBuf.data.nLen) continue;


		// カット情報をセット
		cnt = pInf->nFfCnt;
		memcpy(pTypZi->Dt[cnt].Kizukbn, Z_FF_DFCT_CUT, sizeof(pTypZi->Dt[cnt].Kizukbn));	// 疵検出区分  (カット)
		memset(pTypZi->Dt[cnt].Kind, ' ', sizeof(pTypZi->Dt[cnt].Kind));					// 疵種(2桁スペース)
		memset(pTypZi->Dt[cnt].Grade, '0', sizeof(pTypZi->Dt[cnt].Grade));					// 疵グレード(2桁0設定)
		memset(pTypZi->Dt[cnt].WidPos, '0', sizeof(pTypZi->Dt[cnt].WidPos));				// 疵巾方向位置(4桁0設定)
		memset(pTypZi->Dt[cnt].Len, '0', sizeof(pTypZi->Dt[cnt].Len));						// 疵長さ(4桁0設定)
		cWk.Format("%06d", m_typBuf.data.saisun[ii].nEnd);									// カット位置 Fカット位置をセット [m] → [0.1m]対応
		memcpy(pTypZi->Dt[cnt].DefectPos, cWk, sizeof(pTypZi->Dt[cnt].DefectPos));			// 疵総長さ方向位置(右詰, 0パディング)
		memset(pTypZi->Dt[cnt].kbn, ' ', sizeof(pTypZi->Dt[cnt].kbn));						// 疵区分(スペース)
		memset(pTypZi->Dt[cnt].No, '0', sizeof(pTypZi->Dt[cnt].No));						// 疵No (0)
		memset(pTypZi->Dt[cnt].OpeDsp, ' ', sizeof(pTypZi->Dt[cnt].OpeDsp));				// オペガイ(スペース)
		memset(pTypZi->Dt[cnt].SysKbn, ' ', sizeof(pTypZi->Dt[cnt].SysKbn));				// システム区分(スペース)

		pInf->nFfCnt++;
		nCutCount++;
	}

}
//------------------------------------------
// 疵情報エリアにセットする
// int ou 0:表
// FF_INF * pInf 編集ワーク
// FF_JISSEKI* pTypZi FF実績伝文
//------------------------------------------
void MakeFfManager::MakeDefectData(int ou, FF_INF * pInf, FF_JISSEKI* pTypZi)
{
	int ii = 0;
	int cnt;				// FF情報セット位置

	// 何件目までセットしたか
	int nDefNow = 0;		// 疵

	int grec = 0;			// グレード情報のレコード
	CString cWk;
	char charWk[5];
	int nWk;


	// SELECTで取得時、優先度順に並び替えをしているため、このチェックは不要なのでは？？(2018/08/30 5CGL極微笑φω)

	////// グレード優先度順(1～)にデータをセットしてく
	//for (grec = 0; grec<m_Param_Grade.nNum; grec++) {
		//// 疵 チェック
		for (ii = nDefNow; ii<pInf->nDefCnt; ii++) {
		//	// セットする対象の優先度？
		//	if (m_Param_Grade.typGrade_id[grec].nPriority < pInf->typDef[ii].nGPriority) break;
			if (MAX_FF_DFCT_CNT <= pInf->nFfCnt) {
				pInf->bOverStatus = true;
				break;
			}

			// セット
			cnt = pInf->nFfCnt;

			memcpy(pTypZi->Dt[cnt].Kizukbn, Z_FF_DFCT_KENSA, sizeof(pTypZi->Dt[cnt].Kizukbn));	// 疵検出区分  (疵)

			cWk.Format("%s  ", pInf->typDef[ii].tFF );
			memcpy(pTypZi->Dt[cnt].Kind, cWk, sizeof(pTypZi->Dt[cnt].Kind));					// 疵種(2桁スペース)

			cWk.Format("%02d", pInf->typDef[ii].nGFF);
			memcpy(pTypZi->Dt[cnt].Grade, cWk, sizeof(pTypZi->Dt[cnt].Grade));					// 疵グレード(右詰, 0パディング)

			if(0 > pInf->typDef[ii].nWS) pInf->typDef[ii].nWS = 0;
			cWk.Format("%04d", pInf->typDef[ii].nWS);
			memcpy(pTypZi->Dt[cnt].WidPos, cWk, sizeof(pTypZi->Dt[cnt].WidPos));				// 疵巾方向位置[mm](右詰, 0パディング)

			nWk = (pInf->typDef[ii].nLen / 100);
			if(1 > nWk) nWk = 1;
			cWk.Format("%04d", nWk);
			memcpy(pTypZi->Dt[cnt].Len, cWk, sizeof(pTypZi->Dt[cnt].Len));						// 疵長さ[mm]→[0.1m](右詰, 0パディング)

			nWk = (int)((pInf->typDef[ii].nYpos / 100.0) + 0.5);								// [mm]→[0.1m] 四捨五入
			cWk.Format("%06d", nWk);
			memcpy(pTypZi->Dt[cnt].DefectPos, cWk, sizeof(pTypZi->Dt[cnt].DefectPos));			// 疵総長さ方向位置(右詰, 0パディング)

			memset(pTypZi->Dt[cnt].kbn, ' ', sizeof(pTypZi->Dt[cnt].kbn));						// 疵区分

			// 20200520 疵保存個数上限拡張 --->>>
			//cWk.Format("%04X", pInf->typDef[ii].nNo);
			//memcpy(pTypZi->Dt[cnt].No, cWk, sizeof(pTypZi->Dt[cnt].No));						// 疵No (Hexの文字表示)(右詰, 0パディング)

			// Base32HEXでセットする
			_itoa_s(pInf->typDef[ii].nNo, charWk, 32);
			cWk.Format("%04s", charWk);
			cWk.MakeUpper();
			memcpy(pTypZi->Dt[cnt].No, cWk, sizeof(pTypZi->Dt[cnt].No));						// 疵No (Hexの文字表示)(右詰, 0パディング)
			// 20200520 疵保存個数上限拡張 ---<<<

			memset(pTypZi->Dt[cnt].OpeDsp, ' ', sizeof(pTypZi->Dt[cnt].OpeDsp));				// オペガイ(スペース)
			memset(pTypZi->Dt[cnt].SysKbn, ' ', sizeof(pTypZi->Dt[cnt].SysKbn));				// システム区分(スペース)

			// 後処理
			pInf->typDef[ii].bFFSet = true;		// FFに実際にセットした場合True
			pInf->nFfCnt ++;
			nDefNow ++;
		}
	//}

	//// 疵個数オーバーステータス(2桁目スペース)
	if( pInf->bOverStatus ){
		cWk.Format("%d  ", Z_FF_KOVER_NG);
		memcpy(pTypZi->OverStatus, cWk, sizeof(pTypZi->OverStatus));
		LOG(em_MSG), "[%s] <%s> 上限到達 %d件(Sts:%d)", my_sThreadName, DivNameManager::GetTorB(ou), pInf->nFfCnt, pInf->bOverStatus);
	}else{
		cWk.Format("%d  ", Z_FF_KOVER_OK);
		memcpy(pTypZi->OverStatus, cWk, sizeof(pTypZi->OverStatus));
	}

	//// データ設定数
	cWk.Format("%04d", pInf->nFfCnt);
	memcpy(pTypZi->DataDefault, cWk, sizeof(pTypZi->DataDefault));


	//// エンド処理
	LOG(em_MSG), "[%s] <%s> トータル=%d件",	my_sThreadName, DivNameManager::GetTorB(ou), pInf->nFfCnt );
}
//------------------------------------------
// FF送信用 検査状態伝文生成
// int ou 0:表　1:裏
// int pcode 検査パターン (0オリジン)
// int nIsResult コイル実績有無 (0:有り 1:無し)
// FF_STS * pPos 編集ワーク
// FF_STS_JISSEKI* pTypSts 各面のみの実績伝文
// 戻り値 : 復帰情報
//------------------------------------------
bool MakeFfManager::MakeSendStsData(int ou, int pcode, int nIsResult, FF_STS* pPos, FF_STS_JISSEKI* pTypSts)
{
	CString cWk;

	//// 初期化
	memset(pTypSts, 0x20, sizeof(FF_STS_JISSEKI));


	//===========================
	// ヘッダー部
	//===========================
	if (0 == ou) memcpy(pTypSts->Hd.Header_Code, FF_TC_CODE_STSJI_O, sizeof(pTypSts->Hd.Header_Code));
	else		memcpy(pTypSts->Hd.Header_Code, FF_TC_CODE_STSJI_U, sizeof(pTypSts->Hd.Header_Code));
	memcpy(pTypSts->Hd.Rec_Code, Z_FF_RENBAN, sizeof(pTypSts->Hd.Rec_Code));

	// 送信時刻
	SYSTEMTIME STime;
	GetLocalTime(&STime);
	cWk.Format("%04d%02d%02d%02d%02d%02d", STime.wYear, STime.wMonth, STime.wDay, STime.wHour, STime.wMinute, STime.wSecond);
	memcpy(pTypSts->Hd.Date, cWk, sizeof(pTypSts->Hd.Date));

	// 工程コード ( 5CGL=85 )
	memcpy(pTypSts->Hd.Line_Code, KOUTEI_CODE, sizeof(pTypSts->Hd.Line_Code));

	// 命令番号
	memcpy(pTypSts->Hd.Meino, m_typBuf.data.KeyInf.cMeiNo, sizeof(pTypSts->Hd.Meino));

	// 冷延コイル番号
	//memcpy(pTypSts->Hd.Coilno, m_typBuf.data.KeyInf.cCoilNo, sizeof(pTypSts->Hd.Coilno));
	if ('-' == m_typBuf.data.KeyInf.cCoilNo[1]) {
		// コイルNoの'-'は削除してFF送信
		pTypSts->Hd.Coilno[0] = m_typBuf.data.KeyInf.cCoilNo[0];
		memcpy(&pTypSts->Hd.Coilno[1], &m_typBuf.data.KeyInf.cCoilNo[2], sizeof(pTypSts->Hd.Coilno) - 2);
	}
	else {
		memcpy(pTypSts->Hd.Coilno, m_typBuf.data.KeyInf.cCoilNo, sizeof(pTypSts->Hd.Coilno));
	}


	//===========================
	// コイル情報部
	//===========================
	// 管理No
	if (0 == nIsResult) memcpy(pTypSts->Dt.Id, m_cKizukenNo, strlen(m_cKizukenNo));		// 管理Noの最後のNULLは、コピーしたくないため


	// これ以外のコイル情報部は、これ以降の処理で順次確定させていく


	//===========================
	// 検査状態部
	//===========================
	//// 情報を取得する
	if (0 == nIsResult) {			// コイル実績がある場合のみ
									// 対象コイルのコイル位置情報を取得する
		if (!GetData_CoilPosition(ou, pPos)) {
			LOG(em_ERR), "[%s] <%s> 疵情報取得失敗", my_sThreadName, DivNameManager::GetTorB(ou));
			pPos->nCnt = 0;		// 念の為に無かったことにしておく
		}

	} else {
		// コイル実績が無いときは、検査状態セット数=0 検査状態データ=オールスペース
		cWk.Format("%05d", 0);
		memcpy(pTypSts->Dt.dataSize, cWk, sizeof(pTypSts->Dt.dataSize));
		return true;
	}

	//// 検査状態エリアにデータセット
	MakeStatusData(ou, pPos, pTypSts);

	return true;
}
//------------------------------------------
// 対象コイルのコイル位置情報を取得する
// int ou 0:表
// FF_STS * pSts 編集ワーク
//------------------------------------------
bool MakeFfManager::GetData_CoilPosition(int ou, FF_STS* pPos)
{
	int					rec;
	CString				cSql;
	short				nKensa;			// 検査状態
	CString				cWk;

	cSql = "SELECT ID, ";				// 必須
	cSql += (0 == ou) ? "検査状態_表" : "検査状態_裏";
	cSql += " FROM ";
	cSql += DB_COIL_POSITION;
	cWk.Format(" WHERE 管理No = '%s'", m_cKizukenNo); cSql += cWk;
	cSql += "ORDER BY ID ASC";

	if (!mcls_pDB->ExecuteDirect(cSql)) {
		LOG(em_ERR), "[%s] コイル位置情報取得失敗 [%s]", my_sThreadName, m_cKizukenNo);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", m_cKizukenNo);
		return false;
	}
	while (true) {
		int sqlrc = mcls_pDB->Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り

			rec = pPos->nCnt;

			mcls_pDB->GetData(1, &pPos->sts[rec].nID);

			mcls_pDB->GetData(2, &nKensa);

			pPos->sts[rec].emKensa = (EM_DIV_KENSA)nKensa;

			pPos->nCnt++;
			if (MAP_ROW_NUM <= pPos->nCnt) break;	// 後は削除していくだけ

		} else if (sqlrc == KIZUODBC_FETCH_NODATE) {		// データ無し
			break;
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", m_cKizukenNo);
			return false;
		}
	}

	LOG(em_MSG), "[%s] コイル位置情報個数=%d", my_sThreadName, pPos->nCnt );

	return true;
}
//------------------------------------------
// 検査状態エリアにセットする
// int ou 0:表
// FF_STS * pPos 編集ワーク
// FF_STS_JISSEKI* pTypSts FF検査状態伝文
//------------------------------------------
void MakeFfManager::MakeStatusData(int ou, FF_STS * pPos, FF_STS_JISSEKI* pTypSts)
{
	int	nStart;			// データセット開始行(0オリジン)
	int	nEnd;			// データセット終了行(0オリジン)
	int nMax;			// データセット最終行(0オリジン) ※コイル長[m]
	int nOkRow;			// 歯抜け処理用前回値用登録ありアドレスワーク
	int ii;				// ループカウンタ
	int jj;				// ループカウンタ
	int kk;				// ループカウンタ
	int nKensa;			// 検査状態セット用ワーク
	CString sWk;		// ワーク
	double dWk;			// ワーク
	char cWk;			// ワーク
	int nWk;			// ワーク


						// 初期化
	nStart = 0;
	nEnd = 0;
	nKensa = 0;
	// データセット最終行算出（０オリジン）
	nMax = int(m_nLen * m_dHosei / 1000.0 + 0.999) - 1;

	// 登録データ分ループ
	for (ii = 0; ii<pPos->nCnt; ii++) {
		// 検査状態をセットする。
		nKensa = pPos->sts[ii].emKensa;
		// 検査状態が０未満の場合は、9(EM_DIV_KENSA.DIV_KENSA_STOP)に置き換える。
		if (0 > nKensa) nKensa = 9;

		// IDに対するセット区間を算出する。
		// データセット開始行(0オリジン)算出
		dWk = (pPos->sts[ii].nID) * MAP_REPRES_MIN * m_dHosei;
		nStart = int(dWk);
		// データセット終了行(0オリジン)算出
		dWk = (pPos->sts[ii].nID + 1) * MAP_REPRES_MIN * m_dHosei + 0.999;
		nEnd = int(dWk) - 1;

		//下限チェック
		if (0 > nStart) nStart = 0;
		//データセット区間開始位置がデータセット最終行を超えた場合は終了とする。
		if (nMax < nStart) break;
		//下限チェック
		if (0 > nEnd) nEnd = 0;
		//データセット区間終了位置がデータセット最終行を超えた場合はデータセット最終行までとする。
		if (nMax < nEnd) nEnd = nMax;

		// データセット
		for (jj = nStart; jj <= nEnd; jj++) {
			if (0 == memcmp(" ", pTypSts->Dt.data[jj].status, sizeof(pTypSts->Dt.data[jj].status))) {
				// 未登録の場合は無条件セット(1桁)
				sWk.Format("%01d", nKensa);
				memcpy(pTypSts->Dt.data[jj].status, sWk, sizeof(pTypSts->Dt.data[jj].status));
			} else {
				// 既に登録済みの場合は優先度比較してセット(数字大きい方が、優先度：高)
				memcpy(&cWk,&pTypSts->Dt.data[jj],sizeof(pTypSts->Dt.data[jj]));
				nWk = atoi(&cWk);
				if (nWk < nKensa){
					sWk.Format("%01d", nKensa);
					memcpy(pTypSts->Dt.data[jj].status, sWk, sizeof(pTypSts->Dt.data[jj].status));
				}
			}
		}
	}
	// 歯抜けデータセット
	// ①先頭から歯抜けの場合、先頭からの歯抜け箇所を検査状態=9で埋める。
	if (0 == memcmp(" ", pTypSts->Dt.data[0].status, sizeof(pTypSts->Dt.data[0].status))) {
		for (ii = 0; ii <= nMax; ii++){
			if (0 == memcmp(" ", pTypSts->Dt.data[ii].status, sizeof(pTypSts->Dt.data[ii].status))) {
				//未登録に検査状態=9をセット
				sWk.Format("%01d", 9);
				memcpy(pTypSts->Dt.data[ii].status, sWk, sizeof(pTypSts->Dt.data[ii].status));
			} else {
				// 登録済みの為、処理を抜ける。
				break;
			}
		}
	}

	// ②途中(最終より前へ歯抜けも含む)の歯抜けは前回値をセット
	nOkRow = 0;
	for (ii = 0; ii <= nMax; ii++){
		if (0 == memcmp(" ", pTypSts->Dt.data[ii].status, sizeof(pTypSts->Dt.data[ii].status))) {
			memcpy(pTypSts->Dt.data[ii].status, pTypSts->Dt.data[nOkRow].status, sizeof(pTypSts->Dt.data[ii].status));
		}
		// 前回値用登録ありアドレス保存
		nOkRow = ii;
	}

	// 検査状態セット数
	sWk.Format("%05d", nMax + 1);
	memcpy(pTypSts->Dt.dataSize, sWk, sizeof(pTypSts->Dt.dataSize));

	//// エンド処理
	LOG(em_MSG), "[%s] <%s> 検査状態 件数=%d", my_sThreadName, DivNameManager::GetTorB(ou), nMax + 1);

}
//------------------------------------------
// FF用検査実績伝文 を バッファテーブルに登録
// char const* cID キー
// FF_JISSEKI const* pTypZi 実績伝文
// FF_STS_JISSEKI const* pTypSts 検査状態伝文
//------------------------------------------
bool MakeFfManager::InsertZiBuf(char const* cID, FF_JISSEKI const* pTypZi ,FF_STS_JISSEKI const* pTypSts)
{
	CString		cSql;

	cSql.Format("INSERT INTO %s VALUES(", DB_BUF_FF_S_ZISEKI);
	
	CreateSQL::sqlColumnInsert(&cSql, cID, SIZE_SERIAL_FF_NO);	// ID
	CreateSQL::sqlColumnAddTime(&cSql, true);					// 登録日付
	CreateSQL::sqlColumnInsert(&cSql, 1);						// 伝文区分(1:未送信)
	CreateSQL::sqlColumnInsert(&cSql, (char*)&m_typBuf.data.KeyInf, sizeof(m_typBuf.data.KeyInf));	// キー情報
	cSql += "?, ?, ?, ?,";										// 生伝文
	CreateSQL::sqlColumnInsert(&cSql, m_cKizukenNo, SIZE_KIZUKEN_NO);				// 管理No
	CreateSQL::sqlColumnInsert(&cSql, NULL, 0);					// 送信日付
	CreateSQL::sqlColumnTrim(&cSql);
	cSql += ")";


#ifndef TEST_ONLINE_DATA
	DWORD wksize[4];
	LPVOID wkpVal[4];
	// データセット
	wksize[0] = sizeof(FF_JISSEKI);
	wksize[1] = sizeof(FF_JISSEKI);
	wksize[2] = sizeof(FF_STS_JISSEKI);
	wksize[3] = sizeof(FF_STS_JISSEKI);
	wkpVal[0] = (BYTE*)&pTypZi[0];
	wkpVal[1] = (BYTE*)&pTypZi[1];
	wkpVal[2] = (BYTE*)&pTypSts[0];
	wkpVal[3] = (BYTE*)&pTypSts[1];

	// 登録
	if ( ! mcls_pDB->SqlBinalyWrite(cSql, 4, wksize, wkpVal,false) ) {
		LOG(em_NON), "%s", cSql);
		LOG(em_ERR), "[%s] 検査実績登録失敗 [%s]", my_sThreadName, cID);
		return false;
	}
#else
	// バイナリダンプ
	KizuFunction::DumpData("debag FF検査実績 表.txt", sizeof(FF_JISSEKI), (BYTE*)&pTypZi[0]); 
	KizuFunction::DumpData("debag FF検査実績 裏.txt", sizeof(FF_JISSEKI), (BYTE*)&pTypZi[1]);
	KizuFunction::DumpData("debag FF検査状態 表.txt", sizeof(FF_STS_JISSEKI), (BYTE*)&pTypSts[0]);
	KizuFunction::DumpData("debag FF検査状態 裏.txt", sizeof(FF_STS_JISSEKI), (BYTE*)&pTypSts[1]);
	LOG(em_WAR), "[%s] テストモードにより、FF検査実績をダンプ出力", my_sThreadName);

#endif

	return true;
}

//------------------------------------------
// 採寸実績テーブル を 更新
// char const* cID キー
// 戻り値 : 成否
//------------------------------------------
bool MakeFfManager::UpdateProconBuf(char const* cID)
{
	CString		cSql;

	cSql.Format("UPDATE %s SET ", DB_BUF_PRO_R_SAISUN);
	CreateSQL::sqlColumnUpdate(&cSql, "管理No", m_cKizukenNo, SIZE_KIZUKEN_NO);
	cSql += "編集日付=";
	CreateSQL::sqlColumnAddTime(&cSql);							// 編集日付
	cSql += " WHERE ID='";
	cSql += cID;
	cSql += "'";

	// 登録
	if (!mcls_pDB->ExecuteDirect(cSql))
	{
		LOG(em_ERR), "[%s] 採寸実績 UPDATEエラー [ID=%s]", my_sThreadName, cID);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", DB_BUF_PRO_R_SAISUN);
		return false;
	}

	return true;
}

//------------------------------------------
// コイル実績テーブル を 更新
// 戻り値 : 成否
//------------------------------------------
bool MakeFfManager::UpdateCoilResult()
{
	CString		cSql;
	CString		cWk;

	cSql.Format("UPDATE %s SET ", DB_COIL_RESULT);
	CreateSQL::sqlColumnUpdate(&cSql, "補正率", m_dHosei);
	CreateSQL::sqlColumnUpdate(&cSql, "プロコン実績反映", 0);	 
	CreateSQL::sqlColumnUpdate(&cSql, "プロコン実測長", m_typBuf.data.nLen * 100);			// [10-1m] → [mm] へ単位変換
	CreateSQL::sqlColumnUpdate(&cSql, "成品数", m_typBuf.data.nSetCnt);
	CreateSQL::sqlColumnTrim(&cSql);
	cSql += " WHERE 管理No='";
	cSql += m_cKizukenNo;
	cSql += "'";

	// 登録
	if (!mcls_pDB->ExecuteDirect(cSql))
	{
		LOG(em_ERR), "[%s] コイル実績 UPDATEエラー [管理No=%s]", my_sThreadName, m_cKizukenNo);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", DB_COIL_RESULT, m_cKizukenNo);
		return false;
	}

	return true;
}

//------------------------------------------
// カット実績テーブル に挿入
// const char *cID  キー
// 戻り値 : 成否
//------------------------------------------
bool MakeFfManager::InsertCutResult(const char* cID)
{
	CString		cSql;
	CString		cWk;
	int			nCutCount = 0;			// カット回数
	int			nCutPos;				// カット位置 [mm]

	// 溶接巻き込まれがある場合
	if (0 < m_typBuf.data.nMakiLenF)
	{
		// カット位置 ([10-1m] → [mm] へ単位変換)
		nCutPos = m_typBuf.data.nMakiLenF * 100;

		cSql.Format("INSERT INTO %s VALUES(", DB_COIL_CUT_RESULT);
		CreateSQL::sqlColumnInsert(&cSql, m_cKizukenNo, sizeof(m_cKizukenNo));									// 管理No
		CreateSQL::sqlColumnInsert(&cSql, nCutCount + 1);														// カットNo
		CreateSQL::sqlColumnInsert(&cSql, cID, SIZE_SERIAL_FF_NO);												// ID
		CreateSQL::sqlColumnInsert(&cSql, m_typBuf.data.KeyInf.cMeiNo, sizeof(m_typBuf.data.KeyInf.cMeiNo));	// 命令No
		CreateSQL::sqlColumnInsert(&cSql, m_typBuf.data.KeyInf.cCoilNo, sizeof(m_typBuf.data.KeyInf.cCoilNo));	// 生産No
		CreateSQL::sqlColumnInsert(&cSql, nCutPos);																// カット位置 [mm]
		CreateSQL::sqlColumnTrim(&cSql);
		cSql += ")";

		// 登録
		if (!mcls_pDB->ExecuteDirect(cSql))
		{
			LOG(em_ERR), "[%s] カット実績テーブル INSERTエラー [管理No=%s]", my_sThreadName, m_cKizukenNo);
			LOG(em_NON), "%s", cSql);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", DB_COIL_CUT_RESULT, m_cKizukenNo);
			return false;
		}
		nCutCount++;
	}

	// 採寸個数分セット (Fカット位置(=Endの方)を採用)
	for (int ii = 0; ii < m_typBuf.data.nSetCnt; ii++)
	{
		// カット位置 ([10-1m] → [mm] へ単位変換)
		nCutPos = m_typBuf.data.saisun[ii].nEnd * 100;

		cSql.Format("INSERT INTO %s VALUES(", DB_COIL_CUT_RESULT);
		CreateSQL::sqlColumnInsert(&cSql, m_cKizukenNo, sizeof(m_cKizukenNo));									// 管理No
		CreateSQL::sqlColumnInsert(&cSql, nCutCount + 1);														// カットNo
		CreateSQL::sqlColumnInsert(&cSql, cID, SIZE_SERIAL_FF_NO);												// ID
		CreateSQL::sqlColumnInsert(&cSql, m_typBuf.data.KeyInf.cMeiNo, sizeof(m_typBuf.data.KeyInf.cMeiNo));	// 命令No
		CreateSQL::sqlColumnInsert(&cSql, m_typBuf.data.KeyInf.cCoilNo, sizeof(m_typBuf.data.KeyInf.cCoilNo));	// 生産No
		CreateSQL::sqlColumnInsert(&cSql, nCutPos);																// カット位置 [mm]
		CreateSQL::sqlColumnTrim(&cSql);
		cSql += ")";

		// 登録
		if (!mcls_pDB->ExecuteDirect(cSql))
		{
			LOG(em_ERR), "[%s] カット実績テーブル INSERTエラー [管理No=%s]", my_sThreadName, m_cKizukenNo);
			LOG(em_NON), "%s", cSql);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", DB_COIL_CUT_RESULT, m_cKizukenNo);
			return false;
		}
		nCutCount++;
	}

	return true;
}

//------------------------------------------
// 採寸実績テーブル に挿入
// const char *cID  キー
// 戻り値 : 成否
//------------------------------------------
bool MakeFfManager::InsertSaisunResult(const char *cID)
{
	CString		cSql;
	CString		cWk;
	int			nDiv;					// 採寸区分 (0:通常カット 1:前コイルと結合している 2:後コイルと結合している 3:両方)
	int			nStartPos;				// 採寸開始位置 [mm]
	int			nEndPos;				// 採寸終了位置 [mm]

	// 採寸個数分セット
	for (int ii = 0; ii < m_typBuf.data.nSetCnt; ii++)
	{
		// 採寸区分
		if (0 == m_typBuf.data.saisun[ii].nStart && m_typBuf.data.nLen == m_typBuf.data.saisun[ii].nEnd)
		{
			nDiv = DIV_PRO_CUT_UNION;			// 前後コイルと結合
		}
		else if (0 == m_typBuf.data.saisun[ii].nStart)
		{
			nDiv = DIV_PRO_CUT_FRONT_UNION;		// 前コイルと結合
		}
		else if (m_typBuf.data.nLen == m_typBuf.data.saisun[ii].nEnd)
		{
			nDiv = DIV_PRO_CUT_TAIL_UNION;		// 後コイルと結合
		}
		else
		{
			nDiv = DIV_PRO_CUT_NORMAL;			// 通常カット
		}

		// 採寸開始位置、採寸終了位置 ([10-1mm] → [mm] へ単位変換)
		nStartPos = m_typBuf.data.saisun[ii].nStart * 100;
		nEndPos = m_typBuf.data.saisun[ii].nEnd * 100;

		cSql.Format("INSERT INTO %s VALUES(", DB_COIL_SAISUN_RESULT);
		CreateSQL::sqlColumnInsert(&cSql, m_cKizukenNo, sizeof(m_cKizukenNo));									// 管理No
		CreateSQL::sqlColumnInsert(&cSql, ii + 1);																// 採寸No
		CreateSQL::sqlColumnInsert(&cSql, cID, SIZE_SERIAL_FF_NO);												// ID
		CreateSQL::sqlColumnInsert(&cSql, m_typBuf.data.KeyInf.cMeiNo, sizeof(m_typBuf.data.KeyInf.cMeiNo));	// 命令No
		CreateSQL::sqlColumnInsert(&cSql, m_typBuf.data.KeyInf.cCoilNo, sizeof(m_typBuf.data.KeyInf.cCoilNo));	// 生産No
		CreateSQL::sqlColumnInsert(&cSql, nDiv);																// 採寸区分
		CreateSQL::sqlColumnInsert(&cSql, nStartPos);															// 採寸開始位置 [mm]
		CreateSQL::sqlColumnInsert(&cSql, nEndPos);																// 採寸終了位置 [mm]
		CreateSQL::sqlColumnTrim(&cSql);
		cSql += ")";

		// 登録
		if (!mcls_pDB->ExecuteDirect(cSql))
		{
			LOG(em_ERR), "[%s] 採寸実績テーブル INSERTエラー [管理No=%s]", my_sThreadName, m_cKizukenNo);
			LOG(em_NON), "%s", cSql);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", DB_COIL_SAISUN_RESULT, m_cKizukenNo);
			return false;
		}
	}

	return true;
}



//---------------------------------------------------------
// 次工程疵情報テーブル    (ACCESS_NEXT_DEFECT_INF)登録
// 次工程コイル情報テーブル(ACCESS_NEXT_COIL_INF)登録
// FF_INF*     pInf   編集ワーク
// FF_JISSEKI* pTypZi 各面のみの実績伝文
// 戻り値 : 復帰情報
//---------------------------------------------------------
bool MakeFfManager::InsertAccessNextInf( FF_INF pInf[], FF_JISSEKI pTypZi[])
{
	CString		cSql;
	int			nDefCnt[2];

	//// 疵情報を検索して、FFにセットした疵情報をDBに登録する
	for(int ou=0; ou<2; ou++ ) {
		nDefCnt[ou] = 0;

		for(int ii=0; ii < pInf[ou].nDefCnt; ii++ ) {
			if( ! pInf[ou].typDef[ii].bFFSet ) continue;

			cSql.Format("INSERT INTO %s VALUES(", DB_ACCESS_NEXT_DEFECT_INF );

			CreateSQL::sqlColumnInsert( &cSql, m_cKizukenNo, sizeof(m_cKizukenNo));	// 管理No
			CreateSQL::sqlColumnInsert( &cSql, pInf[ou].typDef[ii].nNo);			// 疵No
			CreateSQL::sqlColumnInsert( &cSql, ou);									// 表裏区分
			CreateSQL::sqlColumnInsert( &cSql, 0);									// 事前編集フラグ (デフォルトは0通常)
			CreateSQL::sqlColumnInsert( &cSql, pInf[ou].typDef[ii].nYpos);			// 補正率考慮後の長手位置

			// 20191010 ＲＣＬ　仮想検査装置　ソフト改造 溝口 ----->>>
			CreateSQL::sqlColumnInsert( &cSql, NULL, 0);							// 判定疵種
			CreateSQL::sqlColumnInsert( &cSql, NULL, 0);							// 判定グレード
			// 20191010 ＲＣＬ　仮想検査装置　ソフト改造 溝口 <<<-----

			CreateSQL::sqlColumnTrim(&cSql);
			cSql += ")";

			nDefCnt[ou] ++;

			// 登録
#ifndef TEST_ONLINE_DATA
			if( ! mcls_pDB->ExecuteDirect(cSql,false) ) {
				LOG(em_ERR), "[%s] 次工程疵情報 INSERTエラー [%s]", my_sThreadName, m_cKizukenNo);
				LOG(em_NON), "%s", cSql);
				syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", DB_ACCESS_NEXT_DEFECT_INF, m_cKizukenNo);
				return false;
			}
#endif
		}
	}


	//// コイル単位の情報をセット

	cSql.Format("INSERT INTO %s VALUES(", DB_ACCESS_NEXT_COIL_INF);
	
	CreateSQL::sqlColumnInsert( &cSql, m_cKizukenNo, SIZE_KIZUKEN_NO);	// 管理No
	CreateSQL::sqlColumnAddTime(&cSql, true);							// 登録日付
	CreateSQL::sqlColumnInsert( &cSql, 0 );								// 事前編集済みフラグ
	CreateSQL::sqlColumnInsert( &cSql, 0 );								// 次工程取得済みフラグ
	CreateSQL::sqlColumnInsert( &cSql, m_typBuf.data.nLen * 100);		// コイル長(プロコン採寸実績)(mm) [0.1m]->[mm]対応
	CreateSQL::sqlColumnInsert( &cSql, pInf[0].nFfCnt);					// データ設定数_表
	CreateSQL::sqlColumnInsert( &cSql, pInf[1].nFfCnt);					// データ設定数_裏
	CreateSQL::sqlColumnInsert( &cSql, nDefCnt[0]);						// 疵情報数_表
	CreateSQL::sqlColumnInsert( &cSql, nDefCnt[1]);						// 疵情報数_裏
	// 20200302 RCL 事前編集実施記録用DB追加 溝口 --->>>
	//cSql += ",";															// 
	CreateSQL::sqlColumnInsert(&cSql, NULL, 0);							// 事前編集_PC名称
	CreateSQL::sqlColumnInsert(&cSql, NULL, 0);							// 事前編集_ユーザー名
	CreateSQL::sqlColumnInsert(&cSql, NULL, 0);							// 事前編集_IPアドレス
	CreateSQL::sqlColumnInsert(&cSql, NULL, 0);							// 事前編集_RCL疵検管理No
	CreateSQL::sqlColumnInsert(&cSql, NULL, 0);							// 事前編集_RCLラインID
	CreateSQL::sqlColumnInsert(&cSql, NULL, 0);							// 事前編集_コイルNo
	CreateSQL::sqlColumnInsert(&cSql, NULL, 0);							// 事前編集_登録日付
	// 20200302 RCL 事前編集実施記録用DB追加 溝口 <<<---
	CreateSQL::sqlColumnTrim(&cSql);
	cSql += ")";

	// 登録
#ifndef TEST_ONLINE_DATA
	if( ! mcls_pDB->ExecuteDirect( cSql , false) ) {
		LOG(em_ERR), "[%s] 次工程コイル情報テーブル INSERTエラー [%s]", my_sThreadName, m_cKizukenNo);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", DB_ACCESS_NEXT_COIL_INF, m_cKizukenNo);
		return false;
	}
#endif

	return true;
}


//------------------------------------------
// 対FF通信 に送信依頼
// char const* cKey ID
//------------------------------------------
void MakeFfManager::Send_To_FF(char const* cKey)
{
	COMMON_QUE que;					// メール構造体
	memset(&que, 0x00, sizeof(que));

	que.nEventNo = FACT_TO_FFSERV_01;
	que.nLineNo = getlineid();
	// FF送信要求には下記は必要ないのでコメントアウト 
	//strcpy(que.fc.data, cKey);
	send_mail(TO_FFSERV, ".", &que);

	//LOG(em_MSG), "[%s] FF用検査実績送信要求送信完了 (FACT_TO_FFSERV_01) [ID=%s]", my_sThreadName, que.fc.data);
	//syslog(PARAM_SYSLOG_BASENO + 0, "[ID=%s]", que.fc.data);	// 0:FF送信用の検査実績伝文を作成しました。

	LOG(em_MSG), "[%s] FF用検査実績送信要求送信完了 (FACT_TO_FFSERV_01) [ID=%s]", my_sThreadName, cKey);
	syslog(355, "[ID=%s]", cKey);	// 0:FF送信用の検査実績伝文を作成しました。
}



//------------------------------------------
// 未送信の実績の検出とFF送信処理
// char const* cKizukenNo 管理No
//------------------------------------------
bool MakeFfManager::ExecUnsentResult(char const* cKizukenNo)
{
	LOG(em_MSG), "[%s] 未送信実績送信処理<%s> 開始", my_sThreadName, cKizukenNo );


	// メンバ変数のバックアップを行う
	char sKizukenNo_backup[SIZE_KIZUKEN_NO];
	memcpy(sKizukenNo_backup, m_cKizukenNo, SIZE_KIZUKEN_NO);
	PRO_TYPE_SAISUN typBuf_backup;
	memcpy(&typBuf_backup, &m_typBuf, sizeof(PRO_TYPE_SAISUN));
	double dHosei_backup = m_dHosei;


	// 接続が切れるかもしれないので、一旦切断してから、再接続処理を行っていたが、必要か？

	//// 処理中変数 初期化
	//SAFE_DELETE(mcls_pDB);

	////// ODBC接続クラス インスタンス生成
	//mcls_pDB = new OdbcBase();
	//if( ! mcls_pDB->Connect(TASKINI_NAME)) {
	//	// データベース接続エラー
	//	LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
	//	syslog(SYSNO_DB_CONNECT_ERR, "[%s]", my_sThreadName);
	//	LOG(em_ERR), "[%s] 未送信実績送信処理<%s> 失敗", my_sThreadName, cKizukenNo);
	//	return false;
	//}

	// 未送信実績取得
	if( ! GetUnsentCoilResult(cKizukenNo) ) {
		LOG(em_ERR), "[%s] 未送信実績送信処理<%s> 失敗", my_sThreadName, cKizukenNo);
		return false;
	}

	// 未送信の実績が存在したか
	if( 0 == m_nRecCnt ){
		LOG(em_MSG), "[%s] 未送信実績送信処理<%s> 完了 バックアップ送信対象無し", my_sThreadName, cKizukenNo);
		return true;
	}

	//// FF送信疵情報と特定する為にパラメータを取得
	//// 既に読み込み完了しているので、ここで読み込む必要は無い
	//if( ! ReadParam() ) {
	//	LOG(em_ERR), "[%s] パラメータ読み込み失敗", my_sThreadName);
	//	return false;
	//}

	// 未送信実績の生成と送信
	for(int ii = 0 ; ii < m_nRecCnt ; ii++ ){
		if( MakeUnsentFFSendData(ii) ) {
			UpdateCoilResultForUnsentData(m_unsent_data[ii].kizukenNo);
			// バックアップ送信済みカウント++
			m_nSentCnt++;
		}
	}

	LOG(em_MSG), "[%s] 未送信実績送信処理<%s> 完了 送信件数 [%d]/[%d]", my_sThreadName, cKizukenNo,m_nSentCnt,m_nRecCnt);
	syslog(357, "[送信件数=%d]", m_nSentCnt);	// FFバックアップ実績送信を行いました。

	// バックアップしたメンバ変数を復帰する
	memcpy(m_cKizukenNo,sKizukenNo_backup,SIZE_KIZUKEN_NO);
	memcpy(&m_typBuf,&typBuf_backup,sizeof(PRO_TYPE_SAISUN));
	m_dHosei =  dHosei_backup;

	return true;
}

//------------------------------------------
// 未送信データのコイル実績を抽出
// char const* cKizukenNo 管理No
//------------------------------------------
bool MakeFfManager::GetUnsentCoilResult(char const* cKizukenNo)
{
	CString		cSql;
	CString     fmt;

	// 未反映レコード件数カウントエリア初期化
	m_nRecCnt = 0; 
	// バックアップ送信済みカウント初期化
	m_nSentCnt = 0;

    // 未反映レコード情報の退避エリア初期化
	for(int i01 = 0 ; i01 < UNSENT_MAX ; i01++){
		memset(m_unsent_data[i01].kizukenNo, 0x00 , sizeof(m_unsent_data[i01].kizukenNo));
		m_unsent_data[i01].len = 0;
		m_unsent_data[i01].pcode[0] = 0;
		m_unsent_data[i01].pcode[1] = 0;
	}

	// 当該管理Noのレコードより登録日付が過去のデータを抽出(但し遡る期間指定)
	cSql="SELECT R.管理No, R.検査実測長, R.検査パターンコード_表, R.検査パターンコード_裏";
	cSql += ",C.命令No , C.生産No ";
	cSql += "FROM ";
	cSql += DB_COIL_RESULT;
	cSql += " as R ";
	cSql += " LEFT OUTER JOIN ";
	cSql += DB_ACCESS_NEXT_COIL_INF;
	cSql += " as N on R.管理No = N.管理No ";
	cSql += "RIGHT OUTER JOIN ";
	cSql += DB_COIL_INF;
	cSql += " as C on R.管理No = C.管理No ";
	cSql += "WHERE R.登録日付 < ";
	cSql += "(SELECT 登録日付 FROM ";
	cSql += DB_COIL_RESULT;
	cSql += " WHERE 管理No = '";
	cSql += cKizukenNo;
	cSql += "')";
	cSql += " AND R.登録日付 >= ";
	cSql += "(SELECT 登録日付 FROM ";
	cSql += DB_COIL_RESULT;
	cSql += " WHERE 管理No = '";
	cSql += cKizukenNo;
	cSql += "') - ";
	fmt.Format("%d", m_nChkUnSentDate);				// INIファイルの強制送信検索期間(日数)
	cSql += fmt;
	cSql += " AND R.[ウォーマー材区分] <> 1";		// ウォーマー材は、強制送信対象から除外する
	cSql += " AND R.[プロコン実績反映] = 1";		// プロコン実績反映が 1:未反映 が対象
	cSql += " AND R.[外部実績送信許可] = 0";		// 外部実績送信許可が 0:送信 が対象 
	cSql += " AND N.管理No IS NULL";				// 次工程コイル情報テーブルに管理Noを持たないレコードが対象
	cSql += " ORDER BY R.登録日付 ";

	//LOG(em_NON), "[%s] %s", my_sThreadName, cSql);

	if( ! mcls_pDB->ExecuteDirect(cSql)) {
		LOG(em_ERR), "[%s] バックアップ送信対象 SELECTエラー [%s]", my_sThreadName, m_cKizukenNo);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", m_cKizukenNo);
		return false;
	}
	while(true) {
		int sqlrc = mcls_pDB->Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {				// データ有り

			if( UNSENT_MAX <= m_nRecCnt ){
				// 最大件数オーバー
				LOG(em_WAR), "[%s] バックアップ送信対象件数オーバー(%d件)", my_sThreadName, UNSENT_MAX);
				break;
			}

			UNSENT_DATA* pData = &m_unsent_data[m_nRecCnt];
			
			mcls_pDB->GetData(1, &(pData->kizukenNo) , SIZE_KIZUKEN_NO );	// 管理No
			mcls_pDB->GetData(2, &(pData->len ));							// 疵検実測長
			mcls_pDB->GetData(3, &(pData->pcode[0]));						// 検査パターンコード 表
			pData->pcode[0]--;	// 0オリジンにする
			mcls_pDB->GetData(4, &(pData->pcode[1]));						// 検査パターンコード 裏
			pData->pcode[1]--;	// 0オリジンにする
			mcls_pDB->GetData(5, &(pData->MeiNo) , sizeof(pData->MeiNo));			// 命令番号
			mcls_pDB->GetData(6, &(pData->CoilNo) , sizeof(pData->CoilNo));			// コイルNo
			
			m_nLen = pData->len;

			LOG(em_MSG), "[%s] バックアップ送信対象実績[%02d] <%s> %d(mm) 検査パターン[%d][%d]", my_sThreadName, m_nRecCnt, pData->kizukenNo, pData->len, pData->pcode[0]+1, pData->pcode[1]+1 );
			m_nRecCnt++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {	// データ無し
			break;	
		} else {										// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", m_cKizukenNo);
			return false;
		}
	}

	return true;
}


//------------------------------------------
// 未送信実績データの生成
// int nRec 未送信データのレコード
//------------------------------------------
bool MakeFfManager::MakeUnsentFFSendData(int nRec)
{
	UNSENT_DATA* pData = &m_unsent_data[nRec];

	FF_JISSEKI			typZi[2];						// FF用 疵検査実績伝文
	FF_STS_JISSEKI		typSts[2];						// FF用 検査状態伝文
	FF_STS				typPosition[2];					// コイル位置情報
	FF_INF				typDefect[2];					// FFに送信する可能性がある疵情報
	int					nSpm = pData->nSpm;
	memset(typPosition, 0x00, sizeof(typPosition));
	memset(typDefect, 0x00, sizeof(typDefect));

	// 管理Noをセットする
	memcpy(m_cKizukenNo, pData->kizukenNo, SIZE_KIZUKEN_NO);

	// プロコン受信データは未受信のため、初期値をセットする
	// 命令番号セット (COIL_INFより取得)
	memcpy(m_typBuf.data.KeyInf.cMeiNo, pData->MeiNo, sizeof(m_typBuf.data.KeyInf.cMeiNo));
	// コイルNoセット (COIL_INFより取得)
	memcpy(m_typBuf.data.KeyInf.cCoilNo, pData->CoilNo, sizeof(m_typBuf.data.KeyInf.cCoilNo));
	// プロコン実績コイル長として、検査実測長をセットする[mm]→[10-1m]
	m_typBuf.data.nLen = pData->len / 100;

	// フロント巻き込まれ長さ 不定の為 0 とする
	m_typBuf.data.nMakiLenF = 0;
	// 採寸個数 不定のため 1 とする
	m_typBuf.data.nSetCnt = 1;
	// 採寸情報 １レコード目はコイル長をセットする それ以降は0をセット
	m_typBuf.data.saisun[0].nStart = 0;
	m_typBuf.data.saisun[0].nEnd = m_typBuf.data.nLen;
	for(int ii=1;ii<PRO_MAX_CUT_COUNT;ii++){
		m_typBuf.data.saisun[ii].nStart = 0;
		m_typBuf.data.saisun[ii].nEnd = 0;
	}

	// 補正率は 1.0 とする
	m_dHosei = 1.0;

	//// FF送信用 実績情報生成
	for(int ii=0; ii<NUM_MEN; ii++) {
		// バックアップ送信時は必ずコイル実績が存在するので、 0 で設定
		if( ! MakeSendData( ii, pData->pcode[ii], 0, &typDefect[ii], &typZi[ii]) ) {
			LOG(em_ERR), "[%s] <%s> FF検査実績伝文作成失敗(バックアップ送信) <%.20s>", my_sThreadName, DivNameManager::GetTorB(ii), m_cKizukenNo);
			syslog(356, "[管理No=%s:検査実績伝文作成失敗(%s)]", m_cKizukenNo, DivNameManager::GetTorB(ii));	// 検査実績伝文作成で異常
			return false;
		}
		if( ! MakeSendStsData( ii, pData->pcode[ii], 0, &typPosition[ii], &typSts[ii]) ){
			LOG(em_ERR), "[%s] <%s> FF検査状態伝文作成失敗(バックアップ送信) <%.20s>", my_sThreadName, DivNameManager::GetTorB(ii), m_cKizukenNo);
			syslog(356, "[管理No=%s:検査状態伝文作成失敗(%s)]", m_cKizukenNo, DivNameManager::GetTorB(ii));	// 検査状態伝文作成で異常
			return false;
		}
	}

	// 基本となるIDNoを生成       (yyyymmdd hhnnssff\0)
	char cID[SIZE_SERIAL_FF_NO];
	KizuLib::KizuFunction::GetSerialNoFF(cID);	

	//// FF用検査実績伝文 を バッファテーブルに登録
	if( InsertZiBuf(cID, typZi, typSts) ) {
		LOG(em_MSG), "[%s] FF検査実績送信バッファテーブル(BUF_FF_S_ZISEKI)登録完了 (バックアップ送信) <%.20s>", my_sThreadName, m_cKizukenNo);

	}else {
		LOG(em_ERR), "[%s] FF検査実績送信バッファテーブル(BUF_FF_S_ZISEKI)登録失敗 (バックアップ送信) <%.20s>", my_sThreadName, m_cKizukenNo);
		syslog(356, "[管理No=%s:登録失敗]", m_cKizukenNo);	// 検査実績送信伝文作成で異常
		return false;
	}

	//// プロコン採寸実績受信バッファテーブル 更新 ⇒ 受信していないので更新出来ない
	//if( UpdateProconBuf(cID) ) {
	//	LOG(em_MSG), "[%s] プロコン採寸実績受信バッファテーブル(BUF_PRO_R_SAISUN)更新完了", my_sThreadName);
	//} else {
	//	LOG(em_ERR), "[%s] プロコン採寸実績受信バッファテーブル(BUF_PRO_R_SAISUN)更新失敗", my_sThreadName);
	//}

	//// FF送信疵情報 を次工程情報テーブルに登録
	////  次工程コイル情報テーブル(ACCESS_NEXT_COIL_INF)
	////  次工程疵情報テーブル    (ACCESS_NEXT_DEFECT_INF)
	if( InsertAccessNextInf( &typDefect[0], &typZi[0]) ) {
		LOG(em_MSG), "[%s] 次工程情報テーブル 登録完了(バックアップ送信) <%.20s>", my_sThreadName, m_cKizukenNo);
	}else {
		LOG(em_ERR), "[%s] 次工程情報テーブル 登録失敗(バックアップ送信) <%.20s>", my_sThreadName, m_cKizukenNo);
	}

	return true;
}

//------------------------------------------
// 強制送信レコードのコイル実績テーブル を 更新
//------------------------------------------
bool MakeFfManager::UpdateCoilResultForUnsentData(char* cKizukenNo)
{
	CString		cSql;

	cSql = "UPDATE ";
	cSql += DB_COIL_RESULT;
	cSql += " SET ";
	cSql += "プロコン実績反映 = 2 "; // バックアップ
	cSql += " WHERE 管理No = '";
	cSql += cKizukenNo;
	cSql += "'";

	// 登録
#ifndef TEST_ONLINE_DATA
	if( ! mcls_pDB->ExecuteDirect(cSql,false) ) {
		LOG(em_ERR), "[%s] コイル実績 UPDATEエラー [%s](バックアップ)", my_sThreadName, cKizukenNo);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", DB_COIL_RESULT, m_cKizukenNo);
		return false;
	}
#endif

	return true;
}