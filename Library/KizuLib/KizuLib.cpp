
#include "stdafx.h"
#include "KizuLib.h"

#include <crtdbg.h>
#include <stdio.h>

#include "Class\KizuMacro.h"												// �}�N���W
#include "Class\MailSlotManager.h"											// ���[���X���b�g�N���X
#include "Class\AutoLock.h"													// CRITICAL_SECTION �̃��b�N�E�A�����b�N �N���X
#include "Class\KizuPcInfManager.h"											// PC�Ǘ�
#include "Class\MailTransManager.h"											// ���[���]���N���X

//#include <odbcinst.h>														// ODBC�����ݒ�p

using namespace KizuLib;

// ///////////////////////////////////////////////////////////////////
// �O���[�o���ϐ�
//
char PCNAME[MAX_PCCOUNT][SIZE_NAME_PC];										// KizuCommon.h�ɂ���exertn�錾�̎���

// ///////////////////////////////////////////////////////////////////
// �X�^�e�B�b�N �����o�ϐ�
//
//#define KIZULIB_KIZULIB_SESSION				"PC"						// ini�t�@�C���Z�b�V����
static MailSlotManager*		mcls_pMail = NULL;								// ���[���X���b�g�N���X
static KizuPcInfManager*	mcls_pPcInf = NULL;								// PC���̊Ǘ��N���X

static int					g_nPCID = 0;									// ��PCID (1�I���W��)
static int					g_nLineID = LINE_ID;							// �����C��ID (�ʏ�0�Œ�B����PC�ɕ������C�����݂���ꍇ�Ɏg�p)
static char					g_cMyTaskName[SIZE_NAME_TASK];					// ���^�X�N����


// ///////////////////////////////////////////////////////////////////
// ���\�b�h
//
//------------------------------------------
// �^�X�N����
// LPCSTR cTaskName ���^�X�N����
// �߂�l ���A���
//------------------------------------------
int	task_init(LPCSTR cTaskName) 
{
	// ini �t�@�C�����擾
	int nPCID = GetPrivateProfileInt(INI_COMMON, "PCID", -99, PCINI_NAME);
	_ASSERT(-99 != nPCID);
	if(-99 == nPCID) return -99;

	return task_init(cTaskName, nPCID);
}

//------------------------------------------
// �^�X�N����
// LPCSTR cTaskName ���^�X�N����
// int nPCID ��PCID (0:DB�ڑ�����(PC����.)�A -1:DB�ڑ�����(PC���̓R���s���[�^�����擾)�A -9:DB�ڑ�����(PC���̓R���s���[�^�����擾))
// �߂�l ���A���
//------------------------------------------
int task_init(LPCSTR cTaskName, int nPCID)
{
	// PC���̊Ǘ��N���X�쐬
	if(NULL != mcls_pPcInf) return -9;
	mcls_pPcInf = new KizuPcInfManager(nPCID);
	getpcname();												// PC���̎擾

	// ���^�X�N���[���X���b�g�쐬
	if(NULL != mcls_pMail) return -9;
	mcls_pMail = new MailSlotManager();
	if( !mcls_pMail->Create(cTaskName) ) return -1;

	// �X�^�e�B�b�N�ϐ��ێ�
	g_nPCID = nPCID;
	strcpy(g_cMyTaskName, cTaskName);
	return 0;
}

//------------------------------------------
// �^�X�N���� (����PC�����^�X�N���s�p)
// LPCSTR cTaskName ���^�X�N���� (�T�C�Y�̊֌W��A16-4=12�������炢�ɂ��Ă�����)
// int nLineID ���C��ID (�ʏ�́Agetlineid() �������Ă����l)
// �߂�l ���A���
//------------------------------------------
int task_init_multi(LPCSTR cTaskName, int nLineID)
{
	// �����^�X�N���̌���
	char cMultiTaskName[SIZE_NAME_TASK];
	memset(cMultiTaskName, 0x00, sizeof(cMultiTaskName));
	sprintf(cMultiTaskName, "%s_%d", cTaskName, nLineID );

	// �^�N�X����
	return task_init(cMultiTaskName);
}

