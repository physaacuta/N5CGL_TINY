#pragma once
// *********************************************************************************
//	RPIライブラリの基本部分を含むヘッダ。
//　　構造体やEnum、クラスの前方宣言が含まれる。
//
//	[Ver]
//		Ver.01    2015/01/15  
//
//	[メモ]
//　	Rpiを使いたいクラスの宣言(.h  )で#include "librpi/rpi.h"
//　	Rpiを使いたいクラスの定義(.cpp)で#include "librpi/RpiWriter.h"とか、"librpi/RpiReaderFile"とかする
//// *********************************************************************************


#include <deque>

//VisualC++ にstdint.h式の型名を与える
#if _MSC_VER >= 1600
//VC2010以降は、標準ヘッダにある
#include <stdint.h>
#else
typedef   signed char  int8_t ;
typedef unsigned char uint8_t ;

typedef   signed short  int16_t ;
typedef unsigned short uint16_t ;

typedef   signed int  int32_t ;
typedef unsigned int uint32_t ;

typedef   signed __int64  int64_t ;
typedef unsigned __int64 uint64_t ;
#endif

namespace RpiLib
{
//------------------------------------------
// 拡張ヘッダー部構造体 (MAX224byte→416に拡張)
//------------------------------------------
#define	RPI_THEAD_VER		2		// ヘッダーバージョン

#if RPI_THEAD_VER == 1
#define	NUM_CAM_INFO		8		// RPIカメラ情報格納数
#else
#define	NUM_CAM_INFO		16		// RPIカメラ情報格納数
#endif


struct T_RPI_HEADER_BASE {
    long            nTHeaderSize;    // この構造体のサイズ
    long            nTHeaderVer;     // Ｔヘッダーバージョン (Ver1:カメラ情報8台 Ver2:カメラ情報16台)
    char            cSysName[20];    // システム略称
    char            cKizukenNo[20];  // 疵検管理No
    short           frame_info_type; // フレーム情報部の格納形式 (0:無し 1:T_RPI_FRAME_INFO_T1)
    short           frame_info_size; // フレーム情報部のサイズ
    DWORD           nFrameNumInit;   // コイル先頭のフレームNo
    DWORD           nFrameNumStart;  // このファイル先頭のフレームNo
    long            nFileNum;        // コイル単位でのファイル何個目(１コイルの中で、ファイル分割した時に加算。)(0:分割無し 1～:分割有りで且つ何個目。1コ目のときは1)
    long            nLineCenter;     // ラインセンター画素(連結画像左端からのオフセット値) [pixel]
    long            nStartCamset;    // 左端カメラセット番号 (1オリジン)
    long            nCamsetNum;      // 幅方向フレーム結合数 (1オリジン)
    long            nCamAngle;       // カメラ角度 (1オリジン) 色差の場合は、色(1オリジン)
    long            nMen;            // 表裏区分 (0:表 1:裏)

    struct CAM_INFO {
        float        dBaseXoffset;   // 基準位置。ラインセンターからの各カメラ左端の位置(左方向は、マイナス値) [mm] (PARAM_CAMSETのXoffset値そのもの)
        int          nBaseLeft;      // 基準画素。結合後のフレーム左端から自カメラ画像左端までの距離
        int          nOverLap;       // 次カメラとの右側のオーバーラップ量
    }caminfo[NUM_CAM_INFO];			 // 8->16に拡張(極微小φω対応)
    long            nStartLen;       // 先頭フレーム長手位置 [mm]
    long            nColorType;      // 画像タイプ (0:グレー画像  1:カラー画像  2:色差画像)    
    long            nSysType;        // 疵検タイプ (0:連続ライン  1:バッチライン)
    long            nFTposCfNo[2];   // 先後端位置フレームNo (このファイル先頭からのフレームNo (0オリジン))            ※ バッチのみ
    long            nFTposPixel[2];  // 先後端位置フレーム内pixel (そのフレーム上端からのピクセル位置 (0オリジン))

	int64_t			nFooterStartPos; // フッターの開始位置（ファイル先頭からの位置）
	long			szFooterRecord;	 // フッターの１レコードあたりのサイズ
	
