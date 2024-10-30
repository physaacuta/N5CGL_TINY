#include "StdAfx.h"
#include "IportBase.h"


// Pleora ヘッダー
#include <CyConfig.h>
#include <CyDeviceFinder.h>
#include <CyCameraRegistry.h>
#include <CyCameraInterface.h>
#include <CyGrabber.h>
#include <CyXMLDocument.h>
//#include <CyPerfMonitor.h>


// メモリーリーク検出 ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


using namespace IportLib;


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ローカル関数

//------------------------------------------
// 接続モード
//------------------------------------------
static ACCESSMODE GetMode(CyConfig::AccessMode mode)
{
    switch( mode )
    {
    case CyConfig::MODE_DRV:    return PerfDrv;
    case CyConfig::MODE_UDP:    return WinStackUDP;
    case CyConfig::MODE_TCP:    return WinStackTCP;
    case CyConfig::MODE_FILTER: return FilterDrv;
    }
    return ModeUnknown;
}
static CyConfig::AccessMode GetMode(ACCESSMODE mode)
{
    switch( mode )
    {
    case PerfDrv:     return CyConfig::MODE_DRV;
    case WinStackUDP: return CyConfig::MODE_UDP;
    case WinStackTCP: return CyConfig::MODE_TCP;
    case FilterDrv:   return CyConfig::MODE_FILTER;
    }
    return CyConfig::MODE_DEFAULT; // Unknown
}

//------------------------------------------
// 現在のmy_pCyCameraの設定から幅・高さ[pixel]を返す
//------------------------------------------
static unsigned short CalcFrameSizeX(const CyCameraInterface& camera)
{
	unsigned short n;
	unsigned short bin, deci;

	camera.GetSizeX(n);
	camera.GetBinningX(bin);
	camera.GetDecimationX(deci);
	n = (n / bin)  + (n % bin  ? 1 : 0);
	n = (n / deci) + (n % deci ? 1 : 0);
	return n; 
}
static unsigned short CalcFrameSizeY(const CyCameraInterface& camera)
{
	unsigned short n;
	unsigned short bin, deci;

	camera.GetSizeY(n);
	camera.GetBinningY(bin);
	camera.GetDecimationY(deci);
	n = (n / bin)  + (n % bin  ? 1 : 0);
	n = (n / deci) + (n % deci ? 1 : 0);
	return n;
}
//------------------------------------------
// 現在のmy_pCyCameraの設定から画素情報を返す
//------------------------------------------
static void CalcFrameNChannelPixelNBit(const CyCameraInterface& camera, long& nch, long& nbit)
{
	CyPixelTypeID pixeltype;

	camera.GetPixelType(pixeltype);
	if( pixeltype & CyPixelType::GRAYSCALE ) {
	    nch = 1;
	} else if( pixeltype & CyPixelType::RGB_COLOUR ) {
	    nch = 3;
	} else {
		nch = 1;	// 通常ありえないはず
	}
	nbit = camera.GetGrabber().GetPixelDepth();
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 基本

//------------------------------------------
// コンストラクタ
// CString sThreadName 自クラスの名前
//------------------------------------------
IportBase::IportBase(CString sThreadName) :
my_pCyGrabber(NULL), my_pCyBuffer(NULL), my_pCyCamera(NULL), 
my_nResendCount(0), my_nLostCount(0), my_bLockWaiting(false),
//my_bIsFreeRun(false), my_nFreeRunHz(0),
my_sThreadName(sThreadName),
mcls_pLog(NULL)
{
	// その他初期化
	memset(&my_typDevEntry,	0x00, sizeof(my_typDevEntry));
	memset(my_cCameraName,	0x00, sizeof(my_cCameraName));
	memset(&my_typFrameInfo,0x00, sizeof(my_typFrameInfo));

}

//------------------------------------------
// デストラクタ
//------------------------------------------
IportBase::~IportBase(void)
{
	Dispose();	// 念のため解放
}

//------------------------------------------
// 関数初期化
//------------------------------------------
bool IportBase::Init()
{
	_ASSERT( NULL == my_pCyGrabber );
	_ASSERT( NULL == my_pCyBuffer );
	_ASSERT( NULL == my_pCyCamera );

	//// インスタンス生成
    my_pCyGrabber	= new CyGrabber;
    my_pCyBuffer	= new CyImageBuffer;

	LOG(em_MSG), "[%s] Init完了", my_sThreadName);
	return true;
}

