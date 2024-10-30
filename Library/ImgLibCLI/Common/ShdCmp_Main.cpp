// ����� ���C�� DLL �t�@�C���ł��B

#include "ShdCmp_Main.h"
#include <crtdbg.h>

using namespace System;
using namespace System::Runtime::InteropServices;

using namespace ImgLibCLI;

//------------------------------------------
// �R�X�g���N�^
//------------------------------------------
ShadingComp::ShadingComp() :
m_bIsLpf(false),
mcls_pDeshade(NULL),
mcls_pParams(NULL),
m_pImgBuf(IntPtr::Zero)
{

}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
ShadingComp::~ShadingComp(void)
{
	// �����J�����ĂȂ�������J��
	Free();
}


//------------------------------------------
// ���b�N
// long imgW �Ώۉ摜�̕�
// long imgH �Ώۉ摜�̍���
//------------------------------------------
BOOL ShadingComp::Alloc(long imgW, long imgH)
{
	//if(NULL != m_pImgBuf) return false;
	if(0 == m_pImgBuf.Size) return false;
	
	// �̈�m��
	//m_pImgBuf = (BYTE*)_aligned_malloc(imgW * imgH, 32);
	m_pImgBuf = Marshal::AllocHGlobal( imgW * imgH );
	
	// �N���X����
	mcls_pDeshade = new CDeShadeMMX();
	mcls_pParams  = new CDeShadeMMX::Params();

	// �ϐ��ێ�
	m_nImgW = imgW;
	m_nImgH = imgH;

	return TRUE;
}

