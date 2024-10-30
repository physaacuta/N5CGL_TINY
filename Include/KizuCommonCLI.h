// *********************************************************************************
//	マネージドコード用インクルード
//	[Ver]
//		Ver.01    2007/01/30  vs2005 対応
//
//	[メモ]
//

/*************
public struct DataInfo
{
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 5)]
    public string strAAA;

    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 2)]
    public string strBBB;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 3)]
    public string strCCC;

}

string strData = "1234567890";

C++をC#にしたのですが、diDataInfo.strAAAの値は正常、diDataInfo.strBBBから値がおかしくなります。

正常なら、

diDataInfo.strAAA = "12345"
diDataInfo.strBBB = "67"
diDataInfo.strCCC = "890"

なのに、上で書いた内容では、
diDataInfo.strAAA = "12345"
diDataInfo.strBBB = "78"
diDataInfo.strCCC = "90"
となる。

ByValTStr だと必ず終端文字が加えられるのでその結果になる

の理由の為、stringからByte配列に変更

**********/


// *********************************************************************************
#pragma once

#include "KizuCommon.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace KizuLibCLI
{
	//***************************************************************
	// メールスロット通信用構造体
	//***************************************************************
	// ----------------------------------------------
	// データ構造体 (int基本型)
	[StructLayout(LayoutKind::Sequential, Size=SIZE_MSL, Pack=1)]
	//public ref struct COMMON_QUE_L {			// GC対象のクラス  となる。用はclass
	public value struct COMMON_QUE_L {			// GC不要の型      となる。用はstruct

		int nEventNo;											// イベントコード
		int nLineNo;											// ラインNo (通常は、LINE_ID固定。1PCで複数のシステムが同居している場合に 0は1RCL 1は2RCL用とかに使用する。)

		[MarshalAs(UnmanagedType::ByValArray, SizeConst=(SIZE_MSL_DATA)/sizeof(int))]
		array<int>^ data;
		
	public :
		void inisialize() {
			data = gcnew array<int>( (SIZE_MSL_DATA)/sizeof(int) );
		}
	};

	// ----------------------------------------------
	// データ構造体 (byte基本型) tModのクラスを使用すれば文字列に変換できる
	[StructLayout(LayoutKind::Sequential, Size=SIZE_MSL, Pack=1)]
	//public ref struct COMMON_QUE_S {
	public value struct COMMON_QUE_S {
		int nEventNo;
		int nLineNo;
/*
		// ↓ByValTStr これだと 終端に勝手にNULLが付与されるらしい。どうする？
		[MarshalAs(UnmanagedType::ByValTStr, SizeConst=SIZE_MSL_DATA)]
		String^ data;
*/

		[MarshalAs(UnmanagedType::ByValArray, SizeConst=(SIZE_MSL_DATA) )]
		array<Byte>^ data;

	public :
		void inisialize() {
			data = gcnew array<Byte>( (SIZE_MSL_DATA) );
		}
	};

	// ----------------------------------------------
	// データ構造体 (混合基本型) (最初の80byteが数値) tModのクラスを使用すれば文字列に変換できる
	[StructLayout(LayoutKind::Sequential, Size=SIZE_MSL, Pack=1)]
	//public ref struct COMMON_QUE_MIX {
	public value struct COMMON_QUE_MIX {
		int nEventNo;
		int nLineNo;

		[MarshalAs(UnmanagedType::ByValArray, SizeConst=20)]
		array<int>^ idata;

/*
		[MarshalAs(UnmanagedType::ByValTStr, SizeConst=32)]
		String^ cdata;

		[MarshalAs(UnmanagedType::ByValTStr, SizeConst=(SIZE_MSL_DATA-sizeof(int)*10-32))]
		String^ cdata2;
	
*/
		[MarshalAs(UnmanagedType::ByValArray, SizeConst=32)]
		array<Byte>^ cdata1;
		[MarshalAs(UnmanagedType::ByValArray, SizeConst=32)]
		array<Byte>^ cdata2;
		[MarshalAs(UnmanagedType::ByValArray, SizeConst=32)]
		array<Byte>^ cdata3;
		[MarshalAs(UnmanagedType::ByValArray, SizeConst=32)]
		array<Byte>^ cdata4;
		[MarshalAs(UnmanagedType::ByValArray, SizeConst=32)]
		array<Byte>^ cdata5;

		[MarshalAs(UnmanagedType::ByValArray, SizeConst=(SIZE_MSL_DATA-sizeof(int)*20-32*5) )]
		array<Byte>^ ydata;

	public :
		void inisialize() {
			idata = gcnew array<int>( 20 );
			cdata1 = gcnew array<Byte>( 32 );
			cdata2 = gcnew array<Byte>( 32 );
			cdata3 = gcnew array<Byte>( 32 );
			cdata4 = gcnew array<Byte>( 32 );
			cdata5 = gcnew array<Byte>( 32 );
			ydata = gcnew array<Byte>( (SIZE_MSL_DATA-4*20-32*5) );
		}
	}; 

	// ----------------------------------------------
	// データ構造体 (string基本型)
	[StructLayout(LayoutKind::Sequential, Size=SIZE_MSL, Pack=1)]
	public value struct COMMON_QUE_STR {
		int nEventNo;
		int nLineNo;

		// ↓ByValTStr これだと 終端に勝手にNULLが付与される為、終端1バイトが削除される。
		[MarshalAs(UnmanagedType::ByValTStr, SizeConst=SIZE_MSL_DATA)]
		String^ data;

	public :
		void inisialize() {
		}
	};

	// ----------------------------------------------
	// データ構造体 (混合基本型) (最初の40byteが数値)
	[StructLayout(LayoutKind::Sequential, Size=SIZE_MSL, Pack=1)]
	public value struct COMMON_QUE_STRMIX {
		int nEventNo;
		int nLineNo;
		
		[MarshalAs(UnmanagedType::ByValArray, SizeConst=20)]
		array<int>^ idata;

		// ↓ByValTStr これだと 終端に勝手にNULLが付与される為、終端1バイトが削除される。
		[MarshalAs(UnmanagedType::ByValTStr, SizeConst=32)]
		String^ cdata1;
		[MarshalAs(UnmanagedType::ByValTStr, SizeConst=32)]
		String^ cdata2;
		[MarshalAs(UnmanagedType::ByValTStr, SizeConst=32)]
		String^ cdata3;
		[MarshalAs(UnmanagedType::ByValTStr, SizeConst=32)]
		String^ cdata4;
		[MarshalAs(UnmanagedType::ByValTStr, SizeConst=32)]
		String^ cdata5;

		[MarshalAs(UnmanagedType::ByValTStr, SizeConst=(SIZE_MSL_DATA-sizeof(int)*20-32*5))]
		String^ ydata;

	public :
		void inisialize() {
			idata = gcnew array<int>( 20 );
		}
	}; 
}


