#include "StdAfx.h"
#include "LinkswManager.h"

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
LinkswManager::LinkswManager(void) : 
ThreadManager("LinkswMgr"),
mcls_p232c(NULL),
m_bIsConnect(false),
m_nEventWaitNum(0),
m_emMainMode(MODE_NONE),
m_nSubMode(0),
m_nCamMode(0),
m_nApxMode(0),
m_nAocMode(0),
m_nAocSubMode(0),
m_nRegSizeMode(0),
m_nSwMode(0),
m_bCCMode(FALSE),
m_nSlaveJoinTorB(0),
m_nSlaveJoinFNo(0),
m_nResetFNo(0),
m_bInitialSpeedChange(false),
m_bEnableRapidSerial(false)
{
	// ����������΂���
	m_nUnit = 0;
	my_sThreadName = TO_LINKSW;
	m_cMySession = TO_LINKSW;

	char	cWk[1024];
	CString	sWk;
	CString	sWk2;
	int		nWk;

	// ������
	memset( m_cRecvBuf, 0x00, sizeof(m_cRecvBuf) );
	memset( m_cSendBuf, 0x00, sizeof(m_cSendBuf) );

	// �C�x���g����
	m_evRecvEnd			= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evSendEnd			= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evReTry			= CreateWaitableTimer(NULL, FALSE, NULL);
	m_evAnswerTimeOut	= CreateWaitableTimer(NULL, FALSE, NULL);
	m_evTimerCycleCheck	= CreateWaitableTimer(NULL, FALSE, NULL);
	m_evCounterCheck	= CreateEvent(NULL, FALSE, FALSE, NULL);

	//// INI�t�@�C���ݒ�擾
	// ������`�F�b�N�^�C�}����
	m_nCycleTime = GetPrivateProfileInt(m_cMySession, "TIMER_CYCLE", 0, TASKINI_NAME);
	m_nCycleTime *= 1000;
	// �@���Ԋ֘A
	for( int ii=0; ii<NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; ii++ )
	{
		sWk.Format("%s_%d", m_cMySession, ii);
		// FPGA�_�C���x
		m_nDieTempLimit[ii]	= GetPrivateProfileInt(sWk, "FPGADIE_TEMP_LIMIT",	0, TASKINI_NAME);
		m_nDieTempID[ii]	= GetPrivateProfileInt(sWk, "FPGADIE_TEMP_ID",		0, TASKINI_NAME);
		m_nDieTempStID[ii]	= GetPrivateProfileInt(sWk, "FPGADIE_TEMP_ST_ID",	0, TASKINI_NAME);
		m_nDieTemp[ii]		= 0;
		m_bDieTempSt[ii]	= true;
		// ���͉��x
		m_nAmbTempLimit[ii]	= GetPrivateProfileInt(sWk, "AMBIENT_TEMP_LIMIT",	0, TASKINI_NAME);
		m_nAmbTempID[ii]	= GetPrivateProfileInt(sWk, "AMBIENT_TEMP_ID",		0, TASKINI_NAME);
		m_nAmbTempStID[ii]	= GetPrivateProfileInt(sWk, "AMBIENT_TEMP_ST_ID",	0, TASKINI_NAME);
		m_nAmbTemp[ii]		= 0;
		m_bAmbTempSt[ii]	= true;
		// Opt-C:Link�X�e�[�^�XID
		for( int jj=0; jj<MAX_CAMLINKSW_INPUT; jj++ )
		{
			// B
			sWk2.Format("OPTCLINK%d_B_ST_ID", jj+1);
			m_nOptCLinkStID[ii][jj][0] = GetPrivateProfileInt(sWk, sWk2, 0, TASKINI_NAME);
			m_bOptCLinkSt[ii][jj][0]	= true;
			// M/F
			sWk2.Format("OPTCLINK%d_MF_ST_ID", jj+1);
			m_nOptCLinkStID[ii][jj][1] = GetPrivateProfileInt(sWk, sWk2, 0, TASKINI_NAME);
			m_bOptCLinkSt[ii][jj][1]	= true;
		}
	}

	// ini�t�@�C������n�[�t�ŃJ���������NSW�̃t���O���擾
	memset(m_bHalfFlg, 0x00, sizeof(m_bHalfFlg));
	for (int jj = 0; jj<NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; jj++)
	{
		sWk.Format("HALF_FLG_%d", (int)jj / MAX_CAMLINKSW_FPGA + 1);
		nWk = GetPrivateProfileInt(m_cMySession, sWk, 0, TASKINI_NAME);
		if (0 == nWk)	m_bHalfFlg[jj] = false;
		else			m_bHalfFlg[jj] = true;
	}

	// ini�t�@�C���w��̃J��������CH, �\���敪���擾
	memset( m_nSelectCam, 0x00, sizeof(m_nSelectCam) );
	memset( m_nSelectTorB, 0x00, sizeof(m_nSelectTorB) );
	for( int ii=0; ii<NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; ii++ )
	{
		for( int jj=0; jj<MAX_CAMLINKSW_OUTPUT; jj++ )
		{
			sWk.Format("FPGA_%d_BOARD_%d", ii, jj+1);
			GetPrivateProfileString(m_cMySession, sWk, "", cWk, sizeof(cWk), TASKINI_NAME);

			if( 0 < strlen(cWk) )
			{
				sWk = cWk;

				// �g�[�N������
				int		rec = 0;
				int		curPos = 0;
				CString	strToken = sWk.Tokenize(",", curPos);
				while( strToken != "" )
				{
					strToken.Trim(_T(" 	"));			// ini�t�@�C���ݒ蕶���̑O��̃X�y�[�X�A�^�u����菜��

					if( 0 == rec )						// �J��������CH
					{
						// 2017.02.27 Master/Slave��DB�w�� ---------->>>>
						//nWk = atoi(strToken);
				 		//if( 0 >= nWk || MAX_CAMLINKSW_ALLINPUT < nWk ) break;
						//m_nSelectCam[ii][jj] = nWk;
						
						nWk = atoi(strToken);
			 			if( (-1 * MAX_CAMLINKSW_ALLINPUT) > nWk || MAX_CAMLINKSW_ALLINPUT < nWk ) break;

						if (0 != nWk) {
							if (m_bHalfFlg[ii] && !HARF_CC_SOCK_EXIST[jj]) {
								LOG(em_ERR), "[%s] <%d> �n�[�t�łɑ��݂��Ȃ��{�[�h��ini�ɐݒ肳��Ă��邽�ߋ����I�ɖ��I���ɕύX(board=%d, cam=%d)", my_sThreadName, ii, jj, nWk);
								break;
							}
						}
						m_nSelectCam[ii][jj] = nWk;
						
						// 2017.02.27 Master/Slave��DB�w�� <<<<----------


					}
					else if( 1 == rec )					// �\���敪
					{
						nWk = atoi(strToken);
						m_nSelectTorB[ii][jj] = nWk;
					}

					// ����
					strToken = sWk.Tokenize(",", curPos);
					rec++;
				}
			}
		}
	}

	// �����ʐM�ؑ֎��̃{�[���[�g
	m_bEnableRapidSerial = ( 1 == GetPrivateProfileInt(m_cMySession, "ENABLE_SPEED_CHANGE", 0, TASKINI_NAME) ? true : false );

	// �r���Q�����W�X�^�o�b�t�@ �N���A
	memset(m_nCounterNow, 0x00, sizeof(m_nCounterNow));
	memset(m_nCounterSet, 0x00, sizeof(m_nCounterSet));

	// Opt-C:Link�X�e�[�^�X�o�b�t�@ �N���A
	memset(m_nOptCLinkSt,		0x00, sizeof(m_nOptCLinkSt));
	memset(m_bOptCLink_RxSt,	0x01, sizeof(m_bOptCLink_RxSt));
	memset(m_bOptCLink_LinkSt,	0x01, sizeof(m_bOptCLink_LinkSt));
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
LinkswManager::~LinkswManager(void)
{
	// �C�x���g���
	CloseHandle(m_evRecvEnd);
	CloseHandle(m_evSendEnd);
	CloseHandle(m_evReTry);
	CloseHandle(m_evAnswerTimeOut);
	CloseHandle(m_evTimerCycleCheck);
	CloseHandle(m_evCounterCheck);

	// 232C �N���[�Y
	if( NULL != mcls_p232c )
	{
		SAFE_DELETE(mcls_p232c);
	}
}

//------------------------------------------
// ���N���X�Z�b�g
// int nUnit			���j�b�g�ԍ� (0�I���W��)
// char const* sSession	Ini�Z�N�V������
//------------------------------------------
void LinkswManager::SetClass(int nUnit, char* sSession)
{
	m_nUnit = nUnit;
	m_cMySession = sSession;
	my_sThreadName.Format("LinkSwMgr_%d", m_nUnit); 


	// ���̑�����H�����ݒ荀�ځH�Ǎ�
	char	cWk[1024];
	CString	sWk;
	CString	sWk2;
	int		nWk;
	//GetPrivateProfileString(m_cMySession, "PARAM_CNT_TBL_NAME", m_cPrmCommonTblName, cWk, sizeof(cWk), TASKINI_NAME);	m_cPrmCommonTblName	= cWk;
	//m_nMyCamNum		= GetPrivateProfileInt(m_cMySession, "CAMID_NUM", m_nMyCamNum, TASKINI_NAME);
	//m_nMyCamIdStart = GetPrivateProfileInt(m_cMySession, "CAMID_START", m_nMyCamIdStart, TASKINI_NAME); 
	//m_nMyCamIdEnd	= m_nMyCamIdStart+m_nMyCamNum;


	
	//// INI�t�@�C���ݒ�擾
	// ������`�F�b�N�^�C�}����
	m_nCycleTime = GetPrivateProfileInt(m_cMySession, "TIMER_CYCLE", 0, TASKINI_NAME);
	m_nCycleTime *= 1000;
	// �@���Ԋ֘A
	for( int ii=0; ii<NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; ii++ )
	{
		sWk.Format("%s_%d", m_cMySession, ii);
		// FPGA�_�C���x
		m_nDieTempLimit[ii]	= GetPrivateProfileInt(sWk, "FPGADIE_TEMP_LIMIT",	0, TASKINI_NAME);
		m_nDieTempID[ii]	= GetPrivateProfileInt(sWk, "FPGADIE_TEMP_ID",		0, TASKINI_NAME);
		m_nDieTempStID[ii]	= GetPrivateProfileInt(sWk, "FPGADIE_TEMP_ST_ID",	0, TASKINI_NAME);
		m_nDieTemp[ii]		= 0;
		m_bDieTempSt[ii]	= true;
		// ���͉��x
		m_nAmbTempLimit[ii]	= GetPrivateProfileInt(sWk, "AMBIENT_TEMP_LIMIT",	0, TASKINI_NAME);
		m_nAmbTempID[ii]	= GetPrivateProfileInt(sWk, "AMBIENT_TEMP_ID",		0, TASKINI_NAME);
		m_nAmbTempStID[ii]	= GetPrivateProfileInt(sWk, "AMBIENT_TEMP_ST_ID",	0, TASKINI_NAME);
		m_nAmbTemp[ii]		= 0;
		m_bAmbTempSt[ii]	= true;
		// Opt-C:Link�X�e�[�^�XID
		for( int jj=0; jj<MAX_CAMLINKSW_INPUT; jj++ )
		{
			// B
			sWk2.Format("OPTCLINK%d_B_ST_ID", jj+1);
			m_nOptCLinkStID[ii][jj][0] = GetPrivateProfileInt(sWk, sWk2, 0, TASKINI_NAME);
			m_bOptCLinkSt[ii][jj][0]	= true;
			// M/F
			sWk2.Format("OPTCLINK%d_MF_ST_ID", jj+1);
			m_nOptCLinkStID[ii][jj][1] = GetPrivateProfileInt(sWk, sWk2, 0, TASKINI_NAME);
			m_bOptCLinkSt[ii][jj][1]	= true;
		}
	}

	// ini�t�@�C������n�[�t�ŃJ���������NSW�̃t���O���擾
	memset( m_bHalfFlg, 0x00, sizeof(m_bHalfFlg));
	for (int jj = 0; jj<NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; jj++)
	{
		sWk.Format("HALF_FLG_%d", (int)jj / MAX_CAMLINKSW_FPGA + 1);
		nWk = GetPrivateProfileInt(m_cMySession, sWk, 0, TASKINI_NAME);
		if (0 == nWk)	m_bHalfFlg[jj] = false;
		else			m_bHalfFlg[jj] = true;
	}

	// ini�t�@�C���w��̃J��������CH, �\���敪���擾
	memset( m_nSelectCam, 0x00, sizeof(m_nSelectCam) );
	memset( m_nSelectTorB, 0x00, sizeof(m_nSelectTorB) );
	for( int ii=0; ii<NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; ii++ )
	{
		for( int jj=0; jj<MAX_CAMLINKSW_OUTPUT; jj++ )
		{
			sWk.Format("FPGA_%d_BOARD_%d", ii, jj+1);
			GetPrivateProfileString(m_cMySession, sWk, "", cWk, sizeof(cWk), TASKINI_NAME);

			if( 0 < strlen(cWk) )
			{
				sWk = cWk;

				// �g�[�N������
				int		rec = 0;
				int		curPos = 0;
				CString	strToken = sWk.Tokenize(",", curPos);
				while( strToken != "" )
				{
					strToken.Trim(_T(" 	"));			// ini�t�@�C���ݒ蕶���̑O��̃X�y�[�X�A�^�u����菜��

					if( 0 == rec )						// �J��������CH
					{						
						nWk = atoi(strToken);
			 			if( (-1 * MAX_CAMLINKSW_ALLINPUT) > nWk || MAX_CAMLINKSW_ALLINPUT < nWk ) break;
						if (0 != nWk) {
							if (m_bHalfFlg[ii] && !HARF_CC_SOCK_EXIST[jj]) {
								LOG(em_ERR), "[%s] <%d> �n�[�t�łɑ��݂��Ȃ��{�[�h��ini�ɐݒ肳��Ă��邽�ߋ����I�ɖ��I���ɕύX(board=%d, cam=%d)", my_sThreadName, ii, jj, nWk);
								break;
							}
						}
						m_nSelectCam[ii][jj] = nWk;
					}
					else if( 1 == rec )					// �\���敪
					{
						nWk = atoi(strToken);
						m_nSelectTorB[ii][jj] = nWk;
					}

					// ����
					strToken = sWk.Tokenize(",", curPos);
					rec++;
				}
			}
		}
	}

	// �����ʐM�ؑ֎��̃{�[���[�g
	m_bEnableRapidSerial = ( 1 == GetPrivateProfileInt(m_cMySession, "ENABLE_SPEED_CHANGE", 0, TASKINI_NAME) ? true : false );

}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �X���b�h�����`


//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int LinkswManager::ThreadFirst()
{
	// �p�����[�^�Ǎ���
	while( TRUE )
	{
		if( GetParamStatic() &&
			GetParam() ) break;
		LOG(em_MSG), "[%s] �p�����[�^�Ǎ����s(DB�ڑ��ُ�H�I)�B���g���C���E�E�E", my_sThreadName);
		Sleep(3000);
	}
	// ini�t�@�C���w��̃J��������CH��ݒ�

	// 2017.02.27 Master/Slave��DB�w�� --------------------------------------------->>>>
	//for( int ii=0; ii<NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; ii++ )
	//{
	//	for( int jj=0; jj<MAX_CAMLINKSW_OUTPUT; jj++ )
	//	{
	//		if( 0 >= m_nSelectCam[ii][jj] || MAX_CAMLINKSW_ALLINPUT < m_nSelectCam[ii][jj] ) continue;
	//		m_Param.linksw[ ii ].nSelectCam[ jj ] = m_nSelectCam[ii][jj];
	//		LOG(em_INF), "[%s] �J�����I��ݒ�(INI) �擾 [ID=%d, �{�[�h%d=%d]", my_sThreadName, ii, jj+1, m_Param.linksw[ ii ].nSelectCam[ jj ]);
	//	}
	//}

	// ��鎖���ώG�ɂȂ����̂ŕʊ֐���
	GetParamIni();

	// 2017.02.27 Master/Slave��DB�w�� <<<<---------------------------------------------

	// �Z�o�p�����[�^�擾
	CalcParam();

	// 232C �����ڑ�
	if( ! Open(true) )
	{
		Close();
	}

	//// �C�x���g�ݒ�
	HANDLE hArray[] =
	{
		m_evReTry,
		m_evSendEnd,
		m_evRecvEnd,
		m_evAnswerTimeOut,
		gque_Deli.g_evSem,
		m_evTimerCycleCheck,
		m_evCounterCheck
	};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	m_nEventWaitNum = sizeof(hArray)/sizeof(HANDLE);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int LinkswManager::ThreadLast()
{
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void LinkswManager::ThreadEvent(int nEventNo)
{
	COMMON_QUE * pQue;

	//// �V�O�i���̏���
	enum
	{
		EV_RETRY,							// �Đڑ��J�n
		EV_SENDEND,							// ���M����
		EV_RECVEND,							// ��M����
		EV_ANSWERTIMEOUT,					// ���M�A���T�[�^�C���A�E�g
		EV_QUE,								// ���[���X���b�g��M
		EV_TIMERCYCLECHECK,					// ������`�F�b�N�^�C�}
		EV_COUNTERCHECK,					// �J�E���^�`�F�b�N
		EV_END
	};

	//// �V�O�i����������
	switch( nEventNo )
	{
//-----------------------------------------------------------------------------------------------
	case EV_RETRY:							// �Đڑ��J�n
		if( ! Open(false) )
		{
			Close();
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_SENDEND:						// ���M����
		// �R�}���h�𑗐M�����̂ŁA��M�J�n
		if( ! Recv() )
		{
			Close();
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_RECVEND:						// ��M����
		// ��M����
		CancelWaitableTimer(m_evAnswerTimeOut);

		// ��M�f�[�^�`�F�b�N
		if( ! CheckDataRecv() )
		{
			Close();
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_ANSWERTIMEOUT:					// �����`���^�C���A�E�g
		if( m_bIsConnect )
		{
			LOG(em_ERR), "[%s] �����`���^�C���A�E�g send=%s", my_sThreadName, m_cSendBuf);
			syslog(211, "[�����`���^�C���A�E�g:send=%s]", m_cSendBuf);
		}
		Close();		// �Đڑ�
		break;

//-----------------------------------------------------------------------------------------------
	case EV_TIMERCYCLECHECK:				// ������`�F�b�N�^�C�}
		// �L���[����
		pQue = new COMMON_QUE;
		pQue->nEventNo = FACT_CYCLE_CHECK;
		pQue->nLineNo = getlineid();

		// �R�}���h����
		if( ! SetDataMailCommand(pQue) )
		{
			Close();
		}
		SAFE_DELETE(pQue);
		break;

//-----------------------------------------------------------------------------------------------
	case EV_COUNTERCHECK:					// �J�E���^�`�F�b�N
		// �L���[����
		pQue = new COMMON_QUE;
		pQue->nEventNo = FACT_COUNTER_CHECK;
		pQue->nLineNo = getlineid();

		// �R�}���h����
		if( ! SetDataMailCommand(pQue) )
		{
			Close();
		}
		SAFE_DELETE(pQue);
		break;

//-----------------------------------------------------------------------------------------------
	case EV_QUE:							// ���[���X���b�g��M
		// �L���[���o��
		pQue = gque_Deli.GetFromHead();

		// �R�}���h����
		if( ! SetDataMailCommand(pQue) )
		{
			Close();
		}
		SAFE_DELETE(pQue);
		break;

//-----------------------------------------------------------------------------------------------
	default:
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo);
		return;
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �V�[�P���X����


//------------------------------------------
// �R�}���h����
// COMMON_QUE * pQue ���[���X���b�g�f�[�^
// �߂�l ���A���
//------------------------------------------
bool LinkswManager::SetDataMailCommand(COMMON_QUE * pQue)
{
	// �J���������N�X�C�b�`�ڑ����H
	if( ! m_bIsConnect )
	{
		LOG(em_WAR), "[%s] ����ؒf��ԂŃR�}���h��M", my_sThreadName);
		return true;
	}

	// 20190205 �v���ɑ΂��鉞�����ʗp --->>>
	memcpy(&m_RecvQue, pQue, sizeof(COMMON_QUE));
	// 20190205 �v���ɑ΂��鉞�����ʗp ---<<<

	// �C�x���gNo�ɂ�菈������
	switch( pQue->nEventNo )
	{
//---------------------------------------------------------
	case FACT_TO_LINKSW_01:									// �p�����[�^�ύX�v��
		// ���I�p�����[�^�Ǎ���
		GetParam();

		// �����_�ł͓��I�p�����[�^�͖����Ȃ̂ŁA���̂܂܏I��
		return true;

		break;

//---------------------------------------------------------
	case FACT_TO_LINKSW_02:									// �����p�ݒ�R�}���h
		// �p�����[�^�Ǎ���
		GetParamStatic();
		GetParam();
		// ini�t�@�C���w��̃J��������CH��ݒ�
		// 2017.02.27 Master/Slave��DB�w�� ---------->>>>
		//for( int ii=0; ii<NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; ii++ )
		//{
		//	for( int jj=0; jj<MAX_CAMLINKSW_OUTPUT; jj++ )
		//	{
		//		if( 0 >= m_nSelectCam[ii][jj] || MAX_CAMLINKSW_ALLINPUT < m_nSelectCam[ii][jj] ) continue;
		//		m_Param.linksw[ ii ].nSelectCam[ jj ] = m_nSelectCam[ii][jj];
		//	}
		//}

		// Ini�p�����[�^�𔽉f(��鎖���ώG�������̂ŕʊ֐�)
		GetParamIni();
		// 2017.02.27 Master/Slave��DB�w�� <<<<----------


		// �Z�o�p�����[�^�擾
		CalcParam();
		// �J�E���^���Z�b�g�t���[��No ���Z�b�g
		m_nResetFNo = 0;
		// ���M����
		m_emMainMode = MODE_START_INIT;
		m_nSubMode = 0;
		m_nCamMode = 0;
		m_nApxMode = 0;
		m_nAocMode = 0;
		m_nAocSubMode = 0;
		m_nRegSizeMode = 0;
		m_nSwMode = 0;
		break;

//---------------------------------------------------------
	case FACT_TO_LINKSW_03:									// �J�����I��ݒ�R�}���h
		// �n�[�t�ŃJ���������NSW�̏ꍇ�͑I���{�[�h�̃`�F�b�N�����{
		if (m_bHalfFlg[pQue->fl.data[0]]) {
			if (! HARF_CC_SOCK_EXIST[pQue->fl.data[1] - 1]) {
				LOG(em_ERR), "[%s] <%d> �n�[�t�łɑ��݂��Ȃ��{�[�h���I�����ꂽ���ߖ���(board=%d, cam=%d)", my_sThreadName, pQue->fl.data[0], pQue->fl.data[1], pQue->fl.data[2]);
				// �����Ɉُ��ʒm
				// 20190205 �v���ɑ΂��鉞�����ʗp --->>>
				//Send_ToDefect_StartInit(false);
				Send_ToDefect_SelectInit(false);
				// 20190205 �v���ɑ΂��鉞�����ʗp ---<<<
				return true;
			}
		}

		// �p�����[�^�ݒ�
		m_Param.linksw[ pQue->fl.data[0] ].nSelectCam[ pQue->fl.data[1]-1 ] = (short)pQue->fl.data[2];

		// 2017.02.27 Master/Slave��DB�w�� ---------->>>>

		// Master/Slave�敪 1�Ȃ�Master�A-1�Ȃ�Slave ����ȊO�Ȃ�Master(�����w�肵�Ȃ������ꍇ�A0�Ȃ̂�Master)
		switch(pQue->fl.data[3]){
			case 1:
				m_Param.linksw[ pQue->fl.data[0] ].emChSelMode[ pQue->fl.data[1]-1 ] = CH_SELECT_MASTER;
				break;
			case -1:
				m_Param.linksw[ pQue->fl.data[0] ].emChSelMode[ pQue->fl.data[1]-1 ] = CH_SELECT_SLAVE;
				break;
			default:
				m_Param.linksw[ pQue->fl.data[0] ].emChSelMode[ pQue->fl.data[1]-1 ] = CH_SELECT_MASTER;
				break;
		}

#ifdef ENABLE_MAIL_FORCIBLY_MASTER

		if( CH_SELECT_MASTER == m_Param.linksw[ pQue->fl.data[0] ].emChSelMode[ pQue->fl.data[1]-1 ] ){

			SetForciblyMaster( pQue->fl.data[0], pQue->fl.data[1]-1, pQue->fl.data[2]);
		}

#endif
		// 2017.02.27 Master/Slave��DB�w�� <<<<----------



		CalcParam();
		// ���M����
		// 2020.05.07 �ΏۃJ�����̂ݐݒ� ---------->>>>
		if (-1 == pQue->fl.data[3]) {
			m_emMainMode = MODE_START_SELECT_INIT;
			for (int ii = 0; ii<NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; ii++)
			{
				for (int jj = 0; jj<MAX_CAMLINKSW_OUTPUT; jj++)
				{
					m_Param.linksw[ii].bChSelCam[jj] = FALSE;							// �J�����I��ݒ� OFF
				}
			}
			m_Param.linksw[pQue->fl.data[0]].bChSelCam[pQue->fl.data[1] - 1] = TRUE;
		}
		else {
			m_emMainMode = MODE_START_INIT;
		}
		//m_emMainMode = MODE_START_INIT;
		// 2020.05.07 �ΏۃJ�����̂ݐݒ� <<<<----------
		m_nSubMode = 0;
		m_nCamMode = 0;
		m_nApxMode = 0;
		m_nAocMode = 0;
		m_nAocSubMode = 0;
		m_nRegSizeMode = 0;
		m_nSwMode = 0;
		break;

//---------------------------------------------------------
	case FACT_TO_LINKSW_04:									// �r���Q���ݒ�R�}���h
		// �r���Q���\���敪 �Z�b�g
		m_nSlaveJoinTorB = pQue->fl.data[0];
		// �r���Q���t���[��No �Z�b�g
		m_nSlaveJoinFNo = (DWORD)pQue->fl.data[1];
		// ���M����
		m_emMainMode = MODE_SLAVE_JOIN;
		m_nSubMode = 0;
		m_nCamMode = 0;
		m_nApxMode = 0;
		m_nAocMode = 0;
		m_nAocSubMode = 0;
		m_nRegSizeMode = 0;
		m_nSwMode = 0;
		break;

//---------------------------------------------------------
	case FACT_TO_LINKSW_05:									// �������Z�b�g�ʒm
		// �J�E���^���Z�b�g�t���[��No �Z�b�g
		m_nResetFNo = (DWORD)pQue->fl.data[0];
		// ���M����
		m_emMainMode = MODE_COUNTER_RESET;
		m_nSubMode = 0;
		m_nCamMode = 0;
		m_nApxMode = 0;
		m_nAocMode = 0;
		m_nAocSubMode = 0;
		m_nRegSizeMode = 0;
		m_nSwMode = 0;
		break;

//---------------------------------------------------------
	case FACT_TO_LINKSW_06:									// AOC�V���A���X�s�[�h�ύX
		// ���M����
		{
			// �o�͑��̔ԍ��i���������N�X�C�b�`������΁A�ʔԁj
			const int out_board_index = pQue->fl.data[0];
			if( pQue->fl.data[0] <= 0 ) {
				LOG(em_ERR), "[%s] �I���{�[�h�ُ�(board=%d, speed=%d)", my_sThreadName, pQue->fl.data[0], pQue->fl.data[1]);
				return true;
			}
			if (pQue->fl.data[1] <= 0) {
				LOG(em_ERR), "[%s] �I���X�s�[�h�ُ�(board=%d, speed=%d)", my_sThreadName, pQue->fl.data[0], pQue->fl.data[1]);
				return true;
			}

			// �o�͑��ԍ�����A�Ђ��Â����͑��ԍ�(camindex)���擾����B
			int nLinkSW = 0;
			int nFPGA = 0;
			int nChannel = -1;


			for( int iLinkSW =0;iLinkSW<NUM_CAMLINKSW; iLinkSW ++){
				for( int iFPGA=0; iFPGA<MAX_CAMLINKSW_FPGA; iFPGA++){
					const short *linksw_clients = m_Param.nCamSelBoardNo[iFPGA + MAX_CAMLINKSW_FPGA*iLinkSW];
					for( int iCh=0; iCh<MAX_CAMLINKSW_ALLINPUT; iCh++){
						const int connected_output = linksw_clients[iCh] + iFPGA*MAX_CAMLINKSW_OUTPUT + iLinkSW * MAX_CAMLINKSW_ALLOUTPUT;
						if( connected_output == out_board_index ){
							//nLinkSW = iLinkSW;
							nLinkSW = linksw_clients[iCh];
							////nFPGA = iFPGA;
							////nChannel = iCh+1;
							nFPGA  = iLinkSW * MAX_CAMLINKSW_FPGA + iCh / MAX_CAMLINKSW_INPUT;
							nChannel  = iCh % MAX_CAMLINKSW_INPUT;
						}
					}
				}
			}


			if( nChannel >= 0 ){
				m_emMainMode = MODE_CHANGE_AOC_SERIALSPEED;
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nAocSubMode = 0;
				m_nRegSizeMode = 0;
				m_nApxMode= nLinkSW;
				m_nSwMode = nFPGA;
				m_nAocMode = nChannel;

				int nAOCNo = m_nAocMode + m_nSwMode *MAX_CAMLINKSW_INPUT;
				LOG(em_MSG), "[%s] ���͒ʔ�[%d]�̃}�X�^�[��[%d-%d-%02d] (�ʔ�%d)", 
					my_sThreadName, out_board_index, 
					nLinkSW, nFPGA, nChannel,
					nAOCNo
				);
				m_Param.nSerialSpeeds[ nAOCNo ] = pQue->fl.data[1];
			}
			else{
				LOG(em_MSG), "[%s] ���͒ʔ�[%d]�̓X���[�u", 
					my_sThreadName, out_board_index);
				return true;
			}
		}
		break;
