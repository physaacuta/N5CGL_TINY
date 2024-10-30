// *********************************************************************************
//	RPI�t�@�C���̊֘A�\����
//	[Ver]
//		Ver.01    2014/09/15  
//
//	[����]
//
// *********************************************************************************

#pragma once
#include "vc_stdint.h"

namespace RpiLib
{
//------------------------------------------
// �g���w�b�_�[���\���� (MAX224byte��416�Ɋg��)
//------------------------------------------
struct T_RPI_HEADER_BASE {
    long            nTHeaderSize;    // ���̍\���̂̃T�C�Y
    long            nTHeaderVer;     // �s�w�b�_�[�o�[�W���� (���̂Ƃ���1�Œ�)
    char            cSysName[20];    // �V�X�e������
    char            cKizukenNo[20];  // �r���Ǘ�No
    short           frame_info_type; // �t���[����񕔂̊i�[�`�� (0:���� 1:T_RPI_FRAME_INFO_T1)
    short           frame_info_size; // �t���[����񕔂̃T�C�Y
    DWORD           nFrameNumInit;   // �R�C���擪�̃t���[��No
    DWORD           nFrameNumStart;  // ���̃t�@�C���擪�̃t���[��No
    long            nFileNum;        // �R�C���P�ʂł̃t�@�C������(�P�R�C���̒��ŁA�t�@�C�������������ɉ��Z�B)(0:�������� 1�`:�����L��Ŋ����ځB1�R�ڂ̂Ƃ���1)
    long            nLineCenter;     // ���C���Z���^�[��f(�A���摜���[����̃I�t�Z�b�g�l) [pixel]
    long            nStartCamset;    // ���[�J�����Z�b�g�ԍ� (1�I���W��)
    long            nCamsetNum;      // �������t���[�������� (1�I���W��)
    long            nCamAngle;       // �J�����p�x (1�I���W��) �F���̏ꍇ�́A�F(1�I���W��)
    long            nMen;            // �\���敪 (0:�\ 1:��)

    struct CAM_INFO {
        float        dBaseXoffset;   // ��ʒu�B���C���Z���^�[����̊e�J�������[�̈ʒu(�������́A�}�C�i�X�l) [mm] (PARAM_CAMSET��Xoffset�l���̂���)
        int          nBaseLeft;      // ���f�B������̃t���[�����[���玩�J�����摜���[�܂ł̋���
        int          nOverLap;       // ���J�����Ƃ̉E���̃I�[�o�[���b�v��
    }caminfo[8];
    long            nStartLen;       // �擪�t���[������ʒu [mm]
    long            nColorType;      // �摜�^�C�v (0:�O���[�摜  1:�J���[�摜  2:�F���摜)    
    long            nSysType;        // �r���^�C�v (0:�A�����C��  1:�o�b�`���C��)
    long            nFTposCfNo[2];   // ���[�ʒu�t���[��No (���̃t�@�C���擪����̃t���[��No (0�I���W��))            �� �o�b�`�̂�
    long            nFTposPixel[2];  // ���[�ʒu�t���[����pixel (���̃t���[����[����̃s�N�Z���ʒu (0�I���W��))

	int64_t			nFooterStartPos; // �t�b�^�[�̊J�n�ʒu�i�t�@�C���擪����̈ʒu�j
	long			szFooterRecord;	 // �t�b�^�[�̂P���R�[�h������̃T�C�Y
	
	T_RPI_HEADER_BASE(){
		memset( this, 0, sizeof(*this) );
		nTHeaderSize  = sizeof(*this);
		nTHeaderVer   = 1;
	}
};

//�\���G���A���܂߂��AT�w�b�_
//����ŕϐ����m�ۂ��Ă����ƁA�g�����ꂽ�Ƃ�������Ȃ�ɑ��v
struct T_RPI_HEADER : T_RPI_HEADER_BASE{
	char yobi[ 416 - sizeof(T_RPI_HEADER_BASE) ];

