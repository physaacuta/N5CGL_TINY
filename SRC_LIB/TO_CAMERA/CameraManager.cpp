#include "StdAfx.h"
#include "CameraManager.h"

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
CameraManager::CameraManager(void) :
m_bIsPulsu(false),
m_bIsConnect(false),
m_bNowSend(false),
m_emMainMode(MODE_NONE),
m_nSubMode(0),
m_nCamMode(0),
m_nSwMode(0),
m_emPLGmode(DIV_PLG_LINE),
#ifdef IS_TO_CAMERA_TBL
m_hCam(NULL),
#endif
mcls_p232c(NULL),
mcls_pLog(NULL)
{
	// ����������΂���
	m_nUnit			= 0;
	my_sThreadName	= TO_CAMERA;
	m_cMySession	= TO_CAMERA;
	m_cPrmCommonTblName	= DB_PARAM_COMMON;
	m_nMyCamNum		= NUM_CAMERA;
	m_nMyCamIdStart	= 0;
	m_nMyCamIdEnd	= m_nMyCamIdStart+m_nMyCamNum;


	memset(m_cRecvBuf, 0x00, sizeof(m_cRecvBuf));
	memset(m_cSendBuf, 0x00, sizeof(m_cSendBuf));
	memset(m_cSendManyu, 0x00, sizeof(m_cSendManyu));
	memset(&m_typAgcInf, 0x00, sizeof(m_typAgcInf));

	for(int ii=0; ii<NUM_CAMERA; ii++){
		m_bAlame[ii] = true;
		m_nNowExpTime[ii] = 0;
	}

	// �\�ʏ�Ԃ́A�f�t�H���g�\�ʏ�ԂƂ���
	m_nNowScode[0] = MAX_SCODE;
	m_nNowScode[1] = MAX_SCODE;

	// �p�����[�^�N���A
	memset(&m_Param, 0x00, sizeof(m_Param));

	// �C�x���g����
	m_evRecvEnd = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evSendEnd = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evReTry	= CreateWaitableTimer(NULL, FALSE, NULL);
	m_evAnserTimeOut = CreateWaitableTimer(NULL, FALSE, NULL);

#ifndef VER_AVALDATA
	//// �z���~�X�΍� (�悤�́ADS��=[0]���ǂ̃J����ID�����L�q)
	// �f�t�H���g
	for(int ii=0; ii<NUM_CAMERA; ii++) {
		m_nCamID[ii] = ii;
	}
	// ini�t�@�C���ł̎w��l���擾
	CString strVal;
	int		nVal;
	for(int ii=0; ii<NUM_CAMERA; ii++) {
		strVal.Format("CAMID_%d", ii+1);
		nVal = GetPrivateProfileInt(m_cMySession, strVal, 0, TASKINI_NAME);
		if( 0 >= nVal || 32 < nVal) continue;

		m_nCamID[ii] = nVal -1 ;
	}
	// �C�ӂŕύX
	//m_nCamID[0] = 4 -1;
	//m_nCamID[1] = 8 -1;
	//m_nCamID[2] = 3 -1;
	//m_nCamID[3] = 7 -1;
	//m_nCamID[4] = 2 -1;
	//m_nCamID[5] = 6 -1;
	//m_nCamID[6] = 1 -1;
	//m_nCamID[7] = 5 -1;

	//m_nCamID[8]  = 12 -1;
	//m_nCamID[9]  = 16 -1;
	//m_nCamID[10] = 11 -1;
	//m_nCamID[11] = 15  -1;
	//m_nCamID[12] = 10 -1;
	//m_nCamID[13] = 14 -1;
	//m_nCamID[14] = 9 -1;
	//m_nCamID[15] = 13 -1;
#endif

	// �t�B���^�[�l�擾 [0.1��s]
	m_nFilter = GetPrivateProfileInt(m_cMySession, "FILTER", 0, TASKINI_NAME);
	// # H�R�}���h (�����l0) ���W�b�^�[�΍�B�U���t���΍�B�ׂ���ňړ����ς����B0�`5
	m_nH = GetPrivateProfileInt(m_cMySession, "COMMAND_H", 0, TASKINI_NAME);

	// �C�ӃR�}���h
	m_nCmdIniInputNum = 0;
	CString sKey;
	while(true) {
		char cWk[64];
		sKey.Format("CMD%d", m_nCmdIniInputNum);
		GetPrivateProfileString( m_cMySession, sKey, "", cWk, sizeof(cWk), TASKINI_NAME );
		if(0 == strlen(cWk)) break;

		m_lstCmdIniInput.push_back( cWk );
		m_nCmdIniInputNum++;
	}
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
CameraManager::~CameraManager(void)
{
	CloseHandle(m_evRecvEnd);
	CloseHandle(m_evSendEnd);
	CloseHandle(m_evReTry);
	CloseHandle(m_evAnserTimeOut);

	// �N���[�Y
	if(NULL != mcls_p232c) {
		SAFE_DELETE(mcls_p232c);
	}

	// �J���R�����e�[�u��
#ifdef IS_TO_CAMERA_TBL
	if( NULL != m_hCam ) mem_close(&m_hCam, (LPVOID *)&mtbl_pCam);							// �J���R�����
#endif
}

//------------------------------------------
// ���N���X�Z�b�g
// int nUnit			���j�b�g�ԍ� (0�I���W��)
// char const* sSession	Ini�Z�N�V������
//------------------------------------------
void CameraManager::SetClass(int nUnit, char* sSession)
{
	m_nUnit = nUnit;
	m_cMySession = sSession;
	my_sThreadName.Format("CamMgr_%d", m_nUnit); 


	// ���̑�����H�����ݒ荀�ځH�Ǎ�
	char	cWk[64];
	int		nWk;
	GetPrivateProfileString(m_cMySession, "PARAM_CNT_TBL_NAME", m_cPrmCommonTblName, cWk, sizeof(cWk), TASKINI_NAME);	m_cPrmCommonTblName	= cWk;
	m_nMyCamNum		= GetPrivateProfileInt(m_cMySession, "CAMID_NUM", m_nMyCamNum, TASKINI_NAME);
	m_nMyCamIdStart = GetPrivateProfileInt(m_cMySession, "CAMID_START", m_nMyCamIdStart, TASKINI_NAME); 
	m_nMyCamIdEnd	= m_nMyCamIdStart+m_nMyCamNum;

	// �t�B���^�[�l�擾 [0.1��s]
	m_nFilter = GetPrivateProfileInt(m_cMySession, "FILTER", 0, TASKINI_NAME);
	// # H�R�}���h (�����l0) ���W�b�^�[�΍�B�U���t���΍�B�ׂ���ňړ����ς����B0�`5
	m_nH = GetPrivateProfileInt(m_cMySession, "COMMAND_H", 0, TASKINI_NAME);

	// �C�ӃR�}���h
	m_nCmdIniInputNum = 0;
	CString sKey;
	while (true) {
		char cWk[64];
		sKey.Format("CMD%d", m_nCmdIniInputNum);
		GetPrivateProfileString(m_cMySession, sKey, "", cWk, sizeof(cWk), TASKINI_NAME);
		if (0 == strlen(cWk)) break;

		m_lstCmdIniInput.push_back(cWk);
		m_nCmdIniInputNum++;
	}
}

//------------------------------------------
// �X���b�h
//------------------------------------------
UINT WINAPI CameraManager::MainThread(LPVOID param)
{
	////// ���C���C���X�^���X���o��
	CameraManager *p = (CameraManager *)param;
	p->SetTid();								// ���X���b�hID�Z�b�g

	// WaitForMultipleObjects�C�x���g
	HANDLE	hArray[] = {p->my_evStop,
						p->m_evReTry,
						p->m_evSendEnd,
						p->m_evRecvEnd,
						p->m_evAnserTimeOut,
						p->gque_Deli.g_evSem 
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// �X���b�h�I��
			EV_RETRY,						// �Đڑ��J�n
			EV_SENDEND,						// ���M����
			EV_RECVEND,						// ��M����
			EV_ANSERTIMEOUT,				// ���M�A���T�[�^�C���A�E�g
			EV_QUE,							// ���[���X���b�g��M
			EV_END	};
	

	//// ���L������
	// �J���R�����e�[�u��
#ifdef IS_TO_CAMERA_TBL
//*/----> ��
#ifdef LOCAL
	p->m_hCam = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(TO_CAMERA_TBL), TBL_TO_CAMERA_NAME);
	p->mtbl_pCam =(TO_CAMERA_TBL*) MapViewOfFile(p->m_hCam, FILE_MAP_ALL_ACCESS, 0, 0, 0);
#endif
//*///<<---

	mem_connect(&p->m_hCam, (LPVOID *)&p->mtbl_pCam, TBL_TO_CAMERA_NAME);
	if(p->m_hCam == NULL) {
		pLOG(em_ERR), "[%s] �J���R�����e�[�u�� �A�N�Z�X���s", p->my_sThreadName);
		syslog(SYSNO_MEM_OPEN, "�J���R�����e�[�u�� �A�N�Z�X���s [Unit=%d]", p->m_nUnit);
		_ASSERT(FALSE);
	}
	pLOG(em_INF), "[%s] �J���R�����e�[�u�� �A�N�Z�X���[�h", p->my_sThreadName);
#endif



	// �p�����[�^�ǂݍ���
	while(TRUE) {
		if( p->GetParamStatic() &&
			p->GetParam() ) break;
		pLOG(em_MSG), "[%s] �p�����[�^�Ǎ����s(DB�ڑ��ُ�H�I)�B���g���C���E�E�E", p->my_sThreadName);
		Sleep(3000);
	}

