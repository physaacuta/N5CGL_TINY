#include "StdAfx.h"
#include "DataRecver.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// �R���X�g���N�^
// LPCSTR cPipeName �ڑ�����PIPE�p�X
// LPCSTR cSmemName ���L����������
//------------------------------------------
DataRecver::DataRecver(LPCSTR cPipeName, LPCSTR cSmemName ) :
m_cPipeName(cPipeName),
m_cSmemName(cSmemName),
m_hMap(NULL),
mtbl_pMap(NULL),
m_bConnect(false),
m_bRunExit(false),
mcls_pPipe(NULL),
m_bFirstInfoExists(false),
ThreadManager("DataRecver")
{
	//// ������
	InitializeCriticalSection(&m_csLock);						// �N���e�B�J���Z�N�V�����I�u�W�F�N�g��������

	memset(&m_ovlSend, 0x00, sizeof(OVERLAPPED));
	memset(&m_ovlRecv, 0x00, sizeof(OVERLAPPED));

	// �V�O�i������
	m_evOpenPipe  = CreateEvent(NULL, FALSE, FALSE, NULL);				// PIPE�̐ڑ��p�C�x���g
	m_evClosePipe = CreateEvent(NULL, FALSE, FALSE, NULL);				// PIPE�̐ؒf�p�C�x���g
	
	m_ovlSend.hEvent = CreateEvent(NULL, FALSE, TRUE, NULL);			// �񓯊����M�p (����́A�V�O�i����Ԃɂ��Ă���)
	m_ovlRecv.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);			// �蓮���Z�b�g
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
DataRecver::~DataRecver(void)
{
	//// �J��
	DeleteCriticalSection(&m_csLock);							// �N���e�B�J���Z�N�V�����I�u�W�F�N�g���J��

	CloseHandle(m_evOpenPipe);
	CloseHandle(m_evClosePipe);

	CloseHandle(m_ovlSend.hEvent);
	CloseHandle(m_ovlRecv.hEvent);
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int DataRecver::ThreadFirst()
{
	//// �C�x���g�ݒ�
	HANDLE hArray[] = {	
						m_evOpenPipe,
						m_evClosePipe,
						m_ovlRecv.hEvent
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int DataRecver::ThreadLast()
{
	// �p�C�v�N���X�̃C���X�^���X������ꍇ�́A�J��
	if(NULL != mcls_pPipe) {
		mcls_pPipe->Cancel();										// ��������I/O��������
		SAFE_DELETE(mcls_pPipe);									// �C���X�^���X�J��
		mem_close(&m_hMap, (LPVOID *)&mtbl_pMap);					// ���L������
	}
	m_bConnect = false;
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void DataRecver::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	
			EV_OPEN = 0,					// �I�[�v���v��
			EV_CLOSE,						// �N���[�Y�v��
			EV_RECV_END,					// ��M�����ʒm
			EV_END
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_OPEN:												// �I�[�v���v��
		if(NULL != mcls_pPipe) break;

		// ���L������ �I�[�v��
		mem_connect(&m_hMap, (LPVOID *)&mtbl_pMap, m_cSmemName);
		if(m_hMap == NULL) {
			LOG(em_ERR), "[%s] �t���[�����󂯓n���e�[�u�� �A�N�Z�X���s", my_sThreadName);			
			syslog(SYSNO_MEM_OPEN, "�t���[�����󂯓n���e�[�u�� �A�N�Z�X���s");
		
			//m_emErrCode 		= CPIPE_ERR_SMEM;
			//m_nDetailErrCode	= 0;
			//SetEvent(m_evOnError);							// �O���֒ʒm
			CPIPE_ERROR * pCPipeErr = new CPIPE_ERROR;
			memset(pCPipeErr, 0x00, sizeof(CPIPE_ERROR));
			pCPipeErr->nErrCode		= CPIPE_ERR_SMEM;
			pCPipeErr->nDetailCode	= 0;
			if( ! gque_CPipeErr.AddToTailFreeCheck( pCPipeErr ) ) {
				LOG(em_ERR), "[%s] ��n���L���[�t�� (ð��ٱ������s)", my_sThreadName);
				delete pCPipeErr;
			}
			break;
		}
	
		{
			FIRST_SHMEM_INFOv1* pSmemH = (FIRST_SHMEM_INFOv1*)mtbl_pMap;	// ���L�������̐擪�A�h���X = �J�������̈ʒu
			m_bFirstInfoExists = (::IsEqualGUID(ID_FIRST_SHMEM_INFO_IDv1, pSmemH->first_smem_req_id) ? true : false);
		}

		// �p�C�v �C���X�^���X����
		int nCnt;
		for(nCnt=0; nCnt<PIPE_OPEN_RETRY; nCnt++) {
			try {
				mcls_pPipe = new PipeManager(m_cPipeName);
				break;			
			} catch(int code) {
				LOG(em_WAR), "[%s] PIPE �C���X�^���X�������s [%d]", my_sThreadName, code);
				mcls_pPipe = NULL;
			}
			Sleep(PIPE_OPEN_INTERVAL);
		}
		if(PIPE_OPEN_RETRY == nCnt) {
			LOG(em_ERR), "[%s] PIPE �C���X�^���X�������s", my_sThreadName);
			//m_emErrCode 		= CPIPE_OPEN_ERR;
			//m_nDetailErrCode	= NULL==mcls_pPipe ? 0 : mcls_pPipe->GetError();		// 0;
			//SetEvent(m_evOnError);							// �O���֒ʒm
			CPIPE_ERROR * pCPipeErr = new CPIPE_ERROR;
			memset(pCPipeErr, 0x00, sizeof(CPIPE_ERROR));
			pCPipeErr->nErrCode		= CPIPE_OPEN_ERR;
			pCPipeErr->nDetailCode	= NULL==mcls_pPipe ? 0 : mcls_pPipe->GetError();
			if( ! gque_CPipeErr.AddToTailFreeCheck( pCPipeErr ) ) {
				LOG(em_ERR), "[%s] ��n���L���[�t�� (����)", my_sThreadName);
				delete pCPipeErr;
			}
			break;
		}


		// �p�C�v�ǂݍ���
		if( !mcls_pPipe->Recv(m_cRecvBuf, PIPE_RECVBUF_SIZE, &m_ovlRecv) ) {
			LOG(em_ERR), "[%s] PIPE �ǂݍ��ݎ��s", my_sThreadName);

			//m_emErrCode			= CPIPE_READ_ERR;
			//m_nDetailErrCode	= mcls_pPipe->GetError();
			//SetEvent(m_evOnError);
			CPIPE_ERROR * pCPipeErr = new CPIPE_ERROR;
			memset(pCPipeErr, 0x00, sizeof(CPIPE_ERROR));
			pCPipeErr->nErrCode		= CPIPE_READ_ERR;
			pCPipeErr->nDetailCode	= mcls_pPipe->GetError();
			if( ! gque_CPipeErr.AddToTailFreeCheck( pCPipeErr ) ) {
				LOG(em_ERR), "[%s] ��n���L���[�t�� (�Ǎ�)", my_sThreadName);
				delete pCPipeErr;
			}
		}
		// �L���[���Z�b�g
		m_bConnect = true;
		gque_Deli.ReSetMaxCount(); 
		LOG(em_MSG), "[%s] PIPE �ڑ�����", my_sThreadName);
		break;



