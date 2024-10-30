// *********************************************************************************
//	高度画像圧縮クラス
//	[Ver]
//		Ver.01    2011/02/20  vs2005 対応
//		Ver.02    2014/11/04  LacのDLL分離に対応
//
//	[メモ]
//		EXEと同一フォルダに LacDLL.dll を入れておくこと		LAC
//		EXEと同一フォルダに Lac2DLL.dll を入れておくこと	NUC
// *********************************************************************************

#pragma once

//#include <windows.h>
#include <stdint.h>

class LacManager
{
	
public:
	// 復帰情報
	typedef enum
	{
		LacStatus_Ok,						// 0:正常
		LacStatus_OutOfMemory,				// 1:メモリ不足 ←たぶんPCのメモリ領域っぽい
		LacStatus_ParamError,				// 2:パラメータ指定が異常
		LacStatus_IoFileOpenFail,			// 3:ファイルオープン失敗
		LacStatus_IoFileInvalidFormat,		// 4:オープンは成功したが、ファイル自体が壊れてる(=ヘッダーが変？)
		LacStatus_IoGeneralError,			// 5:その他I/O関係の異常
		LacStatus_InternalError,			// 6:LACコーディック自体の内部異常
		LacStatus_LicenseError,				// 7:ライセンスエラー


		LacStatus_OtherErr = -1				// このクラス内部での異常
	} LacStatus;

	// 入出力形式
	typedef enum
	{
		LacDataType_FileName,				// ファイル受け渡し時
		LacDataType_MemBlock				// メモリ受け渡し時
	} LacDataType;

	// 入出力情報
	typedef struct  
	{
		LacDataType		type;				// 入出力形式
		void			*ptr;				// パラメータ (ファイル渡し時：ファイルパス。   メモリ渡し時：メモリポインター(領域は、呼び元で確保))
		int				nbytes;				// パラメータがメモリ渡し時のみ有効。 メモリにセットした値
												// メモリ入力時(inのみ) : in:入力データのサイズ
												// メモリ出力時(in/out) : in:出力用メモリの領域確保したサイズ  out:変換後にLACがセットしたサイズ 
	} LacData;


	typedef struct Lac2CodecDummy *PLac2Codec; // NUCエンコーダインスタンス
	struct  Lac2CreateParams
	{
	   int          width;                  // 画像の幅
	   int          height;                 // 画像の高
	   int          roicnt;                 // 圧縮に使用するスレッド数
	};

	//// 画像圧縮タイプ
	enum EM_DLL_TYPE
	{
		DLL_TYPE_LAC = 0,												// LAC圧縮 (高度圧縮技術)
		DLL_TYPE_NUC,													// NUC圧縮 (改良版高度圧縮技術)
		DLL_TYPE_LAC5													// LAC5圧縮 (GPU版高度圧縮技術)
	};

protected:
    
	// DLL内のファンクションポインタ
    // LAC/NUCのデコード
	typedef LacStatus (*typfLacDecode)(LacData *src, LacData *dst);
    
    // LACのエンコード。VER 2.05以降は上側(圧縮率がfloat)
	typedef LacStatus (*typfLacEncode_f)(LacData *src, LacData *dst, float rate);	
	typedef LacStatus (*typfLacEncode)(LacData *src, LacData *dst, int rate);

    // NUCのエンコード。エンコーダインスタンス作成・削除を含む。
    typedef PLac2Codec (*typfLac2Create)(Lac2CreateParams *params);
	typedef void       (*typfLac2Destroy)(PLac2Codec enc);
	typedef LacStatus  (*typfLac2Encode)(PLac2Codec enc, LacData *src,LacData *dst, int rate);

    // LAC5のエンコード。エンコーダインスタンス作成・削除を含む。
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
	virtual bool	Load(int nMode);			// DLLロード
    virtual void	UnLoad();					// DLL解放
	int		IsLoadType() { return m_nLoadType; }// DLLロード種類

