// *********************************************************************************
//	���C���Ǘ��X���b�h
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//		
// *********************************************************************************

#pragma once


// �C���N���[�h
#include <process.h>

// �r�� �C���N���[�h
#include "..\..\include\KizuCommon.h"									// �r�����ʃw�b�_�[
#include "..\..\Library\KizuLib\KizuLib.h"								// �r�����C�u�����w�b�_�[

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLib\CLASS\KizuMacro.h"						// �r���}�N��
#include "..\..\Library\KizuLib\CLASS\WorkThread.h"						// ���[�J�[�X���b�h
#include "..\..\Library\KizuLibMFC\CLASS\LogFileManager.h"				// ���O�}�l�[�W���[

// �r�����C�u���� �C���N���[�h
//#include "..\..\Library\KizuSOCK3\SockBase.h"							// �\�P�b�g����N���X
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC����N���X


// �e���[�J�[�X���b�h
#include "MainConst.h"
#include "ControlManager.h"												// �폜�X���b�h�R���g���[���N���X
#include "ControlLowGr.h"												// ���QGr�폜�Ǘ�
#include "ControlDefragManager.h"										// �C���f�b�N�X�Ǘ�


using namespace KizuLib;
using namespace KizuODBC;

class MainInctance : public WorkThread
{

public:
	MainInctance(void);
	virtual ~MainInctance(void);

	void Start() { SetEvent(m_evThStart); };							// �X���b�h�J�n
	void Stop()  { WorkThread::StopMSG(WM_USER_TH_END); };				// �X���b�h�I��
	BOOL Init();														// ����������
	void Exit();														// �I������
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ���O�t�@�C���Z�b�g

	void SetWaitMode(bool mode);										// �폜/���� �D�惂�[�h (true:�폜�D��)



	// �e���[�J�[�X���b�h
	int						g_nClassCnt;								// �C���X�^���X�̐�����
	ControlManager*			gcls_pContMgr[CONTROL_MAXCNT];				// ControlManager�X���b�h�C���X�^���X
	ControlLowGr*			gcls_pContLowGr;							// ���QGr�폜�Ǘ�
	ControlDefragManager	gcls_ContDefrag;							// �C���f�b�N�X�Ǘ�


private:
	static UINT WINAPI MainThread(LPVOID param);						// ���C���X���b�h
	

	//// �e�C���X�^���X
	LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X

	// �����V�O�i��
	HANDLE					m_evThStart;								// MainInctance�X���b�h�̎��s�v��

	// ���̑��������

};
