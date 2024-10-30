<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class CamName
    Inherits System.Windows.Forms.UserControl

    'UserControl はコンポーネント一覧をクリーンアップするために dispose をオーバーライドします。
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
        Me.grbCam = New System.Windows.Forms.GroupBox()
        Me.grbCam2 = New System.Windows.Forms.GroupBox()
        Me.lblCam2 = New System.Windows.Forms.Label()
        Me.grbCam1 = New System.Windows.Forms.GroupBox()
        Me.lblCam1 = New System.Windows.Forms.Label()
        Me.grbCam.SuspendLayout()
        Me.grbCam2.SuspendLayout()
        Me.grbCam1.SuspendLayout()
        Me.SuspendLayout()
        '
        'grbCam
        '
        Me.grbCam.Controls.Add(Me.grbCam2)
        Me.grbCam.Controls.Add(Me.grbCam1)
        Me.grbCam.Dock = System.Windows.Forms.DockStyle.Fill
        Me.grbCam.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.grbCam.ForeColor = System.Drawing.SystemColors.ControlText
        Me.grbCam.Location = New System.Drawing.Point(0, 0)
        Me.grbCam.Name = "grbCam"
        Me.grbCam.Size = New System.Drawing.Size(183, 225)
        Me.grbCam.TabIndex = 13
        Me.grbCam.TabStop = False
        Me.grbCam.Text = "カメラ"
        '
        'grbCam2
        '
        Me.grbCam2.Controls.Add(Me.lblCam2)
        Me.grbCam2.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.grbCam2.Location = New System.Drawing.Point(74, 20)
        Me.grbCam2.Name = "grbCam2"
        Me.grbCam2.Size = New System.Drawing.Size(106, 200)
        Me.grbCam2.TabIndex = 10
        Me.grbCam2.TabStop = False
        Me.grbCam2.Text = "ｶﾒﾗｾｯﾄ"
        '
        'lblCam2
        '
        Me.lblCam2.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!)
        Me.lblCam2.Location = New System.Drawing.Point(4, 20)
        Me.lblCam2.Name = "lblCam2"
        Me.lblCam2.Size = New System.Drawing.Size(99, 172)
        Me.lblCam2.TabIndex = 72
        Me.lblCam2.Text = "Label1"
        '
        'grbCam1
        '
        Me.grbCam1.Controls.Add(Me.lblCam1)
        Me.grbCam1.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.grbCam1.Location = New System.Drawing.Point(3, 20)
        Me.grbCam1.Name = "grbCam1"
        Me.grbCam1.Size = New System.Drawing.Size(69, 200)
        Me.grbCam1.TabIndex = 9
        Me.grbCam1.TabStop = False
        Me.grbCam1.Text = "ｶﾒﾗ角度"
        '
        'lblCam1
        '
        Me.lblCam1.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0!)
        Me.lblCam1.Location = New System.Drawing.Point(4, 20)
        Me.lblCam1.Name = "lblCam1"
        Me.lblCam1.Size = New System.Drawing.Size(61, 172)
        Me.lblCam1.TabIndex = 71
        Me.lblCam1.Text = "Label1"
        '
        'CamName
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.Controls.Add(Me.grbCam)
        Me.Name = "CamName"
        Me.Size = New System.Drawing.Size(183, 225)
        Me.grbCam.ResumeLayout(False)
        Me.grbCam2.ResumeLayout(False)
        Me.grbCam1.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub
    Public WithEvents grbCam As System.Windows.Forms.GroupBox
	Public WithEvents grbCam2 As System.Windows.Forms.GroupBox
	Public WithEvents grbCam1 As System.Windows.Forms.GroupBox
	Public WithEvents lblCam1 As System.Windows.Forms.Label
	Public WithEvents lblCam2 As System.Windows.Forms.Label

End Class
