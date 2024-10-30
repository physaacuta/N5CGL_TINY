#include <crtdbg.h>
#include <malloc.h>

#include "ShdCmp2_MainC.h"
#include "common/LPFSAT.h"
#include "common/ShdCmp_Devide.h"
#include <ipp.h>

using namespace ImgLibCLI;
#define ToBytePtr(x) x

//�V�F�[�f�B���O�̏�����
//imgW�F  �摜��
//imgH�F�@�摜����
//lpfW�F�@�摜�𕽋ω����镝�B�����FSHRINK_WIDTH�̔{��
//lpfH�F�@�摜�𕽋ω����鍂�B�����FSHRINK_HEIGHT�̔{��
//target: �ڕW�P�x
ShdCmp2_MainC::ShdCmp2_MainC(int imgW, int imgH, int lpfW, int lpfH, BYTE target)
{
	mcls_pLPFSAT = NULL;
	mcls_pDivide = NULL;
	m_pLPFPic  = NULL;

	CreateLPF( imgW, imgH, lpfW, lpfH, target );
}

//�V�F�[�f�B���O�̏�����
//���Ƃ�CreateLPF�Ă�ł˔�
ShdCmp2_MainC::ShdCmp2_MainC()
{
	mcls_pLPFSAT = NULL;
	mcls_pDivide = NULL;
	m_pLPFPic  = NULL;
}


//�V�F�[�f�B���O�̏�����
//imgW�F  �摜��
//imgH�F�@�摜����
//lpfW�F�@�摜�𕽋ω����镝�B�����FSHRINK_WIDTH�̔{��
//lpfH�F�@�摜�𕽋ω����鍂�B�����FSHRINK_HEIGHT�̔{��
//target: �ڕW�P�x
BOOL ShdCmp2_MainC::CreateLPF(int imgW, int imgH, int lpfW, int lpfH, BYTE target)
{
	Free();
	//stride��8byte�A���C���Ƃ���
	int stride = (imgW+3) & ~3;

	//LPF������쐬�E������
	mcls_pLPFSAT = new CLPFSAT();
	mcls_pLPFSAT->Init( imgW, stride, stride, imgH, lpfW, lpfH, SHRINK_WIDTH, SHRINK_HEIGHT);

	//�ڕW�P�x�p�̏��Z��𐶐�
	mcls_pDivide = new CDevideMMX();
	CDevideMMX::Params param = { target, 0 }; //�ڕW�ݒ�B�I�t�Z�b�g�͖���
	mcls_pDivide->Init( imgW, imgH, param, false );

	//LPF�i�[�p�o�b�t�@�쐬
	m_pLPFPic =  (BYTE*)_aligned_malloc( stride * imgH , 32);

	return TRUE;
}


//���ɉ������Ȃ��A�C���^�[�t�F�[�X���ʗp�B
//IPP�̏����������Ă���
BOOL					ShdCmp2_MainC::Alloc()										// ���b�N
{
	ippStaticInit();
	return TRUE;
}

//���ɉ������Ȃ��A�C���^�[�t�F�[�X���ʗp�B
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

//���
ShdCmp2_MainC::~ShdCmp2_MainC()
{
	Free();
}
// �V�F�[�f�B���O�␳�@�S��
bool	ShdCmp2_MainC::Exec(const BYTE * pSrc, BYTE * pDst)		
{
	int width = mcls_pLPFSAT->my_width;
	mcls_pLPFSAT->ExecEdge( ToBytePtr(pSrc), ToBytePtr(m_pLPFPic), 0, width -1, 0, width-1, 1,1);
	mcls_pDivide->Exec    ( ToBytePtr(pSrc), ToBytePtr(m_pLPFPic), ToBytePtr( pDst ) );
	return true;
}


// �V�F�[�f�B���O�␳�@�G�b�W�O��LPF�T�C�Y�����@�\�t��
bool	ShdCmp2_MainC::ExecEdgeCopy(int lEdge, int rEdge, const BYTE * pSrc, BYTE * pDst)
{
	int width = mcls_pLPFSAT->my_width;
	mcls_pLPFSAT->ExecEdgeTarget( ToBytePtr(pSrc), ToBytePtr(m_pLPFPic), 0, width -1, lEdge, rEdge, 1, 1, m_target );
	mcls_pDivide->Exec    ( ToBytePtr(pSrc), ToBytePtr(m_pLPFPic), ToBytePtr( pDst ) );
	return true;

}



// �V�F�[�f�B���O�␳�@�G�b�W�O��0��
bool	ShdCmp2_MainC::ExecEdgeMask(int lEdge, int rEdge, const BYTE * pSrc, BYTE * pDst)
{
	int width = mcls_pLPFSAT->my_width;
	mcls_pLPFSAT->ExecEdgeTarget( ToBytePtr(pSrc), ToBytePtr(m_pLPFPic), 0, width -1, lEdge, rEdge, 1, 1, 0 );
	mcls_pDivide->Exec    ( ToBytePtr(pSrc), ToBytePtr(m_pLPFPic), ToBytePtr( pDst ) );
	return true;

}





//static ���\�b�h�Q

//STATIC �V�F�[�f�B���O�␳�@�S��
bool ShdCmp2_MainC::ExecStatic(int imgW, int imgH, const BYTE * pSrc, BYTE * pDst)
{
	ShdCmp2_MainC sc( imgW, imgH, DEF_LPF_WIDTH, DEF_LPF_HEIGHT, DEF_LPF_TARGET );
	return sc.Exec( pSrc, pDst );
}


//STATIC �V�F�[�f�B���O�␳�@�S��@LPF�ETARGET�w���
bool	ShdCmp2_MainC::ExecStatic(int imgW, int imgH, int lpfW, int lpfH, BYTE target, const  BYTE * pSrc,BYTE * pDst)
{
	ShdCmp2_MainC sc( imgW, imgH, lpfW, lpfH, target);
	return sc.Exec( pSrc, pDst );
}


//------------------------------------------
// ������
// �@�N����ɂP��Ăяo���Ă����ƁA
// �@IPP�����s����CPU�ɉ����čœK�ȏ�����I������
//------------------------------------------
int ShdCmp2_MainC::ippStaticInit()
{
	static LONG bIppInited = 0;
	if( InterlockedIncrement( &bIppInited ) == 1 ){
		return ::ippStaticInit();
	}
	return 0;
}
