#ifndef KIZU_MMX_MEMCOPY_X64_H
#define KIZU_MMX_MEMCOPY_X64_H

//namespace KizuMMX
//{
	// 8�o�C�g�P�ʂ̃R�s�[
	extern "C" void __cdecl mem_copy_8(
		const unsigned char* src,
		unsigned long src_pitch,
		unsigned char* dst,
		unsigned long dst_pitch,
		unsigned long width,
		unsigned long height);

	// 32�o�C�g�P�ʂ̃R�s�[
	extern "C" void __cdecl mem_copy_32(
		const unsigned char* src,
		unsigned long src_pitch,
		unsigned char* dst,
		unsigned long dst_pitch,
		unsigned long width,
		unsigned long height);

	// 64�o�C�g�P�ʂ̃R�s�[
	extern "C" void __cdecl mem_copy_64(
		const unsigned char* src,
		unsigned long src_pitch,
		unsigned char* dst,
		unsigned long dst_pitch,
		unsigned long width,
		unsigned long height);

	// SSE2���߂�p����16�o�C�g�P�ʂ̃R�s�[
	extern "C" void __cdecl mem_copy_16xmm(
		const unsigned char* src,		// must be aligned on 16byte boundary
		unsigned long src_pitch,
		unsigned char* dst,
		unsigned long dst_pitch,
		unsigned long width,
		unsigned long height);

	// SSE2���߂�p����64�o�C�g�P�ʂ̃������N���A
	extern "C" void __cdecl mem_clear_64xmm(
		void* dst,						// must be aligned on 16byte boundary
		unsigned long size);

	// SSE2���߂�p����64�o�C�g�P�ʂ̃������Z�b�g
	extern "C" void __cdecl mem_set_64xmm_short(
		unsigned short* dst,
		unsigned short val,
		unsigned long sz);

	// SSE2���߂�p����64�o�C�g�P�ʂ̃R�s�[
	extern "C" void __cdecl mem_copy_x64_pitch(
		const unsigned char* src,		// must be aligned on 16byte boundary
		unsigned long src_pitch,
		unsigned char* dst,
		unsigned long dst_pitch,
		unsigned long width,
		unsigned long height);

	// �u���b�N�P�ʂ�SSE2���߂�p����64�o�C�g�P�ʂ̃R�s�[
	extern "C" void __cdecl mem_copy_x64_block(
		void *dst,
		const void *src,
		size_t bksize);

	// non-temporal(�L���b�V��������Ȃ��j�������݂�p�����������R�s�[
	extern "C" void __cdecl mem_copy_nt(
		void *dst,
		const void *src,
		size_t bksize);

//};

#endif
