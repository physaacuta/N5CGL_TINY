#include <crtdbg.h>
#include <malloc.h>

#include "ShdCmp2_MainC.h"
#include "common/LPFSAT.h"
#include "common/ShdCmp_Devide.h"
#include <ipp.h>

using namespace ImgLibCLI;
#define ToBytePtr(x) x

//シェーディングの初期化
//imgW：  画像幅
//imgH：　画像高さ
//lpfW：　画像を平均化する幅。条件：SHRINK_WIDTHの倍数
//lpfH：　画像を平均化する高。条件：SHRINK_HEIGHTの倍数
//target: 目標輝度
ShdCmp2_MainC::ShdCmp2_MainC(int imgW, int imgH, int lpfW, int lpfH, BYTE target)
{
	mcls_pLPFSAT = NULL;
	mcls_pDivide = NULL;
	m_pLPFPic  = NULL;

	CreateLPF( imgW, imgH, lpfW, lpfH, target );
}

//シェーディングの初期化
//あとでCreateLPF呼んでね版
ShdCmp2_MainC::ShdCmp2_MainC()
{
	mcls_pLPFSAT = NULL;
	mcls_pDivide = NULL;
	m_pLPFPic  = NULL;
}


//シェーディングの初期化
//imgW：  画像幅
//imgH：　画像高さ
//lpfW：　画像を平均化する幅。条件：SHRINK_WIDTHの倍数
//lpfH：　画像を平均化する高。条件：SHRINK_HEIGHTの倍数
//target: 目標輝度
BOOL ShdCmp2_MainC::CreateLPF(int imgW, int imgH, int lpfW, int lpfH, BYTE target)
{
	Free();
	//strideは8byteアラインとする
	int stride = (imgW+3) & ~3;

	//LPF生成器作成・初期化
	mcls_pLPFSAT = new CLPFSAT();
	mcls_pLPFSAT->Init( imgW, stride, stride, imgH, lpfW, lpfH, SHRINK_WIDTH, SHRINK_HEIGHT);

	//目標輝度用の除算器を生成
	mcls_pDivide = new CDevideMMX();
	CDevideMMX::Params param = { target, 0 }; //目標設定。オフセットは無し
	mcls_pDivide->Init( imgW, imgH, param, false );

	//LPF格納用バッファ作成
	m_pLPFPic =  (BYTE*)_aligned_malloc( stride * imgH , 32);

	return TRUE;
}


//特に何もしない、インターフェース共通用。
//IPPの初期化をしておく
BOOL					ShdCmp2_MainC::Alloc()										// ロック
{
	ippStaticInit();
	return TRUE;
}

//特に何もしない、インターフェース共通用。
void					ShdCmp2_MainC::Free()
{
	delete mcls_pDivide;  
	mcls_pDivide = NULL;

	delete mcls_pLPFSAT;
	mcls_pDivide = NULL;

	if( m_pLPFPic ){ 
		_aligned_free( m_pLPFPic);
		m_pLPFPic = NULL ;
	}

}

//解放
ShdCmp2_MainC::~ShdCmp2_MainC()
{
	Free();
}
// シェーディング補正　全域
bool	ShdCmp2_MainC::Exec(const BYTE * pSrc, BYTE * pDst)		
{
	int width = mcls_pLPFSAT->my_width;
	mcls_pLPFSAT->ExecEdge( ToBytePtr(pSrc), ToBytePtr(m_pLPFPic), 0, width -1, 0, width-1, 1,1);
	mcls_pDivide->Exec    ( ToBytePtr(pSrc), ToBytePtr(m_pLPFPic), ToBytePtr( pDst ) );
	return true;
}


// シェーディング補正　エッジ外部LPFサイズ減少機能付き
bool	ShdCmp2_MainC::ExecEdgeCopy(int lEdge, int rEdge, const BYTE * pSrc, BYTE * pDst)
{
	int width = mcls_pLPFSAT->my_width;
	mcls_pLPFSAT->ExecEdgeTarget( ToBytePtr(pSrc), ToBytePtr(m_pLPFPic), 0, width -1, lEdge, rEdge, 1, 1, m_target );
	mcls_pDivide->Exec    ( ToBytePtr(pSrc), ToBytePtr(m_pLPFPic), ToBytePtr( pDst ) );
	return true;

}



// シェーディング補正　エッジ外部0化
bool	ShdCmp2_MainC::ExecEdgeMask(int lEdge, int rEdge, const BYTE * pSrc, BYTE * pDst)
{
	int width = mcls_pLPFSAT->my_width;
	mcls_pLPFSAT->ExecEdgeTarget( ToBytePtr(pSrc), ToBytePtr(m_pLPFPic), 0, width -1, lEdge, rEdge, 1, 1, 0 );
	mcls_pDivide->Exec    ( ToBytePtr(pSrc), ToBytePtr(m_pLPFPic), ToBytePtr( pDst ) );
	return true;

}





//static メソッド群

//STATIC シェーディング補正　全域
bool ShdCmp2_MainC::ExecStatic(int imgW, int imgH, const BYTE * pSrc, BYTE * pDst)
{
	ShdCmp2_MainC sc( imgW, imgH, DEF_LPF_WIDTH, DEF_LPF_HEIGHT, DEF_LPF_TARGET );
	return sc.Exec( pSrc, pDst );
}


//STATIC シェーディング補正　全域　LPF・TARGET指定版
bool	ShdCmp2_MainC::ExecStatic(int imgW, int imgH, int lpfW, int lpfH, BYTE target, const  BYTE * pSrc,BYTE * pDst)
{
	ShdCmp2_MainC sc( imgW, imgH, lpfW, lpfH, target);
	return sc.Exec( pSrc, pDst );
}


//------------------------------------------
// 初期化
// 　起動後に１回呼び出しておくと、
// 　IPPが実行時のCPUに応じて最適な処理を選択する
//------------------------------------------
int ShdCmp2_MainC::ippStaticInit()
{
	static LONG bIppInited = 0;
	if( InterlockedIncrement( &bIppInited ) == 1 ){
		return ::ippStaticInit();
	}
	return 0;
}
