#include "StdAfx.h"
#include "CoilManager.h"

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 比較用マクロ
#define JUDGE_CHECK_STR(coil, prm)   if(0!=strlen(prm)) { if(!clsAtlReg.IsRegularA(coil, sizeof(coil), prm) ) continue;  }		// 文字列チェック (正規表現)
#define JUDGE_CHECK_INTVAL_L(coil, prm)  if(0!=prm[0]) { if(coil<prm[0]) continue; }											// 上下限チェック (下限)
#define JUDGE_CHECK_INTVAL_H(coil, prm)  if(0!=prm[1]) { if(coil>prm[1]) continue; }											// 上下限チェック (上限)


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ヘッダー部

//------------------------------------------
// コンストラクタ
//------------------------------------------
CoilManager::CoilManager(LogFileManager* pLog) :
ThreadManager("CoilMgr")
{
	// ログファイルをセット
	SetLogMgr(pLog);
	mcls_Param.SetLogMgr(pLog);

//*//----> ◆
#ifdef LOCAL
	// LOCAL環境では 共有メモリを自分で生成する
	m_hCoil = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(TO_COIL_TBL), TBL_TO_COIL_NAME);
	mtbl_pCoil =(TO_COIL_TBL*) MapViewOfFile(m_hCoil, FILE_MAP_ALL_ACCESS, 0, 0, 0);
#endif
//*///<<---

	//// 共有メモリアクセス
	// コイル情報テーブル
	mem_connect(&m_hCoil, (LPVOID *)&mtbl_pCoil, TBL_TO_COIL_NAME);
	if( NULL == m_hCoil ) {
		syslog(SYSNO_MEM_OPEN, "コイル情報テーブル アクセス失敗");
		_ASSERT(FALSE);
	}

	// その他もろもろ
	memset(&m_typNewCoil, 0x00, sizeof(m_typNewCoil));
}

