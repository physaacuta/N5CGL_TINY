#include "StdAfx.h"
#include "HddManager.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �{���̕�
//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
HddManager::HddManager(void) :
ThreadManager("HddMgr")
{
	//// �n���h������
	m_evTimerCheck	  = CreateWaitableTimer(NULL, FALSE, NULL);

	//// ���̑�������
	for(int ii=0; ii<MAX_INS; ii++) m_bSaveFolderAccess[ii] = false;
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
HddManager::~HddManager(void)
{
	//// �n���h�����
	CloseHandle(m_evTimerCheck);
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int HddManager::ThreadFirst()
{
	ParamManager::P_MAIN		const& prm	= mcls_pParam->GetMain();


	//// �t�H���_����
	for(int ii=0; ii<prm.nIns; ii++) {
		ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[ ii ];
		// 0:OK  EEXIST:�����L��  ENOENT:�p�X����	
		if( 0 != _mkdir( prmI.cRawSavePath ) ) {
			int* nErr = _errno();
			if( ENOENT == *nErr) { LOG(em_ERR), "[%s] �t�H���_�������s(%s)", my_sThreadName, prmI.cRawSavePath); }
		}

		for(int jj=0; jj<prmI.nRpiFolderBufNum; jj++)  { 
			if( 0 != _mkdir( prmI.cRpiFolderBuf[jj] ) ) {
				int* nErr = _errno();
				if( ENOENT == *nErr) { LOG(em_ERR), "[%s] �t�H���_�������s(%s)", my_sThreadName, prmI.cRpiFolderBuf[jj]); }
			}
		}
		for(int jj=0; jj<prmI.nRpiFolderSaveNum; jj++) { 
			if( 0 != _mkdir( prmI.cRpiFolderSave[jj] ) ) {
				int* nErr = _errno();
				if( ENOENT == *nErr) { LOG(em_ERR), "[%s] �t�H���_�������s(%s)", my_sThreadName, prmI.cRpiFolderSave[jj]); }
			}
		}
	}


	//// ���[�N�̈�ɂ��� �O��̎c�[���폜
	FastDelete();

	//// �ۑ��t�H���_ �A�N�Z�X�`�F�b�N
	if( ! AccessSaveFolder(-1) ) {
		// �����`�F�b�N
		StartTimer_SaveFolder();
	}

	//// �󂫗e�ʃ`�F�b�N
	for(int ii=0; ii<prm.nIns; ii++) {
		ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[ii];

		//// �ŌÃt�@�C���̃t�H���_�̂ݍ폜
		for(int jj=0; jj<prmI.nRpiFolderBufNum; jj++) {
			FreeSpace( prmI.cRpiFolderBuf[jj], prm.nHddFreeSizeLocal );
		}
	}
	 

	//// �C�x���g�ݒ�
	HANDLE hArray[] = { m_evTimerCheck,
						mque_pRpiMove->g_evSem
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int HddManager::ThreadLast()
{
	//// �L���[���
	RPI_MOVE* pDeli;
	while(true) {
		pDeli = mque_pRpiMove->GetFromHead(0);
		if( pDeli == NULL ) break;
		delete pDeli;
	}
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void HddManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_TIEM_CHECK = 0,				// �ۑ��t�H���_�A�N�Z�X�`�F�b�N
			EV_ADD_QUE,						// ��n���L���[
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_TIEM_CHECK: 					// �ۑ��t�H���_�A�N�Z�X�`�F�b�N
		if( ! AccessSaveFolder(-1) ) {
			StartTimer_SaveFolder();
		} else {	
			BackUpCopy();
		}	
		return;

//-----------------------------------------------------------------------------------------------
	case EV_ADD_QUE: 						// ��n���L���[
		if(true) {
			RPI_MOVE* pDeli = mque_pRpiMove->GetFromHead();
			if( Exec(pDeli) ) {										// ���s
				if( 0 == mque_pRpiMove->GetCount() ) BackUpCopy(pDeli);	// �o�b�N�A�b�v�ړ�
			} else {
				m_bSaveFolderAccess[ pDeli->nIns ] = false; 
				StartTimer_SaveFolder();
			}
			delete pDeli;
		}
		return;
	
//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �O������

//------------------------------------------
// ���C������
// RPI_MOVE const* p ��n�����
//------------------------------------------
bool HddManager::Exec(RPI_MOVE const* p)
{
	CString sWk;	
	ParamManager::P_MAIN		const& prm	= mcls_pParam->GetMain();
	ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[ p->nIns ];


	//// ���ڑ����
	if( ! m_bSaveFolderAccess[p->nIns] ) goto Ending;


	//// �ŐV�t�@�C���̃t�H���_�����
	int nIndex = SearchFolderIndex(p->nIns, true);	


	//// �󂫗e�ʃ`�F�b�N
	//		�ŐV�t�@�C���̃t�H���_�ɋ󂫂����邩�H
	//		�󂫂������ꍇ�A���̃t�H���_�ɋ󂫂����邩�H(�܂��������ĂȂ��ꍇ�����邽�߂̏��u)
	//		�ŌÃt�@�C���̃t�H���_�ɋ󂫂����B
	if( ! CheckFreeSpace( prmI.cRpiFolderSave[nIndex], prm.nHddFreeSizeSave) ) {

		if( 1 < prmI.nRpiFolderSaveNum ) {
			// ���̃t�H���_�͋󂢂Ă��邩�B�܂��������ĂȂ��ꍇ�����邽�߂̏��u
			nIndex ++;
			if(nIndex >= prmI.nRpiFolderSaveNum ) nIndex = 0;
			if( ! CheckFreeSpace( prmI.cRpiFolderSave[nIndex], prm.nHddFreeSizeSave) ) {
				// ���Ɉ������Ă���P�[�X�B
				// �ŌÂ���肵�āA�ŌÃt�@�C���̃t�H���_�ɑ΂��ċ󂫗̈���m��
				nIndex = SearchFolderIndex(p->nIns, false);	
				if( ! FreeSpace(prmI.cRpiFolderSave[nIndex], prm.nHddFreeSizeSave) )goto Ending;
			}
		} else {
			// 1�����Ȃ����߁A���̃t�H���_�̋󂫗̈���m��
			if( ! FreeSpace(prmI.cRpiFolderSave[nIndex], prm.nHddFreeSizeSave) ) goto Ending;
		}
	}

	//// �R�s�[���t�@�C���������������邩�H (2�񗈂�\�������邽�߂̏��u)
	// �t�@�C����
	sWk.Format("%s\\%s", p->sPath_F, p->sName );
	if( ! GetFileUmu(sWk) ) {
		LOG(em_WAR), "[%s] �ړ��O�Ŋ��ɃR�s�[���t�@�C������ %s", my_sThreadName, sWk);
		goto Ending;
	}


	// ���ʂ̃t�@�C���ړ�
	if( ! CopyFileExec(p, nIndex) ) {

		//// ���g���C����
		
		// �ړ���ɓ��ꖼ�̂�������
		for(int ii=0; ii<prmI.nRpiFolderSaveNum; ii++) {
			sWk.Format("%s\\%s", prmI.cRpiFolderSave[ii], p->sName );
			if( DeleteFile(sWk) ) {
				LOG(em_WAR), "[%s] �ړ��O�̐������폜���� %s", my_sThreadName, sWk);
			}
		}
		
		// �ړ�
		if( ! CopyFileExec(p, nIndex) ) goto Ending;
	}

Ending:
	//// ���̕ۑ��̂��߂� ���̈���`�F�b�N
	FreeSpace(p->sPath_F, prm.nHddFreeSizeLocal);
	return true;
}


