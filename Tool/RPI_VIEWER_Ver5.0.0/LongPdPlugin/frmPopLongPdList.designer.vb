<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPopLongPdList
	Inherits System.Windows.Forms.Form

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
		Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(frmPopLongPdList))
		Me.dgvLongPdList = New System.Windows.Forms.DataGridView()
		Me.btnEnd = New System.Windows.Forms.Button()
		Me.btnCSV = New System.Windows.Forms.Button()
		Me.LabelIcon = New System.Windows.Forms.Label()
		Me.btnSet = New System.Windows.Forms.Button()
		Me.Label1 = New System.Windows.Forms.Label()
		Me.nudFrom = New System.Windows.Forms.NumericUpDown()
		Me.nudTo = New System.Windows.Forms.NumericUpDown()
		Me.Label2 = New System.Windows.Forms.Label()
		Me.Label3 = New System.Windows.Forms.Label()
		Me.nudTolerance = New System.Windows.Forms.NumericUpDown()
		CType(Me.dgvLongPdList, System.ComponentModel.ISupportInitialize).BeginInit()
		CType(Me.nudFrom, System.ComponentModel.ISupportInitialize).BeginInit()
		CType(Me.nudTo, System.ComponentModel.ISupportInitialize).BeginInit()
		CType(Me.nudTolerance, System.ComponentModel.ISupportInitialize).BeginInit()
		Me.SuspendLayout()
		'
		'dgvLongPdList
		'
		Me.dgvLongPdList.AllowUserToAddRows = False
		Me.dgvLongPdList.AllowUserToDeleteRows = False
		Me.dgvLongPdList.Anchor = CType((((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Bottom) _
			Or System.Windows.Forms.AnchorStyles.Left) _
			Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.dgvLongPdList.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
		Me.dgvLongPdList.Location = New System.Drawing.Point(4, 36)
		Me.dgvLongPdList.Name = "dgvLongPdList"
		Me.dgvLongPdList.ReadOnly = True
		Me.dgvLongPdList.RowTemplate.Height = 21
		Me.dgvLongPdList.Size = New System.Drawing.Size(614, 237)
		Me.dgvLongPdList.TabIndex = 5
		'
		'btnEnd
		'
		Me.btnEnd.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.btnEnd.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.btnEnd.Location = New System.Drawing.Point(536, 6)
		Me.btnEnd.Name = "btnEnd"
		Me.btnEnd.Size = New System.Drawing.Size(82, 24)
		Me.btnEnd.TabIndex = 17
		Me.btnEnd.Text = "ï¬Ç∂ÇÈ"
		Me.btnEnd.UseVisualStyleBackColor = True
		'
		'btnCSV
		'
		Me.btnCSV.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.btnCSV.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.btnCSV.Location = New System.Drawing.Point(452, 6)
		Me.btnCSV.Name = "btnCSV"
		Me.btnCSV.Size = New System.Drawing.Size(80, 24)
		Me.btnCSV.TabIndex = 17
		Me.btnCSV.Text = "CSVèoóÕ"
		Me.btnCSV.UseVisualStyleBackColor = True
		'
		'LabelIcon
		'
		Me.LabelIcon.AutoSize = True
		Me.LabelIcon.Image = CType(resources.GetObject("LabelIcon.Image"), System.Drawing.Image)
		Me.LabelIcon.Location = New System.Drawing.Point(264, 134)
		Me.LabelIcon.Name = "LabelIcon"
		Me.LabelIcon.Size = New System.Drawing.Size(17, 12)
		Me.LabelIcon.TabIndex = 43
		Me.LabelIcon.Text = "   "
		Me.LabelIcon.Visible = False
		'
		'btnSet
		'
		Me.btnSet.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.btnSet.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.75!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.btnSet.Location = New System.Drawing.Point(372, 6)
		Me.btnSet.Name = "btnSet"
		Me.btnSet.Size = New System.Drawing.Size(76, 24)
		Me.btnSet.TabIndex = 17
		Me.btnSet.Text = "îΩâf"
		Me.btnSet.UseVisualStyleBackColor = True
		'
		'Label1
		'
		Me.Label1.AutoSize = True
		Me.Label1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.75!)
		Me.Label1.Location = New System.Drawing.Point(4, 4)
		Me.Label1.Name = "Label1"
		Me.Label1.Size = New System.Drawing.Size(63, 26)
		Me.Label1.TabIndex = 44
		Me.Label1.Text = "é¸ä˙îªíË" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "ÀﬂØ¡"
		Me.Label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
		'
		'nudFrom
		'
		Me.nudFrom.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
		Me.nudFrom.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.nudFrom.Location = New System.Drawing.Point(69, 8)
		Me.nudFrom.Maximum = New Decimal(New Integer() {99999, 0, 0, 0})
		Me.nudFrom.Name = "nudFrom"
		Me.nudFrom.Size = New System.Drawing.Size(68, 19)
		Me.nudFrom.TabIndex = 45
		Me.nudFrom.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
		'
		'nudTo
		'
		Me.nudTo.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
		Me.nudTo.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.nudTo.Location = New System.Drawing.Point(160, 8)
		Me.nudTo.Maximum = New Decimal(New Integer() {99999, 0, 0, 0})
		Me.nudTo.Name = "nudTo"
		Me.nudTo.Size = New System.Drawing.Size(68, 19)
		Me.nudTo.TabIndex = 45
		Me.nudTo.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
		'
		'Label2
		'
		Me.Label2.AutoSize = True
		Me.Label2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.75!)
		Me.Label2.Location = New System.Drawing.Point(136, 12)
		Me.Label2.Name = "Label2"
		Me.Label2.Size = New System.Drawing.Size(21, 13)
		Me.Label2.TabIndex = 44
		Me.Label2.Text = "Å`"
		'
		'Label3
		'
		Me.Label3.AutoSize = True
		Me.Label3.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.75!)
		Me.Label3.Location = New System.Drawing.Point(232, 4)
		Me.Label3.Name = "Label3"
		Me.Label3.Size = New System.Drawing.Size(63, 26)
		Me.Label3.TabIndex = 44
		Me.Label3.Text = "∏ﬁŸ∞Ãﬂâª" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "åˆç∑"
		Me.Label3.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
		'
		'nudTolerance
		'
		Me.nudTolerance.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
		Me.nudTolerance.Font = New System.Drawing.Font("ÇlÇr ñæí©", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.nudTolerance.Location = New System.Drawing.Point(296, 8)
		Me.nudTolerance.Maximum = New Decimal(New Integer() {99999, 0, 0, 0})
		Me.nudTolerance.Name = "nudTolerance"
		Me.nudTolerance.Size = New System.Drawing.Size(68, 19)
		Me.nudTolerance.TabIndex = 45
		Me.nudTolerance.TextAlign = System.Windows.Forms.HorizontalAlignment.Right
		'
		'frmPopLongPdList
		'
		Me.ClientSize = New System.Drawing.Size(624, 281)
		Me.Controls.Add(Me.nudTolerance)
		Me.Controls.Add(Me.nudTo)
		Me.Controls.Add(Me.nudFrom)
		Me.Controls.Add(Me.Label2)
		Me.Controls.Add(Me.Label3)
		Me.Controls.Add(Me.Label1)
		Me.Controls.Add(Me.LabelIcon)
		Me.Controls.Add(Me.dgvLongPdList)
		Me.Controls.Add(Me.btnSet)
		Me.Controls.Add(Me.btnCSV)
		Me.Controls.Add(Me.btnEnd)
		Me.MinimumSize = New System.Drawing.Size(640, 200)
		Me.Name = "frmPopLongPdList"
		Me.Text = "í∑é¸ä˙åáä◊àÍóóâÊñ "
		CType(Me.dgvLongPdList, System.ComponentModel.ISupportInitialize).EndInit()
		CType(Me.nudFrom, System.ComponentModel.ISupportInitialize).EndInit()
		CType(Me.nudTo, System.ComponentModel.ISupportInitialize).EndInit()
		CType(Me.nudTolerance, System.ComponentModel.ISupportInitialize).EndInit()
		Me.ResumeLayout(False)
		Me.PerformLayout()

	End Sub
	Friend WithEvents dgvLongPdList As System.Windows.Forms.DataGridView
	Friend WithEvents btnEnd As System.Windows.Forms.Button
	Friend WithEvents btnCSV As Windows.Forms.Button
	Friend WithEvents LabelIcon As Windows.Forms.Label
	Friend WithEvents btnSet As Windows.Forms.Button
	Friend WithEvents Label1 As Windows.Forms.Label
	Friend WithEvents nudFrom As Windows.Forms.NumericUpDown
	Friend WithEvents nudTo As Windows.Forms.NumericUpDown
	Friend WithEvents Label2 As Windows.Forms.Label
	Friend WithEvents Label3 As Windows.Forms.Label
	Friend WithEvents nudTolerance As Windows.Forms.NumericUpDown
End Class
