#include "StdAfx.h"
#include "AvalACL2Base.h"


#include <AcapLib2.h>
#pragma comment(lib, "AcapLib2.lib")


// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

using namespace AvalLib;


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ローカル関数


//------------------------------------------
// コンストラクタ
//------------------------------------------
AvalACL2Base::AvalACL2Base() :

my_hDev(INVALID_HANDLE_VALUE), my_nBoardChId(0),
my_nWidth(0), my_nHeight(0), my_nPixelNBit(0),
my_bRunning(false),
my_nBufNum(0), my_nBufSize(0), my_pBuff(NULL)
{
	ClearStatCounters();
}

//------------------------------------------
// デストラクタ
//------------------------------------------
AvalACL2Base::~AvalACL2Base(void)
{

}

//------------------------------------------
// 初期化
// int board_id		ボード番号(0オリジン)
// int board_ch_id	オープンチャンネル (1オリジン) ※0:全CH
// const char* ini_filename	設定ファイルフルパス
// int nbuff		バッファ数
//------------------------------------------
BOOL AvalACL2Base::Init(int board_id, int board_ch_id, const char* ini_filename, int nbuff)
{
	my_nBoardChId = board_ch_id;

	// =========================================================
	// アクセスボード特定

	// ボード情報取得
    ACAPBOARDINFO	bi;
	if( ! AcapGetBoardInfo(&bi) ) {
		return FALSE;		// ボード情報 or ライブラリ にアクセス失敗
	}

	// ボード名取得
	char* cBoardName = NULL;
	int ii;
	for( ii=0; ii<bi.nBoardNum; ii++) {
		if( bi.BOARDINDEX[ii].nBoardID == board_id ) {
			cBoardName = bi.BOARDINDEX[ii].pBoardName; 
			break;
		}
	}

	if( ii == bi.nBoardNum ){
		return FALSE;		// 一致ボード無し
	}


	// =========================================================
	// ドライバ アクセス

	// open device
	my_hDev = AcapOpen(cBoardName, board_id, my_nBoardChId );
	if( INVALID_HANDLE_VALUE == my_hDev ) {
		return FALSE;		// デバイスアクセス失敗
	}

	// inifile
	if( ! AcapSelectFile(my_hDev, my_nBoardChId, (char*)ini_filename, 0) ) {
		return FALSE;		// iniファイルNG
	}

	// 設定値読込
	//if( ! AcapGetInfo(my_hDev, my_nBoardChId, ACL_X_SIZE, 0, (int*)&my_nWidth ) ) return FALSE;
	//if( ! AcapGetInfo(my_hDev, my_nBoardChId, ACL_Y_SIZE, 0, (int*)&my_nHeight ) ) return FALSE;
	//if( ! AcapGetInfo(my_hDev, my_nBoardChId, ACL_CAM_BIT, 0, (int*)&my_nPixelNBit ) ) return FALSE;
	if( ! reload_size() ) return FALSE;
    if( my_nPixelNBit < 8 || my_nPixelNBit > 32 ) return FALSE;

	// buffer
	my_nBufNum = nbuff;
	if( ! AcapSetInfo(my_hDev, my_nBoardChId, ACL_MEM_NUM, 0, my_nBufNum ) ) return FALSE;

	//int pixel_nbyte = 0;
	//if(8 == my_nPixelNBit)							pixel_nbyte = sizeof(BYTE);
	//if(8 <  my_nPixelNBit && my_nPixelNBit <= 16 )	pixel_nbyte = sizeof(USHORT);
	//if(16<  my_nPixelNBit)							pixel_nbyte = sizeof(ULONG);
	//
	//my_nBufSize = my_nWidth * my_nHeight * my_nPixelNBit;
	my_pBuff.resize(my_nBufNum);					// 領域確保
	// my_buffはAcapGetBufferAddressのアドレスがDMAオプションで変わるため、StartGrab直前に値を入れることにする

	// acap event
	if( ! AcapSetEvent(my_hDev, my_nBoardChId, ACL_INT_FRAMEEND, TRUE ) ) return FALSE;


	ClearStatCounters();
	return TRUE;
}