//---------------------------------------------------------
	case FACT_CYCLE_CHECK:									// ������`�F�b�N
		// ���M����
		m_emMainMode = MODE_CYCLE_CHECK;
		m_nSubMode = 0;
		m_nCamMode = 0;
		m_nApxMode = 0;
		m_nAocMode = 0;
		m_nAocSubMode = 0;
		m_nRegSizeMode = 0;
		m_nSwMode = 0;
		break;

//---------------------------------------------------------
	case FACT_COUNTER_CHECK:								// �J�E���^�`�F�b�N
		// �o�b�t�@�N���A
		memset(m_nCounterNow, 0x00, sizeof(m_nCounterNow));
		memset(m_nCounterSet, 0x00, sizeof(m_nCounterSet));
		// ���M����
		m_emMainMode = MODE_COUNTER_CHECK;
		m_nSubMode = 0;
		m_nCamMode = 0;
		m_nApxMode = 0;
		m_nAocMode = 0;
		m_nAocSubMode = 0;
		m_nRegSizeMode = 0;
		m_nSwMode = 0;
		break;

//---------------------------------------------------------
	default:
		// ���肦�Ȃ��I�I
		return false;
	}

	// �����܂ł����� �J���������N�X�C�b�` �֒ʒm
	if( ! CheckNextSend() ) return false;
	return true;
}

//------------------------------------------
// �����M����
// �߂�l ���A���
//------------------------------------------
bool LinkswManager::CheckNextSend()
{
	//// �O����
	memset(m_cSendBuf, 0x00, sizeof(m_cSendBuf));	// ���M�o�b�t�@ �N���A
	//// �T�u���[�h�ύX (�@�했�̐ݒ�����Ă���ꍇ�́A�T�u���[�h���ێ�)
	if( 0 == m_nCamMode &&
		0 == m_nApxMode &&
		0 == m_nAocMode &&
		0 == m_nAocSubMode &&
		0 == m_nRegSizeMode ) m_nSubMode++;


	//// ��������
	switch( m_emMainMode )
	{
//---------------------------------------------------------
	////////////////////////////////////
	// �I�[�v���`�F�b�N
	////////////////////////////////////
	case MODE_OPEN_CHECK:
		if( 1 == m_nSubMode &&
			0 == m_nApxMode &&
			0 == m_nAocMode &&
			0 == m_nAocSubMode &&
			0 == m_nRegSizeMode &&
			0 == m_nCamMode )
		{
			LOG(em_MSG), "[%s] <%d> �I�[�v���`�F�b�N �J�n -------------------------------------------------->>>", my_sThreadName, m_nSwMode);
			ChangeWaitEventNum( true );
		}

		LOG(em_INF), "[%s] <%d> �X�C�b�`FPGA �X�e�[�^�X���W�X�^", my_sThreadName, m_nSwMode);

		sprintf(m_cSendBuf, "@%02XR002\r", m_nSwMode);																	// �X�C�b�`FPGA �X�e�[�^�X���W�X�^
		break;

//---------------------------------------------------------
	////////////////////////////////////
	// ������
	////////////////////////////////////
	case MODE_START_UP:
		if( 1 == m_nSubMode &&
			0 == m_nApxMode &&
			0 == m_nAocMode &&
			0 == m_nAocSubMode &&
			0 == m_nRegSizeMode &&
			0 == m_nCamMode )
		{
			LOG(em_MSG), "[%s] <%d> ������ �J�n -------------------------------------------------->>>", my_sThreadName, m_nSwMode);
			ChangeWaitEventNum( true );
		}

		if( 1 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> �J����%.2d Opt-C:Link Line Buffer �i�[�o�X�����W�X�^ [%d]", my_sThreadName, m_nSwMode, m_nCamMode+1, m_Param.nLineBuffer);

			int nAddr = 0x102 + (0x010 * m_nCamMode);
			sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, m_Param.nLineBuffer);							// Opt-C:Link Line Buffer �i�[�o�X�����W�X�^

			// �J�������̃��[�h�ݒ�
			if( m_nCamMode == MAX_CAMLINKSW_ALLINPUT-1 )	m_nCamMode = 0;					// �Ō�̃J������
			else											m_nCamMode++;
		}
		else if( 2 == m_nSubMode )
		{
			//// ���W�X�^�T�C�Y�J��Ԃ����[�h�ݒ�
			if( 0 == m_nRegSizeMode )
			{
				LOG(em_MSG), "[%s] <%d> �J����%.2d Opt-C:Link YSize ���W�X�^ [%d]", my_sThreadName, m_nSwMode, m_nCamMode+1, m_Param.nYSize);
				m_nRegSizeMode = 2;
			}

			if( 2 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �J����%.2d Opt-C:Link YSize ���W�X�^ (7:0)", my_sThreadName, m_nSwMode, m_nCamMode+1);

				int nAddr = 0x104 + (0x010 * m_nCamMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, LOBYTE(m_Param.nYSize));						// Opt-C:Link YSize ���W�X�^ (7:0)
			}
			else if( 1 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �J����%.2d Opt-C:Link YSize ���W�X�^ (15:8)", my_sThreadName, m_nSwMode, m_nCamMode+1);

				int nAddr = 0x105 + (0x010 * m_nCamMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, HIBYTE(m_Param.nYSize));						// Opt-C:Link YSize ���W�X�^ (15:8)

				// �J�������̃��[�h�ݒ�
				if( m_nCamMode == MAX_CAMLINKSW_ALLINPUT-1 )	m_nCamMode = 0;				// �Ō�̃J������
				else											m_nCamMode++;
			}
		}
		else if( 3 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> �{�[�h%.2d ���[�h���W�X�^ (�r���Q���J�E���^���[�h�I��, �x���J�E���g�M���I��) [%d]", my_sThreadName, m_nSwMode, m_nApxMode+1, 64 + 8 * COUNTER_MODE);

			int nAddr = 0x010 + (0x010 * m_nApxMode);
			sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 64 + 8 * COUNTER_MODE);							// �{�[�h ���[�h���W�X�^ (�r���Q���J�E���^���[�h�I��, �x���J�E���g�M���I��)

			// �{�[�h���̃��[�h�ݒ�
			if( m_nApxMode == MAX_CAMLINKSW_OUTPUT-1 )		m_nApxMode = 0;					// �Ō�̃{�[�h��
			else											m_nApxMode++;
		}
		else if( 4 == m_nSubMode )
		{
			//// ���W�X�^�T�C�Y�J��Ԃ����[�h�ݒ�
			if( 0 == m_nRegSizeMode )
			{
				LOG(em_MSG), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ [%d]", my_sThreadName, m_nSwMode, m_nApxMode+1, 0);
				m_nRegSizeMode = 8;
			}

			if( 8 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (7:0)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x018 + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (7:0) Write
			}
			else if( 7 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (15:8)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x019 + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (15:8) Write
			}
			else if( 6 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (23:16)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01A + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (23:16) Write
			}
			else if( 5 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (31:24)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01B + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (31:24) Write
			}
			else if( 4 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (39:32)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01C + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (39:32) Write
			}
			else if( 3 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (47:40)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01D + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (47:40) Write
			}
			else if( 2 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (55:48)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01E + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (55:48) Write
			}
			else if( 1 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (63:56)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01F + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (63:56) Write

				// �{�[�h���̃��[�h�ݒ�
				if( m_nApxMode == MAX_CAMLINKSW_OUTPUT-1 )		m_nApxMode = 0;				// �Ō�̃{�[�h��
				else											m_nApxMode++;
			}
		}
		else if( 5 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> �{�[�h%.2d �J�����I�����W�X�^ [%d(0:�o�͒�~ 1�`8:����CH)]", my_sThreadName, m_nSwMode, m_nApxMode+1, m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]);

			int nAddr = 0x011 + (0x010 * m_nApxMode);
			sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]);	// �{�[�h �J�����I�����W�X�^

			// �{�[�h���̃��[�h�ݒ�
			if( m_nApxMode == MAX_CAMLINKSW_OUTPUT-1 )		m_nApxMode = 0;					// �Ō�̃{�[�h��
			else											m_nApxMode++;
		}
		else if( 6 == m_nSubMode )
		{
			// �{�[�h�ԍ�
			int nBoard = m_Param.nCamSelBoardNo[m_nSwMode][m_nCamMode];

			// �{�[�h�ԍ� �����l �̏ꍇ
			if( 0 == nBoard )
			{
				// �X�C�b�`�}�X�^ and �X�C�b�`�}�X�^���̃J����
				if( m_Param.linksw[m_nSwMode].bStatus && MAX_CAMLINKSW_INPUT >= m_nCamMode+1 )
				{
					// �X�C�b�`�X���[�u���̃{�[�g�Ɛڑ�����J����������
					for( int ii=0; ii<MAX_CAMLINKSW_OUTPUT; ii++ )
					{
						if( m_nCamMode+1 == m_Param.linksw[m_nSwMode + 1].nSelectCam[ii] )
						{
							nBoard = 16;	// Arria5
							break;
						}
					}
				}
				// �X�C�b�`�X���[�u and �X�C�b�`�X���[�u���̃J����
				if( ! m_Param.linksw[m_nSwMode].bStatus && MAX_CAMLINKSW_INPUT < m_nCamMode+1 )
				{
					// �X�C�b�`�}�X�^���̃{�[�g�Ɛڑ�����J����������
					for( int ii=0; ii<MAX_CAMLINKSW_OUTPUT; ii++ )
					{
						if( m_nCamMode+1 == m_Param.linksw[m_nSwMode - 1].nSelectCam[ii] )
						{
							nBoard = 16;	// Arria5
							break;
						}
					}
				}
			}

			LOG(em_INF), "[%s] <%d> �J����%.2d �{�[�h�I�����W�X�^ [%d(1�`15:�{�[�hNo 16:Arria5)]", my_sThreadName, m_nSwMode, m_nCamMode+1, nBoard);

			int nAddr = 0x108 + (0x010 * m_nCamMode);
			sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, nBoard);											// �J���� �{�[�h�I�����W�X�^

			// �J�������̃��[�h�ݒ�
			if( m_nCamMode == MAX_CAMLINKSW_ALLINPUT-1 )	m_nCamMode = 0;					// �Ō�̃J������
			else											m_nCamMode++;
		}
		else if( 7 == m_nSubMode )
		{
			//// AOC�T�u���[�h�ύX
			if( 0 == m_nRegSizeMode )	m_nAocSubMode++;

			if( 1 == m_nAocSubMode &&
				0 == m_nRegSizeMode )
			{
				LOG(em_MSG), "[%s] <%d> AOC%.2d Line Buffer �i�[�o�X�����W�X�^ [%d]", my_sThreadName, m_nSwMode, m_nAocMode+1, m_Param.nLineBuffer);
			}

			if( 1 == m_nAocSubMode )
			{
				//// ���W�X�^�T�C�Y�J��Ԃ����[�h�ݒ�
				if( 0 == m_nRegSizeMode )	m_nRegSizeMode = 2;

				if( 2 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Address ���W�X�^ (7:0)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x180 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x70);									// AOC Address ���[�h���W�X�^ (7:0)
				}
				else if( 1 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Address ���W�X�^ (15:8)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x181 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x00);									// AOC Address ���[�h���W�X�^ (15:8)
				}
			}
			else if( 2 == m_nAocSubMode )
			{
				//// ���W�X�^�T�C�Y�J��Ԃ����[�h�ݒ�
				if( 0 == m_nRegSizeMode )	m_nRegSizeMode = 4;

				if( 4 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data ���W�X�^ (7:0)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x182 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x00);									// AOC Write Data ���W�X�^ (7:0)
				}
				else if( 3 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data ���W�X�^ (15:8)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x183 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, m_Param.nLineBuffer);					// AOC Write Data ���W�X�^ (15:8)
				}
				else if( 2 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data ���W�X�^ (23:16)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x184 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x00);									// AOC Write Data ���W�X�^ (23:16)
				}
				else if( 1 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data ���W�X�^ (31:24)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x185 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x00);									// AOC Write Data ���W�X�^ (31:24)
				}
			}
			else if( 3 == m_nAocSubMode )
			{
				LOG(em_INF), "[%s] <%d> AOC%.2d Ctrl ���W�X�^ (Write Access)", my_sThreadName, m_nSwMode, m_nAocMode+1);

				int nAddr = 0x18A + (0x010 * m_nAocMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x11);										// AOC Ctrl ���W�X�^ (Write Access)
			}
			else if( 4 == m_nAocSubMode )
			{
				LOG(em_INF), "[%s] <%d> AOC%.2d Ctrl ���W�X�^ (Read Access)", my_sThreadName, m_nSwMode, m_nAocMode+1);

				int nAddr = 0x18A + (0x010 * m_nAocMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);													// AOC Ctrl ���W�X�^ (Read Access)
			}
			else if( 5 == m_nAocSubMode )
			{
				LOG(em_INF), "[%s] <%d> AOC%.2d Ctrl ���W�X�^ (���M�������Z�b�g)", my_sThreadName, m_nSwMode, m_nAocMode+1);

				int nAddr = 0x18A + (0x010 * m_nAocMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x04);										// AOC Ctrl ���W�X�^ (���M�������Z�b�g)

				// AOC���̃��[�h�ݒ�
				if( m_nAocMode == MAX_CAMLINKSW_INPUT-1 )	m_nAocMode = 0;					// �Ō��AOC��
				else										m_nAocMode++;
			}
		}
		else if( 8 == m_nSubMode )
		{
			//// AOC�T�u���[�h�ύX
			if( 0 == m_nRegSizeMode )	m_nAocSubMode++;

			if( 1 == m_nAocSubMode &&
				0 == m_nRegSizeMode )
			{
				LOG(em_MSG), "[%s] <%d> AOC%.2d YSize ���W�X�^ [%d]", my_sThreadName, m_nSwMode, m_nAocMode+1, m_Param.nYSize);
			}

			if( 1 == m_nAocSubMode )
			{
				//// ���W�X�^�T�C�Y�J��Ԃ����[�h�ݒ�
				if( 0 == m_nRegSizeMode )	m_nRegSizeMode = 2;

				if( 2 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Address ���W�X�^ (7:0)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x180 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x78);									// AOC Address ���[�h���W�X�^ (7:0)
				}
				else if( 1 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Address ���W�X�^ (15:8)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x181 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x00);									// AOC Address ���[�h���W�X�^ (15:8)
				}
			}
			else if( 2 == m_nAocSubMode )
			{
				//// ���W�X�^�T�C�Y�J��Ԃ����[�h�ݒ�
				if( 0 == m_nRegSizeMode )	m_nRegSizeMode = 4;

				if( 4 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data ���W�X�^ (7:0)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x182 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, LOBYTE(m_Param.nYSize));					// AOC Write Data ���W�X�^ (7:0)
				}
				else if( 3 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data ���W�X�^ (15:8)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x183 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, HIBYTE(m_Param.nYSize));					// AOC Write Data ���W�X�^ (15:8)
				}
				else if( 2 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data ���W�X�^ (23:16)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x184 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x00);									// AOC Write Data ���W�X�^ (23:16)
				}
				else if( 1 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data ���W�X�^ (31:24)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x185 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x00);									// AOC Write Data ���W�X�^ (31:24)
				}
			}
			else if( 3 == m_nAocSubMode )
			{
				LOG(em_INF), "[%s] <%d> AOC%.2d Ctrl ���W�X�^ (Write Access)", my_sThreadName, m_nSwMode, m_nAocMode+1);

				int nAddr = 0x18A + (0x010 * m_nAocMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x11);										// AOC Ctrl ���W�X�^ (Write Access)
			}
			else if( 4 == m_nAocSubMode )
			{
				LOG(em_INF), "[%s] <%d> AOC%.2d Ctrl ���W�X�^ (Read Access)", my_sThreadName, m_nSwMode, m_nAocMode+1);

				int nAddr = 0x18A + (0x010 * m_nAocMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);													// AOC Ctrl ���W�X�^ (Read Access)
			}
			else if( 5 == m_nAocSubMode )
			{
				LOG(em_INF), "[%s] <%d> AOC%.2d Ctrl ���W�X�^ (���M�������Z�b�g)", my_sThreadName, m_nSwMode, m_nAocMode+1);

				int nAddr = 0x18A + (0x010 * m_nAocMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x04);										// AOC Ctrl ���W�X�^ (���M�������Z�b�g)

				// AOC���̃��[�h�ݒ�
				if( m_nAocMode == MAX_CAMLINKSW_INPUT-1 )	m_nAocMode = 0;					// �Ō��AOC��
				else										m_nAocMode++;
			}
		}
		else if( 10 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> AOC%.2d CC���[�h���W�X�^ [%d(0:FPGA 1:Board)]", my_sThreadName, m_nSwMode, m_nAocMode+1, m_bCCMode);

			int nAddr = 0x18B + (0x010 * m_nAocMode);
			sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, m_bCCMode);										// AOC CC���[�h���W�X�^

			// AOC���̃��[�h�ݒ�
			if( m_nAocMode == MAX_CAMLINKSW_INPUT-1 )		m_nAocMode = 0;					// �Ō��AOC��
			else											m_nAocMode++;
		}
		else if( 9 == m_nSubMode )
		{
			//// AOC�T�u���[�h�ύX
			if( 0 == m_nRegSizeMode )	m_nAocSubMode++;

			if( 1 == m_nAocSubMode &&
				0 == m_nRegSizeMode )
			{
				LOG(em_MSG), "[%s] <%d> AOC%.2d Serial ���W�X�^ [%d]", my_sThreadName, m_nSwMode, m_nAocMode+1, 9600);
			}

			if( 1 == m_nAocSubMode )
			{
				//// ���W�X�^�T�C�Y�J��Ԃ����[�h�ݒ�
				if( 0 == m_nRegSizeMode )	m_nRegSizeMode = 2;

				if( 2 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Address ���W�X�^ (7:0)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x180 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x7C);									// AOC Address ���[�h���W�X�^ (7:0)
				}
				else if( 1 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Address ���W�X�^ (15:8)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x181 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x00);									// AOC Address ���[�h���W�X�^ (15:8)
				}
			}
			else if( 2 == m_nAocSubMode )
			{
				//// ���W�X�^�T�C�Y�J��Ԃ����[�h�ݒ�
				if( 0 == m_nRegSizeMode )	m_nRegSizeMode = 4;

				if( 4 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data ���W�X�^ (7:0)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x182 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, LOBYTE(m_Param.nSamplingTiming()));					// AOC Write Data ���W�X�^ (7:0)
				}
				else if( 3 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data ���W�X�^ (15:8)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x183 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, HIBYTE(m_Param.nSamplingTiming()));					// AOC Write Data ���W�X�^ (15:8)
				}
				else if( 2 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data ���W�X�^ (23:16)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x184 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, LOBYTE(m_Param.nSamplingDelay()));									// AOC Write Data ���W�X�^ (23:16)
				}
				else if( 1 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data ���W�X�^ (31:24)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x185 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, HIBYTE(m_Param.nSamplingDelay()));									// AOC Write Data ���W�X�^ (31:24)
				}
			}
			else if( 3 == m_nAocSubMode )
			{
				LOG(em_INF), "[%s] <%d> AOC%.2d Ctrl ���W�X�^ (Write Access)", my_sThreadName, m_nSwMode, m_nAocMode+1);

				int nAddr = 0x18A + (0x010 * m_nAocMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x11);										// AOC Ctrl ���W�X�^ (Write Access)
			}
			else if( 4 == m_nAocSubMode )
			{
				LOG(em_INF), "[%s] <%d> AOC%.2d Ctrl ���W�X�^ (Read Access)", my_sThreadName, m_nSwMode, m_nAocMode+1);

				int nAddr = 0x18A + (0x010 * m_nAocMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);													// AOC Ctrl ���W�X�^ (Read Access)
			}
			else if( 5 == m_nAocSubMode )
			{
				LOG(em_INF), "[%s] <%d> AOC%.2d Ctrl ���W�X�^ (���M�������Z�b�g)", my_sThreadName, m_nSwMode, m_nAocMode+1);

				int nAddr = 0x18A + (0x010 * m_nAocMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x04);										// AOC Ctrl ���W�X�^ (���M�������Z�b�g)

				// AOC���̃��[�h�ݒ�
				if( m_nAocMode == MAX_CAMLINKSW_INPUT-1 )	m_nAocMode = 0;					// �Ō��AOC��
				else										m_nAocMode++;
			}
		}
		else if( 11 == m_nSubMode )
		{
			LOG(em_MSG), "[%s] <%d> ������ �I�� --------------------------------------------------<<<" , my_sThreadName, m_nSwMode);

			// ������ �� �ݒ�҂�
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nApxMode = 0;
			m_nAocMode = 0;
			m_nAocSubMode = 0;
			m_nRegSizeMode = 0;
			m_nSwMode = 0;
			ChangeWaitEventNum( false );													// 1�V�[�P���X���튮��
		}
		else
		{
			LOG(em_ERR), "[%s] <%d> �T�u���[�h�ُ� [mode = ������, sub_mode = %d]", my_sThreadName, m_nSwMode, m_nSubMode);
			syslog(210, "[<%d> mode = ������ sub_mode = %d]", m_nSwMode, m_nSubMode);
			ChangeWaitEventNum( false );
			return false;
		}
		break;

