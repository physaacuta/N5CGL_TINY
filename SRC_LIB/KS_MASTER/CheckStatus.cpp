#include "StdAfx.h"
#include "CheckStatus.h"

//------------------------------------------
// �R���X�g���N�^
// bool isMaster �}�X�^�[PC�Ǘ���true
//------------------------------------------
CheckStatus::CheckStatus(bool isMaster) :
m_bIsMaster(isMaster),
mcls_pLog(NULL)
{
	//// ������
	InitializeCriticalSection(&m_csLock);							// �N���e�B�J���Z�N�V�����I�u�W�F�N�g��������
	m_nStateCount = 0;
	memset( m_typStateCheck, 0x00, sizeof(m_typStateCheck) );

	//// �V�O�i������
	m_evHBeatCheck = CreateWaitableTimer(NULL, FALSE, NULL);
	m_evHBeatSend = CreateWaitableTimer(NULL, FALSE, NULL);

	//// ini�t�@�C���f�[�^�擾
	Get_IniData();
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
CheckStatus::~CheckStatus(void)
{
	//// �J��
	DeleteCriticalSection(&m_csLock);								// �N���e�B�J���Z�N�V�����I�u�W�F�N�g���J��
	//// �V�O�i���J��
	CloseHandle(m_evHBeatCheck);
	CloseHandle(m_evHBeatSend);
}

//------------------------------------------
// �X���b�h
//------------------------------------------
UINT WINAPI CheckStatus::MainThread(LPVOID param)
{
	////// ���C���C���X�^���X���o��
	CheckStatus *p = (CheckStatus *)param;
	p->SetTid();								// ���X���b�hID�Z�b�g

	// WaitForMultipleObjects�C�x���g
	HANDLE	hArray[] = {p->my_evStop,
						p->m_evHBeatCheck,
						p->m_evHBeatSend
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// �X���b�h�I��
			EV_HB_CHECK,					// �n�[�g�r�[�g�Ď��^�C�}�C�x���g
			EV_HB_SEND,						// �n�[�g�r�[�g���M�^�C�}�C�x���g
			EV_END	};
	

	//// �^�C�}�ݒ�
	// �n�[�g�r�[�g�`�F�b�N�^�C�}
	if( p->m_bIsMaster || 0 != p->m_nMasterCheckID ) {
		LARGE_INTEGER ts;
		ts.QuadPart = (__int64)HEART_BEAT_CHECK_CYCLE * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
		SetWaitableTimer(p->m_evHBeatCheck, &ts, HEART_BEAT_CHECK_CYCLE, NULL, NULL, FALSE);		
	}

	//// �n�[�g�r�[�g���M�^�C�}
	if( 0 != p->m_nSendHBID ) {
		LARGE_INTEGER ts;
		ts.QuadPart = (__int64)HEART_BEAT_SEND_CYCLE * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
		SetWaitableTimer(p->m_evHBeatSend, &ts, HEART_BEAT_SEND_CYCLE, NULL, NULL, FALSE);		
	}


	// ���b�Z�[�W�L���[�ǂݍ���
	pLOG(em_MSG), "[CheckStatus] �X���b�h�J�n [0x%X]", p->GetTid());
	while (1) {
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);

		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// �X���b�h�I��
			pLOG(em_MSG), "[CheckStatus] �X���b�h�I�� [0x%X]", p->GetTid());
			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_HB_CHECK:											// �n�[�g�r�[�g�Ď��^�C�}�C�x���g
			p->Check_HeatBeat();
			break;

//-----------------------------------------------------------------------------------------------
		case EV_HB_SEND:											// �n�[�g�r�[�g���M�^�C�}�C�x���g
			p->Send_HeatBeat();
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
// �f�[�^�m��
//------------------------------------------
void CheckStatus::Alloc()
{
	//// �@���ԃe�[�u��������
	if(m_bIsMaster) Init_StatusInf();

	//// �@��ID���擾
	if(m_bIsMaster)					 Get_Status_ID(0);					// �}�X�^�[PC�Ǘ�
	else if( 0 != m_nMasterCheckID ) Get_Status_ID(m_nMasterCheckID);	// �X���[�uPC�Ǘ� ���� �����n�[�g�r�[�g�m�F�L�莞
};

