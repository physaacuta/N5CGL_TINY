// *********************************************************************************
//	�ʐM�֘A�̑���M���b�\�h�̃C���^�[�t�F�[�X
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//
// *********************************************************************************
#pragma once

#include <windows.h>
#include "KizuMacro.h"												// �}�N���W

namespace KizuLib
{
	// ����
	interface ICommunity
	//class ICommunity
	{
		virtual int Recv(LPVOID, DWORD) = 0;						// ��M
		virtual int Send(LPCVOID, DWORD) = 0;						// ���M
	};
	// �񓯊�
	interface ICommunityAsync
	//class ICommunityAsync
	{
		virtual bool Recv(LPVOID, DWORD, OVERLAPPED*) = 0;			// ��M
		virtual bool Send(LPCVOID, DWORD, OVERLAPPED*) = 0;			// ���M
		virtual bool GetResult(OVERLAPPED*) = 0;					// �񓯊����ʖ₢���킹
		virtual int GetError() = 0;									// �ŏI�ُ�
	};

};