#ifdef VER_AVALDATA
	//// �z���~�X�΍�
	// �������N���A
	for(int ii=0; ii<NUM_CAMLINKSW; ii++) {
		p->m_nCamNum[ii] = 0;
		for(int jj=0; jj<MAX_CAMLINKSW_ALLINPUT; jj++) {
			p->m_nCamID[ii][jj] = 0;
			p->m_nInpCh[ii][jj] = 0;
		}
	}
	// ini�t�@�C���ł̎w��l���擾
	CString strVal;
	int     nCamID;
	int		nInpCh;
	for(int ii=0; ii<NUM_CAMLINKSW; ii++) {
		for(int jj=0; jj<MAX_CAMLINKSW_ALLINPUT; jj++) {
			// �J����ID
			strVal.Format("CAMID_%d_%d", p->m_Param.nSwitchMasterID[ii], jj+1);
			nCamID = GetPrivateProfileInt(p->m_cMySession, strVal, 0, TASKINI_NAME);
			if( 0 >= nCamID || NUM_CAMERA < nCamID) continue;

			// ����CH
			strVal.Format("INPCH_%d_%d", p->m_Param.nSwitchMasterID[ii], jj+1);
			nInpCh = GetPrivateProfileInt(p->m_cMySession, strVal, 0, TASKINI_NAME);
			if( 0 >= nInpCh || MAX_CAMLINKSW_ALLINPUT < nInpCh) continue;

			// �Z�b�g
			p->m_nCamID[ii][jj] = nCamID -1;
			p->m_nInpCh[ii][jj] = nInpCh -1;
			p->m_nCamNum[ii]++;
		}
	}
	// �f�t�H���g
	bool	bDefault = true;
	for(int ii=0; ii<NUM_CAMLINKSW; ii++) {
		if( 0 != p->m_nCamNum[ii] ) {
			bDefault = false;
			break;
		}
	}
	if( bDefault ) {
		for(int ii=0; ii<NUM_CAMERA; ii++) {


#if LINKSW_CAM_SELECT_MODE == 0		// �J�������e�J���������N�X�C�b�`�ɑO�l�߂Őڑ�Ver
			p->m_nCamID[ ii / (MAX_CAMLINKSW_ALLINPUT) ][ ii % (MAX_CAMLINKSW_ALLINPUT) ] = ii;
			p->m_nInpCh[ ii / (MAX_CAMLINKSW_ALLINPUT) ][ ii % (MAX_CAMLINKSW_ALLINPUT) ] = ii % MAX_CAMLINKSW_ALLINPUT;
			p->m_nCamNum[ ii / (MAX_CAMLINKSW_ALLINPUT) ]++;
#elif LINKSW_CAM_SELECT_MODE == 1	// �J�������e�J���������N�X�C�b�`�ɓ����z�Őڑ�Ver
			p->m_nCamID[ ii / (NUM_CAMERA / NUM_CAMLINKSW) ][ ii % (NUM_CAMERA / NUM_CAMLINKSW) ] = ii;
			p->m_nInpCh[ ii / (NUM_CAMERA / NUM_CAMLINKSW) ][ ii % (NUM_CAMERA / NUM_CAMLINKSW) ] = ii % (NUM_CAMERA / NUM_CAMLINKSW);
			p->m_nCamNum[ ii / (NUM_CAMERA / NUM_CAMLINKSW) ]++;
#else								// ���w��ł���΁A�̂Ȃ���̑O�l�߂Őڑ�Ver
			p->m_nCamID[ ii / (MAX_CAMLINKSW_ALLINPUT) ][ ii % (MAX_CAMLINKSW_ALLINPUT) ] = ii;
			p->m_nInpCh[ ii / (MAX_CAMLINKSW_ALLINPUT) ][ ii % (MAX_CAMLINKSW_ALLINPUT) ] = ii % MAX_CAMLINKSW_ALLINPUT;
			p->m_nCamNum[ ii / (MAX_CAMLINKSW_ALLINPUT) ]++;
#endif
		}
	}
	// �C�ӂŕύX
	//p->m_nCamID[0][0] = 4 -1;
	//p->m_nCamID[0][1] = 8 -1;
	//p->m_nCamID[0][2] = 3 -1;
	//p->m_nCamID[0][3] = 7 -1;
	//p->m_nCamID[0][4] = 2 -1;
	//p->m_nCamID[0][5] = 6 -1;
	//p->m_nCamID[0][6] = 1 -1;
	//p->m_nCamID[0][7] = 5 -1;
	
	//p->m_nInpCh[0][0] = 4 -1;
	//p->m_nInpCh[0][1] = 8 -1;
	//p->m_nInpCh[0][2] = 3 -1;
	//p->m_nInpCh[0][3] = 7 -1;
	//p->m_nInpCh[0][4] = 2 -1;
	//p->m_nInpCh[0][5] = 6 -1;
	//p->m_nInpCh[0][6] = 1 -1;
	//p->m_nInpCh[0][7] = 5 -1;

	//p->m_nCamID[1][0] = 12 -1;
	//p->m_nCamID[1][1] = 16 -1;
	//p->m_nCamID[1][2] = 11 -1;
	//p->m_nCamID[1][3] = 15 -1;
	//p->m_nCamID[1][4] = 10 -1;
	//p->m_nCamID[1][5] = 14 -1;
	//p->m_nCamID[1][6] = 9  -1;
	//p->m_nCamID[1][7] = 13 -1;

	//p->m_nInpCh[1][0] = 4 -1;
	//p->m_nInpCh[1][1] = 8 -1;
	//p->m_nInpCh[1][2] = 3 -1;
	//p->m_nInpCh[1][3] = 7 -1;
	//p->m_nInpCh[1][4] = 2 -1;
	//p->m_nInpCh[1][5] = 6 -1;
	//p->m_nInpCh[1][6] = 1 -1;
	//p->m_nInpCh[1][7] = 5 -1;
#endif

	// RS-232C �����ڑ�
	if( ! p->Open(true) ){
		p->Close();
	}

	// ���b�Z�[�W�L���[�ǂݍ���
	pLOG(em_MSG), "[%s] �X���b�h�J�n [0x%X]", p->my_sThreadName, p->GetTid());
	while (1) {
		int wait_num = (p->m_bNowSend) ? EV_END-1 : EV_END;
		DWORD ret = WaitForMultipleObjects(wait_num, hArray, FALSE, INFINITE);

		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// �X���b�h�I��
			pLOG(em_MSG), "[%s] �X���b�h�I�� [0x%X]", p->my_sThreadName, p->GetTid());
			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_RETRY:												// �Đڑ��J�n
			if( ! p->Open(false) ){
				p->Close();
			}
			break;

//-----------------------------------------------------------------------------------------------	
		case EV_SENDEND:											// ���M����
			// �R�}���h�𑗐M�����̂ŁA��M�J�n
			if( ! p->Recv() ){
				p->Close();
				break;
			}
			break;

//-----------------------------------------------------------------------------------------------	
		case EV_RECVEND:											// ��M����
			//// ��������
			//p->m_bNowSend = false;
			CancelWaitableTimer(p->m_evAnserTimeOut);

			//// ��M�f�[�^�`�F�b�N
			if( ! p->CheckDataRecv() ){
				p->Close();
			}
			break;

//-----------------------------------------------------------------------------------------------	
		case EV_ANSERTIMEOUT:										// ���M�A���T�[�^�C���A�E�g
			if(p->m_bIsConnect) {
				pLOG(em_ERR), "[%s] �����`���^�C���A�E�g send=%s", p->my_sThreadName, p->m_cSendBuf);
				syslog(201, "[�����`���^�C���A�E�g:Unit=%d:send=%s]", p->m_nUnit, p->m_cSendBuf);
			}
			p->Close();					// �Đڑ�
			break;

//-----------------------------------------------------------------------------------------------
		case EV_QUE:												// ���[���X���b�g��M
			// �L�[���o��
			COMMON_QUE *pQue;
			pQue = p->gque_Deli.GetFromHead();

			// ��������̏����˗������s����
			if( ! p->SetDataMailCommand(pQue) ) {
				p->Close();
			}
			SAFE_DELETE(pQue);
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
// �����Ǘ�����̃��[���X���b�g�̃R�}���h����
// COMMON_QUE *pQue ���[���X���b�g�f�[�^
// �߂�l ���A��� (false:�ُ�ɂ��N���[�Y���Ă�)
//------------------------------------------
bool CameraManager::SetDataMailCommand(COMMON_QUE *pQue)
{
	// �J�����R���g���[���[�ƒʐM�ł���ꍇ�̂݁A �ȍ~�̏������s
	if( ! m_bIsConnect) {
		LOG(em_WAR), "[%s] ����ؒf��ԂŁA�R�}���h��M", my_sThreadName);
		return true;
	}

	// �C�x���gNo�ɂ�菈������
	switch(pQue->nEventNo) {

//---------------------------------------------------------
		case FACT_TO_CAMERA_01 :							// �p�����[�^�ύX�v��
			// ���I�p�����[�^�ǂݍ���
			GetParam();
			// ���݂̌����\�ʏ�Ԃ̂���Ώۂ������ꍇ�A�J�����R���g���[���[�ɒʒm����
			if( CH_SCODE_ALL   != pQue->fl.data[0] && 
				m_nNowScode[0] != pQue->fl.data[0] &&
				m_nNowScode[1] != pQue->fl.data[0] )   return true;
#ifndef AGCOFF
			// �ʒm����I�����Ԃ�����
			m_typAgcInf.nSetCnt = 0;
			for(int ii=0; ii<m_nMyCamNum; ii++ ) {
				int camid = GetCamIdxToId( ii );
				m_typAgcInf.AgcInfData[ii].camid = camid;
				m_typAgcInf.AgcInfData[ii].nExpTime = m_Param.nInitialExpTime[ m_nNowScode[NUM_TorB(camid)]-1 ][camid]; 

				m_typAgcInf.nSetCnt++;
			}

			if( 0 == m_typAgcInf.nSetCnt ) return true;		// �ꌏ���ݒ��񂪖���������~��
			// ���M����
			m_emMainMode = MODE_RO;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode = 0;
#else
			return true;
#endif
			break;

//---------------------------------------------------------
		case FACT_TO_CAMERA_02 :							// �����p�ݒ�R�}���h
			// PLG���[�h�ݒ�
			if( DIV_PLG_LINE != pQue->fl.data[0] &&
				DIV_PLG_TOP  != pQue->fl.data[0] &&
				DIV_PLG_BOT  != pQue->fl.data[0] &&
				DIV_PLG_TEST != pQue->fl.data[0] )  return false;
			// �������̓p���X��~���̂�
			if(m_bIsPulsu) {
				LOG(em_ERR), "[%s] �p���X�o�͒��͖����ł��B", my_sThreadName);
				return true;
			}

			// �p�����[�^�Ǎ�
			GetParamStatic();
			GetParam();
			// PLG���[�h�擾
			m_emPLGmode = (EM_DIV_PLG)pQue->fl.data[0];
			// ���M����
			m_emMainMode = MODE_START_INIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode = 0;
			break;

//---------------------------------------------------------
		case FACT_TO_CAMERA_03 :							// �p���X�o�͐ݒ�
			if( 0 == pQue->fl.data[0] ) {							// ON
				// ���M����
				m_emMainMode = MODE_PULSE_ON;
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nSwMode = 0;
			} else {												// OFF
				// ���M����
				m_emMainMode = MODE_PULSE_OFF;
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nSwMode = 0;
			}
			break;

//---------------------------------------------------------
		case FACT_TO_CAMERA_04 :							// AGC�ݒ� 
			memset( &m_typAgcInf, 0x00, sizeof(m_typAgcInf));
			if( -1 == pQue->fl.data[0] ) {							// �S�J����
				for(int ii=0; ii<m_nMyCamNum; ii++ ) {
					SetExposureTime( GetCamIdxToId(ii), pQue->fl.data[1], pQue->fl.data[2+ii] );
				}

			} else {
				SetExposureTime( pQue->fl.data[0], pQue->fl.data[1], pQue->fl.data[2] );
			}

			if( 0 == m_typAgcInf.nSetCnt ) return true;		// �ꌏ���ݒ��񂪖���������~��
			// ���M����
			m_emMainMode = MODE_RO;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode = 0;
			break;


//---------------------------------------------------------
		case FACT_TO_CAMERA_05 :							// �R�C���ؑ�
#ifndef AGCOFF
			int nPreScode;									// �O��̌����\�ʏ��No

			// �\�ʏ�ԕύX
			if( 0 == pQue->fl.data[1] ) {							// �\
				nPreScode      = m_nNowScode[0];
				m_nNowScode[0] = pQue->fl.data[0];
				if( 0 > m_nNowScode[0] || MAX_SCODE <= m_nNowScode[0]) m_nNowScode[0] = MAX_SCODE;
			} else {												// ��
				nPreScode      = m_nNowScode[1];
				m_nNowScode[1] = pQue->fl.data[0];
				if( 0 > m_nNowScode[1] || MAX_SCODE <= m_nNowScode[1]) m_nNowScode[1] = MAX_SCODE;
			}

#ifdef VER_SCODE_CHG
			// �\�ʏ�Ԃ̕ω����f(0:�Ȃ��A1:����)
			if(m_Param.bScodeChg && nPreScode == pQue->fl.data[0]) {
				LOG(em_INF), "[%s] �\�ʏ�Ԃ̕ω������̂��߁A�I�����Ԃ̃Z�b�g���� Scode:%d", my_sThreadName, pQue->fl.data[0]);
				return true;		// �\�ʏ�Ԃ��ω������ł���΁A�I�����Ԃ̏����l�̓Z�b�g���Ȃ�
			}
#endif

			// �ʒm����I�����Ԃ�����
			m_typAgcInf.nSetCnt = 0;
			for(int ii=0; ii<m_nMyCamNum; ii++ ) {
				int camid = GetCamIdxToId( ii );

				if( NUM_TorB(camid) != pQue->fl.data[1] ) continue;	// �Y���ʂ���Ȃ������疳��
				m_typAgcInf.AgcInfData[ m_typAgcInf.nSetCnt ].camid = camid;
				m_typAgcInf.AgcInfData[ m_typAgcInf.nSetCnt ].nExpTime = m_Param.nInitialExpTime[ m_nNowScode[NUM_TorB(camid)]-1 ][camid]; 

				m_typAgcInf.nSetCnt++;
			}

			if( 0 == m_typAgcInf.nSetCnt ) return true;		// �ꌏ���ݒ��񂪖���������~��
			// ���M����
			m_emMainMode = MODE_RO;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode = 0;
#else
			return true;
#endif
			break;
		
//---------------------------------------------------------
		case FACT_TO_CAMERA_06 :							// ���I�����Ԗ₢���킹
			// ���I�����Ԃ𓝊��Ǘ��֒ʒm
			Send_ToDefect_AgcVal();
			return true;
			break;

//---------------------------------------------------------
		case FACT_TO_CAMERA_07:								// �Z���p�ݒ�R�}���h
			// �������̓p���X��~���̂�
			if(m_bIsPulsu) {
				LOG(em_ERR), "[CameraManager] �p���X�o�͒��͖����ł��B");
				return true;
			}

			// �p�����[�^�Ǎ�
			GetParamStatic();
			GetParam();
			// PLG���[�h�擾
			m_emPLGmode = DIV_PLG_TEST;								// �e�X�g�p���X�Œ�
			// �e�X�g�p���X�����擾
			m_nTestTime = pQue->fl.data[0]							// �e�X�g�p���X���� [��s�P��]
							/ m_Param.nPlgDiv[m_emPLGmode-1]		// �� ������
							* m_Param.nPlgMul[m_emPLGmode-1];		// �~ ���{��
			// �I�����Ԏ擾
			m_nExpTime = pQue->fl.data[1]							// �I������ [��s�P��]
							* 1000 / 25;							// [25��s�P��] �֕ϊ�
			// ���M����
			m_emMainMode = MODE_CHECK_INIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode = 0;
			break;
		
//---------------------------------------------------------
		case FACT_TO_CAMERA_09 :							// �C��
			memcpy(m_cSendManyu, pQue->fc.data, sizeof(pQue->fc.data));
			// ���M����
			m_emMainMode = MODE_MANYU;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode = 0;
			break;

//---------------------------------------------------------
		default :
			return false;		// �L�蓾�Ȃ�
	}

	// �����܂ł����� �J�����R���g���[���[�ɒʒm
	if( ! CheckNextSend() ) return false;
	return true;
}

//------------------------------------------
// �I�����Ԍ���
// int cam �J�����ԍ� (0�I���W��)
// int mode ���[�h (0:���Βl 1:��Βl)
// int div �l (���Έʒu�̏ꍇ�́A -1:���� 1:�グ 0:���̂܂܁B  ��Βl�̏ꍇ�́A�w��l  0:���̂܂�)
// �߂�l true:�L���f�[�^���Z�b�g�����ꍇ
//------------------------------------------
bool CameraManager::SetExposureTime(int cam, int mode, int div )
{
	int iExp = 0;				// �I�����Ԍv�Z�p
	int scode;					// �w��J�����̕\�ʏ�� (0�I���W��)

	//// �v���̂������J�����ԍ����`�F�b�N
//	if( cam < 0 || NUM_CAMERA <= cam ) {
	if( ! IsCheckMyCamId(cam) ) { 
		LOG(em_ERR), "[%s] AGC�v���̃J�����ԍ����s�� [camera = %d]", my_sThreadName, cam+1);
		syslog(202, "[Unit=%d ID=%d]", m_nUnit, cam+1);
		return false;
	}
	
	//// �f�[�^�Z�b�g
	scode = m_nNowScode[ NUM_TorB(cam) ] - 1;

	//// �I�����Ԍv�Z
	if(0 == mode) {					// ���Βl
		// �グ ����
#ifndef VER_IDULE_CAMERA
		if(1 == div)	   iExp = (int)(m_nNowExpTime[cam] * (1 + m_Param.dAGCstep[ scode ]) + 0.9);	// �؎̂Ă�1�����ɂȂ�\��������ׁA��� �ω��ʂ�1�ȏ�ɂȂ�悤�ɂ���
		else if(-1 == div) iExp = (int)(m_nNowExpTime[cam] * (1 - m_Param.dAGCstep[ scode ]) );
#else
		if(1 == div)	   iExp = (int)(m_nNowExpTime[cam] * (1 + m_Param.dAGCstep[ scode ][cam]) + 0.9);	// �؎̂Ă�1�����ɂȂ�\��������ׁA��� �ω��ʂ�1�ȏ�ɂȂ�悤�ɂ���
		else if(-1 == div) iExp = (int)(m_nNowExpTime[cam] * (1 - m_Param.dAGCstep[ scode ][cam]) );
#endif

	} else if(1==mode) {			// ��Βl
		iExp = div;
	}

	//// �l��0�̎��́A����
	if( 0 == div ) return false;

	//// �������`�F�b�N(�v�Z�����I�����Ԃ��͈͓��H)
	if( iExp < m_Param.nExSyncMinH ) {
		if( m_bAlame[cam] ) {
			m_bAlame[cam] = false;
			LOG(em_ERR), "[%s] �I������ �����l���B [ID=%d] �I[%d]", my_sThreadName, cam+1, iExp);
//			syslog(203, "[Unit=%d ID=%d �I������=%d]", m_nUnit, cam+1, iExp);	// ��ʂɕ\�������ׁA�����H��
		}
#ifdef IS_TO_CAMERA_TBL
		mtbl_pCam->caminf[cam].nMode = -1; 
#endif
		return false;

	} else if( iExp > m_Param.nExSyncLT - m_Param.nExSyncMinL ) {
		if( m_bAlame[cam] ) {
			m_bAlame[cam] = false;
			LOG(em_ERR), "[%s] �I������ ����l���B [ID=%d] �I[%d]", my_sThreadName, cam+1, iExp);
//			syslog(204, "[Unit=%d ID=%d �I������=%d]", m_nUnit, cam+1, iExp);	// ��ʂɕ\�������ׁA�����H��
		}
#ifdef IS_TO_CAMERA_TBL
		mtbl_pCam->caminf[cam].nMode = 1; 
#endif
		return false;
	}

	// ����͈͓���
	m_bAlame[cam] = true;


	//// �I�����ԃZ�b�g
	int cnt = m_typAgcInf.nSetCnt;
	m_typAgcInf.nSetCnt += 1;
	m_typAgcInf.AgcInfData[cnt].camid = cam;
	m_typAgcInf.AgcInfData[cnt].nExpTime = iExp;
	return true;
}

