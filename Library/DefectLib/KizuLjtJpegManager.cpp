
#include "Stdafx.h"
#include "KizuLjtJpegManager.h"
#include "tjheader.h"

#include <fstream>			

using namespace KizuLib;

template<typename T>
inline static bool GetProcAddressT(HINSTANCE hInst, const char *name, T &proc) {
	proc = (T)::GetProcAddress(hInst, name);
	return proc != NULL;
}

//------------------------------------------
// コンストラクタ
//------------------------------------------
KizuLjtJpegManager::KizuLjtJpegManager(void)
{
	if (p_tjInitCompress == NULL) {
		HINSTANCE hInst;
#ifdef _M_X64
		hInst = LoadLibraryA("turbojpeg_x64.dll");
#else
		hInst = LoadLibraryA("turbojpeg_x86.dll");
#endif
		if (hInst) {
			GetProcAddressT(hInst, "tjInitCompress", p_tjInitCompress);
			GetProcAddressT(hInst, "tjCompress2", p_tjCompress2);
			GetProcAddressT(hInst, "tjDestroy", p_tjDestroy);

			GetProcAddressT(hInst, "tjInitDecompress", p_tjInitDecompress);
			GetProcAddressT(hInst, "tjDecompressHeader3", p_tjDecompressHeader3);
			GetProcAddressT(hInst, "tjDecompress2", p_tjDecompress2);

		}
	}
	if (p_tjInitCompress)
	{
		tjEncode = p_tjInitCompress();
	}
	else {
		tjEncode = NULL;
	}
}


//------------------------------------------
// デストラクタ
//------------------------------------------
KizuLjtJpegManager::~KizuLjtJpegManager(void)
{
	if (tjEncode) {
		p_tjDestroy(tjEncode);
		tjEncode = NULL;
	}
}

//------------------------------------------
// BMP→JPEG 変換（グレイスケール）
// int nW BMP画像幅
// int nH BMP画像高さ
// const unsigned char* bP BMP画像の先頭ポインタ
// int jquality JPEG画像の品質 (0～100？)
// int* jSize JPEG変換後のサイズ (ヘッダ等も含まれている) 
// unsigned char* jP JPEG画像の先頭ポインタ (あらかじめ bW*bH 分の領域を確保する事)
// bool nTB 画像反転 true:反転
// 戻り値 0:正常 1～:各所エラー
//------------------------------------------
int KizuLjtJpegManager::BmpToJpeg(int nW, int nH, const unsigned char* bP, int jquality, int* jSize, unsigned char* jP, bool nTB)
{

	if (tjEncode == NULL)
		return 1;

	int outSubsamp = TJSAMP_GRAY;
	int pixelFormat = TJPF_GRAY;

	int flags = TJFLAG_NOREALLOC;
	if (nTB) {
		flags |= TJFLAG_BOTTOMUP;
	}
	//      flags |= TJFLAG_ACCURATEDCT;
	//      flags |= TJFLAG_FASTUPSAMPLE;

	BYTE *jpegBuf = jP;
	int stride = (nW + 3) & ~3;
	unsigned long jpegSize = *jSize;

	if (p_tjCompress2(tjEncode, bP, nW, stride, nH, pixelFormat,
		&jpegBuf, &jpegSize, outSubsamp, jquality, flags) < 0) {
		return 2;
	}

	*jSize = jpegSize;
	return 0;
}

//------------------------------------------
// BMP→JPEG 変換　(カラー版)
// int nW BMP画像幅
// int nH BMP画像高さ
// const unsigned char* bP BMP画像の先頭ポインタ
// int jquality JPEG画像の品質 (0～100？)
// int* jSize JPEG変換後のサイズ (ヘッダ等も含まれている) 
// unsigned char* jP JPEG画像の先頭ポインタ (あらかじめ bW*bH*3 分の領域を確保する事)
// bool nTB 画像反転 true:反転
// 戻り値 0:正常 1～:各所エラー
//------------------------------------------
int KizuLjtJpegManager::BmpToJpegColor(int nW, int nH, const unsigned char* bP, int jquality, int* jSize, unsigned char* jP, bool nTB)
{
	if (tjEncode == NULL)
		return 1;

	int outSubsamp = TJSAMP_411;
	int pixelFormat = TJPF_RGB;
	//int pixelFormat = TJPF_BGR;

	int flags = TJFLAG_NOREALLOC;
	if (nTB) {
		flags |= TJFLAG_BOTTOMUP;
	}
	//      flags |= TJFLAG_ACCURATEDCT;
	//      flags |= TJFLAG_FASTUPSAMPLE;

	BYTE *jpegBuf = jP;
	int stride = (nW * 3 + 3) &~3;
	unsigned long jpegSize = *jSize; // bW*bH*3 ?

	if (p_tjCompress2(tjEncode, bP, nW, stride, nH, pixelFormat,
		&jpegBuf, &jpegSize, outSubsamp, jquality, flags) < 0) {
		return 2;
	}

	*jSize = jpegSize;
	return 0;


}




