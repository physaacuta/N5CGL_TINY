#include <crtdbg.h>
#include <malloc.h>

#include "LPFSAT.h"
#include "MakeSATasm.h"

//実装にIPPを使っているので、IPP無では、クラス毎消える。
#ifdef DEFECTLIB_KIZUJPEGMANAGER
#include <ippi.h>
#include <ipps.h>

/*
 *　・画像に上下左右ののりしろを付与（それぞれLPF/2だけ伸びる）
 *　・shrink_x, shrink_y 単位に分割し、合計値を出す => SAT表
 *  ・SAT表を使って、LPF_X/2、LPF_Y/2毎平均しつつ、LPF小画像を作成する、
 *  ・LPF小画像を、LPF大画像(元画像と同じ大きさ)にIPPで拡大。
 *　・LPF大画像を返します。
 *
 * 注意：SAT表について。
 * SAT表は、画像をshrink_x・shrink_yで区切って、エリア内の輝度合計を入れた二次元配列。
 * ただし、あとでLPF平均を高速化するため、「それまで合計輝度　」が記載されている。
 * 
 *                   Sx*w  Sy*h
 *　　SAT( Sx, Sy ) = Σ   Σ     pic(x,y)
 *                    x=0  y=0
 *  なので、SATの右下隅は、全画素の輝度合計になる。
 *
 *  A-----B  ある点pの前後左右LPF/2の平均をとりたければ、
 *  |     |  その四角形の４点のSAT値(A,B,C,D)について、
 *  |  p  |  (D-C) - (B-A) で四角形内の輝度合計が出る。
 *  |     |  あとは面積で割れば平均輝度となる。
 *  C-----D 
  */


#define _IPP_MAKESAT_USE_PARTIAL
static void save_as_bmp(const char *file,int w, int h, const BYTE *body);


