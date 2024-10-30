'*******************************************************************************
'　機器状態トレンド画面
'	[Ver]
'		Ver.01    2014/02/27  初版
'
'	[メモ]
'		機器状態トレンド情報画面の表示
'*******************************************************************************
Option Strict On

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod

Public Class frmKikiTrand
    '/////////////////////////////////////////////////////////////////////////////////
    '定数
    '/////////////////////////////////////////////////////////////////////////////////
    Private Const mc_MAX_GROUP As Integer = 5               'グラフグループ数
    Private Const mc_XScale As Integer = 10                 'X軸スケール間隔(等分)
    Private Const mc_YScale As Integer = 10                 'Y軸スケール間隔(等分)
    Private Const mc_ThinDspCnt As Integer = 5000           '間引き表示数

    ' 初期カラー値
    Private ReadOnly sdcDefault() As System.Drawing.Color = {
        Color.Red, Color.Orange, Color.Yellow, Color.Green, Color.Lime,
        Color.GreenYellow, Color.Blue, Color.RoyalBlue, Color.Cyan, Color.DarkCyan,
        Color.Violet, Color.DeepPink, Color.DarkRed, Color.PaleVioletRed, Color.DarkGreen,
        Color.DarkSeaGreen, Color.PaleGreen, Color.DarkBlue, Color.Purple, Color.Magenta
    }

    '/////////////////////////////////////////////////////////////////////////////////
    '列挙体
    '/////////////////////////////////////////////////////////////////////////////////
    '表示項目列
    Private Enum em_DataColumnType
        iColor = 0                      ' Color
        iCheck                          ' Check
        iName                           ' Name
    End Enum

    '/////////////////////////////////////////////////////////////////////////////////
    '構造体
    '/////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' トレンドグラフ表示テーブル
    ''' </summary>
    ''' <remarks></remarks>
    Private Structure TYP_TREND_DSP
        Dim nNo As Integer                  'No
        Dim nKubun As Integer               '機器区分
        Dim clsGraph As tClass_MapBase      'グラフエリア
        Dim dgvHanrei As DataGridView        '凡例配列
        Dim nudGraphMin As NumericUpDown    '下限値配列
        Dim nudGraphMax As NumericUpDown    '上限値配列
        Dim txtGraphMin As TextBox          '下限値配列
        Dim txtGraphMax As TextBox          '上限値配列
        Dim cmbCamLumi As ComboBox          '機器
        Dim lblTaniMin As Label             '下限値単位
        Dim lblTaniMax As Label             '上限値単位
        Dim grpGraph As GroupBox            'グループ
        Dim picGraph As PictureBox          'グラフ表示エリア
        Dim chkGraphProt As CheckBox        'グラフプロット表示・非表示
        Dim cmbName As ComboBox             ' トレンド名
        Dim nCnt As Integer                 '件数
        Dim nCoilPos() As Integer           '取得位置
        Dim sTime() As String               '時間
        Dim nData(,) As Single              'データ
        Dim nMin As Integer                 '下限値
        Dim nMax As Integer                 '上限値
        Dim nThin As Integer                '間引き数
    End Structure

    ''' <summary>
    ''' 機器トレンド情報
    ''' </summary>
    ''' <remarks></remarks>
    Private Structure TYP_TREND_INF
        Dim nKubun As Integer           '機器区分
        Dim strDispGroup As String      '表示グループ名
        Dim nMin As Integer             '下限値
        Dim nMax As Integer             '上限値
        Dim nDecPlaces As Integer       '小数点位置
        Dim strItemList As ArrayList    '項目名リスト
        'イニシャライズ
        Public Sub initialize()
            nKubun = 0
            strDispGroup = String.Empty
            nMin = 0
            nMax = 0
            strItemList = New ArrayList
        End Sub
    End Structure

    '/////////////////////////////////////////////////////////////////////////////////
    'メンバ 変数
    '/////////////////////////////////////////////////////////////////////////////////
    Private m_bInitDate As Boolean = True                       '日時初期設定[True:設定中、False:設定完]
    Private m_bInitKiki As Boolean = True                       '機器初期設定[True:設定中、False:設定完]
    Private m_bInitComb As Boolean = True                       '機器コンボ初期設定[True:設定中、False:設定完]
    Private m_nKikiCount As Integer                             '機器数
    Private m_typTrendDsp(mc_MAX_GROUP - 1) As TYP_TREND_DSP  'トレンドグラフデータ
    Private m_strStartTime As String                            '開始時間
    Private m_strEndTime As String                              '終了時間
    Private m_nTimeCnt As Integer                               '開始-終了時間
    Private m_bSetEnd As Boolean

    Private m_dctTrendInf As Dictionary(Of Integer, TYP_TREND_INF)
    Private m_nDefKiki(mc_MAX_GROUP - 1) As Integer             '初期表示No

    '/////////////////////////////////////////////////////////////////////////////////
    'プロパティ
    '/////////////////////////////////////////////////////////////////////////////////

    '/////////////////////////////////////////////////////////////////////////////////
    'メンバ 関数
    '/////////////////////////////////////////////////////////////////////////////////
    Public Sub New()

        ' この呼び出しはデザイナーで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。

        Dim s As Size
        s.Width = WIN_WIDTH_MAX
        s.Height = WIN_HEIGHT_MAX
        Me.MaximumSize = s
        s.Width = WIN_WIDTH_MIN
        s.Height = WIN_HEIGHT_MIN
        Me.MinimumSize = s

        m_dctTrendInf = New Dictionary(Of Integer, TYP_TREND_INF)

    End Sub

    ''' <summary>
    ''' フォーム終了処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmKikiTrand_Closing(ByVal sender As Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles MyBase.Closing

        'iniファイル情報書込
        Try
            For ii As Integer = 0 To m_typTrendDsp.Count - 1
                PutTrendDsp(m_typTrendDsp(ii))
            Next ii

            For ii As Integer = 0 To m_typTrendDsp.Count - 1
                'グラフ名報書込
                tMod.WriteIni(SO_GAMENN, "KIKITRANDNAME_" & CStr(ii + 1), LOCALINI_NAME, m_typTrendDsp(ii).cmbName.SelectedValue.ToString)
            Next ii
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("フォーム終了処理 iniファイル情報書込異常[{0}],{1}", ex.Message, Me.Text))
        End Try

    End Sub

    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmKikiTrand_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        Try
            Dim s As System.Windows.Forms.Screen = System.Windows.Forms.Screen.FromControl(Me.Owner)
            Me.Location = s.Bounds.Location
            Me.Height = Me.Owner.Height
            Me.Width = Me.Owner.Width

            'タイトル設定
            Call SetTitleName(Me)

            '遠隔時
            If NOA_ONLINE <> g_NOA Then
                'フォーム状態
                Me.tProp_FromFormatMode = gem_FromFormatMode.iOFFLINE
                Me.tProp_EndButtonEnable = False
            End If

            'プリンタがあれば、画面印刷
            If tClass_HeadCopy.IsCheckPrinter Then
                btnF10.Enabled = True
            Else
                btnF10.Enabled = False
            End If

            '機器情報取得
            GetKikiInf()

            '機器履歴マスターテーブル取得
            GetStHistoryMaster()

            '画面初期設定
            SetInit()

            For ii As Integer = 0 To m_typTrendDsp.Count - 1
                ' コンボボックス選択イベントを設定
                AddHandler m_typTrendDsp(ii).cmbName.SelectedIndexChanged, AddressOf cmbName_SelectedIndexChanged
            Next ii

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("フォームロード異常[{0}],{1}", ex.Message, Me.Text))
        End Try
    End Sub

    ''' <summary>
    ''' この画面の資源解放イベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmKikiTrand_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
        Try
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("コイルトレンド画面 資源解放異常 [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 機器情報取得
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub GetKikiInf()
        Try
            '初期化
            'm_typKikiInf.Clear()
            Dim typ As TYP_TREND_INF = Nothing
            m_dctTrendInf.Clear()

            '機器数取得
            m_nKikiCount = ReadIniInt(SO_GAMENN, "KIKI_COUNT", TASKINI_NAME)

            '機器情報取得
            For ii As Integer = 0 To m_nKikiCount - 1
                '初期化
                Dim nStr As Integer = 1
                Dim nEnd As Integer = 0
                Dim nLen As Integer = 0
                Dim nItem As Integer = 0
                Dim strTmp As String
                Dim bEnd As Boolean = False
                Dim typWork As TYP_TREND_INF = Nothing
                typWork.initialize()
                '機器設定値読込
                Dim strKikiInf As String = ReadIniStr(SO_GAMENN, "KIKI_INF_" & CStr(ii + 1), TASKINI_NAME)
                While True
                    nEnd = InStr(nStr, strKikiInf, ",")
                    If 0 = nEnd Then    '最終項目
                        bEnd = True
                        nLen = strKikiInf.Length - nStr + 1
                    Else                '途中項目
                        nLen = nEnd - nStr
                    End If
                    strTmp = Mid(strKikiInf, nStr, nLen)
                    Select Case nItem
                        Case 0          '区分
                            typWork.nKubun = CInt(strTmp)
                        Case 1          '機器グループ名
                            typWork.strDispGroup = strTmp
                        Case 2          '下限値
                            typWork.nMin = CInt(strTmp)
                        Case 3          '上限値
                            typWork.nMax = CInt(strTmp)
                        Case 4          '小数点位置
                            typWork.nDecPlaces = CInt(strTmp)
                    End Select
                    nItem += 1
                    nStr = nEnd + 1
                    If bEnd Then
                        Exit While
                    End If
                End While
                m_dctTrendInf.Add(ii + 1, typWork)
            Next

            '機器初期表示区分取得
            For ii As Integer = 0 To m_nDefKiki.Count - 1
                m_nDefKiki(ii) = ReadIniInt(SO_GAMENN, "DEF_KIKI_" & CStr(ii + 1), TASKINI_NAME, ii + 1)
            Next

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("機器情報取得エラー[{0}],{1}", ex.Message, Me.Text))
        End Try
    End Sub

    ''' <summary>
    ''' 機器履歴マスターテーブル取得
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub GetStHistoryMaster()
        Dim strSQL As String
        Dim strItemName As ArrayList    '機器マスター項目名リスト
        Dim nNameCount As Integer       '機器マスター名称数
        Dim clsDB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim clsRead As SqlClient.SqlDataReader = Nothing

        'DBオープン
        If Not clsDB.Open() Then
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB接続エラー")
            Exit Sub
        End If

        Try

            '機器マスター項目名リスト
            strItemName = New ArrayList
            strItemName.Clear()

            '機器マスター項目名取得
            strSQL = " SELECT"
            strSQL &= " NAME"
            strSQL &= " FROM  SYS.COLUMNS"
            strSQL &= " WHERE OBJECT_ID = OBJECT_ID('" & DB_ST_HISTORY_MASTER & "')"
            clsRead = clsDB.SelectExecute(strSQL)
            If clsRead Is Nothing Then
                Exit Try
            End If
            While clsRead.Read()
                strItemName.Add(CStr(clsRead.GetValue(0)))
            End While
            clsRead.Close()

            '表示項目名称取得
            nNameCount = 0
            For jj As Integer = 0 To strItemName.Count - 1
                If Mid(strItemName.Item(jj).ToString, 1, 2) = "名称" Then
                    nNameCount += 1
                End If
            Next

            Dim keys As Integer() = New Integer(m_dctTrendInf.Keys.Count - 1) {}
            m_dctTrendInf.Keys.CopyTo(keys, 0)
            For ii As Integer = 0 To keys.Count - 1
                Dim typ As TYP_TREND_INF = m_dctTrendInf(keys(ii))

                strSQL = " SELECT"
                For jj As Integer = 0 To nNameCount - 1
                    If 0 = jj Then
                        strSQL &= " 名称" & CStr(jj + 1)
                    Else
                        strSQL &= ",名称" & CStr(jj + 1)
                    End If
                Next
                strSQL &= " FROM " & DB_ST_HISTORY_MASTER
                strSQL &= " WHERE 区分 = " & typ.nKubun.ToString
                clsRead = clsDB.SelectExecute(strSQL)
                If clsRead Is Nothing Then
                    Exit Try
                End If
                While clsRead.Read()
                    For jj As Integer = 0 To nNameCount - 1
                        If clsRead.IsDBNull(jj) Then
                            Continue For
                        End If
                        If String.Empty <> CStr(clsRead.GetValue(jj)).Trim() Then
                            typ.strItemList.Add(CStr(clsRead.GetValue(jj)))
                        End If
                    Next
                End While
                clsRead.Close()
                m_dctTrendInf.Remove(keys(ii))
                m_dctTrendInf.Add(keys(ii), typ)
            Next

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("機器履歴マスターテーブル取得異常[{0}],{1}", ex.Message, Me.Text))
        Finally
            clsRead.Close()
            clsRead = Nothing
            clsDB.Close()
            clsDB.Dispose()
            clsDB = Nothing
        End Try
    End Sub

    ''' <summary>
    ''' 画面初期設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetInit()

        Dim ii As Integer

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


        'イベントハンドラ関連付け
        For ii = 0 To m_typTrendDsp.Count - 1
            With m_typTrendDsp(ii)
                'マップ初期定義
                .clsGraph = New tClass.tClass_MapBase
                MapInitialize(.clsGraph)

                'コンボボックス設定
                .nNo = tMod.ReadIniInt(SO_GAMENN, "KIKITRANDNAME_" & CStr(ii + 1), LOCALINI_NAME, m_nDefKiki(ii))
                cmbName_Init(.cmbName, .nNo)

                'グリッドの初期設定
                dgvInitialize(.dgvHanrei)

                'イベントハンドラ関連付け
                'マップ
                AddHandler .clsGraph.tEv_DrawPaint_End, AddressOf tEv_DrawPaint_End

                'データグリッドビュー
                AddHandler .dgvHanrei.CellContentClick, AddressOf dgvGraph_CellContentClick
                AddHandler .dgvHanrei.SelectionChanged, AddressOf dgvGraph_SelectionChanged

                'ピクチャーボックス
                AddHandler .picGraph.Paint, AddressOf PictureBox_Paint

                'チェックボックス
                AddHandler .chkGraphProt.CheckedChanged, AddressOf chkGraphProt_CheckedChanged

                'スピン ボックス (アップダウン コントロール)
                AddHandler .nudGraphMin.ValueChanged, AddressOf nudGraphMaxMin_ValueChanged
                AddHandler .nudGraphMax.ValueChanged, AddressOf nudGraphMaxMin_ValueChanged

            End With

        Next ii

        '開始終了時間初期表示
        dtpStartDay.Value = CDate(Format(Now, "yyyy/MM/dd") & " 00:00:00")
        dtpStartTime.Value = CDate(Format(Now, "yyyy/MM/dd") & " 00:00:00")
        dtpEndDay.Value = CDate(Format(Now, "yyyy/MM/dd") & " 23:59:59")
        dtpEndTime.Value = CDate(Format(Now, "yyyy/MM/dd") & " 23:59:59")

    End Sub

    ''' <summary>
    ''' マップ基本情報定義
    ''' </summary>
    ''' <param name="MapBase">グラフマップ</param>
    ''' <remarks></remarks>
    Private Sub MapInitialize(ByVal MapBase As tClass_MapBase)
        'グラフ描画に必要な情報を設定
        With MapBase
            '表示領域情報
            .tProp_AriaInf_BackGroundLineColor = Pens.Black
            .tProp_AriaInf_BackGroundColor = New SolidBrush(Color.White)
            .tProp_AriaInf_MapColor = New SolidBrush(Color.White)
            .tProp_AriaInf_AriaSize = picGraph1.Size
            .tProp_AriaInf_Margin = New Padding(40, 10, 35, 35)

            'マップ領域
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
            .tProp_MapInf_Offset = New tClass_MapBase.xyValue(0, 0)

            'マップ対象外領域設定
            .tProp_LimitInf_LimitDraw = False

            'メモリ設定
            Dim p As Pen = New Pen(Color.LightBlue, 1)
            p.DashStyle = Drawing2D.DashStyle.DashDot
            .tProp_MemoriInf_LinePen = p
            .tProp_MemoriInf_StringFont = New Font("ＭＳ ゴシック", 11, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
            .tProp_MemoriInf_StringBsh = New SolidBrush(Color.Black)
            .tProp_MemoriInf_PosStringX = 10
            .tProp_MemoriInf_PosStringY = 1
            .tProp_MemoriInf_ScaleInterval = New tClass_MapBase.xyValue(100, 10)
            .tProp_MemoriInf_ScaleStringFormatX = ""
            .tProp_MemoriInf_ScaleStringFormatY = "0"
            .tProp_MemoriInf_ScaleStringRange = New tClass_MapBase.RATE(1, 1)
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
    ''' 入力データチェック
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function InputDateCheck() As Boolean
        Dim strMsg As String

        If dtpStartDay.Value.ToString("yyyyMMdd") > dtpEndDay.Value.ToString("yyyyMMdd") Then
            strMsg = String.Format("開始日と終了日が逆転しています。")
            MsgBox(strMsg, MsgBoxStyle.OkOnly, "入力チェック")
            dtpStartDay.Select()
            dtpStartDay.Focus()
            Return False
        ElseIf dtpStartDay.Value.ToString("yyyyMMdd") = dtpEndDay.Value.ToString("yyyyMMdd") Then
            If dtpStartTime.Value.ToString("HHmmss") >= dtpEndTime.Value.ToString("HHmmss") Then
                strMsg = String.Format("開始時間と終了時間が同じか逆転しています。")
                MsgBox(strMsg, MsgBoxStyle.OkOnly, "入力チェック")
                dtpStartTime.Select()
                dtpStartTime.Focus()
                Return False
            End If
        End If
        Return True

    End Function

    ''' <summary>
    ''' マップ再定義
    ''' </summary>
    ''' <param name="typGraph">グラフ表示テーブル</param>
    ''' <remarks></remarks>
    Private Sub MapReSetring(ByVal typGraph As TYP_TREND_DSP)

        'グラフ描画に必要な情報を設定
        With typGraph.clsGraph
            'マップ領域
            .tProp_MapInf_LimitXmax = m_nTimeCnt
            .tProp_MapInf_LimitYmin = CInt(typGraph.nMin)
            .tProp_MapInf_LimitYmax = CInt(typGraph.nMax)

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

        Try
            '行がクリックされていなければ終了
            If 0 > e.RowIndex Then Exit Sub

            '列インデックスでどの列がクリックされたか判別
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
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("グリッドクリック異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' dgvの選択が変更されたら、セルの選択状態をキャンセル
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvGraph_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs)
        Try
            Dim dgv As DataGridView = CType(sender, DataGridView)   '対象のdgv
            dgv.CurrentCell.Selected = False
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("グリッド選択変更異常[{0}]", ex.Message))
        End Try
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

        '色選択ダイアログでokボタンが押されたら、ボタンの色を変更する 
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
        Try
            For ii As Integer = 0 To m_typTrendDsp.Count - 1
                'グラフグループの検索
                If sender Is m_typTrendDsp(ii).picGraph Then
                    'ビットマップ取得
                    Dim bmp As Bitmap = m_typTrendDsp(ii).clsGraph.tProp_Bmp
                    If bmp Is Nothing Then Return

                    '背景をセット
                    e.Graphics.DrawImage(bmp, 0, 0)
                End If
            Next ii
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("描画異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 画面データ描画
    ''' </summary>
    ''' <param name="cls"></param>
    ''' <param name="g"></param>
    ''' <remarks></remarks>
    Private Sub tEv_DrawPaint_End(ByVal cls As tClass_MapBase, ByVal g As System.Drawing.Graphics)
        Dim ii As Integer                   'INDEX
        Dim jj As Integer                   'INDEX
        Dim nPixel As Integer               '凡例X軸位置
        Dim nGraphXInterval As Integer      'X軸表示周期
        Dim nGraphXPoint As Integer         'X軸表示ポインタ
        Dim nGraphYInterval As Integer      'Y軸表示周期
        Dim nGraphYPoint As Integer         'Y軸表示ポインタ
        Dim strWork As String               'ワークエリア
        Dim nTimeCnt As Integer             'ワークエリア 時間数


        ' グラフグループの検索
        For Each typ As TYP_TREND_DSP In m_typTrendDsp
            If cls IsNot typ.clsGraph Then
                Continue For
            End If

            'X軸凡例表示
            'X軸スケール間隔算出
            nGraphXInterval = (m_nTimeCnt - 1) \ mc_XScale
            If 0 <> (m_nTimeCnt - 1) Mod mc_XScale Then
                nGraphXInterval += 1
            End If
            nGraphXPoint = 1
            For jj = 1 To m_nTimeCnt

                If jj = nGraphXPoint Or jj = m_nTimeCnt Then

                    'Xポジション取得
                    nPixel = cls.GetVtoP_X(jj)

                    '最初と最後は縦線を表示しない
                    If 1 <> jj AndAlso m_nTimeCnt <> jj Then
                        '線
                        g.DrawLine(cls.tProp_MemoriInf.LinePen, nPixel, cls.tProp_CalcAns.MapRect.Top + 1, nPixel, cls.tProp_CalcAns.MapRect.Bottom)

                    End If

                    '文字
                    Dim cdtTime As Date = CDate(m_strStartTime).AddSeconds(jj - 1)
                    strWork = Format(cdtTime, "yyyy/MM/dd") & vbNewLine & Format(cdtTime, "HH:mm:ss")
                    cls.DrawString(g, nPixel, cls.tProp_CalcAns.MapRect.Bottom, 0, +cls.tProp_MemoriInf.nPosStringX, strWork, ContentAlignment.BottomCenter, cls.tProp_MemoriInf.StringFont, cls.tProp_MemoriInf.StringBsh)
                    nGraphXPoint += nGraphXInterval
                End If

            Next jj

            ''Y軸凡例表示
            'Y軸スケール間隔算出
            nGraphYInterval = ((typ.nMax - typ.nMin) - 1) \ mc_YScale
            If 0 <> (typ.nMax - typ.nMin) - 1 Mod mc_YScale Then
                nGraphYInterval += 1
            End If
            nGraphYPoint = typ.nMin
            For jj = typ.nMin To typ.nMax
                If jj = nGraphYPoint Or jj = typ.nMax Then

                    'Yポジション取得
                    nPixel = cls.GetVtoP_Y(jj)

                    '最初と最後は横線を表示しない
                    If typ.nMin <> jj AndAlso typ.nMax <> jj Then
                        '線
                        g.DrawLine(cls.tProp_MemoriInf.LinePen, cls.tProp_CalcAns.MapRect.Left + 1, nPixel, cls.tProp_CalcAns.MapRect.Right, nPixel)

                    End If

                    '文字
                    strWork = Format(jj, cls.tProp_MemoriInf.sScaleStringFormatY)
                    cls.DrawString(g, cls.tProp_CalcAns.MapRect.Left, nPixel, -cls.tProp_MemoriInf.nPosStringY, 0, strWork, ContentAlignment.MiddleLeft, cls.tProp_MemoriInf.StringFont, cls.tProp_MemoriInf.StringBsh)
                    nGraphYPoint += nGraphYInterval
                End If

            Next jj

            'グラフ表示データ=0で終了
            If 0 = typ.nCnt Then Return

            '描画
            Dim dVal As Single
            Dim x1, y1, x2, y2 As Integer
            Dim strDspTimeMax As String = ""            '表示算出用 時間 最大
            Dim strDspTimeMin As String = ""            '表示算出用 時間 最小
            Dim nDspDataMax As Single = 0              '表示算出用 データ 最大
            Dim nDspDataMin As Single = 0              '表示算出用 データ 最小
            Dim strDspTime(2 - 1) As String             '表示用 時間
            Dim nDspData(2 - 1) As Single              '表示用 データ
            Dim nThinCnt As Integer = 0                 '間引き用 カウンタ

            For ii = 0 To typ.dgvHanrei.RowCount - 1

                'グラフ表示チェック 非表示で次のグラフ（線）
                If False = CBool(typ.dgvHanrei(em_DataColumnType.iCheck, ii).Value) Then Continue For

                '始点算出 (先頭の座標算出) X軸
                strWork = CStr(CDate(typ.sTime(0)).ToBinary - CDate(m_strStartTime).ToBinary)
                If "0" = strWork Then
                    nTimeCnt = 0
                Else
                    nTimeCnt = CInt(strWork.Remove(strWork.Length - 7))
                End If
                x1 = cls.GetVtoP_X(nTimeCnt + 1)

                '始点算出 (先頭の座標算出) Y軸
                dVal = typ.nData(ii, 0)

                'クランプ設定
                If dVal > typ.nMax Then dVal = typ.nMax
                If dVal < typ.nMin Then dVal = typ.nMin

                y1 = cls.GetVtoP_Y(dVal)

                Using p As New Pen(typ.dgvHanrei(em_DataColumnType.iColor, ii).Style.BackColor, 1)

                    'プロットの指定
                    If typ.chkGraphProt.Checked Then
                        g.FillEllipse(New SolidBrush(p.Color), x1 - 3, y1 - 3, 6, 6)
                    End If

                    For jj = 0 To typ.nCnt - 1

                        If 2 >= typ.nThin Then
                            '間引き設定が２以下の場合
                            '配列片側のみデータを挿入
                            strDspTime(0) = typ.sTime(jj)
                            nDspData(0) = typ.nData(ii, jj)
                            strDspTime(1) = ""
                            nDspData(1) = 0
                        Else
                            If 0 = nThinCnt Then
                                '間引き用カウンタが０(間引き数毎の最初)
                                '最大、最小両方にデータを挿入
                                strDspTimeMax = typ.sTime(jj)
                                nDspDataMax = typ.nData(ii, jj)
                                strDspTimeMin = typ.sTime(jj)
                                nDspDataMin = typ.nData(ii, jj)
                                '間引き用カウンタ インクリメント
                                nThinCnt += 1
                                Continue For
                            ElseIf nThinCnt < typ.nThin Then
                                '間引き用カウンタが０(間引き数毎の最初)
                                If nDspDataMax <= typ.nData(ii, jj) Then
                                    '最大値更新
                                    strDspTimeMax = typ.sTime(jj)
                                    nDspDataMax = typ.nData(ii, jj)
                                ElseIf nDspDataMin >= typ.nData(ii, jj) Then
                                    '最小値更新
                                    strDspTimeMin = typ.sTime(jj)
                                    nDspDataMin = typ.nData(ii, jj)
                                End If
                                '間引き用カウンタ インクリメント
                                nThinCnt += 1
                                If nThinCnt <> typ.nThin And jj < typ.nCnt - 1 Then
                                    '間引き用カウンタが設定値に到達しておらず、データ個数ＭＡＸになっていない
                                    Continue For
                                Else
                                    '時間によって表示する順番を入れ替える
                                    If CDate(strDspTimeMax).ToBinary > CDate(strDspTimeMin).ToBinary Then
                                        strDspTime(0) = strDspTimeMin
                                        nDspData(0) = nDspDataMin
                                        strDspTime(1) = strDspTimeMax
                                        nDspData(1) = nDspDataMax
                                    Else
                                        strDspTime(0) = strDspTimeMax
                                        nDspData(0) = nDspDataMax
                                        strDspTime(1) = strDspTimeMin
                                        nDspData(1) = nDspDataMin
                                    End If
                                    nThinCnt = 0
                                End If
                            End If
                        End If

                        For kk As Integer = 0 To 2 - 1
                            'データが入っていない場合次へ
                            If strDspTime(kk) = "" Then Exit For
                            strWork = CStr(CDate(strDspTime(kk)).ToBinary - CDate(m_strStartTime).ToBinary)
                            If "0" = strWork Then
                                nTimeCnt = 0
                            Else
                                nTimeCnt = CInt(strWork.Remove(strWork.Length - 7))
                            End If
                            x2 = cls.GetVtoP_X(nTimeCnt + 1)

                            dVal = nDspData(kk)

                            If dVal > typ.nMax Then dVal = typ.nMax
                            If dVal < typ.nMin Then dVal = typ.nMin
                            y2 = cls.GetVtoP_Y(dVal)

                            '描画
                            g.DrawLine(p, x1, y1, x2, y2)

                            'プロットの指定
                            If typ.chkGraphProt.Checked Then
                                g.FillEllipse(New SolidBrush(p.Color), x2 - 3, y2 - 3, 6, 6)
                            End If

                            '次の準備
                            x1 = x2
                            y1 = y2
                        Next kk
                    Next jj
                End Using
            Next ii
            Exit For
        Next

    End Sub

    ''' <summary>
    ''' プロット表示チェック状態変更
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chkGraphProt_CheckedChanged(ByVal sender As Object, ByVal e As System.EventArgs)
        Try
            For ii As Integer = 0 To m_typTrendDsp.Count - 1
                'グラフグループの検索
                If sender Is m_typTrendDsp(ii).chkGraphProt Then
                    If 0 < m_typTrendDsp(ii).nCnt Then
                        'マップ再生成依頼
                        m_typTrendDsp(ii).clsGraph.DrawPaint()

                        'マップ再描画
                        m_typTrendDsp(ii).picGraph.Refresh()

                        Exit For
                    End If
                End If
            Next ii
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("プロット表示変更異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' フォーム初期表示
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmKikiTrand_Shown(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Shown
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
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("フォーム初期表示異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 画面初期表示
    ''' </summary>
    ''' <param name="typDsp">グラフ表示テーブル</param>
    ''' <remarks></remarks>
    Private Sub DspInit(ByRef typDsp As TYP_TREND_DSP)

        With typDsp

            If Not m_dctTrendInf.ContainsKey(.nNo) Then
                '存在しない？
                Exit Sub
            End If

            Dim inf As TYP_TREND_INF = m_dctTrendInf(.nNo)

            ' 凡例内容表示
            .dgvHanrei.RowCount = inf.strItemList.Count

            For ii As Integer = 0 To inf.strItemList.Count - 1

                ' 表示・非表示
                Dim nDisp As Integer = tMod.ReadIniInt(SO_GAMENN, "KIKITRANDDISP_" & CStr(.nNo) & "_" & CStr(ii + 1), LOCALINI_NAME, 1)
                If 1 = nDisp Or 0 = nDisp Then
                    .dgvHanrei(em_DataColumnType.iCheck, ii).Value = nDisp
                Else
                    .dgvHanrei(em_DataColumnType.iCheck, ii).Value = 1
                End If

                ' 表示色
                Dim strColor As String = tMod.ReadIniStr(SO_GAMENN, "KIKITRANDCOLOR_" & CStr(.nNo) & "_" & CStr(ii + 1), LOCALINI_NAME)
                If "" <> strColor Then
                    .dgvHanrei(em_DataColumnType.iColor, ii).Style.BackColor = Color.FromArgb(Convert.ToInt32("FF" & strColor, 16))
                Else
                    .dgvHanrei(em_DataColumnType.iColor, ii).Style.BackColor = Color.FromArgb(Convert.ToInt32(Hex(sdcDefault(ii).ToArgb), 16))
                End If

                ' 名称
                .dgvHanrei(em_DataColumnType.iName, ii).Value = inf.strItemList.Item(ii)
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
            .nudGraphMax.Text = tMod.ReadIniStr(SO_GAMENN, "KIKITRANDMAX_" & CStr(.nNo), LOCALINI_NAME)
            If False = IsNumeric(.nudGraphMax.Text) Then
                .nudGraphMax.Text = inf.nMax.ToString
            End If

            ' 下限値
            .nudGraphMin.Text = tMod.ReadIniStr(SO_GAMENN, "KIKITRANDMIN_" & CStr(.nNo), LOCALINI_NAME)
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
                tMod.WriteIni(SO_GAMENN, "KIKITRANDDISP_" & CStr(typDsp.nNo) & "_" & CStr(ii + 1), LOCALINI_NAME, "1")
            Else
                tMod.WriteIni(SO_GAMENN, "KIKITRANDDISP_" & CStr(typDsp.nNo) & "_" & CStr(ii + 1), LOCALINI_NAME, "0")
            End If
        Next

        For ii As Integer = 0 To typDsp.dgvHanrei.RowCount - 1
            '表示色情報書込
            strWk = Microsoft.VisualBasic.Right(Hex(typDsp.dgvHanrei(em_DataColumnType.iColor, ii).Style.BackColor.ToArgb), 6)
            tMod.WriteIni(SO_GAMENN, "KIKITRANDCOLOR_" & CStr(typDsp.nNo) & "_" & CStr(ii + 1), LOCALINI_NAME, strWk)
        Next

        '上下限入力値チェック
        Dim strMin As String = CStr(typDsp.nudGraphMin.Value)
        Dim strMax As String = CStr(typDsp.nudGraphMax.Value)

        If False = tClass.tClass_ImputCheck.CheckMinMax(strMin, strMax, 0, False, typDsp.cmbName.Text, False) Then
            'NumericUpDownで異常時の場合は、書込みを行わない
        Else
            'グラフ上限値書込
            tMod.WriteIni(SO_GAMENN, "KIKITRANDMAX_" & CStr(typDsp.nNo), LOCALINI_NAME, typDsp.nudGraphMax.Text)

            'グラフ下限値書込
            tMod.WriteIni(SO_GAMENN, "KIKITRANDMIN_" & CStr(typDsp.nNo), LOCALINI_NAME, typDsp.nudGraphMin.Text)
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
    Private Sub frmKikiTrand_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        'ファンクションボタン有効項目のみ
        If Not enable Then
            Exit Sub
        End If

        Try
            Select Case FncNo
                Case 4                  'F05    検索
                    Search()
                Case 8                  'F09    CSV出力
                    CsvOutTrand(CSV_PATH, "機器状態トレンド画面.csv")
                Case 9                  'F10    画面印刷
                    PrintOut()
                Case 10                 'F11    画面保存
                    HardCopy(IMG_PATH, "機器状態トレンド画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
                Case 11                 'F12    メニュー
                    MyBase.ExecFromClose()
            End Select
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("ファンクション押下異常[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 日付コントロールに対して ホイールで加算減算が可能とする
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dtp_MouseWheel(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles dtpStartDay.MouseWheel, dtpEndDay.MouseWheel, dtpStartTime.MouseWheel, dtpEndTime.MouseWheel
        '現在のdatetimepicker に↑↓キーを押された事とする
        SendKeys.Send(CStr(IIf(e.Delta > 0, "{DOWN}", "{UP}")))
    End Sub

    '***************************************************************************
    'モジュール
    '***************************************************************************
    ''' <summary>
    ''' 動作定義
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Private Sub DgvInit(ByVal dgv As DataGridView)

        With dgv
            .ColumnCount = 0
            .AutoGenerateColumns = False                          '列自動生成 (falseにするとDataSourceが効かなくなるので注意)
            .AllowUserToAddRows = False                           'ユーザー 行追加
            .AllowUserToDeleteRows = False                        'ユーザー 行削除
            .AllowUserToOrderColumns = False                      'ユーザー 列移動
            .AllowUserToResizeColumns = False                     'ユーザー 列サイズ変更
            .AllowUserToResizeRows = False                        'ユーザー 行サイズ変更
            .ShowCellErrors = False                               'セルエラーの表示
            .ShowCellToolTips = False                             'ツールヒントの表示
            .ShowEditingIcon = False                              '編集中のセルの明示
            .ShowRowErrors = False                                'エラー行の明示
            .ScrollBars = ScrollBars.Both                         'スクロールバーの表示
            .ColumnHeadersHeight = 42                             'ヘッダーの高さ
            .SelectionMode = DataGridViewSelectionMode.CellSelect 'セル選択
            .MultiSelect = False                                  '複数行選択
            .ReadOnly = False                                     '読み取り専用はダメ
            .EditMode = DataGridViewEditMode.EditOnEnter          'セル書き換え
            .ColumnHeadersVisible = False                         '列ヘッダ表示
            .RowHeadersVisible = False                            '行ヘッダ表示
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None '可視列サイズ自動調整
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None '可視行サイズ自動調整
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

            '列ヘッダー 文字サイズ変更,文字色変更
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            '選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor

            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  '1行おきに色替え
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
    Private Sub dgvInitialize(ByVal dgv As DataGridView)

        '動作定義
        DgvInit(dgv)

        With dgv
            '列定義
            '1列目
            Using clm As New DataGridViewButtonColumn
                clm.HeaderText = "表示色"
                clm.Width = 45
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                clm.DefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                .Columns.Add(clm)
            End Using

            '2列目
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

            '3列目
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "名称"
                clm.Width = 450
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
    ''' 検索
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub Search()

        '入力データチェック
        If False = InputDateCheck() Then Exit Sub

        '時間算出
        '開始時間取得
        m_strStartTime = dtpStartDay.Value.ToString("yyyy/MM/dd ") & dtpStartTime.Value.ToString("HH:mm:ss")
        '終了時間取得
        m_strEndTime = dtpEndDay.Value.ToString("yyyy/MM/dd ") & dtpEndTime.Value.ToString("HH:mm:ss")
        '開始-終了時間取得
        Dim nFindTime As Long = CDate(m_strEndTime).ToBinary - CDate(m_strStartTime).ToBinary

        m_nTimeCnt = CInt(nFindTime \ 10000000) + 1

        For ii As Integer = 0 To m_typTrendDsp.Count - 1
            SetDsp(m_typTrendDsp(ii))
        Next ii

    End Sub

    ''' <summary>
    ''' プロットデータ取得
    ''' </summary>
    ''' <param name="typGraph">グラフ表示テーブル</param>
    ''' <remarks></remarks>
    Private Sub GetProtData(ByRef typGraph As TYP_TREND_DSP)

        Dim nGrpNo As Integer = typGraph.nNo    'グラフグループＮｏ
        Dim nGrpNoCnt As Integer                'INDEX
        Dim strSQL As String                    'SQL文
        Dim sTime() As String                   'DB取得用 登録日付
        Dim nData(,) As Single                 'DB取得用 機器状態
        Dim nCnt As Integer                     'DB取得件数


        typGraph.nCnt = nCnt

        If Not m_dctTrendInf.ContainsKey(typGraph.nNo) Then
            '存在しない？
            Exit Sub
        End If

        Dim inf As TYP_TREND_INF = m_dctTrendInf(typGraph.nNo)

        'グラフデータ初期化
        'SQL接続準備
        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        If Not tcls_DB.Open() Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "DB接続エラー")
            Exit Sub
        End If

        'SQL文生成
        Dim strKbn As String = String.Empty
        strKbn = inf.nKubun.ToString()
        strSQL = ""
        strSQL += " SELECT COUNT(*)"
        strSQL += " FROM " & DB_ST_HISTORY_INF
        strSQL += " WHERE 区分 = '" & strKbn & "'"
        strSQL += " AND 登録日付 >= '" & m_strStartTime & "'"
        strSQL += " AND 登録日付 <= '" & m_strEndTime & "'"

        'データ個数取得
        tcls_DB.GetSelectKey(strSQL, nCnt)

        'SQL文生成
        strSQL = ""
        strSQL += " SELECT 登録日付,"
        For nGrpNoCnt = 0 To typGraph.dgvHanrei.RowCount - 1
            strSQL += "        機器状態_" & CStr(nGrpNoCnt + 1) & ","
        Next nGrpNoCnt
        strSQL = strSQL.Remove(strSQL.Length - 1, 1)
        strSQL += " FROM " & DB_ST_HISTORY_INF
        strSQL += " WHERE 区分 = '" & strKbn & "'"
        strSQL += " AND 登録日付 >= '" & m_strStartTime & "'"
        strSQL += " AND 登録日付 <= '" & m_strEndTime & "'"
        strSQL += " ORDER BY 登録日付 ASC"

        'データ取得
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "DB読み込みエラー [" & DB_ST_HISTORY_INF & "]")
            Exit Sub
        End If

        ReDim Preserve sTime(nCnt - 1)
        ReDim Preserve nData(typGraph.dgvHanrei.RowCount - 1, nCnt - 1)

        nCnt = 0
        Try
            While sqlRead.Read()
                '登録日付取得
                sTime(nCnt) = sqlRead.GetValue(0).ToString
                For nGrpNoCnt = 0 To typGraph.dgvHanrei.RowCount - 1
                    '機器状態 取得
                    If IsNumeric(sqlRead.GetValue(nGrpNoCnt + 1).ToString) Then
                        If inf.nDecPlaces > 0 Then
                            nData(nGrpNoCnt, nCnt) = CInt(sqlRead.GetValue(nGrpNoCnt + 1)) * CDec(1 / (10 ^ inf.nDecPlaces))
                        Else
                            nData(nGrpNoCnt, nCnt) = CInt(sqlRead.GetValue(nGrpNoCnt + 1).ToString)
                        End If
                    Else
                        nData(nGrpNoCnt, nCnt) = 0
                    End If
                Next nGrpNoCnt

                '取得件数インクリメント
                nCnt += 1
            End While
        Catch ex As Exception
            '異常時取得件数=0
            nCnt = 0
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("プロットデータ取得エラー[{0}],{1}", ex.Message, Me.Text))
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

        typGraph.nCnt = nCnt
        typGraph.sTime = sTime
        typGraph.nData = nData
        typGraph.nThin = CInt(nCnt \ mc_ThinDspCnt)
        If 0 <> (nCnt Mod mc_ThinDspCnt) Then
            typGraph.nThin += 1
        End If

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

                m_typTrendDsp(ii).nNo = CType(cmbData.SelectedValue, Integer)

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

        Dim strFPath As String                          'フルパス
        Dim strMsg As String = ""                       '通知メッセージ
        Dim strData As String = ""                      '1行のデータ
        Dim nn As Integer                               'INDEX
        Dim nCntMax As Integer = 0                      'データ件数MAX

        ''フルパス
        strFPath = IO.Path.Combine(strFolder, strName)

        ''フォルダの有無チェック(無かったら作ろう)
        CheckFolder(strFolder)

        ''CSV出力
        Using cls_Stram As New tClass_FileStream(strFPath, IO.FileMode.Create, IO.FileAccess.ReadWrite, IO.FileShare.ReadWrite)
            Try
                'オープン
                cls_Stram.Open()

                strData = ""
                'ヘッダー出力(トレンド名）
                For ii As Integer = 0 To mc_MAX_GROUP - 1
                    strData += m_typTrendDsp(ii).cmbName.Text & ","
                    For jj As Integer = 0 To m_typTrendDsp(ii).dgvHanrei.RowCount - 1
                        strData += ","
                    Next jj
                    strData += ","
                Next ii

                '最後の","を排除
                strData = strData.Remove(strData.Length - 2, 2)

                'ＣＳＶへ書き込み
                cls_Stram.WriteLine(strData)

                strData = ""
                'ヘッダー出力(凡例名）
                For ii As Integer = 0 To mc_MAX_GROUP - 1
                    strData += "日付,"
                    For jj As Integer = 0 To m_typTrendDsp(ii).dgvHanrei.RowCount - 1
                        strData += m_typTrendDsp(ii).dgvHanrei(em_DataColumnType.iName, jj).Value.ToString
                        strData += ","
                    Next jj
                    strData += ","
                Next ii

                '最後の","を排除
                strData = strData.Remove(strData.Length - 2, 2)

                'ＣＳＶへ書き込み
                cls_Stram.WriteLine(strData)

                'データ件数の最大値取得
                For ii As Integer = 0 To mc_MAX_GROUP - 1
                    If nCntMax < m_typTrendDsp(ii).nCnt Then
                        nCntMax = m_typTrendDsp(ii).nCnt
                    End If
                Next ii

                'データ出力
                nn = 0
                While nCntMax > nn
                    strData = ""
                    For ii As Integer = 0 To mc_MAX_GROUP - 1
                        'データ出力件数がデータ最大数以下？
                        If m_typTrendDsp(ii).nCnt > nn Then
                            'データの設定
                            strData += m_typTrendDsp(ii).sTime(nn)
                            strData += ","
                            For jj As Integer = 0 To m_typTrendDsp(ii).dgvHanrei.RowCount - 1
                                strData += CStr(m_typTrendDsp(ii).nData(jj, nn))
                                strData += ","
                            Next jj
                            strData += ","
                        Else
                            '空白の設定
                            strData += ","
                            For jj As Integer = 0 To m_typTrendDsp(ii).dgvHanrei.RowCount - 1
                                strData += ","
                            Next jj
                            strData += ","
                        End If

                    Next ii
                    '最後の","を排除
                    strData = strData.Remove(strData.Length - 2, 2)

                    'ＣＳＶへ書き込み
                    cls_Stram.WriteLine(strData)

                    nn += 1
                    '
                End While

                'クローズ
                cls_Stram.Close()
                strMsg = System.IO.Path.GetFullPath(strFPath) & "にCSVファイルを保存しました。"

            Catch ex As Exception
                gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("CSV出力異常[{0}],{1}", ex.Message, Me.Text))
            End Try
        End Using

        'ガイダンス表示
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
    ''' <param name="nNo"></param>
    ''' <remarks></remarks>
    Private Sub cmbName_Init(ByVal sender As ComboBox, ByVal nNo As Integer)

        Dim ds As New DataTable()
        ds.Columns.Add("NAME", GetType(String))
        ds.Columns.Add("VALUE", GetType(Integer))

        'トレンド情報に登録されているグループ名
        Dim keys As Integer() = New Integer(m_dctTrendInf.Keys.Count - 1) {}
        m_dctTrendInf.Keys.CopyTo(keys, 0)
        For ii As Integer = 0 To keys.Count - 1
            Dim typ As TYP_TREND_INF = m_dctTrendInf(keys(ii))
            Dim row As DataRow = ds.NewRow()
            row("NAME") = typ.strDispGroup
            row("VALUE") = keys(ii)
            ds.Rows.Add(row)
        Next

        ds.AcceptChanges()
        sender.Items.Clear()
        sender.DataSource = ds
        sender.DisplayMember = "NAME"
        sender.ValueMember = "VALUE"
        sender.DropDownStyle = ComboBoxStyle.DropDownList
        sender.SelectedValue = nNo

    End Sub

End Class