//-----------------------------------------------------------------------------------------------
	case EV_CLOSE:												// �N���[�Y�v��
		// �p�C�v�N���X�̃C���X�^���X������ꍇ�́A�J��
		if(NULL != mcls_pPipe) {
			// �p�C�v
			mcls_pPipe->Cancel();									// ��������I/O��������
			SAFE_DELETE(mcls_pPipe);									// �C���X�^���X�J��
			// ���L������
			mem_close(&m_hMap, (LPVOID *)&mtbl_pMap);

			m_bConnect = false;
			LOG(em_MSG), "[%s] PIPE �ؒf����", my_sThreadName);
		}
		break;



//-----------------------------------------------------------------------------------------------
	case EV_RECV_END:											// ��M�����ʒm
		// �񓯊����ʖ₢���킹 (��M�҂���ԂŁAI/O�L�����Z�������R�[�������)
		bool bRetc;
		if( mcls_pPipe != NULL )	bRetc = mcls_pPipe->GetResult(&m_ovlRecv);
		else						bRetc = false;	
		ResetEvent(m_ovlRecv.hEvent);										// �V�O�i���}�j���A�����Z�b�g

		// �f�[�^�擾��̏���
		if( bRetc ) {

			// �L���[�ɒǉ�
			BYTE * pNewData = new BYTE[PIPE_RECVBUF_SIZE];
			memset(pNewData, 0x00, PIPE_RECVBUF_SIZE);
			memcpy(pNewData, m_cRecvBuf, PIPE_RECVBUF_SIZE);

			// �X���b�h�L���[�ɓo�^
			if( gque_Deli.GetFreeCount() > 0 ) {
				gque_Deli.AddToTail(pNewData);		

			} else {												// �󂫖���
				LOG(em_WAR), "[%s] �L���[�t��!!", my_sThreadName);
			}
			

			// �p�C�v�ǂݍ���
			if( !mcls_pPipe->Recv(m_cRecvBuf, PIPE_RECVBUF_SIZE, &m_ovlRecv) ) {
				//m_emErrCode			= CPIPE_READ_ERR;
				//m_nDetailErrCode	= mcls_pPipe->GetError();
				//SetEvent(m_evOnError);
				CPIPE_ERROR * pCPipeErr = new CPIPE_ERROR;
				memset(pCPipeErr, 0x00, sizeof(CPIPE_ERROR));
				pCPipeErr->nErrCode		= CPIPE_READ_ERR;
				pCPipeErr->nDetailCode	= mcls_pPipe->GetError();
				if( ! gque_CPipeErr.AddToTailFreeCheck( pCPipeErr ) ) {
					LOG(em_ERR), "[%s] ��n���L���[�t�� (�Ǎ�)", my_sThreadName);
					delete pCPipeErr;
				}
			}

		} else {

			// �I���������͗�O
			if( m_bRunExit ) {
				m_bRunExit = false;
				break; 
			}
			// �R�}���h�p�C�v�p�C�v�ؒf
			//m_emErrCode				= CPIPE_DISCONNECT_ERR;
			//m_nDetailErrCode		= 0; //cls_pPipe->GetError();
			//SetEvent(m_evOnError);
			CPIPE_ERROR * pCPipeErr = new CPIPE_ERROR;
			memset(pCPipeErr, 0x00, sizeof(CPIPE_ERROR));
			pCPipeErr->nErrCode		= CPIPE_DISCONNECT_ERR;
			pCPipeErr->nDetailCode	= 0;
			if( ! gque_CPipeErr.AddToTailFreeCheck( pCPipeErr ) ) {
				LOG(em_ERR), "[%s] ��n���L���[�t�� (����ɐؒf)", my_sThreadName);
				delete pCPipeErr;
			}
		}
		break;


