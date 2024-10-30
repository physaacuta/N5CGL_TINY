#include "stdafx.h"
#include "KizuOpSend.h"

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// �R���X�g���N�^
// const char *cSession ini�t�@�C���Z�b�V����
//------------------------------------------
KizuOpSend::KizuOpSend(const char *cSession) :
my_cSession(cSession),
my_bIsNowSend(FALSE),
my_pHead(NULL),
my_nSendingDataSize(0),
mcls_pSock(NULL),
mcls_pLog(NULL)
{
	// �S�N���C�A���g�ؒf���F��
	for(int ii=0; ii<PARAM_SOCK_MAX; ii++) my_bIsConnect[ii] = FALSE;


	// �V�O�i������
	my_evConnect	= CreateEvent(NULL, FALSE, FALSE, NULL);
	my_evClose		= CreateEvent(NULL, FALSE, FALSE, NULL);
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
KizuOpSend::~KizuOpSend()
{
	// �S�L���[�J��
	QueAllFree();

	// �\�P�b�g�J��
	if(NULL != mcls_pSock) delete mcls_pSock;

	// �V�O�i���J��
	CloseHandle(my_evConnect);
	CloseHandle(my_evClose);
}


//------------------------------------------
// ���o�b�t�@���J������
//------------------------------------------
void KizuOpSend::QueBufFree()
{
	if(my_pHead==NULL) return;
	
	// �J��
	if ( my_pHead->addr[0] != NULL ) delete my_pHead->addr[0];		// �f�[�^���J��
	if ( my_pHead->addr[1] != NULL ) delete my_pHead->addr[1];
	delete my_pHead;

	my_pHead = NULL;
}

//------------------------------------------
// ���ׂẴL���[�J��
//------------------------------------------
void KizuOpSend::QueAllFree()
{
	SOCK_BASE_HEAD*		deli = NULL;

	// �f�o�b�N�p���O
	if(0 != gque_Deli.GetMaxCount() ){		// 1��������Ă��Ȃ����͏o���Ȃ�
		LOG(em_MSG), "[KizuOpSend] <%s> �����M�L���[�J��! Que=%d, MAX=%d", my_cSession, gque_Deli.GetCount(), gque_Deli.GetMaxCount());
	}

	// �L���[�ɗ��܂��Ă���C���X�^���X��S���J��
	while(true) {
		deli = gque_Deli.GetFromHead(0);
		if (deli == NULL) break;
		// �J��
		if ( deli->addr[0] != NULL ) delete deli->addr[0];		// �f�[�^���J��
		if ( deli->addr[1] != NULL ) delete deli->addr[1];
		delete deli;
		deli = NULL;
	}
	// �ő匏�����Z�b�g
	gque_Deli.ReSetMaxCount(); 
	// ���o���ς݂̃o�b�t�@������Έꏏ�ɊJ������
	QueBufFree();
}



//------------------------------------------
// �X���b�h
//  MsgWaitForMultipleObjects ���g��Ȃ��ƃ_���B�V�O�i�������ł͑Ή��s�B����M��msg��ID���K�v
//------------------------------------------
UINT WINAPI KizuOpSend::MainThread(LPVOID param)
{
	MSG	msg;								// MSG�\����
	int id;

	////// ���C���C���X�^���X���o��
	KizuOpSend *p = (KizuOpSend *)param;
	p->SetTid();								// ���X���b�hID�Z�b�g
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);			// ���b�Z�[�W�쐬


	//// �\�P�b�g���� + Listen (��������܂Ńu���b�N)
	bool disp = true;
	while( TRUE ) {
		if( p->Listen(disp) ) break;		// �ŏ��̈��̂݃��O�o�� (�ʏ�Listen�Ŏ��s���鎖����)
		Sleep(2000);
		disp = false;
	}


	//// �w���V�[���M�^�C�}�[�J�n
	__int64 ts = PARAM_TIME_HELCY_SEND * -10000;	// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	HANDLE	evHelcy;								// �w���V�[�`�����M�^�C�}�[
	evHelcy	= CreateWaitableTimer(NULL, FALSE, NULL);			
	SetWaitableTimer(evHelcy, (LARGE_INTEGER *)&ts, PARAM_TIME_HELCY_SEND, NULL, NULL, FALSE);


	//// WaitForMultipleObjects�C�x���g
	HANDLE	hArray[] = {p->my_evStop,
						evHelcy,
						p->gque_Deli.g_evSem,
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// �X���b�h�I��
			EV_HELCY,						// �w���V�[�`�����M
			EV_QUE,							// �L���[�C�x���g
			EV_END	};


	// ���b�Z�[�W�L���[�ǂݍ���
	pLOG(em_MSG), "[KizuOpSend] <%s> �X���b�h�J�n [0x%X]", p->my_cSession, p->GetTid());
	while (1) {
		int wait_num = ( p->my_bIsNowSend ) ? EV_QUE : EV_END;
//		DWORD ret = WaitForMultipleObjects(wait_num, hArray, FALSE, INFINITE);
		int ret = MsgWaitForMultipleObjects(wait_num, hArray, FALSE, INFINITE, QS_POSTMESSAGE | QS_TIMER);
		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// �X���b�h�I��
			pLOG(em_MSG), "[KizuOpSend] <%s> �X���b�h�I�� [0x%X]", p->my_cSession, p->GetTid());

			// �w���V�[���M�^�C�}�[��~
			CancelWaitableTimer(evHelcy);
			CloseHandle(evHelcy);
			return 0;