//---------------------------------------------------------
	////////////////////////////////////
	// �����J�n�ݒ�
	////////////////////////////////////
	case MODE_START_INIT:
		if( 1 == m_nSubMode &&
			0 == m_nApxMode &&
			0 == m_nAocMode &&
			0 == m_nAocSubMode &&
			0 == m_nRegSizeMode &&
			0 == m_nCamMode )
		{
			LOG(em_MSG), "[%s] <%d> �����J�n�ݒ� �J�n -------------------------------------------------->>>", my_sThreadName, m_nSwMode);
			ChangeWaitEventNum( true );
		}

		if( 1 == m_nSubMode )
		{
			//// ���W�X�^�T�C�Y�J��Ԃ����[�h�ݒ�
			if( 0 == m_nRegSizeMode )
			{
				LOG(em_MSG), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ [%d]", my_sThreadName, m_nSwMode, m_nApxMode+1, 0);
				m_nRegSizeMode = 8;
			}

			if( 8 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (7:0)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x018 + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (7:0) Write
			}
			else if( 7 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (15:8)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x019 + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (15:8) Write
			}
			else if( 6 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (23:16)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01A + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (23:16) Write
			}
			else if( 5 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (31:24)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01B + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (31:24) Write
			}
			else if( 4 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (39:32)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01C + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (39:32) Write
			}
			else if( 3 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (47:40)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01D + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (47:40) Write
			}
			else if( 2 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (55:48)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01E + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (55:48) Write
			}
			else if( 1 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (63:56)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01F + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (63:56) Write

				// �{�[�h���̃��[�h�ݒ�
				if( m_nApxMode == MAX_CAMLINKSW_OUTPUT-1 )		m_nApxMode = 0;				// �Ō�̃{�[�h��
				else											m_nApxMode++;
			}
		}
		else if( 2 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> �{�[�h%.2d �J�����I�����W�X�^ [%d(0:�o�͒�~ 1�`8:����CH)]", my_sThreadName, m_nSwMode, m_nApxMode+1, m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]);

			int nAddr = 0x011 + (0x010 * m_nApxMode);
			sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]);	// �{�[�h �J�����I�����W�X�^

			// �{�[�h���̃��[�h�ݒ�
			if( m_nApxMode == MAX_CAMLINKSW_OUTPUT-1 )		m_nApxMode = 0;					// �Ō�̃{�[�h��
			else											m_nApxMode++;
		}
		else if( 3 == m_nSubMode )
		{
			// �{�[�h�ԍ�
			int nBoard = m_Param.nCamSelBoardNo[m_nSwMode][m_nCamMode];

			// �{�[�h�ԍ� �����l �̏ꍇ
			if( 0 == nBoard )
			{
				// �X�C�b�`�}�X�^ and �X�C�b�`�}�X�^���̃J����
				if( m_Param.linksw[m_nSwMode].bStatus && MAX_CAMLINKSW_INPUT >= m_nCamMode+1 )
				{
					// �X�C�b�`�X���[�u���̃{�[�g�Ɛڑ�����J����������
					for( int ii=0; ii<MAX_CAMLINKSW_OUTPUT; ii++ )
					{
						if( m_nCamMode+1 == m_Param.linksw[m_nSwMode + 1].nSelectCam[ii] )
						{
							nBoard = 16;	// Arria5
							break;
						}
					}
				}
				// �X�C�b�`�X���[�u and �X�C�b�`�X���[�u���̃J����
				if( ! m_Param.linksw[m_nSwMode].bStatus && MAX_CAMLINKSW_INPUT < m_nCamMode+1 )
				{
					// �X�C�b�`�}�X�^���̃{�[�g�Ɛڑ�����J����������
					for( int ii=0; ii<MAX_CAMLINKSW_OUTPUT; ii++ )
					{
						if( m_nCamMode+1 == m_Param.linksw[m_nSwMode - 1].nSelectCam[ii] )
						{
							nBoard = 16;	// Arria5
							break;
						}
					}
				}
			}

			LOG(em_INF), "[%s] <%d> �J����%.2d �{�[�h�I�����W�X�^ [%d(1�`15:�{�[�hNo 16:Arria5)]", my_sThreadName, m_nSwMode, m_nCamMode+1, nBoard);

			int nAddr = 0x108 + (0x010 * m_nCamMode);
			sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, nBoard);											// �J���� �{�[�h�I�����W�X�^

			// �J�������̃��[�h�ݒ�
			if( m_nCamMode == MAX_CAMLINKSW_ALLINPUT-1 )	m_nCamMode = 0;					// �Ō�̃J������
			else											m_nCamMode++;
		}
		else if( 4 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> AOC%.2d CC���[�h���W�X�^ [%d(0:FPGA 1:Board)]", my_sThreadName, m_nSwMode, m_nAocMode+1, m_bCCMode);

			int nAddr = 0x18B + (0x010 * m_nAocMode);
			sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, m_bCCMode);										// AOC CC���[�h���W�X�^

			// AOC���̃��[�h�ݒ�
			if( m_nAocMode == MAX_CAMLINKSW_INPUT-1 )		m_nAocMode = 0;					// �Ō��AOC��
			else											m_nAocMode++;
		}
		else if( 5 == m_nSubMode )
		{
			LOG(em_MSG), "[%s] <%d> �����J�n�ݒ� �I�� --------------------------------------------------<<<" , my_sThreadName, m_nSwMode);

			// �����J�n�ݒ� �� �ݒ�҂�
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nApxMode = 0;
			m_nAocMode = 0;
			m_nAocSubMode = 0;
			m_nRegSizeMode = 0;
			m_nSwMode = 0;
			ChangeWaitEventNum( false );													// 1�V�[�P���X���튮��

			// �����Ǘ� �� �J���������N�X�C�b�`�ݒ芮���ʒm
			// 20190205 �v���ɑ΂��鉞�����ʗp --->>>
			//Send_ToDefect_StartInit();
			if (FACT_TO_LINKSW_02 == m_RecvQue.nEventNo) {
				Send_ToDefect_StartInit();
			}
			else {
				Send_ToDefect_SelectInit();
			}
			// 20190205 �v���ɑ΂��鉞�����ʗp ---<<<
		}
		else
		{
			LOG(em_ERR), "[%s] <%d> �T�u���[�h�ُ� [mode = �����J�n�ݒ�, sub_mode = %d]", my_sThreadName, m_nSwMode, m_nSubMode);
			syslog(210, "[<%d> mode = �����J�n�ݒ� sub_mode = %d]", m_nSwMode, m_nSubMode);
			ChangeWaitEventNum( false );
			return false;
		}
		break;

// 2020.05.07 �ΏۃJ�����̂ݐݒ� ---------->>>>
//---------------------------------------------------------
////////////////////////////////////
// �����J�n�ݒ�(�J�����I��ݒ�)
////////////////////////////////////
	case MODE_START_SELECT_INIT:
		if (1 == m_nSubMode &&
			0 == m_nApxMode &&
			0 == m_nAocMode &&
			0 == m_nAocSubMode &&
			0 == m_nRegSizeMode &&
			0 == m_nCamMode)
		{
			LOG(em_MSG), "[%s] <%d> �����J�n�ݒ�(�J�����I��ݒ�) �J�n -------------------------------------------------->>>", my_sThreadName, m_nSwMode);
			ChangeWaitEventNum(true);
		}

		if (1 == m_nSubMode)
		{
			//// ���W�X�^�T�C�Y�J��Ԃ����[�h�ݒ�
			if (0 == m_nRegSizeMode)
			{
				if(TRUE == m_Param.linksw[m_nSwMode].bChSelCam[m_nApxMode]) {
					LOG(em_MSG), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ [%d]", my_sThreadName, m_nSwMode, m_nApxMode + 1, 0);
				}
				else
				{
					LOG(em_MSG), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ [-]", my_sThreadName, m_nSwMode, m_nApxMode + 1);
				}
				m_nRegSizeMode = 8;
			}

			if (8 == m_nRegSizeMode)
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (7:0)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x018 + (0x010 * m_nApxMode);
				if (TRUE == m_Param.linksw[m_nSwMode].bChSelCam[m_nApxMode]) {
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (7:0) Write
				}
				else
				{
					// �r���Q�����Ȃ��{�[�h��Read���Ă����I
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (15:8) Read
				}
			}
			else if (7 == m_nRegSizeMode)
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (15:8)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x019 + (0x010 * m_nApxMode);
				if (TRUE == m_Param.linksw[m_nSwMode].bChSelCam[m_nApxMode]) {
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (7:0) Write
				}
				else
				{
					// �r���Q�����Ȃ��{�[�h��Read���Ă����I
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (15:8) Read
				}
			}
			else if (6 == m_nRegSizeMode)
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (23:16)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01A + (0x010 * m_nApxMode);
				if (TRUE == m_Param.linksw[m_nSwMode].bChSelCam[m_nApxMode]) {
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (7:0) Write
				}
				else
				{
					// �r���Q�����Ȃ��{�[�h��Read���Ă����I
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (15:8) Read
				}
			}
			else if (5 == m_nRegSizeMode)
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (31:24)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01B + (0x010 * m_nApxMode);
				if (TRUE == m_Param.linksw[m_nSwMode].bChSelCam[m_nApxMode]) {
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (7:0) Write
				}
				else
				{
					// �r���Q�����Ȃ��{�[�h��Read���Ă����I
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (15:8) Read
				}
			}
			else if (4 == m_nRegSizeMode)
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (39:32)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01C + (0x010 * m_nApxMode);
				if (TRUE == m_Param.linksw[m_nSwMode].bChSelCam[m_nApxMode]) {
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (7:0) Write
				}
				else
				{
					// �r���Q�����Ȃ��{�[�h��Read���Ă����I
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (15:8) Read
				}
			}
			else if (3 == m_nRegSizeMode)
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (47:40)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01D + (0x010 * m_nApxMode);
				if (TRUE == m_Param.linksw[m_nSwMode].bChSelCam[m_nApxMode]) {
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (7:0) Write
				}
				else
				{
					// �r���Q�����Ȃ��{�[�h��Read���Ă����I
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (15:8) Read
				}
			}
			else if (2 == m_nRegSizeMode)
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (55:48)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01E + (0x010 * m_nApxMode);
				if (TRUE == m_Param.linksw[m_nSwMode].bChSelCam[m_nApxMode]) {
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (7:0) Write
				}
				else
				{
					// �r���Q�����Ȃ��{�[�h��Read���Ă����I
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (15:8) Read
				}
			}
			else if (1 == m_nRegSizeMode)
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (63:56)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01F + (0x010 * m_nApxMode);
				if (TRUE == m_Param.linksw[m_nSwMode].bChSelCam[m_nApxMode]) {
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (7:0) Write
				}
				else
				{
					// �r���Q�����Ȃ��{�[�h��Read���Ă����I
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (15:8) Read
				}

																														// �{�[�h���̃��[�h�ݒ�
				if (m_nApxMode == MAX_CAMLINKSW_OUTPUT - 1)		m_nApxMode = 0;				// �Ō�̃{�[�h��
				else											m_nApxMode++;
			}
		}
		else if (2 == m_nSubMode)
		{
			int nAddr = 0x011 + (0x010 * m_nApxMode);

			if (TRUE == m_Param.linksw[m_nSwMode].bChSelCam[m_nApxMode]) {
				LOG(em_INF), "[%s] <%d> �{�[�h%.2d �J�����I�����W�X�^ [%d(0:�o�͒�~ 1�`8:����CH)]", my_sThreadName, m_nSwMode, m_nApxMode + 1, m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]);	// �{�[�h �J�����I�����W�X�^
			}
			else
			{
				// �r���Q�����Ȃ��{�[�h��Read���Ă����I
				LOG(em_INF), "[%s] <%d> �{�[�h%.2d �J�����I�����W�X�^ [%d(0:�o�͒�~ 1�`8:����CH)] Read", my_sThreadName, m_nSwMode, m_nApxMode + 1, m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (15:8) Read
			}


																															// �{�[�h���̃��[�h�ݒ�
			if (m_nApxMode == MAX_CAMLINKSW_OUTPUT - 1)		m_nApxMode = 0;					// �Ō�̃{�[�h��
			else											m_nApxMode++;
		}
		else if (3 == m_nSubMode)
		{
			LOG(em_MSG), "[%s] <%d> �����J�n�ݒ�(�J�����I��ݒ�) �I�� --------------------------------------------------<<<", my_sThreadName, m_nSwMode);

			// �����J�n�ݒ� �� �ݒ�҂�
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nApxMode = 0;
			m_nAocMode = 0;
			m_nAocSubMode = 0;
			m_nRegSizeMode = 0;
			m_nSwMode = 0;
			ChangeWaitEventNum(false);													// 1�V�[�P���X���튮��

			// �����Ǘ� �� �J���������N�X�C�b�`�ݒ芮���ʒm
			Send_ToDefect_SelectInit();
		}
		else
		{
			LOG(em_ERR), "[%s] <%d> �T�u���[�h�ُ� [mode = �����J�n�ݒ�, sub_mode = %d]", my_sThreadName, m_nSwMode, m_nSubMode);
			syslog(210, "[<%d> mode = �����J�n�ݒ� sub_mode = %d]", m_nSwMode, m_nSubMode);
			ChangeWaitEventNum(false);
			return false;
		}
		break;


