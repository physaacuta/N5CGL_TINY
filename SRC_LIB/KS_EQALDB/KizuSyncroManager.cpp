#include "StdAfx.h"
#include "KizuSyncroManager.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// �R���X�g���N�^
// CString cSession �Z�b�V����
// LogFileManager* pLog ���O�t�@�C���}�l�[�W���[�|�C���^
//------------------------------------------
KizuSyncroManager::KizuSyncroManager(CString cSession, LogFileManager* pLog) :
my_cSession(cSession),
mcls_pLog(pLog),
my_bStopFlg(false),
mcls_pDbA(NULL),
mcls_pDbB(NULL)
{
	//// �V�O�i������
	my_evNowExecAtoB	 = CreateEvent(NULL, FALSE, FALSE, NULL);	
	my_evNowExecBtoA	 = CreateEvent(NULL, FALSE, FALSE, NULL);	


	//// �K�v�ȏ���ini����擾
	GetPrivateProfileString(my_cSession, "DB_ODBC_A", "", my_cOdbcSession[0], sizeof(my_cOdbcSession[0]), TASKINI_NAME);
	GetPrivateProfileString(my_cSession, "DB_ODBC_B", "", my_cOdbcSession[1], sizeof(my_cOdbcSession[1]), TASKINI_NAME);
	GetPrivateProfileString(my_cSession, "LINK_PATH_AB", "", my_cLinkPath[0], sizeof(my_cLinkPath[0]), TASKINI_NAME);
	GetPrivateProfileString(my_cSession, "LINK_PATH_BA", "", my_cLinkPath[1], sizeof(my_cLinkPath[1]), TASKINI_NAME);

	my_nTimeFast	 = GetPrivateProfileInt(my_cSession, "TIME_FAST", -1, TASKINI_NAME);
	my_nTimeFast	 = my_nTimeFast * 60 * 1000;								// �� �� ms
	my_nTimeInterval = GetPrivateProfileInt(my_cSession, "TIME_INTERVAL", -1, TASKINI_NAME);
	my_nTimeInterval = my_nTimeInterval * 60 * 1000;							// �� �� ms
	my_nMailKey		 = GetPrivateProfileInt(my_cSession, "MAIL_KEY", 0, TASKINI_NAME);



	// �`�F�b�N
	_ASSERT(strlen(my_cOdbcSession[0]));
	_ASSERT(strlen(my_cOdbcSession[1]));
	//_ASSERT(strlen(my_cLinkPath[0]));
	//_ASSERT(strlen(my_cLinkPath[1]));
	//_ASSERT(my_nTimeFast);
	//_ASSERT(my_nTimeInterval);
}


