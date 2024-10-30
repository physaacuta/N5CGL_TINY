//=====================================================================
// �t�H���_�A�t�@�C������n�̃N���X�Q
//	�y�S���C���z
//		�E��{�I�Ɋ��S���p�\�Ȃ悤�ɍ��
//=====================================================================
#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\FuncBase.h"					// ���W���[���̊��N���X
#include "..\..\Library\KizuLib\CLASS\PingManager.h"				


// �W��
#include <direct.h>
// STL
#include <vector>														// ���X�g�p
#include <algorithm>													// �\�[�g�p

// �ǉ�
#include <imagehlp.h>
#pragma comment(lib, "imagehlp.lib")					// �T�u�f�B���N�g���܂ň�C�ɍ��

namespace KizuLib
{
	class FolderFunc  : public FuncBase
	{

	public:
		struct F_ITEM {
			CString			sName;			// ���O (�t���p�X���ᖳ��)
			CTime			tTime;			// �����ݓ��t
		};

	public:
		FolderFunc(void);
		virtual ~FolderFunc(void);

		// ��PC������
		bool CreateFolder(char const* cPath);								// �T�u�f�B���N�g���܂ň�C�ɍ쐬
		bool CheckFolderUmu(char const* cPath);								// �t�H���_�L���`�F�b�N
		bool CheckFileUmu(char const* cFPath);								// �t�@�C���L���`�F�b�N
		bool CheckFileAccess(char const* cFPath);							// �t�@�C���A�N�Z�X�`�F�b�N
		bool DeleteFolder(char const* cPath);								// �t�H���_�ȉ����폜����֐�
		bool DeleteFileAll(char const* cPath);								// *�w�� or �t���p�X�w�� �t�@�C���� �폜����֐�

		double CheckFreeSpace(char const* sFolder);							// �w��t�H���_�̋󂫃G���A�`�F�b�N
		void GetList(bool bFolder, char const* cRoot, std::vector<F_ITEM>* vItem, int nSort);	// �t�H���_�ꗗ���擾
		
		
		// �t��
		static bool PingCheck(char const* srv, bool bIpAddr = true);					// Ping�`�F�b�N


	private:
		static inline bool sort_fd_old(const F_ITEM &a, const F_ITEM &b);	// �\�[�g�֐�
		static inline bool sort_fd_new(const F_ITEM &a, const F_ITEM &b);	// �\�[�g�֐�


	};
};