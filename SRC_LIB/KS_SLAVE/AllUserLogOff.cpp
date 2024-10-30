#include "stdafx.h"
#include "AllUserLogOff.h"

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
AllUserLogOff::AllUserLogOff(void) :
FuncBase("LogOff")
{
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
AllUserLogOff::~AllUserLogOff(void)
{
}

//------------------------------------------
// �����[�g���[�U�[���O�I�t���s����
//------------------------------------------
// �߂�l
// ���A���(bool):true �c����I��(�����ƃT�[�o�ȊO�̓��O�I�t����)
//                false�c�ُ�I��(�ُ�ׁ̈A���O�I�t������)
//------------------------------------------
bool AllUserLogOff::Exec()
{
	// �ϐ��̐錾_������
	DWORD nMyProcessID = GetCurrentProcessId();		// ���v���Z�XID���擾����
	DWORD nMySessionID = 0;							// ���Z�b�V����ID���i�[����
	WTS_SESSION_INFO* psessions = 0;				// �Z�b�V���������i�[����
	DWORD nScount = 0;								// �Z�b�V���������i�[����

	// ���v���Z�XID���玩�Z�b�V����ID���擾����
	ProcessIdToSessionId( nMyProcessID, &nMySessionID );

	// �Z�b�V���������擾����
	if( ! WTSEnumerateSessions( WTS_CURRENT_SERVER_HANDLE, 0, 1, &psessions, &nScount ) ) return false;

	// �Z�b�V�������ƃZ�b�V���������擾�ł����ꍇ
	// �Z�b�V���������������s��
	for( DWORD ii = 0; ii < nScount; ii++ )
	{
		// �����̃Z�b�V����ID�ȊO�ƃT�[�r�X�p�Z�b�V����(ID:0)�ȊO�ƃ��b�X���Z�b�V����65536�̃��[�U�[�����O����
		if( ( nMySessionID != psessions[ii].SessionId ) && 
			( ( 0 != psessions[ii].SessionId ) && ( 0 != lstrcmp( psessions[ii].pWinStationName, "Services") ) ) &&
			( WTSListen != psessions[ii].State ) )
		{
			// �����̃��[�U���ȊO�ƃT�[�r�X�p�Z�b�V�����ȊO�ƃ��b�X���Z�b�V����65536�̃��[�U�[�����O�I�t����
			BOOL bRet = WTSLogoffSession( WTS_CURRENT_SERVER_HANDLE, psessions[ii].SessionId, TRUE );
			LOG(em_MSG), "[%s] ���O�I�t�Z�b�V������:%s ���O�I�t�Z�b�V����ID:%d ���O�I�t�֐��߂�l:%d ", my_sThreadName, psessions[ii].pWinStationName, psessions[ii].SessionId, bRet );		// ���X�g�{�b�N�X�Ƀ��O�I�t�����Z�b�V����ID��\��������
			continue;									// ���̃Z�b�V����ID�m�F
		}
		LOG(em_MSG), "[%s] ���O�I���̂܂� (���O�I�t�Z�b�V������:%s �Z�b�V����ID:%d)", my_sThreadName, psessions[ii].pWinStationName, psessions[ii].SessionId );		// ���X�g�{�b�N�X�Ƀ��O�I�t�����Z�b�V����ID��\��������
	}
	// ���������J������
	WTSFreeMemory( psessions );
	return true;
}

/***
typedef BOOL
WINAPI
WTSConnectSessionA_Type(
    IN ULONG LogonId,
    IN ULONG TargetLogonId,
    __in PSTR pPassword,
    IN BOOL bWait
    );
***/


//------------------------------------------
// �R���\�[������ �\�łȂ����ɂ́A�R���\�[����\�ɕύX����
//   ��C�ł͍��̂Ƃ���g��Ȃ��B�T���v�����x�B�����Ɠ���
//------------------------------------------
bool AllUserLogOff::Change()
{
	bool bRetc = false; 
/*** VS2010�ȍ~����Ȃ��ƃR���p�C���ʂ�Ȃ��B �� WTSConnectSessionA_Type �Ή���VS2005�ł������͂������B
	HMODULE hDll = LoadLibrary("wtsapi32.dll");
	WTSConnectSessionA_Type *pWTSConnectSessionA = (WTSConnectSessionA_Type*)GetProcAddress( hDll, "WTSConnectSessionA");
	if( NULL == pWTSConnectSessionA)
		return false;


	// �ϐ��̐錾_������
	DWORD nMyProcessID = GetCurrentProcessId();		// ���v���Z�XID���擾����
	DWORD nMySessionID = 0;							// ���Z�b�V����ID���i�[����
	WTS_SESSION_INFO* psessions = 0;				// �Z�b�V���������i�[����
	DWORD nScount = 0;								// �Z�b�V���������i�[����

	// ���v���Z�XID���玩�Z�b�V����ID���擾����
	ProcessIdToSessionId( nMyProcessID, &nMySessionID );

	// �Z�b�V���������擾����
	if( ! WTSEnumerateSessions( WTS_CURRENT_SERVER_HANDLE, 0, 1, &psessions, &nScount ) ) return false;

	// �R���\�[���Z�b�V���������
	DWORD	nConsoleId = 0;
	for( DWORD ii = 0; ii < nScount; ii++) {
		if(0 == lstrcmp( psessions[ii].pWinStationName, "Console") ) {
			nConsoleId = psessions[ii].SessionId;
			break;
		}
	}

	// �؂�ւ�
	if( 0 != nConsoleId && nMySessionID != nConsoleId ) {
		// �y�d�v�z��R�����̓p�X���[�h�B �����ł���΋󔒂ł悢�͂��B
		if( pWTSConnectSessionA( nMySessionID, nConsoleId, "", true) )  bRetc = true;
	}

	// ���������J������
	WTSFreeMemory( psessions );
***/
	return bRetc;
}