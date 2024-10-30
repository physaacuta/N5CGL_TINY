#include "StdAfx.h"
#include "ParamManager.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

#include "MainInstance.h"

//------------------------------------------
// コンストラクタ
//------------------------------------------
ParamManager::ParamManager(void) :
ThreadManager("ParamMgr")
{
	m_evReadEnd = CreateEvent(NULL, FALSE, FALSE, NULL);
}

//------------------------------------------
// デストラクタ
//------------------------------------------
ParamManager::~ParamManager(void)
{
	CloseHandle(m_evReadEnd);
}


//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int ParamManager::ThreadFirst()
{

	//// イベント設定
	HANDLE hArray[] = { gque_Deli.g_evSem
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッド終了前処理 (終了シグナル後にコールバック)
//------------------------------------------
int ParamManager::ThreadLast()
{
	return 0;
}

//------------------------------------------
// スレッドイベント発生
// int nEventNo イベントNo (0オリジン)
//------------------------------------------
void ParamManager::ThreadEvent(int nEventNo)
{
	////// シグナルの条件
	enum {	EV_QUE = 0,						// パラメータ変更通知
	};

	////// シグナル条件分け
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_QUE: 							// パラメータ変更通知
		COMMON_QUE*	pDeli;
		pDeli = gque_Deli.GetFromHead();
		ExecParam(pDeli);
		delete pDeli;

		// パラメータ読込完了通知
		SetEvent(m_evReadEnd);
		return;

	
//-----------------------------------------------------------------------------------------------
	default :
		// ありえない！！
		return;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 読み込み制御
void ParamManager::ExecParam(COMMON_QUE* pQue)
{
	int dly	  = pQue->fl.data[0];
	int id    = pQue->fl.data[1];			// テーブルID
	int scode = pQue->fl.data[2];			// 基準面
	int scode_men[NUM_MEN];					// 面固定
	for(int ii=0; ii<NUM_MEN; ii++) scode_men[ii] = pQue->fl.data[3+ii];

	//================================================
	//// 他タスクへパラメータ変更通知
	// 判定機能 へ通知
	switch( id ) {
		case CH_PARAM_ALL:					// 全部
		case CH_PARAM_TYPE_GRADE:			// 疵種・Gr
		case CH_PARAM_COMMON:				// 固定パラメータ
		case CH_PARAM_CAMERA:				// カメラ設定
		case CH_PARAM_EDGE:					// エッジ検出
		case CH_PARAM_SHD:					// シェーディング補正
		case CH_PARAM_THRESHOLD:			// 閾値設定
		case CH_PARAM_DETECT:				// 疵検出
		case CH_PARAM_CYCLE:				// 周期疵判定
		//case CH_PARAM_MERGE:				// カメラ境界処理 (疵統合)
		//case CH_PARAM_LIGHT:				// 照明
		case CH_PARAM_TINY:					// 微小欠陥
		case CH_PARAM_SYNCADD:				// 画像加算
		//case CH_PARAM_YOUSET:				// 溶接点検出
		case CH_PARAM_COLOR_DIV:			// 混色分離
		case CH_PARAM_PCINFODESC:			// 上位設定情報
		case CH_PARAM_TAJUUTHRESHOLD:		// 多重分解能閾値設定
		//case CH_PARAM_CYCLETHRESHOLD:		// 周期用分解能閾値設定
		case CH_PARAM_REDUCTION:			// 縮小率設定
		case CH_PARAM_FILTER:				// フィルタ設定
		case CH_PARAM_PARA:					// ロジックパララン設定
		case CH_PARAM_CROP_SHRINK:			// 間引き設定

			for(int ii=0; ii<NUM_MEN; ii++) {
				MainInstance::Send_HtDefect_PreLoadParam(ii, DIV_HANTEI_NON, scode_men[ii]);	// パラメータ事前読込通知
				MainInstance::Send_HtDefect_Param(dly, ii, scode_men[ii] );
			}
			break;
	}

	// カメラコントローラーへ送信
	switch( id ) {
		case CH_PARAM_ALL:					// 全部
		case CH_PARAM_COMMON:				// 固定パラメータ
		case CH_PARAM_CAMERA:				// カメラ設定

			ToOutMailSender::Send_ToCamera_Param(scode);
			break;
	}

	// カメラリンクスイッチ通信
	switch( id )
	{
		case CH_PARAM_ALL:						// 全部
		case CH_PARAM_COMMON:					// 固定パラメータ

			for(int ii=0; ii<NUM_MEN; ii++)	ToOutMailSender::Send_ToLinksw_Param( scode_men[ii] );
			break;
	}

	// 照明装置制御 へ送信
	switch( id ) {
		case CH_PARAM_ALL:					// 全部
		case CH_PARAM_LIGHT:				// 照明
#ifndef KT_LEDMGR
			ToOutMailSender::Send_ToLights_Param(scode);
#else
			ToOutMailSender::Send_KtLedMgr_Param(scode);
#endif
		break;
	}

	// 表示管理 へ送信
	switch( id ) {
		case CH_PARAM_ALL:					// 全部	
		case CH_PARAM_TYPE_GRADE:			// 疵種・Gr
		case CH_PARAM_TINY:					// 微小欠陥設定
		case CH_PARAM_SYNCADD:				// 画像加算 (周期用)
		case CH_PARAM_SETTIG_PCODE:			// 検査パターン マスター・条件設定・出力設定
		case CH_PARAM_SETTIG_SCODE:			// 検査表面状態 マスター・条件設定・出力設定  探傷画面にscodeの名称を出しているため
		case CH_PARAM_SETTIG_COMMON:		// 定数設定
//		case CH_PARAM_SETTIG_PCOMMON:		// 検査パターン 固有 出力設定 5CGL極微小では使用しない
		case CH_PARAM_SETTIG_PALL:			// 検査パターン 共通 出力設定
		case CH_PARAM_MAE_MAST:				// 前工程設定
		case CH_PARAM_DEFLIST:				// 疵一覧
		case CH_PARAM_SETTIG_ALARM:			// 疵検警報出力設定

			MainInstance::Send_HyDefect_Param(id);
			break;
	}

	// 疵点マップ画面 へ送信
	switch (id) {
	case CH_PARAM_ALL:						// 全部	
	case CH_PARAM_TYPE_GRADE:				// 疵種・Gr
	case CH_PARAM_SETTIG_PCODE:				// 検査パターン マスター・条件設定・出力設定
		MainInstance::Send_SoDefMap_Param(id);
		break;
	}

	//// 入力データ編集管理 へ送信
	//switch( id ){
	//	case CH_PARAM_ALL:					// 全部
	//	case CH_PARAM_SETTIG_SCODE:			// 検査表面状態 マスター・条件設定・出力設定
	//	case CH_PARAM_SETTIG_PCODE:			// 検査パターン マスター・条件設定・出力設定
	//	case CH_PARAM_SETTIG_COMMON:		// 固定パラメータ

	//		MainInstance::Send_ToGetdat_Param(id);

	//}

	//// 実績編集管理 へ送信
	//switch( id ){
	//	case CH_PARAM_ALL:					// 全部
	//	case CH_PARAM_TYPE_GRADE:			// 疵種・Gr
	//	case CH_PARAM_SETTIG_PCODE:			// 検査パターン マスター・条件設定・出力設定
	//	case CH_PARAM_SETTIG_COMMON:		// 固定パラメータ

	//		MainInstance::Send_ToPutdat_Param(id);

	//}

	//// 前工程実績編集管理 へ送信
	//switch( id ){
	//	case CH_PARAM_ALL:					// 全部
	//	case CH_PARAM_MAE_MAST:				// 前工程設定

	//		MainInstance::Send_ToMaedat_Param(id);

	//}


	//================================================
	//// 自分のパラメータ変更
	switch( id ) {
		case CH_PARAM_ALL:					// 全部
			Read_CommonMast();
			Read_TypeName();
			Read_GradeName();
			Read_ScodeOutput();							// 疵種Grの設定により、変更される可能性有り
			Read_PcodeOutput();
			Read_PallOutput(); 
			//Read_PCommonOutput();			//5CGL極微小では使用しない

			Read_Param_Common();
			Read_Param_CamSet();
			Read_Param_Scode();
			Read_Param_Camset_Scode();
			Read_Param_Camset_Angle();
			Read_Param_Camset_Angle_Scode();
			break;

		case CH_PARAM_TYPE_GRADE:			// 疵種・Gr
			Read_TypeName();
			Read_GradeName();
			Read_ScodeOutput();							// 疵種Grの設定により、変更される可能性有り
			Read_PcodeOutput();
			Read_PallOutput(); 
			//Read_PCommonOutput();			//5CGL極微小では使用しない
			break;

	//CH_PARAM_ATTR_NAME
	//CH_PARAM_PCINFODESC

		case CH_PARAM_COMMON:				// 固定パラメータ
			Read_Param_Common();
			Read_Param_CamSet();
			Read_Param_Scode();
			break;

		case CH_PARAM_CAMERA:				// カメラ設定
			Read_Param_Scode();
			Read_Param_Camset_Scode();
			Read_Param_Camset_Angle();
			Read_Param_Camset_Angle_Scode();
			break;	

		case CH_PARAM_EDGE:					// エッジ検出
			Read_Param_Scode();
			Read_Param_Camset_Scode();
			Read_Param_Camset_Angle_Scode();
			break;	

	//CH_PARAM_SHD
	//CH_PARAM_THRESHOLD
	//CH_PARAM_DETECT
	//CH_PARAM_CYCLE

		case CH_PARAM_MERGE:				// 疵統合
			Read_Param_Common();
			break;

	//CH_PARAM_LIGHT

 		case CH_PARAM_TINY:					// 微小欠陥設定
			Read_CommonMast();
			Read_Param_Scode();
			break;

 		case CH_PARAM_SYNCADD:		        // 画像加算
			break;

	//CH_PARAM_YOUSET
	//CH_PARAM_COLOR_DIV


		case CH_PARAM_SETTIG_SCODE:			// 検査表面状態 マスター・条件設定・出力設定
			Read_ScodeOutput();
			break;

		case CH_PARAM_SETTIG_PCODE:			// 検査パターン マスター・条件設定・出力設定
			Read_PcodeOutput();
			break;

		case CH_PARAM_SETTIG_COMMON:		// 定数設定
			Read_CommonMast();
			//// 判定PC側でJPEG圧縮させる
			MainInstance::Send_HtDefect_JpegQuality(GetCommonMast().nJpegQuality);
			break;

		//5CGL極微小では使用しない
		//case CH_PARAM_SETTIG_PCOMMON:		// 検査パターン 固有 出力設定
		//	Read_PCommonOutput();
		//	break;

		case CH_PARAM_SETTIG_PALL:			// 検査パターン 共通 出力設定
			Read_PallOutput(); 
			break;

	//CH_PARAM_SETTIG_LOG
	//CH_PARAM_SETTIG_STATUS

		// ライン独自 ----------------------------- 
		//case CH_PARAM_SETTIG_GAP:			// 複眼ズレ補正条件設定
		//	Read_CommonMast();
		//	Read_Param_CamSet();
		//	break;

		// 前工程設定
		case CH_PARAM_MAE_MAST:
			break;

		// 多重分解能閾値設定
		case CH_PARAM_TAJUUTHRESHOLD:
			break;

		//// 周期用分解能閾値設定
		//case CH_PARAM_CYCLETHRESHOLD:
		//	break;

		// 縮小率設定
		case CH_PARAM_REDUCTION:
			break;

		// フィルタ設定
		case CH_PARAM_FILTER:
			break;

		// 疵一覧設定
		case CH_PARAM_DEFLIST:
			Read_CommonMast();
			Read_PallOutput(); 
			break;

		case CH_PARAM_SETTIG_ALARM:
			Read_CommonMast();
			break;
	}

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 出力条件チェック

//==========================================
// 疵種EdasysID の有無
//------------------------------------------
// long		tid		: 疵種EdasysID	
// 戻り値			: 復帰情報
//==========================================
bool ParamManager::CheckEdasysID_T(long tid) const 
{
	if(0==tid) return false;
	for(int ii=0; ii<m_TypeName.num; ii++) {
		if( m_TypeName.Type_Id[ii].nEdasysID == tid ) return true;
	}
	return false;
}

//==========================================
// グレードEdasysID の有無
//------------------------------------------
// long		tid		: グレードEdasysID
// 戻り値			: 復帰情報
//==========================================
bool ParamManager::CheckEdasysID_G(long gid) const 
{
	if(0==gid) return false;
	for(int ii=0; ii<m_GradeName.num; ii++) {
		if( m_GradeName.Grade_Id[ii].nEdasysID == gid ) return true;
	}
	return false;
}

//==========================================
// 疵種優先度取得
//------------------------------------------
// long		tid		: 疵種EdasysID
// 戻り値			: 優先度 (未検出-1)
//==========================================
int ParamManager::GetYusenT(long tid) const 
{
	for(int ii=0; ii<m_TypeName.num; ii++) {
		if( m_TypeName.Type_Id[ii].nEdasysID == tid ) return m_TypeName.Type_Id[ii].nPriority;
	}
	return -1;
}

//==========================================
// グレード優先度取得
//------------------------------------------
// long		gid		: グレードEdasysID
// 戻り値			: 優先度 (未検出-1)
//==========================================
int ParamManager::GetYusenG(long gid) const
{
	for(int ii=0; ii<m_GradeName.num; ii++) {
		if( m_GradeName.Grade_Id[ii].nEdasysID == gid ) return m_GradeName.Grade_Id[ii].nPriority;
	}
	return -1;
}

//==========================================
// 欠陥グレード取得
//------------------------------------------
// long		gid		: グレードEdasysID
// 戻り値			: 欠陥グレード (未検出-1)
//==========================================
int ParamManager::GetRankG(long gid) const
{
	for(int ii=0; ii<m_GradeName.num; ii++) {
		if( m_GradeName.Grade_Id[ii].nEdasysID == gid ) return m_GradeName.Grade_Id[ii].rank;
	}
	return -1;
}

//==========================================
// 出力対象の欠陥か判定
//------------------------------------------
// EM_OUTPUTFLG	output	: どの出力条件をチェックするの？
// long			pcode	: 検査パターン (0オリジン)
// long			tid		: 疵種EdasysID
// long			gid		: グレードEdasysID
// 戻り値				: true:対象
//==========================================
bool ParamManager::Check_Output(EM_OUTPUTFLG output, long pcode, long tid, long gid) const
{
	// 引数チェック
	if( 0 > pcode || MAX_PCODE <= pcode ) {
		LOG(em_ERR), "[%s] output=%d pcodeNG=%d", my_sThreadName, output, pcode);	
		return false;
	}

	//// 疵種EdasysID 指定有り？
	for(int ii=0; ii<m_PcodeOutput.rec[pcode].nPutNum; ii++) {
		if( tid == m_PcodeOutput.rec[pcode].output[ii].edasysID ) {
			// Gr設定無しの疵種の為 全部対象外
			if( 0 == m_PcodeOutput.rec[pcode].output[ii].kind[output] ){
				return false;
			}

			// Gr設定有りの疵種の為 優先度比較
			if( GetYusenG(gid) <= GetYusenG(m_PcodeOutput.rec[pcode].output[ii].kind[output]) )	return true;
			else																				return false;
		}
	}

	//// 無いときは デフォルト指定 (ココまで来た場合は、疵種設定無しの時)
	// Gr設定無しの疵種の為 全部対象外
	if( 0 == m_PcodeOutput.rec[pcode].defput.kind[output] ) return false;

	// Gr設定有りの疵種の為 優先度比較
	if( GetYusenG(gid) <= GetYusenG(m_PcodeOutput.rec[pcode].defput.kind[output]) )	return true;
	else																			return false;
}

//==========================================
// 出力対象の欠陥か判定
//------------------------------------------
// EM_OUTPUTFLG	output	: どの出力条件をチェックするの？
// long			pcode	: 検査パターン (0オリジン)
// long			tid		: 疵種EdasysID
// long			gid		: グレードEdasysID
// 戻り値				: -9:対象外指定  -1:より小さい  0:同じ  1:より大きい
//==========================================
int ParamManager::Check_OutputEx(EM_OUTPUTFLG output, long pcode, long tid, long gid) const
{
	int nVal, nPrm;

	// 引数チェック
	if( 0 > pcode || MAX_PCODE <= pcode ) {
		LOG(em_ERR), "[%s] output=%d pcodeNG=%d", my_sThreadName, output, pcode);	
		return -9;
	}

	//// 疵種EdasysID 指定有り？
	for(int ii=0; ii<m_PcodeOutput.rec[pcode].nPutNum; ii++) {
		if( tid == m_PcodeOutput.rec[pcode].output[ii].edasysID ) {
			// Gr設定無しの疵種の為 全部対象外
			if( 0 == m_PcodeOutput.rec[pcode].output[ii].kind[output] ) return -9;

			// Gr設定有りの疵種の為 優先度比較
			nVal = GetYusenG(gid);
			nPrm = GetYusenG(m_PcodeOutput.rec[pcode].output[ii].kind[output]);
			if( nVal == nPrm )		{ return 0; }
			else if( nVal < nPrm )	{ return 1; }
			else					{ return -1;}
		}
	}

	//// 無いときは デフォルト指定 (ココまで来た場合は、疵種設定無しの時)
	// Gr設定無しの疵種の為 全部対象外
	if( 0 == m_PcodeOutput.rec[pcode].defput.kind[output] ) return -9;

	// Gr設定有りの疵種の為 優先度比較
	nVal = GetYusenG(gid);
	nPrm = GetYusenG(m_PcodeOutput.rec[pcode].defput.kind[output]);
	if( nVal == nPrm )		{ return 0; }
	else if( nVal < nPrm )	{ return 1; }
	else					{ return -1;}
}

//==========================================
// 検査パターン共通出力対象の欠陥か判定
//------------------------------------------
// EM_OUTPUT_PALLFLG	output		: どの出力条件をチェックするの？
// long					tid			: 疵種EdasysID
// long					gid			: グレードEdasysID
// bool					bUp = true	: (true:指定条件以上で対象  false:指定条件以下で対象)
// 戻り値							: true:対象
//==========================================
bool ParamManager::Check_Pall_Output(EM_OUTPUT_PALLFLG output, long tid, long gid, bool bUp) const
{
	//// 疵種EdasysID 指定有り？
	for(int ii=0; ii<m_PallOutput.nPutNum; ii++) {
		if( tid == m_PallOutput.output[ii].edasysID ) {
			// Gr設定無しの疵種の為 全部対象外
			if( 0 == m_PallOutput.output[ii].kind[output] ) return false;

			// Gr設定有りの疵種の為 優先度比較
			if(bUp) {
				if( GetYusenG(gid) <= GetYusenG(m_PallOutput.output[ii].kind[output]) )	return true;
				else																	return false;
			} else {
				if( GetYusenG(gid) >= GetYusenG(m_PallOutput.output[ii].kind[output]) )	return true;
				else																	return false;
			}
		}
	}

	//// 無いときは デフォルト指定 (ココまで来た場合は、疵種設定無しの時)
	// Gr設定無しの疵種の為 全部対象外
	if( 0 == m_PallOutput.defput.kind[output] ) return false;

	// Gr設定有りの疵種の為 優先度比較
	if(bUp) {
		if( GetYusenG(gid) <= GetYusenG(m_PallOutput.defput.kind[output]) )	return true;
		else																return false;
	} else {
		if( GetYusenG(gid) >= GetYusenG(m_PallOutput.defput.kind[output]) )	return true;
		else																return false;
	}
}

//------------------------------------------
// 疵種に対して疵画像の表示対象かチェック
// long tid		疵種EdasysID
// int index	画像種類 (0オリジン)
// 戻り値 true:対象
//------------------------------------------
bool ParamManager::Check_ImgDsp(long tid, int index) const
{

	// 複眼
	//return true;

	// 色差
	if(0==tid) return false;
	if(index>=MAX_IMAGE_CNT) return false;
	for(int ii=0; ii<m_TypeName.num; ii++) {
		if( m_TypeName.Type_Id[ii].nEdasysID == tid ) {
			return m_TypeName.Type_Id[ii].bEnable[ index ];
		}
	}

	return false;
}

//------------------------------------------
// 計算値縦分解能 (完全固定分解能としたいため)
// int idx	チャンネル (1:ﾗｲﾝPLG  2:表校正用パルス 3:裏校正用パルス 4:テストパルス)  
// 戻り値				: 縦分解能[mm/pixel]
//------------------------------------------
double ParamManager::CalcResV(int idx)
{
	double dResV = 1.0;		// 縦分解能
	dResV = m_Param_Common.dPls * m_Param_Common.typCamCon[idx-1].nDiv / m_Param_Common.typCamCon[idx-1].nMul;
	return dResV;
}

//------------------------------------------
// 計算値１フレームカウンタ値 (完全固定分解能としたいため)
// int idx	チャンネル (1:ﾗｲﾝPLG  2:表校正用パルス 3:裏校正用パルス 4:テストパルス)  
// 戻り値				: カウンタ値(1フレーム) [pls]
//------------------------------------------
double ParamManager::CalcSampBuff(int idx)
{
	double dSampBuff = 0.0;		// カウンタ値(1フレーム)
	dSampBuff = (int)(SIZE_FRAME_IMG_Y * m_Param_Common.typCamCon[idx-1].nDiv / m_Param_Common.typCamCon[idx-1].nMul);
	return dSampBuff;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// 今回固有の公開関数
//------------------------------------------
// ボイス出力条件の優先度からDB項目を取得
// ENUM_VOICE_DEF_TYPE emViceDefType	:  
// 戻り値				: カウンタ値(1フレーム) [pls]
//------------------------------------------
ParamManager::EM_OUTPUTFLG ParamManager::GetOutputFlg(ENUM_VOICE_PRI_TYPE emViceDefType)
{
	switch (emViceDefType) {
	case em_VOICE_PRI_OT:	return OUTPUTFLG_VOICE_OTHER;		// 疵接近
	case em_VOICE_PRI_BI:	return OUTPUTFLG_VOICE_CYCLE;		// 周期疵 単発
	case em_VOICE_PRI_SY:	return OUTPUTFLG_VOICE_CYCLE;		// 周期疵
	case em_VOICE_PRI_TB:	return OUTPUTFLG_VOICE_SAME;		// 表裏同一疵
	case em_VOICE_PRI_SU:	return OUTPUTFLG_VOICE_SURI;		// スリバ
	case em_VOICE_PRI_HE:	return OUTPUTFLG_VOICE_HEGE;		// ヘゲ
	case em_VOICE_PRI_JU:	return OUTPUTFLG_VOICE_HIGH;		// 重大疵
	case em_VOICE_PRI_RE:	return OUTPUTFLG_VOICE_SEQ;			// 連続疵
	default:				return OUTPUTFLG_END;	 			// 無し
	}

}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// パラメータ読込

//------------------------------------------
// 全パラメータ読み込み
//------------------------------------------
bool ParamManager::ReadParam_All()
{
	if( ! Read_CommonMast() )	return false;
	if( ! Read_TypeName() )		return false;
	if( ! Read_GradeName() )	return false;
	if( ! Read_ScodeOutput() )	return false;
	if( ! Read_PcodeOutput() )	return false;
	if( ! Read_PallOutput() )	return false;
//	if( ! Read_PCommonOutput())	return false;	//5CGL極微小では使用しない

	if( ! Read_Param_Common() )	return false;
	if( ! Read_Param_CamSet() )	return false;
	if( ! Read_Param_Scode() )	return false;
	if( ! Read_Param_Camset_Scode() )		return false;
	if( ! Read_Param_Camset_Angle() )		return false;
	if( ! Read_Param_Camset_Angle_Scode() )	return false;
	return true;
}
//------------------------------------------
// φω定数パラメータ読込
//------------------------------------------
bool ParamManager::Read_CommonMast()
{
	CString sql;					// SQL文

	//// パラメータ初期化
	CString cDbName;
	cDbName = DB_COMMON_MASTER;
	P_COMMON_MAST&	p = m_CommonMast;
	P_COMMON_MAST	buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー[%s]", my_sThreadName, clsDB.GetLastErrMsg());
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL文作成
	sql  = "SELECT ";
	sql += " [RAS正常色] ";
	sql += ",[RAS異常色] ";
	sql += ",[RAS注意色] ";
	sql += ",[LOG表示件数] ";
	sql += ",[LOGイベント色] ";
	sql += ",[LOG重故障色] ";
	sql += ",[LOG軽故障色] ";

	sql += ",[探傷画面表示区間小] ";
	sql += ",[探傷画面表示区間大] ";
	sql += ",[ウォーマ材指定位置] ";
	sql += ",[Jpeg切り出し画像品質] ";
	sql += ",[カメラ温度閾値] ";

	sql += ",[エッジ整合性_実施種別] ";
	sql += ",[エッジ整合性_検出範囲] ";
	sql += ",[エッジ整合性_リカバリー検出範囲] ";
	sql += ",[エッジ整合性_板幅許容誤差] ";
	sql += ",[エッジ整合性_前フレームとの許容誤差] ";
	sql += ",[エッジ整合性_溶接後不感帯] ";
	sql += ",[エッジ整合性_エッジ検出異常FF送信] ";
	sql += ",[エッジ検出_実施種別] ";
	sql += ",[エッジ検出_検出範囲] ";

	sql += ",[重複疵マップ背景色_軽] ";
	sql += ",[重複疵マップ背景色_中] ";
	sql += ",[重複疵マップ背景色_重] ";
	sql += ",[重複疵マップ疵個数閾値_軽] ";
	sql += ",[重複疵マップ疵個数閾値_中] ";
	sql += ",[重複疵マップ疵個数閾値_重] ";

	sql += ",[設備長_検査基点_上流カメラ] ";
	sql += ",[設備長_検査基点_SPM] ";
	sql += ",[設備長_表示基点_裏検査台] ";
	sql += ",[設備長_表示基点_表検査台] ";
	sql += ",[設備長_表示基点_シャー] ";
	sql += ",[設備長_検査基点ゲート信号ON_検査基点] ";
	sql += ",[設備長_表示基点ゲート信号ON_表示基点] ";

	sql += ",[疵接近ランプ_不感帯_先端] ";
	sql += ",[疵接近ランプ_不感帯_尾端] ";
	sql += ",[疵接近ランプ_出力区間_前] ";
	sql += ",[疵接近ランプ_出力区間_後] ";
	sql += ",[疵接近アラーム_不感帯_先端] ";
	sql += ",[疵接近アラーム_不感帯_尾端] ";
	sql += ",[疵接近アラーム_出力区間_前] ";
	sql += ",[疵接近アラーム_出力区間_後] ";
	sql += ",[ボイス_出力位置_前] ";

	sql += ",[全長画像_保存_R] ";
	sql += ",[全長画像_保存_G] ";
	sql += ",[全長画像_保存_B] ";
	sql += ",[全長画像_保存_差] ";

	sql += ",[PLG距離比較異常上限値] ";
	sql += ",[PLG距離表示異常上限値] ";
	sql += ",[プロコンコイル長比較異常上限値] ";

	sql += ",[ロジックパララン機能01_入力カメラ01] ";
	sql += ",[ロジックパララン機能01_入力カメラ02] ";
	sql += ",[ロジックパララン機能02_入力カメラ01] ";
	sql += ",[ロジックパララン機能02_入力カメラ02] ";

	sql += "FROM ";
	sql += cDbName;

	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
		return false;
	}

	//// 取得
	int nRecord = 0;
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り
	
			// データ部
			clsDB.GetData(1, &buf.Ras.nColorNorm);				// RAS正常色
			clsDB.GetData(2, &buf.Ras.nColorErr);				// RAS異常色
			clsDB.GetData(3, &buf.Ras.nColorWarn);				// RAS注意色
			clsDB.GetData(4, &buf.Log.nDispNum);				// LOG表示件数
			clsDB.GetData(5, &buf.Log.nEventColor);				// LOGイベント色
			clsDB.GetData(6, &buf.Log.nFailColor_Ju);			// LOG重故障色
			clsDB.GetData(7, &buf.Log.nFailColor_Kei);			// LOG軽故障色

			clsDB.GetData(8, &buf.Tansho.nDspSectionMin);		// 探傷画面表示区間小
			clsDB.GetData(9, &buf.Tansho.nDspSectionMax);		// 探傷画面表示区間大
			clsDB.GetData(10, &buf.walm);						// ウォーマ材指定位置
			clsDB.GetData(11, &buf.nJpegQuality);				// Jpeg切り出し画像品質
			clsDB.GetData(12, &buf.nCamTempThr);				// カメラ温度閾値

			clsDB.GetData(13, &buf.Edge.nType);					// エッジ整合性_実施種別
			clsDB.GetData(14, &buf.Edge.dDetectRange);			// エッジ整合性_検出範囲
			clsDB.GetData(15, &buf.Edge.dDetectRangeR);			// エッジ整合性_リカバリー検出範囲
			clsDB.GetData(16, &buf.Edge.dWidthDiff);			// エッジ整合性_板幅許容誤差
			clsDB.GetData(17, &buf.Edge.dPerEdgeDiff);			// エッジ整合性_前フレームとの許容誤差
			clsDB.GetData(18, &buf.Edge.nFromSkip);				// エッジ整合性_溶接後不感帯
			clsDB.GetData(19, &buf.Edge.nFFErr);				// エッジ整合性_エッジ検出異常FF送信
			clsDB.GetData(20, &buf.Edge_Restrict.nType);		// エッジ検出_実施種別
			clsDB.GetData(21, &buf.Edge_Restrict.dDetectRange);	// エッジ検出_検出範囲
			
			clsDB.GetData(22, &buf.DefMap.nBGColor_Kei);		// 重複疵マップ背景色_軽
			clsDB.GetData(23, &buf.DefMap.nBGColor_Tyu);		// 重複疵マップ背景色_中
			clsDB.GetData(24, &buf.DefMap.nBGColor_Ju);			// 重複疵マップ背景色_重
			clsDB.GetData(25, &buf.DefMap.nDefCntThr_Kei);		// 重複疵マップ疵個数閾値_軽
			clsDB.GetData(26, &buf.DefMap.nDefCntThr_Tyu);		// 重複疵マップ疵個数閾値_中
			clsDB.GetData(27, &buf.DefMap.nDefCntThr_Ju);		// 重複疵マップ疵個数閾値_重

			clsDB.GetData(28, &buf.Dist.nKenBase);				// 設備長_検査基点_上流カメラ
			clsDB.GetData(29, &buf.Dist.nKenBaseSpm);			// 設備長_検査基点_SPM
			clsDB.GetData(30, &buf.Dist.nDspBaseKensaBot);		// 設備長_表示基点_裏検査台
			clsDB.GetData(31, &buf.Dist.nDspBaseKensaTop);		// 設備長_表示基点_表検査台
			clsDB.GetData(32, &buf.Dist.nDspSharCut);			// 設備長_表示基点_シャー
			clsDB.GetData(33, &buf.Dist.nKenBaseGate);			// 設備長_検査基点ゲート信号ON_検査基点
			clsDB.GetData(34, &buf.Dist.nDspBaseGate);			// 設備長_表示基点ゲート信号ON_表示基点

			clsDB.GetData(35, &buf.nNoLamp[0]);					// 疵接近ランプ_不感帯_先端
			buf.nNoLamp[0] *= 1000;
			clsDB.GetData(36, &buf.nNoLamp[1]);					// 疵接近ランプ_不感帯_尾端
			buf.nNoLamp[1] *= 1000;
			clsDB.GetData(37, &buf.nKizuLamp[0]);				// 疵接近ランプ_出力区間_前
			buf.nKizuLamp[0] *= 1000;
			clsDB.GetData(38, &buf.nKizuLamp[1]);				// 疵接近ランプ_出力区間_後
			buf.nKizuLamp[1] *= 1000;
			clsDB.GetData(39, &buf.nNoAlarm[0]);				// 疵接近アラーム不感帯_先端
			buf.nNoAlarm[0] *= 1000;
			clsDB.GetData(40, &buf.nNoAlarm[1]);				// 疵接近アラーム_不感帯_尾端
			buf.nNoAlarm[1] *= 1000;
			clsDB.GetData(41, &buf.nKizuAlarm[0]);				// 疵接近アラーム_出力区間_前
			buf.nKizuAlarm[0] *= 1000;
			clsDB.GetData(42, &buf.nKizuAlarm[1]);				// 疵接近アラーム_出力区間_後
			buf.nKizuAlarm[1] *= 1000;
			clsDB.GetData(43, &buf.nVoicePos);					// ボイス_出力位置_前
			buf.nVoicePos *= 1000;

			clsDB.GetData(44, &buf.nRPISaveColor_R);			// 全長画像_保存_R
			clsDB.GetData(45, &buf.nRPISaveColor_G);			// 全長画像_保存_G
			clsDB.GetData(46, &buf.nRPISaveColor_B);			// 全長画像_保存_B
			clsDB.GetData(47, &buf.nRPISaveColor_Sa);			// 全長画像_保存_差

			clsDB.GetData(48, &buf.dPlgDiffLimit[0]);			// PLG距離比較異常上限値
			clsDB.GetData(49, &buf.dPlgDiffLimit[1]);			// PLG距離表示異常上限値
			clsDB.GetData(50, &buf.dProLenDiffLimit);			// プロコンコイル長比較異常上限値

			clsDB.GetData(51, &buf.nLogiParaCam[0][0]);			// ロジックパララン機能01_入力カメラ01
			clsDB.GetData(52, &buf.nLogiParaCam[0][1]);			// ロジックパララン機能01_入力カメラ02
			clsDB.GetData(53, &buf.nLogiParaCam[1][0]);			// ロジックパララン機能02_入力カメラ01
			clsDB.GetData(54, &buf.nLogiParaCam[1][1]);			// ロジックパララン機能02_入力カメラ02

			nRecord++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
			return false;
		}
	}

	//// 取ってきたレコード数は正常？？
	if( 1 != nRecord ){
		LOG(em_ERR), "[%s] %s レコード数異常[%d]", my_sThreadName, cDbName, nRecord);
		return false;
	}

	//// 取得データをメンバー変数にセット
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, cDbName);
	return true;
}


