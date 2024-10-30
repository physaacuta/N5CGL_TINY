#include "StdAfx.h"
#include "IportManager1p.h"

#include <io.h>
#include <fcntl.h>

// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

using namespace IportLib;


////===================================================================
//// マクロ
////===================================================================
//#define LOM(t) mcls_LogMe.Write(KizuLib::LogFileManager::t			// ローカル専用ログ



//------------------------------------------
// コンストラクタ
// CString sMyManagerName 自クラスの名称
// CString sMyIport iPortクラスの名称
//------------------------------------------
IportManager1p::IportManager1p(CString sMyManagerName, CString sMyIport) :
// 1.名前
IportManager(sMyManagerName),

my_nChDiffMax(0)
{
	for(int ii=0; ii<MAX_IPORT_CAMNUM; ii++) {
		mycls_pIport[ii]	= NULL;
		my_nChDiff[ii]		= 0;
	}
}

//------------------------------------------
// デストラクタ
//------------------------------------------
IportManager1p::~IportManager1p(void)
{
	// 念のために解放
	Dispose();
}

//------------------------------------------
// 初期化 ※ 自分で画像をキューイングしたい場合に使うかな
// DWORD nFrameW フレームサイズ
// DWORD nFrameH フレームサイズ
// DWORD camset 幅方向数
// DWORD angle 角度数
// BYTE nDummyVal ダミー画像の色
//------------------------------------------
bool IportManager1p::Init(DWORD nFrameW, DWORD nFrameH, DWORD camset, DWORD angle, BYTE nDummyVal)
{
	//// メンバー変数に登録
	my_nImgSize	  = nFrameW*nFrameH;	// 使う機会が多いから定義しておく
	my_nFrameW	  = nFrameW;
	my_nFrameH	  = nFrameH;
	my_nCamsetNum = camset;
	my_nAngleNum  = angle;


	//// いろいろチェック	
	int camnum			= camset * angle;
	int nMaxQueSize		= myque_pFrame->GetSemCount();

	if(1 > my_nImgSize) {
		LOG(em_ERR), "[%s] フレームサイズ異常 (w=%d, h=%d)", my_sThreadName, nFrameW, nFrameH);
		_ASSERT(false);
		return false;
	}
	if(1 > nMaxQueSize) {
		LOG(em_ERR), "[%s] キューサイズ異常=%d", my_sThreadName, nMaxQueSize);
		_ASSERT(false);
		return false;
	}
	_ASSERT(camset		   < MAX_IPORT_CAMSET );
	_ASSERT(angle		   < MAX_IPORT_CAMANGLE );
	_ASSERT(camset * angle < MAX_IPORT_CAMNUM );

/*  タスクの方で生成するように変更
	//// 回転寿司形式のキューを確保
	int  nAryWk[5];
	nAryWk[0] = my_nImgSize;
	nAryWk[1] = my_nFrameW;
	nAryWk[2] = my_nFrameH;
	nAryWk[3] = my_nCamsetNum;
	nAryWk[4] = my_nAngleNum;	//camnum;
	myque_pFrame->QueueAlloc(nMaxQueSize, nAryWk); 
*/

	//// ダミーフレームの生成
	if (NULL != my_pDummyBuf) _aligned_free(my_pDummyBuf);		// 念のため
	DWORD image_size = my_nFrameW * my_nFrameH * my_nChannel;
	my_pDummyBuf = _aligned_malloc(image_size, 32);
	_ASSERT(my_pDummyBuf);
	memset(my_pDummyBuf, nDummyVal, image_size);
	
	my_bInit = true;
	return true;
}

//------------------------------------------
// 解放
//------------------------------------------
void IportManager1p::Dispose()
{
	//// iPortと切断 (念のため)
	DisConnect();
	CloseBmp();
	CloseAvi();

/* タスクの方で解放するように変更
	//// 回転寿司形式のキューを解放
	myque_pFrame->QueueFree();
*/

	//// ダミー画像領域解放
	if (NULL != my_pDummyBuf) {
		_aligned_free(my_pDummyBuf);
		my_pDummyBuf = NULL;
	}
	
	my_bInit = false;
}

//------------------------------------------
// スレッド開始 ※ このスレッドは、ホントにスレッドが実行されるまでロックされる
//------------------------------------------
bool IportManager1p::Start()
{
	// 前準備
	my_bThreadLoop	= true;
	my_hThreadReady	= CreateEvent(NULL, FALSE, FALSE, NULL);
	ResetEvent(my_hThreadGoStop);				// 停止状態にしておく

	// 基本処理 クリティカルで実行
	ThreadManager::Start(THREAD_PRIORITY_TIME_CRITICAL);
	
	// このスレッドはちゃんとスレッドが起動するまで待機
    const unsigned long timeout = 1000;
    DWORD wait = WaitForSingleObject(my_hThreadReady, timeout);
	bool ret = WAIT_OBJECT_0 == wait ? true : false;

	// 後始末
	SAFE_DELETE_HANDLE(my_hThreadReady);
	return ret;
}

