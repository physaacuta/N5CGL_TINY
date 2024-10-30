// *********************************************************************************
//	AVI�摜�N���X
//	[Ver]
//		Ver.01    2010/02/16  vs2005 �Ή�
//
//	[����]
//		�Evfw.lib���g�p���Ă��邽�߁AKizuLib���ɂ͓���Ȃ�
//		�E�f�U�C���p�^�[���̃V���O���g���ɑΉ� ���Ȃ���vfw���_���Ȃ̂ł�߂�
// *********************************************************************************

#pragma once

#include <windows.h>
#include <vfw.h>

namespace ImgLib
{
	//// �{��
	class KizuAviManager
	{
	public:
		KizuAviManager(void);
		virtual ~KizuAviManager(void);


		//// �X�^�e�B�b�N
		static bool OpenAVIFile(const char* filename, PAVIFILE& avi_file, PAVISTREAM& avi_stream, AVISTREAMINFO* avi_info);	// AVI�t�@�C���̃I�[�v��
		static void CloseAVIFile(PAVIFILE& avi_file, PAVISTREAM& avi_stream);												// AVI�t�@�C���̃N���[�X
		static bool GetAVIInfo(const char* filename, DWORD* width, DWORD* height, DWORD* bit_count, DWORD* nframes);		// AVI�t�@�C���̏��擾
		static BITMAPINFO* GetAVIFormat(PAVISTREAM avi_stream);													// AVI�t�@�C���̃t�H�[�}�b�g���擾


		//// �����o�[
		bool OpenAVIFile(const char* filename);				// AVI�t�@�C���̃I�[�v��
		void CloseAVIFile();								// AVI�t�@�C���̃N���[�X
		bool LoadFromAVI(long count, BYTE* img, bool nTB=false) const;	// AVI�t�@�C���̂P�t���[���ǂݍ���


		//// �v���p�e�B
		DWORD		GetWidth()	  const { return m_nW; }
		DWORD		GetHeight()   const { return m_nH; }
		DWORD		GetBitCnt()   const { return m_nBitCnt; }
		DWORD		GetFrameCnt() const { return m_nFrameCnt; }

		
		//// ���̑��x���n
		static void HantenTB( DWORD nW, DWORD nH, BYTE *p );	// �㉺�𔽓]���Ȃ���R�s�[����


	private:
		PAVIFILE			m_AviFile;						// AVI�t�@�C���̃n���h��
		PAVISTREAM			m_AviStream;					// AVI�t�@�C���̃X�g���[��
		AVISTREAMINFO		m_AviInfo;						// AVI�t�@�C���̃w�b�_�[���

		// AIV�t�@�C���̏��
		DWORD				m_nW;							// �摜��
		DWORD				m_nH;							// �摜����
		DWORD				m_nBitCnt;						// �P�s�N�Z��������̃r�b�g��
		DWORD				m_nFrameCnt;					// AVI�t�@�C���Ɋ܂܂��t���[����

	};


	//// ���n���h�� (singleton)
	class KizuAviEnv
	{
	private :
		static KizuAviEnv			m_inst;					// exe�ŗB��̃C���X�^���X�B ����������new����āA�I������delete�����.

		KizuAviEnv();										// private �R���X�g���N�^
	public :
		virtual ~KizuAviEnv();								// �f�X�g���N�^

	};
};