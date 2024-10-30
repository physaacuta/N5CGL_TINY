#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[

#include "DefragManager.h"												// �C���f�b�N�X�Ǘ�


class ControlDefragManager : public ThreadManager
{
public:
	ControlDefragManager(void);
	~ControlDefragManager(void);


	void WorkerNew(int id, char* cSession);								// �Ǘ��X���b�h����
	void WorkerStop();													// �Ǘ��X���b�h��~����


	// �O���A�N�Z�X
	bool IsDefragEnable()	const { return m_nDefragIntervalDay == 0 ? false : true; }


//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����

	void Exec();														// �C���f�b�N�X �f�t���O���s�۔���

protected:
	// �e���[�J�[�X���b�h
	int						m_nClassCnt;								// �C���X�^���X�̐�����
	DefragManager*			mcls_pDefrag[CONTROL_MAXCNT];				// �C���f�b�N�X�Ǘ�

	//// �n���h��
	HANDLE					m_evTimerExec;								// ��������s�C�x���g


	// ���̑��������
	int						m_nDefragIntervalDay;						// �f�t���O���s�Ԋu [��] 0:�����s


	// �q�����Q��
	bool					m_bExecDefrag;								// �f�t���O���s�̏ꍇ�Atrue

};
