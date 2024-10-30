#include "StdAfx.h"
#include "RpiFileManager.h"

#include "../../libPG/libRpi/RpiWriter.h"

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// コンストラクタ
// char const* cThreadName 自スレッド名称
//------------------------------------------
RpiFileManager::RpiFileManager(char const* cThreadName) :
ThreadManager(cThreadName),
my_sFullPath(""),
my_sFileName(""),
my_nWriteFrameCnt(0),
my_nSaveFolderIndex(0)
{
	//// メンバー初期化
	memset(&my_Setting, 0x00, sizeof(my_Setting));

	my_TimeWrite[0] = 0.0;
	my_TimeWrite[1] = 9999.9;
	my_TimeWrite[2] = 0.0;

	this->my_pWriter = new TRpiWriter();
}

//------------------------------------------
// デストラクタ
//------------------------------------------
RpiFileManager::~RpiFileManager(void)
{
	delete my_pWriter;
}

bool				RpiFileManager::IsOpen() const
{
	return my_pWriter->IsOpen();
}

RpiLib::RpiHeader const*	RpiFileManager::GetRpiHeader() const{
	return &my_pWriter->Header;
}

//------------------------------------------
// HDDのセクタサイズを取得する
// char const* cDrivePath 調査したいドライブパス。フォルダパスでもOK
// 戻り値 ： セクタサイズ
//------------------------------------------
int RpiFileManager::GetSecSize(char const* cDrivePath)
{
	DWORD	n1;		// クラスタ当たりのセクタ数
	DWORD	n2;		// セクタ当たりのバイト数
	DWORD	n3;		// 空きクラスタ数
	DWORD	n4;		// 全クラスタ数

//	GetDiskFreeSpace(cDrivePath, lpSectorsPerCluster, lpBytesPerSector, lpNumberOfFreeClusters, lpTotalNumberOfClusters);
	if( GetDiskFreeSpace(cDrivePath, &n1, &n2, &n3, &n4) ) {
		return n2;
	} else {	
		return 512;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 処理

//------------------------------------------
// １コイル単位の初めてのファイルの生成
//------------------------------------------
bool RpiFileManager::RpiOpen()
{
	CString			sWk;
	RpiHeader	h;

	//================================================
	// 前準備
	// オープンチェック
	if( IsOpen() ) return false;

	// ヘッダー生成
	CreateHeader(h);

	// フルパス決定
	OnSetFileName(h, my_Setting, my_CoilInfo, &sWk);	// ファイル名の決定


	//================================================
	// ファイルオープン
	if( ! Open(sWk, &h)) {
		RpiClose();	
		return false;
	}
	LOG(em_MSG), "[%s] RPIファイルオープン完了(%s)", my_sThreadName, sWk);

	//================================================
	// 初回の準備

	my_TimeWrite[0] = 0.0;
	my_TimeWrite[1] = 9999.9;
	my_TimeWrite[2] = 0.0;
	return true;
}

//------------------------------------------
// ファイル終了
//------------------------------------------
bool RpiFileManager::RpiClose()
{
	// ファイルの後始末
	if( IsOpen() ) {
		my_pWriter->CloseFile();
		LOG(em_MSG), "[%s] RPIファイルクローズ完了(%s) =>%s ", my_sThreadName,  my_sFileName, my_pWriter->GetError());

		// RPIファイルが完全に出来上がった為、通知
		OnRpiFileSave( my_Setting.cFolderPath[my_nSaveFolderIndex], &my_sFileName);

		// 次フォルダに切替
		my_nSaveFolderIndex = (my_nSaveFolderIndex+1) % my_Setting.nFolderNum;

	}
	return true;
}


