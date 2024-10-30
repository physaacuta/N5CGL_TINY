#include "StdAfx.h"
#include "OdbcBase.h"

using namespace KizuODBC;

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
OdbcBase::OdbcBase(void) :
mycls_pDB(NULL)
{
	// ���� (�ڑ��n���h���܂Ő���)
	mycls_pDB = new OdbcManager();
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
OdbcBase::~OdbcBase(void)
{
	// �O�ׂ̈ɐؒf
	DisConnect();

	// ���
	if(mycls_pDB) {
		delete mycls_pDB;
		mycls_pDB = NULL;
	}
}

// //////////////////////////////////////////////////////////////////////////////
// OdbcManager �̔�킹�֘A
// //////////////////////////////////////////////////////////////////////////////

//------------------------------------------
// �ڑ��`�F�b�N
// LPCSTR cIniPath ini�t�@�C���p�X
// LPCSTR cSession �Z�b�V����
// �߂�l ���A���
//------------------------------------------
bool OdbcBase::OpenCheck(LPCSTR cIniPath, LPCSTR cSession)
{
	int retc;
	KizuODBC::OdbcManager * cls_pDB = new OdbcManager();
	if(NULL == cSession) retc = cls_pDB->Connect(cIniPath);
	else				 retc = cls_pDB->Connect(cIniPath, cSession);
	if( 0 == retc ) {
		cls_pDB->DisConnect();
	}
	delete cls_pDB;
	return 0==retc ? true : false;
}

//------------------------------------------
// �C�ӎw��Őڑ��`�F�b�N
// char const* dsn  �f�[�^�x�[�X��
// char const* uid  ���[�U�[��
// char const* pwd  �p�X���[�h
// �߂�l ���A���
//------------------------------------------
bool OdbcBase::OpenCheck(char const* dsn, char const* uid, char const* pwd)
{
	int retc;
	KizuODBC::OdbcManager * cls_pDB = new OdbcManager();
	retc = cls_pDB->Connect(dsn, uid, pwd);
	if( 0 == retc ) {
		cls_pDB->DisConnect();
	}
	delete cls_pDB;
	return 0==retc ? true : false;
}

//------------------------------------------
// ODBC�𖢒�`�Őڑ�
// char const* addr �f�[�^�x�[�X�A�h���X
// char const* dsn  �f�[�^�x�[�X��
// char const* uid  ���[�U�[��
// char const* pwd  �p�X���[�h
// bool bNative SQL Native Client �Őڑ��� true�B�ʏ��false
// �߂�l ���A���
//------------------------------------------
bool OdbcBase::OpenCheck(char const* addr, char const* dsn, char const* uid, char const* pwd, bool bNative)
{
	int retc;
	KizuODBC::OdbcManager * cls_pDB = new OdbcManager();
	retc = cls_pDB->ConnectDriver(addr, dsn, uid, pwd, bNative);
	if( 0 == retc ) {
		cls_pDB->DisConnect();
	}
	delete cls_pDB;
	return 0==retc ? true : false;
}

//------------------------------------------
// �Z�b�V�����w��ł̐ڑ�
// LPCSTR cIniPath ini�t�@�C���p�X
// LPCSTR cSession �Z�b�V����
// �߂�l ���A���
//------------------------------------------
bool OdbcBase::Connect(LPCSTR cIniPath, LPCSTR cSession)
{
	if( NULL==mycls_pDB )				return false;
	if( ! mycls_pDB->IsCreateHandle() )	return false;		// �n���h������NG
	if( mycls_pDB->IsConnect() )		return true;		// �ڑ��ς�

	int retc;
	if(NULL == cSession) retc = mycls_pDB->Connect(cIniPath);
	else				 retc = mycls_pDB->Connect(cIniPath, cSession);
	if( 0 == retc ) {
		return true;
	} else if( 1 == retc) {
		return true;
	} else {
		mycls_pDB->GetErrMsg();
		DisConnect();					// �ؒf
		return false;
	}
}

//------------------------------------------
// �C�ӎw��ł̐ڑ�
// char const* dsn  �f�[�^�x�[�X��
// char const* uid  ���[�U�[��
// char const* pwd  �p�X���[�h
// �߂�l ���A���
//------------------------------------------
bool OdbcBase::Connect(char const* dsn, char const* uid, char const* pwd)
{
	if( NULL==mycls_pDB )				return false;
	if( ! mycls_pDB->IsCreateHandle() )	return false;		// �n���h������NG
	if( mycls_pDB->IsConnect() )		return true;		// �ڑ��ς�

	int retc;
	retc = mycls_pDB->Connect(dsn, uid, pwd);
	if( 0 == retc ) {
		return true;
	} else if( 1 == retc) {
		return true;
	} else {
		mycls_pDB->GetErrMsg();
		DisConnect();					// �ؒf
		return false;
	}
}

//------------------------------------------
// ODBC�𖢒�`�Őڑ�
// char const* addr �f�[�^�x�[�X�A�h���X
// char const* dsn  �f�[�^�x�[�X��
// char const* uid  ���[�U�[��
// char const* pwd  �p�X���[�h
// bool bNative SQL Native Client �Őڑ��� true�B�ʏ��false
// �߂�l ���A���
//------------------------------------------
bool OdbcBase::ConnectDriver(char const* addr, char const* dsn, char const* uid, char const* pwd, bool bNative)
{
	if( NULL==mycls_pDB )				return false;
	if( ! mycls_pDB->IsCreateHandle() )	return false;		// �n���h������NG
	if( mycls_pDB->IsConnect() )		return true;		// �ڑ��ς�

	int retc;
	retc = mycls_pDB->ConnectDriver(addr, dsn, uid, pwd, bNative);
	if( 0 == retc ) {
		return true;
	} else if( 1 == retc) {
		return true;
	} else {
		mycls_pDB->GetErrMsg();
		DisConnect();					// �ؒf
		return false;
	}
}



//------------------------------------------
// DB�ؒf
//------------------------------------------
void OdbcBase::DisConnect()
{
	// �ؒf
	if(NULL != mycls_pDB) {
		mycls_pDB->DisConnect(); 
	}
}

//------------------------------------------
// �I�[�g�R�~�b�g ON/OFF
// bool flg �g�����U�N�V�����Ǝg�������ꍇ�́Afalse�ɂ��鎖
// �߂�l ���A���
//------------------------------------------
bool OdbcBase::AutoCommitEnable(bool flg)
{
	if( ! mycls_pDB->IsConnect()) return false;			// ���ڑ�

	SQLRETURN sqlrc = mycls_pDB->AutoCommitEnable(flg);
	if( sqlrc == SQL_ERROR || sqlrc == SQL_INVALID_HANDLE) {
		mycls_pDB->GetErrMsg();
		DisConnect();					// �ؒf
		return false;
	} else {
		return true;
	}
}

//------------------------------------------
// SQL�����s
// LPCSTR cSql SQL��
// bool bErrAutoClose �G���[�������ؒf�@�\ (�f�t�H���g true)
//						�� �g�����U�N�V������L���Ƃ��Ă���ꍇ�ȂǂŁA����ɐؒf���ꂽ�獢���Ή�
// �߂�l ���A���
//------------------------------------------
bool OdbcBase::ExecuteDirect(LPCSTR cSql, bool bErrAutoClose)
{
	if( ! mycls_pDB->IsConnect()) return false;			// ���ڑ�

	SQLRETURN sqlrc = mycls_pDB->ExecuteDirect(cSql);
	if( sqlrc == SQL_ERROR || sqlrc == SQL_INVALID_HANDLE) {
		mycls_pDB->GetErrMsg();
		if(bErrAutoClose) DisConnect();			// �ؒf
		return false;
	} else {
		return true;
	}
}

//------------------------------------------
// �R�~�b�g
// �߂�l ���A���
//------------------------------------------
bool OdbcBase::TranCommit()
{
	if( ! mycls_pDB->IsConnect()) return false;			// ���ڑ�

	mycls_pDB->TranCommit();
	return true;
}

//------------------------------------------
// ���[���o�b�N
// �߂�l ���A���
//------------------------------------------
bool OdbcBase::TranRollBack()
{
	if( ! mycls_pDB->IsConnect()) return false;			// ���ڑ�

	mycls_pDB->TranRollBack();
	return true;
}

//------------------------------------------
// �t�F�b�`
// �߂�l KIZUODBC_FETCH_xxx
//------------------------------------------
int OdbcBase::Fetch()
{
	if( ! mycls_pDB->IsConnect()) return KIZUODBC_FETCH_ERR;			// ���ڑ�

	SQLRETURN sqlrc = mycls_pDB->Fetch();
	if (sqlrc == SQL_ERROR || sqlrc == SQL_INVALID_HANDLE) {
		mycls_pDB->GetErrMsg();
		DisConnect();					// �ؒf
		return KIZUODBC_FETCH_ERR;
	} else if(sqlrc == SQL_NO_DATA) {
		return KIZUODBC_FETCH_NODATE;	// �f�[�^����
	} else {
		return KIZUODBC_FETCH_OK;
	}
}

// //////////////////////////////////////////////////////////////////////////////
// �o�C�i���f�[�^�o�^�֘A
// //////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// �������o�C�i���o�^
// LPCSTR cSql ? �t����SQL��
// int paramCnt �p�����[�^�̐�
// DWORD size �p�����[�^�����̃o�C�i���f�[�^�T�C�Y�̔z�� (NULL�Z�b�g���́A0���w��)
// LPVOID pVal �p�����[�^�����̃o�C�i���f�[�^�̐擪�|�C���^�̔z�� (NULL�Z�b�g���́ANULL���w��)
// bool bErrAutoClose �G���[�������ؒf�@�\ (�f�t�H���g true)
// �߂�l ���A���
//------------------------------------------
bool OdbcBase::SqlBinalyWrite(LPCSTR cSql, int paramCnt, DWORD *size, LPVOID *pVal, bool bErrAutoClose)
{
	if( ! mycls_pDB->IsConnect()) return false;			// ���ڑ�

	int ii;
	SQLRETURN sqlrc;
	SQLPOINTER pToken;

	//// �p�����[�^�t��SQL���Z�b�g
	sqlrc = mycls_pDB->Prepare(cSql);
	if( sqlrc == SQL_ERROR || sqlrc == SQL_INVALID_HANDLE) {
		mycls_pDB->GetErrMsg();
		DisConnect();					// �ؒf
		return false;
	}

	//// �������o�C���h�o�^
	// ����ȍ~�� return �̓_��
	SQLLEN *cbPhotoParam = new SQLLEN[paramCnt];
	memset( cbPhotoParam, 0x00, sizeof(SQLLEN)*paramCnt);
	// �Z�b�g
	for( ii=0; ii<paramCnt; ii++) {
		//cbPhotoParam[ii] = SQL_LEN_DATA_AT_EXEC(size[ii]);

		// �o�C���h
		if( NULL == pVal[ii] ) {					// NULL�f�[�^
			sqlrc = mycls_pDB->BindParameter(ii+1, 1, &cbPhotoParam[ii]);
		} else {									// ���f�[�^
			sqlrc = mycls_pDB->BindParameter(ii+1, size[ii], &cbPhotoParam[ii]);
		}
		if( sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO ) {
			// ����
		} else {
			goto NG;
		}

		// �p�����[�^�� 1 �̃f�[�^�͎��s���Ɏw�肷��B    
		// ���ӂƂ��āA�����ł̓p�����[�^�̒�����       
		// SQL_LEN_DATA_AT_EXEC �}�N���ŁA0 �ɂ��Ă���B
		// ����́ASQLGetInfo �� SQL_NEED_LONG_DATA_LEN ��� 
		// "N" ��Ԃ��h���C�o�ŋ������B
		cbPhotoParam[ii] = (SQLLEN)SQL_LEN_DATA_AT_EXEC(0);
	}

	//// ���s
	sqlrc = mycls_pDB->Execute();
	if( sqlrc == SQL_ERROR || sqlrc == SQL_INVALID_HANDLE) {
		goto NG;
	}

	//// �p�����[�^�o�^
	for( ii=0; ii<paramCnt; ii++) {
		// �p�����[�^�ԍ��擾
		if( SQL_NEED_DATA == mycls_pDB->ParamData(&pToken)) {
			// �o�C���h���܂��̈�OK
		} else {
			goto NG;
		}

		// �f�[�^�Z�b�g 
		if( NULL == pVal[ii] ) {					// NULL�f�[�^
			sqlrc = mycls_pDB->PutData(0, 0);
		} else {									// ���f�[�^
			sqlrc = mycls_pDB->PutData(pVal[ii], size[ii]);
		}
		if (sqlrc == SQL_ERROR || sqlrc == SQL_INVALID_HANDLE) {
			goto NG;
		}
	}

	//// �p�����[�^�ԍ��擾
	if( SQL_SUCCESS == mycls_pDB->ParamData(&pToken) ) {
		// ����
	} else {
		goto NG;
	}

	//// �I������
	if (NULL != cbPhotoParam) {
		delete [] cbPhotoParam; 
		cbPhotoParam = NULL;
	}
	return true;
NG:
	mycls_pDB->GetErrMsg();
	if(bErrAutoClose) DisConnect();					// �ؒf
	if (NULL != cbPhotoParam) {
		delete [] cbPhotoParam; 
		cbPhotoParam = NULL;
	}
	return false;
}

//------------------------------------------
// 1���o�C�i���o�^ (�ȈՔ�)
// LPCSTR cSql SQL��
// DWORD size �o�C�i���f�[�^�T�C�Y
// LPVOID pVal �o�C�i���f�[�^
// bool bErrAutoClose �G���[�������ؒf�@�\ (�f�t�H���g true)
// �߂�l ���A���
//------------------------------------------
bool OdbcBase::SqlBinalyWrite(LPCSTR cSql, DWORD size, LPVOID pVal, bool bErrAutoClose)
{
	return SqlBinalyWrite(cSql, 1, &size, &pVal, bErrAutoClose);
}
//------------------------------------------
// 2���o�C�i���o�^ (�ȈՔ�)
// LPCSTR cSql SQL��
// DWORD size1 �o�C�i���f�[�^�T�C�Y
// DWORD size2 �o�C�i���f�[�^�T�C�Y
// LPVOID pVal1 �o�C�i���f�[�^
// LPVOID pVal2 �o�C�i���f�[�^
// bool bErrAutoClose �G���[�������ؒf�@�\ (�f�t�H���g true)
// �߂�l ���A���
//------------------------------------------
bool OdbcBase::SqlBinalyWrite(LPCSTR cSql, DWORD size1, DWORD size2, LPVOID pVal1, LPVOID pVal2, bool bErrAutoClose)
{
	DWORD wksize[2];
	LPVOID wkpVal[2];
	// �f�[�^�Z�b�g
	wksize[0] = size1;
	wksize[1] = size2;
	wkpVal[0] = pVal1;
	wkpVal[1] = pVal2;
	// �o�^
	return SqlBinalyWrite(cSql, 2, wksize, wkpVal, bErrAutoClose);
}

// //////////////////////////////////////////////////////////////////////////////
// �o�C�i���t�@�C���f�[�^�o�^�֘A
// //////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// �������o�C�i���t�@�C���o�^ (MAX10��)
// LPCSTR cSql ? �t����SQL��
// int paramCnt �p�����[�^�̐�
// int pathsize �o�C�i���t�@�C���p�X �̔z��T�C�Y �ʏ� MAX_PATH
// const char* cFPath �o�C�i���t�@�C���̃t���p�X
// �߂�l ���A���
//------------------------------------------
bool OdbcBase::SqlBinalyFileWrite(LPCSTR cSql, int paramCnt, int pathsize, const char* cFPath, bool bErrAutoClose)
{
	if( ! mycls_pDB->IsConnect()) return false;			// ���ڑ�

	int ii;
	SQLRETURN sqlrc;
	SQLPOINTER pToken;


	//// �t�@�C���֌W �̃f�[�^���O����
	FILE*		fp;											// �t�@�C���|�C���^
	fpos_t*		size = new fpos_t[paramCnt];				// �t�@�C���T�C�Y
	BYTE*		data = new BYTE[BinalyFileOneWriteSize];	// ���̏������݃o�b�t�@�G���A
	for( ii=0; ii<paramCnt; ii++) {
		fp = fopen(&cFPath[ii*pathsize], "rb");
		if(fp == NULL){        // �I�[�v�����s
			size[ii] = 0;
			continue;
		}
		// �t�@�C���T�C�Y�擾
		fseek(fp, 0, SEEK_END);						// ���[�Ɉړ�
		fgetpos(fp, &size[ii]);						// �t�@�C���|�C���^�ʒu�擾 = �ŏI�ʒu�Ȃ̂Ńt�@�C���T�C�Y��������
		fclose(fp);
	}
	

	//// �p�����[�^�t��SQL���Z�b�g
	sqlrc = mycls_pDB->Prepare(cSql);
	if( sqlrc == SQL_ERROR || sqlrc == SQL_INVALID_HANDLE) {
		mycls_pDB->GetErrMsg();
		DisConnect();					// �ؒf
		return false;
	}

	//// �������o�C���h�o�^
	// ����ȍ~�� return �̓_��
	SQLLEN *cbPhotoParam = new SQLLEN[paramCnt];
	memset( cbPhotoParam, 0x00, sizeof(SQLLEN)*paramCnt);
	// �Z�b�g
	for( ii=0; ii<paramCnt; ii++) {
		// �����ł����ƃT�C�Y���w�肵�Ȃ��ƃ_��
		// �g�[�^��30M�𒴂���悤�ȃo�C�i���f�[�^�̏ꍇ�ASQL_LEN_DATA_AT_EXEC(0); ���ꂾ�ƃG���[�ɂȂ�B
//		cbPhotoParam[ii] = (SQLLEN)SQL_LEN_DATA_AT_EXEC(size[ii]);


		// �o�C���h
		if( 0 == size[ii] ) {						// NULL�f�[�^
			sqlrc = mycls_pDB->BindParameter(ii+1, 1, &cbPhotoParam[ii]);
		} else {									// ���f�[�^
			sqlrc = mycls_pDB->BindParameter(ii+1, (SQLUINTEGER)size[ii], &cbPhotoParam[ii]);
		}
		if( sqlrc == SQL_SUCCESS || sqlrc == SQL_SUCCESS_WITH_INFO ) {
			// ����
		} else {
			goto NG;
		}


		// �p�����[�^�� 1 �̃f�[�^�͎��s���Ɏw�肷��B    
		// ���ӂƂ��āA�����ł̓p�����[�^�̒�����       
		// SQL_LEN_DATA_AT_EXEC �}�N���ŁA0 �ɂ��Ă���B
		// ����́ASQLGetInfo �� SQL_NEED_LONG_DATA_LEN ��� 
		// "N" ��Ԃ��h���C�o�ŋ������B

		//varbinary(MAX)�͂���
		cbPhotoParam[ii] = SQL_LEN_DATA_AT_EXEC(0);	

	}

	//// ���s
	sqlrc = mycls_pDB->Execute();
	if( sqlrc == SQL_ERROR || sqlrc == SQL_INVALID_HANDLE) {
		goto NG;
	}

	//// �p�����[�^�o�^
	for( ii=0; ii<paramCnt; ii++) {
		// �p�����[�^�ԍ��擾
		if( SQL_NEED_DATA == mycls_pDB->ParamData(&pToken)) {
			// �o�C���h���܂��̈�OK
		} else {
			goto NG;
		}

		// �f�[�^�Z�b�g 
		if( NULL == size[ii] ) {					// NULL�f�[�^
			sqlrc = mycls_pDB->PutData(0, 0);
		} else {									// ���f�[�^
			long	nReadSize;
			__int64 nTotalSize = 0;
			// �o�C�i���t�@�C���������� (�����Ŏ��s����͂�������)
			fp = fopen(&cFPath[ii*pathsize], "rb");
			while( nReadSize = (long)fread( data, sizeof(BYTE), BinalyFileOneWriteSize, fp ) ) {

				sqlrc =mycls_pDB->PutData(data, nReadSize);
				if (sqlrc == SQL_ERROR || sqlrc == SQL_INVALID_HANDLE) {
					goto NG;
				}
				nTotalSize += nReadSize;
			}
			fclose(fp);
		}
	}

	//// �p�����[�^�ԍ��擾
	// �����Ŏ��ۂɏ������ނ͂��B �ނ���ނ��᎞�Ԃ��������Ă���
	if( SQL_SUCCESS == mycls_pDB->ParamData(&pToken) ) {
		// ����
	} else {
		goto NG;
	}

	//// �I������
	if (NULL != cbPhotoParam) {
		delete [] cbPhotoParam; 
		cbPhotoParam = NULL;
	}
	delete size;
	delete data;
	return true;
NG:
	mycls_pDB->GetErrMsg();
	if(bErrAutoClose) DisConnect();					// �ؒf
	if (NULL != cbPhotoParam) {
		delete [] cbPhotoParam; 
		cbPhotoParam = NULL;
	}
	delete size;
	delete data;
	return false;
}


//------------------------------------------
// �f�[�^�擾
// WORD n �J�����ʒu (1�I���W��)
// LPVOID pVal �o�b�t�@�|�C���^
// DWORD size �o�b�t�@�T�C�Y
// �߂�l ���A���
//------------------------------------------
// ����
long OdbcBase::GetData(WORD n, LPVOID pVal, DWORD size)
{
	if( ! mycls_pDB->IsConnect()) return -1;			// ���ڑ�
	return mycls_pDB->GetData(n, pVal, size);
}
// ����
long OdbcBase::GetData(WORD n, short *pVal)
{
	if( ! mycls_pDB->IsConnect()) return -1;			// ���ڑ�
	return mycls_pDB->GetData(n, pVal);
}
long OdbcBase::GetData(WORD n, int *pVal)
{
	if( ! mycls_pDB->IsConnect()) return -1;			// ���ڑ�
	return mycls_pDB->GetData(n, pVal);
}
long OdbcBase::GetData(WORD n, long *pVal)
{
	if( ! mycls_pDB->IsConnect()) return -1;			// ���ڑ�
	return mycls_pDB->GetData(n, pVal);
}
// ����
long OdbcBase::GetData(WORD n, float *pVal)
{
	if( ! mycls_pDB->IsConnect()) return -1;			// ���ڑ�
	return mycls_pDB->GetData(n, pVal);
}
long OdbcBase::GetData(WORD n, double *pVal)
{
	if( ! mycls_pDB->IsConnect()) return -1;			// ���ڑ�
	return mycls_pDB->GetData(n, pVal);
}

//------------------------------------------
// ���t�^
// WORD n �J�����ʒu (0�I���W��)
//------------------------------------------
// int [3]
long OdbcBase::GetDataDate(WORD n, int *pVal)
{
	if( ! mycls_pDB->IsConnect()) return -1;			// ���ڑ�
	return mycls_pDB->GetDataDate(n, pVal);
}
// int [6]
long OdbcBase::GetDataDateTime(WORD n, int *pVal)
{
	if( ! mycls_pDB->IsConnect()) return -1;			// ���ڑ�
	return mycls_pDB->GetDataDateTime(n, pVal);
}

//------------------------------------------
// �o�C�i���f�[�^�擾
// WORD n �J�����ʒu (0�I���W��)
// LPVOID pVal �o�b�t�@�|�C���^
// DWORD size �o�b�t�@�T�C�Y
// �߂�l �擾�o�C�g��
//------------------------------------------
long OdbcBase::GetDataImage(WORD n, LPVOID pVal, DWORD size )
{
	if( ! mycls_pDB->IsConnect()) return -1;			// ���ڑ�
	return mycls_pDB->GetDataImage(n, pVal, size);
}



// //////////////////////////////////////////////////////////////////////////////
// SQL�� ��������֘A
// //////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// insert
// LPSTR pSql �ҏW����SQL��
// int bufsize �ҏW����SQL���̃o�b�t�@
// const char *val �ǉ����镶����
// int size ������̒��� (0�̏ꍇ�ANULL�������Z�b�g)
// �߂�l �擾�o�C�g��
//------------------------------------------
// ������
int OdbcBase::sqlColumnInsert(LPSTR pSql, const char *val, int size)
{
	// �f�[�^��NULL
	if( NULL == val || 0 == size ) {
		//return sprintf(pSql+strlen(pSql), bufsize-strlen(pSql), "NULL," );
		return sprintf(pSql+strlen(pSql), "NULL," );
	}
	
	// ���[�N�Z�b�g
	int iwk = size+8;
	char *cpwk = new char[iwk];
	memset(cpwk, 0x00, iwk);
	memcpy(cpwk, val, size);

	// �f�[�^�Z�b�g
	int len;
	//len = sprintf_s( pSql+strlen(pSql), bufsize-strlen(pSql), "'%s',", cpwk );
	len = sprintf( pSql+strlen(pSql), "'%s',", cpwk );
	delete [] cpwk;
	return len;
}
// ����
int OdbcBase::sqlColumnInsert(LPSTR pSql, short val)
{
	//return sprintf_s(pSql+strlen(pSql), bufsize-strlen(pSql), "%d,", val);
	return sprintf(pSql+strlen(pSql), "%d,", val);
}
int OdbcBase::sqlColumnInsert(LPSTR pSql, int val)
{
	//return sprintf_s(pSql+strlen(pSql), bufsize-strlen(pSql), "%d,", val);
	return sprintf(pSql+strlen(pSql), "%d,", val);
}
int OdbcBase::sqlColumnInsert(LPSTR pSql, long val)
{
	//return sprintf_s(pSql+strlen(pSql), bufsize-strlen(pSql), "%d,", val);
	return sprintf(pSql+strlen(pSql), "%d,", val);
}
int OdbcBase::sqlColumnInsert(LPSTR pSql, DWORD val)
{
	//return sprintf_s(pSql+strlen(pSql), bufsize-strlen(pSql), "%d,", val);
	return sprintf(pSql+strlen(pSql), "%d,", val);
}
// ����
int OdbcBase::sqlColumnInsert(LPSTR pSql, float val)
{
	//return sprintf_s(pSql+strlen(pSql), bufsize-strlen(pSql), "%0.6f,", val);
	return sprintf(pSql+strlen(pSql), "%0.6f,", val);
}
int OdbcBase::sqlColumnInsert(LPSTR pSql, double val)
{
	//return sprintf_s(pSql+strlen(pSql), bufsize-strlen(pSql), "%0.15lf,", val);
	return sprintf(pSql+strlen(pSql), "%0.15lf,", val);
}


//------------------------------------------
// update
// LPSTR pSql �ҏW����SQL��
// const char *name ���ږ���
// const char *val �ǉ����镶����
// int size ������̒��� (0�̏ꍇ�ANULL�������Z�b�g)
// �߂�l �擾�o�C�g��
//------------------------------------------
// ������
int OdbcBase::sqlColumnUpdate(LPSTR pSql, const char *name, const char *val, int size)
{
	// �f�[�^��NULL
	if( NULL == val || 0 == size ) {
		//return sprintf_s(pSql+strlen(pSql), bufsize-strlen(pSql), "%s=NULL,", name );
		return sprintf(pSql+strlen(pSql), "%s=NULL,", name );
	}
	
	// ���[�N�Z�b�g
	int iwk = size+8;
	char *cpwk = new char[iwk];
	memset(cpwk, 0x00, iwk);
	memcpy(cpwk, val, size);

	// �f�[�^�Z�b�g
	int len;
	//len = sprintf_s(pSql+strlen(pSql), bufsize-strlen(pSql), "%s='%s',", name, cpwk );
	len = sprintf(pSql+strlen(pSql), "%s='%s',", name, cpwk );
	delete [] cpwk;
	return len;
}
// ����
int OdbcBase::sqlColumnUpdate(LPSTR pSql, const char *name, int val)
{
	return sprintf(pSql+strlen(pSql), "%s=%d,", name, val);
}
int OdbcBase::sqlColumnUpdate(LPSTR pSql, const char *name, long val)
{
	return sprintf(pSql+strlen(pSql), "%s=%d,", name, val);
}
// ����
int OdbcBase::sqlColumnUpdate(LPSTR pSql, const char *name, float val)
{
	return sprintf(pSql+strlen(pSql), "%s=%0.6f,", name, val);
}
int OdbcBase::sqlColumnUpdate(LPSTR pSql, const char *name, double val)
{
	return sprintf(pSql+strlen(pSql), "%s=%0.15lf,", name, val);
}


//------------------------------------------
// SQL���̍Ō�� "," ����菜��
// LPSTR pSql �ҏW����SQL��
//------------------------------------------
int OdbcBase::sqlColumnTrim(LPSTR pSql)
{
	int len = (int)strlen(pSql);
	if( ',' == pSql[len-1] ) {
		pSql[len-1] = NULL;
		return len-1;
	}
	return len;
}
//------------------------------------------
// SQL���̓o�^���t�𐶐� (������)
// CString *cSql �ҏW����SQL��
//------------------------------------------
int OdbcBase::sqlColumnAddTime(LPSTR pSql)
{
	SYSTEMTIME time;
    GetLocalTime(&time);

//	return sprintf_s(pSql+strlen(pSql), bufsize-strlen(pSql), "'%d/%d/%d %d:%d:%d'", 
//				time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

	return sprintf(pSql+strlen(pSql), "'%d/%d/%d %d:%d:%d'", 
				time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
}

// //////////////////////////////////////////////////////////////////////////////
// �W���I��Select���̎��s�擾
// //////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// �P��s1���ڂ�int�f�[�^�̂ݎ擾 (������count(*)��SQL�����s�Ɏg�p)
// char const *cSql ���s����SQL��
// int size �o�b�t�@�T�C�Y
// char *pVal �擾�f�[�^
// �߂�l ���A���
//------------------------------------------
bool OdbcBase::GetSelectKey(char const *cSql, int size, char *pVal)
{
	if( ! mycls_pDB->IsConnect()) return false;			// ���ڑ�

	// SQL���s
	if(! this->ExecuteDirect( cSql ) ){
		this->DisConnect();
		return false;
	}

	// �t�F�b�`
	int sqlrc = this->Fetch();	
	if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��	(�ؒf���Ȃ�)
		this->GetData(1, pVal, size);
		return true;
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����	(�ؒf���Ȃ�)
		*pVal = 0;
		return false;					
	} else {											// �G���[		(�ؒf�����)
		*pVal = 0;
		return false;
	}
}
bool OdbcBase::GetSelectKey(char const *cSql, int *pVal)
{
	if( ! mycls_pDB->IsConnect()) return false;			// ���ڑ�
	// SQL���s
	if(! this->ExecuteDirect( cSql ) ){
		this->DisConnect();
		return false;
	}

	// �t�F�b�`
	int sqlrc = this->Fetch();	
	if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��	(�ؒf���Ȃ�)
		this->GetData(1, pVal);
		return true;
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����	(�ؒf���Ȃ�)
		*pVal = 0;
		return false;					
	} else {											// �G���[		(�ؒf�����)
		*pVal = 0;
		return false;
	}
}
bool OdbcBase::GetSelectKey(char const *cSql, float *pVal)
{
	if( ! mycls_pDB->IsConnect()) return false;			// ���ڑ�
	// SQL���s
	if(! this->ExecuteDirect( cSql ) ){
		this->DisConnect();
		return false;
	}

	// �t�F�b�`
	int sqlrc = this->Fetch();	
	if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��	(�ؒf���Ȃ�)
		this->GetData(1, pVal);
		return true;
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����	(�ؒf���Ȃ�)
		*pVal = 0;
		return false;					
	} else {											// �G���[		(�ؒf�����)
		*pVal = 0;
		return false;
	}
}
bool OdbcBase::GetSelectKey(char const *cSql, double *pVal)
{
	if( ! mycls_pDB->IsConnect()) return false;			// ���ڑ�
	// SQL���s
	if(! this->ExecuteDirect( cSql ) ){
		this->DisConnect();
		return false;
	}

	// �t�F�b�`
	int sqlrc = this->Fetch();	
	if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��	(�ؒf���Ȃ�)
		this->GetData(1, pVal);
		return true;
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����	(�ؒf���Ȃ�)
		*pVal = 0;
		return false;					
	} else {											// �G���[		(�ؒf�����)
		*pVal = 0;
		return false;
	}
}
// ���t�擾 int[6]
bool OdbcBase::GetSelectDateTime(char const *cSql, int *pVal)
{
	if( ! mycls_pDB->IsConnect()) return false;			// ���ڑ�
	// SQL���s
	if(! this->ExecuteDirect( cSql ) ){
		this->DisConnect();
		return false;
	}

	// �t�F�b�`
	int sqlrc = this->Fetch();	
	if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��	(�ؒf���Ȃ�)
		this->GetDataDateTime(1, pVal);
		return true;
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����	(�ؒf���Ȃ�)
		return false;					
	} else {											// �G���[		(�ؒf�����)
		return false;
	}
}
// �o�C�i���f�[�^
bool OdbcBase::GetSelectImage(char const *cSql, int size, char *pVal)
{
	if( ! mycls_pDB->IsConnect()) return false;			// ���ڑ�
	// SQL���s
	if(! this->ExecuteDirect( cSql ) ){
		this->DisConnect();
		return false;
	}

	// �t�F�b�`
	int sqlrc = this->Fetch();	
	if (sqlrc == KIZUODBC_FETCH_OK) {					// �f�[�^�L��	(�ؒf���Ȃ�)
		this->GetDataImage(1, pVal, size);
		return true;
	} else if( sqlrc == KIZUODBC_FETCH_NODATE ) {		// �f�[�^����	(�ؒf���Ȃ�)
		*pVal = 0;
		return false;					
	} else {											// �G���[		(�ؒf�����)
		*pVal = 0;
		return false;
	}
}


