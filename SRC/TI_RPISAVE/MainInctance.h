// *********************************************************************************
//	���C���Ǘ��X���b�h
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//		
// *********************************************************************************

#pragma once

// �r�����C�u���� �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLibMFC\CLASS\ThreadLoopQueue.h"				// ��]���i�`���̃X���b�h�L���[
#include "..\..\Library\KizuLibMFC\Class\ThreadSyncManager.h"			// ����X���b�h�ԏ����ۏᐧ��N���X
#include "..\..\Library\KizuLibMFC\Class\ImageWindow.h"					// �摜�\������N���X

#include "..\..\Library\DefectLib\SockOneToAnyRecv.h"					// ��]�����`����M


// ���
#include "ProcessQueThread.h"
#include "ProcessQueImpl.h"
#include "FrameDataManager.h"											// �t���[���f�[�^�Ǘ��N���X

// �e���[�J�[�X���b�h
#include "MainConst.h"
#include "ParamManager.h"
#include "StatusManager.h"												// �X�e�[�^�X����N���X
#include "HddManager.h"													// HDD�Ǘ��N���X


//// Proc�֌W
#include "ImgSyncProc.h"												// �������c��
#include "CoilControlProc.h"											// �R�C���Ǘ��N���X
#include "ImgUnionProc.h"												// �������摜�A���N���X
#include "ImgBufProc.h"													// �t���[���摜 �o�b�t�@�����O�Ǘ��N���X
#include "ImgCompProc.h"												// �摜���k�N���X
#include "RpiControlProc.h"												// RPI�t�@�C������N���X
#include "RpiWriteProc.h"												// RPI�t�@�C�������N���X
#include "DispProc.h"													// ���ʕ\���N���X
#include "CudaAlloc.h"													// CUDA�������Ǘ��N���X


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
	ParamManager*						gcls_pParam;					// �p�����[�^�Ǘ��N���X
	StatusManager						gcls_Status;					// �X�e�[�^�X����N���X
	ImageWindow							gcls_ImgWin;					// �摜�\���N���X
	HddManager							gcls_Hdd;						// HDD�Ǘ��N���X


	KizuLib::LogFileManager *			gcls_InsLog[MAX_INS];
	KizuLib::LogFileManager *			gcls_InsLog_ImgBuf_Union;
	// �摜�̎󂯓n�����ԂɋL�q
	SockOneToAnyRecv*					gcls_pSock[MAX_INS][NUM_CAM_POS];		// �摜�捞�N���X
	ImgSyncProc*						gcls_pSyncProc[MAX_INS];				// �������R�t���Ǘ��N���X	
	CoilControlProc*					gcls_pCoilProc[MAX_INS];				// �R�C���Ǘ��N���X
	ImgUnionProc*						gcls_pUnionProc[MAX_THREAD_MULTI_P];	// �������摜�A���N���X (���񏈗��̂���)
	ImgBufProc*							gcls_pImgBufProc;						// �t���[���摜 �o�b�t�@�����O�Ǘ��N���X
	ImgCompProc*						gcls_pCompProc[MAX_THREAD_MULTI_P];		// �摜���k�N���X (���񏈗��̂���)
	RpiControlProc*						gcls_pRpiCProc;							// PRI�t�@�C������N���X (�P��N���X) ���̒��ŁARPI�����N���X�������ς���������
	RpiWriteProc*						gcls_pRpiWProc[MAX_INS][MAX_COLOR_NUM];	// PRI�t�@�C�������N���X
	DispProc*							gcls_pDispProc;							// ���ʕ\���N���X

	LacManager							gcls_Lac;								// ���x���k�N���X
	CudaAlloc*							gcls_pCudaAlloc;						// CUDA�������Ǘ��N���X


	//---------------------------
	// �L���[
	
	//�_�~�[�𔻒肵�āASockRecv��Free���ɕԂ��悤�̃v���L�V�L���[�B
	std::vector< IQueue<HT_RPI_FDATA> *> que_pSockRecv_Free;

	// ����ő傫�ȃ��[�v���\�z�B IQueue �T�|�[�g�N���X
	ThreadLoopQueueImpl<HT_RPI_FDATA>*		gque_pSockRecv[MAX_INS];	// ���肩���RAW�摜�󂯓n���L���[(��]���i�`��)	[LIB��AP��] (LIB���������ChainProc��LIB)
	ThreadLoopQueueImpl<FrameDataManager>*	gque_pFrame;				// �t���[���摜�󂯓n���L���[(��]���i�`��)	[AP��AP��] (ImgUnionProc�������DispProc)

	// 1�J�����P��
	ThreadQueueImpl<FRAME_DATA_ARRAY>*		gque_pCoilC[MAX_INS];		// �R�C���Ǘ��X���b�h��  [PlgSyncProc��CoilControlProc��ImgUnionProc] �o�b�`���C���̂�
	ThreadSyncManagerImpl<FRAME_DATA_ARRAY>*gque_pUnion;				// �t���[���摜�󂯓n���X���b�h�����ۏ�׽  [CoilControlProc��ImgUnionProc��ChainProc]

	// �A���摜�P��
	ThreadSyncManagerImpl<FrameDataManager>*gque_pComp;					// �t���[���摜�󂯓n���X���b�h�����ۏ�׽  [ImgBufProc��ImgCompProc��RpiControlProc]
	ThreadQueueImpl<DELIVERY_RPIWRITE>*		gque_pRpiWrite[MAX_INS][MAX_COLOR_NUM];	// �t���[���摜�󂯓n���L���[	[RpiControlProc��RpiWriteProc]
	ThreadQueueImpl<FrameDataManager>		gque_Disp;					// �t���[���摜�󂯓n���L���[				[RpiWriteProc��DispProc]

	// ���̑�
	ThreadQueue<RPI_MOVE>				gque_RpiMove;					// RPI�t�@�C���ړ��v���L���[				[RpiWriteProc��HddManager]


//// �����o�[�֐�
private:		
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast() { return 0; }										// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	
	// ����
	void GetInsLoop(int nMode, int& ii_s, int& ii_e)					// �C���X�^���XNo�w�莞�̑ΏێZ�o
	{
		if(-1==nMode) { ii_s=0; ii_e=gcls_pParam->GetMain().nIns-1; }
		else		  { ii_s=ii_e=nMode;}
	}

	// �V�[�P���X����
	int	 DetectStart(COMMON_QUE* que);									// �����J�n����
	int  DetectStop(int mode, int ansRetc);								// ������~����



//// �����o�[�ϐ�
private:
	// ��������
	ThreadQueue<DELIVERY_MAIL>	mque_DeliMail;							// ���[�����󂯓n���L���[
	HANDLE						m_evThStart;							// MainInctance�X���b�h�̎��s�v��
};
