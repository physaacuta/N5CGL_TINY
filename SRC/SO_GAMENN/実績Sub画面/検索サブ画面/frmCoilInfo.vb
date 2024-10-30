'*********************************************************************************
'　コイル情報送信伝文クラス
'	[Ver]
'		Ver.01    2016/09/01  初版
'
'	[メモ]
'		
'*********************************************************************************
'Option Strict On
Option Strict Off

Imports tClassLibrary

Public Class frmCoilInfo

    ' 表示項目アイテム
    Structure ITEM
        Dim name As String
        Dim size As Integer
        Dim type As System.Type     ' 将来用 0:byte 1:short 2:int 3:char など・・・

        Public Overrides Function ToString() As String
            Return name
        End Function

        Public Sub New(ByVal name As String, ByVal size As Integer, ByVal type As System.Type)
            Me.name = name
            Me.size = size
            Me.type = type
        End Sub

    End Structure


    Private m_bRaw As Byte()                '生データ
    Private m_lstName As New List(Of ITEM)  '項目名称一覧
    Private m_mode As Integer               '要求モード(1:オンライン用、0:ツール用）
    Private m_strKizukenNo As String        '管理No
    Private m_nUpNum As Integer = 57        '上下切替位置

    ''' <summary>
    ''' パラメータに要求モード(1:オンライン用、0:ツール用）
    ''' </summary>
    ''' <param name="mode">1:オンライン用、0:ツール用</param>
    ''' <param name="strKizukenNo">出側管理No</param>
    ''' <remarks></remarks>
    Public Sub New(ByVal mode As Integer, Optional ByVal strKizukenNo As String = "")
        ' この呼び出しは、Windows フォーム デザイナで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。
        'メンバ変数セット
        m_mode = mode                   'パラメータに要求モード(1:オンライン用、0:ツール用）
        m_strKizukenNo = strKizukenNo   '出側管理No

        '-------------------------------------------------------------------
        ' ファンクションキー設定
        '-------------------------------------------------------------------
        AddFunctionButton(btnEnd, 12 - 1)
    End Sub


    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmCoil_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        Me.tProp_Moveable = True
        Me.TopMost = False

        'タイトル設定
        Call SetTitleName(Me)

        'ポップアップ画面 低解像度対応
        Call SetPcHeight(Me)

        SetNameList()

        'DGV初期化
        dgvInitialize(dgvCoilInfo, True)
        AddHandler dgvCoilInfo.ColumnHeaderMouseDoubleClick, AddressOf dgvColumnHeader_DoubleClick
        dgvInitialize(dgvNextCoilInfo, False)
        AddHandler dgvNextCoilInfo.ColumnHeaderMouseDoubleClick, AddressOf dgvColumnHeader_DoubleClick

        MakeIDList()

        If 1 = m_mode Then                  ' オンラインモード
            btnRead.Visible = False         ' 読込ボタン「非表示」
            cmbID.DropDownStyle = ComboBoxStyle.DropDownList
            If cmbID.Items.Count <> 0 Then
                cmbID.SelectedIndex = 0
            End If
        Else                                ' ツールモード
            btnRead.Visible = True          ' 読込ボタン「表示」
        End If

    End Sub


    ''' <summary>
    ''' 表示する項目名をセットする
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetNameList()
        m_lstName.Add(New ITEM("サブヘッダー", 2, GetType(Byte)))
        m_lstName.Add(New ITEM("データ長", 2, GetType(Short)))
        m_lstName.Add(New ITEM("ＴＣ", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("伝文区分", 2, GetType(Short)))
        m_lstName.Add(New ITEM("送信時刻", 12, GetType(Short)))
        m_lstName.Add(New ITEM("グラントフラグ", 2, GetType(Short)))
        m_lstName.Add(New ITEM("予備", 40, GetType(Char)))
        m_lstName.Add(New ITEM("命令No", PRO_SIZE_ORDER_NO, GetType(Char)))
        m_lstName.Add(New ITEM("生産No", PRO_SIZE_COIL_NO, GetType(Char)))

        m_lstName.Add(New ITEM("送信位置", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("コイル長", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("材料板厚", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("材料板幅", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("注文板厚", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("注文板幅", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("現工程トリム幅", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("SPM伸び率", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("TLV伸び率", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("入側フロント端板長さ", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("入側テール端板長さ", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("SPMワークロール径 (上)", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("SPMワークロール径 (下)", 4, GetType(Int32)))

        m_lstName.Add(New ITEM("予備", 124, GetType(Char)))
        m_lstName.Add(New ITEM("検査基準 表", 4, GetType(Char)))
        m_lstName.Add(New ITEM("検査基準 裏", 4, GetType(Char)))
        m_lstName.Add(New ITEM("クロメート命令 種類", 2, GetType(Char)))
        m_lstName.Add(New ITEM("クロメート命令 有無", 2, GetType(Char)))
        m_lstName.Add(New ITEM("品種", 1, GetType(Char)))
        m_lstName.Add(New ITEM("メッキ種類", 1, GetType(Char)))
        m_lstName.Add(New ITEM("主材質", 2, GetType(Char)))
        m_lstName.Add(New ITEM("ディファレンシャルマーク", 1, GetType(Char)))
        m_lstName.Add(New ITEM("原板区分", 1, GetType(Char)))
        m_lstName.Add(New ITEM("メッキサイクル", 1, GetType(Char)))
        m_lstName.Add(New ITEM("レベラー インラインSPM", 1, GetType(Char)))
        m_lstName.Add(New ITEM("化学処理塗油", 1, GetType(Char)))
        m_lstName.Add(New ITEM("オフラインSPM圧下率", 1, GetType(Char)))
        m_lstName.Add(New ITEM("メッキ区分", 1, GetType(Char)))
        m_lstName.Add(New ITEM("表面処理1桁目", 1, GetType(Char)))
        m_lstName.Add(New ITEM("表面処理2桁目", 1, GetType(Char)))
        m_lstName.Add(New ITEM("塗油", 1, GetType(Char)))
        m_lstName.Add(New ITEM("工程記号予備", 1, GetType(Char)))
        m_lstName.Add(New ITEM("バウンダリ合わせ", 1, GetType(Char)))
        m_lstName.Add(New ITEM("CAPL主材質", 10, GetType(Char)))
        m_lstName.Add(New ITEM("目付規格", 8, GetType(Char)))
        m_lstName.Add(New ITEM("Ni付着量", 2, GetType(Char)))
        m_lstName.Add(New ITEM("TCMロール粗度区分", 2, GetType(Char)))
        m_lstName.Add(New ITEM("SPM圧下有無", 2, GetType(Char)))
        m_lstName.Add(New ITEM("計画次工程コード", 2, GetType(Char)))
        m_lstName.Add(New ITEM("向先", 32, GetType(Char)))
        m_lstName.Add(New ITEM("用途", 4, GetType(Char)))
        m_lstName.Add(New ITEM("CAPL表面仕上げ", 4, GetType(Char)))
        m_lstName.Add(New ITEM("CAPL酸洗有無", 2, GetType(Char)))
        m_lstName.Add(New ITEM("品種区分", 1, GetType(Char)))
        m_lstName.Add(New ITEM("GA/CR区分", 1, GetType(Char)))
        m_lstName.Add(New ITEM("SPM圧下実績有無", 2, GetType(Char)))
        m_lstName.Add(New ITEM("工程コード", 2, GetType(Char)))
        m_lstName.Add(New ITEM("予備", 42, GetType(Char)))

        m_lstName.Add(New ITEM("2コイル目" & vbCrLf & "命令No", 4, GetType(Char)))
        m_lstName.Add(New ITEM("2コイル目" & vbCrLf & "生産No", 12, GetType(Char)))
        m_lstName.Add(New ITEM("2コイル目" & vbCrLf & "入側フロント端板長", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("2コイル目" & vbCrLf & "工程コード", 2, GetType(Char)))
        m_lstName.Add(New ITEM("2コイル目" & vbCrLf & "予備", 2, GetType(Char)))

        m_lstName.Add(New ITEM("3コイル目" & vbCrLf & "命令No", 4, GetType(Char)))
        m_lstName.Add(New ITEM("3コイル目" & vbCrLf & "生産No", 12, GetType(Char)))
        m_lstName.Add(New ITEM("3コイル目" & vbCrLf & "入側フロント端板長", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("3コイル目" & vbCrLf & "工程コード", 2, GetType(Char)))
        m_lstName.Add(New ITEM("3コイル目" & vbCrLf & "予備", 2, GetType(Char)))

        m_lstName.Add(New ITEM("4コイル目" & vbCrLf & "命令No", 4, GetType(Char)))
        m_lstName.Add(New ITEM("4コイル目" & vbCrLf & "生産No", 12, GetType(Char)))
        m_lstName.Add(New ITEM("4コイル目" & vbCrLf & "入側フロント端板長", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("4コイル目" & vbCrLf & "工程コード", 2, GetType(Char)))
        m_lstName.Add(New ITEM("4コイル目" & vbCrLf & "予備", 2, GetType(Char)))

        m_lstName.Add(New ITEM("5コイル目" & vbCrLf & "命令No", 4, GetType(Char)))
        m_lstName.Add(New ITEM("5コイル目" & vbCrLf & "生産No", 12, GetType(Char)))
        m_lstName.Add(New ITEM("5コイル目" & vbCrLf & "入側フロント端板長", 4, GetType(Int32)))
        m_lstName.Add(New ITEM("5コイル目" & vbCrLf & "工程コード", 2, GetType(Char)))
        m_lstName.Add(New ITEM("5コイル目" & vbCrLf & "予備", 2, GetType(Char)))

        m_lstName.Add(New ITEM("予備", 16, GetType(Char)))

    End Sub

    ''' <summary>
    ''' IDリスト作成
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub MakeIDList()

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文

        strSQL = ""

        If m_mode = 1 Then           'オンラインモード
            ' オンラインモードの場合は、管理No.から検索する
            strSQL &= " SELECT A.ID"
            strSQL &= " FROM " & DB_BUF_PRO_R_COILINF & " as A"
            strSQL &= " INNER JOIN " & DB_COIL_INF & " as B"
            strSQL &= " ON A.キー情報 = B.キー情報"
            strSQL &= " AND B.管理No = '" & m_strKizukenNo & "'"
            strSQL &= " ORDER BY A.登録日付 DESC"
        Else                         'ツールモード
            'strSQL &= " SELECT TOP 200 ID"
            strSQL &= " SELECT ID"
            strSQL &= " FROM " & DB_BUF_PRO_R_COILINF
            strSQL &= " ORDER BY 登録日付 DESC"
        End If

        'DBオープン
        If Not tcls_DB.Open() Then
            Exit Sub
        End If

        ' Select文実行
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try
            cmbID.Items.Clear()
            ' データ取得
            While sqlRead.Read()

                cmbID.Items.Add(sqlRead.GetValue(0).ToString)

            End While

        Catch ex As Exception
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try


    End Sub

    ''' <summary>
    ''' グリッドの初期化
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitialize(ByVal dgv As DataGridView, ByVal bUpper As Boolean)
        Dim ii As Integer = 0

        With dgv
            .Rows.Clear()
            ' 動作定義
            .AutoGenerateColumns = False                                    ' 列自動生成 (falseにするとDataSourceが効かなくなるので注意)
            .AllowUserToAddRows = False                                     ' ユーザー 行追加
            .AllowUserToDeleteRows = False                                  ' ユーザー 行削除
            .AllowUserToOrderColumns = False                                ' ユーザー 列移動
            .AllowUserToResizeColumns = False                               ' ユーザー 列サイズ変更
            .AllowUserToResizeRows = False                                  ' ユーザー 行サイズ変更
            .ShowCellErrors = False
            .ShowCellToolTips = False
            .ShowEditingIcon = False
            .ShowRowErrors = False
            .ScrollBars = ScrollBars.Both

            .SelectionMode = DataGridViewSelectionMode.FullRowSelect        ' 選択モード(行選択)
            .MultiSelect = False                                            ' 複数行選択
            .ReadOnly = True                                                ' 読み取り専用
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' セル書き換え

            .ColumnHeadersVisible = True                                    ' 列ヘッダ表示
            .RowHeadersVisible = False                                      ' 行ヘッダ表示
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill     ' 可視列サイズ自動調整
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.AllCells       ' 可視行サイズ自動調整
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

            ' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' 選択
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue      ' 1行おきに色替え
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            ' 列定義
            .ColumnCount = 6                                                'textbox のみを追加する場合はこれの方が楽
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .Columns(0).Width = 45
            .Columns(0).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(0).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(0).Resizable = DataGridViewTriState.False

            .Columns(1).Width = 190
            .Columns(1).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(1).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(1).Resizable = DataGridViewTriState.False

            .Columns(2).Width = 100
            .Columns(2).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(2).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(2).Visible = False
            .Columns(2).Resizable = DataGridViewTriState.False

            .Columns(3).Width = 45
            .Columns(3).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(3).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(3).Resizable = DataGridViewTriState.False

            .Columns(4).Width = 180
            .Columns(4).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(4).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(4).Resizable = DataGridViewTriState.True

            .Columns(5).Width = 435
            .Columns(5).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(5).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(5).Resizable = DataGridViewTriState.True

            .Columns(0).HeaderText = "Index"
            .Columns(1).HeaderText = "名称"
            .Columns(2).HeaderText = "型"
            .Columns(3).HeaderText = "Size"
            .Columns(4).HeaderText = "値"
            .Columns(5).HeaderText = "HEX"

        End With

        Dim nStart As Integer
        If bUpper Then
            dgv.RowCount = m_nUpNum
            nStart = 0
        Else
            dgv.RowCount = m_lstName.Count - m_nUpNum
            nStart = m_nUpNum
        End If

        For ii = 0 To dgv.RowCount - 1
            dgv(0, ii).Value = GetPos(nStart + ii).ToString
            dgv(1, ii).Value = m_lstName(nStart + ii)
            dgv(2, ii).Value = m_lstName(nStart + ii).type.ToString
            dgv(3, ii).Value = m_lstName(nStart + ii).size.ToString
        Next ii

    End Sub

    ''' <summary>
    ''' データ(Hex)取得
    ''' </summary>
    ''' <param name="idx"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetHex(ByVal idx As Integer) As String
        Dim sRet As String = ""

        If idx < 0 Or m_lstName.Count < idx Then Return ""

        Dim start As Integer = GetPos(idx)
        Dim size As Integer = m_lstName(idx).size

        If m_bRaw.Length < start + size Then Return ""

        For ii As Integer = 0 To size - 1
            sRet &= String.Format("{0:X2} ", m_bRaw(start + ii))
            If ii Mod 16 = 0 Then
                sRet &= vbCrLf
            End If
        Next

        Return sRet
    End Function


    ''' <summary>
    ''' データ(値)取得
    ''' </summary>
    ''' <param name="idx"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetValue(ByVal idx As Integer) As String
        Dim sRet As String = ""

        If idx < 0 Or m_lstName.Count < idx Then Return ""

        Dim start As Integer = GetPos(idx)
        Dim size As Integer = m_lstName(idx).size

        If m_bRaw.Length < start + size Then Return ""

        If m_lstName(idx).type Is GetType(Char) Then
            sRet = System.Text.ASCIIEncoding.ASCII.GetString(m_bRaw, start, size)
        ElseIf m_lstName(idx).type Is GetType(Short) Then
            Dim svalue As Short
            svalue = System.BitConverter.ToInt16(m_bRaw, start)
            sRet = svalue.ToString
        ElseIf m_lstName(idx).type Is GetType(Integer) Then
            Dim ivalue As Integer
            ivalue = System.BitConverter.ToInt32(m_bRaw, start)
            sRet = ivalue.ToString
        ElseIf m_lstName(idx).type Is GetType(Byte) Then
            sRet = GetHex(idx)
        End If

        Return sRet

    End Function


    ''' <summary>
    ''' 日時取得
    ''' </summary>
    ''' <param name="idx"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetTimeValue(ByVal idx As Integer) As String


        If idx < 0 Or m_lstName.Count < idx Then Return ""

        Dim start As Integer = GetPos(idx)
        Dim size As Integer = m_lstName(idx).size
        Dim sRet(size / 2) As String

        If m_bRaw.Length < start + size Then Return ""

        For ii As Integer = 0 To size / 2
            Dim svalue As Short
            svalue = System.BitConverter.ToInt16(m_bRaw, start)
            sRet(ii) = svalue.ToString
            start = start + 2
        Next

        Return String.Format("{0}/{1}/{2} {3}:{4}:{5}", sRet(0), sRet(1), sRet(2), sRet(3), sRet(4), sRet(5))

    End Function

    ''' <summary>
    ''' 伝文位置取得
    ''' </summary>
    ''' <param name="idx"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetPos(ByVal idx As Integer) As Integer
        Dim ret As Integer = 0

        For ii As Integer = 0 To idx - 1
            ret += m_lstName(ii).size
        Next
        Return ret
    End Function

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
                    btnEnd_Click(Nothing, Nothing)
            End Select
        Catch ex As Exception
        End Try
    End Sub

    ''' <summary>
    ''' 閉じるボタン押下
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Me.Close()

    End Sub


    ''' <summary>
    ''' IDリスト選択
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>

    Private Sub cmbID_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles cmbID.SelectedIndexChanged
        Call btnRead_Click(sender, e)
    End Sub


    ''' <summary>
    ''' 読み込みボタン押下
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRead.Click

        'データ表示
        If GetData(cmbID.Text) Then
            Call DispData()
        End If

    End Sub

    ''' <summary>
    ''' データ表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DispData()
        Dim dgv As DataGridView
        Dim row As Integer

        For ii As Integer = 0 To m_lstName.Count - 1
            If ii < m_nUpNum Then
                dgv = dgvCoilInfo
                row = ii
            Else
                dgv = dgvNextCoilInfo
                row = ii - m_nUpNum
            End If
            If "送信時刻" = m_lstName(ii).name Then
                dgv(4, row).Value = GetTimeValue(ii)
            Else
                dgv(4, row).Value = GetValue(ii)
            End If
            dgv(5, row).Value = GetHex(ii)
        Next ii

    End Sub

    ''' <summary>
    ''' データ取得
    ''' </summary>
    ''' <remarks></remarks>
    Private Function GetData(ByVal strID As String) As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文

        '疵検管理Noあり
        strSQL = ""
        strSQL &= " Select 生伝文"
        strSQL &= " FROM " & DB_BUF_PRO_R_COILINF
        strSQL &= " WHERE ID = '" & strID & "'"
        strSQL &= " ORDER BY 登録日付 DESC"

        'DBオープン
        If Not tcls_DB.Open() Then
            Return False
        End If

        ' Select文実行
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Return False

        Try

            ' データ取得
            sqlRead.Read()

            '生伝文
            m_bRaw = CType(sqlRead.GetValue(0), Byte())

        Catch ex As Exception
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

        Return True

    End Function


    ''' <summary>
    ''' 列表示切替
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvColumnHeader_DoubleClick(ByVal sender As System.Object, ByVal e As DataGridViewCellMouseEventArgs)
        Dim dgv As DataGridView = dgvCoilInfo
        With dgv.Columns(2)
            If .Visible Then
                .Visible = False
            Else
                .Visible = True
            End If
            dgvNextCoilInfo.Columns(2).Visible = .Visible
        End With
    End Sub


End Class
