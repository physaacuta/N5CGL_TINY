#include "StdAfx.h"
#include "CommandRecvre.h"

CommandRecvre::CommandRecvre(void) :
mcls_pLog(NULL)
{

	m_hPipe = CreateNamedPipe(	EPC_PIPE_COMMAND, //NTCNTRL,
								PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
								PIPE_WAIT | PIPE_READMODE_MESSAGE | PIPE_TYPE_MESSAGE,
								1,	256, 256, 0, NULL);
	ASSERT(INVALID_HANDLE_VALUE != m_hPipe);

	// ���M�p��OVERLAPPED
	memset(&m_ovlSend, 0, sizeof(OVERLAPPED));
	m_ovlSend.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL); // manual, reset
	ASSERT(NULL != m_ovlSend.hEvent);

	for (int ii = 0; ii<NCL_ncam_max; ii++) {
		m_temp[ii] = (float)(10 * (ii + 1));
		m_volt[ii] = 100;
		for (int jj = 0; jj<3; jj++) {
			m_exptime[jj][ii] = (float)(100 * (ii + 1) + 10 * (jj + 1));
			m_gain[jj][ii] = (float)(1 * (ii + 1) + 0.1 * (jj + 1));
		}
	}

}

CommandRecvre::~CommandRecvre(void)
{
	CloseHandle(m_hPipe);
	CloseHandle(m_ovlSend.hEvent);
}



//------------------------------------------
// �X���b�h
//------------------------------------------
UINT WINAPI CommandRecvre::MainThread(LPVOID param)
{
	////// ���C���C���X�^���X���o��
	CommandRecvre *p = (CommandRecvre *)param;					// ���N���X�̃C���X�^���X���擾
	p->SetTid();								// ���X���b�hID�Z�b�g

	OVERLAPPED ovlRecv;			// �񓯊�IO
	OVERLAPPED ovlConnect;
	OVERLAPPED ovlSend;
	memset(&ovlRecv, 0, sizeof(OVERLAPPED));
	memset(&ovlConnect, 0, sizeof(OVERLAPPED));
	memset(&ovlSend, 0, sizeof(OVERLAPPED));
	ovlRecv.hEvent		= CreateEvent(NULL, TRUE, FALSE, NULL); // manual, reset
	ovlConnect.hEvent	= CreateEvent(NULL, FALSE, FALSE, NULL); // auto, reset
	ovlSend.hEvent		= CreateEvent(NULL, TRUE, FALSE, NULL); // manual, reset

	BYTE readdata[512];
	DWORD dwTransByte, dwRetCode;
	BOOL bRet;

	// WaitForMultipleObjects�C�x���g
	HANDLE	hArray[] = {p->my_evStop,
						ovlConnect.hEvent,
						ovlRecv.hEvent
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// �X���b�h�I��
			EV_OPEN,						// �I�[�v��
			EV_RECV_END,					// ��M�����ʒm
			EV_END	};




	// �ŏ��̐ڑ�
	if ( ! ConnectNamedPipe(p->m_hPipe, &ovlConnect)) {
		dwRetCode = GetLastError();
		ASSERT(ERROR_IO_PENDING == dwRetCode || ERROR_PIPE_CONNECTED == dwRetCode);
		if (ERROR_PIPE_CONNECTED == dwRetCode) {

			if ( ! ReadFile(p->m_hPipe, readdata, sizeof(readdata), NULL, &ovlRecv)) {
				dwRetCode = GetLastError();
				ASSERT(ERROR_IO_PENDING == dwRetCode);
			}
		}
	}
	pLOG(em_MSG), "[CommandRecvre] �X���b�h�J�n [0x%X]", p->GetTid());
	while (1) {
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);
		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// �X���b�h�I��
			pLOG(em_MSG), "[CommandRecvre] �X���b�h�I�� [0x%X]", p->GetTid());
			DisconnectNamedPipe(p->m_hPipe);
			CloseHandle(ovlRecv.hEvent);
			CloseHandle(ovlConnect.hEvent);
			CloseHandle(ovlSend.hEvent);
			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_OPEN:												// �I�[�v��
			pLOG(em_MSG), "[CommandRecvre] �R�}���hPipe �I�[�v������");
			if (!ReadFile(p->m_hPipe, readdata, sizeof(readdata), NULL, &ovlRecv)) {
				dwRetCode = GetLastError();
				ASSERT(ERROR_IO_PENDING == dwRetCode);
			}
			break;

