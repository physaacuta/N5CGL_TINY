#include "StdAfx.h"
#include "CmdSender.h"


// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


#include "MainInstance.h"

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
CmdSender::CmdSender(void) :
ThreadManager("CmdSend")
{

	////====================================
	//// Ini�t�@�C������p�����[�^�Ǎ���
	CString cWk;

	// ���M�p�@����|�[�gNo
	int nSendPort[NUM_MEN];
	for(int ii=0; ii<NUM_MEN; ii++){
		cWk.Format("HT_SEND_PORT_%d", ii);
		nSendPort[ii] = GetPrivateProfileInt("UDP", cWk, 0, TASKINI_NAME);
		_ASSERT( 0 != nSendPort[ii] );
	}

	// ���M�p IP�A�h���X
	memset(m_cUniIp, 0x00, sizeof(m_cUniIp));
	for(int ii=0; ii<NUM_SENDER; ii++){
		cWk.Format("HT_SEND_IP_%d", ii+1);
		GetPrivateProfileString("UDP", cWk, "", m_cUniIp[ii], 32, TASKINI_NAME);
		_ASSERT(0 != strlen(m_cUniIp[ii]));
	}


	////====================================
	//// UDP�ݒ�

	//---------------------------
	// �摜�ۑ�PC ���M�\�P�b�g
	WSADATA wsaData;
	WORD wVersionRequired = MAKEWORD(2, 2);
	WSAStartup(wVersionRequired, &wsaData);
	m_SockHtSend = (int)socket(AF_INET, SOCK_DGRAM, 0);
	_ASSERT(INVALID_SOCKET != m_SockHtSend);
	for(int ii=0; ii<NUM_MEN; ii++){
		m_AddrHtSend[ii].sin_family			= AF_INET;
		m_AddrHtSend[ii].sin_addr.s_addr	= INADDR_ANY;
		m_AddrHtSend[ii].sin_port			= htons( nSendPort[ii] );
	}
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
CmdSender::~CmdSender(void)
{

	//// �\�P�b�g���
	if(INVALID_SOCKET != m_SockHtSend)		closesocket(m_SockHtSend);

	// ���
	WSACleanup();
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int CmdSender::ThreadFirst()
{
	// �C�x���g�ݒ�
	HANDLE hArray[] = { gque_SendUdp.g_evSem
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);

	return 0;
}
//------------------------------------------
// �X���b�h�I���O���� (�X���b�h�I�����O�ɃR�[���o�b�N)
//------------------------------------------
int CmdSender::ThreadLast()
{

	return 0;
}
//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void CmdSender::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���		
	enum {	EV_SEND_HT = 0,							// �����p���R�[�_�[	UDP���M
			EV_END	};

	////// �V�O�i����������
	switch (nEventNo) {
//-----------------------------------------------------------------------------------------------
	case EV_SEND_HT:								// �����p���R�[�_�[��UDP���M
		if(true){
			SEND_UDP* pData = gque_SendUdp.GetFromHead();
			HtUdp_Send(pData->nTorB, pData->nPcId, pData->data, pData->nSize, pData->nCommand);
			delete pData;
		}
		break;
	
//-----------------------------------------------------------------------------------------------	
	default :
		// ���肦�Ȃ��I�I
		break;
	}
}

//=============================================
// UDP���M(�u���[�h�L���X�g) 
//---------------------------------------------
// int		TorB		: 0:�\�A1:���A-1:����
// char*	cData		: ���M�f�[�^
// int		nSize		: ���M�T�C�Y
// int		nCommand	: ���M�R�}���h
//=============================================
void CmdSender::HtUdp_Send(int TorB, char* cData, int nSize, int nCommand)
{
	HtUdp_Send(TorB, -1, cData, nSize, nCommand);
}


//==================================================
// UDP���M
//--------------------------------------------------
// int		TorB		: 0:�\�A1:���A-1:����
// int		pcid		: ���M����PC(0�I���W��)
//						�@-1���A�u���[�h�L���X�g���M		
// char*	cData		: ���M�f�[�^
// int		nSize		: ���M�T�C�Y
// int		nCommand	: ���M�R�}���h
//==================================================
void CmdSender::HtUdp_Send(int TorB, int nPcNo, char* cData, int nSize, int nCommand)
{
	_ASSERT(-1 <= nPcNo && NUM_SENDER > nPcNo);

	// ���M�������
	int ii_s, ii_e;
	if( -1 == TorB ){	ii_s = 0;	ii_e = 1;	}
	else			{	ii_s = ii_e = TorB;		}


	for(int ii = ii_s; ii<=ii_e; ii++){
		// ���ۂɑ��M
		if( -1 == nPcNo )	{ m_AddrHtSend[ii].sin_addr.s_addr = inet_addr(EPC_UDP_IP);	}
		else				{ m_AddrHtSend[ii].sin_addr.s_addr = inet_addr(m_cUniIp[nPcNo]); }
		int ret = sendto(m_SockHtSend, cData, nSize, 0, (SOCKADDR*)&m_AddrHtSend[ii], sizeof(m_AddrHtSend[ii]));

		// �������O���o��
		CString	sLogMsg;
		sLogMsg.Format("[%s] �R�}���h���M", my_sThreadName);
		sLogMsg += (0 == ii?"[�\��]":"[����]");
		switch(nCommand){
			case NCL_START:		sLogMsg += "[�����J�n(NC_START)]";			break;
			case NCL_STOP:		sLogMsg += "[������~(NC_STOP)]";			break;
			case NCL_PARAM:		sLogMsg += "[�p�����[�^�ύX(NC_PARAM)]";	break;
			case NCL_COILINFO:	sLogMsg += "[��ʏ�񑗐M(NC_COILINFO)]";	break;
			default:			sLogMsg += "[�s���ȃR�}���h]";				break;	
		}

		// �p�����[�^�ύX or ��ʏ�񑗐M�̏ꍇ�͍X�ɏ���ǉ�!
		if( NCL_PARAM == nCommand ){
			int		scode = ((NCL_CMD_PARAM *)cData)->scode;
			CString	sWk;
			sWk.Format("[scode=%d]", scode);
			sLogMsg += sWk;
		}else if(NCL_COILINFO == nCommand){
			char	cHotCoilNo[PRO_SIZE_COIL_NO + 1];
			memset(cHotCoilNo, 0x00, PRO_SIZE_COIL_NO + 1);
			memcpy(cHotCoilNo, ((TYPE_EPC_COIL *)((NCL_CMD_COILINFO *)cData)->coil_info)->cCoilNo, PRO_SIZE_COIL_NO );
			
			sLogMsg += "[�R�C��No=";
			sLogMsg += cHotCoilNo;
			sLogMsg += "]";
		}

		if( SOCKET_ERROR != ret){
			LOG(em_MSG), sLogMsg);
		}else{
			LOG(em_ERR), "%s[err_code=%d]",sLogMsg, GetLastError());
		}
	}

}

