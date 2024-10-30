// *********************************************************************************
//	�t���[���摜 �������A�������N���X
//	[Ver]
//		Ver.01    2010/03/04	����
//
//	[����]
//		�E�ʒP�ʂŏ���
// *********************************************************************************

#pragma once

// ���ʃw�b�_�[
#include "..\..\Include\KizuCommon.h"									// �V�X�e���p�C���N���[�h


// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLibMFC\Class\ThreadSyncManager.h"			// ����X���b�h�ԏ����ۏᐧ��N���X


// ���[�J��
#include "MainConst.h"													// ���N���X�萔�w�b�_�[
#include "StatusManager.h"												// �X�e�[�^�X����N���X
#include "ParamManager.h"												// �p�����[�^�Ǘ��N���X
#include "FrameDataManager.h"											// �t���[���f�[�^�Ǘ��N���X
#include "ProcessQueThread.h"											// ProcThread���

#include "..\..\Library\KizuLib\Class\KizuBitmapManager.h"


using namespace KizuLib;

class ImgUnionProc : public ThreadManager,
					 public ProcessQueThread<FRAME_DATA_ARRAY, FRAME_DATA_ARRAY>
{
//// ���J�֐�
public:
	ImgUnionProc(int nId);
	virtual ~ImgUnionProc(void);

	void Reset();														// ���Z�b�g
	void SetIsExec(bool bMode) { m_bIsExec = bMode; }					// �������s�L��


	// �v���p�e�B
	void SetStatusMgr(StatusManager* pCls) { mcls_pStatus = pCls; }		// �X�e�[�^�X�Ǘ�
	void SetParamMgr(ParamManager* pCls) { mcls_pParam = pCls; }		// �p�����[�^�Ǘ�

	void SetQueFrame(ThreadLoopQueue<FrameDataManager>* pQue) { mque_pFrame = pQue; }	// �������ŏ��Ɏ擾����L���[
	void SetQueComp (ThreadSyncManager<FrameDataManager>* pQue){ mque_pComp = pQue; }	// �L���[ (�����擾����������)

	void SetDetailLog( KizuLib::LogFileManager *log) { mcls_detail = log; }

//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	

	// ����
	virtual FRAME_DATA_ARRAY* OnAddNewItem( FRAME_DATA_ARRAY* pIn );	// ������
	void Exec(FRAME_DATA_ARRAY* p);										// �������s
	void Exec_Union(FRAME_DATA_ARRAY* p, bool bSkip);					// �摜�A��

	void Exec_Kido(FRAME_DATA_ARRAY* p);								// �P�x�␳

	KizuLib::LogFileManager *			mcls_detail;


//// �����o�[�ϐ�
private:

	// �󂯓n���L���[
	ThreadSyncManager<FrameDataManager>*mque_pComp;						// �t���[���摜�󂯓n���X���b�h�����ۏ�׽  [�f�[�^�Q�Ɨp����]
	ThreadLoopQueue<FrameDataManager>*	mque_pFrame;					// �t���[���摜�󂯓n���L���[(��]���i�`��)	[�擾]


	// �e�C���X�^���X
	StatusManager*						mcls_pStatus;					// �X�e�[�^�X�Ǘ�
	ParamManager*						mcls_pParam;					// �p�����[�^�Ǘ�

	// ���ʓI�ȏ��
	bool								m_bIsExec;						// ���s���� true (�B���J�n��true)
	static const  BYTE							*m_BlankData;
};
