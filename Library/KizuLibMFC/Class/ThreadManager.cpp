#include "StdAfx.h"
#include "ThreadManager.h"


//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// �R���X�g���N�^
// const char* pThreadName ���X���b�h��
// int nMaxWaitNum WaitForMultipleObjects�̍ő�҂��s��
//------------------------------------------
ThreadManager::ThreadManager(const char* pThreadName, int nMaxWaitNum) :
my_sThreadName(pThreadName),
PARAM_MAX_WAIT_NUM(nMaxWaitNum),
my_pEvWait(NULL),
my_nWaitNum(0),
my_nWaitNumNow(0),
mcls_pLog(NULL)
{
	// �̈�m��
	my_pEvWait = new HANDLE[PARAM_MAX_WAIT_NUM];
	for(int ii=0; ii<PARAM_MAX_WAIT_NUM; ii++) my_pEvWait[ii] = INVALID_HANDLE_VALUE; 
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
ThreadManager::~ThreadManager(void)
{
	SAFE_DELETE_ARRAY(my_pEvWait);
	my_pEvWait = NULL;
}

//------------------------------------------
// �C�x���g�Z�b�g
// int num �Z�b�g����V�O�i����
// LPHANDLE pWait �V�O�i���̔z��
//------------------------------------------
bool ThreadManager::SetEventNum(int num, LPHANDLE pWait)
{ 
	if(PARAM_MAX_WAIT_NUM <= num) return false;
	memcpy(&my_pEvWait[1], pWait, sizeof(HANDLE)*num);						// �I���V�O�i�����l��
	my_nWaitNum = num;	
	my_nWaitNumNow = my_nWaitNum;
	return true;
}

//------------------------------------------
// �C�x���g�̒ǉ� (1�R����)
// HANDLE pWait �V�O�i��  (�֐��̖߂�l�ɑ΂��Ă̂Ƃ�)
//------------------------------------------
bool ThreadManager::AddEventNum(HANDLE pWait)
{ 
	if(PARAM_MAX_WAIT_NUM <= (my_nWaitNum+1) ) return false;

	memcpy(&my_pEvWait[1+my_nWaitNum], &pWait, sizeof(HANDLE));			// �I���V�O�i�����l��
	my_nWaitNum += 1;	
	my_nWaitNumNow = my_nWaitNum;
	return true;
}

//------------------------------------------
// �C�x���g�̒ǉ� (�z��)
// int num �Z�b�g����V�O�i����
// LPHANDLE pWait �V�O�i���̔z��
//------------------------------------------
bool ThreadManager::AddEventNum(int num, LPHANDLE pWait)
{ 
	if(PARAM_MAX_WAIT_NUM <= (my_nWaitNum+num) ) return false;
	memcpy(&my_pEvWait[1+my_nWaitNum], pWait, sizeof(HANDLE)*num);			// �I���V�O�i�����l��
	my_nWaitNum += num;	
	my_nWaitNumNow = my_nWaitNum;
	return true;
}
//------------------------------------------
// WaitFor�̑҂�����ύX (�ʐM�^�X�N���Ŏg�p�����)
// int num �Z�b�g����V�O�i����
//------------------------------------------
bool ThreadManager::ChangeWaitNum(int num)
{ 
	if(PARAM_MAX_WAIT_NUM <= num) return false;
	my_nWaitNumNow = num;	
	return true;
}

//------------------------------------------
// �X���b�h
//------------------------------------------
UINT ThreadManager::MainThread(LPVOID param)
{

/*
	int nEventNo;
	int nRetc;

	////// ���C���C���X�^���X���o��
	ThreadManager *p = (ThreadManager *)param;
	p->SetTid();									// ���X���b�hID�Z�b�g
	nRetc = p->ThreadFirst();						// �X���b�h�J�n���R�[���o�b�N

	//// WaitForMultipleObjects�C�x���g
	p->my_pEvWait[0] = p->my_evStop;
	

	pLOG(em_MSG), "[%s] �X���b�h�J�n=%d [0x%X]", p->my_sThreadName, nRetc, p->GetTid());
	while( 1 ) {
		DWORD ret = WaitForMultipleObjects((p->my_nWaitNum+1), p->my_pEvWait, FALSE, INFINITE);
		nEventNo = ret - WAIT_OBJECT_0; 

//-----------------------------------------------------------------------------------------------
		// �X���b�h�I��
		if( 0 == nEventNo ) {
			nRetc = p->ThreadLast();				// �X���b�h�I�����R�[���o�b�N
			pLOG(em_MSG), "[%s] �X���b�h�I��=%d [0x%X]", p->my_sThreadName, nRetc, p->GetTid());
			return nRetc;
		}

//-----------------------------------------------------------------------------------------------
		// ��M
		if( 1 <= nEventNo && nEventNo <= p->my_nWaitNum ) {
			p->ThreadEvent(nEventNo-1);				// �C�x���g���R�[���o�b�N
			continue;
		}
	}

	// ���肦�Ȃ�
	pLOG(em_ERR), "[%s] ���C�����[�v���甲�����I�I=%d [0x%X]", p->my_sThreadName, nRetc, p->GetTid());
	return -1;
*/

	////// ���C���C���X�^���X���o��
	ThreadManager *p = (ThreadManager *)param;
	p->SetTid();									// ���X���b�hID�Z�b�g
	p->SetThreadName( p->GetThreadName());			// ���X���b�h���̐ݒ�


	//// WaitForMultipleObjects�C�x���g
	p->my_pEvWait[0] = p->my_evStop;
	
	//// ���C�����[�v�Ńu���b�N
	return p->ThreadMainLoop();
}

//------------------------------------------
// �X���b�h���C�����[�v
//------------------------------------------
int	ThreadManager::ThreadMainLoop()
{
	int nEventNo;
	int nRetc;

	nRetc = ThreadFirst();						// �X���b�h�J�n���R�[���o�b�N
	if (INVALID_HANDLE_VALUE != my_hThreadReady) { SetEvent(my_hThreadReady); }		// ���s�ҋ@��

	LOG(em_MSG), "[%s] �X���b�h�J�n=%d [0x%X]", my_sThreadName, nRetc, GetTid());
	while( 1 ) {
		//// �V�O�i���҂����ύX
		ThreadWaitFor(my_nWaitNum, &my_nWaitNumNow);

		//// �V�O�i���҂�
		DWORD ret = WaitForMultipleObjects((my_nWaitNumNow+1), my_pEvWait, FALSE, INFINITE);
		nEventNo = ret - WAIT_OBJECT_0; 

//-----------------------------------------------------------------------------------------------
		// �X���b�h�I��
		if( 0 == nEventNo ) {
			nRetc = ThreadLast();				// �X���b�h�I�����R�[���o�b�N
			LOG(em_MSG), "[%s] �X���b�h�I��=%d [0x%X]", my_sThreadName, nRetc, GetTid());
			return nRetc;
		}

//-----------------------------------------------------------------------------------------------
		// ��M
		if( 1 <= nEventNo && nEventNo <= my_nWaitNum ) {
			ThreadEvent(nEventNo-1);				// �C�x���g���R�[���o�b�N
			continue;
		}
	}

	// ���肦�Ȃ�
	LOG(em_ERR), "[%s] ���C�����[�v���甲�����I�I=%d [0x%X]", my_sThreadName, nRetc, GetTid());
	return -1;
}