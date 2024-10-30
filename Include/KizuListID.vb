'// *********************************************************************************
'// 疵検工程コード定義ヘッダー
'//	[Ver]
'//		Ver.01    2007/01/30  vs2005 対応
'//
'//	[メモ]
'//		基本的に、追記のみ (切り替えスイッチ用のみ)
'// *********************************************************************************

Public Module KizuList

    '//***************************************************************
    '// 工程コード (工程記号?)
    '//   随時追加
	'//***************************************************************
	Public Const SIZE_KOUTEI_CODE As Integer = 2

	Public Const KOUTEI_CODE_ZZ As String = "ZZ"				'// 工程コードが無いライン

    Public Const KOUTEI_CODE_CC As String = "CC"

	Public Const KOUTEI_CODE_SHL As String = "01"
	Public Const KOUTEI_CODE_HSL As String = "02"
	Public Const KOUTEI_CODE_HOT As String = "03"
	Public Const KOUTEI_CODE_SLL As String = "07"

    Public Const KOUTEI_CODE_1PL As String = "11"
    Public Const KOUTEI_CODE_2PL As String = "12"
    Public Const KOUTEI_CODE_3PL As String = "13"
    Public Const KOUTEI_CODE_4PL As String = "14"
    Public Const KOUTEI_CODE_4PL_C As String = "14"

    Public Const KOUTEI_CODE_2TCM As String = "22"
    Public Const KOUTEI_CODE_3TCM As String = "23"

    Public Const KOUTEI_CODE_2ECL As String = "32"
    Public Const KOUTEI_CODE_3ECL As String = "33"

    Public Const KOUTEI_CODE_2BAF As String = "42"
    Public Const KOUTEI_CODE_3BAF As String = "43"

	Public Const KOUTEI_CODE_2CGL_CAPL As String = "53"
	Public Const KOUTEI_CODE_1CAPL As String = "54"
	Public Const KOUTEI_CODE_2CAPL As String = "55"
    Public Const KOUTEI_CODE_1TPM As String = "57"
    Public Const KOUTEI_CODE_2TPM As String = "58"

    Public Const KOUTEI_CODE_1RCL As String = "61"
    Public Const KOUTEI_CODE_2RCL As String = "62"
    Public Const KOUTEI_CODE_3RCL As String = "63"
    Public Const KOUTEI_CODE_4RCL As String = "64"
    Public Const KOUTEI_CODE_5RCL As String = "65"
    Public Const KOUTEI_CODE_6RCL As String = "66"
    Public Const KOUTEI_CODE_1CPL As String = "67"
    Public Const KOUTEI_CODE_2CPL As String = "68"
    Public Const KOUTEI_CODE_3CPL As String = "69"

    Public Const KOUTEI_CODE_1CGL As String = "81"
    Public Const KOUTEI_CODE_2CGL As String = "82"
    Public Const KOUTEI_CODE_3CGL As String = "83"
    Public Const KOUTEI_CODE_4CGL As String = "84"
    Public Const KOUTEI_CODE_5CGL As String = "85"
	Public Const KOUTEI_CODE_1EGL As String = "87"

    Public Const KOUTEI_CODE_ETL As String = "91"

	Public Const KOUTEI_CODE_CTL As String = "A1"
    Public Const KOUTEI_CODE_1FLL As String = "A5"

    ' 2015.06.17 1FLL追加 ----->>>>
    Public Const KOUTEI_CODE_1FLL_IN As String = "A5"
    Public Const KOUTEI_CODE_1FLL_OUT As String = "A5"
    ' 2015.06.17 1FLL追加 <<<<-----

	Public Const KOUTEI_CODE_2FLL As String = "A6"

    Public Const KOUTEI_CODE_1MSL As String = "D1"
    Public Const KOUTEI_CODE_2MSL As String = "D2"
    Public Const KOUTEI_CODE_3MSL As String = "D3"
    Public Const KOUTEI_CODE_4MSL As String = "D4"

    '2014.07.25 川原    所間連携    >>>>>>>>
    Public Const KOUTEI_CODE_HHOT As String = "H4"
    Public Const KOUTEI_CODE_HPGL As String = "H5"
    '2014.07.25 川原    所間連携    <<<<<<<<

    '//***************************************************************
    '// 疵検ID(疵検管理Noの先頭2桁)
    '//   随時追加
	'//***************************************************************
	Public Const SIZE_KIZUKEN_CODE As Integer = 2

	Public Const KIZUKEN_CODE_2TPM As String = "01"
	Public Const KIZUKEN_CODE_2CAPL As String = "03"
	Public Const KIZUKEN_CODE_ETL As String = "04"
	Public Const KIZUKEN_CODE_CTL As String = "05"
	Public Const KIZUKEN_CODE_1MSHL As String = "06"		'// 簡易仮想疵検
	Public Const KIZUKEN_CODE_HOT As String = "07"			'// 予定
	Public Const KIZUKEN_CODE_2MSHL As String = "06"		'// 簡易仮想疵検
	Public Const KIZUKEN_CODE_HOTFM As String = "09"

	Public Const KIZUKEN_CODE_2TCM As String = "12"
	Public Const KIZUKEN_CODE_5CGL As String = "13"
	Public Const KIZUKEN_CODE_1CGL As String = "14"
	Public Const KIZUKEN_CODE_1CAPL As String = "15"
	Public Const KIZUKEN_CODE_2CGL As String = "16"
	Public Const KIZUKEN_CODE_3TCM As String = "17"
	Public Const KIZUKEN_CODE_2FLLIN As String = "18"

	Public Const KIZUKEN_CODE_1RCL As String = "20"
	Public Const KIZUKEN_CODE_2RCL As String = "21"
	Public Const KIZUKEN_CODE_4RCL As String = "22"
	Public Const KIZUKEN_CODE_6RCL As String = "23"
	Public Const KIZUKEN_CODE_HOTMAKI As String = "24"

	Public Const KIZUKEN_CODE_2CPLMP As String = "25"		 '// 情報蓄積装置
	Public Const KIZUKEN_CODE_4PL As String = "25"
    Public Const KIZUKEN_CODE_4PL_C As String = "34"

	Public Const KIZUKEN_CODE_2FLLOUT As String = "26"		 '//"19"

	Public Const KIZUKEN_CODE_HOTACR As String = "28"		 '// 情報蓄積装置
	Public Const KIZUKEN_CODE_1TPM As String = "29"
	Public Const KIZUKEN_CODE_1CC As String = "30"
	Public Const KIZUKEN_CODE_2CC As String = "31"
	Public Const KIZUKEN_CODE_2PL As String = "32"
	Public Const KIZUKEN_CODE_3PL As String = "33"

	Public Const KIZUKEN_CODE_N2FLLMARK As String = "38"

	Public Const KIZUKEN_CODE_MARK_CTL As String = "40"
	Public Const KIZUKEN_CODE_MARK_ETL As String = "41"		   '// 予約
	Public Const KIZUKEN_CODE_MARK_1MSHL As String = "42"		 '// 予約
	Public Const KIZUKEN_CODE_MARK_2CPL As String = "43"		'// 予約

	Public Const KIZUKEN_CODE_V2CPL As String = "45"
	Public Const KIZUKEN_CODE_V3CPL As String = "46"
	Public Const KIZUKEN_CODE_VSHL As String = "47"
	Public Const KIZUKEN_CODE_VHSL As String = "48"
	Public Const KIZUKEN_CODE_VSLL As String = "49"

	Public Const KIZUKEN_CODE_1FLL_IN As String = "52"
	Public Const KIZUKEN_CODE_1FLL_OUT As String = "53"

    Public Const KIZUKEN_CODE_N1FLLMARK1 As String = "54"
    Public Const KIZUKEN_CODE_N1FLLMARK2DS As String = "55"
    Public Const KIZUKEN_CODE_N1FLLMARK2WS As String = "56"
    
    Public Const KIZUKEN_CODE_NCTL As String = "57"

    Public Const KIZUKEN_CODE_NHOT_FM2 As String = "58"

	Public Const KIZUKEN_CODE_N1CAPL_SPM As String = "59"
	Public Const KIZUKEN_CODE_N1CAPL_ANA As String = "60"

	Public Const KIZUKEN_CODE_NHOT_SHIAGE As String = "61"

	Public Const KIZUKEN_CODE_N5CGL_TINY As String = "62"

    Public Const KIZUKEN_CODE_N1CC_W As String = "63"
	
	Public Const KIZUKEN_CODE_NHOT_COMP As String = "64"

	Public Const KIZUKEN_CODE_N2TCM_ANA As String = "66"
	Public Const KIZUKEN_CODE_N2TCM_VZ As String = "67"

	Public Const KIZUKEN_CODE_VRCL As String = "90"
End Module
