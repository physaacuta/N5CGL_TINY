// *********************************************************************************
//	�g���`����RPI�t�@�C�����������ރN���X�B
//	[Ver]
//		Ver.01    2014/09/15  
//
//	[�����E�g����]
//�@�@�@1.�R���X�g���N�g�B
//�@�@�@2.�w�b�_��p�ӂ��āAOpenFile�ŏo�̓t�@�C�����쐬
//�@�@�@3.WriteFrameOpt,WriteFrameCopy�ȂǂŃt���[�����o��
//�@�@�@4.�w�b�_���X�V���ACloseFile�Ńt�@�C�������
//
// *********************************************************************************
#pragma once

namespace RpiLib
{

//
class TRpiWriter
{
protected:
	CString m_sErrorMsg;				//�Ō�̃G���[���i�[���Ă���B

	struct  MemoryDataVMAlloc *m_ovHeader;	//�������ݗp�w�b�_�i�Z�N�^�A���C���j�B
	CString m_sFilename;


	int prepare_header();				//�\���̃w�b�_->�������ݗp�o�C�g�w�b�_�ցB

	void initLinkArray();				//�������ݗp�����N���̏�����
	void modify_last_nextpos(int64_t lastpos);
	int64_t write_index_footer();

	RPIFrameLink &PrepareFrameOpt( BYTE *frame_image, int image_size, const void *t_frameinf );

public:
	RpiHeader Header;					//�w�b�_�B�\���̂Ƃ��Ď�舵���p
	T_RPI_HEADER_BASE &theader;			//�g���w�b�_�B Header.theader�̕ʖ�
	RpiFrameLinkArray LinkArray;		//�����N���B���ƂŏW���C���f�b�N�X�t�b�^���������߂Ȃ�

	TRpiWriter();
	virtual ~TRpiWriter();

	//�t�@�C���I�[�v���B
	// bDirect:FILE_FLAG_NO_BUFFERING�����邩�ǂ����B
	bool OpenFile ( const CString &filename, const RpiHeader &ah, bool bDirect );
    
	bool IsOpen() const { return 0 != m_hFile; }

	const char *GetFileName() const { return m_sFilename ; } 

	HANDLE GetHandle() { return m_hFile; }
	
	//�����B�t���[���������݁Bframe_imgae�́A12�o�C�g�J���Ă������ƁB512�o�C�g�A���C������Ă��邱�ƁB
	int WriteFrameOpt( BYTE *frame_image, int image_size, const void  *t_frameinf=NULL);

	//�����B�t���[���������݁Bimage���A���C������Ă��Ȃ��ꍇ��A12�o�C�g�̐擪�󔒂������ꍇ�͂�����B
	int WriteFrameCopy( const BYTE *image, int image_size, const void *t_frameinf=NULL);
    

	//���ݏ������ݍς݂̃t���[������Ԃ�
	int GetFrameNum() const {  return static_cast<int>( LinkArray.size() ) - 1; } 
    

	//RPI�t�@�C���̃w�b�_�������I�ɍX�V����
	bool FlushHeader();


	//�t�@�C�������
	bool CloseFile();

	//�Ȃ񂩃��\�b�h���G���[�ɂȂ����ꍇ(false���߂��Ă�����)
	//�����ɃG���[�̏ڍׂ��l�܂��Ă��邩������Ȃ��B
	const char *GetError() const {  return m_sErrorMsg; } 


protected:
	HANDLE m_hFile;						//�������ݗp�t�@�C���n���h��
	CRITICAL_SECTION m_csFile;			//�t�@�C���n���h���̕ی�p
	int WriteFileAt( int64_t nPos, const void *pMem, int nSize );	//��������
	int ReadFileAt ( int64_t nPos, void *pMem, int nSize );			//�ǂݍ��݁B

};
};