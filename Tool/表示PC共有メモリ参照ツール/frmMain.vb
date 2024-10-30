Imports System.Reflection

Public Class frmMain

    ''' <summary>
    ''' ステータス情報
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnStatusInf_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnStatusInf.Click
        Dim frm As New frmHyStatus
        frm.Show()
    End Sub

    ''' <summary>
    ''' ポインター情報
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub butPointInf_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles butPointInf.Click
        Dim frm As New frmHyPoint
        frm.Show()
    End Sub

    ''' <summary>
    ''' コイル情報
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnCoilInf_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnCoilInf.Click
        Dim frm As New frmHyCoilInf
        frm.Show()
    End Sub

    ''' <summary>
    ''' コイル実績情報
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnCoilResult_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnCoilResult.Click
        Dim frm As New frmHyCoilResult
        frm.Show()
    End Sub

    ''' <summary>
    ''' マップ情報
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub bntMapInf_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles bntMapRow.Click
        Dim frm As New frmHyMap
        frm.Show()
    End Sub

    ''' <summary>
    ''' 疵情報
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnKizuInf_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnKizuInf.Click
        Dim frm As New frmHyDefectInf
        frm.Show()
    End Sub

    ''' <summary>
    ''' 重欠陥一覧表示ポインタ情報
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnHyDefList_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnHyDefList.Click
        Dim frm As New frmHyDefList
        frm.Show()
    End Sub


    ''' <summary>
    ''' 前工程疵情報
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnMaeInf_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnMaeInf.Click
        Dim frm As New frmHyMaeinf
        frm.Show()
    End Sub


    '''' <summary>
    '''' 周期疵情報
    '''' </summary>
    '''' <param name="sender"></param>
    '''' <param name="e"></param>
    '''' <remarks></remarks>
    'Private Sub btnCycleInf_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnCycleInf.Click
    '    Dim frm As New frmHyCycleInf
    '    frm.Show()
    'End Sub


    ''' <summary>
    ''' 警報情報
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnAlarm_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnAlarm.Click
        Dim frm As New frmHyAlarm
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

	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	''' <summary>
	''' 長周期情報
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	Private Sub btnLongPd_Click(sender As Object, e As EventArgs) Handles btnLongPd.Click
		Dim frm As New frmHyLongPd
		frm.Show()
	End Sub
	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

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
