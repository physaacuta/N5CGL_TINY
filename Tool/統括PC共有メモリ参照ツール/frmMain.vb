Imports System.Reflection

Public Class frmMain




    Private Sub frmMain_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        PutStructureSizes()

    End Sub

    Private Sub btnCoil_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnCoil.Click
        Dim frm As New frmToCoilTbl
        frm.Show()
    End Sub

    Private Sub btnSeqmem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSeqmem.Click
        Dim frm As New frmToSeqmemTbl
        frm.Show()
    End Sub

    Private Sub btnCamera_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnCamera.Click
        Dim frm As New frmToCameraTbl
        frm.Show()
    End Sub

    ''' <summary>
    ''' ファイル操作
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnFile_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnFile.Click
        Dim frm As New frmFile
        frm.Show()
    End Sub

    ''' <summary>
    ''' 画面 閉じようとした
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMain_FormClosing(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles Me.FormClosing
        ' メインスレッド終了通知
        modMain.TaskEnd()
    End Sub

End Class
