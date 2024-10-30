#include "StdAfx.h"
#include "KizuAviManager.h"


using namespace ImgLib;


// //////////////////////////////////////////////////////////////////////////////
// 環境変数クラス のスタティック変数初期化
// //////////////////////////////////////////////////////////////////////////////
KizuAviEnv			KizuAviEnv::m_inst;					// スタティック変数初期化

//////////////////////////////////////////////////////////////////////
// 環境変数クラス
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// コンストラクタ (exeの起動時一回のみコール)
//------------------------------------------
KizuAviEnv::KizuAviEnv(void)
{
//	AVIFileInit();
}

//------------------------------------------
// デストラクタ (exeの起動時一回のみコール)
//------------------------------------------
KizuAviEnv::~KizuAviEnv(void)
{
	AVIFileExit(); 
}

// //////////////////////////////////////////////////////////////////////////////
// メインクラス
// //////////////////////////////////////////////////////////////////////////////

//------------------------------------------
// コンストラクタ
//------------------------------------------
KizuAviManager::KizuAviManager(void) :
m_AviFile(NULL),
m_AviStream(NULL),
m_nW(0),
m_nH(0),
m_nBitCnt(0),
m_nFrameCnt(0)
{
	// シングルトンのために一回実行しておく
	//KizuAviEnv;
AVIFileInit();
	memset(&m_AviInfo, 0x00, sizeof(m_AviInfo));
}

//------------------------------------------
// デストラクタ
//------------------------------------------
KizuAviManager::~KizuAviManager(void)
{
}


// //////////////////////////////////////////////////////////////////////////////
// スタティック関数
// //////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// AVIファイルのオープン
// const char* filename AVIファイルフルパス
// PAVIFILE& avi_file AVIファイルのハンドル
// PAVISTREAM& avi_stream AVIファイルのストリーム
// AVISTREAMINFO* avi_info AVIファイルのヘッダー情報
//------------------------------------------
bool KizuAviManager::OpenAVIFile(const char* filename, PAVIFILE& avi_file, PAVISTREAM& avi_stream, AVISTREAMINFO* avi_info)
{
    _ASSERT( NULL == avi_file && NULL == avi_stream );

    // open file
    HRESULT hr;
    hr = AVIFileOpen(&avi_file, filename, OF_READ, NULL);
    if( FAILED(hr) ) return false;

    // streamの取得
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
// AVIファイルのクロース
// PAVIFILE& avi_file オープンで取得したAVIファイルのハンドル
// PAVISTREAM& avi_stream オープンで取得したAVIファイルのストリーム
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
// AVIファイルの情報取得
// const char* filename AVIファイルフルパス
// DWORD* width 幅
// DWORD* height 高さ
// DWORD* bit_count １ピクセル当たりのビット数
// DWORD* nframes AVIファイルに含まれるフレーム数
//------------------------------------------
bool KizuAviManager::GetAVIInfo(const char* filename, DWORD* width, DWORD* height, DWORD* bit_count, DWORD* nframes)
{
    AVISTREAMINFO	info;
    PAVIFILE		file	= NULL;
    PAVISTREAM		stream	= NULL;

	// オープン
    if( ! OpenAVIFile(filename, file, stream, &info) ) return false;

	// データ取得
	*width   = info.rcFrame.right;
	*height  = info.rcFrame.bottom;
	*nframes = info.dwLength;

	// ビットカウント
    *bit_count = 0;
	BITMAPINFO* bminfo = GetAVIFormat(stream);
	if( NULL != bminfo ) {
		*bit_count = bminfo->bmiHeader.biBitCount;
		delete[] bminfo;
	}
    
	// クローズ
    CloseAVIFile(file, stream);
    return false;
}

//------------------------------------------
// AVIファイルのフォーマット情報取得
// PAVISTREAM& avi_stream オープンで取得したAVIファイルのストリーム
// 戻り値 ： 戻り値は内部でnewしているので、使い終わったら delete[] すること
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
// 通常の関数
// //////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// AVIファイルのオープン
// const char* filename AVIファイルフルパス
//------------------------------------------
bool KizuAviManager::OpenAVIFile(const char* filename)
{
	bool ret;

	// オープン
	ret = OpenAVIFile(filename, m_AviFile, m_AviStream, &m_AviInfo);
	if( ! ret) return false;	

	// 基本的な情報を取得しておく
	m_nW		= m_AviInfo.rcFrame.right;
	m_nH		= m_AviInfo.rcFrame.bottom;
	m_nFrameCnt = m_AviInfo.dwLength;

	// ビットカウント
    m_nBitCnt = 0;
	BITMAPINFO* bminfo = GetAVIFormat(m_AviStream);
	if( NULL != bminfo ) {
		m_nBitCnt = bminfo->bmiHeader.biBitCount;
		delete[] bminfo;
	}

	return true;
}

//------------------------------------------
// AVIファイルのクロース
//------------------------------------------
void KizuAviManager::CloseAVIFile()
{
	if( NULL == m_AviFile || NULL == m_AviStream) return;

	return CloseAVIFile(m_AviFile, m_AviStream);
}


//------------------------------------------
// AVIファイルの１フレーム読み込み
// long count 対象フレームNo (0オリジン)
// bool nTB 画像反転 true:反転 
// BYTE* img 画像データ
//------------------------------------------
bool KizuAviManager::LoadFromAVI(long count, BYTE* img, bool nTB) const
{
	DWORD len = m_nW * m_nH;
	HRESULT rc = AVIStreamRead(m_AviStream, count, 1, img, len, NULL, NULL);
	
	// 画像上下反転
	if(nTB) HantenTB(m_nW, m_nH, img);

	return SUCCEEDED(rc);
}

//------------------------------------------
// 上下を反転しながらコピーする
// DWORD nW 画像幅 
// DWORD nH 画像高
// BYTE *p 対象画像
//------------------------------------------
void KizuAviManager::HantenTB( DWORD nW, DWORD nH, BYTE *p )
{
	// ちょっとだけ早い
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
