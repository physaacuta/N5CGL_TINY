#include "Stdafx.h"
#include "KizuJpegManager.h"

using namespace KizuLib;

//------------------------------------------
// コンストラクタ
//------------------------------------------
KizuJpegManager::KizuJpegManager(void)
{
}

//------------------------------------------
// デストラクタ
//------------------------------------------
KizuJpegManager::~KizuJpegManager(void)
{
}

//------------------------------------------
// BMP→JPEG 変換（グレイスケール）
// int nW BMP画像幅
// int nH BMP画像高さ
// const BYTE* bP BMP画像の先頭ポインタ
// int jquality JPEG画像の品質 (0～100？)
// int* jSize JPEG変換後のサイズ (ヘッダ等も含まれている) 
// BYTE* jP JPEG画像の先頭ポインタ (あらかじめ bW*bH 分の領域を確保する事)
// bool nTB 画像反転 true:反転
// 戻り値 0:正常 1～:各所エラー
//------------------------------------------
int KizuJpegManager::BmpToJpeg(int nW, int nH, const BYTE* bP, int jquality, int* jSize, BYTE* jP, bool nTB)
{
#ifdef DEFECTLIB_KIZUJPEGMANAGER
	IJLERR ret;

	////// 初期化
	JPEG_CORE_PROPERTIES jcprops;			// JPEG変換データ引渡し用構造体
	ret = ijlInit(&jcprops);
	if( IJL_OK != ret) {
		ijlFree(&jcprops);
		return 1;
	}

	////// 構造体にデータセット (画像処理で上下反対となるらしい。DIBHeightをマイナスにする) ← 判定ダミーツールで画像を読み込んで送信した時のみ？(本番ではマイナスにしたらダメ)
	jcprops.DIBWidth = nW;						// 画像の横サイズ
	jcprops.DIBHeight = (nTB ? -nH : nH); //nH;				// 画像の縦サイズ (マイナスを付ける)
	jcprops.DIBBytes = (UCHAR*)bP;				// 非圧縮画像へのポインタ
	jcprops.DIBChannels = 1;					// モノクロなので1
	jcprops.DIBColor = IJL_G;					// モノクロなのでIJL_G
	jcprops.DIBPadBytes = 0;					// 画像の端の余りバイト数(幅が2のべき乗)
	jcprops.JPGChannels = 1;					// モノクロなので1
	jcprops.JPGColor = IJL_G;					// モノクロなのでIJL_G
	jcprops.JPGSubsampling = IJL_NONE;			// モノクロなのでサブサンプリング無し
	jcprops.jquality = jquality;				// 品質
	jcprops.JPGFile = NULL;						// ファイル出力の場合のファイル名
	jcprops.JPGBytes = jP;						// メモリ出力の場合のバッファへのポインタ
	jcprops.JPGWidth = nW;						// 出力画像サイズ = 入力と同じ
	jcprops.JPGHeight = nH;						// 出力画像サイズ = 入力と同じ
	jcprops.JPGSizeBytes = nW * nH;				// 変換後のバイト数
	
	////// 圧縮
	ret = ijlWrite(&jcprops, IJL_JBUFF_WRITEWHOLEIMAGE);		// バッファ格納

	////// 開放
	ijlFree(&jcprops);

	// 圧縮のアンサーチェック
	if(IJL_OK == ret) {				// 正常
		*jSize = jcprops.JPGSizeBytes;			// 画像サイズがセットされている
		return 0;
	} else {						// 異常
		*jSize = 0;
		return 2;
	}
#else
	// とりあえずそのままのデータを返すようにする
	_ASSERT(false);
	*jSize = nW*nH;
	jP = (BYTE*)bP;
	return 0;
#endif
}

