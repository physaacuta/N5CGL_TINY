#include "StdAfx.h"
#include "MakeFfManager.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
MakeFfManager::MakeFfManager(void) :
ThreadManager("FF"),
m_bCreateOk(true),
mcls_pProMgr(NULL),
mcls_pDB(NULL)
{
	//// INI�t�@�C������萔�捞
	// �R�C�������̑Ώۊ���
	m_nChkCoilDate = GetPrivateProfileInt(TO_PUTDAT, "CHKCOILDATE", 1, TASKINI_NAME);
	// �R�C�����ъm�F�̃��g���C��
	m_nRetryCount = GetPrivateProfileInt(TO_PUTDAT, "RETRYCOUNT", 15, TASKINI_NAME);
	// �R�C�����ъm�F�̃��g���C����
	m_nRetryWaitTime = GetPrivateProfileInt(TO_PUTDAT, "RETRYWAITTIME", 20000, TASKINI_NAME);
	// �̐����і���M�̎��ь�������
	m_nChkUnSentDate = GetPrivateProfileInt(TO_PUTDAT, "CHKUNSENTDATE", 1, TASKINI_NAME);
	m_dHosei = 1.0;
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
MakeFfManager::~MakeFfManager(void)
{
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int MakeFfManager::ThreadFirst()
{
	//// �C�x���g�ݒ�
	HANDLE hArray[] =
	{
		gque_Deli.g_evSem 
	};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int MakeFfManager::ThreadLast()
{
	DELIVERY_KEY *pDeli;				// �L���[�f�[�^

	// �������̃L���[��j��
	while (true)
	{
		pDeli = gque_Deli.GetFromHead(0);
		if( pDeli == NULL ) break;
		
		// �폜
		LOG(em_WAR), "[%s] �������L���[�폜 [%s]", my_sThreadName, pDeli->cKey);
		SAFE_DELETE(pDeli);
	}
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void MakeFfManager::ThreadEvent(int nEventNo)
{
	DELIVERY_KEY*	pDeli;					// �󂯓n���˗��f�[�^

	////// �V�O�i���̏���
	enum
	{
		EV_QUE = 0,							// �L���[�˗�
	};

	////// �V�O�i����������
	switch (nEventNo)
	{
//-----------------------------------------------------------------------------------------------
	case EV_QUE: 							// �L���[�˗�
		if (true)
		{
			pDeli = gque_Deli.GetFromHead(); 
			bool retc = Exec(pDeli->cKey);
			// �������ϐ� ������
			ClearMember();
			SAFE_DELETE(pDeli);
			
			// �@����
			//if(m_bCreateOk != retc)
			//{
				m_bCreateOk = retc;
				KizuFunction::SetStatus("ZI_CREATE_FF_ERR_ID", retc, false);	// �V�X���O�\���͏o���Ȃ�
			//}
		}
		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo);
		return;
	}
}

//------------------------------------------
// �������ϐ� ������
//------------------------------------------
void MakeFfManager::ClearMember()
{
	// �������ϐ� ������
	SAFE_DELETE(mcls_pDB);
	memset(m_cKizukenNo, 0x00, sizeof(m_cKizukenNo));
	memset(&m_typBuf, 0x00, sizeof(m_typBuf));
}

//------------------------------------------
// �e�e�T�[�o�[�������� �`������
// char const* cID BUF_PRO_R_SAISUN�̃L�[
// �߂�l ���A���
//------------------------------------------
bool MakeFfManager::Exec(char const* cID)
{
	FF_JISSEKI			typZi[2];						// FF�p �r�������ѓ`��
	FF_INF				typDefect[2];					// FF�ɑ��M����\���������r���
	memset(typDefect, 0x00, sizeof(typDefect));
	int					nZiErr = 0;						// ���э쐬�����G���[(0:���� 1:�ُ�)
	int					nIsWarm = 0;					// �E�H�[�}�ދ敪 (0:��ʍ� 1:�E�H�[�}��)
	int					nIsResult = 0;					// �R�C�����їL�� (0:�L�� 1:����)
	int					nIsSendOk = 0;					// �O�����ё��M���� (0:���M 1:���M���Ȃ�)	��4PL�Ή�
	int					pcode[NUM_MEN];					// �����p�^�[�� (0�I���W��)

	FF_STS_JISSEKI		typSts[2];						// FF�p ������ԓ`��
	FF_STS				typPosition[2];					// �R�C���ʒu���
	memset(typPosition, 0x00, sizeof(typPosition));

	//// �������ϐ� ������
	ClearMember();

	//// ODBC�ڑ��N���X �C���X�^���X����
	mcls_pDB = new OdbcBase();
	if (!mcls_pDB->Connect(TASKINI_NAME))
	{
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "[%s]", my_sThreadName);
		syslog(356, "[ID=%s DB�ڑ��G���[]", cID);
		return false;
	}

	//// �v���R���̐����ю�M�o�b�t�@�e�[�u���擾
	if (!GetBufferSai(cID))
	{
		// �擾���s
		LOG(em_ERR), "[%s] �̐����ю�M�o�b�t�@�e�[�u���f�[�^�Ȃ� [ID=%s]", my_sThreadName, cID);
		syslog(356, "[ID=%s �̐����ъY������]", cID);
		return false;
	}

	//// ����������M�����̐����т̃O�����g�t���O���ُ�
	if (PRO_GRAND_OK != m_typBuf.data.Common.nGrandFlg)
	{
		LOG(em_WAR), "[%s] �̐����� �O�����g�t���O�ُ� [%d]", my_sThreadName, m_typBuf.data.Common.nGrandFlg);
		syslog(356, "[ID=%s �̐����уO�����g�t���O�ُ�]", cID);
		//nZiErr = 1;		// �O�����g�t���O���ُ펞���������т��ُ�Ƃ��đ��M����
	}

	//// �R�C�����擾
	if (!GetCoilInf_KizukenNo())
	{
		LOG(em_ERR), "[%s] �Y���R�C���Ȃ� [�R�C��No=%.12s][ID=%s]", my_sThreadName, m_typBuf.data.KeyInf.cCoilNo, cID);
		syslog(356, "[ID=%s �R�C�����Y������]", cID);
		nZiErr = 1;
	}

	//// FF���M�r���Ɠ��肷��ׂɃp�����[�^���擾
	if (!ReadParam()) {
		LOG(em_ERR), "[%s] �p�����[�^�ǂݍ��ݎ��s�ɂ�� ���ѓ`���쐬�����Ȃ� [�L�[���=%.14s]", my_sThreadName, &m_typBuf.data.KeyInf);
		syslog(356, "[�Ǘ�No=%s:�p�����[�^�擾���s]", m_cKizukenNo);	// 8:�������ё��M�`���쐬�ňُ�B
		return false;
	}

	// ���̎��_ nZiErr = 0 �ł���΂őΏۂ̊Ǘ�No�͌��肵�Ă���B�� m_KizukenNo �ɃZ�b�g����Ă���̂�
	// �o�b�N�A�b�v���M�������\
	if( 0 == nZiErr ) {
		// �����M�̃R�C�����т̃o�b�N�A�b�v���M����
		bool retc2 = ExecUnsentResult(m_cKizukenNo);
	}


	if (0 == nZiErr)
	{
		m_nLen = 0;
		//// �R�C�����т��o���オ���Ă��邩�`�F�b�N & �␳���Z�o
		// �u���b�N�����̂Œ���
		if (!CheckCoilResultInsertOk(&nIsWarm, &nIsSendOk, pcode))
		{
			LOG(em_ERR), "[%s] �R�C�����ъY���Ȃ� [�R�C��No=%.12s][ID=%s]", my_sThreadName, m_typBuf.data.KeyInf.cCoilNo, cID);
			//syslog(356, "[ID=%s �R�C�����ъY������]", cID);
			nIsResult = 1;
		}
	}


	//// �R�C�����уe�[�u�� �X�V
	if (0 == nIsResult && 0 == nZiErr)
	{
		if (!UpdateCoilResult())
		{
			LOG(em_ERR), "[%s] �R�C�����уe�[�u��(T_COIL_RESULT) �X�V���s", my_sThreadName);
		}
		else
		{
			LOG(em_MSG), "[%s] �R�C�����уe�[�u��(T_COIL_RESULT) �X�V����", my_sThreadName);
		}
	}

	//// �J�b�g���уe�[�u�� �o�^
	if (0 == nIsResult && 0 == nZiErr)
	{
		if (!InsertCutResult(cID))
		{
			LOG(em_ERR), "[%s] �J�b�g���уe�[�u��(T_COIL_CUT_RESULT) �o�^���s", my_sThreadName);
		}
		else
		{
			LOG(em_MSG), "[%s] �J�b�g���уe�[�u��(T_COIL_CUT_RESULT) �o�^����", my_sThreadName);
		}
	}

	//// �̐����уe�[�u�� �o�^
	if (0 == nIsResult && 0 == nZiErr)
	{
		if (!InsertSaisunResult(cID))
		{
			LOG(em_ERR), "[%s] �̐����уe�[�u��(T_COIL_SAISUN_RESULT) �o�^���s", my_sThreadName);
		}
		else
		{
			LOG(em_MSG), "[%s] �̐����уe�[�u��(T_COIL_SAISUN_RESULT) �o�^����", my_sThreadName);
		}
	}


	//// �v���R���J�b�g���ю�M�o�b�t�@�e�[�u�� �X�V
	if (!UpdateProconBuf(cID))
	{
		LOG(em_ERR), "[%s] �v���R���̐����ю�M�o�b�t�@�e�[�u��(BUF_FF_R_SAISUN) �X�V���s", my_sThreadName);
	}
	else
	{
		LOG(em_MSG), "[%s] �v���R���̐����ю�M�o�b�t�@�e�[�u��(BUF_FF_R_SAISUN) �X�V����", my_sThreadName);
	}

	//// FF���M�p ���я��̐����\�H
	if (1 == nZiErr || 1 == nIsWarm || 1 == nIsSendOk) {
		if (1 == nIsSendOk){	// 4PL�Ή�
			LOG(em_WAR), "[%s] �O�����ё��M����[1:���M���Ȃ�]�̂��ߎ��э쐬�����Ȃ� [�Ǘ�No=%s, �L�[���=%.14s]", my_sThreadName, m_cKizukenNo, &m_typBuf.data.KeyInf);
			syslog(356, "[�Ǘ�No=%s �O�����ё��M����NG]", m_cKizukenNo);	// 4:�������ѓ`�����M�ΏۊO�ł��B
		} else if (1 == nIsWarm) {
			LOG(em_WAR), "[%s] �E�H�[�}�[�ނ̂��ߎ��э쐬�����Ȃ� [�Ǘ�No=%s, �L�[���=%.14s]", my_sThreadName, m_cKizukenNo, &m_typBuf.data.KeyInf);
			syslog(356, "[�Ǘ�No=%s �E�H�[�}�[��]", m_cKizukenNo);	// 4:�������ѓ`�����M�ΏۊO�ł��B
		} else {
			LOG(em_ERR), "[%s] �������ُ�̂��ߎ��э쐬�����Ȃ� [�Ǘ�No=%s, �L�[���=%.14s]", my_sThreadName, m_cKizukenNo, &m_typBuf.data.KeyInf);
		}

		return true;
	}

	//// FF���M�p ���я�񐶐�
	for(int ii=0; ii<NUM_MEN; ii++) {
		if( ! MakeSendData( ii, pcode[ii], nIsResult, &typDefect[ii], &typZi[ii]) ) {
			//// �����ۂɂ� MakeSendData �� False �ƂȂ�����͑��݂��Ȃ�
			LOG(em_ERR), "[%s] <%s> FF�������ѓ`���쐬���s", my_sThreadName, DivNameManager::GetTorB(ii));
			syslog(356, "[�Ǘ�No=%s:�`���쐬���s]", m_cKizukenNo);	// 8:�������ё��M�`���쐬�ňُ�B

			return false;
		}
	}
	//// FF���M�p ������Ԑ���
	for (int ii = 0; ii<NUM_MEN; ii++) {
		if (!MakeSendStsData(ii, pcode[ii], nIsResult, &typPosition[ii], &typSts[ii])) {
			LOG(em_ERR), "[%s] <%s> FF������ԓ`���쐬���s", my_sThreadName, DivNameManager::GetTorB(ii));
			syslog(356, "[�Ǘ�No = %s]", m_cKizukenNo);				// 8:�������ё��M�`���쐬�ňُ�
			return false;
		}
	}

	// ��{�ƂȂ�IDNo�𐶐�       (yyyymmdd hhnnssff\0)
	char cID_Send[SIZE_SERIAL_FF_NO];
	KizuLib::KizuFunction::GetSerialNoFF(cID_Send);	

	//// FF�p�������ѓ`�� �� �o�b�t�@�e�[�u���ɓo�^
	if( InsertZiBuf(cID_Send, typZi, typSts) ) {
		LOG(em_MSG), "[%s] FF�������ё��M�o�b�t�@�e�[�u��(BUF_FF_S_ZISEKI)�o�^����", my_sThreadName);

	} else {
		LOG(em_ERR), "[%s] FF�������ё��M�o�b�t�@�e�[�u��(BUF_FF_S_ZISEKI)�o�^���s", my_sThreadName);
		syslog(356, "[�Ǘ�No=%s:�o�^���s]", m_cKizukenNo);	// 8: �������ё��M�`���쐬�ňُ�B

		return false;
	}

	//// FF���M�r��� �����H�����e�[�u���ɓo�^
	////  ���H���R�C�����e�[�u��(ACCESS_NEXT_COIL_INF)
	////  ���H���r���e�[�u��    (ACCESS_NEXT_DEFECT_INF)
	if (InsertAccessNextInf(&typDefect[0], &typZi[0])) {
		LOG(em_MSG), "[%s] ���H�����e�[�u�� �o�^����", my_sThreadName);
	} else {
		LOG(em_ERR), "[%s] ���H�����e�[�u�� �o�^���s", my_sThreadName);
	}

	//// �e�e�T�[�o�Ɏ��ё��M�v��
	Send_To_FF(cID);


	return true;
}

