#include "StdAfx.h"
#include "IportManager.h"

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
//------------------------------------------
IportManager::IportManager(CString sMyManagerName) :
// 名前
GrabberManager(GRAB_TYPE_IPORT, sMyManagerName),

my_nCamsetNum(0),
my_nAngleNum(0),
my_nFrameNum(0),
my_nResendTotal(0),
my_nLostTotal(0),
my_nQueueCount(0),
my_nMaxQueueCount(0),
my_nImgSize(0),
my_nFrameW(0),
my_nFrameH(0),
my_nChannel(1),

my_reset_module(true),	// 初期値は、SpyderGigE モード
my_pDummyBuf(NULL),

my_bImgTurn_LR(false),

my_nSmpPathFormatMode(0),
my_hThreadReady(INVALID_HANDLE_VALUE),
my_hThreadGoStop(INVALID_HANDLE_VALUE),
my_bThreadLoop(false)
{


	for(int ii=0; ii<MAX_IPORT_CAMNUM; ii++) {
		my_nSeqNum[ii]		= 0;
		my_nSkipCnt[ii]		= 0;

		for(int jj=0; jj<MAX_IPORT_CHANNEL; jj++ ) mycls_Aiv[jj][ii] = NULL;
	}

	my_evSmpImgTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	my_hThreadGoStop = CreateEvent(NULL, TRUE, FALSE, NULL);		// マニュアル
}

//------------------------------------------
// デストラクタ
//------------------------------------------
IportManager::~IportManager(void)
{
	SAFE_DELETE_HANDLE(my_evSmpImgTimer);
	SAFE_DELETE_HANDLE(my_hThreadGoStop);
}




// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// サンプル画像操作系

//------------------------------------------
// 連続ビットマップ定義
// char const* cPath ビットマップファイルのフォーマット(形式は、カメラセット(0オリジン),カメラ角度(0オリジン),連番) (例:test_Set0_Ang0_No0.bmp)
// int nNoFrom 連番の開始位置
// int nNoTo 連番の終了位置
// int nCycleTimer 画像取込周期 [ms]
//------------------------------------------
bool IportManager::OpenBmp(char const* cPathFormat, int nNoFrom, int nNoTo, int nCycleTimer)
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
	for(DWORD ii=0; ii<my_nCamsetNum; ii++) {
		for(DWORD jj=0; jj<my_nAngleNum; jj++) {
			for(int kk=my_nSmpIntervalNo[0]; kk<=my_nSmpIntervalNo[1]; kk++) {

				if( SMPPATHFROMAT_NOM == my_nSmpPathFormatMode ) {

					// ファイルパスの生成
					sPath.Format(my_sSmpPathFormat, ii, jj, kk);
					int fh = _open( sPath, _O_RDONLY ); 
					if(-1 == fh ) {
						LOG(em_ERR), "[%s] BitMapファイルがありません。(%s)", my_sThreadName, sPath);
						return false;
					}
					_close(fh);

				} else if( SMPPATHFROMAT_CHN == my_nSmpPathFormatMode ) {

					for(int ll=0; ll<my_nChannel; ll++) {

						// ファイルパスの生成
						sPath.Format(my_sSmpPathFormat, ii, jj, ll, kk);
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
	}

	//// その他必要なデータ
	my_nSmpNowNo		 = nNoFrom;
	my_emGrabMode		 = IportManager::GRAB_MODE_BMP;
	return true;
}

//------------------------------------------
// 連続ビットマップ解放
//------------------------------------------
void IportManager::CloseBmp()
{
	//// 準備はしたけど、今のところ何もしないかな
}

//------------------------------------------
// AVI定義
// char const* cPath AVIファイルのフォーマット(形式は、カメラセット(0オリジン),カメラ角度(0オリジン)) (例:test_Set0_Ang0.avi)
// int nCycleTimer 画像取込周期 [ms]
//------------------------------------------
bool IportManager::OpenAvi(char const* cPathFormat, int nCycleTimer)
{
	//// データセット
	my_sSmpPathFormat	 = cPathFormat;
	my_nSmpCycleTimer	 = nCycleTimer;

	//// AVIファイルの準備
	CString				sPath;
	DWORD				ch;
	bool				ret;

	for(DWORD ii=0; ii<my_nCamsetNum; ii++) {
		for(DWORD jj=0; jj<my_nAngleNum; jj++) {
			ch = GetCamIndex(ii, jj);
		

			if( SMPPATHFROMAT_NOM == my_nSmpPathFormatMode ) {
				// 生成		
				mycls_Aiv[0][ch] = new KizuAviManager();
				// ファイルパスの生成
				sPath.Format(my_sSmpPathFormat, ii, jj);
				// AVIファイルオープン
				ret = mycls_Aiv[0][ch]->OpenAVIFile(sPath); 
				if( ! ret) {
					LOG(em_ERR), "[%s] AVIファイルがありません。(%s)", my_sThreadName, sPath);
					return false;
				}

			} else if( SMPPATHFROMAT_CHN == my_nSmpPathFormatMode ) {
				for(int ll=0; ll<my_nChannel; ll++) {
					// 生成		
					mycls_Aiv[ll][ch] = new KizuAviManager();
					// ファイルパスの生成
					sPath.Format(my_sSmpPathFormat, ii, jj, ll);
					// AVIファイルオープン
					ret = mycls_Aiv[ll][ch]->OpenAVIFile(sPath); 
					if( ! ret) {
						LOG(em_ERR), "[%s] AVIファイルがありません。(%s)", my_sThreadName, sPath);
						return false;
					}
				}
			}

			// 読み込み
			my_nSmpIntervalNo[0] = 0;
			my_nSmpIntervalNo[1] = mycls_Aiv[0][ch]->GetFrameCnt();
		}
	}

	//// その他必要なデータ
	my_nSmpNowNo		 = 0;
	my_emGrabMode		 = IportManager::GRAB_MODE_AVI;
	return true;
}