	//// 基本
    virtual bool  InitEncoder( int nWidth, int nHeight );                                                       // NUCの場合の圧縮前初期化。
    virtual int   InitEncoder( int nWidth, int nHeight, int nQuality );                                         // GPU版LAC(LAC5)の場合の圧縮前初期化。
	virtual LacStatus doLacEncode(LacData *src, LacData *dst, float dRate);										// BMP→LAC (圧縮基本)
    virtual LacStatus doLacDecode(LacData *src, LacData *dst);													// LAC→BMP (解凍基本)
	virtual int doLacEncodeGpu(BYTE const* pInImg, int nInSize, BYTE* pOutImg, int* nOutSize);					// BMP→LAC (圧縮基本) GPU版LAC(LAC5)
    virtual int doLacDecodeGpu(BYTE const* pInImg, int nInSize, BYTE* pOutImg);									// LAC→BMP (解凍基本) GPU版LAC(LAC5)

	//// 圧縮 使いやすいように
	LacStatus LacEncode(float dRate, int nInSize, BYTE const* pInImg, int nMemSize, BYTE* pOutImg, int* nOutSize);	// BMP→LAC (mem→mem)
	LacStatus LacEncode(float dRate, int nInSize, BYTE const* pInImg, char const* cOutPath);				// BMP→LAC (mem→file)
	LacStatus LacEncode(float dRate, char const* cInPath, int nMemSize, BYTE* pOutImg, int* nOutSize);	    // BMP→LAC (file→mem)
	LacStatus LacEncode(float dRate, char const* cInPath, char const* cOutPath);							// BMP→LAC (file→file)
	int LacEncodeGpu(BYTE const* pInImg, int nInSize, BYTE* pOutImg, int* nOutSize);						// BMP→LAC (mem→mem) GPU版LAC(LAC5)

	//// 圧縮 使いやすいように（旧版。圧縮率がint。実装は、float版へ丸投げ)
	LacStatus LacEncode(int nRate, int nInSize, BYTE const* pInImg, int nMemSize, BYTE* pOutImg, int* nOutSize)	// BMP→LAC (mem→mem)
    {return LacEncode( static_cast<float>(nRate), nInSize, pInImg,  nMemSize,  pOutImg, nOutSize); }

    LacStatus LacEncode(int nRate, int nInSize, BYTE const* pInImg, char const* cOutPath)				// BMP→LAC (mem→file)
    {return LacEncode( static_cast<float>(nRate), nInSize, pInImg, cOutPath); }

    LacStatus LacEncode(int nRate, char const* cInPath, int nMemSize, BYTE* pOutImg, int* nOutSize)	    // BMP→LAC (file→mem)
    {return LacEncode( static_cast<float>(nRate), cInPath, nMemSize, pOutImg, nOutSize); }

    LacStatus LacEncode(int nRate, char const* cInPath, char const* cOutPath)							// BMP→LAC (file→file)
    {return LacEncode( static_cast<float>(nRate), cInPath, cOutPath); }

	//// 解凍 使いやすいように
	LacStatus LacDecode(int nInSize, BYTE const* pInImg, int nMemSize, BYTE* pOutImg, int* nOutSize);	// LAC→BMP (mem→mem)
	LacStatus LacDecode(int nInSize, BYTE const* pInImg, char const* cOutPath);							// LAC→BMP (mem→file)
	LacStatus LacDecode(char const* cInPath, int nMemSize, BYTE* pOutImg, int* nOutSize);				// LAC→BMP (file→mem)
	LacStatus LacDecode(char const* cInPath, char const* cOutPath);										// LAC→BMP (file→file)
	int LacDecodeGpu(BYTE const* pInImg, int nInSize, BYTE* pOutImg);									// LAC→BMP (mem→mem) GPU版LAC(LAC5)
	

	void SetThrId(int nId)	{ m_nThrId = nId; };														// スレッドIDをセット[←ImgComp]

