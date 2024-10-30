#include "StdAfx.h"
#include "DMemManager.h"

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
DMemManager::DMemManager(void) :
m_bIsConnect(false),
m_bIsStatus(false),
m_pCom(NULL),
mcls_pLog(NULL)
{
//*/----> ��
	m_hDataMem = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(TO_SEQMEM_TBL), TBL_TO_SEQMEM_NAME);
	mtbl_pDM =(TO_SEQMEM_TBL*) MapViewOfFile(m_hDataMem, FILE_MAP_ALL_ACCESS, 0, 0, 0);
//*///<<---

	//// ���L������
	// �f�[�^���������e�[�u��
	mem_connect(&m_hDataMem, (LPVOID *)&mtbl_pDM, TBL_TO_SEQMEM_NAME);
	if(m_hDataMem == NULL) {
		syslog(SYSNO_MEM_OPEN, "�f�[�^���������e�[�u�� �A�N�Z�X���s");
		_ASSERT(FALSE);
	}

	//// ini�t�@�C���f�[�^�擾
	GetPrivateProfileString(TO_SEQDM, "READADDR", "", m_cAdderssRead, sizeof(m_cAdderssRead), TASKINI_NAME);
	if ( 0 == strlen( m_cAdderssRead )) {
		_ASSERT(FALSE);
	}
	GetPrivateProfileString(TO_SEQDM, "WRITEADDR", "", m_cAdderssWrite, sizeof(m_cAdderssWrite), TASKINI_NAME);
	if ( 0 == strlen( m_cAdderssWrite )) {
		_ASSERT(FALSE);
	}
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
DMemManager::~DMemManager(void)
{
	//// ���L�������J��
	mem_close(&m_hDataMem, (LPVOID *)&mtbl_pDM);				// �f�[�^���������
}

//------------------------------------------
// �X���b�h
//------------------------------------------
UINT WINAPI DMemManager::MainThread(LPVOID param)
{
	////// ���C���C���X�^���X���o��
	DMemManager *p = (DMemManager *)param;				// ���N���X�̃C���X�^���X���擾
	p->SetTid();										// ���X���b�hID�Z�b�g


	////////////////////////////////////
	// ���O����
	////////////////////////////////////
	__int64 ts = 0;	// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	// ���s�����p
	HANDLE evTimerExec = CreateWaitableTimer(NULL, FALSE, NULL);
	SetWaitableTimer(evTimerExec, (LARGE_INTEGER *)&ts, TIME_INTERVAL_EXEC, NULL, NULL, FALSE);


	////////////////////////////////////
	// �ʏ�̃��C���X���b�h����
	////////////////////////////////////

	// WaitForMultipleObjects�C�x���g
	HANDLE	hArray[] = {p->my_evStop,
						evTimerExec,
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// �X���b�h�I��
			EV_TIMER_EXEC,					// �������s ������^�C�}
			EV_END	};


	// ���b�Z�[�W�L���[�ǂݍ���
	pLOG(em_MSG), "[DMemManager] �X���b�h�J�n [0x%X]", p->GetTid());
	while (1) {
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);

		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// �X���b�h�I��
			pLOG(em_MSG), "[DMemManager] �X���b�h�I�� [0x%X]", p->GetTid());
			CancelWaitableTimer(evTimerExec);
			CloseHandle(evTimerExec);
			return 0;


//-----------------------------------------------------------------------------------------------
		case EV_TIMER_EXEC:											// �������s ������^�C�}
			// �I�[�v�����Ă���ꍇ�̂�
			if( ! p->m_bIsConnect ) break; 

			// �ǂݏ������s
			p->Exec_ReadWrite();
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
// �f�[�^�̓Ǎ��Ə����ݏ���
//------------------------------------------
void DMemManager::Exec_ReadWrite()
{
	//// �ǂݍ��ݏ���
	// �V�[�P���T�f�[�^�擾
	if( MxRead() ) {
		// �V�[�P���T�[�f�[�^������ɓǂݍ��߂��狤�L�������ɏ�������
		memcpy( &mtbl_pDM->fb_dm[DM_RECV_REC], m_cDMBuf[DM_RECV_REC], DM_DATA_MAX);
	}


	//// �������ݏ���
	// ���L�������f�[�^�擾
	memcpy( m_cDMBuf[DM_SEND_REC], &mtbl_pDM->fb_dm[DM_SEND_REC], DM_DATA_MAX);
	// �V�[�P���T�f�[�^������
	MxWrite();

	return;
}


