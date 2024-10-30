#include "StdAfx.h"
#include "SmyuPlgManager.h"

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
SmyuPlgManager::SmyuPlgManager(int nMyID) :
SmyuBaseManager("SmyuPlgMgr", nMyID)
{	
	//// �n���h������
	m_evTimerExec  = CreateWaitableTimer(NULL, FALSE, NULL);

	for(int ii=0; ii<SMYU_PLG_MAX_CHANNEL; ii++) m_nStoryIndex[ii] = 0;
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
SmyuPlgManager::~SmyuPlgManager(void)
{
	SmyuStop();

	//// �n���h�����
	CloseHandle(m_evTimerExec);
}


//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int SmyuPlgManager::ThreadFirst()
{
	//// �C�x���g�ݒ�
	HANDLE hArray[] = { m_evTimerExec
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int SmyuPlgManager::ThreadLast()
{
	// �������~
	CancelWaitableTimer(m_evTimerExec);
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void SmyuPlgManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_TIMER_EXEC = 0,				// �������s ������^�C�}
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_TIMER_EXEC: 					// �������s ������^�C�}
		SmyuExec();
		return;

	
//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����

//------------------------------------------
// �V�~�����[�^ �X�^�[�g
//------------------------------------------
void SmyuPlgManager::SmyuStart()
{
	for(int ii=0; ii<SMYU_PLG_MAX_CHANNEL; ii++) m_nStoryIndex[ii] = 0;

	//// ���O����
	__int64 ts = TIME_EXEC_INTERVAL * -10000; 
	SetWaitableTimer(m_evTimerExec, (LARGE_INTEGER *)&ts, TIME_EXEC_INTERVAL, NULL, NULL, FALSE);

}
//------------------------------------------
// �V�~�����[�^ �X�g�b�v
//------------------------------------------
void SmyuPlgManager::SmyuStop()
{
	CancelWaitableTimer(m_evTimerExec);
}

//------------------------------------------
// �V�~�����[�^ ���s
//------------------------------------------
void SmyuPlgManager::SmyuExec()
{
	double	dLen[SMYU_PLG_MAX_CHANNEL];
	int		nCnt = (int)m_lstStory.size();

	//================================================
	// �ʔ������擾
	for(int ii=0; ii<SMYU_PLG_MAX_CHANNEL; ii++) {
		dLen[ii] = mcls_pPlg->GetLength(ii); 
	}


	//================================================
	// �V�i���I���s
	for(int ii=0; ii<SMYU_PLG_MAX_CHANNEL; ii++) {

		int nIndex = m_nStoryIndex[ii];						// ���ݎ��s�ʒu���擾
		for(int jj=nIndex; jj<nCnt; jj++) {
			if(DIV_KIND_PLG != m_lstStory[jj]->emRaise) goto jjcontnue;	// �������� PLG �łȂ��B �ʏ킠�肦�Ȃ�

			//// ���������� �擾
			TYP_RAISE_PLG_POS* pD = (TYP_RAISE_PLG_POS*)m_lstStory[jj]->pRaise;
			if(ii != pD->nCh ) goto jjcontnue;							// �����Ch�Ƃ͈Ⴄ
			if( dLen[ii] < pD->nLen ) break;							// �܂��A�Ώۈʒu�ɓ��B���Ă��Ȃ�


			//// �C�x���g���`�F�b�N
			if( DIV_KIND_ZZ == m_lstStory[jj]->emEvent && 0 == m_lstStory[jj]->nEventSub ) {
				// ���Z�b�g����
				TYP_EVENT_ZZ_RESET* pE = (TYP_EVENT_ZZ_RESET*)m_lstStory[jj]->pEvent;
				
				// �ʔ�����������
				dLen[ii] = 0.0;
				mcls_pPlg->ResetLength(pE->nCh);
				LOG(em_MSG), "[%s] CH=%d �ʔ������Z�b�g", my_sThreadName, pE->nCh);

				// �V�i���I���ŏ�����Ƃ���
				if( 1 == pE->nMode ) {
					m_nStoryIndex[pE->nCh] = 0;	
					LOG(em_MSG), "[%s] CH=%d �V�i���I�擪������s", my_sThreadName, pE->nCh);
					break;
				}

			} else {

				// �V�i���I ����
				this->RaiseEvents(m_lstStory[jj]->emEvent, m_lstStory[jj]->nEventSub, (BYTE*)m_lstStory[jj]->pEvent);
			}
			
jjcontnue:
			// �V�i���I���s�ʒu��ύX
			m_nStoryIndex[ii] = jj+1;
		}
	}
}