//------------------------------------------
// ini�t�@�C���f�[�^�擾
//------------------------------------------
void CheckStatus::Get_IniData()
{
	CString cSession;
	cSession = m_bIsMaster ? KS_MASTER : KS_SLAVE;

	// �n�[�g�r�[�g���M�L��
	m_nSendHBID = 0;
	m_nSendHBID = GetPrivateProfileInt(cSession, "HB_SEND_ENABLE", 0, PCINI_NAME);

	// �����n�[�g�r�[�g�m�F�L�� (�X���[�uPC�Ǘ��̂�)
	m_nMasterCheckID = 0;
	if( ! m_bIsMaster ) {
		m_nMasterCheckID = GetPrivateProfileInt(cSession, "HB_MASTER_CHECK", 0, PCINI_NAME);
	}
}

//------------------------------------------
// �@���ԃe�[�u�������� (�}�X�^�[���̂�)
//------------------------------------------
bool CheckStatus::Init_StatusInf()
{
	AutoLock autolock(&m_csLock);

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase cls_DB; 
	if( !cls_DB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[ (�܂����肦�Ȃ�)
		LOG(em_ERR), "[CheckStatus] �ڑ��G���[");
		syslog(SYSNO_DB_CONNECT_ERR,"");
		return false;
	}

	//// �o�^�����擾
	CString cSql;
	int nCnt;
	cSql.Format("SELECT COUNT(*) FROM %s ", DB_STATUS_MASTER);
	cls_DB.GetSelectKey(cSql, &nCnt); 

	//// �X�V
	for( int ii=1; ii<=nCnt; ii++ ) {
/*
UPDATE STATUS_INF SET �@����=(select �����l from STATUS_MASTER where �@��ID=3)
where �@��ID=3 and �@��ID=(select �@��ID from STATUS_MASTER where �����l is not null and �@��ID=3)
*/
		cSql.Format("UPDATE %s SET �@����=(SELECT �����l FROM %s WHERE �@��ID=%d) WHERE �@��ID=(SELECT �@��ID FROM %s WHERE �����l IS NOT NULL AND �@��ID=%d)",
					DB_STATUS_INF, DB_STATUS_MASTER, ii, DB_STATUS_MASTER, ii );

		// �X�V
		if( ! cls_DB.ExecuteDirect(cSql) ) {
			// �f�[�^�x�[�X�ڑ��G���[ (�܂����肦�Ȃ�)
			LOG(em_ERR), "[CheckStatus] UPDATE�G���[ [%s]", cSql);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", DB_STATUS_INF);
		}
	}
	//// �R�~�b�g
	cls_DB.TranCommit();
	//// �f�[�^�x�[�X�ؒf
	cls_DB.DisConnect();
	
	LOG(em_MSG), "[CheckStatus] �@���ԃe�[�u������������");
	return true;
}