// 2020.05.07 �ΏۃJ�����̂ݐݒ� <<<<----------

//---------------------------------------------------------
	////////////////////////////////////
	// �r���Q���ݒ�
	////////////////////////////////////
	case MODE_SLAVE_JOIN:
		if( 1 == m_nSubMode &&
			0 == m_nApxMode &&
			0 == m_nAocMode &&
			0 == m_nAocSubMode &&
			0 == m_nRegSizeMode &&
			0 == m_nCamMode )
		{
			LOG(em_MSG), "[%s] <%d> �r���Q���ݒ� �J�n -------------------------------------------------->>>", my_sThreadName, m_nSwMode);
			ChangeWaitEventNum( true );
		}

		if( 1 == m_nSubMode )
		{
			//// ���W�X�^�T�C�Y�J��Ԃ����[�h�ݒ�
			if( 0 == m_nRegSizeMode )
			{
				if( (0 != m_nSelectCam[m_nSwMode][m_nApxMode] ||
					 0 != m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]) &&		// �J��������CH�ݒ�L�� ����
					m_nSlaveJoinTorB == m_nSelectTorB[m_nSwMode][m_nApxMode] )		// �r���Q���\���敪��v
				{
					LOG(em_MSG), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ [%I64d]", my_sThreadName, m_nSwMode, m_nApxMode+1, (__int64)(((__int64)m_nSlaveJoinFNo - (__int64)m_nResetFNo) * SIZE_FRAME_IMG_Y));
				}
				else
				{
					LOG(em_MSG), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ [-]", my_sThreadName, m_nSwMode, m_nApxMode+1);
				}
				m_nRegSizeMode = 8;
			}

			if( 8 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (7:0)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x018 + (0x010 * m_nApxMode);
				if( (0 != m_nSelectCam[m_nSwMode][m_nApxMode] ||
					 0 != m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]) &&		// �J��������CH�ݒ�L�� ����
					m_nSlaveJoinTorB == m_nSelectTorB[m_nSwMode][m_nApxMode] )		// �r���Q���\���敪��v
				{
					__int64 nVal = (((__int64)m_nSlaveJoinFNo - (__int64)m_nResetFNo) * SIZE_FRAME_IMG_Y);
					nVal >>= (64 - 8 * 8);
					nVal &= 0xFF;
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, nVal);									// �{�[�h �r���Q�����W�X�^ (7:0) Write
				}
				else
				{
					// �r���Q�����Ȃ��{�[�h��Read���Ă����I
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (7:0) Read
				}
			}
			else if( 7 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (15:8)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x019 + (0x010 * m_nApxMode);
				if( (0 != m_nSelectCam[m_nSwMode][m_nApxMode] ||
					 0 != m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]) &&		// �J��������CH�ݒ�L�� ����
					m_nSlaveJoinTorB == m_nSelectTorB[m_nSwMode][m_nApxMode] )		// �r���Q���\���敪��v
				{
					__int64 nVal = (((__int64)m_nSlaveJoinFNo - (__int64)m_nResetFNo) * SIZE_FRAME_IMG_Y);
					nVal >>= (64 - 8 * 7);
					nVal &= 0xFF;
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, nVal);									// �{�[�h �r���Q�����W�X�^ (15:8) Write
				}
				else
				{
					// �r���Q�����Ȃ��{�[�h��Read���Ă����I
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (15:8) Read
				}
			}
			else if( 6 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (23:16)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01A + (0x010 * m_nApxMode);
				if( (0 != m_nSelectCam[m_nSwMode][m_nApxMode] ||
					 0 != m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]) &&		// �J��������CH�ݒ�L�� ����
					m_nSlaveJoinTorB == m_nSelectTorB[m_nSwMode][m_nApxMode] )		// �r���Q���\���敪��v
				{
					__int64 nVal = (((__int64)m_nSlaveJoinFNo - (__int64)m_nResetFNo) * SIZE_FRAME_IMG_Y);
					nVal >>= (64 - 8 * 6);
					nVal &= 0xFF;
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, nVal);									// �{�[�h �r���Q�����W�X�^ (23:16) Write
				}
				else
				{
					// �r���Q�����Ȃ��{�[�h��Read���Ă����I
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (23:16) Read
				}
			}
			else if( 5 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (31:24)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01B + (0x010 * m_nApxMode);
				if( (0 != m_nSelectCam[m_nSwMode][m_nApxMode] ||
					 0 != m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]) &&		// �J��������CH�ݒ�L�� ����
					m_nSlaveJoinTorB == m_nSelectTorB[m_nSwMode][m_nApxMode] )		// �r���Q���\���敪��v
				{
					__int64 nVal = (((__int64)m_nSlaveJoinFNo - (__int64)m_nResetFNo) * SIZE_FRAME_IMG_Y);
					nVal >>= (64 - 8 * 5);
					nVal &= 0xFF;
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, nVal);									// �{�[�h �r���Q�����W�X�^ (31:24) Write
				}
				else
				{
					// �r���Q�����Ȃ��{�[�h��Read���Ă����I
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (31:24) Read
				}
			}
			else if( 4 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (39:32)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01C + (0x010 * m_nApxMode);
				if( (0 != m_nSelectCam[m_nSwMode][m_nApxMode] ||
					 0 != m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]) &&		// �J��������CH�ݒ�L�� ����
					m_nSlaveJoinTorB == m_nSelectTorB[m_nSwMode][m_nApxMode] )		// �r���Q���\���敪��v
				{
					__int64 nVal = (((__int64)m_nSlaveJoinFNo - (__int64)m_nResetFNo) * SIZE_FRAME_IMG_Y);
					nVal >>= (64 - 8 * 4);
					nVal &= 0xFF;
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, nVal);									// �{�[�h �r���Q�����W�X�^ (39:32) Write
				}
				else
				{
					// �r���Q�����Ȃ��{�[�h��Read���Ă����I
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (39:32) Read
				}
			}
			else if( 3 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (47:40)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01D + (0x010 * m_nApxMode);
				if( (0 != m_nSelectCam[m_nSwMode][m_nApxMode] ||
					 0 != m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]) &&		// �J��������CH�ݒ�L�� ����
					m_nSlaveJoinTorB == m_nSelectTorB[m_nSwMode][m_nApxMode] )		// �r���Q���\���敪��v
				{
					__int64 nVal = (((__int64)m_nSlaveJoinFNo - (__int64)m_nResetFNo) * SIZE_FRAME_IMG_Y);
					nVal >>= (64 - 8 * 3);
					nVal &= 0xFF;
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, nVal);									// �{�[�h �r���Q�����W�X�^ (47:40) Write
				}
				else
				{
					// �r���Q�����Ȃ��{�[�h��Read���Ă����I
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (47:40) Read
				}
			}
			else if( 2 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (55:48)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01E + (0x010 * m_nApxMode);
				if( (0 != m_nSelectCam[m_nSwMode][m_nApxMode] ||
					 0 != m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]) &&		// �J��������CH�ݒ�L�� ����
					m_nSlaveJoinTorB == m_nSelectTorB[m_nSwMode][m_nApxMode] )		// �r���Q���\���敪��v
				{
					__int64 nVal = (((__int64)m_nSlaveJoinFNo - (__int64)m_nResetFNo) * SIZE_FRAME_IMG_Y);
					nVal >>= (64 - 8 * 2);
					nVal &= 0xFF;
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, nVal);									// �{�[�h �r���Q�����W�X�^ (55:48) Write
				}
				else
				{
					// �r���Q�����Ȃ��{�[�h��Read���Ă����I
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (55:48) Read
				}
			}
			else if( 1 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (63:56)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01F + (0x010 * m_nApxMode);
				if( (0 != m_nSelectCam[m_nSwMode][m_nApxMode] ||
					 0 != m_Param.linksw[m_nSwMode].nSelectCam[m_nApxMode]) &&		// �J��������CH�ݒ�L�� ����
					m_nSlaveJoinTorB == m_nSelectTorB[m_nSwMode][m_nApxMode] )		// �r���Q���\���敪��v
				{
					__int64 nVal = (((__int64)m_nSlaveJoinFNo - (__int64)m_nResetFNo) * SIZE_FRAME_IMG_Y);
					nVal >>= (64 - 8 * 1);
					nVal &= 0xFF;
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, nVal);									// �{�[�h �r���Q�����W�X�^ (63:56) Write
				}
				else
				{
					// �r���Q�����Ȃ��{�[�h��Read���Ă����I
					sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (63:56) Read
				}

				// �{�[�h���̃��[�h�ݒ�
				if( m_nApxMode == MAX_CAMLINKSW_OUTPUT-1 )		m_nApxMode = 0;				// �Ō�̃{�[�h��
				else											m_nApxMode++;
			}
		}
		else if( 2 == m_nSubMode )
		{
			LOG(em_MSG), "[%s] <%d> �r���Q���ݒ� �I�� --------------------------------------------------<<<" , my_sThreadName, m_nSwMode);

			// ������ �� �ݒ�҂�
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nApxMode = 0;
			m_nAocMode = 0;
			m_nAocSubMode = 0;
			m_nRegSizeMode = 0;
			m_nSwMode = 0;
			ChangeWaitEventNum( false );													// 1�V�[�P���X���튮��

			// �����Ǘ� �� �J���������N�X�C�b�`�ݒ芮���ʒm
			Send_ToDefect_SlaveJoin();
		}
		else
		{
			LOG(em_ERR), "[%s] <%d> �T�u���[�h�ُ� [mode = �r���Q���ݒ�, sub_mode = %d]", my_sThreadName, m_nSwMode, m_nSubMode);
			syslog(210, "[<%d> mode = �r���Q���ݒ� sub_mode = %d]", m_nSwMode, m_nSubMode);
			ChangeWaitEventNum( false );
			return false;
		}
		break;

//---------------------------------------------------------
	////////////////////////////////////
	// �J�E���^���Z�b�g
	////////////////////////////////////
	case MODE_COUNTER_RESET:
		if( 1 == m_nSubMode &&
			0 == m_nApxMode &&
			0 == m_nAocMode &&
			0 == m_nAocSubMode &&
			0 == m_nRegSizeMode &&
			0 == m_nCamMode )
		{
			LOG(em_MSG), "[%s] <%d> �J�E���^���Z�b�g �J�n -------------------------------------------------->>>", my_sThreadName, m_nSwMode);
			ChangeWaitEventNum( true );
		}

#if 0	// �r���Q���J�E���^�}�j���A�����Z�b�g�̓��삪�s���̂��߃���
		if( 1 == m_nSubMode )
		{
			//// ���W�X�^�T�C�Y�J��Ԃ����[�h�ݒ�
			if( 0 == m_nRegSizeMode )
			{
				LOG(em_MSG), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ [%d]", my_sThreadName, m_nSwMode, m_nApxMode+1, 0);
				m_nRegSizeMode = 8;
			}

			if( 8 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (7:0)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x018 + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (7:0) Write
			}
			else if( 7 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (15:8)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x019 + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (15:8) Write
			}
			else if( 6 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (23:16)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01A + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (23:16) Write
			}
			else if( 5 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (31:24)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01B + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (31:24) Write
			}
			else if( 4 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (39:32)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01C + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (39:32) Write
			}
			else if( 3 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (47:40)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01D + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (47:40) Write
			}
			else if( 2 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (55:48)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01E + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (55:48) Write
			}
			else if( 1 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (63:56)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01F + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (63:56) Write

				// �{�[�h���̃��[�h�ݒ�
				if( m_nApxMode == MAX_CAMLINKSW_OUTPUT-1 )		m_nApxMode = 0;				// �Ō�̃{�[�h��
				else											m_nApxMode++;
			}
		}
		else if( 2 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> �{�[�h%.2d ���[�h���W�X�^ (�r���Q���J�E���^�}�j���A�����Z�b�g) [%d]", my_sThreadName, m_nSwMode, m_nApxMode+1, 64 + 16 + 8 * COUNTER_MODE);

			int nAddr = 0x010 + (0x010 * m_nApxMode);
			sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 64 + 16 + 8 * COUNTER_MODE);						// �{�[�h ���[�h���W�X�^ (�r���Q���J�E���^�}�j���A�����Z�b�g)

			// �{�[�h���̃��[�h�ݒ�
			if( m_nApxMode == MAX_CAMLINKSW_OUTPUT-1 )		m_nApxMode = 0;					// �Ō�̃{�[�h��
			else											m_nApxMode++;
		}
		else if( 3 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> �{�[�h%.2d ���[�h���W�X�^ (�r���Q���J�E���^�}�j���A�����Z�b�g����) [%d]", my_sThreadName, m_nSwMode, m_nApxMode+1, 64 + 0 + 8 * COUNTER_MODE);

			int nAddr = 0x010 + (0x010 * m_nApxMode);
			sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 64 + 0 + 8 * COUNTER_MODE);						// �{�[�h ���[�h���W�X�^ (�r���Q���J�E���^�}�j���A�����Z�b�g����)

			// �{�[�h���̃��[�h�ݒ�
			if( m_nApxMode == MAX_CAMLINKSW_OUTPUT-1 )		m_nApxMode = 0;					// �Ō�̃{�[�h��
			else											m_nApxMode++;
		}
		else if( 4 == m_nSubMode )
#else
		if( 1 == m_nSubMode )
		{
			//// ���W�X�^�T�C�Y�J��Ԃ����[�h�ݒ�
			if( 0 == m_nRegSizeMode )
			{
				LOG(em_MSG), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ [%d]", my_sThreadName, m_nSwMode, m_nApxMode+1, 0);
				m_nRegSizeMode = 8;
			}

			if( 8 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (7:0)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x018 + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (7:0) Write
			}
			else if( 7 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (15:8)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x019 + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (15:8) Write
			}
			else if( 6 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (23:16)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01A + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (23:16) Write
			}
			else if( 5 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (31:24)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01B + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (31:24) Write
			}
			else if( 4 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (39:32)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01C + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (39:32) Write
			}
			else if( 3 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (47:40)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01D + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (47:40) Write
			}
			else if( 2 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (55:48)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01E + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (55:48) Write
			}
			else if( 1 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (63:56)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01F + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0);											// �{�[�h �r���Q�����W�X�^ (63:56) Write

				// �{�[�h���̃��[�h�ݒ�
				if( m_nApxMode == MAX_CAMLINKSW_OUTPUT-1 )		m_nApxMode = 0;				// �Ō�̃{�[�h��
				else											m_nApxMode++;
			}
		}
		else if( 2 == m_nSubMode )
#endif
		{
			LOG(em_MSG), "[%s] <%d> �J�E���^���Z�b�g �I�� --------------------------------------------------<<<" , my_sThreadName, m_nSwMode);

			// ������ �� �ݒ�҂�
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nApxMode = 0;
			m_nAocMode = 0;
			m_nAocSubMode = 0;
			m_nRegSizeMode = 0;
			m_nSwMode = 0;
			ChangeWaitEventNum( false );													// 1�V�[�P���X���튮��
		}
		else
		{
			LOG(em_ERR), "[%s] <%d> �T�u���[�h�ُ� [mode = �J�E���^���Z�b�g, sub_mode = %d]", my_sThreadName, m_nSwMode, m_nSubMode);
			syslog(210, "[<%d> mode = �J�E���^���Z�b�g sub_mode = %d]", m_nSwMode, m_nSubMode);
			ChangeWaitEventNum( false );
			return false;
		}
		break;