namespace ImgLibCLI
{


CLPFSAT::CLPFSAT()
	: my_width(0)
	, my_height(0)
	, my_src_pitch(0)
	, my_expand_input(NULL)
	, my_sat(NULL)
	, my_tmpline(NULL)
#ifdef _IPP_VERSION_7
	, my_ipp_resize_buf(NULL)
	, my_ipp_resize_spec(NULL)
#endif
{
}

CLPFSAT::~CLPFSAT()
{
	Dispose();
}

void CLPFSAT::Dispose()
{
    my_width = 0;
    my_height = 0;


#ifdef _IPP_VERSION_7
	if (NULL != my_ipp_resize_spec) {
		ippsFree(my_ipp_resize_spec);
		my_ipp_resize_spec = NULL;
	}
	if (NULL != my_ipp_resize_buf) {
		ippsFree(my_ipp_resize_buf);
		my_ipp_resize_buf = NULL;
	}
#endif

	_aligned_free(my_expand_input);
	my_expand_input = 0;
	_aligned_free(my_sat);
	my_sat = 0;
	_aligned_free(my_tmpline);
	my_tmpline = 0;
}

BOOL CLPFSAT::Init(long width, long src_pitch, long dst_pitch, long height, long lpf_x, long lpf_y, long shrink_x, long shrink_y)
{
	_ASSERT(NULL == my_expand_input);
	_ASSERT(NULL == my_sat);
	_ASSERT(NULL == my_tmpline);
	_ASSERT(lpf_x >= shrink_x);
	_ASSERT(lpf_y >= shrink_y);
	my_src_pitch = src_pitch;
	my_dst_pitch = dst_pitch;
    my_width = width;
    my_height = height;

    return Set(lpf_x, lpf_y, shrink_x, shrink_y) 
		&&  AllocateBuffer(width, src_pitch, dst_pitch, height, lpf_x, lpf_y, shrink_x, shrink_y )
		;
}

BOOL CLPFSAT::Set(long lpf_x, long lpf_y, long shrink_x, long shrink_y)
{
	_ASSERT(my_width);
	_ASSERT(my_height);

	my_lpf_x = (lpf_x / (2*shrink_x)) * 2*shrink_x + shrink_x; // shrink_xの奇数倍とする→両端に付加するサイズが等しくなる
	my_lpf_y = (lpf_y / (2*shrink_y)) * 2*shrink_y + shrink_y; // shrink_yの奇数倍とする→両端に付加するサイズが等しくなる
	my_shr_x = shrink_x;
	my_shr_y = shrink_y;

	my_add_x = my_lpf_x/2;	// 両端に付加するサイズ
	my_add_y = my_lpf_y/2;	// 両端に付加するサイズ
	my_expand_input_width = my_width + 2*my_add_x;
	my_expand_input_height = my_height + 2*my_add_y;
	my_expand_input_pitch = 32*(((my_expand_input_width-1) / 32)+1);
	my_sat_width = my_expand_input_width / my_shr_x;
	my_sat_height = my_expand_input_height / my_shr_y;

#ifdef _IPP_VERSION_7
	if (NULL != my_ipp_resize_buf) {
		ippsFree(my_ipp_resize_buf);
	}
	if (NULL != my_ipp_resize_spec) {
		ippsFree(my_ipp_resize_spec);
	}
	int shrink_width = my_width / my_shr_x;
	int shrink_height = my_height / my_shr_y;
	IppiSize src_sz = {shrink_width, shrink_height};
	IppiSize dst_sz = {my_width, my_height};
	int spec_size, init_size, buf_size;
	ippiResizeGetSize_8u(src_sz, dst_sz, ippLinear, 0, &spec_size, &init_size);
	my_ipp_resize_spec = (IppiResizeSpec_32f*)ippsMalloc_8u(spec_size);
	ippiResizeLinearInit_8u(src_sz, dst_sz, my_ipp_resize_spec);
	ippiResizeGetBufferSize_8u(my_ipp_resize_spec, dst_sz, 1, &buf_size);
	my_ipp_resize_buf = ippsMalloc_8u(buf_size);
#endif

	return TRUE;
}

BOOL CLPFSAT::AllocateBuffer(long width, long src_pitch, long dst_pitch, long height, long lpf_x, long lpf_y, long shrink_x, long shrink_y)
{
	_ASSERT(width);
	_ASSERT(height);

	int sat_size = sizeof(long)*my_sat_width*my_sat_height;
	int exp_input_size = __max(sat_size, my_expand_input_pitch * shrink_y); // make_mean_imageでmean_imageの置き場になるため

	int tmpline_size = sizeof(long)*my_expand_input_pitch;

	my_expand_input = (unsigned char *)_aligned_malloc(exp_input_size, 16);
	my_sat = (unsigned long *)_aligned_malloc(sat_size, 4);
	my_tmpline = (unsigned long *)_aligned_malloc(tmpline_size, 16);	
		
	return TRUE;
}



// partial版
// src：元画像
// Top、bkheight：元画像のうち、使用する領域（高さ）
// Left、Right：有効幅。これ以外のエリアは、 Left～+16、Right～-16pxの平均値で埋める。
//出力：my_expand_input。LPF計算用の拡大・元画像エリア。画像の領域は、両端でy_add_x伸びる。
void CLPFSAT::expand_input(const unsigned char * src, long top, long bkheight, long left, long right)
{
	_ASSERT(left >= 0);
	_ASSERT(right < my_width);
	_ASSERT(right >= left);

	int valid_width = right - left + 1;
	int _left = __min(left, my_width-1-my_add_x);
	int _right = __max(right, my_add_x);
	int dv = 0;	// dst vertical position
	for (int v = top; v < top+bkheight; v++,dv++) {	// v: virtual expand image vertical position


		// 真中にコピーと左右付け足し(直線)
		int sv = v-my_add_y;	// src vertical position
		if (v < my_add_y) {
			sv = my_add_y-1-v;
		}
		if (v >= my_height + my_add_y) {
			sv = 2*my_height+my_add_y-1-v;
		}
		const BYTE *pSrc = &src[ sv*my_src_pitch];
		BYTE *pDst = &my_expand_input[ dv * my_expand_input_pitch ];

		// 真ん中コピー
		memcpy(&pDst[my_add_x+left], &pSrc[left], valid_width);
#if 0
		// 左直線
		float slope = ((float)*(src+sv*my_src_pitch+_left+my_add_x)
			- (float)*(src+sv*my_src_pitch+_left))/my_add_x;
		float offset = (float)*(src+sv*my_src_pitch+_left) - slope*(my_add_x+_left);
		ippsVectorSlope_8u(my_expand_input+dv*my_expand_input_pitch, my_add_x+left, offset, slope);
		// 右直線
		slope = ((float)*(src+sv*my_src_pitch+_right)
			- (float)*(src+sv*my_src_pitch+_right-my_add_x))/my_add_x;
		offset = (float)*(src+sv*my_src_pitch+_right);
		ippsVectorSlope_8u(my_expand_input+dv*my_expand_input_pitch+my_add_x+_right+1,
			my_add_x+(my_width-1-_right), offset, slope);
#else
//		ippsFlip_8u(src+sv*my_src_pitch+left, my_expand_input+dv*my_expand_input_pitch, my_add_x+left);	// 試しにミラー
//		ippsFlip_8u(src+sv*my_src_pitch+right-(my_add_x+(my_width-1-right))-1, my_expand_input+dv*my_expand_input_pitch+my_add_x+right+1,
//			my_add_x+(my_width-1-right));
		// 単純に延長
		const int avg_n = 16;
		int left_val = pSrc[left];
		if (left < my_width-avg_n) {
			for (int i = 0; i < avg_n-1; i++) {
				left_val += pSrc[left+i+1];
			}
			left_val /= avg_n;
		}
		memset(&pDst[0], left_val, my_add_x+left);

		int right_val = pSrc[right];
		if (right >= avg_n) {
			for (int i = 1; i < avg_n; i++) {
				right_val += pSrc[right-i];
			}
			right_val /= avg_n;
		}
		memset(&pDst[my_add_x+right+1],
			right_val, my_add_x+(my_width-1-right));
#endif

	}
}



// 縮小画像（平均画素法）を作成
// サイズ固定版（元画像に対して、LPF_X, LPF_Yでの領域での平均）
void CLPFSAT::make_mean_image()
{
	// 平均画像作成
	// 左端と1番上の行は特別扱い
	int horz_offset = my_lpf_x / my_shr_x;
	int vert_offset = my_lpf_y / my_shr_y;
	int shrink_width = my_width / my_shr_x;
	int shrink_height = my_height / my_shr_y;
	DWORD div_factor = my_lpf_x * my_lpf_y;
	my_expand_input[0] = (BYTE)(my_sat[(vert_offset-1)*my_sat_width + (horz_offset-1)] / div_factor);
	for (int i = 1; i < shrink_width; i++) {
		DWORD val = my_sat[(vert_offset-1)*my_sat_width + (horz_offset-1+i)];
		val -= my_sat[(vert_offset-1)*my_sat_width + (-1+i)];
		my_expand_input[i] = (BYTE)(val / div_factor);
	}

	for (int i = 1; i < shrink_height; i++) {
		DWORD val = my_sat[(vert_offset-1+i)*my_sat_width + (horz_offset-1)];
		val -= my_sat[(-1+i)*my_sat_width + (horz_offset-1)];
		my_expand_input[i*shrink_width] = (BYTE)(val / div_factor);
		for (int j = 1; j < shrink_width; j++) {
			val = my_sat[(vert_offset-1+i)*my_sat_width + (horz_offset-1+j)];
			val += my_sat[(-1+i)*my_sat_width + (-1+j)];
			val -= my_sat[(vert_offset-1+i)*my_sat_width + (-1+j)];
			val -= my_sat[(-1+i)*my_sat_width + (horz_offset-1+j)];
			my_expand_input[i*shrink_width+j] = (BYTE)(val / div_factor);
		}
	}
}

// 画素の平均を求める関数
// out_x, out_y:SHR座標（内）系での、中心座標
// extend_x, extend_y : 平均をとるサイズ。左右への拡大量(0,1,2....)
BYTE CLPFSAT::get_mean_val(long out_x, long out_y, long extend_x, long extend_y)
{
	_ASSERT(out_x >= 0);
	_ASSERT(out_x < (my_width / my_shr_x));
	_ASSERT(out_y >= 0);
	_ASSERT(out_y < (my_height / my_shr_y));
	int add_x = my_add_x / my_shr_x;	// 縮小単位での横付加量
	int add_y = my_add_y / my_shr_y;	// 縮小単位での縦付加量

	int left = __max(out_x + add_x - extend_x - 1, -1);
	int right = __min(out_x + add_x + extend_x, my_sat_width - 1);
	int top = __max(out_y + add_y - extend_y - 1, -1);
	int bottom = __min(out_y + add_y + extend_y, my_sat_height - 1);
	DWORD div_factor = my_shr_x * (right - left) * my_shr_y * (bottom - top);
	DWORD val = my_sat[bottom*my_sat_width + right];
	if (-1 < left) 	val -= my_sat[bottom*my_sat_width + left];
	if (-1 < top) val -= my_sat[top*my_sat_width + right];
	if (-1 < top && -1 < left) val += my_sat[top*my_sat_width + left];
	return (BYTE)(val / div_factor);
}


// 縮小画像（平均画素法）を作成
// エッジ付近が倍率可変になる版
// 入力： my_sat          : SAT行列
// 出力： my_expand_input : 1/SHR LPF画像
void CLPFSAT::make_mean_image(long left, long right, long slope_x, long slope_y)
{
	_ASSERT(left <= right);
	
	//SHR座標（内）系の、左右エッジ部分
	int shrink_left = (left + (my_shr_x>>1)) / my_shr_x;		
	int shrink_right = (right + (my_shr_x>>1)) / my_shr_x;
	//SHR座標（内）系の、画像の幅・高
	int shrink_width = my_width / my_shr_x;
	int shrink_height = my_height / my_shr_y;
	//SHR座標（内）系と、SHR座標（外）の変換用
	int add_x = my_add_x / my_shr_x;	// 縮小単位での横付加量
	int add_y = my_add_y / my_shr_y;	// 縮小単位での縦付加量

	//このi,jはSHR座標（内)系
	for (int i = 0; i < shrink_height; i++) {
		for (int j = 0; j < shrink_width; j++) {
			int ext_x = add_x;
			if (slope_x > 0) {
				//left～rightエリア外では、倍率は０(平均化としてshr分だけ。LPFまで大きくならない)
				//板端ちかくでも、倍率を低くする。(slopeに応じて)
				ext_x = __min(__max((j - shrink_left)/slope_x, 0), add_x);
				int ext_x2 = __min(__max((shrink_right-j)/slope_x, 0), add_x);
				ext_x = __min(ext_x, ext_x2);
			}
			int ext_y = add_y;
			if (slope_y > 0) {
				ext_y = __min(__max((j - shrink_left)/slope_y, 0), add_y);
				int ext_y2 = __min(__max((shrink_right-j)/slope_y, 0), add_y);
				ext_y = __min(ext_y, ext_y2);
			}
			my_expand_input[i*shrink_width+j] = get_mean_val(j, i, ext_x, ext_y);
		}
	}
}

// 1/SHRのサイズとして作成されたLPF用画像、my_expand_inputを
// 元の画像サイズに拡大する。
//入力：my_expand_input
//出力：output
void CLPFSAT::expand_output(unsigned char *output)
{
	int shrink_width = my_width / my_shr_x;
	int shrink_height = my_height / my_shr_y;
	// IPPで拡大
	IppiSize src_sz = {shrink_width, shrink_height};
	IppiRect src_roi = {0, 0, src_sz.width, src_sz.height};
	IppiSize dst_sz = {my_width, my_height};
#ifdef _IPP_VERSION_7
	ippiResizeLinearInit_8u(src_sz, dst_sz, my_ipp_resize_spec);
	int buf_size;
	ippiResizeGetBufferSize_8u(my_ipp_resize_spec, dst_sz, 1, &buf_size);
	IppiPoint dst_offset = {0, 0};
	ippiResizeLinear_8u_C1R(my_expand_input, shrink_width, output, my_dst_pitch, dst_offset, dst_sz,
		ippBorderRepl, NULL, my_ipp_resize_spec, my_ipp_resize_buf);
#else
	ippiResize_8u_C1R(my_expand_input, src_sz, shrink_width, src_roi, 
					  output, my_dst_pitch, dst_sz, 
					  my_shr_x, my_shr_y, IPPI_INTER_LINEAR);
#endif

}

//画像全域で、LPF画像を作成
void CLPFSAT::Exec(const BYTE* src, BYTE *output)
{
	ExecCenter(src, output, 0, my_width-1);
}


//画像のLEFT,RIGHTを指定して、そのうちで、LPF画像を作成
void CLPFSAT::ExecCenter(const BYTE* src, BYTE *output, long left_point, long right_point)
{
	// tmplineクリア
	memset(my_tmpline, 0, sizeof(DWORD)*my_expand_input_width);
#ifdef _IPP_MAKESAT_USE_PARTIAL
	// 縮小したSATを作る
	for (int i = 0; i < my_expand_input_height; i+=my_shr_y) {
		expand_input(src, i, my_shr_y, left_point, right_point);
		for (int j = 0; j < my_shr_y; j++) {
			int inofs = j*my_expand_input_pitch;
			SwdetectMMX::VertAdd(my_expand_input+inofs, my_expand_input_pitch, my_tmpline);
		}
		int outofs = (i/my_shr_y)*my_sat_width;
		SwdetectMMX::HorzAccumStep(my_tmpline, my_expand_input_width, my_shr_x, my_sat+outofs);
	}
#else
	expand_input(src, left_point, right_point);
	// 縮小したSATを作る
	for (int i = 0; i < my_expand_input_height; i+=my_shr_y) {
		for (int j = 0; j < my_shr_y; j++) {
			int inofs = (i+j)*my_expand_input_pitch;
			SwdetectMMX::VertAdd(my_expand_input+inofs, my_expand_input_pitch, my_tmpline);
		}
		int outofs = (i/my_shr_y)*my_sat_width;
		SwdetectMMX::HorzAccumStep(my_tmpline, my_expand_input_width, my_shr_x, my_sat+outofs);
	}
#endif

	// 平均画像作成
	make_mean_image();

	// 拡大
	expand_output(output);

}

 


void CLPFSAT::ExecEdge(const BYTE* src, BYTE *output, long left_proc, long right_proc,
					   long left_edge, long right_edge, long slope_x, long slope_y)
{
	// tmplineクリア
	memset(my_tmpline, 0, sizeof(DWORD)*my_expand_input_width);
#ifdef _IPP_MAKESAT_USE_PARTIAL
	// 縮小したSATを作る
	for (int i = 0; i < my_expand_input_height; i+=my_shr_y) {
		expand_input(src, i, my_shr_y, left_proc, right_proc);
		for (int j = 0; j < my_shr_y; j++) {
			int inofs = j*my_expand_input_pitch;
			SwdetectMMX::VertAdd(my_expand_input+inofs, my_expand_input_pitch, my_tmpline);
		}
		int outofs = (i/my_shr_y)*my_sat_width;
		SwdetectMMX::HorzAccumStep(my_tmpline, my_expand_input_width, my_shr_x, my_sat+outofs);
	}
#else
	expand_input(src, left_point, right_point);
	// 縮小したSATを作る
	for (int i = 0; i < my_expand_input_height; i+=my_shr_y) {
		for (int j = 0; j < my_shr_y; j++) {
			int inofs = (i+j)*my_expand_input_pitch;
			SwdetectMMX::VertAdd(my_expand_input+inofs, my_expand_input_pitch, my_tmpline);
		}
		int outofs = (i/my_shr_y)*my_sat_width;
		SwdetectMMX::HorzAccumStep(my_tmpline, my_expand_input_width, my_shr_x, my_sat+outofs);
	}
#endif

	// 平均画像作成
	make_mean_image(left_edge, right_edge, slope_x, slope_y);

	// 拡大
	expand_output(output);

#if 0
	//debug sat, lpf bitmaps;
__declspec( thread ) int tls_i = 1;
	if( tls_i == 1 ){
		char tmp[256];
		sprintf(tmp, "imglib.%d.lpf.bmp",GetCurrentThreadId() );
		save_as_bmp( tmp, my_width, my_height, output );
		tls_i = 0;


		sprintf(tmp, "sat.%d.txt",GetCurrentThreadId() ); 
		if( FILE *fp = fopen(tmp, "w") ){
			for( int y=0; y< my_expand_input_height/my_shr_y; y++){
				for( int x=0; x<my_sat_width; x++){
					fprintf(fp, "\t%u", my_sat[ y*my_sat_width + x ] );

				}
				fprintf(fp, "\n");
			}

			fclose(fp);
		}
	}
#endif
}


void CLPFSAT::ExecEdgeTarget(const BYTE* src, BYTE *output, long left_proc, long right_proc,
					   long left_edge, long right_edge, long slope_x, long slope_y, BYTE target)
{
	ExecEdge( src, output, left_proc, right_proc, left_edge, right_edge, slope_x, slope_y );

	//エッジ外部をtargetでクリア
	int nEdgeSizeR = my_width-right_edge;
	for(int ii=0; ii<my_height; ii++) {

		memset( &output[ii*my_src_pitch+0         ], target, left_edge );
		memset( &output[ii*my_src_pitch+right_edge], target, nEdgeSizeR );
	}
}



void CLPFSAT::ExecLimiter(const unsigned char* src, unsigned char *output, unsigned char lower, unsigned char upper)
{
	// tmplineクリア
	memset(my_tmpline, 0, sizeof(DWORD)*my_expand_input_width);
#ifdef _IPP_MAKESAT_USE_PARTIAL
	// 縮小したSATを作る
	for (int i = 0; i < my_expand_input_height; i+=my_shr_y) {
		expand_input(src, i, my_shr_y, 0, my_width-1);
		for (int j = 0; j < my_shr_y; j++) {
			int inofs = j*my_expand_input_pitch;
			SwdetectMMX::VertAddLimit(my_expand_input+inofs, my_expand_input_pitch, my_tmpline, lower, upper);
		}
		int outofs = (i/my_shr_y)*my_sat_width;
		SwdetectMMX::HorzAccumStep(my_tmpline, my_expand_input_width, my_shr_x, my_sat+outofs);
	}
	// 平均画像作成
	// 左端と1番上の行は特別扱い
#else
	expand_input(src, 0, my_width-1);
	// 縮小したSATを作る
	for (int i = 0; i < my_expand_input_height; i+=my_shr_y) {
		for (int j = 0; j < my_shr_y; j++) {
			int inofs = (i+j)*my_expand_input_pitch;
			SwdetectMMX::VertAddLimit(my_expand_input+inofs, my_expand_input_pitch, my_tmpline, lower, upper);
		}
		int outofs = (i/my_shr_y)*my_sat_width;
		SwdetectMMX::HorzAccumStep(my_tmpline, my_expand_input_width, my_shr_x, my_sat+outofs);
	}
#endif
	make_mean_image();

	// 拡大
	expand_output(output);
}

void CLPFSAT::MakeShrinkLimiter(const unsigned char* src, unsigned char lower, unsigned char upper)
{
	MakeShrinkLimiter(src, lower, upper, 0, my_width-1);
}

void CLPFSAT::MakeShrinkLimiter(const unsigned char* src, unsigned char lower, unsigned char upper, long left, long right)
{
	// tmplineクリア
	memset(my_tmpline, 0, sizeof(DWORD)*my_expand_input_width);
#ifdef _IPP_MAKESAT_USE_PARTIAL
	// 縮小したSATを作る
	for (int i = 0; i < my_expand_input_height; i+=my_shr_y) {
		expand_input(src, i, my_shr_y, left, right);
		for (int j = 0; j < my_shr_y; j++) {
			int inofs = j*my_expand_input_pitch;
			SwdetectMMX::VertAddLimit(my_expand_input+inofs, my_expand_input_pitch, my_tmpline, lower, upper);
		}
		int outofs = (i/my_shr_y)*my_sat_width;
		SwdetectMMX::HorzAccumStep(my_tmpline, my_expand_input_width, my_shr_x, my_sat+outofs);
	}
	// 平均画像作成
	// 左端と1番上の行は特別扱い
#else
	expand_input(src, 0, my_width-1);
	// 縮小したSATを作る
	for (int i = 0; i < my_expand_input_height; i+=my_shr_y) {
		for (int j = 0; j < my_shr_y; j++) {
			int inofs = (i+j)*my_expand_input_pitch;
			SwdetectMMX::VertAddLimit(my_expand_input+inofs, my_expand_input_pitch, my_tmpline, lower, upper);
		}
		int outofs = (i/my_shr_y)*my_sat_width;
		SwdetectMMX::HorzAccumStep(my_tmpline, my_expand_input_width, my_shr_x, my_sat+outofs);
	}
#endif
	make_mean_image();

}

void CLPFSAT::GetOutputPartial(unsigned char *output, long top, long bkheight)
{
	int shrink_width = my_width / my_shr_x;
	int shrink_height = my_height / my_shr_y;
	// IPPで拡大
#ifdef _IPP_VERSION_7
	IppiSize src_sz = {shrink_width, bkheight/my_shr_y};
	IppiSize dst_sz = {my_width, bkheight};
	ippiResizeLinearInit_8u(src_sz, dst_sz, my_ipp_resize_spec);
	int buf_size;
	ippiResizeGetBufferSize_8u(my_ipp_resize_spec, dst_sz, 1, &buf_size);
	IppiPoint dst_offset = {0, 0};
	ippiResizeLinear_8u_C1R(my_expand_input + (top/my_shr_y)*shrink_width, shrink_width, output, my_dst_pitch, dst_offset, dst_sz,
		ippBorderRepl, NULL, my_ipp_resize_spec, my_ipp_resize_buf);
#else
	IppiSize src_sz = {shrink_width, shrink_height};
	IppiRect src_roi = {0, top/my_shr_y, src_sz.width, bkheight/my_shr_y};
	IppiSize dst_roi_sz = {my_width, bkheight};

	ippiResize_8u_C1R(my_expand_input, src_sz, shrink_width, src_roi,
		output, my_dst_pitch, dst_roi_sz, my_shr_x, my_shr_y, IPPI_INTER_LINEAR);
#endif
}
};
#endif