//------------------------------------------
// デストラクタ
//------------------------------------------
CoilManager::~CoilManager(void)
{
	//// 共有メモリ開放
	mem_close(&m_hCoil, (LPVOID *)&mtbl_pCoil);							// コイル情報
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 疵検共通情報特定

//------------------------------------------
// コイル情報を特定
// int  nRec        共有メモリレコード位置 (0オリジン)
// bool bRevFlg     表裏反転フラグ(デフォルトFalse)
// bool bHandSetFlg ダイアログ入力フラグ(デフォルトFalse)
//------------------------------------------
void CoilManager::Check_CoilInf(int nRec, bool bRevFlg, bool bHandSetFlg)
{
	//================================================
	// 初期準備

	COIL_SETTING			setting;
	COIL_BASEDATA			basedata;
	PRO_TYPE_COILINF		coil;

	int						nWk = 0;

	if (nRec < 0 || nRec >= 2) {
		LOG(em_ERR), "[%s] コイル情報 レコード位置異常 [%d]", my_sThreadName, nRec);
		return;
	}

	////// 共有メモリの情報を取得する
	memset( &m_typNewCoil, 0x00, sizeof(m_typNewCoil));
	memset( &setting,   0x00, sizeof(setting));
	memset( &basedata,  0x00, sizeof(basedata));
	memcpy( &coil, mtbl_pCoil->rec[nRec].val, sizeof(coil));

	////// 初期値セット
	for(int ii=0; ii<NUM_MEN; ii++) {
		setting.bDefScode[ii] = true;
		setting.bDefPcode[ii] = true;
		setting.scode[ii] = MAX_SCODE;
		setting.pcode[ii] = MAX_PCODE;
		setting.scode_spmoff[ii] = MAX_SCODE;
		setting.pcode_spmoff[ii] = MAX_PCODE;
	}
	setting.bWalm = false;


	////// 最新のパラメータを取得する
	//// パラメータ取得
	mcls_Param.SetLogMgr(mcls_pLog); 
	if( ! mcls_Param.ReadParam_All() ) {
		LOG(em_ERR), "[%s] 最新パラメータ取得失敗", my_sThreadName);
	}

	//================================================
	// data情報を生成

	// 命令No ← プロコン伝文 命令No
	memcpy(basedata.cMeiNo,	coil.data.CoilInf.KeyInf.cMeiNo, sizeof(coil.data.CoilInf.KeyInf.cMeiNo));
	// 入側コイルNo+NULL  (コイルNo) ガイダンス用 ← プロコン伝文 コイルNo
	memcpy(basedata.cCoilNoIn,	coil.data.CoilInf.KeyInf.cCoilNo, sizeof(coil.data.CoilInf.KeyInf.cCoilNo));					// コイルNo
	// コイルを一意に特定するためのキー ← コイルNo+命令No
	memcpy(basedata.cCoilKey, &coil.data.CoilInf.KeyInf, (SIZE_MEI_NO+PRO_SIZE_COIL_NO));

	// 入側コイル長[m] (上位情報の長さ) ← プロコン伝文 コイル長[m] - 入側テール端板長さ[mm] - 入側フロント端板長さ[mm]
	// 入側テール端板長さと入側フロント端板長さは[mm] → [m]に変換
	basedata.nLenIn = (int)((float)coil.data.CoilInf.nLenIn - (float)coil.data.CoilInf.nCutF / 1000.0f - (float) coil.data.CoilInf.nCutT / 1000.0f);
	
	// 板厚 [μm] ← プロコン伝文 材料板厚 [μm]
	basedata.nAtu = coil.data.CoilInf.nAtuZai;

	// 板幅 [mm] ← プロコン伝文 材料板幅[mm]
	basedata.nWid = coil.data.CoilInf.nWidZai;

	// 入側フロント端板長さ[mm] ← プロコン伝文 入側フロント端板長さ [mm]
	basedata.nCutLenF = coil.data.CoilInf.nCutF;

	// 入側テール端板長さ[mm] ← プロコン伝文 入側テール端板長さ [mm]
	basedata.nCutLenT = coil.data.CoilInf.nCutT;

	// SPM伸び率[10^2%] ← プロコン伝文 SPM伸び率 [10-2%]
	basedata.dNobiSPM = (double)(coil.data.CoilInf.nNobiSPM / 10000.0);

	// 出側コイル長[m]
	basedata.nLenOut = basedata.nLenIn;

	memcpy( &m_typNewCoil.basedata, &basedata, sizeof(COIL_BASEDATA) );

	//================================================
	// setting 情報を収集

	//// 各検査パターン等を特定する
	for(int ii=0; ii<NUM_MEN; ii++) {
		setting.bDefScode[ii] = false;
		setting.bDefPcode[ii] = false;

		SelectScode(&coil.data, ii, true,  &setting.scode[ii] );			// デフォルト SPM圧下
		SelectScode(&coil.data, ii, false, &setting.scode_spmoff[ii] );		// コイル先端通過時、PIO状態により切り替え用

		SelectPcode(&coil.data, ii, true,  &setting.pcode[ii] );			// デフォルト SPM圧下
		SelectPcode(&coil.data, ii, false, &setting.pcode_spmoff[ii] );		// コイル先端通過時、PIO状態により切り替え用
	}

	//// ウォーマ材判定 (だからと言って検査装置では別に何もしない)
	if( mcls_Param.GetCommonMast().nWalm == setting.pcode[0] ) {			// ウォーマ材?
		LOG(em_WAR), "[%s] ウォーマ材 認識！！ [コイルNo=%s]", my_sThreadName, basedata.cCoilNoIn);
		setting.bWalm = true;
	} else {
		setting.bWalm = false;
	}
	
	memcpy( &m_typNewCoil.setting, &setting, sizeof(COIL_SETTING));

	//================================================
	// 事後処理
	// 今回の結果を共有メモリにセット
	memcpy(&mtbl_pCoil->rec[nRec].setting,  &setting,  sizeof(setting));
	memcpy(&mtbl_pCoil->rec[nRec].basedata, &basedata, sizeof(basedata));
	
	CString sMsg;
	if( 0 == nRec ){
		sMsg.Format( "%s" , "検査用" );
	} else if( 1 == nRec ) {
		sMsg.Format( "%s" , "表示用" );
	} else {
		// この条件は本来ありえない
		sMsg.Format( "%s rec = (%d)" , "不定" , nRec);
	}

	LOG(em_MSG), "[%s] [%s] コイルNo=[%s] (%d/%d)(%d/%d) SPM_OFF(%d/%d)(%d/%d) %s ", my_sThreadName, sMsg,
		m_typNewCoil.basedata.cCoilNoIn, setting.scode[0], setting.scode[1],	setting.pcode[0], setting.pcode[1],
		setting.scode_spmoff[0], setting.scode_spmoff[1], setting.pcode_spmoff[0], setting.pcode_spmoff[1],
		setting.bWalm ? "ウォーマ材" : "");

	
	syslog(395, "[%s コイルNo=[%s] (%d/%d)(%d/%d) SPM_OFF(%d/%d)(%d/%d) %s]", sMsg,
		m_typNewCoil.basedata.cCoilNoIn, setting.scode[0], setting.scode[1],	setting.pcode[0], setting.pcode[1], 
		setting.scode_spmoff[0], setting.scode_spmoff[1], setting.pcode_spmoff[0], setting.pcode_spmoff[1],
		setting.bWalm ? "ウォーマ材" : "");

	//// 統括に通知
	COMMON_QUE		que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo	= FACT_TO_DEFECT_11;	// コイル情報受信通知 (←TO_GETDAT)
	que.nLineNo		= getlineid();
	que.fl.data[0]	= nRec;
	send_mail(TO_DEFECT, ".", &que);
}

