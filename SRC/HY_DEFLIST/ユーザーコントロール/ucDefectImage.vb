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


    Public ReadOnly Property ImgDsp() As tClass_ImageDsp2
        Get
            Return mcls_ImgDsp
        End Get
    End Property


    Public Property ImgData() As Image
        Get
            Return m_imgData
        End Get
        Set(value As Image)
            m_imgData = value
        End Set
    End Property


    Public Sub New(ByVal clsLog As tClass_LogManager, ByVal strDefPath As String)

        ' この呼び出しはデザイナーで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。

        mcls_ImgDsp = New tClass_ImageDsp2()
        mcls_ImgDsp.ScaleInterval = 5                               ' 目盛り詳細表示 3分割
        mcls_ImgDsp.SetLogManager = clsLog                          ' ログ管理セット
        InitImageDsp(mcls_ImgDsp, picImg, strDefPath)               ' 画像表示クラス初期化

        ImgData = Nothing

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