//------------------------------------------
// スレッド停止
//------------------------------------------
bool IportManager1p::Stop()
{
	// 停止フラグセット
	my_bThreadLoop = false;						// 処理中だった場合、途中中断用
	WorkThread::StopSignal();					// 通常の停止シグナルを発行
	SetEvent(my_hThreadGoStop);					// 停止しているスレッドを動かすため


	// LockFrame()でブロック中の場合、キャンセル
	for(int ii=0; ii<MAX_IPORT_CAMNUM; ii++) {
		if( NULL == mycls_pIport[ii] ) continue;

		mycls_pIport[ii]->AbortLock();			// LockFrame()から出す
	}

	// 基本処理
	ThreadManager::Stop(5000);
	ResetEvent(my_hThreadGoStop);				// 元の停止状態に戻す
	return true;
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 撮像系

//------------------------------------------
// 撮像開始
//------------------------------------------
int IportManager1p::StartGrab()
{
	DWORD ii, jj, ch;

	LOG(em_MSG), "[%s] StartGrab･･･", my_sThreadName);	
	LOM(em_MSG), "[%s] StartGrab･･･", my_sThreadName);


	//==============================================
	// iPort
	if( IportManager::GRAB_MODE_IPORT == my_emGrabMode ) {
		//// 条件チェック
		for(ii=0; ii<my_nCamsetNum; ii++) {
			for(jj=0; jj<my_nAngleNum; jj++) {
				ch = GetCamIndex(ii, jj);

				_ASSERT(NULL != mycls_pIport[ch]);
				_ASSERT(mycls_pIport[ch]->GetFrameNChannel() == my_nChannel);
			}
		}

		//// StartGrab
		for(ii=0; ii<my_nCamsetNum; ii++) {
			for(jj=0; jj<my_nAngleNum; jj++) {
				ch = GetCamIndex(ii, jj);

				mycls_pIport[ch]->ClearQueueItemInBuffer();
				ERRCODE ret = mycls_pIport[ch]->StartGrab();
				if( NoErr == ret ) {
					LOG(em_MSG), "[%s] [%s] OK", my_sThreadName, mycls_pIport[ch]->GetThreadName());	
					LOM(em_MSG), "[%s] [%s] OK", my_sThreadName, mycls_pIport[ch]->GetThreadName());	
				} else {
					LOG(em_ERR), "[%s] [%s] Failed(ret=%d)", my_sThreadName, mycls_pIport[ch]->GetThreadName(), ret);	
					LOM(em_ERR), "[%s] [%s] Failed(ret=%d)", my_sThreadName, mycls_pIport[ch]->GetThreadName(), ret);	
					return ret;
				}
			}
		}
	}

	//==============================================
	// BitMap or AVI
	else if( IportManager::GRAB_MODE_BMP == my_emGrabMode || 
			 IportManager::GRAB_MODE_AVI == my_emGrabMode ) {

		my_nSmpNowNo = my_nSmpIntervalNo[0];
		__int64 ts = my_nSmpCycleTimer * -10000;	// 1[ms]→100[ns] (マイナス指定で、指定時間経過でシグナル) (第3引数は2回目以降の起動間隔)
		SetWaitableTimer(my_evSmpImgTimer, (LARGE_INTEGER *)&ts, my_nSmpCycleTimer, NULL, NULL, FALSE);
	}



	//// もろもろ初期化
	for(ii=0; ii<MAX_IPORT_CAMNUM; ii++) {
		my_nSeqNum[ii]	= 0;
		my_nChDiff[ii]	= 0;
		my_nSkipCnt[ii] = 0;
	}
	my_nFrameNum		= 0;
	my_nQueueCount		= 0;
	my_nMaxQueueCount	= 0;
	my_bGrabRun			= true;


	//// iPort時のみの処理
	if( IportManager::GRAB_MODE_IPORT == my_emGrabMode ) {
		SetEvent(my_hThreadGoStop);			// メインスレッドを実行状態
	}

	LOG(em_MSG), "[%s] StartGrab 完了", my_sThreadName);
	LOM(em_MSG), "[%s] StartGrab 完了", my_sThreadName);	
	return NoErr;
}

//------------------------------------------
// 撮像停止
//------------------------------------------
int IportManager1p::StopGrab()
{
	DWORD ii, jj, ch;
	

	LOG(em_MSG), "[%s] StopGrab･･･", my_sThreadName);	
	LOM(em_MSG), "[%s] StopGrab･･･", my_sThreadName);
	
	ERRCODE ret = NoErr;

	//==============================================
	// iPort
	if( IportManager::GRAB_MODE_IPORT == my_emGrabMode ) {
		ResetEvent(my_hThreadGoStop);		// メインスレッドを実行中止

		//// StopGrab
		for(ii=0; ii<my_nCamsetNum; ii++) {
			for(jj=0; jj<my_nAngleNum; jj++) {
				ch = GetCamIndex(ii, jj);

				if( NULL != mycls_pIport[ch] ) {

					LOG(em_MSG), "[%s] [%s] StopGrab発行", my_sThreadName, mycls_pIport[ch]->GetThreadName());
					LOM(em_MSG), "[%s] [%s] StopGrab発行", my_sThreadName, mycls_pIport[ch]->GetThreadName());
					ERRCODE ret2 = mycls_pIport[ch]->StopGrab();

					if (ret2 != NoErr) ret = ret2;

					LOG(em_MSG), "[%s] [%s] AbortLock発行", my_sThreadName, mycls_pIport[ch]->GetThreadName());
					LOM(em_MSG), "[%s] [%s] AbortLock発行", my_sThreadName, mycls_pIport[ch]->GetThreadName());
					mycls_pIport[ch]->AbortLock();
				}
			}
		}
	}
	
	//==============================================
	// BitMap or AVI
	else if( IportManager::GRAB_MODE_BMP == my_emGrabMode || 
			 IportManager::GRAB_MODE_AVI == my_emGrabMode ) {

		CancelWaitableTimer(my_evSmpImgTimer);
	}

	my_bGrabRun			= false;
	LOG(em_MSG), "[%s] StopGrab 完了", my_sThreadName);
	LOM(em_MSG), "[%s] StopGrab 完了", my_sThreadName);
	return ret;
}