//------------------------------------------
// �^�X�N�I��
// LPCSTR cTaskName �^�X�N����
// LPCSTR cPcName PC����
// �߂�l ���A���
//------------------------------------------
void task_exit()
{
	SAFE_DELETE(mcls_pMail);
	SAFE_DELETE(mcls_pPcInf);
}

//------------------------------------------
// ���[�����M
// LPCSTR SendSlotName ���M���[���X���b�g����
// LPCSTR SendPcName ���MPC����
// LPCVOID pDate ���M���
// �߂�l ���A���
//------------------------------------------
int	send_mail(LPCSTR SendSlotName, LPCSTR SendPcName, LPCVOID pDate)
{
	return MailSlotManager::SendMail(SendSlotName, SendPcName, pDate, SIZE_MSL);
}
//------------------------------------------
// ���[���]��
// LPCSTR SendSlotName ���M���[���X���b�g����
// LPCSTR SendPcName ���MPC����
// LPCVOID pDate ���M���
// �߂�l ���A���
//------------------------------------------
int	send_mail_trans(LPCSTR SendSlotName, LPCSTR SendPcName, LPCVOID pDate)
{
	return MailTransManager::SendMailTrans(SendSlotName, SendPcName, pDate, SIZE_MSL);
}
//------------------------------------------
// ���[���]��
// LPCSTR SendUdpAddr ���M��IP�A�h���X
// LPCSTR SendSlotName ���M���[���X���b�g����
// LPCSTR SendPcName ���MPC����
// LPCVOID pDate ���M���
// �߂�l ���A���
//------------------------------------------
int	send_mail_trans(LPCSTR SendUdpAddr, LPCSTR SendSlotName, LPCSTR SendPcName, LPCVOID pDate)
{
	return MailTransManager::SendMailTrans(SendUdpAddr, SendSlotName, SendPcName, pDate, SIZE_MSL);
}

//------------------------------------------
// ���[����M
// LPVOID pDate ��M���
// �߂�l ���A���
//------------------------------------------
int	recv_mail(LPVOID pDate)
{
	return mcls_pMail->Recv(pDate, SIZE_MSL);
}
//------------------------------------------
// ���[����M (�񓯊�)
// LPVOID pDate ��M���
// OVERLAPPED* ovlRecv �I�[�o�[���b�v
// �߂�l ���A���
//------------------------------------------
bool recv_mail_ovl(LPVOID pDate, OVERLAPPED* ovlRecv)
{
	return mcls_pMail->Recv(pDate, SIZE_MSL, ovlRecv);
}
//------------------------------------------
// �SPC���̂�DB���擾
//------------------------------------------
bool getpcname()
{
	if(NULL == mcls_pPcInf) return false;
	
	return mcls_pPcInf->SetPcName(PCNAME[0], TASKINI_NAME);			// DB��� PC���̂��擾
}

//------------------------------------------
// ��PC���̎擾
//------------------------------------------
char const* getmypcname()
{
	if(NULL == mcls_pPcInf) return NULL;
	
	return mcls_pPcInf->GetMyPcName();
}

