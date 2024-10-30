// *********************************************************************************
//	�}�l�[�W�h�R�[�h�p�C���N���[�h
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
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

C++��C#�ɂ����̂ł����AdiDataInfo.strAAA�̒l�͐���AdiDataInfo.strBBB����l�����������Ȃ�܂��B

����Ȃ�A

diDataInfo.strAAA = "12345"
diDataInfo.strBBB = "67"
diDataInfo.strCCC = "890"

�Ȃ̂ɁA��ŏ��������e�ł́A
diDataInfo.strAAA = "12345"
diDataInfo.strBBB = "78"
diDataInfo.strCCC = "90"
�ƂȂ�B

ByValTStr ���ƕK���I�[��������������̂ł��̌��ʂɂȂ�

�̗��R�ׁ̈Astring����Byte�z��ɕύX

**********/


// *********************************************************************************
#pragma once

#include "KizuCommon.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace KizuLibCLI
{
	//***************************************************************
	// ���[���X���b�g�ʐM�p�\����
	//***************************************************************
	// ----------------------------------------------
	// �f�[�^�\���� (int��{�^)
	[StructLayout(LayoutKind::Sequential, Size=SIZE_MSL, Pack=1)]
	//public ref struct COMMON_QUE_L {			// GC�Ώۂ̃N���X  �ƂȂ�B�p��class
	public value struct COMMON_QUE_L {			// GC�s�v�̌^      �ƂȂ�B�p��struct

		int nEventNo;											// �C�x���g�R�[�h
		int nLineNo;											// ���C��No (�ʏ�́ALINE_ID�Œ�B1PC�ŕ����̃V�X�e�����������Ă���ꍇ�� 0��1RCL 1��2RCL�p�Ƃ��Ɏg�p����B)

		[MarshalAs(UnmanagedType::ByValArray, SizeConst=(SIZE_MSL_DATA)/sizeof(int))]
		array<int>^ data;
		
	public :
		void inisialize() {
			data = gcnew array<int>( (SIZE_MSL_DATA)/sizeof(int) );
		}
	};

	// ----------------------------------------------
	// �f�[�^�\���� (byte��{�^) tMod�̃N���X���g�p����Ε�����ɕϊ��ł���
	[StructLayout(LayoutKind::Sequential, Size=SIZE_MSL, Pack=1)]
	//public ref struct COMMON_QUE_S {
	public value struct COMMON_QUE_S {
		int nEventNo;
		int nLineNo;
/*
		// ��ByValTStr ���ꂾ�� �I�[�ɏ����NULL���t�^�����炵���B�ǂ�����H
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
	// �f�[�^�\���� (������{�^) (�ŏ���80byte�����l) tMod�̃N���X���g�p����Ε�����ɕϊ��ł���
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
	// �f�[�^�\���� (string��{�^)
	[StructLayout(LayoutKind::Sequential, Size=SIZE_MSL, Pack=1)]
	public value struct COMMON_QUE_STR {
		int nEventNo;
		int nLineNo;

		// ��ByValTStr ���ꂾ�� �I�[�ɏ����NULL���t�^�����ׁA�I�[1�o�C�g���폜�����B
		[MarshalAs(UnmanagedType::ByValTStr, SizeConst=SIZE_MSL_DATA)]
		String^ data;

	public :
		void inisialize() {
		}
	};

	// ----------------------------------------------
	// �f�[�^�\���� (������{�^) (�ŏ���40byte�����l)
	[StructLayout(LayoutKind::Sequential, Size=SIZE_MSL, Pack=1)]
	public value struct COMMON_QUE_STRMIX {
		int nEventNo;
		int nLineNo;
		
		[MarshalAs(UnmanagedType::ByValArray, SizeConst=20)]
		array<int>^ idata;

		// ��ByValTStr ���ꂾ�� �I�[�ɏ����NULL���t�^�����ׁA�I�[1�o�C�g���폜�����B
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


