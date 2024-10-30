// *********************************************************************************
// 疵検工程コード定義ヘッダー
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//		基本的に、追記のみ (切り替えスイッチ用のみ)
// *********************************************************************************

#ifndef	_KIZULISTID_H
#define	_KIZULISTID_H

//***************************************************************
// 工程コード (工程記号?)
//   随時追加
//***************************************************************
#define SIZE_KOUTEI_CODE			2

#define KOUTEI_CODE_ZZ				"ZZ"				// 工程コードが無いライン (巻き戻しライン等々)

#define KOUTEI_CODE_CC				"CC"

#define KOUTEI_CODE_SHL				"01"
#define KOUTEI_CODE_HSL				"02"
#define KOUTEI_CODE_HOT				"03"
#define KOUTEI_CODE_SLL				"07"

#define KOUTEI_CODE_1PL				"11"
#define KOUTEI_CODE_2PL				"12"
#define KOUTEI_CODE_3PL				"13"
#define KOUTEI_CODE_4PL				"14"
#define KOUTEI_CODE_4PL_C			"14"

#define KOUTEI_CODE_2TCM			"22"
#define KOUTEI_CODE_3TCM			"23"

#define KOUTEI_CODE_2ECL			"32"
#define KOUTEI_CODE_3ECL			"33"

#define KOUTEI_CODE_2BAF			"42"
#define KOUTEI_CODE_3BAF			"43"

#define KOUTEI_CODE_2CGL_CAPL		"53"	// 2CGL-CAPL操業
#define KOUTEI_CODE_1CAPL			"54"
#define KOUTEI_CODE_2CAPL			"55"
#define KOUTEI_CODE_1TPM			"57"
#define KOUTEI_CODE_2TPM			"58"

#define KOUTEI_CODE_1RCL			"61"
#define KOUTEI_CODE_2RCL			"62"
#define KOUTEI_CODE_3RCL			"63"
#define KOUTEI_CODE_4RCL			"64"
#define KOUTEI_CODE_5RCL			"65"
#define KOUTEI_CODE_6RCL			"66"
#define KOUTEI_CODE_1CPL			"67"
#define KOUTEI_CODE_2CPL			"68"
#define KOUTEI_CODE_3CPL			"69"

#define KOUTEI_CODE_1CGL			"81"
#define KOUTEI_CODE_2CGL			"82"	// 2CGL-CGL操業
#define KOUTEI_CODE_3CGL			"83"
#define KOUTEI_CODE_4CGL			"84"
#define KOUTEI_CODE_5CGL			"85"
#define KOUTEI_CODE_1EGL			"87"

#define KOUTEI_CODE_ETL				"91"

#define KOUTEI_CODE_CTL				"A1"
#define	KOUTEI_CODE_1FLL			"A5"
#define KOUTEI_CODE_N1FLL_IN		"A5"
#define KOUTEI_CODE_N1FLL_OUT		"A5"
#define KOUTEI_CODE_2FLL			"A6"

#define KOUTEI_CODE_1MSL			"D1"
#define KOUTEI_CODE_2MSL			"D2"
#define KOUTEI_CODE_3MSL			"D3"
#define KOUTEI_CODE_4MSL			"D4"

#define KOUTEI_CODE_PLT_TOSOU		"E1"	// 予約

// 2014.07.25 川原  所間連携    >>>>>>>>
#define KOUTEI_CODE_HHOT			"H4"    // 広畑HOT
#define KOUTEI_CODE_HPGL			"H5"	// 広畑PGL
// 2014.07.25 川原  所間連携    <<<<<<<<


//***************************************************************
// 疵検ID(疵検管理Noの先頭2桁)
//   随時追加
//***************************************************************
#define SIZE_KIZUKEN_CODE			2

