<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPopKaihatu
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
        Me.GroupBox1 = New System.Windows.Forms.GroupBox
        Me.radQuarity2 = New System.Windows.Forms.RadioButton
        Me.radQuarity1 = New System.Windows.Forms.RadioButton
        Me.GroupBox2 = New System.Windows.Forms.GroupBox
        Me.radDraw2 = New System.Windows.Forms.RadioButton
        Me.radDraw1 = New System.Windows.Forms.RadioButton
        Me.GroupBox1.SuspendLayout()
        Me.GroupBox2.SuspendLayout()
        Me.SuspendLayout()
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.radQuarity2)
        Me.GroupBox1.Controls.Add(Me.radQuarity1)
        Me.GroupBox1.Location = New System.Drawing.Point(4, 0)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(140, 44)
        Me.GroupBox1.TabIndex = 0
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "品質"
        '
        'radQuarity2
        '
        Me.radQuarity2.Appearance = System.Windows.Forms.Appearance.Button
        Me.radQuarity2.Location = New System.Drawing.Point(72, 16)
        Me.radQuarity2.Name = "radQuarity2"
        Me.radQuarity2.Size = New System.Drawing.Size(60, 20)
        Me.radQuarity2.TabIndex = 0
        Me.radQuarity2.TabStop = True
        Me.radQuarity2.Text = "高画質"
        Me.radQuarity2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.radQuarity2.UseVisualStyleBackColor = True
        '
        'radQuarity1
        '
        Me.radQuarity1.Appearance = System.Windows.Forms.Appearance.Button
        Me.radQuarity1.Location = New System.Drawing.Point(8, 16)
        Me.radQuarity1.Name = "radQuarity1"
        Me.radQuarity1.Size = New System.Drawing.Size(60, 20)
        Me.radQuarity1.TabIndex = 0
        Me.radQuarity1.TabStop = True
        Me.radQuarity1.Text = "低画質"
        Me.radQuarity1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.radQuarity1.UseVisualStyleBackColor = True
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.radDraw2)
        Me.GroupBox2.Controls.Add(Me.radDraw1)
        Me.GroupBox2.Location = New System.Drawing.Point(4, 52)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(140, 44)
        Me.GroupBox2.TabIndex = 0
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "描画方法"
        '
        'radDraw2
        '
        Me.radDraw2.Appearance = System.Windows.Forms.Appearance.Button
        Me.radDraw2.Location = New System.Drawing.Point(72, 16)
        Me.radDraw2.Name = "radDraw2"
        Me.radDraw2.Size = New System.Drawing.Size(60, 20)
        Me.radDraw2.TabIndex = 0
        Me.radDraw2.TabStop = True
        Me.radDraw2.Text = "ＩＰＰ"
        Me.radDraw2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.radDraw2.UseVisualStyleBackColor = True
        '
        'radDraw1
        '
        Me.radDraw1.Appearance = System.Windows.Forms.Appearance.Button
        Me.radDraw1.Location = New System.Drawing.Point(8, 16)
        Me.radDraw1.Name = "radDraw1"
        Me.radDraw1.Size = New System.Drawing.Size(60, 20)
        Me.radDraw1.TabIndex = 0
        Me.radDraw1.TabStop = True
        Me.radDraw1.Text = "Win標準"
        Me.radDraw1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.radDraw1.UseVisualStyleBackColor = True
        '
        'frmPopKaihatu
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(148, 104)
        Me.Controls.Add(Me.GroupBox2)
        Me.Controls.Add(Me.GroupBox1)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "frmPopKaihatu"
        Me.Text = "開発用"
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox2.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents radQuarity2 As System.Windows.Forms.RadioButton
    Friend WithEvents radQuarity1 As System.Windows.Forms.RadioButton
    Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
    Friend WithEvents radDraw2 As System.Windows.Forms.RadioButton
    Friend WithEvents radDraw1 As System.Windows.Forms.RadioButton
End Class
