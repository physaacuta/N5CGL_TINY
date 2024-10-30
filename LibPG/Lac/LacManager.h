// *********************************************************************************
//	���x�摜���k�N���X
//	[Ver]
//		Ver.01    2011/02/20  vs2005 �Ή�
//		Ver.02    2014/11/04  Lac��DLL�����ɑΉ�
//
//	[����]
//		EXE�Ɠ���t�H���_�� LacDLL.dll �����Ă�������		LAC
//		EXE�Ɠ���t�H���_�� Lac2DLL.dll �����Ă�������	NUC
// *********************************************************************************

#pragma once

//#include <windows.h>
#include <stdint.h>

class LacManager
{
	
public:
	// ���A���
	typedef enum
	{
		LacStatus_Ok,						// 0:����
		LacStatus_OutOfMemory,				// 1:�������s�� �����Ԃ�PC�̃������̈���ۂ�
		LacStatus_ParamError,				// 2:�p�����[�^�w�肪�ُ�
		LacStatus_IoFileOpenFail,			// 3:�t�@�C���I�[�v�����s
		LacStatus_IoFileInvalidFormat,		// 4:�I�[�v���͐����������A�t�@�C�����̂����Ă�(=�w�b�_�[���ρH)
		LacStatus_IoGeneralError,			// 5:���̑�I/O�֌W�ُ̈�
		LacStatus_InternalError,			// 6:LAC�R�[�f�B�b�N���̂̓����ُ�
		LacStatus_LicenseError,				// 7:���C�Z���X�G���[


		LacStatus_OtherErr = -1				// ���̃N���X�����łُ̈�
	} LacStatus;

	// ���o�͌`��
	typedef enum
	{
		LacDataType_FileName,				// �t�@�C���󂯓n����
		LacDataType_MemBlock				// �������󂯓n����
	} LacDataType;

	// ���o�͏��
	typedef struct  
	{
		LacDataType		type;				// ���o�͌`��
		void			*ptr;				// �p�����[�^ (�t�@�C���n�����F�t�@�C���p�X�B   �������n�����F�������|�C���^�[(�̈�́A�Ăь��Ŋm��))
		int				nbytes;				// �p�����[�^���������n�����̂ݗL���B �������ɃZ�b�g�����l
												// ���������͎�(in�̂�) : in:���̓f�[�^�̃T�C�Y
												// �������o�͎�(in/out) : in:�o�͗p�������̗̈�m�ۂ����T�C�Y  out:�ϊ����LAC���Z�b�g�����T�C�Y 
	} LacData;


	typedef struct Lac2CodecDummy *PLac2Codec; // NUC�G���R�[�_�C���X�^���X
	struct  Lac2CreateParams
	{
	   int          width;                  // �摜�̕�
	   int          height;                 // �摜�̍�
	   int          roicnt;                 // ���k�Ɏg�p����X���b�h��
	};

	//// �摜���k�^�C�v
	enum EM_DLL_TYPE
	{
		DLL_TYPE_LAC = 0,												// LAC���k (���x���k�Z�p)
		DLL_TYPE_NUC,													// NUC���k (���ǔō��x���k�Z�p)
		DLL_TYPE_LAC5													// LAC5���k (GPU�ō��x���k�Z�p)
	};

protected:
    
	// DLL���̃t�@���N�V�����|�C���^
    // LAC/NUC�̃f�R�[�h
	typedef LacStatus (*typfLacDecode)(LacData *src, LacData *dst);
    
    // LAC�̃G���R�[�h�BVER 2.05�ȍ~�͏㑤(���k����float)
	typedef LacStatus (*typfLacEncode_f)(LacData *src, LacData *dst, float rate);	
	typedef LacStatus (*typfLacEncode)(LacData *src, LacData *dst, int rate);

    // NUC�̃G���R�[�h�B�G���R�[�_�C���X�^���X�쐬�E�폜���܂ށB
    typedef PLac2Codec (*typfLac2Create)(Lac2CreateParams *params);
	typedef void       (*typfLac2Destroy)(PLac2Codec enc);
	typedef LacStatus  (*typfLac2Encode)(PLac2Codec enc, LacData *src,LacData *dst, int rate);

    // LAC5�̃G���R�[�h�B�G���R�[�_�C���X�^���X�쐬�E�폜���܂ށB
	struct GpuLac5EncoderImplND;

	typedef GpuLac5EncoderImplND* gpulac5_encoder;

