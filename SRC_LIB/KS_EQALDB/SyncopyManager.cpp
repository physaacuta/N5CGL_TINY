#include "StdAfx.h"
#include "SyncopyManager.h"

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
SyncopyManager::SyncopyManager(CString cSession, LogFileManager* pLog):
KizuSyncroManager(cSession, pLog)
{
	LOG(em_INF), "[SyncopyManager] [%s] �������J�n -------->>>>", my_cSession);

	//// ���шړ��Ώۃe�[�u����荞��
	char    cWk[1024];
	CString strVal;
	memset( m_Tbl, 0x00, sizeof(m_Tbl));
	for(int ii=0; ii<PARAM_MAX_TABLE; ii++ ) {
		strVal.Format("T_%d", ii+1);
		GetPrivateProfileString(my_cSession, strVal, "", cWk, sizeof(cWk), TASKINI_NAME);		
	
		if( 0 < strlen(cWk) ) {
			strVal = cWk;
			
			// �g�[�N������
			int		rec = 0;
			int		curPos = 0;
			CString strToken = strVal.Tokenize(",", curPos);
			while ( strToken != "" ) {
				strToken.Trim(_T(" 	"));		// ini�t�@�C���ݒ蕶���̑O��̃X�y�[�X�A�^�u����菜��

				
				if( 0 == rec ) {
					strcpy(m_Tbl[ii].cTbl, strToken); 
				} else if( 1 == rec ) {
					m_Tbl[ii].nExecCnt = atoi(strToken); 

				} else {
					if( PARAM_MAX_KEYCNT <= m_Tbl[ii].nKeyCnt ) break;
					// ��L�[�w��
					if( 0 != strToken.GetLength() ) {
						strcpy(m_Tbl[ii].cKey[m_Tbl[ii].nKeyCnt], strToken); 
						m_Tbl[ii].nKeyCnt += 1;
					}
				}

				// ����
				strToken = strVal.Tokenize(",", curPos);
				rec ++;
			}

			if( 0 >= m_Tbl[ii].nKeyCnt || 0 >= m_Tbl[ii].nExecCnt ) {
				_ASSERT(false);			
				break;		// ���������g�[�N�����L���H
			}
			m_Tbl[ii].bEnable = true;
			LOG(em_INF), "[SyncopyManager] T_%d ��� %s, [%d=%s,%s,%s]", ii+1, m_Tbl[ii].cTbl, m_Tbl[ii].nKeyCnt, m_Tbl[ii].cKey[0], m_Tbl[ii].cKey[1], m_Tbl[ii].cKey[2] );
		}
	}
	LOG(em_INF), "[SyncopyManager] [%s] ���������� <<<<--------", my_cSession);
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
SyncopyManager::~SyncopyManager(void)
{
}

//------------------------------------------
// �X���b�h
//------------------------------------------
UINT WINAPI SyncopyManager::MainThread(LPVOID param)
{
	////// ���C���C���X�^���X���o��
	SyncopyManager *p = (SyncopyManager *)param;		// ���N���X�̃C���X�^���X���擾
	p->SetTid();										// ���X���b�hID�Z�b�g


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
	pLOG(em_MSG), "[SyncopyManager] [%s] �X���b�h�J�n [0x%X]", p->my_cSession, p->GetTid());
	while (1) {
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);

		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// �X���b�h�I��
			pLOG(em_MSG), "[SyncopyManager] [%s] �X���b�h�I�� [0x%X]", p->my_cSession, p->GetTid());
			CancelWaitableTimer(evTimeCycle);
			CloseHandle(evTimeCycle);
			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_TIMECYCLE:											// ������^�C�}�[
			pLOG(em_MSG), "[SyncopyManager] [%s] ���ѓ����J�n -------------->>>", p->my_cSession);
		
			p->Exec(true);
			p->Exec(false);

			pLOG(em_MSG), "[SyncopyManager] [%s] ���ѓ������� <<<--------------", p->my_cSession);
			break;

		case EV_EXEC_AtoB:											// A��B �����J�n
			pLOG(em_MSG), "[SyncopyManager] [%s] A��B ���ѓ����J�n -------------->>>", p->my_cSession);
		
			p->Exec(true);

			pLOG(em_MSG), "[SyncopyManager] [%s] A��B ���ѓ������� <<<--------------", p->my_cSession);
			break;


//-----------------------------------------------------------------------------------------------
		case EV_EXEC_BtoA:											// B��A �����J�n
			pLOG(em_WAR), "[SyncopyManager] [%s] B��A ���ѓ����J�n -------------->>>", p->my_cSession);
			
			p->Exec(false);

			pLOG(em_WAR), "[SyncopyManager] [%s] B��A ���ѓ����J�n <<<--------------", p->my_cSession);
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
// B�n�̎��т�A�n�Ɉړ�������
// bool bAtoB    true:A��B  false:B��A
//------------------------------------------
bool SyncopyManager::Exec(bool bAtoB)
{
	//// ���C���ƃT�u�������Ă���H
	if( ! CheckDB_Connect() ) {
		LOG(em_WAR), "[SyncopyManager] [%s] 2DB�ڑ����s", my_cSession);
		return false;
	}


	//// DB�ڑ�
	OdbcBase clsDbA;
	OdbcBase clsDbB;
	this->SetOdbcBaseA(&clsDbA); 
	this->SetOdbcBaseB(&clsDbB); 

	//// ���уR�s�[���s
	for(int ii=0; ii<PARAM_MAX_TABLE; ii++) {
		if( m_Tbl[ii].bEnable ) {
			// ���шړ�
			Syncopy_DB(bAtoB, m_Tbl[ii].cTbl, m_Tbl[ii].nExecCnt, m_Tbl[ii].nKeyCnt, (char const*)m_Tbl[ii].cKey );	
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
