// ContecPioManager.cpp: ContecPioManager �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ContecPioManager.h"
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
ContecPioManager::ContecPioManager() :
m_hDrv(CONTEC_PIO_NOID),
m_bIsOpen(false),
ContecPioBaseManager()
{
#ifdef DEFECTLIB_CONTEC_PIO_WMD		// WMD��
	//// ���荞�ݗp
	m_nCheckCallBackNum = 0;
	memset(m_CheckCallBackArray, 0x00, sizeof(m_CheckCallBackArray));
#endif
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
ContecPioManager::~ContecPioManager()
{
	//// �̈�J��
	Free();
}

#ifndef DEFECTLIB_CONTEC_PIO_WMD		// �]����
//------------------------------------------
// �̈�m��
// WORD drvno PIO�{�[�h �h���C�oNo
// WORD grpno PIO�{�[�h �O���[�vNo
// WORD port_num �|�[�g��
// WORD kid �@��ID
//------------------------------------------
bool ContecPioManager::Alloc(WORD drvno, WORD grpno, WORD port_num, WORD kid)
{
	m_drvno = drvno;
	m_grpno = grpno;

#else									// WMD��
//------------------------------------------
// �̈�m��
// WORD port_num �|�[�g��
// WORD kid �@��ID
// char const* dev_name �{�[�h����
//------------------------------------------
bool ContecPioManager::Alloc(WORD port_num, WORD kid, char const* dev_name)
{
	memset(m_dev_name, 0x00, sizeof(m_dev_name));
	strcpy(m_dev_name, dev_name);

#endif

	_ASSERT( port_num<=8 );
	m_nPortNum = port_num;
	m_nKID = kid;

	// new
	m_inpPortNo = new short[m_nPortNum];
	m_outPortNo = new short[m_nPortNum];
	m_bufInp	= new BYTE[m_nPortNum];
	m_bufOut	= new BYTE[m_nPortNum];
	m_bufWk		= new BYTE[m_nPortNum];
	m_pDI		= new BYTE[m_nPortNum];
	m_pDO		= new BYTE[m_nPortNum];

	// Set port
	for( WORD ii=0; ii<port_num; ii++ ) {
		m_inpPortNo[ii] = ii;
		m_outPortNo[ii] = ii;
	}

#ifndef DEFECTLIB_CONTEC_PIO_WMD		// �]����
	// Set DINP
	m_lpDInp.InpPortNo	= (WORD*)m_inpPortNo;	// Set the pointer of InpPortNo
	m_lpDInp.PortNum	= m_nPortNum;			// Total numbe of logical ports
	m_lpDInp.Buf		= m_bufInp;				// Set the pointer of Buf

	// Set DOUT
	m_lpDOut.OutPortNo	= (WORD*)m_outPortNo;	// Set the pointer of OutPortNo
	m_lpDOut.PortNum	= m_nPortNum;			// Total numbe of logical ports
	m_lpDOut.Buf		= m_bufOut;				// Set the pointer of Buf
#endif

	// �M��������
	memset(m_pDI,		0x00, sizeof(BYTE)*m_nPortNum);
	memset(m_pDO,		0x00, sizeof(BYTE)*m_nPortNum);
	memset(m_bufInp,	0x00, sizeof(BYTE)*m_nPortNum);

	//// �V�~�����[�^�p����
#ifndef DEFECTLIB_CONTEC_PIO_WMD		// �]����
	m_sSmyuName.Format(SMYU_PIO_TBL_NAME, grpno );
#else
	m_sSmyuName.Format(SMYU_PIO_TBL_NAME, dev_name );
#endif

	//// �ڑ�
#ifndef NO_HW
	return Open(true);
#else
	return true;
#endif
}

//------------------------------------------
// �̈�J��
//------------------------------------------
void ContecPioManager::Free()
{
#ifndef NO_HW
	Close();
#endif

	// ���
	SAFE_DELETE_ARRAY(m_inpPortNo);
	SAFE_DELETE_ARRAY(m_outPortNo);
	SAFE_DELETE_ARRAY(m_bufInp);
	SAFE_DELETE_ARRAY(m_bufOut);
	SAFE_DELETE_ARRAY(m_bufWk);
	SAFE_DELETE_ARRAY(m_pDI);
	SAFE_DELETE_ARRAY(m_pDO);
}


//------------------------------------------
// �ڑ�
// bool bDsp true�� ���O�\��
//------------------------------------------
bool ContecPioManager::Open(bool bDsp)
{
	if(m_bIsOpen) return true;	

	// �I�[�v��
#ifndef DEFECTLIB_CONTEC_PIO_WMD		// �]����
	DWORD nRet = DioOpen(&m_hDrv, m_drvno, m_grpno);
#else									// WMD��
	long  nRet = DioInit(m_dev_name, &m_hDrv);
#endif

	if( 0 != nRet ) {
		m_hDrv = CONTEC_PIO_NOID;
		m_bIsOpen = false;

		if(bDsp) {
			LOG(em_ERR), "[ContecPioManager] �I�[�v���G���[ [err_code = %d]", nRet);
			syslog(SYSNO_PIO_ERR, "[OPEN err_code = %d]", nRet);
		}
		m_bStatusDi = FALSE;
		m_bStatusDo = FALSE;
		SetEvent(m_evDiErr);
		SetEvent(m_evDoErr);
		KizuFunction::SetStatus(m_nKID, false, bDsp, "[PIO�{�[�h �I�[�v���G���[ ]");
		return false;
	} else {
		m_bIsOpen = true;
		LOG(em_MSG), "[ContecPioManager] �I�[�v������");
		KizuFunction::SetStatus(m_nKID, true, true);
	}


	//// �f�W�^���t�B���^
	//nRet = DioSetDigitalFilter(m_hDrv, 13);
	//if( 0 != nRet ) {
	//	return false;
	//}

	return true;
}