//------------------------------------------
// 関数開放
//------------------------------------------
void IportBase::Dispose()
{
	//// 解放
	if( NULL != my_pCyCamera) {
		delete my_pCyCamera;
		my_pCyCamera = NULL;
	}
	if( NULL != my_pCyGrabber) {
		//my_pCyGrabber->Disconnect();
		Disconnect();
		delete my_pCyGrabber;
		my_pCyGrabber = NULL;
	}
	if( NULL != my_pCyBuffer) {
		delete my_pCyBuffer;
		my_pCyBuffer = NULL;
	}

	// 初期化
	memset(&my_typDevEntry,	0x00, sizeof(my_typDevEntry));
	memset(my_cCameraName,	0x00, sizeof(my_cCameraName));
	memset(&my_typFrameInfo,0x00, sizeof(my_typFrameInfo));


	// 念の為に確認
	_ASSERT( NULL == my_pCyGrabber );
	_ASSERT( NULL == my_pCyBuffer );
	_ASSERT( NULL == my_pCyCamera );

	LOG(em_MSG), "[%s] Dispose完了", my_sThreadName);
}




// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// iPort 操作系

//------------------------------------------
// iPort設定XMLファイルから IPアドレス関係を取得
// const char* filename XMLフルパス
// char* ip		IPアドレス
// char* mac	MACアドレス
// char* mc		マルチキャストアドレス
// IportLib::ERRCODE 復帰情報
//------------------------------------------
IportLib::ERRCODE IportBase::GetXMLFileToIP(const char* filename, CString* ip, CString* mac, CString* mc)
{
	CyResult ret = CY_RESULT_OK;

	//// XMLファイル読み込み
	CyXMLDocument	cyXmlDoc(filename);
	ret = cyXmlDoc.LoadDocument();
	if( CY_RESULT_OK != ret )	{
		RETURN_CY_ERR(ret);
	}

	//// XMLファイルから内部設定情報に変換
	CyConfig		cyConfig;
	ret = cyConfig.LoadFromXML(cyXmlDoc);
	if( CY_RESULT_OK != ret )	{
		RETURN_CY_ERR(ret);
	}
	cyConfig.SetIndex( 0 );		// 設定情報の何番目を使うか (通常1つしかないから0固定)

	//// XMLファイルからアドレス取得
	// MACアドレス
	CyString mac_addr;
	ret = cyConfig.GetMACAddress(mac_addr);
	if ( CY_RESULT_OK != ret) RETURN_CY_ERR(ret);
	// IPアドレス
	CyString ip_addr;
	ret = cyConfig.GetAddress(ip_addr);
	if ( CY_RESULT_OK != ret) RETURN_CY_ERR(ret);
	// マルチキャストアドレス
	const DWORD mc_len = 256;
	char mc_addr[mc_len];
	ret = cyConfig.GetMulticastAddress(mc_addr, 256);
	if ( CY_RESULT_OK != ret) RETURN_CY_ERR(ret);

	// 返却
	*mac = mac_addr.c_str();
	*ip = ip_addr.c_str();
	*mc = mc_addr;


	RETURN_CY_ERR(CY_RESULT_OK);
}

//------------------------------------------
// iPort設定XMLファイルを使用して、カメラに接続
// const char* filename XMLフルパス
// bool bSaveIpAddr カメラ本体にIPアドレスを記憶させるか？ (true:XMLに書かれているIPを設定) ※ たぶんfalse時は、IPアドレスでコネクト。true時は、MACアドレスでコネクトすると思われる
//						(カメラ電源OFF→ONで、IPを忘れてしまうため)
// bool bIsSlave SLAVEモードで接続 (true:XMLでマスター指定時でも、強制的にSLAVEになる。false:XMLの指定通り)
// long nAdapterIndex iPort用デバイスマネージャーの何口目を使用するか (0オリジン) (-1:XMLに記述されているアダプターを使用)
        // adapter_index = -1の場合はファイル中のadapter_idをそのまま用いる。
        // adapter_index >= 0の場合は、CyAdapterID::GetIdentifier( CyConfig::MODE_DRV, adapter_index, lAdapterID ); で取得したlAdapterIDを用いる
