<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPop_Search
	Inherits tClassForm.frmBase_Popup

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
        Me.chkDateSet = New System.Windows.Forms.CheckBox
        Me.dtpDateEnd = New System.Windows.Forms.DateTimePicker
        Me.Label3 = New System.Windows.Forms.Label
        Me.dtpDateStart = New System.Windows.Forms.DateTimePicker
        Me.txtSaveName = New System.Windows.Forms.TextBox
        Me.txtCoil = New System.Windows.Forms.TextBox
        Me.GroupBox1 = New System.Windows.Forms.GroupBox
        Me.rdoSortDesc = New System.Windows.Forms.RadioButton
        Me.rdoSortAsc = New System.Windows.Forms.RadioButton
        Me.grpDate = New System.Windows.Forms.GroupBox
        Me.chkTimeSet = New System.Windows.Forms.CheckBox
        Me.grpTime = New System.Windows.Forms.GroupBox
        Me.dtpTimeEnd = New System.Windows.Forms.DateTimePicker
        Me.Label4 = New System.Windows.Forms.Label
        Me.dtpTimeStart = New System.Windows.Forms.DateTimePicker
        Me.grpSearch = New System.Windows.Forms.GroupBox
        Me.Label2 = New System.Windows.Forms.Label
        Me.Label1 = New System.Windows.Forms.Label
        Me.grpDisp = New System.Windows.Forms.GroupBox
        Me.GroupBox2 = New System.Windows.Forms.GroupBox
        Me.chkNagate = New System.Windows.Forms.CheckBox
        Me.chkHaba = New System.Windows.Forms.CheckBox
        Me.GroupBox3 = New System.Windows.Forms.GroupBox
        Me.btnGradeAllCancel = New System.Windows.Forms.Button
        Me.btnGradeAllSelect = New System.Windows.Forms.Button
        Me.dgvDispGrade = New System.Windows.Forms.DataGridView
        Me.GroupBox4 = New System.Windows.Forms.GroupBox
        Me.btnTypeAllCancel = New System.Windows.Forms.Button
        Me.btnTypeAllSelect = New System.Windows.Forms.Button
        Me.dgvDispType = New System.Windows.Forms.DataGridView
        Me.picSearch = New System.Windows.Forms.PictureBox
        Me.pnlMsg = New System.Windows.Forms.Panel
        Me.prbMsg = New System.Windows.Forms.ProgressBar
        Me.txtMsg = New System.Windows.Forms.TextBox
        Me.lblGuide = New System.Windows.Forms.Label
        Me.btnCancel = New System.Windows.Forms.Button
        Me.btnSelect = New System.Windows.Forms.Button
        Me.btnEnd = New System.Windows.Forms.Button
        Me.btnSearch = New System.Windows.Forms.Button
        Me.dgvResult = New tClassLibrary.tCnt.tCnt_DataGridView
        Me.btnCsv = New System.Windows.Forms.Button
        Me.GroupBox1.SuspendLayout()
        Me.grpDate.SuspendLayout()
        Me.grpTime.SuspendLayout()
        Me.grpSearch.SuspendLayout()
        Me.grpDisp.SuspendLayout()
        Me.GroupBox2.SuspendLayout()
        Me.GroupBox3.SuspendLayout()
        CType(Me.dgvDispGrade, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox4.SuspendLayout()
        CType(Me.dgvDispType, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.picSearch, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.pnlMsg.SuspendLayout()
        CType(Me.dgvResult, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'chkDateSet
        '
        Me.chkDateSet.AutoSize = True
        Me.chkDateSet.Location = New System.Drawing.Point(12, 24)
        Me.chkDateSet.Name = "chkDateSet"
        Me.chkDateSet.Size = New System.Drawing.Size(180, 20)
        Me.chkDateSet.TabIndex = 3
        Me.chkDateSet.Text = "ì˙ïtÉtÉBÉãÉ^Å[égóp"
        Me.chkDateSet.UseVisualStyleBackColor = True
        '
        'dtpDateEnd
        '
        Me.dtpDateEnd.Enabled = False
        Me.dtpDateEnd.Format = System.Windows.Forms.DateTimePickerFormat.[Short]
        Me.dtpDateEnd.Location = New System.Drawing.Point(164, 52)
        Me.dtpDateEnd.Name = "dtpDateEnd"
        Me.dtpDateEnd.Size = New System.Drawing.Size(124, 23)
        Me.dtpDateEnd.TabIndex = 2
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Location = New System.Drawing.Point(136, 56)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(25, 16)
        Me.Label3.TabIndex = 1
        Me.Label3.Text = "Å`"
        '
        'dtpDateStart
        '
        Me.dtpDateStart.Enabled = False
        Me.dtpDateStart.Format = System.Windows.Forms.DateTimePickerFormat.[Short]
        Me.dtpDateStart.Location = New System.Drawing.Point(8, 52)
        Me.dtpDateStart.Name = "dtpDateStart"
        Me.dtpDateStart.Size = New System.Drawing.Size(124, 23)
        Me.dtpDateStart.TabIndex = 0
        '
        'txtSaveName
        '
        Me.txtSaveName.Location = New System.Drawing.Point(152, 44)
        Me.txtSaveName.MaxLength = 20
        Me.txtSaveName.Name = "txtSaveName"
        Me.txtSaveName.Size = New System.Drawing.Size(188, 23)
        Me.txtSaveName.TabIndex = 4
        Me.txtSaveName.Text = "12345678901234567123"
        '
        'txtCoil
        '
        Me.txtCoil.Location = New System.Drawing.Point(12, 44)
        Me.txtCoil.MaxLength = 12
        Me.txtCoil.Name = "txtCoil"
        Me.txtCoil.Size = New System.Drawing.Size(132, 23)
        Me.txtCoil.TabIndex = 3
        Me.txtCoil.Text = "123456789012"
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.rdoSortDesc)
        Me.GroupBox1.Controls.Add(Me.rdoSortAsc)
        Me.GroupBox1.Location = New System.Drawing.Point(612, 76)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(156, 84)
        Me.GroupBox1.TabIndex = 2
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "åüçıåãâ ï\é¶èá"
        '
        'rdoSortDesc
        '
        Me.rdoSortDesc.AutoSize = True
        Me.rdoSortDesc.Location = New System.Drawing.Point(84, 40)
        Me.rdoSortDesc.Name = "rdoSortDesc"
        Me.rdoSortDesc.Size = New System.Drawing.Size(60, 20)
        Me.rdoSortDesc.TabIndex = 1
        Me.rdoSortDesc.TabStop = True
        Me.rdoSortDesc.Text = "ç~èá"
        Me.rdoSortDesc.UseVisualStyleBackColor = True
        '
        'rdoSortAsc
        '
        Me.rdoSortAsc.AutoSize = True
        Me.rdoSortAsc.Location = New System.Drawing.Point(12, 40)
        Me.rdoSortAsc.Name = "rdoSortAsc"
        Me.rdoSortAsc.Size = New System.Drawing.Size(60, 20)
        Me.rdoSortAsc.TabIndex = 0
        Me.rdoSortAsc.TabStop = True
        Me.rdoSortAsc.Text = "è∏èá"
        Me.rdoSortAsc.UseVisualStyleBackColor = True
        '
        'grpDate
        '
        Me.grpDate.Controls.Add(Me.chkDateSet)
        Me.grpDate.Controls.Add(Me.dtpDateEnd)
        Me.grpDate.Controls.Add(Me.Label3)
        Me.grpDate.Controls.Add(Me.dtpDateStart)
        Me.grpDate.Location = New System.Drawing.Point(12, 76)
        Me.grpDate.Name = "grpDate"
        Me.grpDate.Size = New System.Drawing.Size(296, 84)
        Me.grpDate.TabIndex = 5
        Me.grpDate.TabStop = False
        Me.grpDate.Text = "ì˙ïtÉtÉBÉãÉ^Å["
        '
        'chkTimeSet
        '
        Me.chkTimeSet.AutoSize = True
        Me.chkTimeSet.Location = New System.Drawing.Point(12, 24)
        Me.chkTimeSet.Name = "chkTimeSet"
        Me.chkTimeSet.Size = New System.Drawing.Size(180, 20)
        Me.chkTimeSet.TabIndex = 4
        Me.chkTimeSet.Text = "éûçèÉtÉBÉãÉ^Å[égóp"
        Me.chkTimeSet.UseVisualStyleBackColor = True
        '
        'grpTime
        '
        Me.grpTime.Controls.Add(Me.chkTimeSet)
        Me.grpTime.Controls.Add(Me.dtpTimeEnd)
        Me.grpTime.Controls.Add(Me.Label4)
        Me.grpTime.Controls.Add(Me.dtpTimeStart)
        Me.grpTime.Location = New System.Drawing.Point(312, 76)
        Me.grpTime.Name = "grpTime"
        Me.grpTime.Size = New System.Drawing.Size(296, 84)
        Me.grpTime.TabIndex = 6
        Me.grpTime.TabStop = False
        Me.grpTime.Text = "éûä‘ÉtÉBÉãÉ^Å["
        '
        'dtpTimeEnd
        '
        Me.dtpTimeEnd.Enabled = False
        Me.dtpTimeEnd.Format = System.Windows.Forms.DateTimePickerFormat.Time
        Me.dtpTimeEnd.Location = New System.Drawing.Point(164, 52)
        Me.dtpTimeEnd.Name = "dtpTimeEnd"
        Me.dtpTimeEnd.ShowUpDown = True
        Me.dtpTimeEnd.Size = New System.Drawing.Size(124, 23)
        Me.dtpTimeEnd.TabIndex = 2
        Me.dtpTimeEnd.Value = New Date(2007, 7, 1, 23, 59, 59, 0)
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Location = New System.Drawing.Point(136, 56)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(25, 16)
        Me.Label4.TabIndex = 1
        Me.Label4.Text = "Å`"
        '
        'dtpTimeStart
        '
        Me.dtpTimeStart.Enabled = False
        Me.dtpTimeStart.Format = System.Windows.Forms.DateTimePickerFormat.Time
        Me.dtpTimeStart.Location = New System.Drawing.Point(8, 52)
        Me.dtpTimeStart.Name = "dtpTimeStart"
        Me.dtpTimeStart.ShowUpDown = True
        Me.dtpTimeStart.Size = New System.Drawing.Size(124, 23)
        Me.dtpTimeStart.TabIndex = 0
        Me.dtpTimeStart.Value = New Date(2007, 7, 19, 0, 0, 0, 0)
        '
        'grpSearch
        '
        Me.grpSearch.Controls.Add(Me.grpTime)
        Me.grpSearch.Controls.Add(Me.grpDate)
        Me.grpSearch.Controls.Add(Me.txtSaveName)
        Me.grpSearch.Controls.Add(Me.txtCoil)
        Me.grpSearch.Controls.Add(Me.GroupBox1)
        Me.grpSearch.Controls.Add(Me.Label2)
        Me.grpSearch.Controls.Add(Me.Label1)
        Me.grpSearch.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.grpSearch.Location = New System.Drawing.Point(20, 20)
        Me.grpSearch.Name = "grpSearch"
        Me.grpSearch.Size = New System.Drawing.Size(784, 168)
        Me.grpSearch.TabIndex = 0
        Me.grpSearch.TabStop = False
        Me.grpSearch.Text = "åüçıèåè"
        '
        'Label2
        '
        Me.Label2.AutoSize = True
        Me.Label2.Location = New System.Drawing.Point(152, 24)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(127, 16)
        Me.Label2.TabIndex = 1
        Me.Label2.Text = "ï€ë∂ÉtÉ@ÉCÉãñº"
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.Location = New System.Drawing.Point(12, 24)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(77, 16)
        Me.Label1.TabIndex = 0
        Me.Label1.Text = "ÉRÉCÉãNO"
        '
        'grpDisp
        '
        Me.grpDisp.Controls.Add(Me.GroupBox2)
        Me.grpDisp.Controls.Add(Me.GroupBox3)
        Me.grpDisp.Controls.Add(Me.GroupBox4)
        Me.grpDisp.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.grpDisp.Location = New System.Drawing.Point(20, 196)
        Me.grpDisp.Name = "grpDisp"
        Me.grpDisp.Size = New System.Drawing.Size(784, 300)
        Me.grpDisp.TabIndex = 1
        Me.grpDisp.TabStop = False
        Me.grpDisp.Text = "ï\é¶èåè"
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.chkNagate)
        Me.GroupBox2.Controls.Add(Me.chkHaba)
        Me.GroupBox2.Location = New System.Drawing.Point(624, 24)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(156, 268)
        Me.GroupBox2.TabIndex = 8
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "ìWäJï˚ñ@ëIë"
        '
        'chkNagate
        '
        Me.chkNagate.Location = New System.Drawing.Point(8, 40)
        Me.chkNagate.Name = "chkNagate"
        Me.chkNagate.Size = New System.Drawing.Size(132, 36)
        Me.chkNagate.TabIndex = 2
        Me.chkNagate.Text = "í∑éËï˚å¸îΩì]" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "(TÅÀF)" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10)
        Me.chkNagate.UseVisualStyleBackColor = True
        '
        'chkHaba
        '
        Me.chkHaba.AutoSize = True
        Me.chkHaba.Location = New System.Drawing.Point(8, 88)
        Me.chkHaba.Name = "chkHaba"
        Me.chkHaba.Size = New System.Drawing.Size(112, 36)
        Me.chkHaba.TabIndex = 3
        Me.chkHaba.Text = "ïùï˚å¸îΩì]" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "(WÅÀD)" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10)
        Me.chkHaba.UseVisualStyleBackColor = True
        '
        'GroupBox3
        '
        Me.GroupBox3.Controls.Add(Me.btnGradeAllCancel)
        Me.GroupBox3.Controls.Add(Me.btnGradeAllSelect)
        Me.GroupBox3.Controls.Add(Me.dgvDispGrade)
        Me.GroupBox3.Location = New System.Drawing.Point(320, 24)
        Me.GroupBox3.Name = "GroupBox3"
        Me.GroupBox3.Size = New System.Drawing.Size(300, 268)
        Me.GroupBox3.TabIndex = 6
        Me.GroupBox3.TabStop = False
        Me.GroupBox3.Text = "ÉOÉåÅ[Éhï\é¶ÉtÉBÉãÉ^Å["
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
        'dgvDispGrade
        '
        Me.dgvDispGrade.AllowUserToAddRows = False
        Me.dgvDispGrade.AllowUserToDeleteRows = False
        Me.dgvDispGrade.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvDispGrade.Location = New System.Drawing.Point(8, 21)
        Me.dgvDispGrade.Name = "dgvDispGrade"
        Me.dgvDispGrade.ReadOnly = True
        Me.dgvDispGrade.RowTemplate.Height = 21
        Me.dgvDispGrade.Size = New System.Drawing.Size(284, 199)
        Me.dgvDispGrade.TabIndex = 1
        '
        'GroupBox4
        '
        Me.GroupBox4.Controls.Add(Me.btnTypeAllCancel)
        Me.GroupBox4.Controls.Add(Me.btnTypeAllSelect)
        Me.GroupBox4.Controls.Add(Me.dgvDispType)
        Me.GroupBox4.Location = New System.Drawing.Point(12, 24)
        Me.GroupBox4.Name = "GroupBox4"
        Me.GroupBox4.Size = New System.Drawing.Size(300, 268)
        Me.GroupBox4.TabIndex = 5
        Me.GroupBox4.TabStop = False
        Me.GroupBox4.Text = "·réÌï\é¶ÉtÉBÉãÉ^Å["
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
        'btnTypeAllSelect
        '
        Me.btnTypeAllSelect.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnTypeAllSelect.Location = New System.Drawing.Point(56, 228)
        Me.btnTypeAllSelect.Name = "btnTypeAllSelect"
        Me.btnTypeAllSelect.Size = New System.Drawing.Size(116, 32)
        Me.btnTypeAllSelect.TabIndex = 5
        Me.btnTypeAllSelect.Text = "ëSëIë"
        Me.btnTypeAllSelect.UseVisualStyleBackColor = True
        '
        'dgvDispType
        '
        Me.dgvDispType.AllowUserToAddRows = False
        Me.dgvDispType.AllowUserToDeleteRows = False
        Me.dgvDispType.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvDispType.Location = New System.Drawing.Point(8, 20)
        Me.dgvDispType.Name = "dgvDispType"
        Me.dgvDispType.ReadOnly = True
        Me.dgvDispType.RowTemplate.Height = 21
        Me.dgvDispType.Size = New System.Drawing.Size(284, 200)
        Me.dgvDispType.TabIndex = 0
        '
        'picSearch
        '
        Me.picSearch.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.picSearch.Location = New System.Drawing.Point(12, 12)
        Me.picSearch.Name = "picSearch"
        Me.picSearch.Size = New System.Drawing.Size(800, 492)
        Me.picSearch.TabIndex = 18
        Me.picSearch.TabStop = False
        '
        'pnlMsg
        '
        Me.pnlMsg.Controls.Add(Me.prbMsg)
        Me.pnlMsg.Controls.Add(Me.txtMsg)
        Me.pnlMsg.Location = New System.Drawing.Point(328, 512)
        Me.pnlMsg.Name = "pnlMsg"
        Me.pnlMsg.Size = New System.Drawing.Size(320, 64)
        Me.pnlMsg.TabIndex = 17
        Me.pnlMsg.Visible = False
        '
        'prbMsg
        '
        Me.prbMsg.Dock = System.Windows.Forms.DockStyle.Bottom
        Me.prbMsg.Location = New System.Drawing.Point(0, 44)
        Me.prbMsg.Name = "prbMsg"
        Me.prbMsg.Size = New System.Drawing.Size(320, 20)
        Me.prbMsg.Style = System.Windows.Forms.ProgressBarStyle.Continuous
        Me.prbMsg.TabIndex = 1
        '
        'txtMsg
        '
        Me.txtMsg.BackColor = System.Drawing.Color.Khaki
        Me.txtMsg.Dock = System.Windows.Forms.DockStyle.Top
        Me.txtMsg.Font = New System.Drawing.Font("ÇlÇr ñæí©", 20.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.txtMsg.Location = New System.Drawing.Point(0, 0)
        Me.txtMsg.Multiline = True
        Me.txtMsg.Name = "txtMsg"
        Me.txtMsg.Size = New System.Drawing.Size(320, 44)
        Me.txtMsg.TabIndex = 0
        '
        'lblGuide
        '
        Me.lblGuide.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblGuide.Location = New System.Drawing.Point(16, 508)
        Me.lblGuide.Name = "lblGuide"
        Me.lblGuide.Size = New System.Drawing.Size(508, 28)
        Me.lblGuide.TabIndex = 16
        Me.lblGuide.Text = "10åèå©Ç¬Ç©ÇËÇ‹ÇµÇΩÅB"
        Me.lblGuide.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'btnCancel
        '
        Me.btnCancel.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnCancel.Location = New System.Drawing.Point(820, 508)
        Me.btnCancel.Name = "btnCancel"
        Me.btnCancel.Size = New System.Drawing.Size(116, 32)
        Me.btnCancel.TabIndex = 6
        Me.btnCancel.Text = "ÉLÉÉÉìÉZÉã"
        Me.btnCancel.UseVisualStyleBackColor = True
        '
        'btnSelect
        '
        Me.btnSelect.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnSelect.Location = New System.Drawing.Point(696, 508)
        Me.btnSelect.Name = "btnSelect"
        Me.btnSelect.Size = New System.Drawing.Size(116, 32)
        Me.btnSelect.TabIndex = 5
        Me.btnSelect.Text = "ëIë"
        Me.btnSelect.UseVisualStyleBackColor = True
        '
        'btnEnd
        '
        Me.btnEnd.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.btnEnd.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnEnd.Location = New System.Drawing.Point(820, 58)
        Me.btnEnd.Name = "btnEnd"
        Me.btnEnd.Size = New System.Drawing.Size(116, 32)
        Me.btnEnd.TabIndex = 3
        Me.btnEnd.Text = "ï¬Ç∂ÇÈ"
        Me.btnEnd.UseVisualStyleBackColor = True
        '
        'btnSearch
        '
        Me.btnSearch.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnSearch.Location = New System.Drawing.Point(820, 16)
        Me.btnSearch.Name = "btnSearch"
        Me.btnSearch.Size = New System.Drawing.Size(116, 32)
        Me.btnSearch.TabIndex = 2
        Me.btnSearch.Text = "åüçı"
        Me.btnSearch.UseVisualStyleBackColor = True
        '
        'dgvResult
        '
        Me.dgvResult.Location = New System.Drawing.Point(12, 548)
        Me.dgvResult.Name = "dgvResult"
        Me.dgvResult.RowTemplate.Height = 21
        Me.dgvResult.Size = New System.Drawing.Size(920, 328)
        Me.dgvResult.TabIndex = 7
        '
        'btnCsv
        '
        Me.btnCsv.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.btnCsv.Location = New System.Drawing.Point(572, 508)
        Me.btnCsv.Name = "btnCsv"
        Me.btnCsv.Size = New System.Drawing.Size(116, 32)
        Me.btnCsv.TabIndex = 4
        Me.btnCsv.Text = "CSVèoóÕ"
        Me.btnCsv.UseVisualStyleBackColor = True
        '
        'frmPop_Search
        '
        Me.ClientSize = New System.Drawing.Size(947, 889)
        Me.Controls.Add(Me.pnlMsg)
        Me.Controls.Add(Me.dgvResult)
        Me.Controls.Add(Me.grpSearch)
        Me.Controls.Add(Me.grpDisp)
        Me.Controls.Add(Me.picSearch)
        Me.Controls.Add(Me.lblGuide)
        Me.Controls.Add(Me.btnCancel)
        Me.Controls.Add(Me.btnSelect)
        Me.Controls.Add(Me.btnEnd)
        Me.Controls.Add(Me.btnSearch)
        Me.Controls.Add(Me.btnCsv)
        Me.Name = "frmPop_Search"
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox1.PerformLayout()
        Me.grpDate.ResumeLayout(False)
        Me.grpDate.PerformLayout()
        Me.grpTime.ResumeLayout(False)
        Me.grpTime.PerformLayout()
        Me.grpSearch.ResumeLayout(False)
        Me.grpSearch.PerformLayout()
        Me.grpDisp.ResumeLayout(False)
        Me.GroupBox2.ResumeLayout(False)
        Me.GroupBox2.PerformLayout()
        Me.GroupBox3.ResumeLayout(False)
        CType(Me.dgvDispGrade, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox4.ResumeLayout(False)
        CType(Me.dgvDispType, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.picSearch, System.ComponentModel.ISupportInitialize).EndInit()
        Me.pnlMsg.ResumeLayout(False)
        Me.pnlMsg.PerformLayout()
        CType(Me.dgvResult, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
	Protected WithEvents btnEnd As System.Windows.Forms.Button
	Protected WithEvents Label3 As System.Windows.Forms.Label
	Protected WithEvents GroupBox1 As System.Windows.Forms.GroupBox
	Protected WithEvents grpDate As System.Windows.Forms.GroupBox
	Protected WithEvents grpTime As System.Windows.Forms.GroupBox
	Protected WithEvents Label4 As System.Windows.Forms.Label
	Protected WithEvents grpSearch As System.Windows.Forms.GroupBox
	Protected WithEvents Label2 As System.Windows.Forms.Label
	Protected WithEvents Label1 As System.Windows.Forms.Label
	Protected WithEvents grpDisp As System.Windows.Forms.GroupBox
	Protected WithEvents GroupBox2 As System.Windows.Forms.GroupBox
	Protected WithEvents GroupBox3 As System.Windows.Forms.GroupBox
	Protected WithEvents GroupBox4 As System.Windows.Forms.GroupBox
	Protected WithEvents picSearch As System.Windows.Forms.PictureBox
	Protected WithEvents pnlMsg As System.Windows.Forms.Panel
	Protected WithEvents rdoSortDesc As System.Windows.Forms.RadioButton
	Protected WithEvents rdoSortAsc As System.Windows.Forms.RadioButton
	Protected WithEvents chkDateSet As System.Windows.Forms.CheckBox
	Protected WithEvents dtpDateEnd As System.Windows.Forms.DateTimePicker
	Protected WithEvents dtpDateStart As System.Windows.Forms.DateTimePicker
	Protected WithEvents txtSaveName As System.Windows.Forms.TextBox
	Protected WithEvents txtCoil As System.Windows.Forms.TextBox
	Protected WithEvents chkTimeSet As System.Windows.Forms.CheckBox
	Protected WithEvents dtpTimeEnd As System.Windows.Forms.DateTimePicker
	Protected WithEvents dtpTimeStart As System.Windows.Forms.DateTimePicker
	Protected WithEvents btnGradeAllCancel As System.Windows.Forms.Button
	Protected WithEvents btnGradeAllSelect As System.Windows.Forms.Button
	Protected WithEvents dgvDispGrade As System.Windows.Forms.DataGridView
	Protected WithEvents btnTypeAllCancel As System.Windows.Forms.Button
	Protected WithEvents btnTypeAllSelect As System.Windows.Forms.Button
	Protected WithEvents dgvDispType As System.Windows.Forms.DataGridView
	Protected WithEvents prbMsg As System.Windows.Forms.ProgressBar
	Protected WithEvents txtMsg As System.Windows.Forms.TextBox
	Protected WithEvents lblGuide As System.Windows.Forms.Label
	Protected WithEvents btnCancel As System.Windows.Forms.Button
	Protected WithEvents btnSelect As System.Windows.Forms.Button
	Protected WithEvents btnSearch As System.Windows.Forms.Button
	Protected WithEvents chkNagate As System.Windows.Forms.CheckBox
	Protected WithEvents chkHaba As System.Windows.Forms.CheckBox
    Protected WithEvents dgvResult As tClassLibrary.tCnt.tCnt_DataGridView
    Protected WithEvents btnCsv As System.Windows.Forms.Button

End Class