//------------------------------------------
// ���I�����Ԃ𓝊��Ǘ��֑��M
//------------------------------------------
void CameraManager::Send_ToDefect_AgcVal()
{
	COMMON_QUE que;
	que.nEventNo = FACT_TO_DEFECT_14;
	//que.nLineNo = getlineid();
	que.nLineNo = m_nUnit+1;

	for(int ii=0; ii<NUM_CAMERA; ii++ ) {
		que.fl.data[ii] = m_nNowExpTime[ii];
	}
		
	// ���M
	send_mail(TO_DEFECT, ".", &que);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �J�����R���g���[������
//------------------------------------------
// �����[�h���̎擾
// �߂�l ���
//------------------------------------------
// �����[�h���擾
CString CameraManager::GetMainModeName()
{
	return GetMainModeName(m_emMainMode);
}
// �w�胂�[�h���擾
CString CameraManager::GetMainModeName(ENUM_MAINMODE emMainMode)
{
	CString wk;
	switch( emMainMode ) {
		case MODE_NONE:
			wk = "�������";
			break;
		case MODE_OPEN_CHECK:
			wk = "�I�[�v���`�F�b�N";
			break;
		case MODE_START_INIT:
			wk = "������";
			break;
		case MODE_CHECK_INIT:
			wk = "�Z��������";
			break;
		case MODE_WAIT:
			wk = "�ݒ�҂�";
			break;

		case MODE_PULSE_ON:
			wk = "�p���X�o�͊J�n";
			break;
		case MODE_PULSE_OFF:
			wk = "�p���X�o�͒�~";
			break;

		case MODE_RO:
			wk = "�I�����Ԑݒ蒆";
			break;
		case MODE_RETRY:
			wk = "�I�[�v�����g���C��";
			break;
		case MODE_MANYU:
			wk = "�蓮���M��";
			break;
		default :
			wk = "���[�h�ُ�";
			break;
	}
	return wk;
}
//------------------------------------------
// �����M����
// �߂�l false:����ؒf���������{
//------------------------------------------
bool CameraManager::CheckNextSend() 
{
	//// �O����
	memset(m_cSendBuf, 0x00, sizeof(m_cSendBuf));	// ���M�o�b�t�@ �N���A
	//// �T�u���[�h�ύX (�@�했�̐ݒ�����Ă���ꍇ�́A�T�u���[�h���ێ�)
	if(0 == m_nCamMode) m_nSubMode++;

	//// ��������
	switch( m_emMainMode ) {

//---------------------------------------------------------
	////////////////////////////////////
	// �I�[�v���`�F�b�N
	////////////////////////////////////
	case MODE_OPEN_CHECK:
		if( 1==m_nSubMode &&
			0==m_nCamMode ) {
			LOG(em_MSG), "[%s] <%d> �I�[�v���`�F�b�N �J�n", my_sThreadName, m_Param.nSwitchMasterID[m_nSwMode]);
			m_bNowSend = true;												// 1�V�[�P���X�J�n
		}

		sprintf(m_cSendBuf, "%s\r", m_Param.cSwitchMasterID[m_nSwMode]);
		break;


//---------------------------------------------------------
	////////////////////////////////////
	// ������
	////////////////////////////////////
	case MODE_START_INIT:
		if( 1==m_nSubMode &&
			0==m_nCamMode ) {
			LOG(em_MSG), "[%s] <%d> ������ �J�n", my_sThreadName, m_Param.nSwitchMasterID[m_nSwMode]);
			m_bNowSend = true;												// 1�V�[�P���X�J�n
		}

		if		 (1==m_nSubMode) {
//			sprintf(m_cSendBuf, "%sE0\r", m_Param.cSwitchMasterID[m_nSwMode]);											// �p���X�o�͒�~
//			m_bIsPulsu = false;
//
//		} else if(2==m_nSubMode) {
			sprintf(m_cSendBuf, "%sS\r", m_Param.cSwitchMasterID[m_nSwMode]);											// �J�E���^���Z�b�g

		} else if(2==m_nSubMode) {
			sprintf(m_cSendBuf, "%sL%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nExSyncLT);						// LT�ŏ�����

		} else if(3==m_nSubMode) {
			sprintf(m_cSendBuf, "%sM%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgMul[m_emPLGmode-1]);			// �J����PLG���{��

		} else if(4==m_nSubMode) {
			sprintf(m_cSendBuf, "%sD%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgDiv[m_emPLGmode-1]);			// �J����PLG������

		} else if(5==m_nSubMode) {
#if ONLINE_INPUT_C == 1
			sprintf(m_cSendBuf, "%sC0\r", m_Param.cSwitchMasterID[m_nSwMode]);											// �I�����C���P (�g���b�L���O)
#elif ONLINE_INPUT_C == 2
			sprintf(m_cSendBuf, "%sC1\r", m_Param.cSwitchMasterID[m_nSwMode]);											// �I�����C���Q (�J����)
#else
			sprintf(m_cSendBuf, "%sC0\r", m_Param.cSwitchMasterID[m_nSwMode]);											// �I�����C���P (�g���b�L���O)
#endif

		} else if(6==m_nSubMode) {											// I�R�}���h�͓���ch
			sprintf(m_cSendBuf, "%sI1%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgPulse[0]);					// �I�����C��[�g���b�L���O] (�����ETTL)  (4���[�h�́A�J�����R���g���[���[�ɓ��͂���郉�C��PLG�̐M������)

		} else if(7==m_nSubMode) {
			sprintf(m_cSendBuf, "%sI2%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgPulse[1]);					// ��ʍZ�� (�����ETTL)

		} else if(8==m_nSubMode) {
			sprintf(m_cSendBuf, "%sI3%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgPulse[2]);					// ���ʍZ�� (�����ETTL)

		} else if(9==m_nSubMode) {
			sprintf(m_cSendBuf, "%sI0%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgPulse[3]);					// �I�����C��[�J����] (�����ETTL)

		} else if(10==m_nSubMode) {
			sprintf(m_cSendBuf, "%sN%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgTestInterval);				// �e�X�g�p���X����

		} else if(11==m_nSubMode) {
			sprintf(m_cSendBuf, "%sU%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgIsou[m_emPLGmode-1]);		// �ʑ��ݒ� (�Q���E�P��)

		} else if(12==m_nSubMode) {
			sprintf(m_cSendBuf, "%sR%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgIsouSa[m_emPLGmode-1]);		// �ʑ����ݒ� (�A�b�v�J�E���g�E�_�E���J�E���g)

			// ���̃T�u���[�h�ŃJ�������̐ݒ�����邽�߂̏���
			m_nCamMode = 0;

		} else if(13==m_nSubMode) {											// �I�����ԕύX
#ifndef AGCOFF
#ifndef VER_AVALDATA
			sprintf(m_cSendBuf, "%sA%c%d\r",
				m_Param.cSwitchMasterID[m_nSwMode],
				GetChannelNo( m_nCamID[m_nCamMode] ),
				//m_Param.nInitialExpTime[ m_nNowScode[NUM_TorB(ii)]-1 ][ m_nCamMode ] );	// ���̌����\�ʏ�Ԃ��g��
				m_Param.nInitialExpTime[ MAX_SCODE-1 ][ m_nCamMode ] );	// �f�t�H���g�����\�ʏ�Ԃ��g��

			// ���݂̘I�����Ԃ�ύX
			m_nNowExpTime[m_nCamMode] = m_Param.nInitialExpTime[ MAX_SCODE-1 ][ m_nCamMode ];
#ifdef IS_TO_CAMERA_TBL
			mtbl_pCam->caminf[m_nCamMode].nExpTime = m_nNowExpTime[m_nCamMode];
			mtbl_pCam->caminf[m_nCamMode].nMode = 0; 
#endif

			// �J�������̃��[�h�ݒ�
			if( m_nCamMode == NUM_CAMERA-1 ) m_nCamMode = 0;				// �Ō�̃J������
			else							 m_nCamMode++;
#else
			sprintf(m_cSendBuf, "%sA%c%d\r",
				m_Param.cSwitchMasterID[m_nSwMode],
				GetChannelNo( m_nInpCh[m_nSwMode][m_nCamMode] ),
				//m_Param.nInitialExpTime[ m_nNowScode[NUM_TorB(ii)]-1 ][ m_nCamID[m_nSwMode][m_nCamMode] ] );			// ���̌����\�ʏ�Ԃ��g��
				m_Param.nInitialExpTime[ MAX_SCODE-1 ][ m_nCamID[m_nSwMode][m_nCamMode] ] );							// �f�t�H���g�����\�ʏ�Ԃ��g��

			// ���݂̘I�����Ԃ�ύX
			m_nNowExpTime[ m_nCamID[m_nSwMode][m_nCamMode] ] = m_Param.nInitialExpTime[ MAX_SCODE-1 ][ m_nCamID[m_nSwMode][m_nCamMode] ];
#ifdef IS_TO_CAMERA_TBL
			mtbl_pCam->caminf[ m_nCamID[m_nSwMode][m_nCamMode] ].nExpTime = m_nNowExpTime[ m_nCamID[m_nSwMode][m_nCamMode] ];
			mtbl_pCam->caminf[ m_nCamID[m_nSwMode][m_nCamMode] ].nMode = 0; 
#endif

			// �J�������̃��[�h�ݒ�
			if( m_nCamMode == m_nCamNum[m_nSwMode]-1 )	m_nCamMode = 0;		// �Ō�̃J������
			else										m_nCamMode++;
#endif
#else
			// �I�����Ԃ̏����l��ύX ----->>>
#ifndef VER_AVALDATA
			sprintf(m_cSendBuf, "%sA%c%d\r",
				m_Param.cSwitchMasterID[m_nSwMode],
				GetChannelNo(m_nCamID[m_nCamMode]),
				m_Param.nExSyncMinH);							// �I�����Ԃ̍ŏ��l��ݒ�
			// �J�������̃��[�h�ݒ�
			if (m_nCamMode == NUM_CAMERA - 1) m_nCamMode = 0;				// �Ō�̃J������
			else							 m_nCamMode++;
#else
			sprintf(m_cSendBuf, "%sA%c%d\r",
				m_Param.cSwitchMasterID[m_nSwMode],
				GetChannelNo(m_nInpCh[m_nSwMode][m_nCamMode]),
				m_Param.nExSyncMinH);							// �I�����Ԃ̍ŏ��l��ݒ�
			// �J�������̃��[�h�ݒ�
			if (m_nCamMode == m_nCamNum[m_nSwMode] - 1)	m_nCamMode = 0;		// �Ō�̃J������
			else										m_nCamMode++;
#endif
			//sprintf(m_cSendBuf, "%s\r", m_Param.cSwitchMasterID[m_nSwMode]);											// �T�u���[�h��ς��Ȃ��ׁA���������R�}���h�𑗐M���Ă���
			// �I�����Ԃ̏����l��ύX -----<<<
#endif

		} else if(14==m_nSubMode) {											// PLG�敪�ύX (T�R�}���h�͏o��ch)
			if(0==m_nSwMode) {												// �f�C�W�[�`�F�[���ڑ���1���
				if( DIV_PLG_LINE == m_emPLGmode ) {							// ���C��
					sprintf(m_cSendBuf, "%sT0\r", m_Param.cSwitchMasterID[m_nSwMode]);
				} else if( DIV_PLG_TOP == m_emPLGmode ) {					// �\�Z��
					sprintf(m_cSendBuf, "%sT1\r", m_Param.cSwitchMasterID[m_nSwMode]);
				} else if( DIV_PLG_BOT == m_emPLGmode ) {					// ���Z��
					sprintf(m_cSendBuf, "%sT2\r", m_Param.cSwitchMasterID[m_nSwMode]);
				} else {													// �e�X�g
					sprintf(m_cSendBuf, "%sT3\r", m_Param.cSwitchMasterID[m_nSwMode]);
				}
			} else {														// �f�C�W�[�`�F�[���ڑ���2��ڈȍ~ (�� 1��ڂ�PLG���͂��g�p)
				sprintf(m_cSendBuf, "%sT4\r", m_Param.cSwitchMasterID[m_nSwMode]);
			}

		} else if(15==m_nSubMode) {
			if(0 >= m_nFilter || 99999 < m_nFilter) {
				sprintf(m_cSendBuf, "%s\r", m_Param.cSwitchMasterID[m_nSwMode]);
			} else {
				sprintf(m_cSendBuf, "%sF%05d\r", m_Param.cSwitchMasterID[m_nSwMode], m_nFilter);						// �t�B���^�[[0.1��s] (�J�E���^�[�{�[�h�Ɠ����t�B���^���� 0.8��s ���Z�b�g)
			}

		} else if(16==m_nSubMode) {
			if(0 >= m_nH) {
				sprintf(m_cSendBuf, "%s\r", m_Param.cSwitchMasterID[m_nSwMode]);
			} else {
				sprintf(m_cSendBuf, "%sH%01d\r", m_Param.cSwitchMasterID[m_nSwMode], m_nH);
			}


		} else if(17==m_nSubMode) {					// ���ꂩ��́A����ݸ�����Ɋ֌W�����̐ݒ�B
			if(0 >= m_nCmdIniInputNum) {
				sprintf(m_cSendBuf, "%s\r", m_Param.cSwitchMasterID[m_nSwMode]);
			} else {
				sprintf(m_cSendBuf, "%s\r", m_lstCmdIniInput[m_nCamMode], m_nH);
			}
			// �J�������[�h���g���āA�C�ӃR�}���h�̑��M�����𑗂�
			if( m_nCamMode >= m_nCmdIniInputNum-1 ) m_nCamMode = 0;		// �Ō�̃J������
			else								    m_nCamMode++;

		} else if(18==m_nSubMode) {

			// �������� �� �ݒ�҂�	
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode = 0;
			m_bNowSend = false;												// 1�V�[�P���X���튮��

		} else {
			LOG(em_ERR), "[%s] <%d> �T�u���[�h�ُ� [mode = ������, sub_mode = %d]", my_sThreadName, m_Param.nSwitchMasterID[m_nSwMode], m_nSubMode);
			syslog(200, "Unit=%d [<%d> mode = ������ sub_mode = %d]", m_nUnit, m_Param.nSwitchMasterID[m_nSwMode], m_nSubMode);
			m_bNowSend = false;												// 1�V�[�P���X�͋����I��
			return false;
		}
		break;
		
//---------------------------------------------------------
	////////////////////////////////////
	// �Z��������
	////////////////////////////////////
	case MODE_CHECK_INIT:
		if( 1==m_nSubMode &&
			0==m_nCamMode ) {
			LOG(em_MSG), "[CameraManager] <%d> �Z�������� �J�n", m_Param.nSwitchMasterID[m_nSwMode]);
			m_bNowSend = true;												// 1�V�[�P���X�J�n
		}

		if		 (1==m_nSubMode) {
			sprintf(m_cSendBuf, "%sS\r", m_Param.cSwitchMasterID[m_nSwMode]);											// �J�E���^���Z�b�g

		} else if(2==m_nSubMode) {
			sprintf(m_cSendBuf, "%sL%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nExSyncLT);						// LT�ŏ�����
		
		} else if(3==m_nSubMode) {
			sprintf(m_cSendBuf, "%sM%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgMul[m_emPLGmode-1]);			// �J����PLG���{��
		
		} else if(4==m_nSubMode) {
			sprintf(m_cSendBuf, "%sD%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgDiv[m_emPLGmode-1]);			// �J����PLG������
		
		} else if(5==m_nSubMode) {
#if ONLINE_INPUT_C == 1
			sprintf(m_cSendBuf, "%sC0\r", m_Param.cSwitchMasterID[m_nSwMode]);											// �I�����C���P (�g���b�L���O)
#elif ONLINE_INPUT_C == 2
			sprintf(m_cSendBuf, "%sC1\r", m_Param.cSwitchMasterID[m_nSwMode]);											// �I�����C���Q (�J����)
#else
			sprintf(m_cSendBuf, "%sC0\r", m_Param.cSwitchMasterID[m_nSwMode]);											// �I�����C���P (�g���b�L���O)
#endif

		} else if(6==m_nSubMode) {											// I�R�}���h�͓���ch
			sprintf(m_cSendBuf, "%sI1%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgPulse[0]);					// �I�����C��[�g���b�L���O] (�����ETTL)  (4���[�h�́A�J�����R���g���[���[�ɓ��͂���郉�C��PLG�̐M������)

		} else if(7==m_nSubMode) {
			sprintf(m_cSendBuf, "%sI2%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgPulse[1]);					// ��ʍZ�� (�����ETTL)

		} else if(8==m_nSubMode) {
			sprintf(m_cSendBuf, "%sI3%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgPulse[2]);					// ���ʍZ�� (�����ETTL)

		} else if(9==m_nSubMode) {
			sprintf(m_cSendBuf, "%sI0%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgPulse[3]);					// �I�����C��[�J����] (�����ETTL)

		} else if(10==m_nSubMode) {											// �e�X�g�p���X����
			sprintf(m_cSendBuf, "%sN%d\r",
				m_Param.cSwitchMasterID[m_nSwMode],
				m_nTestTime );

			// ���݂̃e�X�g�p���X������ύX (�� �p�����[�^��ύX)
			m_Param.nPlgTestInterval = m_nTestTime;

		} else if(11==m_nSubMode) {
			sprintf(m_cSendBuf, "%sU%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgIsou[m_emPLGmode-1]);		// �ʑ��ݒ� (�Q���E�P��)

		} else if(12==m_nSubMode) {
			sprintf(m_cSendBuf, "%sR%d\r", m_Param.cSwitchMasterID[m_nSwMode], m_Param.nPlgIsouSa[m_emPLGmode-1]);		// �ʑ����ݒ� (�A�b�v�J�E���g�E�_�E���J�E���g)

			// ���̃T�u���[�h�ŃJ�������̐ݒ�����邽�߂̏���
			m_nCamMode = 0;

		} else if(13==m_nSubMode) {											// �I�����ԕύX
			sprintf(m_cSendBuf, "%sA%c%d\r",
				m_Param.cSwitchMasterID[m_nSwMode],
				GetChannelNo( m_nInpCh[m_nSwMode][m_nCamMode] ),
				m_nExpTime );

			// ���݂̘I�����Ԃ�ύX
			m_nNowExpTime[ m_nCamID[m_nSwMode][m_nCamMode] ] = m_nExpTime;
#ifdef IS_TO_CAMERA_TBL
			mtbl_pCam->caminf[m_nCamMode].nExpTime = m_nNowExpTime[m_nCamMode];
			mtbl_pCam->caminf[m_nCamMode].nMode = 0; 
#endif

			// �J�������̃��[�h�ݒ�
			if( m_nCamMode == m_nCamNum[m_nSwMode]-1 )	m_nCamMode = 0;		// �Ō�̃J������
			else										m_nCamMode++;

		} else if(14==m_nSubMode) {											// PLG�敪�ύX (T�R�}���h�͏o��ch)
			if( 0 == m_nSwMode ) {											// �f�C�W�[�`�F�[���ڑ���1���
				if( DIV_PLG_LINE == m_emPLGmode ) {							// ���C��
					sprintf(m_cSendBuf, "%sT0\r", m_Param.cSwitchMasterID[m_nSwMode]);
				} else if( DIV_PLG_TOP == m_emPLGmode ) {					// �\�Z��
					sprintf(m_cSendBuf, "%sT1\r", m_Param.cSwitchMasterID[m_nSwMode]);
				} else if( DIV_PLG_BOT == m_emPLGmode ) {					// ���Z��
					sprintf(m_cSendBuf, "%sT2\r", m_Param.cSwitchMasterID[m_nSwMode]);
				} else {													// �e�X�g
					sprintf(m_cSendBuf, "%sT3\r", m_Param.cSwitchMasterID[m_nSwMode]);
				}
			} else {														// �f�C�W�[�`�F�[���ڑ���2��ڈȍ~ (�� 1��ڂ�PLG���͂��g�p)
				sprintf(m_cSendBuf, "%sT4\r", m_Param.cSwitchMasterID[m_nSwMode]);
			}

		} else if(15==m_nSubMode) {
			if(0 >= m_nFilter || 99999 < m_nFilter) {
				sprintf(m_cSendBuf, "%s\r", m_Param.cSwitchMasterID[m_nSwMode]);
			} else {
				sprintf(m_cSendBuf, "%sF%05d\r", m_Param.cSwitchMasterID[m_nSwMode], m_nFilter);						// �t�B���^�[[0.1��s] (�J�E���^�[�{�[�h�Ɠ����t�B���^���� 0.8��s ���Z�b�g)
			}

		} else if(16==m_nSubMode) {
			if(0 >= m_nH) {
				sprintf(m_cSendBuf, "%s\r", m_Param.cSwitchMasterID[m_nSwMode]);
			} else {
				sprintf(m_cSendBuf, "%sH%01d\r", m_Param.cSwitchMasterID[m_nSwMode], m_nH);
			}


		} else if(17==m_nSubMode) {					// ���ꂩ��́A����ݸ�����Ɋ֌W�����̐ݒ�B
			if(0 >= m_nCmdIniInputNum) {
				sprintf(m_cSendBuf, "%s\r", m_Param.cSwitchMasterID[m_nSwMode]);
			} else {
				sprintf(m_cSendBuf, "%s\r", m_lstCmdIniInput[m_nCamMode], m_nH);
			}
			// �J�������[�h���g���āA�C�ӃR�}���h�̑��M�����𑗂�
			if( m_nCamMode >= m_nCmdIniInputNum-1 ) m_nCamMode = 0;		// �Ō�̃J������
			else								    m_nCamMode++;

			// �J���������N�X�C�b�`���̃��[�h�ݒ�
			if( m_nSwMode != NUM_CAMLINKSW-1 ) {							// �Ō�̃J���������N�X�C�b�`�ȊO��
				m_nSubMode = 0;
				m_nCamMode = 0;
			}
			m_nSwMode++;

		} else if(18==m_nSubMode) {

			// �������� �� �ݒ�҂�	
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode = 0;
			m_bNowSend = false;												// 1�V�[�P���X���튮��

		} else {
			LOG(em_ERR), "[CameraManager] <%d> �T�u���[�h�ُ� [mode = �Z��������, sub_mode = %d]", m_Param.nSwitchMasterID[m_nSwMode], m_nSubMode);
			syslog(200, "[<%d> mode = �Z�������� sub_mode = %d]", m_Param.nSwitchMasterID[m_nSwMode], m_nSubMode);
			m_bNowSend = false;												// 1�V�[�P���X�͋����I��
			return false;
		}
		break;

//---------------------------------------------------------
	////////////////////////////////////
	// �p���X�o�͊J�n
	////////////////////////////////////
	case MODE_PULSE_ON:
		if( 1==m_nSubMode &&
			0==m_nCamMode ) {
			LOG(em_MSG), "[%s] �p���X�o�� �J�n", my_sThreadName);
			m_bNowSend = true;												// 1�V�[�P���X�J�n
		}

		if		 (1==m_nSubMode) {
			if(0==m_nSwMode) {												// �f�C�W�[�`�F�[���ڑ���1���
				sprintf(m_cSendBuf, "%sS\r", m_Param.cSwitchMasterID[m_nSwMode]);										// �J�E���^���Z�b�g
			} else {														// �f�C�W�[�`�F�[���ڑ���2��ڈȍ~
				sprintf(m_cSendBuf, "%s\r", m_Param.cSwitchMasterID[m_nSwMode]);
			}

		} else if(2==m_nSubMode) {
			if(0==m_nSwMode) {												// �f�C�W�[�`�F�[���ڑ���1���
				sprintf(m_cSendBuf, "%sE1\r", m_Param.cSwitchMasterID[m_nSwMode]);										// �p���X�o�͊J�n
			} else {														// �f�C�W�[�`�F�[���ڑ���2��ڈȍ~
				sprintf(m_cSendBuf, "%s\r", m_Param.cSwitchMasterID[m_nSwMode]);
			}

		} else if(3==m_nSubMode) {

			// �p���X�o�͊J�n�� �� �ݒ�҂�	
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode = 0;
			m_bIsPulsu = true;
			m_bNowSend = false;												// 1�V�[�P���X���튮��

		} else {
			LOG(em_ERR), "[%s] <%d> �T�u���[�h�ُ� [mode = �p���X�o�͊J�n, sub_mode = %d]", my_sThreadName, m_Param.nSwitchMasterID[m_nSwMode], m_nSubMode);
			syslog(200, "Unit=%d [<%d> mode = �p���X�o�͊J�n sub_mode = %d]", m_nUnit, m_Param.nSwitchMasterID[m_nSwMode], m_nSubMode);
			m_bNowSend = false;												// 1�V�[�P���X�͋����I��
			return false;
		}
		break;

//---------------------------------------------------------
	////////////////////////////////////
	// �p���X�o�͒�~
	////////////////////////////////////
	case MODE_PULSE_OFF:
		if( 1==m_nSubMode &&
			0==m_nCamMode ) {
			LOG(em_MSG), "[%s] �p���X��~ �J�n", my_sThreadName);
			m_bNowSend = true;												// 1�V�[�P���X�J�n
		}

		if		 (1==m_nSubMode) {
			if(0==m_nSwMode) {												// �f�C�W�[�`�F�[���ڑ���1���
				sprintf(m_cSendBuf, "%sE0\r", m_Param.cSwitchMasterID[m_nSwMode]);										// �p���X�o�͒�~
			} else {														// �f�C�W�[�`�F�[���ڑ���2��ڈȍ~
				sprintf(m_cSendBuf, "%s\r", m_Param.cSwitchMasterID[m_nSwMode]);
			}

		} else if(2==m_nSubMode) {

			// �p���X�o�͒�~�� �� �ݒ�҂�	
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode = 0;
			m_bIsPulsu = false;
			m_bNowSend = false;												// 1�V�[�P���X���튮��

		} else {
			LOG(em_ERR), "[%s] <%d> �T�u���[�h�ُ� [mode = �p���X�o�͒�~, sub_mode = %d]", my_sThreadName, m_Param.nSwitchMasterID[m_nSwMode], m_nSubMode);
			syslog(200, "Unit=%d [<%d> mode = �p���X�o�͒�~ sub_mode = %d]", m_nUnit, m_Param.nSwitchMasterID[m_nSwMode], m_nSubMode);
			m_bNowSend = false;												// 1�V�[�P���X�͋����I��
			return false;
		}
		break;

//---------------------------------------------------------
	////////////////////////////////////
	// �I�����Ԑݒ蒆
	////////////////////////////////////
	case MODE_RO:
		if( 1==m_nSubMode &&
			0==m_nCamMode ) {
			LOG(em_MSG), "[%s] <%d> �I�����ԕύX �J�n", my_sThreadName, m_Param.nSwitchMasterID[m_nSwMode]); 
			m_bNowSend = true;												// 1�V�[�P���X�J�n
		}

		if		 (1==m_nSubMode) {											// �I�����ԕύX
			int id = m_typAgcInf.AgcInfData[ m_nCamMode ].camid;			// �J����ID
			int val= m_typAgcInf.AgcInfData[ m_nCamMode ].nExpTime;			// �I������

#ifndef VER_AVALDATA
			sprintf(m_cSendBuf, "%sA%c%d\r", m_Param.cSwitchMasterID[m_nSwMode], GetChannelNo( m_nCamID[ id ] ), val );

			// ���݂̘I�����Ԃ�ύX
			m_nNowExpTime[id] = val;
#ifdef IS_TO_CAMERA_TBL
			mtbl_pCam->caminf[id].nExpTime = val;
			mtbl_pCam->caminf[id].nMode = 0; 
#endif
#else
			//// ���Y�J����ID���J���������N�X�C�b�`�ɐڑ�����Ă��邩�`�F�b�N
			for(int ii=0; ii<m_nCamNum[m_nSwMode]; ii++) {
				if( m_nCamID[m_nSwMode][ii] == id ) {
					sprintf(m_cSendBuf, "%sA%c%d\r",
						m_Param.cSwitchMasterID[m_nSwMode],
						GetChannelNo( m_nInpCh[m_nSwMode][ ii ] ),
						val);

					// ���݂̘I�����Ԃ�ύX
					m_nNowExpTime[ ii ] = val;
				}
			}
			if( 0 == strlen(m_cSendBuf) ) {
				sprintf(m_cSendBuf, "%s\r",
					m_Param.cSwitchMasterID[m_nSwMode]);
			}
#ifdef IS_TO_CAMERA_TBL
			mtbl_pCam->caminf[id].nExpTime = val;
			mtbl_pCam->caminf[id].nMode = 0; 
#endif
#endif

			// �J�������̃��[�h�ݒ�
			if( m_nCamMode == m_typAgcInf.nSetCnt-1 ) m_nCamMode = 0;		// �Ō�̃J������
			else									  m_nCamMode++;

		} else if(2==m_nSubMode) {

			// �I�����Ԑݒ蒆 �� �ݒ�҂�	
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode = 0;
			m_bNowSend = false;												// 1�V�[�P���X���튮��

		} else {
			LOG(em_ERR), "[%s] <%d> �T�u���[�h�ُ� [mode = �I�����Ԑݒ蒆, sub_mode = %d]", my_sThreadName, m_Param.nSwitchMasterID[m_nSwMode], m_nSubMode);
			syslog(200, "Unit=%d [<%d> mode = �I�����Ԑݒ蒆 sub_mode = %d]", m_nUnit, m_Param.nSwitchMasterID[m_nSwMode], m_nSubMode);
			m_bNowSend = false;												// 1�V�[�P���X�͋����I��
			return false;
		}
		break;

//---------------------------------------------------------
	////////////////////////////////////
	// �C�ӃR�}���h
	case MODE_MANYU:
		if		 (1==m_nSubMode) {
			m_bNowSend = true;
			// �z���g�ɑS�������Ŏ�ł�
			//sprintf(m_cSendBuf, "%s%s\r", m_Param.cSwitchMasterID[m_nSwMode], m_cSendManyu);
			sprintf(m_cSendBuf, "%s\r", m_cSendManyu);

		} else {
			m_emMainMode = MODE_WAIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode = 0;
			m_bNowSend = false;
		}
		break;

//---------------------------------------------------------
	default:
		LOG(em_ERR), "[%s] <%d> ���[�h�ُ� [MainMode = %d]", my_sThreadName, m_Param.nSwitchMasterID[m_nSwMode], m_emMainMode);
		syslog(200, "Unit=%d [<%d> mode = %d]", m_nUnit, m_Param.nSwitchMasterID[m_nSwMode], m_emMainMode);
		m_bNowSend = false;													// 1�V�[�P���X�͋����I��
		return false;
	}

	//// ���M
	if( 0==strlen(m_cSendBuf) ) return true;								// ���M�f�[�^�����̏ꍇ�́A�����Ԃ��B
	if( !Send() ) return false;
	return true;
}

//------------------------------------------
// ��M�`���`�F�b�N
// �߂�l false:����ؒf���������{
//------------------------------------------
bool CameraManager::CheckDataRecv() 
{
	//LOG(em_INF), "[%s] �����`����M [%s]", my_sThreadName, m_cRecvBuf);

	////////////////////////////////////
	// �R�}���h���ߎ��s
	////////////////////////////////////
	if ( 0 == memcmp(m_cRecvBuf, R_NG, strlen(m_cRecvBuf)) ) {
		LOG(em_ERR), "[%s] ��M�`�� �ُ� [���ߎ��s:send=%s]", my_sThreadName, m_cSendBuf);
		syslog(201, "Unit=%d [���ߎ��s:send=%s]", m_nUnit, m_cSendBuf);
		m_bNowSend = false;													// 1�V�[�P���X�͋����I��
		return false;
	

	////////////////////////////////////
	// �R�}���h����
	////////////////////////////////////
	} else if ( 0 == memcmp(m_cRecvBuf, R_NOTHING, strlen(m_cRecvBuf)) ) {
		// �I�[�v���`�F�b�N���̂� ����F���Ƃ���
		if( CameraManager::MODE_OPEN_CHECK == m_emMainMode ) {
#ifdef VER_AVALDATA
			//// �J���������N�X�C�b�`���̃��[�h�ݒ�
			// �I�[�v���`�F�b�N��
			if( 1 == m_nSubMode &&
				m_nSwMode != NUM_CAMLINKSW-1 )	{							// �Ō�̃J���������N�X�C�b�`�ȊO��
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nSwMode++;

				// ���̑��M�f�[�^���� & ���M
				if( ! CheckNextSend() ) {
					return false;
				}
				return true;
			}
#endif

			// �ڑ�����
			LOG(em_MSG), "[%s] �J�����R���g���[����H �ڑ��m�F����", my_sThreadName);
			syslog(205, "Unit=%d", m_nUnit);
			m_bIsConnect = true;

			CString sWk;
			sWk.Format("CAMCONTROL_ID_%d", m_nUnit + 1);
			int id = GetPrivateProfileInt(INI_STATUS, sWk, -1, TASKINI_NAME);
			if( -1 == id ) {
				KizuFunction::SetStatus("CAMCONTROL_ID", true);
			}
			else {
				KizuFunction::SetStatus(sWk, true);
			}


			// ���������� �J�n
			m_emMainMode = CameraManager::MODE_START_INIT;
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode = 0;
			if( ! CheckNextSend() ) {
				return false;
			}
		
		// �����������M����������������A�m�F�ׂ̈����Ȃ̂Ŗ������ɂn�j�Ƃ���
		} else if(strcmp("\r", m_cSendBuf)) {
#ifdef VER_AVALDATA
			//// �J���������N�X�C�b�`���̃��[�h�ݒ�
			// ��������
			if( CameraManager::MODE_START_INIT == m_emMainMode &&
				16 == m_nSubMode &&
				m_nSwMode != NUM_CAMLINKSW-1 ) {							// �Ō�̃J���������N�X�C�b�`�ȊO��
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nSwMode++;
			}
			// �I�����Ԑݒ蒆��
			if( CameraManager::MODE_RO == m_emMainMode &&
				1 == m_nSubMode &&
				0 == m_nCamMode &&											// �Ō�̃J������
				m_nSwMode != NUM_CAMLINKSW-1 ) {							// �Ō�̃J���������N�X�C�b�`�ȊO��
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nSwMode++;
			}
			// �Z����������
			if( CameraManager::MODE_CHECK_INIT == m_emMainMode &&
				16 == m_nSubMode &&
				m_nSwMode != NUM_CAMLINKSW-1 ) {							// �Ō�̃J���������N�X�C�b�`�ȊO��
				m_nSubMode = 0;
				m_nCamMode = 0;
				m_nSwMode++;
			}
#endif

			// ���̑��M�f�[�^���� & ���M
			if( ! CheckNextSend() ) {
				return false;
			}
		

		// �ʏ�́A�ُ�F��
		} else {
			LOG(em_ERR), "[%s] ��M�`�� �ُ� [�R�}���h����:send=%s]", my_sThreadName, m_cSendBuf);
			syslog(201, "Unit=%d [�R�}���h����:send=%s]", m_nUnit, m_cSendBuf);
			m_bNowSend = false;												// 1�V�[�P���X�͋����I��
			return false;
		}


	////////////////////////////////////
	// �R�}���h����
	////////////////////////////////////
	} else if ( 0 == memcmp(m_cRecvBuf, R_OK, strlen(m_cRecvBuf)) ) {
#ifdef VER_AVALDATA
		//// �J���������N�X�C�b�`���̃��[�h�ݒ�
		// ��������
		if( CameraManager::MODE_START_INIT == m_emMainMode &&
			16 == m_nSubMode &&
			m_nSwMode != NUM_CAMLINKSW-1 ) {								// �Ō�̃J���������N�X�C�b�`�ȊO��
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode++;
		}
		// �I�����Ԑݒ蒆��
		if( CameraManager::MODE_RO == m_emMainMode &&
			1 == m_nSubMode &&
			0 == m_nCamMode &&												// �Ō�̃J������
			m_nSwMode != NUM_CAMLINKSW-1 ) {								// �Ō�̃J���������N�X�C�b�`�ȊO��
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode++;
		}
		// �Z����������
		if( CameraManager::MODE_CHECK_INIT == m_emMainMode &&
			16 == m_nSubMode &&
			m_nSwMode != NUM_CAMLINKSW-1 ) {								// �Ō�̃J���������N�X�C�b�`�ȊO��
			m_nSubMode = 0;
			m_nCamMode = 0;
			m_nSwMode++;
		}
#endif
		
		// ���̑��M�f�[�^���� & ���M
		if( ! CheckNextSend() ) {
			return false;
		}


	////////////////////////////////////
	// �R�}���h�s��
	////////////////////////////////////
	} else {																// �s��
		LOG(em_ERR), "[%s] ��M�`�� �ُ� [�s��:recv=%s]", my_sThreadName, m_cRecvBuf);
		syslog(201, "Unit=%d [�s��:recv=%s]", m_nUnit, m_cRecvBuf);
		m_bNowSend = false;													// 1�V�[�P���X�͋����I��	
		return false; 
	}
	
	return true;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 232C�ʐM�֘A

//------------------------------------------
// 232C �I�[�v��
// bool bLogDsp false:���O�o�͖���
//------------------------------------------
bool CameraManager::Open(bool bLogDsp) 
{

	// Rs232cBase�C���X�^���X����
	try{
		// �J�����R���g���[����PC�Ԃ́A��������q�����Ă��Ȃ��ׁA�\�t�g�t���[����𖳌��Ƃ���
		mcls_p232c = new Rs232cBase(m_cMySession, TASKINI_NAME, GetTid(), PARAM_232C_RECVBUF_SIZE, PARAM_232C_SENDBUF_SIZE, WM_USER, true);
		mcls_p232c->SetRecvEndCR(CR);  // �I�[�R�[�h�`�F�b�N�ɉϒ��ɓǍ��݂Ƃ���

	} catch(int code) {
		mcls_p232c = NULL;
		if(bLogDsp) {
			LOG(em_ERR), "[%s] 232C�I�[�v���G���[(err_code = %d)", my_sThreadName, code);
			syslog(SYSNO_232C_NEW_ERR, "Unit=%d [err_code = %d]", m_nUnit, code);
		}
		return false;
	}
	
	// �ڑ�����
	memset(m_cRecvBuf, 0x00, sizeof(m_cRecvBuf));
	memset(m_cSendBuf, 0x00, sizeof(m_cSendBuf));
//	syslog(SYSNO_232C_OPEN, "Unit=%d", m_nUnit);

	// �J�����R���g���[���[�ƒʐM�ł��邩�m�F
	m_emMainMode = MODE_OPEN_CHECK;
	m_nSubMode = 0;
	m_nCamMode = 0;
	m_nSwMode = 0;
	if( ! CheckNextSend() ) return false;
	return true;
}

//------------------------------------------
// 232C �N���[�Y
//------------------------------------------
void CameraManager::Close() 
{
	// �S�^�C�}��~
	CancelWaitableTimer(m_evReTry);
	CancelWaitableTimer(m_evAnserTimeOut);

	// �N���[�Y
	if(NULL != mcls_p232c) {
		SAFE_DELETE(mcls_p232c);
	}

	//// ���g���C�̂��тɃ��O�o�͂����̋֎~
	// ��x�q�������ȍ~����̏o��
	if(m_bIsConnect) {
		LOG(em_MSG), "[%s] 232C�N���[�Y", my_sThreadName);
		syslog(SYSNO_232C_CLOSE, "Unit=%d", m_nUnit);
		CString sWk;
		sWk.Format("CAMCONTROL_ID_%d", m_nUnit + 1);
		KizuFunction::SetStatus(sWk, true);
		int id = GetPrivateProfileInt(INI_STATUS, sWk, -1, TASKINI_NAME);
		if( -1 == id ) {
			KizuFunction::SetStatus("CAMCONTROL_ID", false);
		}
		else {
			KizuFunction::SetStatus(sWk, false);
		}
	}

	// �X�e�[�^�X������
	m_bIsConnect = false;
	m_bNowSend = false;
	m_emMainMode = MODE_RETRY;
	m_nSubMode = 0;
	m_nCamMode = 0;
	m_nSwMode = 0;

	// ���g���C�^�C�} �Z�b�g
	LARGE_INTEGER ts;
	ts.QuadPart = (__int64)PARAM_TIMEOUT_RETRY * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evReTry, &ts, 0, NULL, NULL, FALSE);
}

//------------------------------------------
// 232C �񓯊� ��M
//------------------------------------------
bool CameraManager::Recv() 
{
	if(NULL == mcls_p232c) return true;

	// ��M�o�b�t�@�N���A
	memset(m_cRecvBuf, 0x00, sizeof(m_cRecvBuf));

	// �I�[�R�[�h������ �V�O�i����Ԃ��^�C�v
	int iRetc = mcls_p232c->Recv(m_cRecvBuf, 1, m_evRecvEnd );		// �ϒ��ǂݍ��ׁ݂̈A1�o�C�g�Œ�
	if(0 != iRetc) {
		// ��M�G���[
		LOG(em_ERR), "[%s] 232C��M�G���[(err_code = %d)", my_sThreadName, iRetc);
		syslog(SYSNO_232C_RECV_ERR, "Unit=%d [err_code = %d]", m_nUnit, iRetc);
		return false;
	}
	return true;
}

//------------------------------------------
// 232C �񓯊� ���M
//------------------------------------------
bool CameraManager::Send() 
{
	if(NULL == mcls_p232c) return true;

	//// ���M
	LOG(em_INF), "[%s] �R�}���h���M [%s]", my_sThreadName, m_cSendBuf);
	int iRetc = mcls_p232c->Send(m_cSendBuf, (DWORD)strlen(m_cSendBuf), m_evSendEnd);
	if(0 != iRetc) {
		// ��M�G���[
		LOG(em_ERR), "[%s] 232C���M�G���[(err_code = %d)", my_sThreadName, iRetc);
		syslog(SYSNO_232C_SEND_ERR, "Unit=%d [err_code = %d]", m_nUnit, iRetc);
		return false;
	}
	
	//// ���M��
	// �����^�C�} �Z�b�g
	LARGE_INTEGER ts;
	ts.QuadPart = (__int64)PARAM_TIMEOUT_ANSER * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evAnserTimeOut, &ts, 0, NULL, NULL, FALSE);

	//// �X�e�[�^�X�ύX
	//m_bNowSend = true;
	return true;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DB�֘A

