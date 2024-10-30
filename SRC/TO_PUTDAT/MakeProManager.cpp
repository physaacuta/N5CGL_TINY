#include "StdAfx.h"
#include "MakeProManager.h"
#include "limits.h"

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
MakeProManager::MakeProManager(void) :
ThreadManager("Pro"),
m_bCreateOk(true),
mcls_pDB(NULL)
{
	//// �N���e�B�J���Z�N�V�����I�u�W�F�N�g��������
	InitializeCriticalSection(&m_csLock);

	// �������ϐ� ������
	ClearMember();
	
	//// INI�t�@�C������萔�捞
	// �R�C�����я������ݖ₢���킹�����҂����� [msec]
	m_nMaxWaitTime = GetPrivateProfileInt(TO_PUTDAT, "ADDDB_MAXWAITTIME", 5000, TASKINI_NAME);
	// �R�C�������̑Ώۊ���
	m_nChkCoilDate = GetPrivateProfileInt(TO_PUTDAT, "CHKCOILDATE", 1, TASKINI_NAME);

	//// �V�O�i������
	m_evDefectDbStop	= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evDefectDbAdd		= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_dHosei = 1.0;
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
MakeProManager::~MakeProManager(void)
{
	//// �N���e�B�J���Z�N�V�����I�u�W�F�N�g�����
	DeleteCriticalSection(&m_csLock);

	//// �V�O�i�����
	CloseHandle(m_evDefectDbStop);
	CloseHandle(m_evDefectDbAdd);
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int MakeProManager::ThreadFirst()
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
int MakeProManager::ThreadLast()
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
void MakeProManager::ThreadEvent(int nEventNo)
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
			//if (m_bCreateOk != retc)
			//{
				m_bCreateOk = retc;
				KizuFunction::SetStatus("ZI_CREATE_PRO_ERR_ID", retc, false);	// �V�X���O�\���͏o���Ȃ�
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
// �ΐ���V�[�P���T�f�[�^�������ʐM �ɑ��M�˗�
// const char *cKey ID
// �߂�l : ���M����
//------------------------------------------
bool MakeProManager::Send_To_Procon(const char *cKey)
{
	bool bRet = true;

	COMMON_QUE que;					// ���[���\����
	memset(&que, 0x00, sizeof(que));

	que.nEventNo = FACT_TO_PROCON_01;
	que.nLineNo = getlineid();

	strcpy(que.fc.data, cKey);

	int nRet = send_mail(TO_PROCON, ".", &que);
	if (0 != nRet)
	{
		LOG(em_ERR), "[%s] �v���R���p�������ё��M�v��(FACT_TO_PROCON_01) ���M�G���[ [ID=%s][err_code=%d]", my_sThreadName, que.fc.data, nRet);
		syslog(SYSNO_MAIL_SNED_ERR, "[�v���R���p�������ё��M�v��(FACT_TO_PROCON_01)][err_code=%d]", nRet);
		bRet = false;
	}
	else
	{
		LOG(em_MSG), "[%s] �v���R���p�������ё��M�v��(FACT_TO_PROCON_01) ���M [ID=%s]", my_sThreadName, que.fc.data);
		//syslog(, "[ID=%s]", que.fc.data);
		bRet = true;
	}

	return bRet;
}

//------------------------------------------
// �����Ǘ��Ɏ��я������ݏ󋵖₢���킹���M�˗�
// const PRO_TYPE_DATA_CUT *pData �v���R���J�b�g���ѐ��f�[�^
//------------------------------------------
void MakeProManager::Send_To_Defect(const PRO_TYPE_DATA_CUT *pData)
{
	COMMON_QUE que;					// ���[���\����
	memset(&que, 0x00, sizeof(que));

	que.nEventNo = FACT_TO_DEFECT_13;
	que.nLineNo = getlineid();

	memcpy(que.mix.cdata[0], pData->KeyInf.cCoilNo, sizeof(pData->KeyInf.cCoilNo));
	memcpy(que.mix.cdata[1], m_cKizukenNo, sizeof(m_cKizukenNo));

	int nRet = send_mail(TO_DEFECT, ".", &que);
	if (0 != nRet)
	{
		LOG(em_ERR), "[%s] ���я����ݏ󋵖⍇��(FACT_TO_DEFECT_13) ���M�G���[ [�R�C��No=%s][�Ǘ�No=%s][err_code=%d]", my_sThreadName, que.mix.cdata[0], que.mix.cdata[1], nRet);
		syslog(SYSNO_MAIL_SNED_ERR, "[���я����ݏ󋵖⍇��(FACT_TO_DEFECT_13)][err_code=%d]", nRet);
	}
	else
	{
		LOG(em_MSG), "[%s] ���я����ݏ󋵖⍇��(FACT_TO_DEFECT_13) ���M [�R�C��No=%s][�Ǘ�No=%s]", my_sThreadName, que.mix.cdata[0], que.mix.cdata[1]);
		//syslog(, "[�Ǘ�No=%s; �R�C��No=%s]", que.mix.cdata[1], que.mix.cdata[0]);
	}
}

