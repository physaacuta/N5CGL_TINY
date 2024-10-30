<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmLongPd
	Inherits frmMain_ParSet_UplList_SysName

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
		Me.dgvOutput = New tClassLibrary.tCnt.tCnt_DataGridView()
		Me.dgvFilter = New tClassLibrary.tCnt.tCnt_DataGridView()
		Me.dgvOutputBtnClear = New tClassLibrary.tCnt.tCnt_DataGridView()
		Me.GroupBox1 = New System.Windows.Forms.GroupBox()
		Me.dgvOutputEtc = New tClassLibrary.tCnt.tCnt_DataGridView()
		Me.lblPCode = New System.Windows.Forms.Label()
		Me.cmbEnable = New System.Windows.Forms.ComboBox()
		Me.cmbPCode = New System.Windows.Forms.ComboBox()
		Me.Label6 = New System.Windows.Forms.Label()
		Me.Label1 = New System.Windows.Forms.Label()
		Me.Label7 = New System.Windows.Forms.Label()
		Me.GroupBox6 = New System.Windows.Forms.GroupBox()
		Me.dgvCommon = New tClassLibrary.tCnt.tCnt_DataGridView()
		Me.pnlItem.SuspendLayout()
		Me.grpBase.SuspendLayout()
		CType(Me.dgvOutput, System.ComponentModel.ISupportInitialize).BeginInit()
		CType(Me.dgvFilter, System.ComponentModel.ISupportInitialize).BeginInit()
		CType(Me.dgvOutputBtnClear, System.ComponentModel.ISupportInitialize).BeginInit()
		Me.GroupBox1.SuspendLayout()
		CType(Me.dgvOutputEtc, System.ComponentModel.ISupportInitialize).BeginInit()
		Me.GroupBox6.SuspendLayout()
		CType(Me.dgvCommon, System.ComponentModel.ISupportInitialize).BeginInit()
		Me.SuspendLayout()
		'
		'grpBase
		'
		Me.grpBase.Controls.Add(Me.GroupBox6)
		Me.grpBase.Controls.Add(Me.GroupBox1)
		Me.grpBase.Text = "í∑é¸ä˙åáä◊åüèoê›íË"
		'
		'btnF12
		'
		Me.btnF12.Enabled = True
		Me.btnF12.Text = "èIóπ"
		'
		'btnF11
		'
		Me.btnF11.Enabled = True
		Me.btnF11.Text = "ê›íË"
		'
		'btnF09
		'
		Me.btnF09.Enabled = True
		Me.btnF09.Text = "êÿë÷"
		'
		'btnF08
		'
		Me.btnF08.Enabled = True
		Me.btnF08.Text = "âÊñ ï€ë∂"
		'
		'btnF07
		'
		Me.btnF07.Enabled = True
		Me.btnF07.Text = "âÊñ àÛç¸"
		'
		'dgvOutput
		'
		Me.dgvOutput.BorderStyle = System.Windows.Forms.BorderStyle.None
		Me.dgvOutput.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
		Me.dgvOutput.Location = New System.Drawing.Point(8, 80)
		Me.dgvOutput.Name = "dgvOutput"
		Me.dgvOutput.RowTemplate.Height = 21
		Me.dgvOutput.Size = New System.Drawing.Size(1008, 300)
		Me.dgvOutput.TabIndex = 1
		'
		'dgvFilter
		'
		Me.dgvFilter.BorderStyle = System.Windows.Forms.BorderStyle.None
		Me.dgvFilter.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
		Me.dgvFilter.Location = New System.Drawing.Point(8, 434)
		Me.dgvFilter.Name = "dgvFilter"
		Me.dgvFilter.RowTemplate.Height = 21
		Me.dgvFilter.Size = New System.Drawing.Size(1008, 200)
		Me.dgvFilter.TabIndex = 2
		'
		'dgvOutputBtnClear
		'
		Me.dgvOutputBtnClear.BorderStyle = System.Windows.Forms.BorderStyle.None
		Me.dgvOutputBtnClear.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
		Me.dgvOutputBtnClear.Location = New System.Drawing.Point(198, 409)
		Me.dgvOutputBtnClear.Name = "dgvOutputBtnClear"
		Me.dgvOutputBtnClear.RowTemplate.Height = 21
		Me.dgvOutputBtnClear.Size = New System.Drawing.Size(801, 21)
		Me.dgvOutputBtnClear.TabIndex = 4
		'
		'GroupBox1
		'
		Me.GroupBox1.Controls.Add(Me.dgvOutputEtc)
		Me.GroupBox1.Controls.Add(Me.lblPCode)
		Me.GroupBox1.Controls.Add(Me.dgvOutput)
		Me.GroupBox1.Controls.Add(Me.cmbEnable)
		Me.GroupBox1.Controls.Add(Me.cmbPCode)
		Me.GroupBox1.Controls.Add(Me.dgvOutputBtnClear)
		Me.GroupBox1.Controls.Add(Me.Label6)
		Me.GroupBox1.Controls.Add(Me.dgvFilter)
		Me.GroupBox1.Controls.Add(Me.Label1)
		Me.GroupBox1.Controls.Add(Me.Label7)
		Me.GroupBox1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.GroupBox1.ForeColor = System.Drawing.SystemColors.ControlText
		Me.GroupBox1.Location = New System.Drawing.Point(8, 268)
		Me.GroupBox1.Name = "GroupBox1"
		Me.GroupBox1.Size = New System.Drawing.Size(1048, 648)
		Me.GroupBox1.TabIndex = 42
		Me.GroupBox1.TabStop = False
		Me.GroupBox1.Text = "åüç∏ÉpÉ^Å[Éìï "
		'
		'dgvOutputEtc
		'
		Me.dgvOutputEtc.BorderStyle = System.Windows.Forms.BorderStyle.None
		Me.dgvOutputEtc.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
		Me.dgvOutputEtc.Location = New System.Drawing.Point(9, 384)
		Me.dgvOutputEtc.Name = "dgvOutputEtc"
		Me.dgvOutputEtc.RowTemplate.Height = 21
		Me.dgvOutputEtc.Size = New System.Drawing.Size(990, 20)
		Me.dgvOutputEtc.TabIndex = 49
		'
		'lblPCode
		'
		Me.lblPCode.BackColor = System.Drawing.Color.LightSteelBlue
		Me.lblPCode.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
		Me.lblPCode.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.lblPCode.ForeColor = System.Drawing.SystemColors.ControlText
		Me.lblPCode.Location = New System.Drawing.Point(188, 48)
		Me.lblPCode.Name = "lblPCode"
		Me.lblPCode.Size = New System.Drawing.Size(300, 24)
		Me.lblPCode.TabIndex = 48
		Me.lblPCode.Text = "12345678901234567890123456789012"
		Me.lblPCode.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
		'
		'cmbEnable
		'
		Me.cmbEnable.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
		Me.cmbEnable.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!)
		Me.cmbEnable.FormattingEnabled = True
		Me.cmbEnable.Location = New System.Drawing.Point(504, 48)
		Me.cmbEnable.Name = "cmbEnable"
		Me.cmbEnable.Size = New System.Drawing.Size(156, 24)
		Me.cmbEnable.TabIndex = 45
		'
		'cmbPCode
		'
		Me.cmbPCode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
		Me.cmbPCode.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!)
		Me.cmbPCode.FormattingEnabled = True
		Me.cmbPCode.Location = New System.Drawing.Point(8, 48)
		Me.cmbPCode.Name = "cmbPCode"
		Me.cmbPCode.Size = New System.Drawing.Size(176, 24)
		Me.cmbPCode.TabIndex = 45
		'
		'Label6
		'
		Me.Label6.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
		Me.Label6.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!)
		Me.Label6.ForeColor = System.Drawing.SystemColors.ControlText
		Me.Label6.Location = New System.Drawing.Point(188, 24)
		Me.Label6.Name = "Label6"
		Me.Label6.Size = New System.Drawing.Size(300, 24)
		Me.Label6.TabIndex = 47
		Me.Label6.Text = "åüç∏ÉpÉ^Å[ÉìñºèÃ"
		Me.Label6.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
		'
		'Label1
		'
		Me.Label1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
		Me.Label1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!)
		Me.Label1.ForeColor = System.Drawing.SystemColors.ControlText
		Me.Label1.Location = New System.Drawing.Point(504, 24)
		Me.Label1.Name = "Label1"
		Me.Label1.Size = New System.Drawing.Size(154, 24)
		Me.Label1.TabIndex = 46
		Me.Label1.Text = "é¸ä˙îªíËèàóù"
		Me.Label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter
		'
		'Label7
		'
		Me.Label7.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
		Me.Label7.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!)
		Me.Label7.ForeColor = System.Drawing.SystemColors.ControlText
		Me.Label7.Location = New System.Drawing.Point(8, 24)
		Me.Label7.Name = "Label7"
		Me.Label7.Size = New System.Drawing.Size(174, 24)
		Me.Label7.TabIndex = 46
		Me.Label7.Text = "åüç∏ÉpÉ^Å[ÉìÉRÅ[Éh"
		Me.Label7.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
		'
		'GroupBox6
		'
		Me.GroupBox6.Controls.Add(Me.dgvCommon)
		Me.GroupBox6.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
		Me.GroupBox6.ForeColor = System.Drawing.SystemColors.ControlText
		Me.GroupBox6.Location = New System.Drawing.Point(8, 24)
		Me.GroupBox6.Name = "GroupBox6"
		Me.GroupBox6.Size = New System.Drawing.Size(1048, 240)
		Me.GroupBox6.TabIndex = 43
		Me.GroupBox6.TabStop = False
		Me.GroupBox6.Text = "ã§í "
		'
		'dgvCommon
		'
		Me.dgvCommon.BorderStyle = System.Windows.Forms.BorderStyle.None
		Me.dgvCommon.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
		Me.dgvCommon.Location = New System.Drawing.Point(8, 24)
		Me.dgvCommon.Name = "dgvCommon"
		Me.dgvCommon.RowTemplate.Height = 21
		Me.dgvCommon.Size = New System.Drawing.Size(1008, 210)
		Me.dgvCommon.TabIndex = 1
		'
		'frmLongPd
		'
		Me.ClientSize = New System.Drawing.Size(1272, 997)
		Me.Name = "frmLongPd"
		Me.Text = "í∑é¸ä˙åáä◊åüèoê›íËâÊñ "
		Me.pnlItem.ResumeLayout(False)
		Me.grpBase.ResumeLayout(False)
		CType(Me.dgvOutput, System.ComponentModel.ISupportInitialize).EndInit()
		CType(Me.dgvFilter, System.ComponentModel.ISupportInitialize).EndInit()
		CType(Me.dgvOutputBtnClear, System.ComponentModel.ISupportInitialize).EndInit()
		Me.GroupBox1.ResumeLayout(False)
		CType(Me.dgvOutputEtc, System.ComponentModel.ISupportInitialize).EndInit()
		Me.GroupBox6.ResumeLayout(False)
		CType(Me.dgvCommon, System.ComponentModel.ISupportInitialize).EndInit()
		Me.ResumeLayout(False)

	End Sub
	Friend WithEvents dgvFilter As tClassLibrary.tCnt.tCnt_DataGridView
	Friend WithEvents dgvOutput As tClassLibrary.tCnt.tCnt_DataGridView
	Friend WithEvents dgvOutputBtnClear As tClassLibrary.tCnt.tCnt_DataGridView
	Friend WithEvents GroupBox1 As GroupBox
	Friend WithEvents GroupBox6 As GroupBox
	Friend WithEvents dgvCommon As tClassLibrary.tCnt.tCnt_DataGridView
	Friend WithEvents lblPCode As Label
	Friend WithEvents cmbPCode As ComboBox
	Friend WithEvents Label6 As Label
	Friend WithEvents Label7 As Label
	Friend WithEvents cmbEnable As ComboBox
	Friend WithEvents Label1 As Label
	Friend WithEvents dgvOutputEtc As tClassLibrary.tCnt.tCnt_DataGridView
End Class