//------------------------------------------
// ���I�p�����[�^�擾
//------------------------------------------
bool CameraManager::GetParam()
{
#ifndef AGCOFF
	CString sql;					// SQL��
	int scode;
	int camangle;
	int camset;
	int wk;

	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "Unit %d", m_nUnit);
		return false;
	}
	

	//// SQL���쐬
#ifndef VER_P4_CAMERA
#ifndef VER_IDULE_CAMERA
	////// �\�ʏ�ԁ~�J�����Z�b�g�~�J�����p�x��
	CString sWk;
	sWk.Format("%d", MAX_PHYSICAL_CAMANGLE);
	sql =  "SELECT";
	sql += " scode, camangle, camset, InitialExpTime";
	sql += " FROM ";
	sql += DB_PARAM_CAMSET_ANGLE_SCODE;
	sql += " WHERE camangle>=1 and camangle<=";
	sql += sWk;
#else
	sql =  "SELECT";
	sql += " scode, 1, camnum, ExpTime, AGCstep";
	sql += " FROM ";
	sql += DB_PARAM_CAM_SCODE;
#endif
#else
	////// �\�ʏ�ԁ~�J�����Z�b�g
	// P4�J����
	sql =  "SELECT";
	sql += " scode, 1, camset, InitialExpTime";
	sql += " FROM ";
	sql += DB_PARAM_CAMSET_SCODE;