//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
KizuSyncroManager::~KizuSyncroManager(void)
{
	// �J��
	CloseHandle(my_evNowExecAtoB);
	CloseHandle(my_evNowExecBtoA);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���ʃ��\�b�h

//------------------------------------------
// �������s�V�O�i���R�[��
// int nAtoB  0:A��B    1:B��A
// int key �C�ӂ̒l
// �߂�l�F���s�Ώێ�true
//------------------------------------------
bool KizuSyncroManager::SetEvNowExec(int nAtoB, int nMailKey)
{
	// �����̃C���X�^���X���ΏہH
	if(0 == nMailKey) return false;
	if(nMailKey != my_nMailKey) return false;

	// 0:A��B    1:B��A
	if(0 == nAtoB) {
		SetEvent(my_evNowExecAtoB);
	} else if(1 == nAtoB) {
		SetEvent(my_evNowExecBtoA);
	}
	return true;
}

//------------------------------------------
// A�n��B�n��DB�������Ă��邩�`�F�b�N����
// �߂�l�F trur:OK   false:NG
//------------------------------------------
bool KizuSyncroManager::CheckDB_Connect()
{
	CString		sql;
	int			nTbl;

	////// �T�[�o�[�A�N�Z�X A�n�m�F
	// ODBC���o�R���ăA�N�Z�X�\���m�F
	OdbcBase cls_DB;
	if( ! cls_DB.Connect( TASKINI_NAME, my_cOdbcSession[0]) ) return false;
	// A�n�̃����N�T�[�o�[���o�R���āAB�n�ɃA�N�Z�X�\���m�F
	sql.Format("SELECT COUNT(*) FROM %s%s", my_cLinkPath[0], "SysObjects");
	if( ! cls_DB.GetSelectKey( sql, &nTbl ) ) return false;
	cls_DB.DisConnect();


	////// �T�[�o�[�A�N�Z�X B�n�m�F
	// ODBC���o�R���ăA�N�Z�X�\���m�F
	if( ! cls_DB.Connect( TASKINI_NAME, my_cOdbcSession[1]) ) return false;
	// B�n�̃����N�T�[�o�[���o�R���āAA�n�ɃA�N�Z�X�\���m�F
	sql.Format("SELECT COUNT(*) FROM %s%s", my_cLinkPath[1], "SysObjects");
	if( ! cls_DB.GetSelectKey( sql, &nTbl ) ) return false;
	cls_DB.DisConnect();

	// �����܂ŗ�����A�n�EB�n�Ƃ���OK
	return true;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �}�X�^�[�ړ��֌W


//------------------------------------------
// 2DB�Ԃ̃e�[�u���𓯂��ɂ���
// bool bAtoB    true:A��B  false:B��A
// char const* tbl		�e�[�u������
//------------------------------------------
bool KizuSyncroManager::Synclock_Master(bool bAtoB, char const* tbl)
{
	bool		bAns = true;
	CString		sql;
	if(NULL == mcls_pDbA) return false; 
	if(NULL == mcls_pDbB) return false; 


	//// DB�ڑ��`�F�b�N (�ʏ�ڑ�����Ă���͂������A�O�̈�)
	// A�n
	if( ! mcls_pDbA->Connect(TASKINI_NAME, my_cOdbcSession[0] ) ) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[Synclock_Master] DB�ڑ��G���[ [%s:%s]", my_cSession, tbl);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}
	// B�n
	if( ! mcls_pDbB->Connect(TASKINI_NAME, my_cOdbcSession[1] ) ) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[Synclock_Master] DB�ڑ��G���[ [%s:%s]", my_cSession, tbl);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	// ����ȍ~�� �ُ�ŏ�����������ꍇ�́A���[���o�b�N����K�����鎖
	mcls_pDbA->AutoCommitEnable(false);				// �g�����U�N�V������L����
	mcls_pDbB->AutoCommitEnable(false);				// �g�����U�N�V������L����	


	// -------------------------------------------------------
	// �y��ȗ���z
	// �E�g�����U�N�V�����J�n
	// �E�R�s�[��̃e�[�u����S���폜
	// �E�R�s�[������R�s�[��ցA�ꊇ�őS���o�^
	// �E�R�~�b�g
	// �y���Ӂz
	// �ETo����ƂȂ�B
	// -------------------------------------------------------

	//// �A�N�Z�X�����񐶐�
	CString		sFromTblAccessPass;		// �R�s�[��DB���烊���N�T�[�o�[�o�R�Ō�DB�ɃA�N�Z�X���鎞�� ���e�[�u���p�X
	OdbcBase*	cls_pDbFrom;			// ��DB
	OdbcBase*	cls_pDbTo;				// ��DB
	if(bAtoB) {
		sFromTblAccessPass.Format("%s%s", my_cLinkPath[1], tbl);
		cls_pDbFrom = mcls_pDbA;
		cls_pDbTo	= mcls_pDbB;
	} else {
		sFromTblAccessPass.Format("%s%s", my_cLinkPath[0], tbl);
		cls_pDbFrom = mcls_pDbB;
		cls_pDbTo	= mcls_pDbA;
	}


	//// �R�s�[��̃e�[�u����S���폜
	sql.Format("DELETE FROM %s", tbl);
	if( !cls_pDbTo->ExecuteDirect(sql, false)) {	// �g�����U�N�V�����L�����Ȃ̂ŁA�ُ펞�̎����ؒf�͂��Ȃ�
		LOG(em_ERR), "[Synclock_Master] �폜 [%s:%s]", my_cSession, sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s:%s]", my_cSession, tbl);

		bAns = false;
		goto Ending;
	}


	//// �R�s�[������R�s�[��ցA�ꊇ�őS���o�^
	sql.Format("INSERT INTO %s SELECT * FROM %s",
		tbl, sFromTblAccessPass);
	if( !cls_pDbTo->ExecuteDirect(sql, false)) {	// �g�����U�N�V�����L�����Ȃ̂ŁA�ُ펞�̎����ؒf�͂��Ȃ�
		LOG(em_ERR), "[Synclock_Master] �}�X�^�[�����ُ� [%s:%s]", my_cSession, sql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s:%s]", my_cSession, tbl);
		
		bAns = false;
		goto Ending;
	}


	//// �I������ (�K�{)
Ending:
	if(bAns) {
		// �R�~�b�g
		cls_pDbTo->TranCommit();
		LOG(em_INF), "[Synclock_Master] �}�X�^�[�������� [%s:%s]", my_cSession, tbl);
	} else {
		// ���[���o�b�N
		cls_pDbTo->TranRollBack();
	}

	return bAns;
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���шړ��֌W


//------------------------------------------
// ���ʃR�s�[
// bool bAtoB    true:A��B  false:B��A
// char const* tbl		�e�[�u������
// int nExecCnt �P��̏�������
// int nKeyCnt �L����L�[��
// char const* key ��L�[�z��
//------------------------------------------
bool KizuSyncroManager::Copy_DB(bool bAtoB, char const* tbl, int nExecCnt, int nKeyCnt, char const* key)
{
	bool		bAns = true;
	if(NULL == mcls_pDbA) return false; 
	if(NULL == mcls_pDbB) return false; 

	char const* key1 = &key[PARAM_MAX_STRING*0];
	char const* key2 = &key[PARAM_MAX_STRING*1];
	char const* key3 = &key[PARAM_MAX_STRING*2];

	//// DB�ڑ��`�F�b�N (�ʏ�ڑ�����Ă���͂������A�O�̈�)
	// A�n
	if( ! mcls_pDbA->Connect(TASKINI_NAME, my_cOdbcSession[0] ) ) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[Copy_DB] DB�ڑ��G���[ [%s:%s]", my_cSession, tbl);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}
	// B�n
	if( ! mcls_pDbB->Connect(TASKINI_NAME, my_cOdbcSession[1] ) ) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[Copy_DB] DB�ڑ��G���[ [%s:%s]", my_cSession, tbl);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	// -------------------------------------------------------
	// �y��ȗ���z
	// �E�g�����U�N�V�����J�n  // �R�s�[�����Ȃ̂ŁA�g�����U�N�V��������
	// �E�R�s�[��̍ŐV�ʒuA�����
	// �E�R�s�[����A�ȍ~�̃f�[�^���A�R�s�[��֎w�茏���R�s�[ (�Â�����珇�ԂɃR�s�[�ƂȂ�)
	// �E�R�~�b�g
	// �E�҂�
	// �E�J��Ԃ� �ŏ���
	// �y���Ӂz
	// �ETo����ƂȂ�B
	// -------------------------------------------------------

	//// �A�N�Z�X�����񐶐�
	CString		sFromTblAccessPass;		// �R�s�[��DB���烊���N�T�[�o�[�o�R�Ō�DB�ɃA�N�Z�X���鎞�� ���e�[�u���p�X
//	OdbcBase*	cls_pDbFrom;			// ��DB�BFrom�ɂ�TO��LINKSERVER�o�R�ŃA�N�Z�X����̂ŃR�����g���B
	OdbcBase*	cls_pDbTo;				// ��DB
	if(bAtoB) {
		sFromTblAccessPass.Format("%s%s", my_cLinkPath[1], tbl);
//		cls_pDbFrom = mcls_pDbA;
		cls_pDbTo	= mcls_pDbB;
	} else {
		sFromTblAccessPass.Format("%s%s", my_cLinkPath[0], tbl);
//		cls_pDbFrom = mcls_pDbB;
		cls_pDbTo	= mcls_pDbA;
	}

	////// �R�s�[ (������ƂÂR�s�[)
	int nCntwk = 1;
	//�R�s�[�̕��j
	//PK�̌^���l�������Ɩʓ|�Ȃ̂ŁA�S��������ň����I
	//WHERE [���l����] > '300'�Ȃǂ��A���s�p�t�H�[�}���X��A���Ȃ��͂��B


	while(true) {
		CString sValPK[4];		//TO���ɂ���ŐV�̃��R�[�h��PK
		bool bToEmpty = false;	//TO���̃e�[�u���͋󂾂����A�A�A

		//�����̎����Ă���Ȃ��ŁA��ԐV�������R�[�h��PK���擾
		{	
			CString sSqlFillMyPK = "SELECT TOP(1) ";
			for( int ii=0; ii<nKeyCnt; ii++){
				const char *key_ii = &key[PARAM_MAX_STRING*ii];
				sSqlFillMyPK += key_ii;
				sSqlFillMyPK += ",";
            }
			sSqlFillMyPK.TrimRight(',');
			sSqlFillMyPK.AppendFormat(" FROM %s ORDER BY ", tbl);
			for( int ii=0; ii<nKeyCnt; ii++){
				const char *key_ii = &key[PARAM_MAX_STRING*ii];
				sSqlFillMyPK += key_ii;
				sSqlFillMyPK += " desc,";
			}
			sSqlFillMyPK.TrimRight(',');
            
			if( cls_pDbTo->ExecuteDirect(sSqlFillMyPK) )
			{
				int nFetchResult = cls_pDbTo->Fetch();
				if( KIZUODBC_FETCH_OK == nFetchResult  ){
					for( int nCol=1; nCol<=nKeyCnt; nCol++){
						CString sPKwk;
						int len = cls_pDbTo->GetData(nCol, sPKwk.GetBuffer( 64 ), 64 );
						sPKwk.ReleaseBuffer();
                        //�f�[�^����ꂽ
                        sValPK[nCol-1] =  sPKwk ;
                    }
                }
                else if( KIZUODBC_FETCH_NODATE == nFetchResult )
                {
                    //�f�[�^�����Ȃ��Ƃ��p
                    for( int nCol=1; nCol<=nKeyCnt; nCol++){
                        sValPK[nCol-1] = "<NUL>";	//WHERE���̂��ƂŐ�����Ԃ̂ŁA�K���B
					}
					bToEmpty = true;
                }
                else{
                    LOG(em_ERR), "[Copy_DB] ���уR�s�[F [%s:%s]", my_cSession, sSqlFillMyPK);
					syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s:%s]", my_cSession, tbl);
					bAns = false;
					goto Ending;
				}
			}
			else{
				LOG(em_ERR), "[Copy_DB] ���уR�s�[E [%s:%s]", my_cSession, sSqlFillMyPK);
				syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s:%s]", my_cSession, tbl);
				bAns = false;
				goto Ending;
			}
		}

		
		//// �R�s�[�ΏۂƂȂ�SQL���iWHERE��AORDER BY��j�̍쐬
		CString strSqlWhere;     
		CString strSqlOrderBy;
		
		if(1 == nKeyCnt) {
            CString	strSqlExecWk;
			strSqlExecWk += "WHERE ";
			strSqlExecWk += "(%s > '%s') ";
			
			strSqlWhere.Format(strSqlExecWk,
				key1, sValPK[0]);
			strSqlOrderBy.Format("ORDER BY %s asc", key1 );

		} else if(2 == nKeyCnt){
            CString	strSqlExecWk;
			strSqlExecWk += "WHERE ";
			strSqlExecWk +=  "(%s > '%s') OR ";
			strSqlExecWk += "((%s = '%s') AND (%s > '%s') ) ";
		
			strSqlWhere.Format(strSqlExecWk,
				key1, sValPK[0],
				key1, sValPK[0],     key2, sValPK[1] );
            strSqlOrderBy.Format( "ORDER BY %s asc, %s asc", key1, key2);
            
		} else if(3 == nKeyCnt){
            CString	strSqlExecWk;
			strSqlExecWk += "WHERE ";
			strSqlExecWk += "  (%s > '%s') OR ";
			strSqlExecWk += " ((%s = '%s') AND (%s > '%s') ) OR ";
			strSqlExecWk += " ((%s = '%s') AND (%s = '%s') AND (%s > '%s') ) ";
		
			strSqlWhere.Format(strSqlExecWk,
				key1, sValPK[0],
				key1, sValPK[0],     key2, sValPK[1],
				key1, sValPK[0],     key2, sValPK[1],     key3, sValPK[2] );

            strSqlOrderBy.Format("ORDER BY %s asc, %s asc, %s asc",	key1, key2, key3);
		
		} else {
			_ASSERT(false);	
		}

		//TO�̃e�[�u���͋�Ȃ̂ŁAWHERE�͍폜�B
		//�폜����̂́AVARCHAR(N), DATETIME, INT�ɋ��ʂŒʗp���鏉���l���Ȃ��ׁB
		//  '0'���ƁADATETIME���ɁA�u0��DATETIME�ɕϊ��ł��܂���v�Ƃ������o���B
		//�@Int��0 �Ȃ炢����悤�Ɍ����邪�AVARCHAR(N)�̃��R�[�h��'6000000000'�݂�����
		//�@���l�݂̂ō\������Ă�ƁA�uVARCHAR->INT�ϊ� ��INT�v�̔�r�ɂȂ�
		//�@�u�ϊ��ŃI�[�o�[�t���[�����v�Ƃ������o���B
		//�@�uVARCHAR��INT->VARCHAR�ϊ��v�Ŕ�r���邩�Ǝv�����̂ɁB
		if( bToEmpty ){
			strSqlWhere = "";
		}

		//// From�ɂ́ATO���V�������R�[�h�͂�������́H
		int nToCopy = 0;
        {
            CString		sqlCnt;
            sqlCnt.Format("SELECT COUNT(*) FROM %s ", sFromTblAccessPass );
            sqlCnt.Append(strSqlWhere);
            
            cls_pDbTo->GetSelectKey(sqlCnt, &nToCopy);
		}

        //�R�s�[���Ȃ��Ƃ����Ȃ��̂��ȁH
        if(0 == nToCopy)  { goto Ending;}
        else			{ LOG(em_INF), "[Copy_DB] ���уR�s�[ ��� %d���(�c%d)  %s, %s, %s", nCntwk, nToCopy,  sValPK[0], sValPK[1], sValPK[2]);}
      	nCntwk ++;

		// �R�s�[�J�n
		CString		sqlExec;
		sqlExec.AppendFormat("INSERT INTO %s ", tbl );
		sqlExec.AppendFormat("SELECT TOP %d * FROM %s ", nExecCnt, sFromTblAccessPass);
		sqlExec.Append(strSqlWhere);
		sqlExec.Append(strSqlOrderBy);

		if( !cls_pDbTo->ExecuteDirect(sqlExec)) { //, false)) {		// �g�����U�N�V�����L�����Ȃ̂ŁA�ُ펞�̎����ؒf�͂��Ȃ�
			LOG(em_ERR), "[Copy_DB] ���уR�s�[I [%s:%s]", my_cSession, sqlExec);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s:%s]", my_cSession, tbl);

			bAns = false;
			goto Ending;
		}

		//// �҂�
		Sleep(100);
		if(my_bStopFlg) goto Ending; // �I���H
	}

	//// �I������ (�K�{)
