#include "StdAfx.h"
#include "LacManager.h"
#include <exception>

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// �R���X�g���N�^
// int nMode ���[�h (-1:DLL�Ǎ����� 0:LAC 1:NUC 2:LAC5)
//------------------------------------------
LacManager::LacManager(int nMode)
	:m_pCodec(0), m_pfLac2Destroy(0), m_nThrId(-1),
	m_bIsGpuEncoderExist(false), m_nEncoderWidth(0), m_nEncoderHeight(0), m_nEncoderQuality(0)
{
	m_hInst[0] = m_hInst[1] = 0;

	UnLoad();
    
	// �Ǎ�
	if(-1 != nMode) {
		Load(nMode);
	}
}
//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
LacManager::~LacManager(void)
{
	UnLoad();
}


//////////////////////////////////////////////////////////////////////
// DLL���[�h

enum {
    DLL_ENC,  //�G���R�[�h�pDLL
    DLL_DEC,  //�f�R�[�h�pDLL
    DLL_OLD   //��������DLL
};
    

//------------------------------------------
// DLL���[�h
// int nVer ���[�h (0:LAC 1:NUC 2:LAC5)
//------------------------------------------
bool LacManager::Load(int nMode)
{
	//// DLL�C���|�[�g
	m_hInst[DLL_ENC] = LoadLibrary( DllName(nMode, DLL_ENC) );
	m_hInst[DLL_DEC] = LoadLibrary( DllName(nMode, DLL_DEC) );

	if(!m_hInst[0] && !m_hInst[1]) {
        UnLoad();
        
        // ��DLL�͖����݂����Ȃ̂ŁA���ʂ���݂Ȃ����B
        m_hInst[DLL_ENC] = LoadLibrary( DllName(nMode, DLL_OLD) );
        m_hInst[DLL_DEC] = LoadLibrary( DllName(nMode, DLL_OLD) );

        if( !m_hInst[0] ) {
            // ����DLL������
            return false;
        }
	}
    
    if(nMode == EM_DLL_TYPE::DLL_TYPE_LAC)
    {
		// LAC�̊֐��Q
        m_pfLacDecode = (typfLacDecode)(::GetProcAddress(m_hInst[DLL_DEC], "LacDecode"));
        
        //*********���k�E�W�J����DLL�Ȃ�A���k��Float�ŁA�Ƃ�������
        if( m_hInst[0] != m_hInst[1] ){
            m_pfLacEncode_f = (typfLacEncode_f)(::GetProcAddress(m_hInst[DLL_ENC], "LacEncode"));
        }
        else{
            m_pfLacEncode = (typfLacEncode)(::GetProcAddress(m_hInst[DLL_ENC], "LacEncode"));
        }
        m_nLoadType = nMode ;
    }
    else if(nMode == EM_DLL_TYPE::DLL_TYPE_NUC)
	{
		// NUC�̊֐��Q
		m_pfLacDecode = (typfLacDecode)(::GetProcAddress(m_hInst[DLL_DEC], "Lac2Decode"));

        m_pfLac2Create  =(typfLac2Create)::GetProcAddress(m_hInst[DLL_ENC], "Lac2Create");
        m_pfLac2Destroy =(typfLac2Destroy)::GetProcAddress(m_hInst[DLL_ENC], "Lac2Destroy");
        m_pfLac2Encode = (typfLac2Encode )::GetProcAddress(m_hInst[DLL_ENC], "Lac2Encode");
        m_nLoadType = nMode ;
    }
	else {
		// LAC5�̊֐��Q
		m_pfGpuLacEncoderCreate		= (gpulac5_encoder_create_t )::GetProcAddress(m_hInst[DLL_ENC], "gpulac5_encoder_create");
		m_pfGpuLacEncode			= (gpulac5_encoder_encode_t )::GetProcAddress(m_hInst[DLL_ENC], "gpulac5_encoder_encode");
		m_pfGpuLacEncoderDestroy	= (gpulac5_encoder_destroy_t )::GetProcAddress(m_hInst[DLL_ENC], "gpulac5_encoder_destroy");
		m_pfGpuLacDeviceCount		= (gpulac5_get_cuda_device_count_t)::GetProcAddress(m_hInst[DLL_ENC], "gpulac5_get_cuda_device_count");
		m_pfGpuLacDeviceUuid		= (gpulac5_get_cuda_device_uuid_t)::GetProcAddress(m_hInst[DLL_ENC], "gpulac5_get_cuda_device_uuid");
		m_pfGpuLacDeviceLicense		= (gpulac5_get_cuda_device_license_t)::GetProcAddress(m_hInst[DLL_ENC], "gpulac5_get_cuda_device_license");

		m_pfGpuLacDecoderSize		= (cpulac5_decoder_get_image_size_t)::GetProcAddress(m_hInst[DLL_DEC], "cpulac5_decoder_get_image_size");
		m_pfGpuLacDecode			= (cpulac5_decoder_decode_t )::GetProcAddress(m_hInst[DLL_DEC], "cpulac5_decoder_decode");

		m_pfMallocHost				= (gpulac5_malloc_host_t)::GetProcAddress(m_hInst[DLL_ENC], "gpulac5_malloc_host");
		m_pfFreeHost				= (gpulac5_free_host_t)::GetProcAddress(m_hInst[DLL_ENC], "gpulac5_free_host");

		// �G���R�[�_��DLL������ꍇ�́AGPU��LAC���g�p�\���m�F
		if (m_hInst[0]) {
			int nDevCnt = GetDeviceCnt();	// PC���CUDA�f�o�C�X�̐�

			if (0 == nDevCnt)
			{
				UnLoad();
				return false;
			}

			for (int ii = 0; ii < nDevCnt; ii++)
			{
				if (!IsHaveLacLicense(ii))		// ���C�Z���X�L���`�F�b�N
				{
					UnLoad();
					return false;
				}
			}
		}


	}
    return true;
}

