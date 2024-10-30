#include "StdAfx.h"
#include "DspListView.h"

using namespace KizuLib;

//------------------------------------------
// �R���X�g���N�^
// CListCtrl* lstView ���X�g�R���g���[��
//------------------------------------------
DspListView::DspListView(CListCtrl* lstView) :
m_pView(lstView),
m_nColumnNum(0),
m_nRowNum(0)
{
	// ���������X�g�`���ɕύX (�Œ�)
	m_pView->SetView(LV_VIEW_LIST); 
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
DspListView::~DspListView(void)
{
}

//------------------------------------------
// ���` (�ꊇ��`)
// int nClmNum �� (=��������œn���Ă���z��̐�)
// int nStringSize ������̃T�C�Y
// int const* nWidthSize �e��̃T�C�Y
// char const* cStringHeader �e��̃w�b�_�[ (NULL�ŏI��镶����) (cccc[][32] = {"���i", "�P��","��"};)
//------------------------------------------
void DspListView::SetColumnAll(int nClmNum, int nStringSize, int const* nWidthSize, char const* cStringHeader)
{
	LVCOLUMN    lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;    // �L���t���O
	m_nColumnNum = nClmNum;

	// ���`
	for(int ii=0; ii<nClmNum; ii++) {
        lvc.iSubItem    = ii;										// �T�u�A�C�e���ԍ�
        lvc.pszText     = (LPSTR)&cStringHeader[nStringSize*ii];	// ���o���e�L�X�g
        lvc.cx          = nWidthSize[ii];							// ����

		// �Z�b�g
		m_pView->InsertColumn(ii, &lvc);
	}
}
// int const* fmt	LVCFMT_LEFT,	LVCFMT_RIGHT,	LVCFMT_CENTER
void DspListView::SetColumnAll(int nClmNum, int nStringSize, int const* nWidthSize, char const* cStringHeader, int const* fmt)
{
	LVCOLUMN    lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVCF_FMT;    // �L���t���O
	m_nColumnNum = nClmNum;

	// ���`
	for(int ii=0; ii<nClmNum; ii++) {
        lvc.iSubItem    = ii;										// �T�u�A�C�e���ԍ�
        lvc.pszText     = (LPSTR)&cStringHeader[nStringSize*ii];	// ���o���e�L�X�g
        lvc.cx          = nWidthSize[ii];							// ����
		lvc.fmt			= fmt[ii];									// �t�H�[�}�b�g

		// �Z�b�g
		m_pView->InsertColumn(ii, &lvc);
	}
}

//------------------------------------------
// ���ǉ�����
// int nWidthSize ��̃T�C�Y
// char const* cStringHeader ��̃w�b�_�[ (NULL�ŏI��镶����)
//------------------------------------------
void DspListView::SetColumnAdd(int nWidthSize, char const* cStringHeader)
{
	LVCOLUMN    lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;    // �L���t���O

	// ���`
    lvc.iSubItem	= m_nColumnNum;			// �T�u�A�C�e���ԍ�
    lvc.pszText     = (LPSTR)cStringHeader;	// ���o���e�L�X�g
    lvc.cx          = nWidthSize;			// ����

	// �Z�b�g
	m_pView->InsertColumn(m_nColumnNum, &lvc);
	m_nColumnNum += 1;
}

//------------------------------------------
// �s��ǉ����� (�ꊇ��`)
// int nRowNum �s��
//------------------------------------------
void DspListView::SetRowAll(int nRowNum)
{
	for(int ii=0; ii<nRowNum; ii++) SetRowAdd();
}

//------------------------------------------
// �s��ǉ�����
// char const* cStringHeader �ō���̏��
//------------------------------------------
void DspListView::SetRowAdd(char const* cKey)
{
	LVITEM       lvi;
	lvi.mask		= LVIF_TEXT;
	lvi.iItem		= m_nRowNum;
	lvi.iSubItem	= 0;
	lvi.pszText		= (LPSTR)cKey;

	m_pView->InsertItem(&lvi);
	m_nRowNum += 1;
}

//------------------------------------------
// �������ύX
// int row �s�ʒu (0�I���W��)
// int clm ��ʒu (0�I���W��)
//------------------------------------------
void DspListView::SetItem(int row, int clm, char const* s)
{
	m_pView->SetItemText(row, clm, s); 
}
void DspListView::SetItem(int row, int clm, CString const* s)
{
	m_pView->SetItemText(row, clm, *s ); 
}
void DspListView::SetItem(int row, int clm, int s)
{
	CString sWk;
	sWk.Format("%d", s);
	m_pView->SetItemText(row, clm, sWk ); 
}
void DspListView::SetItem(int row, int clm, DWORD s)
{
	CString sWk;
	sWk.Format("%d", s);
	m_pView->SetItemText(row, clm, sWk ); 
}
//------------------------------------------
// ������̎擾
// int row �s�ʒu (0�I���W��)
// int clm ��ʒu (0�I���W��)
//------------------------------------------
CString DspListView::GetItem(int row, int clm)
{
	return m_pView->GetItemText(row, clm); 
}