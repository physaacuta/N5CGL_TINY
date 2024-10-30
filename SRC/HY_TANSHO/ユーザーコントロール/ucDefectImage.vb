'*******************************************************************************
'   疵画像拡大表示ユーザークラス
'
'  [Ver]
'       Ver.01  2017/09/08  初版
'
'	[メモ]
'*******************************************************************************
Option Strict On

Imports System.Reflection

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.tClass.tClass_LogManager
Imports tClassLibrary.tMod


Public Class ucDefectImage


    Private mcls_ImgDsp As tClass_ImageDsp2
    Private m_imgData As Image
    Private m_bLablePos As Boolean                  ' ラベル位置(TRUE:横、FALSE:縦)
    Private m_lblTitle As Label                     ' タイトルラベル
    Private m_lblImageSideL As Label                ' 画像左ラベル
    Private m_lblImageSideR As Label                ' 画像右ラベル


    ''' <summary>
    ''' 画像クラス
    ''' </summary>
    ''' <returns></returns>
    Public ReadOnly Property ImgDsp() As tClass_ImageDsp2
        Get
            Return mcls_ImgDsp
        End Get
    End Property


    ''' <summary>
    ''' 画像データ
    ''' </summary>
    ''' <returns></returns>
    Public Property ImgData() As Image
        Get
            Return m_imgData
        End Get
        Set(value As Image)
            m_imgData = value
        End Set
    End Property

    ''' <summary>
    ''' タイトル
    ''' </summary>
    Public WriteOnly Property Title() As String
        Set(value As String)
            m_lblTitle.Text = value
        End Set
    End Property

    ''' <summary>
    ''' 左ラベル
    ''' </summary>
    Public WriteOnly Property SideL() As String
        Set(value As String)
            If m_lblImageSideL IsNot Nothing Then m_lblImageSideL.Text = value
        End Set
    End Property

    ''' <summary>
    ''' 右ラベル
    ''' </summary>
    Public WriteOnly Property SideR() As String
        Set(value As String)
            If m_lblImageSideR IsNot Nothing Then m_lblImageSideR.Text = value
        End Set
    End Property

    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="clsLog"></param>
    ''' <param name="strDefPath"></param>
    ''' <param name="bLabelPos"></param>
    Public Sub New(ByRef clsLog As tClass_LogManager, ByVal strDefPath As String, ByVal bLabelPos As Boolean)

        ' この呼び出しはデザイナーで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。

        mcls_ImgDsp = New tClass_ImageDsp2()
        mcls_ImgDsp.ScaleInterval = 5                               ' 目盛り詳細表示 3分割
        mcls_ImgDsp.SetLogManager = clsLog                          ' ログ管理セット
        InitImageDsp(mcls_ImgDsp, picImg, strDefPath)               ' 画像表示クラス初期化

        m_bLablePos = bLabelPos

        If m_bLablePos Then
            ' 縦ラベルを非表示
            TableLayoutPanel1.ColumnStyles.Item(0).Width = 0
            lblTitleName_V.Visible = False
            lblImgSideL_V.Visible = False
            lblImgSideR_V.Visible = False
            m_lblTitle = lblTitleName_H
            m_lblImageSideR = lblImgSideR_H
            m_lblImageSideL = lblImgSideL_H
        Else
            ' 横ラベルを非表示
            TableLayoutPanel1.RowStyles.Item(0).Height = 0
            lblTitleName_H.Visible = False
            lblImgSideL_H.Visible = False
            lblImgSideR_H.Visible = False
            m_lblTitle = lblTitleName_V
            m_lblImageSideR = Nothing
            m_lblImageSideL = Nothing
        End If

        ImgData = Nothing
        Title = ""
        SideL = ""
        SideR = ""

    End Sub

    ''' <summary>
    ''' tClass_ImageDspの初期設定
    ''' </summary>
    ''' <param name="dsp">画像表示クラス</param>
    ''' <param name="pic">ピクチャーボックス</param>
    ''' <param name="strPath">デフォルト画像のリソースパス</param>
    ''' <remarks></remarks>
    Private Sub InitImageDsp(ByVal dsp As tClass_ImageDsp, ByRef pic As PictureBox, ByVal strPath As String)
        Try
            Dim asmData As System.Reflection.Assembly = [Assembly].GetExecutingAssembly()   ' アセンブリにアクセス
            Dim stream As IO.Stream = asmData.GetManifestResourceStream(strPath)
            Dim img As Image = Image.FromStream(stream, False, True)            ' 読み込みが早くなる
            stream.Close()

            With dsp
                .SetImgInf(img.Width, img.Height, 1, 1)
                .tProp_AriaInf_Aria = pic.ClientSize
                .tProp_AriaInf_IsCenter = True
                .tProp_Paint_IsPaint = True
                .tProp_Paint_IsDefImgDspn = True
                .tProp_ImgKind_StrechType = tClass_ImageDsp_Calc.EM_IMG_STRECH.emRawWidth
                .Calculation()

                .AddHandler_PaintEvent(pic)
                .SetDefImage(img)                                               ' デフォルト画像を設定

                Dim memo As tClass_ImageDspBase.MEMORI_INF
                memo.bIsDsp = False
                memo.LineWidth = 2                              'メモリ線サイズ
                memo.FontSize = 12                              'メモリフォントサイズ
                memo.Color = g_ColorMemori                      'メモリ表示色
                memo.HojoLineLen = 5                            '区切り線の高さ

                memo.strBaseString = "0[mm]"                    '原点表示文字
                memo.nPosStringBot = 15                         'X軸の文字表示位置(下から何ピクセル)
                memo.nPosStringLeft = 5                         'Y軸の文字表示位置(左から何ピクセル)

                memo.nPosLineBot = 20                           'X軸のライン表示位置(下から何ピクセル)
                memo.nPosLineLeft = 30                          'Y軸のライン表示位置(左から何ピクセル)

                memo.nLoopCntX = 5                              'X軸のメモリ表示回数
                memo.nIntervalX = 10                            'X軸のメモリ表示間隔[mm]

                memo.nLoopCntY = 5                              'Y軸のメモリ表示回数
                memo.nIntervalY = 10                            'Y軸のメモリ表示間隔[mm]
                .tProp_MemoriInf = memo

            End With
        Catch ex As Exception
            Log(LNO.ERR, String.Format("拡大画像 tClass_ImageDsp初期化異常 [{0}]", ex.Message))
        End Try
    End Sub

End Class
