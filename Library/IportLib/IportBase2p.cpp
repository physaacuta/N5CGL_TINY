#include "StdAfx.h"
#include "IportBase2p.h"

//#define TTT	// どうもこれでnewしたらメモリリークする

#ifdef TTT
 #include <new>  // 配列の引数付きコンストラクタを初期化するため
#else
// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

#endif
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 基本

//------------------------------------------
// コンストラクタ
// CString sThreadName 自クラスの名前
//------------------------------------------
IportBase2p::IportBase2p(CString sThreadName)
{	
#ifdef TTT
	// 引数付きコンストラクタの初期化
	new( mycls_Iport + 0) IportBase(sThreadName+"M");		// 配列の0番目は、マスター
	new( mycls_Iport + 1) IportBase(sThreadName+"S");		// 配列の1番目は、スレーブ
#endif
	for(int ii=0; ii<2; ii++) my_lock_err[ii] = NoErr;
	my_nSkipPort = 0;
}

//------------------------------------------
// デストラクタ
//------------------------------------------
IportBase2p::~IportBase2p(void)
{
	Dispose();	// 念のため解放

}

//------------------------------------------
// 関数初期化
//------------------------------------------
bool IportBase2p::Init()
{
	for(int ii=0; ii<2; ii++) {
		if( ! mycls_Iport[ii].Init() ) return false;
	}
	my_nSkipPort = 0;
	return true;
}