//---------------------------------------------------------
	////////////////////////////////////
	// ������`�F�b�N
	////////////////////////////////////
	case MODE_CYCLE_CHECK:
		if( 1 == m_nSubMode &&
			0 == m_nApxMode &&
			0 == m_nAocMode &&
			0 == m_nAocSubMode &&
			0 == m_nRegSizeMode &&
			0 == m_nCamMode )
		{
			LOG(em_MSG), "[%s] <%d> ������`�F�b�N �J�n -------------------------------------------------->>>", my_sThreadName, m_nSwMode);
			ChangeWaitEventNum( true );
		}

		if( 1 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> ���͉��x���W�X�^", my_sThreadName, m_nSwMode);

			sprintf(m_cSendBuf, "@%02XR003\r", m_nSwMode);																// ���͉��x���W�X�^
		}
		else if( 2 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> FPGA�_�C���x���W�X�^", my_sThreadName, m_nSwMode);

			sprintf(m_cSendBuf, "@%02XR004\r", m_nSwMode);																// FPGA�_�C���x���W�X�^
		}
		else if( 3 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> �J����%.2d Opt-C:Link B   Ctrl/Status ���W�X�^", my_sThreadName, m_nSwMode, m_nCamMode+1);

			int nAddr = 0x100 + (0x010 * m_nCamMode);
			sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);														// Opt-C:Link B   Ctrl/Status ���W�X�^

			// �J�������̃��[�h�ݒ�
			if( m_nCamMode == MAX_CAMLINKSW_ALLINPUT-1 )	m_nCamMode = 0;					// �Ō�̃J������
			else											m_nCamMode++;
		}
		else if( 4 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> �J����%.2d Opt-C:Link M/F Ctrl/Status ���W�X�^", my_sThreadName, m_nSwMode, m_nCamMode+1);

			int nAddr = 0x101 + (0x010 * m_nCamMode);
			sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);														// Opt-C:Link M/F Ctrl/Status ���W�X�^

			// �J�������̃��[�h�ݒ�
			if( m_nCamMode == MAX_CAMLINKSW_ALLINPUT-1 )	m_nCamMode = 0;					// �Ō�̃J������
			else											m_nCamMode++;
		}
		else if( 5 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> �J����%.2d Opt-C:Link B   Ctrl/Status ���W�X�^ CRC�G���[���Z�b�g", my_sThreadName, m_nSwMode, m_nCamMode+1);

			int nAddr = 0x100 + (0x010 * m_nCamMode);
			sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 2);

			// �J�������̃��[�h�ݒ�
			if( m_nCamMode == MAX_CAMLINKSW_ALLINPUT-1 )	m_nCamMode = 0;					// �Ō�̃J������
			else											m_nCamMode++;
		}
		else if( 6 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> �J����%.2d Opt-C:Link M/F Ctrl/Status ���W�X�^ CRC�G���[���Z�b�g", my_sThreadName, m_nSwMode, m_nCamMode+1);

			int nAddr = 0x101 + (0x010 * m_nCamMode);
			sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 2);

			// �J�������̃��[�h�ݒ�
			if( m_nCamMode == MAX_CAMLINKSW_ALLINPUT-1 )	m_nCamMode = 0;					// �Ō�̃J������
			else											m_nCamMode++;
		}
		else if( 7 == m_nSubMode )
		{
			for( int ii=0; ii<NUM_CAMLINKSW; ii++ )
			{
				//// �}�X�^�[�� (ID������)
				for( int jj=0; jj<MAX_CAMLINKSW_INPUT; jj++ )
				{
					//// B
					LOG(em_MSG), "[%s] <%d> �J����%.2d [Opt-C:Link B   Status=%d]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA, jj+1, m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0]);
					// Opt-C;Link�X�e�[�^�X�`�F�b�N
					if( 0 != m_nOptCLinkStID[ii*MAX_CAMLINKSW_FPGA][jj][0] )
					{
						if( (    0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] & 8)			// RxStatus�G���[
							  || 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] & 4)			// LinkStatus�G���[
							  || 0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] & 2)			// CRC�G���[
							) && m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] )
						{
							// ���� �� �ُ�
							KizuFunction::SetStatus(m_nOptCLinkStID[ii*MAX_CAMLINKSW_FPGA][jj][0], false);
							m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] = false;
						}
						else if( (    0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] & 8)	// RxStatus�G���[
								   && 0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] & 4)	// LinkStatus�G���[
								   && 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] & 2)	// CRC�G���[
								 ) && ! m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] )
						{
							// �ُ� �� ����
							KizuFunction::SetStatus(m_nOptCLinkStID[ii*MAX_CAMLINKSW_FPGA][jj][0], true);
							m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] = true;
						}

						// RxStatus�G���[ (��M�������W���[���̎�����)
						if( 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] & 8) &&
							m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA][jj][0] )
						{
							LOG(em_ERR), "[%s] <%d> �J����%.2d [Opt-C:Link B   RxStatus�G���[]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA, jj+1);
							syslog(213, "[<%d> Ch%d B]", ii*MAX_CAMLINKSW_FPGA, jj+1);
							m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA][jj][0] = FALSE;
						}
						else if( ! m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA][jj][0] )
						{
							m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA][jj][0] = TRUE;
						}

						// LinkStatus�G���[ (�����N���)
						if( 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] & 4) &&
							m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] )
						{
							LOG(em_ERR), "[%s] <%d> �J����%.2d [Opt-C:Link B   LinkStatus�G���[]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA, jj+1);
							syslog(214, "[<%d> Ch%d B]", ii*MAX_CAMLINKSW_FPGA, jj+1);
							m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] = FALSE;
						}
						else if( ! m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] )
						{
							m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] = TRUE;
						}

						// CRC�G���[
						if( 0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][0] & 2) )
						{
							LOG(em_WAR), "[%s] <%d> �J����%.2d [Opt-C:Link B   CRC�G���[]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA, jj+1);
							syslog(215, "[<%d> Ch%d B]", ii*MAX_CAMLINKSW_FPGA, jj+1);
						}
					}

					//// M/F
					LOG(em_MSG), "[%s] <%d> �J����%.2d [Opt-C:Link M/F Status=%d]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA, jj+1, m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1]);
					// Opt-C;Link�X�e�[�^�X�`�F�b�N
					if( 0 != m_nOptCLinkStID[ii*MAX_CAMLINKSW_FPGA][jj][1] )
					{
						if( (    0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] & 8)			// RxStatus�G���[
							  || 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] & 4)			// LinkStatus�G���[
							  || 0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] & 2)			// CRC�G���[
							) && m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] )
						{
							// ���� �� �ُ�
							KizuFunction::SetStatus(m_nOptCLinkStID[ii*MAX_CAMLINKSW_FPGA][jj][1], false);
							m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] = false;
						}
						else if( (    0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] & 8)	// RxStatus�G���[
								   && 0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] & 4)	// LinkStatus�G���[
								   && 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] & 2)	// CRC�G���[
								 ) && ! m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] )
						{
							// �ُ� �� ����
							KizuFunction::SetStatus(m_nOptCLinkStID[ii*MAX_CAMLINKSW_FPGA][jj][1], true);
							m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] = true;
						}

						// RxStatus�G���[ (��M�������W���[���̎�����)
						if( 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] & 8) &&
							m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA][jj][1] )
						{
							LOG(em_ERR), "[%s] <%d> �J����%.2d [Opt-C:Link M/F RxStatus�G���[]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA, jj+1);
							syslog(213, "[<%d> Ch%d M/F]", ii*MAX_CAMLINKSW_FPGA, jj+1);
							m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA][jj][1] = FALSE;
						}
						else if( ! m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA][jj][1] )
						{
							m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA][jj][1] = TRUE;
						}

						// LinkStatus�G���[ (�����N���)
						if( 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] & 4) &&
							m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] )
						{
							LOG(em_ERR), "[%s] <%d> �J����%.2d [Opt-C:Link M/F LinkStatus�G���[]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA, jj+1);
							syslog(214, "[<%d> Ch%d M/F]", ii*MAX_CAMLINKSW_FPGA, jj+1);
							m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] = FALSE;
						}
						else if( ! m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] )
						{
							m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] = TRUE;
						}

						// CRC�G���[
						if( 0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA][jj][1] & 2) )
						{
							LOG(em_WAR), "[%s] <%d> �J����%.2d [Opt-C:Link M/F CRC�G���[]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA, jj+1);
							syslog(215, "[<%d> Ch%d M/F]", ii*MAX_CAMLINKSW_FPGA, jj+1);
						}
					}
				}

				//// �X���[�u�� (ID���)
				for( int jj=MAX_CAMLINKSW_INPUT; jj<MAX_CAMLINKSW_ALLINPUT; jj++ )
				{
					//// B
					LOG(em_MSG), "[%s] <%d> �J����%.2d [Opt-C:Link B   Status=%d]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA+1, jj+1, m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0]);
					// Opt-C;Link�X�e�[�^�X�`�F�b�N
					if( 0 != m_nOptCLinkStID[ii*MAX_CAMLINKSW_FPGA+1][jj-MAX_CAMLINKSW_INPUT][0] )
					{
						if( (    0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] & 8)		// RxStatus�G���[
							  || 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] & 4)		// LinkStatus�G���[
							  || 0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] & 2)		// CRC�G���[
							) && m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj-MAX_CAMLINKSW_INPUT][0] )
						{
							// ���� �� �ُ�
							KizuFunction::SetStatus(m_nOptCLinkStID[ii*MAX_CAMLINKSW_FPGA+1][jj-MAX_CAMLINKSW_INPUT][0], false);
							m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj-MAX_CAMLINKSW_INPUT][0] = false;
						}
						else if( (    0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] & 8)	// RxStatus�G���[
								   && 0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] & 4)	// LinkStatus�G���[
								   && 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] & 2)	// CRC�G���[
								 ) && ! m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj-MAX_CAMLINKSW_INPUT][0] )
						{
							// �ُ� �� ����
							KizuFunction::SetStatus(m_nOptCLinkStID[ii*MAX_CAMLINKSW_FPGA+1][jj-MAX_CAMLINKSW_INPUT][0], true);
							m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj-MAX_CAMLINKSW_INPUT][0] = true;
						}

						// RxStatus�G���[ (��M�������W���[���̎�����)
						if( 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] & 8) &&
							m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] )
						{
							LOG(em_ERR), "[%s] <%d> �J����%.2d [Opt-C:Link B   RxStatus�G���[]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA+1, jj+1);
							syslog(213, "[<%d> Ch%d B]", ii*MAX_CAMLINKSW_FPGA+1, jj+1);
							m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] = FALSE;
						}
						else if( ! m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] )
						{
							m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] = TRUE;
						}

						// LinkStatus�G���[ (�����N���)
						if( 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] & 4) &&
							m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] )
						{
							LOG(em_ERR), "[%s] <%d> �J����%.2d [Opt-C:Link B   LinkStatus�G���[]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA+1, jj+1);
							syslog(214, "[<%d> Ch%d B]", ii*MAX_CAMLINKSW_FPGA+1, jj+1);
							m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] = FALSE;
						}
						else if( ! m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] )
						{
							m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] = TRUE;
						}

						// CRC�G���[
						if( 0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][0] & 2) )
						{
							LOG(em_WAR), "[%s] <%d> �J����%.2d [Opt-C:Link B   CRC�G���[]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA+1, jj+1);
							syslog(215, "[<%d> Ch%d B]", ii*MAX_CAMLINKSW_FPGA+1, jj+1);
						}
					}

					//// M/F
					LOG(em_MSG), "[%s] <%d> �J����%.2d [Opt-C:Link M/F Status=%d]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA+1, jj+1, m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1]);
					// Opt-C;Link�X�e�[�^�X�`�F�b�N
					if( 0 != m_nOptCLinkStID[ii*MAX_CAMLINKSW_FPGA+1][jj-MAX_CAMLINKSW_INPUT][1] )
					{
						if( (    0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] & 8)		// RxStatus�G���[
							  || 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] & 4)		// LinkStatus�G���[
							  || 0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] & 2)		// CRC�G���[
							) && m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj-MAX_CAMLINKSW_INPUT][1] )
						{
							// ���� �� �ُ�
							KizuFunction::SetStatus(m_nOptCLinkStID[ii*MAX_CAMLINKSW_FPGA+1][jj-MAX_CAMLINKSW_INPUT][1], false);
							m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj-MAX_CAMLINKSW_INPUT][1] = false;
						}
						else if( (    0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] & 8)	// RxStatus�G���[
								   && 0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] & 4)	// LinkStatus�G���[
								   && 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] & 2)	// CRC�G���[
								 ) && ! m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj-MAX_CAMLINKSW_INPUT][1] )
						{
							// �ُ� �� ����
							KizuFunction::SetStatus(m_nOptCLinkStID[ii*MAX_CAMLINKSW_FPGA+1][jj-MAX_CAMLINKSW_INPUT][1], true);
							m_bOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj-MAX_CAMLINKSW_INPUT][1] = true;
						}

						// RxStatus�G���[ (��M�������W���[���̎�����)
						if( 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] & 8) &&
							m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] )
						{
							LOG(em_ERR), "[%s] <%d> �J����%.2d [Opt-C:Link M/F RxStatus�G���[]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA+1, jj+1);
							syslog(213, "[<%d> Ch%d M/F]", ii*MAX_CAMLINKSW_FPGA+1, jj+1);
							m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] = FALSE;
						}
						else if( ! m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] )
						{
							m_bOptCLink_RxSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] = TRUE;
						}

						// LinkStatus�G���[ (�����N���)
						if( 0 == (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] & 4) &&
							m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] )
						{
							LOG(em_ERR), "[%s] <%d> �J����%.2d [Opt-C:Link M/F LinkStatus�G���[]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA+1, jj+1);
							syslog(214, "[<%d> Ch%d M/F]", ii*MAX_CAMLINKSW_FPGA+1, jj+1);
							m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] = FALSE;
						}
						else if( ! m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] )
						{
							m_bOptCLink_LinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] = TRUE;
						}

						// CRC�G���[
						if( 0 != (m_nOptCLinkSt[ii*MAX_CAMLINKSW_FPGA+1][jj][1] & 2) )
						{
							LOG(em_WAR), "[%s] <%d> �J����%.2d [Opt-C:Link M/F CRC�G���[]", my_sThreadName, ii*MAX_CAMLINKSW_FPGA+1, jj+1);
							syslog(215, "[<%d> Ch%d M/F]", ii*MAX_CAMLINKSW_FPGA+1, jj+1);
						}
					}
				}
			}

			LOG(em_MSG), "[%s] <%d> ������`�F�b�N �I�� --------------------------------------------------<<<" , my_sThreadName, m_nSwMode);

			// ������`�F�b�N �� �ݒ�҂�
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nApxMode = 0;
			m_nAocMode = 0;
			m_nAocSubMode = 0;
			m_nRegSizeMode = 0;
			m_nSwMode = 0;
			ChangeWaitEventNum( false );													// 1�V�[�P���X���튮��

			// ������`�F�b�N�^�C�} �ĊJ
			LARGE_INTEGER ts;
			ts.QuadPart = (__int64)m_nCycleTime * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
			SetWaitableTimer(m_evTimerCycleCheck, &ts, 0, NULL, NULL, FALSE);
		}
		else
		{
			LOG(em_ERR), "[%s] <%d> �T�u���[�h�ُ� [mode = ������`�F�b�N, sub_mode = %d]", my_sThreadName, m_nSwMode, m_nSubMode);
			syslog(210, "[<%d> mode = ������`�F�b�N sub_mode = %d]", m_nSwMode, m_nSubMode);
			ChangeWaitEventNum( false );
			return false;
		}
		break;

//---------------------------------------------------------
	////////////////////////////////////
	// �J�E���^�`�F�b�N
	////////////////////////////////////
	case MODE_COUNTER_CHECK:
		if( 1 == m_nSubMode &&
			0 == m_nApxMode &&
			0 == m_nAocMode &&
			0 == m_nAocSubMode &&
			0 == m_nRegSizeMode &&
			0 == m_nCamMode )
		{
			LOG(em_MSG), "[%s] <%d> �J�E���^�`�F�b�N �J�n -------------------------------------------------->>>", my_sThreadName, m_nSwMode);
			ChangeWaitEventNum( true );
		}

		if( 1 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> �{�[�h%.2d ���[�h���W�X�^ (�r���Q���J�E���^���[�h�I��)(���ݒlRead) [%d]", my_sThreadName, m_nSwMode, m_nApxMode+1, 0 + 8 * COUNTER_MODE);

			int nAddr = 0x010 + (0x010 * m_nApxMode);
			sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0 + 8 * COUNTER_MODE);							// �{�[�h ���[�h���W�X�^ (�r���Q���J�E���^���[�h�I��, �x���J�E���g�M���I��)

			// �{�[�h���̃��[�h�ݒ�
			if( m_nApxMode == MAX_CAMLINKSW_OUTPUT-1 )		m_nApxMode = 0;					// �Ō�̃{�[�h��
			else											m_nApxMode++;
		}
		else if( 2 == m_nSubMode )
		{
			//// ���W�X�^�T�C�Y�J��Ԃ����[�h�ݒ�
			if( 0 == m_nRegSizeMode )
			{
				LOG(em_MSG), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ [���ݒlRead]", my_sThreadName, m_nSwMode, m_nApxMode+1);
				m_nRegSizeMode = 8;
			}

			if( 8 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (7:0)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x018 + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (7:0) Read
			}
			else if( 7 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (15:8)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x019 + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (15:8) Read
			}
			else if( 6 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (23:16)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01A + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (23:16) Read
			}
			else if( 5 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (31:24)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01B + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (31:24) Read
			}
			else if( 4 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (39:32)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01C + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (39:32) Read
			}
			else if( 3 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (47:40)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01D + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (47:40) Read
			}
			else if( 2 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (55:48)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01E + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (55:48) Read
			}
			else if( 1 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (63:56)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01F + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (63:56) Read

				// �{�[�h���̃��[�h�ݒ�
				if( m_nApxMode == MAX_CAMLINKSW_OUTPUT-1 )		m_nApxMode = 0;				// �Ō�̃{�[�h��
				else											m_nApxMode++;
			}
		}
		else if( 3 == m_nSubMode )
		{
			LOG(em_INF), "[%s] <%d> �{�[�h%.2d ���[�h���W�X�^ (�r���Q���J�E���^���[�h�I��)(�ݒ�lRead) [%d]", my_sThreadName, m_nSwMode, m_nApxMode+1, 64 + 8 * COUNTER_MODE);

			int nAddr = 0x010 + (0x010 * m_nApxMode);
			sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 64 + 8 * COUNTER_MODE);							// �{�[�h ���[�h���W�X�^ (�r���Q���J�E���^���[�h�I��, �x���J�E���g�M���I��)

			// �{�[�h���̃��[�h�ݒ�
			if( m_nApxMode == MAX_CAMLINKSW_OUTPUT-1 )			m_nApxMode = 0;				// �Ō�̃{�[�h��
			else												m_nApxMode++;
		}
		else if( 4 == m_nSubMode )
		{
			//// ���W�X�^�T�C�Y�J��Ԃ����[�h�ݒ�
			if( 0 == m_nRegSizeMode )
			{
				LOG(em_MSG), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ [�ݒ�lRead]", my_sThreadName, m_nSwMode, m_nApxMode+1);
				m_nRegSizeMode = 8;
			}

			if( 8 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (7:0)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x018 + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (7:0) Read
			}
			else if( 7 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (15:8)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x019 + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (15:8) Read
			}
			else if( 6 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (23:16)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01A + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (23:16) Read
			}
			else if( 5 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (31:24)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01B + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (31:24) Read
			}
			else if( 4 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (39:32)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01C + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (39:32) Read
			}
			else if( 3 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (47:40)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01D + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (47:40) Read
			}
			else if( 2 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (55:48)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01E + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (55:48) Read
			}
			else if( 1 == m_nRegSizeMode )
			{
				//LOG(em_INF), "[%s] <%d> �{�[�h%.2d �r���Q�����W�X�^ (63:56)", my_sThreadName, m_nSwMode, m_nApxMode+1);

				int nAddr = 0x01F + (0x010 * m_nApxMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);												// �{�[�h �r���Q�����W�X�^ (63:56) Read

				// �{�[�h���̃��[�h�ݒ�
				if( m_nApxMode == MAX_CAMLINKSW_OUTPUT-1 )		m_nApxMode = 0;				// �Ō�̃{�[�h��
				else											m_nApxMode++;
			}
		}
		else if( 5 == m_nSubMode )
		{
			for( int ii=0; ii<NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; ii++ )
			{
				for( int jj=0; jj<MAX_CAMLINKSW_OUTPUT; jj++ )
				{
					LOG(em_MSG), "[%s] <%d> �{�[�h%.2d [���ݒl=%I64d �ݒ�l=%I64d]", my_sThreadName, ii, jj+1, m_nCounterNow[ii][jj], m_nCounterSet[ii][jj]);
				}
			}

			LOG(em_MSG), "[%s] <%d> �J�E���^�`�F�b�N �I�� --------------------------------------------------<<<" , my_sThreadName, m_nSwMode);

			// ������ �� �ݒ�҂�
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nApxMode = 0;
			m_nAocMode = 0;
			m_nAocSubMode = 0;
			m_nRegSizeMode = 0;
			m_nSwMode = 0;
			ChangeWaitEventNum( false );													// 1�V�[�P���X���튮��
		}
		else
		{
			LOG(em_ERR), "[%s] <%d> �T�u���[�h�ُ� [mode = �J�E���^�`�F�b�N, sub_mode = %d]", my_sThreadName, m_nSwMode, m_nSubMode);
			syslog(210, "[<%d> mode = �J�E���^�`�F�b�N sub_mode = %d]", m_nSwMode, m_nSubMode);
			ChangeWaitEventNum( false );
			return false;
		}
		break;

//---------------------------------------------------------
	case MODE_CHANGE_AOC_SERIALSPEED:
			//// AOC�T�u���[�h�ύX
		//AOC���W�X�^���ʂɕҏW����̂ŁA�ʒu�ɂ���Ă� ���ɂ���submode++ �������Ȃ����Ƃ�����̂ŁB
		if( (0 == m_nSubMode||1 == m_nSubMode  ) &&
			0 == m_nAocSubMode &&
			0 == m_nRegSizeMode &&
			0 == m_nCamMode )
		{
			//int nAOCNo = m_nAocMode * MAX_CAMLINKSW_ALLINPUT + m_nSwMode *MAX_CAMLINKSW_INPUT + m_nApxMode;
			int nAOCNo = m_nAocMode + m_nSwMode *MAX_CAMLINKSW_INPUT;
			LOG(em_MSG), "[%s] <%d> AOC�V���A�����x�ύX �J�n[%d.%d.%d <= %d] -------------------------------------------------->>>",
				my_sThreadName, m_nSwMode, 
				m_nApxMode,				m_nSwMode ,				m_nAocMode,
				m_Param.GetSerialSpeed(nAOCNo) );
			ChangeWaitEventNum( true );
			m_nSubMode = 1;
		}

		if(1 == m_nSubMode )
		{
			if( 0 == m_nRegSizeMode )	m_nAocSubMode++;

			if( 1 == m_nAocSubMode )
			{
				//// ���W�X�^�T�C�Y�J��Ԃ����[�h�ݒ�
				if( 0 == m_nRegSizeMode )	m_nRegSizeMode = 2;

				if( 2 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Address ���W�X�^ (7:0)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x180 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x7C);									// AOC Address ���[�h���W�X�^ (7:0)
				}
				else if( 1 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Address ���W�X�^ (15:8)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x181 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x00);									// AOC Address ���[�h���W�X�^ (15:8)
				}
			}
			else if( 2 == m_nAocSubMode )
			{
				//int nAOCNo =  m_nAocMode * MAX_CAMLINKSW_ALLINPUT + m_nSwMode *MAX_CAMLINKSW_INPUT + m_nApxMode;
				int nAOCNo = m_nAocMode + m_nSwMode *MAX_CAMLINKSW_INPUT;
				//// ���W�X�^�T�C�Y�J��Ԃ����[�h�ݒ�
				if( 0 == m_nRegSizeMode )	m_nRegSizeMode = 4;

				if( 4 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data ���W�X�^ (7:0)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x182 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, LOBYTE(m_Param.nSamplingTiming(nAOCNo )));					// AOC Write Data ���W�X�^ (7:0)
				}
				else if( 3 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data ���W�X�^ (15:8)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x183 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, HIBYTE(m_Param.nSamplingTiming(nAOCNo)));					// AOC Write Data ���W�X�^ (15:8)
				}
				else if( 2 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data ���W�X�^ (23:16)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x184 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, LOBYTE(m_Param.nSamplingDelay(nAOCNo)));									// AOC Write Data ���W�X�^ (23:16)
				}
				else if( 1 == m_nRegSizeMode )
				{
					//LOG(em_INF), "[%s] <%d> AOC%.2d Write Data ���W�X�^ (31:24)", my_sThreadName, m_nSwMode, m_nAocMode+1);

					int nAddr = 0x185 + (0x010 * m_nAocMode);
					sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, HIBYTE(m_Param.nSamplingDelay(nAOCNo)));									// AOC Write Data ���W�X�^ (31:24)
				}
			}
			else if( 3 == m_nAocSubMode )
			{
				LOG(em_INF), "[%s] <%d> AOC%.2d Ctrl ���W�X�^ (Write Access)", my_sThreadName, m_nSwMode, m_nAocMode+1);

				int nAddr = 0x18A + (0x010 * m_nAocMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x11);										// AOC Ctrl ���W�X�^ (Write Access)
			}
			else if( 4 == m_nAocSubMode )
			{
				LOG(em_INF), "[%s] <%d> AOC%.2d Ctrl ���W�X�^ (Read Access)", my_sThreadName, m_nSwMode, m_nAocMode+1);

				int nAddr = 0x18A + (0x010 * m_nAocMode);
				sprintf(m_cSendBuf, "@%02XR%03X\r", m_nSwMode, nAddr);													// AOC Ctrl ���W�X�^ (Read Access)
			}
			else if( 5 == m_nAocSubMode )
			{
				LOG(em_INF), "[%s] <%d> AOC%.2d Ctrl ���W�X�^ (���M�������Z�b�g)", my_sThreadName, m_nSwMode, m_nAocMode+1);

				int nAddr = 0x18A + (0x010 * m_nAocMode);
				sprintf(m_cSendBuf, "@%02XW%03X=%02X\r", m_nSwMode, nAddr, 0x04);										// AOC Ctrl ���W�X�^ (���M�������Z�b�g)
				
				m_nApxMode = 0;
				m_nAocMode = 0;
			}
		}

		else if( 2 == m_nSubMode )
			{
				LOG(em_MSG), "[%s] <%d> AOC�V���A�����x�ύX  �I�� --------------------------------------------------<<<" , my_sThreadName, m_nSwMode);

				// ������ �� �ݒ�҂�
				m_emMainMode = MODE_WAIT;
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nApxMode = 0;
				m_nAocMode = 0;
				m_nAocSubMode = 0;
				m_nRegSizeMode = 0;
				m_nSwMode = 0;
				ChangeWaitEventNum( false );													// 1�V�[�P���X���튮��
		}
		else
		{
			LOG(em_ERR), "[%s] <%d> �T�u���[�h�ُ� [mode = AOC�V���A�����x�ύX , sub_mode = %d]", my_sThreadName, m_nSwMode, m_nSubMode);
			syslog(210, "[<%d> mode =  AOC�V���A�����x�ύX sub_mode = %d]", m_nSwMode, m_nSubMode);
			ChangeWaitEventNum( false );
			return false;
		}
		break;
	break;