//------------------------------------------
// �v���R���̐����ю�M�o�b�t�@�e�[�u���擾
// const char *cID �L�[
//------------------------------------------
bool MakeFfManager::GetBufferSai(const char *cID)
{
	//// SQL���쐬
	CString				cSql;
	cSql.Format("SELECT ���`�� FROM %s WHERE ID='%s'", DB_BUF_PRO_R_SAISUN, cID);

	// �f�[�^�擾
	return mcls_pDB->GetSelectImage(cSql, sizeof(PRO_TYPE_SAISUN), (char*)&m_typBuf);
}

//------------------------------------------
// �R�C����񂩂�Ǘ�No���擾
//------------------------------------------
bool MakeFfManager::GetCoilInf_KizukenNo()
{
	CString				cSql;
	CString				cWk;

	//// ���ݓ����擾
	SYSTEMTIME st;
	GetLocalTime(&st);

	//// �R�C����񂩂�Ǘ�No���擾
	cSql  = "SELECT TOP 1 �Ǘ�No FROM ";
	cSql += DB_COIL_INF;
	cSql += " WHERE ";
		cWk.Format("���YNo='%.12s' AND ", m_typBuf.data.KeyInf.cCoilNo);									cSql += cWk;
		cWk.Format("�o�^���t>DATEADD(d,-%d,'%d/%d/%d') ", m_nChkCoilDate, st.wYear, st.wMonth, st.wDay);	cSql += cWk;
	cSql += "ORDER BY �o�^���t DESC";

	if (!mcls_pDB->GetSelectKey(cSql, SIZE_KIZUKEN_NO, m_cKizukenNo))
	{
		// 1��������
		return false;
	}
	
	LOG(em_MSG), "[%s] �R�C��No=%.12s ��T_COIL_INF������� [�Ǘ�No=%s]", my_sThreadName, m_typBuf.data.KeyInf.cCoilNo, m_cKizukenNo);
	return true;
}

//------------------------------------------
// �R�C�����т��o�^�����܂ő҂��B���␳���m��
// int *nIsWarm     �E�H�[�}�ދ敪 (0:��ʍ� 1:�E�H�[�}��)
// �߂�l : false:���݂��Ȃ� 
//------------------------------------------
bool MakeFfManager::CheckCoilResultInsertOk(int *nIsWarm, int* nIsSendOk, int* pcode)
{
	CString				cSql;
	ENUM_END_MODE		emEndMode;			// ���ъi�[�敪
	int					nLen;				// ���������� [mm]
	int					nWk;
	
	cSql.Format("SELECT ���ъi�[�敪, �E�H�[�}�[�ދ敪, �O�����ё��M����, \
				����������, \
				�����p�^�[���R�[�h_�\, �����p�^�[���R�[�h_�� \
				FROM %s \
				WHERE �Ǘ�No='%s'", DB_COIL_RESULT, m_cKizukenNo);

	//// �R�C�����т̃f�[�^�L���m�F
	for (int ii=0; ii<m_nRetryCount; ii++)
	{
		if (!mcls_pDB->ExecuteDirect(cSql))
		{
			LOG(em_ERR), "[%s] �R�C������ �擾�G���[ [�Ǘ�No=%s]", my_sThreadName, m_cKizukenNo);
			LOG(em_NON), "%s", cSql);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", DB_COIL_RESULT, m_cKizukenNo);
			return false;
		}
		int sqlrc = mcls_pDB->Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK)						// �f�[�^�L��
		{
			mcls_pDB->GetData(1, (int *)&emEndMode);		// ���ъi�[�敪
			mcls_pDB->GetData(2, nIsWarm);					// �E�H�[�}�ދ敪
			mcls_pDB->GetData(3, nIsSendOk);				// �O�����ё��M����
			mcls_pDB->GetData(4, &nLen);					// ����������
			mcls_pDB->GetData(5, &nWk);						// �����p�^�[���R�[�h_�\
			pcode[0] = nWk - 1;								// (0�I���W��)
			mcls_pDB->GetData(6, &nWk);						// �����p�^�[���R�[�h_��
			pcode[1] = nWk - 1;								// (0�I���W��)

			m_nLen = nLen;									// �������сA������ԓ`���쐬���Ɏg�p�B�ُ펞�͑O��l�g�p
			break;
		}
		else if (sqlrc == KIZUODBC_FETCH_NODATE)			// �f�[�^����
		{
			// �Ō�܂� �R�C�����т�������Ȃ�
			if (ii >= m_nRetryCount - 1)
			{
				LOG(em_ERR), "[%s] �R�C������ ���� [�Ǘ�No=%s]", my_sThreadName, m_cKizukenNo);
				syslog(356, "�R�C�����і��� [�R�C��No=%.12s][�Ǘ�No=%s]", m_typBuf.data.KeyInf.cCoilNo, m_cKizukenNo);
				return false;
			}
			LOG(em_WAR), "[%s] �R�C������ ������ ��: %d", my_sThreadName, ii + 1);
			Sleep(m_nRetryWaitTime);
		}
		else												// �G���[
		{
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", m_cKizukenNo);
			return false;
		}
	}

	//------------------------------------------
	// �␳���v�Z
	//------------------------------------------
	double	dWk;
	double 	dHani;						// �v���R���R�C������r�ُ����l [%]
	////m_dHosei = 1.0;						// �␳�l�f�t�H���g���Z�b�g ���␳���ُ펞�͑O��l���g�p
	double	dHosei;						// �␳��		

	cSql.Format("SELECT �v���R���R�C������r�ُ����l FROM %s", DB_COMMON_MASTER);
	if( ! mcls_pDB->ExecuteDirect(cSql)) {
		LOG(em_ERR), "[%s] ��r�ُ����l �擾�G���[ [�Ǘ�No=%s]", my_sThreadName, m_cKizukenNo);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", DB_COMMON_MASTER, m_cKizukenNo);
		return true;
	}
	int sqlrc = mcls_pDB->Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

		mcls_pDB->GetData( 1, &dHani);					// �v���R���R�C������r�ُ����l

	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
		LOG(em_ERR), "[%s] ��r�ُ����l ���� [%.20s]", my_sThreadName, m_cKizukenNo);
		LOG(em_NON), "%s", cSql);
		return true;
	} else {								// �G���[
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", m_cKizukenNo);
		return true;
	}

	//------------------------------------------
	// PLG������r�ُ����l[%] 0�̏ꍇ�A�␳���v�Z���s��Ȃ�
	//------------------------------------------
	if (0 == dHani) {
		LOG(em_MSG), "[%s] �␳���v�Z �����{ [PLG������r�ُ����l=0]", my_sThreadName);
		syslog(353, "[�Ǘ�No=%s]", m_cKizukenNo);
		// �@���Ԃ𐳏�֍X�V����
		KizuFunction::SetStatus("HOSEIRITU_ID", true, false);
		return true;
	}

	//------------------------------------------
	// �v���R���������`�F�b�N
	//------------------------------------------
	if (0 >= m_typBuf.data.nLen)
	{
		LOG(em_ERR), "[%s] �v���R�������� �������ُ� [%.1fm][�Ǘ�No=%s]", my_sThreadName, (double)m_typBuf.data.nLen / 10.0, m_cKizukenNo);
		syslog(351, "[�v���R����=%.1fm][�R�C��No=%.12s][�Ǘ�No=%s]", (double)m_typBuf.data.nLen / 10.0, m_typBuf.data.KeyInf.cCoilNo, m_cKizukenNo);
		// �@���Ԃ��ُ�֍X�V����
		KizuFunction::SetStatus("HOSEIRITU_ID", false, false);
		return true;
	}

	//------------------------------------------
	// �␳���v�Z
	//------------------------------------------
	// �����������`�F�b�N
	if (0 >= m_nLen)
	{
		LOG(em_ERR), "[%s] ���������� �������ُ� [%.3fm][�Ǘ�No=%s]", my_sThreadName, (double)m_nLen / 1000.0, m_cKizukenNo);
		syslog(352, "[����������=%.3fm][�R�C��No=%.12s][�Ǘ�No=%s]", (double)m_nLen / 1000.0, m_typBuf.data.KeyInf.cCoilNo, m_cKizukenNo);
		// ���ъi�[�敪�� �ʏ�ؑ� �̏ꍇ�̂�
		if (DIV_END_NORMAL == emEndMode)
		{
			// �@���Ԃ��ُ�֍X�V����
			KizuFunction::SetStatus("HOSEIRITU_ID", false, false);
		}
		//return true;
	}
	else
	{
		// �␳���v�Z
		dHosei = (double)m_typBuf.data.nLen * 100.0 / (double)m_nLen;

		// �␳���v�Z���ʍ����̐�Βl(%)���擾
		if (1.0 < dHosei)
		{
			dWk = (dHosei - 1.0) * 100.0;
		}
		else
		{
			dWk = (1.0 - dHosei) * 100.0;
		}

		//------------------------------------------
		// �␳���͈͔���
		//------------------------------------------
		if (0 != dHani && dHani < dWk)
		{
			// �͈͊O
			LOG(em_ERR), "[%s] �␳���v�Z �ُ� [�␳��(%f) = �v���R����=%.1fm / ����������=%.3fm]", my_sThreadName, dHosei, (double)m_typBuf.data.nLen / 10.0, (double)m_nLen / 1000.0);
			syslog(351, "[%lf%% = �v���R����=%.1fm / ����������=%.3fm][�R�C��No=%.12s][�Ǘ�No=%s]", dHosei*100, (double)m_typBuf.data.nLen / 10.0, (double)m_nLen / 1000.0, m_typBuf.data.KeyInf.cCoilNo, m_cKizukenNo);
			// ���ъi�[�敪�� �ʏ�ؑ� �̏ꍇ�̂�
			if (DIV_END_NORMAL == emEndMode)
			{
				// �@���Ԃ��ُ�֍X�V����
				KizuFunction::SetStatus("HOSEIRITU_ID", false, false);
			}
		}
		else
		{
			// �͈͓�
			m_dHosei = dHosei;
			LOG(em_MSG), "[%s] �␳���v�Z [�␳��(%f) = �v���R����=%.1fm / ����������=%.3fm]", my_sThreadName, m_dHosei, (double)m_typBuf.data.nLen / 10.0, (double)m_nLen / 1000.0);
			syslog(350, "[%lf%% = �v���R����=%.1fm / ����������=%.3fm][�R�C��No=%.12s][�Ǘ�No=%s]", m_dHosei*100, (double)m_typBuf.data.nLen / 10.0, (double)m_nLen / 1000.0, m_typBuf.data.KeyInf.cCoilNo, m_cKizukenNo);
			// �v���R���p�̃N���X�ɂ��Z�b�g���Ă�����
			mcls_pProMgr->SetHosei(m_dHosei);
			// �@���Ԃ𐳏�֍X�V����
			KizuFunction::SetStatus("HOSEIRITU_ID", true, false);
		}
	}

	return true;
}



//------------------------------------------
// �p�����[�^�Ǎ�
//------------------------------------------
bool MakeFfManager::ReadParam()
{
	int id;
	int typid;
	int no;
	CString				cSql;


	//===========================
	// PARAM_PCODE & PCOMMON_OUTPUT
	//===========================
	PARAM_PCODE	buf;
	memset(&buf, 0x00, sizeof(buf));

	//// PARAM_PCODE
	cSql = "SELECT pcode, �r��EdasysID, FF���M�L�� ";
	cSql += "FROM ";
	cSql += DB_PCODE_OUTPUT;
	cSql += " ORDER BY pcode ASC, �r��EdasysID ASC";

	if (!mcls_pDB->ExecuteDirect(cSql)) {
		LOG(em_ERR), "[%s] [%s SELECT�G���[]", my_sThreadName, DB_PCODE_OUTPUT);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", DB_PCODE_OUTPUT);
		return false;
	}
	while (true) {
		int sqlrc = mcls_pDB->Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

			// ��L�[
			mcls_pDB->GetData(1, &id);						// pcode
			
			if (MAX_PCODE < id || 0 >= id) {
				// ��L�[�s��
				LOG(em_ERR), "[%s] [%s ��L�[�s�� id=%d]", my_sThreadName, DB_PCODE_OUTPUT, id);
				syslog(SYSNO_DB_DATA_ERR, "[%s id=%d]", DB_PCODE_OUTPUT, id);
				continue;
			}
			mcls_pDB->GetData(2, &typid);					// �r��EdasysID
			if (-1 == typid) {
				// �f�t�H���g
				buf.rec[id-1].typDefput.nEdasysID = typid;
				mcls_pDB->GetData(3, (int*)&buf.rec[id-1].typDefput.nKind);	// FF���M�L��
			} else {
				no = buf.rec[id-1].nPutNum;
				buf.rec[id-1].typOutput[no].nEdasysID = typid;
				mcls_pDB->GetData(3, (int*)&buf.rec[id-1].typOutput[no].nKind);	// FF���M�L��
				buf.rec[id-1].nPutNum = no + 1;
			}
		} else if (sqlrc == KIZUODBC_FETCH_NODATE) {		// �f�[�^����
			break;
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", m_cKizukenNo);
			return false;
		}
	}

	// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy(&m_Param_Pcode, &buf, sizeof(buf));
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, DB_PCODE_OUTPUT);

	//===========================
	// PARAM_GRADE
	//===========================
	PARAM_GRADE	grade;
	memset(&grade, 0x00, sizeof(grade));

	cSql =  "SELECT";
	cSql += " EdasysID, �D��x, ���׃O���[�h";
	cSql += " FROM ";
	cSql += DB_GRADE_NAME;
	cSql += " ORDER BY ���׃O���[�h DESC, �D��x ASC";

	if( ! mcls_pDB->ExecuteDirect(cSql)) {
		LOG(em_ERR), "[%s] [%s SELECT�G���[]", my_sThreadName, DB_GRADE_NAME);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", DB_GRADE_NAME);
		return false;
	}
	for(int ii = 0; ii < MAX_GRADE; ii++){
		int sqlrc = mcls_pDB->Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

			// ��L�[
			mcls_pDB->GetData(1, &id);						// EdasysID
			if( 0 >= id ) {
				// ��L�[�s��
				LOG(em_ERR), "[%s] [%s ��L�[�s�� EdasysID=%d]", my_sThreadName, DB_GRADE_NAME, id);
				syslog(SYSNO_DB_DATA_ERR, "[%s EdasysID=%d]", DB_GRADE_NAME, id);					
				continue;
			}
			
			// �f�[�^��
			grade.typGrade_id[ii].nEdasysID = id;							// EdasysID
			mcls_pDB->GetData(2, &grade.typGrade_id[ii].nPriority );		// �D��x
			mcls_pDB->GetData(3, &grade.typGrade_id[ii].nRank);				// ���׃O���[�h

			grade.nNum++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", DB_GRADE_NAME);
			return false;
		}
	}

	// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy( &m_Param_Grade, &grade, sizeof(grade));
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, DB_GRADE_NAME);

	//===========================
	// PARAM_COMMON_MAST
	//===========================
	PARAM_COMMON_MAST	common;
	memset ( &common, 0x00, sizeof(common));

	common.nEdgeNgCount = 0;
	cSql =  "SELECT";
	cSql += " �G�b�W������_�G�b�W���o�ُ�FF���M";
	cSql += " FROM ";
	cSql += DB_COMMON_MASTER;

	if( ! mcls_pDB->ExecuteDirect(cSql)) {
		LOG(em_ERR), "[%s] [%s SELECT�G���[]", my_sThreadName, DB_COMMON_MASTER);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", DB_COMMON_MASTER);
		return false;
	}

	int sqlrc = mcls_pDB->Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��
	
		// �f�[�^��
		mcls_pDB->GetData(1, &common.nEdgeNgCount);

	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
		return false;	
	} else {								// �G���[
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", DB_COMMON_MASTER);
		return false;
	}

	// �擾�f�[�^�������o�[�ϐ��ɃZ�b�g
	memcpy( &m_Param_CommonMast, &common, sizeof(common));
	LOG(em_MSG), "[%s] %s �擾����", my_sThreadName, DB_COMMON_MASTER);
	return true;
}