// int nGrabQueSize グラバーのリングバッファサイズ
// IportLib::ERRCODE 復帰情報
//------------------------------------------
IportLib::ERRCODE IportBase::SetupFromXMLFile(const char* filename, 
											  bool bSaveIpAddr, bool bIsSlave, long nAdapterIndex, int nGrabQueSize)
{
/***
	CyResult ret = CY_RESULT_OK;
	my_bIsSlave  = bIsSlave;

	//// XMLファイル読み込み
	CyXMLDocument	cyXmlDoc(filename);
	ret = cyXmlDoc.LoadDocument();
	if( CY_RESULT_OK != ret )	{
		LOG(em_ERR), "[%s] LoadDocument失敗 (%d)", my_sThreadName, ret);
		RETURN_CY_ERR(ret);
	}

	//// XMLファイルから内部設定情報に変換
	CyConfig		cyConfig;
	ret = cyConfig.LoadFromXML(cyXmlDoc);
	if( CY_RESULT_OK != ret )	{
		LOG(em_ERR), "[%s] LoadFromXML失敗 (%d)", my_sThreadName, ret);
		RETURN_CY_ERR(ret);
	}
	cyConfig.SetIndex( 0 );		// 設定情報の何番目を使うか (通常1つしかないから0固定)



	//// XMLファイルからアドレス取得
	// MACアドレス
	CyString mac_addr;
	ret = cyConfig.GetMACAddress(mac_addr);
	if ( CY_RESULT_OK != ret) RETURN_CY_ERR(ret);
	// IPアドレス
	CyString ip_addr;
	ret = cyConfig.GetAddress(ip_addr);
	if ( CY_RESULT_OK != ret) RETURN_CY_ERR(ret);
	// マルチキャストアドレス
	const DWORD mc_len = 256;
	char mc_addr[mc_len];
	ret = cyConfig.GetMulticastAddress(mc_addr, mc_len);
	if ( CY_RESULT_OK != ret) RETURN_CY_ERR(ret);


	////// 本体へ
	return SetupFromXMLFile(filename, mac_addr.c_str(), ip_addr.c_str(), mc_addr, bSaveIpAddr, bIsSlave, nAdapterIndex, nGrabQueSize);
***/
	CyResult ret = CY_RESULT_OK;
	CString		ip;
	CString		mac;
	CString		mc;

	ret = GetXMLFileToIP(filename, &ip, &mac, &mc);
	if ( CY_RESULT_OK != ret) RETURN_CY_ERR(ret);


	return SetupFromXMLFile(filename, mac, ip, mc, bSaveIpAddr, bIsSlave, nAdapterIndex, nGrabQueSize);
}

