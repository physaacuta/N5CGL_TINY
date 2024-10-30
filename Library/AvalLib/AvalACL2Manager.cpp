#include "StdAfx.h"
#include <vector>
#include "AvalACL2Manager.h"

#include <AcapLib2.h>
#pragma comment(lib, "AcapLib2.lib")


// テスト画像取得用
//#include "..\ImgLib\CLASS\KizuAviManager.h"									// AVIファイル
#include "..\KizuLib\CLASS\KizuBitmapManager.h"								// BitMapファイル

#include <io.h>
#include <fcntl.h>



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
// CString sMyManagerName 自クラスの名称
// CString sMyGrabberName Grabberクラスの名称
//------------------------------------------
AvalACL2Manager::AvalACL2Manager(CString sMyManagerName, CString sMyGrabberName):
// 名前
GrabberManager(GRAB_TYPE_AVAL, sMyManagerName),

my_emGrabMode(GRAB_MODE_HW),
my_nFrameNum(0),
my_nImgSize(0),
my_nFrameW(0),
my_nFrameH(0),

my_nGrabber(0),
my_pGrabber(NULL),
my_pDummyBuf(NULL),
my_bImgTurn_LR(false),
my_bThreadLoop(false)
{
	my_nChannel = 1;		// グレー画像

	memset(my_hAddInfo, 0x00, sizeof(my_hAddInfo));
	my_evSmpImgTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	my_hHwRun = CreateEvent(NULL, TRUE, FALSE, NULL);		// マニュアル
	my_hInited =  CreateEvent(NULL, FALSE, FALSE, NULL);


	// テスト画像用
	my_nSmpNowNo = 0;
	for(int ii=0; ii<MAX_CAMNUM; ii++) {
		for(int jj=0; jj<MAX_COLOR_CHANNEL; jj++ ) mycls_Aiv[jj][ii] = NULL;
	}
}

//------------------------------------------
// デストラクタ
//------------------------------------------
AvalACL2Manager::~AvalACL2Manager(void)
{
	Dispose();

	SAFE_DELETE_HANDLE(my_evSmpImgTimer);
	SAFE_DELETE_HANDLE(my_hHwRun);
}


//------------------------------------------
// 初期化
// int board_id		ボード番号(0オリジン)
// int board_ch_id	オープンチャンネル (1オリジン)
// const char* ini_filename	設定ファイルフルパス
// int nbuff		バッファ数

// DWORD nFrameW フレームサイズ
// DWORD nFrameH フレームサイズ
// BYTE nDummyVal ダミー画像の色
//------------------------------------------
BOOL AvalACL2Manager::Init(int board_id, int board_ch_id, const char* ini_filename, int nbuff, DWORD nFrameW, DWORD nFrameH, BYTE nDummyVal)
{
	my_nGrabber = 1;
	my_pGrabber = new AvalACL2Base[my_nGrabber];
	my_pGrabber[0].SetThreadName("AvalBase");
	my_pGrabber[0].SetLogMgr(mcls_pLog );

	my_nBatchSize.resize(my_nGrabber);
	my_nMaxBatchSize.resize(my_nGrabber);

	if( ! my_pGrabber[0].Init(board_id, board_ch_id, ini_filename, nbuff) ) {
		return FALSE;
	}

	my_nBaseBufNum = nbuff;
	InitInfo(nFrameW, nFrameH, nDummyVal);
	my_bThreadLoop = true;
	StartLock(THREAD_PRIORITY_TIME_CRITICAL);
	return TRUE;
}

//------------------------------------------
// 初期化 (ボード番号の小さい順に割り当てる)
// int nchannel				接続数
// const char* ini_filename	設定ファイルフルパス
// int nbuff				バッファ数

// DWORD nFrameW フレームサイズ
// DWORD nFrameH フレームサイズ
// BYTE nDummyVal ダミー画像の色
//------------------------------------------
BOOL AvalACL2Manager::Init(int nchannel, const char* ini_filename, int nbuff, DWORD nFrameW, DWORD nFrameH, BYTE nDummyVal)
{
	// ボード情報取得
    ACAPBOARDINFOEX		bi;
    if( ! AcapGetBoardInfoEx(&bi) ) {
        return FALSE;
    }

	// 初期設定
	my_nGrabber			= nchannel;
	my_pGrabber			= new AvalACL2Base[my_nGrabber];
	my_nBatchSize.resize(my_nGrabber);
	my_nMaxBatchSize.resize(my_nGrabber);

	my_nBaseBufNum		= nbuff;

	int	nch = 0;
	for(int ii=0; ii<bi.nBoardNum && nch < nchannel; ii++ ) {
		for(int jj=0; jj<bi.BOARDINDEX[ii].nChannelNum && nch < nchannel; jj++ ) {
	
			const int board_ch_id = jj + 1;
	        
			CString sWk;
			sWk.Format("AvalBase_%d(%d-%d)", nch, bi.BOARDINDEX[ii].nBoardID, board_ch_id); 
			my_pGrabber[nch].SetThreadName(sWk);
			my_pGrabber[nch].SetLogMgr(mcls_pLog );

			if( ! my_pGrabber[nch].Init( bi.BOARDINDEX[ii].nBoardID, board_ch_id, ini_filename, nbuff) ) {
				return FALSE;
			}
			LOG(em_MSG), "[%s] Init(): grabber[%d] --> (%d, %d)", my_sThreadName, nch, bi.BOARDINDEX[ii].nBoardID, board_ch_id);
			nch ++;
		}
	}
	if( nch < nchannel) {
        // channelが足りない
        LOG(em_ERR), "[%s] Init(): nch < nchannel", my_sThreadName);
        return FALSE;
	}

	InitInfo(nFrameW, nFrameH, nDummyVal);
	my_bThreadLoop = true;
	StartLock(THREAD_PRIORITY_TIME_CRITICAL);
	return TRUE;
}

