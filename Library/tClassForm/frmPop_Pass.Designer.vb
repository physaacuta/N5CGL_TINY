<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPop_Pass
	Inherits tClassForm.frmPop_Btn2

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing AndAlso components IsNot Nothing Then
            components.Dispose()
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.lblMsg01 = New System.Windows.Forms.Label
        Me.lblMsg = New System.Windows.Forms.Label
        Me.txtPass = New System.Windows.Forms.TextBox
        Me.SuspendLayout()
        '
        'grpFunction
        '
        Me.grpFunction.Location = New System.Drawing.Point(76, 112)
        '
        'btnF01
        '
        Me.btnF01.Tag = 0
        '
        'btnF02
        '
        Me.btnF02.Tag = 11
        '
        'lblMsg01
        '
        Me.lblMsg01.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblMsg01.Location = New System.Drawing.Point(36, 8)
        Me.lblMsg01.Name = "lblMsg01"
        Me.lblMsg01.Size = New System.Drawing.Size(312, 24)
        Me.lblMsg01.TabIndex = 12
        Me.lblMsg01.Text = "実行するにはパスワードが必要です。"
        '
        'lblMsg
        '
        Me.lblMsg.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblMsg.Location = New System.Drawing.Point(36, 32)
        Me.lblMsg.Name = "lblMsg"
        Me.lblMsg.Size = New System.Drawing.Size(312, 36)
        Me.lblMsg.TabIndex = 13
        Me.lblMsg.Text = "パスワードを入力して下さい。" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "パスワードは[KIZU]です。"
        '
        'txtPass
        '
        Me.txtPass.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper
        Me.txtPass.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtPass.Location = New System.Drawing.Point(36, 76)
        Me.txtPass.Name = "txtPass"
        Me.txtPass.Size = New System.Drawing.Size(308, 23)
        Me.txtPass.TabIndex = 14
        '
        'frmPop_Pass
        '
        Me.ClientSize = New System.Drawing.Size(377, 202)
        Me.Controls.Add(Me.txtPass)
        Me.Controls.Add(Me.lblMsg)
        Me.Controls.Add(Me.lblMsg01)
        Me.Name = "frmPop_Pass"
        Me.Text = "パスワード確認"
        Me.Controls.SetChildIndex(Me.lblMsg01, 0)
        Me.Controls.SetChildIndex(Me.lblMsg, 0)
        Me.Controls.SetChildIndex(Me.grpFunction, 0)
        Me.Controls.SetChildIndex(Me.txtPass, 0)
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
	Friend WithEvents lblMsg01 As System.Windows.Forms.Label
    Friend WithEvents lblMsg As System.Windows.Forms.Label
    Friend WithEvents txtPass As System.Windows.Forms.TextBox

End Class
