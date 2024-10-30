<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()>
Partial Class frmPopHotRoll
    Inherits System.Windows.Forms.Form

    'フォームがコンポーネントの一覧をクリーンアップするために dispose をオーバーライドします。
    <System.Diagnostics.DebuggerNonUserCode()>
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Windows フォーム デザイナーで必要です。
    Private components As System.ComponentModel.IContainer

    'メモ: 以下のプロシージャは Windows フォーム デザイナーで必要です。
    'Windows フォーム デザイナーを使用して変更できます。  
    'コード エディターを使って変更しないでください。
    <System.Diagnostics.DebuggerStepThrough()>
    Private Sub InitializeComponent()
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(frmPopHotRoll))
        Me.CheckBox1 = New System.Windows.Forms.CheckBox()
        Me.CheckBox2 = New System.Windows.Forms.CheckBox()
        Me.CheckBox3 = New System.Windows.Forms.CheckBox()
        Me.CheckBox4 = New System.Windows.Forms.CheckBox()
        Me.CheckBox5 = New System.Windows.Forms.CheckBox()
        Me.CheckBox6 = New System.Windows.Forms.CheckBox()
        Me.CheckBox7 = New System.Windows.Forms.CheckBox()
        Me.btnForward = New System.Windows.Forms.Button()
        Me.btnBackward = New System.Windows.Forms.Button()
        Me.LabelIcon = New System.Windows.Forms.Label()
        Me.LabelIconForDraw = New System.Windows.Forms.Label()
        Me.btnChageY_Init = New System.Windows.Forms.Button()
        Me.btnChageY_Dev = New System.Windows.Forms.Button()
        Me.nudResY_Dev = New System.Windows.Forms.NumericUpDown()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.lblBaseResY = New System.Windows.Forms.Label()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.lblNowResY = New System.Windows.Forms.Label()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.chkContrast = New System.Windows.Forms.CheckBox()
        Me.GroupBox2 = New System.Windows.Forms.GroupBox()
        Me.trbContrast = New System.Windows.Forms.TrackBar()
        Me.Label11 = New System.Windows.Forms.Label()
        Me.Label10 = New System.Windows.Forms.Label()
        Me.Label9 = New System.Windows.Forms.Label()
        Me.GroupBox3 = New System.Windows.Forms.GroupBox()
        Me.trbBright = New System.Windows.Forms.TrackBar()
        Me.Label8 = New System.Windows.Forms.Label()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.lblContrast = New System.Windows.Forms.Label()
        Me.lblBright = New System.Windows.Forms.Label()
        Me.GroupBox5 = New System.Windows.Forms.GroupBox()
        CType(Me.nudResY_Dev, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox2.SuspendLayout()
        CType(Me.trbContrast, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox3.SuspendLayout()
        CType(Me.trbBright, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox5.SuspendLayout()
        Me.SuspendLayout()
        '
        'CheckBox1
        '
        Me.CheckBox1.Appearance = System.Windows.Forms.Appearance.Button
        Me.CheckBox1.AutoSize = True
        Me.CheckBox1.Font = New System.Drawing.Font("Calibri", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.CheckBox1.Location = New System.Drawing.Point(18, 6)
        Me.CheckBox1.MinimumSize = New System.Drawing.Size(66, 0)
        Me.CheckBox1.Name = "CheckBox1"
        Me.CheckBox1.Size = New System.Drawing.Size(66, 33)
        Me.CheckBox1.TabIndex = 0
        Me.CheckBox1.Text = "F1"
        Me.CheckBox1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.CheckBox1.UseVisualStyleBackColor = True
        '
        'CheckBox2
        '
        Me.CheckBox2.Appearance = System.Windows.Forms.Appearance.Button
        Me.CheckBox2.AutoSize = True
        Me.CheckBox2.Font = New System.Drawing.Font("Calibri", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.CheckBox2.Location = New System.Drawing.Point(90, 6)
        Me.CheckBox2.MinimumSize = New System.Drawing.Size(66, 0)
        Me.CheckBox2.Name = "CheckBox2"
        Me.CheckBox2.Size = New System.Drawing.Size(66, 33)
        Me.CheckBox2.TabIndex = 0
        Me.CheckBox2.Text = "F2"
        Me.CheckBox2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.CheckBox2.UseVisualStyleBackColor = True
        '
        'CheckBox3
        '
        Me.CheckBox3.Appearance = System.Windows.Forms.Appearance.Button
        Me.CheckBox3.AutoSize = True
        Me.CheckBox3.Font = New System.Drawing.Font("Calibri", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.CheckBox3.Location = New System.Drawing.Point(162, 6)
        Me.CheckBox3.MinimumSize = New System.Drawing.Size(66, 0)
        Me.CheckBox3.Name = "CheckBox3"
        Me.CheckBox3.Size = New System.Drawing.Size(66, 33)
        Me.CheckBox3.TabIndex = 0
        Me.CheckBox3.Text = "F3"
        Me.CheckBox3.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.CheckBox3.UseVisualStyleBackColor = True
        '
        'CheckBox4
        '
        Me.CheckBox4.Appearance = System.Windows.Forms.Appearance.Button
        Me.CheckBox4.AutoSize = True
        Me.CheckBox4.Font = New System.Drawing.Font("Calibri", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.CheckBox4.Location = New System.Drawing.Point(234, 6)
        Me.CheckBox4.MinimumSize = New System.Drawing.Size(66, 0)
        Me.CheckBox4.Name = "CheckBox4"
        Me.CheckBox4.Size = New System.Drawing.Size(66, 33)
        Me.CheckBox4.TabIndex = 0
        Me.CheckBox4.Text = "F4"
        Me.CheckBox4.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.CheckBox4.UseVisualStyleBackColor = True
        '
        'CheckBox5
        '
        Me.CheckBox5.Appearance = System.Windows.Forms.Appearance.Button
        Me.CheckBox5.AutoSize = True
        Me.CheckBox5.Font = New System.Drawing.Font("Calibri", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.CheckBox5.Location = New System.Drawing.Point(306, 6)
        Me.CheckBox5.MinimumSize = New System.Drawing.Size(66, 0)
        Me.CheckBox5.Name = "CheckBox5"
        Me.CheckBox5.Size = New System.Drawing.Size(66, 33)
        Me.CheckBox5.TabIndex = 0
        Me.CheckBox5.Text = "F5"
        Me.CheckBox5.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.CheckBox5.UseVisualStyleBackColor = True
        '
        'CheckBox6
        '
        Me.CheckBox6.Appearance = System.Windows.Forms.Appearance.Button
        Me.CheckBox6.AutoSize = True
        Me.CheckBox6.Font = New System.Drawing.Font("Calibri", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.CheckBox6.Location = New System.Drawing.Point(378, 6)
        Me.CheckBox6.MinimumSize = New System.Drawing.Size(66, 0)
        Me.CheckBox6.Name = "CheckBox6"
        Me.CheckBox6.Size = New System.Drawing.Size(66, 33)
        Me.CheckBox6.TabIndex = 0
        Me.CheckBox6.Text = "F6"
        Me.CheckBox6.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.CheckBox6.UseVisualStyleBackColor = True
        '
        'CheckBox7
        '
        Me.CheckBox7.Appearance = System.Windows.Forms.Appearance.Button
        Me.CheckBox7.AutoSize = True
        Me.CheckBox7.Font = New System.Drawing.Font("Calibri", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.CheckBox7.Location = New System.Drawing.Point(448, 6)
        Me.CheckBox7.MinimumSize = New System.Drawing.Size(66, 0)
        Me.CheckBox7.Name = "CheckBox7"
        Me.CheckBox7.Size = New System.Drawing.Size(66, 33)
        Me.CheckBox7.TabIndex = 0
        Me.CheckBox7.Text = "F7"
        Me.CheckBox7.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.CheckBox7.UseVisualStyleBackColor = True
        '
        'btnForward
        '
        Me.btnForward.Font = New System.Drawing.Font("MS UI Gothic", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnForward.Location = New System.Drawing.Point(18, 45)
        Me.btnForward.Name = "btnForward"
        Me.btnForward.Size = New System.Drawing.Size(210, 35)
        Me.btnForward.TabIndex = 1
        Me.btnForward.Text = "順方向＋"
        Me.btnForward.UseVisualStyleBackColor = True
        '
        'btnBackward
        '
        Me.btnBackward.Font = New System.Drawing.Font("MS UI Gothic", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnBackward.Location = New System.Drawing.Point(304, 45)
        Me.btnBackward.Name = "btnBackward"
        Me.btnBackward.Size = New System.Drawing.Size(210, 35)
        Me.btnBackward.TabIndex = 1
        Me.btnBackward.Text = "逆方向－"
        Me.btnBackward.UseVisualStyleBackColor = True
        '
        'LabelIcon
        '
        Me.LabelIcon.AutoSize = True
        Me.LabelIcon.Image = CType(resources.GetObject("LabelIcon.Image"), System.Drawing.Image)
        Me.LabelIcon.Location = New System.Drawing.Point(264, 45)
        Me.LabelIcon.Name = "LabelIcon"
        Me.LabelIcon.Size = New System.Drawing.Size(17, 12)
        Me.LabelIcon.TabIndex = 3
        Me.LabelIcon.Text = "   "
        Me.LabelIcon.Visible = False
        '
        'LabelIconForDraw
        '
        Me.LabelIconForDraw.AutoSize = True
        Me.LabelIconForDraw.Image = CType(resources.GetObject("LabelIconForDraw.Image"), System.Drawing.Image)
        Me.LabelIconForDraw.Location = New System.Drawing.Point(264, 68)
        Me.LabelIconForDraw.Name = "LabelIconForDraw"
        Me.LabelIconForDraw.Size = New System.Drawing.Size(17, 12)
        Me.LabelIconForDraw.TabIndex = 3
        Me.LabelIconForDraw.Text = "   "
        Me.LabelIconForDraw.Visible = False
        '
        'btnChageY_Init
        '
        Me.btnChageY_Init.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnChageY_Init.Location = New System.Drawing.Point(433, 151)
        Me.btnChageY_Init.Name = "btnChageY_Init"
        Me.btnChageY_Init.Size = New System.Drawing.Size(81, 24)
        Me.btnChageY_Init.TabIndex = 13
        Me.btnChageY_Init.Text = "ﾃﾞﾌｫﾙﾄ値"
        Me.btnChageY_Init.UseVisualStyleBackColor = True
        '
        'btnChageY_Dev
        '
        Me.btnChageY_Dev.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnChageY_Dev.Location = New System.Drawing.Point(340, 150)
        Me.btnChageY_Dev.Name = "btnChageY_Dev"
        Me.btnChageY_Dev.Size = New System.Drawing.Size(58, 24)
        Me.btnChageY_Dev.TabIndex = 15
        Me.btnChageY_Dev.Text = "変更"
        Me.btnChageY_Dev.UseVisualStyleBackColor = True
        '
        'nudResY_Dev
        '
        Me.nudResY_Dev.DecimalPlaces = 2
        Me.nudResY_Dev.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.nudResY_Dev.Increment = New Decimal(New Integer() {1, 0, 0, 131072})
        Me.nudResY_Dev.Location = New System.Drawing.Point(226, 151)
        Me.nudResY_Dev.Name = "nudResY_Dev"
        Me.nudResY_Dev.Size = New System.Drawing.Size(112, 23)
        Me.nudResY_Dev.TabIndex = 12
        Me.nudResY_Dev.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudResY_Dev.Value = New Decimal(New Integer() {1, 0, 0, 65536})
        '
        'Label3
        '
        Me.Label3.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label3.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label3.Location = New System.Drawing.Point(226, 120)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(112, 28)
        Me.Label3.TabIndex = 5
        Me.Label3.Text = "倍率指定 [率]"
        Me.Label3.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'Label6
        '
        Me.Label6.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label6.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label6.Location = New System.Drawing.Point(44, 120)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(89, 28)
        Me.Label6.TabIndex = 6
        Me.Label6.Text = "元[mm/pix]"
        Me.Label6.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lblBaseResY
        '
        Me.lblBaseResY.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblBaseResY.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblBaseResY.Location = New System.Drawing.Point(44, 150)
        Me.lblBaseResY.Name = "lblBaseResY"
        Me.lblBaseResY.Size = New System.Drawing.Size(88, 24)
        Me.lblBaseResY.TabIndex = 7
        Me.lblBaseResY.Text = "0.123456"
        Me.lblBaseResY.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'Label2
        '
        Me.Label2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label2.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label2.Location = New System.Drawing.Point(137, 120)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(86, 28)
        Me.Label2.TabIndex = 8
        Me.Label2.Text = "現状"
        Me.Label2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'lblNowResY
        '
        Me.lblNowResY.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblNowResY.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblNowResY.Location = New System.Drawing.Point(137, 151)
        Me.lblNowResY.Name = "lblNowResY"
        Me.lblNowResY.Size = New System.Drawing.Size(86, 23)
        Me.lblNowResY.TabIndex = 9
        Me.lblNowResY.Text = "0.123456"
        Me.lblNowResY.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'Label1
        '
        Me.Label1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label1.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.Location = New System.Drawing.Point(12, 150)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(26, 24)
        Me.Label1.TabIndex = 10
        Me.Label1.Text = "縦"
        Me.Label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        '
        'chkContrast
        '
        Me.chkContrast.AutoSize = True
        Me.chkContrast.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.chkContrast.Location = New System.Drawing.Point(12, 194)
        Me.chkContrast.Name = "chkContrast"
        Me.chkContrast.Size = New System.Drawing.Size(177, 20)
        Me.chkContrast.TabIndex = 18
        Me.chkContrast.Text = "コントラスト、明るさ補正"
        Me.chkContrast.UseVisualStyleBackColor = True
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.Label9)
        Me.GroupBox2.Controls.Add(Me.Label10)
        Me.GroupBox2.Controls.Add(Me.Label11)
        Me.GroupBox2.Controls.Add(Me.trbContrast)
        Me.GroupBox2.Location = New System.Drawing.Point(8, 12)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(240, 72)
        Me.GroupBox2.TabIndex = 1
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "コントラスト補正"
        '
        'trbContrast
        '
        Me.trbContrast.LargeChange = 4
        Me.trbContrast.Location = New System.Drawing.Point(8, 16)
        Me.trbContrast.Maximum = 255
        Me.trbContrast.Name = "trbContrast"
        Me.trbContrast.Size = New System.Drawing.Size(212, 45)
        Me.trbContrast.TabIndex = 0
        Me.trbContrast.TickFrequency = 16
        Me.trbContrast.Value = 255
        '
        'Label11
        '
        Me.Label11.AutoSize = True
        Me.Label11.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label11.Location = New System.Drawing.Point(8, 48)
        Me.Label11.Name = "Label11"
        Me.Label11.Size = New System.Drawing.Size(15, 15)
        Me.Label11.TabIndex = 1
        Me.Label11.Text = "0"
        '
        'Label10
        '
        Me.Label10.AutoSize = True
        Me.Label10.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label10.Location = New System.Drawing.Point(200, 48)
        Me.Label10.Name = "Label10"
        Me.Label10.Size = New System.Drawing.Size(31, 15)
        Me.Label10.TabIndex = 1
        Me.Label10.Text = "255"
        '
        'Label9
        '
        Me.Label9.AutoSize = True
        Me.Label9.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label9.Location = New System.Drawing.Point(100, 48)
        Me.Label9.Name = "Label9"
        Me.Label9.Size = New System.Drawing.Size(31, 15)
        Me.Label9.TabIndex = 1
        Me.Label9.Text = "128"
        '
        'GroupBox3
        '
        Me.GroupBox3.Controls.Add(Me.Label5)
        Me.GroupBox3.Controls.Add(Me.Label7)
        Me.GroupBox3.Controls.Add(Me.Label8)
        Me.GroupBox3.Controls.Add(Me.trbBright)
        Me.GroupBox3.Location = New System.Drawing.Point(8, 88)
        Me.GroupBox3.Name = "GroupBox3"
        Me.GroupBox3.Size = New System.Drawing.Size(240, 72)
        Me.GroupBox3.TabIndex = 1
        Me.GroupBox3.TabStop = False
        Me.GroupBox3.Text = "明るさ補正"
        '
        'trbBright
        '
        Me.trbBright.LargeChange = 4
        Me.trbBright.Location = New System.Drawing.Point(8, 16)
        Me.trbBright.Maximum = 255
        Me.trbBright.Name = "trbBright"
        Me.trbBright.Size = New System.Drawing.Size(212, 45)
        Me.trbBright.TabIndex = 0
        Me.trbBright.TickFrequency = 16
        Me.trbBright.Value = 128
        '
        'Label8
        '
        Me.Label8.AutoSize = True
        Me.Label8.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label8.Location = New System.Drawing.Point(8, 48)
        Me.Label8.Name = "Label8"
        Me.Label8.Size = New System.Drawing.Size(15, 15)
        Me.Label8.TabIndex = 1
        Me.Label8.Text = "0"
        '
        'Label7
        '
        Me.Label7.AutoSize = True
        Me.Label7.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label7.Location = New System.Drawing.Point(200, 48)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(31, 15)
        Me.Label7.TabIndex = 1
        Me.Label7.Text = "255"
        '
        'Label5
        '
        Me.Label5.AutoSize = True
        Me.Label5.Font = New System.Drawing.Font("MS UI Gothic", 11.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label5.Location = New System.Drawing.Point(100, 48)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(31, 15)
        Me.Label5.TabIndex = 1
        Me.Label5.Text = "128"
        '
        'lblContrast
        '
        Me.lblContrast.AutoSize = True
        Me.lblContrast.Location = New System.Drawing.Point(248, 20)
        Me.lblContrast.Name = "lblContrast"
        Me.lblContrast.Size = New System.Drawing.Size(23, 12)
        Me.lblContrast.TabIndex = 8
        Me.lblContrast.Text = "000"
        Me.lblContrast.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'lblBright
        '
        Me.lblBright.AutoSize = True
        Me.lblBright.Location = New System.Drawing.Point(248, 96)
        Me.lblBright.Name = "lblBright"
        Me.lblBright.Size = New System.Drawing.Size(23, 12)
        Me.lblBright.TabIndex = 9
        Me.lblBright.Text = "000"
        Me.lblBright.TextAlign = System.Drawing.ContentAlignment.MiddleRight
        '
        'GroupBox5
        '
        Me.GroupBox5.Controls.Add(Me.lblBright)
        Me.GroupBox5.Controls.Add(Me.lblContrast)
        Me.GroupBox5.Controls.Add(Me.GroupBox3)
        Me.GroupBox5.Controls.Add(Me.GroupBox2)
        Me.GroupBox5.Location = New System.Drawing.Point(36, 211)
        Me.GroupBox5.Name = "GroupBox5"
        Me.GroupBox5.Size = New System.Drawing.Size(302, 168)
        Me.GroupBox5.TabIndex = 17
        Me.GroupBox5.TabStop = False
        '
        'frmPopHotRoll
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(526, 394)
        Me.Controls.Add(Me.chkContrast)
        Me.Controls.Add(Me.GroupBox5)
        Me.Controls.Add(Me.btnChageY_Init)
        Me.Controls.Add(Me.btnChageY_Dev)
        Me.Controls.Add(Me.nudResY_Dev)
        Me.Controls.Add(Me.Label3)
        Me.Controls.Add(Me.Label6)
        Me.Controls.Add(Me.lblBaseResY)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.lblNowResY)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.LabelIconForDraw)
        Me.Controls.Add(Me.LabelIcon)
        Me.Controls.Add(Me.btnBackward)
        Me.Controls.Add(Me.btnForward)
        Me.Controls.Add(Me.CheckBox7)
        Me.Controls.Add(Me.CheckBox6)
        Me.Controls.Add(Me.CheckBox5)
        Me.Controls.Add(Me.CheckBox4)
        Me.Controls.Add(Me.CheckBox3)
        Me.Controls.Add(Me.CheckBox2)
        Me.Controls.Add(Me.CheckBox1)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow
        Me.Name = "frmPopHotRoll"
        Me.Text = "ロール肌荒れ確認ウインドウ"
        CType(Me.nudResY_Dev, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox2.ResumeLayout(False)
        Me.GroupBox2.PerformLayout()
        CType(Me.trbContrast, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox3.ResumeLayout(False)
        Me.GroupBox3.PerformLayout()
        CType(Me.trbBright, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox5.ResumeLayout(False)
        Me.GroupBox5.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

    Friend WithEvents CheckBox1 As Windows.Forms.CheckBox
    Friend WithEvents CheckBox2 As Windows.Forms.CheckBox
    Friend WithEvents CheckBox3 As Windows.Forms.CheckBox
    Friend WithEvents CheckBox4 As Windows.Forms.CheckBox
    Friend WithEvents CheckBox5 As Windows.Forms.CheckBox
    Friend WithEvents CheckBox6 As Windows.Forms.CheckBox
    Friend WithEvents CheckBox7 As Windows.Forms.CheckBox
    Friend WithEvents btnForward As Windows.Forms.Button
    Friend WithEvents btnBackward As Windows.Forms.Button
    Friend WithEvents LabelIcon As Windows.Forms.Label
    Friend WithEvents LabelIconForDraw As Windows.Forms.Label
    Friend WithEvents btnChageY_Init As Windows.Forms.Button
    Friend WithEvents btnChageY_Dev As Windows.Forms.Button
    Friend WithEvents nudResY_Dev As Windows.Forms.NumericUpDown
    Friend WithEvents Label3 As Windows.Forms.Label
    Friend WithEvents Label6 As Windows.Forms.Label
    Friend WithEvents lblBaseResY As Windows.Forms.Label
    Friend WithEvents Label2 As Windows.Forms.Label
    Friend WithEvents lblNowResY As Windows.Forms.Label
    Friend WithEvents Label1 As Windows.Forms.Label
    Friend WithEvents chkContrast As Windows.Forms.CheckBox
    Friend WithEvents GroupBox2 As Windows.Forms.GroupBox
    Friend WithEvents Label9 As Windows.Forms.Label
    Friend WithEvents Label10 As Windows.Forms.Label
    Friend WithEvents Label11 As Windows.Forms.Label
    Friend WithEvents trbContrast As Windows.Forms.TrackBar
    Friend WithEvents GroupBox3 As Windows.Forms.GroupBox
    Friend WithEvents Label5 As Windows.Forms.Label
    Friend WithEvents Label7 As Windows.Forms.Label
    Friend WithEvents Label8 As Windows.Forms.Label
    Friend WithEvents trbBright As Windows.Forms.TrackBar
    Friend WithEvents lblContrast As Windows.Forms.Label
    Friend WithEvents lblBright As Windows.Forms.Label
    Friend WithEvents GroupBox5 As Windows.Forms.GroupBox
End Class
