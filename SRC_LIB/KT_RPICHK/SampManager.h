#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[



class SampManager :	public ThreadManager
{

//// ���J�֐�
public:
	SampManager(void);
	virtual ~SampManager(void);


//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	
//// �����o�[�ϐ�
private:

	//// �n���h��
	HANDLE					m_evTimerExec;								// ��������s�C�x���g


};
