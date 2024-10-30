#include "StdAfx.h"
#include "TempManager.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// �R���X�g���N�^
// char const* cName ���O (ini�t�@�C���̃L�[�ƂȂ�)
//------------------------------------------
TempManager::TempManager(char const* cName) :
ThreadManager(cName),
m_emMainMode(MODE_NONE),
m_nSubMode(0),
m_nUnitMode(0),
m_bIsConnect(DIV_BOOL_NON),
m_bIsInital(DIV_BOOL_NON),
//m_bIsInital(false),
mcls_p232c(NULL)
{
	char	 cWk[64];
	CString  sWk;
	CString  sSession;

	//// �����̐ݒ�L�[������
	m_cMySession.Format("%s_%s", KT_TEMPMT, cName );
	m_nUnitNum		= GetPrivateProfileInt(m_cMySession, "UNIT_NUM",	0, TASKINI_NAME);
	_ASSERT(0!=m_nUnitNum);
	m_nCycleTime	= GetPrivateProfileInt(m_cMySession, "TIMER_CYCLE",	0, TASKINI_NAME);
	m_nCycleTime	*= 1000;
	m_nTempKeta		= GetPrivateProfileInt(m_cMySession, "TEMP_KETA",	0, TASKINI_NAME);


	//// �@��ID���擾
	m_nIdConnect	= GetPrivateProfileInt(m_cMySession, "CONNECT_ID",	0, TASKINI_NAME);
	for(int ii=0; ii<m_nUnitNum; ii++) {
		sSession.Format("%s_%s_UNIT%d", KT_TEMPMT, cName, ii+1 );

		GetPrivateProfileString(sSession, "NAME", "", cWk, sizeof(cWk), TASKINI_NAME);
		m_cUnitName[ii]		= cWk;
		m_nTempLimit[ii]	= GetPrivateProfileInt(sSession, "TEMP_LIMIT",	0, TASKINI_NAME);	
		m_nIdTemp[ii]		= GetPrivateProfileInt(sSession, "TEMP_ID",		0, TASKINI_NAME);		
		m_nIdTempSts[ii]	= GetPrivateProfileInt(sSession, "TEMP_ST_ID",	0, TASKINI_NAME);	
		m_nIdUnten[ii]		= GetPrivateProfileInt(sSession, "UNTEN_ID",	0, TASKINI_NAME);	
		m_nIdState[ii]		= GetPrivateProfileInt(sSession, "STATE_ID",	0, TASKINI_NAME);	
	}


	//// ������
	for(int ii=0; ii<MAX_TEMP_UNIT; ii++) {
		m_emUnten[ii]= DIV_BOOL_NON;
		m_nStatus[ii] = -1;
		m_dTemp[ii] = 0.0;
// --->>> 20180313 BUGFIX Ver(1.0.0.5 �� 1.1.0.0)
//		m_bTempSt[ii] = false;
		m_bTempSt[ii] = true;
// <<<--- 20180313 BUGFIX Ver(1.0.0.5 �� 1.1.0.0)
	}

	//// �n���h������
	m_evRecvEnd = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evSendEnd = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evReTry	= CreateWaitableTimer(NULL, FALSE, NULL);
	m_evAnserTimeOut= CreateWaitableTimer(NULL, FALSE, NULL);
	m_evCycleExec	= CreateWaitableTimer(NULL, FALSE, NULL); 
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
TempManager::~TempManager(void)
{
	//// �n���h�����
	CloseHandle(m_evRecvEnd);
	CloseHandle(m_evSendEnd);
	CloseHandle(m_evReTry);
	CloseHandle(m_evAnserTimeOut);
	CloseHandle(m_evCycleExec);
	// �N���[�Y
	if(NULL != mcls_p232c) {
		SAFE_DELETE(mcls_p232c);
	}
}


//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int TempManager::ThreadFirst()
{
	Sleep(50);
	this->SetTid();

	//// RS-232C �����ڑ�
	if( ! Open(true) ) {
		Close();
	}

	////// ���O����

	//// �C�x���g�ݒ�
	HANDLE hArray[] = { m_evReTry,
						m_evSendEnd,
						m_evRecvEnd,
						m_evAnserTimeOut,
						m_evCycleExec
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int TempManager::ThreadLast()
{
	// �������~
	CancelWaitableTimer(m_evReTry);
	CancelWaitableTimer(m_evAnserTimeOut);
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void TempManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_RETRY = 0,					// �Đڑ��J�n
			EV_SENDEND,						// ���M����
			EV_RECVEND,						// ��M����
			EV_ANSERTIMEOUT,				// ���M�A���T�[�^�C���A�E�g
			EV_CYCLEEXEC					// ��������s
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_RETRY: 							// �Đڑ��J�n
		if( ! Open(false) ){
			Close();
		}
		return;

//-----------------------------------------------------------------------------------------------
	case EV_SENDEND: 						// ���M����
		// ���ɉ������Ȃ�
		return;

//-----------------------------------------------------------------------------------------------
	case EV_RECVEND: 						// ��M����
		if( ! CheckDataRecv() ) {
			Close();
			return;
		}
		// ����Ɏ�M�ł����̂Ŏ��̔񓯊���M�J�n
		if( ! Recv() ) {
			return;
		}
		return;

//-----------------------------------------------------------------------------------------------
	case EV_ANSERTIMEOUT: 					// ���M�A���T�[�^�C���A�E�g
		if( DIV_BOOL_TRUE == m_bIsConnect) {
			LOG(em_ERR), "[%s] �����`���^�C���A�E�g <%s>:<%s>", my_sThreadName, GetMainModeName(), GetUnitName(m_nUnitMode-1));
			//syslog(0, "[<%s>:<%s>:<%s>]", my_sThreadName, GetMainModeName(), GetUnitName(m_nUnitMode-1));
		}
//		Close();					// �Đڑ�

// --->>> 20180313 BUGFIX Ver(1.0.0.5 �� 1.1.0.0)
//		if( ! CheckNextSend() ) return ;
		if (!CheckNextSend()) {
			Close();				// �Đڑ�
		}
// <<<--- 20180313 BUGFIX Ver(1.0.0.5 �� 1.1.0.0)
		return;

//-----------------------------------------------------------------------------------------------
	case EV_CYCLEEXEC: 						// ��������s
		if( DIV_BOOL_TRUE == m_bIsConnect) {
			m_emMainMode = MODE_READ_STATUS;
			if( ! CheckNextSend() ) {
				Close();			// �Đڑ�		// ���݂�NG
			}
		}
		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���x�p�l�����[�^����

//------------------------------------------
// �����M����
// �߂�l false:����ؒf���������{
//------------------------------------------
bool TempManager::CheckNextSend()
{
	//// �O����
	memset(m_cSendBuf, 0x00, sizeof(m_cSendBuf));		// ���M�o�b�t�@ �N���A

	//// ��������
	switch( m_emMainMode ) {
	
	//======================================================
	// ���������m�F��
	case MODE_OPEN_CHECK:
		// �T�u���[�h�ύX (�@�했�̐ݒ�����Ă���ꍇ�́A�T�u���[�h���ێ�)
		if( m_nUnitNum <= m_nUnitMode ) m_nUnitMode = 0;			// �Ō�̋@�펞
		if( 0 == m_nUnitMode ) m_nSubMode++;						// ���̃T�u���[�h��

		//---------------------------------------------
		if( 1==m_nSubMode ) {										// �G�R�[�o�b�N�e�X�g
			LOG(em_MSG), "[%s] ---------------------------------------- �G�R�[�o�b�N�e�X�g �J�n [UnitNo=%d]", my_sThreadName, m_nUnitMode + 1);

			CMD_TYPE_ECHO_BACK* buf = (CMD_TYPE_ECHO_BACK *)m_cSendBuf;
			memset(buf->cSTX,		STX_CODE,	sizeof(buf->cSTX));
			sprintf(buf->cNodeNo,	"%02d",		m_nUnitMode + 1);
			memcpy(buf->cSubAddr,	"00",		sizeof(buf->cSubAddr));
			memcpy(buf->cServiceID,	"0",		sizeof(buf->cServiceID));
			// FINS-mini ------------------------------------------------>>>
			memcpy(buf->cMRC,		"08",		sizeof(buf->cMRC));
			memcpy(buf->cSRC,		"01",		sizeof(buf->cSRC));
			// FINS-mini <<<------------------------------------------------
			
			memset(buf->cETX,		ETX_CODE,	sizeof(buf->cETX));
			// BCC�N���G�C�g
			CreateBCC((char*)buf, sizeof(CMD_TYPE_ECHO_BACK));

			// �@�했�̐ݒ�
			m_nUnitMode++;

		////---------------------------------------------
		//} else if( 2==m_nSubMode ) {
		//	LOG(em_MSG), "[%s] ---------------------------------------- �R���g���[���X�e�[�^�X�Ǐo�� �J�n [UnitNo=%d]", my_sThreadName, m_nUnitMode + 1);

		//	CMD_TYPE_CONTROLER_STATUS* buf = (CMD_TYPE_CONTROLER_STATUS *)m_cSendBuf;
		//	memset(buf->cSTX,		STX_CODE,	sizeof(buf->cSTX));
		//	sprintf(buf->cNodeNo,	"%02d",		m_nUnitMode + 1);
		//	memcpy(buf->cSubAddr,	"00",		sizeof(buf->cSubAddr));
		//	memcpy(buf->cServiceID,	"0",		sizeof(buf->cServiceID));
		//	// FINS-mini ------------------------------------------------>>>
		//	memcpy(buf->cMRC,		"06",		sizeof(buf->cMRC));
		//	memcpy(buf->cSRC,		"01",		sizeof(buf->cSRC));
		//	// FINS-mini <<<------------------------------------------------
		//	memset(buf->cETX,		ETX_CODE,	sizeof(buf->cETX));
		//	// BCC�N���G�C�g
		//	CreateBCC((char*)buf, sizeof(CMD_TYPE_CONTROLER_STATUS));

		//	// �@�했�̐ݒ�
		//	m_nUnitMode++;

		//---------------------------------------------
		} else if( 2==m_nSubMode ) {
			// ��������
			m_bIsInital = DIV_BOOL_TRUE;

			// �@���ԕύX
			if(0 != m_nIdConnect) KizuFunction::SetStatus(m_nIdConnect, true);

			// �ҋ@
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nUnitMode = 0;

			// �ڑ����������ׁA�^�C�}�[�Z�b�g
			LARGE_INTEGER ts;
			ts.QuadPart = (__int64)1000 * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
			SetWaitableTimer(m_evCycleExec, &ts, 0, NULL, NULL, FALSE);
		}
		break;


	//======================================================
	// �R���g���[���X�e�[�^�X�Ǐo����
	case MODE_READ_STATUS:
		// �T�u���[�h�ύX (�@�했�̐ݒ�����Ă���ꍇ�́A�T�u���[�h���ێ�)
		if( m_nUnitNum <= m_nUnitMode ) m_nUnitMode = 0;			// �Ō�̋@�펞
		if( 0 == m_nUnitMode ) m_nSubMode++;						// ���̃T�u���[�h��

		//---------------------------------------------
		if( 1==m_nSubMode ) {										// �R���g���[���X�e�[�^�X�Ǐo��
/*
			LOG(em_MSG), "[%s] ---------------------------------------- �R���g���[���X�e�[�^�X�Ǐo�� �J�n [UnitNo=%d]", my_sThreadName, m_nUnitMode + 1);

			CMD_TYPE_CONTROLER_STATUS* buf = (CMD_TYPE_CONTROLER_STATUS *)m_cSendBuf;
			memset(buf->cSTX,		STX_CODE,	sizeof(buf->cSTX));
			sprintf(buf->cNodeNo,	"%02d",		m_nUnitMode + 1);
			memcpy(buf->cSubAddr,	"00",		sizeof(buf->cSubAddr));
			memcpy(buf->cServiceID,	"0",		sizeof(buf->cServiceID));
			// FINS-mini ------------------------------------------------>>>
			memcpy(buf->cMRC,		"06",		sizeof(buf->cMRC));
			memcpy(buf->cSRC,		"01",		sizeof(buf->cSRC));
			// FINS-mini <<<------------------------------------------------
			memset(buf->cETX,		ETX_CODE,	sizeof(buf->cETX));
			// BCC�N���G�C�g
			CreateBCC((char*)buf, sizeof(CMD_TYPE_CONTROLER_STATUS));

			// �@�했�̐ݒ�
			m_nUnitMode++;
*/
			LOG(em_MSG), "[%s] ---------------------------------------- �R���g���[���X�e�[�^�X�Ǐo�� �J�n [UnitNo=%d]", my_sThreadName, m_nUnitMode + 1);

			CMD_TYPE_CONTROLER_STATUS* buf = (CMD_TYPE_CONTROLER_STATUS *)m_cSendBuf;
			memset(buf->cSTX,		STX_CODE,	sizeof(buf->cSTX));
			sprintf(buf->cNodeNo,	"%02d",		m_nUnitMode + 1);
			memcpy(buf->cSubAddr,	"00",		sizeof(buf->cSubAddr));
			memcpy(buf->cServiceID,	"0",		sizeof(buf->cServiceID));
			// FINS-mini ------------------------------------------------>>>
			memcpy(buf->cMRC,		"06",		sizeof(buf->cMRC));
			memcpy(buf->cSRC,		"01",		sizeof(buf->cSRC));
			// FINS-mini <<<------------------------------------------------
			memset(buf->cETX,		ETX_CODE,	sizeof(buf->cETX));
			// BCC�N���G�C�g
			CreateBCC((char*)buf, sizeof(CMD_TYPE_CONTROLER_STATUS));

			// �@�했�̐ݒ�
			m_nUnitMode++;


		//---------------------------------------------
		} else if( 2==m_nSubMode ) {
			LOG(em_MSG), "[%s] ---------------------------------------- ���j�^�l�Ǐo�� �J�n [UnitNo=%d]", my_sThreadName, m_nUnitMode + 1);

			CMD_TYPE_MONITOR_READ* buf = (CMD_TYPE_MONITOR_READ *)m_cSendBuf;
			memset(buf->cSTX,		STX_CODE,	sizeof(buf->cSTX));
			sprintf(buf->cNodeNo,	"%02d",		m_nUnitMode + 1);
			memcpy(buf->cSubAddr,	"00",		sizeof(buf->cSubAddr));
			memcpy(buf->cServiceID,	"0",		sizeof(buf->cServiceID));
			// FINS-mini ------------------------------------------------>>>
			memcpy(buf->cMRC,		"01",		sizeof(buf->cMRC));
			memcpy(buf->cSRC,		"01",		sizeof(buf->cSRC));
			memcpy(buf->cKind,		"C0",		sizeof(buf->cKind));
			memcpy(buf->cAddr,		"0002",		sizeof(buf->cAddr));
			memcpy(buf->cBitPos,	"00",		sizeof(buf->cBitPos));
			memcpy(buf->cCount,		"0001",		sizeof(buf->cCount));
			// FINS-mini <<<------------------------------------------------
			memset(buf->cETX,		ETX_CODE,	sizeof(buf->cETX));
			// BCC�N���G�C�g
			CreateBCC((char*)buf, sizeof(CMD_TYPE_MONITOR_READ));

			// �@�했�̐ݒ�
			m_nUnitMode++;


		//---------------------------------------------
		} else if( 3==m_nSubMode ) {
			//LOG(em_MSG), "[TempManager] �R���g���[���X�e�[�^�X�Ǐo�� ����");

			// �ҋ@
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nUnitMode = 0;

			// �ڑ����������ׁA�^�C�}�[�Z�b�g
			LARGE_INTEGER ts;
			ts.QuadPart = (__int64)m_nCycleTime * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
			SetWaitableTimer(m_evCycleExec, &ts, 0, NULL, NULL, FALSE);
		}
		break;

/*
	//======================================================
	// ���j�^�l�Ǐo����
	case MODE_READ_MONITOR:
		// �T�u���[�h�ύX (�@�했�̐ݒ�����Ă���ꍇ�́A�T�u���[�h���ێ�)
		if( m_nUnitNum <= m_nUnitMode ) m_nUnitMode = 0;			// �Ō�̋@�펞
		if( 0 == m_nUnitMode ) m_nSubMode++;						// ���̃T�u���[�h��

		//---------------------------------------------
		if( 1==m_nSubMode ) {										// ���j�^�l�Ǐo��
			LOG(em_MSG), "[%s] ---------------------------------------- ���j�^�l�Ǐo�� �J�n [UnitNo=%d]", my_sThreadName, m_nUnitMode + 1);

			CMD_TYPE_MONITOR_READ* buf = (CMD_TYPE_MONITOR_READ *)m_cSendBuf;
			memset(buf->cSTX,		STX_CODE,	sizeof(buf->cSTX));
			sprintf(buf->cNodeNo,	"%02d",		m_nUnitMode + 1);
			memcpy(buf->cSubAddr,	"00",		sizeof(buf->cSubAddr));
			memcpy(buf->cServiceID,	"0",		sizeof(buf->cServiceID));
			// FINS-mini ------------------------------------------------>>>
			memcpy(buf->cMRC,		"01",		sizeof(buf->cMRC));
			memcpy(buf->cSRC,		"01",		sizeof(buf->cSRC));
			memcpy(buf->cKind,		"C0",		sizeof(buf->cKind));
			memcpy(buf->cAddr,		"0002",		sizeof(buf->cAddr));
			memcpy(buf->cBitPos,	"00",		sizeof(buf->cBitPos));
			memcpy(buf->cCount,		"0001",		sizeof(buf->cCount));
			// FINS-mini <<<------------------------------------------------
			memset(buf->cETX,		ETX_CODE,	sizeof(buf->cETX));
			// BCC�N���G�C�g
			CreateBCC((char*)buf, sizeof(CMD_TYPE_MONITOR_READ));

			// �@�했�̐ݒ�
			m_nUnitMode++;

		//---------------------------------------------
		} else if( 2==m_nSubMode ) {
			//LOG(em_MSG), "[TempManager] ���j�^�l�Ǐo�� ����");

			// �ҋ@
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nUnitMode = 0;

			// �ڑ����������ׁA�^�C�}�[�Z�b�g
			LARGE_INTEGER ts;
			ts.QuadPart = (__int64)m_nCycleTime * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
			SetWaitableTimer(m_evReTry, &ts, 0, NULL, NULL, FALSE);
		}
		break;
*/
	}


	//======================================================
	// ���M
	if( 0==strlen(m_cSendBuf) ) return true;			// ���M�f�[�^�����̏ꍇ�́A�����Ԃ�
	if( ! Send() ) return false;

	// ���M���O
//��
	LOG(em_MSG), "[%s] ���M [%s]", my_sThreadName, m_cSendBuf);
	return true;
}