//------------------------------------------
// �P��s1���ڂ�int�f�[�^�̂ݎ擾 (������count(*)��SQL�����s�Ɏg�p)
// char const *cSql ���s����SQL��
// int size �o�b�t�@�T�C�Y
// char *pVal �擾�f�[�^
// LPCSTR cIniPath ini�t�@�C���p�X
// LPCSTR cSession �Z�b�V����
// �߂�l ���A���
//------------------------------------------
bool OdbcBase::GetSelectKey_Static(char const *cSql, int size, char *pVal, LPCSTR cIniPath, LPCSTR cSession)
{
	OdbcBase cls_DB;
	if( ! cls_DB.Connect(cIniPath, cSession) ) return false;
	return cls_DB.GetSelectKey(cSql, size, pVal);
}

bool OdbcBase::GetSelectKey_Static(char const *cSql, int *pVal, LPCSTR cIniPath, LPCSTR cSession)
{
	OdbcBase cls_DB;
	if( ! cls_DB.Connect(cIniPath, cSession) ) return false;
	return cls_DB.GetSelectKey(cSql, pVal);
}
bool OdbcBase::GetSelectKey_Static(char const *cSql, float *pVal, LPCSTR cIniPath, LPCSTR cSession)
{
	OdbcBase cls_DB;
	if( ! cls_DB.Connect(cIniPath, cSession) ) return false;
	return cls_DB.GetSelectKey(cSql, pVal);
}
bool OdbcBase::GetSelectKey_Static(char const *cSql, double *pVal, LPCSTR cIniPath, LPCSTR cSession)
{
	OdbcBase cls_DB;
	if( ! cls_DB.Connect(cIniPath, cSession) ) return false;
	return cls_DB.GetSelectKey(cSql, pVal);
}
bool OdbcBase::GetSelectDateTime_Static(char const *cSql, int *pVal, LPCSTR cIniPath, LPCSTR cSession)
{
	OdbcBase cls_DB;
	if( ! cls_DB.Connect(cIniPath, cSession) ) return false;
	return cls_DB.GetSelectDateTime(cSql, pVal);
}
bool OdbcBase::GetSelectImage_Static(char const *cSql, int size, char *pVal, LPCSTR cIniPath, LPCSTR cSession)
{
	OdbcBase cls_DB;
	if( ! cls_DB.Connect(cIniPath, cSession) ) return false;
	return cls_DB.GetSelectImage(cSql, size, pVal);
}


