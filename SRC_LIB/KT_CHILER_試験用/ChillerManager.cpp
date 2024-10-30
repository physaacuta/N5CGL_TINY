#include "StdAfx.h"
#include "ChillerManager.h"

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
// int nTempThr ���x臒l [�x]
// int nTimerCycle ���x��荞�ݎ��� [�b]
//------------------------------------------
ChillerManager::ChillerManager(char const* cName, int nTempThr, int nTimerCycle) :
ThreadManager(cName),
m_nTempThr(nTempThr),
m_nTimerCycle(nTimerCycle),
mcls_p232c(NULL),
m_bIsOpen(false),
m_bIsConnect(false),
m_emMainMode(MODE_NONE),
m_dNowTemp(0.0),
m_dSetTemp(0.0),
m_emUnten(MODE_STOP),
m_sAlmNo(""),
m_bLimitTemp(true),
m_bAlmMachine(true)
{
	//// �����̐ݒ�L�[������
	m_cMySession.Format("%s_%s", KT_CHILER, cName );

	//// �`���[�̌^�����擾
	m_nChilerType = GetPrivateProfileInt(m_cMySession, "CHILER_TYPE",	0, TASKINI_NAME);			// �`���[�^��
	
	//// �@��ID���擾
	m_nIdConnect= GetPrivateProfileInt(m_cMySession, "CHILER_CONNECT_ID",	0, TASKINI_NAME);		// ������
	m_nIdState	= GetPrivateProfileInt(m_cMySession, "CHILER_STATE_ID",		0, TASKINI_NAME);		// �^�]���	
	m_nIdErr	= GetPrivateProfileInt(m_cMySession, "CHILER_ERR_ID",		0, TASKINI_NAME);		// �ُ���
	m_nIdTemp	= GetPrivateProfileInt(m_cMySession, "CHILER_TEMP_ID",		0, TASKINI_NAME);		// ���x
	m_nIdTempSt	= GetPrivateProfileInt(m_cMySession, "CHILER_TEMP_ST_ID",	0, TASKINI_NAME);		// ���x���

	//// �n���h������
	m_evRecv	  = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_tmReTry	  = CreateWaitableTimer(NULL, FALSE, NULL);
	m_tmCycleExec = CreateWaitableTimer(NULL, FALSE, NULL);
	m_tmSendAnser = CreateWaitableTimer(NULL, FALSE, NULL);
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
ChillerManager::~ChillerManager(void)
{
	//// �n���h�����
	CloseHandle(m_evRecv);
	CloseHandle(m_tmReTry);
	CloseHandle(m_tmCycleExec);
	CloseHandle(m_tmSendAnser);

	// �C���X�^���X���
	SAFE_DELETE(mcls_p232c);
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int ChillerManager::ThreadFirst()
{
	//// RS-232C �����ڑ�
	if( ! RS_Open(true) ) {
		RS_Close();
	}

	//// �C�x���g�ݒ�
	HANDLE hArray[] = { m_evRecv,
						m_tmReTry,
						m_tmSendAnser,
						m_tmCycleExec,
						mque_SendItem.g_evSem
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int ChillerManager::ThreadLast()
{
	RS_Close();
	return 0;
}

//------------------------------------------
// �X���b�h��WaitFor�҂��̒��O����  
// int nDefWaitNum ���̐�
// int* nNowWaitNum �ύX��̐�
//------------------------------------------
void ChillerManager::ThreadWaitFor(int nDefWaitNum, int* nNowWaitNum)
{
	// ��M���́A���M�v�����󂯕t���Ȃ����߂̏��u
	if( MODE_RECV_EXEC == m_emMainMode )*nNowWaitNum = nDefWaitNum-1;
	else								*nNowWaitNum = nDefWaitNum;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void ChillerManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_RECV = 0,					// ��M����
			EV_TIME_RETRY,					// ����I�[�v�����g���C
			EV_TIME_OUT,					// �����A���T�[NG
			EV_TIME_SATE_CHECK,				// ������`�F�b�N
			EV_QUE,							// ���M�˗��L���[
	};


	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_RECV: 								// ��M����
		if( -1 == RecvEnd() ) {
			RS_Close();		// ��M�ُ�ɂ��ؒf
		}
		return;

//-----------------------------------------------------------------------------------------------
	case EV_TIME_RETRY:						// ����I�[�v�����g���C
		LOG(em_ERR), "[%s] ����I�[�v�����g���C", my_sThreadName);
		if( ! RS_Open(false) ){
			RS_Close();
		}
		return;

//-----------------------------------------------------------------------------------------------
	case EV_TIME_OUT:						// �����A���T�[�^�C���A�E�g
		LOG(em_ERR), "[%s] �A���T�[�^�C���A�E�g [%s]", my_sThreadName, GetSendCommandName(GetSendCommand((char*)&m_InfSend.cBuf[3])));
		RS_Close();
		break;

//-----------------------------------------------------------------------------------------------
	// ���ڂ̂��Ƃ肪����ɍs����܂Œ�����m�F�͓��삵�Ȃ�
	case EV_TIME_SATE_CHECK:				// �X�e�[�^�X������`�F�b�N
		AddQue_SendCommand(SEND_CHECK_NOW_TEMP);	// �t������l
		AddQue_SendCommand(SEND_CHECK_SET_TEMP);	// �t���ݒ�l
		AddQue_SendCommand(SEND_CHECK_MODE);		// �^�]���
		AddQue_SendCommand(SEND_CHECK_ALM);			// �x��M��

		return;

//-----------------------------------------------------------------------------------------------
	case EV_QUE: 							// ���M�˗��L���[
		if(true) {
			// ���M�f�[�^���o��
			INF_SEND* pDeli = mque_SendItem.GetFromHead(); 
			memcpy(&m_InfSend, pDeli, sizeof(m_InfSend));
			delete pDeli;

			// ���M
			if( ! SendStart() ){
				RS_Close();
			}
		}
		break;

//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
	return;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����M����

//------------------------------------------
// �L���[�ɗ��܂��Ă��鑗�M�˗���S�č폜
// �߂�l ���A���
//------------------------------------------
void ChillerManager::DeleteAllQue()
{
	INF_SEND* pDeli = NULL;

	while(true) {
		pDeli = mque_SendItem.GetFromHead(0);
		if( pDeli == NULL ) break;
		
		// ���
		delete pDeli;
	}
}

//------------------------------------------
// ���M�R�}���h
// EM_CHILER_SEND_COMMAND em ���M�R�}���h
// �߂�l ���A���
//------------------------------------------
bool ChillerManager::AddQue_SendCommand(EM_CHILER_SEND_COMMAND em)
{
	// ����
	INF_SEND* pDeli = new INF_SEND;
	memset(pDeli, 0x00, sizeof(INF_SEND));

	pDeli->emChilerCommand = em;
	pDeli->nSize = 6;

	// �R�}���h�Z�b�g

	// �f�o�C�XNo
	pDeli->cBuf[0] = 0x04;		// EOT
	pDeli->cBuf[1] = 0x30;		// �f�o�C�X�A�h���X
	pDeli->cBuf[2] = 0x30;		// �V

	switch( em ) {
	// [M1] �t������l
	case SEND_CHECK_NOW_TEMP:
		pDeli->cBuf[3] = 'M';
		pDeli->cBuf[4] = '1';
		break;

	// [S1] �t���ݒ�l
	case SEND_CHECK_SET_TEMP:
		pDeli->cBuf[3] = 'S';
		pDeli->cBuf[4] = '1';
		break;

	// [JO] �^�]���
	case SEND_CHECK_MODE:
		pDeli->cBuf[3] = 'J';
		pDeli->cBuf[4] = 'O';
		break;

	// [ER] �x��M��
	case RECV_CHECK_ALM:
		pDeli->cBuf[3] = 'E';
		pDeli->cBuf[4] = 'R';
		break;
	}

	pDeli->cBuf[5] = 0x05;

	// �ǉ�
	if( ! mque_SendItem.AddToTail(pDeli, 20, 20) ) {		// ��Γo�^
		// ��Γo�^�������E�E�E
		LOG(em_ERR), "[%s] ���M�˗��L���[�t�� [%s]", my_sThreadName, GetSendCommandName(em));
		syslog(SYSNO_QUEFULL_ERR, "[%s:%s]", my_sThreadName, GetSendCommandName(em));
	
		// �̈���
		delete pDeli;
		return false;
	}

	return true;
}

//------------------------------------------
// ���M�J�n
// �߂�l ���A���
//------------------------------------------
bool ChillerManager::SendStart()
{
	// �Ή������M�R�}���h������ꍇ�̂� 
	m_emMainMode = MODE_RECV_EXEC;			// ��M�R�}���h�҂��ɂ���

	// ���M
	return RS_Send();
}

//------------------------------------------
// ��M����
// �߂�l 0:����������M  -1:�ُ�(�ؒf���Ă�) 1:�R�}���h�ɑ΂���A���T�[����M
//------------------------------------------
int ChillerManager::RecvEnd()
{
	bool retc;

	// �R�}���h����
	EM_CHILER_RECV_COMMAND  emRecv = GetRecvCommand((char*)&m_InfRecv.cBuf[1]);
	EM_CHILER_SEND_COMMAND  emSend = GetSendCommand((char*)&m_InfSend.cBuf[3]);

//*/ ��
	LOG(em_INF), "[%s] ��M [�O�񑗐M=%s] [%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X]", 
		my_sThreadName,
		GetSendCommandName(emSend),
		m_InfRecv.cBuf[0],
		m_InfRecv.cBuf[1],
		m_InfRecv.cBuf[2],
		m_InfRecv.cBuf[3],
		m_InfRecv.cBuf[4],
		m_InfRecv.cBuf[5],
		m_InfRecv.cBuf[6],
		m_InfRecv.cBuf[7],
		m_InfRecv.cBuf[8],
		m_InfRecv.cBuf[9],
		m_InfRecv.cBuf[10],
		m_InfRecv.cBuf[11],
		m_InfRecv.cBuf[12],
		m_InfRecv.cBuf[13],
		m_InfRecv.cBuf[14]);
//*/

	// ///////////////////////////////////////////////////////////////////
	// �O����
	//
	if( RECV_CHECK_NOW_TEMP == emRecv || 
		RECV_CHECK_SET_TEMP == emRecv || 
		RECV_CHECK_MODE		== emRecv || 
		RECV_CHECK_ALM		== emRecv) {

		// �f�[�^���������M�R�}���h����M
		if( 3 == m_InfRecv.nNow ) {
			//��M�R�}���h�̎�ʂŁA�f�[�^���̒��������߂ĕ�����̂ŁA�S�̒������Ē�`
			//�܂���M���S���������Ă��Ȃ��ׁA����������M
			if (RECV_CHECK_MODE == emRecv) {
				m_InfRecv.nSize = 6;
			} else if (RECV_CHECK_NOW_TEMP == emRecv || RECV_CHECK_SET_TEMP == emRecv) {
				m_InfRecv.nSize = 13;
			} else if (RECV_CHECK_ALM == emRecv) {
				if(0 == m_nChilerType)	m_InfRecv.nSize = 8;
				else					m_InfRecv.nSize = 9;
			}

			retc = RS_Recv(m_InfRecv.nNow, m_InfRecv.nSize - m_InfRecv.nNow);
			return retc ? 0 : -1;
		}
	}

	// ///////////////////////////////////////////////////////////////////
	// �P�R�}���h��M������
	//
	//LOG(em_INF), "[%s] �R�}���h��M [%s] [size=%d,%.2s] [�O�񑗐M=%s]",
	//	my_sThreadName, GetRecvCommandName(emRecv), m_InfRecv.nSize, &m_InfRecv.cBuf[1], GetSendCommandName(emSend) );

	// ����ڑ��F�� (������M������OK)
	if( ! m_bIsConnect) {
		LOG(em_MSG), "[%s] �`���[���u���u ��������", my_sThreadName);
		syslog(SYSNO_232C_OPEN, "[%s]", my_sThreadName);

		m_bIsConnect = true;
		KizuFunction::SetStatus(m_nIdConnect, true);		

		// ���񂩂�̎����^�C�}�[�X�^�[�g
		__int64 ts = (__int64)(m_nTimerCycle*1000)* -10000;	// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
		SetWaitableTimer(m_tmCycleExec, (LARGE_INTEGER *)&ts, m_nTimerCycle*1000, NULL, NULL, FALSE);
	}

	// �R�}���h�ɂ��U�蕪��
	if( RECV_CHECK_NON == emRecv) {							// �s���Ȃ�ُ�Ƃ��Ă���
		LOG(em_ERR), "[%s] �s���R�}���h [size=%d,%s]", my_sThreadName, m_InfRecv.nSize, m_InfRecv.cBuf);
		syslog(SYSNO_232C_SEND_ERR, "[%s:size=%d:%s]", my_sThreadName, m_InfRecv.nSize, m_InfRecv.cBuf);
		return -1;
	}
	
	// ///////////////////////////////////////////////////////////////////
	// �����܂ŗ�����A��M�R�}���h
	//
	CString  wk;
	if(MODE_RECV_EXEC != m_emMainMode) { LOG(em_INF), "�����Ȃ��M�R�}���h�������Ă����I");}

	// �R�}���h�ɂ�菈������
	switch( emRecv ) {
		// -------------------------------------------------------------
		case RECV_CHECK_NOW_TEMP:												// �t������l
			if (true) {
				char* sValue;
				if( 0 == m_nChilerType ) {
					sValue = new char[6];
					sprintf(sValue, "%.5s", (char*)&m_InfRecv.cBuf[6]);
					m_dNowTemp = atof(sValue);
				}
				else {
					sValue = new char[7];
					sprintf(sValue, "%.6s", (char*)&m_InfRecv.cBuf[5]);
					m_dNowTemp = atof(sValue);
				}
				delete sValue;

				// �t������l�i�[
				if( (double)m_nTempThr <= m_dNowTemp) {
					if( m_bLimitTemp ) {
						LOG(em_ERR), "[%s] �`���[���x����I�[�o�[ %2.2f��", my_sThreadName, m_dNowTemp);
						if(0!=m_nIdTempSt) KizuFunction::SetStatus(m_nIdTempSt, false);
					}
					m_bLimitTemp = false;
				} else {
					if( ! m_bLimitTemp ) {
						LOG(em_MSG), "[%s] �`���[���x���한�A %2.2f��", my_sThreadName, m_dNowTemp);
						if(0!=m_nIdTempSt) KizuFunction::SetStatus(m_nIdTempSt, true);
					}
					m_bLimitTemp = true;
				}
				if(0!=m_nIdTemp) KizuFunction::SetStatus(m_nIdTemp, (int)(m_dNowTemp), false);
			}
			break;

		// -------------------------------------------------------------
		case RECV_CHECK_SET_TEMP:
			if (true) {
				char* sValue;
				if( 0 == m_nChilerType ) {
					sValue = new char[6];
					sprintf(sValue, "%.5s", (char*)&m_InfRecv.cBuf[6]);
					m_dSetTemp = atof(sValue);
				}
				else {
					sValue = new char[7];
					sprintf(sValue, "%.6s", (char*)&m_InfRecv.cBuf[5]);
					m_dSetTemp = atof(sValue);
				}
				delete sValue;
			}
			break;

			// -------------------------------------------------------------
		case RECV_CHECK_MODE:
			if (true) {
				CString  wk;
				int nVal = atoi((char*)&m_InfRecv.cBuf[3]);

				if (m_emUnten != MODE_COOL && (EM_CHILER_UNTENMODE)nVal == MODE_COOL) {
					LOG(em_MSG), "[%s] �^�]������", my_sThreadName);
					if(0!=m_nIdState) KizuFunction::SetStatus(m_nIdState, true);
				} else if (m_emUnten == MODE_COOL && (EM_CHILER_UNTENMODE)nVal != MODE_COOL) {
					LOG(em_MSG), "[%s] �|���v��~", my_sThreadName);
					if(0!=m_nIdState) KizuFunction::SetStatus(m_nIdState, false);
				}

				m_emUnten = (EM_CHILER_UNTENMODE)nVal;
			}
			break;

			// -------------------------------------------------------------
		case RECV_CHECK_ALM:
			if (true) {
				if( 0 == m_nChilerType ) {
					m_sAlmNo.Format("%.3s", (char*)&m_InfRecv.cBuf[3]);
				}
				else {
					m_sAlmNo.Format("%.4s", (char*)&m_InfRecv.cBuf[3]);
				}

				if (m_sAlmNo != "    ") {
					if (m_bAlmMachine) {
						CString wk;
						LOG(em_ERR), "[%s] �`���[���u�ُ� Alarm[%s]", my_sThreadName, m_sAlmNo);
						wk.Format("[ErrID:%s]", m_sAlmNo);
						if(0!=m_nIdErr) KizuFunction::SetStatus(m_nIdErr, false, true, wk);
					}
					m_bAlmMachine = false;
				} else {
					if (!m_bAlmMachine) {
						LOG(em_MSG), "[%s] �`���[���u�ُ한�A", my_sThreadName);
						if(0!=m_nIdErr) KizuFunction::SetStatus(m_nIdErr, true);
					}
					m_bAlmMachine = true;
				}
			}

		default :
			break;
	}

	// ��n��
	m_emMainMode = MODE_WAIT;
	CancelWaitableTimer(m_tmSendAnser);

	// ��M�����㏭���҂�(�`���[���u�̎d�l) (�p�́A��M�����瑬�U�Ŏ��`���𑗂�Ȃ�)
	Sleep(200);

	// ����M
	retc = RS_Recv();
	return retc ? 1 : -1;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RS-232C����

//------------------------------------------
// 232C �I�[�v��
// bool bLogDsp false:���O�o�͖���
//------------------------------------------
bool ChillerManager::RS_Open(bool bLogDsp) 
{
	// Rs232cBase�C���X�^���X����
	try{
		mcls_p232c = new Rs232cBase(m_cMySession, TASKINI_NAME, GetTid(), PARAM_232C_RECVBUF_SIZE, PARAM_232C_SENDBUF_SIZE, WM_USER, true);
	} catch(int code) {
		mcls_p232c = NULL;
		if(bLogDsp) {
			LOG(em_ERR), "[%s] 232C�I�[�v���G���[(err_code = %d)", my_sThreadName, code);
			syslog(SYSNO_232C_NEW_ERR, "[%s err_code = %d]", my_sThreadName, code);
		}
		return false;
	}
	
	// �X�e�[�^�X������
	memset(&m_InfRecv, 0x00, sizeof(m_InfRecv));
	memset(&m_InfSend, 0x00, sizeof(m_InfSend));
	m_bIsOpen = true;			// �Ƃ肠�����I�[�v���͏o���Ă���
	m_bIsConnect = false;		// �܂��A�`���[���u�Ɛڑ��ł��邩�͕s��
	m_emMainMode = MODE_NONE;

	// ��M�J�n
	//if( ! RS_Recv(0, 3, 13) ) return false;
	if( ! RS_Recv() ) return false;

	AddQue_SendCommand(SEND_CHECK_NOW_TEMP);	// �t������l
	AddQue_SendCommand(SEND_CHECK_SET_TEMP);	// �t���ݒ�l
	AddQue_SendCommand(SEND_CHECK_MODE);		// �^�]���

	return true;
}