//------------------------------------------
// ��M�`���`�F�b�N
// RES_TYPE_ECHO_BACK* buf ��M�`��
//------------------------------------------
bool TempManager::Check_Echo_Back(RES_TYPE_ECHO_BACK* buf)
{
	int id = m_nUnitMode - 1;

	// MRC
	if( 0 != memcmp(buf->cMRC, "08", sizeof(buf->cMRC)) ) {
		LOG(em_ERR), "[%s] MRC �ُ� [%.2s]", my_sThreadName, buf->cMRC);
		//syslog(353, "[<%s> %s ���j�b�gNo=%.2s MRC=%.2s]", GetMainModeName(), my_sThreadName, buf->cNodeNo, buf->cMRC);
		return false;
	}
	// SRC
	if( 0 != memcmp(buf->cSRC, "01", sizeof(buf->cSRC)) ) {
		LOG(em_ERR), "[%s] SRC �ُ� [%.2s]", my_sThreadName, buf->cSRC);
		//syslog(353, "[<%s> %s ���j�b�gNo=%.2s SRC=%.2s]", GetMainModeName(), my_sThreadName, buf->cNodeNo, buf->cSRC);
		return false;
	}
	// MRES/SRES
	if( 0 != memcmp(buf->cResCode, RES_OK, sizeof(buf->cResCode)) ) {
		LOG(em_ERR), "[%s] MRES/SRES �ُ� [%s]", my_sThreadName, GetResCodeName(buf->cResCode));
		//syslog(353, "[<%s> %s ���j�b�gNo=%.2s MRES/SRES=%.4s(%s)]", GetMainModeName(), my_sThreadName, buf->cNodeNo, buf->cResCode, GetResCodeName(buf->cResCode));
		return false;
	}
	// EXT
	if( 0 != memcmp(buf->cEXT, EXT_OK, sizeof(buf->cEXT)) ) {
		LOG(em_ERR), "[%s] EXT �ُ� [%s]", my_sThreadName, GetEXTName(buf->cEXT));
		//syslog(353, "[<%s> %s ���j�b�gNo=%.2s EXT=%.2s(%s)]", GetMainModeName(), my_sThreadName, buf->cNodeNo, buf->cEXT, GetEXTName(buf->cEXT));
		return false;
	}
	return true;
}
bool TempManager::Check_Controler_Status(RES_TYPE_CONTROLER_STATUS* buf)
{
	int id = m_nUnitMode - 1;

	// MRC
	if( 0 != memcmp(buf->cMRC, "06", sizeof(buf->cMRC)) ) {
		LOG(em_ERR), "[%s] MRC �ُ� [%.2s]", my_sThreadName, buf->cMRC);
		//syslog(353, "[<%s> %s ���j�b�gNo=%.2s MRC=%.2s]", GetMainModeName(), my_sThreadName, buf->cNodeNo, buf->cMRC);
		return false;
	}
	// SRC
	if( 0 != memcmp(buf->cSRC, "01", sizeof(buf->cSRC)) ) {
		LOG(em_ERR), "[%s] SRC �ُ� [%.2s]", my_sThreadName, buf->cSRC);
		//syslog(353, "[<%s> %s ���j�b�gNo=%.2s SRC=%.2s]", GetMainModeName(), my_sThreadName, buf->cNodeNo, buf->cSRC);
		return false;
	}			
	// MRES/SRES
	if( 0 != memcmp(buf->cResCode, RES_OK, sizeof(buf->cResCode)) ) {
		LOG(em_ERR), "[%s] MRES/SRES �ُ� [%s]", my_sThreadName, GetResCodeName(buf->cResCode));
		//syslog(353, "[<%s> %s ���j�b�gNo=%.2s MRES/SRES=%.4s(%s)]", GetMainModeName(), my_sThreadName, buf->cNodeNo, buf->cResCode, GetResCodeName(buf->cResCode));
		return false;
	}
	// EXT
    if( 0 != memcmp(buf->cEXT, EXT_OK, sizeof(buf->cEXT)) ) {
		LOG(em_ERR), "[%s] EXT �ُ� [%s]", my_sThreadName, GetEXTName(buf->cEXT));
		//syslog(353, "[<%s> %s ���j�b�gNo=%.2s EXT=%.2s(%s)]", GetMainModeName(), my_sThreadName, buf->cNodeNo, buf->cEXT, GetEXTName(buf->cEXT));
		return false;
	}

	//======================================================
	// �^�]���
	if( 0 == memcmp(buf->cData1, "00", sizeof(buf->cData1)) ) {
		if( DIV_BOOL_TRUE != m_emUnten[id] ) {	
			LOG(em_MSG), "[%s] �^�]��� %s [����^�]��(%.2s)]", my_sThreadName, m_cUnitName[id], buf->cData1);		
			m_emUnten[id] = DIV_BOOL_TRUE;
			if(0 != m_nIdUnten[id]) KizuFunction::SetStatus(m_nIdUnten[id], true);

		}
	} else if( 0 == memcmp(buf->cData1, "01", sizeof(buf->cData1)) ) {
		LOG(em_ERR), "[%s] �^�]��� %s [��~��(%.2s)]", my_sThreadName, m_cUnitName[id], buf->cData1);
		if( DIV_BOOL_FALSE != m_emUnten[id] ) {
			//syslog(354, "[<%s> %s ��~]", GetMainModeName(), m_cUnitName[id]);
			m_emUnten[id] = DIV_BOOL_FALSE;
			if(0 != m_nIdUnten[id]) KizuFunction::SetStatus(m_nIdUnten[id], false);
		}
	} else {
		LOG(em_ERR), "[%s] �^�]��� %s [�s��(%.2s)]", my_sThreadName, m_cUnitName[id], buf->cData1);
		if( DIV_BOOL_FALSE != m_emUnten[id] ) {
			//syslog(354, "[<%s> %s �s��]", GetMainModeName(), m_cUnitName[id]);
			m_emUnten[id] = DIV_BOOL_FALSE;
			if(0 != m_nIdUnten[id]) KizuFunction::SetStatus(m_nIdUnten[id], false);
		}
	}

	//======================================================
	// �X�e�[�^�X
	if( 0 == memcmp(buf->cData2, "00", sizeof(buf->cData2)) ) {
		if( m_nStatus[id] != MT_STATUS_OK ) {
			LOG(em_MSG), "[%s] �X�e�[�^�X %s [����(%.2s)]", my_sThreadName, m_cUnitName[id], buf->cData2);
			m_nStatus[id] = MT_STATUS_OK;
			if(0 != m_nIdState[id]) KizuFunction::SetStatus(m_nIdState[id], m_nStatus[id]);
		}
	} else {
		if( buf->cData2[1] & 0x01 ) {
			LOG(em_WAR), "[%s] �X�e�[�^�X %s [���v�����(%.2s)]", my_sThreadName, m_cUnitName[id], buf->cData2);
			if( ! (m_nStatus[id] & MT_STATUS_ERR_NO_MEASURE) ) {
				//syslog(355, "[<%s> %s ���v�����]", GetMainModeName(), m_cUnitName[id]);
				m_nStatus[id] = MT_STATUS_ERR_NO_MEASURE;
				if(0 != m_nIdState[id]) KizuFunction::SetStatus(m_nIdState[id], m_nStatus[id]);
			}
			return false;
		}
		if( buf->cData2[1] & 0x02 ) {
			LOG(em_WAR), "[%s] �X�e�[�^�X %s [�\���͈̓I�[�o�[(%.2s)]", my_sThreadName, m_cUnitName[id], buf->cData2);
			if( ! (m_nStatus[id] & MT_STATUS_ERR_DISPLAY_OVER) ) {
				//syslog(355, "[<%s> %s �\���͈̓I�[�o�[]", GetMainModeName(), m_cUnitName[id]);
				m_nStatus[id] = MT_STATUS_ERR_DISPLAY_OVER;
				if(0 != m_nIdState[id]) KizuFunction::SetStatus(m_nIdState[id], m_nStatus[id]);
			}
			return false;
		}
		if( buf->cData2[1] & 0x04 ) {
			LOG(em_ERR), "[%s] �X�e�[�^�X %s [���ُ͈�A(%.2s)]", my_sThreadName, m_cUnitName[id], buf->cData2);
			if( ! (m_nStatus[id] & MT_STATUS_ERR_INPUT_A) ) {
				//syslog(355, "[<%s> %s ���ُ͈�A]", GetMainModeName(), m_cUnitName[id]);
				m_nStatus[id] = MT_STATUS_ERR_INPUT_A;
				if(0 != m_nIdState[id]) KizuFunction::SetStatus(m_nIdState[id], m_nStatus[id]);
			}
			return false;
		}
		if( buf->cData2[1] & 0x08 ) {
			LOG(em_ERR), "[%s] �X�e�[�^�X %s [���ُ͈�B(%.2s)]", my_sThreadName, m_cUnitName[id], buf->cData2);
			if( ! (m_nStatus[id] & MT_STATUS_ERR_INPUT_B) ) {
				//syslog(355, "[<%s> %s ���ُ͈�B]", GetMainModeName(), m_cUnitName[id]);
				m_nStatus[id] = MT_STATUS_ERR_INPUT_B;
				if(0 != m_nIdState[id]) KizuFunction::SetStatus(m_nIdState[id], m_nStatus[id]);
			}
			return false;
		}
	}
	return true;
}

