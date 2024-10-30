//======================================================================
//======================================================================
//	API-CAP(W32)
//	CCAPDIO.H					CONTEC.CO.,LTD.
//======================================================================
//======================================================================


//======================================================================
// Error Code
//======================================================================
#define	DIO_ERR_SUCCESS					0		//正常終了 
#define	DIO_ERR_INI_MEMORY				3		//メモリの割り当てに失敗しました。 
#define	DIO_ERR_INI_REGISTRY			4		//設定ファイルのアクセスに失敗しました。 
#define	DIO_ERR_DLL_DEVICE_NAME			10000	//設定ファイルに登録されていないデバイス名が指定されました。 
#define	DIO_ERR_DLL_INVALID_ID			10001	//無効なIDが指定されました。 
#define	DIO_ERR_DLL_CREATE_FILE			10003	//ハンドルの取得に作成に失敗しました。 
#define	DIO_ERR_DLL_CLOSE_FILE			10004	//ハンドルのクローズに失敗しました。 
#define	DIO_ERR_ACCESS_RIGHT			10005	//アクセス権エラーです。
#define	DIO_ERR_DLL_TIMEOUT				10006	//通信タイムアウトが発生しました。
#define	DIO_ERR_COMPOSITION				10007	//機器構成エラーです。
#define	DIO_ERR_INFO_INVALID_DEVICE		10050	//指定したデバイス名称が見つかりません。スペルを確認してください。 
#define	DIO_ERR_INFO_NOT_FIND_DEVICE	10051	//利用可能なデバイスが見つかりません。 
#define	DIO_ERR_DLL_BUFF_ADDRESS		10100	//データバッファアドレスが不正です。 
#define	DIO_ERR_SYS_NOT_SUPPORTED		20001	//このデバイスではこの関数は使用できません。
#define	DIO_ERR_SYS_PORT_NO				20100	//ポート番号が指定可能範囲を超えています。 
#define	DIO_ERR_SYS_PORT_NUM			20101	//ポート数が指定可能範囲を超えています。 
#define	DIO_ERR_SYS_BIT_NO				20102	//ビット番号が指定可能範囲を超えています。 
#define	DIO_ERR_SYS_BIT_NUM				20103	//ビット数が指定可能範囲を超えています。 
#define	DIO_ERR_SYS_BIT_DATA			20104	//ビットデータが0か1以外です。 
#define	DIO_ERR_SYS_DIRECTION			20106	//入出力方向が指定範囲外です。(200X6)
#define	DIO_ERR_SYS_FILTER				20400	//フィルタ時定数が指定範囲外です。 

//======================================================================
// Function Prototype
//======================================================================
#ifdef __cplusplus
extern"C"{
#endif

long WINAPI	DioInit(char *Devicename, short *Id);
long WINAPI	DioExit(short Id);
long WINAPI	DioGetErrorString(long Err, char *szMsg);
// リモートPIOで競合したためコメント化
//long WINAPI	DioQueryDeviceName(short Index, short *GroupID, short *UnitID, short *DeviceID, char *DeviceName, char *Device);
long WINAPI	DioSetAccessRight(BYTE *AccsRight);
long WINAPI	DioSetDirectMode(short Id, short DirectMode);
long WINAPI	DioSetIOInterval(long Interval);
long WINAPI	DioGetRemoteStatus(short Id, BYTE *Start, BYTE *Status);
long WINAPI	DioSetDigitalFilter(short Id, short FilterValue);
long WINAPI	DioGetDigitalFilter(short Id, short *FilterValue);
// リモートPIOで競合したためコメント化
//long WINAPI	DioSetIoDirection(short Id, long Dir);
//long WINAPI	DioGetIoDirection(short Id, long *Dir);
long WINAPI	DioInpByte(short Id, short PortNo, BYTE *Data);
long WINAPI	DioInpBit(short Id, short BitNo, BYTE *Data);
long WINAPI	DioOutByte(short Id, short PortNo, BYTE Data);
long WINAPI	DioOutBit(short Id, short BitNo, BYTE Data);
long WINAPI	DioEchoBackByte(short Id, short PortNo, BYTE *Data);
long WINAPI	DioEchoBackBit(short Id, short BitNo, BYTE *Data);
long WINAPI	DioInpMultiByte(short Id, short *PortNo, short PortNum, BYTE *Data);
long WINAPI	DioInpMultiBit(short Id, short *BitNo, short BitNum, BYTE *Data);
long WINAPI	DioOutMultiByte(short Id, short *PortNo, short PortNum, BYTE *Data);
long WINAPI	DioOutMultiBit(short Id, short *BitNo, short BitNum, BYTE *Data);
long WINAPI	DioEchoBackMultiByte(short Id, short *PortNo, short PortNum, BYTE *Data);
long WINAPI	DioEchoBackMultiBit(short Id, short *BitNo, short BitNum, BYTE *Data);
long WINAPI	DioGetMaxPorts(short Id, short *InPortNum, short *OutPortNum);
long WINAPI	DioGetMaxBits(short Id, short *InBitNum, short *OutBitNum);

#ifdef __cplusplus
}
#endif