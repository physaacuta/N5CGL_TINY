#include "StdAfx.h"
#include "IportBase.h"


// Pleora �w�b�_�[
#include <CyConfig.h>
#include <CyDeviceFinder.h>
#include <CyCameraRegistry.h>
#include <CyCameraInterface.h>
#include <CyGrabber.h>
#include <CyXMLDocument.h>
//#include <CyPerfMonitor.h>


// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


using namespace IportLib;


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���[�J���֐�

//------------------------------------------
// �ڑ����[�h
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
// ���݂�my_pCyCamera�̐ݒ肩�畝�E����[pixel]��Ԃ�
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
// ���݂�my_pCyCamera�̐ݒ肩���f����Ԃ�
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
		nch = 1;	// �ʏ킠�肦�Ȃ��͂�
	}
	nbit = camera.GetGrabber().GetPixelDepth();
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��{

//------------------------------------------
// �R���X�g���N�^
// CString sThreadName ���N���X�̖��O
//------------------------------------------
IportBase::IportBase(CString sThreadName) :
my_pCyGrabber(NULL), my_pCyBuffer(NULL), my_pCyCamera(NULL), 
my_nResendCount(0), my_nLostCount(0), my_bLockWaiting(false),
//my_bIsFreeRun(false), my_nFreeRunHz(0),
my_sThreadName(sThreadName),
mcls_pLog(NULL)
{
	// ���̑�������
	memset(&my_typDevEntry,	0x00, sizeof(my_typDevEntry));
	memset(my_cCameraName,	0x00, sizeof(my_cCameraName));
	memset(&my_typFrameInfo,0x00, sizeof(my_typFrameInfo));

}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
IportBase::~IportBase(void)
{
	Dispose();	// �O�̂��߉��
}

//------------------------------------------
// �֐�������
//------------------------------------------
bool IportBase::Init()
{
	_ASSERT( NULL == my_pCyGrabber );
	_ASSERT( NULL == my_pCyBuffer );
	_ASSERT( NULL == my_pCyCamera );

	//// �C���X�^���X����
    my_pCyGrabber	= new CyGrabber;
    my_pCyBuffer	= new CyImageBuffer;

	LOG(em_MSG), "[%s] Init����", my_sThreadName);
	return true;
}

//------------------------------------------
// �֐��J��
//------------------------------------------
void IportBase::Dispose()
{
	//// ���
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

	// ������
	memset(&my_typDevEntry,	0x00, sizeof(my_typDevEntry));
	memset(my_cCameraName,	0x00, sizeof(my_cCameraName));
	memset(&my_typFrameInfo,0x00, sizeof(my_typFrameInfo));


	// �O�ׂ̈Ɋm�F
	_ASSERT( NULL == my_pCyGrabber );
	_ASSERT( NULL == my_pCyBuffer );
	_ASSERT( NULL == my_pCyCamera );

	LOG(em_MSG), "[%s] Dispose����", my_sThreadName);
}




// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// iPort ����n

//------------------------------------------
// iPort�ݒ�XML�t�@�C������ IP�A�h���X�֌W���擾
// const char* filename XML�t���p�X
// char* ip		IP�A�h���X
// char* mac	MAC�A�h���X
// char* mc		�}���`�L���X�g�A�h���X
// IportLib::ERRCODE ���A���
//------------------------------------------
IportLib::ERRCODE IportBase::GetXMLFileToIP(const char* filename, CString* ip, CString* mac, CString* mc)
{
	CyResult ret = CY_RESULT_OK;

	//// XML�t�@�C���ǂݍ���
	CyXMLDocument	cyXmlDoc(filename);
	ret = cyXmlDoc.LoadDocument();
	if( CY_RESULT_OK != ret )	{
		RETURN_CY_ERR(ret);
	}

	//// XML�t�@�C����������ݒ���ɕϊ�
	CyConfig		cyConfig;
	ret = cyConfig.LoadFromXML(cyXmlDoc);
	if( CY_RESULT_OK != ret )	{
		RETURN_CY_ERR(ret);
	}
	cyConfig.SetIndex( 0 );		// �ݒ���̉��Ԗڂ��g���� (�ʏ�1�����Ȃ�����0�Œ�)

	//// XML�t�@�C������A�h���X�擾
	// MAC�A�h���X
	CyString mac_addr;
	ret = cyConfig.GetMACAddress(mac_addr);
	if ( CY_RESULT_OK != ret) RETURN_CY_ERR(ret);
	// IP�A�h���X
	CyString ip_addr;
	ret = cyConfig.GetAddress(ip_addr);
	if ( CY_RESULT_OK != ret) RETURN_CY_ERR(ret);
	// �}���`�L���X�g�A�h���X
	const DWORD mc_len = 256;
	char mc_addr[mc_len];
	ret = cyConfig.GetMulticastAddress(mc_addr, 256);
	if ( CY_RESULT_OK != ret) RETURN_CY_ERR(ret);

	// �ԋp
	*mac = mac_addr.c_str();
	*ip = ip_addr.c_str();
	*mc = mc_addr;


	RETURN_CY_ERR(CY_RESULT_OK);
}