//------------------------------------------
// 232C �N���[�Y
//------------------------------------------
void ChillerManager::RS_Close() 
{
	// ���g���C�^�C�}�ȊO�͂��ׂĒ�~
	CancelWaitableTimer(m_evRecv);
	CancelWaitableTimer(m_tmCycleExec);
	CancelWaitableTimer(m_tmSendAnser);

	// �N���[�Y
	if(NULL != mcls_p232c) {
		SAFE_DELETE(mcls_p232c);
	}

	//// ���g���C�̂��тɃ��O�o�͂����̋֎~
	// ���킩��ُ�ɕς�����ꍇ�̂�
	if(m_bIsConnect) {
		LOG(em_MSG), "[%s] 232C�N���[�Y", my_sThreadName);
		syslog(SYSNO_232C_CLOSE, "[%s]", my_sThreadName);
		KizuFunction::SetStatus(m_nIdConnect, false);		
	}

	// �X�e�[�^�X������
	memset(&m_InfRecv, 0x00, sizeof(m_InfRecv));
	memset(&m_InfSend, 0x00, sizeof(m_InfSend));
	m_bIsOpen = false;
	m_bIsConnect = false;
	m_emMainMode = MODE_NONE;

	// ���M�˗��̃L���[������΍폜���Ă���
	DeleteAllQue();

	// ���g���C�^�C�} �Z�b�g
	__int64 ts = PARAM_TIMEOUT_RETRY * -10000; ;	// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_tmReTry,  (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE);
}