//------------------------------------------
// �N���[�Y
//------------------------------------------
bool ContecPioManager::Close()
{
	// Close process
	if( CONTEC_PIO_NOID != m_hDrv && m_bIsOpen ) {
		// �I������O�� �o�͂��Ă���M���� �S��OFF����
		memset(m_pDO, 0x00, sizeof(BYTE)*m_nPortNum);		
		Output();
		// Close
#ifndef DEFECTLIB_CONTEC_PIO_WMD		// �]����
		DWORD nRet = DioClose(m_hDrv);
#else									// WMD��
		long  nRet = DioExit(m_hDrv);
#endif
		if( 0 != nRet ) {
			LOG(em_ERR), "[ContecPioManager] �N���[�Y�G���[ [err_code = %d]", nRet);
			syslog(SYSNO_PIO_ERR, "[CLOSE err_code = %d]", nRet);
			return false;
		} else {
			m_hDrv = CONTEC_PIO_NOID;
		}
	}
	m_bIsOpen = false;
	return true;
}


//------------------------------------------
// �X���b�h
//------------------------------------------
UINT WINAPI ContecPioManager::MainThread(LPVOID param)
{
	int ii;
	bool bRefresh = false;

	////// ���C���C���X�^���X���o��
	ContecPioManager *p = (ContecPioManager *)param;
	p->SetTid();								// ���X���b�hID�Z�b�g

	//// �^�C�}�[�Z�b�g
	HANDLE evTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	__int64 ts = 0;	// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(evTimer, (LARGE_INTEGER *)&ts, mc_nReadInterval, NULL, NULL, FALSE);


/*
	// WaitForMultipleObjects�C�x���g
	HANDLE	hArray[] = {p->my_evStop,
						p->m_evOutputEvt,
						evTimer 
						};
*/
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
	pLOG(em_MSG), "[ContecPioManager] �X���b�h�J�n [0x%X]", p->GetTid());
	while (1) {
		DWORD ret = WaitForMultipleObjects(nWaitNum, hArray, FALSE, INFINITE);

		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// �X���b�h�I��
			pLOG(em_MSG), "[ContecPioManager] �X���b�h�I�� [0x%X]", p->GetTid());
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

			//// �X�e�[�^�X�̃A�N�Z�X�ׁ̈A����l�𔽉f
			memcpy(p->m_bufWk, p->m_pDI,	sizeof(BYTE)*p->m_nPortNum);	// �O��l
			memcpy(p->m_pDI,   p->m_bufInp, sizeof(BYTE)*p->m_nPortNum);



#ifdef DEFECTLIB_CONTEC_PIO_WMD			// WMD��
			//// ���荞�ݑΉ�
			if(p->m_bSmyu) {	
				for(int ii=0; ii<(int)p->m_nCheckCallBackNum; ii++ ) {
					int retc = p->PioNowStateCheck(p->m_CheckCallBackArray[ii].port, p->m_CheckCallBackArray[ii].addr, p->m_bufInp, p->m_bufWk);
					if( 1 == retc && p->m_CheckCallBackArray[ii].bMode )	SetEvent(p->m_CheckCallBackArray[ii].evSignOn  );
					if(-1 == retc && ! p->m_CheckCallBackArray[ii].bMode )	SetEvent(p->m_CheckCallBackArray[ii].evSignOn  );
				}
			}
