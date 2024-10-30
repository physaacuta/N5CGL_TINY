#include "StdAfx.h"
#include "StatusBaseManager.h"


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
StatusBaseManager::StatusBaseManager(void) :
ThreadManager("StatusMgr"),
m_pCoilBase(NULL),
mcls_pStatusFunc(NULL),
mcls_pKiki(NULL)
{
	int ii;

	//// ������
	InitializeCriticalSection(&m_csLock);						// �N���e�B�J���Z�N�V�����I�u�W�F�N�g��������

	//// �X�e�[�^�X��� ������
	m_bMenMode	= false;
	m_nMenState = -1;
	m_emPlg		= DIV_PLG_LINE;
	m_emUnten	= DIV_UNTEN_STOP;
	m_emSogyo	= DIV_SOGYO_NON;
#ifdef DSP_SOGYO_CHECK
	m_emSogyoDsp = DIV_SOGYO_NON;
#endif
	m_emKadou	= DIV_KADOU_INIT;
	for(ii=0; ii< NUM_MEN; ii++) {
		m_emSys[ii]		 = DIV_SYS_INIT;
		m_emCycleSys[ii] = DIV_SYS_INIT;
		m_emParaSys[ii]	 = DIV_SYS_INIT;
		m_emKensa[ii]	 = DIV_KENSA_NON; //DIV_KENSA_OK;
		m_emKiki[ii]	 = DIV_KIKI_OK;
	}

	//// �@���Ԕc��
	memset( &m_oldNowSystem, 0x00, sizeof(m_oldNowSystem));
	memset( &m_oldStateRead, 0x00, sizeof(m_oldStateRead));

	//// �@���ԃC���X�^���X����
	mcls_pKiki = new KikiManager(PARAM_DB_READ_INTERVAL_TIME);
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
StatusBaseManager::~StatusBaseManager(void)
{
	//// �@���ԃX���b�h�J��
	SAFE_DELETE(mcls_pKiki);

	//// �J��
	DeleteCriticalSection(&m_csLock);							// �N���e�B�J���Z�N�V�����I�u�W�F�N�g���J��
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int StatusBaseManager::ThreadFirst()
{
	//// �@���ԊJ�n
	mcls_pKiki->SetLogMgr(mcls_pLog);
	if(m_bDspSyslog) mcls_pKiki->SetOutPutLog();						// �T����ʂ֌̏Ⴭ�O�\��
	mcls_pKiki->CheckBitSysArea(0);
	mcls_pKiki->Start();	// �ŏ��̓��O���o�Ȃ��Ă�����

	

	//// �C�x���g�ݒ�
	HANDLE hWk = mcls_pKiki->GetEvReadEnd();
	AddEventNum(1, &hWk);

	//// �C�x���g�ǉ�
	ThreadFirst_AddHandle();
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int StatusBaseManager::ThreadLast()
{
	//// �@���Ԓ�~
	mcls_pKiki->Stop();
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void StatusBaseManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���		
	enum {	EV_READEND = 0,								// �@���ԃ`�F�b�N����
			EV_LAST
			};

	////// �V�O�i����������
	//======================================================
	// �@���ԃ`�F�b�N����
	if(nEventNo == EV_READEND) {
		ReadKikiStatus();

	//======================================================
	// ����ȊO�B�p����Œǉ������V�O�i��
	} else {
		//// �p����ɒʒm
		OnAddSignal( nEventNo - EV_LAST );
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �@���Ԕc��

//------------------------------------------
// �@���Ԏ擾 & ��ԃ`�F�b�N
//------------------------------------------
void StatusBaseManager::ReadKikiStatus()
{
	int ii;
	TYP_STATUS_NOWSYSTEM		typStatus;						// �X�e�[�^�X���

	// �@���ԃN���X���@���ԏ����擾
	mcls_pKiki->GetStatusNow( &typStatus );


	////////////////////////////////////
	// ����c��
	////////////////////////////////////
	// ���@���ԂƔ�r
	for(ii=0; ii< NUM_MEN; ii++) {
		//// �������
		if( m_emKensa[ii] != typStatus.kensa[ii] ) {

			if( DIV_KENSA_OK == typStatus.kensa[ii]) {					// ����ɂȂ���
				if( DIV_KENSA_SKIP == m_emKensa[ii] || DIV_KENSA_DBNON == m_emKensa[ii] ) {
				} else {
					LOG(em_MSG), "[%s] %s ������ԕύX [%s �� %s]", my_sThreadName, 
						DivNameManager::GetTorB(ii), DivNameManager::GetDivKensa(m_emKensa[ii]), DivNameManager::GetDivKensa(typStatus.kensa[ii]) );
				}
			} else {													// �ُ�ɂȂ���
				if( DIV_KENSA_SKIP == typStatus.kensa[ii]) {
				} else if( DIV_KENSA_DBNON == typStatus.kensa[ii]) {
				} else if( DIV_KENSA_EDGENG == typStatus.kensa[ii]) {
				} else {
					if( DIV_KENSA_SKIP == m_emKensa[ii] || DIV_KENSA_DBNON == m_emKensa[ii] || DIV_KENSA_EDGENG == m_emKensa[ii] ) {
					} else {
						LOG(em_ERR), "[%s] %s ������ԕύX [%s �� %s]", my_sThreadName, 
							DivNameManager::GetTorB(ii), DivNameManager::GetDivKensa(m_emKensa[ii]), DivNameManager::GetDivKensa(typStatus.kensa[ii]) );
					}
				}
			}


			//// �V�X���O�o��
			CString		sLog1;
			CString		sLog2;
			sLog1.Format("%s �O��<%s>", DivNameManager::GetTorB(ii), DivNameManager::GetDivKensa(m_emKensa[ii]) );
			if(NULL != m_pCoilBase) {
				sLog2.Format(" (%s)(%s)", m_cKizukenNo, m_pCoilBase->cCoilNoIn);
			}
			if(DIV_KENSA_OK == typStatus.kensa[ii])		{ syslog(PARAM_SYSLOG_BASENO+10, "%s%s", sLog1, sLog2); }	// 310
			if(DIV_KENSA_NG == typStatus.kensa[ii])		{ syslog(PARAM_SYSLOG_BASENO+11, "%s%s", sLog1, sLog2); }	// 311
			if(DIV_KENSA_EDGENG == typStatus.kensa[ii]) { syslog(PARAM_SYSLOG_BASENO+14, "%s%s", sLog1, sLog2); }	// 314
			if(DIV_KENSA_DBNON == typStatus.kensa[ii])	{ syslog(PARAM_SYSLOG_BASENO+15, "%s%s", sLog1, sLog2); }	// 315
			if(DIV_KENSA_SKIP == typStatus.kensa[ii])	{ syslog(PARAM_SYSLOG_BASENO+16, "%s%s", sLog1, sLog2); }	// 316
			if(DIV_KENSA_STOP == typStatus.kensa[ii])	{ syslog(PARAM_SYSLOG_BASENO+19, "%s%s", sLog1, sLog2); }	// 319


			// ���f
			m_emKensa[ii] = (EM_DIV_KENSA)typStatus.kensa[ii];
		}

		//// �@����
		if( m_emKiki[ii] != typStatus.kiki[ii] ) {
			// ��ԕύX
			if( DIV_KIKI_OK == typStatus.kiki[ii]) {					// ����ɂȂ���
				LOG(em_MSG), "[%s] %s �@���ԕύX [%s �� %s]", my_sThreadName,
					DivNameManager::GetTorB(ii), DivNameManager::GetDivKiki(m_emKiki[ii]), DivNameManager::GetDivKiki(typStatus.kiki[ii]) );


			} else {													// �ُ�ɂȂ���
				LOG(em_ERR), "[%s] %s �@���ԕύX [%s �� %s]", my_sThreadName, 
					DivNameManager::GetTorB(ii), DivNameManager::GetDivKiki(m_emKiki[ii]), DivNameManager::GetDivKiki(typStatus.kiki[ii]) );
			}
			// ���f
			m_emKiki[ii] = (EM_DIV_KIKI)typStatus.kiki[ii];
		}
	}

	////////////////////////////////////
	// �ύX�����i�[
	////////////////////////////////////
	bool bChange = false;		// �O��Ɣ�ׂĕύX�L��H

	//// �O��l�Ə�Ԃ��Ⴄ�H (�ُ�̍��ڂ݂̂����`�F�b�N�ł��Ȃ���)	
	for(ii=0; ii< NUM_MEN; ii++) {
		// �O��ƈ���Ă���H
		if( !bChange && m_oldNowSystem.kensa[ii] != typStatus.kensa[ii] ) bChange = true;
		if( !bChange && m_oldNowSystem.kiki [ii] != typStatus.kiki [ii] ) bChange = true;
		if( !bChange && m_oldNowSystem.nCnt		 != typStatus.nCnt )	  bChange = true;		// �D��x900�ԑ�Ή�
		for(int jj=0; jj<typStatus.nCnt; jj++ ) {
			if( ! bChange && m_oldNowSystem.ngInf[jj].id != typStatus.ngInf[jj].id && 
				( 0 == typStatus.ngInf[jj].area || ii+1 == typStatus.ngInf[jj].area )) bChange = true;
		}	
	}
	// �擪���b�Z�[�W�̂ݕύX�Ή�
	if( ! bChange && m_oldNowSystem.ngInf[0].id != typStatus.ngInf[0].id ) bChange = true;


	//// ����l�Z�b�g
	EnterCriticalSection(&m_csLock);
	memcpy( &m_oldNowSystem, &typStatus, sizeof(typStatus));
	mcls_pKiki->GetStatusInf( (TYP_STATUS_INF*)&m_oldStateRead, sizeof(m_oldStateRead));
	LeaveCriticalSection(&m_csLock);


	////////////////////////////////////
	// �G���h����
	////////////////////////////////////
	//// �X�e�[�^�X���`�F�b�N
	if(m_bAddStlog) CheckStatusLog();

	//// �p�����
	OnReadKikiStatusEnd(bChange);
}


//------------------------------------------
// �X�e�[�^�X���`�F�b�N
//------------------------------------------
void StatusBaseManager::CheckStatusLog()
{
	if( NULL == mcls_pStatusFunc) return;
	StatusLogFunc::TYP_STATUS_LOG	typStatus;
	memset(&typStatus, 0x00, sizeof(typStatus));

	//--------------------------------
	// ������Z�b�g
	typStatus.Data.emKadou = m_emKadou;
	for(int ii=0; ii<NUM_MEN; ii++) {
		typStatus.Data.emKensa[ii]	= m_oldNowSystem.kensa[ii];
		typStatus.Data.emKiki[ii]	= m_oldNowSystem.kiki[ii];
	}
	memcpy(typStatus.Data.cMsg, m_oldNowSystem.ngInf[0].msg, sizeof(typStatus.Data.cMsg)); 

	// �g����
	if(NULL != m_pCoilBase) {
		memcpy(typStatus.cKizukenNo, m_cKizukenNo, SIZE_KIZUKEN_NO );
		memcpy(typStatus.cCoilNo, m_pCoilBase->cCoilNoIn, sizeof(m_pCoilBase->cCoilNoIn));
	}

	//// �X�e�[�^�X���O�N���X�ɓn��
	mcls_pStatusFunc->CheckStatusLog(&typStatus);
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �X�e�[�^�X����

//------------------------------------------
// �V�[�P���X���s�����Ԃ�
// �߂�l true:�V�[�P���X�\ false:�V�[�P���X�s�\
//------------------------------------------
bool StatusBaseManager::IsDoSequence()
{
	// �ғ���ԁF�ғ��� & (�^�]��ԁF�ʏ�^�] or �V�~�����[�V����)
	if( IsRun() &&
		(	DIV_UNTEN_NOMAL == GetUnten() ||
			DIV_UNTEN_SMYU  == GetUnten()	)
	  ) return true;
	return false;
}

//------------------------------------------
// �Жʌ����L���ύX
// bool st ����:true / �s����:false 
//------------------------------------------
bool StatusBaseManager::SetMenMode(bool st)
{
	////// ���C���C���X�^���X���o��
	bool	old;
	old = GetMenMode();
	if( old == st ) return false;

	//// �N���e�B�J���Z�N�V�����˓�
	AutoLock lock(&m_csLock);

	//// �ύX�X�e�[�^�X�ɂ�菈������
	m_bMenMode = st;
	syslog(PARAM_SYSLOG_BASENO+0, "[<%s> �� <%s>]", GetMenModeName(old), GetMenModeName(st));	// 300
	LOG(em_MSG), "<<�Жʌ����L���ύX>> [<%s> �� <%s>]", GetMenModeName(old), GetMenModeName(st)); 
	return true;

}
//------------------------------------------
// �����ʏ�ԕύX
// int st ���{��� (-1:���ʌ��� 0:�\�݂̂Ŏ��{ 1:���݂̂Ŏ��{)
//------------------------------------------
bool StatusBaseManager::SetMenState(int st)
{
	////// ���C���C���X�^���X���o��
	int	old;
	old = GetMenState();
	if( old == st ) return false;

	//// �N���e�B�J���Z�N�V�����˓�
	AutoLock lock(&m_csLock);

	//// �ύX�X�e�[�^�X�ɂ�菈������
	m_nMenState = st;
	if( -1 == st ) {
		syslog(PARAM_SYSLOG_BASENO+1, "[���ʌ���]"); // 301
		LOG(em_MSG), "<<�����ʏ�ԕύX>> [���ʌ���]"); 	
	} else if(0==st) {
		syslog(PARAM_SYSLOG_BASENO+1, "[�\�ʂ̂݌���]");  // 301
		LOG(em_MSG), "<<�����ʏ�ԕύX>> [�\�ʂ̂݌���]"); 	
	} else if(1==st) {
		syslog(PARAM_SYSLOG_BASENO+1, "[���ʂ̂݌���]");  // 301
		LOG(em_MSG), "<<�����ʏ�ԕύX>> [���ʂ̂݌���]"); 	
	}
	return true;
}


