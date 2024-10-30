#include "StdAfx.h"
#include "CheckDB.h"


//------------------------------------------
// �R���X�g���N�^
// bool isMaster �}�X�^�[PC�Ǘ���true
//------------------------------------------
CheckDB::CheckDB(bool isMaster) :
m_bIsMaster(isMaster),
m_bStateDB(false),
m_bFastExec(true),
mcls_pLog(NULL)
{
	//// �V�O�i������
	m_evDBCheck = CreateWaitableTimer(NULL, FALSE, NULL);

	//// ini�t�@�C���f�[�^�擾
	Get_IniData();
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
CheckDB::~CheckDB(void)
{
	//// �V�O�i���J��
	CloseHandle(m_evDBCheck);
}

//------------------------------------------
// �X���b�h
//------------------------------------------
UINT WINAPI CheckDB::MainThread(LPVOID param)
{
	////// ���C���C���X�^���X���o��
	CheckDB *p = (CheckDB *)param;
	p->SetTid();								// ���X���b�hID�Z�b�g

	// WaitForMultipleObjects�C�x���g
	HANDLE	hArray[] = {p->my_evStop,
						p->m_evDBCheck
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// �X���b�h�I��
			EV_DB_CHECK,					// DB�Ď��^�C�}�C�x���g
			EV_END	};
	

	//// �^�C�}�ݒ�
	// �n�[�g�r�[�g�`�F�b�N�^�C�}
	LARGE_INTEGER ts;
	ts.QuadPart = 0;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(p->m_evDBCheck, &ts, HEART_BEAT_DBCHECK_CYCLE, NULL, NULL, FALSE);

	// ���b�Z�[�W�L���[�ǂݍ���
	pLOG(em_MSG), "[CheckDB] �X���b�h�J�n [0x%X]", p->GetTid());
	while (1) {
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);

		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// �X���b�h�I��
			pLOG(em_MSG), "[CheckDB] �X���b�h�I�� [0x%X]", p->GetTid());
			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_DB_CHECK:											// DB�Ď��^�C�}�C�x���g
			p->Check_DataBase();
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
// ini�t�@�C���f�[�^�擾
//------------------------------------------
void CheckDB::Get_IniData()
{
	CString cSession;
	cSession = m_bIsMaster ? KS_MASTER : KS_SLAVE;

	// DB������`�F�b�N�L��
	int nWk;
	nWk = GetPrivateProfileInt(cSession, "DB_ENABLE", 0, PCINI_NAME);
	m_bCheckDB = nWk == 1 ? true : false;

	// ���M�^�X�N�擾
	CString cWk;
	m_nSendTaskCnt = 0;
	memset(m_cSendTaskName, 0x00, sizeof(m_cSendTaskName));
	for( int ii=0; ii<MAX_SEND_TASK; ii++ ) {
		cWk.Format("DB_SENDTASK%d", ii+1 );
		GetPrivateProfileString(cSession, cWk, "", m_cSendTaskName[ii], SIZE_NAME_TASK, PCINI_NAME);
		if( 0==strlen(m_cSendTaskName[ii]) ) break;
		m_nSendTaskCnt ++;
	}
}

