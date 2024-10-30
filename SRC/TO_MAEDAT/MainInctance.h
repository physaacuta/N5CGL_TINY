// *********************************************************************************
//	���C���Ǘ��X���b�h
//	[Ver]
//		Ver.01    2012/09/06
//
//	[����]
//		
// *********************************************************************************

#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[

// �e���[�J�[�X���b�h
#include "MainConst.h"
#include "PreProcManager.h"


using namespace KizuLib;


class MainInctance : public ThreadManager
{
private :
	//=============================
	// �{�N���X�󂯓n���p�N���X
	//=============================
	//// ���[�����󂯓n���L���[
	struct DELIVERY_MAIL {
		ENUM_MAEDATOPERATION	emNo;										// �C�x���gNo
		COMMON_QUE				que;										// �t�����
	};



//// ���J�֐�
public:
	MainInctance(void);
	virtual ~MainInctance(void);

	void Start() { SetEvent(m_evThStart); }								// �X���b�h�J�n (MainInctance�̎������̓I�[�o�[���C�h)

	BOOL Init();														// ����������
	void Exit();														// �I������

	// �O���A�N�Z�X
	void SetDeliveryMail(ENUM_MAEDATOPERATION emNo, COMMON_QUE * que);		// �X���b�h�L���[�Ƀ��[���f�[�^�o�^


//// ���J�ϐ�
public:
	// �e���[�J�[�X���b�h
	PreProcManager*			gcls_PreProcManager;						// �O�H�����C���Ǘ��N���X
	FFSendManager			gcls_FFSend;								// FF���M�Ǘ�
	ParamManager			gcls_Param;									// �p�����[�^�Ǘ��N���X
	PreDBManager*			gcls_pPreDB;								// �O�H�����я��DB�o�^�Ǘ�
	PreDefectManager*       gcls_pPreDefect;							// �O�H���ҏW�N���X


//// �����o�[�֐�
private:		
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast() { return 0; }										// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent( int nEventNo );									// �X���b�h�C�x���g����
	

//// �����o�[�ϐ�
private:
	// ��������
	ThreadQueue<DELIVERY_MAIL>	mque_DeliMail;							// ���[�����󂯓n���L���[
	HANDLE						m_evThStart;							// MainInctance�X���b�h�̎��s�v��




};
