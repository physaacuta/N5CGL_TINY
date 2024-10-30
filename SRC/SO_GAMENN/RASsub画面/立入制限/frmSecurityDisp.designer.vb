<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmSecurityDisp
    'Inherits System.Windows.Forms.Form
    Inherits tClassForm.frmBase_Popup

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
        Me.components = New System.ComponentModel.Container()
        Me.cmdEnd = New System.Windows.Forms.Button()
        Me.timGetStatus = New System.Windows.Forms.Timer(Me.components)
        Me.timDisp = New System.Windows.Forms.Timer(Me.components)
        Me.lblStat11 = New System.Windows.Forms.Label()
        Me.Label10 = New System.Windows.Forms.Label()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.lblStat12 = New System.Windows.Forms.Label()
        Me.SuspendLayout()
        '
        'cmdEnd
        '
        Me.cmdEnd.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink
        Me.cmdEnd.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmdEnd.Location = New System.Drawing.Point(240, 112)
        Me.cmdEnd.Name = "cmdEnd"
        Me.cmdEnd.Size = New System.Drawing.Size(112, 32)
        Me.cmdEnd.TabIndex = 10
        Me.cmdEnd.Text = "F12:閉じる"
        Me.cmdEnd.UseVisualStyleBackColor = True
        '
        'timGetStatus
        '
        Me.timGetStatus.Enabled = True
        '
        'timDisp
        '
        Me.timDisp.Enabled = True
        '
        'lblStat11
        '
        Me.lblStat11.BackColor = System.Drawing.Color.FromArgb(CType(CType(192, Byte), Integer), CType(CType(192, Byte), Integer), CType(CType(255, Byte), Integer))
        Me.lblStat11.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblStat11.Font = New System.Drawing.Font("ＭＳ 明朝", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblStat11.Location = New System.Drawing.Point(132, 44)
        Me.lblStat11.Name = "lblStat11"
        Me.lblStat11.Size = New System.Drawing.Size(108, 64)
        Me.lblStat11.TabIndex = 67
        Me.lblStat11.Text = "不明"
        Me.lblStat11.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label10
        '
        Me.Label10.BackColor = System.Drawing.Color.Black
        Me.Label10.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label10.Font = New System.Drawing.Font("ＭＳ 明朝", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label10.ForeColor = System.Drawing.Color.Transparent
        Me.Label10.Location = New System.Drawing.Point(4, 44)
        Me.Label10.Name = "Label10"
        Me.Label10.Size = New System.Drawing.Size(124, 64)
        Me.Label10.TabIndex = 66
        Me.Label10.Text = "電気室"
        Me.Label10.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label1
        '
        Me.Label1.BackColor = System.Drawing.Color.Black
        Me.Label1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label1.Font = New System.Drawing.Font("ＭＳ 明朝", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.ForeColor = System.Drawing.Color.Transparent
        Me.Label1.Location = New System.Drawing.Point(132, 4)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(108, 36)
        Me.Label1.TabIndex = 66
        Me.Label1.Text = "扉"
        Me.Label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label2
        '
        Me.Label2.BackColor = System.Drawing.Color.Black
        Me.Label2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label2.Font = New System.Drawing.Font("ＭＳ 明朝", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label2.ForeColor = System.Drawing.Color.Transparent
        Me.Label2.Location = New System.Drawing.Point(244, 4)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(108, 36)
        Me.Label2.TabIndex = 66
        Me.Label2.Text = "鍵"
        Me.Label2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lblStat12
        '
        Me.lblStat12.BackColor = System.Drawing.Color.FromArgb(CType(CType(192, Byte), Integer), CType(CType(192, Byte), Integer), CType(CType(255, Byte), Integer))
        Me.lblStat12.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblStat12.Font = New System.Drawing.Font("ＭＳ 明朝", 15.75!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblStat12.Location = New System.Drawing.Point(244, 44)
        Me.lblStat12.Name = "lblStat12"
        Me.lblStat12.Size = New System.Drawing.Size(108, 64)
        Me.lblStat12.TabIndex = 67
        Me.lblStat12.Text = "不明"
        Me.lblStat12.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'frmSecurityDisp
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(357, 149)
        Me.Controls.Add(Me.lblStat12)
        Me.Controls.Add(Me.lblStat11)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.Label10)
        Me.Controls.Add(Me.cmdEnd)
        Me.Name = "frmSecurityDisp"
        Me.Text = "立入制限表示画面"
        Me.TopMost = False
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents cmdEnd As System.Windows.Forms.Button
    Friend WithEvents timGetStatus As System.Windows.Forms.Timer
    Friend WithEvents timDisp As System.Windows.Forms.Timer
    Friend WithEvents lblStat11 As System.Windows.Forms.Label
    Friend WithEvents Label10 As System.Windows.Forms.Label
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents lblStat12 As System.Windows.Forms.Label
End Class
