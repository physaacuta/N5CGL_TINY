// ContecPioBaseManager.cpp: ContecPioBaseManager �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ContecPioBaseManager.h"
#ifndef  DEFECTLIB_CONTEC_PIO_NON		////// ���S������


// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
ContecPioBaseManager::ContecPioBaseManager() :
m_nPortNum(0),
m_inpPortNo(NULL),
m_outPortNo(NULL),
m_bufInp(NULL),
m_bufOut(NULL),
m_pDI(NULL),
m_pDO(NULL),
m_bStatusDi(TRUE),
m_bStatusDo(TRUE),
m_nCheckNum(0),
m_bAuto(true),
m_bSmyu(false),
mcls_pLog(NULL)
{
	//// ������
	memset(m_CheckArray, 0x00, sizeof(m_CheckArray));

	//// �C�x���g����
	m_evDiErr		= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evDoErr		= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evOutputEvt	= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evLockReadStart = CreateEvent(NULL, FALSE, FALSE, NULL);				// �����Ǎ��݊J�n�C�x���g
	m_evLockReadEnd	  = CreateEvent(NULL, FALSE, FALSE, NULL);				// �����Ǎ��݊����C�x���g


	//// DO�p���X�o�͗p
	m_nPulsDoNum = 0;
	memset(m_DoPulsArray, 0x00, sizeof(m_DoPulsArray));

	//// �V�~�����[�^�p
	m_sSmyuName = "";
	m_hTblSmyu = NULL;
	mtbl_pSmyu = NULL; 
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
ContecPioBaseManager::~ContecPioBaseManager()
{
	//// �̈�J��
	Free();

	//// �C�x���g�J��
	CloseHandle(m_evDiErr);
	CloseHandle(m_evDoErr);
	CloseHandle(m_evOutputEvt);
	CloseHandle(m_evLockReadStart);
	CloseHandle(m_evLockReadEnd);

	//// DO�p���X�o�͗p
	for(int ii=0; ii<mc_nMaxDoPulsCount; ii++) {
		if(NULL != m_DoPulsArray[ii].evTimer) {
			CancelWaitableTimer(m_DoPulsArray[ii].evTimer);
			CloseHandle(m_DoPulsArray[ii].evTimer);
		}
	}

	//// �V�~�����[�^�p
	SetSmyuMode(false);
}

/*
//------------------------------------------
// �X���b�h
//------------------------------------------
UINT WINAPI ContecPioBaseManager::MainThread(LPVOID param)
{
	int ii;
	bool bRefresh = false;

	////// ���C���C���X�^���X���o��
	ContecPioBaseManager *p = (ContecPioBaseManager *)param;
	p->SetTid();								// ���X���b�hID�Z�b�g

	//// �^�C�}�[�Z�b�g
	HANDLE evTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	__int64 ts = 0;	// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(evTimer, (LARGE_INTEGER *)&ts, mc_nReadInterval, NULL, NULL, FALSE);


//
//	// WaitForMultipleObjects�C�x���g
//	HANDLE	hArray[] = {p->my_evStop,
//						p->m_evOutputEvt,
//						evTimer 
//						};
//
	enum {	EV_STOP = WAIT_OBJECT_0,		// �X���b�h�I��
			EV_OUTPUT,						// DO�ݒ�C�x���g
			EV_READ_START,					// �����Ǎ���
			EV_TIMER,						// ������Ǎ��C�x���g

			EV_DOPULS	};					// DO�p���X�o�̓^�C�}�[


	HANDLE	hArray[EV_DOPULS+mc_nMaxDoPulsCount];
	hArray[0] = p->my_evStop;
	hArray[1] = p->m_evOutputEvt;
	hArray[2] = p->m_evLockReadStart;
	hArray[3] = evTimer;


	//// DO�p���X�o�͗p�^�C�}�[�Z�b�g
	int nWaitNum = EV_DOPULS + p->m_nPulsDoNum;
	for(ii=0; ii<p->m_nPulsDoNum; ii++) hArray[EV_DOPULS+ii] = p->m_DoPulsArray[ii].evTimer;



	// ���b�Z�[�W�L���[�ǂݍ���
	pLOG(em_MSG), "[ContecPioBaseManager] �X���b�h�J�n [0x%X]", p->GetTid());
	while (1) {
		DWORD ret = WaitForMultipleObjects(nWaitNum, hArray, FALSE, INFINITE);

		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// �X���b�h�I��
			pLOG(em_MSG), "[ContecPioBaseManager] �X���b�h�I�� [0x%X]", p->GetTid());
			// �^�C�}�[�j��
			CancelWaitableTimer(evTimer);
			CloseHandle(evTimer);
			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_OUTPUT:												// DO�ݒ�C�x���g
			p->Output();										
			break;


//-----------------------------------------------------------------------------------------------
		case EV_READ_START:											// �����Ǎ���
			bRefresh = true;
		case EV_TIMER:												// ������Ǎ��C�x���g

			if( p->m_bAuto ) {
				// �o�b�t�@������
				memset(p->m_bufInp, 0x00, sizeof(BYTE)*p->m_nPortNum);

				// �M������
				p->Input();

			}

			//// �M����Ԃ��`�F�b�N
			for( ii=0; ii<(int)p->m_nCheckNum; ii++ ) {
				int retc = p->PioNowState(p->m_CheckArray[ii].port, p->m_CheckArray[ii].addr);
				// �V�O�i���ʒm
				if( 1 == retc && NULL != p->m_CheckArray[ii].evSignOn  ) SetEvent(p->m_CheckArray[ii].evSignOn  );
				if(-1 == retc && NULL != p->m_CheckArray[ii].evSignOff ) SetEvent(p->m_CheckArray[ii].evSignOff );
			}
			
			//// �X�e�[�^�X�̃A�N�Z�X�ׁ̈A����l�𔽉f
			memcpy(p->m_pDI, p->m_bufInp, sizeof(BYTE)*p->m_nPortNum);


			//// �����ǂݍ��݊����V�O�i��
			if(bRefresh) SetEvent(p->m_evLockReadEnd);									// �Ǎ��݊����V�O�i���Z�b�g
			bRefresh = false;
			break;


//-----------------------------------------------------------------------------------------------
		case EV_DOPULS + 0:											// DO�p���X�o�̓^�C�}�[
		case EV_DOPULS + 1:
		case EV_DOPULS + 2:
		case EV_DOPULS + 3:
		case EV_DOPULS + 4:
		case EV_DOPULS + 5:
		case EV_DOPULS + 6:
		case EV_DOPULS + 7:
		case EV_DOPULS + 8:
		case EV_DOPULS + 9:
		case EV_DOPULS +10:
		case EV_DOPULS +11:
		case EV_DOPULS +12:
		case EV_DOPULS +13:
		case EV_DOPULS +14:
		case EV_DOPULS +15:
			if(true) {
				// ON��OFF
				int index = ret - EV_DOPULS;
				p->m_DoPulsArray[index].bExec = false; 
				p->SetDO(p->m_DoPulsArray[index].port, p->m_DoPulsArray[index].addr, ! p->m_DoPulsArray[index].bOnOff, false ); 
				
				// �������f
				p->Output();
			}
			break;

//-----------------------------------------------------------------------------------------------
		default :
			// ���肦�Ȃ��I�I
			break;
		}
	}
	return 0;
}
*/
//------------------------------------------
// �f�W�^���l�X�V
// int port �`�F�b�N����|�[�g�z��ʒu 
// BYTE addr �`�F�b�N����r�b�g�ʒu (0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 �̂����ꂩ)
// BYTE* now ����l 
// BYTE* old �O��l
// �߂�l �ύX��� (9:�ύX����(ON) -9:�ύX����(OFF) 1:OFF��ON -1:ON��OFF)
//------------------------------------------
int ContecPioBaseManager::PioNowState(PIO_PORT port, PIO_BIT addr, BYTE* now, BYTE* old )
{
	if( (now[port] & addr) == addr ) {		// ���� ON
		if( (old[port] & addr) == 0 ) {			// �O�� OFF
			old[port] |= addr;						// �l�ύX(�w��r�b�g�̂�ON)
			return 1;										// OFF��ON
		} else {
			return 9;										// ON�̂܂�
		}
	} else {								// ���� OFF
		if( (old[port] & addr) == addr ) {		// �O�� ON
			old[port] &= ~addr;						// �l�ύX(�w��r�b�g�̂�OFF)
			return -1;										// ON��OFF
		} else {
			return -9;										// OFF�̂܂�
		}
	}
}
//------------------------------------------
// PIO�ω���ԃ`�F�b�N
// int port �`�F�b�N����|�[�g�z��ʒu 
// BYTE addr �`�F�b�N����r�b�g�ʒu (0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 �̂����ꂩ)
// BYTE const* now ����l 
// BYTE const* old �O��l
// �߂�l �ύX��� (9:�ύX����(ON) -9:�ύX����(OFF) 1:OFF��ON -1:ON��OFF)
//------------------------------------------
int ContecPioBaseManager::PioNowStateCheck(PIO_PORT port, PIO_BIT addr, BYTE const* now, BYTE const* old )
{
	if( (now[port] & addr) == addr ) {		// ���� ON
		if( (old[port] & addr) == 0 ) {			// �O�� OFF
			return 1;										// OFF��ON
		} else {
			return 9;										// ON�̂܂�
		}
	} else {									// ���� OFF
		if( (old[port] & addr) == addr ) {		// �O�� ON
			return -1;										// ON��OFF
		} else {
			return -9;										// OFF�̂܂�
		}
	}
}

//------------------------------------------
// �`�F�b�N��� �ݒ�
// DWORD port		�`�F�b�N����|�[�g�z��ʒu 
// BYTE addr		�`�F�b�N����r�b�g�ʒu (0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 �̂����ꂩ)
// HANDLE evSignOn	ON�ʒm�p�V�O�i��
// HANDLE evSignOff OFF�ʒm�p�V�O�i��
//------------------------------------------
void ContecPioBaseManager::SetCheckArray(PIO_PORT port, PIO_BIT addr, HANDLE evSignOn, HANDLE evSignOff) 
{
	m_CheckArray[m_nCheckNum].addr		 = addr;
	m_CheckArray[m_nCheckNum].port		 = port;
	m_CheckArray[m_nCheckNum].evSignOn	 = evSignOn;
	m_CheckArray[m_nCheckNum].evSignOff  = evSignOff;
	m_nCheckNum++;
}

//------------------------------------------
// DO �w��ʒu�r�b�g ON/OFF
// int port			�`�F�b�N����|�[�g�z��ʒu 
// BYTE addr		�`�F�b�N����r�b�g�ʒu (0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 �̂����ꂩ)
// bool onoff		On / Off �ݒ�
// bool evt			�����݂��s���H (true:���ۂɃZ�b�g false:�����o�b�t�@�̒l�ɂ̂݃Z�b�g)
//------------------------------------------
void ContecPioBaseManager::SetDO(PIO_PORT port, PIO_BIT addr, bool onoff, bool evt)
{
	if( onoff )	m_pDO[port] |= addr;
	else		m_pDO[port] &= ~addr;
	if(evt) SetEvent(m_evOutputEvt);
}

//------------------------------------------
// DI �w��ʒu�r�b�g ON/OFF
// int port			�`�F�b�N����|�[�g�z��ʒu 
// BYTE addr		�`�F�b�N����r�b�g�ʒu (0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 �̂����ꂩ)
// bool onoff		On / Off �ݒ�
//------------------------------------------
void ContecPioBaseManager::SetDI(PIO_PORT port, PIO_BIT addr, bool onoff)
{
	if( onoff )	m_bufInp[port] |= addr;
	else		m_bufInp[port] &= ~addr;
}

//------------------------------------------
// �w��ʒu�r�b�g �� No �ɕϊ�
// BYTE addr		�`�F�b�N����r�b�g�ʒu (0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 �̂����ꂩ)
// �߂�l			0�`7
//------------------------------------------
int ContecPioBaseManager::GetBitToIndex(BYTE addr)
{
	if		(BIT_0 == addr)	{ return 0; }
	else if (BIT_1 == addr)	{ return 1; }
	else if (BIT_2 == addr)	{ return 2; }
	else if (BIT_3 == addr)	{ return 3; }
	else if (BIT_4 == addr)	{ return 4; }
	else if (BIT_5 == addr)	{ return 5; }
	else if (BIT_6 == addr)	{ return 6; }
	else if (BIT_7 == addr)	{ return 7; }
	else					{ return 0; }	// ����		
}
//------------------------------------------
// �w��ʒuNo �� �r�b�g �ɕϊ�
// int index			0�`7
// �߂�l		�`�F�b�N����r�b�g�ʒu (0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 �̂����ꂩ)			
//------------------------------------------
BYTE ContecPioBaseManager::GetIndexToBit(int index)
{
	if		(0 == index)	{ return BIT_0; }
	else if (1 == index)	{ return BIT_1; }
	else if (2 == index)	{ return BIT_2; }
	else if (3 == index)	{ return BIT_3; }
	else if (4 == index)	{ return BIT_4; }
	else if (5 == index)	{ return BIT_5; }
	else if (6 == index)	{ return BIT_6; }
	else if (7 == index)	{ return BIT_7; }
	else					{ return BIT_0; }	// ����	
}


//------------------------------------------
// �����X�V (�Ǎ��݊�������܂Ńu���b�N�����)
//------------------------------------------
void ContecPioBaseManager::GetDIRefresh()
{
	SetEvent(m_evLockReadStart);
	WaitForSingleObject(m_evLockReadEnd, 2000);
}


//////////////////////////////////////////////////////////////////////
// DO�p���X

//------------------------------------------
// DO�p���X�ݒ�(Start�O�Ɏ��s���邱��)
// DWORD port		�`�F�b�N����|�[�g�z��ʒu 
// BYTE addr		�`�F�b�N����r�b�g�ʒu (0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 �̂����ꂩ)
// bool bOnOff		�Z�b�g���̏�� (true:OFF��ON��OFF  false:ON��OFF��ON)
//------------------------------------------
void ContecPioBaseManager::SetPulsDoSetting(PIO_PORT port, PIO_BIT addr, bool bOnOff)
{
	_ASSERT( mc_nMaxDoPulsCount > m_nPulsDoNum );

	m_DoPulsArray[m_nPulsDoNum].port  = port;
	m_DoPulsArray[m_nPulsDoNum].addr  = addr;
	m_DoPulsArray[m_nPulsDoNum].bOnOff = bOnOff;
	m_DoPulsArray[m_nPulsDoNum].bExec  = false;
	m_DoPulsArray[m_nPulsDoNum].evTimer = CreateWaitableTimer(NULL, FALSE, NULL);

	m_nPulsDoNum++;
}

//------------------------------------------
// DO �w��ʒu�r�b�g�� �w�莞�ԃp���X�o��
// DWORD port		�`�F�b�N����|�[�g�z��ʒu 
// BYTE addr		�`�F�b�N����r�b�g�ʒu (0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 �̂����ꂩ)
// int nTime		�Z�b�g���� [ms]
//------------------------------------------
bool ContecPioBaseManager::SetPulsDO(PIO_PORT port, PIO_BIT addr, int nTime)
{
	__int64 ts = nTime * -10000;			// 1�~���b��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)

	for(int ii=0; ii<m_nPulsDoNum; ii++) {
		if( m_DoPulsArray[ii].port == port && m_DoPulsArray[ii].addr == addr) {

			// ON
			m_DoPulsArray[ii].bExec = true; 
			SetDO(m_DoPulsArray[ii].port, m_DoPulsArray[ii].addr, m_DoPulsArray[ii].bOnOff, true);

			// OFF�̃^�C�}�[�Z�b�g
			SetWaitableTimer(m_DoPulsArray[ii].evTimer, (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE);
			return true;
		}
	}
	return false;
}
//------------------------------------------
// DO�p���X�o�͎��s���H
// DWORD port		�`�F�b�N����|�[�g�z��ʒu 
// BYTE addr		�`�F�b�N����r�b�g�ʒu (0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 �̂����ꂩ)
//------------------------------------------
bool ContecPioBaseManager::IsPulsDO(PIO_PORT port, PIO_BIT addr)
{
	for(int ii=0; ii<m_nPulsDoNum; ii++) {
		if( m_DoPulsArray[ii].port == port && m_DoPulsArray[ii].addr == addr) {
			if( m_DoPulsArray[ii].bExec ) return true;
			else						  return false;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////
// �V�~�����[�V����
//------------------------------------------
// �V�~�����[�^���[�h�̐ݒ�
// bool bMode True:�V�~�����[�V������ false:�ʏ�
//------------------------------------------
bool ContecPioBaseManager::SetSmyuMode(bool bMode) 
{
	bool bRetc=true;
	//// �V�~�����[�V������ 
	if(bMode) {
		mem_connect(&m_hTblSmyu, (LPVOID *)&mtbl_pSmyu, m_sSmyuName);
		if(m_hTblSmyu == NULL) {
			LOG(em_ERR), "[ContecPioBaseManager] �V�~�����[�^�p���L�������A�N�Z�X���s [%s]", m_sSmyuName);
			m_hTblSmyu = NULL;
			mtbl_pSmyu = NULL; 
			bMode = false;
			bRetc = false;
			goto ending;
		}

	} else {
		if(NULL != mtbl_pSmyu) {
			mem_close(&m_hTblSmyu, (LPVOID *)&mtbl_pSmyu);
		}
		m_hTblSmyu = NULL;
		mtbl_pSmyu = NULL; 
	}

	//// �t���O�̕ύX
ending:
	m_bSmyu = bMode;
	return bRetc;
}

//------------------------------------------
// �f�W�^���f�[�^�̓���
//------------------------------------------
void ContecPioBaseManager::SmyuInput()
{
	//// �ʏ��PIO�ǂݍ���
	memcpy( m_bufInp, mtbl_pSmyu->nDi, sizeof(BYTE)*m_nPortNum);
}

//------------------------------------------
// �f�W�^���f�[�^�̏o��
//------------------------------------------
void ContecPioBaseManager::SmyuOutput()
{
	memcpy( mtbl_pSmyu->nDo, m_bufOut, sizeof(BYTE)*m_nPortNum);
}

#endif