//------------------------------------------
// フレーム情報書込み
// int nImgSize 画像データ部のみのサイズ
// int nFrameSize 1フレーム分の書込みサイズ (セクタの倍数)
// BYTE* pFrame 1フレーム情報 書込みデータ
//------------------------------------------
bool RpiFileManager::WriteFrame(int nImgSize, int nFrameSize, BYTE* pFrame)
{
	//// １コイル内ファイル分割

	T_RPI_HEADER_BASE &tH = my_pWriter->theader;
	int nframe = my_pWriter->Header.nframe;


	if( 0 != tH.nFileNum) {
		if( (1==tH.nFileNum && 0!=my_Setting.nFastFrame && nframe >= my_Setting.nFastFrame ) ||
			(1!=tH.nFileNum && 0!=my_Setting.nCycleFrame && nframe >= my_Setting.nCycleFrame ) ) {

			// 次ファイル
			Next();
		}
	}

	//// 書込み
	mycls_Time.PutTimestamp(0);
	my_pWriter->WriteFrameOpt( pFrame, nImgSize, NULL );
	mycls_Time.PutTimestamp(1);

	//// 書込み時間計測
	double dWk = mycls_Time.GetTimestamp(1,0);
	dWk = ((double)nFrameSize/1024.0/1024.0 ) / (dWk/1000.0);		// [MByte/s]
	my_TimeWrite[0] = my_TimeWrite[0]*0.8 + dWk*0.2;
	my_TimeWrite[1] = __min(my_TimeWrite[1], dWk);
	my_TimeWrite[2] = __max(my_TimeWrite[2], dWk);
	return true;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 内部処理

//------------------------------------------
// ヘッダー情報生成
// RpiHeader& h 生成エリア
//------------------------------------------
void RpiFileManager::CreateHeader(RpiHeader& h)
{
	T_RPI_HEADER_BASE &tH = h.theader;

	h.codec			= my_Setting.codec;
	h.width			= my_Setting.width;
	h.height			= my_Setting.height;
	h.nchannel			= 1;						// 現状グレー画像のみ対応
	h.pixel_nbit		= 8;						// 1画素8bitのみ対応
	h.pitch_nbyte		= my_Setting.width;			// 幅と同一画像のみ対応
	h.nframe			= 0;						// 後で更新
	h.reso_x			= my_Setting.reso_x;
	h.reso_y			= my_CoilInfo.reso_y;		

	if (0 >= h.reso_y) {
		LOG(em_ERR), "[%s] 縦分解能未設定。バックアップにより設定変更(%f→%f)", my_sThreadName, my_CoilInfo.reso_y, h.reso_x);
		h.reso_y = h.reso_x;		// バックアップ処置
	}
		LOG(em_INF), "[%s] 縦分解能未設定。バックアップにより設定変更(%f→%f)", my_sThreadName, h.reso_y, h.reso_x);


	// TEX部
	memcpy(tH.cSysName, my_Setting.cSysName, sizeof(tH.cSysName));
	memcpy(tH.cKizukenNo, my_CoilInfo.cKizukenNo, sizeof(tH.cKizukenNo));
	tH.frame_info_type	= my_Setting.frame_info_type;
	tH.frame_info_size	= my_Setting.frame_info_size;
	tH.nFrameNumInit	= my_CoilInfo.nCFNo;
	tH.nFrameNumStart	= my_CoilInfo.nCFNo;
	tH.nFileNum			= 0==my_Setting.nFastFrame ? 0 : 1;
	tH.nLineCenter		= my_Setting.nLineCenter;
	tH.nStartCamset		= my_Setting.nStartCamset;
	tH.nCamsetNum		= my_Setting.nCamsetNum;
	tH.nCamAngle		= my_Setting.nCamAngle;
	tH.nMen				= my_Setting.nMen;

	for(int ii=0; ii<my_Setting.nCamsetNum; ii++) {
		tH.caminfo[ii].dBaseXoffset	= my_Setting.caminfo[ii].dBaseXoffset;
		tH.caminfo[ii].nBaseLeft	= my_Setting.caminfo[ii].nBaseLeft; 
		tH.caminfo[ii].nOverLap		= my_Setting.caminfo[ii].nOverLap;
	}
	tH.nStartLen			= my_CoilInfo.nStartLen;
	tH.nColorType			= my_Setting.nColorType;
	tH.nSysType				= my_Setting.nSysType;

	tH.nFTposCfNo[0]		= my_CoilInfo.nFTposCfNo[0];
	tH.nFTposPixel[0]		= my_CoilInfo.nFTposPixel[0];
	tH.nFTposCfNo[1]		= 0;		// 尾端は後で更新
	tH.nFTposPixel[1]		= 0;
}

//------------------------------------------
// ヘッダー情報更新(コイル確定時)
// RpiHeader& h 生成エリア
//------------------------------------------
void RpiFileManager::CreateHeader_Ending(RpiHeader& h)
{
	h.theader.nFTposCfNo[1]		= my_CoilInfo.nFTposCfNo[1];
	h.theader.nFTposPixel[1]	= my_CoilInfo.nFTposPixel[1];
	if (h.reso_y != my_CoilInfo.reso_y) {
		LOG(em_INF), "[%s] コイル確定時に縦分解能更新 (%f→%f)", my_sThreadName, h.reso_y, my_CoilInfo.reso_y);
		h.reso_y = my_CoilInfo.reso_y;
	}

}

//------------------------------------------
// ファイルの生成
// CString sName ファイル名
// RpiHeader* h ヘッダー情報
//------------------------------------------
bool RpiFileManager::Open(CString sName, RpiHeader* h) {

	//// セクタ単位の同期書込み
	// FILE_FLAG_NO_BUFFERING によるセクタ単位の書込み。 512単位とかになるので、ファイルサイズが大きくなるが、
	// その分一番高速と思われる。
	
	// どうも最近のキャッシュ搭載のHDD,SSDだと、FILE_FLAG_NO_BUFFERINGで
	// キャッシュ無しで即時書込みとかすると、同時に読み込む時などで
	// ロックされてしまうような感じとなる。

	// 結果として、何も無しが一番早いかも・・・ 今後も引き続き調査


	CString sFPath;
	sFPath.Format("%s\\%s", my_Setting.cFolderPath[my_nSaveFolderIndex], sName);

	if ( !my_pWriter->OpenFile( sFPath, *h, 1 == my_Setting.nWriteMode) ){
		LOG(em_ERR), "[%s] RPIファイル生成失敗=%s", my_sThreadName, sFPath);
		return false;
	}

	//// ローカル変数 初期化
	//my_sFullPath		= sFPath;
	CStringCS::LockCpy(&my_sFullPath, sFPath);		// DlgでCStringを参照するため、排他処理を入れる
	my_sFileName		= sName;
	return true;
}
//------------------------------------------
// 1コイル内でのファイル切り替え
//------------------------------------------
bool RpiFileManager::Next()
{

	//================================================
	// 前準備
	
	// 今のファイルを閉じる
	RpiClose();

	// 次ファイルのために、ヘッダーを更新する
	RpiHeader	h = my_pWriter->Header;
	h.nframe = 0;
	h.theader.nFileNum ++;
	h.theader.nFrameNumStart = my_FrameInfo.nFNo; 

	// ファイル名決定
	CString			sWk;
	OnSetFileName(h, my_Setting, my_CoilInfo, &sWk);		// ファイル名の決定
	if( 0 >= sWk.GetLength() ) return false;

	//================================================
	// ファイルオープン
	if( ! Open(sWk, &h)) {
		RpiClose();	
		return false;
	}

	return true;
}