#pragma once


// SListBox
#include <vector>

class SListBox : public CListCtrl
{
	DECLARE_DYNAMIC(SListBox)

	std::vector< CButton *> m_buttons;
	int m_nSubItem;
	CFont m_font;

	static const int BASE_NUM = 500;


public:
	SListBox();
	virtual ~SListBox();

	void SyncButtons(int iSubItem);

	// ボタン取り出し (0オリジン) 変なところの場合は -1
	// イベントはNM_CLICK
	static int  GetClickItemIndex( NMHDR* p ) { 
		NMITEMACTIVATE* notify = (NMITEMACTIVATE*) p;
		return notify->iItem;
	}

protected:
	void Dispose();
	void OnPaint();
	void OnClickButton(UINT id);

	DECLARE_MESSAGE_MAP()
};


