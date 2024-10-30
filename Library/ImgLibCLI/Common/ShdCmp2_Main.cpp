
#include "ShdCmp2_Main.h"
//LPFSAT�̎�����IPP���g���Ă���̂ŁAIPP���ł́A�N���X��������B
#ifdef DEFECTLIB_KIZUJPEGMANAGER


#include "lpfsat.h"
#include "ShdCmp_Devide.h"

#include <crtdbg.h>

using namespace System;
using namespace System::Runtime::InteropServices;

using namespace ImgLibCLI;


//IntPtr -> BYTE*�ւ̃L���X�g
static BYTE *ToBytePtr( IntPtr p )
{
	return (BYTE*)p.ToPointer();
}

//�V�F�[�f�B���O�̏�����
//imgW�F  �摜��
//imgH�F�@�摜����
//lpfW�F�@�摜�𕽋ω����镝�FSHRINK_WIDTH�̔{���̂���
//lpfH�F�@�摜�𕽋ω����鍂�FSHRINK_HEIGHT�̔{���̂���
//target: �ڕW�P�x
ShadingComp2::ShadingComp2(int imgW, int imgH, int lpfW, int lpfH, BYTE target)
{
	Alloc( imgW, imgH );
	CreateLPF( lpfW, lpfH, target );
}

//���
ShadingComp2::~ShadingComp2()
{
	Free();
}

// �R���X�g���N�^
ShadingComp2::ShadingComp2()		
{
	mcls_pDivide = NULL;
	mcls_pLPFSAT = NULL;
	m_pLPFPic = IntPtr::Zero;
	m_nImgWidth = 0;
	m_nImgHeight = 0;
}

bool					ShadingComp2::Alloc(long imgW, long imgH)					// ���b�N
{
	m_nImgWidth = imgW;
	m_nImgHeight = imgH;

	return true;
}

void					ShadingComp2::Free()										// �J��
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
bool					ShadingComp2::CreateLPF()									// LPF����
{
	return CreateLPF( DEF_LPF_WIDTH, DEF_LPF_HEIGHT, DEF_LPF_TARGET);
}

bool					ShadingComp2::CreateLPF(long lpfW, long lpfH, BYTE target)	// LPF����
{
	Free();

	if(m_nImgWidth  == 0 || m_nImgHeight==0 )
		return false;

	//stride��8byte�A���C���Ƃ���
	int stride = (m_nImgWidth+3) & ~3;

	//LPF������쐬�E������
	mcls_pLPFSAT = new CLPFSAT();
	mcls_pLPFSAT->Init( m_nImgWidth, stride, stride, m_nImgHeight, lpfW, lpfH, SHRINK_WIDTH, SHRINK_HEIGHT);

	//�͈͊O�P�x�����摜�Ɠ����ɂ��邽�ߑΏۊO�B
	m_target = target;

	//�ڕW�P�x�p�̏��Z��𐶐�
	mcls_pDivide = new CDevideMMX();
	CDevideMMX::Params param = { target, 0 }; //�ڕW�ݒ�B�I�t�Z�b�g�͖���
	mcls_pDivide->Init( m_nImgWidth, m_nImgHeight, param, false );

	//LPF�i�[�p�o�b�t�@�쐬
	m_pLPFPic = Marshal::AllocHGlobal( stride * m_nImgHeight );
	return true;
}






// �V�F�[�f�B���O�␳�@�S��
bool	ShadingComp2::Exec(array<BYTE>^ pSrc, array<BYTE>^ pDst)		
{
	pin_ptr<BYTE> ppSrc( &pSrc[0] ) ;
	pin_ptr<BYTE> ppDst( &pDst[0] ) ;
	return this->Exec( IntPtr(ppSrc),  IntPtr(ppDst) );
}

// �V�F�[�f�B���O�␳�@�S��
bool	ShadingComp2::Exec(IntPtr pSrc, IntPtr pDst)		
{
	int width = mcls_pLPFSAT->my_width;
	mcls_pLPFSAT->ExecEdge( ToBytePtr(pSrc), ToBytePtr(m_pLPFPic), 0, width -1, 0, width-1, 1,1);
	mcls_pDivide->Exec    ( ToBytePtr(pSrc), ToBytePtr(m_pLPFPic), ToBytePtr( pDst ) );
	return true;
}

// �V�F�[�f�B���O�␳�@�G�b�W�O���͌��摜�̂܂�
bool	ShadingComp2::ExecEdgeCopy(array<BYTE>^ pSrc, array<BYTE>^ pDst, int lEdge, int rEdge)
{
	pin_ptr<BYTE> ppSrc( &pSrc[0] ) ;
	pin_ptr<BYTE> ppDst( &pDst[0] ) ;
	return this->ExecEdgeCopy( IntPtr(ppSrc),  IntPtr(ppDst), lEdge, rEdge );

}

// �V�F�[�f�B���O�␳�@�G�b�W�O���͌��摜�̂܂�
bool	ShadingComp2::ExecEdgeCopy(IntPtr pSrc, IntPtr pDst, int lEdge, int rEdge)
{
	int width = mcls_pLPFSAT->my_width;
	//�G�b�W�O��m_target�Ŗ��߂�w��B����Divide�ŁAm_target/m_target�łP�i���摜�j�ɂȂ�v�Z�B
	mcls_pLPFSAT->ExecEdgeTarget( ToBytePtr(pSrc), ToBytePtr(m_pLPFPic), 0, width -1, lEdge, rEdge, 1, 1, m_target );
	mcls_pDivide->Exec    ( ToBytePtr(pSrc), ToBytePtr(m_pLPFPic), ToBytePtr( pDst ) );
	return true;

}


// �V�F�[�f�B���O�␳�@�G�b�W�O��0��
bool	ShadingComp2::ExecEdgeMask(array<BYTE>^ pSrc, array<BYTE>^ pDst, int lEdge, int rEdge)
{
	pin_ptr<BYTE> ppSrc( &pSrc[0] ) ;
	pin_ptr<BYTE> ppDst( &pDst[0] ) ;
	return this->ExecEdgeMask( IntPtr(ppSrc),  IntPtr(ppDst), lEdge, rEdge );

}

// �V�F�[�f�B���O�␳�@�G�b�W�O��0��
bool	ShadingComp2::ExecEdgeMask(IntPtr pSrc, IntPtr pDst, int lEdge, int rEdge)
{
	int width = mcls_pLPFSAT->my_width;
	mcls_pLPFSAT->ExecEdgeTarget( ToBytePtr(pSrc), ToBytePtr(m_pLPFPic), 0, width -1, lEdge, rEdge, 1, 1, 0 );
	mcls_pDivide->Exec    ( ToBytePtr(pSrc), ToBytePtr(m_pLPFPic), ToBytePtr( pDst ) );
	return true;

}






//static ���\�b�h�Q

//STATIC �V�F�[�f�B���O�␳�@�S��
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


//STATIC �V�F�[�f�B���O�␳�@�S��@LPF�ETARGET�w���
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