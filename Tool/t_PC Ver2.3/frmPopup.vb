Public Class frmPopup
	Public g_strMsg As String

	Private Sub lblMsg_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles lblMsg.Click
		Me.Hide()
	End Sub

	Private Sub timClose_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles timClose.Tick
		Me.Hide()
	End Sub



	Private Sub frmPopup_VisibleChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.VisibleChanged
		If Me.Visible Then
			Me.Location = New Point(My.Computer.Screen.WorkingArea.Width - Me.Size.Width, My.Computer.Screen.WorkingArea.Height - Me.Size.Height)

			lblMsg.Text = g_strMsg
			timClose.Start()
		Else
			'タイマーが残っているかもしれないのでキャンセル
			timClose.Stop()
		End If
	End Sub
End Class