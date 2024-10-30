#include "StdAfx.h"
#include "EdgeRestrictSender.h"


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
EdgeRestrictSender::EdgeRestrictSender(void) :
ThreadManager("EdgeRestrictSender")
{

}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
EdgeRestrictSender::~EdgeRestrictSender(void)
{

}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int EdgeRestrictSender::ThreadFirst()
{
	// �|�[�gNo���p�����[�^����擾���邽�߁A�R���X�g���N�^�ł͎��{�ł��Ȃ�

	// ���M�p�@����|�[�gNo
	int nSendPort = mcls_pParam->GetParamCommon().nEdgeRestrictPort;
	_ASSERT( 0 != nSendPort );

	////====================================
	//// UDP�ݒ�

	//---------------------------
	// ����PC ���M�\�P�b�g
	WSADATA wsaData;
	WORD wVersionRequired = MAKEWORD(2, 2);
	WSAStartup(wVersionRequired, &wsaData);
	m_SockHtSend = (int)socket(AF_INET, SOCK_DGRAM, 0);
	_ASSERT(INVALID_SOCKET != m_SockHtSend);
	m_AddrHtSend.sin_family			= AF_INET;
	m_AddrHtSend.sin_addr.s_addr	= inet_addr(EPC_UDP_IP_EDGE);
	m_AddrHtSend.sin_port			= htons( nSendPort );

	// �C�x���g�ݒ�
	HANDLE hArray[] = { gque_SendUdp.g_evSem
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);

	return 0;
}
//------------------------------------------
// �X���b�h�I���O���� (�X���b�h�I�����O�ɃR�[���o�b�N)
//------------------------------------------
int EdgeRestrictSender::ThreadLast()
{

	//// �\�P�b�g���
	if(INVALID_SOCKET != m_SockHtSend)		closesocket(m_SockHtSend);

	// ���
	WSACleanup();

	return 0;
}
//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void EdgeRestrictSender::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���		
	enum {	EV_SEND_HT = 0,							// �G�b�W���o�͈͎w�� UDP���M
			EV_END	};

	////// �V�O�i����������
	switch (nEventNo) {
//-----------------------------------------------------------------------------------------------
	case EV_SEND_HT:								// �G�b�W���o�͈͎w�� UDP���M
		if(true){
			NCL_EDGE_RESTRICT_DATA* pData = gque_SendUdp.GetFromHead();
			int ret = sendto(m_SockHtSend, (const char *)pData, sizeof(NCL_EDGE_RESTRICT_DATA), 0, (SOCKADDR*)&m_AddrHtSend, sizeof(m_AddrHtSend));
#ifdef DEBUG_LOG_EDGE_RESTRICT
			LOG(em_MSG), "[%s] �G�b�W���o�͈͎w�� UDP���M �t���[��No:%05d [<%s> L(%.1f�`%.1fmm) R(%.1f�`%.1fmm)] [<%s> L(%.1f�`%.1fmm) R(%.1f�`%.1fmm)]", 
				my_sThreadName, pData->frame_num,
				(TRUE == pData->limit[0].use_limit ? "�͈͎w��":"����G�b�W"), pData->limit[0].left_outer, pData->limit[0].left_inner, pData->limit[0].right_inner, pData->limit[0].right_outer,
				(TRUE == pData->limit[1].use_limit ? "�͈͎w��":"����G�b�W"), pData->limit[1].left_outer, pData->limit[1].left_inner, pData->limit[1].right_inner, pData->limit[1].right_outer);
#endif
				memcpy(&m_strLastSendData, pData, sizeof(NCL_EDGE_RESTRICT_DATA));

			delete pData;
		}
		break;
	
//-----------------------------------------------------------------------------------------------	
	default :
		// ���肦�Ȃ��I�I
		break;
	}
}

//*******************************************
// PFC�\�t�g��UDP���M(���N���X����N��)
//*******************************************
//==========================================
// �摜�����G���W���փG�b�W�ʒu��񑗐M
//------------------------------------------
// int		nFrame		: �t���[��No
// int*		nUseLimit	: �G�b�W����t���O(0:����G�b�W, 1:�G�b�W���o�͈͎w��) [0:�\ 1:��]
// float*	dEdge_Left	: ���G�b�W�ʒu[0:�\ 1:��]
// float*	dEdge_Right	: �E�G�b�W�ʒu[0:�\ 1:��]
//==========================================
void EdgeRestrictSender::SendEdgeRestrict(int nFrame, int* nUseLimit, float* dEdge_Left, float* dEdge_Right)
{
	NCL_EDGE_RESTRICT_DATA*	data = new NCL_EDGE_RESTRICT_DATA;
	memset(data, 0x00, sizeof(NCL_EDGE_RESTRICT_DATA));

	int		nType			= mcls_pParam->GetCommonMast().Edge_Restrict.nType;				// �G�b�W���o_���{���
	float	dDetectRange	= mcls_pParam->GetCommonMast().Edge_Restrict.dDetectRange;		// �G�b�W���o_���o�͈�

	data->frame_num = nFrame;

	if(0 != nType) {		// �G�b�W���o�͈� �w��L��̏ꍇ
		for(int ii=0; ii<NUM_MEN; ii++) {
				// �G�b�W���o��̐M�����m�F
				float dEdge[2];		// �G�b�W�ʒu[0:DS��,1:WS��]
				switch (nType) {
				//-------------------------
				case 1:				// �����G�b�W�͈͎w��
					if (0 == nUseLimit[ii]) {
						data->limit[ii].use_limit = FALSE;										// FALSE:����G�b�W TRUE:�G�b�W���o�͈͎w��
						continue;
					} else {
						dEdge[0] = dEdge_Left[ii];
						dEdge[1] = dEdge_Right[ii];
					}
					break;
				//-------------------------
				default:				// ���̑�
					dEdge[0] = dEdge_Left[ii];
					dEdge[1] = dEdge_Right[ii];
					break;
				}

				// �f�[�^�Z�b�g
				data->limit[ii].use_limit = TRUE;												// FALSE:����G�b�W TRUE:�G�b�W���o�͈͎w��
				data->limit[ii].left_outer  = dEdge[0]  - (float)(dDetectRange * 0.5);	// DS���G�b�W�͈�(�O��)
				data->limit[ii].left_inner  = dEdge[0]  + (float)(dDetectRange * 0.5);	// DS���G�b�W�͈�(����)
				data->limit[ii].right_inner = dEdge[1] - (float)(dDetectRange * 0.5);	// WS���G�b�W�͈�(����)
				data->limit[ii].right_outer = dEdge[1] + (float)(dDetectRange * 0.5);	// WS���G�b�W�͈�(�O��)
		}
	}
	gque_SendUdp.AddToTail(data);
}

