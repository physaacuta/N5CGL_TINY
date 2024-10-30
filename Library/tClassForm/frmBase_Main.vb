'*********************************************************************************
' メニュー有り画面の親フォーム
'	[Ver]
'		Ver.01    2007/03/31  vs2005 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

''' <summary>
''' メニュー有り画面の親フォーム
''' </summary>
''' <remarks></remarks>
Public Class frmBase_Main

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ メンバ 変数
	'/ /////////////////////////////////////////////////////////////////////////////////
	Protected Friend my_btnFunction(11) As Button						' ボタンコントロール配列 (Newでインスタンスセット済み)
	Protected Friend my_frmParent As Form = Nothing						' 通常はメニュー画面

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 外部プロパティ
	'/ /////////////////////////////////////////////////////////////////////////////////

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 外部イベント
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' ファンクションボタン押下イベント
	''' </summary>
	''' <param name="FncNo">0オリジン</param>
	''' <param name="enable">ボタンコントロール有効時 True</param>
	''' <param name="onKey">ボタンクリック時も取れるよ</param>
	''' <remarks></remarks>
	Public Event tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys)


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ イベント
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' コンストラクタ
	''' </summary>
	''' <remarks></remarks>
	Public Sub New()
		' この呼び出しは、Windows フォーム デザイナで必要です。
		InitializeComponent()

		' InitializeComponent() 呼び出しの後で初期化を追加します。
		'''' ファンクションボタンのコントロール配列を生成する
		my_btnFunction(0) = btnF01
		my_btnFunction(1) = btnF02
		my_btnFunction(2) = btnF03
		my_btnFunction(3) = btnF04
		my_btnFunction(4) = btnF05
		my_btnFunction(5) = btnF06
		my_btnFunction(6) = btnF07
		my_btnFunction(7) = btnF08
		my_btnFunction(8) = btnF09
		my_btnFunction(9) = btnF10
		my_btnFunction(10) = btnF11
		my_btnFunction(11) = btnF12
		'' ボタン番号セット
		For ii As Integer = 0 To my_btnFunction.Length - 1
			my_btnFunction(ii).Tag = ii
		Next
	End Sub

	''' <summary>
	''' ファンクションボタンクリック
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub FunctionButton_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles btnF01.Click, btnF02.Click, btnF03.Click, btnF04.Click, btnF05.Click, btnF06.Click, btnF07.Click, btnF08.Click, btnF09.Click, btnF10.Click, btnF11.Click, btnF12.Click
		Dim btn As Button = CType(sender, Button)
		Dim FncNo As Integer = CInt(btn.Tag)		' ボタンNo取り出し

		' ファンクションボタン押下イベント発生
		Call SetEvFunctionButton(FncNo, btn.Enabled, Control.ModifierKeys())
	End Sub

	''' <summary>
	''' キーダウン (ファンクションキー)
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmBaseMain_KeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles Me.KeyDown
		Select Case e.KeyValue
			Case Keys.F1 To Keys.F12
				Dim FncNo As Integer = e.KeyValue - Keys.F1

				' ↓この処理止め。  ボタン無効状態で、Shift+F12 とかのイベントが取れない為
				'' '' ファンクションボタンが無効の場合は、イベント発生させない
				' ''If Not my_btnFunction(FncNo).Enabled Then Exit Sub

				' これ以降は、メソッドにキーイベントを発生させないようにする
				e.Handled = True

				' ファンクションボタン押下イベント発生
				Call SetEvFunctionButton(FncNo, my_btnFunction(FncNo).Enabled, e.Modifiers)
		End Select
	End Sub


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ 外部アクセス メソッド
	'/ /////////////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' イベントを発生させる
	''' </summary>
	''' <param name="FncNo">どのファンクションボタン (0オリジン) </param>
	''' <param name="enable">ボタンコントロール有効時 True</param>
	''' <param name="onKey">一緒に押されているModifiersキー</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub SetEvFunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys)
		RaiseEvent tEv_FunctionButton(FncNo, enable, onKey)
	End Sub
	''' <summary>
	''' 呼び出し元フォームをセット
	''' </summary>
	''' <param name="frm"></param>
	''' <remarks></remarks>
	Public Overridable Sub SetParentForm(ByRef frm As Form)
		my_frmParent = frm
	End Sub
	''' <summary>
	''' フォーム終了時に 親フォームを表示
	''' </summary>
	''' <remarks></remarks>
	Protected Friend Overridable Sub ExecFromClose()
		If Not my_frmParent Is Nothing Then my_frmParent.Show() ' 親フォームを表示
		Me.Close()
	End Sub
End Class