#endif

	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "Unit=%d [%s]", m_nUnit, DB_PARAM_CAMSET_ANGLE_SCODE);
		return false;
	}
	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��
			// ��L�[
			clsDB.GetData(1, &scode);
			clsDB.GetData(2, &camangle);
			clsDB.GetData(3, &camset);

			if( scode > MAX_SCODE || scode <= 0 || camset > MAX_CAMSET || camset <= 0 || camangle > MAX_PHYSICAL_CAMANGLE || camangle <= 0) {
				// ��L�[�s��
				LOG(em_ERR), "[%s] [%s ��L�[�s�� scode=%d camset=%d camangle=%d]", my_sThreadName, DB_PARAM_CAMSET_ANGLE_SCODE, scode, camset, camangle);
				syslog(SYSNO_DB_DATA_ERR, "Unit=%d [%s scode=%d camset=%d camangle=%d]", m_nUnit, DB_PARAM_CAMSET_ANGLE_SCODE, scode, camset, camangle);	
				continue;
			}

			// �f�[�^��
			clsDB.GetData(4, &m_Param.nInitialExpTime[scode-1][((camset-1)*MAX_PHYSICAL_CAMANGLE) + (camangle-1)] );
#ifdef VER_IDULE_CAMERA
			clsDB.GetData(5, &m_Param.dAGCstep[scode-1][((camset-1)*MAX_PHYSICAL_CAMANGLE) + (camangle-1)]);
