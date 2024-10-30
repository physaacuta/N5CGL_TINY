#include "StdAfx.h"
#include "ControlDefragManager.h"

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
ControlDefragManager::ControlDefragManager(void) :
ThreadManager("CntDefragMgr"),
m_nClassCnt(0),
m_bExecDefrag(false)
{
	// ������
	for(int ii=0; ii<CONTROL_MAXCNT; ii++) mcls_pDefrag[ii] = NULL;
	//// �n���h������
	m_evTimerExec  = CreateWaitableTimer(NULL, FALSE, NULL);

	m_nDefragIntervalDay = GetPrivateProfileInt(TI_DELETE, "DEFRAG_INTERVAL_DAY", 0, TASKINI_NAME);
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
ControlDefragManager::~ControlDefragManager(void)
{
	for(int ii=0; ii<CONTROL_MAXCNT; ii++) {
		SAFE_DELETE( mcls_pDefrag[ii] );
	}
	//// �n���h�����
	CloseHandle(m_evTimerExec);
}


//------------------------------------------
// �Ǘ��X���b�h����
// int id ������ID (1�I���W��)
// CString cSession �Z�b�V����
//------------------------------------------
void ControlDefragManager::WorkerNew(int id, char* cSession)
{
	if( ! IsDefragEnable() ) return;

	//// ���[�J�[�X���b�h ������
	if( m_nClassCnt < id ) m_nClassCnt = id;
	mcls_pDefrag[id-1] = new DefragManager(id, cSession);
	mcls_pDefrag[id-1]->SetLogMgr( mcls_pLog );
	mcls_pDefrag[id-1]->SetExecDefragPnt(&m_bExecDefrag);
	mcls_pDefrag[id-1]->SetRunCnacel(false);	// ���s��
}

//------------------------------------------
// �Ǘ��X���b�h �X�g�b�v�ʒm
//------------------------------------------
void ControlDefragManager::WorkerStop()
{
	if( ! IsDefragEnable() ) return;

	for(int ii=0; ii<CONTROL_MAXCNT; ii++) {
		if( NULL == mcls_pDefrag[ii]) continue;
		
		mcls_pDefrag[ii]->SetRunCnacel(true);	// ���s�s��
	}
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int ControlDefragManager::ThreadFirst()
{

	//// �q�����J�n
	for(int ii=0; ii<CONTROL_MAXCNT; ii++) {
		if( NULL == mcls_pDefrag[ii]) continue;
		
		mcls_pDefrag[ii]->Start();
	}


	//// ���O����
	const int nCycle = 2 * 60 * 1000;
//	const int nCycle = 3 * 1000;

	__int64 ts = nCycle * -10000; 
	SetWaitableTimer(m_evTimerExec, (LARGE_INTEGER *)&ts, nCycle, NULL, NULL, FALSE);

	//// �C�x���g�ݒ�
	HANDLE hArray[] = { m_evTimerExec
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int ControlDefragManager::ThreadLast()
{
	// �������~
	CancelWaitableTimer(m_evTimerExec);


	// �q������~
	for(int ii=0; ii<CONTROL_MAXCNT; ii++) {
		if( NULL == mcls_pDefrag[ii]) continue;
		
		mcls_pDefrag[ii]->Stop();
	}
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void ControlDefragManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_TIMER_EXEC = 0,				// �������s ������^�C�}
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_TIMER_EXEC: 					// �������s ������^�C�}
		Exec();
		return;
	
//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

//------------------------------------------
// �C���f�b�N�X �f�t���O���s�۔���
//------------------------------------------
void ControlDefragManager::Exec()
{
	char	cWk[2048];
	GetPrivateProfileString(TI_DELETE, "SQL_DEFRAG_RUN", "", cWk, sizeof(cWk), TASKINI_NAME);


	// ���s
	int		nExec;		// �f�t���O���s��=1, �s��=0
	OdbcBase clsDB;
	clsDB.GetSelectKey_Static(cWk, &nExec, TASKINI_NAME);

	// �Z�b�g
	m_bExecDefrag = nExec;
}