    T_RPI_HEADER(){
        memset( yobi, 0, sizeof(yobi) );
    }
};

//------------------------------------------
//�t���[���t����� �^�C�v�@  128byte�Œ�
//------------------------------------------
struct T_RPI_FRAME_INFO_T1 {
	DWORD			nFNo;											// �t���[��No
	long			nSpeed;											// ���C�����x [mm/sec]
	float			vRes;											// �c����\ [mm/pixel]
	long			nFromLen;										// �R�C����[����̋��� [mm]		
	float			dEdgePos[2];									// �G�b�W�ʒu [mm]
	int				nEdgeIdx[2];									// ���[����̍��E�G�b�WPixel�ʒu [pixel]
	
	int				nYobi[24];

	T_RPI_FRAME_INFO_T1()
	{
		memset( this, 0, sizeof(*this) );
	}
};


//------------------------------------------
// �g���t�b�^�[�\���́B
// �t�@�C�������ɁA���̍\���̂��t���[�������J��Ԃ��������B
//------------------------------------------
struct T_RPI_FOOTER_RECORD
{
	int64_t nFilePos;			//�t���[���̃t�@�C���擪����̊J�n�ʒu
	int32_t szFrame;			//�t���[���̃T�C�Y�i�摜�E�]���E�s�t���[����񕪂ȂǊ܂ށj
	int32_t szImage;			//�摜�̃T�C�Y
};


//------------------------------------------
// RPI��{�w�b�_�B�@RPI�t�@�C���̐擪�ɒu�����B
//------------------------------------------
struct RpiHeader
{    
    int32_t type;					//RPI�^�C�v
    int32_t header_size;			//RPI�w�b�_�̃T�C�Y
    int32_t width;					//�t���[���摜�̕�
    int32_t height;					//�t���[���摜�̍���
    int32_t nchannel;				//�t���[���摜�̃`���l����
    int32_t pixel_nbit;				//1pixel������r�b�g���B
    int32_t nframe;					//�t���[����
    int32_t frame_start_pos;		//�P�t���[���ڂ̎n�܂�t�@�C���̈ʒu
    float   reso_x; //opt			//�𑜓x�E�� [mm/pixel]
    float   reso_y; //opt			//�𑜓x�E�c [mm/pixel]

	int32_t codec;                   //���k�`���B[RfRF] Cf CF  ;Raw�`����CODEC�͖{���������A�֋X��CODEC_RAW������B
    
//�ȉ��́A�t�H�[�}�b�g�^�C�v�ɂ���Ă�������Ȃ�������B�Ȃ��ꍇ�͂O�ɂȂ�
    int32_t frame_info_type;         //   RF     CF  
    int32_t frame_nbyte;             //Rf RF
    int32_t frame_pitch_nbyte;       //Rf
    int32_t pitch_nbyte;             //Rf RF  Cf     ;�Ȃ�ŁACf�ɂ���̂��͂킩��Ȃ��B
    int32_t tHeader_start_pos;	 	 //       Cf[opt]

	T_RPI_HEADER theader;            //�g���w�b�_

//�ȉ��w���p���\�b�h
	enum FormatType{
		FORMATTYPE_UNKNOWN,			 //0000:�s���Ȍ`��
		FORMATTYPE_Rf = 1,           //0001:Rf:�@���E���t���[��
		FORMATTYPE_RF = 3,           //0011:RF:�@���E�L�t���[��
		FORMATTYPE_Cf = 5,           //0101:Cf:���k�E���t���[��
		FORMATTYPE_CF = 7,           //0111:CF:���k�E�L�t���[��
	};
	//type��header����ARPI�`����Ԃ�
	FormatType GetFormatType() const ;
	
	//����RPI��EPC�t���[�����������H
	bool HasFrameInfo() const;

	//����RPI�͉𑜓x���������H
	bool HasResolution()const;

	mutable char m_sCodec[5];
	//codec���𕶎���ł������B
	const char *GetCodecName() const {  
		const char *wkcodec = ( const char*)&codec;
		m_sCodec[0] = 	wkcodec[3];
		m_sCodec[1] = 	wkcodec[2];
		m_sCodec[2] = 	wkcodec[1];
		m_sCodec[3] = 	wkcodec[0];
		m_sCodec[4] = 	0;
		return m_sCodec;
	}