	T_RPI_HEADER_BASE(){
		memset( this, 0, sizeof(*this) );
		nTHeaderSize  = sizeof(*this);
		nTHeaderVer   = RPI_THEAD_VER;
	}
};

//予備エリアを含めた、Tヘッダ
//これで変数を確保しておくと、拡張されたときもそれなりに大丈夫
struct T_RPI_HEADER : T_RPI_HEADER_BASE{
	char yobi[ 416 - sizeof(T_RPI_HEADER_BASE) ];

    T_RPI_HEADER(){
        memset( yobi, 0, sizeof(yobi) );
    }
};

//------------------------------------------
//フレーム付加情報 タイプ①  128byte固定
//------------------------------------------
struct T_RPI_FRAME_INFO_T1 {
	DWORD			nFNo;											// フレームNo
	long			nSpeed;											// ライン速度 [mm/sec]
	float			vRes;											// 縦分解能 [mm/pixel]
	long			nFromLen;										// コイル先端からの距離 [mm]		
	float			dEdgePos[2];									// エッジ位置 [mm]
	int				nEdgeIdx[2];									// 左端からの左右エッジPixel位置 [pixel]
	
	int				nYobi[24];

	T_RPI_FRAME_INFO_T1()
	{
		memset( this, 0, sizeof(*this) );
	}
};


//------------------------------------------
// 拡張フッター構造体。
// ファイル末尾に、この構造体がフレーム数分繰り返しおかれる。
//------------------------------------------
struct T_RPI_FOOTER_RECORD
{
	int64_t nFilePos;			//フレームのファイル先頭からの開始位置
	int32_t szFrame;			//フレームのサイズ（画像・余白・Ｔフレーム情報分など含む）
	int32_t szImage;			//画像のサイズ
};


//------------------------------------------
// RPI基本ヘッダ。　RPIファイルの先頭に置かれる。
//------------------------------------------
struct RpiHeader
{    
    int32_t type;					//RPIタイプ
    int32_t header_size;			//RPIヘッダのサイズ
    int32_t width;					//フレーム画像の幅
    int32_t height;					//フレーム画像の高さ
    int32_t nchannel;				//フレーム画像のチャネル数
    int32_t pixel_nbit;				//1pixelあたりビット数。
    int32_t nframe;					//フレーム数
    int32_t frame_start_pos;		//１フレーム目の始まるファイルの位置
    float   reso_x; //opt			//解像度・横 [mm/pixel]
    float   reso_y; //opt			//解像度・縦 [mm/pixel]

	int32_t codec;                   //圧縮形式。[RfRF] Cf CF  ;Raw形式にCODECは本来無いが、便宜上CODEC_RAWを入れる。
    
//以下は、フォーマットタイプによってあったりなかったり。ない場合は０になる
    int32_t frame_info_type;         //   RF     CF  
    int32_t frame_nbyte;             //Rf RF
    int32_t frame_pitch_nbyte;       //Rf
    int32_t pitch_nbyte;             //Rf RF  Cf     ;なんで、Cfにあるのかはわからない。
    int32_t tHeader_start_pos;	 	 //       Cf[opt]

	T_RPI_HEADER theader;            //拡張ヘッダ

//以下ヘルパメソッド
	enum FormatType{
		FORMATTYPE_UNKNOWN,			 //0000:不明な形式
		FORMATTYPE_Rf = 1,           //0001:Rf:　生・無フレーム
		FORMATTYPE_RF = 3,           //0011:RF:　生・有フレーム
		FORMATTYPE_Cf = 5,           //0101:Cf:圧縮・無フレーム
		FORMATTYPE_CF = 7,           //0111:CF:圧縮・有フレーム
	};
	//typeとheaderから、RPI形式を返す
	FormatType GetFormatType() const ;
	
	//このRPIはEPCフレーム情報を持つか？
	bool HasFrameInfo() const;

	//このRPIは解像度情報を持つか？
	bool HasResolution()const;

	mutable char m_sCodec[5];
	//codec名を文字列でかえす。
	const char *GetCodecName() const {  
		const char *wkcodec = ( const char*)&codec;
		m_sCodec[0] = 	wkcodec[3];
		m_sCodec[1] = 	wkcodec[2];
		m_sCodec[2] = 	wkcodec[1];
		m_sCodec[3] = 	wkcodec[0];
		m_sCodec[4] = 	0;
		return m_sCodec;
	}