//------------------------------------------
// SetInfoでサイズが変わった場合にmy_width等を再設定
//------------------------------------------
BOOL AvalACL2Base::reload_size()
{
	if( ! AcapGetInfo(my_hDev, my_nBoardChId, ACL_X_SIZE, 0, (int*)&my_nWidth ) ) return FALSE;
	if( ! AcapGetInfo(my_hDev, my_nBoardChId, ACL_Y_SIZE, 0, (int*)&my_nHeight ) ) return FALSE;
	if( ! AcapGetInfo(my_hDev, my_nBoardChId, ACL_BOARD_BIT, 0, (int*)&my_nPixelNBit ) ) return FALSE;

	int pixel_nbyte = 0;
	if(8 == my_nPixelNBit)							pixel_nbyte = sizeof(BYTE);
	if(8 <  my_nPixelNBit && my_nPixelNBit <= 16 )	pixel_nbyte = sizeof(USHORT);
	if(16<  my_nPixelNBit)							pixel_nbyte = sizeof(ULONG);
	
	my_nBufSize = my_nWidth * my_nHeight * my_nPixelNBit / 8;
	return TRUE;
}

//------------------------------------------
// 開放
//------------------------------------------
void AvalACL2Base::Dispose()
{
	if( INVALID_HANDLE_VALUE != my_hDev ) {
		AcapClose(my_hDev, my_nBoardChId);
		my_hDev = INVALID_HANDLE_VALUE;
	}
	my_nBoardChId = 0;
	my_pBuff .clear();
	my_nBufNum	= 0;
	my_nBufSize = 0;

	my_nWidth		= 0;
	my_nHeight		= 0;
	my_nPixelNBit	= 0;
	my_bRunning		= false;
}

//------------------------------------------
// 撮像開始
//------------------------------------------
BOOL AvalACL2Base::StartGrab()
{
	if( my_bRunning ) return FALSE;
	my_nLastFrom =  0;

	// DMAバッファアドレス確定
	for(DWORD ii=0; ii<my_nBufNum; ii++ ) {
		if( ! AcapGetBufferAddress(my_hDev, my_nBoardChId, ACL_IMAGE_PTR, ii+1, &my_pBuff[ii] ) ) {
			return FALSE;
		}
	}

	// 撮像開始
	if( ! AcapGrabStart(my_hDev, my_nBoardChId, 0)) {
		return FALSE;
	}

	my_bRunning	 = true;
	return TRUE;
}

//------------------------------------------
// 撮像停止
//------------------------------------------
BOOL AvalACL2Base::StopGrab()
{
	if( ! my_bRunning ) return FALSE;

	// 強制停止
	if( ! AcapGrabAbort(my_hDev, my_nBoardChId) ) {
	//	ACAPERRORINFO aei;
	//	AcapGetLastErrorCode(&aei, FALSE);
 //       return FALSE;
	//}

	//// 停止
 //   if( ! AcapGrabStop(my_hDev, my_nBoardChId) ) {
	//	ACAPERRORINFO aei;
	//	AcapGetLastErrorCode(&aei, FALSE);
        return FALSE;
    }

    my_bRunning = false;
    return TRUE;

}


//------------------------------------------
// 入力フレーム情報の取得
// DWORD& nFrame	フレームNO ※参照なので注意
//------------------------------------------
AvalACL2Base::FrameWaitResult AvalACL2Base::GetReadyFrameCount(DWORD& nFrameCount)
{
	nFrameCount = 0;
	DWORD	fp;

	// fp取得したフレーム数であるが、DMA書き込み中のメモリインデックスと言っても良い
	if( ! AcapGetFrameNo(my_hDev, my_nBoardChId, (int*)&fp, NULL, NULL)) {
		return FrameWait_ACLError;
	}


	AvalACL2Base::FrameWaitResult ret = FrameWait_Success;

	if( fp >= my_nLastFrom + my_nBufNum) {		// my_last_from+my_last_countが読んでいないデータの先頭index
		ret = FrameWait_Overrun;
	}
	nFrameCount = fp - my_nLastFrom ;	
	return ret;
}

