// *********************************************************************************
//	PRI�t�@�C������N���X
//	[Ver]
//		Ver.01    2010/03/04	����
//
//	[����]
//		�E
// *********************************************************************************

#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLibMFC\Class\ThreadSyncManager.h"			// ����X���b�h�ԏ����ۏᐧ��N���X

// ���[�J��
#include "MainConst.h"													// ���N���X�萔�w�b�_�[
#include "ParamManager.h"												// �p�����[�^�Ǘ��N���X
#include "FrameDataManager.h"											// �t���[���f�[�^�Ǘ��N���X
#include "RpiWriteProc.h"												// RPI�t�@�C�������N���X
#include "ProcessQueThread.h"											// ProcThread���

using namespace KizuLib;

class RpiControlProc : public ThreadManager,
						public ProcessQueThread<FrameDataManager, DELIVERY_RPIWRITE>
{
//// ���J�֐�
public:
	RpiControlProc(void);
	virtual ~RpiControlProc(void);

	bool Init();														// �����J�n�� ����
	void ClosingRpi();													// RPI�t�@�C���N���[�Y�v��
	void ClosingRpi(int nIns);											// RPI�t�@�C���N���[�Y�v��
	void Reset();														// ���Z�b�g

	// �v���p�e�B
	void SetParamMgr(ParamManager* pCls) { mcls_pParam = pCls; }		// �p�����[�^�Ǘ�	
	void SetRpiWProc(RpiWriteProc* pCls[][MAX_COLOR_NUM]) { for(int ii=0; ii<MAX_INS; ii++) for(int jj=0; jj<MAX_COLOR_NUM; jj++) mcls_pRpiWProc[ii][jj] = pCls[ii][jj]; }	// RPI�t�@�C�������N���X

	virtual void SetQueOut(ThreadQueueImpl<DELIVERY_RPIWRITE>* pQue[][MAX_COLOR_NUM]){ for(int ii=0; ii<MAX_INS; ii++) for(int jj=0; jj<MAX_COLOR_NUM; jj++) mque_pOut[ii][jj] = pQue[ii][jj]; }	// RpiProc�ɓn���L���[

//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	
	// ����
	void AddNewItem();													// �摜�擾
	virtual DELIVERY_RPIWRITE* OnAddNewItem( FrameDataManager* pIn ) {return NULL;};	// ������


//// �����o�[�ϐ�
private:
	//// �󂯓n���L���[
	ThreadQueueImpl<DELIVERY_RPIWRITE>*	mque_pOut[MAX_INS][MAX_COLOR_NUM]; // RPI�t�@�C���������݂֎󂯓n���L���[	[out] (�z��̃|�C���^)


	//// �e�C���X�^���X
	ParamManager*						mcls_pParam;					// �p�����[�^�Ǘ�
	RpiWriteProc*						mcls_pRpiWProc[MAX_INS][MAX_COLOR_NUM];		// RPI�t�@�C�������N���X
};