//------------------------------------------
// 疵種パラメータ読込
//------------------------------------------
bool ParamManager::Read_TypeName()
{
	CString sql;					// SQL文
	int id;
	int imgidx;

	//// パラメータ初期化
	CString cDbName;
	cDbName = DB_TYPE_NAME;
	P_TYPE_NAME&	p = m_TypeName;
	P_TYPE_NAME		buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー[%s]", my_sThreadName, clsDB.GetLastErrMsg());
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL文作成
	sql  = "SELECT ";
	sql += "ID, EdasysID, 優先度 ";
	sql += ", ISNULL(切出画像種類1,1), ISNULL(切出画像種類2,2)";
	sql += "FROM ";
	sql += cDbName;

	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
		return false;
	}

	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り
	
			// 主キー
			clsDB.GetData(1, &id);
			if( MAX_TYPE < id || 0 > id ){
				LOG(em_ERR), "[%s] [%s 主キー不正 id=%d]", my_sThreadName, cDbName, id);
				syslog(SYSNO_DB_DATA_ERR, "[%s id=%d]", cDbName, id);
				continue;
			}

			// データ部
			clsDB.GetData(2, &buf.Type_Id[buf.num].nEdasysID );
			clsDB.GetData(3, &buf.Type_Id[buf.num].nPriority );

			clsDB.GetData(4, &imgidx);
			if (0<imgidx || imgidx <= MAX_IMAGE_CNT) buf.Type_Id[buf.num].bEnable[imgidx - 1] = true;

			clsDB.GetData(5, &imgidx);
			if (0<imgidx || imgidx <= MAX_IMAGE_CNT) buf.Type_Id[buf.num].bEnable[imgidx - 1] = true;

			buf.num++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
			return false;
		}
	}

	//// 取得データをメンバー変数にセット
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, cDbName);
	return true;
}
//------------------------------------------
// グレードパラメータ読込
//------------------------------------------
bool ParamManager::Read_GradeName()
{
	CString sql;					// SQL文
	int id;

	//// パラメータ初期化
	CString cDbName;
	cDbName = DB_GRADE_NAME;
	P_GRADE_NAME&	p = m_GradeName;
	P_GRADE_NAME	buf;
	memset(&buf, 0x00, sizeof(buf));

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL文作成
	sql =  "SELECT";
	sql += " ID, EdasysID, 優先度, 欠陥グレード";
	sql += " FROM ";
	sql += cDbName;
	sql += " ORDER BY 優先度 ASC ";

	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
		return false;
	}

	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り

			// 主キー
			clsDB.GetData(1, &id);
			if( MAX_GRADE <= id || 0 > id ) {
				// 主キー不正
				LOG(em_ERR), "[%s] [%s 主キー不正 id=%d]", my_sThreadName, cDbName, id);
				syslog(SYSNO_DB_DATA_ERR, "[%s id=%d]", cDbName, id);					
				continue;
			}
			
			// -----------------
			// データ部
			clsDB.GetData(2, &buf.Grade_Id[buf.num].nEdasysID );
			clsDB.GetData(3, &buf.Grade_Id[buf.num].nPriority );
			clsDB.GetData(4, &buf.Grade_Id[buf.num].rank );

			buf.num++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
			return false;
		}
	}

	//// 取得データをメンバー変数にセット
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, cDbName);
	return true;
}
//------------------------------------------
// 検査表面状態パラメータ読込
//------------------------------------------
bool ParamManager::Read_ScodeOutput()
{
	CString sql;					// SQL文
	int id;


	//// パラメータ初期化
	CString cDbName;
	P_SCODE_OUTPUT&	p = m_ScodeOutput;
	P_SCODE_OUTPUT buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	////////////////////////////////////
	// マスター
	////////////////////////////////////
	cDbName = DB_SCODE_MASTER;
	//// SQL文作成
	sql =  "SELECT";
	sql += " scode, 有効判定";
	sql += " FROM ";
	sql += cDbName;

	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
		return false;
	}

	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り

			// 主キー
			clsDB.GetData(1, &id);
			if( MAX_SCODE < id || 0 >= id ) {
				// 主キー不正
				LOG(em_ERR), "[%s] [%s 主キー不正 id=%d]", my_sThreadName, cDbName, id);
				syslog(SYSNO_DB_DATA_ERR, "[%s id=%d]", cDbName, id);					
				continue;
			}
			id = id -1;		// 使いやすいように0オリジンに変更


			// データ部
			clsDB.GetData(2, &buf.rec[id].umu );

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
			return false;
		}
	}

	//// 取得データをメンバー変数にセット
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, cDbName);

	return true;
}
//------------------------------------------
// 検査パターン別パラメータ読込
//------------------------------------------
bool ParamManager::Read_PcodeOutput()
{
	CString sql;					// SQL文
	int id;
	int no;

	//// パラメータ初期化
	CString cDbName;
	P_PCODE_OUTPUT&	p = m_PcodeOutput;
	//P_PCODE_OUTPUT		buf;
	// MAX_TYPE=64→256にしたため、P_PCODE_OUTPUTのサイズが大きくなってスタックオーバーフローが発生したためstatic化
	static P_PCODE_OUTPUT		buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	////////////////////////////////////
	// マスター
	////////////////////////////////////
	cDbName = DB_PCODE_MASTER;
	//// SQL文作成
	sql =  "SELECT";
	sql += " pcode, 有効判定";

	sql += " FROM ";
	sql += cDbName;

	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
		return false;
	}

	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り

			// 主キー
			clsDB.GetData(1, &id);
			if( MAX_PCODE < id || 0 >= id ) {
				// 主キー不正
				LOG(em_ERR), "[%s] [%s 主キー不正 id=%d]", my_sThreadName, cDbName, id);
				syslog(SYSNO_DB_DATA_ERR, "[%s id=%d]", cDbName, id);					
				continue;
			}
			id = id -1;		// 使いやすいように0オリジンに変更


			// -----------------
			// データ部
			clsDB.GetData(2, &buf.rec[id].umu );

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
			return false;
		}
	}
	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, cDbName);

	////////////////////////////////////
	// 出力条件
	////////////////////////////////////
	int typid;	
	no = 0;
	cDbName = DB_PCODE_OUTPUT;
	//// SQL文作成
	sql  = "SELECT ";
	sql += " [pcode] ";
	sql += ",[疵種EdasysID] ";
	sql += ",[探傷画面表示_表] ";
	sql += ",[探傷画面表示_裏] ";
	sql += ",[入側単位疵点マップ画面表示] ";
	sql += ",[FF送信有無] ";
	sql += ",[疵接近軽] ";
	sql += ",[疵接近重] ";
	sql += ",[パトライト出力] ";
	sql += ",[ボイス連続疵] ";
	sql += ",[ボイス重大疵] ";
	sql += ",[ボイスヘゲ] ";
	sql += ",[ボイススリバ] ";
	sql += ",[ボイス表裏同一疵] ";
	sql += ",[ボイス周期疵] ";
	sql += ",[ボイスその他疵] ";
	sql += ",[DB保存_一般_表] ";
	sql += ",[DB保存_一般_裏] ";
	sql += ",[DB保存_優先_表] ";
	sql += ",[DB保存_優先_裏] ";

	sql += " FROM ";
	sql += cDbName;

	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
		return false;
	}

	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り

			// 主キー
			clsDB.GetData(1, &id);
			if( MAX_PCODE < id || 0 >= id ) {
				// 主キー不正
				LOG(em_ERR), "[%s] [%s 主キー不正 id=%d]", my_sThreadName, cDbName, id);
				syslog(SYSNO_DB_DATA_ERR, "[%s id=%d]", cDbName, id);					
				continue;
			}
			id = id -1;		// 使いやすいように0オリジンに変更


			// -----------------
			clsDB.GetData(2, &typid);
			if(-1 == typid) {								// デフォルト
				buf.rec[id].defput.edasysID = typid;
				clsDB.GetData(3, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_DSP_O]);
				clsDB.GetData(4, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_DSP_U]);
				clsDB.GetData(5, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_DSP_DEFMAP]);
				clsDB.GetData(6, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_FF_SEND]);
				clsDB.GetData(7, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_ALARM_RANP_LOW]);
				clsDB.GetData(8, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_ALARM_RANP_HIGH]);
				clsDB.GetData(9, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_PATOLOGHT]);
				clsDB.GetData(10, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_VOICE_SEQ]);
				clsDB.GetData(11, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_VOICE_HIGH]);
				clsDB.GetData(12, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_VOICE_HEGE]);
				clsDB.GetData(13, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_VOICE_SURI]);
				clsDB.GetData(14, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_VOICE_SAME]);
				clsDB.GetData(15, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_VOICE_CYCLE]);
				clsDB.GetData(16, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_VOICE_OTHER]);
				clsDB.GetData(17, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_LOWGR_DB_O]);
				clsDB.GetData(18, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_LOWGR_DB_U]);
				clsDB.GetData(19, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_DB_O]);
				clsDB.GetData(20, (int*)&buf.rec[id].defput.kind[OUTPUTFLG_DB_U]);

			} else {										// 通常
				no = buf.rec[id].nPutNum;
				buf.rec[id].output[no].edasysID = typid;

				clsDB.GetData(3, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_DSP_O]);
				clsDB.GetData(4, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_DSP_U]);
				clsDB.GetData(5, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_DSP_DEFMAP]);
				clsDB.GetData(6, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_FF_SEND]);
				clsDB.GetData(7, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_ALARM_RANP_LOW]);
				clsDB.GetData(8, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_ALARM_RANP_HIGH]);
				clsDB.GetData(9, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_PATOLOGHT]);
				clsDB.GetData(10, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_VOICE_SEQ]);
				clsDB.GetData(11, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_VOICE_HIGH]);
				clsDB.GetData(12, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_VOICE_HEGE]);
				clsDB.GetData(13, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_VOICE_SURI]);
				clsDB.GetData(14, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_VOICE_SAME]);
				clsDB.GetData(15, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_VOICE_CYCLE]);
				clsDB.GetData(16, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_VOICE_OTHER]);
				clsDB.GetData(17, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_LOWGR_DB_O]);
				clsDB.GetData(18, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_LOWGR_DB_U]);
				clsDB.GetData(19, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_DB_O]);
				clsDB.GetData(20, (int*)&buf.rec[id].output[no].kind[OUTPUTFLG_DB_U]);
				
				buf.rec[id].nPutNum = no + 1;
			}

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
			return false;
		}
	}

	//// 取得データをメンバー変数にセット
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, cDbName);

	return true;
}
//------------------------------------------
// 検査パターン共通出力設定パラメータ読込
//------------------------------------------
bool ParamManager::Read_PallOutput()
{
//#ifdef DISABLE_GAP_CALC
//	return true;
//#endif

	CString sql;					// SQL文

	//// パラメータ初期化
	CString cDbName;
	cDbName = DB_PALL_OUTPUT;
	P_PALL_OUTPUT&	p = m_PallOutput;
	P_PALL_OUTPUT	buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}


	////////////////////////////////////
	// 出力条件
	////////////////////////////////////
	int no;
	int typid;	
	no = 0;

	//// SQL文作成
	sql =  "SELECT";
	sql += " 疵種EdasysID"; 
	sql += ", 重欠陥一覧画面表示_詳細条件, 重欠陥一覧画面表示_簡易条件, 重欠陥一覧画面表示_簡易項目 ";
	sql += " FROM ";
	sql += cDbName;

	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return false;
	}

	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り

			// 主キー
			clsDB.GetData(1, &typid);
			if(-1 == typid) {								// デフォルト
				buf.defput.edasysID = typid;

				clsDB.GetData(2, (int*)&buf.defput.kind[OUTPUTPALLFLG_DEFLIST_DETAIL]);
				clsDB.GetData(3, (int*)&buf.defput.kind[OUTPUTPALLFLG_DEFLIST_SIMPLE]);
				clsDB.GetData(4, (int*)&buf.defput.kind[OUTPUTPALLFLG_DEFLIST_SIMPLE_LIST]);

			} else {										// 通常
				no = buf.nPutNum;
				buf.output[no].edasysID = typid;

				clsDB.GetData(2, (int*)&buf.output[no].kind[OUTPUTPALLFLG_DEFLIST_DETAIL]);
				clsDB.GetData(3, (int*)&buf.output[no].kind[OUTPUTPALLFLG_DEFLIST_SIMPLE]);
				clsDB.GetData(4, (int*)&buf.output[no].kind[OUTPUTPALLFLG_DEFLIST_SIMPLE_LIST]);


				buf.nPutNum = no + 1;
			}

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return false;
		}
	}

	//// 取得データをメンバー変数にセット
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, cDbName);

	return true;
}
//5CGL極微小では使用しない
//------------------------------------------
// 検査パターン固有パラメータ読込
//------------------------------------------
//bool ParamManager::Read_PCommonOutput()
//{
//	CString sql;					// SQL文
//	int id;
//
//
//	//// パラメータ初期化
//	CString cDbName;
//	cDbName = DB_PCOMMON_OUTPUT;
//	P_PCOMMON_OUTPUT&	p = m_PcommonOutput;
//	P_PCOMMON_OUTPUT	buf;
//	memset(&buf, 0x00, sizeof(buf));
//
//
//	//// ODBC接続クラス インスタンス生成
//	OdbcBase clsDB;
//	if( ! clsDB.Connect(TASKINI_NAME)) {
//		// データベース接続エラー
//		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
//		syslog(SYSNO_DB_CONNECT_ERR, "");
//		return false;
//	}
//
//	////////////////////////////////////
//	// マスター
//	////////////////////////////////////
//	cDbName = DB_PCOMMON_OUTPUT;
//	//// SQL文作成
//	sql =  "SELECT";
//	sql += " pcode ";
//	sql += ", ストップマークFF送信有無 ";
//	sql += " FROM ";
//	sql += cDbName;
//	sql += " ORDER BY pcode ASC";
//
//	//// 実行
//	if( !clsDB.ExecuteDirect(sql)) {
//		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
//		return false;
//	}
//
//	//// 取得
//	while (TRUE) {
//		int sqlrc = clsDB.Fetch();
//		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り
//
//			// 主キー
//			clsDB.GetData(1, &id);
//			if( MAX_PCODE < id || 0 >= id ) {
//				// 主キー不正
//				LOG(em_ERR), "[%s] [%s 主キー不正 id=%d]", my_sThreadName, cDbName, id);
//				syslog(SYSNO_DB_DATA_ERR, "[%s id=%d]", cDbName, id);					
//				continue;
//			}
//			id = id -1;		// 使いやすいように0オリジンに変更
//
//
//
//			//-----------------
//			//データ部
//			clsDB.GetData(2, &buf.rec[id].nStopMarkFFSend );
//
//		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
//			break;	
//		} else {								// エラー
//			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
//			return false;
//		}
//	}
//
//	//// 取得データをメンバー変数にセット
//	memcpy( &m_PcommonOutput, &buf, sizeof(buf));
//	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, cDbName);
//	return true;
//}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EPCパラメータ読込
//------------------------------------------
// 共通パラメータ 読込
//------------------------------------------
bool ParamManager::Read_Param_Common()
{
	CString sql;					// SQL文
	double  wk;

	//// パラメータ初期化
	CString cDbName;
	cDbName = DB_PARAM_COMMON;
	P_PARAM_COMMON&	p = m_Param_Common;
	P_PARAM_COMMON	buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL文作成
	sql =  "SELECT ";
	sql += " [PLGパルスレート] ";				// パルスレートは一応DBから取得する（あとでDMから取得し、DB上書き）
	sql += ",[PLGパルスレート比較] ";
	sql += ",[PLGパルスレート表示] ";
	sql += ",0 ";
	sql += ",0 ";
	sql += ",0 ";
	//sql += ",[カウンタボードPLG逓倍率] ";
	//sql += ",[カウンタボードPLG信号特性] ";
	//sql += ",[カウンタボードPLG位相] ";
	//sql += ",[カウンタボードPLG位相差] ";
	//sql += ",[カウンタボードPLGデジタルフィルタ] ";
	sql += ",[表裏間基準面] ";
	sql += ",[表裏間距離] ";
	sql += ",[ExSyncIntval] ";
	sql += ",[ExSyncMinH] ";
	sql += ",[ExSyncMinL] ";
	//sql += ",[AGCbyCamcont] ";
	sql += ",0 ";		// AGCbyCamcontがない？
	sql += ",[外接長方形の重なり] ";
#ifndef DISABLE_GAP_CALC
	sql += ", UseWarpProj";
#else
	sql += ",0 ";		// 連番が変わりたくないから
#endif

	// 固定分解能化
	sql += ",[カメラPLG逓倍率] ";
	sql += ",[カメラPLG分周率] ";
	sql += ",[カメラPLG逓倍率_O] ";
	sql += ",[カメラPLG分周率_O] ";
	sql += ",[カメラPLG逓倍率_U] ";
	sql += ",[カメラPLG分周率_U] ";
	sql += ",[カメラPLG逓倍率_T] ";
	sql += ",[カメラPLG分周率_T] ";
	
	// エッジ検出制限
	sql += ",[EdgeRestrictPort] ";
	
	// フレーム画像間引き
	sql += ",[ShFrameShrinkX] ";
	sql += ",[ShFrameShrinkY] ";
	
	sql += " FROM ";
	sql += cDbName;
		
	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
		return false;
	}

	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り

			// データ部
			clsDB.GetData(1,  &buf.dPls );
			clsDB.GetData(2,  &buf.dPlsSub);
			clsDB.GetData(3,  &buf.dPlsDsp);
			//clsDB.GetData(2,  &buf.typCntSet[0].nPlgMul );
			//clsDB.GetData(3,  &buf.typCntSet[0].nPlgPulse );
			//clsDB.GetData(4,  &buf.typCntSet[0].nPlgPhase );
			//clsDB.GetData(5,  &buf.typCntSet[0].nPlgPhaseDiff );
			//clsDB.GetData(6,  &buf.typCntSet[0].nPlgFilter );

