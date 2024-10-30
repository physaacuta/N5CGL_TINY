#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC����N���X

// ���[�J���̃N���X
#include "MainConst.h"

using namespace KizuLib;
using namespace KizuODBC;

class MaeManager :	public ThreadManager
{
//// ���J�萔
public:

	enum em_EXEC_MODE
	{
		UPDATE_MAE_REQUEST = 0,		// �S�O�H�����X�V
		UPDATE_FRAG_ONLY,			// �t���O�̂ݍX�V
		END
	};

	// ���m��Ǘ�No�� �Ǘ�No�� UpDate
	struct TYP_MAE_REQUEST
	{
		em_EXEC_MODE			emMode;									// ���[�h
		char					cMeiNo[SIZE_MEI_NO+1];					// �ύX�Ώۂ̖���No
		char					cCoilNo[SIZE_COIL_NO + 1];					// �ύX�Ώۂ̃R�C��No

		// �X�V���
		char					cKizukenNo[SIZE_KIZUKEN_NO];			// ������̊Ǘ�No
		long					nNobiSPM;								// SPM�L�ї� [10-2%]
		long					nCutF;									// �����ɂ�����t�����g�[�J�b�g�̎��ђ�[mm]
	};


//// ���J�֐�
public:
	MaeManager(void);
	virtual ~MaeManager(void);


	//=========================================
	// �O���󂯓n��
	void RecvCoilInf(COIL_INF const* pCoil);					//  �R�C������M
	void RecvMaeWrite(char const* cMeiNo, char const* cCoilNo);	//  �O�H���r��񏑍��݊����ʒm��M
	void RecvCoilResultWrite(COIL_INF const* pCoil);			//  �R�C�����я����݊����ʒm��M

private:
	void AddQue_MaeRequest(COIL_INF const* pCoil, em_EXEC_MODE emMode);	// ���Ǘ�No�X�V �˗�	//�ȑO�͌��J�֐��ł��������A�����o�[��


//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);										// �X���b�h�C�x���g����
	

	void Exec_MaeRequest(TYP_MAE_REQUEST const* p);						// ���Ǘ�No�X�V
	void Exec_FragOnly(TYP_MAE_REQUEST const* p);						// �t���O�̂ݍX�V


//// �����o�[�ϐ�
private:

	//=========================================
	// �e�C���X�^���X

	//// �L���[
	ThreadQueue<TYP_MAE_REQUEST> mque_MaeRequest;						// �󂯓n���f�[�^�p�L���[

	COIL_INF*					m_pCoilInf;								// �Ō�Ɏ�M�����R�C�����

};
