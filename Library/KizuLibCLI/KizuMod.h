// *********************************************************************************
//	VB�p���C�u�����֐�
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//
// *********************************************************************************
#pragma once

#include <windows.h>
#include "..\KizuLib\KizuLib.h"											// �r�����C�u�����w�b�_�[
#include "..\..\Include\KizuCommon.h"									// �r�����ʃw�b�_�[
#include "..\..\Include\KizuCommonCLI.h"								// �r��CLI���ʃw�b�_�[


using namespace System;
using namespace System::Runtime::InteropServices;


namespace KizuLibCLI
{
	// ���[����M���̃C�x���g
	public delegate void EvRecvMailSlot(Object^ pData, int nEvtNo);

	/// �r�����C�u���� (C++��VB�̎󂯓n��)
	public ref class KizuMod
	{
	public:

		KizuMod();																// �R���X�g���N�^
		virtual ~KizuMod();														// �f�X�g���N�^


		// ���[���X���b�g�p
		event		EvRecvMailSlot^	evRecvMail;									// ���[���X���b�g��M�C�x���g	 (�f���Q�[�g)				
		int			Task_Init(String^ cTaskName);								// �^�X�N����
		int			Task_Init(String^ cTaskName, int nPCID);					// �^�X�N����
		void		Task_Exit();												// �^�X�N�I��
		static int	Send_Mail(String^ SendSlotName, String^ SendPcName, Object^ pDate);	// ���[�����M
		//int	Recv_Mail(Object^% pDate, Type^ structureType);					// ���[����M (���܂�g��Ȃ�)
		static int	Send_Mail_Trans(String^ SendSlotName, String^ SendPcName, Object^ pDate);	// ���[���]��
		static int	Send_Mail_Trans(String^ SendUdpAddr, String^ SendSlotName, String^ SendPcName, Object^ pDate);	// ���[���]��
		void		typConv(Object^ pTo, Object^ pFrom);						// �N���X�ϊ� (new����K�v�L��)
		ValueType^	typConv(Object^ pFrom, Type^ structureType);				// �\���̕ϊ� (new����K�v����)


		// ���̑����C�u�����A�N�Z�X
		static bool GetPcName()				{ return getpcname(); };			// �SPC���̂�DB���擾
		static String^ GetMyPcName()		{ return Marshal::PtrToStringAnsi( IntPtr( (char*)getmypcname() )); };	// ��PC���̎擾
		static int	SysLog(int nSysNo, String^ cMsg);							// �V�X�e�����O�o��
		static void SetStatus(int id, int state);								// DB_SYSTEM_STATE�ɋ@���Ԃ��Z�b�g���� (0:����)
		static void SetStatus(int id, bool state);								// DB_SYSTEM_STATE�ɋ@���Ԃ��Z�b�g���� (true:����)
		static void SetStatus(int id, int state, bool bLogDsp);					// DB_SYSTEM_STATE�ɋ@���Ԃ��Z�b�g���� (0:����)
		static void SetStatus(int id, bool state, bool bLogDsp);				// DB_SYSTEM_STATE�ɋ@���Ԃ��Z�b�g���� (true:����)


		static bool			ksMemCreate(String^ cTblName, int size);			// ���L������ ����
		static void			ksMemClose(String^ cTblName);						// ���L������ �J��


		static ValueType^	ksMemRead(String^ cTblName, Type^ structureType);								// ���L������ �f�[�^�Ǎ� (������g���Ȃ� �����g������)
		static ValueType^	ksMemRead(String^ cTblName, int startIndex, Type^ structureType, int^% nRetc);	// ���L������ �f�[�^�Ǎ� (������g���Ȃ� �����g������)
		static bool			ksMemRead(String^ cTblName, int startIndex, ValueType^% p);						// ���L������ �f�[�^�Ǎ� (�\���̓Ǎ���) �� ��{������g�p���邱��

		static bool			ksMemRead(String^ cTblName, int size, array<unsigned char>^ byteVal);						// ���L������ �f�[�^�Ǎ� (�o�C�g�z��Ǎ���)					�� �\���̓Ǎ��݂��\�ƂȂ������߁A�g���K�v�قږ���
		static bool			ksMemRead(String^ cTblName, int startIndex, int size, array<unsigned char>^ byteVal);		// ���L������ �f�[�^�Ǎ� (�o�C�g�z��Ǎ���) �� �\���̓Ǎ��݂��\�ƂȂ������߁A�g���K�v�قږ���

		static bool			ksMemWrite(String^ cTblName, ValueType^ tbl);												// ���L������ �f�[�^������

		// ���L�����������̂݃}�b�s���O (�Q�Ƃ���e�[�u�����傫���ꍇ�́A������g�p���邱��)
		static ValueType^	ksMemReadMap(String^ cTblName, int startIndex, int size, Type^ structureType, int^% nRetc);	// ���L������ �f�[�^�Ǎ� (������g���Ȃ� �����g������)
		static bool			ksMemReadMap(String^ cTblName, int startIndex, int size, ValueType^% p);					// ���L������ �f�[�^�Ǎ� (�\���̓Ǎ���) �� ��{������g�p���邱��

		static bool			ksMemWriteMap(String^ cTblName, int startIndex, int size, ValueType^ tbl);					// ���L��������̃f�[�^������


		// ���܂���
		static int			GetPcID()		{ return getpcid(); }				// ��PCID (1�I���W��)
		static void			SetLineID()		{ setlineid(); }					// �����C��ID�Z�b�g
		static void			SetLineID(int nLineID) { setlineid(nLineID);}		// �����C��ID�Z�b�g
		static int			GetLineID()		{ return getlineid(); }				// �����C��ID�擾
		static String^		GetMyTaskName() { return Marshal::PtrToStringAnsi( IntPtr( (char*)getmytaskname() )); }	// ���^�X�N����

		// PC���̎擾�p
		static String^		Pcid_Name(int pcid) {return Marshal::PtrToStringAnsi(IntPtr( PCID_NAME(pcid) )); }	// PC���� (pcid 1�I���W��) Ver.1.1.1.0

		static String^		Hantei(int no)	{ return Marshal::PtrToStringAnsi(IntPtr( HANTEI(no) )); }	// ����@�\��L����PC����
		static String^		Tokatu()		{ return Marshal::PtrToStringAnsi(IntPtr( TOKATU )); }		// �����@�\��L����PC����
		static String^		Tikuseki()		{ return Marshal::PtrToStringAnsi(IntPtr( TIKUSEKI )); }	// �~�ϋ@�\��L����PC����
		static String^		Sousa()			{ return Marshal::PtrToStringAnsi(IntPtr( SOUSA )); }		// ����@�\��L����PC����
		static String^		Hyouzi(int no)	{ return Marshal::PtrToStringAnsi(IntPtr( HYOUZI(no) )); }	// �\���@�\��L����PC����

		static String^		MasterPcName()	{ return Marshal::PtrToStringAnsi(IntPtr( MASTER_PC_NAME )); }	// �r����ԊǗ������PC����
		static String^		SyslogPcName()	{ return Marshal::PtrToStringAnsi(IntPtr( SYSLOG_PC_NAME )); }	// syslog�����PC����
		
	private:
		// ���[���X���b�g�p
		static void MainThread(Object^ arg);									// ��M�p�X���b�h
		Threading::Thread ^ mcls_thread;										// ��M�p�X���b�h�C���X�^���X
		Threading::AutoResetEvent ^	m_evStop;									// ��M�p�X���b�h��~

	};
}