//------------------------------------------
// �S���摜�ۑ�PC�̋󂫃G���A���m��
// char const* sFolder �Ώۃt�H���_ (�Ō��\�͖���)
//------------------------------------------
bool HddManager::FreeSpace(char const* sFolder, const int nFreeSpaceGB)
{
	double					dNowFreeSize;	// ���݂̋󂫗e�� [GB]
	std::vector<RPI_ITEM>	vItem;			// RPI�t�@�C���ꗗ
	CString	sWk;

	ParamManager::P_MAIN	 const& prm	 = mcls_pParam->GetMain();


	// =========================================================
	// �󂫗e�ʂ��`�F�b�N
	// �Ȃ񂩃l�b�g���[�N�o�R���ƍŌ��\��t���Ȃ��ƃ_�����ۂ�
	dNowFreeSize = CheckFreeSpace( sFolder );
	if( 0 > (int)dNowFreeSize ) return true;					// �T�C�Y�擾�ُ�Ȃ̂ŁA����
	if(nFreeSpaceGB <= (int)dNowFreeSize ) return true;	// �Œ�󂫗e�ʂ����傫���H


	// =========================================================
	// �t�H���_���̌Â����̏���RPI�t�@�C���ꗗ�𐶐�
	
	//// �t�H���_����RPI�t�@�C���ꗗ���擾
	sWk.Format("%s\\*.%s", sFolder, RPI_TYPE );
	WIN32_FIND_DATA	fd;
	HANDLE			hFind;
	memset(&fd, 0x00, sizeof(fd));
	hFind = FindFirstFile(sWk, &fd);
	while(hFind != INVALID_HANDLE_VALUE) {
		// �t�@�C���̂�
		if( FILE_ATTRIBUTE_DIRECTORY != fd.dwFileAttributes ) {
			// ���X�g�ɒǉ�
			RPI_ITEM t;
			t.sName				= fd.cFileName;
			t.tTime				= fd.ftLastAccessTime;
			t.nSize.HighPart	= fd.nFileSizeHigh;
			t.nSize.LowPart		= fd.nFileSizeLow;
			vItem.push_back(t);
		}
		// ��
		if(! FindNextFile(hFind, &fd)) {
			if(ERROR_NO_MORE_FILES != GetLastError()) {
				LOG(em_ERR), "[%s] �t�@�C���ꗗ�擾���s %d", my_sThreadName, GetLastError());
			}
			break;
		}
	}
	FindClose(hFind);

	//// �Â����Ƀ\�[�g
	std::sort(vItem.begin(), vItem.end(), sort_fd_old);


	// =========================================================
	// �Œ�󂫗e�� �ȏ�ɂȂ�܂� �Â�RPI�t�@�C�����폜
	double dWk;
	double dTotalDelSize = 0.0;				// �폜�t�@�C���T�C�Y [GB]	
	for(int ii=0; ii<(int)vItem.size()-prm.nIns; ii++) {				// ���߂̐��t�@�C���͐�΂ɏ����Ȃ����ƁB�����������玩����������Ȃ����E�E�E

		dWk = vItem[ii].nSize.QuadPart  /1024.0/1024.0/1024.0;	// [B��GB]
		sWk.Format("%s\\%s", sFolder, vItem[ii].sName );
		DeleteFile(sWk);
		int nWk = GetLastError();
		LOG(em_INF), "[%s] RPI̧�ٍ폜 [%.3fGB](�A�N�Z�X���t=%s)(%s) retc=%d", my_sThreadName, dWk, vItem[ii].tTime.Format("'%Y/%m/%d %H:%M:%S'"), sWk, nWk);

		// ���̂���
		dTotalDelSize += dWk;
		if( 2<=ii ) {   //���Ȃ��Ƃ�3�t�@�C�����炢�͏���
			if(nFreeSpaceGB <= (int)(dNowFreeSize+dTotalDelSize) ) break;
		}
	}

	// �������`�F�b�N
	if(nFreeSpaceGB> (int)(dNowFreeSize+dTotalDelSize) ) {
		LOG(em_ERR), "[%s] �S��RPI�t�@�C���폜�����̂ɁA�Œ�󂫗e��[%dGB]�������I", my_sThreadName, nFreeSpaceGB, GetLastError());
		return false;
	}

	return true;
}

