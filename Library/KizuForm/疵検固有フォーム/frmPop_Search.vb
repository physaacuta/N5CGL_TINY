
'*********************************************************************************
' �������ь�����ʂ̐e�N���X
'	[Ver]
'		Ver.01    2007/08/29  vs2005 �Ή�
'
'	[����]
'		�˒J�� ����ŁB    frmPop_Search2 �̕����g�p����悤�ɂ��Ă��������B
'*********************************************************************************
Option Strict On
Imports tClassLibrary
Public Class frmPop_Search

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �\����
    '/ /////////////////////////////////////////////////////////////////////////////////

    '�r��E�O���[�h�t�B���^�[��
    Public Enum EM_FILTER_COL                ' �r��
        nCheck = 0                           ' �`�F�b�N�{�b�N�X
        nName                                ' ����
        nEdasysID                            ' EdasysID
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o �ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_strSection As String          '�r�탊�X�g�̃`�F�b�N���̃Z�N�V������
    Private m_strTypeKey As String          '�r�탊�X�g�̃`�F�b�N����ۑ�����̃L�[��
    Private m_strGradeKey As String         '�O���[�h���X�g�̃`�F�b�N����ۑ�����L�[��
	Private m_sDbSession As String = INI_DB									' DB�Z�b�V����
    Private m_strIniName As String = LOCALINI_NAME  '�`�F�b�N����ۑ�����INI�t�@�C����

    ''' <summary>
    ''' �r��E�O���[�h���X�g�̃`�F�b�N����ۑ�����Z�N�V���������Z�b�g
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.DefaultValue(DB_PC_INF) _
     , System.ComponentModel.Category("���������ь�����ʐ�p�v���p�e�B") _
     , System.ComponentModel.Description("�r��E�O���[�h���X�g�̃`�F�b�N����ۑ�����Z�N�V������")> _
    Public Property tProp_IniSection() As String
        Get
            Return m_strSection
        End Get
        Set(ByVal Value As String)
            m_strSection = Value
        End Set
    End Property

    ''' <summary>
    ''' �r��`�F�b�N����ۑ�����L�[�����Z�b�g
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.DefaultValue(DB_PC_INF) _
     , System.ComponentModel.Category("���������ь�����ʐ�p�v���p�e�B") _
     , System.ComponentModel.Description("�r��`�F�b�N����ۑ�����L�[��")> _
    Public Property tProp_TypeKey() As String
        Get
            Return m_strTypeKey
        End Get
        Set(ByVal Value As String)
            m_strTypeKey = Value
        End Set
    End Property

    ''' <summary>
    ''' �r��`�F�b�N����ۑ�����L�[�����Z�b�g
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.DefaultValue(DB_PC_INF) _
     , System.ComponentModel.Category("���������ь�����ʐ�p�v���p�e�B") _
     , System.ComponentModel.Description("�r��`�F�b�N����ۑ�����L�[��")> _
    Public Property tProp_GradeKey() As String
        Get
            Return m_strGradeKey
        End Get
        Set(ByVal Value As String)
            m_strGradeKey = Value
        End Set
    End Property


	''' <summary>
	''' DB�Z�N�V����
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.DefaultValue(INI_DB) _
     , System.ComponentModel.Category("���������ь�����ʐ�p�v���p�e�B") _
     , System.ComponentModel.Description("DB�Z�b�V����")> _
    Public Property tProp_DbSession() As String
        Get
            Return m_sDbSession
        End Get
        Set(ByVal Value As String)
            m_sDbSession = Value
        End Set
    End Property

    ''' <summary>
    ''' �`�F�b�N����ۑ�����INI�t�@�C����
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.DefaultValue(INI_DB) _
     , System.ComponentModel.Category("���������ь�����ʐ�p�v���p�e�B") _
     , System.ComponentModel.Description("�`�F�b�N����ۑ�����INI�t�@�C����")> _
    Public Property tProp_IniName() As String
        Get
            Return m_strIniName
        End Get
        Set(ByVal Value As String)
            m_strIniName = Value
        End Set
    End Property


    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub New()

        ' ���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
        InitializeComponent()

        ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B

        '�r��E�O���[�h�t�B���^�[������
        Call dgvInitializeFilterGrid(dgvDispType, "�r�햼��")
        Call dgvInitializeFilterGrid(dgvDispGrade, "�O���[�h����")

        '�r��S�I���A�S�����ɊY���O���b�h�����Z�b�g
        btnTypeAllSelect.Tag = dgvDispType
        btnTypeAllCancel.Tag = dgvDispType

        '�O���[�h�S�I���A�S�����ɊY���O���b�h�����Z�b�g
        btnGradeAllSelect.Tag = dgvDispGrade
        btnGradeAllCancel.Tag = dgvDispGrade

	End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �r��t�B���^�[ �O���b�h������
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �r��t�B���^�[ �O���b�h������
    ''' </summary>
    ''' <param name="dgv">�t�B���^�[�p�̃f�[�^�O���b�h</param>
    ''' <param name="strName">2��ڂ̍��ږ�</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitializeFilterGrid(ByVal dgv As DataGridView, ByVal strName As String)

        With dgv
            .RowCount = 0
            .ColumnCount = 0

            '' �����`
            .AutoGenerateColumns = False            ' �񎩓����� (false�ɂ����DataSource�������Ȃ��Ȃ�̂Œ���)
            .AllowUserToAddRows = False             ' ���[�U�[ �s�ǉ�
            .AllowUserToDeleteRows = False          ' ���[�U�[ �s�폜
            .AllowUserToOrderColumns = False        ' ���[�U�[ ��ړ�
            .AllowUserToResizeColumns = False        ' ���[�U�[ ��T�C�Y�ύX
            .AllowUserToResizeRows = False          ' ���[�U�[ �s�T�C�Y�ύX
            .ShowCellErrors = False                 ' �Z���G���[��\�����Ȃ�
            .ShowCellToolTips = False               ' �c�[���q���g��\�����Ȃ�
            .ShowEditingIcon = False                ' 
            .ShowRowErrors = False                  '

            .SelectionMode = DataGridViewSelectionMode.CellSelect       ' �Z���I��
            .MultiSelect = False                                        ' �����s�I��
            .ReadOnly = False                                           ' �ǂݎ���p�̓_��
            .EditMode = DataGridViewEditMode.EditOnEnter                ' �Z����������

            .ScrollBars = ScrollBars.Vertical                                                       '�X�N���[���o�[ �c�̂ݗL��
            .ColumnHeadersVisible = True                                                           ' ��w�b�_�\��
            .RowHeadersVisible = False                                                              ' �s�w�b�_�\��
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None                             ' ����T�C�Y��������
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None                                   ' ���s�T�C�Y��������
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

            '''' ��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            '' �I���Z���F��`
			.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue		' �I��
            .DefaultCellStyle().SelectionForeColor = Color.Black            '
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1�s�����ɐF�ւ�
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point) '
            .DefaultCellStyle().ForeColor = Color.Black

            ''''' ��ǉ�
            .Columns.Add(New DataGridViewCheckBoxColumn)  ' �`�F�b�N�{�b�N�X
            .Columns.Add(New DataGridViewTextBoxColumn)   ' ����
            .Columns.Add(New DataGridViewTextBoxColumn)   ' EdasysID

            '''' ���`
            '1���:�`�F�b�N�{�b�N�X
            .Columns(EM_FILTER_COL.nCheck).HeaderText = ""
            .Columns(EM_FILTER_COL.nCheck).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_FILTER_COL.nCheck).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(EM_FILTER_COL.nCheck).Width = 20
            .Columns(EM_FILTER_COL.nCheck).SortMode = DataGridViewColumnSortMode.NotSortable

            '2���:�r�햼��
            .Columns(EM_FILTER_COL.nName).HeaderText = strName & vbCrLf
            .Columns(EM_FILTER_COL.nName).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_FILTER_COL.nName).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_FILTER_COL.nName).Width = 260
            .Columns(EM_FILTER_COL.nName).SortMode = DataGridViewColumnSortMode.NotSortable

            '3���:EdasysID
            .Columns(EM_FILTER_COL.nEdasysID).Visible = False   '��\��

        End With
    End Sub

    ''' <summary>
    ''' �r��E�O���[�h�t�B���^�Z�b�g
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h</param>
    ''' <param name="strTable">�e�[�u����</param>
    ''' <param name="strSection">�Z�N�V������</param>
    ''' <param name="strKey">�L�[</param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub SetFilterData(ByVal dgv As DataGridView, ByVal strTable As String, _
                                ByVal strSection As String, ByVal strKey As String)

		Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, m_sDbSession)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String = ""
        Dim iCnt As Integer = 0
        Dim strSetString As String = ""

        'INI�t�@�C���̐ݒ���擾
        strSetString = tClassLibrary.tMod.ReadIniStr(strSection, strKey, m_strIniName)

        'DB�I�[�v��
        If Not tcls_DB.Open() Then Exit Sub

        'SQL���쐬
        strSQL = ""
        strSQL &= " SELECT Name, "
        strSQL &= "        EdasysID "
        strSQL &= " FROM " & strTable
        strSQL &= " ORDER BY ID "

        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try

            While sqlRead.Read()

                dgv.RowCount += 1   '�\���s����ݒ�

                ''�ݒ�s�� �w��
                dgv(EM_FILTER_COL.nName, iCnt).ReadOnly = True

                '�f�[�^�Z�b�g
                dgv(EM_FILTER_COL.nName, iCnt).Value = sqlRead.GetString(0)
                dgv(EM_FILTER_COL.nName, iCnt).Style.WrapMode = DataGridViewTriState.False
                dgv(EM_FILTER_COL.nEdasysID, iCnt).Value = sqlRead.GetInt32(1)

                '���o�^�܂��́A1���ݒ肳��Ă���ꍇ�́A�`�F�b�N������
                If "" = strSetString OrElse "1" = Mid(strSetString, iCnt + 1, 1) Then
                    dgv(EM_FILTER_COL.nCheck, iCnt).Value = True
                Else
                    dgv(EM_FILTER_COL.nCheck, iCnt).Value = False
                End If

                iCnt += 1

            End While

        Catch ex As Exception

        Finally
            ' DB�J��
            If Not sqlRead Is Nothing Then sqlRead.Close()
            Call tcls_DB.Dispose()

        End Try

    End Sub

    ''' <summary>
    ''' �Z���`�F���W����
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgv_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvDispType.CellValueChanged, dgvDispGrade.CellValueChanged

        Dim dgv As DataGridView = CType(sender, DataGridView)

        If e.ColumnIndex = EM_FILTER_COL.nCheck Then

            '�w�b�_�͏������Ȃ�
            If e.RowIndex = -1 Then Exit Sub

            If CBool(dgv(e.ColumnIndex, e.RowIndex).Value) Then
                '�`�F�b�N��t������
                dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.LightGreen
            Else
                '�`�F�b�N���O������
                If e.RowIndex Mod 2 = 0 Then
                    dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.White
                Else
                    dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.AliceBlue
                End If
            End If
        End If

    End Sub

    ''' <summary>
    ''' �Z���̏�Ԃ��ω��������̏���
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub dgvData_CurrentCellDirtyStateChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvDispType.CurrentCellDirtyStateChanged, dgvDispGrade.CurrentCellDirtyStateChanged

        ' �Z�����R�~�b�g����Ă��Ȃ��ύX���܂ޏꍇ
        If dgvDispType.IsCurrentCellDirty Then
            '�R�~�b�g����
            dgvDispType.CommitEdit(DataGridViewDataErrorContexts.Commit)
        End If

        ' �Z�����R�~�b�g����Ă��Ȃ��ύX���܂ޏꍇ
        If dgvDispGrade.IsCurrentCellDirty Then
            '�R�~�b�g����
            dgvDispGrade.CommitEdit(DataGridViewDataErrorContexts.Commit)
        End If

    End Sub


    ''' <summary>
    ''' ��ʏI��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmdEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnEnd.Click

        Me.Hide()

    End Sub
    ''' <summary>
    ''' �S�I���{�^���N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnAllSelect_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTypeAllSelect.Click, btnGradeAllSelect.Click

        '�`�F�b�N�{�b�N�X��S��ON
        Call CheckGrid(CType(CType(sender, Button).Tag, DataGridView), True)

    End Sub

    ''' <summary>
    ''' �O���b�h�̃`�F�b�N�{�b�N�X�Z�b�g
    ''' </summary>
    ''' <param name="dgvData">�f�[�^�O���b�h</param>
    ''' <param name="bValue">True or False</param>
    ''' <remarks></remarks>
    Private Sub CheckGrid(ByVal dgvData As DataGridView, ByVal bValue As Boolean)

        Dim ii As Integer

        ' �O���b�h�̑S�Ẵ`�F�b�N�{�b�N�X��ݒ�
        For ii = 0 To dgvData.RowCount - 1
            dgvData(EM_FILTER_COL.nCheck, ii).Value = bValue
        Next ii

    End Sub
    ''' <summary>
    ''' �S�����{�^���N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnAllCancel_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTypeAllCancel.Click, btnGradeAllCancel.Click

        '�`�F�b�N�{�b�N�X��S��OFF
        Call CheckGrid(CType(CType(sender, Button).Tag, DataGridView), False)

    End Sub

    ''' <summary>
    ''' �����t�B���^�[ �L���^�����ݒ�
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chkTimeSet_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkTimeSet.CheckedChanged

        ' �R���g���[���̗L������
        If chkTimeSet.Checked = True Then
            dtpTimeStart.Enabled = True
            dtpTimeEnd.Enabled = True
        Else
            dtpTimeStart.Enabled = False
            dtpTimeEnd.Enabled = False
        End If

        '���t�t�B���^�[���L���Ȃ�A���t�t�B���^�[���L���ɂ���
        If chkTimeSet.Checked Then If chkDateSet.Checked = False Then chkDateSet.Checked = True

    End Sub
    ''' <summary>
    ''' ���t�t�B���^�[ �L���^�����ݒ�
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chkDateSet_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkDateSet.CheckedChanged

        ' �R���g���[���̗L������
        If chkDateSet.Checked = True Then
            dtpDateStart.Enabled = True
            dtpDateEnd.Enabled = True
        Else
            dtpDateStart.Enabled = False
            dtpDateEnd.Enabled = False
        End If

        '���t�t�B���^�[�������Ȃ�A�����t�B���^�[�������ɂ���
        If Not chkDateSet.Checked Then If chkTimeSet.Checked = True Then chkTimeSet.Checked = False

    End Sub


    ''' <summary>
    ''' ���t�R���g���[���ɑ΂��� �z�C�[���ŉ��Z���Z���\�Ƃ���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dtp_MouseWheel(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles dtpTimeStart.MouseWheel, dtpTimeEnd.MouseWheel, dtpDateStart.MouseWheel, dtpDateEnd.MouseWheel
        ' ���݂�datetimepicker �Ɂ����L�[�������ꂽ���Ƃ���
        SendKeys.Send(CStr(IIf(e.Delta > 0, "{DOWN}", "{UP}")))
    End Sub


	''' <summary>
	''' �r��A�O���[�h�t�B���^�[�̃`�F�b�N�ۑ�
	''' </summary>
	''' <remarks></remarks>
	Public Sub SaveCheck()

		Dim strSetString As String
		Dim ii As Integer

		'�r��,�O���[�h�t�B���^�[�̏�����INI�t�@�C���֓o�^
		With dgvDispType
			strSetString = ""
			For ii = 0 To dgvDispType.RowCount - 1
				strSetString = CStr(IIf(CType(dgvDispType(EM_FILTER_COL.nCheck, ii).Value, Boolean), strSetString & "1", strSetString & "0"))
			Next ii
            tClassLibrary.tMod.WriteIni(m_strSection, m_strTypeKey, m_strIniName, strSetString)
		End With

		With dgvDispGrade
			strSetString = ""
			For ii = 0 To dgvDispGrade.RowCount - 1
				strSetString = CStr(IIf(CType(dgvDispGrade(EM_FILTER_COL.nCheck, ii).Value, Boolean), strSetString & "1", strSetString & "0"))
			Next ii
            tClassLibrary.tMod.WriteIni(m_strSection, m_strGradeKey, m_strIniName, strSetString)
		End With

	End Sub

	'' 20130522 �J���}����������ׂɈ����ǉ�
	''' <summary>
	''' CSV�o��
	''' </summary>
	''' <param name="strFolder">�ۑ��t�@�C���̃t�H���_�p�X</param>
	''' <param name="strName">�ۑ��t�@�C���̃t�@�C����</param>
	''' <param name="bDelComma">�J���}������</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub CsvOut(ByVal strFolder As String, ByVal strName As String, Optional ByVal bDelComma As Boolean = False)
		Dim strFPath As String
		Dim strMsg As String = ""
		Dim strData As String = ""						'1�s�̃f�[�^
		Dim strWk() As String
		Dim strWkData As String

		'' �t���p�X
		strFPath = IO.Path.Combine(strFolder, strName)

		'' �t�H���_�̗L���`�F�b�N(�����������낤)
		tMod.CheckFolder(strFolder)

		'' CSV�o��
		Using cls_Stram As New tClass.tClass_FileStream(strFPath, IO.FileMode.Create, IO.FileAccess.ReadWrite, IO.FileShare.ReadWrite)
			Try
				' �I�[�v��
				cls_Stram.Open()



				'�w�b�_���o��
				strData = ""
				For ii As Integer = 0 To dgvResult.ColumnCount - 1

					'���s�R�[�h�����ׂĎ�菜��
					strWk = dgvResult.Columns(ii).HeaderText.Split(Chr(10), Chr(13))
					strWkData = ""
					For jj As Integer = 0 To strWk.Length - 1
						strWkData &= strWk(jj).Trim(Chr(10), Chr(13))
					Next jj

					strData &= Trim(strWkData) & ","
				Next ii

				'�Ō�̃^�u��r��
				strData = strData.Remove(strData.Length - 1, 1)

				cls_Stram.WriteLine(strData)

				'�f�[�^�o��
				For ii As Integer = 0 To dgvResult.Rows.Count - 1
					strData = ""
					For jj As Integer = 0 To dgvResult.ColumnCount - 1
						'���ڂ�Tab�ŘA������
						'' 20130522�ύX �J���}������------------------------>>>
						If True = bDelComma Then
							strData &= dgvResult(jj, ii).Value.ToString.Replace(",", "") & ","
						Else
							strData &= dgvResult(jj, ii).Value.ToString & ","
						End If
						'' <<<-------------------------------------------------
					Next jj

					'�Ō�̃^�u��r��
					strData = strData.Remove(strData.Length - 1, 1)

					cls_Stram.WriteLine(strData)
				Next ii

				' �N���[�Y
				cls_Stram.Close()
				strMsg = System.IO.Path.GetFullPath(strFPath) & "��CSV�t�@�C����ۑ����܂����B"

			Catch ex As Exception
				strMsg = "CSV�o�͂Ɏ��s���܂����B"
			End Try
		End Using

		'''' �K�C�_���X�\��
		Using frmMsg As New tClassForm.frmPop_MsgBox(strMsg, Me.Text)
			frmMsg.ShowDialog()
		End Using
	End Sub


End Class