//------------------------------------------
// �V�X�e�����O�o��
// long nSysNo ���b�Z�[�WNo
// LPCSTR __cBuffer �ψ���
// �߂�l ���A���
//------------------------------------------
int syslog(long nSysNo, LPCSTR __cBuffer, ... )
{
	char	cMsg[2048];
    SYSTEMTIME time;
    GetLocalTime(&time);
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));

	//// �ψ������A���b�Z�[�W�쐬
	memset( cMsg, 0x00, sizeof(cMsg));
	va_list	vmarker;
	va_start( vmarker, __cBuffer ); 				// �ό̈����̏�����
	//_vsnprintf( (char*)cMsg, _TRUNCATE, __cBuffer, vmarker );
	_vsnprintf( (char*)cMsg, sizeof(que.syslog_que.cMsg)-1, __cBuffer, vmarker );
	va_end( vmarker );								// �ό̈����̃��Z�b�g

	//// ���b�Z�[�W����
	que.nEventNo = FACT_KS_SYSLOG_01;												// �C�x���g�R�[�h
	que.nLineNo = getlineid();														// ���C��ID
	que.syslog_que.nSysNo = nSysNo;													// ���b�Z�[�WNo
	que.syslog_que.nPcKind = NULL == mcls_pPcInf ? 0 : mcls_pPcInf->GetPcKind();	// PCKIND 
	que.syslog_que.date[0] = time.wYear;											// ��PC����
	que.syslog_que.date[1] = time.wMonth;
	que.syslog_que.date[2] = time.wDay;
	que.syslog_que.date[3] = time.wHour;
	que.syslog_que.date[4] = time.wMinute;
	que.syslog_que.date[5] = time.wSecond;
	strcpy( que.syslog_que.cTaskName, g_cMyTaskName);				// �^�X�N��
	if(NULL != mcls_pPcInf) strcpy( que.syslog_que.cPcName, mcls_pPcInf->GetMyPcName() );	// PC��
	strcpy( que.syslog_que.cMsg, cMsg);				// ���b�Z�[�W���e

	//// ���[�����M
	return send_mail(KS_SYSLOG, SYSLOG_PC_NAME, &que );
}

//------------------------------------------
// �@���ԕύX���O�o�� (��{�I�ɂ́A���C�u��������݂̂����R�[�����Ȃ��͂�)
// long nSysNo ���b�Z�[�WNo
// long nStateId �@����ID
// long nStateVal �@���Ԓl (0:����  1�`:�ُ�)
// LPCSTR __cBuffer �ψ���
// �߂�l ���A���
//------------------------------------------
int statelog(long nSysNo, long nStateId, long nStateVal, LPCSTR __cBuffer, ... )
{
	char	cMsg[2048];
    SYSTEMTIME time;
    GetLocalTime(&time);
	COMMON_QUE que;
	memset(&que, 0x00, sizeof(que));

	//// �ψ������A���b�Z�[�W�쐬
	memset( cMsg, 0x00, sizeof(cMsg));
	va_list	vmarker;
	va_start( vmarker, __cBuffer ); 				// �ό̈����̏�����
	//_vsnprintf( (char*)cMsg, _TRUNCATE, __cBuffer, vmarker );
	_vsnprintf( (char*)cMsg, sizeof(que.statelog_que.cMsg)-1, __cBuffer, vmarker );
	va_end( vmarker );								// �ό̈����̃��Z�b�g

	//// ���b�Z�[�W����
	que.nEventNo = FACT_KS_SYSLOG_02;												// �C�x���g�R�[�h
	que.nLineNo = getlineid();														// ���C��ID
	que.statelog_que.nSysNo = nSysNo;												// ���b�Z�[�WNo
	que.statelog_que.nPcKind = NULL == mcls_pPcInf ? 0 : mcls_pPcInf->GetPcKind();	// PCKIND 
	que.statelog_que.nStateId = nStateId;											// �@����ID
	que.statelog_que.nStateVal = nStateVal;											// �@���Ԓl
	que.statelog_que.date[0] = time.wYear;											// ��PC����
	que.statelog_que.date[1] = time.wMonth;
	que.statelog_que.date[2] = time.wDay;
	que.statelog_que.date[3] = time.wHour;
	que.statelog_que.date[4] = time.wMinute;
	que.statelog_que.date[5] = time.wSecond;
	strcpy( que.statelog_que.cTaskName, g_cMyTaskName);			// �^�X�N��
	if(NULL != mcls_pPcInf) strcpy( que.statelog_que.cPcName, mcls_pPcInf->GetMyPcName() );	// PC��
	strcpy( que.statelog_que.cMsg, cMsg);			// ���b�Z�[�W���e

	//// ���[�����M
	return send_mail(KS_SYSLOG, SYSLOG_PC_NAME, &que );
}