//---------------------------------------------------------
	default:
		LOG(em_ERR), "[%s] ���[�h�ُ� [MainMode = %d]", my_sThreadName, m_emMainMode);
		syslog(210, "[mode = %d]", m_emMainMode);
		ChangeWaitEventNum( false );
		return false;
	}

	//// ���M
	if( 0 == strlen(m_cSendBuf) ) return true;												// ���M�f�[�^�����̏ꍇ�́A�����Ԃ��B
	if( ! Send() ) return false;
	return true;
}

//------------------------------------------
// ��M�`���`�F�b�N(AOC MODE)
// �߂�l ���A���
//------------------------------------------
bool LinkswManager::CheckDataRecv_AOC()
{
	// AOC Ctrl ���W�X�^ (Write Access)
	if( 3 == m_nAocSubMode )
	{
		//// ���ɖ���
	}
	// AOC Ctrl ���W�X�^ (Read Access)
	else if( 4 == m_nAocSubMode )
	{
		char cWk[256];
		int nCtrl;
		memset(cWk, 0x00, sizeof(cWk));
		memcpy(cWk, &m_cRecvBuf[10], 2);
		sscanf(cWk, "%X", &nCtrl);
		//// ���[�h�f�[�^ �`�F�b�N
		if( 0x04 != (nCtrl & 0x06) )		// 1bit�ڂ�0�A2bit�ڂ�1�ƂȂ��Ă��邩�`�F�b�N
		{
			// �J��������Ch
			if( m_Param.bCamInputCh[ m_nSwMode ][ m_nAocMode ] )
			{
				LOG(em_ERR), "[%s] <%d> AOC%.2d Ctrl ���W�X�^ �f�[�^���M ���s (%d)", my_sThreadName, m_nSwMode, m_nAocMode+1, m_nAocSubMode);
// --->>>
// �J�����I��ݒ�R�}���h�ŁA���݂��Ȃ��J���� or ���݂��邪���ڑ��̃J������I�������ꍇ�A
// ������(MODE_START_UP)�V�[�P���X�A�����J�n�ݒ�(MODE_START_INIT)�V�[�P���X�𖳌����[�v���Ă��܂����߂̑΍�
//						ChangeWaitEventNum( false );										// 1�V�[�P���X�����I��
//						return false;
// <<<---
			}
			// �J����������Ch
			else
			{
				LOG(em_WAR), "[%s] <%d> AOC%.2d Ctrl ���W�X�^ �f�[�^���M ���s (�J���������̓`�����l���̂��ߖ���) (%d)", my_sThreadName, m_nSwMode, m_nAocMode+1, m_nAocSubMode);
			}
		}
	}
	// AOC Ctrl ���W�X�^ (���M�������Z�b�g)
	else if( 5 == m_nAocSubMode )
	{
		char cWk[256];
		int nCtrl;
		memset(cWk, 0x00, sizeof(cWk));
		memcpy(cWk, &m_cRecvBuf[10], 2);
		sscanf(cWk, "%X", &nCtrl);
		//// ���C�g�f�[�^ �`�F�b�N
		if( 0x00 != (nCtrl & 0x04) )		// 2bit�ڂ�0�ƂȂ��Ă��邩�`�F�b�N
		{
			// �J��������Ch
			if( m_Param.bCamInputCh[ m_nSwMode ][ m_nAocMode ] )
			{
				LOG(em_ERR), "[%s] <%d> AOC%.2d Ctrl ���W�X�^ ���M�������Z�b�g ���s (%d)", my_sThreadName, m_nSwMode, m_nAocMode+1, m_nAocSubMode);
				ChangeWaitEventNum( false );										// 1�V�[�P���X�����I��
				return false;
			}
			// �J����������Ch
			else
			{
				LOG(em_WAR), "[%s] <%d> AOC%.2d Ctrl ���W�X�^ ���M�������Z�b�g ���s (�J���������̓`�����l���̂��ߖ���) (%d)", my_sThreadName, m_nSwMode, m_nAocMode+1, m_nAocSubMode);
			}
		}

		//// AOC�T�u���[�h�N���A
		m_nAocSubMode = 0;
	}
	return true;
}

//------------------------------------------
// ��M�`���`�F�b�N
// �߂�l ���A���
//------------------------------------------
bool LinkswManager::CheckDataRecv()
{
	CString sWk;
	//LOG(em_INF), "[%s] �����`����M [%s]", my_sThreadName, m_cRecvBuf);

	//// �X�C�b�`FPGA�{�[�hID �`�F�b�N
	if( 0 != memcmp(&m_cRecvBuf[0], &m_cSendBuf[1], 2) )
	{
		LOG(em_ERR), "[%s] ��M�`�� �ُ� [�{�[�hID����:send=%s][%s]", my_sThreadName, m_cSendBuf, m_cRecvBuf);
		syslog(211, "[�{�[�hID����:send=%s]", m_cSendBuf);
		ChangeWaitEventNum( false );														// 1�V�[�P���X�����I��
		return false;
	}

	//// �����敪 �`�F�b�N
	if( 0 != memcmp(&m_cRecvBuf[3], R_OK, 2) )
	{
		LOG(em_ERR), "[%s] ��M�`�� �ُ� [�����敪�ُ�:send=%s]", my_sThreadName, m_cSendBuf);
		syslog(211, "[�����敪�ُ�:send=%s]", m_cSendBuf);
		ChangeWaitEventNum( false );														// 1�V�[�P���X�����I��
		return false;
	}

	//// �A�h���X �`�F�b�N
	if( 0 != memcmp(&m_cRecvBuf[6], &m_cSendBuf[4], 3) )
	{
		LOG(em_ERR), "[%s] ��M�`�� �ُ� [�A�h���X����:send=%s]", my_sThreadName, m_cSendBuf);
		syslog(211, "[�A�h���X����:send=%s]", m_cSendBuf);
		ChangeWaitEventNum( false );														// 1�V�[�P���X�����I��
		return false;
	}

	//// ���W�X�^�T�C�Y�J��Ԃ����[�h�ύX
	if( 0 != m_nRegSizeMode )	m_nRegSizeMode--;

	//// ��������
	switch( m_emMainMode )
	{
//---------------------------------------------------------
	////////////////////////////////////
	// �I�[�v���`�F�b�N
	////////////////////////////////////
	case MODE_OPEN_CHECK:
		//// ���[�h�f�[�^ �`�F�b�N
		// �X�C�b�`FPGA �X�e�[�^�X���W�X�^
		if( m_Param.linksw[ m_nSwMode ].bStatus
			? 0 != memcmp(&m_cRecvBuf[10], "01", 2)			// �X�C�b�`�}�X�^
			: 0 != memcmp(&m_cRecvBuf[10], "00", 2) )		// �X�C�b�`�X���[�u
		{
			LOG(em_ERR), "[%s] <%d> ��M�`�� �ُ� [�X�e�[�^�X���W�X�^����:DB(%s)��REG(%s)]",
				my_sThreadName, m_nSwMode,
				m_Param.linksw[ m_nSwMode ].bStatus ? "�X�C�b�`�}�X�^" : "�X�C�b�`�X���[�u",
				m_Param.linksw[ m_nSwMode ].bStatus ? "�X�C�b�`�X���[�u" : "�X�C�b�`�}�X�^");
			ChangeWaitEventNum( false );													// 1�V�[�P���X�����I��
			return false;
		}

		// �X�C�b�`FPGA���̃��[�h�ݒ�
		if( m_nSwMode != NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA-1 )								// �Ō�̃X�C�b�`FPGA�ȊO��
		{
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nApxMode = 0;
			m_nAocMode = 0;
			m_nAocSubMode = 0;
			m_nRegSizeMode = 0;
			m_nSwMode++;

			// ���̑��M�f�[�^���� & ���M
			if( ! CheckNextSend() )
			{
				return false;
			}
			break;
		}

		// �ڑ�����
		LOG(em_MSG), "[%s] �J���������N�X�C�b�`��H �ڑ��m�F���� --------------------------------------------------<<<", my_sThreadName);
		syslog(212, "");
		m_bIsConnect = true;
		//sWk.Format("CAMSWITCH_ID_%d",m_nUnit+1);
		//KizuFunction::SetStatus(sWk, true);
		if (true) {
			CString sWk;
			sWk.Format("CAMSWITCH_ID_%d", m_nUnit + 1);
			int id = GetPrivateProfileInt(INI_STATUS, sWk, -1, TASKINI_NAME);
			if (-1 == id) {
				KizuFunction::SetStatus("CAMSWITCH_ID", true);
			}
			else {
				KizuFunction::SetStatus(sWk, true);
			}
		}

		// ������`�F�b�N�^�C�} �J�n
		LARGE_INTEGER ts;
		ts.QuadPart = (__int64)m_nCycleTime * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
		SetWaitableTimer(m_evTimerCycleCheck, &ts, 0, NULL, NULL, FALSE);

		// �I�[�v���`�F�b�N �� ������
		m_emMainMode = MODE_START_UP;
		m_nSubMode = 0;
		m_nCamMode = 0;
		m_nApxMode = 0;
		m_nAocMode = 0;
		m_nAocSubMode = 0;
		m_nRegSizeMode = 0;
		m_nSwMode = 0;
		if( ! CheckNextSend() )
		{
			return false;
		}
		break;

//---------------------------------------------------------
	////////////////////////////////////
	// ������
	////////////////////////////////////
	case MODE_START_UP:
		if( 7 == m_nSubMode ||
			8 == m_nSubMode ||
			9 == m_nSubMode )
		{
			if( !CheckDataRecv_AOC() )
						return false;
					}
		else
		{
			//// ���C�g�f�[�^ �`�F�b�N
			if( 0 != memcmp(&m_cRecvBuf[10], &m_cSendBuf[8], 2) )
			{
				LOG(em_ERR), "[%s] <%d> ��M�`�� �ُ� [���C�g�f�[�^����:���M(%.2s)����M(%.2s)]",
					my_sThreadName, m_nSwMode, &m_cSendBuf[8], &m_cRecvBuf[10]);
				ChangeWaitEventNum( false );												// 1�V�[�P���X�����I��
				return false;
			}
		}

		// �X�C�b�`FPGA���̃��[�h�ݒ�
		if( 10 == m_nSubMode &&
			0 == m_nAocMode )																// �Ō��AOC��
		{
			if( m_nSwMode != NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA-1 )							// �Ō�̃X�C�b�`FPGA�ȊO��
			{
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nApxMode = 0;
				m_nAocMode = 0;
				m_nAocSubMode = 0;
				m_nRegSizeMode = 0;
				m_nSwMode++;
			}
		}

		// ���̑��M�f�[�^���� & ���M
		if( ! CheckNextSend() )
		{
			return false;
		}
		break;

//---------------------------------------------------------
	////////////////////////////////////
	// �����J�n�ݒ�
	////////////////////////////////////
	case MODE_START_INIT:
		//// ���C�g�f�[�^ �`�F�b�N
		if( 0 != memcmp(&m_cRecvBuf[10], &m_cSendBuf[8], 2) )
		{
			LOG(em_ERR), "[%s] <%d> ��M�`�� �ُ� [���C�g�f�[�^����:���M(%.2s)����M(%.2s)]",
				my_sThreadName, m_nSwMode, &m_cSendBuf[8], &m_cRecvBuf[10]);
			ChangeWaitEventNum( false );													// 1�V�[�P���X�����I��
			return false;
		}

		// �X�C�b�`FPGA���̃��[�h�ݒ�
		if( 4 == m_nSubMode &&
			0 == m_nAocMode )																// �Ō��AOC��
		{
			if( m_nSwMode != NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA-1 )							// �Ō�̃X�C�b�`FPGA�ȊO��
			{
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nApxMode = 0;
				m_nAocMode = 0;
				m_nAocSubMode = 0;
				m_nRegSizeMode = 0;
				m_nSwMode++;
			}	
		}

		// ���̑��M�f�[�^���� & ���M
		if( ! CheckNextSend() )
		{
			return false;
		}
		break;

// 2020.05.07 �ΏۃJ�����̂ݐݒ� ---------->>>>
//---------------------------------------------------------
////////////////////////////////////
// �����J�n�ݒ�(�J�����I��ݒ�)
////////////////////////////////////
	case MODE_START_SELECT_INIT:
		//// ���C�g�f�[�^ �`�F�b�N
		if (0 == memcmp("=", &m_cSendBuf[7], 1) &&
			0 != memcmp(&m_cRecvBuf[10], &m_cSendBuf[8], 2))
		{
			LOG(em_ERR), "[%s] <%d> ��M�`�� �ُ� [���C�g�f�[�^����:���M(%.2s)����M(%.2s)]",
				my_sThreadName, m_nSwMode, &m_cSendBuf[8], &m_cRecvBuf[10]);
				ChangeWaitEventNum(false);													// 1�V�[�P���X�����I��
				return false;
		}

		// �X�C�b�`FPGA���̃��[�h�ݒ�
		if (2 == m_nSubMode &&
			0 == m_nRegSizeMode &&
			0 == m_nApxMode)																// �Ō�̃{�[�h��
		{
			if (m_nSwMode != NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA - 1)							// �Ō�̃X�C�b�`FPGA�ȊO��
			{
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nApxMode = 0;
				m_nAocMode = 0;
				m_nAocSubMode = 0;
				m_nRegSizeMode = 0;
				m_nSwMode++;
			}
		}

		// ���̑��M�f�[�^���� & ���M
		if (!CheckNextSend())
		{
			return false;
		}
		break;

// 2020.05.07 �ΏۃJ�����̂ݐݒ� <<<<----------

//---------------------------------------------------------
	////////////////////////////////////
	// �r���Q���ݒ�
	////////////////////////////////////
	case MODE_SLAVE_JOIN:
		//// ���C�g�f�[�^ �`�F�b�N
		if( 0 == memcmp("=",             &m_cSendBuf[7], 1) &&
			0 != memcmp(&m_cRecvBuf[10], &m_cSendBuf[8], 2) )
		{
			LOG(em_ERR), "[%s] <%d> ��M�`�� �ُ� [���C�g�f�[�^����:���M(%.2s)����M(%.2s)]",
				my_sThreadName, m_nSwMode, &m_cSendBuf[8], &m_cRecvBuf[10]);
			ChangeWaitEventNum( false );													// 1�V�[�P���X�����I��
			return false;
		}

		// �X�C�b�`FPGA���̃��[�h�ݒ�
		if( 1 == m_nSubMode &&
			0 == m_nRegSizeMode &&
			0 == m_nApxMode )																// �Ō�̃{�[�h��
		{
			if( m_nSwMode != NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA-1 )							// �Ō�̃X�C�b�`FPGA�ȊO��
			{
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nApxMode = 0;
				m_nAocMode = 0;
				m_nAocSubMode = 0;
				m_nRegSizeMode = 0;
				m_nSwMode++;
			}
		}

		// ���̑��M�f�[�^���� & ���M
		if( ! CheckNextSend() )
		{
			return false;
		}
		break;

//---------------------------------------------------------
	////////////////////////////////////
	// �J�E���^���Z�b�g
	////////////////////////////////////
	case MODE_COUNTER_RESET:
		//// ���C�g�f�[�^ �`�F�b�N
		if( 0 != memcmp(&m_cRecvBuf[10], &m_cSendBuf[8], 2) )
		{
			LOG(em_ERR), "[%s] <%d> ��M�`�� �ُ� [���C�g�f�[�^����:���M(%.2s)����M(%.2s)]",
				my_sThreadName, m_nSwMode, &m_cSendBuf[8], &m_cRecvBuf[10]);
			ChangeWaitEventNum( false );													// 1�V�[�P���X�����I��
			return false;
		}

#if 0	// �r���Q���J�E���^�}�j���A�����Z�b�g�̓��삪�s���̂��߃���
		// �X�C�b�`FPGA���̃��[�h�ݒ�
		if( 3 == m_nSubMode &&
			0 == m_nApxMode )																// �Ō�̃{�[�h��
		{
			if( m_nSwMode != NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA-1 )							// �Ō�̃X�C�b�`FPGA�ȊO��
			{
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nApxMode = 0;
				m_nAocMode = 0;
				m_nAocSubMode = 0;
				m_nRegSizeMode = 0;
				m_nSwMode++;
			}
		}
#else
		// �X�C�b�`FPGA���̃��[�h�ݒ�
		if( 1 == m_nSubMode &&
			0 == m_nRegSizeMode &&
			0 == m_nApxMode )																// �Ō�̃��[�h��
		{
			if( m_nSwMode != NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA-1 )							// �Ō�̃X�C�b�`FPGA�ȊO��
			{
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nApxMode = 0;
				m_nAocMode = 0;
				m_nAocSubMode = 0;
				m_nRegSizeMode = 0;
				m_nSwMode++;
			}
		}
#endif

		// ���̑��M�f�[�^���� & ���M
		if( ! CheckNextSend() )
		{
			return false;
		}
		break;

//---------------------------------------------------------
	////////////////////////////////////
	// ������`�F�b�N
	////////////////////////////////////
	case MODE_CYCLE_CHECK:
		if( 1 == m_nSubMode )
		{
			//// ���͉��x �`�F�b�N
			char cWk[256];
			int nTemp;		// ���x [��]
			memset(cWk, 0x00, sizeof(cWk));
			memcpy(cWk, &m_cRecvBuf[10], 2);
			sscanf(cWk, "%X", &nTemp);
			LOG(em_INF), "[%s] <%d> ���͉��x [%d��]", my_sThreadName, m_nSwMode, nTemp);

			// ���x�ۑ�
			if( nTemp != m_nAmbTemp[m_nSwMode] )
			{
				// ���x�ω����̂�
				m_nAmbTemp[m_nSwMode] = nTemp;
				if( 0 != m_nAmbTempID[m_nSwMode] )		KizuFunction::SetStatus(m_nAmbTempID[m_nSwMode], nTemp, false);
			}
			// ���x�`�F�b�N
			bool bLimitOk = true;
			if( 0 != m_nAmbTempLimit[m_nSwMode] && m_nAmbTempLimit[m_nSwMode] <= nTemp ) bLimitOk = false;
			if( ! bLimitOk && m_bAmbTempSt[m_nSwMode] )
			{
				// ���� �� �ُ�
				if( 0 != m_nAmbTempStID[m_nSwMode] )	KizuFunction::SetStatus(m_nAmbTempStID[m_nSwMode], false);
			}
			else if( bLimitOk && ! m_bAmbTempSt[m_nSwMode] )
			{
				// �ُ� �� ����
				if( 0 != m_nAmbTempStID[m_nSwMode] )	KizuFunction::SetStatus(m_nAmbTempStID[m_nSwMode], true);
			}
			m_bAmbTempSt[m_nSwMode] = bLimitOk;
		}
		else if( 2 == m_nSubMode )
		{
			//// FPGA�_�C���x �`�F�b�N
			char cWk[256];
			int nTemp;		// ���x [��]
			memset(cWk, 0x00, sizeof(cWk));
			memcpy(cWk, &m_cRecvBuf[10], 2);
			sscanf(cWk, "%X", &nTemp);
			LOG(em_INF), "[%s] <%d> FPGA�_�C���x [%d��]", my_sThreadName, m_nSwMode, nTemp);

			// ���x�ۑ�
			if( nTemp != m_nDieTemp[m_nSwMode] )
			{
				// ���x�ω����̂�
				m_nDieTemp[m_nSwMode] = nTemp;
				if( 0 != m_nDieTempID[m_nSwMode] )		KizuFunction::SetStatus(m_nDieTempID[m_nSwMode], nTemp, false);
			}
			// ���x�`�F�b�N
			bool bLimitOk = true;
			if( 0 != m_nDieTempLimit[m_nSwMode] && m_nDieTempLimit[m_nSwMode] <= nTemp ) bLimitOk = false;
			if( ! bLimitOk && m_bDieTempSt[m_nSwMode] )
			{
				// ���� �� �ُ�
				if( 0 != m_nDieTempStID[m_nSwMode] )	KizuFunction::SetStatus(m_nDieTempStID[m_nSwMode], false);
			}
			else if( bLimitOk && ! m_bDieTempSt[m_nSwMode] )
			{
				// �ُ� �� ����
				if( 0 != m_nDieTempStID[m_nSwMode] )	KizuFunction::SetStatus(m_nDieTempStID[m_nSwMode], true);
			}
			m_bDieTempSt[m_nSwMode] = bLimitOk;
		}
		else if( 3 == m_nSubMode ||
				 4 == m_nSubMode )
		{
			//// Opt-C:Link�X�e�[�^�X �`�F�b�N
			char cWk[256];
			BYTE nVal;
			memset(cWk, 0x00, sizeof(cWk));
			memcpy(cWk, &m_cRecvBuf[10], 2);
			nVal = (BYTE)strtol(cWk, NULL, 16);

			// ���W�X�^�l�ۑ�
			if( 3 == m_nSubMode )	m_nOptCLinkSt[m_nSwMode][0 != m_nCamMode ? m_nCamMode - 1 : MAX_CAMLINKSW_ALLINPUT - 1][0] = nVal;	// B
			else					m_nOptCLinkSt[m_nSwMode][0 != m_nCamMode ? m_nCamMode - 1 : MAX_CAMLINKSW_ALLINPUT - 1][1] = nVal;	// M/F
		}
		//else
		//{
		//	//// ���C�g�f�[�^ �`�F�b�N
		//	if( 0 != memcmp(&m_cRecvBuf[10], &m_cSendBuf[8], 2) )
		//	{
		//		LOG(em_ERR), "[%s] <%d> ��M�`�� �ُ� [���C�g�f�[�^����:���M(%.2s)����M(%.2s)]",
		//			my_sThreadName, m_nSwMode, &m_cSendBuf[8], &m_cRecvBuf[10]);
		//		ChangeWaitEventNum( false );												// 1�V�[�P���X�����I��
		//		return false;
		//	}
		//}

		// �X�C�b�`FPGA���̃��[�h�ݒ�
		if( 6 == m_nSubMode &&
			0 == m_nCamMode )																// �Ō�̃J������
		{
			if( m_nSwMode != NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA-1 )							// �Ō�̃X�C�b�`FPGA�ȊO��
			{
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nApxMode = 0;
				m_nAocMode = 0;
				m_nAocSubMode = 0;
				m_nRegSizeMode = 0;
				m_nSwMode++;
			}
		}

		// ���̑��M�f�[�^���� & ���M
		if( ! CheckNextSend() )
		{
			return false;
		}
		break;

