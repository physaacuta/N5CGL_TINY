// *********************************************************************************
//	LOG�񓯊������� �N���X
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή� (�R���X�g���N�^�ŃT�C�Y�w��ɕύX)
//
//	[����]
//		MFC��ListBox�ɂ� ���O��\�����鎖���\
// *********************************************************************************

#if !defined(AFX_LOGFILEMANAGER_H__CA3C4798_AC15_40E3_9772_335FAA1A0EF5__INCLUDED_)
#define AFX_LOGFILEMANAGER_H__CA3C4798_AC15_40E3_9772_335FAA1A0EF5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable : 4996)							// ***���Â��`���Ƃ��Đ錾����܂����B���o���Ȃ��΍�

#include <fstream>	
#include "..\..\KizuLib\Class\ThreadQueue.h"					// �L���[�N���X
#include "..\..\KizuLib\Class\WorkThread.h"						// ���[�J�[�X���b�h�N���X
#include "..\..\KizuLib\Class\AutoLock.h"						// �N���e�B�J���Z�N�V�����N���X

namespace KizuLib
{
	class LogFileManager : public WorkThread
	{
	private:
		
		static const int KIZULIB_LOGFILEMANAGER_MAX_ROW_NUM	= 20000;		// 1�t�@�C��������̍s��
		static const int KIZULIB_LOGFILEMANAGER_MAX_LISTBOX	= 200;			// ���X�g�{�b�N�X�\���s��
		static const int KIZULIB_LOGFILEMANAGER_MAX_QUE		= 512;			// ���O�t�@�C���ő吔

	public:
		
		// //////////////////////////////////////////////////////////////////////////////
		// �萔��`
		// //////////////////////////////////////////////////////////////////////////////
		//// �󂯓n���\���̂̔ԍ�
		typedef enum LNO {
			em_NON = 0,							// ����
			em_MSG,								// �� 
			em_ERR,								// ��
			em_INF,								// �� (�ʏ� ���ʃN���X���ŏo��)
			em_WAR,								// �� (�ʏ� ���ʃN���X���ŏo��)
			em_FIL								// �� 
		}lNo;


		// //////////////////////////////////////////////////////////////////////////////
		// ���\�b�h
		// //////////////////////////////////////////////////////////////////////////////
		LogFileManager(LPCSTR filename, CListBox *list = NULL, int cycleCnt=3, int backCnt=4);		// �R���X�g���N�^
		virtual ~LogFileManager();											// �f�X�g���N�^

		// ���O�o��
		void Write( LogFileManager::LNO no, LPCSTR __cBuffer, ... );		// �񓯊��������݃G���g���[ (�ψ����Ή�)
		void WriteLock( LogFileManager::LNO no, LPCSTR __cBuffer, ... );	// ���������� (�ψ����Ή�)
		int AllGetQueue();													// �S�L���[�����o��
		int AllDeleteQueue();												// �S�L���[�����폜

		// ���̑��������
		void SetListOut(bool umu) {m_bListOut = umu; AllGetQueue(); Sleep(100); };	// ���X�g�{�b�N�X�R���g���[���o�͗L��
		void SetFileOut(bool umu) {m_bFileOut = umu;}								// �t�@�C���o�͗L��
		CString GetNowLogPath() {return m_cNowFPath;}						// ���݂̃��O�t�@�C���̐�΃p�X��Ԃ�
		void ShowEdit();													// �������N��
		void WriteFile(LPCSTR msg);											// �t�@�C���ɏ�������

	private:
		// �񓯊��֘A
		static UINT WINAPI MainThread(LPVOID param);						// ���C���X���b�h
		void Start() { WorkThread::Start(MainThread); };					// �X���b�h�J�n
		void Stop()  { WorkThread::Stop(10000); };							// �X���b�h�I��


		// ���O�o�͊֘A
		void WriteList(LPCSTR msg);											// ���X�g�{�b�N�X�ɏ�������

		
		// ���O�o�͐���
		//ofstream				m_stream;									// �X�g���[��	// VS6
		std::ofstream			m_stream;									// �X�g���[��	// VS2003�ȍ~
		CListBox*				m_cntList;									// ���X�g�{�b�N�X

		CRITICAL_SECTION		m_csOutPut;									// �N���e�B�J���Z�N�V����
		CRITICAL_SECTION		m_csANC;									// �񓯊��p�N���e�B�J���Z�N�V����

		ThreadQueue<char>		mque_Log;									// ���O�L���[
		bool					m_bFileOut;									// �t�@�C���o�� (true:�t�@�C���o�͗L��)
		bool					m_bListOut;									// ���X�g�{�b�N�X�R���g���[���o�� (true:���X�g�o�͗L��)

		// ���O�t�@�C���֘A
		CString					m_cBaseFolder;								// �x�[�X�̃t�H���_
		CString					m_cBaseFile;								// �x�[�X�̃t�@�C����
		CString					m_cNowFPath;								// ���ݏo�͒��̃t�@�C���t���p�X

		DWORD					m_nNowFileNo;								// �t�@�C����
		DWORD					m_nNowRow;									// �s��
		int						m_nMaxCycleCnt;								// �ő�T�C�N���b�N�t�@�C������
		int						m_nMaxBackCnt;								// �ő�o�b�N�A�b�v����
	
	};
};
#endif // !defined(AFX_LOGFILEMANAGER_H__CA3C4798_AC15_40E3_9772_335FAA1A0EF5__INCLUDED_)
