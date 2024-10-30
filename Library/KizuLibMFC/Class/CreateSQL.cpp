#include "StdAfx.h"
#include "CreateSQL.h"

using namespace KizuLib;

CreateSQL::CreateSQL(void)
{
}

CreateSQL::~CreateSQL(void)
{
}

//------------------------------------------
// insert
// CString *cSql �ҏW����SQL��
// const char *val �ǉ����镶����
// int size ������̒��� (0�̏ꍇ�ANULL�������Z�b�g)
// �߂�l �擾�o�C�g��
//------------------------------------------
// ������
void CreateSQL::sqlColumnInsert(CString *cSql, const char *val, int size)
{
	CString str;
	CString wk(val, size);		// �w�蕶�����Z�b�g�R���X�g���N�^
	if( 0 == strlen(wk) ) str = "NULL,";
	else str.Format("'%s',", wk);
	*cSql += str;				// �ǉ�
	return;
}
// ����
void CreateSQL::sqlColumnInsert(CString *cSql, short val)
{
	char str[256];
	sprintf(str, "%d,", val);
	*cSql += str;				// �ǉ�
}
void CreateSQL::sqlColumnInsert(CString *cSql, int val)
{
	char str[256];
	sprintf(str, "%d,", val);
	*cSql += str;				// �ǉ�
}
void CreateSQL::sqlColumnInsert(CString *cSql, long val)
{
	char str[256];
	sprintf(str, "%d,", val);
	*cSql += str;				// �ǉ�
}
void CreateSQL::sqlColumnInsert(CString *cSql, DWORD val)
{
	char str[256];
	sprintf(str, "%d,", val);
	*cSql += str;				// �ǉ�
}
// ����
void CreateSQL::sqlColumnInsert(CString *cSql, float val)
{
	char str[256];
	sprintf(str, "%0.6f,", val);
	*cSql += str;				// �ǉ�
}
void CreateSQL::sqlColumnInsert(CString *cSql, double val)
{
	char str[256*2];
	sprintf(str, "%0.15lf,", val);
	*cSql += str;				// �ǉ�
}

// ������
void CreateSQL::sqlColumnInsert(CString *cSql, const char *val)
{
	*cSql += "'";	
	*cSql += val;
	*cSql += "',";
}
void CreateSQL::sqlColumnInsert(CString *cSql, CString val)
{
	*cSql += "'";	
	*cSql += val;
	*cSql += "',";
	return;
}

//------------------------------------------
// update
// CString *cSql �ҏW����SQL��
// const char *name ���ږ���
// const char *val �ǉ����镶����
// int size ������̒��� (0�̏ꍇ�ANULL�������Z�b�g)
// �߂�l �擾�o�C�g��
//------------------------------------------

// ������
void CreateSQL::sqlColumnUpdate(CString *cSql, const char *name, const char *val, int size)
{
	CString str;
	CString wk(val, size);		// �w�蕶�����Z�b�g�R���X�g���N�^

	// NULL���`�F�b�N
	if( 0 == strlen(wk)) str.Format("%s=NULL,", name);
	else str.Format("%s='%s',", name, wk);
	*cSql += str;				// �ǉ�
}
// ����
void CreateSQL::sqlColumnUpdate(CString *cSql, const char *name, short val)
{
	char str[256];
	sprintf(str, "%s=%d,", name, val);
	*cSql += str;				// �ǉ�
}

void CreateSQL::sqlColumnUpdate(CString *cSql, const char *name, int val)
{
	char str[256];
	sprintf(str, "%s=%d,", name, val);
	*cSql += str;				// �ǉ�
}
void CreateSQL::sqlColumnUpdate(CString *cSql, const char *name, long val)
{
	char str[256];
	sprintf(str, "%s=%d,", name, val);
	*cSql += str;				// �ǉ�
}
void CreateSQL::sqlColumnUpdate(CString *cSql, const char *name, DWORD val)
{
	char str[256];
	sprintf(str, "%s=%d,", name, val);
	*cSql += str;				// �ǉ�
}
// ����
void CreateSQL::sqlColumnUpdate(CString *cSql, const char *name, float val)
{
	char str[256];
	sprintf(str, "%s=%0.6f,", name, val);
	*cSql += str;				// �ǉ�
}
void CreateSQL::sqlColumnUpdate(CString *cSql, const char *name, double val)
{
	char str[256];
	sprintf(str, "%s=%0.15lf,", name, val);
	*cSql += str;				// �ǉ�
}
// ������
void CreateSQL::sqlColumnUpdate(CString *cSql, const char *name, CString val)
{
	CString str;
	str.Format("%s='%s',", name, val);
	*cSql += str;				// �ǉ�
}

//------------------------------------------
// sqlColumnInsert,sqlColumnUpdate�ōŌ�ɂ��� ","���������
// CString *cSql �ҏW����SQL��
//------------------------------------------
void CreateSQL::sqlColumnTrim(CString *cSql)
{
	if(cSql->Right(1) == ",") 
		cSql->Delete(cSql->GetLength()-1);	// �Ō��","���폜
}

//------------------------------------------
// SQL���̓o�^���t�𐶐� (������)
// CString *cSql �ҏW����SQL��
// bool bAddCanma �Ō�̃J���}��t�^���邵�Ȃ�
//------------------------------------------
void CreateSQL::sqlColumnAddTime(CString *cSql, bool bAddCanma)
{
	CTime t = CTime::GetCurrentTime();
	CString str = t.Format("'%Y/%m/%d %H:%M:%S'");
	if(bAddCanma) str += ",";

	*cSql += str;				// �ǉ� 
}

//------------------------------------------
// SQL���̓o�^���t�𐶐� (������)
// CString *cSql �ҏW����SQL��
// CTime t �Z�b�g���鎞��
//------------------------------------------
void CreateSQL::sqlColumnAddTime(CString *cSql, CTime t, bool bAddCanma)
{
	CString str = t.Format("'%Y/%m/%d %H:%M:%S'");
	if(bAddCanma) str += ",";
	*cSql += str;				// �ǉ� 
}

//------------------------------------------
// ������[yyyymmddhhnnss]�`������[yyyy/mm/dd hh:nn:ss]�`���ɕϊ�
// char const* pWk [yyyymmddhhnnss]�`���̕�����
//------------------------------------------
CString CreateSQL::StringToTime(char const* pWk)
{
	CString		wk;
	char cTime[20];
	memset(cTime, 0x00, sizeof(cTime));

	// yyyy/mm/dd hh:nn:ss
	// 0123456789012345678
	// yyyymmddhhnnss
	memcpy(&cTime[0], &pWk[0], 4);
	cTime[4] = '/';
	memcpy(&cTime[5], &pWk[4], 2);
	cTime[7] = '/';
	memcpy(&cTime[8], &pWk[6], 2);
	cTime[10] = ' ';
	memcpy(&cTime[11], &pWk[8], 2);
	cTime[13] = ':';
	memcpy(&cTime[14], &pWk[10], 2);
	cTime[16] = ':';
	memcpy(&cTime[17], &pWk[12], 2);
	
	wk = cTime;
	return wk;
}

//------------------------------------------
// �����񂩂�O��̃X�y�[�X����菜��
// CString *cSql �ҏW����SQL��
//------------------------------------------
CString CreateSQL::StringTrim(const char *val, int size)
{
	int	nSize = (int)strlen(val);
	if(size > nSize) size = nSize;
	CString	wk(val, size);		// �ǂ���� size�����ۂ̕����񂪏��Ȃ��ꍇ�A�c��̃G���A��NULL�łȂ��ւ�ȃf�[�^���t�����ꂻ��
	return wk.Trim();
}