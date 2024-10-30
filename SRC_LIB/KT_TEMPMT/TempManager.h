#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\Kizu232C\Rs232cBase.h"							// RS-232C����N���X
#include "..\..\Library\KizuLibMFC\CLASS\DivNameManager.h"

// �e���[�J�[�X���b�h
#include "MainConst.h"

using namespace KizuLib;
using namespace Kizu232C;

class TempManager :	public ThreadManager
{
//// �Œ�萔
private:
	//------------------------------------------
	// �Œ�萔
	//------------------------------------------
	// RS232C�ݒ�
	static const int PARAM_232C_SENDBUF_SIZE		= 256;				// 232C�̑��M�o�b�t�@�T�C�Y
	static const int PARAM_232C_RECVBUF_SIZE		= 256;				// 232C�̎�M�o�b�t�@�T�C�Y

	// �^�C�}�[
	static const int PARAM_TIMEOUT_RETRY			= 5000;				// �Đڑ��v��
	static const int PARAM_TIMEOUT_ANSER			= 30000;			// �����`����M�^�C���A�E�g



//// ���J�֐�
public:
	TempManager(char const* cName);
	virtual ~TempManager(void);


	//=========================================
	// �O���A�N�Z�X
	CString GetMyName() { return my_sThreadName;}
	int		GetUnitNum() { return m_nUnitNum; }

	CString	GetMainModeName(EM_TEMP_MAINMODE em);							// ���C�����[�h����
	CString	GetMainModeName() { return GetMainModeName(m_emMainMode); };	// ���̏��
	CString GetUnitName(int id) { return m_cUnitName[id];};					// ���j�b�g����

	CString GetConnect() { return DivNameManager::GetDivBool(m_bIsInital);}
	CString GetUnten(int id) { return DivNameManager::GetDivBool(m_emUnten[id]);}
	int		GetStatus(int id){ return m_nStatus[id];}
	double	GetTemp(int id){ return m_dTemp[id];}
	bool	GetTempSt(int id){ return m_bTempSt[id];}


//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	

	// ���x�p�l�����[�^����
	bool CheckNextSend();												// �����M����
	bool CheckDataRecv();												// ��M�`���`�F�b�N
	
	bool Check_Echo_Back(RES_TYPE_ECHO_BACK* buf);
	bool Check_Controler_Status(RES_TYPE_CONTROLER_STATUS* buf);
	bool Check_Monitor_Read(RES_TYPE_MONITOR_READ* buf);

	CString GetResCodeName(char* cResCode);								// ���X�|���X�R�[�h���� �擾
	CString GetEXTName(char* cEXT);										// �I���R�[�h���� �擾


	// RS-232C�֘A
	bool	Open(bool bLogDsp);											// 232C �I�[�v��
	void	Close();													// 232C �N���[�Y
	bool	Recv();														// 232C �񓯊� ��M
	bool	Send();														// 232C ���� ���M

	// BCC�֘A
	void	CreateBCC(char* cSendBuf, int nBufSize);					// BCC�N���G�C�g
	bool	CheckBCC(char* cRecvBuf, int nBufSize);						// BCC�`�F�b�N



//// �����o�[�ϐ�
private:
	// �e�C���X�^���X
	Rs232cBase*				mcls_p232c;									// 232c�ڑ��C���X�^���X


	// �n���h��
	HANDLE					m_evRecvEnd;								// ��M�����C�x���g�F���p
	HANDLE					m_evSendEnd;								// ���M�����C�x���g�F���p
	HANDLE					m_evReTry;									// �Đڑ��v�� �C�x���g
	HANDLE					m_evAnserTimeOut;							// �����`���҂��^�C���A�E�g �C�x���g
	HANDLE					m_evCycleExec;								// ������m�F�C�x���g


	// 232C�֘A
	char					m_cRecvBuf[PARAM_232C_RECVBUF_SIZE];		// ��M�o�b�t�@
	char					m_cSendBuf[PARAM_232C_SENDBUF_SIZE];		// ���M�o�b�t�@

	// �X�e�[�^�X�֘A
	EM_TEMP_MAINMODE		m_emMainMode;								// ���݂̃��C�����[�h
	int						m_nSubMode;									// ���݂̃T�u���[�h
	int						m_nUnitMode;								// ���݂̃��j�b�g�䐔�J��Ԃ����[�h

	// ���x�p�l�����[�^ ���
	EM_DIV_BOOL				m_bIsConnect;								// ���x�p�l�����[�^ �ڑ���� (true:�ڑ���, false:�ؒf��)
	EM_DIV_BOOL				m_bIsInital;								// ���x�p�l�����[�^ ��������� (true:�I�[�v���`�F�b�N������True)

	EM_DIV_BOOL				m_emUnten[MAX_TEMP_UNIT];					// �^�]��� (true:����^�]�� false:��~�� or �s��)
	int						m_nStatus[MAX_TEMP_UNIT];					// �X�e�[�^�X (0x00:���� 0x01:���v����� 0x02:�\���͈̓I�[�o�[ 0x04:���ُ͈�A 0x08:���ُ͈�B)
	double					m_dTemp[MAX_TEMP_UNIT];						// ���x
	bool					m_bTempSt[MAX_TEMP_UNIT];					// ���x���

	// �ݒ�l
	CString					m_cUnitName[MAX_TEMP_UNIT];					// ���́B���O�Ŏg�p
	int						m_nTempLimit[MAX_TEMP_UNIT];				// ���x臒l [�x] (0���`�F�b�N�B)���̒l�ȏ�ŉ��x�ُ�ɂ���

	int						m_nIdConnect;								// �ڑ�ID 			(0�F�@���Ԃɓo�^���Ȃ�)
	int						m_nIdTemp[MAX_TEMP_UNIT];					// ���xID				�V
	int						m_nIdTempSts[MAX_TEMP_UNIT];				// ���x���ID			�V
	int						m_nIdUnten[MAX_TEMP_UNIT];					// �^�]���ID			�V
	int						m_nIdState[MAX_TEMP_UNIT];					// �X�e�[�^�X���ID		�V


	// ���̑��������
	CString					m_cMySession;								// �����ʎq (�@���� ���X�̐؂�ւ��Ɏg�p����)
	int						m_nUnitNum;									// �Ǘ��Ώۃ��j�b�g��
	int						m_nCycleTime;								// ���� [msec]
	int						m_nTempKeta;								// �擾���x�̌����킹 (1��1/10�B2��1/100�B0�͂��̂܂܁B�}�C�i�X�͖��Ή�)
	
};
