''' <summary>
''' ソースが大きくなってきたので、分離。キー操作に特化
'''   ※まだ Rartial がよくわかっていないが、フォームを開くと変になるので注意。ソースのみ開くこと
''' </summary>
''' <remarks></remarks>
Partial Public Class frmMain

    ''' <summary>
    ''' キー入力
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMain_KeyDown(sender As Object, e As System.Windows.Forms.KeyEventArgs) Handles Me.KeyDown
        '''' 入力コントロールなら無視
        Dim cnt As frmMain = CType(sender, frmMain)
        If Not TypeOf cnt.ActiveControl Is SplitContainer Then Return ' メイン部分

        Dim cntSub As SplitContainer = CType(cnt.ActiveControl, SplitContainer)
        If TypeOf cntSub.ActiveControl Is NumericUpDown Then Return ' 開発欄の入力

        For Each kev As tClassLibrary.RpiPlugin.IKeyInput In m_plugins.KeyInputs
            If kev.OnKeyDown(e) Then
                Return
            End If
        Next


        '''' ここから
        '=====================================
        ' ファンクション関係関係
        If e.KeyCode = Keys.F5 Then
            Call tsbSearch_Click(Nothing, Nothing)
        End If


        '=====================================
        ' ブックマーク関係

        If e.KeyCode >= Keys.D1 And e.KeyCode <= Keys.D9 Then
            Dim nNo As Integer = e.KeyValue - Keys.D1
            ' ジャンプ
            'If (e.Modifiers And Keys.Shift) = Keys.Shift Then	'単独で聞きたいので、これではダメ
            If e.Modifiers = Keys.Shift Then
                Call mfrm_Bookmark.MarkJump(nNo)
            End If

            ' 追加
            'If (e.Modifiers And Keys.Control) = Keys.Control Then
            If e.Modifiers = Keys.Control Then
                Call mfrm_Bookmark.MarkAdd(nNo)
            End If

            ' 削除
            'If (e.Modifiers And Keys.Control) = Keys.Control And (e.Modifiers And Keys.Shift) = Keys.Shift Then
            If e.Modifiers = (Keys.Control Or Keys.Shift) Then
                Call mfrm_Bookmark.MarkDel(nNo)
            End If

            Call RePaint()
        End If

    End Sub
End Class
