#pragma once
#ifndef SWDETECT_IPP_MAKESAT_H
#define SWDETECT_IPP_MAKESAT_H

#include "../../../include/LibSelect.h"									// ライブラリ切替ヘッダー
//実装にIPPを使っているので、IPP無では、クラス毎消える。
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

		//LPF実行。エッジ外はlpfがshinkまで小さくなる。
		virtual void ExecEdge(const BYTE* src, BYTE *output, long left_point, long right_point, 
						    	long left_edge, long right_edge, long slope_x, long slope_y);

		//LPF実行。エッジ外は、target値にする
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
		long my_lpf_x;	// ローパスのサイズ
		long my_lpf_y;
		long my_shr_x;	// 何画素とびでSAT->LPF画像を作るか(8単位)
		long my_shr_y;	// yは制約なし
		long my_add_x;	// 両端に何画素付け足しコピーするか
		long my_add_y;
		long my_expand_input_width;	// 拡大入力画像のサイズ
		long my_expand_input_pitch;	// 拡大入力画像のピッチ(16の倍数)
		long my_expand_input_height;
		long my_sat_width;	// SATのサイズ
		long my_sat_height;	
		unsigned char *my_expand_input;	// 拡大された入力画像
		unsigned long *my_sat;	// SAT(縮小している場合がある）
		unsigned long *my_tmpline; // 中間の１行分加算バッファ
		void expand_input(const unsigned char * src, long top, long bkheight, long left, long right);	// partial版
		void make_mean_image(); // サイズ固定版
		BYTE get_mean_val(long out_x, long out_y, long extend_x, long extend_y);
		void make_mean_image(long left, long right, long slope_x, long slope_y); // エッジ付近サイズ可変版
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