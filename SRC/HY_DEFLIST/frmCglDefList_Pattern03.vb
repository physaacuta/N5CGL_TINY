'*********************************************************************************
'　重欠陥一覧画面パターン03(画像数30枚)
'	[Ver]
'		Ver.01    2017/10/25  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Public Class frmCglDefList_Pattern03
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバークラス
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_lstKizuInf As New List(Of clsKizuInf.TYP_DEFECT_DSP)   '疵情報コントロールのクラス
    Private lblDmy As Label = New System.Windows.Forms.Label

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

        SetCtrl(1, UcDefect03_01, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(2, UcDefect03_02, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(3, UcDefect03_03, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(4, UcDefect03_04, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(5, UcDefect03_05, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(6, UcDefect03_06, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(7, UcDefect03_07, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(8, UcDefect03_08, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(9, UcDefect03_09, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(10, UcDefect03_10, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(11, UcDefect03_11, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(12, UcDefect03_12, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(13, UcDefect03_13, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(14, UcDefect03_14, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(15, UcDefect03_15, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(16, UcDefect03_16, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(17, UcDefect03_17, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(18, UcDefect03_18, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(19, UcDefect03_19, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(20, UcDefect03_20, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(21, UcDefect03_21, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(22, UcDefect03_22, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(23, UcDefect03_23, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(24, UcDefect03_24, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(25, UcDefect03_25, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(26, UcDefect03_26, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(27, UcDefect03_27, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(28, UcDefect03_28, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(29, UcDefect03_29, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(30, UcDefect03_30, typDefect)
        m_lstKizuInf.Add(typDefect)

        Call SetKizuInf(m_lstKizuInf)

    End Sub

    Private Sub SetCtrl(ByVal nNo As Integer, ByVal ucinf As ucDefect03, ByRef typdata As clsKizuInf.TYP_DEFECT_DSP)
        typdata.Initialize()
        typdata.nNo = nNo                               '情報No.
        typdata.pnlImage = ucinf.pnlImage               '画像パネル

        typdata.lblPos = lblDmy                         'F～
        typdata.lblWs = lblDmy                          'WS～
        typdata.lblDs = lblDmy                          'DS～
        typdata.lblType = lblDmy                        '疵種名
        typdata.lblGrade = lblDmy                       'グレード
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
