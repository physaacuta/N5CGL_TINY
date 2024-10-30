Option Strict On								' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)


Imports KizuLibCLI
Imports tClassLibrary.tClass.tClass_LogManager
Imports tClassLibrary
Imports System.Reflection

'''''''''''''''''''''
'Imports system.Runtime.InteropServices					' 属性指定
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
	Private WithEvents mcls_Mod As KizuLibCLI.KizuMod                   ' 総合クラス
	Private mcls_Log As tClassLibrary.tClass.tClass_LogManager			' ログ管理クラス

	Private Delegate Sub deleRecvMail(ByVal pDate As Object, ByVal nEvtNo As Integer)      '別スレッドからのコントロールアクセス


	''' <summary>
	''' フォームロード
	''' </summary>
	Private Sub frmMain_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
		' ※KizuLibCLI 32bit / 64bit 切り替え用
		AddHandler AppDomain.CurrentDomain.AssemblyResolve, New ResolveEventHandler(AddressOf OnAssemblyResolve)

		Task_First()

	End Sub


	''' <summary>
	''' フォーム終了
	''' </summary>
	Private Sub frmMain_FormClosed(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosedEventArgs) Handles Me.FormClosed
		' リスト開放
		mcls_Log.Dispose()
		mcls_Log = Nothing
		' メールクラス解放
		mcls_Mod.Dispose()
		mcls_Mod = Nothing
	End Sub

	'ログファイル出力無しの為 ヤメ
	'''' <summary>
	'''' ログリスト ダブルクリック (メモ帳起動)
	'''' </summary>
	'Private Sub lstMsg_DoubleClick(ByVal sender As Object, ByVal e As System.EventArgs) Handles lstMsg.DoubleClick
	'	mcls_Log.ShowEdit()
	'End Sub

	''' <summary>
	''' タスク初期化時処理
	''' </summary>
	''' <remarks>Main内でKizuLibCLIにアクセスさせないために分離した</remarks>
	Private Sub Task_First()
		mcls_Mod = New KizuLibCLI.KizuMod
		mcls_Log = New tClassLibrary.tClass.tClass_LogManager(Nothing, Nothing, lstMsg)
	End Sub

	''' <summary>
	''' メールスロット 生成
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnMailOpen_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnMailOpen.Click
		If txtRecvTask.Text.Trim.Length = 0 Then
			mcls_Log.Write(LNO.ERR, "生成タスク名無し！")
			Exit Sub
		End If

		' 生成
		If 0 <> mcls_Mod.Task_Init(txtRecvTask.Text, 0) Then
			mcls_Log.Write(LNO.ERR, "生成失敗！")
			Exit Sub
		End If
		mcls_Log.Write(LNO.MSG, "生成完了")
	End Sub

	''' <summary>
	''' メールスロット 開放
	''' </summary>
	Private Sub btnMailClose_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnMailClose.Click
		If Not mcls_Mod Is Nothing Then
			mcls_Mod.Task_Exit()
			mcls_Log.Write(LNO.MSG, "開放完了")
		End If
	End Sub

	''' <summary>
	''' メール受信後の背景色元通り
	''' </summary>
	Private Sub tmrRecving_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles tmrRecving.Tick
		tmrRecving.Enabled = False
		gpbRecv.BackColor = SystemColors.Control
	End Sub


	''' <summary>
	''' 送信ボタンクリック
	''' </summary>
	Private Sub btnSend_Int_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSend_Int.Click, btnSend_Str.Click, btnSend_Mix.Click
		On Error GoTo NG

		'' 入力値チェック
		If 0 = txtSendPC.Text.Trim.Length Then
			mcls_Log.Write(LNO.ERR, "送信PC名無し！")
			Exit Sub
		End If
		If 0 = txtSendTask.Text.Trim.Length Then
			mcls_Log.Write(LNO.ERR, "送信タスク名無し！")
			Exit Sub
		End If
		If False = IsNumeric(txtSendEvtNo.Text.Trim) Then
			mcls_Log.Write(LNO.ERR, "送信イベントNo異常！")
			Exit Sub
		End If
		Dim strPc As String = txtSendPC.Text.Trim
		Dim strTask As String = txtSendTask.Text.Trim
		Dim iEvtNo As Integer = CInt(txtSendEvtNo.Text.Trim)
		Dim iLineNo As Integer = CInt(txtSendLineNo.Text.Trim)
        Dim strUdpAddr As String = txtUdpAddr.Text.Trim

		' 送信データセット
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
                ' メール転送
                If "" <> strUdpAddr Then
                    retc = KizuMod.Send_Mail_Trans(strUdpAddr, strTask, strPc, que)
                Else
                    retc = KizuMod.Send_Mail_Trans(strTask, strPc, que)
                End If

            Else
                ' メール送信
                retc = KizuMod.Send_Mail(strTask, strPc, que)
            End If
            If retc = 0 Then
                mcls_Log.Write(LNO.MSG, "L 送信完了 Evt=" & iEvtNo)
            Else
                mcls_Log.Write(LNO.ERR, "L 送信失敗 Evt=" & iEvtNo & " retc=" & retc)
            End If

        ElseIf 1 = mode Then
            Dim que As New KizuLibCLI.COMMON_QUE_S
            'Dim que As New KizuLibCLI.COMMON_QUE_STR
            que.inisialize()
            que.nEventNo = iEvtNo
            que.nLineNo = iLineNo
            tMod.StringToByte(que.data, txtSend_STR.Text.Trim)
            If chkTrans.Checked Then
                ' メール転送
                If "" <> strUdpAddr Then
                    retc = KizuMod.Send_Mail_Trans(strUdpAddr, strTask, strPc, que)
                Else
                    retc = KizuMod.Send_Mail_Trans(strTask, strPc, que)
                End If
            Else
                ' メール送信
                retc = KizuMod.Send_Mail(strTask, strPc, que)
            End If
            If retc = 0 Then
                mcls_Log.Write(LNO.MSG, "S 送信完了 Evt=" & iEvtNo)
            Else
                mcls_Log.Write(LNO.ERR, "S 送信失敗 Evt=" & iEvtNo & " retc=" & retc)
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
                ' メール転送
                If "" <> strUdpAddr Then
                    retc = KizuMod.Send_Mail_Trans(strUdpAddr, strTask, strPc, que)
                Else
                    retc = KizuMod.Send_Mail_Trans(strTask, strPc, que)
                End If
                retc = KizuMod.Send_Mail_Trans(strTask, strPc, que)
            Else
                ' メール送信
                retc = KizuMod.Send_Mail(strTask, strPc, que)
            End If
            If retc = 0 Then
                mcls_Log.Write(LNO.MSG, "MIX 送信完了 Evt=" & iEvtNo)
            Else
                mcls_Log.Write(LNO.ERR, "MIX 送信失敗 Evt=" & iEvtNo & " retc=" & retc)
            End If

            End If
        Exit Sub
NG:
        mcls_Log.Write(LNO.ERR, "送信時に何かしらのエラー")
	End Sub


	''' <summary>
	''' メール受信イベント (コントロールのメインスレッドとは別スレッドなので注意する事)
	''' </summary>
	''' <param name="pData">受信構造体 (COMMON_QUE_Lの構造体の為、使いたい時に変更してね)</param>
	''' <param name="nEvtNo"></param>
	''' <remarks></remarks>
	Private Sub mcls_Mail_evRecvMail(ByVal pData As Object, ByVal nEvtNo As Integer) Handles mcls_Mod.evRecvMail
		Dim dele As New deleRecvMail(AddressOf subDeleRecvMail)

		'Me.BeginInvoke(dele, New Object() {pData, nEvtNo})			'別スレッドからのアクセス用
		'Me.Invoke(dele, New Object() {pData, nEvtNo})			'別スレッドからのアクセス用
		Me.Invoke(dele, pData, nEvtNo)					'別スレッドからのアクセス用
	End Sub
	Private Sub subDeleRecvMail(ByVal pData As Object, ByVal nEvtNo As Integer)
		' ベースはCOMMON_QUE_Lです
		Dim que_l As COMMON_QUE_L = CType(pData, COMMON_QUE_L)
		Dim que_s As COMMON_QUE_S = CType(mcls_Mod.typConv(pData, GetType(COMMON_QUE_S)), COMMON_QUE_S)
		Dim que_m As COMMON_QUE_MIX = CType(mcls_Mod.typConv(pData, GetType(COMMON_QUE_MIX)), COMMON_QUE_MIX)

'Dim que_sss As COMMON_QUE_STR = CType(mcls_Mod.typConv(pData, GetType(COMMON_QUE_STR)), COMMON_QUE_STR)
'Dim que_mmm As COMMON_QUE_STRMIX = CType(mcls_Mod.typConv(pData, GetType(COMMON_QUE_STRMIX)), COMMON_QUE_STRMIX)

		' イベントNo
		txtRecvEvtNo.Text = nEvtNo.ToString
		' ラインNo
		txtRecvLineNo.Text = que_l.nLineNo.ToString

		'Intデータ
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

		'Stringデータ
		txtRecv_Str.Text = tMod.ByteToString(que_s.data)

		'混合データ
		txtRecv_MIX.Text = tMod.ByteToString(que_m.cdata1)

		'' 終了処理
		mcls_Log.Write(LNO.MSG, "受信完了 EvtNo=" & nEvtNo)
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
	''' KizuLibCLI.DLLの読み込み
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="args"></param>
	''' <returns></returns>
	''' <remarks>32bit/64bit切り替え対応</remarks>
	Private Function OnAssemblyResolve(ByVal sender As Object, ByVal args As System.ResolveEventArgs) As Assembly

		If args.Name.ToUpper.StartsWith("KIZULIBCLI") Then

			If IntPtr.Size = 8 Then
				' 64bit環境
				Return Assembly.LoadFile(IO.Path.Combine(Application.StartupPath, "KizuLibCLIx64.dll"))
			Else
				' 32bit環境
				Return Assembly.LoadFile(IO.Path.Combine(Application.StartupPath, "KizuLibCLIx32.dll"))
			End If

		End If

		Return Nothing

	End Function
End Class