//------------------------------------------
// ���ʂ̃t�@�C���ړ�
// RPI_MOVE const* p ��n�����
// int nHddIndex HDDIndex
//------------------------------------------
bool HddManager::CopyFileExec(RPI_MOVE const* p, int nHddIndex)
{
	CString			sPathSrc,	sPathDst;				// �t���p�X
	ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[ p->nIns ];

	//// �p�X����
	sPathSrc.Format("%s\\%s", p->sPath_F, p->sName );
	sPathDst.Format("%s\\%s", prmI.cRpiFolderSave[nHddIndex], p->sName );


	//// �ړ�
	if( MoveFile(sPathSrc, sPathDst) ) {
	//if( MoveFileEx(sPathSrc, sPathDst, MOVEFILE_REPLACE_EXISTING) ) {
		LOG(em_MSG), "[%s] RPI̧�وړ����� [%s]", my_sThreadName, sPathSrc);
		return true;
	} else {
		LOG(em_ERR), "[%s] RPI̧�وړ����s [%s][%d]", my_sThreadName, sPathSrc, GetLastError());
		return false;
	}
}

//------------------------------------------
// �����N������ �S�~�폜
//------------------------------------------
void HddManager::FastDelete()
{
	//// �ʏ햳���͂������A�ꎟ���[�N�̈�� *.rpiwk ���c�[�Ƃ��Ďc���Ă���ꍇ�A�폜����
	std::vector<CString>		vItem;			// RPI�t�@�C���ꗗ
	CString						sWk;
	WIN32_FIND_DATA				fd;
	HANDLE						hFind;
	ParamManager::P_MAIN		const& prm	= mcls_pParam->GetMain();


	for(int ii=0; ii<prm.nIns; ii++) {
		ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[ii];

		//// �t�H���_����RPI���[�N�t�@�C���ꗗ���擾
		sWk.Format("%s\\*.%s", prmI.cRpiFolderBuf, RPI_TYPE_WK );
		memset(&fd, 0x00, sizeof(fd));
		hFind = FindFirstFile(sWk, &fd);
		while(hFind != INVALID_HANDLE_VALUE) {
			// �t�@�C���̂�
			if( FILE_ATTRIBUTE_DIRECTORY != fd.dwFileAttributes ) {
				// ���X�g�ɒǉ�
				vItem.push_back(fd.cFileName);
			}
			// ��
			if(! FindNextFile(hFind, &fd)) {
				if(ERROR_NO_MORE_FILES != GetLastError()) {
					LOG(em_ERR), "[%s] �t�@�C���ꗗ�擾���s %d", my_sThreadName, GetLastError());
				}
				break;
			}
		}
		FindClose(hFind);

		//// �폜
		for(int ii=0; ii<(int)vItem.size(); ii++) {
			sWk.Format("%s\\%s", prmI.cRpiFolderBuf, vItem[ii] );
			DeleteFile(sWk);
			LOG(em_WAR), "[%s] RPIܰ�̧�ٍ폜 (%s)", my_sThreadName, sWk);
		}

		//// ����
		vItem.clear(); 
	}
}