#ifndef PLG_DSP_KEN_EQAL
			// 表示用PLGは今回無し
#else
			// 検査用PLG と 表示用PLGが同じ
#endif
			clsDB.GetData(7,  &buf.nDistPosition );
			clsDB.GetData(8,  &buf.dDistLen );

			clsDB.GetData(9,  &buf.nExSyncIntval );
			clsDB.GetData(10, &buf.nExSyncMinH );
			clsDB.GetData(11, &buf.nExSyncMinL );
			//clsDB.GetData(12, &buf.nAGCbyCamcont );
			clsDB.GetData(13, &wk );				buf.dLapRate = wk / 100.0f;

#ifndef DISABLE_GAP_CALC
			clsDB.GetData(14, &buf.nUseWarpProj );		
#endif

			clsDB.GetData(15, &buf.typCamCon[0].nMul );
			clsDB.GetData(16, &buf.typCamCon[0].nDiv );
			clsDB.GetData(17, &buf.typCamCon[1].nMul );
			clsDB.GetData(18, &buf.typCamCon[1].nDiv );
			clsDB.GetData(19, &buf.typCamCon[2].nMul );
			clsDB.GetData(20, &buf.typCamCon[2].nDiv );
			clsDB.GetData(21, &buf.typCamCon[3].nMul );
			clsDB.GetData(22, &buf.typCamCon[3].nDiv );

			clsDB.GetData(23, &buf.nEdgeRestrictPort);

			clsDB.GetData(24, &buf.nShFrameShrinkX);
			clsDB.GetData(25, &buf.nShFrameShrinkY);
		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
			return false;
		}
	}

	// 有効桁少数点４位として５位を四捨五入
	buf.dPls	= ((buf.dPls * 10000.0) + 0.5) / 10000.0;
	buf.dPlsSub = ((buf.dPlsSub * 10000.0) + 0.5) / 10000.0;
	buf.dPlsDsp = ((buf.dPlsDsp * 10000.0) + 0.5) / 10000.0;

	//// 取得データをメンバー変数にセット
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, cDbName);
	return true;
}

