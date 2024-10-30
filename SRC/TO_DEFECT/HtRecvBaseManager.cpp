#include "StdAfx.h"
#include "HtRecvBaseManager.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// �R���X�g���N�^
// const char *cSession �Z�b�V����������
// int TorB �\�����ʃt���O(0:�\, 1:��)
// bool bDistPostion ��ʂ̏ꍇtrue
// int accept_num �N���C�A���g�� (���������)
// int recv_num �t���[���P�ʂł̎�M�� (�_�������)
//------------------------------------------
HtRecvBaseManager::HtRecvBaseManager(const char *cSession, int TorB, bool bDistPostion, int accept_num, int recv_num, int HtCy) :
KizuHtRecv( cSession, TorB, accept_num, recv_num),
m_bDistPostion(bDistPostion),
mcls_pLog(NULL),
mcls_pLock(NULL),
mcls_pStatus(NULL),
mcls_pParam(NULL),
m_nHtCy(HtCy),
ThreadManager(cSession, HT_RECV_EVENT_MAX)
{
	//// ���v���Ԋ֘A ������

	//// �摜�捞�`���聨�����`�������܂ł̕��ρA�ő及�v���ԃN���A
	m_dHtProcNowTime = 0.0;
	m_dHtProcAveTime = 0.0;
	m_dHtProcMaxTime = 0.0;

	//// �摜�捞�`�������������܂ł̕��ρA�ő及�v���ԃN���A
	m_dToProcNowTime = 0.0;
	m_dToProcAveTime = 0.0;
	m_dToProcMaxTime = 0.0;


	//// �摜�����G���W�����������`�����`���J�n�܂ł̕��ρA�ő及�v���ԃN���A
	for(int ii=0; ii<NUM_CAM_POS; ii++) {
		m_dNcHtProcNowTime[ii] = 0.0;
		m_dNcHtProcAveTime[ii] = 0.0;
		m_dNcHtProcMaxTime[ii] = 0.0;
	}


	//// DS/WS���r����M���̃^�C���X�^���v
	m_pc_RecvFirst = 0;
	m_pc_RecvSecond = 0;


	//// DS(WS)���r����M�J�n�`WS(DS)���r����M�J�n�܂ł̕��ρA�ő及�v���ԃN���A
	m_dRecvProcNowTime = 0.0;
	m_dRecvProcAveTime = 0.0;
	m_dRecvProcMaxTime = 0.0;


	//// DS(WS)���r����M�J�n�`WS(DS)���r����M�����܂ł̕��ρA�ő及�v���ԃN���A
	m_dRecvProcTotalNowTime = 0.0;
	m_dRecvProcTotalAveTime = 0.0;
	m_dRecvProcTotalMaxTime = 0.0;


	// �p�t�H�[�}���X�J�E���^�̎��g�����擾
	QueryPerformanceFrequency((LARGE_INTEGER *)&m_pf);



	//// ���̑�������돉����
	m_nPerFramePos = 0;
	m_nNowGridRow  = 0;
	m_nNowMinRow   = 0;

	m_nNowOutGridRow = 0;
	m_dNowOutLength  = 0.0;
	m_nFrameNo       = 0;

	m_nTrId = GetTrId(m_nHtCy, TorB);						// �t���[���g���b�L���OID
	m_nTrOtherId = GetTrOtherId(m_nTrId);					// �����ʂ̃t���[���g���b�L���OID
	memset(&m_trFrm, 0x00, sizeof(m_trFrm));				// �t���[���g���b�L���O���

	// ��ʂ̔c��
	if(bDistPostion) {
		m_nDistPos = TorB;
		m_nDistNotPos = TorB == 0 ? 1 : 0;
	} else {
		m_nDistNotPos = TorB;
		m_nDistPos = TorB == 0 ? 1 : 0;
	}

	// �t���[���X�L�b�v & �G�b�W�s��
	m_nFrameSkipId		= 0;
	m_bFrameSkipState	= false;
	m_evNowFrameSkip	= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evTimeFrameSkip	= CreateWaitableTimer(NULL, FALSE, NULL);

	m_nEdgeNgId			= 0;
	m_bEdgeNgState		= false;
	m_evNowEdgeNg		= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evTimeEdgeNg		= CreateWaitableTimer(NULL, FALSE, NULL);

	// ���̑��V�O�i��
	m_evOpNewConnect	= CreateEvent(NULL, FALSE, FALSE, NULL);
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
HtRecvBaseManager::~HtRecvBaseManager(void)
{
	// �t���[���X�L�b�v & �G�b�W�s��
	CloseHandle(m_evNowFrameSkip);
	CloseHandle(m_evTimeFrameSkip);
	CloseHandle(m_evNowEdgeNg);
	CloseHandle(m_evTimeEdgeNg);

	CloseHandle(m_evOpNewConnect);
}


//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int HtRecvBaseManager::ThreadFirst()
{
	//// �C�x���g�ݒ� (���I�ǉ�)
	// �ڑ����� �ő�8�܂ł͕ύX����K�v�Ȃ�
	AddEventNum(PARAM_SOCK_MAX, my_evRecv);
	AddEventNum(1, &my_evDetectWaitStart);
	AddEventNum(1, &my_evDetectRun);
	AddEventNum(1, &my_evDetectStop);
	AddEventNum(1, &my_evAccept);
	AddEventNum(PARAM_SOCK_MAX, my_evDisConnect);
	AddEventNum(PARAM_SOCK_MAX, my_evRecvTimeOut);
	
	AddEventNum(1, &m_evTimeFrameSkip);
	AddEventNum(1, &m_evNowFrameSkip);
	AddEventNum(1, &m_evTimeEdgeNg);
	AddEventNum(1, &m_evNowEdgeNg);

	AddEventNum(1, &m_evOpNewConnect);

	//// �C�x���g�ǉ�
	ThreadFirst_AddHandle();


	//HANDLE hArray[] = { my_evRecv[0],				// �����̔z��֌W�����A�䐔����������K�v����
	//					my_evDetectWaitStart,
	//					my_evDetectRun,
	//					my_evDetectStop,
	//					my_evAccept,
	//					my_evDisConnect[0],
	//					my_evRecvTimeOut[0]
	//					};
	//SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);


	//// �\�P�b�g���� + Listen (��������܂Ńu���b�N)
	SetSockTID( GetTid() );
	while( TRUE ) {
		if( Listen(true) ) break;
		Sleep(2000);
	}
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int HtRecvBaseManager::ThreadLast()
{
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void HtRecvBaseManager::ThreadEvent(int nEventNo)
{
	int id = 0;										// �������ID (0�I���W��)
	int pcid = 0;									// ����@�\ID (1�I���W��)
	int recvid = 0;									// �_�����ID (0�I���W��)

	////// �V�O�i���̏���
	enum {	EV_RECV				= 0,								// ��M�����ʒm			(PARAM_SOCK_MAX��)
			EV_DETECT_WAITSTART	= EV_RECV + PARAM_SOCK_MAX,			// �����J�n�҂��v��
			EV_DETECT_RUN,											// �����J�n�v��
			EV_DETECT_STOP,											// ������~�v��
			EV_ACCEPT,												// �A�N�Z�v�g�ʒm
			EV_DISCONNECT,											// �ؒf�ʒm				(PARAM_SOCK_MAX��)
			EV_RECVTIMEOUT		= EV_DISCONNECT+PARAM_SOCK_MAX,		// ��M�^�C���A�E�g		(PARAM_SOCK_MAX��)
			EV_TIME_FRAME_SKIP	= EV_RECVTIMEOUT+PARAM_SOCK_MAX,	// �t���[���X�L�b�v�I��
			EV_NOW_FRAME_SKIP,										// �t���[���X�L�b�v����

			EV_TIME_EDGE_NG,										// �G�b�W�s���I��
			EV_NOW_EDGE_NG,											// �G�b�W�s������

			EV_OP_NEW_CONNECT,										// �V�K�\���@�\�V�K�ڑ�

			EV_LAST
	};

	////// �V�O�i����������
	
	//======================================================
	// ��M�����ʒm (PARAM_SOCK_MAX��)
	if(nEventNo >= EV_RECV && nEventNo < EV_RECV+PARAM_SOCK_MAX) {
		id = nEventNo - EV_RECV;
		OnRecvEnd(id);
	

	//======================================================
	// �����J�n�҂��v��
	} else if(nEventNo == EV_DETECT_WAITSTART)	{
		LOG(em_MSG), "[%s] <%s> �����J�n�҂��v��", my_sThreadName, DivNameManager::GetTorB(my_nTorB) );
		ExecDetectWaitStart();


	//======================================================
	// �����J�n�v��
	} else if(nEventNo == EV_DETECT_RUN)	{
		LOG(em_MSG), "[%s] <%s> �����J�n�v��", my_sThreadName, DivNameManager::GetTorB(my_nTorB) );
		ExecDetectRun();


	//======================================================
	// ������~�v��
	} else if(nEventNo == EV_DETECT_STOP)	{
		LOG(em_MSG), "[%s] <%s> ������~�v��", my_sThreadName, DivNameManager::GetTorB(my_nTorB) );
		ExecDetectStop();
		// ����ؒf
		this->CloseAll(true); 


	//======================================================
	// �A�N�Z�v�g�ʒm
	} else if(nEventNo == EV_ACCEPT)	{
		LOG(em_MSG), "[%s] <%s> �A�N�Z�v�g �V�O�i��", my_sThreadName, DivNameManager::GetTorB(my_nTorB) );

		id = this->Accept();
		if( id < 0 ) return;

		// ��M�J�n (�C�j�V�����`��)
		if( ! this->RecvHeaderInitialize(id) ) {
			LOG(em_ERR), "[%s] <%s> �C�j�V�����`�� ��M�J�n�ُ�", my_sThreadName, DivNameManager::GetTorB(my_nTorB));
			this->CloseAll();
			return;
		}


	//======================================================
	// �ؒf�ʒm (PARAM_SOCK_MAX��)
	} else if(nEventNo >= EV_DISCONNECT && nEventNo < EV_DISCONNECT+PARAM_SOCK_MAX) {
		id = nEventNo - EV_DISCONNECT;
		pcid = my_PC[ id ] - 1;

		if( DIV_SYS_RUN == mcls_pStatus->GetSys( my_nTorB ) ) {
			LOG(em_ERR), "[%s] <%s> �������ɂ����Ȃ����ؒf�I�I <%s> mode=%d ID=%d PCID=%d", my_sThreadName, DivNameManager::GetTorB(my_nTorB), my_cClientName[pcid], mcls_pSock->GetCloseKindID(id), id, pcid+1 );
			syslog(244, "[%s %s mode=%d]", DivNameManager::GetTorB(my_nTorB), my_cClientName[pcid], mcls_pSock->GetCloseKindID(id) );
			
		} else {
			LOG(em_MSG), "[%s] <%s> ����ؒf <%s>", my_sThreadName, DivNameManager::GetTorB(my_nTorB), my_cClientName[pcid] );
		}
		this->CloseAll();


	//======================================================
	// ��M�^�C���A�E�g (PARAM_SOCK_MAX��)
	} else if(nEventNo >= EV_RECVTIMEOUT && nEventNo < EV_RECVTIMEOUT+PARAM_SOCK_MAX) {
		// �������Ȃ�

	//======================================================
	// �t���[���X�L�b�v
	} else if(nEventNo == EV_TIME_FRAME_SKIP )	{		// �I��
		FrameSkipEnd();

	} else if(nEventNo == EV_NOW_FRAME_SKIP)	{		// �J�n
		FrameSkipStart();


	//======================================================
	// �G�b�W�s��
	} else if(nEventNo == EV_TIME_EDGE_NG )	{		// �I��
		EdgeNgEnd();

	} else if(nEventNo == EV_NOW_EDGE_NG)	{		// �J�n
		EdgeNgStart();

	//======================================================
	// �V�K�\���@�\�V�K�ڑ�
	} else if(nEventNo == EV_OP_NEW_CONNECT)	{
		OnOpNewConnect();

	//======================================================
	// ����ȊO�B�p����Œǉ������V�O�i��
	} else {
		// �p����ɒʒm
		OnAddSignal( nEventNo - EV_LAST );
	}
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �W������

//------------------------------------------
// ��M�����ʒm
// int id �������ID (0�I���W��)
//------------------------------------------
void HtRecvBaseManager::OnRecvEnd(int id) 
{
	int recvid = 0;									// �_�����ID (0�I���W��)


	switch( my_emSockState[id] ) {
	
	case KizuHtRecv::STATE_HEAD:								// �w�b�_�[��M

		//// �C�j�V�����`���H
		if( -1 == my_PC[id] ) {									// �C�j�V�����`��
			if( ! this->AnsInitializeSocket(id) ) {
				LOG(em_ERR), "[%s] <%s> �C�j�V�����`�� ��M�ُ�", my_sThreadName, DivNameManager::GetTorB(my_nTorB));
				CloseAll();
			}
			return;
		
		//// �Ɩ��`�� (1�t���[���̃w�b�_�[����M)
		} else {

			// �w���V�[�`��?  ���w���V�[�`���Ƃ������A���̂̓t���[���P�ʂłȂ��P���̓`����M
			if( 0 == memcmp( my_cRecvWkHead[ id ].base_head.code, T_CODE_HELCY, SIZE_SOCK_TC) ) {

#ifdef DEBUG_LOG_HT_HEALTHY
				LOG(em_FIL), "[%s] <%s> �Ɩ��`���w���V�[", my_sThreadName, DivNameManager::GetTorB(my_nTorB));
#endif
				// �p����ŏ���
				OnMainHls( &my_cRecvWkHead[ id ]);

				// ����M�J�n
				if( ! this->RecvHeaderDefect(id) ) {	
					LOG(em_ERR), "[%s] <%s> �w���V�[��M��̎���M�J�n���s", my_sThreadName, DivNameManager::GetTorB(my_nTorB));
					CloseAll();	
				}
				return;
			}

			// �w�b�_�[����M��������
			recvid = this->AnsHeader(id);
			if( 0 > recvid ) {
				LOG(em_ERR), "[%s] <%s> �Ɩ��`���w�b�_�[�� ��M�ُ�", my_sThreadName, DivNameManager::GetTorB(my_nTorB));
				return;
			}
			
			// �t���[��No�`�F�b�N
			if( ! this->CheckFrameNo( ((HT_TO_HEAD*)&my_cRecvHead[ recvid ])->nFrameNo, id, recvid ) ) {
				LOG(em_ERR), "[%s] <%s> �Ɩ��`���w�b�_�[���̃t���[��No�ُ�", my_sThreadName, DivNameManager::GetTorB(my_nTorB));
				syslog(243, "[%s: recvid=%d pre=%d now=%d]", my_cClientName[my_PC[id] - 1], recvid, my_nFrameNo[recvid], ((HT_TO_HEAD *)&my_cRecvHead[recvid])->nFrameNo);
				return;
			}

			// ��s��M����̎��Ԍv��
			int nRecvCnt	= 0;
			for(int ii=0; ii<my_nRecvNum; ii++) {
				if( my_nFrameNo[ii] == ((HT_TO_HEAD*)&my_cRecvHead[ recvid ])->nFrameNo ) nRecvCnt++;
			}
			if(1 == nRecvCnt)		QueryPerformanceCounter( (LARGE_INTEGER*)&m_pc_RecvFirst );
			else					QueryPerformanceCounter( (LARGE_INTEGER*)&m_pc_RecvSecond );


			// �Ɩ��`�� ��M��̃��C������
			if( this->CheckAllReadEnd() )  {
				MainDefect();
			}
		}
		break;

	case KizuHtRecv::STATE_DATA:								// �f�[�^����M
		// �Ɩ��`�� ��M�����`�F�b�N
		this->AnsData(id);
		
		// �Ɩ��`�� ��M��̃��C������
		if( this->CheckAllReadEnd() )  {
			MainDefect();
		}
		break;
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �V�[�P���X����

//------------------------------------------
// �����J�n�҂��J�n����
//------------------------------------------
void HtRecvBaseManager::ExecDetectWaitStart()
{
	//// �q�����ĂȂ��͂������A�O�ׂ̈� �ؒf����
	this->CloseAll(true);

	//// ������돉����
	m_nPerFramePos = 0;
	m_nNowGridRow  = 0;
	m_nNowMinRow   = 0;
	//m_nNowKizuSeq  = 0;


	m_nNowOutGridRow = 0;
	m_dNowOutLength  = 0.0;
	m_nFrameNo       = 0;

	// ��ʂ̕�����ɐi��ł��鎖�Ƃ���B	// OnExecDetectWaitStart() ������ �����Ƃ����l�ŏ��������邱��
	//if( m_bDistPostion ) {
	//	m_nNowGridRow = m_nDistLen / (MAP_REPRES_MIN*1000);
	//} else {
	//	m_nNowGridRow = 0;
	//}


	//// �@���ԏ�����
	if( 0 != m_nFrameSkipId ) {
		mcls_pStatus->SetStatusQue(m_nFrameSkipId, true, false);
	}

	//// �������锻��PC�̃N���C�A���g�����A�@�\�̏��Ԃ� �Z�b�g����
	for(int ii=0; ii<my_nAcceptNum; ii++) {
		// �R���s���[�^�[�����Z�b�g
		int index = ii + this->my_nTorB*my_nAcceptNum + ((m_nHtCy-1)*NUM_MEN*NUM_HANTEI);
		this->SetCliantName(ii, OnGetPcName(index) );
		LOG(em_MSG), "[%s] <%s> ����PC�N���C�A���g���o�^[%s]", my_sThreadName, DivNameManager::GetTorB(my_nTorB), OnGetPcName(index) );
	}

	//// �p����ɒʒm
	OnExecDetectWaitStart();

	// ���̌�A�e������R�l�N�g����Ă���͂��c
}

//------------------------------------------
// �����J�n����
//------------------------------------------
void HtRecvBaseManager::ExecDetectRun()
{
	//// �p�����[�^���Z�b�g
	EM_DIV_PLG emIsLinePLG = mcls_pStatus->GetPlg();
	m_dResV	= mcls_pParam->CalcResV( (int)emIsLinePLG );

	//// ���Ԍv������
	mcls_Time.InitAve();

	//// �摜�捞�`���聨�����`�������܂ł̕��ρA�ő及�v���ԃN���A
	m_dHtProcNowTime = 0.0;
	m_dHtProcAveTime = 0.0;
	m_dHtProcMaxTime = 0.0;

	//// �摜�捞�`�������������܂ł̕��ρA�ő及�v���ԃN���A
	m_dToProcNowTime = 0.0;
	m_dToProcAveTime = 0.0;
	m_dToProcMaxTime = 0.0;

	//// �摜�����G���W�����������`�����`���J�n�܂ł̕��ρA�ő及�v���ԃN���A
	for(int ii=0; ii<NUM_CAM_POS; ii++) {
		m_dNcHtProcNowTime[ii] = 0.0;
		m_dNcHtProcAveTime[ii] = 0.0;
		m_dNcHtProcMaxTime[ii] = 0.0;
	}

	//// DS/WS���r����M���̃^�C���X�^���v
	m_pc_RecvFirst = 0;
	m_pc_RecvSecond = 0;

	//// DS(WS)���r����M�J�n�`WS(DS)���r����M�J�n�܂ł̕��ρA�ő及�v���ԃN���A
	m_dRecvProcNowTime = 0.0;
	m_dRecvProcAveTime = 0.0;
	m_dRecvProcMaxTime = 0.0;
	//// DS(WS)���r����M�J�n�`WS(DS)���r����M�����܂ł̕��ρA�ő及�v���ԃN���A
	m_dRecvProcTotalNowTime = 0.0;
	m_dRecvProcTotalAveTime = 0.0;
	m_dRecvProcTotalMaxTime = 0.0;


	//// �T���J�n
	this->DefectStart(); 
	
	//// �p����ɒʒm
	OnExecDetectRun();

	//// ��M�J�n
	if( ! this->RecvAllHeader() ) {
		LOG(em_ERR), "[%s] <%s> �T���J�n���s�I�I", my_sThreadName, DivNameManager::GetTorB(my_nTorB));
		return;
	}
}

//------------------------------------------
// ������~����
//------------------------------------------
void HtRecvBaseManager::ExecDetectStop()
{
	//// �T����~
	this->DefectStop();

	//// �p����ɒʒm
	OnExecDetectStop();
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���C������
//------------------------------------------
// �Ɩ��`�� ��M��̃��C������
//------------------------------------------
void HtRecvBaseManager::MainDefect()
{
	/////////////////////////////////////////////////////////////////////
	// ���O����

	// �������Ԍv���J�n
	mcls_Time.CounterStart();

	// �u���b�N
	// �� �R�C���؂�ւ������Ă��鎞�́A������Ƒ҂�)
	mcls_pLock->BlockTh();

	// �t���[���g���b�L���O��� ���o�� & �폜
	HT_TO_HEAD& hth = (HT_TO_HEAD&)this->my_cRecvHead[0];	
	bool bRet;
	for(int ii=0; ii<TRACKING_ITEM_REPEAT_NUM; ii++) {
		bRet = mcls_pTrFrm->GetDelItem(m_nTrId, hth.nFrameNo, &m_trFrm);
		// ���������Ȃ玟�֐i��
		if(bRet) break;
		// ������~��Ɏ�M�����t���[���ɂ��Ă͕R�Â��𖳎�����
		if (DIV_KADOU_STOP == mcls_pStatus->GetKadou()) {
			bRet = !m_bFrameUnmatchState;				// �O��l���Z�b�g
			break;
		}
		// ���s�Ȃ�ҋ@��ă`�������W
		LOG(em_ERR), "[%s] <%s> �t���[���g���b�L���O���ꔭ�����̂��ߑҋ@�E�E�E[Fno=%d]", my_sThreadName, DivNameManager::GetTorB(my_nTorB), hth.nFrameNo);
		if(ii >= TRACKING_ITEM_REPEAT_NUM) break;
		Sleep(TRACKING_ITEM_SLEEP);
	}

	// �t���[������@���ԕύX
	if(0 != m_nFrameUnmatchId) {
		if (!bRet && !m_bFrameUnmatchState) {
			// �@���ԃZ�b�g
			m_bFrameUnmatchState = true;
			mcls_pStatus->SetStatusQue(m_nFrameUnmatchId, false, false);	// �t���[��No����
			mcls_pStatus->SetStatusRead();									// �񓯊������ǂݍ��݃V�O�i���Z�b�g (�����ɏ�Ԃ�ʒm����������)
			LOG(em_ERR), "[%s] <%s> �t���[���g���b�L���O����ُ�[Fno=%d]", my_sThreadName, DivNameManager::GetTorB(my_nTorB), hth.nFrameNo);
			syslog(245, "[%s, Fno=%d]", DivNameManager::GetTorB(my_nTorB), hth.nFrameNo);

			// �V�[�P���T�t���[����M�x��
			mcls_pStatus->AddQueMyErrWar(true, ON_SEQ_FRAME_DELAY);
		}
		else if(bRet && m_bFrameUnmatchState){
			// �@���ԃZ�b�g
			m_bFrameUnmatchState = false;
			mcls_pStatus->SetStatusQue(m_nFrameUnmatchId, true, false);		// �t���[��No����
			mcls_pStatus->SetStatusRead();									// �񓯊������ǂݍ��݃V�O�i���Z�b�g (�����ɏ�Ԃ�ʒm����������)
			LOG(em_MSG), "[%s] <%s> �t���[���g���b�L���O���ꐳ��[Fno=%d]", my_sThreadName, DivNameManager::GetTorB(my_nTorB), hth.nFrameNo);
		}
	}

	if(!bRet) memset(&m_trFrm, 0x00, sizeof(m_trFrm));
	int nMenMode	= mcls_pStatus->GetMenState();					// �����ʏ��(-1:���ʌ��� 0:�\�݂̂Ō��� 1:���݂̂Ō���)
	if(-1 != nMenMode) mcls_pTrFrm->DelItem(m_nTrOtherId, hth.nFrameNo);	// �Жʌ������́A�����ʂ̍폜�����{

	/////////////////////////////////////////////////////////////////////
	// �O����
	//

	/////////////////////////////////////////////////////////////////////
	// �p����Ŏ��s�����鏈��
	OnMainDefect();

	/////////////////////////////////////////////////////////////////////
	// �㏈��
	//

	// �����X�V
	//m_nPerFramePos = hth.frame_pos;								// �O��t���[���̒���ʒu [mm]
	m_nNowGridRow  = hth.frame_pos / (MAP_REPRES_MIN*1000);		// ����̑��M�}�b�v�ʒu
	m_nNowMinRow   = hth.frame_pos / (1*1000);					// �ŏ�����

	int nLen = (int)(hth.frame_pos + m_dOneFrameLen);
	m_nPerFramePos = nLen;								// �t���[���̒���ʒu [mm]

	bool bCoilChange	= false;
	//if( 1 == hth.toVal[TOVAL_TOP_COILCHANGE] && 0 == my_nTorB )	bCoilChange = true;
	//if( 1 == hth.toVal[TOVAL_BOT_COILCHANGE] && 1 == my_nTorB )	bCoilChange = true;
	if(1 == m_trFrm.nCoilChg[my_nTorB]) bCoilChange = true;

	if(bCoilChange) {
		// �R�C���ؑ֎��A�c����\���Đݒ�(�p���X���[�g�ύX���l��)
		EM_DIV_PLG emIsLinePLG = mcls_pStatus->GetPlg();
		m_dResV	= mcls_pParam->CalcResV( (int)emIsLinePLG );
		SetOneFrameLen( mcls_pParam->CalcResV((int)emIsLinePLG)*SIZE_FRAME_IMG_Y );
	}

	if( hth.nFrameNo != m_nFrameNo ){
		m_nFrameNo = hth.nFrameNo;
		//if( 1 == hth.toVal[TOVAL_LCUT_COILOUTCHANGE] ){
		if(1 == m_trFrm.nFCut) {
			m_nNowOutGridRow = 0;
			m_dNowOutLength  = 0.0;
		} else {
			m_dNowOutLength += (double)(SIZE_FRAME_IMG_Y) * m_dResV / 1000.0;
			m_nNowOutGridRow = (int)m_dNowOutLength;		// �o�������P�ʂ̒ʔ���[m]
		}
	}

	// ���̃t���[����Ǎ�
	if( ! this->RecvAllHeader() ) {
		LOG(em_ERR), "[%s] <%s> ���t���[���Ǎ��ُ�", my_sThreadName, DivNameManager::GetTorB(my_nTorB));
		// �������g���C����
		CloseAll();
	}

	// �������Ԍv���I��
	mcls_Time.CounterEnd();
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �t���[���X�L�b�v
//------------------------------------------
// �t���[���X�L�b�v �J�n
//------------------------------------------
void HtRecvBaseManager::FrameSkipStart()
{
	// ���ɃX�L�b�v���ł���� �@���Ԃɓo�^�͂��Ȃ�
	if( ! m_bFrameSkipState ) {
		// �@���ԃZ�b�g
		m_bFrameSkipState = true;
		mcls_pStatus->SetStatusQue(m_nFrameSkipId, false, false);		// �X�L�b�v
		mcls_pStatus->SetStatusRead();							// �񓯊������ǂݍ��݃V�O�i���Z�b�g (�����ɏ�Ԃ�ʒm����������)
	}

	// �O�����J�V�O�i��
	__int64 ts = (__int64)TIME_FRAME_SKIP_WAIT * -10000;
	SetWaitableTimer(m_evTimeFrameSkip, (LARGE_INTEGER*)&ts, 0, NULL, NULL, FALSE);
}

//------------------------------------------
// �t���[���X�L�b�v ����
//------------------------------------------
void HtRecvBaseManager::FrameSkipEnd()
{
	m_bFrameSkipState = false;
	mcls_pStatus->SetStatusQue(m_nFrameSkipId, true, false);		// �X�L�b�v
	mcls_pStatus->SetStatusRead();							// �񓯊������ǂݍ��݃V�O�i���Z�b�g (�����ɏ�Ԃ�ʒm����������)
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �G�b�W�s��
//------------------------------------------
// �G�b�W�s�� �J�n
//------------------------------------------
void HtRecvBaseManager::EdgeNgStart()
{
	// ���ɃX�L�b�v���ł���� �@���Ԃɓo�^�͂��Ȃ�
	if( ! m_bEdgeNgState ) {
		// �@���ԃZ�b�g
		m_bEdgeNgState = true;
		mcls_pStatus->SetStatusQue(m_nEdgeNgId, false, false);		// �G�b�W�s��
		mcls_pStatus->SetStatusRead();								// �񓯊������ǂݍ��݃V�O�i���Z�b�g (�����ɏ�Ԃ�ʒm����������)
	}

	// �O�����J�V�O�i��
	__int64 ts = (__int64)TIME_EDGE_NG_WAIT * -10000;
	SetWaitableTimer(m_evTimeEdgeNg, (LARGE_INTEGER*)&ts, 0, NULL, NULL, FALSE);
}

//------------------------------------------
// �G�b�W�s�� ����
//------------------------------------------
void HtRecvBaseManager::EdgeNgEnd()
{
	m_bEdgeNgState = false;
	mcls_pStatus->SetStatusQue(m_nEdgeNgId, true, false);		// �G�b�W�s��
	mcls_pStatus->SetStatusRead();								// �񓯊������ǂݍ��݃V�O�i���Z�b�g (�����ɏ�Ԃ�ʒm����������)
}
