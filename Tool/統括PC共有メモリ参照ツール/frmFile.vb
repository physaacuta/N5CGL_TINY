Option Strict On

Imports System.Runtime.InteropServices


Public Class frmFile

#Region "API定義"
	' tClass_Library が無くても iniファイルが扱えるように定義
	'********************************************************************
	'API定義
	'********************************************************************
	''' <summary>
	''' [API] iniファイルから文字列読み込み
	''' </summary>
	<DllImport("KERNEL32.DLL", EntryPoint:="GetPrivateProfileStringA", SetLastError:=True, ExactSpelling:=False, CharSet:=CharSet.Ansi)> _
	Private Shared Function GetPrivateProfileString( _
	 <MarshalAs(UnmanagedType.LPStr)> ByVal lpApplicationName As String, _
	 <MarshalAs(UnmanagedType.LPStr)> ByVal lpKeyName As String, _
	 <MarshalAs(UnmanagedType.LPStr)> ByVal lpDefault As String, _
	 <MarshalAs(UnmanagedType.LPStr)> ByVal lpReturnedString As System.Text.StringBuilder, _
	 ByVal nSize As Integer, _
	 <MarshalAs(UnmanagedType.LPStr)> ByVal lpFileName As String) _
	 As Integer
		'' Integerの部分は、C言語の元々の関数定義ではDWORD型なのでUInt32の方が適切。でも、使い勝手の面でIntegerに変更
	End Function

	''' <summary>
	''' [API] iniファイルから数値読み込み
	''' </summary>
	<DllImport("KERNEL32.DLL", EntryPoint:="GetPrivateProfileInt", SetLastError:=True, ExactSpelling:=False, CharSet:=CharSet.Ansi)> _
	Private Shared Function GetPrivateProfileInt( _
	 <MarshalAs(UnmanagedType.LPStr)> ByVal lpAppName As String, _
	 <MarshalAs(UnmanagedType.LPStr)> ByVal lpKeyName As String, _
	 ByVal nDefault As Integer, _
	 <MarshalAs(UnmanagedType.LPStr)> ByVal lpFileName As String) _
	 As Integer
	End Function

	''' <summary>
	''' [API] iniファイルに文字列書き込み
	''' </summary>
	<DllImport("KERNEL32.DLL", EntryPoint:="WritePrivateProfileStringA", SetLastError:=True, ExactSpelling:=False, CharSet:=CharSet.Ansi)> _
	Private Shared Function WritePrivateProfileString( _
	 <MarshalAs(UnmanagedType.LPStr)> ByVal lpAppName As String, _
	 <MarshalAs(UnmanagedType.LPStr)> ByVal lpKeyName As String, _
	 <MarshalAs(UnmanagedType.LPStr)> ByVal lpString As String, _
	 <MarshalAs(UnmanagedType.LPStr)> ByVal lpFileName As String) _
	 As Boolean
	End Function

	'********************************************************************
	'メソッド定義
	'********************************************************************
	''' <summary>
	''' iniファイルから 文字列 を読み込む
	''' </summary>
	''' <param name="strSection">セクション</param>
	''' <param name="strKey">キー</param>
	''' <param name="strFileName">iniファイルのフルパス</param>
	''' <param name="strDef">初期値(失敗した場合の戻り値)</param>
	''' <param name="intSize">読み込む最大文字数</param>
	''' <returns>取得した値</returns>
	''' <remarks></remarks>
	Public Function ReadIniStr(ByVal strSection As String, ByVal strKey As String, ByVal strFileName As String, Optional ByVal strDef As String = "", Optional ByVal intSize As Integer = 1024) As String
		Dim sbBuff As System.Text.StringBuilder = New System.Text.StringBuilder(intSize)
		Try
			'''' 値取得
			Call GetPrivateProfileString(strSection, strKey, strDef, sbBuff, sbBuff.Capacity, strFileName)
			Return sbBuff.ToString
		Catch ex As Exception
			Return strDef
		End Try
	End Function

	''' <summary>
	''' iniファイルから 数値 を読み込む
	''' </summary>
	''' <param name="strSection">セクション</param>
	''' <param name="strKey">キー</param>
	''' <param name="strFileName">iniファイルのフルパス</param>
	''' <param name="intDef">初期値(失敗した場合の戻り値)</param>
	''' <returns>取得した値</returns>
	''' <remarks></remarks>
	Public Function ReadIniInt(ByVal strSection As String, ByVal strKey As String, ByVal strFileName As String, Optional ByVal intDef As Integer = 0) As Integer
		Try
			'''' 値取得
			Return GetPrivateProfileInt(strSection, strKey, intDef, strFileName)
		Catch ex As Exception
			Return intDef
		End Try
	End Function

	''' <summary>
	''' iniファイルの 値 に文字列 を書込み
	''' </summary>
	''' <param name="strSection">セクション</param>
	''' <param name="strKey">キー(該当キーが無ければキーも追加)</param>
	''' <param name="strFileName">iniファイルのフルパス</param>
	''' <param name="strVal">書き込む値</param>
	''' <returns>復帰情報</returns>
	''' <remarks></remarks>
	Public Function WriteIni(ByVal strSection As String, ByVal strKey As String, ByVal strFileName As String, ByVal strVal As String) As Boolean
		Try
			'''' 値書き込み
			Return WritePrivateProfileString(strSection, strKey, strVal, strFileName)
		Catch ex As Exception
			Return False
		End Try
	End Function
