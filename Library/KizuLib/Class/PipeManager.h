// *********************************************************************************
//	Pipe �N���X (�N���C�A���g���̂�)
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//		Ver.02	  2010/03/02  �T�[�o�[���[�h�ɑΉ�
//
//	[����]
//		�����瑤���T�[�o�[�ƂȂ��āACreateNamedPipe�����s���鏈���͎������ĂȂ��ג���
// *********************************************************************************

#pragma once

#include "ICommunity.h"										// �ʐM�֘A�̑���M���b�\�h�̃C���^�[�t�F�[�X

namespace KizuLib
{
	class PipeManager : implements ICommunity, 
						implements ICommunityAsync
	{
	public:
		PipeManager(LPCSTR PipeName, bool bIsServer=false);
		virtual ~PipeManager();

		void Cancel() const;												// ��������I/O��������
		DWORD Listen(OVERLAPPED *ovl);										// �N���C�A���g����̐ڑ����󂯕t����B(��u���b�N)


		// ICommunity�̎���
		virtual int Recv(LPVOID pDate, DWORD len);							// ��M
		virtual int Send(LPCVOID pDate, DWORD len);							// ���M

		// ICommunityAsync�̎���
		virtual bool Recv(LPVOID pDate, DWORD len, OVERLAPPED *ovl);		// ��M
		virtual bool Send(LPCVOID pDate, DWORD len, OVERLAPPED *ovl);		// ���M
		virtual bool GetResult(OVERLAPPED *ovl);							// �񓯊����ʖ₢���킹
		virtual int GetError()	{ return m_nLastError; }					// �ŏI�G���[�擾 (���ۃN���X���܂ވ�const�����Ⴞ��)

		// �v���p�e�B
		HANDLE		GetPipeHandle()	{ return m_hPipe; }

	private:
		HANDLE					m_hPipe;									// �p�C�v�{��
		bool					m_bIsServer;								// Server��true
		int						m_nLastError;								// �ŏI�G���[
	};
};
