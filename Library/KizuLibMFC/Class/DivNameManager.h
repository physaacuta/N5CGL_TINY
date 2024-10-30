// *********************************************************************************
//	各種 DIV 定数の名称を取得するクラス
//	[Ver]
//		Ver.01    2007/08/13  vs2005 対応
//
//	[メモ]
//		ほとんど デバック用
// *********************************************************************************
#pragma once

//#include <windows.h>
//#pragma warning(disable : 4996)							// ***が古い形式として宣言されました。を出さない対策

#include "..\..\..\include\KizuCommon.h"									// 疵検共通ヘッダー

namespace KizuLib
{
	class DivNameManager
	{
	public:
		static CString GetDivBool(EM_DIV_BOOL em) ;							// 拡張BOOL
		static CString GetDivPlg(EM_DIV_PLG em) ;							// PLG区分
		static CString GetDivUnten(EM_DIV_UNTEN em) ;						// 運転状態
		static CString GetDivSogyo(EM_DIV_SOGYO em) ;						// 操業状態
		static CString GetDivKadou(EM_DIV_KADOU em) ;						// 稼動状態
		static CString GetDivKensa(EM_DIV_KENSA em) ;						// 検査状態
		static CString GetDivKiki(EM_DIV_KIKI em) ;							// 機器状態
		static CString GetDivCamSet(EM_DIV_CAMSET em) ;						// カメラセット名称

		static CString GetDivBool(int em)  {return GetDivBool((EM_DIV_BOOL)em); };			// 拡張BOOL
		static CString GetDivPlg(int em)   {return GetDivPlg((EM_DIV_PLG)em); };			// PLG区分
		static CString GetDivUnten(int em) {return GetDivUnten((EM_DIV_UNTEN)em); };		// 運転状態
		static CString GetDivSogyo(int em) {return GetDivSogyo((EM_DIV_SOGYO)em); };		// 操業状態
		static CString GetDivKadou(int em) {return GetDivKadou((EM_DIV_KADOU)em); };		// 稼動状態
		static CString GetDivKensa(int em) {return GetDivKensa((EM_DIV_KENSA)em); };		// 検査状態
		static CString GetDivKiki(int em)  {return GetDivKiki((EM_DIV_KIKI)em); };			// 機器状態
		static CString GetDivCamSet(int em)  {return GetDivCamSet((EM_DIV_CAMSET)em); };	// カメラセット名称

		//static CString GetDivTorB(bool ou) ;								// 表/裏
		static CString GetTorB(int ou) {return 0==ou?"表面":"裏面";}		// 表/裏

	};
};

