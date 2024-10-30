// *********************************************************************************
//	HDD����N���X
//	[Ver]
//		Ver.01    2010/03/04	����
//
//	[����]
//		�E�S���ۑ�PC�̋󂫗e�ʂ��`�F�b�N���āA�ŌÂ������
//		�ERPI�t�@�C�����o���オ�����^�C�~���O�� �S���ۑ�PC�Ɉړ�
//		�E�ړ����I����āA���t�H���_���� ���R�s�[�����z���P���ȏ�Â����t�̃t�@�C�����L��΁A�o�b�N�A�b�v�ړ�
// *********************************************************************************

#pragma once
// ���ʃw�b�_�[
#include "..\..\Include\KizuCommon.h"									// �V�X�e���p�C���N���[�h

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[

#include <direct.h>														// �t�H���_����
#include <shlwapi.h>													// �p�X��������֌W

// STL
#include <vector>														// ���X�g�p
#include <algorithm>													// �\�[�g�p

// ���[�J���̃N���X
#include "MainConst.h"
#include "ParamManager.h"												// �p�����[�^�Ǘ��N���X
#include "RpiWriteProc.h"												// RPI�t�@�C�������N���X

using namespace KizuLib;

class HddManager : public ThreadManager
{


//// �����o�[�萔
private:
	struct RPI_ITEM {
		CString			sName;			// RPI�t�@�C����
		CTime			tTime;			// �A�N�Z�X���t
		LARGE_INTEGER	nSize;			// �t�@�C���T�C�Y
	};

	// �萔
	static const int COPY_VIEW_SIZE		= 1*1024*1024;					// RPI�t�@�C�������ɃR�s�[����T�C�Y[MB] ���̂��炢���ő��B�傫�����Ă��A���������Ă��t�ɒx���Ȃ�
	static const int FOLDER_CHECK_TIME	= 2 * 60 * 1000;				// �t�H���_�`�F�b�N���� [ms]


//// ���J�֐�
public:
	HddManager(void);
	virtual ~HddManager(void);


	// �v���p�e�B
	void SetParamMgr(ParamManager* pCls) { mcls_pParam = pCls; }		// �p�����[�^�Ǘ�	
	void SetQueRpiMove(ThreadQueue<RPI_MOVE>* pQue) { mque_pRpiMove = pQue; }	//
	void SetQueRpiWrite(ThreadQueue<DELIVERY_RPIWRITE>* pQue[][MAX_COLOR_NUM]){ for(int ii=0; ii<MAX_INS; ii++) for(int jj=0; jj<MAX_COLOR_NUM; jj++) mque_pRpiWrite[ii][jj] = pQue[ii][jj]; }	// RpiProc�ɓn���L���[

	// ����
	int SearchFolderIndex(int nIns, bool bIsNew);						// �V���t�@�C���̃t�H���_Index���Z�o


//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	

	bool Exec(RPI_MOVE const* p);										// ���C������
	bool FreeSpace(char const* sFolder,const int nFreeSpaceGB);								// �S���摜�ۑ�PC�̋󂫃G���A���m��
	bool CopyFileExec(RPI_MOVE const* p, int nHddIndex);				// ���ʂ̃t�@�C���ړ�
	void FastDelete();													// �����N������ �S�~�폜
	void BackUpCopy(RPI_MOVE const* p=NULL);							// �ꎟ�t�H���_�ɂ���RPI�t�@�C�����ړ� (���펞�ł͑Ώۃt�@�C���͖����͂�)

	// ����
	static inline bool sort_fd_old(const RPI_ITEM &a, const RPI_ITEM &b);	// �\�[�g�֐�
	static inline bool sort_fd_new(const RPI_ITEM &a, const RPI_ITEM &b);	// �\�[�g�֐�
	bool AccessSaveFolder(int nIns);									// �����ۑ��p�t�H���_�ɃA�N�Z�X�\���`�F�b�N
	void StartTimer_SaveFolder();										// �����ۑ��p�t�H���_�ɃA�N�Z�X�\�`�F�b�N�^�C�}�[

	bool RootFolderCheck(char const* cPath1, char const* cPath2);		// ����h���C�u���`�F�b�N
	double CheckFreeSpace(char const* sFolder);							// �S���摜�ۑ�PC�̋󂫃G���A�`�F�b�N
	bool   CheckFreeSpace(char const* sFolder, double dLimit);			// �S���摜�ۑ�PC�̋󂫃G���A�`�F�b�N
	bool GetFileUmu(char const* cFPath);								// �t�@�C���L�����`�F�b�N



//// �����o�[�ϐ�
private:

	// �󂯓n���L���[
	ThreadQueue<RPI_MOVE>*				mque_pRpiMove;					// RPI�t�@�C���ړ��v���󂯓n���L���[ [in]
	ThreadQueue<DELIVERY_RPIWRITE>*	mque_pRpiWrite[MAX_INS][MAX_COLOR_NUM]; // RPI�t�@�C���������݂֎󂯓n���L���[	[out]


	// �e�C���X�^���X
	ParamManager*						mcls_pParam;					// �p�����[�^�Ǘ�

	// �n���h��
	HANDLE								m_evTimerCheck;					// �ۑ���t�H���_�A�N�Z�X�`�F�b�N �^�C�}�[ [ms]

	// ���̑��������
	bool								m_bSaveFolderAccess[MAX_INS];	// �ۑ���t�H���_�ɃA�N�Z�X�\�� (true:�A�N�Z�X�\ false:�s��)
};