//------------------------------------------
// �������ϐ� ������
//------------------------------------------
void MakeProManager::ClearMember()
{
	// �������ϐ� ������
	SAFE_DELETE(mcls_pDB);
	memset(m_cKizukenNo, 0x00, sizeof(m_cKizukenNo));
	memset(&m_typBuf, 0x00, sizeof(m_typBuf));
}

//------------------------------------------
// �v���R���������� �`������
// char const* cID BUF_PRO_R_CUT�̃L�[
// �߂�l ���A���
//------------------------------------------
bool MakeProManager::Exec(char const* cID)
{
	AutoLock lock(&m_csLock);

	PRO_TYPE_ZISEKI		typZi;							// �v���R���p �������ѓ`��
	int					nZiErr = 0;						// ���э쐬�����G���[(0:���� 1:�ُ�)

	//// �������ϐ� ������
	ClearMember();

	//// ODBC�ڑ��N���X �C���X�^���X����
	mcls_pDB = new OdbcBase();
	if (!mcls_pDB->Connect(TASKINI_NAME))
	{
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "[%s]", my_sThreadName);
		syslog(359, "[ID=%s DB�ڑ��G���[]", cID);
		return false;
	}

	//// �v���R���J�b�g���ю�M�o�b�t�@�e�[�u���擾
	if (!GetBufferCut(cID))
	{
		// �擾���s
		LOG(em_ERR), "[%s] �J�b�g���ю�M�o�b�t�@�e�[�u���f�[�^�Ȃ� [ID=%s]", my_sThreadName, cID);
		syslog(359, "[ID=%s �J�b�g���ъY������]", cID);
		return false;
	}

// ����J�b�g���т��`�F�b�N�́A���񖳂��Ƃ���I
#if 0		// 1,5CGL�͓���J�b�g���ѓ`���������Ă���ꍇ�L��B�������A���ы敪�ȊO�͓�����e�ł���A�Đ������Ă����Ȃ��̂͂��I
	//// ����J�b�g���т��`�F�b�N
	// �J�b�g��R�C���������ł� ����̃J�b�g���ѓ`���������Ă���ׂ̏��u
	if (CheckEqalCut(cID))
	{
		// �΃v���R���ʐM�ɑ��M�v�������ׁA�����I���
		return true;
	}
#endif // 0

	//// �R�C�����擾
	if (!GetCoilInf_KizukenNo())
	{
		LOG(em_ERR), "[%s] �Y���R�C���Ȃ��ɂ�� �s�M�����ѓ`���𐶐� [�R�C��No=%.12s]", my_sThreadName, m_typBuf.data.KeyInf.cCoilNo);
		nZiErr = 1;
	}

	//// �����Ǘ����ŁA�Ώ۔͈͂��r���т��S��DB�o�^�������Ă��邩�`�F�b�N
	// �u���b�N�����̂Œ���
	if (0 == nZiErr)			// �R�C�����������Ȃ疳��
	{
		if (!CheckDefectInsertOk())
		{
			LOG(em_ERR), "[%s] �����Ǘ� ���я����ݒx��ɂ�� �s�M�����ѓ`���𐶐� [�R�C��No=%.12s]", my_sThreadName, m_typBuf.data.KeyInf.cCoilNo);
			syslog(354, "[�R�C��No=%.12s][�Ǘ�No=%s]", m_typBuf.data.KeyInf.cCoilNo, m_cKizukenNo);
			nZiErr = 1;
		}
	}

	//// ����������M�����J�b�g���т̃O�����g�t���O���ُ�
	if (PRO_GRAND_OK != m_typBuf.data.Common.nGrandFlg)
	{
		LOG(em_WAR), "[%s] �J�b�g���� �O�����g�t���O�ُ� [%d]", my_sThreadName, m_typBuf.data.Common.nGrandFlg);
		nZiErr = 1;
	}
	
	//// �v���R�����M�p ���ѓ`������
	MakeSendData(nZiErr, &typZi);
	

	//// �v���R���p�������ѓ`�� �� �o�b�t�@�e�[�u���ɓo�^
	if (InsertZiBuf(cID, &typZi))
	{
		LOG(em_MSG), "[%s] �v���R���������ё��M�o�b�t�@�e�[�u��(BUF_PRO_S_ZISEKI) �o�^����", my_sThreadName);

		//// �΃v���R���ʐM�Ƀv���R���p�������ё��M�v��
		if (!Send_To_Procon(cID))
		{
			LOG(em_ERR), "[%s]  [ID=%s �ʐM�^�X�N�ւ̑��M�v���ʒm�G���[]", my_sThreadName, cID);
			syslog(359, "[ID=%s �ʐM�^�X�N�ւ̑��M�v���ʒm�G���[]", cID);
		}
		else
		{
			syslog(358, "[�R�C��No=%.12s][�q��=%d][�Ǘ�No=%s][ID=%s; ���ы敪=%s; ���W�敪=%s]",
				m_typBuf.data.KeyInf.cCoilNo, m_typBuf.data.nKoban, m_cKizukenNo, cID,
				(0 == typZi.data.nZiMode) ? "����" : "�ُ�",
				(0 == typZi.data.nKikiMode) ? "����" : "�ُ�");
		}

	}
	else
	{
		LOG(em_ERR), "[%s] �v���R���������ё��M�o�b�t�@�e�[�u��(BUF_PRO_S_ZISEKI) �o�^���s", my_sThreadName);
		syslog(359, "[ID=%s �`��DB�o�^�G���[]", cID);
		return false;
	}

	//// �v���R���J�b�g���ю�M�o�b�t�@�e�[�u�� �X�V
	if (UpdateProconBuf(cID))
	{
		LOG(em_MSG), "[%s] �v���R���J�b�g���ю�M�o�b�t�@�e�[�u��(BUF_PRO_R_CUT) �X�V����", my_sThreadName);
	}
	else
	{
		LOG(em_ERR), "[%s] �v���R���J�b�g���ю�M�o�b�t�@�e�[�u��(BUF_PRO_R_CUT) �X�V���s", my_sThreadName);
	}
	
	return true;
}