//------------------------------------------
// �ꎟ�t�H���_�ɂ���RPI�t�@�C�����ړ� (���펞�ł͑Ώۃt�@�C���͖����͂�)
//------------------------------------------
void HddManager::BackUpCopy(RPI_MOVE const* p)
{

	//// �ʏ햳���͂������A�ꎟ���[�N�̈�� *.rpi ���c���Ă���ꍇ�A�ړ�����
	std::vector<CString>		vItem;			// RPI�t�@�C���ꗗ
	CString						sWk;
	WIN32_FIND_DATA				fd;
	HANDLE						hFind;
	ParamManager::P_MAIN		const& prm	= mcls_pParam->GetMain();

	CString sMyFile;
	if(NULL != p) sMyFile = p->sName;

	for(int ii=0; ii<prm.nIns; ii++) {
		if( ! m_bSaveFolderAccess[ii] ) continue;

		ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[ii];

		for(int rec=0; rec<prmI.nRpiFolderBufNum; rec++) {

			//// �t�H���_����RPI���[�N�t�@�C���ꗗ���擾
			sWk.Format("%s\\*.%s", prmI.cRpiFolderBuf[rec], RPI_TYPE );
			memset(&fd, 0x00, sizeof(fd));
			hFind = FindFirstFile(sWk, &fd);
			while(hFind != INVALID_HANDLE_VALUE) {
				// �t�@�C���̂�
				if( FILE_ATTRIBUTE_DIRECTORY != fd.dwFileAttributes ) {
	//				if( 0 < strcmp(sMyFile, fd.cFileName ) ) {		// ��������������������Ώ�
					if( 0 != strcmp(sMyFile, fd.cFileName ) ) {		// �����͑ΏۊO
						// ���X�g�ɒǉ�
						LOG(em_WAR), "[%s] RPI̧�� �ޯ����߈ړ��ΏۗL�� (%s)", my_sThreadName, fd.cFileName);
						vItem.push_back(fd.cFileName);
					}
				}
				// ��
				if(! FindNextFile(hFind, &fd)) {
					if(ERROR_NO_MORE_FILES != GetLastError()) {
						LOG(em_ERR), "[%s] �t�@�C���ꗗ�擾���s %d", my_sThreadName, GetLastError());
					}
					break;
				}
			}
			FindClose(hFind);

			//// �����ۑ��ꏊ�ɓ���t�@�C��������΁A��x����
			// ���r���[��RPI�t�@�C�����ł��Ă���ꍇ������΍�
			for(int jj=0; jj<(int)vItem.size(); jj++) {
				for(int kk=0; kk<prmI.nRpiFolderSaveNum; kk++) {
					sWk.Format("%s\\%s", prmI.cRpiFolderSave[kk], vItem[jj] );
					if( DeleteFile(sWk) ) {
						LOG(em_WAR), "[%s] �o�b�N�A�b�v�폜���� %s", my_sThreadName, sWk);
						//break;	// �����������畡�����ɑ��݂��邩���m��Ȃ��̂ŁA�ꉞ�S���`�F�b�N
					}
				}
			}

			//// �ړ�
			for(int jj=0; jj<(int)vItem.size(); jj++) {
				if( 0 != mque_pRpiMove->GetCount() ) return;	// �������s�����̂�h�~?�Ȃ������ɂ��ꂪ����̂����܂����o���Ă��Ȃ��B

				// ��n���f�[�^����
				RPI_MOVE* p = new RPI_MOVE;
				p->nIns		= ii;	
				p->sName	= vItem[jj];
				p->sPath_F	= prmI.cRpiFolderBuf[rec];

				// ���s
				Exec(p);

				// ��n��
				delete p;
			}
			vItem.clear(); 
		}
	}
}
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����

//// �\�[�g�̔�r�֐�
//------------------------------------------
// �Â����������BA���Â��ꍇTrue
//------------------------------------------
inline bool HddManager::sort_fd_old(const RPI_ITEM &a, const RPI_ITEM &b)
{
	return a.tTime < b.tTime;
}
//------------------------------------------
// �V�������������BA���V�����ꍇTrue
//------------------------------------------
inline bool HddManager::sort_fd_new(const RPI_ITEM &a, const RPI_ITEM &b)
{
	return a.tTime > b.tTime;
}

//------------------------------------------
// �����ۑ��p�t�H���_�ɃA�N�Z�X�\���`�F�b�N
// int nIns �ΏۃC���X�^���X -1:�S��  0�`:�ΏۃC���X�^���X�̂�
// �߂�l -1���F�S��OK��true  0�`���F�ΏۃC���X�^���X�̏��
//------------------------------------------
bool HddManager::AccessSaveFolder(int nIns)
{
	int ii_s, ii_e;
	bool bRetc = true;	// �S�̂�ʂ���1�ł�NG����
	bool bAccess;

	ParamManager::P_MAIN		const& prm	= mcls_pParam->GetMain();

	//// �Ώی���
	if(-1==nIns) { ii_s=0; ii_e=prm.nIns-1; }
	else		 { ii_s=ii_e=nIns;}

	//// �A�N�Z�X�`�F�b�N
	for(int ii=ii_s; ii<=ii_e; ii++) {
		ParamManager::P_MAIN::INS    const& prmI = mcls_pParam->GetMain().ins[ ii ];
		bAccess = true;
		// �t�H���_�`�F�b�N
		// D:\ �ł� D: �ł��ǂ���ł�OK
		for(int jj=0; jj<prmI.nRpiFolderSaveNum; jj++) {
			if( ! PathIsDirectory( prmI.cRpiFolderSave[jj] ) ) {
				LOG(em_WAR), "[%s] �����ۑ��p�t�H���_�A�N�Z�X���s [%s]", my_sThreadName, prmI.cRpiFolderSave[jj] );	
				bAccess = false;
				bRetc = false;
			}
		}
		// ���ʃZ�b�g
		m_bSaveFolderAccess[ii] = bAccess;
	}
	return bRetc;
}

