#include <crtdbg.h>
#include <malloc.h>

#include "ShdCmp_MainC.h"

using namespace ImgLibCLI;

//------------------------------------------
// コストラクタ
//------------------------------------------
ShdCmp_MainC::ShdCmp_MainC(void) :
m_bIsLpf(false),
mcls_pDeshade(NULL),
mcls_pParams(NULL)
{
}

//------------------------------------------
// デストラクタ
//------------------------------------------
ShdCmp_MainC::~ShdCmp_MainC(void)
{
	// もし開放してなかったら開放
	Free();
}

//------------------------------------------
// ロック
//------------------------------------------
BOOL ShdCmp_MainC::Alloc()
{
	Free();

	// クラス生成
	mcls_pDeshade = new CDeShadeMMX();
	mcls_pParams  = new CDeShadeMMX::Params();
	return TRUE;
}

//------------------------------------------
// 開放
//------------------------------------------
void ShdCmp_MainC::Free()
{
	if(NULL != mcls_pDeshade) {
		delete mcls_pDeshade;
		mcls_pDeshade = NULL;
	}
	if(NULL != mcls_pParams) {
		delete mcls_pParams;
		mcls_pParams = NULL;
	}
}

//------------------------------------------
// 画像サイズ可変のLPF生成
// long imgW 対象画像の幅   (imgMaxWより小さいこと)
// long imgH 対象画像の高さ (imgMaxWより小さいこと)
// long lpfW LPFブロック幅
// long lpfH LPFブロック高さ
// BYTE taget シェーリング補正の目標輝度値
//------------------------------------------
BOOL ShdCmp_MainC::CreateLPF(long imgW, long imgH, long lpfW, long lpfH, BYTE taget)
{
	// 画像サイズ変更
	m_nImgW = imgW;
	m_nImgH = imgH;

	// 自動的に開放
	if(m_bIsLpf) {
		mcls_pDeshade->Dispose();
		m_bIsLpf = false;
	}

	// LPF生成
	m_bIsLpf = true;
	mcls_pParams->lpf_width = lpfW;
	mcls_pParams->lpf_height = lpfH;
	mcls_pParams->target = taget;
	return mcls_pDeshade->Init(imgW, imgH, (CDeShadeMMX::Params const)*mcls_pParams);
}

//------------------------------------------
// シェーディング補正
// const BYTE* src 処理対象の画像データ
// BYTE* dst シェーディング補正後の画像データ
//------------------------------------------
BOOL ShdCmp_MainC::Exec(const BYTE* pSrc, BYTE* pDst)
{
	// 第一引数をシェーディング補正して、第二引数に入れる。
	mcls_pDeshade->Exec(pSrc, pDst);
	return TRUE;
}

//------------------------------------------
// シェーディング補正(エッジ外を黒埋め)
// long nPosL	左画像基点からの左エッジ位置 [pixel]
// long nPosR	左画像基点からの右エッジ位置 [pixel]
// const BYTE* src 処理対象の画像データ
// BYTE* dst シェーディング補正後の画像データ
//------------------------------------------
BOOL ShdCmp_MainC::ExecEdgeMask(long nPosL, long nPosR, const BYTE* pSrc, BYTE* pDst) {
	// 第一引数をシェーディング補正して、第二引数に入れる。
	mcls_pDeshade->ExecEdgeMask(nPosL, nPosR, pSrc, pDst);
	return 0;
}

//------------------------------------------
// シェーディング補正(エッジ外をコピー)
// long nPosL	左画像基点からの左エッジ位置 [pixel]
// long nPosR	左画像基点からの右エッジ位置 [pixel]
// const BYTE* src 処理対象の画像データ
// BYTE* dst シェーディング補正後の画像データ
//------------------------------------------
BOOL ShdCmp_MainC::ExecEdgeCopy(long nPosL, long nPosR, const BYTE* pSrc, BYTE* pDst) {
	// 第一引数をシェーディング補正して、第二引数に入れる。
	mcls_pDeshade->ExecEdgeCopy(nPosL, nPosR, pSrc, pDst);
	return 0;
}

// /////////////////////////////////////////////////////////////////////////////////
// スタティック用
// /////////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// シェーディング補正
// long imgW 対象画像の幅
// long imgH 対象画像の高さ
// const BYTE* src 処理対象の画像データ
// BYTE* dst シェーディング補正後の画像データ
//------------------------------------------
BOOL ShdCmp_MainC::ExecStatic(long imgW, long imgH, const BYTE* pSrc, BYTE* pDst)
{
	return ExecStatic(imgW, imgH, DEF_LPF_WIDTH, DEF_LPF_HEIGHT, DEF_LPF_TARGET, pSrc, pDst);
}


//------------------------------------------
// シェーディング補正
// long imgW 対象画像の幅
// long imgH 対象画像の高さ
// long lpfW LPFブロック幅
// long lpfH LPFブロック高さ
// BYTE taget シェーリング補正の目標輝度値
// const BYTE* src 処理対象の画像データ
// BYTE* dst シェーディング補正後の画像データ
//------------------------------------------
BOOL ShdCmp_MainC::ExecStatic(long imgW, long imgH, long lpfW, long lpfH, BYTE taget, const BYTE* pSrc, BYTE* pDst)
{
	bool bRetc = FALSE;
	CDeShadeMMX deshade;
	CDeShadeMMX::Params params;

	// 引数チェック
	_ASSERT(0 == lpfW%8);
	_ASSERT(0 == lpfH%8);


	// シェーディング補正のパラメータ。以下の値が常套。
	params.lpf_width = lpfW;
	params.lpf_height = lpfH;
	params.target = taget;
	if( ! deshade.Init(imgW, imgH, params) ) goto Ending;

	// 第一引数をシェーディング補正して、第二引数に入れる。
	deshade.Exec((const BYTE*)pSrc, (BYTE*)pDst);
	bRetc = TRUE;

Ending:
	deshade.Dispose();
	return bRetc;
}
