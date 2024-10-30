// Tracking.cpp: Tracking �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tracking.h"

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// �R���X�g���N�^
// DWORD dist ��mm��ɒʒm? [mm] (�Œ蒷�Ȃ炱�ꂾ���ł悢�B�ϒ��Ȃ�A�g���b�L���O�J�n�O��ChangeDist�ŋ������Z�b�g���Ȃ�������)
// const double* length �`�F�b�N���鋗���|�C���^ [mm] (�ʏ�́A�J�E���^�[�{�[�h�ŊǗ����Ă��鋗���̃A�h���X��n��)
// const LPHANDLE evEnd �w�苗���o�ߌ� ON����n���h���|�C���^
// const LPHANDLE evCancel �r�����f�����ꍇ�� ON����n���h���|�C���^
//------------------------------------------
Tracking::Tracking(DWORD dist, const double* length, const LPHANDLE evEnd, const LPHANDLE evCancel) :
m_nDist(dist),
m_dLength(length),
m_evTrackingEnd(evEnd),
m_evTrackingCancel(evCancel),
g_pAddr(NULL),
m_bTkRun(false),
m_dChangeLengthMax(0)
{
	// ������
	m_dDistNow			= -1;
	m_evTkCancelStop	= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evTkTimer			= CreateWaitableTimer(NULL,FALSE, NULL);
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
Tracking::~Tracking()
{
	Stop();		// ���s���̏ꍇ ��x�Ƃ߂�

	CloseHandle(m_evTkCancelStop);
	CancelWaitableTimer(m_evTkTimer);
	CloseHandle(m_evTkTimer);
}

//------------------------------------------
// �g���b�L���O���J�n
//------------------------------------------
BOOL Tracking::TrackingStart()
{
	if(m_bTkRun) return FALSE;

	// ������
	m_dDistNow = *m_dLength;
	m_bTkRun = true;
	m_dChangeLengthMax = 0;


	// �O�ׂ̈ɓo�^�ς݂̃V�O�i����S�ăL�����Z��
	if(NULL != m_evTrackingEnd)		ResetEvent(*m_evTrackingEnd);
	if(NULL != m_evTrackingCancel)	ResetEvent(*m_evTrackingCancel);


	// �g���b�L���O�^�C�}�[�J�n
	__int64 ts = 0;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evTkTimer, (LARGE_INTEGER *)&ts, 20, NULL, NULL, FALSE);

	return TRUE;
}

//------------------------------------------
// �g���b�L���O��������~ (�g���b�L���O���~�B�V�O�i�����s����Ȃ��B=Exe��~���Ȃǂɔ��s)
//------------------------------------------
void Tracking::TrackingStop()
{
	m_bTkRun = false;
	m_dChangeLengthMax = 0;

	// �g���b�L���O�^�C�}�[��~
	CancelWaitableTimer(m_evTkTimer);

	// �O�ׂ̈ɓo�^�ς݂̃V�O�i����S�ăL�����Z��
	if(NULL != m_evTrackingEnd)		ResetEvent(*m_evTrackingEnd);
	if(NULL != m_evTrackingCancel)	ResetEvent(*m_evTrackingCancel);
}

//------------------------------------------
// �X���b�h
//------------------------------------------
UINT WINAPI Tracking::MainThread(LPVOID param)
{
	////// ���C���C���X�^���X���o��
	Tracking *p = (Tracking *)param;
	p->SetTid();								// ���X���b�hID�Z�b�g


	// WaitForMultipleObjects�C�x���g
	HANDLE	hArray[] = {p->my_evStop, 
						p->m_evTkCancelStop, 
						p->m_evTkTimer };
	enum {	EV_STOP = WAIT_OBJECT_0,		// �X���b�h�I��
			EV_CANCEL,						// ������~
			EV_TIMER,						// ������^�C�}
			EV_END};

	// ���b�Z�[�W�L���[�ǂݍ���
	while (1) {
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);
		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// �X���b�h�I��
			p->m_bTkRun = false;
			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_CANCEL:												// �����I��
			if( ! p->m_bTkRun) break;														// �L�����Z�����������ɁA���O��End�ƂȂ��Ă����ꍇ�΍�

			if(NULL != p->m_evTrackingCancel) SetEvent(*p->m_evTrackingCancel);				// �Ăь��ւ̃C�x���g�ʒm
			CancelWaitableTimer(p->m_evTkTimer);
			p->m_bTkRun = false;
			p->m_dChangeLengthMax = 0;
			break;

//-----------------------------------------------------------------------------------------------
		case EV_TIMER:												// �^�C�}�[
			if( ! p->m_bTkRun) break;														// �O�̈�

			// �ʒmt�Ώۋ����ɒB�����ꍇ�ɒʒm
			if( *p->m_dLength - (p->m_dDistNow - p->m_dChangeLengthMax) >= (double)p->m_nDist) {
				SetEvent(*p->m_evTrackingEnd);
				CancelWaitableTimer(p->m_evTkTimer);
				p->m_bTkRun = false;
				p->m_dChangeLengthMax = 0;
			}
			break;
		}
	}
}
