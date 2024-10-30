<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class ucStatusAria
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
		Me.Label4 = New System.Windows.Forms.Label
		Me.lblKadou = New System.Windows.Forms.Label
		Me.lblKensa = New System.Windows.Forms.Label
		Me.lblKiki = New System.Windows.Forms.Label
		Me.Label103 = New System.Windows.Forms.Label
		Me.Label6 = New System.Windows.Forms.Label
		Me.Label2 = New System.Windows.Forms.Label
		Me.lblGaidans = New System.Windows.Forms.Label
		Me.SuspendLayout()
		'
		'Label4
		'
		Me.Label4.BackColor = System.Drawing.SystemColors.Control
		Me.Label4.Font = New System.Drawing.Font("ＭＳ 明朝", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.Label4.ForeColor = System.Drawing.Color.Black
		Me.Label4.Location = New System.Drawing.Point(844, 4)
		Me.Label4.Name = "Label4"
		Me.Label4.Size = New System.Drawing.Size(103, 28)
		Me.Label4.TabIndex = 46
		Me.Label4.Text = "機器状態"
		Me.Label4.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
		'
		'lblKadou
		'
		Me.lblKadou.BackColor = System.Drawing.Color.Blue
		Me.lblKadou.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
		Me.lblKadou.Font = New System.Drawing.Font("ＭＳ 明朝", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.lblKadou.ForeColor = System.Drawing.Color.Yellow
		Me.lblKadou.Location = New System.Drawing.Point(732, 4)
		Me.lblKadou.Name = "lblKadou"
		Me.lblKadou.Size = New System.Drawing.Size(112, 28)
		Me.lblKadou.TabIndex = 47
		Me.lblKadou.Text = "稼動中"
		Me.lblKadou.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
		'
		'lblKensa
		'
		Me.lblKensa.BackColor = System.Drawing.Color.Magenta
		Me.lblKensa.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
		Me.lblKensa.Font = New System.Drawing.Font("ＭＳ 明朝", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.lblKensa.ForeColor = System.Drawing.Color.Yellow
		Me.lblKensa.Location = New System.Drawing.Point(1156, 4)
		Me.lblKensa.Name = "lblKensa"
		Me.lblKensa.Size = New System.Drawing.Size(112, 28)
		Me.lblKensa.TabIndex = 44
		Me.lblKensa.Text = "縮退"
		Me.lblKensa.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
		'
		'lblKiki
		'
		Me.lblKiki.BackColor = System.Drawing.Color.Red
		Me.lblKiki.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
		Me.lblKiki.Font = New System.Drawing.Font("ＭＳ 明朝", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.lblKiki.ForeColor = System.Drawing.Color.Yellow
		Me.lblKiki.Location = New System.Drawing.Point(944, 4)
		Me.lblKiki.Name = "lblKiki"
		Me.lblKiki.Size = New System.Drawing.Size(112, 28)
		Me.lblKiki.TabIndex = 45
		Me.lblKiki.Text = "重故障"
		Me.lblKiki.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
		'
		'Label103
		'
		Me.Label103.BackColor = System.Drawing.SystemColors.Control
		Me.Label103.Font = New System.Drawing.Font("ＭＳ 明朝", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.Label103.ForeColor = System.Drawing.Color.Black
		Me.Label103.Location = New System.Drawing.Point(632, 4)
		Me.Label103.Name = "Label103"
		Me.Label103.Size = New System.Drawing.Size(103, 28)
		Me.Label103.TabIndex = 40
		Me.Label103.Text = "稼動状態"
		Me.Label103.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
		'
		'Label6
		'
		Me.Label6.BackColor = System.Drawing.SystemColors.Control
		Me.Label6.Font = New System.Drawing.Font("ＭＳ 明朝", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.Label6.ForeColor = System.Drawing.Color.Black
		Me.Label6.Location = New System.Drawing.Point(1056, 4)
		Me.Label6.Name = "Label6"
		Me.Label6.Size = New System.Drawing.Size(103, 28)
		Me.Label6.TabIndex = 43
		Me.Label6.Text = "検査状態"
		Me.Label6.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
		'
		'Label2
		'
		Me.Label2.BackColor = System.Drawing.SystemColors.Control
		Me.Label2.Font = New System.Drawing.Font("ＭＳ 明朝", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.Label2.ForeColor = System.Drawing.Color.Black
		Me.Label2.Location = New System.Drawing.Point(0, 4)
		Me.Label2.Name = "Label2"
		Me.Label2.Size = New System.Drawing.Size(96, 28)
		Me.Label2.TabIndex = 42
		Me.Label2.Text = "ｶﾞｲﾀﾞﾝｽ"
		Me.Label2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
		'
		'lblGaidans
		'
		Me.lblGaidans.BackColor = System.Drawing.SystemColors.Control
		Me.lblGaidans.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
		Me.lblGaidans.Font = New System.Drawing.Font("ＭＳ 明朝", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.lblGaidans.ForeColor = System.Drawing.Color.Yellow
		Me.lblGaidans.Location = New System.Drawing.Point(92, 4)
		Me.lblGaidans.Name = "lblGaidans"
		Me.lblGaidans.Size = New System.Drawing.Size(532, 28)
		Me.lblGaidans.TabIndex = 41
		Me.lblGaidans.Text = "照明装置 表1異常"
		Me.lblGaidans.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
		'
		'ucStatusAria
		'
		Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
		Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
		Me.Controls.Add(Me.lblKensa)
		Me.Controls.Add(Me.lblKiki)
		Me.Controls.Add(Me.lblKadou)
		Me.Controls.Add(Me.lblGaidans)
		Me.Controls.Add(Me.Label4)
		Me.Controls.Add(Me.Label103)
		Me.Controls.Add(Me.Label6)
		Me.Controls.Add(Me.Label2)
		Me.Margin = New System.Windows.Forms.Padding(0)
		Me.Name = "ucStatusAria"
		Me.Size = New System.Drawing.Size(1272, 34)
		Me.ResumeLayout(False)

	End Sub
	Friend WithEvents Label4 As System.Windows.Forms.Label
	Friend WithEvents lblKadou As System.Windows.Forms.Label
	Friend WithEvents lblKensa As System.Windows.Forms.Label
	Friend WithEvents lblKiki As System.Windows.Forms.Label
	Friend WithEvents Label103 As System.Windows.Forms.Label
	Friend WithEvents Label6 As System.Windows.Forms.Label
	Friend WithEvents Label2 As System.Windows.Forms.Label
	Friend WithEvents lblGaidans As System.Windows.Forms.Label

End Class
