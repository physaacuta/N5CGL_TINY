// *********************************************************************************
//	���C���Ǘ��X���b�h
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//		
// *********************************************************************************

#pragma once

// ���Œ�
#include "..\..\Include\KizuSmyu.h"										// GOT�C���N���[�h

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[

// �r�����C�u���� �C���N���[�h
//#include "..\..\Library\KizuLibMFC\Class\DivNameManager.h"				// �r�����C�u�����w�b�_�[

// �e���[�J�[�X���b�h
#include "MainConst.h"
#include "PlgControlManager.h"
#include "PioControlManager.h"

#include "SmyuPlgManager.h"
#include "SmyuTimManager.h"
#include "SmyuEvtManager.h"


using namespace KizuLib;

class MainInctance : public ThreadManager
{
private :
	//=============================
	// �{�N���X�󂯓n���p�N���X
	//=============================
	//// ���[�����󂯓n���L���[
	struct DELIVERY_MAIL {
		ENUM_MAININCTANCE	emNo;										// �C�x���gNo
		COMMON_QUE			que;										// �t�����
	};



//// ���J�֐�
public:
	MainInctance(void);
	virtual ~MainInctance(void);

	void Start() { SetEvent(m_evThStart); }								// �X���b�h�J�n (MainInctance�̎������̓I�[�o�[���C�h)

	BOOL Init();														// ����������
	void Exit();														// �I������

	// �O���A�N�Z�X
	void SetDeliveryMail(ENUM_MAININCTANCE emNo, COMMON_QUE * que);		// �X���b�h�L���[�Ƀ��[���f�[�^�o�^


//// ���J�ϐ�
public:
	// �e���[�J�[�X���b�h
	PlgControlManager*			gcls_pPlg;								// PLG�����Ǘ��N���X
	PioControlManager*			gcls_pPio;								// PIO�Ǘ��t�@���N�V����

	// �V�~�����[�^�p
	SmyuPlgManager*				gcls_pSmyuPlg[SMYU_STORY_MAX_FILE];		// �����쓮�V�i���I�p
	SmyuTimManager*				gcls_pSmyuTim[SMYU_STORY_MAX_FILE];		// ���ԋ쓮�V�i���I�p
	SmyuEvtManager*				gcls_pSmyuEvt;							// �C�x���g�쓮�V�i���I�p



//// �����o�[�֐�
private:		
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast() { return 0; }										// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	

	void ExecInit();													// ������
	void ExecStart();													// �V�i���I�J�n
	void ExecStop();													// �V�i���I��~


//// �����o�[�ϐ�
private:
	// ��������
	ThreadQueue<DELIVERY_MAIL>	mque_DeliMail;							// ���[�����󂯓n���L���[
	HANDLE						m_evThStart;							// MainInctance�X���b�h�̎��s�v��




};