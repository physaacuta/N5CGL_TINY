// *********************************************************************************
//	�O���[�摜�A2�l�摜����ʂɕ\��
//	[Ver]
//		Ver.01    2010/03/04	����
//
//	[����]
//		�E
// *********************************************************************************

#pragma once

// �r���N���X �C���N���[�h
#include "ThreadManager.h"												// ���[�J�[�X���b�h�}�l�[�W���[


using namespace KizuLib;
class ImageWindow :	public ThreadManager
{

//// ���J�萔
public:
	static const int MAX_NIMAGE_WINDOW				= 16;				// �Ǘ�����ő��ʐ�
	static const int IDC_MOVE                       = 101;


#pragma pack(push, 1) // byte align��1byte�ɂ���
	// �r�b�g�}�b�v�w�b�_
	struct BITMAPHEADERS												// �r�b�g�}�b�v�w�b�_ (�`�͈Ⴄ���AKizuBitmapManager�Ɠ���)
	{
		BITMAPFILEHEADER file;
		BITMAPINFO       info;											// ����́ABITMAPINFOHEADER�ARGBQUAD ���������Ă���
	};
#pragma pack(pop)


//// ���J�֐�
public:
	ImageWindow();
	virtual ~ImageWindow(void);
	void SetThreadName(CString str) { this->my_sThreadName = str; }		// �X���b�h���̐ݒ�


	bool Init(int nWinNum, HINSTANCE hInst);							// �����ݒ�
	void Dispose();														// �I��

	// ����̉�ʏ���
	void		SetZoom(long windex, float x, float y);					// �g��
	void		SetScale(long windex, float x, float y);				// �\���摜�̔{��
	void		SetSize(long windex, long cx, long cy);					// ��ʃT�C�Y
	void		SetPos(long windex, long sx, long sy);					// ��ʕ\���ʒu
	void		SetTitle(long windex, const char* title);				// �^�C�g��

	void		SetColor(COLORREF ref) {my_current_color = ref;};		// �y���F�ݒ�
	void		SetStretchMode(int mode){my_gray_stretch_mode = mode;};	// �X�g���b�`���[�h

	BOOL		ShowWindow(long windex, int show) const;				// ��ʕ\��
	BOOL		UpdateWindow(long windex) const;						// ��ʍX�V

	// �`��֌W
	BOOL		Display(long windex, BITMAPHEADERS const* bitheader, BYTE const* img, BOOL update=TRUE); // �摜�\��
	
	BOOL		DrawLine(long windex, long x1, long y1, long x2, long y2, BOOL update);	// �����A�ΐ�
	BOOL		DrawPolyline(long windex, const POINT *pt, long len, BOOL update);		// �܂���O���t�`���̐�
	BOOL		DrawRect(long windex, long x1, long y1, long x2, long y2, BOOL update);	// ��`
	BOOL		DrawText(long windex, long x, long y, const char *text, BOOL update);	// �����`��

	template <class T> BOOL DrawRects(long windex, T *x1, T *y1, T *x2, T *y2, long len, T offset, BOOL update = TRUE)
	{
		IMGWIN& win = my_Win[windex];			// �g���₷���悤�ɃG�C���A�X
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



//// �����o�[�֐�
protected:
	int			ThreadFirst()	{ return 0;};							// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int			ThreadLast()	{ return 0;};							// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
	void		ThreadEvent(int nEventNo) {return;};					// �X���b�h�C�x���g����
	int			ThreadMainLoop();										// �X���b�h���C�����[�v

	// ��{����
	static LRESULT CALLBACK WndProcStatic(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	virtual BOOL			WndProcEntry(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT& res);
	virtual LRESULT			WndProcMain(long windex, UINT msg, WPARAM wparam, LPARAM lparam);

	virtual LRESULT			OnDestroy	 (long windex, WPARAM wparam, LPARAM lparam);	// ��ʂ̏I��
	virtual LRESULT			OnSize		 (long windex, WPARAM wparam, LPARAM lparam);	// ��ʃT�C�Y�ύX
	//virtual LRESULT			OnSizing	 (long windex, WPARAM wparam, LPARAM lparam);
	virtual LRESULT			OnPaint		 (long windex, WPARAM wparam, LPARAM lparam);	// �`��
	virtual LRESULT			OnHScroll	 (long windex, WPARAM wparam, LPARAM lparam);	// X���X�N���[��
	virtual LRESULT			OnVScroll	 (long windex, WPARAM wparam, LPARAM lparam);	// Y���X�N���[��
	virtual LRESULT			OnMouseMove	 (long windex, WPARAM wparam, LPARAM lparam);	// ��ʏ�ł̃}�E�X�ړ�
	virtual LRESULT			OnLButtonDown(long windex, WPARAM wparam, LPARAM lparam);	// ���}�E�X�{�^������
	virtual LRESULT			OnRButtonDown(long windex, WPARAM wparam, LPARAM lparam);	// ���}�E�X�{�^���グ	
	virtual LRESULT			OnLButtonUp	 (long windex, WPARAM wparam, LPARAM lparam);	// �E�}�E�X�{�^������
	virtual LRESULT			OnRButtonUp	 (long windex, WPARAM wparam, LPARAM lparam);	// �E�}�E�X�{�^���グ


	// ����̉�ʏ���
	void		set_scroll_range(long windex, BOOL redraw);				// �X�N���[���o�[�͈̔�
	void		set_scroll_pos_x(long windex, int x, BOOL redraw);		// �X�N���[���o�[�̈ʒu
	void		set_scroll_pos_y(long windex, int x, BOOL redraw);


    // DC�ւ̕`��
    virtual BOOL StretchBltBase(HDC dst,
							BITMAPHEADERS const* bitheader, BYTE const* img,
                            long dst_x, long dst_y, long dst_w, long dst_h,
                            long x, long y, long w, long h,
                            DWORD rop=SRCCOPY) const;


//// �����o�[�ϐ�
protected :
	// ��{
	static bool				my_bIsOk[MAX_NIMAGE_WINDOW];				// ��ʕ`��OK

	static ImageWindow*		my_ImageWindow[];							// ��ʖ{��
	static HINSTANCE		my_hInst;									// �x�[�X�̉�ʃC���X�^���X
	static ATOM				my_reg_atom;
    struct IMGWIN{
        char    title[256];						// ��ʃ^�C�g��

        HWND    hwnd;
        HBITMAP old_hbitmap;
        RECT    hwnd_rect;						// ���݂̃E�B���h�E�T�C�Y

        HDC     mem_hdc;
        HBITMAP mem_old_hbitmap;
        RECT    mem_rect;

        float   zoom_x;							// �g��{��
        float   zoom_y;

		float	scale_x;						// �\���摜�̔{��
		float	scale_y;

        struct {
            int val;
            int max;
        } scroll_x, scroll_y;					// �X�N���[���ݒ�

        struct {
            BOOL sw;
            POINT start;
            POINT scroll;
        } grab;
    }*						my_Win;										// ��ʏ��
	int						my_nWin;									// ��ʐ�

	HCURSOR					my_old_cursor;								// �O��̃}�E�X�J�[�\���`��
	COLORREF				my_current_color;							// ���݂̕`��y���F
	int						my_gray_stretch_mode;						// �X�g���b�`���[�h

	// ����
	HANDLE					my_evThReady;								// �X���b�h�̏��������V�O�i��
	CRITICAL_SECTION*		my_cs;										// �N���e�B�J���Z�N�V����
};
