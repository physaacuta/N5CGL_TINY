// *********************************************************************************
//	���C���Ǘ��X���b�h
//	[Ver]
//		Ver.01    2012/08/30
//
//	[����]
//		
// *********************************************************************************

#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[


// �e���[�J�[�X���b�h
#include "MainConst.h"
#include "SockManager.h"												// �ʐM���䃏�[�J�[�X���b�h
#include "SockDataExec.h"												// ����M�f�[�^�󂯓n���N���X


using namespace KizuLib;


class MainInctance : public ThreadManager
{
private :
	//=============================
	// �{�N���X�󂯓n���p�N���X
	//=============================
	//// ���[�����󂯓n���L���[
	struct DELIVERY_MAIL {
		QUE_SEND_STAT		emNo;										// �C�x���gNo
		COMMON_QUE			que;										// �t�����
	};



//// ���J�֐�
public:
	MainInctance(void);
	virtual ~MainInctance(void);

	void Start() { SetEvent(m_evThStart); };							// �X���b�h�J�n (MainInctance�̎������̓I�[�o�[���C�h)

	BOOL Init();														// ����������
	void Exit();														// �I������

	// �O���A�N�Z�X
	void SetDeliveryMail(QUE_SEND_STAT emNo, COMMON_QUE * que);			// �X���b�h�L���[�Ƀ��[���f�[�^�o�^


//// ���J�ϐ�
public:
	// �e���[�J�[�X���b�h
	SockManager*		gcls_pSock;										// �ʐM���䃏�[�J�[�X���b�h
	SockDataExec*		gcls_pData;										// ����M�f�[�^�󂯓n���N���X



	// �󂯓n���L���[
	ThreadQueue<DELIVERY_MAIL>	mque_DeliMail;							// ���[�����󂯓n���L���[

//// �����o�[�֐�
private:		
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast() { return 0; }										// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	

//// �����o�[�ϐ�
private:
	HANDLE						m_evThStart;							// MainInctance�X���b�h�̎��s�v��


};
