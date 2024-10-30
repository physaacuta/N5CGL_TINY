#include "stdafx.h"
#include "SeqFrameRecv.h"

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
SeqFrameRecv::SeqFrameRecv() :
ThreadManager("SeqFrameR"),
m_nSock(INVALID_SOCKET),
m_bSt(false),
m_bRun(false),
my_bHelcyOk(false),
m_bDebugWpd(false)
{
	// �C�x���g
	my_tmHelcy	= CreateWaitableTimer(NULL, FALSE, NULL);


	// ������
	memset(my_RecvBuf, 0x00, sizeof(my_RecvBuf));
	memset(&m_typLastFrame, 0x00, sizeof(m_typLastFrame));



	// UDP�\�P�b�g����
	WSADATA	wsadata;
	//WSAStartup(MAKEWORD(2,2), &wsadata);
	WORD wVersionRequired = MAKEWORD(2, 2);
	WSAStartup(wVersionRequired, &wsadata);
	m_nSock = WSASocket(AF_INET, SOCK_DGRAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	_ASSERT(INVALID_SOCKET != m_nSock);
	int opt = 1;
	setsockopt(m_nSock, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));
	// bind
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(SEQ_UDP_PORT);
	int ret = bind(m_nSock, (LPSOCKADDR)&addr, sizeof(SOCKADDR_IN));
	_ASSERT(0 == ret);

	// �I�[�o�[���b�v����
	memset(&m_ovlRecv, 0, sizeof(OVERLAPPED));
	m_ovlRecv.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);		// manual reset
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
SeqFrameRecv::~SeqFrameRecv()
{
	// �w���V�[�`���^�C���[�Ď��I��
	CancelWaitableTimer(my_tmHelcy);

	//// �\�P�b�g���
	closesocket(m_nSock);

	//// �C�x���g
	CloseHandle(my_tmHelcy);	
	CloseHandle(m_ovlRecv.hEvent);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �X���b�h����

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int SeqFrameRecv::ThreadFirst()
{
	//// �C�x���g�ݒ�
	AddEventNum(1, &m_ovlRecv.hEvent);
	AddEventNum(1, &my_tmHelcy);


	// �J�n
	SetEvWaitableTimer(my_tmHelcy, PARAM_TIMEOUT_HELCY_RECV, PARAM_TIMEOUT_HELCY_RECV);
	if( ! Recving() ) OnErr(false);
	return 0;
}
//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int SeqFrameRecv::ThreadLast()
{
	// �w���V�[�`���^�C���[�Ď��I��
	CancelWaitableTimer(my_tmHelcy);
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void SeqFrameRecv::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_RECVEND	= 0,		// ��M�����ʒm
			EV_HELCY				// �w���V�[�^�C�}�[
	};
	
	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_RECVEND: 									// ��M�����ʒm
		if( ! RecvCheck() ) {
			OnErr(false);
		}
		break;

//-----------------------------------------------------------------------------------------------
	case EV_HELCY: 										// �w���V�[�Ď�
		if( ! my_bHelcyOk ) {
			OnErr(false);
			if(m_bSt) {			// ���큨�ُ�
				LOG(em_ERR), "[%s] �w���V�[�Ď��^�C���A�E�g", my_sThreadName);
				LOGSEQ(em_ERR), "[%s] �w���V�[�Ď��^�C���A�E�g", my_sThreadName);
				mcls_pStatus->SetStatusQue( "UDP_SEQ_ID", false, true);
			}

			m_bSt = false;	
		} else {
			if( ! m_bSt) {		// �ُ큨����
				LOG(em_MSG), "[%s] �w���V�[�Ď����한�A", my_sThreadName);
				LOGSEQ(em_MSG), "[%s] �w���V�[�Ď����한�A", my_sThreadName);
				mcls_pStatus->SetStatusQue( "UDP_SEQ_ID", true, true);
			}
			m_bSt = true;
		}
		my_bHelcyOk = false;
		break;

//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		LOGSEQ(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

