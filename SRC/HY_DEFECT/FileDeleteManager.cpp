#include "StdAfx.h"
#include "FileDeleteManager.h"
#include <io.h>

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
FileDeleteManager::FileDeleteManager(void):
mque_pFileDel(20),
ThreadManager("FileDelMgr")
{
	//// �n���h������
	m_evRuning		= CreateEvent(NULL, TRUE, FALSE, NULL);		// �}�j���A�����Z�b�g
	m_evRunEnding	= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evRunEnd		= CreateEvent(NULL, FALSE, FALSE, NULL);

	// ������
	m_bRun = false;
	memset(m_cPath, 0x00, sizeof(m_cPath));
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
FileDeleteManager::~FileDeleteManager(void)
{
	CloseHandle(m_evRuning);
	CloseHandle(m_evRunEnding);
	CloseHandle(m_evRunEnd);
}


//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int FileDeleteManager::ThreadFirst()
{
	//// ���O����

	//// �C�x���g�ݒ�
	HANDLE hArray[] = { m_evRunEnding,
						m_evRuning,
						mque_pFileDel.g_evSem
						};

	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	//// �ǉ�
	//this->AddEventNum(1, &mque_pFileDel.g_evSem);
	return 0;
}
//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int FileDeleteManager::ThreadLast()
{
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void FileDeleteManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_RUN_ENDING = 0,				// ���s���f�v���V�O�i��
			EV_RUN,							// �t�@�C���폜
			EV_QUE							// �t�@�C���폜(QUE)
	};

	TYP_DELETE_FILE* pDeli = NULL;				// �󂯓n�����

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_RUN_ENDING:		
		// �폜�������������������Ă��Ȃ��ꍇ
		ResetEvent(m_evRuning);
		SetEvent(m_evRunEnd);
		m_bRun = false;
		break;

//-----------------------------------------------------------------------------------------------
	case EV_RUN:
		DeleteExec();
		break;

//-----------------------------------------------------------------------------------------------
	case EV_QUE:
		pDeli = mque_pFileDel.GetFromHead();
		DeleteExecQue(pDeli->cPath);
		delete pDeli;
		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����

//------------------------------------------
// �폜�J�n
// char const* pPath �t�H���_�p�X (.\\temp\\rec0)
//------------------------------------------
bool FileDeleteManager::DeleteStart(char const* pPath)
{
	bool bRetc = true;

	// �O�񂪂܂������Ă���H
	if (m_bRun) {
		SetEvent(m_evRunEnding);
		// �Ƃ܂�܂ő҂�
		if (WAIT_TIMEOUT == WaitForSingleObject(m_evRunEnd, 100)) {
			LOG(em_WAR), "[%s] ���s��~�^�C���A�E�g <%s>", my_sThreadName, m_cPath );
			bRetc = false;
		}
	}

	// �폜�J�n
	memset(m_cPath, 0x00, sizeof(m_cPath));
	strcpy(m_cPath, pPath);
	SetEvent(m_evRuning);
	return bRetc;
}

//------------------------------------------
// �폜�J�n(QUE)
// char const* pPath �t�H���_�p�X (.\\temp\\rec0)
//------------------------------------------
bool FileDeleteManager::DeleteAll(char const* pPath)
{
	bool bRetc = true;
	TYP_DELETE_FILE* DelPath = new TYP_DELETE_FILE;
	memset(DelPath, 0x00, sizeof(TYP_DELETE_FILE));
	strcpy(DelPath->cPath , pPath);

	bRetc=mque_pFileDel.AddToTail(DelPath);
	return bRetc;
}