//------------------------------------------
// 関数開放
//------------------------------------------
void IportBase2p::Dispose()
{
	for(int ii=0; ii<2; ii++) mycls_Iport[ii].Dispose();
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// iPort 操作系

//------------------------------------------
// iPort設定XMLファイルを使用して、カメラに接続 (iPort2口用)
// const char* filename		XMLフルパス (マスター)
// const char* mac_addr1	MACアドレス (マスター)
// const char* ip_addr1		IPアドレス (マスター)
// long nAdapterIndex1		アダプターID (マスター)
// char const* mc_addr1		マルチキャストアドレス (マスター)
// const char* mac_addr2	MACアドレス (スレーブ)
// const char* ip_addr2		IPアドレス (スレーブ)
// long nAdapterIndex2		アダプターID (スレーブ)
// char const* mc_addr2		マルチキャストアドレス (スレーブ)
// bool bIsSlave			SLAVEモードで接続 (true:XMLでマスター指定時でも、強制的にSLAVEになる。false:XMLの指定通り)
// long nAdapterIndex		iPort用デバイスマネージャーの何口目を使用するか (0オリジン) (-1:XMLに記述されているアダプターを使用)
// int nGrabQueSize			グラバーのリングバッファサイズ
// IportLib::ERRCODE 復帰情報
//------------------------------------------
// それぞれのXML設定有り。接続は、マスター、スレーブともXML
IportLib::ERRCODE IportBase2p::SetupFromXMLFile(const char* filename1, const char* filename2,
												long nAdapterIndex1, long nAdapterIndex2, 
												bool bSaveIpAddr, bool bIsSlave, int nGrabQueSize)
{
	IportLib::ERRCODE err		= NoErr;
	const char* filename[]		= { filename1, filename2 };
	const long  nAdapterIndex[] = { nAdapterIndex1, nAdapterIndex2};

	for(int ii=0; ii<2; ii++) {
		// 接続
		err = mycls_Iport[ii].SetupFromXMLFile(filename[ii], bSaveIpAddr, bIsSlave, nAdapterIndex[ii], nGrabQueSize );
		if(NoErr != err) return err;
	}

	// 異なるXMLファイルを使用するため、どんな設定でもOKのはず。だからノーチェック
	return NoErr;
}
// マスターのXML設定使う。接続は、マスター、スレーブとも外部から指示
IportLib::ERRCODE IportBase2p::SetupFromXMLFile(const char* filename,
												const char* mac_addr1, const char* ip_addr1, long nAdapterIndex1, char const* mc_addr1,
												const char* mac_addr2, const char* ip_addr2, long nAdapterIndex2, char const* mc_addr2,
												bool bSaveIpAddr, bool bIsSlave, int nGrabQueSize)
{
	IportLib::ERRCODE err		= NoErr;
	const char* mac_addr[]		= { mac_addr1, mac_addr2 };
	const char* ip_addr[]		= { ip_addr1, ip_addr2 };
	const long  nAdapterIndex[] = { nAdapterIndex1, nAdapterIndex2};
	const char* mc_addr[]		= { mc_addr1, mc_addr2 };

	for(int ii=0; ii<2; ii++) {
		// 引数チェック
		if( NULL == mac_addr[ii] || NULL == ip_addr[ii] || 0 > nAdapterIndex[ii] ) {
			RETURN_CY_ERR(CY_RESULT_INVALID_ARGUMENT);
		}

		// 接続
		err = mycls_Iport[ii].SetupFromXMLFile(filename, mac_addr[ii], ip_addr[ii], mc_addr[ii], bSaveIpAddr, bIsSlave, nAdapterIndex[ii], nGrabQueSize );
		if(NoErr != err) return err;
	}

	// 画像定義チェック
	if( mycls_Iport[0].GetFrameHeight()		!= mycls_Iport[1].GetFrameHeight() ||
		mycls_Iport[0].GetFrameNChannel()	!= mycls_Iport[1].GetFrameNChannel() ||
		mycls_Iport[0].GetFramePixelNBit()	!= mycls_Iport[1].GetFramePixelNBit() ) {
		RETURN_CY_ERR(CY_RESULT_INVALID_ARGUMENT);
	}
	
	return NoErr;
}

// マスターのXML設定使う。接続は、マスターはXML、スレーブは外部から指示
IportLib::ERRCODE IportBase2p::SetupFromXMLFile(const char* filename, long nAdapterIndex1,
											    const char* mac_addr2, const char* ip_addr2, long nAdapterIndex2, char const* mc_addr2,
												bool bSaveIpAddr, bool bIsSlave, int nGrabQueSize)
{
	IportLib::ERRCODE err		= NoErr;

	// マスター
	err = mycls_Iport[0].SetupFromXMLFile(filename, bSaveIpAddr, bIsSlave, nAdapterIndex1, nGrabQueSize );
	if(NoErr != err) return err;
	// スレーブ
	err = mycls_Iport[1].SetupFromXMLFile(filename, mac_addr2, ip_addr2, mc_addr2, bSaveIpAddr, bIsSlave, nAdapterIndex2, nGrabQueSize );
	if(NoErr != err) return err;

	// 画像定義チェック
	if( mycls_Iport[0].GetFrameHeight()		!= mycls_Iport[1].GetFrameHeight() ||
		mycls_Iport[0].GetFrameNChannel()	!= mycls_Iport[1].GetFrameNChannel() ||
		mycls_Iport[0].GetFramePixelNBit()	!= mycls_Iport[1].GetFramePixelNBit() ) {
		RETURN_CY_ERR(CY_RESULT_INVALID_ARGUMENT);
	}
	
	return NoErr;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (5) grabの制御
//------------------------------------------
// カメラ取込開始
// IportLib::ERRCODE 復帰情報
//------------------------------------------
IportLib::ERRCODE IportBase2p::StartGrab()
{
	IportLib::ERRCODE err		= NoErr;

	// マスター
	err = mycls_Iport[0].StartGrab();
	if(NoErr != err) return err;

	// スレーブ
	err = mycls_Iport[1].StartGrab();
	if(NoErr != err) {
		mycls_Iport[0].StopGrab();	// すでにマスター側は開始しているため、停止させる
		return err;
	}

	return NoErr;
}

//------------------------------------------
// カメラ取込停止
// IportLib::ERRCODE 復帰情報
//------------------------------------------
IportLib::ERRCODE IportBase2p::StopGrab()
{
	IportLib::ERRCODE err0 = mycls_Iport[0].StopGrab();
	IportLib::ERRCODE err1 = mycls_Iport[1].StopGrab();
	if(NoErr != err0) return err0;
	if(NoErr != err1) return err1;
	return NoErr;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (6) デバイスの切断
//------------------------------------------
// デバイスの切断
// IportLib::ERRCODE 復帰情報
//------------------------------------------
IportLib::ERRCODE IportBase2p::Disconnect()
{
	IportLib::ERRCODE err0 = mycls_Iport[0].Disconnect();
	IportLib::ERRCODE err1 = mycls_Iport[1].Disconnect();
	if(NoErr != err0) return err0;
	if(NoErr != err1) return err1;
	return NoErr;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (7) grabした画像の取り扱い
//------------------------------------------
// 画像読み込み(画像取り込み完了までブロッキング)
// void*& p 画像格納ポインタ
// unsigned long& len 取得画像サイズ
// unsigned long& iid iPortシーケンスNo (0オリジン)
// unsigned long& status ステータス
// IportLib::ERRCODE 復帰情報
//------------------------------------------
IportLib::ERRCODE IportBase2p::LockFrame(void*& p0, unsigned long& len0, unsigned long& iid0, unsigned long& stat0,
										 void*& p1, unsigned long& len1, unsigned long& iid1, unsigned long& stat1)
{
    void**            p[] = {&p0,    &p1};
    unsigned long*  len[] = {&len0,  &len1};
    unsigned long*  iid[] = {&iid0,  &iid1};
    unsigned long* stat[] = {&stat0, &stat1};

	// ズレ補正対策 ----------------------->> 
	if( 0 != my_nSkipPort ) {
		int nIndex;
		int nCnt;
		// 1カメラ内でiPortシーケンスNoズレ補正用 (0:無し 1:マスターを1回スキップ -1:スレーブを1回スキップ)
		if( 0 < my_nSkipPort) {
			nIndex = 0;
			nCnt = my_nSkipPort;
		} else {
			nIndex = 1;
			nCnt = my_nSkipPort*-1;
		}
		// 指定回数分取り込むことで、マスタースレーブを同じにする
		for(int ii=0; ii<nCnt; ii++) {
			if(NoErr == mycls_Iport[nIndex].LockFrame(*p[nIndex], *len[nIndex], *iid[nIndex], *stat[nIndex]) ) {
				mycls_Iport[nIndex].ReleaseFrame();	
			}
		}
		my_nSkipPort = 0;
	}
	// <<---------------------------------------


	// 取り込み
	for(int ii=0; ii<2; ii++) {
		my_lock_err[ii] = mycls_Iport[ii].LockFrame(*p[ii], *len[ii], *iid[ii], *stat[ii]);
	}

	// エラーチェック
	if(NoErr != my_lock_err[0]) {
		if(NoErr == my_lock_err[1]) mycls_Iport[1].ReleaseFrame();
		return my_lock_err[0];

	} else if(NoErr != my_lock_err[1]) {
		mycls_Iport[0].ReleaseFrame();
		return my_lock_err[1];
	}

	return NoErr;
}

//------------------------------------------
// 画像読み込み領域返却
// IportLib::ERRCODE 復帰情報
//------------------------------------------
void IportBase2p::ReleaseFrame() const
{
	for(int ii=0; ii<2; ii++) {
		mycls_Iport[ii].ReleaseFrame();
	}
}
//------------------------------------------
// LockForReadから強制的に抜ける
// IportLib::ERRCODE 復帰情報
//------------------------------------------
void IportBase2p::AbortLock() const
{
	for(int ii=0; ii<2; ii++) {
		mycls_Iport[ii].AbortLock();
	}
}
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (9) Reset
//------------------------------------------
// デバイスの切断
// IportLib::ERRCODE 復帰情報
//------------------------------------------
IportLib::ERRCODE IportBase2p::ResetModule()
{
	IportLib::ERRCODE err0 = mycls_Iport[0].ResetModule();
	IportLib::ERRCODE err1 = mycls_Iport[1].ResetModule();
	if(NoErr != err0) return err0;
	if(NoErr != err1) return err1;
	return NoErr;
}