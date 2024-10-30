// *********************************************************************************
//	PC管理クラス
//	[Ver]
//		Ver.01    2007/04/01  vs2005 対応
//
//	[メモ]
//
// *********************************************************************************

#pragma once

#include <windows.h>

#include "..\..\..\include\KizuCommon.h"									// 疵検共通ヘッダー
#include "..\..\KizuODBC\OdbcBase.h"										// DB操作クラス

using namespace KizuODBC;

namespace KizuLib
{
	class KizuPcInfManager
	{
	public:
		KizuPcInfManager(int nPCID);
		virtual ~KizuPcInfManager(void);

		bool SetPcName(char *pcname, LPCSTR cIniPath, LPCSTR cSession = NULL);			// PC_INFからPC種別を取得

		// 外部アクセス
		char const* GetMyPcName() const {return m_cMyPcName; }				// 自PC名
		int GetPcMode() const {return m_nPcMode;}							// 区分 (0:online 1:offline)
		int GetPcKind() const {return m_nPcKind;}							// PCKIND
		char const* GetPcKindName() const {return m_cPcKindName; }			// PC種別

		// PC_INF アクセス
		static bool GetGroupPcID(LPCSTR cIniPath, int groupno, int maxcnt, int* count, int* nPcid, bool bTablePcinf = false);	// 指定グループのPCIDを取得
		static bool SetGroupPcID(LPCSTR cIniPath, int groupno, int count, int const* nPcid );		// 指定グループのPCIDをセットする  (PC_INF)


	private :
		// 自分自身の情報
		int						m_nPcId;									// PCID
		char					m_cMyPcName[SIZE_NAME_PC];					// 自PC名
		int						m_nPcMode;									// 区分 (0:online 1:offline)
		int						m_nPcKind;									// PCKIND
		char					m_cPcKindName[32];							// PC種別
	};
};