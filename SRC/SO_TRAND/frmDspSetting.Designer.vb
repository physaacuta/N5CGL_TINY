<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmDspSetting
    Inherits tClassForm.frmPop_Btn2

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing AndAlso components IsNot Nothing Then
            components.Dispose()
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.GroupBox1 = New System.Windows.Forms.GroupBox()
        Me.grpGrade = New System.Windows.Forms.GroupBox()
        Me.dgvDispGrade = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.btnGradeAllCancel = New System.Windows.Forms.Button()
        Me.btnGradeAllSelect = New System.Windows.Forms.Button()
        Me.grpType = New System.Windows.Forms.GroupBox()
        Me.dgvDispType = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.btnTypeAllCancel = New System.Windows.Forms.Button()
        Me.GroupBox2 = New System.Windows.Forms.GroupBox()
        Me.dtpDateEnd = New System.Windows.Forms.DateTimePicker()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.dtpDateStart = New System.Windows.Forms.DateTimePicker()
        Me.rdoSelect2 = New System.Windows.Forms.RadioButton()
        Me.rdoSelect1 = New System.Windows.Forms.RadioButton()
        Me.GroupBox3 = New System.Windows.Forms.GroupBox()
        Me.GroupBox6 = New System.Windows.Forms.GroupBox()
        Me.btnLineColor5 = New System.Windows.Forms.Button()
        Me.btnLineColor4 = New System.Windows.Forms.Button()
        Me.btnLineColor3 = New System.Windows.Forms.Button()
        Me.btnLineColor2 = New System.Windows.Forms.Button()
        Me.btnLineColor1 = New System.Windows.Forms.Button()
        Me.GroupBox5 = New System.Windows.Forms.GroupBox()
        Me.nudXInterval = New System.Windows.Forms.NumericUpDown()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.nudXMax = New System.Windows.Forms.NumericUpDown()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.GroupBox4 = New System.Windows.Forms.GroupBox()
        Me.nudYInterval = New System.Windows.Forms.NumericUpDown()
        Me.nudYMax = New System.Windows.Forms.NumericUpDown()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.GroupBox1.SuspendLayout()
        Me.grpGrade.SuspendLayout()
        CType(Me.dgvDispGrade, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.grpType.SuspendLayout()
        CType(Me.dgvDispType, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox2.SuspendLayout()
        Me.GroupBox3.SuspendLayout()
        Me.GroupBox6.SuspendLayout()
        Me.GroupBox5.SuspendLayout()
        CType(Me.nudXInterval, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nudXMax, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox4.SuspendLayout()
        CType(Me.nudYInterval, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nudYMax, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'grpFunction
        '
        Me.grpFunction.Location = New System.Drawing.Point(420, 40)
        '
        'btnF01
        '
        Me.btnF01.Tag = 0
        Me.btnF01.Text = "ìoò^"
        '
        'btnF02
        '
        Me.btnF02.Tag = 11
        Me.btnF02.Text = "ï¬Ç∂ÇÈ"
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.grpGrade)
        Me.GroupBox1.Controls.Add(Me.grpType)
        Me.GroupBox1.Controls.Add(Me.GroupBox2)
        Me.GroupBox1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle))
        Me.GroupBox1.ForeColor = System.Drawing.Color.Navy
        Me.GroupBox1.Location = New System.Drawing.Point(8, 8)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(644, 416)
        Me.GroupBox1.TabIndex = 12
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "ï\é¶èåè"
        '
        'grpGrade
        '
        Me.grpGrade.Controls.Add(Me.dgvDispGrade)
        Me.grpGrade.Controls.Add(Me.btnGradeAllCancel)
        Me.grpGrade.Controls.Add(Me.btnGradeAllSelect)
        Me.grpGrade.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold)
        Me.grpGrade.ForeColor = System.Drawing.SystemColors.ControlText
        Me.grpGrade.Location = New System.Drawing.Point(332, 132)
        Me.grpGrade.Name = "grpGrade"
        Me.grpGrade.Size = New System.Drawing.Size(300, 268)
        Me.grpGrade.TabIndex = 9
        Me.grpGrade.TabStop = False
        Me.grpGrade.Text = "ÉOÉåÅ[Éhï\é¶ÉtÉBÉãÉ^Å["
        '
        'dgvDispGrade
        '
        Me.dgvDispGrade.Location = New System.Drawing.Point(8, 20)
        Me.dgvDispGrade.Name = "dgvDispGrade"
        Me.dgvDispGrade.RowTemplate.Height = 21
        Me.dgvDispGrade.ScrollBars = System.Windows.Forms.ScrollBars.Horizontal
        Me.dgvDispGrade.Size = New System.Drawing.Size(284, 200)
        Me.dgvDispGrade.TabIndex = 9
        '
        'btnGradeAllCancel
        '
        Me.btnGradeAllCancel.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnGradeAllCancel.Location = New System.Drawing.Point(176, 228)
        Me.btnGradeAllCancel.Name = "btnGradeAllCancel"
        Me.btnGradeAllCancel.Size = New System.Drawing.Size(116, 32)
        Me.btnGradeAllCancel.TabIndex = 8
        Me.btnGradeAllCancel.Text = "ëSâèú"
        Me.btnGradeAllCancel.UseVisualStyleBackColor = True
        '
        'btnGradeAllSelect
        '
        Me.btnGradeAllSelect.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnGradeAllSelect.Location = New System.Drawing.Point(56, 228)
        Me.btnGradeAllSelect.Name = "btnGradeAllSelect"
        Me.btnGradeAllSelect.Size = New System.Drawing.Size(116, 32)
        Me.btnGradeAllSelect.TabIndex = 7
        Me.btnGradeAllSelect.Text = "ëSëIë"
        Me.btnGradeAllSelect.UseVisualStyleBackColor = True
        '
        'grpType
        '
        Me.grpType.Controls.Add(Me.dgvDispType)
        Me.grpType.Controls.Add(Me.btnTypeAllCancel)
        Me.grpType.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold)
        Me.grpType.ForeColor = System.Drawing.SystemColors.ControlText
        Me.grpType.Location = New System.Drawing.Point(12, 132)
        Me.grpType.Name = "grpType"
        Me.grpType.Size = New System.Drawing.Size(300, 268)
        Me.grpType.TabIndex = 8
        Me.grpType.TabStop = False
        Me.grpType.Text = "·réÌï\é¶ÉtÉBÉãÉ^Å["
        '
        'dgvDispType
        '
        Me.dgvDispType.Location = New System.Drawing.Point(8, 20)
        Me.dgvDispType.Name = "dgvDispType"
        Me.dgvDispType.RowTemplate.Height = 21
        Me.dgvDispType.ScrollBars = System.Windows.Forms.ScrollBars.Horizontal
        Me.dgvDispType.Size = New System.Drawing.Size(284, 200)
        Me.dgvDispType.TabIndex = 10
        '
        'btnTypeAllCancel
        '
        Me.btnTypeAllCancel.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnTypeAllCancel.Location = New System.Drawing.Point(176, 228)
        Me.btnTypeAllCancel.Name = "btnTypeAllCancel"
        Me.btnTypeAllCancel.Size = New System.Drawing.Size(116, 32)
        Me.btnTypeAllCancel.TabIndex = 6
        Me.btnTypeAllCancel.Text = "ëSâèú"
        Me.btnTypeAllCancel.UseVisualStyleBackColor = True
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.dtpDateEnd)
        Me.GroupBox2.Controls.Add(Me.Label3)
        Me.GroupBox2.Controls.Add(Me.dtpDateStart)
        Me.GroupBox2.Controls.Add(Me.rdoSelect2)
        Me.GroupBox2.Controls.Add(Me.rdoSelect1)
        Me.GroupBox2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox2.ForeColor = System.Drawing.SystemColors.ControlText
        Me.GroupBox2.Location = New System.Drawing.Point(12, 32)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(396, 84)
        Me.GroupBox2.TabIndex = 7
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "ëŒè€ÉRÉCÉãéwíË"
        '
        'dtpDateEnd
        '
        Me.dtpDateEnd.Enabled = False
        Me.dtpDateEnd.Format = System.Windows.Forms.DateTimePickerFormat.[Short]
        Me.dtpDateEnd.Location = New System.Drawing.Point(264, 52)
        Me.dtpDateEnd.Name = "dtpDateEnd"
        Me.dtpDateEnd.Size = New System.Drawing.Size(124, 23)
        Me.dtpDateEnd.TabIndex = 6
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Location = New System.Drawing.Point(236, 56)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(25, 16)
        Me.Label3.TabIndex = 5
        Me.Label3.Text = "Å`"
        '
        'dtpDateStart
        '
        Me.dtpDateStart.Enabled = False
        Me.dtpDateStart.Format = System.Windows.Forms.DateTimePickerFormat.[Short]
        Me.dtpDateStart.Location = New System.Drawing.Point(108, 52)
        Me.dtpDateStart.Name = "dtpDateStart"
        Me.dtpDateStart.Size = New System.Drawing.Size(124, 23)
        Me.dtpDateStart.TabIndex = 4
        '
        'rdoSelect2
        '
        Me.rdoSelect2.Location = New System.Drawing.Point(12, 48)
        Me.rdoSelect2.Name = "rdoSelect2"
        Me.rdoSelect2.Size = New System.Drawing.Size(96, 32)
        Me.rdoSelect2.TabIndex = 3
        Me.rdoSelect2.TabStop = True
        Me.rdoSelect2.Text = "ä˙ä‘éwíË"
        Me.rdoSelect2.UseVisualStyleBackColor = True
        '
        'rdoSelect1
        '
        Me.rdoSelect1.BackColor = System.Drawing.SystemColors.Control
        Me.rdoSelect1.Location = New System.Drawing.Point(12, 20)
        Me.rdoSelect1.Name = "rdoSelect1"
        Me.rdoSelect1.Size = New System.Drawing.Size(148, 32)
        Me.rdoSelect1.TabIndex = 2
        Me.rdoSelect1.TabStop = True
        Me.rdoSelect1.Text = "íºãﬂÉRÉCÉãéwíË"
        Me.rdoSelect1.UseVisualStyleBackColor = False
        '
        'GroupBox3
        '
        Me.GroupBox3.Controls.Add(Me.GroupBox6)
        Me.GroupBox3.Controls.Add(Me.GroupBox5)
        Me.GroupBox3.Controls.Add(Me.GroupBox4)
        Me.GroupBox3.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, CType((System.Drawing.FontStyle.Bold Or System.Drawing.FontStyle.Italic), System.Drawing.FontStyle))
        Me.GroupBox3.ForeColor = System.Drawing.Color.Navy
        Me.GroupBox3.Location = New System.Drawing.Point(8, 440)
        Me.GroupBox3.Name = "GroupBox3"
        Me.GroupBox3.Size = New System.Drawing.Size(644, 196)
        Me.GroupBox3.TabIndex = 13
        Me.GroupBox3.TabStop = False
        Me.GroupBox3.Text = "ÉOÉâÉtê›íË"
        '
        'GroupBox6
        '
        Me.GroupBox6.Controls.Add(Me.btnLineColor5)
        Me.GroupBox6.Controls.Add(Me.btnLineColor4)
        Me.GroupBox6.Controls.Add(Me.btnLineColor3)
        Me.GroupBox6.Controls.Add(Me.btnLineColor2)
        Me.GroupBox6.Controls.Add(Me.btnLineColor1)
        Me.GroupBox6.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold)
        Me.GroupBox6.ForeColor = System.Drawing.SystemColors.ControlText
        Me.GroupBox6.Location = New System.Drawing.Point(12, 128)
        Me.GroupBox6.Name = "GroupBox6"
        Me.GroupBox6.Size = New System.Drawing.Size(620, 56)
        Me.GroupBox6.TabIndex = 1
        Me.GroupBox6.TabStop = False
        Me.GroupBox6.Text = "ê¸êF"
        '
        'btnLineColor5
        '
        Me.btnLineColor5.Location = New System.Drawing.Point(492, 24)
        Me.btnLineColor5.Name = "btnLineColor5"
        Me.btnLineColor5.Size = New System.Drawing.Size(116, 24)
        Me.btnLineColor5.TabIndex = 0
        Me.btnLineColor5.Text = "·réÌÇT(Åü)"
        Me.btnLineColor5.UseVisualStyleBackColor = True
        '
        'btnLineColor4
        '
        Me.btnLineColor4.Location = New System.Drawing.Point(372, 24)
        Me.btnLineColor4.Name = "btnLineColor4"
        Me.btnLineColor4.Size = New System.Drawing.Size(116, 24)
        Me.btnLineColor4.TabIndex = 0
        Me.btnLineColor4.Text = "·réÌÇS(Å£)"
        Me.btnLineColor4.UseVisualStyleBackColor = True
        '
        'btnLineColor3
        '
        Me.btnLineColor3.Location = New System.Drawing.Point(252, 24)
        Me.btnLineColor3.Name = "btnLineColor3"
        Me.btnLineColor3.Size = New System.Drawing.Size(116, 24)
        Me.btnLineColor3.TabIndex = 0
        Me.btnLineColor3.Text = "·réÌÇR(Å•)"
        Me.btnLineColor3.UseVisualStyleBackColor = True
        '
        'btnLineColor2
        '
        Me.btnLineColor2.Location = New System.Drawing.Point(132, 24)
        Me.btnLineColor2.Name = "btnLineColor2"
        Me.btnLineColor2.Size = New System.Drawing.Size(116, 24)
        Me.btnLineColor2.TabIndex = 0
        Me.btnLineColor2.Text = "·réÌÇQ(Å°)"
        Me.btnLineColor2.UseVisualStyleBackColor = True
        '
        'btnLineColor1
        '
        Me.btnLineColor1.Location = New System.Drawing.Point(16, 24)
        Me.btnLineColor1.Name = "btnLineColor1"
        Me.btnLineColor1.Size = New System.Drawing.Size(112, 24)
        Me.btnLineColor1.TabIndex = 0
        Me.btnLineColor1.Text = "·réÌÇP(Åú)"
        Me.btnLineColor1.UseVisualStyleBackColor = True
        '
        'GroupBox5
        '
        Me.GroupBox5.Controls.Add(Me.nudXInterval)
        Me.GroupBox5.Controls.Add(Me.Label4)
        Me.GroupBox5.Controls.Add(Me.nudXMax)
        Me.GroupBox5.Controls.Add(Me.Label5)
        Me.GroupBox5.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold)
        Me.GroupBox5.ForeColor = System.Drawing.SystemColors.ControlText
        Me.GroupBox5.Location = New System.Drawing.Point(332, 32)
        Me.GroupBox5.Name = "GroupBox5"
        Me.GroupBox5.Size = New System.Drawing.Size(300, 88)
        Me.GroupBox5.TabIndex = 0
        Me.GroupBox5.TabStop = False
        Me.GroupBox5.Text = "â°é≤ÅiXé≤Åj"
        '
        'nudXInterval
        '
        Me.nudXInterval.Location = New System.Drawing.Point(168, 56)
        Me.nudXInterval.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nudXInterval.Name = "nudXInterval"
        Me.nudXInterval.Size = New System.Drawing.Size(76, 23)
        Me.nudXInterval.TabIndex = 1
        Me.nudXInterval.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudXInterval.Value = New Decimal(New Integer() {5, 0, 0, 0})
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Location = New System.Drawing.Point(12, 28)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(120, 16)
        Me.Label4.TabIndex = 0
        Me.Label4.Text = "ç≈ëÂíl [ñ{êî]"
        '
        'nudXMax
        '
        Me.nudXMax.Increment = New Decimal(New Integer() {5, 0, 0, 0})
        Me.nudXMax.Location = New System.Drawing.Point(168, 24)
        Me.nudXMax.Maximum = New Decimal(New Integer() {1000, 0, 0, 0})
        Me.nudXMax.Minimum = New Decimal(New Integer() {5, 0, 0, 0})
        Me.nudXMax.Name = "nudXMax"
        Me.nudXMax.Size = New System.Drawing.Size(76, 23)
        Me.nudXMax.TabIndex = 1
        Me.nudXMax.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudXMax.Value = New Decimal(New Integer() {5, 0, 0, 0})
        '
        'Label5
        '
        Me.Label5.AutoSize = True
        Me.Label5.Location = New System.Drawing.Point(12, 60)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(154, 16)
        Me.Label5.TabIndex = 0
        Me.Label5.Text = "ÉÅÉÇÉää‘äu [ñ{êî]"
        '
        'GroupBox4
        '
        Me.GroupBox4.Controls.Add(Me.nudYInterval)
        Me.GroupBox4.Controls.Add(Me.nudYMax)
        Me.GroupBox4.Controls.Add(Me.Label2)
        Me.GroupBox4.Controls.Add(Me.Label1)
        Me.GroupBox4.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold)
        Me.GroupBox4.ForeColor = System.Drawing.SystemColors.ControlText
        Me.GroupBox4.Location = New System.Drawing.Point(12, 32)
        Me.GroupBox4.Name = "GroupBox4"
        Me.GroupBox4.Size = New System.Drawing.Size(300, 88)
        Me.GroupBox4.TabIndex = 0
        Me.GroupBox4.TabStop = False
        Me.GroupBox4.Text = "ècé≤ÅiYé≤Åj"
        '
        'nudYInterval
        '
        Me.nudYInterval.Location = New System.Drawing.Point(148, 56)
        Me.nudYInterval.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nudYInterval.Name = "nudYInterval"
        Me.nudYInterval.Size = New System.Drawing.Size(76, 23)
        Me.nudYInterval.TabIndex = 1
        Me.nudYInterval.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudYInterval.Value = New Decimal(New Integer() {5, 0, 0, 0})
        '
        'nudYMax
        '
        Me.nudYMax.Increment = New Decimal(New Integer() {5, 0, 0, 0})
        Me.nudYMax.Location = New System.Drawing.Point(148, 24)
        Me.nudYMax.Maximum = New Decimal(New Integer() {200, 0, 0, 0})
        Me.nudYMax.Minimum = New Decimal(New Integer() {5, 0, 0, 0})
        Me.nudYMax.Name = "nudYMax"
        Me.nudYMax.Size = New System.Drawing.Size(76, 23)
        Me.nudYMax.TabIndex = 1
        Me.nudYMax.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudYMax.Value = New Decimal(New Integer() {5, 0, 0, 0})
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Location = New System.Drawing.Point(12, 60)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(129, 16)
        Me.Label2.TabIndex = 0
        Me.Label2.Text = "ÉÅÉÇÉää‘äu [%]"
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(12, 28)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(95, 16)
        Me.Label1.TabIndex = 0
        Me.Label1.Text = "ç≈ëÂíl [%]"
        '
        'frmDspSetting
        '
        Me.ClientSize = New System.Drawing.Size(661, 645)
        Me.Controls.Add(Me.GroupBox1)
        Me.Controls.Add(Me.GroupBox3)
        Me.Name = "frmDspSetting"
        Me.Text = "ï\é¶èåèê›íËâÊñ "
        Me.TopMost = False
        Me.Controls.SetChildIndex(Me.GroupBox3, 0)
        Me.Controls.SetChildIndex(Me.GroupBox1, 0)
        Me.Controls.SetChildIndex(Me.grpFunction, 0)
        Me.GroupBox1.ResumeLayout(False)
        Me.grpGrade.ResumeLayout(False)
        CType(Me.dgvDispGrade, System.ComponentModel.ISupportInitialize).EndInit()
        Me.grpType.ResumeLayout(False)
        CType(Me.dgvDispType, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox2.ResumeLayout(False)
        Me.GroupBox2.PerformLayout()
        Me.GroupBox3.ResumeLayout(False)
        Me.GroupBox6.ResumeLayout(False)
        Me.GroupBox5.ResumeLayout(False)
        Me.GroupBox5.PerformLayout()
        CType(Me.nudXInterval, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nudXMax, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox4.ResumeLayout(False)
        Me.GroupBox4.PerformLayout()
        CType(Me.nudYInterval, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nudYMax, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
	Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
	Friend WithEvents rdoSelect2 As System.Windows.Forms.RadioButton
	Friend WithEvents rdoSelect1 As System.Windows.Forms.RadioButton
	Protected Friend WithEvents grpGrade As System.Windows.Forms.GroupBox
	Protected Friend WithEvents dgvDispGrade As tClassLibrary.tCnt.tCnt_DataGridView
	Protected Friend WithEvents btnGradeAllCancel As System.Windows.Forms.Button
	Protected Friend WithEvents btnGradeAllSelect As System.Windows.Forms.Button
	Protected Friend WithEvents grpType As System.Windows.Forms.GroupBox
	Protected Friend WithEvents dgvDispType As tClassLibrary.tCnt.tCnt_DataGridView
	Protected Friend WithEvents btnTypeAllCancel As System.Windows.Forms.Button
	Protected Friend WithEvents dtpDateEnd As System.Windows.Forms.DateTimePicker
	Protected Friend WithEvents Label3 As System.Windows.Forms.Label
	Protected Friend WithEvents dtpDateStart As System.Windows.Forms.DateTimePicker
	Friend WithEvents GroupBox3 As System.Windows.Forms.GroupBox
	Friend WithEvents GroupBox4 As System.Windows.Forms.GroupBox
	Friend WithEvents GroupBox5 As System.Windows.Forms.GroupBox
	Friend WithEvents Label1 As System.Windows.Forms.Label
	Friend WithEvents nudYMax As System.Windows.Forms.NumericUpDown
	Friend WithEvents Label2 As System.Windows.Forms.Label
	Friend WithEvents nudXInterval As System.Windows.Forms.NumericUpDown
	Friend WithEvents Label4 As System.Windows.Forms.Label
	Friend WithEvents nudXMax As System.Windows.Forms.NumericUpDown
	Friend WithEvents Label5 As System.Windows.Forms.Label
	Friend WithEvents nudYInterval As System.Windows.Forms.NumericUpDown
	Friend WithEvents GroupBox6 As System.Windows.Forms.GroupBox
	Friend WithEvents btnLineColor5 As System.Windows.Forms.Button
	Friend WithEvents btnLineColor4 As System.Windows.Forms.Button
	Friend WithEvents btnLineColor3 As System.Windows.Forms.Button
	Friend WithEvents btnLineColor2 As System.Windows.Forms.Button
	Friend WithEvents btnLineColor1 As System.Windows.Forms.Button

End Class