//------------------------------------------
// �����ۑ��p�t�H���_�ɃA�N�Z�X�\�`�F�b�N�^�C�}�[
//------------------------------------------
void HddManager::StartTimer_SaveFolder()
{
	__int64 ts = FOLDER_CHECK_TIME * -10000;		// 1�~���b��100�i�m�b (�}�C�i�X�w��ŁA�w�莞�Ԍo�߂ŃV�O�i��) (��3������2��ڈȍ~�̋N���Ԋu)
	SetWaitableTimer(m_evTimerCheck, (LARGE_INTEGER *)&ts, 0, NULL, NULL, FALSE);
}


//------------------------------------------
// ����h���C�u���`�F�b�N
// char const* cPath1
// char const* cPath2
// �߂�l�F����h���C�u�� ture
//------------------------------------------
bool HddManager::RootFolderCheck(char const* cPath1, char const* cPath2)
{
	char cWk1[MAX_PATH];
	char cWk2[MAX_PATH];
	
	memset(cWk1, 0x00, sizeof(cWk1));
	memset(cWk2, 0x00, sizeof(cWk2));
	strcpy(cWk1, cPath1);
	strcpy(cWk2, cPath2);

	// �Ō��\�}�[�N��t�^�B(d: �̏ꍇ����Ɏ��Ȃ���)
	PathAddBackslash(cWk1);
	PathAddBackslash(cWk2);

	// ��r
	return PathIsSameRoot(cWk1, cWk2);
}


//------------------------------------------
// �S���摜�ۑ�PC�̋󂫃G���A�`�F�b�N
// char const* sFolder �Ώۃt�H���_ (�Ō��\�͖���)
// �߂�l	0�`�F���݂̋󂫗e�� [GB]	0>:���s
//------------------------------------------
double HddManager::CheckFreeSpace(char const* sFolder)
{
	CString	sWk;
	ParamManager::P_MAIN	 const& prm	 = mcls_pParam->GetMain();

	// =========================================================
	// �󂫗e�ʂ��`�F�b�N
	// �Ȃ񂩃l�b�g���[�N�o�R���ƍŌ��\��t���Ȃ��ƃ_�����ۂ�
	sWk.Format("%s\\", sFolder );

	// �󂫃T�C�Y�擾
	ULARGE_INTEGER n1;		// �g�p�\�o�C�g��
	ULARGE_INTEGER n2;		// �S�o�C�g��
	ULARGE_INTEGER n3;		// �󂫃o�C�g��
	if( ! GetDiskFreeSpaceEx( sWk, &n1, &n2, &n3 )) {
		int nRetc = GetLastError();
		LOG(em_ERR), "[%s] �󂫃T�C�Y�擾���s =%d [%s]", my_sThreadName, nRetc, sWk);
		return -1;
	}

	double dNowFreeSize = n3.QuadPart /1024.0/1024.0/1024.0;	// ���݂̋󂫗e�� [GB] [B��GB]
	return dNowFreeSize;
}

