#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLib\Class\PipeManager.h"					// PIPE�N���X

// EPC�C���N���[�h
#include "..\..\Include\EPC\NCL2DETECT_if.h"							// I/F��`

// ���[�J��
#include "MainConst.h"													// ���N���X�萔�w�b�_�[

class DataRecver : public ThreadManager
{
//// ���J�萔
public :
	//------------------------------------------
	// �{�N���X�̃G���[
	//------------------------------------------
	enum EM_PIPE_ERR {
		CPIPE_NON = 0,							// �������
		CPIPE_OPEN_ERR,							// �I�[�v���Ŏ��s			
		CPIPE_READ_ERR,							// �Ǎ��݂Ŏ��s
		CPIPE_ERR_SMEM,							// ���L�������I�[�v�����s
		CPIPE_DISCONNECT_ERR					// �p�C�v������ɐؒf���ꂽ
	};

//// �����o�[�萔
private :
	//------------------------------------------
	// �Œ�萔
	//------------------------------------------
	static const int PIPE_ANSER_TIMEOUT				= 3000;				// �p�C�v�̉����^�C���A�E�g����
	static const int PIPE_RECVBUF_SIZE				= 256;				// �p�C�v�̍ő��M�T�C�Y
	static const int PIPE_OPEN_INTERVAL				= 1000;				// �p�C�v�I�[�v���`�F�b�N�҂�����
	static const int PIPE_OPEN_RETRY				= 5;				// �p�C�v�I�[�v���`�F�b�N��


//// ���J�֐�
public:
	DataRecver(LPCSTR cPipeName, LPCSTR cSmemName);
	virtual ~DataRecver(void);


	//// �O���A�N�Z�X
	void		SendBack(DWORD ofs);									// �I�t�Z�b�g�̈�̕ԋp�֐�
	bool		IsConnect()		   const { return m_bConnect;}			// �ڑ����
	bool		IsFirstInfoExists();									// ���L���������̃J�������L��
	void		HelcyStat(ENG_STAT *st);								// ���L���������̃J�������擾
	// Get
	BYTE*		GetMapPointer()	   const { return mtbl_pMap; }			// ���L�������̐擪�A�h���X

	// Set
	void		SetEvPipeOpen()	   { SetEvent(m_evOpenPipe); }			// PIPE�̐ڑ��v���V�O�i��
	void		SetEvPipeClose()   { SetEvent(m_evClosePipe); }			// PIPE�̐ؒf�v���V�O�i��
	void		SetRunExit()	   { m_bRunExit = true; }				// �I���������t���O�Z�b�g
	void		SetPipeName(LPCSTR pipe_name) {m_cPipeName=pipe_name;}	// PIPE���̕ύX 
	void		SetSmemName(LPCSTR smem_name) {m_cSmemName=smem_name;}	// ���L���������̕ύX 



//// ���J�ϐ�
public:
	// �󂯓n���L���[
	ThreadQueue<BYTE>			gque_Deli;								// ���L����������p�C�v���M�f�[�^�󂯓n��
	ThreadQueue<CPIPE_ERROR>	gque_CPipeErr;							// �󂯓n���R�}���h�p�C�v�G���[�p�L���[


//// �����o�[�֐�
private:		
	// �X���b�h�����`
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	

//// �����o�[�ϐ�
private:


	//// �e�C���X�^���X
	PipeManager*			mcls_pPipe;									// �p�C�v�Ǘ��N���X

	//// ��������
	CRITICAL_SECTION		m_csLock;									// �N���e�B�J���Z�N�V����


	// �V�O�i�� (PIPE����)
	HANDLE					m_evOpenPipe;								// PIPE�̐ڑ��p�C�x���g
	HANDLE					m_evClosePipe;								// PIPE�̐ؒf�p�C�x���g


	// PIPE�֘A
	bool					m_bConnect;									// �ڑ����
	CString					m_cPipeName;								// �ڑ�����PIPE�p�X
	BYTE					m_cRecvBuf[PIPE_RECVBUF_SIZE];				// ��M�o�b�t�@
	OVERLAPPED				m_ovlSend;									// �񓯊����M�p�̃I�[�o�[���b�v�\����
	OVERLAPPED				m_ovlRecv;									// ��M�p�񓯊�IO

	// ���L�������֘A
	CString					m_cSmemName;								// ���L����������
	HANDLE					m_hMap;										// �t���[�����󂯓n���p���L������ �n���h��
	BYTE*					mtbl_pMap;									// �t���[�����󂯓n���p���L������ �f�[�^

	// ���̑��������
	bool					m_bRunExit;									// �I��������
	bool					m_bFirstInfoExists;							// ���L���������̃J�������L��

};
