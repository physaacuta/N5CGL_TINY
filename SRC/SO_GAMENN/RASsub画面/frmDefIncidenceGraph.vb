'*******************************************************************************
'　疵個数トレンドグラフ画面
'	[Ver]
'		Ver.01    2019/03/20  初版
'
'	[メモ]
'		疵個数トレンドグラフ画面の表示
'*******************************************************************************
Option Strict On

Imports tClassLibrary

Public Class frmDefIncidenceGraph
    '/ ////////////////////////////////////////////////////////////////////////////////
    '  列挙体
    '/ ////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' コイル情報のデータグリッド列
    ''' </summary>
    Private Enum EM_COL_COLINF
        ORDER_NO = 0        ' 命令No
        COIL_NO             ' 生産番号
        TYPE_DIV            ' 品種
        MATERIAL            ' 材質
        NEXT_STEP           ' 次工程
        KENSA_REF           ' 検査基準
        SIZE                ' 材料厚幅
        ORDER               ' 注文
        MAX_COL             ' 最大列数
    End Enum

    ''' <summary>
    ''' 疵種データグリッド列
    ''' </summary>
    Private Enum EM_KIZU_KIND_DATA_COL
        COLOR = 0                      ' 表示色
        CHECK                          ' 選択
        NAME                           ' 疵名
    End Enum

    ''' <summary>
    ''' 疵個数データグリッド列
    ''' </summary>
    Private Enum EM_KIZU_CNT_DATA_COL
        NAME = 0                       ' 疵名
        CNT                            ' 疵個数（表個数/裏個数）
    End Enum

    ''' <summary>
    ''' 疵個数トレンドグラフ画面：表示選択
    ''' </summary>
    Private Enum EM_KIZU_CNT_TRAND_DSP
        CNT = 0                         ' 疵個数
        RATE                            ' 疵混入率
    End Enum

    ''' <summary>
    ''' 疵個数トレンドグラフ画面：面選択
    ''' </summary>
    Private Enum EM_KIZU_CNT_TRAND_MEN
        FRONT = 0                       ' 表面
        BACK                            ' 裏面
        SUM                             ' 表裏面合算
    End Enum

    '/ ////////////////////////////////////////////////////////////////////////////////
    '  構造体
    '/ ////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 疵個数トレンド：検索条件
    ''' </summary>
    Public Structure TYP_KIZU_TRAND_SEARCH
        Dim nSelCoilIdx As Integer                  ' 選択コイルIdx（コイル情報リスト）
        Dim nOKizuCntMax As Integer                 ' 疵個数最大値（表）
        Dim nUKizuCntMax As Integer                 ' 疵個数最大値（裏）
        Dim nAKizuCntMax As Integer                 ' 疵個数最大値（合計）
        Dim dOKizuRateMax As Single                 ' 疵混入率最大値（表）
        Dim dUKizuRateMax As Single                 ' 疵混入率最大値（裏）
        Dim dAKizuRateMax As Single                 ' 疵混入率最大値（合計）
        Dim strStartDate As String                  ' 検索開始日付
        Dim strStartTime As String                  ' 検索開始時刻
        Dim strEndDate As String                    ' 検索終了日付
        Dim strEndTime As String                    ' 検索終了時刻
    End Structure

    ''' <summary>
    ''' 疵個数トレンド：疵情報
    ''' </summary>
    Public Structure TYP_KIZU_TRAND_KIZUINF
        Dim nEdasysID As Integer                    ' 疵種
        Dim nOKizuCnt As Integer                    ' 表疵件数
        Dim nUKizuCnt As Integer                    ' 裏疵件数
        Dim nAKizuCnt As Integer                    ' 合計疵件数
        Dim dOKizuRate As Single                    ' 表疵件混入率
        Dim dUKizuRate As Single                    ' 裏疵件混入率
        Dim dAKizuRate As Single                    ' 合計疵混入率
    End Structure

    ''' <summary>
    ''' 疵個数トレンド：コイル情報
    ''' </summary>
    Public Structure TYP_KIZU_TRAND_COILINF
        Dim strKizuKanNo As String                  ' 管理No
        Dim strOrderNo As String                    ' 命令No
        Dim strCoilNo As String                     ' 生産No
        Dim strHinCode As String                    ' 品種
        Dim strSyuZai As String                     ' 主材質
        Dim strNextCode As String                   ' 計画次工程
        Dim strKenKijun As String                   ' 検査基準（表）
        Dim nCoilAtu As Integer                     ' 材料板厚
        Dim nCoilWidth As Integer                   ' 材料板幅
        Dim nCoilAtuSei As Integer                  ' 注文板厚
        Dim nCoilWidSei As Integer                  ' 注文板幅
        Dim nOKizuCnt As Integer                    ' 表疵件数
        Dim nUKizuCnt As Integer                    ' 裏疵件数
        Dim nAKizuCnt As Integer                    ' 合計疵件数
        Dim lstKizuInf As List(Of TYP_KIZU_TRAND_KIZUINF) ' 疵情報

        Public Sub initialize()
            lstKizuInf = New List(Of TYP_KIZU_TRAND_KIZUINF)
        End Sub
    End Structure

    ''' <summary>
    ''' 疵個数トレンド情報
    ''' </summary>
    Public Structure TYP_KIZU_TRAND_INF
        Dim typSearch As TYP_KIZU_TRAND_SEARCH              ' 検索画面の検索条件
        Dim lstCoil As List(Of TYP_KIZU_TRAND_COILINF)      ' コイル情報リスト（グラフ表示対象のコイル情報）
        Dim lstAllKizu As List(Of Integer)                  ' 疵種リスト（疵種表示フィルターに表示する疵種）

        Public Sub initialize()
            lstCoil = New List(Of TYP_KIZU_TRAND_COILINF)
            lstAllKizu = New List(Of Integer)
        End Sub
    End Structure

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const INTERVAL_X_PIC_MAX As Double = 62                     ' グラフX軸の間隔最大ピクセル（グラフ上で生産番号が重ならずに表示できる限界値）
    Private Const INTERVAL_X_INIT As Integer = 1                        ' グラフX軸の表示間隔初期値
    Private Const INTERVAL_X_NEXT As Integer = 5                        ' グラフX軸の表示間隔
    Private Const DEFAULT_Y_MAX As Integer = 100                        ' グラフY軸のデフォルト最大値（デフォルト最大値を超えると表示間隔を変更する）
    Private Const INTERVAL_Y_INIT As Integer = 1                        ' グラフY軸の表示間隔初期値
    Private Const INTERVAL_Y_NEXT As Integer = 10                       ' グラフY軸の表示間隔
    Private Const GRAPH_PADDING_LEFT As Integer = 40                    ' グラフ左パディング
    Private Const GRAPH_PADDING_TOP As Integer = 10                     ' グラフ上パディング
    Private Const GRAPH_PADDING_RIGHT As Integer = 33                   ' グラフ右パディング
    Private Const GRAPH_PADDING_BOT As Integer = 40                     ' グラフ下パディング

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ デリゲート
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Delegate Function GetKizuCntRate(ByVal nEdasysID As Integer, ByVal lstKizu As List(Of TYP_KIZU_TRAND_KIZUINF), ByVal nMen As EM_KIZU_CNT_TRAND_MEN) As Single
    Private m_KizuDspMethod As GetKizuCntRate                           ' 疵個数 or 疵混入率を取得する関数

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバ 変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_Param As clsParamManager                               ' パラメータ管理
    Private mcls_Log As tClass.tClass_LogManager                        ' ログ管理
    Private mcls_pMap As New tClass.tClass_MapBase                      ' マップ描画基底クラス 
    Private mfrm_DefTrand As New frmDefTrand                            ' 疵個数トレンドグラフ検索画面
    Private m_typProtData As TYP_KIZU_TRAND_INF                         ' 疵個数トレンドグラフプロットデータ（検索画面から取得）
    Private m_strDspFormat As String                                    ' 疵個数/混入率表示フォーマット


    '/ /////////////////////////////////////////////////////////////////////////////////
    ' メンバ関数
    '/ /////////////////////////////////////////////////////////////////////////////////
#Region "コンストラクタ"
    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <param name="log">ログ管理</param>
    ''' <remarks></remarks>
    Public Sub New(ByVal log As tClass.tClass_LogManager)
        Try
            ' この呼び出しは、Windows フォーム デザイナで必要です。
            InitializeComponent()

            ' InitializeComponent() 呼び出しの後で初期化を追加します。

            Dim s As Size
            s.Width = WIN_WIDTH_MAX
            s.Height = WIN_HEIGHT_MAX
            Me.MaximumSize = s
            s.Width = WIN_WIDTH_MIN
            s.Height = WIN_HEIGHT_MIN
            Me.MinimumSize = s

            ' 検索画面を自フォームの子供に追加
            'Me.AddOwnedForm(mfrm_DefTrand)

            mfrm_DefTrand = New frmDefTrand                 ' 疵個数トレンドグラフ検索画面
            m_typProtData = mfrm_DefTrand.KizuTrandInf      ' 疵個数トレンドグラフプロットデータ
            mcls_Log = log                                  ' ログ管理  
            mcls_Param = New clsParamManager(mcls_Log)

        Catch ex As Exception
            log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("疵マップ画面 作成異常 [{0}]", ex.Message))
        End Try
    End Sub