//-----------------------------------------------------------------------------------------------
		case EV_RECV_END:											// ��M�����ʒm
			bRet = GetOverlappedResult(p->m_hPipe, &ovlRecv, &dwTransByte, TRUE);
			ResetEvent(ovlRecv.hEvent);
			//pLOG(em_MSG), "[CommandRecvre] �R�}���h��M [%d]", readdata[0]);
			if (bRet) {
				p->m_RetCode = 0;
				memset(&p->m_RetState, 0x00, sizeof(p->m_RetState));

				// �R�}���h����
				int nRetc;
				memcpy(&nRetc, readdata, 4);
				if(nRetc != NCL_QUERY_STATUS) {		// ��Ԗ₢���킹�ȊO�͑����������M
					// ���^�[���R�[�h���M
					WriteFile(p->m_hPipe, &p->m_RetCode, sizeof(p->m_RetCode), NULL, &ovlSend);
				}

				p->GetCommandExec(nRetc, readdata);

				if(nRetc == NCL_QUERY_STATUS) {		// ��Ԗ₢���킹�́A�₢���킹���ʂ�ҏW��ɑ��M
					pLOG(em_INF), "[CommandRecvre] �G���W����� %d num:%d", p->m_RetState.state, p->m_RetState.camnum);
					for (DWORD ii = 0; ii < p->m_RetState.camnum; ii++)
					{
						pLOG(em_INF), " �J������� [ID:%d][���x:%.3f][�I��:%.3f %.3f %.3f][�Q�C��:%.3f %.3f %.3f][�d��:%.3f]"
							, p->m_RetState.camid[ii]
							, p->m_RetState.temp[ii]
							, p->m_RetState.exptime[0][ii]
							, p->m_RetState.exptime[1][ii]
							, p->m_RetState.exptime[2][ii]
							, p->m_RetState.gain[0][ii]
							, p->m_RetState.gain[1][ii]
							, p->m_RetState.gain[2][ii]
							, p->m_RetState.volt[ii]
							);
					}

					// ���^�[���R�[�h���M
					WriteFile(p->m_hPipe, &p->m_RetState, sizeof(p->m_RetState), NULL, &ovlSend);
				}


				// ���̃��[�h���s
				ReadFile(p->m_hPipe, readdata, sizeof(readdata), NULL, &ovlRecv);
			} else {
				pLOG(em_ERR), "[CommandRecvre] �R�}���hPipe �ؒf");
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
// �R�}���h���M
//------------------------------------------
int CommandRecvre::SendCommand(LPVOID senddata, DWORD sendsize)
{
	WriteFile(m_hPipe, senddata, sendsize, NULL, &m_ovlSend);
	return 0;
}

//------------------------------------------
// �R�}���h����
// int command �R�}���h
// BYTE* pBuf ��M�f�[�^�{��
//------------------------------------------
void CommandRecvre::GetCommandExec(int command, BYTE* pBuf)
{
	LOG(em_MSG), "[CommandRecvre] �R�}���h��M [%d]", command);

	if(NCL_INIT == command) {
		NCL_CMD_INIT* cmd = (NCL_CMD_INIT*)pBuf;

		LOG(em_MSG), " -->NCL_INIT:[num:%d camset:%d %d]", cmd->camnum, cmd->camid[0], cmd->camid[1]);
		mcls_pData->SetCamset(cmd->camnum, (DWORD *)cmd->camid );

		mcls_pData->InitFrameNo();
		mcls_pData->InitPosCmd();

		// ���L�������ւ̃J�������̏�������
		FIRST_SHMEM_INFOv1 info;
		memset(&info, 0, sizeof(FIRST_SHMEM_INFOv1));
		memcpy(&info.first_smem_req_id, &ID_FIRST_SHMEM_INFO_IDv1, sizeof(GUID));
		mcls_pData->GetCamset((int *)&info.camnum, &info.camid[0]);
		for (DWORD ii = 0; ii < info.camnum; ii++)
		{
			for (int jj = 0; jj < 3; jj++)
			{
				info.gain[ii][jj] = GetGain(jj, ii);
				info.exptime[ii][jj] = GetExp(jj, ii);
			}
			info.volt[ii] = GetVolt(ii);
			info.temp[ii] = GetTemp(ii);

			LOG(em_MSG), " �J������� [ID:%d][���x:%.3f][�I��:%.3f %.3f %.3f][�Q�C��:%.3f %.3f %.3f][�d��:%.3f]"
				, info.camid[ii]
				, info.temp[ii]
				, info.exptime[ii][0]
				, info.exptime[ii][1]
				, info.exptime[ii][2]
				, info.gain[ii][0]
				, info.gain[ii][1]
				, info.gain[ii][2]
				, info.volt[ii]
				);
		}
		mcls_pData->SetFirstSHMem(&info);
	}


	if(NCL_EXIT == command) {
		// ��ʂ̏I���v��
		PostThreadMessage(GetMainPostTid(), WM_QUIT, 0, 0);
	}


	if(NCL_START == command) {


	}

	if(NCL_STOP == command) {
	}


	if(NCL_SET_POSITION == command) {
		// �����������v��
		mcls_pData->SetNextPosCmd( (NCL_CMD_SET_POSITION*)pBuf );
	}

	if(NCL_CLEAR_QUEUE == command) {
		// �����s�ݒ�̃L�����Z��
		mcls_pData->ClearPosCmd();
	}

	if(NCL_QUERY_STATUS == command) {
		// ������X�e�[�^�X�v��
		m_RetState.retcode = NCL_SUCCESS;
		m_RetState.cmd = NCL_QUERY_STATUS;
		m_RetState.state = NCL_RUNNING;
		mcls_pData->GetCamset((int*)&m_RetState.camnum, &m_RetState.camid[0]);
		//for(int ii=0; ii<m_RetState.camnum; ii++) {
		//	m_RetState.temp[ii] = 10 * (ii+1);
		//	m_RetState.volt[ii] = 100;
		//	for(int jj=0; jj<3; jj++) {
		//		m_RetState.exptime[jj][ii] = 100 * (ii+1) + 10 * (jj+1);
		//		m_RetState.gain[jj][ii] = 1 * (ii+1) + 0.1 * (jj+1);
		//	}
		//}
		for(int ii=0; ii<(int)m_RetState.camnum; ii++) {
			m_RetState.temp[ii] = m_temp[ii];
			m_RetState.volt[ii] = m_volt[ii];
			for(int jj=0; jj<3; jj++) {
				m_RetState.exptime[jj][ii] = m_exptime[jj][ii];
				m_RetState.gain[jj][ii] = m_gain[jj][ii];
			}
		}
	}

	if (NCL_RESUME_START == command)
	{
		NCL_CMD_RESUME_START* cmd = (NCL_CMD_RESUME_START*)pBuf;
		LOG(em_MSG), "[CommandRecvre] -->�r���Q���J�n�t���[�� [%d]", cmd->start_frame_num);
	}

	return ;
}
