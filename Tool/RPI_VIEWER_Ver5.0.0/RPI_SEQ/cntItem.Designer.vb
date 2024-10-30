<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class cntItem
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
        Me.Label1 = New System.Windows.Forms.Label
        Me.txtRpiPath = New System.Windows.Forms.TextBox
        Me.btnExec = New System.Windows.Forms.Button
        Me.lblNo = New System.Windows.Forms.Label
        Me.nudOffsetY = New System.Windows.Forms.NumericUpDown
        Me.Label2 = New System.Windows.Forms.Label
        Me.nudOffsetX = New System.Windows.Forms.NumericUpDown
        Me.Label3 = New System.Windows.Forms.Label
        Me.nudRateY = New System.Windows.Forms.NumericUpDown
        Me.Label4 = New System.Windows.Forms.Label
        Me.txtValY = New System.Windows.Forms.TextBox
        Me.txtValX = New System.Windows.Forms.TextBox
        Me.ckbTurn_Top = New System.Windows.Forms.CheckBox
        Me.ckbTurn_Left = New System.Windows.Forms.CheckBox
        CType(Me.nudOffsetY, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nudOffsetX, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nudRateY, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(28, 8)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(57, 24)
        Me.Label1.TabIndex = 0
        Me.Label1.Text = "RPIファイル" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "フルパス"
        '
        'txtRpiPath
        '
        Me.txtRpiPath.Location = New System.Drawing.Point(84, 4)
        Me.txtRpiPath.Multiline = True
        Me.txtRpiPath.Name = "txtRpiPath"
        Me.txtRpiPath.ScrollBars = System.Windows.Forms.ScrollBars.Vertical
        Me.txtRpiPath.Size = New System.Drawing.Size(276, 36)
        Me.txtRpiPath.TabIndex = 1
        '
        'btnExec
        '
        Me.btnExec.Location = New System.Drawing.Point(360, 4)
        Me.btnExec.Name = "btnExec"
        Me.btnExec.Size = New System.Drawing.Size(80, 36)
        Me.btnExec.TabIndex = 2
        Me.btnExec.Text = "RPI_VIEWER" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "起動"
        Me.btnExec.UseVisualStyleBackColor = True
        '
        'lblNo
        '
        Me.lblNo.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblNo.Dock = System.Windows.Forms.DockStyle.Left
        Me.lblNo.Location = New System.Drawing.Point(0, 0)
        Me.lblNo.Name = "lblNo"
        Me.lblNo.Size = New System.Drawing.Size(24, 68)
        Me.lblNo.TabIndex = 0
        Me.lblNo.Text = "99"
        Me.lblNo.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'nudOffsetY
        '
        Me.nudOffsetY.Location = New System.Drawing.Point(624, 24)
        Me.nudOffsetY.Maximum = New Decimal(New Integer() {100000, 0, 0, 0})
        Me.nudOffsetY.Minimum = New Decimal(New Integer() {100000, 0, 0, -2147483648})
        Me.nudOffsetY.Name = "nudOffsetY"
        Me.nudOffsetY.Size = New System.Drawing.Size(84, 19)
        Me.nudOffsetY.TabIndex = 3
        Me.nudOffsetY.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Location = New System.Drawing.Point(452, 28)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(82, 12)
        Me.Label2.TabIndex = 4
        Me.Label2.Text = "ｵﾌｾｯﾄX,Y[mm]"
        '
        'nudOffsetX
        '
        Me.nudOffsetX.Location = New System.Drawing.Point(536, 24)
        Me.nudOffsetX.Maximum = New Decimal(New Integer() {100000, 0, 0, 0})
        Me.nudOffsetX.Minimum = New Decimal(New Integer() {100000, 0, 0, -2147483648})
        Me.nudOffsetX.Name = "nudOffsetX"
        Me.nudOffsetX.Size = New System.Drawing.Size(84, 19)
        Me.nudOffsetX.TabIndex = 3
        Me.nudOffsetX.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Location = New System.Drawing.Point(452, 8)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(154, 12)
        Me.Label3.TabIndex = 4
        Me.Label3.Text = "No0(基準ﾗｲﾝ)からの伸び率[%]"
        '
        'nudRateY
        '
        Me.nudRateY.DecimalPlaces = 6
        Me.nudRateY.Increment = New Decimal(New Integer() {1, 0, 0, 65536})
        Me.nudRateY.Location = New System.Drawing.Point(624, 4)
        Me.nudRateY.Minimum = New Decimal(New Integer() {1, 0, 0, 65536})
        Me.nudRateY.Name = "nudRateY"
        Me.nudRateY.Size = New System.Drawing.Size(84, 19)
        Me.nudRateY.TabIndex = 3
        Me.nudRateY.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudRateY.Value = New Decimal(New Integer() {1, 0, 0, 0})
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Location = New System.Drawing.Point(452, 48)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(83, 12)
        Me.Label4.TabIndex = 4
        Me.Label4.Text = "現在値X,Y[mm]"
        '
        'txtValY
        '
        Me.txtValY.Location = New System.Drawing.Point(624, 44)
        Me.txtValY.Name = "txtValY"
        Me.txtValY.ReadOnly = True
        Me.txtValY.Size = New System.Drawing.Size(84, 19)
        Me.txtValY.TabIndex = 5
        Me.txtValY.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'txtValX
        '
        Me.txtValX.Location = New System.Drawing.Point(536, 44)
        Me.txtValX.Name = "txtValX"
        Me.txtValX.ReadOnly = True
        Me.txtValX.Size = New System.Drawing.Size(84, 19)
        Me.txtValX.TabIndex = 5
        Me.txtValX.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        '
        'ckbTurn_Top
        '
        Me.ckbTurn_Top.Appearance = System.Windows.Forms.Appearance.Button
        Me.ckbTurn_Top.AutoSize = True
        Me.ckbTurn_Top.Location = New System.Drawing.Point(84, 44)
        Me.ckbTurn_Top.Name = "ckbTurn_Top"
        Me.ckbTurn_Top.Size = New System.Drawing.Size(63, 22)
        Me.ckbTurn_Top.TabIndex = 6
        Me.ckbTurn_Top.Text = "上下反転"
        Me.ckbTurn_Top.UseVisualStyleBackColor = True
        '
        'ckbTurn_Left
        '
        Me.ckbTurn_Left.Appearance = System.Windows.Forms.Appearance.Button
        Me.ckbTurn_Left.AutoSize = True
        Me.ckbTurn_Left.Location = New System.Drawing.Point(152, 44)
        Me.ckbTurn_Left.Name = "ckbTurn_Left"
        Me.ckbTurn_Left.Size = New System.Drawing.Size(63, 22)
        Me.ckbTurn_Left.TabIndex = 6
        Me.ckbTurn_Left.Text = "左右反転"
        Me.ckbTurn_Left.UseVisualStyleBackColor = True
        '
        'cntItem
        '
        Me.AllowDrop = True
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Controls.Add(Me.ckbTurn_Left)
        Me.Controls.Add(Me.ckbTurn_Top)
        Me.Controls.Add(Me.txtValX)
        Me.Controls.Add(Me.txtValY)
        Me.Controls.Add(Me.Label4)
        Me.Controls.Add(Me.Label3)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.nudOffsetX)
        Me.Controls.Add(Me.nudRateY)
        Me.Controls.Add(Me.nudOffsetY)
        Me.Controls.Add(Me.btnExec)
        Me.Controls.Add(Me.txtRpiPath)
        Me.Controls.Add(Me.lblNo)
        Me.Controls.Add(Me.Label1)
        Me.Name = "cntItem"
        Me.Size = New System.Drawing.Size(710, 68)
        CType(Me.nudOffsetY, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nudOffsetX, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nudRateY, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub
    Friend WithEvents txtRpiPath As System.Windows.Forms.TextBox
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents btnExec As System.Windows.Forms.Button
    Friend WithEvents lblNo As System.Windows.Forms.Label
    Friend WithEvents nudOffsetY As System.Windows.Forms.NumericUpDown
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents nudOffsetX As System.Windows.Forms.NumericUpDown
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents nudRateY As System.Windows.Forms.NumericUpDown
    Friend WithEvents Label4 As System.Windows.Forms.Label
    Friend WithEvents txtValY As System.Windows.Forms.TextBox
    Friend WithEvents txtValX As System.Windows.Forms.TextBox
    Friend WithEvents ckbTurn_Top As System.Windows.Forms.CheckBox
    Friend WithEvents ckbTurn_Left As System.Windows.Forms.CheckBox

End Class