//------------------------------------------
// BMP→JPEG 変換　(カラー版)
// int nW BMP画像幅
// int nH BMP画像高さ
// const BYTE* bP BMP画像の先頭ポインタ
// int jquality JPEG画像の品質 (0～100？)
// int* jSize JPEG変換後のサイズ (ヘッダ等も含まれている) 
// BYTE* jP JPEG画像の先頭ポインタ (あらかじめ bW*bH*3 分の領域を確保する事)
// bool nTB 画像反転 true:反転
// 戻り値 0:正常 1～:各所エラー
//------------------------------------------
int KizuJpegManager::BmpToJpegColor(int nW, int nH, const BYTE* bP, int jquality, int* jSize, BYTE* jP, bool nTB)
{
#ifdef DEFECTLIB_KIZUJPEGMANAGER
	IJLERR ret;

	////// 初期化
	JPEG_CORE_PROPERTIES jcprops;			// JPEG変換データ引渡し用構造体
	ret = ijlInit(&jcprops);
	if( IJL_OK != ret) {
		ijlFree(&jcprops);
		return 1;
	}

	////// 構造体にデータセット (画像処理で上下反対となるらしい。DIBHeightをマイナスにする) ← 判定ダミーツールで画像を読み込んで送信した時のみ？(本番ではマイナスにしたらダメ)
	jcprops.DIBWidth = nW;						// 画像の横サイズ
	jcprops.DIBHeight = (nTB ? -nH : nH); //nH;				// 画像の縦サイズ (マイナスを付ける)
	jcprops.DIBBytes = (UCHAR*)bP;				// 非圧縮画像へのポインタ
	jcprops.DIBChannels = 3;					// カラーデフォルト
	jcprops.DIBColor = IJL_RGB;					// カラーデフォルト
	jcprops.DIBPadBytes = 0;					// 画像の端の余りバイト数(幅が2のべき乗)
	jcprops.JPGChannels = 3;					// カラーデフォルト
	jcprops.JPGColor = IJL_YCBCR;				// カラーデフォルト
	jcprops.JPGSubsampling = IJL_411;			// カラーデフォルト
	jcprops.jquality = jquality;				// 品質
	jcprops.JPGFile = NULL;						// ファイル出力の場合のファイル名
	jcprops.JPGBytes = jP;						// メモリ出力の場合のバッファへのポインタ
	jcprops.JPGWidth = nW;						// 出力画像サイズ = 入力と同じ
	jcprops.JPGHeight = nH;						// 出力画像サイズ = 入力と同じ
	jcprops.JPGSizeBytes = nW * nH;				// 変換後のバイト数
	
	////// 圧縮
	ret = ijlWrite(&jcprops, IJL_JBUFF_WRITEWHOLEIMAGE);		// バッファ格納

	////// 開放
	ijlFree(&jcprops);

	// 圧縮のアンサーチェック
	if(IJL_OK == ret) {				// 正常
		*jSize = jcprops.JPGSizeBytes;			// 画像サイズがセットされている
		return 0;
	} else {						// 異常
		*jSize = 0;
		return 2;
	}
#else
	// とりあえずそのままのデータを返すようにする
	_ASSERT(false);
	*jSize = nW*nH;
	jP = (BYTE*)bP;
	return 0;
#endif
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
int KizuJpegManager::BmpToJPEGfile(int nW, int nH, const BYTE* bP, int jquality, const char* Path, bool nTB)
{
#ifdef DEFECTLIB_KIZUJPEGMANAGER
	IJLERR ret;

	////// 初期化
	JPEG_CORE_PROPERTIES jcprops;			// JPEG変換データ引渡し用構造体
	ret = ijlInit(&jcprops);
	if( IJL_OK != ret) {
		ijlFree(&jcprops);
		return 1;
	}

	////// 構造体にデータセット (画像処理で上下反対となるらしい。DIBHeightをマイナスにする) ← 判定ダミーツールで画像を読み込んで送信した時のみ？(本番ではマイナスにしたらダメ？)
	jcprops.DIBWidth = nW;						// 画像の横サイズ
	jcprops.DIBHeight = (nTB ? -nH : nH);//-nH;					// 画像の縦サイズ (マイナスを付ける)
	jcprops.DIBBytes = (UCHAR*)bP;				// 非圧縮画像へのポインタ
	jcprops.DIBChannels = 1;					// モノクロなので1
	jcprops.DIBColor = IJL_G;					// モノクロなのでIJL_G
	jcprops.DIBPadBytes = 0;					// 画像の端の余りバイト数(幅が2のべき乗)
	jcprops.JPGChannels = 1;					// モノクロなので1
	jcprops.JPGColor = IJL_G;					// モノクロなのでIJL_G
	jcprops.JPGSubsampling = IJL_NONE;			// モノクロなのでサブサンプリング無し
	jcprops.jquality = jquality;				// 品質
	jcprops.JPGFile = Path;						// ファイル出力の場合のファイル名
	jcprops.JPGWidth = nW;						// 出力画像サイズ = 入力と同じ
	jcprops.JPGHeight = nH;						// 出力画像サイズ = 入力と同じ
	
	////// 圧縮
	ret = ijlWrite(&jcprops, IJL_JFILE_WRITEWHOLEIMAGE);		// ファイル格納

	////// 開放
	ijlFree(&jcprops);

	// 圧縮のアンサーチェック
	if(IJL_OK == ret) {				// 正常
		return 0;
	} else {						// 異常
		return 2;
	}
#else
	return 0;
#endif
}


//------------------------------------------
// JPEG→BMP 変換
// const BYTE* jP JPEG画像の先頭ポインタ
// int jSize JPEGのサイズ
// int nW BMP画像幅
// int nH BMP画像高さ
// BYTE* bP デコード後のBMP画像のデータ部の先頭ポインタ (あらかじめ bW*bH 分の領域を確保する事)
// bool nTB 画像反転 true:反転
// 戻り値 0:正常 1～:各所エラー
//------------------------------------------
int KizuJpegManager::JpegToBmp(const BYTE* jP, int jSize, int nW, int nH, BYTE* bP, bool nTB )
{
#ifdef DEFECTLIB_KIZUJPEGMANAGER
	IJLERR ret;

	////// 初期化
	JPEG_CORE_PROPERTIES jcprops;			// JPEG変換データ引渡し用構造体
	ret = ijlInit(&jcprops);
	if( IJL_OK != ret) {
		ijlFree(&jcprops);
		return 1;
	}

	////// 構造体にデータセット (画像処理で上下反対となるらしい。DIBHeightをマイナスにする) ← 判定ダミーツールで画像を読み込んで送信した時のみ？(本番ではマイナスにしたらダメ？)
	jcprops.DIBWidth = nW;						// 変換後の画像の横サイズ
	jcprops.DIBHeight = (nTB ? -nH : nH);//-nH;					// 変換後の画像の縦サイズ (マイナスを付ける)
	jcprops.DIBBytes = bP;						// 変換後のBMP画像へのポインタ
	jcprops.DIBChannels = 1;					// モノクロなので1
	jcprops.DIBColor = IJL_G;					// モノクロなのでIJL_G
	jcprops.DIBPadBytes = 0;					// 画像の端の余りバイト数(幅が2のべき乗)
	jcprops.JPGChannels = 1;					// モノクロなので1
	jcprops.JPGColor = IJL_G;					// モノクロなのでIJL_G
	jcprops.JPGSubsampling = IJL_NONE;			// モノクロなのでサブサンプリング無し
	jcprops.jquality = 0;						// 品質 (デコードの際は設定必要なし)
	jcprops.JPGFile = NULL;						// ファイル出力の場合のファイル名
	jcprops.JPGBytes = (UCHAR*)jP;				// メモリのバッファへのポインタ
	jcprops.JPGWidth = nW;						// 画像サイズ
	jcprops.JPGHeight = nH;						// 画像サイズ
	jcprops.JPGSizeBytes = jSize;				// 変換後のバイト数
	
	////// 圧縮
	ret = ijlRead(&jcprops, IJL_JBUFF_READWHOLEIMAGE);		// バッファ格納

	////// 開放
	ijlFree(&jcprops);

	// 圧縮のアンサーチェック
	if(IJL_OK == ret) {				// 正常
		return 0;
	} else {						// 異常
		return 2;
	}
#else
	return 0;
#endif
}
/*
//------------------------------------------
// JPEGファイル→BMP 変換
// const char* Path 保存ファイルパス
// int* nW BMP画像幅
// int* nH BMP画像高さ
// BYTE* bP デコード後のBMP画像のデータ部の先頭ポインタ (この関数無いで領域確保)
// 戻り値 0:正常 1～:各所エラー
//------------------------------------------
int KizuJpegCreat::BMPtoJPEGfile(int nW, int nH, const BYTE* bP, int jquality, const char* Path)
{
#ifdef DEFECTLIB_KIZUJPEGMANAGER
	IJLERR ret;

	////// 初期化
	JPEG_CORE_PROPERTIES jcprops;			// JPEG変換データ引渡し用構造体
	ret = ijlInit(&jcprops);
	if( IJL_OK != ret) {
		ijlFree(&jcprops);
		return 1;
	}

	////// 構造体にデータセット (画像処理で上下反対となるらしい。DIBHeightをマイナスにする) ← 判定ダミーツールで画像を読み込んで送信した時のみ？(本番ではマイナスにしたらダメ？)
	jcprops.DIBWidth = nW;						// 画像の横サイズ
	jcprops.DIBHeight = nH;	//-bH;				// 画像の縦サイズ (マイナスを付ける)
	jcprops.DIBBytes = bP;						// 非圧縮画像へのポインタ
	jcprops.DIBChannels = 1;					// モノクロなので1
	jcprops.DIBColor = IJL_G;					// モノクロなのでIJL_G
	jcprops.DIBPadBytes = 0;					// 画像の端の余りバイト数(幅が2のべき乗)
	jcprops.JPGChannels = 1;					// モノクロなので1
	jcprops.JPGColor = IJL_G;					// モノクロなのでIJL_G
	jcprops.JPGSubsampling = IJL_NONE;			// モノクロなのでサブサンプリング無し
	jcprops.jquality = jquality;				// 品質
	jcprops.JPGFile = Path;						// ファイル出力の場合のファイル名
	jcprops.JPGWidth = nW;						// 出力画像サイズ = 入力と同じ
	jcprops.JPGHeight = nH;						// 出力画像サイズ = 入力と同じ
	
	////// 圧縮
	ret = ijlWrite(&jcprops, IJL_JFILE_WRITEWHOLEIMAGE);		// ファイル格納

	////// 開放
	ijlFree(&jcprops);

	// 圧縮のアンサーチェック
	if(IJL_OK == ret) {				// 正常
		return 0;
	} else {						// 異常
		return 2;
	}
#endif
}
*/


//------------------------------------------
// JPEG→BMP 変換 (カラー版)
// const BYTE* jP JPEG画像の先頭ポインタ
// int jSize JPEGのサイズ
// int nW BMP画像幅
// int nH BMP画像高さ
// BYTE* bP デコード後のBMP画像のデータ部の先頭ポインタ (あらかじめ bW*bH*3 分の領域を確保する事)
// bool nTB 画像反転 true:反転
// 戻り値 0:正常 1～:各所エラー
//------------------------------------------
int KizuJpegManager::JpegToBmpColor(const BYTE* jP, int jSize, int nW, int nH, BYTE* bP, bool nTB )
{
#ifdef DEFECTLIB_KIZUJPEGMANAGER
	IJLERR ret;

	////// 初期化
	JPEG_CORE_PROPERTIES jcprops;			// JPEG変換データ引渡し用構造体
	ret = ijlInit(&jcprops);
	if( IJL_OK != ret) {
		ijlFree(&jcprops);
		return 1;
	}

	////// 構造体にデータセット (画像処理で上下反対となるらしい。DIBHeightをマイナスにする) ← 判定ダミーツールで画像を読み込んで送信した時のみ？(本番ではマイナスにしたらダメ？)
	jcprops.DIBWidth = nW;						// 変換後の画像の横サイズ
	jcprops.DIBHeight = (nTB ? -nH : nH);//-nH;					// 変換後の画像の縦サイズ (マイナスを付ける)
	jcprops.DIBBytes = bP;						// 変換後のBMP画像へのポインタ
	jcprops.DIBChannels = 3;					// カラーデフォルト
	jcprops.DIBColor = IJL_RGB;					// カラーデフォルト
	jcprops.DIBPadBytes = 0;					// 画像の端の余りバイト数(幅が2のべき乗)
	jcprops.JPGChannels = 3;					// カラーデフォルト
	jcprops.JPGColor = IJL_YCBCR;				// カラーデフォルト
	jcprops.JPGSubsampling =  IJL_411;			// カラーデフォルト
	jcprops.jquality = 0;						// 品質 (デコードの際は設定必要なし)
	jcprops.JPGFile = NULL;						// ファイル出力の場合のファイル名
	jcprops.JPGBytes = (UCHAR*)jP;				// メモリのバッファへのポインタ
	jcprops.JPGWidth = nW;						// 画像サイズ
	jcprops.JPGHeight = nH;						// 画像サイズ
	jcprops.JPGSizeBytes = jSize;				// 変換後のバイト数
	
	////// 圧縮
	ret = ijlRead(&jcprops, IJL_JBUFF_READWHOLEIMAGE);		// バッファ格納

	////// 開放
	ijlFree(&jcprops);

	// 圧縮のアンサーチェック
	if(IJL_OK == ret) {				// 正常
		return 0;
	} else {						// 異常
		return 2;
	}
#else
	return 0;
#endif
}
