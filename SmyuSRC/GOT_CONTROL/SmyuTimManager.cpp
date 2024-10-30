#include "StdAfx.h"
#include "SmyuTimManager.h"

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
SmyuTimManager::SmyuTimManager(int nMyID) :
SmyuBaseManager("SmyuTimMgr", nMyID)
{
	//// �n���h������
	for(int ii=0; ii<TIME_MAX_NUM; ii++) {
		m_evTimerExec[ii] = CreateWaitableTimer(NULL, FALSE, NULL);
	}
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
SmyuTimManager::~SmyuTimManager(void)
{
	SmyuStop();

	//// �n���h�����
	for(int ii=0; ii<TIME_MAX_NUM; ii++) {
		CloseHandle(m_evTimerExec[ii]);
	}
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int SmyuTimManager::ThreadFirst()
{
	//// �C�x���g�ݒ�
	this->AddEventNum(TIME_MAX_NUM, m_evTimerExec);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int SmyuTimManager::ThreadLast()
{
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void SmyuTimManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_TIMER_EXEC = 0,				// �������s ������^�C�}
	};
 

	if( nEventNo >= EV_TIMER_EXEC && nEventNo < EV_TIMER_EXEC+TIME_MAX_NUM) {
		int nIndex = nEventNo - EV_TIMER_EXEC;

		SmyuExec(nIndex);
	}
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����

//------------------------------------------
// �V�~�����[�^ �X�^�[�g
//------------------------------------------
void SmyuTimManager::SmyuStart()
{
	//// �Ώۂ̔��������������
	for(int ii=0; ii<(int)m_lstStory.size(); ii++ ) {
		if(DIV_KIND_TIME != m_lstStory[ii]->emRaise ) continue;

		TYP_RAISE_TIM_CYCLE* pD = (TYP_RAISE_TIM_CYCLE*)m_lstStory[ii]->pRaise;
 		
		TimerExec(pD->nID, pD->nFirst, pD->nCycle );
	}
}

//------------------------------------------
// �V�~�����[�^ �X�g�b�v
//------------------------------------------
void SmyuTimManager::SmyuStop()
{
	for(int ii=0; ii<TIME_MAX_NUM; ii++) {
		CancelWaitableTimer(m_evTimerExec[ii]);
	}
}

//------------------------------------------
// �V�~�����[�^ ���s
// int nIndex �z��ʒu 
//------------------------------------------
void SmyuTimManager::SmyuExec(int nIndex)
{
	//// ����^�C�}�[ID�̂��̂����s������
	for(int ii=0; ii<(int)m_lstStory.size(); ii++ ) {
		if(DIV_KIND_TIME != m_lstStory[ii]->emRaise ) continue;

		TYP_RAISE_TIM_CYCLE* pD = (TYP_RAISE_TIM_CYCLE*)m_lstStory[ii]->pRaise;
		if( nIndex != pD->nID ) continue;		

		// �V�i���I ����
		this->RaiseEvents(m_lstStory[ii]->emEvent, m_lstStory[ii]->nEventSub, (BYTE*)m_lstStory[ii]->pEvent);
	}
}

//------------------------------------------
// �^�C�}�[����
// int nId �z��ʒu
// int nFirst �P��ڎ���
// int nCycle �Q��ڈȍ~�̌J��Ԃ�
//------------------------------------------
void SmyuTimManager::TimerExec(int nId, int nFirst, int nCycle)
{
	if(0 > nId || TIME_MAX_NUM <= nId ) return;

	if(0 < nFirst) {
		__int64 ts = nFirst * -10000; 
		SetWaitableTimer(m_evTimerExec[nId], (LARGE_INTEGER *)&ts, nCycle, NULL, NULL, FALSE);
	} else {
		CancelWaitableTimer(m_evTimerExec[nId]);
	}
}