//*******************************************
// PFC�\�t�g�ɃR�}���h���M(���N���X����N��)
//*******************************************
//==========================================
// �����J�n�R�}���h�ʒm
//------------------------------------------
// int	TorB	: 0:�\�A1:���A-1:����
//==========================================
void CmdSender::SendCommand_Start(int TorB)
{
	NCL_CMD		typWk;
	typWk.cmd = NCL_START;
	//typWk.dlyExec = 0;

	SEND_UDP*	data = new SEND_UDP;
	memset(data, 0x00, sizeof(SEND_UDP));

	data->nTorB		= TorB;
	data->nPcId		= -1;
	data->nSize		= sizeof(NCL_CMD);
	data->nCommand	= NCL_START;
	memcpy(data->data, &typWk, data->nSize);


	gque_SendUdp.AddToTail(data);
}

//==========================================
// ������~�R�}���h�ʒm
//------------------------------------------
// int	TorB	: 0:�\�A1:���A-1:����
//==========================================
void CmdSender::SendCommand_Stop(int TorB)
{
	NCL_CMD		typWk;
	typWk.cmd = NCL_STOP;
	//typWk.dlyExec = 0;

	SEND_UDP*	data = new SEND_UDP;
	memset(data, 0x00, sizeof(SEND_UDP));

	data->nTorB		= TorB;
	data->nPcId		= -1;
	data->nSize		= sizeof(NCL_CMD);
	data->nCommand	= NCL_STOP;
	memcpy(data->data, &typWk, data->nSize);


	gque_SendUdp.AddToTail(data);
}

//==============================================
// �����\�ʏ�ԕύX�R�}���h�ʒm
//----------------------------------------------
// int		TorB	: 0:�\�A1:���A-1:����
// int		scode	: �����\�ʏ��
//==============================================
void CmdSender::SendCommand_Param(int TorB, int scode)
{
	NCL_CMD_PARAM	typWk;
	typWk.cmd  = NCL_PARAM;
	//typWk.dlyExec = 0;
	typWk.scode = scode;
	typWk.framenum = 0;

	SEND_UDP*	data = new SEND_UDP;
	memset(data, 0x00, sizeof(SEND_UDP));

	data->nTorB		= TorB;
	data->nPcId		= -1;
	data->nSize		= sizeof(NCL_CMD_PARAM);
	data->nCommand	= NCL_PARAM;
	memcpy(data->data, &typWk, data->nSize);


	gque_SendUdp.AddToTail(data);
}

//===================================================
// �R�C������ʒm
//---------------------------------------------------
// int				TorB		: 0:�\�A1:���A-1:����
// TYPE_EPC_COIL	typData		: �R�C�����
//===================================================
void CmdSender::SendCommand_CoilInf(int TorB, TYPE_EPC_COIL typData)
{
	BYTE			Info[sizeof(NCL_CMD_COILINFO) + sizeof(TYPE_EPC_COIL)];
	NCL_CMD_COILINFO&	typInfo = (NCL_CMD_COILINFO&)Info;
	typInfo.cmd = NCL_COILINFO;
	//typInfo.dlyExec = 0;
	typInfo.info_size = sizeof(TYPE_EPC_COIL);
	memcpy(typInfo.coil_info, &typData, typInfo.info_size);

	SEND_UDP*	data = new SEND_UDP;
	memset(data, 0x00, sizeof(SEND_UDP));
	data->nTorB		= TorB;
	data->nPcId		= -1;
	data->nSize		= sizeof(Info);
	data->nCommand	= NCL_COILINFO;
	memcpy(data->data, &typInfo, sizeof(NCL_CMD_COILINFO) + sizeof(TYPE_EPC_COIL));
	gque_SendUdp.AddToTail(data);
}
//==========================================
// �R�}���h���̂��擾
//------------------------------------------
// int	nCommand	: �R�}���h
// �ߒl	CString		: �R�}���h����
//==========================================
CString	CmdSender::GetCmdName(int nCommand)
{
	CString	sWk;

	switch(nCommand){
		case NCL_START:		sWk.Format("�����J�n(NC_START)");			break;
		case NCL_STOP:		sWk.Format("������~(NC_STOP)");			break;
		case NCL_PARAM:		sWk.Format("�p�����[�^�ύX(NC_PARAM)");		break;
		case NCL_COILINFO:	sWk.Format("��ʏ��ݒ�(NC_COILINFO)");	break;
		default:			sWk.Format("�s���ȃR�}���h(%d)", nCommand);	break;
	}

	return sWk;
}