#include "StdAfx.h"
#include "IfRpiSave.h"
// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// コンストラクタ
//------------------------------------------
IfRpiSave::IfRpiSave(void)
{
}

//------------------------------------------
// デストラクタ
//------------------------------------------
IfRpiSave::~IfRpiSave(void)
{
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 全長画像保存関係のモジュール群

//------------------------------------------
// 全長画像処理 へ 検査開始要求
//------------------------------------------
void IfRpiSave::Send_TiRpiSave_Start()
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_TI_RPISAVE_01;
	que.nLineNo = getlineid();

	// 送信
#ifndef SEND_RPI_MGR
	send_mail(TI_RPISAVE, "*", &que);
#else
	send_mail(TO_RPIMGR, ".", &que);
#endif
}
//------------------------------------------
// 全長画像処理 へ 検査停止要求
//------------------------------------------
void IfRpiSave::Send_TiRpiSave_Stop()
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_TI_RPISAVE_02;
	que.nLineNo = getlineid();

	// 送信
#ifndef SEND_RPI_MGR
	send_mail(TI_RPISAVE, "*", &que);
#else
	send_mail(TO_RPIMGR, ".", &que);
#endif
}
//------------------------------------------
// 全長画像処理 へ 次コイル情報
// char const*          cKizukenNo 管理No
// COIL_SETTING const*  setting    コイル基本設定
// COIL_BASEDATA const* base       コイル基本情報
// int                  framenum   実行フレームNo
// int                  front_pos  先頭長手位置[mm]
// int                  nMen       実行面(0:表 1:裏)
//------------------------------------------
void IfRpiSave::Send_TiRpiSave_CoilSend(char const* cKizukenNo, COIL_SETTING const* setting, COIL_BASEDATA const * base, int framenum, int front_pos, int nMen)
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));
	que.nEventNo = FACT_TI_RPISAVE_04;
	que.nLineNo = getlineid();

	que.mix.idata[0] = nMen;		// 対象インスタンス (-1:全部 0～:対象インスタンス)
	//que.mix.idata[0] = -1;		// 対象インスタンス (-1:全部 0～:対象インスタンス) 今回は-1固定
	que.mix.idata[1] = setting->pcode[0];
	que.mix.idata[2] = setting->pcode[1];
	que.mix.idata[3] = setting->scode[0];
	que.mix.idata[4] = setting->scode[1];
	que.mix.idata[5] = framenum;
	que.mix.idata[6] = front_pos;
	memcpy(que.mix.cdata[0], cKizukenNo, SIZE_KIZUKEN_NO);
	memcpy(que.mix.cdata[1], base->cCoilNoIn, sizeof(base->cCoilNoIn));

	// 送信
#ifndef SEND_RPI_MGR
	send_mail(TI_RPISAVE, "*", &que);
#else
	send_mail(TO_RPIMGR, ".", &que);
#endif
}