//------------------------------------------
// FF���M�p ���ѓ`������
// int ou 0:�\
// int nIsResult �R�C�����їL�� (0:�L�� 1:����)
// FF_INF * pInf �ҏW���[�N
// FF_JISSEKI* pTypZi �e�ʂ݂̂̎��ѓ`��
// �߂�l : ���A���
//------------------------------------------
bool MakeFfManager::MakeSendData(int ou, int pcode, int nIsResult, FF_INF* pInf, FF_JISSEKI* pTypZi)
{
	CString cWk;
	int		nWk;

	//// ������
	memset( pTypZi, 0x20, sizeof(FF_JISSEKI));


	//===========================
	// �w�b�_�[��
	//===========================
	if(0 == ou) memcpy( pTypZi->Hd.Header_Code, FF_TC_CODE_JI_O, sizeof(pTypZi->Hd.Header_Code) );
	else		memcpy( pTypZi->Hd.Header_Code, FF_TC_CODE_JI_U, sizeof(pTypZi->Hd.Header_Code) );
	memcpy(pTypZi->Hd.Rec_Code, Z_FF_RENBAN, sizeof(pTypZi->Hd.Rec_Code));

	// ���M����
	SYSTEMTIME STime;
	GetLocalTime(&STime);
	cWk.Format("%04d%02d%02d%02d%02d%02d", STime.wYear, STime.wMonth, STime.wDay, STime.wHour, STime.wMinute, STime.wSecond); 
	memcpy(pTypZi->Hd.Date, cWk, sizeof(pTypZi->Hd.Date));

	// �H���R�[�h ( 5CGL=85 )
	memcpy(pTypZi->Hd.Line_Code, KOUTEI_CODE, sizeof(pTypZi->Hd.Line_Code));

	// ���ߔԍ�
	memcpy(pTypZi->Hd.Meino, &m_typBuf.data.KeyInf.cMeiNo, sizeof(pTypZi->Hd.Meino));

	// �≄�R�C���ԍ�
	//memcpy(pTypZi->Hd.Coilno, &m_typBuf.data.KeyInf.cCoilNo, sizeof(pTypZi->Hd.Coilno));
	if ('-' == m_typBuf.data.KeyInf.cCoilNo[1]) {
		// �R�C��No��'-'�͍폜����FF���M
		pTypZi->Hd.Coilno[0] = m_typBuf.data.KeyInf.cCoilNo[0];
		memcpy(&pTypZi->Hd.Coilno[1], &m_typBuf.data.KeyInf.cCoilNo[2], sizeof(pTypZi->Hd.Coilno) - 2);
	}
	else {
		memcpy(pTypZi->Hd.Coilno, &m_typBuf.data.KeyInf.cCoilNo, sizeof(pTypZi->Hd.Coilno));
	}


 	//===========================
	// �R�C�����
	//===========================
	// �R�C������(�E�l, 0�p�f�B���O)

	// �R�C��������`�F�b�N
	nWk = m_typBuf.data.nLen;
	if ( (COIL_LEN_MAX * 10) < m_typBuf.data.nLen) {
		// �����R�C����MAX[10-1m] �ɂĐݒ�
		nWk = COIL_LEN_MAX * 10;
		LOG(em_ERR), "[%s] <%s> �R�C��������͈͊O �v���R����=%d[m]", my_sThreadName, DivNameManager::GetTorB(ou), nWk);
	}
	cWk.Format("%06d", nWk);
	memcpy(pTypZi->CoilLen, cWk, sizeof(pTypZi->CoilLen));


	// �r���������[�h(2���ڃX�y�[�X)
	if (0 != m_typBuf.data.Common.nGrandFlg)
	{
		// �s�M���t���O��NG�̏ꍇ��NG�Ƃ��đ��M����B
		nWk = Z_FF_KMODE_NG;
	}
	else
	{
		if (0 != nIsResult) {			// �R�C�����і��� =�r����ғ�+�r����~�������Ŗ������Ƃ͑��������̂ŁA�蓮�ɂ��Ă���
			nWk = Z_FF_KMODE_STOP;
		}
		else {
			// ��Ԏ擾
			nWk = GetStatus_Kensa();				// coil_result
			nWk = GetStatus_EdgeDetect(ou, nWk);	// EdgeCoilInf
		}
	}
	cWk.Format("%01d  ", nWk);
	memcpy(pTypZi->Mode, cWk, sizeof(pTypZi->Mode));
	LOG(em_INF), "[%s] <%s> �r���������[�h=%d", my_sThreadName, DivNameManager::GetTorB(ou), nWk);

	// �Ǘ�No
	if(0 == nIsResult) memcpy(pTypZi->Id, m_cKizukenNo, strlen(m_cKizukenNo));		// �Ǘ�No�̍Ō��NULL�́A�R�s�[�������Ȃ�����

	// ����ȊO�̃R�C����񕔂́A����ȍ~�̏����ŏ����m�肳���Ă���


 	//===========================
	// �r���
	//===========================
	if(0 == nIsResult) {			// �R�C�����т�����ꍇ�̂� (�����p�^�[�����s���ƂȂ邽�߁A���M���ׂ��r���т�����ł��Ȃ�)
		// �r�����擾����
		if( ! GetData_DefectInf(ou, pcode, pInf) ) {
			LOG(em_ERR), "[%s] <%s> �r���擾���s", my_sThreadName, DivNameManager::GetTorB(ou));
			pInf->nDefCnt = 0;		// �O�ׂ̈ɖ����������Ƃɂ��Ă���
		}
	}

	//// �J�b�g���͍ŗD��ׁ̈A��ԍŏ��Ɏ��ѕt�^���Ă���
	MakeCutData(ou, pInf, pTypZi);

	//// �r���G���A�Ƀf�[�^�Z�b�g
	MakeDefectData(ou, pInf, pTypZi);

	return true;
}
//==========================================
// �ΏۃR�C�����r�����擾����
//------------------------------------------
// int		ou		: �\�� (0:�\ 1:��)
// int      pcode   : �����p�^�[�� (0�I���W��)
// FF_INF*	pInf	: �ҏW���[�N
// �ߒl		bool	: ���A���
//==========================================
bool MakeFfManager::GetData_DefectInf(int ou, int pcode, FF_INF* pInf)
{
	int					rec;				// �r��񃌃R�[�h�ʒu(0�I���W��)
	int					gid;				// ����O���[�h
	int					tid;				// �����r��
	int					nPos;				// ����ʒu
	int					no;					// �rNo
	int					ws;					// WS�ʒu
	int					gFF;				// FF�`������(�O���[�h)
	char				tFF[3];				// FF�`������(�r��)
	int					len;				// �r����
	int					nRank;				// ���׃O���[�h
	int					nGPriority;			// �O���[�h�D��x
	int					nTPriority;			// �r��D��x
	int					nKensys;			// ���o�V�X�e�� 0:�����Ӄ� 1:�o���Ӄ� 2:����


	int					nSendCnt = 0;		// ���M�Ώۂ̌�

	int					ii;
	CString				cSql;
	CString				cWk;

	cSql = "SELECT d.����O���[�h, d.�����r��, d.����ʒu, ";				// �K�{
	cSql += "g.���׃O���[�h, g.�D��x, t.�D��x, d.�rNo, d.WS�ʒu, g.FF�`������, t.FF�`������ ";
	cSql += ", d.�r���� ";
	cSql += "FROM ";
	cSql += DB_COIL_RESULT;
	cSql += " r, ";
	cSql += (0 == ou) ? DB_DEFECT_INF_T : DB_DEFECT_INF_B;
	cSql += " d, ";
	cSql += DB_TYPE_NAME;
	cSql += " t, ";
	cSql += DB_GRADE_NAME;
	cSql += " g ";
	cSql += "WHERE ";
	cSql += "r.�Ǘ�No = d.�Ǘ�No AND d.�����r��=t.EdasysID AND d.����O���[�h=g.EdasysID AND d.�d������=0 AND ";
	cSql += "r.�Ǘ�No = '";
	cSql += m_cKizukenNo;
	cSql += "' ";
	cSql += "ORDER BY g.���׃O���[�h DESC, g.�D��x ASC, t.�D��x ASC, d.����ʒu ASC";

	//LOG(em_NON), "%s", cSql);
	if( ! mcls_pDB->ExecuteDirect(cSql)) {
		LOG(em_ERR), "[%s] �r���擾�G���[ [%s]", my_sThreadName, m_cKizukenNo);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", m_cKizukenNo);
		return false;
	}
	while(true) {
		int sqlrc = mcls_pDB->Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��
				
			mcls_pDB->GetData(1,  &gid);				// DB_DEFECT_INF_T/B : ����O���[�h
			mcls_pDB->GetData(2,  &tid);				// DB_DEFECT_INF_T/B : �����r��
			mcls_pDB->GetData(3,  &nPos);				// DB_DEFECT_INF_T/B : ����ʒu
			mcls_pDB->GetData(4,  &nRank);				// DB_GRADE_NAME     : ���׃O���[�h
			mcls_pDB->GetData(5,  &nGPriority);			// DB_GRADE_NAME	 : �O���[�h�D��x
			mcls_pDB->GetData(6,  &nTPriority);			// DB_TYPE_NAME		 : �r��D��x
			mcls_pDB->GetData(7,  &no);					// DB_DEFECT_INF_T/B : �rNo
			mcls_pDB->GetData(8,  &ws);					// DB_DEFECT_INF_T/B : WS�ʒu
			mcls_pDB->GetData(9,  &gFF);				// DB_GRADE_NAME     : FF�`������
			mcls_pDB->GetData(10, &tFF, sizeof(tFF));	// DB_TYPE_NAME      : FF�`������
			mcls_pDB->GetData(11, &len);				// DB_DEFECT_INF_T/B : �r����

			// FF���M�Ώ�?
			if (!Check_Output(pcode, tid, gid)) continue;

			// ���i�͈͓̔��H
			nPos = (int)(nPos * m_dHosei);					// �␳���𔽉f������
			for (ii = 0; ii<m_typBuf.data.nSetCnt; ii++) {
				if (m_typBuf.data.saisun[ii].nStart * 100 <= nPos && m_typBuf.data.saisun[ii].nEnd * 100 >= nPos) break;		// [m] �� [0.1m]�Ή�
			}
			if (ii == m_typBuf.data.nSetCnt) {
				//LOG(em_MSG), "�ΏۊO���r %dmm", nPos);			
				continue;	// �͈͊O(=�J�b�g���ꂽ����)	
			}

			if (MAX_FF_DFCT_CNT <= pInf->nDefCnt) {
				pInf->bOverStatus = true;		// �r��񂾂���FF���M��MAX�ɓ��B�B��͍폜���Ă�������
				break;
			}

			// �ҏW���[�N�ɃZ�b�g
			rec = pInf->nDefCnt;

			pInf->typDef[rec].nGid       = gid;
			pInf->typDef[rec].nTid       = tid;
			pInf->typDef[rec].nYpos      = nPos;
			pInf->typDef[rec].nRank		 = nRank;
			pInf->typDef[rec].nGPriority = nGPriority;
			pInf->typDef[rec].nTPriority = nTPriority;
			pInf->typDef[rec].nNo        = no;
			pInf->typDef[rec].nWS        = ws;
			pInf->typDef[rec].nGFF       = gFF;
			memcpy(&pInf->typDef[rec].tFF, tFF, sizeof(pInf->typDef[rec].tFF));
			pInf->typDef[rec].nLen       = len;

			pInf->nDefCnt ++;
			nSendCnt ++;


		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
			break;	
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", m_cKizukenNo);
			return false;
		}
	}

	LOG(em_MSG), "[%s] <%s> FF���M�Ώ� ���M�Ώې�(%d) pcode=%d", my_sThreadName, DivNameManager::GetTorB(ou), nSendCnt, pcode + 1);
	return true;
}
//------------------------------------------
// �ΏۃR�C���̌������(�r���̉ғ���)���擾����
// �߂�l : �ΏۃR�C���̌������
//------------------------------------------
int MakeFfManager::GetStatus_Kensa()
{
	CString				cSql;
	int					nKensa[2];		// �������
	int					nEndMode;		// ���ъi�[�敪

	cSql.Format("SELECT �������_�\, �������_��, ���ъi�[�敪 FROM %s WHERE �Ǘ�No='%s'", DB_COIL_RESULT, m_cKizukenNo); 
	// ���s	
	if( ! mcls_pDB->ExecuteDirect(cSql)) {
		LOG(em_ERR), "[%s] ������Ԏ擾���s [%s]", my_sThreadName, m_cKizukenNo);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s: �L�[���=%.16s]", DB_COIL_RESULT, m_cKizukenNo);
		return Z_FF_KMODE_NG;
	}
	// �擾
	int sqlrc = mcls_pDB->Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

		mcls_pDB->GetData(1, &nKensa[0]);				// �������_�\
		mcls_pDB->GetData(2, &nKensa[1]);				// �������_��
		mcls_pDB->GetData(3, &nEndMode);				// ���ъi�[�敪

	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
		LOG(em_ERR), "[%s] ������Ԏ擾���s", my_sThreadName);
		return Z_FF_KMODE_NG;				// �L�蓾�Ȃ�
	} else {								// �G���[
		LOG(em_ERR), "[%s] ������Ԏ擾���s", my_sThreadName);
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", m_cKizukenNo);
		return Z_FF_KMODE_NG;				// �L�蓾�Ȃ�
	}


	//// �������[�h����
	int		nAns = 0;
	if( DIV_KENSA_OK == nKensa[0] && DIV_KENSA_OK == nKensa[1]) {
		nAns = Z_FF_KMODE_OK;						// ���ʂƂ����� (= ���ʐ���)

	} else if( DIV_KENSA_OK == nKensa[0] ) {	
		nAns = Z_FF_KMODE_OK_O;						// �\�ʂ̂ݐ��� (= ���ʏk��)

	} else if( DIV_KENSA_OK == nKensa[1] ) {
		nAns = Z_FF_KMODE_OK_U;						// ���ʂ̂ݐ��� (= �\�ʏk��)

	} else {
		nAns = Z_FF_KMODE_NG;						// ���ʂƂ����킶��Ȃ� (= ���ʏk��)
	}

	// ��~�͍ŗD��
	if( DIV_END_STOP_MANUAL == nEndMode || DIV_END_STOP_AUTO == nEndMode ) {			// ������~ or �蓮�r����~
		nAns = Z_FF_KMODE_STOP;
	}
	return nAns;
}
//------------------------------------------
// �ΏۃR�C���̃G�b�W���o�󋵂��擾����
// int ou 0:�\
// int nState ��̏��
// �߂�l : �ΏۃR�C���̌������
//------------------------------------------
int MakeFfManager::GetStatus_EdgeDetect(int ou, int nState)
{
	//// ����̎��ɂ����u�G�b�W�����o��ԗL��v�̃`�F�b�N�����Ȃ�
	if(Z_FF_KMODE_OK != nState) return nState;
	if( 0 == m_Param_CommonMast.nEdgeNgCount ) return nState;

	//// ���J�o���[���[�h�ɂȂ����������擾
	CString				cSql;
	int					nNgCount;		// ���J�o���[���[�h�ɂȂ�������
	int					nLostCnt[2];	// �G�b�W���o���s�� (��,�E)
	cSql.Format("SELECT �����e�͈͊O��, �G�b�W���o���s��_��, �G�b�W���o���s��_�E FROM %s WHERE �Ǘ�No='%s' AND �\���敪=%d", DB_EDGE_COIL_INF, m_cKizukenNo, ou); 

	if( ! mcls_pDB->ExecuteDirect(cSql)) {
		LOG(em_ERR), "[%s] �G�b�W���o��Ԏ擾���s(%s) [%s]", my_sThreadName, DivNameManager::GetTorB(ou), m_cKizukenNo);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", m_cKizukenNo);
		return nState;
	}
	// �擾
	int sqlrc = mcls_pDB->Fetch();
	if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

		mcls_pDB->GetData(1, &nNgCount);				// �����e�͈͊O��
		mcls_pDB->GetData(2, &nLostCnt[0]);				// �G�b�W���o���s��_��
		mcls_pDB->GetData(3, &nLostCnt[1]);				// �G�b�W���o���s��_�E

	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����
		LOG(em_ERR), "[%s] �G�b�W���o��Ԏ擾����(%s) [%s]", my_sThreadName, DivNameManager::GetTorB(ou), m_cKizukenNo);
		return nState;			// �L�蓾�Ȃ�
	} else {								// �G���[
		LOG(em_ERR), "[%s] �G�b�W���o��Ԏ擾���s(%s) [%s]", my_sThreadName, DivNameManager::GetTorB(ou), m_cKizukenNo);
		syslog(SYSNO_DB_FETCH_ERR, "[%s]", m_cKizukenNo);
		return nState;			// �L�蓾�Ȃ�
	}

	//// ��������
	if(m_Param_CommonMast.nEdgeNgCount <= nNgCount ||
	   m_Param_CommonMast.nEdgeNgCount <= nLostCnt[0] ||
	   m_Param_CommonMast.nEdgeNgCount <= nLostCnt[1]) {
		nState = Z_FF_KMODE_EDGENG;
	}
	return nState;
}
//------------------------------------------
// �o�͑Ώۂ̌��ׂ�����
// long pcode	�����p�^�[�� (0�I���W��)
// long tid		�r��EdasysID
// long gid		�O���[�hEdasysID
// �߂�l true:�Ώ�
//------------------------------------------
bool MakeFfManager::Check_Output(long pcode, long tid, long gid) const
{
	// �����`�F�b�N
	if (0 > pcode || MAX_PCODE <= pcode) {
		LOG(em_ERR), "[%s] pcodeNG=%d", my_sThreadName, pcode+1);
		return false;
	}

	//// �r��EdasysID �w��L��H
	for (int ii = 0; ii<m_Param_Pcode.rec[pcode].nPutNum; ii++) {
		if (tid == m_Param_Pcode.rec[pcode].typOutput[ii].nEdasysID) {
			// Gr�ݒ薳�����r��̈� �S���ΏۊO
			if (0 == m_Param_Pcode.rec[pcode].typOutput[ii].nKind) return false;

			// Gr�ݒ�L����r��̈� �D��x��r
			if (GetYusenG(gid) <= GetYusenG(m_Param_Pcode.rec[pcode].typOutput[ii].nKind))	return true;
			else																			return false;
		}
	}

	//// �����Ƃ��� �f�t�H���g�w�� (�R�R�܂ŗ����ꍇ�́A�r��ݒ薳���̎�)
	// Gr�ݒ薳�����r��̈� �S���ΏۊO
	if (0 == m_Param_Pcode.rec[pcode].typDefput.nKind) return false;

	// Gr�ݒ�L����r��̈� �D��x��r
	if (GetYusenG(gid) <= GetYusenG(m_Param_Pcode.rec[pcode].typDefput.nKind))	return true;
	else																	return false;
}
//------------------------------------------
// �O���[�h�D��x�擾
// long gid �O���[�hEdasysID
// �߂�l �D��x (�����o-1)
//------------------------------------------
int MakeFfManager::GetYusenG(long gid) const
{
	for(int ii=0; ii<m_Param_Grade.nNum; ii++) {
		if( m_Param_Grade.typGrade_id[ii].nEdasysID == gid ) return m_Param_Grade.typGrade_id[ii].nPriority;
	}
	return -1;
}
//------------------------------------------
// �J�b�g���� FF���тɃZ�b�g����
// int ou 0:�\
// FF_INF * pInf �ҏW���[�N
// FF_JISSEKI* pTypZi FF���ѓ`��
//------------------------------------------
void MakeFfManager::MakeCutData(int ou, FF_INF * pInf, FF_JISSEKI* pTypZi)
{
	int nCutCount = 0;				// �J�b�g��
	int cnt;
	CString cWk;


	//// �n�ڊ������܂ꂪ����ꍇ�́A�J�b�g�����쐬
	// �n�ڊ������܂ꎞ�́A�v���R�����́A�J�b�g���Ƃ��č���Ă���ĂȂ��ׂ̏��u
	if (0 < m_typBuf.data.nMakiLenF) {
		LOG(em_INF), "[%s] <%s> �n�ڊ������܂�L�� %.1fm", my_sThreadName, DivNameManager::GetTorB(ou), (m_typBuf.data.nMakiLenF/10.0));

		// �n�ڊ������܂���Z�b�g
		cnt = pInf->nFfCnt;
		memcpy(pTypZi->Dt[cnt].Kizukbn, Z_FF_DFCT_MAKI, sizeof(pTypZi->Dt[cnt].Kizukbn));	// �r���o�敪  (�n�ڊ������܂�)
		memset(pTypZi->Dt[cnt].Kind, ' ', sizeof(pTypZi->Dt[cnt].Kind));					// �r��(2���X�y�[�X)
		memset(pTypZi->Dt[cnt].Grade, '0', sizeof(pTypZi->Dt[cnt].Grade));					// �r�O���[�h(2��0�ݒ�)
		memset(pTypZi->Dt[cnt].WidPos, '0', sizeof(pTypZi->Dt[cnt].WidPos));				// �r�Е����ʒu(4��0�ݒ�)
		memset(pTypZi->Dt[cnt].Len, '0', sizeof(pTypZi->Dt[cnt].Len));						// �r����(4��0�ݒ�)
		cWk.Format("%06d", m_typBuf.data.nMakiLenF);										// �n�ڊ������܂꒷�����Z�b�g [m] �� [0.1m]�Ή�
		memcpy(pTypZi->Dt[cnt].DefectPos, cWk, sizeof(pTypZi->Dt[cnt].DefectPos));			// �r�����������ʒu(�E�l, 0�p�f�B���O)
		memset(pTypZi->Dt[cnt].kbn, ' ', sizeof(pTypZi->Dt[cnt].kbn));						// �r�敪(�X�y�[�X)
		memset(pTypZi->Dt[cnt].No, '0', sizeof(pTypZi->Dt[cnt].No));						// �rNo (0)
		memset(pTypZi->Dt[cnt].OpeDsp, ' ', sizeof(pTypZi->Dt[cnt].OpeDsp));				// �I�y�K�C(�X�y�[�X)
		memset(pTypZi->Dt[cnt].SysKbn, ' ', sizeof(pTypZi->Dt[cnt].SysKbn));				// �V�X�e���敪(�X�y�[�X)

		pInf->nFfCnt++;


		// �J�b�g�����Z�b�g
		cnt = pInf->nFfCnt;
		memcpy(pTypZi->Dt[cnt].Kizukbn, Z_FF_DFCT_CUT, sizeof(pTypZi->Dt[cnt].Kizukbn));	// �r���o�敪  (�J�b�g)
		memset(pTypZi->Dt[cnt].Kind, ' ', sizeof(pTypZi->Dt[cnt].Kind));					// �r��(2���X�y�[�X)
		memset(pTypZi->Dt[cnt].Grade, '0', sizeof(pTypZi->Dt[cnt].Grade));					// �r�O���[�h(2��0�ݒ�)
		memset(pTypZi->Dt[cnt].WidPos, '0', sizeof(pTypZi->Dt[cnt].WidPos));				// �r�Е����ʒu(4��0�ݒ�)
		memset(pTypZi->Dt[cnt].Len, '0', sizeof(pTypZi->Dt[cnt].Len));						// �r����(4��0�ݒ�)
		cWk.Format("%06d", m_typBuf.data.nMakiLenF);										// �n�ڊ������܂꒷�����Z�b�g [m] �� [0.1m]�Ή�
		memcpy(pTypZi->Dt[cnt].DefectPos, cWk, sizeof(pTypZi->Dt[cnt].DefectPos));			// �r�����������ʒu(�E�l, 0�p�f�B���O)
		memset(pTypZi->Dt[cnt].kbn, ' ', sizeof(pTypZi->Dt[cnt].kbn));						// �r�敪(�X�y�[�X)
		memset(pTypZi->Dt[cnt].No, '0', sizeof(pTypZi->Dt[cnt].No));						// �rNo (0)
		memset(pTypZi->Dt[cnt].OpeDsp, ' ', sizeof(pTypZi->Dt[cnt].OpeDsp));				// �I�y�K�C(�X�y�[�X)
		memset(pTypZi->Dt[cnt].SysKbn, ' ', sizeof(pTypZi->Dt[cnt].SysKbn));				// �V�X�e���敪(�X�y�[�X)

		pInf->nFfCnt++;
		nCutCount++;
	}

	//// �ʏ�̃J�b�g���𐶐�
	for (int ii = 0; ii<m_typBuf.data.nSetCnt; ii++) {
		// End(=�t�@�[�X�g�J�b�g)�ʒu�ƃR�C�����������ꍇ�́A�J�b�g�ł͖����A�܂����R�C���ւƑ����Ă���
		if (m_typBuf.data.saisun[ii].nEnd == m_typBuf.data.nLen) continue;


		// �J�b�g�����Z�b�g
		cnt = pInf->nFfCnt;
		memcpy(pTypZi->Dt[cnt].Kizukbn, Z_FF_DFCT_CUT, sizeof(pTypZi->Dt[cnt].Kizukbn));	// �r���o�敪  (�J�b�g)
		memset(pTypZi->Dt[cnt].Kind, ' ', sizeof(pTypZi->Dt[cnt].Kind));					// �r��(2���X�y�[�X)
		memset(pTypZi->Dt[cnt].Grade, '0', sizeof(pTypZi->Dt[cnt].Grade));					// �r�O���[�h(2��0�ݒ�)
		memset(pTypZi->Dt[cnt].WidPos, '0', sizeof(pTypZi->Dt[cnt].WidPos));				// �r�Е����ʒu(4��0�ݒ�)
		memset(pTypZi->Dt[cnt].Len, '0', sizeof(pTypZi->Dt[cnt].Len));						// �r����(4��0�ݒ�)
		cWk.Format("%06d", m_typBuf.data.saisun[ii].nEnd);									// �J�b�g�ʒu F�J�b�g�ʒu���Z�b�g [m] �� [0.1m]�Ή�
		memcpy(pTypZi->Dt[cnt].DefectPos, cWk, sizeof(pTypZi->Dt[cnt].DefectPos));			// �r�����������ʒu(�E�l, 0�p�f�B���O)
		memset(pTypZi->Dt[cnt].kbn, ' ', sizeof(pTypZi->Dt[cnt].kbn));						// �r�敪(�X�y�[�X)
		memset(pTypZi->Dt[cnt].No, '0', sizeof(pTypZi->Dt[cnt].No));						// �rNo (0)
		memset(pTypZi->Dt[cnt].OpeDsp, ' ', sizeof(pTypZi->Dt[cnt].OpeDsp));				// �I�y�K�C(�X�y�[�X)
		memset(pTypZi->Dt[cnt].SysKbn, ' ', sizeof(pTypZi->Dt[cnt].SysKbn));				// �V�X�e���敪(�X�y�[�X)

		pInf->nFfCnt++;
		nCutCount++;
	}

}
//------------------------------------------
// �r���G���A�ɃZ�b�g����
// int ou 0:�\
// FF_INF * pInf �ҏW���[�N
// FF_JISSEKI* pTypZi FF���ѓ`��
//------------------------------------------
void MakeFfManager::MakeDefectData(int ou, FF_INF * pInf, FF_JISSEKI* pTypZi)
{
	int ii = 0;
	int cnt;				// FF���Z�b�g�ʒu

	// �����ڂ܂ŃZ�b�g������
	int nDefNow = 0;		// �r

	int grec = 0;			// �O���[�h���̃��R�[�h
	CString cWk;
	char charWk[5];
	int nWk;


	// SELECT�Ŏ擾���A�D��x���ɕ��ёւ������Ă��邽�߁A���̃`�F�b�N�͕s�v�Ȃ̂ł́H�H(2018/08/30 5CGL�ɔ��΃Ӄ�)

	////// �O���[�h�D��x��(1�`)�Ƀf�[�^���Z�b�g���Ă�
	//for (grec = 0; grec<m_Param_Grade.nNum; grec++) {
		//// �r �`�F�b�N
		for (ii = nDefNow; ii<pInf->nDefCnt; ii++) {
		//	// �Z�b�g����Ώۂ̗D��x�H
		//	if (m_Param_Grade.typGrade_id[grec].nPriority < pInf->typDef[ii].nGPriority) break;
			if (MAX_FF_DFCT_CNT <= pInf->nFfCnt) {
				pInf->bOverStatus = true;
				break;
			}

			// �Z�b�g
			cnt = pInf->nFfCnt;

			memcpy(pTypZi->Dt[cnt].Kizukbn, Z_FF_DFCT_KENSA, sizeof(pTypZi->Dt[cnt].Kizukbn));	// �r���o�敪  (�r)

			cWk.Format("%s  ", pInf->typDef[ii].tFF );
			memcpy(pTypZi->Dt[cnt].Kind, cWk, sizeof(pTypZi->Dt[cnt].Kind));					// �r��(2���X�y�[�X)

			cWk.Format("%02d", pInf->typDef[ii].nGFF);
			memcpy(pTypZi->Dt[cnt].Grade, cWk, sizeof(pTypZi->Dt[cnt].Grade));					// �r�O���[�h(�E�l, 0�p�f�B���O)

			if(0 > pInf->typDef[ii].nWS) pInf->typDef[ii].nWS = 0;
			cWk.Format("%04d", pInf->typDef[ii].nWS);
			memcpy(pTypZi->Dt[cnt].WidPos, cWk, sizeof(pTypZi->Dt[cnt].WidPos));				// �r�Е����ʒu[mm](�E�l, 0�p�f�B���O)

			nWk = (pInf->typDef[ii].nLen / 100);
			if(1 > nWk) nWk = 1;
			cWk.Format("%04d", nWk);
			memcpy(pTypZi->Dt[cnt].Len, cWk, sizeof(pTypZi->Dt[cnt].Len));						// �r����[mm]��[0.1m](�E�l, 0�p�f�B���O)

			nWk = (int)((pInf->typDef[ii].nYpos / 100.0) + 0.5);								// [mm]��[0.1m] �l�̌ܓ�
			cWk.Format("%06d", nWk);
			memcpy(pTypZi->Dt[cnt].DefectPos, cWk, sizeof(pTypZi->Dt[cnt].DefectPos));			// �r�����������ʒu(�E�l, 0�p�f�B���O)

			memset(pTypZi->Dt[cnt].kbn, ' ', sizeof(pTypZi->Dt[cnt].kbn));						// �r�敪

			// 20200520 �r�ۑ�������g�� --->>>
			//cWk.Format("%04X", pInf->typDef[ii].nNo);
			//memcpy(pTypZi->Dt[cnt].No, cWk, sizeof(pTypZi->Dt[cnt].No));						// �rNo (Hex�̕����\��)(�E�l, 0�p�f�B���O)

			// Base32HEX�ŃZ�b�g����
			_itoa_s(pInf->typDef[ii].nNo, charWk, 32);
			cWk.Format("%04s", charWk);
			cWk.MakeUpper();
			memcpy(pTypZi->Dt[cnt].No, cWk, sizeof(pTypZi->Dt[cnt].No));						// �rNo (Hex�̕����\��)(�E�l, 0�p�f�B���O)
			// 20200520 �r�ۑ�������g�� ---<<<

			memset(pTypZi->Dt[cnt].OpeDsp, ' ', sizeof(pTypZi->Dt[cnt].OpeDsp));				// �I�y�K�C(�X�y�[�X)
			memset(pTypZi->Dt[cnt].SysKbn, ' ', sizeof(pTypZi->Dt[cnt].SysKbn));				// �V�X�e���敪(�X�y�[�X)

			// �㏈��
			pInf->typDef[ii].bFFSet = true;		// FF�Ɏ��ۂɃZ�b�g�����ꍇTrue
			pInf->nFfCnt ++;
			nDefNow ++;
		}
	//}

	//// �r���I�[�o�[�X�e�[�^�X(2���ڃX�y�[�X)
	if( pInf->bOverStatus ){
		cWk.Format("%d  ", Z_FF_KOVER_NG);
		memcpy(pTypZi->OverStatus, cWk, sizeof(pTypZi->OverStatus));
		LOG(em_MSG), "[%s] <%s> ������B %d��(Sts:%d)", my_sThreadName, DivNameManager::GetTorB(ou), pInf->nFfCnt, pInf->bOverStatus);
	}else{
		cWk.Format("%d  ", Z_FF_KOVER_OK);
		memcpy(pTypZi->OverStatus, cWk, sizeof(pTypZi->OverStatus));
	}

	//// �f�[�^�ݒ萔
	cWk.Format("%04d", pInf->nFfCnt);
	memcpy(pTypZi->DataDefault, cWk, sizeof(pTypZi->DataDefault));


	//// �G���h����
	LOG(em_MSG), "[%s] <%s> �g�[�^��=%d��",	my_sThreadName, DivNameManager::GetTorB(ou), pInf->nFfCnt );
}
//------------------------------------------
// FF���M�p ������ԓ`������
// int ou 0:�\�@1:��
// int pcode �����p�^�[�� (0�I���W��)
// int nIsResult �R�C�����їL�� (0:�L�� 1:����)
// FF_STS * pPos �ҏW���[�N
// FF_STS_JISSEKI* pTypSts �e�ʂ݂̂̎��ѓ`��
// �߂�l : ���A���
//------------------------------------------
bool MakeFfManager::MakeSendStsData(int ou, int pcode, int nIsResult, FF_STS* pPos, FF_STS_JISSEKI* pTypSts)
{
	CString cWk;

	//// ������
	memset(pTypSts, 0x20, sizeof(FF_STS_JISSEKI));


	//===========================
	// �w�b�_�[��
	//===========================
	if (0 == ou) memcpy(pTypSts->Hd.Header_Code, FF_TC_CODE_STSJI_O, sizeof(pTypSts->Hd.Header_Code));
	else		memcpy(pTypSts->Hd.Header_Code, FF_TC_CODE_STSJI_U, sizeof(pTypSts->Hd.Header_Code));
	memcpy(pTypSts->Hd.Rec_Code, Z_FF_RENBAN, sizeof(pTypSts->Hd.Rec_Code));

	// ���M����
	SYSTEMTIME STime;
	GetLocalTime(&STime);
	cWk.Format("%04d%02d%02d%02d%02d%02d", STime.wYear, STime.wMonth, STime.wDay, STime.wHour, STime.wMinute, STime.wSecond);
	memcpy(pTypSts->Hd.Date, cWk, sizeof(pTypSts->Hd.Date));

	// �H���R�[�h ( 5CGL=85 )
	memcpy(pTypSts->Hd.Line_Code, KOUTEI_CODE, sizeof(pTypSts->Hd.Line_Code));

	// ���ߔԍ�
	memcpy(pTypSts->Hd.Meino, m_typBuf.data.KeyInf.cMeiNo, sizeof(pTypSts->Hd.Meino));

	// �≄�R�C���ԍ�
	//memcpy(pTypSts->Hd.Coilno, m_typBuf.data.KeyInf.cCoilNo, sizeof(pTypSts->Hd.Coilno));
	if ('-' == m_typBuf.data.KeyInf.cCoilNo[1]) {
		// �R�C��No��'-'�͍폜����FF���M
		pTypSts->Hd.Coilno[0] = m_typBuf.data.KeyInf.cCoilNo[0];
		memcpy(&pTypSts->Hd.Coilno[1], &m_typBuf.data.KeyInf.cCoilNo[2], sizeof(pTypSts->Hd.Coilno) - 2);
	}
	else {
		memcpy(pTypSts->Hd.Coilno, m_typBuf.data.KeyInf.cCoilNo, sizeof(pTypSts->Hd.Coilno));
	}


	//===========================
	// �R�C�����
	//===========================
	// �Ǘ�No
	if (0 == nIsResult) memcpy(pTypSts->Dt.Id, m_cKizukenNo, strlen(m_cKizukenNo));		// �Ǘ�No�̍Ō��NULL�́A�R�s�[�������Ȃ�����


	// ����ȊO�̃R�C����񕔂́A����ȍ~�̏����ŏ����m�肳���Ă���


	//===========================
	// ������ԕ�
	//===========================
	//// �����擾����
	if (0 == nIsResult) {			// �R�C�����т�����ꍇ�̂�
									// �ΏۃR�C���̃R�C���ʒu�����擾����
		if (!GetData_CoilPosition(ou, pPos)) {
			LOG(em_ERR), "[%s] <%s> �r���擾���s", my_sThreadName, DivNameManager::GetTorB(ou));
			pPos->nCnt = 0;		// �O�ׂ̈ɖ����������Ƃɂ��Ă���
		}

	} else {
		// �R�C�����т������Ƃ��́A������ԃZ�b�g��=0 ������ԃf�[�^=�I�[���X�y�[�X
		cWk.Format("%05d", 0);
		memcpy(pTypSts->Dt.dataSize, cWk, sizeof(pTypSts->Dt.dataSize));
		return true;
	}

	//// ������ԃG���A�Ƀf�[�^�Z�b�g
	MakeStatusData(ou, pPos, pTypSts);

	return true;
}
//------------------------------------------
// �ΏۃR�C���̃R�C���ʒu�����擾����
// int ou 0:�\
// FF_STS * pSts �ҏW���[�N
//------------------------------------------
bool MakeFfManager::GetData_CoilPosition(int ou, FF_STS* pPos)
{
	int					rec;
	CString				cSql;
	short				nKensa;			// �������
	CString				cWk;

	cSql = "SELECT ID, ";				// �K�{
	cSql += (0 == ou) ? "�������_�\" : "�������_��";
	cSql += " FROM ";
	cSql += DB_COIL_POSITION;
	cWk.Format(" WHERE �Ǘ�No = '%s'", m_cKizukenNo); cSql += cWk;
	cSql += "ORDER BY ID ASC";

	if (!mcls_pDB->ExecuteDirect(cSql)) {
		LOG(em_ERR), "[%s] �R�C���ʒu���擾���s [%s]", my_sThreadName, m_cKizukenNo);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", m_cKizukenNo);
		return false;
	}
	while (true) {
		int sqlrc = mcls_pDB->Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

			rec = pPos->nCnt;

			mcls_pDB->GetData(1, &pPos->sts[rec].nID);

			mcls_pDB->GetData(2, &nKensa);

			pPos->sts[rec].emKensa = (EM_DIV_KENSA)nKensa;

			pPos->nCnt++;
			if (MAP_ROW_NUM <= pPos->nCnt) break;	// ��͍폜���Ă�������

		} else if (sqlrc == KIZUODBC_FETCH_NODATE) {		// �f�[�^����
			break;
		} else {								// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", m_cKizukenNo);
			return false;
		}
	}

	LOG(em_MSG), "[%s] �R�C���ʒu����=%d", my_sThreadName, pPos->nCnt );

	return true;
}
//------------------------------------------
// ������ԃG���A�ɃZ�b�g����
// int ou 0:�\
// FF_STS * pPos �ҏW���[�N
// FF_STS_JISSEKI* pTypSts FF������ԓ`��
//------------------------------------------
void MakeFfManager::MakeStatusData(int ou, FF_STS * pPos, FF_STS_JISSEKI* pTypSts)
{
	int	nStart;			// �f�[�^�Z�b�g�J�n�s(0�I���W��)
	int	nEnd;			// �f�[�^�Z�b�g�I���s(0�I���W��)
	int nMax;			// �f�[�^�Z�b�g�ŏI�s(0�I���W��) ���R�C����[m]
	int nOkRow;			// �����������p�O��l�p�o�^����A�h���X���[�N
	int ii;				// ���[�v�J�E���^
	int jj;				// ���[�v�J�E���^
	int kk;				// ���[�v�J�E���^
	int nKensa;			// ������ԃZ�b�g�p���[�N
	CString sWk;		// ���[�N
	double dWk;			// ���[�N
	char cWk;			// ���[�N
	int nWk;			// ���[�N


						// ������
	nStart = 0;
	nEnd = 0;
	nKensa = 0;
	// �f�[�^�Z�b�g�ŏI�s�Z�o�i�O�I���W���j
	nMax = int(m_nLen * m_dHosei / 1000.0 + 0.999) - 1;

	// �o�^�f�[�^�����[�v
	for (ii = 0; ii<pPos->nCnt; ii++) {
		// ������Ԃ��Z�b�g����B
		nKensa = pPos->sts[ii].emKensa;
		// ������Ԃ��O�����̏ꍇ�́A9(EM_DIV_KENSA.DIV_KENSA_STOP)�ɒu��������B
		if (0 > nKensa) nKensa = 9;

		// ID�ɑ΂���Z�b�g��Ԃ��Z�o����B
		// �f�[�^�Z�b�g�J�n�s(0�I���W��)�Z�o
		dWk = (pPos->sts[ii].nID) * MAP_REPRES_MIN * m_dHosei;
		nStart = int(dWk);
		// �f�[�^�Z�b�g�I���s(0�I���W��)�Z�o
		dWk = (pPos->sts[ii].nID + 1) * MAP_REPRES_MIN * m_dHosei + 0.999;
		nEnd = int(dWk) - 1;

		//�����`�F�b�N
		if (0 > nStart) nStart = 0;
		//�f�[�^�Z�b�g��ԊJ�n�ʒu���f�[�^�Z�b�g�ŏI�s�𒴂����ꍇ�͏I���Ƃ���B
		if (nMax < nStart) break;
		//�����`�F�b�N
		if (0 > nEnd) nEnd = 0;
		//�f�[�^�Z�b�g��ԏI���ʒu���f�[�^�Z�b�g�ŏI�s�𒴂����ꍇ�̓f�[�^�Z�b�g�ŏI�s�܂łƂ���B
		if (nMax < nEnd) nEnd = nMax;

		// �f�[�^�Z�b�g
		for (jj = nStart; jj <= nEnd; jj++) {
			if (0 == memcmp(" ", pTypSts->Dt.data[jj].status, sizeof(pTypSts->Dt.data[jj].status))) {
				// ���o�^�̏ꍇ�͖������Z�b�g(1��)
				sWk.Format("%01d", nKensa);
				memcpy(pTypSts->Dt.data[jj].status, sWk, sizeof(pTypSts->Dt.data[jj].status));
			} else {
				// ���ɓo�^�ς݂̏ꍇ�͗D��x��r���ăZ�b�g(�����傫�������A�D��x�F��)
				memcpy(&cWk,&pTypSts->Dt.data[jj],sizeof(pTypSts->Dt.data[jj]));
				nWk = atoi(&cWk);
				if (nWk < nKensa){
					sWk.Format("%01d", nKensa);
					memcpy(pTypSts->Dt.data[jj].status, sWk, sizeof(pTypSts->Dt.data[jj].status));
				}
			}
		}
	}
	// �������f�[�^�Z�b�g
	// �@�擪���玕�����̏ꍇ�A�擪����̎������ӏ����������=9�Ŗ��߂�B
	if (0 == memcmp(" ", pTypSts->Dt.data[0].status, sizeof(pTypSts->Dt.data[0].status))) {
		for (ii = 0; ii <= nMax; ii++){
			if (0 == memcmp(" ", pTypSts->Dt.data[ii].status, sizeof(pTypSts->Dt.data[ii].status))) {
				//���o�^�Ɍ������=9���Z�b�g
				sWk.Format("%01d", 9);
				memcpy(pTypSts->Dt.data[ii].status, sWk, sizeof(pTypSts->Dt.data[ii].status));
			} else {
				// �o�^�ςׁ݂̈A�����𔲂���B
				break;
			}
		}
	}

	// �A�r��(�ŏI���O�֎��������܂�)�̎������͑O��l���Z�b�g
	nOkRow = 0;
	for (ii = 0; ii <= nMax; ii++){
		if (0 == memcmp(" ", pTypSts->Dt.data[ii].status, sizeof(pTypSts->Dt.data[ii].status))) {
			memcpy(pTypSts->Dt.data[ii].status, pTypSts->Dt.data[nOkRow].status, sizeof(pTypSts->Dt.data[ii].status));
		}
		// �O��l�p�o�^����A�h���X�ۑ�
		nOkRow = ii;
	}

	// ������ԃZ�b�g��
	sWk.Format("%05d", nMax + 1);
	memcpy(pTypSts->Dt.dataSize, sWk, sizeof(pTypSts->Dt.dataSize));

	//// �G���h����
	LOG(em_MSG), "[%s] <%s> ������� ����=%d", my_sThreadName, DivNameManager::GetTorB(ou), nMax + 1);

}
//------------------------------------------
// FF�p�������ѓ`�� �� �o�b�t�@�e�[�u���ɓo�^
// char const* cID �L�[
// FF_JISSEKI const* pTypZi ���ѓ`��
// FF_STS_JISSEKI const* pTypSts ������ԓ`��
//------------------------------------------
bool MakeFfManager::InsertZiBuf(char const* cID, FF_JISSEKI const* pTypZi ,FF_STS_JISSEKI const* pTypSts)
{
	CString		cSql;

	cSql.Format("INSERT INTO %s VALUES(", DB_BUF_FF_S_ZISEKI);
	
	CreateSQL::sqlColumnInsert(&cSql, cID, SIZE_SERIAL_FF_NO);	// ID
	CreateSQL::sqlColumnAddTime(&cSql, true);					// �o�^���t
	CreateSQL::sqlColumnInsert(&cSql, 1);						// �`���敪(1:�����M)
	CreateSQL::sqlColumnInsert(&cSql, (char*)&m_typBuf.data.KeyInf, sizeof(m_typBuf.data.KeyInf));	// �L�[���
	cSql += "?, ?, ?, ?,";										// ���`��
	CreateSQL::sqlColumnInsert(&cSql, m_cKizukenNo, SIZE_KIZUKEN_NO);				// �Ǘ�No
	CreateSQL::sqlColumnInsert(&cSql, NULL, 0);					// ���M���t
	CreateSQL::sqlColumnTrim(&cSql);
	cSql += ")";


#ifndef TEST_ONLINE_DATA
	DWORD wksize[4];
	LPVOID wkpVal[4];
	// �f�[�^�Z�b�g
	wksize[0] = sizeof(FF_JISSEKI);
	wksize[1] = sizeof(FF_JISSEKI);
	wksize[2] = sizeof(FF_STS_JISSEKI);
	wksize[3] = sizeof(FF_STS_JISSEKI);
	wkpVal[0] = (BYTE*)&pTypZi[0];
	wkpVal[1] = (BYTE*)&pTypZi[1];
	wkpVal[2] = (BYTE*)&pTypSts[0];
	wkpVal[3] = (BYTE*)&pTypSts[1];

	// �o�^
	if ( ! mcls_pDB->SqlBinalyWrite(cSql, 4, wksize, wkpVal,false) ) {
		LOG(em_NON), "%s", cSql);
		LOG(em_ERR), "[%s] �������ѓo�^���s [%s]", my_sThreadName, cID);
		return false;
	}
