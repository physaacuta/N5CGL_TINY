#include "StdAfx.h"
#include "OpSendFunc.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


//#include "HtRecv.h"

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 使用するクラスの定義

// インクルード定義
#include "StatusManager.h"												// ステータス制御基底クラス
#include "ParamManager.h"												// パラメータ管理
#include "CoilManager.h"												// コイル情報管理クラス
#include "PlgManager.h"													// PLG管理クラス
#include "HtRecv.h"														// 判定⇒統括間通信管理クラス
//
//// 静的領域
StatusManager*					mcls_pStatus	= NULL;					// ステータス制御基底クラス
ParamManager*					mcls_pParam		= NULL;					// パラメータ管理
CoilManager*					mcls_pCoil		= NULL;					// コイル管理クラス
PlgManager*						mcls_pPlg		= NULL;					// PLG管理クラス
HtRecv*							mcls_pHtRecv[NUM_MEN];					// 判定⇒統括間通信管理クラス



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 静的変数の初期化

// 同期制御用
CRITICAL_SECTION*				OpSendFunc::m_csLockCoil = NULL;	
ThreadQueue<SOCK_BASE_HEAD>*	OpSendFunc::mque_pOpSend[HY_SOCK_NUM] = { NULL };

// ログ用
LogFileManager*					OpSendFunc::mcls_pLog	= NULL;
LogFileManager*					OpSendFunc::mcls_pLogMark = NULL;
CString							OpSendFunc::my_sThreadName;



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// プロパティ

void OpSendFunc::SetStatusMgr(void* pCls)	{ mcls_pStatus	= (StatusManager*)pCls; }
void OpSendFunc::SetParamMgr(void* pCls)	{ mcls_pParam	= (ParamManager*)pCls; }
void OpSendFunc::SetCoilMgr(void* pCls)		{ mcls_pCoil	= (CoilManager*)pCls; }
void OpSendFunc::SetPlgMgr(void* pCls)		{ mcls_pPlg		= (PlgManager*)pCls; }
void OpSendFunc::SetHtRecv(int men, void* pCls)		{ mcls_pHtRecv[men]	= (HtRecv*) pCls; }



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 基本

//==========================================
// コンストラクタ
//==========================================
OpSendFunc::OpSendFunc() 
{
	my_sThreadName = "OpSendFnc";
}

//==========================================
// デストラクタ
//==========================================
OpSendFunc::~OpSendFunc(void)
{
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 共通処理


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 送信

//==========================================
// #0 ステータス情報を表示機能へ
//==========================================
void OpSendFunc::SendOp_Status()
{
	int					nSocnSize;				// 送信伝文長
	BYTE*				pSendBuf;				// 実領域
	BYTE*				pSend;				// 送信用バッファ
	
	//// 出力条件チェック
	for(int ii=0; ii<HY_SOCK_NUM; ii++) {
		if (NULL == mque_pOpSend[ii]) return;
	}
	
	//================================================
	////// 表示機能へ状態変更通知
	//// 送信領域確保
	nSocnSize = sizeof(TO_SO_HEAD) + sizeof(TO_SO_DATA_STATUS);
	pSendBuf  = new BYTE[nSocnSize];
	memset(pSendBuf, 0x00, nSocnSize);
	TO_SO_HEAD&			head = (TO_SO_HEAD&)pSendBuf[0];							// ヘッダ部エイリアス
	TO_SO_DATA_STATUS&	data = (TO_SO_DATA_STATUS&)pSendBuf[sizeof(TO_SO_HEAD)];	// データ部エイリアス

	//// データセット
	// ヘッダー部(ベース)
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len		= nSocnSize;
	head.base_head.addr[0]	= NULL;
	head.base_head.addr[1]	= NULL;

	// ヘッダー部(拡張)
	head.men				= -1;												// 表裏関係無し
	head.kind				= TO_SO_DATA_KIND_STATUS;							// ステータス情報

	// データ部(構造体)
	data.nUnten 			= mcls_pStatus->GetUnten(); 
	data.nKadou 			= mcls_pStatus->GetKadou(); 
	memcpy( &data.status, &mcls_pStatus->GetStatusNowSystem(), sizeof(TYP_STATUS_NOWSYSTEM));

	//================================================
	//// 送信依頼
//◆	LOG(em_MSG), "[%s] 統括⇒表示間 ステータス情報", my_sThreadName);
	CString sWk;
	for(int ii=0;ii<HY_SOCK_NUM; ii++) {
		pSend = new BYTE[nSocnSize];
		memcpy(pSend, pSendBuf, nSocnSize);
		sWk.Format("%s_%d", INI_OP_SEND, ii + 1);
		if (!mque_pOpSend[ii]->AddToTailFreeCheck((SOCK_BASE_HEAD*)pSend, 5)) {		// 登録
			LOG(em_ERR), "[%s] <%s> 統括⇒表示間 ステータス情報送信依頼キューフル", my_sThreadName, sWk.operator LPCSTR());
			syslog(SYSNO_QUEFULL_WAR, "[ステータス情報]");
			delete pSend;
			break;
		}
	}
	delete pSendBuf;
}