//------------------------------------------
// 初期化	(複数指定チャンネルに割り当て)
// int nchannel				接続数
// int board_id*		ボード番号(0オリジン)
// int board_ch_id*		オープンチャンネル (1オリジン)
// const char* ini_filename	設定ファイルフルパス
// int nbuff		バッファ数

// DWORD nFrameW フレームサイズ
// DWORD nFrameH フレームサイズ
// BYTE nDummyVal ダミー画像の色
//------------------------------------------
BOOL AvalACL2Manager::Init(int nchannel, int const* board_id, int const* board_ch_id, const char* ini_filename, int nbuff, DWORD nFrameW, DWORD nFrameH, BYTE nDummyVal)
{
	// ボード情報取得
    ACAPBOARDINFOEX		bi;
    if( ! AcapGetBoardInfoEx(&bi) ) {
        return FALSE;
    }

	// 初期設定
	my_nGrabber			= nchannel;
	my_pGrabber			= new AvalACL2Base[my_nGrabber];
	my_nBatchSize.resize(my_nGrabber);
	my_nMaxBatchSize.resize(my_nGrabber);

	my_nBaseBufNum		= nbuff;


	for(int ii=0; ii<nchannel; ii++ ) {
		CString sWk;
		sWk.Format("AvalBase_%d(%d-%d)", ii, board_id[ii], board_ch_id[ii]); 
		my_pGrabber[ii].SetThreadName(sWk);
		my_pGrabber[ii].SetLogMgr(mcls_pLog );

		if( ! my_pGrabber[ii].Init( board_id[ii], board_ch_id[ii], ini_filename, nbuff) ) {
			return FALSE;
		}
		LOG(em_MSG), "[%s] Init(): grabber[%d] --> (%d, %d)", my_sThreadName, ii, board_id[ii], board_ch_id[ii]);	
	}
	
	InitInfo(nFrameW, nFrameH, nDummyVal);
	my_bThreadLoop = true;
	StartLock(THREAD_PRIORITY_TIME_CRITICAL);
	return TRUE;
}

//------------------------------------------
// 初期情報セット
//------------------------------------------
void AvalACL2Manager::InitInfo(DWORD nFrameW, DWORD nFrameH, BYTE nDummyVal)
{
	my_nImgSize	  = nFrameW*nFrameH;	// 使う機会が多いから定義しておく
	my_nFrameW	  = nFrameW;
	my_nFrameH	  = nFrameH;

	//// ダミーフレームの生成
	if (NULL != my_pDummyBuf) _aligned_free(my_pDummyBuf);		// 念のため
	DWORD image_size = my_nFrameW * my_nFrameH * my_nChannel;
	my_pDummyBuf = _aligned_malloc(image_size, 32);
	_ASSERT(my_pDummyBuf);
	memset(my_pDummyBuf, nDummyVal, image_size);

	SetEvent(my_hInited);
}

//------------------------------------------
// 開放
//------------------------------------------
void AvalACL2Manager::Dispose()
{
	// スレッドが停止していなかったら停止
	Stop();	// △


	if(NULL != my_pGrabber) {
		for(int ii=0; ii<my_nGrabber; ii++){
			my_pGrabber[ii].Dispose();
		}
		SAFE_DELETE_ARRAY(my_pGrabber);
	}


	my_nGrabber = 0;
	my_nFrameNum = 0;
	my_nBaseBufNum = 0;	
	my_nBatchSize.clear();
	my_nMaxBatchSize.clear();


	//// ダミー画像領域解放
	if (NULL != my_pDummyBuf) {
		_aligned_free(my_pDummyBuf);
		my_pDummyBuf = NULL;
	}
}

//------------------------------------------
// スレッド開始
//------------------------------------------
bool AvalACL2Manager::Start()
{
	return true;
}

//------------------------------------------
// スレッド停止
//------------------------------------------
bool AvalACL2Manager::Stop()
{
	my_bThreadLoop = false;
	StopGrab();

	for(int ii=0; ii<my_nGrabber; ii++){
		my_pGrabber[ii].AbortFrameWait();
	}

	ThreadManager::Stop(5000);
	return true;
}	



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 撮像系