//------------------------------------------
// リセット
//------------------------------------------
bool IportManager1p::Reset()
{
	DWORD ii, jj, ch;

	//==============================================
	// iPort
	if( IportManager::GRAB_MODE_IPORT == my_emGrabMode ) {
		for(ii=0; ii<my_nCamsetNum; ii++) {
			for(jj=0; jj<my_nAngleNum; jj++) {
				ch = GetCamIndex(ii, jj);

				// iPortのバッファクリア
				mycls_pIport[ch]->ClearQueueItemInBuffer();

				// マスターの場合、リセットモジュール実行
				if( my_reset_module ) {
					mycls_pIport[ch]->ResetModule();
				}

				// その他メンバー初期化
				my_nChDiff[ch] = 0;
			}
		}
	}
	
	//==============================================
	// BitMap or AVI
	else if( IportManager::GRAB_MODE_BMP == my_emGrabMode || 
			 IportManager::GRAB_MODE_AVI == my_emGrabMode ) {

		 my_nSmpNowNo = my_nSmpIntervalNo[0];
	}
	
	
	//// その他メンバー初期化
	for(ii=0; ii<MAX_IPORT_CAMNUM; ii++) {
		my_nSeqNum[ii]	= 0;
	}
	my_nFrameNum		= 0;
	my_nQueueCount		= 0;
	my_nMaxQueueCount	= 0;

	my_nResendTotal		= 0;
	my_nLostTotal		= 0;
	my_nChDiffMax		= 0;

	return true;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// iPort 操作系

//------------------------------------------
// iPortに接続
// DWORD set カメラセット (0オリジン)
// DWORD ang カメラ角度 (0オリジン)
// CString sXmlPath XMLフルパス
// bool bSaveIpAddr カメラ本体にIPアドレスを記憶させるか？ (true:XMLに書かれているIPを設定) ※ たぶんfalse時は、IPアドレスでコネクト。true時は、MACアドレスでコネクトすると思われる
//						(カメラ電源OFF→ONで、IPを忘れてしまうため)
// bool bIsSlave SLAVEモードで接続 (true:XMLでマスター指定時でも、強制的にSLAVEになる。false:XMLの指定通り)
// long nAdapterIndex iPort用デバイスマネージャーの何口目を使用するか (0オリジン) (-1:XMLに記述されているアダプターを使用)
// int nGrabQueSize グラバーのリングバッファサイズ
// IportLib::ERRCODE 復帰情報
//------------------------------------------
IportLib::ERRCODE IportManager1p::Connect(DWORD set, DWORD ang, CString sXmlPath, bool bSaveIpAddr, bool bIsSlave, long nAdapterIndex, int nGrabQueSize)
{
/*
	IportLib::ERRCODE	ret;
	int ch = GetCamIndex(set, ang);

	_ASSERT( NULL == mycls_pIport[ch]);

	//// カメラの準備
	CString sWk;
	sWk.Format("iPortBase_%d", ch+1); 
	mycls_pIport[ch] = new IportBase(sWk);
	mycls_pIport[ch]->SetLogMgr(mcls_pLog);
	if( ! mycls_pIport[ch]->Init() ) {
		LOG(em_ERR), "[%s] Init %s 失敗", my_sThreadName, mycls_pIport[ch]->GetThreadName());
		LOM(em_ERR), "[%s] Init %s 失敗", my_sThreadName, mycls_pIport[ch]->GetThreadName());
		DisConnect();
		return ApErr;
	}


	//// iPort接続
	ret = mycls_pIport[ch]->SetupFromXMLFile(sXmlPath, bSaveIpAddr, bIsSlave, nAdapterIndex, nGrabQueSize);
	if( NoErr == ret ) {
		
		// ※ 太平製はpitch8のみ
		if( 0 != mycls_pIport[ch]->GetFrameWidth() % (mycls_pIport[ch]->GetFramePixelNBit()/my_nChannel) ) {
			LOG(em_ERR), "[%s] %s pitchサイズ不整合 (%d)", my_sThreadName, mycls_pIport[ch]->GetDeviceIP(), mycls_pIport[ch]->GetFramePixelNBit());	
			LOM(em_ERR), "[%s] %s pitchサイズ不整合 (%d)", my_sThreadName, mycls_pIport[ch]->GetDeviceIP(), mycls_pIport[ch]->GetFramePixelNBit());	
			return ApErr;
		}

		// 最低限のXML整合性チェック
		if( my_nFrameW != mycls_pIport[ch]->GetFrameWidth() ||
			my_nFrameH != mycls_pIport[ch]->GetFrameHeight() ) {
			LOG(em_ERR), "[%s] %s XMLサイズ不整合 (w=%d/%d,h=%d/%d)", my_sThreadName, mycls_pIport[ch]->GetDeviceIP(), my_nFrameW, mycls_pIport[ch]->GetFrameWidth(), my_nFrameH, mycls_pIport[ch]->GetFrameHeight());	
			LOM(em_ERR), "[%s] %s XMLサイズ不整合 (w=%d/%d,h=%d/%d)", my_sThreadName, mycls_pIport[ch]->GetDeviceIP(), my_nFrameW, mycls_pIport[ch]->GetFrameWidth(), my_nFrameH, mycls_pIport[ch]->GetFrameHeight());	
			return ApErr;
		} else {

			LOG(em_INF), "[%s] %s 接続完了(w=%d,h=%d)[%s]", my_sThreadName, mycls_pIport[ch]->GetDeviceIP(), my_nFrameW, my_nFrameH, sXmlPath);	
			LOM(em_INF), "[%s] %s 接続完了(w=%d,h=%d)[%s]", my_sThreadName, mycls_pIport[ch]->GetDeviceIP(), my_nFrameW, my_nFrameH, sXmlPath);	
		}
	} else {
		LOG(em_ERR), "[%s] SetupFromXMLFile失敗=%d [%s]", my_sThreadName, ret, sXmlPath);
		LOM(em_ERR), "[%s] SetupFromXMLFile失敗=%d [%s]", my_sThreadName, ret, sXmlPath);
		return ret;
	}

	
	//// その他必要なデータ
	my_emGrabMode = IportManager::GRAB_MODE_IPORT;
	return NoErr;
*/
	CyResult ret = CY_RESULT_OK;
	CString		ip;
	CString		mac;
	CString		mc;

	// IP取得
	ret = IportBase::GetXMLFileToIP(sXmlPath, &ip, &mac, &mc);
	if ( CY_RESULT_OK != ret) RETURN_CY_ERR(ret);

	// 接続
	//ret = Connect(set, ang, sXmlPath, ip, mac, mc, bSaveIpAddr, bIsSlave, nAdapterIndex, nGrabQueSize);
	ret = Connect(set, ang, sXmlPath, mac, ip, mc, bSaveIpAddr, bIsSlave, nAdapterIndex, nGrabQueSize);
	RETURN_CY_ERR(ret);
}

IportLib::ERRCODE IportManager1p::Connect(DWORD set, DWORD ang, CString sXmlPath, const char* mac_addr, const char* ip_addr, char const* mc_addr, bool bSaveIpAddr, bool bIsSlave, long nAdapterIndex, int nGrabQueSize)
{
	IportLib::ERRCODE	ret;
	int ch = GetCamIndex(set, ang);

	_ASSERT( NULL == mycls_pIport[ch]);

	//// カメラの準備
	CString sWk;
	sWk.Format("iPortBase_%d", ch+1); 
	mycls_pIport[ch] = new IportBase(sWk);
	mycls_pIport[ch]->SetLogMgr(mcls_pLog);
	if( ! mycls_pIport[ch]->Init() ) {
		LOG(em_ERR), "[%s] Init %s 失敗", my_sThreadName, mycls_pIport[ch]->GetThreadName());
		LOM(em_ERR), "[%s] Init %s 失敗", my_sThreadName, mycls_pIport[ch]->GetThreadName());
		DisConnect();
		return ApErr;
	}


	//// iPort接続
	ret = mycls_pIport[ch]->SetupFromXMLFile(sXmlPath, mac_addr, ip_addr, mc_addr, bSaveIpAddr, bIsSlave, nAdapterIndex, nGrabQueSize);
	if( NoErr == ret ) {
		
		// ※ 太平製はpitch8のみ
		if( 0 != mycls_pIport[ch]->GetFrameWidth() % (mycls_pIport[ch]->GetFramePixelNBit()/my_nChannel) ) {
			LOG(em_ERR), "[%s] %s pitchサイズ不整合 (%d)", my_sThreadName, mycls_pIport[ch]->GetDeviceIP(), mycls_pIport[ch]->GetFramePixelNBit());	
			LOM(em_ERR), "[%s] %s pitchサイズ不整合 (%d)", my_sThreadName, mycls_pIport[ch]->GetDeviceIP(), mycls_pIport[ch]->GetFramePixelNBit());	
			return ApErr;
		}

		// 最低限のXML整合性チェック
		if( my_nFrameW != mycls_pIport[ch]->GetFrameWidth() ||
			my_nFrameH != mycls_pIport[ch]->GetFrameHeight() ) {
			LOG(em_ERR), "[%s] %s XMLサイズ不整合 (w=%d/%d,h=%d/%d)", my_sThreadName, mycls_pIport[ch]->GetDeviceIP(), my_nFrameW, mycls_pIport[ch]->GetFrameWidth(), my_nFrameH, mycls_pIport[ch]->GetFrameHeight());	
			LOM(em_ERR), "[%s] %s XMLサイズ不整合 (w=%d/%d,h=%d/%d)", my_sThreadName, mycls_pIport[ch]->GetDeviceIP(), my_nFrameW, mycls_pIport[ch]->GetFrameWidth(), my_nFrameH, mycls_pIport[ch]->GetFrameHeight());	
			return ApErr;
		} else {

			LOG(em_INF), "[%s] %s 接続完了(w=%d,h=%d)[%s]", my_sThreadName, mycls_pIport[ch]->GetDeviceIP(), my_nFrameW, my_nFrameH, sXmlPath);	
			LOM(em_INF), "[%s] %s 接続完了(w=%d,h=%d)[%s]", my_sThreadName, mycls_pIport[ch]->GetDeviceIP(), my_nFrameW, my_nFrameH, sXmlPath);	
		}
	} else {
		LOG(em_ERR), "[%s] SetupFromXMLFile失敗=%d [%s]", my_sThreadName, ret, sXmlPath);
		LOM(em_ERR), "[%s] SetupFromXMLFile失敗=%d [%s]", my_sThreadName, ret, sXmlPath);
		return ret;
	}

	
	//// その他必要なデータ
	my_emGrabMode = IportManager::GRAB_MODE_IPORT;
	return NoErr;
}