//==========================================
// #1 現在の検査用コイル情報を表示機能へ
//------------------------------------------
// COIL_INF const*	pCoilInf : 対象のコイル情報
//==========================================
void OpSendFunc::SendOp_CoilInf_Ken(COIL_INF const* pCoilInf)
{
	int					nSocnSize;				// 送信伝文長
	BYTE*				pSendBuf;				// 実領域
	BYTE*				pSend;				// 送信用バッファ
	
	//// 出力条件チェック
	if( NULL == mque_pOpSend ) return;
	if( NULL == pCoilInf ) return;


	//================================================
	////// 表示機能へコイル情報通知
	//// 送信領域確保
	nSocnSize = sizeof(TO_SO_HEAD) + sizeof(TO_SO_DATA_COIL);
	pSendBuf  = new BYTE[nSocnSize];
	memset(pSendBuf, 0x00, nSocnSize);
	TO_SO_HEAD&			head = (TO_SO_HEAD&)pSendBuf[0];							// ヘッダ部エイリアス
	TO_SO_DATA_COIL&	data = (TO_SO_DATA_COIL&)pSendBuf[sizeof(TO_SO_HEAD)];		// データ部エイリアス

	//// データセット
	// ヘッダー部(ベース)
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len		= nSocnSize;
	head.base_head.addr[0]	= NULL;
	head.base_head.addr[1]	= NULL;

	// ヘッダー部(拡張)
	memcpy( head.KizukenNo, pCoilInf->cKizukenNo, SIZE_KIZUKEN_NO);
	head.men				= -1;												// 表裏関係無し
	head.kind				= TO_SO_DATA_KIND_COIL_KEN;							// コイル情報（検査用）

	// データ部(構造体)
	memcpy( &data.coilbase, &pCoilInf->data,   sizeof(COIL_BASEDATA));
	memcpy( &data.coilinf,  &pCoilInf->data_u, sizeof(PRO_TYPE_DATA_COILINF));
	for(int ii=0; ii<NUM_MEN; ii++) {
		data.scode[ii]			= pCoilInf->setting.scode[ii];			// SPM圧下時の検査表面状態
		data.pcode[ii]			= pCoilInf->setting.pcode[ii];			// SPM圧下時の検査パターン
		data.scode_spmoff[ii]	= pCoilInf->setting.scode_spmoff[ii];	// SPM開放時の検査表面状態
		data.pcode_spmoff[ii]	= pCoilInf->setting.pcode_spmoff[ii];	// SPM開放時の検査パターン
	}
	data.nMapColNum			= pCoilInf->nMapCol;

	//================================================
	//// 送信依頼
	CString sWk;
	for(int ii=0;ii<HY_SOCK_NUM; ii++) {
		pSend = new BYTE[nSocnSize];
		memcpy(pSend, pSendBuf, nSocnSize);
		sWk.Format("%s_%d", INI_OP_SEND, ii + 1);
		if( mque_pOpSend[ii]->AddToTail((SOCK_BASE_HEAD*)pSend, 10, 20) ) {		// 絶対登録
			LOG(em_MSG), "[%s] <%s> 統括⇒表示間 検査用コイル情報送信 (%s) コイルNo(%s)", my_sThreadName, sWk.operator LPCSTR(),
				pCoilInf->cKizukenNo, pCoilInf->data.cCoilNoIn);

		} else {									// 空き無し
			LOG(em_ERR), "[%s] <%s> 統括⇒表示間 検査用コイル情報送信依頼キューフル", my_sThreadName, sWk.operator LPCSTR());
			syslog(SYSNO_QUEFULL_ERR, "[検査用コイル情報] <%s> <%s> コイルNo(%s)]", sWk.operator LPCSTR(), pCoilInf->cKizukenNo, pCoilInf->data.cCoilNoIn);
			delete pSend;
			break;
		}
	}
	delete pSendBuf;
}

//==========================================
// #2 コイル実績を表示機能へ
//------------------------------------------
// COIL_INF const*	pCoilInf : 対象のコイル情報
//==========================================
void OpSendFunc::SendOp_CoilResult(COIL_INF const* pCoilInf)
{
	int					nSocnSize;				// 送信伝文長
	BYTE*				pSendBuf;				// 実領域
	BYTE*				pSend;				// 送信用バッファ
		
	//// 出力条件チェック
	if( NULL == mque_pOpSend ) return;
	if( NULL == pCoilInf ) return;


	//================================================
	////// 表示機能へ状態変更通知
	//// 送信領域確保
	nSocnSize = sizeof(TO_SO_HEAD);
	pSendBuf  = new BYTE[nSocnSize];
	memset(pSendBuf, 0x00, nSocnSize);
	TO_SO_HEAD&			head = (TO_SO_HEAD&)pSendBuf[0];							// ヘッダ部エイリアス

	//// データセット
	// ヘッダー部(ベース)
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len		= nSocnSize;
	head.base_head.addr[0]	= NULL;
	head.base_head.addr[1]	= NULL;

	// ヘッダー部(拡張)
	memcpy( head.KizukenNo, pCoilInf->cKizukenNo, SIZE_KIZUKEN_NO);
	head.men				= -1;												// 表裏関係無し
	head.kind				= TO_SO_DATA_KIND_COIL_RESULT;						// コイル情報（表示用）

	// ヘッダー部(拡張)
	head.data_coilresult.len = pCoilInf->nLength;

	//================================================
	//// 送信依頼
	CString sWk;
	for(int ii=0;ii<HY_SOCK_NUM; ii++) {
		pSend = new BYTE[nSocnSize];
		memcpy(pSend, pSendBuf, nSocnSize);
		sWk.Format("%s_%d", INI_OP_SEND, ii + 1);
		if( mque_pOpSend[ii]->AddToTail((SOCK_BASE_HEAD*)pSend, 10, 20) ) {		// 絶対登録
			LOG(em_MSG), "[%s] <%s> 統括⇒表示間 コイル実績送信 (%s) コイルNo(%s) [%dmm]", my_sThreadName, sWk.operator LPCSTR(),
				pCoilInf->cKizukenNo, pCoilInf->data.cCoilNoIn, pCoilInf->nLength );
		} else {									// 空き無し
			LOG(em_ERR), "[%s] <%s> 統括⇒表示間 コイル実績送信依頼キューフル", my_sThreadName, sWk.operator LPCSTR());
			syslog(SYSNO_QUEFULL_ERR, "[コイル実績 <%s> <%s> コイルNo(%s)]", sWk.operator LPCSTR(), pCoilInf->cKizukenNo, pCoilInf->data.cCoilNoIn);
			delete pSend;
			break;
		}
	}
	delete pSendBuf;
}

