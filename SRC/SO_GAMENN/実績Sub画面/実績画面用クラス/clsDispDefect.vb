'*******************************************************************************
'   疵情報表示クラス
'
'  [Ver]
'       Ver.01  2013/12/25  初版
'
'	[メモ]
'       ksMemRead関数を使用しているため
'       [Option Strict Off]とする(エラー回避)
'*******************************************************************************
Option Strict On

Imports System.Reflection

Imports KizuLibCLI.KizuMod

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod

''' <summary>
''' 疵情報表示クラス
''' </summary>
''' <remarks></remarks>
Public Class clsDispDefect
	Implements IDisposable
	'///////////////////////////////////////////////////////////////////////////
	'定数定義
	'///////////////////////////////////////////////////////////////////////////
    Private Const DEFECT_ZOOM As Boolean = False        ' 疵が小さい時に拡大するか false:拡大しない true:拡大する
    Private Const DEFECT_DGV_WIDTH As Integer = 120     ' 疵情報DGV幅

    '///////////////////////////////////////////////////////////////////////////
    '構造体
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 疵表示情報
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure TYP_DEFECT_DSP
        Dim nNo As Integer                              '情報No.
        Dim lblTitleL() As Label                        '左タイトル
        Dim lblTitleR() As Label                        '右タイトル
        Dim picImg() As PictureBox                      '疵画像
        Dim lblImg() As Label                           '疵画像タイトル

        Dim lblType As Label                            ' 疵種名
        Dim lblGrade As Label                           ' グレード名
        Dim lblY As Label                               ' 長手位置
        Dim lblKizuNo As Label                          ' 疵No
        Dim lblYName As Label                           ' 長手位置(HED)
        Dim lblKizuNoName As Label                      ' 疵No(HED)
        Dim lblCamNo As Label                           ' カメラNo

        Dim clrLine As Color                            '補助線（メモリ及びオーバーレイ）の色
        Dim tblLayoutPnl As TableLayoutPanel
        Dim dgvInfo As tCnt.tCnt_DataGridView

        'イニシャライズ
        Public Sub Initialize()
            ReDim lblTitleL(DSP_IMAGE_MAX - 1)
            ReDim lblTitleR(DSP_IMAGE_MAX - 1)
            ReDim picImg(DSP_IMAGE_MAX - 1)
            ReDim lblImg(DSP_IMAGE_MAX - 1)
        End Sub
    End Structure

    '---------------------------------------------------------------------------
    ' 定数定義
    ''---------------------------------------------------------------------------
    ''' <summary>
    ''' 疵情報表示フォーマット
    ''' </summary>
    ''' <remarks></remarks>
    Private Enum EM_DEF_FORMAT
        emName1 = 0                                     ' ヘッダー1
        emData1                                         ' データ1
        emName2                                         ' ヘッダー2
        emData2                                         ' データ2
        emEnd
    End Enum

    Private Const DGV_HED_WIDTH As Integer = 80
    Private Const DGV_DATA_WIDTH As Integer = 48

    '///////////////////////////////////////////////////////////////////////////
    'メンバ変数
    '///////////////////////////////////////////////////////////////////////////
    Private m_bIsDesposed As Boolean = False                    'リソース開放フラグ[True:開放済,False:未開放]

    Private mcls_Param As clsParamManager                       'パラメータ管理
    Private mcls_Log As tClass_LogManager                       'ログ管理
    Private mcls_ImgDsp() As tClass_ImageDsp2                    '画像表示クラス
    Private mcls_DataMap As clsDataMap                          ' 疵マップデータクラス
    Private mcls_DataDef As clsDataDefect                       ' 疵画像クラス

    Private m_typMapValue As TYP_MAP_LIST_VALUE                 '表示選択疵

    Private m_typDefectDsp As TYP_DEFECT_DSP                    '疵情報表示

    Private m_bDspData As Boolean                               '疵情報表示状態 (True:表示中 False:非表示)
    Private m_bOverlay As Boolean                               'オーバーレイの表示有無 (True:表示 False:非表示)
    Private m_bScale As Boolean                                 'スケール（メモリ）表示(True:有効 False:無効)
    Private m_bImageZoom As Boolean                             '拡大表示(True:有効 False:無効)
    Private m_nZoomRate As Integer                              '疵画像の表示倍率
    Private m_nImageWidth As Integer                            '表示エリアの横幅[mm]
    Private m_emSide As EM_MAP_SIDE                             '疵画像ラベルDS位置[NORMAL:左がDS、REVERSE:左がWS]

    Private m_nImageExtend As Single = 1.5                      ' 小さい疵を拡大表示する倍の倍率設定
    Private m_extend As Boolean = False                         ' 表示している疵画像が拡大表示かどうかの設定
    Private m_picbox1 As PictureBox

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
            For Each dsp As tClass_ImageDsp2 In mcls_ImgDsp
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
            For Each dsp As tClass_ImageDsp2 In mcls_ImgDsp
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
            For Each dsp As tClass_ImageDsp2 In mcls_ImgDsp
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
            For Each dsp As tClass_ImageDsp2 In mcls_ImgDsp
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
            For Each dsp As tClass_ImageDsp2 In mcls_ImgDsp
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

    ''' <summary>
    ''' 現在表示中の疵情報
    ''' </summary>
    ''' <value>疵情報構造体</value>
    ''' <returns>疵情報構造体</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property CurrentDefect() As TYP_MAP_LIST_VALUE
        Get
            Return m_typMapValue
        End Get
    End Property

    ''' <summary>
    ''' 疵マップデータクラス設定
    ''' </summary>
    Public WriteOnly Property DataMap() As clsDataMap
        Set(value As clsDataMap)
            mcls_DataMap = value
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
            m_typMapValue.Initialize()

            mcls_Log = log
            mcls_Param = param
            m_typDefectDsp = typDefectDsp
            m_bDspData = False

            mcls_DataDef = New clsDataDefect(mcls_Param, gcls_Log)

            ReDim mcls_ImgDsp(m_typDefectDsp.picImg.Length - 1)

            For nCnt As Integer = 0 To m_typDefectDsp.picImg.Length - 1
                mcls_ImgDsp(nCnt) = New tClass_ImageDsp2()
                mcls_ImgDsp(nCnt).ScaleInterval = 5                               ' 目盛り詳細表示 3分割
                mcls_ImgDsp(nCnt).SetLogManager = mcls_Log                        ' ログ管理セット
                InitImageDsp(mcls_ImgDsp(nCnt), m_typDefectDsp.picImg(nCnt), strPath)
                m_typDefectDsp.lblImg(nCnt).Text = String.Empty
                m_typDefectDsp.lblCamNo.Text = String.Empty
                dgvInitialize(typDefectDsp.dgvInfo)
                AddHandler m_typDefectDsp.tblLayoutPnl.Resize, AddressOf Ctrl_ReSize
            Next

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
                For Each dsp As tClass_ImageDsp2 In mcls_ImgDsp               '画像表示の開放
                    If Not dsp Is Nothing Then
                        dsp.Dispose()
                        dsp = Nothing
                    End If
                Next
                If Not mcls_DataDef Is Nothing Then
                    mcls_DataDef.Dispose()
                    mcls_DataDef = Nothing
                End If
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
    Private Sub InitImageDsp(ByVal clsImgDsp As tClass_ImageDsp2, ByRef clsPic As PictureBox, ByVal strPath As String)
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
                .Color = m_typDefectDsp.clrLine
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
            For Each dsp As tClass_ImageDsp2 In mcls_ImgDsp
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
    ''' 疵情報表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspDefInf()
        Try
            '-------------------------------------------------------------------
            '疵情報を表示する
            '-------------------------------------------------------------------
            Dim typData As DETECT_BASE_DATA = mcls_DataDef.DefectData

            With m_typDefectDsp
                '疵情報表示
                .lblType.Text = mcls_Param.GetTypeName(typData.nTid)
                .lblGrade.Text = mcls_Param.GetGradeName(typData.nGid)
                .lblGrade.BackColor = mcls_Param.GetGradeColor(typData.nGid)
				'// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>				
				'.lblY.Text = Format(Int(typData.y / 100) / 10, "####0.0")
				.lblY.Text = Format(Int(typData.y_out / 100) / 10, "####0.0")
				'// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
                .lblKizuNo.Text = CStr(typData.nKizuNo)
                If 0 = typData.dsws Then
                    'DS
                    .dgvInfo.Item(EM_DEF_FORMAT.emData1, 0).Value = typData.ds.ToString("####0")

                    .dgvInfo.Item(EM_DEF_FORMAT.emData2, 0).Value = typData.ws.ToString("####0")

                    .dgvInfo.Item(EM_DEF_FORMAT.emData1, 0).Style.BackColor = Color.White
                    .dgvInfo.Item(EM_DEF_FORMAT.emData1, 0).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                    .dgvInfo.Item(EM_DEF_FORMAT.emData2, 0).Style.BackColor = Color.LightGray
                    .dgvInfo.Item(EM_DEF_FORMAT.emData2, 0).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

                Else
                    'WS
                    .dgvInfo.Item(EM_DEF_FORMAT.emData1, 0).Value = typData.ds.ToString("####0")

                    .dgvInfo.Item(EM_DEF_FORMAT.emData2, 0).Value = typData.ws.ToString("####0")

                    .dgvInfo.Item(EM_DEF_FORMAT.emData1, 0).Style.BackColor = Color.LightGray
                    .dgvInfo.Item(EM_DEF_FORMAT.emData1, 0).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                    .dgvInfo.Item(EM_DEF_FORMAT.emData2, 0).Style.BackColor = Color.White
                    .dgvInfo.Item(EM_DEF_FORMAT.emData2, 0).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

                End If

                .dgvInfo.Item(EM_DEF_FORMAT.emData2, 1).Value = typData.speed


                If 1 = typData.cycle_div Or 2 = typData.cycle_div Then                                    ' 周期性疵の場合
                    .dgvInfo.Item(EM_DEF_FORMAT.emData1, 1).Value = "有"
                    If 1 = typData.cycle_div Then
                        .dgvInfo.Item(EM_DEF_FORMAT.emData1, 1).Style.BackColor = g_ColorSimpleCycle
                        .dgvInfo.Item(EM_DEF_FORMAT.emData1, 1).Style.ForeColor = Color.Black
                    Else
                        .dgvInfo.Item(EM_DEF_FORMAT.emData1, 1).Style.BackColor = g_ColorKizuCycle
                        .dgvInfo.Item(EM_DEF_FORMAT.emData1, 1).Style.ForeColor = Color.White
                    End If
                    .dgvInfo.Item(EM_DEF_FORMAT.emData1, 2).Value = typData.pitch
                    .dgvInfo.Item(EM_DEF_FORMAT.emData2, 2).Value = typData.roolkei.ToString("####0")
                Else
                    .dgvInfo.Item(EM_DEF_FORMAT.emData1, 1).Value = "無"
                    .dgvInfo.Item(EM_DEF_FORMAT.emData1, 1).Style.BackColor = Color.White
                    .dgvInfo.Item(EM_DEF_FORMAT.emData1, 1).Style.ForeColor = Color.Black
                    .dgvInfo.Item(EM_DEF_FORMAT.emData1, 2).Value = ""
                    .dgvInfo.Item(EM_DEF_FORMAT.emData2, 2).Value = ""
                End If

            End With

            Dim nPos As Integer = 0                         '画像位置(0:上,1:下))
            For Each dsp As tClass_ImageDsp2 In mcls_ImgDsp
                dsp.tProp_AriaInf_ZoomW = CDbl(100.0 * typData.w_res / m_nImageWidth)
                dsp.tProp_AriaInf_ZoomH = CDbl(100.0 * typData.w_res / m_nImageWidth)

                Dim nNo As Integer = mcls_Param.GetTypeImage(typData.nTid, CType(nPos, EM_IMAGE_POS))
                If 0 >= nNo Then
                    If 1 = nPos Then
                        nNo = 2
                    Else
                        nNo = 1
                    End If
                End If
                '表示する画像をSET
                Dim imgData As Image = mcls_DataDef.DefectImage(nNo - 1)
                '画像種類に対応するタイトルをセットする
                m_typDefectDsp.lblImg(nPos).Text = mcls_DataDef.ImageTitle(nNo - 1)
                m_typDefectDsp.lblCamNo.Text = mcls_DataDef.DefectData.camset.ToString
                '---------------------------------------------------------------
                '疵画像を設定する
                '---------------------------------------------------------------
                If Not imgData Is Nothing Then                                       '正反射画像が取得できれば設定
                    dsp.SetImgInf(imgData.Width, imgData.Height, typData.w_res, typData.h_res, typData.center_x, typData.center_y)
                    dsp.SetImage(imgData, True)
                    dsp.tProp_Paint_IsDefImgDspn = False
                    dsp.tProp_AriaInf_IsCenter = True

                    '拡大表示時の倍率を設定
                    dsp.tProp_AriaInf_ZoomW = dsp.tProp_AriaInf_ZoomW * m_nImageExtend
                    dsp.tProp_AriaInf_ZoomH = dsp.tProp_AriaInf_ZoomH * m_nImageExtend

                    dsp.Calculation()

                    ' 拡大した状態で疵の外接枠が表示範囲に収まる場合は、拡大した画像を表示する
                    Dim sz As Point = dsp.GetSimplePoint_ImgToDsp(typData.box_x_max - typData.box_x_min, typData.box_y_max - typData.box_y_min)
                    If sz.X < m_picbox1.Width And sz.Y < m_picbox1.Height And DEFECT_ZOOM = True Then
                        '拡大表示ON
                        m_extend = True
                    Else
                        '拡大表示OFF
                        m_extend = False
                        '値を戻す
                        dsp.tProp_AriaInf_ZoomW = CDbl(100.0 * typData.w_res / m_nImageWidth)
                        dsp.tProp_AriaInf_ZoomH = CDbl(100.0 * typData.w_res / m_nImageWidth)
                        dsp.Calculation()
                    End If


                    If 0 < dsp.tProp_OverLayCnt Then        '既にオーバーレイが設定されていた場合
                        dsp.ResetOverLayInf()
                    End If

                    '-----------------------------------------------------------
                    'オーバーレイを設定する
                    '-----------------------------------------------------------
                    Dim oly As tClass_ImageDsp.OVERLAY_INF = Nothing
                    oly.bIsDsp = m_bOverlay
                    oly.Color = m_typDefectDsp.clrLine
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
    ''' 疵情報表示（実績画面用）
    ''' </summary>
    Public Sub DspDefect(ByVal nMen As Integer, ByVal typMapValue As TYP_MAP_LIST_VALUE, Optional ByVal bReTry As Boolean = False)

        Try
            Dim data As New DETECT_BASE_DATA
            If Not mcls_DataMap.DefectData(nMen, New clsDataMap.DEFECT_KEY(typMapValue.nKizuNo), data) Then
                Exit Try
            End If
            mcls_DataDef.SetDefect(mcls_DataMap.KizukenNo_In, data)
            m_typMapValue = typMapValue

            '疵情報表示
            DspDefInf()

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵情報表示異常[{0}]", ex.Message))
        End Try

    End Sub


    ''' <summary>
    ''' 表示初期化
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspInit()
        Try
            With m_typDefectDsp
                .lblType.Text = ""

                .lblGrade.Text = ""
                .lblGrade.BackColor = g_ColorControl

                .lblY.Text = ""
                .lblKizuNo.Text = ""

                Call DspDgvInit(.dgvInfo)

                For ii As Integer = 0 To .lblImg.Length - 1
                    .lblImg(ii).Text = ""
                Next

                .lblCamNo.Text = ""
            End With

            For Each dsp As tClass_ImageDsp2 In mcls_ImgDsp
                dsp.tProp_Paint_IsPaint = True
                dsp.tProp_Paint_IsDefImgDspn = True
            Next

            mcls_DataDef.DataInit()

            ' メンバ変数クリア
            m_bDspData = False
            m_typMapValue.Initialize()

            Refresh()

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵情報表示初期化表示異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵情報欄初期化
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub DspDgvInit(ByVal dgv As DataGridView)
        With dgv
            .Item(EM_DEF_FORMAT.emData1, 0).Value = ""

            .Item(EM_DEF_FORMAT.emData2, 0).Value = ""

            .Item(EM_DEF_FORMAT.emData1, 0).Style.BackColor = Color.White
            .Item(EM_DEF_FORMAT.emData1, 0).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .Item(EM_DEF_FORMAT.emData2, 0).Style.BackColor = Color.White
            .Item(EM_DEF_FORMAT.emData2, 0).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)


            .Item(EM_DEF_FORMAT.emData2, 1).Value = ""
            .Item(EM_DEF_FORMAT.emData1, 1).Value = "無"
            .Item(EM_DEF_FORMAT.emData1, 1).Style.BackColor = Color.White
            .Item(EM_DEF_FORMAT.emData1, 1).Style.ForeColor = Color.Black
            .Item(EM_DEF_FORMAT.emData1, 2).Value = ""
            .Item(EM_DEF_FORMAT.emData2, 2).Value = ""

        End With
    End Sub

    ''' <summary>
    ''' 表示更新
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub Refresh()
        Try

            For Each dsp As tClass_ImageDsp2 In mcls_ImgDsp
                dsp.Refresh()
            Next

            m_typDefectDsp.dgvInfo.Refresh()

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵情報表示更新異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵拡大画像表示
    ''' </summary>
    ''' <param name="clsPopup"></param>
    ''' <remarks></remarks>
    Public Sub ShowPopup(ByVal clsPopup As frmDefectImage)
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

            '画像が読み込まれていない場合は終了
            If Not mcls_DataDef.IsImgRead Then
                Exit Try
            End If

            '---------------------------
            '疵画像拡大表示
            '---------------------------
            With clsPopup
                .DispImgNo = m_typDefectDsp.nNo                                 '画像No.
                .SetVisibleMemory(MemoriVisible)                                'メモリの表示有無設定
                .SetVisibleOverlay(OverlayVisible)                              'オーバーレイの表示有無設定
                .SetColor = m_typDefectDsp.clrLine                              'メモリ、オーバレイ色
                .IsTurnLR = IsTurnLR                                            '左右反転設定
                .IsTurnTB = IsTurnTB                                            '上下反転設定
                .IsRotate90 = IsTurn90                                          '90度回転設定()
                .Side = Side                                                    '疵画像ラベルDS位置
                ''疵情報
                .SetDefectData(mcls_DataDef.DefectData, mcls_DataDef.DefectImage, mcls_DataDef.ImageTitle)
                '表示倍率
                .ZoomRate = CInt((100.0 * mcls_DataDef.DefectData.w_res / m_nImageWidth) * 100.0)
                '目盛り描画詳細
                .SetMemoryConfig(JISSEKI_SCALE_INTVAL_X, JISSEKI_SCALE_NUM_X, JISSEKI_SCALE_INTVAL_Y, JISSEKI_SCALE_NUM_Y)
                .DspUpDate(True)                                                          '表示
            End With
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵拡大画像表示異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵画像描画終了イベント
    ''' </summary>
    ''' <param name="e">グラフィックオブジェクト</param>
    ''' <remarks></remarks>
    Private Sub ImgDsp_tEv_Paint_End(ByVal e As System.Windows.Forms.PaintEventArgs)
        Dim nPosLineTop As Integer
        Dim nLineWidth As Integer

        Dim typAriaInf As tClass_ImageDsp.ARIA_INF                     ' 表示領域情報
        Dim typCalcAns As tClass_ImageDsp.CALC_ANS                     ' 座標最終結果
        Dim typMemori As tClass_ImageDsp.MEMORI_INF                    ' メモリ情報

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

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("詳細目盛描画異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵情報データグリッド初期処理
    ''' </summary>
    ''' <param name="dgv"></param>
    Private Sub dgvInitialize(ByVal dgv As tCnt.tCnt_DataGridView)
        '''' 基本を設定
        dgv.dgvInitialize(dgv)
        dgv.DataGridViewSelectionCancel(dgv)                           ' 選択色無し

        With dgv

            .RowCount = 0
            .RowTemplate.Height = 30

            '' 動作定義
            .ScrollBars = ScrollBars.Both
            .DoubleBuffered = True

            .ReadOnly = True                                                ' 読み取り専用
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' セル書き換え
            .ColumnHeadersVisible = False                                   ' 列ヘッダ表示なし

            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '選択色
            .DefaultCellStyle().SelectionForeColor = .DefaultCellStyle().ForeColor      '選択色
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .DefaultCellStyle.WrapMode = DataGridViewTriState.True

        End With
        Dim nWid As Integer = CInt((dgv.Size.Width - DGV_HED_WIDTH * 2) / 2)
        dgvColSetText(dgv, "Name1", DGV_HED_WIDTH, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter, , , True)
        dgvColSetText(dgv, "data1", nWid, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter, , , True)
        dgvColSetText(dgv, "Name2", DGV_HED_WIDTH, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter, , , True)
        dgvColSetText(dgv, "data2", nWid, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter, , , True)

        dgv.RowCount = 3
        For ii As Integer = 0 To dgv.RowCount - 1
            dgv.Item(EM_DEF_FORMAT.emName1, ii).Style.BackColor = Color.LightGray
            dgv.Item(EM_DEF_FORMAT.emName2, ii).Style.BackColor = Color.LightGray
            dgv.Item(EM_DEF_FORMAT.emName1, ii).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            dgv.Item(EM_DEF_FORMAT.emName2, ii).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            dgv.Item(EM_DEF_FORMAT.emName1, ii).Style.WrapMode = DataGridViewTriState.True
            dgv.Item(EM_DEF_FORMAT.emName2, ii).Style.WrapMode = DataGridViewTriState.True
        Next
        dgv.Item(EM_DEF_FORMAT.emName1, 0).Value = DIRECTION_NAME_DS & "～[mm]"
        dgv.Item(EM_DEF_FORMAT.emName1, 1).Value = "周期性"
        dgv.Item(EM_DEF_FORMAT.emData1, 1).Style.Alignment = DataGridViewContentAlignment.MiddleCenter
        dgv.Item(EM_DEF_FORMAT.emName1, 2).Value = "ピッチ"

        dgv.Item(EM_DEF_FORMAT.emName2, 0).Value = DIRECTION_NAME_WS & "～[mm]"
        dgv.Item(EM_DEF_FORMAT.emName2, 1).Value = "速度[mpm]"
        dgv.Item(EM_DEF_FORMAT.emName2, 2).Value = "径[mm]"

    End Sub


    ''///////////////////////////////////////////////////////////////////////////
    ''イベントハンドラ関数
    ''///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' リサイズ(pic)
    ''' </summary>
    ''' <param name="sender">イベント発生元</param>
    ''' <param name="e">イベントパラメータ</param>
    ''' <remarks></remarks>
    Public Sub Ctrl_ReSize(ByVal sender As Object, ByVal e As System.EventArgs)

        '画像
        For ii As Integer = 0 To mcls_ImgDsp.Length - 1
            If mcls_ImgDsp(ii) Is Nothing Then Continue For
            ' サイズ変更
            mcls_ImgDsp(ii).tProp_AriaInf_Aria = m_typDefectDsp.picImg(ii).ClientSize
            mcls_ImgDsp(ii).Calculation()
        Next

        '疵情報
        With m_typDefectDsp.dgvInfo
            Dim nWid As Integer = CInt((.Size.Width - DGV_HED_WIDTH * 2) / 2)
            If nWid < 0 Then
                nWid = CInt(.Size.Width / 2)
                .Columns(EM_DEF_FORMAT.emName1).Width = 0
                .Columns(EM_DEF_FORMAT.emData1).Width = nWid
                .Columns(EM_DEF_FORMAT.emName2).Width = 0
                .Columns(EM_DEF_FORMAT.emData2).Width = nWid
                For ii As Integer = 0 To .RowCount - 1
                    .Item(EM_DEF_FORMAT.emName1, ii).Style.BackColor = Color.LightGray
                    .Item(EM_DEF_FORMAT.emName2, ii).Style.BackColor = Color.LightGray
                    .Item(EM_DEF_FORMAT.emName1, ii).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                    .Item(EM_DEF_FORMAT.emName2, ii).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                Next
                m_typDefectDsp.lblYName.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                m_typDefectDsp.lblKizuNoName.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            Else
                If nWid < DGV_DATA_WIDTH Then
                    Dim nWidX As Integer = CInt((.Size.Width - DGV_DATA_WIDTH * 2) / 2)
                    .Columns(EM_DEF_FORMAT.emName1).Width = nWidX
                    .Columns(EM_DEF_FORMAT.emData1).Width = DGV_DATA_WIDTH
                    .Columns(EM_DEF_FORMAT.emName2).Width = nWidX
                    .Columns(EM_DEF_FORMAT.emData2).Width = DGV_DATA_WIDTH

                    For ii As Integer = 0 To .RowCount - 1
                        .Item(EM_DEF_FORMAT.emName1, ii).Style.BackColor = Color.LightGray
                        .Item(EM_DEF_FORMAT.emName2, ii).Style.BackColor = Color.LightGray
                        .Item(EM_DEF_FORMAT.emName1, ii).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 8.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                        .Item(EM_DEF_FORMAT.emName2, ii).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 8.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                    Next
                    m_typDefectDsp.lblYName.Font = New System.Drawing.Font("ＭＳ 明朝", 8.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                    m_typDefectDsp.lblKizuNoName.Font = New System.Drawing.Font("ＭＳ 明朝", 8.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                Else
                    .Columns(EM_DEF_FORMAT.emName1).Width = DGV_HED_WIDTH
                    .Columns(EM_DEF_FORMAT.emData1).Width = nWid
                    .Columns(EM_DEF_FORMAT.emName2).Width = DGV_HED_WIDTH
                    .Columns(EM_DEF_FORMAT.emData2).Width = nWid

                    For ii As Integer = 0 To .RowCount - 1
                        .Item(EM_DEF_FORMAT.emName1, ii).Style.BackColor = Color.LightGray
                        .Item(EM_DEF_FORMAT.emName2, ii).Style.BackColor = Color.LightGray
                        .Item(EM_DEF_FORMAT.emName1, ii).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                        .Item(EM_DEF_FORMAT.emName2, ii).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                    Next
                    m_typDefectDsp.lblYName.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                    m_typDefectDsp.lblKizuNoName.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                End If
            End If

        End With

        Refresh()

    End Sub

End Class