//------------------------------------------
// カメラセット別 パラメータ 読込
//------------------------------------------
bool ParamManager::Read_Param_CamSet()
{
	CString sql;					// SQL文
	int camset;

	//// パラメータ初期化
	CString cDbName;
	cDbName = DB_PARAM_CAMSET;
	P_PARAM_CAMSET&	p = m_Param_CamSet;
	P_PARAM_CAMSET	buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL文作成
	sql =  "SELECT ";
	sql += " [camset] ";
	sql += ",[Hres] ";
	sql += ",[Xoffset] ";

#ifndef DISABLE_GAP_CALC		// 一番最後
	// 複眼ズレ
	sql += ", ";
	if(true){
		CString		cWk;
		for(int ii=1; ii<=MAX_CAMERA_GAP_WIDDIV; ii++){
			cWk.Format("[複眼ズレ補正_幅方向分割%d], ", ii);
			sql += cWk;
		}
		for(int ii=1; ii<=MAX_CAMERA_GAP_WIDDIV; ii++){
			cWk.Format("[複眼ズレ補正_集計実施件数%d], ", ii);
			sql += cWk;
		}		
	}
#endif

	sql += " FROM ";
	sql += cDbName;
		
	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
		return false;
	}

	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り
		
			// 主キー
			clsDB.GetData(1, &camset);
			if( MAX_CAMSET < camset || 0 >= camset ) {
				// 主キー不正
				LOG(em_ERR), "[%s] [%s 主キー不正 camset=%d]", my_sThreadName, cDbName, camset);
				syslog(SYSNO_DB_DATA_ERR, "[%s camset=%d]", cDbName, camset);					
				continue;
			}
			camset = camset -1;		// 使いやすいように0オリジンに変更

			// データ部
			clsDB.GetData(2, &buf.camset[camset].dHres );
			clsDB.GetData(3, &buf.camset[camset].dXoffset );

