Option Strict On

Public Class frmDebugLog

	Public Sub New()

		' ���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
		InitializeComponent()

		' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B

		lstDebug.Items.Add("TEST")

	End Sub

	Private Sub btnHide_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnHide.Click
		Me.Hide()
	End Sub

	''' <summary>
	''' ���X�g�{�b�N�X���_�u���N���b�N�����Ƃ��Ƀ��������J��
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub lstDebug_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles lstDebug.DoubleClick
		gcls_Log.ShowEdit()
	End Sub

	''' <summary>
	''' X�{�^���ł��̃t�H�[������悤�Ƃ����ꍇ�͖������Ƃ���
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmDebugLog_FormClosing(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles Me.FormClosing
		e.Cancel = True
		Me.Hide()
	End Sub
End Class