#End Region

	''' <summary>
	''' 共有メモリ情報
	''' </summary>
	''' <remarks></remarks>
	Structure SHMEM
		Dim name As String
		Dim size As Integer

		Public Sub New(ByVal n As String, ByVal s As Integer)
			name = n
			size = s
		End Sub
	End Structure

	Private lstSharedMemory As New List(Of SHMEM)

	Private m_strBaseFolder As String	' 共有メモリ保存・読込フォルダ


	Public Sub New()

		' この呼び出しは、Windows フォーム デザイナで必要です。
		InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。

        ' 対象共有メモリをセットする

        ' 設定が必要なのは、下記の共有メモリ関連のみ
        lstSharedMemory.Add(New SHMEM(TBL_TO_COIL_NAME, Marshal.SizeOf(GetType(TO_COIL_TBL))))
        lstSharedMemory.Add(New SHMEM(TBL_TO_SEQMEM_NAME, Marshal.SizeOf(GetType(TO_SEQMEM_TBL))))
        lstSharedMemory.Add(New SHMEM(TBL_TO_CAMERA_NAME, Marshal.SizeOf(GetType(TO_CAMERA_TBL))))

        m_strBaseFolder = ReadIniStr("共有メモリ参照ツール", "BASE_FOLDER", ".\設定.ini")
		If m_strBaseFolder = "" Then
			m_strBaseFolder = ".\"
		End If

		If IO.Directory.Exists(m_strBaseFolder) = False Then
			m_strBaseFolder = ".\"
		End If

		m_strBaseFolder = IO.Path.GetFullPath(m_strBaseFolder)

		WriteIni("共有メモリ参照ツール", "BASE_FOLDER", ".\設定.ini", m_strBaseFolder)

		lblBaseFolder.Text = m_strBaseFolder

	End Sub

	''' <summary>
	''' 読み込み処理(ファイル⇒共有メモリ)
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRead.Click
		If MessageBox.Show("ファイルから読み込んで共有メモリに書き込みます。" & vbCrLf & "実行しますか?", "確認", MessageBoxButtons.OKCancel) <> DialogResult.OK Then
			Exit Sub
		End If

		Dim dgv As DataGridView = dgvSharedMemory

		For ii As Integer = 0 To dgv.RowCount - 1
			dgv(2, ii).Value = ""
			dgv(2, ii).Style.BackColor = Nothing
		Next

		For ii As Integer = 0 To dgv.RowCount - 1
			Dim cls As New SharedMemoryReaeWrite
			Dim rc As Integer = cls.ReadFromFile(dgv(0, ii).Value.ToString, CInt(dgv(1, ii).Value))
			If rc = 0 Then
				dgv(2, ii).Value = "ファイル⇒メモリ 成功"
				dgv(2, ii).Style.BackColor = Color.LightGreen
			ElseIf rc = 1 Then
				dgv(2, ii).Value = String.Format("失敗 共有メモリが存在しません")
				dgv(2, ii).Style.BackColor = Color.Pink
			ElseIf rc = 2 Then
				dgv(2, ii).Value = String.Format("失敗 共有メモリサイズが0です")
				dgv(2, ii).Style.BackColor = Color.Pink
			ElseIf rc = 3 Then
				dgv(2, ii).Value = String.Format("失敗 指定サイズ[{0}] は共有メモリよりも大きいです", dgv(1, ii).Value)
				dgv(2, ii).Style.BackColor = Color.Pink
			ElseIf rc = 4 Then
				dgv(2, ii).Value = String.Format("失敗 指定ファイルが存在しません")
				dgv(2, ii).Style.BackColor = Color.Pink
			Else
				dgv(2, ii).Value = String.Format("ファイル⇒メモリ 失敗 rc[{0}]", rc)
				dgv(2, ii).Style.BackColor = Color.Pink
			End If
			cls.Dispose()
			Application.DoEvents()
		Next

	End Sub

	''' <summary>
	''' 書き込み処理(共有メモリ⇒ファイル)
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnWrite_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnWrite.Click

		If MessageBox.Show("共有メモリから読み込んでファイルに出力します。" & vbCrLf & "実行しますか?", "確認", MessageBoxButtons.OKCancel) <> DialogResult.OK Then
			Exit Sub
		End If


		Dim dgv As DataGridView = dgvSharedMemory

		For ii As Integer = 0 To dgv.RowCount - 1
			dgv(2, ii).Value = ""
			dgv(2, ii).Style.BackColor = Nothing
		Next


		For ii As Integer = 0 To dgv.RowCount - 1
			Dim cls As New SharedMemoryReaeWrite
			Dim rc As Integer = cls.WriteToFile(dgv.Item(0, ii).Value.ToString, CInt(dgv(1, ii).Value))
			If rc = 0 Then
				dgv(2, ii).Value = "メモリ⇒ファイル 成功"
				dgv(2, ii).Style.BackColor = Color.LightGreen
			ElseIf rc = 1 Then
				dgv(2, ii).Value = String.Format("失敗 共有メモリが存在しません")
				dgv(2, ii).Style.BackColor = Color.Pink
			ElseIf rc = 2 Then
				dgv(2, ii).Value = String.Format("失敗 共有メモリサイズが0です")
				dgv(2, ii).Style.BackColor = Color.Pink
			ElseIf rc = 3 Then
				dgv(2, ii).Value = String.Format("失敗 指定サイズ[{0}] は共有メモリよりも大きいです", dgv(1, ii).Value)
				dgv(2, ii).Style.BackColor = Color.Pink
			ElseIf rc = 4 Then
				dgv(2, ii).Value = String.Format("失敗 指定ファイルが存在しません")
				dgv(2, ii).Style.BackColor = Color.Pink
			Else
				dgv(2, ii).Value = String.Format("ファイル⇒メモリ 失敗 rc[{0}]", rc)
				dgv(2, ii).Style.BackColor = Color.Pink
			End If
			cls.Dispose()
			Application.DoEvents()
		Next

	End Sub

	''' <summary>
	''' フォームロード
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmFile_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
		' DataGridView初期化
		dgvInitialize(dgvSharedMemory)
	End Sub


	''' <summary>
	''' マップ初期化処理
	''' </summary>
	''' <param name="dgv">マップデータグリッド</param>
	''' <remarks></remarks>
	Private Sub dgvInitialize(ByVal dgv As DataGridView)
		Dim ii As Integer = 0
		With dgv
			'行列設定
			.ColumnCount = 3

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
			.ScrollBars = ScrollBars.Both

			.SelectionMode = DataGridViewSelectionMode.FullRowSelect		' セル選択
			.MultiSelect = False											' 複数行選択
			.ReadOnly = True												' 読み取り専用
			.EditMode = DataGridViewEditMode.EditProgrammatically			' セル書き換え

			.ColumnHeadersVisible = True									' 列ヘッダ表示
			.RowHeadersVisible = False										' 行ヘッダ表示
			.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None		' 可視列サイズ自動調整
			.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None			' 可視行サイズ自動調整
			.ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing	'
			.RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing			'
			.DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)

			'' 列定義
			.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
			.ColumnHeadersHeight = 32										' 列ヘッダサイズ

			'行ヘッダ設定
			For ii = 0 To .Columns.Count - 1
				Select Case ii
					Case 0
						.Columns(ii).HeaderText = "共有メモリ名"
						.Columns(ii).Width = 140
					Case 1
						.Columns(ii).HeaderText = "サイズ"
						.Columns(ii).Width = 80
					Case 2
						.Columns(ii).HeaderText = "結果"
						.Columns(ii).Width = .Width - 140 - 80 - 4

				End Select
			Next ii

			.RowCount = lstSharedMemory.Count
			For ii = 0 To lstSharedMemory.Count - 1
				.Item(0, ii).Value = lstSharedMemory(ii).name
				.Item(1, ii).Value = lstSharedMemory(ii).size
			Next

			' 選択をクリアする
			.ClearSelection()

		End With

		' 選択されてもクリアするようにイベントを定義
		AddHandler dgv.SelectionChanged, AddressOf dgv_SelectionChanged

	End Sub

	''' <summary>
	''' DataGridView 選択変更イベント
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub dgv_SelectionChanged(ByVal sender As Object, ByVal e As EventArgs)
		Dim dgv As DataGridView = CType(sender, DataGridView)
		' 選択されたらすぐにクリアする
		dgv.ClearSelection()
	End Sub

	''' <summary>
	''' 共有メモリ保存・読み込みフォルダ指定
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnBaseFolder_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnBaseFolder.Click
		Dim fbd As New FolderBrowserDialog

		fbd.SelectedPath = m_strBaseFolder

		If fbd.ShowDialog = Windows.Forms.DialogResult.OK Then
			m_strBaseFolder = fbd.SelectedPath
		End If

		fbd.Dispose()

		WriteIni("共有メモリ参照ツール", "BASE_FOLDER", ".\設定.ini", m_strBaseFolder)

		lblBaseFolder.Text = m_strBaseFolder

	End Sub
End Class