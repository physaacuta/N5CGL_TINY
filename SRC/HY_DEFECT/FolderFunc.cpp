#include "StdAfx.h"
#include "FolderFunc.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------


using namespace KizuLib;

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
FolderFunc::FolderFunc(void) :
FuncBase("FolderFunc")
{
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
FolderFunc::~FolderFunc(void)
{
}


//------------------------------------------
// �T�u�f�B���N�g���܂ň�C�ɍ쐬
// char const* cPath �t���p�X  ���Ō��\ �K�{
// �߂�l ���A���  ���J��Ԃ�����Ă��ُ�ɂȂ�Ȃ�
//------------------------------------------
bool FolderFunc::CreateFolder(char const* cPath)
{
	if( MakeSureDirectoryPathExists(cPath) ) return true;
	else return false;
}


//------------------------------------------
// �t�H���_�L���`�F�b�N
// char const* cPath �`�F�b�N�t�H���_�̃t���p�X
//------------------------------------------
bool FolderFunc::CheckFolderUmu(char const* cPath)
{
	CFileFind fFind;
	//�����p�̃��C���h�J�[�h�t���p�X��p��
	CString sPath = cPath;
	sPath.TrimRight(_T("\\"));
	sPath += _T("\\*.*");

	if( ! fFind.FindFile( sPath, 0)) return false;
	
	fFind.Close();
	return true;
}

//------------------------------------------
// �t�@�C���L���`�F�b�N
// char const* cFPath �擾�������t�@�C���̃t���p�X
// �߂�l: false:�ُ�  true:�t�@�C���L��
//------------------------------------------
bool FolderFunc::CheckFileUmu(char const* cFPath)
{
	CFileFind fFind;
	CString sPath = cFPath;

	if( ! fFind.FindFile( sPath, 0)) return false;
	
	fFind.Close();
	return true;
}

//------------------------------------------
// �t�@�C���A�N�Z�X�`�F�b�N
// char const* cFPath �擾�������t�@�C���̃t���p�X
// �߂�l: false:�ُ�  true:�t�@�C���A�N�Z�X��
//------------------------------------------
bool FolderFunc::CheckFileAccess(char const* cFPath)
{
	//// �t�@�C���֌W �̃f�[�^���O����
	FILE*	fp = NULL;			// �t�@�C���|�C���^

	// �t�@�C���I�[�v��
	fp = fopen(cFPath, "rb");
	if(fp == NULL){        // �I�[�v�����s
		return false;
	}
	// �t�@�C���T�C�Y�擾
	fclose(fp);
	
	return true;
}

//------------------------------------------
// �t�H���_�ȉ����폜����֐�
// char const* cPath �폜����t�H���_�̃t���p�X
//------------------------------------------
bool FolderFunc::DeleteFolder(char const* cPath)
{
	CFileFind fFind;

	//�����p�̃��C���h�J�[�h�t���p�X��p��
	CString sPath = cPath;
	sPath.TrimRight(_T("\\"));
	sPath += _T("\\*.*");

	if( ! fFind.FindFile( sPath )) return false;
	BOOL bRet=true;
	while(bRet){
		bRet=fFind.FindNextFile();
		if(fFind.IsDots()) continue;

		// GetFilePath ���g����
		// ����Ƀp�X���擾�ł��Ȃ��ꍇ������̂�
		// �t���p�X���쐬���鏈��������
		CString sWk;
		sWk.Format(_T("%s\\%s"), cPath, fFind.GetFileName());
		// �t�H���_������΍ċA
		// �t�@�C���Ȃ�폜
		if(fFind.IsDirectory()){
			DeleteFolder(sWk);
		}else{
			DeleteFile(sWk);
		}
	}
	fFind.Close();

	return RemoveDirectory(cPath);
}


//------------------------------------------
// *�w�� or �t���p�X�w�� �t�@�C���� �폜����֐�
// char const* cPath �폜����t�@�C���̃t���p�X
//------------------------------------------
bool FolderFunc::DeleteFileAll(char const* cPath)
{
	CFileFind fFind;
	CString sPath = cPath;

	if( ! fFind.FindFile( sPath, 0)) return false;
	BOOL bRet=true;
	while(bRet){
		bRet=fFind.FindNextFile();
		if(fFind.IsDots()) continue;

		// �t�@�C���Ȃ�폜
		if(fFind.IsDirectory()){

		}else{
			DeleteFile( fFind.GetFilePath());
		}
	}
	fFind.Close();
	return true;
}

//------------------------------------------
// �w��t�H���_�̋󂫃G���A�`�F�b�N
// char const* sFolder �Ώۃt�H���_ (�Ō��\�͖���)
// �߂�l	0�`�F���݂̋󂫗e�� [GB]	0>:���s
//------------------------------------------
double FolderFunc::CheckFreeSpace(char const* sFolder)
{
	CString	sWk;

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
// �t�H���_�ꗗ���擾
// bool					bFolder	true:�t�H���_  false:�t�@�C��
// char const*			cRoot	�����p�X (�t�H���_��[aa\\*]  �t�@�C����[aa\\*.csv])
// std::vector<F_ITEM>* vItem	�擾�A�C�e��
// int nSort					���ёւ��I�v�V���� (-1:�Â��� 0:���� 1:�V������) �������ݓ��t�Ń\�[�g
//------------------------------------------
void FolderFunc::GetList(bool bFolder, char const* cRoot, std::vector<F_ITEM>* vItem, int nSort)
{
	// =========================================================
	// �t�H���_���̌Â����̏���RPI�t�@�C���ꗗ�𐶐�
	WIN32_FIND_DATA	fd;
	HANDLE			hFind;
	memset(&fd, 0x00, sizeof(fd));
	hFind = FindFirstFile(cRoot, &fd);
	while(hFind != INVALID_HANDLE_VALUE) {
		// ����͑ΏۊO
		if( 0==strcmp(".", fd.cFileName ) ) goto NEXT;
		if( 0==strcmp("..", fd.cFileName ) ) goto NEXT;

		// �Ώۂ̂�
		//if( ( bFolder && 0 != (FILE_ATTRIBUTE_DIRECTORY & fd.dwFileAttributes) ) ||			// �t�H���_
		//	(!bFolder && 0 == (FILE_ATTRIBUTE_DIRECTORY & fd.dwFileAttributes) ) ) {		// �t�@�C��
		if( ( bFolder && (FILE_ATTRIBUTE_DIRECTORY == fd.dwFileAttributes) ) ||			// �t�H���_
			(!bFolder && (FILE_ATTRIBUTE_DIRECTORY != fd.dwFileAttributes) ) ) {		// �t�@�C��

			// ���X�g�ɒǉ�
			F_ITEM t;
			t.sName				= fd.cFileName;
			t.tTime				= fd.ftLastWriteTime;
			vItem->push_back(t);
		}


		// ��
NEXT:
		if(! FindNextFile(hFind, &fd)) {
			if(ERROR_NO_MORE_FILES != GetLastError()) {
				LOG(em_ERR), "[%s] �t�@�C���ꗗ�擾���s %d", my_sThreadName, GetLastError());
			}
			break;
		}
	}
	FindClose(hFind);

	// =========================================================
	// �\�[�g
	if(-1 == nSort)		{ std::sort(vItem->begin(), vItem->end(), sort_fd_old); }
	else if(1 == nSort) { std::sort(vItem->begin(), vItem->end(), sort_fd_new); }

}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����

//// �\�[�g�̔�r�֐�
//------------------------------------------
// �Â����������BA���Â��ꍇTrue
//------------------------------------------
inline bool FolderFunc::sort_fd_old(const F_ITEM &a, const F_ITEM &b)
{
	return a.tTime < b.tTime;
}
//------------------------------------------
// �V�������������BA���V�����ꍇTrue
//------------------------------------------
inline bool FolderFunc::sort_fd_new(const F_ITEM &a, const F_ITEM &b)
{
	return a.tTime > b.tTime;
}


//------------------------------------------
// Ping�`�F�b�N
// char const* srv  �T�[�o�[�A�h���X
// bool bIpAddr true:IP�A�h���X   false:�z�X�g��
// �߂�l -:�ُ� 1:�ڑ��ς� 0:����ڑ�
//------------------------------------------
bool FolderFunc::PingCheck(char const* srv, bool bIpAddr)
{
	PingManager clsPing;

	char cIp[256];
	memset(cIp, 0x00, sizeof(cIp));

	// IP�A�h���X�ɕϊ�
	if(bIpAddr) {
		memcpy(cIp, srv, sizeof(cIp));
	} else {
		if( ! clsPing.HostToIp( srv, cIp )) return false;
	}

	return clsPing.Ping(cIp);
}
