<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmTestSem
    Inherits System.Windows.Forms.Form

    'フォームがコンポーネントの一覧をクリーンアップするために dispose をオーバーライドします。
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing AndAlso components IsNot Nothing Then
            components.Dispose()
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Windows フォーム デザイナで必要です。
    Private components As System.ComponentModel.IContainer

    'メモ: 以下のプロシージャは Windows フォーム デザイナで必要です。
    'Windows フォーム デザイナを使用して変更できます。  
    'コード エディタを使って変更しないでください。
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
Me.components = New System.ComponentModel.Container
Me.Timer1 = New System.Windows.Forms.Timer(Me.components)
Me.TextBox1 = New System.Windows.Forms.TextBox
Me.SuspendLayout()
'
'Timer1
'
Me.Timer1.Enabled = True
Me.Timer1.Interval = 30
'
'TextBox1
'
Me.TextBox1.Location = New System.Drawing.Point(48, 104)
Me.TextBox1.Name = "TextBox1"
Me.TextBox1.Size = New System.Drawing.Size(44, 19)
Me.TextBox1.TabIndex = 0
'
'frmTestSem
'
Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
Me.ClientSize = New System.Drawing.Size(292, 273)
Me.Controls.Add(Me.TextBox1)
Me.Name = "frmTestSem"
Me.Text = "frmTestSem"
Me.ResumeLayout(False)
Me.PerformLayout()

End Sub
	Friend WithEvents Timer1 As System.Windows.Forms.Timer
 Friend WithEvents TextBox1 As System.Windows.Forms.TextBox
End Class