#ifndef DISABLE_GAP_CALC
			for(int ii=0; ii<MAX_CAMERA_GAP_WIDDIV; ii++) clsDB.GetData(4+ii,  &buf.camset[camset].nGapGroup[ii] );
			for(int ii=0; ii<MAX_CAMERA_GAP_WIDDIV; ii++) clsDB.GetData(4+MAX_CAMERA_GAP_WIDDIV+ii, &buf.camset[camset].nGapCount[ii] );
#endif

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
			return false;
		}
	}

	//// 取得データをメンバー変数にセット
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, cDbName);
	return true;
}


//------------------------------------------
// 表面状態別 パラメータ 読込
//------------------------------------------
bool ParamManager::Read_Param_Scode()
{
	CString sql;					// SQL文
	int scode;

	//// パラメータ初期化
	CString cDbName;
	cDbName = DB_PARAM_SCODE;
	P_PARAM_SCODE&	p = m_Param_Scode;
	P_PARAM_SCODE	buf;
	memset(&buf, 0x00, sizeof(buf));

	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL文作成
	sql =  "SELECT ";
	sql += " [scode] ";

#ifndef DISABLE_TINY_CALC
	// 微小欠陥
	sql += ",[微小欠陥重み_正白_乱白] ";
	sql += ",[微小欠陥重み_正白_乱黒] ";
	sql += ",[微小欠陥重み_正黒_乱白] ";
	sql += ",[微小欠陥重み_正黒_乱黒] ";
	sql += ",[微小欠陥重み_正白のみ] ";
	sql += ",[微小欠陥重み_正黒のみ] ";
	sql += ",[微小欠陥重み_乱白のみ] ";
	sql += ",[微小欠陥重み_乱黒のみ] ";

	sql += ",[白微小欠陥閾値レベル1_縦横16] ";
	sql += ",[白微小欠陥閾値レベル2_縦横16] ";
	sql += ",[白微小欠陥閾値レベル3_縦横16] ";
	sql += ",[白微小欠陥閾値レベル1_横4] ";
	sql += ",[白微小欠陥閾値レベル2_横4] ";
	sql += ",[白微小欠陥閾値レベル3_横4] ";
	sql += ",[白微小欠陥閾値レベル1_縦4] ";
	sql += ",[白微小欠陥閾値レベル2_縦4] ";
	sql += ",[白微小欠陥閾値レベル3_縦4] ";
	sql += ",[白微小欠陥閾値レベル1_1] ";
	sql += ",[白微小欠陥閾値レベル2_1] ";
	sql += ",[白微小欠陥閾値レベル3_1] ";

	sql += ",[黒微小欠陥閾値レベル1_縦横16] ";
	sql += ",[黒微小欠陥閾値レベル2_縦横16] ";
	sql += ",[黒微小欠陥閾値レベル3_縦横16] ";
	sql += ",[黒微小欠陥閾値レベル1_横4] ";
	sql += ",[黒微小欠陥閾値レベル2_横4] ";
	sql += ",[黒微小欠陥閾値レベル3_横4] ";
	sql += ",[黒微小欠陥閾値レベル1_縦4] ";
	sql += ",[黒微小欠陥閾値レベル2_縦4] ";
	sql += ",[黒微小欠陥閾値レベル3_縦4] ";
	sql += ",[黒微小欠陥閾値レベル1_1] ";
	sql += ",[黒微小欠陥閾値レベル2_1] ";
	sql += ",[黒微小欠陥閾値レベル3_1] ";
#endif

	sql += " FROM ";
	sql += cDbName;
		
	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
		return false;
	}

	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り
		
			// 主キー
			clsDB.GetData(1, &scode);
			if( MAX_SCODE < scode || 0 >= scode ) {
				// 主キー不正
				LOG(em_ERR), "[%s] [%s 主キー不正 scode=%d]", my_sThreadName, cDbName, scode);
				syslog(SYSNO_DB_DATA_ERR, "[%s scode=%d]", cDbName, scode);					
				continue;
			}
			scode = scode -1;		// 使いやすいように0オリジンに変更

			// データ部

