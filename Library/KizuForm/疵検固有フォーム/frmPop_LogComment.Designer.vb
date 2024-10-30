<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPop_LogComment
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
		Me.btnClose = New System.Windows.Forms.Button
		Me.btnInsert = New System.Windows.Forms.Button
		Me.btnUpdate = New System.Windows.Forms.Button
		Me.btnDelete = New System.Windows.Forms.Button
		Me.dtpStartDay = New System.Windows.Forms.DateTimePicker
		Me.dtpStartTime = New System.Windows.Forms.DateTimePicker
		Me.Label1 = New System.Windows.Forms.Label
		Me.Label2 = New System.Windows.Forms.Label
		Me.cmbKind = New System.Windows.Forms.ComboBox
		Me.Label3 = New System.Windows.Forms.Label
		Me.cmbPcName = New System.Windows.Forms.ComboBox
		Me.txtTask = New System.Windows.Forms.TextBox
		Me.Label4 = New System.Windows.Forms.Label
		Me.txtMsg = New System.Windows.Forms.TextBox
		Me.Label5 = New System.Windows.Forms.Label
		Me.SuspendLayout()
		'
		'btnClose
		'
		Me.btnClose.Font = New System.Drawing.Font("ÇlÇr ñæí©", 11.25!, System.Drawing.FontStyle.Bold)
		Me.btnClose.Location = New System.Drawing.Point(672, 4)
		Me.btnClose.Name = "btnClose"
		Me.btnClose.Size = New System.Drawing.Size(104, 32)
		Me.btnClose.TabIndex = 10
		Me.btnClose.Text = "F12:ï¬Ç∂ÇÈ"
		Me.btnClose.UseVisualStyleBackColor = True
		'
		'btnInsert
		'
		Me.btnInsert.Font = New System.Drawing.Font("ÇlÇr ñæí©", 11.25!, System.Drawing.FontStyle.Bold)
		Me.btnInsert.Location = New System.Drawing.Point(552, 4)
		Me.btnInsert.Name = "btnInsert"
		Me.btnInsert.Size = New System.Drawing.Size(112, 32)
		Me.btnInsert.TabIndex = 7
		Me.btnInsert.TabStop = False
		Me.btnInsert.Text = "F1:êVãKí«â¡"
		Me.btnInsert.UseVisualStyleBackColor = True
		'
		'btnUpdate
		'
		Me.btnUpdate.Font = New System.Drawing.Font("ÇlÇr ñæí©", 11.25!, System.Drawing.FontStyle.Bold)
		Me.btnUpdate.Location = New System.Drawing.Point(552, 40)
		Me.btnUpdate.Name = "btnUpdate"
		Me.btnUpdate.Size = New System.Drawing.Size(112, 32)
		Me.btnUpdate.TabIndex = 8
		Me.btnUpdate.TabStop = False
		Me.btnUpdate.Text = "F2:èCê≥"
		Me.btnUpdate.UseVisualStyleBackColor = True
		'
		'btnDelete
		'
		Me.btnDelete.Font = New System.Drawing.Font("ÇlÇr ñæí©", 11.25!, System.Drawing.FontStyle.Bold)
		Me.btnDelete.Location = New System.Drawing.Point(552, 76)
		Me.btnDelete.Name = "btnDelete"
		Me.btnDelete.Size = New System.Drawing.Size(112, 32)
		Me.btnDelete.TabIndex = 9
		Me.btnDelete.TabStop = False
		Me.btnDelete.Text = "F4:çÌèú"
		Me.btnDelete.UseVisualStyleBackColor = True
		'
		'dtpStartDay
		'
		Me.dtpStartDay.CustomFormat = "yyyy/MM/dd"
		Me.dtpStartDay.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.dtpStartDay.Format = System.Windows.Forms.DateTimePickerFormat.Custom
		Me.dtpStartDay.ImeMode = System.Windows.Forms.ImeMode.Disable
		Me.dtpStartDay.Location = New System.Drawing.Point(4, 20)
		Me.dtpStartDay.Name = "dtpStartDay"
		Me.dtpStartDay.Size = New System.Drawing.Size(140, 26)
		Me.dtpStartDay.TabIndex = 1
		'
		'dtpStartTime
		'
		Me.dtpStartTime.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.dtpStartTime.Format = System.Windows.Forms.DateTimePickerFormat.Time
		Me.dtpStartTime.ImeMode = System.Windows.Forms.ImeMode.Disable
		Me.dtpStartTime.Location = New System.Drawing.Point(148, 20)
		Me.dtpStartTime.Name = "dtpStartTime"
		Me.dtpStartTime.ShowUpDown = True
		Me.dtpStartTime.Size = New System.Drawing.Size(116, 26)
		Me.dtpStartTime.TabIndex = 2
		'
		'Label1
		'
		Me.Label1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.Label1.Location = New System.Drawing.Point(8, 4)
		Me.Label1.Name = "Label1"
		Me.Label1.Size = New System.Drawing.Size(168, 16)
		Me.Label1.TabIndex = 0
		Me.Label1.Text = "ø∞ƒ/åüçıópìùàÍéûçè"
		'
		'Label2
		'
		Me.Label2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.Label2.Location = New System.Drawing.Point(8, 52)
		Me.Label2.Name = "Label2"
		Me.Label2.Size = New System.Drawing.Size(42, 16)
		Me.Label2.TabIndex = 9
		Me.Label2.Text = "ãÊï™"
		'
		'cmbKind
		'
		Me.cmbKind.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
		Me.cmbKind.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, System.Drawing.FontStyle.Bold)
		Me.cmbKind.FormattingEnabled = True
		Me.cmbKind.Location = New System.Drawing.Point(4, 68)
		Me.cmbKind.Name = "cmbKind"
		Me.cmbKind.Size = New System.Drawing.Size(140, 27)
		Me.cmbKind.TabIndex = 3
		'
		'Label3
		'
		Me.Label3.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.Label3.Location = New System.Drawing.Point(152, 52)
		Me.Label3.Name = "Label3"
		Me.Label3.Size = New System.Drawing.Size(43, 16)
		Me.Label3.TabIndex = 9
		Me.Label3.Text = "PCñº"
		'
		'cmbPcName
		'
		Me.cmbPcName.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
		Me.cmbPcName.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, System.Drawing.FontStyle.Bold)
		Me.cmbPcName.FormattingEnabled = True
		Me.cmbPcName.Items.AddRange(New Object() {"ëSî ", "øÃƒ≥™±", " ∞ƒﬁ≥™±"})
		Me.cmbPcName.Location = New System.Drawing.Point(148, 68)
		Me.cmbPcName.Name = "cmbPcName"
		Me.cmbPcName.Size = New System.Drawing.Size(188, 27)
		Me.cmbPcName.TabIndex = 4
		'
		'txtTask
		'
		Me.txtTask.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, System.Drawing.FontStyle.Bold)
		Me.txtTask.ImeMode = System.Windows.Forms.ImeMode.Off
		Me.txtTask.Location = New System.Drawing.Point(340, 68)
		Me.txtTask.Name = "txtTask"
		Me.txtTask.Size = New System.Drawing.Size(208, 26)
		Me.txtTask.TabIndex = 5
		'
		'Label4
		'
		Me.Label4.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.Label4.Location = New System.Drawing.Point(344, 52)
		Me.Label4.Name = "Label4"
		Me.Label4.Size = New System.Drawing.Size(59, 16)
		Me.Label4.TabIndex = 9
		Me.Label4.Text = "î≠ê∂å≥"
		'
		'txtMsg
		'
		Me.txtMsg.Font = New System.Drawing.Font("ÇlÇr ñæí©", 14.25!, System.Drawing.FontStyle.Bold)
		Me.txtMsg.ImeMode = System.Windows.Forms.ImeMode.[On]
		Me.txtMsg.Location = New System.Drawing.Point(4, 116)
		Me.txtMsg.Name = "txtMsg"
		Me.txtMsg.Size = New System.Drawing.Size(772, 26)
		Me.txtMsg.TabIndex = 6
		'
		'Label5
		'
		Me.Label5.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.Label5.Location = New System.Drawing.Point(8, 100)
		Me.Label5.Name = "Label5"
		Me.Label5.Size = New System.Drawing.Size(93, 16)
		Me.Label5.TabIndex = 9
		Me.Label5.Text = "ÉÅÉbÉZÅ[ÉW"
		'
		'frmPop_LogComment
		'
		Me.ClientSize = New System.Drawing.Size(780, 146)
		Me.Controls.Add(Me.txtMsg)
		Me.Controls.Add(Me.txtTask)
		Me.Controls.Add(Me.cmbPcName)
		Me.Controls.Add(Me.Label5)
		Me.Controls.Add(Me.Label4)
		Me.Controls.Add(Me.Label3)
		Me.Controls.Add(Me.cmbKind)
		Me.Controls.Add(Me.Label2)
		Me.Controls.Add(Me.Label1)
		Me.Controls.Add(Me.dtpStartDay)
		Me.Controls.Add(Me.dtpStartTime)
		Me.Controls.Add(Me.btnDelete)
		Me.Controls.Add(Me.btnUpdate)
		Me.Controls.Add(Me.btnInsert)
		Me.Controls.Add(Me.btnClose)
		Me.Name = "frmPop_LogComment"
		Me.Text = "åÃè·ÉçÉO ÉRÉÅÉìÉgï“èWâÊñ  "
		Me.TopMost = False
		Me.tProp_Moveable = True
		Me.ResumeLayout(False)
		Me.PerformLayout()

	End Sub
	Friend WithEvents btnClose As System.Windows.Forms.Button
	Friend WithEvents btnInsert As System.Windows.Forms.Button
	Friend WithEvents btnUpdate As System.Windows.Forms.Button
	Friend WithEvents btnDelete As System.Windows.Forms.Button
	Protected Friend WithEvents dtpStartDay As System.Windows.Forms.DateTimePicker
	Protected Friend WithEvents dtpStartTime As System.Windows.Forms.DateTimePicker
	Friend WithEvents Label1 As System.Windows.Forms.Label
	Friend WithEvents Label2 As System.Windows.Forms.Label
	Friend WithEvents cmbKind As System.Windows.Forms.ComboBox
	Friend WithEvents Label3 As System.Windows.Forms.Label
	Friend WithEvents cmbPcName As System.Windows.Forms.ComboBox
	Friend WithEvents txtTask As System.Windows.Forms.TextBox
	Friend WithEvents Label4 As System.Windows.Forms.Label
	Friend WithEvents txtMsg As System.Windows.Forms.TextBox
	Friend WithEvents Label5 As System.Windows.Forms.Label

End Class
