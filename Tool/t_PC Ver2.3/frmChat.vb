'' �񓯊��̓s�x�ڑ����s���B��M�����T�[�o�[�B���M�����N���C�A���g�̂Q��������Ƃ���

'Imports tClassLibrary.tClass

Public Class frmChat

	'//***************************************************************
	'// �萔 & �\����
	'//***************************************************************

	Private Const mcSOCK_SERVER_PORT As Integer = 12346					' �T�[�o�[�|�[�g
	Private Const mcSOCK_DATA_MAXSIZE As Integer = 1024					' �`���ő�T�C�Y
	Private mcls_SockRecv As tClass.tClass_Socket.tClass_SocketMgr


	''' <summary>
	''' ����M�p���[�h (1byte�Ŏg�p)
	''' </summary>
	''' <remarks></remarks>
	Private Enum EM_SEND_MODE
		EM_SYSLOG = 1				' �V�X�e�����ԐM�������b�Z�[�W (2byte�ڈȍ~�A�V�X���O)
		EM_MSG						' �`���b�g���b�Z�[�W (2byte�ڈȍ~�A�ʏ�̃`���b�g���b�Z�[�W)

		EM_FILE_EXEC				' �w��t�@�C�������s (2byte�ڈȍ~�A�Ώ�PC�ł̎��s�t�@�C���t���p�X)
		EM_COMMAND_EXEC				' �w��R�}���h�����s (2byte�ڈȍ~�A�Ώ�PC�ł̎��s����R�}���h)
	End Enum



	'//***************************************************************
	'// �C���X�^���X
	'//***************************************************************
	Private m_frmPopup As New frmPopup									' �|�b�v�A�b�v���


	'//***************************************************************
	'// �ݒ�萔 ��`
	'//***************************************************************

	''' <summary>
	''' �\�P�b�g����
	''' </summary>
	''' <remarks></remarks>
	Public Sub Alloc()
		'' ��M�p �T�[�o�[��`
		mcls_SockRecv = New tClass.tClass_Socket.tClass_SocketMgr(AddressOf subDele_SOCK_Recv_Event)

		If mcls_SockRecv.SockOpen(Net.Sockets.SocketType.Stream, Net.Sockets.ProtocolType.Tcp) Then
			mcls_SockRecv.Listen(mcSOCK_SERVER_PORT)
			'MsgBox("��,[clsDensou::Open],��M�|�[�g �A�N�Z�v�g�J�n")
		Else													'�ُ�
			MsgBox("��,[clsDensou::Open],��M�|�[�g �I�[�v�����s")
			Exit Sub
		End If

	End Sub


	''' <summary>
	''' �\�P�b�g���
	''' </summary>
	''' <remarks></remarks>
	Public Sub Free()
		'' ���
		If Not mcls_SockRecv Is Nothing Then
			mcls_SockRecv.SockClose()
			mcls_SockRecv = Nothing
		End If

		m_frmPopup.Close()
		m_frmPopup.Dispose()
		m_frmPopup = Nothing
	End Sub



	Public Sub TxtScroll()
		'�J���b�g�ʒu���Ō�Ɉړ�
		Me.txtRecv.SelectionStart = Me.txtRecv.Text.Length
		'�e�L�X�g�{�b�N�X�Ƀt�H�[�J�X���ړ�
		Me.txtRecv.Focus()
		'�J���b�g�ʒu�܂ŃX�N���[�� (�Ō�̈ʒu�ƂȂ�B����̓e�L�X�g�{�b�N�X�Ƀt�H�[�J�X���������ĂȂ��ƃ_��)
		Me.txtRecv.ScrollToCaret()
	End Sub


	''' <summary>
	''' �t�H�[�������߂ĕ\������鎞�A
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmChat_Shown(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Shown
		'' �~�{�^���𖳌��ɂ���
		tMod.subFormEndButton_Desebled(Me.Handle)

		'' ini�t�@�C������ݒ���擾
		Dim nCnt As Integer = 1
		Dim strWk As String = ""
		While (True)
			strWk = tMod.ReadIniStr("ADD", "P" & nCnt.ToString(), m_ini)
			strWk = strWk.Trim
			If strWk.Length = 0 Then
				Exit While
			End If

			cmbSendIp.Items.Add(strWk)
			nCnt += 1
		End While

	End Sub


	''' <summary>
	''' �t�H�[����\��
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnClose_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnClose.Click
		Me.Hide()
	End Sub


	''' <summary>
	''' �t�H�[���N���[�W���O
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmChat_FormClosing(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles Me.FormClosing
		'' ���
		If Not mcls_SockRecv Is Nothing Then
			mcls_SockRecv.SockClose()
			mcls_SockRecv = Nothing
		End If
	End Sub


	''' <summary>
	''' �A�C�R�����m�[�}���ɖ߂��^�C�}
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub timIc_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timIc.Tick
		timIc.Stop()
		IconChange(True)
	End Sub

	''' <summary>
	''' ����M�����폜
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnClear_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnClear.Click
		txtRecv.Text = ""
	End Sub



	''' <summary>
	''' ����ʂ��\�����ꂽ
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmChat_VisibleChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.VisibleChanged
		If Me.Visible Then
			'�|�b�v�A�b�v��ʂ��\�����Ȃ����
			m_frmPopup.Hide()
		End If
	End Sub

	''' <summary>
	''' �G�N�X�v���[���[�\��
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnExplorer_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnExplorer.Click
		Dim strCommand As String
		Dim wk() As String = cmbSendIp.Text.Split("@"c)

		'�G�N�X�v���[���N��()
		strCommand = "/e,"		'�t�H���_�c���[���\��
		'strCommand = "/n,"		'�t�H���_�c���[��\��
		strCommand &= "\\" & wk(0) & "\c$"

		Process.Start("explorer.exe", strCommand)
	End Sub

	''' <summary>
	''' �����[�g�f�X�N�g�b�v (�R���\�[��)
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnRemoto_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRemoto.Click
		Dim strCommand As String
		Dim wk() As String = cmbSendIp.Text.Split("@"c)

		strCommand = "/console"	 'sp2
		'strCommand = "/admin"	 'sp3
		strCommand &= " /v:" & wk(0)
		Process.Start("mstsc.exe", strCommand)
	End Sub
	''' <summary>
	''' �����[�g�f�X�N�g�b�v (�V�R���\�[��)
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnRemoto2_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRemoto2.Click
		Dim strCommand As String
		Dim wk() As String = cmbSendIp.Text.Split("@"c)

		'strCommand = "/console"	 'sp2
		strCommand = "/admin"	 'sp3
		strCommand &= " /v:" & wk(0)
		Process.Start("mstsc.exe", strCommand)
	End Sub
	''' <summary>
	''' �����[�g�f�X�N�g�b�v (�m���R���\�[��)
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnRemotoNon_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRemotoNon.Click
		Dim strCommand As String
		Dim wk() As String = cmbSendIp.Text.Split("@"c)

		strCommand = " /v:" & wk(0)
		Process.Start("mstsc.exe", strCommand)
	End Sub

	'//***************************************************************
	'// ���M
	'//***************************************************************
	''' <summary>
	''' ���M�f�[�^����
	''' </summary>
	Function CreateSendByte(ByVal cmd As Integer, ByVal msg As String) As Byte()
		Dim buf() As Byte = System.Text.Encoding.GetEncoding("shift-jis").GetBytes(msg)

		Dim ansbuf(buf.Length) As Byte
		ansbuf(0) = CByte(cmd)

		Array.Copy(buf, 0, ansbuf, 1, buf.Length)
		Return ansbuf
	End Function

	''' <summary>
	''' ���M�J�n
	''' </summary>
	Private Function SendStart(ByVal sendbuf() As Byte, ByVal sendsize As Integer, ByVal strIp As String) As Boolean
		'' ���M�� ����
		Dim remoteEP As Net.IPEndPoint = Nothing

		'' ���M��IP �`�F�b�N
		If strIp.Length = 0 Then
			MsgBox("���M��IP����")
			Return False
		End If
		If strIp.Length < 1 Then
			MsgBox("���M��IP����")
			Return False
		End If

		Try
			remoteEP = New Net.IPEndPoint(Net.IPAddress.Parse(strIp), mcSOCK_SERVER_PORT)
		Catch ex As Exception
			MsgBox("���M��IP����")
			Return False
		End Try


		'' ���M
		Try
			Dim sock As New Net.Sockets.Socket(Net.Sockets.AddressFamily.InterNetwork, Net.Sockets.SocketType.Stream, Net.Sockets.ProtocolType.Tcp)
			sock.Connect(remoteEP)
			Dim bytesSent As Integer = sock.Send(sendbuf, sendsize, Net.Sockets.SocketFlags.None)
			sock.Shutdown(Net.Sockets.SocketShutdown.Both)
			sock.Close()
			If bytesSent <> 0 Then
				'��M���X�g�֕\��
				SetRecvText(String.Format("=====���M===== [{0}]", strIp))

				'txtSendMsg.Text = ""
				Return True
			End If
		Catch ex As Exception
			MsgBox("���M���s")
			Return False
		End Try
	End Function




	''' <summary>
	''' ���b�Z�[�W���M
	''' </summary>
	Private Sub btnSend_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSend.Click
		If txtSendMsg.Text.Length = 0 Then Return


		'' ���M�f�[�^
		Dim sendsize As Integer
		Dim sendbuf() As Byte  '���M�o�b�t�@
		sendbuf = CreateSendByte(EM_SEND_MODE.EM_MSG, txtSendMsg.Text)
		sendsize = CInt(IIf(mcSOCK_DATA_MAXSIZE < sendbuf.Length, mcSOCK_DATA_MAXSIZE, sendbuf.Length))

		'' ���M�J�n
		Dim wk() As String = cmbSendIp.Text.Split("@"c)
		If Not SendStart(sendbuf, sendsize, wk(0)) Then Return

		SetRecvText(txtSendMsg.Text & vbCrLf)
		'txtSendMsg.Text = ""
	End Sub

	''' <summary>
	''' �t�@�C�����s
	''' </summary>
	Private Sub btnExecFile_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnExecFile.Click
		If txtExecItem.Text.Length = 0 Then Return


		'' ���M�f�[�^
		Dim sendsize As Integer
		Dim sendbuf() As Byte  '���M�o�b�t�@
		sendbuf = CreateSendByte(EM_SEND_MODE.EM_FILE_EXEC, txtExecItem.Text)
		sendsize = CInt(IIf(mcSOCK_DATA_MAXSIZE < sendbuf.Length, mcSOCK_DATA_MAXSIZE, sendbuf.Length))

		'' ���M�J�n
		Dim wk() As String = cmbSendIp.Text.Split("@"c)
		If Not SendStart(sendbuf, sendsize, wk(0)) Then Return

		SetRecvText(txtExecItem.Text & vbCrLf)
		'txtExecItem.Text = ""
	End Sub
	''' <summary>
	''' �R�}���h���s
	''' </summary>
	Private Sub btnExecCommand_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnExecCommand.Click
		If txtExecItem.Text.Length = 0 Then Return


		'' ���M�f�[�^
		Dim sendsize As Integer
		Dim sendbuf() As Byte  '���M�o�b�t�@
		sendbuf = CreateSendByte(EM_SEND_MODE.EM_COMMAND_EXEC, txtExecItem.Text)
		sendsize = CInt(IIf(mcSOCK_DATA_MAXSIZE < sendbuf.Length, mcSOCK_DATA_MAXSIZE, sendbuf.Length))

		'' ���M�J�n
		Dim wk() As String = cmbSendIp.Text.Split("@"c)
		If Not SendStart(sendbuf, sendsize, wk(0)) Then Return

		SetRecvText(txtExecItem.Text & vbCrLf)
		'txtExecItem.Text = ""
	End Sub


	'//***************************************************************
	'// �R���g���[���񓯊��A�N�Z�X
	'//***************************************************************

	''' <summary>
	''' �e�L�X�g�{�b�N�X �A�N�Z�X
	''' </summary>
	''' <param name="strMsg"></param>
	''' <remarks></remarks>
	Delegate Sub SetTextCallback(ByVal strMsg As String)

	Private Sub SetRecvText(ByVal strMsg As String)
		If Me.InvokeRequired Then
			Dim d As New SetTextCallback(AddressOf SetRecvText)
			Me.Invoke(d, New Object() {strMsg})
		Else
			'�e�L�X�g�{�b�N�X�� ��M�f�[�^���Z�b�g
			Me.txtRecv.Text &= strMsg & vbCrLf

			'�J���b�g�ʒu�܂ŃX�N���[�� (�Ō�̈ʒu�ƂȂ�B����̓e�L�X�g�{�b�N�X�Ƀt�H�[�J�X���������ĂȂ��ƃ_��)
			TxtScroll()

			' ''�J���b�g�ʒu���Ō�Ɉړ�
			''Me.txtRecv.SelectionStart = Me.txtRecv.Text.Length
			' ''�e�L�X�g�{�b�N�X�Ƀt�H�[�J�X���ړ�
			''Me.txtRecv.Focus()
			' ''�J���b�g�ʒu�܂ŃX�N���[�� (�Ō�̈ʒu�ƂȂ�B����̓e�L�X�g�{�b�N�X�Ƀt�H�[�J�X���������ĂȂ��ƃ_��)
			''Me.txtRecv.ScrollToCaret()
		End If
	End Sub

	''' <summary>
	''' �^�C�}�[ �A�N�Z�X
	''' </summary>
	''' <remarks></remarks>
	Delegate Sub SetHideModeCallback(ByVal strMsg As String)

	Private Sub SetHideMode(ByVal strMsg As String)

		If Me.InvokeRequired Then
			Dim d As New SetHideModeCallback(AddressOf SetHideMode)
			Me.Invoke(d, New Object() {strMsg})
		Else
			' �A���R���ύX�^�C�}�[
			Me.timIc.Start()

			' �|�b�v�A�b�v�t�H�[���̕\��
			m_frmPopup.g_strMsg = strMsg
			m_frmPopup.Show()
		End If
	End Sub

	''' <summary>
	''' ���M�� �A�N�Z�X
	''' </summary>
	''' <param name="strIp"></param>
	''' <remarks></remarks>
	Delegate Sub SetSendIpCallback(ByVal strIp As String)

	Private Sub SetSendIp(ByVal strIp As String)
		If Me.InvokeRequired Then
			Dim d As New SetTextCallback(AddressOf SetSendIp)
			Me.Invoke(d, New Object() {strIp})
		Else
			Me.cmbSendIp.Text = strIp
		End If
	End Sub

	'----------------------------------------------------------------
	' �T�v�FTCP/IP �R�[���o�b�N�֐�
	' �����F(in :Object) sender = tClass.tClass_Socket_State_*** �N���X
	' �����F(in :tEnum_Sock_EventNO) e = �C�x���gNO
	' �ߒl�F�Ȃ�
	' ���l�F��M�|�[�g�p �\�P�b�g
	'----------------------------------------------------------------
	Private Sub subDele_SOCK_Recv_Event(ByVal sender As Object, ByVal e As tClass.tClass_Socket.tClass_Socket_Base.tEnum_Sock_EventNO)
		Dim iRetc As Integer = 0
		'' �C�x���gNO�ŏ�������
		Select Case e
			Case tClass.tClass_Socket.tClass_Socket_Base.tEnum_Sock_EventNO.ACCEPT			'' ACCEPT�ڑ�����
				Dim state As tClass.tClass_Socket.tClass_Socket_State_Accept = CType(sender, tClass.tClass_Socket.tClass_Socket_State_Accept)
				'��M�҂�
				state.clsSock.Recv(mcSOCK_DATA_MAXSIZE)				'��M�J�n


			Case tClass.tClass_Socket.tClass_Socket_Base.tEnum_Sock_EventNO.RECV			'' ��M����
				Dim state As tClass.tClass_Socket.tClass_Socket_State_Recv = CType(sender, tClass.tClass_Socket.tClass_Socket_State_Recv)
				'MsgBox("���M�|�[�g ��M,[" & System.Text.Encoding.Unicode.GetString(state.buffer, 0, state.nowRecvSize) & "]")


				' ���[�h����
				Dim strMsg As String = ""
				Dim strSubMsg As String = ""
				Dim bMode As Byte = state.buffer(0)

				If EM_SEND_MODE.EM_SYSLOG = bMode Then	' �V�X���O
					strMsg = System.Text.Encoding.GetEncoding("shift-jis").GetString(state.buffer, 1, state.nowRecvSize - 1)
					strSubMsg = "�V�X���O"

				ElseIf EM_SEND_MODE.EM_MSG = bMode Then	' �`���b�g���b�Z�[�W
					strMsg = System.Text.Encoding.GetEncoding("shift-jis").GetString(state.buffer, 1, state.nowRecvSize - 1)

					'' �t�H�[������\�����̂� �A�C�R����ύX
					If Not Me.Visible Then
						IconChange(False)
						' �A�C�R�����ɖ߂��^�C�}�[�Z�b�g
						SetHideMode(strMsg)
					End If

				ElseIf EM_SEND_MODE.EM_FILE_EXEC = bMode Then	' �w��t�@�C�������s
					strMsg = System.Text.Encoding.GetEncoding("shift-jis").GetString(state.buffer, 1, state.nowRecvSize - 1)
					strSubMsg = "�w��t�@�C�����s"

					Call ExecLocalPc(EM_SEND_MODE.EM_FILE_EXEC, strMsg, state.clsSock.tProp_ToIP)

				ElseIf EM_SEND_MODE.EM_COMMAND_EXEC = bMode Then	' �w��R�}���h�����s
					strMsg = System.Text.Encoding.GetEncoding("shift-jis").GetString(state.buffer, 1, state.nowRecvSize - 1)
					strSubMsg = "�w��R�}���h���s"

					Call ExecLocalPc(EM_SEND_MODE.EM_COMMAND_EXEC, strMsg, state.clsSock.tProp_ToIP)

				Else							' ���o�[�W�����A���m�R�}���h
					strMsg = System.Text.Encoding.GetEncoding("shift-jis").GetString(state.buffer, 0, state.nowRecvSize)
					strSubMsg = "���o�[�W��������̎�M"

					'' �t�H�[������\�����̂� �A�C�R����ύX
					If Not Me.Visible Then
						IconChange(False)
						' �A�C�R�����ɖ߂��^�C�}�[�Z�b�g
						SetHideMode(strMsg)
					End If
				End If



				'' ��M�f�[�^��\��
				SetRecvText(String.Format("=====��M {0}===== [{1}]", strSubMsg, state.clsSock.tProp_ToIP))
				SetRecvText(strMsg & vbCrLf)
				'' ��M����IP�𑗐M��ɃZ�b�g
				SetSendIp(state.clsSock.tProp_ToIP)


			Case tClass.tClass_Socket.tClass_Socket_Base.tEnum_Sock_EventNO.CLOSE_SOCK		'' �ؒf���m
				Dim state As tClass.tClass_Socket.tClass_Socket_State_Close = CType(sender, tClass.tClass_Socket.tClass_Socket_State_Close)

			Case Else
				'' ���肦�Ȃ�
				'MsgBox(String.Format("��,[clsDensou::�H�H�H],�C�x���gNo �ُ�[{0}]", e))
		End Select
	End Sub


	''' <summary>
	''' ���[�J��PC�Ŏ��s
	''' </summary>
	''' <param name="emMode"></param>
	''' <param name="strVal"></param>
	''' <remarks></remarks>
	Private Sub ExecLocalPc(ByVal emMode As EM_SEND_MODE, ByVal strVal As String, ByVal sendPcIp As String)
		Dim strAns As String = String.Format("���� [{0}]", strVal)   ' ����

		Try
			If emMode = EM_SEND_MODE.EM_FILE_EXEC Then
				' �t�@�C��������H
				If 0 = Dir(strVal).Length Then
					strAns = "�Y���t�@�C������"
				Else
					'Process.Start(strVal)
					Shell(strVal)
				End If

			ElseIf emMode = EM_SEND_MODE.EM_COMMAND_EXEC Then
				Shell("cmd /c " & strVal, AppWinStyle.Hide)
			End If

		Catch ex As Exception
			strAns = "���s�E�E�E" & ex.Message
		End Try



		'' ���M�f�[�^
		Dim sendsize As Integer
		Dim sendbuf() As Byte  '���M�o�b�t�@
		sendbuf = CreateSendByte(EM_SEND_MODE.EM_SYSLOG, strAns)
		sendsize = CInt(IIf(mcSOCK_DATA_MAXSIZE < sendbuf.Length, mcSOCK_DATA_MAXSIZE, sendbuf.Length))

		'' ���M�J�n
		If Not SendStart(sendbuf, sendsize, sendPcIp) Then Return
		SetRecvText(strAns & vbCrLf)
	End Sub




End Class