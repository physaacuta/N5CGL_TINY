#include "StdAfx.h"
#include "FrameDataManager.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


//------------------------------------------
// コンストラクタ
//------------------------------------------
FrameDataManager::FrameDataManager(void)
{
}

//------------------------------------------
// デストラクタ
//------------------------------------------
FrameDataManager::~FrameDataManager(void)
{
}

//------------------------------------------
// 器生成時 (IportManagerでInitされた時コールバック)
// int const* p=0 受け渡しカスタム情報(int 配列)   (0～1までは、固定。継承元で使用するから)
	// [0] 画像サイズ (本来、[1]*[2]でいいが、よく使うから渡しておく)   (本システムは、1角度のみ)
	// [1] カメラ角度数をセットしておくこと (互換の為に、0の場合は、強制的に1にする)
	// [2] 1フレーム部のデータサイズ (割り切れる数)
	// [3] 書込みモード (0:同期書込みモード 1:セクタ書込みモード)
	// [4] 関数ポインタ(64bit関数時は、8Byte分使用)
	// [5] 関数ポインタ
//------------------------------------------
void FrameDataManager::Alloc(int const* p)
{
	//// 一番最初に継承元のエリアを確保&初期化
	FRAME_DATA::Alloc(p);

	//// 自分で使う分の引数取り出し
	nBufSize		= p[2];
	nWriteMode		= p[3];

	//// 自分のエリアを確保
	_ASSERT( 0 == nBufSize%512);
	pBuf		= (BYTE*)VirtualAlloc(NULL, nBufSize, MEM_COMMIT, PAGE_READWRITE);
}

//------------------------------------------
// 器解放時 (IportManagerでDisposeされた時コールバック)
//------------------------------------------
void FrameDataManager::Free()
{
	//// 一番最初に継承元のエリアを解放
	FRAME_DATA::Free();

	//// 自分のエリアを解放
	VirtualFree(pBuf, 0, MEM_RELEASE);
}

//------------------------------------------
// メンバー初期化
//------------------------------------------
void FrameDataManager::Clear()
{
	//// 毎回セットしないもののみ
	// スキップするため
	this->cls_Time.PutTimestamp( TIME_ImgComp_START,	(__int64)0);
	this->cls_Time.PutTimestamp( TIME_ImgComp_END, 		(__int64)0);
	this->cls_Time.PutTimestamp( TIME_RpiWrite_START, 	(__int64)0);
	this->cls_Time.PutTimestamp( TIME_RpiWrite_END, 	(__int64)0);


}