#End Region

#Region "フォーム表示/終了イベント関連"
    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmDefIncidenceGraph_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        Dim s As System.Windows.Forms.Screen = System.Windows.Forms.Screen.FromControl(Me.Owner)
        Me.Location = s.Bounds.Location

        ' タイトル設定
        SetTitleName(Me)

        ' ポップアップ画面 低解像度対応
        SetPcHeight(Me)
        SetPcWidth(Me)

        If NOA_ONLINE <> g_NOA Then                                         ' デバッグ時
            ' フォーム状態
            Me.tProp_FromFormatMode = gem_FromFormatMode.iOFFLINE
            Me.tProp_EndButtonEnable = False
            Me.FormBorderStyle = Windows.Forms.FormBorderStyle.Sizable      ' フォームサイズ変更可能
        End If

        While (Not mcls_Param.ReadAllParam())                               ' パラメータ読み込めるまで、粘る
            System.Threading.Thread.Sleep(1000)
        End While

        ' 画面項目初期化
        SetInit()

        ' 疵種表示フィルターに全疵種表示
        m_typProtData.initialize()
        SetAllKizuKind()

    End Sub

    ''' <summary>
    ''' フォーム表示
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmDefIncidenceGraph_Shown(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Shown
        Try
            '''' 画面項目値設定
            SetDsp()

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("フォーム表示 例外[{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' フォーム終了処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmDefIncidenceGraph_Closing(ByVal sender As Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles MyBase.Closing

        'iniファイル情報書込
        PutKizuCntTrendDsp()

    End Sub

    ''' <summary>
    ''' この画面の資源解放イベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmDefIncidenceGraph_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
        Try
            '-------------------------------------------------------------------
            ' 資源解放
            '-------------------------------------------------------------------
            m_KizuDspMethod = Nothing

            If Not mfrm_DefTrand Is Nothing Then
                mfrm_DefTrand.Close()
                mfrm_DefTrand.Dispose()
                mfrm_DefTrand = Nothing
            End If

            If Not mcls_pMap Is Nothing Then
                mcls_pMap.Dispose()
                mcls_pMap = Nothing
            End If

            If Not mcls_Param Is Nothing Then
                mcls_Param.Dispose()
                mcls_Param = Nothing
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("疵個数トレンドグラフ画面 資源解放異常 [{0}]", ex.Message))
        End Try
    End Sub

#End Region

#Region "初期化処理"
    ''' <summary>
    ''' 画面初期設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetInit()

        ' コイル情報初期化
        CoilInit()

        ' 疵種表示フィルター初期化
        KizuKindInit()

        ' 疵個数/混入率初期化
        KizuCntRateInit()

        ' マップ初期化
        MapInit()

        ' 日付初期化
        lblDate.Text = ""

        ' イベント設定
        SetItemEvent(True)
    End Sub

    ''' <summary>
    ''' マップ基本情報初期化
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub MapInit()
        '''' グラフ描画に必要な情報を設定
        With mcls_pMap
            '' 表示領域情報
            .tProp_AriaInf_BackGroundLineColor = Pens.Black
            .tProp_AriaInf_BackGroundColor = New SolidBrush(Color.White)
            .tProp_AriaInf_MapColor = New SolidBrush(Color.White)
            .tProp_AriaInf_AriaSize = picGraph.Size
            .tProp_AriaInf_Margin = New Padding(GRAPH_PADDING_LEFT, GRAPH_PADDING_TOP, GRAPH_PADDING_RIGHT, GRAPH_PADDING_BOT)

            '' マップ領域
            .tProp_MapInf_IsCenterX = False
            .tProp_MapInf_IsCenterY = False
            .tProp_MapInf_IsBaseX_L = True
            .tProp_MapInf_IsBaseY_T = False
            .tProp_MapInf_IsStrechX = True
            .tProp_MapInf_IsStrechY = True
            .tProp_MapInf_LimitXmin = 1
            .tProp_MapInf_LimitXmax = 1000
            .tProp_MapInf_LimitYmin = 0
            .tProp_MapInf_LimitYmax = 100
            .tProp_MapInf_Offset = New tClass.tClass_MapBase.xyValue(0, 0)

            '' マップ対象外領域設定
            .tProp_LimitInf_LimitDraw = False

            '' メモリ設定
            Dim p As Pen = New Pen(Color.LightBlue, 1)
            p.DashStyle = Drawing2D.DashStyle.DashDot
            .tProp_MemoriInf_LinePen = p
            .tProp_MemoriInf_StringFont = New Font("ＭＳ ゴシック", 11, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
            .tProp_MemoriInf_StringBsh = New SolidBrush(Color.Black)
            .tProp_MemoriInf_PosStringX = 10
            .tProp_MemoriInf_PosStringY = 1
            .tProp_MemoriInf_ScaleInterval = New tClass.tClass_MapBase.xyValue(INTERVAL_X_INIT, INTERVAL_Y_INIT)
            .tProp_MemoriInf_ScaleStringFormatX = "0本前"
            .tProp_MemoriInf_ScaleStringFormatY = "0"
            .tProp_MemoriInf_ScaleStringRange = New tClass.tClass_MapBase.RATE(1, 1)
            .tProp_MemoriInf_StringDisp_T = False
            .tProp_MemoriInf_StringDisp_B = True
            .tProp_MemoriInf_StringDisp_L = True
            .tProp_MemoriInf_StringDisp_R = False
            .tProp_MemoriInf_LineDsp_X = True
            .tProp_MemoriInf_LineDsp_Y = True
        End With
    End Sub

    ''' <summary>
    ''' コイル情報初期化
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub CoilInit()

        Dim dgv As tCnt.tCnt_DataGridView = dgvCoil

        dgv.dgvInitialize(dgv)
        dgv.DataGridViewSelectionCancel(dgv)                                ' 選択色無し

        With dgv
            .ScrollBars = ScrollBars.Horizontal                             ' スクロールバー(水平のみ)
            .DoubleBuffered = True

            .ReadOnly = True                                                ' 読み取り専用
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' セル書き換え
            .ColumnHeadersVisible = True                                    ' 列ヘッダ表示あり

            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '選択色
            .DefaultCellStyle().SelectionForeColor = .DefaultCellStyle().ForeColor      '選択色
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 20.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .DefaultCellStyle.WrapMode = DataGridViewTriState.True
            .ColumnHeadersHeight = 20                                       ' 列ヘッダ高さ調整

            For col As EM_COL_COLINF = EM_COL_COLINF.ORDER_NO To EM_COL_COLINF.MAX_COL
                Select Case col
                    Case EM_COL_COLINF.ORDER_NO
                        ' 命令No
                        dgvColSetText(dgv, "命令No", 70, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                    Case EM_COL_COLINF.COIL_NO
                        ' 生産番号
                        dgvColSetText(dgv, "生産番号", 167, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                    Case EM_COL_COLINF.TYPE_DIV
                        ' 品種
                        dgvColSetText(dgv, "品種", 38, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                    Case EM_COL_COLINF.MATERIAL
                        ' 材質
                        dgvColSetText(dgv, "材質", 38, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                    Case EM_COL_COLINF.NEXT_STEP
                        ' 次工程
                        dgvColSetText(dgv, "次工程", 70, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                    Case EM_COL_COLINF.KENSA_REF
                        ' 検査基準
                        dgvColSetText(dgv, "検査基準", 70, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                    Case EM_COL_COLINF.SIZE
                        ' 原板
                        dgvColSetText(dgv, "原板", 186, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                    Case EM_COL_COLINF.ORDER
                        ' 注文
                        dgvColSetText(dgv, "注文", 186, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

                End Select
            Next

            .RowCount = 1
            .Rows(0).Height = .ClientSize.Height - .ColumnHeadersHeight

        End With

        ' XX本目を非表示
        pnlCoilCnt.Visible = False

    End Sub

    ''' <summary>
    ''' 疵種表示フィルター初期化
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub KizuKindInit()

        Dim dgv As tCnt.tCnt_DataGridView = dgvKizukind

        dgv.dgvInitialize(dgv)
        dgv.DataGridViewSelectionCancel(dgv)                                ' 選択色無し

        With dgv
            .ColumnCount = 0
            .AllowUserToResizeColumns = False                               ' ユーザー 列サイズ変更
            .ScrollBars = ScrollBars.Vertical                               ' スクロールバーの表示（垂直のみ）
            .ColumnHeadersHeight = 42                                       ' ヘッダーの高さ

            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' セル選択
            .ReadOnly = False                                               ' 読み取り専用ではない
            .EditMode = DataGridViewEditMode.EditOnEnter                    ' セル書き換え

            .ColumnHeadersVisible = False                                   ' 列ヘッダ非表示

            ' 列ヘッダー 文字サイズ変更,文字色変更
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            ' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor

            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1行おきに色替え
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            ' 列定義
            ' 1列目
            Using clm As New DataGridViewButtonColumn
                clm.HeaderText = "表示色"
                clm.Width = 45
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                clm.DefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                .Columns.Add(clm)
            End Using

            ' 2列目
            Using clm As New DataGridViewCheckBoxColumn
                clm.HeaderText = "有効無効" & vbNewLine & "(ﾁｪｯｸ：有効)"

                clm.Width = 25
                clm.Frozen = False
                clm.ReadOnly = False
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter

                .Columns.Add(clm)
            End Using

            ' 3列目
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "名称"
                clm.Width = .Width - 70 - 3
                clm.Frozen = False
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                .Columns.Add(clm)
            End Using

        End With

    End Sub

    ''' <summary>
    ''' 疵個数/疵混入率初期化
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub KizuCntRateInit()

        Dim dgv As tCnt.tCnt_DataGridView = dgvKizuCntRate

        dgv.dgvInitialize(dgv)
        dgv.DataGridViewSelectionCancel(dgv)   ' 選択色無し

        With dgv
            '' 定義
            .SelectionMode = DataGridViewSelectionMode.FullRowSelect        ' 行選択
            .ScrollBars = ScrollBars.Horizontal                             ' スクロールバー(水平のみ)
            .ReadOnly = True
            .AllowUserToResizeColumns = True                                ' ユーザー 列サイズ変更

            '' 選択セル色定義
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 14.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .DefaultCellStyle.WrapMode = DataGridViewTriState.False

            '' 列定義
            .ColumnCount = dgvKizukind.Rows.Count
            .ColumnHeadersVisible = False
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 14.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 20                                       ' 列ヘッダサイズ

            ' 列が設定されている場合
            If 0 < .ColumnCount Then
                '行定義
                .RowCount = 2
                For ii As Integer = 0 To .Rows.Count - 1
                    .Rows(ii).Height = CInt(dgv.ClientSize.Height / dgv.Rows.Count)     '行の幅をセット
                Next ii

                '' 列定義
                For ii As Integer = 0 To .Columns.Count - 1
                    .Columns(ii).Width = 100
                    .Columns(ii).SortMode = DataGridViewColumnSortMode.NotSortable
                Next ii
            End If

        End With

    End Sub

    ''' <summary>
    ''' 画面項目イベント設定
    ''' </summary>
    ''' <param name="bSet">イベント設定有無 true：イベント設定 false：イベント解除</param>
    ''' <remarks></remarks>
    Private Sub SetItemEvent(ByVal bSet As Boolean)
        If bSet Then
            ' マップ
            AddHandler mcls_pMap.tEv_DrawPaint_End, AddressOf tEv_DrawPaint_End

            ' データグリッドビュー（コイル情報）
            AddHandler dgvCoil.Resize, AddressOf Ctrl_ReSize

            ' データグリッドビュー（疵種表示フィルター）
            AddHandler dgvKizukind.CellContentClick, AddressOf dgvGraph_CellContentClick
            AddHandler dgvKizukind.SelectionChanged, AddressOf dgvGraph_SelectionChanged

            ' データグリッドビュー（疵個数）
            AddHandler dgvKizuCntRate.Resize, AddressOf Ctrl_ReSize
            AddHandler dgvKizuCntRate.ColumnWidthChanged, AddressOf dgvColumnWidthChanged

            ' ピクチャボックス
            AddHandler picGraph.Paint, AddressOf PictureBox_Paint
            AddHandler picGraph.Resize, AddressOf PictureBox_Resize

            ' チェックボックス（プロット設定）
            AddHandler chkGraphProt.CheckedChanged, AddressOf chkGraphProt_CheckedChanged

            ' ラジオボタン（面選択）
            AddHandler radFront.CheckedChanged, AddressOf radbtn_CheckedChanged
            AddHandler radBack.CheckedChanged, AddressOf radbtn_CheckedChanged
            AddHandler radSum.CheckedChanged, AddressOf radbtn_CheckedChanged

            ' ラジオボタン（表示選択）
            AddHandler radKizuCnt.CheckedChanged, AddressOf radbtn_CheckedChanged
            AddHandler radKizuRate.CheckedChanged, AddressOf radbtn_CheckedChanged
        Else
            ' マップ
            RemoveHandler mcls_pMap.tEv_DrawPaint_End, AddressOf tEv_DrawPaint_End

            ' データグリッドビュー（コイル情報）
            RemoveHandler dgvCoil.Resize, AddressOf Ctrl_ReSize

            ' データグリッドビュー（疵種表示フィルター）
            RemoveHandler dgvKizukind.CellContentClick, AddressOf dgvGraph_CellContentClick
            RemoveHandler dgvKizukind.SelectionChanged, AddressOf dgvGraph_SelectionChanged

            ' データグリッドビュー（疵個数）
            RemoveHandler dgvKizuCntRate.Resize, AddressOf Ctrl_ReSize

            ' ピクチャボックス
            RemoveHandler picGraph.Paint, AddressOf PictureBox_Paint
            RemoveHandler picGraph.Resize, AddressOf PictureBox_Resize

            ' チェックボックス（プロット設定）
            RemoveHandler chkGraphProt.CheckedChanged, AddressOf chkGraphProt_CheckedChanged

            ' ラジオボタン（面選択）
            RemoveHandler radFront.CheckedChanged, AddressOf radbtn_CheckedChanged
            RemoveHandler radBack.CheckedChanged, AddressOf radbtn_CheckedChanged
            RemoveHandler radSum.CheckedChanged, AddressOf radbtn_CheckedChanged

            ' ラジオボタン（表示選択）
            RemoveHandler radKizuCnt.CheckedChanged, AddressOf radbtn_CheckedChanged
            RemoveHandler radKizuRate.CheckedChanged, AddressOf radbtn_CheckedChanged
        End If

    End Sub

    ''' <summary>
    ''' 疵種表示フィルター設定
    ''' 全疵種を疵種表示フィルターに設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetAllKizuKind()

        If m_typProtData.lstAllKizu Is Nothing Then Return

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)    'DBクラス
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String = ""               'Sql文作成用

        'DBオープン
        If Not tcls_DB.Open() Then Exit Sub

        'SQL文作成
        strSQL = ""
        strSQL &= " SELECT EdasysID "
        strSQL &= " FROM " & DB_TYPE_NAME
        strSQL &= " ORDER BY ID "

        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try

            While sqlRead.Read()
                m_typProtData.lstAllKizu.Add(sqlRead.GetInt32(0))
            End While

        Catch ex As Exception

        Finally
            ' DB開放
            If Not sqlRead Is Nothing Then sqlRead.Close()

            If Not tcls_DB Is Nothing Then
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
            End If

        End Try

    End Sub
#End Region

#Region "プロットデータ設定"
    ''' <summary>
    ''' プロットデータを画面項目に設定
    ''' </summary>
    Private Function SetProtData() As Boolean
        Dim bVal As Boolean = False

        Try
            '''' 画面項目に値を設定する為一時的にイベント解除
            SetItemEvent(False)

            '''' 画面項目表示
            SetItem()

            '''' 表示選択設定
            SetDspKind()

            '''' スクロールバー表示時のセル・フォントサイズ設定
            dgvCoilWidthResize()
            dgvKizuCntWidthResize()

            bVal = True
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("画面データ設定 例外[{0}]", ex.Message))
        Finally
            '''' イベント再設定
            SetItemEvent(True)
        End Try

        Return bVal
    End Function

    ''' <summary>
    ''' 画面項目表示
    ''' 表示選択、面選択、プロット表示に前回値を反映し
    ''' 取得したプロットデータからコイル情報、疵種情報、疵個数/混入率情報、日付情報表示を表示する
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetItem()

        ' 表示選択
        Dim nDsp As Integer = tMod.ReadIniInt(SO_GAMENN, "KIZUCNTTRANDDISP", LOCALINI_NAME, EM_KIZU_CNT_TRAND_DSP.CNT)
        If EM_KIZU_CNT_TRAND_DSP.RATE = nDsp Then
            radKizuRate.Select()
        Else
            radKizuCnt.Select()
        End If

        ' 面選択
        Dim nMen As Integer = tMod.ReadIniInt(SO_GAMENN, "KIZUCNTTRANDMEN", LOCALINI_NAME, EM_KIZU_CNT_TRAND_MEN.FRONT)
        If EM_KIZU_CNT_TRAND_MEN.SUM = nMen Then
            radSum.Select()
        ElseIf EM_KIZU_CNT_TRAND_MEN.BACK = nMen Then
            radBack.Select()
        Else
            radFront.Select()
        End If

        ' プロット表示
        Dim nProt As Integer = tMod.ReadIniInt(SO_GAMENN, "KIZUCNTTRANDPROT", LOCALINI_NAME, 0)
        If 1 = nProt Then
            chkGraphProt.Checked = True
        Else
            chkGraphProt.Checked = False
        End If

        ' 日付情報表示
        Dim strStartDate As String = String.Format("{0} {1}",
                              m_typProtData.typSearch.strStartDate, m_typProtData.typSearch.strStartTime).Trim
        Dim strEndDate As String = String.Format("{0} {1}",
                                      m_typProtData.typSearch.strEndDate, m_typProtData.typSearch.strEndTime).Trim
        If String.Empty = strStartDate Or String.Empty = strEndDate Then
            lblDate.Text = ""
        Else
            lblDate.Text = String.Format("期間：{0} ～ {1}", strStartDate, strEndDate)
        End If

        ' 全選択、全解除ボタン設定
        If Not m_typProtData.lstCoil Is Nothing AndAlso 0 < m_typProtData.lstCoil.Count _
           AndAlso Not m_typProtData.lstAllKizu Is Nothing AndAlso 0 < m_typProtData.lstAllKizu.Count Then
            ' 表示コイル・疵情報あり
            btnAllSelect.Enabled = True
            btnAllCancel.Enabled = True
        Else
            ' 上記以外
            btnAllSelect.Enabled = False
            btnAllCancel.Enabled = False
        End If

        ' コイル情報欄表示
        DspCoil()

        ' 疵種情報表示
        DspKizuKind()

        ' 疵個数/混入率情報表示
        DspKizuCntRate()

    End Sub

    ''' <summary>
    ''' 表示選択データの設定（疵個数 or 疵混入率）
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetDspKind()

        If radKizuRate.Checked Then
            m_KizuDspMethod = New GetKizuCntRate(AddressOf GetKizuRate)
            If radSum.Checked Then
                m_strDspFormat = "{0:#0.0}/{1:#0.0}"
            Else
                m_strDspFormat = "{0:#0.0}"
            End If
        Else
            m_KizuDspMethod = New GetKizuCntRate(AddressOf GetKizuCnt)
            If radSum.Checked Then
                m_strDspFormat = "{0}/{1}"
            Else
                m_strDspFormat = "{0}"
            End If
        End If
    End Sub

    ''' <summary>
    ''' コイル情報表示
    ''' </summary>
    Private Sub DspCoil()
        Try
            If m_typProtData.lstCoil Is Nothing OrElse 1 > m_typProtData.lstCoil.Count Then
                ' XX本目非表示
                pnlCoilCnt.Visible = False
                Return
            End If

            With dgvCoil

                Dim nSelect As Integer = m_typProtData.typSearch.nSelCoilIdx
                Dim typCoilInf As TYP_KIZU_TRAND_COILINF = m_typProtData.lstCoil(nSelect)

                ' XX本目設定
                pnlCoilCnt.Visible = True
                lblCoilCnt.Text = (nSelect + 1).ToString

                For ii As Integer = 0 To .ColumnCount - 1
                    Select Case ii
                        Case EM_COL_COLINF.ORDER_NO           ' 命令No
                            .Item(ii, 0).Value = typCoilInf.strOrderNo
                        Case EM_COL_COLINF.COIL_NO            ' 生産番号
                            .Item(ii, 0).Value = typCoilInf.strCoilNo
                        Case EM_COL_COLINF.TYPE_DIV           ' 品種
                            .Item(ii, 0).Value = typCoilInf.strHinCode
                        Case EM_COL_COLINF.MATERIAL           ' 材質
                            .Item(ii, 0).Value = typCoilInf.strSyuZai
                        Case EM_COL_COLINF.NEXT_STEP          ' 次工程
                            .Item(ii, 0).Value = typCoilInf.strNextCode
                        Case EM_COL_COLINF.KENSA_REF          ' 検査基準
                            .Item(ii, 0).Value = typCoilInf.strKenKijun ' 検査基準_表
                        Case EM_COL_COLINF.SIZE
                            ''サイズ(板厚[μm⇒mm]×板幅[mm])
                            Dim sThick As String = Format(typCoilInf.nCoilAtu / 1000, "0.000")
                            Dim sWidth As String = typCoilInf.nCoilWidth.ToString
                            .Item(ii, 0).Value = String.Format("{0}×{1,4}", sThick, sWidth)
                        Case EM_COL_COLINF.ORDER
                            ''サイズ(板厚[μm⇒mm]×板幅[mm])
                            Dim sThickSei As String = Format(typCoilInf.nCoilAtuSei / 1000, "0.000")
                            Dim sWidthSei As String = typCoilInf.nCoilWidSei.ToString
                            .Item(ii, 0).Value = String.Format("{0}×{1,4}", sThickSei, sWidthSei)
                        Case Else
                            .Item(ii, 0).Value = ""
                            .Item(ii, 0).Style.BackColor = .DefaultCellStyle().BackColor
                    End Select
                Next
            End With
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("コイル情報表示異常 [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵種情報表示
    ''' </summary>
    Private Sub DspKizuKind()
        Dim ii As Integer = 0

        Try
            If m_typProtData.lstAllKizu Is Nothing _
               OrElse m_typProtData.lstCoil Is Nothing Then Return

            dgvKizukind.RowCount = 0

            For Each nEdasysID As Integer In m_typProtData.lstAllKizu

                '''' 疵種データグリッド
                dgvKizukind.RowCount = dgvKizukind.RowCount + 1

                ' 疵種フィルター
                dgvKizukind(EM_KIZU_KIND_DATA_COL.CHECK, ii).Value = True

                ' 表示色
                Dim strColor As String = tMod.ReadIniStr(SO_GAMENN, "KIZUCNTTRANDCOLOR_" & nEdasysID, LOCALINI_NAME, "")
                If "" <> strColor Then
                    dgvKizukind(EM_KIZU_KIND_DATA_COL.COLOR, ii).Style.BackColor = Color.FromArgb(Convert.ToInt32("FF" & strColor, 16))
                Else
                    dgvKizukind(EM_KIZU_KIND_DATA_COL.COLOR, ii).Style.BackColor = mcls_Param.GetTypeSymbolColor(nEdasysID)
                End If

                ' 名称
                dgvKizukind(EM_KIZU_KIND_DATA_COL.NAME, ii).Value = mcls_Param.GetTypeName(nEdasysID)

                ' EdasysID保持（チェック状態保持用）
                dgvKizukind(EM_KIZU_KIND_DATA_COL.CHECK, ii).Tag = nEdasysID

                ii += 1
            Next
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("疵種情報表示異常 [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵個数/混入率情報表示
    ''' </summary>
    Private Sub DspKizuCntRate()
        Dim ii As Integer = 0

        Try
            If m_typProtData.lstAllKizu Is Nothing _
               OrElse m_typProtData.lstCoil Is Nothing _
               OrElse 1 > m_typProtData.lstCoil.Count Then Return

            ' 疵個数/混入率情報初期化
            KizuCntRateInit()

            For Each nEdasysID As Integer In m_typProtData.lstAllKizu

                '''' 疵個数データグリッド
                Dim nSelect As Integer = m_typProtData.typSearch.nSelCoilIdx
                Dim typCoilInf As TYP_KIZU_TRAND_COILINF = m_typProtData.lstCoil(nSelect)

                ' 疵個数/混入率取得
                Dim strValue As String = ""
                If radSum.Checked Then
                    strValue = String.Format(m_strDspFormat,
                                                 m_KizuDspMethod(nEdasysID, typCoilInf.lstKizuInf, EM_KIZU_CNT_TRAND_MEN.FRONT),
                                                 m_KizuDspMethod(nEdasysID, typCoilInf.lstKizuInf, EM_KIZU_CNT_TRAND_MEN.BACK))
                ElseIf radBack.Checked Then
                    strValue = String.Format(m_strDspFormat, m_KizuDspMethod(nEdasysID, typCoilInf.lstKizuInf, EM_KIZU_CNT_TRAND_MEN.BACK))
                Else
                    strValue = String.Format(m_strDspFormat, m_KizuDspMethod(nEdasysID, typCoilInf.lstKizuInf, EM_KIZU_CNT_TRAND_MEN.FRONT))
                End If

                dgvKizuCntRate(ii, EM_KIZU_CNT_DATA_COL.NAME).Value = dgvKizukind(EM_KIZU_KIND_DATA_COL.NAME, ii).Value.ToString
                dgvKizuCntRate(ii, EM_KIZU_CNT_DATA_COL.NAME).Style.ForeColor = dgvKizukind(EM_KIZU_KIND_DATA_COL.COLOR, ii).Style.BackColor
                dgvKizuCntRate(ii, EM_KIZU_CNT_DATA_COL.CNT).Value = strValue

                ii += 1
            Next
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("疵個数情報表示異常 [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵個数取得
    ''' 表示選択で疵個数を選択している場合、m_KizuDspMethodに当関数を設定
    ''' </summary>
    ''' <param name="nEdasysID">疵種</param>
    ''' <param name="lstKizu">疵情報</param>
    ''' <param name="nMen">対象面</param>
    ''' <remarks></remarks>
    Private Function GetKizuCnt(ByVal nEdasysID As Integer, ByVal lstKizu As List(Of TYP_KIZU_TRAND_KIZUINF), ByVal nMen As EM_KIZU_CNT_TRAND_MEN) As Integer

        For Each typKizu As TYP_KIZU_TRAND_KIZUINF In lstKizu
            If nEdasysID = typKizu.nEdasysID Then
                If EM_KIZU_CNT_TRAND_MEN.FRONT = nMen Then      ' 表面
                    Return typKizu.nOKizuCnt
                ElseIf EM_KIZU_CNT_TRAND_MEN.BACK = nMen Then   ' 裏面
                    Return typKizu.nUKizuCnt
                Else                                            ' 表裏合算
                    Return typKizu.nAKizuCnt
                End If
            End If
        Next

        Return 0
    End Function

    ''' <summary>
    ''' 疵混入率取得
    ''' 表示選択で疵混入率を選択している場合、m_KizuDspMethodに当関数を設定
    ''' </summary>
    ''' <param name="nEdasysID">疵種</param>
    ''' <param name="lstKizu">疵情報</param>
    ''' <param name="nMen">対象面</param>
    ''' <remarks></remarks>
    Private Function GetKizuRate(ByVal nEdasysID As Integer, ByVal lstKizu As List(Of TYP_KIZU_TRAND_KIZUINF), ByVal nMen As EM_KIZU_CNT_TRAND_MEN) As Single
        Dim result As Integer = 0

        For Each typKizu As TYP_KIZU_TRAND_KIZUINF In lstKizu
            If nEdasysID = typKizu.nEdasysID Then
                If EM_KIZU_CNT_TRAND_MEN.FRONT = nMen Then      ' 表面
                    Return typKizu.dOKizuRate
                ElseIf EM_KIZU_CNT_TRAND_MEN.BACK = nMen Then   ' 裏面
                    Return typKizu.dUKizuRate
                Else                                            ' 表裏合算
                    Return typKizu.dAKizuRate
                End If
            End If
        Next

        Return result
    End Function
#End Region

#Region "画面描画"
    ''' <summary>
    ''' 画面データ描画 (設定変更時に実行) (画面表示時にも)
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetDsp()
        Dim bVal As Boolean = False

        '''' プロットデータ設定
        If SetProtData() Then
            '''' マップ再描画
            ReDrawMap()
        End If

    End Sub

    ''' <summary>
    ''' マップ再描画
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ReDrawMap()

        '''' マップ領域再定義
        MapReSetring()

        '''' マップ再生成依頼
        mcls_pMap.DrawPaint()

        '''' マップ再描画
        picGraph.Refresh()

    End Sub

    ''' <summary>
    ''' マップ再定義
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub MapReSetring()

        '''' グラフ描画に必要な情報を設定
        With mcls_pMap

            Dim nXMax As Integer = 0
            Dim nYMax As Integer = DEFAULT_Y_MAX

            '''' X軸最大値の設定
            If Not m_typProtData.lstCoil Is Nothing AndAlso 1 < m_typProtData.lstCoil.Count Then
                ' コイル情報が2件以上あった場合にX軸最大値を設定
                ' コイル情報が1件の場合、コイル情報は始点のみになるのでデフォルトの0のままでよい
                nXMax = m_typProtData.lstCoil.Count
            End If

            '''' Y軸最大値の設定
            If Not m_typProtData.lstCoil Is Nothing AndAlso 0 < m_typProtData.lstCoil.Count _
               AndAlso Not m_typProtData.lstAllKizu Is Nothing AndAlso 0 < m_typProtData.lstAllKizu.Count Then
                ' コイル情報が存在かつ疵情報も取得していた場合にY軸最大値を設定
                If radKizuRate.Checked Then
                    ' 疵混入率は四捨五入してY軸最大値を設定
                    If radSum.Checked Then
                        nYMax = CInt(m_typProtData.typSearch.dAKizuRateMax + 0.5)
                    ElseIf radBack.Checked Then
                        nYMax = CInt(m_typProtData.typSearch.dUKizuRateMax + 0.5)
                    Else
                        nYMax = CInt(m_typProtData.typSearch.dOKizuRateMax + 0.5)
                    End If
                Else
                    If radSum.Checked Then
                        nYMax = m_typProtData.typSearch.nAKizuCntMax
                    ElseIf radBack.Checked Then
                        nYMax = m_typProtData.typSearch.nUKizuCntMax
                    Else
                        nYMax = m_typProtData.typSearch.nOKizuCntMax
                    End If
                End If

                ' Y軸最大値に0を設定するとグラフの描画がおかしくなる（tClass_MapBaseの仕様？）ので
                ' Y軸最大値が0ならデフォルト値を設定
                nYMax = IIf(0 = nYMax, DEFAULT_Y_MAX, nYMax)
            End If


            '''' X軸の間隔を取得（可変設定）
            Dim nXLen As Integer = picGraph.Width - (GRAPH_PADDING_LEFT + GRAPH_PADDING_RIGHT) ' グラフX軸の長さを取得
            Dim nXSplitCnt As Integer = CInt(Math.Floor(nXLen / INTERVAL_X_PIC_MAX))           ' X軸の最大分割数を取得
            Dim nXMaxSplitVal As Integer = CInt(Math.Ceiling(nXMax / nXSplitCnt))              ' X軸最大値を分割する値を取得

            ' X軸の間隔を求める
            Dim nXInterval As Integer
            If 1 >= nXMaxSplitVal / INTERVAL_X_INIT Then         ' 分割値が間隔初期値以下
                ' 間隔を初期値に設定
                nXInterval = INTERVAL_X_INIT
            Else
                ' 分割値をINTERVAL_X_NEXTの倍数に修正して間隔を設定
                nXInterval = INTERVAL_X_NEXT * CInt(Math.Ceiling(nXMaxSplitVal / INTERVAL_X_NEXT))
            End If


            '''' Y軸の間隔を取得（可変設定）
            Dim nYSplitCnt As Integer = CInt(Math.Floor(DEFAULT_Y_MAX / INTERVAL_Y_NEXT))      ' Y軸の最大分割数を取得
            Dim nYMaxSplitVal As Integer = CInt(Math.Ceiling(nYMax / nYSplitCnt))              ' Y軸最大値を分割する値を取得

            ' Y軸の間隔を求める
            Dim nYInterval As Integer
            If 1 >= nYMaxSplitVal / INTERVAL_Y_INIT Then         ' 分割値が間隔初期値以下
                ' 間隔を初期値に設定
                nYInterval = INTERVAL_Y_INIT
            Else
                ' 分割値をINTERVAL_Y_NEXTの倍数に修正して間隔を設定
                nYInterval = INTERVAL_Y_NEXT * CInt(Math.Ceiling(nYMaxSplitVal / INTERVAL_Y_NEXT))
            End If

            ' X,Y軸の間隔を設定
            .tProp_MemoriInf_ScaleInterval = New tClass.tClass_MapBase.xyValue(nXInterval, nYInterval)


            '''' マップ領域設定
            ' Y軸の間隔が初期値の場合、Y軸の最大値をINTERVAL_Y_NEXTに設定
            nYInterval = IIf(INTERVAL_Y_INIT = nYInterval, INTERVAL_Y_NEXT, nYInterval)

            ' Y軸の最大値をINTERVAL_Y_NEXTで割り切れる値に補正（Y軸の一番上に常に数値を表示する）
            If (0 <> nYMax Mod nYInterval) Then nYMax = nYMax + nYInterval - (nYMax Mod nYInterval)

            .tProp_MapInf_LimitXmax = nXMax
            .tProp_MapInf_LimitYmax = nYMax

        End With
    End Sub
#End Region

#Region "その他"
    ''' <summary>
    ''' 疵個数トレンドグラフ設定保存(Iniファイル書込み)
    ''' </summary>
    Private Sub PutKizuCntTrendDsp()

        Try
            'iniファイル情報書込
            For ii As Integer = 0 To dgvKizukind.RowCount - 1
                ' フィルター設定
                If True = CBool(dgvKizukind(EM_KIZU_KIND_DATA_COL.CHECK, ii).Value) Then
                    tMod.WriteIni(SO_GAMENN, "KIZUCNTTRANDFILTER_" & dgvKizukind(EM_KIZU_KIND_DATA_COL.CHECK, ii).Tag.ToString, LOCALINI_NAME, "1")
                Else
                    tMod.WriteIni(SO_GAMENN, "KIZUCNTTRANDFILTER_" & dgvKizukind(EM_KIZU_KIND_DATA_COL.CHECK, ii).Tag.ToString, LOCALINI_NAME, "0")
                End If

                ' 選択色
                Dim strColor As String = Microsoft.VisualBasic.Right(Hex(dgvKizukind(EM_KIZU_KIND_DATA_COL.COLOR, ii).Style.BackColor.ToArgb), 6)
                tMod.WriteIni(SO_GAMENN, "KIZUCNTTRANDCOLOR_" & dgvKizukind(EM_KIZU_KIND_DATA_COL.CHECK, ii).Tag.ToString, LOCALINI_NAME, strColor)
            Next

            ' 表示選択
            If radKizuRate.Checked Then
                tMod.WriteIni(SO_GAMENN, "KIZUCNTTRANDDISP", LOCALINI_NAME, "1")
            Else
                tMod.WriteIni(SO_GAMENN, "KIZUCNTTRANDDISP", LOCALINI_NAME, "0")
            End If

            ' 面選択
            If radSum.Checked Then
                tMod.WriteIni(SO_GAMENN, "KIZUCNTTRANDMEN", LOCALINI_NAME, "2")
            ElseIf radBack.Checked Then
                tMod.WriteIni(SO_GAMENN, "KIZUCNTTRANDMEN", LOCALINI_NAME, "1")
            Else
                tMod.WriteIni(SO_GAMENN, "KIZUCNTTRANDMEN", LOCALINI_NAME, "0")
            End If

            ' プロット表示
            If chkGraphProt.Checked Then
                tMod.WriteIni(SO_GAMENN, "KIZUCNTTRANDPROT", LOCALINI_NAME, "1")
            Else
                tMod.WriteIni(SO_GAMENN, "KIZUCNTTRANDPROT", LOCALINI_NAME, "0")
            End If
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("INIファイル書込時異常 [{0}]", ex.Message))
        End Try

    End Sub

#End Region


    '/ /////////////////////////////////////////////////////////////////////////////////
    '  イベントハンドラ関数
    '/ /////////////////////////////////////////////////////////////////////////////////
#Region "リサイズ関連イベント"
    ''' <summary>
    ''' リサイズ
    ''' </summary>
    ''' <param name="sender">イベント発生元</param>
    ''' <param name="e">イベントパラメータ</param>
    ''' <remarks></remarks>
    Private Sub Ctrl_ReSize(ByVal sender As Object, ByVal e As System.EventArgs)
        If sender.Equals(dgvKizuCntRate) Then
            dgvKizuCntWidthResize()
        ElseIf sender.Equals(dgvCoil) Then
            dgvCoilWidthResize()
        End If
    End Sub

    ''' <summary>
    ''' リサイズによる表示変更（コイル情報）
    ''' </summary>
    Private Sub dgvCoilWidthResize()
        Dim hsc As HScrollBar

        ' 行幅を設定
        hsc = CType(dgvCoil.Controls(0), HScrollBar)
        If hsc.Visible Then
            ' 水平スクロールバー表示中
            dgvCoil.ColumnHeadersHeight = 16
            dgvCoil.Rows(0).Height = dgvCoil.ClientSize.Height - dgvCoil.ColumnHeadersHeight - hsc.Height
        Else
            ' 水平スクロールバー未表示
            dgvCoil.ColumnHeadersHeight = 20
            dgvCoil.Rows(0).Height = dgvCoil.ClientSize.Height - dgvCoil.ColumnHeadersHeight
        End If

        ' 列数分の文字サイズを設定
        For ii As Integer = 0 To dgvCoil.ColumnCount - 1
            If hsc.Visible Then
                ' 水平スクロールバー表示中
                dgvCoil.Item(ii, 0).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            Else
                ' 水平スクロールバー未表示
                dgvCoil.Item(ii, 0).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 20.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            End If
        Next

    End Sub

    ''' <summary>
    ''' リサイズによる表示変更（疵個数/疵混入率情報）
    ''' </summary>
    Private Sub dgvKizuCntWidthResize()
        Dim hsc As HScrollBar
        Dim nHeight As Integer

        If 1 > dgvKizuCntRate.Rows.Count Then Exit Sub

        ' 行幅を取得
        hsc = CType(dgvKizuCntRate.Controls(0), HScrollBar)
        If hsc.Visible Then
            ' 水平スクロールバー表示中
            nHeight = CInt((dgvKizuCntRate.ClientSize.Height - hsc.Height) / dgvKizuCntRate.Rows.Count)
        Else
            ' 水平スクロールバー未表示
            nHeight = CInt(dgvKizuCntRate.ClientSize.Height / dgvKizuCntRate.Rows.Count)
        End If

        ' 行数分ループ
        For ii As Integer = 0 To dgvKizuCntRate.Rows.Count - 1
            dgvKizuCntRate.Rows(ii).Height = nHeight   '行幅を設定

            ' 列数分の文字サイズを設定
            For jj As Integer = 0 To dgvKizuCntRate.ColumnCount - 1
                If hsc.Visible Then
                    '水平スクロールバー表示中
                    dgvKizuCntRate.Item(jj, ii).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                Else
                    ' 水平スクロールバー未表示
                    Dim emSize As Single = 14.0
                    ' 表と裏の疵個数/疵混入率値をセル領域内に全て表示できない場合（9文字以上）
                    If ii = 1 AndAlso Not dgvKizuCntRate(jj, ii).Value Is Nothing AndAlso dgvKizuCntRate(jj, ii).Value.ToString.Count > 8 Then
                        ' セル領域内に文字を全て表示できるように文字サイズを変更
                        emSize = 10.0
                    End If

                    dgvKizuCntRate(jj, ii).Style.Font = New System.Drawing.Font("ＭＳ 明朝", emSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                End If
            Next
        Next
    End Sub

    ''' <summary>
    ''' 列幅のサイズ変更イベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    Private Sub dgvColumnWidthChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewColumnEventArgs)

        Dim dgv As tCnt.tCnt_DataGridView = CType(sender, tCnt.tCnt_DataGridView)

        ' 疵個数/疵混入率欄表示領域再設定
        dgvKizuCntWidthResize()

    End Sub
#End Region

#Region "疵種情報欄関連イベント"
    ''' <summary>
    ''' 全選択ボタン押下
    ''' </summary>
    ''' <param name="sender">イベント発生元</param>
    ''' <param name="e">イベントパラメータ</param>
    ''' <remarks></remarks>
    Private Sub btnAllSelect_Click(sender As Object, e As EventArgs) Handles btnAllSelect.Click

        Try
            If m_typProtData.lstCoil Is Nothing OrElse 1 > m_typProtData.lstCoil.Count Then Return

            'チェックボックスを全てON
            CheckGrid(dgvKizukind, True)

            ' マップ再描画
            ReDrawMap()

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("全選択ボタン押下異常 [{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' 全解除ボタン押下
    ''' </summary>
    ''' <param name="sender">イベント発生元</param>
    ''' <param name="e">イベントパラメータ</param>
    ''' <remarks></remarks>
    Private Sub btnAllCancel_Click(sender As Object, e As EventArgs) Handles btnAllCancel.Click

        Try
            If m_typProtData.lstCoil Is Nothing OrElse 1 > m_typProtData.lstCoil.Count Then Return

            'チェックボックスを全てOFF
            CheckGrid(dgvKizukind, False)

            ' マップ再描画
            ReDrawMap()

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("全解除ボタン押下異常 [{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' グリッドのチェックボックスセット
    ''' </summary>
    ''' <param name="dgvData">データグリッド</param>
    ''' <param name="bValue">True or False</param>
    ''' <remarks></remarks>
    Private Sub CheckGrid(ByVal dgvData As DataGridView, ByVal bValue As Boolean)

        Dim ii As Integer

        ' グリッドの全てのチェックボックスを設定
        For ii = 0 To dgvData.RowCount - 1
            dgvData(EM_KIZU_KIND_DATA_COL.CHECK, ii).Value = bValue
        Next ii

    End Sub


    ''' <summary>
    ''' 疵種フィルターのセルのボタンが押されたときの処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvGraph_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)

        Try
            If m_typProtData.lstCoil Is Nothing Then Return

            Dim dgv As DataGridView = CType(sender, DataGridView)

            ' 行がクリックされていなければ終了
            If 0 > e.RowIndex Then Exit Sub

            ' 列インデックスでどの列がクリックされたか判別
            Select Case e.ColumnIndex
                Case EM_KIZU_KIND_DATA_COL.COLOR  '表示色
                    GetDlgColor(dgv, e.ColumnIndex, e.RowIndex)

                    ' 疵個数/混入率情報欄に変更色を反映
                    dgvKizuCntRate(e.RowIndex, EM_KIZU_CNT_DATA_COL.NAME).Style.ForeColor = dgv(e.ColumnIndex, e.RowIndex).Style.BackColor

                Case EM_KIZU_KIND_DATA_COL.CHECK
                    dgv(e.ColumnIndex, e.RowIndex).Value = dgv(e.ColumnIndex, e.RowIndex).EditedFormattedValue

                Case EM_KIZU_KIND_DATA_COL.NAME
            End Select

            ' マップ再描画
            ReDrawMap()
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("疵種フィルターセル押下異常 [{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' 表示色ボタンが押されたら、色選択ダイアログを表示し、ボタンの色を変更
    ''' </summary>
    ''' <param name="dgv">対象のデータグリッドビュー</param>
    ''' <param name="nColIndex">ボタンの列インデックス</param>
    ''' <param name="nRowIndex">ボタンの行インデックス</param>
    ''' <remarks></remarks>
    Private Sub GetDlgColor(ByVal dgv As DataGridView, ByVal nColIndex As Integer, ByVal nRowIndex As Integer)
        '色選択ダイアログの設定
        '色の作成を使用しない
        dlgColor.AllowFullOpen = False

        '色選択ダイアログの選択色を、ボタンの背景色にする
        dlgColor.Color = dgv(nColIndex, nRowIndex).Style.BackColor

        ' 色選択ダイアログでokボタンが押されたら、ボタンの色を変更する 
        If Windows.Forms.DialogResult.OK = dlgColor.ShowDialog() Then
            '選択色をボタンの背景色に
            dgv(nColIndex, nRowIndex).Style.BackColor = dlgColor.Color
        End If

        dlgColor.Dispose()
    End Sub


    ''' <summary>
    ''' dgvの選択が変更されたら、セルの選択状態をキャンセル
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvGraph_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs)
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' 対象のdgv
        dgv.CurrentCell.Selected = False
    End Sub

#End Region

#Region "画面項目チェックイベント"
    ''' <summary>
    ''' プロット表示チェック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chkGraphProt_CheckedChanged(ByVal sender As Object, ByVal e As System.EventArgs)

        Try
            If m_typProtData.lstCoil Is Nothing OrElse 1 > m_typProtData.lstCoil.Count Then Return

            ' マップ再描画
            ReDrawMap()

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("プロット表示チェック異常 [{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' ラジオボタンチェック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub radbtn_CheckedChanged(ByVal sender As Object, ByVal e As System.EventArgs)

        Try
            If m_typProtData.lstCoil Is Nothing OrElse 1 > m_typProtData.lstCoil.Count Then Return

            ' 表示選択設定
            SetDspKind()

            ' 疵個数/混入率情報表示
            DspKizuCntRate()

            ' 疵個数/疵混入率欄表示領域再設定
            dgvKizuCntWidthResize()

            ' マップ再描画
            ReDrawMap()

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("ラジオボタンチェック異常 [{0}]", ex.Message))
        End Try

    End Sub

#End Region

#Region "グラフ関連イベント"
    ''' <summary>
    ''' グラフ押下
    ''' グラフ押下された場合、グラフのY軸に対応するコイル情報を画面上に表示
    ''' </summary>
    ''' <param name="sender">イベント発生元</param>
    ''' <param name="e">イベントパラメータ</param>
    ''' <remarks></remarks>
    Private Sub picGraph_Click(sender As Object, e As EventArgs) Handles picGraph.Click

        Try
            If m_typProtData.lstCoil Is Nothing OrElse 1 > m_typProtData.lstCoil.Count Then Return

            Dim pos As Point = picGraph.PointToClient(Windows.Forms.Cursor.Position)         ' クリック位置 (画面座標からPicコントロールの左上からの座標に変換)

            '''' X軸範囲設定
            Dim nXHosei As Integer = 0
            ' コイルが２つ以上存在
            If 1 < m_typProtData.lstCoil.Count Then
                ' コイル間の幅/2をX軸の範囲に設定
                nXHosei = CInt((mcls_pMap.GetVtoP_X(2) - mcls_pMap.GetVtoP_X(1)) / 2)
            End If

            '''' クリック位置に一番近いコイル情報を取得
            ' コイル数分ループ
            For ii As Integer = 1 To m_typProtData.lstCoil.Count

                ' X軸表示最大件数を超えた場合
                If ii > mcls_pMap.tProp_MapInf_LimitXmax Then Exit For

                ' X軸ポイント算出
                Dim x1 As Integer = mcls_pMap.GetVtoP_X(ii)

                ' 対象外
                If x1 - nXHosei > pos.X OrElse x1 + nXHosei < pos.X Then Continue For

                ' 選択コイル更新
                m_typProtData.typSearch.nSelCoilIdx = ii - 1

                ' コイル情報欄表示
                DspCoil()

                ' 疵個数/混入率情報表示
                DspKizuCntRate()

                ' 疵個数/疵混入率欄表示領域再設定
                dgvKizuCntWidthResize()

                Exit For
            Next

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("グラフ押下異常 [{0}]", ex.Message))
        End Try

    End Sub

#End Region

#Region "描画関連イベント"
    ''' <summary>
    ''' 描画
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' </summary>
    Private Sub PictureBox_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs)

        '''' ビットマップ取得
        Dim bmp As Bitmap = mcls_pMap.tProp_Bmp
        If bmp Is Nothing Then Return

        '''' 背景をセット
        e.Graphics.DrawImage(bmp, 0, 0)

    End Sub

    ''' <summary>
    ''' ピクチャボックスのサイズ変更
    ''' </summary>
    Private Sub PictureBox_Resize(ByVal sender As Object, ByVal e As System.EventArgs)
        '''' ピクチャボックスサイズを再設定
        mcls_pMap.tProp_AriaInf_AriaSize = picGraph.Size

        '''' 再描画
        ReDrawMap()
    End Sub

    ''' <summary>
    ''' 画面データ描画
    ''' </summary>
    ''' <param name="cls"></param>
    ''' <param name="g"></param>
    ''' <remarks></remarks>
    Private Sub tEv_DrawPaint_End(ByVal cls As tClassLibrary.tClass.tClass_MapBase, ByVal g As System.Drawing.Graphics)

        ' コイル情報未取得の場合は描画なし
        If m_typProtData.lstCoil Is Nothing OrElse 1 > m_typProtData.lstCoil.Count Then Return

        Dim lstCoil As List(Of TYP_KIZU_TRAND_COILINF) = m_typProtData.lstCoil
        Dim nMen As EM_KIZU_CNT_TRAND_MEN

        If radFront.Checked Then
            nMen = EM_KIZU_CNT_TRAND_MEN.FRONT
        ElseIf radBack.Checked Then
            nMen = EM_KIZU_CNT_TRAND_MEN.BACK
        Else
            nMen = EM_KIZU_CNT_TRAND_MEN.SUM
        End If

        '''' 折れ線グラフ描画
        Dim dVal As Single
        Dim x1, y1, x2, y2 As Integer
        Try
            For ii As Integer = 0 To dgvKizukind.RowCount - 1

                ' 疵種表示有無
                If False = CType(dgvKizukind(EM_KIZU_KIND_DATA_COL.CHECK, ii).Value, Boolean) Then Continue For

                Dim nEdasysID As Integer = CInt(dgvKizukind(EM_KIZU_KIND_DATA_COL.CHECK, ii).Tag)
                Dim colLine As Color = dgvKizukind(EM_KIZU_KIND_DATA_COL.COLOR, ii).Style.BackColor

                ' 始点算出 (先頭の座標算出)
                x1 = cls.GetVtoP_X(1)
                dVal = m_KizuDspMethod(nEdasysID, lstCoil(0).lstKizuInf, nMen)

                If dVal > cls.tProp_MapInf_LimitYmax Then dVal = cls.tProp_MapInf_LimitYmax
                y1 = cls.GetVtoP_Y(dVal)

                Using p As New Pen(colLine, 1)
                    ' 始点のみプロットの指定
                    If chkGraphProt.Checked Then
                        g.FillEllipse(New SolidBrush(p.Color), x1 - 4, y1 - 4, 8, 8)
                    End If

                    ' コイル情報が1件のみ
                    If 1 = lstCoil.Count Then
                        ' 描画
                        g.DrawLine(p, x1, y1, x1, y1)
                        Continue For
                    End If

                    ' コイル2本目以降の描画
                    For jj As Integer = 1 To lstCoil.Count - 1

                        ' 表示最大件数を超えた場合
                        If jj > cls.tProp_MapInf_LimitXmax - 1 Then Exit For

                        ' 終点算出
                        x2 = cls.GetVtoP_X(jj + 1)
                        dVal = m_KizuDspMethod(nEdasysID, lstCoil(jj).lstKizuInf, nMen)

                        If dVal > cls.tProp_MapInf_LimitYmax Then dVal = cls.tProp_MapInf_LimitYmax
                        y2 = cls.GetVtoP_Y(dVal)

                        ' 描画
                        g.DrawLine(p, x1, y1, x2, y2)

                        ' プロットの指定
                        If chkGraphProt.Checked Then
                            g.FillEllipse(New SolidBrush(p.Color), x2 - 4, y2 - 4, 8, 8)
                        End If

                        ' 次の準備
                        x1 = x2
                        y1 = y2
                    Next
                End Using
            Next
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("折れ線グラフ描画時異常 [{0}]", ex.Message))
        End Try


        '''' メモリ軸のコイルNoを表示
        Dim b As SolidBrush = Nothing
        Dim f As Font = Nothing
        x1 = 0
        y1 = 0
        Try
            b = New SolidBrush(Color.Black)
            f = New Font("ＭＳ ゴシック", 11, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)

            ' 原点表示
            y1 = picGraph.Height
            x1 = cls.GetVtoP_X(1)
            cls.DrawString(g, x1, y1, 0, -1, lstCoil(0).strCoilNo, ContentAlignment.TopCenter, f, b)

            ' 原点以降表示
            Dim nInterval As Integer = CInt(cls.tProp_MemoriInf_ScaleInterval.X)
            For ii As Integer = nInterval To lstCoil.Count Step nInterval
                ' 表示するデータは、メモリが100の場合は、配列の99。メモリが配列の0を表示している為
                x1 = cls.GetVtoP_X(ii)
                cls.DrawString(g, x1, y1, 0, -1, lstCoil(ii - 1).strCoilNo, ContentAlignment.TopCenter, f, b)
            Next
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("コイルNo描画時異常 [{0}]", ex.Message))
        Finally
            b.Dispose()
            f.Dispose()
        End Try

    End Sub
#End Region

#Region "キーイベント"
    ''' <summary>
    ''' ファンクションキーイベント
    ''' </summary>
    ''' <param name="FncNo">ファンクションNo 0オリジン</param>
    ''' <param name="enable">ボタンコントロールの有効無効</param>
    ''' <param name="onKey"></param>
    ''' <remarks></remarks>
    Private Sub frmDefIncidenceGraph_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        Try
            Select Case FncNo
                Case 4                                  ' 検索
                    ' 疵個数トレンドグラフ検索画面の表示
                    ShowDefTrand()

                Case 9                                  ' 画面印刷
                    PrintOut()

                Case 10                                 ' 画面保存
                    HardCopy(IMG_PATH, "疵個数トレンドグラフ画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)

                Case 11                                 ' RAS画面表示
                    MyBase.ExecFromClose()
            End Select
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("ファンクション押下異常 [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵個数トレンド検索画面表示
    ''' </summary>
    Private Sub ShowDefTrand()

        ' 疵個数トレンド検索画面表示
        mfrm_DefTrand.ShowDialog()

        ' 検索画面設定保存
        mfrm_DefTrand.SaveCheck()

        ' グラフ出力ボタン押下
        If frmDefTrand.KIZU_TRAND_RETURN.nShowKizuTrand = mfrm_DefTrand.ReturnCode Then

            ' 疵個数トレンド画面設定保存
            PutKizuCntTrendDsp()

            '''' プロットデータをセット
            m_typProtData = mfrm_DefTrand.KizuTrandInf

            ' データ再設定
            SetDsp()
        End If

    End Sub

#End Region


End Class