//==========================================
// #3 現在の表示用コイル情報を表示機能へ
//------------------------------------------
// COIL_INF const*	pCoilInf 対象のコイル情報
//==========================================
void OpSendFunc::SendOp_CoilInf_Dsp(COIL_INF const* pCoilInf)
{
	int					nSocnSize;				// 送信伝文長
	BYTE*				pSendBuf;				// 実領域
	BYTE*				pSend;				// 送信用バッファ
	
	//// 出力条件チェック
	if( NULL == mque_pOpSend ) return;
	if( NULL == pCoilInf ) return;


	//================================================
	////// 表示機能へ状態変更通知
	//// 送信領域確保
	nSocnSize = sizeof(TO_SO_HEAD) + sizeof(TO_SO_DATA_COIL);
	pSendBuf = new BYTE[nSocnSize];
	memset(pSendBuf, 0x00, nSocnSize);
	TO_SO_HEAD&			head = (TO_SO_HEAD&)pSendBuf[0];						// ヘッダ部エイリアス
	TO_SO_DATA_COIL&	data = (TO_SO_DATA_COIL&)pSendBuf[sizeof(TO_SO_HEAD)];	// データ部エイリアス

	//// データセット
	// ヘッダー部(ベース)
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len		= nSocnSize;
	head.base_head.addr[0]	= NULL;
	head.base_head.addr[1]	= NULL;

	// ヘッダー部(拡張)
	memcpy( head.KizukenNo, pCoilInf->cKizukenNo, SIZE_KIZUKEN_NO);
	head.men				= -1;										// 表裏関係無し
	head.kind				= TO_SO_DATA_KIND_COIL_DSP;					// コイル情報（表示用）
	head.data_coilinf_dsp.nSpmUmu = pCoilInf->nSt_Spm;					// SPM圧下有無　0：開放状態 1：圧下状態

	// データ部(構造体)
	memcpy(&data.coilbase, &pCoilInf->data, sizeof(COIL_BASEDATA));
	memcpy(&data.coilinf, &pCoilInf->data_u, sizeof(PRO_TYPE_DATA_COILINF));
	for (int ii = 0; ii<NUM_MEN; ii++)
	{
		data.scode[ii]			= pCoilInf->setting.scode[ii];			// SPM圧下時の検査表面状態
		data.pcode[ii]			= pCoilInf->setting.pcode[ii];			// SPM圧下時の検査パターン
		data.scode_spmoff[ii]	= pCoilInf->setting.scode_spmoff[ii];	// SPM開放時の検査表面状態
		data.pcode_spmoff[ii]	= pCoilInf->setting.pcode_spmoff[ii];	// SPM開放時の検査パターン
	}
	data.nMapColNum = pCoilInf->nMapCol;

	//================================================
	//// 送信依頼
	CString sWk;
	for(int ii=0;ii<HY_SOCK_NUM; ii++) {
		pSend = new BYTE[nSocnSize];
		memcpy(pSend, pSendBuf, nSocnSize);
		sWk.Format("%s_%d", INI_OP_SEND, ii + 1);
		if( mque_pOpSend[ii]->AddToTail((SOCK_BASE_HEAD*)pSend, 10, 20) ) {		// 絶対登録
			LOG(em_MSG), "[%s] <%s> 統括⇒表示間 表示用コイル情報送信 (%s) コイルNo(%s)", my_sThreadName, sWk.operator LPCSTR(),
				pCoilInf->cKizukenNo, pCoilInf->data.cCoilNoIn);

		} else {									// 空き無し
			LOG(em_ERR), "[%s] <%s> 統括⇒表示間 表示用コイル情報送信依頼キューフル", my_sThreadName, sWk.operator LPCSTR());
			syslog(SYSNO_QUEFULL_ERR, "[表示用コイル情報 <%s> <%s> コイルNo(%s)]", sWk.operator LPCSTR(), pCoilInf->cKizukenNo, pCoilInf->data.cCoilNoIn);
			delete pSend;
			break;
		}
	}
	delete pSendBuf;
}