#ifndef DISABLE_TINY_CALC
			clsDB.GetData(5,  &buf.scode[scode].dTinyWeight.white_b_white_d );
			clsDB.GetData(6,  &buf.scode[scode].dTinyWeight.white_b_black_d );
			clsDB.GetData(7,  &buf.scode[scode].dTinyWeight.black_b_white_d );
			clsDB.GetData(8,  &buf.scode[scode].dTinyWeight.black_b_black_d );
			clsDB.GetData(9,  &buf.scode[scode].dTinyWeight.white_b );
			clsDB.GetData(10,  &buf.scode[scode].dTinyWeight.white_d );
			clsDB.GetData(11, &buf.scode[scode].dTinyWeight.black_b );
			clsDB.GetData(12, &buf.scode[scode].dTinyWeight.black_d );

			clsDB.GetData(13, &buf.scode[scode].dTinyLimit[0][0][0] );
			clsDB.GetData(14, &buf.scode[scode].dTinyLimit[0][0][1] );
			clsDB.GetData(15, &buf.scode[scode].dTinyLimit[0][0][2] );
			clsDB.GetData(16, &buf.scode[scode].dTinyLimit[0][1][0] );
			clsDB.GetData(17, &buf.scode[scode].dTinyLimit[0][1][1] );
			clsDB.GetData(18, &buf.scode[scode].dTinyLimit[0][1][2] );
			clsDB.GetData(19, &buf.scode[scode].dTinyLimit[0][2][0] );
			clsDB.GetData(20, &buf.scode[scode].dTinyLimit[0][2][1] );
			clsDB.GetData(21, &buf.scode[scode].dTinyLimit[0][2][2] );
			clsDB.GetData(22, &buf.scode[scode].dTinyLimit[0][3][0] );
			clsDB.GetData(23, &buf.scode[scode].dTinyLimit[0][3][1] );
			clsDB.GetData(24, &buf.scode[scode].dTinyLimit[0][3][2] );

			clsDB.GetData(25, &buf.scode[scode].dTinyLimit[1][0][0] );
			clsDB.GetData(26, &buf.scode[scode].dTinyLimit[1][0][1] );
			clsDB.GetData(27, &buf.scode[scode].dTinyLimit[1][0][2] );
			clsDB.GetData(28, &buf.scode[scode].dTinyLimit[1][1][0] );
			clsDB.GetData(29, &buf.scode[scode].dTinyLimit[1][1][1] );
			clsDB.GetData(30, &buf.scode[scode].dTinyLimit[1][1][2] );
			clsDB.GetData(31, &buf.scode[scode].dTinyLimit[1][2][0] );
			clsDB.GetData(32, &buf.scode[scode].dTinyLimit[1][2][1] );
			clsDB.GetData(33, &buf.scode[scode].dTinyLimit[1][2][2] );
			clsDB.GetData(34, &buf.scode[scode].dTinyLimit[1][3][0] );
			clsDB.GetData(35, &buf.scode[scode].dTinyLimit[1][3][1] );
			clsDB.GetData(36, &buf.scode[scode].dTinyLimit[1][3][2] );
