// *********************************************************************************
//	^XNΜξ{θ
//	[Ver]
//		Ver.01    2012/09/06
//
//	[]
//		
// *********************************************************************************

#ifndef	_MAINCONST_H
#define _MAINCONST_H

// αr CN[h
#include "..\..\include\LineCommon.h"									// αr€Κwb_[

//=======================================
// €Κθ
//=======================================
static const int PARAM_SYSLOG_BASENO = 370;						// VXOΜωθNo
																// 0 : FFT[o[ΙY½ίNoRC³΅
																// 1 : OHφαrξρAαrζo^Έs
																// 2 : OHφ~ΟDBΙΪ±¬χ
																// 3 : OHφ~ΟDB PingΩν
																// 4 : OHφ~ΟDB ODBCΪ±Ων
																// 5 : OHφ~ΟDB RCΐΡΘ΅


// 2TCM DB
#define DB_2TCM_DEFECT_INF_IN		"DEFECT_INF_IN"			// [DB] ό€αrξρe[u
#define DB_2TCM_DEFECT_IMG_IN		"DEFECT_IMG_IN"			// [DB] ό€αrζe[u

// 3TCM DB
#define DB_3TCM_DEFECT_INF_IN		"DEFECT_INF_IN"			// [DB] ό€αrξρe[u
#define DB_3TCM_DEFECT_IMG_IN		"DEFECT_IMG_IN"			// [DB] ό€αrζe[u

// 20200830 β@Έu±@@n[hE\tgi2TCMXVj aϋ ----->>>
// N2TCM_ANA DB
#define DB_N2TCM_ANA_DEFECT_INF		"T_DEFECT_INF"			// [DB] αrξρe[u
#define DB_N2TCM_ANA_DEFECT_IMG		"T_DEFECT_IMG"			// [DB] αrζe[u
// 20200830 β@Έu±@@n[hE\tgi2TCMXVj aϋ <<<-----

// 20221021 2TCMΐ@»ΙΊ€ΊHφό’ ¬ --->>>
// N2TCM_ANA DB
#define DB_N2TCM_KIZU_DEFECT_INF		"T_DEFECT_INF_T"			// [DB] αrξρe[u
#define DB_N2TCM_KIZU_DEFECT_IMG		"T_DEFECT_IMG_T"			// [DB] αrζe[u
// 20221021 2TCMΐ@»ΙΊ€ΊHφό’ ¬ ---<<<


enum ENUM_MAEDATOPERATION {
	// [XbgσM
	E_PD_ML_COIL = WM_USER + 100,		// RCξρσMΚm
	E_PD_ML_FF,							// FFOHφαrξρσMΚm
	E_PD_END							// IΉ
};

// FFOHφΐΡξρ\’Μ
struct FF_MAE_INF
{
	struct TORB{
		FF_MAE_JISSEKI	typFFData;						
	} torb[NUM_MEN];						// \  				
};

// OHφαrξρ\’Μ
struct MAE_DEFECT_IMAGE_INF
{
	char cKizuKenNo[SIZE_KIZUKEN_NO];		// ΗNo
	char cPreKizuKenNo[SIZE_KIZUKEN_NO];	// OHφΗNo
	int  nKouteiSyubetu;					// HφνΚuαroHφ»vͺΜ½ΤΪ©
	int  nKizuNo;							// αrNo
	int  nMaeKizuNo;						// OHφαrNo
	int  nOuDiv;							// \ ζͺ
	int  nCnt;								// ζf[^
	DETECT_BASE_DATA typDefectData;			// αrξρ
	int  nGazouNo;							// ζNo
	DWORD nSize[MAX_MAE_IMAGE];				// ζTCY
	BYTE* pImg[MAX_MAE_IMAGE];				// ζf[^
};

#endif
