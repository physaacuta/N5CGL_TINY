'*********************************************************************************
' 全画面の基底フォーム
'	[Ver]
'		Ver.01    2007/03/31  vs2005 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary

''' <summary>
''' 全画面の基底フォーム
''' </summary>
''' <remarks></remarks>
Public Class frmBase
	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 定義
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' 画面状態のデフォルト定義
	''' </summary>
	''' <remarks></remarks>
	Public Enum gem_FromFormatMode
		''' <summary>指定無し</summary>
		iNON = 0
		''' <summary>オンライン用</summary>
		iONLINE = 1
		''' <summary>オフライン(遠隔)用</summary>
		iOFFLINE = 2
	End Enum

	Private Delegate Sub dele_LoadEnding()								' 非同期イベント実行用。(shownイベントの存在を知らなかった為、作った。もういらないけど、互換の為残しておこう)


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ メンバ 変数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private m_nFromFormatMode As gem_FromFormatMode = gem_FromFormatMode.iNON		' 画面状態のデフォルト定義 (0:指定無し 1:オンライン用 2:オフライン(遠隔)用)					
	Private m_bMoveable As Boolean = False								' 画面移動禁止 (VB6のMoveableに相当させた)
	Private m_bAltF4Enable As Boolean = False							' Windowsキーによる画面終了禁止 (Alt + F4 無効化)
	Private m_bEndButtonEnable As Boolean = True						' ×ボタン有効無効


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 外部プロパティ
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' 画面移動禁止 (VB6のMoveableに相当させた)
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(False) _
	 , System.ComponentModel.Category("☆kizuForm専用プロパティ") _
	 , System.ComponentModel.Description("画面移動禁止 (VB6のMoveableに相当させた)")> _
	Public Property tProp_Moveable() As Boolean
		Get
			Return m_bMoveable
		End Get
		Set(ByVal Value As Boolean)
			m_bMoveable = Value
		End Set
	End Property
	''' <summary>
	''' Windowsキーによる画面終了禁止 (Alt + F4 無効化)
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(False) _
	 , System.ComponentModel.Category("☆kizuForm専用プロパティ") _
	 , System.ComponentModel.Description("Windowsキーによる画面終了禁止 (Alt + F4 無効化)")> _
	Public Property tProp_AltF4Enable() As Boolean
		Get
			Return m_bAltF4Enable
		End Get
		Set(ByVal Value As Boolean)
			m_bAltF4Enable = Value
		End Set
	End Property
	''' <summary>
	''' ×ボタン有効無効切り替え
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(True) _
	 , System.ComponentModel.Category("☆kizuForm専用プロパティ") _
	 , System.ComponentModel.Description("×ボタン有効無効切り替え (false:無効)")> _
	Public Property tProp_EndButtonEnable() As Boolean
		Get
			Return m_bEndButtonEnable
		End Get
		Set(ByVal Value As Boolean)
			m_bEndButtonEnable = Value
			' 実行中の切り替えだったら即時実行
			If Not Value Then
				subEndButtonEnable()
			End If
		End Set
	End Property
	''' <summary>
	''' 画面状態定義
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(gem_FromFormatMode.iNON) _
	 , System.ComponentModel.Category("☆kizuForm専用プロパティ") _
	 , System.ComponentModel.Description("画面状態のデフォルト定義" & vbCrLf & _
										 "  NON:指定無し" & vbCrLf & _
										 "  ONLINE:オンライン用(フォームサイズ変更不可・最大最小ボタン削除等)" & vbCrLf & _
										 "  OFFLINE:オフライン用(フォームサイズ変更・最大最小ボタン表示等))")> _
	Public Property tProp_FromFormatMode() As gem_FromFormatMode
		Get
			Return m_nFromFormatMode
		End Get
		Set(ByVal Value As gem_FromFormatMode)
			m_nFromFormatMode = Value
			Call setFormFormatMode(Value)
		End Set
	End Property




	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 外部アクセス メソッド
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' 画面モード設定
	''' </summary>
	''' <param name="mode">iONLINE:オンライン用 iOFFLINE:オフライン(遠隔)用</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub SetFormFormatMode(ByVal mode As gem_FromFormatMode)
		If gem_FromFormatMode.iNON = mode Then						' 指定無し

		ElseIf gem_FromFormatMode.iONLINE = mode Then				' オンライン
			Me.ControlBox = False													' コントロールボタン 無効
			Me.MaximizeBox = False													' ボタンが見えないだけで、タイトルバークリックしたら最大化する為
			Me.MinimizeBox = False
			Me.FormBorderStyle = Windows.Forms.FormBorderStyle.FixedSingle			' フォームサイズ変更不可
			Me.tProp_Moveable = False												' 画面移動禁止

		ElseIf gem_FromFormatMode.iOFFLINE = mode Then				' 遠隔
			Me.ControlBox = True													' コントロールボタン 有効
			Me.MaximizeBox = True
			Me.MinimizeBox = True
			Me.FormBorderStyle = Windows.Forms.FormBorderStyle.Sizable				' フォームサイズ変更可能
			Me.tProp_Moveable = True												' 画面移動許可
		End If
	End Sub

	''' <summary>
	''' Me.Activated では使い物にならない為、Loadの終了一回だけ非同期でメソッドをコールしてあげるイベントを作ったよ。
	''' </summary>
	''' <remarks></remarks>
	Protected Overridable Sub LoadEnding()
		'' ×ボタンの動作
		subEndButtonEnable()
	End Sub

	''' <summary>
	''' 自画面を指定モニタ位置に表示
	''' </summary>
	''' <param name="nMonitorNo">モニタ番号 (1オリジン)</param>
	''' <param name="bFit">フォームをモニターサイズに合わせる</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub SetLocationForm(ByVal nMonitorNo As Integer, Optional ByVal bFit As Boolean = False)
		'' 表示したいモニタの情報にあわせてフォームサイズを変更
		Dim monitor() As System.Windows.Forms.Screen
		monitor = System.Windows.Forms.Screen.AllScreens()

		nMonitorNo -= 1	'なぜか昔から1オリジンになっていた為、互換性の為にそのままとする

		'' 番号チェック
		If 0 > nMonitorNo Or nMonitorNo >= monitor.Length Then
			nMonitorNo = 0
		End If

		'' 位置のみ、 サイズも含めた 調整？
		If bFit Then
			Me.Bounds = monitor(nMonitorNo).Bounds
		Else
			Me.Location = monitor(nMonitorNo).Bounds.Location
		End If
	End Sub

	''' <summary>
	''' 画面印刷
	''' </summary>
	''' <param name="bYoko">True:横 False:縦</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub PrintOut(Optional ByVal bYoko As Boolean = True)
		Dim cls_Hcopy As New tClass.tClass_HeadCopy

		Try
			cls_Hcopy.prop_Landscape = bYoko	' 印刷方向
			cls_Hcopy.prop_Strach = True		' ストレッチ
			Call cls_Hcopy.Print(Me)			' 表示中の自画面を印刷

		Catch ex As Exception

		Finally
			If Not cls_Hcopy Is Nothing Then
				cls_Hcopy.Dispose()
				cls_Hcopy = Nothing
			End If
		End Try

	End Sub

	''' <summary>
	''' ハードコピー
	''' </summary>
	''' <param name="strFolder">保存ファイルのフォルダパス</param>
	''' <param name="strName">保存ファイルのファイル名</param>
	''' <param name="emSaveMode">保存形式 (jpeg時:System.Drawing.Imaging.ImageFormat.Jpeg)</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub HardCopy(ByVal strFolder As String, ByVal strName As String, ByVal emSaveMode As System.Drawing.Imaging.ImageFormat)
		Dim cls_Hcopy As New tClass.tClass_HeadCopy
		Dim strFPath As String
		Dim strMsg As String

		'''' フルパス
		strFPath = IO.Path.Combine(strFolder, strName)

		'''' フォルダの有無チェック(無かったら作ろう)
		tMod.CheckFolder(strFolder)

		'''' ハードコピー
		Try
			Call cls_Hcopy.HCopy(Me, strFPath, emSaveMode)
			strMsg = System.IO.Path.GetFullPath(strFPath) & "に画像を保存しました。"
		Catch ex As Exception
			strMsg = "画像保存に失敗しました。"
		Finally
			If Not cls_Hcopy Is Nothing Then
				cls_Hcopy.Dispose()
				cls_Hcopy = Nothing
			End If
		End Try

		'''' ガイダンス表示
		Using frmMsg As New frmPop_MsgBox(strMsg, Me.Text)
			frmMsg.ShowDialog()
		End Using
	End Sub


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ イベント
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' フォームロード
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmBaseMain_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        ' 視覚スタイルを無効にする  (DataGridViewでボタン等を追加している時に、視覚スタイルが有効だと背景色が変更できないから)
        Application.VisualStyleState = VisualStyles.VisualStyleState.NonClientAreaEnabled

        '' 非同期でフォーム終了した時点でイベントを発生してあげる
        'Dim dele As New dele_LoadEnding(AddressOf LoadEnding)
        'Me.BeginInvoke(dele) '別スレッドからのアクセス用
    End Sub

	''' <summary>
	''' ロード完了
	''' </summary>
	Private Sub frmBase_Shown(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Shown
		' 非同期でフォーム終了した時点でイベントを発生してあげる
		Dim dele As New dele_LoadEnding(AddressOf LoadEnding)
		Me.BeginInvoke(dele) '別スレッドからのアクセス用
	End Sub

	''' <summary>
	''' 開放
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmBaseMain_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
		' 現状なにも無し
	End Sub

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 内部 メソッド
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' ×ボタン 無効化処理 (デザイナの最後でコールされる)
	''' </summary>
	''' <remarks></remarks>
	Private Sub subEndButtonEnable()
		' デザイナの場合は、無効にしたほうがよい？

		' フォームの表示が完了した後でないと、ハンドルが無効の為、注意する事
		Try
			If Not m_bEndButtonEnable Then
				tClassLibrary.tMod.subFormEndButton_Desebled(Me.Handle)
			End If
		Catch ex As Exception
		End Try
	End Sub


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ オーバーライド メソッド
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' 自画面を指定モニタ位置に表示
	''' </summary>
	''' <param name="nMonitorNo">モニタ番号 (1オリジン)</param>
	''' <param name="bFit">フォームをモニターサイズに合わせる</param>
	''' <remarks></remarks>
	Public Overloads Sub Show(ByVal nMonitorNo As Integer, Optional ByVal bFit As Boolean = False)
		' 独自
		Call SetLocationForm(nMonitorNo, bFit)

		' 元からの処理を実行
		MyBase.Show()
	End Sub
	Public Overloads Sub ShowDialog(ByVal nMonitorNo As Integer, Optional ByVal bFit As Boolean = False)
		' 独自
		Call SetLocationForm(nMonitorNo, bFit)

		' 元からの処理を実行
		MyBase.ShowDialog()
	End Sub

	'' デバック向上対策
	' デバックでこれが有効としていると、デバック中にフックされてテストしにくい為、デバック時のみ無効化
	' その為、以下の処理が、デバック時には、有効とならない為、注意する事
	'	・ 画面移動の禁止
	'	・ Alt + F4 によるが画面終了 無効化
#If Not Debug Then
	''' <summary>
	''' WndProc オーバーライド
	''' </summary>
	''' <param name="m"></param>
	''' <remarks></remarks>
	Protected Overrides Sub WndProc(ByRef m As Message)
		Const WM_SYSCOMMAND As Integer = &H112		' システムコマンド	
		Const SC_MOVE As Integer = &HF010		   ' ウィンドウ移動

		Try
			'''''' カスタマイズ
			' 画面移動禁止
			If Not m_bMoveable AndAlso m.Msg = WM_SYSCOMMAND AndAlso (m.WParam.ToInt32() And &HFFF0) = SC_MOVE Then
				m.Result = IntPtr.Zero
			Else
				'' 全メッセージ 処理
				MyBase.WndProc(m)
			End If
		Catch ex As Exception

		End Try
	End Sub

	''' <summary>
	''' ProcessCmdKey オーバーライド
	''' </summary>
	''' <param name="msg"></param>
	''' <param name="keyData"></param>
	''' <returns></returns>
	''' <remarks></remarks>
	Protected Overrides Function ProcessCmdKey(ByRef msg As System.Windows.Forms.Message, ByVal keyData As System.Windows.Forms.Keys) As Boolean
		Const WM_SYSKEYDOWN As Integer = &H104		' Altキーを押した状態で他のキーを押した時
		Const VK_F4 As Integer = &H73				' F4キー

		Try
			'''''' カスタマイズ
			' Alt + F4 によるが画面終了 無効化
			If Not m_bAltF4Enable AndAlso msg.Msg = WM_SYSKEYDOWN AndAlso msg.WParam.ToInt32() = VK_F4 Then
				msg.Result = IntPtr.Zero
				Return True
			Else
				'' 全メッセージ 処理
				MyBase.ProcessCmdKey(msg, keyData)
			End If
		Catch ex As Exception

		End Try
	End Function
#End If



End Class