#else
	// �o�C�i���_���v
	KizuFunction::DumpData("debag FF�������� �\.txt", sizeof(FF_JISSEKI), (BYTE*)&pTypZi[0]); 
	KizuFunction::DumpData("debag FF�������� ��.txt", sizeof(FF_JISSEKI), (BYTE*)&pTypZi[1]);
	KizuFunction::DumpData("debag FF������� �\.txt", sizeof(FF_STS_JISSEKI), (BYTE*)&pTypSts[0]);
	KizuFunction::DumpData("debag FF������� ��.txt", sizeof(FF_STS_JISSEKI), (BYTE*)&pTypSts[1]);
	LOG(em_WAR), "[%s] �e�X�g���[�h�ɂ��AFF�������т��_���v�o��", my_sThreadName);

#endif

	return true;
}

//------------------------------------------
// �̐����уe�[�u�� �� �X�V
// char const* cID �L�[
// �߂�l : ����
//------------------------------------------
bool MakeFfManager::UpdateProconBuf(char const* cID)
{
	CString		cSql;

	cSql.Format("UPDATE %s SET ", DB_BUF_PRO_R_SAISUN);
	CreateSQL::sqlColumnUpdate(&cSql, "�Ǘ�No", m_cKizukenNo, SIZE_KIZUKEN_NO);
	cSql += "�ҏW���t=";
	CreateSQL::sqlColumnAddTime(&cSql);							// �ҏW���t
	cSql += " WHERE ID='";
	cSql += cID;
	cSql += "'";

	// �o�^
	if (!mcls_pDB->ExecuteDirect(cSql))
	{
		LOG(em_ERR), "[%s] �̐����� UPDATE�G���[ [ID=%s]", my_sThreadName, cID);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", DB_BUF_PRO_R_SAISUN);
		return false;
	}

	return true;
}

