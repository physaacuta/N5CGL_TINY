#include "StdAfx.h"
#include "CamLinkSpeedManager.h"

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
CamLinkSpeedManager::CamLinkSpeedManager(void) :
ThreadManager("LnkSpdMgr")
{
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
CamLinkSpeedManager::~CamLinkSpeedManager(void)
{
}


//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int CamLinkSpeedManager::ThreadFirst()
{
	//// UDP�̎�M����
	OpenUdpPort();

	//// Ini�t�@�C����������擾
	GetIniSetting();

	//// �C�x���g�ݒ�
	HANDLE hArray[] = { m_hEventRecv
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int CamLinkSpeedManager::ThreadLast()
{
	CloseUdpPort();
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void CamLinkSpeedManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_RECV_UDP = 0,				// �摜�����G���W������UDP��M
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_RECV_UDP: 						// �摜�����G���W������UDP��M

		if(true){

			// �C�x���g�����Z�b�g
			WSANETWORKEVENTS netevent;
			WSAEnumNetworkEvents( m_udp, m_hEventRecv, &netevent );

			//read all item in que.
			while( 0 != (netevent.lNetworkEvents & FD_READ) )
			{
				char				buf[2048];
				sockaddr_storage	peer;
				int					peerlen = sizeof(peer);
				int					len = recvfrom( m_udp, buf, 2048, 0, (sockaddr*)&peer, &peerlen );
				
				if( 0 >= len ){
					break;
				}

				const TYP_SER_CHANGE_REQ *packet = (const TYP_SER_CHANGE_REQ*)buf;
				if( cam_ser_packet_id != packet->PacketId ){
					LOG(em_ERR), "[%s] �����ȃp�P�b�g����M. [size:%d][PacketID:%x]", my_sThreadName, len, packet->PacketId );
					break;
				}

				// �����܂ŗ�����A����ȑ��x�ύX�ʒm
				LOG(em_MSG), "[%s] �V���A���ʐM���x �ύX�v������M [CamID:%d][Speed:%dbps]", my_sThreadName, packet->CamId, packet->nSpeed );


				// �J����ID����A�t���[���O���o�[�{�[�hID�����
				int nBoardID[MAX_PHYSICAL_CAMANGLE];
				int nCnt = SelectBoardID(packet->CamId, nBoardID);

				if( 0 != nBoardID ){

					for(int ii=0; ii<nCnt; ii++){

						LOG(em_MSG), "[%s] UDP�p�P�b�g���Ǌ��� [CamID:%d]��[BoardID:%d][Speed:%dbps]", my_sThreadName,
																									   packet->CamId,
																									   nBoardID[ii],
																									   packet->nSpeed);

						ToOutMailSender::Send_ToLinksw_ChangeLinkSpeed( nBoardID[ii], packet->nSpeed );
					}

				}else{

					LOG(em_ERR), "[%s] UDP�p�P�b�g�̃J����ID����O���o�[�{�[�hNo�����ł��Ȃ�����[CamID:%d]", my_sThreadName,
																											   packet->CamId);
				}
			}
		}
		return;

	
//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}


//=====================================
// UDP�|�[�g �I�[�v��
//=====================================
void CamLinkSpeedManager::OpenUdpPort()
{
	WSADATA	wsaData;
	WORD	wVersionRequired = MAKEWORD(2, 2);		// WinSock 2.0 Windows NT
	WSAStartup(wVersionRequired, &wsaData);

	m_udp		 = 0;
	m_hEventRecv = 0;
	
	// �摜�����G���W���ƒʐM�p�|�[�g�͐���H
	_ASSERT(1024 < cam_ser_udp_port);

	m_hEventRecv = WSACreateEvent();
	m_udp        = socket( AF_INET, SOCK_DGRAM, IPPROTO_IP );
	if( INVALID_SOCKET == m_udp ){

		LOG(em_ERR), "[%s] Socket Creation Err [Port:%d]", my_sThreadName, cam_ser_udp_port);
	}

	BOOL bTrue = 1;
	setsockopt(m_udp, SOL_SOCKET, SO_BROADCAST, (const char *)&bTrue, 4 );
	setsockopt(m_udp, SOL_SOCKET, SO_REUSEADDR, (const char *)&bTrue, 4 );

	sockaddr_in sin = {AF_INET};
	sin.sin_port = htons(cam_ser_udp_port);
	if( bind( m_udp, (sockaddr*)&sin, sizeof(sin) ) !=0){
		LOG(em_ERR), "[%s] Socket Error. Bind [Port:%d]", my_sThreadName, cam_ser_udp_port);
	}
	WSAEventSelect( m_udp, m_hEventRecv, FD_READ );
}

//=====================================
// UDP�|�[�g �N���[�Y
//=====================================
void CamLinkSpeedManager::CloseUdpPort()
{
	if( 0 < m_udp ){
		WSAEventSelect( m_udp, m_hEventRecv, 0 );
		closesocket( m_udp );
		WSACloseEvent(m_hEventRecv);
		m_udp = 0;
	}
}

