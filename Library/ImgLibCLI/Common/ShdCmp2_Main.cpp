
#include "ShdCmp2_Main.h"
//LPFSATの実装にIPPを使っているので、IPP無では、クラス毎消える。
#ifdef DEFECTLIB_KIZUJPEGMANAGER


#include "lpfsat.h"
#include "ShdCmp_Devide.h"

#include <crtdbg.h>

using namespace System;
using namespace System::Runtime::InteropServices;

using namespace ImgLibCLI;


//IntPtr -> BYTE*へのキャスト
static BYTE *ToBytePtr( IntPtr p )
{
	return (BYTE*)p.ToPointer();
}

//シェーディングの初期化
//imgW：  画像幅
//imgH：　画像高さ
//lpfW：　画像を平均化する幅：SHRINK_WIDTHの倍数のこと
//lpfH：　画像を平均化する高：SHRINK_HEIGHTの倍数のこと
//target: 目標輝度
ShadingComp2::ShadingComp2(int imgW, int imgH, int lpfW, int lpfH, BYTE target)
{
	Alloc( imgW, imgH );
	CreateLPF( lpfW, lpfH, target );
}

//解放
ShadingComp2::~ShadingComp2()
{
	Free();
}

// コンストラクタ
ShadingComp2::ShadingComp2()		
{
	mcls_pDivide = NULL;
	mcls_pLPFSAT = NULL;
	m_pLPFPic = IntPtr::Zero;
	m_nImgWidth = 0;
	m_nImgHeight = 0;
}

bool					ShadingComp2::Alloc(long imgW, long imgH)					// ロック
{
	m_nImgWidth = imgW;
	m_nImgHeight = imgH;

	return true;
}

void					ShadingComp2::Free()										// 開放
{
	if( mcls_pDivide ){
		delete mcls_pDivide;
		delete mcls_pLPFSAT;
		Marshal::FreeHGlobal(  m_pLPFPic );

		mcls_pDivide = NULL;
		mcls_pLPFSAT = NULL;
		m_pLPFPic = IntPtr::Zero;
	}
}
bool					ShadingComp2::CreateLPF()									// LPF生成
{
	return CreateLPF( DEF_LPF_WIDTH, DEF_LPF_HEIGHT, DEF_LPF_TARGET);
}

bool					ShadingComp2::CreateLPF(long lpfW, long lpfH, BYTE target)	// LPF生成
{
	Free();

	if(m_nImgWidth  == 0 || m_nImgHeight==0 )
		return false;

	//strideは8byteアラインとする
	int stride = (m_nImgWidth+3) & ~3;

	//LPF生成器作成・初期化
	mcls_pLPFSAT = new CLPFSAT();
	mcls_pLPFSAT->Init( m_nImgWidth, stride, stride, m_nImgHeight, lpfW, lpfH, SHRINK_WIDTH, SHRINK_HEIGHT);

	//範囲外輝度を元画像と同じにするため対象外。
	m_target = target;

	//目標輝度用の除算器を生成
	mcls_pDivide = new CDevideMMX();
	CDevideMMX::Params param = { target, 0 }; //目標設定。オフセットは無し
	mcls_pDivide->Init( m_nImgWidth, m_nImgHeight, param, false );

	//LPF格納用バッファ作成
	m_pLPFPic = Marshal::AllocHGlobal( stride * m_nImgHeight );
	return true;
}






// シェーディング補正　全域
bool	ShadingComp2::Exec(array<BYTE>^ pSrc, array<BYTE>^ pDst)		
{
	pin_ptr<BYTE> ppSrc( &pSrc[0] ) ;
	pin_ptr<BYTE> ppDst( &pDst[0] ) ;
	return this->Exec( IntPtr(ppSrc),  IntPtr(ppDst) );
}

// シェーディング補正　全域
bool	ShadingComp2::Exec(IntPtr pSrc, IntPtr pDst)		
{
	int width = mcls_pLPFSAT->my_width;
	mcls_pLPFSAT->ExecEdge( ToBytePtr(pSrc), ToBytePtr(m_pLPFPic), 0, width -1, 0, width-1, 1,1);
	mcls_pDivide->Exec    ( ToBytePtr(pSrc), ToBytePtr(m_pLPFPic), ToBytePtr( pDst ) );
	return true;
}