//------------------------------------------
// �@��ID���擾
// int mode ���[�h (�}�X�^�[��:0�Œ�   �X���[�u���F�����m�F����Y���@��ID�B�`�F�b�N�����̏ꍇ�́A���������R�[������Ȃ�)
//------------------------------------------
bool CheckStatus::Get_Status_ID(int mode)
{
	AutoLock autolock(&m_csLock);

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase cls_DB;
	if( !cls_DB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[ (�܂����肦�Ȃ�)
		LOG(em_ERR), "[CheckStatus] �ڑ��G���[");
		syslog(SYSNO_DB_CONNECT_ERR,"");
		return false;
	}

	//// �n�[�g�r�[�g���s���@����擾
	// SQL������
	CString cSql;
	cSql.Format("SELECT m.�@��ID, m.�@�햼, i.�@����, m.����m�F�L�� FROM %s m, %s i WHERE m.�@��ID=i.�@��ID",
				DB_STATUS_MASTER, DB_STATUS_INF );
	if( 0 != mode ) {
		CString cWk;
		cWk.Format( " AND m.�@��ID=%d", mode );
		cSql += cWk;
	} else {
	//	cSql += " AND m.����m�F�L��=1";	// ����m�F�L��̂��̂����K�v����	// ��߁B�@�햼�ׂ̂̈��������ǑS���擾����
	}

	// ���s
	if( ! cls_DB.ExecuteDirect(cSql) ) {
		// �f�[�^�x�[�X�ڑ��G���[ (�܂����肦�Ȃ�)
		LOG(em_ERR), "[CheckStatus] SELECT�G���[ [%s]", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", DB_STATUS_MASTER);
		return false;
	}
	
	// �擾
	int nCnt = 0;
	while(true) {
		int sqlrc = cls_DB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��
			cls_DB.GetData(1, &m_typStateCheck[nCnt].id);
			cls_DB.GetData(2, &m_typStateCheck[nCnt].name, sizeof(m_typStateCheck[nCnt].name));
			cls_DB.GetData(3, &m_typStateCheck[nCnt].status);
			cls_DB.GetData(4, &m_typStateCheck[nCnt].nHb);
			m_typStateCheck[nCnt].statusHb = false;

			nCnt++;
		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", DB_STATUS_MASTER);
			return false;
		}
	}
	m_nStateCount = nCnt;
	LOG(em_MSG), "[CheckStatus] �@�퐔=%d", m_nStateCount );

	//// ����HB�`�F�b�N�̏ꍇ�́A�v���O�������Ńw���V�[�m�F�����t���O�𗧂Ă�
	if(0 != mode && 1 <= nCnt ) {
		m_typStateCheck[0].nHb = 1;
	}

	//// �f�[�^�x�[�X�ؒf
	cls_DB.DisConnect();
	return true;
}

//------------------------------------------
// �X�e�[�^�X�Z�b�g
// int id �@��ID
// bool status �X�e�[�^�X (true:����)
//------------------------------------------
void CheckStatus::Set_StatusID(int id, bool status)
{
	// �Y���@��ID���ǂ̈ʒu�H
	for(int ii=0; ii<m_nStateCount; ii++) {
		if( m_typStateCheck[ii].id == id ) {
			// ������
			Set_StatusIndex(ii, status);
			return ;
		}
	}
	LOG(em_ERR), "[CheckStatus] �@��ID���� [�@��ID=%d]", id);
}
void CheckStatus::Set_StatusIndex(int index, bool status)
{
	AutoLock autolock(&m_csLock);
	m_typStateCheck[index].status = status ? 0 : 1;
}

//------------------------------------------
// �n�[�g�r�[�g��������t���O�Z�b�g
// int id �@��ID
// bool status �X�e�[�^�X (true:����)
//------------------------------------------
void CheckStatus::Set_HeatBeatID(int id, bool status)
{
	if( !m_bIsMaster && 0 == m_nMasterCheckID ) return;

	// �Y���@��ID���ǂ̈ʒu�H
	for(int ii=0; ii<m_nStateCount; ii++) {
		if( m_typStateCheck[ii].id == id ) {
			// ������
			if(1 != m_typStateCheck[ii].nHb) { LOG(em_ERR), "[CheckStatus] �n�[�g�r�[�g�m�F�����̂͂��Ȃ̂Ƀn�[�g�r�[�g������ [�@��ID=%d]", id); }

			Set_HeatBeatIndex(ii, status);
			return ;
		}
	}
	LOG(em_ERR), "[CheckStatus] �n�[�g�r�[�g�̋@��ID���� [�@��ID=%d]", id);
}

