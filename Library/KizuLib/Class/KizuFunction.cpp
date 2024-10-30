#include "StdAfx.h"
#include "KizuFunction.h"

#include <stdio.h>
#include <fstream>	
#include <direct.h>
#include <math.h>

#include "..\..\..\Include\KizuCommon.h"							// �r�����ʃC���N���[�h
#include "..\KizuLib.h"												// �r�����C�u�����C���N���[�h
#include "..\..\KizuODBC\OdbcBase.h"								// ODBC����N���X
#include "KizuMacro.h"												// �r���}�N��

#include <tchar.h>
using namespace KizuODBC;
using namespace KizuLib;


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �@���Ԋ֌W

//------------------------------------------
// DB_SYSTEM_STATE�ɋ@���Ԃ��Z�b�g����
// int id �@��ID
// int state �@���� (0:����  1�`:�ُ�)
// bool bLogDsp �V�X���O�o�̗͂L��
// char const * cMsg �ǉ����b�Z�[�W
//------------------------------------------

bool KizuFunction::SetStatus(char const* cSec, char const * cKeyFormat, int id, int state, bool bLogDsp, char const * cMsg)
{
	char cKey[256];
	memset(cKey, 0x00, sizeof(cKey));
	sprintf_s(cKey, sizeof(cKey), cKeyFormat, id);

	return SetStatus(cSec, cKey, state, bLogDsp, cMsg );
}
bool KizuFunction::SetStatus(char const* cSec, char const * cKey, int state, bool bLogDsp, char const * cMsg)
{
	int id = GetPrivateProfileInt(cSec, cKey, 0, TASKINI_NAME);
	return SetStatus(id, state, bLogDsp, cMsg );
}
bool KizuFunction::SetStatus(char const* cSec, char const * cKey, bool state, bool bLogDsp, char const * cMsg)
{
	int id = GetPrivateProfileInt(cSec, cKey, 0, TASKINI_NAME);
	return SetStatus(id, (state ? 0 : 1), bLogDsp, cMsg );
}

bool KizuFunction::SetStatus(char const * cKeyFormat, int id, int state, bool bLogDsp, char const * cMsg)
{
	char cKey[256];
	memset(cKey, 0x00, sizeof(cKey));
	sprintf_s(cKey, sizeof(cKey), cKeyFormat, id);

	return SetStatus(cKey, state, bLogDsp, cMsg );
}
bool KizuFunction::SetStatus(char const * cKey, int state, bool bLogDsp, char const * cMsg)
{
	int id = GetPrivateProfileInt(INI_STATUS, cKey, 0, TASKINI_NAME);
	return SetStatus(id, state, bLogDsp, cMsg );
}
bool KizuFunction::SetStatus(char const * cKey, bool state, bool bLogDsp, char const * cMsg)
{
	int id = GetPrivateProfileInt(INI_STATUS, cKey, 0, TASKINI_NAME);
	return SetStatus(id, (state ? 0 : 1), bLogDsp, cMsg );
}
bool KizuFunction::SetStatus(int id, bool state, bool bLogDsp, char const * cMsg)
{
	return SetStatus(id, (state ? 0 : 1), bLogDsp, cMsg );
}
bool KizuFunction::SetStatus(int id, int state, bool bLogDsp, char const * cMsg)
{
	//if(0==id) {
	//	MessageBox(NULL, "SetStatus", "", 0);
	//}
	_ASSERT(0!=id);

	//// DB�ڑ�
	OdbcBase cls_DB;						// ���̗L���DB�ڑ��N���X
	if( ! cls_DB.Connect(TASKINI_NAME) ){
		syslog(SYSNO_DB_CONNECT_ERR,"[%s�����ݎ��s ID=%d State=%d]", DB_STATUS_INF, id, state);
		return false;
	}
	
	//// SQL���쐬
	char cSql[256];
	int len=0;
	memset(cSql, 0x00, sizeof(cSql));
	sprintf( cSql, "UPDATE %s SET �@����=%d WHERE �@��ID=%d", DB_STATUS_INF, state, id );

	//// SQL�����s
	//if( ! cls_DB.ExecuteDirect(cSql) ) {
	//	syslog(SYSNO_DB_EXECUTEDIRECT_ERR,"[%s�����ݎ��s ID=%d State=%d]", DB_STATUS_INF, id, state);
	//	return false;
	//}
	bool bRetc = cls_DB.ExecuteDirect(cSql);
	if( !bRetc ) {
		cls_DB.DisConnect();
		if( ! cls_DB.Connect(TASKINI_NAME) ){
			syslog(SYSNO_DB_CONNECT_ERR,"[%s�����ݎ��s(2���) ID=%d State=%d]", DB_STATUS_INF, id, state);
			return false;
		}
		bRetc = cls_DB.ExecuteDirect(cSql);
	}
	if( ! bRetc ) {
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR,"[%s�����ݎ��s ID=%d State=%d]", DB_STATUS_INF, id, state);
		return false;
	}


	//// �R�~�b�g
	cls_DB.TranCommit(); 
	//// �ؒf
	cls_DB.DisConnect();

	//// �V�X���O�o��
	if(bLogDsp) {
		if(0==state) statelog(SYSNO_STATUS_OK, id, state, cMsg );
		else		 statelog(SYSNO_STATUS_NG, id, state, cMsg );
	}
	return true;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �O��I/F�֌W

