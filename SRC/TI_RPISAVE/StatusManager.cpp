#include "StdAfx.h"
#include "StatusManager.h"

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
StatusManager::StatusManager(void) :
ThreadManager("StatusMgr"),
// �X�e�[�^�X������
m_emMainSt(STATE_NON)
{
	for(int ii=0; ii<MAX_INS; ii++) {
		//m_emSysSt[ii]		= EM_SYS_STATE::SYS_STATE_NON;
		m_bFrameSync[ii]	= false;
		m_nFrameSyncNo[ii]	= 0;
	}
	// ������
	InitializeCriticalSection(&m_csLock);						// �N���e�B�J���Z�N�V�����I�u�W�F�N�g��������
	m_evReFrameSync			= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evReFrameSyncAuto		= CreateEvent(NULL, FALSE, FALSE, NULL);
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
StatusManager::~StatusManager(void)
{
	//// �J��
	DeleteCriticalSection(&m_csLock);							// �N���e�B�J���Z�N�V�����I�u�W�F�N�g���J��
	CloseHandle(m_evReFrameSync);
	CloseHandle(m_evReFrameSyncAuto);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �X���b�h

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int StatusManager::ThreadFirst()
{
	//// �C�x���g�ݒ�
	HANDLE hArray[] = { m_evReFrameSync,
						m_evReFrameSyncAuto
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void StatusManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���		
	enum {	EV_RE_FRAME_SYNC = 0,								// �ē���
			EV_RE_FRAME_SYNC_AUTO,								// �ē��� (�Y�����̂�)
			EV_END	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_RE_FRAME_SYNC:									// �ē���
		//��ReFrameSync();
		break;

	case EV_RE_FRAME_SYNC_AUTO:								// �ē��� (�Y�����̂�)
		// ���s
		//if( ! GetFrameSyncAllSt( mcls_pParam->GetMain().nIns ) ) ReFrameSync();
		break;

//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		break;
	}
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �X�e�[�^�X����

//------------------------------------------
// �w��̃X�e�[�^�X���̎擾
// EM_MAIN_STATE st �X�e�[�^�X
//------------------------------------------
CString StatusManager::GetMainStName(EM_MAIN_STATE st) const
{
	CString wk;
	if		(st == STATE_NON)		{ wk = "��������"; }
	else if (st == STATE_INIT)		{ wk = "�������"; }
	else if (st == STATE_RUN)		{ wk = "������";}
//	else if (st == STATE_CANNOT)	{ wk = "���s�s�\";}
	else							{ wk.Format("�X�e�[�^�X�ُ� <%d>", st); }
	return wk;
}

//------------------------------------------
// �X�e�[�^�X�ύX
// EM_MAIN_STATE st �X�e�[�^�X
//------------------------------------------
bool StatusManager::SetMainSt(EM_MAIN_STATE st)
{
	////// ���C���C���X�^���X���o��
	EM_MAIN_STATE	old;
	old = GetMainSt();
	if( old == st ) {
		LOG(em_ERR), "�X�e�[�^�X�ύXNG [%s] �� [%s]", GetMainStName(old), GetMainStName(st)); 
		return false;
	}

	//// �N���e�B�J���Z�N�V�����˓�
	AutoLock lock(&m_csLock);

	//// �ύX�X�e�[�^�X�ɂ�菈������
	m_emMainSt = st;
	//syslog(102, "[%s] �� [%s]", GetMainStName(old), GetMainStName(st)); 

	//// ��ʏ�ԕύX
	LOG(em_MSG), "============= <<ST�ύX>> [%s] �� [%s] =============", GetMainStName(old), GetMainStName(st)); 
	return true;
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �t���[���Y��

//------------------------------------------
// �t���[���Y����Ԏ擾
// int nInsNum �C���X�^���X��
// �߂�l �S�����펞 true
//------------------------------------------
bool StatusManager::GetFrameSyncAllSt(int nInsNum)
{
	for(int ii=0; ii<nInsNum; ii++) {
		if( ! m_bFrameSync[ii]) return false;	// �Y���L��
	}
	return true;	// �Y������
}

//------------------------------------------
// �t���[���Y����ԕύX
// int nIns Ins�ԍ� (0�I���W��)
// bool bMode true:����  false:�Y��
//------------------------------------------
void StatusManager::SetFrameSyncSt(int nIns, bool bMode)
{
	if( m_bFrameSync[nIns] == bMode) return;

	m_bFrameSync[nIns] = bMode;
	if(bMode) {
		LOG(em_MSG), "============= <<�t���[����������>> [Ins=%d] =============", nIns); 
	} else {
		LOG(em_ERR), "============= <<�t���[���Y������>> [Ins=%d] =============", nIns); 
	}
}
//------------------------------------------
// �t���[���Y����ԃ��Z�b�g
//------------------------------------------
void StatusManager::ResetFrameSyncAllSt()
{
	for(int ii=0; ii<MAX_INS; ii++) {
		m_bFrameSync[ii] = false;
		m_nFrameSyncNo[ii] = 0;
	}
}
void StatusManager::ResetFrameSyncSt(int nInsNum, int nFno)
{
	m_bFrameSync[nInsNum] = false;
	m_nFrameSyncNo[nInsNum] = nFno;	// �ē����t���[��No
}

//------------------------------------------
// �\�t�g�����ċN��
//------------------------------------------
void StatusManager::ResetAp()
{
	LOG(em_ERR), "============= �\�t�g�����ċN�� ============="); 
	// �\�t�g�������Z�b�g����

	COMMON_QUE que;
	que.nLineNo    = getlineid();
	que.nEventNo = FACT_TI_RPISAVE_02;
	send_mail(getmytaskname(), ".", &que);

	que.nEventNo = FACT_TI_RPISAVE_01;
	send_mail(getmytaskname(), ".", &que);
}
