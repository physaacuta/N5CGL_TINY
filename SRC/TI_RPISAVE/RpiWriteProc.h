// *********************************************************************************
//	PRI�t�@�C�������N���X
//	[Ver]
//		Ver.01    2010/03/04	����
//
//	[����]
//		�E
// *********************************************************************************

#pragma once

// ���ʃw�b�_�[
#include "..\..\Include\KizuCommon.h"									// �V�X�e���p�C���N���[�h


// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[

// ���[�J��
#include "MainConst.h"													// ���N���X�萔�w�b�_�[
#include "ParamManager.h"												// �p�����[�^�Ǘ��N���X
#include "RpiFileManager.h"
#include "FrameDataManager.h"											// �t���[���f�[�^�Ǘ��N���X
#include "ProcessQueThread.h"											// ProcThread���

// STL
#include <vector>														// ���X�g�p
#include <algorithm>													// �\�[�g�p


//// ���J�萔
class RpiWriteProc : public RpiFileManager,
					 public ProcessQueThread<DELIVERY_RPIWRITE, FrameDataManager>
{


//// ���J�萔
public:


//// ���J�֐�
public:
	RpiWriteProc(int nIns, int nColor);
	virtual ~RpiWriteProc(void);

	void Reset();														// ���Z�b�g
	void SetIsExec(bool bMode) { m_bIsExec = bMode; }					// �������s�L��

	// �v���p�e�B
	void SetParamMgr(ParamManager* pCls) { mcls_pParam = pCls; }		// �p�����[�^�Ǘ�
	void SetQueRpiWrite(ThreadQueue<DELIVERY_RPIWRITE>* pQue){ mque_pRpiWrite = pQue; }	// RpiProc�ɓn���L���[
	void SetQueRpiMove(ThreadQueue<RPI_MOVE>* pQue) { mque_pRpiMove = pQue; }		//


	bool SetData_Setting();												// �ݒ��� �Z�b�g
	bool SetData_CoilInfo(FrameDataManager* p);							// �R�C���P�ʂ̐ݒ��� �Z�b�g (�R�C����[)
	bool SetData_CoilResult(FrameDataManager* p);						// �R�C���P�ʂ̎��я�� �Z�b�g (�R�C�����[)
	bool SetData_FrameInfo(FrameDataManager* p);						// �t���[���P�ʂ̐ݒ��� �Z�b�g
	

//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	
	// ����
	virtual FrameDataManager* OnAddNewItem( DELIVERY_RPIWRITE* pIn );	// ������

	void Exec_WriteFrame(FrameDataManager* p);							// �����ݏ�������
	void Exec_FileClose(FrameDataManager* p=NULL);						// �I������


	//=========================================
	// ���z�֐�
	// ����ǉ�
	virtual void		OnSetFileName(RpiHeader const& h, RPI_SETTING const& s, RPI_COIL_INFO const& c, CString* strName);	// �t�@�C��������
	virtual void		OnRpiFileSave(char const* strPath, CString* strName);	// RPI�t�@�C����������


//// �����o�[�ϐ�
private:
	//// �󂯓n���L���[
	ThreadQueue<DELIVERY_RPIWRITE>*		mque_pRpiWrite;					// RPI�t�@�C���������݂֎󂯓n���L���[	[in]
	ThreadQueue<RPI_MOVE>*				mque_pRpiMove;					// RPI�t�@�C���ړ��v���L���[�ʒm	[out]

	//// �e�C���X�^���X
	ParamManager*						mcls_pParam;					// �p�����[�^�Ǘ�

	// ���ʓI�ȏ��
	int									m_nIns;							// �C���X�^���X�ԍ� (0�I���W��)
	int									m_nColor;						// �摜�F�ԍ� (0�I���W��)
	bool								m_bIsExec;						// ���s���� true (�B���J�n��true)
};
