Option Strict On

Public Class frmDebugLog

	Public Sub New()

		' この呼び出しは、Windows フォーム デザイナで必要です。
		InitializeComponent()

		' InitializeComponent() 呼び出しの後で初期化を追加します。

		lstDebug.Items.Add("TEST")

	End Sub

	Private Sub btnHide_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnHide.Click
		Me.Hide()
	End Sub

	''' <summary>
	''' リストボックスをダブルクリックしたときにメモ帳を開く
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub lstDebug_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles lstDebug.DoubleClick
		gcls_Log.ShowEdit()
	End Sub

	''' <summary>
	''' Xボタンでこのフォームを閉じようとした場合は無効化とする
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmDebugLog_FormClosing(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles Me.FormClosing
		e.Cancel = True
		Me.Hide()
	End Sub
End Class
