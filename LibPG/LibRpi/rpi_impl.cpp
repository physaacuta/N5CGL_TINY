#include <stdafx.h>
#include <stddef.h>
#include "rpi.h"
#include "rpi_impl.h"

namespace RpiLib{


//------------------------------------------
//RPI�ǂݍ��ݗp�̃w�b�_�̃R���X�g���N�^
//�قƂ�ǉ������Ȃ��B
//------------------------------------------
RpiHeader::RpiHeader() {
	//�O�̂���
	this->type        = TYPE_RWFL ;
	this->header_size = SIZE_COMP_RESO;
	this->nframe      = 0;
}

//------------------------------------------
//RPI�������ݗp�̃w�b�_�̃R���X�g���N�^
//������x�T�^�I��RPI�w�b�_���\�z����
//int32_t width, int32_t height    �t���[���摜�̕��E�� [pixel]
//emCodecType codec                �摜�̈��k���@ 
//const T_RPI_HEADER_BASE &theader �g���w�b�_
//------------------------------------------
RpiHeader::RpiHeader(int32_t width, int32_t height, emCodecType codec, const T_RPI_HEADER_BASE &a_theader) 
{
	this->type        = TYPE_RWFL ;
	this->header_size = SIZE_COMP_RESO;
	this->width       = width;
	this->height      = height;
	this->nchannel	  = 1;				//�Ƃ肠�����A�f�t�H���g�Ƃ��āA
	this->pixel_nbit  = 8;				//8bpp���������ȁA���x��
	this->nframe      = 0;
	this->frame_start_pos = 512;
	this->reso_x      = 1;
	this->reso_y      = 1;
	this->codec       = codec;
	this->pitch_nbyte = width;			// ���Ɠ���摜�̂ݑΉ�
	this->frame_info_type = 0; 
	this->frame_nbyte     = 0;     
	this->frame_pitch_nbyte = 0;
//	this->tHeader_start_pos 

	memcpy( &this->theader, &a_theader,  min( sizeof( this->theader ),  size_t(a_theader.nTHeaderSize )) );
}



//------------------------------------------
//���t���[���̃����N�����m�肳���A���t���[���p�̃����N����Ԃ��B
/// int szImg   �摜�T�C�Y
//  LinkArray	�t���[�����\�����钷�����̋l�܂����\����
//�߂�l�@���̃t���[���̃����N���i�ʒu�̂ݐݒ�ς݁j
//------------------------------------------
RPIFrameLink RPIFrameLink::CalcFrameSizeAndReturnNextPos( int szImg , const RPIFrameSizeInfo &LinkArray)
{
	//�摜�T�C�Y���킩�����̂ŁA���̃t���[�����m��
	this->szImg = szImg;
	this->szFrame = LinkArray.CalcFrameSize(szImg);

	//���t���[���̈ʒu�����ݒ肵�ĕԂ�
	RPIFrameLink next;
	next.nPosInFile = this->nPosInFile + this->szFrame;
	next.szImg = 0;
	next.szFrame = 0;
	return next;
}

///�R�s�[�\���̋�� Parse_[RC][Ff]
// [RC] R:���`���AC�F���k�`��
// [Ff] f:EPC�t���[�����@ F:EPC�t���[���L�A


//���E���t���[���p�R�s�[�\����
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

//���k�E���t���[���p�R�s�[�\����
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

//���E�L�t���[���p�R�s�[�\����
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


//���k�E�L�t���[���p�R�s�[�\����
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
//�߂�l�@����RPI�t�@�C���́A�t���[���L�`�����H
//------------------------------------------
bool RpiHeader::HasFrameInfo() const{ return TYPE_RWFT == type; }

//------------------------------------------
//�߂�l�@����RPI�t�@�C���́A����\�����邩�H
//------------------------------------------
bool RpiHeader::HasResolution()const{ return SIZE_RAW_RESO == header_size || SIZE_COMP_RESO == header_size; }

//------------------------------------------
//�߂�l�@RPI��{�w�b�_��ނ�������
// RPI�^�C�v��RPI�T�C�Y���画�f����B
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