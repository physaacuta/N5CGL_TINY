#include <crtdbg.h>
#include <malloc.h>

#include "ShdCmp_MainC.h"

using namespace ImgLibCLI;

//------------------------------------------
// �R�X�g���N�^
//------------------------------------------
ShdCmp_MainC::ShdCmp_MainC(void) :
m_bIsLpf(false),
mcls_pDeshade(NULL),
mcls_pParams(NULL)
{
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
ShdCmp_MainC::~ShdCmp_MainC(void)
{
	// �����J�����ĂȂ�������J��
	Free();
}

//------------------------------------------
// ���b�N
//------------------------------------------
BOOL ShdCmp_MainC::Alloc()
{
	Free();

	// �N���X����
	mcls_pDeshade = new CDeShadeMMX();
	mcls_pParams  = new CDeShadeMMX::Params();
	return TRUE;
}

//------------------------------------------
// �J��
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
// �摜�T�C�Y�ς�LPF����
// long imgW �Ώۉ摜�̕�   (imgMaxW��菬��������)
// long imgH �Ώۉ摜�̍��� (imgMaxW��菬��������)
// long lpfW LPF�u���b�N��
// long lpfH LPF�u���b�N����
// BYTE taget �V�F�[�����O�␳�̖ڕW�P�x�l
//------------------------------------------
BOOL ShdCmp_MainC::CreateLPF(long imgW, long imgH, long lpfW, long lpfH, BYTE taget)
{
	// �摜�T�C�Y�ύX
	m_nImgW = imgW;
	m_nImgH = imgH;

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
	return mcls_pDeshade->Init(imgW, imgH, (CDeShadeMMX::Params const)*mcls_pParams);
}

//------------------------------------------
// �V�F�[�f�B���O�␳
// const BYTE* src �����Ώۂ̉摜�f�[�^
// BYTE* dst �V�F�[�f�B���O�␳��̉摜�f�[�^
//------------------------------------------
BOOL ShdCmp_MainC::Exec(const BYTE* pSrc, BYTE* pDst)
{
	// ���������V�F�[�f�B���O�␳���āA�������ɓ����B
	mcls_pDeshade->Exec(pSrc, pDst);
	return TRUE;
}

//------------------------------------------
// �V�F�[�f�B���O�␳(�G�b�W�O��������)
// long nPosL	���摜��_����̍��G�b�W�ʒu [pixel]
// long nPosR	���摜��_����̉E�G�b�W�ʒu [pixel]
// const BYTE* src �����Ώۂ̉摜�f�[�^
// BYTE* dst �V�F�[�f�B���O�␳��̉摜�f�[�^
//------------------------------------------
BOOL ShdCmp_MainC::ExecEdgeMask(long nPosL, long nPosR, const BYTE* pSrc, BYTE* pDst) {
	// ���������V�F�[�f�B���O�␳���āA�������ɓ����B
	mcls_pDeshade->ExecEdgeMask(nPosL, nPosR, pSrc, pDst);
	return 0;
}

//------------------------------------------
// �V�F�[�f�B���O�␳(�G�b�W�O���R�s�[)
// long nPosL	���摜��_����̍��G�b�W�ʒu [pixel]
// long nPosR	���摜��_����̉E�G�b�W�ʒu [pixel]
// const BYTE* src �����Ώۂ̉摜�f�[�^
// BYTE* dst �V�F�[�f�B���O�␳��̉摜�f�[�^
//------------------------------------------
BOOL ShdCmp_MainC::ExecEdgeCopy(long nPosL, long nPosR, const BYTE* pSrc, BYTE* pDst) {
	// ���������V�F�[�f�B���O�␳���āA�������ɓ����B
	mcls_pDeshade->ExecEdgeCopy(nPosL, nPosR, pSrc, pDst);
	return 0;
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
BOOL ShdCmp_MainC::ExecStatic(long imgW, long imgH, const BYTE* pSrc, BYTE* pDst)
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
BOOL ShdCmp_MainC::ExecStatic(long imgW, long imgH, long lpfW, long lpfH, BYTE taget, const BYTE* pSrc, BYTE* pDst)
{
	bool bRetc = FALSE;
	CDeShadeMMX deshade;
	CDeShadeMMX::Params params;

	// �����`�F�b�N
	_ASSERT(0 == lpfW%8);
	_ASSERT(0 == lpfH%8);


	// �V�F�[�f�B���O�␳�̃p�����[�^�B�ȉ��̒l���퓅�B
	params.lpf_width = lpfW;
	params.lpf_height = lpfH;
	params.target = taget;
	if( ! deshade.Init(imgW, imgH, params) ) goto Ending;

	// ���������V�F�[�f�B���O�␳���āA�������ɓ����B
	deshade.Exec((const BYTE*)pSrc, (BYTE*)pDst);
	bRetc = TRUE;

Ending:
	deshade.Dispose();
	return bRetc;
}
