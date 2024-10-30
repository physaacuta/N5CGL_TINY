#include "StdAfx.h"
#include "SyncManager.h"

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
SyncManager::SyncManager(CString cSession, LogFileManager* pLog):
KizuSyncroManager(cSession, pLog)
{
	LOG(em_INF), "[SyncManager] [%s] �������J�n -------->>>>", my_cSession);

	// �����Ώۃe�[�u����荞��
	char    cWk[128];
	CString strVal;
	memset( m_Tbl, 0x00, sizeof(m_Tbl));
	for(int ii=0; ii<PARAM_MAX_TABLE; ii++ ) {
		strVal.Format("T_%d", ii+1);
		GetPrivateProfileString(my_cSession, strVal, "", cWk, sizeof(cWk), TASKINI_NAME);		
		if( 0 < strlen(cWk) ) {
			memcpy(m_Tbl[ii].cTbl, cWk, sizeof(cWk));
			m_Tbl[ii].bEnable = true;
			LOG(em_INF), "[SyncManager] T_%d ��� %s", ii+1, m_Tbl[ii].cTbl);

		} else {
			//_ASSERT(false);	
		}
	}
	LOG(em_INF), "[SyncManager] [%s] ���������� <<<<--------", my_cSession);
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
SyncManager::~SyncManager(void)
{
}

//------------------------------------------
// �X���b�h
//------------------------------------------
UINT WINAPI SyncManager::MainThread(LPVOID param)
{
	////// ���C���C���X�^���X���o��
	SyncManager *p = (SyncManager *)param;					// ���N���X�̃C���X�^���X���擾
	p->SetTid();											// ���X���b�hID�Z�b�g

	////////////////////////////////////
	// ���O����
	////////////////////////////////////
	//// ������^�C�}�[�Z�b�g
	HANDLE		evTimeCycle;										// �w���V�[		
	evTimeCycle	= CreateWaitableTimer(NULL, FALSE, NULL);	
	if(0 != p->my_nTimeFast) {
		__int64 ts = (__int64)p->my_nTimeFast * (__int64)-10000;	// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
		SetWaitableTimer(evTimeCycle,   (LARGE_INTEGER *)&ts, p->my_nTimeInterval, NULL, NULL, FALSE);
	}

	////////////////////////////////////
	// �ʏ�̃��C���X���b�h����
	////////////////////////////////////

	// WaitForMultipleObjects�C�x���g
	HANDLE	hArray[] = {p->my_evStop,
						evTimeCycle,
						p->my_evNowExecAtoB,
						p->my_evNowExecBtoA
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// �X���b�h�I��
			EV_TIMECYCLE,					// ������^�C�}�[
			EV_EXEC_AtoB,					// A��B �}�X�^�[�����J�n
			EV_EXEC_BtoA,					// B��A �}�X�^�[�����J�n
			EV_END	};

	// ���b�Z�[�W�L���[�ǂݍ���
	pLOG(em_MSG), "[SyncroMaster] �X���b�h�J�n [0x%X]", p->GetTid());
	while (1) {
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);

		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// �X���b�h�I��
			pLOG(em_MSG), "[SyncroMaster] [%s] �X���b�h�I�� [0x%X]", p->my_cSession, p->GetTid());
			CancelWaitableTimer(evTimeCycle);
			CloseHandle(evTimeCycle);
			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_TIMECYCLE:											// ������^�C�}�[
		case EV_EXEC_AtoB:											// A��B �����J�n
			pLOG(em_MSG), "[SyncroMaster] [%s] A��B �}�X�^�[�����J�n -------------->>>", p->my_cSession);
			
			p->Exec(true);

			pLOG(em_MSG), "[SyncroMaster] [%s] A��B �}�X�^�[�������� <<<--------------", p->my_cSession);
			break;

//-----------------------------------------------------------------------------------------------
		case EV_EXEC_BtoA:											// B��A �����J�n
			pLOG(em_WAR), "[SyncroMaster] [%s] B��A �}�X�^�[�����J�n -------------->>>", p->my_cSession);
			
			p->Exec(false);

			pLOG(em_WAR), "[SyncroMaster] [%s] B��A �}�X�^�[�������� <<<--------------", p->my_cSession);
			break;


//-----------------------------------------------------------------------------------------------
		default :
			// ���肦�Ȃ��I�I
			break;
		}
	}
	return 0;
}


//------------------------------------------
// 2DB�Ԃ̃e�[�u���𓯂��ɂ���
// bool bAtoB    true:A��B  false:B��A
//------------------------------------------
bool SyncManager::Exec(bool bAtoB)
{
	//// ���C���ƃT�u�������Ă���H
	if( ! CheckDB_Connect() ) {
		LOG(em_WAR), "[SyncManager] [%s] 2DB�ڑ����s", my_cSession);
		return false;
	}

	//// DB�ڑ�
	OdbcBase clsDbA;
	OdbcBase clsDbB;
	this->SetOdbcBaseA(&clsDbA); 
	this->SetOdbcBaseB(&clsDbB); 

	//// �}�X�^�[�������s
	for(int ii=0; ii<PARAM_MAX_TABLE; ii++) {
		if( m_Tbl[ii].bEnable ) {
			// ����
			Synclock_Master(bAtoB, m_Tbl[ii].cTbl);
			if(my_bStopFlg) goto Ending;	 // �I���H

			// �҂�
			Sleep(1000);
		}

		if(my_bStopFlg) goto Ending;	 // �I���H
	}

Ending:
	//// DB�ڑ��J��
	this->SetOdbcBaseA(NULL); 
	this->SetOdbcBaseB(NULL); 
	return true;
}


