/*******************************************************

	API-CNT(PC/98)W95, NT
	FILE NAME	APICNT8M.H for CNT32-8M(PCI)
    UPDATE,VERSION  2001.02.19  Ver 3.20

********************************************************/

#pragma pack(push, 4)

typedef struct tagCNTMESS
{
	WORD		MessType;
	WORD		MessData;
}
CNTMESS,*PCNTMESS;

#pragma pack(pop)

/***********************************
  API-CNT(PC/98)W95, NT FUNCTION 
************************************/
#ifdef __cplusplus
extern"C"{
#endif
// Common Function
DWORD	FAR		PASCAL	CntOpen(LPHANDLE,WORD,WORD,HANDLE,WORD,CNTMESS FAR *);
DWORD	FAR		PASCAL	CntOpenEx(char *,LPHANDLE,HANDLE,WORD,CNTMESS FAR *);
DWORD	FAR		PASCAL	CntClose(HANDLE);
DWORD	FAR		PASCAL	CntReadCnt(HANDLE, LPWORD, WORD, LPDWORD);
DWORD	FAR		PASCAL	CntReadSts(HANDLE, WORD, LPWORD);
DWORD	FAR		PASCAL	CntPreset(HANDLE, LPWORD, WORD, LPDWORD);
DWORD	FAR		PASCAL	CntRunSts(HANDLE, WORD, LPWORD);
/********************************
 New Function Ver3.20
********************************/
// New Counter Functin
DWORD	FAR		PASCAL	CntSetZMode(HANDLE, WORD, WORD);
DWORD	FAR		PASCAL	CntSetZLogic(HANDLE, WORD, WORD);
DWORD	FAR		PASCAL	CntNotifyCountUp(HANDLE, WORD, WORD, DWORD, HANDLE, DWORD);
DWORD	FAR		PASCAL	CntStopNotifyCountUp(HANDLE, WORD, WORD);
DWORD	FAR		PASCAL	CntSelectChannelSignal(HANDLE, WORD, WORD);
DWORD	FAR		PASCAL	CntSetCountDirection(HANDLE, WORD, WORD);
DWORD	FAR		PASCAL	CntSetDigitalFilter(HANDLE, WORD, WORD);
DWORD	FAR		PASCAL	CntSetOperationMode(HANDLE, WORD, WORD, WORD, WORD);
DWORD	FAR		PASCAL	CntStartCount(HANDLE, LPWORD, WORD, LPWORD);
DWORD	FAR		PASCAL	CntStopCount(HANDLE, LPWORD, WORD, LPWORD);
DWORD	FAR		PASCAL	CntNotifyTimeUp(HANDLE, DWORD, HANDLE, DWORD);
DWORD	FAR		PASCAL	CntStopNotifyTimeUp(HANDLE);
void	FAR		PASCAL	CntUseMutex(BOOL bSw);

// Counter Function for CNT32-8M(PCI)
DWORD 	FAR		PASCAL	CntNotifyCounterError(HANDLE hDrv, HANDLE hWnd, DWORD dwMsg);
DWORD 	FAR		PASCAL	CntSetOutputSignalEvent(HANDLE hDrv, WORD ChNo, WORD OutputLogic, DWORD EventType, DWORD PulseLength);
DWORD	FAR		PASCAL	CntSetInputSignalEvent(HANDLE hDrv, WORD ChNo, WORD Reserved, DWORD EventType, WORD RF0, WORD RF1);
DWORD	FAR		PASCAL	CntSetCountUpEvent(HANDLE hDrv, WORD ChNo, WORD RegNo, DWORD EventType, DWORD Reserved);
DWORD	FAR		PASCAL	CntSetPresetReg(HANDLE hDrv, WORD ChNo, WORD Reserved, DWORD PresetData);
DWORD	FAR		PASCAL	CntNotifyCarryBorrow(HANDLE hDrv, HANDLE hWnd, DWORD dwMsg);
DWORD	FAR		PASCAL	CntReadStsEx(HANDLE hDrv, WORD ChNo, DWORD *StsEx);
DWORD	FAR		PASCAL	CntOutPulse(HANDLE hDrv, WORD ChNo, WORD Reserved, BYTE OutData);
DWORD	FAR		PASCAL	CntResetChannel(HANDLE hDrv, WORD *ChNo, WORD ChNum);

// Sampling Function
DWORD	FAR		PASCAL	CntSetSyncSignal(HANDLE hDrv, DWORD Clock, DWORD StartTrg, DWORD StopTrg);
DWORD	FAR		PASCAL	CntSetStandAlone(HANDLE hDrv);
DWORD	FAR		PASCAL	CntSetMasterCfg(HANDLE hDrv, DWORD ExtSig1, DWORD ExtSig2, DWORD ExtSig3, DWORD MasterHalt, DWORD SlaveHalt);
DWORD	FAR		PASCAL	CntSetSlaveCfg(HANDLE hDrv, DWORD ExtSig1, DWORD ExtSig2, DWORD ExtSig3, DWORD MasterHalt, DWORD SlaveHal);
DWORD	FAR		PASCAL	CntSetSamplingMode(HANDLE hDrv, WORD ChNo, WORD ChMode);
DWORD	FAR		PASCAL	CntSetSamplingStartTrg(HANDLE hDrv, DWORD Start, WORD CountStart);
DWORD	FAR		PASCAL	CntSetSamplingClockTrg(HANDLE hDrv, DWORD Clock);
DWORD	FAR		PASCAL	CntSetSamplingStopTrg(HANDLE hDrv, DWORD Stop, WORD CountStop);
DWORD	FAR		PASCAL	CntSetSamplingChannel(HANDLE hDrv, WORD ChNum);
DWORD	FAR		PASCAL	CntSetSamplingClock(HANDLE hDrv, DWORD Clock, DWORD Unit);
DWORD	FAR		PASCAL	CntSetSamplingStopNum(HANDLE hDrv, DWORD StopNum);
DWORD	FAR		PASCAL	CntResetSampling(HANDLE hDrv, DWORD ResetType);
DWORD	FAR		PASCAL	CntSetSamplingBuff(HANDLE hDrv, DWORD *Buff, WORD ChNum, DWORD ScanNum, DWORD Ring);
DWORD	FAR		PASCAL	CntStartSampling(HANDLE hDrv);
DWORD	FAR		PASCAL	CntStopSampling(HANDLE hDrv);
DWORD	FAR		PASCAL	CntGetSamplingStatus(HANDLE hDrv, DWORD *Status, DWORD *Err);
DWORD	FAR		PASCAL	CntGetSamplingCount(HANDLE hDrv, DWORD *ScanNum, DWORD *Reserved);
DWORD	FAR		PASCAL	CntNotifySamplingStop(HANDLE hDrv, HANDLE hWnd, DWORD dwMsg);
DWORD	FAR		PASCAL	CntNotifySamplingCount(HANDLE hDrv, DWORD Count, HANDLE hWnd, DWORD dwMsg);

#ifdef __cplusplus
}
#endif
/*****************************
 Parameters
*****************************/
// Z Phase Mode
#define	CNT_ZPHASE_NOT_USE			1
#define	CNT_ZPHASE_NEXT_ONE			2
#define	CNT_ZPHASE_EVERY_TIME		3
// Z Phase Logic
#define	CNT_ZLOGIC_POSITIVE			0
#define	CNT_ZLOGIC_NEGATIVE			1
// Signal Source
#define	CNT_SIGTYPE_ISOLATE			0
#define	CNT_SIGTYPE_TTL				1
#define	CNT_SIGTYPE_LINERECEIVER	2
// Count Direction
#define	CNT_DIR_DOWN				0
#define	CNT_DIR_UP					1
// 1Phase/2Phase
#define	CNT_MODE_1PHASE				0
#define	CNT_MODE_2PHASE				1
#define	CNT_MODE_GATECONTROL		2
// Mul
#define	CNT_MUL_X1					0
#define	CNT_MUL_X2					1
#define	CNT_MUL_X4					2
// Sync Clear
#define	CNT_CLR_ASYNC				0
#define	CNT_CLR_SYNC				1
// Generic I/O Signal
#define	CNT_GENIO_LINERECEIVER		0
#define	CNT_GENIO_TTL				1

