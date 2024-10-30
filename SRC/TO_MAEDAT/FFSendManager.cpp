#include "StdAfx.h"
#include "FFSendManager.h"

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
FFSendManager::FFSendManager(void) :
ThreadManager("FFSendManager")
{
	// �V�O�i������
	m_evFFSend		= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evTimeCycle	= CreateEvent(NULL, FALSE, FALSE, NULL);
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
FFSendManager::~FFSendManager(void)
{
	// �C�x���g�J��
	CloseHandle( m_evFFSend );
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int FFSendManager::ThreadFirst()
{
	////////////////////////////////////
	// ���O����
	////////////////////////////////////
	//// PIO��Ԋm�F ������^�C�}�[�Z�b�g
	__int64 ts = (__int64)FFSEND_TIMER * -10000;	// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)

	m_evTimeCycle	= CreateWaitableTimer( NULL, FALSE, NULL );			
	SetWaitableTimer( m_evTimeCycle,   (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE );

	//// �C�x���g�ݒ�
	HANDLE	hArray[] = {m_evFFSend,
						m_evTimeCycle};

	SetEventNum( sizeof(hArray)/sizeof(HANDLE), hArray );
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int FFSendManager::ThreadLast()
{
	CancelWaitableTimer( m_evTimeCycle );
	CloseHandle( m_evTimeCycle );

	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void FFSendManager::ThreadEvent( int nEventNo )
{
	//// PIO��Ԋm�F ������^�C�}�[�Z�b�g
	__int64 ts = (__int64)FFSEND_TIMER * -10000;	// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)

	////// �V�O�i���̏���
	enum {	EV_FFSEND,						// FF���M�v��
			EV_TIMECYCLE,					// ��������s
			};

	////// �V�O�i����������
	switch ( nEventNo ) {

//-----------------------------------------------------------------------------------------------
	case EV_FFSEND:
		LOG(em_MSG), "[%s] FF���M�v���`�F�b�N����", my_sThreadName );
		FFSendMain();
		
		// ���̃^�C�}�[�𔭍s
		CancelWaitableTimer( m_evTimeCycle );
		SetWaitableTimer( m_evTimeCycle,   (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE );
		break;

//-----------------------------------------------------------
	case EV_TIMECYCLE:
		LOG(em_MSG), "[%s] ��������� [0x%X]", my_sThreadName, GetTid() );
		FFSendMain();

		// ���̃^�C�}�[�𔭍s
		CancelWaitableTimer( m_evTimeCycle );
		SetWaitableTimer( m_evTimeCycle,   (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE );
		break;

//-----------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		break;
	}
}

//------------------------------------------
// �O�H�����їv�����C������
//------------------------------------------
void FFSendManager::FFSendMain()
{
	CString sql;					// SQL��
	CString wk;
	CString cDbName;
	cDbName = DB_MAE_COIL_INF;

	CTime t = CTime::GetCurrentTime() - CTimeSpan( 1, 0, 0, 0 );	// 1���O
	CString strTime = t.Format("'%Y/%m/%d %H:%M:%S'");
	char	cKeyinf[SIZE_COIL_KEY + 1];						// �L�[���
	char	cLine_Code[SIZE_KOUTEI_CODE + 1];				// �H���R�[�h
	char	cMeiNo[SIZE_MEI_NO + 1];						// ����No
	char	cCoilNo[SIZE_COIL_NO + 1];						// �R�C��No

	memset( &cKeyinf, 0x00, sizeof(cKeyinf) );

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect( TASKINI_NAME ) ) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName );
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return;
	}

	//// SQL���쐬(����No�擾)
	sql =  "SELECT";
	sql += " �L�[���, ����No, �H���R�[�h, �R�C��No ";
	sql += " FROM ";
	sql += cDbName;
	sql += " WHERE FF�O�H���r����M�t���O = -1";
	sql += " AND ���Ǘ�No�t���O = -1 ";
	wk.Format( " AND �o�^���t > %s", strTime );
	sql += wk;
	sql += " ORDER BY �Ǘ�No ASC ";		// �ߋ����珇�Ԃɑ��M(�o�^���t�ł́A�������R�[�h�o�^���ɒʏ��̍Ōォ��v������Ă��܂�����)
	//sql += " ORDER BY �o�^���t ASC ";
	
	//// ���s
	if( ! clsDB.ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		LOG(em_ERR), "[%s] �O�H���R�C�������s�G���[ [%s]", my_sThreadName, cDbName );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return;
	}
	
	//// �擾
	while (true) {

		int sqlrc = clsDB.Fetch();
		if( sqlrc == KIZUODBC_FETCH_OK ) {					// �f�[�^�L��
			
			clsDB.GetData(1, cKeyinf, sizeof(cKeyinf));			// �L�[���
			clsDB.GetData(2, cMeiNo, sizeof(cMeiNo));			// ����No
			clsDB.GetData(3, cLine_Code, sizeof(cLine_Code));	// �H���R�[�h
			clsDB.GetData(4, cCoilNo, sizeof(cCoilNo));			// �R�C��No

			// �O�H�����їv��
			SendMailFFReq(cKeyinf, cMeiNo, cLine_Code, cCoilNo);

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			return;

		} else {								// �G���[
			LOG(em_NON), "%s", sql );
			LOG(em_ERR), "[%s] �O�H���R�C�����擾�G���[ [%s]", my_sThreadName, cDbName );
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return;
		}
	}

	return;

}

//------------------------------------------
// �O�H�����їv��
// char* cKeyinf	�R�C�����肷�邽�߂̃L�[���
// char* cMeiNo		����No
// char* cLine_Code	�H���R�[�h
// char* cCoilNo	�R�C��No
//------------------------------------------
void FFSendManager::SendMailFFReq(char* cKeyinf, char* cMeiNo, char* cLine_Code, char* cCoilNo)
{
	char cMeiNoBuff[SIZE_MEI_NO + 1];
	COMMON_QUE		que;
	memset(&que, 0x00, sizeof(que));

	memset(&cMeiNoBuff, 0x00, sizeof(cMeiNoBuff));
	strcpy(cMeiNoBuff, cMeiNo);
	KizuFunction::Trim(cMeiNoBuff);

	que.nLineNo = getlineid();

	//// ��FF�T�[�o�ʐM�֒ʒm
	que.nEventNo = FACT_TO_FFSERV_02;							// �v�����[�h
	memcpy(&que.mix.cdata[0], cKeyinf, SIZE_COIL_KEY);			// �R�C�����肷�邽�߂̃L�[���
	//memcpy(&que.mix.cdata[1], cLine_Code, SIZE_KOUTEI_CODE);	// �H���R�[�h
	memcpy(&que.mix.cdata[1], KOUTEI_CODE, SIZE_KOUTEI_CODE);	// �H���R�[�h(�v���R������̃R�C�����ɂ͍H���R�[�h�������Ă��Ȃ�)
	sprintf(que.mix.cdata[2], "%04s", cMeiNoBuff);				// ����No
	memcpy(&que.mix.cdata[3], cCoilNo, SIZE_COIL_NO);			// �R�C��No


	send_mail(TO_FFSERV, ".", &que);

	LOG(em_MSG), "[%s] �O�H�����їv�����M [�L�[���=%16s]", my_sThreadName, cKeyinf );

}
