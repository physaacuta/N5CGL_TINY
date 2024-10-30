'�����W�X�g���\���́A�ȉ��̒ʂ�Ƃ���B(�S�r������)
'�EHKEY_LOCAL_MACHINE / SOFTWARE / KizuSystem / BasePath = �S�V�X�e���̃C���X�g�[���t�H���_�t���p�X�p�X
'                                             / �V�X�e���� / EXE_1�`
'                                                            �� �l�́A�V���[�g�J�b�g�̃t���p�X
'���X�^�[�g���j���[�\���́A
'�EkizuSystem / �V�X�e���t�H���_ / �eEXE�̃V���[�g�J�b�g


Imports Microsoft.Win32
Imports tClassLibrary.tMod


Public Class frmMain

	'********************************************************************
	' ���[�J���ϐ�
	'********************************************************************
	'''' ���ʍ���
	Private Const mcBaseName As String = "KizuSystem"
	Private Const mcINIFILE As String = "KizuInstal.ini"
	Private mcDefaultPath As String = "C:\" & mcBaseName

	'' ���W�X�g���֌W
	Private Const mcRegSystemKey As String = "Software\" & mcBaseName
	Private Const mcRegBasePath As String = "BasePath"


	'' �ŏ����P��
	Private Class typInfo
		Public strName As String							' ���� (ini�t�@�C��Key��1��)
		Public strType As String							' ���� (ini�t�@�C��Key��2��)
		Public bInstal As Boolean							' �C���X�g�[�����

		''' <summary>
		''' �f�[�^�̃Z�b�g
		''' </summary>
		''' <param name="strVal">ini�t�@�C���L�[���</param>
		Public Sub New(ByVal strVal As String)
			Dim strAry() As String = strVal.Split(","c)
			If 2 = strAry.Length Then
				strName = strAry(0).Trim(" "c)
				strType = strAry(1).Trim(" "c)
			End If
		End Sub
	End Class

	'' 1�V�X�e���̏��
	Private Class typSystemInf
		Public typSys As typInfo							' �V�X�e�����
		Public lstExe As List(Of typInfo)					' EXE���
	End Class

	'' ���[�h�ݒ�
	Private Enum EM_INSTALMODE
		install = 0											' �C���X�g�[��
		update = 1											' �A�b�v�f�[�g
		delete = 2											' �A���C���X�g�[��
	End Enum


	'''''' ���[�J���ϐ�
	Private m_lstNowInf As New List(Of typSystemInf)		' ���݂̃C���X�g�[�����
	Private m_lstSetInf As New List(Of typSystemInf)		' ���݂̐ݒ���
	Private m_FastInstal As Boolean							' ���߂ẴC���X�g�[����true


	'********************************************************************
	' ��ʏ���
	'********************************************************************
	''' <summary>
	''' �t�H�[�����[�h
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmMain_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
		'' ini�t�@�C�����擾
		ReadIni(m_lstNowInf)
		ReadIni(m_lstSetInf)

		'' ��ʗp�f�[�^�ēǍ�
		RefreshDisp(EM_INSTALMODE.install)
	End Sub

	''' <summary>
	''' ���W�X�g���L�[��Ԃ����� �C���X�g�[���x�[�X�t�H���_��ԕύX���s��
	''' </summary>
	''' <param name="regkey"></param>
	''' <remarks></remarks>
	Private Sub EnableSelectFolder(ByVal regkey As RegistryKey)
		Try
			If regkey Is Nothing Then			'' �܂��C���X�g�[�����ꂽ��������
				'�x�[�X�t�H���_�����l�Z�b�g
				txtNowInstalFolder.Text = ""
				txtSetInstalFolder.Text = mcDefaultPath

				m_FastInstal = True
				SetMsgBox(True, "�V�K�C���X�g�[���ł��B" & vbCrLf & "D�h���C�u������ꍇ�́A�o���邾��D:\KizuSystem�ɃC���X�g�[���ɂ��ĉ������B")

			Else								'' �C���X�g�[���ς�
				'�x�[�X�t�H���_�͕ύX�֎~
				txtNowInstalFolder.Text = regkey.GetValue(mcRegBasePath).ToString
				txtSetInstalFolder.Text = regkey.GetValue(mcRegBasePath).ToString

				m_FastInstal = False
				SetMsgBox(True, "[" & txtSetInstalFolder.Text & "] ���r�����V�X�e���̃C���X�g�[���t�H���_�𔭌����܂����B" & vbCrLf & "�ȍ~�A�t�H���_��ύX���鎖�́A�o���܂���B")
			End If
		Catch ex As Exception

		End Try

	End Sub


	''' <summary>
	''' ini�t�@�C�����擾���āA��{�ƂȂ�V�X�e�����𐶐�
	''' </summary>
	''' <param name="lst">�Z�b�g����\����</param>
	''' <remarks></remarks>
	Private Sub ReadIni(ByRef lst As List(Of typSystemInf))
		Dim strVal As String
		Dim strSection As String

		Dim sysid As Integer = 1
		Dim exeid As Integer

		'' ini�t�@�C���t���p�X�擾
		Dim strIniPath As String = IO.Path.Combine(System.Windows.Forms.Application.StartupPath, mcINIFILE)


		'' �V�X�e�����擾
		sysid = 1
		While (True)
			' �V�X�e�����L��H
			strSection = "ID_" & sysid
			strVal = tMod_IniFile.ReadIniStr(strSection, "Name", strIniPath)
			If 0 = strVal.Length Then Exit While '���������H

			Dim sysinf As typSystemInf = New typSystemInf
			sysinf.lstExe = New List(Of typInfo)

			'�V�X�e����񐶐�
			sysinf.typSys = New typInfo(strVal)
			lst.Add(sysinf)

			'' EXE���
			exeid = 1
			While True
				' EXE���L��H
				strVal = tMod_IniFile.ReadIniStr(strSection, "EXE_" & exeid, strIniPath)
				If 0 = strVal.Length Then Exit While '���������H

				'EXE���
				Dim exeinf As New typInfo(strVal)
				sysinf.lstExe.Add(exeinf)

				exeid += 1
			End While

			sysid += 1
		End While
	End Sub

	''' <summary>
	''' ���W�X�g���̓o�^��ԂŁA�C���X�g�[����Ԃ�c������
	''' </summary>
	''' <param name="regkey">���W�X�g���L�[</param>
	''' <param name="lst"></param>
	''' <remarks></remarks>
	Private Sub ReadInstalReg(ByVal regkey As RegistryKey, ByRef lst As List(Of typSystemInf))
		Try
			Dim syskey As RegistryKey		'�e�V�X�e���̃��W�X�g�����

			'''' ������
			For ii As Integer = 0 To lst.Count - 1
				lst(ii).typSys.bInstal = False
				For jj As Integer = 0 To lst(ii).lstExe.Count - 1
					lst(ii).lstExe(jj).bInstal = False
				Next jj
			Next ii

			'''' ���W�X�g���f�[�^����
			If regkey Is Nothing Then Return


			'''' ���W�X�g���f�[�^����
			For ii As Integer = 0 To lst.Count - 1
				'''' �V�X�e���̃L�[������H
				syskey = regkey.OpenSubKey(lst(ii).typSys.strType, False)
				If syskey Is Nothing Then
					lst(ii).typSys.bInstal = False
					Continue For
				Else
					lst(ii).typSys.bInstal = True
				End If


				'''' �o�^EXE������H
				For jj As Integer = 0 To lst(ii).lstExe.Count - 1
					Dim o As Object = syskey.GetValue("EXE_" & jj.ToString)
					If o Is Nothing Then
						lst(ii).lstExe(jj).bInstal = False
					Else
						lst(ii).lstExe(jj).bInstal = True
					End If
				Next jj
			Next ii
		Catch ex As Exception

		End Try

	End Sub



	''' <summary>
	''' �c���[�r���[�ɃA�C�e���ǉ�
	''' </summary>
	''' <param name="trv">�ǉ�����R���g���[��</param>
	''' <param name="lst">�ǉ�����A�C�e��</param>
	''' <param name="nAddMode">�ǉ����� (0:�S�� 1:ON�̓z�̂� 2:OFF�̓z�̂�)</param>
	''' <param name="bSysOnly">�ǉ����� (true:�q�m�[�h����)</param>	
	''' <remarks></remarks>
	Private Sub SetTreeViewItem(ByVal trv As TreeView, ByVal lst As List(Of typSystemInf), ByVal nAddMode As Integer, ByVal bSysOnly As Boolean)

		Dim nodeBase As TreeNode
		Dim nodeKo As TreeNode
		With trv
			.Nodes.Clear()

			For ii As Integer = 0 To lst.Count - 1
				If 1 = nAddMode AndAlso Not lst(ii).typSys.bInstal Then Continue For
				'If 2 = nAddMode AndAlso lst(ii).typSys.bInstal Then Continue For		' �q�m�[�h�̒ǉ����ɃG���[�ɂȂ��

				'�V�X�e����� (�e�m�[�h)
				nodeBase = New TreeNode()
				nodeBase.Text = lst(ii).typSys.strName & "    ��� [" & lst(ii).typSys.strType & "]"
				nodeBase.Tag = lst(ii).typSys
				.Nodes.Add(nodeBase)

				If Not bSysOnly Then
					For jj As Integer = 0 To lst(ii).lstExe.Count - 1
						If 1 = nAddMode AndAlso Not lst(ii).lstExe(jj).bInstal Then Continue For
						If 2 = nAddMode AndAlso lst(ii).lstExe(jj).bInstal Then Continue For

						'EXE��� (�q�m�[�h)
						nodeKo = New TreeNode()
						nodeKo.Text = lst(ii).lstExe(jj).strName
						nodeKo.Tag = lst(ii).lstExe(jj)
						nodeBase.Nodes.Add(nodeKo)
					Next jj
				End If

				' �q�m�[�h��W�J
				.Nodes(.Nodes.Count - 1).Expand()
			Next ii
		End With
	End Sub


	''' <summary>
	''' ��ʗp�f�[�^�ēǍ�
	''' </summary>
	''' <remarks></remarks>
	Private Sub RefreshDisp(ByVal emInstall As EM_INSTALMODE)
		Try
			Dim regkey As RegistryKey = My.Computer.Registry.LocalMachine.OpenSubKey(mcRegSystemKey)

			'' �C���X�g�[����Ԃɂ�菉���l�Z�b�g
			EnableSelectFolder(regkey)

			'''' ���݂̃V�X�e����ԍēǍ�
			ReadInstalReg(regkey, m_lstNowInf)
			ReadInstalReg(regkey, m_lstSetInf)
			SetTreeViewItem(trvNowInf, m_lstNowInf, 1, False)	   ' �C���X�g�[���ς݂̍��ڂ̂�
			SetMode(emInstall)

			If Not regkey Is Nothing Then
				regkey.Close()
			End If
		Catch ex As Exception

		End Try
	End Sub


	''' <summary>
	''' �m�[�h�N���b�N
	''' </summary>
	Private Sub trvSetInf_NodeMouseClick(ByVal sender As Object, ByVal e As System.Windows.Forms.TreeNodeMouseClickEventArgs) Handles trvSetInf.NodeMouseClick
		'' �����̎q�m�[�h�̃`�F�b�N��Ԃ�ύX����
		For ii As Integer = 0 To e.Node.Nodes.Count - 1
			e.Node.Nodes(ii).Checked = e.Node.Checked
		Next ii

		'' �����̐e�m�[�h���`�F�b�N��ԂɕύX����
		If e.Node.Checked Then
			If Not e.Node.Parent Is Nothing Then
				e.Node.Parent.Checked = True
			End If
		End If
	End Sub


	''' <summary>
	''' �x�[�X�t�H���_�I��
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnSelectFolder_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSelectFolder.Click, Button1.Click
		Dim dlg As New FolderBrowserDialog
		dlg.ShowNewFolderButton = True
		' �f�t�H���g�I���p�X
		Dim path As String = txtSetInstalFolder.Text
		If IO.Directory.Exists(path) Then	' �t�H���_�L��
			dlg.SelectedPath = path
		Else									' �t�H���_����
			dlg.SelectedPath = IO.Path.GetDirectoryName(path)
		End If

		' �t�H���_�I��
		dlg.ShowDialog()

		' �I���t�H���_�擾
		txtSetInstalFolder.Text = dlg.SelectedPath
	End Sub


	'// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
	'// �C���X�g�[���֘A

	''' <summary>
	''' �R�}���h���s
	''' </summary>
	Private Sub btnExecute_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnExecute.Click
		'''' ���O�`�F�b�N
		Dim strMsg As String
		If rdoInstal1.Checked Then
			strMsg = rdoInstal1.Text
		ElseIf rdoInstal2.Checked Then
			strMsg = rdoInstal2.Text
		Else
			strMsg = rdoInstal3.Text
		End If
		Dim retc As MsgBoxResult = MsgBox("�`�F�b�N�{�b�N�X�őI������Ă��鍀�ڂ� [" & strMsg & "] �����s���܂����H", MsgBoxStyle.OkCancel Or MsgBoxStyle.DefaultButton2 Or MsgBoxStyle.Question, "�m�F")
		If retc <> MsgBoxResult.Ok Then Return


		'''' ���s
		If rdoInstal1.Checked Then
			Call Execute_Install()
		ElseIf rdoInstal2.Checked Then
			Call Execute_UpDate()
		Else
			Call Execute_UnInstall()
		End If
	End Sub

	''' <summary>
	''' �C���X�g�[���J�n
	''' </summary>
	Private Sub Execute_Install()
		'''''' �O����
		Dim strSysPathFrom As String		' �V�X�e���P�ʂ̃R�s�[���̃p�X
		Dim strSysPathTo As String			' �V�X�e���P�ʂ̃R�s�[��̃p�X
		Dim strLinkPath As String			' �V���[�g�J�b�g�̃p�X
		Dim strStartMenuPath As String		' �X�^�[�g���j���[�p�X
		Dim typSys As typInfo = Nothing
		Dim typExe As typInfo = Nothing

		' �X�^�[�g���j���[�t�H���_����
		strStartMenuPath = IO.Path.Combine(My.Computer.FileSystem.SpecialDirectories.Programs, mcBaseName)	' �X�^�[�g���j���[�̃t�H���_�p�X
		IO.Directory.CreateDirectory(strStartMenuPath)


		' ���W�X�g���ɏ������� & �I�[�v�� (�x�[�X�t�H���_�p�̃L�[&�l)
		Dim regkey As RegistryKey = My.Computer.Registry.LocalMachine.CreateSubKey(mcRegSystemKey, RegistryKeyPermissionCheck.ReadWriteSubTree)
		regkey.SetValue(mcRegBasePath, txtSetInstalFolder.Text)


		' �C���X�g�[��
		For ii As Integer = 0 To trvSetInf.Nodes.Count - 1
			typSys = CType(trvSetInf.Nodes(ii).Tag, typInfo)

			'''' �V�X�e���̐V�K�C���X�g�[�� (�܂��C���X�g�[�����ĂȂ� ���� ����C���X�g�[��)
			' �e�m�[�h�̃`�F�b�N�t�����t�H���_����
			If trvSetInf.Nodes(ii).Checked And Not typSys.bInstal Then

				' �x�[�X�t�H���_�̉��� �V�X�e���P�ʂ̃t�H���_���R�s�[ (�㏑���֎~)
				strSysPathFrom = IO.Path.Combine(System.Windows.Forms.Application.StartupPath, typSys.strType)	   ' �����̃t�H���_�p�X
				strSysPathTo = IO.Path.Combine(txtSetInstalFolder.Text, typSys.strType)
				tMod_Common.CopyDirectory(strSysPathFrom, strSysPathTo, False, True)

				' �X�^�[�g���j���[�t�H���_ �̉��� �V�X�e���P�ʂ̃t�H���_�쐬
				strLinkPath = IO.Path.Combine(strStartMenuPath, typSys.strName)
				Try
					IO.Directory.CreateDirectory(strLinkPath)
				Catch ex As Exception
				End Try

				' ���W�X�g���ɏ������� (�V�X�e���p�̃L�[)
				regkey = My.Computer.Registry.LocalMachine.CreateSubKey(mcRegSystemKey & "\" & typSys.strType, RegistryKeyPermissionCheck.ReadWriteSubTree)


				' �q�m�[�h�̃`�F�b�N�t���̓z�̂݃V���[�g�J�b�g�쐬
				For jj As Integer = 0 To trvSetInf.Nodes(ii).Nodes.Count - 1
					If trvSetInf.Nodes(ii).Nodes(jj).Checked Then
						typExe = CType(trvSetInf.Nodes(ii).Nodes(jj).Tag, typInfo)
						Dim strWk As String = strLinkPath & "\" & typExe.strName & ".lnk"			' �V���[�g�J�b�g�t�@�C����

						' �V���[�g�J�b�g�쐬
						tMod_CommonStrictOff.CreateShortCut(strWk, strSysPathTo & "\EXE", typExe.strType & ".EXE")
						' ���W�X�g���ɏ�������
						regkey.SetValue("EXE_" & jj.ToString(), strWk)
					End If
				Next jj
			End If
		Next ii


		'''' ���W�X�g���N���[�Y
		If Not regkey Is Nothing Then
			regkey.Close()
		End If

		'' ��ʗp�f�[�^�ēǍ�
		RefreshDisp(EM_INSTALMODE.install)
	End Sub


	''' <summary>
	''' �A�b�v�f�[�g�J�n
	''' </summary>
	''' <remarks></remarks>
	Private Sub Execute_UpDate()
		Dim strSysPathFrom As String		' �V�X�e���P�ʂ̃R�s�[���̃p�X
		Dim strSysPathTo As String			' �V�X�e���P�ʂ̃R�s�[��̃p�X
		Dim typSys As typInfo = Nothing

		' �A�b�v�f�[�g
		For ii As Integer = 0 To trvSetInf.Nodes.Count - 1
			typSys = CType(trvSetInf.Nodes(ii).Tag, typInfo)

			If trvSetInf.Nodes(ii).Checked Then

				' �x�[�X�t�H���_�̉��� �V�X�e���P�ʂ̃t�H���_���R�s�[ (�㏑������)
				strSysPathFrom = IO.Path.Combine(System.Windows.Forms.Application.StartupPath, typSys.strType)		' �����̃t�H���_�p�X
				strSysPathTo = IO.Path.Combine(txtSetInstalFolder.Text, typSys.strType)
				tMod_Common.CopyDirectory(strSysPathFrom, strSysPathTo, True, True)
			End If
		Next ii

		'' ��ʗp�f�[�^�ēǍ�
		RefreshDisp(EM_INSTALMODE.update)
	End Sub


	''' <summary>
	''' �A���C���X�g�[���J�n
	''' </summary>
	''' <remarks></remarks>
	Private Sub Execute_UnInstall()
		Dim strStartMenuPath As String		' �X�^�[�g���j���[�p�X
		Dim typSys As typInfo = Nothing
		Dim typExe As typInfo = Nothing
		Dim strWk As String

		' �X�^�[�g���j���[�t�H���_
		strStartMenuPath = IO.Path.Combine(My.Computer.FileSystem.SpecialDirectories.Programs, mcBaseName)	' �X�^�[�g���j���[�̃t�H���_�p�X


		' �A���C���X�g�[��
		For ii As Integer = 0 To trvSetInf.Nodes.Count - 1
			typSys = CType(trvSetInf.Nodes(ii).Tag, typInfo)

			If trvSetInf.Nodes(ii).Checked Then
				' �x�[�X�t�H���_�̉��� �V�X�e���P�ʂ̃t�H���_���폜
				strWk = IO.Path.Combine(txtSetInstalFolder.Text, typSys.strType)
				Try
					IO.Directory.Delete(strWk, True)
				Catch ex As Exception
				End Try

				' �X�^�[�g���j���[�t�H���_ �̉��� �V�X�e���P�ʂ̃t�H���_���폜
				strWk = IO.Path.Combine(strStartMenuPath, typSys.strName)
				Try
					IO.Directory.Delete(strWk, True)
				Catch ex As Exception
				End Try

				' ���W�X�g���폜
				Try
					My.Computer.Registry.LocalMachine.DeleteSubKeyTree(mcRegSystemKey & "\" & typSys.strType)
				Catch ex As Exception
				End Try
			End If
		Next ii

		'' ��ʗp�f�[�^�ēǍ�
		RefreshDisp(EM_INSTALMODE.delete)
	End Sub


	''' <summary>
	''' �S�V�X�e�� �A���C���X�g�[���J�n
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnAllUninstall_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnAllUninstall.Click

		'''' ���O�`�F�b�N
		Dim retc As MsgBoxResult = MsgBox("�S�V�X�e�����A���C���X�g�[�����܂����H", MsgBoxStyle.OkCancel Or MsgBoxStyle.DefaultButton2 Or MsgBoxStyle.Question, "�m�F")
		If retc <> MsgBoxResult.Ok Then Return


		'' �x�[�X�t�H���_�폜
		Try
			IO.Directory.Delete(txtSetInstalFolder.Text, True)
		Catch ex As Exception
		End Try

		'' �X�^�[�g���j���[�t�H���_�폜
		Try
			IO.Directory.Delete(IO.Path.Combine(My.Computer.FileSystem.SpecialDirectories.Programs, mcBaseName), True)
		Catch ex As Exception
		End Try

		'' ���W�X�g������
		Try
			My.Computer.Registry.LocalMachine.DeleteSubKeyTree(mcRegSystemKey)
		Catch ex As Exception
		End Try


		'' ��ʗp�f�[�^�ēǍ�
		RefreshDisp(EM_INSTALMODE.install)
	End Sub



	'// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
	'// ���̑����ʏ���

	''' <summary>
	''' ���b�Z�[�W�{�b�N�X�Ƀ��b�Z�[�W��������
	''' </summary>
	''' <param name="bln"></param>
	''' <param name="strMsg"></param>
	''' <remarks></remarks>
	Private Sub SetMsgBox(ByVal bln As Boolean, ByVal strMsg As String)
		lblMsgBox.Text = strMsg
		If bln Then
			lblMsgBox.ForeColor = Color.Black
		Else
			lblMsgBox.ForeColor = Color.Red
		End If

		System.Windows.Forms.Application.DoEvents()
	End Sub


	''' <summary>
	''' �C���X�g�[����ԕύX
	''' </summary>
	''' <param name="emInstall">�C���X�g�[���ݒ莞0   �A���C���X�g�[����false</param>
	''' <remarks></remarks>
	Private Sub SetMode(ByVal emInstall As EM_INSTALMODE)
		Dim bBaseFolderOn As Boolean = True

		'''' ��ʏ�ԕύX
		rdoInstal1.BackColor = Color.PeachPuff
		rdoInstal2.BackColor = Color.PeachPuff
		rdoInstal3.BackColor = Color.PeachPuff
		If EM_INSTALMODE.install = emInstall Then
			rdoInstal1.Checked = True
			rdoInstal1.BackColor = Color.Cyan
			grpSet.Text = "�C���X�g�[���ݒ�"
			btnExecute.Text = "�C���X�g�[���J�n"
			bBaseFolderOn = True

		ElseIf EM_INSTALMODE.update = emInstall Then
			rdoInstal2.Checked = True
			rdoInstal2.BackColor = Color.Cyan
			grpSet.Text = "�A�b�v�f�[�g�ݒ�"
			btnExecute.Text = "�A�b�v�f�[�g�J�n"
			bBaseFolderOn = False

		Else
			rdoInstal3.Checked = True
			rdoInstal3.BackColor = Color.Cyan
			grpSet.Text = "�A���C���X�g�[���ݒ�"
			btnExecute.Text = "�A���C���X�g�[���J�n"
			bBaseFolderOn = False
		End If

		'''' �x�[�X�t�H���_�ύX�\�H
		If bBaseFolderOn And m_FastInstal Then
			txtSetInstalFolder.ReadOnly = False
			btnSelectFolder.Enabled = True
		Else
			txtSetInstalFolder.ReadOnly = True
			btnSelectFolder.Enabled = False
		End If

		'''' �ݒ��Ԃ̃��X�g�{�b�N�X�ύX
		If EM_INSTALMODE.install = emInstall Then
			SetTreeViewItem(trvSetInf, m_lstSetInf, 2, False)		' �܂��C���X�g�[������ĂȂ��z
		ElseIf EM_INSTALMODE.update = emInstall Then
			SetTreeViewItem(trvSetInf, m_lstSetInf, 1, True)		' �C���X�g�[���ς݂̓z
		Else
			SetTreeViewItem(trvSetInf, m_lstSetInf, 1, True)		' �C���X�g�[���ς݂̓z
		End If
	End Sub


	''' <summary>
	''' �C���X�g�[���E�A���C���X�g�[���ݒ�
	''' </summary>
	Private Sub rdoInstal_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles rdoInstal1.CheckedChanged, rdoInstal2.CheckedChanged, rdoInstal3.CheckedChanged
		If rdoInstal1.Checked Then
			SetMode(EM_INSTALMODE.install)
		ElseIf rdoInstal2.Checked Then
			SetMode(EM_INSTALMODE.update)
		Else
			SetMode(EM_INSTALMODE.delete)
		End If
	End Sub



End Class
