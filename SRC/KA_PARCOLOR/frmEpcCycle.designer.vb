<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmEpcCycle
    Inherits frmMain_ParSet_SysName

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
        Me.dgvTypeMscData = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.dgvPeriodData = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.dgvTypeMscBtnAll = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.dgvTypeMscBtnClear = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.grpBase.SuspendLayout()
        CType(Me.dgvTypeMscData, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvPeriodData, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvTypeMscBtnAll, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvTypeMscBtnClear, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'grpBase
        '
        Me.grpBase.Controls.Add(Me.dgvTypeMscBtnClear)
        Me.grpBase.Controls.Add(Me.dgvTypeMscBtnAll)
        Me.grpBase.Controls.Add(Me.dgvPeriodData)
        Me.grpBase.Controls.Add(Me.dgvTypeMscData)
        Me.grpBase.Text = "é¸ä˙·rê›íË"
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
        'dgvTypeMscData
        '
        Me.dgvTypeMscData.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvTypeMscData.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvTypeMscData.Location = New System.Drawing.Point(12, 28)
        Me.dgvTypeMscData.Name = "dgvTypeMscData"
        Me.dgvTypeMscData.RowTemplate.Height = 21
        Me.dgvTypeMscData.Size = New System.Drawing.Size(1008, 483)
        Me.dgvTypeMscData.TabIndex = 1
        '
        'dgvPeriodData
        '
        Me.dgvPeriodData.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvPeriodData.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvPeriodData.Location = New System.Drawing.Point(12, 564)
        Me.dgvPeriodData.Name = "dgvPeriodData"
        Me.dgvPeriodData.RowTemplate.Height = 21
        Me.dgvPeriodData.Size = New System.Drawing.Size(991, 346)
        Me.dgvPeriodData.TabIndex = 2
        '
        'dgvTypeMscBtnAll
        '
        Me.dgvTypeMscBtnAll.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvTypeMscBtnAll.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvTypeMscBtnAll.Location = New System.Drawing.Point(202, 515)
        Me.dgvTypeMscBtnAll.Name = "dgvTypeMscBtnAll"
        Me.dgvTypeMscBtnAll.RowTemplate.Height = 21
        Me.dgvTypeMscBtnAll.Size = New System.Drawing.Size(801, 21)
        Me.dgvTypeMscBtnAll.TabIndex = 3
        '
        'dgvTypeMscBtnClear
        '
        Me.dgvTypeMscBtnClear.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvTypeMscBtnClear.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvTypeMscBtnClear.Location = New System.Drawing.Point(202, 540)
        Me.dgvTypeMscBtnClear.Name = "dgvTypeMscBtnClear"
        Me.dgvTypeMscBtnClear.RowTemplate.Height = 21
        Me.dgvTypeMscBtnClear.Size = New System.Drawing.Size(801, 21)
        Me.dgvTypeMscBtnClear.TabIndex = 4
        '
        'frmEpcCycle
        '
        Me.ClientSize = New System.Drawing.Size(1272, 997)
        Me.MaximizeBox = False
        Me.Name = "frmEpcCycle"
        Me.Text = "é¸ä˙·rê›íËâÊñ "
        Me.grpBase.ResumeLayout(False)
        CType(Me.dgvTypeMscData, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvPeriodData, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvTypeMscBtnAll, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvTypeMscBtnClear, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents dgvTypeMscBtnAll As tClassLibrary.tCnt.tCnt_DataGridView
	Friend WithEvents dgvPeriodData As tClassLibrary.tCnt.tCnt_DataGridView
	Friend WithEvents dgvTypeMscData As tClassLibrary.tCnt.tCnt_DataGridView
	Friend WithEvents dgvTypeMscBtnClear As tClassLibrary.tCnt.tCnt_DataGridView

End Class
