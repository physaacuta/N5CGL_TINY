#include "StdAfx.h"
#include "ImageWindow.h"


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// STATIC用

bool ImageWindow::my_bIsOk[MAX_NIMAGE_WINDOW];

HINSTANCE ImageWindow::my_hInst = 0;
ATOM ImageWindow::my_reg_atom = 0;
ImageWindow* ImageWindow::my_ImageWindow[MAX_NIMAGE_WINDOW];


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 基本処理

//------------------------------------------
// コンストラクタ
//------------------------------------------
ImageWindow::ImageWindow():
ThreadManager("tImageWindow"),
my_Win(NULL),
my_nWin(0),
my_gray_stretch_mode(COLORONCOLOR),
my_current_color(RGB(255, 255, 255)),
my_evThReady(NULL)
{

}

//------------------------------------------
// デストラクタ
//------------------------------------------
ImageWindow::~ImageWindow(void)
{
	Dispose();
}

//------------------------------------------
// 初期設定
// int nWinNum 画面数
// HINSTANCE hInst 画面インスタンス (AfxGetApp()->m_hInstance)
//------------------------------------------
bool ImageWindow::Init(int nWinNum, HINSTANCE hInst)
{
	int ii;

	//// 一番最初のみ
    if( 0 == my_reg_atom ) {
		_ASSERT( 0 == my_hInst );
		my_hInst = hInst;

		WNDCLASSEX	wnd_class;		// ウィンドウクラス
		wnd_class.cbSize = sizeof(wnd_class);
		//wnd_class.style = CS_CLASSDC;
		//wnd_class.style = CS_OWNDC | CS_NOCLOSE;
		wnd_class.style = CS_NOCLOSE;
		wnd_class.lpfnWndProc = WndProcStatic;
		wnd_class.cbClsExtra = 0;
		wnd_class.cbWndExtra = 0;
		wnd_class.hInstance = hInst;
		wnd_class.hIcon = NULL;
		wnd_class.hCursor = LoadCursor(NULL, IDC_ARROW);
		//wnd_class.hbrBackground = NULL;
		wnd_class.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
		wnd_class.lpszMenuName = NULL;
		wnd_class.lpszClassName = "ImageWindow";
		wnd_class.hIconSm = NULL;
		
		my_reg_atom = RegisterClassEx(&wnd_class);
		_ASSERT( 0 != my_reg_atom );
		
		for(ii=0; ii<MAX_NIMAGE_WINDOW; ii++) my_ImageWindow[ii] = NULL;
	}

	//// 今回のデータを情報をセット
	for( ii=0; ii<MAX_NIMAGE_WINDOW; ii++) {
		if( NULL == my_ImageWindow[ii] ) {
			my_ImageWindow[ii] = this;
			break;
		}
	}

	//// 空き無し
	if( ii == MAX_NIMAGE_WINDOW ) {
		Dispose();
		return false;
	}

	//// HWND配列の生成
    _ASSERT( 0 == my_nWin);
    _ASSERT( NULL == my_Win );
    my_nWin = nWinNum;
    my_Win = new IMGWIN[my_nWin];
	for( ii=0; ii<my_nWin; ii++) {
		memset(&my_Win[ii], 0x00, sizeof(IMGWIN));
		my_Win[ii].hwnd		= NULL;
		my_Win[ii].zoom_x	= my_Win[ii].zoom_y	 = 1.0f;
		my_Win[ii].scale_x	= my_Win[ii].scale_y = 1.0f;
	}

	//// CSの準備
	my_cs = new CRITICAL_SECTION[my_nWin];
	for (ii=0; ii<my_nWin; ii++) InitializeCriticalSection(&my_cs[ii]);

	//// スレッドの準備
    _ASSERT( NULL == my_evThReady );
	// 開始
    my_evThReady = CreateEvent(NULL, FALSE, FALSE, NULL);
	this->Start();
	
	// スレッドが開始されるまで待ち
    if( WAIT_OBJECT_0 != WaitForSingleObject(my_evThReady, INFINITE) ) {
        _ASSERT( false );
    }
    CloseHandle(my_evThReady);
    my_evThReady = NULL;
	return true;
}
//------------------------------------------
// 終了
//------------------------------------------
void ImageWindow::Dispose()
{
	//// 画面停止指示
	for(int ii=0; ii<my_nWin; ii++) {
		if( NULL != my_Win[ii].hwnd ) {
            PostMessage(my_Win[ii].hwnd, WM_QUIT, 0, 0);
            //break;
		}
	}

	//// スレッド停止
	this->Stop();

	//// 基本の解放
	if( NULL != my_Win ) {
		for(int ii=0; ii<my_nWin; ii++) {
			DeleteCriticalSection(&my_cs[ii]);
			_ASSERT( NULL == my_Win[ii].hwnd );
		}
        delete[] my_Win;
		delete[] my_cs;
        my_Win  = NULL;
        my_nWin = 0;
	}

	//// その他解放
	for(int ii=0; ii<MAX_NIMAGE_WINDOW; ii++) {
        if( this == my_ImageWindow[ii] ) {
            my_ImageWindow[ii] = NULL;
            break;
        }
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// スレッド

//------------------------------------------
// スレッドメインループ
//------------------------------------------
int ImageWindow::ThreadMainLoop()
{
    _ASSERT( NULL != my_Win );
    _ASSERT( 0 != my_reg_atom );

	// ============================================
	// 前準備

    // ウィンドウの作成、DC、メモリDCの作成・設定
    HDC hdc_desktop = GetDC(HWND_DESKTOP);
    const int style = WS_OVERLAPPED | WS_CAPTION	 | WS_THICKFRAME
					| WS_SYSMENU    | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
					| WS_HSCROLL	| WS_VSCROLL;

    const int width  = 128;
    const int height = 128;

    for(int ii=0; ii<my_nWin; ii++) {
		IMGWIN& win = my_Win[ii];			// 使いやすいようにエイリアス

		// タイトル決定
		//MultiThreadでwin.titleが競合すると嫌なので、tmp_titleで一旦うける
		char tmp_title[sizeof(win.title)]="";
        sprintf(tmp_title, "%s#%d", my_sThreadName, ii);
		memcpy( win.title, tmp_title, sizeof(win.title) );

        char buff[256];
        sprintf(buff, "%s(100%%,100%%)", tmp_title);
        _ASSERT( NULL == win.hwnd );


		// 画面生成
        win.hwnd = CreateWindowEx(0, (const char*)my_reg_atom,
                                        buff, style,
                                        CW_USEDEFAULT, CW_USEDEFAULT,
                                        width, height,
                                        HWND_DESKTOP, NULL, my_hInst, NULL);
        GetClientRect(win.hwnd, &win.hwnd_rect);
        win.mem_rect = win.hwnd_rect;
        win.zoom_x = win.zoom_y = 1.0;
Sleep(100);
        set_scroll_range(ii, FALSE);
        win.scroll_x.val = 0;
        win.scroll_y.val = 0;
        SetScrollPos(win.hwnd, SB_HORZ, 0, TRUE);
        SetScrollPos(win.hwnd, SB_VERT, 0, TRUE);

        HDC hdc = GetDC(win.hwnd);

        // memory DCの生成、BITMAPの生成・設定
        win.mem_hdc  = CreateCompatibleDC(hdc);
        HBITMAP hbmp = CreateCompatibleBitmap(hdc, win.mem_rect.right, win.mem_rect.bottom);
        win.mem_old_hbitmap = (HBITMAP)SelectObject(win.mem_hdc, hbmp);
        ReleaseDC(win.hwnd, hdc);

		// ウィンドウ表示

		::ShowWindow(win.hwnd, SW_SHOW);
Sleep(200);
		::UpdateWindow(win.hwnd);
        win.grab.sw = FALSE;
		my_bIsOk[ii] = true;
	}
Sleep(200);	// 待たないとたまに画面が落ちる
	ReleaseDC(HWND_DESKTOP, hdc_desktop);

	// これで準備完了
	SetEvent(my_evThReady);

	// ============================================
	// メインループ
    MSG msg;
    while( GetMessage(&msg, NULL, 0, 0) ) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

	// ============================================
	// 後始末
    // ウィンドウの破壊
    for(int ii=0; ii<my_nWin; ii++) {
        if( NULL != my_Win[ii].hwnd ) {
            DestroyWindow(my_Win[ii].hwnd);
        }
    }

	return (int)msg.wParam; // お約束
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 画面処理

//------------------------------------------
// 標準のWindowProc を拡張
//------------------------------------------
LRESULT CALLBACK ImageWindow::WndProcStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	for(int ii=0; ii<MAX_NIMAGE_WINDOW; ii++) {
        if( NULL != my_ImageWindow[ii]) {
            LRESULT res;
            if( my_ImageWindow[ii]->WndProcEntry(hwnd, msg, wparam, lparam, res) ) {
                return res;
            }
        }
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}
BOOL ImageWindow::WndProcEntry(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT& res)
{
    for(int ii=0; ii<my_nWin; ii++) {
        if( my_Win[ii].hwnd == hwnd  && my_bIsOk[ii]) {
            res = WndProcMain(ii, msg, wparam, lparam);
            return TRUE;
        }
    }
    return FALSE;
}
//------------------------------------------
// 標準の画面操作
//------------------------------------------
LRESULT ImageWindow::WndProcMain(long windex, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch( msg ) {
    case WM_DESTROY:				// 終了
		return OnDestroy(windex, wparam, lparam);
        
    case WM_SIZE:
		return OnSize(windex, wparam, lparam);

    case WM_PAINT:
		return OnPaint(windex, wparam, lparam);

    case WM_HSCROLL:
		return OnHScroll(windex, wparam, lparam);

    case WM_VSCROLL:
		return OnVScroll(windex, wparam, lparam);

    case WM_MOUSEMOVE:
		return OnMouseMove(windex, wparam, lparam);

    case WM_LBUTTONDOWN:
		return OnLButtonDown(windex, wparam, lparam);

    case WM_RBUTTONDOWN:
		return OnRButtonDown(windex, wparam, lparam);

    case WM_LBUTTONUP:
		return OnLButtonUp(windex, wparam, lparam);

    case WM_RBUTTONUP:
		return OnRButtonUp(windex, wparam, lparam);
    };

    return DefWindowProc(my_Win[windex].hwnd, msg, wparam, lparam);
}

// 画面の終了
LRESULT ImageWindow::OnDestroy(long windex, WPARAM wparam, LPARAM lparam)
{
	IMGWIN& win = my_Win[windex];			// 使いやすいようにエイリアス

    DeleteObject(SelectObject(win.mem_hdc, win.mem_old_hbitmap));
    DeleteDC(win.mem_hdc);

	my_bIsOk[windex] = false;
    win.hwnd = NULL;
    return 0;
}
// 画面サイズ変更
LRESULT ImageWindow::OnSize(long windex, WPARAM wparam, LPARAM lparam)
{
	IMGWIN& win = my_Win[windex];			// 使いやすいようにエイリアス

    GetClientRect(win.hwnd, &win.hwnd_rect);
    set_scroll_range(windex, TRUE);
    return 0;
}

//LRESULT ImageWindow::OnSizing(long windex, WPARAM wparam, LPARAM lparam)
//{
//	IMGWIN& win = my_Win[windex];			// 使いやすいようにエイリアス
//
//	RECT r;
//	r.top = r.left = 0;
//	r.bottom = win.mem_rect.bottom;
//	r.right = win.mem_rect.right;
//	AdjustWindowRect(&r, WS_CAPTION | WS_THICKFRAME, FALSE);
//	r.right = (int)(win.zoom_x*(r.right - r.left));
//	r.bottom = (int)(win.zoom_y*(r.bottom - r.top));
//	RECT *s = (RECT *)lparam;
//	if ((s->right - s->left) > r.right) s->right = s->left + r.right;
//	if ((s->bottom - s->top) > r.bottom) s->bottom = s->top + r.bottom;
//
//    return 0;
//}
// 描画
LRESULT ImageWindow::OnPaint(long windex, WPARAM wparam, LPARAM lparam)
{
	IMGWIN& win = my_Win[windex];			// 使いやすいようにエイリアス

    PAINTSTRUCT paint;
    HDC dest = BeginPaint(win.hwnd, &paint);
	SetStretchBltMode(dest, my_gray_stretch_mode);
	EnterCriticalSection(&my_cs[windex]);
	
	long src_w = (int)(win.hwnd_rect.right / win.zoom_x);
	long src_h = (int)(win.hwnd_rect.bottom / win.zoom_y);
	if (src_w == win.hwnd_rect.right && src_h == win.hwnd_rect.bottom) {
		BitBlt(dest, 0, 0,
			win.hwnd_rect.right, win.hwnd_rect.bottom,
			win.mem_hdc, win.scroll_x.val, win.scroll_y.val,
			SRCCOPY);
	} else {
		StretchBlt(dest,
			0, 0,
			win.hwnd_rect.right, win.hwnd_rect.bottom,
			//
			win.mem_hdc, 
			win.scroll_x.val, win.scroll_y.val,
			src_w,
			src_h,
			SRCCOPY);
	}
	LeaveCriticalSection(&my_cs[windex]);
    EndPaint(win.hwnd, &paint);
    return 0;
}
// X軸スクロール
LRESULT ImageWindow::OnHScroll(long windex, WPARAM wparam, LPARAM lparam)
{
	IMGWIN& win = my_Win[windex];			// 使いやすいようにエイリアス
    const int tick = 1;

    int pos;
    switch( LOWORD(wparam) ) {
    case SB_LINELEFT:
        pos = win.scroll_x.val - tick;
        break;

    case SB_LINERIGHT:
        pos = win.scroll_x.val + tick;
        break;

    case SB_PAGELEFT:
        pos = win.scroll_x.val - tick * 10;
        break;

    case SB_PAGERIGHT:
        pos = win.scroll_x.val + tick * 10;
        break;

    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        pos = HIWORD(wparam);
        break;

    case SB_LEFT:
        pos = 0;
        break;

    case SB_RIGHT:
        pos = win.scroll_x.max;
        break;

    default:
        return 0;
    }
    set_scroll_pos_x(windex, pos, TRUE);
    InvalidateRect(win.hwnd, NULL, FALSE);
    return 0;
}
// Y軸スクロール
LRESULT ImageWindow::OnVScroll(long windex, WPARAM wparam, LPARAM lparam)
{
	IMGWIN& win = my_Win[windex];			// 使いやすいようにエイリアス
    const int tick = 1;

    int pos;
    switch( LOWORD(wparam) ) {
    case SB_LINEUP:
        pos = win.scroll_y.val - tick;
        break;

    case SB_LINEDOWN:
        pos = win.scroll_y.val + tick;
        break;

    case SB_PAGEUP:
        pos = win.scroll_y.val - tick * 10;
        break;

    case SB_PAGEDOWN:
        pos = win.scroll_y.val + tick * 10;
        break;
        
    case SB_THUMBPOSITION:
    case SB_THUMBTRACK:
        pos = HIWORD(wparam);
        break;

    case SB_TOP:
        pos = 0;
        break;

    case SB_BOTTOM:
        pos = win.scroll_y.max;
        break;

    default:
        return 0;
    }
    set_scroll_pos_y(windex, pos, TRUE);
    InvalidateRect(win.hwnd, NULL, FALSE);
    return 0;
}
// 画面上でのマウス移動
LRESULT ImageWindow::OnMouseMove(long windex, WPARAM wparam, LPARAM lparam)
{
	IMGWIN& win = my_Win[windex];			// 使いやすいようにエイリアス

    int x = GET_X_LPARAM(lparam);
    int y = GET_Y_LPARAM(lparam); 

    if( win.grab.sw ) {
        int dx = (int)(x / win.zoom_x) - win.grab.start.x;
        int dy = (int)(y / win.zoom_y) - win.grab.start.y;

        set_scroll_pos_x(windex, win.grab.scroll.x - dx, TRUE);
        set_scroll_pos_y(windex, win.grab.scroll.y - dy, TRUE);
        InvalidateRect(win.hwnd, NULL, FALSE);
                    
        return 0;
    }

	x = (int)(((x / win.zoom_x) + win.scroll_x.val) / win.scale_x);
	y = (int)(((y / win.zoom_y) + win.scroll_y.val) / win.scale_y);

	//// タイトルの再描画
    char buff[256];
    sprintf(buff, "%s(%d%%,%d%%) X=%d, Y=%d",
            win.title,
            (int)(win.zoom_x*100), (int)(win.zoom_y*100), x, y);
    SetWindowText(win.hwnd, buff);
    return 0;
}
// 左マウスボタン下げ
LRESULT ImageWindow::OnLButtonDown(long windex, WPARAM wparam, LPARAM lparam)
{
	IMGWIN& win = my_Win[windex];			// 使いやすいようにエイリアス

    if( wparam & MK_CONTROL ) {
        SetZoom(windex, win.zoom_x * 2, win.zoom_y * 2);
        return 0;
    }

    win.grab.sw = TRUE;
    win.grab.start.x = (int)(GET_X_LPARAM(lparam) / win.zoom_x);
    win.grab.start.y = (int)(GET_Y_LPARAM(lparam) / win.zoom_y);
    win.grab.scroll.x = win.scroll_x.val;
    win.grab.scroll.y = win.scroll_y.val;
    SetCapture(win.hwnd);
    HCURSOR hcur = (HCURSOR)LoadImage(my_hInst,
                                      MAKEINTRESOURCE(IDC_MOVE),
                                      IMAGE_CURSOR, 0, 0,
                                      LR_DEFAULTCOLOR);
    my_old_cursor = NULL;
    if( NULL != hcur ) {
        my_old_cursor = SetCursor(hcur);
    }
    return 0;
}
// 左マウスボタン上げ
LRESULT ImageWindow::OnLButtonUp(long windex, WPARAM wparam, LPARAM lparam)
{
    my_Win[windex].grab.sw = FALSE;
    ReleaseCapture();
    if( NULL != my_old_cursor ) {
        DestroyCursor(SetCursor(my_old_cursor));
    }
    return 0;
}
// 右マウスボタン下げ
LRESULT ImageWindow::OnRButtonDown(long windex, WPARAM wparam, LPARAM lparam)
{
    if( wparam & MK_CONTROL ) {
        SetZoom(windex,
                my_Win[windex].zoom_x / 2, my_Win[windex].zoom_y / 2);
        return 0;
    }
    return 0;
}
// 右マウスボタン上げ
LRESULT ImageWindow::OnRButtonUp(long windex, WPARAM wparam, LPARAM lparam)
{
    return 0;
}

//------------------------------------------
// 画面表示
//------------------------------------------
BOOL ImageWindow::ShowWindow(long windex, int show) const
{
    _ASSERT( NULL != my_Win[windex].hwnd );
    return ::ShowWindow(my_Win[windex].hwnd, show);
}
//------------------------------------------
// 画面更新
//------------------------------------------
BOOL ImageWindow::UpdateWindow(long windex) const
{
	_ASSERT( NULL != my_Win[windex].hwnd );
	return ::UpdateWindow(my_Win[windex].hwnd);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 自作の画面処理

//------------------------------------------
// スクロールバーの範囲
// long windex Winの配列 (0オリジン)
// BOOL redraw 再描画有無
//------------------------------------------
void ImageWindow::set_scroll_range(long windex, BOOL redraw)
{
	IMGWIN& win = my_Win[windex];			// 使いやすいようにエイリアス

	//// X
    win.scroll_x.max = win.mem_rect.right - (int)(win.hwnd_rect.right / win.zoom_x);
    if( win.scroll_x.max < 0 ) win.scroll_x.max = 0;
	if ( 0 == win.scroll_x.max ) {
		::ShowScrollBar(win.hwnd, SB_HORZ, FALSE);
	} else {
		::ShowScrollBar(win.hwnd, SB_HORZ, TRUE);
		::SetScrollRange(win.hwnd, SB_HORZ, 0, win.scroll_x.max, redraw);
	}
    if( win.scroll_x.val > win.scroll_x.max ) win.scroll_x.val = win.scroll_x.max;
 
	//// Y
    win.scroll_y.max = win.mem_rect.bottom - (int)(win.hwnd_rect.bottom / win.zoom_y);
    if( win.scroll_y.max < 0 ) win.scroll_y.max = 0;
	if ( 0 == win.scroll_y.max ) {
		::ShowScrollBar(win.hwnd, SB_VERT, FALSE);
	} else {
		::ShowScrollBar(win.hwnd, SB_VERT, TRUE);
		::SetScrollRange(win.hwnd, SB_VERT, 0, win.scroll_y.max, redraw);
	}
    if( win.scroll_y.val > win.scroll_y.max ) win.scroll_y.val = win.scroll_y.max;
}
//------------------------------------------
// スクロールバーの位置
// long windex Winの配列 (0オリジン)
// int x 位置
// BOOL redraw 再描画有無
//------------------------------------------
void ImageWindow::set_scroll_pos_x(long windex, int x, BOOL redraw)
{
	IMGWIN& win = my_Win[windex];			// 使いやすいようにエイリアス

    win.scroll_x.val = x;
    if( win.scroll_x.val < 0 ) {
        win.scroll_x.val = 0;
    } else if( win.scroll_x.val > win.scroll_x.max ) {
        win.scroll_x.val = win.scroll_x.max;
    }
	::SetScrollPos(win.hwnd, SB_HORZ, win.scroll_x.val, redraw);
}
void ImageWindow::set_scroll_pos_y(long windex, int y, BOOL redraw)
{
	IMGWIN& win = my_Win[windex];			// 使いやすいようにエイリアス

    win.scroll_y.val = y;
    if( win.scroll_y.val < 0 ) {
        win.scroll_y.val = 0;
    } else if( win.scroll_y.val > win.scroll_y.max ) {
        win.scroll_y.val = win.scroll_y.max;
    }
	::SetScrollPos(win.hwnd, SB_VERT, win.scroll_y.val, redraw);
}

//------------------------------------------
// 拡大
// float x 倍率
// float y 倍率
//------------------------------------------
void ImageWindow::SetZoom(long windex, float x, float y)
{
	IMGWIN& win = my_Win[windex];			// 使いやすいようにエイリアス
    win.zoom_x = x;
    win.zoom_y = y;
    set_scroll_range(windex, TRUE);
    InvalidateRect(win.hwnd, NULL, TRUE);
    SetTitle(windex, NULL);
}
//------------------------------------------
// 表示画像の倍率
//------------------------------------------
void ImageWindow::SetScale(long windex, float x, float y)
{
	IMGWIN& win = my_Win[windex];			// 使いやすいようにエイリアス
    win.scale_x = x;
    win.scale_y = y;
}
//------------------------------------------
// 画面サイズ
//------------------------------------------
void ImageWindow::SetSize(long windex, long cx, long cy)
{
	RECT r;
	r.top = r.left = 0;
	r.right = cx;
	r.bottom = cy;
	AdjustWindowRect(&r, WS_CAPTION | WS_THICKFRAME, FALSE);
	IMGWIN& win = my_Win[windex];			// 使いやすいようにエイリアス
	::SetWindowPos(win.hwnd, NULL, 0, 0, r.right-r.left, r.bottom-r.top, SWP_NOMOVE | SWP_NOZORDER);
}
//------------------------------------------
// 画面表示位置
//------------------------------------------
void ImageWindow::SetPos(long windex, long sx, long sy)
{
	IMGWIN& win = my_Win[windex];			// 使いやすいようにエイリアス
	::SetWindowPos(win.hwnd, NULL, sx, sy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

//------------------------------------------
// タイトル
// const char* title タイトル
//------------------------------------------
void ImageWindow::SetTitle(long windex, const char* title)
{
	IMGWIN& win = my_Win[windex];			// 使いやすいようにエイリアス

	//MultiThreadでwin.titleが競合すると嫌なので、tmp_titleで一旦うける
	if( NULL != title ) {
		char tmp_title[sizeof(win.title)]="";
		strcpy( tmp_title, title );
//        strncpy(win.title, title, sizeof(win.title)-1);
		memcpy( win.title, tmp_title, sizeof(win.title) );
    }

    char buff[256];
    sprintf(buff, "%s(%d%%,%d%%)",
            win.title,
            (int)(win.zoom_x*100), (int)(win.zoom_y*100));
    SetWindowText(win.hwnd, buff);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 描画関係

//------------------------------------------
// 画像表示
// long windex Winの配列 (0オリジン)
// BITMAPHEADERS const* bitheader ビットマップヘッダー (KizuBitmapManager::CreateBitmapHeader_Gray で作ったものでOK。反転はtrue設定)
// BYTE const* img 画像データ
// BOOL update 更新有無
//------------------------------------------
BOOL ImageWindow::Display(long windex, BITMAPHEADERS const* bitheader, BYTE const* img, BOOL update)
{
	IMGWIN& win = my_Win[windex];			// 使いやすいようにエイリアス

	////// ビットマップヘッダーから基本情報を取得
	int	w = bitheader->info.bmiHeader.biWidth;
	int h = abs(bitheader->info.bmiHeader.biHeight);	// 画像を反転させるために、－があるかもしれないため
	int nBitCount = bitheader->info.bmiHeader.biBitCount;
	LONG width  = (long)(win.scale_x*w);
    LONG height = (long)(win.scale_y*h);

	EnterCriticalSection(&my_cs[windex]);

    if( width != win.mem_rect.right || height != win.mem_rect.bottom )
    {
        win.mem_rect.right  = width;
        win.mem_rect.bottom = height;
        DeleteObject(SelectObject(win.mem_hdc,
                                  CreateCompatibleBitmap(win.mem_hdc,
                                                         width, height)));
    }
    set_scroll_range(windex, TRUE);
	if (nBitCount == 1) {
		SetStretchBltMode(win.mem_hdc, WHITEONBLACK);
	} else {
		SetStretchBltMode(win.mem_hdc, my_gray_stretch_mode);
	}
	BOOL ret = StretchBltBase(win.mem_hdc,
						bitheader, img,
                        0, 0, win.mem_rect.right, win.mem_rect.bottom,
                        0, 0, w, h);

	if (update) InvalidateRect(win.hwnd, NULL, FALSE);

	LeaveCriticalSection(&my_cs[windex]);
    return ret;
}

//------------------------------------------
// DCへの描画 (FCのDIBitmapにある)  (8bit、1bitに対応)
// HDC dst デバイスコンテキストのハンドル
// BITMAPHEADERS const* bitheader ビットマップヘッダー (KizuBitmapManager::CreateBitmapHeader_Gray で作ったものでOK。反転はtrue設定)
// BYTE const* img 画像データ
// long dst_x コピー先長方形の左上のX座標
// long dst_y コピー先長方形の左上のY座標
// long dst_w コピー先長方形の幅
// long dst_h コピー先長方形の高さ
// long x コピー元長方形の左上のX座標
// long y コピー元長方形の左上のY座標
// long w コピー元長方形の幅
// long h コピー元長方形の高さ
// DWORD rop ラスタオペレーションコード
//------------------------------------------
BOOL ImageWindow::StretchBltBase(HDC dst,
						   BITMAPHEADERS const* bitheader, BYTE const* img,
                           long dst_x, long dst_y, long dst_w, long dst_h,
                           long x, long y, long w, long h,
                           DWORD rop) const
{
	////// ビットマップヘッダーから基本情報を取得
    LONG width  = bitheader->info.bmiHeader.biWidth;
    LONG height = abs(bitheader->info.bmiHeader.biHeight);	// 画像を反転させるために、－があるかもしれないため

	//// サイズチェック
    if( x + w > width )  w = width - x;
    if( y + h > height ) h = height - y;

	//// 描画
    return GDI_ERROR != StretchDIBits(dst, dst_x, dst_y, dst_w, dst_h,
                                      x, height - (y + h), w, h,
                                      img, &bitheader->info,
                                      DIB_RGB_COLORS, rop);
	return 0;
}
//------------------------------------------
// 直線、斜線
// long windex Winの配列 (0オリジン)
// long x1, long y1, long x2, long y2 生画像に対する座標
// BOOL update 更新有無
//------------------------------------------
BOOL ImageWindow::DrawLine(long windex, long x1, long y1, long x2, long y2, BOOL update)
{
	IMGWIN& win = my_Win[windex];			// 使いやすいようにエイリアス

	EnterCriticalSection(&my_cs[windex]);

	long pen_width = (long)(1.0f / __min(win.zoom_x, win.zoom_y) + 0.5);
    HPEN new_pen = CreatePen(PS_SOLID, pen_width, my_current_color);
    HGDIOBJ old_pen = SelectObject(win.mem_hdc, new_pen);

    MoveToEx(win.mem_hdc, (long)(win.scale_x*x1), (long)(win.scale_y*y1), NULL);
    LineTo(win.mem_hdc, (long)(win.scale_x*x2), (long)(win.scale_y*y2));

    DeleteObject(SelectObject(win.mem_hdc, old_pen));

    if (update) InvalidateRect(win.hwnd, NULL, FALSE);
	
	LeaveCriticalSection(&my_cs[windex]);
    return TRUE;
}
//------------------------------------------
// 折れ線グラフ形式の線
// long windex Winの配列 (0オリジン)
// const POINT *pt 連続座標
// long len 配列数
// BOOL update 更新有無
//------------------------------------------
BOOL ImageWindow::DrawPolyline(long windex, const POINT *pt, long len, BOOL update)
{
	IMGWIN& win = my_Win[windex];			// 使いやすいようにエイリアス

	// 表示条件に再変換
	POINT * newpt = new POINT [len];
	for (int i = 0; i < len; i++) {
		newpt[i].x = (long)(win.scale_x * pt[i].x);
		newpt[i].y = (long)(win.scale_y * pt[i].y);
	}

	EnterCriticalSection(&my_cs[windex]);
	long pen_width = (long)(1.0f / __min(win.zoom_x, win.zoom_y) + 0.5);
    HPEN new_pen = CreatePen(PS_SOLID, pen_width, my_current_color);
    HGDIOBJ old_pen = SelectObject(win.mem_hdc, new_pen);

    Polyline(win.mem_hdc, newpt, len);

    DeleteObject(SelectObject(win.mem_hdc, old_pen));

    if (update) InvalidateRect(win.hwnd, NULL, FALSE);
	LeaveCriticalSection(&my_cs[windex]);

	delete [] newpt;
    return TRUE;
}
//------------------------------------------
// 矩形
// long windex Winの配列 (0オリジン)
// long x1, long y1, long x2, long y2 生画像に対する座標
// BOOL update 更新有無
//------------------------------------------
BOOL ImageWindow::DrawRect(long windex, long x1, long y1, long x2, long y2, BOOL update)
{
	IMGWIN& win = my_Win[windex];			// 使いやすいようにエイリアス

	EnterCriticalSection(&my_cs[windex]);
	long pen_width = (long)(1.0f / __min(win.zoom_x, win.zoom_y) + 0.5);
    HPEN new_pen = CreatePen(PS_SOLID, pen_width, my_current_color);
    HGDIOBJ old_pen = SelectObject(win.mem_hdc, new_pen);

    MoveToEx(win.mem_hdc, (long)(win.scale_x*x1), (long)(win.scale_y*y1), NULL);
    LineTo(win.mem_hdc, (long)(win.scale_x*x2), (long)(win.scale_y*y1));
	LineTo(win.mem_hdc, (long)(win.scale_x*x2), (long)(win.scale_y*y2));
	LineTo(win.mem_hdc, (long)(win.scale_x*x1), (long)(win.scale_y*y2));
	LineTo(win.mem_hdc, (long)(win.scale_x*x1), (long)(win.scale_y*y1));

    DeleteObject(SelectObject(win.mem_hdc, old_pen));

    if (update) InvalidateRect(win.hwnd, NULL, FALSE);
	LeaveCriticalSection(&my_cs[windex]);
    return TRUE;
}

//------------------------------------------
// 文字描画
// long windex Winの配列 (0オリジン)
// long x, long y 生画像に対する座標
// const char *text 文字列 (2バイト文字もOK)
// BOOL update 更新有無
//------------------------------------------
BOOL ImageWindow::DrawText(long windex, long x, long y, const char *text, BOOL update)
{
	IMGWIN& win = my_Win[windex];			// 使いやすいようにエイリアス

	EnterCriticalSection(&my_cs[windex]);
	::SetBkMode(win.mem_hdc, TRANSPARENT);
	::SetTextColor(win.mem_hdc, my_current_color);
	::TextOut(win.mem_hdc, (long)(win.scale_x*x), (long)(win.scale_y*y), text, (int)strlen(text));
    if (update) InvalidateRect(win.hwnd, NULL, FALSE);
	LeaveCriticalSection(&my_cs[windex]);
    return TRUE;
}