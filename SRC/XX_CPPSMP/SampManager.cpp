#include "StdAfx.h"
#include "SampManager.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
SampManager::SampManager(void) :
ThreadManager("SampManager")
{
	//// �n���h������
	m_evTimerExec  = CreateWaitableTimer(NULL, FALSE, NULL);
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
SampManager::~SampManager(void)
{
	//// �n���h�����
	CloseHandle(m_evTimerExec);
}


//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int SampManager::ThreadFirst()
{
	//// ���O����
	__int64 ts = 3000 * -10000; 
	SetWaitableTimer(m_evTimerExec, (LARGE_INTEGER *)&ts, 300000, NULL, NULL, FALSE);

	//// �C�x���g�ݒ�
	HANDLE hArray[] = { m_evTimerExec
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int SampManager::ThreadLast()
{
	// �������~
	CancelWaitableTimer(m_evTimerExec);
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void SampManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_TIMER_EXEC = 0,				// �������s ������^�C�}
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_TIMER_EXEC: 					// �������s ������^�C�}
		LOG(em_MSG), "[%s] Thread Exe=%d", my_sThreadName, nEventNo );
		SampExec();
		return;

	
//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

//------------------------------------------
// ��������
//------------------------------------------
int SampManager::SampExec()
{
	return 0;
}