//------------------------------------------
// �R�C�����уe�[�u�� �� �X�V
// �߂�l : ����
//------------------------------------------
bool MakeFfManager::UpdateCoilResult()
{
	CString		cSql;
	CString		cWk;

	cSql.Format("UPDATE %s SET ", DB_COIL_RESULT);
	CreateSQL::sqlColumnUpdate(&cSql, "�␳��", m_dHosei);
	CreateSQL::sqlColumnUpdate(&cSql, "�v���R�����є��f", 0);	 
	CreateSQL::sqlColumnUpdate(&cSql, "�v���R��������", m_typBuf.data.nLen * 100);			// [10-1m] �� [mm] �֒P�ʕϊ�
	CreateSQL::sqlColumnUpdate(&cSql, "���i��", m_typBuf.data.nSetCnt);
	CreateSQL::sqlColumnTrim(&cSql);
	cSql += " WHERE �Ǘ�No='";
	cSql += m_cKizukenNo;
	cSql += "'";

	// �o�^
	if (!mcls_pDB->ExecuteDirect(cSql))
	{
		LOG(em_ERR), "[%s] �R�C������ UPDATE�G���[ [�Ǘ�No=%s]", my_sThreadName, m_cKizukenNo);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", DB_COIL_RESULT, m_cKizukenNo);
		return false;
	}

	return true;
}

