'*******************************************************************************
'　コイルトレンド画面
'	[Ver]
'		Ver.01    2012/07/12  初版
'
'	[メモ]
'		コイルトレンド情報画面の表示
'*******************************************************************************
Option Strict On

Imports tClassLibrary

Public Class frmCoilTrand
    '---------------------------------------------------------------------------
    ' 列挙体
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' コイル情報のデータグリッド列
    ''' </summary>
    ''' <remarks></remarks>
    Private Enum EM_COL_COLINF
        ORDER_NO = 0        ' 命令No
        COIL_LEN            ' コイル長
        KANRI_NO            ' 管理No
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
    ''' トレンドデータのデータグリッド列
    ''' </summary>
    Private Enum EM_COL_CALC
        MAX_NAME = 0
        MAX_DATA
        MIN_NAME
        MIN_DATA
        AVG_NAME
        AVG_DATA
        MAX_COL             ' 最大列数
    End Enum

    ''' <summary>
    ''' 表示コイルトレンド
    ''' </summary>
    Private Enum EM_COIL_TREND
        SPEED = 0           ' ライン速度
        WIDTH               ' 板幅
        CAM_KIDO_O          ' カメラ輝度(表)
        CAM_KIDO_U          ' カメラ輝度(裏)
        CAM_GAIN_O          ' カメラゲイン(表)
        CAM_GAIN_U          ' カメラゲイン(裏)
        CAM_TIME_O          ' カメラ露光時間(表)
        CAM_TIME_U          ' カメラ露光時間(裏)
        WALK                ' ウォーク量
        LED_O               ' 照明装置調光値(表)
        LED_U               ' 照明装置調光値(裏)
        MAX
    End Enum

    ' 凡例のデータグリッド列
    Private Enum em_DataColumnType
        iColor = 0                      ' Color
        iCheck                          ' Check
        iName                           ' Name
    End Enum

    '/////////////////////////////////////////////////////////////////////////////////
    '構造体
    '/////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' コイルトレンド情報
    ''' </summary>
    Private Structure TYP_TREND_INF
        Dim strGroupName As String                                      ' グループ名
        Dim nMin As Integer                                             ' 下限値
        Dim nMax As Integer                                             ' 上限値
        Dim nDecPlaces As Integer                                       ' 小数点桁数
        Dim lstItem As ArrayList
        Public Sub initialize()
            lstItem = New ArrayList
        End Sub
    End Structure

    ''' <summary>
    ''' カメラ情報
    ''' </summary>
    ''' <remarks></remarks>
    Private Structure CAM_INFO
        Dim nKidoR As Integer               ' 輝度R_C
        Dim nKidoG As Integer               ' 輝度G_C
        Dim nKidoB As Integer               ' 輝度B_C
        Dim dGainR As Single                ' ゲインR
        Dim dGainG As Single                ' ゲインG
        Dim dGainB As Single                ' ゲインB
        Dim nTimeR As Integer               ' 露光時間R
        Dim nTimeG As Integer               ' 露光時間G
        Dim nTimeB As Integer               ' 露光時間B
    End Structure

    ''' <summary>
    ''' コイルトレンド表示データ
    ''' </summary>
    ''' <remarks></remarks>
    Private Structure TYP_TREND_DATA
        Dim TorB As Integer             ' 表裏区分 (0:表 1:裏)
        Dim nPos As Integer             ' 取得位置
        Dim nSpeed As Integer           ' ライン速度
        Dim dItahaba() As Single          ' 板幅
        Dim typCamInf() As CAM_INFO     ' カメラ情報
        Dim dWalk As Single             ' ｳｫｰｸ量
        Dim nDimming() As Integer         ' 調光値
        Public Sub initialize()
            ReDim typCamInf(mc_CameraN - 1)
            ReDim dItahaba(2 - 1)
            ReDim nDimming(5 - 1)
        End Sub
    End Structure

    ''' <summary>
    ''' コイルトレンド表示計算値
    ''' </summary>
    Private Structure TYP_TREND_CALC
        Dim min As String               '最小値
        Dim max As String               '最大値
        Dim avg As String               '平均値
        Public Sub initialize()
            min = String.Empty
            max = String.Empty
            avg = String.Empty
        End Sub
    End Structure

    ''' <summary>
    ''' トレンドグラフ表示テーブル
    ''' </summary>
    Private Structure TYP_TREND_DSP
        Dim clsGraph As tClass.tClass_MapBase                           ' グラフエリア
        Dim dgvHanrei As DataGridView                                   ' 凡例配列
        Dim nudGraphMin As NumericUpDown                                ' 下限値配列
        Dim nudGraphMax As NumericUpDown                                ' 上限値配列
        Dim picGraph As PictureBox                                      ' グラフ表示エリア
        Dim chkGraphProt As CheckBox                                    ' グラフプロット表示・非表示          
        Dim cmbName As ComboBox                                         ' トレンド名
        Dim dgvCalc As tCnt.tCnt_DataGridView                           ' 最大/最小/平均表示
        Dim emType As EM_COIL_TREND
        Dim nCnt As Integer                                             '件数
        Dim nCoilPos() As Integer                                       '取得位置
        Dim nData(,) As Single                                          'データ
        Dim nMin As Integer                                             '下限値
        Dim nMax As Integer                                             '上限値
    End Structure

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const mc_NUM As Integer = 5                                 ' グラフグループ数
    Private Const mc_CameraN As Integer = 10                            ' カメラ台数
    Private Const mc_XScale As Integer = 10                             ' X軸スケール間隔(等分)
    Private Const mc_YScale As Integer = 10                             ' Y軸スケール間隔(等分)

    Private Const NON_TRAND_DATA As Integer = -999999

    ' 初期カラー値
    Private ReadOnly sdcDefault() As System.Drawing.Color = {
        Color.Red, Color.Orange, Color.Yellow, Color.Green, Color.Lime,
        Color.GreenYellow, Color.Blue, Color.RoyalBlue, Color.Cyan, Color.DarkCyan,
        Color.Violet, Color.DeepPink, Color.DarkRed, Color.PaleVioletRed, Color.DarkGreen,
        Color.DarkSeaGreen, Color.PaleGreen, Color.DarkBlue, Color.Purple, Color.Magenta,
        Color.IndianRed, Color.Indigo, Color.Khaki, Color.LawnGreen, Color.MediumOrchid,
        Color.Tomato, Color.Brown, Color.Aquamarine, Color.DeepSkyBlue, Color.BurlyWood
    }

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバ 変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_Log As tClass.tClass_LogManager                        ' ログ管理
    Private m_typCoilInf As clsDataMap.TYP_COIL_INF                     ' コイル情報
    Private m_bNagate As Boolean                                        ' 実績画面長手方向反転(True:反転あり、false:反転なし）

    '' 保持ワークデータ
    Private m_typTrendDsp(mc_NUM - 1) As TYP_TREND_DSP
    Private m_lstData As List(Of TYP_TREND_DATA)                        ' トレンドデータ
    Private m_dctCalc As Dictionary(Of EM_COIL_TREND, TYP_TREND_CALC)   ' 計算値データ（最小、最大、平均)
    Private m_dctTrendInf As Dictionary(Of EM_COIL_TREND, TYP_TREND_INF)
    Private m_bSetEnd As Boolean                                        ' 初期情報セット完了フラグ(True:セット完了、False:未完了)

    '---------------------------------------------------------------------------
    ' メンバ関数
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' コイルトレンド画面を作成します
    ''' </summary>
    ''' <param name="typCoilInf">コイル情報</param>
    ''' <param name="bNagate">実績画面長手方向反転(True:反転あり、false:反転なし)</param>
    ''' <param name="log">ログ管理</param>
    ''' <remarks></remarks>
    Public Sub New(ByVal typCoilInf As clsDataMap.TYP_COIL_INF, ByVal bNagate As Boolean, ByRef log As tClass.tClass_LogManager)
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

            mcls_Log = log                  ' ログ管理  

            m_typCoilInf = typCoilInf       ' コイル情報
            m_bNagate = bNagate             ' 実績画面長手方向反転

            m_dctTrendInf = New Dictionary(Of EM_COIL_TREND, TYP_TREND_INF)
            m_dctCalc = New Dictionary(Of EM_COIL_TREND, TYP_TREND_CALC)
            m_bSetEnd = False

        Catch ex As Exception
            log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("疵マップ画面 作成異常 [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' フォーム終了処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmCoilTrand_Closing(ByVal sender As Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles MyBase.Closing

        Dim ii As Integer = 0
        'iniファイル情報書込

        For ii = 0 To m_typTrendDsp.Count - 1
            PutTrendDsp(m_typTrendDsp(ii))
        Next ii

        For ii = 0 To m_typTrendDsp.Count - 1
            'グラフ名報書込
            tMod.WriteIni(SO_GAMENN, "COILTRANDNAME_" & CStr(ii + 1), LOCALINI_NAME, m_typTrendDsp(ii).cmbName.SelectedValue.ToString)
        Next ii

    End Sub

    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmCoilTrand_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        Dim s As System.Windows.Forms.Screen = System.Windows.Forms.Screen.FromControl(Me.Owner)
        Me.Location = s.Bounds.Location

        If NOA_ONLINE <> g_NOA Then                                         ' デバッグ時
            ' フォーム状態
            Me.tProp_FromFormatMode = gem_FromFormatMode.iOFFLINE
            Me.tProp_EndButtonEnable = False
            Me.FormBorderStyle = Windows.Forms.FormBorderStyle.Sizable      ' フォームサイズ変更可能
        End If

        'トレンド情報初期設定
        TrandInfInit()

        'タイトル設定
        SetTitleName(Me)

        'ポップアップ画面 低解像度対応
        Call SetPcHeight(Me)
        Call SetPcWidth(Me)

        '''' 画面初期設定
        SetInit()

        ''''' コイル情報表示
        dgvCoilInit()
        DspCoil()

        For ii As Integer = 0 To m_typTrendDsp.Count - 1
            ' コンボボックス選択イベントを設定
            AddHandler m_typTrendDsp(ii).cmbName.SelectedIndexChanged, AddressOf cmbName_SelectedIndexChanged
        Next ii

        ' DB読み込み
        m_lstData = New List(Of TYP_TREND_DATA)
        GetCoilTrandData()

        m_dctCalc.Clear()
        GetCalc()

        AddHandler dgvCoil.Resize, AddressOf Ctrl_ReSize

        'ポップアップ画面 低解像度対応(スクロールバーがあるとコイル情報が見切れる）
        dgvWidthResize(dgvCoil)
    End Sub


    ''' <summary>
    ''' この画面の資源解放イベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmCoilTrand_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
        Try
        Catch ex As Exception
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("コイルトレンド画面 資源解放異常 [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 動作定義
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Private Sub DgvInit(ByVal dgv As DataGridView)

        With dgv
            .ColumnCount = 0
            .AutoGenerateColumns = False                                    ' 列自動生成 (falseにするとDataSourceが効かなくなるので注意)
            .AllowUserToAddRows = False                                     ' ユーザー 行追加
            .AllowUserToDeleteRows = False                                  ' ユーザー 行削除
            .AllowUserToOrderColumns = False                                ' ユーザー 列移動
            .AllowUserToResizeColumns = False                               ' ユーザー 列サイズ変更
            .AllowUserToResizeRows = False                                  ' ユーザー 行サイズ変更
            .ShowCellErrors = False                                         ' セルエラーの表示
            .ShowCellToolTips = False                                       ' ツールヒントの表示
            .ShowEditingIcon = False                                        ' 編集中のセルの明示
            .ShowRowErrors = False                                          ' エラー行の明示
            .ScrollBars = ScrollBars.Both                                   ' スクロールバーの表示
            .ColumnHeadersHeight = 42                                       ' ヘッダーの高さ

            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' セル選択
            .MultiSelect = False                                            ' 複数行選択
            .ReadOnly = False                                                ' 読み取り専用はダメ
            .EditMode = DataGridViewEditMode.EditOnEnter                    ' セル書き換え

            .ColumnHeadersVisible = False                                   ' 列ヘッダ表示
            .RowHeadersVisible = False                                      ' 行ヘッダ表示
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' 可視列サイズ自動調整
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' 可視行サイズ自動調整
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

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

        End With

    End Sub

    ''' <summary>
    ''' 共通dgv初期化
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Private Sub dgvHanreiInit(ByVal dgv As DataGridView)


        ' 動作定義
        DgvInit(dgv)

        With dgv
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
                clm.Width = dgv.Width - 70 - 3
                clm.Frozen = False
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                .Columns.Add(clm)
            End Using

        End With

    End Sub

    Private Sub TrandInfInit()

        Dim typ As TYP_TREND_INF = Nothing
        m_dctTrendInf.Clear()

        For em As EM_COIL_TREND = EM_COIL_TREND.SPEED To EM_COIL_TREND.MAX
            With typ
                .initialize()
                Select Case em
                    Case EM_COIL_TREND.SPEED
                        .strGroupName = "ライン速度[mpm]"
                        .nMin = 0
                        .nMax = 1000
                        .nDecPlaces = 0
                        .lstItem.Add("表")
                        .lstItem.Add("裏")
                    Case EM_COIL_TREND.WIDTH
                        .strGroupName = "板幅[mm]"
                        .nMin = 0
                        .nMax = COIL_WID_MAX
                        .nDecPlaces = 0
                        .lstItem.Add("表 板幅Min")
                        .lstItem.Add("表 板幅Max")
                        .lstItem.Add("裏 板幅Min")
                        .lstItem.Add("裏 板幅Max")
                    Case EM_COIL_TREND.CAM_KIDO_O
                        .strGroupName = "カメラ輝度[輝度] (表)"
                        .nMin = 0
                        .nMax = 255
                        .nDecPlaces = 0
                        .lstItem.Add("カメラ1_R")
                        .lstItem.Add("カメラ1_G")
                        .lstItem.Add("カメラ1_B")
                        .lstItem.Add("カメラ2_R")
                        .lstItem.Add("カメラ2_G")
                        .lstItem.Add("カメラ2_B")
                        .lstItem.Add("カメラ3_R")
                        .lstItem.Add("カメラ3_G")
                        .lstItem.Add("カメラ3_B")
                        .lstItem.Add("カメラ4_R")
                        .lstItem.Add("カメラ4_G")
                        .lstItem.Add("カメラ4_B")
                        .lstItem.Add("カメラ5_R")
                        .lstItem.Add("カメラ5_G")
                        .lstItem.Add("カメラ5_B")
                        .lstItem.Add("カメラ6_R")
                        .lstItem.Add("カメラ6_G")
                        .lstItem.Add("カメラ6_B")
                        .lstItem.Add("カメラ7_R")
                        .lstItem.Add("カメラ7_G")
                        .lstItem.Add("カメラ7_B")
                        .lstItem.Add("カメラ8_R")
                        .lstItem.Add("カメラ8_G")
                        .lstItem.Add("カメラ8_B")
                        .lstItem.Add("カメラ9_R")
                        .lstItem.Add("カメラ9_G")
                        .lstItem.Add("カメラ9_B")
                        .lstItem.Add("カメラ10_R")
                        .lstItem.Add("カメラ10_G")
                        .lstItem.Add("カメラ10_B")
                    Case EM_COIL_TREND.CAM_KIDO_U
                        .strGroupName = "カメラ輝度[輝度] (裏)"
                        .nMin = 0
                        .nMax = 255
                        .nDecPlaces = 0
                        .lstItem.Add("カメラ1_R")
                        .lstItem.Add("カメラ1_G")
                        .lstItem.Add("カメラ1_B")
                        .lstItem.Add("カメラ2_R")
                        .lstItem.Add("カメラ2_G")
                        .lstItem.Add("カメラ2_B")
                        .lstItem.Add("カメラ3_R")
                        .lstItem.Add("カメラ3_G")
                        .lstItem.Add("カメラ3_B")
                        .lstItem.Add("カメラ4_R")
                        .lstItem.Add("カメラ4_G")
                        .lstItem.Add("カメラ4_B")
                        .lstItem.Add("カメラ5_R")
                        .lstItem.Add("カメラ5_G")
                        .lstItem.Add("カメラ5_B")
                        .lstItem.Add("カメラ6_R")
                        .lstItem.Add("カメラ6_G")
                        .lstItem.Add("カメラ6_B")
                        .lstItem.Add("カメラ7_R")
                        .lstItem.Add("カメラ7_G")
                        .lstItem.Add("カメラ7_B")
                        .lstItem.Add("カメラ8_R")
                        .lstItem.Add("カメラ8_G")
                        .lstItem.Add("カメラ8_B")
                        .lstItem.Add("カメラ9_R")
                        .lstItem.Add("カメラ9_G")
                        .lstItem.Add("カメラ9_B")
                        .lstItem.Add("カメラ10_R")
                        .lstItem.Add("カメラ10_G")
                        .lstItem.Add("カメラ10_B")
                    Case EM_COIL_TREND.CAM_GAIN_O
                        .strGroupName = "カメラゲイン[倍] (表)"
                        .nMin = 0
                        .nMax = 10
                        .nDecPlaces = 2
                        .lstItem.Add("カメラ1_R")
                        .lstItem.Add("カメラ1_G")
                        .lstItem.Add("カメラ1_B")
                        .lstItem.Add("カメラ2_R")
                        .lstItem.Add("カメラ2_G")
                        .lstItem.Add("カメラ2_B")
                        .lstItem.Add("カメラ3_R")
                        .lstItem.Add("カメラ3_G")
                        .lstItem.Add("カメラ3_B")
                        .lstItem.Add("カメラ4_R")
                        .lstItem.Add("カメラ4_G")
                        .lstItem.Add("カメラ4_B")
                        .lstItem.Add("カメラ5_R")
                        .lstItem.Add("カメラ5_G")
                        .lstItem.Add("カメラ5_B")
                        .lstItem.Add("カメラ6_R")
                        .lstItem.Add("カメラ6_G")
                        .lstItem.Add("カメラ6_B")
                        .lstItem.Add("カメラ7_R")
                        .lstItem.Add("カメラ7_G")
                        .lstItem.Add("カメラ7_B")
                        .lstItem.Add("カメラ8_R")
                        .lstItem.Add("カメラ8_G")
                        .lstItem.Add("カメラ8_B")
                        .lstItem.Add("カメラ9_R")
                        .lstItem.Add("カメラ9_G")
                        .lstItem.Add("カメラ9_B")
                        .lstItem.Add("カメラ10_R")
                        .lstItem.Add("カメラ10_G")
                        .lstItem.Add("カメラ10_B")
                    Case EM_COIL_TREND.CAM_GAIN_U
                        .strGroupName = "カメラゲイン[倍] (裏)"
                        .nMin = 0
                        .nMax = 10
                        .nDecPlaces = 2
                        .lstItem.Add("カメラ1_R")
                        .lstItem.Add("カメラ1_G")
                        .lstItem.Add("カメラ1_B")
                        .lstItem.Add("カメラ2_R")
                        .lstItem.Add("カメラ2_G")
                        .lstItem.Add("カメラ2_B")
                        .lstItem.Add("カメラ3_R")
                        .lstItem.Add("カメラ3_G")
                        .lstItem.Add("カメラ3_B")
                        .lstItem.Add("カメラ4_R")
                        .lstItem.Add("カメラ4_G")
                        .lstItem.Add("カメラ4_B")
                        .lstItem.Add("カメラ5_R")
                        .lstItem.Add("カメラ5_G")
                        .lstItem.Add("カメラ5_B")
                        .lstItem.Add("カメラ6_R")
                        .lstItem.Add("カメラ6_G")
                        .lstItem.Add("カメラ6_B")
                        .lstItem.Add("カメラ7_R")
                        .lstItem.Add("カメラ7_G")
                        .lstItem.Add("カメラ7_B")
                        .lstItem.Add("カメラ8_R")
                        .lstItem.Add("カメラ8_G")
                        .lstItem.Add("カメラ8_B")
                        .lstItem.Add("カメラ9_R")
                        .lstItem.Add("カメラ9_G")
                        .lstItem.Add("カメラ9_B")
                        .lstItem.Add("カメラ10_R")
                        .lstItem.Add("カメラ10_G")
                        .lstItem.Add("カメラ10_B")
                    Case EM_COIL_TREND.CAM_TIME_O
                        .strGroupName = "露光時間[25ns] (表)"
                        .nMin = 320
                        .nMax = 120000
                        .nDecPlaces = 0
                        .lstItem.Add("カメラ1_R")
                        .lstItem.Add("カメラ1_G")
                        .lstItem.Add("カメラ1_B")
                        .lstItem.Add("カメラ2_R")
                        .lstItem.Add("カメラ2_G")
                        .lstItem.Add("カメラ2_B")
                        .lstItem.Add("カメラ3_R")
                        .lstItem.Add("カメラ3_G")
                        .lstItem.Add("カメラ3_B")
                        .lstItem.Add("カメラ4_R")
                        .lstItem.Add("カメラ4_G")
                        .lstItem.Add("カメラ4_B")
                        .lstItem.Add("カメラ5_R")
                        .lstItem.Add("カメラ5_G")
                        .lstItem.Add("カメラ5_B")
                        .lstItem.Add("カメラ6_R")
                        .lstItem.Add("カメラ6_G")
                        .lstItem.Add("カメラ6_B")
                        .lstItem.Add("カメラ7_R")
                        .lstItem.Add("カメラ7_G")
                        .lstItem.Add("カメラ7_B")
                        .lstItem.Add("カメラ8_R")
                        .lstItem.Add("カメラ8_G")
                        .lstItem.Add("カメラ8_B")
                        .lstItem.Add("カメラ9_R")
                        .lstItem.Add("カメラ9_G")
                        .lstItem.Add("カメラ9_B")
                        .lstItem.Add("カメラ10_R")
                        .lstItem.Add("カメラ10_G")
                        .lstItem.Add("カメラ10_B")
                    Case EM_COIL_TREND.CAM_TIME_U
                        .strGroupName = "露光時間[25ns] (裏)"
                        .nMin = 320
                        .nMax = 120000
                        .nDecPlaces = 0
                        .lstItem.Add("カメラ1_R")
                        .lstItem.Add("カメラ1_G")
                        .lstItem.Add("カメラ1_B")
                        .lstItem.Add("カメラ2_R")
                        .lstItem.Add("カメラ2_G")
                        .lstItem.Add("カメラ2_B")
                        .lstItem.Add("カメラ3_R")
                        .lstItem.Add("カメラ3_G")
                        .lstItem.Add("カメラ3_B")
                        .lstItem.Add("カメラ4_R")
                        .lstItem.Add("カメラ4_G")
                        .lstItem.Add("カメラ4_B")
                        .lstItem.Add("カメラ5_R")
                        .lstItem.Add("カメラ5_G")
                        .lstItem.Add("カメラ5_B")
                        .lstItem.Add("カメラ6_R")
                        .lstItem.Add("カメラ6_G")
                        .lstItem.Add("カメラ6_B")
                        .lstItem.Add("カメラ7_R")
                        .lstItem.Add("カメラ7_G")
                        .lstItem.Add("カメラ7_B")
                        .lstItem.Add("カメラ8_R")
                        .lstItem.Add("カメラ8_G")
                        .lstItem.Add("カメラ8_B")
                        .lstItem.Add("カメラ9_R")
                        .lstItem.Add("カメラ9_G")
                        .lstItem.Add("カメラ9_B")
                        .lstItem.Add("カメラ10_R")
                        .lstItem.Add("カメラ10_G")
                        .lstItem.Add("カメラ10_B")
                    Case EM_COIL_TREND.WALK
                        .strGroupName = "ウォーク量[m]"
                        .nMin = -100
                        .nMax = 100
                        .nDecPlaces = 0
                        .lstItem.Add("表")
                        .lstItem.Add("裏")
                    Case EM_COIL_TREND.LED_O
                        .strGroupName = "照明装置_調光値[%] (表)"
                        .nMin = 0
                        .nMax = 100
                        .nDecPlaces = 0
                        .lstItem.Add("照明装置_調光値_1")
                        .lstItem.Add("照明装置_調光値_2")
                        .lstItem.Add("照明装置_調光値_3")
                        .lstItem.Add("照明装置_調光値_4")
                        .lstItem.Add("照明装置_調光値_5")
                    Case EM_COIL_TREND.LED_U
                        .strGroupName = "照明装置_調光値[%] (裏)"
                        .nMin = 0
                        .nMax = 100
                        .nDecPlaces = 0
                        .lstItem.Add("照明装置_調光値_1")
                        .lstItem.Add("照明装置_調光値_2")
                        .lstItem.Add("照明装置_調光値_3")
                        .lstItem.Add("照明装置_調光値_4")
                        .lstItem.Add("照明装置_調光値_5")
                    Case Else
                        Continue For
                End Select
            End With
            m_dctTrendInf.Add(em, typ)
        Next
    End Sub

    ''' <summary>
    ''' 画面初期設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetInit()

        'グリッドのメンバー変数設定
        m_typTrendDsp(0).dgvHanrei = dgvHanrei1
        m_typTrendDsp(1).dgvHanrei = dgvHanrei2
        m_typTrendDsp(2).dgvHanrei = dgvHanrei3
        m_typTrendDsp(3).dgvHanrei = dgvHanrei4
        m_typTrendDsp(4).dgvHanrei = dgvHanrei5

        '下限値のメンバー変数設定
        m_typTrendDsp(0).nudGraphMin = nudGraphMin1
        m_typTrendDsp(1).nudGraphMin = nudGraphMin2
        m_typTrendDsp(2).nudGraphMin = nudGraphMin3
        m_typTrendDsp(3).nudGraphMin = nudGraphMin4
        m_typTrendDsp(4).nudGraphMin = nudGraphMin5

        '上限値のメンバー変数設定
        m_typTrendDsp(0).nudGraphMax = nudGraphMax1
        m_typTrendDsp(1).nudGraphMax = nudGraphMax2
        m_typTrendDsp(2).nudGraphMax = nudGraphMax3
        m_typTrendDsp(3).nudGraphMax = nudGraphMax4
        m_typTrendDsp(4).nudGraphMax = nudGraphMax5

        'ピクチャーのメンバー変数設定
        m_typTrendDsp(0).picGraph = picGraph1
        m_typTrendDsp(1).picGraph = picGraph2
        m_typTrendDsp(2).picGraph = picGraph3
        m_typTrendDsp(3).picGraph = picGraph4
        m_typTrendDsp(4).picGraph = picGraph5

        'グラフプロットのメンバー変数設定
        m_typTrendDsp(0).chkGraphProt = chkGraphProt1
        m_typTrendDsp(1).chkGraphProt = chkGraphProt2
        m_typTrendDsp(2).chkGraphProt = chkGraphProt3
        m_typTrendDsp(3).chkGraphProt = chkGraphProt4
        m_typTrendDsp(4).chkGraphProt = chkGraphProt5

        'グラフ名コンボBOXのメンバー変数設定
        m_typTrendDsp(0).cmbName = cmbName1
        m_typTrendDsp(1).cmbName = cmbName2
        m_typTrendDsp(2).cmbName = cmbName3
        m_typTrendDsp(3).cmbName = cmbName4
        m_typTrendDsp(4).cmbName = cmbName5

        ''最大値/最小値/平均値表示DGVのメンバー変数設定
        m_typTrendDsp(0).dgvCalc = dgvCalc1
        m_typTrendDsp(1).dgvCalc = dgvCalc2
        m_typTrendDsp(2).dgvCalc = dgvCalc3
        m_typTrendDsp(3).dgvCalc = dgvCalc4
        m_typTrendDsp(4).dgvCalc = dgvCalc5

        'マップのメンバー変数設定
        For ii As Integer = 0 To m_typTrendDsp.Count - 1
            With m_typTrendDsp(ii)

                'マップ初期定義
                .clsGraph = New tClass.tClass_MapBase
                MapInitialize(.clsGraph)

                'コンボボックス設定
                .emType = CType(tMod.ReadIniInt(SO_GAMENN, "COILTRANDNAME_" & CStr(ii + 1), LOCALINI_NAME, ii), EM_COIL_TREND)
                cmbName_Init(.cmbName, .emType)

                'グリッドの初期設定
                dgvHanreiInit(.dgvHanrei)
                dgvCalcInit(.dgvCalc)

                'イベントハンドラ関連付け
                ' マップ
                AddHandler .clsGraph.tEv_DrawPaint_End, AddressOf tEv_DrawPaint_End

                ' データグリッドビュー
                AddHandler .dgvHanrei.CellContentClick, AddressOf dgvGraph_CellContentClick
                AddHandler .dgvHanrei.SelectionChanged, AddressOf dgvGraph_SelectionChanged

                ' ピクチャーボックス
                AddHandler .picGraph.Paint, AddressOf PictureBox_Paint

                ' チェックボックス
                AddHandler .chkGraphProt.CheckedChanged, AddressOf chkGraphProt_CheckedChanged

                ' スピン ボックス (アップダウン コントロール)
                AddHandler .nudGraphMin.ValueChanged, AddressOf nudGraphMaxMin_ValueChanged
                AddHandler .nudGraphMax.ValueChanged, AddressOf nudGraphMaxMin_ValueChanged
            End With
        Next

    End Sub

    ''' <summary>
    ''' マップ基本情報定義
    ''' </summary>
    ''' <param name="MapBase">グラフマップ</param>
    ''' <remarks></remarks>
    Private Sub MapInitialize(ByVal MapBase As tClass.tClass_MapBase)
        '''' グラフ描画に必要な情報を設定
        With MapBase
            '' 表示領域情報
            .tProp_AriaInf_BackGroundLineColor = Pens.Black
            .tProp_AriaInf_BackGroundColor = New SolidBrush(Color.White)
            .tProp_AriaInf_MapColor = New SolidBrush(Color.White)
            .tProp_AriaInf_AriaSize = picGraph1.Size
            .tProp_AriaInf_Margin = New Padding(50, 10, 15, 25)

            '' マップ領域
            .tProp_MapInf_IsCenterX = False
            .tProp_MapInf_IsCenterY = False
            .tProp_MapInf_IsBaseX_L = True
            .tProp_MapInf_IsBaseY_T = False
            .tProp_MapInf_IsStrechX = True
            .tProp_MapInf_IsStrechY = True
            .tProp_MapInf_LimitXmin = 0
            .tProp_MapInf_LimitXmax = COIL_LEN_MAX * 1000
            .tProp_MapInf_LimitYmin = 0
            .tProp_MapInf_LimitYmax = 1000
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

            .tProp_MemoriInf_ScaleInterval = New tClass.tClass_MapBase.xyValue(50 * 1000, 200)
            .tProp_MemoriInf_ScaleStringFormatX = "###0"
            .tProp_MemoriInf_ScaleStringFormatY = "0"
            .tProp_MemoriInf_ScaleStringRange = New tClass.tClass_MapBase.RATE(1000, 1)
            .tProp_MemoriInf_StringDisp_T = False
            .tProp_MemoriInf_StringDisp_B = False
            .tProp_MemoriInf_StringDisp_L = False
            .tProp_MemoriInf_StringDisp_R = False
            .tProp_MemoriInf_LineDsp_X = False
            .tProp_MemoriInf_LineDsp_Y = False
        End With
    End Sub
    ''' <summary>
    ''' 画面データ描画 (設定変更時にイベント実行) (画面表示時にも)
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetDsp(ByRef typDsp As TYP_TREND_DSP)

        Try

            ' 入力データチェック
            If False = InputDataCheck(typDsp) Then
                Exit Try
            End If

            ' 上下限保持
            typDsp.nMax = CInt(typDsp.nudGraphMax.Value)
            typDsp.nMin = CInt(typDsp.nudGraphMin.Value)

            '''' プロットデータをセット
            GetProtData(typDsp)

            '''' 最小、最大、平均データをセット
            DspCalcData(typDsp)

            '''' マップ領域再定義
            MapReSetring(typDsp)

            '''' マップ再生成依頼
            typDsp.clsGraph.DrawPaint()

            '''' マップ再描画
            typDsp.picGraph.Refresh()

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("画面データ描画 例外[{0}]", ex.Message))
        End Try


    End Sub

    ''' <summary>
    ''' 入力データチェック
    ''' </summary>
    ''' <param name="typDsp">グラフグループ番号(０オリジン)</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function InputDataCheck(ByRef typDsp As TYP_TREND_DSP) As Boolean
        Dim strMin As String        ' dgvのデータ格納用
        Dim strMax As String        ' dgvのデータ格納用


        '上下限入力値チェック
        strMin = CStr(typDsp.nudGraphMin.Value)
        strMax = CStr(typDsp.nudGraphMax.Value)

        'NumericUpDownで異常時にメッセージを出すと落ちるため、色替え処理のみとする。
        If False = tClass.tClass_ImputCheck.CheckMinMax(strMin, strMax, 0, False, typDsp.cmbName.Text, False) Then
            typDsp.nudGraphMin.BackColor = Color.Red
            typDsp.nudGraphMax.BackColor = Color.Red
            Return False
        Else
            typDsp.nudGraphMin.BackColor = Color.White
            typDsp.nudGraphMax.BackColor = Color.White
        End If


        Return True
    End Function

    ''' <summary>
    ''' マップ再定義
    ''' </summary>
    ''' <param name="typDsp">グラフ表示テーブル</param>
    ''' <remarks></remarks>
    Private Sub MapReSetring(ByVal typDsp As TYP_TREND_DSP)

        '''' グラフ描画に必要な情報を設定
        With typDsp.clsGraph
            '' マップ領域
            .tProp_MapInf_LimitXmax = m_typCoilInf.nCoilLen
            .tProp_MapInf_LimitYmin = CInt(typDsp.nMin)
            .tProp_MapInf_LimitYmax = CInt(typDsp.nMax)

        End With
    End Sub

    ''' <summary>
    ''' dgvのセルのボタンが押されたときの処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvGraph_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' 行がクリックされていなければ終了
        If 0 > e.RowIndex Then Exit Sub

        ' 列インデックスでどの列がクリックされたか判別
        Select Case e.ColumnIndex
            Case em_DataColumnType.iColor  '表示色
                GetDlgColor(dgv, e.ColumnIndex, e.RowIndex)
            Case em_DataColumnType.iCheck
                dgv(e.ColumnIndex, e.RowIndex).Value = dgv(e.ColumnIndex, e.RowIndex).EditedFormattedValue
            Case em_DataColumnType.iName
        End Select

        For ii As Integer = 0 To m_typTrendDsp.Count - 1
            ' グラフグループの検索
            If sender Is m_typTrendDsp(ii).dgvHanrei Then
                If 0 < m_typTrendDsp(ii).nCnt Then
                    ''''' マップ再生成依頼
                    m_typTrendDsp(ii).clsGraph.DrawPaint()

                    ''''' マップ再描画
                    m_typTrendDsp(ii).picGraph.Refresh()

                    Exit For
                End If
            End If

        Next ii

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

    ''' <summary>
    ''' マップ表示色ボタンが押されたら、色選択ダイアログを表示し、ボタンの色を変更
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
    ''' 描画
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' </summary>
    Private Sub PictureBox_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs)
        For ii As Integer = 0 To m_typTrendDsp.Count - 1
            ' グラフグループの検索
            If sender Is m_typTrendDsp(ii).picGraph Then
                '''' ビットマップ取得
                Dim bmp As Bitmap = m_typTrendDsp(ii).clsGraph.tProp_Bmp
                If bmp Is Nothing Then Return

                '''' 背景をセット
                e.Graphics.DrawImage(bmp, 0, 0)
            End If
        Next ii

    End Sub

    ''' <summary>
    ''' 画面データ描画
    ''' </summary>
    ''' <param name="cls"></param>
    ''' <param name="g"></param>
    ''' <remarks></remarks>
    Private Sub tEv_DrawPaint_End(ByVal cls As tClassLibrary.tClass.tClass_MapBase, ByVal g As System.Drawing.Graphics)
        Dim ii As Integer                   ' INDEX
        Dim jj As Integer                   ' INDEX
        Dim nPixel As Integer               ' 凡例X軸位置
        Dim nGraphXInterval As Integer      ' X軸表示周期
        Dim nGraphXPoint As Integer         ' X軸表示ポインタ
        Dim nGraphYInterval As Integer      ' Y軸表示周期
        Dim nGraphYPoint As Integer         ' Y軸表示ポインタ
        Dim strWork As String               ' ワークエリア
        Try

            ' グラフグループの検索
            For Each typ As TYP_TREND_DSP In m_typTrendDsp
                If cls IsNot typ.clsGraph Then
                    Continue For
                End If

                'If Not m_dctTrendInf.ContainsKey(typ.emType) Then
                '    Exit Try
                'End If
                'Dim inf As TYP_TREND_INF = m_dctTrendInf(typ.emType)

                ' '' X軸凡例表示
                '' X軸スケール間隔算出
                Dim dLen As Double = m_typCoilInf.nCoilLen / 1000
                If dLen <= 200 Then                                         ' コイル長 200m以下なら20m間隔
                    nGraphXInterval = 20
                ElseIf dLen <= 1000 Then                                    ' コイル長 1000m以下なら100m間隔
                    nGraphXInterval = 100
                ElseIf dLen <= 3000 Then                                    ' コイル長 3000m以下なら200m間隔
                    nGraphXInterval = 200
                ElseIf dLen <= 6000 Then                                    ' コイル長 6000m以下なら500m間隔
                    nGraphXInterval = 500
                ElseIf dLen <= 10000 Then                                   ' コイル長 10000m以下なら1000m間隔
                    nGraphXInterval = 1000
                Else                                                        ' コイル長 10000m越えなら2000m間隔
                    nGraphXInterval = 2000
                End If
                nGraphXInterval *= 1000                                     ' メートル -> ミリメート変換

                nGraphXPoint = 1
                For jj = 1 To m_typCoilInf.nCoilLen

                    If jj = nGraphXPoint Or jj = m_typCoilInf.nCoilLen Then

                        ' Xポジション取得
                        nPixel = cls.GetVtoP_X(jj)

                        ' 最初と最後は縦線を表示しない
                        If 1 <> jj AndAlso m_typCoilInf.nCoilLen <> jj Then
                            '線
                            g.DrawLine(cls.tProp_MemoriInf.LinePen, nPixel, cls.tProp_CalcAns.MapRect.Top + 1, nPixel, cls.tProp_CalcAns.MapRect.Bottom)

                        End If

                        ' 文字
                        strWork = CStr(jj \ 1000) & "m"
                        cls.DrawString(g, nPixel, cls.tProp_CalcAns.MapRect.Bottom, 0, +cls.tProp_MemoriInf.nPosStringX, strWork, ContentAlignment.BottomCenter, cls.tProp_MemoriInf.StringFont, cls.tProp_MemoriInf.StringBsh)
                        nGraphXPoint += nGraphXInterval
                    End If

                Next jj

                '' Y軸凡例表示
                ' Y軸スケール間隔算出
                nGraphYInterval = ((typ.nMax - typ.nMin) - 1) \ mc_YScale
                If 0 <> ((typ.nMax - typ.nMin) - 1) Mod mc_YScale Then
                    nGraphYInterval += 1
                End If
                nGraphYPoint = typ.nMin
                For jj = typ.nMin To typ.nMax
                    If jj = nGraphYPoint Or jj = typ.nMax Then
                        ' Yポジション取得
                        nPixel = cls.GetVtoP_Y(jj)

                        ' 最初と最後は横線を表示しない
                        If typ.nMin <> jj AndAlso typ.nMax <> jj Then
                            '線
                            g.DrawLine(cls.tProp_MemoriInf.LinePen, cls.tProp_CalcAns.MapRect.Left + 1, nPixel, cls.tProp_CalcAns.MapRect.Right, nPixel)

                        End If

                        ' 文字
                        strWork = Format(jj, cls.tProp_MemoriInf.sScaleStringFormatY)
                        cls.DrawString(g, cls.tProp_CalcAns.MapRect.Left, nPixel, -cls.tProp_MemoriInf.nPosStringY, 0, strWork, ContentAlignment.MiddleLeft, cls.tProp_MemoriInf.StringFont, cls.tProp_MemoriInf.StringBsh)
                        nGraphYPoint += nGraphYInterval
                    End If

                Next jj

                ' グラフ表示データ=0で終了
                If 0 = typ.nCnt Then Return

                '''' 描画
                Dim dVal As Single
                Dim nAddr As Integer = -1
                Dim x1, y1, x2, y2 As Integer
                For ii = 0 To typ.dgvHanrei.RowCount - 1
                    ' グラフ表示チェック 非表示で次のグラフ（線）
                    If False = CBool(typ.dgvHanrei(em_DataColumnType.iCheck, ii).Value) Then Continue For

                    For jj = 0 To typ.nCnt - 1
                        dVal = typ.nData(ii, jj)
                        If NON_TRAND_DATA = dVal Then
                            Continue For
                        Else
                            nAddr = jj
                            Exit For
                        End If
                    Next
                    If -1 = nAddr Then
                        Continue For
                    End If
                    ' 始点算出 (先頭の座標算出) X軸
                    x1 = cls.GetVtoP_X(typ.nCoilPos(nAddr) + 1)

                    ' 始点算出 (先頭の座標算出) Y軸
                    dVal = typ.nData(ii, nAddr)
                    If dVal > typ.nMax Then dVal = typ.nMax
                    If dVal < typ.nMin Then dVal = typ.nMin
                    y1 = cls.GetVtoP_Y(dVal)

                    Using p As New Pen(typ.dgvHanrei(em_DataColumnType.iColor, ii).Style.BackColor, 1)

                        ' プロットの指定
                        If typ.chkGraphProt.Checked Then
                            g.FillEllipse(New SolidBrush(p.Color), x1 - 3, y1 - 3, 6, 6)       ' ●
                        End If

                        For jj = 0 To typ.nCnt - 1

                            ' 終点算出 X軸
                            x2 = cls.GetVtoP_X(typ.nCoilPos(jj) + 1)

                            ' 終点算出 Y軸
                            dVal = typ.nData(ii, jj)
                            If dVal = NON_TRAND_DATA Then Continue For
                            If dVal > typ.nMax Then dVal = typ.nMax
                            If dVal < typ.nMin Then dVal = typ.nMin
                            y2 = cls.GetVtoP_Y(dVal)

                            ' 描画
                            g.DrawLine(p, x1, y1, x2, y2)

                            ' プロットの指定
                            If typ.chkGraphProt.Checked Then
                                g.FillEllipse(New SolidBrush(p.Color), x2 - 3, y2 - 3, 6, 6)       ' ●
                            End If

                            ' 次の準備
                            x1 = x2
                            y1 = y2

                        Next jj

                    End Using
                Next
            Next

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("画面データ描画1 例外[{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' プロット表示チェック状態変更
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chkGraphProt_CheckedChanged(ByVal sender As Object, ByVal e As System.EventArgs)
        For ii As Integer = 0 To m_typTrendDsp.Count - 1
            ' グラフグループの検索
            If sender Is m_typTrendDsp(ii).chkGraphProt Then
                If 0 < m_typTrendDsp(ii).nCnt Then
                    '''' マップ再生成依頼
                    m_typTrendDsp(ii).clsGraph.DrawPaint()

                    '''' マップ再描画
                    m_typTrendDsp(ii).picGraph.Refresh()

                    Exit For
                End If
            End If
        Next ii
    End Sub



    ''' <summary>
    ''' フォーム表示
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmCoilTrand_Shown(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Shown
        Try

            '''' 画面初期化
            For ii As Integer = 0 To m_typTrendDsp.Count - 1
                DspInit(m_typTrendDsp(ii))
            Next ii

            m_bSetEnd = True

            For ii As Integer = 0 To m_typTrendDsp.Count - 1
                SetDsp(m_typTrendDsp(ii))
            Next ii

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("フォーム表示 例外[{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' 画面初期表示
    ''' </summary>
    ''' <param name="typDsp">グラフ表示テーブル</param>
    ''' <remarks></remarks>
    Private Sub DspInit(ByRef typDsp As TYP_TREND_DSP)
        Dim ii As Integer                       ' INDEX

        With typDsp

            If Not m_dctTrendInf.ContainsKey(.emType) Then
                '存在しない？
                Exit Sub
            End If

            Dim inf As TYP_TREND_INF = m_dctTrendInf(.emType)

            ' 凡例内容表示
            .dgvHanrei.RowCount = 0

            For ii = 0 To inf.lstItem.Count - 1
                .dgvHanrei.RowCount = .dgvHanrei.RowCount + 1
                ' 表示・非表示
                Dim nDisp As Integer = tMod.ReadIniInt(SO_GAMENN, "COILTRANDDISP_" & CStr(.emType) & "_" & CStr(ii + 1), LOCALINI_NAME, 1)
                If 1 = nDisp Or 0 = nDisp Then
                    .dgvHanrei(em_DataColumnType.iCheck, ii).Value = nDisp
                Else
                    .dgvHanrei(em_DataColumnType.iCheck, ii).Value = 1
                End If

                ' 表示色
                Dim strColor As String = tMod.ReadIniStr(SO_GAMENN, "COILTRANDCOLOR_" & CStr(.emType) & "_" & CStr(ii + 1), LOCALINI_NAME)
                If "" <> strColor Then
                    .dgvHanrei(em_DataColumnType.iColor, ii).Style.BackColor = Color.FromArgb(Convert.ToInt32("FF" & strColor, 16))
                Else
                    .dgvHanrei(em_DataColumnType.iColor, ii).Style.BackColor = Color.FromArgb(Convert.ToInt32(Hex(sdcDefault(ii).ToArgb), 16))
                End If

                ' 名称
                .dgvHanrei(em_DataColumnType.iName, ii).Value = inf.lstItem.Item(ii)
                'End If
            Next ii

            'ハンドラを一旦解除する
            RemoveHandler .nudGraphMin.ValueChanged, AddressOf nudGraphMaxMin_ValueChanged
            RemoveHandler .nudGraphMax.ValueChanged, AddressOf nudGraphMaxMin_ValueChanged

            .nudGraphMin.Maximum = inf.nMax
            .nudGraphMin.Minimum = inf.nMin
            .nudGraphMin.DecimalPlaces = inf.nDecPlaces
            If 0 < inf.nDecPlaces Then
                .nudGraphMin.Increment = CDec(1 / (10 ^ inf.nDecPlaces))
            Else
                .nudGraphMin.Increment = 1
            End If

            .nudGraphMax.Maximum = .nudGraphMin.Maximum
            .nudGraphMax.Minimum = .nudGraphMin.Minimum
            .nudGraphMax.DecimalPlaces = .nudGraphMin.DecimalPlaces
            .nudGraphMax.Increment = .nudGraphMin.Increment

            ' 上限値
            .nudGraphMax.Text = tMod.ReadIniStr(SO_GAMENN, "COILTRANDMAX_" & CStr(.emType), LOCALINI_NAME)
            If False = IsNumeric(.nudGraphMax.Text) Then
                .nudGraphMax.Text = inf.nMax.ToString
            End If

            ' 下限値
            .nudGraphMin.Text = tMod.ReadIniStr(SO_GAMENN, "COILTRANDMIN_" & CStr(.emType), LOCALINI_NAME)
            If False = IsNumeric(.nudGraphMin.Text) Then
                .nudGraphMin.Text = inf.nMin.ToString
            End If

            If False = tClass.tClass_ImputCheck.CheckMinMax(.nudGraphMin.Text, .nudGraphMax.Text, 0, False, .cmbName.Text, False) Then
                'NumericUpDownで異常の場合は、デフォルト設定にする
                .nudGraphMax.Text = inf.nMax.ToString
                .nudGraphMin.Text = inf.nMin.ToString
            End If

            'ハンドラ再設定
            AddHandler .nudGraphMin.ValueChanged, AddressOf nudGraphMaxMin_ValueChanged
            AddHandler .nudGraphMax.ValueChanged, AddressOf nudGraphMaxMin_ValueChanged

        End With

    End Sub

    ''' <summary>
    ''' トレンド設定保存(Iniファイル書込み)
    ''' </summary>
    ''' <param name="typDsp"></param>
    Private Sub PutTrendDsp(ByVal typDsp As TYP_TREND_DSP)
        Dim strWk As String     ' 文字列ワーク

        'iniファイル情報書込
        For ii As Integer = 0 To typDsp.dgvHanrei.RowCount - 1
            '表示・非表示情報書込
            If True = CBool(typDsp.dgvHanrei(em_DataColumnType.iCheck, ii).Value) Then
                tMod.WriteIni(SO_GAMENN, "COILTRANDDISP_" & CStr(typDsp.emType) & "_" & CStr(ii + 1), LOCALINI_NAME, "1")
            Else
                tMod.WriteIni(SO_GAMENN, "COILTRANDDISP_" & CStr(typDsp.emType) & "_" & CStr(ii + 1), LOCALINI_NAME, "0")
            End If
        Next

        For ii As Integer = 0 To typDsp.dgvHanrei.RowCount - 1
            '表示色情報書込
            strWk = Microsoft.VisualBasic.Right(Hex(typDsp.dgvHanrei(em_DataColumnType.iColor, ii).Style.BackColor.ToArgb), 6)
            tMod.WriteIni(SO_GAMENN, "COILTRANDCOLOR_" & CStr(typDsp.emType) & "_" & CStr(ii + 1), LOCALINI_NAME, strWk)
        Next

        '上下限入力値チェック
        Dim strMin As String = CStr(typDsp.nudGraphMin.Value)
        Dim strMax As String = CStr(typDsp.nudGraphMax.Value)

        If False = tClass.tClass_ImputCheck.CheckMinMax(strMin, strMax, 0, False, typDsp.cmbName.Text, False) Then
            'NumericUpDownで異常時の場合は、書込みを行わない
        Else
            'グラフ上限値書込
            tMod.WriteIni(SO_GAMENN, "COILTRANDMAX_" & CStr(typDsp.emType), LOCALINI_NAME, typDsp.nudGraphMax.Text)

            'グラフ下限値書込
            tMod.WriteIni(SO_GAMENN, "COILTRANDMIN_" & CStr(typDsp.emType), LOCALINI_NAME, typDsp.nudGraphMin.Text)
        End If
    End Sub

    '***************************************************************************
    ' キーイベント
    '***************************************************************************
    ''' <summary>
    ''' ファンクションキーイベント
    ''' </summary>
    ''' <param name="FncNo">ファンクションNo 0オリジン</param>
    ''' <param name="enable">ボタンコントロールの有効無効</param>
    ''' <param name="onKey"></param>
    ''' <remarks></remarks>
    Private Sub frmCoilTrand_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        Select Case FncNo
            Case 8                                  ' CSV出力
                CsvOutTrand(CSV_PATH, "コイルトレンド画面.csv")

            Case 9                                  ' 画面印刷
                PrintOut()

            Case 10                                 ' 画面保存
                HardCopy(IMG_PATH, "コイルトレンド画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)

            Case 11                                 ' メニュー画面表示
                MyBase.ExecFromClose()
        End Select
    End Sub

    ''' <summary>
    ''' 指定管理Noのコイルトレンド情報(COIL_TRAND)を読み込む
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub GetCoilTrandData()
        Dim strSQL As String                    ' SQL文
        Dim typData As TYP_TREND_DATA = Nothing

        '''' グラフデータ初期化
        m_lstData.Clear()

        '''' SQL接続準備
        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        If Not tcls_DB.Open() Then
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB接続エラー [" & DB_COIL_TRAND & "]")
            Exit Sub
        End If

        '''' SQL文生成
        strSQL = ""
        strSQL += " SELECT * "
        strSQL += " FROM " & DB_COIL_TRAND
        strSQL += " WHERE 管理No = '" & m_typCoilInf.strKanriNo & "'"
        strSQL += " ORDER BY 取得位置 ASC"

        '''' データ取得
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB読み込みエラー [" & DB_COIL_TRAND & "]")
            Exit Sub
        End If

        Try
            While sqlRead.Read()
                With sqlRead
                    typData.initialize()

                    typData.TorB = .GetByte(.GetOrdinal("表裏区分"))
                    typData.nPos = .GetInt32(.GetOrdinal("取得位置"))

                    If m_typCoilInf.bCorrect Then
                        '取得位置に補正率を乗算
                        typData.nPos = CInt(typData.nPos * m_typCoilInf.dCorrect)
                    End If

                    If m_bNagate = True Then
                        ' データの上下反転があればトップからの距離反転
                        typData.nPos = m_typCoilInf.nCoilLen - typData.nPos
                        If typData.nPos < 0 Then
                            typData.nPos = 0
                        End If
                    End If

                    'ライン速度
                    typData.nSpeed = .GetInt32(.GetOrdinal("ライン速度"))

                    '板幅
                    typData.dItahaba(0) = .GetFloat(.GetOrdinal("板幅Min"))
                    typData.dItahaba(1) = .GetFloat(.GetOrdinal("板幅Max"))

                    'カメラ輝度
                    typData.typCamInf(0).nKidoR = .GetByte(.GetOrdinal("カメラ1_輝度R_C"))
                    typData.typCamInf(0).nKidoG = .GetByte(.GetOrdinal("カメラ1_輝度G_C"))
                    typData.typCamInf(0).nKidoB = .GetByte(.GetOrdinal("カメラ1_輝度B_C"))
                    typData.typCamInf(1).nKidoR = .GetByte(.GetOrdinal("カメラ2_輝度R_C"))
                    typData.typCamInf(1).nKidoG = .GetByte(.GetOrdinal("カメラ2_輝度G_C"))
                    typData.typCamInf(1).nKidoB = .GetByte(.GetOrdinal("カメラ2_輝度B_C"))
                    typData.typCamInf(2).nKidoR = .GetByte(.GetOrdinal("カメラ3_輝度R_C"))
                    typData.typCamInf(2).nKidoG = .GetByte(.GetOrdinal("カメラ3_輝度G_C"))
                    typData.typCamInf(2).nKidoB = .GetByte(.GetOrdinal("カメラ3_輝度B_C"))
                    typData.typCamInf(3).nKidoR = .GetByte(.GetOrdinal("カメラ4_輝度R_C"))
                    typData.typCamInf(3).nKidoG = .GetByte(.GetOrdinal("カメラ4_輝度G_C"))
                    typData.typCamInf(3).nKidoB = .GetByte(.GetOrdinal("カメラ4_輝度B_C"))
                    typData.typCamInf(4).nKidoR = .GetByte(.GetOrdinal("カメラ5_輝度R_C"))
                    typData.typCamInf(4).nKidoG = .GetByte(.GetOrdinal("カメラ5_輝度G_C"))
                    typData.typCamInf(4).nKidoB = .GetByte(.GetOrdinal("カメラ5_輝度B_C"))
                    typData.typCamInf(5).nKidoR = .GetByte(.GetOrdinal("カメラ6_輝度R_C"))
                    typData.typCamInf(5).nKidoG = .GetByte(.GetOrdinal("カメラ6_輝度G_C"))
                    typData.typCamInf(5).nKidoB = .GetByte(.GetOrdinal("カメラ6_輝度B_C"))
                    typData.typCamInf(6).nKidoR = .GetByte(.GetOrdinal("カメラ7_輝度R_C"))
                    typData.typCamInf(6).nKidoG = .GetByte(.GetOrdinal("カメラ7_輝度G_C"))
                    typData.typCamInf(6).nKidoB = .GetByte(.GetOrdinal("カメラ7_輝度B_C"))
                    typData.typCamInf(7).nKidoR = .GetByte(.GetOrdinal("カメラ8_輝度R_C"))
                    typData.typCamInf(7).nKidoG = .GetByte(.GetOrdinal("カメラ8_輝度G_C"))
                    typData.typCamInf(7).nKidoB = .GetByte(.GetOrdinal("カメラ8_輝度B_C"))
                    typData.typCamInf(8).nKidoR = .GetByte(.GetOrdinal("カメラ9_輝度R_C"))
                    typData.typCamInf(8).nKidoG = .GetByte(.GetOrdinal("カメラ9_輝度G_C"))
                    typData.typCamInf(8).nKidoB = .GetByte(.GetOrdinal("カメラ9_輝度B_C"))
                    typData.typCamInf(9).nKidoR = .GetByte(.GetOrdinal("カメラ10_輝度R_C"))
                    typData.typCamInf(9).nKidoG = .GetByte(.GetOrdinal("カメラ10_輝度G_C"))
                    typData.typCamInf(9).nKidoB = .GetByte(.GetOrdinal("カメラ10_輝度B_C"))

                    'カメラ露光時間
                    typData.typCamInf(0).nTimeR = .GetInt16(.GetOrdinal("カメラ1_露光時間R"))
                    typData.typCamInf(0).nTimeG = .GetInt16(.GetOrdinal("カメラ1_露光時間G"))
                    typData.typCamInf(0).nTimeB = .GetInt16(.GetOrdinal("カメラ1_露光時間B"))
                    typData.typCamInf(1).nTimeR = .GetInt16(.GetOrdinal("カメラ2_露光時間R"))
                    typData.typCamInf(1).nTimeG = .GetInt16(.GetOrdinal("カメラ2_露光時間G"))
                    typData.typCamInf(1).nTimeB = .GetInt16(.GetOrdinal("カメラ2_露光時間B"))
                    typData.typCamInf(2).nTimeR = .GetInt16(.GetOrdinal("カメラ3_露光時間R"))
                    typData.typCamInf(2).nTimeG = .GetInt16(.GetOrdinal("カメラ3_露光時間G"))
                    typData.typCamInf(2).nTimeB = .GetInt16(.GetOrdinal("カメラ3_露光時間B"))
                    typData.typCamInf(3).nTimeR = .GetInt16(.GetOrdinal("カメラ4_露光時間R"))
                    typData.typCamInf(3).nTimeG = .GetInt16(.GetOrdinal("カメラ4_露光時間G"))
                    typData.typCamInf(3).nTimeB = .GetInt16(.GetOrdinal("カメラ4_露光時間B"))
                    typData.typCamInf(4).nTimeR = .GetInt16(.GetOrdinal("カメラ5_露光時間R"))
                    typData.typCamInf(4).nTimeG = .GetInt16(.GetOrdinal("カメラ5_露光時間G"))
                    typData.typCamInf(4).nTimeB = .GetInt16(.GetOrdinal("カメラ5_露光時間B"))
                    typData.typCamInf(5).nTimeR = .GetInt16(.GetOrdinal("カメラ6_露光時間R"))
                    typData.typCamInf(5).nTimeG = .GetInt16(.GetOrdinal("カメラ6_露光時間G"))
                    typData.typCamInf(5).nTimeB = .GetInt16(.GetOrdinal("カメラ6_露光時間B"))
                    typData.typCamInf(6).nTimeR = .GetInt16(.GetOrdinal("カメラ7_露光時間R"))
                    typData.typCamInf(6).nTimeG = .GetInt16(.GetOrdinal("カメラ7_露光時間G"))
                    typData.typCamInf(6).nTimeB = .GetInt16(.GetOrdinal("カメラ7_露光時間B"))
                    typData.typCamInf(7).nTimeR = .GetInt16(.GetOrdinal("カメラ8_露光時間R"))
                    typData.typCamInf(7).nTimeG = .GetInt16(.GetOrdinal("カメラ8_露光時間G"))
                    typData.typCamInf(7).nTimeB = .GetInt16(.GetOrdinal("カメラ8_露光時間B"))
                    typData.typCamInf(8).nTimeR = .GetInt16(.GetOrdinal("カメラ9_露光時間R"))
                    typData.typCamInf(8).nTimeG = .GetInt16(.GetOrdinal("カメラ9_露光時間G"))
                    typData.typCamInf(8).nTimeB = .GetInt16(.GetOrdinal("カメラ9_露光時間B"))
                    typData.typCamInf(9).nTimeR = .GetInt16(.GetOrdinal("カメラ10_露光時間R"))
                    typData.typCamInf(9).nTimeG = .GetInt16(.GetOrdinal("カメラ10_露光時間G"))
                    typData.typCamInf(9).nTimeB = .GetInt16(.GetOrdinal("カメラ10_露光時間B"))

                    'カメラゲイン
                    typData.typCamInf(0).dGainR = CSng(.GetDouble(.GetOrdinal("カメラ1_ゲイン_R")))
                    typData.typCamInf(0).dGainG = CSng(.GetDouble(.GetOrdinal("カメラ1_ゲイン_G")))
                    typData.typCamInf(0).dGainB = CSng(.GetDouble(.GetOrdinal("カメラ1_ゲイン_B")))
                    typData.typCamInf(1).dGainR = CSng(.GetDouble(.GetOrdinal("カメラ2_ゲイン_R")))
                    typData.typCamInf(1).dGainG = CSng(.GetDouble(.GetOrdinal("カメラ2_ゲイン_G")))
                    typData.typCamInf(1).dGainB = CSng(.GetDouble(.GetOrdinal("カメラ2_ゲイン_B")))
                    typData.typCamInf(2).dGainR = CSng(.GetDouble(.GetOrdinal("カメラ3_ゲイン_R")))
                    typData.typCamInf(2).dGainG = CSng(.GetDouble(.GetOrdinal("カメラ3_ゲイン_G")))
                    typData.typCamInf(2).dGainB = CSng(.GetDouble(.GetOrdinal("カメラ3_ゲイン_B")))
                    typData.typCamInf(3).dGainR = CSng(.GetDouble(.GetOrdinal("カメラ4_ゲイン_R")))
                    typData.typCamInf(3).dGainG = CSng(.GetDouble(.GetOrdinal("カメラ4_ゲイン_G")))
                    typData.typCamInf(3).dGainB = CSng(.GetDouble(.GetOrdinal("カメラ4_ゲイン_B")))
                    typData.typCamInf(4).dGainR = CSng(.GetDouble(.GetOrdinal("カメラ5_ゲイン_R")))
                    typData.typCamInf(4).dGainG = CSng(.GetDouble(.GetOrdinal("カメラ5_ゲイン_G")))
                    typData.typCamInf(4).dGainB = CSng(.GetDouble(.GetOrdinal("カメラ5_ゲイン_B")))
                    typData.typCamInf(5).dGainR = CSng(.GetDouble(.GetOrdinal("カメラ6_ゲイン_R")))
                    typData.typCamInf(5).dGainG = CSng(.GetDouble(.GetOrdinal("カメラ6_ゲイン_G")))
                    typData.typCamInf(5).dGainB = CSng(.GetDouble(.GetOrdinal("カメラ6_ゲイン_B")))
                    typData.typCamInf(6).dGainR = CSng(.GetDouble(.GetOrdinal("カメラ7_ゲイン_R")))
                    typData.typCamInf(6).dGainG = CSng(.GetDouble(.GetOrdinal("カメラ7_ゲイン_G")))
                    typData.typCamInf(6).dGainB = CSng(.GetDouble(.GetOrdinal("カメラ7_ゲイン_B")))
                    typData.typCamInf(7).dGainR = CSng(.GetDouble(.GetOrdinal("カメラ8_ゲイン_R")))
                    typData.typCamInf(7).dGainG = CSng(.GetDouble(.GetOrdinal("カメラ8_ゲイン_G")))
                    typData.typCamInf(7).dGainB = CSng(.GetDouble(.GetOrdinal("カメラ8_ゲイン_B")))
                    typData.typCamInf(8).dGainR = CSng(.GetDouble(.GetOrdinal("カメラ9_ゲイン_R")))
                    typData.typCamInf(8).dGainG = CSng(.GetDouble(.GetOrdinal("カメラ9_ゲイン_G")))
                    typData.typCamInf(8).dGainB = CSng(.GetDouble(.GetOrdinal("カメラ9_ゲイン_B")))
                    typData.typCamInf(9).dGainR = CSng(.GetDouble(.GetOrdinal("カメラ10_ゲイン_R")))
                    typData.typCamInf(9).dGainG = CSng(.GetDouble(.GetOrdinal("カメラ10_ゲイン_G")))
                    typData.typCamInf(9).dGainB = CSng(.GetDouble(.GetOrdinal("カメラ10_ゲイン_B")))

                    'ウォーク量
                    typData.dWalk = .GetFloat(.GetOrdinal("ウォーク量"))

                    '照明装置_調光値
                    typData.nDimming(0) = .GetInt16(.GetOrdinal("照明装置_調光値_1"))
                    typData.nDimming(1) = .GetInt16(.GetOrdinal("照明装置_調光値_2"))
                    typData.nDimming(2) = .GetInt16(.GetOrdinal("照明装置_調光値_3"))
                    typData.nDimming(3) = .GetInt16(.GetOrdinal("照明装置_調光値_4"))
                    typData.nDimming(4) = .GetInt16(.GetOrdinal("照明装置_調光値_5"))

                End With

                m_lstData.Add(typData)

            End While

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("COIL_TRAND 取得異常 [{0}]", ex.Message))
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try


    End Sub


    ''' <summary>
    ''' コイルトレンド情報(COIL_TRAND)を読込み　MAX,MIN,AVEを抽出
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub GetCalc()
        Dim strSQL As String                    ' SQL文
        Dim typ As TYP_TREND_CALC = Nothing

        '''' SQL接続準備
        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        If Not tcls_DB.Open() Then
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB接続エラー [" & DB_COIL_TRAND & "]")
            Exit Sub
        End If

        '''' SQL文生成
        strSQL = ""
        strSQL &= " SELECT ISNULL(MAX(ライン速度),0) AS 速度MAX"
        strSQL &= ", ISNULL(MIN(ライン速度),0) AS 速度MIN"
        strSQL &= ", ISNULL(AVG(ライン速度),0) AS 速度AVG"
        'strSQL &= ", ISNULL(MAX(板幅Min),0) AS 板幅MAX1"
        'strSQL &= ", ISNULL(MIN(板幅Min),0) AS 板幅MIN1"
        'strSQL &= ", ISNULL(AVG(板幅Min),0) AS 板幅AVG1"
        'strSQL &= ", ISNULL(MAX(板幅Max),0) AS 板幅MAX2"
        'strSQL &= ", ISNULL(MIN(板幅Max),0) AS 板幅MIN2"
        'strSQL &= ", ISNULL(AVG(板幅Max),0) AS 板幅AVG2"
        strSQL &= ", ISNULL(MAX(ABS(ウォーク量)),0) AS ウォークMAX"
        strSQL &= ", ISNULL(MIN(ABS(ウォーク量)),0) AS ウォークMIN"
        strSQL &= ", ISNULL(AVG(ABS(ウォーク量)),0) AS ウォークAVG"
        'strSQL &= ", ISNULL(MAX(照明装置_調光値),0) AS 調光値MAX"
        'strSQL &= ", ISNULL(MIN(照明装置_調光値),0) AS 調光値MIN"
        'strSQL &= ", ISNULL(AVG(照明装置_調光値),0) AS 調光値AVG"
        strSQL += " FROM " & DB_COIL_TRAND
        strSQL += " WHERE 管理No = '" & m_typCoilInf.strKanriNo & "'"

        '''' データ取得
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB読み込みエラー [" & DB_COIL_TRAND & "]")
            Exit Sub
        End If

        Try
            'While sqlRead.Read()
            With sqlRead
                .Read()

                For em As EM_COIL_TREND = EM_COIL_TREND.SPEED To EM_COIL_TREND.MAX
                    typ.initialize()
                    Select Case em
                        Case EM_COIL_TREND.SPEED
                            typ.min = .GetInt32(sqlRead.GetOrdinal("速度MIN")).ToString("#0")
                            typ.max = .GetInt32(sqlRead.GetOrdinal("速度MAX")).ToString("#0")
                            typ.avg = .GetInt32(sqlRead.GetOrdinal("速度AVG")).ToString("#0")
                            'Case EM_COIL_TREND.WIDTH
                            '    typ.min = .GetFloat(sqlRead.GetOrdinal("板幅MIN")).ToString("#0")
                            '    typ.max = .GetFloat(sqlRead.GetOrdinal("板幅MAX")).ToString("#0")
                            '    typ.avg = .GetDouble(sqlRead.GetOrdinal("板幅AVG")).ToString("#0")
                        Case EM_COIL_TREND.WALK
                            typ.min = .GetDouble(sqlRead.GetOrdinal("ウォークMIN")).ToString("#0")
                            typ.max = .GetDouble(sqlRead.GetOrdinal("ウォークMAX")).ToString("#0")
                            typ.avg = .GetDouble(sqlRead.GetOrdinal("ウォークAVG")).ToString("#0")
                            'Case EM_COIL_TREND.LED
                            '    typ.min = .GetByte(sqlRead.GetOrdinal("調光値MIN")).ToString("#0")
                            '    typ.max = .GetByte(sqlRead.GetOrdinal("調光値MAX")).ToString("#0")
                            '    typ.avg = .GetInt32(sqlRead.GetOrdinal("調光値AVG")).ToString("#0")
                        Case Else
                            Continue For
                    End Select

                    m_dctCalc.Add(em, typ)

                Next

            End With
            'End While
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("COIL_TRAND 取得異常 [{0}]", ex.Message))
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try
    End Sub

    ''' <summary>
    ''' 表裏のプロットデータを取得する
    ''' </summary>
    ''' <param name="typDsp">グラフ表示テーブル</param>
    ''' <remarks></remarks>
    Private Sub GetProtData(ByRef typDsp As TYP_TREND_DSP)
        Dim nPos() As Integer                   ' DB取得用 取得位置
        Dim nData(,) As Single                  ' DB取得用 データ
        Dim nCnt As Integer                     ' DB取得件数

        nCnt = m_lstData.Count
        ReDim Preserve nPos(nCnt - 1)
        ReDim Preserve nData(typDsp.dgvHanrei.RowCount - 1, nCnt - 1)

        '初期化（データ無しの値をセットする）
        For ii As Integer = 0 To typDsp.dgvHanrei.RowCount - 1
            For jj As Integer = 0 To nCnt - 1
                nData(ii, jj) = NON_TRAND_DATA
            Next
        Next
        Select Case typDsp.emType
            Case EM_COIL_TREND.SPEED
                nCnt = 0
                For ii As Integer = 0 To m_lstData.Count - 1
                    Dim nCk As Integer = -1
                    '既に同一取得位置でセット有りか？
                    For jj As Integer = 0 To nCnt - 1
                        If nPos(jj) = m_lstData(ii).nPos Then
                            nCk = jj
                            Exit For
                        End If
                    Next
                    If 0 > nCk Then
                        ' 同一取得位置でセットなし
                        nPos(nCnt) = m_lstData(ii).nPos
                        If 0 = m_lstData(ii).TorB Then
                            nData(0, nCnt) = m_lstData(ii).nSpeed
                        Else
                            nData(1, nCnt) = m_lstData(ii).nSpeed
                        End If
                        nCnt += 1
                    Else
                        ' 既に同一取得位置でセット有り
                        If 0 = m_lstData(ii).TorB Then
                            nData(0, nCk) = m_lstData(ii).nSpeed
                        Else
                            nData(1, nCk) = m_lstData(ii).nSpeed
                        End If
                    End If
                Next

            Case EM_COIL_TREND.WIDTH
                nCnt = 0
                For ii As Integer = 0 To m_lstData.Count - 1
                    nPos(nCnt) = m_lstData(ii).nPos
                    If 0 = m_lstData(ii).TorB Then
                        nData(0, nCnt) = m_lstData(ii).dItahaba(0)
                        nData(1, nCnt) = m_lstData(ii).dItahaba(1)
                    Else
                        nData(2, nCnt) = m_lstData(ii).dItahaba(0)
                        nData(3, nCnt) = m_lstData(ii).dItahaba(1)
                    End If
                    nCnt += 1
                Next

            Case EM_COIL_TREND.CAM_KIDO_O
                nCnt = 0
                For ii As Integer = 0 To m_lstData.Count - 1
                    If 0 <> m_lstData(ii).TorB Then
                        '表データのみ
                        Continue For
                    End If
                    nPos(nCnt) = m_lstData(ii).nPos
                    nData(0, nCnt) = m_lstData(ii).typCamInf(0).nKidoR
                    nData(1, nCnt) = m_lstData(ii).typCamInf(0).nKidoG
                    nData(2, nCnt) = m_lstData(ii).typCamInf(0).nKidoB
                    nData(3, nCnt) = m_lstData(ii).typCamInf(1).nKidoR
                    nData(4, nCnt) = m_lstData(ii).typCamInf(1).nKidoG
                    nData(5, nCnt) = m_lstData(ii).typCamInf(1).nKidoB
                    nData(6, nCnt) = m_lstData(ii).typCamInf(2).nKidoR
                    nData(7, nCnt) = m_lstData(ii).typCamInf(2).nKidoG
                    nData(8, nCnt) = m_lstData(ii).typCamInf(2).nKidoB
                    nData(9, nCnt) = m_lstData(ii).typCamInf(3).nKidoR
                    nData(10, nCnt) = m_lstData(ii).typCamInf(3).nKidoG
                    nData(11, nCnt) = m_lstData(ii).typCamInf(3).nKidoB
                    nData(12, nCnt) = m_lstData(ii).typCamInf(4).nKidoR
                    nData(13, nCnt) = m_lstData(ii).typCamInf(4).nKidoG
                    nData(14, nCnt) = m_lstData(ii).typCamInf(4).nKidoB
                    nData(15, nCnt) = m_lstData(ii).typCamInf(5).nKidoR
                    nData(16, nCnt) = m_lstData(ii).typCamInf(5).nKidoG
                    nData(17, nCnt) = m_lstData(ii).typCamInf(5).nKidoB
                    nData(18, nCnt) = m_lstData(ii).typCamInf(6).nKidoR
                    nData(19, nCnt) = m_lstData(ii).typCamInf(6).nKidoG
                    nData(20, nCnt) = m_lstData(ii).typCamInf(6).nKidoB
                    nData(21, nCnt) = m_lstData(ii).typCamInf(7).nKidoR
                    nData(22, nCnt) = m_lstData(ii).typCamInf(7).nKidoG
                    nData(23, nCnt) = m_lstData(ii).typCamInf(7).nKidoB
                    nData(24, nCnt) = m_lstData(ii).typCamInf(8).nKidoR
                    nData(25, nCnt) = m_lstData(ii).typCamInf(8).nKidoG
                    nData(26, nCnt) = m_lstData(ii).typCamInf(8).nKidoB
                    nData(27, nCnt) = m_lstData(ii).typCamInf(9).nKidoR
                    nData(28, nCnt) = m_lstData(ii).typCamInf(9).nKidoG
                    nData(29, nCnt) = m_lstData(ii).typCamInf(9).nKidoB
                    ' 取得件数インクリメント
                    nCnt += 1
                Next

            Case EM_COIL_TREND.CAM_KIDO_U
                nCnt = 0
                For ii As Integer = 0 To m_lstData.Count - 1
                    If 1 <> m_lstData(ii).TorB Then
                        '裏データのみ
                        Continue For
                    End If
                    nPos(nCnt) = m_lstData(ii).nPos
                    nData(0, nCnt) = m_lstData(ii).typCamInf(0).nKidoR
                    nData(1, nCnt) = m_lstData(ii).typCamInf(0).nKidoG
                    nData(2, nCnt) = m_lstData(ii).typCamInf(0).nKidoB
                    nData(3, nCnt) = m_lstData(ii).typCamInf(1).nKidoR
                    nData(4, nCnt) = m_lstData(ii).typCamInf(1).nKidoG
                    nData(5, nCnt) = m_lstData(ii).typCamInf(1).nKidoB
                    nData(6, nCnt) = m_lstData(ii).typCamInf(2).nKidoR
                    nData(7, nCnt) = m_lstData(ii).typCamInf(2).nKidoG
                    nData(8, nCnt) = m_lstData(ii).typCamInf(2).nKidoB
                    nData(9, nCnt) = m_lstData(ii).typCamInf(3).nKidoR
                    nData(10, nCnt) = m_lstData(ii).typCamInf(3).nKidoG
                    nData(11, nCnt) = m_lstData(ii).typCamInf(3).nKidoB
                    nData(12, nCnt) = m_lstData(ii).typCamInf(4).nKidoR
                    nData(13, nCnt) = m_lstData(ii).typCamInf(4).nKidoG
                    nData(14, nCnt) = m_lstData(ii).typCamInf(4).nKidoB
                    nData(15, nCnt) = m_lstData(ii).typCamInf(5).nKidoR
                    nData(16, nCnt) = m_lstData(ii).typCamInf(5).nKidoG
                    nData(17, nCnt) = m_lstData(ii).typCamInf(5).nKidoB
                    nData(18, nCnt) = m_lstData(ii).typCamInf(6).nKidoR
                    nData(19, nCnt) = m_lstData(ii).typCamInf(6).nKidoG
                    nData(20, nCnt) = m_lstData(ii).typCamInf(6).nKidoB
                    nData(21, nCnt) = m_lstData(ii).typCamInf(7).nKidoR
                    nData(22, nCnt) = m_lstData(ii).typCamInf(7).nKidoG
                    nData(23, nCnt) = m_lstData(ii).typCamInf(7).nKidoB
                    nData(24, nCnt) = m_lstData(ii).typCamInf(8).nKidoR
                    nData(25, nCnt) = m_lstData(ii).typCamInf(8).nKidoG
                    nData(26, nCnt) = m_lstData(ii).typCamInf(8).nKidoB
                    nData(27, nCnt) = m_lstData(ii).typCamInf(9).nKidoR
                    nData(28, nCnt) = m_lstData(ii).typCamInf(9).nKidoG
                    nData(29, nCnt) = m_lstData(ii).typCamInf(9).nKidoB
                    ' 取得件数インクリメント
                    nCnt += 1
                Next

            Case EM_COIL_TREND.CAM_GAIN_O
                nCnt = 0
                For ii As Integer = 0 To m_lstData.Count - 1
                    If 0 <> m_lstData(ii).TorB Then
                        '表データのみ
                        Continue For
                    End If
                    nPos(nCnt) = m_lstData(ii).nPos
                    nData(0, nCnt) = m_lstData(ii).typCamInf(0).dGainR
                    nData(1, nCnt) = m_lstData(ii).typCamInf(0).dGainG
                    nData(2, nCnt) = m_lstData(ii).typCamInf(0).dGainB
                    nData(3, nCnt) = m_lstData(ii).typCamInf(1).dGainR
                    nData(4, nCnt) = m_lstData(ii).typCamInf(1).dGainG
                    nData(5, nCnt) = m_lstData(ii).typCamInf(1).dGainB
                    nData(6, nCnt) = m_lstData(ii).typCamInf(2).dGainR
                    nData(7, nCnt) = m_lstData(ii).typCamInf(2).dGainG
                    nData(8, nCnt) = m_lstData(ii).typCamInf(2).dGainB
                    nData(9, nCnt) = m_lstData(ii).typCamInf(3).dGainR
                    nData(10, nCnt) = m_lstData(ii).typCamInf(3).dGainG
                    nData(11, nCnt) = m_lstData(ii).typCamInf(3).dGainB
                    nData(12, nCnt) = m_lstData(ii).typCamInf(4).dGainR
                    nData(13, nCnt) = m_lstData(ii).typCamInf(4).dGainG
                    nData(14, nCnt) = m_lstData(ii).typCamInf(4).dGainB
                    nData(15, nCnt) = m_lstData(ii).typCamInf(5).dGainR
                    nData(16, nCnt) = m_lstData(ii).typCamInf(5).dGainG
                    nData(17, nCnt) = m_lstData(ii).typCamInf(5).dGainB
                    nData(18, nCnt) = m_lstData(ii).typCamInf(6).dGainR
                    nData(19, nCnt) = m_lstData(ii).typCamInf(6).dGainG
                    nData(20, nCnt) = m_lstData(ii).typCamInf(6).dGainB
                    nData(21, nCnt) = m_lstData(ii).typCamInf(7).dGainR
                    nData(22, nCnt) = m_lstData(ii).typCamInf(7).dGainG
                    nData(23, nCnt) = m_lstData(ii).typCamInf(7).dGainB
                    nData(24, nCnt) = m_lstData(ii).typCamInf(8).dGainR
                    nData(25, nCnt) = m_lstData(ii).typCamInf(8).dGainG
                    nData(26, nCnt) = m_lstData(ii).typCamInf(8).dGainB
                    nData(27, nCnt) = m_lstData(ii).typCamInf(9).dGainR
                    nData(28, nCnt) = m_lstData(ii).typCamInf(9).dGainG
                    nData(29, nCnt) = m_lstData(ii).typCamInf(9).dGainB
                    ' 取得件数インクリメント
                    nCnt += 1
                Next

            Case EM_COIL_TREND.CAM_GAIN_U
                nCnt = 0
                For ii As Integer = 0 To m_lstData.Count - 1
                    If 1 <> m_lstData(ii).TorB Then
                        '裏データのみ
                        Continue For
                    End If
                    nPos(nCnt) = m_lstData(ii).nPos
                    nData(0, nCnt) = m_lstData(ii).typCamInf(0).dGainR
                    nData(1, nCnt) = m_lstData(ii).typCamInf(0).dGainG
                    nData(2, nCnt) = m_lstData(ii).typCamInf(0).dGainB
                    nData(3, nCnt) = m_lstData(ii).typCamInf(1).dGainR
                    nData(4, nCnt) = m_lstData(ii).typCamInf(1).dGainG
                    nData(5, nCnt) = m_lstData(ii).typCamInf(1).dGainB
                    nData(6, nCnt) = m_lstData(ii).typCamInf(2).dGainR
                    nData(7, nCnt) = m_lstData(ii).typCamInf(2).dGainG
                    nData(8, nCnt) = m_lstData(ii).typCamInf(2).dGainB
                    nData(9, nCnt) = m_lstData(ii).typCamInf(3).dGainR
                    nData(10, nCnt) = m_lstData(ii).typCamInf(3).dGainG
                    nData(11, nCnt) = m_lstData(ii).typCamInf(3).dGainB
                    nData(12, nCnt) = m_lstData(ii).typCamInf(4).dGainR
                    nData(13, nCnt) = m_lstData(ii).typCamInf(4).dGainG
                    nData(14, nCnt) = m_lstData(ii).typCamInf(4).dGainB
                    nData(15, nCnt) = m_lstData(ii).typCamInf(5).dGainR
                    nData(16, nCnt) = m_lstData(ii).typCamInf(5).dGainG
                    nData(17, nCnt) = m_lstData(ii).typCamInf(5).dGainB
                    nData(18, nCnt) = m_lstData(ii).typCamInf(6).dGainR
                    nData(19, nCnt) = m_lstData(ii).typCamInf(6).dGainG
                    nData(20, nCnt) = m_lstData(ii).typCamInf(6).dGainB
                    nData(21, nCnt) = m_lstData(ii).typCamInf(7).dGainR
                    nData(22, nCnt) = m_lstData(ii).typCamInf(7).dGainG
                    nData(23, nCnt) = m_lstData(ii).typCamInf(7).dGainB
                    nData(24, nCnt) = m_lstData(ii).typCamInf(8).dGainR
                    nData(25, nCnt) = m_lstData(ii).typCamInf(8).dGainG
                    nData(26, nCnt) = m_lstData(ii).typCamInf(8).dGainB
                    nData(27, nCnt) = m_lstData(ii).typCamInf(9).dGainR
                    nData(28, nCnt) = m_lstData(ii).typCamInf(9).dGainG
                    nData(29, nCnt) = m_lstData(ii).typCamInf(9).dGainB
                    ' 取得件数インクリメント
                    nCnt += 1
                Next

            Case EM_COIL_TREND.CAM_TIME_O
                nCnt = 0
                For ii As Integer = 0 To m_lstData.Count - 1
                    If 0 <> m_lstData(ii).TorB Then
                        '表データのみ
                        Continue For
                    End If
                    nPos(nCnt) = m_lstData(ii).nPos
                    nData(0, nCnt) = CInt(m_lstData(ii).typCamInf(0).nTimeR)
                    nData(1, nCnt) = CInt(m_lstData(ii).typCamInf(0).nTimeG)
                    nData(2, nCnt) = CInt(m_lstData(ii).typCamInf(0).nTimeB)
                    nData(3, nCnt) = CInt(m_lstData(ii).typCamInf(1).nTimeR)
                    nData(4, nCnt) = CInt(m_lstData(ii).typCamInf(1).nTimeG)
                    nData(5, nCnt) = CInt(m_lstData(ii).typCamInf(1).nTimeB)
                    nData(6, nCnt) = CInt(m_lstData(ii).typCamInf(2).nTimeR)
                    nData(7, nCnt) = CInt(m_lstData(ii).typCamInf(2).nTimeG)
                    nData(8, nCnt) = CInt(m_lstData(ii).typCamInf(2).nTimeB)
                    nData(9, nCnt) = CInt(m_lstData(ii).typCamInf(3).nTimeR)
                    nData(10, nCnt) = CInt(m_lstData(ii).typCamInf(3).nTimeG)
                    nData(11, nCnt) = CInt(m_lstData(ii).typCamInf(3).nTimeB)
                    nData(12, nCnt) = CInt(m_lstData(ii).typCamInf(4).nTimeR)
                    nData(13, nCnt) = CInt(m_lstData(ii).typCamInf(4).nTimeG)
                    nData(14, nCnt) = CInt(m_lstData(ii).typCamInf(4).nTimeB)
                    nData(15, nCnt) = CInt(m_lstData(ii).typCamInf(5).nTimeR)
                    nData(16, nCnt) = CInt(m_lstData(ii).typCamInf(5).nTimeG)
                    nData(17, nCnt) = CInt(m_lstData(ii).typCamInf(5).nTimeB)
                    nData(18, nCnt) = CInt(m_lstData(ii).typCamInf(6).nTimeR)
                    nData(19, nCnt) = CInt(m_lstData(ii).typCamInf(6).nTimeG)
                    nData(20, nCnt) = CInt(m_lstData(ii).typCamInf(6).nTimeB)
                    nData(21, nCnt) = CInt(m_lstData(ii).typCamInf(7).nTimeR)
                    nData(22, nCnt) = CInt(m_lstData(ii).typCamInf(7).nTimeG)
                    nData(23, nCnt) = CInt(m_lstData(ii).typCamInf(7).nTimeB)
                    nData(24, nCnt) = CInt(m_lstData(ii).typCamInf(8).nTimeR)
                    nData(25, nCnt) = CInt(m_lstData(ii).typCamInf(8).nTimeG)
                    nData(26, nCnt) = CInt(m_lstData(ii).typCamInf(8).nTimeB)
                    nData(27, nCnt) = CInt(m_lstData(ii).typCamInf(9).nTimeR)
                    nData(28, nCnt) = CInt(m_lstData(ii).typCamInf(9).nTimeG)
                    nData(29, nCnt) = CInt(m_lstData(ii).typCamInf(9).nTimeB)
                    nCnt += 1

                Next

            Case EM_COIL_TREND.CAM_TIME_U
                nCnt = 0
                For ii As Integer = 0 To m_lstData.Count - 1
                    If 1 <> m_lstData(ii).TorB Then
                        '裏データのみ
                        Continue For
                    End If
                    nPos(nCnt) = m_lstData(ii).nPos
                    nData(0, nCnt) = CInt(m_lstData(ii).typCamInf(0).nTimeR)
                    nData(1, nCnt) = CInt(m_lstData(ii).typCamInf(0).nTimeG)
                    nData(2, nCnt) = CInt(m_lstData(ii).typCamInf(0).nTimeB)
                    nData(3, nCnt) = CInt(m_lstData(ii).typCamInf(1).nTimeR)
                    nData(4, nCnt) = CInt(m_lstData(ii).typCamInf(1).nTimeG)
                    nData(5, nCnt) = CInt(m_lstData(ii).typCamInf(1).nTimeB)
                    nData(6, nCnt) = CInt(m_lstData(ii).typCamInf(2).nTimeR)
                    nData(7, nCnt) = CInt(m_lstData(ii).typCamInf(2).nTimeG)
                    nData(8, nCnt) = CInt(m_lstData(ii).typCamInf(2).nTimeB)
                    nData(9, nCnt) = CInt(m_lstData(ii).typCamInf(3).nTimeR)
                    nData(10, nCnt) = CInt(m_lstData(ii).typCamInf(3).nTimeG)
                    nData(11, nCnt) = CInt(m_lstData(ii).typCamInf(3).nTimeB)
                    nData(12, nCnt) = CInt(m_lstData(ii).typCamInf(4).nTimeR)
                    nData(13, nCnt) = CInt(m_lstData(ii).typCamInf(4).nTimeG)
                    nData(14, nCnt) = CInt(m_lstData(ii).typCamInf(4).nTimeB)
                    nData(15, nCnt) = CInt(m_lstData(ii).typCamInf(5).nTimeR)
                    nData(16, nCnt) = CInt(m_lstData(ii).typCamInf(5).nTimeG)
                    nData(17, nCnt) = CInt(m_lstData(ii).typCamInf(5).nTimeB)
                    nData(18, nCnt) = CInt(m_lstData(ii).typCamInf(6).nTimeR)
                    nData(19, nCnt) = CInt(m_lstData(ii).typCamInf(6).nTimeG)
                    nData(20, nCnt) = CInt(m_lstData(ii).typCamInf(6).nTimeB)
                    nData(21, nCnt) = CInt(m_lstData(ii).typCamInf(7).nTimeR)
                    nData(22, nCnt) = CInt(m_lstData(ii).typCamInf(7).nTimeG)
                    nData(23, nCnt) = CInt(m_lstData(ii).typCamInf(7).nTimeB)
                    nData(24, nCnt) = CInt(m_lstData(ii).typCamInf(8).nTimeR)
                    nData(25, nCnt) = CInt(m_lstData(ii).typCamInf(8).nTimeG)
                    nData(26, nCnt) = CInt(m_lstData(ii).typCamInf(8).nTimeB)
                    nData(27, nCnt) = CInt(m_lstData(ii).typCamInf(9).nTimeR)
                    nData(28, nCnt) = CInt(m_lstData(ii).typCamInf(9).nTimeG)
                    nData(29, nCnt) = CInt(m_lstData(ii).typCamInf(9).nTimeB)
                    nCnt += 1

                Next

            Case EM_COIL_TREND.WALK
                nCnt = 0
                For ii As Integer = 0 To m_lstData.Count - 1
                    Dim nCk As Integer = -1
                    '既に同一取得位置でセット有りか？
                    For jj As Integer = 0 To nCnt - 1
                        If nPos(jj) = m_lstData(ii).nPos Then
                            nCk = jj
                            Exit For
                        End If
                    Next
                    If 0 > nCk Then
                        ' 同一取得位置でセットなし
                        nPos(nCnt) = m_lstData(ii).nPos
                        If 0 = m_lstData(ii).TorB Then
                            nData(0, nCnt) = CInt(m_lstData(ii).dWalk)
                        Else
                            nData(1, nCnt) = CInt(m_lstData(ii).dWalk)
                        End If
                        nCnt += 1
                    Else
                        ' 既に同一取得位置でセット有り
                        If 0 = m_lstData(ii).TorB Then
                            nData(0, nCnt) = CInt(m_lstData(ii).dWalk)
                        Else
                            nData(1, nCnt) = CInt(m_lstData(ii).dWalk)
                        End If
                    End If
                Next

            Case EM_COIL_TREND.LED_O
                nCnt = 0
                For ii As Integer = 0 To m_lstData.Count - 1
                    If 0 <> m_lstData(ii).TorB Then
                        '表データのみ
                        Continue For
                    End If
                    Dim nCk As Integer = -1
                    '既に同一取得位置でセット有りか？
                    For jj As Integer = 0 To nCnt - 1
                        If nPos(jj) = m_lstData(ii).nPos Then
                            nCk = jj
                            Exit For
                        End If
                    Next
                    If 0 > nCk Then
                        ' 同一取得位置でセットなし
                        nPos(nCnt) = m_lstData(ii).nPos
                        nData(0, nCnt) = m_lstData(ii).nDimming(0)
                        nData(1, nCnt) = m_lstData(ii).nDimming(1)
                        nData(2, nCnt) = m_lstData(ii).nDimming(2)
                        nData(3, nCnt) = m_lstData(ii).nDimming(3)
                        nData(4, nCnt) = m_lstData(ii).nDimming(4)
                        nCnt += 1
                    Else
                        ' 既に同一取得位置でセット有り
                        nData(0, nCk) = m_lstData(ii).nDimming(0)
                        nData(1, nCk) = m_lstData(ii).nDimming(1)
                        nData(2, nCk) = m_lstData(ii).nDimming(2)
                        nData(3, nCk) = m_lstData(ii).nDimming(3)
                        nData(4, nCk) = m_lstData(ii).nDimming(4)
                    End If
                Next
            Case EM_COIL_TREND.LED_U
                nCnt = 0
                For ii As Integer = 0 To m_lstData.Count - 1
                    If 1 <> m_lstData(ii).TorB Then
                        '裏データのみ
                        Continue For
                    End If
                    Dim nCk As Integer = -1
                    '既に同一取得位置でセット有りか？
                    For jj As Integer = 0 To nCnt - 1
                        If nPos(jj) = m_lstData(ii).nPos Then
                            nCk = jj
                            Exit For
                        End If
                    Next
                    If 0 > nCk Then
                        ' 同一取得位置でセットなし
                        nPos(nCnt) = m_lstData(ii).nPos
                        nData(0, nCnt) = m_lstData(ii).nDimming(0)
                        nData(1, nCnt) = m_lstData(ii).nDimming(1)
                        nData(2, nCnt) = m_lstData(ii).nDimming(2)
                        nData(3, nCnt) = m_lstData(ii).nDimming(3)
                        nData(4, nCnt) = m_lstData(ii).nDimming(4)
                        nCnt += 1
                    Else
                        ' 既に同一取得位置でセット有り
                        nData(0, nCk) = m_lstData(ii).nDimming(0)
                        nData(1, nCk) = m_lstData(ii).nDimming(1)
                        nData(2, nCk) = m_lstData(ii).nDimming(2)
                        nData(3, nCk) = m_lstData(ii).nDimming(3)
                        nData(4, nCk) = m_lstData(ii).nDimming(4)
                    End If
                Next

        End Select

        typDsp.nCnt = nCnt
        typDsp.nCoilPos = nPos
        typDsp.nData = nData

    End Sub

    ''' <summary>
    ''' 最小、最大、平均データ表示
    ''' </summary>
    ''' <param name="typDsp"></param>
    Private Sub DspCalcData(ByVal typDsp As TYP_TREND_DSP)

        With typDsp.dgvCalc

            If Not m_dctCalc.ContainsKey(typDsp.emType) Then
                .Visible = False
                Exit Sub
            End If

            Dim calc As TYP_TREND_CALC = m_dctCalc(typDsp.emType)

            .Item(EM_COL_CALC.MIN_DATA, 0).Value = calc.min
            .Item(EM_COL_CALC.MAX_DATA, 0).Value = calc.max
            .Item(EM_COL_CALC.AVG_DATA, 0).Value = calc.avg
            .Visible = True

        End With

    End Sub

    ''' <summary>
    ''' コンボボックス
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbName_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)

        Dim cmbData As ComboBox = CType(sender, ComboBox)

        If Not m_bSetEnd Then
            Exit Sub
        End If

        Try
            For ii As Integer = 0 To m_typTrendDsp.Count - 1
                If cmbData IsNot m_typTrendDsp(ii).cmbName Then
                    Continue For
                End If

                PutTrendDsp(m_typTrendDsp(ii))

                m_typTrendDsp(ii).emType = CType(cmbData.SelectedValue, EM_COIL_TREND)

                DspInit(m_typTrendDsp(ii))

                SetDsp(m_typTrendDsp(ii))

            Next

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("コンボボックス 例外[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' CSV出力
    ''' </summary>
    ''' <param name="strFolder">保存ファイルパス</param>
    ''' <param name="strName">保存ファイル名</param>
    ''' <remarks></remarks>
    Private Sub CsvOutTrand(ByVal strFolder As String, ByVal strName As String)

        Dim strFPath As String                          ' フルパス
        Dim strMsg As String = ""                       ' 通知メッセージ
        Dim strData As String = ""                      ' 1行のデータ
        Dim nn As Integer                               ' INDEX
        Dim nCntMax As Integer = 0                      ' データ件数MAX

        '' フルパス
        strFPath = IO.Path.Combine(strFolder, strName)

        '' フォルダの有無チェック(無かったら作ろう)
        tMod.CheckFolder(strFolder)

        '' CSV出力
        Using cls_Stram As New tClass.tClass_FileStream(strFPath, IO.FileMode.Create, IO.FileAccess.ReadWrite, IO.FileShare.ReadWrite)
            Try
                ' オープン
                cls_Stram.Open()

                strData = ""
                'ヘッダー出力(トレンド名）
                For ii As Integer = 0 To m_typTrendDsp.Count - 1
                    strData += m_typTrendDsp(ii).cmbName.Text & ","
                    For jj As Integer = 0 To m_typTrendDsp(ii).dgvHanrei.RowCount - 1
                        strData += ","
                    Next jj
                    strData += ","
                Next ii

                ' 最後の","を排除
                strData = strData.Remove(strData.Length - 2, 2)

                ' ＣＳＶへ書き込み
                cls_Stram.WriteLine(strData)

                strData = ""
                'ヘッダー出力(凡例名）
                For ii As Integer = 0 To m_typTrendDsp.Count - 1
                    strData += "取得位置,"
                    For jj As Integer = 0 To m_typTrendDsp(ii).dgvHanrei.RowCount - 1
                        strData += m_typTrendDsp(ii).dgvHanrei(em_DataColumnType.iName, jj).Value.ToString
                        strData += ","
                    Next jj
                    strData += ","
                Next ii

                ' 最後の","を排除
                strData = strData.Remove(strData.Length - 2, 2)

                ' ＣＳＶへ書き込み
                cls_Stram.WriteLine(strData)

                ' データ件数の最大値取得
                For ii As Integer = 0 To m_typTrendDsp.Count - 1
                    If nCntMax < m_typTrendDsp(ii).nCnt Then
                        nCntMax = m_typTrendDsp(ii).nCnt
                    End If
                Next ii

                'データ出力
                nn = 0
                While nCntMax > nn
                    strData = ""
                    For ii As Integer = 0 To m_typTrendDsp.Count - 1
                        ' データ出力件数がデータ最大数以下？
                        If m_typTrendDsp(ii).nCnt > nn Then
                            ' データの設定
                            strData += CStr(m_typTrendDsp(ii).nCoilPos(nn))
                            strData += ","
                            For jj As Integer = 0 To m_typTrendDsp(ii).dgvHanrei.RowCount - 1
                                strData += CStr(m_typTrendDsp(ii).nData(jj, nn))
                                strData += ","
                            Next jj
                            strData += ","
                        Else
                            ' 空白の設定
                            strData += ","
                            For jj As Integer = 0 To m_typTrendDsp(ii).dgvHanrei.RowCount - 1
                                strData += ","
                            Next jj
                            strData += ","
                        End If

                    Next ii
                    '最後の","を排除
                    strData = strData.Remove(strData.Length - 2, 2)

                    ' ＣＳＶへ書き込み
                    cls_Stram.WriteLine(strData)

                    nn += 1
                    ' 
                End While

                ' クローズ
                cls_Stram.Close()
                strMsg = System.IO.Path.GetFullPath(strFPath) & "にCSVファイルを保存しました。"

            Catch ex As Exception
                gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("CSV出力 例外[{0}]", ex.Message))
                strMsg = "CSV出力に失敗しました。"
            End Try
        End Using

        '''' ガイダンス表示
        Using frmMsg As New tClassForm.frmPop_MsgBox(strMsg, Me.Text)
            frmMsg.ShowDialog()
        End Using

    End Sub


    ''' <summary>
    ''' アップダウン コントロール
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub nudGraphMaxMin_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)

        Dim nud As NumericUpDown = CType(sender, NumericUpDown)

        If Not m_bSetEnd Then
            Exit Sub
        End If

        Try
            For ii As Integer = 0 To m_typTrendDsp.Count - 1
                If nud IsNot m_typTrendDsp(ii).nudGraphMax AndAlso nud IsNot m_typTrendDsp(ii).nudGraphMin Then
                    Continue For
                End If

                ' 入力データチェック
                If False = InputDataCheck(m_typTrendDsp(ii)) Then
                    Exit Try
                End If

                SetDsp(m_typTrendDsp(ii))
            Next
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("アップダウン コントロール 例外[{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' コンボボックス初期設定
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="val"></param>
    ''' <remarks></remarks>
    Private Sub cmbName_Init(ByVal sender As ComboBox, ByVal val As EM_COIL_TREND)

        Dim ds As New DataTable()
        ds.Columns.Add("NAME", GetType(String))
        ds.Columns.Add("VALUE", GetType(EM_COIL_TREND))

        'トレンド情報に登録されているグループ名
        Dim keys As EM_COIL_TREND() = New EM_COIL_TREND(m_dctTrendInf.Keys.Count - 1) {}
        m_dctTrendInf.Keys.CopyTo(keys, 0)
        For ii As Integer = 0 To keys.Count - 1
            Dim typ As TYP_TREND_INF = m_dctTrendInf(keys(ii))
            Dim row As DataRow = ds.NewRow()
            row("NAME") = typ.strGroupName
            row("VALUE") = keys(ii)
            ds.Rows.Add(row)
        Next

        ds.AcceptChanges()
        sender.Items.Clear()
        sender.DataSource = ds
        sender.DisplayMember = "NAME"
        sender.ValueMember = "VALUE"
        sender.DropDownStyle = ComboBoxStyle.DropDownList
        sender.SelectedValue = val

    End Sub


    ''///////////////////////////////////////////////////////////////////////////
    ''データグリッドビュー
    ''///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' グリッド初期化
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub dgvCoilInit()

        Dim dgv As tCnt.tCnt_DataGridView = dgvCoil

        dgv.dgvInitialize(dgv)
        dgv.DataGridViewSelectionCancel(dgv)                                    ' 選択色無し

        Try
            With dgv
                .ScrollBars = ScrollBars.Horizontal                             ' スクロールバー(水平のみ)
                .DoubleBuffered = True

                .ReadOnly = True                                                ' 読み取り専用
                .EditMode = DataGridViewEditMode.EditProgrammatically           ' セル書き換え
                .ColumnHeadersVisible = True                                   ' 列ヘッダ表示あり

                '' 選択セル色定義
                .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '選択色
                .DefaultCellStyle().SelectionForeColor = .DefaultCellStyle().ForeColor      '選択色
                .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                .DefaultCellStyle.WrapMode = DataGridViewTriState.True
                .ColumnHeadersHeight = 20                                       ' 列ヘッダ高さ調整

                For col As EM_COL_COLINF = EM_COL_COLINF.ORDER_NO To EM_COL_COLINF.MAX_COL
                    Select Case col
                        Case EM_COL_COLINF.ORDER_NO
                            ' 命令No
                            dgvColSetText(dgv, "命令No", 78, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                        Case EM_COL_COLINF.COIL_LEN
                            ' コイル長
                            dgvColSetText(dgv, "コイル長", 78, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                        Case EM_COL_COLINF.KANRI_NO
                            ' 管理No
                            dgvColSetText(dgv, "管理No", 240, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)



                        Case EM_COL_COLINF.COIL_NO
                            ' 生産番号
                            dgvColSetText(dgv, "生産番号", 206, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                        Case EM_COL_COLINF.TYPE_DIV
                            ' 品種
                            dgvColSetText(dgv, "品種", 78, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_COL_COLINF.MATERIAL
                            ' 材質
                            dgvColSetText(dgv, "材質", 38, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_COL_COLINF.NEXT_STEP
                            ' 次工程
                            dgvColSetText(dgv, "次工程", 78, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_COL_COLINF.KENSA_REF
                            ' 検査基準
                            dgvColSetText(dgv, "検査基準", 78, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
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
        Catch ex As Exception
        End Try

    End Sub

    ''' <summary>
    ''' コイル情報表示
    ''' </summary>
    Private Sub DspCoil()
        Try
            With dgvCoil
                For ii As Integer = 0 To .ColumnCount - 1
                    Select Case ii
                        Case EM_COL_COLINF.ORDER_NO         ' 命令No
                            .Item(ii, 0).Value = m_typCoilInf.strOrderNo
                        Case EM_COL_COLINF.COIL_LEN         ' コイル長
                            .Item(ii, 0).Value = Format(m_typCoilInf.nCoilLen / 1000, "0")
                        Case EM_COL_COLINF.KANRI_NO         ' 管理No
                            .Item(ii, 0).Value = m_typCoilInf.strKanriNo
                        Case EM_COL_COLINF.COIL_NO            ' 生産番号
                            .Item(ii, 0).Value = m_typCoilInf.strCoilNo
                        Case EM_COL_COLINF.TYPE_DIV           ' 品種
                            .Item(ii, 0).Value = m_typCoilInf.strHinCode
                        Case EM_COL_COLINF.MATERIAL           ' 材質
                            .Item(ii, 0).Value = m_typCoilInf.strSyuZai
                        Case EM_COL_COLINF.NEXT_STEP          ' 次工程
                            .Item(ii, 0).Value = m_typCoilInf.strNextCode
                        Case EM_COL_COLINF.KENSA_REF          ' 検査基準
                            .Item(ii, 0).Value = m_typCoilInf.strKenKijun(0) '検査基準_表
                        Case EM_COL_COLINF.SIZE
                            ''サイズ(板厚[μm⇒mm]×板幅[mm])
                            Dim sThick As String = Format(m_typCoilInf.nCoilAtu / 1000, "0.000")
                            Dim sWidth As String = m_typCoilInf.nCoilWidth.ToString
                            .Item(ii, 0).Value = String.Format("{0}×{1,4}", sThick, sWidth)
                        Case EM_COL_COLINF.ORDER
                            ''サイズ(板厚[μm⇒mm]×板幅[mm])
                            Dim sThickSei As String = Format(m_typCoilInf.nCoilAtuSei / 1000, "0.000")
                            Dim sWidthSei As String = m_typCoilInf.nCoilWidSei.ToString
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
    ''' 最大、最小、平均データ表示DGVイニシャル
    ''' </summary>
    ''' <param name="dgv"></param>
    Private Sub dgvCalcInit(ByVal dgv As tCnt.tCnt_DataGridView)


        dgv.dgvInitialize(dgv)
        dgv.DataGridViewSelectionCancel(dgv)                           ' 選択色無し

        Try
            With dgv
                .ScrollBars = ScrollBars.None                                   ' スクロールバーなし
                .DoubleBuffered = True

                .ReadOnly = True                                                ' 読み取り専用
                .EditMode = DataGridViewEditMode.EditProgrammatically           ' セル書き換え
                .ColumnHeadersVisible = False                                   ' 列ヘッダ表示なし

                '' 選択セル色定義
                .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '選択色
                .DefaultCellStyle().SelectionForeColor = .DefaultCellStyle().ForeColor      '選択色
                .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                .DefaultCellStyle.WrapMode = DataGridViewTriState.True

                For col As EM_COL_CALC = EM_COL_CALC.MAX_NAME To EM_COL_CALC.MAX_COL
                    Select Case col
                        Case EM_COL_CALC.MAX_NAME, EM_COL_CALC.MIN_NAME, EM_COL_CALC.AVG_NAME
                            dgvColSetText(dgv, "", 58, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                        Case EM_COL_CALC.MAX_DATA, EM_COL_CALC.MIN_DATA, EM_COL_CALC.AVG_DATA
                            dgvColSetText(dgv, "", 88, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)
                    End Select
                Next

                .RowCount = 1
                .Rows(0).Height = .ClientSize.Height
                .Item(EM_COL_CALC.MAX_NAME, 0).Value = "最大値"
                .Item(EM_COL_CALC.MAX_NAME, 0).Style.BackColor = g_ColorControl
                .Item(EM_COL_CALC.MIN_NAME, 0).Value = "最小値"
                .Item(EM_COL_CALC.MIN_NAME, 0).Style.BackColor = g_ColorControl
                .Item(EM_COL_CALC.AVG_NAME, 0).Value = "平均値"
                .Item(EM_COL_CALC.AVG_NAME, 0).Style.BackColor = g_ColorControl

            End With
        Catch ex As Exception
        End Try

    End Sub

    ''' <summary>
    ''' リサイズによる表示変更
    ''' </summary>
    ''' <param name="dgv"></param>
    Private Sub dgvWidthResize(ByVal dgv As tCnt.tCnt_DataGridView)
        Dim hsc As HScrollBar

        hsc = CType(dgv.Controls(0), HScrollBar)
        If hsc.Visible Then
            dgv.ColumnHeadersHeight = 16
            '水平スクロールバー表示
            dgv.Rows(0).Height = dgv.ClientSize.Height - dgv.ColumnHeadersHeight - hsc.Height
        Else
            dgv.ColumnHeadersHeight = 20
            dgv.Rows(0).Height = dgv.ClientSize.Height - dgv.ColumnHeadersHeight
        End If

        For ii As Integer = 0 To dgv.ColumnCount - 1
            If hsc.Visible Then
                '水平スクロールバー表示
                dgv.Item(ii, 0).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            Else
                dgv.Item(ii, 0).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            End If
        Next

    End Sub

    ''///////////////////////////////////////////////////////////////////////////
    ''イベントハンドラ関数
    ''///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' リサイズ
    ''' </summary>
    ''' <param name="sender">イベント発生元</param>
    ''' <param name="e">イベントパラメータ</param>
    ''' <remarks></remarks>
    Private Sub Ctrl_ReSize(ByVal sender As Object, ByVal e As System.EventArgs)
        dgvWidthResize(dgvCoil)
    End Sub
End Class
