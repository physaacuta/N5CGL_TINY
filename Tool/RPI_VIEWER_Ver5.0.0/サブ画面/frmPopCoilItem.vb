Option Strict On

Imports tClassLibrary
Imports tClassLibrary.tClass

Public Class frmPopCoilItem


    Private Enum EM_GRID_COL_ID
        em_txtName = 0		'名称
        em_txtData			'データ
        em_End
    End Enum


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数

    ' クラス
    Private mcls_pRpiView As tClass_ViewRpiThread = Nothing				' RPIファイル表示クラス (ポインタ)

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ プロパティ

    ''' <summary>
    ''' RPIクラス
    ''' </summary>
    ''' <param name="cls">RPIクラス</param>
    ''' <remarks></remarks>
    Public Sub SetCls_RpiView(ByRef cls As tClass_ViewRpiThread)
        mcls_pRpiView = cls
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 処理

    ''' <summary>
    ''' 再表示
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ReDsp()
        ' dgv初期設定
        Call DgvInit(dgvCoil)
        ' 列名を取得
        Call GetClmSQL()

        ' データセット
        Call DspUpDate()

        ' 独自追加
        Call DspUpDateItem()
    End Sub


    ''' <summary>
    ''' フォームロード
    ''' </summary>
    Private Sub frmPopCoilItem_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        SetFormLocationAtOnce = True
        ' データ表示
        Call ReDsp()
    End Sub
    ''' <summary>
    ''' 動作定義
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Private Sub DgvInit(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)

        Call dgv.dgvInitialize(dgv)

        With dgv

            .ScrollBars = ScrollBars.Vertical								   ' 垂直スクロールバーの表示
            .ColumnHeadersVisible = False									   ' 列ヘッダ表示
            .AllowUserToResizeColumns = True								' ユーザー 列サイズ変更

            ' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor

            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue	' 1行おきに色替え
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)

            .DefaultCellStyle().ForeColor = Color.Black

            ' フォームのサイズに合わせて横幅を均等に割り付ける処理
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill

            '' 列定義
            .ColumnCount = EM_GRID_COL_ID.em_End		'textbox のみを追加する場合はこれの方が楽
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft

            '列0 (アドレス) 名称
            .Columns(EM_GRID_COL_ID.em_txtName).HeaderText = "名称"
            .Columns(EM_GRID_COL_ID.em_txtName).ReadOnly = False
            .Columns(EM_GRID_COL_ID.em_txtName).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_GRID_COL_ID.em_txtName).SortMode = DataGridViewColumnSortMode.NotSortable

            '列1 (アドレス) データ
            .Columns(EM_GRID_COL_ID.em_txtData).HeaderText = "値"
            .Columns(EM_GRID_COL_ID.em_txtData).ReadOnly = False
            .Columns(EM_GRID_COL_ID.em_txtData).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_GRID_COL_ID.em_txtData).SortMode = DataGridViewColumnSortMode.NotSortable
        End With
    End Sub

    ''' <summary>
    ''' DB項目名取得
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub GetClmSQL()
        Dim sSession As String = "DB_" & mcls_pRpiView.GetRpi_THeader.cSysName
        Dim sCoilInf As String = tMod.tMod_IniFile.ReadIniStr(sSession, "KEY_COILINF", TASKINI_NAME)
        Dim sSQL As String = " SELECT NAME FROM SYS.COLUMNS WHERE OBJECT_ID = OBJECT_ID('" & sCoilInf & "')"

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, sSession)
        Dim sqlAdpter As New SqlClient.SqlDataAdapter
        Dim sqlData As New DataSet

        Dim ii As Integer						' INDEX

        Try
            ' DBオープン
            If Not tcls_DB.Open() Then Exit Sub

            ' Select文実行 
            sqlData.Clear()
            sqlAdpter = tcls_DB.SelectAdapter(sSQL)
            sqlAdpter.Fill(sqlData)

            ' DB項目名称セット
            Dim sqlGet() As DataRow = sqlData.Tables(0).Select()

            ' dgv表示処理
            dgvCoil.RowCount = sqlData.Tables(0).Select().Length
            For ii = 0 To dgvCoil.RowCount - 1
                dgvCoil(EM_GRID_COL_ID.em_txtName, ii).Value = sqlGet(ii).Item("NAME")
            Next ii

            sqlAdpter.Dispose()

        Catch ex As Exception

        Finally
            tcls_DB.Cancel()
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

    End Sub

    ''' <summary>
    ''' コイル情報一覧を表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspUpDate()
        Dim sSession As String = "DB_" & mcls_pRpiView.GetRpi_THeader.cSysName
        Dim sCoilInf As String = tMod.tMod_IniFile.ReadIniStr(sSession, "KEY_COILINF", TASKINI_NAME)
        Dim sKey As String = tMod.tMod_IniFile.ReadIniStr(sSession, "KEY_NO", TASKINI_NAME)

        Dim sSQL As String = String.Format("SELECT * FROM {0} WHERE {1}='{2}'", sCoilInf, sKey, mcls_pRpiView.GetRpi_THeader.cKizukenNo)

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, sSession)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing


        Try
            ' DB接続
            If Not tcls_DB.Open() Then Return

            'SQL実行
            sqlRead = tcls_DB.SelectExecute(sSQL)
            If sqlRead Is Nothing Then Return
            If Not sqlRead.Read() Then Return

            ' データ取得
            For ii As Integer = 0 To dgvCoil.RowCount - 1
                dgvCoil(EM_GRID_COL_ID.em_txtData, ii).Value = sqlRead.GetValue(ii)
            Next

        Catch ex As Exception
            Return
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try
        Return
    End Sub



    ''' <summary>
    ''' 拡張コイル情報一覧を表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspUpDateItem()
        Dim sSession As String = "DB_" & mcls_pRpiView.GetRpi_THeader.cSysName
        Dim sWk As String = tMod.tMod_IniFile.ReadIniStr(sSession, "SELECT_COIL_ITEM", TASKINI_NAME)
        Dim sSQL As String = String.Format(sWk, mcls_pRpiView.GetRpi_THeader.cKizukenNo)
        If sWk = "" Then Return

        Dim nRow As Integer = dgvCoil.RowCount

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, sSession)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing


        Try
            ' DB接続
            If Not tcls_DB.Open() Then Return

            'SQL実行
            sqlRead = tcls_DB.SelectExecute(sSQL)
            If sqlRead Is Nothing Then Return

            ' データ取得
            dgvCoil.Enabled = False
            Dim nRowCnt As Integer = 0
            While sqlRead.Read()
                ' データ取得
                nRowCnt += 1

                For ii As Integer = 0 To sqlRead.FieldCount() - 1
                    dgvCoil.RowCount += 1

                    sWk = String.Format("[{0}件目] {1}", nRowCnt, sqlRead.GetName(ii))
                    dgvCoil(EM_GRID_COL_ID.em_txtName, dgvCoil.RowCount - 1).Value = sWk
                    dgvCoil(EM_GRID_COL_ID.em_txtData, dgvCoil.RowCount - 1).Value = sqlRead.GetValue(ii)
                Next

            End While


        Catch ex As Exception
            Return
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()

            dgvCoil.Enabled = True

        End Try
        Return
    End Sub

End Class