//------------------------------------------
// 検査表面状態を決定
// SEQ_TYPE_COIL const* c コイル情報
// int ou 表裏
// bool bSpm SPM状態 (true:閉  false:開)
// int* scode 算出した検査表面状態 (1オリジン)
//------------------------------------------
void CoilManager::SelectScode(PRO_TYPE_DATA_COILINF const* c, int ou, bool bSpm, int* scode)
{
	AtlRegManager	clsAtlReg;				// 正規表現クラス
	const ParamManager::P_SCODE_OUTPUT& p = mcls_Param.GetScodeOutput();

	int		id;
	int		nWk = 0;
	float	dWk = 0.0;

	// -1 は、一番最後のデフォルトを実行させないため 
	for(int ii=0; ii<p.nMasterNum; ii++) {
		id = p.nPriority[ii]; 

		// このマスターは有効？
		if( ! p.rec[id].bUmu ) continue;

		// 正規表現に一致する条件をチェック
		for(int jj=0; jj<MAX_CODE_JUDGE; jj++) {
			// この条件は有効？
			if (!p.rec[id].typJudge[jj].bUmu) continue;

			//// 条件比較
			// SPM圧下有無 (-1:無し 0:×(無し:開放) 1:○(有り:圧下))
			if (-1 != p.rec[id].typJudge[jj].nSpm) {
				if ((0 == p.rec[id].typJudge[jj].nSpm && bSpm) ||
					(1 == p.rec[id].typJudge[jj].nSpm && !bSpm)) {
					// SPM圧下／開放の不一致
					continue;
				}
			}

			// 命令No (正規表現) プロコン:命令No 条件:命令No
			JUDGE_CHECK_STR(c->CoilInf.KeyInf.cMeiNo, p.rec[id].typJudge[jj].cMeiNo);

			// コイルNo (正規表現) プロコン:コイルNo 条件:生産No
			JUDGE_CHECK_STR(c->CoilInf.KeyInf.cCoilNo, p.rec[id].typJudge[jj].cCoilNo);

			// 板厚 (上限・下限) プロコン:注文板厚[μm] 条件:注文板厚[μm]
			JUDGE_CHECK_INTVAL_L(c->CoilInf.nAtuSei, p.rec[id].typJudge[jj].nAtuSei);
			JUDGE_CHECK_INTVAL_H(c->CoilInf.nAtuSei, p.rec[id].typJudge[jj].nAtuSei);

			// 検査基準 (正規表現) プロコン:検査基準 条件:検査基準
			// 本来 検査基準は表裏毎にあるのだが、CGLは、裏のデータが無い為、裏の場合も、表のデータを使用
			if (0 == ou)
			{
				JUDGE_CHECK_STR(c->CoilInf.cKenKijun_O, p.rec[id].typJudge[jj].cKenKijun);
			}
			else
			{
				JUDGE_CHECK_STR(c->CoilInf.cKenKijun_O , p.rec[id].typJudge[jj].cKenKijun);
			}

			// 向先 (正規表現) プロコン:向先 条件:向先
			JUDGE_CHECK_STR(c->CoilInf.cMuke, p.rec[id].typJudge[jj].cMuke);

			// 用途 (正規表現) プロコン:用途 条件:用途
			JUDGE_CHECK_STR(c->CoilInf.cYouto ,	p.rec[id].typJudge[jj].cYouto);

			// 主材質 (正規表現) プロコン:主材質 条件:主材質
			JUDGE_CHECK_STR(c->CoilInf.cSyuZai, p.rec[id].typJudge[jj].cSyuZai);

			// 表面処理1桁目 (正規表現) プロコン:表面処理1桁目 条件:表面処理1桁目
			JUDGE_CHECK_STR(c->CoilInf.cOmoteSyori1,	p.rec[id].typJudge[jj].cOmoteSyori1);

			// 品種 (正規表現) プロコン:品種 条件:品種
			JUDGE_CHECK_STR(c->CoilInf.cHinCode, p.rec[id].typJudge[jj].cHinCode);


			//// ここまできたらその検査表面状態の条件Noに一致
			LOG(em_MSG), "[%s] <%s> [%s] 検査表面状態=%d, 一致条件No=%d を使用", my_sThreadName, DivNameManager::GetTorB(ou), GetSpmModeName(bSpm), id+1, jj+1);
			*scode = id+1;
			return;
		}
	}

	// ここまできたら一致無しの為、デフォルトを使用
	LOG(em_MSG), "[%s] <%s> [%s] デフォルト検査表面状態=%d を使用", my_sThreadName, DivNameManager::GetTorB(ou), GetSpmModeName(bSpm), MAX_SCODE);
	*scode = MAX_SCODE;
}

