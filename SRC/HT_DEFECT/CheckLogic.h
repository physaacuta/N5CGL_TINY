#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC����N���X

class CheckLogic : public ThreadManager
{

private:
	struct TYP_DELEV
	{
		int				scode;											// scode 0:�S��
	};


//// ���J�֐�
public:
	CheckLogic();
	virtual ~CheckLogic();

	void	Check(int scode);											// Logic�L���`�F�b�N


//// ���J�ϐ�
public : 
	// �X���b�h�L���[
	ThreadQueue<TYP_DELEV>	gque_Check;									// �`�F�b�N�˗��p�L���[





//// �����o�[�֐�
private:		
	// �X���b�h�����`
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast() { return 0; };										// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	
	EM_DIV_BOOL ExecLogic(int scode);									// ���W�b�N�t�@�C���L���`�F�b�N


private:

	// ���̑�
	EM_DIV_BOOL			m_bLogicUmu;									// ���W�b�N�t�@�C���L������
};