bool TempManager::Check_Monitor_Read(RES_TYPE_MONITOR_READ* buf)
{
	int id = m_nUnitMode - 1;
	// MRC
	if( 0 != memcmp(buf->cMRC, "01", sizeof(buf->cMRC)) ) {
		LOG(em_ERR), "[%s] MRC �ُ� [%.2s]", my_sThreadName, buf->cMRC);
		//syslog(353, "[<%s> %s ���j�b�gNo=%.2s MRC=%.2s]", GetMainModeName(), my_sThreadName, buf->cNodeNo, buf->cMRC);
		return false;
	}
	// SRC
	if( 0 != memcmp(buf->cSRC, "01", sizeof(buf->cSRC)) ) {
		LOG(em_ERR), "[%s] SRC �ُ� [%.2s]", my_sThreadName, buf->cSRC);
		//syslog(353, "[<%s> %s ���j�b�gNo=%.2s SRC=%.2s]", GetMainModeName(), my_sThreadName, buf->cNodeNo, buf->cSRC);
		return false;
	}			
	// MRES/SRES
	if( 0 != memcmp(buf->cResCode, RES_OK, sizeof(buf->cResCode)) ) {
		LOG(em_ERR), "[%s] MRES/SRES �ُ� [%s]", my_sThreadName, GetResCodeName(buf->cResCode));
		//syslog(353, "[<%s> %s ���j�b�gNo=%.2s MRES/SRES=%.4s(%s)]", GetMainModeName(), my_sThreadName, buf->cNodeNo, buf->cResCode, GetResCodeName(buf->cResCode));
		return false;
	}
	// EXT
	if( 0 != memcmp(buf->cEXT, EXT_OK, sizeof(buf->cEXT)) ) {
		LOG(em_ERR), "[%s] EXT �ُ� [%s]", my_sThreadName, GetEXTName(buf->cEXT));
		//syslog(353, "[<%s> %s ���j�b�gNo=%.2s EXT=%.2s(%s)]", GetMainModeName(), my_sThreadName, buf->cNodeNo, buf->cEXT, GetEXTName(buf->cEXT));
		return false;
	}


	//======================================================
	// ���j�^�l
	CString cwk;
