'*********************************************************************************
' PC 再起動等 支援クラス
'	[Ver]
'		Ver.01    2007/04/26  新規作成
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports System.Runtime.InteropServices

Module modMain

	Public Const m_ini As String = ".\t_PC.ini"

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ローカル変数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private Enum mc_nPC
		nMy = 0			' 自PC
		nAll			' 全PC
		nHantei			' 判定PC
		nSelect			' 指定PC
	End Enum
	Private Enum mc_nKind
		nLogoff = 0		' ログオフ (リモート含め全部)
		nUnForceLogoff	' リモートユーザーのみログオフ
		nDown			' シャットダウン
        nReboot         ' 再起動
        nConsole        ' コンソール側に切り替え
	End Enum


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 参照できない定義
	'/ /////////////////////////////////////////////////////////////////////////////////
	<StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Size:=SIZE_MSL, Pack:=1)> _
	Public Structure COMMON_QUE_STRMIX
		Dim nEventNo As Integer
		Dim nLineNo As Integer

		<VBFixedArray(20 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=20)> _
		Dim idata() As Integer

		'// ↓ByValTStr これだと 終端に勝手にNULLが付与される為、終端1バイトが削除される。
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
	'/ ローカル変数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private WithEvents mcls_Mail As tClass.tClass_MailSlot				' メールスロット受信クラス
	Private m_RecvQueBuf As COMMON_QUE_STRMIX							' メールスロット受信バッファ

	Private m_ap As ApplicationContext									' コンテキスト

	Private WithEvents m_nfyIcon As System.Windows.Forms.NotifyIcon		' タスクトレイのアイコン コントロール
	Private m_cPcName As String											' 自PC名称

	Private m_frmChat As frmChat										' チャット用フォーム
	Private m_icNomal As Icon											' 通常アイコン
	Private m_icRecv As Icon											' メール受信時のアイコン




	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 定義
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' エントリーポイント
	''' </summary>
	''' <remarks></remarks>
	Public Sub main()

        '' タスクイニシャル
        Try
            mcls_Mail = New tClass.tClass_MailSlot("t_PC", 300)
            mcls_Mail.Mail_ReadStart(m_RecvQueBuf, AddressOf tDele_GetMail)
        Catch ex As Exception
            'MsgBox("Init失敗")
            End
        End Try


		'' もろもろ初期化
		m_cPcName = System.Windows.Forms.SystemInformation.ComputerName()		' コンピューター名取得

		'' タスクトレイのアイコン取得
		Try															' アイコン設定
			'' 埋め込まれたリソースからアイコンを取り出し
			Dim MyAsm As Reflection.Assembly = Reflection.Assembly.GetExecutingAssembly()
			m_icNomal = New Icon(MyAsm.GetManifestResourceStream("t_PC.t_PC.ico"))
			m_icRecv = New Icon(MyAsm.GetManifestResourceStream("t_PC.t_PCchatrecv.ico"))


		Catch ex As Exception
		End Try


		'' タスクトレイのアイコン コントロール インスタンス生成
		m_nfyIcon = New System.Windows.Forms.NotifyIcon
		m_nfyIcon.ContextMenu = CreateNotifyIconMenu_Def()		'タスクトレイのアイコンメニュー定義
		m_nfyIcon.Text = "PC動作制御"
		IconChange(True)	' 通常アイコンセット
		m_nfyIcon.Visible = True






		'' チャット用フォーム生成
		m_frmChat = New frmChat
		m_frmChat.Location = New Point(My.Computer.Screen.WorkingArea.Width - m_frmChat.Size.Width, My.Computer.Screen.WorkingArea.Height - m_frmChat.Size.Height)
		m_frmChat.Alloc()		'ソケット生成

		m_frmChat.Opacity = 0	' 表示するときに 透明化する事で フォーム起動の一瞬のちらつきを防止
		m_frmChat.Show()
		m_frmChat.Hide()
		m_frmChat.Opacity = 1

		'' メインスレッド生成
		m_ap = New ApplicationContext
		Application.Run(m_ap)			'ExitThreadがコールされるまで ここでブロック

		'' 終了
		m_frmChat.Free()		'ソケット解放
		m_frmChat.Dispose()

		m_nfyIcon.Dispose()
		' ''mcls_Mod.Task_Exit()
		mcls_Mail.Mail_ReadStop()
		mcls_Mail.Dispose()
		mcls_Mail = Nothing
	End Sub

	''' <summary>
	''' タスクトレイのアイコンメニュー定義
	''' </summary>
	''' <returns></returns>
	''' <remarks></remarks>
	Private Function CreateNotifyIconMenu_Def() As ContextMenu

        Dim closeItem() As MenuItem = { _
         New MenuItem("自PCログオフ", AddressOf m_menuItem_Click), _
         New MenuItem("自PC(裏のみ)ﾛｸﾞｵﾌ", AddressOf m_menuItem_Click), _
         New MenuItem("自PCシャットダウン", AddressOf m_menuItem_Click), _
         New MenuItem("自PC再起動", AddressOf m_menuItem_Click), _
         New MenuItem("[常駐をｺﾝｿｰﾙ側に]", AddressOf m_menuItem_Click), _
         New MenuItem("全PCログオフ", AddressOf m_menuItem_Click), _
         New MenuItem("全PC(裏のみ)ﾛｸﾞｵﾌ", AddressOf m_menuItem_Click), _
         New MenuItem("全PCシャットダウン", AddressOf m_menuItem_Click), _
         New MenuItem("全PC再起動", AddressOf m_menuItem_Click), _
         New MenuItem("[常駐をｺﾝｿｰﾙ側に]", AddressOf m_menuItem_Click), _
         New MenuItem("判定PCログオフ", AddressOf m_menuItem_Click), _
         New MenuItem("判定PC(裏のみ)ﾛｸﾞｵﾌ", AddressOf m_menuItem_Click), _
         New MenuItem("判定PCシャットダウン", AddressOf m_menuItem_Click), _
         New MenuItem("判定PC再起動", AddressOf m_menuItem_Click), _
         New MenuItem("[常駐をｺﾝｿｰﾙ側に]", AddressOf m_menuItem_Click), _
         New MenuItem("指定PCログオフ", AddressOf m_menuItem_Click), _
         New MenuItem("指定PC(裏のみ)ﾛｸﾞｵﾌ", AddressOf m_menuItem_Click), _
         New MenuItem("指定PCシャットダウン", AddressOf m_menuItem_Click), _
         New MenuItem("指定PC再起動", AddressOf m_menuItem_Click), _
         New MenuItem("[常駐をｺﾝｿｰﾙ側に]", AddressOf m_menuItem_Click), _
         New MenuItem("全ツール終了", AddressOf m_menuItem_Click), _
         New MenuItem("ツール終了[" & m_cPcName & "]", AddressOf m_menuItem_Click) _
         }

		'' 改行設定
        closeItem(5).BarBreak = True
        closeItem(10).BarBreak = True
        closeItem(15).BarBreak = True
        closeItem(20).BarBreak = True

		'' コンテキストメニュー定義
		Dim menu As New ContextMenu
		menu.MenuItems.AddRange(closeItem)

		Return menu
	End Function

	''' <summary>
	''' タスクトレイのアイコンメニュークリックイベント
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub m_menuItem_Click(ByVal sender As Object, ByVal e As System.EventArgs)
		' 送信キュー
		Dim que As COMMON_QUE_STRMIX = Nothing
		que.inisialize()
		que.nEventNo = 9800

		' メニューにより処理分け
		Dim menu As MenuItem = CType(sender, MenuItem)
		Select Case menu.Index
			Case 0									' 自PCログオフ
				que.idata(0) = mc_nPC.nMy
				que.idata(1) = mc_nKind.nLogoff
                mcls_Mail.Mail_Send(".", "t_PC", que)
			Case 1									' 自PCログオフ(裏のみ)
				que.idata(0) = mc_nPC.nMy
				que.idata(1) = mc_nKind.nUnForceLogoff
                mcls_Mail.Mail_Send(".", "t_PC", que)
			Case 2									' 自PCシャットダウン
				que.idata(0) = mc_nPC.nMy
				que.idata(1) = mc_nKind.nDown
                mcls_Mail.Mail_Send(".", "t_PC", que)
			Case 3									' 自PC再起動
				que.idata(0) = mc_nPC.nMy
				que.idata(1) = mc_nKind.nReboot
                mcls_Mail.Mail_Send(".", "t_PC", que)
            Case 4                                  ' コンソール
                que.idata(0) = mc_nPC.nMy
                que.idata(1) = mc_nKind.nConsole
                mcls_Mail.Mail_Send(".", "t_PC", que)


            Case 5                                  ' 全PCログオフ
                If MsgBox("全PCログオフしますか？", MsgBoxStyle.OkCancel, "確認") <> MsgBoxResult.Ok Then Return
                que.idata(0) = mc_nPC.nAll
                que.idata(1) = mc_nKind.nLogoff
                mcls_Mail.Mail_Send("*", "t_PC", que)
            Case 6                                  ' 全PCログオフ(裏のみ)
                If MsgBox("全PC(ｺﾝｿｰﾙ以外)ログオフしますか？", MsgBoxStyle.OkCancel, "確認") <> MsgBoxResult.Ok Then Return
                que.idata(0) = mc_nPC.nAll
                que.idata(1) = mc_nKind.nUnForceLogoff
                mcls_Mail.Mail_Send("*", "t_PC", que)
            Case 7                                  ' 全PCシャットダウン
                If MsgBox("全PCシャットダウンしますか？", MsgBoxStyle.OkCancel, "確認") <> MsgBoxResult.Ok Then Return
                que.idata(0) = mc_nPC.nAll
                que.idata(1) = mc_nKind.nDown
                mcls_Mail.Mail_Send("*", "t_PC", que)
            Case 8                                  ' 全PC再起動
                If MsgBox("全PC再起動しますか？", MsgBoxStyle.OkCancel, "確認") <> MsgBoxResult.Ok Then Return
                que.idata(0) = mc_nPC.nAll
                que.idata(1) = mc_nKind.nReboot
                mcls_Mail.Mail_Send("*", "t_PC", que)
            Case 9                                  ' 
                If MsgBox("全PC常駐ﾀｽｸをｺﾝｿｰﾙ側にしますか？", MsgBoxStyle.OkCancel, "確認") <> MsgBoxResult.Ok Then Return
                que.idata(0) = mc_nPC.nAll
                que.idata(1) = mc_nKind.nConsole
                mcls_Mail.Mail_Send("*", "t_PC", que)


            Case 10                             ' 判定PCログオフ
                If MsgBox("全判定PCログオフしますか？", MsgBoxStyle.OkCancel, "確認") <> MsgBoxResult.Ok Then Return
                que.idata(0) = mc_nPC.nHantei
                que.idata(1) = mc_nKind.nLogoff
                mcls_Mail.Mail_Send("*", "t_PC", que)
            Case 11                             ' 判定PCログオフ(裏のみ)
                If MsgBox("全判定PC(ｺﾝｿｰﾙ以外)ログオフしますか？", MsgBoxStyle.OkCancel, "確認") <> MsgBoxResult.Ok Then Return
                que.idata(0) = mc_nPC.nHantei
                que.idata(1) = mc_nKind.nUnForceLogoff
                mcls_Mail.Mail_Send("*", "t_PC", que)
            Case 12                                 ' 判定PCシャットダウン
                If MsgBox("全判定PCシャットダウンしますか？", MsgBoxStyle.OkCancel, "確認") <> MsgBoxResult.Ok Then Return
                que.idata(0) = mc_nPC.nHantei
                que.idata(1) = mc_nKind.nDown
                mcls_Mail.Mail_Send("*", "t_PC", que)
            Case 13                                 ' 判定PC再起動
                If MsgBox("全判定PC再起動しますか？", MsgBoxStyle.OkCancel, "確認") <> MsgBoxResult.Ok Then Return
                que.idata(0) = mc_nPC.nHantei
                que.idata(1) = mc_nKind.nReboot
                mcls_Mail.Mail_Send("*", "t_PC", que)
            Case 14                                 ' 判定PC再起動
                If MsgBox("全判定常駐ﾀｽｸをｺﾝｿｰﾙ側にしますか？", MsgBoxStyle.OkCancel, "確認") <> MsgBoxResult.Ok Then Return
                que.idata(0) = mc_nPC.nHantei
                que.idata(1) = mc_nKind.nConsole
                mcls_Mail.Mail_Send("*", "t_PC", que)



            Case 15                                 ' 指定PCログオフ
                Dim strName As String = InputBox("指定PC名称入力要求" & vbCrLf & "  (入力文字の先頭から一致する全PCが対象となる。大文字小文字関係ないよ)", "PC名称指定")
                If strName.Length = 0 Then Exit Sub
                que.idata(0) = mc_nPC.nSelect
                que.idata(1) = mc_nKind.nLogoff
                que.cdata1 = strName
                mcls_Mail.Mail_Send("*", "t_PC", que)
            Case 16                                 ' 指定PCログオフ
                Dim strName As String = InputBox("指定PC名称入力要求" & vbCrLf & "  (入力文字の先頭から一致する全PCが対象となる。大文字小文字関係ないよ)", "PC名称指定")
                If strName.Length = 0 Then Exit Sub
                que.idata(0) = mc_nPC.nSelect
                que.idata(1) = mc_nKind.nUnForceLogoff
                que.cdata1 = strName
                mcls_Mail.Mail_Send("*", "t_PC", que)
            Case 17                                 ' 指定PCシャットダウン
                Dim strName As String = InputBox("指定PC名称入力要求" & vbCrLf & "  (入力文字の先頭から一致する全PCが対象となる。大文字小文字関係ないよ)", "PC名称指定")
                If strName.Length = 0 Then Exit Sub
                que.idata(0) = mc_nPC.nSelect
                que.idata(1) = mc_nKind.nDown
                que.cdata1 = strName
                mcls_Mail.Mail_Send("*", "t_PC", que)
            Case 18                                 ' 指定PC再起動
                Dim strName As String = InputBox("指定PC名称入力要求" & vbCrLf & "  (入力文字の先頭から一致する全PCが対象となる。大文字小文字関係ないよ)", "PC名称指定")
                If strName.Length = 0 Then Exit Sub
                que.idata(0) = mc_nPC.nSelect
                que.idata(1) = mc_nKind.nReboot
                que.cdata1 = strName
                mcls_Mail.Mail_Send("*", "t_PC", que)
            Case 19
                Dim strName As String = InputBox("指定PC名称入力要求" & vbCrLf & "  (入力文字の先頭から一致する全PCが対象となる。大文字小文字関係ないよ)", "PC名称指定")
                If strName.Length = 0 Then Exit Sub
                que.idata(0) = mc_nPC.nSelect
                que.idata(1) = mc_nKind.nConsole
                que.cdata1 = strName
                mcls_Mail.Mail_Send("*", "t_PC", que)


            Case 20                                 ' 全ツール終了
                que.nEventNo = FACT_KS_END
                mcls_Mail.Mail_Send("*", "t_PC", que)
            Case 21                                 ' ツール終了
                que.nEventNo = FACT_KS_END
                mcls_Mail.Mail_Send(".", "t_PC", que)
        End Select
	End Sub

	''' <summary>
	''' メールスロット受信
	''' </summary>
    Public Sub tDele_GetMail(ByVal recvVal As System.ValueType)
        Dim que As COMMON_QUE_STRMIX = CType(recvVal, COMMON_QUE_STRMIX)
        Dim nEvtNo As Integer = que.nEventNo

        Select Case nEvtNo
            Case 9800
                ' このメールは自分が対象？
                If que.idata(0) = mc_nPC.nMy Then                       '自PC
                ElseIf que.idata(0) = mc_nPC.nAll Then                  '全PC
                ElseIf que.idata(0) = mc_nPC.nHantei Then               '判定PC
                    If Not m_cPcName.StartsWith("hantei", StringComparison.CurrentCultureIgnoreCase) Then
                        Return
                    End If
                ElseIf que.idata(0) = mc_nPC.nSelect Then               ' 指定PC
                    If Not m_cPcName.StartsWith(que.cdata1, StringComparison.CurrentCultureIgnoreCase) Then
                        Return
                    End If
                Else
                    'NG
                    Return
                End If

                '''' ここまで来たら対象の場合しかありえない
                Dim bMyTaskEnd As Boolean = True
                ' 処理わけ
                If que.idata(1) = mc_nKind.nLogoff Then             ' ログオフ
                    ' まずその他セッションを。
                    AllUserLogOff.Exec()

                    tClass.tClass_Shutdown.ExitWindows(tClass.tClass_Shutdown.ShutdownFlag.LogOff Or tClass.tClass_Shutdown.ShutdownFlag.Force Or tClass.tClass_Shutdown.ShutdownFlag.ForceIfHung)

                ElseIf que.idata(1) = mc_nKind.nUnForceLogoff Then        ' ログオフ（コンソール以外）
                    AllUserLogOff.Exec()
                    bMyTaskEnd = False

                ElseIf que.idata(1) = mc_nKind.nDown Then           ' シャットダウン
                    tClass.tClass_Shutdown.ExitWindows(tClass.tClass_Shutdown.ShutdownFlag.Shutdown Or tClass.tClass_Shutdown.ShutdownFlag.Force Or tClass.tClass_Shutdown.ShutdownFlag.ForceIfHung)
                ElseIf que.idata(1) = mc_nKind.nReboot Then         ' 再起動
                    tClass.tClass_Shutdown.ExitWindows(tClass.tClass_Shutdown.ShutdownFlag.Reboot Or tClass.tClass_Shutdown.ShutdownFlag.Force Or tClass.tClass_Shutdown.ShutdownFlag.ForceIfHung)
                ElseIf que.idata(1) = mc_nKind.nConsole Then         ' コンソール側
                    AllUserLogOff.Change()
                    bMyTaskEnd = False
                End If

                ' 自タスクをどうするか？
                If bMyTaskEnd Then
                    m_ap.ExitThread()
                End If

            Case FACT_KS_END                            ' タスク終了
                m_ap.ExitThread()                                       ' メインスレッド終了
        End Select

    End Sub







	''' <summary>
	''' タスクトレイのアイコン変更
	''' </summary>
	''' <param name="bNomal">true:ノーマル  false:チャット受信</param>
	''' <remarks></remarks>
	Public Sub IconChange(ByVal bNomal As Boolean)
		If bNomal Then
			m_nfyIcon.Icon = m_icNomal
		Else
			m_nfyIcon.Icon = m_icRecv
		End If
	End Sub

	''' <summary>
	''' タスクトレイアイコン ダブルクリック
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub nfyIcon_DoubleClick(ByVal sender As Object, ByVal e As System.EventArgs) Handles m_nfyIcon.DoubleClick
		'' アイコンを元に戻す
		IconChange(True)

		'' フォーム表示中なら消す
		If m_frmChat.Visible Then
			m_frmChat.Hide()
		Else
			'' フォーム表示
			m_frmChat.Show()
			m_frmChat.TopMost = True	'最初のみ最前面に出す
			m_frmChat.TopMost = False
			'カレット位置までスクロール (最後の位置となる。これはテキストボックスにフォーカスが当たってないとダメ)
			m_frmChat.TxtScroll()
		End If
	End Sub

End Module