//------------------------------------------
// 新規フレーム数の取得 (フレーム入力までブロック)
// DWORD& nFrame	フレームNO ※参照なので注意
// DWORD nTimeout	待機時間 [ms]
//------------------------------------------
AvalACL2Base::FrameWaitResult AvalACL2Base::WaitNFrameReady(DWORD& nFrameCount, DWORD nTimeout)
{
	nFrameCount = 0;
	if( 0 < nTimeout ){
		if( ! AcapWaitEvent(my_hDev, my_nBoardChId, ACL_INT_FRAMEEND,  nTimeout)) {
			ACAPERRORINFO aei;
			AcapGetLastErrorCode(&aei, FALSE);
			if (ACL_ABORT_EVENT_WAIT == aei.dwBoardErrorCode) return FrameWait_Aborted;
			if (ACL_EVENT_TIMEOUT_ERROR ==aei.dwCommonErrorCode)return FrameWait_Timeout;
			if (ACL_WAIT_EVENT_ERROR   ==  aei.dwCommonErrorCode)return FrameWait_Timeout;
			return FrameWait_ACLError;
		}
	}

	return GetReadyFrameCount(nFrameCount); 
}

//------------------------------------------
// ポインタの取得
//		WaitNFrameReadyより多くても少なくても良い
// void**	ptrs			画像ポインタ配列
// DWORD	ptrslen			取得画像数
// DWORD&	nvalid			ptrsの有効数(<= ptrslen)  ※実際に取得した画像数
// DWORD&	from_framenum	今回のフレームNo(複数の最初)
//------------------------------------------
AvalACL2Base::FrameWaitResult AvalACL2Base::GetFramePointers(void** ptrs, DWORD ptrslen, DWORD& nvalid, DWORD& from_framenum)
{
	AvalACL2Base::FrameWaitResult ret = GetReadyFrameCount(nvalid);		// nvalid がセットされて帰ってくる
	if (FrameWait_Overrun == ret) {
		my_nFrameOverrun += nvalid - my_nBufNum + 1;
	}

	// ポインタ更新
	from_framenum	 = my_nLastFrom;
	nvalid			 = __min(nvalid, ptrslen);

	// 画像ポインタセット
	for(DWORD ii=0; ii<nvalid; ii++) {
		ptrs[ii] = my_pBuff[ (my_nLastFrom + ii) % my_nBufNum ];
	}
	my_nLastFrom	+= nvalid;
	return ret;
}

//------------------------------------------
// 読み込み遅れチェック
// ポインタ使用後のオーバーランチェック、
// 直前のGetFramePointersで得た範囲にDMAが食い込んでいないかどうか
//------------------------------------------
BOOL AvalACL2Base::CheckOverrun(int nReadFrameCount)
{
	DWORD	nFrameCount;
	if( FrameWait_Success != GetReadyFrameCount(nFrameCount) ) return FALSE;

	if( nFrameCount > nReadFrameCount + my_nBufNum -1 ) {
		my_nFrameOverrun += nFrameCount + nReadFrameCount - my_nBufNum + 1;
		return FALSE;
	}
	return TRUE;
}