//------------------------------------------
// �S���摜�ۑ�PC�̋󂫃G���A�`�F�b�N
// char const* sFolder �Ώۃt�H���_ (�Ō��\�͖���)
// double dLimit �󂫗e�� [GB]
// �߂�l	true:����
//------------------------------------------
bool HddManager::CheckFreeSpace(char const* sFolder, double dLimit)
{
	double	dNowFreeSize = CheckFreeSpace(sFolder);		// ���݂̋󂫗e�� [GB]
	if( 0 > dNowFreeSize ) return true;					// �擾���s�̏ꍇ�́A����F���B

	if( dLimit <= (int)dNowFreeSize ) return true;		// �Œ�󂫗e�ʂ����傫���H
	return false;
}

//------------------------------------------
// �V���t�@�C���̃t�H���_Index���Z�o
// int nIns �ΏۃC���X�^���X 0�`:�ΏۃC���X�^���X�̂�
// bool bIsNew		ture:�ŐV�Z�o false:�ŌÎZ�o
// �߂�l�F�t�H���_Index
//------------------------------------------
int HddManager::SearchFolderIndex(int nIns, bool bIsNew)
{
	int		nIndex = 0;
	CTime	tm;
	CString sWk;

	//// �O����
	ParamManager::P_MAIN::INS    const& prmI = mcls_pParam->GetMain().ins[ nIns ];
	if( 1 >= prmI.nRpiFolderSaveNum ) return 0;			// 1�����Ȃ��ꍇ�́A�������Ɋm��
	if(bIsNew)	tm = 0;
	else		tm = CTime::GetCurrentTime();

	//// �Ώۃt�H���_�T�[�`
	for(int ii=0; ii<prmI.nRpiFolderSaveNum; ii++) {
	
		// �t�H���_����RPI�t�@�C���ꗗ���擾
		sWk.Format("%s\\*.%s", prmI.cRpiFolderSave[ii], RPI_TYPE );
		WIN32_FIND_DATA	fd;
		HANDLE			hFind;
		memset(&fd, 0x00, sizeof(fd));
		hFind = FindFirstFile(sWk, &fd);
		while(hFind != INVALID_HANDLE_VALUE) {
			// �t�@�C���̂�
			if( FILE_ATTRIBUTE_DIRECTORY != fd.dwFileAttributes ) {
				// �ǂ�
				if( ( bIsNew && (tm < fd.ftLastAccessTime) ) ||
					(!bIsNew && (tm > fd.ftLastAccessTime) ) ) {
					
					tm		= fd.ftLastAccessTime;
					nIndex	= ii; 
				}
			}
			// ��
			if(! FindNextFile(hFind, &fd)) {
				if(ERROR_NO_MORE_FILES != GetLastError()) {
					LOG(em_ERR), "[%s] �t�@�C���ꗗ�擾���s %d", my_sThreadName, GetLastError());
				}
				break;
			}
		}
		FindClose(hFind);
	}
	
	LOG(em_INF), "[%s] Index=%d���ΏہB�A�N�Z�X���t(%s) ", my_sThreadName, nIndex, tm.Format("'%Y/%m/%d %H:%M:%S'"));
	return nIndex;
}



//------------------------------------------
// �t�@�C���L�����`�F�b�N
// char const* cFPath �擾�������t�@�C���̃t���p�X
// �߂�l: false:�ُ�  true:�t�@�C���L��
//------------------------------------------
bool HddManager::GetFileUmu(char const* cFPath)
{
	//// �t�@�C���֌W �̃f�[�^���O����
	FILE*	fp = NULL;			// �t�@�C���|�C���^
	fpos_t	size;				// �t�@�C���T�C�Y

	// �t�@�C���I�[�v��
	fp = fopen(cFPath, "rb");
	if(fp == NULL){        // �I�[�v�����s
		return false;
	}
	// �t�@�C���T�C�Y�擾
	fclose(fp);
	
	return true;
}