//------------------------------------------
// �J�b�g���уe�[�u�� �ɑ}��
// const char *cID  �L�[
// �߂�l : ����
//------------------------------------------
bool MakeFfManager::InsertCutResult(const char* cID)
{
	CString		cSql;
	CString		cWk;
	int			nCutCount = 0;			// �J�b�g��
	int			nCutPos;				// �J�b�g�ʒu [mm]

	// �n�ڊ������܂ꂪ����ꍇ
	if (0 < m_typBuf.data.nMakiLenF)
	{
		// �J�b�g�ʒu ([10-1m] �� [mm] �֒P�ʕϊ�)
		nCutPos = m_typBuf.data.nMakiLenF * 100;

		cSql.Format("INSERT INTO %s VALUES(", DB_COIL_CUT_RESULT);
		CreateSQL::sqlColumnInsert(&cSql, m_cKizukenNo, sizeof(m_cKizukenNo));									// �Ǘ�No
		CreateSQL::sqlColumnInsert(&cSql, nCutCount + 1);														// �J�b�gNo
		CreateSQL::sqlColumnInsert(&cSql, cID, SIZE_SERIAL_FF_NO);												// ID
		CreateSQL::sqlColumnInsert(&cSql, m_typBuf.data.KeyInf.cMeiNo, sizeof(m_typBuf.data.KeyInf.cMeiNo));	// ����No
		CreateSQL::sqlColumnInsert(&cSql, m_typBuf.data.KeyInf.cCoilNo, sizeof(m_typBuf.data.KeyInf.cCoilNo));	// ���YNo
		CreateSQL::sqlColumnInsert(&cSql, nCutPos);																// �J�b�g�ʒu [mm]
		CreateSQL::sqlColumnTrim(&cSql);
		cSql += ")";

		// �o�^
		if (!mcls_pDB->ExecuteDirect(cSql))
		{
			LOG(em_ERR), "[%s] �J�b�g���уe�[�u�� INSERT�G���[ [�Ǘ�No=%s]", my_sThreadName, m_cKizukenNo);
			LOG(em_NON), "%s", cSql);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", DB_COIL_CUT_RESULT, m_cKizukenNo);
			return false;
		}
		nCutCount++;
	}

	// �̐������Z�b�g (F�J�b�g�ʒu(=End�̕�)���̗p)
	for (int ii = 0; ii < m_typBuf.data.nSetCnt; ii++)
	{
		// �J�b�g�ʒu ([10-1m] �� [mm] �֒P�ʕϊ�)
		nCutPos = m_typBuf.data.saisun[ii].nEnd * 100;

		cSql.Format("INSERT INTO %s VALUES(", DB_COIL_CUT_RESULT);
		CreateSQL::sqlColumnInsert(&cSql, m_cKizukenNo, sizeof(m_cKizukenNo));									// �Ǘ�No
		CreateSQL::sqlColumnInsert(&cSql, nCutCount + 1);														// �J�b�gNo
		CreateSQL::sqlColumnInsert(&cSql, cID, SIZE_SERIAL_FF_NO);												// ID
		CreateSQL::sqlColumnInsert(&cSql, m_typBuf.data.KeyInf.cMeiNo, sizeof(m_typBuf.data.KeyInf.cMeiNo));	// ����No
		CreateSQL::sqlColumnInsert(&cSql, m_typBuf.data.KeyInf.cCoilNo, sizeof(m_typBuf.data.KeyInf.cCoilNo));	// ���YNo
		CreateSQL::sqlColumnInsert(&cSql, nCutPos);																// �J�b�g�ʒu [mm]
		CreateSQL::sqlColumnTrim(&cSql);
		cSql += ")";

		// �o�^
		if (!mcls_pDB->ExecuteDirect(cSql))
		{
			LOG(em_ERR), "[%s] �J�b�g���уe�[�u�� INSERT�G���[ [�Ǘ�No=%s]", my_sThreadName, m_cKizukenNo);
			LOG(em_NON), "%s", cSql);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", DB_COIL_CUT_RESULT, m_cKizukenNo);
			return false;
		}
		nCutCount++;
	}

	return true;
}