//------------------------------------------
// 撮像開始
//------------------------------------------
BOOL AvalACL2Manager::StartGrab()
{
	LOG(em_MSG), "[%s] StartGrab･･･", my_sThreadName);	
	LOM(em_MSG), "[%s] StartGrab･･･", my_sThreadName);

	//==============================================
	// もろもろ初期化
	my_nFrameNum		= 0;


	//==============================================
	// HW
	if( GRAB_MODE_HW == my_emGrabMode ) {

		for(int ii=0; ii<my_nGrabber; ii++){
			BOOL ret = my_pGrabber[ii].StartGrab();
			if( ! ret ) {
				DWORD cErr, bErr, eErr;
				cErr = my_pGrabber[ii].GetLastError(&bErr, &eErr);
				LOG(em_ERR), "[%s] StartGrab失敗 [%d], Common = %d, Board = %d, Extend = %d", my_sThreadName, ii, cErr, bErr, eErr);
				LOM(em_ERR), "[%s] StartGrab失敗 [%d], Common = %d, Board = %d, Extend = %d", my_sThreadName, ii, cErr, bErr, eErr);


				// StartGrab()したのを止める
				for(int jj=0; jj<ii; jj++)
				{
					my_pGrabber[jj].AbortFrameWait();
					my_pGrabber[jj].StopGrab();
				}

				return cErr;
			}
		}

		//// ここまで来たら StartGrab成功したも同然だから
		SetEvent(my_hHwRun);			// メインスレッドを実行状態
	}

	//==============================================
	// BitMap or AVI
	else if( GRAB_MODE_BMP == my_emGrabMode || 
			 GRAB_MODE_AVI == my_emGrabMode ) {

///		my_nSmpNowNo = my_nSmpIntervalNo[0];
		__int64 ts = my_nSmpCycleTimer * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
		SetWaitableTimer(my_evSmpImgTimer, (LARGE_INTEGER *)&ts, my_nSmpCycleTimer, NULL, NULL, FALSE);

	}


	my_bGrabRun = true;
	LOG(em_MSG), "[%s] StartGrab 完了", my_sThreadName);
	LOM(em_MSG), "[%s] StartGrab 完了", my_sThreadName);	
	return 0;
}

//------------------------------------------
// 撮像停止
//------------------------------------------
BOOL AvalACL2Manager::StopGrab()
{
	LOG(em_MSG), "[%s] StopGrab･･･", my_sThreadName);	
	LOM(em_MSG), "[%s] StopGrab･･･", my_sThreadName);

	BOOL ret = TRUE;

	//==============================================
	// iPort
	if( GRAB_MODE_HW == my_emGrabMode ) {
		ResetEvent(my_hHwRun);			// メインスレッドを実行状態


		for(int ii=0; ii<my_nGrabber; ii++){
			BOOL r = my_pGrabber[ii].StopGrab();
			if( ! r ) {
				DWORD cErr, bErr, eErr;
				cErr = my_pGrabber[ii].GetLastError(&bErr, &eErr);
				LOG(em_ERR), "[%s] StopGrab失敗 [%d], Common = %d, Board = %d, Extend = %d", my_sThreadName, ii, cErr, bErr, eErr);
				LOM(em_ERR), "[%s] StopGrab失敗 [%d], Common = %d, Board = %d, Extend = %d", my_sThreadName, ii, cErr, bErr, eErr);
			}
			ret &= r;
		}
	}

	//==============================================
	// BitMap or AVI
	else if( GRAB_MODE_BMP == my_emGrabMode || 
			 GRAB_MODE_AVI == my_emGrabMode ) {
		CancelWaitableTimer(my_evSmpImgTimer);
	}

	my_bGrabRun = false;
	LOG(em_MSG), "[%s] StopGrab 完了", my_sThreadName);
	LOM(em_MSG), "[%s] StopGrab 完了", my_sThreadName);	
	return ret ? 0 : -1;
}