//------------------------------------------
// 232C �񓯊� ��M
// int pos �Z�b�g����o�b�t�@�J�n�ʒu (0�I���W��)
// int size ��M�T�C�Y
// int total �S���̃T�C�Y
//------------------------------------------
bool ChillerManager::RS_Recv(int pos, int size, int total) 
{
	if(NULL == mcls_p232c) return true;

	// ��M�J�n����
	if(0 == pos) {
		memset(&m_InfRecv, 0x00, sizeof(m_InfRecv));
		m_InfRecv.nNow = 0;
		m_InfRecv.nSize = total;
	}

	// �r���ł̕ύX����t���
	if(1 != total) {
		m_InfRecv.nSize = total;
	}

	// �w��o�C�g�ÂǍ�
	int iRetc = mcls_p232c->Recv(&m_InfRecv.cBuf[pos], size, m_evRecv );
	if(0 != iRetc) {
		// ��M�G���[
		LOG(em_ERR), "[%s] 232C��M�G���[(err_code = %d)", my_sThreadName, iRetc);
		syslog(SYSNO_232C_RECV_ERR, "[%s err_code = %d]", my_sThreadName, iRetc);
		return false;
	}

	// ���㏈��
	m_InfRecv.nNow += size;			// �����܂ł́A��M������
	return true;
}

