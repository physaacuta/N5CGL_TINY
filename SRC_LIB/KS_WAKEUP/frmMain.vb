Public Class frmMain
	'/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�萔
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private Delegate Sub m_deleSetImgLoad(ByVal pc As Integer, ByVal rec As Integer)	   '�ʃX���b�h����̃R���g���[���A�N�Z�X

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ���`
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_nMyID As Integer                              ' ��ID (���j�^�ԍ�:0�I���W��)

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ���\�b�h��`
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �R���X�g���N�^
	''' </summary>
	''' <param name="nMyID">��ID (���j�^�ԍ�:0�I���W��)</param>
	''' <remarks></remarks>
	Public Sub New(ByVal nMyID As Integer)

		' ���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
		InitializeComponent()

		' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B
		m_nMyID = nMyID
	End Sub

	''' <summary>
	''' ��ʕ\��
	''' </summary>
	''' <remarks></remarks>
	Public Shadows Sub Show()
		SetImgLoadEntry(0, False)		' ����
		SetImgLoadEntry(1, False)		' �~��

		'' �\�����������j�^�̏��ɂ��킹�ăt�H�[���T�C�Y��ύX
		Dim monitor() As System.Windows.Forms.Screen
		monitor = System.Windows.Forms.Screen.AllScreens()
		Me.Bounds = monitor(m_nMyID).Bounds
		Me.TopMost = True				 '�őO��		'��߃f�o�b�N���ɔ��ɂ��ɂ�������


		'' ����ʐݒ�
		'Me.Size = New Size(My.Computer.Screen.Bounds.Width, My.Computer.Screen.Bounds.Height)		' ��ʂ�������ɂ���
		' ''Me.TopMost = True				 '�őO��		'��߃f�o�b�N���ɔ��ɂ��ɂ�������

		'' �}���`���j�^�p�ɍ��E�ɕ\��
		'If m_nMyID = 0 Then		'��
		'	Me.Location = New Point(0, 0)
		'Else					'�E
		'	Me.Location = New Point(Me.Size.Width, 0)
		'End If

		' �x�[�X��ʕ\��
		MyBase.Show()
	End Sub

	''' <summary>
	''' �C���[�W�����[�h����G���g���[���\�b�h
	''' </summary>
	''' <param name="pc">pc (0:���� 1:�~��)</param>
	''' <param name="mode">PC ON/OFF</param>
    Public Sub SetImgLoadEntry(ByVal pc As Integer, ByVal mode As Boolean)
        Dim rec As Integer = CInt(IIf(mode, 1, 0))   '�C���[�W���X�g (0:OFF 1:ON)

		' �ʃX���b�h����̃A�N�Z�X�\�Ƃ���
		If Me.InvokeRequired Then					' �ʃX���b�h����̃A�N�Z�X�v��
			Dim dele As New m_deleSetImgLoad(AddressOf SetImgLoad)
			Me.Invoke(dele, pc, rec)
		Else										' �t�H�[�������X���b�h����̃A�N�Z�X�v��
			Call SetImgLoad(pc, rec)
		End If

    End Sub




	''' <summary>
	''' �C���[�W�����[�h
	''' </summary>
	''' <param name="pc">pc (0:���� 1:�~��)</param>
	''' <param name="rec">�C���[�W���X�g (0:OFF 1:ON)</param>
	Private Sub SetImgLoad(ByVal pc As Integer, ByVal rec As Integer)
        If pc = modMain.em_Delivery.iTO Then                           ' ����PC
            picTokatu.Image = imgList.Images(rec)
        ElseIf pc = modMain.em_Delivery.iTI Then                      ' �~��PC
            picTikuseki.Image = imgList.Images(rec)
        End If
	End Sub

	''' <summary>
	''' �L�[�_�E��
	''' </summary>
	Private Sub frmMain_KeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles Me.KeyDown
		'If e.KeyValue = Keys.F12 AndAlso e.Modifiers = Keys.Shift Then
		'	'' ��ʋ����I���ʒm
		'	Dim que As KizuLibCLI.COMMON_QUE_L = Nothing
		'	que.nEventNo = FACT_KS_END
		'	que.nLineNo = KizuLibCLI.KizuMod.GetLineID()
		'	KizuLibCLI.KizuMod.Send_Mail(KizuLibCLI.KizuMod.GetMyTaskName(), ".", que)

		'ElseIf e.KeyValue = Keys.F11 AndAlso e.Modifiers = Keys.Shift Then	' ����PC�e�X�g
		'	Dim que As KizuLibCLI.COMMON_QUE_L = Nothing
		'	que.nEventNo = FACT_KS_WAKEUP_01
		'	que.nLineNo = KizuLibCLI.KizuMod.GetLineID()
		'	KizuLibCLI.KizuMod.Send_Mail(KizuLibCLI.KizuMod.GetMyTaskName(), ".", que)

		'ElseIf e.KeyValue = Keys.F10 AndAlso e.Modifiers = Keys.Shift Then	' �~��PC�e�X�g
		'	Dim que As KizuLibCLI.COMMON_QUE_L = Nothing
		'	que.nEventNo = FACT_KS_WAKEUP_02
		'	que.nLineNo = KizuLibCLI.KizuMod.GetLineID()
		'	KizuLibCLI.KizuMod.Send_Mail(KizuLibCLI.KizuMod.GetMyTaskName(), ".", que)

		'ElseIf e.KeyValue = Keys.F1 AndAlso e.Modifiers = Keys.Shift Then	' �őS�ʐؑ�
		'	Me.TopMost = Not Me.TopMost
		'End If

		If e.KeyValue = Keys.F12 AndAlso e.Modifiers = Keys.Shift Then
			'' ��ʋ����I���ʒm
			Dim que As COMMON_QUE_L = Nothing
			que.nEventNo = FACT_KS_END
			que.nLineNo = 0
			gcls_Mail.Mail_Send(".", KS_WAKEUP, que)

		ElseIf e.KeyValue = Keys.F11 AndAlso e.Modifiers = Keys.Shift Then	' ����PC�e�X�g
			Dim que As COMMON_QUE_L = Nothing
			que.nEventNo = FACT_KS_WAKEUP_01
			que.nLineNo = 0
			gcls_Mail.Mail_Send(".", KS_WAKEUP, que)

		ElseIf e.KeyValue = Keys.F10 AndAlso e.Modifiers = Keys.Shift Then	' �~��PC�e�X�g
			Dim que As COMMON_QUE_L = Nothing
			que.nEventNo = FACT_KS_WAKEUP_02
			que.nLineNo = 0
			gcls_Mail.Mail_Send(".", KS_WAKEUP, que)

		ElseIf e.KeyValue = Keys.F1 AndAlso e.Modifiers = Keys.Shift Then	' �őS�ʐؑ�
			Me.TopMost = Not Me.TopMost
		End If
	End Sub

	''' <summary>
	''' �^�C�}�[
	''' </summary>
	Private Sub trmCycle_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles trmCycle.Tick
		If ProgressBar1.Value = ProgressBar1.Maximum Then
			ProgressBar1.Value = 0
		End If
		ProgressBar1.Increment(1)
	End Sub

End Class
