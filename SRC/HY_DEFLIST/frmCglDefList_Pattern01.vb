'*********************************************************************************
'　重欠陥一覧画面パターン01(画像数5枚)
'	[Ver]
'		Ver.01    2017/10/25  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Public Class frmCglDefList_Pattern01

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバークラス
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_lstKizuInf As New List(Of clsKizuInf.TYP_DEFECT_DSP)   '疵情報コントロールのクラス

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

        SetCtrl(1, UcDefect01_01, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(2, UcDefect01_02, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(3, UcDefect01_03, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(4, UcDefect01_04, typDefect)
        m_lstKizuInf.Add(typDefect)
        SetCtrl(5, UcDefect01_05, typDefect)
        m_lstKizuInf.Add(typDefect)

        Call SetKizuInf(m_lstKizuInf)

    End Sub

    Private Sub SetCtrl(ByVal nNo As Integer, ByVal ucinf As ucDefect01, ByRef typdata As clsKizuInf.TYP_DEFECT_DSP)
        typdata.Initialize()
        typdata.nNo = nNo                               '情報No.
        typdata.pnlImage = ucinf.pnlImage               '画像パネル

        typdata.lblPos = ucinf.lblPos                   'F～
        typdata.lblWs = ucinf.lblWs                     'WS～
        typdata.lblDs = ucinf.lblDs                     'DS～
        typdata.lblType = ucinf.lblType                 '疵種名
        typdata.lblGrade = ucinf.lblGrade               'グレード
        typdata.lblSuccess = ucinf.lblSuccess           '周期
        typdata.lblSpeed = ucinf.lblSpeed               '速度
        typdata.lblPitch = ucinf.lblPitch               'ピッチ
        typdata.lblRoll = ucinf.lblRoll                 'ロール径

        typdata.lblPosTitle = ucinf.lblPosTitle         '疵位置タイトル
        typdata.lblProductNo = ucinf.lblProductNo       '生産番号
        typdata.lblDate = ucinf.lblSTime                '発生時刻

        typdata.lblImgName(0) = ucinf.lblKizuTitle01    '画像タイトル
        typdata.lblImgName(1) = ucinf.lblKizuTitle02    '画像タイトル
        typdata.lblTitleL(0) = ucinf.lblImag01_L        '画像右ラベル
        typdata.lblTitleL(1) = ucinf.lblImag02_L        '画像右ラベル
        typdata.lblTitleR(0) = ucinf.lblImag01_R        '画像左ラベル
        typdata.lblTitleR(1) = ucinf.lblImag02_R        '画像左ラベル

        typdata.lblRec = ucinf.lblRec
        typdata.lblPnt = ucinf.lblPnt
        typdata.lblKizuNo = ucinf.lblKizNo

        typdata.picImg(0) = ucinf.picImage01            '画像
        typdata.picImg(1) = ucinf.picImage02            '画像

    End Sub
End Class