//---------------------------------------------------------
	////////////////////////////////////
	// �J�E���^�`�F�b�N
	////////////////////////////////////
	case MODE_COUNTER_CHECK:
		if( 2 == m_nSubMode ||
			4 == m_nSubMode )
		{
			//// �r���Q���J�E���^ �`�F�b�N
			char cWk[256];
			__int64	nVal;
			memset(cWk, 0x00, sizeof(cWk));
			memcpy(cWk, &m_cRecvBuf[10], 2);
			nVal = strtol(cWk, NULL, 16);
			nVal <<= (64 - 8 * (m_nRegSizeMode + 1));

			// �J�E���^�l�ۑ�
			if( 2 == m_nSubMode )	m_nCounterNow[m_nSwMode][1 != (m_nRegSizeMode + 1) ? m_nApxMode : m_nApxMode - 1] += nVal;			// ���ݒl
			else					m_nCounterSet[m_nSwMode][1 != (m_nRegSizeMode + 1) ? m_nApxMode : m_nApxMode - 1] += nVal;			// �ݒ�l
		}

		// �X�C�b�`FPGA���̃��[�h�ݒ�
		if( 4 == m_nSubMode &&
			0 == m_nRegSizeMode &&
			0 == m_nApxMode )																// �Ō�̃{�[�h��
		{
			if( m_nSwMode != NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA-1 )							// �Ō�̃X�C�b�`FPGA�ȊO��
			{
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nApxMode = 0;
				m_nAocMode = 0;
				m_nAocSubMode = 0;
				m_nRegSizeMode = 0;
				m_nSwMode++;
			}
		}

		// ���̑��M�f�[�^���� & ���M
		if( ! CheckNextSend() )
		{
			return false;
		}
		break;
	case MODE_CHANGE_AOC_SERIALSPEED:
		if( 1 == m_nSubMode ){
			if( !CheckDataRecv_AOC() )
				return false;
		}
		else
		{
			//// ���C�g�f�[�^ �`�F�b�N
			if( 0 != memcmp(&m_cRecvBuf[10], &m_cSendBuf[8], 2) )
			{
				LOG(em_ERR), "[%s] <%d> ��M�`�� �ُ� [���C�g�f�[�^����:���M(%.2s)����M(%.2s)]",
					my_sThreadName, m_nSwMode, &m_cSendBuf[8], &m_cRecvBuf[10]);
				ChangeWaitEventNum( false );												// 1�V�[�P���X�����I��
				return false;
			}
		}

		// �X�C�b�`FPGA���̃��[�h�ݒ�
		if( 10 == m_nSubMode &&
			0 == m_nAocMode )																// �Ō��AOC��
		{
			if( m_nSwMode != NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA-1 )							// �Ō�̃X�C�b�`FPGA�ȊO��
			{
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nApxMode = 0;
				m_nAocMode = 0;
				m_nAocSubMode = 0;
				m_nRegSizeMode = 0;
				m_nSwMode++;
			}
		}

		// ���̑��M�f�[�^���� & ���M
		if( ! CheckNextSend() )
		{
			return false;
		}
		break;
//---------------------------------------------------------
	default:
		LOG(em_ERR), "[%s] ���[�h�ُ� [MainMode = %d]", my_sThreadName, m_emMainMode);
		syslog(210, "[mode = %d]", m_emMainMode);
		ChangeWaitEventNum( false );
		return false;
	}

	return true;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �O���ʒm


//------------------------------------------
// �����Ǘ� �� �J���������N�X�C�b�`�ݒ芮���ʒm
//------------------------------------------
void LinkswManager::Send_ToDefect_StartInit(bool bFlg)
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));

	que.nEventNo	= FACT_TO_DEFECT_30;
	//que.nLineNo = getlineid();
	que.nLineNo		= m_nUnit+1;
	que.fl.data[0]	= bFlg;				// �ڑ�����ُ�t���O

	// ���M
	int nRetc = send_mail(TO_DEFECT, ".", &que);
	if( 0 != nRetc ) syslog(SYSNO_MAIL_SNED_ERR, "[�����Ǘ��ւ̃J���������N�X�C�b�`�ݒ芮���ʒm err_code=%d]", nRetc);
	LOG(em_MSG), "[%s] �����Ǘ��ւ̃J���������N�X�C�b�`�ݒ芮���ʒm [Fact = %d][Flg:%d]", my_sThreadName, que.nEventNo, bFlg);
}

//------------------------------------------
// �����Ǘ� �� �r���Q���ݒ芮���ʒm
//------------------------------------------
void LinkswManager::Send_ToDefect_SlaveJoin()
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));

	que.nEventNo	= FACT_TO_DEFECT_31;
	//que.nLineNo		= getlineid();
	que.nLineNo		= m_nUnit+1;
	que.fl.data[0]	= m_nSlaveJoinTorB;		// �r���Q���\���敪
	que.fl.data[1]	= m_nSlaveJoinFNo;		// �r���Q���t���[��No

	// ���M
	int nRetc = send_mail(TO_DEFECT, ".", &que);
	if( 0 != nRetc ) syslog(SYSNO_MAIL_SNED_ERR, "[�����Ǘ��ւ̓r���Q���ݒ芮���ʒm err_code=%d]", nRetc);
	LOG(em_MSG), "[%s] �����Ǘ��ւ̓r���Q���ݒ芮���ʒm [Fact = %d]", my_sThreadName, que.nEventNo);
}

//------------------------------------------
// �����Ǘ� �� �J�����I��ݒ芮���ʒm
//------------------------------------------
void LinkswManager::Send_ToDefect_SelectInit(bool bFlg)
{
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));

	que.nEventNo = FACT_TO_DEFECT_32;
	//que.nLineNo = getlineid();
	que.nLineNo = m_nUnit + 1;
	memcpy(que.fl.data, m_RecvQue.fl.data, sizeof(que.fl.data));
	que.fl.data[4] = bFlg;				// �ڑ�����ُ�t���O

										// ���M
	int nRetc = send_mail(TO_DEFECT, ".", &que);
	if (0 != nRetc) syslog(SYSNO_MAIL_SNED_ERR, "[�����Ǘ��ւ̃J�����I��ݒ芮���ʒm err_code=%d]", nRetc);
	LOG(em_MSG), "[%s] �����Ǘ��ւ̃J�����I��ݒ芮���ʒm [Fact = %d][Flg:%d]", my_sThreadName, que.nEventNo, bFlg);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �O���A�N�Z�X


//------------------------------------------
// �����[�h���̎擾
// �߂�l ���[�h����
//------------------------------------------
CString LinkswManager::GetMainModeName()
{
	return GetMainModeName(m_emMainMode);
}

//------------------------------------------
// �w�胂�[�h���̎擾
// ENUM_MAINMODE emMainMode ���C�����[�h
// �߂�l ���[�h����
//------------------------------------------
CString LinkswManager::GetMainModeName(ENUM_MAINMODE emMainMode)
{
	CString wk;
	switch( emMainMode )
	{
	case MODE_NONE:
		wk = "�������";
		break;
	case MODE_OPEN_CHECK:
		wk = "�I�[�v���`�F�b�N";
		break;
	case MODE_START_UP:
		wk = "������";
		break;
	case MODE_START_INIT:
		wk = "�����J�n�ݒ�";
		break;
	case MODE_SLAVE_JOIN:
		wk = "�r���Q���ݒ�";
		break;
	case MODE_COUNTER_RESET:
		wk = "�J�E���^���Z�b�g";
		break;
	case MODE_COUNTER_CHECK:
		wk = "�J�E���^�`�F�b�N";
		break;
	case MODE_CYCLE_CHECK:
		wk = "������`�F�b�N";
		break;
	case MODE_WAIT:
		wk = "�ݒ�҂�";
		break;
	case MODE_RETRY:
		wk = "�I�[�v�����g���C��";
		break;
	case MODE_CHANGE_AOC_SERIALSPEED:
		wk = "AOC�V���A�����x�ݒ�";
		break;
// 2020.05.07 �ΏۃJ�����̂ݐݒ� ---------->>>>
	case MODE_START_SELECT_INIT:
		wk = "�����J�n�ݒ�(�J�����I��ݒ�)";
		break;
// 2020.05.07 �ΏۃJ�����̂ݐݒ� <<<<----------
	}
	return wk;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 232C�֘A

//LINKSW���������[�h(115200bps)�ɕύX����B
//Rs232cBase�̃��[�h�iKizuTask.ini�̋L�ځj�ɂ�炸�A9600bps��
//  ���M����K�v������̂ŁA���ڃV���A�����������B
//�܂��A������ύX����ƁA�O���o�[�{�[�h�̃V���A����115200bps�ɂȂ�̂Œ���
bool LinkswManager::DirectSendSpeedChange()
{
	// �ύX����K�v�Ȃ��BLINKSW�����Z�b�g�����9600�ɂȂ�B
	if( LINKW_SERIAL_SPEED == 9600 )
		return true;

	char cComName[256];
	GetPrivateProfileString(m_cMySession, "com", "",	cComName, sizeof(cComName), TASKINI_NAME);
	if (strlen(cComName) == 0) {
		return false;				// COM�|�[�g�w�肪�Ȃ�
	}

	// �ʐM�|�[�g���I�[�v������
	const HANDLE hCom = CreateFile(cComName, GENERIC_READ|GENERIC_WRITE , NULL,
						NULL, OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if( hCom == INVALID_HANDLE_VALUE ) 
		return false;
	{
		DCB dcb = {sizeof(DCB)};
		GetCommState( hCom, &dcb );
		dcb.BaudRate = CBR_9600;
		dcb.fBinary = 1;
		dcb.fDtrControl = DTR_CONTROL_ENABLE;
		dcb.fRtsControl = RTS_CONTROL_ENABLE;
		SetCommState( hCom, &dcb );
	}
	{
		COMMTIMEOUTS timeout = {};
		timeout.WriteTotalTimeoutMultiplier = 1;
		SetCommTimeouts( hCom, &timeout);
	}
	PurgeComm(hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

	// TODO. �f�C�W�[�`�F�[���̏ꍇ�A�J���������N�̑䐔�����A���M���Ȃ��Ƃ��߂����B
	// ���̂Ƃ��́A�����炭�A�`�F�[���̉����ق����瑗�M����K�v������B�B
#if 1
	static const char sCmd[] = "@FFW1D0=04\r" "@FFW1D1=01\r";
	DWORD nSent = 0;
	int r = WriteFile(hCom, &sCmd, sizeof( sCmd ) -1, &nSent, 0 );
	LOG(em_MSG), "[%s] CamLinkSW SpeedUp.(%d) => %d(%d)", my_sThreadName, nSent, r, GetLastError());
#else
	// �z�X�g�A�N�Z�X�V���A���ʐM�{�[���[�g�I�����W�X�^
	// �ݒ�ύX��A1D1h�Ԓn�̃��W�X�^�L�����ɕύX���e�����f
	for (int ii = NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; ii > 0; ii--) {
		char sCmd[1024];
		sprintf(sCmd, "@%02XW1D0=04\r", ii-1);
		DWORD nSent = 0;
		int r = WriteFile(hCom, &sCmd, sizeof(sCmd) - 1, &nSent, 0);
		LOG(em_MSG), "[%s] CamLinkSW SpeedUp. Board:%d Select[%s] (%d) => %d(%d)", my_sThreadName, (ii - 1), sCmd, nSent, r, GetLastError());
	}
	// �z�X�g�A�N�Z�X�V���A���ʐM�{�[���[�g�ݒ背�W�X�^
	for (int ii = NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; ii > 0; ii--) {
		{
			char sCmd[1024];
			sprintf(sCmd, "@%02XW1D1=01\r", ii - 1);
			DWORD nSent = 0;
			int r = WriteFile(hCom, &sCmd, sizeof(sCmd) - 1, &nSent, 0);
			LOG(em_MSG), "[%s] CamLinkSW SpeedUp. Board:%d Set[%s] (%d) => %d(%d)", my_sThreadName, (ii - 1), sCmd, nSent, r, GetLastError());
		}
	}
#endif


	//�߂�͂��Ȃ��\���������̂œǂ܂Ȃ��B
	// (�v�́A���ł�115200bps�ɂȂ��Ă����Ƃ��B)
	CloseHandle( hCom );

	return true;

}
//------------------------------------------
// 232C �I�[�v��
// bool bDspLog false:���O�o�͖���
//------------------------------------------
bool LinkswManager::Open(bool bDspLog)
{
	// Rs232cBase �C���X�^���X����
	try
	{
		if( ! m_bInitialSpeedChange && m_bEnableRapidSerial  )
		{
			if (DirectSendSpeedChange() )
			{
				m_bInitialSpeedChange = true;
			}

		}

		// �J�����R���g���[����PC�Ԃ́A��������q�����Ă��Ȃ��ׁA�\�t�g�t���[����𖳌��Ƃ���
		mcls_p232c = new Rs232cBase(m_cMySession, TASKINI_NAME, GetTid(), PARAM_232C_RECVBUF_SIZE, PARAM_232C_SENDBUF_SIZE, WM_USER, true);
		mcls_p232c->SetRecvEndCR(CR);	// �I�[�R�[�h�`�F�b�N�ɉϒ��ɓǍ��݂Ƃ���
	}
	catch( int code )
	{
		mcls_p232c = NULL;
		if( bDspLog )
		{
			LOG(em_ERR), "[%s] 232C�I�[�v���G���[(err_code = %d)", my_sThreadName, code);
			syslog(SYSNO_232C_NEW_ERR, "[err_code = %d]", code);
		}
		return false;
	}

	// �ڑ�����
	memset(m_cRecvBuf, 0x00, sizeof(m_cRecvBuf));
	memset(m_cSendBuf, 0x00, sizeof(m_cSendBuf));
	//syslog(SYSNO_232C_OPEN, "");

	// �J���������N�X�C�b�`�ƒʐM�ł��邩�m�F
	m_emMainMode = MODE_OPEN_CHECK;
	m_nSubMode = 0;
	m_nCamMode = 0;
	m_nApxMode = 0;
	m_nAocMode = 0;
	m_nAocSubMode = 0;
	m_nRegSizeMode = 0;
	m_nSwMode = 0;
	if( ! CheckNextSend() ) return false;
	return true;
}

//------------------------------------------
// 232C �N���[�Y
//------------------------------------------
void LinkswManager::Close()
{
	// �S�^�C�}��~
	CancelWaitableTimer(m_evReTry);
	CancelWaitableTimer(m_evAnswerTimeOut);
	CancelWaitableTimer(m_evTimerCycleCheck);

	// �N���[�Y
	if( NULL != mcls_p232c )
	{
		SAFE_DELETE(mcls_p232c);
	}

	//// ���g���C�̂��тɃ��O�o�͂����̋֎~
	// ��x�q�������ȍ~����̏o��
	if( m_bIsConnect )
	{
		LOG(em_MSG), "[%s] 232C�N���[�Y", my_sThreadName);
		syslog(SYSNO_232C_CLOSE, "");
		KizuFunction::SetStatus("CAMSWITCH_ID", false);
	}

	// �X�e�[�^�X������
	m_bIsConnect = false;
	m_emMainMode = MODE_RETRY;
	m_nSubMode = 0;
	m_nCamMode = 0;
	m_nApxMode = 0;
	m_nAocMode = 0;
	m_nAocSubMode = 0;
	m_nRegSizeMode = 0;
	m_nSwMode = 0;
	ChangeWaitEventNum( false );

	// ���g���C�^�C�} �Z�b�g
	LARGE_INTEGER ts;
	ts.QuadPart = (__int64)PARAM_TIMEOUT_RETRY * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evReTry, &ts, 0, NULL, NULL, FALSE);
}

//------------------------------------------
// 232C �񓯊� ��M
//------------------------------------------
bool LinkswManager::Recv()
{
	if( NULL == mcls_p232c ) return true;

	// ��M�o�b�t�@�N���A
	memset(m_cRecvBuf, 0x00, sizeof(m_cRecvBuf));

	// �I�[�R�[�h�����ăV�O�i����Ԃ��^�C�v
	int iRetc = mcls_p232c->Recv(m_cRecvBuf, 1, m_evRecvEnd);		// �ϒ��Ǎ��ׁ݂̈A1�o�C�g�Œ�
	if( 0 != iRetc )
	{
		// ��M�G���[
		LOG(em_ERR), "[%s] 232C��M�G���[(err_code = %d)", my_sThreadName, iRetc);
		syslog(SYSNO_232C_RECV_ERR, "[err_code = %d]", iRetc);
		return false;
	}
	return true;
}