	//CODEC一覧
#define RPIFOURCC( c0, c1, c2, c3 )   ( (c0) << 24 | (c1)<< 16 | (c2) <<8 | (c3))
    enum emCodecType{
		CODEC_UNKNOWN = 0,									//不明
        CODEC_JPEG = RPIFOURCC( 'J', 'P', 'E', 'G' ),		//JPEG圧縮
        CODEC_LAC  = RPIFOURCC( 'L', 'A', 'C', ' ' ),		//LAC(高度圧縮)
        CODEC_NUC  = RPIFOURCC( 'N', 'U', 'C', ' ' ),		//NUC
        CODEC_RAW  = RPIFOURCC( 'R', 'A', 'W', ' ' ),		//RAW：フレーム生。
        CODEC_BMP  = RPIFOURCC( 'B', 'M', 'P', ' ' ),		//Windowsビットマップ

		CODEC_JP2K = RPIFOURCC( 'J', 'P', '2', 'K' ),		//Jpeg2000

		CODEC_LAC5 = RPIFOURCC( 'L', 'A', 'C', '5' )		//LAC5(GPU版高度圧縮)
    };


	//何も初期化しないコンストラクタ。読み込み用。
	RpiHeader();

	//書き込み用に、基本メンバの値をセットするコンストラクタ
	RpiHeader(int32_t width, int32_t height, emCodecType codec, const T_RPI_HEADER_BASE &theader=T_RPI_HEADER_BASE());
};


//------------------------------------------
// フレームリンク情報 構造体
//ファイルのnPosInFile ~ szFrameまで読んだら、
//画像サイズ・次  [0                         ~  szPreImg ]。
//画像            [szPreImg                  ~  szImg ]。
//EPCフレーム情報 [szPreImg + szImg          ~  szInf]。
//Ｔフレーム情報  [szPreImg + szImg + szInf  ~  szTFrameInf ]。
//------------------------------------------
struct RPIFrameLink
{
	int64_t nPosInFile;
	int32_t szImg;
	int32_t szFrame;


	RPIFrameLink CalcFrameSizeAndReturnNextPos( int szImg , const struct RPIFrameSizeInfo &LinkArray)	;
};

//------------------------------------------
//ファイル形式によって違う、フレームの前後サイズ情報 構造体
//------------------------------------------
struct RPIFrameSizeInfo
{
	int32_t szPreImg;			//フレームの初め、画像前にあるリンク情報のサイズ
    int32_t szInfo;				//画像の後にある、EPCフレーム情報のサイズ
    int32_t szTFrameInf;		//EPCフレーム情報のあとにある、Ｔフレーム情報のサイズ
	
	int32_t nSectorAlign;		//最小書き込み単位。
	
	RPIFrameSizeInfo() : nSectorAlign(512) {}

	//------------------------------------------
	//int size 指定サイズを最小書き込み単位で切り上げた値をかえす
	//------------------------------------------
	int roundup(int size) const
	{
		return ((size + nSectorAlign-1)/nSectorAlign)*nSectorAlign;
	}

	//------------------------------------------
	//画像のサイズから、それを格納するフレームの大きさをかえす。
	// imagesize : 画像のサイズ。
	//------------------------------------------
	int CalcFrameSize(int imagesize) const
	{
		return roundup(szPreImg + imagesize + szInfo + szTFrameInf);
	}

	static const int LINK_SIZE_Cf = 12;
};


//------------------------------------------
//RPIファイルの各フレーム位置を保持するクラス。
//フレーム前後情報を継承しているので、各種計算にも使える。
//------------------------------------------
class RpiFrameLinkArray 
	: public RPIFrameSizeInfo
	, public std::deque<RPIFrameLink>
{
public:
	//------------------------------------------
    //今フレームでついでに次フレームの先頭を4Kくらい読んでおく。
    //（今フレームを読む瞬間には、次フレームの位置はわかっている）
    //その「次フレーム情報」から、次フレームのサイズと、次次フレームの位置を決定する
	//------------------------------------------
    bool UpdateNextPosOnRead(int nframe, const BYTE *pNextFrame );
};

//クラスの前方宣言
class RpiReader;
class IRpiFrame;
class IRpiFrameEnum ;
class RpiReaderFile;
class RpiReaderMemory;
class TRpiWriter;
struct MemoryData;

};