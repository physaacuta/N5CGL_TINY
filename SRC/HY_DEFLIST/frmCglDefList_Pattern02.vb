'*********************************************************************************
'　重欠陥一覧画面パターン02(画像数14枚)
'	[Ver]
'		Ver.01    2017/10/25  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Public Class frmCglDefList_Pattern02
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバークラス
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_lstKizuInf As New List(Of clsKizuInf.TYP_DEFECT_DSP)   '疵情報コントロールのクラス
    Dim lblDmy As Label = New System.Windows.Forms.Label

    Public Sub New()

        ' この呼び出しは、Windows フォーム デザイナで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。

        Call CreateKizu()

    End Sub


    ''' <summary>
    ''' 疵情報クラス生成
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub CreateKizu()
        Dim typDefect As clsKizuInf.TYP_DEFECT_DSP = Nothing

        SetCtrl(1, UcDefect02_01, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(2, UcDefect02_02, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(3, UcDefect02_03, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(4, UcDefect02_04, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(5, UcDefect02_05, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(6, UcDefect02_06, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(7, UcDefect02_07, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(8, UcDefect02_08, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(9, UcDefect02_09, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(10, UcDefect02_10, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(11, UcDefect02_11, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(12, UcDefect02_12, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(13, UcDefect02_13, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(14, UcDefect02_14, typDefect)
        m_lstKizuInf.Add(typDefect)

        Call SetKizuInf(m_lstKizuInf)

    End Sub

    Private Sub SetCtrl(ByVal nNo As Integer, ByVal ucinf As ucDefect02, ByRef typdata As clsKizuInf.TYP_DEFECT_DSP)
        typdata.Initialize()
        typdata.nNo = nNo                               '情報No.
        typdata.pnlImage = ucinf.pnlImage               '画像パネル

        typdata.lblPos = ucinf.lblPos                   'F～
        typdata.lblWs = ucinf.lblWs                     'WS～
        typdata.lblDs = lblDmy                          'DS～
        typdata.lblType = ucinf.lblType                 '疵種名
        typdata.lblGrade = ucinf.lblGrade               'グレード
        typdata.lblSuccess = lblDmy                     '周期
        typdata.lblSpeed = lblDmy                       '速度
        typdata.lblPitch = lblDmy                       'ピッチ
        typdata.lblRoll = lblDmy                        'ロール径

        typdata.lblPosTitle = ucinf.lblPosTitle         '疵位置タイトル
        typdata.lblProductNo = ucinf.lblProductNo       '生産番号
        typdata.lblDate = lblDmy                        '発生時刻

        typdata.lblImgName(0) = ucinf.lblKizuTitle01    '画像タイトル
        typdata.lblImgName(1) = ucinf.lblKizuTitle02    '画像タイトル
        typdata.lblTitleL(0) = lblDmy                   '画像左ラベル
        typdata.lblTitleL(1) = lblDmy                   '画像左ラベル
        typdata.lblTitleR(0) = lblDmy                   '画像右ラベル
        typdata.lblTitleR(1) = lblDmy                   '画像右ラベル

        typdata.lblRec = ucinf.lblRec
        typdata.lblPnt = ucinf.lblPnt
        typdata.lblKizuNo = ucinf.lblKizNo

        typdata.picImg(0) = ucinf.picImage01            '画像
        typdata.picImg(1) = ucinf.picImage02            '画像

    End Sub

End Class
