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
#include "..\..\include\KizuCommon.h"									// �r�����ʃw�b�_�[
#include "..\..\include\LineCommon.h"									// �V�X�e�����ʃw�b�_�[
#include "..\..\include\OutInterface.h"									// �O���@��p�w�b�_�[
#include "..\..\Library\KizuLib\KizuLib.h"								// �r�����C�u�����w�b�_�[

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLib\CLASS\KizuMacro.h"						// �r���}�N��
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLibMFC\CLASS\LogFileManager.h"				// ���O�}�l�[�W���[

// �r�����C�u���� �C���N���[�h
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC����N���X


// �e���[�J�[�X���b�h
#include "MainConst.h"
#include "SockDataExec.h"												// �ʐM���䃏�[�J�[�X���b�h

using namespace KizuLib;
//using namespace KizuSock3;
//using namespace KizuODBC;

class MainInctance : public ThreadManager
{
private:
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

	void Start() { SetEvent(m_evThStart); };							// �X���b�h�J�n

	BOOL Init();														// ����������
	void Exit();														// �I������

	// �O���A�N�Z�X
	void SetDeliveryMail(ENUM_MAININCTANCE emNo, COMMON_QUE * que);		// �X���b�h�L���[�Ƀ��[���f�[�^�o�^

//// ���J�ϐ�
public:
	// �e���[�J�[�X���b�h
	SockDataExec*		gcls_pSock;										// �ʐM���䃏�[�J�[�X���b�h

																		// �󂯓n���L���[
	ThreadQueue<Delivery>		gque_DeliSend;							// ���M�˗��p�L���[(�R�C�����v���`��/�e�X�g�`��)



//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast() { return 0; }										// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
	void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����



//// �����o�[�ϐ�
private:
	// �󂯓n���L���[
	ThreadQueue<DELIVERY_MAIL>	mque_DeliMail;							// ���[�����󂯓n���L���[

	// �����V�O�i��
	HANDLE	m_evThStart;												// MainInctance�X���b�h�̎��s�v��

	// ���̑��������


};
