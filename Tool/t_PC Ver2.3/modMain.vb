'*********************************************************************************
' PC �ċN���� �x���N���X
'	[Ver]
'		Ver.01    2007/04/26  �V�K�쐬
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports System.Runtime.InteropServices

Module modMain

	Public Const m_ini As String = ".\t_PC.ini"

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ���[�J���ϐ�
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private Enum mc_nPC
		nMy = 0			' ��PC
		nAll			' �SPC
		nHantei			' ����PC
		nSelect			' �w��PC
	End Enum
	Private Enum mc_nKind
		nLogoff = 0		' ���O�I�t (�����[�g�܂ߑS��)
		nUnForceLogoff	' �����[�g���[�U�[�̂݃��O�I�t
		nDown			' �V���b�g�_�E��
        nReboot         ' �ċN��
        nConsole        ' �R���\�[�����ɐ؂�ւ�
	End Enum


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �Q�Ƃł��Ȃ���`
	'/ /////////////////////////////////////////////////////////////////////////////////
	<StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_MSL, Pack:=1)> _
	Public Structure COMMON_QUE_STRMIX
		Dim nEventNo As Integer
		Dim nLineNo As Integer

		<VBFixedArray(20 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=20)> _
		Dim idata() As Integer

		'// ��ByValTStr ���ꂾ�� �I�[�ɏ����NULL���t�^�����ׁA�I�[1�o�C�g���폜�����B
		<MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
		Dim cdata1 As String
		<MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
		Dim cdata2 As String
		<MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
		Dim cdata3 As String
		<MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
		Dim cdata4 As String
		<MarshalAs(UnmanagedType.ByValTStr, SizeConst:=32)> _
		Dim cdata5 As String

		<MarshalAs(UnmanagedType.ByValTStr, SizeConst:=(SIZE_MSL_DATA - 4 * 20 - 32 * 5))> _
		Dim ydata As String

		Public Sub inisialize()
			ReDim idata(20 - 1)
		End Sub

	End Structure





	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ���[�J���ϐ�
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private WithEvents mcls_Mail As tClass.tClass_MailSlot				' ���[���X���b�g��M�N���X
	Private m_RecvQueBuf As COMMON_QUE_STRMIX							' ���[���X���b�g��M�o�b�t�@

	Private m_ap As ApplicationContext									' �R���e�L�X�g

	Private WithEvents m_nfyIcon As System.Windows.Forms.NotifyIcon		' �^�X�N�g���C�̃A�C�R�� �R���g���[��
	Private m_cPcName As String											' ��PC����

	Private m_frmChat As frmChat										' �`���b�g�p�t�H�[��
	Private m_icNomal As Icon											' �ʏ�A�C�R��
	Private m_icRecv As Icon											' ���[����M���̃A�C�R��




	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ��`
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �G���g���[�|�C���g
	''' </summary>
	''' <remarks></remarks>
	Public Sub main()

        '' �^�X�N�C�j�V����
        Try
            mcls_Mail = New tClass.tClass_MailSlot("t_PC", 300)
            mcls_Mail.Mail_ReadStart(m_RecvQueBuf, AddressOf tDele_GetMail)
        Catch ex As Exception
            'MsgBox("Init���s")
            End
        End Try


		'' ������돉����
		m_cPcName = System.Windows.Forms.SystemInformation.ComputerName()		' �R���s���[�^�[���擾

		'' �^�X�N�g���C�̃A�C�R���擾
		Try															' �A�C�R���ݒ�
			'' ���ߍ��܂ꂽ���\�[�X����A�C�R�������o��
			Dim MyAsm As Reflection.Assembly = Reflection.Assembly.GetExecutingAssembly()
			m_icNomal = New Icon(MyAsm.GetManifestResourceStream("t_PC.t_PC.ico"))
			m_icRecv = New Icon(MyAsm.GetManifestResourceStream("t_PC.t_PCchatrecv.ico"))


		Catch ex As Exception
		End Try


		'' �^�X�N�g���C�̃A�C�R�� �R���g���[�� �C���X�^���X����
		m_nfyIcon = New System.Windows.Forms.NotifyIcon
		m_nfyIcon.ContextMenu = CreateNotifyIconMenu_Def()		'�^�X�N�g���C�̃A�C�R�����j���[��`
		m_nfyIcon.Text = "PC���쐧��"
		IconChange(True)	' �ʏ�A�C�R���Z�b�g
		m_nfyIcon.Visible = True






		'' �`���b�g�p�t�H�[������
		m_frmChat = New frmChat
		m_frmChat.Location = New Point(My.Computer.Screen.WorkingArea.Width - m_frmChat.Size.Width, My.Computer.Screen.WorkingArea.Height - m_frmChat.Size.Height)
		m_frmChat.Alloc()		'�\�P�b�g����

		m_frmChat.Opacity = 0	' �\������Ƃ��� ���������鎖�� �t�H�[���N���̈�u�̂������h�~
		m_frmChat.Show()
		m_frmChat.Hide()
		m_frmChat.Opacity = 1

		'' ���C���X���b�h����
		m_ap = New ApplicationContext
		Application.Run(m_ap)			'ExitThread���R�[�������܂� �����Ńu���b�N

		'' �I��
		m_frmChat.Free()		'�\�P�b�g���
		m_frmChat.Dispose()

		m_nfyIcon.Dispose()
		' ''mcls_Mod.Task_Exit()
		mcls_Mail.Mail_ReadStop()
		mcls_Mail.Dispose()
		mcls_Mail = Nothing
	End Sub

	''' <summary>
	''' �^�X�N�g���C�̃A�C�R�����j���[��`
	''' </summary>
	''' <returns></returns>
	''' <remarks></remarks>
	Private Function CreateNotifyIconMenu_Def() As ContextMenu

        Dim closeItem() As MenuItem = { _
         New MenuItem("��PC���O�I�t", AddressOf m_menuItem_Click), _
         New MenuItem("��PC(���̂�)۸޵�", AddressOf m_menuItem_Click), _
         New MenuItem("��PC�V���b�g�_�E��", AddressOf m_menuItem_Click), _
         New MenuItem("��PC�ċN��", AddressOf m_menuItem_Click), _
         New MenuItem("[�풓��ݿ�ّ���]", AddressOf m_menuItem_Click), _
         New MenuItem("�SPC���O�I�t", AddressOf m_menuItem_Click), _
         New MenuItem("�SPC(���̂�)۸޵�", AddressOf m_menuItem_Click), _
         New MenuItem("�SPC�V���b�g�_�E��", AddressOf m_menuItem_Click), _
         New MenuItem("�SPC�ċN��", AddressOf m_menuItem_Click), _
         New MenuItem("[�풓��ݿ�ّ���]", AddressOf m_menuItem_Click), _
         New MenuItem("����PC���O�I�t", AddressOf m_menuItem_Click), _
         New MenuItem("����PC(���̂�)۸޵�", AddressOf m_menuItem_Click), _
         New MenuItem("����PC�V���b�g�_�E��", AddressOf m_menuItem_Click), _
         New MenuItem("����PC�ċN��", AddressOf m_menuItem_Click), _
         New MenuItem("[�풓��ݿ�ّ���]", AddressOf m_menuItem_Click), _
         New MenuItem("�w��PC���O�I�t", AddressOf m_menuItem_Click), _
         New MenuItem("�w��PC(���̂�)۸޵�", AddressOf m_menuItem_Click), _
         New MenuItem("�w��PC�V���b�g�_�E��", AddressOf m_menuItem_Click), _
         New MenuItem("�w��PC�ċN��", AddressOf m_menuItem_Click), _
         New MenuItem("[�풓��ݿ�ّ���]", AddressOf m_menuItem_Click), _
         New MenuItem("�S�c�[���I��", AddressOf m_menuItem_Click), _
         New MenuItem("�c�[���I��[" & m_cPcName & "]", AddressOf m_menuItem_Click) _
         }

		'' ���s�ݒ�
        closeItem(5).BarBreak = True
        closeItem(10).BarBreak = True
        closeItem(15).BarBreak = True
        closeItem(20).BarBreak = True

		'' �R���e�L�X�g���j���[��`
		Dim menu As New ContextMenu
		menu.MenuItems.AddRange(closeItem)

		Return menu
	End Function

	''' <summary>
	''' �^�X�N�g���C�̃A�C�R�����j���[�N���b�N�C�x���g
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub m_menuItem_Click(ByVal sender As Object, ByVal e As System.EventArgs)
		' ���M�L���[
		Dim que As COMMON_QUE_STRMIX = Nothing
		que.inisialize()
		que.nEventNo = 9800

		' ���j���[�ɂ�菈������
		Dim menu As MenuItem = CType(sender, MenuItem)
		Select Case menu.Index
			Case 0									' ��PC���O�I�t
				que.idata(0) = mc_nPC.nMy
				que.idata(1) = mc_nKind.nLogoff
                mcls_Mail.Mail_Send(".", "t_PC", que)
			Case 1									' ��PC���O�I�t(���̂�)
				que.idata(0) = mc_nPC.nMy
				que.idata(1) = mc_nKind.nUnForceLogoff
                mcls_Mail.Mail_Send(".", "t_PC", que)
			Case 2									' ��PC�V���b�g�_�E��
				que.idata(0) = mc_nPC.nMy
				que.idata(1) = mc_nKind.nDown
                mcls_Mail.Mail_Send(".", "t_PC", que)
			Case 3									' ��PC�ċN��
				que.idata(0) = mc_nPC.nMy
				que.idata(1) = mc_nKind.nReboot
                mcls_Mail.Mail_Send(".", "t_PC", que)
            Case 4                                  ' �R���\�[��
                que.idata(0) = mc_nPC.nMy
                que.idata(1) = mc_nKind.nConsole
                mcls_Mail.Mail_Send(".", "t_PC", que)


            Case 5                                  ' �SPC���O�I�t
                If MsgBox("�SPC���O�I�t���܂����H", MsgBoxStyle.OkCancel, "�m�F") <> MsgBoxResult.Ok Then Return
                que.idata(0) = mc_nPC.nAll
                que.idata(1) = mc_nKind.nLogoff
                mcls_Mail.Mail_Send("*", "t_PC", que)
            Case 6                                  ' �SPC���O�I�t(���̂�)
                If MsgBox("�SPC(�ݿ�وȊO)���O�I�t���܂����H", MsgBoxStyle.OkCancel, "�m�F") <> MsgBoxResult.Ok Then Return
                que.idata(0) = mc_nPC.nAll
                que.idata(1) = mc_nKind.nUnForceLogoff
                mcls_Mail.Mail_Send("*", "t_PC", que)
            Case 7                                  ' �SPC�V���b�g�_�E��
                If MsgBox("�SPC�V���b�g�_�E�����܂����H", MsgBoxStyle.OkCancel, "�m�F") <> MsgBoxResult.Ok Then Return
                que.idata(0) = mc_nPC.nAll
                que.idata(1) = mc_nKind.nDown
                mcls_Mail.Mail_Send("*", "t_PC", que)
            Case 8                                  ' �SPC�ċN��
                If MsgBox("�SPC�ċN�����܂����H", MsgBoxStyle.OkCancel, "�m�F") <> MsgBoxResult.Ok Then Return
                que.idata(0) = mc_nPC.nAll
                que.idata(1) = mc_nKind.nReboot
                mcls_Mail.Mail_Send("*", "t_PC", que)
            Case 9                                  ' 
                If MsgBox("�SPC�풓�����ݿ�ّ��ɂ��܂����H", MsgBoxStyle.OkCancel, "�m�F") <> MsgBoxResult.Ok Then Return
                que.idata(0) = mc_nPC.nAll
                que.idata(1) = mc_nKind.nConsole
                mcls_Mail.Mail_Send("*", "t_PC", que)


            Case 10                             ' ����PC���O�I�t
                If MsgBox("�S����PC���O�I�t���܂����H", MsgBoxStyle.OkCancel, "�m�F") <> MsgBoxResult.Ok Then Return
                que.idata(0) = mc_nPC.nHantei
                que.idata(1) = mc_nKind.nLogoff
                mcls_Mail.Mail_Send("*", "t_PC", que)
            Case 11                             ' ����PC���O�I�t(���̂�)
                If MsgBox("�S����PC(�ݿ�وȊO)���O�I�t���܂����H", MsgBoxStyle.OkCancel, "�m�F") <> MsgBoxResult.Ok Then Return
                que.idata(0) = mc_nPC.nHantei
                que.idata(1) = mc_nKind.nUnForceLogoff
                mcls_Mail.Mail_Send("*", "t_PC", que)
            Case 12                                 ' ����PC�V���b�g�_�E��
                If MsgBox("�S����PC�V���b�g�_�E�����܂����H", MsgBoxStyle.OkCancel, "�m�F") <> MsgBoxResult.Ok Then Return
                que.idata(0) = mc_nPC.nHantei
                que.idata(1) = mc_nKind.nDown
                mcls_Mail.Mail_Send("*", "t_PC", que)
            Case 13                                 ' ����PC�ċN��
                If MsgBox("�S����PC�ċN�����܂����H", MsgBoxStyle.OkCancel, "�m�F") <> MsgBoxResult.Ok Then Return
                que.idata(0) = mc_nPC.nHantei
                que.idata(1) = mc_nKind.nReboot
                mcls_Mail.Mail_Send("*", "t_PC", que)
            Case 14                                 ' ����PC�ċN��
                If MsgBox("�S����풓�����ݿ�ّ��ɂ��܂����H", MsgBoxStyle.OkCancel, "�m�F") <> MsgBoxResult.Ok Then Return
                que.idata(0) = mc_nPC.nHantei
                que.idata(1) = mc_nKind.nConsole
                mcls_Mail.Mail_Send("*", "t_PC", que)



            Case 15                                 ' �w��PC���O�I�t
                Dim strName As String = InputBox("�w��PC���̓��͗v��" & vbCrLf & "  (���͕����̐擪�����v����SPC���ΏۂƂȂ�B�啶���������֌W�Ȃ���)", "PC���̎w��")
                If strName.Length = 0 Then Exit Sub
                que.idata(0) = mc_nPC.nSelect
                que.idata(1) = mc_nKind.nLogoff
                que.cdata1 = strName
                mcls_Mail.Mail_Send("*", "t_PC", que)
            Case 16                                 ' �w��PC���O�I�t
                Dim strName As String = InputBox("�w��PC���̓��͗v��" & vbCrLf & "  (���͕����̐擪�����v����SPC���ΏۂƂȂ�B�啶���������֌W�Ȃ���)", "PC���̎w��")
                If strName.Length = 0 Then Exit Sub
                que.idata(0) = mc_nPC.nSelect
                que.idata(1) = mc_nKind.nUnForceLogoff
                que.cdata1 = strName
                mcls_Mail.Mail_Send("*", "t_PC", que)
            Case 17                                 ' �w��PC�V���b�g�_�E��
                Dim strName As String = InputBox("�w��PC���̓��͗v��" & vbCrLf & "  (���͕����̐擪�����v����SPC���ΏۂƂȂ�B�啶���������֌W�Ȃ���)", "PC���̎w��")
                If strName.Length = 0 Then Exit Sub
                que.idata(0) = mc_nPC.nSelect
                que.idata(1) = mc_nKind.nDown
                que.cdata1 = strName
                mcls_Mail.Mail_Send("*", "t_PC", que)
            Case 18                                 ' �w��PC�ċN��
                Dim strName As String = InputBox("�w��PC���̓��͗v��" & vbCrLf & "  (���͕����̐擪�����v����SPC���ΏۂƂȂ�B�啶���������֌W�Ȃ���)", "PC���̎w��")
                If strName.Length = 0 Then Exit Sub
                que.idata(0) = mc_nPC.nSelect
                que.idata(1) = mc_nKind.nReboot
                que.cdata1 = strName
                mcls_Mail.Mail_Send("*", "t_PC", que)
            Case 19
                Dim strName As String = InputBox("�w��PC���̓��͗v��" & vbCrLf & "  (���͕����̐擪�����v����SPC���ΏۂƂȂ�B�啶���������֌W�Ȃ���)", "PC���̎w��")
                If strName.Length = 0 Then Exit Sub
                que.idata(0) = mc_nPC.nSelect
                que.idata(1) = mc_nKind.nConsole
                que.cdata1 = strName
                mcls_Mail.Mail_Send("*", "t_PC", que)


            Case 20                                 ' �S�c�[���I��
                que.nEventNo = FACT_KS_END
                mcls_Mail.Mail_Send("*", "t_PC", que)
            Case 21                                 ' �c�[���I��
                que.nEventNo = FACT_KS_END
                mcls_Mail.Mail_Send(".", "t_PC", que)
        End Select
	End Sub

	''' <summary>
	''' ���[���X���b�g��M
	''' </summary>
    Public Sub tDele_GetMail(ByVal recvVal As System.ValueType)
        Dim que As COMMON_QUE_STRMIX = CType(recvVal, COMMON_QUE_STRMIX)
        Dim nEvtNo As Integer = que.nEventNo

        Select Case nEvtNo
            Case 9800
                ' ���̃��[���͎������ΏہH
                If que.idata(0) = mc_nPC.nMy Then                       '��PC
                ElseIf que.idata(0) = mc_nPC.nAll Then                  '�SPC
                ElseIf que.idata(0) = mc_nPC.nHantei Then               '����PC
                    If Not m_cPcName.StartsWith("hantei", StringComparison.CurrentCultureIgnoreCase) Then
                        Return
                    End If
                ElseIf que.idata(0) = mc_nPC.nSelect Then               ' �w��PC
                    If Not m_cPcName.StartsWith(que.cdata1, StringComparison.CurrentCultureIgnoreCase) Then
                        Return
                    End If
                Else
                    'NG
                    Return
                End If

                '''' �����܂ŗ�����Ώۂ̏ꍇ�������肦�Ȃ�
                Dim bMyTaskEnd As Boolean = True
                ' �����킯
                If que.idata(1) = mc_nKind.nLogoff Then             ' ���O�I�t
                    ' �܂����̑��Z�b�V�������B
                    AllUserLogOff.Exec()

                    tClass.tClass_Shutdown.ExitWindows(tClass.tClass_Shutdown.ShutdownFlag.LogOff Or tClass.tClass_Shutdown.ShutdownFlag.Force Or tClass.tClass_Shutdown.ShutdownFlag.ForceIfHung)

                ElseIf que.idata(1) = mc_nKind.nUnForceLogoff Then        ' ���O�I�t�i�R���\�[���ȊO�j
                    AllUserLogOff.Exec()
                    bMyTaskEnd = False

                ElseIf que.idata(1) = mc_nKind.nDown Then           ' �V���b�g�_�E��
                    tClass.tClass_Shutdown.ExitWindows(tClass.tClass_Shutdown.ShutdownFlag.Shutdown Or tClass.tClass_Shutdown.ShutdownFlag.Force Or tClass.tClass_Shutdown.ShutdownFlag.ForceIfHung)
                ElseIf que.idata(1) = mc_nKind.nReboot Then         ' �ċN��
                    tClass.tClass_Shutdown.ExitWindows(tClass.tClass_Shutdown.ShutdownFlag.Reboot Or tClass.tClass_Shutdown.ShutdownFlag.Force Or tClass.tClass_Shutdown.ShutdownFlag.ForceIfHung)
                ElseIf que.idata(1) = mc_nKind.nConsole Then         ' �R���\�[����
                    AllUserLogOff.Change()
                    bMyTaskEnd = False
                End If

                ' ���^�X�N���ǂ����邩�H
                If bMyTaskEnd Then
                    m_ap.ExitThread()
                End If

            Case FACT_KS_END                            ' �^�X�N�I��
                m_ap.ExitThread()                                       ' ���C���X���b�h�I��
        End Select

    End Sub







	''' <summary>
	''' �^�X�N�g���C�̃A�C�R���ύX
	''' </summary>
	''' <param name="bNomal">true:�m�[�}��  false:�`���b�g��M</param>
	''' <remarks></remarks>
	Public Sub IconChange(ByVal bNomal As Boolean)
		If bNomal Then
			m_nfyIcon.Icon = m_icNomal
		Else
			m_nfyIcon.Icon = m_icRecv
		End If
	End Sub

	''' <summary>
	''' �^�X�N�g���C�A�C�R�� �_�u���N���b�N
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub nfyIcon_DoubleClick(ByVal sender As Object, ByVal e As System.EventArgs) Handles m_nfyIcon.DoubleClick
		'' �A�C�R�������ɖ߂�
		IconChange(True)

		'' �t�H�[���\�����Ȃ����
		If m_frmChat.Visible Then
			m_frmChat.Hide()
		Else
			'' �t�H�[���\��
			m_frmChat.Show()
			m_frmChat.TopMost = True	'�ŏ��̂ݍőO�ʂɏo��
			m_frmChat.TopMost = False
			'�J���b�g�ʒu�܂ŃX�N���[�� (�Ō�̈ʒu�ƂȂ�B����̓e�L�X�g�{�b�N�X�Ƀt�H�[�J�X���������ĂȂ��ƃ_��)
			m_frmChat.TxtScroll()
		End If
	End Sub

End Module