
'*********************************************************************************
'　疵一覧情報クラス
'	[Ver]
'		Ver.01    2017/11/01  初版
'
'	[メモ]
'
'*********************************************************************************
Option Strict Off

Imports System.Reflection

Imports KizuLibCLI.KizuMod

Imports tClassLibrary.tClass
Imports tClassLibrary.tMod

Public Class clsKizuInf
    Implements IDisposable

    '///////////////////////////////////////////////////////////////////////////
    '定数定義
    '///////////////////////////////////////////////////////////////////////////
    Private Const DEFECT_ZOOM As Boolean = False        ' 疵が小さい時に拡大するか false:拡大しない true:拡大する
    'Private m_ColorMemori As Color = Color.DarkGreen    ' オーバーレイ、目盛り表示色

    '///////////////////////////////////////////////////////////////////////////
    '構造体
    '///////////////////////////////////////////////////////////////////////////
    '画面疵情報構造体
    Public Structure TYP_DEFECT_DSP
        Dim nNo As Integer              '情報No.
        Dim pnlImage As Panel           '画像パネル

        Dim lblPos As Label             'F～
        Dim lblWs As Label              'WS～
        Dim lblDs As Label              'DS～
        Dim lblType As Label            '疵種名
        Dim lblGrade As Label           'グレード
        Dim lblSuccess As Label         '周期
        Dim lblSpeed As Label           '速度
        Dim lblPitch As Label           'ピッチ
        Dim lblRoll As Label            'ロール径

        Dim lblPosTitle As Label        '疵位置タイトル
        Dim lblProductNo As Label       '生産番号
        Dim lblDate As Label            '発生時刻

        Dim lblImgName() As Label       '画像タイトル
        Dim lblTitleL() As Label        '画像右ラベル
        Dim lblTitleR() As Label        '画像左ラベル

        Dim lblRec As Label             'レコード番号(デバッグ用)
        Dim lblPnt As Label             'ポインタ番号(デバッグ用)
        Dim lblKizuNo As Label          '疵No(デバッグ用)

        Dim picImg() As PictureBox      '画像
        'イニシャライズ
        Public Sub Initialize()
            ReDim lblImgName(DSP_IMAGE_MAX - 1)
            ReDim lblTitleL(DSP_IMAGE_MAX - 1)
            ReDim lblTitleR(DSP_IMAGE_MAX - 1)
            ReDim picImg(DSP_IMAGE_MAX - 1)
        End Sub
    End Structure
    '///////////////////////////////////////////////////////////////////////////
    'メンバ変数
    '///////////////////////////////////////////////////////////////////////////
    Private m_bIsDesposed As Boolean = False                    'リソース開放フラグ[True:開放済,False:未開放]

    Private mcls_Param As clsParamManager                       'パラメータ管理
    Private mcls_Log As tClass_LogManager                       'ログ管理
    Private mcls_ImgDsp() As tClass_ImageDsp                    '画像表示クラス

    Private m_nMen As Integer = -1                              '面[0:表、1:裏]
    Private m_nPnt As Integer = -1                              'TBL_HY_DEFLISTのレコードNo(-1:疵無し、0～ポインタ位置(データ有り))
    Private m_typDefectDsp As TYP_DEFECT_DSP                    '疵情報表示
    Private m_typDefData As DSP_LIST_DEFECT                     '疵情報(データ)

    Private m_bDspData As Boolean                               '疵情報表示状態 (True:表示中 False:非表示)
    Private m_bOverlay As Boolean                               'オーバーレイの表示有無 (True:表示 False:非表示)
    Private m_bScale As Boolean                                 'スケール（メモリ）表示(True:有効 False:無効)
    Private m_bImageZoom As Boolean                             '拡大表示(True:有効 False:無効)
    Private m_nZoomRate As Integer                              '疵画像の表示倍率
    Private m_nImageWidth As Integer = 25                       '表示エリアの横幅[mm]
    Private m_emSide As EM_MAP_SIDE                             '疵画像ラベルDS位置[NORMAL:左がDS、REVERSE:左がWS]

    Private m_nImageExtend As Single = 1.5                      ' 小さい疵を拡大表示する倍の倍率設定
    Private m_dRate As Single = 1.0                             ' 小さい疵を拡大表示する倍の倍率設定
    Private m_extend As Boolean = False                         ' 表示している疵画像が拡大表示かどうかの設定
    Private m_picbox1 As PictureBox
    Private m_colProduct As Color


    '///////////////////////////////////////////////////////////////////////////
    'プロパティ
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' オーバーレイ表示有無
    ''' </summary>
    ''' <value>True:表示 False:非表示</value>
    ''' <returns>True:表示 False:非表示</returns>
    ''' <remarks></remarks>
    Public Property OverlayVisible() As Boolean
        Get
            Return m_bOverlay
        End Get
        Set(ByVal value As Boolean)
            m_bOverlay = value
            For Each dsp As tClass_ImageDsp In mcls_ImgDsp
                If 0 < dsp.tProp_OverLayCnt Then
                    dsp.OverLayDspChange(m_bOverlay)
                End If
            Next
        End Set
    End Property

    ''' <summary>
    ''' 目盛り表示有無
    ''' </summary>
    ''' <value>True:表示 False:非表示</value>
    ''' <returns>True:表示 False:非表示</returns>
    ''' <remarks></remarks>
    Public Property MemoriVisible() As Boolean
        Get
            Return m_bScale
        End Get
        Set(ByVal value As Boolean)
            m_bScale = value
            Dim memTmp As tClass_ImageDspBase.MEMORI_INF
            For Each dsp As tClass_ImageDsp In mcls_ImgDsp
                memTmp = dsp.tProp_MemoriInf
                memTmp.bIsDsp = value
                dsp.tProp_MemoriInf = memTmp
                dsp.Refresh()
            Next
        End Set
    End Property

    ''' <summary>
    ''' 疵表示情報を取得
    ''' </summary>
    ''' <returns>疵表示情報</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property DefectViewInf() As TYP_DEFECT_DSP
        Get
            Return m_typDefectDsp
        End Get
    End Property

    ''' <summary>
    ''' 疵情報(キー情報)を取得
    ''' </summary>
    ''' <returns>疵表示情報</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property GetDefectKeyInf() As DSP_LIST_DEFECT
        Get
            Return m_typDefData
        End Get
    End Property

    ''' <summary>
    ''' 表示疵画像を取得
    ''' </summary>
    ''' <param name="emkind">0:上 1:下</param>
    ''' <returns>疵画像</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property DspImage(ByVal emkind As EM_IMAGE_POS) As Image
        Get
            Return mcls_ImgDsp(emkind).tProp_Paint_Img(0)
        End Get
    End Property

    ''' <summary>
    ''' 表示疵画像タイトルを取得
    ''' </summary>
    ''' <param name="emkind">0:上 1:下</param>
    ''' <returns>疵画像</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property DspImgTitle(ByVal emkind As EM_IMAGE_POS) As String
        Get
            Return m_typDefectDsp.lblImgName(emkind).Text
        End Get
    End Property

    ''' <summary>
    ''' デフォルト画像表示状態を取得します
    ''' </summary>
    ''' <param name="emkind">0:上 1:下</param>
    ''' <returns>True:デフォルト画像表示 False:疵画像を表示</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property IsDspDefault(ByVal emkind As EM_IMAGE_POS) As Boolean
        Get
            If mcls_ImgDsp(emkind) Is Nothing = True Then
                Return True
            End If

            Return mcls_ImgDsp(emkind).tProp_Paint_IsDefImgDspn
        End Get
    End Property

    ''' <summary>
    ''' 疵画像表示エリアの基準横幅[mm]
    ''' </summary>
    ''' <value>疵画像表示エリアの基準横幅[mm]</value>
    ''' <returns>疵画像表示エリアの基準横幅[mm]</returns>
    ''' <remarks></remarks>
    Public Property DspRate() As Single
        Get
            Return m_dRate
        End Get
        Set(ByVal value As Single)
            m_dRate = value
        End Set
    End Property

    ''' <summary>
    ''' 疵画像表示エリアの基準横幅[mm]
    ''' </summary>
    ''' <value>疵画像表示エリアの基準横幅[mm]</value>
    ''' <returns>疵画像表示エリアの基準横幅[mm]</returns>
    ''' <remarks></remarks>
    Public Property ImageWidth() As Integer
        Get
            Return m_nImageWidth
        End Get
        Set(ByVal value As Integer)
            m_nImageWidth = value
        End Set
    End Property

    ''' <summary>
    ''' 上下反転有無を取得
    ''' </summary>
    ''' <value>NORMAL:反転無し、REVERSE:反転有り</value>
    ''' <returns>NORMAL:反転無し、REVERSE:反転有り</returns>
    ''' <remarks></remarks>
    Public Property IsTurnTB() As EM_IMAGE_TOP
        Get
            If mcls_ImgDsp(0).tProp_ImgKind_IsTurnTB Then
                Return EM_IMAGE_TOP.REVERSE
            Else
                Return EM_IMAGE_TOP.NORMAL
            End If
        End Get
        Set(ByVal value As EM_IMAGE_TOP)
            For Each dsp As tClass_ImageDsp In mcls_ImgDsp
                If value = EM_IMAGE_TOP.REVERSE Then
                    dsp.tProp_ImgKind_IsTurnTB = True
                Else
                    dsp.tProp_ImgKind_IsTurnTB = False
                End If
            Next
        End Set
    End Property

    ''' <summary>
    ''' 左右反転有無を取得
    ''' </summary>
    ''' <value>NORMAL:反転無し、REVERSE:反転有り</value>
    ''' <returns>NORMAL:反転無し、REVERSE:反転有り</returns>
    ''' <remarks></remarks>
    Public Property IsTurnLR() As EM_IMAGE_SIDE
        Get
            If mcls_ImgDsp(0).tProp_ImgKind_IsTurnLR Then
                Return EM_IMAGE_SIDE.REVERSE
            Else
                Return EM_IMAGE_SIDE.NORMAL
            End If
        End Get
        Set(ByVal value As EM_IMAGE_SIDE)
            For Each dsp As tClass_ImageDsp In mcls_ImgDsp
                If value = EM_IMAGE_SIDE.REVERSE Then
                    dsp.tProp_ImgKind_IsTurnLR = True
                Else
                    dsp.tProp_ImgKind_IsTurnLR = False
                End If
            Next
        End Set
    End Property

    ''' <summary>
    ''' 90度回転有無を取得
    ''' </summary>
    ''' <value>NORMAL:反転無し、LOTATE:反転有り</value>
    ''' <returns>NORMAL:反転無し、LOTATE:反転有り</returns>
    ''' <remarks></remarks>
    Public Property IsTurn90() As EM_IMAGE_90
        Get
            If mcls_ImgDsp(0).tProp_ImgKind_Rotate90 Then
                Return EM_IMAGE_90.LOTATE
            Else
                Return EM_IMAGE_90.NORMAL
            End If
        End Get
        Set(ByVal value As EM_IMAGE_90)
            For Each dsp As tClass_ImageDsp In mcls_ImgDsp
                If value = EM_IMAGE_90.LOTATE Then
                    dsp.tProp_ImgKind_Rotate90 = True
                Else
                    dsp.tProp_ImgKind_Rotate90 = False
                End If
            Next
        End Set
    End Property

    ''' <summary>
    ''' 疵情報表示状態の取得
    ''' </summary>
    ''' <returns>True:表示中 False:非表示中</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property IsDspData() As Boolean
        Get
            Return m_bDspData
        End Get
    End Property

    ''' <summary>
    ''' 拡大表示有効/無効
    ''' </summary>
    ''' <value>True:有効、False:無効</value>
    ''' <returns>True:有効、False:無効</returns>
    ''' <remarks></remarks>
    Public Property ImageZoom() As Boolean
        Get
            Return m_bImageZoom
        End Get
        Set(ByVal value As Boolean)
            m_bImageZoom = value
        End Set
    End Property

    ''' <summary>
    ''' 疵画像ラベルDS位置
    ''' </summary>
    ''' <value>NORMAL:左がDS、REVERSE:左がWS</value>
    ''' <returns>NORMAL:左がDS、REVERSE:左がWS</returns>
    ''' <remarks></remarks>
    Public Property Side() As EM_MAP_SIDE
        Get
            Return m_emSide
        End Get
        Set(ByVal value As EM_MAP_SIDE)
            m_emSide = value
            If m_emSide = EM_MAP_SIDE.NORMAL Then           '左がDS
                m_typDefectDsp.lblTitleL(0).Text = DIRECTION_NAME_DS
                m_typDefectDsp.lblTitleR(0).Text = DIRECTION_NAME_WS
                m_typDefectDsp.lblTitleL(1).Text = DIRECTION_NAME_DS
                m_typDefectDsp.lblTitleR(1).Text = DIRECTION_NAME_WS
            Else                                            '左がWS
                m_typDefectDsp.lblTitleL(0).Text = DIRECTION_NAME_WS
                m_typDefectDsp.lblTitleR(0).Text = DIRECTION_NAME_DS
                m_typDefectDsp.lblTitleL(1).Text = DIRECTION_NAME_WS
                m_typDefectDsp.lblTitleR(1).Text = DIRECTION_NAME_DS
            End If
        End Set
    End Property

    '---------------------------------------------------------------------------
    ' メソッド
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <param name="typDefectDsp">疵情報表示コントロール</param>
    ''' <param name="param">パラメータ管理</param>
    ''' <param name="log">ログ管理</param>
    ''' <param name="strPath">デフォルト画像のリソースパス</param>
    ''' <remarks></remarks>
    Public Sub New(ByVal typDefectDsp As TYP_DEFECT_DSP, ByRef param As clsParamManager, ByRef log As tClass_LogManager, ByVal strPath As String)
        Try
            mcls_Log = log
            mcls_Param = param
            m_typDefectDsp = typDefectDsp
            m_bDspData = False

            ReDim mcls_ImgDsp(m_typDefectDsp.picImg.Length - 1)

            For nCnt As Integer = 0 To m_typDefectDsp.picImg.Length - 1
                mcls_ImgDsp(nCnt) = New tClass_ImageDsp()
                InitImageDsp(mcls_ImgDsp(nCnt), m_typDefectDsp.picImg(nCnt), strPath)
                m_typDefectDsp.lblImgName(nCnt).Text = String.Empty
                'AddHandler m_typDefectDsp.tblLayoutPnl.Resize, AddressOf Ctrl_ReSize
            Next
            ImageZoom = True

            m_colProduct = g_ColorControl
            m_nZoomRate = 100                                                   '標準は倍率100%で表示

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("コンストラクタ異常[{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' デストラクタ
    ''' </summary>
    ''' <param name="disposing">True:明示的な呼び出し False:暗黙の呼び出し</param>
    ''' <remarks></remarks>
    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        Try
            If Not m_bIsDesposed Then
                'オブジェクト破棄
                For Each dsp As tClass_ImageDsp In mcls_ImgDsp               '画像表示の開放
                    If Not dsp Is Nothing Then
                        dsp.Dispose()
                        dsp = Nothing
                    End If
                Next
            End If
            m_bIsDesposed = True                                                'Dispose済みとする
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("オブジェクト破棄異常[{0}]", ex.Message))
        End Try
    End Sub