void CheckStatus::Set_HeatBeatIndex(int index, bool status)
{
	AutoLock autolock(&m_csLock);
	m_typStateCheck[index].statusHb = status;
}

//------------------------------------------
// �n�[�g�r�[�g�`�F�b�N
//------------------------------------------
void CheckStatus::Check_HeatBeat()
{
	// �n�[�g�r�[�g�̋@��S�����`�F�b�N
	for(int ii=0; ii<m_nStateCount; ii++) {
		if(1 != m_typStateCheck[ii].nHb) continue;			// �n�[�g�r�[�g�m�F����

		if( m_typStateCheck[ii].statusHb ) {				// �n�[�g�r�[�g����
			// �����_�ňُ�H		
			if( 0 != m_typStateCheck[ii].status ) {
				// ���한�A
				LOG(em_INF), "[CheckStatus] �n�[�g�r�[�g���한�A�F%s", m_typStateCheck[ii].name );

				// ��ԕύX
				Set_StatusIndex(ii, true);
				KizuFunction::SetStatus(m_typStateCheck[ii].id, true, true, "[�n�[�g�r�[�g]");
			}

		} else {									// �n�[�g�r�[�g���ĂȂ�
			// �����_�Ő���H
			if( 0 == m_typStateCheck[ii].status ) {
				// ���한�A
				LOG(em_WAR), "[CheckStatus] �n�[�g�r�[�g�ُ팟�o�F%s", m_typStateCheck[ii].name );
				
				// ��ԕύX
				Set_StatusIndex(ii, false);
				KizuFunction::SetStatus(m_typStateCheck[ii].id, false, true, "[�n�[�g�r�[�g]");		
			}
		}

		// ����
		Set_HeatBeatIndex(ii, false );
	}
}

//------------------------------------------
// �n�[�g�r�[�g���M
//------------------------------------------
void CheckStatus::Send_HeatBeat()
{
	COMMON_QUE que;
	if(m_bIsMaster) {	// �}�X�^�[ �� �X���[�u
		que.nEventNo = FACT_KS_SLAVE_02;
		que.nLineNo = getlineid();															// ���C��ID
		que.fl.data[0] = m_nSendHBID;  
		int retc = send_mail( KS_SLAVE, "*", &que );		// �󂯎��悪�ǂꂩ�m��Ȃ��ׁAUDP�ő��M
		if( 0 != retc ) syslog(SYSNO_MAIL_SNED_ERR, "[%d:�n�[�g�r�[�g�ʒm <err=%d>]", FACT_KS_SLAVE_02, retc);

	} else {				// �X���[�u �� �}�X�^�[
		que.nEventNo = FACT_KS_MASTER_02;
		que.nLineNo = getlineid();															// ���C��ID
		que.fl.data[0] = m_nSendHBID;  
		int retc = send_mail( KS_MASTER, MASTER_PC_NAME, &que );
		if( 0 != retc ) syslog(SYSNO_MAIL_SNED_ERR, "[%d:�n�[�g�r�[�g�ʒm <err=%d>]", FACT_KS_MASTER_02, retc);
	}
}

//------------------------------------------
// �@��ID����z��ʒu���擾����
// int id �@��ID
// �߂�l �z��ʒu (0�I���W��) -1:�Y������
//------------------------------------------
int CheckStatus::GetIndex(int id)
{
	for(int ii=0; ii<m_nStateCount; ii++) {
		if( m_typStateCheck[ii].id == id ) return ii;
	}
	return -1;
}
//------------------------------------------
// �Y���̋@��ID�̋@�햼�̂��擾
// int id �@��ID
// �߂�l �@�햼��
//------------------------------------------
char const * CheckStatus::GetName(int id)
{
	// �Y���@��ID���ǂ̈ʒu�H
	for(int ii=0; ii<m_nStateCount; ii++) {
		if( m_typStateCheck[ii].id == id ) {
			return m_typStateCheck[ii].name ;
		}
	}
	return NULL;
}
