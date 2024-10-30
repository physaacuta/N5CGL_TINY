// *********************************************************************************
//	ライブラリの設定切り替えスイッチ集　(各システムで自由に変更可能)
//	[Ver]
//		Ver.01    2009/08/28  x64 対応
//
//	[メモ]
//		基本的に、書き換え可 (切り替えスイッチ用のみ)
// *********************************************************************************


#pragma once


//***************************************************************
// ライブラリ・タスク共通 定義
//***************************************************************
//=======================================
// 切り替え用定数定義
//=======================================
// ここは、オンライン時すべてコメントにしておくこと！！
//#define LOCAL												// 本番時コメント  (テスト時コメントアウト)
//#define NO_HW												// 本番時コメント  (ハードウェアが無し)
//#define PLG_DEBUG											// PLGテスト
//#define PIO_DEBUG											// PIOテスト
//#define NON_DB_SAVE										// 実績DB保存無し
//#define DEF_DETECT_LOG									// 疵検出ログ出力無し



//***************************************************************
// ライブラリ 定義
//***************************************************************
//=======================================
// KizuBcp
//=======================================
//// SQL Native Cliant のバージョン設定
//#define KIZUBCP_SQLSERVER_2005
//#define KIZUBCP_SQLSERVER_2008
#define KIZUBCP_SQLSERVER_2014

// VS2010では、Libraryに*.libをpragmaで引っ付けるのがうまくいかないので、
// タスク側でlibをリンクする時に有効化。  ※VS2010では必須かも
//   パスとlib両方。
#define LIB_MANUAL


//// BCP超高速画像データ書き込み時の最大イメージサイズ設定
#define KIZUBCP_MAX_IMGSIZE 	4096*4096					// 分からなければ、そのシステムのフレームサイズを指定しておけばとりあえずOK。



//// パス関係(全部コメントなら、従来どおり KizuBcpの中のやつを使用する)
// KizuBcpの奴と違う場合、上書きでこのパスを使用する
#ifdef KIZUBCP_SQLSERVER_2005
	#define KIZUBCP_PATH_H	"C:\\Program Files\\Microsoft SQL Server\\90\\SDK\\Include\\sqlncli.h"

  #ifndef LIB_MANUAL
	#ifdef _M_X64			// x64 コンパイル
		#ifdef _DEBUG
			#define KIZUBCP_PATH_L	"C:\\Program Files\\Microsoft SQL Server\\90\\SDK\\Lib\\x64\\sqlncli.lib"
		#else
			#define KIZUBCP_PATH_L	"\"C:\\Program Files\\Microsoft SQL Server\\90\\SDK\\Lib\\x64\\sqlncli.lib\""
		#endif
	#else
		#ifdef _DEBUG
			#define KIZUBCP_PATH_L	"C:\\Program Files\\Microsoft SQL Server\\90\\SDK\\Lib\\x86\\sqlncli.lib"
		#else
			#define KIZUBCP_PATH_L	"\"C:\\Program Files\\Microsoft SQL Server\\90\\SDK\\Lib\\x86\\sqlncli.lib\""
		#endif
	#endif
  #endif
#endif
#ifdef KIZUBCP_SQLSERVER_2008
	#define KIZUBCP_PATH_H	"C:\\Program Files\\Microsoft SQL Server\\100\\SDK\\Include\\sqlncli.h"

  #ifndef LIB_MANUAL
	#ifdef _M_X64			// x64 コンパイル
		#ifdef _DEBUG
			#define KIZUBCP_PATH_L	"C:\\Program Files\\Microsoft SQL Server\\100\\SDK\\Lib\\x64\\sqlncli10.lib"
		#else
			#define KIZUBCP_PATH_L	"\"C:\\Program Files\\Microsoft SQL Server\\100\\SDK\\Lib\\x64\\sqlncli10.lib\""
		#endif
	#else
		#ifdef _DEBUG
			#define KIZUBCP_PATH_L	"C:\\Program Files\\Microsoft SQL Server\\100\\SDK\\Lib\\x86\\sqlncli10.lib"
		#else
			#define KIZUBCP_PATH_L	"\"C:\\Program Files\\Microsoft SQL Server\\100\\SDK\\Lib\\x86\\sqlncli10.lib\""
		#endif
	#endif
  #endif
