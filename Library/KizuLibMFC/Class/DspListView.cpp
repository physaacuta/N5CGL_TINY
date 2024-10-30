#include "StdAfx.h"
#include "DspListView.h"

using namespace KizuLib;

//------------------------------------------
// コンストラクタ
// CListCtrl* lstView リストコントロール
//------------------------------------------
DspListView::DspListView(CListCtrl* lstView) :
m_pView(lstView),
m_nColumnNum(0),
m_nRowNum(0)
{
	// 書式をリスト形式に変更 (固定)
	m_pView->SetView(LV_VIEW_LIST); 
}

//------------------------------------------
// デストラクタ
//------------------------------------------
DspListView::~DspListView(void)
{
}

//------------------------------------------
// 列定義 (一括定義)
// int nClmNum 列数 (=今回引数で渡している配列の数)
// int nStringSize 文字列のサイズ
// int const* nWidthSize 各列のサイズ
// char const* cStringHeader 各列のヘッダー (NULLで終わる文字列) (cccc[][32] = {"商品", "単価","個数"};)
//------------------------------------------
void DspListView::SetColumnAll(int nClmNum, int nStringSize, int const* nWidthSize, char const* cStringHeader)
{
	LVCOLUMN    lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;    // 有効フラグ
	m_nColumnNum = nClmNum;

	// 列定義
	for(int ii=0; ii<nClmNum; ii++) {
        lvc.iSubItem    = ii;										// サブアイテム番号
        lvc.pszText     = (LPSTR)&cStringHeader[nStringSize*ii];	// 見出しテキスト
        lvc.cx          = nWidthSize[ii];							// 横幅

		// セット
		m_pView->InsertColumn(ii, &lvc);
	}
}
// int const* fmt	LVCFMT_LEFT,	LVCFMT_RIGHT,	LVCFMT_CENTER
void DspListView::SetColumnAll(int nClmNum, int nStringSize, int const* nWidthSize, char const* cStringHeader, int const* fmt)
{
	LVCOLUMN    lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVCF_FMT;    // 有効フラグ
	m_nColumnNum = nClmNum;

	// 列定義
	for(int ii=0; ii<nClmNum; ii++) {
        lvc.iSubItem    = ii;										// サブアイテム番号
        lvc.pszText     = (LPSTR)&cStringHeader[nStringSize*ii];	// 見出しテキスト
        lvc.cx          = nWidthSize[ii];							// 横幅
		lvc.fmt			= fmt[ii];									// フォーマット

		// セット
		m_pView->InsertColumn(ii, &lvc);
	}
}

//------------------------------------------
// 列を追加する
// int nWidthSize 列のサイズ
// char const* cStringHeader 列のヘッダー (NULLで終わる文字列)
//------------------------------------------
void DspListView::SetColumnAdd(int nWidthSize, char const* cStringHeader)
{
	LVCOLUMN    lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;    // 有効フラグ

	// 列定義
    lvc.iSubItem	= m_nColumnNum;			// サブアイテム番号
    lvc.pszText     = (LPSTR)cStringHeader;	// 見出しテキスト
    lvc.cx          = nWidthSize;			// 横幅

	// セット
	m_pView->InsertColumn(m_nColumnNum, &lvc);
	m_nColumnNum += 1;
}

//------------------------------------------
// 行を追加する (一括定義)
// int nRowNum 行数
//------------------------------------------
void DspListView::SetRowAll(int nRowNum)
{
	for(int ii=0; ii<nRowNum; ii++) SetRowAdd();
}

//------------------------------------------
// 行を追加する
// char const* cStringHeader 最左列の情報
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
// 文字列を変更
// int row 行位置 (0オリジン)
// int clm 列位置 (0オリジン)
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
// 文字列の取得
// int row 行位置 (0オリジン)
// int clm 列位置 (0オリジン)
//------------------------------------------
CString DspListView::GetItem(int row, int clm)
{
	return m_pView->GetItemText(row, clm); 
}