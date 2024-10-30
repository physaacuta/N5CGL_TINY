// *********************************************************************************
//	�������J�����̕R�t���N���X
//	[Ver]
//		Ver.01    2010/03/04	����
//
//	[����]
//		�E
// *********************************************************************************

#pragma once
// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLib\Class\KizuPerformanceCounter.h"			// �^�C�}�[
#include "..\..\Library\KizuLib\Class\KizuBitmapManager.h"

// ���[�J��
#include "MainConst.h"													// ���N���X�萔�w�b�_�[
#include "ProcessQueThread.h"											// ProcThread���

#include "ParamManager.h"												// �p�����[�^�Ǘ��N���X
#include "StatusManager.h"												// �X�e�[�^�X����N���X

using namespace KizuLib;

class ImgSyncProc :	public ThreadManager,
					public ProcessQueThread<HT_RPI_FDATA, FRAME_DATA_ARRAY>
{
public:
	ImgSyncProc(int nIns);
	virtual ~ImgSyncProc(void);

	void Reset();														// ���Z�b�g
	void SetIsExec(bool bMode) { m_bIsExec = bMode; }					// �������s�L��


	// �v���p�e�B
	void SetStatusMgr(StatusManager* pCls) { mcls_pStatus = pCls; }		// �X�e�[�^�X�Ǘ�
	void SetParamMgr(ParamManager* pCls) { mcls_pParam = pCls; }		// �p�����[�^�Ǘ�	
	
	void SetQueFree( IQueue<HT_RPI_FDATA >  *pQue ) { mque_pFree  = pQue; }		// �ԋpQue�Z�b�g


	// ���
	DWORD GetRecvFrameNo(int id) const {return m_nRecvFrameNo[id];}
	DWORD GetNowExecFrameNo(int id) const {return m_nRecvFrameNo[id];}
	void SetDetailLog( KizuLib::LogFileManager *log) { mcls_detail = log; }


//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	

	// ����
	virtual FRAME_DATA_ARRAY* OnAddNewItem( HT_RPI_FDATA* pIn ) {return NULL;};	// ������ �J�X�^�}�C�Y����̂ŁA�f�t�H���g����
	void AddNewItem();													// �摜�擾
	void Exec();														// �������s

	void PoolBack_RecvFreeQue(int nIndex);								// ��M�L���[�o�b�t�@��ɓ����Ă���M��ԋp
	void ItemReset();													// ��M�L���[�o�b�t�@��̏���������

	bool CheckAndAdd(HT_RPI_FDATA* p);									// �S�J�����Z�b�g�œ��������H
	bool CheckCamset();									// �S�J�����Z�b�g�����������`�F�b�N
	bool CheckSeqNo(HT_RPI_FDATA* p);									// �P�J�����ł̑��M�A�ԃ`�F�b�N
	int OutputIncompleteQueue(HT_RPI_FDATA* p);


	// �o�b�t�@�����O�Ǘ�
	bool AddToTail(int cam, HT_RPI_FDATA* p);							// �o�b�t�@�����O
	HT_RPI_FDATA* GetFromHead(int cam);									// �o�b�t�@���o��
	HT_RPI_FDATA* PeekFromHead(int cam);								// �o�b�t�@�݂邾��
	void ResetBufFrame();												// �o�b�t�@�N���A
	void ResetBufFrame(int nIndex);


	HT_RPI_FDATA* CreateDummyFrame(const HT_RPI_FDATA* p, int nCamId, int nCamIndex);

public:	
	int GetMaxBufFrameNum();											// �o�b�t�@�ő吔
private:

	KizuLib::LogFileManager *			mcls_detail;
	HT_RPI_FDATA *GetMinimumFrame( bool bNeedAllCamera, int *pnCamIndex );

//// �����o�[�ϐ�
private:

	// �󂯓n���L���[
	IQueue<HT_RPI_FDATA>*				mque_pFree;					// �t���[���摜��M�L���[ �ԋp�ꏊ


	// �e�C���X�^���X
	StatusManager*						mcls_pStatus;					// �X�e�[�^�X�Ǘ�
	ParamManager*						mcls_pParam;					// �p�����[�^�Ǘ�
	KizuPerformanceCounter				mcls_Time;						// �t���[���擾���Ԍv���p


	// ���ʓI�ȏ��
	int									m_nIns;							// �C���X�^���X�ԍ� (0�I���W��)
	bool								m_bIsExec;						// ���s���� true (�B���J�n��true)
	bool								m_bIsSync;						// ���񓯊������� true
	DWORD								m_nRecvFrameNo[NUM_CAM_POS];	// ��M�t���[��No (0�I���W��)
	DWORD								m_nEndFrameNo;					// ���������t���[��No

	std::deque<HT_RPI_FDATA*>			m_pBufFrame[NUM_CAM_POS];		// ��M�L���[�o�b�t�@��(����J�����������񗈂��΍�)
	bool m_bEnabled[NUM_CAM_POS];

};

