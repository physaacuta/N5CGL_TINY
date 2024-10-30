//=====================================================================
// �摜�f�[�^��񓯊��ɕۑ�����
//	�y�����z
//		�E��M�^�C�~���O�ŕۑ����Ă���ƁA���Ԃ��|���邽�߂̑΍�
//=====================================================================

#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[


// �r�����ʃw�b�_�[
#include "..\..\include\KizuCommon.h"									// �r�����ʃw�b�_�[

using namespace KizuLib;

class FileSaveManager :	public ThreadManager
{
//// ���J�萔
public:

	// �r�摜�ۑ��p�e�[�u��
	struct TYP_SAVE_IMG
	{
		int						nSetCnt;								// �f�[�^�Z�b�g��
		char					cPath[MAX_CAMANGLE][32];				// �ۑ��p�X
		int						nSize[MAX_CAMANGLE];					// �摜�T�C�Y
		BYTE*					pImg [MAX_CAMANGLE];					// �摜�f�[�^

		char*					pMemPath[MAX_CAMANGLE];					// ���L�������p�X	(NULL�ȊO�̏ꍇ�A�摜�ۑ���A�ۑ��p�X�������ɏ�������)
	};


//// ���J�֐�
public:
	FileSaveManager(void);
	virtual ~FileSaveManager(void);

	// �v���p�e�B
	void SetQueImgSave	 (ThreadQueue<TYP_SAVE_IMG>* pQue) { mque_pImgSave = pQue; }		// �󂯓n���f�[�^�p�L���[
	void SetQueImgSaveLow(ThreadQueue<TYP_SAVE_IMG>* pQue) { mque_pImgSaveLow = pQue; }		// �󂯓n���f�[�^�p�L���[

	// �O���A�N�Z�X
	void SetEvClearQue()		{ SetEvent(m_evClearQue); }				// �������L���[��S�폜


//// ���J�ϐ�
public : 


//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	
	void AllQueFree(ThreadQueue<TYP_SAVE_IMG>* pQue);					// �S�L���[�J��
	void SaveImg(TYP_SAVE_IMG* p);										// �摜�t�@�C����������


//// �����o�[�ϐ�
private:
	// �X���b�h�L���[
	ThreadQueue<TYP_SAVE_IMG>*	mque_pImgSave;							// �󂯓n���f�[�^�p�L���[ (��\�r)
	ThreadQueue<TYP_SAVE_IMG>*	mque_pImgSaveLow;						// �󂯓n���f�[�^�p�L���[ (���X�g���ŉB����r)


	// �V�O�i��
	HANDLE						m_evClearQue;							// �������C�x���g

};
