#include "StdAfx.h"
#include "BCPBase.h"

using namespace KizuODBC;

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
BCPBase::BCPBase(void) : 
m_hdbc(NULL),
mycls_pDB(NULL)
{
	memset(m_cTableName, 0x00, sizeof(m_cTableName) );

	// ���� (�ڑ��n���h���܂Ő���)
	mycls_pDB = new BCPManager();
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
BCPBase::~BCPBase(void)
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
// �Z�b�V�����w��ł̐ڑ�
// LPCSTR cIniPath ini�t�@�C���p�X
// LPCSTR cSession �Z�b�V����
// �߂�l ���A���
//------------------------------------------
bool BCPBase::Connect(LPCSTR cIniPath, LPCSTR cSession)
{
	if( NULL==mycls_pDB )				return false;
	if( ! mycls_pDB->IsCreateHandle() )	return false;		// �n���h������NG
	if( mycls_pDB->IsConnect() )		return true;		// �ڑ��ς�

	int retc;
	if(NULL == cSession) retc = mycls_pDB->Connect(cIniPath);
	else				 retc = mycls_pDB->Connect(cIniPath, cSession);

	// ��{���Z�b�g
	m_hdbc = mycls_pDB->GetHDBC();				// BCP�n�ŕK�v�Ȉ� �R�l�N�g�������ɕێ����Ă���
	
	if( 0 == retc ) {					// �V�K�ڑ�
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
void BCPBase::DisConnect()
{
	// �ؒf
	if(NULL != mycls_pDB) {
		mycls_pDB->DisConnect(); 
	}
}


// //////////////////////////////////////////////////////////////////////////////
// BCP�n
// //////////////////////////////////////////////////////////////////////////////

//------------------------------------------
// BCP�n ������
// LPCSTR cTableName �ڑ��e�[�u�� (�����ɂ́A�uODBC�̖���..DB���́v�����A[DB����]�݂̂ł�OK)
// �߂�l ���A���
//
// �� bcp_init �́A����e�[�u���ɑ΂��A�����������_���B(= �������ސl����������NG)
//------------------------------------------
bool BCPBase::BCP_Init()
{
	return BCP_Init( (LPCSTR)mycls_pDB->GetDSN() );
}
bool BCPBase::BCP_Init(LPCSTR cTableName)
{
	// �e�[�u�����i�[
	strcpy(m_cTableName, cTableName);

	if( SUCCEED == bcp_init(m_hdbc, cTableName, NULL, NULL, DB_IN ) ) {
//	if( SUCCEED == bcp_init(m_hdbc, "test_tbl", NULL, NULL, DB_IN ) ) {
		return true;
	} else {
		return false;
	}
}
//------------------------------------------
// BCP�n �o�C���h (�l�^)
// int coulmn �J�����ʒu (1�I���W��)
// BYTE* pData �o�C���h����擪�A�h���X (���̃A�h���X�ɃZ�b�g����Ă���f�[�^�����̂܂܂c�a�֓]�������)
// emBcpBindType typ �o�C���h����^
// �߂�l ���A���
// -------------------------------
// �Ăь��ŁA�o�C���h����A�h���X���Œ肵�Ă�����
//------------------------------------------
bool BCPBase::BCP_Bind_Value(int coulmn, BYTE* pData, emBcpBindType typ)
{
	if( SUCCEED == bcp_bind(m_hdbc, pData, 0, SQL_VARLEN_DATA, NULL, 0, typ, coulmn) ) {
		return true;
	} else {
		return false;
	}
}

//------------------------------------------
// BCP�n �o�C���h (�o�C�i���z��^)
// int coulmn �J�����ʒu (1�I���W��)
// typBindData* pData �o�C���h����o�C�i���f�[�^�̐擪�A�h���X (�f�[�^�T�C�Y�Ǝ��f�[�^�͘A�����Ă����K�v������B)
// emBcpBindType typ �o�C���h����^ (em_Binary,em_VerChar���قڌŒ�)
// �߂�l ���A���
// -------------------------------
// �Ăь��ŁA�o�C���h����A�h���X���Œ肵�Ă�����
//------------------------------------------
bool BCPBase::BCP_Bind_Binary(int coulmn, typBindData* pData, emBcpBindType typ)
{
	if( SUCCEED == bcp_bind(m_hdbc, (BYTE*)pData, 4, SQL_VARLEN_DATA, NULL, 0, typ, coulmn) ) {
//	if( SUCCEED == bcp_bind(m_hdbc, (BYTE*)pData, 4, SQL_NULL_DATA, NULL, 0, typ, coulmn) ) {
		return true;
	} else {
		return false;
	}
}
bool BCPBase::BCP_Bind_Binary(int coulmn, typBindDataEx* pData, emBcpBindType typ)
{
	if( SUCCEED == bcp_bind(m_hdbc, pData->GetBindPointer(), 8, SQL_VARLEN_DATA, NULL, 0, typ, coulmn) ) {
		return true;
	} else {
		return false;
	}
}

// ���ړo�^�B�擪����8�o�C�g���f�[�^�T�C�Y�ȘA���f�[�^�B�g���Ƃ��ɖ���ăo�C���h����K�v�L��
bool BCPBase::BCP_Bind_VerBinary(int coulmn, BYTE* pData)
{
	if( SUCCEED == bcp_bind(m_hdbc, pData, 8, SQL_VARLEN_DATA, NULL, 0, SQLVARBINARY, coulmn) ) {
	//if( SUCCEED == bcp_bind(m_hdbc, pData, 8, SQL_VARLEN_DATA, NULL, 0, SQLBIGVARBINARY, coulmn) ) {
	//if( SUCCEED == bcp_bind(m_hdbc, pData, 4, SQL_VARLEN_DATA, NULL, 0, SQLVARBINARY, coulmn) ) {
		return true;
	} else {
		return false;
	}
}

////// BCP��NULL�f�[�^�̃Z�b�g���@���킩��Ȃ��� �b��
// pData�̐擪4�o�C�g��FF�Ŗ��߂��NULL���o�^����邱�Ƃ𔭌��B
//	�������Ɖe�����傫�����߁A�ꉞ���̂܂܎b��Ƃ��Ďc���Ă����B
bool BCPBase::BCP_Bind_BinaryNull(int coulmn, typBindData* pData, emBcpBindType typ)
{
//	if( SUCCEED == bcp_bind(m_hdbc, (BYTE*)pData, 4, SQL_VARLEN_DATA, NULL, 0, typ, coulmn) ) {
	if( SUCCEED == bcp_bind(m_hdbc, (BYTE*)pData, 4, SQL_NULL_DATA, NULL, 0, typ, coulmn) ) {
		return true;
	} else {
		return false;
	}
}

//------------------------------------------
// BCP�n �o�C���h (������^ �I�[NULL�K�{)
// int coulmn �J�����ʒu (1�I���W��)
// BYTE* pData �o�C���h����擪�A�h���X (�I�[��NULL�K�{)
// �߂�l ���A���
// -------------------------------
// �Ăь��ŁA�o�C���h����A�h���X���Œ肵�Ă�����
//------------------------------------------
bool BCPBase::BCP_Bind_String(int coulmn, char* pData)
{
	if( SUCCEED == bcp_bind(m_hdbc, (BYTE*)pData, 0, SQL_VARLEN_DATA, (UCHAR*)"", 1, SQLCHARACTER, coulmn) ) {
		return true;
	} else {
		return false;
	}
}

//------------------------------------------
// BCP�n DB�փf�[�^�]��
// �߂�l ���A���
//------------------------------------------
bool BCPBase::BCP_SendRow()
{
	RETCODE rec = bcp_sendrow(m_hdbc);
	if( SUCCEED == rec ) {
		return true;
	} else {
		return false;
	}
}
//------------------------------------------
// BCP�n �f�[�^�]�������f�[�^�̃R�~�b�g�݂����ȓz
// �߂�l �R�~�b�g��������
//------------------------------------------
int BCPBase::BCP_Batch()
{
	return bcp_batch(m_hdbc);
}

//------------------------------------------
// BCP�n �J�� (Batch����ĂȂ��z�������Batch�����H)
// �߂�l ���A���
//------------------------------------------
bool BCPBase::BCP_Down()
{
	// �{���́A�ۑ����ꂽ������Ԃ��B�܂��A-1�Ȃ�ُ�
	if( -1 != bcp_done(m_hdbc) ) {
		return true;
	} else {
		return false;
	}
}


//------------------------------------------
// BCP�n ���t�o�^�����擾
// char* cTime �Z�b�g������t�G���A (20byte�ȏ�K�v)
//------------------------------------------
void BCPBase::Get_BPC_Time(char* cTime)
{
	SYSTEMTIME time;
    GetLocalTime(&time);

	sprintf(cTime, "%04d-%02d-%02d %02d:%02d:%02d", 
				time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
}

//------------------------------------------
// BCP�n �o�C���h�̃o�C�i���f�[�^�T�C�Y�w�� ()
// int coulmn �e�[�u���̃J�����ʒu (1�I���W��)
// long nSize �T�C�Y
// �߂�l ���A���
// -------------------------------
// varbinary(MAX) ���� 0xFFFF �ȏ�̃T�C�Y�����܂����Ɠo�^�ł��Ȃ��b��Ή���
//------------------------------------------
bool BCPBase::BCP_ColLen(int coulmn, long nSize)
{
	if( 0xFFFF >= nSize ) {
		// �K�v�����B�Ƃ������A�w�肵�Ă����f����Ȃ��B�C���W�P�[�^���D�悳���
		return true;
	} else {
		// 0xFFFF �ȏゾ�� �C���W�P�[�^���D�悳��Ȃ��B�Ȃ��H
		RETCODE rec = bcp_collen(m_hdbc, nSize, coulmn);
		if( SUCCEED == rec ) {
			return true;
		} else {
			return false;
		}
	}
}


