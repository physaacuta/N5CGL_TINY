Imports System.Reflection

Public Class frmMain

    ''' <summary>
    ''' �X�e�[�^�X���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnStatusInf_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnStatusInf.Click
        Dim frm As New frmHyStatus
        frm.Show()
    End Sub

    ''' <summary>
    ''' �|�C���^�[���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub butPointInf_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles butPointInf.Click
        Dim frm As New frmHyPoint
        frm.Show()
    End Sub

    ''' <summary>
    ''' �R�C�����
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnCoilInf_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnCoilInf.Click
        Dim frm As New frmHyCoilInf
        frm.Show()
    End Sub

    ''' <summary>
    ''' �R�C�����я��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnCoilResult_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnCoilResult.Click
        Dim frm As New frmHyCoilResult
        frm.Show()
    End Sub

    ''' <summary>
    ''' �}�b�v���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub bntMapInf_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles bntMapRow.Click
        Dim frm As New frmHyMap
        frm.Show()
    End Sub

    ''' <summary>
    ''' �r���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnKizuInf_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnKizuInf.Click
        Dim frm As New frmHyDefectInf
        frm.Show()
    End Sub

    ''' <summary>
    ''' �d���׈ꗗ�\���|�C���^���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnHyDefList_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnHyDefList.Click
        Dim frm As New frmHyDefList
        frm.Show()
    End Sub


    ''' <summary>
    ''' �O�H���r���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnMaeInf_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnMaeInf.Click
        Dim frm As New frmHyMaeinf
        frm.Show()
    End Sub


    '''' <summary>
    '''' �����r���
    '''' </summary>
    '''' <param name="sender"></param>
    '''' <param name="e"></param>
    '''' <remarks></remarks>
    'Private Sub btnCycleInf_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnCycleInf.Click
    '    Dim frm As New frmHyCycleInf
    '    frm.Show()
    'End Sub


    ''' <summary>
    ''' �x����
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnAlarm_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnAlarm.Click
        Dim frm As New frmHyAlarm
        frm.Show()
    End Sub

    ''' <summary>
    ''' �t�@�C������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnFile_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnFile.Click
        Dim frm As New frmFile
        frm.Show()
    End Sub

	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	''' <summary>
	''' ���������
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	Private Sub btnLongPd_Click(sender As Object, e As EventArgs) Handles btnLongPd.Click
		Dim frm As New frmHyLongPd
		frm.Show()
	End Sub
	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

	''' <summary>
	''' ��� ���悤�Ƃ���
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmMain_FormClosing(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles Me.FormClosing
        ' ���C���X���b�h�I���ʒm
        modMain.TaskEnd()
    End Sub


End Class