//------------------------------------------
// �_���v�o�͊֐�
//   �f�[�^�s�łȂ������� �擪�� # �����鎖�B(�r���_���v�t�@�C����p�t�H�[�}�b�g)
// LPCSTR filename �_���v�t�@�C����
// long len �o�̓T�C�Y
// const BYTE* data �f�[�^
// bool addTime �_���v�t�@�C�����ɓ��t��t�^���邵�Ȃ�
//------------------------------------------
void KizuFunction::DumpData(LPCSTR filename, long len, const BYTE* data, bool addTime)
{
	//// VS2003 �� VS2005 �ł́A
	// ofstream::open()�֐��̎������ς��AVS2003�ł̓}���`�o�C�g������̂܂܈����Ă����̂��A
	// VS2005�ł̓��C�h�����ɕϊ����Ă��爵���悤�ɂȂ������߁A�ϊ������܂������悤�Ƀ��P�[����ݒ肵�Ă����Ȃ��Ƃ����Ȃ�
	setlocale(LC_ALL, "Japanese"); 

	/////// �f�[�^�s�łȂ������� �擪�� # �����鎖�B(�r���_���v�t�@�C����p�t�H�[�}�b�g)
	// ���O�t�H���_����
	_mkdir("Dump");
	// �t�@�C���I�[�v��
	std::ofstream	os;
	char			cFileName[MAX_PATH];
	sprintf(cFileName, ".\\Dump\\%s", filename );
	if(addTime) {									// �ʐM�e�X�g���Ɏg�p (��M���Ƀt�@�C������)
		SYSTEMTIME time;
		GetLocalTime(&time);
		//cFileName.Format("Dump\\%04d%02d%02d_%02d%02d%02d_%03d_%s.log", 
		sprintf(cFileName, ".\\Dump\\%04d%02d%02d_%02d%02d%02d_%03d_%s", 
			time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds,
			filename);
	}
	os.open(cFileName, std::ios::out | std::ios::trunc);

	// �_���v�o��
	char buf[64];
	char wk[64];
	int ii;
	int rec = 0;

	sprintf(buf, "# %d[byte]����M", len);
	os << buf << std::endl;
	os << "# (Hex)ADDR(Dec)  :  0  1  2  3  4  5  6  7 -  8  9  A  B  C  D  E  F | [ASCII]" << std::endl;

	while(true) {
		if(rec == len) break;		// �Ō�܂œ��B

		sprintf(buf, "%07X (%07d) :", rec, rec);
		os << buf;

		memset(wk, 0x00, sizeof(wk));
		//strcpy(wk, " | ");		// _s�����Ă���ANULL�̌�낪�s��l�ɂȂ��Ă��܂��B
		memcpy(wk, &" | ", sizeof(" | ") );

		for(ii=0; ii<16; ii++) {
			if(ii == 8 ) os << " -";
			sprintf(buf, " %02X", data[rec]);
			os << buf;

			if( data[rec] < 0x20 || data[rec] > 0x7F )	wk[ii+3] = '�'; //0x20;		// " | "��3������
			else										wk[ii+3] = data[rec];	// " | "��3������

			// ��
			rec ++;
			if( rec == len ) {							// �ŏI�f�[�^
				for(int jj=ii+1; jj<16; jj++) {					// �]�����ݒ�
					if(ii == 8 ) os << " -";
					os << "   ";
				}
				os << wk << std::endl;
				break;
			}
			if( ii == 15 ) {							// 1�s�I��� 
				os << wk << std::endl;
				break;
			}
			/*
			if( ii == 15 || rec == len) {	// 1�s�I��� ���� �o�b�t�@�Ō� 
				os << wk << std::endl;
				break;
			}
			*/
		}
	}
	os << std::endl;
	os.close();
}

