#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLib\Class\ProcessManager.h"					// �v���Z�X�Ǘ�

// �e���[�J�[�X���b�h
#include "MainConst.h"

class HddManager :	public ThreadManager
{
//// �萔
private:
#define	CMD_TXT_NAME		"hddstatus.txt"								// HDD��ԃe�L�X�g
	static const int		MAX_CHK_TXT			= 10;					// ��
	static const int		TIME_EXEC_CYCLE		= 1 * 60 * 60 * 1000;	// HDD�`�F�b�N���� (1���Ԃ�1��)

//// ���J�֐�
public:
	HddManager(void);
	virtual ~HddManager(void);

	void Alloc();														// �֐�������

	
	//// ��ʗp
	void SetEvManualExec()		{ SetEvent(m_evManualExec); }			// �蓮���s�V�O�i��
	void ShowEdit();													// �ڍ׌��ʕ\��
	
	bool	IsState() const		{ return m_bState; }
	CString	GetNgMsg() const	{ return m_sNgMsg; }



//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	
	void Exec();														// �`�F�b�N
	// 20181220 Smart Storage Administrator�Ή� --->>>
	void ExecSSA();														// �`�F�b�N(SSA)
	// 20181220 Smart Storage Administrator�Ή� ---<<<
	bool GetColumn(CString& sVal, int& curPos, CString& sToken);		// �P�s�̃f�[�^���� , �P�ʂ̕�������擾


//// �����o�[�ϐ�
private:

	//// �n���h��
	HANDLE					m_evManualExec;								// �蓮���s�C�x���g
	HANDLE					m_evTimerExec;								// ��������s�C�x���g

	//// ����
	bool					m_bState;									// HDD���
	CString					m_sNgMsg;									// NG���̕�����



	//// ini�t�@�C���ݒ�l
	int						m_nChkNum;									
	CString					m_sChkTxt[MAX_CHK_TXT];						// �`�F�b�N���镶����
	int						m_nOkNum;
	CString					m_sOkTxt[MAX_CHK_TXT];						// ����Ɣ��f���镶����
	int						m_nStatusID;								// �@��ID
	// 20181220 Smart Storage Administrator�Ή� --->>>
	int						m_nMode;									// ���[�h(0:Universal Raid Utility, 1:Smart Storage Administrator)
	// 20181220 Smart Storage Administrator�Ή� ---<<<

};
