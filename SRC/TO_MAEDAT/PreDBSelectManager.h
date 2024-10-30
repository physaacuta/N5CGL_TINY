//------------------------------------------------------------------------------
//	�O�H���ڑ��T�[�o�[�Ǘ��N���X
//	[Ver]
//		Ver.01	2013/09/19	����
//
//	[����]
//
//------------------------------------------------------------------------------
#pragma once

// �r�����C�u����
#include "..\..\Library\KizuLib\KizuLib.h"					// �r�����C�u����

#include "..\..\Library\KizuLib\Class\KizuMacro.h"			// �}�N���W
#include "..\..\Library\KizuLib\Class\KizuFunction.h"		// �X�^�e�B�b�N���C�u�����W
#include "..\..\Library\KizuLib\Class\PingManager.h"		// Ping�Ǘ��N���X

#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"	// ���O�Ǘ��N���X
#include "..\..\Library\KizuLibMFC\Class\CreateSQL.h"		// SQL�������⏕�N���X

#include "..\..\Library\KizuODBC\OdbcBase.h"				// DB����N���X

using namespace KizuLib;

class PreDBSelectManager
{
public:
	//------------------------------------------------------
	// �񋓑̒�`
	//------------------------------------------------------
	// DB�I�����[�h
	enum EM_SELECT_DB{
		INIT_DB		= -1,									// �����l
		NON_DB		= 0,									// ���I��
		PRE_DB		= 1,									// �~��DB
		SHARE_DB	= 2										// ����DB
	};

	// �I�����
	enum EM_SELECT_STATUS{
		SELECT_NON		= -1,								// �����{
		SELECT_OK		=  0,								// ����
		SELECT_NG		=  1								// �ُ�
	};

	//------------------------------------------------------
	// �\���̒�`
	//------------------------------------------------------
	// �I�����
	typedef struct TYP_SELECT_STATUS{
		EM_SELECT_STATUS	nPing;							// Ping����				[-1:�����{�A0:����A1:�ُ�]
		EM_SELECT_STATUS	nConnect;						// DB�ڑ�����			[-1:�����{�A0:����A1:�ُ�]
		EM_SELECT_STATUS	nSearchCoil;					// �R�C�����ь�������	[-1:�����{�A0:����A1:�ُ�]
	};

public:
	//------------------------------------------------------
	// ���\�b�h��`
	//------------------------------------------------------
	// �R���X�g���N�^
	PreDBSelectManager(CString strTaskSection, CString strRetrySection, CString strSqlKey);

	// �f�X�g���N�^
	~PreDBSelectManager(void);

	// ���O�Ǘ��N���X�ݒ�
	void SetLogManager(LogFileManager* pLog){mcls_pLog = pLog;}

	// �O�H���~�σT�[�o�[DB�Z�N�V�����ݒ�
	void SetPreDBSection(CString strSection){m_strDBSectionPre = strSection;}

	// �����T�[�o�[DB�Z�N�V�����ݒ�
	void SetShareDBSection(CString strSection){m_strDBSectionShare = strSection;}

	// �ڑ��T�[�o�[�I�����[�h�擾
	EM_SELECT_DB GetSelectMode(void){return	m_nSelectMode;}

	// �ڑ��T�[�o�[�I����Ԏ擾
	void GetSelectStatus(EM_SELECT_DB nSelectMode, TYP_SELECT_STATUS* typStatus);

	// �O�H���ڑ�DB�I��
	bool SelectConnectDB(char* cPreKizukenNo, OdbcBase* pConnectDB);

	// �O�H���ڑ�DB�I��
	bool SelectConnectDB(CString strPreDB, CString strShareDB, char* cPreKizukenNo, OdbcBase* pConnectDB);

	// �I���T�[�o�[DB�ڑ�
	bool ConnectSelectDB(OdbcBase* pConnectDB);

