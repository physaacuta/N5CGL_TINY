#include "StdAfx.h"
#include "KizuAviManager.h"


using namespace ImgLib;


// //////////////////////////////////////////////////////////////////////////////
// ���ϐ��N���X �̃X�^�e�B�b�N�ϐ�������
// //////////////////////////////////////////////////////////////////////////////
KizuAviEnv			KizuAviEnv::m_inst;					// �X�^�e�B�b�N�ϐ�������

//////////////////////////////////////////////////////////////////////
// ���ϐ��N���X
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// �R���X�g���N�^ (exe�̋N�������̂݃R�[��)
//------------------------------------------
KizuAviEnv::KizuAviEnv(void)
{
//	AVIFileInit();
}

//------------------------------------------
// �f�X�g���N�^ (exe�̋N�������̂݃R�[��)
//------------------------------------------
KizuAviEnv::~KizuAviEnv(void)
{
	AVIFileExit(); 
}

// //////////////////////////////////////////////////////////////////////////////
// ���C���N���X
// //////////////////////////////////////////////////////////////////////////////

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
KizuAviManager::KizuAviManager(void) :
m_AviFile(NULL),
m_AviStream(NULL),
m_nW(0),
m_nH(0),
m_nBitCnt(0),
m_nFrameCnt(0)
{
	// �V���O���g���̂��߂Ɉ����s���Ă���
	//KizuAviEnv;
AVIFileInit();
	memset(&m_AviInfo, 0x00, sizeof(m_AviInfo));
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
KizuAviManager::~KizuAviManager(void)
{
}


// //////////////////////////////////////////////////////////////////////////////
// �X�^�e�B�b�N�֐�
// //////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// AVI�t�@�C���̃I�[�v��
// const char* filename AVI�t�@�C���t���p�X
// PAVIFILE& avi_file AVI�t�@�C���̃n���h��
// PAVISTREAM& avi_stream AVI�t�@�C���̃X�g���[��
// AVISTREAMINFO* avi_info AVI�t�@�C���̃w�b�_�[���
//------------------------------------------
bool KizuAviManager::OpenAVIFile(const char* filename, PAVIFILE& avi_file, PAVISTREAM& avi_stream, AVISTREAMINFO* avi_info)
{
    _ASSERT( NULL == avi_file && NULL == avi_stream );

    // open file
    HRESULT hr;
    hr = AVIFileOpen(&avi_file, filename, OF_READ, NULL);
    if( FAILED(hr) ) return false;

    // stream�̎擾
    hr = AVIFileGetStream(avi_file, &avi_stream, 0, 0);
    if( FAILED(hr) ) {
        AVIFileRelease(avi_file);
        avi_file	= NULL;
        avi_stream	= NULL;
        return false;
	}

	// get info
	AVIStreamInfo(avi_stream, avi_info, sizeof(AVISTREAMINFO));

    return true;
}

//------------------------------------------
// AVI�t�@�C���̃N���[�X
// PAVIFILE& avi_file �I�[�v���Ŏ擾����AVI�t�@�C���̃n���h��
// PAVISTREAM& avi_stream �I�[�v���Ŏ擾����AVI�t�@�C���̃X�g���[��
//------------------------------------------
void KizuAviManager::CloseAVIFile(PAVIFILE& avi_file, PAVISTREAM& avi_stream)
{
    _ASSERT( NULL != avi_file && NULL != avi_stream );

    AVIStreamRelease(avi_stream);
    avi_stream = NULL;

    AVIFileRelease(avi_file);
    avi_file = NULL;
}

//------------------------------------------
// AVI�t�@�C���̏��擾
// const char* filename AVI�t�@�C���t���p�X
// DWORD* width ��
// DWORD* height ����
// DWORD* bit_count �P�s�N�Z��������̃r�b�g��
// DWORD* nframes AVI�t�@�C���Ɋ܂܂��t���[����
//------------------------------------------
bool KizuAviManager::GetAVIInfo(const char* filename, DWORD* width, DWORD* height, DWORD* bit_count, DWORD* nframes)
{
    AVISTREAMINFO	info;
    PAVIFILE		file	= NULL;
    PAVISTREAM		stream	= NULL;

	// �I�[�v��
    if( ! OpenAVIFile(filename, file, stream, &info) ) return false;

	// �f�[�^�擾
	*width   = info.rcFrame.right;
	*height  = info.rcFrame.bottom;
	*nframes = info.dwLength;

	// �r�b�g�J�E���g
    *bit_count = 0;
	BITMAPINFO* bminfo = GetAVIFormat(stream);
	if( NULL != bminfo ) {
		*bit_count = bminfo->bmiHeader.biBitCount;
		delete[] bminfo;
	}
    
	// �N���[�Y
    CloseAVIFile(file, stream);
    return false;
}

//------------------------------------------
// AVI�t�@�C���̃t�H�[�}�b�g���擾
// PAVISTREAM& avi_stream �I�[�v���Ŏ擾����AVI�t�@�C���̃X�g���[��
// �߂�l �F �߂�l�͓�����new���Ă���̂ŁA�g���I������� delete[] ���邱��
//------------------------------------------
BITMAPINFO* KizuAviManager::GetAVIFormat(PAVISTREAM avi_stream)
{
    LONG nbyte = 0;
    if( 0 != AVIStreamReadFormat(avi_stream, 0, NULL, &nbyte) ) {
        return NULL;
    }
   
    BITMAPINFO* bminfo = (BITMAPINFO*)new char[nbyte];
    if( 0 != AVIStreamReadFormat(avi_stream, 0, bminfo, &nbyte) ) {
        delete[] bminfo;
        bminfo = NULL;
    }
    return bminfo;
}

// //////////////////////////////////////////////////////////////////////////////
// �ʏ�̊֐�
// //////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// AVI�t�@�C���̃I�[�v��
// const char* filename AVI�t�@�C���t���p�X
//------------------------------------------
bool KizuAviManager::OpenAVIFile(const char* filename)
{
	bool ret;

	// �I�[�v��
	ret = OpenAVIFile(filename, m_AviFile, m_AviStream, &m_AviInfo);
	if( ! ret) return false;	

	// ��{�I�ȏ����擾���Ă���
	m_nW		= m_AviInfo.rcFrame.right;
	m_nH		= m_AviInfo.rcFrame.bottom;
	m_nFrameCnt = m_AviInfo.dwLength;

	// �r�b�g�J�E���g
    m_nBitCnt = 0;
	BITMAPINFO* bminfo = GetAVIFormat(m_AviStream);
	if( NULL != bminfo ) {
		m_nBitCnt = bminfo->bmiHeader.biBitCount;
		delete[] bminfo;
	}

	return true;
}

//------------------------------------------
// AVI�t�@�C���̃N���[�X
//------------------------------------------
void KizuAviManager::CloseAVIFile()
{
	if( NULL == m_AviFile || NULL == m_AviStream) return;

	return CloseAVIFile(m_AviFile, m_AviStream);
}


//------------------------------------------
// AVI�t�@�C���̂P�t���[���ǂݍ���
// long count �Ώۃt���[��No (0�I���W��)
// bool nTB �摜���] true:���] 
// BYTE* img �摜�f�[�^
//------------------------------------------
bool KizuAviManager::LoadFromAVI(long count, BYTE* img, bool nTB) const
{
	DWORD len = m_nW * m_nH;
	HRESULT rc = AVIStreamRead(m_AviStream, count, 1, img, len, NULL, NULL);
	
	// �摜�㉺���]
	if(nTB) HantenTB(m_nW, m_nH, img);

	return SUCCEEDED(rc);
}

//------------------------------------------
// �㉺�𔽓]���Ȃ���R�s�[����
// DWORD nW �摜�� 
// DWORD nH �摜��
// BYTE *p �Ώۉ摜
//------------------------------------------
void KizuAviManager::HantenTB( DWORD nW, DWORD nH, BYTE *p )
{
	// ������Ƃ�������
	DWORD ii, jj;
	DWORD F = 0;
	DWORD T;
	BYTE w;
	DWORD nHend = nH / 2;
	for(ii=0; ii<nHend; ii++) {
		T = (nH-ii-1)*nW;
		for(jj=0; jj<nW; jj++) {
			w    = p[F];
			p[F] = p[T];
			p[T] = w;
			
			F ++;
			T ++;	
		}
	}

/*
	DWORD ii, jj;
	DWORD F = 0;
	DWORD T;
	BYTE w;
	DWORD nHend = nH / 2;
	for(ii=0; ii<nHend; ii++) {
		for(jj=0; jj<nW; jj++) {
			T = (nH-ii-1)*nW + jj;

			w    = p[F];
			p[F] = p[T];
			p[T] = w;
			
			F ++;
		}
	}
*/
}
