#include "StdAfx.h"
#include "DivNameManager.h"

using namespace KizuLib;

//------------------------------------------
// 拡張BOOL
//------------------------------------------
CString DivNameManager::GetDivBool(EM_DIV_BOOL em) 
{
	CString wk;
	if		(DIV_BOOL_TRUE == em)				{ wk = "○"; }
	else if (DIV_BOOL_FALSE == em)				{ wk = "×";}
	else if (DIV_BOOL_NON == em)				{ wk = "－";}
	else if (DIV_BOOL_NULL == em)				{ wk = "＊";}
	else										{ wk.Format("ステータス異常 <%d>", em);}
	return wk;
}

//------------------------------------------
// PLG区分 名称
//------------------------------------------
CString DivNameManager::GetDivPlg(EM_DIV_PLG em) 
{
	CString wk;
	if		(DIV_PLG_LINE == em)				{ wk = "ラインPLG"; }
	else if (DIV_PLG_TOP == em)					{ wk = "表校正"; }
	else if (DIV_PLG_BOT == em)					{ wk = "裏校正";}
	else if (DIV_PLG_TEST == em)				{ wk = "テストパルス";}
	else										{ wk.Format("ステータス異常 <%d>", em);}
	return wk;
}
//------------------------------------------
// 運転状態 名称
//------------------------------------------
CString DivNameManager::GetDivUnten(EM_DIV_UNTEN em) 
{
	CString wk;
	if		(DIV_UNTEN_STOP == em)				{ wk = "停止モード"; }
	else if (DIV_UNTEN_NOMAL == em)				{ wk = "通常運転モード"; }
	else if (DIV_UNTEN_SAMP == em)				{ wk = "テスト運転 (サンプル撮影)";}
	else if (DIV_UNTEN_CAMERA == em)			{ wk = "校正モード (カメラ調整)";}
	else if (DIV_UNTEN_SMYU == em)				{ wk = "シミュレーションモード";}
	else										{ wk.Format("ステータス異常 <%d>", em);}
	return wk;
}
//------------------------------------------
// 操業状態 名称
//------------------------------------------
CString DivNameManager::GetDivSogyo(EM_DIV_SOGYO em) 
{
	CString wk;
	if		(DIV_SOGYO_NON == em)				{ wk = "不明"; }
	else if (DIV_SOGYO_STOP == em)				{ wk = "休止中";}
	else if (DIV_SOGYO_SLOW == em)				{ wk = "低速操業";}
	else if (DIV_SOGYO_RUN == em)				{ wk = "操業中"; }
	else										{ wk.Format("ステータス異常 <%d>", em);}
	return wk;
}
//------------------------------------------
// 稼動状態 名称
//------------------------------------------
CString DivNameManager::GetDivKadou(EM_DIV_KADOU em) 
{
	CString wk;
	if		(DIV_KADOU_INIT == em)				{ wk = "初期化中"; }
	else if (DIV_KADOU_STOP == em)				{ wk = "停止中"; }
	else if (DIV_KADOU_START == em)				{ wk = "稼動中";}
	else if (DIV_KADOU_RETRY == em)				{ wk = "リトライ中";}
	else										{ wk.Format("ステータス異常 <%d>", em);}
	return wk;
}
//------------------------------------------
// 検査状態 名称
//------------------------------------------
CString DivNameManager::GetDivKensa(EM_DIV_KENSA em) 
{
	CString wk;

	if		(DIV_KENSA_OK   == em)				{ wk = "正常"; }
	else if (DIV_KENSA_NG   == em)				{ wk = "縮退";}
	else if (DIV_KENSA_MENTE== em)				{ wk = "メンテ中"; }

// 拡張部 なので、他システムで定数未定義も有り得るので、定数は使わない---->>
	else if	(4 == em)							{ wk = "ｴｯｼﾞ不正"; }
	else if	(5 == em)							{ wk = "未保存"; }
	else if	(6 == em)							{ wk = "未検査"; }

	else if	(-1 == em)							{ wk = "停止 (初期)"; }
//<<<----

	else if	(DIV_KENSA_STOP == em)				{ wk = "停止"; }
	else if	(DIV_KENSA_NON  == em)				{ wk = "停止 (対象外)"; }
	else										{ wk.Format("ステータス異常 <%d>", em);}
	return wk;
}
//------------------------------------------
// 機器状態 名称
//------------------------------------------
CString DivNameManager::GetDivKiki(EM_DIV_KIKI em) 
{
	CString wk;
	if		(DIV_KIKI_OK == em)					{ wk = "正常"; }
	else if (DIV_KIKI_KEI == em)				{ wk = "軽故障"; }
	//else if (DIV_KIKI_TYU == em)				{ wk = "中故障";}
	else if (DIV_KIKI_JYU == em)				{ wk = "重故障";}
	else										{ wk.Format("ステータス異常 <%d>", em);}
	return wk;
}