//=====================================
// Ini�t�@�C������ڑ�CH���擾���Ĕz��ɃZ�b�g
//=====================================
void CamLinkSpeedManager::GetIniSetting()
{
	// �ȉ��̎d�l��INI�t�@�C���ɒ�`����Ă���ڑ�CH�����擾
	//-------------------------------------------------------------
	// CAMLINKSW_PCIDx_y : ����PC�Ɛڑ�����Ă������ݸ�����o�� (FPGA�{�[�hNo*100 + ch(1�`15))
	// 		x:PCID 1�`
	// 		y:�ڑ��� 1�`

	memset(m_nBoardID, 0x00, sizeof(m_nBoardID));

	for(int ii = 1; ii <= NUM_ALL_HANTEI; ii++){

		for(int jj = 1; jj <= NUM_CAMLINK_BOARD; jj++){

			CString	ss;	
			ss.Format("CAMLINKSW_PCID%d_%d", ii,jj);

			int nIniID	= GetPrivateProfileInt(TO_DEFECT, ss, 0, TASKINI_NAME);		// INI�t�@�C���������Ă������l

			// ��`����������A�o��ch�����
			if( 0 != nIniID ){

				int nFPGA	 = nIniID / 100;		// FPGA�{�[�h��ID�B0�F1��ڂ�Master�A1�F1��ڂ�Slave�A�ȍ~�A��
				int nBoardID = nIniID % 100;		// �o��ch��No�i1�`15�j

				// �������̃`�F�b�N
				if( 0 > nFPGA ||
					NUM_CAMLINKSW * MAX_CAMLINKSW_FPGA <= nFPGA ||
					0 >= nBoardID ||
					MAX_CAMLINKSW_OUTPUT < nBoardID){

					LOG(em_ERR),"[%s] INI�ݒ�ُ�[%s = %d]��[FPGABoard:%d][ch:%d]", my_sThreadName, ss, nIniID, nFPGA, nBoardID);
					continue;
				}

				// �z��ɕێ�
				m_nBoardID[ii-1][jj-1] = (nFPGA * MAX_CAMLINKSW_OUTPUT) + nBoardID;

#ifdef ENABLE_DEBUG_CAMLINK
				LOG(em_MSG), "[%s] INI�ݒ�Ǎ�����[%s = %d]��[FPGABoard:%d][ch:%d]", my_sThreadName, ss, nIniID, nFPGA, nBoardID);
#endif
			}
		}
	}
}

//==========================================================
// �J����ID����A�t���[���O���o�[�{�[�hNo�����
//----------------------------------------------------------
// int		nCamId	: �J����ID(1�I���W��)
// int*		nBoardID: �t���[���O���o�[�{�[�hNo
// �ߒl		int		: �t���[���O���o�[�{�[�hNo�̐�(�ُ펞�A0)
//==========================================================
int CamLinkSpeedManager::SelectBoardID(const int nCamId, int* nBoardID)
{
	int nCamSetPerPc = mcls_pStatus->GetCamSetPerPc();				// 1PC������̃J�����Z�b�g��

	int	nTorB	= (nCamId - 1) / NUM_CAM_POS;						// �\�^��		0:�\ 1:��
	int nPos	= (nCamId - 1) % NUM_CAM_POS;						// �ʒu			0:DS 1:DQ 2:WQ 3:WS
	int nPcNo	= nPos / nCamSetPerPc;								// �S������PCno(0�I���W��)
	int nCamSet = nPos % nCamSetPerPc;								// 1PC���́ACamSet���ځH(0�I���W��)
	int nPcId	= mcls_pStatus->GetHtID(nTorB, nPcNo);				// �S������PCID(1�I���W��)

	int nNum    = 0;	// �ΏۂƂȂ�t���[���O���o�[�{�[�hNo�̐�(MAX_PHYSICAL_CAMANGLE�Ɠ������Ȃ�͂�)

	// �J�����Z�����[�h��( = iPortViewer����̗v��)�̏ꍇ�́A
	// �Y�����锻��PC�̐擪�Ɋ���U��ꂽ�J����No��؂�ւ�
	if( EM_DIV_UNTEN::DIV_UNTEN_CAMERA == mcls_pStatus->GetUnten() ){
	
		for(int ii=0; ii<MAX_PHYSICAL_CAMANGLE; ii++){
			nBoardID[ii] = m_nBoardID[nPcId - 1][ii];
			nNum++;
		}

	// ����ȊO�̏ꍇ�́A�Y�����锻��PC�́A�Y������J�����̂ݐ؂�ւ�
	}else{
		for(int ii=0; ii<MAX_PHYSICAL_CAMANGLE; ii++){

			if( NUM_CAMLINK_BOARD > nCamSet+ii ){
				nBoardID[ii] = m_nBoardID[nPcId - 1][(nCamSet*MAX_PHYSICAL_CAMANGLE)+ii];
				nNum++;
			}
		}
	}

	return nNum;
}


//==========================================================
// �V���A�����x�������ݒ�ɖ߂�
//----------------------------------------------------------
// int		nBaudrate	: �V���A���ʐM���x[bps]
//==========================================================
void CamLinkSpeedManager::InitCamLinkSpeed(int nBaudrate)
{
	for(int ii=0; ii<NUM_ALL_HANTEI; ii++){
		for(int jj=0; jj<NUM_CAMLINK_BOARD; jj++){

			int nBoardID = m_nBoardID[ii][jj];

			if( 0 != nBoardID ){

				ToOutMailSender::Send_ToLinksw_ChangeLinkSpeed(nBoardID, nBaudrate);
			}
		}
	}
}
