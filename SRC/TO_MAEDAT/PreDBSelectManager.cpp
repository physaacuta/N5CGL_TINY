#include "StdAfx.h"
#include "PreDBSelectManager.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//----------------------------------------------------------
// �R���X�g���N�^
//----------------------------------------------------------
//	[IN]	CString	strTaskSection	���^�X�N�Z�N�V����
//	[IN]	CString	strRetrySection	���g���C�Z�N�V����
//	[IN]	CString	strSqlKey		���ь���SQL�L�[
//----------------------------------------------------------
PreDBSelectManager::PreDBSelectManager(CString strTaskSection, CString strRetrySection, CString strSqlKey)
{
	char	cTmp[128];

	// ������
	m_strTaskSection	= strTaskSection;					// ���^�X�N	�Z�N�V����[KizuTask.ini]
	m_strRetrySection	= strRetrySection;					// ���g���C�Z�N�V����[KizuTask.ini]
	m_strSqlKey			= strSqlKey;						// ���ь���SQL�L�[

	mcls_pLog		= NULL;									// ���O�Ǘ��N���X
	m_nSelectMode	= NON_DB;								// �ڑ��T�[�o�[�I�����[�h

	// �O�H���~�σT�[�o�[�I�����
	m_typStatusPre.nPing		= SELECT_NON;
	m_typStatusPre.nConnect		= SELECT_NON;
	m_typStatusPre.nSearchCoil	= SELECT_NON;

	// �����T�[�o�[�I�����
	m_typStatusShare.nPing			= SELECT_NON;
	m_typStatusShare.nConnect		= SELECT_NON;
	m_typStatusShare.nSearchCoil	= SELECT_NON;

	//----------------------------------
	// �O�H���~�σT�[�o�[
	//----------------------------------
	// PING���g���C��
	m_typPingRetryPre.nCount	= GetPrivateProfileInt(m_strRetrySection, "PING_RETRY_COUNT_PRE", 3, TASKINI_NAME);
	// PING���g���C�҂�����[ms]
	m_typPingRetryPre.nTime		= GetPrivateProfileInt(m_strRetrySection, "PING_RETRY_TIME_PRE", 1000, TASKINI_NAME);
	// DB�ڑ����g���C��
	m_typConnectRetryPre.nCount	= GetPrivateProfileInt(m_strRetrySection, "CONNECT_RETRY_COUNT_PRE", 1, TASKINI_NAME);
	// DB�ڑ����g���C�҂�����[ms]
	m_typConnectRetryPre.nTime	= GetPrivateProfileInt(m_strRetrySection, "CONNECT_RETRY_TIME_PRE", 1000, TASKINI_NAME);

	//----------------------------------
	// �����T�[�o�[
	//----------------------------------
	// PING���g���C��
	m_typPingRetryShare.nCount		= GetPrivateProfileInt(m_strRetrySection, "PING_RETRY_COUNT_SHARE", 3, TASKINI_NAME);
	// PING���g���C�҂�����[ms]
	m_typPingRetryShare.nTime		= GetPrivateProfileInt(m_strRetrySection, "PING_RETRY_TIME_SHARE", 1000, TASKINI_NAME);
	// DB�ڑ����g���C��
	m_typConnectRetryShare.nCount	= GetPrivateProfileInt(m_strRetrySection, "CONNECT_RETRY_COUNT_SHARE", 1, TASKINI_NAME);
	// DB�ڑ����g���C�҂�����[ms]
	m_typConnectRetryShare.nTime	= GetPrivateProfileInt(m_strRetrySection, "CONNECT_RETRY_TIME_SHARE", 1000, TASKINI_NAME);

	// ���ь���SQL�t�H�[�}�b�g
	m_strSqlFmt.Empty();
	GetPrivateProfileString(m_strTaskSection, m_strSqlKey, "", cTmp, sizeof(cTmp), TASKINI_NAME);
	if( strlen(cTmp) > 0 ){				// SQL
		m_strSqlFmt	= cTmp;
	}
}