	// �O�H���~�σT�[�o�[DB�ڑ�
	bool ConnectPreDB(OdbcBase* pConnectDB);

	// �����T�[�o�[DB�ڑ�
	bool ConnectShareDB(OdbcBase* pConnectDB);

	// �O�H���~�σT�[�o�[DB�ڑ��`�F�b�N
	bool CheckPreDB(void);

	// �����T�[�o�[DB�ڑ��`�F�b�N
	bool CheckShareDB(void);

	// DB�ڑ��`�F�b�N
	bool CheckConnectDB(CString strDBSection, EM_SELECT_DB nSelect);

    // 2014.07.25 �쌴  ���ԘA�g    >>>>>>>>
	// �O�H���~�σT�[�o�[�ڑ�
	bool ConnectPreServer(CString strDBSection, char* cPreKizukenNo, OdbcBase* pConnectDB);
	// �����T�[�o�[�ڑ�
	bool ConnectShareServer(CString strDBSection, OdbcBase* pConnectDB);
    // 2014.07.25 �쌴  ���ԘA�g    <<<<<<<<

private:
	//------------------------------------------------------
	// �\���̒�`
	//------------------------------------------------------
	// ���g���C���
	typedef struct TYP_RETRY_INF{
		int		nCount;										// ���g���C��
		int		nTime;										// ���g���C����[ms]
	};

private:
	//------------------------------------------------------
	// ���\�b�h��`
	//------------------------------------------------------
	// Ping�T�[�o�[
	bool PingServer(CString strDBSection, TYP_RETRY_INF* typRetry);

	// DB�ڑ�
	bool ConnectDB(CString strDBSection, TYP_RETRY_INF* typRetry, OdbcBase* pConnectDB);

	// �R�C�����ь���
	bool SearchCoilResult(CString strPreKizukenNo, OdbcBase* pConnectDB);

    // 2014.07.25 �쌴  ���ԘA�g    >>>>>>>>
	// �O�H���~�σT�[�o�[�ڑ�
	//bool ConnectPreServer(CString strDBSection, CString strPreKizukenNo, OdbcBase* pConnectDB);
	// �����T�[�o�[�ڑ�
	//bool ConnectShareServer(CString strDBSection, OdbcBase* pConnectDB);
    // 2014.07.25 �쌴  ���ԘA�g    <<<<<<<<


private:
	//------------------------------------------------------
	// �ϐ���`
	//------------------------------------------------------
	LogFileManager*	mcls_pLog;								// ���O�Ǘ��N���X
	PingManager		mcls_Ping;								// Ping�Ǘ��N���X

	EM_SELECT_DB	m_nSelectMode;							// �ڑ��T�[�o�[�I�����[�h

	CString			m_strTaskSection;						// ���^�X�N�Z�N�V����[KizuTask.ini]
	CString			m_strRetrySection;						// ���g���C�Z�N�V����[KizuTask.ini]
	CString			m_strCoilResult;						// �R�C�����уe�[�u����[COIL_RESULT]
	CString			m_strSqlKey;							// ���ь���SQL�L�[
	CString			m_strSqlFmt;							// ���ь���SQL�t�H�[�}�b�g

	// �O�H���~�σT�[�o�[
	CString				m_strDBSectionPre;					// DB�Z�N�V����[KizuTask.ini]
	TYP_RETRY_INF		m_typPingRetryPre;					// Ping���g���C
	TYP_RETRY_INF		m_typConnectRetryPre;				// DB�ڑ����g���C
	TYP_SELECT_STATUS	m_typStatusPre;						// �I�����

	// �����T�[�o�[
	CString				m_strDBSectionShare;				// DB�Z�N�V����[KizuTask.ini]
	TYP_RETRY_INF		m_typPingRetryShare;				// Ping���g���C
	TYP_RETRY_INF		m_typConnectRetryShare;				// DB�ڑ����g���C
	TYP_SELECT_STATUS	m_typStatusShare;					// �I�����
};