#Region " IDisposable Support "
    'このコードは、破棄可能なパターンを正しく実装できるように Visual Basic によって追加されました。
    Public Sub Dispose() Implements IDisposable.Dispose
        'このコードを変更しないでください。クリーンアップ コードを上の Dispose(ByVal disposing As Boolean) に記述します。
        Dispose(True)
        GC.SuppressFinalize(Me)
    End Sub
#End Region

    ''' <summary>
    ''' 画像表示初期化
    ''' </summary>
    ''' <param name="clsImgDsp">画像表示クラス</param>
    ''' <param name="clsPic">ピクチャーボックス</param>
    ''' <param name="strPath">表示画像のパス</param>
    ''' <remarks></remarks>
    Private Sub InitImageDsp(ByVal clsImgDsp As tClass_ImageDsp, ByRef clsPic As PictureBox, ByVal strPath As String)
        Try

            m_picbox1 = clsPic

            '---------------------------
            '表示画像取得
            '---------------------------
            Dim asmData As System.Reflection.Assembly = [Assembly].GetExecutingAssembly()
            Dim clsStream As System.IO.Stream = asmData.GetManifestResourceStream(strPath)
            Dim clsImg As Image = Image.FromStream(clsStream, False, True)
            clsStream.Close()

            '---------------------------
            '画像設定
            '---------------------------
            With clsImgDsp
                .SetImgInf(clsImg.Width, clsImg.Height, 1, 1)
                .tProp_AriaInf_Aria = clsPic.ClientSize
                .tProp_AriaInf_IsCenter = True
                .tProp_Paint_IsPaint = True
                .tProp_Paint_IsDefImgDspn = True
                .tProp_ImgKind_StrechType = tClass_ImageDsp_Calc.EM_IMG_STRECH.emRawWidth
                .Calculation()
                .AddHandler_PaintEvent(clsPic)
                'デフォルト画像設定
                .SetDefImage(clsImg)
            End With

            '---------------------------
            '目盛り設定
            '---------------------------
            Dim typMemo As tClass_ImageDspBase.MEMORI_INF
            With typMemo
                .bIsDsp = False
                .LineWidth = 2
                .FontSize = 12
                .Color = g_ColLstOverlay
                .HojoLineLen = 5
                .strBaseString = "0[mm]"
                .nPosStringBot = 15
                .nPosStringLeft = 5
                .nPosLineBot = 20
                .nPosLineLeft = 30
                .nLoopCntX = 6
                .nIntervalX = 15
                .nLoopCntY = 12
                .nIntervalY = 15
            End With
            clsImgDsp.tProp_MemoriInf = typMemo
            AddHandler clsImgDsp.tEv_Paint_End, AddressOf ImgDsp_tEv_Paint_End
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("画像表示初期化異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' スケールを設定します
    ''' </summary>
    ''' <param name="nIntvalX">X軸の1メモリ単位[mm]</param>
    ''' <param name="nNumX">X軸のメモリ数</param>
    ''' <param name="nIntvalY">Y軸の1メモリ単位[mm]</param>
    ''' <param name="nNumY">Y軸のメモリ数</param>
    ''' <remarks></remarks>
    Public Sub SetScaleConfig(ByVal nIntvalX As Integer, ByVal nNumX As Integer, ByVal nIntvalY As Integer, ByVal nNumY As Integer)
        Try
            For Each dsp As tClass_ImageDsp In mcls_ImgDsp
                Dim memo As tClass_ImageDsp.MEMORI_INF = dsp.tProp_MemoriInf
                memo.nIntervalX = nIntvalX
                memo.nIntervalY = nIntvalY
                memo.nLoopCntX = nNumX
                memo.nLoopCntY = nNumY
                dsp.tProp_MemoriInf = memo
            Next
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("画像表示クラスメモリ設定異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵画像を削除します
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DisposeImage()
        Try
            For ii As Integer = 0 To mcls_ImgDsp.Length - 1
                If mcls_ImgDsp(ii) Is Nothing = True Then
                    Continue For
                End If
                Dim old_img As Image = mcls_ImgDsp(ii).tProp_Paint_Img(0)
                If old_img Is Nothing = False Then                              '画像を捨てる
                    old_img.Dispose()
                End If
            Next
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("画像表示クラス画像破棄異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 画像取得
    ''' </summary>
    ''' <param name="typDefInf">疵情報</param>
    ''' <param name="emImgPos">画像位置</param>
    ''' <param name="strTitle">疵タイトル</param>
    ''' <returns>画像イメージ</returns>
    ''' <remarks></remarks>
    Private Function GetImage(ByVal typDefInf As DSP_TYP_DEFECTINF, ByVal emImgPos As EM_IMAGE_POS, ByRef strTitle As String) As Image
        Dim imgData As Image = Nothing

        Try
            '初期化
            strTitle = ""
            Dim strImgPath As String = Nothing

            Select Case CType(mcls_Param.GetTypeImage(typDefInf.data.nTid, emImgPos), EM_IMAGE_NO)
                Case EM_IMAGE_NO.S
                    strImgPath = typDefInf.image_path_s
                    strTitle = DEFECT_IMG_NAME(0)
                Case EM_IMAGE_NO.R1
                    strImgPath = typDefInf.image_path_r
                    strTitle = DEFECT_IMG_NAME(1)
                Case EM_IMAGE_NO.R2
                    strImgPath = typDefInf.image_path_r2
                    strTitle = DEFECT_IMG_NAME(2)
                Case EM_IMAGE_NO.R3
                    strImgPath = typDefInf.image_path_r3
                    strTitle = DEFECT_IMG_NAME(3)
                Case Else
                    'strTitle = String.Empty
                    If EM_IMAGE_POS.IMG2 = emImgPos Then
                        strImgPath = typDefInf.image_path_r
                        strTitle = DEFECT_IMG_NAME(1)
                    Else
                        strImgPath = typDefInf.image_path_s
                        strTitle = DEFECT_IMG_NAME(0)
                    End If
                    Return Nothing
            End Select

            If 0 = strImgPath.Length Then
                '画像保存パスが未指定(画像ファイルなし)
                Return Nothing
            End If

            Return tClass_BmpJpeg.BmpFileLoad(strImgPath)                   ' 画像読み込み

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("画像取得異常[{0}]", ex.Message))
        End Try

        Return imgData
    End Function


    ''' <summary>
    ''' 疵情報表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspDefInf(ByVal nType As modMain.EM_DSP_PATTERN)
        Try
            '-------------------------------------------------------------------
            '疵情報を表示する
            '-------------------------------------------------------------------
            Dim typData As DETECT_BASE_DATA = m_typDefData.typ.data

            With m_typDefectDsp
                'デバッグ用
                .lblRec.Text = ""
                .lblPnt.Text = ""
                .lblKizuNo.Text = typData.nKizuNo

                '疵情報表示
                .lblType.Text = mcls_Param.GetTypeName(typData.nTid)
                .lblGrade.Text = mcls_Param.GetGradeName(typData.nGid)
                .lblGrade.BackColor = mcls_Param.GetGradeColor(typData.nGid)
                .lblPos.Text = Format(Int(typData.y / 100) / 10, "####0.0")
                'If modMain.EM_DSP_PATTERN.nPatternNo02 = nType Then
                '    .lblPos.Text = Format(Int(typData.y / 100) / 10, "####0.0")
                'Else
                '    .lblPos.Text = Format(CInt(Fix(typData.y / 1000)), "####0")
                'End If

                If modMain.EM_DSP_PATTERN.nPatternNo02 = nType Then
                    '.lblWs.Text = CStr(Format(typData.ws, "###0.0"))
                    '.lblWs.Text = Format(CInt(Int(typData.ws)), "###0")
                    .lblWs.Text = typData.ws.ToString("####0")
                Else
                    If 0 = typData.dsws Then
                        .lblWs.Text = ""
                        '.lblDs.Text = Format(CInt(Int(typData.ds)), "###0")
                        .lblDs.Text = typData.ds.ToString("####0")
                    Else
                        '.lblWs.Text = Format(CInt(Int(typData.ws)), "###0")
                        .lblWs.Text = typData.ws.ToString("####0")
                        .lblDs.Text = ""
                    End If
                End If
                .lblSpeed.Text = typData.speed
                If 1 = typData.cycle_div Or 2 = typData.cycle_div Then                                    ' 周期性疵の場合
                    .lblSuccess.Text = "有"
                    If 1 = typData.cycle_div Then
                        .lblSuccess.BackColor = g_ColorSimpleCycle
                        .lblSuccess.ForeColor = Color.Black
                    Else
                        .lblSuccess.BackColor = g_ColorKizuCycle
                        .lblSuccess.ForeColor = Color.White
                    End If
                    .lblPitch.Text = typData.pitch
                    '.lblRoll.Text = typData.roolkei.ToString("####0")
                    .lblRoll.Text = Format(CInt(Int(typData.roolkei)), "###0")
                Else
                    .lblSuccess.Text = "無"
                    .lblSuccess.BackColor = Color.White
                    .lblSuccess.ForeColor = Color.Black
                    .lblPitch.Text = ""
                    .lblRoll.Text = ""
                End If


                .lblProductNo.Text = tMod_Common.ByteToString(m_typDefData.cCoilNo)
                .lblProductNo.BackColor = m_colProduct
                .lblDate.Text = String.Format("{0:0000}/{1:00}/{2:00} {3:00}:{4:00}:{5:00}",
                                              typData.time(0), typData.time(1), typData.time(2),
                                              typData.time(3), typData.time(4), typData.time(5))

            End With

            Dim nPos As Integer = 0                         '画像位置(0:上,1:下))
            DisposeImage()                                      ' 古い画像を破棄する
            For Each dsp As tClass_ImageDsp In mcls_ImgDsp
                dsp.tProp_AriaInf_ZoomW = CDbl(100.0 * typData.w_res / m_nImageWidth) * m_dRate
                dsp.tProp_AriaInf_ZoomH = CDbl(100.0 * typData.w_res / m_nImageWidth) * m_dRate
                Dim imgData As Image = GetImage(m_typDefData.typ, CType(nPos, EM_IMAGE_POS), m_typDefectDsp.lblImgName(nPos).Text)
                '---------------------------------------------------------------
                '疵画像を設定する
                '---------------------------------------------------------------
                If Not imgData Is Nothing Then                                       '正反射画像が取得できれば設定
                    dsp.SetImgInf(imgData.Width, imgData.Height, typData.w_res, typData.h_res, typData.center_x, typData.center_y)
                    dsp.SetImage(imgData, True)
                    dsp.tProp_Paint_IsDefImgDspn = False
                    dsp.tProp_AriaInf_IsCenter = True

                    '拡大表示時の倍率を設定
                    'dsp.tProp_AriaInf_ZoomW = dsp.tProp_AriaInf_ZoomW * m_nImageExtend
                    'dsp.tProp_AriaInf_ZoomH = dsp.tProp_AriaInf_ZoomH * m_nImageExtend


                    dsp.Calculation()

                    '' 拡大した状態で疵の外接枠が表示範囲に収まる場合は、拡大した画像を表示する
                    'Dim sz As Point = dsp.GetSimplePoint_ImgToDsp(typData.box_x_max - typData.box_x_min, typData.box_y_max - typData.box_y_min)
                    'If sz.X < m_picbox1.Width And sz.Y < m_picbox1.Height And DEFECT_ZOOM = True Then
                    '    '拡大表示ON
                    '    m_extend = True
                    'Else
                    '    '拡大表示OFF
                    '    m_extend = False
                    '    '値を戻す
                    '    dsp.tProp_AriaInf_ZoomW = CDbl(100.0 * typData.w_res / m_nImageWidth)
                    '    dsp.tProp_AriaInf_ZoomH = CDbl(100.0 * typData.w_res / m_nImageWidth)
                    '    dsp.Calculation()
                    'End If


                    If 0 < dsp.tProp_OverLayCnt Then        '既にオーバーレイが設定されていた場合
                        dsp.ResetOverLayInf()
                    End If

                    '-----------------------------------------------------------
                    'オーバーレイを設定する
                    '-----------------------------------------------------------
                    Dim oly As tClass_ImageDsp.OVERLAY_INF = Nothing
                    oly.bIsDsp = m_bOverlay
                    oly.Color = g_ColLstOverlay
                    oly.LineWidth = 2
                    oly.HoseiX = 10
                    oly.HoseiY = 10
                    oly.ovlPos1 = New Point(typData.box_x_min, typData.box_y_min)
                    oly.ovlPos2 = New Point(typData.box_x_max, typData.box_y_max)
                    dsp.SetOverLayInf(oly)                      'オーバーレイ設定

                Else                                            '失敗していればデフォルト画像を表示
                    dsp.tProp_Paint_IsDefImgDspn = True
                End If
                nPos += 1
            Next

            m_bDspData = True

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵情報表示異常[{0}]", ex.Message))
            m_bDspData = False

        Finally
            Refresh()
        End Try
    End Sub

    ''' <summary>
    ''' 疵情報表示処理で画像が取得できていない場合のリカバリー処理(探傷画面)
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspDefectRetry(ByVal nType As modMain.EM_DSP_PATTERN)

        Try
            '' 無効レコードのチェック 疵が選択されていない場合などは -1をセットしている
            If 0 > m_nMen Then
                '    'mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("[{0}]無効レコード[{1}]", TBL_HY_DEFECTINF_NAME, m_nNowRecord))
                Exit Try
            End If

            If 0 > m_nPnt Then
                '    'mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("[{0}]無効レコード[{1}]", TBL_HY_DEFECTINF_NAME, m_nNowRecord))
                Exit Try
            End If

            ' 表示する画像種類を疵種から判定し、上の画像と下に表示する画像パスをそれぞれ取得
            Dim strPath(1) As String
            For ii As Integer = 0 To strPath.Length - 1
                Select Case CType(mcls_Param.GetTypeImage(m_typDefData.typ.data.nTid, CType(ii, EM_IMAGE_POS)), EM_IMAGE_NO)
                    Case EM_IMAGE_NO.S
                        strPath(ii) = m_typDefData.typ.image_path_s
                    Case EM_IMAGE_NO.R1
                        strPath(ii) = m_typDefData.typ.image_path_r
                    Case EM_IMAGE_NO.R2
                        strPath(ii) = m_typDefData.typ.image_path_r2
                    Case EM_IMAGE_NO.R3
                        strPath(ii) = m_typDefData.typ.image_path_r3
                    Case Else
                        ' 不定？
                        'strPath(ii) = String.Empty
                        If EM_IMAGE_POS.IMG2 = CType(ii, EM_IMAGE_POS) Then
                            strPath(ii) = m_typDefData.typ.image_path_r
                        Else
                            strPath(ii) = m_typDefData.typ.image_path_s
                        End If
                End Select
            Next

            Dim nReadAddr As Integer
            Dim nRetc As ValueType = 0


            '疵情報ポインタ読み込み[共有メモリ]
            nReadAddr = SIZE_DSP_LIST_HEADER + SIZE_DSP_LIST_DATA * m_nMen + SIZE_DSP_LIST_DEFECT * m_nPnt
            m_typDefData = CType(ksMemRead(TBL_HY_DEFLIST_NAME, nReadAddr, GetType(DSP_LIST_DEFECT), nRetc), DSP_LIST_DEFECT)
            If Not CBool(nRetc) Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("[{0}]疵情報再表示 疵情報ポインタ読み込み失敗[{1}]", TBL_HY_DEFLIST_NAME, nReadAddr))
                Exit Try
            End If

            If strPath(0) = "" Or strPath(1) = "" Then
                ' 上下２画像のうちどちらかのパスが取得出来ていない場合は、
                ' 再度共有メモリから取得を試みる
                ' 共有メモリからデータ取得後、表示する画像種類を疵種から判定し、上の画像と下に表示する画像パスをそれぞれ取得
                Dim strPathNew(1) As String
                For ii As Integer = 0 To strPathNew.Length - 1
                    Select Case CType(mcls_Param.GetTypeImage(m_typDefData.typ.data.nTid, CType(ii, EM_IMAGE_POS)), EM_IMAGE_NO)
                        Case EM_IMAGE_NO.S
                            strPathNew(ii) = m_typDefData.typ.image_path_s
                        Case EM_IMAGE_NO.R1
                            strPathNew(ii) = m_typDefData.typ.image_path_r
                        Case EM_IMAGE_NO.R2
                            strPathNew(ii) = m_typDefData.typ.image_path_r2
                        Case EM_IMAGE_NO.R3
                            strPathNew(ii) = m_typDefData.typ.image_path_r3
                        Case Else
                            ' 不定？
                            If EM_IMAGE_POS.IMG2 = CType(ii, EM_IMAGE_POS) Then
                                strPathNew(ii) = m_typDefData.typ.image_path_r
                            Else
                                strPathNew(ii) = m_typDefData.typ.image_path_s
                            End If
                    End Select
                Next

                If strPath(0) <> strPathNew(0) Or strPath(1) <> strPathNew(1) Then
                    '今回のリカバリー処理で画像パスの取得が出来たので、疵情報表示
                    mcls_Log.Write(tClass_LogManager.LNO.MSG, String.Format("疵画像表示リカバリー[{0}]⇒[{1}] [{2}]⇒[{3}]", strPath(0), strPathNew(0), strPath(1), strPathNew(1)))
                    DspDefInf(nType)
                End If

            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵情報再表示異常[{0}]", ex.Message))
        End Try


    End Sub

    ''' <summary>
    ''' 疵情報表示
    ''' </summary>
    ''' <param name="nMen">面[0:表、1:裏]</param>
    ''' <param name="nPnt"></param>
    ''' <param name="typDefData">疵情報</param>
    ''' <param name="col"></param>
    Public Sub DspDefect(ByVal nMen As Integer, ByVal nPnt As Integer, ByVal typDefData As DSP_LIST_DEFECT, ByVal col As Color, ByVal nType As modMain.EM_DSP_PATTERN)

        Try
            m_nMen = nMen
            m_nPnt = nPnt
            m_typDefData = typDefData
            m_colProduct = col
            '疵情報表示
            DspDefInf(nType)

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵情報表示(データセット)異常[{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' 表示初期化
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspInit(Optional ByVal bDebug As Boolean = False)
        Try
            With m_typDefectDsp
                .pnlImage.BackColor = g_ColorControl        '画像パネル
                .lblType.Text = ""

                .lblGrade.Text = ""
                .lblGrade.BackColor = g_ColorControl
                .lblPos.Text = ""
                .lblWs.Text = ""
                .lblDs.Text = ""
                .lblSpeed.Text = ""
                .lblSuccess.Text = "無"
                .lblSuccess.BackColor = Color.White
                .lblSuccess.ForeColor = Color.Black
                .lblPitch.Text = ""
                .lblRoll.Text = ""

                .lblPosTitle.Text = ""        '疵位置タイトル
                .lblPosTitle.BackColor = g_ColorControl
                .lblProductNo.Text = ""       '生産番号
                .lblProductNo.BackColor = g_ColorControl
                .lblDate.Text = ""            '発生時刻

                .lblRec.Text = ""
                .lblPnt.Text = ""
                .lblKizuNo.Text = ""
                If bDebug Then
                    .lblRec.Visible = True
                    .lblPnt.Visible = True
                    .lblKizuNo.Visible = True
                Else
                    .lblRec.Visible = False
                    .lblPnt.Visible = False
                    .lblKizuNo.Visible = False
                End If
                For ii As Integer = 0 To .lblImgName.Length - 1
                    .lblImgName(ii).Text = ""
                Next
            End With

            For Each dsp As tClass_ImageDsp In mcls_ImgDsp
                dsp.tProp_Paint_IsPaint = True
                dsp.tProp_Paint_IsDefImgDspn = True
            Next

            ' メンバ変数クリア
            m_bDspData = False
            m_nMen = -1
            m_nPnt = -1
            m_typDefData.cKanriNo = ""
            m_typDefData.cCoilNo = Nothing
            m_typDefData.cMeiNo = Nothing
            m_typDefData.typ = Nothing
            m_typDefData.initialize()



            Refresh()

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵情報表示初期化表示異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 表示更新
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub Refresh()
        Try

            For Each dsp As tClass_ImageDsp In mcls_ImgDsp
                dsp.Refresh()
            Next
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵情報表示更新異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵拡大画像表示
    ''' </summary>
    ''' <param name="clsPopup"></param>
    ''' <remarks></remarks>
    Public Function ShowPopup(ByVal clsPopup As frmDefectImage) As Boolean
        Dim bRet As Boolean = False
        Dim clsImg(DSP_KIRI_IMAGE_MAX - 1) As Image
        Dim tmpTitle(DSP_KIRI_IMAGE_MAX - 1) As String
        Dim nRetc As ValueType = 0

        Try

            '疵画像が表示されていない
            If Not IsDspData Then
                Exit Try
            End If

            '画像が設定されていない場合は終了
            If mcls_ImgDsp(0) Is Nothing Then
                Exit Try
            End If

            '画像は存在しない場合があるので表示中の画像を使用
            For ii As Integer = 0 To clsImg.Length - 1
                If IsDspDefault(ii) Then
                    clsImg(ii) = Nothing
                Else
                    clsImg(ii) = DspImage(ii)
                End If
                tmpTitle(ii) = DspImgTitle(ii)
            Next

            '---------------------------
            '疵画像拡大表示
            '---------------------------
            With clsPopup
                .DispImgNo = m_typDefectDsp.nNo                                 '画像No.
                .SetVisibleMemory(MemoriVisible)                                'メモリの表示有無設定
                .SetVisibleOverlay(OverlayVisible)                              'オーバーレイの表示有無設定
                .SetColor = g_ColLstOverlay                                     'メモリ、オーバレイ色
                .IsTurnLR = IsTurnLR                                            '左右反転設定
                .IsTurnTB = IsTurnTB                                            '上下反転設定
                .IsRotate90 = IsTurn90                                          '90度回転設定()
                .Side = Side                                                    '疵画像ラベルDS位置
                ''疵情報
                .SetDefectData(m_typDefData, m_typDefData.typ.data, clsImg, tmpTitle)
                '表示倍率
                .ZoomRate = CInt((100.0 * m_typDefData.typ.data.w_res / m_nImageWidth) * 100.0)
                '目盛り描画詳細
                .SetMemoryConfig(TANSHO_SCALE_INTVAL_X, TANSHO_SCALE_NUM_X * 2, TANSHO_SCALE_INTVAL_Y, TANSHO_SCALE_NUM_Y * 2)
                .DspUpDate(True)                                                '表示

                bRet = True
            End With
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵拡大画像表示異常[{0}]", ex.Message))
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' 疵画像描画終了イベント
    ''' </summary>
    ''' <param name="e">グラフィックオブジェクト</param>
    ''' <remarks></remarks>
    Private Sub ImgDsp_tEv_Paint_End(ByVal e As System.Windows.Forms.PaintEventArgs)
        Dim ii As Integer
        Dim nPosLineTop As Integer
        Dim nLineWidth As Integer

        Dim typAriaInf As tClass_ImageDsp.ARIA_INF                     ' 表示領域情報
        Dim typCalcAns As tClass_ImageDsp.CALC_ANS                     ' 座標最終結果
        Dim typMemori As tClass_ImageDsp.MEMORI_INF                    ' メモリ情報
        Dim clsPen As Pen

        Try
            typMemori = mcls_ImgDsp(0).tProp_MemoriInf
            typAriaInf = mcls_ImgDsp(0).tProp_AriaInf
            typCalcAns = mcls_ImgDsp(0).tProp_CalcAns

            If EM_IMAGE_90.NORMAL = IsTurn90 Then
                nPosLineTop = typAriaInf.nAriaHeight - typMemori.nPosLineBot
            Else
                nPosLineTop = typAriaInf.nAriaWidth - typMemori.nPosLineBot
            End If
            nLineWidth = CInt(typMemori.LineWidth / 2)
            If nLineWidth <= 0 Then
                nLineWidth = 1
            End If

            'エッジカット不可表示
            With m_typDefectDsp
                Dim g As Graphics = e.Graphics
                Dim sf As New StringFormat
                Dim f As New Font("MS Pゴシック", 9)
                If m_extend Then
                    sf.Alignment = StringAlignment.Center
                    sf.LineAlignment = StringAlignment.Center
                    g.DrawString("拡大表示", f, Brushes.Yellow, m_picbox1.Width - 40, m_picbox1.Height - 9, sf)
                End If

            End With

            If (MemoriVisible = True) Then
                clsPen = New System.Drawing.Pen(typMemori.Color, nLineWidth)
                'Ｘ軸表示
                For ii = 1 To typMemori.nLoopCntX * typMemori.nIntervalX
                    If ii Mod (typMemori.nIntervalX \ 5) = 0 Then
                        Dim nPos As Integer = typMemori.nPosLineLeft + CInt(ii * typCalcAns.RateRes.dW)
                        '区切り線
                        e.Graphics.DrawLine(clsPen, nPos, nPosLineTop, nPos, nPosLineTop - CInt(typMemori.HojoLineLen / 2))
                    End If
                Next ii

                'Ｙ軸表示
                For ii = 1 To typMemori.nLoopCntY * typMemori.nIntervalY
                    If ii Mod (typMemori.nIntervalY \ 5) = 0 Then
                        Dim nPos As Integer = nPosLineTop - CInt(ii * typCalcAns.RateRes.dH)
                        '区切り線
                        e.Graphics.DrawLine(clsPen, typMemori.nPosLineLeft, nPos, typMemori.nPosLineLeft + CInt(typMemori.HojoLineLen / 2), nPos)
                    End If
                Next ii
                clsPen.Dispose()
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("詳細目盛描画異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 最新画像枠表示
    ''' </summary>
    ''' <param name="colValue">色</param>
    ''' <remarks></remarks>
    Public Sub ChangePnlImage(ByVal colValue As Color)

        'オブジェクトがない場合は、終了
        If m_typDefectDsp.pnlImage Is Nothing Then Exit Sub

        '背景色表示
        m_typDefectDsp.pnlImage.BackColor = colValue
    End Sub


    ''' <summary>
    ''' 検査台(前or後),ロック中ラベル表示
    ''' </summary>
    ''' <param name="strValue"></param>
    ''' <remarks></remarks>
    Public Sub SetKensaLabel(ByVal strValue As String, ByVal colValue As Color)

        'オブジェクトがない場合は、終了
        If m_typDefectDsp.lblPosTitle Is Nothing Then Exit Sub

        'データ表示
        m_typDefectDsp.lblPosTitle.Text = strValue
        m_typDefectDsp.lblPosTitle.BackColor = colValue

    End Sub

End Class
