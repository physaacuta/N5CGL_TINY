// *********************************************************************************
//	���L������ �N���X
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//		
// *********************************************************************************

#pragma once

#pragma warning(disable : 4996)							// ***���Â��`���Ƃ��Đ錾����܂����B���o���Ȃ��΍�

#include <Windows.h>

//#include "..\..\..\Include\KsCommon.h"		// �r�����ʃw�b�_�[


namespace KizuLib
{
	class SmemManager  
	{
	private:
		#define KIZULIB_SMEMMANAGER_MAX_NUM				50			// �Ǘ��\���L��������
		#define KIZULIB_SMEMMANAGER_SESSION				"MEM"		// ini�t�@�C���Z�b�V����

	public:
		SmemManager();
		virtual ~SmemManager();

		void CreateMem(bool clear = true);					// ���L����������
		void DeleteMem();									// ���L�������j��

		LPVOID GetAddress(int id) const { return m_pMapped[id]; }		// �w�肳�ꂽ�}�b�s���O�A�h���X

	private:
		int		m_nNum;										// ���L�������𐶐�������
		HANDLE	m_hMapFile[KIZULIB_SMEMMANAGER_MAX_NUM];	// �n���h��
		LPVOID	m_pMapped[KIZULIB_SMEMMANAGER_MAX_NUM];		// �}�b�s���O�A�h���X
	};
};

