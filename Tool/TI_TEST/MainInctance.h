// *********************************************************************************
//	���C���Ǘ��X���b�h
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//		
// *********************************************************************************

#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\DefectLib\KizuToSend.h"							// ����˓����ԒʐM �N���X


// �e���[�J�[�X���b�h
#include "MainConst.h"													// ���N���X�萔�w�b�_�[
#include "ParamManager.h"												// �p�����[�^�N���X
#include "TestManager.h"												// �e�X�g�N���X


// �S�������
#include "..\..\Library\DefectLib\SockOneToAnySend.h"					// ��]���i�`���̑��M�N���X
#include "..\..\Include\OutIF_HT_RAW.h"									// �S����`
#include "CullManager.h"

using namespace KizuLib;


class MainInctance : public ThreadManager
{
//// �����o�[�萔
private :
	//=============================
	// �{�N���X�󂯓n���p�N���X
	//=============================
	//// ���[�����󂯓n���L���[
	struct DELIVERY_MAIL {
		ENUM_MAININCTANCE	emNo;										// �C�x���gNo
		COMMON_QUE			que;										// �t�����
	};

	static const int PARAM_TIMEOUT_AUTO_START = 10 * 1000;				// �����J�n �����J�n����[ms]


//// ���J�֐�
public:
	MainInctance(void);
	virtual ~MainInctance(void);

	void Start() { SetEvent(m_evThStart); }								// �X���b�h�J�n (MainInctance�̎������̓I�[�o�[���C�h)

	BOOL Init();														// ����������
	void Exit();														// �I������


	// �O���A�N�Z�X
	void SetDeliveryMail(ENUM_MAININCTANCE emNo, COMMON_QUE * que);		// �X���b�h�L���[�Ƀ��[���f�[�^�o�^
	void SetLogMgrCull(LogFileManager* pCls) { mcls_pLogCull = pCls; }	// �S�������p���O�N���X

	void	SetEvKenStart() { SetEvent(m_evKenStart); }					// �V�O�i���Z�b�g �J�n
	void	SetEvKenStop() { SetEvent(m_evKenStop); }					// �V�O�i���Z�b�g ��~

	void	SetMen(int nMen)	 { m_nMen = nMen; }						// ������

	int		GetSysMode() { return m_nSysMode; }							// �������(0:��~ 1:�J�n)

	void	Send_TiTest_Rendo(int nCycle);								// �S���ۑ������p�c�[���A���N�� �J�n�^��~
	void	Send_TiTest_FrameNo(int nFrameNo, int nCycleTime);			// �S���ۑ������p�c�[���A���N�� �t���[��No������

//// ���J�ϐ�
public:

	// �e���[�J�[�X���b�h
	ParamManager			gcls_PrmMgr;								// �p�����[�^�N���X
	TestManager*			gcls_pTest;									// TestManager�C���X�^���X

	int						g_nNumCam;									// �J�����䐔
	int						g_nCamStart;								// �J�����J�n�ʒu


//// �����o�[�֐�
private:		
	// �X���b�h�����`
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	

//// �����o�[�ϐ�
private:
	// �e���[�J�[�X���b�h
	LogFileManager*				mcls_pLogCull;							// �S�������p���O�N���X

	// ��������
	ThreadQueue<DELIVERY_MAIL>	mque_DeliMail;							// ���[�����󂯓n���L���[
	HANDLE						m_evThStart;							// MainInctance�X���b�h�̎��s�v��
	HANDLE						m_evKenStart;							// �����J�n		
	HANDLE						m_evKenStop;							// ������~		
	HANDLE						m_evTimerExec;							// ���������J�n�^�C�}�[�C�x���g


	// �p�����[�^
	int			 				m_nCamPair;								// ���݂̃J�����y�A 1�` (0:�o�b�N�A�b�v=���蓖�Ė���)
	int							m_nMen;									// ���݂̖� (-1:�s��  0:�\  1:��)
	int							m_nNowScode[NUM_MEN];					// ���݂̌����\�ʏ��No(0:�Ώ�Scode)
	EM_DIV_HANTEI				m_emKindHt;								// ����@�\
	DWORD						m_nAppParam;							// APP�ُ퍀�ڃp�����[�^
	bool						m_bNgCam;								// �ُ�J�����S�� true

	int							m_nSysMode;								// �������(0:��~ 1:�J�n)

//// �S���摜�p
public:
	SockOneToAnySend*				gcls_pSockRpi[NUM_CAM_POS];	// ����ˑS�������ԒʐM �N���X
	ThreadLoopQueue<HT_RPI_FDATA>*	gque_pSockRpi[NUM_CAM_POS];	// �t���[���摜�󂯓n���L���[(��]���i�`��)	[LIB��AP��] �� �L���[�T�C�Y��AP����w�肵�������߁A�|�C���^�ɂ��Ă���
	CullManager*					gcls_pCull[NUM_CAM_POS];		// �Ԉ����X���b�h

	//SockOneToAnySend*				gcls_pSockRpi[FRAME_HT_SEND_PORT];	// ����ˑS�������ԒʐM �N���X
	//ThreadLoopQueue<HT_RPI_FDATA>*	gque_pSockRpi[FRAME_HT_SEND_PORT];	// �t���[���摜�󂯓n���L���[(��]���i�`��)	[LIB��AP��] �� �L���[�T�C�Y��AP����w�肵�������߁A�|�C���^�ɂ��Ă���
	//CullManager*					gcls_pCull[FRAME_HT_SEND_PORT];		// �Ԉ����X���b�h

private:
	void QueueAlloc(int idx, int n, int const* p);						// �S���������M�p�̉�]���i�L���[�m��
	void QueueFree();													// �S���������M�p�̉�]���i�L���[���


};