//------------------------------------------
// リセット
//------------------------------------------
bool AvalACL2Manager::Reset()
{
	my_nFrameNum = 0;
	return true;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// スレッド操作

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int AvalACL2Manager::ThreadFirst()
{
	// イベント設定
	HANDLE hArray[] = { my_evSmpImgTimer,		// ダミーフレーム時 (ビットマップやAVI時)
						my_hHwRun				// ボード接続時のみ
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}


//------------------------------------------
// スレッドメインループ
//------------------------------------------
int AvalACL2Manager::ThreadMainLoop()
{
	DWORD	ret;

	////// シグナルの条件
	enum {	EV_STOP = WAIT_OBJECT_0,			// 停止シグナル
			EV_SMP_IMG_TIMER,					// テスト画像取得時 (ビットマップやAVI時)
			EV_RUN								// 画像取込 (マニュアルのシグナル)
	};


	////////////////////////////////////
	// 領域確保

	WaitForSingleObject(my_hInited, INFINITE);

	AvalACL2Base::FrameWaitResult*	emWaitResult = new AvalACL2Base::FrameWaitResult[my_nGrabber];
	void**	p = new void* [my_nGrabber];

	std::vector< void *> p_batch [MAX_CAMNUM];
	for(int ii=0; ii<my_nGrabber; ii++) {
		p_batch[ii].resize( my_nBaseBufNum );
	}
	DWORD nSeqNum[64] = {};
//	memset(nSeqNum, 0, sizeof(DWORD) * my_nGrabber);
	m_nSeqNum = nSeqNum;

	


	////////////////////////////////////
	// 前準備
	bool bWaitAbort = false;
	int nRetc = ThreadFirst();						// スレッド開始をコールバック
	if (INVALID_HANDLE_VALUE != my_hThreadReady) { SetEvent(my_hThreadReady); }		// 実行待機版

	LOG(em_MSG), "[%s] スレッド開始=%d [0x%X]", my_sThreadName, nRetc, GetTid());


	////////////////////////////////////
	// メインループ
	while( my_bThreadLoop ) {

		ret = WaitForMultipleObjects((my_nWaitNum+1), my_pEvWait, FALSE, INFINITE);

//-----------------------------------------------------------------------------------------------
		// スレッド終了
		if( ret == EV_STOP || ! my_bThreadLoop) {
			LOG(em_MSG), "[%s] スレッド終了=%d [0x%X]", my_sThreadName, ret, GetTid());
			break;
		}


//-----------------------------------------------------------------------------------------------
		// 画像取込
		if( ret == EV_RUN ) {

			//------------------------------
			// フレーム生成待ち
			DWORD	nframe = 0;				// 取得可能なフレーム数
			__int64	nFastGetTime;			// 最初の情報を取得した時間

			for(int ii=0; ii<my_nGrabber; ii++) {

				if( 0 ) {
					LOM(em_MSG), "lock (set=%d)", ii);
					emWaitResult[ii]	= my_pGrabber[ii].WaitNFrameReady( my_nBatchSize[ii], INFINITE);
					LOM(em_MSG), "(%d)->ret=%d, Fno=%d, batch=%d", ii, emWaitResult[ii], my_nFrameNum, my_nBatchSize[ii]);
				}
				else{
					emWaitResult[ii]	= my_pGrabber[ii].WaitNFrameReady( my_nBatchSize[ii], 0);
					if(0 == my_nBatchSize[ii]){
						//待つ必要がある
						//// フレーム画像取得。※ブロック。
						emWaitResult[ii]	= my_pGrabber[ii].WaitNFrameReady( my_nBatchSize[ii], 300 );
						if( my_nBatchSize[ii] > 0 )
						LOM(em_MSG), "(%d)->ret=%d, Fno=%d, batch=%d", ii, emWaitResult[ii], my_nFrameNum, my_nBatchSize[ii]);
					}
					else{
					//待たなくてもいい
						LOM(em_MSG), "get  (set=%d) ->ret=%d, Fno=%d, batch=%d", ii, emWaitResult[ii], my_nFrameNum, my_nBatchSize[ii]);
					}
				}
				my_nMaxBatchSize[ii ]= __max( my_nMaxBatchSize[ii], my_nBatchSize[ii]);			
				if( 0 == ii) {
					nframe		= my_nBatchSize[ii] ;
					QueryPerformanceCounter((LARGE_INTEGER *)&nFastGetTime);
				}
				else{
					nframe =  __min(my_nBatchSize[ii], nframe) ;
				}

				// スレッドをとめる時だけABORTする
				if( AvalACL2Base::FrameWait_Aborted == emWaitResult[ii] ) {
					bWaitAbort = true;
					break;
				}

				// 正常以外
				if( AvalACL2Base::FrameWait_Success != emWaitResult[ii] &&
					AvalACL2Base::FrameWait_Timeout != emWaitResult[ii]) {
					OnGrabError( my_nFrameNum, ii, emWaitResult[ii] );	
				}
			}
			if(bWaitAbort) break;

			if( nframe == 0 ){
//				LOM(em_MSG), "!? nframe = 0, bo=%d, my=%d",  my_nBatchSize[0], my_nFrameNum);
				continue;
			}


			//------------------------------
			// フレーム取り出し
			nframe = __min( nframe, my_nBaseBufNum );
			for(int ii=0; ii<my_nGrabber; ii++) {
				DWORD nvalid;
				emWaitResult[ii]	= my_pGrabber[ii].GetFramePointers( &p_batch[ii][0], nframe, nvalid, nSeqNum[ii] );
				_ASSERT(nframe == nvalid);

				if( AvalACL2Base::FrameWait_Success != emWaitResult[ii] ) {
					OnGrabError( my_nFrameNum, ii, emWaitResult[ii] );
				}
			}

			//------------------------------
			// 下流へ
			for(DWORD ii=0; ii<nframe; ii++ ) {
				for(int jj=0; jj<my_nGrabber; jj++) {
					p[jj] = p_batch[jj][ii];
				}
				// 1カメラセット分の画像を取得して、スレッドループキューにキューイング 
				OnFrameGrabbed(p, my_nFrameNum++, (int*)nSeqNum, nFastGetTime );
				// シーケンスNo加算
				for(int jj=0; jj<my_nGrabber; jj++) {
					nSeqNum[jj] ++;
				}
			}
			for(int ii=0; ii<my_nGrabber; ii++) {
				if( ! my_pGrabber[ii].CheckOverrun(nframe) ) {
					OnGrabError(my_nFrameNum, ii, AvalACL2Base::FrameWaitResult::FrameWait_Overrun);
				}
			}
		}

//-----------------------------------------------------------------------------------------------
		//// テスト画像用
		else if( ret == EV_SMP_IMG_TIMER) {
			if (GRAB_MODE_BMP == my_emGrabMode) {
				OnBmpFrameGrabbed();
			}
			for (int jj = 0; jj<my_nGrabber; jj++) {
				nSeqNum[jj] ++;
			}
		//	} else if(IportManager::GRAB_MODE_AVI == my_emGrabMode ) {
		//		OnAviFrameGrabbed();
		//	}
		}

	}


	////////////////////////////////////
	// 後始末
	delete [] emWaitResult;
//	delete [] nSeqNum;
	delete []p;
	
	m_nSeqNum = NULL;

	//nRetc = ThreadLast();				// スレッド終了をコールバック
	LOG(em_MSG), "[%s] スレッド終了=%d [0x%X]", my_sThreadName, nRetc, GetTid());
	return nRetc;
}