//------------------------------------------
// �̐����уe�[�u�� �ɑ}��
// const char *cID  �L�[
// �߂�l : ����
//------------------------------------------
bool MakeFfManager::InsertSaisunResult(const char *cID)
{
	CString		cSql;
	CString		cWk;
	int			nDiv;					// �̐��敪 (0:�ʏ�J�b�g 1:�O�R�C���ƌ������Ă��� 2:��R�C���ƌ������Ă��� 3:����)
	int			nStartPos;				// �̐��J�n�ʒu [mm]
	int			nEndPos;				// �̐��I���ʒu [mm]

	// �̐������Z�b�g
	for (int ii = 0; ii < m_typBuf.data.nSetCnt; ii++)
	{
		// �̐��敪
		if (0 == m_typBuf.data.saisun[ii].nStart && m_typBuf.data.nLen == m_typBuf.data.saisun[ii].nEnd)
		{
			nDiv = DIV_PRO_CUT_UNION;			// �O��R�C���ƌ���
		}
		else if (0 == m_typBuf.data.saisun[ii].nStart)
		{
			nDiv = DIV_PRO_CUT_FRONT_UNION;		// �O�R�C���ƌ���
		}
		else if (m_typBuf.data.nLen == m_typBuf.data.saisun[ii].nEnd)
		{
			nDiv = DIV_PRO_CUT_TAIL_UNION;		// ��R�C���ƌ���
		}
		else
		{
			nDiv = DIV_PRO_CUT_NORMAL;			// �ʏ�J�b�g
		}

		// �̐��J�n�ʒu�A�̐��I���ʒu ([10-1mm] �� [mm] �֒P�ʕϊ�)
		nStartPos = m_typBuf.data.saisun[ii].nStart * 100;
		nEndPos = m_typBuf.data.saisun[ii].nEnd * 100;

		cSql.Format("INSERT INTO %s VALUES(", DB_COIL_SAISUN_RESULT);
		CreateSQL::sqlColumnInsert(&cSql, m_cKizukenNo, sizeof(m_cKizukenNo));									// �Ǘ�No
		CreateSQL::sqlColumnInsert(&cSql, ii + 1);																// �̐�No
		CreateSQL::sqlColumnInsert(&cSql, cID, SIZE_SERIAL_FF_NO);												// ID
		CreateSQL::sqlColumnInsert(&cSql, m_typBuf.data.KeyInf.cMeiNo, sizeof(m_typBuf.data.KeyInf.cMeiNo));	// ����No
		CreateSQL::sqlColumnInsert(&cSql, m_typBuf.data.KeyInf.cCoilNo, sizeof(m_typBuf.data.KeyInf.cCoilNo));	// ���YNo
		CreateSQL::sqlColumnInsert(&cSql, nDiv);																// �̐��敪
		CreateSQL::sqlColumnInsert(&cSql, nStartPos);															// �̐��J�n�ʒu [mm]
		CreateSQL::sqlColumnInsert(&cSql, nEndPos);																// �̐��I���ʒu [mm]
		CreateSQL::sqlColumnTrim(&cSql);
		cSql += ")";

		// �o�^
		if (!mcls_pDB->ExecuteDirect(cSql))
		{
			LOG(em_ERR), "[%s] �̐����уe�[�u�� INSERT�G���[ [�Ǘ�No=%s]", my_sThreadName, m_cKizukenNo);
			LOG(em_NON), "%s", cSql);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", DB_COIL_SAISUN_RESULT, m_cKizukenNo);
			return false;
		}
	}

	return true;
}



//---------------------------------------------------------
// ���H���r���e�[�u��    (ACCESS_NEXT_DEFECT_INF)�o�^
// ���H���R�C�����e�[�u��(ACCESS_NEXT_COIL_INF)�o�^
// FF_INF*     pInf   �ҏW���[�N
// FF_JISSEKI* pTypZi �e�ʂ݂̂̎��ѓ`��
// �߂�l : ���A���
//---------------------------------------------------------
bool MakeFfManager::InsertAccessNextInf( FF_INF pInf[], FF_JISSEKI pTypZi[])
{
	CString		cSql;
	int			nDefCnt[2];

	//// �r�����������āAFF�ɃZ�b�g�����r����DB�ɓo�^����
	for(int ou=0; ou<2; ou++ ) {
		nDefCnt[ou] = 0;

		for(int ii=0; ii < pInf[ou].nDefCnt; ii++ ) {
			if( ! pInf[ou].typDef[ii].bFFSet ) continue;

			cSql.Format("INSERT INTO %s VALUES(", DB_ACCESS_NEXT_DEFECT_INF );

			CreateSQL::sqlColumnInsert( &cSql, m_cKizukenNo, sizeof(m_cKizukenNo));	// �Ǘ�No
			CreateSQL::sqlColumnInsert( &cSql, pInf[ou].typDef[ii].nNo);			// �rNo
			CreateSQL::sqlColumnInsert( &cSql, ou);									// �\���敪
			CreateSQL::sqlColumnInsert( &cSql, 0);									// ���O�ҏW�t���O (�f�t�H���g��0�ʏ�)
			CreateSQL::sqlColumnInsert( &cSql, pInf[ou].typDef[ii].nYpos);			// �␳���l����̒���ʒu

			// 20191010 �q�b�k�@���z�������u�@�\�t�g���� �a�� ----->>>
			CreateSQL::sqlColumnInsert( &cSql, NULL, 0);							// �����r��
			CreateSQL::sqlColumnInsert( &cSql, NULL, 0);							// ����O���[�h
			// 20191010 �q�b�k�@���z�������u�@�\�t�g���� �a�� <<<-----

			CreateSQL::sqlColumnTrim(&cSql);
			cSql += ")";

			nDefCnt[ou] ++;

			// �o�^
#ifndef TEST_ONLINE_DATA
			if( ! mcls_pDB->ExecuteDirect(cSql,false) ) {
				LOG(em_ERR), "[%s] ���H���r��� INSERT�G���[ [%s]", my_sThreadName, m_cKizukenNo);
				LOG(em_NON), "%s", cSql);
				syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", DB_ACCESS_NEXT_DEFECT_INF, m_cKizukenNo);
				return false;
			}
#endif
		}
	}


	//// �R�C���P�ʂ̏����Z�b�g

	cSql.Format("INSERT INTO %s VALUES(", DB_ACCESS_NEXT_COIL_INF);
	
	CreateSQL::sqlColumnInsert( &cSql, m_cKizukenNo, SIZE_KIZUKEN_NO);	// �Ǘ�No
	CreateSQL::sqlColumnAddTime(&cSql, true);							// �o�^���t
	CreateSQL::sqlColumnInsert( &cSql, 0 );								// ���O�ҏW�ς݃t���O
	CreateSQL::sqlColumnInsert( &cSql, 0 );								// ���H���擾�ς݃t���O
	CreateSQL::sqlColumnInsert( &cSql, m_typBuf.data.nLen * 100);		// �R�C����(�v���R���̐�����)(mm) [0.1m]->[mm]�Ή�
	CreateSQL::sqlColumnInsert( &cSql, pInf[0].nFfCnt);					// �f�[�^�ݒ萔_�\
	CreateSQL::sqlColumnInsert( &cSql, pInf[1].nFfCnt);					// �f�[�^�ݒ萔_��
	CreateSQL::sqlColumnInsert( &cSql, nDefCnt[0]);						// �r���_�\
	CreateSQL::sqlColumnInsert( &cSql, nDefCnt[1]);						// �r���_��
	// 20200302 RCL ���O�ҏW���{�L�^�pDB�ǉ� �a�� --->>>
	//cSql += ",";															// 
	CreateSQL::sqlColumnInsert(&cSql, NULL, 0);							// ���O�ҏW_PC����
	CreateSQL::sqlColumnInsert(&cSql, NULL, 0);							// ���O�ҏW_���[�U�[��
	CreateSQL::sqlColumnInsert(&cSql, NULL, 0);							// ���O�ҏW_IP�A�h���X
	CreateSQL::sqlColumnInsert(&cSql, NULL, 0);							// ���O�ҏW_RCL�r���Ǘ�No
	CreateSQL::sqlColumnInsert(&cSql, NULL, 0);							// ���O�ҏW_RCL���C��ID
	CreateSQL::sqlColumnInsert(&cSql, NULL, 0);							// ���O�ҏW_�R�C��No
	CreateSQL::sqlColumnInsert(&cSql, NULL, 0);							// ���O�ҏW_�o�^���t
	// 20200302 RCL ���O�ҏW���{�L�^�pDB�ǉ� �a�� <<<---
	CreateSQL::sqlColumnTrim(&cSql);
	cSql += ")";

	// �o�^
#ifndef TEST_ONLINE_DATA
	if( ! mcls_pDB->ExecuteDirect( cSql , false) ) {
		LOG(em_ERR), "[%s] ���H���R�C�����e�[�u�� INSERT�G���[ [%s]", my_sThreadName, m_cKizukenNo);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", DB_ACCESS_NEXT_COIL_INF, m_cKizukenNo);
		return false;
	}
#endif

	return true;
}


//------------------------------------------
// ��FF�ʐM �ɑ��M�˗�
// char const* cKey ID
//------------------------------------------
void MakeFfManager::Send_To_FF(char const* cKey)
{
	COMMON_QUE que;					// ���[���\����
	memset(&que, 0x00, sizeof(que));

	que.nEventNo = FACT_TO_FFSERV_01;
	que.nLineNo = getlineid();
	// FF���M�v���ɂ͉��L�͕K�v�Ȃ��̂ŃR�����g�A�E�g 
	//strcpy(que.fc.data, cKey);
	send_mail(TO_FFSERV, ".", &que);

	//LOG(em_MSG), "[%s] FF�p�������ё��M�v�����M���� (FACT_TO_FFSERV_01) [ID=%s]", my_sThreadName, que.fc.data);
	//syslog(PARAM_SYSLOG_BASENO + 0, "[ID=%s]", que.fc.data);	// 0:FF���M�p�̌������ѓ`�����쐬���܂����B

	LOG(em_MSG), "[%s] FF�p�������ё��M�v�����M���� (FACT_TO_FFSERV_01) [ID=%s]", my_sThreadName, cKey);
	syslog(355, "[ID=%s]", cKey);	// 0:FF���M�p�̌������ѓ`�����쐬���܂����B
}



//------------------------------------------
// �����M�̎��т̌��o��FF���M����
// char const* cKizukenNo �Ǘ�No
//------------------------------------------
bool MakeFfManager::ExecUnsentResult(char const* cKizukenNo)
{
	LOG(em_MSG), "[%s] �����M���ё��M����<%s> �J�n", my_sThreadName, cKizukenNo );


	// �����o�ϐ��̃o�b�N�A�b�v���s��
	char sKizukenNo_backup[SIZE_KIZUKEN_NO];
	memcpy(sKizukenNo_backup, m_cKizukenNo, SIZE_KIZUKEN_NO);
	PRO_TYPE_SAISUN typBuf_backup;
	memcpy(&typBuf_backup, &m_typBuf, sizeof(PRO_TYPE_SAISUN));
	double dHosei_backup = m_dHosei;


	// �ڑ����؂�邩������Ȃ��̂ŁA��U�ؒf���Ă���A�Đڑ��������s���Ă������A�K�v���H

	//// �������ϐ� ������
	//SAFE_DELETE(mcls_pDB);

	////// ODBC�ڑ��N���X �C���X�^���X����
	//mcls_pDB = new OdbcBase();
	//if( ! mcls_pDB->Connect(TASKINI_NAME)) {
	//	// �f�[�^�x�[�X�ڑ��G���[
	//	LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
	//	syslog(SYSNO_DB_CONNECT_ERR, "[%s]", my_sThreadName);
	//	LOG(em_ERR), "[%s] �����M���ё��M����<%s> ���s", my_sThreadName, cKizukenNo);
	//	return false;
	//}

	// �����M���ю擾
	if( ! GetUnsentCoilResult(cKizukenNo) ) {
		LOG(em_ERR), "[%s] �����M���ё��M����<%s> ���s", my_sThreadName, cKizukenNo);
		return false;
	}

	// �����M�̎��т����݂�����
	if( 0 == m_nRecCnt ){
		LOG(em_MSG), "[%s] �����M���ё��M����<%s> ���� �o�b�N�A�b�v���M�Ώۖ���", my_sThreadName, cKizukenNo);
		return true;
	}

	//// FF���M�r���Ɠ��肷��ׂɃp�����[�^���擾
	//// ���ɓǂݍ��݊������Ă���̂ŁA�����œǂݍ��ޕK�v�͖���
	//if( ! ReadParam() ) {
	//	LOG(em_ERR), "[%s] �p�����[�^�ǂݍ��ݎ��s", my_sThreadName);
	//	return false;
	//}

	// �����M���т̐����Ƒ��M
	for(int ii = 0 ; ii < m_nRecCnt ; ii++ ){
		if( MakeUnsentFFSendData(ii) ) {
			UpdateCoilResultForUnsentData(m_unsent_data[ii].kizukenNo);
			// �o�b�N�A�b�v���M�ς݃J�E���g++
			m_nSentCnt++;
		}
	}

	LOG(em_MSG), "[%s] �����M���ё��M����<%s> ���� ���M���� [%d]/[%d]", my_sThreadName, cKizukenNo,m_nSentCnt,m_nRecCnt);
	syslog(357, "[���M����=%d]", m_nSentCnt);	// FF�o�b�N�A�b�v���ё��M���s���܂����B

	// �o�b�N�A�b�v���������o�ϐ��𕜋A����
	memcpy(m_cKizukenNo,sKizukenNo_backup,SIZE_KIZUKEN_NO);
	memcpy(&m_typBuf,&typBuf_backup,sizeof(PRO_TYPE_SAISUN));
	m_dHosei =  dHosei_backup;

	return true;
}