// //////////////////////////////////////////////////////////////////////////////
// �֗�SQL
// //////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// �����N�T�[�o�[��ǉ�
// bool bDelete true:�����̃����N�T�[�o�[����x����
// char const *cLinkName �����N�T�[�o�[��
// char const* addr �f�[�^�x�[�X�A�h���X
// char const* dsn  �f�[�^�x�[�X��
// char const* uid  ���[�U�[��
// char const* pwd  �p�X���[�h
// bool bDelete  true:�����̃����N�T�[�o�[����x����  (�f�t�H���g�Ftrue)
// char const* srvproduct ���i�� (�f�t�H���g:mySQLServer)
// char const* provider �v���o�C�_ (�f�t�H���g:SQLNCLI) �� GUI�̃v���o�C�_�l�����̂܂܃Z�b�g������NG�������B��p�̕����񂪗L�肻���Ȃ̂Œ���
// �߂�l ���A���
//------------------------------------------
bool OdbcBase::ExecuteDirect_AddLinkServer(char const *cLinkName, char const* addr, char const* dsn, char const* uid, char const* pwd, bool bDelete, char const* srvproduct, char const* provider)
{
	if( ! mycls_pDB->IsConnect()) return false;			// ���ڑ�

	char cSql[256];

	// �����̃����N�T�[�o�[������΍폜
	if( bDelete ) {
		memset(cSql, 0x00, sizeof(cSql));
		sprintf(cSql, "sp_dropserver '%s', droplogins", cLinkName);

		if(! this->ExecuteDirect( cSql, false ) ){
			// �����������ꍇ�̓G���[�ƂȂ邪�A�ʂɖ��Ȃ��ׁA�������Ȃ�
			//this->DisConnect();
			//return false;
		}
	}

	// �����N�T�[�o�[�ǉ�
	memset(cSql, 0x00, sizeof(cSql));
	sprintf(cSql, "sp_addlinkedserver @server='%s',@srvproduct='%s',@provider='%s',@datasrc='%s',@catalog='%s'",
		cLinkName, srvproduct, provider, addr, dsn);

	if(! this->ExecuteDirect( cSql ) ){
		this->DisConnect();
		return false;
	}

	// ���O�C�����[�U�[�̐ݒ�
	memset(cSql, 0x00, sizeof(cSql));
	sprintf(cSql, "sp_addlinkedsrvlogin @rmtsrvname='%s',@useself=false,@rmtuser='%s',@rmtpassword='%s'",
		cLinkName, uid, pwd);

	if(! this->ExecuteDirect( cSql ) ){
		this->DisConnect();
		return false;
	}

	return true;
}

