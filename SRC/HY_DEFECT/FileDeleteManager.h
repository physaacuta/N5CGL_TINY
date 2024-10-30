//=====================================================================
// �ߋ��̉摜�f�[�^���폜
//	�y�����z
//		�E�����R�[�h�؂�ւ����ɁA�w�背�R�[�h��Ώۂɍ폜����
//		�E4���ȏ�o�߂����Â�JPEG�摜���폜
//=====================================================================

#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[


// �r�����ʃw�b�_�[
#include "..\..\include\KizuCommon.h"									// �r�����ʃw�b�_�[


using namespace KizuLib;

class FileDeleteManager : public ThreadManager
{
	//// ���J�萔
public:

	// �r�摜�ۑ��p�e�[�u��
	struct TYP_DELETE_FILE
	{
		char					cPath[128];								// �p�X
	};

	//// ���J�֐�
public:
	FileDeleteManager(void);
	virtual ~FileDeleteManager(void);


	bool	DeleteStart(char const* pPath);								// �폜�J�n
	bool	DeleteAll(char const* pPath);								// �폜�J�n(QUE)
	bool	IsRun() const { return m_bRun; }							// �폜��

	ThreadQueue<TYP_DELETE_FILE>	mque_pFileDel;						// �󂯓n���f�[�^�p�L���[


//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	

	void DeleteExec();													// �폜����
	void DeleteExecQue(char const* pPath);								// �폜����(QUE)


//// �����o�[ �ϐ�
private:
	

	//=========================================
	// �V�O�i��
	HANDLE					m_evRuning;									// ���s���V�O�i�� (�蓮)
	HANDLE					m_evRunEnding;								// ���s���f�v���V�O�i��
	HANDLE					m_evRunEnd;									// ���s���f�����V�O�i��


	//=========================================
	// ���
	bool					m_bRun;										// �폜�� true
	char					m_cPath[128];								// �폜�Ώۃt�H���_

};
