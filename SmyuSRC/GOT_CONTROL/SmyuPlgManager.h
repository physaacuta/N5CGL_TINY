#pragma once

#include "SmyuBaseManager.h"

class SmyuPlgManager :	public SmyuBaseManager
{
//// �����o�[�萔
private :
	static const int	TIME_EXEC_INTERVAL		= 30;					// PLG�`�F�b�N���s����

//// ���J�֐�
public:
	SmyuPlgManager(int nMyID);
	virtual ~SmyuPlgManager(void);


	virtual void SmyuStart();
	virtual void SmyuStop();


//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	
	void SmyuExec();													// �V�~�����[�^ ���s


//// �����o�[�ϐ�
private:

	//// �n���h��
	HANDLE					m_evTimerExec;								// ��������s�C�x���g
	
	//// ���̑�
	int						m_nStoryIndex[SMYU_PLG_MAX_CHANNEL];		// �V�i���I���s�ʒu
};