//==========================================
// #4 疵情報(疵情報+疵画像)を表示機能へ
//------------------------------------------
// int men	0:表, 1:裏
// COIL_INF const*	pCoilInf 疵が含まれるコイル情報
// DETECT_BASE_DATA const* pData 検査結果(疵情報)
// DEFECT_DATA_IMAGE* pImg 太平用画像情報ポインタ
// float dLen 疵発生位置（SPM状態を加味した疵発生位置）
//==========================================
void OpSendFunc::SendOp_DetectData(int men, COIL_INF const* pCoilInf, DETECT_BASE_DATA const* pData, DEFECT_DATA_IMAGE const* pImg, float dLen)
{
	int					nSocnSize;				// 送信伝文長
	BYTE*				pSendBuf;				// 実領域
	BYTE*				pSend;				// 送信用バッファ
	int					nWk = 0;

	//// 出力条件チェック
	for(int ii=0;ii<HY_SOCK_NUM; ii++) {
		if( NULL == mque_pOpSend[ii] ) return;
	}
	if( NULL == pCoilInf ) return;


	////================================================
	//// 送信領域確保
	
	// 画像部データ計算
	nWk = 0;
	for(int ii=0; ii<MAX_IMAGE_CNT; ii++) nWk += pImg->nSize[ii];

	// 送信領域確保 (-4 は、本来-1だがバウンダリがある為の処置)
	nSocnSize = sizeof(TO_SO_HEAD) + sizeof(TO_SO_DATA_DETECT) - 4 + nWk;		// この時点では、まだ仮
	pSendBuf  = new BYTE[nSocnSize];
	//memset(pSendBuf, 0x00, nSocnSize);		// これはいるか？？
	TO_SO_HEAD&			head = (TO_SO_HEAD&)pSendBuf[0];							// ヘッダ部エイリアス
	TO_SO_DATA_DETECT&	data = (TO_SO_DATA_DETECT&)pSendBuf[sizeof(TO_SO_HEAD)];	// データ部エイリアス

	BYTE* pJpegBuf = data.image;		// 画像データ先頭ポインタ


	//================================================
	// 通常のデータセット
	//// データセット (実コイルが切り替わってもまだ前回のコイルの情報を送信する。だって前の情報だもん)
	// ヘッダー部(ベース)
	memset(&head, 0x00, sizeof(TO_SO_HEAD));
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len		= nSocnSize;
	head.base_head.addr[0]	= NULL;
	head.base_head.addr[1]	= NULL;

	// ヘッダー部(拡張)
	memcpy( head.KizukenNo, pCoilInf->cKizukenNo, SIZE_KIZUKEN_NO);	
	head.men				= men;
	head.kind				= TO_SO_DATA_KIND_DETECT;					// 疵情報 (行列情報)
	
	// データ部(構造体)
	memcpy( &data.datas, pData, sizeof(DETECT_BASE_DATA));

	// 入側先端からの位置[mm]
	data.datas.y = dLen;
	
	//----------------------------------------
	// 個々の画像データを送信領域にセット
	nWk = 0;
	for(int ii=0; ii<MAX_IMAGE_CNT; ii++) {
		// 疵画像は存在するか？
		if (0 >= pImg->nSize[ii] ) {
			data.image_len[ii] = 0;
			continue;
		}
		// 表示対象の疵画像か？
		if (!mcls_pParam->Check_ImgDsp(pData->nTid, ii)) {
			data.image_len[ii] = 0;
			continue;
		}
		memcpy( &pJpegBuf[nWk], pImg->pImg[ii], pImg->nSize[ii]);
		nWk += pImg->nSize[ii];

		data.image_len[ii] = pImg->nSize[ii];
	}



	//================================================
	//// 送信依頼
//◆LOG(em_MSG), "[%s] 統括⇒表示間 疵情報 <%s>[%s][No=%d][t=%d][g=%d]", my_sThreadName, DivNameManager::GetTorB(head.men), head.KizukenNo, data.data.nKizuNo,	data.data.nTid, data.data.nGid);
	CString sWk;
	for(int ii=0;ii<HY_SOCK_NUM; ii++) {
		pSend = new BYTE[nSocnSize];
		memcpy(pSend, pSendBuf, nSocnSize);
		sWk.Format("%s_%d", INI_OP_SEND, ii + 1);
		if( ! mque_pOpSend[ii]->AddToTailFreeCheck((SOCK_BASE_HEAD*)pSend, 10) ) {
			delete pSend;
			LOG(em_ERR), "[%s] <%s> <%d> 統括⇒表示間 疵情報送信依頼キューフル", my_sThreadName, sWk.operator LPCSTR(), men);
			syslog(SYSNO_QUEFULL_WAR, "[%d TO_SO_HEAD TO_SO_DATA_KIND_DETECT]", men);
			break;
		}
	}
	delete pSendBuf;
}