    typedef int        (*gpulac5_encoder_create_t)(int device_id, int width, int height, int quality, gpulac5_encoder* encoder);
	typedef int        (*gpulac5_encoder_encode_t)(gpulac5_encoder encoder, uint8_t* original_image, uint8_t** compressed_image, int* compressed_image_size);
	typedef int        (*gpulac5_encoder_destroy_t)(gpulac5_encoder encoder);
	typedef int        (*gpulac5_get_cuda_device_uuid_t)(int device_id, char* uuid);
	typedef int        (*gpulac5_get_cuda_device_count_t)(int* count);
	typedef int        (*gpulac5_get_cuda_device_license_t)(int device_id);
	typedef void*      (*gpulac5_malloc_host_t)(size_t size);
	typedef void       (*gpulac5_free_host_t)(void *ptr);

	typedef int        (*cpulac5_decoder_get_image_size_t)(uint8_t* compressed, int compressed_size, int* width, int* height);
	typedef int        (*cpulac5_decoder_decode_t)(uint8_t *compressed, int compressed_size, uint8_t *decompressed_image);

public:
	LacManager(int nMode = 0);
	virtual ~LacManager(void);
	virtual bool	Load(int nMode);			// DLL���[�h
    virtual void	UnLoad();					// DLL���
	int		IsLoadType() { return m_nLoadType; }// DLL���[�h���

	//// ��{
    virtual bool  InitEncoder( int nWidth, int nHeight );                                                       // NUC�̏ꍇ�̈��k�O�������B
    virtual int   InitEncoder( int nWidth, int nHeight, int nQuality );                                         // GPU��LAC(LAC5)�̏ꍇ�̈��k�O�������B
	virtual LacStatus doLacEncode(LacData *src, LacData *dst, float dRate);										// BMP��LAC (���k��{)
    virtual LacStatus doLacDecode(LacData *src, LacData *dst);													// LAC��BMP (�𓀊�{)
	virtual int doLacEncodeGpu(BYTE const* pInImg, int nInSize, BYTE* pOutImg, int* nOutSize);					// BMP��LAC (���k��{) GPU��LAC(LAC5)
    virtual int doLacDecodeGpu(BYTE const* pInImg, int nInSize, BYTE* pOutImg);									// LAC��BMP (�𓀊�{) GPU��LAC(LAC5)

	//// ���k �g���₷���悤��
	LacStatus LacEncode(float dRate, int nInSize, BYTE const* pInImg, int nMemSize, BYTE* pOutImg, int* nOutSize);	// BMP��LAC (mem��mem)
	LacStatus LacEncode(float dRate, int nInSize, BYTE const* pInImg, char const* cOutPath);				// BMP��LAC (mem��file)
	LacStatus LacEncode(float dRate, char const* cInPath, int nMemSize, BYTE* pOutImg, int* nOutSize);	    // BMP��LAC (file��mem)
	LacStatus LacEncode(float dRate, char const* cInPath, char const* cOutPath);							// BMP��LAC (file��file)
	int LacEncodeGpu(BYTE const* pInImg, int nInSize, BYTE* pOutImg, int* nOutSize);						// BMP��LAC (mem��mem) GPU��LAC(LAC5)

	//// ���k �g���₷���悤�Ɂi���ŁB���k����int�B�����́Afloat�ł֊ۓ���)
	LacStatus LacEncode(int nRate, int nInSize, BYTE const* pInImg, int nMemSize, BYTE* pOutImg, int* nOutSize)	// BMP��LAC (mem��mem)
    {return LacEncode( static_cast<float>(nRate), nInSize, pInImg,  nMemSize,  pOutImg, nOutSize); }

    LacStatus LacEncode(int nRate, int nInSize, BYTE const* pInImg, char const* cOutPath)				// BMP��LAC (mem��file)
    {return LacEncode( static_cast<float>(nRate), nInSize, pInImg, cOutPath); }

    LacStatus LacEncode(int nRate, char const* cInPath, int nMemSize, BYTE* pOutImg, int* nOutSize)	    // BMP��LAC (file��mem)
    {return LacEncode( static_cast<float>(nRate), cInPath, nMemSize, pOutImg, nOutSize); }

    LacStatus LacEncode(int nRate, char const* cInPath, char const* cOutPath)							// BMP��LAC (file��file)
    {return LacEncode( static_cast<float>(nRate), cInPath, cOutPath); }

	//// �� �g���₷���悤��
	LacStatus LacDecode(int nInSize, BYTE const* pInImg, int nMemSize, BYTE* pOutImg, int* nOutSize);	// LAC��BMP (mem��mem)
	LacStatus LacDecode(int nInSize, BYTE const* pInImg, char const* cOutPath);							// LAC��BMP (mem��file)
	LacStatus LacDecode(char const* cInPath, int nMemSize, BYTE* pOutImg, int* nOutSize);				// LAC��BMP (file��mem)
	LacStatus LacDecode(char const* cInPath, char const* cOutPath);										// LAC��BMP (file��file)
	int LacDecodeGpu(BYTE const* pInImg, int nInSize, BYTE* pOutImg);									// LAC��BMP (mem��mem) GPU��LAC(LAC5)
	

