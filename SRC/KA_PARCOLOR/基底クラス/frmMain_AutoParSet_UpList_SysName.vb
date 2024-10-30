Public Class frmMain_AutoParSet_UpList_SysName

    Private Sub frmMain_AutoParSet_UpList_SysName_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        'If LINE_ID_1FLL_IN = KizuLibCLI.KizuMod.GetLineID() Then
        '    lblSysName.Text = "ラミ前φω"
        '    lblSysName.BackColor = Color.DarkOrange
        '    lblSysName.ForeColor = Color.Navy
        'End If
        'If LINE_ID_1FLL_OUT = KizuLibCLI.KizuMod.GetLineID() Then
        '    lblSysName.Text = "ラミ後φω"
        '    lblSysName.BackColor = Color.DarkBlue
        '    lblSysName.ForeColor = Color.Cornsilk
        'End If
    End Sub

    ''' <summary>
    ''' メニューラベル設定
    ''' </summary>
    ''' <param name="nKind">表示種別 0：オンライン 0以外：ロジックパララン</param>
    ''' <remarks></remarks>
    Public Sub SetLabelSysName(ByVal nKind As Integer)

        If 0 = nKind Then
            lblSysName.Text = "オンライン"
            lblSysName.BackColor = Color.DarkBlue
            lblSysName.ForeColor = Color.Cornsilk
        Else
            lblSysName.Text = "ロジックパララン"
            lblSysName.BackColor = Color.DarkOrange
            lblSysName.ForeColor = Color.Navy
        End If

    End Sub
End Class