//==========================================
// #6 通板距離把握 & 送信
//------------------------------------------
// int men	0:表, 1:裏
// int y	通板距離[m]
//==========================================
void OpSendFunc::SendOp_Length(int men, int y)
{
	int					nSocnSize;				// 送信伝文長
	BYTE*				pSendBuf;				// 実領域
	BYTE*				pSend;				// 送信用バッファ
	
	const COIL_INF*		pCoil;					// ワーク
	const COIL_INF*		pCoilBaseKen = mcls_pCoil->GetCoilBaseKen();		// 検査用コイル情報
	const COIL_INF*		pCoilBaseDsp = mcls_pCoil->GetCoilBaseDsp();		// 表示用コイル情報


	// 出力条件チェック
	for(int ii=0;ii<HY_SOCK_NUM; ii++) {
		if( NULL == mque_pOpSend[ii] ) return;
	}
	if( NULL == pCoilBaseKen ) return;		// 検査用情報無しの場合は送信しない
	if( NULL == pCoilBaseDsp ) return;		// 表示用情報無しの場合は送信しない
	if( ! mcls_pStatus->IsRun() ) return;	// 検査中のみ


	//// コイル情報クラス クリティカル
	//AutoLock autolock(m_csLockCoil);
 
	//================================================
	////// 表示機能へ表示用通板距離変更通知
	//// 送信領域確保
	nSocnSize = sizeof(TO_SO_HEAD) + sizeof(TO_SO_DATA_LENGTH);
	pSendBuf  = new BYTE[nSocnSize];
	memset(pSendBuf, 0x00, nSocnSize);
	TO_SO_HEAD&			head = (TO_SO_HEAD&)pSendBuf[0];							// ヘッダ部エイリアス
	TO_SO_DATA_LENGTH&	data = (TO_SO_DATA_LENGTH&)pSendBuf[sizeof(TO_SO_HEAD)];	// データ部エイリアス

	//// データセット
	// ヘッダー部(ベース)
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len		= nSocnSize;
	head.base_head.addr[0]	= NULL;
	head.base_head.addr[1]	= NULL;

	// ヘッダー部(拡張)
	memset( head.KizukenNo, 0x00, SIZE_KIZUKEN_NO);								// 管理No無し
	head.men				= -1;												// 表裏関係無し
	head.kind				= TO_SO_DATA_KIND_LENGTH;							// 表示基点位置でのコイル通板位置情報


	// データ部(構造体)
	// 検査
	for(int ii=0; ii<MAX_DISP_POS; ii++) {
		pCoil = mcls_pCoil->GetCoilAll(ii);
		if(NULL == pCoil) continue;

		memcpy(data.dsp[ii].KizukenNo, pCoil->cKizukenNo, SIZE_KIZUKEN_NO);
		// 現在の通板距離をそのままセット
		data.dsp[ii].len = (long)(mcls_pPlg->GetLen_All(ii) / 1000.0);
	}

	//================================================
	//// 送信依頼
//◆	LOG(em_MSG), "[%s] 統括⇒表示間 通板距離 [%s][%dm]", my_sThreadName, data.dsp[0].KizukenNo, data.dsp[0].len);
//LOG(em_MSG), "距離情報 Head(%d/%s/%d) Size(max:%d=send:%d) KenP(%s:%dm) KenD(%s:%dm) Dsp1(%s:%dm)", 
//	head.kind, head.KizukenNo, head.men, nSocnSize, head.base_head.len, 
//	data.ken[0].KizukenNo, data.ken[0].len, data.kenData[0].KizukenNo, data.kenData[0].len, data.dsp[0].KizukenNo, data.dsp[0].len);
	CString sWk;
	for(int ii=0;ii<HY_SOCK_NUM; ii++) {
		pSend = new BYTE[nSocnSize];
		memcpy(pSend, pSendBuf, nSocnSize);
		sWk.Format("%s_%d", INI_OP_SEND, ii + 1);
		if( ! mque_pOpSend[ii]->AddToTailFreeCheck((SOCK_BASE_HEAD*)pSend, 7) ) {		// 登録
			LOG(em_ERR), "[%s] <%s> 統括⇒表示間 通板距離送信依頼キューフル", my_sThreadName, sWk.operator LPCSTR());
			syslog(SYSNO_QUEFULL_WAR, "[通板距離]");
			delete pSend;
			break;
		}
	}
	delete pSendBuf;
}

//==========================================
// #7 カット情報を表示機能へ
//------------------------------------------
// char const*			 cKizukenNo	: 管理No
// TO_SO_DATA_CUT const* pInf		: 対象の情報
//==========================================
void OpSendFunc::SendOp_CutInf(char const* cKizukenNo, TO_SO_DATA_CUT const* pInf)
{
	int					nSocnSize;				// 送信伝文長
	BYTE*				pSendBuf;				// 実領域
	BYTE*				pSend;				// 送信用バッファ
	
	//// 事前チェック
	if( NULL == mque_pOpSend ) return;
	if( NULL == pInf) return;


	//================================================
	////// 表示機能へ通知
	//// 送信領域確保
	nSocnSize = sizeof(TO_SO_HEAD) + sizeof(TO_SO_DATA_CUT);
	pSendBuf  = new BYTE[nSocnSize];
	memset(pSendBuf, 0x00, nSocnSize);
	TO_SO_HEAD&			head = (TO_SO_HEAD&)pSendBuf[0];							// ヘッダ部エイリアス
	TO_SO_DATA_CUT&		data = (TO_SO_DATA_CUT&)pSendBuf[sizeof(TO_SO_HEAD)];		// データ部エイリアス

	//// データセット
	// ヘッダー部(ベース)
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len		= nSocnSize;
	head.base_head.addr[0]	= NULL;
	head.base_head.addr[1]	= NULL;

	// ヘッダー部(拡張)
	memcpy( head.KizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);
	head.men				= -1;												// 表裏関係無し
	head.kind				= TO_SO_DATA_KIND_CUT;								// カット情報

	// データ部(構造体)
	memcpy( &data, pInf, sizeof(TO_SO_DATA_CUT));


	//================================================
	//// 送信依頼
	LOG(em_MSG), "[%s] 統括⇒表示間  カット情報等 [%s][%d:%dmm]", my_sThreadName, head.KizukenNo, data.nMode, data.nPos );
	CString sWk;
	for(int ii=0;ii<HY_SOCK_NUM; ii++) {
		pSend = new BYTE[nSocnSize];
		memcpy(pSend, pSendBuf, nSocnSize);
		sWk.Format("%s_%d", INI_OP_SEND, ii + 1);
		if( ! mque_pOpSend[ii]->AddToTail((SOCK_BASE_HEAD*)pSend, 10, 20 ) ) {		// 登録
			LOG(em_ERR), "[%s] <%s> 統括⇒表示間 カット情報等送信依頼キューフル", my_sThreadName, sWk.operator LPCSTR());
			syslog(SYSNO_QUEFULL_WAR, "[カット情報] <%s>]", cKizukenNo);
			delete pSend;
			break;
		}
	}
	delete pSendBuf;
}