//------------------------------------------
// iPort設定XMLファイルを使用して、カメラに接続
// const char* filename XMLフルパス
// const char* mac_addr MACアドレス				"00-00" or "00:00"	
// const char* ip_addr  IPアドレス				"xxx.xxx.xxx.xxx"
// char const* mc_addr  マルチキャストアドレス	"xxx.xxx.xxx.xxx"
// bool bSaveIpAddr カメラ本体にIPアドレスを記憶させるか？ (true:XMLに書かれているIPを設定) ※ たぶんfalse時は、IPアドレスでコネクト。true時は、MACアドレスでコネクトすると思われる
//						(カメラ電源OFF→ONで、IPを忘れてしまうため)
// bool bIsSlave SLAVEモードで接続 (true:XMLでマスター指定時でも、強制的にSLAVEになる。false:XMLの指定通り)
// long nAdapterIndex iPort用デバイスマネージャーの何口目を使用するか (0オリジン) (-1:XMLに記述されているアダプターを使用)
// int nGrabQueSize グラバーのリングバッファサイズ
// IportLib::ERRCODE 復帰情報
//------------------------------------------
IportLib::ERRCODE IportBase::SetupFromXMLFile(const char* filename, 
											  const char* mac_addr, const char* ip_addr, char const* mc_addr, 
											  bool bSaveIpAddr, bool bIsSlave, long nAdapterIndex, int nGrabQueSize)
{
	// 念の為
	_ASSERT( NULL != my_pCyGrabber );
	_ASSERT( NULL != my_pCyBuffer );
	_ASSERT( NULL == my_pCyCamera );

	CyResult ret = CY_RESULT_OK;
	my_bIsSlave  = bIsSlave;

	//// XMLファイル読み込み
	CyXMLDocument	cyXmlDoc(filename);
	ret = cyXmlDoc.LoadDocument();
	if( CY_RESULT_OK != ret )	{
		LOG(em_ERR), "[%s] LoadDocument失敗 (%d)", my_sThreadName, ret);
		RETURN_CY_ERR(ret);
	}

	//// XMLファイルから内部設定情報に変換
	CyConfig		cyConfig;
	ret = cyConfig.LoadFromXML(cyXmlDoc);
	if( CY_RESULT_OK != ret )	{
		LOG(em_ERR), "[%s] LoadFromXML失敗 (%d)", my_sThreadName, ret);
		RETURN_CY_ERR(ret);
	}
	cyConfig.SetIndex( 0 );		// 設定情報の何番目を使うか (通常1つしかないから0固定)


	//// アドレス変換
	// MACアドレス
	char mac2[128];
	memset(mac2, 0x00, sizeof(mac2));
	_ASSERT(strlen(mac_addr) < 128);
	for(int ii=0; ii<strlen(mac_addr); ii++) {
		mac2[ii] = (mac_addr[ii] == ':') ? '-' : mac_addr[ii];	// ":" → "-" に変換 
	}
	cyConfig.SetMACAddress(mac2);

	// IPアドレス [xxx.xxx.xxx.xxx]
	CyString str(ip_addr);
	if ('[' != str[0]) {
		str = "[";
		str += ip_addr;
		str += "]";
	}
	cyConfig.SetAddress(str.c_str());

	// マルチキャストアドレス [xxx.xxx.xxx.xxx]
	str = mc_addr;
	if ('[' != str[0]) {
		str = "[";
		str += mc_addr;
		str += "]";
	}
	cyConfig.SetMulticastAddress(str.c_str());



	//// 設定情報を強制的に書き換え
	// SLAVEモード？
	if( bIsSlave && 
			CY_CONFIG_DSM_MULTICAST == cyConfig.GetParameterInt(CY_CONFIG_PARAM_DATA_SENDING_MODE) &&
			cyConfig.GetParameterInt(CY_CONFIG_PARAM_DATA_SENDING_MODE_MASTER) ) 
	{
		// 強制的にslaveにする
		cyConfig.SetParameter(CY_CONFIG_PARAM_DATA_SENDING_MODE_MASTER, FALSE);
	}

	// アダプター指定有り？
	unsigned long mode = cyConfig.GetParameterInt(CY_CONFIG_PARAM_ACCESS_MODE);
    if( 0 <= nAdapterIndex ) {
		CyAdapterID lAdapterID;
		//CyAdapterID::GetIdentifier( CyConfig::MODE_DRV, nAdapterIndex, lAdapterID);
		CyAdapterID::GetIdentifier( mode, nAdapterIndex, lAdapterID);
		cyConfig.SetAdapterID(lAdapterID);
    }
	

	//// デバイス Find
	CyDeviceFinder				cyFinder;
	CyDeviceFinder::DeviceList	cyList;
