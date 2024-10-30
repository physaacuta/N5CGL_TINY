// *********************************************************************************
//	�@���ԃ}�l�[�W���[
//	[Ver]
//		Ver.01    2007/07/16  vs2005 �Ή�
//		Ver.02	  2009/02/16  STATUS_MASTER�̃V�X�e���͈͒ǉ��ɑΉ�
//		Ver.03	  2009/03/04  �񓯊��̃X�e�[�^�X�������݂ɑΉ�
//		Ver.04	  2009/06/24  ����ł̃��b�Z�[�W�o�͂ɑΉ�
//		Ver.05	  2013/04/30  LOG_MASTER�̃I�y�K�C�L���ɑΉ�
//
//	[����]
//		DB��STATUS_MASTER��STATUS_INF��ǂݍ��݁A�@���Ԃ��`�F�b�N����
// *********************************************************************************
#pragma once

// ���Œ�
#include "..\..\Include\KizuCommon.h"									// �r���C���N���[�h
#include "..\KizuLib\KizuLib.h"											// �r�����C�u����
#include "..\KizuLib\Class\KizuMacro.h"									// �}�N���W
#include "..\KizuLibMFC\Class\LogFileManager.h"							// ���O�Ǘ��N���X

// �r���N���X �C���N���[�h
#include "..\KizuLib\Class\WorkThread.h"								// ���[�J�[�X���b�h�N���X
#include "..\KizuLib\Class\AutoLock.h"									// �N���e�B�J���Z�N�V�����N���X
#include "..\KizuLib\Class\KizuPerformanceCounter.h"					// ���ԑ���N���X

#include "..\KizuODBC\OdbcBase.h"										// DB����N���X

using namespace KizuLib;
using namespace KizuODBC;


class KikiManager : public WorkThread
{
private:
	static const int STATUS_OK_MSG_YUUSENID_MIN = 900;					// �@���ԁE������� ����ł����̗D��x�ȉ��ł���΁A���b�Z�[�W�o�� Ver.04
	static const int STATUS_OK_MSG_YUUSENID_MAX = 999;

	struct DELIVERYSTATUS {											// �X�e�[�^�X�ύX�󂯓n���\����
		int		id;														// int id �@��ID
		int		state;													// �@���� (0:����  1�`:�ُ�)
		bool	bLogDsp;												// �V�X���O�o�̗͂L��
		CString	strMsg;													// �ǉ����b�Z�[�W
	};

public:
	KikiManager(int nCycleTime, LPCSTR cIniPath=TASKINI_NAME, LPCSTR cSession=INI_DB);
	virtual ~KikiManager(void);

	void Start() { WorkThread::Start(MainThread); };					// �X���b�h�J�n
	void Stop()  { WorkThread::Stop(10000); };							// �X���b�h�I��
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ���O�t�@�C���Z�b�g

	void SetOutPutLog() { m_bOutPutLog = true; }						// LOG_MASTER��[��߶޲�L��]�Ή��� true

	//// �O���A�N�Z�X�p
	void	SetKadou(bool isRun) { m_bIsKadou = isRun; }				// �ғ����
	HANDLE	GetEvReadEnd() const { return m_evReadEnd; }				// �񓯊��Ǎ��݊����C�x���g
	void	Refresh();													// �����X�V (�Ǎ��݊�������܂Ńu���b�N�����)
	void	GetStatusInf(TYP_STATUS_INF* buf, int size) {				// �@���Ԉꗗ�擾
				AutoLock lock(&m_csLock);
				memcpy(buf, m_typStatInf, size);
			}		
	void	GetStatusNow(TYP_STATUS_NOWSYSTEM* buf) {					// ���ُ��Ԏ擾
				AutoLock lock(&m_csLock);
				memcpy(buf, &m_typStatNow, sizeof(m_typStatNow));
			}
			
	void	CheckBitSysArea(int bitPattan) {m_nSysArea_BitPattan = bitPattan; }	// �ΏۂƂȂ�V�X�e���͈͂̃r�b�g�p�^�[�����Z�b�g Ver.02