//------------------------------------------
// 1カメラセット分の画像を取得して、スレッドループキューにキューイング
// void** pImgs 画像(カメラ角度分)
// DWORD nFrameNum フレームNo (1コだけ)
// int* nSeqNum iPortシーケンス番号
// __int64 nFastTime 最初のカメラ角度を取得した時間
//------------------------------------------
void AvalACL2Manager::OnFrameGrabbed(void** pImgs, DWORD nFrameNum, int* nSeqNum, __int64 nFastTime)
{
	//// 空き器を取得
	FRAME_DATA* p = myque_pFrame->GetFree(0);				// セマフォー待ちバージョン(すぐに結果を返したい)
	if( NULL == p ) {
		// 器空きなし (回復可能なエラー)
		LOG(em_ERR), "[%s] GRABバッファオーバラン(SNo=%d, FNo=%d):スレッドループキュー空き無し!", my_sThreadName, nSeqNum[0], nFrameNum);
		OnAddInfo(IportLib::DIV_ID_ERROR_NO_IMG_QUEFULL, nFrameNum, nSeqNum[0], 0, 0 );
		return;
	}


	//// 画像を格納
	p->cls_Time.PutTimestamp(0, nFastTime);			// ＃０：最初のカメラ角度を取得した時間


	// データコピー
	const long nImageSize = my_pGrabber[0].GetWidth() * my_pGrabber[0].GetHeight();
	p->nCamset	= 0;
	p->nLen		= nImageSize;
	p->nNo		= nFrameNum;


	for(int ii=0; ii<my_nGrabber; ii++) {
		p->nSeqNum[ii] = (WORD)nSeqNum[ii];		//iportSQNOとの互換性をとるため、16bitにする
		p->hInfo[ii]   = my_hAddInfo[ii];

		if( ! my_bImgTurn_LR ) {
			KizuMMX::mem_copy_64xmm(&p->pImgs[ii*nImageSize], pImgs[ii], nImageSize);
		} else {
			_ASSERT( false );	// 未対応
		}
	}

	p->cls_Time.PutTimestamp(1);					// ＃１：全部のカメラ角度が揃った時間 (=今)


	// 登録
	myque_pFrame->AddPool(p); 
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// その他小物

//------------------------------------------
// エラー定義
// unsigned long frame_num				フレームNo
// int grabber_index					Ch
// AvalACL2Base::FrameWaitResult err	エラーNo
//------------------------------------------
void AvalACL2Manager::OnGrabError(unsigned long frame_num, int grabber_index, AvalACL2Base::FrameWaitResult err)
{
	LOG(em_WAR), "[%s] Grab Error : frame_num=%d, grabber_index=%d, err=%d", my_sThreadName, frame_num, grabber_index, err);
	LOM(em_WAR), "[%s] Grab Error : frame_num=%d, grabber_index=%d, err=%d", my_sThreadName, frame_num, grabber_index, err);


	if (err == AvalACL2Base::FrameWait_ACLError) {
		DWORD cErr, bErr, eErr;
		cErr = my_pGrabber[grabber_index].GetLastError(&bErr, &eErr);

		LOG(em_ERR), "[%s] -> ACLError : Common = %d, Board = %d, Extend = %d", my_sThreadName, cErr, bErr, eErr);
		LOM(em_ERR), "[%s] -> ACLError : Common = %d, Board = %d, Extend = %d", my_sThreadName, cErr, bErr, eErr);
	}

	// 外へ
	OnAddInfo(DIV_ID_ERROR_GRABBER_FRAME_LOCK_FAILED, frame_num, grabber_index, 0, err);
}


//------------------------------------------
// 付帯情報キューイング
// EM_DELIVERY_ID em 区分
// DWORD nFrameNum フレームNo
// DWORD set カメラセット (0オリジン)
// DWORD ang カメラ角度 (0オリジン)
// DWORD st ステータス情報
//------------------------------------------
void AvalACL2Manager::OnAddInfo(EM_DELIVERY_ID em, DWORD nFrameNum, DWORD set, DWORD ang, DWORD st)
{
	//// データ生成
	DELIVERY_INFO* pDeli = new DELIVERY_INFO;
	pDeli->em		 = em;
	pDeli->nFrameNum = nFrameNum;
	pDeli->nCamset	 = set;
	pDeli->nAngle	 = ang;
	pDeli->status	 = st;

	//// キューイング
	if( ! myque_Info.AddToTailFreeCheck(pDeli) ) {
		LOG(em_ERR), "[%s] スレッドキュー空き無し!", my_sThreadName);
		delete pDeli;
	}
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// サンプル画像操作系

//------------------------------------------
// 連続ビットマップ定義
// char const* cPath ビットマップファイルのフォーマット(形式は、カメラ角度(0オリジン),連番) (例:test_Ang0_No0.bmp)
// int nNoFrom 連番の開始位置
// int nNoTo 連番の終了位置
// int nCycleTimer 画像取込周期 [ms]
//------------------------------------------
bool AvalACL2Manager::OpenBmp(char const* cPathFormat, int nNoFrom, int nNoTo, int nCycleTimer)
{
	//// データセット
	my_sSmpPathFormat	 = cPathFormat;
	my_nSmpIntervalNo[0] = nNoFrom;
	my_nSmpIntervalNo[1] = nNoTo;
	my_nSmpCycleTimer	 = nCycleTimer;


	//// オープンチェック
	// ビットマップの場合は、実際のオープンは毎回画像取込時に行う。
	// ここでは、ファイルがあるか程度のチェックを行う
	CString				sPath;

	for(int jj=0; jj<my_nGrabber; jj++) {
		for(int kk=my_nSmpIntervalNo[0]; kk<=my_nSmpIntervalNo[1]; kk++) {

			if( SMPPATHFROMAT_NOM == my_nSmpPathFormatMode ) {

				// ファイルパスの生成
				sPath.Format(my_sSmpPathFormat, jj, kk);
				int fh = _open( sPath, _O_RDONLY ); 
				if(-1 == fh ) {
					LOG(em_ERR), "[%s] BitMapファイルがありません。(%s)", my_sThreadName, sPath);
					return false;
				}
				_close(fh);

			} else if( SMPPATHFROMAT_CHN == my_nSmpPathFormatMode ) {

				for(int ll=0; ll<my_nChannel; ll++) {

					// ファイルパスの生成
					sPath.Format(my_sSmpPathFormat, jj, ll, kk);
					int fh = _open( sPath, _O_RDONLY ); 
					if(-1 == fh ) {
						LOG(em_ERR), "[%s] BitMapファイルがありません。(%s)", my_sThreadName, sPath);
						return false;
					}
					_close(fh);
				}
			}
		}
	}

	//// その他必要なデータ
	my_nSmpNowNo		 = nNoFrom;
	my_emGrabMode		 = GrabberManager::GRAB_MODE_BMP;
	my_nBaseBufNum = 1;
