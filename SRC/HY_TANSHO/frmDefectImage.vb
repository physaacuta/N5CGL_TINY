'*******************************************************************************
'   疵画像拡大表示
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
Public Class frmDefectImage
    '///////////////////////////////////////////////////////////////////////////
    '列挙体
    '///////////////////////////////////////////////////////////////////////////
    Public Enum EM_RATE_LIST
        emRate_XY                       '全体
        emRate_X                        '全幅
        emRate_Y                        '全長
        emRate_025                      '1/4倍
        emRate_050                      '1/2倍
        emRate_100                      '1倍
        emRate_150                      '1.5倍
        emRate_200                      '2倍
        emRate_300                      '3倍
        emRate_400                      '4倍
        RATE_END
    End Enum

    ''' <summary>
    ''' 
    ''' </summary>
    Private Enum EM_ROW_DEFECTINF
        TYPE = 0                        ' 疵種名
        GRADE                           ' グレード名
        FPOS                            ' コイル先端からの距離
        DS                              ' DSからの距離
        WS                              ' WSからの距離
        LENGTH                          ' 疵長さ
        WIDTH                           ' 疵幅
        NO                              ' 疵No
        TORB                            ' 表裏区分
        CYCLE_DIV                       ' 周期疵区分
        DSWS                            ' DSWS区分
        CAMSET                          ' カメラセット
        CDS                             ' DS側板センターからの距離[mm]
        CWS                             ' WS側板センターからの距離[mm]
        SPEED                           ' 速度[mpm]
        PITCH                           ' ピッチ[mm]
        ROOLKEI                         ' ロール径[mm]
        ARIA                            ' 面積[mm^2]
        H_RES                           ' 縦分解能[mm/pixel]
        W_RES                           ' 横分解能[mm/pixel]
        NFRAMENO                        ' フレームNo
        FRM_X_MIN                       ' フレーム外接Xmin[pixel]
        FRM_X_MAX                       ' フレーム外接Xmax[pixel]
        FRM_Y_MIN                       ' フレーム外接Ymin[pixel]
        FRM_Y_MAX                       ' フレーム外接Ymax[pixel]
        CULL_FRM_X_MIN                  ' 間引きフレーム外接Xmin[pixel]
        CULL_FRM_X_MAX                  ' 間引きフレーム外接Xmax[pixel]
        CULL_FRM_Y_MIN                  ' 間引きフレーム外接Ymin[pixel]
        CULL_FRM_Y_MAX                  ' 間引きフレーム外接Ymax[pixel]
        BOX_X_MIN                       ' 外接Xmin[pixel]
        BOX_X_MAX                       ' 外接Xmax[pixel]
        BOX_Y_MIN                       ' 外接Ymin[pixel]
        BOX_Y_MAX                       ' 外接Ymax[pixel]
        BASE_BOX_X_MIN                  ' 元画像_外接Xmin[pixel]
        BASE_BOX_X_MAX                  ' 元画像_外接Xmax[pixel]
        BASE_BOX_Y_MIN                  ' 元画像_外接Ymin[pixel]
        BASE_BOX_Y_MAX                  ' 元画像_外接Ymax[pixel]
        BASE_CENTER_X                   ' 元画像_疵中心位置X[pixel]
        BASE_CENTER_Y                   ' 元画像_疵中心位置Y[pixel]
        BASE_H_SIZE                     ' 元画像_縦画素数[pixel]
        BASE_W_SIZE                     ' 元画像_横画素数[pixel]
        BASE_H_RES                      ' 元画像_縦分解能[mm/pixel]
        BASE_W_RES                      ' 元画像_横分解能[mm/pixel]
        SHRINK_RATIO                    ' 縮小率 (1, 2, 4)
        CENTER_X                        ' 疵中心位置X[pixel]
        CENTER_Y                        ' 疵中心位置Y[pixel]
        H_SIZE                          ' 縦画素数[pixel]
        W_SIZE                          ' 横画素数[pixel]
        TYUU                            ' 疵種優先度
        GYUU                            ' グレード優先度
        WIDABSPOS                       ' 幅方向絶対位置
        WIDDIV                          ' 幅分割区分
        NOBJTANSHO                      ' 探傷画面対象疵
        OBJDEFLIST                      ' 重欠陥一覧画面対象疵
        MODEDEFLIST                     ' 重欠陥一覧強調表示対象疵
        HASSEIDATE                      ' 疵発生日付
        MAX_ROW
    End Enum

    Private Enum EM_COL_DEFECTINF
        TYPE = 0                        ' 疵種名
        GRADE                           ' グレード名
        FPOS                            ' コイル先端からの距離
        MAX_COL
    End Enum

    Private Structure CMB_RATE_VALUE
        Dim emId As EM_RATE_LIST        ' ID
        Dim dValue As Double            ' 倍率係数
    End Structure

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_Log As tClass_LogManager                           ' ログ管理
    Private mcls_Param As clsParamManager                           ' パラメータ管理

    Private m_strSection As String                                  ' iniファイルセクション名

    Private m_bOverlay As Boolean                                   ' オーバーレイの表示有無 (True:表示 False:非表示)
    Private m_nClickPos As Point                                    ' 左クリック選択位置
    Private m_nDispTop As Integer                                   ' 表示位置(上端)
    Private m_nDispLeft As Integer                                  ' 表示位置(左端)
    Private m_nDispHeight As Integer                                ' 表示縦幅
    Private m_nDispWidth As Integer                                 ' 表示横幅
    Private m_nZoomRate As Integer                                  ' 画像拡大倍率

    Private m_emSide As EM_MAP_SIDE                                 ' 疵画像ラベルDS位置[NORMAL:左がDS、REVERSE:左がWS]
    Private m_colLine As Color
    Private m_nDispImgNo As Integer = 0                             ' 表示画像№[1=次疵,2=次々疵]
    Private m_DefectData As DETECT_BASE_DATA                        ' 疵情報

    Private m_ucDefectImg() As ucDefectImage

    '///////////////////////////////////////////////////////////////////////////
    'プロパティ
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 表示画像№
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Property DispImgNo() As Integer
        Get
            Return m_nDispImgNo
        End Get
        Set(ByVal value As Integer)
            m_nDispImgNo = value
        End Set
    End Property

    ''' <summary>
    ''' 画像拡大倍率[%]
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Property ZoomRate() As Integer
        Get
            Return m_nZoomRate
        End Get
        Set(ByVal value As Integer)
            m_nZoomRate = value
        End Set
    End Property

    ''' <summary>
    ''' オーバーレイ表示有無設定
    ''' </summary>
    ''' <param name="bVisible">True:表示 False:非表示</param>
    ''' <remarks></remarks>
    Public Sub SetVisibleOverlay(ByVal bVisible As Boolean)
        Try
            m_bOverlay = bVisible
            For Each uc As ucDefectImage In m_ucDefectImg
                uc.ImgDsp.OverLayDspChange(bVisible)
            Next
        Catch ex As Exception
            Log(LNO.ERR, String.Format("拡大画像 オーバーレイ表示設定異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' メモリ表示有無設定
    ''' </summary>
    ''' <param name="bVisible">True:表示 False:非表示</param>
    ''' <remarks></remarks>
    Public Sub SetVisibleMemory(ByVal bVisible As Boolean)
        Try
            For Each uc As ucDefectImage In m_ucDefectImg
                Dim memory As tClass_ImageDsp.MEMORI_INF = uc.ImgDsp.tProp_MemoriInf
                memory.bIsDsp = bVisible
                uc.ImgDsp.tProp_MemoriInf = memory
                uc.Refresh()
            Next
        Catch ex As Exception
            Log(LNO.ERR, String.Format("拡大画像 メモリ表示設定異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 上下反転有無を取得
    ''' </summary>
    ''' <value>NORMAL:反転無し、REVERSE:反転有り</value>
    ''' <returns>NORMAL:反転無し、REVERSE:反転有り</returns>
    ''' <remarks></remarks>
    Public Property IsTurnTB() As EM_IMAGE_TOP
        Get
            If m_ucDefectImg(0).ImgDsp.tProp_ImgKind_IsTurnTB Then
                Return EM_IMAGE_TOP.REVERSE
            Else
                Return EM_IMAGE_TOP.NORMAL
            End If
        End Get
        Set(ByVal value As EM_IMAGE_TOP)
            For Each uc As ucDefectImage In m_ucDefectImg
                If value = EM_IMAGE_TOP.REVERSE Then
                    uc.ImgDsp.tProp_ImgKind_IsTurnTB = True
                Else
                    uc.ImgDsp.tProp_ImgKind_IsTurnTB = False
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
            If m_ucDefectImg(0).ImgDsp.tProp_ImgKind_IsTurnLR Then
                Return EM_IMAGE_SIDE.REVERSE
            Else
                Return EM_IMAGE_SIDE.NORMAL
            End If
        End Get
        Set(ByVal value As EM_IMAGE_SIDE)
            For Each uc As ucDefectImage In m_ucDefectImg
                If value = EM_IMAGE_SIDE.REVERSE Then
                    uc.ImgDsp.tProp_ImgKind_IsTurnLR = True
                Else
                    uc.ImgDsp.tProp_ImgKind_IsTurnLR = False
                End If
            Next
        End Set
    End Property

    ''' <summary>
    ''' 画像の90度回転を取得
    ''' </summary>
    ''' <value>NORMAL:反転無し、REVERSE:反転有り</value>
    ''' <returns>NORMAL:反転無し、REVERSE:反転有り</returns>
    ''' <remarks></remarks>
    Public Property IsRotate90() As EM_IMAGE_90
        Get
            If m_ucDefectImg(0).ImgDsp.tProp_ImgKind_Rotate90 Then
                Return EM_IMAGE_90.LOTATE
            Else
                Return EM_IMAGE_90.NORMAL
            End If
        End Get
        Set(ByVal value As EM_IMAGE_90)
            For Each uc As ucDefectImage In m_ucDefectImg
                If value = EM_IMAGE_90.LOTATE Then
                    uc.ImgDsp.tProp_ImgKind_Rotate90 = True
                Else
                    uc.ImgDsp.tProp_ImgKind_Rotate90 = False
                End If
            Next
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
            For Each uc As ucDefectImage In m_ucDefectImg
                If m_emSide = EM_MAP_SIDE.REVERSE Then           '左がDS
                    uc.SideR = DIRECTION_NAME_DS
                    uc.SideL = DIRECTION_NAME_WS
                Else                                            '左がWS
                    uc.SideL = DIRECTION_NAME_DS
                    uc.SideR = DIRECTION_NAME_WS
                End If
            Next
        End Set
    End Property

    ''' <summary>
    ''' 表示位置(上端)
    ''' </summary>
    ''' <value></value>
    ''' <remarks></remarks>
    Public WriteOnly Property DispTop() As Integer
        Set(ByVal value As Integer)
            m_nDispTop = value
        End Set
    End Property

    ''' <summary>
    ''' 表示位置(左端)
    ''' </summary>
    ''' <value></value>
    ''' <remarks></remarks>
    Public WriteOnly Property DispLeft() As Integer
        Set(ByVal value As Integer)
            m_nDispLeft = value
        End Set
    End Property

    ''' <summary>
    ''' 表示縦幅
    ''' </summary>
    ''' <value></value>
    ''' <remarks></remarks>
    Public WriteOnly Property DispHeight() As Integer
        Set(ByVal value As Integer)
            m_nDispHeight = value
        End Set
    End Property

    ''' <summary>
    ''' 表示横幅
    ''' </summary>
    ''' <value></value>
    ''' <remarks></remarks>
    Public WriteOnly Property DispWidth() As Integer
        Set(ByVal value As Integer)
            m_nDispWidth = value
        End Set
    End Property

    ''' <summary>
    ''' オーバーレイ、メモリ色
    ''' </summary>
    Public WriteOnly Property SetColor As Color
        Set(value As Color)
            m_colLine = value
        End Set
    End Property
    '---------------------------------------------------------------------------
    ' メソッド
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <param name="clsLog">ログクラス</param>
    ''' <param name="param">パラメータクラス</param>
    ''' <param name="strDefPath">デフォルト画像パス</param>
    ''' <param name="strSection">セクション名</param>
    ''' <param name="nPicNum">表示画像数</param>
    ''' <param name="bPicDirect">画像並び順(false:通常、true：逆）</param>
    ''' <remarks></remarks>
    Public Sub New(ByRef clsLog As tClass_LogManager, ByRef param As clsParamManager, ByVal strDefPath As String, ByVal strSection As String, ByVal nPicNum As UShort, ByVal bPicDirect As Boolean)
        ' この呼び出しはデザイナーで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。

        Me.Text = String.Format("[{0}] 疵拡大画像画面", SYSTEM_TITLE)

        mcls_Log = clsLog
        mcls_Param = param

        m_strSection = strSection
        m_nClickPos = Point.Empty

        m_nZoomRate = 100

        ' 拡大画面リサイズ最小設定
        Dim s As Size
        s.Width = 400
        s.Height = 400
        Me.MinimumSize = s

        Dim nNum As Integer = 1
        ' テーブルレイアウトの列を追加        
        For ii As Integer = 1 To nPicNum - 1
            tlpImage.ColumnStyles.Add(New ColumnStyle())
            tlpImage.ColumnCount = tlpImage.ColumnCount + 1
        Next
        ' 列幅を等分化
        Dim c As ColumnStyle
        For Each c In tlpImage.ColumnStyles
            c.SizeType = SizeType.Percent
            c.Width = CSng(100 / tlpImage.ColumnCount)
        Next
        nNum = tlpImage.ColumnCount

        ReDim m_ucDefectImg(nNum - 1)

        For ii As Integer = 0 To m_ucDefectImg.Length - 1
            m_ucDefectImg(ii) = New ucDefectImage(clsLog, strDefPath, True)
            Dim pos As Integer = ii
            If bPicDirect Then
                '逆方向に画像を表示する
                pos = m_ucDefectImg.Length - (ii + 1)
            End If

            tlpImage.Controls.Add(m_ucDefectImg(ii), pos, 0)

            With m_ucDefectImg(ii)
                Dim location As Point
                Dim margin As Padding
                location.X = 0
                location.Y = 0
                margin.All = 0
                .Location = location
                .Margin = margin
                .Anchor = AnchorStyles.Top Or AnchorStyles.Bottom Or AnchorStyles.Left Or AnchorStyles.Right
                AddHandler .picImg.MouseDoubleClick, AddressOf picImage_MouseDoubleClick
                AddHandler .picImg.MouseWheel, AddressOf picImage_MouseWheel
                AddHandler .picImg.MouseMove, AddressOf picImage_MouseMove
                AddHandler .picImg.MouseDown, AddressOf picImage_MouseDown
            End With
        Next

        AddHandler vsbImg.ValueChanged, AddressOf vsbImg_ValueChanged
        AddHandler hsbImg.ValueChanged, AddressOf hsbImg_ValueChanged

        Me.StartPosition = FormStartPosition.Manual

        cmbPicRate_Init()
        AddHandler cmbPicRate.SelectedIndexChanged, AddressOf cmbPicRate_SelectedIndexChanged

        cbDetail.Checked = False
        AddHandler cbDetail.CheckedChanged, AddressOf cbDetail_CheckedChanged

        '-------------------------------------------------------------------
        ' ファンクションキー設定
        '-------------------------------------------------------------------
        AddFunctionButton(cmdEnd, 12 - 1)

        InitdgvDefInf(dgvKizuInfo)
        InitdgvCoilInf2(dgvKizuInfo2)
        AddHandler tlpTitle.Resize, AddressOf title_ReSize

    End Sub

    ''' <summary>
    ''' ロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmDetectImage_Load(sender As System.Object, e As System.EventArgs) Handles MyBase.Load
        For Each uc As ucDefectImage In m_ucDefectImg
            If uc.ImgDsp Is Nothing Then
                Continue For
            End If
            With uc.ImgDsp
                .tProp_AriaInf_Aria = uc.picImg.ClientSize
                .Calculation()
            End With
        Next

        ' 疵情報表示
        Dim hsc As HScrollBar
        Dim dgv As tCnt.tCnt_DataGridView

        dgv = dgvKizuInfo2
        hsc = CType(dgv.Controls(0), HScrollBar)
        If hsc.Visible Then
            '水平スクロールバー表示
            dgv.ColumnHeadersVisible = False
        Else
            dgv.ColumnHeadersVisible = True
        End If

    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 外部処理
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 最新の情報で疵情報を表示する
    ''' </summary>
    ''' <param name="bVisible">フォーム表示</param>
    ''' <remarks></remarks>
    Public Sub DspUpDate(ByVal bVisible As Boolean)

        If 0 >= m_DefectData.nKizuNo Then
            Exit Sub
        End If

        If Not Visible Then
            ReadPosition()
            SetLocationAndSize()
        End If

        ' スクロールバー初期化
        vsbImg.Value = 0
        hsbImg.Value = 0

        '''' 画像表示
        '' イメージ表示クラス準備セット
        For ii As Integer = 0 To m_ucDefectImg.Length - 1
            If m_ucDefectImg(ii).ImgDsp Is Nothing Then
                Continue For
            End If
            With m_ucDefectImg(ii)

                If bVisible Then
                    '' 基本情報セット
                    If .ImgData IsNot Nothing Then
                        .ImgDsp.SetImgInf(.ImgData.Width, .ImgData.Height,
                                                  m_DefectData.w_res, m_DefectData.h_res,
                                                  m_DefectData.center_x, m_DefectData.center_y, 1, 1)

                        If .ImgData.Width <> m_DefectData.w_size Or .ImgData.Height <> m_DefectData.h_size Then
                            Log(LNO.WAR, String.Format("画像サイズ不一致 img[w={0} h={1}] Data[w={2} h={3}]",
                                                                                .ImgData.Width, .ImgData.Height,
                                                                                m_DefectData.w_size, m_DefectData.h_size))
                        End If

                        .ImgDsp.tProp_AriaInf_Aria = .picImg.ClientSize             ' 全部同じサイズだから これでOK
                        '' オーバーレイ
                        Dim oly As tClass.tClass_ImageDsp.OVERLAY_INF = Nothing
                        oly.bIsDsp = m_bOverlay
                        oly.Tag = Nothing
                        oly.Color = m_colLine
                        oly.LineWidth = 2
                        oly.HoseiX = 10
                        oly.HoseiY = 10
                        oly.ovlPos1 = New Point(m_DefectData.box_x_min, m_DefectData.box_y_min)
                        oly.ovlPos2 = New Point(m_DefectData.box_x_max, m_DefectData.box_y_max)
                        .ImgDsp.ResetOverLayInf()
                        .ImgDsp.SetOverLayInf(oly)
                        '-----------------------------------------------------------
                        'スケール（目盛り）設定
                        '-----------------------------------------------------------
                        Dim memory As tClass_ImageDsp.MEMORI_INF = .ImgDsp.tProp_MemoriInf
                        memory.Color = m_colLine
                        .ImgDsp.tProp_MemoriInf = memory
                        ' 切り出し画像表示
                        .ImgDsp.SetImage(CType(.ImgData, Image))
                        .ImgDsp.tProp_Paint_IsDefImgDspn = False
                    Else
                        ' ×画像
                        .ImgDsp.tProp_Paint_IsDefImgDspn = True
                    End If

                    .ImgDsp.tProp_Paint_IsPaint = True                                                 ' 画像表示をON
                End If

                Rate_Change(.ImgDsp)
                .ImgDsp.tProp_AriaInf_IsCenter = True                                  ' 疵中心位置移動
                .ImgDsp.Calculation()                                                  ' 表示条件確定
                .ImgDsp.tProp_AriaInf_IsCenter = False                                 ' 疵中心位置移動無し
                .ImgDsp.Refresh()

            End With

        Next

        ' スクロールバー反映
        SetCommonScrollBar()
        For Each uc As ucDefectImage In m_ucDefectImg
            If uc.ImgDsp Is Nothing Then
                Continue For
            End If
            With uc.ImgDsp
                If vsbImg.Enabled Then
                    vsbImg.ValueProgramable = -1 * .tProp_AriaInf_Offset.Y   ' 現在値変更
                End If
                If hsbImg.Enabled Then
                    hsbImg.ValueProgramable = -1 * .tProp_AriaInf_Offset.X   ' 現在値変更
                End If
                Exit For
            End With
        Next

        If bVisible = True Then
            'ちらつくため、ここでVisible=Trueにする
            Me.Visible = True
            Me.Focus()
        End If

        'Me.Owner.Focus()

    End Sub

    ''' <summary>
    ''' スケール設定
    ''' </summary>
    ''' <param name="nIntvalX">X軸の1メモリ単位[mm]</param>
    ''' <param name="nNumX">X軸のメモリ数</param>
    ''' <param name="nIntvalY">Y軸の1メモリ単位[mm]</param>
    ''' <param name="nNumY">Y軸のメモリ数</param>
    ''' <remarks></remarks>
    Public Sub SetMemoryConfig(ByVal nIntvalX As Integer, ByVal nNumX As Integer, ByVal nIntvalY As Integer, ByVal nNumY As Integer)
        Try
            For Each uc As ucDefectImage In m_ucDefectImg
                If uc.ImgDsp Is Nothing Then
                    Continue For
                End If
                With uc.ImgDsp
                    Dim memo As tClass_ImageDsp.MEMORI_INF = .tProp_MemoriInf
                    memo.nIntervalX = nIntvalX
                    memo.nIntervalY = nIntvalY
                    memo.nLoopCntX = nNumX
                    memo.nLoopCntY = nNumY
                    .tProp_MemoriInf = memo
                End With
            Next
        Catch ex As Exception
            Log(LNO.ERR, String.Format("画像表示 メモリ設定異常[{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' 切り出し画像情報 セット
    ''' </summary>
    ''' <param name="img"></param>
    ''' <remarks></remarks>
    Public Sub SetDefectData(ByVal data As DETECT_BASE_DATA, ByVal img() As Image, ByVal title() As String)

        Try

            m_DefectData = data
            SetDgvData()

            For ii As Integer = 0 To m_ucDefectImg.Length - 1
                If img.Length > ii Then
                    m_ucDefectImg(ii).ImgData = img(ii)
                End If
                If title.Length > ii Then
                    m_ucDefectImg(ii).Title = title(ii)
                End If
            Next
        Catch ex As Exception
            Log(LNO.ERR, String.Format("画像表示 メモリ設定異常[{0}]", ex.Message))
        End Try

    End Sub

    '***************************************************************************
    ' リサイズイベント
    '***************************************************************************
    ''' <summary>
    ''' リサイズ
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub picImg_Resize(ByVal sender As Object, ByVal e As System.EventArgs) Handles tlpImage.Resize
        If m_ucDefectImg Is Nothing Then Exit Sub

        For Each uc As ucDefectImage In m_ucDefectImg
            If uc.ImgDsp Is Nothing Then
                Continue For
            End If
            With uc.ImgDsp
                .tProp_AriaInf_Aria = uc.picImg.ClientSize
                .Calculation()
            End With
        Next

        DspUpDate(False)

    End Sub

    ''' <summary>
    ''' リサイズ
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Public Sub title_ReSize(ByVal sender As Object, ByVal e As System.EventArgs)

        ' 疵情報表示
        Dim hsc As HScrollBar
        Dim dgv As tCnt.tCnt_DataGridView

        dgv = dgvKizuInfo2
        hsc = CType(dgv.Controls(0), HScrollBar)
        If hsc.Visible Then
            '水平スクロールバー表示
            dgv.ColumnHeadersVisible = False
        Else
            dgv.ColumnHeadersVisible = True
        End If

    End Sub

    '***************************************************************************
    ' スクロールイベント
    '***************************************************************************
    ''' <summary>
    ''' 縦スクロール
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub vsbImg_ValueChanged(ByVal sender As Object, ByVal e As Boolean)
        If e = True Then Return ' プログラム中で変更した場合は、無処理

        Dim val As Integer = -1 * vsbImg.Value

        For Each uc As ucDefectImage In m_ucDefectImg
            If uc.ImgDsp Is Nothing Then
                Continue For
            End If
            With uc.ImgDsp
                If .tProp_AriaInf_OffsetY = val Then Exit Sub
                .tProp_AriaInf_OffsetY = val
                .Calculation()
                .Refresh()
            End With
        Next

    End Sub

    ''' <summary>
    ''' 横スクロール
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub hsbImg_ValueChanged(ByVal sender As Object, ByVal e As Boolean)
        If e = True Then Return ' プログラム中で変更した場合は、無処理

        Dim val As Integer = -1 * hsbImg.Value

        For Each uc As ucDefectImage In m_ucDefectImg
            If uc.ImgDsp Is Nothing Then
                Continue For
            End If
            With uc.ImgDsp
                If .tProp_AriaInf_OffsetX = val Then Exit Sub
                .tProp_AriaInf_OffsetX = val
                .Calculation()
                .Refresh()
            End With
        Next

    End Sub

    ''' <summary>
    ''' スクロールバー 最大サイズ変更
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetCommonScrollBar()

        For Each uc As ucDefectImage In m_ucDefectImg
            If uc.ImgDsp Is Nothing Then
                Continue For
            End If
            With uc.ImgDsp
                ' 縦スクロールバー 有効化
                If 0 >= .tProp_CalcAns.ImgNonAriaSize.Height Then
                    vsbImg.Enabled = False
                    vsbImg.Maximum = 0
                Else
                    vsbImg.Value = 0
                    vsbImg.Maximum = .tProp_CalcAns.ImgDspRect.Height - 1
                    vsbImg.Minimum = 0
                    vsbImg.LargeChange = .tProp_CalcAns.ImgDspRect.Height - .tProp_CalcAns.ImgNonAriaSize.Height
                    vsbImg.Enabled = True
                End If

                ' 横スクロールバー 有効化
                If 0 >= .tProp_CalcAns.ImgNonAriaSize.Width Then
                    hsbImg.Enabled = False
                    hsbImg.Maximum = 0
                Else
                    hsbImg.Value = 0
                    hsbImg.Maximum = .tProp_CalcAns.ImgDspRect.Width - 1
                    hsbImg.Minimum = 0
                    hsbImg.LargeChange = .tProp_CalcAns.ImgDspRect.Width - .tProp_CalcAns.ImgNonAriaSize.Width
                    hsbImg.Enabled = True
                End If
                Exit For
            End With
        Next

    End Sub

    ''' <summary>
    ''' 縦スクロール値設定
    ''' </summary>
    ''' <param name="value">スクロール値</param>
    ''' <remarks></remarks>
    Private Sub SetVerticalScroll(ByVal value As Integer)
        Try
            For Each uc As ucDefectImage In m_ucDefectImg
                If uc.ImgDsp Is Nothing Then
                    Continue For
                End If
                With uc.ImgDsp
                    .tProp_AriaInf_OffsetY = -1 * value
                End With
            Next
            vsbImg.ValueProgramable = value

        Catch ex As Exception
            Log(LNO.ERR, String.Format("拡大画像 縦スクロール値設定異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 横スクロール値設定
    ''' </summary>
    ''' <param name="value">スクロール値</param>
    ''' <remarks></remarks>
    Private Sub SetHorizontalScroll(ByVal value As Integer)
        Try
            For Each uc As ucDefectImage In m_ucDefectImg
                If uc.ImgDsp Is Nothing Then
                    Continue For
                End If
                With uc.ImgDsp
                    .tProp_AriaInf_OffsetX = -1 * value
                End With
            Next
            hsbImg.ValueProgramable = value
        Catch ex As Exception
            Log(LNO.ERR, String.Format("拡大画像 横スクロール値設定異常[{0}]", ex.Message))
        End Try
    End Sub

    '***************************************************************************
    ' 倍率コンボボックスイベント
    '***************************************************************************
    ''' <summary>
    ''' 倍率コンボボックスイニシャル
    ''' </summary>
    Private Sub cmbPicRate_Init()
        Dim nSelIndex As Integer = 0

        Dim picTable As New DataTable()
        picTable.Columns.Add("NAME", GetType(String))
        picTable.Columns.Add("VALUE", GetType(CMB_RATE_VALUE))

        For emId As EM_RATE_LIST = EM_RATE_LIST.emRate_XY To EM_RATE_LIST.RATE_END
            Dim strName As String = ""
            Dim dValue As Double = 0
            Select Case emId
                'Case EM_RATE_LIST.emRate_XY
                '    strName = "全体"
                '    dValue = -1
                Case EM_RATE_LIST.emRate_X
                    strName = "全幅"
                    dValue = -1
                Case EM_RATE_LIST.emRate_Y
                    strName = "全長"
                    dValue = -1
                Case EM_RATE_LIST.emRate_025
                    strName = "1/4倍"
                    dValue = 0.25
                Case EM_RATE_LIST.emRate_050
                    strName = "1/2倍"
                    dValue = 0.5
                Case EM_RATE_LIST.emRate_100
                    strName = "１倍"
                    dValue = 1
                    nSelIndex = picTable.Rows.Count     ' デフォルトで指定する倍率
                'Case EM_RATE_LIST.emRate_150
                '    strName = "1.5倍"
                '    dValue = 1.5
                Case EM_RATE_LIST.emRate_200
                    strName = "２倍"
                    dValue = 2
                'Case EM_RATE_LIST.emRate_300
                '    strName = "３倍"
                '    dValue = 3
                Case EM_RATE_LIST.emRate_400
                    strName = "４倍"
                    dValue = 4
                Case Else
                    Continue For
            End Select
            Dim typ As New CMB_RATE_VALUE
            typ.emId = emId
            typ.dValue = dValue
            Dim row As DataRow = picTable.NewRow()
            row("NAME") = strName
            row("VALUE") = typ
            picTable.Rows.Add(row)
        Next

        picTable.AcceptChanges()
        cmbPicRate.Items.Clear()
        cmbPicRate.DataSource = picTable
        cmbPicRate.DisplayMember = "NAME"
        cmbPicRate.ValueMember = "VALUE"
        cmbPicRate.SelectedIndex = nSelIndex
        AddHandler cmbPicRate.SelectedIndexChanged, AddressOf cmbPicRate_SelectedIndexChanged
    End Sub


    ''' <summary>
    ''' 表示倍率コンボボックス
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbPicRate_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)

        ' 座標データ計算
        For Each uc As ucDefectImage In m_ucDefectImg
            If uc.ImgDsp Is Nothing Then
                Continue For
            End If
            Rate_Change(uc.ImgDsp)
            uc.ImgDsp.Calculation()
        Next

        ' スクロールバー反映
        SetCommonScrollBar()

        ' 再描画
        DspUpDate(False)

    End Sub

    '***************************************************************************
    ' キーイベント
    '***************************************************************************
    ''' <summary>
    ''' ファンクションボタン
    ''' </summary>
    ''' <param name="FncNo">ファンクションボタンNo 0オリジン</param>
    ''' <param name="onKey"></param>
    Private Sub frm_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        Try
            Select Case FncNo                                                   ' ファンクションボタン (0オリジン)
                '---------------------------------------------------------------
                Case 12 - 1                                                     ' 終了
                    cmdEnd.PerformClick()
            End Select
        Catch ex As Exception
            Log(LNO.ERR, String.Format("拡大画像 ファンクションイベント異常 [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' フォームを隠す
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub CloseFrom()
        For Each uc As ucDefectImage In m_ucDefectImg
            If uc.ImgDsp Is Nothing Then
                Continue For
            End If
            With uc.ImgDsp
                .tProp_Paint_IsPaint = False
            End With
        Next
        SavePosition()
        m_nDispImgNo = 0
        cbDetail.Checked = False        '前回開くときは詳細は閉じた状態
        ' 終了
        Me.Visible = False
    End Sub

    ''' <summary>
    ''' 閉じるボタン押下
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmdEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmdEnd.Click
        CloseFrom()
    End Sub

    ''' <summary>
    ''' 疵画像を削除します
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DisposeImage()
        Try
            For Each uc As ucDefectImage In m_ucDefectImg
                If uc.ImgDsp Is Nothing Then
                    Continue For
                End If
                With uc.ImgDsp
                    Dim old_img As Image = .tProp_Paint_Img(0)
                    If old_img Is Nothing = False Then              ' 画像を捨てる
                        old_img.Dispose()
                    End If
                End With
            Next
        Catch ex As Exception
            Log(LNO.ERR, String.Format("拡大画像 画像破棄異常 [{0}]", ex.Message))
        End Try
    End Sub

    '***************************************************************************
    ' マウスイベント
    '***************************************************************************
    ''' <summary>
    ''' ピクチャーボックスのマウスホイールイベント
    ''' </summary>
    ''' <param name="sender">イベント発生元</param>
    ''' <param name="e">イベントパラメータ</param>
    ''' <remarks></remarks>
    Public Sub picImage_MouseWheel(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)
        Try
            If vsbImg.Enabled Then
                Dim nValue As Integer = CInt(IIf(e.Delta > 0, 20, -20))             '変化量を計算
                nValue = vsbImg.Value - nValue
                If nValue < 0 Then
                    nValue = 0
                End If
                If nValue > vsbImg.Maximum - vsbImg.LargeChange Then
                    nValue = vsbImg.Maximum - vsbImg.LargeChange
                End If

                SetVerticalScroll(nValue)
                For Each uc As ucDefectImage In m_ucDefectImg
                    uc.ImgDsp.Refresh()
                Next
            End If
        Catch ex As Exception
            Log(LNO.ERR, String.Format("拡大画像 マウスホイールイベント異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' ピクチャーボックスのマウス移動イベント
    ''' </summary>
    ''' <param name="sender">イベント発生元</param>
    ''' <param name="e">イベントパラメータ</param>
    ''' <remarks></remarks>
    Public Sub picImage_MouseMove(ByVal sender As System.Object, ByVal e As System.Windows.Forms.MouseEventArgs)
        Try
            Dim nValue As Integer
            If e.Button = Windows.Forms.MouseButtons.Left Then                  '左ボタンドラッグ時
                If hsbImg.Enabled Then
                    Dim nX As Integer = e.X - m_nClickPos.X                          'X軸方向のスクロール量を計算
                    nValue = hsbImg.Value - nX
                    If 0 > nValue Then                                     'スクロールすると左端にぶつかる
                        nValue = 0
                    ElseIf nValue > hsbImg.Maximum - hsbImg.LargeChange Then                'スクロールすると右端にぶつかる
                        nValue = hsbImg.Maximum - hsbImg.LargeChange
                    End If
                    SetHorizontalScroll(nValue)
                End If
                If vsbImg.Enabled Then
                    Dim nY As Integer = e.Y - m_nClickPos.Y                          'Y軸方向のスクロール量を計算
                    nValue = vsbImg.Value - nY
                    If 0 > nValue Then                                     'スクロールすると左端にぶつかる
                        nValue = 0
                    ElseIf nValue > vsbImg.Maximum - vsbImg.LargeChange Then                'スクロールすると右端にぶつかる
                        nValue = vsbImg.Maximum - vsbImg.LargeChange
                    End If
                    SetVerticalScroll(nValue)
                End If

                m_nClickPos.X = e.X
                m_nClickPos.Y = e.Y

                For Each uc As ucDefectImage In m_ucDefectImg
                    uc.ImgDsp.Refresh()
                Next
            End If

        Catch ex As Exception
            Log(LNO.ERR, String.Format("拡大画像 マウス移動イベント異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' ピクチャーボックスのマウスクリックイベント
    ''' </summary>
    ''' <param name="sender">イベント発生元</param>
    ''' <param name="e">イベントパラメータ</param>
    ''' <remarks></remarks>
    Private Sub picImage_MouseDown(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)
        Try
            If e.Button = Windows.Forms.MouseButtons.Left Then                  '左ボタン押下場所を記憶する
                m_nClickPos.X = e.X
                m_nClickPos.Y = e.Y
            End If
            CType(sender, PictureBox).Focus()
        Catch ex As Exception
            Log(LNO.ERR, String.Format("拡大画像 マウスクリックイベント異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' ピクチャーボックスのダブルクリックイベント
    ''' </summary>
    ''' <param name="sender">イベント発生元</param>
    ''' <param name="e">イベントパラメータ</param>
    ''' <remarks></remarks>
    Public Sub picImage_MouseDoubleClick(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)
        Try
            If e.Button = Windows.Forms.MouseButtons.Left Then
                CloseFrom()
            End If
        Catch ex As Exception
            Log(LNO.ERR, String.Format("拡大画像 ダブルクリックイベント異常[{0}]", ex.Message))
        End Try
    End Sub


    '***************************************************************************
    ' 表示位置 ＆ 表示サイズ
    '***************************************************************************
    ''' <summary>
    ''' 表示位置 ＆ 表示サイズ保存
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub SavePosition()
        tMod_IniFile.WriteIni(m_strSection, "IMAGE_POS_TOP", LOCALINI_NAME, CStr(Me.Top))
        tMod_IniFile.WriteIni(m_strSection, "IMAGE_POS_LEFT", LOCALINI_NAME, CStr(Me.Left))
        tMod_IniFile.WriteIni(m_strSection, "IMAGE_WINDOW_HEIGHT", LOCALINI_NAME, CStr(Me.Height))
        Dim nWidth As Integer = Me.Width
        If cbDetail.Checked Then
            nWidth = nWidth - dgvKizuInfo.Width
        End If
        tMod_IniFile.WriteIni(m_strSection, "IMAGE_WINDOW_WIDTH", LOCALINI_NAME, CStr(nWidth))
        tMod_IniFile.WriteIni(m_strSection, "IMAGE_DISP_RATE", LOCALINI_NAME, CStr(cmbPicRate.SelectedIndex))

    End Sub

    ''' <summary>
    ''' 表示位置 ＆ 表示サイズ読み込み
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ReadPosition()
        Dim nDispTop As Integer = tMod_IniFile.ReadIniInt(m_strSection, "IMAGE_POS_TOP", LOCALINI_NAME, -9999)
        If nDispTop >= Owner.Top And nDispTop <= Owner.Bottom Then
            Me.DispTop = nDispTop
        Else
            Me.DispTop = -9999
        End If
        Dim nDispLeft As Integer = tMod_IniFile.ReadIniInt(m_strSection, "IMAGE_POS_LEFT", LOCALINI_NAME, -9999)
        If nDispLeft >= Owner.Left And nDispLeft <= Owner.Right Then
            Me.DispLeft = nDispLeft
        Else
            Me.DispLeft = -9999
        End If
        Dim nSelIndex As Integer = tMod_IniFile.ReadIniInt(m_strSection, "IMAGE_DISP_RATE", LOCALINI_NAME, -1)
        If 0 <= nSelIndex Then
            cmbPicRate.SelectedIndex = nSelIndex
        End If

        Me.DispHeight = tMod_IniFile.ReadIniInt(m_strSection, "IMAGE_WINDOW_HEIGHT", LOCALINI_NAME, 640)
        Dim nWidth As Integer = tMod_IniFile.ReadIniInt(m_strSection, "IMAGE_WINDOW_WIDTH", LOCALINI_NAME, 720)

        If cbDetail.Checked Then
            Me.DispWidth = nWidth + dgvKizuInfo.Width
        Else
            Me.DispWidth = nWidth
        End If


    End Sub

    ''' <summary>
    ''' 表示位置＆サイズ設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetLocationAndSize()
        '表示サイズ
        If m_nDispWidth = -9999 Or m_nDispHeight = -9999 Then
            '元サイズ
        Else
            Me.Size = New Size(m_nDispWidth, m_nDispHeight)
        End If
        '表示位置
        If m_nDispTop = -9999 Or m_nDispLeft = -9999 Then
            Me.Location = New Point(
                Me.Owner.Location.X + (Me.Owner.Width - Me.Width) \ 2,
                Me.Owner.Location.Y + (Me.Owner.Height - Me.Height) \ 2)
        Else
            Me.Location = New Point(m_nDispLeft, m_nDispTop)
        End If

    End Sub

    Private Sub Rate_Change(ByRef dsp As tClass_ImageDsp2)

        Dim emStrechMode As tClass_ImageDsp_Calc.EM_IMG_STRECH
        Dim dRate As Double
        Dim typ As CMB_RATE_VALUE = CType(cmbPicRate.SelectedValue, CMB_RATE_VALUE)

        With dsp
            Select Case typ.emId
                Case EM_RATE_LIST.emRate_XY
                    Dim d_sz_h As Double = m_DefectData.h_size * m_DefectData.h_res
                    Dim d_sz_w As Double = m_DefectData.w_size * m_DefectData.w_res
                    Dim n_area_height As Integer = .tProp_AriaInf.nAriaHeight
                    Dim n_area_width As Integer = .tProp_AriaInf.nAriaWidth
                    ' 画像サイズの縦横比率とクライアントサイズの縦横比率の比較
                    If d_sz_h / d_sz_w > n_area_height / n_area_width Then
                        emStrechMode = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechHeight    ' 縦横比保持のみ
                    Else
                        emStrechMode = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechWidth     ' 縦横比保持のみ
                    End If
                    dRate = 1.0
                Case EM_RATE_LIST.emRate_X
                    dRate = 1.0
                    emStrechMode = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechWidth         ' 縦横比保持のみ
                Case EM_RATE_LIST.emRate_Y
                    dRate = 1.0
                    emStrechMode = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechHeight        ' 縦横比保持のみ
                Case Else
                    dRate = typ.dValue * (m_nZoomRate / 100.0)  '倍率
                    emStrechMode = tClass_ImageDsp_Calc.EM_IMG_STRECH.emRawWidth
            End Select

            '' オフセット値初期化
            .tProp_ImgKind_StrechType = emStrechMode
            .tProp_AriaInf_OffsetX = 0
            .tProp_AriaInf_OffsetY = 0
            .tProp_AriaInf_Zoom = New tClass.tClass_ImageDsp_Calc.RATE(dRate, dRate)
        End With


    End Sub

    ''' <summary>
    ''' 疵情報欄表示設定
    ''' </summary>
    ''' <param name="dgv">疵情報データグリッド</param>
    ''' <remarks></remarks>
    Private Sub InitdgvDefInf(ByVal dgv As tCnt.tCnt_DataGridView)
        '''' 基本を設定
        dgv.dgvInitialize(dgv)
        dgv.DataGridViewSelectionCancel(dgv)                           ' 選択色無し

        With dgv
            .RowCount = EM_ROW_DEFECTINF.MAX_ROW

            '' 動作定義
            .ScrollBars = ScrollBars.Vertical
            .DoubleBuffered = True

            .ReadOnly = True                                                ' 読み取り専用
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' セル書き換え
            .ColumnHeadersVisible = False                                   ' 列ヘッダ表示なし

            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '選択色
            .DefaultCellStyle().SelectionForeColor = .DefaultCellStyle().ForeColor      '選択色
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

            .ColumnCount = 2                ' マップ列数
            '''' 列定義
            '1列目:項目名
            .Columns(0).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(0).Width = 58
            .Columns(0).SortMode = DataGridViewColumnSortMode.NotSortable
            '2列目:値
            .Columns(1).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(1).Width = .ClientSize.Width - .Columns(0).Width - 3
            .Columns(1).SortMode = DataGridViewColumnSortMode.NotSortable

            '行定義　高さ
            For ii As Integer = 0 To .Rows.Count - 1
                .Rows(ii).Height = 20   '行の高さセット
                .Item(0, ii).Style.BackColor = Color.LightGray
                Select Case ii
                    Case EM_ROW_DEFECTINF.TYPE
                        .Item(0, ii).Value = "疵種名"
                    Case EM_ROW_DEFECTINF.GRADE
                        .Item(0, ii).Value = "グレード"
					Case EM_ROW_DEFECTINF.FPOS
						'// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
						'.Item(0, ii).Value = "F～"
						.Item(0, ii).Value = "出側"
						'// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
					Case EM_ROW_DEFECTINF.DS
                        .Item(0, ii).Value = "DS～"
                    Case EM_ROW_DEFECTINF.WS
                        .Item(0, ii).Value = "WS～"
                    Case EM_ROW_DEFECTINF.LENGTH
                        .Item(0, ii).Value = "欠陥長"
                    Case EM_ROW_DEFECTINF.WIDTH
                        .Item(0, ii).Value = "欠陥幅"
                    Case EM_ROW_DEFECTINF.NO
                        .Item(0, ii).Value = "疵連番"
                    Case EM_ROW_DEFECTINF.TORB
                        .Item(0, ii).Value = "表裏区分"
                    Case EM_ROW_DEFECTINF.CYCLE_DIV
                        .Item(0, ii).Value = "周期疵"
                    Case EM_ROW_DEFECTINF.DSWS
                        .Item(0, ii).Value = "DSWS"
                    Case EM_ROW_DEFECTINF.CAMSET
                        .Item(0, ii).Value = "ｶﾒﾗｾｯﾄ"
                    Case EM_ROW_DEFECTINF.CDS
                        .Item(0, ii).Value = "C～DS"
                    Case EM_ROW_DEFECTINF.CWS
                        .Item(0, ii).Value = "C～WS"
                    Case EM_ROW_DEFECTINF.SPEED
                        .Item(0, ii).Value = "速度"
                    Case EM_ROW_DEFECTINF.PITCH
                        .Item(0, ii).Value = "ピッチ"
                    Case EM_ROW_DEFECTINF.ROOLKEI
                        .Item(0, ii).Value = "ロール径"
                    Case EM_ROW_DEFECTINF.ARIA
                        .Item(0, ii).Value = "面積"
                    Case EM_ROW_DEFECTINF.H_RES
                        .Item(0, ii).Value = "縦分解能"
                    Case EM_ROW_DEFECTINF.W_RES
                        .Item(0, ii).Value = "横分解能"
                    Case EM_ROW_DEFECTINF.NFRAMENO
                        .Item(0, ii).Value = "FNo"
                    Case EM_ROW_DEFECTINF.FRM_X_MIN
                        .Item(0, ii).Value = "FXmin"
                    Case EM_ROW_DEFECTINF.FRM_X_MAX
                        .Item(0, ii).Value = "FXmax"
                    Case EM_ROW_DEFECTINF.FRM_Y_MIN
                        .Item(0, ii).Value = "FYmin"
                    Case EM_ROW_DEFECTINF.FRM_Y_MAX
                        .Item(0, ii).Value = "FYmax"
                    Case EM_ROW_DEFECTINF.CULL_FRM_X_MIN
                        .Item(0, ii).Value = "CFXmin"
                    Case EM_ROW_DEFECTINF.CULL_FRM_X_MAX
                        .Item(0, ii).Value = "CFXmax"
                    Case EM_ROW_DEFECTINF.CULL_FRM_Y_MIN
                        .Item(0, ii).Value = "CFYmin"
                    Case EM_ROW_DEFECTINF.CULL_FRM_Y_MAX
                        .Item(0, ii).Value = "CFYmax"
                    Case EM_ROW_DEFECTINF.BOX_X_MIN
                        .Item(0, ii).Value = "Xmin"
                    Case EM_ROW_DEFECTINF.BOX_X_MAX
                        .Item(0, ii).Value = "Xmax"
                    Case EM_ROW_DEFECTINF.BOX_Y_MIN
                        .Item(0, ii).Value = "Ymin"
                    Case EM_ROW_DEFECTINF.BOX_Y_MAX
                        .Item(0, ii).Value = "Ymax"
                    Case EM_ROW_DEFECTINF.BASE_BOX_X_MIN
                        .Item(0, ii).Value = "元Xmin"
                    Case EM_ROW_DEFECTINF.BASE_BOX_X_MAX
                        .Item(0, ii).Value = "元Xmax"
                    Case EM_ROW_DEFECTINF.BASE_BOX_Y_MIN
                        .Item(0, ii).Value = "元Ymin"
                    Case EM_ROW_DEFECTINF.BASE_BOX_Y_MAX
                        .Item(0, ii).Value = "元Ymax"
                    Case EM_ROW_DEFECTINF.BASE_CENTER_X
                        .Item(0, ii).Value = "元中心X"
                    Case EM_ROW_DEFECTINF.BASE_CENTER_Y
                        .Item(0, ii).Value = "元中心Y"
                    Case EM_ROW_DEFECTINF.BASE_H_SIZE
                        .Item(0, ii).Value = "元縦画数"
                    Case EM_ROW_DEFECTINF.BASE_W_SIZE
                        .Item(0, ii).Value = "元横画数"
                    Case EM_ROW_DEFECTINF.BASE_H_RES
                        .Item(0, ii).Value = "元縦分解"
                    Case EM_ROW_DEFECTINF.BASE_W_RES
                        .Item(0, ii).Value = "元横分解"
                    Case EM_ROW_DEFECTINF.SHRINK_RATIO
                        .Item(0, ii).Value = "縮小率"
                    Case EM_ROW_DEFECTINF.CENTER_X
                        .Item(0, ii).Value = "疵中心X"
                    Case EM_ROW_DEFECTINF.CENTER_Y
                        .Item(0, ii).Value = "疵中心Y"
                    Case EM_ROW_DEFECTINF.H_SIZE
                        .Item(0, ii).Value = "縦画素数"
                    Case EM_ROW_DEFECTINF.W_SIZE
                        .Item(0, ii).Value = "横画素数"
                    Case EM_ROW_DEFECTINF.TYUU
                        .Item(0, ii).Value = "疵種優先"
                    Case EM_ROW_DEFECTINF.GYUU
                        .Item(0, ii).Value = "Gr優先"
                    Case EM_ROW_DEFECTINF.WIDABSPOS
                        .Item(0, ii).Value = "幅絶対"
                    Case EM_ROW_DEFECTINF.WIDDIV
                        .Item(0, ii).Value = "幅分割"
                    Case EM_ROW_DEFECTINF.NOBJTANSHO
                        .Item(0, ii).Value = "探傷対象"
                    Case EM_ROW_DEFECTINF.OBJDEFLIST
                        .Item(0, ii).Value = "重欠対象"
                    Case EM_ROW_DEFECTINF.MODEDEFLIST
                        .Item(0, ii).Value = "強調対象"
                    Case EM_ROW_DEFECTINF.HASSEIDATE
                        .Item(0, ii).Value = "時刻"
                End Select
            Next ii
        End With


    End Sub

    ''' <summary>
    ''' 検査材情報表示設定
    ''' </summary>
    ''' <param name="dgv">疵一覧データグリッド</param>
    ''' <remarks></remarks>
    Public Sub InitdgvCoilInf2(ByVal dgv As tCnt.tCnt_DataGridView)
        dgv.dgvInitialize(dgv)
        dgv.DataGridViewSelectionCancel(dgv)                           ' 選択色無し

        Try
            With dgv
                .ScrollBars = ScrollBars.Horizontal                             ' スクロールバー(水平のみ)
                .DoubleBuffered = True

                .EditMode = DataGridViewEditMode.EditProgrammatically           ' セル書き換え
                .ReadOnly = True                                                ' 読み取り専用
                .EditMode = DataGridViewEditMode.EditProgrammatically           ' セル書き換え
                .ColumnHeadersVisible = True                                    ' 列ヘッダ表示あり
                .AllowUserToResizeColumns = True                                ' 列のサイズ変更有

                '' 選択セル色定義
                .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '選択色
                .DefaultCellStyle().SelectionForeColor = .DefaultCellStyle().ForeColor      '選択色
                .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                .ColumnHeadersHeight = 16                                       ' 列ヘッダ高さ調整

                For col As EM_COL_DEFECTINF = EM_COL_DEFECTINF.TYPE To EM_COL_DEFECTINF.MAX_COL
                    Select Case col
                        Case EM_COL_DEFECTINF.TYPE
                            ' 疵種名
                            dgvColSetText(dgv, "疵種名", 120, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.MiddleCenter)
                        Case EM_COL_DEFECTINF.GRADE
                            ' グレード
                            dgvColSetText(dgv, "グレード", 70, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.MiddleCenter)
                        Case EM_COL_DEFECTINF.FPOS
                            ' コイル先端からの位置
                            '// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
                            'dgvColSetText(dgv, "F～", 80, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.MiddleCenter)
							dgvColSetText(dgv, "出側", 80, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.MiddleCenter)
							'// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
                    End Select
                Next

                .RowCount = 1
                .Rows(0).Height = .ClientSize.Height - .ColumnHeadersHeight

            End With
        Catch ex As Exception
        End Try


    End Sub

    ''' <summary>
    ''' 疵情報セット
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub SetDgvData()
        Try
            '-------------------------------------------------------------------
            '疵情報を表示DGVにセットする。
            '-------------------------------------------------------------------
            With dgvKizuInfo
                For ii As Integer = 0 To .RowCount - 1
                    Select Case ii
                        Case EM_ROW_DEFECTINF.NO
                            .Item(1, ii).Value = m_DefectData.nKizuNo.ToString
                        Case EM_ROW_DEFECTINF.TYPE
                            .Item(1, ii).Value = mcls_Param.GetTypeName(m_DefectData.nTid)
                        Case EM_ROW_DEFECTINF.GRADE
                            .Item(1, ii).Value = mcls_Param.GetGradeName(m_DefectData.nGid)
                            .Item(1, ii).Style.BackColor = mcls_Param.GetGradeColor(m_DefectData.nGid)
                            .Item(1, ii).Style.SelectionBackColor = .Item(1, ii).Style.BackColor
						Case EM_ROW_DEFECTINF.FPOS
							'// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>							
							'.Item(1, ii).Value = Format(Int(m_DefectData.y / 100) / 10, "####0.0") 'F表示
							.Item(1, ii).Value = Format(Int(m_DefectData.y_out / 100) / 10, "####0.0") 'F表示
							'// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
						Case EM_ROW_DEFECTINF.DS
                            .Item(1, ii).Value = m_DefectData.ds.ToString("0.0")
                        Case EM_ROW_DEFECTINF.WS
                            .Item(1, ii).Value = m_DefectData.ws.ToString("0.0")
                        Case EM_ROW_DEFECTINF.WIDTH
                            .Item(1, ii).Value = m_DefectData.kizu_wid.ToString
                        Case EM_ROW_DEFECTINF.LENGTH
                            .Item(1, ii).Value = m_DefectData.kizu_len.ToString
                        Case EM_ROW_DEFECTINF.TORB
                            .Item(1, ii).Value = m_DefectData.TorB.ToString
                        Case EM_ROW_DEFECTINF.CYCLE_DIV
                            .Item(1, ii).Value = m_DefectData.cycle_div.ToString
                        Case EM_ROW_DEFECTINF.DSWS
                            .Item(1, ii).Value = m_DefectData.dsws.ToString
                        Case EM_ROW_DEFECTINF.CAMSET
                            .Item(1, ii).Value = m_DefectData.camset.ToString
                        Case EM_ROW_DEFECTINF.CDS
                            .Item(1, ii).Value = m_DefectData.dsc.ToString("0.0")
                        Case EM_ROW_DEFECTINF.CWS
                            .Item(1, ii).Value = m_DefectData.wsc.ToString("0.0")
                        Case EM_ROW_DEFECTINF.SPEED
                            .Item(1, ii).Value = m_DefectData.speed.ToString
                        Case EM_ROW_DEFECTINF.PITCH
                            If 1 = m_DefectData.cycle_div Or 2 = m_DefectData.cycle_div Then    ' 周期性疵の場合
                                .Item(1, ii).Value = m_DefectData.pitch.ToString
                            Else
                                .Item(1, ii).Value = ""
                            End If
                        Case EM_ROW_DEFECTINF.ROOLKEI
                            If 1 = m_DefectData.cycle_div Or 2 = m_DefectData.cycle_div Then    ' 周期性疵の場合
                                .Item(1, ii).Value = m_DefectData.roolkei.ToString("####0")
                            Else
                                .Item(1, ii).Value = ""
                            End If
                        Case EM_ROW_DEFECTINF.ARIA
                            .Item(1, ii).Value = m_DefectData.aria.ToString("0.0")
                        Case EM_ROW_DEFECTINF.H_RES
                            .Item(1, ii).Value = m_DefectData.h_res.ToString
                        Case EM_ROW_DEFECTINF.W_RES
                            .Item(1, ii).Value = m_DefectData.w_res.ToString
                        Case EM_ROW_DEFECTINF.NFRAMENO
                            .Item(1, ii).Value = m_DefectData.nFrameNo.ToString
                        Case EM_ROW_DEFECTINF.FRM_X_MIN
                            .Item(1, ii).Value = m_DefectData.frm_x_min.ToString
                        Case EM_ROW_DEFECTINF.FRM_X_MAX
                            .Item(1, ii).Value = m_DefectData.frm_x_max.ToString
                        Case EM_ROW_DEFECTINF.FRM_Y_MIN
                            .Item(1, ii).Value = m_DefectData.frm_y_min.ToString
                        Case EM_ROW_DEFECTINF.FRM_Y_MAX
                            .Item(1, ii).Value = m_DefectData.frm_y_max.ToString
                        Case EM_ROW_DEFECTINF.CULL_FRM_X_MIN
                            .Item(1, ii).Value = m_DefectData.cull_frm_x_min.ToString
                        Case EM_ROW_DEFECTINF.CULL_FRM_X_MAX
                            .Item(1, ii).Value = m_DefectData.cull_frm_x_max.ToString
                        Case EM_ROW_DEFECTINF.CULL_FRM_Y_MIN
                            .Item(1, ii).Value = m_DefectData.cull_frm_y_min.ToString
                        Case EM_ROW_DEFECTINF.CULL_FRM_Y_MAX
                            .Item(1, ii).Value = m_DefectData.cull_frm_y_max.ToString
                        Case EM_ROW_DEFECTINF.BOX_X_MIN
                            .Item(1, ii).Value = m_DefectData.box_x_min.ToString
                        Case EM_ROW_DEFECTINF.BOX_X_MAX
                            .Item(1, ii).Value = m_DefectData.box_x_max.ToString
                        Case EM_ROW_DEFECTINF.BOX_Y_MIN
                            .Item(1, ii).Value = m_DefectData.box_y_min.ToString
                        Case EM_ROW_DEFECTINF.BOX_Y_MAX
                            .Item(1, ii).Value = m_DefectData.box_y_max.ToString
                        Case EM_ROW_DEFECTINF.BASE_BOX_X_MIN
                            .Item(1, ii).Value = m_DefectData.base_box_x_min.ToString
                        Case EM_ROW_DEFECTINF.BASE_BOX_X_MAX
                            .Item(1, ii).Value = m_DefectData.base_box_x_max.ToString
                        Case EM_ROW_DEFECTINF.BASE_BOX_Y_MIN
                            .Item(1, ii).Value = m_DefectData.base_box_y_min.ToString
                        Case EM_ROW_DEFECTINF.BASE_BOX_Y_MAX
                            .Item(1, ii).Value = m_DefectData.base_box_y_max.ToString
                        Case EM_ROW_DEFECTINF.BASE_CENTER_X
                            .Item(1, ii).Value = m_DefectData.base_center_x.ToString
                        Case EM_ROW_DEFECTINF.BASE_CENTER_Y
                            .Item(1, ii).Value = m_DefectData.base_center_y.ToString
                        Case EM_ROW_DEFECTINF.BASE_H_SIZE
                            .Item(1, ii).Value = m_DefectData.base_h_size.ToString
                        Case EM_ROW_DEFECTINF.BASE_W_SIZE
                            .Item(1, ii).Value = m_DefectData.base_w_size.ToString
                        Case EM_ROW_DEFECTINF.BASE_H_RES
                            .Item(1, ii).Value = m_DefectData.base_h_res.ToString
                        Case EM_ROW_DEFECTINF.BASE_W_RES
                            .Item(1, ii).Value = m_DefectData.base_w_res.ToString
                        Case EM_ROW_DEFECTINF.SHRINK_RATIO
                            .Item(1, ii).Value = m_DefectData.nShrinkRatio.ToString
                        Case EM_ROW_DEFECTINF.CENTER_X
                            .Item(1, ii).Value = m_DefectData.center_x.ToString
                        Case EM_ROW_DEFECTINF.CENTER_Y
                            .Item(1, ii).Value = m_DefectData.center_y.ToString
                        Case EM_ROW_DEFECTINF.H_SIZE
                            .Item(1, ii).Value = m_DefectData.h_size.ToString
                        Case EM_ROW_DEFECTINF.W_SIZE
                            .Item(1, ii).Value = m_DefectData.w_size.ToString
                        Case EM_ROW_DEFECTINF.TYUU
                            .Item(1, ii).Value = m_DefectData.nTyuu.ToString
                        Case EM_ROW_DEFECTINF.GYUU
                            .Item(1, ii).Value = m_DefectData.nGyuu.ToString
                        Case EM_ROW_DEFECTINF.WIDABSPOS
                            .Item(1, ii).Value = m_DefectData.nWidAbsPos.ToString
                        Case EM_ROW_DEFECTINF.WIDDIV
                            .Item(1, ii).Value = m_DefectData.nWidDiv.ToString
                        Case EM_ROW_DEFECTINF.NOBJTANSHO
                            .Item(1, ii).Value = m_DefectData.nObjTansho.ToString
                        Case EM_ROW_DEFECTINF.OBJDEFLIST
                            .Item(1, ii).Value = m_DefectData.nObjDeflist.ToString
                        Case EM_ROW_DEFECTINF.MODEDEFLIST
                            .Item(1, ii).Value = m_DefectData.nModeDeflist.ToString
                        Case EM_ROW_DEFECTINF.HASSEIDATE
                            .Item(1, ii).Value = String.Format("{0:0000}/{1:00}/{2:00} {3:00}:{4:00}:{5:00}",
                                                               m_DefectData.time(0), m_DefectData.time(1), m_DefectData.time(2),
                                                               m_DefectData.time(3), m_DefectData.time(4), m_DefectData.time(5))
                            .Item(1, ii).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 7.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                    End Select
                Next
            End With

            With dgvKizuInfo2
                For col As EM_COL_DEFECTINF = EM_COL_DEFECTINF.TYPE To EM_COL_DEFECTINF.MAX_COL
                    Select Case col
                        Case EM_COL_DEFECTINF.TYPE
							.Item(col, 0).Value = mcls_Param.GetTypeName(m_DefectData.nTid)
						Case EM_COL_DEFECTINF.GRADE
                            .Item(col, 0).Value = mcls_Param.GetGradeName(m_DefectData.nGid)
                            .Item(col, 0).Style.BackColor = mcls_Param.GetGradeColor(m_DefectData.nGid)
                            .Item(col, 0).Style.SelectionBackColor = .Item(col, 0).Style.BackColor
                        Case EM_COL_DEFECTINF.FPOS
                            ' 品種区分
                            '// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
                            '.Item(col, 0).Value = Format(Int(m_DefectData.y / 100) / 10, "####0.0") 'F表示
							.Item(col, 0).Value = Format(Int(m_DefectData.y_out / 100) / 10, "####0.0") 'F表示
							'// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
                    End Select
                Next
            End With

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵情報表示異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 詳細情報表示切替
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub cbDetail_CheckedChanged(sender As Object, e As EventArgs)
        Dim cb As CheckBox = CType(sender, CheckBox)
        Dim minSize As Size = Me.MinimumSize


        ' 画面サイズを変える前にパネルの右アンカーを外す
        tlpMain.Anchor = tlpMain.Anchor And (AnchorStyles.Top Or AnchorStyles.Bottom Or AnchorStyles.Left)

        If cb.Checked Then
            cb.BackColor = Color.Cyan
            Me.Width = Me.Width + dgvKizuInfo.Width
            minSize.Width = minSize.Width + dgvKizuInfo.Width
            Me.MinimumSize = minSize
        Else
            minSize.Width = minSize.Width - dgvKizuInfo.Width
            Me.MinimumSize = minSize
            cb.BackColor = SystemColors.Control
            Me.Width = Me.Width - dgvKizuInfo.Width
            tlpMain.Width = Me.ClientSize.Width
        End If


        ' 右アンカーを再セット
        tlpMain.Anchor = tlpMain.Anchor Or AnchorStyles.Right

    End Sub
End Class
