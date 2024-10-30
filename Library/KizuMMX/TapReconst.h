#ifndef KIZU_MMX_TAP_RECONST_H
#define KIZU_MMX_TAP_RECONST_H

namespace KizuMMX
{

    // dst: width x height�̉摜�̍���A�s�b�`��dst_pitch�o�C�g�B
    // src_lefthalf : �������̉摜(width/2 x height)�̍���A�s�b�`��width/2
    // src_righthalf: �E�����̉摜(width/2 x height)�̍���A�s�b�`��width/2
    void __stdcall hflip_righthalf(
		const unsigned char* src_lefthalf,
		const unsigned char* src_righthalf,
		unsigned char* dst,
		long dst_pitch,
		long width, long height);

};

#endif
