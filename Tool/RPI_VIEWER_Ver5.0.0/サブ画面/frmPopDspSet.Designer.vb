<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPopDspSet
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
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.btnXoffset = New System.Windows.Forms.Button()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.lblXoffset = New System.Windows.Forms.Label()
        Me.Label14 = New System.Windows.Forms.Label()
        Me.trbXoffset = New System.Windows.Forms.TrackBar()
        Me.GroupBox2 = New System.Windows.Forms.GroupBox()
        Me.btnScal = New System.Windows.Forms.Button()
        Me.nudScalFsize = New System.Windows.Forms.NumericUpDown()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.nudScalStringY = New System.Windows.Forms.NumericUpDown()
        Me.nudScalStringX = New System.Windows.Forms.NumericUpDown()
        Me.nudScalCntY = New System.Windows.Forms.NumericUpDown()
        Me.nudScalCntX = New System.Windows.Forms.NumericUpDown()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.GroupBox3 = New System.Windows.Forms.GroupBox()
        Me.ckbTypeDsp = New System.Windows.Forms.CheckBox()
        Me.nudTypeFsize = New System.Windows.Forms.NumericUpDown()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.GroupBox1.SuspendLayout()
        CType(Me.trbXoffset, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox2.SuspendLayout()
        CType(Me.nudScalFsize, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nudScalStringY, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nudScalStringX, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nudScalCntY, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nudScalCntX, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox3.SuspendLayout()
        CType(Me.nudTypeFsize, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.btnXoffset)
        Me.GroupBox1.Controls.Add(Me.Label1)
        Me.GroupBox1.Controls.Add(Me.lblXoffset)
        Me.GroupBox1.Controls.Add(Me.Label14)
        Me.GroupBox1.Controls.Add(Me.trbXoffset)
        Me.GroupBox1.Location = New System.Drawing.Point(0, 0)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(228, 68)
        Me.GroupBox1.TabIndex = 0
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "X軸メモリズラシ量"
        '
        'btnXoffset
        '
        Me.btnXoffset.Location = New System.Drawing.Point(4, 20)
        Me.btnXoffset.Name = "btnXoffset"
        Me.btnXoffset.Size = New System.Drawing.Size(40, 20)
        Me.btnXoffset.TabIndex = 1
        Me.btnXoffset.Text = "初期"
        Me.btnXoffset.UseVisualStyleBackColor = True
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.Location = New System.Drawing.Point(24, 44)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(39, 15)
        Me.Label1.TabIndex = 2
        Me.Label1.Text = "-500"
        '
        'lblXoffset
        '
        Me.lblXoffset.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblXoffset.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblXoffset.Location = New System.Drawing.Point(180, 20)
        Me.lblXoffset.Name = "lblXoffset"
        Me.lblXoffset.Size = New System.Drawing.Size(44, 20)
        Me.lblXoffset.TabIndex = 2
        Me.lblXoffset.Text = "0"
        Me.lblXoffset.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'Label14
        '
        Me.Label14.AutoSize = True
        Me.Label14.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label14.Location = New System.Drawing.Point(168, 44)
        Me.Label14.Name = "Label14"
        Me.Label14.Size = New System.Drawing.Size(31, 15)
        Me.Label14.TabIndex = 2
        Me.Label14.Text = "500"
        '
        'trbXoffset
        '
        Me.trbXoffset.LargeChange = 10
        Me.trbXoffset.Location = New System.Drawing.Point(40, 16)
        Me.trbXoffset.Maximum = 500
        Me.trbXoffset.Minimum = -500
        Me.trbXoffset.Name = "trbXoffset"
        Me.trbXoffset.Size = New System.Drawing.Size(144, 42)
        Me.trbXoffset.TabIndex = 1
        Me.trbXoffset.TickFrequency = 50
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.btnScal)
        Me.GroupBox2.Controls.Add(Me.nudScalFsize)
        Me.GroupBox2.Controls.Add(Me.Label7)
        Me.GroupBox2.Controls.Add(Me.nudScalStringY)
        Me.GroupBox2.Controls.Add(Me.nudScalStringX)
        Me.GroupBox2.Controls.Add(Me.nudScalCntY)
        Me.GroupBox2.Controls.Add(Me.nudScalCntX)
        Me.GroupBox2.Controls.Add(Me.Label3)
        Me.GroupBox2.Controls.Add(Me.Label5)
        Me.GroupBox2.Controls.Add(Me.Label4)
        Me.GroupBox2.Controls.Add(Me.Label2)
        Me.GroupBox2.Location = New System.Drawing.Point(0, 72)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(228, 88)
        Me.GroupBox2.TabIndex = 1
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "メモリ変更"
        '
        'btnScal
        '
        Me.btnScal.Location = New System.Drawing.Point(176, 64)
        Me.btnScal.Name = "btnScal"
        Me.btnScal.Size = New System.Drawing.Size(44, 20)
        Me.btnScal.TabIndex = 4
        Me.btnScal.Text = "反映"
        Me.btnScal.UseVisualStyleBackColor = True
        '
        'nudScalFsize
        '
        Me.nudScalFsize.Location = New System.Drawing.Point(116, 64)
        Me.nudScalFsize.Maximum = New Decimal(New Integer() {60, 0, 0, 0})
        Me.nudScalFsize.Minimum = New Decimal(New Integer() {2, 0, 0, 0})
        Me.nudScalFsize.Name = "nudScalFsize"
        Me.nudScalFsize.Size = New System.Drawing.Size(52, 19)
        Me.nudScalFsize.TabIndex = 3
        Me.nudScalFsize.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudScalFsize.Value = New Decimal(New Integer() {16, 0, 0, 0})
        '
        'Label7
        '
        Me.Label7.AutoSize = True
        Me.Label7.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label7.Location = New System.Drawing.Point(8, 64)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(72, 15)
        Me.Label7.TabIndex = 2
        Me.Label7.Text = "文字サイズ"
        '
        'nudScalStringY
        '
        Me.nudScalStringY.Location = New System.Drawing.Point(172, 44)
        Me.nudScalStringY.Maximum = New Decimal(New Integer() {50, 0, 0, 0})
        Me.nudScalStringY.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nudScalStringY.Name = "nudScalStringY"
        Me.nudScalStringY.Size = New System.Drawing.Size(52, 19)
        Me.nudScalStringY.TabIndex = 3
        Me.nudScalStringY.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudScalStringY.Value = New Decimal(New Integer() {5, 0, 0, 0})
        '
        'nudScalStringX
        '
        Me.nudScalStringX.Location = New System.Drawing.Point(116, 44)
        Me.nudScalStringX.Maximum = New Decimal(New Integer() {50, 0, 0, 0})
        Me.nudScalStringX.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nudScalStringX.Name = "nudScalStringX"
        Me.nudScalStringX.Size = New System.Drawing.Size(52, 19)
        Me.nudScalStringX.TabIndex = 3
        Me.nudScalStringX.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudScalStringX.Value = New Decimal(New Integer() {2, 0, 0, 0})
        '
        'nudScalCntY
        '
        Me.nudScalCntY.Location = New System.Drawing.Point(172, 24)
        Me.nudScalCntY.Maximum = New Decimal(New Integer() {10000, 0, 0, 0})
        Me.nudScalCntY.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nudScalCntY.Name = "nudScalCntY"
        Me.nudScalCntY.Size = New System.Drawing.Size(52, 19)
        Me.nudScalCntY.TabIndex = 3
        Me.nudScalCntY.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudScalCntY.Value = New Decimal(New Integer() {100, 0, 0, 0})
        '
        'nudScalCntX
        '
        Me.nudScalCntX.Location = New System.Drawing.Point(116, 24)
        Me.nudScalCntX.Maximum = New Decimal(New Integer() {1000, 0, 0, 0})
        Me.nudScalCntX.Minimum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nudScalCntX.Name = "nudScalCntX"
        Me.nudScalCntX.Size = New System.Drawing.Size(52, 19)
        Me.nudScalCntX.TabIndex = 3
        Me.nudScalCntX.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudScalCntX.Value = New Decimal(New Integer() {100, 0, 0, 0})
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label3.Location = New System.Drawing.Point(8, 44)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(97, 15)
        Me.Label3.TabIndex = 2
        Me.Label3.Text = "文字間隔 [回]"
        '
        'Label5
        '
        Me.Label5.AutoSize = True
        Me.Label5.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label5.Location = New System.Drawing.Point(132, 8)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(31, 15)
        Me.Label5.TabIndex = 2
        Me.Label5.Text = "X軸"
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label4.Location = New System.Drawing.Point(180, 8)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(31, 15)
        Me.Label4.TabIndex = 2
        Me.Label4.Text = "Y軸"
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label2.Location = New System.Drawing.Point(8, 24)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(104, 15)
        Me.Label2.TabIndex = 2
        Me.Label2.Text = "メモリ間隔 [mm]"
        '
        'GroupBox3
        '
        Me.GroupBox3.Controls.Add(Me.ckbTypeDsp)
        Me.GroupBox3.Controls.Add(Me.nudTypeFsize)
        Me.GroupBox3.Controls.Add(Me.Label6)
        Me.GroupBox3.Location = New System.Drawing.Point(0, 168)
        Me.GroupBox3.Name = "GroupBox3"
        Me.GroupBox3.Size = New System.Drawing.Size(228, 44)
        Me.GroupBox3.TabIndex = 2
        Me.GroupBox3.TabStop = False
        Me.GroupBox3.Text = "疵名表示"
        '
        'ckbTypeDsp
        '
        Me.ckbTypeDsp.Appearance = System.Windows.Forms.Appearance.Button
        Me.ckbTypeDsp.Checked = True
        Me.ckbTypeDsp.CheckState = System.Windows.Forms.CheckState.Checked
        Me.ckbTypeDsp.Location = New System.Drawing.Point(8, 16)
        Me.ckbTypeDsp.Name = "ckbTypeDsp"
        Me.ckbTypeDsp.Size = New System.Drawing.Size(64, 24)
        Me.ckbTypeDsp.TabIndex = 0
        Me.ckbTypeDsp.Text = "疵名表示"
        Me.ckbTypeDsp.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.ckbTypeDsp.UseVisualStyleBackColor = True
        '
        'nudTypeFsize
        '
        Me.nudTypeFsize.Location = New System.Drawing.Point(172, 16)
        Me.nudTypeFsize.Maximum = New Decimal(New Integer() {50, 0, 0, 0})
        Me.nudTypeFsize.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nudTypeFsize.Name = "nudTypeFsize"
        Me.nudTypeFsize.Size = New System.Drawing.Size(52, 19)
        Me.nudTypeFsize.TabIndex = 3
        Me.nudTypeFsize.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudTypeFsize.Value = New Decimal(New Integer() {12, 0, 0, 0})
        '
        'Label6
        '
        Me.Label6.AutoSize = True
        Me.Label6.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label6.Location = New System.Drawing.Point(96, 16)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(72, 15)
        Me.Label6.TabIndex = 2
        Me.Label6.Text = "文字サイズ"
        '
        'frmPopDspSet
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(230, 214)
        Me.Controls.Add(Me.GroupBox3)
        Me.Controls.Add(Me.GroupBox2)
        Me.Controls.Add(Me.GroupBox1)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "frmPopDspSet"
        Me.Text = "表示設定"
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox1.PerformLayout()
        CType(Me.trbXoffset, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox2.ResumeLayout(False)
        Me.GroupBox2.PerformLayout()
        CType(Me.nudScalFsize, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nudScalStringY, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nudScalStringX, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nudScalCntY, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nudScalCntX, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox3.ResumeLayout(False)
        Me.GroupBox3.PerformLayout()
        CType(Me.nudTypeFsize, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents trbXoffset As System.Windows.Forms.TrackBar
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Label14 As System.Windows.Forms.Label
    Friend WithEvents lblXoffset As System.Windows.Forms.Label
    Friend WithEvents btnXoffset As System.Windows.Forms.Button
    Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents nudScalCntY As System.Windows.Forms.NumericUpDown
    Friend WithEvents nudScalCntX As System.Windows.Forms.NumericUpDown
    Friend WithEvents btnScal As System.Windows.Forms.Button
    Friend WithEvents nudScalStringY As System.Windows.Forms.NumericUpDown
    Friend WithEvents nudScalStringX As System.Windows.Forms.NumericUpDown
    Friend WithEvents GroupBox3 As System.Windows.Forms.GroupBox
    Friend WithEvents ckbTypeDsp As System.Windows.Forms.CheckBox
    Friend WithEvents nudTypeFsize As System.Windows.Forms.NumericUpDown
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents nudScalFsize As System.Windows.Forms.NumericUpDown
    Friend WithEvents Label7 As System.Windows.Forms.Label
End Class
