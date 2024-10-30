// *********************************************************************************
//	�����p���Ƃ̕R�t���N���X
//	�������̉摜�R�t���N���X
//	[Ver]
//		Ver.01    2010/03/04	����
//
//	[����]
// *********************************************************************************

#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLibMFC\CLASS\ThreadLoopQueue.h"				// ��]���i�`���̃X���b�h�L���[


// ���[�J��
#include "MainConst.h"													// ���N���X�萔�w�b�_�[
#include "ProcessQueThread.h"											// ProcThread���

#include "ParamManager.h"												// �p�����[�^�Ǘ��N���X

class CoilControlProc : public ThreadManager,
						public ProcessQueThread<FRAME_DATA_ARRAY, FRAME_DATA_ARRAY>
{

private:


	//=========================================
	// �R�C�����i�[�G���A
	struct COIL_INFO_BUF {
		char							cKanriNo[SIZE_KIZUKEN_NO];
		char							cCoilNo[32];
		int								pcode[2];			// 1�I���W�� 
		int								scode[2];			// 1�I���W�� 

		DWORD							nChangeFno;			// �ؑփt���[��No
		int								mmFromHead;		// �n�ړ_����̋���[mm]

		// �����Z�b�g
		EM_COMP_TYPE					emCompType;			// �摜���k�^�C�v
		int								nCompQuality;		// ���k�i��
		bool							bDummyCoil;			// true:�_�~�[�R�C��
	};


//// ���J�֐�
public:
	CoilControlProc(int nIns);
	virtual ~CoilControlProc(void);


	void Reset();														// ���Z�b�g
	void SetIsExec(bool bMode) { m_bIsExec = bMode; }					// �������s�L��
	void SetNextCoil(COMMON_QUE const* p);								// ���R�C�����Z�b�g


	// �v���p�e�B
	void SetParamMgr(ParamManager* pCls) { mcls_pParam = pCls; }		// �p�����[�^�Ǘ�	
	void SetQueRecvFree(IQueue<HT_RPI_FDATA>* pQue) { mque_pRecvFree = pQue; }		// �r���ԋp�p


	// ��ʃA�N�Z�X�p
	char const* GetNextCoil() const {return m_NextCoil.cKanriNo;}


//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����

	// ����
	void AddNewItem();													// �摜�擾
	virtual FRAME_DATA_ARRAY* OnAddNewItem( FRAME_DATA_ARRAY* pIn ) {return NULL;};	// ������

	// �R�C�����
	void SetDummeyCoil(COIL_INFO_BUF& typCoil);							// ���R�C�����G���A�Ƀ_�~�[�R�C�����Z�b�g
	bool ChangeCoil(FRAME_DATA_ARRAY* p);								// �R�C���ؑ� (�A���A�o�b�` ��������)

	// �A�����C���p
	void Exec_Len(FRAME_DATA_ARRAY* p);									// �A�����C�����̏������s


//// �����o�[�ϐ�
private:

	// �󂯓n���L���[
	IQueue<HT_RPI_FDATA>*		mque_pRecvFree;					// [LIB�ɕԂ��L���[] 
//	ThreadQueue<DELIVERY_MAIL>			mque_Mail;						// �O�����󂯓n���L���[


	// �e�C���X�^���X
	ParamManager*						mcls_pParam;					// �p�����[�^�Ǘ�

	// ���ʓI�ȏ��
	int									m_nIns;							// �C���X�^���X�ԍ� (0�I���W��)
	bool								m_bIsExec;						// ���s���� true (�B���J�n��true)
	DWORD								m_nRecvFno;						// ��M�t���[��No

	// �R�C�����
	bool								m_bDebugNextWpd;				// ���R�C�������ؑ֎� true
	COIL_INFO_BUF						m_NextCoil;						// ���R�C�����G���A

	// �����
	COIL_INFO_BUF						m_NowCoil;						// ���R�C�����G���A
	DWORD								m_nCFno;						// �R�C���擪�̃t���[��No

};