//----------------------------------------------------------
// �f�X�g���N�^
//----------------------------------------------------------
PreDBSelectManager::~PreDBSelectManager(void)
{
}

//----------------------------------------------------------
// �ڑ��T�[�o�[�I����Ԏ擾
//----------------------------------------------------------
//	[IN]	EM_SELECT_DB		nSelectMode	DB�I�����[�h
//	[OUT]	TYP_SELECT_STATUS*	typStatus	�I�����
//----------------------------------------------------------
void PreDBSelectManager::GetSelectStatus(EM_SELECT_DB nSelectMode, TYP_SELECT_STATUS* typStatus)
{
	// ������
	typStatus->nPing		= SELECT_NON;
	typStatus->nConnect		= SELECT_NON;
	typStatus->nSearchCoil	= SELECT_NON;

	switch( nSelectMode ){
		case PRE_DB:
			typStatus->nPing		= m_typStatusPre.nPing;
			typStatus->nConnect		= m_typStatusPre.nConnect;
			typStatus->nSearchCoil	= m_typStatusPre.nSearchCoil;
			break;
		case SHARE_DB:
			typStatus->nPing		= m_typStatusShare.nPing;
			typStatus->nConnect		= m_typStatusShare.nConnect;
			typStatus->nSearchCoil	= m_typStatusShare.nSearchCoil;
			break;
	}
}

//----------------------------------------------------------
// �O�H���ڑ�DB�I��
//----------------------------------------------------------
//	[IN]	char*		cPreKizukenNo	�O�H���r���Ǘ���
//	[OUT]	OdbcBase*	pConnectDB		�ڑ�DB
//----------------------------------------------------------
//	�߂�l	bool
//			true	����
//			false	�ُ�
//----------------------------------------------------------
bool PreDBSelectManager::SelectConnectDB(char* cPreKizukenNo, OdbcBase* pConnectDB)
{
	bool	bRet;

	// �O�H���ڑ�DB�I��
	bRet	= SelectConnectDB(m_strDBSectionPre, m_strDBSectionShare, cPreKizukenNo, pConnectDB);

	return bRet;
}

//----------------------------------------------------------
// �O�H���ڑ�DB�I��
//----------------------------------------------------------
//	[IN]	CString		strPreDB,		�O�H���~�σT�[�o�[DB�Z�N�V����
//	[IN]	CString		strShareDB,		�����T�[�o�[DB�Z�N�V����
//	[IN]	char*		cPreKizukenNo	�O�H���r���Ǘ���
//	[OUT]	OdbcBase*	pConnectDB		�ڑ�DB
//----------------------------------------------------------
//	�߂�l	bool
//			true	����
//			false	�ُ�
//----------------------------------------------------------
bool PreDBSelectManager::SelectConnectDB(CString strPreDB, CString strShareDB, char* cPreKizukenNo, OdbcBase* pConnectDB)
{
	bool	bRet;

    // 2014.07.25 �쌴  ���ԘA�g    >>>>>>>>
	//CString	strPreKizukenNo(cPreKizukenNo, SIZE_KIZUKEN_NO);

    // ������
	m_nSelectMode	= NON_DB;			// ���I��

	// �O�H���~�σT�[�o�[�ڑ�
	bRet	= ConnectPreServer(strPreDB, cPreKizukenNo, pConnectDB);
	if( ! bRet ){
		// �����T�[�o�[�ڑ�
		bRet	= ConnectShareServer(strShareDB, pConnectDB);
	}
    // 2014.07.25 �쌴  ���ԘA�g    <<<<<<<<

	return bRet;
}

//----------------------------------------------------------
// �I���T�[�o�[DB�ڑ�
//----------------------------------------------------------
//	[OUT]	OdbcBase*	pConnectDB	�ڑ�DB
//----------------------------------------------------------
//	�߂�l	bool
//			true	����
//			false	�ُ�
//----------------------------------------------------------
bool PreDBSelectManager::ConnectSelectDB( OdbcBase* pConnectDB)
{
	bool	bRet;

	// �I���T�[�o�[����
	switch( m_nSelectMode ){
		case PRE_DB:	// �O�H���~��DB
			bRet	= ConnectPreDB(pConnectDB);
			break;
		case SHARE_DB:	// ����DB
			bRet	= ConnectShareDB(pConnectDB);
			break;
		default:		// ���I��
			bRet	= false;
			break;
	}

	return bRet;
}

