#pragma once

// ���Œ�
#include "..\..\Include\KizuCommon.h"									// �r���C���N���[�h
#include "..\..\Library\KizuLib\KizuLib.h"								// �r�����C�u����
#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// �}�N���W
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// �X�^�e�B�b�N���C�u�����W
#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ���O�Ǘ��N���X

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLib\Class\WorkThread.h"						// ���[�J�[�X���b�h�N���X
#include "..\..\Library\KizuLib\Class\ThreadQueue.h"					// �X���b�h�L���[�N���X

using namespace KizuLib;

class EvtlogManager : public WorkThread
{
private:
	static const int MSG_CHECK_CNT				= 50;					// ���b�Z�[�W�`�F�b�N����
	static const int PARAM_TIME_CYCLE			= 5*60*1000;			// ������^�C�}�[

public:
	enum EM_EVLOGNO {													// �C�x���g���O���
		em_APPLICATION = 0,														// Application
		em_SYSTEM,																// System
		em_SECURITY,															// Security
		em_END																	// �_�~�[
	};			


public:
	EvtlogManager(void);
	virtual ~EvtlogManager(void);

	void Start() { WorkThread::Start(MainThread); };					// �X���b�h�J�n
	void Stop()  { WorkThread::Stop(3000); };							// �X���b�h�I��
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ���O�t�@�C���Z�b�g

	void ResetErr();													// �ُ��Ԃ���������

protected :
	static UINT WINAPI MainThread(LPVOID param);						// ���C���X���b�h
	
	bool CheckEventLog();												// �C�x���g���O����ُ��Ԃ𔻒�
	CString DispMessage( EM_EVLOGNO emEvLogNo, const char *SourceName, 
		const char **Args, DWORD MessageId) const;	// ���b�Z�[�W�̎擾
	bool GetModuleNameFromSourceName( EM_EVLOGNO emEvLogNo, const char *SourceName,			
		const char *EntryName, char *ExpandedName ) const;				// �\�[�X�����烂�W���[�������擾

	void CheckApplicationPopup(EM_EVLOGNO ii,time_t time, const char *pSourceName,const char *pArg1, const CString &sMessage);
	void CheckStringInEvent(EM_EVLOGNO ii,time_t time, const char *pSourceName,const char *pArg1, const CString &sMessage);


	//// �e�C���X�^���X
	LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X

	//// ini�t�@�C���ݒ���
	int						m_nEnable;									// �C�x���g���O 0:�`�F�b�N���Ȃ� 1:�`�F�b�N����
	int						m_nApId;									// �A�v���P�[�V�����|�b�v�A���b�v���ُ̈�@��ID (=0:�`�F�b�N�Ȃ�)
	int						m_nStateId;									// �C�x���g���O���b�Z�[�W�ُ펞�̋@��ID (=0:�`�F�b�N�Ȃ�)
	char					m_cMsg[MSG_CHECK_CNT][256];					// �`�F�b�N���b�Z�[�W
	int						m_nMsgCheckCnt;								// �`�F�b�N���b�Z�[�W����

	//// �C�x���g���O�`�F�b�N�p
	DWORD					m_dayLastCheckTime;							// �ŏI�`�F�b�N����
	time_t					m_tmRealBoot;								// KS_EVTLOG�N������
	/// ��ꂽ���O�̃L���b�V���p�i���\�[�XDLL���Ȃ��j
	static const int SZUNKNOWN = 16;
	mutable CString m_nUnknownEvent[SZUNKNOWN];
	mutable int   m_nUnknownEventIndex;
};
