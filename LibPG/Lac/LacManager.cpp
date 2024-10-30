#include "StdAfx.h"
#include "LacManager.h"
#include <exception>

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// コンストラクタ
// int nMode モード (-1:DLL読込無し 0:LAC 1:NUC 2:LAC5)
//------------------------------------------
LacManager::LacManager(int nMode)
	:m_pCodec(0), m_pfLac2Destroy(0), m_nThrId(-1),
	m_bIsGpuEncoderExist(false), m_nEncoderWidth(0), m_nEncoderHeight(0), m_nEncoderQuality(0)
{
	m_hInst[0] = m_hInst[1] = 0;

	UnLoad();
    
	// 読込
	if(-1 != nMode) {
		Load(nMode);
	}
}
//------------------------------------------
// デストラクタ
//------------------------------------------
LacManager::~LacManager(void)
{
	UnLoad();
}


//////////////////////////////////////////////////////////////////////
// DLLロード

enum {
    DLL_ENC,  //エンコード用DLL
    DLL_DEC,  //デコード用DLL
    DLL_OLD   //両方あるDLL
};
    

//------------------------------------------
// DLLロード
// int nVer モード (0:LAC 1:NUC 2:LAC5)
//------------------------------------------
bool LacManager::Load(int nMode)
{
	//// DLLインポート
	m_hInst[DLL_ENC] = LoadLibrary( DllName(nMode, DLL_ENC) );
	m_hInst[DLL_DEC] = LoadLibrary( DllName(nMode, DLL_DEC) );

	if(!m_hInst[0] && !m_hInst[1]) {
        UnLoad();
        
        // 個別DLLは無いみたいなので、共通をよみなおし。
        m_hInst[DLL_ENC] = LoadLibrary( DllName(nMode, DLL_OLD) );
        m_hInst[DLL_DEC] = LoadLibrary( DllName(nMode, DLL_OLD) );

        if( !m_hInst[0] ) {
            // 何もDLLが無い
            return false;
        }
	}
    
    if(nMode == EM_DLL_TYPE::DLL_TYPE_LAC)
    {
		// LACの関数群
        m_pfLacDecode = (typfLacDecode)(::GetProcAddress(m_hInst[DLL_DEC], "LacDecode"));
        
        //*********圧縮・展開が別DLLなら、圧縮率Float版、という仮定
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
		// NUCの関数群
		m_pfLacDecode = (typfLacDecode)(::GetProcAddress(m_hInst[DLL_DEC], "Lac2Decode"));

        m_pfLac2Create  =(typfLac2Create)::GetProcAddress(m_hInst[DLL_ENC], "Lac2Create");
        m_pfLac2Destroy =(typfLac2Destroy)::GetProcAddress(m_hInst[DLL_ENC], "Lac2Destroy");
        m_pfLac2Encode = (typfLac2Encode )::GetProcAddress(m_hInst[DLL_ENC], "Lac2Encode");
        m_nLoadType = nMode ;
    }
	else {
		// LAC5の関数群
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

		// エンコーダのDLLがある場合は、GPU版LACが使用可能か確認
		if (m_hInst[0]) {
			int nDevCnt = GetDeviceCnt();	// PC上のCUDAデバイスの数

			if (0 == nDevCnt)
			{
				UnLoad();
				return false;
			}

			for (int ii = 0; ii < nDevCnt; ii++)
			{
				if (!IsHaveLacLicense(ii))		// ライセンス有無チェック
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
//DLLの名称を取得する。
// int nMode モード (-1:DLL読込無し 0:LAC 1:NUC 2:LAC5)
// int nEncode: DLLのエディション。　0:共通、1:圧縮のみ、2:展開のみ
//------------------------------------------
const char *LacManager::DllName( int nMode, int nEncode )
{
    static const char sArch[]  = ""
		#ifdef _M_X64			// x64 コンパイル
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
// DLL解放
//------------------------------------------
void LacManager::UnLoad()
{
    //エンコーダインスタンスがあれば始末する
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

    //DLLを解放する
    if( m_hInst[0] ){
		FreeLibrary( m_hInst[0] );
        m_hInst[0] = 0;
	}
	if( m_hInst[1] ){
		FreeLibrary( m_hInst[1] );
        m_hInst[1] = 0;
	}

    // DLLの関数ポインタ連中をクリアする
    m_pfLacDecode=0;
    
	m_pfLacEncode_f=0;
    m_pfLacEncode=0;

    //NUCのエンコード
    m_pfLac2Create =0;
    m_pfLac2Destroy=0;
    m_pfLac2Encode =0;
    
	// LAC5のエンコード
	m_pfGpuLacEncoderCreate = 0;
	m_pfGpuLacEncoderDestroy = 0;
	m_pfGpuLacEncode = 0;
	m_pfMallocHost = 0;
	m_pfFreeHost = 0;

    m_nLoadType = -1 ;
	
	m_nCodecHeight = 0;
	m_nCodecWidth = 0;
}

// NUCの場合のEncoderの初期化。
// width  : 圧縮画像の幅
// height : 圧縮画像の高
bool  LacManager::InitEncoder( int nWidth, int nHeight )
{
    if( m_pfLac2Create ){
        if( m_pCodec )
        {
			//パラメタが同じなら、不要
			if( m_nCodecWidth == nWidth && m_nCodecHeight == nHeight ){
				return true;
			}

            m_pfLac2Destroy(m_pCodec);
            m_pCodec = 0;
        }

		//エンコーダインスタンス作成
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

// LAC5の場合のEncoderの初期化。
// nWidth   : 圧縮画像の幅
// nHeight  : 圧縮画像の高
// nQuality : 画質パラメータ値
int  LacManager::InitEncoder(int nWidth, int nHeight, int nQuality)
{
	// 整合性チェック
	if (40 > nQuality || 100 < nQuality)	return -2;		// 引数異常

	if (m_pfGpuLacEncoderCreate) {
		if (nHeight != GetGpuEncoderH() ||
			nWidth != GetGpuEncoderW() ||
			nQuality != GetGpuEncoderQ())
		{
			// 幅、高さ、画質パラメータが変更されている場合は、エンコーダ生成

			DestroyEncoder();										// エンコーダ解放

			int nRet = CreateEncoder(nWidth, nHeight, nQuality);	// エンコーダ生成
			return nRet;
		}
		if(IsGpuEncoder()) return 0;								// 生成済
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////
// 基本

//------------------------------------------
// BMP→LAC (圧縮基本)
// LacData *src			入力情報
// LacData *dst			出力情報
// float dRate			圧縮率:1<高画質>～   品質:-1～-100<高画質>
//------------------------------------------
LacManager::LacStatus LacManager::doLacEncode(LacData *src, LacData *dst, float dRate)
{
	// 整合性チェック
	if( 0 == dRate || -100 > dRate)	return LacStatus_ParamError;		// 引数異常

	// 圧縮
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
        return LacStatus_OtherErr;			// DLLロード正常?
    }
}

//------------------------------------------
// LAC→BMP (解凍基本)
// LacData *src			入力情報
// LacData *dst			出力情報
//------------------------------------------
LacManager::LacStatus LacManager::doLacDecode(LacData *src, LacData *dst)
{
	// 解凍
    if( m_pfLacDecode ){
		return m_pfLacDecode(src, dst);
    }
    else{
        return LacStatus_OtherErr;			// DLLロード正常?
    }
}


//////////////////////////////////////////////////////////////////////
// 使いやすいように 圧縮

//------------------------------------------
// BMP→LAC (mem→mem)
// float dRate			圧縮率:1<高画質>～   品質:-1～-100<高画質>
// int nInSize			入力データサイズ
// BYTE const* pInImg	画像データ
// int nMemSize			呼び出し元で確保した領域サイズ
// BYTE* pOutImg		LACデータ領域 (呼び出し元で領域確保)
// int* nOutSize		圧縮後のサイズ (in/out)
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

// BMP→LAC (mem→file)
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

// BMP→LAC (file→mem)
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
// BMP→LAC (file→file)
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
// 使いやすいように 解凍

//------------------------------------------
// LAC→BMP (mem→mem)
// int nInSize			入力データサイズ
// BYTE const* pInImg	画像データ
// int nMemSize			呼び出し元で確保した領域サイズ
// BYTE* pOutImg		BMPデータ領域 (呼び出し元で領域確保)
// int* nOutSize		解凍後のサイズ (in/out)
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
// LAC→BMP (mem→file)
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
// LAC→BMP (file→mem)
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
// LAC→BMP (file→file)
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
// GPU版LAC(LAC5) 関数群
//------------------------------------------------------------------------------------

//------------------------------------------
// BMP→LAC (圧縮基本)
// BYTE* pInImg			入力画像データ
// int   nInSize		入力画像サイズ
// BYTE* pOutImg		出力画像データ
// int*  nOutSize		出力画像サイズ
// 戻り値: =0:正常  !=0:異常
//------------------------------------------
int LacManager::doLacEncodeGpu(BYTE const* pInImg, int nInSize, BYTE* pOutImg, int* nOutSize)
{
	// 圧縮
	if (m_pfGpuLacEncode)
	{
		if (!IsGpuEncoder()) return LacStatus_OtherErr;
		return (LacStatus)DoGpuEncode((BYTE*)pInImg, nInSize, (BYTE*)pOutImg, nOutSize);
	}
	else {
		return LacStatus_OtherErr;			// DLLロード正常?
	}
}

//------------------------------------------
// LAC→BMP (解凍基本)
// BYTE* pInImg			入力画像データ
// int   nInSize		入力画像サイズ
// BYTE* pOutImg		出力画像データ
// 戻り値: =0:正常  !=0:異常
//------------------------------------------
int LacManager::doLacDecodeGpu(BYTE const* pInImg, int nInSize, BYTE* pOutImg)
{
	// 解凍
	if (m_pfGpuLacDecode) {
		return (LacStatus)m_pfGpuLacDecode((uint8_t*)pInImg, nInSize, (uint8_t*)pOutImg);
	}
	else {
		return LacStatus_OtherErr;			// DLLロード正常?
	}
}


//////////////////////////////////////////////////////////////////////
// 使いやすいように 圧縮

//------------------------------------------
// BMP→LAC (mem→mem)
// BYTE* pInImg			入力画像データ
// int   nInSize		入力画像サイズ
// BYTE* pOutImg		出力画像データ
// int*  nOutSize		出力画像サイズ
// 戻り値: =0:正常  !=0:異常
//------------------------------------------
int LacManager::LacEncodeGpu(BYTE const* pInImg, int nInSize, BYTE* pOutImg, int* nOutSize)
{
	int nRetc = doLacEncodeGpu((BYTE*)pInImg, nInSize, (BYTE*)pOutImg, nOutSize);

	return nRetc;
}


//////////////////////////////////////////////////////////////////////
// 使いやすいように 解凍

//------------------------------------------
// LAC→BMP (mem→mem)
// BYTE* pInImg			入力画像データ
// int   nInSize		入力画像サイズ
// BYTE* pOutImg		出力画像データ
// int*  nOutSize		出力画像サイズ
// 戻り値: =0:正常  !=0:異常
//------------------------------------------
int LacManager::LacDecodeGpu(BYTE const* pInImg, int nInSize, BYTE* pOutImg)
{
	int nRetc = doLacDecodeGpu(pInImg, nInSize, pOutImg);

	return nRetc;
}

//------------------------------------------
// CUDAデバイス数のチェック
// 戻り値:CUDAデバイス数
//------------------------------------------
int LacManager::GetDeviceCnt()
{
	int nRetc = -1;
	int nCount = 0;
	nRetc = m_pfGpuLacDeviceCount(&nCount);

	return nCount;
}

//------------------------------------------
// CUDAデバイスのUUID取得
// int nDevID			CUDAデバイスID(0から始まる)
// 戻り値:UUID
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
// CUDAデバイスのLAC5ライセンス有無チェック
// int nDevID			CUDAデバイスID(0から始まる)
// 戻り値:true:ライセンス有り false:ライセンス無し
//------------------------------------------
bool LacManager::IsHaveLacLicense(int nDevID)
{
	int nRet = m_pfGpuLacDeviceLicense(nDevID);

	return (1 == nRet ? true : false);
}

//------------------------------------------
// LAC5 エンコーダ生成
// int nWidth			非圧縮画像の幅[pixel]
// int nHeight			非圧縮画像の高さ[pixel]
// int nQuality			画質パラメータ
// 戻り値: =0:正常  !=0:異常
//------------------------------------------
int LacManager::CreateEncoder(int nWidth, int nHeight, int nQuality)
{
	int nDevID = m_nThrId;		// CUDAデバイスID(0から始まる)

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
// LAC5 エンコーダ破棄
// 戻り値:true:正常 false:異常
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
// LAC5 エンコード実行
// BYTE* pInImg			入力画像データ
// int   nInSize		入力画像サイズ
// BYTE* pOutImg		出力画像データ
// int*  nOutSize		出力画像サイズ
// 戻り値: =0:正常  !=0:異常
//------------------------------------------
int LacManager::DoGpuEncode(BYTE const* pInImg, int nInSize, BYTE* pOutImg, int* nOutSize)
{
	uint8_t*	original_image = (uint8_t*)pInImg;
	uint8_t*	compressed_image = NULL;	// (uint8_t*)pOutImg;		// 返却は変数へのポインタ
	int			compressed_image_size = 0;

	int nRet = m_pfGpuLacEncode(m_GpuEncoder, original_image, &compressed_image, &compressed_image_size);
	// GPU版LACは圧縮後データの返却エリアをエンコーダ側で確保しているため、
	// 出力画像データには、データコピーして返却する必要がある
	memcpy(pOutImg, compressed_image, compressed_image_size);

	*nOutSize = compressed_image_size;

	return nRet;
}

//------------------------------------------
// LAC5 メモリ確保
// size_t  nSize		要求された割り当てサイズ(バイト単位)
// 戻り値: 成功した場合、割り当てられたメモリへのポインタ
//------------------------------------------
void* LacManager::GpuMallocHost(size_t nSize)
{
	if (!m_pfMallocHost) return NULL;
	return (BYTE*)m_pfMallocHost(nSize);
	//return (BYTE*)malloc(nSize);
}

//------------------------------------------
// LAC5 メモリ開放
// void*  ptr		メモリを開放するポインタ
//------------------------------------------
void LacManager::GpuFreeHost(void* ptr)
{
	if (NULL == ptr) return;
	if (!m_pfFreeHost) return;
	return m_pfFreeHost(ptr);
	//return free(ptr);
}
