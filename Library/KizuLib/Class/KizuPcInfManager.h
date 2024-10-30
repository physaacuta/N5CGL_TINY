// *********************************************************************************
//	PC�Ǘ��N���X
//	[Ver]
//		Ver.01    2007/04/01  vs2005 �Ή�
//
//	[����]
//
// *********************************************************************************

#pragma once

#include <windows.h>

#include "..\..\..\include\KizuCommon.h"									// �r�����ʃw�b�_�[
#include "..\..\KizuODBC\OdbcBase.h"										// DB����N���X

using namespace KizuODBC;

namespace KizuLib
{
	class KizuPcInfManager
	{
	public:
		KizuPcInfManager(int nPCID);
		virtual ~KizuPcInfManager(void);

		bool SetPcName(char *pcname, LPCSTR cIniPath, LPCSTR cSession = NULL);			// PC_INF����PC��ʂ��擾

		// �O���A�N�Z�X
		char const* GetMyPcName() const {return m_cMyPcName; }				// ��PC��
		int GetPcMode() const {return m_nPcMode;}							// �敪 (0:online 1:offline)
		int GetPcKind() const {return m_nPcKind;}							// PCKIND
		char const* GetPcKindName() const {return m_cPcKindName; }			// PC���

		// PC_INF �A�N�Z�X
		static bool GetGroupPcID(LPCSTR cIniPath, int groupno, int maxcnt, int* count, int* nPcid, bool bTablePcinf = false);	// �w��O���[�v��PCID���擾
		static bool SetGroupPcID(LPCSTR cIniPath, int groupno, int count, int const* nPcid );		// �w��O���[�v��PCID���Z�b�g����  (PC_INF)


	private :
		// �������g�̏��
		int						m_nPcId;									// PCID
		char					m_cMyPcName[SIZE_NAME_PC];					// ��PC��
		int						m_nPcMode;									// �敪 (0:online 1:offline)
		int						m_nPcKind;									// PCKIND
		char					m_cPcKindName[32];							// PC���
	};
};