Option Strict On								' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)


Imports KizuLibCLI
Imports tClassLibrary.tClass.tClass_LogManager
Imports tClassLibrary
Imports System.Reflection

'''''''''''''''''''''
'Imports system.Runtime.InteropServices					' �����w��
'	<StructLayout(LayoutKind.Sequential, Size:=400, Pack:=1)> _
'	Structure FL
'		<VBFixedArray(100), MarshalAs(UnmanagedType.ByValArray, SizeConst:=100)> _
'		Dim data() As Integer

'		Public Sub inisialize()
'			ReDim data(99)
'		End Sub
'	End Structure
''''''''''''''''




Public Class frmMain
	Private WithEvents mcls_Mod As KizuLibCLI.KizuMod                   ' �����N���X
	Private mcls_Log As tClassLibrary.tClass.tClass_LogManager			' ���O�Ǘ��N���X

	Private Delegate Sub deleRecvMail(ByVal pDate As Object, ByVal nEvtNo As Integer)      '�ʃX���b�h����̃R���g���[���A�N�Z�X


	''' <summary>
	''' �t�H�[�����[�h
	''' </summary>
	Private Sub frmMain_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
		' ��KizuLibCLI 32bit / 64bit �؂�ւ��p
		AddHandler AppDomain.CurrentDomain.AssemblyResolve, New ResolveEventHandler(AddressOf OnAssemblyResolve)

		Task_First()

	End Sub


	''' <summary>
	''' �t�H�[���I��
	''' </summary>
	Private Sub frmMain_FormClosed(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosedEventArgs) Handles Me.FormClosed
		' ���X�g�J��
		mcls_Log.Dispose()
		mcls_Log = Nothing
		' ���[���N���X���
		mcls_Mod.Dispose()
		mcls_Mod = Nothing
	End Sub

	'���O�t�@�C���o�͖����̈� ����
	'''' <summary>
	'''' ���O���X�g �_�u���N���b�N (�������N��)
	'''' </summary>
	'Private Sub lstMsg_DoubleClick(ByVal sender As Object, ByVal e As System.EventArgs) Handles lstMsg.DoubleClick
	'	mcls_Log.ShowEdit()
	'End Sub

	''' <summary>
	''' �^�X�N������������
	''' </summary>
	''' <remarks>Main����KizuLibCLI�ɃA�N�Z�X�����Ȃ����߂ɕ�������</remarks>
	Private Sub Task_First()
		mcls_Mod = New KizuLibCLI.KizuMod
		mcls_Log = New tClassLibrary.tClass.tClass_LogManager(Nothing, Nothing, lstMsg)
	End Sub

	''' <summary>
	''' ���[���X���b�g ����
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnMailOpen_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnMailOpen.Click
		If txtRecvTask.Text.Trim.Length = 0 Then
			mcls_Log.Write(LNO.ERR, "�����^�X�N�������I")
			Exit Sub
		End If

		' ����
		If 0 <> mcls_Mod.Task_Init(txtRecvTask.Text, 0) Then
			mcls_Log.Write(LNO.ERR, "�������s�I")
			Exit Sub
		End If
		mcls_Log.Write(LNO.MSG, "��������")
	End Sub

	''' <summary>
	''' ���[���X���b�g �J��
	''' </summary>
	Private Sub btnMailClose_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnMailClose.Click
		If Not mcls_Mod Is Nothing Then
			mcls_Mod.Task_Exit()
			mcls_Log.Write(LNO.MSG, "�J������")
		End If
	End Sub

	''' <summary>
	''' ���[����M��̔w�i�F���ʂ�
	''' </summary>
	Private Sub tmrRecving_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tmrRecving.Tick
		tmrRecving.Enabled = False
		gpbRecv.BackColor = SystemColors.Control
	End Sub


	''' <summary>
	''' ���M�{�^���N���b�N
	''' </summary>
	Private Sub btnSend_Int_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSend_Int.Click, btnSend_Str.Click, btnSend_Mix.Click
		On Error GoTo NG

		'' ���͒l�`�F�b�N
		If 0 = txtSendPC.Text.Trim.Length Then
			mcls_Log.Write(LNO.ERR, "���MPC�������I")
			Exit Sub
		End If
		If 0 = txtSendTask.Text.Trim.Length Then
			mcls_Log.Write(LNO.ERR, "���M�^�X�N�������I")
			Exit Sub
		End If
		If False = IsNumeric(txtSendEvtNo.Text.Trim) Then
			mcls_Log.Write(LNO.ERR, "���M�C�x���gNo�ُ�I")
			Exit Sub
		End If
		Dim strPc As String = txtSendPC.Text.Trim
		Dim strTask As String = txtSendTask.Text.Trim
		Dim iEvtNo As Integer = CInt(txtSendEvtNo.Text.Trim)
		Dim iLineNo As Integer = CInt(txtSendLineNo.Text.Trim)
        Dim strUdpAddr As String = txtUdpAddr.Text.Trim

		' ���M�f�[�^�Z�b�g
		Dim retc As Integer
        Dim mode As Integer = CInt(CType(sender, Button).Tag)
        If 0 = mode Then
            Dim que As New KizuLibCLI.COMMON_QUE_L
            que.inisialize()
            que.nEventNo = iEvtNo
            que.nLineNo = iLineNo
            que.data(0) = CInt(txtSend_INT0.Text.Trim)
            que.data(1) = CInt(txtSend_INT1.Text.Trim)
            que.data(2) = CInt(txtSend_INT2.Text.Trim)
            que.data(3) = CInt(txtSend_INT3.Text.Trim)
            que.data(4) = CInt(txtSend_INT4.Text.Trim)
            que.data(5) = CInt(txtSend_INT5.Text.Trim)
            que.data(6) = CInt(txtSend_INT6.Text.Trim)
            que.data(7) = CInt(txtSend_INT7.Text.Trim)
            que.data(8) = CInt(txtSend_INT8.Text.Trim)
            que.data(9) = CInt(txtSend_INT9.Text.Trim)
            If chkTrans.Checked Then
                ' ���[���]��
                If "" <> strUdpAddr Then
                    retc = KizuMod.Send_Mail_Trans(strUdpAddr, strTask, strPc, que)
                Else
                    retc = KizuMod.Send_Mail_Trans(strTask, strPc, que)
                End If

            Else
                ' ���[�����M
                retc = KizuMod.Send_Mail(strTask, strPc, que)
            End If
            If retc = 0 Then
                mcls_Log.Write(LNO.MSG, "L ���M���� Evt=" & iEvtNo)
            Else
                mcls_Log.Write(LNO.ERR, "L ���M���s Evt=" & iEvtNo & " retc=" & retc)
            End If

        ElseIf 1 = mode Then
            Dim que As New KizuLibCLI.COMMON_QUE_S
            'Dim que As New KizuLibCLI.COMMON_QUE_STR
            que.inisialize()
            que.nEventNo = iEvtNo
            que.nLineNo = iLineNo
            tMod.StringToByte(que.data, txtSend_STR.Text.Trim)
            If chkTrans.Checked Then
                ' ���[���]��
                If "" <> strUdpAddr Then
                    retc = KizuMod.Send_Mail_Trans(strUdpAddr, strTask, strPc, que)
                Else
                    retc = KizuMod.Send_Mail_Trans(strTask, strPc, que)
                End If
            Else
                ' ���[�����M
                retc = KizuMod.Send_Mail(strTask, strPc, que)
            End If
            If retc = 0 Then
                mcls_Log.Write(LNO.MSG, "S ���M���� Evt=" & iEvtNo)
            Else
                mcls_Log.Write(LNO.ERR, "S ���M���s Evt=" & iEvtNo & " retc=" & retc)
            End If

        ElseIf 2 = mode Then
            Dim que As New KizuLibCLI.COMMON_QUE_MIX
            'Dim que As New KizuLibCLI.COMMON_QUE_STRMIX
            que.inisialize()
            que.nEventNo = iEvtNo
            que.nLineNo = iLineNo
            que.idata(0) = CInt(txtSend_INT0.Text.Trim)
            que.idata(1) = CInt(txtSend_INT1.Text.Trim)
            que.idata(2) = CInt(txtSend_INT2.Text.Trim)
            que.idata(3) = CInt(txtSend_INT3.Text.Trim)
            que.idata(4) = CInt(txtSend_INT4.Text.Trim)
            que.idata(5) = CInt(txtSend_INT5.Text.Trim)
            que.idata(6) = CInt(txtSend_INT6.Text.Trim)
            que.idata(7) = CInt(txtSend_INT7.Text.Trim)
            que.idata(8) = CInt(txtSend_INT8.Text.Trim)
            que.idata(9) = CInt(txtSend_INT9.Text.Trim)
            tMod.StringToByte(que.cdata1, txtSend_STR.Text.Trim)
            tMod.StringToByte(que.cdata2, txtSend_STR2.Text.Trim)
            tMod.StringToByte(que.cdata3, txtSend_STR3.Text.Trim)
            tMod.StringToByte(que.cdata4, txtSend_STR4.Text.Trim)
            tMod.StringToByte(que.cdata5, txtSend_STR5.Text.Trim)

            'que.cdata1 = txtSend_STR.Text.Trim

            If chkTrans.Checked Then
                ' ���[���]��
                If "" <> strUdpAddr Then
                    retc = KizuMod.Send_Mail_Trans(strUdpAddr, strTask, strPc, que)
                Else
                    retc = KizuMod.Send_Mail_Trans(strTask, strPc, que)
                End If
                retc = KizuMod.Send_Mail_Trans(strTask, strPc, que)
            Else
                ' ���[�����M
                retc = KizuMod.Send_Mail(strTask, strPc, que)
            End If
            If retc = 0 Then
                mcls_Log.Write(LNO.MSG, "MIX ���M���� Evt=" & iEvtNo)
            Else
                mcls_Log.Write(LNO.ERR, "MIX ���M���s Evt=" & iEvtNo & " retc=" & retc)
            End If

            End If
        Exit Sub
NG:
        mcls_Log.Write(LNO.ERR, "���M���ɉ�������̃G���[")
	End Sub


	''' <summary>
	''' ���[����M�C�x���g (�R���g���[���̃��C���X���b�h�Ƃ͕ʃX���b�h�Ȃ̂Œ��ӂ��鎖)
	''' </summary>
	''' <param name="pData">��M�\���� (COMMON_QUE_L�̍\���ׁ̂̈A�g���������ɕύX���Ă�)</param>
	''' <param name="nEvtNo"></param>
	''' <remarks></remarks>
	Private Sub mcls_Mail_evRecvMail(ByVal pData As Object, ByVal nEvtNo As Integer) Handles mcls_Mod.evRecvMail
		Dim dele As New deleRecvMail(AddressOf subDeleRecvMail)

		'Me.BeginInvoke(dele, New Object() {pData, nEvtNo})			'�ʃX���b�h����̃A�N�Z�X�p
		'Me.Invoke(dele, New Object() {pData, nEvtNo})			'�ʃX���b�h����̃A�N�Z�X�p
		Me.Invoke(dele, pData, nEvtNo)					'�ʃX���b�h����̃A�N�Z�X�p
	End Sub
	Private Sub subDeleRecvMail(ByVal pData As Object, ByVal nEvtNo As Integer)
		' �x�[�X��COMMON_QUE_L�ł�
		Dim que_l As COMMON_QUE_L = CType(pData, COMMON_QUE_L)
		Dim que_s As COMMON_QUE_S = CType(mcls_Mod.typConv(pData, GetType(COMMON_QUE_S)), COMMON_QUE_S)
		Dim que_m As COMMON_QUE_MIX = CType(mcls_Mod.typConv(pData, GetType(COMMON_QUE_MIX)), COMMON_QUE_MIX)

'Dim que_sss As COMMON_QUE_STR = CType(mcls_Mod.typConv(pData, GetType(COMMON_QUE_STR)), COMMON_QUE_STR)
'Dim que_mmm As COMMON_QUE_STRMIX = CType(mcls_Mod.typConv(pData, GetType(COMMON_QUE_STRMIX)), COMMON_QUE_STRMIX)

		' �C�x���gNo
		txtRecvEvtNo.Text = nEvtNo.ToString
		' ���C��No
		txtRecvLineNo.Text = que_l.nLineNo.ToString

		'Int�f�[�^
		txtRecv_INT0.Text = que_l.data(0).ToString
		txtRecv_INT1.Text = que_l.data(1).ToString
		txtRecv_INT2.Text = que_l.data(2).ToString
		txtRecv_INT3.Text = que_l.data(3).ToString
		txtRecv_INT4.Text = que_l.data(4).ToString
		txtRecv_INT5.Text = que_l.data(5).ToString
		txtRecv_INT6.Text = que_l.data(6).ToString
		txtRecv_INT7.Text = que_l.data(7).ToString
		txtRecv_INT8.Text = que_l.data(8).ToString
		txtRecv_INT9.Text = que_l.data(9).ToString

		'String�f�[�^
		txtRecv_Str.Text = tMod.ByteToString(que_s.data)

		'�����f�[�^
		txtRecv_MIX.Text = tMod.ByteToString(que_m.cdata1)

		'' �I������
		mcls_Log.Write(LNO.MSG, "��M���� EvtNo=" & nEvtNo)
		If Not tmrRecving.Enabled Then
			tmrRecving.Start()
			gpbRecv.BackColor = Color.Pink
		End If
	End Sub

	Private Sub chkTrans_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkTrans.CheckedChanged
		If chkTrans.Checked Then
			txtUdpAddr.Enabled = True
		Else
			txtUdpAddr.Enabled = False
		End If

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
