#include "StdAfx.h"
#include "CommandSender.h"


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
//------------------------------------------
CommandSender::CommandSender(LPCSTR cPipeName) :
m_cPipeName(cPipeName),
m_bConnect(false),
m_nSendSize(0),
mcls_pPipe(NULL),
m_bRunExit(false),
m_bNgCam(false),
ThreadManager("CmdSender")
{
	// ������
	memset( m_cRecvWk,  0x00, sizeof(m_cRecvWk) );
	memset( m_cRecvBuf, 0x00, sizeof(m_cRecvBuf) );
	memset( m_cSendBuf, 0x00, sizeof(m_cSendBuf) );

	// �V�O�i������
	m_evOpenPipe  = CreateEvent(NULL, FALSE, FALSE, NULL);				// PIPE�̐ڑ��p�C�x���g
	m_evClosePipe = CreateEvent(NULL, FALSE, FALSE, NULL);				// PIPE�̐ؒf�p�C�x���g
	m_evSendStart = CreateEvent(NULL, FALSE, FALSE, NULL);				// ��M�J�n�C�x���g�F���p
	m_evSendEnd	  = CreateEvent(NULL, FALSE, FALSE, NULL);				// ���M�����C���x�g�F���p
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
CommandSender::~CommandSender(void)
{
	CloseHandle(m_evOpenPipe);
	CloseHandle(m_evClosePipe);
	CloseHandle(m_evSendStart);
	CloseHandle(m_evSendEnd);
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int CommandSender::ThreadFirst()
{
	//// ���O����
	memset(&m_ovlRecv, 0x00, sizeof(OVERLAPPED));
	m_ovlRecv.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);		// �蓮���Z�b�g

	//// �C�x���g�ݒ�
	HANDLE hArray[] = { 
						m_evOpenPipe,
						m_evClosePipe,
						m_evSendStart,
						m_ovlRecv.hEvent
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int CommandSender::ThreadLast()
{
	// �p�C�v�N���X�̃C���X�^���X������ꍇ�́A�J��
	if(NULL != mcls_pPipe) {
		mcls_pPipe->Cancel();										// ��������I/O��������
		SAFE_DELETE(mcls_pPipe);									// �C���X�^���X�J��
	}
	m_bConnect = false;

	// �n���h���̉��
	CloseHandle(m_ovlRecv.hEvent);
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void CommandSender::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	
			EV_OPEN = 0,					// �I�[�v���v��
			EV_CLOSE,						// �N���[�Y�v��
			EV_SEND_START,					// ���M�J�n�v��
			EV_RECV_END,					// ��M�����ʒm
			EV_END
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_OPEN:												// �I�[�v���v��
		if(NULL != mcls_pPipe) break;
		
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
			//m_nDetailErrCode	= NULL==mcls_pPipe ? 0 : mcls_pPipe->GetError();
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
		
		// ������
		ResetEvent(m_evSendStart);
		ResetEvent(m_evSendEnd);


		// �p�C�v�ǂݍ���
		if( !mcls_pPipe->Recv(m_cRecvWk, PIPE_RECVBUF_SIZE, &m_ovlRecv) ) {
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
		m_bConnect = true;
		LOG(em_MSG), "[%s] PIPE �ڑ�����", my_sThreadName);
		break;


//-----------------------------------------------------------------------------------------------
	case EV_CLOSE:												// �N���[�Y�v��
		// �p�C�v�N���X�̃C���X�^���X������ꍇ�́A�J��
		if(NULL != mcls_pPipe) {
			mcls_pPipe->Cancel();										// ��������I/O��������
			SAFE_DELETE(mcls_pPipe);									// �C���X�^���X�J��
			m_bConnect = false;
			LOG(em_MSG), "[%s] PIPE �ؒf����", my_sThreadName);
		}
		break;

		
//-----------------------------------------------------------------------------------------------
	case EV_SEND_START:											// ���M�J�n�v��
		if(NULL != mcls_pPipe) {
			// �����ł́A���ɃG���[�����͂��Ȃ���B�����Ŏ��s������A�A���T�[�^�C���A�E�g����Δ������邩��
//LOG(em_WAR), "[���M] size=%d, cmd=%d, %02X, %02X, %02X, %02X", m_nSendSize, ((int*)m_cSendBuf)[0], m_cSendBuf[4], m_cSendBuf[5], m_cSendBuf[6], m_cSendBuf[7]);
			mcls_pPipe->Send(m_cSendBuf, m_nSendSize);					// �������M�Ƃ���B
		}
		break;


//-----------------------------------------------------------------------------------------------
	case EV_RECV_END:											// ��M�����ʒm
		// �񓯊����ʖ₢���킹 (��M�҂���ԂŁAI/O�L�����Z�������R�[�������)
		bool bRetc;
		if( mcls_pPipe != NULL )	bRetc = mcls_pPipe->GetResult(&m_ovlRecv);
		else 						bRetc = false;	
		ResetEvent(m_ovlRecv.hEvent);									// �V�O�i���}�j���A�����Z�b�g

		// �f�[�^�擾��̏���
		if( bRetc ) {
			// ��M�f�[�^�̍ŏ���4�o�C�g���擾? ���^�[���R�[�h�������Ă���
			int retcode = *((int *)m_cRecvWk);
//LOG(em_WAR), "[��M] %d, %02X, %02X, %02X, %02X", retcode, m_cRecvBuf[0], m_cRecvBuf[1], m_cRecvBuf[2], m_cRecvBuf[3]);

			if ( 0 <= retcode ) {										// ���̒l�́A�񓯊��G���[
				memcpy(m_cRecvBuf, m_cRecvWk, sizeof(m_cRecvBuf));			// ���̎���M�ŁA�㏑�������̂�h�~ 
				SetEvent(m_evSendEnd);										// �R�}���h��M�����ɂ��A�O���ɃC�x���g�ʒm

			} else {
				//m_emErrCode			= CPIPE_ERR_COMMAND;
				//m_nDetailErrCode	= retcode;
				//SetEvent(m_evOnError);

				//-------------------------------------------
				// �O���֒ʒm (�񓯊��֘A)
				CPIPE_ERROR * pCPipeErr = new CPIPE_ERROR;
				memset(pCPipeErr, 0x00, sizeof(CPIPE_ERROR));
				pCPipeErr->nErrCode		= CPIPE_ERR_COMMAND;
				pCPipeErr->nDetailCode	= retcode;

				//-------------------------------------------
				// ���ꏈ���� ���O���ɏ��S���󂯓n��
				//if(ERR_EVENT_TOP_DETECTED == retcode) {
				//	pCPipeErr->pData = new BYTE[PIPE_RECVBUF_SIZE];
				//	memcpy(pCPipeErr->pData, m_cRecvWk, sizeof(m_cRecvBuf));
				//} else if(ERR_EVENT_BOT_DETECTED == retcode) {
				//	pCPipeErr->pData = new BYTE[PIPE_RECVBUF_SIZE];
				//	memcpy(pCPipeErr->pData, m_cRecvWk, sizeof(m_cRecvBuf));
				//} 


				// �˗�
				if( ! gque_CPipeErr.AddToTailFreeCheck( pCPipeErr ) ) {
					LOG(em_ERR), "[%s] ��n���L���[�t�� (��M���� �d��ُ�)", my_sThreadName);
					delete pCPipeErr;
				}
			}

			// �p�C�v�ǂݍ���
			if( !mcls_pPipe->Recv(m_cRecvWk, PIPE_RECVBUF_SIZE, &m_ovlRecv) ) {
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
			LOG(em_ERR), "[%s] PIPE ������ɐؒf���ꂽ", my_sThreadName);
			//m_emErrCode				= CPIPE_DISCONNECT_ERR;
			//m_nDetailErrCode		= mcls_pPipe->GetError();	// 0;
			//SetEvent(m_evOnError);
			CPIPE_ERROR * pCPipeErr = new CPIPE_ERROR;
			memset(pCPipeErr, 0x00, sizeof(CPIPE_ERROR));
			pCPipeErr->nErrCode		= CPIPE_DISCONNECT_ERR;
			pCPipeErr->nDetailCode	= mcls_pPipe->GetError();
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
// �R�}���h��t
// BYTE *pSendData ���M�f�[�^�Z�b�g
// DWORD nSendSize ���M�T�C�Y�Z�b�g
// void *pRecvRetc  ��M�f�[�^�̐擪*byte�̒l (�ʏ�4byte��int�̒l���Z�b�g�����)
// int nRecvSize   ��M�f�[�^����̎�荞�݃T�C�Y (�ʏ�4byte)
// DWORD nWaitTime �ő�҂�����
// �߂�l ���A��� (0:����A���T�[ 1:�ُ�A���T�[ -1:�A���T�[���� -9:�p�C�v����)
//------------------------------------------
int CommandSender::SendCommand(BYTE *pSendData, DWORD nSendSize, void *pRecvRetc, int nRecvSize, DWORD nWaitTime)
{
	int nSendCode = *((int*)pSendData);
	if( m_bNgCam ) return 0;	// �ُ�J�����Ȃ�S������
	if( ! m_bConnect ) {
		LOG(em_ERR), "[%s] �R�}���h�p�C�v �ؒf�� [���M�R�}���h=%d]", my_sThreadName, nSendCode);
		return -9;
	}

	// ���M���Z�b�g
	memcpy(m_cSendBuf, pSendData, nSendSize);
	m_nSendSize = nSendSize;

	// ���M�v��
	ResetEvent(m_evSendEnd);	// ���ʂ��肦�Ȃ��͂������ǁA�O�̃V�O�i�����c����ςȂ��̂Ƃ�
	SetEvent(m_evSendStart);
	
	// ���M�������� �A���T�[���A���ė���܂ő҂�
	DWORD ret = WaitForSingleObject(m_evSendEnd, nWaitTime);
	if (WAIT_TIMEOUT == ret) {
		LOG(em_ERR), "[%s] �R�}���h�A���T�[ �������� [���M�R�}���h=%d]", my_sThreadName, nSendCode);

#ifdef ENG_TEST
		return 0;
#endif

		return -1;									// �R�}���h��������
	}

	// ��M�f�[�^�̐擪*�o�C�g���R�s�[
	memcpy(pRecvRetc, m_cRecvBuf, nRecvSize);

	// ���ʍ\���̂ɕϊ��H
	NCL_ANSWER* pWk = (NCL_ANSWER*)pRecvRetc;

	if( NCL_SUCCESS == pWk->retcode ) {
//		LOG(em_MSG), "[%s] �R�}���h�A���T�[ �����L�� [���M�R�}���h=%d]", my_sThreadName, *((int*)pSendData));
		return 0;
	} else {

		LOG(em_ERR), "[%s] �R�}���h�A���T�[�ُ� [���M�R�}���h=%d �A���T�[=%d]", my_sThreadName, nSendCode, pWk->retcode);
		return 1;
	}
}

//------------------------------------------
// �c��̃f�[�^�Ǎ� (m_cRecvBuf�T�C�Y�ȏ�̃f�[�^�ɂ͂����������Ή�)
// int nOffset				�O��܂ł̎�荞�݃T�C�Y
// void* pRecvRetc	��M�f�[�^�擪�|�C���^
// int nRecvSize			�c��Ǎ��T�C�Y
//------------------------------------------
void CommandSender::ReadPerData(int nOffset, void* pRecvRetc, int nRecvSize )
{
	// ��M�f�[�^�̐擪*�o�C�g���R�s�[
	memcpy(&((BYTE*)pRecvRetc)[nOffset], &m_cRecvBuf[nOffset], nRecvSize);
}