//------------------------------------------
// iPortと切断
//------------------------------------------
void IportManager1p::DisConnect()
{
	//// カメラの解放
	for(int ii=0; ii<MAX_IPORT_CAMNUM; ii++) {
		if( NULL != mycls_pIport[ii] ) {
			mycls_pIport[ii]->Dispose();
			SAFE_DELETE(mycls_pIport[ii]);
		}
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// スレッド操作

//------------------------------------------
// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
//------------------------------------------
int IportManager1p::ThreadFirst()
{
	//// イベント設定
	HANDLE hArray[] = { my_evSmpImgTimer,		// ダミーフレーム時 (ビットマップやAVI時)
						my_hThreadGoStop		// iPort接続時
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// スレッドメインループ
//------------------------------------------
int IportManager1p::ThreadMainLoop()
{
	int		nRetc;
	DWORD	ret;

	////// シグナルの条件
	enum {	EV_STOP = WAIT_OBJECT_0,			// 停止シグナル
			EV_SMP_IMG_TIMER,					// テスト画像取得時 (ビットマップやAVI時)
			EV_IPORT_IMG						// iPort画像取込 (マニュアルのシグナル)
	};


	////////////////////////////////////
	// 前準備
	////////////////////////////////////
	nRetc = ThreadFirst();						// スレッド開始をコールバック
	LOG(em_MSG), "[%s] スレッド開始=%d [0x%X]", my_sThreadName, nRetc, GetTid());

	SetEvent(my_hThreadReady);					// スレッドが起動したことを伝える
	

	////////////////////////////////////
	// メインループ
	////////////////////////////////////

	while( 1 ) {
		ret = WaitForMultipleObjects((my_nWaitNum+1), my_pEvWait, FALSE, INFINITE);

//-----------------------------------------------------------------------------------------------
		// スレッド終了
		if( ret == EV_STOP || ! my_bThreadLoop) {
			//nRetc = ThreadLast();				// スレッド終了をコールバック
			LOG(em_MSG), "[%s] スレッド終了=%d [0x%X]", my_sThreadName, nRetc, GetTid());
			return nRetc;
		}

//-----------------------------------------------------------------------------------------------
		// iPort画像取込
		if( ret == EV_IPORT_IMG ) {
			DWORD ch;

			for(DWORD ii=0; ii<my_nCamsetNum; ii++) {
				bool	camset_noerr = true;
				void*	p[MAX_IPORT_CAMNUM];				// ホントはMAX_IPORT_CAMANGLEが正しいが、chとjjの配列要素を使う箇所が多くなるので、可読性優先で MAX_IPORT_CAMNUM にしている
				DWORD	len[MAX_IPORT_CAMNUM];
				WORD	seq_number[MAX_IPORT_CAMNUM];						// iPort番号はshortなので注意
				DWORD	seqwk;
				DWORD	status[MAX_IPORT_CAMNUM];							// 一応絵は取れたけども、なんかおかしい場合だと思う
				ERRCODE ret[MAX_IPORT_CAMNUM];								// そもそものLockForReadの失敗
				__int64	nFastGetTime;										// 最初のカメラ角度を取得した時間
				bool	lock[MAX_IPORT_CAMNUM];								// LockForReadが成功してから解放するまでの間 true
				memset(lock, 0x00, sizeof(lock));

				//// 1カメラセット分取込
				for(DWORD jj=0; jj<my_nAngleNum; jj++) {
					ch = GetCamIndex(ii, jj);

					// ロック
					LOM(em_MSG), "lock (set=%d, angle=%d)", ii, jj);

					ret[ch]			= mycls_pIport[ch]->LockFrame(p[ch], len[ch], seqwk, status[ch]);		// LockForRead
					seq_number[ch] = (WORD)seqwk;
					if( 0==ret[ch] ) lock[ch] = true;					
					if( 0==jj ) QueryPerformanceCounter((LARGE_INTEGER *)&nFastGetTime);
					DWORD	nResend = mycls_pIport[ch]->GetResendCount();	// 成功した場合にしかセットされない
					DWORD	nLost	= mycls_pIport[ch]->GetLostCount();		// 成功した場合にしかセットされない	
					DWORD	nQueCnt = mycls_pIport[ch]->GetNQueueItemsInBuffer();

					// セット
					my_nResendTotal  += nResend;
					my_nLostTotal    += nLost;			
					my_nQueueCount	  = nQueCnt;
					my_nMaxQueueCount = __max( my_nMaxQueueCount, nQueCnt );
					if( NoErr != ret[ch] ) {
						camset_noerr = false;
						LOM(em_ERR), "->ret=%d, st=%d, wait=%02d, lost=%d, resend=%d, Fno=%d, seq(自,iport)=%d/%d", ret[ch], status[ch], nQueCnt, nLost, nResend, my_nFrameNum, my_nSeqNum[ch], seq_number[ch]);

					} else {
						LOM(em_MSG), "->ret=%d, st=%d, wait=%02d, lost=%d, resend=%d, Fno=%d, seq(自,iport)=%d/%d", ret[ch], status[ch], nQueCnt, nLost, nResend, my_nFrameNum, my_nSeqNum[ch], seq_number[ch]);
					}

					// スキップフレーム？
					if( 0 != my_nSkipCnt[ch] ) {
						// スキップフレームならもう次フレームを連続で取得
						LOG(em_MSG), "[%s] フレーム同期の為、スキップ (set=%d, ang=%d)(Fno=%d,iPort=%d)",my_sThreadName, ii, jj, my_nFrameNum, seq_number[ch]); 
						mycls_pIport[jj]->ReleaseFrame();	// 返却
						jj--;								// 再ループ
						my_nSkipCnt[ch] --;
						continue;
					}
				}

				//// 停止確認 
				if (WAIT_TIMEOUT == WaitForSingleObject(my_hThreadGoStop, 0)) {
					// 取り込んだロック中のフレーム画像を解放 (EPCには無し) ←やっぱりリークの原因となっているっぽい
					for(DWORD jj=0; jj<my_nCamsetNum; jj++) {
						if(lock[jj]) mycls_pIport[jj]->ReleaseFrame();
					}
					LOM(em_MSG), "停止認識!");
					break;	// 停止がかかっていたらループを抜ける
				}


				//// カメラセット単位の処理 (正常時)
				if(camset_noerr) {
					// BadImagePassthroughにした場合、lenがフレームサイズより小さい場合がある。
					//   この場合、ダミーフレーム挿入
					for(DWORD jj=0; jj<my_nAngleNum; jj++) {
						ch = GetCamIndex(ii, jj);
						const unsigned long size0 = mycls_pIport[ch]->GetFrameWidth() * mycls_pIport[ch]->GetFrameHeight() * my_nChannel;

						_ASSERT( (my_nImgSize * my_nChannel) == (size0) ); 

						// たぶん、0x1fはmValueで定義されている17コのうち一個でもNGならダメとしていると思われる。
						if(len[ch] != size0 || (status[ch] & 0x1f) ) {
							LOG(em_WAR), "[%s] ダミーフレームセット (set=%d, ang=%d, no=%d) missing byte %d", my_sThreadName, ii, jj, seq_number[ch], size0-len[ch]);
							p[ch]  = my_pDummyBuf;
							len[ch]= size0;
						}
					}

					// iPortSeqNo が 0 のときがある対策
					if(0 != my_nFrameNum && 0 == seq_number[ch] ) {
						LOG(em_WAR), "[%s] iPortシーケンス番号取得失敗 (set=%d)(Fno=%d, 自=%d, iPort=%d)", my_sThreadName, ii, my_nFrameNum, my_nSeqNum[ch], seq_number[ch]);
						seq_number[ch] = my_nSeqNum[ch] - my_nChDiff[ch];
					}


					// 画像取得
					//   EPCは FrameNumを使用。アプリ側で%カメラセットして、何セット目のデータか特定している。
					OnFrameGrabbed(&p[ii*my_nAngleNum], mycls_pIport[0]->GetFrameWidth(), mycls_pIport[0]->GetFrameHeight(), my_nFrameNum, ii, seq_number, nFastGetTime);


					// 個別のステータスチェック (LockForReadが失敗した場合は、camset_noerrがNgとなる)
					for(DWORD jj=0; jj<my_nAngleNum; jj++) {
						ch = GetCamIndex(ii, jj);
				
						// LockForReadの解放 (※ ２回すると落ちるので注意。必ず1対1となること)
						mycls_pIport[ch]->ReleaseFrame();


						// 異常チェック (回復可能なエラー)
						//if( my_nSeqNum[ch] != seq_number[ch]+my_nChDiff[ch] ) {
						if( (long)my_nSeqNum[ch] != ((long)seq_number[ch]+(long)my_nChDiff[ch]) ) {
							
							if(0 == my_nFrameNum / my_nCamsetNum) {	// 初回のみ特別扱い
								my_nSeqNum[ch] = seq_number[ch]+my_nChDiff[ch];
							} else {
								LOG(em_WAR), "[%s] フレームズレ発生:シーケンス番号不一致 (set=%d, ang=%d)(Fno=%d, 自=%d, iPort=%d + chdiff=%d)", my_sThreadName, 
									ii, jj, my_nFrameNum, my_nSeqNum[ch], seq_number[ch], my_nChDiff[ch]);
								OnAddInfo(IportLib::DIV_ID_ERROR_GRABBER_IMAGEID_MISMATCH, my_nFrameNum, ii, jj, seq_number[ch] );
							}

							my_nChDiff[ch] = (short)((long)my_nSeqNum[ch] - (long)seq_number[ch]);
							if( abs(my_nChDiffMax) > abs(my_nChDiff[ch]) ) my_nChDiffMax = my_nChDiff[ch];
						}

						// シーケンス番号加算
						my_nSeqNum[ch] ++;				// my_seq_numはiportと一致するようにインクリメントする (iPort番号がshortだから)


						// mValue で定義 (回復可能なエラー)
						if( IportBase::IsFrameOverrun(status[ch]) ) {
							LOG(em_WAR), "[%s] フレームオーバーラン発生 (set=%d, ang=%d, no=%d, st=%d)", my_sThreadName, ii, jj, my_nFrameNum, status[ch]);
							OnAddInfo(IportLib::DIV_ID_ERROR_GRABBER_FRAME_OVERRUN, my_nFrameNum, ii, jj, status[ch] );
						}
						if( IportBase::IsGrabberFIFOOverrun(status[ch]) ) {
							LOG(em_WAR), "[%s] FIFOオーバーラン発生 (set=%d, ang=%d, no=%d, st=%d)", my_sThreadName, ii, jj, my_nFrameNum, status[ch]);
							OnAddInfo(IportLib::DIV_ID_ERROR_GRABBER_FIFO_OVERRUN, my_nFrameNum, ii, jj, status[ch] );
						}
						if( IportBase::IsImageDropped(status[ch]) ) {
							LOG(em_WAR), "[%s] IMAGE DROP発生 (set=%d, ang=%d, no=%d, st=%d)", my_sThreadName, ii, jj, my_nFrameNum, status[ch]);
							OnAddInfo(IportLib::DIV_ID_ERROR_GRABBER_IMAGE_DROPPED, my_nFrameNum, ii, jj, status[ch] );
						}
						if( IportBase::IsPartialLineMissing(status[ch]) ) {
							LOG(em_WAR), "[%s] PARTIAL LINE MISSING発生 (set=%d, ang=%d, no=%d, st=%d)", my_sThreadName, ii, jj, my_nFrameNum, status[ch]);
							OnAddInfo(IportLib::DIV_ID_ERROR_GRABBER_PARTIAL_LINE_MISSING, my_nFrameNum, ii, jj, status[ch] );
						}
						if( IportBase::IsFullLineMissing(status[ch]) ) {
							LOG(em_WAR), "[%s] FULL LINE MISSING発生 (set=%d, ang=%d, no=%d, st=%d)", my_sThreadName, ii, jj, my_nFrameNum, status[ch]);
							OnAddInfo(IportLib::DIV_ID_ERROR_GRABBER_FULL_LINE_MISSING, my_nFrameNum, ii, jj, status[ch] );
						}
					}
				}
				//// FYI: AbortLock()がかかるとretはCY_RESULT_ABORTEDになる
				else {
#if 0
					// 失敗したch(まだ救えるやつのみ)には ダミーバッファのアドレスを入れる
					bool len_ok = true;
					for(DWORD jj=0; jj<my_nAngleNum; jj++) {
						ch = GetCamIndex(ii, jj);
						if( ret[ch] == CY_RESULT_MISSING_PACKETS || ret[ch] == CY_RESULT_IMAGE_ERROR) {
							p[ch]	= my_pDummyBuf;
							len[ch] = my_nImgSize;
						}
						if( len[ch] != my_nImgSize) len_ok = false;
					}

					// 画像取得
					//   画像サイズが正常な場合のみ
					if(len_ok) OnFrameGrabbed(&p[ii*my_nAngleNum], len[ii*my_nAngleNum], my_nFrameNum, ii, seq_number, nFastGetTime);
#endif					

					for(DWORD jj=0; jj<my_nAngleNum; jj++) {
						ch = GetCamIndex(ii, jj);
						// 正常だったiPortのみ解放
						if( ret[ch] == NoErr ) {
							// LockForReadの解放 (※ ２回すると落ちるので注意。必ず1対1となること)
							mycls_pIport[ch]->ReleaseFrame();

							// 異常チェック (回復可能なエラー)
							if( my_nSeqNum[ch] != seq_number[ch]+my_nChDiff[ch] ) {
								LOG(em_WAR), "[%s] フレームズレ発生:シーケンス番号不一致 (set=%d, ang=%d)(Fno=%d, 自:%d != iPort:%d + chdiff:%d)", my_sThreadName, ii, jj, my_nFrameNum, my_nSeqNum[ch], seq_number[ch], my_nChDiff[ch]);
								OnAddInfo(IportLib::DIV_ID_ERROR_GRABBER_IMAGEID_MISMATCH, my_nFrameNum, ii, jj, seq_number[ch] );
							
								my_nChDiff[ch] = (short)(my_nSeqNum[ch] - seq_number[ch]);
								if( abs(my_nChDiffMax) > abs(my_nChDiff[ch]) ) my_nChDiffMax = my_nChDiff[ch];
							}

							// シーケンス番号加算
							my_nSeqNum[ch] ++;				// my_seq_numはiportと一致するようにインクリメントする (iPort番号がshortだから)

						
						} else if( ret[ch] == CY_RESULT_TIMEOUT ) {
							// LockForRead 失敗 (回復可能なエラー)
							LOG(em_WAR), "[%s] LockForReadタイムアウト (set=%d, ang=%d, no=%d)", my_sThreadName, ii, jj, my_nFrameNum);
							OnAddInfo(IportLib::DIV_ID_ERROR_GRABBER_FRAME_LOCK_TIMEOUT, my_nFrameNum, ii, jj, 0 );

						} else if( ret[ch] != CY_RESULT_ABORTED ) {
							// LockForRead 失敗 (回復可能なエラー)
							LOG(em_WAR), "[%s] LockForRead失敗 (set=%d, ang=%d, no=%d, retCy=%d)", my_sThreadName, ii, jj, my_nFrameNum, ret[ch]);
							OnAddInfo(IportLib::DIV_ID_ERROR_GRABBER_FRAME_LOCK_FAILED, my_nFrameNum, ii, jj, ret[ch] );

						} else {
							// CY_RESULT_ABORTED 時は、上の停止確認でキャッチできているはずなので、ここまで来ることはありえない 
						}
					}
				}
			}
			my_nFrameNum ++;			// フレーム番号
		}