//------------------------------------------
// AVI解放
//------------------------------------------
void IportManager::CloseAvi()
{
	for(int ii=0; ii<MAX_IPORT_CAMNUM; ii++) {
		for(int jj=0; jj<MAX_IPORT_CHANNEL; jj++) {
			if(NULL == mycls_Aiv[jj][ii]) continue;

			mycls_Aiv[jj][ii]->CloseAVIFile();
			SAFE_DELETE( mycls_Aiv[jj][ii] );
		}
	}
}

//------------------------------------------
// 画像データを手動で追加
//------------------------------------------
bool IportManager::OpenManual()
{
	my_nSmpNowNo		 = 0;
	my_emGrabMode		 = IportManager::GRAB_MODE_MAN;
	return true;
}
//------------------------------------------
// 画像データを手動で追加
//------------------------------------------
void IportManager::CloseManual()
{
	//// 準備はしたけど、今のところ何もしないかな
}


//------------------------------------------
// 全カメラセット、カメラ角度に対してビットマップ画像をセット
//------------------------------------------
void IportManager::OnBmpFrameGrabbed()
{
	KizuBitmapManager	cls_bmp;
	CString				sPath;
	int					nW;
	int					nH;

	for(DWORD ii=0; ii<my_nCamsetNum; ii++) {
		//// 空き器を取得
		FRAME_DATA* p = myque_pFrame->GetFree(0);				// セマフォー待ちバージョン(すぐに結果を返したい)
		if( NULL == p ) {
			// 器空きなし (回復可能なエラー)
			LOG(em_WAR), "[%s] GRABバッファオーバラン(set=%d, No=%d):スレッドループキュー空き無し!", my_sThreadName, ii, my_nFrameNum);
			OnAddInfo(IportLib::DIV_ID_ERROR_NO_IMG_QUEFULL, my_nFrameNum, ii, 0, 0 );
			for(DWORD jj=0; jj<my_nAngleNum; jj++) my_nSeqNum[jj] ++;
			my_nFrameNum ++;
			return;
		}

//memset(p->pImgs,0xff,128);//◆

		p->cls_Time.PutTimestamp(0);						// ＃０：最初のカメラ角度を取得した時間 (=今)


		//// Bitmapファイルを取得
		LOM(em_MSG), "lock (set=%d)", ii);
		for(DWORD jj=0; jj<my_nAngleNum; jj++) {
			p->nSeqNum[jj] = my_nSeqNum[jj];
			p->hInfo[jj]   = my_hAddInfo[GetCamIndex(ii, jj)];


			if( SMPPATHFROMAT_NOM == my_nSmpPathFormatMode ) {
				sPath.Format( my_sSmpPathFormat, ii, jj, my_nSmpNowNo );
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

					sPath.Format( my_sSmpPathFormat, ii, jj, kk, my_nSmpNowNo );
					cls_bmp.BMPfileToData_Gray( sPath, &nW, &nH, &p->pImgs[ my_nImgSize * (jj*my_nChannel+kk) ] );
					
					// 画像上下反転
					KizuAviManager::HantenTB(nW, nH, &p->pImgs[ my_nImgSize * (jj*my_nChannel+kk)  ]);
					_ASSERT( nW == my_nFrameW && nH == my_nFrameH );
				}
			}

			//nW = my_nFrameW;
			//nH = my_nFrameH;
			//memcpy( &p->pImgs[ii*my_nImgSize], my_pDummyBuf, my_nImgSize );	

	//nW = my_nFrameW;
	//nH = my_nFrameH; 
