// *********************************************************************************
//	MaliSlot�A�N�Z�X�p���C�u�����֐�
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//
// *********************************************************************************
#pragma once

#include "..\KizuLib\KizuLib.h"
#include "..\..\Include\KizuCommonCLI.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace KizuLibCLI
{
	// ���[����M���̃C�x���g
	public delegate void EvRecvMailSlot(Object^ pData, int nEvtNo);

	/// �r�����C�u���� (C++��VB�̎󂯓n��)
	public ref class KizuMail
	{

	public :

		event EvRecvMailSlot^	evRecvMail;										// ���[���X���b�g��M�C�x���g	 (�f���Q�[�g)				

		KizuMail();																// �R���X�g���N�^
		virtual ~KizuMail();													// �f�X�g���N�^

		// ���[��
		int Task_Init(String^ cTaskName);	
		int Task_Init(String^ cTaskName, String^ cPcName);						// �^�X�N����
		void Task_Exit();														// �^�X�N�I��
		static int Send_Mail(String^ SendSlotName, String^ SendPcName, Object^ pDate);	// ���[�����M
		//int	Recv_Mail(Object^% pDate, Type^ structureType);						// ���[����M (���܂�g��Ȃ�)
	
		void typConv(Object^ pTo, Object^ pFrom);								// �N���X�ϊ� (new����K�v�L��)
		Object^ typConv(Object^ pFrom, Type^ structureType);					// �\���̕ϊ� (new����K�v����)

	private:
		static void MainThread(Object^ arg);									// ��M�p�X���b�h

		Threading::Thread ^ mcls_thread;										// ��M�p�X���b�h�C���X�^���X
		Threading::AutoResetEvent ^	m_evStop;									// ��M�p�X���b�h��~
	};
}
