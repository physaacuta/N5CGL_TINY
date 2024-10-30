// *********************************************************************************
//	�������R�s�[ �֐�
//	[Ver]
//		Ver.01    2012/05/07  ����
//
//	[����]
//		�E
//
//
// *********************************************************************************
#ifndef KIZU_MMX_MEMCOPY_H
#define KIZU_MMX_MEMCOPY_H

namespace KizuMMX
{
	// 8�o�C�g�P�ʂ̃R�s�[
    void __stdcall mem_copy_8(
		const unsigned char* src,
		unsigned long src_pitch,
		unsigned char* dst,
		unsigned long dst_pitch,
		unsigned long width,
		unsigned long height);

	// 32�o�C�g�P�ʂ̃R�s�[
    void __stdcall mem_copy_32(
		const unsigned char* src,
		unsigned long src_pitch,
		unsigned char* dst,
		unsigned long dst_pitch,
		unsigned long width,
		unsigned long height);

	// 64�o�C�g�P�ʂ̃R�s�[
    void __stdcall mem_copy_64(
		const unsigned char* src,
		unsigned long src_pitch,
		unsigned char* dst,
		unsigned long dst_pitch,
		unsigned long width,
		unsigned long height);

	// SSE2���߂�p����16�o�C�g�P�ʂ̃R�s�[
    void __stdcall mem_copy_16xmm(
        const unsigned char* src,		// must be aligned on 16byte boundary
        unsigned long src_pitch,
        unsigned char* dst,
        unsigned long dst_pitch,
        unsigned long width,
        unsigned long height);

	// SSE2���߂�p����64�o�C�g�P�ʂ̃R�s�[
    void __stdcall mem_copy_64xmm(
        const unsigned char* src,		// must be aligned on 16byte boundary
        unsigned long src_pitch,
        unsigned char* dst,
        unsigned long dst_pitch,
        unsigned long width,
        unsigned long height);

	// SSE2���߂�p����64�o�C�g�P�ʂ̃������N���A
	void __stdcall mem_clear_64xmm(
		void* dst,						// must be aligned on 16byte boundary
        unsigned long size);

	// SSE2���߂�p����64�o�C�g�P�ʂ̃������Z�b�g
	void __stdcall mem_set_64xmm_short(
		unsigned short* dst,			// must be aligned on 16byte boundary
        unsigned short val,
		unsigned long size);

	// �u���b�N�P�ʂ�SSE2���߂�p����64�o�C�g�P�ʂ̃R�s�[
	void __stdcall mem_copy_64xmm(
		void *dst,
		const void *src,
		size_t bksize);

	// non-temporal(�L���b�V��������Ȃ��j�������݂�p�����������R�s�[
	void __stdcall mem_copy_nt(
		void *dst,
		const void *src,
		size_t bksize);
};

#endif