//memset(&p->pImgs[ my_nFrameW*0 ],0xff,128);
//memset(&p->pImgs[ my_nFrameW*1 ],0xff,128);
//memset(&p->pImgs[ my_nFrameW*2 ],0xff,128);
//memset(&p->pImgs[ my_nFrameW*3 ],0xff,128);
//memset(&p->pImgs[ my_nFrameW*4 ],0xff,128);
			
		}

		p->cls_Time.PutTimestamp();							// ＃１：全部のカメラ角度が揃った時間 (=今)

		LOM(em_MSG), "->Fno=%d, path=%s", my_nFrameNum, sPath);


		//// データコピー
		p->nCamset	= ii;
		p->nLen		= nW*nH;
		p->nNo		= my_nFrameNum;

		//// 登録
		myque_pFrame->AddPool(p); 
		//myque_pFrame->AddFree(p);  // テスト
	}

	//// 次のため
	for(DWORD jj=0; jj<my_nAngleNum; jj++) my_nSeqNum[jj] ++;
	my_nFrameNum ++;
	my_nSmpNowNo ++;
	if( my_nSmpNowNo > my_nSmpIntervalNo[1] ) my_nSmpNowNo = my_nSmpIntervalNo[0];		// 一周まわったから最初の画像を取得
}

//------------------------------------------
// 全カメラセット、カメラ角度に対してAVI画像をセット
//------------------------------------------
void IportManager::OnAviFrameGrabbed()
{
	bool ret;

	for(DWORD ii=0; ii<my_nCamsetNum; ii++) {
		//// 空き器を取得
		FRAME_DATA* p = myque_pFrame->GetFree(0);				// セマフォー待ちバージョン(すぐに結果を返したい)
		if( NULL == p ) {
			// 器空きなし (回復可能なエラー)
			LOG(em_WAR), "[%s] GRABバッファオーバラン(set=%d, No=%d):スレッドループキュー空き無し!", my_sThreadName, ii, my_nFrameNum);
			OnAddInfo(IportLib::DIV_ID_ERROR_NO_IMG_QUEFULL, my_nFrameNum, ii, 0, 0 );
			for(DWORD jj=0; jj<my_nAngleNum; jj++) my_nSeqNum[jj] ++;
			my_nFrameNum ++;
			return;
		}


		p->cls_Time.PutTimestamp(0);						// ＃０：最初のカメラ角度を取得した時間 (=今)

		
		//// AVI画像読み込み
		LOM(em_MSG), "lock (set=%d)", ii);
		for(DWORD jj=0; jj<my_nAngleNum; jj++) {
			p->nSeqNum[jj] = my_nSeqNum[jj];
			p->hInfo[jj]   = my_hAddInfo[GetCamIndex(ii, jj)];

			if( SMPPATHFROMAT_NOM == my_nSmpPathFormatMode ) {
				ret = mycls_Aiv[0][ii]->LoadFromAVI( my_nSmpNowNo, &p->pImgs[ my_nImgSize * (jj*my_nChannel) ], true );
				if( ! ret ) {
					LOG(em_ERR), "[%s] AVIファイル読み込み失敗 (set=%d, ang=%d, no=%d)", my_nFrameNum, ii, jj, my_nSmpNowNo);
				}

				// チャンネル数分ループ。色差対応
				for(int kk=1; kk<my_nChannel; kk++) {
					memcpy( &p->pImgs[ (jj*my_nChannel+kk) * my_nImgSize], &p->pImgs[ my_nImgSize * (jj*my_nChannel) ], my_nImgSize );	
				}
			} else if( SMPPATHFROMAT_CHN == my_nSmpPathFormatMode ) {

				for(int kk=0; kk<my_nChannel; kk++) {
					ret = mycls_Aiv[kk][ii]->LoadFromAVI( my_nSmpNowNo, &p->pImgs[ my_nImgSize * (jj*my_nChannel+kk) ], true );
					if( ! ret ) {
						LOG(em_ERR), "[%s] AVIファイル読み込み失敗 (set=%d, ang=%d, ch=%d, no=%d)", my_nFrameNum, ii, jj, kk, my_nSmpNowNo);
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
		p->nCamset	= ii;
		p->nLen		= my_nImgSize;
		p->nNo		= my_nFrameNum;


		//// 登録
		myque_pFrame->AddPool(p); 
		//myque_pFrame->AddFree(p);  // テスト
	}

	//// 次のため
	for(DWORD jj=0; jj<my_nAngleNum; jj++) my_nSeqNum[jj] ++;
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
bool IportManager::OnManualFrameGrabbed(BYTE* pImgs, DWORD nBytes, DWORD nCamset)
{
	//// 空き器を取得
	FRAME_DATA* p = myque_pFrame->GetFree(0);				// セマフォー待ちバージョン(すぐに結果を返したい)
	if( NULL == p ) {
		// 器空きなし (回復可能なエラー)
		LOG(em_ERR), "[%s] GRABバッファオーバラン(set=%d, No=%d):スレッドループキュー空き無し!", my_sThreadName, nCamset, my_nFrameNum);
		OnAddInfo(IportLib::DIV_ID_ERROR_NO_IMG_QUEFULL, my_nFrameNum, nCamset, 0, 0 );
		//my_nSeqNum	 ++;
		for(DWORD jj=0; jj<my_nAngleNum; jj++) my_nSeqNum[jj] ++;
		my_nFrameNum ++;
		return false;
	}
	p->cls_Time.PutTimestamp(0);						// ＃０：最初のカメラ角度を取得した時間 (=今)

	// データコピー
	p->nCamset	= nCamset;
	p->nLen		= nBytes;
	p->nNo		= my_nFrameNum;
	for(DWORD ii=0; ii<my_nAngleNum; ii++) {
		p->nSeqNum[ii] = my_nSeqNum[ii];
		p->hInfo[ii]   = my_hAddInfo[GetCamIndex(nCamset, ii)];

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
	//my_nSeqNum	 ++;
	for(DWORD jj=0; jj<my_nAngleNum; jj++) my_nSeqNum[jj] ++;
	my_nFrameNum ++;
	my_nSmpNowNo ++;
	return true;
}

//------------------------------------------
// 付帯情報キューイング
// EM_DELIVERY_ID em 区分
// DWORD nFrameNum フレームNo
// DWORD set カメラセット (0オリジン)
// DWORD ang カメラ角度 (0オリジン)
// DWORD st ステータス情報
//------------------------------------------
void IportManager::OnAddInfo(EM_DELIVERY_ID em, DWORD nFrameNum, DWORD set, DWORD ang, DWORD st)
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