//==========================================
// 警報状態を表示機能へ
//------------------------------------------
// ALARME_OUTPUT const* pInf 対象のアラーム情報
//==========================================
void OpSendFunc::SendOp_AlarmData(ALARME_OUTPUT const* pInf)
{
	int					nSocnSize;				// 送信伝文長
	BYTE*				pSendBuf;				// 実領域
	BYTE*				pSend;				// 送信用バッファ
	
	//// 出力条件チェック
	for(int ii=0;ii<HY_SOCK_NUM; ii++) {
		if( NULL == mque_pOpSend[ii] ) return;
	}
	if( NULL == pInf) return;

	//================================================
	////// 表示機能へコイル情報通知
	//// 送信領域確保
	nSocnSize = sizeof(TO_SO_HEAD) + sizeof(TO_SO_DATA_ALARM);
	pSendBuf  = new BYTE[nSocnSize];
	memset(pSendBuf, 0x00, nSocnSize);
	TO_SO_HEAD&			head = (TO_SO_HEAD&)pSendBuf[0];							// ヘッダ部エイリアス
	TO_SO_DATA_ALARM&	data = (TO_SO_DATA_ALARM&)pSendBuf[sizeof(TO_SO_HEAD)];		// データ部エイリアス


	//// データセット
	// ヘッダー部(ベース)
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len		= nSocnSize;
	head.base_head.addr[0]	= NULL;
	head.base_head.addr[1]	= NULL;

	// ヘッダー部(拡張)
	head.men				= -1;												// 表裏関係無し
	head.kind				= TO_SO_DATA_KIND_ALARM;							// 疵接近ランプ情報

	// データ部(構造体)
	for (int ii = 0; ii < MAX_ALARME_POS; ii++)
	{
		switch (ii)
		{
			// 表面検査台
		case 0:
			// 疵接近ランプ
			for (int jj = 0; jj<NUM_MEN; jj++)
			{
				for (int kk = 0; kk < LAMP_COL_NUM; kk++)
				{
					// 重疵
					if (em_RAMP_STAT_ON == pInf->ranp[jj][kk][em_RAMP_JYU])		data.a_mode[ii][jj][kk] = 2;
					// 軽疵
					else if (em_RAMP_STAT_ON == pInf->ranp[jj][kk][em_RAMP_KEI])data.a_mode[ii][jj][kk] = 1;
				}
			}
			break;
			// 裏面検査台
		case 1:
			// 疵接近アラーム
			for (int jj = 0; jj<NUM_MEN; jj++)
			{
				for (int kk = 0; kk < LAMP_COL_NUM; kk++)
				{
					// 重疵
					if (em_RAMP_STAT_ON == pInf->alarm[jj][kk][em_RAMP_JYU])	data.a_mode[ii][jj][kk] = 2;
					// 軽疵
					else if (em_RAMP_STAT_ON == pInf->alarm[jj][kk][em_RAMP_KEI])data.a_mode[ii][jj][kk] = 1;
				}
			}
			break;
		}
	}

	//================================================
	//// 送信依頼
	CString sWk;
	for(int ii=0;ii<HY_SOCK_NUM; ii++) {
		pSend = new BYTE[nSocnSize];
		memcpy(pSend, pSendBuf, nSocnSize);
		sWk.Format("%s_%d", INI_OP_SEND, ii + 1);
		if( ! mque_pOpSend[ii]->AddToTailFreeCheck((SOCK_BASE_HEAD*)pSend, 7) ) {		// 登録
			LOG(em_ERR), "[%s] <%s> 統括⇒表示間 アラーム情報送信依頼キューフル", my_sThreadName, sWk.operator LPCSTR());
			syslog(SYSNO_QUEFULL_WAR, "[アラーム情報]");
			delete pSend;
			break;
		}
	}
	delete pSendBuf;
}



//==========================================
// 疵一覧表示情報削除要求を表示機能へ
//------------------------------------------
// char const* cKizukenNo 管理No
// int nMen 面番号	0:表, 1:裏
// int nKizuSeq キャンセル対象の疵連番
//==========================================
void OpSendFunc::SendOp_PatoReset(char const* cKizukenNo, int nMen, int nKizuSeq)
{
	int					nSocnSize;				// 送信伝文長
	BYTE*				pSendBuf;				// 実領域
	BYTE*				pSend;				// 送信用バッファ
	
	//// 事前チェック
	for(int ii=0;ii<HY_SOCK_NUM; ii++) {
		if( NULL == mque_pOpSend[ii] ) return;
	}
	if( NULL == cKizukenNo) return;

	//================================================
	////// 表示機能へ通知
	//// 送信領域確保
	nSocnSize = sizeof(TO_SO_HEAD);
	pSendBuf = new BYTE[nSocnSize];
	memset(pSendBuf, 0x00, nSocnSize);
	TO_SO_HEAD&			head = (TO_SO_HEAD&)pSendBuf[0];							// ヘッダ部エイリアス


	//// データセット
	// ヘッダー部(ベース)
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len = nSocnSize;
	head.base_head.addr[0] = NULL;
	head.base_head.addr[1] = NULL;

	// ヘッダー部(拡張)
	memcpy(head.KizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);
	head.men = nMen;											// 表裏関係無し
	head.kind = TO_SO_DATA_KIND_DEFLIST_CANCEL;					// 疵一覧表疵情報削除要求
	head.deflist_cancel.nKizuSeq = nKizuSeq;


	//================================================
	CString sWk;
	for (int ii = 0; ii < HY_SOCK_NUM; ii++) {
		pSend = new BYTE[nSocnSize];
		memcpy(pSend, pSendBuf, nSocnSize);
		sWk.Format("%s_%d", INI_OP_SEND, ii + 1);
		//// 送信依頼
		if (!mque_pOpSend[ii]->AddToTailFreeCheck((SOCK_BASE_HEAD*)pSend, 7)) {		// 登録
			LOG(em_ERR), "[%s] <%s> 統括⇒表示間 パトライト疵リセット依頼キューフル", my_sThreadName, sWk.operator LPCSTR());
			syslog(SYSNO_QUEFULL_WAR, "[アラーム情報]");
			delete pSend;
			break;
		}
	}
	delete pSendBuf;
}


