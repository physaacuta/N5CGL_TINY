Imports tClassLibrary
Imports System.Reflection

Public Class frmMain
    Private WithEvents mcls_Mod As KizuLibCLI.KizuMod                   ' 総合クラス
    Private m_bData() As Byte                                   ' 読み込みデータ


    ''' <summary>
    ''' ロード
    ''' </summary>
    Private Sub frmMain_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
		dgvInitialize(dgvList)

        ' ※KizuLibCLI 32bit / 64bit 切り替え用
        AddHandler AppDomain.CurrentDomain.AssemblyResolve, New ResolveEventHandler(AddressOf OnAssemblyResolve)

        Task_First()

    End Sub

    ''' <summary>
    ''' タスク初期化時処理
    ''' </summary>
    ''' <remarks>Main内でKizuLibCLIにアクセスさせないために分離した</remarks>
    Private Sub Task_First()
        mcls_Mod = New KizuLibCLI.KizuMod
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView 関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 表示設定
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitialize(ByVal dgv As DataGridView)
		Dim ii As Integer = 0
		With dgv
			.RowCount = 1

			'' 動作定義
			.AutoGenerateColumns = False									' 列自動生成 (falseにするとDataSourceが効かなくなるので注意)
			.AllowUserToAddRows = False										' ユーザー 行追加
			.AllowUserToDeleteRows = False									' ユーザー 行削除
			.AllowUserToOrderColumns = False								' ユーザー 列移動
			.AllowUserToResizeColumns = False								' ユーザー 列サイズ変更
			.AllowUserToResizeRows = False									' ユーザー 行サイズ変更
			.ShowCellErrors = False
			.ShowCellToolTips = False
			.ShowEditingIcon = False
			.ShowRowErrors = False

			.SelectionMode = DataGridViewSelectionMode.CellSelect		' セル選択
			.MultiSelect = False											' 複数行選択
			.ReadOnly = True												' 読み取り専用
			.EditMode = DataGridViewEditMode.EditProgrammatically			' セル書き換え

			.ColumnHeadersVisible = True									' 列ヘッダ表示
			.RowHeadersVisible = False										' 行ヘッダ表示
			.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None		' 可視列サイズ自動調整
			.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None			' 可視行サイズ自動調整
			.ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing	'
			.RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing			'

			'' 選択セル色定義
			.DefaultCellStyle().SelectionBackColor = Color.LightPink			' 選択
			'.DefaultCellStyle().SelectionBackColor = Color.Transparent		' 選択色 無効化 (dgvLog_SelectionChangedが必須)
			.DefaultCellStyle().SelectionForeColor = Color.Black
			.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue		' 1行おきに色替え
			.DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ ゴシック", 8.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)

			'' 列定義
			.ColumnCount = 19			'textbox のみを追加する場合はこれの方が楽
			'.Columns.AddRange( _
			' New DataGridViewTextBoxColumn(), _
			'New DataGridViewTextBoxColumn(), _
			'New DataGridViewTextBoxColumn(), _
			'New DataGridViewTextBoxColumn(), _
			'New DataGridViewTextBoxColumn(), _
			' New DataGridViewTextBoxColumn())
			.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 7.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.ColumnHeadersHeight = 20										' 列ヘッダサイズ

			'列0 (アドレス)
			.Columns(0).HeaderText = "Hex"
			.Columns(0).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
			.Columns(0).Width = 60
			.Columns(0).SortMode = DataGridViewColumnSortMode.NotSortable
			.Columns(0).DefaultCellStyle.Format = "X6"
			.Columns(0).Frozen = True										' 水平移動禁止
			.Columns(0).DefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			'列1 (アドレス)
			.Columns(1).HeaderText = "Dec"
			.Columns(1).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
			.Columns(1).Width = 60
			.Columns(1).SortMode = DataGridViewColumnSortMode.NotSortable
			.Columns(1).DefaultCellStyle.Format = "D6"
			.Columns(1).Frozen = True										' 水平移動禁止
			.Columns(1).DefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			'列2～18 (データ)
			For ii = 0 To 16 - 1
				.Columns(ii + 2).HeaderText = ii.ToString("X")
				.Columns(ii + 2).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
				.Columns(ii + 2).Width = 20
				.Columns(ii + 2).SortMode = DataGridViewColumnSortMode.NotSortable
				.Columns(ii + 2).DefaultCellStyle.Format = "X2"
				.Columns(ii + 2).DefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			Next ii
			'列1 (アドレス)
			.Columns(18).HeaderText = "ASCII"
			.Columns(18).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
			.Columns(18).Width = 130
			.Columns(18).SortMode = DataGridViewColumnSortMode.NotSortable
			.Columns(18).Frozen = True										' 水平移動禁止
			.Columns(18).DefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)

		End With
	End Sub

	''' <summary>
	''' グリッドにデータをセット
	''' </summary>
	Private Sub setdata(ByVal dgv As DataGridView, ByVal b() As Byte)
		Dim rowCount As Integer = CInt(IIf(0 = (b.Length Mod 16), CInt(b.Length \ 16) - 1, CInt(b.Length \ 16)))
		Dim index As Integer = 0

		dgv.RowCount = rowCount + 1
		' データセット
		For ii As Integer = 0 To rowCount
			index = ii * 16
			dgv(0, ii).Value = index
			dgv(1, ii).Value = index
			dgv(16 + 2, ii).Value = ""	'とりあえず初期化 そうしないと、tostring時にnothingで落ちる

			For jj As Integer = 0 To 15
				' ヘキサ データ
				dgv(jj + 2, ii).Value = b(index + jj)

				' ASCII データ
				If b(index + jj) < &H20 Or b(index + jj) > &H7F Then
					dgv(16 + 2, ii).Value = dgv(16 + 2, ii).Value.ToString & "･"
				Else
					dgv(16 + 2, ii).Value = dgv(16 + 2, ii).Value.ToString & Chr(b(index + jj))
				End If


				'最後？
				If b.Length - 1 <= index + jj Then Exit For
			Next jj
		Next ii

		'サイズをセット
		lblByteSize.Text = b.Length.ToString("#,##0")
	End Sub

	''' <summary>
	''' 選択セル制御 (選択位置のセルの色変え。及び、各種型に変換して表示)
	''' </summary>
	Private Sub dgvList_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvList.SelectionChanged
		' データ部以外は選択禁止
		If dgvList.SelectedCells.Count <= 0 Then Return ' 1→0 に選択セル変更時 対策
		If dgvList.CurrentCell.ColumnIndex = 0 Or dgvList.CurrentCell.ColumnIndex = 1 Or dgvList.CurrentCell.ColumnIndex = 18 Then
			dgvList.CurrentCell.Selected = False		' PC名称列は選択禁止とする
			Return
		End If

		'' データ部選択
		Dim rec As Integer = dgvList.CurrentCell.RowIndex * 16 + dgvList.CurrentCell.ColumnIndex - 2
		lblByteItiD.Text = "D=" & rec.ToString("#,##0")
		lblByteItiH.Text = "H=" & rec.ToString("X")

		'' 型変換
		Try
			lblByte_Short.Text = BitConverter.ToInt16(m_bData, rec).ToString("#,##0")
		Catch ex As Exception
			lblByte_Short.Text = ""
		End Try
		Try
			lblByte_Int.Text = BitConverter.ToInt32(m_bData, rec).ToString("#,##0")
		Catch ex As Exception
			lblByte_Int.Text = ""
		End Try
		Try
			lblByte_Long.Text = BitConverter.ToInt64(m_bData, rec).ToString("#,##0")
		Catch ex As Exception
			lblByte_Long.Text = ""
		End Try
		Try
			lblByte_Single.Text = BitConverter.ToSingle(m_bData, rec).ToString("#,##0.#")
		Catch ex As Exception
			lblByte_Single.Text = ""
		End Try
		Try
			lblByte_Double.Text = BitConverter.ToDouble(m_bData, rec).ToString("#,##0.#")
		Catch ex As Exception
			lblByte_Double.Text = ""
		End Try
		Try
			lblByte_Byte.Text = m_bData(rec).ToString
		Catch ex As Exception
			lblByte_Byte.Text = ""
		End Try
		Try
			lblByte_Char.Text = Chr(m_bData(rec))
		Catch ex As Exception
			lblByte_Char.Text = ""
		End Try
		Try
			Dim size As Integer = m_bData.Length - rec
			If size > 32 Then size = 32
			lblByte_String.Text = System.Text.Encoding.GetEncoding("shift_jis").GetString(m_bData, rec, size)
		Catch ex As Exception
			lblByte_String.Text = ""
		End Try
	End Sub

	''' <summary>
	''' 矢印キーでのセル移動制御
	''' </summary>
	Private Sub dgvList_PreviewKeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.PreviewKeyDownEventArgs) Handles dgvList.PreviewKeyDown
		Dim rec As Boolean
		'データ部のみで移動させたい為
		If Keys.Right = e.KeyCode Then
			If dgvList.RowCount - 1 = dgvList.CurrentCell.RowIndex Then Return
			If dgvList.CurrentCell.ColumnIndex <> 17 Then Return
			rec = True

		ElseIf Keys.Left = e.KeyCode Then
			If 0 = dgvList.CurrentCell.RowIndex Then Return
			If dgvList.CurrentCell.ColumnIndex <> 2 Then Return
			rec = False
		Else
			Return
		End If

		'データFの位置時は次行のデータ0に移動
		' たぶんこの後で、矢印キーの処理が走るからそれを考慮した位置にしておく
		If rec Then
			dgvList(2 - 1, dgvList.CurrentCell.RowIndex + 1).Selected = True
		Else
			dgvList(17 + 1, dgvList.CurrentCell.RowIndex - 1).Selected = True
		End If
	End Sub








	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ データ入出力 関連
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' バイナリファイル出力
	''' </summary>
	Private Sub btnByteWrite_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnByteWrite.Click
		Dim strPath As String = txtDumpData.Text.Trim
		If 0 = strPath.Length Then
			MsgBox("ファイル名異常")
			Exit Sub
		End If
		If m_bData Is Nothing OrElse 0 = m_bData.Length Then
			MsgBox("データ無し")
			Exit Sub
		End If
		'EXE実行直下に生成
		Dim strFolder As String = Application.StartupPath()
		strFolder = strFolder & "\Dump"
		'' ログフォルダ生成
		tMod.CheckFolder(strFolder)

		'' ファイル名生成
		strPath = strFolder & "\" & strPath & ".bin"
		BiteFileWrite(strPath)
	End Sub

	''' <summary>
	''' バイナリダンプファイル出力
	''' </summary>
	Private Sub BiteFileWrite(ByVal strFPath As String)
		Dim os As New IO.FileStream(strFPath, IO.FileMode.OpenOrCreate, IO.FileAccess.Write, IO.FileShare.ReadWrite)
		os.Write(m_bData, 0, m_bData.Length)
		os.Close()
		os.Dispose()
	End Sub

	''' <summary>
	''' ダンプファイル出力
	''' </summary>
	Private Sub btnDumpData_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnDumpWrite.Click
		Dim strPath As String = txtDumpData.Text.Trim
		If 0 = strPath.Length Then
			MsgBox("ファイル名異常")
			Exit Sub
		End If
		If m_bData Is Nothing OrElse 0 = m_bData.Length Then
			MsgBox("データ無し")
			Exit Sub
		End If

		'EXE実行直下に生成
		Dim strFolder As String = Application.StartupPath()
		tMod.DumpData(strPath, strFolder, m_bData.Length, m_bData, False)
	End Sub

	''' <summary>
	''' ダンプ読込ファイル選択
	''' </summary>
	Private Sub btnDumpReadFileSelect_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnDumpReadFileSelect.Click
		Dim dlg As New OpenFileDialog
		dlg.Title = "ダンプ読込ファイル選択"
		dlg.DefaultExt = "*.log"
		dlg.Filter = "疵検ダンプファイル (*.log,*.bin)|*.log;*.bin"
		dlg.InitialDirectory = ".\"
		dlg.Multiselect = False
		dlg.ShowHelp = False
		dlg.ShowReadOnly = False
		dlg.InitialDirectory = txtDumpReadFileSelect.Text
		dlg.FileName = txtDumpReadFileSelect.Text
		dlg.ShowDialog()

		If dlg.FileName <> "" Then
			txtDumpReadFileSelect.Text = dlg.FileName
		End If
	End Sub
	''' <summary>
	''' ダンプファイル読込
	''' </summary>
	Private Sub btnDumpRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnDumpRead.Click
		Dim strPath As String
		strPath = txtDumpReadFileSelect.Text
		If Not IO.File.Exists(strPath) Then
			MsgBox("ダンプファイルが存在せんよ")
			Return
		End If

		'' ファイルオープン
		Dim os As New IO.StreamReader(strPath, System.Text.Encoding.GetEncoding("shift-jis"))


		'' 行単位で読込
		Dim wk As String
		Dim rec As Integer
		Dim ii As Integer
		Dim buf As String
		Dim ary As New ArrayList
		While Not os.EndOfStream
			wk = os.ReadLine()		' 1行単位で読込
			If wk.StartsWith("#") Then Continue While ' 先頭 # は無視
			If wk.Length < 11 Then Continue While '絶対にデータ部では無いので 無視

			''''' たぶんデータ行

			' まず初期化
			' データ部をバラス
			rec = 20
			For ii = 0 To 15
				buf = wk.Substring(rec, 2)
				If buf.Trim.Length = 2 Then
					ary.Add(CByte("&h" & buf))
				End If
				rec += 3
				If ii = 7 Then rec += 2
			Next ii
		End While
		os.Close()
		os.Dispose()

		' 読み込んだデータを セット
		ReDim m_bData(ary.Count - 1)
		ary.CopyTo(m_bData)

		' グリッドセット
		setdata(dgvList, m_bData)
	End Sub

	''' <summary>
	''' バイナリダンプファイル読込
	''' </summary>
	Private Sub btnByteRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnByteRead.Click
		Dim strPath As String
		strPath = txtDumpReadFileSelect.Text
		If Not IO.File.Exists(strPath) Then
			MsgBox("バイナリダンプファイルが存在せんよ")
			Return
		End If

		'' ファイルオープン
		Dim os As New IO.FileStream(strPath, IO.FileMode.Open, IO.FileAccess.Read, IO.FileShare.ReadWrite)
		Dim bwk() As Byte
		ReDim bwk(CInt(os.Length - 1))
		os.Read(bwk, 0, bwk.Length)
		os.Close()
		os.Dispose()

		' 読み込んだデータを セット
		ReDim m_bData(bwk.Length - 1)
		m_bData = bwk

		' グリッドセット
		setdata(dgvList, m_bData)
	End Sub

	''' <summary>
	''' 共有メモリ参照
	''' </summary>
	Private Sub btnMemRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnMemRead.Click
		If 0 <> tClass.tClass_ImputCheck.CheckImputMsg_Int(txtMemReadTableSize.Text, 1, , False) Then Exit Sub
		If txtMemReadTableName.Text.Trim.Length = 0 Then
			MsgBox("共有メモリテーブル名称なし")
			Return
		End If
		Dim size As Integer = CInt(txtMemReadTableSize.Text)
		Dim name As String = txtMemReadTableName.Text.Trim
		Dim bwk(size - 1) As Byte
		If Not KizuLibCLI.KizuMod.ksMemRead(name, size, bwk) Then
			MsgBox("共有メモリアクセス失敗")
			Return
		End If

		' 読み込んだデータを セット
		ReDim m_bData(size - 1)
		m_bData = bwk

		' グリッドセット
		setdata(dgvList, m_bData)
	End Sub

	''' <summary>
	''' DB読込
	''' </summary>
	Private Sub btnDbRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnDbRead.Click
		Dim strAddless As String = txtDbReadAddless.Text.Trim
		Dim strDbName As String = txtDbReadDbName.Text.Trim
		Dim strUser As String = txtDbReadUser.Text.Trim
		Dim strPass As String = txtDbReadPass.Text.Trim
		Dim strSQL As String = txtDbReadSql.Text

		Dim bwk() As Byte = Nothing

		'' SQLServer接続
        Dim tcls_db As tClass.tClass_SQLServer = New tClass.tClass_SQLServer(strAddless, strDbName, strUser, strPass)
        tcls_db.Open()
        Dim o As Object
        o = tcls_db.GetSelectKey(strSQL)
        If o Is Nothing Then
            tcls_db.Close()
            MsgBox("DB情報 取得失敗")
            Return
        End If
        tcls_db.Close()

		' 読み込んだデータを セット
        bwk = CType(o, Byte())
        ReDim m_bData(bwk.Length - 1)
		m_bData = bwk

		' グリッドセット
		setdata(dgvList, m_bData)
	End Sub

    ''' <summary>
    ''' KizuLibCLI.DLLの読み込み
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="args"></param>
    ''' <returns></returns>
    ''' <remarks>32bit/64bit切り替え対応</remarks>
    Private Function OnAssemblyResolve(ByVal sender As Object, ByVal args As System.ResolveEventArgs) As Assembly

        If args.Name.ToUpper.StartsWith("KIZULIBCLI") Then

            If IntPtr.Size = 8 Then
                ' 64bit環境
                Return Assembly.LoadFile(IO.Path.Combine(Application.StartupPath, "KizuLibCLIx64.dll"))
            Else
                ' 32bit環境
                Return Assembly.LoadFile(IO.Path.Combine(Application.StartupPath, "KizuLibCLIx32.dll"))
            End If

        End If

        Return Nothing

    End Function
End Class
