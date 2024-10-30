#pragma once

void __stdcall rgbplanar_colortwist_asm(BYTE** pdst, size_t width, size_t dst_pitch,
		const BYTE*const* psrc,  size_t src_pitch, size_t height, const float mat[12]);

void __stdcall rgbplanar_colortwist_asm(BYTE** pdst, size_t width, size_t dst_pitch,
		const BYTE* psrc,  size_t src_pitch, size_t height, const float mat[12]);



//下は64bitのASM（内部用）。
extern "C" {
void rgbplanar_colortwist_asm_sse(BYTE** pdst, size_t width, size_t dst_pitch,
									  const BYTE* psrc,  size_t src_pitch, size_t height,
									  const short* c_r, const short* c_g, const short* c_b);

void rgbplanar_colortwist_asm_avx(BYTE** pdst, size_t width, size_t dst_pitch,
									  const BYTE* psrc,  size_t src_pitch, size_t height,
									  const short* c_r, const short* c_g, const short* c_b);

void rgbplanar_colortwist_asm_sse_2tap(BYTE** pdst, size_t width, size_t dst_pitch,
									  const BYTE*const* psrc,  size_t src_pitch, size_t height,
									  const short* c_r, const short* c_g, const short* c_b);
void rgbplanar_colortwist_asm_avx_2tap(BYTE** pdst, size_t width, size_t dst_pitch,
									  const BYTE*const* psrc,  size_t src_pitch, size_t height,
									  const short* c_r, const short* c_g, const short* c_b);
};