//------------------------------------------
// �v���R���J�b�g���ю�M�o�b�t�@�e�[�u���擾
// char const* cID �L�[
//------------------------------------------
bool MakeProManager::GetBufferCut(char const* cID)
{
	//// SQL���쐬
	CString				cSql;
	cSql.Format("SELECT ���`�� FROM %s WHERE ID='%s'", DB_BUF_PRO_R_CUT, cID);

	// �f�[�^�擾(Binary)
	//LOG(em_NON), "%s", cSql);
	return mcls_pDB->GetSelectImage(cSql, sizeof(PRO_TYPE_CUT), (char*)&m_typBuf);
}

//------------------------------------------
// ����J�b�g���т��`�F�b�N
// char const* cID �L�[
// �߂�l true:�d���L��  false:�d������
//------------------------------------------
bool MakeProManager::CheckEqalCut(char const* cID)
{
	CString				cSql;
	CString				cWk;
	char				cSendKey[SIZE_SERIAL_FF_NO];				// ����L�[

	//// ���ݓ����擾
	SYSTEMTIME st;
	GetLocalTime(&st);

	//// ����̃J�b�g���ѓ`�����擾
	cSql = "SELECT TOP 1 ID FROM ";
	cSql += DB_BUF_PRO_R_CUT;
	cSql += " WHERE ";
	cWk.Format("ID<>'%s' AND ", cID); 																		cSql += cWk;
	cWk.Format("�L�[���='%.4s%.12s' AND ", m_typBuf.data.KeyInf.cMeiNo, m_typBuf.data.KeyInf.cCoilNo); 	cSql += cWk;
	cWk.Format("�J�n�ʒu=%d AND ", m_typBuf.data.nStart);													cSql += cWk;
	cWk.Format("�I���ʒu=%d AND ", m_typBuf.data.nEnd);														cSql += cWk;
	cWk.Format("�o�^���t>DATEADD(d,-%d,'%d/%d/%d') AND ", m_nChkCoilDate, st.wYear, st.wMonth, st.wDay);	cSql += cWk;
	cSql += "�ҏW���t IS NOT NULL ";
	cSql += "ORDER BY �o�^���t DESC";

	if (!mcls_pDB->GetSelectKey(cSql, sizeof(cSendKey), cSendKey))
	{
		// ����L�[�͖���
		LOG(em_NON), "%s", cSql);
		return false;
	}

	//// ���M����v���R�����ѓ`�����z���g�ɂ���H
	int nCnt;
	cSql.Format("SELECT COUNT(*) FROM %s WHERE ID='%s'", DB_BUF_PRO_S_ZISEKI, cSendKey);
	if (!mcls_pDB->GetSelectKey(cSql, &nCnt))
	{
		LOG(em_NON), "%s", cSql);
		return false;
	}
	if (1 != nCnt)
	{
		LOG(em_WAR), "[%s] ����J�b�g���т͂��������A���я��`���������� [ID=%s]", my_sThreadName, cSendKey);
		return false;
	}

	//// ���ɐ����������ѓ`���𑗐M
	LOG(em_INF), "[%s] ����J�b�g���т�����̂őO��̏��𑗐M [ID=%s]", my_sThreadName, cSendKey);
	if (!Send_To_Procon(cSendKey))
	{
		syslog(359, "[ID=%s �ʐM�^�X�N�ւ̑��M�v���ʒm�G���[]", cSendKey);
	}
	else
	{
		syslog(358, " (�O���񑗐M) [�R�C��No=%.12s][�q��=%d][ID=%s]",
			m_typBuf.data.KeyInf.cCoilNo,
			m_typBuf.data.nKoban,
			cSendKey);
	}
	return true;
}

