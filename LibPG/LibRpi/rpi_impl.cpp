#include <stdafx.h>
#include <stddef.h>
#include "rpi.h"
#include "rpi_impl.h"

namespace RpiLib{


//------------------------------------------
//RPI読み込み用のヘッダのコンストラクタ
//ほとんど何もしない。
//------------------------------------------
RpiHeader::RpiHeader() {
	//念のため
	this->type        = TYPE_RWFL ;
	this->header_size = SIZE_COMP_RESO;
	this->nframe      = 0;
}

//------------------------------------------
//RPI書き込み用のヘッダのコンストラクタ
//ある程度典型的なRPIヘッダを構築する
//int32_t width, int32_t height    フレーム画像の幅・高 [pixel]
//emCodecType codec                画像の圧縮方法 
//const T_RPI_HEADER_BASE &theader 拡張ヘッダ
//------------------------------------------
RpiHeader::RpiHeader(int32_t width, int32_t height, emCodecType codec, const T_RPI_HEADER_BASE &a_theader) 
{
	this->type        = TYPE_RWFL ;
	this->header_size = SIZE_COMP_RESO;
	this->width       = width;
	this->height      = height;
	this->nchannel	  = 1;				//とりあえず、デフォルトとして、
	this->pixel_nbit  = 8;				//8bppが多いかな、レベル
	this->nframe      = 0;
	this->frame_start_pos = 512;
	this->reso_x      = 1;
	this->reso_y      = 1;
	this->codec       = codec;
	this->pitch_nbyte = width;			// 幅と同一画像のみ対応
	this->frame_info_type = 0; 
	this->frame_nbyte     = 0;     
	this->frame_pitch_nbyte = 0;
//	this->tHeader_start_pos 

	memcpy( &this->theader, &a_theader,  min( sizeof( this->theader ),  size_t(a_theader.nTHeaderSize )) );
}



//------------------------------------------
//現フレームのリンク情報を確定させ、次フレーム用のリンク情報を返す。
/// int szImg   画像サイズ
//  LinkArray	フレームを構成する長さ情報の詰まった構造体
//戻り値　次のフレームのリンク情報（位置のみ設定済み）
//------------------------------------------
RPIFrameLink RPIFrameLink::CalcFrameSizeAndReturnNextPos( int szImg , const RPIFrameSizeInfo &LinkArray)
{
	//画像サイズがわかったので、今のフレームを確定
	this->szImg = szImg;
	this->szFrame = LinkArray.CalcFrameSize(szImg);

	//次フレームの位置だけ設定して返す
	RPIFrameLink next;
	next.nPosInFile = this->nPosInFile + this->szFrame;
	next.szImg = 0;
	next.szFrame = 0;
	return next;
}

///コピー構造体区別 Parse_[RC][Ff]
// [RC] R:生形式、C：圧縮形式
// [Ff] f:EPCフレーム無　 F:EPCフレーム有、


//生・無フレーム用コピー構造体
const CopyInt32 RpiHeaderCopier::Parse_Rf[] = {
    { offsetof( RpiHeader, type              ),   0},
    { offsetof( RpiHeader, header_size       ),   4},
    { offsetof( RpiHeader, width             ),   8},
    { offsetof( RpiHeader, height            ),  12},
    { offsetof( RpiHeader, nchannel          ),  16},
    { offsetof( RpiHeader, pixel_nbit        ),  20},
    { offsetof( RpiHeader, pitch_nbyte       ),  24},
    { offsetof( RpiHeader, nframe            ),  28},
    { offsetof( RpiHeader, frame_nbyte       ),  32},
    { offsetof( RpiHeader, frame_start_pos   ),  36},
    { offsetof( RpiHeader, frame_pitch_nbyte ),  40},
    { offsetof( RpiHeader, reso_x            ),  44},
    { offsetof( RpiHeader, reso_y            ),  48},
    { -1, -1},
};

//圧縮・無フレーム用コピー構造体
const CopyInt32 RpiHeaderCopier::Parse_Cf[] =
{
    { offsetof( RpiHeader, type              ),   0},
    { offsetof( RpiHeader, header_size       ),   4},
    { offsetof( RpiHeader, codec             ),   8},
    { offsetof( RpiHeader, width             ),  12},
    { offsetof( RpiHeader, height            ),  16},
    { offsetof( RpiHeader, nchannel          ),  20},
    { offsetof( RpiHeader, pixel_nbit        ),  24},
    { offsetof( RpiHeader, pitch_nbyte       ),  28},
    { offsetof( RpiHeader, nframe            ),  32},
    { offsetof( RpiHeader, frame_start_pos   ),  36},
    { offsetof( RpiHeader, reso_x            ),  40},
    { offsetof( RpiHeader, reso_y            ),  44},
    { offsetof( RpiHeader, tHeader_start_pos), 48 },
    { -1, -1},
};

//生・有フレーム用コピー構造体
const CopyInt32 RpiHeaderCopier::Parse_RF[] = 
{
    { offsetof( RpiHeader, type              ),   0},
    { offsetof( RpiHeader, header_size       ),   4},
    { offsetof( RpiHeader, width             ),   8},
    { offsetof( RpiHeader, height            ),  12},
    { offsetof( RpiHeader, nchannel          ),  16},
    { offsetof( RpiHeader, pixel_nbit        ),  20},
    { offsetof( RpiHeader, pitch_nbyte       ),  24},
    { offsetof( RpiHeader, nframe            ),  28},
    { offsetof( RpiHeader, frame_nbyte       ),  32},
    { offsetof( RpiHeader, frame_start_pos   ),  36},
    { offsetof( RpiHeader, frame_info_type   ),  40},
    { offsetof( RpiHeader, reso_x            ),  44},
    { offsetof( RpiHeader, reso_y            ),  48},
    { -1, -1},
};


//圧縮・有フレーム用コピー構造体
const CopyInt32 RpiHeaderCopier::Parse_CF[] =
{
    { offsetof( RpiHeader, type              ),   0},
    { offsetof( RpiHeader, header_size       ),   4},
    { offsetof( RpiHeader, codec             ),   8},
    { offsetof( RpiHeader, width             ),  12},
    { offsetof( RpiHeader, height            ),  16},
    { offsetof( RpiHeader, nchannel          ),  20},
    { offsetof( RpiHeader, pixel_nbit        ),  24},
    { offsetof( RpiHeader, nframe            ),  28},
    { offsetof( RpiHeader, frame_start_pos   ),  32},
    { offsetof( RpiHeader, frame_info_type   ),  36},
    { offsetof( RpiHeader, reso_x            ),  40},
    { offsetof( RpiHeader, reso_y            ),  44},
    { -1, -1},
};

//------------------------------------------
//戻り値　このRPIファイルは、フレーム有形式か？
//------------------------------------------
bool RpiHeader::HasFrameInfo() const{ return TYPE_RWFT == type; }

//------------------------------------------
//戻り値　このRPIファイルは、分解能があるか？
//------------------------------------------
bool RpiHeader::HasResolution()const{ return SIZE_RAW_RESO == header_size || SIZE_COMP_RESO == header_size; }

//------------------------------------------
//戻り値　RPI基本ヘッダ種類をかえす
// RPIタイプとRPIサイズから判断する。
//------------------------------------------
RpiHeader::FormatType  RpiHeader::GetFormatType() const 
{
	if( TYPE_RWFT == type )
	{
		if(  SIZE_COMP_nreso == header_size || SIZE_COMP_RESO == header_size ){
			return FORMATTYPE_CF;
		}
		
		if(  SIZE_RAW_nreso == header_size || SIZE_RAW_RESO == header_size ) {
			return  FORMATTYPE_RF;
		}

	}

	if( TYPE_RWFL == type ){
		if(  SIZE_COMP_nreso == header_size || SIZE_COMP_RESO == header_size ){
			return FORMATTYPE_Cf;
		}
		if(  SIZE_RAW_nreso == header_size || SIZE_RAW_RESO == header_size ) {
			return  FORMATTYPE_Rf;
		}

	}
	return FORMATTYPE_UNKNOWN;
}

};