//==========================================
// 疵個数情報を表示機能へ
//------------------------------------------
// char const* cKizukenNo 管理No
// int nMen 面番号	0:表, 1:裏
// long nLen 実測長 [mm]
// long nNum 疵個数
//==========================================
void OpSendFunc::SendOp_DetectNum(char const* cKizukenNo, int nMen, long nLen, long nNum)
{
	int					nSocnSize;							// 送信伝文長
	BYTE*				pSendBuf;							// 実領域
	BYTE*				pSend;								// 送信用バッファ

	//// 事前チェック
	for (int ii = 0;ii<HY_SOCK_NUM; ii++) {
		if (NULL == mque_pOpSend[ii]) return;
	}
	if (NULL == cKizukenNo) return;

	//================================================
	////// 表示機能へ通知
	//// 送信領域確保
	nSocnSize = sizeof(TO_SO_HEAD);
	pSendBuf = new BYTE[nSocnSize];
	memset(pSendBuf, 0x00, nSocnSize);
	TO_SO_HEAD&			head = (TO_SO_HEAD&)pSendBuf[0];	// ヘッダ部エイリアス

	//// データセット
	// ヘッダー部(ベース)
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len = nSocnSize;
	head.base_head.addr[0] = NULL;
	head.base_head.addr[1] = NULL;

	// ヘッダー部(拡張)
	memcpy(head.KizukenNo, cKizukenNo, SIZE_KIZUKEN_NO);	// 管理No
	head.men = nMen;										// 表裏区分
	head.kind = TO_SO_DATA_KIND_DETECT_NUM;					// 疵個数情報 (最小区間内)

	// ヘッダー部(拡張)
	head.data_defnum.len = nLen;							// 実測長 [mm]
	head.data_defnum.num = nNum;							// 疵個数情(最小区間内)

	//================================================
	CString sWk;
	for (int ii = 0; ii < HY_SOCK_NUM; ii++) {
		pSend = new BYTE[nSocnSize];
		memcpy(pSend, pSendBuf, nSocnSize);
		sWk.Format("%s_%d", INI_OP_SEND, ii + 1);
		//// 送信依頼
		if (!mque_pOpSend[ii]->AddToTailFreeCheck((SOCK_BASE_HEAD*)pSend, 7)) {		// 登録
			LOG(em_ERR), "[%s] <%s> 統括⇒表示間 パトライト疵リセット依頼キューフル", my_sThreadName, sWk.operator LPCSTR());
			syslog(SYSNO_QUEFULL_WAR, "[アラーム情報]");
			delete pSend;
			break;
		}
	}
	delete pSendBuf;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 疵情報

//------------------------------------------
// 表示機能に最小区間の行列情報を送信
// int men	0:表, 1:裏
// COIL_INF const*	pCoilInf 疵が含まれるコイル情報
// long y 送信行位置
// TYP_COLROW_BUF BufDefCol[] 代表疵情報
// TYP_COLROW_BUF BufDefAry[][MAP_NEST_NUM] リスト疵情報
// int nDefCnt 現在の最小区間の疵数
//------------------------------------------
void OpSendFunc::SendOp_ColRowData(int men, COIL_INF const* pCoilInf, long y, TYP_COLROW_BUF BufDefCol[], TYP_COLROW_BUF BufDefAry[][MAP_NEST_NUM], int nDefCnt)
{
	int nTopDefCnt = 0;

	// 列数分ループ
	for (int ii = 0; ii < MAP_COL_NUM; ii++)
	{
		if (BufDefCol[ii].umu)
		{
			// 代表疵数加算
			nTopDefCnt++;
		}
	}

	// 代表疵が存在しない場合＝送信データ無し
	if (0 == nTopDefCnt) return;

	// 列数分ループ
	for (int ii = 0; ii<MAP_COL_NUM; ii++)
	{
		if (!BufDefCol[ii].umu) continue;

		// 代表疵情報 (疵情報+疵画像)送信
		SendOp_ColRowData(men, pCoilInf, &BufDefCol[ii]);
	}

	// 列数分ループ
	for (int ii = 0; ii < MAP_COL_NUM; ii++)
	{
		// リスト数分ループ
		for (int jj = 0; jj < MAP_NEST_NUM; jj++)
		{
			if (!BufDefAry[ii][jj].umu) continue;

			// リスト疵情報 (疵情報+疵画像)送信
			SendOp_ColRowData(men, pCoilInf, &BufDefAry[ii][jj]);
		}
	}

#ifdef DEF_DETECT_LOG
	LOG(em_INF), "[%s] 統括⇒表示間 疵情報送信 [長手位置=%d 疵数=%d]", my_sThreadName,
		y * (MAP_REPRES_MIN * 1000), nDefCnt);