#endif

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "Unit=%d [%s]", m_nUnit, DB_PARAM_CAMSET_ANGLE_SCODE);
			return false;
		}
	}
	
#ifndef VER_IDULE_CAMERA
	////// �\�ʏ��
	//// SQL���쐬
	sql =  "SELECT";
	sql += " scode, AGCstep";
	sql += " FROM ";
	sql += DB_PARAM_SCODE;
	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "Unit=%d [%s]", m_nUnit, DB_PARAM_SCODE);
		return false;
	}
	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��
			// ��L�[
			clsDB.GetData(1, &scode);

			if( scode > MAX_SCODE || scode <= 0 ) {
				// ��L�[�s��
				LOG(em_ERR), "[%s] [%s ��L�[�s�� scode=%d]", my_sThreadName, DB_PARAM_SCODE, scode);
				syslog(SYSNO_DB_DATA_ERR, "Unit=%d [%s scode=%d]", m_nUnit, DB_PARAM_SCODE, scode);	
				continue;
			}

			// �f�[�^��
			clsDB.GetData(2, &wk ); m_Param.dAGCstep[scode-1] = (double)(wk / 100.0);

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "Unit=%d [%s]", m_nUnit, DB_PARAM_SCODE);
			return false;
		}
	}
#endif
	
#ifdef VER_SCODE_CHG
	////// ���ʃp�����[�^
	//// SQL���쐬
	sql =  "SELECT";
	sql += " EnableInterCoilAGC";
	
	sql += " FROM ";
	//sql += DB_PARAM_COMMON;
	sql += m_cPrmCommonTblName;
	
	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "Unit=%d [%s]", m_nUnit, DB_PARAM_COMMON);
		return false;
	}
	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {						// �f�[�^�L��
			clsDB.GetData(1, &m_Param.bScodeChg);				// �L�����́ADB�̐ݒ�l�𔽉f

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// �f�[�^����
			break;	
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "Unit=%d [%s]", m_nUnit, DB_PARAM_COMMON);
			return false;
		}
	}
#else
	m_Param.bScodeChg = false;									// �������́A���f����
#endif

#else
//	static const int AGCOFF_DEF_InitilaExpTime		= 1800;				// �I�����ԏ����l [25��s]
//	static const int AGCOFF_DEF_AGCstep				= 5;				// AGC�X�e�b�v�T�C�Y

	for(int ii=0; ii<MAX_SCODE; ii++) {
		m_Param.dAGCstep[ii] = (double)(AGCOFF_DEF_AGCstep / 100.0);

		for(int jj=0; jj<NUM_CAMERA; jj++) {
			m_Param.nInitialExpTime[ii][jj] = AGCOFF_DEF_InitilaExpTime;
		}
	}
	m_Param.bScodeChg = false;									// �������́A���f����
#endif

	LOG(em_MSG), "[%s] ���I�p�����[�^�擾����", my_sThreadName);
	return true;
}

//------------------------------------------
// �ÓI�p�����[�^�擾
//------------------------------------------
bool CameraManager::GetParamStatic()
{
	CString sql;					// SQL��
	int nId;

	//// �p�����[�^�L��
	bool bAri[3];
	bAri[0] = 1 == GetPrivateProfileInt(m_cMySession, "PARAM_O", 0, TASKINI_NAME) ? true : false;	// �\�Z��
	bAri[1] = 1 == GetPrivateProfileInt(m_cMySession, "PARAM_U", 0, TASKINI_NAME) ? true : false;	// ���Z��
	bAri[2] = 1 == GetPrivateProfileInt(m_cMySession, "PARAM_T", 0, TASKINI_NAME) ? true : false;	// �e�X�gPLG
	
#if 1
	//// ODBC�ڑ��N���X �C���X�^���X����
	OdbcBase clsDB;
	if( ! clsDB.Connect(TASKINI_NAME)) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "Unit=%d", m_nUnit);
		return false;
	}
	
	////// ���ʃp�����[�^
	//// SQL���쐬
	sql =  "SELECT";
#ifndef VER_IDULE_CAMERA
	sql += " �J����PLG���{��, �J����PLG������, �J����PLG�M������, �J����PLG�ʑ�, �J����PLG�ʑ���, ";
	sql += " DummyIntval, ExSyncIntval, ExSyncMinH, ExSyncMinL";
#else
	sql += " PLGmag, PLGdiv, �J����PLG�M������, �J����PLG�ʑ�, �J����PLG�ʑ���, ";
	sql += " DummyIntval, MinFrameInterval, MinExpTime, MinFrameInterval - MaxExpTime";
#endif
	
	if(bAri[0]) sql += ", �J����PLG���{��_O, �J����PLG������_O, �J����PLG�M������_O, �J����PLG�ʑ�_O, �J����PLG�ʑ���_O";
	if(bAri[1]) sql += ", �J����PLG���{��_U, �J����PLG������_U, �J����PLG�M������_U, �J����PLG�ʑ�_U, �J����PLG�ʑ���_U";
	if(bAri[2]) sql += ", �J����PLG���{��_T, �J����PLG������_T, �J����PLG�M������_T, �J����PLG�ʑ�_T, �J����PLG�ʑ���_T";

	sql += " FROM ";
	//sql += DB_PARAM_COMMON;
	sql += m_cPrmCommonTblName;
	
	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "Unit=%d [%s]", m_nUnit, DB_PARAM_COMMON);
		return false;
	}
	//// �擾
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {						// �f�[�^�L��
			clsDB.GetData(1, &m_Param.nPlgMul[0]);
			clsDB.GetData(2, &m_Param.nPlgDiv[0]);
			clsDB.GetData(3, &m_Param.nPlgPulse[0]);
			clsDB.GetData(4, &m_Param.nPlgIsou[0]);
			clsDB.GetData(5, &m_Param.nPlgIsouSa[0]);
			clsDB.GetData(6, &m_Param.nPlgTestInterval);
			clsDB.GetData(7, &m_Param.nExSyncLT);
			clsDB.GetData(8, &m_Param.nExSyncMinH);
			clsDB.GetData(9, &m_Param.nExSyncMinL);
			nId = 10;

			for(int ii=0; ii<3; ii++) {
				if(bAri[ii]) {
					clsDB.GetData(nId+0, &m_Param.nPlgMul[ii+1]);
					clsDB.GetData(nId+1, &m_Param.nPlgDiv[ii+1]);
					clsDB.GetData(nId+2, &m_Param.nPlgPulse[ii+1]);
					clsDB.GetData(nId+3, &m_Param.nPlgIsou[ii+1]);
					clsDB.GetData(nId+4, &m_Param.nPlgIsouSa[ii+1]);

					nId += 5;
				} else {
					m_Param.nPlgMul[ii+1] = m_Param.nPlgMul[0];
					m_Param.nPlgDiv[ii+1] = m_Param.nPlgDiv[0];
					m_Param.nPlgPulse[ii+1] = m_Param.nPlgPulse[0];
					m_Param.nPlgIsou[ii+1] = m_Param.nPlgIsou[0];
					m_Param.nPlgIsouSa[ii+1] = m_Param.nPlgIsouSa[0];
				}
			}

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// �f�[�^����
			break;	
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "Unit=%d [%s]", m_nUnit, DB_PARAM_COMMON);
			return false;
		}
	}
#else
	for( int ii=0; ii<4; ii++ )
	{
		m_Param.nPlgMul[ii]		= 3;		// �J����PLG���{��
		m_Param.nPlgDiv[ii]		= 2;		// �J����PLG������
		m_Param.nPlgPulse[ii]	= 0;		// �J����PLG�M������ (0:���� 1:TTL)
		m_Param.nPlgIsou[ii]	= 0;		// �J����PLG�ʑ� (0:2�� 1:�P��)
		m_Param.nPlgIsouSa[ii]	= 0;		// �J����PLG�ʑ��� (0:�A�b�v�J�E���g 1:�_�E���J�E���g)
	}
	m_Param.nPlgTestInterval	= 45;		// �J����PLG�e�X�g�p���X���� (�P�x���胂�[�h���̎���) [��s]	
	m_Param.nExSyncLT			= 775;		// LT�ŏ����� [25��s]
	m_Param.nExSyncMinH			= 80;		// LT�����̍ŏ��l [25��s]
	m_Param.nExSyncMinL			= 80;		// LT�����̍ő�l�̕� �ő�l=(nExSyncLT-nExSyncMinL) [25��s]
#endif

	// ����ݒ�
	if(! bAri[2]) {						// �e�X�g�p���X�̃p�����[�^�w�肪�����ꍇ�̂�
		m_Param.nPlgIsouSa[3] = 0;		// �e�X�g�p���X�̓A�b�v�J�E���g�Œ�
	}

#ifdef VER_AVALDATA
#if 1
	////// �J���������N�X�C�b�`�ݒ�p�����[�^
	//// SQL���쐬
	sql =  "SELECT";
	sql += " �{�[�hID";
	sql += " FROM ";
	sql += DB_CAMERA_LINKSWITCH;
	sql += " WHERE ";
	sql += " �X�e�[�^�X = 1";
	sql.AppendFormat( " AND ID = %d",  m_nUnit+1);
	sql += " ORDER BY ID ASC";		// ID�̏���
	//// ���s
	if( !clsDB.ExecuteDirect(sql)) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "Unit=%d [%s]", m_nUnit, DB_CAMERA_LINKSWITCH);
		return false;
	}
	//// �擾
	nId = 0;
	while (TRUE) {
		int sqlrc = clsDB.Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {						// �f�[�^�L��
			if( NUM_CAMLINKSW <= nId ) {
				// �X�C�b�`�}�X�^ FPGA�{�[�hID�����I�[�o�[
				LOG(em_ERR), "[%s] �X�C�b�`�}�X�^ FPGA�{�[�hID �����I�[�o�[ [%d��]", my_sThreadName, nId);
				return false;
			}
			clsDB.GetData(1, &m_Param.nSwitchMasterID[nId]);
			LOG(em_INF), "[%s] �X�C�b�`�}�X�^ FPGA�{�[�hID �擾 [ID=%d]", my_sThreadName, m_Param.nSwitchMasterID[nId]);
			sprintf(m_Param.cSwitchMasterID[nId], "@%02X", m_Param.nSwitchMasterID[nId]);
			nId++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {			// �f�[�^����
			break;	
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "Unit=%d [%s]", m_nUnit, DB_CAMERA_LINKSWITCH);
			return false;
		}
	}
#else
	m_Param.nSwitchMasterID[0]	= 0;
	sprintf(m_Param.cSwitchMasterID[0], "@%02X", m_Param.nSwitchMasterID[0]);
	m_Param.nSwitchMasterID[1]	= 2;
	sprintf(m_Param.cSwitchMasterID[1], "@%02X", m_Param.nSwitchMasterID[1]);
#endif
#endif

	LOG(em_MSG), "[%s] �ÓI�p�����[�^�擾����", my_sThreadName);
	return true;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �R�}���h�֘A


//------------------------------------------
// �J����ID����`�����l���ԍ��擾
// int nCamID �J����ID (0�I���W��)
//------------------------------------------
char CameraManager::GetChannelNo(int nCamID)
{
	char cChNo;

#ifdef VER_AVALDATA
	// �J���������N�X�C�b�`�̃`�����l���ԍ���4�P��
	// ���R�F�J�����R���g���[��FPGA��ExSync���ȉ��̂悤�ɐڑ�����Ă��邽�߁I
	// <�`�����l���ԍ�>		<�ڑ���>
	// 0					�J����1��CC1
	// 1					�J����1��CC2
	// 2					�J����1��CC3
	// 3					�J����1��CC4
	// 4					�J����2��CC1
	// 5					�J����2��CC2
	// 6					�J����2��CC3
	// 7					�J����2��CC4
	nCamID *= 4;
#endif

	if( 0 <= nCamID && 10 > nCamID ) {
		cChNo = '0' + nCamID;
	} else if( 10 <= nCamID && 32 > nCamID ) {
		cChNo = 'a' + nCamID - 10;
	} else {
		cChNo = NULL;
	}

	return cChNo;
}