//------------------------------------------
// �R�C����񂩂�Ǘ�No���擾
//------------------------------------------
bool MakeProManager::GetCoilInf_KizukenNo()
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
		LOG(em_NON), "%s", cSql);
		return false;
	}
	
	LOG(em_MSG), "[%s] �R�C��No=%.12s ��T_COIL_INF������� [�Ǘ�No=%s]", my_sThreadName, m_typBuf.data.KeyInf.cCoilNo, m_cKizukenNo);
	return true;
}

//------------------------------------------
// �����Ǘ���DB�������݊��������H
// �߂�l true:�������ŏ������݊��� false:�܂���������ł���Œ��Ȃ̂ŁA�����s�M���ɂ���
//------------------------------------------
bool MakeProManager::CheckDefectInsertOk()
{
	bool				bAnser = true;
	CString				cSql;

	//// ���������R�C�����т�����΁ADB�o�^�͊������Ă���
	int nCnt;
	cSql.Format("SELECT COUNT(*) FROM %s WHERE �Ǘ�No='%s'", DB_COIL_RESULT, m_cKizukenNo);
	if (!mcls_pDB->GetSelectKey(cSql, &nCnt))
	{
		LOG(em_NON), "%s", cSql);
		return false;
	}
	if (1 == nCnt)
	{
		LOG(em_INF), "[%s] COIL_RESULT�����ɗL��B(�ʔ����ς�) [�Ǘ�No=%s]", my_sThreadName, m_cKizukenNo);
		//LOG(em_NON), "%s", cSql);
		return true;
	}

	//// �����Ǘ��̏������ݏ�Ԗ₢���킹�҂�
	ResetEvent( m_evDefectDbAdd	);
	m_nDefectDbAddAnser = 0;

	// �����Ǘ��ɏ������ݏ�Ԗ₢���킹
	Send_To_Defect(&m_typBuf.data);

	// ��������̌��ʑ҂�
	HANDLE evCheckTimer = CreateWaitableTimer(NULL, FALSE, NULL);
	__int64 ts = m_nMaxWaitTime * -10000;	// 1�ʕb��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(evCheckTimer,   (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE);

	HANDLE hArray[] =
	{
		m_evDefectDbStop,
		m_evDefectDbAdd,
		evCheckTimer
	};
	enum
	{
		EV_STOP = WAIT_OBJECT_0,			// �X���b�h�I��
		EV_ANSER,							// �����Ǘ����A���T�[
		EV_CHECK_TIMER,						// ������Ď��^�C�}�[
		EV_END
	};

	LOG(em_MSG), "[%s] ���я����݊����ʒm (FACT_TO_PUTDAT_04) ��M�҂����", my_sThreadName);

	bool wEnd = true;
	while (wEnd)
	{
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);

		switch (ret)
		{
//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// �X���b�h�I��
			wEnd = false;
			bAnser = false;
			LOG(em_ERR), "[%s] �����Ǘ��� ���я����ݑ҂� �������f", my_sThreadName);
			break;

//-----------------------------------------------------------------------------------------------
		case EV_ANSER:												// �����Ǘ����A���T�[
			wEnd = false;
			bAnser = 0==m_nDefectDbAddAnser ? true : false;
			LOG(em_MSG), "[%s] �����Ǘ��� ���я����ݑ҂� ���� [����=%d]", my_sThreadName, m_nDefectDbAddAnser);
			break;

//-----------------------------------------------------------------------------------------------
		case EV_CHECK_TIMER:										// ������Ď��^�C�}�[
			wEnd = false;
			bAnser = false;
			LOG(em_ERR), "[%s] �����Ǘ��� ���я����ݑ҂� ��M�^�C���A�E�g", my_sThreadName);
			break;

//-----------------------------------------------------------------------------------------------
		default :
			// ���肦�Ȃ��I�I
			break;
		}

	}

	// ���
	CancelWaitableTimer(evCheckTimer);
	CloseHandle(evCheckTimer);

	return bAnser;
}

