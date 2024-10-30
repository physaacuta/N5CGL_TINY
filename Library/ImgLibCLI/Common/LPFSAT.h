#pragma once
#ifndef SWDETECT_IPP_MAKESAT_H
#define SWDETECT_IPP_MAKESAT_H

#include "../../../include/LibSelect.h"									// ���C�u�����ؑփw�b�_�[
//������IPP���g���Ă���̂ŁAIPP���ł́A�N���X��������B
#ifdef DEFECTLIB_KIZUJPEGMANAGER

#include <Windows.h>
#include <crtdbg.h>

#ifdef _IPP_VERSION_7
#include <ippi.h>
#endif


namespace ImgLibCLI
{
    class CLPFSAT
   {
    public:
        CLPFSAT();
        virtual ~CLPFSAT();
		virtual void Dispose();

        virtual BOOL Init(long width, long src_pitch, long dst_pitch, long height, long lpf_x, long lpf_y, long shrink_x, long shrink_y);


        virtual void Exec(const BYTE* src, BYTE *output);
		virtual void ExecLimiter(const BYTE* src, BYTE *output, BYTE lower, BYTE upper);
		virtual void ExecCenter(const BYTE* src, BYTE *output, long left_point, long right_point);

		//LPF���s�B�G�b�W�O��lpf��shink�܂ŏ������Ȃ�B
		virtual void ExecEdge(const BYTE* src, BYTE *output, long left_point, long right_point, 
						    	long left_edge, long right_edge, long slope_x, long slope_y);

		//LPF���s�B�G�b�W�O�́Atarget�l�ɂ���
		virtual void ExecEdgeTarget(const BYTE* src, BYTE *output, long left_point, long right_point, 
						    	long left_edge, long right_edge, long slope_x, long slope_y, BYTE target);
		long my_width;
        long my_height;

   protected:
        virtual BOOL Set(long lpf_x, long lpf_y, long shrink_x, long shrink_y);
		virtual BOOL AllocateBuffer(long width, long src_pitch, long dst_pitch, long height, long lpf_x, long lpf_y, long shrink_x, long shrink_y);
		virtual void MakeShrinkLimiter(const unsigned char* src, unsigned char lower, unsigned char upper);
		virtual void MakeShrinkLimiter(const unsigned char* src, unsigned char lower, unsigned char upper, long left, long right);
		virtual void GetOutputPartial(unsigned char* output, long top, long bkheight);

		
		long my_src_pitch;
		long my_dst_pitch;
		long my_lpf_x;	// ���[�p�X�̃T�C�Y
		long my_lpf_y;
		long my_shr_x;	// ����f�Ƃт�SAT->LPF�摜����邩(8�P��)
		long my_shr_y;	// y�͐���Ȃ�
		long my_add_x;	// ���[�ɉ���f�t�������R�s�[���邩
		long my_add_y;
		long my_expand_input_width;	// �g����͉摜�̃T�C�Y
		long my_expand_input_pitch;	// �g����͉摜�̃s�b�`(16�̔{��)
		long my_expand_input_height;
		long my_sat_width;	// SAT�̃T�C�Y
		long my_sat_height;	
		unsigned char *my_expand_input;	// �g�傳�ꂽ���͉摜
		unsigned long *my_sat;	// SAT(�k�����Ă���ꍇ������j
		unsigned long *my_tmpline; // ���Ԃ̂P�s�����Z�o�b�t�@
		void expand_input(const unsigned char * src, long top, long bkheight, long left, long right);	// partial��
		void make_mean_image(); // �T�C�Y�Œ��
		BYTE get_mean_val(long out_x, long out_y, long extend_x, long extend_y);
		void make_mean_image(long left, long right, long slope_x, long slope_y); // �G�b�W�t�߃T�C�Y�ϔ�
		void expand_output(unsigned char * output);
		void expand_output_partial(unsigned char * output, long top, long bkheight);

#ifdef _IPP_VERSION_7
		Ipp8u*	my_ipp_resize_buf;
		IppiResizeSpec_32f*	my_ipp_resize_spec;
#endif
		
	};
};

#endif
#endif