//	InitInfo(3968, 4096, 1);
	//InitInfo(4096, 4096, 1);
	//my_bThreadLoop = true;
	//StartLock(THREAD_PRIORITY_TIME_CRITICAL);
	return TRUE;

	return true;

}

//------------------------------------------
// 連続ビットマップ解放
//------------------------------------------
void AvalACL2Manager::CloseBmp()
{
	//// 準備はしたけど、今のところ何もしないかな
}

//------------------------------------------
// AVI定義
// char const* cPath AVIファイルのフォーマット(形式は、カメラ角度(0オリジン)) (例:test_Set0_Ang0.avi)
// int nCycleTimer 画像取込周期 [ms]
//------------------------------------------
bool AvalACL2Manager::OpenAvi(char const* cPathFormat, int nCycleTimer)
{
	//// データセット
	my_sSmpPathFormat	 = cPathFormat;
	my_nSmpCycleTimer	 = nCycleTimer;

	//// AVIファイルの準備
	CString				sPath;
	bool				ret;

	for(int jj=0; jj<my_nGrabber; jj++) {

		if( SMPPATHFROMAT_NOM == my_nSmpPathFormatMode ) {
			// 生成		
			mycls_Aiv[0][jj] = new KizuAviManager();
			// ファイルパスの生成
			sPath.Format(my_sSmpPathFormat, jj);
			// AVIファイルオープン
			ret = mycls_Aiv[0][jj]->OpenAVIFile(sPath); 
			if( ! ret) {
				LOG(em_ERR), "[%s] AVIファイルがありません。(%s)", my_sThreadName, sPath);
				return false;
			}

		} else if( SMPPATHFROMAT_CHN == my_nSmpPathFormatMode ) {
			for(int ll=0; ll<my_nChannel; ll++) {
				// 生成		
				mycls_Aiv[ll][jj] = new KizuAviManager();
				// ファイルパスの生成
				sPath.Format(my_sSmpPathFormat, jj, ll);
				// AVIファイルオープン
				ret = mycls_Aiv[ll][jj]->OpenAVIFile(sPath); 
				if( ! ret) {
					LOG(em_ERR), "[%s] AVIファイルがありません。(%s)", my_sThreadName, sPath);
					return false;
				}
			}
		}

		// 読み込み
		my_nSmpIntervalNo[0] = 0;
		my_nSmpIntervalNo[1] = mycls_Aiv[0][jj]->GetFrameCnt();
	}


	//// その他必要なデータ
	my_nSmpNowNo		 = 0;
	my_emGrabMode		 = GrabberManager::GRAB_MODE_AVI;
	return true;
}

//------------------------------------------
// AVI解放
//------------------------------------------
void AvalACL2Manager::CloseAvi()
{
	for(int ii=0; ii<MAX_CAMNUM; ii++) {
		for(int jj=0; jj<MAX_COLOR_CHANNEL; jj++) {
			if(NULL == mycls_Aiv[jj][ii]) continue;

			mycls_Aiv[jj][ii]->CloseAVIFile();
			SAFE_DELETE( mycls_Aiv[jj][ii] );
		}
	}
}

//------------------------------------------
// 画像データを手動で追加
//------------------------------------------
bool AvalACL2Manager::OpenManual()
{
	my_nSmpNowNo		 = 0;
	my_emGrabMode		 = GrabberManager::GRAB_MODE_MAN;
	return true;
}
//------------------------------------------
// 画像データを手動で追加
//------------------------------------------
void AvalACL2Manager::CloseManual()
{
	//// 準備はしたけど、今のところ何もしないかな
}

//------------------------------------------
// 全カメラセット、カメラ角度に対してビットマップ画像をセット
//------------------------------------------
void AvalACL2Manager::OnBmpFrameGrabbed()
{
	KizuBitmapManager	cls_bmp;
	CString				sPath;
	int					nW;
	int					nH;


	//// 空き器を取得
	FRAME_DATA* p = myque_pFrame->GetFree(0);				// セマフォー待ちバージョン(すぐに結果を返したい)
	if( NULL == p ) {
		// 器空きなし (回復可能なエラー)
		LOG(em_WAR), "[%s] GRABバッファオーバラン(No=%d):スレッドループキュー空き無し!", my_sThreadName, my_nFrameNum);
		OnAddInfo(IportLib::DIV_ID_ERROR_NO_IMG_QUEFULL, my_nFrameNum, 0, 0, 0 );
// △まだ		for(DWORD jj=0; jj<my_nGrabber; jj++) my_nSeqNum[jj] ++;
		my_nFrameNum ++;
		return;
	}