#endif
#ifdef KIZUBCP_SQLSERVER_2014
	#define KIZUBCP_PATH_H	"C:\\Program Files\\Microsoft SQL Server\\110\\SDK\\Include\\sqlncli.h"

  #ifndef LIB_MANUAL
	#ifdef _M_X64			// x64 コンパイル
		#ifdef _DEBUG
			#define KIZUBCP_PATH_L	"C:\\Program Files\\Microsoft SQL Server\\110\\SDK\\Lib\\x64\\sqlncli11.lib"
		#else
			#define KIZUBCP_PATH_L	"\"C:\\Program Files\\Microsoft SQL Server\\110\\SDK\\Lib\\x64\\sqlncli11.lib\""
		#endif
	#else
		#ifdef _DEBUG
			#define KIZUBCP_PATH_L	"C:\\Program Files\\Microsoft SQL Server\\110\\SDK\\Lib\\x86\\sqlncli11.lib"
		#else
			#define KIZUBCP_PATH_L	"\"C:\\Program Files\\Microsoft SQL Server\\110\\SDK\\Lib\\x86\\sqlncli11.lib\""
		#endif
	#endif
  #endif
#endif

//=======================================
// DefectLib
//=======================================
//// IPP ライブラリ インストール済み 設定
#ifdef _M_X64			// x64 コンパイル
	#define DEFECTLIB_KIZUJPEGMANAGER							// ipp未インストール時は、コメント化することで、コンパイルが通るようになる
#else
//	#define DEFECTLIB_KIZUJPEGMANAGER							// ipp未インストール時は、コメント化することで、コンパイルが通るようになる
#endif


//// コンテック WMD版ドライバ使用設定
#define DEFECTLIB_CONTEC_PIO_NON							// そもそもPIOボード無しシステム コメントアウト
#define DEFECTLIB_CONTEC_CNT_NON							// そもそもCNTボード無しシステム コメントアウト

#define DEFECTLIB_CONTEC_PIO_WMD							// PIOボード WMD版ドライバ使用時 コメントアウト
#define DEFECTLIB_CONTEC_CNT_WMD							// CNTボード WMD版ドライバ使用時 コメントアウト

//-----------------------------
//// パス関係(全部コメントなら、従来どおり ライブラリの中のやつを使用する)
#ifndef DEFECTLIB_CONTEC_CNT_NON
#ifndef DEFECTLIB_CONTEC_CNT_WMD		// 従来版
	#define DEFECTLIB_CONTEC_CNT_PATH_H "..\..\include\Other\APICNT8M.H"						// カウンターボード インクルード

  #ifndef LIB_MANUAL
	// VSのバグっぽい。
	#ifdef _DEBUG
		#define DEFECTLIB_CONTEC_CNT_PATH_L "C:\\Program Files\\CONTEC\\API-PAC(W32)\\CNT\\Samples\\lib\\APICNT.lib"
	#else
		#define DEFECTLIB_CONTEC_CNT_PATH_L "\"C:\\Program Files\\CONTEC\\API-PAC(W32)\\CNT\\Samples\\lib\\APICNT.lib\""
	#endif
  #endif