// --->>> 20141212 MOD �}�C�i�X�l�ϊ��s��C��
//	cwk.Format("0x%.8s", buf->cData);
//	double monitor = (double)strtol(cwk, NULL, 0);
//	//monitor /= 10;
//	for(int ii=0; ii<m_nTempKeta; ii++) monitor /= 10;			// ���̂ɂ���Č����قȂ邽�߂̑Ή�	
	cwk.Format("%.8s", buf->cData);
	long nwk;
	sscanf(cwk, "%X", &nwk);
	double monitor = (double)nwk;
	for(int ii=0; ii<m_nTempKeta; ii++) monitor /= 10.0;		// ���̂ɂ���Č����قȂ邽�߂̑Ή�	
// <<<--- 20141212 MOD �}�C�i�X�l�ϊ��s��C��

	// �^�]��ԃ`�F�b�N
	if( DIV_BOOL_TRUE != m_emUnten[id] ) monitor = 0.0;

	LOG(em_INF), "[%s] %s ���x=%lf [��]", my_sThreadName, m_cUnitName[id], monitor);

	// ���j�^�l �ۑ�
	if(m_dTemp[id] != monitor) {
		if(0 != m_nIdTemp[id]) KizuFunction::SetStatus(m_nIdTemp[id], (int)(monitor+0.5), false);	// �l�̌ܓ�
	}
	m_dTemp[id] = monitor;

	// ���x�`�F�b�N
	bool bLimitOk = true;
	if( 0 != m_nTempLimit[id] && m_nTempLimit[id] <= (int)monitor ) bLimitOk = false;
	if( ! bLimitOk && m_bTempSt[id] ) {	// ���큨�ُ�
		if(0 != m_nIdTempSts[id]) KizuFunction::SetStatus(m_nIdTempSts[id], false);

	} else if( bLimitOk && ! m_bTempSt[id]){ // �ُ큨����
		if(0 != m_nIdTempSts[id]) KizuFunction::SetStatus(m_nIdTempSts[id], true);

	}
	m_bTempSt[id] = bLimitOk; 

	return true;
}


