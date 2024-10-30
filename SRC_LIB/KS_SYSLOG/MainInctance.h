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
#include "..\..\Library\KizuLib\CLASS\ThreadQueue.h"					// �X���b�h�L���[

// �r�����C�u���� �C���N���[�h
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC����N���X

// �e���[�J�[�X���b�h
#include "..\..\Library\DefectLib\DBSaveManager.h"						// DB�����݊�{�N���X


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
	DBSaveManager* gcls_pDb;											// DB�����݃N���X

	// �O���A�N�Z�X
	void AddSysLogSqlQue(COMMON_QUE const* que);						// Syslog���X���b�h�L���[�ɓo�^
	void AddStateLogSqlQue(COMMON_QUE const* que);						// Statelog���X���b�h�L���[�ɓo�^

	void SetSyslogDsp(bool flg) {m_bSyslogDsp = flg; }					// �V�X���O��ʕ\���L��

private:
	static UINT WINAPI MainThread(LPVOID param);						// ���C���X���b�h

	void InitAutoNo();													// AutoNo�ɕK�v�ȃf�[�^�������Z�b�g����
	void RefrashAutoNo(int nDay);										// ���t���`�F�b�N���āA�����ς��΁A�I�[�gNo�����Z�b�g����B


	//// �X���b�h�L���[
	ThreadQueue<DBSaveManager::DeliverySQL> mque_AddDB;					// �󂯓n���f�[�^�p�L���[ (�����ɓ������L���[�͑S�ēo�^����)

	//// �e�C���X�^���X
	LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X
	
	// �����V�O�i��
	HANDLE					m_evThStart;								// MainInctance�X���b�h�̎��s�v��

	// ���̑��������
	bool					m_bSyslogDsp;								// �V�X���O��ʕ\���L��

	// ��L�[�֘A
	//  �E��L�[�P: �o�^���t
	//  �E��L�[�Q: m_nAutoNo*10 + m_nODBCID
	int						m_nDay;										// ���ݓ�
	int						m_nAutoNo;									// ��L�[�̈�ƂȂ�A��
																			//�E��{�I�ɂ́A�����ς��^�C�~���O�Ń��Z�b�g(1)����
																			//�E�����グ���^�C�~���O�ł́A�ŐV���t��AutoNo�̒l�Ƃ���
	int						m_nOdbcID;									// ODBC�̐ݒ�ʒu

};