//	ret = cyFinder.Find( CY_DEVICE_ACCESS_MODE_DRV, cyList, 500, true, NULL, NULL );		// 最大500msブロック
	ret = cyFinder.Find( mode, cyList, 500, true, NULL, NULL );		// 最大500msブロック
	if( CY_RESULT_OK != ret ) {
		LOG(em_ERR), "[%s] Find失敗 (%d)", my_sThreadName, ret);
		RETURN_CY_ERR(ret);
    }
	// 見つかったリストから対象のカメラを検索
	ret = CY_RESULT_NOT_FOUND;
	CyString	cyMacStr;
	cyConfig.GetMACAddress(cyMacStr);
	CyDeviceFinder::DeviceList::const_iterator	cyItr;
	for(cyItr = cyList.begin(); cyItr != cyList.end(); cyItr++ ) {
		if( (*cyItr).mAddressMAC == cyMacStr) {
			ret = CY_RESULT_OK;
			break;
		}
	}
	if( CY_RESULT_OK != ret ) {
		LOG(em_ERR), "[%s] 該当カメラなし (%s)", my_sThreadName, cyMacStr.c_str() );
		RETURN_CY_ERR(ret);
	}

	
	//// iPortに接続
	unsigned long nFrags = bSaveIpAddr ? CyDevice::FLAG_SET_IP_ON_CONNECT : 0;
	for(int ii=0; ii<MC_CONNECT_RETRY_COUNT; ii++) {
		ret = my_pCyGrabber->Connect(cyConfig, nFrags);
		if(CY_RESULT_OK == ret) break;
		Sleep(100);
	}
	if( CY_RESULT_OK != ret ) {
		LOG(em_ERR), "[%s] Connect失敗 (%d)", my_sThreadName, ret);
		RETURN_CY_ERR(ret);
	}


	//// カメラ情報取得
	// カメラ機種を取得
	cyConfig.GetDeviceType(my_cCameraName, sizeof(my_cCameraName));				// コンフィグからカメラ機種を取得
	if( 0 >= strlen(my_cCameraName) ) {
		LOG(em_ERR), "[%s] GetDeviceType失敗 (カメラ機種取得失敗)", my_sThreadName);
		RETURN_CY_ERR(CY_RESULT_NOT_FOUND);
	}
	// カメラ一覧に該当機種があるか？
	CyCameraRegistry	CyReg;
	ret = CyReg.FindCamera(my_cCameraName);
	if( CY_RESULT_OK != ret ) {
		LOG(em_ERR), "[%s] FindCamera失敗 (不明なカメラ機種) (%d:%s)", my_sThreadName, ret, my_cCameraName);
		RETURN_CY_ERR(ret);
	}
	// カメラ情報取得
	ret = CyReg.CreateCamera(&my_pCyCamera, my_pCyGrabber);
	if( CY_RESULT_OK != ret ) {
		LOG(em_ERR), "[%s] CreateCamera失敗 (%d)", my_sThreadName, ret);
		RETURN_CY_ERR(ret);
	}
	_ASSERT( NULL != my_pCyCamera );
	_ASSERT( NULL != my_pCyGrabber );



	//===========================================
	// これ以降は、my_pCyCamera, my_pCyGrabber にインスタンスが割りあっているので return ダメよ


	//// XMLファイルからカメラ設定情報(ゲイン,露光時間等々)をカメラに設定する
	ret = my_pCyCamera->LoadFromXML(cyXmlDoc);
	if( CY_RESULT_OK != ret ) {
		Dispose();
		LOG(em_ERR), "[%s] Camera LoadFromXML失敗 (%d)", my_sThreadName, ret);
		RETURN_CY_ERR(ret);
	}

	
	//// カメラに設定情報を送信 (コミットみたいな感じ)
	ret = my_pCyCamera->UpdateToCamera();
	if( CY_RESULT_OK != ret ) {
		Dispose();
		LOG(em_ERR), "[%s] UpdateToCamera失敗 (%d)", my_sThreadName, ret);
		RETURN_CY_ERR(ret);
	}


	//// my_typDeviceEntry に必要な情報をセット
    //
	CyConfig::AccessMode	wkMode;
	cyConfig.GetAccessMode(&wkMode);
	my_typDevEntry.emMode = ::GetMode(wkMode);
	//
	CyAdapterID				wkAdapterId;
	cyConfig.GetAdapterID(&wkAdapterId);											// ?
	for( int ii=0; ii<6; ii++) my_typDevEntry.adapter_mac[ii] = wkAdapterId.GetIdentifier(ii);
	//
	cyConfig.GetAddress(my_typDevEntry.ip,	   sizeof(my_typDevEntry.ip));	// ?
	cyConfig.GetMACAddress(my_typDevEntry.mac, sizeof(my_typDevEntry.mac));	// ?
	my_pCyGrabber->GetDevice().GetId( &my_typDevEntry.id, &my_typDevEntry.module_id, &my_typDevEntry.smodule_id);
	my_typDevEntry.valid = TRUE;

	
	//// 画像情報に必要な情報をセット
	my_typFrameInfo.nWidth	= ::CalcFrameSizeX(*my_pCyCamera);
	my_typFrameInfo.nHeight	= ::CalcFrameSizeY(*my_pCyCamera);
	::CalcFrameNChannelPixelNBit(*my_pCyCamera, my_typFrameInfo.nNChannel, my_typFrameInfo.nPixelNbit);

	
	//// カメラ画像バッファ に iPort取り込み画像領域を確保
	ret = SetupBuffer(nGrabQueSize);
	if( CY_RESULT_OK != ret ) {
		Dispose();
		LOG(em_ERR), "[%s] %s バッファ確保失敗 (%d)", my_sThreadName, my_typDevEntry, ret);
		RETURN_CY_ERR(ret);
	}
	
	//// 太平拡張 (マスター時のみ)
	if( ! bIsSlave) {
		ret = my_pCyGrabber->GetDevice().ConfigPort(CY_DEVICE_PORT_SERIAL_0, "8N1-9600");
		ret = SendSerialCommand("sbr 9600\r", 1000);
	}

