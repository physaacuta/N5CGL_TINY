// LogFileManager.cpp: LogFileManager �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogFileManager.h"

#include <direct.h>
#include "..\..\KizuLib\Class\KizuMacro.h"												// �}�N���W

using namespace KizuLib;

//------------------------------------------
// �R���X�g���N�^
// LPCSTR filename ���O�t�@�C���� (NULL�� ���O�o�͂��Ȃ�)
// CListBox *list ���X�g�{�b�N�X�R���g���[�� (NULL�� ���X�g�{�b�N�X�\�����Ȃ�)
// int cycleCnt �ő�T�C�N���b�N�t�@�C������
// int backCnt �ő�o�b�N�A�b�v����
//------------------------------------------
//LogFileManager::LogFileManager()
LogFileManager::LogFileManager(LPCSTR filename, CListBox *list, int cycleCnt, int backCnt) :
m_cntList(list),
m_nMaxCycleCnt(cycleCnt),
m_nMaxBackCnt(backCnt),
m_nNowRow(0),
m_nNowFileNo(0),
mque_Log(KIZULIB_LOGFILEMANAGER_MAX_QUE)
{
	
	//// ������
	InitializeCriticalSection(&m_csOutPut);
	InitializeCriticalSection(&m_csANC);

	//// �o�͂��邵�Ȃ�����
	m_bFileOut = NULL==filename ? false : true;
	m_bListOut = NULL==list ? false : true;

	//// �o�b�N�A�b�v (�t�@�C���o�͗L�莞�̂�)
	if ( m_bFileOut ) {
		int ii, jj;
		CString cFileName;
		CString cFileNameNew;

		// ���O�t�H���_����
		_mkdir("Log");
		
		// �x�[�X���̌���
		//_getdcwd( 0, m_cBaseFolder.GetBuffer(256) , 256 );	// ���݂̃t�H���_���擾
		char cwk[256];
		GetCurrentDirectory( 256, cwk);
		m_cBaseFolder += cwk;
		m_cBaseFolder += "\\Log\\";
		m_cBaseFile = filename;

		// bk_xxx_@9_0.log �` bk_xxx_@9_9.log���폜  (�ŌÃt�@�C���폜)
		for( ii=0; ii<=m_nMaxCycleCnt; ii++ ) {
			cFileName.Format("%sbk_%s_@%d_%d.log", m_cBaseFolder, m_cBaseFile, m_nMaxBackCnt, ii );
			remove(cFileName);
		}

		// bk_xxx_@0_0.log �` bk_xxx_@0_9.log �� bk_xxx_@1_0.log �` bk_xxx_@1_9.log �ɕύX (�o�b�N�A�b�v�t�@�C�����l�[��)
		for( ii=0; ii<=m_nMaxCycleCnt; ii++ ) {
			for( jj=m_nMaxBackCnt; jj>0; jj-- ) {
				cFileName.Format("%sbk_%s_@%d_%d.log", m_cBaseFolder, m_cBaseFile, jj-1, ii);	// �ϊ���
				cFileNameNew.Format("%sbk_%s_@%d_%d.log", m_cBaseFolder, m_cBaseFile, jj, ii);	// �ϊ���
				rename(cFileName, cFileNameNew);
			}
		}

		// xxx_0.log�`xxx_9.log �� bk_xxx_@0_0.log �` bk_xxx_@0_9.log �ɕύX (�ŐV�t�@�C�����o�b�N�A�b�v�t�@�C���Ƀ��l�[��)
		for( ii=0; ii<=m_nMaxCycleCnt; ii++ ) {
			cFileName.Format("%s%s_%d.log", m_cBaseFolder, m_cBaseFile, ii);						// �ϊ���
			cFileNameNew.Format("%sbk_%s_@0_%d.log", m_cBaseFolder, m_cBaseFile, ii);			// �ϊ���
			rename(cFileName, cFileNameNew);
		}

		// ���O�t�@�C���I�[�v��
		cFileName.Format("%s%s_%d.log", m_cBaseFolder, m_cBaseFile, m_nNowFileNo);
		m_stream.open(cFileName, std::ios::out | std::ios::trunc);
		m_cNowFPath = cFileName;
	}

	//// ���C���X���b�h�N��
	Start();
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
LogFileManager::~LogFileManager()
{
	// ���C���X���b�h��~
	Stop();				// �X���b�h�I��
	mque_Log.Free();	// �X���b�h�L���[�j��
	// �t�@�C���N���[�Y
	//if (m_stream != NULL) {			// ���������|�C���^����Ȃ�
	if ( m_stream.is_open() ) {
		// �N���[�Y
		m_stream.close();
	}
	// �J��
	DeleteCriticalSection(&m_csOutPut);
	DeleteCriticalSection(&m_csANC);
	
}

//------------------------------------------
// �񓯊��������݃G���g���[
// LogFileManager::LNO no �V���{��
// LPCSTR __cBuffer �ϔz�񃁃b�Z�[�W
//------------------------------------------
void LogFileManager::Write( LogFileManager::LNO no, LPCSTR __cBuffer, ... )
{
	char	cTyp[8];
	char	cMsg[20480];
    SYSTEMTIME time;
    GetLocalTime(&time);

	AutoLock autolock(&m_csANC);

	//// �ψ������A���b�Z�[�W�쐬
	memset( cMsg, 0x00, sizeof(cMsg));
	va_list	vmarker;
	va_start( vmarker, __cBuffer ); 				// �ό̈����̏�����
//	_vsnprintf( (char*)cMsg, _TRUNCATE, __cBuffer, vmarker );
	_vsnprintf( (char*)cMsg, sizeof(cMsg)-1, __cBuffer, vmarker );
	va_end( vmarker );								// �ό̈����̃��Z�b�g
	
	//// �V���{���t��
	memset( cTyp, 0x00, sizeof(cTyp));
	if ( no==em_NON ) {				// ����
	}else if ( no==em_MSG ) {
		strcpy(cTyp, "�� "); 
	}else if ( no==em_ERR ) {
		strcpy(cTyp, "�� "); 
	}else if ( no==em_INF ) {
		strcpy(cTyp, "�� "); 
	}else if ( no==em_WAR ) {
		strcpy(cTyp, "�� "); 
	}else if ( no==em_FIL ) {
		strcpy(cTyp, "�� "); 
	}

	//// �������ݗp�o�b�t�@�쐬 (�g���I�������K���J�����鎖)
	int		nSize;
	nSize = (int)strlen(cMsg) + 256;			// 256�� ���� �� �L���[�������Ƀ��b�Z�[�W ���Ŏg�p�����
	LPSTR pStr = new char [nSize];							// CString �� char*�ɕύX (�A���ŏ������ނ� 0.2ms ���� 0.1ms�ɒZ�k)
	// ���b�Z�[�W����
	sprintf(pStr, "%04d/%02d/%02d %02d:%02d:%02d.%03d %s%s",
		time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, 
		cTyp, cMsg);


	//// �L���[�ɓo�^
	// �L���[�c�e�ʃ`�F�b�N
	if( 0 == mque_Log.GetFreeCount() ) {			// �󂫂Ȃ�
		// �S���O�J��
		int cnt;
		cnt = AllDeleteQueue();
		// �񓯊��������݃��O�폜
		memset(pStr, 0x00, nSize);
		sprintf(pStr, "%04d/%02d/%02d %02d:%02d:%02d.%03d %s%s%d", 
			time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, 
			"�� ", "���O�L���[�t��!! ���O�폜����=", cnt);
	}
	// �L���[�ɓo�^
	mque_Log.AddToTail( pStr );		// str�͎擾��ɊJ��
}


//------------------------------------------
// ���������� (�ψ����Ή�)
// LogFileManager::LNO no �V���{��
// LPCSTR __cBuffer �ϔz�񃁃b�Z�[�W
//------------------------------------------
void LogFileManager::WriteLock( LogFileManager::LNO no, LPCSTR __cBuffer, ... )
{
	char	cTyp[8];
	char	cMsg[20480];
    SYSTEMTIME time;
    GetLocalTime(&time);

	AutoLock autolock(&m_csANC);

	//// �ψ������A���b�Z�[�W�쐬
	memset( cMsg, 0x00, sizeof(cMsg));
	va_list	vmarker;
	va_start( vmarker, __cBuffer ); 				// �ό̈����̏�����
	_vsnprintf( (char*)cMsg, sizeof(cMsg)-1, __cBuffer, vmarker );
	va_end( vmarker );								// �ό̈����̃��Z�b�g
	
	//// �V���{���t��
	memset( cTyp, 0x00, sizeof(cTyp));
	if ( no==em_NON ) {				// ����
	}else if ( no==em_MSG ) {
		strcpy(cTyp, "�� "); 
	}else if ( no==em_ERR ) {
		strcpy(cTyp, "�� "); 
	}else if ( no==em_INF ) {
		strcpy(cTyp, "�� "); 
	}else if ( no==em_WAR ) {
		strcpy(cTyp, "�� "); 
	}else if ( no==em_FIL ) {
		strcpy(cTyp, "�� "); 
	}

	//// �������ݗp�o�b�t�@�쐬 (�g���I�������K���J�����鎖)
	int		nSize;
	nSize = (int)strlen(cMsg) + 256;			// 256�� ���� �� �L���[�������Ƀ��b�Z�[�W ���Ŏg�p�����
	LPSTR pStr = new char [nSize];							// CString �� char*�ɕύX (�A���ŏ������ނ� 0.2ms ���� 0.1ms�ɒZ�k)
	// ���b�Z�[�W����
	sprintf(pStr, "%04d/%02d/%02d %02d:%02d:%02d.%03d %s%s",
		time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, 
		cTyp, cMsg);


	// ���X�g�{�b�N�X�ɒǉ�
	if( m_bListOut) WriteList(pStr);
	// �t�@�C���o��
	if( m_bFileOut) WriteFile(pStr);


	SAFE_DELETE_ARRAY( pStr );
}


//------------------------------------------
// ���䏑������
//------------------------------------------
void LogFileManager::WriteFile(LPCSTR pStr)
{
	AutoLock autolock(&m_csOutPut);

	////// �t�@�C���o��
	//// ���O�t�@�C���ɏ�������
    m_stream << pStr << std::endl;
	m_nNowRow++;

	if( m_nNowRow > KIZULIB_LOGFILEMANAGER_MAX_ROW_NUM ) {
		m_stream.close();
		m_nNowRow = 0;
		m_nNowFileNo++;
		if( (int)m_nNowFileNo > m_nMaxCycleCnt ) m_nNowFileNo = 0;
		CString cFileName;
		cFileName.Format("%s%s_%d.log", m_cBaseFolder, m_cBaseFile, m_nNowFileNo);
		m_stream.open(cFileName, std::ios::out | std::ios::trunc);
		m_cNowFPath = cFileName;
	}
}

//------------------------------------------
// ���X�g�{�b�N�X�ɏ�������
//------------------------------------------
void LogFileManager::WriteList(LPCSTR pStr)
{
	AutoLock autolock(&m_csOutPut);

	///// ���X�g�{�b�N�X�ɒǉ�
	try{
		if(m_cntList) {
			m_cntList->AddString(pStr);
			if( m_cntList->GetCount() > KIZULIB_LOGFILEMANAGER_MAX_LISTBOX ) m_cntList->DeleteString(0);
			m_cntList->SetCurSel( m_cntList->GetCount()-1);
		}
	} catch(int code) {
		code = code;
	}
}

//------------------------------------------
// �S�L���[�����o��
// �߂�l �o�͌���
//------------------------------------------
int LogFileManager::AllGetQueue()
{
	LPSTR pStr = 0;
	int cnt = 0;
	// �S�L���[���o��
	while (true) { 
		pStr = mque_Log.GetFromHead(0);
		if(pStr) {
			// ���X�g�{�b�N�X�ɒǉ�
			if( m_bListOut) WriteList(pStr);
			// �t�@�C���o��
			if( m_bFileOut) WriteFile(pStr);
			SAFE_DELETE_ARRAY( pStr ) ;
			cnt++;
		} else {
			break;
		}
	}
	return cnt;
}

//------------------------------------------
// �S�L���[�����폜
// �߂�l �폜����
//------------------------------------------
int LogFileManager::AllDeleteQueue()
{
	LPSTR pStr = 0;
	int cnt = 0;
	// �S�L���[���o��
	while (true) { 
		pStr = mque_Log.GetFromHead(0);
		if(pStr) {
			SAFE_DELETE_ARRAY( pStr );
			cnt++;
		} else {
			break;
		}
	}
	return cnt;
}

//------------------------------------------
// �������N��
//------------------------------------------
void LogFileManager::ShowEdit()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	GetStartupInfo(&si);
	si.dwFlags = 0;

	// �������N��
	char path[MAX_PATH];
	sprintf(path, "notepad.exe %s", GetNowLogPath());
	CreateProcess(NULL, path, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
	CloseHandle(pi.hProcess);	// �v���Z�X�n���h���N���[�Y
	CloseHandle(pi.hThread);	// �v���Z�X�E�X���b�h�E�n���h���N���[�Y
}


//------------------------------------------
// �X���b�h
//------------------------------------------
UINT WINAPI LogFileManager::MainThread(LPVOID param)
{
	LogFileManager *p = (LogFileManager *)param;
	p->SetTid();
//	p->my_Tid = GetCurrentThreadId();

	// �o�̓t���O
	bool blnList = p->m_bListOut;
	bool blnFile = p->m_bFileOut;

	////// �C�x���g�쐬
	// ��~�v��, DB�������ݏ��X���b�h�L���[
	HANDLE	hArray[2] = { p->my_evStop,
						  p->mque_Log.g_evSem};	// WaitForMultipleObjects�C�x���g

	enum { EV_STOP = WAIT_OBJECT_0,			// �I���ʒm
		   EV_QUE,							// �L���[
		   EV_END};
	
	// ���b�Z�[�W�L���[�ǂݍ���
	while( TRUE ) {
		LPSTR pStr = 0;
		// �V�O�i���ҋ@
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);
		
		// �o�̓t���O����
		blnList = p->m_bListOut;
		blnFile = p->m_bFileOut;

		switch (ret) {
		case EV_STOP :				// �X���b�h�I��
			//// �S���O�J��		
			p->AllDeleteQueue();
			return 0;

		case EV_QUE:				// ���O��������
			pStr = p->mque_Log.GetFromHead();
			if(!pStr) break;

			// ���X�g�{�b�N�X�ɒǉ�
			if( blnList) p->WriteList(pStr);
			// �t�@�C���o��
			if( blnFile) p->WriteFile(pStr);
			
			SAFE_DELETE_ARRAY( pStr );
			break;
		default:
			break;
		}
	}
}
