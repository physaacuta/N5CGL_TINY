/*******************************************************

	API-DIO(PC/98)NT, W95
	FILE NAME	APIDIO.H
	DATE,VERSION	1996.01.30	Ver 2.20
	DATE,VERSION	1998.11.20	Ver 2.50
	DATE,VERSION	1998.12.23	Ver 2.51
	DATE,VERSION	1999.02.23	Ver 3.00
	DATE,VERSION	1999.06.25	Ver 3.10
	DATE,VERSION	2002.11.01	Ver 4.20

********************************************************/

#pragma pack(push, 4)

typedef struct  _DINP {
        LPWORD  InpPortNo;
        WORD    PortNum;
        LPBYTE  Buf;
} DINP;

typedef struct  _DINPB {
        LPWORD  InpPortNo;
        WORD    PortNum;
        WORD    DataNum;
        LPBYTE  Buf;
        WORD    Tim;
} DINPB;

typedef struct  _DBINP {
        LPWORD  InpBitNo;
        WORD    BitNum;
        LPBYTE  Buf;
} DBINP;

typedef struct  _DBINPB {
        LPWORD  InpBitNo;
        WORD    BitNum;
        WORD    DataNum;
        LPBYTE  Buf;
        WORD    Tim;
} DBINPB;

typedef struct  _DOUT {
        LPWORD  OutPortNo;
        WORD    PortNum;
        LPBYTE  Buf;
} DOUT;

typedef struct  _DOUTB {
        LPWORD  OutPortNo;
        WORD    PortNum;
        WORD    DataNum;
        LPBYTE  Buf;
        WORD    Tim;
} DOUTB;

typedef struct  _DBOUT {
        LPWORD  OutBitNo;
        WORD    BitNum;
        LPBYTE  Buf;
} DBOUT;

typedef struct  _DBOUTB {
        LPWORD  OutBitNo;
        WORD    BitNum;
        WORD    DataNum;
        LPBYTE  Buf;
        WORD    Tim;
} DBOUTB;

typedef struct  _DIBCD {
        WORD    InpPortNo;
        DWORD   Figure;
        LPDWORD Value;
} DIBCD;

typedef struct  _DOBCD {
        WORD    OutPortNo;
        DWORD   Figure;
        DWORD   Value;
} DOBCD;

typedef struct  _DPTNSET {
        LPWORD  InpPortNo;
        WORD    PortNum;
        LPDWORD StsBuf;
        LONG    StsBufNum;
        WORD    Tim;
} DPTNSET;

typedef struct  _DTRGSET {
        LPWORD  InpBitNo;
        WORD    BitNum;
        LPBYTE  Buf;
        LPWORD  TrgBitNo;
        WORD    TrgBitNum;
        LPBYTE  TrgKind;
        WORD    Tim;
} DTRGSET;

#pragma pack(pop)

/*********************************************
  API-DIO(PC/98)NT, API-DIO(PC/98)W95 FUNCTION
*********************************************/
#ifdef __cplusplus
extern"C"{
#endif

DWORD	FAR	PASCAL	DioOpen(LPHANDLE, WORD, WORD);
DWORD	FAR	PASCAL	DioOpenEx(char *, LPHANDLE);
DWORD	FAR	PASCAL	DioWait(HANDLE, WORD);
DWORD	FAR	PASCAL	DioWaitEx(HANDLE, DWORD);
DWORD	FAR	PASCAL	DioSFilter(HANDLE, WORD, WORD);
DWORD	FAR	PASCAL	Dio8255Mode(HANDLE, WORD, WORD);
DWORD	FAR	PASCAL	DioInp(HANDLE, DINP FAR *);
DWORD	FAR	PASCAL	DioInpBack(HANDLE, DINPB FAR *, HANDLE, WORD, HANDLE, WORD);
DWORD	FAR	PASCAL	DioBitInp(HANDLE, DBINP FAR *);
DWORD	FAR	PASCAL	DioBitInpBack(HANDLE, DBINPB FAR *, HANDLE, WORD, HANDLE, WORD);
DWORD	FAR	PASCAL	DioOut(HANDLE, DOUT FAR *);
DWORD	FAR	PASCAL	DioOutBack(HANDLE, DOUTB FAR *, HANDLE, WORD, HANDLE, WORD);
DWORD	FAR	PASCAL	DioBitOut(HANDLE, DBOUT FAR *);
DWORD	FAR	PASCAL	DioBitOutBack(HANDLE, DBOUTB FAR *, HANDLE, WORD, HANDLE, WORD);
DWORD	FAR	PASCAL	DioEchoBack(HANDLE, DINP FAR *);
DWORD	FAR	PASCAL	DioBitEchoBack(HANDLE, DBINP FAR *);
DWORD	FAR	PASCAL	DioPtnSet(HANDLE, DPTNSET FAR *);
DWORD	FAR	PASCAL	DioPtnStart(HANDLE, HANDLE, WORD);
DWORD	FAR	PASCAL	DioPtnSts(HANDLE, LPDWORD);
DWORD	FAR	PASCAL	DioTrgSet(HANDLE, DTRGSET FAR *);
DWORD	FAR	PASCAL	DioTrgStart(HANDLE, HANDLE, WORD);
DWORD	FAR	PASCAL	DioTrgSts(HANDLE, LPDWORD);
DWORD	FAR	PASCAL	DioInpBCD(HANDLE, DIBCD FAR *);
DWORD	FAR	PASCAL	DioOutBCD(HANDLE, DOBCD FAR *);
DWORD	FAR	PASCAL	DioNInpBCD(HANDLE, DIBCD FAR *);
DWORD	FAR	PASCAL	DioNOutBCD(HANDLE, DOBCD FAR *);
DWORD	FAR	PASCAL	DioEvent(HANDLE, WORD, WORD, HANDLE, WORD);
DWORD	FAR	PASCAL	DioIntSence(HANDLE, LPWORD);
DWORD	FAR	PASCAL	DioIntEnable(HANDLE, WORD);
DWORD	FAR	PASCAL	DioSts(HANDLE, LPWORD, LPWORD);
DWORD	FAR	PASCAL	DioStop(HANDLE);
DWORD	FAR	PASCAL	DioClose(HANDLE);
DWORD	FAR	PASCAL	DioInpByte(HANDLE, WORD, LPBYTE);
DWORD	FAR PASCAL	DioOutByte(HANDLE, WORD, BYTE);
DWORD	FAR PASCAL	DioEchoBackByte(HANDLE, WORD, LPBYTE);
DWORD	FAR PASCAL	DioInpBit(HANDLE, WORD, LPBYTE);
DWORD	FAR PASCAL	DioOutBit(HANDLE, WORD,BYTE);
DWORD	FAR PASCAL	DioEchoBackBit(HANDLE, WORD, LPBYTE);
DWORD	FAR PASCAL	DioEventEx(HANDLE, WORD, WORD, HANDLE, WORD);
DWORD	FAR PASCAL	DioIntEnableEx(HANDLE, WORD, WORD);
DWORD	FAR PASCAL	DioGet8255Mode(HANDLE, WORD, LPWORD);
void	FAR PASCAL	DioUseMutex(WORD);
DWORD	FAR PASCAL	DioSetIoDirection(HANDLE, WORD, DWORD);
DWORD	FAR PASCAL	DioGetIoDirection(HANDLE, WORD, DWORD *);


#ifdef __cplusplus
}
#endif