// //////////////////////////////////////////////////////////////////////////////
// ���S�ȕ⏕�@�\�֘A
// //////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// ini�t�@�C���̃Z�b�V���������ꂩ����
// LPCSTR cIniPath1 ini�t�@�C���p�X
// LPCSTR cSession1 �Z�b�V����
// LPCSTR cIniPath2 ini�t�@�C���p�X
// LPCSTR cSession2 �Z�b�V����
// �߂�l ���A���
//------------------------------------------
bool OdbcBase::CheckIniSession(LPCSTR cIniPath1, LPCSTR cSession1, LPCSTR cIniPath2, LPCSTR cSession2)
{
	//  C�Ŏg�p���鍀�ڂ݂̂Ŕ���
//	char wk_addr[2][128];
	char wk_odbc[2][128];
//	char wk_dsna[2][128];
	char wk_user[2][128];
	char wk_pass[2][128];

//	GetPrivateProfileString(cSession1, "DBADDLESS", "", (char *)wk_addr[0], sizeof(wk_addr), cIniPath1);
	GetPrivateProfileString(cSession1, "ODBCNAME",  "", (char *)wk_odbc[0], sizeof(wk_odbc), cIniPath1);
//	GetPrivateProfileString(cSession1, "DSNAME",    "", (char *)wk_dsna[0], sizeof(wk_dsna), cIniPath1);
	GetPrivateProfileString(cSession1, "USERID",    "", (char *)wk_user[0], sizeof(wk_user), cIniPath1);
	GetPrivateProfileString(cSession1, "PASSWORD",  "", (char *)wk_pass[0], sizeof(wk_pass), cIniPath1);

//	GetPrivateProfileString(cSession2, "DBADDLESS", "", (char *)wk_addr[1], sizeof(wk_addr), cIniPath2);
	GetPrivateProfileString(cSession2, "ODBCNAME",  "", (char *)wk_odbc[1], sizeof(wk_odbc), cIniPath2);
//	GetPrivateProfileString(cSession2, "DSNAME",    "", (char *)wk_dsna[1], sizeof(wk_dsna), cIniPath2);
	GetPrivateProfileString(cSession2, "USERID",    "", (char *)wk_user[1], sizeof(wk_user), cIniPath2);
	GetPrivateProfileString(cSession2, "PASSWORD",  "", (char *)wk_pass[1], sizeof(wk_pass), cIniPath2);

	if(
//		0 ==strcmp(wk_addr[0], wk_addr[1]) &&
		0 ==strcmp(wk_odbc[0], wk_odbc[1]) &&
//		0 ==strcmp(wk_dsna[0], wk_dsna[1]) &&
		0 ==strcmp(wk_user[0], wk_user[1]) &&
		0 ==strcmp(wk_pass[0], wk_pass[1]) 
		) return true;
	else  return false;
}

