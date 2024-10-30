// *********************************************************************************
//	メモリコピー 関数
//	[Ver]
//		Ver.01    2012/05/07  初版
//
//	[メモ]
//		・
//
//
// *********************************************************************************
#ifndef KIZU_MMX_MEMCOPY_H
#define KIZU_MMX_MEMCOPY_H

namespace KizuMMX
{
	// 8バイト単位のコピー
    void __stdcall mem_copy_8(
		const unsigned char* src,
		unsigned long src_pitch,
		unsigned char* dst,
		unsigned long dst_pitch,
		unsigned long width,
		unsigned long height);

	// 32バイト単位のコピー
    void __stdcall mem_copy_32(
		const unsigned char* src,
		unsigned long src_pitch,
		unsigned char* dst,
		unsigned long dst_pitch,
		unsigned long width,
		unsigned long height);

	// 64バイト単位のコピー
    void __stdcall mem_copy_64(
		const unsigned char* src,
		unsigned long src_pitch,
		unsigned char* dst,
		unsigned long dst_pitch,
		unsigned long width,
		unsigned long height);

	// SSE2命令を用いた16バイト単位のコピー
    void __stdcall mem_copy_16xmm(
        const unsigned char* src,		// must be aligned on 16byte boundary
        unsigned long src_pitch,
        unsigned char* dst,
        unsigned long dst_pitch,
        unsigned long width,
        unsigned long height);

	// SSE2命令を用いた64バイト単位のコピー
    void __stdcall mem_copy_64xmm(
        const unsigned char* src,		// must be aligned on 16byte boundary
        unsigned long src_pitch,
        unsigned char* dst,
        unsigned long dst_pitch,
        unsigned long width,
        unsigned long height);

	// SSE2命令を用いた64バイト単位のメモリクリア
	void __stdcall mem_clear_64xmm(
		void* dst,						// must be aligned on 16byte boundary
        unsigned long size);

	// SSE2命令を用いた64バイト単位のメモリセット
	void __stdcall mem_set_64xmm_short(
		unsigned short* dst,			// must be aligned on 16byte boundary
        unsigned short val,
		unsigned long size);

	// ブロック単位のSSE2命令を用いた64バイト単位のコピー
	void __stdcall mem_copy_64xmm(
		void *dst,
		const void *src,
		size_t bksize);

	// non-temporal(キャッシュを消費しない）書き込みを用いたメモリコピー
	void __stdcall mem_copy_nt(
		void *dst,
		const void *src,
		size_t bksize);
};

#endif
