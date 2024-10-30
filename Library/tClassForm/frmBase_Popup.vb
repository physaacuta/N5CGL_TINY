'*********************************************************************************
' 疵検用ポップアップ画面の親フォーム
'	[Ver]
'		Ver.01    2007/03/31  vs2005 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

''' <summary>
''' 疵検用ポップアップ画面の親フォーム
''' </summary>
''' <remarks></remarks>
Public Class frmBase_Popup
	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 外部プロパティ
	'/ /////////////////////////////////////////////////////////////////////////////////

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ メンバ 変数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Protected Friend my_lstFunction As New List(Of Button)						' ボタンコントロール配列
	Protected Friend my_nSelectedFNo As Integer = -1								' 初期選択ファンクションボタンNo (0オリジン。-1は選択無し)

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 外部イベント
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' ファンクションボタン押下イベント
	''' </summary>
	''' <param name="FncNo">0オリジン</param>
	''' <param name="onKey">ボタンクリック時も取れるよ</param>
	''' <remarks></remarks>
	Public Event tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys)


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 外部アクセス メソッド
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' ファンクションボタン用コントロール を登録
	''' </summary>
	''' <param name="btn">ファンクションボタンコントロール</param>
	''' <param name="FncNo">FncNo (0オリジン)</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub AddFunctionButton(ByVal btn As Button, ByVal FncNo As Integer)
		btn.Tag = FncNo			'Tagにセット
		my_lstFunction.Add(btn)

		'' ついでにイベントハンドラに追加
		AddHandler btn.Click, AddressOf FunctionButton_Click
	End Sub

	''' <summary>
	''' イベントを発生させる
	''' </summary>
	''' <param name="FncNo">どのファンクションボタン (0オリジン) </param>
	''' <param name="onKey">一緒に押されているModifiersキー</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub SetEvFunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys)
		RaiseEvent tEv_FunctionButton(FncNo, onKey)
	End Sub

	''' <summary>
	''' 初期選択ファンクションボタンNoを指定
	''' </summary>
	''' <param name="FncNo">選択状態にするファンクションボタンNo(0オリジン。-1は選択無し)</param>
	''' <remarks></remarks>
	Public Sub SetSelectedFNo(ByVal FncNo As Integer)
		my_nSelectedFNo = FncNo
	End Sub



	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ イベント
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' 生成
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmBase_Popup_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

		'' デバック向上対策
#If DEBUG Then
		' デバック中は最前面にしない
		Me.TopMost = False
#End If


	End Sub

	''' <summary>
	''' 表示完了
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmBase_Popup_Shown(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Shown
		If -1 = my_nSelectedFNo Then Return
		'''' 該当するファンクションボタンを追加
		For Each btn As Button In my_lstFunction
			If Not btn.Tag Is Nothing AndAlso CInt(btn.Tag) = my_nSelectedFNo Then
				btn.Select()
				Exit For
			End If
		Next
	End Sub
	''' <summary>
	''' 開放
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmBase_Popup_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
		'' 開放
		my_lstFunction.Clear()
	End Sub
	''' <summary>
	''' ファンクションボタンクリック
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub FunctionButton_Click(ByVal sender As Object, ByVal e As System.EventArgs)
		Dim btn As Button = CType(sender, Button)
		Dim FncNo As Integer = CInt(btn.Tag)		' ボタンNo取り出し

		' ファンクションボタン押下イベント発生
		Call SetEvFunctionButton(FncNo, Control.ModifierKeys())
	End Sub

	''' <summary>
	''' キーダウン (ファンクションキー)
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmBasePopup_KeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles Me.KeyDown
		Select Case e.KeyValue
			Case Keys.F1 To Keys.F12
				Dim FncNo As Integer = e.KeyValue - Keys.F1
				For ii As Integer = 0 To my_lstFunction.Count - 1
					If FncNo = CInt(my_lstFunction(ii).Tag) Then
						' ファンクションボタンが無効の場合は、イベント発生させない
						If Not my_lstFunction(ii).Enabled Then Exit Sub
						' これ以降は、メソッドにキーイベントを発生させないようにする
						e.Handled = True

						' ファンクションボタン押下イベント発生
						Call SetEvFunctionButton(FncNo, e.Modifiers)
						Exit Sub
					End If
				Next ii
		End Select
	End Sub


End Class
