#include "StdAfx.h"
#include "LongPdCheckManager.h"

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
LongPdCheckManager::LongPdCheckManager(void) :
ThreadManager("LPdMgr"),
m_nParamCycleTime(-1)
{
	//// �n���h������
	m_evTimerExec  = CreateWaitableTimer(NULL, FALSE, NULL);

	//// ������
	memset(m_CheckInf, 0x00, sizeof(m_CheckInf));
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
LongPdCheckManager::~LongPdCheckManager(void)
{
	//// �n���h�����
	CloseHandle(m_evTimerExec);

	//// �J��
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int LongPdCheckManager::ThreadFirst()
{
	//// ���O����
	m_nParamCycleTime = mcls_pParam->GetCommonMast().nLongPd_GuidanceCheckInterval * 1000;

	__int64 ts = m_nParamCycleTime * -10000; 
	SetWaitableTimer(m_evTimerExec, (LARGE_INTEGER *)&ts, m_nParamCycleTime, NULL, NULL, FALSE);

	//// �C�x���g�ݒ�
	HANDLE hArray[] = { m_evTimerExec
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int LongPdCheckManager::ThreadLast()
{
	// �������~
	CancelWaitableTimer(m_evTimerExec);
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void LongPdCheckManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_TIMER_EXEC = 0,				// �������s ������^�C�}
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_TIMER_EXEC: 					// �������s ������^�C�}
		{
			// �p�����[�^�ēǂݍ���
			bool bRead = mcls_pParam->ReadParam_All();
			if ( true == bRead )
			{
				// ���������׃`�F�b�N
				CheckLongPd();

				// �^�C�}�[�̎��Ԃ��ύX����Ă����甽�f
				int nTime = mcls_pParam->GetCommonMast().nLongPd_GuidanceCheckInterval * 1000;
				if( m_nParamCycleTime != nTime ) 
				{
					LOG(em_MSG), "[%s] ���������׃`�F�b�N �Ď������ύX [%d]->[%d]sec", my_sThreadName, m_nParamCycleTime / 1000, mcls_pParam->GetCommonMast().nLongPd_GuidanceCheckInterval );

					m_nParamCycleTime = nTime;

					// �ύX���ꂽ�l�Ń^�C�}�[�N��
					CancelWaitableTimer(m_evTimerExec);
					__int64 ts = m_nParamCycleTime * -10000; 
					SetWaitableTimer(m_evTimerExec, (LARGE_INTEGER *)&ts, m_nParamCycleTime, NULL, NULL, FALSE);
				}
			}
		}
		
		break;
	
//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		break;

	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �o��

//------------------------------------------
// ���������׃`�F�b�N�@�������
//------------------------------------------
void LongPdCheckManager::CheckLongPd()
{
	const ParamManager::P_COMMON_MAST& prm = mcls_pParam->GetCommonMast();
	CString csSQL = "";
	CString csTemp = "";
	CString csLog = "";
	char cSQLBuf[256];

	LOG(em_MSG), "[%s] ----- ���������ׁ@������`�F�b�N ---------------------------", my_sThreadName);

	//---------------------------------
	// �K�C�_���X�@�\�������Ȃ�`�F�b�N����
	//---------------------------------
	if( 0 == prm.nLongPd_GuidanceAvailable ) 		
	{
		LOG(em_MSG), "[%s] �K�C�_���X�@�\ �������̂��߃`�F�b�N����", my_sThreadName);
		return;
	}

	// ������
	CEHCK_INF	wkInf[CHECK_COIL_NUM];
	memset(wkInf, 0x00, sizeof(wkInf));

	// DB
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return;
	}

	//---------------------------------
	// �ΏۃR�C�����擾
	//---------------------------------
	csSQL = " SELECT ";
	csTemp.Format(" TOP %d ", CHECK_COIL_NUM);
	csSQL += csTemp;
	csSQL += "  �Ǘ�No ";
	csSQL += " ,ISNULL(���YNo, '') As �R�C��No ";
	csTemp.Format(" FROM %s ", DB_COIL_INF);
	csSQL += csTemp;
	csTemp.Format(" ORDER BY �o�^���t DESC ");
	csSQL += csTemp;

	if (!clsDB.ExecuteDirect(csSQL)) {
		LOG(em_ERR), "[%s] %s<%s>", my_sThreadName, clsDB.GetLastErrMsg(), csSQL);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", DB_COIL_INF);
		return;
	}

	//// �擾
	int nGetCount = 0;
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��
			// �Ǘ�No
			memset(cSQLBuf, 0x00, sizeof(cSQLBuf));
			clsDB.GetData(1, &cSQLBuf, sizeof(cSQLBuf) );
			memcpy(wkInf[nGetCount].cManagementNo, cSQLBuf, sizeof(wkInf[nGetCount].cManagementNo));

			// �R�C��No
			memset(cSQLBuf, 0x00, sizeof(cSQLBuf));
			clsDB.GetData(2, &cSQLBuf, sizeof(cSQLBuf) );
			memcpy(wkInf[nGetCount].cCoilNoIn, cSQLBuf, sizeof(wkInf[nGetCount].cCoilNoIn));
		
			csTemp.Format(" �Ώ�%d:[%.*s]", nGetCount + 1, sizeof(wkInf[nGetCount].cManagementNo), wkInf[nGetCount].cManagementNo);
			csLog += csTemp;

			// �J�E���g�A�b�v
			nGetCount ++;
		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", DB_COIL_INF, clsDB.GetLastErrMsg());
			return;
		}
	}

	if ( 0 < nGetCount )
	{
		LOG(em_MSG), "[%s] �`�F�b�N�Ώ� %s", my_sThreadName, csLog);
	}

	// �ΏۃR�C���ɒ��������ׂ����݂��邩�`�F�b�N
	// ���������ׂ͘F�`�J�i�萔�}�X�^�̐ݒ�͈͓��j�̂��̂�ΏۂƂ���B
	for ( int nCoilIndex = 0; nCoilIndex < nGetCount; nCoilIndex++ ) 		
	{
		// ���������ׂ̑��݃`�F�b�N
		int nLongPdCnt[NUM_MEN];
		memset(nLongPdCnt, 0x00, sizeof(nLongPdCnt));

		for( int nMen = 0; nMen < NUM_MEN; nMen++ ) 			
		{
			// SQL�쐬
			CString csTable = (0 == nMen ? DB_LONGPD_EXEC_RESULT_DEFECT_T : DB_LONGPD_EXEC_RESULT_DEFECT_B);

			csSQL = " SELECT ";
			csSQL += " COUNT(DefectNo) ";
			csSQL += " FROM ";
			csTemp.Format(" %s ", csTable);
			csSQL += csTemp;
			csSQL += " WHERE ";
			csTemp.Format(" ManagementNo = '%.20s' ", wkInf[nCoilIndex].cManagementNo);
			csSQL += csTemp;
			csSQL += " AND ( ";
			for( int ii = 1; ii <= MAX_LONGPD_FILTER; ii++ ) 				
			{
				if( ii > 1 ) 					
				{
					csSQL += " OR ";
				}
				csTemp.Format(" Cycle_Filter%d BETWEEN %d AND %d ", ii, prm.nLongPd_NearFurnaceRangeFrom, prm.nLongPd_NearFurnaceRangeTo);
				csSQL += csTemp;
			}
			csSQL += " ) ";

			// ���s���Ď擾
			if (!clsDB.ExecuteDirect(csSQL)) {
				LOG(em_ERR), "[%s] %s<%s>", my_sThreadName, clsDB.GetLastErrMsg(), csSQL);
				syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", csTable);
				return;
			}

			//// �擾
			while (TRUE) {
				int sqlrc = clsDB.Fetch();
				if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��
					clsDB.GetData(1, &wkInf[nCoilIndex].nCount[nMen] );
					if( 0 < wkInf[nCoilIndex].nCount[nMen] ) 						
					{
						// ���������ׂ���
						wkInf[nCoilIndex].bLongPdExist = TRUE;
					}
				} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
					break;	
				} else {								// �G���[
					syslog(SYSNO_DB_FETCH_ERR, "[%s][%s]", csTable, clsDB.GetLastErrMsg());
					return;
				}
			}
		}

		// �O��̌��ʂ��L��Ύ擾
		BOOL bPrevCheck = false;
		for( int ii = 0; ii < CHECK_COIL_NUM; ii++ )
		{
			if( 0 == memcmp(wkInf[nCoilIndex].cManagementNo, m_CheckInf[ii].cManagementNo, SIZE_KIZUKEN_NO) ) 					
			{
				bPrevCheck = m_CheckInf[ii].bLongPdExist;
			}
		}

		// �O��`�F�b�N���Ɍ��o����Ă��炸�A���񌟏o���ꂽ�Ȃ�\���Ǘ��֒ʒm����B
		if( 0 < wkInf[nCoilIndex].nCount[0] || 0 < wkInf[nCoilIndex].nCount[1] )
		{
			if( TRUE == wkInf[nCoilIndex].bLongPdExist && FALSE == bPrevCheck ) 						
			{
				LOG(em_MSG), "[%s] ���������ה��� �Ǘ�No[%.20s] �\[%d] ��[%d]", my_sThreadName, wkInf[nCoilIndex].cManagementNo, wkInf[nCoilIndex].nCount[0], wkInf[nCoilIndex].nCount[1] );

				// �\���Ǘ��֒ʒm
				COMMON_QUE que;
				memset(&que, 0x00, sizeof(que));

				que.nEventNo = FACT_HY_DEFECT_05;
				que.nLineNo = getlineid();
				memcpy(que.mix.cdata[0], wkInf[nCoilIndex].cManagementNo, SIZE_KIZUKEN_NO);

				// ���M
				int iRet = send_mail(HY_DEFECT, "*", &que);
				if( 0 != iRet ) 
				{
					syslog(SYSNO_MAIL_SNED_ERR, "[�\���Ǘ��֒��������ה����ʒm err_code=%d]", iRet);
				}
				else 							
				{
					LOG(em_MSG), "[%s] ���������ה����ʒm(FACT_HY_DEFECT_05) ���M �Ǘ�No[%.20s]", my_sThreadName, wkInf[nCoilIndex].cManagementNo);
				}
			}
		}
	}

	// �o�b�t�@�ɃR�s�[
	memcpy(m_CheckInf, wkInf, sizeof(m_CheckInf));
}