// シェーディング補正　エッジ外部は元画像のまま
bool	ShadingComp2::ExecEdgeCopy(array<BYTE>^ pSrc, array<BYTE>^ pDst, int lEdge, int rEdge)
{
	pin_ptr<BYTE> ppSrc( &pSrc[0] ) ;
	pin_ptr<BYTE> ppDst( &pDst[0] ) ;
	return this->ExecEdgeCopy( IntPtr(ppSrc),  IntPtr(ppDst), lEdge, rEdge );

}

// シェーディング補正　エッジ外部は元画像のまま
bool	ShadingComp2::ExecEdgeCopy(IntPtr pSrc, IntPtr pDst, int lEdge, int rEdge)
{
	int width = mcls_pLPFSAT->my_width;
	//エッジ外はm_targetで埋める指定。次のDivideで、m_target/m_targetで１（元画像）になる計算。
	mcls_pLPFSAT->ExecEdgeTarget( ToBytePtr(pSrc), ToBytePtr(m_pLPFPic), 0, width -1, lEdge, rEdge, 1, 1, m_target );
	mcls_pDivide->Exec    ( ToBytePtr(pSrc), ToBytePtr(m_pLPFPic), ToBytePtr( pDst ) );
	return true;

}


// シェーディング補正　エッジ外部0化
bool	ShadingComp2::ExecEdgeMask(array<BYTE>^ pSrc, array<BYTE>^ pDst, int lEdge, int rEdge)
{
	pin_ptr<BYTE> ppSrc( &pSrc[0] ) ;
	pin_ptr<BYTE> ppDst( &pDst[0] ) ;
	return this->ExecEdgeMask( IntPtr(ppSrc),  IntPtr(ppDst), lEdge, rEdge );

}

// シェーディング補正　エッジ外部0化
bool	ShadingComp2::ExecEdgeMask(IntPtr pSrc, IntPtr pDst, int lEdge, int rEdge)
{
	int width = mcls_pLPFSAT->my_width;
	mcls_pLPFSAT->ExecEdgeTarget( ToBytePtr(pSrc), ToBytePtr(m_pLPFPic), 0, width -1, lEdge, rEdge, 1, 1, 0 );
	mcls_pDivide->Exec    ( ToBytePtr(pSrc), ToBytePtr(m_pLPFPic), ToBytePtr( pDst ) );
	return true;

}






//static メソッド群

//STATIC シェーディング補正　全域
bool ShadingComp2::ExecStatic(long imgW, long imgH, array<BYTE>^ pSrc, array<BYTE>^ pDst)
{
	pin_ptr<BYTE> ppSrc( &pSrc[0] ) ;
	pin_ptr<BYTE> ppDst( &pDst[0] ) ;
	return ExecStatic( imgW, imgH, IntPtr(ppSrc),  IntPtr(ppDst));
}
bool ShadingComp2::ExecStatic(long imgW, long imgH, IntPtr pSrc, IntPtr pDst)
{
	ShadingComp2 sc( imgW, imgH, DEF_LPF_WIDTH, DEF_LPF_HEIGHT, DEF_LPF_TARGET );
	return sc.Exec( pSrc, pDst );
}


//STATIC シェーディング補正　全域　LPF・TARGET指定版
bool	ShadingComp2::ExecStatic(long imgW, long imgH, long lpfW, long lpfH, BYTE target, array<BYTE>^ pSrc, array<BYTE>^ pDst)
{
	pin_ptr<BYTE> ppSrc( &pSrc[0] ) ;
	pin_ptr<BYTE> ppDst( &pDst[0] ) ;
	return ExecStatic( imgW, imgH, lpfW, lpfH, target ,IntPtr(ppSrc),  IntPtr(ppDst) );
}

bool	ShadingComp2::ExecStatic(long imgW, long imgH, long lpfW, long lpfH, BYTE target,IntPtr pSrc,IntPtr pDst)
{
	ShadingComp2 sc( imgW, imgH, lpfW, lpfH, target);
	return sc.Exec( pSrc, pDst );
}
#endif