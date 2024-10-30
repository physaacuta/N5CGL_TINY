#include "StdAfx.h"
#include "HddManager.h"

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
HddManager::HddManager(void) :
ThreadManager("HddManager")
{
	//// �n���h������
	m_evManualExec = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evTimerExec  = CreateWaitableTimer(NULL, FALSE, NULL);

	//// ������
	m_bState = true;
	m_sNgMsg = "";
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
HddManager::~HddManager(void)
{
	//// �n���h�����
	CloseHandle(m_evManualExec);
	CloseHandle(m_evTimerExec);
}


//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int HddManager::ThreadFirst()
{
	//// ���O����
	__int64 ts = 3*1000 * -10000; 
	SetWaitableTimer(m_evTimerExec, (LARGE_INTEGER *)&ts, TIME_EXEC_CYCLE, NULL, NULL, FALSE);

	//// �C�x���g�ݒ�
	HANDLE hArray[] = { m_evTimerExec,
						m_evManualExec
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int HddManager::ThreadLast()
{
	// �������~
	CancelWaitableTimer(m_evTimerExec);
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void HddManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_TIMER_EXEC = 0,				// �������s ������^�C�}
			EV_MANUAL_EXEC					// �蓮���s
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_TIMER_EXEC: 					// �������s ������^�C�}
	case EV_MANUAL_EXEC:					// �蓮���s
		// 20181220 Smart Storage Administrator�Ή� --->>>
		if (EM_HDDCHK_MODE_SSA == m_nMode) {
			ExecSSA();
		}
		else {
			Exec();
		}
		//Exec();
		// 20181220 Smart Storage Administrator�Ή� ---<<<
		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��{

//------------------------------------------
// �֐�������
//------------------------------------------
void HddManager::Alloc()
{
	//// ini�t�@�C���擾
	CString  cKey;
	char wk[256];

	m_nChkNum = 0;
	for(int ii=0; ii<MAX_CHK_TXT; ii++) {
		cKey.Format("CHK_TXT%d", ii+1);
		GetPrivateProfileString(KS_HDDCHK, cKey, "", wk, sizeof(wk), PCINI_NAME);
		if( 0 == strlen(wk) ) {
			break;
		} else {
			m_sChkTxt[m_nChkNum] = wk;
			m_nChkNum += 1;
		}
	}

	m_nOkNum = 0;
	for(int ii=0; ii<MAX_CHK_TXT; ii++) {
		cKey.Format("OK_TXT%d", ii+1);
		GetPrivateProfileString(KS_HDDCHK, cKey, "", wk, sizeof(wk), PCINI_NAME);
		if( 0 == strlen(wk) ) {
			break;
		} else {
			m_sOkTxt[m_nOkNum] = wk;
			m_nOkNum += 1;
		}
	}

	m_nStatusID = GetPrivateProfileInt(KS_HDDCHK, "STATEID", 0, PCINI_NAME);

	// 20181220 Smart Storage Administrator�Ή� --->>>
	m_nMode = GetPrivateProfileInt(KS_HDDCHK, "MODE", 0, PCINI_NAME);
	// 20181220 Smart Storage Administrator�Ή� ---<<<


	//// �������`�F�b�N
	_ASSERT(m_nChkNum);
	_ASSERT(m_nOkNum);
	_ASSERT(m_nStatusID);

	//// �@���Ԃ𐳏�ɖ߂�
	KizuFunction::SetStatus(m_nStatusID, true, false);
}

//------------------------------------------
// �ڍ׌��ʕ\��
//------------------------------------------
void HddManager::ShowEdit()
{
	// �������N��
	char cPrm[MAX_PATH];
	sprintf(cPrm, "notepad.exe %s", CMD_TXT_NAME);
	ProcessManager::ProcessExec(cPrm, SW_SHOWMAXIMIZED);//SW_NORMAL);
}

//------------------------------------------
// �`�F�b�N
//------------------------------------------
void HddManager::Exec()
{
	CString sRow;
	CString sWk;
	char	cWk[2048];
	int		nWk;
	bool	bAllOk = true;

	//================================================
	// �`�F�b�N�e�L�X�g�o��
	sprintf(cWk, "property -tg=all > %s", CMD_TXT_NAME);
	ProcessManager::ShellExec("raidcmd", cWk, SW_HIDE);
//	ProcessManager::ShellExec("""c:\\Program Files (x86)\\Universal RAID Utility\\cli\\raidcmd""", cWk, SW_HIDE);

	// �t�@�C�������S�ɏo���オ��܂ł�����Ƒ҂�
	Sleep(2000);


	//================================================
	// �e�L�X�g�̒��g���`�F�b�N
	//// �I�[�v��
	FILE* fp = fopen(CMD_TXT_NAME, "rt"); 
	if(NULL == fp) {
		LOG(em_ERR), "[%s] HDD��ԃt�@�C�� �I�[�v�����s [%s]", my_sThreadName, CMD_TXT_NAME);
		return;
	}

	//// �ǂݍ���
	while (!feof(fp)) {
		char* ret = fgets(cWk, sizeof(cWk), fp);	// 1�s�擾
		if( 0 == ret ) break;

		//// �L�[�@:�@�l �̍s�`���ƂȂ��Ă���
		sRow = cWk;
		nWk  = 0;
		GetColumn(sRow, nWk, sWk);			// �L�[���擾

		// �`�F�b�N�Ώۂ̍s���o	
		for(int ii=0; ii<m_nChkNum; ii++) {
			if( 0 == strcmp(sWk, m_sChkTxt[ii]) ) {
					
				GetColumn(sRow, nWk, sWk);			// �l���擾
				
				// OK�F���ƂȂ镶��������
				bool bOk = false;
				for(int jj=0; jj<m_nOkNum; jj++) {
					if( 0 == strcmp(sWk, m_sOkTxt[jj]) ) {
						bOk = true;
						break;
					}
				}

				// HDD�ُ�
				if( ! bOk ) {
					if(m_bState) {
						LOG(em_ERR), "[%s] HDD�ُ팟�m�I[%s]", my_sThreadName, sWk);
						KizuFunction::SetStatus(m_nStatusID, false, true, sWk);
					}
					m_bState = false;
					m_sNgMsg = sWk;
					bAllOk = false;
				}
			}
		}
	}

	//// �N���[�Y
	fclose(fp);

	//// HDD���S��OK�̏ꍇ�A���한��
	if( bAllOk && ! m_bState) {
		LOG(em_INF), "[%s] HDD���한��", my_sThreadName);
		KizuFunction::SetStatus(m_nStatusID, true, true, sWk);
		m_bState = true;
		m_sNgMsg = "";
	}

	LOG(em_MSG), "[%s] HDD��ԃ`�F�b�N���� [%s]", my_sThreadName, m_bState?"����":"�ُ�");
}


//------------------------------------------
// �P�s�̃f�[�^���� : �P�ʂ̕�������擾
// CString& sVal 1�s�P�ʂ̏��
// int& curPos ��؂�ʒu
// CString& sToken �擾������̕�����
// �߂�l ���A���
//------------------------------------------
bool HddManager::GetColumn(CString& sVal, int& curPos, CString& sToken)
{
	CString sWk;
	sWk = sVal.Tokenize(":", curPos);
	sWk.Trim(_T(" 	\r\n"));					// �O��̃X�y�[�X�A�^�u����菜��
	if( 0 == sWk.GetLength() ) return false;

	sToken = sWk;
	return true;
}

// 20181220 Smart Storage Administrator�Ή� --->>>
//------------------------------------------
// �`�F�b�N
//------------------------------------------
void HddManager::ExecSSA()
{
	CString sWk;
	char	cWk[2048];
	bool	bAllOk = true;

	static const char cCmd[] = "\"%PROGRAMFILES%\\Smart Storage Administrator\\ssacli\\bin\\ssacli.exe\" ctrl all show config";
	CString sExpcmd;
	int nLen = ExpandEnvironmentStrings(cCmd, sExpcmd.GetBuffer(4096), 4096);		// ���ϐ�������𕶎���ɓW�J
	sExpcmd.ReleaseBuffer(nLen);

	//================================================
	// �`�F�b�N�e�L�X�g�o��
	CString sResult;
	int res = ProcessManager::ProcessExecWaitAns(sExpcmd, sResult.GetBuffer(4 * 1024 * 1024), SW_HIDE);		// �R�}���h���s(���ʕԋp)
	sResult.ReleaseBuffer();

	FILE* fw = fopen(CMD_TXT_NAME, "w");
	if (NULL != fw) {
		fwrite(sResult, sResult.GetLength(), 1, fw);
		fclose(fw);
	}
	if (0 == sResult.GetLength()) {
		// ���ʂȂ��ُ͈�
		sWk.Format("HDD��Ԏ擾���s[%d]", res);
		if (m_bState) {
			KizuFunction::SetStatus(m_nStatusID, false, true, sWk);
		}
		LOG(em_ERR), "[%s] HDD��Ԏ擾���s�I[%s]", my_sThreadName, sWk);

		m_bState = false;
		m_sNgMsg = sWk;
		bAllOk = false;
	}

	LOG(em_INF), "[%s] HDD��ԃt�@�C���o�� [%s]", my_sThreadName, sExpcmd);

	//================================================
	// �e�L�X�g�̒��g���`�F�b�N
	//// �I�[�v��
	FILE* fp = fopen(CMD_TXT_NAME, "rt");
	if (NULL == fp) {
		LOG(em_ERR), "[%s] HDD��ԃt�@�C�� �I�[�v�����s [%s]", my_sThreadName, CMD_TXT_NAME);
		return;
	}

	//// �ǂݍ���
	while (!feof(fp)) {
		char* ret = fgets(cWk, sizeof(cWk), fp);	// 1�s�擾
		if (0 == ret) break;

		sWk = cWk;
		sWk.Trim(_T(" 	\r\n"));					// �O��̃X�y�[�X�A�^�u����菜��

		// �`�F�b�N�Ώۂ̍s���o	
		for (int ii = 0; ii < m_nChkNum; ii++) {
			//// �`�F�b�N���镶������T�[�`�B
			if (NULL != strstr(sWk, m_sChkTxt[ii])) {
				bool bOk = false;
				// OK�F���ƂȂ镶��������
				for (int jj = 0; jj<m_nOkNum; jj++) {
					if (NULL != strstr(sWk, m_sOkTxt[jj])) {
						bOk = true;
						break;
					}
				}

				// HDD�ُ�
				if (!bOk) {
					if (m_bState) {
						KizuFunction::SetStatus(m_nStatusID, false, true, sWk);
					}
					LOG(em_ERR), "[%s] HDD�ُ팟�m�I[%s]", my_sThreadName, sWk);

					m_bState = false;
					m_sNgMsg = sWk;
					bAllOk = false;
				}
			}
		}

	}

	//// �N���[�Y
	fclose(fp);

	//// HDD���S��OK�̏ꍇ�A���한��
	if (bAllOk && !m_bState) {
		LOG(em_INF), "[%s] HDD���한��", my_sThreadName);
		KizuFunction::SetStatus(m_nStatusID, true, true, sWk);
		m_bState = true;
		m_sNgMsg = "";
	}

	LOG(em_MSG), "[%s] HDD��ԃ`�F�b�N���� [%s]", my_sThreadName, m_bState ? "����" : "�ُ�");
}
// 20181220 Smart Storage Administrator�Ή� ---<<<
