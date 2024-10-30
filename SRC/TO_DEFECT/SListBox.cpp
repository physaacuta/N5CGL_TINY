// SListBox.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SListBox.h"


// SListBox

IMPLEMENT_DYNAMIC(SListBox, CListCtrl)

SListBox::SListBox()
{

}

SListBox::~SListBox()
{
	Dispose();
}

void SListBox::Dispose()
{
	CButton *bt = NULL;
	for (int ii = 0; ii < (int)m_buttons.size(); ii++) {
		bt = m_buttons[ii];
		if( bt->m_hWnd != NULL ){
			bt->DestroyWindow();
		}
		delete bt;
	}
	m_buttons.clear();
}

void SListBox::SyncButtons(int nSubItem)
{
	LOGFONT log_font;
	GetFont()->GetLogFont( &log_font );
	log_font.lfHeight = 12;
	m_font.CreateFontIndirectA(&log_font);


	m_nSubItem = nSubItem;
	int nItem = GetItemCount();
	for (int ii = 0; ii < nItem; ii++) {
		CString caption = GetItemText(ii, m_nSubItem);

		CRect r;		GetSubItemRect(ii, m_nSubItem, LVIR_BOUNDS, r);

		CButton *bt = NULL;
		if (ii < m_buttons.size())
			bt = m_buttons[ii];

		if ( !bt ) {
			bt = new CButton();
			m_buttons.push_back(bt);
			bt->Create(caption, WS_CHILD | WS_VISIBLE, r, this, BASE_NUM + ii);



			bt->SetFont(&m_font);
		}
		bt->SetWindowText(caption);
	}

}

void SListBox::OnPaint()
{
	Default();

	int nItem = GetItemCount();
	if (m_buttons.size() != nItem)
		return;

	for (int ii = 0; ii < nItem; ii++) {
		CRect r;
		GetSubItemRect(ii, m_nSubItem, LVIR_BOUNDS, r);

		m_buttons[ii]->MoveWindow(r);
	}
}


void SListBox::OnClickButton(UINT id) {
	NMITEMACTIVATE notify;
	notify.hdr.code = NM_CLICK;
	notify.hdr.hwndFrom = *this;
	notify.hdr.idFrom = GetDlgCtrlID();
	notify.iItem = id - BASE_NUM;
	notify.iSubItem = m_nSubItem;
	notify.uNewState = 0;
	notify.uOldState = 0;
	notify.uChanged = 0;
	GetCursorPos(&notify.ptAction);
	notify.lParam = 0;
	notify.uKeyFlags = 0;

	GetParent()->SendMessage(WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&notify);
}

BEGIN_MESSAGE_MAP(SListBox, CListCtrl)
	ON_WM_PAINT()
	ON_COMMAND_RANGE(500, 550, OnClickButton)
END_MESSAGE_MAP()



// SListBox メッセージ ハンドラー


