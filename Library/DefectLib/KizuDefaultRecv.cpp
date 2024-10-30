#include "StdAfx.h"
#include "KizuDefaultRecv.h"

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// �R���X�g���N�^
// const char *cSession ini�t�@�C���Z�b�V����
// ThreadQueue<SOCK_BASE_HEAD>* que_pRecv ��M�f�[�^�\���̎󂯓n���L���[
//------------------------------------------
KizuDefaultRecv::KizuDefaultRecv(const char *cSession, ThreadQueue<SOCK_BASE_HEAD>* que_pRecv) :
m_cSession(cSession),
mque_pRecv(que_pRecv),
my_emSockState(STATE_NONE),
my_nRecvingDataSize(0),
my_pRecvData(NULL),
mcls_pSock(NULL),
mcls_pLog(NULL)
{
	//// �V�O�i������
	my_evReTryConnect	= CreateWaitableTimer(NULL, FALSE, NULL);
	my_evHelcy			= CreateWaitableTimer(NULL, FALSE, NULL);
	my_evDisConnect		= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evRecvEnd		= CreateEvent(NULL, FALSE, FALSE, NULL);
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
KizuDefaultRecv::~KizuDefaultRecv(void)
{
	//// �J��
	SAFE_DELETE_ARRAY(my_pRecvData);
	SAFE_DELETE(mcls_pSock);				// �\�P�b�g�N���X �J��
	// �V�O�i���J��
	CloseHandle(my_evReTryConnect);
	CloseHandle(my_evHelcy);
	CloseHandle(my_evDisConnect);
	CloseHandle(my_evRecvEnd);
}

//------------------------------------------
// �w��̃X�e�[�^�X����
// SOCK_STATE st �X�e�[�^�X
//------------------------------------------
CString KizuDefaultRecv::GetSockStateName(SOCK_STATE st) const
{
	CString wk;
	if		(st == STATE_NONE)		{ wk = "������� (�ؒf��)"; }
	else if (st == STATE_IDLE)		{ wk = "�������� (�ڑ���)"; }
	else if (st == STATE_HEAD)		{ wk = "�w�b�_�� ��M��";}
	else if (st == STATE_DATA)		{ wk = "�f�[�^�� ��M��";}
	else							{ wk = "�X�e�[�^�X�ُ�";}
	return wk;
}

//------------------------------------------
// �X���b�h
//------------------------------------------
UINT WINAPI KizuDefaultRecv::MainThread(LPVOID param)
{
	////// ���C���C���X�^���X���o��
	KizuDefaultRecv *p = (KizuDefaultRecv *)param;
	p->SetTid();								// ���X���b�hID�Z�b�g

	// WaitForMultipleObjects�C�x���g
	HANDLE	hArray[] = {p->my_evStop,
						p->my_evDisConnect, 
						p->my_evReTryConnect,
						p->my_evHelcy,
						p->my_evRecvEnd  
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// �X���b�h�I��
			EV_DISCONNECT,					// �ؒf�C�x���g
			EV_RETRY,						// �Đڑ��v���^�C�}�C�x���g
			EV_HELCY_TIMEOUT,				// �w���V�[�`���^�C���A�E�g
			EV_RECVEND,						// ��M�����C�x���g
			EV_END	};

	//// �����ڑ�
	p->Connect(true);

	// ���b�Z�[�W�L���[�ǂݍ���
	pLOG(em_MSG), "[KizuDefaultRecv] <%s> �X���b�h�J�n [0x%X]", p->m_cSession, p->GetTid());
	while (1) {
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);

		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// �X���b�h�I��
			pLOG(em_MSG), "[KizuDefaultRecv] <%s> �X���b�h�I�� [0x%X]", p->m_cSession, p->GetTid());
			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_DISCONNECT:											// �ؒf�C�x���g
			p->Close();													// �ؒf
			break;

//-----------------------------------------------------------------------------------------------
		case EV_RETRY:												// �Đڑ��v���^�C�}�C�x���g
			p->Connect(false);											// �����ڑ�
			break;

//-----------------------------------------------------------------------------------------------
		case EV_HELCY_TIMEOUT:										// �w���V�[�`���^�C���A�E�g
			// �^�C���A�E�g�ɂ��ؒf
			pLOG(em_ERR), "[KizuDefaultRecv] <%s> �\�P�b�g �w���V�[�`���^�C���A�E�g", p->m_cSession);
			syslog(SYSNO_SOCK_HELCY_TIMEOUT, "[%s]", p->m_cSession);

			p->Close();	
			break;