//------------------------------------------
// PLG�敪�ύX
// EM_DIV_PLG st PLG�敪
//------------------------------------------
bool StatusBaseManager::SetPlg(EM_DIV_PLG st)
{
	SetStatusQue("DIV_PLGMODE_ID", st, false);

	////// ���C���C���X�^���X���o��
	EM_DIV_PLG	old;
	old = GetPlg();
	if( old == st ) return false;

	//// �N���e�B�J���Z�N�V�����˓�
	AutoLock lock(&m_csLock);

	//// �ύX�X�e�[�^�X�ɂ�菈������
	m_emPlg = st;
	syslog(PARAM_SYSLOG_BASENO+2, "[<%s> �� <%s>]", DivNameManager::GetDivPlg(old), DivNameManager::GetDivPlg(st));  // 302
	LOG(em_MSG), "<<PLG�敪�ύX>> [<%s> �� <%s>]", DivNameManager::GetDivPlg(old), DivNameManager::GetDivPlg(st)); 
	return true;
}

//------------------------------------------
// �^�]��ԕύX
// M_DIV_UNTEN st �^�]���
//------------------------------------------
bool StatusBaseManager::SetUnten(EM_DIV_UNTEN st)
{
	SetStatusQue("DIV_UNTEN_ID", st, false);

	////// ���C���C���X�^���X���o��
	EM_DIV_UNTEN	old;
	old = GetUnten();
	if( old == st ) return false;

	//// �N���e�B�J���Z�N�V�����˓�
	AutoLock lock(&m_csLock);

	//// �ύX�X�e�[�^�X�ɂ�菈������
	m_emUnten = st;
	syslog(PARAM_SYSLOG_BASENO+3, "[<%s> �� <%s>]", DivNameManager::GetDivUnten(old), DivNameManager::GetDivUnten(st));  // 303
	LOG(em_MSG), "<<�^�]��ԕύX>> [<%s> �� <%s>]", DivNameManager::GetDivUnten(old), DivNameManager::GetDivUnten(st)); 
	return true;
}

//------------------------------------------
// ���Ə�ԕύX
// EM_DIV_SOGYO st ���Ə��
// int nMpm ���C�����x [mpm]
//------------------------------------------
bool StatusBaseManager::SetSogyo(EM_DIV_SOGYO st, int nMpm)
{
	SetStatusQue("DIV_SOGYO_ID", st, false);

	////// ���C���C���X�^���X���o��
	EM_DIV_SOGYO	old;
	old = GetSogyo();
	if( old == st ) return false;

	//// �N���e�B�J���Z�N�V�����˓�
	AutoLock lock(&m_csLock);

	//// �ύX�X�e�[�^�X�ɂ�菈������
	m_emSogyo = st;
	syslog(PARAM_SYSLOG_BASENO+4, "[<%s> �� <%s>] (���C�����x=%dmpm)", DivNameManager::GetDivSogyo(old), DivNameManager::GetDivSogyo(st), nMpm); // 304
	LOG(em_MSG), "<<���Ə�ԕύX>> [<%s> �� <%s>] (���C�����x=%dmpm)", DivNameManager::GetDivSogyo(old), DivNameManager::GetDivSogyo(st), nMpm);
	return true;
}

#ifdef DSP_SOGYO_CHECK
//------------------------------------------
// ���Ə�ԕύX �\���p
// EM_DIV_SOGYO st ���Ə��
// int nMpm ���C�����x [mpm]
//------------------------------------------
bool StatusBaseManager::SetSogyoDsp(EM_DIV_SOGYO st, int nMpm)
{
	//KizuFunction::SetStatus("DIV_SOGYO_ID", st, false);

	////// ���C���C���X�^���X���o��
	EM_DIV_SOGYO	old;
	old = GetSogyoDsp();
	if( old == st ) return false;

	//// �N���e�B�J���Z�N�V�����˓�
	AutoLock lock(&m_csLock);

	//// �ύX�X�e�[�^�X�ɂ�菈������
	m_emSogyoDsp = st;
	syslog(PARAM_SYSLOG_BASENO+8, "[<%s> �� <%s>] (���C�����x=%dmpm)", DivNameManager::GetDivSogyo(old), DivNameManager::GetDivSogyo(st), nMpm); // 308
	LOG(em_MSG), "<<�\���p���Ə�ԕύX>> [<%s> �� <%s>] (���C�����x=%dmpm)", DivNameManager::GetDivSogyo(old), DivNameManager::GetDivSogyo(st), nMpm);
	return true;
}
#endif

//------------------------------------------
// �ғ���ԕύX
// EM_DIV_KADOU st �ғ���ԕύX
//------------------------------------------
bool StatusBaseManager::SetKadou(EM_DIV_KADOU st)
{
	SetStatusQue("DIV_KADOU_ID", st, false);
	
	// �ғ���Ԃ��ς���������@��Ǘ��֒ʒm
	if(NULL != mcls_pKiki ) {
		if( DIV_KADOU_START == st)  mcls_pKiki->SetKadou(true);
		else						mcls_pKiki->SetKadou(false);

		// �������f	
		if( mcls_pKiki->IsThreadRun() ) {
			mcls_pKiki->Refresh();
			ReadKikiStatus();
		}
	}

	////// ���C���C���X�^���X���o��
	EM_DIV_KADOU	old;
	old = GetKadou();
	if( old == st ) return false;

	//// �N���e�B�J���Z�N�V�����˓�
	AutoLock lock(&m_csLock);

	//// �ύX�X�e�[�^�X�ɂ�菈������
	m_emKadou = st;
	syslog(PARAM_SYSLOG_BASENO+5, "[<%s> �� <%s>]", DivNameManager::GetDivKadou(old), DivNameManager::GetDivKadou(st));  // 305
	LOG(em_MSG), "<<�ғ���ԕύX>> [<%s> �� <%s>]", DivNameManager::GetDivKadou(old), DivNameManager::GetDivKadou(st)); 
	return true;
}


//------------------------------------------
// �V�X�e����ԕύX
// int ou �\:0 ��:1
// EM_DIV_SYS st �V�X�e�����
//------------------------------------------
bool StatusBaseManager::SetSys(int ou, EM_DIV_SYS st)
{
	////// ���C���C���X�^���X���o��
	EM_DIV_SYS	old;
	old = GetSys(ou);
	if( old == st ) return false;

	//// �N���e�B�J���Z�N�V�����˓�
	AutoLock lock(&m_csLock);

	//// �ύX�X�e�[�^�X�ɂ�菈������
	m_emSys[ou] = st;
	syslog(PARAM_SYSLOG_BASENO+6, "[%s:<%s> �� <%s>]", DivNameManager::GetTorB(ou), GetDivSys(old), GetDivSys(st));  // 306
	LOG(em_MSG), "<<�V�X�e����ԕύX>> [%s:<%s> �� <%s>]", DivNameManager::GetTorB(ou), GetDivSys(old), GetDivSys(st)); 
	return true;
}
//------------------------------------------
// �����@�\�V�X�e����ԕύX
// int ou �\:0 ��:1
// EM_DIV_SYS st �V�X�e�����
//------------------------------------------
bool StatusBaseManager::SetCycleSys(int ou, EM_DIV_SYS st)
{
	////// ���C���C���X�^���X���o��
	EM_DIV_SYS	old;
	old = GetCycleSys(ou);
	if( old == st ) return false;

	//// �N���e�B�J���Z�N�V�����˓�
	AutoLock lock(&m_csLock);

	//// �ύX�X�e�[�^�X�ɂ�菈������
	m_emCycleSys[ou] = st;
	syslog(PARAM_SYSLOG_BASENO+7, "[%s:<%s> �� <%s>]", DivNameManager::GetTorB(ou), GetDivSys(old), GetDivSys(st));  // 307
	LOG(em_MSG), "<<�����@�\�V�X�e����ԕύX>> [%s:<%s> �� <%s>]", DivNameManager::GetTorB(ou), GetDivSys(old), GetDivSys(st)); 
	return true;
}
//------------------------------------------
// �p�������@�\�V�X�e����ԕύX
// int pcno �p������PC1:0 �p������PC2:1
// EM_DIV_SYS st �V�X�e�����
//------------------------------------------
bool StatusBaseManager::SetParaSys(int pcno, EM_DIV_SYS st)
{
	KizuFunction::SetStatus(GetStatusRead().ParaKadou[pcno].id, Get_ParaKadou(st), false);

	////// ���C���C���X�^���X���o��
	EM_DIV_SYS	old;
	old = GetParaSys(pcno);
	if (old == st) return false;

	//// �N���e�B�J���Z�N�V�����˓�
	AutoLock lock(&m_csLock);

	//// �ύX�X�e�[�^�X�ɂ�菈������
	m_emParaSys[pcno] = st;
	syslog(PARAM_SYSLOG_BASENO + 9, "[�p������PC%d:<%s> �� <%s>]", pcno + 1, GetDivSys(old), GetDivSys(st));  // 309
	LOG(em_MSG), "<<�p�������@�\�V�X�e����ԕύX>> [�p������PC%d:<%s> �� <%s>]", pcno + 1, GetDivSys(old), GetDivSys(st));
	return true;
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �������ُ�c��

//------------------------------------------
// �������ُ픭��
// bool bErr ��敪 (true:Err false:War) Err���́A������~
// int nKind �敪
// int nSt1 �t�����
// int nSt2 �t�����
//------------------------------------------
void StatusBaseManager::AddQueMyErrWar(bool bErr, int nKind, int nSt1, int nSt2)
{
	//// �󂯓n���\����
	TYP_MY_ERRWAR_INF* p = new TYP_MY_ERRWAR_INF;
	p->bErr		= bErr;
	p->nKind	= nKind;
	p->nSt[0]	= nSt1;
	p->nSt[1]	= nSt2;

	if( ! mque_MyErrWar.AddToTailFreeCheck(p, 0) ) {
		LOG(em_ERR), "[%s] �������ُ�L���[�t��", my_sThreadName);
		syslog(SYSNO_QUEFULL_WAR, "[�������ُ�]");
		delete p;
	}
}