//------------------------------------------
// �f�[�^������ �f�[�^�擾
//------------------------------------------
bool DMemManager::MxRead()
{
	if(NULL == m_pCom) return false;

	// �f�[�^�ǂݍ��� (���[�h�P�ʂȂ̂Œ���)
	long lRet = m_pCom->ReadDeviceBlock2((LPCTSTR)m_cAdderssRead, DM_DATA_MAX/sizeof(short), (short*)&m_cDMBuf[DM_RECV_REC]);         // ��������ɃV�[�P���T�f�[�^��ǂݍ���ł���
	if(m_bIsStatus && 0 != lRet) {
		// ���큨�ُ�ƂȂ�ꍇ�̂� �O�ׂ̈ɍă`�F�b�N
		LOG(em_WAR), "[DMemManager] �f�[�^�������Ǎ��G���[?? [lRet = %X]", lRet);
		lRet = m_pCom->ReadDeviceBlock2((LPCTSTR)m_cAdderssRead, DM_DATA_MAX/sizeof(short), (short*)&m_cDMBuf[DM_RECV_REC]);         // ��������ɃV�[�P���T�f�[�^��ǂݍ���ł���
	}

	if( 0 != lRet ) {
		if( m_bIsStatus ) {                                                    // �Ǎ��G���[�̏ꍇ�ɐݒ�
			m_bIsStatus = false;
			syslog(341, "[errcode = %X]", lRet);
			LOG(em_ERR), "[DMemManager] �f�[�^�������Ǎ��G���[ [lRet = %X]", lRet);
			KizuFunction::SetStatus("DATAMEM_SEQ_ID", false);
		}
		return false;

	} else {
		if( !m_bIsStatus ) {                                                   // �O��Ǎ��G���[�̏ꍇ�œǂݍ��߂��ꍇ�͏�Ԃ𕜋A
			m_bIsStatus = true;
			syslog(343, "");
			LOG(em_MSG), "[DMemManager] �f�[�^�������Ǎ�����");
			KizuFunction::SetStatus("DATAMEM_SEQ_ID", true);
		}
		return true;
	}
}

//------------------------------------------
// �f�[�^������ �f�[�^����
//------------------------------------------
bool DMemManager::MxWrite()
{
	if(NULL == m_pCom) return false;

	// �f�[�^�ǂݍ��� (���[�h�P�ʂȂ̂Œ���)
	long lRet = m_pCom->WriteDeviceBlock2((LPCTSTR)m_cAdderssWrite, DM_DATA_MAX/sizeof(short), (short*)&m_cDMBuf[DM_SEND_REC]);    // MX�R���|�[�l���g�ɏ����݂��s���Ă���
	if(m_bIsStatus && 0 != lRet) {
		// ���큨�ُ�ƂȂ�ꍇ�̂� �O�ׂ̈ɍă`�F�b�N
		LOG(em_WAR), "[DMemManager] �f�[�^�����������G���[?? [lRet = %X]", lRet);
		lRet = m_pCom->WriteDeviceBlock2((LPCTSTR)m_cAdderssWrite, DM_DATA_MAX/sizeof(short), (short*)&m_cDMBuf[DM_SEND_REC]);        // ��������ɃV�[�P���T�f�[�^��ǂݍ���ł���
	}	
	
	if( 0 != lRet ) {
		if( m_bIsStatus ) {
			m_bIsStatus = false;
			syslog(342, "[errcode = %X]", lRet);
			LOG(em_ERR), "[DMemManager] �f�[�^�����������G���[ [lRet = %X]", lRet);
			KizuFunction::SetStatus("DATAMEM_SEQ_ID", false);
		}
		return false;

	} else {
		if( !m_bIsStatus ) {
			m_bIsStatus = true;
			syslog(344, "");
			LOG(em_MSG), "[DMemManager] �f�[�^��������������");
			KizuFunction::SetStatus("DATAMEM_SEQ_ID", true);
		}
		return true;
	}	
}
