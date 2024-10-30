<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPopHotJump
    Inherits System.Windows.Forms.Form

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

    'Windows フォーム デザイナーで必要です。
    Private components As System.ComponentModel.IContainer

    'メモ: 以下のプロシージャは Windows フォーム デザイナーで必要です。
    'Windows フォーム デザイナーを使用して変更できます。  
    'コード エディターを使って変更しないでください。
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Dim DataGridViewCellStyle1 As System.Windows.Forms.DataGridViewCellStyle = New System.Windows.Forms.DataGridViewCellStyle()
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(frmPopHotJump))
        Dim DataGridViewCellStyle2 As System.Windows.Forms.DataGridViewCellStyle = New System.Windows.Forms.DataGridViewCellStyle()
        Me.CheckBox1 = New System.Windows.Forms.CheckBox()
        Me.CheckBox2 = New System.Windows.Forms.CheckBox()
        Me.CheckBox3 = New System.Windows.Forms.CheckBox()
        Me.CheckBox4 = New System.Windows.Forms.CheckBox()
        Me.CheckBox5 = New System.Windows.Forms.CheckBox()
        Me.CheckBox6 = New System.Windows.Forms.CheckBox()
        Me.CheckBox7 = New System.Windows.Forms.CheckBox()
        Me.CheckBox8 = New System.Windows.Forms.CheckBox()
        Me.CheckBox9 = New System.Windows.Forms.CheckBox()
        Me.CheckBox10 = New System.Windows.Forms.CheckBox()
        Me.CheckBox11 = New System.Windows.Forms.CheckBox()
        Me.CheckBox12 = New System.Windows.Forms.CheckBox()
        Me.CheckBox13 = New System.Windows.Forms.CheckBox()
        Me.CheckBox14 = New System.Windows.Forms.CheckBox()
        Me.btnForward = New System.Windows.Forms.Button()
        Me.btnBackward = New System.Windows.Forms.Button()
        Me.dgvCoil = New System.Windows.Forms.DataGridView()
        Me.Column1 = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.Column2 = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.dgvPitch = New System.Windows.Forms.DataGridView()
        Me.LabelIcon = New System.Windows.Forms.Label()
        Me.LabelIconForDraw = New System.Windows.Forms.Label()
        Me.TextBox1 = New System.Windows.Forms.TextBox()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.nudFontSize = New System.Windows.Forms.NumericUpDown()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.nudLineWidth = New System.Windows.Forms.NumericUpDown()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.cmbDashStyle = New System.Windows.Forms.ComboBox()
        Me.Button1 = New System.Windows.Forms.Button()
        Me.ColumnStd = New System.Windows.Forms.DataGridViewTextBoxColumn()
        Me.ColumnPitch = New System.Windows.Forms.DataGridViewTextBoxColumn()
        CType(Me.dgvCoil, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvPitch, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nudFontSize, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.nudLineWidth, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'CheckBox1
        '
        Me.CheckBox1.Appearance = System.Windows.Forms.Appearance.Button
        Me.CheckBox1.AutoSize = True
        Me.CheckBox1.Font = New System.Drawing.Font("Calibri", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.CheckBox1.Location = New System.Drawing.Point(12, 12)
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
        Me.CheckBox2.Location = New System.Drawing.Point(84, 12)
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
        Me.CheckBox3.Location = New System.Drawing.Point(156, 12)
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
        Me.CheckBox4.Location = New System.Drawing.Point(228, 12)
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
        Me.CheckBox5.Location = New System.Drawing.Point(300, 12)
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
        Me.CheckBox6.Location = New System.Drawing.Point(372, 12)
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
        Me.CheckBox7.Location = New System.Drawing.Point(442, 12)
        Me.CheckBox7.MinimumSize = New System.Drawing.Size(66, 0)
        Me.CheckBox7.Name = "CheckBox7"
        Me.CheckBox7.Size = New System.Drawing.Size(66, 33)
        Me.CheckBox7.TabIndex = 0
        Me.CheckBox7.Text = "F7"
        Me.CheckBox7.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.CheckBox7.UseVisualStyleBackColor = True
        '
        'CheckBox8
        '
        Me.CheckBox8.Appearance = System.Windows.Forms.Appearance.Button
        Me.CheckBox8.AutoSize = True
        Me.CheckBox8.Font = New System.Drawing.Font("Calibri", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.CheckBox8.Location = New System.Drawing.Point(12, 51)
        Me.CheckBox8.MinimumSize = New System.Drawing.Size(66, 0)
        Me.CheckBox8.Name = "CheckBox8"
        Me.CheckBox8.Size = New System.Drawing.Size(66, 33)
        Me.CheckBox8.TabIndex = 0
        Me.CheckBox8.Text = "HM"
        Me.CheckBox8.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.CheckBox8.UseVisualStyleBackColor = True
        '
        'CheckBox9
        '
        Me.CheckBox9.Appearance = System.Windows.Forms.Appearance.Button
        Me.CheckBox9.AutoSize = True
        Me.CheckBox9.Font = New System.Drawing.Font("Calibri", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.CheckBox9.Location = New System.Drawing.Point(84, 51)
        Me.CheckBox9.MinimumSize = New System.Drawing.Size(66, 0)
        Me.CheckBox9.Name = "CheckBox9"
        Me.CheckBox9.Size = New System.Drawing.Size(66, 33)
        Me.CheckBox9.TabIndex = 0
        Me.CheckBox9.Text = "RRM1"
        Me.CheckBox9.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.CheckBox9.UseVisualStyleBackColor = True
        '
        'CheckBox10
        '
        Me.CheckBox10.Appearance = System.Windows.Forms.Appearance.Button
        Me.CheckBox10.AutoSize = True
        Me.CheckBox10.Font = New System.Drawing.Font("Calibri", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.CheckBox10.Location = New System.Drawing.Point(156, 51)
        Me.CheckBox10.MinimumSize = New System.Drawing.Size(66, 0)
        Me.CheckBox10.Name = "CheckBox10"
        Me.CheckBox10.Size = New System.Drawing.Size(66, 33)
        Me.CheckBox10.TabIndex = 0
        Me.CheckBox10.Text = "RRM2"
        Me.CheckBox10.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.CheckBox10.UseVisualStyleBackColor = True
        '
        'CheckBox11
        '
        Me.CheckBox11.Appearance = System.Windows.Forms.Appearance.Button
        Me.CheckBox11.AutoSize = True
        Me.CheckBox11.Font = New System.Drawing.Font("Calibri", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.CheckBox11.Location = New System.Drawing.Point(228, 51)
        Me.CheckBox11.MinimumSize = New System.Drawing.Size(66, 0)
        Me.CheckBox11.Name = "CheckBox11"
        Me.CheckBox11.Size = New System.Drawing.Size(66, 33)
        Me.CheckBox11.TabIndex = 0
        Me.CheckBox11.Text = "RRM3"
        Me.CheckBox11.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.CheckBox11.UseVisualStyleBackColor = True
        '
        'CheckBox12
        '
        Me.CheckBox12.Appearance = System.Windows.Forms.Appearance.Button
        Me.CheckBox12.AutoSize = True
        Me.CheckBox12.Font = New System.Drawing.Font("Calibri", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.CheckBox12.Location = New System.Drawing.Point(300, 51)
        Me.CheckBox12.MinimumSize = New System.Drawing.Size(66, 0)
        Me.CheckBox12.Name = "CheckBox12"
        Me.CheckBox12.Size = New System.Drawing.Size(66, 33)
        Me.CheckBox12.TabIndex = 0
        Me.CheckBox12.Text = "RRM4"
        Me.CheckBox12.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.CheckBox12.UseVisualStyleBackColor = True
        '
        'CheckBox13
        '
        Me.CheckBox13.Appearance = System.Windows.Forms.Appearance.Button
        Me.CheckBox13.AutoSize = True
        Me.CheckBox13.Font = New System.Drawing.Font("Calibri", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.CheckBox13.Location = New System.Drawing.Point(372, 51)
        Me.CheckBox13.MinimumSize = New System.Drawing.Size(66, 0)
        Me.CheckBox13.Name = "CheckBox13"
        Me.CheckBox13.Size = New System.Drawing.Size(66, 33)
        Me.CheckBox13.TabIndex = 0
        Me.CheckBox13.Text = "RRM5"
        Me.CheckBox13.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.CheckBox13.UseVisualStyleBackColor = True
        '
        'CheckBox14
        '
        Me.CheckBox14.Appearance = System.Windows.Forms.Appearance.Button
        Me.CheckBox14.AutoSize = True
        Me.CheckBox14.Font = New System.Drawing.Font("Calibri", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.CheckBox14.Location = New System.Drawing.Point(442, 51)
        Me.CheckBox14.MinimumSize = New System.Drawing.Size(66, 0)
        Me.CheckBox14.Name = "CheckBox14"
        Me.CheckBox14.Size = New System.Drawing.Size(66, 33)
        Me.CheckBox14.TabIndex = 0
        Me.CheckBox14.Text = "R5"
        Me.CheckBox14.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        Me.CheckBox14.UseVisualStyleBackColor = True
        '
        'btnForward
        '
        Me.btnForward.Font = New System.Drawing.Font("MS UI Gothic", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnForward.Location = New System.Drawing.Point(12, 90)
        Me.btnForward.Name = "btnForward"
        Me.btnForward.Size = New System.Drawing.Size(210, 35)
        Me.btnForward.TabIndex = 1
        Me.btnForward.Text = "順方向＋"
        Me.btnForward.UseVisualStyleBackColor = True
        '
        'btnBackward
        '
        Me.btnBackward.Font = New System.Drawing.Font("MS UI Gothic", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnBackward.Location = New System.Drawing.Point(298, 90)
        Me.btnBackward.Name = "btnBackward"
        Me.btnBackward.Size = New System.Drawing.Size(210, 35)
        Me.btnBackward.TabIndex = 1
        Me.btnBackward.Text = "逆方向－"
        Me.btnBackward.UseVisualStyleBackColor = True
        '
        'dgvCoil
        '
        Me.dgvCoil.AllowUserToAddRows = False
        Me.dgvCoil.AllowUserToDeleteRows = False
        Me.dgvCoil.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left), System.Windows.Forms.AnchorStyles)
        Me.dgvCoil.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvCoil.Columns.AddRange(New System.Windows.Forms.DataGridViewColumn() {Me.Column1, Me.Column2})
        Me.dgvCoil.Location = New System.Drawing.Point(12, 178)
        Me.dgvCoil.Name = "dgvCoil"
        Me.dgvCoil.ReadOnly = True
        Me.dgvCoil.RowHeadersWidth = 4
        Me.dgvCoil.RowTemplate.Height = 21
        Me.dgvCoil.Size = New System.Drawing.Size(244, 340)
        Me.dgvCoil.TabIndex = 2
        '
        'Column1
        '
        Me.Column1.HeaderText = "タイトル"
        Me.Column1.Name = "Column1"
        Me.Column1.ReadOnly = True
        '
        'Column2
        '
        Me.Column2.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill
        DataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleRight
        Me.Column2.DefaultCellStyle = DataGridViewCellStyle1
        Me.Column2.HeaderText = "値"
        Me.Column2.Name = "Column2"
        Me.Column2.ReadOnly = True
        '
        'dgvPitch
        '
        Me.dgvPitch.AllowUserToAddRows = False
        Me.dgvPitch.AllowUserToDeleteRows = False
        Me.dgvPitch.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.dgvPitch.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvPitch.Columns.AddRange(New System.Windows.Forms.DataGridViewColumn() {Me.ColumnStd, Me.ColumnPitch})
        Me.dgvPitch.Location = New System.Drawing.Point(285, 178)
        Me.dgvPitch.Name = "dgvPitch"
        Me.dgvPitch.RowTemplate.Height = 21
        Me.dgvPitch.Size = New System.Drawing.Size(229, 340)
        Me.dgvPitch.TabIndex = 2
        '
        'LabelIcon
        '
        Me.LabelIcon.AutoSize = True
        Me.LabelIcon.Image = CType(resources.GetObject("LabelIcon.Image"), System.Drawing.Image)
        Me.LabelIcon.Location = New System.Drawing.Point(258, 90)
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
        Me.LabelIconForDraw.Location = New System.Drawing.Point(258, 113)
        Me.LabelIconForDraw.Name = "LabelIconForDraw"
        Me.LabelIconForDraw.Size = New System.Drawing.Size(17, 12)
        Me.LabelIconForDraw.TabIndex = 3
        Me.LabelIconForDraw.Text = "   "
        Me.LabelIconForDraw.Visible = False
        '
        'TextBox1
        '
        Me.TextBox1.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
            Or System.Windows.Forms.AnchorStyles.Left) _
            Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.TextBox1.Font = New System.Drawing.Font("MS UI Gothic", 15.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.TextBox1.Location = New System.Drawing.Point(12, 178)
        Me.TextBox1.Multiline = True
        Me.TextBox1.Name = "TextBox1"
        Me.TextBox1.Size = New System.Drawing.Size(502, 340)
        Me.TextBox1.TabIndex = 4
        Me.TextBox1.Visible = False
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.Location = New System.Drawing.Point(12, 141)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(91, 16)
        Me.Label1.TabIndex = 5
        Me.Label1.Text = "フォントサイズ"
        '
        'nudFontSize
        '
        Me.nudFontSize.Font = New System.Drawing.Font("MS UI Gothic", 12.0!)
        Me.nudFontSize.Location = New System.Drawing.Point(109, 138)
        Me.nudFontSize.Maximum = New Decimal(New Integer() {72, 0, 0, 0})
        Me.nudFontSize.Minimum = New Decimal(New Integer() {6, 0, 0, 0})
        Me.nudFontSize.Name = "nudFontSize"
        Me.nudFontSize.Size = New System.Drawing.Size(65, 23)
        Me.nudFontSize.TabIndex = 6
        Me.nudFontSize.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudFontSize.Value = New Decimal(New Integer() {36, 0, 0, 0})
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label2.Location = New System.Drawing.Point(197, 141)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(59, 16)
        Me.Label2.TabIndex = 5
        Me.Label2.Text = "ライン幅"
        '
        'nudLineWidth
        '
        Me.nudLineWidth.Font = New System.Drawing.Font("MS UI Gothic", 12.0!)
        Me.nudLineWidth.Location = New System.Drawing.Point(257, 138)
        Me.nudLineWidth.Maximum = New Decimal(New Integer() {10, 0, 0, 0})
        Me.nudLineWidth.Minimum = New Decimal(New Integer() {1, 0, 0, 0})
        Me.nudLineWidth.Name = "nudLineWidth"
        Me.nudLineWidth.Size = New System.Drawing.Size(44, 23)
        Me.nudLineWidth.TabIndex = 6
        Me.nudLineWidth.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
        Me.nudLineWidth.Value = New Decimal(New Integer() {2, 0, 0, 0})
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Font = New System.Drawing.Font("MS UI Gothic", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label3.Location = New System.Drawing.Point(313, 142)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(75, 16)
        Me.Label3.TabIndex = 5
        Me.Label3.Text = "ライン形式"
        '
        'cmbDashStyle
        '
        Me.cmbDashStyle.Font = New System.Drawing.Font("MS UI Gothic", 12.0!)
        Me.cmbDashStyle.FormattingEnabled = True
        Me.cmbDashStyle.Items.AddRange(New Object() {"直線", "--------", "・・・・・・・", "-・-・-・-", "-・・-・・-"})
        Me.cmbDashStyle.Location = New System.Drawing.Point(394, 138)
        Me.cmbDashStyle.Name = "cmbDashStyle"
        Me.cmbDashStyle.Size = New System.Drawing.Size(86, 24)
        Me.cmbDashStyle.TabIndex = 7
        '
        'Button1
        '
        Me.Button1.Location = New System.Drawing.Point(488, 139)
        Me.Button1.Name = "Button1"
        Me.Button1.Size = New System.Drawing.Size(25, 22)
        Me.Button1.TabIndex = 8
        Me.Button1.Text = "保存"
        Me.Button1.UseVisualStyleBackColor = True
        '
        'ColumnStd
        '
        Me.ColumnStd.FillWeight = 60.0!
        Me.ColumnStd.HeaderText = "STD・パス"
        Me.ColumnStd.Name = "ColumnStd"
        Me.ColumnStd.ReadOnly = True
        Me.ColumnStd.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable
        Me.ColumnStd.Width = 60
        '
        'ColumnPitch
        '
        Me.ColumnPitch.AutoSizeMode = System.Windows.Forms.DataGridViewAutoSizeColumnMode.Fill
        DataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleRight
        DataGridViewCellStyle2.Font = New System.Drawing.Font("Calibri", 14.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        DataGridViewCellStyle2.Format = "N0"
        DataGridViewCellStyle2.NullValue = Nothing
        Me.ColumnPitch.DefaultCellStyle = DataGridViewCellStyle2
        Me.ColumnPitch.HeaderText = "理論ピッチ"
        Me.ColumnPitch.Name = "ColumnPitch"
        Me.ColumnPitch.SortMode = System.Windows.Forms.DataGridViewColumnSortMode.NotSortable
        '
        'frmPopHotJump
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(526, 524)
        Me.Controls.Add(Me.Button1)
        Me.Controls.Add(Me.cmbDashStyle)
        Me.Controls.Add(Me.nudLineWidth)
        Me.Controls.Add(Me.Label3)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.nudFontSize)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.LabelIconForDraw)
        Me.Controls.Add(Me.LabelIcon)
        Me.Controls.Add(Me.dgvPitch)
        Me.Controls.Add(Me.dgvCoil)
        Me.Controls.Add(Me.btnBackward)
        Me.Controls.Add(Me.btnForward)
        Me.Controls.Add(Me.CheckBox14)
        Me.Controls.Add(Me.CheckBox13)
        Me.Controls.Add(Me.CheckBox7)
        Me.Controls.Add(Me.CheckBox12)
        Me.Controls.Add(Me.CheckBox6)
        Me.Controls.Add(Me.CheckBox11)
        Me.Controls.Add(Me.CheckBox5)
        Me.Controls.Add(Me.CheckBox10)
        Me.Controls.Add(Me.CheckBox4)
        Me.Controls.Add(Me.CheckBox9)
        Me.Controls.Add(Me.CheckBox3)
        Me.Controls.Add(Me.CheckBox8)
        Me.Controls.Add(Me.CheckBox2)
        Me.Controls.Add(Me.CheckBox1)
        Me.Controls.Add(Me.TextBox1)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow
        Me.Name = "frmPopHotJump"
        Me.Text = "frmPopHotJump"
        CType(Me.dgvCoil, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvPitch, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nudFontSize, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.nudLineWidth, System.ComponentModel.ISupportInitialize).EndInit()
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
    Friend WithEvents CheckBox8 As Windows.Forms.CheckBox
    Friend WithEvents CheckBox9 As Windows.Forms.CheckBox
    Friend WithEvents CheckBox10 As Windows.Forms.CheckBox
    Friend WithEvents CheckBox11 As Windows.Forms.CheckBox
    Friend WithEvents CheckBox12 As Windows.Forms.CheckBox
    Friend WithEvents CheckBox13 As Windows.Forms.CheckBox
    Friend WithEvents CheckBox14 As Windows.Forms.CheckBox
    Friend WithEvents btnForward As Windows.Forms.Button
    Friend WithEvents btnBackward As Windows.Forms.Button
    Friend WithEvents dgvCoil As Windows.Forms.DataGridView
    Friend WithEvents dgvPitch As Windows.Forms.DataGridView
    Friend WithEvents LabelIcon As Windows.Forms.Label
    Friend WithEvents LabelIconForDraw As Windows.Forms.Label
    Friend WithEvents TextBox1 As Windows.Forms.TextBox
    Friend WithEvents Column1 As Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents Column2 As Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents Label1 As Windows.Forms.Label
    Friend WithEvents nudFontSize As Windows.Forms.NumericUpDown
    Friend WithEvents Label2 As Windows.Forms.Label
    Friend WithEvents nudLineWidth As Windows.Forms.NumericUpDown
    Friend WithEvents Label3 As Windows.Forms.Label
    Friend WithEvents cmbDashStyle As Windows.Forms.ComboBox
    Friend WithEvents Button1 As Windows.Forms.Button
    Friend WithEvents ColumnStd As Windows.Forms.DataGridViewTextBoxColumn
    Friend WithEvents ColumnPitch As Windows.Forms.DataGridViewTextBoxColumn
End Class
