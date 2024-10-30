<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPopResetOvl
    Inherits PopBaseForm

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
        Me.btnClearOffSert = New System.Windows.Forms.Button
        Me.cmbCam = New System.Windows.Forms.ComboBox
        Me.Label1 = New System.Windows.Forms.Label
        Me.Label2 = New System.Windows.Forms.Label
        Me.nudOffset = New System.Windows.Forms.NumericUpDown
        Me.btnSetOffSert = New System.Windows.Forms.Button
        Me.Label3 = New System.Windows.Forms.Label
        Me.lblNowOffset = New System.Windows.Forms.Label
        Me.btnAddOffSert = New System.Windows.Forms.Button
        CType(Me.nudOffset, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'btnClearOffSert
        '
        Me.btnClearOffSert.Location = New System.Drawing.Point(8, 60)
        Me.btnClearOffSert.Name = "btnClearOffSert"
        Me.btnClearOffSert.Size = New System.Drawing.Size(72, 24)
        Me.btnClearOffSert.TabIndex = 0
        Me.btnClearOffSert.Text = "補正無し"
        Me.btnClearOffSert.UseVisualStyleBackColor = True
        '
        'cmbCam
        '
        Me.cmbCam.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbCam.FormattingEnabled = True
        Me.cmbCam.Location = New System.Drawing.Point(80, 4)
        Me.cmbCam.Name = "cmbCam"
        Me.cmbCam.Size = New System.Drawing.Size(80, 20)
        Me.cmbCam.TabIndex = 1
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(8, 12)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(29, 12)
        Me.Label1.TabIndex = 2
        Me.Label1.Text = "対象"
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Location = New System.Drawing.Point(4, 36)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(59, 12)
        Me.Label2.TabIndex = 2
        Me.Label2.Text = "オフセット量"
        '
        'nudOffset
        '
        Me.nudOffset.Location = New System.Drawing.Point(80, 32)
        Me.nudOffset.Minimum = New Decimal(New Integer() {100, 0, 0, -2147483648})
        Me.nudOffset.Name = "nudOffset"
        Me.nudOffset.Size = New System.Drawing.Size(80, 19)
        Me.nudOffset.TabIndex = 3
        Me.nudOffset.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'btnSetOffSert
        '
        Me.btnSetOffSert.Location = New System.Drawing.Point(104, 60)
        Me.btnSetOffSert.Name = "btnSetOffSert"
        Me.btnSetOffSert.Size = New System.Drawing.Size(68, 24)
        Me.btnSetOffSert.TabIndex = 0
        Me.btnSetOffSert.Text = "直接反映"
        Me.btnSetOffSert.UseVisualStyleBackColor = True
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Location = New System.Drawing.Point(172, 16)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(83, 12)
        Me.Label3.TabIndex = 2
        Me.Label3.Text = "現在オフセット量"
        '
        'lblNowOffset
        '
        Me.lblNowOffset.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.lblNowOffset.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblNowOffset.Location = New System.Drawing.Point(184, 32)
        Me.lblNowOffset.Name = "lblNowOffset"
        Me.lblNowOffset.Size = New System.Drawing.Size(60, 20)
        Me.lblNowOffset.TabIndex = 2
        Me.lblNowOffset.Text = "0"
        Me.lblNowOffset.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'btnAddOffSert
        '
        Me.btnAddOffSert.Location = New System.Drawing.Point(176, 60)
        Me.btnAddOffSert.Name = "btnAddOffSert"
        Me.btnAddOffSert.Size = New System.Drawing.Size(68, 24)
        Me.btnAddOffSert.TabIndex = 0
        Me.btnAddOffSert.Text = "加算反映"
        Me.btnAddOffSert.UseVisualStyleBackColor = True
        '
        'frmPopResetOvl
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(258, 90)
        Me.Controls.Add(Me.nudOffset)
        Me.Controls.Add(Me.lblNowOffset)
        Me.Controls.Add(Me.Label3)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.cmbCam)
        Me.Controls.Add(Me.btnAddOffSert)
        Me.Controls.Add(Me.btnSetOffSert)
        Me.Controls.Add(Me.btnClearOffSert)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "frmPopResetOvl"
        Me.Text = "オーバーレイ手動補正"
        CType(Me.nudOffset, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents btnClearOffSert As System.Windows.Forms.Button
    Friend WithEvents cmbCam As System.Windows.Forms.ComboBox
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents nudOffset As System.Windows.Forms.NumericUpDown
    Friend WithEvents btnSetOffSert As System.Windows.Forms.Button
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents lblNowOffset As System.Windows.Forms.Label
    Friend WithEvents btnAddOffSert As System.Windows.Forms.Button
End Class
