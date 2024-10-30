Option Strict On

Imports tClassLibrary
Imports tClassLibrary.tClass

Public Class frmPopCoilItem


    Private Enum EM_GRID_COL_ID
        em_txtName = 0		'����
        em_txtData			'�f�[�^
        em_End
    End Enum


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�

    ' �N���X
    Private mcls_pRpiView As tClass_ViewRpiThread = Nothing				' RPI�t�@�C���\���N���X (�|�C���^)

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �v���p�e�B

    ''' <summary>
    ''' RPI�N���X
    ''' </summary>
    ''' <param name="cls">RPI�N���X</param>
    ''' <remarks></remarks>
    Public Sub SetCls_RpiView(ByRef cls As tClass_ViewRpiThread)
        mcls_pRpiView = cls
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ����

    ''' <summary>
    ''' �ĕ\��
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ReDsp()
        ' dgv�����ݒ�
        Call DgvInit(dgvCoil)
        ' �񖼂��擾
        Call GetClmSQL()

        ' �f�[�^�Z�b�g
        Call DspUpDate()

        ' �Ǝ��ǉ�
        Call DspUpDateItem()
    End Sub


    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    Private Sub frmPopCoilItem_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        SetFormLocationAtOnce = True
        ' �f�[�^�\��
        Call ReDsp()
    End Sub
    ''' <summary>
    ''' �����`
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Sub DgvInit(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)

        Call dgv.dgvInitialize(dgv)

        With dgv

            .ScrollBars = ScrollBars.Vertical								   ' �����X�N���[���o�[�̕\��
            .ColumnHeadersVisible = False									   ' ��w�b�_�\��
            .AllowUserToResizeColumns = True								' ���[�U�[ ��T�C�Y�ύX

            ' �I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor

            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue	' 1�s�����ɐF�ւ�
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)

            .DefaultCellStyle().ForeColor = Color.Black

            ' �t�H�[���̃T�C�Y�ɍ��킹�ĉ������ϓ��Ɋ���t���鏈��
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.Fill

            '' ���`
            .ColumnCount = EM_GRID_COL_ID.em_End		'textbox �݂̂�ǉ�����ꍇ�͂���̕����y
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft

            '��0 (�A�h���X) ����
            .Columns(EM_GRID_COL_ID.em_txtName).HeaderText = "����"
            .Columns(EM_GRID_COL_ID.em_txtName).ReadOnly = False
            .Columns(EM_GRID_COL_ID.em_txtName).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_GRID_COL_ID.em_txtName).SortMode = DataGridViewColumnSortMode.NotSortable

            '��1 (�A�h���X) �f�[�^
            .Columns(EM_GRID_COL_ID.em_txtData).HeaderText = "�l"
            .Columns(EM_GRID_COL_ID.em_txtData).ReadOnly = False
            .Columns(EM_GRID_COL_ID.em_txtData).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_GRID_COL_ID.em_txtData).SortMode = DataGridViewColumnSortMode.NotSortable
        End With
    End Sub

    ''' <summary>
    ''' DB���ږ��擾
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
            ' DB�I�[�v��
            If Not tcls_DB.Open() Then Exit Sub

            ' Select�����s 
            sqlData.Clear()
            sqlAdpter = tcls_DB.SelectAdapter(sSQL)
            sqlAdpter.Fill(sqlData)

            ' DB���ږ��̃Z�b�g
            Dim sqlGet() As DataRow = sqlData.Tables(0).Select()

            ' dgv�\������
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
    ''' �R�C�����ꗗ��\��
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
            ' DB�ڑ�
            If Not tcls_DB.Open() Then Return

            'SQL���s
            sqlRead = tcls_DB.SelectExecute(sSQL)
            If sqlRead Is Nothing Then Return
            If Not sqlRead.Read() Then Return

            ' �f�[�^�擾
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
    ''' �g���R�C�����ꗗ��\��
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
            ' DB�ڑ�
            If Not tcls_DB.Open() Then Return

            'SQL���s
            sqlRead = tcls_DB.SelectExecute(sSQL)
            If sqlRead Is Nothing Then Return

            ' �f�[�^�擾
            dgvCoil.Enabled = False
            Dim nRowCnt As Integer = 0
            While sqlRead.Read()
                ' �f�[�^�擾
                nRowCnt += 1

                For ii As Integer = 0 To sqlRead.FieldCount() - 1
                    dgvCoil.RowCount += 1

                    sWk = String.Format("[{0}����] {1}", nRowCnt, sqlRead.GetName(ii))
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