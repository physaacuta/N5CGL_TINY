#pragma once

#include "SmyuBaseManager.h"

class SmyuTimManager :	public SmyuBaseManager
{
//// �����o�[�萔
private :
	static const int	TIME_MAX_NUM		= 32;						// �ő�^�C�}�[��

//// ���J�֐�
public:
	SmyuTimManager(int nMyID);
	virtual ~SmyuTimManager(void);

	virtual void SmyuStart();
	virtual void SmyuStop();

	void TimerExec(int nId, int nFirst, int nCycle);					// �^�C�}�[����


//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	

	void SmyuExec(int nIndex);											// �V�~�����[�^ ���s


//// �����o�[�ϐ�
private:

	//// �n���h��
	HANDLE					m_evTimerExec[TIME_MAX_NUM];				// ��������s�C�x���g
};