Ending:
	if(bAns) {
		LOG(em_INF), "[Copy_DB] ���уR�s�[���� [%s:%s]", my_cSession, tbl);
	} else {

	}
	return bAns;
}

//------------------------------------------
// ���ʈړ�
// bool bAtoB    true:A��B  false:B��A
// char const* tbl		�e�[�u������
// int nExecCnt �P��̏�������
// int nKeyCnt �L����L�[��
// char const* key ��L�[�z��
//------------------------------------------
bool KizuSyncroManager::Move_DB(bool bAtoB, char const* tbl, int nExecCnt, int nKeyCnt, char const* key)
{
	bool		bAns = true;
	CString		sql;
	int			nowCnt;				// ���݂̍폜�ʒu
	if(NULL == mcls_pDbA) return false; 
	if(NULL == mcls_pDbB) return false; 

	char const* key1 = &key[PARAM_MAX_STRING*0];
	char const* key2 = &key[PARAM_MAX_STRING*1];
//	char const* key3 = &key[PARAM_MAX_STRING*2];	// 3�ڂ͖��Ή�

	//// DB�ڑ��`�F�b�N (�ʏ�ڑ�����Ă���͂������A�O�̈�)
	// A�n
	if( ! mcls_pDbA->Connect(TASKINI_NAME, my_cOdbcSession[0] ) ) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[Move_DB] DB�ڑ��G���[ [%s:%s]", my_cSession, tbl);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}
	// B�n
	if( ! mcls_pDbB->Connect(TASKINI_NAME, my_cOdbcSession[1] ) ) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[Move_DB] DB�ڑ��G���[ [%s:%s]", my_cSession, tbl);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	// ����ȍ~�� �ُ�ŏ�����������ꍇ�́A���[���o�b�N����K�����鎖
	mcls_pDbA->AutoCommitEnable(false);				// �g�����U�N�V������L����
	mcls_pDbB->AutoCommitEnable(false);				// �g�����U�N�V������L����	

	// -------------------------------------------------------
	// �y��ȗ���z
	// �E�g�����U�N�V�����J�n
	// �E�R�s�[������R�s�[��ցA�w�茏���R�s�[ (�V��������珇�ԂɈړ��ƂȂ�)
	// �E�R�s�[������w�茏���폜
	// �E�R�~�b�g
	// �E�҂�
	// �E�J��Ԃ� �ŏ���
	// �y���Ӂz
	// �ETo����ƂȂ�B
	// -------------------------------------------------------

	//// �A�N�Z�X�����񐶐�
	CString		sFromTblAccessPass;		// �R�s�[��DB���烊���N�T�[�o�[�o�R�Ō�DB�ɃA�N�Z�X���鎞�� ���e�[�u���p�X
	OdbcBase*	cls_pDbFrom;			// ��DB
	OdbcBase*	cls_pDbTo;				// ��DB
	if(bAtoB) {
		sFromTblAccessPass.Format("%s%s", my_cLinkPath[1], tbl);
		cls_pDbFrom = mcls_pDbA;
		cls_pDbTo	= mcls_pDbB;
	} else {
		sFromTblAccessPass.Format("%s%s", my_cLinkPath[0], tbl);
		cls_pDbFrom = mcls_pDbB;
		cls_pDbTo	= mcls_pDbA;
	}


	////// ���шړ� (������ƂÂړ�)
	while(true) {
		//// �ړ��ʒu���� (ORDER BY�͕t���Ȃ��Bdelete�ŕt����Ȃ������ׁB �ł���L�[�œo�^�A�폜����̂ŏ��Ԃ͕ς��Ȃ��͂�)
		sql.Format("SELECT COUNT(*) FROM %s", tbl);
		cls_pDbFrom->GetSelectKey(sql, &nowCnt);
		if( 0 >= nowCnt ) break;					// �f�[�^���� 
		LOG(em_INF), "[Move_DB] �ړ� [%s:%s] ��� �c��%d��", my_cSession, tbl, nowCnt);

		//// �R�s�[����SQL�� (SQL��������Ȃ�̂ŁA�t�H�[�}�b�g�̂ݒ�`)
		sql.Format("INSERT INTO %s SELECT TOP %d * FROM %s ORDER BY %s desc",
			tbl, nExecCnt, sFromTblAccessPass, key1);
		if(2 <= nKeyCnt) {
			sql += ", ";
			sql += key2;
			sql += " desc";
		}

		//// �R�s�[
		if( !cls_pDbTo->ExecuteDirect(sql, false)) {	// �g�����U�N�V�����L�����Ȃ̂ŁA�ُ펞�̎����ؒf�͂��Ȃ�
			LOG(em_ERR), "[Move_DB] �R�s�[ [%s:%s]", my_cSession, sql);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s:%s]", my_cSession, tbl);

			bAns = false;
			goto Ending;
		}

		//// �폜����SQL�� (SQL��������Ȃ�̂ŁA�t�H�[�}�b�g�̂ݒ�`)
		CString		strSqlCntWk;
		if(1 == nKeyCnt) {
			strSqlCntWk  = "DELETE FROM %s WHERE ";
			strSqlCntWk += "%s >= (SELECT TOP 1 A.%s FROM ";
			strSqlCntWk +=				"(select TOP %d * from %s order by %s desc) AS A ORDER BY %s asc)";
			
			sql.Format( strSqlCntWk,
						tbl, 
						key1, key1,
						nExecCnt, tbl, key1, key1);
		} else if(2 == nKeyCnt){
			// ��L�[���Q���L���́A��ꍀ�ڂ̊Y�����O�S���ƊY�����ڂ̑�2�̃L�[�S��������
			strSqlCntWk  = "DELETE FROM %s WHERE ";
			strSqlCntWk += "%s > (SELECT TOP 1 A.%s FROM ";
			strSqlCntWk +=				"(select TOP %d * from %s order by %s desc, %s desc) AS A ORDER BY A.%s asc, A.%s asc) OR ";
			
			strSqlCntWk += "(%s = (SELECT TOP 1 B.%s FROM ";
			strSqlCntWk +=				"(select TOP %d * from %s order by %s desc, %s desc) AS B ORDER BY B.%s asc, B.%s asc) AND ";
			
			strSqlCntWk += "%s >= (SELECT TOP 1 C.%s FROM ";
			strSqlCntWk +=				"(select TOP %d * from %s order by %s desc, %s desc) AS C ORDER BY C.%s asc, C.%s asc) )";
			
			sql.Format( strSqlCntWk,
						tbl, 
						key1, key1,
						nExecCnt, tbl, key1, key2, key1, key2,
						key1, key1,
						nExecCnt, tbl, key1, key2, key1, key2,
						key2, key2,
						nExecCnt, tbl, key1, key2, key1, key2 );
		}

		//// �폜
		if( !cls_pDbFrom->ExecuteDirect(sql, false)) {	// �g�����U�N�V�����L�����Ȃ̂ŁA�ُ펞�̎����ؒf�͂��Ȃ�
			LOG(em_ERR), "[Move_DB] �폜 [%s:%s]", my_cSession, sql);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s:%s]", my_cSession, tbl);

			bAns = false;
			goto Ending;
		}

		//// �R�~�b�g
		cls_pDbFrom->TranCommit();
		cls_pDbTo->TranCommit();
		//LOG(em_INF), "[Move_DB] ���шړ����E�E�E [%s:%s]", my_cSession, tbl);

		//// �҂�
		Sleep(1000);
		if(my_bStopFlg) goto Ending; // �I���H
	}


	//// �I������ (�K�{)