//------------------------------------------
// ��M�`���`�F�b�N
// �߂�l false:����ؒf���������{
//------------------------------------------
bool TempManager::CheckDataRecv()
{
	char	cWk[64];
	CString	strWk;

	//======================================================
	// ���O����

	// ���݂�m_nUnitMode����@��̔z��ʒu���Z�o
	int id = m_nUnitMode - 1;
//��
	LOG(em_MSG), "[%s] ��M [%s]", my_sThreadName, m_cRecvBuf);

	// �����`����M�^�C���A�E�g �^�C�}�[��~
	CancelWaitableTimer(m_evAnserTimeOut);

	// BCC �`�F�b�N
	int nRecvBufSize = (int)strlen(m_cRecvBuf);
	if( ! CheckBCC(m_cRecvBuf, nRecvBufSize) ) {
		LOG(em_ERR), "[%s] �����`�� BCC�ُ� [��M=%.1s]", my_sThreadName, &m_cRecvBuf[nRecvBufSize-1]);
		//syslog(0, "[%s:���j�b�gNo=%d]", my_sThreadName, id);	// BCC�`�F�b�N�ُ�
		return false;
	}

	// ���j�b�gNo �`�F�b�N
	sprintf(cWk, "%02d", id+1);
	if( 0 != memcmp(&m_cRecvBuf[1], cWk, 2) ) {
		LOG(em_ERR), "[%s] �����`�� ���j�b�gNo�ُ� [���M=%s][��M=%.2s]", my_sThreadName, cWk, &m_cRecvBuf[1]);
		//syslog(0, "[<%s> %s ���M���j�b�gNo=%s ��M���j�b�gNo=%.2s]", GetMainModeName(), my_sThreadName, cWk, &m_cRecvBuf[1]);
		return false;
	}

	//// ��������
	switch( m_emMainMode ) {
	

	//======================================================
	// ���������m�F��
	case MODE_OPEN_CHECK:

		//---------------------------------------------
		////// �G�R�[�o�b�N�e�X�g
		if( 1==m_nSubMode ) {
			// �`�����e�`�F�b�N
			RES_TYPE_ECHO_BACK* buf = (RES_TYPE_ECHO_BACK *)m_cRecvBuf;
			if( ! Check_Echo_Back(buf) ) return false;
			if( ! CheckNextSend() ) return false;
		}
		////---------------------------------------------
		//} else if( 2==m_nSubMode ) {
		//	RES_TYPE_CONTROLER_STATUS* buf = (RES_TYPE_CONTROLER_STATUS *)m_cRecvBuf;
		//	if( ! Check_Controler_Status(buf) ) return false;
		//	if( ! CheckNextSend() ) return false;
		//}
		break;

	//======================================================
	// �R���g���[���X�e�[�^�X�Ǐo����
	case MODE_READ_STATUS:
		////// �R���g���[���X�e�[�^�X�Ǐo��
		if( 1==m_nSubMode ) {
			RES_TYPE_CONTROLER_STATUS* buf = (RES_TYPE_CONTROLER_STATUS *)m_cRecvBuf;
			if( ! Check_Controler_Status(buf) ) return false;
			if( ! CheckNextSend() ) return false;
		//---------------------------------------------
		} else if( 2==m_nSubMode ) {
			RES_TYPE_MONITOR_READ* buf = (RES_TYPE_MONITOR_READ *)m_cRecvBuf;
			if( ! Check_Monitor_Read(buf) ) return false;
			if( ! CheckNextSend() ) return false;
		}
	}


	return true;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 232C�ʐM�֘A

