'*********************************************************************************
'　疵混入率画面クラス
'	[Ver]
'		Ver.01    2007/09/22  初版
'
'	[メモ]
'		
'*********************************************************************************
'Option Strict On
Option Strict Off

Imports tClassLibrary
Public Class frmMixedRateTool

    Private m_mode As Integer               '要求モード(1:オンライン用、0:ツール用）
    Private m_strKizukenNo As String        '疵検管理No

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const DgvMaxRow_MRare As Integer = 11           'データ行MAX
    Private Const DgvMaxCol_MRare As Integer = 7            'データ列MAX

    ' グリッド　列設定
    Private Enum EM_MRATE_COL_ID
        iTitle = 0              '項目見出し
        iGrade_No01             'Grade1
        iGrade_No02             'Grade2
        iGrade_No03             'Grade3
        iGrade_No04             'Grade4
        iGrade_No05             'Grade5
        iGrade_No06             'Grade6
    End Enum

    ' グリッド 行設定
    Private Enum EM_MRATE_ROW_ID
        iRate = 0               '疵混入率
        iType_No01              '疵種No01
        iType_No02              '疵種No02
        iType_No03              '疵種No03
        iType_No04              '疵種No04
        iType_No05              '疵種No05
        iType_No06              '疵種No06
        iType_No07              '疵種No07
        iType_No08              '疵種No08
        iType_No09              '疵種No09
        iType_No10              '疵種No10
    End Enum

    ''' <summary>
    ''' パラメータに要求モード(1:オンライン用、0:ツール用）
    ''' </summary>
    ''' <param name="mode">1:オンライン用、0:ツール用</param>
    ''' <param name="strKizukenNo">管理No</param>
    ''' <remarks></remarks>
    Public Sub New(ByVal mode As Integer, Optional ByVal strKizukenNo As String = "")
        ' この呼び出しは、Windows フォーム デザイナで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。
        'メンバ変数セット
        m_mode = mode                   'パラメータに要求モード(1:オンライン用、0:ツール用）
        m_strKizukenNo = strKizukenNo   '管理No

        '-------------------------------------------------------------------
        ' ファンクションキー設定
        '-------------------------------------------------------------------
        AddFunctionButton(btnEnd, 12 - 1)
    End Sub

    ''' <summary>
    ''' グリッドの初期化
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitialize(ByVal dgv As DataGridView)
        'Dim ii As Integer = 0
        With dgv
            .RowCount = DgvMaxRow_MRare

            '' 動作定義
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
            .ScrollBars = ScrollBars.None


            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' セル選択
            .MultiSelect = False                                            ' 複数行選択
            .ReadOnly = True                                                ' 読み取り専用
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' セル書き換え

            .ColumnHeadersVisible = True                                    ' 列ヘッダ表示
            .RowHeadersVisible = False                                      ' 行ヘッダ表示
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' 可視列サイズ自動調整
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' 可視行サイズ自動調整
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

            '' 選択セル色定義
            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue                 ' 選択
            '.DefaultCellStyle().SelectionBackColor = Color.Transparent		' 選択色 無効化 (dgvLog_SelectionChangedが必須)
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue      ' 1行おきに色替え
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            '' 列定義
            .ColumnCount = DgvMaxCol_MRare           'textbox のみを追加する場合はこれの方が楽
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 20                                       ' 列ヘッダサイズ

            '列１ (アドレス) 項目見出し
            .Columns(EM_MRATE_COL_ID.iTitle).HeaderText = ""
            .Columns(EM_MRATE_COL_ID.iTitle).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iTitle).Width = 120
            .Columns(EM_MRATE_COL_ID.iTitle).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iTitle).Frozen = True                                       ' 水平移動禁止

            '列２ (アドレス) グレード１
            .Columns(EM_MRATE_COL_ID.iGrade_No01).HeaderText = "グレード１"
            .Columns(EM_MRATE_COL_ID.iGrade_No01).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iGrade_No01).Width = 100
            .Columns(EM_MRATE_COL_ID.iGrade_No01).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iGrade_No01).Frozen = True                                       ' 水平移動禁止

            '列３ (アドレス) グレード２
            .Columns(EM_MRATE_COL_ID.iGrade_No02).HeaderText = "グレード２"
            .Columns(EM_MRATE_COL_ID.iGrade_No02).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iGrade_No02).Width = 100
            .Columns(EM_MRATE_COL_ID.iGrade_No02).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iGrade_No02).Frozen = True                                       ' 水平移動禁止

            '列４ (アドレス) グレード３
            .Columns(EM_MRATE_COL_ID.iGrade_No03).HeaderText = "グレード３"
            .Columns(EM_MRATE_COL_ID.iGrade_No03).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iGrade_No03).Width = 100
            .Columns(EM_MRATE_COL_ID.iGrade_No03).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iGrade_No03).Frozen = True                                       ' 水平移動禁止

            '列５ (アドレス) グレード４
            .Columns(EM_MRATE_COL_ID.iGrade_No04).HeaderText = "グレード４"
            .Columns(EM_MRATE_COL_ID.iGrade_No04).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iGrade_No04).Width = 100
            .Columns(EM_MRATE_COL_ID.iGrade_No04).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iGrade_No04).Frozen = True                                       ' 水平移動禁止


            '列６ (アドレス) グレード５
            .Columns(EM_MRATE_COL_ID.iGrade_No05).HeaderText = "グレード５"
            .Columns(EM_MRATE_COL_ID.iGrade_No05).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iGrade_No05).Width = 100
            .Columns(EM_MRATE_COL_ID.iGrade_No05).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iGrade_No05).Frozen = True                                       ' 水平移動禁止

            '列７ (アドレス) グレード６
            .Columns(EM_MRATE_COL_ID.iGrade_No06).HeaderText = "グレード６"
            .Columns(EM_MRATE_COL_ID.iGrade_No06).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iGrade_No06).Width = 100
            .Columns(EM_MRATE_COL_ID.iGrade_No06).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iGrade_No06).Frozen = True                                       ' 水平移動禁止

        End With
    End Sub

    ''' <summary>
    ''' 画面ロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMixedRate_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        Me.tProp_Moveable = True

        Call SetTitleName(Me)

        '画面初期化
        Call InitDsp()

        'IDリスト作成
        Call MakeIDList()

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
            strSQL &= " SELECT ID"
            strSQL &= " FROM " & DB_BUF_PRO_S_ZISEKI
            strSQL &= " WHERE 管理No = '" & m_strKizukenNo & "'"
            strSQL &= " ORDER BY 登録日付 DESC"
        Else                         'ツールモード
            'strSQL &= " SELECT TOP 100 ID"
            strSQL &= " SELECT ID"
            strSQL &= " FROM " & DB_BUF_PRO_S_ZISEKI
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
    ''' 画面初期化
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub InitDsp()

        '疵検査管理No
        lblKizuKen.Text = ""

        '命令No
        lblMeiNo.Text = ""

        '生産NO
        lblProductNo.Text = ""

        '小番
        lblKoban.Text = ""

        '実績区分
        lblZisekiDiv.Text = ""
        lblZisekiDiv.BackColor = Color.FromKnownColor(KnownColor.Control)

        '収集区分
        lblColectDiv.Text = ""
        lblColectDiv.BackColor = Color.FromKnownColor(KnownColor.Control)

        'グラントフラグ
        lblGrant.Text = ""
        lblGrant.BackColor = Color.FromKnownColor(KnownColor.Control)

        '疵混入率表 初期化
        dgvInitialize(dgvMixedRate)

    End Sub


    ''' <summary>
    ''' データ取得
    ''' </summary>
    ''' <param name="strID">ID</param>
    ''' <param name="typZiseki">疵検査実績データ</param>
    ''' <param name="strKizuken">管理No</param>
    ''' <remarks></remarks>
    Private Function GetData(ByVal strID As String, ByRef typZiseki As PRO_TYPE_ZISEKI, ByRef strKizuken As String) As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文
        Dim bytData() As Byte        '生伝文バイナリデータ

        '疵検管理Noあり
        strSQL = ""
        strSQL &= " SELECT 生伝文,"
        strSQL &= "        管理No"
        strSQL &= " FROM " & DB_BUF_PRO_S_ZISEKI
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
            bytData = CType(sqlRead.GetValue(0), Byte())

            '疵検査管理No
            strKizuken = sqlRead.GetValue(1).ToString

            tMod.MemSet(bytData, 0, typZiseki, tMod.SizeOf(typZiseki))

        Catch ex As Exception
            Return False
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
    ''' データセット
    ''' </summary>
    '''<param name="typZiseki">プロコン実績データ</param>
    ''' <param name="strKizuken">疵検管理No</param>
    ''' <remarks></remarks>
    Private Sub SetData(ByVal typZiseki As PRO_TYPE_ZISEKI, ByVal strKizuken As String)

        '疵検管理No
        lblKizuKen.Text = strKizuken

        ''命令No
        lblMeiNo.Text = tMod.ByteToString(typZiseki.data.cMeiNo)

        ''生産NO
        lblProductNo.Text = tMod.ByteToString(typZiseki.data.cCoilNo)

        '小番
        lblKoban.Text = typZiseki.data.nKoban

        'グラントフラグ
        If 0 = typZiseki.data.Common.nGrandFlg Then
            lblGrant.Text = "正常"
            lblGrant.BackColor = Color.Blue
            lblGrant.ForeColor = Color.Yellow
        Else
            lblGrant.Text = "異常"
            lblGrant.BackColor = Color.Red
            lblGrant.ForeColor = Color.Yellow
        End If

        '実績区分
        If 0 = typZiseki.data.nZiMode Then
            lblZisekiDiv.Text = "正常"
            lblZisekiDiv.BackColor = Color.Blue
            lblZisekiDiv.ForeColor = Color.Yellow
        Else
            lblZisekiDiv.Text = "異常_" & typZiseki.data.nZiMode.ToString
            lblZisekiDiv.BackColor = Color.Red
            lblZisekiDiv.ForeColor = Color.Yellow
        End If

        '収集区分
        If 0 = typZiseki.data.nKikiMode Then
            lblColectDiv.Text = "正常"
            lblColectDiv.BackColor = Color.Blue
            lblColectDiv.ForeColor = Color.Yellow
        Else
            lblColectDiv.Text = "異常_" & typZiseki.data.nKikiMode.ToString
            lblColectDiv.BackColor = Color.Red
            lblColectDiv.ForeColor = Color.Yellow
        End If

        '項目見出し設定
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iRate).Value = "疵混入率 [％]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No01).Value = "疵種０１ [個]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No02).Value = "疵種０２ [個]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No03).Value = "疵種０３ [個]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No04).Value = "疵種０４ [個]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No05).Value = "疵種０５ [個]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No06).Value = "疵種０６ [個]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No07).Value = "疵種０７ [個]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No08).Value = "疵種０８ [個]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No09).Value = "疵種０９ [個]"
        dgvMixedRate(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID.iType_No10).Value = "疵種１０ [個]"


        '桁数確認用
        For jj As Integer = EM_MRATE_COL_ID.iGrade_No01 To EM_MRATE_COL_ID.iGrade_No06
            dgvMixedRate(jj, EM_MRATE_ROW_ID.iRate).Value = Format(typZiseki.data.grade_inf(jj - 1).nMixedRate / 100, "#0.00")
            dgvMixedRate(jj, EM_MRATE_ROW_ID.iRate).Style.Alignment = DataGridViewContentAlignment.MiddleRight
            For ii As Integer = EM_MRATE_ROW_ID.iType_No01 To EM_MRATE_ROW_ID.iType_No10
                dgvMixedRate(jj, ii).Value = typZiseki.data.grade_inf(jj - 1).nTypeCnt(ii - 1)
                dgvMixedRate(jj, ii).Style.Alignment = DataGridViewContentAlignment.MiddleRight
            Next ii
        Next jj

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
                    btnEnd_Click(Nothing, Nothing)
            End Select
        Catch ex As Exception
        End Try
    End Sub
    ''' <summary>
    ''' 画面終了
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)

        '画面終了
        Me.Close()

    End Sub

    ''' <summary>
    ''' セル選択
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvMixedRate_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvMixedRate.SelectionChanged

        '選択を解除
        dgvMixedRate.CurrentCell.Selected = False

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

    Private Sub btnRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRead.Click
        Dim typZiseki As PRO_TYPE_ZISEKI = Nothing  '疵検査実績データ
        Dim strKizuken As String = ""       '疵検管理No

        'データ表示
        If GetData(cmbID.Text, typZiseki, strKizuken) Then
            Call SetData(typZiseki, strKizuken)
        End If
    End Sub


End Class
