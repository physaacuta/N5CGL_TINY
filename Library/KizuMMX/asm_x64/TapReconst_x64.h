#ifndef KIZU_MMX_TAP_RECONST_X64_H
#define KIZU_MMX_TAP_RECONST_X64_H

//namespace KizuMMX
//{
    // dst: width x height�̉摜�̍���A�s�b�`��dst_pitch�o�C�g�B
    // src_lefthalf : �������̉摜(width/2 x height)�̍���A�s�b�`��width/2
    // src_righthalf: �E�����̉摜(width/2 x height)�̍���A�s�b�`��width/2
	extern "C" void __cdecl memcpy_2d_hflip_sse(void* dst, int dst_pitch, const void* src,
			                                    int width, int height);
//};

#endif
