// *********************************************************************************
//	���C���Ǘ��X���b�h
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//		
//
// *********************************************************************************

#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�W���[

// ���[�J��
#include "MainConst.h"													// ���N���X�萔�w�b�_�[
#include "LinkswManager.h"

using namespace KizuLib;

class MainInstance : public ThreadManager
{
//// ���J�֐�
public:
	MainInstance(void);
	virtual ~MainInstance(void);

	void Start()														// �X���b�h�J�n (MainInstance�̎������̓I�[�o�[���C�h)
	{
		SetEvent(m_evThStart);
	}

	BOOL Init();														// ����������
	void Exit();														// �I������

//// ���J�ϐ�
public:
	// �e���[�J�[�X���b�h
	LinkswManager *				gcls_pLinksw[MAX_THREAD];				// �J���������N�X�C�b�`�ʐM�N���X

//// �����o�[�֐�
private:
	// �X���b�h�����`
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����

//// �����o�[�ϐ�
private:
	// ��������
	HANDLE						m_evThStart;							// MainInstance�X���b�h�̎��s�v��

	// ���̑��������
	int							m_nUnitNum;								// �Ǘ��Ώۋ@�퐔

// ======================================================================
// ����
public:
	// ���[�v�Z�o
	void GetChLoop(int nUnit, int& ii_s, int& ii_e)						// ���j�b�g�w�莞�̑ΏێZ�o
	{
		if (0 == nUnit) { ii_s = 0; ii_e = m_nUnitNum; }
		else { ii_s = nUnit - 1; ii_e = nUnit; }
	}
};