//------------------------------------------
// フレーム待ち(WaitNFrameReady)状態の解除
//------------------------------------------
void AvalACL2Base::AbortFrameWait()
{
    AcapSetEvent(my_hDev, my_nBoardChId, ACL_INT_FRAMEEND | ACL_INT_ABORT, TRUE);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// シリアル
//------------------------------------------
// Byte → (16進数HEX表記)
// char* buff		変換後
// int buff_len		変換後領域サイズ
// const BYTE* data	元
// int nbytes		元サイズ
//------------------------------------------
static bool byte_to_hexstr(char* buff, int buff_len, const BYTE* data, int nbytes)
{
    int idx = 0;
    for(int i=0; i<nbytes; i++)
    {
        const int val[] =
            {
                (data[i] & 0xf0) >> 4,
                data[i] & 0x0f
            };
        const int nval = sizeof(val) / sizeof(val[0]);
        for(int j=0; j<nval; j++)
        {
            if( idx + j >= buff_len ) return false;

            buff[idx + j] = val[j] >= 10
                ? 'A' + val[j] - 10
                : '0' + val[j];
        }
        idx += nval;
    }
    if( idx >= buff_len ) return false;
    buff[idx] = '\0';
    return true;
}
//------------------------------------------
// (16進数HEX表記) → Byte
// char* buff		変換後
// int buff_len		変換後領域サイズ
// const BYTE* data	元
// int nbytes		元サイズ
//------------------------------------------
static bool hexstr_to_byte(BYTE* buff, int buff_len, int& nbytes, const char* hexstr, int strlen)
{
    if( strlen % 2 ) return false;
    nbytes = strlen / 2;
    if( nbytes > buff_len ) return false;

    const int nshift[] = {4 , 0};
    for(int i=0, idx=0; i<strlen; i+=2, idx++)
    {
        buff[idx] = 0;

        for(int j=0; j<2; j++)
        {

            const int dig =
                hexstr[i + j] >= 'a' ? hexstr[i + j] - 'a' + 10 :
                hexstr[i + j] >= 'A' ? hexstr[i + j] - 'A' + 10 :
                hexstr[i + j] - '0';
            buff[idx] += dig << nshift[j];
        }
    }
    return true;
}

//------------------------------------------
// オープン
//------------------------------------------
BOOL AvalACL2Base::OpenSerialPort()
{
    return AcapSerialOpen(my_hDev, my_nBoardChId);
}

//------------------------------------------
// シリアル パラメータ設定
// int baud		ボーレート (9600/19200/38400/.../115200)
//------------------------------------------
BOOL AvalACL2Base::SetSerialParam(int baud)
{
	return AcapSerialSetParameter( my_hDev, my_nBoardChId, baud, 
			/* 以降は、ドライバレベルで固定のため変更不可*/
			8 /*int nDataBit*/,
			0 /*int nParity*/, 
			1 /*int nStopBit*/, 
			0 /*int nFlow*/);
}

//------------------------------------------
// 切断
//------------------------------------------
void AvalACL2Base::CloseSerialPort()
{
    AcapSerialClose(my_hDev, my_nBoardChId);
}

//------------------------------------------
// 送信
// const void* data 送信データ
// int len			送信長 1024以下
//------------------------------------------
BOOL AvalACL2Base::SendToSerialPort(const void* data, int len)
{
    char send_buff[1024]; // dataをascii化した
    if( !byte_to_hexstr(send_buff, sizeof(send_buff), (BYTE*)data, len) )
    {
        return FALSE;
    }
	// FALSE : 16進数(HEX) 表記
    if( !AcapSerialWrite(my_hDev, my_nBoardChId, FALSE, send_buff, NULL, NULL) )
    {
        return FALSE;
    }
    return TRUE;
}

//------------------------------------------
// 受信
// void* buff		受信バッファ
// int buff_len		バッファ長
// int& recv_len	受信長
// DWORD timeout	待ち時間 [ms]	※ 0で待機無し？？
//------------------------------------------
BOOL AvalACL2Base::RecvFromSerialPort(void* buff, int buff_len, int&recv_len, DWORD timeout)
{
    char recv_buff[1024];
    int nrecv = 0;
	// FALSE : 16進数(HEX) 表記
    if( !AcapSerialRead(my_hDev, my_nBoardChId, FALSE, timeout, sizeof(recv_buff), NULL, recv_buff, &nrecv) )
    {
        return FALSE;
    }
    if( !hexstr_to_byte((BYTE*)buff, buff_len, recv_len, recv_buff, nrecv) )
    {
        return FALSE;
    }
    return TRUE;
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// こまごま
//------------------------------------------
// パラメータ設定
// ULONG ulType	設定定数 (ACL_MEM_NUM など)
// int nValue	値
//------------------------------------------
BOOL AvalACL2Base::SetInfo(DWORD ulType, int nValue)
{
	BOOL bRet = AcapSetInfo(my_hDev, my_nBoardChId, ulType, 0, nValue);
	reload_size();
	return bRet;
}

//------------------------------------------
// パラメータ読み込み
// DWORD ulType	設定定数 (ACL_MEM_NUM など)
// int& nValue	値
//------------------------------------------
BOOL AvalACL2Base::GetInfo(DWORD ulType, int& nValue)
{
	return AcapGetInfo(my_hDev, my_nBoardChId, ulType, 0, &nValue);
}

//------------------------------------------
// エラー情報取得
// DWORD* err_board	
// DWORD* err_ext	
//------------------------------------------
DWORD AvalACL2Base::GetLastError(DWORD* err_board, DWORD* err_ext)
{
    ACAPERRORINFO err;
    AcapGetLastErrorCode(&err, FALSE);
    my_errcode_common = err.dwCommonErrorCode;
    my_errcode_board  = err.dwBoardErrorCode;
    my_errcode_ext    = err.dwExtendErrorCode;

    if( NULL != err_board ) *err_board = my_errcode_board;
    if( NULL != err_ext   ) *err_ext   = my_errcode_ext;

    return my_errcode_common;
}
