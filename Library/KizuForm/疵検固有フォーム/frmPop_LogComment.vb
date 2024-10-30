'*********************************************************************************
' �̏Ⴭ�O �R�����g�ǉ����
'	[Ver]
'		Ver.01    2009/09/14  vs2005 �Ή�
'
'	[����]
'		LOG_INF �Ɏ�L�[�ݒ肳��Ă���^�C�v(AutoNo���ǉ�)�̂ݎg�p�\
'*********************************************************************************
Option Strict On

Imports tClassLibrary

''' <summary>
''' �̏Ⴭ�O �R�����g�ǉ����
''' </summary>
''' <remarks></remarks>
Public Class frmPop_LogComment

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ���J
	'/ /////////////////////////////////////////////////////////////////////////////////
	' �폜�A�ҏW�̎��̃L�[�Ƃ���
	Public Structure typSelectRowData
		Dim nAutoNo As Integer												' AutoNo
		Dim nSysNo As Integer												' �A���[��No (�V�K���A0)
		Dim dayAddTime As Date												' �I�������o�^���t
		Dim dayDspTime As Date												' �I�������\������

		' �V�K�� �ȍ~�󔒂�OK
		Dim sTaskName As String												' ������
		Dim sPcName As String												' PC��
		Dim nPcKind As Integer												' PCKIND
		Dim sMsg As String													' �\�����b�Z�[�W
	End Structure


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �����o �ϐ�
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private mcls_DB As tClass.tClass_SQLServer								' DB
	Private m_sDbSession As String = INI_DB									' DB�Z�b�V����

	Private m_typSelectRowData As typSelectRowData							' �I���s�̏��
	'Private m_nSelectAutoNo As Integer										' �I�����Ă������AutoNo(��L�[�̈ꕔ)

	Private m_nLogModeSyubetu() As Integer = {SYSNO_COMMENT_ALL, SYSNO_COMMENT_SOFT, SYSNO_COMMENT_HARD} ' ���O��� (�S��, �\�t�g, �n�[�h)
	Private m_nLogKindKubun As Integer = 4									' ���O�敪 (4:�R�����g)
	Private m_bDataUpDate As Boolean = False								' ��������̕ύX�������{�� true


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �v���p�e�B
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' DB�Z�N�V����
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(INI_DB) _
	 , System.ComponentModel.Category("��Log��ʐ�p�v���p�e�B") _
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
	''' �I���s���
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	Public Property tProp_SelectRowData() As typSelectRowData
		Get
			Return m_typSelectRowData
		End Get
		Set(ByVal Value As typSelectRowData)
			m_typSelectRowData = Value
		End Set
	End Property

	''' <summary>
	''' �ύX��Ԏ擾
	''' </summary>
	Public ReadOnly Property Prop_IsDataUpDate() As Boolean
		Get
			Return m_bDataUpDate
		End Get
	End Property

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �����o �ϐ�
	'/ /////////////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' �t�H�[�����[�h
	''' </summary>
	Private Sub frmPop_LogComment_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
		'''' �t�@���N�V�����{�^����`
		MyBase.AddFunctionButton(btnInsert, 0)
		MyBase.AddFunctionButton(btnUpdate, 1)
		MyBase.AddFunctionButton(btnDelete, 3)
		MyBase.AddFunctionButton(btnClose, 11)


		'''' �f�[�^�x�[�X�ڑ�
		mcls_DB = New tClass.tClass_SQLServer(TASKINI_NAME, m_sDbSession)
		If Not mcls_DB.Open() Then
			Using frmMsg As New tClassForm.frmPop_MsgBox("�f�[�^�x�[�X�ڑ����s�B", "�ُ�", Color.Red.ToArgb, True, False)
				frmMsg.ShowDialog()
			End Using
			Return
		End If

		'''' ��ʏ����Z�b�g
		Call DspInit()

		'''' ��ʏ����\��
		Call DspSelect()

		''''' AutoNo���擾
		'' �R�����g�̕ҏW���̂�
		'If 0 <> m_typSelectRowData.nSysNo Then
		'	m_nSelectAutoNo = GetSelectAutoNo(m_typSelectRowData.nSysNo, m_typSelectRowData.dayDspTime, m_typSelectRowData.dayAddTime)
		'End If
	End Sub

	''' <summary>
	''' �t�H�[���N���[�Y
	''' </summary>
	Private Sub frmPop_LogComment_FormClosing(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles Me.FormClosing
		mcls_DB.Close()
		mcls_DB.Dispose()
		mcls_DB = Nothing
	End Sub

	''' <summary>
	''' �t�@���N�V�����{�^��
	''' </summary>
	''' <param name="FncNo"></param>
	''' <param name="onKey"></param>
	''' <remarks></remarks>
	Private Sub frmPop_DefectInput_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
		'' �t�@���N�V�����ؑ�
		Select Case FncNo							' 0�I���W��
			'//-----------------------------------------------------------
			Case 1 - 1								  ' �V�K�o�^
				If Not Exec_Insert() Then Return
				Me.Close()

				'//-----------------------------------------------------------
			Case 2 - 1								  ' �ҏW
				If Not Exec_Update() Then Return
				Me.Close()
				'//-----------------------------------------------------------
			Case 4 - 1								  ' �폜
				If Not Exec_Delete() Then Return
				Me.Close()

				'//-----------------------------------------------------------
			Case 12 - 1								  ' �I��
				Me.Close()
		End Select
	End Sub

	''' <summary>
	''' ��ʂ̏����Z�b�g
	''' </summary>
	''' <remarks></remarks>
	Private Sub DspInit()
		Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
		Dim strSql As String = ""

		'''' �敪
		cmbKind.Items.Clear()
		cmbKind.Items.Add(New tClass_CmbItem(m_nLogModeSyubetu(0), "�S��"))
		cmbKind.Items.Add(New tClass_CmbItem(m_nLogModeSyubetu(1), "��ĳ��"))
		cmbKind.Items.Add(New tClass_CmbItem(m_nLogModeSyubetu(2), "ʰ�޳��"))
		cmbKind.SelectedIndex = 0

		'''' PC��
		cmbPcName.Items.Clear()
		cmbPcName.Items.Add(New tClass_CmbItem(0, ""))
		cmbPcName.SelectedIndex = 0

		' Select�����s
		strSql = "SELECT i.PCKIND, m.PC�� "
		strSql &= "FROM " & DB_PC_MASTER & " m "
		strSql &= "LEFT OUTER JOIN " & DB_PC_INF & " i ON m.PCID = i.PCID "
		strSql &= " ORDER BY m.PCID ASC"
		sqlRead = mcls_DB.SelectExecute(strSql)
		If sqlRead Is Nothing Then
			Return
		End If

		'''' �S���擾
		Dim nRowCnt As Integer = 0
		Try
			Dim strWk As String = ""
			While sqlRead.Read()
	
				cmbPcName.Items.Add(New tClass_CmbItem(sqlRead.GetInt32(0), sqlRead.GetValue(1).ToString))

			End While
		Catch ex As Exception
			Return
		Finally
			sqlRead.Close()
			sqlRead = Nothing
		End Try
	End Sub

	''' <summary>
	''' ��ʂ̑I����ԕ\��
	''' </summary>
	''' <remarks></remarks>
	Private Sub DspSelect()
		Dim ii As Integer
		Dim bNew As Boolean	' �V�K�� true

		'' �V�K or �ҏW
		If 0 <> m_typSelectRowData.nSysNo Then
			bNew = False
		Else
			bNew = True
		End If

		'' �{�^�����
		btnUpdate.Enabled = Not bNew
		btnDelete.Enabled = Not bNew

		'' ����
		dtpStartDay.Value = m_typSelectRowData.dayAddTime	' ���ԕ\���́A�o�^���t�̎��Ԃ��g�p����
		dtpStartTime.Value = m_typSelectRowData.dayAddTime

		'''' ����ȍ~�́A�ҏW���̂�
		If bNew Then Return

		'' �敪
		If SYSNO_COMMENT_ALL = m_typSelectRowData.nSysNo Then cmbKind.SelectedIndex = 0
		If SYSNO_COMMENT_SOFT = m_typSelectRowData.nSysNo Then cmbKind.SelectedIndex = 1
		If SYSNO_COMMENT_HARD = m_typSelectRowData.nSysNo Then cmbKind.SelectedIndex = 2

		'' PC����
		For ii = 0 To cmbPcName.Items.Count - 1
			If CType(cmbPcName.Items(ii), tClass_CmbItem).g_sName = m_typSelectRowData.sPcName Then
				cmbPcName.SelectedIndex = ii
				Exit For
			End If
		Next ii

		'' ���̑�
		txtTask.Text = m_typSelectRowData.sTaskName
		txtMsg.Text = m_typSelectRowData.sMsg

	End Sub

	''' <summary>
	''' ���t�R���g���[���ɑ΂��� �z�C�[���ŉ��Z���Z���\�Ƃ���
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub dtp_MouseWheel(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles dtpStartDay.MouseWheel, dtpStartTime.MouseWheel
		' ���݂�datetimepicker �Ɂ����L�[�������ꂽ���Ƃ���
		SendKeys.Send(CStr(IIf(e.Delta > 0, "{DOWN}", "{UP}")))
	End Sub


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ���C������
	'/ /////////////////////////////////////////////////////////////////////////////////
	'''' <summary>
	'''' AutoNo���擾
	'''' </summary>
	'''' <param name="nSysNo">�V�X���ONo</param>
	'''' <param name="datDspTime">�\������</param>
	'''' <param name="datAddTime">�o�^���t</param>
	'''' <returns>AutoNo</returns>
	'''' <remarks></remarks>
	'Private Function GetSelectAutoNo(ByVal nSysNo As Integer, ByVal datDspTime As Date, ByVal datAddTime As Date) As Integer
	'	Dim strSql As String
	'	strSql = "SELECT TOP 1 AutoNo FROM " & DB_LOG_INF
	'	strSql &= " WHERE �������t����='" & datDspTime.ToString("yyyy/MM/dd HH:mm:ss") & "'"
	'	strSql &= " AND �o�^���t='" & datAddTime.ToString("yyyy/MM/dd HH:mm:ss") & "'"
	'	strSql &= " AND �A���[��No=" & nSysNo

	'	Dim nAutoNo As Integer
	'	mcls_DB.GetSelectKey(strSql, nAutoNo)
	'	Return nAutoNo
	'End Function

	''' <summary>
	''' ����AutoNo���Z�o����
	''' </summary>
	''' <param name="datAddTime">�o�^���t</param>
	''' <returns></returns>
	''' <remarks></remarks>
	Private Function GetNextAutoNo(ByVal datAddTime As Date) As Integer
		Dim strSql As String
		' ���̎��Ԃ̈�ԑ傫�Ȓl���擾����
		strSql = "SELECT MAX(AutoNo) FROM " & DB_LOG_INF
		strSql &= " WHERE �o�^���t='" & datAddTime.ToString("yyyy/MM/dd HH:mm:ss") & "'"
		'strSql &= " AND AutoNo%10=9"		

		' ���̒l���Z�o
		Dim nAutoNo As Integer
		mcls_DB.GetSelectKey(strSql, nAutoNo)
		nAutoNo += 10	' ���̒l
		' �R�����g�̏ꍇ AutoNo�̉�1����9�Œ�ɂ���
		If 9 <> (nAutoNo Mod 10) Then
			nAutoNo = CInt(Int(nAutoNo / 10) * 10 + 9)
		End If
		Return nAutoNo
	End Function

	''' <summary>
	''' ���̓`�F�b�N
	''' </summary>
	''' <returns></returns>
	''' <remarks></remarks>
	Private Function CheckInputData(ByRef newTyp As typSelectRowData) As Boolean
		'''' �f�[�^�`�F�b�N
		If 0 <> tClass.tClass_ImputCheck.CheckImputData_Str(txtMsg.Text, 0, 256) Then
			Using frmMsg As New tClassForm.frmPop_MsgBox("���b�Z�[�W �������ُ�(256�����ȉ�)�B", "�ُ�", Color.Red.ToArgb, True, False)
				frmMsg.ShowDialog()
			End Using
			Return False
		End If
		If 0 <> tClass.tClass_ImputCheck.CheckImputData_Str(txtTask.Text, 0, 16) Then
			Using frmMsg As New tClassForm.frmPop_MsgBox("������ �������ُ�(16�����ȉ�)�B", "�ُ�", Color.Red.ToArgb, True, False)
				frmMsg.ShowDialog()
			End Using
			Return False
		End If

		'''' �f�[�^�擾
		' �V�X���O
		newTyp.nSysNo = CType(cmbKind.SelectedItem, tClass_CmbItem).g_nKey
		' ����
		newTyp.dayAddTime = CDate(dtpStartDay.Value.ToString("yyyy/MM/dd") & dtpStartTime.Value.ToString(" HH:mm:ss"))
		newTyp.dayDspTime = newTyp.dayAddTime
		' PC��/PCKIND
		newTyp.sPcName = CType(cmbPcName.SelectedItem, tClass_CmbItem).g_sName
		newTyp.nPcKind = CType(cmbPcName.SelectedItem, tClass_CmbItem).g_nKey
		' ���̑�
		newTyp.sMsg = txtMsg.Text
		newTyp.sTaskName = txtTask.Text

		Return True
	End Function


	''' <summary>
	''' �폜����
	''' </summary>
	''' <remarks></remarks>
	Private Function Exec_Delete() As Boolean
		'' DB����R�����g�̍폜
		If Db_Delete(m_typSelectRowData.dayAddTime, m_typSelectRowData.nAutoNo) Then
			m_bDataUpDate = True
			Return True
		Else
			Using frmMsg As New tClassForm.frmPop_MsgBox("�R�����g�폜���s�B", "�ُ�", Color.Red.ToArgb, True, False)
				frmMsg.ShowDialog()
			End Using
			Return False
		End If
	End Function

	''' <summary>
	''' �R�����g�s�폜
	''' </summary>
	''' <param name="datAddTime">�o�^���t</param>
	''' <param name="nAutoNo">AutoNo</param>
	''' <returns></returns>
	''' <remarks></remarks>
	Private Function Db_Delete(ByVal datAddTime As Date, ByVal nAutoNo As Integer) As Boolean
		Dim strSql As String
		strSql = "DELETE FROM " & DB_LOG_INF
		strSql &= " WHERE �o�^���t='" & datAddTime.ToString("yyyy/MM/dd HH:mm:ss") & "'"
		strSql &= " AND AutoNo=" & nAutoNo

		'''' �o�^
		If 0 <= mcls_DB.DMLExecute(strSql) Then
			Return True
		Else
			Return False
		End If
	End Function

	''' <summary>
	''' �V�K�ǉ�
	''' </summary>
	''' <remarks></remarks>
	Private Function Exec_Insert() As Boolean
		Dim newTyp As typSelectRowData = Nothing

		'''' ���̓`�F�b�N
		If Not CheckInputData(newTyp) Then Return False

		'''' ��L�[�̎Z�o
		Dim nAutoNo As Integer = 0
		nAutoNo = GetNextAutoNo(newTyp.dayAddTime)

		'''' �o�^
		If Db_Insert(newTyp, nAutoNo) Then
			m_bDataUpDate = True
			Return True
		Else
			Using frmMsg As New tClassForm.frmPop_MsgBox("�R�����g�V�K�o�^���s�B", "�ُ�", Color.Red.ToArgb, True, False)
				frmMsg.ShowDialog()
			End Using
			Return False
		End If
	End Function

	''' <summary>
	''' �R�����g �V�K�ǉ�
	''' </summary>
	''' <param name="newTyp">�V�K�f�[�^</param>
	''' <param name="nAutoNo">�V�K��L�[</param>
	''' <returns></returns>
	''' <remarks></remarks>
	Private Function Db_Insert(ByVal newTyp As typSelectRowData, ByVal nAutoNo As Integer) As Boolean
		Dim strSql As String

		strSql = "INSERT INTO " & DB_LOG_INF
		strSql &= " VALUES("
		strSql &= "'" & newTyp.dayDspTime.ToString("yyyy/MM/dd HH:mm:ss") & "',"
		strSql &= "'" & newTyp.sTaskName & "',"

		strSql &= "'" & newTyp.sPcName & "',"
		strSql &= newTyp.nPcKind & ","
		strSql &= newTyp.nSysNo & ","
		strSql &= "'" & newTyp.sMsg & "',"
		strSql &= "'" & newTyp.dayAddTime.ToString("yyyy/MM/dd HH:mm:ss") & "',"
		strSql &= nAutoNo

		strSql &= ")"

		'''' �o�^
		If 0 <= mcls_DB.DMLExecute(strSql) Then
			Return True
		Else
			Return False
		End If
	End Function

	''' <summary>
	''' �ҏW
	''' </summary>
	''' <returns></returns>
	''' <remarks></remarks>
	Private Function Exec_Update() As Boolean
		Dim newTyp As typSelectRowData = Nothing

		'''' ���̓`�F�b�N
		If Not CheckInputData(newTyp) Then Return False

		'''' ������f�[�^���폜
		If Not Db_Delete(m_typSelectRowData.dayAddTime, m_typSelectRowData.nAutoNo) Then
			Using frmMsg As New tClassForm.frmPop_MsgBox("�R�����g�ҏW���s�B", "�ُ�", Color.Red.ToArgb, True, False)
				frmMsg.ShowDialog()
			End Using
			Return False
		End If

		'''' ��L�[�̎Z�o
		Dim nAutoNo As Integer = 0
		' ���Ԃ��ύX���ꂽ�ꍇ�́A�V�K��AutoNo���Z�o
		' �������ԂȂ�AutoNo�͎g���܂킷 (�\�����Ԃ��ύX�����ׂ̑΍�)
		If m_typSelectRowData.dayAddTime = newTyp.dayAddTime Then
			nAutoNo = m_typSelectRowData.nAutoNo
		Else
			nAutoNo = GetNextAutoNo(newTyp.dayAddTime)
		End If


		'''' �o�^
		If Db_Insert(newTyp, nAutoNo) Then
			m_bDataUpDate = True
			Return True
		Else
			Using frmMsg As New tClassForm.frmPop_MsgBox("�R�����g�ҏW���s�B", "�ُ�", Color.Red.ToArgb, True, False)
				frmMsg.ShowDialog()
			End Using
			Return False
		End If
	End Function


End Class
