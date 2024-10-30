// *********************************************************************************
//	�@���ԊǗ��N���X
//	[Ver]
//		Ver.01    2007/04/30  vs2005 �Ή�
//
//	[����]
//		
// *********************************************************************************

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
#include "..\..\Library\KizuLib\Class\AutoLock.h"						// �N���e�B�J���Z�N�V�����N���X
//#include "..\..\Library\KizuLib\Class\KizuPerformanceCounter.h"			// ���ԑ���N���X

#include "..\..\Library\KizuODBC\OdbcBase.h"							// DB����N���X

using namespace KizuLib;
using namespace KizuODBC;


class CheckStatus : public WorkThread
{
private :
	static const int HEART_BEAT_SEND_CYCLE	= 7000;						// �n�[�g�r�[�g���M����
	static const int HEART_BEAT_CHECK_CYCLE = 20000;					// �n�[�g�r�[�g�`�F�b�N����


	struct CheckStatus_INF {										// �@����
		int  id;														// �n�[�g�r�[�g�`�F�b�N����@��ID
		char name[32];													// �@�햼��
		int  status;													// �@���� (0:����)
		int	 nHb;														// �n�[�g�r�[�g�m�F�L�� (0:���� 1:�L��)
		bool statusHb;													// �n�[�g�r�[�g��M�L�� (false:�܂����ĂȂ�  true:����)
	};


public:
	CheckStatus(bool isMaster);
	virtual ~CheckStatus(void);

	void Alloc();														// �f�[�^�m��
	void Start() { WorkThread::Start(MainThread); };					// �X���b�h�J�n
	void Stop()  { WorkThread::Stop(10000); };							// �X���b�h�I��
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; }			// ���O�t�@�C���Z�b�g

	// Set
	void Set_StatusID(int id, bool status);								// �X�e�[�^�X�Z�b�g
	void Set_StatusIndex(int index, bool status);
	void Set_HeatBeatID(int id, bool status);							// �n�[�g�r�[�g��������t���O�Z�b�g
	void Set_HeatBeatIndex(int index, bool status);

	// Get
	int GetIndex(int id);												// �@��ID����z��ʒu���擾����
	char const * GetName(int id);										// �Y���̋@��ID�̋@�햼�̂��擾

private :
	static UINT WINAPI MainThread(LPVOID param);						// ���C���X���b�h

	void Get_IniData();													// ini�t�@�C���f�[�^�擾
	bool Init_StatusInf();												// �@���ԃe�[�u�������� (�}�X�^�[���̂�)
	bool Get_Status_ID(int mode=0);										// �@��ID���擾
	void Check_HeatBeat();												// �n�[�g�r�[�g�`�F�b�N
	void Send_HeatBeat();												// �n�[�g�r�[�g���M


	//// �e�C���X�^���X
	LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X

	// �N���e�B�J���Z�N�V����
	CRITICAL_SECTION		m_csLock;									// �������b�N�p

	// �V�O�i��
	HANDLE					m_evHBeatCheck;								// �n�[�g�r�[�g�Ď��^�C�}�C�x���g
	HANDLE					m_evHBeatSend;								// �n�[�g�r�[�g���M�^�C�}�C�x���g

	// �@���Ԋ֘A
	int						m_nStateCount;								// �@���ԏ�񌏐�
	CheckStatus_INF			m_typStateCheck[MAX_STATUS];				// �@���ԏ��

	// ���̑��������
	bool					m_bIsMaster;								// PC�}�X�^�[�Ǘ���true
	int						m_nMasterCheckID;							// �}�X�^�[���X���[�u�̃n�[�g�r�[�g�m�F�p�̋@��ID  (0:�m�F���� 1�`:�m�F�L��)
	int						m_nSendHBID;								// �n�[�g�r�[�g���M�@��ID (0:���M���Ȃ� 1�`:���M����)
	
};