#endif

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
			return false;
		}
	}

	//// 取得データをメンバー変数にセット
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, cDbName);
	return true;
}

//------------------------------------------
// カメラセット・検査表面状態別 パラメータ 読込
//------------------------------------------
bool ParamManager::Read_Param_Camset_Scode()
{
	CString sql;					// SQL文
	int scode;
	int camset;

	//// パラメータ初期化
	CString cDbName;
	cDbName = DB_PARAM_CAMSET_SCODE;
	P_PARAM_CAMSET_SCODE&	p = m_Param_CamSetScode;
	P_PARAM_CAMSET_SCODE	buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL文作成
	sql =  "SELECT ";
	sql += " [scode] ";
	sql += ",[camset] ";
	sql += ",[EdgeDetectCam] ";

	sql += " FROM ";
	sql += cDbName;
		
	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
		return false;
	}

	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り
		
			// 主キー
			clsDB.GetData(1, &scode);
			clsDB.GetData(2, &camset);
			if( scode > MAX_SCODE || scode <= 0 || camset > MAX_CAMSET || camset <= 0 ) {
				// 主キー不正
				LOG(em_ERR), "[%s] [%s 主キー不正 scode=%d camset=%d]", my_sThreadName, cDbName, scode, camset);
				syslog(SYSNO_DB_DATA_ERR, "[%s scode=%d camset=%d]", cDbName, scode, camset);					
				continue;
			}
			scode  = scode -1;		// 使いやすいように0オリジンに変更
			camset = camset -1;

			clsDB.GetData(3, &buf.scode[scode].camset[camset].nEdgeDetectCam );

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
			return false;
		}
	}

	//// 取得データをメンバー変数にセット
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, cDbName);
	return true;
}


