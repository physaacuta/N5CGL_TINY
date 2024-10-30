// *********************************************************************************
//	�t���[���摜 �o�b�t�@�����O�Ǘ�
//	[Ver]
//		Ver.01    2010/03/04	����
//
//	[����]
// *********************************************************************************

#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLibMFC\Class\ThreadSyncManager.h"			// ����X���b�h�ԏ����ۏᐧ��N���X
#include "..\..\Library\KizuLibMFC\CLASS\ThreadLoopQueue.h"				// ��]���i�`���̃X���b�h�L���[
#include "..\..\Library\KizuLib\CLASS\KizuBitmapManager.h"				// �r�b�g�}�b�v�摜�N���X

// ���[�J��
#include "MainConst.h"													// ���N���X�萔�w�b�_�[
#include "ParamManager.h"												// �p�����[�^�Ǘ��N���X
#include "FrameDataManager.h"											// �t���[���f�[�^�Ǘ��N���X
#include "ProcessQueThread.h"											// ProcThread���

using namespace KizuLib;

class ImgBufProc : public ThreadManager,
					public ProcessQueThread<FRAME_DATA_ARRAY, FrameDataManager>
{
//// ���J�֐�
public:
	ImgBufProc(void);
	virtual ~ImgBufProc(void);

	void Reset();														// ���Z�b�g
	void SetIsExec(bool bMode);											// �������s�L��
	void Alloc();
	void Free();


	// �v���p�e�B
	void SetParamMgr(ParamManager* pCls) { mcls_pParam = pCls; }		// �p�����[�^�Ǘ�	

	void SetQueOutSkip(ThreadQueueImpl<FrameDataManager>* pQue){ mque_pOutSkip = pQue; }		// DispProc�ɓn���L���[ (���ۑ���)
	void SetQueRecvFree(IQueue<HT_RPI_FDATA>** pQue) { mque_pRecvFree = pQue; }		// �r���ԋp�p

	void SetDetailLog( KizuLib::LogFileManager *log) { mcls_detail = log; }

//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	
	// ����
	void AddNewItem();													// �摜�擾
	virtual FrameDataManager* OnAddNewItem( FRAME_DATA_ARRAY* pIn ) {return NULL;};	// ������
	void Exec(FRAME_DATA_ARRAY* p);										// �������s

	KizuLib::LogFileManager *			mcls_detail;

//// �����o�[�ϐ�
private:
	//// �󂯓n���L���[
	ThreadQueueImpl<FrameDataManager>*	mque_pOutSkip;					// [���ۑ���] �摜�����X�L�b�v��
	IQueue<HT_RPI_FDATA>**		mque_pRecvFree;					// [LIB�ɕԂ��L���[] 


	//// �e�C���X�^���X
	ParamManager*						mcls_pParam;					// �p�����[�^�Ǘ�

	// ���ʓI�ȏ��
	bool								m_bIsExec;						// ���s���� true (�B���J�n��true)
};