//------------------------------------------
// �v���R�����M�p ���ѓ`������
// int nZiErr ���э쐬�����G���[ (0:���� 1:�ُ�)
// PRO_TYPE_ZISEKI *pTypZi ���ѓ`��
//------------------------------------------
void MakeProManager::MakeSendData(int nZiErr, PRO_TYPE_ZISEKI *pTypZi)
{
	bool			bWk;

	//// ������
	memset(pTypZi, 0x00, sizeof(PRO_TYPE_ZISEKI));
	// 0x00�Ŗ��� �\���G���A�͗\�߂��̌^�ŏ��������Ă���
	memset(pTypZi->data.Common.cYobi, 0x20, sizeof(pTypZi->data.Common.cYobi));
	memset(pTypZi->data.cYobi1, 0x20, sizeof(pTypZi->data.cYobi1));
	memset(pTypZi->data.cYobi, 0x20, sizeof(pTypZi->data.cYobi));


	//// �`���֌������уf�[�^�Z�b�g
	if (!ExecDefectInf(pTypZi))
	{
		LOG(em_WAR), "[%s] �������уf�[�^�Z�b�g�ُ�", my_sThreadName);
	}

	//// �w�b�_�[��
	pTypZi->head.bHead[0] = PRO_HEAD_H_GYOUMU;
	pTypZi->head.bHead[1] = PRO_HEAD_L_OK;
	//pTypZi->head.nLen = sizeof(PRO_TYPE_ZISEKI);
	pTypZi->head.nLen = PRO_TYPE_LENGTH(sizeof(PRO_TYPE_ZISEKI));	// Csemi�Ή�
	pTypZi->head.nTC = PRO_TC_ZISEKI;
	pTypZi->data.Common.nKubun = PRO_KUBUN_NOMAL;

	// ���M����
	SYSTEMTIME STime;
	GetLocalTime(&STime);
	pTypZi->data.Common.nDate[0] = STime.wYear % 100;
	pTypZi->data.Common.nDate[1] = STime.wMonth;
	pTypZi->data.Common.nDate[2] = STime.wDay;
	pTypZi->data.Common.nDate[3] = STime.wHour;
	pTypZi->data.Common.nDate[4] = STime.wMinute;
	pTypZi->data.Common.nDate[5] = STime.wSecond;

	// �O�����g�t���O (0:���� 1:�ُ�)
	if (1 == nZiErr) pTypZi->data.Common.nGrandFlg = PRO_GRAND_NG;	// �ُ�
	else			 pTypZi->data.Common.nGrandFlg = PRO_GRAND_OK;	// ����
	
	//// �f�[�^��
	pTypZi->data.nZiMode = pTypZi->data.Common.nGrandFlg;			// ���ы敪 = �O�����g�t���O�̃f�[�^�Ɠ���

	// ���W�敪
	bWk = CheckState_Kensa();								// �Ώ۔͈͓��̌�����Ԃ��`�F�b�N
	pTypZi->data.nKikiMode = bWk ? 0 : 1;					// (0:���� 1:�ُ�)

	memcpy(pTypZi->data.KeyInf.cMeiNo, m_typBuf.data.KeyInf.cMeiNo, sizeof(m_typBuf.data.KeyInf.cMeiNo));		// ����No
	memcpy(pTypZi->data.KeyInf.cCoilNo, m_typBuf.data.KeyInf.cCoilNo, sizeof(m_typBuf.data.KeyInf.cCoilNo));	// �R�C��No
	pTypZi->data.nKoban = m_typBuf.data.nKoban;				// ���� (1CAPL�̂݃f�[�^�������Ă���͂��BCGL�͋�)

	return;
}

//------------------------------------------
// �Ώ۔͈� �̌�����Ԃ͐��킩�`�F�b�N
//------------------------------------------
bool MakeProManager::CheckState_Kensa()
{
	CString			cSql;
	int				nCnt;
	int				nBetween[2];			// �J�n�A�I���͈� COIL_POSITION�̃C���f�b�N�X�ʒu (ID��5m�Ԋu�ŐU����(��:ID=0:0m-4.9m...))

	//// ���������ُ��~���Ă���H
	// ���̎��_�ł́A���т��o���Ă��Ȃ��ꍇ�����ʂɂ���B���̏ꍇ�́A�֌W�Ȃ�
	// ���т��o���オ���Ă���ꍇ�ɁA���ъi�[�敪 �� �ُ��~�̏ꍇ�� �s�M���ɂ���
	cSql.Format("SELECT ISNULL(���ъi�[�敪,0) FROM %s WHERE �Ǘ�No='%s' ", DB_COIL_RESULT, m_cKizukenNo );
	if (mcls_pDB->GetSelectKey(cSql, &nCnt))
	{
		if (DIV_END_STOP_MANUAL == nCnt || DIV_END_STOP_AUTO == nCnt)
		{
			LOG(em_WAR), "[%s] ���ъi�[�敪��������~�̂��߁A�s�M���t���O�Z�b�g", my_sThreadName);
			LOG(em_NON), "%s", cSql);
			return false;
		}
	}

	//// ����̑Ώ۔͈�
#ifdef PRO_SEND_ZI_HOSEI
	nBetween[0] = (int)((double)m_typBuf.data.nStart / 10.0 / m_dHosei) / MAP_REPRES_MIN;
	nBetween[1] = (int)((double)m_typBuf.data.nEnd / 10.0 / m_dHosei + 0.9) / MAP_REPRES_MIN;
	LOG(em_MSG), "[%s] ������Ԕ��� ��ۺݍ̐��J�n,�I���ʒu[%d�`%d](10-1m) �q��[%d] �␳��[%f%%]", my_sThreadName, m_typBuf.data.nStart, m_typBuf.data.nEnd, m_typBuf.data.nKoban, m_dHosei);
#else
	nBetween[0] = (int)((double)m_typBuf.data.nStart / 10.0) / MAP_REPRES_MIN;
	nBetween[1] = (int)((double)m_typBuf.data.nEnd / 10.0 + 0.9) / MAP_REPRES_MIN;
	LOG(em_MSG), "[%s] ������Ԕ��� ��ۺݍ̐��J�n,�I���ʒu[%d�`%d](10-1m) �q��[%d]", my_sThreadName, m_typBuf.data.nStart, m_typBuf.data.nEnd, m_typBuf.data.nSubNo);
#endif // PRO_SEND_ZI_HOSEI
	LOG(em_MSG), "[%s] ������Ԕ��� �ŏ���\�r�Ԋu���Z �J�n,�I���ʒu[%d�`%d](%dm) ", my_sThreadName, nBetween[0], nBetween[1], MAP_REPRES_MIN);

	//// �Ώ۔͈͓��Ƀf�[�^�͂��邩�H (���̏��������ł́A�`�F�b�N�ł��Ȃ���)
	cSql.Format("SELECT COUNT(*) FROM %s WHERE �Ǘ�No='%s' AND (ID BETWEEN %d AND %d)",
		DB_COIL_POSITION, m_cKizukenNo, nBetween[0], nBetween[1]);
	if (!mcls_pDB->GetSelectKey(cSql, &nCnt))
	{
		LOG(em_ERR), "[%s] �ΏۃR�C���̃R�C���ʒu���擾���s [%d�`%d]", my_sThreadName, nBetween[0], nBetween[1]);
		LOG(em_NON), "%s", cSql);
		return false;
	}
	if (0 == nCnt)
	{
		LOG(em_WAR), "[%s] �ΏۃR�C���͈͓̔��� �ꌏ���f�[�^���� [%d�`%d]", my_sThreadName, nBetween[0], nBetween[1]);
		LOG(em_NON), "%s", cSql);
		return false;
	}

	//// �Ώ۔͈͓��ŁA����ȊO�����邩�H
	for (int ii = 0; ii < NUM_MEN; ii++) {
		if (0 == ii) {
			cSql.Format("SELECT COUNT(*) FROM %s WHERE �Ǘ�No='%s' AND �������_�\<>%d AND (ID BETWEEN %d AND %d)",
				DB_COIL_POSITION, m_cKizukenNo, DIV_KENSA_OK, nBetween[0], nBetween[1]);
		}
		else {
			cSql.Format("SELECT COUNT(*) FROM %s WHERE �Ǘ�No='%s' AND �������_��<>%d AND (ID BETWEEN %d AND %d)",
				DB_COIL_POSITION, m_cKizukenNo, DIV_KENSA_OK, nBetween[0], nBetween[1]);
		}
		if (!mcls_pDB->GetSelectKey(cSql, &nCnt))
		{
			LOG(em_ERR), "[%s] �ΏۃR�C���̃R�C���ʒu���擾���s [%d�`%d]", my_sThreadName, nBetween[0], nBetween[1]);
			LOG(em_NON), "%s", cSql);
			return false;
		}
		if (0 != nCnt)
		{
			LOG(em_WAR), "[%s] �ΏۃR�C���͈͓̔��� �����ُ�L�� [%d�`%d] %d��", my_sThreadName, nBetween[0], nBetween[1], nCnt);
			return false;
		}
	}

	// �����܂ŗ�����A���ʂƂ��S�����킾����
	return true;
}

