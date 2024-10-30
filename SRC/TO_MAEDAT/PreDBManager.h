// *********************************************************************************
//	�O�H�����я��DB�o�^�Ǘ��N���X
//	[Ver]
//		Ver.01    2012/09/06
//
//	[����]
//		
// *********************************************************************************

#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLibMFC\Class\CreateSQL.h"					// SQL�������⏕�N���X
#include "..\..\Library\DefectLib\DBSaveManager.h"						// DB�����݊�{�N���X
#include "..\..\Library\KizuODBC\OdbcBase.h"							// DB����N���X
#include "..\..\include\ToTbl.h"										// �R�C�����

// ���[�J���̃N���X
#include "MainConst.h"
#include "ParamManager.h"
// 2013.10.29 ���� �����T�[�o�[�Q�� ------->>>
#include "PreDBSelectManager.h"
// 2013.10.29 ���� �����T�[�o�[�Q�� <<<-------

using namespace KizuLib;
using namespace KizuODBC;
class PreDBManager : public ThreadManager
{

public:

	//// �󂯓n���\����
	struct DeliveryPreDB {
		char cKizuKenNo[SIZE_KIZUKEN_NO+1];				// �Ǘ�No
		char cPreKizuKenNo[SIZE_KIZUKEN_NO+1];			// �O�H���Ǘ�No
		char cLineCode[2+1];							// �H���R�[�h(�r���o�H��)	2013.10.30 ���� �L���ނ̕\��
		int nOuDiv;										// �\���敪
		char cMeiNo[SIZE_MEI_NO+1];						// ����No
		char cCoilNo[SIZE_COIL_NO+1];					// �R�C��No
	};

//// ���J�֐�
	PreDBManager();
	~PreDBManager(void);

    // 2014.07.25 �쌴  ���ԘA�g    >>>>>>>>
    void SetParamManager(ParamManager* pCls) {mcls_pParam = pCls;}  // �p�����[�^�Ǘ��N���X�Z�b�g
    // 2014.07.25 �쌴  ���ԘA�g    <<<<<<<<

// 2013.10.29 ���� �����T�[�o�[�Q�� ----->>>
private:

	//// �e��p�����[�^
	enum EM_PRE_LINE{			    // �O�H�����C��
		PRE_LINE_2TCM	    = 0,    // 2TCM
		PRE_LINE_3TCM       = 1,    // 3TCM
									// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� ----->>>
		PRE_LINE_N2TCM_ANA	= 2,	// 2TCM_ANA
									// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� <<<-----
		// 20221021 2TCM���@���ɔ������H������ ���� --->>>
		PRE_LINE_N2TCM_KIZU = 3		// 2TCM_KIZU
		// 20221021 2TCM���@���ɔ������H������ ���� ---<<<

	};

    enum EM_PRE_MILL{			// �����ށA������
		NAGOYA_MILL		= 0,	// ������
		ANOTHER_MILL	= 1     // ������
	};

#define SYSLOG_BASE		PARAM_SYSLOG_BASENO + 2
    enum EM_ALERT_NO{			// �ڑ���DB�Ɋւ���A���[��No
		ALERT_OK = SYSLOG_BASE, 
		ALERT_NG_PING,
		ALERT_NG_CONNECT,
		ALERT_NG_SEARCH,
		NON_ALERT				// �O��Ɠ������ʂׁ̈A�V�X���O�o�͂Ȃ�
	};

// 2013.10.29 ���� �����T�[�o�[�Q�� <<<-----

//// ���J�ϐ�
public:
	//// �X���b�h�L���[
	ThreadQueue<DeliveryPreDB> gque_AddSQLData;							// �󂯓n���f�[�^�p�L���[



//// �����o�[�֐�
private:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent( int nEventNo );									// �X���b�h�C�x���g����

	// �V�[�P���X����
	void DBInsMain( DeliveryPreDB* pDeli );																// DB�o�^���C������
	bool CheckInsert( OdbcBase* clspDB, DeliveryPreDB* pDeli );											// �o�^�Ώۂ��`�F�b�N
	bool DBIns( EM_PRE_LINE PreLine, DeliveryPreDB* pDeli, OdbcBase* clsInsDB, OdbcBase* clsPreDB );	// DB�o�^����
	
    bool GetNew2TCMInf( OdbcBase* clsPreDB, MAE_DEFECT_IMAGE_INF* typData );							// 2TCM���擾
	bool GetNew3TCMInf( OdbcBase* clsPreDB, MAE_DEFECT_IMAGE_INF* typData );							// 3TCM���擾
																										// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� ----->>>
	bool GetNewN2TCM_ANAInf(OdbcBase* clsPreDB, MAE_DEFECT_IMAGE_INF* typData);							// N2TCM_ANA���擾
																										// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� <<<-----
	// 20221021 2TCM���@���ɔ������H������ ���� --->>>
	bool GetNewN2TCM_KIZUInf(OdbcBase* clsPreDB, MAE_DEFECT_IMAGE_INF* typData);						// N2TCM_KIZU���擾
	// 20221021 2TCM���@���ɔ������H������ ���� ---<<<

	bool InsertDefectInf( OdbcBase* clsDB, MAE_DEFECT_IMAGE_INF* typData );								// �O�H���r���o�^
	bool InsertDefectImg( OdbcBase* clsDB, MAE_DEFECT_IMAGE_INF* typData );								// �O�H���r�摜�o�^
	void UpdateCoilLine( OdbcBase* clsDB, DeliveryPreDB* pDeli );										// �O�H�����C���ʃR�C�����X�V

	// 2013.10.30 ���� �L���ނ̕\�� ------->>>
	int CheckAnotherMill(char* LineCode);														        // �����ށA�����ނ̔���
	int GetAlertNo(PreDBSelectManager::EM_SELECT_DB nSelect,EM_PRE_LINE nPreLine);						// �ڑ����ʂ���A���[��No������
	// 2013.10.30 ���� �L���ނ̕\�� <<<-------

	void SendMailDefect(char* cMeiNo, char* cCoilNo);													// �O�H���r��񏑍��݊����ʒm���M

//// �����o�[�ϐ�
private:
	//// �e�C���X�^���X
	
	PreDBSelectManager* mcls_pSelect2TCM;						// �O�H��DB�I���N���X(2TCM)
	PreDBSelectManager* mcls_pSelect3TCM;						// �O�H��DB�I���N���X(3TCM)
	PreDBSelectManager::EM_SELECT_DB	m_emLastResult_2TCM;	// �O��̑I������
	PreDBSelectManager::EM_SELECT_DB	m_emLastResult_3TCM;	// �O��̑I������
																// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� ----->>>
	PreDBSelectManager* mcls_pSelectN2TCM_ANA;					// �O�H��DB�I���N���X(2TCM_ANA)
	PreDBSelectManager::EM_SELECT_DB	m_emLastResult_N2TCM_ANA;// �O��̑I������
																 // 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� <<<-----
	// 20221021 2TCM���@���ɔ������H������ ���� --->>>
	PreDBSelectManager* mcls_pSelectN2TCM_KIZU;					// �O�H��DB�I���N���X(2TCM_ANA)
	PreDBSelectManager::EM_SELECT_DB	m_emLastResult_N2TCM_KIZU;// �O��̑I������
	// 20221021 2TCM���@���ɔ������H������ ���� ---<<<

    ParamManager*			mcls_pParam;				    // �p�����[�^�Ǘ��N���X

	//// ���L���������
	HANDLE					m_hCoil;									// �R�C����� �n���h��
	TO_COIL_TBL*			mtbl_pCoil;									// �R�C����� �f�[�^
};