//-----------------------------------------------------------------------------------------------
		case EV_RECVEND:											// ��M�����C�x���g
			switch( p->my_emSockState) {
				case STATE_HEAD :										// �w�b�_����M����

					//// �g�����U�N�V�����R�[�h�ɂ�鏈���킯
					if( 0 == memcmp( p->my_RecvHead.code, T_CODE_HELCY, SIZE_SOCK_TC ) ) {	
						//pLOG(em_INF), "[KizuDefaultRecv] �w���V�[�`����M");
						// �w���V�[�Ď����Z�b�g
						p->Recv_Helcy();

						// ����M�J�n
						if( ! p->Recv_Head() ) p->Close();

					} else {
						// �f�[�^����M�J�n
						if( ! p->Recv_Data() ) p->Close();
					}
					break;

				case STATE_DATA :										// �f�[�^����M����		

					//pLOG(em_MSG), "%d[byte]/%d[byte]��M����", p->my_nRecvingDataSize, p->my_RecvHead.len);

					//// �_������P�ʂŃz���g�Ƀf�[�^��S����M�ł����H (�������M�Ή�)
					if(p->my_RecvHead.len > p->my_nRecvingDataSize+sizeof(p->my_RecvHead)) {
						// �܂��f�[�^���̎�M�r��
						if( !p->Recv_Data() ) p->Close();
						break;
					} 

					
					// �f�[�^�R�s�[
					BYTE *pRecvBuf;											// �L���[�o�^�p�o�b�t�@
					pRecvBuf = new BYTE[p->my_RecvHead.len];					// �L���[�̎��o����ŊJ�������
					memcpy( pRecvBuf, &p->my_RecvHead, sizeof(p->my_RecvHead));
					memcpy( &pRecvBuf[sizeof(p->my_RecvHead)], p->my_pRecvData, p->my_RecvHead.len - sizeof(p->my_RecvHead) );
					
					// �L���[�ɓo�^
					if( p->mque_pRecv->GetFreeCount() > 0 ) {
						p->mque_pRecv->AddToTail( (SOCK_BASE_HEAD*)pRecvBuf );			

					} else {												// �󂫖���
						pLOG(em_WAR), "[KizuDefaultRecv] <%s> �L���[�t�� ��M�T�C�Y[%dbyte]", p->m_cSession, p->my_RecvHead.len);
					
						// ��������C���X�^���X�j��
						SAFE_DELETE_ARRAY(pRecvBuf); 
					}
					
					// ��M�o�b�t�@�J��
					SAFE_DELETE_ARRAY(p->my_pRecvData);
	
					// ����M�J�n
					if( ! p->Recv_Head() ) p->Close();
					break;

				default :
					pLOG(em_MSG), "[KizuDefaultRecv] <%s> ��M�����C�x���g���\�P�b�g���[�h�ُ� [%d]", p->m_cSession, p->my_emSockState);
					break;
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

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �V�[�P���X����

//------------------------------------------
// �w�b�_����M�J�n
//------------------------------------------
bool KizuDefaultRecv::Recv_Head()
{
	if(NULL == mcls_pSock) return false;

	memset(&my_RecvHead, 0x00, sizeof(my_RecvHead));
	my_nRecvingDataSize = 0;

	// ��M�����C�x���g�ʒm�^�C�v�̎�M�J�n (�N���C�A���g��0�Œ�)
	int retc = mcls_pSock->Recv( 0, &my_RecvHead, sizeof(my_RecvHead), my_evRecvEnd);
	if( 0 == retc ) {							// ���튮��
		my_emSockState = STATE_HEAD;						// �\�P�b�g��� �w�b�_����M��
		return true;
	} else {
		LOG(em_ERR), "[KizuDefaultRecv] <%s> �\�P�b�g Recv_Head���s err_code=%d", m_cSession, retc);
		syslog(SYSNO_SOCK_RECV_ERR, "[%s,�w�b�_�� err_code=%d]", m_cSession, retc);
		return false;
	}
}

//------------------------------------------
// �f�[�^����M�J�n
//------------------------------------------
bool KizuDefaultRecv::Recv_Data()
{
	if(NULL == mcls_pSock) return false;
/*
	// �f�[�^����M�o�b�t�@����
	int size = my_RecvHead.len - sizeof(my_RecvHead);
	my_pRecvData = new BYTE[size];

	// ��M�����C�x���g�ʒm�^�C�v�̎�M�J�n (�N���C�A���g��0�Œ�)
	int retc = mcls_pSock->Recv( 0, my_pRecvData, size, my_evRecvEnd);
*/

	// �f�[�^����M�o�b�t�@����
	DWORD size = my_RecvHead.len - sizeof(my_RecvHead) - my_nRecvingDataSize;
	if( 0 == my_nRecvingDataSize ) my_pRecvData = new BYTE[size];
	if(size > SIZE_SOCK_DATA_MAX) size = SIZE_SOCK_DATA_MAX;	// 1��̎�M�T�C�Y�I�[�o�[

	// ��M�����C�x���g�ʒm�^�C�v�̎�M�J�n (�N���C�A���g��0�Œ�)
	int retc = mcls_pSock->Recv( 0, &my_pRecvData[my_nRecvingDataSize], size, my_evRecvEnd);

	if( 0 == retc ) {							// ���튮��
		my_emSockState = STATE_DATA;						// �\�P�b�g��� �f�[�^����M��
		my_nRecvingDataSize += size;
		return true;
	} else {
		LOG(em_ERR), "[KizuDefaultRecv] <%s> �\�P�b�g Recv_Data���s err_code=%d", m_cSession, retc);
		syslog(SYSNO_SOCK_RECV_ERR, "[%s,�f�[�^�� err_code=%d]", m_cSession, retc);
		return false;
	}
}