//------------------------------------------
// �`���֌������уf�[�^�Z�b�g
// PRO_TYPE_ZISEKI* pTypZi ���ѓ`��
//------------------------------------------
bool MakeProManager::ExecDefectInf(PRO_TYPE_ZISEKI* pTypZi)
{
	CString			cSql;
	CString			cWk;
	double			dBetween[2];			// �J�n�A�I���͈� [10-1m]
	MIX_RATE_MAP	typMixRate;				// �������Z�o�p�}�b�v
	int				nWk;
	double			dWk;
	int				yId;
	int				gId;
	int				tId;
	int				nKensys;				// ���o�V�X�e�� 0:�����Ӄ� 1:�o���Ӄ� 2:����

	double			dHosei;					// �r���ŕ␳�����ʃX���b�h����ύX�����̂�h�~����ׁB 
	
	memset(&typMixRate, 0x00, sizeof(typMixRate));

	// ����̑Ώ۔͈� (�␳���𔽉f)
#ifdef PRO_SEND_ZI_HOSEI
	dHosei = m_dHosei;		// �r���ŕ␳�����ʃX���b�h����ύX�����̂�h�~����ׁB 
	dBetween[0] = m_typBuf.data.nStart * 0.1 / dHosei;
	dBetween[1] = m_typBuf.data.nEnd   * 0.1 / dHosei;
	LOG(em_MSG), "[%s] �������v�Z ��ۺݍ̐��J�n,�I���ʒu[%d�`%d](10-1m) �␳��[%f] ", my_sThreadName, m_typBuf.data.nStart, m_typBuf.data.nEnd, dHosei);
#else
	dBetween[0] = (double)m_typBuf.data.nStart;
	dBetween[1] = (double)m_typBuf.data.nEnd;
	LOG(em_MSG), "[%s] �������уf�[�^�擾 ��ۺݍ̐��J�n,�I���ʒu[%d�`%d](10-1m) �q��[%d]", my_sThreadName, m_typBuf.data.nStart, m_typBuf.data.nEnd, m_typBuf.data.nSubNo);
#endif // PRO_SEND_ZI_HOSEI

	// �������уf�[�^ �擾
	for (int ii = 0; ii<NUM_MEN; ii++) {
		cSql.Format("SELECT d.����ʒu, t.�v���R���O���[�v, g.�v���R���O���[�v \
					FROM %s AS d, %s AS t, %s AS g \
					WHERE d.�����r��=t.EdasysID AND d.����O���[�h=g.EdasysID AND d.�d������=0 AND \
					d.�Ǘ�No='%s' AND t.�v���R���O���[�v<>0 AND g.�v���R���O���[�v<>0 ",
			(0 == ii ? DB_DEFECT_INF_T : DB_DEFECT_INF_B), DB_TYPE_NAME, DB_GRADE_NAME, m_cKizukenNo);

		// ���s	
		if (!mcls_pDB->ExecuteDirect(cSql)) {
			LOG(em_ERR), "[%s] �r���擾�G���[ [%s]", my_sThreadName, m_cKizukenNo);
			LOG(em_NON), "%s", cSql);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", m_cKizukenNo);
			return false;
		}

		// �擾
		while (true) {
			int sqlrc = mcls_pDB->Fetch();
			if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��

				mcls_pDB->GetData(1, &nWk);
				mcls_pDB->GetData(2, &tId);
				mcls_pDB->GetData(3, &gId);

				// �f�[�^���f
				dWk = nWk / 1000.0;			// ����ʒu [mm]��[m]
				if (dBetween[0] > dWk || dBetween[1] < dWk) continue;	// ����̑Ώ۔͈͊O���r

				yId = (int)dWk;				// [m]�ł̃C���f�b�N�X�ʒu
				tId = tId - 1;
				gId = gId - 1;
				if (0 > yId || COIL_LEN_MAX <= yId) continue;
				if (0 > tId || MAX_PROGRP_TYPE <= tId) continue;
				if (0 > gId || MAX_PROGRP_GRADE <= gId) continue;

				// Short����I�[�o�[�̌��͉��Z���Ȃ�
				if (SHRT_MAX <= typMixRate.gp[gId].nCnt[tId]) continue;

				// �W�v
				if (!typMixRate.gp[gId].bFlg[yId]) {	// �܂������ꍇ�̂�
					typMixRate.gp[gId].bFlg[yId] = true;
					typMixRate.gp[gId].nMixRateCnt++;			// 1����1m���r�����Ƃ���
				}
				typMixRate.gp[gId].nCnt[tId] ++;

			}
			else if (sqlrc == KIZUODBC_FETCH_NODATE) {		// �f�[�^����
				break;
			}
			else {								// �G���[
				LOG(em_ERR), "[%s] FETCH�G���[ [%s]", my_sThreadName, m_cKizukenNo);
				syslog(SYSNO_DB_FETCH_ERR, "[%s]", m_cKizukenNo);
				return false;
			}
		}
	}

	//// ���������Z�o
	//int nYouso = (int)((m_typBuf.data.nEnd * 0.1) - (m_typBuf.data.nStart * 0.1));
	int nYouso = (int)(dBetween[1] - dBetween[0]);
	LOG(em_MSG), "[%s] �������v�Z ���������Z�l ����[%d](m) �J�n,�I���ʒu[%f�`%f](m) ", my_sThreadName, nYouso, dBetween[0], dBetween[1]);
	// �������уf�[�^ ���ѓ`���Z�b�g
	for (int ii=0; ii<MAX_PROGRP_GRADE; ii++)
	{
		// ������
		pTypZi->data.grade_inf[ii].nMixedRate = (short)((double)typMixRate.gp[ii].nMixRateCnt / nYouso * 100 + 0.5f);	// ������ [%]��[10-2%](�l�̌ܓ�)
		// ����
		for (int jj = 0; jj<MAX_PROGRP_TYPE; jj++)	pTypZi->data.grade_inf[ii].nTypeCnt[jj] = typMixRate.gp[ii].nCnt[jj];

		LOG(em_MSG), "[%s] Gr=%d, ������=%.03f%%, �W���=%d, ����=%d,%d,%d,%d,%d %d,%d,%d,%d,%d",
			my_sThreadName, ii + 1, ((double)typMixRate.gp[ii].nMixRateCnt / nYouso), typMixRate.gp[ii].nMixRateCnt,
			typMixRate.gp[ii].nCnt[0], typMixRate.gp[ii].nCnt[1], typMixRate.gp[ii].nCnt[2], typMixRate.gp[ii].nCnt[3], typMixRate.gp[ii].nCnt[4],
			typMixRate.gp[ii].nCnt[5], typMixRate.gp[ii].nCnt[6], typMixRate.gp[ii].nCnt[7], typMixRate.gp[ii].nCnt[8], typMixRate.gp[ii].nCnt[9]);
	}
	
	return true;
}