//------------------------------------------
// 4�o�C�g���] (���f�[�^���������̈� ����)
// long len ���]�f�[�^�T�C�Y (4�o�C�g�P��)
// BYTE* data �ϊ��f�[�^�̐擪�|�C���^
//------------------------------------------
BOOL KizuFunction::ByteHanten4(long len, BYTE* data) 
{
	BYTE wk;

	//// 4�o�C�g�P�ʂłȂ���΂Ȃ�Ȃ�
	if( 0 != (len%4) ) return FALSE;
	
	//// ���] (ABCD �� DCBA)
	for( int ii=0; ii<len; ii+=4 ){
		// A��D����ւ�
		wk = data[ii];
		data[ii] = data[ii+3];
		data[ii+3] = wk;
		// B��C����ւ�
		wk = data[ii+1];
		data[ii+1] = data[ii+2];
		data[ii+2] = wk;
	}
	return TRUE;
}

//------------------------------------------
// 2�o�C�g���] (���f�[�^���������̈� ����)
// long len ���]�f�[�^�T�C�Y (2�o�C�g�P��)
// BYTE* data �ϊ��f�[�^�̐擪�|�C���^
//------------------------------------------
BOOL KizuFunction::ByteHanten2(long len, BYTE* data) 
{
	BYTE wk;

	//// 2�o�C�g�P�ʂłȂ���΂Ȃ�Ȃ�
	if( 0 != (len%2) ) return FALSE;
	
	//// ���] (AB �� BA)
	for( int ii=0; ii<len; ii+=2 ){
		// A��B����ւ�
		wk = data[ii];
		data[ii] = data[ii+1];
		data[ii+1] = wk;
	}
	return TRUE;
}
//------------------------------------------
// 4�o�C�g���]��̃f�[�^�擾 (���f�[�^�͕ς��Ȃ�)
// BYTE* data �ϊ��f�[�^�̐擪�|�C���^
// �߂�l ���]��������
//------------------------------------------
long KizuFunction::ByteHanten4(BYTE* data) 
{
	long wk;
	// ABCD �� DCBA��
	((BYTE*)&wk)[0] = data[3];
	((BYTE*)&wk)[1] = data[2];
	((BYTE*)&wk)[2] = data[1];
	((BYTE*)&wk)[3] = data[0];
	return wk;
}
//------------------------------------------
// 2�o�C�g���]��̃f�[�^�擾 (���f�[�^�͕ς��Ȃ�)
// BYTE* data �ϊ��f�[�^�̐擪�|�C���^
// �߂�l ���]��������
//------------------------------------------
short KizuFunction::ByteHanten2(BYTE* data) 
{
	short wk;
	// ABCD �� DCBA��
	((BYTE*)&wk)[0] = data[1];
	((BYTE*)&wk)[1] = data[0];
	return wk;
}