//------------------------------------------
// 232C �I�[�v��
// bool bLogDsp false:���O�o�͖���
//------------------------------------------
bool TempManager::Open(bool bLogDsp)
{
	// Rs232cBase�C���X�^���X����
	try {
		mcls_p232c = new Rs232cBase(m_cMySession, TASKINI_NAME, GetTid(), PARAM_232C_RECVBUF_SIZE, PARAM_232C_SENDBUF_SIZE, WM_USER);
		//mcls_p232c->SetRecvEndCR(ETX_CODE);	// �I�[�R�[�h�`�F�b�N�ɉϒ��ɓǍ��݂Ƃ���
	} catch(int code) {
		mcls_p232c = NULL;
		if( bLogDsp ) {
			LOG(em_ERR), "[%s] 232C�I�[�v���G���[ (err_code = %d)", my_sThreadName, code);
			syslog(SYSNO_232C_NEW_ERR, "[���x�p�l�����[�^:err_code = %d]", code);
		}
		return false;
	}

	// �ڑ�����
	memset(m_cRecvBuf, 0x00, sizeof(m_cRecvBuf));
	memset(m_cSendBuf, 0x00, sizeof(m_cSendBuf));
	//syslog(SYSNO_232C_OPEN, "[���x�p�l�����[�^]");

	// ��M�J�n
	if( ! Recv() ) return false;

	// �����ʐM�`�F�b�N
	m_bIsConnect = DIV_BOOL_TRUE;
	m_emMainMode = MODE_OPEN_CHECK;
	m_nSubMode = 0;
	m_nUnitMode = 0;

	// ���x�p�l�����[�^�ƒʐM�ł��邩�m�F
	if( ! CheckNextSend() ) return false;
	return true;
}