#else									// WMD版
	#define DEFECTLIB_CONTEC_CNT_PATH_H "..\..\include\Other\CCnt.h"							// CNTボード インクルード

  #ifndef LIB_MANUAL
	// VSのバグっぽい。
	#ifdef _M_X64			// x64 コンパイル
		#ifdef _DEBUG
			#define DEFECTLIB_CONTEC_CNT_PATH_L "C:\\Program Files (x86)\\CONTEC\\API-PAC(W32)\\CNTWDM\\Samples\\Lib_amd64\\CCNT.lib"
		#else
			#define DEFECTLIB_CONTEC_CNT_PATH_L "\"C:\\Program Files (x86)\\CONTEC\\API-PAC(W32)\\CNTWDM\\Samples\\Lib_amd64\\CCNT.lib\""
		#endif
	#else
		#ifdef _DEBUG
			#define DEFECTLIB_CONTEC_CNT_PATH_L  "C:\\Program Files\\CONTEC\\API-PAC(W32)\\CNTWDM\\Samples\\Lib_i386\\CCNT.lib"
		#else
			#define DEFECTLIB_CONTEC_CNT_PATH_L  "\"C:\\Program Files\\CONTEC\\API-PAC(W32)\\CNTWDM\\Samples\\Lib_i386\\CCNT.lib\""
		#endif
	#endif
  #endif
#endif
#endif

//-----------------------------
//// パス関係(全部コメントなら、従来どおり ライブラリの中のやつを使用する)
#ifndef DEFECTLIB_CONTEC_PIO_NON
#ifndef DEFECTLIB_CONTEC_PIO_WMD		// 従来版
	#define DEFECTLIB_CONTEC_PIO_PATH_H "..\..\include\Other\APIDIO.H"								// PIOボード インクルード
	
  #ifndef LIB_MANUAL
// VSのバグっぽい。
	#ifdef _DEBUG
		#define DEFECTLIB_CONTEC_PIO_PATH_L "C:\\Program Files\\CONTEC\\API-PAC(W32)\\Dio\\Samples\\Lib\\APIDIO.lib"
	#else
		#define DEFECTLIB_CONTEC_PIO_PATH_L "\"C:\\Program Files\\CONTEC\\API-PAC(W32)\\Dio\\Samples\\Lib\\APIDIO.lib\""
	#endif
  #endif
#else									// WMD版
	#define DEFECTLIB_CONTEC_PIO_PATH_H "..\..\include\Other\CDio.h"								// PIOボード インクルード

  #ifndef LIB_MANUAL
	// VSのバグっぽい。
	#ifdef _M_X64			// x64 コンパイル
		#ifdef _DEBUG			 
			#define DEFECTLIB_CONTEC_PIO_PATH_L "C:\\Program Files (x86)\\CONTEC\\API-PAC(W32)\\DIOWDM\\Samples\\Lib_amd64\\CDio.lib"
		#else
			#define DEFECTLIB_CONTEC_PIO_PATH_L "\"C:\\Program Files (x86)\\CONTEC\\API-PAC(W32)\\DIOWDM\\Samples\\Lib_amd64\\CDio.lib\""
		#endif
	#else
		#ifdef _DEBUG			 
			#define DEFECTLIB_CONTEC_PIO_PATH_L "C:\\Program Files\\CONTEC\\API-PAC(W32)\\DIOWDM\\Samples\\Lib_i386\\CDio.lib"
		#else
			#define DEFECTLIB_CONTEC_PIO_PATH_L "\"C:\\Program Files\\CONTEC\\API-PAC(W32)\\DIOWDM\\Samples\\Lib_i386\\CDio.lib\""
		#endif
	#endif
  #endif
#endif
#endif

//=======================================
// AtlRegExpLib
//=======================================
//// ATL or STD
//#define ATLREGEXPLIB_IS_ATL									// STDを使用(=VS2008以降)する場合コメント化すること

//=======================================
// IportLib
//=======================================
//// MMXコピー or 通常のmemcpy
//#define IPORTLIB_IMG_COPY_MMX								// MMXのSwdetectMMX::mem_copy_64 を使用する場合、コメントアウト
																// フォルダ構成は、ベースフォルダ\Epc\lib であること
//// 混色分離アセンブラ
#define	IPORTLIB_IMG_CORR_MMX	// コメントアウトでアセンブラ

//=======================================
// Jpeg圧縮ライブラリ
//=======================================
#define JPEGLIB_LJT												// libjpeg-Turboを使用
//#define JPEGLIB_IPP											// インテル IPPを使用