//------------------------------------------
// カメラセット名称
// int id カメラセット (1～8) 0:バックアップ
//------------------------------------------
CString DivNameManager::GetDivCamSet(EM_DIV_CAMSET id) 
{
	CString wk;
	if		(DIV_CAMSET_BACK == id)				{ wk = "バックアップ"; }

#ifdef NUM_CAM_POS_1
	else if (DIV_CAMSET_T_C  == id)				{ wk = "表面 C";}
	else if (DIV_CAMSET_B_C  == id)				{ wk = "裏面 C";}
#endif
#ifdef NUM_CAM_POS_2
	else if (DIV_CAMSET_T_DS == id)				{ wk = "表面 DS";}
	else if (DIV_CAMSET_T_WS == id)				{ wk = "表面 WS";}
	else if (DIV_CAMSET_B_DS == id)				{ wk = "裏面 DS";}
	else if (DIV_CAMSET_B_WS == id)				{ wk = "裏面 WS";}
#endif
#ifdef NUM_CAM_POS_3
	else if (DIV_CAMSET_T_DS == id)				{ wk = "表面 DS";}
	else if (DIV_CAMSET_T_C  == id)				{ wk = "表面 C";}
	else if (DIV_CAMSET_T_WS == id)				{ wk = "表面 WS";}
	else if (DIV_CAMSET_B_DS == id)				{ wk = "裏面 DS";}
	else if (DIV_CAMSET_B_C  == id)				{ wk = "裏面 C";}
	else if (DIV_CAMSET_B_WS == id)				{ wk = "裏面 WS";}
#endif
#ifdef NUM_CAM_POS_4
	else if (DIV_CAMSET_T_DS == id)				{ wk = "表面 DS";}
	else if (DIV_CAMSET_T_D1 == id)				{ wk = "表面 D1";}
	else if (DIV_CAMSET_T_W1 == id)				{ wk = "表面 W1";}
	else if (DIV_CAMSET_T_WS == id)				{ wk = "表面 WS";}
	else if (DIV_CAMSET_B_DS == id)				{ wk = "裏面 DS";}
	else if (DIV_CAMSET_B_D1 == id)				{ wk = "裏面 D1";}
	else if (DIV_CAMSET_B_W1 == id)				{ wk = "裏面 W1";}
	else if (DIV_CAMSET_B_WS == id)				{ wk = "裏面 WS";}
#endif
#ifdef NUM_CAM_POS_5
	else if (DIV_CAMSET_T_DS == id)				{ wk = "表面 DS";}
	else if (DIV_CAMSET_T_D1 == id)				{ wk = "表面 D1";}
	else if (DIV_CAMSET_T_C  == id)				{ wk = "表面 C";}
	else if (DIV_CAMSET_T_W1 == id)				{ wk = "表面 W1";}
	else if (DIV_CAMSET_T_WS == id)				{ wk = "表面 WS";}
	else if (DIV_CAMSET_B_DS == id)				{ wk = "裏面 DS";}
	else if (DIV_CAMSET_B_D1 == id)				{ wk = "裏面 D1";}
	else if (DIV_CAMSET_B_C  == id)				{ wk = "裏面 C";}
	else if (DIV_CAMSET_B_W1 == id)				{ wk = "裏面 W1";}
	else if (DIV_CAMSET_B_WS == id)				{ wk = "裏面 WS";}
#endif
#ifdef NUM_CAM_POS_6
	else if (DIV_CAMSET_T_DS == id)				{ wk = "表面 DS";}
	else if (DIV_CAMSET_T_D1 == id)				{ wk = "表面 D1";}
	else if (DIV_CAMSET_T_D2 == id)				{ wk = "表面 D2";}
	else if (DIV_CAMSET_T_W2 == id)				{ wk = "表面 W2";}
	else if (DIV_CAMSET_T_W1 == id)				{ wk = "表面 W1";}
	else if (DIV_CAMSET_T_WS == id)				{ wk = "表面 WS";}
	else if (DIV_CAMSET_B_DS == id)				{ wk = "裏面 DS";}
	else if (DIV_CAMSET_B_D1 == id)				{ wk = "裏面 D1";}
	else if (DIV_CAMSET_B_D2 == id)				{ wk = "裏面 D2";}
	else if (DIV_CAMSET_B_W2 == id)				{ wk = "裏面 W2";}
	else if (DIV_CAMSET_B_W1 == id)				{ wk = "裏面 W1";}
	else if (DIV_CAMSET_B_WS == id)				{ wk = "裏面 WS";}
#endif
#ifdef NUM_CAM_POS_7
	else if (DIV_CAMSET_T_DS == id)				{ wk = "表面 DS";}
	else if (DIV_CAMSET_T_D1 == id)				{ wk = "表面 D1";}
	else if (DIV_CAMSET_T_D2 == id)				{ wk = "表面 D2";}
	else if (DIV_CAMSET_T_C  == id)				{ wk = "表面 C";}
	else if (DIV_CAMSET_T_W2 == id)				{ wk = "表面 W2";}
	else if (DIV_CAMSET_T_W1 == id)				{ wk = "表面 W1";}
	else if (DIV_CAMSET_T_WS == id)				{ wk = "表面 WS";}
	else if (DIV_CAMSET_B_DS == id)				{ wk = "裏面 DS";}
	else if (DIV_CAMSET_B_D1 == id)				{ wk = "裏面 D1";}
	else if (DIV_CAMSET_B_D2 == id)				{ wk = "裏面 D2";}
	else if (DIV_CAMSET_B_C  == id)				{ wk = "裏面 C";}
	else if (DIV_CAMSET_B_W2 == id)				{ wk = "裏面 W2";}
	else if (DIV_CAMSET_B_W1 == id)				{ wk = "裏面 W1";}
	else if (DIV_CAMSET_B_WS == id)				{ wk = "裏面 WS";}
#endif
#ifdef NUM_CAM_POS_8
	else if (DIV_CAMSET_T_DS == id)				{ wk = "表面 DS";}
	else if (DIV_CAMSET_T_D1 == id)				{ wk = "表面 D1";}
	else if (DIV_CAMSET_T_D2 == id)				{ wk = "表面 D2";}
	else if (DIV_CAMSET_T_D3 == id)				{ wk = "表面 D3";}
	else if (DIV_CAMSET_T_W3 == id)				{ wk = "表面 W3";}
	else if (DIV_CAMSET_T_W2 == id)				{ wk = "表面 W2";}
	else if (DIV_CAMSET_T_W1 == id)				{ wk = "表面 W1";}
	else if (DIV_CAMSET_T_WS == id)				{ wk = "表面 WS";}
	else if (DIV_CAMSET_B_DS == id)				{ wk = "裏面 DS";}
	else if (DIV_CAMSET_B_D1 == id)				{ wk = "裏面 D1";}
	else if (DIV_CAMSET_B_D2 == id)				{ wk = "裏面 D2";}
	else if (DIV_CAMSET_B_D3 == id)				{ wk = "裏面 D3";}
	else if (DIV_CAMSET_B_W3 == id)				{ wk = "裏面 W3";}
	else if (DIV_CAMSET_B_W2 == id)				{ wk = "裏面 W2";}
	else if (DIV_CAMSET_B_W1 == id)				{ wk = "裏面 W1";}
	else if (DIV_CAMSET_B_WS == id)				{ wk = "裏面 WS";}
#endif
#ifdef NUM_CAM_POS_9
	else if (DIV_CAMSET_T_DS == id)				{ wk = "表面 DS"; }
	else if (DIV_CAMSET_T_D1 == id)				{ wk = "表面 D1"; }
	else if (DIV_CAMSET_T_D2 == id)				{ wk = "表面 D2"; }
	else if (DIV_CAMSET_T_D3 == id)				{ wk = "表面 D3"; }
	else if (DIV_CAMSET_T_C == id)				{ wk = "表面 C"; }
	else if (DIV_CAMSET_T_W3 == id)				{ wk = "表面 W3"; }
	else if (DIV_CAMSET_T_W2 == id)				{ wk = "表面 W2"; }
	else if (DIV_CAMSET_T_W1 == id)				{ wk = "表面 W1"; }
	else if (DIV_CAMSET_T_WS == id)				{ wk = "表面 WS"; }
	else if (DIV_CAMSET_B_DS == id)				{ wk = "裏面 DS"; }
	else if (DIV_CAMSET_B_D1 == id)				{ wk = "裏面 D1"; }
	else if (DIV_CAMSET_B_D2 == id)				{ wk = "裏面 D2"; }
	else if (DIV_CAMSET_B_D3 == id)				{ wk = "裏面 D3"; }
	else if (DIV_CAMSET_B_C == id)				{ wk = "裏面 C"; }
	else if (DIV_CAMSET_B_W3 == id)				{ wk = "裏面 W3"; }
	else if (DIV_CAMSET_B_W2 == id)				{ wk = "裏面 W2"; }
	else if (DIV_CAMSET_B_W1 == id)				{ wk = "裏面 W1"; }
	else if (DIV_CAMSET_B_WS == id)				{ wk = "裏面 WS"; }
#endif
#ifdef NUM_CAM_POS_10
	else if (DIV_CAMSET_T_DS == id)				{ wk = "表面 DS"; }
	else if (DIV_CAMSET_T_D1 == id)				{ wk = "表面 D1"; }
	else if (DIV_CAMSET_T_D2 == id)				{ wk = "表面 D2"; }
	else if (DIV_CAMSET_T_D3 == id)				{ wk = "表面 D3"; }
	else if (DIV_CAMSET_T_D4 == id)				{ wk = "表面 D4"; }
	else if (DIV_CAMSET_T_W4 == id)				{ wk = "表面 W4"; }
	else if (DIV_CAMSET_T_W3 == id)				{ wk = "表面 W3"; }
	else if (DIV_CAMSET_T_W2 == id)				{ wk = "表面 W2"; }
	else if (DIV_CAMSET_T_W1 == id)				{ wk = "表面 W1"; }
	else if (DIV_CAMSET_T_WS == id)				{ wk = "表面 WS"; }
	else if (DIV_CAMSET_B_DS == id)				{ wk = "裏面 DS"; }
	else if (DIV_CAMSET_B_D1 == id)				{ wk = "裏面 D1"; }
	else if (DIV_CAMSET_B_D2 == id)				{ wk = "裏面 D2"; }
	else if (DIV_CAMSET_B_D3 == id)				{ wk = "裏面 D3"; }
	else if (DIV_CAMSET_B_D4 == id)				{ wk = "裏面 D4"; }
	else if (DIV_CAMSET_B_W4 == id)				{ wk = "裏面 W4"; }
	else if (DIV_CAMSET_B_W3 == id)				{ wk = "裏面 W3"; }
	else if (DIV_CAMSET_B_W2 == id)				{ wk = "裏面 W2"; }
	else if (DIV_CAMSET_B_W1 == id)				{ wk = "裏面 W1"; }
	else if (DIV_CAMSET_B_WS == id)				{ wk = "裏面 WS"; }
#endif
#ifdef NUM_CAM_POS_11
	else if (DIV_CAMSET_T_DS == id)				{ wk = "表面 DS"; }
	else if (DIV_CAMSET_T_D1 == id)				{ wk = "表面 D1"; }
	else if (DIV_CAMSET_T_D2 == id)				{ wk = "表面 D2"; }
	else if (DIV_CAMSET_T_D3 == id)				{ wk = "表面 D3"; }
	else if (DIV_CAMSET_T_D4 == id)				{ wk = "表面 D4"; }
	else if (DIV_CAMSET_T_C == id)				{ wk = "表面 C"; }
	else if (DIV_CAMSET_T_W4 == id)				{ wk = "表面 W4"; }
	else if (DIV_CAMSET_T_W3 == id)				{ wk = "表面 W3"; }
	else if (DIV_CAMSET_T_W2 == id)				{ wk = "表面 W2"; }
	else if (DIV_CAMSET_T_W1 == id)				{ wk = "表面 W1"; }
	else if (DIV_CAMSET_T_WS == id)				{ wk = "表面 WS"; }
	else if (DIV_CAMSET_B_DS == id)				{ wk = "裏面 DS"; }
	else if (DIV_CAMSET_B_D1 == id)				{ wk = "裏面 D1"; }
	else if (DIV_CAMSET_B_D2 == id)				{ wk = "裏面 D2"; }
	else if (DIV_CAMSET_B_D3 == id)				{ wk = "裏面 D3"; }
	else if (DIV_CAMSET_B_D4 == id)				{ wk = "裏面 D4"; }
	else if (DIV_CAMSET_B_C == id)				{ wk = "裏面 C"; }
	else if (DIV_CAMSET_B_W4 == id)				{ wk = "裏面 W4"; }
	else if (DIV_CAMSET_B_W3 == id)				{ wk = "裏面 W3"; }
	else if (DIV_CAMSET_B_W2 == id)				{ wk = "裏面 W2"; }
	else if (DIV_CAMSET_B_W1 == id)				{ wk = "裏面 W1"; }
	else if (DIV_CAMSET_B_WS == id)				{ wk = "裏面 WS"; }
#endif
#ifdef NUM_CAM_POS_12
	else if (DIV_CAMSET_T_DS == id)				{ wk = "表面 DS"; }
	else if (DIV_CAMSET_T_D1 == id)				{ wk = "表面 D1"; }
	else if (DIV_CAMSET_T_D2 == id)				{ wk = "表面 D2"; }
	else if (DIV_CAMSET_T_D3 == id)				{ wk = "表面 D3"; }
	else if (DIV_CAMSET_T_D4 == id)				{ wk = "表面 D4"; }
	else if (DIV_CAMSET_T_D5 == id)				{ wk = "表面 D5"; }
	else if (DIV_CAMSET_T_W5 == id)				{ wk = "表面 W5"; }
	else if (DIV_CAMSET_T_W4 == id)				{ wk = "表面 W4"; }
	else if (DIV_CAMSET_T_W3 == id)				{ wk = "表面 W3"; }
	else if (DIV_CAMSET_T_W2 == id)				{ wk = "表面 W2"; }
	else if (DIV_CAMSET_T_W1 == id)				{ wk = "表面 W1"; }
	else if (DIV_CAMSET_T_WS == id)				{ wk = "表面 WS"; }
	else if (DIV_CAMSET_B_DS == id)				{ wk = "裏面 DS"; }
	else if (DIV_CAMSET_B_D1 == id)				{ wk = "裏面 D1"; }
	else if (DIV_CAMSET_B_D2 == id)				{ wk = "裏面 D2"; }
	else if (DIV_CAMSET_B_D3 == id)				{ wk = "裏面 D3"; }
	else if (DIV_CAMSET_B_D4 == id)				{ wk = "裏面 D4"; }
	else if (DIV_CAMSET_B_D5 == id)				{ wk = "裏面 D5"; }
	else if (DIV_CAMSET_B_W5 == id)				{ wk = "裏面 W5"; }
	else if (DIV_CAMSET_B_W4 == id)				{ wk = "裏面 W4"; }
	else if (DIV_CAMSET_B_W3 == id)				{ wk = "裏面 W3"; }
	else if (DIV_CAMSET_B_W2 == id)				{ wk = "裏面 W2"; }
	else if (DIV_CAMSET_B_W1 == id)				{ wk = "裏面 W1"; }
	else if (DIV_CAMSET_B_WS == id)				{ wk = "裏面 WS"; }
#endif
#ifdef NUM_CAM_POS_13
	else if (DIV_CAMSET_T_DS == id)				{ wk = "表面 DS"; }
	else if (DIV_CAMSET_T_D1 == id)				{ wk = "表面 D1"; }
	else if (DIV_CAMSET_T_D2 == id)				{ wk = "表面 D2"; }
	else if (DIV_CAMSET_T_D3 == id)				{ wk = "表面 D3"; }
	else if (DIV_CAMSET_T_D4 == id)				{ wk = "表面 D4"; }
	else if (DIV_CAMSET_T_D5 == id)				{ wk = "表面 D5"; }
	else if (DIV_CAMSET_T_C == id)				{ wk = "表面 C"; }
	else if (DIV_CAMSET_T_W5 == id)				{ wk = "表面 W5"; }
	else if (DIV_CAMSET_T_W4 == id)				{ wk = "表面 W4"; }
	else if (DIV_CAMSET_T_W3 == id)				{ wk = "表面 W3"; }
	else if (DIV_CAMSET_T_W2 == id)				{ wk = "表面 W2"; }
	else if (DIV_CAMSET_T_W1 == id)				{ wk = "表面 W1"; }
	else if (DIV_CAMSET_T_WS == id)				{ wk = "表面 WS"; }
	else if (DIV_CAMSET_B_DS == id)				{ wk = "裏面 DS"; }
	else if (DIV_CAMSET_B_D1 == id)				{ wk = "裏面 D1"; }
	else if (DIV_CAMSET_B_D2 == id)				{ wk = "裏面 D2"; }
	else if (DIV_CAMSET_B_D3 == id)				{ wk = "裏面 D3"; }
	else if (DIV_CAMSET_B_D4 == id)				{ wk = "裏面 D4"; }
	else if (DIV_CAMSET_B_D5 == id)				{ wk = "裏面 D5"; }
	else if (DIV_CAMSET_B_C == id)				{ wk = "裏面 C"; }
	else if (DIV_CAMSET_B_W5 == id)				{ wk = "裏面 W5"; }
	else if (DIV_CAMSET_B_W4 == id)				{ wk = "裏面 W4"; }
	else if (DIV_CAMSET_B_W3 == id)				{ wk = "裏面 W3"; }
	else if (DIV_CAMSET_B_W2 == id)				{ wk = "裏面 W2"; }
	else if (DIV_CAMSET_B_W1 == id)				{ wk = "裏面 W1"; }
	else if (DIV_CAMSET_B_WS == id)				{ wk = "裏面 WS"; }
#endif
#ifdef NUM_CAM_POS_14
	else if (DIV_CAMSET_T_DS == id)				{ wk = "表面 DS"; }
	else if (DIV_CAMSET_T_D1 == id)				{ wk = "表面 D1"; }
	else if (DIV_CAMSET_T_D2 == id)				{ wk = "表面 D2"; }
	else if (DIV_CAMSET_T_D3 == id)				{ wk = "表面 D3"; }
	else if (DIV_CAMSET_T_D4 == id)				{ wk = "表面 D4"; }
	else if (DIV_CAMSET_T_D5 == id)				{ wk = "表面 D5"; }
	else if (DIV_CAMSET_T_D6 == id)				{ wk = "表面 D6"; }
	else if (DIV_CAMSET_T_W6 == id)				{ wk = "表面 W6"; }
	else if (DIV_CAMSET_T_W5 == id)				{ wk = "表面 W5"; }
	else if (DIV_CAMSET_T_W4 == id)				{ wk = "表面 W4"; }
	else if (DIV_CAMSET_T_W3 == id)				{ wk = "表面 W3"; }
	else if (DIV_CAMSET_T_W2 == id)				{ wk = "表面 W2"; }
	else if (DIV_CAMSET_T_W1 == id)				{ wk = "表面 W1"; }
	else if (DIV_CAMSET_T_WS == id)				{ wk = "表面 WS"; }
	else if (DIV_CAMSET_B_DS == id)				{ wk = "裏面 DS"; }
	else if (DIV_CAMSET_B_D1 == id)				{ wk = "裏面 D1"; }
	else if (DIV_CAMSET_B_D2 == id)				{ wk = "裏面 D2"; }
	else if (DIV_CAMSET_B_D3 == id)				{ wk = "裏面 D3"; }
	else if (DIV_CAMSET_B_D4 == id)				{ wk = "裏面 D4"; }
	else if (DIV_CAMSET_B_D5 == id)				{ wk = "裏面 D5"; }
	else if (DIV_CAMSET_B_D6 == id)				{ wk = "裏面 D6"; }
	else if (DIV_CAMSET_B_W6 == id)				{ wk = "裏面 W6"; }
	else if (DIV_CAMSET_B_W5 == id)				{ wk = "裏面 W5"; }
	else if (DIV_CAMSET_B_W4 == id)				{ wk = "裏面 W4"; }
	else if (DIV_CAMSET_B_W3 == id)				{ wk = "裏面 W3"; }
	else if (DIV_CAMSET_B_W2 == id)				{ wk = "裏面 W2"; }
	else if (DIV_CAMSET_B_W1 == id)				{ wk = "裏面 W1"; }
	else if (DIV_CAMSET_B_WS == id)				{ wk = "裏面 WS"; }
#endif
#ifdef NUM_CAM_POS_15
	else if (DIV_CAMSET_T_DS == id)				{ wk = "表面 DS"; }
	else if (DIV_CAMSET_T_D1 == id)				{ wk = "表面 D1"; }
	else if (DIV_CAMSET_T_D2 == id)				{ wk = "表面 D2"; }
	else if (DIV_CAMSET_T_D3 == id)				{ wk = "表面 D3"; }
	else if (DIV_CAMSET_T_D4 == id)				{ wk = "表面 D4"; }
	else if (DIV_CAMSET_T_D5 == id)				{ wk = "表面 D5"; }
	else if (DIV_CAMSET_T_D6 == id)				{ wk = "表面 D6"; }
	else if (DIV_CAMSET_T_C == id)				{ wk = "表面 C"; }
	else if (DIV_CAMSET_T_W6 == id)				{ wk = "表面 W6"; }
	else if (DIV_CAMSET_T_W5 == id)				{ wk = "表面 W5"; }
	else if (DIV_CAMSET_T_W4 == id)				{ wk = "表面 W4"; }
	else if (DIV_CAMSET_T_W3 == id)				{ wk = "表面 W3"; }
	else if (DIV_CAMSET_T_W2 == id)				{ wk = "表面 W2"; }
	else if (DIV_CAMSET_T_W1 == id)				{ wk = "表面 W1"; }
	else if (DIV_CAMSET_T_WS == id)				{ wk = "表面 WS"; }
	else if (DIV_CAMSET_B_DS == id)				{ wk = "裏面 DS"; }
	else if (DIV_CAMSET_B_D1 == id)				{ wk = "裏面 D1"; }
	else if (DIV_CAMSET_B_D2 == id)				{ wk = "裏面 D2"; }
	else if (DIV_CAMSET_B_D3 == id)				{ wk = "裏面 D3"; }
	else if (DIV_CAMSET_B_D4 == id)				{ wk = "裏面 D4"; }
	else if (DIV_CAMSET_B_D5 == id)				{ wk = "裏面 D5"; }
	else if (DIV_CAMSET_B_D6 == id)				{ wk = "裏面 D6"; }
	else if (DIV_CAMSET_B_C == id)				{ wk = "裏面 C"; }
	else if (DIV_CAMSET_B_W6 == id)				{ wk = "裏面 W6"; }
	else if (DIV_CAMSET_B_W5 == id)				{ wk = "裏面 W5"; }
	else if (DIV_CAMSET_B_W4 == id)				{ wk = "裏面 W4"; }
	else if (DIV_CAMSET_B_W3 == id)				{ wk = "裏面 W3"; }
	else if (DIV_CAMSET_B_W2 == id)				{ wk = "裏面 W2"; }
	else if (DIV_CAMSET_B_W1 == id)				{ wk = "裏面 W1"; }
	else if (DIV_CAMSET_B_WS == id)				{ wk = "裏面 WS"; }
#endif
#ifdef NUM_CAM_POS_16
	else if (DIV_CAMSET_T_DS == id)				{ wk = "表面 DS"; }
	else if (DIV_CAMSET_T_D1 == id)				{ wk = "表面 D1"; }
	else if (DIV_CAMSET_T_D2 == id)				{ wk = "表面 D2"; }
	else if (DIV_CAMSET_T_D3 == id)				{ wk = "表面 D3"; }
	else if (DIV_CAMSET_T_D4 == id)				{ wk = "表面 D4"; }
	else if (DIV_CAMSET_T_D5 == id)				{ wk = "表面 D5"; }
	else if (DIV_CAMSET_T_D6 == id)				{ wk = "表面 D6"; }
	else if (DIV_CAMSET_T_D7 == id)				{ wk = "表面 D7"; }
	else if (DIV_CAMSET_T_W7 == id)				{ wk = "表面 W7"; }
	else if (DIV_CAMSET_T_W6 == id)				{ wk = "表面 W6"; }
	else if (DIV_CAMSET_T_W5 == id)				{ wk = "表面 W5"; }
	else if (DIV_CAMSET_T_W4 == id)				{ wk = "表面 W4"; }
	else if (DIV_CAMSET_T_W3 == id)				{ wk = "表面 W3"; }
	else if (DIV_CAMSET_T_W2 == id)				{ wk = "表面 W2"; }
	else if (DIV_CAMSET_T_W1 == id)				{ wk = "表面 W1"; }
	else if (DIV_CAMSET_T_WS == id)				{ wk = "表面 WS"; }
	else if (DIV_CAMSET_B_DS == id)				{ wk = "裏面 DS"; }
	else if (DIV_CAMSET_B_D1 == id)				{ wk = "裏面 D1"; }
	else if (DIV_CAMSET_B_D2 == id)				{ wk = "裏面 D2"; }
	else if (DIV_CAMSET_B_D3 == id)				{ wk = "裏面 D3"; }
	else if (DIV_CAMSET_B_D4 == id)				{ wk = "裏面 D4"; }
	else if (DIV_CAMSET_B_D5 == id)				{ wk = "裏面 D5"; }
	else if (DIV_CAMSET_B_D6 == id)				{ wk = "裏面 D6"; }
	else if (DIV_CAMSET_B_D7 == id)				{ wk = "裏面 D7"; }
	else if (DIV_CAMSET_B_W7 == id)				{ wk = "裏面 W7"; }
	else if (DIV_CAMSET_B_W6 == id)				{ wk = "裏面 W6"; }
	else if (DIV_CAMSET_B_W5 == id)				{ wk = "裏面 W5"; }
	else if (DIV_CAMSET_B_W4 == id)				{ wk = "裏面 W4"; }
	else if (DIV_CAMSET_B_W3 == id)				{ wk = "裏面 W3"; }
	else if (DIV_CAMSET_B_W2 == id)				{ wk = "裏面 W2"; }
	else if (DIV_CAMSET_B_W1 == id)				{ wk = "裏面 W1"; }
	else if (DIV_CAMSET_B_WS == id)				{ wk = "裏面 WS"; }
#endif
	else										{ wk.Format("ステータス異常 <%d>", id);}
	return wk;
}