//------------------------------------------
//DLL�̖��̂��擾����B
// int nMode ���[�h (-1:DLL�Ǎ����� 0:LAC 1:NUC 2:LAC5)
// int nEncode: DLL�̃G�f�B�V�����B�@0:���ʁA1:���k�̂݁A2:�W�J�̂�
//------------------------------------------
const char *LacManager::DllName( int nMode, int nEncode )
{
    static const char sArch[]  = ""
		#ifdef _M_X64			// x64 �R���p�C��
            "x64.dll"
		#else
            "x86.dll"
		#endif
    ;

    const char *sDllBase;

    if( nMode == EM_DLL_TYPE::DLL_TYPE_NUC ){
        sDllBase = "Lac2Dll";
    }
    else if( nMode == EM_DLL_TYPE::DLL_TYPE_LAC5 ){
		switch (nEncode) {
		case DLL_ENC: sDllBase = "Lac5Enc"; break;
		case DLL_DEC: sDllBase = "Lac5Dec"; break;
		case DLL_OLD: sDllBase = "Lac5Dll"; break;
		default:sDllBase = "Lac5Dll"; break;
		}
	}
    else{
        switch( nEncode ) {
        case DLL_ENC: sDllBase = "LacEnc"; break;
        case DLL_DEC: sDllBase = "LacDec"; break;
        case DLL_OLD: sDllBase = "LacDll"; break;
        default:sDllBase = "LacDll"; break; 
        }
    }
    
    wsprintfA( mcDllName, "%s%s", sDllBase,  sArch );
    
	return mcDllName;
}

//------------------------------------------
// DLL���
//------------------------------------------
void LacManager::UnLoad()
{
    //�G���R�[�_�C���X�^���X������Ύn������
    if( m_pCodec && m_pfLac2Destroy)
    {
		// NUC
        m_pfLac2Destroy(m_pCodec);
        m_pCodec = 0;
    }
	if (IsGpuEncoder()) {
		// LAC5
		DestroyEncoder();
	}

    //DLL���������
    if( m_hInst[0] ){
		FreeLibrary( m_hInst[0] );
        m_hInst[0] = 0;
	}
	if( m_hInst[1] ){
		FreeLibrary( m_hInst[1] );
        m_hInst[1] = 0;
	}

    // DLL�̊֐��|�C���^�A�����N���A����
    m_pfLacDecode=0;
    
	m_pfLacEncode_f=0;
    m_pfLacEncode=0;

    //NUC�̃G���R�[�h
    m_pfLac2Create =0;
    m_pfLac2Destroy=0;
    m_pfLac2Encode =0;
    
	// LAC5�̃G���R�[�h
	m_pfGpuLacEncoderCreate = 0;
	m_pfGpuLacEncoderDestroy = 0;
	m_pfGpuLacEncode = 0;
	m_pfMallocHost = 0;
	m_pfFreeHost = 0;

    m_nLoadType = -1 ;
	
	m_nCodecHeight = 0;
	m_nCodecWidth = 0;
}

