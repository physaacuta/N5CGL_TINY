<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPopup
    Inherits System.Windows.Forms.Form

    'フォームがコンポーネントの一覧をクリーンアップするために dispose をオーバーライドします。
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Windows フォーム デザイナで必要です。
    Private components As System.ComponentModel.IContainer

    'メモ: 以下のプロシージャは Windows フォーム デザイナで必要です。
    'Windows フォーム デザイナを使用して変更できます。  
    'コード エディタを使って変更しないでください。
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
		Me.components = New System.ComponentModel.Container
		Me.lblMsg = New System.Windows.Forms.Label
		Me.timClose = New System.Windows.Forms.Timer(Me.components)
		Me.SuspendLayout()
		'
		'lblMsg
		'
		Me.lblMsg.BackColor = System.Drawing.Color.MistyRose
		Me.lblMsg.Dock = System.Windows.Forms.DockStyle.Fill
		Me.lblMsg.Font = New System.Drawing.Font("ＭＳ 明朝", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.lblMsg.Location = New System.Drawing.Point(0, 0)
		Me.lblMsg.Name = "lblMsg"
		Me.lblMsg.Size = New System.Drawing.Size(431, 43)
		Me.lblMsg.TabIndex = 0
		Me.lblMsg.Text = "Label1"
		'
		'timClose
		'
		Me.timClose.Interval = 3000
		'
		'frmPopup
		'
		Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
		Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
		Me.ClientSize = New System.Drawing.Size(431, 43)
		Me.ControlBox = False
		Me.Controls.Add(Me.lblMsg)
		Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None
		Me.MaximizeBox = False
		Me.MinimizeBox = False
		Me.Name = "frmPopup"
		Me.Opacity = 0.75
		Me.ShowIcon = False
		Me.ShowInTaskbar = False
		Me.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide
		Me.StartPosition = System.Windows.Forms.FormStartPosition.Manual
		Me.Text = "frmPopup"
		Me.TopMost = True
		Me.ResumeLayout(False)

	End Sub
	Friend WithEvents lblMsg As System.Windows.Forms.Label
	Friend WithEvents timClose As System.Windows.Forms.Timer
End Class