// Error Type
#define	CNT_AI_ERROR				1
#define	CNT_FLT_ERROR				2
#define	CNT_ALM_ERROR				3
// Output Signal Event
#define	CNT_OEVENT_GENOUT			0
#define	CNT_OEVENT_MATCH0			0x01
#define	CNT_OEVENT_MATCH1			0x02
#define	CNT_OEVENT_FLTERR			0x04
#define	CNT_OEVENT_AIERR			0x08
#define	CNT_OEVENT_ALMERR			0x10
// Output Logic
#define	CNT_OEVENT_NEGATIVE			0
#define	CNT_OEVENT_POSITIVE			1
// Input Signal Event
#define	CNT_IEVENT_GENINP			0
#define	CNT_IEVENT_STARTSTOP		1
#define	CNT_IEVENT_PRESET			2
#define	CNT_IEVENT_CLEAR			3
// RIZE/FALL
#define	CNT_INP_NONE				0
#define	CNT_INP_RIZE				1
#define	CNT_INP_FALL				2
// Couunt Match Event
#define	CNT_MATCH_NONE				0
#define	CNT_MATCH_PRESET			1
#define	CNT_MATCH_CLEAR				2
// Carry/Borrow
#define	CNT_CB_CARRY				1
#define	CNT_CB_BORROW				2

