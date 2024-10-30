//======================================================================
//======================================================================
//	API-CAP(W32)
//	CCAPDIO.H					CONTEC.CO.,LTD.
//======================================================================
//======================================================================


//======================================================================
// Error Code
//======================================================================
#define	DIO_ERR_SUCCESS					0		//����I�� 
#define	DIO_ERR_INI_MEMORY				3		//�������̊��蓖�ĂɎ��s���܂����B 
#define	DIO_ERR_INI_REGISTRY			4		//�ݒ�t�@�C���̃A�N�Z�X�Ɏ��s���܂����B 
#define	DIO_ERR_DLL_DEVICE_NAME			10000	//�ݒ�t�@�C���ɓo�^����Ă��Ȃ��f�o�C�X�����w�肳��܂����B 
#define	DIO_ERR_DLL_INVALID_ID			10001	//������ID���w�肳��܂����B 
#define	DIO_ERR_DLL_CREATE_FILE			10003	//�n���h���̎擾�ɍ쐬�Ɏ��s���܂����B 
#define	DIO_ERR_DLL_CLOSE_FILE			10004	//�n���h���̃N���[�Y�Ɏ��s���܂����B 
#define	DIO_ERR_ACCESS_RIGHT			10005	//�A�N�Z�X���G���[�ł��B
#define	DIO_ERR_DLL_TIMEOUT				10006	//�ʐM�^�C���A�E�g���������܂����B
#define	DIO_ERR_COMPOSITION				10007	//�@��\���G���[�ł��B
#define	DIO_ERR_INFO_INVALID_DEVICE		10050	//�w�肵���f�o�C�X���̂�������܂���B�X�y�����m�F���Ă��������B 
#define	DIO_ERR_INFO_NOT_FIND_DEVICE	10051	//���p�\�ȃf�o�C�X��������܂���B 
#define	DIO_ERR_DLL_BUFF_ADDRESS		10100	//�f�[�^�o�b�t�@�A�h���X���s���ł��B 
#define	DIO_ERR_SYS_NOT_SUPPORTED		20001	//���̃f�o�C�X�ł͂��̊֐��͎g�p�ł��܂���B
#define	DIO_ERR_SYS_PORT_NO				20100	//�|�[�g�ԍ����w��\�͈͂𒴂��Ă��܂��B 
#define	DIO_ERR_SYS_PORT_NUM			20101	//�|�[�g�����w��\�͈͂𒴂��Ă��܂��B 
#define	DIO_ERR_SYS_BIT_NO				20102	//�r�b�g�ԍ����w��\�͈͂𒴂��Ă��܂��B 
#define	DIO_ERR_SYS_BIT_NUM				20103	//�r�b�g�����w��\�͈͂𒴂��Ă��܂��B 
#define	DIO_ERR_SYS_BIT_DATA			20104	//�r�b�g�f�[�^��0��1�ȊO�ł��B 
#define	DIO_ERR_SYS_DIRECTION			20106	//���o�͕������w��͈͊O�ł��B(200X6)
#define	DIO_ERR_SYS_FILTER				20400	//�t�B���^���萔���w��͈͊O�ł��B 

//======================================================================
// Function Prototype
//======================================================================
#ifdef __cplusplus
extern"C"{
#endif

long WINAPI	DioInit(char *Devicename, short *Id);
long WINAPI	DioExit(short Id);
long WINAPI	DioGetErrorString(long Err, char *szMsg);
// �����[�gPIO�ŋ����������߃R�����g��
//long WINAPI	DioQueryDeviceName(short Index, short *GroupID, short *UnitID, short *DeviceID, char *DeviceName, char *Device);
long WINAPI	DioSetAccessRight(BYTE *AccsRight);
long WINAPI	DioSetDirectMode(short Id, short DirectMode);
long WINAPI	DioSetIOInterval(long Interval);
long WINAPI	DioGetRemoteStatus(short Id, BYTE *Start, BYTE *Status);
long WINAPI	DioSetDigitalFilter(short Id, short FilterValue);
long WINAPI	DioGetDigitalFilter(short Id, short *FilterValue);
// �����[�gPIO�ŋ����������߃R�����g��
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