#include "StdAfx.h"
#include "FrameDataManager.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
FrameDataManager::FrameDataManager(void)
{
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
FrameDataManager::~FrameDataManager(void)
{
}

//------------------------------------------
// �퐶���� (IportManager��Init���ꂽ���R�[���o�b�N)
// int const* p=0 �󂯓n���J�X�^�����(int �z��)   (0�`1�܂ł́A�Œ�B�p�����Ŏg�p���邩��)
	// [0] �摜�T�C�Y (�{���A[1]*[2]�ł������A�悭�g������n���Ă���)   (�{�V�X�e���́A1�p�x�̂�)
	// [1] �J�����p�x�����Z�b�g���Ă������� (�݊��ׂ̈ɁA0�̏ꍇ�́A�����I��1�ɂ���)
	// [2] 1�t���[�����̃f�[�^�T�C�Y (����؂�鐔)
	// [3] �����݃��[�h (0:���������݃��[�h 1:�Z�N�^�����݃��[�h)
	// [4] �֐��|�C���^(64bit�֐����́A8Byte���g�p)
	// [5] �֐��|�C���^
//------------------------------------------
void FrameDataManager::Alloc(int const* p)
{
	//// ��ԍŏ��Ɍp�����̃G���A���m��&������
	FRAME_DATA::Alloc(p);

	//// �����Ŏg�����̈������o��
	nBufSize		= p[2];
	nWriteMode		= p[3];

	//// �����̃G���A���m��
	_ASSERT( 0 == nBufSize%512);
	pBuf		= (BYTE*)VirtualAlloc(NULL, nBufSize, MEM_COMMIT, PAGE_READWRITE);
}

//------------------------------------------
// ������ (IportManager��Dispose���ꂽ���R�[���o�b�N)
//------------------------------------------
void FrameDataManager::Free()
{
	//// ��ԍŏ��Ɍp�����̃G���A�����
	FRAME_DATA::Free();

	//// �����̃G���A�����
	VirtualFree(pBuf, 0, MEM_RELEASE);
}

//------------------------------------------
// �����o�[������
//------------------------------------------
void FrameDataManager::Clear()
{
	//// ����Z�b�g���Ȃ����̂̂�
	// �X�L�b�v���邽��
	this->cls_Time.PutTimestamp( TIME_ImgComp_START,	(__int64)0);
	this->cls_Time.PutTimestamp( TIME_ImgComp_END, 		(__int64)0);
	this->cls_Time.PutTimestamp( TIME_RpiWrite_START, 	(__int64)0);
	this->cls_Time.PutTimestamp( TIME_RpiWrite_END, 	(__int64)0);


}