// Sampling

// SyncSignal
#define	CNTM_START_OUT_ON			1
#define	CNTM_START_OUT_OFF			0
#define	CNTM_CLOCK_OUT_ON			1
#define	CNTM_CLOCK_OUT_OFF			0
#define	CNTM_STOP_OUT_ON			1
#define	CNTM_STOP_OUT_OFF			0
// ExtSig
#define	CNT8M_EXT_NOTUSE				0		// Not Used
#define CNT8M_EXT_START_SOFT			1		// Software Start
#define CNT8M_EXT_STOP_SOFT				2		// Software Stop
#define CNT8M_EXT_CLOCK					3		// Internal Clock
#define CNT8M_EXT_EXT_TRG				4		// External Clock
#define CNT8M_EXT_START_EXT_RISE		5		// External Start Rize
#define CNT8M_EXT_START_EXT_FALL		6		// External Start Fall
#define CNT8M_EXT_STOP_EXT_RISE			7		// External Stop Rize
#define CNT8M_EXT_STOP_EXT_FALL			8		// External Stop Fall
#define CNT8M_EXT_CLOCK_ERROR			9		// Sampling Clock Error
#define	CNT8M_EXT_TRNSNUM				10		// Stop Number of Transfer Data
#define	CNT8M_EXT_CNTMATCH_CH0_REG0		11		// Count Match Ch0:REG0
#define	CNT8M_EXT_CNTMATCH_CH1_REG0		12		// Count Match CH1:REG0
#define	CNT8M_EXT_CNTMATCH_CH2_REG0		13		// Count Match CH2:REG0
#define	CNT8M_EXT_CNTMATCH_CH3_REG0		14		// Count Match CH3:REG0
#define	CNT8M_EXT_CNTMATCH_CH4_REG0		15		// Count Match CH4:REG0
#define	CNT8M_EXT_CNTMATCH_CH5_REG0		16		// Count Match CH5:REG0
#define	CNT8M_EXT_CNTMATCH_CH6_REG0		17		// Count Match CH6:REG0
#define	CNT8M_EXT_CNTMATCH_CH7_REG0		18		// Count Match CH7:REG0
#define	CNT8M_EXT_CNTMATCH_CH0_REG1		19		// Count Match CH0:REG1
#define	CNT8M_EXT_CNTMATCH_CH1_REG1		20		// Count Match CH1:REG1
#define	CNT8M_EXT_CNTMATCH_CH2_REG1		21		// Count Match CH2:REG1
#define	CNT8M_EXT_CNTMATCH_CH3_REG1		22		// Count Match CH3:REG1
#define	CNT8M_EXT_CNTMATCH_CH4_REG1		23		// Count Match CH4:REG1
#define	CNT8M_EXT_CNTMATCH_CH5_REG1		24		// Count Match CH5:REG1
#define	CNT8M_EXT_CNTMATCH_CH6_REG1		25		// Count Match CH6:REG1
#define	CNT8M_EXT_CNTMATCH_CH7_REG1		26		// Count Match CH7:REG1
// Count Mode
#define	CNT8M_MODE_NORMAL				0		// Normal Counter
#define	CNT8M_MODE_DIFF					1		// Differencial Counter
// Sampling Start
#define	CNT8M_START_SOFT				1		// Software Start
#define	CNT8M_START_EXT_RISE			2		// External Start Rize
#define	CNT8M_START_EXT_FALL			3		// External Start Fall
#define	CNT8M_START_CNTMATCH_CH0_REG0	4		// Count Match CH0:REG0
#define	CNT8M_START_CNTMATCH_CH1_REG0	5		// Count Match CH1:REG0
#define	CNT8M_START_CNTMATCH_CH2_REG0	6		// Count Match CH2:REG0
#define	CNT8M_START_CNTMATCH_CH3_REG0	7		// Count Match CH3:REG0
#define	CNT8M_START_CNTMATCH_CH4_REG0	8		// Count Match CH4:REG0
#define	CNT8M_START_CNTMATCH_CH5_REG0	9		// Count Match CH5:REG0
#define	CNT8M_START_CNTMATCH_CH6_REG0	10		// Count Match CH6:REG0
#define	CNT8M_START_CNTMATCH_CH7_REG0	11		// Count Match CH7:REG0
#define	CNT8M_START_CNTMATCH_CH0_REG1	12		// Count Match CH0:REG1
#define	CNT8M_START_CNTMATCH_CH1_REG1	13		// Count Match CH1:REG1
#define	CNT8M_START_CNTMATCH_CH2_REG1	14		// Count Match CH2:REG1
#define	CNT8M_START_CNTMATCH_CH3_REG1	15		// Count Match CH3:REG1
#define	CNT8M_START_CNTMATCH_CH4_REG1	16		// Count Match CH4:REG1
#define	CNT8M_START_CNTMATCH_CH5_REG1	17		// Count Match CH5:REG1
#define	CNT8M_START_CNTMATCH_CH6_REG1	18		// Count Match CH6:REG1
#define	CNT8M_START_CNTMATCH_CH7_REG1	19		// Count Match CH7:REG1
#define	CNT8M_START_EXTSIG_1			20		// EXTSIG1
#define	CNT8M_START_EXTSIG_2			21		// EXTSIG2
#define	CNT8M_START_EXTSIG_3			22		// EXTSIG3
// Counter Start
#define	CNT8M_START_CNT_NOT_WITH_SAMP	0		// Counter Start not with Sampling Start
#define	CNT8M_START_CNT_WITH_SAMP		1		// Counter Start with Sampling Start
// Sampling Clock
#define	CNT8M_CLOCK_CLOCK				1		// Internal Clock
#define	CNT8M_CLOCK_EXT_TRG				2		// External Clock
#define	CNT8M_CLOCK_EXTSIG_1			3		// EXTSIG1
#define	CNT8M_CLOCK_EXTSIG_2			4		// EXTSIG2
#define	CNT8M_CLOCK_EXTSIG_3			5		// EXTSIG3
// Sampling Stop
#define	CNT8M_STOP_SOFT					1		// Software Stop
#define	CNT8M_STOP_EXT_RISE				2		// External Stop Rize
#define	CNT8M_STOP_EXT_FALL				3		// External Stop Fall
#define	CNT8M_STOP_NUM					4		// Stop with Number of Data
#define	CNT8M_STOP_CNTMATCH_CH0_REG0	5		// Count Match CH0:REG0
#define	CNT8M_STOP_CNTMATCH_CH1_REG0	6		// Count Match CH1:REG0
#define	CNT8M_STOP_CNTMATCH_CH2_REG0	7		// Count Match CH2:REG0
#define	CNT8M_STOP_CNTMATCH_CH3_REG0	8		// Count Match CH3:REG0
#define	CNT8M_STOP_CNTMATCH_CH4_REG0	9		// Count Match CH4:REG0
#define	CNT8M_STOP_CNTMATCH_CH5_REG0	10		// Count Match CH5:REG0
#define	CNT8M_STOP_CNTMATCH_CH6_REG0	11		// Count Match CH6:REG0
#define	CNT8M_STOP_CNTMATCH_CH7_REG0	12		// Count Match CH7:REG0
#define	CNT8M_STOP_CNTMATCH_CH0_REG1	13		// Count Match CH0:REG1
#define	CNT8M_STOP_CNTMATCH_CH1_REG1	14		// Count Match CH1:REG1
#define	CNT8M_STOP_CNTMATCH_CH2_REG1	15		// Count Match CH2:REG1
#define	CNT8M_STOP_CNTMATCH_CH3_REG1	16		// Count Match CH3:REG1
#define	CNT8M_STOP_CNTMATCH_CH4_REG1	17		// Count Match CH4:REG1
#define	CNT8M_STOP_CNTMATCH_CH5_REG1	18		// Count Match CH5:REG1
#define	CNT8M_STOP_CNTMATCH_CH6_REG1	19		// Count Match CH6:REG1
#define	CNT8M_STOP_CNTMATCH_CH7_REG1	20		// Count Match CH7:REG1
#define	CNT8M_STOP_EXTSIG_1				21		// EXTSIG1
#define	CNT8M_STOP_EXTSIG_2				22		// EXTSIG2
#define	CNT8M_STOP_EXTSIG_3				23		// EXTSIG3
// Counter Stop
#define	CNT8M_STOP_CNT_NOT_WITH_SAMP	0		// Counter Stop not with Sampling Stop
#define	CNT8M_STOP_CNT_WITH_SAMP		1		// Counter Stop with Sampling Stop
// Clock Unit
#define	CNT8M_TIM_UNIT_S		1
#define	CNT8M_TIM_UNIT_MS		2
#define	CNT8M_TIM_UNIT_US		3
#define	CNT8M_TIM_UNIT_NS		4
// Reset
#define	CNT8M_RESET_FIFO		0x02
// Buffer
#define	CNT8M_WRITE_ONCE			0
#define	CNT8M_WRITE_RING			1
// Status
#define	CNT8M_STATUS_BMSTOP		0x00000001
#define	CNT8M_STATUS_CNTSTART	0x00000002
#define	CNT8M_STATUS_CNTSTOP	0x00000004
#define	CNT8M_STATUS_TRGIN		0x00000008
#define	CNT8M_STATUS_OVERRUN	0x00000010
// Error
#define	CNT8M_STATUS_FIFOEMPTY	0x00000001
#define	CNT8M_STATUS_FIFOFULL	0x00000002
#define	CNT8M_STATUS_SGOVERIN	0x00000004
#define	CNT8M_STATUS_TRGERR		0x00000008
#define	CNT8M_STATUS_CLKERR		0x00000010
#define	CNT8M_STATUS_SLAVEHALT	0x00000020
#define	CNT8M_STATUS_MASTERHALT	0x00000040