Ending:
	if(bAns) {
		// �R�~�b�g
		cls_pDbFrom->TranCommit();
		cls_pDbTo->TranCommit();
		LOG(em_INF), "[Move_DB] ���шړ����� [%s:%s]", my_cSession, tbl);
	} else {
		// ���[���o�b�N
		cls_pDbFrom->TranRollBack();
		cls_pDbTo->TranRollBack();
	}
	return bAns;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���ѓ����֌W

//------------------------------------------
// ���ъ��S����
// bool bAtoB    true:A��B  false:B��A
// char const* tbl		�e�[�u������
// int nExecCnt �P��̏�������
// int nKeyCnt �L����L�[��
// char const* key ��L�[�z��
//------------------------------------------
bool KizuSyncroManager::Syncopy_DB(bool bAtoB, char const* tbl, int nExecCnt, int nKeyCnt, char const* key)
{
	bool		bAns = true;
	CString		sqlCnt;
	CString		sqlExec;
	if(NULL == mcls_pDbA) return false; 
	if(NULL == mcls_pDbB) return false; 

	char const* key1 = &key[PARAM_MAX_STRING*0];
	char const* key2 = &key[PARAM_MAX_STRING*1];
	char const* key3 = &key[PARAM_MAX_STRING*2];

	//// DB�ڑ��`�F�b�N (�ʏ�ڑ�����Ă���͂������A�O�̈�)
	// A�n
	if( ! mcls_pDbA->Connect(TASKINI_NAME, my_cOdbcSession[0] ) ) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[Syncopy_DB] DB�ڑ��G���[ [%s:%s]", my_cSession, tbl);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}
	// B�n
	if( ! mcls_pDbB->Connect(TASKINI_NAME, my_cOdbcSession[1] ) ) {
		// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[Syncopy_DB] DB�ڑ��G���[ [%s:%s]", my_cSession, tbl);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	// -------------------------------------------------------
	// �y��ȗ���z
	// �E���A��̃L�[���r���āA�Ј���ɂ����Ȃ���L�[���w�茏���R�s�[ (�V��������珇�ԂɈړ��ƂȂ�)
	// �E�҂�
	// �E�J��Ԃ� �ŏ���
	// �y���Ӂz
	// �ETo����ƂȂ�B
	// -------------------------------------------------------

	//// �A�N�Z�X�����񐶐�
	CString		sFromTblAccessPass;		// �R�s�[��DB���烊���N�T�[�o�[�o�R�Ō�DB�ɃA�N�Z�X���鎞�� ���e�[�u���p�X
	OdbcBase*	cls_pDbFrom;			// ��DB
	OdbcBase*	cls_pDbTo;				// ��DB
	if(bAtoB) {
		sFromTblAccessPass.Format("%s%s", my_cLinkPath[1], tbl);
		cls_pDbFrom = mcls_pDbA;
		cls_pDbTo	= mcls_pDbB;
	} else {
		sFromTblAccessPass.Format("%s%s", my_cLinkPath[0], tbl);
		cls_pDbFrom = mcls_pDbB;
		cls_pDbTo	= mcls_pDbA;
	}


	//// �R�s�[����SQL�� (SQL��������Ȃ�̂ŁA�t�H�[�}�b�g�̂ݒ�`)
	CString		strSqlExecWk;
	if(1 == nKeyCnt) {
		// ���Ɛ���O���������āA����NULL�Ő�Ƀf�[�^��������̂̂݃C���T�[�g
		strSqlExecWk  = "INSERT INTO %s ";
		strSqlExecWk += "SELECT TOP %d f.* FROM %s f ";
		strSqlExecWk += "LEFT OUTER JOIN %s t ON f.%s=t.%s ";
		strSqlExecWk += "WHERE t.%s IS NULL ";
		strSqlExecWk += "ORDER BY t.%s asc";
		
		sqlExec.Format(strSqlExecWk,
			tbl, 
			nExecCnt, sFromTblAccessPass,
			tbl, key1, key1,
			key1,
			key1);

	} else if(2 == nKeyCnt){
		strSqlExecWk  = "INSERT INTO %s ";
		strSqlExecWk += "SELECT TOP %d f.* FROM %s f ";
		strSqlExecWk += "LEFT OUTER JOIN %s t ON f.%s=t.%s AND f.%s=t.%s ";
		strSqlExecWk += "WHERE t.%s IS NULL AND t.%s IS NULL ";
		strSqlExecWk += "ORDER BY t.%s asc, t.%s asc";
		
		sqlExec.Format(strSqlExecWk,
			tbl, 
			nExecCnt, sFromTblAccessPass,
			tbl, key1, key1, key2, key2,
			key1, key2,
			key1, key2);

	} else if(3 == nKeyCnt){
		strSqlExecWk  = "INSERT INTO %s ";
		strSqlExecWk += "SELECT TOP %d f.* FROM %s f ";
		strSqlExecWk += "LEFT OUTER JOIN %s t ON f.%s=t.%s AND f.%s=t.%s AND f.%s=t.%s ";
		strSqlExecWk += "WHERE t.%s IS NULL AND t.%s IS NULL AND t.%s IS NULL ";
		strSqlExecWk += "ORDER BY t.%s asc, t.%s asc, t.%s asc";
		
		sqlExec.Format(strSqlExecWk,
			tbl, 
			nExecCnt, sFromTblAccessPass,
			tbl, key1, key1, key2, key2, key3, key3,
			key1, key2, key3,
			key1, key2, key3);

	} else {
		_ASSERT(false);	
	}

	//// �ŐV���`�F�b�N����SQL�� (SQL��������Ȃ�̂ŁA�t�H�[�}�b�g�̂ݒ�`)
	CString		strSqlCntWk;

	if(1 == nKeyCnt) {
		//strSqlCntWk  = "SELECT COUNT(*) FROM %s f ";
		//strSqlCntWk += "LEFT OUTER JOIN %s t ON f.%s=t.%s ";
		//strSqlCntWk += "WHERE t.%s IS NULL ";
		// ���ꂾ�ƑS���R�[�h�����ɂȂ���̂��������Ԃ��|����ׁA�΍�

		strSqlCntWk  = "SELECT COUNT(*) FROM (";

		strSqlCntWk += "SELECT TOP 1 f.* FROM %s f ";
		strSqlCntWk += "LEFT OUTER JOIN %s t ON f.%s=t.%s ";
		strSqlCntWk += "WHERE t.%s IS NULL ";
		strSqlCntWk += "ORDER BY t.%s asc";
		
		strSqlCntWk += ") as bb";

		sqlCnt.Format(strSqlCntWk,
			sFromTblAccessPass,
			tbl, key1, key1,
			key1,
			key1);

	} else if(2 == nKeyCnt){

		strSqlCntWk  = "SELECT COUNT(*) FROM (";

		strSqlCntWk += "SELECT TOP 1 f.* FROM %s f ";
		strSqlCntWk += "LEFT OUTER JOIN %s t ON f.%s=t.%s AND f.%s=t.%s ";
		strSqlCntWk += "WHERE t.%s IS NULL AND t.%s IS NULL ";
		strSqlCntWk += "ORDER BY t.%s asc, t.%s asc";

		strSqlCntWk += ") as bb";

		sqlCnt.Format(strSqlCntWk,
			sFromTblAccessPass,
			tbl, key1, key1, key2, key2,
			key1, key2,
			key1, key2);

	} else if(3 == nKeyCnt){
		strSqlCntWk  = "SELECT COUNT(*) FROM (";

		strSqlCntWk += "SELECT TOP 1 f.* FROM %s f ";
		strSqlCntWk += "LEFT OUTER JOIN %s t ON f.%s=t.%s AND f.%s=t.%s AND f.%s=t.%s ";
		strSqlCntWk += "WHERE t.%s IS NULL AND t.%s IS NULL AND t.%s IS NULL ";
		strSqlCntWk += "ORDER BY t.%s asc, t.%s asc, t.%s asc";

		strSqlCntWk += ") as bb";

		sqlCnt.Format(strSqlCntWk,
			sFromTblAccessPass,
			tbl, key1, key1, key2, key2, key3, key3,
			key1, key2, key3,
			key1, key2, key3);
	}


	////// ���ъ��S���� (������ƂÂR�s�[)
	int nCntwk = 1;
	while(true) {
		// �R�s�[�J�n
		if( !cls_pDbTo->ExecuteDirect(sqlExec)) {	
			LOG(em_ERR), "[Syncopy_DB] ���ѓ��� [%s:%s]", my_cSession, sqlExec);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s:%s]", my_cSession, tbl);

			bAns = false;
			goto Ending;
		}

		//// ����NULL���������`�F�b�N
		int cntWk;
		cls_pDbTo->GetSelectKey(sqlCnt, &cntWk);
		if(0 == cntWk)  { goto Ending;}
		else			{ LOG(em_INF), "[Syncopy_DB] ���ѓ��� ��� %d���", nCntwk);}
		nCntwk ++;

		//// �҂�
		Sleep(100);
		if(my_bStopFlg) goto Ending; // �I���H
	}


	//// �I������ (�K�{)
Ending:
	if(bAns) {
		LOG(em_INF), "[Syncopy_DB] ���ѓ������� [%s:%s]", my_cSession, tbl);
	} else {
		LOG(em_ERR), "[Syncopy_DB] ���ѓ������s [%s:%s]", my_cSession, tbl);
	}
	
	return bAns;
}