//------------------------------------------
// �X�y�[�X��NULL�ɕύX����
// char* cVal �ϊ�����
// int size �`�F�b�N�ő啶����
//------------------------------------------
void KizuFunction::SpaceToNull(char* cVal, int size) 
{
	for(int ii=0; ii<size; ii++) {
		if( cVal[ii] == ' ' ) {
			cVal[ii] = 0x00;
			return;
		} else if( cVal[ii] == 0x00 ) {
			return;
		}
	}
}

//------------------------------------------
// �X�y�[�X���g��������
// char* cVal �ϊ����� (�I�[��NULL�ł��邱��)
// int nMode �g������� (0���� 1:�擪�̂� 2:�G���h�̂�)
//------------------------------------------
void KizuFunction::Trim(char* cVal, int nMode) 
{
	if( NULL == cVal) return;
	int ii = (int)strlen(cVal);
	if( 0 == ii ) return;

	//// ������
	if(0 == nMode || 2 == nMode ) {
		while(--ii >= 0 && cVal[ii] == ' ' ) ;
		cVal[ii+1] = 0x00;
	} 

	if(0 == nMode || 1 == nMode ) {
		ii = 0;
		while( cVal[ii] != 0x00 && cVal[ii] == ' ') ii++;
		strcpy( cVal, &cVal[ii]);
	}
}
//------------------------------------------
// �Œ蒷��������String�ɕϊ��B
// char* cT			�ϊ��㕶��
// int nTSize		�ϊ���̈�T�C�Y (��{�I�ɁA���̈����+1�傫������)��������OK�����A�������������̂͐��NG
// char const* cF	������
// int nFSize		���̈�T�C�Y
// int nMode �g������� (0���� 1:�擪�̂� 2:�G���h�̂�)
//------------------------------------------
void KizuFunction::TrimSpaceNull(char* cT, int nTSize, char const* cF, int nFSize, int nMode)
{
	if( NULL == cF) return;
	if( NULL == cT) return;
	memset(cT, 0x00, nTSize);
	memcpy(cT, cF, nFSize);
	int nEnd = __min(nFSize, nTSize-1);
	cT[nEnd] = 0x00;
	KizuFunction::Trim(cT, nMode);
}