//memset(p->pImgs,0xff,128);//◆

	p->cls_Time.PutTimestamp(0);						// ＃０：最初のカメラ角度を取得した時間 (=今)


	//// Bitmapファイルを取得
	LOM(em_MSG), "lock");
	for(int jj=0; jj<my_nGrabber; jj++) {
// △まだ		p->nSeqNum[jj] = my_nSeqNum[jj];
		p->hInfo[jj]   = my_hAddInfo[jj];


		if( SMPPATHFROMAT_NOM == my_nSmpPathFormatMode ) {
			sPath.Format( my_sSmpPathFormat, jj, my_nSmpNowNo );
			cls_bmp.BMPfileToData_Gray( sPath, &nW, &nH, &p->pImgs[ my_nImgSize * (jj*my_nChannel) ] );
			
			// 画像上下反転
			KizuAviManager::HantenTB(nW, nH, &p->pImgs[ my_nImgSize * (jj*my_nChannel) ]);
			_ASSERT( nW == my_nFrameW && nH == my_nFrameH );

			// チャンネル数分ループ。色差対応
			for(int kk=1; kk<my_nChannel; kk++) {
				memcpy( &p->pImgs[ (jj*my_nChannel+kk) * my_nImgSize], &p->pImgs[ my_nImgSize * (jj*my_nChannel) ], my_nImgSize );	
			}

		} else if( SMPPATHFROMAT_CHN == my_nSmpPathFormatMode ) {
			for(int kk=0; kk<my_nChannel; kk++) {

				sPath.Format( my_sSmpPathFormat, jj, kk, my_nSmpNowNo );
				cls_bmp.BMPfileToData_Gray( sPath, &nW, &nH, &p->pImgs[ my_nImgSize * (jj*my_nChannel+kk) ] );
				
				// 画像上下反転
				KizuAviManager::HantenTB(nW, nH, &p->pImgs[ my_nImgSize * (jj*my_nChannel+kk)  ]);
				_ASSERT( nW == my_nFrameW && nH == my_nFrameH );
			}
		}	
	}

	p->cls_Time.PutTimestamp();							// ＃１：全部のカメラ角度が揃った時間 (=今)

	LOM(em_MSG), "->Fno=%d, path=%s", my_nFrameNum, sPath);


	//// データコピー
	p->nCamset	= 0;
	p->nLen		= nW*nH;
	p->nNo		= my_nFrameNum;


	for (int ii = 0; ii < my_nGrabber; ii++) {
		p->nSeqNum[ii] = (WORD)m_nSeqNum[ii];		//iportSQNOとの互換性をとるため、16bitにする
	}
	//// 登録
	myque_pFrame->AddPool(p); 
	//myque_pFrame->AddFree(p);  // テスト



	//// 次のため
// △まだ	for(DWORD jj=0; jj<my_nGrabber; jj++) my_nSeqNum[jj] ++;
	my_nFrameNum ++;
	my_nSmpNowNo ++;
	if( my_nSmpNowNo > my_nSmpIntervalNo[1] ) my_nSmpNowNo = my_nSmpIntervalNo[0];		// 一周まわったから最初の画像を取得
}

//------------------------------------------
// 全カメラセット、カメラ角度に対してAVI画像をセット
//------------------------------------------
void AvalACL2Manager::OnAviFrameGrabbed()
{
	bool ret;

	//// 空き器を取得
	FRAME_DATA* p = myque_pFrame->GetFree(0);				// セマフォー待ちバージョン(すぐに結果を返したい)
	if( NULL == p ) {
		// 器空きなし (回復可能なエラー)
		LOG(em_WAR), "[%s] GRABバッファオーバラン(No=%d):スレッドループキュー空き無し!", my_sThreadName, my_nFrameNum);
		OnAddInfo(IportLib::DIV_ID_ERROR_NO_IMG_QUEFULL, my_nFrameNum, 0, 0, 0 );
// △まだ		for(DWORD jj=0; jj<my_nAngleNum; jj++) my_nSeqNum[jj] ++;
		my_nFrameNum ++;
		return;
	}


	p->cls_Time.PutTimestamp(0);						// ＃０：最初のカメラ角度を取得した時間 (=今)

	
	//// AVI画像読み込み
	LOM(em_MSG), "lock");
	for(int jj=0; jj<my_nGrabber; jj++) {
// △まだ		p->nSeqNum[jj] = my_nSeqNum[jj];
		p->hInfo[jj]   = my_hAddInfo[jj];

		if( SMPPATHFROMAT_NOM == my_nSmpPathFormatMode ) {
			ret = mycls_Aiv[0][0]->LoadFromAVI( my_nSmpNowNo, &p->pImgs[ my_nImgSize * (jj*my_nChannel) ], true );
			if( ! ret ) {
				LOG(em_ERR), "[%s] AVIファイル読み込み失敗 (ang=%d, no=%d)", my_nFrameNum, jj, my_nSmpNowNo);
			}

			// チャンネル数分ループ。色差対応
			for(int kk=1; kk<my_nChannel; kk++) {
				memcpy( &p->pImgs[ (jj*my_nChannel+kk) * my_nImgSize], &p->pImgs[ my_nImgSize * (jj*my_nChannel) ], my_nImgSize );	
			}
		} else if( SMPPATHFROMAT_CHN == my_nSmpPathFormatMode ) {

			for(int kk=0; kk<my_nChannel; kk++) {
				ret = mycls_Aiv[kk][0]->LoadFromAVI( my_nSmpNowNo, &p->pImgs[ my_nImgSize * (jj*my_nChannel+kk) ], true );
				if( ! ret ) {
					LOG(em_ERR), "[%s] AVIファイル読み込み失敗 (ang=%d, ch=%d, no=%d)", my_nFrameNum, jj, kk, my_nSmpNowNo);
				}
			}
		}

