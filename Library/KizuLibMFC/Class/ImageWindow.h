// *********************************************************************************
//	グレー画像、2値画像を画面に表示
//	[Ver]
//		Ver.01    2010/03/04	初版
//
//	[メモ]
//		・
// *********************************************************************************

#pragma once

// 疵検クラス インクルード
#include "ThreadManager.h"												// ワーカースレッドマネージャー


using namespace KizuLib;
class ImageWindow :	public ThreadManager
{

//// 公開定数
public:
	static const int MAX_NIMAGE_WINDOW				= 16;				// 管理する最大画面数
	static const int IDC_MOVE                       = 101;


#pragma pack(push, 1) // byte alignを1byteにする
	// ビットマップヘッダ
	struct BITMAPHEADERS												// ビットマップヘッダ (形は違うが、KizuBitmapManagerと同一)
	{
		BITMAPFILEHEADER file;
		BITMAPINFO       info;											// これは、BITMAPINFOHEADER、RGBQUAD が結合している
	};
#pragma pack(pop)


//// 公開関数
public:
	ImageWindow();
	virtual ~ImageWindow(void);
	void SetThreadName(CString str) { this->my_sThreadName = str; }		// スレッド名称設定


	bool Init(int nWinNum, HINSTANCE hInst);							// 初期設定
	void Dispose();														// 終了

	// 自作の画面処理
	void		SetZoom(long windex, float x, float y);					// 拡大
	void		SetScale(long windex, float x, float y);				// 表示画像の倍率
	void		SetSize(long windex, long cx, long cy);					// 画面サイズ
	void		SetPos(long windex, long sx, long sy);					// 画面表示位置
	void		SetTitle(long windex, const char* title);				// タイトル

	void		SetColor(COLORREF ref) {my_current_color = ref;};		// ペン色設定
	void		SetStretchMode(int mode){my_gray_stretch_mode = mode;};	// ストレッチモード

	BOOL		ShowWindow(long windex, int show) const;				// 画面表示
	BOOL		UpdateWindow(long windex) const;						// 画面更新

	// 描画関係
	BOOL		Display(long windex, BITMAPHEADERS const* bitheader, BYTE const* img, BOOL update=TRUE); // 画像表示
	
	BOOL		DrawLine(long windex, long x1, long y1, long x2, long y2, BOOL update);	// 直線、斜線
	BOOL		DrawPolyline(long windex, const POINT *pt, long len, BOOL update);		// 折れ線グラフ形式の線
	BOOL		DrawRect(long windex, long x1, long y1, long x2, long y2, BOOL update);	// 矩形
	BOOL		DrawText(long windex, long x, long y, const char *text, BOOL update);	// 文字描画

	template <class T> BOOL DrawRects(long windex, T *x1, T *y1, T *x2, T *y2, long len, T offset, BOOL update = TRUE)
	{
		IMGWIN& win = my_Win[windex];			// 使いやすいようにエイリアス
		EnterCriticalSection(&my_cs[windex]);
		long pen_width = (long)(1.0f / __min(win.zoom_x, win.zoom_y) + 0.5);
		HPEN new_pen = CreatePen(PS_SOLID, pen_width, my_current_color);
		HGDIOBJ old_pen = SelectObject(win.mem_hdc, new_pen);
		
		for (int i = 0; i < len; i++) {
			MoveToEx(win.mem_hdc, (long)(win.scale_x*(x1[i]-offset)), (long)(win.scale_y*(y1[i]-offset)), NULL);
			LineTo(win.mem_hdc, (long)(win.scale_x*(x2[i]+offset)), (long)(win.scale_y*(y1[i]-offset)));
			LineTo(win.mem_hdc, (long)(win.scale_x*(x2[i]+offset)), (long)(win.scale_y*(y2[i]+offset)));
			LineTo(win.mem_hdc, (long)(win.scale_x*(x1[i]-offset)), (long)(win.scale_y*(y2[i]+offset)));
			LineTo(win.mem_hdc, (long)(win.scale_x*(x1[i]-offset)), (long)(win.scale_y*(y1[i]-offset)));
		}
		
		DeleteObject(SelectObject(win.mem_hdc, old_pen));
		
		if (update) InvalidateRect(win.hwnd, NULL, FALSE);
		LeaveCriticalSection(&my_cs[windex]);
		return TRUE;
	}



//// メンバー関数
protected:
	int			ThreadFirst()	{ return 0;};							// スレッド開始前処理 (スレッド実行直後(シグナルセット前)にコールバック)
	int			ThreadLast()	{ return 0;};							// スレッド終了前処理 (終了シグナル後にコールバック)
	void		ThreadEvent(int nEventNo) {return;};					// スレッドイベント発生
	int			ThreadMainLoop();										// スレッドメインループ