//------------------------------------------
// JPEG→BMP 変換
// const unsigned char* jP JPEG画像の先頭ポインタ
// int jSize JPEGのサイズ
// int nW BMP画像幅
// int nH BMP画像高さ
// unsigned char* bP デコード後のBMP画像のデータ部の先頭ポインタ (あらかじめ bW*bH 分の領域を確保する事)
// bool nTB 画像反転 true:反転
// 戻り値 0:正常 1～:各所エラー
//------------------------------------------
int KizuLjtJpegManager::JpegToBmp(const unsigned char* jP, int jSize, int nW, int nH, unsigned char* bP, bool nTB)
{
	if (p_tjInitDecompress == NULL)
		return 1;

	tjhandle tjInstance = p_tjInitDecompress();
	if (tjInstance == NULL)
		return 2;
	int pixelFormat = TJPF_GRAY;

	int flags = TJFLAG_NOREALLOC;
	if (nTB) {
		flags |= TJFLAG_BOTTOMUP;
	}

	int stride = (nW + 3) &~3;

	//	tjDecompressHeader3();
	if (p_tjDecompress2(
		tjInstance,
		jP, jSize,
		bP, nW, stride, nH,
		pixelFormat, flags)) {
		p_tjDestroy(tjInstance);
		return 3;
	}
	p_tjDestroy(tjInstance);
	return 0;
}


//------------------------------------------
// JPEG→BMP 変換 (カラー版)
// const unsigned char* jP JPEG画像の先頭ポインタ
// int jSize JPEGのサイズ
// int nW BMP画像幅
// int nH BMP画像高さ
// unsigned char* bP デコード後のBMP画像のデータ部の先頭ポインタ (あらかじめ bW*bH*3 分の領域を確保する事)
// bool nTB 画像反転 true:反転
// 戻り値 0:正常 1～:各所エラー
//------------------------------------------
int KizuLjtJpegManager::JpegToBmpColor(const unsigned char* jP, int jSize, int nW, int nH, unsigned char* bP, bool nTB)
{
	if (p_tjInitDecompress == NULL)
		return 1;

	tjhandle tjInstance = p_tjInitDecompress();
	if (tjInstance == NULL)
		return 2;

	int pixelFormat = TJPF_RGB;
	//int pixelFormat = TJPF_BGR;

	int flags = TJFLAG_NOREALLOC;
	if (nTB) {
		flags |= TJFLAG_BOTTOMUP;
	}
	int stride = (nW * 3 + 3) &~3;

	//	tjDecompressHeader3();
	if (p_tjDecompress2(
		tjInstance,
		jP, jSize,
		bP, nW, stride, nH,
		pixelFormat, flags)) {
		p_tjDestroy(tjInstance);
		return 3;
	}

	p_tjDestroy(tjInstance);
	return 0;
}



//------------------------------------------
// JPEGヘッダからサイズなどよみこみ
// jP, jSize：Jpegのバッファと長さ
// nW, nH : 幅・高さ(Px) NULL不可
// bColored : モノクロならFalse、カラーならtrue
//------------------------------------------
int KizuLjtJpegManager::JpegToHeader(const unsigned char* jP, int jSize, int *nW, int *nH, bool *bColored)
{
	if (p_tjInitDecompress == NULL)
		return 1;


	tjhandle tjInstance = p_tjInitDecompress();
	if (tjInstance == NULL)
		return 2;
	//	tjDecompressHeader3();


	int subsamp = 0;
	int colorspace = 0;
	if (p_tjDecompressHeader3(
		tjInstance,
		jP, jSize,
		nW, nH,
		&subsamp, &colorspace)) {
		p_tjDestroy(tjInstance);
		return 3;
	}
	*bColored = subsamp != TJSAMP_GRAY;

	p_tjDestroy(tjInstance);
	return 0;
}


//------------------------------------------
// BMP→JPEG 変換後ファイル書き込み
// int nW BMP画像幅
// int nH BMP画像高さ
// const BYTE* bP BMP画像の先頭ポインタ
// int jquality JPEG画像の品質 (0～100？)
// const char* Path 保存ファイルパス
// bool nTB 画像反転 true:反転
// 戻り値 0:正常 1～:各所エラー
//------------------------------------------
int KizuLjtJpegManager::BmpToJPEGfile(int nW, int nH, const BYTE* bP, int jquality, const char* Path, bool nTB)
{
	int	jSize = ((nW * nH) + 3) & ~3;
	if (0 >= jSize) {
		return 1;
	}
	BYTE* jP = new BYTE[jSize];		// BMPのサイズよりは大きくならないはず

	// BMP→JPEG 変換
	int nRtn = BmpToJpeg(nW, nH, bP, jquality, &jSize, jP, nTB);

	// ファイル書込み
	std::ofstream   os;

	os.open(Path, std::ios::out | std::ios::binary);
	os.write((const char*)jP, jSize);
	os.close();

	delete[] jP;

	return nRtn;
}