	void* GpuMallocHost(size_t nSize);																	// メモリ確保
	void  GpuFreeHost(void* ptr);																		// メモリ開放


protected:

	HINSTANCE		m_hInst[2];					// DLLハンドル。

    // DLLの関数ポインタ群。読みん込んだDLLによって、無いばあいもある
    typfLacDecode   m_pfLacDecode;

    typfLacEncode_f m_pfLacEncode_f;
    typfLacEncode   m_pfLacEncode;

    typfLac2Create  m_pfLac2Create;
    typfLac2Destroy m_pfLac2Destroy;
    typfLac2Encode  m_pfLac2Encode;

	// GPU版LAC(LAC5)DLLの関数ポインタ群
	gpulac5_encoder_create_t			m_pfGpuLacEncoderCreate;	// エンコーダ生成
	gpulac5_encoder_encode_t			m_pfGpuLacEncode;			// エンコード実行
	gpulac5_encoder_destroy_t			m_pfGpuLacEncoderDestroy;	// エンコーダ破棄
	gpulac5_get_cuda_device_count_t		m_pfGpuLacDeviceCount;		// CUDAデバイス数のチェック
	gpulac5_get_cuda_device_uuid_t		m_pfGpuLacDeviceUuid;		// CUDAデバイスのUUID取得
	gpulac5_get_cuda_device_license_t	m_pfGpuLacDeviceLicense;	// CUDAデバイスのLAC5ライセンス有無チェック
	gpulac5_malloc_host_t				m_pfMallocHost;				// 領域確保(エンコードの入力エリアは本関数での確保が必須)
	gpulac5_free_host_t					m_pfFreeHost;				// 領域解放

	cpulac5_decoder_get_image_size_t	m_pfGpuLacDecoderSize;		// デコードせずに、圧縮画像の寸法取得
	cpulac5_decoder_decode_t			m_pfGpuLacDecode;			// デコード実行

	int				m_nLoadType;				// 読み込んでいる種類 -1:ロード無し 0:LAC  1:NUC  2:LAC5
    char mcDllName[32]; // 一時的に使用するDLL名称

    //NUCのエンコーダインスタンス
    PLac2Codec m_pCodec;
	int m_nCodecWidth;
	int m_nCodecHeight;
    
	virtual const char *DllName( int nMode, int nEncode );	//読みこむべきDLLのファイル名を返す。

public:
	// GPU版LAC(LAC5)の関数群

	int		GetDeviceCnt();									// CUDAデバイス数のチェック
	CString	GetDeviceUUID(int nDevID);						// CUDAデバイスのUUID取得
	bool	IsHaveLacLicense(int nDevID);					// CUDAデバイスのLAC5ライセンス有無チェック
	int		CreateEncoder(int w, int h, int quality);		// LAC5 エンコーダ生成
	bool	DestroyEncoder();								// LAC5 エンコーダ破棄

	int		DoGpuEncode(BYTE const* pInImg, int nInSize, BYTE* pOutImg, int* nOutSize);	// LAC5 エンコード実行


	
	bool	IsGpuEncoder()		{ return m_bIsGpuEncoderExist; }	// true:GPU版圧縮エンコーダを生成済
	int		GetGpuEncoderW()	{ return m_nEncoderWidth; }			// エンコーダの画像幅
	int		GetGpuEncoderH()	{ return m_nEncoderHeight; }		// エンコーダの画像高さ
	int		GetGpuEncoderQ()	{ return m_nEncoderQuality; }		// エンコーダの圧縮品質

private:

	bool	m_bIsGpuEncoderExist;							// true:GPU版圧縮エンコーダを生成済
	int		m_nEncoderWidth;								// エンコーダの画像幅
	int		m_nEncoderHeight;								// エンコーダの画像高さ
	int		m_nEncoderQuality;								// エンコーダの圧縮品質

	gpulac5_encoder	m_GpuEncoder;							// 作成したエンコーダ

	int		m_nThrId;										// スレッドID


};	