#define KIZUKEN_CODE_2TPM			"01"
#define KIZUKEN_CODE_2CAPL			"03"
#define KIZUKEN_CODE_ETL			"04"
#define KIZUKEN_CODE_CTL			"05"
#define KIZUKEN_CODE_1MSHL			"06"		// 簡易仮想疵検
#define KIZUKEN_CODE_HOT			"07"		// 予定
#define KIZUKEN_CODE_2MSHL			"06"		// 簡易仮想疵検
#define KIZUKEN_CODE_HOTFM			"09"
#define	KIZUKEN_CODE_HSPFM			"10"

#define KIZUKEN_CODE_2TCM			"12"
#define KIZUKEN_CODE_5CGL			"13"
#define KIZUKEN_CODE_1CGL			"14"
#define KIZUKEN_CODE_1CAPL			"15"
#define KIZUKEN_CODE_2CGL			"16"
#define KIZUKEN_CODE_3TCM			"17"
#define KIZUKEN_CODE_2FLLIN			"18"

#define KIZUKEN_CODE_1RCL			"20"
#define KIZUKEN_CODE_2RCL			"21"
#define KIZUKEN_CODE_4RCL			"22"
#define KIZUKEN_CODE_6RCL			"23"
#define KIZUKEN_CODE_HOTMAKI		"24"

#define KIZUKEN_CODE_2CPLMP			"25"		// 情報蓄積装置
#define KIZUKEN_CODE_4PL			"25"		// エッジ
#define KIZUKEN_CODE_4PL_C			"34"		// 色差

#define KIZUKEN_CODE_2FLLOUT		"26" //"19"

#define KIZUKEN_CODE_HOTACR			"28"		// 情報蓄積装置 → 熱延ACR(更新後)
#define KIZUKEN_CODE_1TPM			"29"
#define KIZUKEN_CODE_1CC			"30"
#define KIZUKEN_CODE_2CC			"31"
#define KIZUKEN_CODE_2PL			"32"
#define KIZUKEN_CODE_3PL			"33"

#define KIZUKEN_CODE_PLT			"35"

#define KIZUKEN_CODE_N2FLLMARK		"38"

#define KIZUKEN_CODE_MARK_CTL		"40"
#define KIZUKEN_CODE_MARK_ETL		"41"		// 予約
#define KIZUKEN_CODE_MARK_1MSHL		"42"		// 予約
#define KIZUKEN_CODE_MARK_2CPL		"43"		// 予約

#define KIZUKEN_CODE_V2CPL			"45"
#define KIZUKEN_CODE_V3CPL			"46"
#define KIZUKEN_CODE_VSHL			"47"
#define KIZUKEN_CODE_VHSL			"48"
#define KIZUKEN_CODE_VSLL			"49"

#define KIZUKEN_CODE_PLT_TOSOU		"50"		// 予約
#define KIZUKEN_CODE_3RCL			"51"

#define KIZUKEN_CODE_N1FLL_IN		"52"
#define KIZUKEN_CODE_N1FLL_OUT		"53"

#define	KIZUKEN_CODE_N1FLLMARK1		"54"
#define	KIZUKEN_CODE_N1FLLMARK2DS	"55"
#define	KIZUKEN_CODE_N1FLLMARK2WS	"56"

#define KIZUKEN_CODE_NCTL			"57"

#define KIZUKEN_CODE_NHOT_FM2		"58"

#define KIZUKEN_CODE_N1CAPL_SPM		"59"
#define KIZUKEN_CODE_N1CAPL_ANA		"60"

#define KIZUKEN_CODE_NHOT_SHIAGE    "61"

#define KIZUKEN_CODE_N5CGL_TINY     "62"

#define KIZUKEN_CODE_N1CC_W			"63"

#define KIZUKEN_CODE_NHOT_COMP		"64"

#define KIZUKEN_CODE_N2TCM_ANA		"66"
#define KIZUKEN_CODE_N2TCM_VZ		"67"

#define	KIZUKEN_CODE_VRCL			"90"

#endif
  