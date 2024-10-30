'' 非同期の都度接続を行う。受信側がサーバー。送信側がクライアントの２回線方式とする

'Imports tClassLibrary.tClass

Public Class frmChat

	'//***************************************************************
	'// 定数 & 構造体
	'//***************************************************************

	Private Const mcSOCK_SERVER_PORT As Integer = 12346					' サーバーポート
	Private Const mcSOCK_DATA_MAXSIZE As Integer = 1024					' 伝文最大サイズ
	Private mcls_SockRecv As tClass.tClass_Socket.tClass_SocketMgr


	''' <summary>
	''' 送受信用モード (1byteで使用)
	''' </summary>
	''' <remarks></remarks>
	Private Enum EM_SEND_MODE
		EM_SYSLOG = 1				' システムが返信したメッセージ (2byte目以降、シスログ)
		EM_MSG						' チャットメッセージ (2byte目以降、通常のチャットメッセージ)

		EM_FILE_EXEC				' 指定ファイルを実行 (2byte目以降、対象PCでの実行ファイルフルパス)
		EM_COMMAND_EXEC				' 指定コマンドを実行 (2byte目以降、対象PCでの実行するコマンド)
	End Enum



	'//***************************************************************
	'// インスタンス
	'//***************************************************************
	Private m_frmPopup As New frmPopup									' ポップアップ画面


	'//***************************************************************
	'// 設定定数 定義
	'//***************************************************************

	''' <summary>
	''' ソケット生成
	''' </summary>
	''' <remarks></remarks>
	Public Sub Alloc()
		'' 受信用 サーバー定義
		mcls_SockRecv = New tClass.tClass_Socket.tClass_SocketMgr(AddressOf subDele_SOCK_Recv_Event)

		If mcls_SockRecv.SockOpen(Net.Sockets.SocketType.Stream, Net.Sockets.ProtocolType.Tcp) Then
			mcls_SockRecv.Listen(mcSOCK_SERVER_PORT)
			'MsgBox("☆,[clsDensou::Open],受信ポート アクセプト開始")
		Else													'異常
			MsgBox("★,[clsDensou::Open],受信ポート オープン失敗")
			Exit Sub
		End If

	End Sub


	''' <summary>
	''' ソケット解放
	''' </summary>
	''' <remarks></remarks>
	Public Sub Free()
		'' 解放
		If Not mcls_SockRecv Is Nothing Then
			mcls_SockRecv.SockClose()
			mcls_SockRecv = Nothing
		End If

		m_frmPopup.Close()
		m_frmPopup.Dispose()
		m_frmPopup = Nothing
	End Sub



	Public Sub TxtScroll()
		'カレット位置を最後に移動
		Me.txtRecv.SelectionStart = Me.txtRecv.Text.Length
		'テキストボックスにフォーカスを移動
		Me.txtRecv.Focus()
		'カレット位置までスクロール (最後の位置となる。これはテキストボックスにフォーカスが当たってないとダメ)
		Me.txtRecv.ScrollToCaret()
	End Sub


	''' <summary>
	''' フォームが初めて表示される時、
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmChat_Shown(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Shown
		'' ×ボタンを無効にする
		tMod.subFormEndButton_Desebled(Me.Handle)

		'' iniファイルから設定情報取得
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
	''' フォーム非表示
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnClose_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnClose.Click
		Me.Hide()
	End Sub


	''' <summary>
	''' フォームクロージング
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmChat_FormClosing(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles Me.FormClosing
		'' 解放
		If Not mcls_SockRecv Is Nothing Then
			mcls_SockRecv.SockClose()
			mcls_SockRecv = Nothing
		End If
	End Sub


	''' <summary>
	''' アイコンをノーマルに戻すタイマ
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub timIc_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timIc.Tick
		timIc.Stop()
		IconChange(True)
	End Sub

	''' <summary>
	''' 送受信履歴削除
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnClear_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnClear.Click
		txtRecv.Text = ""
	End Sub



	''' <summary>
	''' 自画面が表示された
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmChat_VisibleChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.VisibleChanged
		If Me.Visible Then
			'ポップアップ画面が表示中なら消す
			m_frmPopup.Hide()
		End If
	End Sub

	''' <summary>
	''' エクスプローラー表示
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnExplorer_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnExplorer.Click
		Dim strCommand As String
		Dim wk() As String = cmbSendIp.Text.Split("@"c)

		'エクスプローラ起動()
		strCommand = "/e,"		'フォルダツリーも表示
		'strCommand = "/n,"		'フォルダツリー非表示
		strCommand &= "\\" & wk(0) & "\c$"

		Process.Start("explorer.exe", strCommand)
	End Sub

	''' <summary>
	''' リモートデスクトップ (コンソール)
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
	''' リモートデスクトップ (新コンソール)
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
	''' リモートデスクトップ (ノンコンソール)
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
	'// 送信
	'//***************************************************************
	''' <summary>
	''' 送信データ生成
	''' </summary>
	Function CreateSendByte(ByVal cmd As Integer, ByVal msg As String) As Byte()
		Dim buf() As Byte = System.Text.Encoding.GetEncoding("shift-jis").GetBytes(msg)

		Dim ansbuf(buf.Length) As Byte
		ansbuf(0) = CByte(cmd)

		Array.Copy(buf, 0, ansbuf, 1, buf.Length)
		Return ansbuf
	End Function

	''' <summary>
	''' 送信開始
	''' </summary>
	Private Function SendStart(ByVal sendbuf() As Byte, ByVal sendsize As Integer, ByVal strIp As String) As Boolean
		'' 送信先 生成
		Dim remoteEP As Net.IPEndPoint = Nothing

		'' 送信先IP チェック
		If strIp.Length = 0 Then
			MsgBox("送信先IPが変")
			Return False
		End If
		If strIp.Length < 1 Then
			MsgBox("送信先IPが変")
			Return False
		End If

		Try
			remoteEP = New Net.IPEndPoint(Net.IPAddress.Parse(strIp), mcSOCK_SERVER_PORT)
		Catch ex As Exception
			MsgBox("送信先IPが変")
			Return False
		End Try


		'' 送信
		Try
			Dim sock As New Net.Sockets.Socket(Net.Sockets.AddressFamily.InterNetwork, Net.Sockets.SocketType.Stream, Net.Sockets.ProtocolType.Tcp)
			sock.Connect(remoteEP)
			Dim bytesSent As Integer = sock.Send(sendbuf, sendsize, Net.Sockets.SocketFlags.None)
			sock.Shutdown(Net.Sockets.SocketShutdown.Both)
			sock.Close()
			If bytesSent <> 0 Then
				'受信リストへ表示
				SetRecvText(String.Format("=====送信===== [{0}]", strIp))

				'txtSendMsg.Text = ""
				Return True
			End If
		Catch ex As Exception
			MsgBox("送信失敗")
			Return False
		End Try
	End Function




	''' <summary>
	''' メッセージ送信
	''' </summary>
	Private Sub btnSend_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSend.Click
		If txtSendMsg.Text.Length = 0 Then Return


		'' 送信データ
		Dim sendsize As Integer
		Dim sendbuf() As Byte  '送信バッファ
		sendbuf = CreateSendByte(EM_SEND_MODE.EM_MSG, txtSendMsg.Text)
		sendsize = CInt(IIf(mcSOCK_DATA_MAXSIZE < sendbuf.Length, mcSOCK_DATA_MAXSIZE, sendbuf.Length))

		'' 送信開始
		Dim wk() As String = cmbSendIp.Text.Split("@"c)
		If Not SendStart(sendbuf, sendsize, wk(0)) Then Return

		SetRecvText(txtSendMsg.Text & vbCrLf)
		'txtSendMsg.Text = ""
	End Sub

	''' <summary>
	''' ファイル実行
	''' </summary>
	Private Sub btnExecFile_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnExecFile.Click
		If txtExecItem.Text.Length = 0 Then Return


		'' 送信データ
		Dim sendsize As Integer
		Dim sendbuf() As Byte  '送信バッファ
		sendbuf = CreateSendByte(EM_SEND_MODE.EM_FILE_EXEC, txtExecItem.Text)
		sendsize = CInt(IIf(mcSOCK_DATA_MAXSIZE < sendbuf.Length, mcSOCK_DATA_MAXSIZE, sendbuf.Length))

		'' 送信開始
		Dim wk() As String = cmbSendIp.Text.Split("@"c)
		If Not SendStart(sendbuf, sendsize, wk(0)) Then Return

		SetRecvText(txtExecItem.Text & vbCrLf)
		'txtExecItem.Text = ""
	End Sub
	''' <summary>
	''' コマンド実行
	''' </summary>
	Private Sub btnExecCommand_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnExecCommand.Click
		If txtExecItem.Text.Length = 0 Then Return


		'' 送信データ
		Dim sendsize As Integer
		Dim sendbuf() As Byte  '送信バッファ
		sendbuf = CreateSendByte(EM_SEND_MODE.EM_COMMAND_EXEC, txtExecItem.Text)
		sendsize = CInt(IIf(mcSOCK_DATA_MAXSIZE < sendbuf.Length, mcSOCK_DATA_MAXSIZE, sendbuf.Length))

		'' 送信開始
		Dim wk() As String = cmbSendIp.Text.Split("@"c)
		If Not SendStart(sendbuf, sendsize, wk(0)) Then Return

		SetRecvText(txtExecItem.Text & vbCrLf)
		'txtExecItem.Text = ""
	End Sub


	'//***************************************************************
	'// コントロール非同期アクセス
	'//***************************************************************

	''' <summary>
	''' テキストボックス アクセス
	''' </summary>
	''' <param name="strMsg"></param>
	''' <remarks></remarks>
	Delegate Sub SetTextCallback(ByVal strMsg As String)

	Private Sub SetRecvText(ByVal strMsg As String)
		If Me.InvokeRequired Then
			Dim d As New SetTextCallback(AddressOf SetRecvText)
			Me.Invoke(d, New Object() {strMsg})
		Else
			'テキストボックスに 受信データをセット
			Me.txtRecv.Text &= strMsg & vbCrLf

			'カレット位置までスクロール (最後の位置となる。これはテキストボックスにフォーカスが当たってないとダメ)
			TxtScroll()

			' ''カレット位置を最後に移動
			''Me.txtRecv.SelectionStart = Me.txtRecv.Text.Length
			' ''テキストボックスにフォーカスを移動
			''Me.txtRecv.Focus()
			' ''カレット位置までスクロール (最後の位置となる。これはテキストボックスにフォーカスが当たってないとダメ)
			''Me.txtRecv.ScrollToCaret()
		End If
	End Sub

	''' <summary>
	''' タイマー アクセス
	''' </summary>
	''' <remarks></remarks>
	Delegate Sub SetHideModeCallback(ByVal strMsg As String)

	Private Sub SetHideMode(ByVal strMsg As String)

		If Me.InvokeRequired Then
			Dim d As New SetHideModeCallback(AddressOf SetHideMode)
			Me.Invoke(d, New Object() {strMsg})
		Else
			' アンコン変更タイマー
			Me.timIc.Start()

			' ポップアップフォームの表示
			m_frmPopup.g_strMsg = strMsg
			m_frmPopup.Show()
		End If
	End Sub

	''' <summary>
	''' 送信先 アクセス
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
	' 概要：TCP/IP コールバック関数
	' 引数：(in :Object) sender = tClass.tClass_Socket_State_*** クラス
	' 引数：(in :tEnum_Sock_EventNO) e = イベントNO
	' 戻値：なし
	' 備考：受信ポート用 ソケット
	'----------------------------------------------------------------
	Private Sub subDele_SOCK_Recv_Event(ByVal sender As Object, ByVal e As tClass.tClass_Socket.tClass_Socket_Base.tEnum_Sock_EventNO)
		Dim iRetc As Integer = 0
		'' イベントNOで処理分け
		Select Case e
			Case tClass.tClass_Socket.tClass_Socket_Base.tEnum_Sock_EventNO.ACCEPT			'' ACCEPT接続完了
				Dim state As tClass.tClass_Socket.tClass_Socket_State_Accept = CType(sender, tClass.tClass_Socket.tClass_Socket_State_Accept)
				'受信待ち
				state.clsSock.Recv(mcSOCK_DATA_MAXSIZE)				'受信開始


			Case tClass.tClass_Socket.tClass_Socket_Base.tEnum_Sock_EventNO.RECV			'' 受信完了
				Dim state As tClass.tClass_Socket.tClass_Socket_State_Recv = CType(sender, tClass.tClass_Socket.tClass_Socket_State_Recv)
				'MsgBox("送信ポート 受信,[" & System.Text.Encoding.Unicode.GetString(state.buffer, 0, state.nowRecvSize) & "]")


				' モード判定
				Dim strMsg As String = ""
				Dim strSubMsg As String = ""
				Dim bMode As Byte = state.buffer(0)

				If EM_SEND_MODE.EM_SYSLOG = bMode Then	' シスログ
					strMsg = System.Text.Encoding.GetEncoding("shift-jis").GetString(state.buffer, 1, state.nowRecvSize - 1)
					strSubMsg = "シスログ"

				ElseIf EM_SEND_MODE.EM_MSG = bMode Then	' チャットメッセージ
					strMsg = System.Text.Encoding.GetEncoding("shift-jis").GetString(state.buffer, 1, state.nowRecvSize - 1)

					'' フォームが非表示時のみ アイコンを変更
					If Not Me.Visible Then
						IconChange(False)
						' アイコン元に戻すタイマーセット
						SetHideMode(strMsg)
					End If

				ElseIf EM_SEND_MODE.EM_FILE_EXEC = bMode Then	' 指定ファイルを実行
					strMsg = System.Text.Encoding.GetEncoding("shift-jis").GetString(state.buffer, 1, state.nowRecvSize - 1)
					strSubMsg = "指定ファイル実行"

					Call ExecLocalPc(EM_SEND_MODE.EM_FILE_EXEC, strMsg, state.clsSock.tProp_ToIP)

				ElseIf EM_SEND_MODE.EM_COMMAND_EXEC = bMode Then	' 指定コマンドを実行
					strMsg = System.Text.Encoding.GetEncoding("shift-jis").GetString(state.buffer, 1, state.nowRecvSize - 1)
					strSubMsg = "指定コマンド実行"

					Call ExecLocalPc(EM_SEND_MODE.EM_COMMAND_EXEC, strMsg, state.clsSock.tProp_ToIP)

				Else							' 旧バージョン、未知コマンド
					strMsg = System.Text.Encoding.GetEncoding("shift-jis").GetString(state.buffer, 0, state.nowRecvSize)
					strSubMsg = "旧バージョンからの受信"

					'' フォームが非表示時のみ アイコンを変更
					If Not Me.Visible Then
						IconChange(False)
						' アイコン元に戻すタイマーセット
						SetHideMode(strMsg)
					End If
				End If



				'' 受信データを表示
				SetRecvText(String.Format("=====受信 {0}===== [{1}]", strSubMsg, state.clsSock.tProp_ToIP))
				SetRecvText(strMsg & vbCrLf)
				'' 受信元のIPを送信先にセット
				SetSendIp(state.clsSock.tProp_ToIP)


			Case tClass.tClass_Socket.tClass_Socket_Base.tEnum_Sock_EventNO.CLOSE_SOCK		'' 切断検知
				Dim state As tClass.tClass_Socket.tClass_Socket_State_Close = CType(sender, tClass.tClass_Socket.tClass_Socket_State_Close)

			Case Else
				'' ありえない
				'MsgBox(String.Format("★,[clsDensou::？？？],イベントNo 異常[{0}]", e))
		End Select
	End Sub


	''' <summary>
	''' ローカルPCで実行
	''' </summary>
	''' <param name="emMode"></param>
	''' <param name="strVal"></param>
	''' <remarks></remarks>
	Private Sub ExecLocalPc(ByVal emMode As EM_SEND_MODE, ByVal strVal As String, ByVal sendPcIp As String)
		Dim strAns As String = String.Format("成功 [{0}]", strVal)   ' 結果

		Try
			If emMode = EM_SEND_MODE.EM_FILE_EXEC Then
				' ファイルがある？
				If 0 = Dir(strVal).Length Then
					strAns = "該当ファイル無し"
				Else
					'Process.Start(strVal)
					Shell(strVal)
				End If

			ElseIf emMode = EM_SEND_MODE.EM_COMMAND_EXEC Then
				Shell("cmd /c " & strVal, AppWinStyle.Hide)
			End If

		Catch ex As Exception
			strAns = "失敗・・・" & ex.Message
		End Try



		'' 送信データ
		Dim sendsize As Integer
		Dim sendbuf() As Byte  '送信バッファ
		sendbuf = CreateSendByte(EM_SEND_MODE.EM_SYSLOG, strAns)
		sendsize = CInt(IIf(mcSOCK_DATA_MAXSIZE < sendbuf.Length, mcSOCK_DATA_MAXSIZE, sendbuf.Length))

		'' 送信開始
		If Not SendStart(sendbuf, sendsize, sendPcIp) Then Return
		SetRecvText(strAns & vbCrLf)
	End Sub




End Class