	// 基本操作
	static LRESULT CALLBACK WndProcStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	virtual BOOL			WndProcEntry(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT& res);
	virtual LRESULT			WndProcMain(long windex, UINT msg, WPARAM wparam, LPARAM lparam);

	virtual LRESULT			OnDestroy	 (long windex, WPARAM wparam, LPARAM lparam);	// 画面の終了
	virtual LRESULT			OnSize		 (long windex, WPARAM wparam, LPARAM lparam);	// 画面サイズ変更
	//virtual LRESULT			OnSizing	 (long windex, WPARAM wparam, LPARAM lparam);
	virtual LRESULT			OnPaint		 (long windex, WPARAM wparam, LPARAM lparam);	// 描画
	virtual LRESULT			OnHScroll	 (long windex, WPARAM wparam, LPARAM lparam);	// X軸スクロール
	virtual LRESULT			OnVScroll	 (long windex, WPARAM wparam, LPARAM lparam);	// Y軸スクロール
	virtual LRESULT			OnMouseMove	 (long windex, WPARAM wparam, LPARAM lparam);	// 画面上でのマウス移動
	virtual LRESULT			OnLButtonDown(long windex, WPARAM wparam, LPARAM lparam);	// 左マウスボタン下げ
	virtual LRESULT			OnRButtonDown(long windex, WPARAM wparam, LPARAM lparam);	// 左マウスボタン上げ	
	virtual LRESULT			OnLButtonUp	 (long windex, WPARAM wparam, LPARAM lparam);	// 右マウスボタン下げ
	virtual LRESULT			OnRButtonUp	 (long windex, WPARAM wparam, LPARAM lparam);	// 右マウスボタン上げ


	// 自作の画面処理
	void		set_scroll_range(long windex, BOOL redraw);				// スクロールバーの範囲
	void		set_scroll_pos_x(long windex, int x, BOOL redraw);		// スクロールバーの位置
	void		set_scroll_pos_y(long windex, int x, BOOL redraw);


    // DCへの描画
    virtual BOOL StretchBltBase(HDC dst,
							BITMAPHEADERS const* bitheader, BYTE const* img,
                            long dst_x, long dst_y, long dst_w, long dst_h,
                            long x, long y, long w, long h,
                            DWORD rop=SRCCOPY) const;


//// メンバー変数
protected :
	// 基本
	static bool				my_bIsOk[MAX_NIMAGE_WINDOW];				// 画面描画OK

	static ImageWindow*		my_ImageWindow[];							// 画面本体
	static HINSTANCE		my_hInst;									// ベースの画面インスタンス
	static ATOM				my_reg_atom;
    struct IMGWIN{
        char    title[256];						// 画面タイトル

        HWND    hwnd;
        HBITMAP old_hbitmap;
        RECT    hwnd_rect;						// 現在のウィンドウサイズ

        HDC     mem_hdc;
        HBITMAP mem_old_hbitmap;
        RECT    mem_rect;

        float   zoom_x;							// 拡大倍率
        float   zoom_y;

		float	scale_x;						// 表示画像の倍率
		float	scale_y;

        struct {
            int val;
            int max;
        } scroll_x, scroll_y;					// スクロール設定

        struct {
            BOOL sw;
            POINT start;
            POINT scroll;
        } grab;
    }*						my_Win;										// 画面情報
	int						my_nWin;									// 画面数

	HCURSOR					my_old_cursor;								// 前回のマウスカーソル形状
	COLORREF				my_current_color;							// 現在の描画ペン色
	int						my_gray_stretch_mode;						// ストレッチモード

	// 制御
	HANDLE					my_evThReady;								// スレッドの準備完了シグナル
	CRITICAL_SECTION*		my_cs;										// クリティカルセクション
};
