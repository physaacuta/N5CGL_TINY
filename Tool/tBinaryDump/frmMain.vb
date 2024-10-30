Imports tClassLibrary
Imports System.Reflection

Public Class frmMain
    Private WithEvents mcls_Mod As KizuLibCLI.KizuMod                   ' �����N���X
    Private m_bData() As Byte                                   ' �ǂݍ��݃f�[�^


    ''' <summary>
    ''' ���[�h
    ''' </summary>
    Private Sub frmMain_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
		dgvInitialize(dgvList)

        ' ��KizuLibCLI 32bit / 64bit �؂�ւ��p
        AddHandler AppDomain.CurrentDomain.AssemblyResolve, New ResolveEventHandler(AddressOf OnAssemblyResolve)

        Task_First()

    End Sub

    ''' <summary>
    ''' �^�X�N������������
    ''' </summary>
    ''' <remarks>Main����KizuLibCLI�ɃA�N�Z�X�����Ȃ����߂ɕ�������</remarks>
    Private Sub Task_First()
        mcls_Mod = New KizuLibCLI.KizuMod
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView �֘A
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �\���ݒ�
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitialize(ByVal dgv As DataGridView)
		Dim ii As Integer = 0
		With dgv
			.RowCount = 1

			'' �����`
			.AutoGenerateColumns = False									' �񎩓����� (false�ɂ����DataSource�������Ȃ��Ȃ�̂Œ���)
			.AllowUserToAddRows = False										' ���[�U�[ �s�ǉ�
			.AllowUserToDeleteRows = False									' ���[�U�[ �s�폜
			.AllowUserToOrderColumns = False								' ���[�U�[ ��ړ�
			.AllowUserToResizeColumns = False								' ���[�U�[ ��T�C�Y�ύX
			.AllowUserToResizeRows = False									' ���[�U�[ �s�T�C�Y�ύX
			.ShowCellErrors = False
			.ShowCellToolTips = False
			.ShowEditingIcon = False
			.ShowRowErrors = False

			.SelectionMode = DataGridViewSelectionMode.CellSelect		' �Z���I��
			.MultiSelect = False											' �����s�I��
			.ReadOnly = True												' �ǂݎ���p
			.EditMode = DataGridViewEditMode.EditProgrammatically			' �Z����������

			.ColumnHeadersVisible = True									' ��w�b�_�\��
			.RowHeadersVisible = False										' �s�w�b�_�\��
			.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None		' ����T�C�Y��������
			.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None			' ���s�T�C�Y��������
			.ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing	'
			.RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing			'

			'' �I���Z���F��`
			.DefaultCellStyle().SelectionBackColor = Color.LightPink			' �I��
			'.DefaultCellStyle().SelectionBackColor = Color.Transparent		' �I��F ������ (dgvLog_SelectionChanged���K�{)
			.DefaultCellStyle().SelectionForeColor = Color.Black
			.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue		' 1�s�����ɐF�ւ�
			.DefaultCellStyle().Font = New System.Drawing.Font("�l�r �S�V�b�N", 8.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)

			'' ���`
			.ColumnCount = 19			'textbox �݂̂�ǉ�����ꍇ�͂���̕����y
			'.Columns.AddRange( _
			' New DataGridViewTextBoxColumn(), _
			'New DataGridViewTextBoxColumn(), _
			'New DataGridViewTextBoxColumn(), _
			'New DataGridViewTextBoxColumn(), _
			'New DataGridViewTextBoxColumn(), _
			' New DataGridViewTextBoxColumn())
			.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 7.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.ColumnHeadersHeight = 20										' ��w�b�_�T�C�Y

			'��0 (�A�h���X)
			.Columns(0).HeaderText = "Hex"
			.Columns(0).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
			.Columns(0).Width = 60
			.Columns(0).SortMode = DataGridViewColumnSortMode.NotSortable
			.Columns(0).DefaultCellStyle.Format = "X6"
			.Columns(0).Frozen = True										' �����ړ��֎~
			.Columns(0).DefaultCellStyle.Font = New System.Drawing.Font("�l�r �S�V�b�N", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			'��1 (�A�h���X)
			.Columns(1).HeaderText = "Dec"
			.Columns(1).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
			.Columns(1).Width = 60
			.Columns(1).SortMode = DataGridViewColumnSortMode.NotSortable
			.Columns(1).DefaultCellStyle.Format = "D6"
			.Columns(1).Frozen = True										' �����ړ��֎~
			.Columns(1).DefaultCellStyle.Font = New System.Drawing.Font("�l�r �S�V�b�N", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			'��2�`18 (�f�[�^)
			For ii = 0 To 16 - 1
				.Columns(ii + 2).HeaderText = ii.ToString("X")
				.Columns(ii + 2).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
				.Columns(ii + 2).Width = 20
				.Columns(ii + 2).SortMode = DataGridViewColumnSortMode.NotSortable
				.Columns(ii + 2).DefaultCellStyle.Format = "X2"
				.Columns(ii + 2).DefaultCellStyle.Font = New System.Drawing.Font("�l�r �S�V�b�N", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			Next ii
			'��1 (�A�h���X)
			.Columns(18).HeaderText = "ASCII"
			.Columns(18).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
			.Columns(18).Width = 130
			.Columns(18).SortMode = DataGridViewColumnSortMode.NotSortable
			.Columns(18).Frozen = True										' �����ړ��֎~
			.Columns(18).DefaultCellStyle.Font = New System.Drawing.Font("�l�r �S�V�b�N", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)

		End With
	End Sub

	''' <summary>
	''' �O���b�h�Ƀf�[�^���Z�b�g
	''' </summary>
	Private Sub setdata(ByVal dgv As DataGridView, ByVal b() As Byte)
		Dim rowCount As Integer = CInt(IIf(0 = (b.Length Mod 16), CInt(b.Length \ 16) - 1, CInt(b.Length \ 16)))
		Dim index As Integer = 0

		dgv.RowCount = rowCount + 1
		' �f�[�^�Z�b�g
		For ii As Integer = 0 To rowCount
			index = ii * 16
			dgv(0, ii).Value = index
			dgv(1, ii).Value = index
			dgv(16 + 2, ii).Value = ""	'�Ƃ肠���������� �������Ȃ��ƁAtostring����nothing�ŗ�����

			For jj As Integer = 0 To 15
				' �w�L�T �f�[�^
				dgv(jj + 2, ii).Value = b(index + jj)

				' ASCII �f�[�^
				If b(index + jj) < &H20 Or b(index + jj) > &H7F Then
					dgv(16 + 2, ii).Value = dgv(16 + 2, ii).Value.ToString & "�"
				Else
					dgv(16 + 2, ii).Value = dgv(16 + 2, ii).Value.ToString & Chr(b(index + jj))
				End If


				'�Ō�H
				If b.Length - 1 <= index + jj Then Exit For
			Next jj
		Next ii

		'�T�C�Y���Z�b�g
		lblByteSize.Text = b.Length.ToString("#,##0")
	End Sub

	''' <summary>
	''' �I���Z������ (�I���ʒu�̃Z���̐F�ς��B�y�сA�e��^�ɕϊ����ĕ\��)
	''' </summary>
	Private Sub dgvList_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvList.SelectionChanged
		' �f�[�^���ȊO�͑I���֎~
		If dgvList.SelectedCells.Count <= 0 Then Return ' 1��0 �ɑI���Z���ύX�� �΍�
		If dgvList.CurrentCell.ColumnIndex = 0 Or dgvList.CurrentCell.ColumnIndex = 1 Or dgvList.CurrentCell.ColumnIndex = 18 Then
			dgvList.CurrentCell.Selected = False		' PC���̗�͑I���֎~�Ƃ���
			Return
		End If

		'' �f�[�^���I��
		Dim rec As Integer = dgvList.CurrentCell.RowIndex * 16 + dgvList.CurrentCell.ColumnIndex - 2
		lblByteItiD.Text = "D=" & rec.ToString("#,##0")
		lblByteItiH.Text = "H=" & rec.ToString("X")

		'' �^�ϊ�
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
	''' ���L�[�ł̃Z���ړ�����
	''' </summary>
	Private Sub dgvList_PreviewKeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.PreviewKeyDownEventArgs) Handles dgvList.PreviewKeyDown
		Dim rec As Boolean
		'�f�[�^���݂̂ňړ�����������
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

		'�f�[�^F�̈ʒu���͎��s�̃f�[�^0�Ɉړ�
		' ���Ԃ񂱂̌�ŁA���L�[�̏��������邩�炻����l�������ʒu�ɂ��Ă���
		If rec Then
			dgvList(2 - 1, dgvList.CurrentCell.RowIndex + 1).Selected = True
		Else
			dgvList(17 + 1, dgvList.CurrentCell.RowIndex - 1).Selected = True
		End If
	End Sub








	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �f�[�^���o�� �֘A
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �o�C�i���t�@�C���o��
	''' </summary>
	Private Sub btnByteWrite_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnByteWrite.Click
		Dim strPath As String = txtDumpData.Text.Trim
		If 0 = strPath.Length Then
			MsgBox("�t�@�C�����ُ�")
			Exit Sub
		End If
		If m_bData Is Nothing OrElse 0 = m_bData.Length Then
			MsgBox("�f�[�^����")
			Exit Sub
		End If
		'EXE���s�����ɐ���
		Dim strFolder As String = Application.StartupPath()
		strFolder = strFolder & "\Dump"
		'' ���O�t�H���_����
		tMod.CheckFolder(strFolder)

		'' �t�@�C��������
		strPath = strFolder & "\" & strPath & ".bin"
		BiteFileWrite(strPath)
	End Sub

	''' <summary>
	''' �o�C�i���_���v�t�@�C���o��
	''' </summary>
	Private Sub BiteFileWrite(ByVal strFPath As String)
		Dim os As New IO.FileStream(strFPath, IO.FileMode.OpenOrCreate, IO.FileAccess.Write, IO.FileShare.ReadWrite)
		os.Write(m_bData, 0, m_bData.Length)
		os.Close()
		os.Dispose()
	End Sub

	''' <summary>
	''' �_���v�t�@�C���o��
	''' </summary>
	Private Sub btnDumpData_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnDumpWrite.Click
		Dim strPath As String = txtDumpData.Text.Trim
		If 0 = strPath.Length Then
			MsgBox("�t�@�C�����ُ�")
			Exit Sub
		End If
		If m_bData Is Nothing OrElse 0 = m_bData.Length Then
			MsgBox("�f�[�^����")
			Exit Sub
		End If

		'EXE���s�����ɐ���
		Dim strFolder As String = Application.StartupPath()
		tMod.DumpData(strPath, strFolder, m_bData.Length, m_bData, False)
	End Sub

	''' <summary>
	''' �_���v�Ǎ��t�@�C���I��
	''' </summary>
	Private Sub btnDumpReadFileSelect_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnDumpReadFileSelect.Click
		Dim dlg As New OpenFileDialog
		dlg.Title = "�_���v�Ǎ��t�@�C���I��"
		dlg.DefaultExt = "*.log"
		dlg.Filter = "�r���_���v�t�@�C�� (*.log,*.bin)|*.log;*.bin"
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
	''' �_���v�t�@�C���Ǎ�
	''' </summary>
	Private Sub btnDumpRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnDumpRead.Click
		Dim strPath As String
		strPath = txtDumpReadFileSelect.Text
		If Not IO.File.Exists(strPath) Then
			MsgBox("�_���v�t�@�C�������݂����")
			Return
		End If

		'' �t�@�C���I�[�v��
		Dim os As New IO.StreamReader(strPath, System.Text.Encoding.GetEncoding("shift-jis"))


		'' �s�P�ʂœǍ�
		Dim wk As String
		Dim rec As Integer
		Dim ii As Integer
		Dim buf As String
		Dim ary As New ArrayList
		While Not os.EndOfStream
			wk = os.ReadLine()		' 1�s�P�ʂœǍ�
			If wk.StartsWith("#") Then Continue While ' �擪 # �͖���
			If wk.Length < 11 Then Continue While '��΂Ƀf�[�^���ł͖����̂� ����

			''''' ���Ԃ�f�[�^�s

			' �܂�������
			' �f�[�^�����o���X
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

		' �ǂݍ��񂾃f�[�^�� �Z�b�g
		ReDim m_bData(ary.Count - 1)
		ary.CopyTo(m_bData)

		' �O���b�h�Z�b�g
		setdata(dgvList, m_bData)
	End Sub

	''' <summary>
	''' �o�C�i���_���v�t�@�C���Ǎ�
	''' </summary>
	Private Sub btnByteRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnByteRead.Click
		Dim strPath As String
		strPath = txtDumpReadFileSelect.Text
		If Not IO.File.Exists(strPath) Then
			MsgBox("�o�C�i���_���v�t�@�C�������݂����")
			Return
		End If

		'' �t�@�C���I�[�v��
		Dim os As New IO.FileStream(strPath, IO.FileMode.Open, IO.FileAccess.Read, IO.FileShare.ReadWrite)
		Dim bwk() As Byte
		ReDim bwk(CInt(os.Length - 1))
		os.Read(bwk, 0, bwk.Length)
		os.Close()
		os.Dispose()

		' �ǂݍ��񂾃f�[�^�� �Z�b�g
		ReDim m_bData(bwk.Length - 1)
		m_bData = bwk

		' �O���b�h�Z�b�g
		setdata(dgvList, m_bData)
	End Sub

	''' <summary>
	''' ���L�������Q��
	''' </summary>
	Private Sub btnMemRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnMemRead.Click
		If 0 <> tClass.tClass_ImputCheck.CheckImputMsg_Int(txtMemReadTableSize.Text, 1, , False) Then Exit Sub
		If txtMemReadTableName.Text.Trim.Length = 0 Then
			MsgBox("���L�������e�[�u�����̂Ȃ�")
			Return
		End If
		Dim size As Integer = CInt(txtMemReadTableSize.Text)
		Dim name As String = txtMemReadTableName.Text.Trim
		Dim bwk(size - 1) As Byte
		If Not KizuLibCLI.KizuMod.ksMemRead(name, size, bwk) Then
			MsgBox("���L�������A�N�Z�X���s")
			Return
		End If

		' �ǂݍ��񂾃f�[�^�� �Z�b�g
		ReDim m_bData(size - 1)
		m_bData = bwk

		' �O���b�h�Z�b�g
		setdata(dgvList, m_bData)
	End Sub

	''' <summary>
	''' DB�Ǎ�
	''' </summary>
	Private Sub btnDbRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnDbRead.Click
		Dim strAddless As String = txtDbReadAddless.Text.Trim
		Dim strDbName As String = txtDbReadDbName.Text.Trim
		Dim strUser As String = txtDbReadUser.Text.Trim
		Dim strPass As String = txtDbReadPass.Text.Trim
		Dim strSQL As String = txtDbReadSql.Text

		Dim bwk() As Byte = Nothing

		'' SQLServer�ڑ�
        Dim tcls_db As tClass.tClass_SQLServer = New tClass.tClass_SQLServer(strAddless, strDbName, strUser, strPass)
        tcls_db.Open()
        Dim o As Object
        o = tcls_db.GetSelectKey(strSQL)
        If o Is Nothing Then
            tcls_db.Close()
            MsgBox("DB��� �擾���s")
            Return
        End If
        tcls_db.Close()

		' �ǂݍ��񂾃f�[�^�� �Z�b�g
        bwk = CType(o, Byte())
        ReDim m_bData(bwk.Length - 1)
		m_bData = bwk

		' �O���b�h�Z�b�g
		setdata(dgvList, m_bData)
	End Sub

    ''' <summary>
    ''' KizuLibCLI.DLL�̓ǂݍ���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="args"></param>
    ''' <returns></returns>
    ''' <remarks>32bit/64bit�؂�ւ��Ή�</remarks>
    Private Function OnAssemblyResolve(ByVal sender As Object, ByVal args As System.ResolveEventArgs) As Assembly

        If args.Name.ToUpper.StartsWith("KIZULIBCLI") Then

            If IntPtr.Size = 8 Then
                ' 64bit��
                Return Assembly.LoadFile(IO.Path.Combine(Application.StartupPath, "KizuLibCLIx64.dll"))
            Else
                ' 32bit��
                Return Assembly.LoadFile(IO.Path.Combine(Application.StartupPath, "KizuLibCLIx32.dll"))
            End If

        End If

        Return Nothing

    End Function
End Class