//------------------------------------------
// �v���R���p�������ѓ`�� �� �o�b�t�@�e�[�u���ɓo�^
// char const* cID �L�[
// PRO_TYPE_ZISEKI const* pTypZi ���ѓ`��
//------------------------------------------
bool MakeProManager::InsertZiBuf(const char *cID, const PRO_TYPE_ZISEKI *pTypZi)
{
	CString		cSql;

	cSql.Format("INSERT INTO %s VALUES(", DB_BUF_PRO_S_ZISEKI);

	CreateSQL::sqlColumnInsert(&cSql, cID, SIZE_SERIAL_FF_NO);				// ID
	CreateSQL::sqlColumnAddTime(&cSql, true);								// �o�^���t
	CreateSQL::sqlColumnInsert(&cSql, 1);									// �`���敪(1:�����M)
	CreateSQL::sqlColumnInsert(&cSql, (char*)&m_typBuf.data.KeyInf.cMeiNo, sizeof(m_typBuf.data.KeyInf));	// �L�[���
	CreateSQL::sqlColumnInsert(&cSql, m_typBuf.data.nKoban);				// ����No
	cSql += "?, ";															// ���`��
	CreateSQL::sqlColumnInsert(&cSql, m_cKizukenNo, SIZE_KIZUKEN_NO);		// �Ǘ�No
	CreateSQL::sqlColumnInsert(&cSql, NULL, 0);								// ���M���t
	CreateSQL::sqlColumnTrim(&cSql);
	cSql += ")";

#ifndef TEST_ONLINE_DATA
	// �o�^
	if (!mcls_pDB->SqlBinalyWrite(cSql, sizeof(PRO_TYPE_ZISEKI), (BYTE*)pTypZi)) {
		LOG(em_ERR), "[%s] �������ѓo�^���s [%s]", my_sThreadName, cID);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", DB_BUF_PRO_S_ZISEKI, cID);
		return false;
	}
#else
	// �o�C�i���_���v
	KizuFunction::DumpData("debag �v���R����������.txt", sizeof(PRO_TYPE_ZISEKI), (BYTE*)pTypZi);
	LOG(em_WAR), "[%s] �e�X�g���[�h�ɂ��A�v���R���������т��_���v�o��", my_sThreadName);

#endif

	return true;
}

//------------------------------------------
// �J�b�g���уe�[�u�� �� �X�V
// char const* cID �L�[
//------------------------------------------
bool MakeProManager::UpdateProconBuf(char const* cID)
{
	CString		cSql;

	cSql.Format("UPDATE %s SET ", DB_BUF_PRO_R_CUT);
	CreateSQL::sqlColumnUpdate(&cSql, "�Ǘ�No", m_cKizukenNo, SIZE_KIZUKEN_NO); 
	cSql += "�ҏW���t=";
	CreateSQL::sqlColumnAddTime(&cSql);							// �ҏW���t
	cSql += " WHERE ID='";
	cSql += cID;
	cSql += "'";

	// �o�^
	if (!mcls_pDB->ExecuteDirect(cSql))
	{
		LOG(em_ERR), "[%s] �J�b�g���� UPDATE�G���[ [ID=%s]", my_sThreadName, cID);
		LOG(em_NON), "%s", cSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s][%s]", DB_BUF_PRO_R_CUT, cID);
		return false;
	}
	return true;
}