//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}


//------------------------------------------
// �I�t�Z�b�g�̈�̕ԋp�֐�
// DWORD ofs �ԋp����I�t�Z�b�g�ʒu
//------------------------------------------
void DataRecver::SendBack(DWORD ofs)
{
	static DWORD nWk;			// �񓯊����M�p�ɐÓI�ɃG���A�m��


	// �����X���b�h����̕ԋp�ɔ����Ĕr��
	AutoLock  lock(&m_csLock);

//	LOG(em_INF), "SendBack(%d)", ofs);
	// �O��̑��M���������Ă��Ȃ��ꍇ�A������Ƒ҂�
	DWORD nWaitRet = WaitForSingleObject(m_ovlSend.hEvent, 1000);
	if ( WAIT_TIMEOUT == nWaitRet) {
		LOG(em_WAR), "[%s] �I�t�Z�b�g�񓯊����M �O��l������", my_sThreadName);
	}

	// �p�C�v�ɔ񓯊���������
	nWk= ofs;
	if( mcls_pPipe->Send(&nWk, sizeof(DWORD), &m_ovlSend) ) {
		if (ERROR_IO_PENDING == GetLastError()) {
			mcls_pPipe->GetResult(&m_ovlSend);
		}
	}
}

//------------------------------------------
// ���L���������̃J�������L����Ԃ�
//------------------------------------------
bool DataRecver::IsFirstInfoExists()
{
	FIRST_SHMEM_INFOv1* pSmemH = (FIRST_SHMEM_INFOv1*)mtbl_pMap;	// ���L�������̐擪�A�h���X = �J�������̈ʒu
	bool bFirstInfoExists = (::IsEqualGUID(ID_FIRST_SHMEM_INFO_IDv1, pSmemH->first_smem_req_id) ? true : false);

	if (bFirstInfoExists != m_bFirstInfoExists)
	{
		m_bFirstInfoExists = bFirstInfoExists;
		LOG(em_MSG), "[%s] �J�������擾�F���L���������ɃJ�������%s", my_sThreadName, (m_bFirstInfoExists ? "�݂�" : "����"));
	}

	return m_bFirstInfoExists;
}

//------------------------------------------
// ���L���������̃J��������Ԃ�
//------------------------------------------
void DataRecver::HelcyStat(ENG_STAT *st)
{
	FIRST_SHMEM_INFOv1* pSmemH = (FIRST_SHMEM_INFOv1*)mtbl_pMap;	// ���L�������̐擪�A�h���X = �J�������̈ʒu
	st->camnum = pSmemH->camnum;

	if (0 > st->camnum || NCL_ncam_max <= st->camnum) return;

	for (int ii = 0; ii < (int)st->camnum; ii++)
	{
		st->camid[ii] = pSmemH->camid[ii];

		for (int jj = 0; jj < (MAX_CAMANGLE - 1); jj++)
		{
			st->exptime[jj][ii] = (float)pSmemH->exptime[ii][jj];
			st->gain[jj][ii] = pSmemH->gain[ii][jj];
		}
		st->temp[ii] = pSmemH->temp[ii];
		st->volt[ii] = pSmemH->volt[ii];
	}
}