//------------------------------------------
// �폜����
//------------------------------------------
void FileDeleteManager::DeleteExec()
{
	BOOL			bRun		= TRUE;
	int				nStopMode	= 0;			// 0:���� 1:�L�����Z�� -1:�ُ�
	int				ii=0;
	//CTime			tBase= CTime::GetCurrentTime() - CTimeSpan(1,0,0,0);			// 1���O
	CTime			tBase = CTime::GetCurrentTime() - CTimeSpan(0, 12, 0, 0);		// 12���ԑO
	//CTime			tBase = CTime::GetCurrentTime() - CTimeSpan(0, 0, 0, 0);		// 0���ԑO
	CString			cPath;						// �󂯓n���p�X
	CString			cFileName;					// ���������t�@�C���̃t���p�X
	CString			cSearchFile;				// �t�H���_���̃t�@�C������������L�[
	CString			sWk;
	std::deque<CString> queDeleteFile;			// �폜�t�@�C��



	//===================================
	// ���O����
	m_bRun = true;
	cPath  = m_cPath;
	cSearchFile.Format( "%s\\*.*", cPath );


	//===================================
	// �폜�Ώۃt�@�C�������
	WIN32_FIND_DATA	fd;
	HANDLE			hFind;
	memset(&fd, 0x00, sizeof(fd));
	hFind = FindFirstFile(cSearchFile, &fd);
	while(hFind != INVALID_HANDLE_VALUE) {
		//// �t�@�C���̂�
		if( FILE_ATTRIBUTE_DIRECTORY != fd.dwFileAttributes ) {
			// �w����ȏ�o�߂����t�@�C��������
			if( tBase > CTime(fd.ftLastWriteTime) ) {

				queDeleteFile.push_back( fd.cFileName );
			}
		}

		//// ��
		bRun = FindNextFile(hFind, &fd);
		if(!bRun) {
			if(ERROR_NO_MORE_FILES != GetLastError()) {
				nStopMode = -1;
				LOG(em_MSG), "[%s] �t�@�C���폜���s<%s>", my_sThreadName, fd.cFileName);
			}
			break;
		}

		//// ��~�v����F������
		if(WAIT_TIMEOUT != WaitForSingleObject(m_evRunEnding, 0)) {
			// ��~�v���L��Ȃ̂Œ��f
			nStopMode = 1;
			break;
		}
	}
	FindClose(hFind);



	//===================================
	// �폜�Ώۃt�@�C�����폜
	int nCnt = (int)queDeleteFile.size();
	for(ii=0; ii<nCnt; ii++ ) {
		sWk.Format("%s\\%s", m_cPath, queDeleteFile[ii]);
		DeleteFile(sWk);

		//// ��~�v����F������
		if(WAIT_TIMEOUT != WaitForSingleObject(m_evRunEnding, 0)) {
			// ��~�v���L��Ȃ̂Œ��f
			nStopMode = 1;
			break;
		}
	}

	LOG(em_MSG), "[%s] �t�@�C���폜���� <%s=%d><%d><%d>", my_sThreadName, cPath, nCnt, nStopMode, ii);



	//===================================
	// �폜��~
	ResetEvent(m_evRuning);
	m_bRun = false;
	if(1 == nStopMode) SetEvent(m_evRunEnd);
}
//------------------------------------------
// �폜����(QUE)
//------------------------------------------
void FileDeleteManager::DeleteExecQue(char const* pPath)
{
	BOOL			bRun = TRUE;
	//int				nStopMode = 0;			// 0:���� 1:�L�����Z�� -1:�ُ�
	int				ii = 0;
	//CTime			tBase= CTime::GetCurrentTime() - CTimeSpan(1,0,0,0);			// 1���O
	CTime			tBase = CTime::GetCurrentTime() - CTimeSpan(0, 12, 0, 0);		// 12���ԑO
	CString			cPath;						// �󂯓n���p�X
	CString			cFileName;					// ���������t�@�C���̃t���p�X
	CString			cSearchFile;				// �t�H���_���̃t�@�C������������L�[
	CString			sWk;
	std::deque<CString> queDeleteFile;			// �폜�t�@�C��



												//===================================
												// ���O����
	cPath = pPath;
	cSearchFile.Format("%s\\*.*", cPath);
	LOG(em_MSG), "[%s] �t�@�C���폜�J�n(QUE) <%s>", my_sThreadName, cSearchFile);


	//===================================
	// �폜�Ώۃt�@�C�������
	WIN32_FIND_DATA	fd;
	HANDLE			hFind;
	memset(&fd, 0x00, sizeof(fd));
	hFind = FindFirstFile(cSearchFile, &fd);
	while (hFind != INVALID_HANDLE_VALUE) {
		//// �t�@�C���̂�
		if (FILE_ATTRIBUTE_DIRECTORY != fd.dwFileAttributes) {
			// �w����ȏ�o�߂����t�@�C��������
			if (tBase > CTime(fd.ftLastWriteTime)) {

				queDeleteFile.push_back(fd.cFileName);
			}
		}

		//// ��
		bRun = FindNextFile(hFind, &fd);
		if (!bRun) {
			if (ERROR_NO_MORE_FILES != GetLastError()) {
				LOG(em_MSG), "[%s] �t�@�C���폜���s<%s>", my_sThreadName, fd.cFileName);
			}
			break;
		}
	}
	FindClose(hFind);



	//===================================
	// �폜�Ώۃt�@�C�����폜
	int nCnt = (int)queDeleteFile.size();
	for (ii = 0; ii<nCnt; ii++) {
		sWk.Format("%s\\%s", cPath, queDeleteFile[ii]);
		DeleteFile(sWk);
	}

	LOG(em_MSG), "[%s] �t�@�C���폜����(QUE) <%s=%d><%d>", my_sThreadName, cPath, nCnt,ii);
}
