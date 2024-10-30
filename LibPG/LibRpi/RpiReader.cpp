#include <stdafx.h>
#include <stddef.h>
#include "rpi.h"
#include "rpi_impl.h"
#include "RpiReader.h"

namespace RpiLib{


//------------------------------------------
//RPI�w�b�_�̎�ނɉ������R�s�[�\���̂��擾
//�@RpiHeader::FormatType type�@�w�b�_���
//�߂�l�@�R�s�[�p�̍\���̔z��
//------------------------------------------
static const CopyInt32 * getHeaderCopier( RpiHeader::FormatType type)
{
	switch( type ){
		case RpiHeader::FORMATTYPE_Rf: return RpiHeaderCopier::Parse_Rf;
		case RpiHeader::FORMATTYPE_RF: return RpiHeaderCopier::Parse_RF;	
		case RpiHeader::FORMATTYPE_Cf: return RpiHeaderCopier::Parse_Cf;
		case RpiHeader::FORMATTYPE_CF: return RpiHeaderCopier::Parse_CF;
		default:		   return NULL;
	}
}

//------------------------------------------
//RPI�t�@�C���̃����N��������������B
// predicate_frame_pitch  �t���[���T�C�Y�\���l�F�Q�t���[���ڂ̈ʒu - �P�t���[���ڂ̈ʒu
//
//�Œ蒷�Ȃǂ̏ꍇ�́A��C�Ƀ����N���i�t���[�����ǂ̃t�@�C���ʒu�ɂ��邩�j������ł���B
//------------------------------------------
bool RpiReader::setupFramePreLength( int predicate_frame_pitch, int first_size_if_cb )
{
	LinkArray.clear();
	LinkArray.szInfo   = 0;
	LinkArray.szTFrameInf = 0;

	//EPC�t���[����񂠂�̏ꍇ�A����͊e�t���[���̉摜�㉽�o�C�g�H
	if( Header.HasFrameInfo() ){
		switch(Header.frame_info_type)
		{
		case  0: LinkArray.szInfo = 0;              break;
		case  1: LinkArray.szInfo = 8;              break;
		case  2: LinkArray.szInfo = 8*Header.height;break;
		default: LinkArray.szInfo = 0;              break;
		}
	}

	LinkArray.resize( Header.nframe + 1 );

	//���߂���ꍇ�A��C�Ɋe�t���[���̊J�n�ʒu�E�T�C�Y�����肷��B
	switch( Header.GetFormatType() )
	{

	case  RpiHeader::FORMATTYPE_Rf:
		//�����́A�Œ蒷�t�H�[�}�b�g�B frame_pitch * N�Ńt���[���J�n�ʒu�����܂�
		LinkArray.szPreImg = 0;
		for( int ii=0; ii<Header.nframe; ii++){
			LinkArray[ii].nPosInFile   = Header.frame_start_pos + int64_t(Header.frame_pitch_nbyte) * ii;
			LinkArray[ii].szImg = Header.frame_nbyte;
			LinkArray[ii].szFrame = Header.frame_pitch_nbyte; 
		}
		break;

	case RpiHeader::FORMATTYPE_RF:
		LinkArray.szPreImg = 8;

		//���L�́A�Œ蒷�t�H�[�}�b�g�ł͂Ȃ����A�t���[�����T�C�Y�͕ς��Ȃ����낤�Ƃ�������
		for( int ii=0; ii<Header.nframe; ii++){
			LinkArray[ii].nPosInFile   = Header.frame_start_pos + int64_t(predicate_frame_pitch) * ii;
			LinkArray[ii].szImg = Header.frame_nbyte;
			LinkArray[ii].szFrame = predicate_frame_pitch; 
		}
		break;

	case RpiHeader::FORMATTYPE_Cf:
	case RpiHeader::FORMATTYPE_CF: 
		LinkArray.szPreImg = RPIFrameSizeInfo::LINK_SIZE_Cf;
		LinkArray[0].nPosInFile = Header.frame_start_pos;

		//�t�H�[�}�b�g�͈��k�`��(�Ƃ�����CODEC�L)�����ACODEC��
		//�񈳏k�̏ꍇ�́A�e�t���[���̊J�n�ʒu�E�T�C�Y������ł���B
		if(RpiHeader::CODEC_BMP == Header.codec || RpiHeader::CODEC_RAW == Header.codec )
		{
			for( int ii=0; ii<Header.nframe; ii++){
				LinkArray[ii].nPosInFile   = Header.frame_start_pos + int64_t(predicate_frame_pitch) * ii;
				LinkArray[ii].szImg   = first_size_if_cb;
				LinkArray[ii].szFrame = predicate_frame_pitch; 
			}
		}
		
		////have Index
		//if(sizeof(T_RPI_FOOTER_RECORD) == Header.theader.szFooterRecord &&
		//	0 != Header.theader.nFooterStartPos )

		break;
	}
	return true;
}

//------------------------------------------
//RPI�t�b�^��ǂݍ��ށB
//LinkArray����C�ɑS�����܂�B
//�@const BYTE *footer	�ǂݍ��񂾃t�b�^�̐擪
//�@int length			�ǂݍ��񂾃t�b�^�̃T�C�Y
//�߂�l�@���肵���t���[���̌�
//------------------------------------------
int RpiReader::ReadTRPIFooter( const BYTE *footer, int length )
{
	//�l���Flength�̓Z�N�^�T�C�Y�Ő؂肠����B
	// �ق�Ƃ̌��́A�ԕ����o�Ă���܂Ł@Or �w�b�_�̃t���[�����B

	// LinkArray��setupFramePreLength�Ń��T�C�Y����Ă���͂��B
	const int szFooteUnit = Header.theader.szFooterRecord; //sizeof(T_RPI_FOOTER_RECORD) ;
	int  nUnit = min( int(LinkArray.size()), length/szFooteUnit);

	for( int ii=0; ii<nUnit; ii++){
		const T_RPI_FOOTER_RECORD *rec =
			 reinterpret_cast<const T_RPI_FOOTER_RECORD*>( &footer[ii*szFooteUnit] );

		if(  -1 == rec->nFilePos )
			return ii;
		LinkArray[ii].nPosInFile = rec->nFilePos;
		LinkArray[ii].szImg = rec->szImage;
		LinkArray[ii].szFrame = rec->szFrame;
	}

	return nUnit;
}


//------------------------------------------
// �t���[���̊J�n�ʒu���X�V����B
// int64_t &nPosFromLink�@�����N�\���̒��́A�t�@�C���ʒu���t�@�����X
// int64_t nPosFromFile�@�@�V���ɔ��������A�t�@�C���̈ʒu
//------------------------------------------
static bool set_with_check_pos( int64_t &nPosFromLink, int64_t nPosFromFile )
{
	// ���摜����Ƃ��ňʒu���킩���Ă���Ƃ��́A��r���Ă���X�V����B
	if( 0 != nPosFromLink && nPosFromLink != nPosFromFile ) 
	{
		nPosFromLink = nPosFromFile;
		return false;
	}
	nPosFromLink = nPosFromFile;
	return true;
}


//------------------------------------------
//���t���[���̐擪�����f�[�^����A
//���t���[���̒����ƁA�����t���[���̊J�n�ʒu�����肷��B
//	int nFrame�@�@�@�@�@�@�@�@�@�@�@ ���t���[���̔ԍ�(0�I���W��)
//  const unsigned char *pNextFrame�@���t���[���̐擪�̈�
//------------------------------------------
bool RpiFrameLinkArray::UpdateNextPosOnRead(int nFrame, const unsigned char *pNextFrame)
{
	bool bRet = true;
	if( nFrame+1 >= (int)this->size() ) return false;

	RPIFrameLink &ThisLink = this->at(nFrame    );
	RPIFrameLink &NextLink = this->at(nFrame + 1);

	switch( szPreImg ){
		//���摜�E���t���[��
		case 0:
			//�t���[�����͂Ȃ��̂łȂɂ��ł��Ȃ�
			break;

		//���摜�E�L�t���[���`��
		case 8:
			bRet = set_with_check_pos(NextLink.nPosInFile, read_int64( pNextFrame, 0 ) );
			break;

		//���k�`��
		case 12:
			ThisLink.szImg = read_int32( pNextFrame , 0);
			bRet = set_with_check_pos(NextLink.nPosInFile , read_int64( pNextFrame ,  4 ) );
			ThisLink.szFrame = static_cast<int>(NextLink.nPosInFile - ThisLink.nPosInFile);
			break;

		default:
			bRet = false;
			//!?
			break;
	}
	return bRet;
}

//------------------------------------------
//�ǂݍ���RPI�t�@�C���̃w�b�_�������߂���RpiHeader�𖄂߂�B
//�@const BYTE *buf	RPI�t�@�C���̐擪�����B
//�@int length		�ǂݍ��񂾒����B�Œ�8192�o�C�g
//�߂�l RPI�Ƃ��ĉ��߂ł����̂Ȃ�Atrue
//------------------------------------------
bool RpiReader::Parse(const BYTE *buf, int length)
{
	memset( &Header, 0, sizeof(Header) );
	
	//�w�b�_�̌`�������߂镔����ǂށB
	Header.type = read_int32( buf, 0);
	Header.header_size = read_int32( buf, 4);

	//�`���ɏ]���āA�t�@�C���̒��g���\���̂փR�s�[�B
	RpiHeader::FormatType ft = Header.GetFormatType();
	const CopyInt32 * copier =  getHeaderCopier( ft );
	if( !copier ) {
		char cType[5]=""; memcpy(&cType, &Header.type, 4);
		m_sErrorMsg.Format("�s���Ȍ^���ł��B�w�b�_�T�C�Y=%d, �^�C�v=%s", Header.header_size, cType );
		//�w�b�_�����m�̂S��ނ̂ǂ�ł��Ȃ������B
		return false;
	}
	for( const CopyInt32 *pCp = copier; pCp->nFile >=0; pCp++)
	{
		pCp->StructFromFile( &Header, buf );
	}

	//���`��RPI�w�b�_�ɂ�CODEC�t�B�[���h�͂Ȃ����A���ւ̂���ODEC_RAW�𖄂߂Ă����B
	if( RpiHeader::FORMATTYPE_RF == ft ||
		RpiHeader::FORMATTYPE_Rf == ft )
	{
		Header.codec = RpiHeader::CODEC_RAW;
	}

	//����\���Ȃ��Ƃ��̃f�t�H���g�l�ݒ�B
	if( !Header.HasResolution() )
	{
		Header.reso_x = 0;
		Header.reso_y = 0;
	}

	//�s�w�b�_�����݂���ꍇ�A�\���̂փR�s�[�B
	if (   0 < Header.tHeader_start_pos  
		&& Header.tHeader_start_pos + int(sizeof(T_RPI_HEADER_BASE)) < length )
	{
		int szTHeader = read_int32(  buf, Header.tHeader_start_pos );
		memcpy( &Header.theader, &buf[Header.tHeader_start_pos],   szTHeader);

        //�s�w�b�_������ꍇ�A�s�t���[����񂪁A�e�t���[���ɉ��o�C�g���邩
		LinkArray.szTFrameInf = Header.theader.frame_info_size;
	}


	//�w�b�_�{������A�t���[���̌���B
	setupFramePreLength( 
		static_cast<int>( read_int64( buf, Header.frame_start_pos + 4 ) - Header.frame_start_pos), 
		read_int32(buf, Header.frame_start_pos)
		);

	//8192���ǂ߂΁A�ŏ��̃t���[���̓����������łɓǂ�ł�ƍl���Ă悢�B
	//�T�C�Y�Ǝ��t���[���ʒu��LinkArray�ɓo�^����
	return LinkArray.UpdateNextPosOnRead( 0, &buf[ Header.frame_start_pos] );
}


};