//// ----- テスト
//		SendSerialCommand("sem 4\r");
//		SendSerialCommand("sem 7\r");
//		SendSerialCommand("ssf 35800\r");
/////


	RETURN_CY_ERR(CY_RESULT_OK);
}	


//------------------------------------------
// カメラ画像バッファ に iPort取り込み画像領域を確保
// int nGrabQueSize グラバーのリングバッファサイズ
// IportLib::ERRCODE 復帰情報
//------------------------------------------
IportLib::ERRCODE IportBase::SetupBuffer(int nGrabQueSize)
{
	_ASSERT( NULL != my_pCyCamera );
	_ASSERT( NULL != my_pCyBuffer );

	CyResult		ret;
	CyPixelTypeID	pid;

	//// バッファ作成
	my_pCyCamera->GetPixelType(pid);
	ret = my_pCyBuffer->SetCapacity(my_typFrameInfo.nWidth, my_typFrameInfo.nHeight, pid);
	if( CY_RESULT_OK != ret ) {
		LOG(em_ERR), "[%s] %s SetCapacity失敗 (%d)", my_sThreadName, my_typDevEntry, ret );
		RETURN_CY_ERR(ret);
	}

	// 画像情報取得
	const CyPixelType&	pixel_type  = my_pCyBuffer->GetFormat();
	my_typFrameInfo.nPixelNbit		= pixel_type.GetPixelBitSize();
	if( pixel_type.GetTypeID() & CyPixelType::GRAYSCALE ) {
		my_typFrameInfo.nNChannel = 1;
	} else if( pixel_type.GetTypeID() & CyPixelType::RGB_COLOUR ) {
		my_typFrameInfo.nNChannel = 3;
	} else {
		_ASSERT( FALSE );
	}


	//// バッファ領域を配列化 (配列にしてバッファリングしないと、待ち無しですぐに取り込まないと行けないから・・・)
	if(1 < nGrabQueSize) {
		ret = my_pCyBuffer->SetQueueSize(nGrabQueSize);
		if( CY_RESULT_OK != ret ) {
			LOG(em_ERR), "[%s] %s SetQueueSize失敗 (%d)", my_sThreadName, my_typDevEntry, ret );
			RETURN_CY_ERR(ret);
		}
	}

	//// ここに入れておく？ 
	my_pCyGrabber->EnableBadImagePassthrough(true);		// trueにすると、lenがフレームサイズより小さい場合が生じる。(取込後に、処置が必要)
	if(my_bIsSlave) {
		my_pCyGrabber->IgnoreMissingPackets(true);		// スレーブ接続時は、ロストパケットを送らない
	} else {
		my_pCyGrabber->IgnoreMissingPackets(false);
	}
	RETURN_CY_ERR(ret);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (1)～(4)

