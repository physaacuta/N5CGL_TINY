#include "StdAfx.h"
#include "MainInctance.h"

#include "HT_DEFECT.h"			// ��ʂ̕��i�ɒ��ڃA�N�Z�X���邽��


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
MainInctance::MainInctance(void) :
m_nCamPair(0),
m_nMen(-1),
m_emKindHt(DIV_HANTEI_HT),
m_bDetectProc(false),
m_bDetectProc_pd(false),
m_bDetectProc_pr(false),
m_bAdjustProc(false),
m_bRectProc(false),
m_bNgCam(false),
m_nAppParam(0),
ThreadManager("MI")
{
	for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
		gcls_pSockRpi[ii] = NULL;
		gque_pSockRpi[ii] = NULL;
		gcls_pCull[ii] = NULL;
	}
	
	for (int ii=0; ii<NUM_MEN; ii++) {
		m_nNowScode[ii] = MAX_SCODE;
	}
	for(int ii=0; ii<HT_CONNECT_NUM; ii++) {
		gcls_pSock[ii]  = NULL;
	}
	memset(&m_pInfo, 0x00, sizeof(m_pInfo));
	memset(&m_typMergedInfo, 0x00, sizeof(m_typMergedInfo));

	// �V�O�i������
	m_evThStart = CreateEvent(NULL, FALSE, FALSE, NULL);				// ���C���X���b�h�����p
	m_evHelcy	= CreateWaitableTimer(NULL, FALSE, NULL);
	m_evGcpCheck = CreateWaitableTimer(NULL, FALSE, NULL);
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
MainInctance::~MainInctance(void)
{
	// �V�O�i���J��
	CloseHandle(m_evThStart);
	CloseHandle(m_evHelcy);
	CloseHandle(m_evGcpCheck);
}

//------------------------------------------
// �X���b�h�L���[�Ƀ��[���f�[�^�o�^
// ENUM_MAININCTANCE emNo ���b�Z�[�WNo (�C�x���gNo���̂܂܂ɂ��悤���Ǝv�������AFACT??_01�Ƃ����ƕ�����ɂ��������߂�)
// COMMON_QUE* que ���[���X���b�g�\���̂��̂���
//------------------------------------------
void MainInctance::SetDeliveryMail(ENUM_MAININCTANCE emNo, COMMON_QUE* que)
{
	DELIVERY_MAIL* pMail = new DELIVERY_MAIL;
	pMail->emNo	= emNo;
	memcpy( &pMail->que, que, sizeof(COMMON_QUE));

	// �X���b�h�L���[�ɓo�^
	if( ! mque_DeliMail.AddToTailFreeCheck(pMail) ) {
		delete pMail;
		LOG(em_ERR), "[%s] mque_DeliMail �L���[�t��!!", my_sThreadName);
	}
}

//------------------------------------------
// ���������� (��x�̂݋N��)
//------------------------------------------
BOOL MainInctance::Init()
{
	//// ���C���C���X�^���X�̂݁A�X���b�h�����͍���Ă��� (�����ɔ�V�O�i���Ƃ���)
	//// �p�����N���X���s (���C������X���b�h�ׁ̈A������ƗD��x����)
	ThreadManager::Start(THREAD_PRIORITY_ABOVE_NORMAL );

	// �X���b�hID��������܂ł�����Ƌx�e
	while( 0==GetTid() ) {
		LOG(em_MSG), "[%s] �X���b�hID���蓖�Ē��E�E�E", my_sThreadName);
		Sleep(1);			// �X���b�h�N���܂ł�����Ƒ҂�
	}


	////////////////////////////////////
	//// �p�����[�^�Ǎ��҂�

	// �X�e�[�^�X����N���X
	gcls_Status.SetLogMgr(mcls_pLog);
	while( true ) {
		if( gcls_Status.GetParam_CamPairInf() ) break;
		LOG(em_MSG), "[%s] �X�e�[�^�X�N���X �p�����[�^�Ǎ����s(DB�ڑ��ُ�H�I)�B���g���C���E�E�E", my_sThreadName);
		Sleep(5000);
	}

	// �p�����[�^�N���X
	gcls_PrmMgr.SetLogMgr(mcls_pLog); 
	while( true ) {
		if( gcls_PrmMgr.ReadParam_All() ) break;
		LOG(em_MSG), "[%s] �p�����[�^�N���X �p�����[�^�Ǎ����s(DB�ڑ��ُ�H�I)�B���g���C���E�E�E", my_sThreadName);
		Sleep(5000);
	}

	////////////////////////////////////
	//// �S���摜�֌W
#ifdef FRAME_RAW_SEND
	for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
		// �o�b�t�@�m��
		gque_pSockRpi[ii] = new ThreadLoopQueue<HT_RPI_FDATA>(MAX_BUF_QUE_RPISOCK);
		int n[1] = { SIZE_FRAME_IMG_X * SIZE_FRAME_IMG_Y * MAX_IMAGE_CNT };
		QueueAlloc(ii, MAX_BUF_QUE_RPISOCK, n);
		// ����M�X���b�h�N��
		gcls_pSockRpi[ii] = new SockOneToAnySend();
		gcls_pSockRpi[ii]->SetLogMgr(mcls_pLog);
		gcls_pSockRpi[ii]->SetTLQue((ThreadLoopQueue<SOCK_F_HEAD2>*) gque_pSockRpi[ii]);
		////gcls_pSockRpi[ii]->StartLock(THREAD_PRIORITY_TIME_CRITICAL);

		// �Ԉ����N���X
		gcls_pCull[ii] = new CullManager();
		gcls_pCull[ii]->SetLogMgr(mcls_pLog);
		gcls_pCull[ii]->SetLogMgrCull(mcls_pLogCull);
		gcls_pCull[ii]->SetParamMgr(&gcls_PrmMgr);
		gcls_pCull[ii]->SetRpiToSend(gcls_pSockRpi[ii]);
		gcls_pCull[ii]->SetQueSockRpi(gque_pSockRpi[ii]);
		////gcls_pCull[ii]->StartLock();

	}
#endif






	////////////////////////////////////
	//// ���[�J�[�X���b�h ������

	// �R�}���h���M�p�N���X
	gcls_pCom = new CommandSender(EPC_PIPE_COMMAND);
	gcls_pCom->SetLogMgr(mcls_pLog);

	// �f�[�^��M�p�N���X
	gcls_pData = new DataRecver(EPC_PIPE_DATA, EPC_SMEM_DATA);
	gcls_pData->SetLogMgr(mcls_pLog);
	
	// ����˓����ԒʐM �N���X
	for(int ii=0; ii<HT_CONNECT_NUM; ii++) {	
		gcls_pSock[ii] = new KizuToSend();
		gcls_pSock[ii]->SetLogMgr(mcls_pLog);
	}

	// New�f�[�^�Ǘ��N���X
	gcls_pSend = new DataSender();
	gcls_pSend->SetLogMgr(mcls_pLog);
	gcls_pSend->SetParamMgr(&gcls_PrmMgr);
	gcls_pSend->SetStatusMgr(&gcls_Status);
	gcls_pSend->SetDataRecver(gcls_pData);
	gcls_pSend->SetKizuToSend(gcls_pSock);
	gcls_pSend->SetCullMgr(gcls_pCull);


	// ���W�b�N�t�@�C���L���`�F�b�N�N���X
	gcls_Logic.SetLogMgr(mcls_pLog);


	////////////////////////////////////
	//// ���[�J�[�X���b�h �N��
	gcls_PrmMgr.StartLock();
	gcls_pCom->StartLock();
	gcls_pData->StartLock();
	for(int ii=0; ii<HT_CONNECT_NUM; ii++) gcls_pSock[ii]->Start();
	//gcls_pSend->StartLock();
	gcls_pSend->StartLock(THREAD_PRIORITY_ABOVE_NORMAL);
	//gcls_pSend->StartLock(THREAD_PRIORITY_TIME_CRITICAL);		// �G���W�������x�ꂪ��������
	gcls_Logic.StartLock();

	// ���W�b�N�t�@�C����ԕ\��
	gcls_Logic.Check(0);


	////////////////////////////////////
	//// �������

	int pcid = getpcid();
	gcls_Status.SetMainState(StatusManager::STATE_INIT);
	KizuFunction::SetStatus("HT_CANNOT_%d_ID", pcid, 0);
	Send_ToDefect_State();			// �����ʒm
	return TRUE;
}

//------------------------------------------
// �I������ (��x�̂݋N��)
//------------------------------------------
void MainInctance::Exit()
{
	//// ���[�J�[�X���b�h ��~
	gcls_pData->Stop();
	gcls_pSend->Stop();
	gcls_pCom->Stop();
	for(int ii=0; ii<HT_CONNECT_NUM; ii++) gcls_pSock[ii]->Stop();
	gcls_Logic.Stop();
	gcls_PrmMgr.Stop();


	//// ���[�J�[�X���b�h �J��
	delete gcls_pCom;
	delete gcls_pData;
	for(int ii=0; ii<HT_CONNECT_NUM; ii++) delete gcls_pSock[ii];
	delete gcls_pSend;

	////////////////////////////////////
	// �S���摜�֌W
#ifdef FRAME_RAW_SEND
	QueueFree();

	for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
		if( gcls_pSockRpi[ii]->IsThreadEvOn() ) gcls_pSockRpi[ii]->Stop();
		if( gcls_pCull[ii]->IsThreadEvOn()) gcls_pCull[ii]->Stop();

		delete gque_pSockRpi[ii];
		delete gcls_pSockRpi[ii];
		delete gcls_pCull[ii];
	}
#endif
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int MainInctance::ThreadFirst()
{
	////// �����[�N�X���b�h�̏����������܂őҋ@ (MainInctance�̂�)
	LOG(em_MSG), "[%s] �����[�N�X���b�h�̏����������܂őҋ@���E�E�E [0x%X]", my_sThreadName, GetTid() );
	WaitForSingleObject(m_evThStart, INFINITE);


	//// �C�x���g�ݒ�
	//HANDLE hArray[] = { 
	//					mque_DeliMail.g_evSem,
	//					gcls_pSock[0]->GetEvClose(),						// �Ж�(�ʏ�)
	//					gcls_pCom->GetEvOnError(),
	//					gcls_pData->GetEvOnError()
	//					};
	//SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);

	HANDLE h[8];
	//// �C�x���g�ݒ�
	AddEventNum(1, &mque_DeliMail.g_evSem);
	AddEventNum(1, &gcls_pCom->gque_CPipeErr.g_evSem);
	AddEventNum(1, &gcls_pData->gque_CPipeErr.g_evSem);
	AddEventNum(1, &m_evHelcy);		// ��������M�^�C�}�[
	AddEventNum(1, &m_evGcpCheck);	// �J����GCP���ʃ`�F�b�N�^�C�}�[

	// �S��
#ifdef FRAME_RAW_SEND
	for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) h[ii] = gcls_pSockRpi[ii]->GetEvConnect();	
	AddEventNum(FRAME_HT_SEND_PORT, h);		// �S���ڑ�
#endif


	for (int ii = 0; ii<HT_CONNECT_NUM; ii++) {
		h[0] = gcls_pSock[ii]->GetEvClose();
		AddEventNum(1, h);
	}
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int MainInctance::ThreadLast()
{
	//// ���� �T�����Ȃ� ��~
	if(gcls_Status.IsRunning()) {
		LOG(em_ERR), "[%s] �������� �����Ȃ胁�C���X���b�h�I���v��", my_sThreadName);
		if (DIV_HANTEI_PR != m_emKindHt)
		{
			DetectStop(0, 0);
		}
		else
		{
			ResumeStop(0, 0);
		}
	}
	//// �摜�����\�t�g�I��
	if( DIV_UNTEN_CAMERA == gcls_Status.GetUnten() )	DisposeProcess_iPortViewer();
	else												DisposeProcess_DetectProc();

	////�@�O�`�Z�o�v���Z�X
	DisposeProcess_RectProc();
	return 0;
}

//------------------------------------------
// ���s�\��Ԃ��`�F�b�N
// int nMen		�Ώۖ� �i0�F�\  1�F��  2�F���ʁj
// int nKind	����@�\ �i-1�F�S�@�\�@1�F�P���@�\  2�F�����@�\  3�F�p�������@�\�j
// �߂�l  true:���s�\	false:�ΏۊO
//------------------------------------------
bool MainInctance::DoSeqenc(int nMen, int nKind )
{
	if( 0 == m_nCamPair) return false;									// �o�b�N�A�b�v�@�Ȃ疳��
	if( DIV_UNTEN_CAMERA == gcls_Status.GetUnten() ) return false;		// �J����������
	if( ! gcls_Status.IsRunning() ) return false;						// ��~���́A����

	// ��
	if( 2 != nMen && m_nMen != nMen ) return false;

	// �@�\
	if( -1 != nKind && m_emKindHt != nKind ) return false;
	return true;
}


//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void MainInctance::ThreadEvent(int nEventNo)
{
	DELIVERY_MAIL* pMail;
	int retc;

	////// �V�O�i���̏���
	enum {	EV_MAIL = 0,					// ���[���X���b�g �ʒm (���[���͈�ԍŌ�ɂ��悤����)
			EV_CLASS_COMMAND_ERR,			// �R�}���h�p�C�v�N���X �G���[
			EV_CLASS_DATA_ERR,				// �f�[�^�p�C�v�N���X �G���[
			EV_ENG_HELCY_STAT,				// ��������
			EV_CAM_GCP_CHECK,				// �J����GCP���ʃ`�F�b�N

#ifdef FRAME_RAW_SEND
			EV_RPI_SOCK_CONECT,				// �ΑS�� �ڑ� (FRAME_HT_SEND_PORT��)

			EV_TO_SOCK_CLOSE=EV_RPI_SOCK_CONECT+FRAME_HT_SEND_PORT		// �Γ����p�̉�����ؒf		(HT_CONNECT_NUM��)
#else
			EV_TO_SOCK_CLOSE				// �Γ����p�̉�����ؒf		(HT_CONNECT_NUM��)
#endif
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_MAIL: 						// ���[���X���b�g �ʒm
	{
		pMail = mque_DeliMail.GetFromHead();
		switch(pMail->emNo) {

//-----------------------------------------------------------
		case E_DEF_ML_PARAM :									// �p�����[�^�ύX�v�� 
			// L[0]  ���s�t���[��No (0�F����)
			// L[1]  ����PC�敪 �i0�F�\  1�F��  2�F���ʁj
			// L[2]  �����\�ʏ��No �i1�`�FNo   99�F�S���j
			// L[3]  ����@�\ �i-1�F�S�@�\  1�F�P���@�\  2�F�����@�\  3�F�p�������j
			/***
			if( 0 == m_nCamPair) break;									// �o�b�N�A�b�v�@�Ȃ疳��
			if( DIV_UNTEN_CAMERA == gcls_Status.GetUnten() ) break;		// �J����������
			if( ! gcls_Status.IsRunning() ) break;						// ��~���́A����

			if( DIV_HANTEI_HT == m_emKindHt ) {							// �����͌��� ����@�\
				if( DIV_HANTEI_HT  != pMail->que.fl.data[3] && 
					DIV_HANTEI_NON != pMail->que.fl.data[3] ) break;
				//if( 2 != pMail->que.fl.data[1] &&						// ���ʑΏہA�������́A�Y���ʂłȂ��ꍇ�͖���
				//	((m_nCamPair-1)/(NUM_CAMPAIR/NUM_MEN)) != pMail->que.fl.data[1] ) break; 
				if (2 != pMail->que.fl.data[1] &&						// ���ʑΏہA�������́A�Y���ʂłȂ��ꍇ�͖���
					m_nMen != pMail->que.fl.data[1]) break;


			} else {													// �����͌��� �����@�\
				if( DIV_HANTEI_CY  != pMail->que.fl.data[3] && 
					DIV_HANTEI_NON != pMail->que.fl.data[3] ) break;
				//if( 2 != pMail->que.fl.data[1] &&						// ���ʑΏہA�������́A�Y���ʂłȂ��ꍇ�͖���
				//	((m_nCamPair-1)/(NUM_CAMPAIR_CYCLE/NUM_MEN)) != pMail->que.fl.data[1] ) break;  
				if (2 != pMail->que.fl.data[1] &&						// ���ʑΏہA�������́A�Y���ʂłȂ��ꍇ�͖���
					m_nMen != pMail->que.fl.data[1]) break;

			}
			***/

			if( ! DoSeqenc(pMail->que.fl.data[1], pMail->que.fl.data[3]) ) break;
			retc = SendCommand_Param( pMail->que.fl.data[0], pMail->que.fl.data[2]);		
			CommandAnswerProc(retc, APP_PARAM_PARAM);	// �A���T�[�ɂ�菈������
			break;	


//-----------------------------------------------------------
		case E_DEF_ML_START :									// �����J�n�v��
			// L[0]  �^�]���[�h�i1�F�ʏ�  2�F�e�X�g[����َB�e]  3�F�Z��[��ג���]�j
			// L[1]  �\�� �����\�ʏ��No�i1�`�j
			// L[2]  ���� �����\�ʏ��No�i1�`�j
			// L[3]  �\�� ��������[mm]
			// L[4]  ���� ��������[mm]
			// L[5 + 2 �~�i����ID-1�j + 0]  PCID���� �J�����y�AID���Z�b�g�i����p�A�����p�j�i1�`�F�I�����C��   0�F�ޯ����� or �ΏۊO)
			// L[5 + 2 �~�i����ID-1�j + 1]  PCID���� �Y������@�\���Z�b�g  ����@�\ �i1�F�P���@�\  2�F�����@�\  0�F�ޯ�����  3�F�p�������j
			if (true)
			{
				int				pcid = (getpcid() - 1);
				EM_DIV_HANTEI	emKind = (EM_DIV_HANTEI)pMail->que.fl.data[5 + 2 * pcid + 1];

				// �����ւ̃f�[�^���M��ԃZ�b�g
				bool bFrameSend = DIV_HANTEI_PR != emKind;
				gcls_pSend->SetFrameSend(bFrameSend);
					
				if (DIV_HANTEI_PR != emKind)
				{
					DetectStart(&pMail->que);
				}
				else
				{
					int	nScode[2];
					nScode[0] = pMail->que.fl.data[1];					// �����\�ʏ�� �\
					nScode[1] = pMail->que.fl.data[2];					// �����\�ʏ�� ��
					int	pcid = (getpcid() - 1);							// PCID(PCKIND�ł͂Ȃ�) (0�I���W��)
					int	campair = getpcid() - ((NUM_HANTEI * NUM_MEN) + NUM_ALL_HTBACK);	// �J�����y�A 1�`

					LOG(em_MSG), "[%s] �����J�n�v���B(s=%d:%d campair=%d kind=%d[1:���� 2:���� 3:�p������] �f�[�^���M:%s)", my_sThreadName, nScode[0], nScode[1], campair, emKind, (gcls_pSend->IsFrameSend() ? "����" : "�Ȃ�"));

					// �����֌��ʂ𑗐M
					int			mailRetc;
					COMMON_QUE	sque;
					sque.nEventNo = FACT_TO_DEFECT_06;				// ����J�n�A���T�[
					sque.nLineNo = getlineid();

					sque.fl.data[0] = campair;						// �J�����y�A=PcKind
					sque.fl.data[1] = getpcid();					// PcId
					sque.fl.data[2] = 0;							// ����
					sque.fl.data[3] = (int)emKind;					// �S������
					mailRetc = send_mail(TO_DEFECT, TOKATU, &sque);
					if (0 != mailRetc) syslog(SYSNO_MAIL_SNED_ERR, "[�����J�n�A���T�[ err_code=%d]", mailRetc);
				}
			}
			break;	

//-----------------------------------------------------------
		case E_DEF_ML_STOP :									// ������~�v��
			if( 0 == m_nCamPair) break;									// �o�b�N�A�b�v�@�Ȃ疳��

			if (DIV_HANTEI_PR != m_emKindHt)
			{
				DetectStop(0, 0);
			}
			else
			{
				int			pcid = (getpcid() - 1);
				int			cond = pMail->que.fl.data[pcid];

				// ���X�e�[�^�X�m�F
				if (StatusManager::STATE_NON == gcls_Status.GetMainState() ||
					StatusManager::STATE_INIT == gcls_Status.GetMainState() ||
					StatusManager::STATE_CANNOT == gcls_Status.GetMainState()) {
					LOG(em_ERR), "[%s] [%s] ���Ɍ�����~�v����M", my_sThreadName, gcls_Status.GetMainStateName() );
					return;
				}

				LOG(em_MSG), "[%s] ������~�v���B(%s) ==============>>>", my_sThreadName, "�����~�v��");
				LOG(em_MSG), "[%s] -->�r����~�������Ăяo��", my_sThreadName);

				ResumeStop(0, 0);

				int			mailRetc;
				COMMON_QUE	sque;
				sque.nEventNo = FACT_TO_DEFECT_07;				// �����~�A���T�[
				sque.nLineNo = getlineid();
				sque.fl.data[0] = m_nCamPair;					// �J�����y�A=PcKind
				sque.fl.data[1] = getpcid();					// PcId
				sque.fl.data[2] = 0;							// ����
				sque.fl.data[3] = 0;							// �敪 (0:�v���ɑ΂���A���T�[ 1:�ُ픭����)
				sque.fl.data[4] = m_emKindHt;

				mailRetc = send_mail(TO_DEFECT, TOKATU, &sque);
				if (0 != mailRetc) syslog(SYSNO_MAIL_SNED_ERR, "[������~�A���T�[ err_code=%d]", mailRetc);
			}
			break;	

//-----------------------------------------------------------
		case E_DEF_ML_COILINF :									// ��ʐݒ���(������)
			//if( 0 == m_nCamPair) break;									// �o�b�N�A�b�v�@�Ȃ疳��
			//if( ! gcls_Status.IsRunning() ) break;						// ��~���́A����
			//if( DIV_UNTEN_CAMERA == gcls_Status.GetUnten() ) break;		// �J����������
			if( ! DoSeqenc(2, -1) ) break;

			Send_CoilInfDiv(&pMail->que);	// ���C������(�����擾&�S���������瑗�M)
			break;


//-----------------------------------------------------------
		case E_DEF_ML_CANOK :									// ���s�s�\����
			SendState_PcCanOk(true);
			break;

//-----------------------------------------------------------
		case E_DEF_ML_STATUS :									// ��Ԗ₢���킹
			Send_ToDefect_State();
			break;


//-----------------------------------------------------------
		case E_DEF_ML_CLEAR_QUEUE:								// �����s�ݒ�̃L�����Z���v��
			if( ! DoSeqenc(pMail->que.fl.data[0], pMail->que.fl.data[1]) ) break;
			retc = SendCommand_ClearQueue( );
			CommandAnswerProc(retc, APP_PARAM_CLEAR_QUEUE);
			break;

//-----------------------------------------------------------
		case E_DEF_ML_SET_POSITION:								// ���������J�E���^�ݒ�
			if( ! DoSeqenc(pMail->que.fl.data[0], pMail->que.fl.data[1]) ) break;
			retc = SendCommand_SetPosition( (NCL_CMD_SET_POSITION*) &pMail->que.fl.data[2]);
			CommandAnswerProc(retc, APP_PARAM_SET_POSITION);
			break;

//-----------------------------------------------------------
		case E_DEF_ML_PRELOAD_PARAM:							// �p�����[�^���O�Ǎ�
			if( ! DoSeqenc(pMail->que.fl.data[0], pMail->que.fl.data[1]) ) break;			
			retc = SendCommand_PreLoad_Param( pMail->que.fl.data[2]);
			CommandAnswerProc(retc, APP_PARAM_PRELOAD_PARAM);
			break;

//-----------------------------------------------------------
		case E_DEF_ML_RESUME_START:								// �r���J�n�v��
			ResumeStart(&pMail->que);
			break;

//-----------------------------------------------------------
		case E_DEF_ML_RESUME_STOP:								// �r����~�v��
			ResumeStop(0, 0);
			break;

//-----------------------------------------------------------
		}
		// �J��
		delete pMail;
		break;
	}

//-----------------------------------------------------------------------------------------------
	case EV_CLASS_COMMAND_ERR:									// �R�}���h�p�C�v�N���X �G���[
		if(true) {
			CPIPE_ERROR *	pCPipeErr = gcls_pCom->gque_CPipeErr.GetFromHead();
			CommandSender::EM_PIPE_ERR	emErrCode = (CommandSender::EM_PIPE_ERR)pCPipeErr->nErrCode;
			long						nDetail = pCPipeErr->nDetailCode;
			
			// �G���[�`�F�b�N
			int							ansRetc;
			ansRetc = CheckCommandPipe_Err(emErrCode, nDetail, pCPipeErr->pData);
			if (NULL != pCPipeErr->pData) delete pCPipeErr->pData;
			delete pCPipeErr;

			// �y�̏� ���� �d�̏� ���� ��~
			if (0 != ansRetc && gcls_Status.IsRunning())
			{
				if (DIV_HANTEI_PR != m_emKindHt)
				{
					DetectStop(1, ansRetc);
				}
				else
				{
					ResumeStop(1, ansRetc);
				}
			}
			// �d�̏� = ���s�s�\
			if( -1 == ansRetc) SendState_PcCanNot();
		}
		break;


//-----------------------------------------------------------------------------------------------
	case EV_CLASS_DATA_ERR:										// �f�[�^�p�C�v�N���X �G���[
		if(true) {
			CPIPE_ERROR *	pCPipeErr = gcls_pData->gque_CPipeErr.GetFromHead();
			DataRecver::EM_PIPE_ERR		emErrCode = (DataRecver::EM_PIPE_ERR)pCPipeErr->nErrCode;
			int	nDetail = pCPipeErr->nDetailCode;
			delete pCPipeErr;


			CString cbuf;
			switch (emErrCode) {
			case DataRecver::CPIPE_OPEN_ERR:
				cbuf = "�I�[�v���Ŏ��s";
				break;
			case DataRecver::CPIPE_READ_ERR:
				cbuf = "�Ǎ��݂Ŏ��s";
				break;
			case DataRecver::CPIPE_ERR_SMEM:
				cbuf = "���L�������I�[�v�����s";
				break;
			case DataRecver::CPIPE_DISCONNECT_ERR:
				cbuf = "�p�C�v������ɐؒf���ꂽ";
				break;
			}
			LOG(em_ERR), "[%s] <�f�[�^�p�C�v�G���[> %s [�ڍ�=%d]", my_sThreadName, cbuf, nDetail);
			syslog(108, "[%s:�ڍ�=%d]", cbuf, nDetail);

			// �N������ ��~
			if (gcls_Status.IsRunning())
			{
				if (DIV_HANTEI_PR != m_emKindHt)
				{
					DetectStop(1, -1);
				}
				else
				{
					ResumeStop(1, -1);
				}
			}
			// �d�̏� = ���s�s�\
			SendState_PcCanNot();
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_ENG_HELCY_STAT:										// �摜�����G���W���ւ̒�������
		if (gcls_pData->IsConnect())
		{
			if (!gcls_pData->IsFirstInfoExists())
			{
#ifdef DEBUG_LOG_HT_HEALTHY
				LOG(em_INF), "[%s] �摜�����G���W���ɖ₢���킹", my_sThreadName);
#endif
				// ���L�������ɃJ������񂪑��݂��Ȃ��ꍇ�̂݉摜�����G���W���ɖ₢���킹�����{����
				HelcyStat();
			}
			else
			{
				ENG_STAT	st;

#ifdef DEBUG_LOG_HT_HEALTHY
				LOG(em_INF), "[%s] ���L������������擾", my_sThreadName);
#endif
				// ���L�������ɃJ������񂪑��݂���ꍇ�A���L������������擾����
				gcls_Status.GetEngStat(&st);
				gcls_pData->HelcyStat(&st);
				if (0 <= st.camnum && NCL_ncam_max > st.camnum)
				{
					for (DWORD ii = 0; ii < st.camnum; ii++)
					{
						LOG(em_INF), "[%s] ���L�������̃J������� [ID:%d][���x:%.3f][�I��:%.3f %.3f %.3f][�Q�C��:%.3f %.3f %.3f][�d��:%.3f]"
							, my_sThreadName
							, st.camid[ii]
							, st.temp[ii]
							, st.exptime[0][ii]
							, st.exptime[1][ii]
							, st.exptime[2][ii]
							, st.gain[0][ii]
							, st.gain[1][ii]
							, st.gain[2][ii]
							, st.volt[ii]
							);
					}
				}
				else
				{
					LOG(em_INF), "[%s] ���L�������̃J�������ُ�:%d", my_sThreadName, st.camnum);
				}
				gcls_Status.SetEngStat(&st);
			}

#ifdef DEBUG_LOG_HT_HEALTHY
			{
				ENG_STAT	st;
				gcls_Status.GetEngStat(&st);

				for (int ii = 0; ii < st.camnum; ii++)
				{
					LOG(em_FIL), "[%s] [ID:%d][���x:%.3f][�I��:%.3f %.3f %.3f][�Q�C��:%.3f %.3f %.3f][�d��:%.3f]"
						, my_sThreadName
						, st.camid[ii]
						, st.temp[ii]
						, st.exptime[0][ii]
						, st.exptime[1][ii]
						, st.exptime[2][ii]
						, st.gain[0][ii]
						, st.gain[1][ii]
						, st.gain[2][ii]
						, st.volt[ii]
						);
				}
			}
#endif
		}
		break;

//-----------------------------------------------------------
	case EV_CAM_GCP_CHECK:										// �J����GCP���ʃ`�F�b�N
		{
			LOG(em_MSG), "[%s] �J����GCP���� �`�F�b�N�J�n ------------------->>>>>", my_sThreadName);

			GcpCheck();

			LOG(em_MSG), "[%s] �J����GCP���� �`�F�b�N���� <<<<<-------------------", my_sThreadName);
		}
		break;

//-----------------------------------------------------------
#ifdef FRAME_RAW_SEND
	case EV_RPI_SOCK_CONECT +0:									// �ΑS�� �ڑ�
		LOG(em_MSG), "[%s] �ΑS��1 �ڑ�����", my_sThreadName);
		break;
	case EV_RPI_SOCK_CONECT +1:									// �ΑS�� �ڑ�
		LOG(em_MSG), "[%s] �ΑS��2 �ڑ�����", my_sThreadName);
		break;
#endif

//-----------------------------------------------------------------------------------------------
	case EV_TO_SOCK_CLOSE + 0:									// �Γ����p�̉�����ؒf
	case EV_TO_SOCK_CLOSE + 1:									// �Γ����p�̉�����ؒf
		if( gcls_Status.IsRunning() ) {
			LOG(em_ERR), "[%s] �������ɑΓ����ʐM������ؒf", my_sThreadName);
			syslog(111, "[�������ɋ����ؒf]");
			if (DIV_HANTEI_PR != m_emKindHt)
			{
				DetectStop(1, 1);
			}
			else
			{
				ResumeStop(1, 1);
			}
		} else {
			LOG(em_MSG), "[%s] �������ȊO�ł̉���ؒf�́A����", my_sThreadName);
		}
		break;

//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma region // �S��

//------------------------------------------
// �S���������M�p�̉�]���i�L���[�m��
// int n		�L���[��
// int const* p	��n���
//					[0] ��n�摜�ő�T�C�Y
//------------------------------------------
void MainInctance::QueueAlloc(int idx, int n, int const* p)
{	
	HT_RPI_FDATA *pD = NULL;

	//// ��̊���쐬
	for (int ii = 0; ii < n; ii++) {
		pD = new HT_RPI_FDATA;
		pD->Alloc(p);
		gque_pSockRpi[idx]->AddFree(pD);			// ���ǉ�
	}
	
	LOG(em_MSG),"[%s] �S������<%d> ���M�L���[%d���m�� [%dMB]", my_sThreadName, idx, n, (__int64)p[0] * n / 1024 / 1024);
	return ;
}

//------------------------------------------
// �S���������M�p�̉�]���i�L���[���
//------------------------------------------
void MainInctance::QueueFree()
{	
	HT_RPI_FDATA *pD = NULL;
	int nPoolNum = 0;
	int nFreeNum = 0;

	for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
		// ��L�L���[
		while(true) {
			pD = gque_pSockRpi[ii]->GetPool();
			if( NULL == pD) break;
			pD->Free();
			delete pD;
			pD = NULL;
			nPoolNum ++;
		}
		// �󂫃L���[
		while(true) {
			pD = gque_pSockRpi[ii]->GetFree();
			if( NULL == pD) break;
			pD->Free();
			delete pD;
			pD = NULL;
			nFreeNum ++;
		}
	}
	LOG(em_MSG),"[%s] �S������ ���M�L���[��� [PoolQue=%d; FreeQue=%d]", my_sThreadName, nPoolNum, nFreeNum );
}

#pragma endregion


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma region // �v���Z�X����


//------------------------------------------
// �O�`�Z�o�v���Z�X �N��
// �߂�l true:�N������  false:�N�����s
//------------------------------------------
bool MainInctance::ExecuteProcess_RectProc()
{
	CString				sWk;

	//=======================================
	// �N���Ώ�PC��I��
	sWk.Format("EXEC_PCKIND_%d", m_nMen);
	int nKind = GetPrivateProfileInt(HT_DEFECT, sWk, 0, TASKINI_NAME);
	if( 0 == m_nCamPair || nKind != m_nCamPair ) {
		// �����ƈႤ�̂ŁA�O������N�����Ă����̂ł���΁A�I��
		DisposeProcess_RectProc();

		if( 0 != m_nCamPair) {
			Sleep(1000);	// rect������PC�ł́Arect�N����������Ƒ҂�
		}
		return true;
	}


	//=======================================
	// �N���`�F�b�N

	// �z���g�ɋN�����H �r���ňُ�I�����Ă��Ȃ����H
	//if( m_bRectProc ) return true;
	if( m_bRectProc ) {
		if( WAIT_TIMEOUT == WaitForSingleObject( m_pInfoRect.hProcess, 300) ) { 
			return true;	// �����Ă���
		} else {
			LOG(em_ERR), "[%s] �O�`�Z�o�\�t�g ���ُ�I�����Ă����\���L��B�ė���", my_sThreadName);
			m_bRectProc = false;
		}
	}



	//=======================================
	// �N������v���Z�X���̂��擾
	char cPath[MAX_PATH];
	GetPrivateProfileString(HT_DEFECT, "RECT_EXE", "", cPath, MAX_PATH, TASKINI_NAME);

	char cCmd[MAX_PATH];					// ����
	memset(cCmd, 0x00, sizeof(cCmd));

	if( 0==m_nMen)  { sprintf(cCmd, "%s", cPath);}		// ���݈̂���1
	else			{ sprintf(cCmd, "%s %d", cPath, m_nMen); }
	sWk.Format("RECT_%d_ID", m_nMen);
	int nID = GetPrivateProfileInt(HT_DEFECT, sWk, 0, TASKINI_NAME);
	

	//=======================================
	// �N��
	// STARTUPINFO�̓��e���擾
	STARTUPINFO si;
	GetStartupInfo(&si);	
	si.dwFlags = 0;

	// �N��
	memset(&m_pInfoRect, 0x00, sizeof(m_pInfoRect));
	BOOL bRetc = CreateProcess( NULL, cCmd, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &m_pInfoRect );		// NULL:�\  1:��
	if( ! bRetc) {					// �������s
		LOG(em_ERR), "[%s] �O�`�Z�o�\�t�g �������s [err_code = %d]", my_sThreadName, GetLastError());
		syslog(100, "[�O�`�Z�o�\�t�g err_code = %d]", GetLastError() );

		KizuFunction::SetStatus(nID, false, false );
		return false;
	}

	// �N���̃`�F�b�N
	DWORD	nExitCode;
	GetExitCodeProcess(m_pInfoRect.hProcess, &nExitCode);
	if( nExitCode != STILL_ACTIVE ) {
		// �I����������Ă�
		switch( nExitCode ) {
		case NCL_PROCESS_NOERR:
			LOG(em_ERR), "[%s] �摜�����\�t�g �N�����s [NCL_PROCESS_NOERR]", my_sThreadName);
			syslog(101, "[NCL_PROCESS_NOERR]");
			break;
		case NCL_PROCESS_ERR_STARTUP:
			LOG(em_ERR), "[%s] �摜�����\�t�g �N�����s [NCL_PROCESS_ERR_STARTUP]", my_sThreadName);
			syslog(101, "[NCL_PROCESS_ERR_STARTUP]");
			break;
		case NCL_PROCESS_ERR_UNKNOWN:
			LOG(em_ERR), "[%s] �摜�����\�t�g �N�����s [NP_CLROCESS_ERR_UNKNOWN]", my_sThreadName);
			syslog(101, "[NCL_PROCESS_ERR_UNKNOWN]");
			break;
		default:
			LOG(em_ERR), "[%s] �摜�����\�t�g �N�����s [�s��:%d]", my_sThreadName, nExitCode);
			syslog(101, "[�s��:%d]", nExitCode);
			break;
		}
		KizuFunction::SetStatus(nID, false, false );
		return false;
	}

	KizuFunction::SetStatus(nID, true, false );

	LOG(em_MSG), "[%s] �O�`�Z�o�\�t�g �N������ [%s]", my_sThreadName, cPath);
	m_bRectProc = true;
	return true;
}


//------------------------------------------
// �O�`�Z�o�v���Z�X �I��
// �߂�l true:����I��  false:����I������Ȃ�
//------------------------------------------
bool MainInctance::DisposeProcess_RectProc()
{
	if( !m_bRectProc ) return true;


	//// �O�`�Z�o�^�X�N �I��
	TerminateProcess(m_pInfoRect.hProcess, -1);

	// ����������܂ł�����Ƒ҂�
	Sleep(PROCESS_PIPE_WAIT_TIME);
	LOG(em_MSG), "[%s] �O�`�Z�o�\�t�g �I��", my_sThreadName);
	m_bRectProc		= false;

	CString sWk;
	sWk.Format("RECT_%d_ID", m_nMen);
	int nID = GetPrivateProfileInt(HT_DEFECT, sWk, 0, TASKINI_NAME);
	KizuFunction::SetStatus(nID, true, false );

	return true;
}



//------------------------------------------
// EPC�摜�����\�t�g�N�� �葱��
// �߂�l true:�N������  false:�N�����s
//------------------------------------------
bool MainInctance::ExecuteProcess_DetectProc()
{
	//=======================================
	// �N���`�F�b�N
	if( m_emKindHt == DIV_HANTEI_HT ) {
		if( m_bDetectProc ) return true;
	} else if( m_emKindHt == DIV_HANTEI_PR ) {
		if( m_bDetectProc_pr ) return true;
	} else {
		if( m_bDetectProc_pd ) return true;
	}


	//=======================================
	// �N������v���Z�X���̂��擾
	char cPath[MAX_PATH];
	if( m_emKindHt == DIV_HANTEI_HT ) {
		GetPrivateProfileString(HT_DEFECT, "DETECT_EXE", "", cPath, MAX_PATH, TASKINI_NAME);
	} else if (m_emKindHt == DIV_HANTEI_PR) {
		GetPrivateProfileString(HT_DEFECT, "DETECT_PR_EXE", "", cPath, MAX_PATH, TASKINI_NAME);
	} else {
		GetPrivateProfileString(HT_DEFECT, "DETECT_PD_EXE", "", cPath, MAX_PATH, TASKINI_NAME);
	}
	_ASSERT(strlen(cPath));


	//=======================================
	// �T���v���Z�X���N������Ă�����A�E���Ă�����{
	if( m_emKindHt == DIV_HANTEI_HT ) {
		if( m_bDetectProc )		DisposeProcess_DetectProc();
	} else if (m_emKindHt == DIV_HANTEI_PR) {
		if( m_bDetectProc_pr )	DisposeProcess_DetectProc();
	} else {
		if( m_bDetectProc_pd )	DisposeProcess_DetectProc();
	}
	if( m_bAdjustProc )	DisposeProcess_iPortViewer();


	//=======================================
	// �摜�����G���W���f�o�b�N�\�`�F�b�N
	CWnd*   cwnd		= ((CHT_DEFECTApp*)AfxGetApp())->GetMainWnd();	// �g���₷���悤�ɃG�C���A�X
	bool bDebugNC		= false;										// �摜�����G���W���f�o�b�N���� true
	if( cwnd->IsDlgButtonChecked(IDC_DEBUG_MANUAL ) ) {
		bDebugNC = !! cwnd->IsDlgButtonChecked(IDC_DEBUG_NC);
	}

	//=======================================
	// �N��
	if( ! bDebugNC ) {
		// STARTUPINFO�̓��e���擾
		STARTUPINFO si;
		GetStartupInfo(&si);	
		si.dwFlags = 0;

		// �N��
		memset(&m_pInfo, 0x00, sizeof(m_pInfo));
		BOOL bRetc = CreateProcess( cPath, "", NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &m_pInfo );
		if( ! bRetc) {					// �������s
			LOG(em_ERR), "[%s] �摜�����\�t�g �������s [err_code = %d]", my_sThreadName, GetLastError());
			syslog(100, "[�摜�����\�t�g err_code = %d]", GetLastError() );
			return false;
		}

		// �����オ��܂ł�����Ƒ҂�
		Sleep( PROCESS_INIT_WAIT_TIME );

		// �N���̃`�F�b�N
		DWORD	nExitCode;
		GetExitCodeProcess(m_pInfo.hProcess, &nExitCode);
		if( nExitCode != STILL_ACTIVE ) {
			// �I����������Ă�
			switch( nExitCode ) {
			case NCL_PROCESS_NOERR:
				LOG(em_ERR), "[%s] �摜�����\�t�g �N�����s [NCL_PROCESS_NOERR]", my_sThreadName);
				syslog(101, "[NCL_PROCESS_NOERR]");
				break;
			case NCL_PROCESS_ERR_STARTUP:
				LOG(em_ERR), "[%s] �摜�����\�t�g �N�����s [NCL_PROCESS_ERR_STARTUP]", my_sThreadName);
				syslog(101, "[NCL_PROCESS_ERR_STARTUP]");
				break;
			case NCL_PROCESS_ERR_UNKNOWN:
				LOG(em_ERR), "[%s] �摜�����\�t�g �N�����s [NP_CLROCESS_ERR_UNKNOWN]", my_sThreadName);
				syslog(101, "[NCL_PROCESS_ERR_UNKNOWN]");
				break;
			default:
				LOG(em_ERR), "[%s] �摜�����\�t�g �N�����s [�s��:%d]", my_sThreadName, nExitCode);
				syslog(101, "[�s��:%d]", nExitCode);
				break;
			}
			return false;
		}
	} else {
		// �f�o�b�J����N������t
		LOG(em_MSG), "[%s] ������3�b�� �蓮�̉摜�����\�t�g �N�� ��҂��܂�", my_sThreadName, cPath);
		Sleep(3000);
	}


	LOG(em_MSG), "[%s] �摜�����\�t�g �N������ [%s]", my_sThreadName, cPath);
	if( m_emKindHt == DIV_HANTEI_HT ) 		m_bDetectProc	= true;
	else if( m_emKindHt == DIV_HANTEI_PR )	m_bDetectProc_pr= true;
	else							  		m_bDetectProc_pd= true;



	//=======================================
	//// �摜�����^�X�N�� �ʐM�����ׂĊm��
	// ����
	gcls_pCom->SetPipeName(EPC_PIPE_COMMAND);
	gcls_pData->SetPipeName(EPC_PIPE_DATA);
	gcls_pData->SetSmemName(EPC_SMEM_DATA);

	// �ڑ��v��
	gcls_pCom->SetEvPipeOpen();
	gcls_pData->SetEvPipeOpen();


	//=======================================
	//// �ڑ��m�F

	// ������Ƒ҂�
	for(int ii=0; ii<5; ii++) {
		if( gcls_pCom->IsConnect() && gcls_pData->IsConnect() ) break;
		Sleep( 1100 );
	}

	// �Ƃ肠�����ȈՓI�����A�ڑ��m���������m�F
	if( ! gcls_pCom->IsConnect() ) {
		LOG(em_ERR), "[%s] �摜�����\�t�g �N�����s [�R�}���h�p�C�v:���ڑ�]", my_sThreadName);
		syslog(101, "[�R�}���h�p�C�v:���ڑ�]");
		return false;
	}
	if( ! gcls_pData->IsConnect() ) {
		LOG(em_ERR), "[%s] �摜�����\�t�g �N�����s [�f�[�^�p�C�v:���ڑ�]", my_sThreadName);
		syslog(101, "[�f�[�^�p�C�v:���ڑ�]");
		return false;
	}
	LOG(em_MSG), "[%s] �摜�����\�t�g�� �ʐM�m������", my_sThreadName);
	return true;
}


//------------------------------------------
// �J���������\�t�g�N��
// �߂�l true:�N������  false:�N�����s
//------------------------------------------
bool MainInctance::ExecuteProcess_iPortViewer()
{
	//=======================================
	// �N���`�F�b�N
	if( m_bAdjustProc ) return true;

	//=======================================
	// �N������v���Z�X���̂��擾
	char cPath[MAX_PATH];
	char cCPath[MAX_PATH];
	if (m_emKindHt == DIV_HANTEI_HT) {
		GetPrivateProfileString(HT_DEFECT, "IPORTVIEWER_EXE", "", cPath, MAX_PATH, TASKINI_NAME);
		GetPrivateProfileString(HT_DEFECT, "IPORTVIEWER_PATH", "", cCPath, MAX_PATH, TASKINI_NAME);
	}
	else if (m_emKindHt == DIV_HANTEI_PR) {
		GetPrivateProfileString(HT_DEFECT, "IPORTVIEWER_PR_EXE", "", cPath, MAX_PATH, TASKINI_NAME);
		GetPrivateProfileString(HT_DEFECT, "IPORTVIEWER_PR_PATH", "", cCPath, MAX_PATH, TASKINI_NAME);
	}
	else {
		GetPrivateProfileString(HT_DEFECT, "IPORTVIEWER_PR_EXE", "", cPath, MAX_PATH, TASKINI_NAME);
		GetPrivateProfileString(HT_DEFECT, "IPORTVIEWER_PR_PATH", "", cCPath, MAX_PATH, TASKINI_NAME);
	}
	_ASSERT(strlen(cPath));


	//=======================================
	// �T���v���Z�X���N������Ă�����A�E���Ă�����{
	if( m_bDetectProc || m_bDetectProc_pd || m_bDetectProc_pr ) DisposeProcess_DetectProc();
	if( m_bAdjustProc )						DisposeProcess_iPortViewer();


	//=======================================
	// �N��

	// STARTUPINFO�̓��e���擾
	STARTUPINFO si;
	GetStartupInfo(&si);	
	si.dwFlags = 0;

	// �N��
	//BOOL bRetc = CreateProcess( cPath, "", NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &m_pInfo );
	BOOL bRetc = CreateProcess( cPath, "", NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, cCPath, &si, &m_pInfo );
	if( ! bRetc) {					// �������s
		LOG(em_ERR), "[%s] �J���������\�t�g �������s [err_code = %d]", my_sThreadName, GetLastError());
		syslog(100, "[�J���������\�t�g: err_code = %d]", GetLastError() );
		return false;
	}

	// �����オ��܂ł�����Ƒ҂�
	Sleep( PROCESS_INIT_WAIT_TIME );
	m_bAdjustProc	 = true;

	LOG(em_MSG), "[%s] �J���������\�t�g �N������ [%s]", my_sThreadName, cPath);
	return true;
}


//------------------------------------------
// EPC�摜�����\�t�g�I��
// �߂�l true:����I��  false:����I������Ȃ�
//------------------------------------------
bool MainInctance::DisposeProcess_DetectProc()
{
	if( !m_bDetectProc && !m_bDetectProc_pd && !m_bDetectProc_pr ) return true;

	//// �������̏ꍇ�́A�X�g�b�v���s���B
	if( gcls_Status.IsRunning() ) {
		if (DIV_HANTEI_PR != m_emKindHt)
		{
			DetectStop(1, 0);
		}
		else
		{
			ResumeStop(1, 0);
		}
	}

	//// �ؒf���Ă��G���[���o�Ȃ��悤��
	gcls_pCom->SetRunExit();
	gcls_pData->SetRunExit();

	//// �摜�����^�X�N �I��
	SendCommand_Exit();								// �I���R�}���h���M

	//// �v���Z�X�̏I�����ő�T�b�ԑ҂��Ă݂�
	if( WAIT_TIMEOUT == WaitForSingleObject( m_pInfo.hProcess, PROCESS_EXIT_WAIT_TIME) ) { 
		LOG(em_ERR), "[%s] �摜�����\�t�g �I�����s", my_sThreadName);
		// �܂������Ă���Ȃ狭���I��
		TerminateProcess(m_pInfo.hProcess, -1);
	}
	// ����������܂ł�����Ƒ҂�
	Sleep(PROCESS_PIPE_WAIT_TIME);
	LOG(em_MSG), "[%s] �摜�����\�t�g �I��", my_sThreadName);
	m_bDetectProc		= false;
	m_bDetectProc_pd	= false;
	m_bDetectProc_pr	= false;

	//// �摜�����^�X�N�� �ʐM�����ׂĐؒf
	gcls_pCom->SetEvPipeClose();
	gcls_pData->SetEvPipeClose();
	LOG(em_MSG), "[%s] �摜�����\�t�g�� �ʐM�ؒf����", my_sThreadName);
	return true;
}

//------------------------------------------
// �J���������\�t�g �I��
// �߂�l true:����I��  false:����I������Ȃ�
//------------------------------------------
bool MainInctance::DisposeProcess_iPortViewer()
{
	if( !m_bAdjustProc ) return true;
	
	//// \mailslot\iPortViewr�փ��[���X���b�g
	// ���e�͉��ł��ǂ����ǁA�Ƃ肠�����A�I���R�[�h�𑗂�
	COMMON_QUE que;
	que.nEventNo = FACT_KS_END;
	send_mail(EPC_MAIL_IPORTVIEWER, ".", &que);

	//// �v���Z�X�̏I�����ő�T�b�ԑ҂��Ă݂�
	if( WAIT_TIMEOUT == WaitForSingleObject( m_pInfo.hProcess, PROCESS_EXIT_WAIT_TIME) ) { 
		LOG(em_ERR), "[%s] �J���������\�t�g �I�����s", my_sThreadName);
		// �܂������Ă���Ȃ狭���I��
		TerminateProcess(m_pInfo.hProcess, -1);
	}
	// ����������܂ł�����Ƒ҂�
	Sleep(PROCESS_PIPE_WAIT_TIME);
	LOG(em_MSG), "[%s] �J���������\�t�g �I��", my_sThreadName);
	m_bAdjustProc		= false;

	return true;
}
#pragma endregion

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �V�[�P���X����


//------------------------------------------
// �����J�n����
//		�� �ʏ펞�́A�����̊��蓖�āA�p�����[�^�ݒ�A�X�^�[�g �����{
//		�� �Z�����́A�摜�����^�X�N�I���AiPortViewer�N��
// COMMON_QUE* que �N���v��
//------------------------------------------
void MainInctance::DetectStart(COMMON_QUE* que)
{
	int					nRetc = 0;											// ���� (0:���� -1:�d�̏� 1:�y�̏�)
	CString				sWk[HT_CONNECT_NUM];

	//// ������
	EM_DIV_UNTEN		unten	= (EM_DIV_UNTEN) que->fl.data[0];			// �^�]���[�h
	int					nScode[2];
						nScode[0] = que->fl.data[1];						// �����\�ʏ�� �\
						nScode[1] = que->fl.data[2];						// �����\�ʏ�� ��

	int					nLen[2];
						nLen[0] = que->fl.data[3];
						nLen[1] = que->fl.data[4];

	int					pcid = (getpcid() -1);								// PCID(PCKIND�ł͂Ȃ�) (0�I���W��)
	int					campair	= que->fl.data[ 5 + 2*pcid + 0];			// �J�����y�A 1�` (0:�o�b�N�A�b�v)   �}�C�i�X�̏ꍇ�ُ�J�����S��
	bool				bNgCam = false;										// �̏�J�����S�����H �̏᎞true
	if (0 > campair) {
		campair = campair*-1;		// �����ɖ߂�
		bNgCam = true;
		LOG(em_WAR), "[%s] �ُ�J�����S������(%d)", my_sThreadName, campair );
	}

	EM_DIV_HANTEI		emKind	= (EM_DIV_HANTEI)que->fl.data[ 5 + 2*pcid + 1];	// �S���@�\ (1:����)
	KizuPcInfManager*	cls_Pc  = (KizuPcInfManager*)getcls_pcinfmgr();		// PC���N���X
	int					nMen	= (0 == ((campair-1)/(NUM_CAMPAIR/NUM_MEN)) ? 0 : 1);											


	//// �o�b�N�A�b�v�@�Ȃ� ����ȍ~��������
	if( 0 == campair ) {
		m_nCamPair = 0;	
		m_nMen = -1;
		return;
	}

	//// ���X�e�[�^�X�m�F
	if( StatusManager::STATE_INIT != gcls_Status.GetMainState() ) {
		LOG(em_ERR), "[%s] [%s] ���Ɍ����J�n�v����M", my_sThreadName, gcls_Status.GetMainStateName() );
		return ;
	}
	LOG(em_MSG), "[%s] �����J�n�v���B(s=%d:%d men=%d campair=%d kind=%d[1:���� 2:���� 3:�p������] �f�[�^���M:%s)", my_sThreadName, nScode[0], nScode[1], nMen, campair, emKind, (gcls_pSend->IsFrameSend() ? "����" : "�Ȃ�"));


	//// �����o�[�ϐ��ɓo�^
	m_nAppParam = 0;	// �G���[�����Z�b�g
	gcls_Status.SetUnten( unten );
	m_nMen			= nMen;
	m_nCamPair		= campair;
	if( DIV_HANTEI_HT == emKind ) {
		m_emKindHt	= DIV_HANTEI_HT;
	} else {
		m_emKindHt	= DIV_HANTEI_CY; 
	}

	m_nNowScode[0]	= nScode[nMen];

	//// �ُ�J����
	m_bNgCam = bNgCam;
	gcls_pCom->SetNgCam(bNgCam);

	//// �����J�n���ɁA�������킩��Ȃ��ꍇ�́A������xDB����PC�����擾���� (�Ȃ�DB�̒��q��������΍�)
	if( m_emKindHt == DIV_HANTEI_HT) {
		if( 0 == memcmp( &"*", HANTEI( campair-1), 1) ) {
			LOG(em_ERR), "[%s] �����̖��O�s��", my_sThreadName );
			getpcname();
		}
		LOG(em_MSG), "[%s] ������PC����[%s]", my_sThreadName, HANTEI( campair-1));  
		for(int ii=0; ii<HT_CONNECT_NUM; ii++) gcls_pSock[ii]->SetPcName( HANTEI( campair-1) ); 

	} else {
		if( 0 == memcmp( &"*", CYCLE( campair-1), 1) ) {
			LOG(em_ERR), "[%s] �����̖��O�s��", my_sThreadName );
			getpcname();
		}
		LOG(em_MSG), "[%s] ������PC����[%s]", my_sThreadName, CYCLE( campair-1));  
		for(int ii=0; ii<HT_CONNECT_NUM; ii++) gcls_pSock[ii]->SetPcName( CYCLE( campair-1) ); 
	}


	
	//// �p�����[�^�ǂݒ���
	//gcls_PrmMgr.SetEv_PrmRead();
	gcls_PrmMgr.ReadParam_All();	//���� �Ǎ��ɕύX



	////////////////////////////////////
	// �J��������
	////////////////////////////////////
	if( DIV_UNTEN_CAMERA == unten ) {				// �J����������

		// iPortViewer �N��
		if( ! ExecuteProcess_iPortViewer() ) {
			// �ُ�
			nRetc = -1;
			SendState_PcCanNot();
		}
		// �J�����������́A�����OK
		goto ending;
	}

	////////////////////////////////////
	// �ʏ� ���� �T���v���B�e ���� �V�~�����[�V����
	////////////////////////////////////

	//// �O�`�Z�o�v���Z�X �N��
	if( ! ExecuteProcess_RectProc() ) {
		// �ُ� ��		
	}

	//// �摜�����\�t�g �N��
	if (!m_bNgCam) {
		if (!ExecuteProcess_DetectProc()) {
			// �ُ�
			nRetc = -1;
			SendState_PcCanNot();
		}
	}




	//---------------------------------------
	//// �����Ɖ���ڑ����s��
	int					nNum[HT_CONNECT_NUM];
	for(int ii=0; ii<HT_CONNECT_NUM; ii++) nNum[ii] = 0;

	// ini�L�[����
	// �\ ���H
	if( m_emKindHt == DIV_HANTEI_HT) {
		sWk[0].Format("%s_TO_%d", HT_DEFECT, m_nMen );
	} else {
		sWk[0].Format("%s_PD_TO_%d", HT_DEFECT, m_nMen);
	}
	// �ڑ�������
	nNum[0] = gcls_Status.GetCamPairItemNum(m_nCamPair, m_emKindHt);

#ifndef SOT_TEST
	for(int ii=0; ii<HT_CONNECT_NUM; ii++) {
		// �ݒ�
		gcls_pSock[ii]->SetSession( sWk[ii] );
		gcls_pSock[ii]->SetSendNum( nNum[ii] );

		// �񓯊��ڑ� (���g���C�͍s��Ȃ��B�ʏ�q����Ȃ��͂��͂Ȃ��B1��_���Ȃ�������߂̂͂��B)
		gcls_pSock[ii]->SetConnect(false);		
	}


	//// �����Ƃ̉�����m���o�����H
	int cnt=0;
	bool bIsConnect = true;
	while(true) {
		cnt++;
		
		// �S���Ȃ����Ă��邩�H
		bIsConnect = true;
		for(int ii=0; ii<HT_CONNECT_NUM; ii++) {
			bIsConnect &= gcls_pSock[ii]->IsConnect();
		}

		if( bIsConnect ) {
			LOG(em_MSG), "[%s] �����Ɖ���ڑ������I", my_sThreadName);		
			break;
		} else {
			if( 10 > cnt ) {
				LOG(em_MSG), "[%s] �����Ɖ���ڑ����E�E�E %d���", my_sThreadName, cnt);	
				Sleep(100);
			} else {
				LOG(em_MSG), "[%s] �����Ɖ���ڑ����s", my_sThreadName);
				for(int ii=0; ii<HT_CONNECT_NUM; ii++) gcls_pSock[ii]->SetDisConnect(); 
				nRetc = -1;
				SendState_PcCanNot();
				goto ending;
			}
		}
	}
#endif

	//// �������R�}���h
	nRetc = SendCommand_Init();
	if( 0 != nRetc ) {
		if(-1 == nRetc) SendState_PcCanNot();

		nRetc = -1;
		goto ending;
	}


	// ------------------------------------------------------------------------------
	//// ������� (����ȍ~�̃G���[�̏ꍇ�́A�����J���̏������K�v��)
	gcls_Status.SetMainState(StatusManager::STATE_WAIT);
	//Sleep(200);												// �摜�����^�X�N�������������܂ł�����Ƒ҂�
	// �P�ƋN�����ɏ������R�}���h���M���Ă���RPC�T�[�o�[�̕R�Â��܂ł̎��Ԃ��Z���������߁A�҂����Ԃ�10�b�ɕύX
	Sleep(10000);												// �摜�����^�X�N�������������܂ł�����Ƒ҂�

	//// ���̑�����
	gcls_pSend->ClearFrameNo();
	gcls_pSend->SetCamPair(m_nCamPair);
	gcls_pSend->SetKindHt(m_emKindHt);






	//// ���������J�E���^
	NCL_CMD_SET_POSITION  pos;
	memset(&pos, 0x00, sizeof(pos));
	pos.cmd = NCL_SET_POSITION;
	pos.framenum = 0;
	pos.update_mask = 0xFFFFFFFF;
	pos.front_pos = (float)nLen[m_nMen];
	if( -1 == SendCommand_SetPosition(&pos)) {										// �d�̏�
		DetectStop(-1, 0);											// �����ւ̑��M�͖��� 
		nRetc = -1;
		SendState_PcCanNot();
		goto ending;
	} 


	//// �p�����[�^���O�Ǎ�
	if( -1 == SendCommand_PreLoad_Param( m_nNowScode[0] ) ) {
		DetectStop(-1, 0);											// �����ւ̑��M�͖��� 
		nRetc = -1;
		SendState_PcCanNot();
		goto ending;
	} 
	//// �w�茟���\�ʏ�ԃR�[�h��MAX�ȊO�̏ꍇ�AMAX_SCODE�̎��O�Ǎ������{����
	//// ��������_WPD�`�����ʒu�ԂŃC�j�V�����d������M�����ꍇ�̑΍�
	//// �@MAX_SCODE�����O�Ǎ����Ă����Ȃ��Ɖ摜�G���W�����_�E������
	if (MAX_SCODE != m_nNowScode[0])
	{
		//// �p�����[�^���O�Ǎ�(�����\�ʏ��MAX�l)
		if (-1 == SendCommand_PreLoad_Param(MAX_SCODE)) {
			DetectStop(-1, 0);										// �����ւ̑��M�͖��� 
			nRetc = -1;
			SendState_PcCanNot();
			goto ending;
		}
	}

	//// �p�����[�^�ύX�R�}���h
	if( -1 == SendCommand_Param(0, m_nNowScode[0])) {				// �d�̏�
		DetectStop(-1, 0);											// �����ւ̑��M�͖��� 
		nRetc = -1;
		SendState_PcCanNot();
		goto ending;
	} 


	//// �T���J�n�R�}���h
	if( -1 == SendCommand_Start() ) {
		DetectStop(-1, 0);										// �����ւ̑��M�͖��� 
		nRetc = -1;
		SendState_PcCanNot();
		goto ending;
	}


	// ���W�b�N�t�@�C����ԕ\��
	gcls_Logic.Check(0);


	//// �w���V�[�₢���킹�J�n
	__int64 ts = (10 * 1000) * -10000;			// 1�~���b��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evHelcy, (LARGE_INTEGER *)&ts, STAT_CHECK_INTERVAL, NULL, NULL, FALSE);
	gcls_pSend->StartHls();

	//// �J����GCP�`�F�b�N���J�n
	GcpCheck_StartTimer();

	//---------------------------------------
	//// �S�������Ɛڑ�

#ifdef FRAME_RAW_SEND
	//if (!m_bNgCam) {
	if( DIV_HANTEI_HT == m_emKindHt	) {
		int nCamset = gcls_Status.GetCamPairItemCamset(m_nCamPair, 0, m_emKindHt) ;

		// ===>> �⊪ 2022.07.20 ������~�J�n��A�S���摜����������Ȃ��s��C��		
		// �L���[�̉���i�O�̂��߁j
		for (int ii = 0; ii < FRAME_HT_SEND_PORT; ii++) {
			int num = gque_pSockRpi[ii]->GetCountPool();
			if (0 < num)
			{
				LOG(em_ERR), "[%s] �����J�n���ɑS���摜���M�L���[����[ii=%d, num=%d]", my_sThreadName, ii, num);
				syslog(130, "[ii=%d, num=%d]", ii, num);
				QueueFree();
				break;
			}
		}
		// <<===
		
		for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
			gcls_pSockRpi[ii]->StartLock(THREAD_PRIORITY_TIME_CRITICAL);
			gcls_pCull[ii]->StartLock();
		}
		Sleep(100);

		for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
			int nId = (nCamset + ii - 1) % NUM_CAM_POS;
			sWk[0].Format("%s_RPI_TO_%d_%d", HT_DEFECT, m_nMen, nId);
			gcls_pSockRpi[ii]->SetSession(sWk[0]);
			gcls_pSockRpi[ii]->SetRetryMode(true);
			gcls_pSockRpi[ii]->SetConnect();
		}
	}
#endif



ending:

	////////////////////////////////////
	// �Ō�̏���
	////////////////////////////////////


	// �����֌��ʂ𑗐M
	int			mailRetc;
	COMMON_QUE	sque;
	sque.nEventNo = FACT_TO_DEFECT_06;				// ����J�n�A���T�[
	sque.nLineNo = getlineid();

	sque.fl.data[0] = m_nCamPair;					// �J�����y�A=PcKind
	sque.fl.data[1] = getpcid();					// PcId
	sque.fl.data[2] = nRetc;						// ����
	sque.fl.data[3] = (int)emKind;					// �S������
	mailRetc = send_mail(TO_DEFECT, TOKATU, &sque);
	if( 0 != mailRetc ) syslog(SYSNO_MAIL_SNED_ERR, "[�����J�n�A���T�[ err_code=%d]", mailRetc);

	// ����H�ُ�
	if( 0 == nRetc ) {
		SendState_Grab(0, true);
		SendState_App(0);
		gcls_Status.SetMainState(StatusManager::STATE_RUN);			 // ������

	} else {
		SendState_Grab(0, false);
		SendState_App(APP_PARAM_EXEC);	// ���s���s
	}
}

//------------------------------------------
// ������~����
//		�� �ʏ펞�́A�X�g�b�v�A�����J�� �����{
//		�� �Z�����́AiPortViewer�I���A�摜�����^�X�N�N��
// int mode �v����� (0:����ȏI���v��  1:�������������ċ����I�ɏI���v�� -1:���M���Ȃ�)
// int ansRetc ���� (0:���� 1:�y�̏� -1:�d�̏�) �� �������ɁA�d�̏Ⴊ���������ꍇ�́A�d�̏�ɂȂ�
//------------------------------------------
void MainInctance::DetectStop(int mode, int ansRetc)
{
	////////////////////////////////////
	// ������
	////////////////////////////////////
	EM_DIV_UNTEN		unten	= gcls_Status.GetUnten();					// �^�]���[�h

	// �o�b�N�A�b�v�@�Ȃ� ����ȍ~��������
	if( 0 == m_nCamPair ) return;


	// ���X�e�[�^�X�m�F
	if( StatusManager::STATE_NON	== gcls_Status.GetMainState() ||
		StatusManager::STATE_INIT	== gcls_Status.GetMainState() ||
		StatusManager::STATE_CANNOT == gcls_Status.GetMainState() ) {
		LOG(em_ERR), "[%s] [%s] ���Ɍ�����~�v����M", my_sThreadName, gcls_Status.GetMainStateName() );
		return;
	}
	LOG(em_MSG), "[%s] ������~�v���B(%s) ==============>>>", my_sThreadName, (0 == mode ? "�����~�v��" : "�ُ�ɂ�鋭����~�v��"));  


	////////////////////////////////////
	// �J��������
	////////////////////////////////////
	if( DIV_UNTEN_CAMERA == unten ) {				// �J����������

		// iPortViewer �I��
		DisposeProcess_iPortViewer();



	////////////////////////////////////
	// �ʏ� ���� �T���v���B�e ���� �V�~�����[�V����
	////////////////////////////////////
	} else {
		//// �w���V�[�^�C�}�[��~
		CancelWaitableTimer(m_evHelcy);
		gcls_pSend->StopHls();

		//// �J����GCP�`�F�b�N���~
		CancelWaitableTimer(m_evGcpCheck);

		//---------------------------------------
		// �S������ �ؒf
#ifdef FRAME_RAW_SEND
		//for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
		//	gcls_pSockRpi[ii]->SetRetryMode(false);		// ���g���C�͂������Ȃ�
		//	gcls_pSockRpi[ii]->SetDisConnect();
		//}

		for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {

			if( gcls_pSockRpi[ii]->IsThreadEvOn() ) {
				gcls_pSockRpi[ii]->SetRetryMode(false);		// ���g���C�͂������Ȃ�
				gcls_pSockRpi[ii]->SetDisConnect();
			}
		}
		Sleep(1000);		// �񓯊��őS�������ؒf���Ă��邽�߁A�ؒf�����O�ɃX���b�h��~����Ă��܂��ƈُ�I���ƂȂ�B������ƒ��߂ɑ҂�
		//Sleep(300);
		for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {		
			if( gcls_pSockRpi[ii]->IsThreadEvOn()) gcls_pSockRpi[ii]->Stop();
			if( gcls_pCull[ii]->IsThreadEvOn()) gcls_pCull[ii]->Stop();
		}
		// ===>> �⊪ 2022.07.20 ������~�J�n��A�S���摜����������Ȃ��s��C��
		// �L���[�̉���i�O�̂��߁j
		for (int ii = 0; ii < FRAME_HT_SEND_PORT; ii++) {
			int num = gque_pSockRpi[ii]->GetCountPool();
			if (0 < num)
			{
				LOG(em_ERR), "[%s] ������~���ɑS���摜���M�L���[����[ii=%d, num=%d]", my_sThreadName, ii, num);
				syslog(131, "[ii=%d, num=%d]", ii, num);
				QueueFree();
				break;
			}
		}
		//// <<===

#endif

		//---------------------------------------
		// �G���W��

		//// �������̏ꍇ�́A��~
		if( StatusManager::STATE_RUN == gcls_Status.GetMainState() ) {
			// ��~�R�}���h
			if( 1 == SendCommand_Stop() ) {
				if(-1 != ansRetc) ansRetc = 1;
			}
			gcls_Status.SetMainState(StatusManager::STATE_WAIT);
			Sleep(200);										// �摜�����^�X�N�������������܂ł�����Ƒ҂�
		}

		//// �����J��
		if( -1 == SendCommand_Dispose() ) {
			ansRetc = -1;
			SendState_PcCanNot();
			goto ending;
		}
	}

ending:

	////////////////////////////////////
	// �Ō�̏���
	////////////////////////////////////
	// �����֌��ʂ𑗐M
	if( -1 != mode ) {
		int			mailRetc;
		COMMON_QUE	sque;
		sque.nEventNo = FACT_TO_DEFECT_07;				// �����~�A���T�[
		sque.nLineNo = getlineid();
		sque.fl.data[0] = m_nCamPair;					// �J�����y�A=PcKind
		sque.fl.data[1] = getpcid();					// PcId
		sque.fl.data[2] = ansRetc;						// ����
		sque.fl.data[3] = mode;							// �敪 (0:�v���ɑ΂���A���T�[ 1:�ُ픭����)
		sque.fl.data[4] = m_emKindHt;

		mailRetc = send_mail(TO_DEFECT, TOKATU, &sque);
		if( 0 != mailRetc ) syslog(SYSNO_MAIL_SNED_ERR, "[������~�A���T�[ err_code=%d]", mailRetc);
	}
	if(0 == ansRetc) { LOG(em_MSG), "[%s] ������~�������� <<<========================", my_sThreadName); }
	else			 { LOG(em_ERR), "[%s] ������~���������B����=%d <<<========================", my_sThreadName, ansRetc); }

	// �X�e�[�^�X �������
	if( -1 != ansRetc )	gcls_Status.SetMainState(StatusManager::STATE_INIT);
}



//------------------------------------------
// ��Ԏ擾
//------------------------------------------
void MainInctance::HelcyStat()
{
	NCL_ANSWER_QUERY_STATE	ans;
	ENG_STAT				st;

	memset( &ans, 0x00, sizeof(ans));
	memset( &st, 0x00, sizeof(st));


	//---------------------------
	// �G���W�����
	int		nRetc;					// ���� (0:���� -1:�d�̏� 1:�y�̏�)

	nRetc = SendCommand_Query_State(&ans);
	if( 0 == nRetc && ( StatusManager::STATE_RUN == gcls_Status.GetMainState() ) ) {
		if(NCL_PREINIT == ans.state || NCL_TRANSITION == ans.state || NCL_INITIALIZED == ans.state ) {
			LOG(em_ERR), "[%s] [%s] ���ɃG���W����~(%d)", my_sThreadName, gcls_Status.GetMainStateName(), ans.state );
			nRetc = -1;
		}
	}
	if(0 != nRetc) {
		if (DIV_HANTEI_PR != m_emKindHt)
		{
			DetectStop(-1, 0);
		}
		else
		{
			ResumeStop(-1, 0);
		}
		nRetc = -1;
		SendState_PcCanNot();
		return;
	}

	//LOG(em_INF), "[%s] �G���W����� ENG������ %d num:%d id:%d %d temp:%.1f %.1f", my_sThreadName, ans.state, ans.camnum, ans.camid[0], ans.camid[1], ans.temp[0], ans.temp[1]);

	// �Z�b�g
	st.camnum = ans.camnum;
	for(int ii=0; ii<(int)st.camnum; ii++) {
		st.camid[ii] = ans.camid[ii];

		for(int jj=0; jj<(MAX_CAMANGLE - 1); jj++) {
			st.exptime[jj][ii] = ans.exptime[jj][ii];
			st.gain[jj][ii] = ans.gain[jj][ii];
		}
		st.temp[ii] = ans.temp[ii];
		st.volt[ii] = ans.volt[ii];
	}
	gcls_Status.SetEngStat(&st); 
	//LOG(em_INF), "[%s] �G���W����� ���聨���� %d num:%d id:%d %d temp:%.1f %.1f", my_sThreadName, st.state, st.camnum, st.camid[0], st.camid[1], st.temp[0], st.temp[1]);

}

//------------------------------------------
// �r���J�n����
//		�� �ʏ펞�́A�����̊��蓖�āA�p�����[�^�ݒ�A�X�^�[�g �����{
//		�� �Z�����́A�摜�����^�X�N�I���AiPortViewer�N��
// COMMON_QUE* que �N���v��
//------------------------------------------
void MainInctance::ResumeStart(COMMON_QUE* que)
{
	int					nRetc = 0;											// ���� (0:���� -1:�d�̏� 1:�y�̏�)
	CString				sWk[HT_CONNECT_NUM];

	// L[0]  �^�]���[�h�i1�F�ʏ�  2�F�e�X�g[����َB�e]  3�F�Z��[��ג���]�j
	// L[1]  �\�� �����\�ʏ��No�i1�`�j
	// L[2]  ���� �����\�ʏ��No�i1�`�j
	// L[3]  �\�� ��������[mm]
	// L[4]  ���� ��������[mm]
	// L[5]  �r���Q���t���[��No �i0�F���� �j
	// L[6 + 2 �~�i����ID-1�j + 0]  PCID���� �i����p�A�����p�F�J�����y�AID�j�܂��́i���W�b�N�p�������p�F�J�����Z�b�g�P�j���Z�b�g�i1�`�F�I�����C��   0�F�ޯ����� or �ΏۊO)
	// L[6 + 2 �~�i����ID-1�j + 1]  PCID���� �i����p�A�����p�F���g�p�j�܂��́i���W�b�N�p�������p�F�J�����Z�b�g�Q�j���Z�b�g�i1�`�F�I�����C��   0�F�ޯ����� or �ΏۊO)
	// L[6 + 2 �~�i����ID-1�j + 2]  PCID���� �Y������@�\���Z�b�g  ����@�\ �i1�F�P���@�\  2�F�����@�\  0�F�ޯ����� 3�F���W�b�N�p�������j
	EM_DIV_UNTEN		unten = (EM_DIV_UNTEN)que->fl.data[0];				// �^�]���[�h
	int					nScode[2];
	nScode[0] = que->fl.data[1];											// �����\�ʏ�� �\
	nScode[1] = que->fl.data[2];											// �����\�ʏ�� ��

	int					nLen[2];
	nLen[0] = que->fl.data[3];
	nLen[1] = que->fl.data[4];

	int					nStartFrameNo = que->fl.data[5];
	int					pcid = (getpcid() - 1);								// PCID(PCKIND�ł͂Ȃ�) (0�I���W��)
	int					camset1 = que->fl.data[6 + 3 * pcid + 0];			// �J�����Z�b�g�P (1�I���W��)
	int					camset2 = que->fl.data[6 + 3 * pcid + 1];			// �J�����Z�b�g�Q (1�I���W��)

	EM_DIV_HANTEI		emKind = (EM_DIV_HANTEI)que->fl.data[6 + 3 * pcid + 2];
	KizuPcInfManager*	cls_Pc = (KizuPcInfManager*)getcls_pcinfmgr();		// PC���N���X

	// �J�����Z�b�gNo����ʂ�����o��
	// ���L���l���ݒ肳��Ă���ꍇ�̂ݎZ�o����
	// �������Ƃ������l�̏ꍇ�A�\�ʂƂ���
	int nMen = 0;
	if (0 != camset1) nMen = (0 == ((camset1 - 1) / (MAX_CAMSET / NUM_MEN)) ? 0 : 1);
	if (0 != camset2) nMen = (0 == ((camset2 - 1) / (MAX_CAMSET / NUM_MEN)) ? 0 : 1);

	if (DIV_HANTEI_PR != emKind)
	{
		LOG(em_WAR), "[%s] �S������ُ�(pcid=%d kind=%d [1:���� 2:���� 3:�p������])", my_sThreadName, getpcid(), emKind);
		nRetc = -1;
		goto ending;
	}
		
	if (0 > camset1)
	{
		LOG(em_WAR), "[%s] �J�����Z�b�g�ُ�(pcid=%d Camset1=%d)", my_sThreadName, getpcid(), camset1);
		nRetc = -1;
		goto ending;
	}

	if (0 > camset2)
	{
		LOG(em_WAR), "[%s] �J�����Z�b�g�ُ�(pcid=%d Camset2=%d)", my_sThreadName, getpcid(), camset2);
		nRetc = -1;
		goto ending;
	}

	//// ���X�e�[�^�X�m�F
	if (StatusManager::STATE_INIT != gcls_Status.GetMainState())
	{
		LOG(em_ERR), "[%s] [%s] ���ɓr���J�n�v����M(pcid=%d)", my_sThreadName, gcls_Status.GetMainStateName(), getpcid());
		nRetc = -1;
		goto ending;
	}

	// �����ւ̃f�[�^���M��ԃZ�b�g�i���M�Ȃ��j
	gcls_pSend->SetFrameSend(false);

	LOG(em_MSG), "[%s] �r���J�n�v�� (pcid=%d s=%d:%d camset1=%d camset2=%d frame=%d kind=%d[1:���� 2:���� 3:�p������] �f�[�^���M:%s)", my_sThreadName, getpcid(), nScode[0], nScode[1], camset1, camset2, nStartFrameNo, emKind, (gcls_pSend->IsFrameSend() ? "����" : "�Ȃ�"));

	//// �����o�[�ϐ��ɓo�^
	m_nAppParam = 0;	// �G���[�����Z�b�g
	gcls_Status.SetUnten(unten);
	gcls_Status.SetParaCamset(camset1, camset2);
	m_nMen = nMen;
	m_nCamPair = getpcid() - ((NUM_HANTEI * NUM_MEN) + NUM_ALL_HTBACK);
	m_emKindHt = DIV_HANTEI_PR;
	m_nNowScode[0] = nScode[m_nMen];

	//// �����J�n���ɁA�������킩��Ȃ��ꍇ�́A������xDB����PC�����擾���� (�Ȃ�DB�̒��q��������΍�)
	if (0 == memcmp(&"*", HANTEI(pcid), 1))
	{
		LOG(em_ERR), "[%s] �����̖��O�s��", my_sThreadName);
		getpcname();
	}
	LOG(em_MSG), "[%s] ������PC����[%s]", my_sThreadName, HANTEI(pcid));

	//// �p�����[�^�ǂݒ���
	gcls_PrmMgr.ReadParam_All();	//���� �Ǎ��ɕύX

	////////////////////////////////////
	// �J��������
	////////////////////////////////////
	if (DIV_UNTEN_CAMERA == unten)
	{
		// �J����������

		// iPortViewer �N��
		if (!ExecuteProcess_iPortViewer())
		{
			// �ُ�
			nRetc = -1;
			SendState_PcCanNot();
		}
		// �J�����������́A�����OK
		goto ending;
	}

	////////////////////////////////////
	// �ʏ� ���� �T���v���B�e ���� �V�~�����[�V����
	////////////////////////////////////

	//// �O�`�Z�o�v���Z�X �N��
	if (!ExecuteProcess_RectProc())
	{
		// �ُ� ��		
	}

	//// �摜�����\�t�g �N��
	if (!m_bNgCam)
	{
		if (!ExecuteProcess_DetectProc())
		{
			// �ُ�
			nRetc = -1;
			SendState_PcCanNot();
		}
	}

	//// �������R�}���h
	nRetc = SendCommand_Init();
	if (0 != nRetc)
	{
		if (-1 == nRetc) SendState_PcCanNot();
		nRetc = -1;
		goto ending;
	}

	// ------------------------------------------------------------------------------
	//// ������� (����ȍ~�̃G���[�̏ꍇ�́A�����J���̏������K�v��)
	gcls_Status.SetMainState(StatusManager::STATE_WAIT);
	// �P�ƋN�����ɏ������R�}���h���M���Ă���RPC�T�[�o�[�̕R�Â��܂ł̎��Ԃ��Z���������߁A�҂����Ԃ�10�b�ɕύX
	Sleep(10000);												// �摜�����^�X�N�������������܂ł�����Ƒ҂�

	//// ���̑�����
	gcls_pSend->ClearFrameNo();
	gcls_pSend->SetCamPair(m_nCamPair);
	gcls_pSend->SetKindHt(m_emKindHt);

	//// ���������J�E���^
	NCL_CMD_SET_POSITION  pos;
	memset(&pos, 0x00, sizeof(pos));
	pos.cmd = NCL_SET_POSITION;
	pos.framenum = 0;
	pos.update_mask = 0xFFFFFFFF;
	pos.front_pos = (float)nLen[m_nMen];
	if (-1 == SendCommand_SetPosition(&pos))
	{
		// �d�̏�
		ResumeStop(-1, 0);											// �����ւ̑��M�͖��� 
		nRetc = -1;
		SendState_PcCanNot();
		goto ending;
	}

	//// �p�����[�^���O�Ǎ�
	if (-1 == SendCommand_PreLoad_Param(m_nNowScode[0]))
	{
		// �d�̏�
		ResumeStop(-1, 0);											// �����ւ̑��M�͖��� 
		nRetc = -1;
		SendState_PcCanNot();
		goto ending;
	}
	//// �w�茟���\�ʏ�ԃR�[�h��MAX�ȊO�̏ꍇ�AMAX_SCODE�̎��O�Ǎ������{����
	//// ��������_WPD�`�����ʒu�ԂŃC�j�V�����d������M�����ꍇ�̑΍�
	//// �@MAX_SCODE�����O�Ǎ����Ă����Ȃ��Ɖ摜�G���W�����_�E������
	if (MAX_SCODE != m_nNowScode[0])
	{
		//// �p�����[�^���O�Ǎ�(�����\�ʏ��MAX�l)
		if (-1 == SendCommand_PreLoad_Param(MAX_SCODE))
		{
			// �d�̏�
			ResumeStop(-1, 0);										// �����ւ̑��M�͖��� 
			nRetc = -1;
			SendState_PcCanNot();
			goto ending;
		}
	}

	//// �p�����[�^�ύX�R�}���h
	if (-1 == SendCommand_Param(0, m_nNowScode[0]))
	{
		// �d�̏�
		ResumeStop(-1, 0);											// �����ւ̑��M�͖��� 
		nRetc = -1;
		SendState_PcCanNot();
		goto ending;
	}

	//// �r���Q���R�}���h
	if (-1 == SendCommand_Resume(nStartFrameNo))
	{
		// �d�̏�
		ResumeStop(-1, 0);											// �����ւ̑��M�͖��� 
		nRetc = -1;
		SendState_PcCanNot();
		goto ending;
	}

	// ���W�b�N�t�@�C����ԕ\��
	gcls_Logic.Check(0);

	//// �w���V�[�₢���킹�J�n
	__int64 ts = (10 * 1000) * -10000;			// 1�~���b��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evHelcy, (LARGE_INTEGER *)&ts, STAT_CHECK_INTERVAL, NULL, NULL, FALSE);
	gcls_pSend->StartHls();

	//// �J����GCP�`�F�b�N���J�n
	GcpCheck_StartTimer();

ending:

	////////////////////////////////////
	// �Ō�̏���
	////////////////////////////////////
	// �����֌��ʂ𑗐M
	ResumeResponse(nStartFrameNo, emKind, nRetc);

	// ����H�ُ�
	if (0 == nRetc) {
		SendState_App(0);
		gcls_Status.SetMainState(StatusManager::STATE_RUN);			 // ������

	}
	else {
		SendState_App(APP_PARAM_EXEC);	// ���s���s
	}
}

//------------------------------------------
// �r���J�n�A���T�[���M����
// int			 nStartFrameNo	�r���Q���t���[��
// EM_DIV_HANTEI emKind			�S������
// int			 nAns			���� (0:���� 1:�y�̏� -1:�d�̏�)
//------------------------------------------
void MainInctance::ResumeResponse(int nStartFrameNo, EM_DIV_HANTEI emKind, int nAns)
{
	int			mailRetc;
	COMMON_QUE	sque;

	sque.nEventNo	= FACT_TO_DEFECT_23;			// �r���J�n�A���T�[
	sque.nLineNo	= getlineid();
	sque.fl.data[0] = getpcid() - ((NUM_HANTEI * NUM_MEN) + NUM_ALL_HTBACK);
	sque.fl.data[1] = getpcid();					// PcId
	sque.fl.data[2] = nAns;							// ����
	sque.fl.data[3] = nStartFrameNo;				// �r���Q���t���[��No �i0�I���W���j
	sque.fl.data[4] = emKind;						// �S������

	mailRetc = send_mail(TO_DEFECT, TOKATU, &sque);
	if (0 != mailRetc) syslog(SYSNO_MAIL_SNED_ERR, "[�r���J�n�A���T�[ err_code=%d]", mailRetc);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��������


//------------------------------------------
// �R�}���h�p�C�v�N���X���G���[
// CommandSender::EM_PIPE_ERR emErrCode �G���[�R�[�h
// int nDetail							�ڍ׃G���[�R�[�h
// BYTE const* pData					��M��� (���K�v�Ȃ��̂̂�)
// �߂�l ���A��� (0:���� 1:�y�̏� -1:�d�̏�)
//------------------------------------------
int MainInctance::CheckCommandPipe_Err(CommandSender::EM_PIPE_ERR emErrCode, int nDetail, BYTE const* pData)
{
	CString cbuf;
	int	 ansRetc = 0;

	//// ��������
	if( CommandSender::CPIPE_ERR_COMMAND == emErrCode ) {			// �񓯊��G���[

		//=======================================
		// �񓯊��C�x���g
		if( NCL_SUCCESS == nDetail ) { return 0; }
		else if( 0 < nDetail) {
			LOG(em_INF), "[%s] <�R�}���h�p�C�v�C�x���g> [%d]", my_sThreadName, nDetail);
			return 0;
		}
		

		//=======================================
		// ���O��`

		//------------------------
		// �񓯊��G���[
		if	   (NCL_ERR_RECV_EDGE_INFO == nDetail)				{ ansRetc =-1; cbuf = "NCL_ERR_RECV_EDGE_INFO:�G�b�W�ʒu�ʐM�G���["; }
		else if(NCL_ERR_SEND_EDGE_INFO == nDetail)				{ ansRetc =-1; cbuf = "NCL_ERR_SEND_EDGE_INFO:�G�b�W�ʒu�ʐM�G���["; }
		else if(NCL_ERR_RECV_EDGE_INFO_TIMEOUT == nDetail)		{ ansRetc =-1; cbuf = "NCL_ERR_RECV_EDGE_INFO_TIMEOUT:�G�b�W�ʒu�ʐM�^�C���A�E�g"; }
		else if(NCL_ERR_SEND_EDGE_INFO_TIMEOUT == nDetail)		{ ansRetc =-1; cbuf = "NCL_ERR_SEND_EDGE_INFO_TIMEOUT:�G�b�W�ʒu�ʐM�^�C���A�E�g"; }
		else if(NCL_ERR_ASYNC_PARAM_FAILED == nDetail)			{ ansRetc = 1; cbuf = "NCL_ERR_ASYNC_PARAM_FAILED:�t���[���ԍ��w���PARAM�R�}���h�̎��s"; }
		else if(NCL_ERR_ASYNC_SET_POSITION_FAILED == nDetail)	{ ansRetc = 1; cbuf = "NCL_ERR_ASYNC_SET_POSITION_FAILED:�t���[���ԍ��w���SET_POSITION�R�}���h�̎��s"; }


		//------------------------
		// �񓯊��C�x���g
		else if(NCL_ERR_FRAME_SKIPPED	== nDetail)				{ ansRetc = 0; cbuf = "NCL_ERR_FRAME_SKIPPED:�t���[���X�L�b�v����"; }
		else if(NCL_ERR_ASYNC_PARAM_SUCCESS	== nDetail)			{ ansRetc = 0; cbuf = "NCL_ERR_ASYNC_PARAM_SUCCESS:�t���[���ԍ��w���PARAM�R�}���h�̐���"; }
		else if(NCL_ERR_ASYNC_SET_POSITION_SUCCESS	== nDetail)	{ ansRetc = 0; cbuf = "NCL_ERR_ASYNC_SET_POSITION_SUCCESS:�t���[���ԍ��w���SET_POSITION�R�}���h�̐���"; }

		//------------------------
		// �񓯊� �s��
		else												{ ansRetc = 1; cbuf.Format("�s���ȃG���[:%d", nDetail);	}


		//=======================================
		// ��ޖ��̏���

		//------------------------
		// �t���[���X�L�b�v����
		if( NCL_ERR_FRAME_SKIPPED == nDetail) {
			LOG(em_MSG), "[%s] <�R�}���h�p�C�v> [%s]", my_sThreadName, cbuf);

			syslog(116, "[�񓯊��ʒm:�ڍ�=%s]", cbuf);

		}
		// ���B���͂ǂ����邩�H


		//------------------------
		// �R�}���h�p�C�v �񓯊��C�x���g
		else if( -2000 >= nDetail ) {	

		}

		//------------------------
		// �R�}���h�p�C�v�G���[
		else if( 0 > nDetail ) {						// �G���[
			LOG(em_WAR), "[%s] <�R�}���h�p�C�v�G���[> �񓯊��G���[ [%s]", my_sThreadName, cbuf);
			syslog(104, "[�񓯊��G���[:�ڍ�=%s]", cbuf);
			// �Ȃ񂩃G���[�����o = �摜�����^�X�N���̂͂��������Ȃ� = �y�̏�
			if(0 == ansRetc) ansRetc = 1;
		}

	//// ���̑��G���[
	} else {														// ���̑��̃G���[
		switch (emErrCode) {
		case CommandSender::CPIPE_OPEN_ERR:
			cbuf = "�I�[�v���Ŏ��s";
			break;
		case CommandSender::CPIPE_READ_ERR:
			cbuf = "�Ǎ��݂Ŏ��s";
			break;
		case CommandSender::CPIPE_DISCONNECT_ERR:
			cbuf = "�p�C�v������ɐؒf���ꂽ";
			break;
		}
		LOG(em_ERR), "[%s] <�R�}���h�p�C�v�G���[> %s [�ڍ�=%d]", my_sThreadName, cbuf, nDetail);
		syslog(104, "[%s:�ڍ�=%d]", cbuf, nDetail);

		// �p�C�v���������� = �摜�����^�X�N���̂��������� = �d�̏�
		ansRetc = -1;

	}

	//// �ŏI�`�F�b�N
	if (-1 == ansRetc) {
		StatusManager::MAIN_STATE st = gcls_Status.GetMainState();
		
		if( CommandSender::CPIPE_DISCONNECT_ERR == emErrCode) {
			// �p�C�v�̐ؒf�͏d�̏�̂܂�

		} else if(StatusManager::STATE_WAIT != st && StatusManager::STATE_RUN != st) {
			LOG(em_ERR), "[%s] �ғ����ł͖����̂ŁA�y�̏�ɕύX", my_sThreadName, gcls_Status.GetMainStateName());
			ansRetc = 1;
		}
	}
	return ansRetc;
}

//------------------------------------------
// APP�ُ퍀�ڃp�����[�^ �؂�ւ�
// DWORD addr �r�b�g�w��ʒu (�r�b�g���쎞�̂ݎg�p)
// bool onoff �ύX�l (true:���� false:�ُ�)
//------------------------------------------
void MainInctance::SendState_App(DWORD state)
{
	if(0 == m_nCamPair) return;		// �o�b�N�A�b�v�@�Ȃ疳��
	m_nAppParam = state;

	// �@���ԕύX
	if(		 DIV_HANTEI_HT == m_emKindHt ) KizuFunction::SetStatus("HT_APP_%d_ID", m_nCamPair, (int)state);
	else if( DIV_HANTEI_PR == m_emKindHt ) KizuFunction::SetStatus("PR_APP_%d_ID", m_nCamPair, (int)state);
	else if( DIV_HANTEI_CY == m_emKindHt ) KizuFunction::SetStatus("CY_APP_%d_ID", m_nCamPair, (int)state); 

	
	if(0==state) { LOG(em_MSG), "[%s] APP�ُ퍀�� ����", my_sThreadName); }
	else		 { LOG(em_ERR), "[%s] APP�ُ퍀�� �ُ� [%d]", my_sThreadName, m_nAppParam); }
}
void MainInctance::SendState_App(DWORD addr, bool onoff)
{
	if(0 == m_nCamPair) return;		// �o�b�N�A�b�v�@�Ȃ疳��
	bool	bSetting = false;		// �l�ύX�L��
	DWORD	nChange = m_nAppParam;
	
	// �r�b�g����
	if( ! onoff )	nChange |= addr;		// �r�b�g���Ă� = �ُ�
	else			nChange &= ~addr;		// �r�b�g���Ƃ� = ����

	if(nChange == m_nAppParam) return;		// �l�̕ύX����

	// �l�̕ύX
	SendState_App(nChange);
}


//------------------------------------------
// ���s�s�\����
// bool reboot �摜�����\�t�g �ċN�� (true:�ċN��)
//------------------------------------------
void MainInctance::SendState_PcCanOk(bool reboot)
{
	if(StatusManager::STATE_CANNOT != gcls_Status.GetMainState() ) {
		KizuFunction::SetStatus("HT_CANNOT_%d_ID", getpcid(), 0, false);		// �H���Ⴂ�h�~�̂���	
		return;
	}

	//// �摜�����^�X�N���ċN��
	if(reboot) {
		// �摜�����\�t�g ��~
		DisposeProcess_DetectProc();
			
		// �O�`�Z�o�v���Z�X
		ExecuteProcess_RectProc();


		// �摜�����\�t�g �N��
		if( ! ExecuteProcess_DetectProc() ) {
			// �ُ�
			SendState_PcCanNot();
			LOG(em_ERR), "[%s] ���s�s�\�������s�I�I", my_sThreadName );
			return;
		}
	}
	//// ����
	KizuFunction::SetStatus("HT_CANNOT_%d_ID", getpcid(), 0);

	//// �������
	gcls_Status.SetMainState(StatusManager::STATE_INIT);
	LOG(em_MSG), "[%s] ���s�s�\ ����", my_sThreadName);
	syslog(114, "");

	//// �����ɏ�Ԃ�ʒm
	Send_ToDefect_State();
}

//------------------------------------------
// ������PC �� ���s�s�\
//------------------------------------------
void MainInctance::SendState_PcCanNot()
{
#ifdef SOT_TEST
	return;
#endif

	// ���s�s�\
	gcls_Status.SetMainState(StatusManager::STATE_CANNOT);

	// �@���ԕύX
	KizuFunction::SetStatus("HT_CANNOT_%d_ID", getpcid(), 1);
	
	LOG(em_ERR), "[%s] ���s�s�\�I�I", my_sThreadName);
	syslog(115, "");

	//---------------------------------------
	// �S������ �ؒf
#ifdef FRAME_RAW_SEND
	for (int ii=0; ii<FRAME_HT_SEND_PORT; ii++) {
		gcls_pSockRpi[ii]->SetRetryMode(false);		// ���g���C�͂������Ȃ�
		gcls_pSockRpi[ii]->SetDisConnect();
	}
#endif
}

//------------------------------------------
// �摜���ُ͈�
// int nCamId  �Ώ�  (0:�S�� 1�`:�Y���J����) ���Ԃ񂻂̃J�������S������J�����Z�b�g,�J�����A���O���̏���1�`�̘A�Ԃ̂͂�
// bool state  ���  (true:���� false:�ُ�)
//------------------------------------------
void MainInctance::SendState_Grab(int nCamId, bool state)
{
	// ���W�b�N�p���������͉摜���ُ͈�𗧂ĂȂ�
	if (DIV_HANTEI_PR == m_emKindHt) return;

	CString wk;
	// �摜����
	if(0 == nCamId) {
		int nCamSetNum = gcls_Status.GetCamPairItemNum(m_nCamPair, m_emKindHt);
		for(int ii=0; ii<nCamSetNum; ii++) {
			int nCh = 0;

			// %d_%d  �J�����y�A_���삳��̃J�����ԍ�(�J�����Z�b�g�A�J�����p�x��)
			wk.Format("HT_GRAB_%d_%d_ID", gcls_Status.GetCamPairItemCamset(m_nCamPair, ii, m_emKindHt), nCh+1 );
			// �����@�\�Ή� ------>>>
			if(DIV_HANTEI_CY == m_emKindHt) 	wk.Format("CY_GRAB_%d_%d_ID", gcls_Status.GetCamPairItemCamset(m_nCamPair, ii, m_emKindHt), nCh+1 );
			// <<<-------------------
			
			KizuFunction::SetStatus(wk, state, !state);
			nCh++;

		}
	} else {
		wk.Format("HT_GRAB_%d_%d_ID", m_nCamPair, nCamId );
		// �����@�\�Ή� ------>>>
		if(DIV_HANTEI_CY == m_emKindHt) 	wk.Format("CY_GRAB_%d_%d_ID", m_nCamPair, nCamId );
		// <<<-------------------

		KizuFunction::SetStatus(wk, state, !state);
	}
}


//------------------------------------------
// ��Ԃ�ʒm
//------------------------------------------
void MainInctance::Send_ToDefect_State()
{
	COMMON_QUE	sque;
	sque.nEventNo = FACT_TO_DEFECT_09;				// �����~�A���T�[
	sque.nLineNo = getlineid();
	sque.fl.data[0] = m_nCamPair;					// �J�����y�A = PcKind
	sque.fl.data[1] = getpcid();					// PcId
	sque.fl.data[2] = (StatusManager::STATE_CANNOT == gcls_Status.GetMainState() ? -1 : 0);	// ��� (0:���� 1:�y�̏� -1:�d�̏�)
	sque.fl.data[3] = m_emKindHt;

	int mailRetc = send_mail(TO_DEFECT, TOKATU, &sque);
	if( 0 != mailRetc ) syslog(SYSNO_MAIL_SNED_ERR, "[��Ԗ₢���킹�A���T�[ err_code=%d]", mailRetc);
}

//------------------------------------------
// �t���[���摜�擾���s��ʒm
// DWORD camset 
// DWORD FrameNo 
//------------------------------------------
//void MainInctance::Send_ToDefect_FrameReqFail(DWORD camset, DWORD FrameNo)
//{
//	COMMON_QUE	sque;
//	sque.nEventNo = FACT_TO_DEFECT_10;				// �t���[���摜�擾���s�ʒm
//	sque.nLineNo = getlineid();
//	sque.fl.data[0] = m_nCamPair;					// �J�����y�A = PcKind
//	sque.fl.data[1] = getpcid();					// PcId
//	sque.fl.data[2] = camset;						// �v���J�����Z�b�g
//	sque.fl.data[3] = FrameNo;						// �v���t���[��No
//	int mailRetc = send_mail(TO_DEFECT, TOKATU, &sque);
//	if( 0 != mailRetc ) syslog(SYSNO_MAIL_SNED_ERR, "[�t���[���摜�擾���s�ʒm err_code=%d]", mailRetc);
//}

//------------------------------------------
// ��ʏ��(������)�擾&���M���C������
// const COMMON_QUE* pQue : �L���[���
// L[0]�@�x�����s�L�[�i�O�F�����@�P�`�F�L�[�j
// L[1]�@�����M�����ʏ��̃T�C�Y[Byte]
// L[2]�@���� (1�I���W��)
// L[3]�@�ő啪����
// L[4]�@BYTE[220]�@��ʏ��
//------------------------------------------
void MainInctance::Send_CoilInfDiv(const COMMON_QUE* pQue)
{
	////============================
	// ���擾
	//int nDly = pQue->fl.data[0];				// �x�����s�L�[(0:�������s 1�`:�L�[)
	int nSize = pQue->fl.data[1];				// ���񑗐M�����ʏ��̃T�C�Y(SIZE_SEND_DIV�𒴂��鎖�͂Ȃ��͂�)
	int nNum = pQue->fl.data[2];				// �f�[�^�̈ʒu(1�I���W��)
	int nMaxCnt = pQue->fl.data[3];				// ������
	int scode[NUM_MEN];
	for( int ii = 0; ii < NUM_MEN; ii++ ) { scode[ii] = pQue->fl.data[4 + ii]; }
	void *pData = (void*)&pQue->fl.data[4+NUM_MEN];
	int nOffSet = DIVCOIL_ONE_SIZE_SEND*(nNum-1);

	// �����������H
	if (1 != nNum && 0 == m_typMergedInfo.nDivCnt) {
		LOG(em_WAR), "[%s] ��ʏ���M ��������! %d��", my_sThreadName, nNum);
		return;
	}

	////============================================
	// �����o�ϐ��ɃZ�b�g

	//// ����1�ڂ̏�񂩂�擾����f�[�^
	if (1 == nNum) {
		memset(&m_typMergedInfo, 0x00, sizeof(m_typMergedInfo));	// ������

		m_typMergedInfo.nDivCnt = nMaxCnt;	// ������				
	}

	////�S�Ă̕�����񂩂�擾����f�[�^
	m_typMergedInfo.nSize += nSize;								// ���v�T�C�Y
	memcpy(&m_typMergedInfo.bPcInfo[nOffSet], pData, nSize);	// ��ʏ��
	m_typMergedInfo.bGetFlg[nNum - 1] = true;					// �f�[�^�ʒu�̃t���O��True��



	////============================================
	// �����f�[�^��M�̓r���Ȃ�΂����ŏI��
	bool bSendOk = true;
	for (int ii = 0; ii < m_typMergedInfo.nDivCnt; ii++) {
		if (!m_typMergedInfo.bGetFlg[ii]) bSendOk = false;
	}
	if (!bSendOk) return;

	LOG(em_MSG), "[%s] ��ʏ���M [%d����]", my_sThreadName, nMaxCnt);


	////==============================================
	// �S�Ă̕����f�[�^����M������EPC�G���W���ɑ��M
	// ��ʐݒ���R�}���h
	int retc = SendCommand_CoilInf(m_typMergedInfo.nSize, m_typMergedInfo.bPcInfo);
	// �A���T�[�ɂ�菈������
	CommandAnswerProc(retc, APP_PARAM_COILINF);

	// �����o�ϐ����N���A
	memset(&m_typMergedInfo, 0x00, sizeof(m_typMergedInfo));

	// ���W�b�N�t�@�C����ԕ\��
	gcls_Logic.Check( scode [ m_nMen ]);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �R�}���h����

//------------------------------------------
// �摜�����^�X�N �������R�}���h (�������蓖��)
// ���A��� (0:���� 1:�y�̏� -1:�d�̏�)
//------------------------------------------
int MainInctance::SendCommand_Init()
{
	NCL_CMD_INIT		cmd;
	memset(&cmd, 0x00, sizeof(cmd));

	//// �������R�}���h ����
	cmd.cmd	= NCL_INIT;
	cmd.camnum = gcls_Status.GetCamPairItemNum(m_nCamPair, m_emKindHt);
	for(DWORD ii=0; ii<cmd.camnum; ii++) cmd.camid[ii] = gcls_Status.GetCamPairItemCamset(m_nCamPair, ii, m_emKindHt); 
	cmd.initial_scode = m_nNowScode[0];


	// ODBC�ڑ������񐶐�
	char cOdbc[64];
	char cUser[64];
	char cPass[64];
	GetPrivateProfileString(INI_DB, "ODBCNAME", "", cOdbc, sizeof(cOdbc), TASKINI_NAME);
	GetPrivateProfileString(INI_DB, "USERID",   "", cUser, sizeof(cUser), TASKINI_NAME);
	GetPrivateProfileString(INI_DB, "PASSWORD", "", cPass, sizeof(cPass), TASKINI_NAME);
	sprintf(cmd.dbconnstr, EPC_ODBC_FORMAT, cOdbc, cUser, cPass);

	// ���W�b�N�p�������̏ꍇ
	if (DIV_HANTEI_PR == m_emKindHt)
	{
		// �J�����Z�b�g�͕����Ƃ���
		for (DWORD ii = 0; ii < cmd.camnum; ii++)
		{
			if (0 != cmd.camid[ii]) cmd.camid[ii] *= -1;
		}

		// ���W�b�N�p�������p�f�[�^�x�[�X�ɕύX
		GetPrivateProfileString(INI_DB_PARA, "ODBCNAME", "", cOdbc, sizeof(cOdbc), TASKINI_NAME);
		GetPrivateProfileString(INI_DB_PARA, "USERID", "", cUser, sizeof(cUser), TASKINI_NAME);
		GetPrivateProfileString(INI_DB_PARA, "PASSWORD", "", cPass, sizeof(cPass), TASKINI_NAME);
		sprintf(cmd.dbconnstr, EPC_ODBC_FORMAT, cOdbc, cUser, cPass);
	}

	//// �R�}���h���M
	LOG(em_MSG), "[%s] �R�}���h���M [%d �������R�}���h:scode=%d camsetnum=%d camset=%d,%d,%d,%d %d,%d,%d,%d dbconnstr=\"%s\"]", my_sThreadName, 
		cmd.cmd, cmd.initial_scode, cmd.camnum, cmd.camid[0], cmd.camid[1], cmd.camid[2], cmd.camid[3], cmd.camid[4], cmd.camid[5], cmd.camid[6], cmd.camid[7], cmd.dbconnstr );


	NCL_ANSWER retc;
	int nRetc = gcls_pCom->SendCommand( (BYTE*)&cmd, sizeof(cmd), &retc, sizeof(retc), COMMAND_INIT_WAIT);
	if( 0 > nRetc ) {
		LOG(em_ERR), "[%s] �R�}���h���M���s [�������R�}���h:%d]", my_sThreadName, nRetc );
		syslog( 107, "[�������R�}���h:%d]", nRetc );
		return -1;			// �������R�}���h���͑S���d�̏�

	} else if (0 != nRetc) {
		LOG(em_ERR), "[%s] �R�}���h�A���T�[�ُ� [�������R�}���h:%d:%d]", my_sThreadName, retc.retcode, retc.cmd );
		syslog( 106, "[�������R�}���h:%d:%d]", retc.retcode, retc.cmd );
		return -1;

	} else {
		return 0;
	}
}


//------------------------------------------
// �摜�����^�X�N �����J���R�}���h
// ���A��� (0:���� 1:�y�̏� -1:�d�̏�)
//------------------------------------------
int MainInctance::SendCommand_Dispose()
{
	NCL_CMD	cmd;
	memset(&cmd, 0x00, sizeof(cmd));

	// �����J���R�}���h ����
	cmd.cmd	= NCL_DISPOSE;
	LOG(em_MSG), "[%s] �R�}���h���M [%d �����J���R�}���h]", my_sThreadName, cmd.cmd );

	// �R�}���h���M
	NCL_ANSWER retc;
	int nRetc = gcls_pCom->SendCommand( (BYTE*)&cmd, sizeof(cmd), &retc, sizeof(retc), COMMAND_DISPOSE_WAIT);
	if( 0 > nRetc ) {
		LOG(em_ERR), "[%s] �R�}���h���M���s [�����J���R�}���h:%d]", my_sThreadName, nRetc );
		syslog( 107, "[�����J���R�}���h:%d]", nRetc );
		return -1;			// �����J���R�R�}���h���͑S���d�̏�

	} else if (0 != nRetc) {
		LOG(em_ERR), "[%s] �R�}���h�A���T�[�ُ� [�����J���R�}���h:%d:%d]", my_sThreadName, retc.retcode, retc.cmd );
		syslog( 106, "[�����J���R�}���h:%d:%d]", retc.retcode, retc.cmd );
		return -1;			// �����J���R�R�}���h���͑S���d�̏�

	} else {
		return 0;
	}
}


//------------------------------------------
// �摜�����^�X�N �I���R�}���h
// ���A��� (0:���� 1:�y�̏� -1:�d�̏�)
//------------------------------------------
int MainInctance::SendCommand_Exit()
{
	NCL_CMD	cmd;
	memset(&cmd, 0x00, sizeof(cmd));

	// �I���R�}���h ����
	cmd.cmd	= NCL_EXIT;
	LOG(em_MSG), "[%s] �R�}���h���M [%d �I���R�}���h]", my_sThreadName, cmd.cmd );

	// �R�}���h���M
	NCL_ANSWER retc;
	int nRetc = gcls_pCom->SendCommand( (BYTE*)&cmd, sizeof(cmd), &retc );
	if( 0 != nRetc ) {
		LOG(em_ERR), "[%s] �R�}���h���M���s [�I���R�}���h:%d]", my_sThreadName, nRetc );
		syslog( 107, "[�I���R�}���h:%d]", nRetc );
	}
	return 0;
}

//------------------------------------------
// �摜�����^�X�N �����J�n�R�}���h
// ���A��� (0:���� 1:�y�̏� -1:�d�̏�)
//------------------------------------------
int MainInctance::SendCommand_Start()
{
	NCL_CMD	cmd;
	memset(&cmd, 0x00, sizeof(cmd));

	// �J�n�R�}���h ����
	cmd.cmd	= NCL_START;
	LOG(em_MSG), "[%s] �R�}���h���M [%d �����J�n�R�}���h]", my_sThreadName, cmd.cmd );

	// �R�}���h���M
	NCL_ANSWER retc;
	int nRetc = gcls_pCom->SendCommand( (BYTE*)&cmd, sizeof(cmd), &retc, sizeof(retc), COMMAND_INIT_WAIT);
	if( 0 > nRetc ) {
		LOG(em_ERR), "[%s] �R�}���h���M���s [�����J�n�R�}���h:%d]", my_sThreadName, nRetc );
		syslog( 107, "[�����J�n�R�}���h:%d]", nRetc );
		return -1;			// �����J�n�R�}���h���͑S���d�̏�

	} else if (0 != nRetc) {
		LOG(em_ERR), "[%s] �R�}���h�A���T�[�ُ� [�����J�n�R�}���h:%d:%d]", my_sThreadName, retc.retcode, retc.cmd );
		syslog( 106, "[�����J�n�R�}���h:%d:%d]", retc.retcode, retc.cmd );
		return -1;			// �����J�n�R�}���h���͑S���d�̏�

	} else {
		return 0;
	}
}
//------------------------------------------
// �摜�����^�X�N ������~�R�}���h
// ���A��� (0:���� 1:�y�̏� -1:�d�̏�)
//------------------------------------------
int MainInctance::SendCommand_Stop()
{
	NCL_CMD	cmd;
	memset(&cmd, 0x00, sizeof(cmd));

	// ��~�R�}���h ����
	cmd.cmd	= NCL_STOP;
	LOG(em_MSG), "[%s] �R�}���h���M [%d ������~�R�}���h]", my_sThreadName, cmd.cmd );

	// �R�}���h���M
	NCL_ANSWER retc;
	int nRetc = gcls_pCom->SendCommand( (BYTE*)&cmd, sizeof(cmd), &retc );
	if( 0 > nRetc ) {
		LOG(em_ERR), "[%s] �R�}���h���M���s [������~�R�}���h:%d]", my_sThreadName, nRetc );
		syslog( 107, "[������~�R�}���h:%d]", nRetc );
		return 1;

	} else if (0 != nRetc) {
		LOG(em_ERR), "[%s] �R�}���h�A���T�[�ُ� [������~�R�}���h:%d:%d]", my_sThreadName, retc.retcode, retc.cmd );
		syslog( 106, "[������~�R�}���h:%d:%d]", retc.retcode, retc.cmd );
		return 1;

	} else {
		return 0;
	}
}

//------------------------------------------
// �摜�����^�X�N �p���[���[�^�ύX�R�}���h
// DWORD framenum ���s�t���[���ԍ� (0�ő������s)
// DWORD scode �����\�ʏ��No
// ���A��� (0:���� 1:�y�̏� -1:�d�̏�)
//------------------------------------------
int MainInctance::SendCommand_Param(DWORD framenum, DWORD scode)
{
	NCL_CMD_PARAM	cmd;
	memset(&cmd, 0x00, sizeof(cmd));

	// �p�����[�^�R�}���h ����
	cmd.cmd	= NCL_PARAM;
	cmd.scode	= scode;
	cmd.framenum = framenum;
	
	LOG(em_MSG), "[%s] �R�}���h���M [%d �p�����[�^�ύX�R�}���h:framenum=%d scode=%d]", my_sThreadName, cmd.cmd, cmd.framenum, cmd.scode );

	// �R�}���h���M
	NCL_ANSWER retc;
	int nRetc = gcls_pCom->SendCommand( (BYTE*)&cmd, sizeof(cmd), &retc, sizeof(retc), COMMAND_PARAM_WAIT);
	if( 0 > nRetc ) {
		LOG(em_ERR), "[%s] �R�}���h���M���s [�p�����[�^�ύX�R�}���h:%d]", my_sThreadName, nRetc );
		syslog( 107, "[�p�����[�^�ύX�R�}���h:%d]", nRetc );
		return -1;

	} else if (0 != nRetc) {
		LOG(em_ERR), "[%s] �R�}���h�A���T�[�ُ� [�p�����[�^�ύX�R�}���h:%d:%d]", my_sThreadName, retc.retcode, retc.cmd );
		syslog( 106, "[�p�����[�^�ύX�R�}���h:%d:%d]", retc.retcode, retc.cmd );
		return 1;

	} else {
		return 0;
	}
}

//------------------------------------------
// �摜�����^�X�N ��ʏ��ݒ�R�}���h
// DWORD size �f�[�^���̃T�C�Y
// BYTE const* pData	���M���
// ���A��� (0:���� 1:�y�̏� -1:�d�̏�)
//------------------------------------------
int MainInctance::SendCommand_CoilInf(DWORD size, BYTE const* pData)
{
	//BYTE	wk[256];
	BYTE	wk[sizeof(NCL_CMD_COILINFO) + DIVCOIL_MAX_SEND_SIZE];
	NCL_CMD_COILINFO& cmd = (NCL_CMD_COILINFO&)wk;	

	// �p�����[�^�R�}���h ����
	cmd.cmd	= NCL_COILINFO;
	cmd.info_size = size;
	memcpy( cmd.coil_info, pData, size);

	LOG(em_MSG), "[%s] �R�}���h���M [%d ��ʏ��ݒ�R�}���h:size=%d]", my_sThreadName, cmd.cmd, cmd.info_size);

	// �R�}���h���M
	NCL_ANSWER retc;
	int nRetc = gcls_pCom->SendCommand( wk, (size+sizeof(NCL_CMD_COILINFO)-sizeof(BYTE)), &retc );
	if( 0 > nRetc ) {
		LOG(em_ERR), "[%s] �R�}���h���M���s [��ʏ��ݒ�R�}���h:%d]", my_sThreadName, nRetc );
		syslog( 107, "[��ʏ��ݒ�R�}���h:%d]", nRetc );
		return -1;

	} else if (0 != nRetc) {
		LOG(em_ERR), "[%s] �R�}���h�A���T�[�ُ� [��ʏ��ݒ�R�}���h:%d:%d]", my_sThreadName, retc.retcode, retc.cmd );
		syslog( 106, "[��ʏ��ݒ�R�}���h:%d:%d]", retc.retcode, retc.cmd );
		return 1;

	} else {
		return 0;
	}
}


//------------------------------------------
// DB���烁�����ւ̃p�����[�^�ǂݍ��ݐݒ�R�}���h
// DWORD scode �����\�ʏ��No (0:�Sscode�Ǎ�)
// ���A��� (0:���� 1:�y�̏� -1:�d�̏�)
//------------------------------------------
int MainInctance::SendCommand_PreLoad_Param(DWORD scode)
{
	NCL_CMD_PRELOAD_PARAM	cmd;
	memset(&cmd, 0x00, sizeof(cmd));

	// �p�����[�^�R�}���h ����
	cmd.cmd		= NCL_PRELOAD_PARAM;
	cmd.scode	= scode;

	LOG(em_MSG), "[%s] �R�}���h���M [%d �p���[���[�^���O�Ǎ��R�}���h:scode=%d]", my_sThreadName, cmd.cmd, cmd.scode );


	// �R�}���h���M
	NCL_ANSWER retc;
	int nRetc = gcls_pCom->SendCommand( (BYTE*)&cmd, sizeof(cmd), &retc );
	if( 0 > nRetc ) {
		LOG(em_ERR), "[%s] �R�}���h���M���s [�p���[���[�^���O�Ǎ��R�}���h:%d]", my_sThreadName, nRetc );
		syslog( 107, "[�p���[���[�^���O�Ǎ��R�}���h:%d]", nRetc );
		return -1;

	} else if (0 != nRetc) {
		LOG(em_ERR), "[%s] �R�}���h�A���T�[�ُ� [�p���[���[�^���O�Ǎ��R�}���h:%d:%d]", my_sThreadName, retc.retcode, retc.cmd );
		syslog( 106, "[�p���[���[�^���O�Ǎ��R�}���h:%d:%d]", retc.retcode, retc.cmd );
		return 1;

	} else {
		return 0;
	}
};

//------------------------------------------
// ���������J�E���^�ݒ�R�}���h
// NCL_CMD_SET_POSITION* pVal �w����
// ���A��� (0:���� 1:�y�̏� -1:�d�̏�)
//------------------------------------------
int MainInctance::SendCommand_SetPosition(NCL_CMD_SET_POSITION* pVal)
{
	NCL_CMD_SET_POSITION	cmd;
	memcpy(&cmd, pVal, sizeof(cmd));

	// �p�����[�^�R�}���h ����
	cmd.cmd		= NCL_SET_POSITION;		// �O�̂��߂ɍăZ�b�g
	LOG(em_MSG), "[%s] �R�}���h���M [%d ���������J�E���^�ݒ� �R�}���h:BIT=%X, f=%d (%.0f,%.0f,%.0f)(%.0f,%.0f)(%.0f,%.0f)]", my_sThreadName,
		cmd.cmd, cmd.update_mask, cmd.framenum, cmd.front_pos, cmd.tail_pos, cmd.shear_cut_pos, 
		cmd.univ_cnt_up_pos[0], cmd.univ_cnt_up_pos[1], cmd.univ_cnt_dn_pos[0], cmd.univ_cnt_dn_pos[1]);


	// �R�}���h���M
	NCL_ANSWER retc;
	int nRetc = gcls_pCom->SendCommand( (BYTE*)&cmd, sizeof(cmd), &retc );
	if( 0 > nRetc ) {
		LOG(em_ERR), "[%s] �R�}���h���M���s [���������J�E���^�ݒ�R�}���h:%d]", my_sThreadName, nRetc );
		syslog( 107, "[���������J�E���^�ݒ�R�}���h:%d]", nRetc );
		return -1;

	} else if (0 != nRetc) {
		LOG(em_ERR), "[%s] �R�}���h�A���T�[�ُ� [���������J�E���^�ݒ�R�}���h:%d:%d]", my_sThreadName, retc.retcode, retc.cmd );
		syslog( 106, "[���������J�E���^�ݒ�R�}���h:%d:%d]", retc.retcode, retc.cmd );
		return 1;

	} else {
		return 0;
	}
};

//------------------------------------------
// �����s�ݒ�̃L�����Z���R�}���h
// ���A��� (0:���� 1:�y�̏� -1:�d�̏�)
//------------------------------------------
int MainInctance::SendCommand_ClearQueue()
{
	NCL_CMD	cmd;
	memset(&cmd, 0x00, sizeof(cmd));

	// �����s��PARAM,SET_POSITION�������R�}���h ����
	cmd.cmd	= NCL_CLEAR_QUEUE;
	LOG(em_MSG), "[%s] �R�}���h���M [%d �����s�ݒ�̃L�����Z���R�}���h]", my_sThreadName, cmd.cmd );

	// �R�}���h���M
	NCL_ANSWER retc;
	int nRetc = gcls_pCom->SendCommand( (BYTE*)&cmd, sizeof(cmd), &retc );
	if( 0 > nRetc ) {
		LOG(em_ERR), "[%s] �R�}���h���M���s [�����s�ݒ�̃L�����Z���R�}���h:%d]", my_sThreadName, nRetc );
		syslog( 107, "[�����s�ݒ�̃L�����Z���R�}���h:%d]", nRetc );
		return 1;

	} else if (0 != nRetc) {
		LOG(em_ERR), "[%s] �R�}���h�A���T�[�ُ� [�����s�ݒ�̃L�����Z���R�}���h:%d:%d]", my_sThreadName, retc.retcode, retc.cmd );
		syslog( 106, "[�����s�ݒ�̃L�����Z���R�}���h:%d:%d]", retc.retcode, retc.cmd );
		return 1;

	} else {
		return 0;
	}
};

//------------------------------------------
// ��Ԗ₢���킹
// NCL_ANSWER_QUERY_STATE* pVal	�擾���
//------------------------------------------
int MainInctance::SendCommand_Query_State(NCL_ANSWER_QUERY_STATE* pVal)
{
	NCL_CMD	cmd;
	memset(&cmd, 0x00, sizeof(cmd));

	// �p�����[�^�R�}���h ����
	cmd.cmd	= NCL_QUERY_STATUS;
	LOG(em_MSG), "[%s] �R�}���h���M [%d ��Ԗ₢���킹]", my_sThreadName, cmd.cmd );

	// �R�}���h���M
	NCL_ANSWER_QUERY_STATE retc;
	int nRetc = gcls_pCom->SendCommand( (BYTE*)&cmd, sizeof(cmd), &retc, sizeof(retc),10*1000 );
	if( 0 > nRetc ) {
		LOG(em_ERR), "[%s] �R�}���h���M���s [��Ԗ₢���킹:%d]", my_sThreadName, nRetc );
		syslog( 107, "[��Ԗ₢���킹:%d]", nRetc );
		return -1;

	} else if (0 != nRetc) {
		LOG(em_ERR), "[%s] �R�}���h�A���T�[�ُ� [��Ԗ₢���킹:%d:%d]", my_sThreadName, retc.retcode, retc.cmd );
		syslog( 106, "[��Ԗ₢���킹:%d:%d]", retc.retcode, retc.cmd );
		return 1;

	} else {
		memcpy(pVal, &retc, sizeof(NCL_ANSWER_QUERY_STATE) );
		return 0;
	}
}


//------------------------------------------
// �摜�����^�X�N �A���T�[������
// int retc ���A���
// DWORD addr APP �ُ퍀�� �p�����[�^
//------------------------------------------
void MainInctance::CommandAnswerProc(int retc, DWORD addr)
{
	if( 0 == retc) {
		SendState_App(addr, true);

	// �y�̏�
	} else if (1 == retc) {
		SendState_App(addr, false);

	// �d�̏�
	} else {
		if (DIV_HANTEI_PR != m_emKindHt)
		{
			DetectStop(1, -1);			// �����ֈُ푗�M
		}
		else
		{
			ResumeStop(1, -1);			// �����ֈُ푗�M
		}
		SendState_PcCanNot();
	}
}

//------------------------------------------
// �摜�����^�X�N �r���Q���R�}���h
// ���A��� (0:���� 1:�y�̏� -1:�d�̏�)
//------------------------------------------
int MainInctance::SendCommand_Resume(int nStartFrameNo)
{
	NCL_CMD_RESUME_START	cmd;
	memset(&cmd, 0x00, sizeof(cmd));

	// �J�n�R�}���h ����
	cmd.cmd = NCL_RESUME_START;
	cmd.start_frame_num = nStartFrameNo;
	LOG(em_MSG), "[%s] �R�}���h���M [%d �r���Q���R�}���h:StartFrameNo=%d]", my_sThreadName, cmd.cmd, cmd.start_frame_num);

	// �R�}���h���M
	NCL_ANSWER retc;
	int nRetc = gcls_pCom->SendCommand((BYTE*)&cmd, sizeof(cmd), &retc, sizeof(retc), COMMAND_INIT_WAIT);
	if (0 > nRetc) {
		LOG(em_ERR), "[%s] �R�}���h���M���s [�r���Q���R�}���h:%d]", my_sThreadName, nRetc );
		syslog(107, "[�����r���Q���R�}���h:%d]", nRetc);
		return -1;			// �����r���Q���R�}���h���͑S���d�̏�

	}
	else if (0 != nRetc) {
		LOG(em_ERR), "[%s] �R�}���h�A���T�[�ُ� [�r���Q���R�}���h:%d:%d]", my_sThreadName, retc.retcode, retc.cmd );
		syslog(106, "[�r���Q���R�}���h:%d:%d]", retc.retcode, retc.cmd);
		return -1;			// �����r���Q���R�}���h���͑S���d�̏�

	}
	else {
		return 0;
	}
}

//------------------------------------------
// �r����~����
//		�� �ʏ펞�́A�X�g�b�v�A�����J�� �����{
//		�� �Z�����́AiPortViewer�I���A�摜�����^�X�N�N��
// int mode �v����� (0:����ȏI���v��  1:�������������ċ����I�ɏI���v�� -1:���M���Ȃ�)
// int ansRetc ���� (0:���� 1:�y�̏� -1:�d�̏�) �� �������ɁA�d�̏Ⴊ���������ꍇ�́A�d�̏�ɂȂ�
//------------------------------------------
void MainInctance::ResumeStop(int mode, int ansRetc)
{
	////////////////////////////////////
	// ������
	////////////////////////////////////
	EM_DIV_UNTEN		unten = gcls_Status.GetUnten();					// �^�]���[�h

	// ���X�e�[�^�X�m�F
	if (StatusManager::STATE_NON == gcls_Status.GetMainState() ||
		StatusManager::STATE_INIT == gcls_Status.GetMainState() ||
		StatusManager::STATE_CANNOT == gcls_Status.GetMainState())
	{
		LOG(em_ERR), "[%s] [%s] ���ɓr����~�v����M", my_sThreadName, gcls_Status.GetMainStateName() );
		return;
	}
	LOG(em_MSG), "[%s] �r����~�v���B(%s) ==============>>>", my_sThreadName, (0 == mode ? "�����~�v��" : "�ُ�ɂ�鋭����~�v��"));

	////////////////////////////////////
	// �J��������
	////////////////////////////////////
	if (DIV_UNTEN_CAMERA == unten)
	{
		// �J����������

		// iPortViewer �I��
		DisposeProcess_iPortViewer();

		////////////////////////////////////
		// �ʏ� ���� �T���v���B�e ���� �V�~�����[�V����
		////////////////////////////////////
	}
	else
	{
		//// �w���V�[�^�C�}�[��~
		CancelWaitableTimer(m_evHelcy);
		gcls_pSend->StopHls();

		//// �J����GCP�`�F�b�N���~
		CancelWaitableTimer(m_evGcpCheck);

		//---------------------------------------
		// �G���W��

		//// �������̏ꍇ�́A��~
		if (StatusManager::STATE_RUN == gcls_Status.GetMainState())
		{
			// ��~�R�}���h
			if (1 == SendCommand_Stop())
			{
				if (-1 != ansRetc) ansRetc = 1;
			}
			gcls_Status.SetMainState(StatusManager::STATE_WAIT);
			Sleep(200);										// �摜�����^�X�N�������������܂ł�����Ƒ҂�
		}

		//// �����J��
		if (-1 == SendCommand_Dispose())
		{
			ansRetc = -1;
			SendState_PcCanNot();
			goto ending;
		}
	}

ending:

	////////////////////////////////////
	// �Ō�̏���
	////////////////////////////////////
	// �����֌��ʂ𑗐M
	if (-1 != mode)
	{
		int			mailRetc;
		COMMON_QUE	sque;
		sque.nEventNo = FACT_TO_DEFECT_24;				// �r����~�A���T�[
		sque.nLineNo = getlineid();
		sque.fl.data[0] = m_nCamPair;					// �J�����y�A=PcKind
		sque.fl.data[1] = getpcid();					// PcId
		sque.fl.data[2] = ansRetc;						// ����
		sque.fl.data[3] = mode;							// �敪 (0:�v���ɑ΂���A���T�[ 1:�ُ픭����)
		sque.fl.data[4] = m_emKindHt;

		mailRetc = send_mail(TO_DEFECT, TOKATU, &sque);
		if (0 != mailRetc) syslog(SYSNO_MAIL_SNED_ERR, "[�r����~�A���T�[ err_code=%d]", mailRetc);
	}
	if (0 == ansRetc) { LOG(em_MSG), "[%s] �r����~�������� <<<========================", my_sThreadName); }
	else { LOG(em_ERR), "[%s] �r����~���������B����=%d <<<========================", my_sThreadName, ansRetc); }

	// �X�e�[�^�X �������
	if (-1 != ansRetc)	gcls_Status.SetMainState(StatusManager::STATE_INIT);
}


//------------------------------------------
// �J����GCP���ʂ̒�����`�F�b�N���J�n
//------------------------------------------
void MainInctance::GcpCheck_StartTimer()
{
	long nInterval = GetPrivateProfileInt(HT_DEFECT, "GCP_CHECK_TIMER", DEFAULT_GCP_CHECK_INTERVAL, TASKINI_NAME);	// �`�F�b�N�Ԋu[sec]

	// ���񂾂��A�����Ɏ��{����
	__int64 ts = (10 * 1000) * -10000;			// 1�~���b��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evGcpCheck, (LARGE_INTEGER *)&ts, (long long)nInterval * 1000LL, NULL, NULL, FALSE);
}


//------------------------------------------
// �J����GCP���ʂ̒�����`�F�b�N
//------------------------------------------
void MainInctance::GcpCheck()
{
	// ���L�������ɒl�̓Z�b�g����Ă���H
	if (!gcls_pData->IsFirstInfoExists())
	{
		LOG(em_ERR), "[%s] ���L�������ɃX�e�[�^�X���Ȃ�", my_sThreadName);
		return;
	}

	FIRST_SHMEM_INFOv1* pSmemH = (FIRST_SHMEM_INFOv1*)gcls_pData->GetMapPointer();	// ���L�������̐擪�A�h���X = �J�������̈ʒu

	// �S������J�����Z�b�g�������`�F�b�N����
	for (int cam = 0; cam < pSmemH->camnum; cam++)
	{
		int camid = pSmemH->camid[cam];
		char* gcp_result = pSmemH->serial_result[cam].Gcp;
		bool  bIsOk = true;

		char cIniKey[16];	// Ini�t�@�C������擾���邽�߂̃L�[
		char cGcpKey[32];	// Ini�t�@�C������擾�����l(GCP����)
		char cGcpVal[32];	// Ini�t�@�C������擾�����l(OK�Ƃ��镶����)
		for (int key = 1; key <= GCP_CHECK_KEY_MAX; key++)
		{
			//----------------------------------------
			// Ini�t�@�C������L�[���擾
			memset(cIniKey, 0x00, sizeof(cIniKey));
			sprintf(cIniKey, "GCP_KEY_%d", key);
			
			memset(cGcpKey, 0x00, sizeof(cGcpKey));
			GetPrivateProfileString(HT_DEFECT, cIniKey, "", cGcpKey, sizeof(cGcpKey), TASKINI_NAME);

			memset(cIniKey, 0x00, sizeof(cIniKey));
			sprintf(cIniKey, "GCP_VAL_%d", key);

			memset(cGcpVal, 0x00, sizeof(cGcpVal));
			GetPrivateProfileString(HT_DEFECT, cIniKey, "", cGcpVal, sizeof(cGcpVal), TASKINI_NAME);

			// �����ꂩ����`�Ȃ�A�����ŏI��
			if (0 == strlen(cGcpKey) || 0 == strlen(cGcpVal)) break;

			// �`�F�b�N�Ώۂ�GCP���ڊm��
			LOG(em_MSG), "[%s] <cam%d> GCP�`�F�b�N����%d�� [%s: %s]", my_sThreadName, camid, key, cGcpKey, cGcpVal);


			// gcp���ʂ���Ώۂ̍s�𔲂��o��
			char cTargetLine[64];
			memset(cTargetLine, 0x00, sizeof(cTargetLine));

			if (!FindLine(gcp_result, cGcpKey, cTargetLine))
			{
				LOG(em_ERR), "[%s] <cam%d> GCP���ʂ�[%s]��������Ȃ�", my_sThreadName, camid, cGcpKey);
				continue;
			}

			LOG(em_MSG), "[%s] <cam%d> GCP���ʂ�[%s]�𔭌�! [%s]", my_sThreadName, camid, cGcpKey, cTargetLine);

			char* p = strstr(cTargetLine, cGcpVal);

			if (NULL != p)
			{
				LOG(em_MSG), "[%s] <cam%d> [%s]�̃`�F�b�N���ʁFOK", my_sThreadName, camid, cGcpKey);
			}
			else
			{
				LOG(em_ERR), "[%s] <cam%d> [%s]�̃`�F�b�N���ʁFNG", my_sThreadName, camid, cGcpKey);
				syslog(140, "[cam%d][%s]", camid, cTargetLine);

				bIsOk = false;
			}
		}

		// �S�ẴL�[���`�F�b�N���I������A�@���Ԃ𔻒�&�V�X���O�o��
		if (bIsOk)
		{
			LOG(em_MSG), "[%s] <cam%d> GCP�`�F�b�NOK", my_sThreadName, camid);

			char cStatusKey[32];
			memset(cStatusKey, 0x00, sizeof(cStatusKey));
			sprintf(cStatusKey, "CAM_GCP_ERR_%d_ID", camid);
			KizuFunction::SetStatus(cStatusKey, true, false);
		}
		else
		{
			LOG(em_ERR), "[%s] <cam%d> GCP�`�F�b�NNG", my_sThreadName, camid);

			char cStatusKey[32];
			memset(cStatusKey, 0x00, sizeof(cStatusKey));
			sprintf(cStatusKey, "CAM_GCP_ERR_%d_ID", camid);
			KizuFunction::SetStatus(cStatusKey, false, false);
		}
	}
}

//------------------------------------------
// GCP���ʂ���w��̍s�𒊏o
// const char*	src				: GCP����			
// const char*	target			: ���o����L�[
// char*		retStr			: target���܂܂��s�̕�����
// const char*	delimiter		: ���s����
//------------------------------------------
bool MainInctance::FindLine(const char* src, const char* target, char* retStr, const char* delimiter)
{
	const char* start = strstr(src, target);

	// �T�����������񂪃q�b�g���Ȃ�
	if (NULL == start)	return false;

	const char* end = strstr(start, delimiter);

	// start�ȍ~�ɉ��s�������Ȃ�
	if (NULL == end) return false;

	int length = end - start;

	memset(retStr, 0x00, length + 1);
	memcpy(retStr, start, length);

	return true;
}