// //////////////////////////////////////////////////////////////////////////////
// �񓯊�SQL���s
// //////////////////////////////////////////////////////////////////////////////

//------------------------------------------
// �񓯊����s
// LPCSTR cSql SQL��
// bool bFast  true:������s  false:2��ڈȍ~
// bool bErrAutoClose �G���[�������ؒf�@�\ (�f�t�H���g true)
// �߂�l�F-1:�ُ�  0:����  1:SQL���s��
//------------------------------------------
int OdbcBase::ExecuteDirectAsync(LPCSTR cSql, bool bFast, bool bErrAutoClose)
{
	if( ! mycls_pDB->IsConnect()) return false;			// ���ڑ�
	SQLRETURN sqlrc;

	//// ���s
	sqlrc = mycls_pDB->ExecuteDirectAsync(cSql, bFast);

	//// ���ʃ`�F�b�N
	if( sqlrc == SQL_ERROR || sqlrc == SQL_INVALID_HANDLE) {
		mycls_pDB->GetErrMsg();
		if(bErrAutoClose) DisConnect();			// �ؒf
		return -1;
	} else if( sqlrc == SQL_STILL_EXECUTING ) {
		return 1;
	} else {
		return 0;
	}
}
//------------------------------------------
// �񓯊����s�L�����Z��
// �߂�l�F���A���
//------------------------------------------
bool OdbcBase::ExecuteDirectAsyncCancel()
{
	if( ! mycls_pDB->IsConnect()) return false;			// ���ڑ�
	SQLRETURN sqlrc;
	//// ���s
	sqlrc = mycls_pDB->ExecuteDirectAsyncCancel();
	if( sqlrc == SQL_ERROR || sqlrc == SQL_INVALID_HANDLE) {
		mycls_pDB->GetErrMsg();
		return false;
	} else {
		return true;
	}
}
