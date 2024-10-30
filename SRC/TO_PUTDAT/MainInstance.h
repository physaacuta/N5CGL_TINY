// *********************************************************************************
//	���C���Ǘ��X���b�h
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//		
// *********************************************************************************

#pragma once

// �C���N���[�h
#include <process.h>

// �r�� �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"						// ���[�J�[�X���b�h�}�l�[�W���[

// �r�����C�u���� �C���N���[�h
#include "..\..\Library\KizuODBC\OdbcBase.h"									// ODBC����N���X

// �e���[�J�[�X���b�h
#include "MainConst.h"															// ���N���X�萔�w�b�_�[
#include "MakeProManager.h"														// �v���R���������э쐬�Ǘ��N���X
#include "MakeFFManager.h"														// �e�e�������э쐬�Ǘ��N���X

using namespace KizuLib;

class MainInstance : public ThreadManager
{
private :
	//=============================
	// �{�N���X�󂯓n���p�N���X
	//=============================
	//// ���[�����󂯓n���L���[
	struct DELIVERY_MAIL
	{
		ENUM_MAININSTANCE		emNo;											// �C�x���gNo
		COMMON_QUE				que;											// �t�����
	};

//// ���J�֐�
public:
	MainInstance(void);
	virtual ~MainInstance(void);

	void Start() { SetEvent(m_evThStart); };									// �X���b�h�J�n (MainInstance�̎������̓I�[�o�[���C�h)

	BOOL Init();																// ����������
	void Exit();																// �I������
	
	//// �O���A�N�Z�X
	void SetDeliveryMail(ENUM_MAININSTANCE emNo, COMMON_QUE * que);				// �X���b�h�L���[�Ƀ��[���f�[�^�o�^

//// ���J�ϐ�
public:
	//// �e���[�J�[�X���b�h
	MakeProManager				gcls_Pro;										// �v���R���������э쐬�Ǘ��N���X
	MakeFfManager				gcls_FF;										// �e�e�������э쐬�Ǘ��N���X

//// �����o�[�֐�
private:		
	int ThreadFirst();															// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast() { return 0;};												// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);												// �X���b�h�C�x���g����
	
//// �����o�[�ϐ�
private:
	// ��������
	ThreadQueue<DELIVERY_MAIL>	mque_DeliMail;									// ���[�����󂯓n���L���[
	HANDLE						m_evThStart;									// MainInstance�X���b�h�̎��s�v��
};