//------------------------------------------
// ��M�f�[�^�̃`�F�b�N
// �߂�l false:�ؒf  ture:�������Ȃ�
//------------------------------------------
bool SeqFrameRecv::RecvCheck()
{
	if( INVALID_SOCKET == m_nSock ) return false;


	//// �Ǎ�
	DWORD dwTransBytes; 
	DWORD flags = 0;
	BOOL bRet = WSAGetOverlappedResult(m_nSock, &m_ovlRecv, &dwTransBytes, TRUE, &flags);
	ResetEvent(m_ovlRecv.hEvent);
	if( ! bRet ) {								// �ǂݍ��ݎ��s
		int errcode = WSAGetLastError();
		LOGSEQ(em_ERR), "[%s] WSAGetOverlappedResult ���s [%d]", my_sThreadName, errcode);
		//return false;
		goto Ending;
	}

	// ���������M�ł���
	SEQ_TYPE_HEAD& h = (SEQ_TYPE_HEAD&)my_RecvBuf;


	//======================================================
	// �`���U�蕪��
	if( SEQ_TC_HELCY == h.nTC) {			// �w���V�[�H
		// ������
		my_bHelcyOk = true;		

	} else if( SEQ_TC_FRAME == h.nTC) {			// �t���[�����H
		my_bHelcyOk = true;		
		if( ! AddNewFrame() ) goto Ending;	// �f�[�^��M����

	} else {
		// ����ȊO�Ȃ炠�肦�Ȃ��B
		LOGSEQ(em_ERR), "[%s] TC�ُ�`����M [tc=%d size=%d]", my_sThreadName, h.nTC, h.nSeqLen);
		goto Ending;
	}


	//// ��
Ending:
	if( ! Recving() ) return false;		// ���`����M
	return true;
}

//------------------------------------------
// �V�K�t���[������M
// �߂�l false:�ؒf  ture:�������Ȃ�
//------------------------------------------
bool SeqFrameRecv::AddNewFrame()
{
	// �f�o�b�N�B����WPD�F��
	if( m_bDebugWpd ) {
		m_bDebugWpd = false;
		SEQ_TYPE_FRAME& wk = (SEQ_TYPE_FRAME&)my_RecvBuf;
		wk.data.nWpdFramePos = CTime::GetCurrentTime().GetSecond();
		LOGSEQ(em_WAR), "[%s] ����WPD�F�� F=%d", my_sThreadName, wk.data.nFrameNo);
	}


	// �f�[�^�Z�b�g
	SEQ_TYPE_FRAME& h = (SEQ_TYPE_FRAME&)my_RecvBuf;
//�� h.data.nFrameNo -= 2;

	int nQue = mque_Frame.GetCount();
	memcpy( &m_typLastFrame, &h.data, sizeof(SEQ_TYPE_DATA_FRAME));

	if( -1 != h.data.nWpdFramePos ) {
		//LOG(em_MSG), "[%s] ---- F=%d ----------------------------------", my_sThreadName, h.data.nFrameNo);
		LOGSEQ(em_MSG), "[%s] ---- F=%d ----------------------------------", my_sThreadName, h.data.nFrameNo);
	}
	LOGSEQ(em_MSG), "[%s] F=%lu PLG��=%lu tm��=%lu LVAL��=%lu PLG��=%lu TIME��=%lu WPD=%d QUE=%d", my_sThreadName,
		h.data.nFrameNo, h.data.nPlgDiff, h.data.nTimeDiff, h.data.nExSyncAdd, h.data.nPlgAdd, h.data.nTimeAdd, h.data.nWpdFramePos, nQue);



	if( ! m_bRun) {		// ����������Ȃ��̂ŏI���
		//LOGSEQ(em_ERR), "[%s] �J�n�O�ɃV�[�P���TUDP��t Fno:%d", my_sThreadName, h.data.nFrameNo );	
		return true;
	}

	// �L���[�C���O
	SEQ_TYPE_DATA_FRAME* pDeli = new SEQ_TYPE_DATA_FRAME;
	memcpy( pDeli, &h.data, sizeof(SEQ_TYPE_DATA_FRAME));
	if( ! mque_Frame.AddToTail(pDeli) ) {
		syslog(SYSNO_QUEFULL_ERR, "[�V�K�t���[�����o�^ F=%d]", h.data.nFrameNo);
		LOG(em_ERR), "[%s] �V�K�t���[�����o�^�L���[�t�� F=%d", my_sThreadName, h.data.nFrameNo);
		LOGSEQ(em_ERR), "[%s] �V�K�t���[�����o�^�L���[�t�� F=%d", my_sThreadName, h.data.nFrameNo);
		delete pDeli;
		return false;
	}
	return true;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TPC/IP �ʐM����


//------------------------------------------
// �Ɩ��`���̎�M���J�n
//------------------------------------------
bool SeqFrameRecv::Recving()
{
	if( INVALID_SOCKET == m_nSock ) return false;

	memset(&my_RecvBuf, 0x00, sizeof(my_RecvBuf));

	// ��񐶐�
	WSABUF wsabuf;								// ��M�\����
	wsabuf.buf = (char*)&my_RecvBuf[0];
	wsabuf.len = PARAM_BUFSIZE;					// ��ԑ傫���`���ł��ς����T�C�Y�ɂ��Ă����B
	//struct sockaddr_in fromAddr;				// ��M��
	my_fromLen = sizeof(my_fromAddr);
	DWORD dwTransBytes; 
	DWORD flags = 0;


	// �w�b�_�[����M�J�n
	ResetEvent(m_ovlRecv.hEvent);
	int ret = WSARecvFrom(m_nSock, &wsabuf, 1, &dwTransBytes, &flags, (struct sockaddr *)&my_fromAddr, &my_fromLen, &m_ovlRecv, NULL);
	int errcode = WSAGetLastError();
	if (SOCKET_ERROR == ret && WSA_IO_PENDING != errcode) {		// �ُ�L��
		LOGSEQ(em_ERR), "[%s]�\�P�b�g �w�b�_�[����M���s err_code=%d", my_sThreadName, errcode);
		syslog(SYSNO_SOCK_RECV_ERR, "[%s,�w�b�_�[�� err_code=%d]", my_sThreadName, errcode);
		return false;
	}

	//LOGSEQ(em_MSG), "[%s]�\�P�b�g �w�b�_�[����M", my_sThreadName);

	return true;
}

//------------------------------------------
// �ُ�ʒm
// bool bManyu   true:�����ŕ������Đؒf
//------------------------------------------
void SeqFrameRecv::OnErr(bool bManyu)
{
	QueAllDelete();						// �����M�o�b�t�@��ԋp

	LOGSEQ(em_ERR), "[%s] �ُ�ʒm(%d)", my_sThreadName, bManyu);

	// �O���� �ؒf��ʒm
	if( ! bManyu && m_bSt ) {
		mcls_pStatus->AddQueMyErrWar(true, ON_SEQ_FRAME_SOCK_CLOSE);		// �V�[�P���T�Ƃ̒ʐM�f��
	}
}

//------------------------------------------
// ���ׂẴL���[��ԋp����
//------------------------------------------
void SeqFrameRecv::QueAllDelete()
{
	SEQ_TYPE_DATA_FRAME* deli;

	// �f�o�b�N�p���O
	if(0 !=mque_Frame.GetMaxCount() ){		// 1��������Ă��Ȃ����͏o���Ȃ�
		LOGSEQ(em_MSG), "[%s] �����M�L���[�J��! Que=%d, MAX=%d", my_sThreadName, mque_Frame.GetCount(), mque_Frame.GetMaxCount());
	}

	// �L���[�ɗ��܂��Ă���C���X�^���X��S���J��
	while(true) {
		deli = mque_Frame.GetFromHead(0);
		if (deli == NULL) break;

		// �ԋp
		delete deli;
		deli = NULL;
	}
	// �ő匏�����Z�b�g
	mque_Frame.ReSetMaxCount(); 
}
