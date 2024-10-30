#include "StdAfx.h"
#include "PreDefectManager.h"

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
PreDefectManager::PreDefectManager():
ThreadManager("PreDefectManager")
{
	// �V�O�i������
	m_evGetData = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evTimeCycle = CreateWaitableTimer(NULL, FALSE, NULL);

}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
PreDefectManager::~PreDefectManager(void)
{
	// �C�x���g�J��
	CloseHandle( m_evGetData );
	CloseHandle( m_evTimeCycle );
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int PreDefectManager::ThreadFirst()
{
	////////////////////////////////////
	// ���O����
	////////////////////////////////////
	//// PIO��Ԋm�F ������^�C�}�[�Z�b�g
	__int64 ts = (__int64)PREPROC_TIMER * -10000;	// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)

	SetWaitableTimer( m_evTimeCycle,   (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE );

	//// �C�x���g�ݒ�
	HANDLE hArray[] = { m_evGetData,
						m_evTimeCycle
						};

	SetEventNum( sizeof(hArray)/sizeof(HANDLE), hArray );

	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void PreDefectManager::ThreadEvent( int nEventNo )
{
	//// PIO��Ԋm�F ������^�C�}�[�Z�b�g
	__int64 ts = (__int64)PREPROC_TIMER * -10000;	// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)

	////// �V�O�i���̏���
	enum {	EV_GETDATA = 0,					// �O�H�����я��擾
			EV_TIMECYCLE,					// ��������s
		};

	////// �V�O�i����������
	switch ( nEventNo ) {
//-----------------------------------------------------------
		case EV_GETDATA:
			LOG(em_MSG), "[%s] �O�H�����ю擾�v�� [0x%X]", my_sThreadName, GetTid() );
			PreDefectMain();

			// ���̃^�C�}�[�𔭍s
			CancelWaitableTimer( m_evTimeCycle );
			SetWaitableTimer( m_evTimeCycle,   (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE );
			break;

//-----------------------------------------------------------
		case EV_TIMECYCLE:											// �����
			LOG(em_MSG), "[%s] ��������� [0x%X]", my_sThreadName, GetTid() );
			PreDefectMain();

			// ���̃^�C�}�[�𔭍s
			CancelWaitableTimer( m_evTimeCycle );
			SetWaitableTimer( m_evTimeCycle,   (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE );
			break;

//-----------------------------------------------------------
		default :
			// ���肦�Ȃ��I�I
			LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
			break;

//-----------------------------------------------------------
	}
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int PreDefectManager::ThreadLast()
{
	return 0;
}

//------------------------------------------
// �O�H�����ѕҏW���C������
//------------------------------------------
void PreDefectManager::PreDefectMain()
{
	
	CString sql;					// SQL��
	CString wk;
	CTime t = CTime::GetCurrentTime() - CTimeSpan( 1, 0, 0, 0 );			// 1���O
	CString strTime = t.Format("'%Y/%m/%d %H:%M:%S'");
	CString cDbName;
	CString cDbName2;
	cDbName = DB_MAE_COIL_INF;
	cDbName2 = DB_MAE_COIL_LINE;

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect( TASKINI_NAME ) ) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName );
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return;
	}

	//// SQL���쐬(�Ǘ�No�擾)
	sql =  "SELECT";
	sql += " INF.�Ǘ�No, LINE.�O�H���Ǘ�No, LINE.�r���o�H��, LINE.�\���敪 , INF.����No, INF.�R�C��No ";	// 2014.05.09 ���� �L���ނ̕\��
	sql += " FROM ";
	sql += cDbName;
	sql += " AS INF, ";
	sql += cDbName2;
	sql += " AS LINE ";
	sql += " WHERE ";
	sql += " INF.�Ǘ�No = LINE.�Ǘ�No ";
	sql += " AND INF.FF�O�H���r����M�t���O = 0 ";
	sql += " AND INF.���Ǘ�No�t���O = -1 ";
	sql += " AND LINE.�����X�e�[�^�X = -1 ";
	wk.Format( " AND �o�^���t > %s", strTime );
	sql += wk;
	sql += " ORDER BY �o�^���t ASC ";

	//// ���s
	if( ! clsDB.ExecuteDirect( sql ) ) {
		LOG(em_NON), "%s", sql );
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", cDbName);
		return;
	}
	
	//// �擾
	while (true) {

		int sqlrc = clsDB.Fetch();
		if( sqlrc == KIZUODBC_FETCH_OK ) {					// �f�[�^�L��
			
			PreDBManager::DeliveryPreDB*  pDeli = new PreDBManager::DeliveryPreDB;

			clsDB.GetData( 1, pDeli->cKizuKenNo, sizeof(pDeli->cKizuKenNo) );				// �Ǘ�No
			clsDB.GetData( 2, pDeli->cPreKizuKenNo, sizeof(pDeli->cPreKizuKenNo) );			// �O�H���Ǘ�No
			clsDB.GetData( 3, pDeli->cLineCode, sizeof(pDeli->cLineCode));					// �H���R�[�h(�r���o�H��)	2014.05.09 ���� �L���ނ̕\��
			clsDB.GetData( 4, &pDeli->nOuDiv );												// �\���敪
			clsDB.GetData( 5, &pDeli->cMeiNo, sizeof(pDeli->cMeiNo) );						// ����No
			clsDB.GetData( 6, &pDeli->cCoilNo, sizeof(pDeli->cCoilNo) );					// �R�C��No

			// DB�o�^�����֑��M
			mcls_pPreDB->gque_AddSQLData.AddToTail( pDeli );

			LOG(em_MSG), "[%s]�@DB�o�^�����v�� [�Ǘ�No=%s][�O�H���Ǘ�No=%s][�\���敪=%d]",
				my_sThreadName, pDeli->cKizuKenNo, pDeli->cPreKizuKenNo, pDeli->nOuDiv );
	
		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			return;

		} else {								// �G���[
			LOG(em_NON), "%s", sql );
			LOG(em_ERR), "[%s] �O�H���R�C�����擾�G���[ [%s][%s]", my_sThreadName, cDbName, cDbName2 );
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", cDbName);
			return;
		}
	}

	return;

}
