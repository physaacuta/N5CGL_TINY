// *********************************************************************************
//	CListCtrl �� ��ʕ\���T�|�[�g�N���X
//	[Ver]
//		Ver.01    2009/05/27  vs2005 �Ή� (�R���X�g���N�^�ŃT�C�Y�w��ɕύX)
//
//	[����]
//		MFC��CListCtrl�ɕ\�������p�N���X
// *********************************************************************************


#pragma once

#include <afxcview.h>        // MFC ��View�R���|�[�l���g

namespace KizuLib
{
	class DspListView
	{

	//// ���J�\����
	public :
		static const int PARAM_HEADER_MAX_STRING = 256;				// �w�b�_�[�^�C�g���ő啶����



	//// ���J���W���[���֌W
	public:
		DspListView(CListCtrl* lstView);
		virtual ~DspListView(void);


		void SetColumnAll(int nClmNum, int nStringSize, int const* nWidthSize, char const* cStringHeader);		// ���` (�ꊇ��`)		
		void SetColumnAll(int nClmNum, int nStringSize, int const* nWidthSize, char const* cStringHeader, int const* fmt);		// ���` (�ꊇ��`)		
		void SetColumnAdd(int nWidthSize, char const* cStringHeader);			// ���ǉ�����

		void SetRowAll(int nRowNum);											// �s��ǉ����� (�ꊇ��`)
		void SetRowAdd(char const* cKey = "");									// �s��ǉ�����


		void SetItem(int row, int clm, char const* s);							// �������ύX
		void SetItem(int row, int clm, CString const* s);
		void SetItem(int row, int clm, int s);
		void SetItem(int row, int clm, DWORD s);

		//// �擾
		CString GetItem(int row, int clm);										// ������̎擾


		//// �p�����[�^�擾�C���[�W
		int	 GetRowCount() const { return m_pView->GetItemCount(); }			// �s���ǉ�
		int  GetSelectRow() const { return (int)(__int64)m_pView->GetFirstSelectedItemPosition(); }	// �ŏ��̑I���s�擾 (0�́A���A�C�e���s�B1�I���W��)

	//// �����o�[�ϐ��֌W
	private :
		CListCtrl*			m_pView;											// ���X�g�R���g���[���̃|�C���^
		
		int					m_nColumnNum;										// ��
		int					m_nRowNum;											// �s��
	};

};