<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPopGridLine
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
        Me.Label1 = New System.Windows.Forms.Label()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.nudY = New System.Windows.Forms.NumericUpDown()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.nudX = New System.Windows.Forms.NumericUpDown()
        Me.btnReLoad = New System.Windows.Forms.Button()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.Label8 = New System.Windows.Forms.Label()
        Me.trbOffsetY = New System.Windows.Forms.TrackBar()
        Me.trbOffsetX = New System.Windows.Forms.TrackBar()
        CType(Me.nudY, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nudX, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.trbOffsetY, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.trbOffsetX, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(0, 4)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(53, 12)
        Me.Label1.TabIndex = 0
        Me.Label1.Text = "長手間隔"
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Location = New System.Drawing.Point(12, 24)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(41, 12)
        Me.Label2.TabIndex = 0
        Me.Label2.Text = "幅間隔"
        '
        'nudY
        '
        Me.nudY.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nudY.Location = New System.Drawing.Point(52, 0)
        Me.nudY.Maximum = New Decimal(New Integer() {1000000, 0, 0, 0})
        Me.nudY.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nudY.Name = "nudY"
        Me.nudY.Size = New System.Drawing.Size(92, 19)
        Me.nudY.TabIndex = 1
        Me.nudY.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudY.Value = New Decimal(New Integer() {100, 0, 0, 0})
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Location = New System.Drawing.Point(144, 4)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(31, 12)
        Me.Label3.TabIndex = 0
        Me.Label3.Text = "[mm]"
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Location = New System.Drawing.Point(144, 24)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(31, 12)
        Me.Label4.TabIndex = 0
        Me.Label4.Text = "[mm]"
        '
        'nudX
        '
        Me.nudX.Increment = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nudX.Location = New System.Drawing.Point(52, 20)
        Me.nudX.Maximum = New Decimal(New Integer() {1000000, 0, 0, 0})
        Me.nudX.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nudX.Name = "nudX"
        Me.nudX.Size = New System.Drawing.Size(92, 19)
        Me.nudX.TabIndex = 1
        Me.nudX.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudX.Value = New Decimal(New Integer() {100, 0, 0, 0})
        '
        'btnReLoad
        '
        Me.btnReLoad.Location = New System.Drawing.Point(180, 4)
        Me.btnReLoad.Name = "btnReLoad"
        Me.btnReLoad.Size = New System.Drawing.Size(24, 36)
        Me.btnReLoad.TabIndex = 2
        Me.btnReLoad.Text = "初期"
        Me.btnReLoad.UseVisualStyleBackColor = True
        '
        'Label5
        '
        Me.Label5.AutoSize = True
        Me.Label5.Location = New System.Drawing.Point(0, 52)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(54, 12)
        Me.Label5.TabIndex = 0
        Me.Label5.Text = "長手ｽﾞﾗｼ"
        '
        'Label6
        '
        Me.Label6.AutoSize = True
        Me.Label6.Location = New System.Drawing.Point(176, 52)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(31, 12)
        Me.Label6.TabIndex = 0
        Me.Label6.Text = "[mm]"
        '
        'Label7
        '
        Me.Label7.AutoSize = True
        Me.Label7.Location = New System.Drawing.Point(12, 96)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(42, 12)
        Me.Label7.TabIndex = 0
        Me.Label7.Text = "幅ｽﾞﾗｼ"
        '
        'Label8
        '
        Me.Label8.AutoSize = True
        Me.Label8.Location = New System.Drawing.Point(176, 96)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(31, 12)
        Me.Label8.TabIndex = 0
        Me.Label8.Text = "[mm]"
        '
        'trbOffsetY
        '
        Me.trbOffsetY.AutoSize = False
        Me.trbOffsetY.LargeChange = 1
        Me.trbOffsetY.Location = New System.Drawing.Point(48, 48)
        Me.trbOffsetY.Maximum = 2000
        Me.trbOffsetY.Name = "trbOffsetY"
        Me.trbOffsetY.Size = New System.Drawing.Size(136, 32)
        Me.trbOffsetY.SmallChange = 10
        Me.trbOffsetY.TabIndex = 3
        Me.trbOffsetY.TickFrequency = 200
        '
        'trbOffsetX
        '
        Me.trbOffsetX.AutoSize = False
        Me.trbOffsetX.LargeChange = 1
        Me.trbOffsetX.Location = New System.Drawing.Point(48, 84)
        Me.trbOffsetX.Maximum = 500
        Me.trbOffsetX.Name = "trbOffsetX"
        Me.trbOffsetX.Size = New System.Drawing.Size(136, 32)
        Me.trbOffsetX.SmallChange = 10
        Me.trbOffsetX.TabIndex = 3
        Me.trbOffsetX.TickFrequency = 50
        '
        'frmPopGridLine
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(208, 118)
        Me.Controls.Add(Me.Label6)
        Me.Controls.Add(Me.Label5)
        Me.Controls.Add(Me.trbOffsetY)
        Me.Controls.Add(Me.btnReLoad)
        Me.Controls.Add(Me.nudX)
        Me.Controls.Add(Me.Label8)
        Me.Controls.Add(Me.nudY)
        Me.Controls.Add(Me.Label4)
        Me.Controls.Add(Me.Label7)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.Label3)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.trbOffsetX)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "frmPopGridLine"
        Me.Text = "グリッド線"
        CType(Me.nudY, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nudX, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.trbOffsetY, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.trbOffsetX, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents nudY As System.Windows.Forms.NumericUpDown
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents nudX As System.Windows.Forms.NumericUpDown
    Friend WithEvents btnReLoad As System.Windows.Forms.Button
    Friend WithEvents Label5 As System.Windows.Forms.Label
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents Label8 As System.Windows.Forms.Label
    Friend WithEvents trbOffsetY As System.Windows.Forms.TrackBar
    Friend WithEvents trbOffsetX As System.Windows.Forms.TrackBar
End Class
