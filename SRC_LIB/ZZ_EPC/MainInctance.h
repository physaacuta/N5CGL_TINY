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
#include <winsock2.h>
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
//#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC����N���X
//#include "..\..\Library\Kizu232C\Rs232cBase.h"							// 232C����N���X

#include "..\..\Library\DefectLib\BaseTcpIp.h"							// TCP/IP����N���X

// �e���[�J�[�X���b�h
#include "CommandRecvre.h"
#include "DataSender.h"
#include "PLGReceive.h"
#include "EdgeReceive.h"

using namespace KizuLib;

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

	// �e���[�J�[�X���b�h
	CommandRecvre*	gcls_pCommand;										// �R�}���h��M
	DataSender*	gcls_pData;												// �f�[�^���M
	PLGReceive* gcls_pPlg;												// ���xUDP��M
	EdgeReceive* gcls_pEdge;											// �G�b�W�ʒu���UDP��M


private:
	static UINT WINAPI MainThread(LPVOID param);						// ���C���X���b�h




	//// �e�C���X�^���X
	LogFileManager*		mcls_pLog;										// LogFileManager�X���b�h�C���X�^���X
	
	// �����V�O�i��
	HANDLE	m_evThStart;												// MainInctance�X���b�h�̎��s�v��

	// ���̑��������
	//BYTE *m_wk;

};