	//CODEC�ꗗ
#define RPIFOURCC( c0, c1, c2, c3 )   ( (c0) << 24 | (c1)<< 16 | (c2) <<8 | (c3))
    enum emCodecType{
		CODEC_UNKNOWN = 0,									//�s��
        CODEC_JPEG = RPIFOURCC( 'J', 'P', 'E', 'G' ),		//JPEG���k
        CODEC_LAC  = RPIFOURCC( 'L', 'A', 'C', ' ' ),		//LAC(���x���k)
        CODEC_NUC  = RPIFOURCC( 'N', 'U', 'C', ' ' ),		//NUC
        CODEC_RAW  = RPIFOURCC( 'R', 'A', 'W', ' ' ),		//RAW�F�t���[�����B
        CODEC_BMP  = RPIFOURCC( 'B', 'M', 'P', ' ' ),		//Windows�r�b�g�}�b�v

		CODEC_JP2K = RPIFOURCC( 'J', 'P', '2', 'K' ),		//Jpeg2000
    };


	//�������������Ȃ��R���X�g���N�^�B�ǂݍ��ݗp�B
	RpiHeader();

	//�������ݗp�ɁA��{�����o�̒l���Z�b�g����R���X�g���N�^
	RpiHeader(int32_t width, int32_t height, emCodecType codec, const T_RPI_HEADER_BASE &theader=T_RPI_HEADER_BASE());
};


//------------------------------------------
// �t���[�������N��� �\����
//�t�@�C����nPosInFile ~ szFrame�܂œǂ񂾂�A
//�摜�T�C�Y�E��  [0                         ~  szPreImg ]�B
//�摜            [szPreImg                  ~  szImg ]�B
//EPC�t���[����� [szPreImg + szImg          ~  szInf]�B
//�s�t���[�����  [szPreImg + szImg + szInf  ~  szTFrameInf ]�B
//------------------------------------------
struct RPIFrameLink
{
	int64_t nPosInFile;
	int32_t szImg;
	int32_t szFrame;


	RPIFrameLink CalcFrameSizeAndReturnNextPos( int szImg , const struct RPIFrameSizeInfo &LinkArray)	;
};

//------------------------------------------
//�t�@�C���`���ɂ���ĈႤ�A�t���[���̑O��T�C�Y��� �\����
//------------------------------------------
struct RPIFrameSizeInfo
{
	int32_t szPreImg;			//�t���[���̏��߁A�摜�O�ɂ��郊���N���̃T�C�Y
    int32_t szInfo;				//�摜�̌�ɂ���AEPC�t���[�����̃T�C�Y
    int32_t szTFrameInf;		//EPC�t���[�����̂��Ƃɂ���A�s�t���[�����̃T�C�Y
	
	int32_t nSectorAlign;		//�ŏ��������ݒP�ʁB
	
	RPIFrameSizeInfo() : nSectorAlign(512) {}

	//------------------------------------------
	//int size �w��T�C�Y���ŏ��������ݒP�ʂŐ؂�グ���l��������
	//------------------------------------------
	int roundup(int size) const
	{
		return ((size + nSectorAlign-1)/nSectorAlign)*nSectorAlign;
	}

	//------------------------------------------
	//�摜�̃T�C�Y����A������i�[����t���[���̑傫�����������B
	// imagesize : �摜�̃T�C�Y�B
	//------------------------------------------
	int CalcFrameSize(int imagesize) const
	{
		return roundup(szPreImg + imagesize + szInfo + szTFrameInf);
	}

	static const int LINK_SIZE_Cf = 12;
};


//------------------------------------------
//RPI�t�@�C���̊e�t���[���ʒu��ێ�����N���X�B
//�t���[���O������p�����Ă���̂ŁA�e��v�Z�ɂ��g����B
//------------------------------------------
class RpiFrameLinkArray 
	: public RPIFrameSizeInfo
	, public std::deque<RPIFrameLink>
{
public:
	//------------------------------------------
    //���t���[���ł��łɎ��t���[���̐擪��4K���炢�ǂ�ł����B
    //�i���t���[����ǂޏu�Ԃɂ́A���t���[���̈ʒu�͂킩���Ă���j
    //���́u���t���[�����v����A���t���[���̃T�C�Y�ƁA�����t���[���̈ʒu�����肷��
	//------------------------------------------
    bool UpdateNextPosOnRead(int nframe, const BYTE *pNextFrame );
};

//�N���X�̑O���錾
class RpiReader;
class IRpiFrame;
class IRpiFrameEnum ;
class RpiReaderFile;
class RpiReaderMemory;

class TRpiWriter;

};