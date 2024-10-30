<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPopPrint
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
        Me.btnPrintH = New System.Windows.Forms.Button
        Me.btnPrintP = New System.Windows.Forms.Button
        Me.rdbPrint2 = New System.Windows.Forms.RadioButton
        Me.rdbPrint1 = New System.Windows.Forms.RadioButton
        Me.GroupBox2 = New System.Windows.Forms.GroupBox
        Me.Label1 = New System.Windows.Forms.Label
        Me.btnSaveBin = New System.Windows.Forms.Button
        Me.btnSaveRawAsBmp = New System.Windows.Forms.Button
        Me.btnSaveDisplay = New System.Windows.Forms.Button
        Me.GroupBox1.SuspendLayout()
        Me.GroupBox2.SuspendLayout()
        Me.SuspendLayout()
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.btnPrintH)
        Me.GroupBox1.Controls.Add(Me.btnPrintP)
        Me.GroupBox1.Controls.Add(Me.rdbPrint2)
        Me.GroupBox1.Controls.Add(Me.rdbPrint1)
        Me.GroupBox1.Location = New System.Drawing.Point(4, 4)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(308, 52)
        Me.GroupBox1.TabIndex = 0
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "ハードコピー 兼 印刷"
        '
        'btnPrintH
        '
        Me.btnPrintH.Location = New System.Drawing.Point(220, 20)
        Me.btnPrintH.Name = "btnPrintH"
        Me.btnPrintH.Size = New System.Drawing.Size(76, 24)
        Me.btnPrintH.TabIndex = 1
        Me.btnPrintH.Text = "ハードコピー"
        Me.btnPrintH.UseVisualStyleBackColor = True
        '
        'btnPrintP
        '
        Me.btnPrintP.Location = New System.Drawing.Point(144, 20)
        Me.btnPrintP.Name = "btnPrintP"
        Me.btnPrintP.Size = New System.Drawing.Size(72, 24)
        Me.btnPrintP.TabIndex = 1
        Me.btnPrintP.Text = "印刷"
        Me.btnPrintP.UseVisualStyleBackColor = True
        '
        'rdbPrint2
        '
        Me.rdbPrint2.AutoSize = True
        Me.rdbPrint2.Checked = True
        Me.rdbPrint2.Location = New System.Drawing.Point(64, 24)
        Me.rdbPrint2.Name = "rdbPrint2"
        Me.rdbPrint2.Size = New System.Drawing.Size(49, 16)
        Me.rdbPrint2.TabIndex = 0
        Me.rdbPrint2.TabStop = True
        Me.rdbPrint2.Text = "A4横"
        Me.rdbPrint2.UseVisualStyleBackColor = True
        '
        'rdbPrint1
        '
        Me.rdbPrint1.AutoSize = True
        Me.rdbPrint1.Location = New System.Drawing.Point(8, 24)
        Me.rdbPrint1.Name = "rdbPrint1"
        Me.rdbPrint1.Size = New System.Drawing.Size(49, 16)
        Me.rdbPrint1.TabIndex = 0
        Me.rdbPrint1.Text = "A4縦"
        Me.rdbPrint1.UseVisualStyleBackColor = True
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.Label1)
        Me.GroupBox2.Controls.Add(Me.btnSaveBin)
        Me.GroupBox2.Controls.Add(Me.btnSaveRawAsBmp)
        Me.GroupBox2.Controls.Add(Me.btnSaveDisplay)
        Me.GroupBox2.Location = New System.Drawing.Point(4, 60)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(308, 68)
        Me.GroupBox2.TabIndex = 1
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "画像出力(表示エリアの全フレーム画像)"
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(12, 48)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(283, 12)
        Me.Label1.TabIndex = 2
        Me.Label1.Text = "※ 全ﾌﾚｰﾑ読込完了前だと正しいﾌﾚｰﾑが出力されません"
        '
        'btnSaveBin
        '
        Me.btnSaveBin.Location = New System.Drawing.Point(196, 20)
        Me.btnSaveBin.Name = "btnSaveBin"
        Me.btnSaveBin.Size = New System.Drawing.Size(108, 24)
        Me.btnSaveBin.TabIndex = 1
        Me.btnSaveBin.Text = "バイナリデータ保存"
        Me.btnSaveBin.UseVisualStyleBackColor = True
        '
        'btnSaveBmp
        '
        Me.btnSaveRawAsBmp.Location = New System.Drawing.Point(96, 20)
        Me.btnSaveRawAsBmp.Name = "btnSaveBmp"
        Me.btnSaveRawAsBmp.Size = New System.Drawing.Size(96, 24)
        Me.btnSaveRawAsBmp.TabIndex = 1
        Me.btnSaveRawAsBmp.Text = "生画像保存"
        Me.btnSaveRawAsBmp.UseVisualStyleBackColor = True
        '
        'btnSaveJpeg
        '
        Me.btnSaveDisplay.Location = New System.Drawing.Point(4, 20)
        Me.btnSaveDisplay.Name = "btnSaveJpeg"
        Me.btnSaveDisplay.Size = New System.Drawing.Size(88, 24)
        Me.btnSaveDisplay.TabIndex = 1
        Me.btnSaveDisplay.Text = "表示画像保存"
        Me.btnSaveDisplay.UseVisualStyleBackColor = True
        '
        'frmPopPrint
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(316, 132)
        Me.Controls.Add(Me.GroupBox2)
        Me.Controls.Add(Me.GroupBox1)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "frmPopPrint"
        Me.Text = "出力設定"
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox1.PerformLayout()
        Me.GroupBox2.ResumeLayout(False)
        Me.GroupBox2.PerformLayout()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents rdbPrint2 As System.Windows.Forms.RadioButton
    Friend WithEvents rdbPrint1 As System.Windows.Forms.RadioButton
    Friend WithEvents btnPrintP As System.Windows.Forms.Button
    Friend WithEvents btnPrintH As System.Windows.Forms.Button
    Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
    Friend WithEvents btnSaveDisplay As System.Windows.Forms.Button
    Friend WithEvents btnSaveRawAsBmp As System.Windows.Forms.Button
    Friend WithEvents btnSaveBin As System.Windows.Forms.Button
    Friend WithEvents Label1 As System.Windows.Forms.Label
End Class