//------------------------------------------
// カメラセット・カメラ角度別 パラメータ 読込
//------------------------------------------
bool ParamManager::Read_Param_Camset_Angle()
{
	CString sql;					// SQL文
	int camset;
	int camangle;
//	int	wk;

	//// パラメータ初期化
	CString cDbName;
	cDbName = DB_PARAM_CAMSET_ANGLE;
	P_PARAM_CAMSET_ANGLE&	p = m_Param_CamsetAngle;
	P_PARAM_CAMSET_ANGLE	buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL文作成
	sql =  "SELECT";
	sql += " [camangle] ";
	sql += ",[camset] ";

#ifndef DISABLE_GAP_CALC
	// 射影変換
	sql += ",[Xdelay] ";
	sql += ",[Xinsert] ";
	sql += ",[YdelayLeft] ";
	sql += ",[YdelayRight] ";
	sql += ",[Proj_a0] ";
	sql += ",[Proj_a1] ";
	sql += ",[Proj_a2] ";
	sql += ",[Proj_c1] ";
	sql += ",[Proj_c2] ";
#endif

	sql += " FROM ";
	sql += cDbName;
		
	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
		return false;
	}

	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り
		
			// 主キー
			clsDB.GetData(1, &camangle);
			clsDB.GetData(2, &camset);
			if( camset > MAX_CAMSET || camset <= 0  || camangle > MAX_CAMANGLE || camangle <= 0 ) {
				// 主キー不正
				LOG(em_ERR), "[%s] [%s 主キー不正 camset=%d camangle=%d]", my_sThreadName, cDbName, camset, camangle);
				syslog(SYSNO_DB_DATA_ERR, "[%s camset=%d camangle=%d]", cDbName, camset, camangle);					
				continue;
			}
			camset = camset -1;		// 使いやすいように0オリジンに変更
			camangle = camangle -1;

#ifndef DISABLE_GAP_CALC
			// 射影変換
			clsDB.GetData(3, &buf.camset[camset].camangle[camangle].nXdelay );
			clsDB.GetData(4, &buf.camset[camset].camangle[camangle].nXinsert );
			clsDB.GetData(5, &buf.camset[camset].camangle[camangle].nYdelayLeft );
			clsDB.GetData(6, &buf.camset[camset].camangle[camangle].nYdelayRight );

			clsDB.GetData(7, &buf.camset[camset].camangle[camangle].dProjA0 );
			clsDB.GetData(8, &buf.camset[camset].camangle[camangle].dProjA1 );
			clsDB.GetData(9, &buf.camset[camset].camangle[camangle].dProjA2 );
			clsDB.GetData(10, &buf.camset[camset].camangle[camangle].dProjC1 );
			clsDB.GetData(11, &buf.camset[camset].camangle[camangle].dProjC2 );
#endif

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
			return false;
		}
	}

	//// 取得データをメンバー変数にセット
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, cDbName);
	return true;
}

//------------------------------------------
// 表面状態・カメラセット・カメラ角度別 パラメータ 読込
//------------------------------------------
bool ParamManager::Read_Param_Camset_Angle_Scode()
{
	CString sql;					// SQL文
	int scode;
	int camset;
	int camangle;
//	int	wk;

	//// パラメータ初期化
	CString cDbName;
	cDbName = DB_PARAM_CAMSET_ANGLE_SCODE;
	P_PARAM_CAMSET_ANGLE_SCODE&	p = m_Param_CamsetAngleScode;
	P_PARAM_CAMSET_ANGLE_SCODE	buf;
	memset(&buf, 0x00, sizeof(buf));


	//// ODBC接続クラス インスタンス生成
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// データベース接続エラー
		LOG(em_ERR), "[%s] DB接続エラー", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	//// SQL文作成
	sql =  "SELECT";
	sql += " [scode] ";
	sql += ",[camangle] ";
	sql += ",[camset] ";
	sql += ",[CameraGain] ";
	sql += " FROM ";
	sql += cDbName;
		
	//// 実行
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
		return false;
	}

	//// 取得
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// データ有り
		
			// 主キー
			clsDB.GetData(1, &scode);
			clsDB.GetData(2, &camangle);
			clsDB.GetData(3, &camset);
			if( scode > MAX_SCODE || scode <= 0 || camset > MAX_CAMSET || camset <= 0  || camangle > MAX_CAMANGLE || camangle <= 0 ) {
				// 主キー不正
				LOG(em_ERR), "[%s] [%s 主キー不正 scode=%d camset=%d camangle=%d]", my_sThreadName, cDbName, scode, camset, camangle);
				syslog(SYSNO_DB_DATA_ERR, "[%s scode=%d camset=%d camangle=%d]", cDbName, scode, camset, camangle);					
				continue;
			}
			scode = scode -1;		// 使いやすいように0オリジンに変更
			camset = camset -1;
			camangle = camangle -1;


			// データ部
			clsDB.GetData(4, &buf.scode[scode].camset[camset].camangle[camangle].dCameraGain);


		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// データ無し
			break;	
		} else {								// エラー
			syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", cDbName, clsDB.GetLastErrMsg());
			return false;
		}
	}

	//// 取得データをメンバー変数にセット
	memcpy( &p, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s 取得完了", my_sThreadName, cDbName);
	return true;
}
//------------------------------------------
// グループ毎の必要判定PC数を取得
// int groupmax 取得するGroupNoの上限
// 戻り値 復帰情報
//------------------------------------------
bool ParamManager::Read_HtGroupNum(int groupmax)
{
	char sqlSelect[256];
	char sqlSelectNum[256];
	int groupno = 1;

	memset(m_nHtGroupNum, 0x00, sizeof(m_nHtGroupNum));
	memset(m_nHtGroupMax, 0x00, sizeof(m_nHtGroupMax));
	
	// DB接続チェック
	OdbcBase cls_Db_Select;
	if (!cls_Db_Select.Connect(TASKINI_NAME))
	{
		return false;
	}
	// DB接続チェック
	OdbcBase cls_Db_SelectNum;
	if (!cls_Db_SelectNum.Connect(TASKINI_NAME))
	{
		return false;
	}

	memset(sqlSelect, 0x00, sizeof(sqlSelect));
	sprintf(sqlSelect, "SELECT distinct m.GroupNo, (SELECT COUNT(*) FROM M_PC_INF WHERE GroupNo = m.GroupNo) as MAX FROM %s m WHERE m.GroupNo > 0 ORDER BY m.GroupNo ASC", DB_PC_INF);

	//// 実行
	if (!cls_Db_Select.ExecuteDirect(sqlSelect))
	{
		return false;
	}

	//// 取得
	while (TRUE)
	{
		int sqlrc = cls_Db_Select.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK)
		{
			// データ有り

			// グループID取得
			cls_Db_Select.GetData(1, &groupno);
			if (0 >= groupno) {
				LOG(em_ERR), "[%s] %s GroupNo=%d 異常", my_sThreadName, DB_PC_INF, groupno);
				return false;
			}
			// グループ内PC総数取得
			m_nHtGroupMax[groupno - 1] = 0;
			cls_Db_Select.GetData(2, &m_nHtGroupMax[groupno - 1]);

			m_nHtGroupNum[groupno - 1] = 0;
			memset(sqlSelectNum, 0x00, sizeof(sqlSelectNum));
			sprintf(sqlSelectNum, "SELECT COUNT(*) FROM %s WHERE GroupNo = %d AND PC区分 = 0", DB_PC_INF, groupno);
			if (!cls_Db_SelectNum.ExecuteDirect(sqlSelectNum))
			{
				return false;
			}
			else
			{
				sqlrc = cls_Db_SelectNum.Fetch();
				if (sqlrc == KIZUODBC_FETCH_OK)
				{
					// データ有り

					// グループ内必要PC数取得
					cls_Db_SelectNum.GetData(1, &m_nHtGroupNum[groupno - 1]);
				}
				else if (sqlrc == KIZUODBC_FETCH_NODATE)
				{
					// データ無し
				}
				else
				{
					// エラー
					return false;
				}
			}
		}
		else if (sqlrc == KIZUODBC_FETCH_NODATE)
		{
			// データ無し
			break;
		}
		else
		{
			// エラー
			return false;
		}

		groupno++;
		if (groupmax < groupno)
		{
			break;
		}
	}

	return true;
}