//------------------------------------------
// 232C ���� ���M
//------------------------------------------
bool ChillerManager::RS_Send() 
{
	if(NULL == mcls_p232c) return true;

	LOG(em_INF), "[%s] �R�}���h���M [%s] [%02X%02X%02X%02X%02X%02X]", my_sThreadName, GetSendCommandName(m_InfSend.emChilerCommand),
		m_InfSend.cBuf[0], m_InfSend.cBuf[1], m_InfSend.cBuf[2], m_InfSend.cBuf[3], m_InfSend.cBuf[4], m_InfSend.cBuf[5]);
	
	//	syslog(217,"[%s:%s]", my_sThreadName, GetSendCommandName(m_InfSend.emChilerCommand) );

	int iRetc = mcls_p232c->Send(m_InfSend.cBuf, m_InfSend.nSize);
	if(0 != iRetc) {
		// ���M�G���[
		LOG(em_ERR), "[%s] [%s] 232C���M�G���[(err_code = %d)", my_sThreadName, GetSendCommandName(m_InfSend.emChilerCommand), iRetc);
		syslog(SYSNO_232C_SEND_ERR, "[%s err_code = %d]", my_sThreadName, iRetc);
		return false;
	}

	// �����A���T�[�^�C�}�[
	__int64 ts = PARAM_TIMEOUT_ANSER * -10000;	// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_tmSendAnser,  (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE);
	return true;
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���ʏ���

//------------------------------------------
// ���M�R�}���h����
// EM_CHILER_SEND_COMMAND em �R�}���h
// �߂�l ����
//------------------------------------------
CString ChillerManager::GetSendCommandName(EM_CHILER_SEND_COMMAND em)
{
	CString wk;
	switch( em ) {
		case SEND_CHECK_NOW_TEMP:
			wk = "�t������l";
			break;
		case SEND_CHECK_SET_TEMP:
			wk = "�t���ݒ�l";
			break;
		case SEND_CHECK_MODE:
			wk = "�^�]���";
			break;
		case SEND_CHECK_ALM:
			wk = "�x��M��";
			break;
		default :
			wk = "���[�h�ُ�";
			break;
	}
	return wk;
}

//------------------------------------------
// ��M�R�}���h����
// EM_CHILER_SEND_COMMAND em �R�}���h
// �߂�l ����
//------------------------------------------
CString ChillerManager::GetRecvCommandName(EM_CHILER_RECV_COMMAND em)
{
	CString wk;
	switch( em ) {
		case RECV_CHECK_NOW_TEMP:
			wk = "�t������l";
			break;
		case RECV_CHECK_SET_TEMP:
			wk = "�t���ݒ�l";
			break;
		case RECV_CHECK_MODE:
			wk = "�^�]���";
			break;
		case RECV_CHECK_ALM:
			wk = "�x��M��";
			break;
		default :
			wk = "���[�h�ُ�";
			break;
	}
	return wk;
}