//----------------------------------------------------------
// �O�H���~�σT�[�o�[DB�ڑ�
//----------------------------------------------------------
//	[OUT]	OdbcBase*	pConnectDB	�ڑ�DB
//----------------------------------------------------------
//	�߂�l	bool
//			true	����
//			false	�ُ�
//----------------------------------------------------------
bool PreDBSelectManager::ConnectPreDB(OdbcBase* pConnectDB)
{
	bool	bRet;

	// DB�ڑ�
	bRet	= ConnectDB(m_strDBSectionPre, &m_typConnectRetryPre, pConnectDB);

	return bRet;
}

//----------------------------------------------------------
// �����T�[�o�[DB�ڑ�
//----------------------------------------------------------
//	[OUT]	OdbcBase*	pConnectDB	�ڑ�DB
//----------------------------------------------------------
//	�߂�l	bool
//			true	����
//			false	�ُ�
//----------------------------------------------------------
bool PreDBSelectManager::ConnectShareDB(OdbcBase* pConnectDB)
{
	bool	bRet;

	// DB�ڑ�
	bRet	= ConnectDB(m_strDBSectionShare, &m_typConnectRetryShare, pConnectDB);

	return bRet;
}

//----------------------------------------------------------
// �O�H���~�σT�[�o�[DB�ڑ��`�F�b�N
//----------------------------------------------------------
//	�߂�l	bool
//			true	����
//			false	�ُ�
//----------------------------------------------------------
bool PreDBSelectManager::CheckPreDB(void)
{
	bool	bRet;

	// �T�[�o�[DB�ڑ��`�F�b�N
	bRet	= CheckConnectDB(m_strDBSectionPre, PRE_DB);

	return bRet;
}

//----------------------------------------------------------
// �����T�[�o�[DB�ڑ��`�F�b�N
//----------------------------------------------------------
//	�߂�l	bool
//			true	����
//			false	�ُ�
//----------------------------------------------------------
bool PreDBSelectManager::CheckShareDB(void)
{
	bool	bRet;

	// �T�[�o�[DB�ڑ��`�F�b�N
	bRet	= CheckConnectDB(m_strDBSectionShare, SHARE_DB);

	return bRet;
}

//----------------------------------------------------------
// DB�ڑ��`�F�b�N
//----------------------------------------------------------
//	[IN]	CString			strDBSection	DB�Z�N�V����
//	[IN]	EM_SELECT_DB	nSelect			�I�����[�h
//----------------------------------------------------------
//	�߂�l	bool
//			true	����
//			false	�ُ�
//----------------------------------------------------------
bool PreDBSelectManager::CheckConnectDB(CString strDBSection, EM_SELECT_DB nSelect)
{
	bool		bRet;

	TYP_RETRY_INF	typPingRetry;
	TYP_RETRY_INF	typConnectRetry;

	OdbcBase	clsDB;

	// ������
	typPingRetry.nCount		= 1;
	typPingRetry.nTime		= 1000;
	typConnectRetry.nCount	= 1;
	typConnectRetry.nTime	= 1000;

	// ���g���C�ݒ�
	switch( nSelect ){
		case PRE_DB:
			typPingRetry	= m_typPingRetryPre;
			typConnectRetry	= m_typConnectRetryPre;
			break;
		case SHARE_DB:
			typPingRetry	= m_typPingRetryShare;
			typConnectRetry	= m_typConnectRetryShare;
			break;
	}

	// Ping�T�[�o�[
	bRet	= PingServer(strDBSection, &typPingRetry);
	if( bRet ){
		// ���g���C��
		for(int ii = 0; ii < typConnectRetry.nCount; ii++){
			// DB�ڑ��`�F�b�N
			bRet	= clsDB.OpenCheck(TASKINI_NAME,strDBSection);
			if( bRet ){						// �ڑ�OK
				break;
			} else{							// �ڑ�NG
				Sleep(typConnectRetry.nTime);
			}
		}
	}

	return bRet;
}