//------------------------------------------
// �����M�f�[�^�̃R�C�����т𒊏o
// char const* cKizukenNo �Ǘ�No
//------------------------------------------
bool MakeFfManager::GetUnsentCoilResult(char const* cKizukenNo)
{
	CString		cSql;
	CString     fmt;

	// �����f���R�[�h�����J�E���g�G���A������
	m_nRecCnt = 0; 
	// �o�b�N�A�b�v���M�ς݃J�E���g������
	m_nSentCnt = 0;

    // �����f���R�[�h���̑ޔ��G���A������
	for(int i01 = 0 ; i01 < UNSENT_MAX ; i01++){
		memset(m_unsent_data[i01].kizukenNo, 0x00 , sizeof(m_unsent_data[i01].kizukenNo));
		m_unsent_data[i01].len = 0;
		m_unsent_data[i01].pcode[0] = 0;
		m_unsent_data[i01].pcode[1] = 0;
	}

	// ���Y�Ǘ�No�̃��R�[�h���o�^���t���ߋ��̃f�[�^�𒊏o(�A���k����Ԏw��)
	cSql="SELECT R.�Ǘ�No, R.����������, R.�����p�^�[���R�[�h_�\, R.�����p�^�[���R�[�h_��";
	cSql += ",C.����No , C.���YNo ";
	cSql += "FROM ";
	cSql += DB_COIL_RESULT;
	cSql += " as R ";
	cSql += " LEFT OUTER JOIN ";
	cSql += DB_ACCESS_NEXT_COIL_INF;
	cSql += " as N on R.�Ǘ�No = N.�Ǘ�No ";
	cSql += "RIGHT OUTER JOIN ";
	cSql += DB_COIL_INF;
	cSql += " as C on R.�Ǘ�No = C.�Ǘ�No ";
	cSql += "WHERE R.�o�^���t < ";
	cSql += "(SELECT �o�^���t FROM ";
	cSql += DB_COIL_RESULT;
	cSql += " WHERE �Ǘ�No = '";
	cSql += cKizukenNo;
	cSql += "')";
	cSql += " AND R.�o�^���t >= ";
	cSql += "(SELECT �o�^���t FROM ";
	cSql += DB_COIL_RESULT;
	cSql += " WHERE �Ǘ�No = '";
	cSql += cKizukenNo;
	cSql += "') - ";
	fmt.Format("%d", m_nChkUnSentDate);				// INI�t�@�C���̋������M��������(����)
	cSql += fmt;
	cSql += " AND R.[�E�H�[�}�[�ދ敪] <> 1";		// �E�H�[�}�[�ނ́A�������M�Ώۂ��珜�O����
	cSql += " AND R.[�v���R�����є��f] = 1";		// �v���R�����є��f�� 1:�����f ���Ώ�
	cSql += " AND R.[�O�����ё��M����] = 0";		// �O�����ё��M���� 0:���M ���Ώ� 
	cSql += " AND N.�Ǘ�No IS NULL";				// ���H���R�C�����e�[�u���ɊǗ�No�������Ȃ����R�[�h���Ώ�
	cSql += " ORDER BY R.�o�^���t ";

	//LOG(em_NON), "[%s] %s", my_sThreadName, cSql);

	if( ! mcls_pDB->ExecuteDirect(cSql)) {
		LOG(em_ERR), "[%s] �o�b�N�A�b�v���M�Ώ� SELECT�G���[ [%s]", my_sThreadName, m_cKizukenNo);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", m_cKizukenNo);
		return false;
	}
	while(true) {
		int sqlrc = mcls_pDB->Fetch();
		if (sqlrc == KIZUODBC_FETCH_OK) {				// �f�[�^�L��

			if( UNSENT_MAX <= m_nRecCnt ){
				// �ő匏���I�[�o�[
				LOG(em_WAR), "[%s] �o�b�N�A�b�v���M�Ώی����I�[�o�[(%d��)", my_sThreadName, UNSENT_MAX);
				break;
			}

			UNSENT_DATA* pData = &m_unsent_data[m_nRecCnt];
			
			mcls_pDB->GetData(1, &(pData->kizukenNo) , SIZE_KIZUKEN_NO );	// �Ǘ�No
			mcls_pDB->GetData(2, &(pData->len ));							// �r��������
			mcls_pDB->GetData(3, &(pData->pcode[0]));						// �����p�^�[���R�[�h �\
			pData->pcode[0]--;	// 0�I���W���ɂ���
			mcls_pDB->GetData(4, &(pData->pcode[1]));						// �����p�^�[���R�[�h ��
			pData->pcode[1]--;	// 0�I���W���ɂ���
			mcls_pDB->GetData(5, &(pData->MeiNo) , sizeof(pData->MeiNo));			// ���ߔԍ�
			mcls_pDB->GetData(6, &(pData->CoilNo) , sizeof(pData->CoilNo));			// �R�C��No
			
			m_nLen = pData->len;

			LOG(em_MSG), "[%s] �o�b�N�A�b�v���M�Ώێ���[%02d] <%s> %d(mm) �����p�^�[��[%d][%d]", my_sThreadName, m_nRecCnt, pData->kizukenNo, pData->len, pData->pcode[0]+1, pData->pcode[1]+1 );
			m_nRecCnt++;

		} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {	// �f�[�^����
			break;	
		} else {										// �G���[
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", m_cKizukenNo);
			return false;
		}
	}

	return true;
}


//------------------------------------------
// �����M���уf�[�^�̐���
// int nRec �����M�f�[�^�̃��R�[�h
//------------------------------------------
bool MakeFfManager::MakeUnsentFFSendData(int nRec)
{
	UNSENT_DATA* pData = &m_unsent_data[nRec];

	FF_JISSEKI			typZi[2];						// FF�p �r�������ѓ`��
	FF_STS_JISSEKI		typSts[2];						// FF�p ������ԓ`��
	FF_STS				typPosition[2];					// �R�C���ʒu���
	FF_INF				typDefect[2];					// FF�ɑ��M����\���������r���
	int					nSpm = pData->nSpm;
	memset(typPosition, 0x00, sizeof(typPosition));
	memset(typDefect, 0x00, sizeof(typDefect));

	// �Ǘ�No���Z�b�g����
	memcpy(m_cKizukenNo, pData->kizukenNo, SIZE_KIZUKEN_NO);

	// �v���R����M�f�[�^�͖���M�̂��߁A�����l���Z�b�g����
	// ���ߔԍ��Z�b�g (COIL_INF���擾)
	memcpy(m_typBuf.data.KeyInf.cMeiNo, pData->MeiNo, sizeof(m_typBuf.data.KeyInf.cMeiNo));
	// �R�C��No�Z�b�g (COIL_INF���擾)
	memcpy(m_typBuf.data.KeyInf.cCoilNo, pData->CoilNo, sizeof(m_typBuf.data.KeyInf.cCoilNo));
	// �v���R�����уR�C�����Ƃ��āA�������������Z�b�g����[mm]��[10-1m]
	m_typBuf.data.nLen = pData->len / 100;

	// �t�����g�������܂꒷�� �s��̈� 0 �Ƃ���
	m_typBuf.data.nMakiLenF = 0;
	// �̐��� �s��̂��� 1 �Ƃ���
	m_typBuf.data.nSetCnt = 1;
	// �̐���� �P���R�[�h�ڂ̓R�C�������Z�b�g���� ����ȍ~��0���Z�b�g
	m_typBuf.data.saisun[0].nStart = 0;
	m_typBuf.data.saisun[0].nEnd = m_typBuf.data.nLen;
	for(int ii=1;ii<PRO_MAX_CUT_COUNT;ii++){
		m_typBuf.data.saisun[ii].nStart = 0;
		m_typBuf.data.saisun[ii].nEnd = 0;
	}

	// �␳���� 1.0 �Ƃ���
	m_dHosei = 1.0;

	//// FF���M�p ���я�񐶐�
	for(int ii=0; ii<NUM_MEN; ii++) {
		// �o�b�N�A�b�v���M���͕K���R�C�����т����݂���̂ŁA 0 �Őݒ�
		if( ! MakeSendData( ii, pData->pcode[ii], 0, &typDefect[ii], &typZi[ii]) ) {
			LOG(em_ERR), "[%s] <%s> FF�������ѓ`���쐬���s(�o�b�N�A�b�v���M) <%.20s>", my_sThreadName, DivNameManager::GetTorB(ii), m_cKizukenNo);
			syslog(356, "[�Ǘ�No=%s:�������ѓ`���쐬���s(%s)]", m_cKizukenNo, DivNameManager::GetTorB(ii));	// �������ѓ`���쐬�ňُ�
			return false;
		}
		if( ! MakeSendStsData( ii, pData->pcode[ii], 0, &typPosition[ii], &typSts[ii]) ){
			LOG(em_ERR), "[%s] <%s> FF������ԓ`���쐬���s(�o�b�N�A�b�v���M) <%.20s>", my_sThreadName, DivNameManager::GetTorB(ii), m_cKizukenNo);
			syslog(356, "[�Ǘ�No=%s:������ԓ`���쐬���s(%s)]", m_cKizukenNo, DivNameManager::GetTorB(ii));	// ������ԓ`���쐬�ňُ�
			return false;
		}
	}

	// ��{�ƂȂ�IDNo�𐶐�       (yyyymmdd hhnnssff\0)
	char cID[SIZE_SERIAL_FF_NO];
	KizuLib::KizuFunction::GetSerialNoFF(cID);	

	//// FF�p�������ѓ`�� �� �o�b�t�@�e�[�u���ɓo�^
	if( InsertZiBuf(cID, typZi, typSts) ) {
		LOG(em_MSG), "[%s] FF�������ё��M�o�b�t�@�e�[�u��(BUF_FF_S_ZISEKI)�o�^���� (�o�b�N�A�b�v���M) <%.20s>", my_sThreadName, m_cKizukenNo);

	}else {
		LOG(em_ERR), "[%s] FF�������ё��M�o�b�t�@�e�[�u��(BUF_FF_S_ZISEKI)�o�^���s (�o�b�N�A�b�v���M) <%.20s>", my_sThreadName, m_cKizukenNo);
		syslog(356, "[�Ǘ�No=%s:�o�^���s]", m_cKizukenNo);	// �������ё��M�`���쐬�ňُ�
		return false;
	}

	//// �v���R���̐����ю�M�o�b�t�@�e�[�u�� �X�V �� ��M���Ă��Ȃ��̂ōX�V�o���Ȃ�
	//if( UpdateProconBuf(cID) ) {
	//	LOG(em_MSG), "[%s] �v���R���̐����ю�M�o�b�t�@�e�[�u��(BUF_PRO_R_SAISUN)�X�V����", my_sThreadName);
	//} else {
	//	LOG(em_ERR), "[%s] �v���R���̐����ю�M�o�b�t�@�e�[�u��(BUF_PRO_R_SAISUN)�X�V���s", my_sThreadName);
	//}

	//// FF���M�r��� �����H�����e�[�u���ɓo�^
	////  ���H���R�C�����e�[�u��(ACCESS_NEXT_COIL_INF)
	////  ���H���r���e�[�u��    (ACCESS_NEXT_DEFECT_INF)
	if( InsertAccessNextInf( &typDefect[0], &typZi[0]) ) {
		LOG(em_MSG), "[%s] ���H�����e�[�u�� �o�^����(�o�b�N�A�b�v���M) <%.20s>", my_sThreadName, m_cKizukenNo);
	}else {
		LOG(em_ERR), "[%s] ���H�����e�[�u�� �o�^���s(�o�b�N�A�b�v���M) <%.20s>", my_sThreadName, m_cKizukenNo);
	}

	return true;
}

//------------------------------------------
// �������M���R�[�h�̃R�C�����уe�[�u�� �� �X�V
//------------------------------------------
bool MakeFfManager::UpdateCoilResultForUnsentData(char* cKizukenNo)
{
	CString		cSql;

	cSql = "UPDATE ";
	cSql += DB_COIL_RESULT;
	cSql += " SET ";
	cSql += "�v���R�����є��f = 2 "; // �o�b�N�A�b�v
	cSql += " WHERE �Ǘ�No = '";
	cSql += cKizukenNo;
	cSql += "'";

	// �o�^
#ifndef TEST_ONLINE_DATA
	if( ! mcls_pDB->ExecuteDirect(cSql,false) ) {
		LOG(em_ERR), "[%s] �R�C������ UPDATE�G���[ [%s](�o�b�N�A�b�v)", my_sThreadName, cKizukenNo);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", DB_COIL_RESULT, m_cKizukenNo);
		return false;
	}
#endif

	return true;
}