//------------------------------------------
// ���M�R�[�h�̓���
// char code �擪1����
// �߂�l ��M�R�}���h
//------------------------------------------
ChillerManager::EM_CHILER_SEND_COMMAND ChillerManager::GetSendCommand(char* code)
{
	if (0 == memcmp(code, "M1", 2)) return SEND_CHECK_NOW_TEMP;
	if (0 == memcmp(code, "S1", 2)) return SEND_CHECK_SET_TEMP;
	if (0 == memcmp(code, "JO", 2)) return SEND_CHECK_MODE;
	if (0 == memcmp(code, "ER", 2)) return SEND_CHECK_ALM;

	return SEND_CHECK_NON;
}

//------------------------------------------
// ��M�R�[�h�̓���
// char code �擪1����
// �߂�l ��M�R�}���h
//------------------------------------------
ChillerManager::EM_CHILER_RECV_COMMAND ChillerManager::GetRecvCommand(char* code)
{
	if (0 == memcmp(code, "M1", 2)) return RECV_CHECK_NOW_TEMP;
	if (0 == memcmp(code, "S1", 2)) return RECV_CHECK_SET_TEMP;
	if (0 == memcmp(code, "JO", 2)) return RECV_CHECK_MODE;
	if (0 == memcmp(code, "ER", 2)) return RECV_CHECK_ALM;

	return RECV_CHECK_NON;
}

//------------------------------------------
// ���C�����[�h����
// EM_LED_MAINMODE em ���[�h
// �߂�l ����
//------------------------------------------
CString ChillerManager::GetMainModeName(EM_CHILER_MAINMODE em)
{
	CString wk;
	switch( em ) {
		case MODE_NONE:
			wk = "�ؒf��";
			break;
		case MODE_WAIT:
			wk = "�ҋ@";
			break;
		case MODE_RECV_EXEC:
			wk = "�����A���T�[�҂�";
			break;
		case MODE_RETRY:
			wk = "�I�[�v�����g���C��";
			break;
		default :
			wk = "���[�h�ُ�";
			break;
	}
	return wk;
}

//------------------------------------------
// �`���[���u�^�]���
// EM_CHILER_UNTENMODE em ���[�h
// �߂�l ����
//------------------------------------------
CString ChillerManager::GetUntenName(EM_CHILER_UNTENMODE em)
{
	CString wk;
	switch( em ) {
		case MODE_STOP:
			wk = "��~";
			break;
		case MODE_COOL:
			wk = "�^�]";
			break;
		case MODE_POMP:
			wk = "�|���v�P�Ɖ^�]";
			break;
		default :
			wk = "���[�h�ُ�";
			break;
	}
	return wk;
}
