#pragma once

// EPC�C���N���[�h
//#include "..\..\Include\EPC\NCcommand.h"								// ����R�}���h�̒�`
#include "..\..\Include\EPC\NCL2DETECT_if.h"							// I/F��`

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLib\Class\PipeManager.h"					// PIPE�N���X

// ���[�J��
#include "MainConst.h"													// ���N���X�萔�w�b�_�[

class CommandSender : public ThreadManager
{
//// ���J�萔
public :
	//------------------------------------------
	// �{�N���X�̃G���[
	//------------------------------------------
	enum EM_PIPE_ERR {
		CPIPE_NON = 0,							// �������
		CPIPE_ERR_COMMAND,						// �摜�����^�X�N����񓯊��ŏd��ُ킪�����Ă���

		CPIPE_OPEN_ERR,							// �I�[�v���Ŏ��s			
		CPIPE_READ_ERR,							// �Ǎ��݂Ŏ��s
		CPIPE_DISCONNECT_ERR					// �p�C�v������ɐؒf���ꂽ
	};

//// �����o�[�萔
private :
	//------------------------------------------
	// �Œ�萔
	//------------------------------------------
	static const int PIPE_ANSER_TIMEOUT				= 18000;//8000;		// �p�C�v�̉����^�C���A�E�g����
	static const int PIPE_RECVBUF_SIZE				= 512;				// �p�C�v�̍ő��M�T�C�Y
	static const int PIPE_SENDBUF_SIZE				= 1024;				// �p�C�v�̍ő呗�M�T�C�Y
	static const int PIPE_OPEN_INTERVAL				= 1000;				// �p�C�v�I�[�v���`�F�b�N�҂�����
	static const int PIPE_OPEN_RETRY				= 5;				// �p�C�v�I�[�v���`�F�b�N��



//// ���J�֐�
public:
	CommandSender(LPCSTR cPipeName);
	virtual ~CommandSender(void);


	//// �O���A�N�Z�X
	int SendCommand(BYTE *pSendData, DWORD nSendSize, void *pRecvRetc, int nRecvSize=sizeof(NCL_ANSWER), DWORD nWaitTime=PIPE_ANSER_TIMEOUT); // �R�}���h��t
	//int SendCommand(BYTE *pSendData, DWORD nSendSize, void *pRecvRetc, int nRecvSize=sizeof(int), DWORD nWaitTime=PIPE_ANSER_TIMEOUT); // �R�}���h��t
	void ReadPerData(int nOffset, void* pRecvRetc, int nRecvSize );

	bool		IsConnect()		   const { return m_bConnect; }			// �ڑ����
	// Set
	void		SetEvPipeOpen()	   { SetEvent(m_evOpenPipe); }			// PIPE�̐ڑ��v���V�O�i��
	void		SetEvPipeClose()   { SetEvent(m_evClosePipe); }			// PIPE�̐ؒf�v���V�O�i��
	void		SetRunExit()	   { m_bRunExit = true; }				// �I���������t���O�Z�b�g
	void		SetPipeName(LPCSTR pipe_name) {m_cPipeName=pipe_name;}	// PIPE���̕ύX 
	void		SetNgCam(bool bMode){ m_bNgCam = bMode;}

//// ���J�ϐ�
public : 
	// �X���b�h�L���[
	ThreadQueue<CPIPE_ERROR>	gque_CPipeErr;							// �󂯓n���R�}���h�p�C�v�G���[�p�L���[


//// �����o�[�֐�
private:		
	// �X���b�h�����`
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	

//// �����o�[�ϐ�
private:

	// �e�C���X�^���X
	PipeManager*			mcls_pPipe;									// �p�C�v�N���X


	// �V�O�i�� (PIPE����)
	HANDLE					m_evOpenPipe;								// PIPE�̐ڑ��p�C�x���g
	HANDLE					m_evClosePipe;								// PIPE�̐ؒf�p�C�x���g
	HANDLE					m_evSendStart;								// ��M�J�n�C�x���g�F���p
	HANDLE					m_evSendEnd;								// ���M�����C���x�g�F���p


	// PIPE�֘A
	bool					m_bConnect;									// �ڑ����
	CString					m_cPipeName;								// �ڑ�����PIPE�p�X
	char					m_cRecvWk[PIPE_RECVBUF_SIZE];				// ��M�o�b�t�@
	char					m_cRecvBuf[PIPE_RECVBUF_SIZE];				// ��M�o�b�t�@
	char					m_cSendBuf[PIPE_SENDBUF_SIZE];				// ���M�o�b�t�@
	DWORD					m_nSendSize;								// ���M�f�[�^��
	OVERLAPPED				m_ovlRecv;									// ��M�p�񓯊�IO

	// ���̑��������
	bool					m_bRunExit;									// �I��������
	bool					m_bNgCam;									// �ُ�J�����S�� true

};
