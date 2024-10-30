// これは メイン DLL ファイルです。

#include "ShdCmp_Main.h"
#include <crtdbg.h>

using namespace System;
using namespace System::Runtime::InteropServices;

using namespace ImgLibCLI;

//------------------------------------------
// コストラクタ
//------------------------------------------
ShadingComp::ShadingComp() :
m_bIsLpf(false),
mcls_pDeshade(NULL),
mcls_pParams(NULL),
m_pImgBuf(IntPtr::Zero)
{

}

//------------------------------------------
// デストラクタ
//------------------------------------------
ShadingComp::~ShadingComp(void)
{
	// もし開放してなかったら開放
	Free();
}


//------------------------------------------
// ロック
// long imgW 対象画像の幅
// long imgH 対象画像の高さ
//------------------------------------------
BOOL ShadingComp::Alloc(long imgW, long imgH)
{
	//if(NULL != m_pImgBuf) return false;
	if(0 == m_pImgBuf.Size) return false;
	
	// 領域確保
	//m_pImgBuf = (BYTE*)_aligned_malloc(imgW * imgH, 32);
	m_pImgBuf = Marshal::AllocHGlobal( imgW * imgH );
	
	// クラス生成
	mcls_pDeshade = new CDeShadeMMX();
	mcls_pParams  = new CDeShadeMMX::Params();

	// 変数保持
	m_nImgW = imgW;
	m_nImgH = imgH;

	return TRUE;
}

//------------------------------------------
// 開放
//------------------------------------------
void ShadingComp::Free()
{
	//if(NULL != m_pImgBuf) {
	if(IntPtr::Zero != m_pImgBuf) {
		//_aligned_free(m_pImgBuf);
		//m_pImgBuf = NULL;
		Marshal::FreeHGlobal(m_pImgBuf);
		m_pImgBuf = IntPtr::Zero;
	}
	if(NULL != mcls_pDeshade) {
		delete mcls_pDeshade;
		mcls_pDeshade = NULL;
	}
	if(NULL != mcls_pParams) {
		delete mcls_pParams;
		mcls_pParams = NULL;
	}

	GC::Collect();
}

//------------------------------------------
// LPF生成
// long lpfW LPFブロック幅
// long lpfH LPFブロック高さ
// BYTE taget シェーリング補正の目標輝度値
//------------------------------------------
BOOL ShadingComp::CreateLPF(long lpfW, long lpfH, BYTE taget)
{
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
	return mcls_pDeshade->Init(m_nImgW, m_nImgH, (CDeShadeMMX::Params const)*mcls_pParams);
}

BOOL ShadingComp::CreateLPF()
{
	return CreateLPF(DEF_LPF_WIDTH, DEF_LPF_HEIGHT, DEF_LPF_TARGET);
}


//------------------------------------------
// シェーディング補正
// const BYTE* src 処理対象の画像データ
// BYTE* dst シェーディング補正後の画像データ
//------------------------------------------
BOOL ShadingComp::Exec(array<BYTE>^ pSrc, array<BYTE>^ pDst)
{
	// 処理データセット
	Marshal::Copy(pSrc, 0, (IntPtr)m_pImgBuf, m_nImgW * m_nImgH);

	// 第一引数をシェーディング補正して、第二引数に入れる。
	//mcls_pDeshade->Exec((const BYTE*)m_pImgBuf, (BYTE*)m_pImgBuf);
	mcls_pDeshade->Exec((const BYTE*)m_pImgBuf.ToPointer(), (BYTE*)m_pImgBuf.ToPointer());
	
	// 結果データセット
	Marshal::Copy((IntPtr)m_pImgBuf, pDst, 0, m_nImgW * m_nImgH);
	return TRUE;
}
//------------------------------------------
// シェーディング補正(エッジ外黒埋め)
// const BYTE* src 処理対象の画像データ
// BYTE* dst シェーディング補正後の画像データ
// long lEdge 左エッジ位置 (0オリジン) [pixel]
// long rEdge 右エッジ位置 (0オリジン) [pixel]
//------------------------------------------
BOOL ShadingComp::ExecEdgeMask(array<BYTE>^ pSrc, array<BYTE>^ pDst, long lEdge, long rEdge)
{
	// 処理データセット
	Marshal::Copy(pSrc, 0, (IntPtr)m_pImgBuf, m_nImgW * m_nImgH);

	// 第一引数をシェーディング補正して、第二引数に入れる。
	mcls_pDeshade->ExecEdgeMask(lEdge, rEdge, (const BYTE*)m_pImgBuf.ToPointer(), (BYTE*)m_pImgBuf.ToPointer());
	
	// 結果データセット
	Marshal::Copy((IntPtr)m_pImgBuf, pDst, 0, m_nImgW * m_nImgH);
	return TRUE;
}
//------------------------------------------
// シェーディング補正(エッジ外は元画像)
// const BYTE* src 処理対象の画像データ
// BYTE* dst シェーディング補正後の画像データ
// long lEdge 左エッジ位置 (0オリジン) [pixel]
// long rEdge 右エッジ位置 (0オリジン) [pixel]
//------------------------------------------
BOOL ShadingComp::ExecEdgeCopy(array<BYTE>^ pSrc, array<BYTE>^ pDst, long lEdge, long rEdge)
{
	// 処理データセット
	Marshal::Copy(pSrc, 0, (IntPtr)m_pImgBuf, m_nImgW * m_nImgH);

	// 第一引数をシェーディング補正して、第二引数に入れる。
	mcls_pDeshade->ExecEdgeCopy(lEdge, rEdge, (const BYTE*)m_pImgBuf.ToPointer(), (BYTE*)m_pImgBuf.ToPointer());
	
	// 結果データセット
	Marshal::Copy((IntPtr)m_pImgBuf, pDst, 0, m_nImgW * m_nImgH);
	return TRUE;
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
BOOL ShadingComp::ExecStatic(long imgW, long imgH, array<BYTE>^ pSrc, array<BYTE>^ pDst)
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
BOOL ShadingComp::ExecStatic(long imgW, long imgH, long lpfW, long lpfH, BYTE taget, array<BYTE>^ pSrc, array<BYTE>^ pDst)
{
	bool bRetc = FALSE;
	CDeShadeMMX deshade;
	CDeShadeMMX::Params params;

	// 引数チェック
	_ASSERT(0 == lpfW%8);
	_ASSERT(0 == lpfH%8);


	// 処理メモリロック
//	BYTE* p = (BYTE*)_aligned_malloc(imgW * imgH, 32);
	IntPtr p = Marshal::AllocHGlobal( imgW * imgH );
	Marshal::Copy(pSrc, 0, (IntPtr)p, imgW * imgH);

	// シェーディング補正のパラメータ。以下の値が常套。
	params.lpf_width = lpfW;
	params.lpf_height = lpfH;
	params.target = taget;
	if( ! deshade.Init(imgW, imgH, params) ) goto Ending;

	// 第一引数をシェーディング補正して、第二引数に入れる。
//	deshade.Exec((const BYTE*)p, (BYTE*)p);
	deshade.Exec((const BYTE*)p.ToPointer(), (BYTE*)p.ToPointer());
	bRetc = TRUE;

	// 結果データセット
//	Marshal::Copy(p, pDst, 0, w * h);
	Marshal::Copy((IntPtr)p, pDst, 0, imgW * imgH);

Ending:
	// 処理メモリ開放
//	_aligned_free(p);
	Marshal::FreeHGlobal(p);
	return bRetc;
}