// NUC�̏ꍇ��Encoder�̏������B
// width  : ���k�摜�̕�
// height : ���k�摜�̍�
bool  LacManager::InitEncoder( int nWidth, int nHeight )
{
    if( m_pfLac2Create ){
        if( m_pCodec )
        {
			//�p�����^�������Ȃ�A�s�v
			if( m_nCodecWidth == nWidth && m_nCodecHeight == nHeight ){
				return true;
			}

            m_pfLac2Destroy(m_pCodec);
            m_pCodec = 0;
        }

		//�G���R�[�_�C���X�^���X�쐬
		try{
			Lac2CreateParams params = { nWidth, nHeight, 1 };
			m_pCodec = m_pfLac2Create( &params );
			if( NULL != m_pCodec ){
				m_nCodecWidth  = nWidth;
				m_nCodecHeight = nHeight;
				return true;
			}
		}catch( std::bad_alloc &be ){
			return false;
		}
		return false;
    }
    return true;
}

// LAC5�̏ꍇ��Encoder�̏������B
// nWidth   : ���k�摜�̕�
// nHeight  : ���k�摜�̍�
// nQuality : �掿�p�����[�^�l
int  LacManager::InitEncoder(int nWidth, int nHeight, int nQuality)
{
	// �������`�F�b�N
	if (40 > nQuality || 100 < nQuality)	return -2;		// �����ُ�

	if (m_pfGpuLacEncoderCreate) {
		if (nHeight != GetGpuEncoderH() ||
			nWidth != GetGpuEncoderW() ||
			nQuality != GetGpuEncoderQ())
		{
			// ���A�����A�掿�p�����[�^���ύX����Ă���ꍇ�́A�G���R�[�_����

			DestroyEncoder();										// �G���R�[�_���

			int nRet = CreateEncoder(nWidth, nHeight, nQuality);	// �G���R�[�_����
			return nRet;
		}
		if(IsGpuEncoder()) return 0;								// ������
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////
// ��{

//------------------------------------------
// BMP��LAC (���k��{)
// LacData *src			���͏��
// LacData *dst			�o�͏��
// float dRate			���k��:1<���掿>�`   �i��:-1�`-100<���掿>
//------------------------------------------
LacManager::LacStatus LacManager::doLacEncode(LacData *src, LacData *dst, float dRate)
{
	// �������`�F�b�N
	if( 0 == dRate || -100 > dRate)	return LacStatus_ParamError;		// �����ُ�

	// ���k
    if( m_pfLac2Encode )
    {
		if( !m_pCodec ) return LacStatus_OtherErr;
        return m_pfLac2Encode( m_pCodec, src, dst, (int)(dRate) );
    }
    else if( m_pfLacEncode_f ){
        return m_pfLacEncode_f(src, dst, dRate);
    }
    else if( m_pfLacEncode ){
        return m_pfLacEncode(src, dst, (int)(dRate) );
    }
    else{
        return LacStatus_OtherErr;			// DLL���[�h����?
    }
}

//------------------------------------------
// LAC��BMP (�𓀊�{)
// LacData *src			���͏��
// LacData *dst			�o�͏��
//------------------------------------------
LacManager::LacStatus LacManager::doLacDecode(LacData *src, LacData *dst)
{
	// ��
    if( m_pfLacDecode ){
		return m_pfLacDecode(src, dst);
    }
    else{
        return LacStatus_OtherErr;			// DLL���[�h����?
    }
}


//////////////////////////////////////////////////////////////////////
// �g���₷���悤�� ���k

//------------------------------------------
// BMP��LAC (mem��mem)
// float dRate			���k��:1<���掿>�`   �i��:-1�`-100<���掿>
// int nInSize			���̓f�[�^�T�C�Y
// BYTE const* pInImg	�摜�f�[�^
// int nMemSize			�Ăяo�����Ŋm�ۂ����̈�T�C�Y
// BYTE* pOutImg		LAC�f�[�^�̈� (�Ăяo�����ŗ̈�m��)
// int* nOutSize		���k��̃T�C�Y (in/out)
//------------------------------------------
LacManager::LacStatus LacManager::LacEncode(float dRate, int nInSize, BYTE const* pInImg, int nMemSize, BYTE* pOutImg, int* nOutSize)
{
	LacData				src, dst;

	src.type			= LacDataType_MemBlock;
	src.ptr				= (void*)pInImg;
	src.nbytes			= nInSize;

	dst.type			= LacDataType_MemBlock;
	dst.ptr				= (void*)pOutImg;
	dst.nbytes			= nMemSize;	
	
	LacStatus emRetc	= doLacEncode(&src, &dst, dRate);
	
	*nOutSize			= dst.nbytes;
	return emRetc;
}

// BMP��LAC (mem��file)
LacManager::LacStatus LacManager::LacEncode(float dRate, int nInSize, BYTE const* pInImg, char const* cOutPath)
{
	LacData				src, dst;

	src.type			= LacDataType_MemBlock;
	src.ptr				= (void*)pInImg;
	src.nbytes			= nInSize;

	dst.type			= LacDataType_FileName;
	dst.ptr				= (void*)cOutPath;
	dst.nbytes			= 0;
	
	return doLacEncode(&src, &dst, dRate);
}

// BMP��LAC (file��mem)
LacManager::LacStatus LacManager::LacEncode(float dRate, char const* cInPath, int nMemSize, BYTE* pOutImg, int* nOutSize)
{
	LacData				src, dst;

	src.type			= LacDataType_FileName;
	src.ptr				= (void*)cInPath;
	src.nbytes			= 0;

	dst.type			= LacDataType_MemBlock;
	dst.ptr				= (void*)pOutImg;
	dst.nbytes			= nMemSize;	
	
	LacStatus emRetc	= doLacEncode(&src, &dst, dRate);
	
	*nOutSize			= dst.nbytes;
	return emRetc;
}
// BMP��LAC (file��file)
LacManager::LacStatus LacManager::LacEncode(float dRate, char const* cInPath, char const* cOutPath)
{
	LacData				src, dst;

	src.type			= LacDataType_FileName;
	src.ptr				= (void*)cInPath;
	src.nbytes			= 0;

	dst.type			= LacDataType_FileName;
	dst.ptr				= (void*)cOutPath;
	dst.nbytes			= 0;
	
	return doLacEncode(&src, &dst, dRate);
}

//////////////////////////////////////////////////////////////////////
// �g���₷���悤�� ��

//------------------------------------------
// LAC��BMP (mem��mem)
// int nInSize			���̓f�[�^�T�C�Y
// BYTE const* pInImg	�摜�f�[�^
// int nMemSize			�Ăяo�����Ŋm�ۂ����̈�T�C�Y
// BYTE* pOutImg		BMP�f�[�^�̈� (�Ăяo�����ŗ̈�m��)
// int* nOutSize		�𓀌�̃T�C�Y (in/out)
//------------------------------------------
LacManager::LacStatus LacManager::LacDecode(int nInSize, BYTE const* pInImg, int nMemSize, BYTE* pOutImg, int* nOutSize)
{
	LacData				src, dst;

	src.type			= LacDataType_MemBlock;
	src.ptr				= (void*)pInImg;
	src.nbytes			= nInSize;

	dst.type			= LacDataType_MemBlock;
	dst.ptr				= (void*)pOutImg;
	dst.nbytes			= nMemSize;	
	
	LacStatus emRetc	= doLacDecode(&src, &dst);
	
	*nOutSize			= dst.nbytes;
	return emRetc;
}
// LAC��BMP (mem��file)
LacManager::LacStatus LacManager::LacDecode(int nInSize, BYTE const* pInImg, char const* cOutPath)
{
	LacData				src, dst;

	src.type			= LacDataType_MemBlock;
	src.ptr				= (void*)pInImg;
	src.nbytes			= nInSize;

	dst.type			= LacDataType_FileName;
	dst.ptr				= (void*)cOutPath;
	dst.nbytes			= 0;	
	
	return doLacDecode(&src, &dst);
}
// LAC��BMP (file��mem)
LacManager::LacStatus LacManager::LacDecode(char const* cInPath, int nMemSize, BYTE* pOutImg, int* nOutSize)
{
	LacData				src, dst;

	src.type			= LacDataType_FileName;
	src.ptr				= (void*)cInPath;
	src.nbytes			= 0;

	dst.type			= LacDataType_MemBlock;
	dst.ptr				= (void*)pOutImg;
	dst.nbytes			= nMemSize;	
	
	LacStatus emRetc	= doLacDecode(&src, &dst);
	
	*nOutSize			= dst.nbytes;
	return emRetc;
}
// LAC��BMP (file��file)
LacManager::LacStatus LacManager::LacDecode(char const* cInPath, char const* cOutPath)
{
	LacData				src, dst;

	src.type			= LacDataType_FileName;
	src.ptr				= (void*)cInPath;
	src.nbytes			= 0;

	dst.type			= LacDataType_FileName;
	dst.ptr				= (void*)cOutPath;
	dst.nbytes			= 0;	
	
	return doLacDecode(&src, &dst);
}

//------------------------------------------------------------------------------------
// GPU��LAC(LAC5) �֐��Q
//------------------------------------------------------------------------------------

//------------------------------------------
// BMP��LAC (���k��{)
// BYTE* pInImg			���͉摜�f�[�^
// int   nInSize		���͉摜�T�C�Y
// BYTE* pOutImg		�o�͉摜�f�[�^
// int*  nOutSize		�o�͉摜�T�C�Y
// �߂�l: =0:����  !=0:�ُ�
//------------------------------------------
int LacManager::doLacEncodeGpu(BYTE const* pInImg, int nInSize, BYTE* pOutImg, int* nOutSize)
{
	// ���k
	if (m_pfGpuLacEncode)
	{
		if (!IsGpuEncoder()) return LacStatus_OtherErr;
		return (LacStatus)DoGpuEncode((BYTE*)pInImg, nInSize, (BYTE*)pOutImg, nOutSize);
	}
	else {
		return LacStatus_OtherErr;			// DLL���[�h����?
	}
}

//------------------------------------------
// LAC��BMP (�𓀊�{)
// BYTE* pInImg			���͉摜�f�[�^
// int   nInSize		���͉摜�T�C�Y
// BYTE* pOutImg		�o�͉摜�f�[�^
// �߂�l: =0:����  !=0:�ُ�
//------------------------------------------
int LacManager::doLacDecodeGpu(BYTE const* pInImg, int nInSize, BYTE* pOutImg)
{
	// ��
	if (m_pfGpuLacDecode) {
		return (LacStatus)m_pfGpuLacDecode((uint8_t*)pInImg, nInSize, (uint8_t*)pOutImg);
	}
	else {
		return LacStatus_OtherErr;			// DLL���[�h����?
	}
}


//////////////////////////////////////////////////////////////////////
// �g���₷���悤�� ���k

//------------------------------------------
// BMP��LAC (mem��mem)
// BYTE* pInImg			���͉摜�f�[�^
// int   nInSize		���͉摜�T�C�Y
// BYTE* pOutImg		�o�͉摜�f�[�^
// int*  nOutSize		�o�͉摜�T�C�Y
// �߂�l: =0:����  !=0:�ُ�
//------------------------------------------
int LacManager::LacEncodeGpu(BYTE const* pInImg, int nInSize, BYTE* pOutImg, int* nOutSize)
{
	int nRetc = doLacEncodeGpu((BYTE*)pInImg, nInSize, (BYTE*)pOutImg, nOutSize);

	return nRetc;
}


//////////////////////////////////////////////////////////////////////
// �g���₷���悤�� ��

//------------------------------------------
// LAC��BMP (mem��mem)
// BYTE* pInImg			���͉摜�f�[�^
// int   nInSize		���͉摜�T�C�Y
// BYTE* pOutImg		�o�͉摜�f�[�^
// int*  nOutSize		�o�͉摜�T�C�Y
// �߂�l: =0:����  !=0:�ُ�
//------------------------------------------
int LacManager::LacDecodeGpu(BYTE const* pInImg, int nInSize, BYTE* pOutImg)
{
	int nRetc = doLacDecodeGpu(pInImg, nInSize, pOutImg);

	return nRetc;
}

//------------------------------------------
// CUDA�f�o�C�X���̃`�F�b�N
// �߂�l:CUDA�f�o�C�X��
//------------------------------------------
int LacManager::GetDeviceCnt()
{
	int nRetc = -1;
	int nCount = 0;
	nRetc = m_pfGpuLacDeviceCount(&nCount);

	return nCount;
}

//------------------------------------------
// CUDA�f�o�C�X��UUID�擾
// int nDevID			CUDA�f�o�C�XID(0����n�܂�)
// �߂�l:UUID
//------------------------------------------
CString	LacManager::GetDeviceUUID(int nDevID)
{
	CString sWk;
	char	cWk[80];
	memset(cWk, 0x00, 80);

	m_pfGpuLacDeviceUuid(nDevID, cWk);

	sWk.Format("%s", cWk);

	return sWk;
}

//------------------------------------------
// CUDA�f�o�C�X��LAC5���C�Z���X�L���`�F�b�N
// int nDevID			CUDA�f�o�C�XID(0����n�܂�)
// �߂�l:true:���C�Z���X�L�� false:���C�Z���X����
//------------------------------------------
bool LacManager::IsHaveLacLicense(int nDevID)
{
	int nRet = m_pfGpuLacDeviceLicense(nDevID);

	return (1 == nRet ? true : false);
}

//------------------------------------------
// LAC5 �G���R�[�_����
// int nWidth			�񈳏k�摜�̕�[pixel]
// int nHeight			�񈳏k�摜�̍���[pixel]
// int nQuality			�掿�p�����[�^
// �߂�l: =0:����  !=0:�ُ�
//------------------------------------------
int LacManager::CreateEncoder(int nWidth, int nHeight, int nQuality)
{
	int nDevID = m_nThrId;		// CUDA�f�o�C�XID(0����n�܂�)

	int nRet = m_pfGpuLacEncoderCreate(nDevID, nWidth, nHeight, nQuality, &m_GpuEncoder);

	if (0 == nRet)
	{
		m_bIsGpuEncoderExist = true;
		m_nEncoderWidth = nWidth;
		m_nEncoderHeight = nHeight;
		m_nEncoderQuality = nQuality;
	}

	return nRet;
}

//------------------------------------------
// LAC5 �G���R�[�_�j��
// �߂�l:true:���� false:�ُ�
//------------------------------------------
bool LacManager::DestroyEncoder()
{
	if (!m_bIsGpuEncoderExist)	return true;

	int nRet = m_pfGpuLacEncoderDestroy(m_GpuEncoder);

	if (0 == nRet)
	{
		m_bIsGpuEncoderExist = false;
		m_nEncoderWidth = 0;
		m_nEncoderHeight = 0;
		m_nEncoderQuality = 0;
	}


	return (0 == nRet ? true : false);
}

//------------------------------------------
// LAC5 �G���R�[�h���s
// BYTE* pInImg			���͉摜�f�[�^
// int   nInSize		���͉摜�T�C�Y
// BYTE* pOutImg		�o�͉摜�f�[�^
// int*  nOutSize		�o�͉摜�T�C�Y
// �߂�l: =0:����  !=0:�ُ�
//------------------------------------------
int LacManager::DoGpuEncode(BYTE const* pInImg, int nInSize, BYTE* pOutImg, int* nOutSize)
{
	uint8_t*	original_image = (uint8_t*)pInImg;
	uint8_t*	compressed_image = NULL;	// (uint8_t*)pOutImg;		// �ԋp�͕ϐ��ւ̃|�C���^
	int			compressed_image_size = 0;

	int nRet = m_pfGpuLacEncode(m_GpuEncoder, original_image, &compressed_image, &compressed_image_size);
	// GPU��LAC�͈��k��f�[�^�̕ԋp�G���A���G���R�[�_���Ŋm�ۂ��Ă��邽�߁A
	// �o�͉摜�f�[�^�ɂ́A�f�[�^�R�s�[���ĕԋp����K�v������
	memcpy(pOutImg, compressed_image, compressed_image_size);

	*nOutSize = compressed_image_size;

	return nRet;
}

//------------------------------------------
// LAC5 �������m��
// size_t  nSize		�v�����ꂽ���蓖�ăT�C�Y(�o�C�g�P��)
// �߂�l: ���������ꍇ�A���蓖�Ă�ꂽ�������ւ̃|�C���^
//------------------------------------------
void* LacManager::GpuMallocHost(size_t nSize)
{
	if (!m_pfMallocHost) return NULL;
	return (BYTE*)m_pfMallocHost(nSize);
	//return (BYTE*)malloc(nSize);
}

//------------------------------------------
// LAC5 �������J��
// void*  ptr		���������J������|�C���^
//------------------------------------------
void LacManager::GpuFreeHost(void* ptr)
{
	if (NULL == ptr) return;
	if (!m_pfFreeHost) return;
	return m_pfFreeHost(ptr);
	//return free(ptr);
}