//-----------------------------------------------------------------------------------------------
		case EV_HELCY:												// �w���V�[�`�����M
			//pLOG(em_MSG), "[KizuOpSend] <%s> �w���V�[�`�����M", p->my_cSession);
			p->SendHelcy();
			break;


//-----------------------------------------------------------------------------------------------
		case EV_QUE:												// ���M�˗� (�L���[�Z�}�t�H�[)
			if ( EV_QUE == wait_num-1 ) {
				p->SendStart();
			}
			break;
		}


		// �����ɗ����Ƃ��́A���b�Z�[�W���������邩������Ȃ�
		// �]���ă��[�v�Ń��b�Z�[�W��S���z���o�� (���������� PeekMessage �łȂ��ƃ_��)
		int rc;
		while (rc = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) {
			switch( msg.message ) {

//-----------------------------------------------------------------------------------------------
			case WM_USER+WM_KS_ACCEPT:								// �ڑ��v��
				//// �A�N�Z�v�g
				id = p->mcls_pSock->Accept(&msg);
				if( id < 0 ) {
					pLOG(em_ERR), "[KizuOpSend] <%s> �N���C�A���g���I�[�o�[", p->my_cSession);
					syslog(SYSNO_SOCK_CLIENT_MAX, "[KizuOpSend]");
					break;
				}

				//// ����
				pLOG(em_MSG), "[KizuOpSend] <%s> �\�P�b�g�ڑ� [client_no=%d]", p->my_cSession, id);
				syslog(SYSNO_SOCK_CONNECT, "[(KizuOpSend) %s: client_no=%d]", p->my_cSession, id);
				p->my_bIsConnect[id] = TRUE;			// �N���C�A���g�ڑ��F��
				SetEvent(p->my_evConnect);
				break;

//-----------------------------------------------------------------------------------------------
			case WM_USER+WM_KS_DISCONNECT:							// �ؒf�ʒm
				//// ���M���ɐؒf�L��
				if( p->my_bIsNowSend ) {
					pLOG(em_ERR), "[KizuOpSend] <%s> ���M���ɐؒf�I���M�r�������ǒ��f", p->my_cSession);
					// ���M�����I��
					p->SendEnd();
				} else {
					pLOG(em_ERR), "[KizuOpSend] <%s> �ؒf�ʒm�I", p->my_cSession);
				}
				//id = p->mcls_pSock->GetSockID((UINT)msg.wParam);		// UINT��64bitAP�̏ꍇNG
				id = p->mcls_pSock->GetSockID(msg.wParam);	
				if(-1==id) break;		// ���ɂǂ����Őؒf�ς�
				p->Close(id); 
				SetEvent(p->my_evClose);
				break;

//-----------------------------------------------------------------------------------------------
			case WM_USER+WM_KS_SEND:								// ���M����
				// �S�N���C�A���g�ɑ��M����
				if(p->my_bIsAllSend) {		
					p->SendEnd();
					break;
				}
				
				// ���𑗐M
				p->Send();
				break;