//----------------------------------------------------------
// Ping�T�[�o�[
//----------------------------------------------------------
//	[IN]	CString			strDBSection	DB�Z�N�V����
//	[IN]	TYP_RETRY_INF*	typRetry		���g���C���
//----------------------------------------------------------
//	�߂�l	bool
//			true	����
//			false	�ُ�
//----------------------------------------------------------
bool PreDBSelectManager::PingServer(CString strDBSection, TYP_RETRY_INF* typRetry)
{
	bool	bRet;

	char	cTmp[128];

	CString	strIP;

	// ������
	bRet	= false;

	// IP�A�h���X�擾
	GetPrivateProfileString(strDBSection, "IPADDLESS", "", cTmp, sizeof(cTmp), TASKINI_NAME);
	if( strlen(cTmp) > 0 ){				// IP�A�h���X�L��
		strIP	= cTmp;
		// ���g���C��
		for(int ii = 0; ii < typRetry->nCount; ii++){
			// Ping���s
			bRet	= mcls_Ping.Ping(strIP, typRetry->nTime);
			if( bRet ){					// Ping���B
				break;
			}
		}
	}

	if( ! bRet ){
		LOG(em_ERR), "[PreDBSelectManager] Ping�G���[ [%s]", strDBSection);
	}

	return bRet;
}

//----------------------------------------------------------
// DB�ڑ�
//----------------------------------------------------------
//	[IN]	CString			strDBSection	DB�Z�N�V����
//	[IN]	TYP_RETRY_INF*	typRetry		���g���C���
//	[OUT]	OdbcBase*		pConnectDB		�ڑ�DB
//----------------------------------------------------------
//	�߂�l	bool
//			true	����
//			false	�ُ�
//----------------------------------------------------------
bool PreDBSelectManager::ConnectDB(CString strDBSection, TYP_RETRY_INF*	typRetry, OdbcBase* pConnectDB)
{
	bool	bRet;

	// ������
	bRet	= false;

	// ���g���C��
	for(int ii = 0; ii < typRetry->nCount; ii++){
		// DB�ڑ�
		bRet	= pConnectDB->Connect(TASKINI_NAME, strDBSection);
		if( bRet ){						// �ڑ�OK
			break;
		} else{							// �ڑ�NG
			Sleep(typRetry->nTime);
		}
	}

	if( ! bRet ){
		LOG(em_ERR), "[PreDBSelectManager] DB�ڑ��G���[ [%s]", strDBSection);
	}

	return bRet;
}

//----------------------------------------------------------
// �R�C�����ь���
//----------------------------------------------------------
//	[IN]	CString		strPreKizukenNo	�O�H���r���Ǘ���
//	[IN]	OdbcBase*	pConnectDB		�ڑ�DB
//----------------------------------------------------------
//	�߂�l	bool
//			true	����
//			false	�ُ�
//----------------------------------------------------------
bool PreDBSelectManager::SearchCoilResult(CString strPreKizukenNo, OdbcBase* pConnectDB)
{
	bool	bRet;

	int		nCnt;

	CString	strTmp;
	CString	strSQL;

	// ������
	bRet	= false;
	nCnt	= 0;

	// SQL�쐬
	if( ! m_strSqlFmt.IsEmpty() ){
		strSQL.Format(m_strSqlFmt,strPreKizukenNo);
		// SQL���s
		bRet	= pConnectDB->GetSelectKey(strSQL, &nCnt);
		if( bRet ){
			if( nCnt > 0 ){
				bRet	= true;
			} else{
				bRet	= false;
			}
		}
	}

	if( ! bRet ){
		LOG(em_ERR), "[PreDBSelectManager] ���ь����G���[ [%s]", strSQL);
	}

	return bRet;
}

