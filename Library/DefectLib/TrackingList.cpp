// TrackingList.cpp: TrackingList �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TrackingList.h"

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////

//------------------------------------------
// �R���X�g���N�^
// const double* length �`�F�b�N���鋗���|�C���^ [mm] (�ʏ�́A�J�E���^�[�{�[�h�ŊǗ����Ă��鋗���̃A�h���X��n��)
//------------------------------------------
TrackingList::TrackingList(const double* length) :
m_dLength(length),
gque_Deli(1024)
{
	// ������
	InitializeCriticalSection(&m_csLock);						// �N���e�B�J���Z�N�V�����I�u�W�F�N�g��������
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
TrackingList::~TrackingList()
{
	// ���
	DeleteCriticalSection(&m_csLock);							// �N���e�B�J���Z�N�V�����I�u�W�F�N�g�����
}

//------------------------------------------
// �V�K�g���b�L���O �A�C�e����ǉ�����
// DWORD len ��m�̈ʒu��ʉ߂�����ʒm? [mm]
// int nKey �L�[��� (0:�L�[�`�F�b�N�����̈Ӗ��ƂȂ�)
// long nAddr1 ���[�U�[�p���[�N�G���A
// long nAddr2 ���[�U�[�p���[�N�G���A
// long const* pAddr ���[�U�[�p���[�N�G���A (�ʏ�N���X�̃|�C���^���Z�b�g)(���g�p����NULL)
//------------------------------------------
void TrackingList::TrackingAdd_Len(DWORD len, int nKey, long nAddr1, long nAddr2, long const* pAddr)
{
	long dist = (long)(len - *m_dLength);	// ��m�Œʉ�
	if(0 > dist) dist = 0;					// ���ɒʉ߂��Ă�����A���`�F�b�N�ő����N���Ƃ���

	this->TrackingAdd_Dist(dist, nKey, nAddr1, nAddr2, pAddr);
}

//------------------------------------------
// �V�K�g���b�L���O �A�C�e����ǉ�����
// DWORD dist ��mm��ɒʒm? [mm]
// int nKey �L�[��� (0:�L�[�`�F�b�N�����̈Ӗ��ƂȂ�)
// long nAddr1 ���[�U�[�p���[�N�G���A
// long nAddr2 ���[�U�[�p���[�N�G���A
// long const* pAddr ���[�U�[�p���[�N�G���A (�ʏ�N���X�̃|�C���^���Z�b�g)(���g�p����NULL)
//------------------------------------------
void TrackingList::TrackingAdd_Dist(DWORD dist, int nKey, long nAddr1, long nAddr2, long const* pAddr)
{
	AutoLock autolock(&m_csLock);

	// �V�K�g���b�L���O�A�C�e���𐶐�
	TR_INF*	pInf = new TR_INF();
	pInf->nDist		 = dist;
	pInf->dDistNow	 = *m_dLength;
	pInf->dDistEnd	 = 0.0;
	pInf->dChangeLengthMax = 0;
	pInf->emEndState = TR_STOP;
	pInf->nKey		 = nKey;
	pInf->nAddr1	 = nAddr1;
	pInf->nAddr2	 = nAddr2;	
	pInf->pAddr		 = (long*)pAddr;

	// ���X�g �ǉ�
	m_List.push_back(pInf);
}


//------------------------------------------
// �R�C���ؑ֓����������A�O��R�C������������A�C�e���ɃZ�b�g����
// double dOldLen �O��R�C���� [mm]
//------------------------------------------
void TrackingList::TrackingLengthChange(double dOldLen)
{
	AutoLock autolock(&m_csLock);

	//// ���X�g�i�[�� �S���`�F�b�N���s��
	TR_INF*							pInf;					// �g���b�L���O���
	std::list<TR_INF*>::iterator	itr;					// �C�e���[�^
	for( itr = m_List.begin(); itr != m_List.end(); itr++ ) {
		pInf = *itr;

		// �Q�^�͂����������Z
		pInf->dChangeLengthMax += dOldLen;
	}
}

//------------------------------------------
// �w��L�[No�̃A�C�e���̃N���[�����擾���� (�����m�肽���ꍇ��)
// int nKey �擾����L�[No
// TR_INF* inf ��ԍŏ��Ɉ�v�����L�[�̃A�C�e���̃N���[�� (���̂ł͖����̂Œ���)
// �߂�l true:�f�[�^�L��  false:�Y���L�[����
//------------------------------------------
bool TrackingList::GetCloneKeyItem(int nKey, TR_INF* inf)
{
	//// ���X�g�i�[�� �S���`�F�b�N���s��
	TR_INF*							pInf;					// �g���b�L���O���
	std::list<TR_INF*>::iterator	itr = m_List.begin();	// �C�e���[�^
	for( itr = m_List.begin(); itr != m_List.end(); itr++ ) {
		pInf  = *itr;
		// ��v����L�[�L��H
		if( pInf->nKey == nKey ) {
			memcpy(inf, pInf, sizeof(TR_INF));
			return true;
		}
	}

	// ��v����
	return false;
}

//------------------------------------------
// �g���b�L���O�A�C�e�� ���`�F�b�N���āA�w�苗����ʔ��Ă����� TR_END �ŃL���[�C���O
//------------------------------------------
void TrackingList::TrackingCheck()
{
	AutoLock autolock(&m_csLock);
	
	//// �O����
	double	dLength = *m_dLength;							// ���݂̋�����ێ�

	//// ���X�g�i�[�� �S���`�F�b�N���s��
	bool							bNext;					// ���C�e���[�^�`�F�b�N�p
	TR_INF*							pInf;					// �g���b�L���O���
	std::list<TR_INF*>::iterator	itr = m_List.begin();	// �C�e���[�^
	while( itr != m_List.end() ) {
		pInf  = *itr;
		bNext = true;

		// �ʒmt�Ώۋ����ɒB�����ꍇ�ɒʒm
		if( dLength - (pInf->dDistNow - pInf->dChangeLengthMax) >= (double)pInf->nDist ) {
			// �X���b�h�L���[�ɓo�^�o�����ꍇ�̂݁A���X�g����폜
			if( gque_Deli.GetFreeCount() > 0 ) {
				pInf->emEndState = TR_END;
				pInf->dDistEnd	 = dLength;
				gque_Deli.AddToTail( pInf );
				// ���X�g����폜
				itr = m_List.erase(itr);	// �폜������A���̃C�e���[�^��߂�
				bNext = false;
			}
		}
		
		// ���̃C�e���[�^��
		if(bNext) itr++;
	}
}

//------------------------------------------
// ������~ (������R�[������� TR_CANCEL �ŃL���[�C���O����)
// double dLength �����i�񂾎��Ƃ��ăg���b�L���O�𔭐������� [mm]
// int nKey �L�[��� (0:�L�[�`�F�b�N�����̈Ӗ��ƂȂ�)
//------------------------------------------
void TrackingList::TrackingCancel(double dLength, int nKey)
{
	AutoLock autolock(&m_csLock);

	//// �O����
	double	dNowLength = *m_dLength;						// ���݂̋�����ێ�

	//// ���X�g�i�[�� �S���`�F�b�N���s��
	bool							bNext;					// ���C�e���[�^�`�F�b�N�p
	TR_INF*							pInf;					// �g���b�L���O���
	std::list<TR_INF*>::iterator	itr = m_List.begin();	// �C�e���[�^
	while( itr != m_List.end() ) {
		pInf  = *itr;
		bNext = true;

		
		// �ʒmt�Ώۋ����ɒB�����ꍇ�ɒʒm
		if( ( dLength - (pInf->dDistNow - pInf->dChangeLengthMax) >= (double)pInf->nDist ) &&
			( (0==nKey) || (nKey==pInf->nKey)) ){

			// �X���b�h�L���[�ɓo�^�o�����ꍇ�̂݁A���X�g����폜
			if( gque_Deli.GetFreeCount() > 0 ) {
				pInf->emEndState = TR_CANCEL;
				pInf->dDistEnd	 = dNowLength;
				gque_Deli.AddToTail( pInf );
				// ���X�g����폜
				itr = m_List.erase(itr);	// �폜������A���̃C�e���[�^��߂�
				bNext = false;
			}
		}
		
		// ���̃C�e���[�^��
		if(bNext) itr++;
	}
}

//------------------------------------------
// ������~ �w��L�[�݂̂��L�����Z��
// int nKey �L�[��� (0:�L�[�`�F�b�N�����̈Ӗ��ƂȂ�)
//------------------------------------------
void TrackingList::TrackingCancelKey(int nKey)
{
	AutoLock autolock(&m_csLock);
	
	//// �O����
	double	dLength = *m_dLength;							// ���݂̋�����ێ�

	//// ���X�g�i�[�� �S���`�F�b�N���s��
	bool							bNext;					// ���C�e���[�^�`�F�b�N�p
	TR_INF*							pInf;					// �g���b�L���O���
	std::list<TR_INF*>::iterator	itr = m_List.begin();	// �C�e���[�^
	while( itr != m_List.end() ) {
		pInf  = *itr;
		bNext = true;

		
		// �ʒmt�Ώۋ����ɒB�����ꍇ�ɒʒm
		if( (0==nKey) || (nKey==pInf->nKey) ) {

			// �X���b�h�L���[�ɓo�^�o�����ꍇ�̂݁A���X�g����폜
			if( gque_Deli.GetFreeCount() > 0 ) {
				pInf->emEndState = TR_CANCEL;
				pInf->dDistEnd	 = dLength;
				gque_Deli.AddToTail( pInf );
				// ���X�g����폜
				itr = m_List.erase(itr);	// �폜������A���̃C�e���[�^��߂�
				bNext = false;
			}
		}
		
		// ���̃C�e���[�^��
		if(bNext) itr++;
	}
}

//------------------------------------------
// �g���b�L���O�A�C�e�� ��S�ĉ�����L���[�C���O����
//------------------------------------------
void TrackingList::TrackingStop()
{
	AutoLock autolock(&m_csLock);

	//// �O����
	double	dLength = *m_dLength;							// ���݂̋�����ێ�

	//// ���X�g�i�[�� �S���`�F�b�N���s��
	TR_INF*							pInf;					// �g���b�L���O���
	std::list<TR_INF*>::iterator	itr;					// �C�e���[�^
	for( itr = m_List.begin(); itr != m_List.end(); itr++ ) {
		pInf = *itr;

		// �X���b�h�L���[�ɓo�^�o�����ꍇ�̂݁A���X�g����폜
		if( gque_Deli.GetFreeCount() > 0 ) {
			pInf->emEndState = TR_STOP;
			pInf->dDistEnd	 = dLength;
			gque_Deli.AddToTail( pInf );
		}
	}
	
	// ���X�g����폜
	m_List.clear();
}

//------------------------------------------
// �V�K�L���[ �A�C�e����ǉ����� (�g���b�L���O�����ɁA�����ɃL���[�C���O)
// int nKey �L�[��� (0:�L�[�`�F�b�N�����̈Ӗ��ƂȂ�)
// long nAddr1 ���[�U�[�p���[�N�G���A
// long nAddr2 ���[�U�[�p���[�N�G���A
// long const* pAddr ���[�U�[�p���[�N�G���A (�ʏ�N���X�̃|�C���^���Z�b�g)(���g�p����NULL)
//------------------------------------------
void TrackingList::QueAdd(DWORD dist, int nKey, long nAddr1, long nAddr2, long const* pAddr)
{
	AutoLock autolock(&m_csLock);

	// �V�K�g���b�L���O�A�C�e���𐶐�
	TR_INF*	pInf = new TR_INF();
	pInf->nDist		 = dist;
	pInf->dDistNow	 = *m_dLength;
	pInf->dDistEnd	 = *m_dLength;
	pInf->dChangeLengthMax = 0;
	pInf->dDistNow	 = *m_dLength;
	pInf->emEndState = TR_STOP;
	pInf->nKey		 = nKey;
	pInf->nAddr1	 = nAddr1;
	pInf->nAddr2	 = nAddr2;	
	pInf->pAddr		 = (long*)pAddr;


	// �����L���[�C���O
	if( gque_Deli.GetFreeCount() > 0 ) {
		pInf->emEndState = TR_END;
		gque_Deli.AddToTail( pInf );
	}
}

//------------------------------------------
// ���ׂẴL���[���J��
//------------------------------------------
void TrackingList::QueAllFree()
{
	AutoLock autolock(&m_csLock);

	//// ���X�g�i�[�� �S���`�F�b�N���s��
	TR_INF*							pInf;					// �g���b�L���O���
	std::list<TR_INF*>::iterator	itr;					// �C�e���[�^
	for( itr = m_List.begin(); itr != m_List.end(); itr++ ) {
		pInf = *itr;

		// �J��
		if ( pInf->pAddr != NULL ) delete pInf->pAddr;		// �f�[�^���J��
		delete pInf;
		pInf = NULL;
	}
	
	// ���X�g����폜
	m_List.clear();
}

//------------------------------------------
// �X���b�h
//------------------------------------------
UINT WINAPI TrackingList::MainThread(LPVOID param)
{
	////// ���C���C���X�^���X���o��
	TrackingList *p = (TrackingList *)param;
	p->my_Tid = GetCurrentThreadId();

	// �^�C�}�[�Z�b�g
	HANDLE evTimer = CreateWaitableTimer(NULL,FALSE, NULL);
	__int64 ts = 0;
	SetWaitableTimer(evTimer, (LARGE_INTEGER *)&ts, 20, NULL, NULL, FALSE);

	// WaitForMultipleObjects�C�x���g
	HANDLE	hArray[] = {p->my_evStop,  
						evTimer
						};
	enum {	EV_STOP = WAIT_OBJECT_0,
			EV_TIMER, 
			EV_END	};

	// ���b�Z�[�W�L���[�ǂݍ���
	while (1) {
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);
		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:														// �X���b�h�I��
			// ���[�J���̃n���h�����
			CancelWaitableTimer(evTimer);
			CloseHandle(evTimer);
			// �g���b�L���O�̒�~
			p->TrackingStop();
			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_TIMER:														// �^�C�}�[
			// �ʒmt�Ώۋ����ɒB�����ꍇ�ɒʒm
			p->TrackingCheck(); 
			break;
		}
	}
	return 0;
}