//------------------------------------------
// DB�`�F�b�N
//------------------------------------------
void CheckDB::Check_DataBase()
{
	COMMON_QUE que;
	OdbcBase cls_DB;									// ODBC�ڑ��N���X �C���X�^���X����

	if( m_bFastExec ) {												// �����ڑ���
		// DB�����グ�m�F����
		//if( cls_DB.OpenCheck( TASKINI_NAME ) ) {							// �ڑ�
		if( OpenCheckEx() ) {

			//// �����ڑ�����
			m_bStateDB = true;
			m_bFastExec = false;
			CancelWaitableTimer(m_evDBCheck);
			// �����DB�`�F�b�N�L��
			if( m_bCheckDB ) {
				// ������X���b�h����
				LARGE_INTEGER ts;
				ts.QuadPart = (__int64)HEART_BEAT_DBCHECK_CYCLE * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
				SetWaitableTimer(m_evDBCheck, &ts, 0, NULL, NULL, FALSE);		
			}
			// �ڑ������ʒm
			PostThreadMessage(GetMainPostTid(), E_DEF_DB_FAST_CONNECT, 0, 0);
			LOG(em_MSG), "DB�ڑ� ����");

		} else {															// ���ڑ�
			//// �����ڑ����s
			LOG(em_WAR), "DB�ڑ��҂����");
		}

	} else {														// DB������m�F��
		
		// �o�^�^�X�N�֒ʒm
//LOG(em_MSG), "DB�ڑ��J�n --->>");
		bool bState = cls_DB.OpenCheck( TASKINI_NAME );
		for(int ii=0; ii<=4; ii++) {
			if( m_bStateDB && ! bState ) {
				// ���큨�ُ�ƂȂ�ꍇ�̂� �O�ׂ̈ɍă`�F�b�N
				LOG(em_WAR), "DB�ڑ� �ُ�?? %d��ڊJ�n", ii+1); 
				syslog(SYSNO_DB_CHECK_RETRY, "���g���C%d��ڊJ�n", ii+1);
				Sleep(100);
				bState = cls_DB.OpenCheck( TASKINI_NAME );
			}
		}
//LOG(em_MSG), "DB�ڑ����� <<---");


		que.nEventNo = FACT_KS_DB_STATEUS;
		que.nLineNo = getlineid();											// ���C��ID
		que.fl.data[0] = (bState ? 0 : 1 );								// 0:���펞
		for(int ii=0; ii<m_nSendTaskCnt; ii++) {
			send_mail( m_cSendTaskName[ii], "*", &que );
		}
		if( m_bStateDB != bState ) {
			m_bStateDB = bState;
			if(bState)	{ 
				// ���C���C���X�^���X�ɕ���
				LOG(em_MSG), "DB�ڑ� ���한�A");
				PostThreadMessage(GetMainPostTid(), E_DEF_DB_NG_TO_OK, 0, 0);

			} else		{ 
				LOG(em_ERR), "DB�ڑ� �ُ�"); 
				syslog(SYSNO_DB_CONNECT_ERR, "");
			}
		}
		// ������X���b�h����
		LARGE_INTEGER ts;
		ts.QuadPart = (__int64)HEART_BEAT_DBCHECK_CYCLE * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
		SetWaitableTimer(m_evDBCheck, &ts, 0, NULL, NULL, FALSE);	
	}
}

//------------------------------------------
// DB�ڑ��m�F
//------------------------------------------
bool CheckDB::OpenCheckEx()
{
	// �ǂ����~��PC�N������� �I�[�v���`�F�b�N���Ă� �C���T�[�g�Ƃ��ł��܂Ɏ��s�ƂȂ�ׁASelect�𔭍s����OK�Ȃ�z���g�Ɋ����Ƃ���
	OdbcBase	cls_DB;
	CString		sql;
	int			nVal;

	// ���������q�����Ă���H
	if( ! cls_DB.Connect( TASKINI_NAME ) ) return false;

	// Select�m�F
	LOG(em_MSG), "DB�ڑ� �m�F�����B������Select�m�F");
	sql.Format("SELECT ISNULL(MIN(�@��ID), 0) FROM %s", DB_STATUS_MASTER); 
	if( ! cls_DB.GetSelectKey( sql, &nVal ) ) {
		LOG(em_ERR), "Select�ňُ�");
		cls_DB.DisConnect();
		return false;
	}
	if( 0 == nVal ) return true;		// �f�[�^�������̂Ń���

	// UPDATE�m�F
	LOG(em_MSG), "DB Select �m�F�����B������Update�m�F");
	sql.Format("UPDATE %s SET �@��ID=%d WHERE �@��ID=%d", DB_STATUS_MASTER, nVal, nVal );
	if( ! cls_DB.ExecuteDirect(sql) ) {
		LOG(em_ERR), "UpDate�ňُ�");
		cls_DB.DisConnect();
		return false;
	}

	// �����܂ŗ�����܂��ԈႢ�Ȃ��͂��E�E�E
	cls_DB.DisConnect();
	return true;
}