Public Class frmMain_AutoParSet_UpList_SysName

    Private Sub frmMain_AutoParSet_UpList_SysName_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        'If LINE_ID_1FLL_IN = KizuLibCLI.KizuMod.GetLineID() Then
        '    lblSysName.Text = "���~�O�Ӄ�"
        '    lblSysName.BackColor = Color.DarkOrange
        '    lblSysName.ForeColor = Color.Navy
        'End If
        'If LINE_ID_1FLL_OUT = KizuLibCLI.KizuMod.GetLineID() Then
        '    lblSysName.Text = "���~��Ӄ�"
        '    lblSysName.BackColor = Color.DarkBlue
        '    lblSysName.ForeColor = Color.Cornsilk
        'End If
    End Sub

    ''' <summary>
    ''' ���j���[���x���ݒ�
    ''' </summary>
    ''' <param name="nKind">�\����� 0�F�I�����C�� 0�ȊO�F���W�b�N�p������</param>
    ''' <remarks></remarks>
    Public Sub SetLabelSysName(ByVal nKind As Integer)

        If 0 = nKind Then
            lblSysName.Text = "�I�����C��"
            lblSysName.BackColor = Color.DarkBlue
            lblSysName.ForeColor = Color.Cornsilk
        Else
            lblSysName.Text = "���W�b�N�p������"
            lblSysName.BackColor = Color.DarkOrange
            lblSysName.ForeColor = Color.Navy
        End If

    End Sub
End Class