#endif
	OpSendFunc::SendOp_DetectNum(pCoilInf->cKizukenNo, men, y * (MAP_REPRES_MIN * 1000), nDefCnt);
}
//------------------------------------------
// 表示機能に最小区間の行列情報を送信
// int men	0:表, 1:裏
// COIL_INF const*	pCoilInf 疵が含まれるコイル情報
// TYP_COLROW_BUF* pData 送信情報
//------------------------------------------
void OpSendFunc::SendOp_ColRowData(int men, COIL_INF const* pCoilInf, TYP_COLROW_BUF* pData)
{
	int					jj;
	int					nSocnSize;				// 送信伝文長
	BYTE*				pSendBuf;				// 実領域
	BYTE*				pSend;					// 送信用バッファ
	int					nTotalLen = 0;			// 全画像サイズ
	int					nWk = 0;

	////================================================
	//// 送信領域確保

	// 画像部データ計算
	for (jj = 0; jj<MAX_CAMANGLE; jj++) nTotalLen += pData->nImgsize[jj];

	// 送信領域確保 (-4 は、本来-1だがバウンダリがある為の処置)
	nSocnSize = sizeof(TO_SO_HEAD) + sizeof(TO_SO_DATA_DETECT) - 4 + nTotalLen;		// この時点では、まだ仮
	pSendBuf = new BYTE[nSocnSize];
	memset(pSendBuf, 0x00, nSocnSize);
	TO_SO_HEAD&			head	= (TO_SO_HEAD&)pSendBuf[0];							// ヘッダ部エイリアス
	TO_SO_DATA_DETECT&	data	= (TO_SO_DATA_DETECT&)pSendBuf[sizeof(TO_SO_HEAD)];	// データ部エイリアス
	BYTE*				pJpegBuf= data.image;										// 画像データ先頭ポインタ

	//================================================
	// 通常のデータセット
	//// データセット (実コイルが切り替わってもまだ前回のコイルの情報を送信する。だって前の情報だもん)
	// ヘッダー部(ベース)
	memcpy(head.base_head.code, T_CODE_DEFECT, SIZE_SOCK_TC);
	strcpy(head.base_head.pname, TOKATU);
	head.base_head.len		= nSocnSize;
	head.base_head.addr[0]	= NULL;
	head.base_head.addr[1]	= NULL;

	// ヘッダー部(拡張)
	memcpy(head.KizukenNo, pCoilInf->cKizukenNo, SIZE_KIZUKEN_NO);
	head.men	= men;
	head.kind	= TO_SO_DATA_KIND_DETECT;	// 疵情報 (行列情報)

	// データ部(構造体)
	memcpy(&data.datas, &pData->Data, sizeof(DETECT_BASE_DATA));

	// 入側先端からの位置[mm]
	data.datas.y = (float)pData->dLen;

	//----------------------------------------
	// 個々の画像データを送信領域にセット
	nWk = 0;
	for (int ii = 0; ii < MAX_IMAGE_CNT; ii++)
	{
		// 疵画像は存在するか？
		if (0 >= pData->nImgsize[ii])
		{
			data.image_len[ii] = 0;
			continue;
		}
		// 表示対象の疵画像か？
		if (!mcls_pParam->Check_ImgDsp(pData->Data.nTid, ii))
		{
			data.image_len[ii] = 0;
			continue;
		}
		memcpy(&pJpegBuf[nWk], pData->pImg[ii], pData->nImgsize[ii]);
		nWk += pData->nImgsize[ii];

		data.image_len[ii] = pData->nImgsize[ii];
	}

	//◆
#ifdef DEF_DETECT_LOG
	LOG(em_MSG), "[%s] 統括⇒表示間 送信行列 [%s][%s][t=%d][g=%d][%d][WidDiv=%d][y=%ldmm]", my_sThreadName, head.KizukenNo, DivNameManager::GetTorB(head.men),
		data.datas.nTid, data.datas.nGid, data.datas.nKizuNo, data.datas.nWidDiv, (long)data.datas.y);
#endif

	//================================================
	//// 送信依頼
	//================================================
	CString sWk;
	for (int ii = 0; ii < HY_SOCK_NUM; ii++) {
		pSend = new BYTE[nSocnSize];
		memcpy(pSend, pSendBuf, nSocnSize);
		sWk.Format("%s_%d", INI_OP_SEND, ii + 1);
		if (!mque_pOpSend[ii]->AddToTailFreeCheck((SOCK_BASE_HEAD*)pSend, 10)) {
			delete pSend;
			LOG(em_ERR), "[%s] <%s> <%s> 統括⇒表示間 疵情報送信依頼キューフル", my_sThreadName, sWk.operator LPCSTR(), DivNameManager::GetTorB(men));
			syslog(SYSNO_QUEFULL_WAR, "[%s TO_SO_HEAD TO_SO_DATA_KIND_DETECT]", DivNameManager::GetTorB(men));
		}
	}
	delete pSendBuf;

	// バッファー領域クリア
	pData->umu = false;
	for (jj = 0; jj < MAX_CAMANGLE; jj++)
	{
		if (0 < pData->nImgsize[jj]) {
			SAFE_DELETE_ARRAY(pData->pImg[jj]);
		}
	}
}