//-----------------------------------------------------------------------------------------------
			case WM_USER+WM_KS_SEND_TIMEOUT:
				pLOG(em_ERR), "[KizuOpSend] <%s> ���M�^�C���A�E�g", p->my_cSession);
				p->SendEnd();
				//id = p->mcls_pSock->GetSockID((UINT)msg.wParam);		// UINT��64bitAP�̏ꍇNG
				id = p->mcls_pSock->GetSockID(msg.wParam);
				if(-1==id) break;		// ���ɂǂ����Őؒf�ς�
				p->Close(id); 
				break;
			}
		}
	}
	return 0;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �V�[�P���X����

//------------------------------------------
// �r���`�����M �J�n����
//------------------------------------------
void KizuOpSend::SendStart()
{
	// �o�b�t�@�ɃL���[�����łɂ���H
	if(my_pHead != NULL) {
		LOG(em_ERR), "[KizuOpSend] <%s> �o�b�t�@�ɃL���[�����łɗL��", my_cSession);
		QueBufFree();
	}

	// �L���[���o��
	my_pHead = gque_Deli.GetFromHead();

	// ���M�J�n����
	my_bIsAllSend = FALSE;
	my_nNowSendCount = 0;
	my_nSendingDataSize =0;

	// �w�b�_�[���M�J�n
	if( Send() ) {						// ���M�J�n
		my_bIsNowSend = TRUE;
	} else {							// 1�������Ȃ�����
		// ���ɂȂɂ����Ȃ�
	}
}

//------------------------------------------
// �r���`�����M ��������
//------------------------------------------
void KizuOpSend::SendEnd()
{
	//// �S�����M����
	my_bIsNowSend = FALSE;
	my_bIsAllSend = TRUE;			// �O�ׂ̈ɁE�E�E
	QueBufFree();
}

//------------------------------------------
// �w���V�[�`�����M ����
//------------------------------------------
void KizuOpSend::SendHelcy()
{
	SOCK_BASE_HEAD*		pSendBuf = new SOCK_BASE_HEAD;

	//// �f�[�^�Z�b�g
	memset(pSendBuf, 0x00, sizeof(SOCK_BASE_HEAD));
	memcpy(pSendBuf->code, T_CODE_HELCY, SIZE_SOCK_TC);
	strcpy(pSendBuf->pname, TOKATU);
	pSendBuf->len		= sizeof(SOCK_BASE_HEAD);
	pSendBuf->addr[0]	= NULL;
	pSendBuf->addr[1]	= NULL;

	//// ���M�˗�
	if( ! gque_Deli.AddToTailFreeCheck(pSendBuf) ) {		// �o�^
		LOG(em_ERR), "[KizuOpSend] �����˕\���� �w���V�[�`�����M�˗��L���[�t��");
		syslog(SYSNO_QUEFULL_WAR, "[�����˕\���� �w���V�[�`��]");
		delete pSendBuf;
	}
}





// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TPC/IP �ʐM����

//------------------------------------------
// Listen
// bool bLogDsp ���O�o�͗L�� (false:���s���O�o���Ȃ�)
//------------------------------------------
BOOL KizuOpSend::Listen(bool bLogDsp)
{
	int retc;
	// �\�P�b�g�C���X�^���X����
	try {
		mcls_pSock = new SockBase(my_cSession, TASKINI_NAME, GetTid(), PARAM_SOCK_TIMEOUT, 
										 PARAM_SOCK_MAX, 0, PARAM_SOCK_SEND_BUFSIZE);
	} catch(int code) {
		mcls_pSock = NULL;
		if(bLogDsp) {
			LOG(em_ERR), "[KizuOpSend] <%s> �\�P�b�g New���s err_code=%d", my_cSession, code);
			syslog(SYSNO_SOCK_NEW_ERR, "[%s: err_code=%d]", my_cSession, code);
		}
		return FALSE;
	}

	// Listen
	if( NULL != mcls_pSock ) {
		retc = mcls_pSock->Listen();
		if( 0 != retc ) {
			delete mcls_pSock;
			mcls_pSock = NULL;
			if(bLogDsp) {
				LOG(em_ERR), "[KizuOpSend] <%s> �\�P�b�g Listen���s err_code=%d", my_cSession, retc);
				syslog(SYSNO_SOCK_LISTEN_ERR, "[%s: err_code=%d]", my_cSession, retc);
			}
			return FALSE;
		}
	}

	LOG(em_MSG), "[KizuOpSend] <%s> �\�P�b�g Listen�J�n", my_cSession);
	return TRUE;
}