//------------------------------------------
// iPort�ݒ�XML�t�@�C�����g�p���āA�J�����ɐڑ�
// const char* filename XML�t���p�X
// bool bSaveIpAddr �J�����{�̂�IP�A�h���X���L�������邩�H (true:XML�ɏ�����Ă���IP��ݒ�) �� ���Ԃ�false���́AIP�A�h���X�ŃR�l�N�g�Btrue���́AMAC�A�h���X�ŃR�l�N�g����Ǝv����
//						(�J�����d��OFF��ON�ŁAIP��Y��Ă��܂�����)
// bool bIsSlave SLAVE���[�h�Őڑ� (true:XML�Ń}�X�^�[�w�莞�ł��A�����I��SLAVE�ɂȂ�Bfalse:XML�̎w��ʂ�)
// long nAdapterIndex iPort�p�f�o�C�X�}�l�[�W���[�̉����ڂ��g�p���邩 (0�I���W��) (-1:XML�ɋL�q����Ă���A�_�v�^�[���g�p)
        // adapter_index = -1�̏ꍇ�̓t�@�C������adapter_id�����̂܂ܗp����B
        // adapter_index >= 0�̏ꍇ�́ACyAdapterID::GetIdentifier( CyConfig::MODE_DRV, adapter_index, lAdapterID ); �Ŏ擾����lAdapterID��p����
// int nGrabQueSize �O���o�[�̃����O�o�b�t�@�T�C�Y
// IportLib::ERRCODE ���A���
//------------------------------------------
IportLib::ERRCODE IportBase::SetupFromXMLFile(const char* filename, 
											  bool bSaveIpAddr, bool bIsSlave, long nAdapterIndex, int nGrabQueSize)
{
/***
	CyResult ret = CY_RESULT_OK;
	my_bIsSlave  = bIsSlave;

	//// XML�t�@�C���ǂݍ���
	CyXMLDocument	cyXmlDoc(filename);
	ret = cyXmlDoc.LoadDocument();
	if( CY_RESULT_OK != ret )	{
		LOG(em_ERR), "[%s] LoadDocument���s (%d)", my_sThreadName, ret);
		RETURN_CY_ERR(ret);
	}

	//// XML�t�@�C����������ݒ���ɕϊ�
	CyConfig		cyConfig;
	ret = cyConfig.LoadFromXML(cyXmlDoc);
	if( CY_RESULT_OK != ret )	{
		LOG(em_ERR), "[%s] LoadFromXML���s (%d)", my_sThreadName, ret);
		RETURN_CY_ERR(ret);
	}
	cyConfig.SetIndex( 0 );		// �ݒ���̉��Ԗڂ��g���� (�ʏ�1�����Ȃ�����0�Œ�)



	//// XML�t�@�C������A�h���X�擾
	// MAC�A�h���X
	CyString mac_addr;
	ret = cyConfig.GetMACAddress(mac_addr);
	if ( CY_RESULT_OK != ret) RETURN_CY_ERR(ret);
	// IP�A�h���X
	CyString ip_addr;
	ret = cyConfig.GetAddress(ip_addr);
	if ( CY_RESULT_OK != ret) RETURN_CY_ERR(ret);
	// �}���`�L���X�g�A�h���X
	const DWORD mc_len = 256;
	char mc_addr[mc_len];
	ret = cyConfig.GetMulticastAddress(mc_addr, mc_len);
	if ( CY_RESULT_OK != ret) RETURN_CY_ERR(ret);


	////// �{�̂�
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
// iPort�ݒ�XML�t�@�C�����g�p���āA�J�����ɐڑ�
// const char* filename XML�t���p�X
// const char* mac_addr MAC�A�h���X				"00-00" or "00:00"	
// const char* ip_addr  IP�A�h���X				"xxx.xxx.xxx.xxx"
// char const* mc_addr  �}���`�L���X�g�A�h���X	"xxx.xxx.xxx.xxx"
// bool bSaveIpAddr �J�����{�̂�IP�A�h���X���L�������邩�H (true:XML�ɏ�����Ă���IP��ݒ�) �� ���Ԃ�false���́AIP�A�h���X�ŃR�l�N�g�Btrue���́AMAC�A�h���X�ŃR�l�N�g����Ǝv����
//						(�J�����d��OFF��ON�ŁAIP��Y��Ă��܂�����)
// bool bIsSlave SLAVE���[�h�Őڑ� (true:XML�Ń}�X�^�[�w�莞�ł��A�����I��SLAVE�ɂȂ�Bfalse:XML�̎w��ʂ�)
// long nAdapterIndex iPort�p�f�o�C�X�}�l�[�W���[�̉����ڂ��g�p���邩 (0�I���W��) (-1:XML�ɋL�q����Ă���A�_�v�^�[���g�p)
// int nGrabQueSize �O���o�[�̃����O�o�b�t�@�T�C�Y
// IportLib::ERRCODE ���A���
//------------------------------------------
IportLib::ERRCODE IportBase::SetupFromXMLFile(const char* filename, 
											  const char* mac_addr, const char* ip_addr, char const* mc_addr, 
											  bool bSaveIpAddr, bool bIsSlave, long nAdapterIndex, int nGrabQueSize)
{
	// �O�̈�
	_ASSERT( NULL != my_pCyGrabber );
	_ASSERT( NULL != my_pCyBuffer );
	_ASSERT( NULL == my_pCyCamera );

	CyResult ret = CY_RESULT_OK;
	my_bIsSlave  = bIsSlave;

	//// XML�t�@�C���ǂݍ���
	CyXMLDocument	cyXmlDoc(filename);
	ret = cyXmlDoc.LoadDocument();
	if( CY_RESULT_OK != ret )	{
		LOG(em_ERR), "[%s] LoadDocument���s (%d)", my_sThreadName, ret);
		RETURN_CY_ERR(ret);
	}

	//// XML�t�@�C����������ݒ���ɕϊ�
	CyConfig		cyConfig;
	ret = cyConfig.LoadFromXML(cyXmlDoc);
	if( CY_RESULT_OK != ret )	{
		LOG(em_ERR), "[%s] LoadFromXML���s (%d)", my_sThreadName, ret);
		RETURN_CY_ERR(ret);
	}
	cyConfig.SetIndex( 0 );		// �ݒ���̉��Ԗڂ��g���� (�ʏ�1�����Ȃ�����0�Œ�)


	//// �A�h���X�ϊ�
	// MAC�A�h���X
	char mac2[128];
	memset(mac2, 0x00, sizeof(mac2));
	_ASSERT(strlen(mac_addr) < 128);
	for(int ii=0; ii<strlen(mac_addr); ii++) {
		mac2[ii] = (mac_addr[ii] == ':') ? '-' : mac_addr[ii];	// ":" �� "-" �ɕϊ� 
	}
	cyConfig.SetMACAddress(mac2);

	// IP�A�h���X [xxx.xxx.xxx.xxx]
	CyString str(ip_addr);
	if ('[' != str[0]) {
		str = "[";
		str += ip_addr;
		str += "]";
	}
	cyConfig.SetAddress(str.c_str());

	// �}���`�L���X�g�A�h���X [xxx.xxx.xxx.xxx]
	str = mc_addr;
	if ('[' != str[0]) {
		str = "[";
		str += mc_addr;
		str += "]";
	}
	cyConfig.SetMulticastAddress(str.c_str());



	//// �ݒ���������I�ɏ�������
	// SLAVE���[�h�H
	if( bIsSlave && 
			CY_CONFIG_DSM_MULTICAST == cyConfig.GetParameterInt(CY_CONFIG_PARAM_DATA_SENDING_MODE) &&
			cyConfig.GetParameterInt(CY_CONFIG_PARAM_DATA_SENDING_MODE_MASTER) ) 
	{
		// �����I��slave�ɂ���
		cyConfig.SetParameter(CY_CONFIG_PARAM_DATA_SENDING_MODE_MASTER, FALSE);
	}

	// �A�_�v�^�[�w��L��H
	unsigned long mode = cyConfig.GetParameterInt(CY_CONFIG_PARAM_ACCESS_MODE);
    if( 0 <= nAdapterIndex ) {
		CyAdapterID lAdapterID;
		//CyAdapterID::GetIdentifier( CyConfig::MODE_DRV, nAdapterIndex, lAdapterID);
		CyAdapterID::GetIdentifier( mode, nAdapterIndex, lAdapterID);
		cyConfig.SetAdapterID(lAdapterID);
    }
	

	//// �f�o�C�X Find
	CyDeviceFinder				cyFinder;
	CyDeviceFinder::DeviceList	cyList;
//	ret = cyFinder.Find( CY_DEVICE_ACCESS_MODE_DRV, cyList, 500, true, NULL, NULL );		// �ő�500ms�u���b�N
	ret = cyFinder.Find( mode, cyList, 500, true, NULL, NULL );		// �ő�500ms�u���b�N
	if( CY_RESULT_OK != ret ) {
		LOG(em_ERR), "[%s] Find���s (%d)", my_sThreadName, ret);
		RETURN_CY_ERR(ret);
    }
	// �����������X�g����Ώۂ̃J����������
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
		LOG(em_ERR), "[%s] �Y���J�����Ȃ� (%s)", my_sThreadName, cyMacStr.c_str() );
		RETURN_CY_ERR(ret);
	}

	
	//// iPort�ɐڑ�
	unsigned long nFrags = bSaveIpAddr ? CyDevice::FLAG_SET_IP_ON_CONNECT : 0;
	for(int ii=0; ii<MC_CONNECT_RETRY_COUNT; ii++) {
		ret = my_pCyGrabber->Connect(cyConfig, nFrags);
		if(CY_RESULT_OK == ret) break;
		Sleep(100);
	}
	if( CY_RESULT_OK != ret ) {
		LOG(em_ERR), "[%s] Connect���s (%d)", my_sThreadName, ret);
		RETURN_CY_ERR(ret);
	}


	//// �J�������擾
	// �J�����@����擾
	cyConfig.GetDeviceType(my_cCameraName, sizeof(my_cCameraName));				// �R���t�B�O����J�����@����擾
	if( 0 >= strlen(my_cCameraName) ) {
		LOG(em_ERR), "[%s] GetDeviceType���s (�J�����@��擾���s)", my_sThreadName);
		RETURN_CY_ERR(CY_RESULT_NOT_FOUND);
	}
	// �J�����ꗗ�ɊY���@�킪���邩�H
	CyCameraRegistry	CyReg;
	ret = CyReg.FindCamera(my_cCameraName);
	if( CY_RESULT_OK != ret ) {
		LOG(em_ERR), "[%s] FindCamera���s (�s���ȃJ�����@��) (%d:%s)", my_sThreadName, ret, my_cCameraName);
		RETURN_CY_ERR(ret);
	}
	// �J�������擾
	ret = CyReg.CreateCamera(&my_pCyCamera, my_pCyGrabber);
	if( CY_RESULT_OK != ret ) {
		LOG(em_ERR), "[%s] CreateCamera���s (%d)", my_sThreadName, ret);
		RETURN_CY_ERR(ret);
	}
	_ASSERT( NULL != my_pCyCamera );
	_ASSERT( NULL != my_pCyGrabber );



	//===========================================
	// ����ȍ~�́Amy_pCyCamera, my_pCyGrabber �ɃC���X�^���X�����肠���Ă���̂� return �_����


	//// XML�t�@�C������J�����ݒ���(�Q�C��,�I�����ԓ��X)���J�����ɐݒ肷��
	ret = my_pCyCamera->LoadFromXML(cyXmlDoc);
	if( CY_RESULT_OK != ret ) {
		Dispose();
		LOG(em_ERR), "[%s] Camera LoadFromXML���s (%d)", my_sThreadName, ret);
		RETURN_CY_ERR(ret);
	}

	
	//// �J�����ɐݒ���𑗐M (�R�~�b�g�݂����Ȋ���)
	ret = my_pCyCamera->UpdateToCamera();
	if( CY_RESULT_OK != ret ) {
		Dispose();
		LOG(em_ERR), "[%s] UpdateToCamera���s (%d)", my_sThreadName, ret);
		RETURN_CY_ERR(ret);
	}


	//// my_typDeviceEntry �ɕK�v�ȏ����Z�b�g
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

	
	//// �摜���ɕK�v�ȏ����Z�b�g
	my_typFrameInfo.nWidth	= ::CalcFrameSizeX(*my_pCyCamera);
	my_typFrameInfo.nHeight	= ::CalcFrameSizeY(*my_pCyCamera);
	::CalcFrameNChannelPixelNBit(*my_pCyCamera, my_typFrameInfo.nNChannel, my_typFrameInfo.nPixelNbit);

	
	//// �J�����摜�o�b�t�@ �� iPort��荞�݉摜�̈���m��
	ret = SetupBuffer(nGrabQueSize);
	if( CY_RESULT_OK != ret ) {
		Dispose();
		LOG(em_ERR), "[%s] %s �o�b�t�@�m�ێ��s (%d)", my_sThreadName, my_typDevEntry, ret);
		RETURN_CY_ERR(ret);
	}
	
	//// �����g�� (�}�X�^�[���̂�)
	if( ! bIsSlave) {
		ret = my_pCyGrabber->GetDevice().ConfigPort(CY_DEVICE_PORT_SERIAL_0, "8N1-9600");
		ret = SendSerialCommand("sbr 9600\r", 1000);
	}

//// ----- �e�X�g
//		SendSerialCommand("sem 4\r");
//		SendSerialCommand("sem 7\r");
//		SendSerialCommand("ssf 35800\r");
/////


	RETURN_CY_ERR(CY_RESULT_OK);
}	


//------------------------------------------
// �J�����摜�o�b�t�@ �� iPort��荞�݉摜�̈���m��
// int nGrabQueSize �O���o�[�̃����O�o�b�t�@�T�C�Y
// IportLib::ERRCODE ���A���
//------------------------------------------
IportLib::ERRCODE IportBase::SetupBuffer(int nGrabQueSize)
{
	_ASSERT( NULL != my_pCyCamera );
	_ASSERT( NULL != my_pCyBuffer );

	CyResult		ret;
	CyPixelTypeID	pid;

	//// �o�b�t�@�쐬
	my_pCyCamera->GetPixelType(pid);
	ret = my_pCyBuffer->SetCapacity(my_typFrameInfo.nWidth, my_typFrameInfo.nHeight, pid);
	if( CY_RESULT_OK != ret ) {
		LOG(em_ERR), "[%s] %s SetCapacity���s (%d)", my_sThreadName, my_typDevEntry, ret );
		RETURN_CY_ERR(ret);
	}

	// �摜���擾
	const CyPixelType&	pixel_type  = my_pCyBuffer->GetFormat();
	my_typFrameInfo.nPixelNbit		= pixel_type.GetPixelBitSize();
	if( pixel_type.GetTypeID() & CyPixelType::GRAYSCALE ) {
		my_typFrameInfo.nNChannel = 1;
	} else if( pixel_type.GetTypeID() & CyPixelType::RGB_COLOUR ) {
		my_typFrameInfo.nNChannel = 3;
	} else {
		_ASSERT( FALSE );
	}


	//// �o�b�t�@�̈��z�� (�z��ɂ��ăo�b�t�@�����O���Ȃ��ƁA�҂������ł����Ɏ�荞�܂Ȃ��ƍs���Ȃ�����E�E�E)
	if(1 < nGrabQueSize) {
		ret = my_pCyBuffer->SetQueueSize(nGrabQueSize);
		if( CY_RESULT_OK != ret ) {
			LOG(em_ERR), "[%s] %s SetQueueSize���s (%d)", my_sThreadName, my_typDevEntry, ret );
			RETURN_CY_ERR(ret);
		}
	}

	//// �����ɓ���Ă����H 
	my_pCyGrabber->EnableBadImagePassthrough(true);		// true�ɂ���ƁAlen���t���[���T�C�Y��菬�����ꍇ��������B(�捞��ɁA���u���K�v)
	if(my_bIsSlave) {
		my_pCyGrabber->IgnoreMissingPackets(true);		// �X���[�u�ڑ����́A���X�g�p�P�b�g�𑗂�Ȃ�
	} else {
		my_pCyGrabber->IgnoreMissingPackets(false);
	}
	RETURN_CY_ERR(ret);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (1)�`(4)

//------------------------------------------
// �ڑ��m�F
// ���A���
//------------------------------------------
bool IportBase::IsConnected() const
{
	return ( NULL == my_pCyGrabber ? false : my_pCyGrabber->GetDevice().IsConnected() );
}

//------------------------------------------
// �}�X�^�[or�X���[�u
// FALSE:master or non  TRUE:multicast
//------------------------------------------
bool IportBase::IsMulticastSlave() const
{
    return ( NULL == my_pCyGrabber ? false : my_pCyGrabber->GetDevice().IsMulticastSlave() );
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (5) grab�̐���
//------------------------------------------
// �J�����捞�J�n
// IportLib::ERRCODE ���A���
//------------------------------------------
IportLib::ERRCODE IportBase::StartGrab()
{
	_ASSERT( NULL != my_pCyCamera );
	_ASSERT( NULL != my_pCyBuffer );
	
	// �����ŏ��������ׂ��͂�??
	my_nResendCount = 0;
	my_nLostCount	= 0;
	my_bLockWaiting = false;

//	CyResult ret = my_pCyGrabber->StartGrabbing(CyChannel(0), *my_pCyBuffer);  // ���Ȃ��A���ꂶ��_���Ȃ�??
	CyResult ret = my_pCyCamera->StartGrabbing(CyChannel(0), *my_pCyBuffer);	// EPC�͂�����
//	return NoErr;
	RETURN_CY_ERR(ret);
}
//------------------------------------------
// �J�����捞��~
// IportLib::ERRCODE ���A���
//------------------------------------------
IportLib::ERRCODE IportBase::StopGrab()
{
	_ASSERT( NULL != my_pCyCamera );
	_ASSERT( NULL != my_pCyBuffer );

	// �ő� timeout �܂� block �����
//	CyResult ret = my_pCyGrabber->StopGrabbing(CyChannel(0));
	CyResult ret = my_pCyCamera->StopGrabbing(CyChannel(0));			// ?
//	return NoErr;
	RETURN_CY_ERR(ret);
}

//------------------------------------------
// �J�����捞�m�F
// IportLib::ERRCODE ���A���
//------------------------------------------
bool IportBase::IsStarted() const
{
	if( ! IsConnected() ) return false;
	return my_pCyGrabber->IsStarted(CyChannel(0));				// ?
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (6) �f�o�C�X�̐ؒf
//------------------------------------------
// �f�o�C�X�̐ؒf
// IportLib::ERRCODE ���A���
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
// (7) grab�����摜�̎�舵��
//------------------------------------------
// �摜�ǂݍ���(�摜��荞�݊����܂Ńu���b�L���O)
// void*& p �摜�i�[�|�C���^
// unsigned long& len �擾�摜�T�C�Y
// unsigned long& iid iPort�V�[�P���XNo (0�I���W��)
// unsigned long& status �X�e�[�^�X
// IportLib::ERRCODE ���A���
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


	// �ȉ��́A����̎��݂̂����A�N�Z�X������_��
	my_nResendCount = my_pCyBuffer->GetReadExpectedResendCount(); 
	my_nLostCount	= my_pCyBuffer->GetReadLostPacketCount(); 

	iid				= my_pCyBuffer->GetReadImageID();
	status			= my_pCyBuffer->GetReadImageStatus(); 

	RETURN_CY_ERR(NoErr);
}

//------------------------------------------
// �摜�ǂݍ��ݗ̈�ԋp
// IportLib::ERRCODE ���A���
//------------------------------------------
void IportBase::ReleaseFrame() const
{
	_ASSERT( NULL != my_pCyBuffer );
	
	my_pCyBuffer->SignalReadEnd();

}

//------------------------------------------
// LockForRead���狭���I�ɔ�����
// IportLib::ERRCODE ���A���
//------------------------------------------
void IportBase::AbortLock() const
{
	_ASSERT( NULL != my_pCyBuffer );
	
	my_pCyBuffer->AbortRead();
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// (8) �` (9)

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
// (10) �`���n

//------------------------------------------
// �V���A���R�}���h ���M��A�A���T�[��M
// char const* sSend ���M������
// CString* sRecv ��M������
// int nAnsWait �A���T�[�҂����� [ms] ���̎��ԕ����b�N�����Bgcp�Ȃǂ͎��Ԃ������邽�߂̑΍�
// bool bDsp ���O�\�� (�f�t�H���g true)
// IportLib::ERRCODE ���A���
//------------------------------------------
IportLib::ERRCODE IportBase::SendSerialCommand(char const* sSend, CString* sRecv, int timeout, bool bDsp)
{
	_ASSERT( NULL != my_pCyGrabber );

	CyResult	ret_s, ret_r;
	CyDevice&	dev = my_pCyGrabber->GetDevice();			// �f�o�C�X�擾
	dev.ResetAnswerQueue(CyDevice::PORT_SERIAL_0);			// ��M�o�b�t�@����x�N���A

	CString		sWk(sSend);
	ret_s = dev.SendCommand(CyDevice::PORT_SERIAL_0, sWk, sWk.GetLength());		// ���M

	Sleep(timeout);											// �A���T�[���A���Ă���܂ł�����Ƒҋ@

	unsigned int	nRecvLen = 512;
	LPTSTR ansWk = sRecv->GetBuffer(nRecvLen);
	ret_r = dev.ReceiveAnswer(CyDevice::PORT_SERIAL_0, ansWk, &nRecvLen);
	sRecv->ReleaseBuffer(nRecvLen);							// ��M�f�[�^�T�C�Y�̂�

	if(bDsp) {
		LOG(em_INF), "[%s] %s iPort����ޑ��M (s=%d,%s)(r=%d,%s)", my_sThreadName, my_typDevEntry.ip, 
			ret_s, sWk, ret_r, *sRecv );	
	}

	if( CY_RESULT_OK != ret_s )										{ RETURN_CY_ERR(ret_s); }
	else if ( CY_RESULT_OK != ret_r && CY_RESULT_TIMEOUT != ret_r ) { RETURN_CY_ERR(ret_r); }
	else															{ RETURN_CY_ERR(CY_RESULT_OK); }
}
//------------------------------------------
// �V���A���R�}���h ���M
// char const* sSend ���M������
// int nAnsWait �A���T�[�҂����� [ms] ���̎��ԕ����b�N�����Bgcp�Ȃǂ͎��Ԃ������邽�߂̑΍�
// bool bDsp ���O�\�� (�f�t�H���g true)
// IportLib::ERRCODE ���A���
//------------------------------------------
IportLib::ERRCODE IportBase::SendSerialCommand(char const* sSend, int nAnsWait, bool bDsp)
{
	CString		sRecv;
	return SendSerialCommand(sSend, &sRecv, nAnsWait, bDsp);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���̑�����

