#include "StdAfx.h"
#include "SmyuEvtManager.h"

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
SmyuEvtManager::SmyuEvtManager(void) :
SmyuBaseManager("SmyuEvtMgr", 0)
{
	//// ���[���X���b�g������
	m_nMailCnt = 0;
	for(int ii=0; ii<MAX_MAIL_WAIT_NUM; ii++) {
		mcls_pMail[ii]   = NULL;
	}

	//// PO����
	memset(m_nDoBuf, 0x00, sizeof(m_nDoBuf));


	//// �n���h������
	m_evTimerExec  = CreateWaitableTimer(NULL, FALSE, NULL);
	for(int ii=0; ii<MAX_MAIL_WAIT_NUM; ii++) {
		m_evRecvMail[ii] = CreateEvent(NULL, FALSE, FALSE, NULL);
	}
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
SmyuEvtManager::~SmyuEvtManager(void)
{
	SmyuStop();

	//// �n���h�����
	CloseHandle(m_evTimerExec);
	for(int ii=0; ii<MAX_MAIL_WAIT_NUM; ii++) {
		CloseHandle(m_evRecvMail[ii]);
	}
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int SmyuEvtManager::ThreadFirst()
{
	//// �C�x���g�ݒ�
	this->AddEventNum(1, &m_evTimerExec);
	this->AddEventNum(MAX_MAIL_WAIT_NUM, m_evRecvMail);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int SmyuEvtManager::ThreadLast()
{
	// �������~
	CancelWaitableTimer(m_evTimerExec);
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void SmyuEvtManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_TIMER_EXEC = 0,				// �������s ������^�C�}
			EV_RECV_MAIL					// ���[���X���b�g��M
	};

	////// �V�O�i����������
	if(nEventNo == EV_TIMER_EXEC) {
		// �C�x���g��������
		SmyuExec_Po();

		// �O��l�Ƃ��ăf�[�^�Z�b�g
		GetPoNowData();

	} else if( nEventNo >= EV_RECV_MAIL && nEventNo < EV_RECV_MAIL+MAX_MAIL_WAIT_NUM) {
		int nIndex = nEventNo - EV_RECV_MAIL;

		// �C�x���g��������
		SmyuExec_Mail(nIndex);

		// �񓯊���M�J�n
		MailRecvStart(nIndex);
	}
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����

//------------------------------------------
// �V�~�����[�^ �X�^�[�g
//------------------------------------------
void SmyuEvtManager::SmyuStart()
{
	int ii, jj;

	//================================================
	// ���[���X���b�g��M�̏������s��
	m_nMailCnt = 0;
	for( ii=0; ii<(int)m_lstStory.size(); ii++ ) {
		if(DIV_KIND_MAIL == m_lstStory[ii]->emRaise ) {
			if(MAX_MAIL_WAIT_NUM <= m_nMailCnt) _ASSERT(FALSE);
			TYP_RAISE_EVT_MAIL* pD = (TYP_RAISE_EVT_MAIL*)m_lstStory[ii]->pRaise;


			// ���ɓ������̂��o�^�ς݁H
			for(jj=0; jj<m_nMailCnt; jj++) {
				if( 0 == strcmp( m_RecvName[jj], pD->cTaskName) ) break;
			}
			if(jj != m_nMailCnt) continue;		// ���ɓo�^�ς�


			// ����
			mcls_pMail[m_nMailCnt] = new KizuLib::MailSlotManager();
			m_RecvName[m_nMailCnt] = pD->cTaskName;
			if( ! mcls_pMail[m_nMailCnt]->Create(m_RecvName[m_nMailCnt]) ) {
				_ASSERT(FALSE);
			}

			// �񓯊���M�J�n
			MailRecvStart(m_nMailCnt);
			m_nMailCnt++;
		}
	}

	//================================================
	// PO�ω�
	// �O��l�Ƃ��āA���݂̒l���擾
	GetPoNowData();

	// �^�C�}�[
	__int64 ts = TIME_EXEC_INTERVAL * -10000; 
	SetWaitableTimer(m_evTimerExec, (LARGE_INTEGER *)&ts, TIME_EXEC_INTERVAL, NULL, NULL, FALSE);
}

//------------------------------------------
// �V�~�����[�^ �X�g�b�v
//------------------------------------------
void SmyuEvtManager::SmyuStop()
{
	//================================================
	// ���[���X���b�g��M
	for(int ii=0; ii<MAX_MAIL_WAIT_NUM; ii++ ) {
		SAFE_DELETE( mcls_pMail[ii]);
		m_RecvName[ii] = "";
	}
	m_nMailCnt = 0;

	//================================================
	// PO�ω�
	CancelWaitableTimer(m_evTimerExec);
}

//------------------------------------------
// ���[���X���b�g ��M�J�n
// int nIndex �z��ʒu 
//------------------------------------------
void SmyuEvtManager::MailRecvStart(int nIndex)
{
	if( NULL == mcls_pMail[nIndex]) return;

	OVERLAPPED ovlRecv;
	memset(&ovlRecv, 0x00, sizeof(OVERLAPPED));

	ovlRecv.hEvent = m_evRecvMail[nIndex];
	mcls_pMail[nIndex]->Recv(&m_RecvBuf[nIndex], sizeof(COMMON_QUE), &ovlRecv);
}

//------------------------------------------
// Po���ݒl�擾
//------------------------------------------
void SmyuEvtManager::GetPoNowData()
{
	for(int ii=0; ii<mcls_pPio->GetMaxBood(); ii++) {
		for(int jj=0; jj<SMYU_PIO_MAX_PORT; jj++) {
			for(int kk=0; kk<8; kk++) {
				m_nDoBuf[ii][jj][kk] = mcls_pPio->GetValPo(ii, jj, kk);
			}
		}
	}
}

//------------------------------------------
// �V�~�����[�^ ���s (���[���X���b�g��M)
// int nIndex ���ڂ̃��[���X���b�g�z�� (0�I���W��)
//------------------------------------------
void SmyuEvtManager::SmyuExec_Mail(int nIndex)
{
	if( NULL == mcls_pMail[nIndex]) return;

	//// �񓯊���M���擾
	OVERLAPPED ovlRecv;
	memset(&ovlRecv, 0x00, sizeof(OVERLAPPED));
	if( ! mcls_pMail[nIndex]->GetResult( &ovlRecv) ) return;


	//// �Ώۂ̔��������������
	for(int ii=0; ii<(int)m_lstStory.size(); ii++ ) {
		if(DIV_KIND_MAIL != m_lstStory[ii]->emRaise ) continue;

		TYP_RAISE_EVT_MAIL* pD = (TYP_RAISE_EVT_MAIL*)m_lstStory[ii]->pRaise;
 		if( 0 == strcmp( m_RecvName[nIndex], pD->cTaskName) &&  m_RecvBuf[nIndex].nEventNo == pD->nEventNo ) {
			
			// �V�i���I ����
			this->RaiseEvents(m_lstStory[ii]->emEvent, m_lstStory[ii]->nEventSub, (BYTE*)m_lstStory[ii]->pEvent);
		}
	}
}

//------------------------------------------
// �V�~�����[�^ ���s (PO�ω�)
//------------------------------------------
void SmyuEvtManager::SmyuExec_Po()
{
	//// �Ώۂ̔��������������
	for(int ii=0; ii<(int)m_lstStory.size(); ii++ ) {
		if(DIV_KIND_PO != m_lstStory[ii]->emRaise ) continue;
	

		TYP_RAISE_EVT_PO* pD = (TYP_RAISE_EVT_PO*)m_lstStory[ii]->pRaise;
 		
		// �O��l�ƕω����������H
		if( m_nDoBuf[pD->nBood][pD->nPort][pD->nBit] != mcls_pPio->GetValPo(pD->nBood, pD->nPort, pD->nBit) ) {
			// 1:���オ�� 0:������
			if( (!! pD->nMode) == mcls_pPio->GetValPo(pD->nBood, pD->nPort, pD->nBit)) {

				// �V�i���I ����
				this->RaiseEvents(m_lstStory[ii]->emEvent, m_lstStory[ii]->nEventSub, (BYTE*)m_lstStory[ii]->pEvent);
			}
		}
	}
}
