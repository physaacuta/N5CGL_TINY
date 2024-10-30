//=====================================================================
// CSV�� Ini�t�@�C���̂悤�ȃZ�N�V�����P�ʂł̐ݒ�t�@�C���̓Ǎ�����N���X
//	�y�S���C���z
//		�E��{�I�Ɋ��S���p�\�Ȃ悤�ɍ��
//=====================================================================

#pragma once

// STL
#include <vector>														// ���X�g�p

namespace KizuLib
{
	class CsvIniFunc
	{
	private :
		static const int	SEC_HEAD_S = '[';								// �Z�N�V������؂�J�n�L�[
		static const int	SEC_HEAD_E = ']';								// �Z�N�V������؂�J�n�L�[

		
	//// ���J�萔
	public:
		CsvIniFunc(void);
		virtual ~CsvIniFunc(void);


		//=========================================
		// �v���p�e�B
		void	SetSecHeader(int rec, char const* cKey) { strcpy( m_cSecHead[rec], cKey); }


		//=========================================
		// �O������֐�
		bool	ReadCsv(LPCSTR sSec, LPCSTR sPath, std::vector<CString>* pRow);		// CSV�t�@�C���Ǎ�
		int		CntColumn(const CString& sVal);										// �v�f�������邩

		bool	GetColumn(const CString& sVal, int& curPos, CString& sToken);			// �P�s�̃f�[�^���� , �P�ʂ̕�������擾
		CString GetColumnStr(const CString& sVal, int& curPos);
		void	GetColumnChar(const CString& sVal, int& curPos, char* cAns, int nSize);
		int		GetColumnInt(const CString& sVal, int& curPos);
		double	GetColumnDouble(const CString& sVal, int& curPos);


	//// �����o�[�֐�
	private:
		// �Z�N�V������؂�
		char						m_cSecHead[2][16];					// �Z�N�V�����J�n��؂�
	};
};