//------------------------------------------
// �w���V�[�`����M���� (�w�b�_�[���̂�)
//------------------------------------------
void KizuDefaultRecv::Recv_Helcy()
{
	// ��x�L�����Z��
	CancelWaitableTimer(my_evHelcy);

	// �w���V�[�`���^�C���[�Ď��J�n
	__int64 ts = (__int64)PARAM_TIMEOUT_HELCY_RECV * -10000;	// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(my_evHelcy, (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TPC/IP �ʐM����

//------------------------------------------
// �ڑ�
// bool bLogDsp ���O�o�͗L�� (false:���s���O�o���Ȃ�)
//------------------------------------------
void KizuDefaultRecv::Connect(bool bLogDsp)
{
	int retc;

	// �\�P�b�g�C���X�^���X����
	try {
		mcls_pSock = new SockBase( m_cSession, TASKINI_NAME, GetTid(), 0, 1, PARAM_SOCK_RECV_BUFSIZE, 0);
	} catch(int code) {
		mcls_pSock = NULL;
		if(bLogDsp) {
			LOG(em_ERR), "[KizuDefaultRecv] <%s> �\�P�b�g New���s err_code=%d", m_cSession, code);
			syslog(SYSNO_SOCK_NEW_ERR, "[%s: err_code=%d]", m_cSession, code);
		}
	}

	// �R�l�N�g
	if( NULL != mcls_pSock ) {
		retc = mcls_pSock->Connect();
		if( 0 != retc ) {
			delete mcls_pSock;
			mcls_pSock = NULL;
			if(bLogDsp) {
				LOG(em_ERR), "[KizuDefaultRecv] <%s> �\�P�b�g Connect���s err_code=%d", m_cSession, retc);
				syslog(SYSNO_SOCK_CONNECT_ERR, "[%s: err_code=%d]", m_cSession, retc);
			}
		}
	}

	// �㏈��
	if( NULL != mcls_pSock ) {								// ����
		mcls_pSock->SetEvDisConnect(my_evDisConnect);			// �ؒf�F�����C�x���g�ɂ���
		my_emSockState = STATE_IDLE;							// �\�P�b�g��� �ҋ@
		
		// �w�b�_�[����M�J�n
		if( Recv_Head() ) {

			// �w���V�[�`���Ď��J�n
			Recv_Helcy();
		
			// �\�P�b�g�ڑ�����
			LOG(em_MSG), "[KizuDefaultRecv] <%s> �\�P�b�g Connect����", m_cSession);
			syslog(SYSNO_SOCK_CONNECT, "[%s]", m_cSession);

		} else {
			Close();												// �ؒf & ���g���C�ݒ�
		}

	} else {												// �\�P�b�g�������s
		Close();												// �ؒf & ���g���C�ݒ�
	}
}

//------------------------------------------
// �ؒf & ���g���C�ݒ�
//------------------------------------------
void KizuDefaultRecv::Close()
{
	// �\�P�b�g�ؒf
	if(NULL != mcls_pSock) {
		SAFE_DELETE(mcls_pSock); 
		LOG(em_ERR), "[KizuDefaultRecv] <%s> �\�P�b�g �ؒf", m_cSession);
		syslog(SYSNO_SOCK_CLOSE, "[%s]", m_cSession);
	}

	// ������
	my_emSockState = STATE_NONE;				// �\�P�b�g��� ������
	SAFE_DELETE_ARRAY( my_pRecvData );		 	// ��M�o�b�t�@������

	// �w���V�[�`���^�C���[�Ď��I��
	CancelWaitableTimer(my_evHelcy);


	// ���g���C
//	LARGE_INTEGER ts;
//	ts.QuadPart = (__int64)PARAM_SOCK_RETRY_TIME * -10000;	// 1[ms]��100[ns] (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
//	SetWaitableTimer(my_evReTryConnect, &ts, 0, NULL, NULL, FALSE);
	__int64 ts = PARAM_SOCK_RETRY_TIME * -10000;	// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(my_evReTryConnect, (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE);
}