//------------------------------------------
// 232C �N���[�Y
//------------------------------------------
void TempManager::Close()
{
	// ���g���C�^�C�}�ȊO�͑S�Ē�~
	CancelWaitableTimer(m_evReTry);
	CancelWaitableTimer(m_evAnserTimeOut);

	// �N���[�Y
	if( NULL != mcls_p232c ) {
		SAFE_DELETE(mcls_p232c);
	}

	//// ���g���C�̂��тɃ��O�o�͂����̋֎~
	// ���킩��ُ�ɕς�����ꍇ�̂�
	if( DIV_BOOL_TRUE == m_bIsConnect ) {
		LOG(em_ERR), "[%s] 232C�N���[�Y", my_sThreadName);
		syslog(SYSNO_232C_CLOSE, "[���x�p�l�����[�^]");
		// �@���ԕύX
		if(0 != m_nIdConnect) KizuFunction::SetStatus(m_nIdConnect, false);
	}

	// �X�e�[�^�X������
	m_bIsConnect = DIV_BOOL_FALSE;
	m_bIsInital  = DIV_BOOL_FALSE;
	m_emMainMode = MODE_RETRY;
	m_nSubMode = 0;
	m_nUnitMode = 0;

	// ���g���C�^�C�} �Z�b�g
	LARGE_INTEGER ts;
	ts.QuadPart = (__int64)PARAM_TIMEOUT_RETRY * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evReTry, &ts, 0, NULL, NULL, FALSE);
}

//------------------------------------------
// 232C �񓯊� ��M
//------------------------------------------
bool TempManager::Recv()
{
	if( NULL == mcls_p232c ) return true;

	// ��M�o�b�t�@�N���A
	memset(m_cRecvBuf, 0x00, sizeof(m_cRecvBuf));

	//��M�o�b�t�@�T�C�Y�����[�h���ƂɌ���
	DWORD nRecvBufSize = 0;
	switch( m_emMainMode ) {
		case MODE_OPEN_CHECK:
			// ���������m�F��
			if(1==m_nSubMode)
			{
				nRecvBufSize = sizeof(RES_TYPE_ECHO_BACK);
			}
			//if(2==m_nSubMode)
			//{
			//	nRecvBufSize = sizeof(RES_TYPE_CONTROLER_STATUS);
			//}
			break;

		case MODE_WAIT:
			nRecvBufSize = sizeof(RES_TYPE_CONTROLER_STATUS);	// WAIT��̏���́A�R���g���[���ǂݍ���
			break;
				
		case MODE_READ_STATUS:
			if( 1==m_nSubMode )
			{
				// �R���g���[���X�e�[�^�X�Ǐo����
				nRecvBufSize = sizeof(RES_TYPE_CONTROLER_STATUS);
			}
			if(2==m_nSubMode)
			{
				nRecvBufSize = sizeof(RES_TYPE_MONITOR_READ);
			}
			break;

		default:		// ��{
			nRecvBufSize = sizeof(RES_TYPE_ECHO_BACK);
			//LOG(em_WAR), "[%s] ���C�����[�h�s�� (MainMode = %d)", my_sThreadName, m_emMainMode);
			break;
	}

	//// ��M
	int iRetc = mcls_p232c->Recv(m_cRecvBuf, nRecvBufSize, m_evRecvEnd);
	if(0 != iRetc) {
		// ��M�G���[
		LOG(em_ERR), "[%s] 232C��M�G���[ (err_code = %d)", my_sThreadName, iRetc);
		syslog(SYSNO_232C_RECV_ERR, "[���x�p�l�����[�^:err_code = %d]", iRetc);
		return false;
	}
	return true;
}

