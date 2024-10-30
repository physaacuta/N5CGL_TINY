#include <stdafx.h>
#include <stddef.h>
#include "rpi.h"
#include "rpi_impl.h"
#include "rpiwriter.h"

namespace RpiLib{

const bool B_WRITE_FOOTER_INDEX =  true; //RPI�t�b�^��ǉ����邩

//---------------------------	---------------
// �R���X�g���N�^
//------------------------------------------
TRpiWriter::TRpiWriter()
	:m_hFile(0)
	,theader(Header.theader)
{
	m_ovHeader = new COVDataVMAlloc(4096);
	m_byHeader = m_ovHeader->pBuffer;
}

//------------------------------------------
// �����N����������
//------------------------------------------
void TRpiWriter::initLinkArray()
{
	LinkArray.szPreImg = RPIFrameSizeInfo::LINK_SIZE_Cf;
	LinkArray.szTFrameInf = theader.frame_info_size;
	LinkArray.szInfo = 0;

	if(  theader.nTHeaderSize <= 416 )
		Header.frame_start_pos = 512;
	else
		Header.frame_start_pos = 4096;
	
	///**EPC �t���[�����̏������݂̓T�|�[�g���Ă��Ȃ��B**
	//if( 1 == Header.frame_info_type ){
	//	LinkArray.szInfo  = 20;
	//}
	//if( 2 == Header.frame_info_type ){
	//	LinkArray.szInfo  = 8;
	//}


	LinkArray.clear();
	LinkArray.push_back( RPIFrameLink());
	LinkArray[0].nPosInFile = Header.frame_start_pos;
	LinkArray[0].szImg = 0;
	LinkArray[0].szFrame = 0;
}


//------------------------------------------
// �t�@�C�����������ݗp�ɊJ��
// const CString &filename	�t�@�C����
// const RpiHeader &ah		RPI�w�b�_
// bool bDirect				FILE_FLAG_NO_BUFFERING�����邩�ǂ���
// �߂�l�@���킩�ُ킩
//------------------------------------------
bool TRpiWriter::OpenFile( const CString &filename, const RpiHeader &ah, bool bDirect)
{
    //���ɊJ���Ă邵..
    if( 0 != m_hFile ){
		this->m_sErrorMsg.Format("���łɊJ���Ă���ۂ�. '%s'", filename);
        SetLastError(ERROR_ALREADY_INITIALIZED);
        return false;
    }
    
	DWORD flag = FILE_FLAG_OVERLAPPED ;
	if( bDirect ){
		flag |= FILE_FLAG_NO_BUFFERING;
	}
	else{
		flag |= FILE_FLAG_SEQUENTIAL_SCAN ;
	}

	m_hFile = CreateFile( filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
					CREATE_ALWAYS, flag, NULL);
	if( INVALID_HANDLE_VALUE == m_hFile ){
		m_hFile = 0;
		this->m_sErrorMsg.Format("CreateFile('%s')=%d", filename, GetLastError());
		return false;
	}
	//�w�b�_���R�s�[
	//�T�|�[�g���ĂȂ��`����n����Ă�����̂ŁA�����ŔO�̂��ߏC��
	m_sFilename = filename ;
	Header = ah;
	Header.frame_info_type = 0;

	//�t���[�������N����������
	initLinkArray();	

	//�Ƃ肠�����A�w�b�_�����o��
	return FlushHeader();
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
TRpiWriter::~TRpiWriter()
{
	delete m_ovHeader;
}


//------------------------------------------
// �w�b�_�\���̂��t�@�C���ɏ������߂�悤�A�o�C�g�̉�ɕϊ�����
// ���� Header, �o�� m_byHeader
//------------------------------------------
int TRpiWriter::prepare_header() {
	//�g���`��PRI�́A���k�E�t���[�����E�𑜓x����Ȃ̂ŁA���̂悤�ɐݒ肷��
	Header.type        = TYPE_RWFL ;
	Header.header_size = SIZE_COMP_RESO;

	//�s�w�b�_�̈ʒu�����߂�
	if( 0 < theader.nTHeaderSize ){
		Header.tHeader_start_pos = 96;
	}

	//���̉𑜓x���Ȃ��ꍇ�͂O�Ȃ̂ŁA�Ƃ肠�����P�ɂ���
	if( 0 == Header.reso_x ) {
		Header.reso_x = 1;
	}
	if( 0 == Header.reso_y ) {
		Header.reso_y = 1;
	}


	//�o�C�g�̉�Ƃ��Ẵw�b�_
	memset( m_byHeader, 0, 4096 );

	//�g���`��PRI�́A[Cf] ���k�E�t���[�����Ȃ̂ŁA���̂悤�ɍ��B
	for( const CopyInt32 *p = RpiHeaderCopier::Parse_Cf; p->nFile >=0; p++)
	{
		p->FileFromStruct( m_byHeader, &Header);
	}

	//�s�w�b�_���R�s�[����B
	if( Header.tHeader_start_pos > 0 ){
		memcpy(& m_byHeader[ Header.tHeader_start_pos ], &Header.theader, Header.theader.nTHeaderSize);
		return  Header.tHeader_start_pos  + Header.theader.nTHeaderSize;
	}
	else{
		return Header.header_size;
	}
}

//------------------------------------------
//�t���[���������ɁA�O�u����ʒu���ƁA��u����s�t���[�������������ށB
//�@�@BYTE *frame_image		�������ރt���[���B�摜��12�o�C�g�ڂ��疄�܂��Ă���
//�@�@int   image_size		�摜�T�C�Y�B
//�@�@BYTE *frame_image		�t�b�^�B�����ꍇ��A����frame_image�ɖ��܂��Ă���Ȃ�ANULL�B
// �����o�ϐ���LinkArray�Ƀ����N����ǉ�����
//------------------------------------------
RPIFrameLink & TRpiWriter::PrepareFrameOpt( BYTE *frame_image, int image_size, const void *t_frameinf )
{
	if(!m_hFile ) 
		return LinkArray.front();
	
	//���t���[������szImg���m�肳���A���t���[������ǉ�����B
	RPIFrameLink &thislink = LinkArray.back();
	RPIFrameLink nextlink = thislink.CalcFrameSizeAndReturnNextPos( image_size, LinkArray );
	LinkArray.push_back( nextlink );
	Header.nframe = static_cast<int>( LinkArray.size() ) - 1;

	//�������Ƀt���[��������������
	put_int32( &frame_image[0], image_size );
	put_int64( &frame_image[4], nextlink.nPosInFile );

	//�������ɂs�t���[�������R�s�[�B
	if( t_frameinf ) {
		memcpy( &frame_image[image_size + LinkArray.szPreImg], t_frameinf, LinkArray.szTFrameInf);
	}

	//�t���[���T�C�Y�̍Ō�̂ق��̌��Ԃ��O�ŃN���A����B
	int tinfo_end = image_size + LinkArray.szPreImg + LinkArray.szTFrameInf;
	memset( &frame_image[tinfo_end], 0, thislink.szFrame - tinfo_end );
	
	return thislink;
}

//------------------------------------------
//�t���[�����f�B�X�N�ɏ������ށB�����B
//  BYTE *frame_image	�������މ摜�B �Z�N�^�A���C������͖����B12�o�C�g�J����Ƃ����Ȃ��B
//  int   image_size	�摜�T�C�Y�B
//  BYTE *frame_image	�t�b�^�B�����ꍇ��A����frame_image�ɖ��܂��Ă���Ȃ�ANULL�B
// �߂�l�@�f�B�X�N�ɏ������񂾃T�C�Y
// frame_image�ւ̐��񂪊ɂ�����ɁA�������m�ہE�R�s�[�̃I�[�o�[�w�b�h�L�B
//------------------------------------------
int TRpiWriter::WriteFrameCopy( const BYTE *frame_image, int image_size, const void *t_frameinf )
{
    //�A���C������Ă��Ȃ��̂ŁA�������ݗp���������m�ۂ���
    COVDataVMAlloc ovd( LinkArray.CalcFrameSize(image_size) );

    //�摜(+EPC�t���[�����)�R�s�[
    memcpy( &ovd.pBuffer[ LinkArray.szPreImg ], frame_image, image_size );

    //�����N���ǉ��B�s�t���[����񏑂����݁B
	RPIFrameLink &thislink = PrepareFrameOpt( ovd.pBuffer, image_size, t_frameinf );
    //�������݂̔��s�B
	ovd.IssureWrite( m_hFile, thislink.nPosInFile, thislink.szFrame );
	return ovd.WaitResult( m_hFile );
}

//------------------------------------------
//�t���[�����f�B�X�N�ɏ������ށB�����B
//�@�@BYTE *frame_image		�������މ摜�B �Z�N�^�A���C������B12�o�C�g�J���Ă���摜��������Ă��邱�ƁB
//�@�@int   image_size		�摜�T�C�Y�B
//�@�@BYTE *frame_image		�t�b�^�B�����ꍇ��A����frame_image�ɖ��܂��Ă���Ȃ�ANULL�B
//  ���ڈ����œn���ꂽ�������̈�ɁA�����N���E�t���[�����𖄂ߍ��݁A�f�B�X�N�ɏ������ށB
//  �������m�ہE�R�s�[�̃I�[�o�[�w�b�h�͖����B
// �߂�l�@�f�B�X�N�ɏ������񂾃T�C�Y
//------------------------------------------
int TRpiWriter::WriteFrameOpt( BYTE *frame_image, int image_size, const void *t_frameinf )
{
	RPIFrameLink &thislink = PrepareFrameOpt( frame_image, image_size, t_frameinf );
	COVData ovd( frame_image,thislink.szFrame );
	ovd.IssureWrite( m_hFile, thislink.nPosInFile, thislink.szFrame );
	return ovd.WaitResult( m_hFile );
}

//------------------------------------------
//�t���[�����f�B�X�N�ɏ������ށB�񓯊����B
//�@�@BYTE *frame_image_ovd	�������މ摜�B �Z�N�^�A���C������B12�o�C�g�J���Ă���摜��������Ă��邱�ƁB
//�@�@int   image_size		�摜�T�C�Y�B
//�@�@BYTE *frame_image		�t�b�^�B�����ꍇ��A����frame_image�ɖ��܂��Ă���Ȃ�ANULL�B
//  ���ڈ����œn���ꂽ�������̈�ɁA�����N���E�t���[�����𖄂ߍ��݁A�f�B�X�N�ɏ������ށB
//  �f�B�X�N�������݂������������ǂ����́Aovd��hEvent��Am_hFile�Ɋ֘A�Â��������iIOCP�Ƃ��j�Ŕ��f���邱��
// �߂�l�@�񓯊��������݂̐ςݍ��݂����������True
//------------------------------------------
int TRpiWriter::WriteFrameOptAsync( COVData *frame_image_ovd, int image_size, const void *t_frameinf )
{
	RPIFrameLink &thislink = PrepareFrameOpt( frame_image_ovd->pBuffer, image_size, t_frameinf );
	return frame_image_ovd->IssureWrite( m_hFile, thislink.nPosInFile, thislink.szFrame );
}




const int szFooteUnit = sizeof(T_RPI_FOOTER_RECORD);


//------------------------------------------
//�����N���̔z����A�t�b�^�p�ɕϊ�����B
//�@�@RpiFrameLinkArray &LinkArray	�����N���B
// �߂�l�@�񓯊��������ݗp�̃N���X�B
//------------------------------------------
static COVDataVMAlloc * SerializeLinkArray( const RpiFrameLinkArray &LinkArray)
{
	// LinkArray�́A���g��ł���ŏI�t���[���������Ă��邽�߁A-1����B
	int linksize = int( LinkArray.size()  ) - 1;

	//�������ރt�b�^�G���A�̏����B�Ō�ɔԕ����������߁A�{�P����
	COVDataVMAlloc *ovdf = new COVDataVMAlloc( LinkArray.roundup(szFooteUnit*(linksize+1)) );

	//�t�b�^�̊e�t���[���̏��B�ʒu�E�T�C�Y�E�摜�T�C�Y��ݒ�B
	for( int ii=0; ii<linksize; ii++){
		T_RPI_FOOTER_RECORD *rec = reinterpret_cast<T_RPI_FOOTER_RECORD*>( &ovdf->pBuffer[ii*szFooteUnit] );
		rec->nFilePos = LinkArray[ii].nPosInFile ;
		rec->szFrame  = LinkArray[ii].szFrame;
		rec->szImage  = LinkArray[ii].szImg ;
	}
	{//�ŏI�ɒu���ԕ��B
		T_RPI_FOOTER_RECORD *erec = reinterpret_cast<T_RPI_FOOTER_RECORD*>( &ovdf->pBuffer[linksize*szFooteUnit] );
		erec->nFilePos = -1;
		erec->szFrame  = linksize;
		memcpy( &erec->szImage, "*END", 4 );
	}
	return ovdf;
}

//------------------------------------------
//�����N�����܂Ƃ߂āA�t�b�^�p�ɏ������ށB
// �߂�l�@�t�b�^��u������̃t�@�C���̍ŏI�ʒu�B
//------------------------------------------
int64_t TRpiWriter::write_index_footer()
{
	//���[�B
	if( 0 == Header.theader.nTHeaderSize ){
		this->m_sErrorMsg.Format("�g���w�b�_�������̂Ńt�b�^�͂����܂���B");
		return 0;
	}
	if( LinkArray.size() < 10 )
		return 0;

	//�t�b�^�������݁B
	COVDataVMAlloc *ovdf =	SerializeLinkArray( LinkArray );
	int64_t nPosFooter = LinkArray.back().nPosInFile;
	if( ovdf->IssureWrite( m_hFile, nPosFooter, ovdf->szCapacity ) ){
		ovArray.push_back(ovdf);

		{
			//�s�w�b�_�Ƀt�b�^�J�n�ʒu�ȂǒǋL
			if( Header.theader.nTHeaderSize < sizeof(T_RPI_HEADER_BASE)){
				Header.theader.nTHeaderSize = sizeof(T_RPI_HEADER_BASE);
			}
			Header.theader.nFooterStartPos = nPosFooter;
			Header.theader.szFooterRecord  = szFooteUnit;

			//�ŏI�t���[���́u���ʒu�v��ύX
			modify_last_nextpos(nPosFooter + ovdf->szCapacity);
			return nPosFooter + ovdf->szCapacity;
		}
	}
	else{
		delete ovdf;
	}
	return 0;
}

//------------------------------------------
// RPI�̍Ō�Ƀt���[�������������ہA
// �ŏI�L���t���[���̎w���Ă���u���t���[���ʒu�v��
// �t�@�C���̍Ō�Ɏ����Ă���悤�C������
//  int64_t lastpos �Ō�̃t���[���̐擪�ʒu
//------------------------------------------
void TRpiWriter::modify_last_nextpos(int64_t lastpos)
{
	//�ŏI�t���[���̈ʒu
	int64_t nPosLastFrame =  LinkArray[ LinkArray.size() - 2].nPosInFile;

	//�ŏI�t���[����ǂݍ���
	int nSize = max(512, LinkArray.nSectorAlign );
	COVDataVMAlloc *lastf=new COVDataVMAlloc( nSize );
	if( lastf->IssureRead( m_hFile, nPosLastFrame, nSize) ){
		lastf->WaitResult(m_hFile);

		//�u���t���[���ʒu�v���C������
		put_int64( &lastf->pBuffer[4], lastpos );

		//�������ށB
		lastf->IssureWrite( m_hFile, nPosLastFrame, nSize);
		ovArray.push_back( lastf );
	}
}

//------------------------------------------
//�w�b�_��ۑ�����B
//�@���B��{�I�ɁA�w�b�_�̍X�V��RPI�̃N���[�Y���̎��������ŏ\���̂͂�
//�@�@�@�������ݒ���RPI�t�@�C����G���Ă���Ƃ��A����ȏꍇ�̂݌Ăяo��
//�߂�l�@�r���̏������݂����s�Ȃǂ���΁AFalse�B
//------------------------------------------
bool TRpiWriter::FlushHeader()
{
	//�w�b�_�̃t���[�����́A���ۂɏ������񂾐��ɏC������B
	Header.nframe = static_cast<int>( LinkArray.size() ) - 1;
	int nHeaderSize = prepare_header();

	//�w�b�_����������
	bool ret = m_ovHeader->IssureWrite( m_hFile, 0, LinkArray.roundup(nHeaderSize ) );
	if( ret ) {
		return m_ovHeader->WaitResult( m_hFile ) > 0;
	}
	return false;
}


//------------------------------------------
//�t�@�C�������B�A
// �E�w�b�_�́A�t���[���������ۂ̐��ɕύX��A�X�V�����
// �E�t�b�^�Ƃ��ďW�������N��񂪏������B
//�߂�l�@�r���̏������݂����s�Ȃǂ���΁AFalse�B
//------------------------------------------
bool TRpiWriter::CloseFile()
{
	if(!m_hFile){ 
		this->m_sErrorMsg.Format("�t�@�C���͂������Ă�B");
		return false;
	}

	/*
	bool ret = FlushHeader();
	if( !ret ) {
		this->m_sErrorMsg.Format("�w�b�_�̍ŏI�X�V1�Ɏ��s. ER=%d", GetLastError());
		return false;
	}
	*/

	//�t�b�^�i�W���C���f�b�N�X�j�̏�������
	if( B_WRITE_FOOTER_INDEX ) 	{
		write_index_footer();
	}

	bool ret = FlushHeader();
	if( !ret ) {
		this->m_sErrorMsg.Format("�w�b�_�̍ŏI�X�V2�Ɏ��s. ER=%d", GetLastError());
		return false;
	}
	
	//�������݊�����҂�
	for( size_t ii=0; ii<ovArray.size(); ii++){
		 ret |= (ovArray[ii]->WaitResult(m_hFile) != 0 );
	}

	CloseHandle( m_hFile ) ;
	m_hFile = 0;

	//�������n���̌�n��
	for( size_t ii=0; ii<ovArray.size(); ii++){
		 delete ovArray[ii];
	}
	ovArray.clear();
	LinkArray.clear();

	return ret;
}
};