//------------------------------------------
// �w�萔���̕������g�p����Int�ɕϊ�
// char const* c �Ώە���
// int size �w�蕶����
//------------------------------------------
int KizuFunction::AtoIex(char const* c, int size)
{
	char wk[32];
	memcpy(wk, c, size);
	wk[size] = 0x00;

	return atoi(wk);
}
//------------------------------------------
// �w�萔���̕������g�p����Float�ɕϊ�
// char const* c �Ώە���
// int size �w�蕶����
//------------------------------------------
float KizuFunction::AtoFex(char const* c, int size)
{
	char wk[32];
	memcpy(wk, c, size);
	wk[size] = 0x00;

	return (float)atof(wk);
}
//------------------------------------------
// �w�萔���̕������g�p����Double�ɕϊ�
// char const* c �Ώە���
// int size �w�蕶����
//------------------------------------------
double KizuFunction::AtoDex(char const* c, int size)
{
	char wk[32];
	memcpy(wk, c, size);
	wk[size] = 0x00;

	return atof(wk);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��L�[����

//------------------------------------------
// ��{�ƂȂ��r���Ǘ�No�𐶐� (ID+yyyymmdd hhnnss\0)
// char* cNo ����������r���Ǘ�No
//------------------------------------------
void KizuFunction::GetKizukenNo(char* cNo) 
{
    SYSTEMTIME time;
    GetLocalTime(&time);

	char cWk[SIZE_KIZUKEN_NO];
	int size;
	if( 0 != strcmp("", KIZUKEN_CODE) ) {
		size = sprintf(cWk, "%02s%04d%02d%02d %02d%02d%02d", KIZUKEN_CODE,
								time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	} else {
		size = sprintf(cWk, "%04d%02d%02d %02d%02d%02d", 
								time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	}
	memset(cNo, 0x00, SIZE_KIZUKEN_NO);
	memcpy(cNo, cWk, size);
}
//------------------------------------------
// ��{�ƂȂ�V���A��No�𐶐� (yyyymmdd hhnnss\0)
// char* cNo ����������r���Ǘ�No
//------------------------------------------
void KizuFunction::GetSerialNo(char* cNo) 
{
    SYSTEMTIME time;
    GetLocalTime(&time);

	char cWk[SIZE_SERIAL_NO];
	int size = sprintf(cWk, "%04d%02d%02d %02d%02d%02d", 
								time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

	memset(cNo, 0x00, SIZE_SERIAL_NO);
	memcpy(cNo, cWk, size);
}
//------------------------------------------
// ��{�ƂȂ�IDNo�𐶐� (yyyymmdd hhnnssff\0)
// char* cNo ���������ID (20��)
//------------------------------------------
void KizuFunction::GetSerialNoFF(char* cNo) 
{
    SYSTEMTIME time;
    GetLocalTime(&time);

	char cWk[SIZE_SERIAL_FF_NO];
	int size = sprintf(cWk, "%04d%02d%02d %02d%02d%02d%03d", 
								time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

	memset(cNo, 0x00, SIZE_SERIAL_FF_NO);
	memcpy(cNo, cWk, size);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���������֌W

//------------------------------------------
// NetBIOS�̃R���s���[�^�[���O
// char const* cNewNetBiosComputerName �V�����R���s���[�^�[��
//------------------------------------------
bool KizuFunction::SetNetBiosComputerName(char const* cNewNetBiosComputerName)
{
	//// �R���s���[�^�[����ύX����
	// �� �������ANetBIOS�̖��O�ˁB (�V�X�e���̃v���p�e�B / �R���s���[�^�� / �ύX / �ڍ� �ɏo�ė��閼�O)
	
	int retc = SetComputerName(cNewNetBiosComputerName);
	return (1==retc ? true : false); 
}

//------------------------------------------
// �t�@�C���T�C�Y���擾����
// char const* cFPath �擾�������t�@�C���̃t���p�X
// �߂�l: -1:�ُ�  1�`:�t�@�C���T�C�Y
//------------------------------------------
long KizuFunction::GetFileSize(char const* cFPath)
{
	//// �t�@�C���֌W �̃f�[�^���O����
	FILE*	fp = NULL;			// �t�@�C���|�C���^
	fpos_t	size;				// �t�@�C���T�C�Y

	// �t�@�C���I�[�v��
	fp = fopen(cFPath, "rb");
	if(fp == NULL){        // �I�[�v�����s
		return -1;
	}
	// �t�@�C���T�C�Y�擾
	fseek(fp, 0, SEEK_END);					// ���[�Ɉړ�
	fgetpos(fp, &size);						// �t�@�C���|�C���^�ʒu�擾 = �ŏI�ʒu�Ȃ̂Ńt�@�C���T�C�Y��������
	fclose(fp);
	
	return (long)size;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �r���Ǝ��ŗǂ��g�p����֐�

//------------------------------------------
// ���חD��x��r
// double nowLen (�V����) ��������ʒu [mm] 
// int nowYusenT (�V����) �r��D��x     (1���ŋ�)
// int nowYusenG (�V����) �O���[�h�D��x (1���ŋ�)
// double preLen (������) ��������ʒu [mm]
// int preYusenT (������) �r��D��x
// int preYusenG (������) �O���[�h�D��x
// �߂�l true:�V���ׂ̕�������  false:�����ׂ̕�������
//------------------------------------------
bool KizuFunction::SelectPriorityDetect(double nowLen, int nowYusenT, int nowYusenG,
										double preLen, int preYusenT, int preYusenG )
{
	//// �`�F�b�N�����B�オ��ԋ���
	// �EGr
	// �E�r��
	// �E�������
	// �E��L3���ꏏ�Ȃ�A�����ׂ�D��
	if( nowYusenG < preYusenG ) {				// ����̃O���[�h�̕������� (���l��������)
		return true;
	} else if( nowYusenG == preYusenG ) {		// �O���[�h�̗D��x������
		if( nowYusenT < preYusenT ) {				// ������r��̕�������
			return true;
		} else if( nowYusenT == preYusenT ) {
			if ( nowLen < preLen ) {					// ����̒�������ʒu���������i�����j
				return true;
			}
		}
	}
	return false;				// �����܂ł�����A�����ׂ�D��
}

//------------------------------------------
// ���חD��x��r Ex
//	 �e�ϐ��z��ɏ����Z�b�g
// int nCnt				��r��
// double const* pNow	��r����V�������ԂɃZ�b�g(�z��̐擪���ŗD��) 
// double const* pPre	��r���鋌�������ԂɃZ�b�g(�z��̐擪���ŗD��) 
// bool const* bBig		��r���� (true:�V����r�����Ƃ��ɑ傫�����������B false:�V����r�����Ƃ��ɏ�������������)
// �߂�l true:�V���ׂ̕�������  false:�����ׂ̕�������
//------------------------------------------
bool KizuFunction::SelectPriorityDetect(int nCnt, double const* pNow, double const* pPre, bool const* bMode)
{
	for(int ii=0; ii<nCnt; ii++) {
		if(pNow[ii] == pPre[ii]) continue;

		if( bMode[ii] ) {
			// �傫����������
			if(pNow[ii] > pPre[ii]) return true;
			else					return false;
		} else {
			// ��������������
			if(pNow[ii] < pPre[ii]) return true;
			else					return false;
		}
	}
	return false;				// �S���ꏏ�Ȃ� �����ׂ�D��
}

//------------------------------------------
// �O���b�h�̗񐔂��Z�o����
// int width �Z�o��������
// int interval ��̊Ԋu
// int maxCol �ő�̗� 
// �߂�l �O���b�h�� 
//------------------------------------------
int KizuFunction::SelectGridCol( int width, int interval, int maxCol )
{
	// �O���b�h�ʒu (0�`199:0�A200�`299:1)
	int ii;

	// ���s���ɂ��A�ő�ɂ���
	if( 0 >= width ) return maxCol;
	// ���ɂ��Z�o
	for(ii=1; ii<(maxCol/2); ii++) {
		if( width < (interval*2)*ii ) break;
	}
	if( ii < (maxCol/2) ) return ii*2;
	else				  return maxCol;
}

//------------------------------------------
// �O���b�h�̗񐔂��Z�o���� (��}�b�v�ɑΉ�)
// int width �Z�o��������
// int interval ��̊Ԋu
// int maxCol �ő�̗� 
// �߂�l �O���b�h�� 
//------------------------------------------
int KizuFunction::SelectGridCol2( int width, int interval, int maxCol )
{
	//// �O���b�h�ʒu (0�`200:0�A200�`400:1)

	// ���s���ɂ��A�ő�ɂ���
	if( 0 >= width ) return maxCol;

	// ���ɂ��Z�o
	int nCol  = 0;
	int env	  = maxCol % 2;		// �T����ʂ̗� (0:������  1:���)

	// ����̋��E�ʒu����
	if(0 == env) {		// ����	
		nCol = ( GET_JUST_INDEX(width, (interval*2) ) + 1 ) * 2;

	} else {			// �
		width -= interval;
		if(0 >= width ) return 1;		// 1�}�X�ȓ��̏ꍇ�̓��ꏈ��

		nCol = ( GET_JUST_INDEX(width, (interval*2) ) + 1 ) * 2 + 1;
	}

	// �������`�F�b�N
	if(maxCol < nCol)	return maxCol;
	if(0 > nCol)		return maxCol;
	return nCol;
}

//------------------------------------------
// �O���b�h�ʒu����֐� (�G�b�W����̋����)
// int DorW 0:DS����r 1:WS����r
// double edge_D ���G�b�W����̋��� [mm]
// double edge_W �E�G�b�W����̋��� [mm]
// long width �R�C���� [mm] (�������`�F�b�N��̒l�ł��鎖)
// long col_width ���ɂ��O���b�h��  (�_���I�ȍő�O���b�h��)
// long col_max �ő�O���b�h			(�����I�ȍő�O���b�h��)
// long col_interval �O���b�h�̕� [mm]
// �߂�l �Z�o�O���b�h�ʒu (0:DS�`7:WS)
//------------------------------------------
int KizuFunction::SelectColPosition(int DorW, double edge_D, double edge_W, long width, long col_width, long col_max, long col_interval)
{
	int centerWid = width / 2;			// ���̒��S
	int widthCol = col_width / 2;		// �������`�F�b�N�p
	int centerCol = col_max / 2;		// DS��̃O���b�h�ʒu�Z�o�p (�߂�l)
	int col;							// �Z�o�O���b�h

	//-----------------------------------------
	// �G�b�W����̋����
	//-----------------------------------------
	if (false) {
		//// �O���b�h�ʒu
		if (0 == DorW)	col = (int)(edge_D / col_interval);				// �G�b�W����ǂꂾ������Ă��邩
		else			col = (int)(edge_W / col_interval);

		//// �������`�F�b�N
		// �R�C�����ƃO���b�h������v���Ȃ��ׁA�O���b�h�����R�C�������L���ꍇ�́A��Ԓ[�̃O���b�h�Ƃ���
		if (0 > col)			col = 0;
		if (widthCol <= col)	col = widthCol - 1;

		//// ��ʒu����̐�Έʒu�ɕϊ�
		if (true) {							// DS=0�
			if (0 == DorW)	return (centerCol - widthCol) + col;		// ���
			else			return (centerCol + widthCol) - col - 1;		// ���Α�

		} else {							// WS=0�
			if (0 == DorW)	return (centerCol + widthCol) - col - 1;		// ���Α�
			else			return (centerCol - widthCol) + col;		// ���

		}


	//-----------------------------------------
	// �R�C���Z���^�[�
	//-----------------------------------------
	} else {
		//// �O���b�h�ʒu (0�`200:0�A201�`400:1)
		if (0 == DorW)	col = (int)((centerWid - edge_D) / col_interval);				// �R�C�����S����ǂꂾ������Ă��邩
		else			col = (int)((centerWid - edge_W) / col_interval);

		//// �������`�F�b�N
		if (0 > col)			col = 0;
		if (widthCol <= col)	col = widthCol - 1;

		//// ��ʒu����̐�Έʒu�ɕϊ�
		if (true) {							// DS=0�
			if (0 == DorW)	return centerCol - col - 1;		// ���
			else			return centerCol + col;			// ���Α�

		} else {							// WS=0�
			if (0 == DorW)	return centerCol + col;			// ���Α�
			else			return centerCol - col - 1;		// ���

		}
	}
}

//------------------------------------------
// �O���b�h�ʒu����֐� (�Z���^�[�)
// int DorW 0:DS����r 1:WS����r

// double edge_D ���G�b�W����̋��� [mm]
// double edge_W �E�G�b�W����̋��� [mm]
// long width �R�C���� [mm] (�������`�F�b�N��̒l�ł��鎖)
// long col_width ���ɂ��O���b�h��  (�_���I�ȍő�O���b�h��)
// long col_max �ő�O���b�h			(�����I�ȍő�O���b�h��)
// long col_interval �O���b�h�̕� [mm]
// �߂�l �Z�o�O���b�h�ʒu (0:DS�`)
//------------------------------------------
// --->>> 20180220 �s��C��
//int KizuFunction::SelectColPosition2(int DorW, double edge_D, double edge_W, long width, long col_width, long col_max, long col_interval)
//{
//	//// �O���b�h�ʒu (0�`200:0�A201�`400:1)
//
//	int env = col_max % 2;		// �T����ʂ̗� (0:������  1:���)
//	int centerWid = width / 2;		// ���̒��S
//	int widthCol = col_width / 2;	// �������`�F�b�N�p
//	int centerCol = col_max / 2;		// DS��̃O���b�h�ʒu�Z�o�p (�߂�l)
//	int centerX;						// �Z���^�[������r�ʒu
//
//
//	//-----------------------------------------
//	// �Z���^�[������r�ʒu
//	if (0 == DorW)	centerX = (int)(centerWid - edge_D);
//	else			centerX = (int)(centerWid - edge_W);
//	if (0 > centerX) {
//		DorW = (0 == DorW ? 1 : 0);
//		centerX = abs(centerX);
//	}
//
//
//	//-----------------------------------------
//	// �R�C���Z���^�[�
//	if (0 == env) {		// ����	
//		int col = GET_JUST_INDEX(centerX, (col_interval));
//
//		if (0 == DorW)	return centerCol - col - 1;		// ���
//		else			return centerCol + col;			// ���Α�
//
//
//	}
//	else {			// �
//		centerX -= col_interval / 2;
//		if (0 >= centerX) return centerCol;		// ����
//
//		int col = GET_JUST_INDEX(centerX, (col_interval));
//
//		if (0 == DorW)	return centerCol - col - 1;		// ���
//		else			return centerCol + col + 1;		// ���Α�
//	}
//}
int KizuFunction::SelectColPosition2(int DorW, double edge_D, double edge_W, long width, long col_width, long col_max, long col_interval)
{
	//// �O���b�h�ʒu (0�`200:0�A201�`400:1)

	int env = col_max % 2;				// �T����ʂ̗� (0:������  1:���)
	double centerWid = width / 2;		// ���̒��S
	int widthCol = col_width / 2;		// �������`�F�b�N�p
	int centerCol = col_max / 2;		// DS��̃O���b�h�ʒu�Z�o�p
	int centerX;						// �Z���^�[������r�ʒu
	int col;							// �߂�l


	//-----------------------------------------
	// �Z���^�[������r�ʒu
	if (0 == DorW)	centerX = (int)ceil(centerWid - edge_D);
	else			centerX = (int)ceil(centerWid - edge_W);
	if (0 > centerX) {
		DorW = (0 == DorW ? 1 : 0);
		centerX = abs(centerX);
	}


	//-----------------------------------------
	// �R�C���Z���^�[�
	if (0 == env) {		// ����	
		col = GET_JUST_INDEX(centerX, (col_interval));

		if (0 == DorW)	col = centerCol - col - 1;		// ���
		else			col = centerCol + col;			// ���Α�


	} else {			// �
		centerX -= col_interval / 2;
		if (0 >= centerX) return centerCol;		// ����

		col = GET_JUST_INDEX(centerX, (col_interval));

		if (0 == DorW)	col = centerCol - col - 1;		// ���
		else			col = centerCol + col + 1;		// ���Α�
	}

	// �������`�F�b�N
	if (col < (centerCol - widthCol))				col = (centerCol - widthCol);
	if (col >= col_max - (centerCol - widthCol))	col = col_max - (centerCol - widthCol);

	return col;
}
// <<<--- 20180220 �s��C��

//------------------------------------------
// �O���b�h�ʒu����֐� (�񐔌Œ�o�[�W����)
// int DorW 0:DS����r 1:WS����r

// double edge_D ���G�b�W����̋��� [mm]
// double edge_W �E�G�b�W����̋��� [mm]
// long width �R�C���� [mm] (�������`�F�b�N��̒l�ł��鎖)
// long col_max �ő�O���b�h			(�����I�ȍő�O���b�h��)
// �߂�l �Z�o�O���b�h�ʒu (0:DS�`)
//------------------------------------------
int KizuFunction::SelectColPositionFix(int DorW, double edge_D, double edge_W, long width, long col_max)
{
	int cMaxCol		= col_max*2;		// �Z�o��ŁA���̏ꍇ�l�����A������ɂ��Čv�Z������
	int nWidIntval	= width / cMaxCol;	// �񕝂�s�x�Z�o
	
	// 2�{�̗񐔂łƂ肠�����Z�o
	int col = SelectColPosition2(DorW, edge_D, edge_W, width, cMaxCol, cMaxCol, nWidIntval);

	// �������`�F�b�N
	if( 0 > col || col >= cMaxCol ) {
		//if( col > 0 )	col = 0;
		if( col < 0 )	col = 0;		// 20150305 �s��Ή�
		else			col = cMaxCol - 1;
	}

	// �{���̗񐔂ɖ߂�
	return (col/2);
}