//------------------------------------------
// �ؒf����
// int id �N���C�A���gID
//------------------------------------------
void KizuOpSend::Close(int id)
{
	if(-1==id) return;		// ���ɂǂ����Őؒf�ς�

	// �ؒf����
	mcls_pSock->CloseID(id);

	my_bIsConnect[id] = FALSE;
	LOG(em_ERR), "[KizuOpSend] <%s> �N���C�A���g�ؒf [client_no=%d]", my_cSession, id);
	syslog(SYSNO_SOCK_CLOSE, "[KizuOpSend(%s): client_no=%d]", my_cSession, id);
}

//------------------------------------------
// �r���`�����M
// �߂�l FALSE:1������ꂸ�ɏI�������ꍇ
//------------------------------------------
BOOL KizuOpSend::Send()
{
/*
	// ���M�\�ȃN���C�A���g��T��
	int ii;
	for(ii=my_nNowSendCount; ii<PARAM_SOCK_MAX; ii++) {
		// �Ȃ����ĂȂ����̂͘_�O
		if( ! my_bIsConnect[ii] ) continue;

		// �����M�̃N���C�A���g����
		int iRet = mcls_pSock->Send(ii, my_pHead, my_pHead->len );
		if( iRet ) {				// ���M�ُ�
			LOG(em_ERR), "[KizuOpSend] <%s> ���M�G���[[client_no=%d, err_code=%d]", my_cSession, ii, iRet);
			syslog(SYSNO_SOCK_SEND_ERR, "[KizuOpSend(%s): client_no = %d: err_code = %d]", my_cSession, ii, iRet);
			Close(ii);
			continue;

		} else {
			// ���M����
			my_nNowSendCount++;
			// �S�����M�ł����H
			if(ii == (PARAM_SOCK_MAX-1) ) my_bIsAllSend = TRUE;
			break;		// 1��Â�������Ȃ�����
		}
	}

	//// ���M�\�ȃN���C�A���g�������ꍇ�͑��M���������Ȃ����� �����ő��M�����Ƃ���
	if(PARAM_SOCK_MAX == ii) {
		SendEnd();
		return FALSE;
	} else {
		return TRUE;
	}
*/

	// ���M�ʒu�̃|�C���^
	BYTE* pSendData = (BYTE*)my_pHead;	
	// ���M�T�C�Y�w��
	DWORD size;

	// ���M�\�ȃN���C�A���g��T��
	int ii;
	for(ii=my_nNowSendCount; ii<PARAM_SOCK_MAX; ii++) {
		// �Ȃ����ĂȂ����̂͘_�O
		if( my_bIsConnect[ii] ) break;
	}

	//// ���M�\�ȃN���C�A���g�������ꍇ�͑��M���������Ȃ����� �����ő��M�����Ƃ���
	if(PARAM_SOCK_MAX == ii ) {
		SendEnd();
		return FALSE;
	}

	// �܂��������M���H
	size = my_pHead->len - my_nSendingDataSize;					// ���M�T�C�Y
	if(size > SIZE_SOCK_DATA_MAX) size = SIZE_SOCK_DATA_MAX;	// 1��̑��M�T�C�Y�I�[�o�[
	pSendData += (my_nSendingDataSize);

	// �����M�̃N���C�A���g����
	int iRet = mcls_pSock->Send(ii, pSendData, size );
	if( iRet ) {				// ���M�ُ�
		LOG(em_ERR), "[KizuOpSend] <%s> ���M�G���[[client_no=%d, err_code=%d]", my_cSession, ii, iRet);
		syslog(SYSNO_SOCK_SEND_ERR, "[KizuOpSend(%s), client_no = %d, err_code = %d]", my_cSession, ii, iRet);
		Close(ii);
		my_nSendingDataSize = 0;
		Send();

	} else {
		my_nSendingDataSize += size;
		if( my_nSendingDataSize >= my_pHead->len  ) {
			// ���M����
			my_nNowSendCount++;
			my_nSendingDataSize = 0;
			// �S�����M�ł����H
			if(ii == (PARAM_SOCK_MAX-1) ) my_bIsAllSend = TRUE;
		}
	}
	return TRUE;
}
