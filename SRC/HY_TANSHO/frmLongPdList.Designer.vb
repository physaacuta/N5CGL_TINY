<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmLongPdList
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
		Me.dgvLongPdList = New System.Windows.Forms.DataGridView()
		Me.btnEnd = New System.Windows.Forms.Button()
		Me.cmbID = New System.Windows.Forms.ComboBox()
		Me.lblCoilNo = New System.Windows.Forms.Label()
		Me.btnCSV = New System.Windows.Forms.Button()
		CType(Me.dgvLongPdList, System.ComponentModel.ISupportInitialize).BeginInit()
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
		Me.dgvLongPdList.Location = New System.Drawing.Point(4, 48)
		Me.dgvLongPdList.Name = "dgvLongPdList"
		Me.dgvLongPdList.ReadOnly = True
		Me.dgvLongPdList.RowTemplate.Height = 21
		Me.dgvLongPdList.Size = New System.Drawing.Size(534, 225)
		Me.dgvLongPdList.TabIndex = 5
		'
		'btnEnd
		'
		Me.btnEnd.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.btnEnd.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.btnEnd.Location = New System.Drawing.Point(434, 4)
		Me.btnEnd.Name = "btnEnd"
		Me.btnEnd.Size = New System.Drawing.Size(104, 39)
		Me.btnEnd.TabIndex = 17
		Me.btnEnd.Text = "F12:ï¬Ç∂ÇÈ"
		Me.btnEnd.UseVisualStyleBackColor = True
		'
		'cmbID
		'
		Me.cmbID.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
		Me.cmbID.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.cmbID.FormattingEnabled = True
		Me.cmbID.Location = New System.Drawing.Point(92, 11)
		Me.cmbID.Name = "cmbID"
		Me.cmbID.Size = New System.Drawing.Size(204, 24)
		Me.cmbID.TabIndex = 42
		'
		'lblCoilNo
		'
		Me.lblCoilNo.AutoSize = True
		Me.lblCoilNo.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.lblCoilNo.Location = New System.Drawing.Point(8, 15)
		Me.lblCoilNo.Name = "lblCoilNo"
		Me.lblCoilNo.Size = New System.Drawing.Size(77, 16)
		Me.lblCoilNo.TabIndex = 41
		Me.lblCoilNo.Text = "ÉRÉCÉãNo"
		'
		'btnCSV
		'
		Me.btnCSV.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
		Me.btnCSV.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.btnCSV.Location = New System.Drawing.Point(324, 4)
		Me.btnCSV.Name = "btnCSV"
		Me.btnCSV.Size = New System.Drawing.Size(104, 39)
		Me.btnCSV.TabIndex = 17
		Me.btnCSV.Text = "F9:CSVèoóÕ"
		Me.btnCSV.UseVisualStyleBackColor = True
		Me.btnCSV.Visible = False
		'
		'frmLongPdList
		'
		Me.ClientSize = New System.Drawing.Size(544, 281)
		Me.Controls.Add(Me.cmbID)
		Me.Controls.Add(Me.lblCoilNo)
		Me.Controls.Add(Me.dgvLongPdList)
		Me.Controls.Add(Me.btnCSV)
		Me.Controls.Add(Me.btnEnd)
		Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Sizable
		Me.MinimumSize = New System.Drawing.Size(560, 200)
		Me.Name = "frmLongPdList"
		Me.Text = "í∑é¸ä˙åáä◊àÍóóâÊñ "
		Me.TopMost = False
		Me.tProp_AltF4Enable = True
		CType(Me.dgvLongPdList, System.ComponentModel.ISupportInitialize).EndInit()
		Me.ResumeLayout(False)
		Me.PerformLayout()

	End Sub
	Friend WithEvents dgvLongPdList As System.Windows.Forms.DataGridView
	Friend WithEvents btnEnd As System.Windows.Forms.Button
	Friend WithEvents cmbID As ComboBox
	Friend WithEvents lblCoilNo As Label
	Friend WithEvents btnCSV As Button
End Class