//----------------------------------------------------------
// �O�H���~�σT�[�o�[�ڑ�
//----------------------------------------------------------
//	[IN]	CString		strDBSection,	DB�Z�N�V����
//	[IN]	char*		cPreKizukenNo   �O�H���r���Ǘ���
//	[OUT]	OdbcBase*	pConnectDB		�ڑ�DB
//----------------------------------------------------------
//	�߂�l	bool
//			true	����
//			false	�ُ�
//----------------------------------------------------------
bool PreDBSelectManager::ConnectPreServer(CString strDBSection, char* cPreKizukenNo, OdbcBase* pConnectDB)
{
	bool	bRet;

    // 2014.07.25 �쌴  ���ԘA�g    >>>>>>>>
	CString	strPreKizukenNo(cPreKizukenNo, SIZE_KIZUKEN_NO);
    // 2014.07.25 �쌴  ���ԘA�g    <<<<<<<<

    // ������
	m_typStatusPre.nPing		= SELECT_NON;
	m_typStatusPre.nConnect		= SELECT_NON;
	m_typStatusPre.nSearchCoil	= SELECT_NON;

    // Ping�T�[�o�[
	bRet	= PingServer(strDBSection, &m_typPingRetryPre);
	if( bRet ){
		m_typStatusPre.nPing	= SELECT_OK;
		// DB�ڑ�
		bRet	= ConnectDB(strDBSection, &m_typConnectRetryPre, pConnectDB);
		if( bRet ){
			m_typStatusPre.nConnect	= SELECT_OK;
			// �R�C�����у`�F�b�N
			bRet	= SearchCoilResult(strPreKizukenNo, pConnectDB);
			if( bRet ){
				m_typStatusPre.nSearchCoil	= SELECT_OK;
			} else{
				m_typStatusPre.nSearchCoil	= SELECT_NG;
				pConnectDB->DisConnect();					// DB�ؒf
			}
		} else{
			m_typStatusPre.nConnect	= SELECT_NG;
		}
	} else{
		m_typStatusPre.nPing	= SELECT_NG;
	}

    // 2014.07.25 �쌴  ���ԘA�g    >>>>>>>>
    // �ڑ����[�h
    if( bRet ){
        m_nSelectMode	= PRE_DB;		// �O�H���~��DB
    }else{
        m_nSelectMode	= NON_DB;		// ���I��
    }
    // 2014.07.25 �쌴  ���ԘA�g    <<<<<<<<

	return bRet;
}

//----------------------------------------------------------
// �����T�[�o�[�ڑ�
//----------------------------------------------------------
//	[IN]	CString		strDBSection	DB�Z�N�V����
//	[OUT]	OdbcBase*	pConnectDB		�ڑ�DB
//----------------------------------------------------------
//	�߂�l	bool
//			true	����
//			false	�ُ�
//----------------------------------------------------------
bool PreDBSelectManager::ConnectShareServer(CString strDBSection, OdbcBase* pConnectDB)
{
	bool	bRet;

	// ������
	m_typStatusShare.nPing			= SELECT_NON;
	m_typStatusShare.nConnect		= SELECT_NON;
	m_typStatusShare.nSearchCoil	= SELECT_NON;

	// Ping�T�[�o�[
	bRet	= PingServer(strDBSection, &m_typPingRetryShare);
	if( bRet ){
		m_typStatusShare.nPing	= SELECT_OK;
		// DB�ڑ�
		bRet	= ConnectDB(strDBSection, &m_typConnectRetryShare, pConnectDB);
		if( bRet ){
			m_typStatusShare.nConnect	= SELECT_OK;
		} else{
			m_typStatusShare.nConnect	= SELECT_NG;
		}
	} else{
		m_typStatusShare.nPing	= SELECT_NG;
	}

    // 2014.07.25 �쌴  ���ԘA�g    >>>>>>>>
    // �ڑ����[�h
    if( bRet ){
        m_nSelectMode	= SHARE_DB;	// ����DB
    }else{
        m_nSelectMode	= NON_DB;	    // ���I��
    }
    // 2014.07.25 �쌴  ���ԘA�g    <<<<<<<<

	return bRet;
}