//-----------------------------------------------------------------------------------------------
		// テスト画像用
		else if( ret == EV_SMP_IMG_TIMER) {
			if( IportManager::GRAB_MODE_BMP == my_emGrabMode ) {
				OnBmpFrameGrabbed();

			} else if(IportManager::GRAB_MODE_AVI == my_emGrabMode ) {
				OnAviFrameGrabbed();
			}
		}
	}

	// ありえない
	LOG(em_ERR), "[%s] メインループから抜けた！！=%d [0x%X]", my_sThreadName, nRetc, GetTid());
	return -1;
}

//------------------------------------------
// 1カメラセット分の画像を取得して、スレッドループキューにキューイング
// void** pImgs 画像(カメラ角度分)
// long nX 画像サイズ (1コだけ)
// long nY 画像サイズ (1コだけ)
// DWORD nFrameNum フレームNo (1コだけ)
// DWORD nCamset カメラセット単位の何個目(0オリジン)
// WORD* nSeqNum iPortシーケンス番号
// __int64 nFastTime 最初のカメラ角度を取得した時間
//------------------------------------------
void IportManager1p::OnFrameGrabbed(void** pImgs, long nX, long nY, DWORD nFrameNum, DWORD nCamset, WORD* nSeqNum, __int64 nFastTime)
{
	//// 空き器を取得
	FRAME_DATA* p = myque_pFrame->GetFree(0);				// セマフォー待ちバージョン(すぐに結果を返したい)
	if( NULL == p ) {
		// 器空きなし (回復可能なエラー)
		LOG(em_ERR), "[%s] GRABバッファオーバラン(set=%d, No=%d):スレッドループキュー空き無し!", my_sThreadName, nCamset, nFrameNum);
		OnAddInfo(IportLib::DIV_ID_ERROR_NO_IMG_QUEFULL, nFrameNum, nCamset, 0, 0 );
		return;
	}

	/*//// ----->>>>
			p->pImgs[ 10 +my_nFrameW*0] = 0x0f;
			p->pImgs[ 10 +my_nFrameW*1] = 0x0f;
			p->pImgs[ 10 +my_nFrameW*2] = 0x0f;
			p->pImgs[ 10 +my_nFrameW*3] = 0x0f;
			p->pImgs[ 10 +my_nFrameW*4] = 0x0f;

				CString str;
				str.Format("C:\\test\\a%d_%d.bmp",nCamset,nFrameNum) ;
				KizuBitmapManager k;
				k.DataToBMPfile_Gray(my_nFrameW, my_nFrameH, &p->pImgs[ my_nImgSize * 0 ], str); 

			//	std::ofstream   os;
			//	os.open(str, std::ios::binary);
			//	os.write((const char*)&p->pImgs[ my_nImgSize * jj ], my_nImgSize);
			//	os.close();
								
	//*/// <<<<-------	

	//// 画像を格納
	// 時間をセット
	p->cls_Time.PutTimestamp(0, nFastTime);			// ＃０：最初のカメラ角度を取得した時間

	// データコピー
	p->nCamset	= nCamset;
	p->nLen		= nX * nY;
	p->nNo		= nFrameNum;

	for(DWORD ii=0; ii<my_nAngleNum; ii++) {
		p->nSeqNum[ii] = nSeqNum[ii];
		p->hInfo[ii]   = my_hAddInfo[GetCamIndex(nCamset, ii)];

		// なぜかここだと、両方とも3msかかる。通常MMXなら0.03msとかになるはずなのに
//#ifdef IPORTLIB_IMG_COPY_MMX
//		SwdetectMMX::mem_copy_64xmm(&p->pImgs[ii*my_nImgSize], pImgs[ii], nBytes);
//#else
//		memcpy( &p->pImgs[ii*my_nImgSize], pImgs[ii], nBytes );	
//#endif
		if( ! my_bImgTurn_LR ) {
			KizuMMX::mem_copy_64xmm(&p->pImgs[ii*my_nImgSize], pImgs[ii], (nX * nY));
		} else {
			_ASSERT( false );	// 未対応
		}
		//memset(&p->pImgs[ my_nFrameW*0 ],0xff,128);
		//memset(&p->pImgs[ my_nFrameW*1 ],0xff,128);
		//memset(&p->pImgs[ my_nFrameW*2 ],0xff,128);
		//memset(&p->pImgs[ my_nFrameW*3 ],0xff,128);
		//memset(&p->pImgs[ my_nFrameW*4 ],0xff,128);
	}

	p->cls_Time.PutTimestamp(1);					// ＃１：全部のカメラ角度が揃った時間 (=今)

	// 登録
	myque_pFrame->AddPool(p); 
//	myque_pFrame->AddFree(p);  // テスト
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// その他小物

//------------------------------------------
// カメラ温度を取得 (カメラと接続時しかダメ)
// DWORD set カメラセット (0オリジン)
// DWORD ang カメラ角度 (0オリジン)
// float& temp 温度 [度]
//------------------------------------------
bool IportManager1p::SendSerialTemp(DWORD set, DWORD ang, float& temp)
{
	DWORD	ch = GetCamIndex(set, ang);
	CString sAns;
	ERRCODE ret = mycls_pIport[ch]->SendSerialCommand("vt\r", &sAns, 40, false);

	if( NoErr != ret ) {temp = 0.0; return false;}

	//// 数値検索
	int numstart = sAns.FindOneOf("0123456789.");
	CString numstr = sAns.Mid(numstart).SpanIncluding("0123456789.");
	temp = (float)atof((LPCTSTR)numstr);
	return true;
}