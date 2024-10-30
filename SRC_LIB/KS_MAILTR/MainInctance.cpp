#include "StdAfx.h"
#include "MainInctance.h"

#include "winsock2.h"

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
MainInctance::MainInctance(void) :
ThreadManager("MI")
{
	// �V�O�i������
	m_evThStart  = CreateEvent(NULL, FALSE, FALSE, NULL);				// ���C���X���b�h�����p
}
//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
MainInctance::~MainInctance(void)
{
	// �V�O�i���J��
	CloseHandle(m_evThStart);
}

//------------------------------------------
// �X���b�h�L���[�Ƀ��[���f�[�^�o�^
// ENUM_MAININCTANCE emNo ���b�Z�[�WNo (�C�x���gNo���̂܂܂ɂ��悤���Ǝv�������AFACT??_01�Ƃ����ƕ�����ɂ��������߂�)
// COMMON_QUE* que ���[���X���b�g�\���̂��̂���
//------------------------------------------
void MainInctance::SetDeliveryMail(ENUM_MAININCTANCE emNo, COMMON_QUE* que)
{
	DELIVERY_MAIL* pMail = new DELIVERY_MAIL;
	pMail->emNo	= emNo;
	memcpy( &pMail->que, que, sizeof(COMMON_QUE));

	// �X���b�h�L���[�ɓo�^
	if( ! mque_DeliMail.AddToTailFreeCheck(pMail) ) {
		delete pMail;
		LOG(em_ERR), "[%s] mque_DeliMail �L���[�t��!!", my_sThreadName);
	}
}

//------------------------------------------
// ���������� (��x�̂݋N��)
//------------------------------------------
BOOL MainInctance::Init()
{
	//// ���C���C���X�^���X�̂݁A�X���b�h�����͍���Ă��� (�����ɔ�V�O�i���Ƃ���)
	//// �p�����N���X���s (���C������X���b�h�ׁ̈A������ƗD��x����)
	ThreadManager::Start(THREAD_PRIORITY_ABOVE_NORMAL );

	// �X���b�hID��������܂ł�����Ƌx�e
	while( 0==GetTid() ) {
		LOG(em_MSG), "[%s] �X���b�hID���蓖�Ē��E�E�E", my_sThreadName);
		Sleep(1);			// �X���b�h�N���܂ł�����Ƒ҂�
	}


	////////////////////////////////////
	//// ���[�J�[�X���b�h ������
	////////////////////////////////////
	gcls_TrUdpMgr.SetLogMgr(mcls_pLog);

	////////////////////////////////////
	//// ���[�J�[�X���b�h �N��
	////////////////////////////////////

	gcls_TrUdpMgr.Start();



	return TRUE;
}

//------------------------------------------
// �I������ (��x�̂݋N��)
//------------------------------------------
void MainInctance::Exit()
{

	// ���[�J�[�X���b�h ��~
	gcls_TrUdpMgr.Stop();

	// ���[�J�[�X���b�h ���
	//////delete gcls_TrUdpMgr;
	
}


//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int MainInctance::ThreadFirst()
{
	//=======================================================
	// UDP����
	while(true){
		if( SetUdp() )	break;
		LOG(em_ERR), "[%s] UDP �������E�E�E ", my_sThreadName);
		Sleep(2000);
	}

	////// �����[�N�X���b�h�̏����������܂őҋ@ (MainInctance�̂�)
	LOG(em_MSG), "[%s] �����[�N�X���b�h�̏����������܂őҋ@���E�E�E [0x%X]", my_sThreadName, GetTid() );
	WaitForSingleObject(m_evThStart, INFINITE);
	
	//// �C�x���g�ݒ�
	HANDLE hArray[] = { mque_DeliMail.g_evSem
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
//*/
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void MainInctance::ThreadEvent(int nEventNo)
{
	DELIVERY_MAIL* pMail;

	////// �V�O�i���̏���
	enum {	EV_MAIL = 0,				// ���[���X���b�g �ʒm (���[���͈�ԍŌ�ɂ��悤����)
	};

	////// �V�O�i����������
	switch (nEventNo) {


//-----------------------------------------------------------------------------------------------
	case EV_MAIL: 						// ���[���X���b�g �ʒm
		pMail = mque_DeliMail.GetFromHead();
		switch(pMail->emNo) {

//-----------------------------------------------------------
		case E_DEF_ML_TRANS:			// ���[���X���b�g�]���v��
				if(true) {
					struct in_addr		addr;
					memcpy(&addr, &pMail->que.mailtr_que.nUdpIpAddr, sizeof(in_addr));

					LOG(em_INF),  "[%s] ���[���X���b�g�]�� [Ip:%s][Pc:%s][Task:%s]", my_sThreadName, inet_ntoa(addr), pMail->que.mailtr_que.cPcName, pMail->que.mailtr_que.cTaskName);

					UdpMail udpmail = {};
					memcpy(udpmail.hostname, pMail->que.mailtr_que.cPcName, sizeof(udpmail.hostname));
					memcpy(udpmail.mailname, pMail->que.mailtr_que.cTaskName, sizeof(udpmail.mailname));
					udpmail.datalen = sizeof(pMail->que.mailtr_que.cMsg);
					memcpy(udpmail.data, pMail->que.mailtr_que.cMsg, udpmail.datalen);


					sockaddr_in sin;
					sin.sin_family = AF_INET;
					sin.sin_port = htons( KS_MAILTR_PORT );
					sin.sin_addr.s_addr = pMail->que.mailtr_que.nUdpIpAddr;

					sendto(m_sock_udp, (const char*)&udpmail, sizeof(UdpMail), 0, (sockaddr*)&sin, sizeof(sin));
				}
			break;


//-----------------------------------------------------------
		}
		// �J��
		delete pMail;
		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

//////////////////////////////////////////////////////////////////////
// �\�P�b�g

//------------------------------------------
// UDP SOCKET����
// �߂�l ���A���
//------------------------------------------
BOOL MainInctance::SetUdp()
{
	WSADATA wsaData;
	WORD wVersionRequired = MAKEWORD(2, 2);
	WSAStartup(wVersionRequired, &wsaData);
	m_sock_udp = (int)socket(AF_INET, SOCK_DGRAM, 0);
	if (INVALID_SOCKET == m_sock_udp) return FALSE;
	return TRUE;
}


