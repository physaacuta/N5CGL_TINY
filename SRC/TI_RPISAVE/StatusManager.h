#pragma once


// ���ʃw�b�_�[
#include "..\..\Include\KizuCommon.h"									// �V�X�e���p�C���N���[�h

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLibMFC\Class\DivNameManager.h"				// DIV�萔 ���̎擾�N���X
#include "..\..\Library\KizuLib\Class\AutoLock.h"						// �N���e�B�J���Z�N�V��������N���X

#include "..\..\Library\DefectLib\KikiManager.h"						// �@���ԊǗ��N���X


// �摜�����p���C�u���� �C���N���[�h
#include "..\..\Library\IportLib\IportCommon.h"							// iPort�p�N���X�萔

// ���[�J��
#include "MainConst.h"
#include "ParamManager.h"												// �p�����[�^�Ǘ��N���X

using namespace KizuLib;

class StatusManager : public ThreadManager
{
//// ���J�萔
public:
	//// �^�X�N �X�e�[�^�X
	enum EM_MAIN_STATE
	{		
		STATE_NON = 0,			// ��������
		STATE_INIT,				// ������� (�����J�����)
		STATE_RUN,				// ������   (�I�����C��) (�������A�B���҂��̉\���͂���)
	};

	// �����X�e�[�^�X���
	enum EM_SYS_STATE
	{	
		SYS_STATE_NON = 0,			// ��������
		SYS_STATE_WAIT,				// �B���҂�

		// �B����--- >>>
		SYS_STATE_OK,
		SYS_STATE_WAR,
		SYS_STATE_ERR
	};



//// ���J�֐�
public:
	StatusManager(void);
	virtual ~StatusManager(void);

	// �v���p�e�B
	void SetParamMgr(ParamManager* pCls) { mcls_pParam = pCls; }		// �p�����[�^�Ǘ�



	//// �^�X�N�X�e�[�^�X����
	bool			IsRun()				const { return STATE_RUN == m_emMainSt ? true : false;}	// �T����:true
	// �X�e�[�^�X
	EM_MAIN_STATE	GetMainSt()			const { return m_emMainSt; }							// ���݂̃X�e�[�^�X�擾
	CString			GetMainStName()		const { return GetMainStName(m_emMainSt); };			// ���݂̃X�e�[�^�X���̎擾
	CString			GetMainStName(EM_MAIN_STATE st) const;										// �w��̃X�e�[�^�X���̎擾
	bool			SetMainSt(EM_MAIN_STATE st);												// �X�e�[�^�X�ύX

	//// �t���[���Y�����
	bool			GetFrameSyncAllSt(int nInsNum);												// �t���[���Y����Ԏ擾
	bool			GetFrameSyncSt(int nIns) { return m_bFrameSync[nIns]; };													// �t���[���Y����ԕύX
	void			SetFrameSyncSt(int nIns, bool bMode);										// �t���[���Y����ԕύX
	void			ResetFrameSyncAllSt();														// �t���[���Y����ԃ��Z�b�g
	void			ResetFrameSyncSt(int nInsNum, int nFno);
	DWORD			GetReSyncFno(int nIns) { return m_nFrameSyncNo[nIns]; }
	void			ResetAp();		// �\�t�g�����ċN��


//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int	ThreadLast()	{return 0;};									// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����



//// �����o�[�ϐ�
private:
	//// �e�C���X�^���X
	ParamManager*						mcls_pParam;					// �p�����[�^�Ǘ�


	// �n���h��
	HANDLE					m_evReFrameSync;							// �ē����v�� (�K�{)
	HANDLE					m_evReFrameSyncAuto;						// �ē����v�� (�Y�����̂�)


	//// ��������
	CRITICAL_SECTION		m_csLock;									// �N���e�B�J���Z�N�V����


	//// �X�e�[�^�X���
	EM_MAIN_STATE			m_emMainSt;									// �^�X�N�X�e�[�^�X
	bool					m_bFrameSync[MAX_INS];						// �t���[���Y����� (ture:����)
	DWORD					m_nFrameSyncNo[MAX_INS];					// �t���[������No
};
