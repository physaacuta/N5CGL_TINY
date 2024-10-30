'●レジストリ構成は、以下の通りとする。(全疵検共通)
'・HKEY_LOCAL_MACHINE / SOFTWARE / KizuSystem / BasePath = 全システムのインストールフォルダフルパスパス
'                                             / システム名 / EXE_1～
'                                                            ↑ 値は、ショートカットのフルパス
'●スタートメニュー構成は、
'・kizuSystem / システムフォルダ / 各EXEのショートカット


Imports Microsoft.Win32
Imports tClassLibrary.tMod


Public Class frmMain

	'********************************************************************
	' ローカル変数
	'********************************************************************
	'''' 共通項目
	Private Const mcBaseName As String = "KizuSystem"
	Private Const mcINIFILE As String = "KizuInstal.ini"
	Private mcDefaultPath As String = "C:\" & mcBaseName

	'' レジストリ関係
	Private Const mcRegSystemKey As String = "Software\" & mcBaseName
	Private Const mcRegBasePath As String = "BasePath"


	'' 最小情報単位
	Private Class typInfo
		Public strName As String							' 名称 (iniファイルKeyの1個目)
		Public strType As String							' 略称 (iniファイルKeyの2個目)
		Public bInstal As Boolean							' インストール状態

		''' <summary>
		''' データのセット
		''' </summary>
		''' <param name="strVal">iniファイルキー情報</param>
		Public Sub New(ByVal strVal As String)
			Dim strAry() As String = strVal.Split(","c)
			If 2 = strAry.Length Then
				strName = strAry(0).Trim(" "c)
				strType = strAry(1).Trim(" "c)
			End If
		End Sub
	End Class

	'' 1システムの情報
	Private Class typSystemInf
		Public typSys As typInfo							' システム情報
		Public lstExe As List(Of typInfo)					' EXE情報
	End Class

	'' モード設定
	Private Enum EM_INSTALMODE
		install = 0											' インストール
		update = 1											' アップデート
		delete = 2											' アンインストール
	End Enum


	'''''' ローカル変数
	Private m_lstNowInf As New List(Of typSystemInf)		' 現在のインストール情報
	Private m_lstSetInf As New List(Of typSystemInf)		' 現在の設定情報
	Private m_FastInstal As Boolean							' 初めてのインストール時true


	'********************************************************************
	' 一般処理
	'********************************************************************
	''' <summary>
	''' フォームロード
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmMain_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
		'' iniファイル情報取得
		ReadIni(m_lstNowInf)
		ReadIni(m_lstSetInf)

		'' 画面用データ再読込
		RefreshDisp(EM_INSTALMODE.install)
	End Sub

	''' <summary>
	''' レジストリキー状態を見て インストールベースフォルダ状態変更を行う
	''' </summary>
	''' <param name="regkey"></param>
	''' <remarks></remarks>
	Private Sub EnableSelectFolder(ByVal regkey As RegistryKey)
		Try
			If regkey Is Nothing Then			'' まだインストールされた事が無い
				'ベースフォルダ初期値セット
				txtNowInstalFolder.Text = ""
				txtSetInstalFolder.Text = mcDefaultPath

				m_FastInstal = True
				SetMsgBox(True, "新規インストールです。" & vbCrLf & "Dドライブがある場合は、出来るだけD:\KizuSystemにインストールにして下さい。")

			Else								'' インストール済み
				'ベースフォルダは変更禁止
				txtNowInstalFolder.Text = regkey.GetValue(mcRegBasePath).ToString
				txtSetInstalFolder.Text = regkey.GetValue(mcRegBasePath).ToString

				m_FastInstal = False
				SetMsgBox(True, "[" & txtSetInstalFolder.Text & "] に疵検査システムのインストールフォルダを発見しました。" & vbCrLf & "以降、フォルダを変更する事は、出来ません。")
			End If
		Catch ex As Exception

		End Try

	End Sub


	''' <summary>
	''' iniファイル情報取得して、基本となるシステム情報を生成
	''' </summary>
	''' <param name="lst">セットする構造体</param>
	''' <remarks></remarks>
	Private Sub ReadIni(ByRef lst As List(Of typSystemInf))
		Dim strVal As String
		Dim strSection As String

		Dim sysid As Integer = 1
		Dim exeid As Integer

		'' iniファイルフルパス取得
		Dim strIniPath As String = IO.Path.Combine(System.Windows.Forms.Application.StartupPath, mcINIFILE)


		'' システム情報取得
		sysid = 1
		While (True)
			' システム情報有り？
			strSection = "ID_" & sysid
			strVal = tMod_IniFile.ReadIniStr(strSection, "Name", strIniPath)
			If 0 = strVal.Length Then Exit While 'もう無い？

			Dim sysinf As typSystemInf = New typSystemInf
			sysinf.lstExe = New List(Of typInfo)

			'システム情報生成
			sysinf.typSys = New typInfo(strVal)
			lst.Add(sysinf)

			'' EXE情報
			exeid = 1
			While True
				' EXE情報有り？
				strVal = tMod_IniFile.ReadIniStr(strSection, "EXE_" & exeid, strIniPath)
				If 0 = strVal.Length Then Exit While 'もう無い？

				'EXE情報
				Dim exeinf As New typInfo(strVal)
				sysinf.lstExe.Add(exeinf)

				exeid += 1
			End While

			sysid += 1
		End While
	End Sub

	''' <summary>
	''' レジストリの登録状態で、インストール状態を把握する
	''' </summary>
	''' <param name="regkey">レジストリキー</param>
	''' <param name="lst"></param>
	''' <remarks></remarks>
	Private Sub ReadInstalReg(ByVal regkey As RegistryKey, ByRef lst As List(Of typSystemInf))
		Try
			Dim syskey As RegistryKey		'各システムのレジストリ状態

			'''' 初期化
			For ii As Integer = 0 To lst.Count - 1
				lst(ii).typSys.bInstal = False
				For jj As Integer = 0 To lst(ii).lstExe.Count - 1
					lst(ii).lstExe(jj).bInstal = False
				Next jj
			Next ii

			'''' レジストリデータ無し
			If regkey Is Nothing Then Return


			'''' レジストリデータあり
			For ii As Integer = 0 To lst.Count - 1
				'''' システムのキーがある？
				syskey = regkey.OpenSubKey(lst(ii).typSys.strType, False)
				If syskey Is Nothing Then
					lst(ii).typSys.bInstal = False
					Continue For
				Else
					lst(ii).typSys.bInstal = True
				End If


				'''' 登録EXEがある？
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
	''' ツリービューにアイテム追加
	''' </summary>
	''' <param name="trv">追加するコントロール</param>
	''' <param name="lst">追加するアイテム</param>
	''' <param name="nAddMode">追加動作 (0:全部 1:ONの奴のみ 2:OFFの奴のみ)</param>
	''' <param name="bSysOnly">追加動作 (true:子ノード無し)</param>	
	''' <remarks></remarks>
	Private Sub SetTreeViewItem(ByVal trv As TreeView, ByVal lst As List(Of typSystemInf), ByVal nAddMode As Integer, ByVal bSysOnly As Boolean)

		Dim nodeBase As TreeNode
		Dim nodeKo As TreeNode
		With trv
			.Nodes.Clear()

			For ii As Integer = 0 To lst.Count - 1
				If 1 = nAddMode AndAlso Not lst(ii).typSys.bInstal Then Continue For
				'If 2 = nAddMode AndAlso lst(ii).typSys.bInstal Then Continue For		' 子ノードの追加時にエラーになる為

				'システム情報 (親ノード)
				nodeBase = New TreeNode()
				nodeBase.Text = lst(ii).typSys.strName & "    ･･･ [" & lst(ii).typSys.strType & "]"
				nodeBase.Tag = lst(ii).typSys
				.Nodes.Add(nodeBase)

				If Not bSysOnly Then
					For jj As Integer = 0 To lst(ii).lstExe.Count - 1
						If 1 = nAddMode AndAlso Not lst(ii).lstExe(jj).bInstal Then Continue For
						If 2 = nAddMode AndAlso lst(ii).lstExe(jj).bInstal Then Continue For

						'EXE情報 (子ノード)
						nodeKo = New TreeNode()
						nodeKo.Text = lst(ii).lstExe(jj).strName
						nodeKo.Tag = lst(ii).lstExe(jj)
						nodeBase.Nodes.Add(nodeKo)
					Next jj
				End If

				' 子ノードを展開
				.Nodes(.Nodes.Count - 1).Expand()
			Next ii
		End With
	End Sub


	''' <summary>
	''' 画面用データ再読込
	''' </summary>
	''' <remarks></remarks>
	Private Sub RefreshDisp(ByVal emInstall As EM_INSTALMODE)
		Try
			Dim regkey As RegistryKey = My.Computer.Registry.LocalMachine.OpenSubKey(mcRegSystemKey)

			'' インストール状態により初期値セット
			EnableSelectFolder(regkey)

			'''' 現在のシステム状態再読込
			ReadInstalReg(regkey, m_lstNowInf)
			ReadInstalReg(regkey, m_lstSetInf)
			SetTreeViewItem(trvNowInf, m_lstNowInf, 1, False)	   ' インストール済みの項目のみ
			SetMode(emInstall)

			If Not regkey Is Nothing Then
				regkey.Close()
			End If
		Catch ex As Exception

		End Try
	End Sub


	''' <summary>
	''' ノードクリック
	''' </summary>
	Private Sub trvSetInf_NodeMouseClick(ByVal sender As Object, ByVal e As System.Windows.Forms.TreeNodeMouseClickEventArgs) Handles trvSetInf.NodeMouseClick
		'' 自分の子ノードのチェック状態を変更する
		For ii As Integer = 0 To e.Node.Nodes.Count - 1
			e.Node.Nodes(ii).Checked = e.Node.Checked
		Next ii

		'' 自分の親ノードをチェック状態に変更する
		If e.Node.Checked Then
			If Not e.Node.Parent Is Nothing Then
				e.Node.Parent.Checked = True
			End If
		End If
	End Sub


	''' <summary>
	''' ベースフォルダ選択
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnSelectFolder_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSelectFolder.Click, Button1.Click
		Dim dlg As New FolderBrowserDialog
		dlg.ShowNewFolderButton = True
		' デフォルト選択パス
		Dim path As String = txtSetInstalFolder.Text
		If IO.Directory.Exists(path) Then	' フォルダ有り
			dlg.SelectedPath = path
		Else									' フォルダ無し
			dlg.SelectedPath = IO.Path.GetDirectoryName(path)
		End If

		' フォルダ選択
		dlg.ShowDialog()

		' 選択フォルダ取得
		txtSetInstalFolder.Text = dlg.SelectedPath
	End Sub


	'// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
	'// インストール関連

	''' <summary>
	''' コマンド実行
	''' </summary>
	Private Sub btnExecute_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnExecute.Click
		'''' 事前チェック
		Dim strMsg As String
		If rdoInstal1.Checked Then
			strMsg = rdoInstal1.Text
		ElseIf rdoInstal2.Checked Then
			strMsg = rdoInstal2.Text
		Else
			strMsg = rdoInstal3.Text
		End If
		Dim retc As MsgBoxResult = MsgBox("チェックボックスで選択されている項目の [" & strMsg & "] を実行しますか？", MsgBoxStyle.OkCancel Or MsgBoxStyle.DefaultButton2 Or MsgBoxStyle.Question, "確認")
		If retc <> MsgBoxResult.Ok Then Return


		'''' 実行
		If rdoInstal1.Checked Then
			Call Execute_Install()
		ElseIf rdoInstal2.Checked Then
			Call Execute_UpDate()
		Else
			Call Execute_UnInstall()
		End If
	End Sub

	''' <summary>
	''' インストール開始
	''' </summary>
	Private Sub Execute_Install()
		'''''' 前準備
		Dim strSysPathFrom As String		' システム単位のコピー元のパス
		Dim strSysPathTo As String			' システム単位のコピー先のパス
		Dim strLinkPath As String			' ショートカットのパス
		Dim strStartMenuPath As String		' スタートメニューパス
		Dim typSys As typInfo = Nothing
		Dim typExe As typInfo = Nothing

		' スタートメニューフォルダ生成
		strStartMenuPath = IO.Path.Combine(My.Computer.FileSystem.SpecialDirectories.Programs, mcBaseName)	' スタートメニューのフォルダパス
		IO.Directory.CreateDirectory(strStartMenuPath)


		' レジストリに書き込み & オープン (ベースフォルダ用のキー&値)
		Dim regkey As RegistryKey = My.Computer.Registry.LocalMachine.CreateSubKey(mcRegSystemKey, RegistryKeyPermissionCheck.ReadWriteSubTree)
		regkey.SetValue(mcRegBasePath, txtSetInstalFolder.Text)


		' インストール
		For ii As Integer = 0 To trvSetInf.Nodes.Count - 1
			typSys = CType(trvSetInf.Nodes(ii).Tag, typInfo)

			'''' システムの新規インストール (まだインストールしてない 且つ 今回インストール)
			' 親ノードのチェック付きをフォルダ生成
			If trvSetInf.Nodes(ii).Checked And Not typSys.bInstal Then

				' ベースフォルダの下に システム単位のフォルダをコピー (上書き禁止)
				strSysPathFrom = IO.Path.Combine(System.Windows.Forms.Application.StartupPath, typSys.strType)	   ' 自分のフォルダパス
				strSysPathTo = IO.Path.Combine(txtSetInstalFolder.Text, typSys.strType)
				tMod_Common.CopyDirectory(strSysPathFrom, strSysPathTo, False, True)

				' スタートメニューフォルダ の下に システム単位のフォルダ作成
				strLinkPath = IO.Path.Combine(strStartMenuPath, typSys.strName)
				Try
					IO.Directory.CreateDirectory(strLinkPath)
				Catch ex As Exception
				End Try

				' レジストリに書き込み (システム用のキー)
				regkey = My.Computer.Registry.LocalMachine.CreateSubKey(mcRegSystemKey & "\" & typSys.strType, RegistryKeyPermissionCheck.ReadWriteSubTree)


				' 子ノードのチェック付きの奴のみショートカット作成
				For jj As Integer = 0 To trvSetInf.Nodes(ii).Nodes.Count - 1
					If trvSetInf.Nodes(ii).Nodes(jj).Checked Then
						typExe = CType(trvSetInf.Nodes(ii).Nodes(jj).Tag, typInfo)
						Dim strWk As String = strLinkPath & "\" & typExe.strName & ".lnk"			' ショートカットファイル名

						' ショートカット作成
						tMod_CommonStrictOff.CreateShortCut(strWk, strSysPathTo & "\EXE", typExe.strType & ".EXE")
						' レジストリに書き込み
						regkey.SetValue("EXE_" & jj.ToString(), strWk)
					End If
				Next jj
			End If
		Next ii


		'''' レジストリクローズ
		If Not regkey Is Nothing Then
			regkey.Close()
		End If

		'' 画面用データ再読込
		RefreshDisp(EM_INSTALMODE.install)
	End Sub


	''' <summary>
	''' アップデート開始
	''' </summary>
	''' <remarks></remarks>
	Private Sub Execute_UpDate()
		Dim strSysPathFrom As String		' システム単位のコピー元のパス
		Dim strSysPathTo As String			' システム単位のコピー先のパス
		Dim typSys As typInfo = Nothing

		' アップデート
		For ii As Integer = 0 To trvSetInf.Nodes.Count - 1
			typSys = CType(trvSetInf.Nodes(ii).Tag, typInfo)

			If trvSetInf.Nodes(ii).Checked Then

				' ベースフォルダの下に システム単位のフォルダをコピー (上書き許可)
				strSysPathFrom = IO.Path.Combine(System.Windows.Forms.Application.StartupPath, typSys.strType)		' 自分のフォルダパス
				strSysPathTo = IO.Path.Combine(txtSetInstalFolder.Text, typSys.strType)
				tMod_Common.CopyDirectory(strSysPathFrom, strSysPathTo, True, True)
			End If
		Next ii

		'' 画面用データ再読込
		RefreshDisp(EM_INSTALMODE.update)
	End Sub


	''' <summary>
	''' アンインストール開始
	''' </summary>
	''' <remarks></remarks>
	Private Sub Execute_UnInstall()
		Dim strStartMenuPath As String		' スタートメニューパス
		Dim typSys As typInfo = Nothing
		Dim typExe As typInfo = Nothing
		Dim strWk As String

		' スタートメニューフォルダ
		strStartMenuPath = IO.Path.Combine(My.Computer.FileSystem.SpecialDirectories.Programs, mcBaseName)	' スタートメニューのフォルダパス


		' アンインストール
		For ii As Integer = 0 To trvSetInf.Nodes.Count - 1
			typSys = CType(trvSetInf.Nodes(ii).Tag, typInfo)

			If trvSetInf.Nodes(ii).Checked Then
				' ベースフォルダの下に システム単位のフォルダを削除
				strWk = IO.Path.Combine(txtSetInstalFolder.Text, typSys.strType)
				Try
					IO.Directory.Delete(strWk, True)
				Catch ex As Exception
				End Try

				' スタートメニューフォルダ の下に システム単位のフォルダを削除
				strWk = IO.Path.Combine(strStartMenuPath, typSys.strName)
				Try
					IO.Directory.Delete(strWk, True)
				Catch ex As Exception
				End Try

				' レジストリ削除
				Try
					My.Computer.Registry.LocalMachine.DeleteSubKeyTree(mcRegSystemKey & "\" & typSys.strType)
				Catch ex As Exception
				End Try
			End If
		Next ii

		'' 画面用データ再読込
		RefreshDisp(EM_INSTALMODE.delete)
	End Sub


	''' <summary>
	''' 全システム アンインストール開始
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub btnAllUninstall_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnAllUninstall.Click

		'''' 事前チェック
		Dim retc As MsgBoxResult = MsgBox("全システムをアンインストールしますか？", MsgBoxStyle.OkCancel Or MsgBoxStyle.DefaultButton2 Or MsgBoxStyle.Question, "確認")
		If retc <> MsgBoxResult.Ok Then Return


		'' ベースフォルダ削除
		Try
			IO.Directory.Delete(txtSetInstalFolder.Text, True)
		Catch ex As Exception
		End Try

		'' スタートメニューフォルダ削除
		Try
			IO.Directory.Delete(IO.Path.Combine(My.Computer.FileSystem.SpecialDirectories.Programs, mcBaseName), True)
		Catch ex As Exception
		End Try

		'' レジストリ消去
		Try
			My.Computer.Registry.LocalMachine.DeleteSubKeyTree(mcRegSystemKey)
		Catch ex As Exception
		End Try


		'' 画面用データ再読込
		RefreshDisp(EM_INSTALMODE.install)
	End Sub



	'// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
	'// その他共通処理

	''' <summary>
	''' メッセージボックスにメッセージ書き込み
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
	''' インストール状態変更
	''' </summary>
	''' <param name="emInstall">インストール設定時0   アンインストール時false</param>
	''' <remarks></remarks>
	Private Sub SetMode(ByVal emInstall As EM_INSTALMODE)
		Dim bBaseFolderOn As Boolean = True

		'''' 画面状態変更
		rdoInstal1.BackColor = Color.PeachPuff
		rdoInstal2.BackColor = Color.PeachPuff
		rdoInstal3.BackColor = Color.PeachPuff
		If EM_INSTALMODE.install = emInstall Then
			rdoInstal1.Checked = True
			rdoInstal1.BackColor = Color.Cyan
			grpSet.Text = "インストール設定"
			btnExecute.Text = "インストール開始"
			bBaseFolderOn = True

		ElseIf EM_INSTALMODE.update = emInstall Then
			rdoInstal2.Checked = True
			rdoInstal2.BackColor = Color.Cyan
			grpSet.Text = "アップデート設定"
			btnExecute.Text = "アップデート開始"
			bBaseFolderOn = False

		Else
			rdoInstal3.Checked = True
			rdoInstal3.BackColor = Color.Cyan
			grpSet.Text = "アンインストール設定"
			btnExecute.Text = "アンインストール開始"
			bBaseFolderOn = False
		End If

		'''' ベースフォルダ変更可能？
		If bBaseFolderOn And m_FastInstal Then
			txtSetInstalFolder.ReadOnly = False
			btnSelectFolder.Enabled = True
		Else
			txtSetInstalFolder.ReadOnly = True
			btnSelectFolder.Enabled = False
		End If

		'''' 設定状態のリストボックス変更
		If EM_INSTALMODE.install = emInstall Then
			SetTreeViewItem(trvSetInf, m_lstSetInf, 2, False)		' まだインストールされてない奴
		ElseIf EM_INSTALMODE.update = emInstall Then
			SetTreeViewItem(trvSetInf, m_lstSetInf, 1, True)		' インストール済みの奴
		Else
			SetTreeViewItem(trvSetInf, m_lstSetInf, 1, True)		' インストール済みの奴
		End If
	End Sub


	''' <summary>
	''' インストール・アンインストール設定
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
