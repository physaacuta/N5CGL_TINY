// *********************************************************************************
//	FF���M�Ǘ��N���X
//	[Ver]
//		Ver.01    2012/09/06
//
//	[����]
//		
// *********************************************************************************

#pragma once


// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[

// ���[�J���̃N���X
#include "MainConst.h"


using namespace KizuLib;
using namespace KizuODBC;

class FFSendManager : public ThreadManager
{


//// ���J�֐�
public:
	FFSendManager(void);
	~FFSendManager(void);

	void SetFFSend() {	SetEvent(m_evFFSend); }	;						// FF�f�[�^���M�v��


//// ���J�ϐ�
public:
	// �e���[�J�[�X���b�h



//// �����o�[�֐�
private:	
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent( int nEventNo );									// �X���b�h�C�x���g����

	// �V�[�P���X����
	void FFSendMain();													// �O�H�����їv�����C������
	void SendMailFFReq(char* cKeyinf, char* cMeiNo, char* cLine_Code, char* cCoilNo);	// �O�H�����їv��

	static const int FFSEND_TIMER = 60*60*1000;							// ����������^�C�}�[�C���^�[�o�� [ms]

//// �����o�[�ϐ�
private:
	// �V�O�i��
	HANDLE m_evFFSend;													// FF�f�[�^���M�v���V�O�i�� 
	HANDLE m_evTimeCycle;												// �w���V�[
};