	bool CheckStatus();													// �@���Ԏ擾 (�Z�[�t�X���b�h�ł͖����ג��ӂ��邱��)


	//// �@���� �񓯊���������
	void SetStatusQue(int id, int state, bool bLogDsp =true, char const * cMsg = "" );			// DB_SYSTEM_STATE�ɋ@���Ԃ��Z�b�g���� (0:����)
	void SetStatusQue(int id, bool state, bool bLogDsp =true, char const * cMsg  = "" );		//										 (true:����)
	void SetStatusQue(char const * cKey, bool state, bool bLogDsp =true, char const * cMsg  = "" );// ini�t�@�C������@��ID���擾���� (true:����)
	void SetStatusQue(char const * cKey, int state, bool bLogDsp =true, char const * cMsg  = "" );// ini�t�@�C������@��ID���擾���� (true:����)

	//// �@���� �񓯊��ǂݍ��ݗv��
	void SetStatusRead() { SetEvent(m_evSyncRead); }					// (�񓯊�)�����Ǎ��݊J�n�C�x���g�B����������ʏ�ʂ� m_evReadEnd ���V�O�i���ƂȂ�


private :
	static UINT WINAPI MainThread(LPVOID param);						// ���C���X���b�h
	bool SetStatusThread(DELIVERYSTATUS* pDeli);						// �X�e�[�^�X �񓯊���������
	void GetLogInfo(TYP_STATUS_NOWSYSTEM* inf);							// �I�y�K�C���郁�b�Z�[�W���擾

	bool ConnectDB(bool bLogDsp);										// DB�ڑ�
	void CloseDB();														// DB�ؒf


	// �e�C���X�^���X
	LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X
	OdbcBase*				mcls_pDB;									// ODBC����N���X
//	KizuPerformanceCounter mcls_Counter;								// ���ԑ���X���b�h
	
	// �����n���h��
	HANDLE					m_evTimeCycleRead;							// ������ǂݍ���
	HANDLE					m_evReadEnd;								// �񓯊��Ǎ��݊����C�x���g
	HANDLE					m_evLockReadStart;							// (����)�����Ǎ��݊J�n�C�x���g
	HANDLE					m_evLockReadEnd;							// (����)�����Ǎ��݊����C�x���g
	HANDLE					m_evSyncRead;								// (�񓯊�)�����Ǎ��݊J�n�C�x���g

	// �N���e�B�J���Z�N�V����
	CRITICAL_SECTION		m_csLock;									// ��񑀍샍�b�N�p

	// �󂯓n���L���[
	ThreadQueue<DELIVERYSTATUS> mque_Deli;								// �X�e�[�^�X�ύX �󂯓n���L���[


	// �@���ԓǍ��֘A
	TYP_STATUS_INF			m_typStatInf[MAX_STATUS];					// DB�擾�̋@���Ԉꗗ
	TYP_STATUS_NOWSYSTEM	m_typStatNow;								// DB�擾�̌��ُ���
	int						m_nCycleTime;								// DB�Ǎ����� [ms]
	int						m_nMsgReadCnt;								// ���b�Z�[�W��Ǎ��ރ^�C�~���O
	bool					m_bOutPutLog;								// LOG_MASTER��[��߶޲�L��]�Ή��� true		//Ver.05

	// ���̑��������
	char					m_cIniPath[128];							// �ڑ��Ώۂ�Ini�t�@�C���p�X
	char					m_cIniSession[128];							// �ڑ��Ώۂ̃Z�b�V����
	bool					m_bDBConnect;								// DB�ڑ���� (true:�ڑ���)
	bool					m_bIsKadou;									// �ғ���� (true:������ false:��~��) ��~���́A������Ԃ𔻒f���Ȃ�

	int						m_nSysArea_BitPattan;						// �ΏۂƂȂ�V�X�e���͈͂̃r�b�g�p�^�[�� (0�̎��́A�V�X�e���͈� ���Ή��ƔF��) Ver.02
};