/*//// ----->>>>
		p->pImgs[ 10 +my_nFrameW*0] = 0x0f;
		p->pImgs[ 10 +my_nFrameW*1] = 0x0f;
		p->pImgs[ 10 +my_nFrameW*2] = 0x0f;
		p->pImgs[ 10 +my_nFrameW*3] = 0x0f;
		p->pImgs[ 10 +my_nFrameW*4] = 0x0f;

			CString str;
			str.Format("C:\\test\\a%d_%d_%d.bmp",ii,jj,my_nFrameNum) ;
			KizuBitmapManager k;
			k.DataToBMPfile_Gray(my_nFrameW, my_nFrameH, &p->pImgs[ my_nImgSize * jj ], str); 

		//	std::ofstream   os;
		//	os.open(str, std::ios::binary);
		//	os.write((const char*)&p->pImgs[ my_nImgSize * jj ], my_nImgSize);
		//	os.close();
							
//*/// <<<<-------	
		
	}
	p->cls_Time.PutTimestamp();							// ＃１：全部のカメラ角度が揃った時間 (=今)

	LOM(em_MSG), "->Fno=%d", my_nFrameNum);


	//// データコピー
	p->nCamset	= 0;
	p->nLen		= my_nImgSize;
	p->nNo		= my_nFrameNum;


	//// 登録
	myque_pFrame->AddPool(p); 
	//myque_pFrame->AddFree(p);  // テスト


	//// 次のため
// △まだ	for(DWORD jj=0; jj<my_nGrabber; jj++) my_nSeqNum[jj] ++;
	my_nFrameNum ++;
	my_nSmpNowNo ++;
	if( my_nSmpNowNo >= my_nSmpIntervalNo[1] ) my_nSmpNowNo = my_nSmpIntervalNo[0];		// 一周まわったから最初の画像を取得
}

//------------------------------------------
// 1カメラセット分の画像を取得して、スレッドループキューにキューイング
// void** pImgs		画像(カメラ角度分)
//					my_nSmpPathFormatMode=1 : [カメラ角度][カメラCH]の順の画像
// DWORD nBytes 画像サイズ (1コだけ)
// DWORD nFrameNum フレームNo (1コだけ)
// DWORD nCamset カメラセット(0オリジン)
// __int64 nFastTime 最初のカメラ角度を取得した時間
//------------------------------------------
bool AvalACL2Manager::OnManualFrameGrabbed(BYTE* pImgs, DWORD nBytes, DWORD nCamset)
{
	//// 空き器を取得
	FRAME_DATA* p = myque_pFrame->GetFree(0);				// セマフォー待ちバージョン(すぐに結果を返したい)
	if( NULL == p ) {
		// 器空きなし (回復可能なエラー)
		LOG(em_ERR), "[%s] GRABバッファオーバラン(set=%d, No=%d):スレッドループキュー空き無し!", my_sThreadName, nCamset, my_nFrameNum);
		OnAddInfo(IportLib::DIV_ID_ERROR_NO_IMG_QUEFULL, my_nFrameNum, 0, 0, 0 );
// △まだ		for(DWORD jj=0; jj<my_nGrabber; jj++) my_nSeqNum[jj] ++;
		my_nFrameNum ++;
		return false;
	}
	p->cls_Time.PutTimestamp(0);						// ＃０：最初のカメラ角度を取得した時間 (=今)

	// データコピー
	p->nCamset	= 0;
	p->nLen		= nBytes;
	p->nNo		= my_nFrameNum;
	for(int ii=0; ii<my_nGrabber; ii++) {
// △まだ		p->nSeqNum[ii] = my_nSeqNum[ii];
		p->hInfo[ii]   = my_hAddInfo[ii];

		if( SMPPATHFROMAT_NOM == my_nSmpPathFormatMode ) {
			KizuMMX::mem_copy_64xmm(&p->pImgs[(ii*my_nChannel)*nBytes], &pImgs[ii*nBytes], nBytes);
			for(int kk=1; kk<my_nChannel; kk++) {			// チャンネル数分ループ。色差対応
				memcpy( &p->pImgs[(ii*my_nChannel+kk) * nBytes], &p->pImgs[ii*nBytes], nBytes );	
			}
		} else if( SMPPATHFROMAT_CHN == my_nSmpPathFormatMode ) {
			KizuMMX::mem_copy_64xmm(&p->pImgs[(ii*my_nChannel)*nBytes], &pImgs[my_nChannel*ii*nBytes], nBytes*my_nChannel);
		}
	}
	p->cls_Time.PutTimestamp(1);					// ＃１：全部のカメラ角度が揃った時間 (=今)

	//// 登録
	myque_pFrame->AddPool(p); 

	//// 次のため
// △まだ	for(DWORD jj=0; jj<my_nGrabber; jj++) my_nSeqNum[jj] ++;
	my_nFrameNum ++;
	my_nSmpNowNo ++;
	return true;
}