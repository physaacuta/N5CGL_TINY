#pragma once


// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLib\CLASS\MailSlotManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[


#include "SmyuBaseManager.h"

class SmyuEvtManager :	public SmyuBaseManager
{
//// �����o�[�萔
private :
	static const int	TIME_EXEC_INTERVAL		= 30;					// PLG�`�F�b�N���s����
	static const int	MAX_MAIL_WAIT_NUM		= 8;					// ���[���X���b�g�҂��Ώۃ^�X�N�ő吔


//// ���J�֐�
public:
	SmyuEvtManager(void);
	virtual ~SmyuEvtManager(void);

	virtual void SmyuStart();
	virtual void SmyuStop();


//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	

	void MailRecvStart(int nIndex);										// ���[���X���b�g ��M�J�n
	void GetPoNowData();												// Po���ݒl�擾
	
	void SmyuExec_Po();													// �V�~�����[�^ ���s (PO�ω�)
	void SmyuExec_Mail(int nIndex);										// �V�~�����[�^ ���s (���[���X���b�g��M)



//// �����o�[�ϐ�
private:

	//// �n���h��
	HANDLE					m_evTimerExec;								// ��������s�C�x���g
	HANDLE					m_evRecvMail[MAX_MAIL_WAIT_NUM];

	//// ���[���X���b�g��M�p
	int						m_nMailCnt;									// ���[���X���b�g �ڑ���
	// �ȉ��̔z��́A�S������Index���g�p
	MailSlotManager*		mcls_pMail[MAX_MAIL_WAIT_NUM];				// ��M���[���X���b�g�N���X
	CString					m_RecvName[MAX_MAIL_WAIT_NUM];				// ��M���[���X���b�g����
	COMMON_QUE				m_RecvBuf[MAX_MAIL_WAIT_NUM];				// ��M�o�b�t�@ (�񓯊��̂��ߕK�v)


	//// PO �ω�
	bool					m_nDoBuf[SMYU_PIO_MAX_BOOD][SMYU_PIO_MAX_PORT][8];		// PO�O��l
};