	void SetThrId(int nId)	{ m_nThrId = nId; };														// �X���b�hID���Z�b�g[��ImgComp]

	void* GpuMallocHost(size_t nSize);																	// �������m��
	void  GpuFreeHost(void* ptr);																		// �������J��


protected:

	HINSTANCE		m_hInst[2];					// DLL�n���h���B

    // DLL�̊֐��|�C���^�Q�B�ǂ݂񍞂�DLL�ɂ���āA�����΂���������
    typfLacDecode   m_pfLacDecode;

    typfLacEncode_f m_pfLacEncode_f;
    typfLacEncode   m_pfLacEncode;

    typfLac2Create  m_pfLac2Create;
    typfLac2Destroy m_pfLac2Destroy;
    typfLac2Encode  m_pfLac2Encode;

	// GPU��LAC(LAC5)DLL�̊֐��|�C���^�Q
	gpulac5_encoder_create_t			m_pfGpuLacEncoderCreate;	// �G���R�[�_����
	gpulac5_encoder_encode_t			m_pfGpuLacEncode;			// �G���R�[�h���s
	gpulac5_encoder_destroy_t			m_pfGpuLacEncoderDestroy;	// �G���R�[�_�j��
	gpulac5_get_cuda_device_count_t		m_pfGpuLacDeviceCount;		// CUDA�f�o�C�X���̃`�F�b�N
	gpulac5_get_cuda_device_uuid_t		m_pfGpuLacDeviceUuid;		// CUDA�f�o�C�X��UUID�擾
	gpulac5_get_cuda_device_license_t	m_pfGpuLacDeviceLicense;	// CUDA�f�o�C�X��LAC5���C�Z���X�L���`�F�b�N
	gpulac5_malloc_host_t				m_pfMallocHost;				// �̈�m��(�G���R�[�h�̓��̓G���A�͖{�֐��ł̊m�ۂ��K�{)
	gpulac5_free_host_t					m_pfFreeHost;				// �̈���

	cpulac5_decoder_get_image_size_t	m_pfGpuLacDecoderSize;		// �f�R�[�h�����ɁA���k�摜�̐��@�擾
	cpulac5_decoder_decode_t			m_pfGpuLacDecode;			// �f�R�[�h���s

	int				m_nLoadType;				// �ǂݍ���ł����� -1:���[�h���� 0:LAC  1:NUC  2:LAC5
    char mcDllName[32]; // �ꎞ�I�Ɏg�p����DLL����

    //NUC�̃G���R�[�_�C���X�^���X
    PLac2Codec m_pCodec;
	int m_nCodecWidth;
	int m_nCodecHeight;
    
	virtual const char *DllName( int nMode, int nEncode );	//�ǂ݂��ނׂ�DLL�̃t�@�C������Ԃ��B

public:
	// GPU��LAC(LAC5)�̊֐��Q

	int		GetDeviceCnt();									// CUDA�f�o�C�X���̃`�F�b�N
	CString	GetDeviceUUID(int nDevID);						// CUDA�f�o�C�X��UUID�擾
	bool	IsHaveLacLicense(int nDevID);					// CUDA�f�o�C�X��LAC5���C�Z���X�L���`�F�b�N
	int		CreateEncoder(int w, int h, int quality);		// LAC5 �G���R�[�_����
	bool	DestroyEncoder();								// LAC5 �G���R�[�_�j��

	int		DoGpuEncode(BYTE const* pInImg, int nInSize, BYTE* pOutImg, int* nOutSize);	// LAC5 �G���R�[�h���s


	
	bool	IsGpuEncoder()		{ return m_bIsGpuEncoderExist; }	// true:GPU�ň��k�G���R�[�_�𐶐���
	int		GetGpuEncoderW()	{ return m_nEncoderWidth; }			// �G���R�[�_�̉摜��
	int		GetGpuEncoderH()	{ return m_nEncoderHeight; }		// �G���R�[�_�̉摜����
	int		GetGpuEncoderQ()	{ return m_nEncoderQuality; }		// �G���R�[�_�̈��k�i��

private:

	bool	m_bIsGpuEncoderExist;							// true:GPU�ň��k�G���R�[�_�𐶐���
	int		m_nEncoderWidth;								// �G���R�[�_�̉摜��
	int		m_nEncoderHeight;								// �G���R�[�_�̉摜����
	int		m_nEncoderQuality;								// �G���R�[�_�̈��k�i��

	gpulac5_encoder	m_GpuEncoder;							// �쐬�����G���R�[�_

	int		m_nThrId;										// �X���b�hID


};	
