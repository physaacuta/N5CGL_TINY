
'*********************************************************************************
' �������ь�����ʂ̐e�N���X
'	[Ver]
'		Ver.01    2008/11/05  vs2005 �Ή�
'
'	[����]
'		frmPop_Search �����ėp�������o�[�W�����B �ɗ͂�������g�p���Ă��������B
'*********************************************************************************
Option Strict On
Imports tClassLibrary

Public Class frmPop_Search2

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �\����
	'/ /////////////////////////////////////////////////////////////////////////////////

	'�r��E�O���[�h�t�B���^�[��
	Public Enum EM_FILTER_COL						' �r��
		emCheck = 0									' �`�F�b�N�{�b�N�X
		emName										' ����
		emEdasysID									' EdasysID (��L�[)
	End Enum


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ���J�����o �ϐ�
	'/ /////////////////////////////////////////////////////////////////////////////////
	Protected my_bCancel As Boolean = False					' �L�����Z���� true


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �����o �ϐ�
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private m_strSettingIniName As String = LOCALINI_NAME	' �ݒ���ۑ� INI�t�@�C����
	Private m_strSettingSession As String					' �ݒ���ۑ� �Z�N�V������

	Private m_sDbSession As String = INI_DB					' DB�Z�b�V����

	Private m_nCoilRowNext As Integer = -1					' ���R�C���̍s�ʒu (���s)
	Private m_nCoilRowPre As Integer = -1					' �O�R�C���̍s�ʒu (��s)



	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �v���p�e�B
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �ݒ���ۑ� INI�t�@�C����
	''' </summary>
	''' <value></value>
	<System.ComponentModel.Browsable(True) _
	  , System.ComponentModel.DefaultValue(INI_DB) _
	  , System.ComponentModel.Category("���������ь�����ʐ�p�v���p�e�B") _
	  , System.ComponentModel.Description("�ݒ���ۑ� INI�t�@�C����")> _
	 Public Property tProp_SettingIniName() As String
		Get
			Return m_strSettingIniName
		End Get
		Set(ByVal Value As String)
			m_strSettingIniName = Value
		End Set
	End Property

	''' <summary>
	''' �ݒ���ۑ� �Z�N�V������
	''' </summary>
	''' <value></value>
	<System.ComponentModel.Browsable(True) _
	  , System.ComponentModel.DefaultValue(DB_PC_INF) _
	  , System.ComponentModel.Category("���������ь�����ʐ�p�v���p�e�B") _
	  , System.ComponentModel.Description("�ݒ���ۑ� �Z�N�V������")> _
	 Public Property tProp_SettingIniSession() As String
		Get
			Return m_strSettingSession
		End Get
		Set(ByVal Value As String)
			m_strSettingSession = Value
		End Set
	End Property


	''' <summary>
	''' DB�Z�N�V����
	''' </summary>
	''' <value></value>
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
	''' ���R�C���̍s�ʒu (���s) (������-1)
	''' </summary>
	''' <value></value>
	Public Property tProp_CoilRowNext() As Integer
		Get
			Return m_nCoilRowNext
		End Get
		Set(ByVal Value As Integer)
			m_nCoilRowNext = Value
		End Set
	End Property

	''' <summary>
	''' �O�R�C���̍s�ʒu (��s) (������-1)
	''' </summary>
	''' <value></value>
	Public Property tProp_CoilRowPre() As Integer
		Get
			Return m_nCoilRowPre
		End Get
		Set(ByVal Value As Integer)
			m_nCoilRowPre = Value
		End Set
	End Property


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ����
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �R���X�g���N�^
	''' </summary>
	''' <remarks></remarks>
	Public Sub New()

		' ���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
		InitializeComponent()

		' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B

		'''' �������� �O���b�h�r���[
		Call dgvInitialize_Result(dgvResult)

		'''' �t�B���^�[�p �O���b�h�r���[
		Call dgvInitialize_FilterGrid(dgvDispType)
		Call dgvInitColumn_FilterGrid(dgvDispType, "�r�햼��")
		Call dgvFilter_AddHandler(dgvDispType)

		Call dgvInitialize_FilterGrid(dgvDispGrade)
		Call dgvInitColumn_FilterGrid(dgvDispGrade, "�O���[�h����")
		Call dgvFilter_AddHandler(dgvDispGrade)

		'�r��S�I���A�S�����ɊY���O���b�h�����Z�b�g
		btnTypeAllSelect.Tag = dgvDispType
		btnTypeAllCancel.Tag = dgvDispType

		'�O���[�h�S�I���A�S�����ɊY���O���b�h�����Z�b�g
		btnGradeAllSelect.Tag = dgvDispGrade
		btnGradeAllCancel.Tag = dgvDispGrade
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


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ���X�g�T�[�`
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' ���݂̃R�C�����X�g�̑O����`�F�b�N
	''' </summary>
	''' <remarks></remarks>
	Protected Friend Overridable Sub CheckCoilRowNextPre()
		'''' �����ł́A���X�g�\������Ă�����̂��S�đΏۂƔ��f���Ă���B
		' �_���Ȃ珟��ɃV���h�E�Ȃ�I�[�o�[���C�h�Ȃ肵�Ă��������B

		Dim nRow As Integer		' ���ݍs
		Dim nWk As Integer
		Try
			'���ݍs���擾
			nRow = dgvResult.CurrentCell.RowIndex
			If -1 = nRow Then
				Me.tProp_CoilRowNext = -1
				Me.tProp_CoilRowPre = -1
				Return
			End If

			' ���̃R�C������
			nWk = nRow + 1
			If nWk > dgvResult.RowCount - 1 Then
				Me.tProp_CoilRowNext = -1
			Else
				Me.tProp_CoilRowNext = nWk
			End If

			' �O�̃R�C������
			nWk = nRow - 1
			If nWk < 0 Then
				Me.tProp_CoilRowPre = -1
			Else
				Me.tProp_CoilRowPre = nWk
			End If

		Catch ex As Exception
		End Try
	End Sub



	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �O���b�h������
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �r��t�B���^�[ �O���b�h������
	''' </summary>
	''' <param name="dgv">�t�B���^�[�p�̃f�[�^�O���b�h</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub dgvInitialize_FilterGrid(ByVal dgv As tCnt.tCnt_DataGridView)
		'''' ��{��ݒ�
		Call dgv.dgvInitialize(dgv)

		'''' �Ǝ��̍��ڂ�ݒ�
		With dgv
			' '' ��`
			'.ColumnHeadersHeight = 25										' �w�b�_�[�̍���
			.SelectionMode = DataGridViewSelectionMode.FullRowSelect		' �s�I��
			.ScrollBars = ScrollBars.Vertical								' �X�N���[���o�[�̕\��

			'' �I���Z���F��`
			.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue		' �I��
			.DefaultCellStyle().SelectionForeColor = Color.Black			'
			.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue	' 1�s�����ɐF�ւ�
			.DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)	'
			.DefaultCellStyle().ForeColor = Color.Black

			.RowCount = 0
		End With
	End Sub

	''' <summary>
	''' �������� �O���b�h������
	''' </summary>
	''' <param name="dgv"></param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub dgvInitialize_Result(ByVal dgv As tCnt.tCnt_DataGridView)
		'''' ��{��ݒ�
		Call dgv.dgvInitialize(dgv)

		'''' �Ǝ��̍��ڂ�ݒ�

		With dgv
			'' ��`
			.ColumnHeadersHeight = 45										' �w�b�_�[�̍���
			.SelectionMode = DataGridViewSelectionMode.FullRowSelect		' �s�I��
			.ScrollBars = ScrollBars.Both

			'' �I���Z���F��`
			.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue		' �I��
			'.DefaultCellStyle().SelectionBackColor = Color.Transparent		' �I��F ������ (dgvLog_SelectionChanged���K�{)
			.DefaultCellStyle().SelectionForeColor = Color.Black			'
			.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue	' 1�s�����ɐF�ւ�
			.DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)	'
			.DefaultCellStyle().ForeColor = Color.Black

			.RowCount = 0
		End With

	End Sub


	''' <summary>
	''' �r��t�B���^�[ �O���b�h���`
	''' </summary>
	''' <param name="dgv">�t�B���^�[�p�̃f�[�^�O���b�h</param>
	''' <param name="strName">2��ڂ̍��ږ�</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub dgvInitColumn_FilterGrid(ByVal dgv As DataGridView, ByVal strName As String)
		With dgv
			''''' ��ǉ�
			.Columns.Add(New DataGridViewCheckBoxColumn)  ' �`�F�b�N�{�b�N�X
			.Columns.Add(New DataGridViewTextBoxColumn)	  ' ����
			.Columns.Add(New DataGridViewTextBoxColumn)	  ' EdasysID

			'''' ���`
			'1���:�`�F�b�N�{�b�N�X
			.Columns(EM_FILTER_COL.emCheck).HeaderText = ""
			.Columns(EM_FILTER_COL.emCheck).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			.Columns(EM_FILTER_COL.emCheck).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
			.Columns(EM_FILTER_COL.emCheck).Width = 20
			.Columns(EM_FILTER_COL.emCheck).SortMode = DataGridViewColumnSortMode.NotSortable

			'2���:�r�햼��
			.Columns(EM_FILTER_COL.emName).HeaderText = strName & vbCrLf
			.Columns(EM_FILTER_COL.emName).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
			.Columns(EM_FILTER_COL.emName).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
			.Columns(EM_FILTER_COL.emName).Width = .ClientSize.Width - 20 - 3
			.Columns(EM_FILTER_COL.emName).SortMode = DataGridViewColumnSortMode.NotSortable

			'3���:EdasysID
			.Columns(EM_FILTER_COL.emEdasysID).Visible = False	'��\��
		End With
	End Sub




	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �t�B���^�[�O���b�h �����`
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �O���b�h���� �n���h�����蓖��
	''' </summary>
	''' <param name="dgv"></param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub dgvFilter_AddHandler(ByVal dgv As tCnt.tCnt_DataGridView)
		AddHandler dgv.CellValueChanged, AddressOf dgv_CellValueChanged
		AddHandler dgv.CurrentCellDirtyStateChanged, AddressOf dgv.dgvData_CurrentCellDirtyStateChanged
	End Sub


	''' <summary>
	''' �Z���`�F���W����
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub dgv_CellValueChanged(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
		Dim dgv As DataGridView = CType(sender, tCnt.tCnt_DataGridView)
		'�w�b�_�͏������Ȃ�
		If e.RowIndex = -1 Then Exit Sub

		If e.ColumnIndex = EM_FILTER_COL.emCheck Then
			If CBool(dgv(e.ColumnIndex, e.RowIndex).Value) Then
				'' �`�F�b�N��t������
				dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.LightGreen
				dgv.Rows(e.RowIndex).DefaultCellStyle().SelectionBackColor = Color.LightGreen
			Else
				'' �`�F�b�N���O������
				dgv.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.Empty
				dgv.Rows(e.RowIndex).DefaultCellStyle().SelectionBackColor = Color.Empty
			End If
		End If

	End Sub




	''' <summary>
	''' �S�I���{�^���N���b�N
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnAllSelect_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTypeAllSelect.Click, btnGradeAllSelect.Click

		'�`�F�b�N�{�b�N�X��S��ON
		Call CheckGridAllChange(CType(CType(sender, Button).Tag, DataGridView), True)

	End Sub
	''' <summary>
	''' �S�����{�^���N���b�N
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnAllCancel_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnTypeAllCancel.Click, btnGradeAllCancel.Click

		'�`�F�b�N�{�b�N�X��S��OFF
		Call CheckGridAllChange(CType(CType(sender, Button).Tag, DataGridView), False)

	End Sub

	''' <summary>
	''' �O���b�h�̃`�F�b�N�{�b�N�X�Z�b�g
	''' </summary>
	''' <param name="dgvData">�f�[�^�O���b�h</param>
	''' <param name="bValue">True or False</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub CheckGridAllChange(ByVal dgvData As DataGridView, ByVal bValue As Boolean)

		Dim ii As Integer

		' �O���b�h�̑S�Ẵ`�F�b�N�{�b�N�X��ݒ�
		For ii = 0 To dgvData.RowCount - 1
			dgvData(EM_FILTER_COL.emCheck, ii).Value = bValue
		Next ii

	End Sub


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �t�B���^�[ ���
	'/ /////////////////////////////////////////////////////////////////////////////////


	''' <summary>
	''' �r��E�O���[�h�t�B���^�Z�b�g
	''' </summary>
	''' <param name="dgv">�f�[�^�O���b�h</param>
	''' <param name="strTable">�e�[�u����</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub SetFilterData(ByVal dgv As DataGridView, ByVal strTable As String)
		Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, m_sDbSession)
		Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
		Dim strSQL As String = ""
		Dim iCnt As Integer = 0

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

				dgv.RowCount += 1	'�\���s����ݒ�

				''�ݒ�s�� �w��
				dgv(EM_FILTER_COL.emName, iCnt).ReadOnly = True

				'�f�[�^�Z�b�g
				dgv(EM_FILTER_COL.emName, iCnt).Value = sqlRead.GetString(0)
				dgv(EM_FILTER_COL.emName, iCnt).Style.WrapMode = DataGridViewTriState.False
				dgv(EM_FILTER_COL.emCheck, iCnt).Value = False
				dgv(EM_FILTER_COL.emEdasysID, iCnt).Value = sqlRead.GetInt32(1)

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
	''' �t�B���^�[�̃`�F�b�N�ۑ�
	''' </summary>
	''' <param name="dgv">�f�[�^�O���b�h</param>
	''' <param name="strInikey">ini�t�@�C���̑ΏۃL�[</param>
	''' <remarks></remarks>
	Protected Sub SaveFilterData(ByVal dgv As DataGridView, ByVal strInikey As String)
		Dim strVal As String = ""

		' �r��,�O���[�h�t�B���^�[�̏�����INI�t�@�C���֓o�^
		For ii As Integer = 0 To dgv.RowCount - 1
			strVal &= CStr(IIf(CType(dgv(EM_FILTER_COL.emCheck, ii).Value, Boolean), "1", "0"))
		Next ii

		tClassLibrary.tMod.WriteIni(m_strSettingSession, strInikey, m_strSettingIniName, strVal)
	End Sub

	''' <summary>
	''' �t�B���^�[�̃`�F�b�N�f�[�^�Ǎ�
	''' </summary>
	''' <param name="dgv"></param>
	''' <param name="strInikey"></param>
	''' <remarks></remarks>
	Protected Sub ReadFilterData(ByVal dgv As DataGridView, ByVal strInikey As String)
		' INI�t�@�C���̐ݒ���擾
		Dim strVal As String = tClassLibrary.tMod.ReadIniStr(m_strSettingSession, strInikey, m_strSettingIniName)
		If "" = strVal Then Exit Sub

		' �ݒ�l�𔽉f
		For ii As Integer = 0 To dgv.RowCount - 1

			'���o�^�܂��́A1���ݒ肳��Ă���ꍇ�́A�`�F�b�N������
			If "1" = Mid(strVal, ii + 1, 1) Then
				dgv(EM_FILTER_COL.emCheck, ii).Value = True
			Else
				dgv(EM_FILTER_COL.emCheck, ii).Value = False
			End If
		Next ii
	End Sub


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ������ �v���O���X�o�[ �֌W
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' ������ �v���O���X�o�[ �\����ԕύX
	''' </summary>
	''' <param name="blnExec"></param>
	''' <remarks></remarks>
	Public Sub WaitControl_EnableMode(ByVal blnExec As Boolean)

		' �R���g���[���̏�ԕύX
		pnlMsg.Visible = blnExec

		btnEnd.Enabled = Not blnExec
		btnSearch.Enabled = Not blnExec
		btnSelect.Enabled = Not blnExec
		dgvResult.Enabled = Not blnExec

		' �L�����Z���t���O�Z�b�g
		my_bCancel = Not blnExec
	End Sub

	''' <summary>
	''' ������ �v���O���X�o�[ �\��
	''' </summary>
	''' <param name="strVal">�\�����b�Z�[�W</param>
	''' <param name="nMax">�ő�l</param>
	''' <remarks></remarks>
	Public Sub WaitControl_Start(ByVal strVal As String, ByVal nMax As Long)
		txtMsg.Text = strVal
		prbMsg.Minimum = 0
        prbMsg.Maximum = CInt(IIf(nMax > prbMsg.Minimum, CType(nMax, Integer), CType(prbMsg.Minimum + 1, Integer)))
		prbMsg.Value = 0

		' ���̂Ƃ��́A�L�����Z���{�^������������悤�ɂ������ׁADoEvent���g�p����
		Application.DoEvents()
	End Sub

	''' <summary>
	''' ������ �v���O���X�o�[ �X�V
	''' </summary>
	''' <param name="nVal">�i���l</param>
	''' <remarks></remarks>
	Public Sub WaitControl_Running(ByVal nVal As Integer)

		'�i���l�Z�b�g
		prbMsg.Value = CInt(IIf(nVal < prbMsg.Maximum, nVal, prbMsg.Maximum))
		'prbMsg.Refresh()
		' ���̂Ƃ��́A�L�����Z���{�^������������悤�ɂ������ׁADoEvent���g�p����
		Application.DoEvents()
	End Sub
	Public Sub WaitControl_Running()
		Call WaitControl_Running(prbMsg.Value + 1)
	End Sub


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ���̑��������
	'/ /////////////////////////////////////////////////////////////////////////////////
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