//------------------------------------------
// 232C �񓯊� ���M
//------------------------------------------
bool LinkswManager::Send()
{
	if( NULL == mcls_p232c ) return true;

	//// ���M
	LOG(em_INF), "[%s] �R�}���h���M [%s]", my_sThreadName, m_cSendBuf);
	int iRetc = mcls_p232c->Send(m_cSendBuf, (DWORD)strlen(m_cSendBuf), m_evSendEnd);
	if( 0 != iRetc )
	{
		// ��M�G���[
		LOG(em_ERR), "[%s] 232C���M�G���[(err_code = %d)", my_sThreadName, iRetc);
		syslog(SYSNO_232C_SEND_ERR, "[err_code = %d]", iRetc);
		return false;
	}

	//// ���M��
	// �����`���҂��^�C�} �Z�b�g
	LARGE_INTEGER ts;
	ts.QuadPart = (__int64)PARAM_TIMEOUT_ANSWER * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evAnswerTimeOut, &ts, 0, NULL, NULL, FALSE);
	return true;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �p�����[�^�֘A


//------------------------------------------
// ���I�p�����[�^�擾
//------------------------------------------
bool LinkswManager::GetParam()
{
	// �����_�ł͓��I�p�����[�^�͖����I�I
	return true;
}

//------------------------------------------
// �ÓI�p�����[�^�擾
//------------------------------------------
bool LinkswManager::GetParamStatic()
{
	CString sql;					// SQL��
	int nId;
	int nCnt;

	//// ������
	memset(&m_Param, 0x00, sizeof(m_Param));

	//serial�̏����l��9600.
	std::fill_n( m_Param.nSerialSpeeds, _countof(m_Param.nSerialSpeeds), 9600 );

#if 1
	//// ODBC�ڑ��N���X �C���X���^�X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME) )
	{
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	////// ���ʃp�����[�^
	//// SQL���쐬
	sql = "SELECT";
	sql += " �J�����^�b�v��";
#ifdef AOI_HEIGHT
	sql += ", AOIheight";
#endif
	sql += " FROM ";
	sql += DB_PARAM_COMMON;
	//// ���s
	if( ! clsDB.ExecuteDirect(sql) )
	{
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", DB_PARAM_COMMON);
		return false;
	}
	//// �擾
	while( TRUE )
	{
		int sqlrc = clsDB.Fetch();
		if( sqlrc == KIZUODBC_FETCH_OK )						// �f�[�^�L��
		{
			int si = 1;
			clsDB.GetData(si++, &m_Param.nLineBuffer);
#ifdef AOI_HEIGHT
			clsDB.GetData(si++, &m_Param.nYSize);	// AOIheight
#else
			m_Param.nYSize = SIZE_FRAME_IMG_Y;	// �c�t���[���摜�T�C�Y
#endif
		}
		else if( sqlrc == KIZUODBC_FETCH_NODATE )				// �f�[�^����
		{
			break;
		}
		else													// �G���[
		{
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", DB_PARAM_COMMON);
			return false;
		}
	}
#else
	m_Param.nLineBuffer	= 8;
	m_Param.nYSize		= SIZE_FRAME_IMG_Y;
#endif

#if 1		// DB����擾
	////// �J���������N�X�C�b�`�ݒ�p�����[�^
	//// SQL���쐬
	sql = "SELECT";
	sql += " �{�[�hID";
	sql += ", �X�e�[�^�X";
	sql += ", �{�[�h1_�J�����I��";
	sql += ", �{�[�h2_�J�����I��";
	sql += ", �{�[�h3_�J�����I��";
	sql += ", �{�[�h4_�J�����I��";
	sql += ", �{�[�h5_�J�����I��";
	sql += ", �{�[�h6_�J�����I��";
	sql += ", �{�[�h7_�J�����I��";
	sql += ", �{�[�h8_�J�����I��";
	sql += ", �{�[�h9_�J�����I��";
	sql += ", �{�[�h10_�J�����I��";
	sql += ", �{�[�h11_�J�����I��";
	sql += ", �{�[�h12_�J�����I��";
	sql += ", �{�[�h13_�J�����I��";
	sql += ", �{�[�h14_�J�����I��";
	sql += ", �{�[�h15_�J�����I��";
	sql += " FROM ";
	sql += DB_CAMERA_LINKSWITCH;
	sql += " WHERE ";
	sql.AppendFormat(" ID =  %d ", m_nUnit+1);
	sql += " ORDER BY ID ASC";		// ID�̏���
	//// ���s
	if( ! clsDB.ExecuteDirect(sql) )
	{
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", DB_CAMERA_LINKSWITCH);
		return false;
	}
	//// �擾
	nCnt = 0;
	while( TRUE )
	{
		int sqlrc = clsDB.Fetch();
		if( sqlrc == KIZUODBC_FETCH_OK )						// �f�[�^�L��
		{
			// ID
			clsDB.GetData(1, &nId);
			if( 0 > nId || NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA <= nId )
			{
				// �X�C�b�`FPGA�{�[�hID �ُ�
				LOG(em_ERR), "[%s] �X�C�b�`FPGA�{�[�hID �ُ� [ID=%d(ID:0�`%d)]", my_sThreadName, nId, NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA-1);
				return false;
			}

			// �X�e�[�^�X
			clsDB.GetData(2, &m_Param.linksw[nId].bStatus);
			LOG(em_INF), "[%s] �X�C�b�`FPGA�{�[�hID �擾 [ID=%d, �X�C�b�`%s]", my_sThreadName, nId, m_Param.linksw[nId].bStatus ? "�}�X�^" : "�X���[�u");

			// �{�[�h1�`15_�J�����I��
			for( int ii=0; ii<MAX_CAMLINKSW_OUTPUT; ii++ )
			{
				// 2017.02.27 Master/Slave��DB�w�� ------------------------------------------------>>>>
				//clsDB.GetData(3+ii, &m_Param.linksw[nId].nSelectCam[ii]);
				//if( 0 > m_Param.linksw[nId].nSelectCam[ii] || MAX_CAMLINKSW_ALLINPUT < m_Param.linksw[nId].nSelectCam[ii] )
				//{
				//	// �J�����I��ݒ� �ُ�
				//	LOG(em_ERR), "[%s] �J�����I��ݒ�(DB) �ُ� [ID=%d, �{�[�h%d=%d]", my_sThreadName, nId, ii+1, m_Param.linksw[nId].nSelectCam[ii]);
				//	return false;
				//}
				//LOG(em_INF), "[%s] �J�����I��ݒ�(DB) �擾 [ID=%d, �{�[�h%d=%d]", my_sThreadName, nId, ii+1, m_Param.linksw[nId].nSelectCam[ii]);

				int                 nSelectCh   = 0;					// �I��ch
				ENUM_CH_SELECT_MODE emChSelMode = CH_SELECT_NON;		// Master/Slave/���I��

				clsDB.GetData(3+ii, &nSelectCh);
				if (m_bHalfFlg[nId] && !HARF_CC_SOCK_EXIST[ii]) {
					// �n�[�t�ŃJ���������NSW�̑��݂��Ȃ��{�[�hID�̏ꍇ�͋����I�ɖ��I��
					if(0 != nSelectCh){
						LOG(em_ERR), "[%s] <%d> �n�[�t�łɑ��݂��Ȃ��{�[�h�������l����Ă������ߋ����I�ɖ��I���ɕύX(board=%d, cam=%d)", my_sThreadName, nId, ii+1, nSelectCh);
						nSelectCh = 0;
					}
				}

				// �I��ch�������Ȃ��Slave�w��
				if( -1 >= nSelectCh )	{ emChSelMode = CH_SELECT_SLAVE; }
				if( 0  <  nSelectCh )	{ emChSelMode = CH_SELECT_MASTER; }

				m_Param.linksw[nId].nSelectCam[ii] = abs(nSelectCh);			// ����ȍ~ch�Ƃ��Ďg���ׂɁA�����l�ɂ���
				m_Param.linksw[nId].emChSelMode[ii] = emChSelMode;
				// 2020.05.07 �ΏۃJ�����̂ݐݒ� ---------->>>>
				m_Param.linksw[nId].bChSelCam[ii] = FALSE;
				// 2020.05.07 �ΏۃJ�����̂ݐݒ� <<<<----------

				if( 0 > m_Param.linksw[nId].nSelectCam[ii] || MAX_CAMLINKSW_ALLINPUT < m_Param.linksw[nId].nSelectCam[ii] )
				{
					// �J�����I��ݒ� �ُ�
					LOG(em_ERR), "[%s] �J�����I��ݒ�(DB) �ُ� [ID=%d, �{�[�h%d=%d, %s]", my_sThreadName, nId, ii+1, m_Param.linksw[nId].nSelectCam[ii], GetChSelectModeName(m_Param.linksw[nId].emChSelMode[ii]));
					return false;
				}
				LOG(em_INF), "[%s] �J�����I��ݒ�(DB) �擾 [ID=%d, �{�[�h%d=%d, %s]", my_sThreadName, nId, ii+1, m_Param.linksw[nId].nSelectCam[ii], GetChSelectModeName(m_Param.linksw[nId].emChSelMode[ii]));
				// 2017.02.27 Master/Slave��DB�w�� <<<<------------------------------------------------

			}

			nCnt++;
		}
		else if( sqlrc == KIZUODBC_FETCH_NODATE )				// �f�[�^����
		{
			break;
		}
		else													// �G���[
		{
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", DB_CAMERA_LINKSWITCH);
			return false;
		}
	}
	// �擾���R�[�h���`�F�b�N
	if( nCnt != NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA )
	{
		LOG(em_ERR), "[%s] �J���������N�X�C�b�`�ݒ� �����ُ� [CNT=%d(CNT:%d)]", my_sThreadName, nCnt, NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA);
		return false;
	}
#else
	m_Param.linksw[0].bStatus			= TRUE;
	m_Param.linksw[0].nSelectCam[0]		= 0;
	m_Param.linksw[0].nSelectCam[1]		= 1;
	m_Param.linksw[0].nSelectCam[2]		= 2;
	m_Param.linksw[0].nSelectCam[3]		= 3;
	m_Param.linksw[0].nSelectCam[4]		= 4;
	m_Param.linksw[0].nSelectCam[5]		= 0;
	m_Param.linksw[0].nSelectCam[6]		= 0;
	m_Param.linksw[0].nSelectCam[7]		= 0;
	m_Param.linksw[0].nSelectCam[8]		= 0;
	m_Param.linksw[0].nSelectCam[9]		= 0;
	m_Param.linksw[0].nSelectCam[10]	= 0;
	m_Param.linksw[0].nSelectCam[11]	= 0;
	m_Param.linksw[0].nSelectCam[12]	= 0;
	m_Param.linksw[0].nSelectCam[14]	= 0;

	m_Param.linksw[1].bStatus			= FALSE;
	m_Param.linksw[1].nSelectCam[0]		= 1;
	m_Param.linksw[1].nSelectCam[1]		= 2;
	m_Param.linksw[1].nSelectCam[2]		= 3;
	m_Param.linksw[1].nSelectCam[3]		= 4;
	m_Param.linksw[1].nSelectCam[4]		= 0;
	m_Param.linksw[1].nSelectCam[5]		= 0;
	m_Param.linksw[1].nSelectCam[6]		= 0;
	m_Param.linksw[1].nSelectCam[7]		= 0;
	m_Param.linksw[1].nSelectCam[8]		= 0;
	m_Param.linksw[1].nSelectCam[9]		= 0;
	m_Param.linksw[1].nSelectCam[10]	= 0;
	m_Param.linksw[1].nSelectCam[11]	= 0;
	m_Param.linksw[1].nSelectCam[12]	= 0;
	m_Param.linksw[1].nSelectCam[14]	= 0;

	//m_Param.linksw[2].bStatus			= TRUE;
	//m_Param.linksw[2].nSelectCam[0]		= 0;
	//m_Param.linksw[2].nSelectCam[1]		= 0;
	//m_Param.linksw[2].nSelectCam[2]		= 0;
	//m_Param.linksw[2].nSelectCam[3]		= 0;
	//m_Param.linksw[2].nSelectCam[4]		= 0;
	//m_Param.linksw[2].nSelectCam[5]		= 0;
	//m_Param.linksw[2].nSelectCam[6]		= 0;
	//m_Param.linksw[2].nSelectCam[7]		= 0;
	//m_Param.linksw[2].nSelectCam[8]		= 0;
	//m_Param.linksw[2].nSelectCam[9]		= 0;
	//m_Param.linksw[2].nSelectCam[10]	= 0;
	//m_Param.linksw[2].nSelectCam[11]	= 0;
	//m_Param.linksw[2].nSelectCam[12]	= 0;
	//m_Param.linksw[2].nSelectCam[14]	= 0;

	//m_Param.linksw[3].bStatus			= FALSE;
	//m_Param.linksw[3].nSelectCam[0]		= 0;
	//m_Param.linksw[3].nSelectCam[1]		= 0;
	//m_Param.linksw[3].nSelectCam[2]		= 0;
	//m_Param.linksw[3].nSelectCam[3]		= 0;
	//m_Param.linksw[3].nSelectCam[4]		= 0;
	//m_Param.linksw[3].nSelectCam[5]		= 0;
	//m_Param.linksw[3].nSelectCam[6]		= 0;
	//m_Param.linksw[3].nSelectCam[7]		= 0;
	//m_Param.linksw[3].nSelectCam[8]		= 0;
	//m_Param.linksw[3].nSelectCam[9]		= 0;
	//m_Param.linksw[3].nSelectCam[10]	= 0;
	//m_Param.linksw[3].nSelectCam[11]	= 0;
	//m_Param.linksw[3].nSelectCam[12]	= 0;
	//m_Param.linksw[3].nSelectCam[14]	= 0;
#endif

	LOG(em_MSG), "[%s] �ÓI�p�����[�^�擾����", my_sThreadName);
	return true;
}

//------------------------------------------
// �Z�o�p�����[�^�擾
//------------------------------------------
bool LinkswManager::CalcParam()
{
	//// ������
	memset(m_Param.bCamInputCh, 0x00, sizeof(m_Param.bCamInputCh));
	memset(m_Param.nCamSelBoardNo, 0x00, sizeof(m_Param.nCamSelBoardNo));


	//// 2017.02.27 Master/Slave��DB�w�� ------------------------------------------------->>>>
	//for( int ii=0; ii<NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; ii++ )
	//{
	//	for( int jj=0; jj<MAX_CAMLINKSW_OUTPUT; jj++ )
	//	{
	//		//// �Z�o
	//		int nFpgaMasterNo	= ii / MAX_CAMLINKSW_FPGA * MAX_CAMLINKSW_FPGA;
	//		int nFpgaNo			= (m_Param.linksw[ii].nSelectCam[jj] - 1) / MAX_CAMLINKSW_INPUT;	// 0�`1
	//		int nCamNo			= (m_Param.linksw[ii].nSelectCam[jj] - 1) % MAX_CAMLINKSW_INPUT;	// 0�`3
	//		int nCamNoAll		=  m_Param.linksw[ii].nSelectCam[jj] - 1;							// 0�`7
	//		// �`�F�b�N
	//		if( 0 > nFpgaMasterNo + nFpgaNo	|| NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA	<= nFpgaMasterNo + nFpgaNo )	continue;
	//		if( 0 > nCamNo					|| MAX_CAMLINKSW_INPUT				<  nCamNo )						continue;
	//		if( 0 > nCamNoAll				|| MAX_CAMLINKSW_ALLINPUT			<  nCamNoAll )					continue;

	//		//// �J��������Ch
	//		m_Param.bCamInputCh[ nFpgaMasterNo + nFpgaNo ][ nCamNo ] = TRUE;

	//		//// �J�����I���{�[�hNo
	//		if( 0 == m_Param.nCamSelBoardNo[ nFpgaMasterNo + 0 ][ nCamNoAll ] &&
	//			0 == m_Param.nCamSelBoardNo[ nFpgaMasterNo + 1 ][ nCamNoAll ] )						// ��ԗD��
	//		{
	//			m_Param.nCamSelBoardNo[ ii ][ nCamNoAll ] = jj+1;
	//		}
	//	}
	//}

	int nMasterCheck[NUM_CAMLINKSW][MAX_CAMLINKSW_ALLINPUT] = {{0}};		// Master�ݒ�`�F�b�N[�J���������N�X�C�b�`][����ch] (0:���I�� -1:Slave�̂݊���� 1:Master�����)

	for( int ii=0; ii<NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; ii++ )
	{
		for( int jj=0; jj<MAX_CAMLINKSW_OUTPUT; jj++ )
		{
			//// �Z�o
			int nCamLinkSwNo    = ii / MAX_CAMLINKSW_FPGA;											// ���̃{�[�h��������J���������N�X�C�b�`
			int nFpgaMasterNo	= ii - (ii % MAX_CAMLINKSW_FPGA);									// ���̃{�[�h��������J���������N�X�C�b�`��Master�{�[�hNo
			int nFpgaNo			= (m_Param.linksw[ii].nSelectCam[jj] - 1) / MAX_CAMLINKSW_INPUT;	// ����InputCh��Master�{�[�h�^Slave�{�[�h�̂ǂ���ɂ��Ă��邩(0�`1)
			int nCamNo			= (m_Param.linksw[ii].nSelectCam[jj] - 1) % MAX_CAMLINKSW_INPUT;	// �I������ch�̃J����No(0�`3)
			int nCamNoAll		=  m_Param.linksw[ii].nSelectCam[jj] - 1;							// �I������ch(0�`7)

			// �`�F�b�N
			if( 0 > nFpgaMasterNo + nFpgaNo	|| NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA	<= nFpgaMasterNo + nFpgaNo )	continue;
			if( 0 > nCamNo					|| MAX_CAMLINKSW_INPUT				<  nCamNo )						continue;
			if( 0 > nCamNoAll				|| MAX_CAMLINKSW_ALLINPUT			<  nCamNoAll )					continue;

			//// �J��������Ch
			m_Param.bCamInputCh[ nFpgaMasterNo + nFpgaNo ][ nCamNo ] = TRUE;

			//// �J�����I���{�[�hNo(=Master�w��)
			bool bIsMaster = true;
			bIsMaster &= (CH_SELECT_MASTER == m_Param.linksw[ii].emChSelMode[jj]);				// Master�w��

			for(int kk = 0; kk < MAX_CAMLINKSW_FPGA; kk++){
				bIsMaster &= ( 0 == m_Param.nCamSelBoardNo[ nFpgaMasterNo + kk ][ nCamNoAll ] );	// �����J���������N�X�C�b�`�̒��ɁA����Master�w��ς݂̏o��ch���݂�
			}

			if( bIsMaster )
			{
				m_Param.nCamSelBoardNo[ ii ][ nCamNoAll ] = jj+1;
				nMasterCheck[nCamLinkSwNo][nCamNoAll] = 1;				// Master�����
			}

			// Slave�ݒ�H
			if( CH_SELECT_SLAVE == m_Param.linksw[ii].emChSelMode[jj] &&
				1 != nMasterCheck[nCamLinkSwNo][nCamNoAll])
			{
				nMasterCheck[nCamLinkSwNo][nCamNoAll] = -1;				// Slave�����
			}
		}
	}

	//// Master���ݒ肳��Ă��Ȃ�ch�͂��Ȃ����H
	CString sErrMsg = "";
	CString sWk;
	for(int ii=0; ii<NUM_CAMLINKSW; ii++){
		for(int jj=0; jj<MAX_CAMLINKSW_ALLINPUT; jj++){

			if( -1 == nMasterCheck[ii][jj] ){
				if( 0 != strlen(sErrMsg) )	sErrMsg+=",";
				sWk.Format("[%d��� ����ch%d]", ii+1, jj+1);
				sErrMsg += sWk;
	}
		}
	}

	if( 0 != strlen(sErrMsg) )	LOG(em_ERR), "[%s] Master�ݒ肪������Ă��Ȃ�ch����I %s", my_sThreadName, sErrMsg);

	//// 2017.02.27 Master/Slave��DB�w�� <<<<-------------------------------------------------
	return true;
}

// 2017.02.27 Master/Slave��DB�w�� ------------------------------------------------->>>>

//--------------------------------------
// Ini�p�����[�^�𔽉f
//--------------------------------------
void LinkswManager::GetParamIni()
{
	for( int ii=0; ii<NUM_CAMLINKSW*MAX_CAMLINKSW_FPGA; ii++ )
	{
		for( int jj=0; jj<MAX_CAMLINKSW_OUTPUT; jj++ )
		{
			int					nSelectCh   = m_nSelectCam[ ii ][ jj ];
			ENUM_CH_SELECT_MODE emChSelMode = CH_SELECT_NON;		// Master/Slave/���I��

			// �I��Ch��0�Ȃ�΁AIni�ł͐ݒ肵�ĂȂ��̂Ŗ����iDB���ݒ肵���l���㏑�����Ȃ��悤�Ɂj
			if( 0 == nSelectCh ) continue;

			
			// �I��ch�������Ȃ��Slave�w��
			if( -1 >= nSelectCh )	{ emChSelMode = CH_SELECT_SLAVE; }
			if( 0  <  nSelectCh )	{ emChSelMode = CH_SELECT_MASTER; }

			// ����ȍ~ch�Ƃ��Ďg���ׂɁA�����l�ɂ���
			nSelectCh = abs(nSelectCh);
			if( 0 >= nSelectCh || MAX_CAMLINKSW_ALLINPUT < nSelectCh ) continue;


			m_Param.linksw[ ii ].nSelectCam[ jj ]  = nSelectCh;
			m_Param.linksw[ ii ].emChSelMode[ jj ] = emChSelMode;
			// 2020.05.07 �ΏۃJ�����̂ݐݒ� ---------->>>>
			m_Param.linksw[ ii ].bChSelCam[ jj ] = FALSE;
			// 2020.05.07 �ΏۃJ�����̂ݐݒ� <<<<----------

			LOG(em_INF), "[%s] �J�����I��ݒ�(INI) �擾 [ID=%d, �{�[�h%d=%d, %s]", my_sThreadName, ii, jj+1, m_Param.linksw[ ii ].nSelectCam[ jj ], GetChSelectModeName(m_Param.linksw[ ii ].emChSelMode[ jj ]));
		}
	}
									
}

//---------------------------------------
// �����I��Master�ݒ�
// �����ł́A�p�����[�^��Master/Slave�ݒ��
// �ύX����̂݁B���ۂɐ؂�ւ���ɂ́A
// ���̌��CalcParam()���R�[�����邱��
//---------------------------------------
// int	nId			: Master�ݒ肷��ch�̃{�[�hID(0�I���W��)
// int	nCh			: Master�ݒ肷��ch(0�I���W��)
// int	nSelectCam	: Master�ݒ肷��ch�̃J�����I��(1�I���W��)
//---------------------------------------
void LinkswManager::SetForciblyMaster(const int nId, const int nCh, const int nSelectCam)
{
	int nFirstBoardNo = nId - (nId % MAX_CAMLINKSW_FPGA);	// �Ώۃ{�[�h������J���������N�X�C�b�`�̐擪�{�[�hID(0�I���W��)

	for(int ii=nFirstBoardNo; ii < (nFirstBoardNo + MAX_CAMLINKSW_FPGA); ii++){

		for(int jj=0; jj < MAX_CAMLINKSW_OUTPUT; jj++){

			// Master�ݒ�̑Ώ�ch�Ȃ�΁A����
			if( nId == ii && nCh == jj)	continue;

			// Master�ݒ肷��ch�ƃJ�����I���������Ȃ�΁ASlave�ݒ�
			if( nSelectCam == m_Param.linksw[ii].nSelectCam[jj] ){
				m_Param.linksw[ii].emChSelMode[jj] = CH_SELECT_SLAVE;
			}
		}
	}
}

// 2017.02.27 Master/Slave��DB�w�� <<<<-------------------------------------------------