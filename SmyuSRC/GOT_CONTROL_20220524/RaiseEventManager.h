#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[


// ���[�J��
#include "MainConst.h"													// ���N���X�萔�w�b�_�[
#include "PlgControlManager.h"
#include "PioControlManager.h"


class RaiseEventManager :	public ThreadManager
{

//// ���J�֐�
public:
	RaiseEventManager(void);
	virtual ~RaiseEventManager(void);

	void SetPlgMgr(PlgControlManager* pCls) { mcls_pPlg = pCls;};		// �X�e�[�^�X����N���X�Z�b�g
	void SetPioMgr(PioControlManager* pCls) { mcls_pPio = pCls;};		// �X�e�[�^�X����N���X�Z�b�g


	//// �C�x���g����
	void RaiseEvents(EM_DIV_KIND emEvent, int nEventSub, BYTE* pEvent);	// �C�x���g����


//// �����o�[�֐�
private:
	int ThreadFirst() {return 0;};										// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast() {return 0;};										// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo) {return;};							// �X���b�h�C�x���g����
	

//// �����o�[�ϐ�
private:

	//=========================================
	// �e�C���X�^���X
	PlgControlManager*			mcls_pPlg;								// PLG�����Ǘ��N���X
	PioControlManager*			mcls_pPio;								// PIO�Ǘ��N���X


	//=========================================
	// ��������
	CRITICAL_SECTION		m_csLock;									// �N���e�B�J���Z�N�V����

};