//------------------------------------------
// 232C ���� ���M
//------------------------------------------
bool TempManager::Send()
{
	if( NULL == mcls_p232c) return true;

	//// ���M
	int iRetc = mcls_p232c->Send(m_cSendBuf, (DWORD)strlen(m_cSendBuf), m_evSendEnd);
	if(0 != iRetc) {
		// ���M�G���[
		LOG(em_ERR), "[%s] 232C���M�G���[ (err_code = %d)", my_sThreadName, iRetc);
		syslog(SYSNO_232C_SEND_ERR, "[���x�p�l�����[�^ err_code = %d]", iRetc);
		return false;
	}

	//// ���M��
	// �����^�C�} �Z�b�g
	LARGE_INTEGER ts;
	ts.QuadPart = (__int64)PARAM_TIMEOUT_ANSER * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evAnserTimeOut, &ts, 0, NULL, NULL, FALSE);
	return true;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �����֘A

//------------------------------------------
// ���C�����[�h����
// EM_TEMP_MAINMODE em ���[�h
// �߂�l ����
//------------------------------------------
CString TempManager::GetMainModeName(EM_TEMP_MAINMODE em)
{
	CString	wk;
	if( MODE_NONE == em ) {
		wk = "�������";
	} else if( MODE_OPEN_CHECK == em ) {
		wk = "���������m�F��";
	} else if( MODE_WAIT == em ) {
		wk = "�ҋ@";
	} else if( MODE_READ_STATUS == em ) {
		wk = "�Ǐo����";
	//} else if(  MODE_READ_MONITOR == em ) {
	//	wk = "���j�^�l�Ǐo����";
	} else if( MODE_RETRY == em ) {
		wk = "�I�[�v�����g���C��";
	} else {
		wk = "���[�h�ُ�";
	}
	return wk;
}

//------------------------------------------
// ���X�|���X�R�[�h����
// char* cResCode ���X�|���X�R�[�h
// �߂�l ����
//------------------------------------------
CString TempManager::GetResCodeName(char* cResCode)
{
	CString wk;

	if( 0==memcmp(cResCode, RES_OK, RES_SIZE) ) {
		wk = "����I��";
	} else if( 0==memcmp(cResCode, RES_ERR_CMDLEN_OVER, RES_SIZE) ) {
		wk = "�R�}���h���I�[�o�[";
	} else if( 0==memcmp(cResCode, RES_ERR_CMDLEN_SHORT, RES_SIZE) ) {
		wk = "�R�}���h���s��";
	} else if( 0==memcmp(cResCode, RES_ERR_AREAKIND, RES_SIZE) ) {
		wk = "�G���A��ʃG���[";
	} else if( 0==memcmp(cResCode, RES_ERR_RESLEN_OVER, RES_SIZE) ) {
		wk = "���X�|���X���I�[�o�[";
	} else if( 0==memcmp(cResCode, RES_ERR_PARAM, RES_SIZE) ) {
		wk = "�p�����[�^�G���[";
	} else if( 0==memcmp(cResCode, RES_ERR_SYSTEM, RES_SIZE) ) {
		wk = "����G���[";
	} else {
		wk = "���X�|���X�R�[�h�ُ�";
	}

	return wk;
}


//------------------------------------------
// �I���R�[�h����
// char* cEXT �I���R�[�h
// �߂�l ����
//------------------------------------------
CString TempManager::GetEXTName(char* cEXT)
{
	CString	wk;

	if( 0==memcmp(cEXT, EXT_OK, EXT_SIZE) ) {
		wk = "����I��";
	} else if( 0==memcmp(cEXT, EXT_ERR_FINS, EXT_SIZE) ) {
		wk = "FINS�R�}���h�G���[";
	} else if( 0==memcmp(cEXT, EXT_ERR_PARITY, EXT_SIZE) ) {
		wk = "�p���e�B�G���[";
	} else if( 0==memcmp(cEXT, EXT_ERR_FRAMING, EXT_SIZE) ) {
		wk = "�t���[�~���O�G���[";
	} else if( 0==memcmp(cEXT, EXT_ERR_OVERRUN, EXT_SIZE) ) {
		wk = "�I�[�o�[�����G���[";
	} else if( 0==memcmp(cEXT, EXT_ERR_BCC, EXT_SIZE) ) {
		wk = "BCC�G���[";
	} else if( 0==memcmp(cEXT, EXT_ERR_FORMAT, EXT_SIZE) ) {
		wk = "�t�H�[�}�b�g�G���[";
	} else if( 0==memcmp(cEXT, EXT_ERR_SUBADDRESS, EXT_SIZE) ) {
		wk = "�T�u�A�h���X�G���[";
	} else if( 0==memcmp(cEXT, EXT_ERR_FRAMELEN, EXT_SIZE) ) {
		wk = "�t���[�����G���[";
	} else {
		wk = "�I���R�[�h�ُ�";
	}

	return wk;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BCC�֘A

//------------------------------------------
// BCC�N���G�C�g
// char* cSendBuf ���M�o�b�t�@
// int nBufSize ���M�f�[�^�T�C�Y
//------------------------------------------
void TempManager::CreateBCC(char* cSendBuf, int nBufSize)
{
	char			cBCC = 0x00;

	// ���j�b�gNo�`ETX�܂ł�BCC���v�Z
	for(int ii=1; ii<nBufSize-1; ii++) {
		cBCC ^= cSendBuf[ii];
	}

	// BCC���i�[
	cSendBuf[nBufSize-1] = cBCC;
}

//------------------------------------------
// BCC�`�F�b�N
// char* cRecvBuf ��M�o�b�t�@
// int nBufSize ��M�f�[�^�T�C�Y
//------------------------------------------
bool TempManager::CheckBCC(char* cRecvBuf, int nBufSize)
{
	char			cBCC = 0x00;
	bool			bRet = true;

	// ���j�b�gNo�`ETX�܂ł�BCC���v�Z
	for(int ii=1; ii<nBufSize-1; ii++) {
		cBCC ^= cRecvBuf[ii];
	}
	if( cBCC != cRecvBuf[nBufSize-1] ) {
		bRet = false;
	}

	return bRet;
}