//------------------------------------------
// 検査パターンを決定
// SEQ_TYPE_COIL const* c コイル情報
// int ou 表裏
// bool bSpm SPM状態 (true:閉  false:開)
// int* pcode 算出した検査パターン (1オリジン)
//------------------------------------------
void CoilManager::SelectPcode(PRO_TYPE_DATA_COILINF const* c, int ou, bool bSpm, int* pcode)
{
	AtlRegManager	clsAtlReg;				// 正規表現クラス
	const ParamManager::P_PCODE_OUTPUT& p = mcls_Param.GetPcodeOutput();

	int		id;
	int		nWk = 0;
	float	dWk = 0.0;

	// -1 は、一番最後のデフォルトを実行させないため 
	for(int ii=0; ii<p.nMasterNum; ii++) {
		id = p.nPriority[ii]; 

		// このマスターは有効？
		if( ! p.rec[id].bUmu ) continue;

		// 正規表現に一致する条件をチェック
		for(int jj=0; jj<MAX_CODE_JUDGE; jj++) {
			// この条件は有効？
			if (!p.rec[id].typJudge[jj].bUmu) continue;

			//// 条件比較
			// 命令No (正規表現) プロコン:命令No 条件:命令No
			JUDGE_CHECK_STR(c->CoilInf.KeyInf.cMeiNo, p.rec[id].typJudge[jj].cMeiNo);

			// コイルNo (正規表現) プロコン:コイルNo 条件:生産No
			JUDGE_CHECK_STR(c->CoilInf.KeyInf.cCoilNo, p.rec[id].typJudge[jj].cCoilNo);

			// 板厚 (上限・下限) プロコン:注文板厚[μm] 条件:注文板厚[μm]
			JUDGE_CHECK_INTVAL_L(c->CoilInf.nAtuSei, p.rec[id].typJudge[jj].nAtuSei);
			JUDGE_CHECK_INTVAL_H(c->CoilInf.nAtuSei, p.rec[id].typJudge[jj].nAtuSei);

			// 検査基準 (正規表現) プロコン:検査基準 条件:検査基準
			// 本来 検査基準は表裏毎にあるのだが、CGLは、裏のデータが無い為、裏の場合も、表のデータを使用
			if (0 == ou)
			{
				JUDGE_CHECK_STR(c->CoilInf.cKenKijun_O, p.rec[id].typJudge[jj].cKenKijun);
			}
			else
			{
				JUDGE_CHECK_STR(c->CoilInf.cKenKijun_O, p.rec[id].typJudge[jj].cKenKijun);
			}

			// 向先 (正規表現) プロコン:向先 条件:向先
			JUDGE_CHECK_STR(c->CoilInf.cMuke, p.rec[id].typJudge[jj].cMuke);

			// 用途 (正規表現) プロコン:用途 条件:用途
			JUDGE_CHECK_STR(c->CoilInf.cYouto, p.rec[id].typJudge[jj].cYouto);

			// 主材質 (正規表現) プロコン:主材質 条件:主材質
			JUDGE_CHECK_STR(c->CoilInf.cSyuZai, p.rec[id].typJudge[jj].cSyuZai);

			// 表面処理1桁目 (正規表現) プロコン:表面処理1桁目 条件:表面処理1桁目
			JUDGE_CHECK_STR(c->CoilInf.cOmoteSyori1, p.rec[id].typJudge[jj].cOmoteSyori1);

			// 品種 (正規表現) プロコン:品種 条件:品種
			JUDGE_CHECK_STR(c->CoilInf.cHinCode, p.rec[id].typJudge[jj].cHinCode);


			//// ここまできたらその検査パターンの条件Noに一致
			LOG(em_MSG), "[%s] <%s> [%s] 検査パターン=%d, 一致条件No=%d を使用", my_sThreadName, DivNameManager::GetTorB(ou), GetSpmModeName(bSpm), id+1, jj+1);
			*pcode = id+1;
			return;
		}
	}

	// ここまできたら一致無しの為、デフォルトを使用
	LOG(em_MSG), "[%s] <%s> [%s] デフォルト検査パターン=%d を使用", my_sThreadName, DivNameManager::GetTorB(ou), GetSpmModeName(bSpm), MAX_PCODE);
	*pcode = MAX_PCODE;
}