//------------------------------------------
// 接続確認
// 復帰情報
//------------------------------------------
bool IportBase::IsConnected() const
{
	return ( NULL == my_pCyGrabber ? false : my_pCyGrabber->GetDevice().IsConnected() );
}

//------------------------------------------
// マスターorスレーブ
// FALSE:master or non  TRUE:multicast
//------------------------------------------
bool IportBase::IsMulticastSlave() const
{
    return ( NULL == my_pCyGrabber ? false : my_pCyGrabber->GetDevice().IsMulticastSlave() );
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (5) grabの制御
//------------------------------------------
// カメラ取込開始
// IportLib::ERRCODE 復帰情報
//------------------------------------------
IportLib::ERRCODE IportBase::StartGrab()
{
	_ASSERT( NULL != my_pCyCamera );
	_ASSERT( NULL != my_pCyBuffer );
	
	// ここで初期化すべきはず??
	my_nResendCount = 0;
	my_nLostCount	= 0;
	my_bLockWaiting = false;

//	CyResult ret = my_pCyGrabber->StartGrabbing(CyChannel(0), *my_pCyBuffer);  // ←なぜ、これじゃダメなの??
	CyResult ret = my_pCyCamera->StartGrabbing(CyChannel(0), *my_pCyBuffer);	// EPCはこっち
//	return NoErr;
	RETURN_CY_ERR(ret);
}
//------------------------------------------
// カメラ取込停止
// IportLib::ERRCODE 復帰情報
//------------------------------------------
IportLib::ERRCODE IportBase::StopGrab()
{
	_ASSERT( NULL != my_pCyCamera );
	_ASSERT( NULL != my_pCyBuffer );

	// 最大 timeout まで block される
//	CyResult ret = my_pCyGrabber->StopGrabbing(CyChannel(0));
	CyResult ret = my_pCyCamera->StopGrabbing(CyChannel(0));			// ?
//	return NoErr;
	RETURN_CY_ERR(ret);
}

//------------------------------------------
// カメラ取込確認
// IportLib::ERRCODE 復帰情報
//------------------------------------------
bool IportBase::IsStarted() const
{
	if( ! IsConnected() ) return false;
	return my_pCyGrabber->IsStarted(CyChannel(0));				// ?
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (6) デバイスの切断
//------------------------------------------
// デバイスの切断
// IportLib::ERRCODE 復帰情報
//------------------------------------------
IportLib::ERRCODE IportBase::Disconnect()
{
	CyResult ret = CY_RESULT_OK;

	if( NULL != my_pCyCamera) {
		delete my_pCyCamera;
		my_pCyCamera = NULL;
	}
	if( NULL != my_pCyGrabber) {
		ret = my_pCyGrabber->Disconnect();
	}
	RETURN_CY_ERR(ret);
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
IportLib::ERRCODE IportBase::LockFrame(void*& p, unsigned long& len, unsigned long& iid, unsigned long& status)
{
	_ASSERT( NULL != my_pCyBuffer );

	my_nResendCount = 0;
	my_nLostCount	= 0;
	iid				= 0;
	status			= 0;

	my_bLockWaiting = true;
	CyResult ret	= my_pCyBuffer->LockForRead(&p, &len, CyBuffer::FLAG_NO_WAIT);
	my_bLockWaiting = false;

	if( CY_RESULT_OK != ret ) RETURN_CY_ERR(ret);


	// 以下は、正常の時のみしかアクセスしたらダメ
	my_nResendCount = my_pCyBuffer->GetReadExpectedResendCount(); 
	my_nLostCount	= my_pCyBuffer->GetReadLostPacketCount(); 

	iid				= my_pCyBuffer->GetReadImageID();
	status			= my_pCyBuffer->GetReadImageStatus(); 

	RETURN_CY_ERR(NoErr);
}

//------------------------------------------
// 画像読み込み領域返却
// IportLib::ERRCODE 復帰情報
//------------------------------------------
void IportBase::ReleaseFrame() const
{
	_ASSERT( NULL != my_pCyBuffer );
	
	my_pCyBuffer->SignalReadEnd();

}

//------------------------------------------
// LockForReadから強制的に抜ける
// IportLib::ERRCODE 復帰情報
//------------------------------------------
void IportBase::AbortLock() const
{
	_ASSERT( NULL != my_pCyBuffer );
	
	my_pCyBuffer->AbortRead();
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (8) ～ (9)

void IportBase::ClearQueueItemInBuffer()
{
    _ASSERT( NULL != my_pCyBuffer );
    my_pCyBuffer->ClearQueue();
}

unsigned long IportBase::GetNQueueItemsInBuffer() const
{
    _ASSERT( NULL != my_pCyBuffer );
    return my_pCyBuffer->GetQueueItemCount();
}


ERRCODE IportBase::ResetModule()
{
    if( !IsMulticastSlave() )
    {
        const CyResult ret = my_pCyGrabber->GetDevice().ResetModule();
        RETURN_CY_ERR(ret);
    }
    return NoErr;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (10) 伝送系

//------------------------------------------
// シルアルコマンド 送信後、アンサー受信
// char const* sSend 送信文字列
// CString* sRecv 受信文字列
// int nAnsWait アンサー待ち時間 [ms] この時間分ロックされる。gcpなどは時間がかかるための対策
// bool bDsp ログ表示 (デフォルト true)
// IportLib::ERRCODE 復帰情報
//------------------------------------------
IportLib::ERRCODE IportBase::SendSerialCommand(char const* sSend, CString* sRecv, int timeout, bool bDsp)
{
	_ASSERT( NULL != my_pCyGrabber );

	CyResult	ret_s, ret_r;
	CyDevice&	dev = my_pCyGrabber->GetDevice();			// デバイス取得
	dev.ResetAnswerQueue(CyDevice::PORT_SERIAL_0);			// 受信バッファを一度クリア

	CString		sWk(sSend);
	ret_s = dev.SendCommand(CyDevice::PORT_SERIAL_0, sWk, sWk.GetLength());		// 送信

	Sleep(timeout);											// アンサーが帰ってくるまでちょっと待機

	unsigned int	nRecvLen = 512;
	LPTSTR ansWk = sRecv->GetBuffer(nRecvLen);
	ret_r = dev.ReceiveAnswer(CyDevice::PORT_SERIAL_0, ansWk, &nRecvLen);
	sRecv->ReleaseBuffer(nRecvLen);							// 受信データサイズのみ

	if(bDsp) {
		LOG(em_INF), "[%s] %s iPortｺﾏﾝﾄﾞ送信 (s=%d,%s)(r=%d,%s)", my_sThreadName, my_typDevEntry.ip, 
			ret_s, sWk, ret_r, *sRecv );	
	}

	if( CY_RESULT_OK != ret_s )										{ RETURN_CY_ERR(ret_s); }
	else if ( CY_RESULT_OK != ret_r && CY_RESULT_TIMEOUT != ret_r ) { RETURN_CY_ERR(ret_r); }
	else															{ RETURN_CY_ERR(CY_RESULT_OK); }
}
//------------------------------------------
// シルアルコマンド 送信
// char const* sSend 送信文字列
// int nAnsWait アンサー待ち時間 [ms] この時間分ロックされる。gcpなどは時間がかかるための対策
// bool bDsp ログ表示 (デフォルト true)
// IportLib::ERRCODE 復帰情報
//------------------------------------------
IportLib::ERRCODE IportBase::SendSerialCommand(char const* sSend, int nAnsWait, bool bDsp)
{
	CString		sRecv;
	return SendSerialCommand(sSend, &sRecv, nAnsWait, bDsp);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// その他小物

