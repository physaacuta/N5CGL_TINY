// *********************************************************************************
//	CListCtrl の 画面表示サポートクラス
//	[Ver]
//		Ver.01    2009/05/27  vs2005 対応 (コンストラクタでサイズ指定に変更)
//
//	[メモ]
//		MFCのCListCtrlに表示する専用クラス
// *********************************************************************************


#pragma once

#include <afxcview.h>        // MFC のViewコンポーネント

namespace KizuLib
{
	class DspListView
	{

	//// 公開構造体
	public :
		static const int PARAM_HEADER_MAX_STRING = 256;				// ヘッダータイトル最大文字列



	//// 公開モジュール関係
	public:
		DspListView(CListCtrl* lstView);
		virtual ~DspListView(void);


		void SetColumnAll(int nClmNum, int nStringSize, int const* nWidthSize, char const* cStringHeader);		// 列定義 (一括定義)		
		void SetColumnAll(int nClmNum, int nStringSize, int const* nWidthSize, char const* cStringHeader, int const* fmt);		// 列定義 (一括定義)		
		void SetColumnAdd(int nWidthSize, char const* cStringHeader);			// 列を追加する

		void SetRowAll(int nRowNum);											// 行を追加する (一括定義)
		void SetRowAdd(char const* cKey = "");									// 行を追加する


		void SetItem(int row, int clm, char const* s);							// 文字列を変更
		void SetItem(int row, int clm, CString const* s);
		void SetItem(int row, int clm, int s);
		void SetItem(int row, int clm, DWORD s);

		//// 取得
		CString GetItem(int row, int clm);										// 文字列の取得


		//// パラメータ取得イメージ
		int	 GetRowCount() const { return m_pView->GetItemCount(); }			// 行数追加
		int  GetSelectRow() const { return (int)(__int64)m_pView->GetFirstSelectedItemPosition(); }	// 最初の選択行取得 (0は、未アイテム行。1オリジン)

	//// メンバー変数関係
	private :
		CListCtrl*			m_pView;											// リストコントロールのポインタ
		
		int					m_nColumnNum;										// 列数
		int					m_nRowNum;											// 行数
	};

};