//------------------------------------------
// �J��
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
// LPF����
// long lpfW LPF�u���b�N��
// long lpfH LPF�u���b�N����
// BYTE taget �V�F�[�����O�␳�̖ڕW�P�x�l
//------------------------------------------
BOOL ShadingComp::CreateLPF(long lpfW, long lpfH, BYTE taget)
{
	// �����I�ɊJ��
	if(m_bIsLpf) {
		mcls_pDeshade->Dispose();
		m_bIsLpf = false;
	}

	// LPF����
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
// �V�F�[�f�B���O�␳
// const BYTE* src �����Ώۂ̉摜�f�[�^
// BYTE* dst �V�F�[�f�B���O�␳��̉摜�f�[�^
//------------------------------------------
BOOL ShadingComp::Exec(array<BYTE>^ pSrc, array<BYTE>^ pDst)
{
	// �����f�[�^�Z�b�g
	Marshal::Copy(pSrc, 0, (IntPtr)m_pImgBuf, m_nImgW * m_nImgH);

	// ���������V�F�[�f�B���O�␳���āA�������ɓ����B
	//mcls_pDeshade->Exec((const BYTE*)m_pImgBuf, (BYTE*)m_pImgBuf);
	mcls_pDeshade->Exec((const BYTE*)m_pImgBuf.ToPointer(), (BYTE*)m_pImgBuf.ToPointer());
	
	// ���ʃf�[�^�Z�b�g
	Marshal::Copy((IntPtr)m_pImgBuf, pDst, 0, m_nImgW * m_nImgH);
	return TRUE;
}
//------------------------------------------
// �V�F�[�f�B���O�␳(�G�b�W�O������)
// const BYTE* src �����Ώۂ̉摜�f�[�^
// BYTE* dst �V�F�[�f�B���O�␳��̉摜�f�[�^
// long lEdge ���G�b�W�ʒu (0�I���W��) [pixel]
// long rEdge �E�G�b�W�ʒu (0�I���W��) [pixel]
//------------------------------------------
BOOL ShadingComp::ExecEdgeMask(array<BYTE>^ pSrc, array<BYTE>^ pDst, long lEdge, long rEdge)
{
	// �����f�[�^�Z�b�g
	Marshal::Copy(pSrc, 0, (IntPtr)m_pImgBuf, m_nImgW * m_nImgH);

	// ���������V�F�[�f�B���O�␳���āA�������ɓ����B
	mcls_pDeshade->ExecEdgeMask(lEdge, rEdge, (const BYTE*)m_pImgBuf.ToPointer(), (BYTE*)m_pImgBuf.ToPointer());
	
	// ���ʃf�[�^�Z�b�g
	Marshal::Copy((IntPtr)m_pImgBuf, pDst, 0, m_nImgW * m_nImgH);
	return TRUE;
}
//------------------------------------------
// �V�F�[�f�B���O�␳(�G�b�W�O�͌��摜)
// const BYTE* src �����Ώۂ̉摜�f�[�^
// BYTE* dst �V�F�[�f�B���O�␳��̉摜�f�[�^
// long lEdge ���G�b�W�ʒu (0�I���W��) [pixel]
// long rEdge �E�G�b�W�ʒu (0�I���W��) [pixel]
//------------------------------------------
BOOL ShadingComp::ExecEdgeCopy(array<BYTE>^ pSrc, array<BYTE>^ pDst, long lEdge, long rEdge)
{
	// �����f�[�^�Z�b�g
	Marshal::Copy(pSrc, 0, (IntPtr)m_pImgBuf, m_nImgW * m_nImgH);

	// ���������V�F�[�f�B���O�␳���āA�������ɓ����B
	mcls_pDeshade->ExecEdgeCopy(lEdge, rEdge, (const BYTE*)m_pImgBuf.ToPointer(), (BYTE*)m_pImgBuf.ToPointer());
	
	// ���ʃf�[�^�Z�b�g
	Marshal::Copy((IntPtr)m_pImgBuf, pDst, 0, m_nImgW * m_nImgH);
	return TRUE;
}



// /////////////////////////////////////////////////////////////////////////////////
// �X�^�e�B�b�N�p
// /////////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// �V�F�[�f�B���O�␳
// long imgW �Ώۉ摜�̕�
// long imgH �Ώۉ摜�̍���
// const BYTE* src �����Ώۂ̉摜�f�[�^
// BYTE* dst �V�F�[�f�B���O�␳��̉摜�f�[�^
//------------------------------------------
BOOL ShadingComp::ExecStatic(long imgW, long imgH, array<BYTE>^ pSrc, array<BYTE>^ pDst)
{
	return ExecStatic(imgW, imgH, DEF_LPF_WIDTH, DEF_LPF_HEIGHT, DEF_LPF_TARGET, pSrc, pDst);
}

//------------------------------------------
// �V�F�[�f�B���O�␳
// long imgW �Ώۉ摜�̕�
// long imgH �Ώۉ摜�̍���
// long lpfW LPF�u���b�N��
// long lpfH LPF�u���b�N����
// BYTE taget �V�F�[�����O�␳�̖ڕW�P�x�l
// const BYTE* src �����Ώۂ̉摜�f�[�^
// BYTE* dst �V�F�[�f�B���O�␳��̉摜�f�[�^
//------------------------------------------
BOOL ShadingComp::ExecStatic(long imgW, long imgH, long lpfW, long lpfH, BYTE taget, array<BYTE>^ pSrc, array<BYTE>^ pDst)
{
	bool bRetc = FALSE;
	CDeShadeMMX deshade;
	CDeShadeMMX::Params params;

	// �����`�F�b�N
	_ASSERT(0 == lpfW%8);
	_ASSERT(0 == lpfH%8);


	// �������������b�N
//	BYTE* p = (BYTE*)_aligned_malloc(imgW * imgH, 32);
	IntPtr p = Marshal::AllocHGlobal( imgW * imgH );
	Marshal::Copy(pSrc, 0, (IntPtr)p, imgW * imgH);

	// �V�F�[�f�B���O�␳�̃p�����[�^�B�ȉ��̒l���퓅�B
	params.lpf_width = lpfW;
	params.lpf_height = lpfH;
	params.target = taget;
	if( ! deshade.Init(imgW, imgH, params) ) goto Ending;

	// ���������V�F�[�f�B���O�␳���āA�������ɓ����B
//	deshade.Exec((const BYTE*)p, (BYTE*)p);
	deshade.Exec((const BYTE*)p.ToPointer(), (BYTE*)p.ToPointer());
	bRetc = TRUE;

	// ���ʃf�[�^�Z�b�g
//	Marshal::Copy(p, pDst, 0, w * h);
	Marshal::Copy((IntPtr)p, pDst, 0, imgW * imgH);

Ending:
	// �����������J��
//	_aligned_free(p);
	Marshal::FreeHGlobal(p);
	return bRetc;
}