#endif


			//// �M����Ԃ��`�F�b�N
			for( ii=0; ii<(int)p->m_nCheckNum; ii++ ) {
				//int retc = p->PioNowState(p->m_CheckArray[ii].port, p->m_CheckArray[ii].addr, p->m_bufInp, p->m_bufWk);
				int retc = p->PioNowStateCheck(p->m_CheckArray[ii].port, p->m_CheckArray[ii].addr, p->m_bufInp, p->m_bufWk);

				// �V�O�i���ʒm
				if( 1 == retc && NULL != p->m_CheckArray[ii].evSignOn  ) SetEvent(p->m_CheckArray[ii].evSignOn  );
				if(-1 == retc && NULL != p->m_CheckArray[ii].evSignOff ) SetEvent(p->m_CheckArray[ii].evSignOff );
			}
			
			////// �X�e�[�^�X�̃A�N�Z�X�ׁ̈A����l�𔽉f	���ԕύX
			//memcpy(p->m_pDI, p->m_bufInp, sizeof(BYTE)*p->m_nPortNum);


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

//------------------------------------------
// �f�W�^���f�[�^�̓���
//------------------------------------------
BOOL ContecPioManager::Input()
{
	//// �V�~�����[�^
	if(m_bSmyu) {	
		SmyuInput();
		return TRUE;
	}

	//// �I�����C��
	long nRet = 0;
#ifndef NO_HW
		if( ! Open(false) ) return FALSE;
		_ASSERT(CONTEC_PIO_NOID != m_hDrv);

		// �f�W�^���f�[�^�̓���
	#ifndef DEFECTLIB_CONTEC_PIO_WMD		// �]����
		nRet = DioInp(m_hDrv, &m_lpDInp);
	#else									// WMD��
		nRet = DioInpMultiByte(m_hDrv, m_inpPortNo, m_nPortNum, m_bufInp);
	#endif
#endif


	if( 0 != nRet ){
		if( m_bStatusDi ) {
			m_bStatusDi = FALSE;
			Close();			// ��x�ؒf

			SetEvent(m_evDiErr);
			LOG(em_ERR), "[ContecPioManager] DI�G���[ [err_code = %d]", nRet);
			syslog(SYSNO_PIO_ERR, "[DI err_code = %d]", nRet);
			KizuFunction::SetStatus(m_nKID, false, true, "[DI ���̓G���[ ]");
		}
		return FALSE;

	} else {
		if( !m_bStatusDi ) m_bStatusDi = TRUE;
	}
	return TRUE;
}
//------------------------------------------
// �f�W�^���f�[�^�̏o��
//------------------------------------------
BOOL ContecPioManager::Output()
{
	//// �o�̓o�b�t�@�ɃZ�b�g
	memcpy(m_bufOut, m_pDO, m_nPortNum);

	//// �V�~�����[�^
	if(m_bSmyu) {	
		SmyuOutput();
		return TRUE;
	}


	//// �I�����C��
	long  nRet = 0;
#ifndef NO_HW
		if( ! Open(false) ) return FALSE;
		_ASSERT(CONTEC_PIO_NOID != m_hDrv);

		// �f�W�^���f�[�^�̏o��
	#ifndef DEFECTLIB_CONTEC_PIO_WMD		// �]����
		nRet = DioOut(m_hDrv, &m_lpDOut);
	#else									// WMD��
		nRet = DioOutMultiByte(m_hDrv, m_outPortNo, m_nPortNum, m_bufOut);
	#endif
#endif

	if( 0 != nRet ){
		if( m_bStatusDo ) {
			m_bStatusDo = FALSE;
			Close();			// ��x�ؒf

			SetEvent(m_evDoErr);
			LOG(em_ERR), "[ContecPioManager] DO�G���[ [err_code = %d]", nRet);
			syslog(SYSNO_PIO_ERR, "[DO err_code = %d]", nRet);
			KizuFunction::SetStatus(m_nKID, false, true, "[DO �o�̓G���[ ]");
		}
		return FALSE;
	} else {
		if( !m_bStatusDo ) m_bStatusDo = true;
	}
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// ���荞��

#ifdef DEFECTLIB_CONTEC_PIO_WMD		// WMD��
//------------------------------------------
// ���荞�݃`�F�b�N��� �ݒ�
// DWORD port		�`�F�b�N����|�[�g�z��ʒu 
// BYTE addr		�`�F�b�N����r�b�g�ʒu (0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 �̂����ꂩ)
// HANDLE evSignOn	���荞�ݒʒm�p�V�O�i��
// bool bMode		���荞�� (true:0��1��  false:1��0��)
//------------------------------------------
void ContecPioManager::SetCheckCallBackArray(PIO_PORT port, PIO_BIT addr, HANDLE evSignOn, bool bMode)
{
	m_CheckCallBackArray[m_nCheckCallBackNum].addr		 = addr;
	m_CheckCallBackArray[m_nCheckCallBackNum].port		 = port;
	m_CheckCallBackArray[m_nCheckCallBackNum].evSignOn	 = evSignOn;
	m_CheckCallBackArray[m_nCheckCallBackNum].bMode		 = bMode;
	m_nCheckCallBackNum++;
}

//------------------------------------------
// ���荞�ݓ��͏��� (Alloc�ASetCheckCallBackArray��������)
//------------------------------------------
BOOL ContecPioManager::InitCallBackInp()
{
	//// ���荞�݂̏������s��
	long nRet = DioSetInterruptCallBackProc(m_hDrv, CallBackProc, this);
	if( 0 != nRet ){
		if( m_bStatusDi ) {
			m_bStatusDi = FALSE;

			SetEvent(m_evDiErr);
			LOG(em_ERR), "[ContecPioManager] DI���荞�ݏ������G���[ [err_code = %d]", nRet);
			syslog(SYSNO_PIO_ERR, "[DI���荞�ݏ����� err_code = %d]", nRet);
			KizuFunction::SetStatus(m_nKID, false, true, "[DI���荞�ݏ����� ���̓G���[ ]");
		}
		return FALSE;

	} else {
		if( !m_bStatusDi ) m_bStatusDi = TRUE;
	}

	//// ���荞�݊J�n
	short BitNo;
	short Kind;
	for (DWORD ii=0; ii<m_nCheckCallBackNum; ii++) {
		BitNo = m_CheckCallBackArray[ii].port * 8 + GetBitToIndex(m_CheckCallBackArray[ii].addr);
		Kind  = m_CheckCallBackArray[ii].bMode ? DIO_INT_RISE : DIO_INT_FALL;
		nRet  = DioNotifyInterrupt(m_hDrv, BitNo, Kind, 0);
		if (nRet != DIO_ERR_SUCCESS) {
			LOG(em_WAR), "[ContecPioManager] DI���荞�ݎw��G���[ [err_code = %d]", nRet);
			break;
		}
	}

	return TRUE;
}

//------------------------------------------
// ���荞�݃R�[���o�b�N
// short hDrv		�f�o�C�XID
// WPARAM wParam	���g�p
// LPARAM lParam	LOWORD(lParam)=���荞�݃r�b�g�ԍ�		HIWORD(lParam)=�����オ��F1�A����������F2
// void *Param		DioSetInterruptCallBackProc�֐��Ɏw�肵���p�����[�^
//------------------------------------------
void ContecPioManager::CallBackProc(short hDrv, WPARAM wParam, LPARAM lParam, void *Param)
{
	////// �������o��
	ContecPioManager *p = (ContecPioManager *)Param;
	WORD	BitNo		= LOWORD(lParam);				// ���荞�݃r�b�g�ԍ�
	short	Kind		= HIWORD(lParam);				// �����オ��:1, ����������:2
	int		port		= BitNo / 8;
	int		index		= (BitNo % 8);
	BYTE	addr		= GetIndexToBit(index);

	////// �V�O�i��
	DWORD ii;
	for(ii=0; ii<p->m_nCheckCallBackNum; ii++) {
		if( p->m_CheckCallBackArray[ii].port == port && p->m_CheckCallBackArray[ii].addr == addr &&
			((  p->m_CheckCallBackArray[ii].bMode && DIO_INT_RISE == Kind) || 
			 (! p->m_CheckCallBackArray[ii].bMode && DIO_INT_FALL == Kind)) ) {
			SetEvent(p->m_CheckCallBackArray[ii].evSignOn  );
			return;
		}
	}
	if(ii==p->m_nCheckCallBackNum) {
		pLOG(em_WAR), "[ContecPioManager] DI���荞�ݖ���`�R�[���o�b�N (port=%d addr=%d)", port, index);
	}
}
#endif
#endif