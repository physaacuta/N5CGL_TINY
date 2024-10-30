#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLibMFC\Class\CreateSQL.h"					// SQL�������⏕�N���X

// STL
#include <vector>														// ���X�g�p

// ���[�J���̃N���X
#include "MainConst.h"

class StatusManager : public ThreadManager
{
//// �����o�[�֐�
protected:
	static const int MAX_COLUMN		= 64;								// �P�s�ŊǗ�����ő��
//	static const int DB_CHECK_TIME	= 1 * 60 * 1000;					// �@���ԃ`�F�b�N���� [ms]
	static const int DB_CHECK_TIME	=  10*1000;							// �@���ԃ`�F�b�N���� [ms]


	// �����Ǘ��\����
	struct TYP_LIST {
		//// �}�X�^�[���
		int			nKey;												// �敪
		int			nCycle;												// ���W�Ԋu [��]
		CString		sName[MAX_COLUMN];									// ����
		int			nId[MAX_COLUMN];									// ���W����@��ID (0:�ΏۊO)
		
		//// �t�����
		int			nNextCnt;											// �����W�҂��J�E���g (nCycle�����āA1�����Ɍ��Y 0:�Ŏ��s)

		CString		sNewTime;											// �o�^���� (��L�[�̈��)
		
		bool		bSetN[MAX_COLUMN];									// �f�[�^�Z�b�g�L�� (���ꂪfalse�̂Ƃ��́ADB��NULL�Z�b�g)
		int			nNewN[MAX_COLUMN];									// �ŐV�̒l

		bool		bSetS[MAX_COLUMN];									// �f�[�^�Z�b�g�L��
		CString		sNewS[MAX_COLUMN];									// �ŐV�̕t�����
	};

//// ���J�֐�
public:
	StatusManager(void);
	virtual ~StatusManager(void);


//// �����o�[�֐�
protected:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	
	// ���C������
	bool	Init();														// ������
	void	CheckItem();												// ���W�^�C�~���O���Ď�
	bool	GetStatusInf(TYP_LIST* t);									// DB����ݒ�����擾

	virtual void SetStringData(TYP_LIST* t) {};							// �t����񕔂ɓo�^



//// �����o�[�ϐ�
protected:
	// �n���h��
	HANDLE								m_evTimerCheck;					// DB�`�F�b�N �^�C�}�[ [ms]

	// ��`
	int									m_nDbColumnKiki;				// �@���� �̗�
	int									m_nDbColumnTemp;				// �t����� �̗�

	// ���̑��������
	std::vector<TYP_LIST>				m_vItem;						// �����Ǘ���� (�P�������X�g)
};