//------------------------------------------
// ���L�������A�N�Z�X
// HANDLE *hMapFile �n���h��(out)
// LPVOID *pMapped �}�b�s���O�A�h���X (out)
// const char* name �ڑ����L���������� (in)
//------------------------------------------
void mem_connect(HANDLE *hMapFile, LPVOID *pMapped, const char* name)
{
	*hMapFile =OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, name);
	*pMapped = MapViewOfFile(*hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
}

//------------------------------------------
// ���L�������A�N�Z�X
// HANDLE *hMapFile �n���h��(out)
// LPVOID *pMapped �}�b�s���O�A�h���X (out)		�����ۂɎg���ꍇ�� *pWk = (char*)*pMapped+nAli; �ł��炷�K�v�L��
// const char* name �ڑ����L���������� (in)
// DWORD offset	�}�b�s���O �I�t�Z�b�g�ʒu (in)
// SIZE_T s �}�b�s���O �T�C�Y (in)
// int	 nAddrAli	�A���C�����g���Ĉړ��������T�C�Y�B  -1 ���炵���ꍇ�́A1���Z�b�g�����
//---------------
// ��	�I�t�Z�b�g���g�p����ꍇ�A�I�t�Z�b�g�̓y�[�W�T�C�Y�̔{���ł���K�v������
//		�܂�A�w�肳�ꂽoffset���y�[�W�T�C�Y�̔{���łȂ��ꍇ�́A
//		�A�h���X��߂��āA�A���C�����g�����킹��B
//		���̖߂��ʂ� nAddrAli �ɃZ�b�g����
//		���ӂƂ��āA�߂����ʕ� �T�C�Y�͑���������
//------------------------------------------
void mem_connect(HANDLE *hMapFile, LPVOID *pMapped, const char* name, DWORD offset, SIZE_T s, int* nAddrAli)
{
	// �y�[�W�T�C�Y�ŃA���C�����g
	SYSTEM_INFO si;
	GetSystemInfo(&si);			// �y�[�W�T�C�Y���擾
	int nAli = offset % si.dwAllocationGranularity;		// ���炵��
	int nAnsOffset = offset - nAli;
	int nAnsSize = s + nAli;


	*hMapFile =OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, name);
	*pMapped = MapViewOfFile(*hMapFile, FILE_MAP_ALL_ACCESS, 0, nAnsOffset, nAnsSize);

	//	*pMapped = (char*)*pMapped+nAli;	// �e�X�g�B����ō\���̂ɒ��ڃA�N�Z���\�B�������AUnmapViewOfFile���ɃY���V������Ă���̂œ{����
	
	*nAddrAli = nAli;
}

//------------------------------------------
// ���L�������g�p�I��
// HANDLE *hMapFile �n���h��(out)
// LPVOID *pMapped �}�b�s���O�A�h���X (out)
//------------------------------------------
void mem_close(HANDLE *hMapFile, LPVOID *pMapped)
{
	BOOL r = UnmapViewOfFile(*pMapped);
	CloseHandle(*hMapFile);
}

//------------------------------------------
// ��PCID (1�I���W��)
//------------------------------------------
int getpcid()
{
	return g_nPCID;
}
//------------------------------------------
// �����C��ID�Z�b�g
//------------------------------------------
void setlineid()
{
	int nLineID = GetPrivateProfileInt(INI_COMMON, "LINEID", -1, TASKINI_NAME);
	_ASSERT(-1 != nLineID);
	g_nLineID = nLineID;
}
void setlineid(int nLineID)
{
	g_nLineID = nLineID;
}
//------------------------------------------
// �����C��ID�擾
//------------------------------------------
int getlineid()
{
	return g_nLineID;
}
//------------------------------------------
// ���^�X�N����
//------------------------------------------
char const* getmytaskname()
{
	return g_cMyTaskName;
}
//------------------------------------------
// KizuPcInfManager�N���X�ԋp
//------------------------------------------
LPCVOID getcls_pcinfmgr()
{
	return (LPCVOID)mcls_pPcInf;
}
