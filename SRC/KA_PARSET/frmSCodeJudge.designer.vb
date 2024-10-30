<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmSCodeJudge
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
        Me.lblSCode = New System.Windows.Forms.Label()
        Me.cmbSCode = New System.Windows.Forms.ComboBox()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.dgvData = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.pnlItem.SuspendLayout()
        Me.grpBase.SuspendLayout()
        CType(Me.dgvData, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'btnUpCodeItem_UpDate
        '
        '
        'grpBase
        '
        Me.grpBase.Controls.Add(Me.lblSCode)
        Me.grpBase.Controls.Add(Me.cmbSCode)
        Me.grpBase.Controls.Add(Me.Label2)
        Me.grpBase.Controls.Add(Me.Label1)
        Me.grpBase.Controls.Add(Me.dgvData)
        Me.grpBase.Text = "åüç∏ï\ñ èÛë‘èåèê›íË"
        '
        'btnF02
        '
        Me.btnF02.Enabled = True
        Me.btnF02.Text = "éüçÄ"
        '
        'btnF01
        '
        Me.btnF01.Enabled = True
        Me.btnF01.Text = "ëOçÄ"
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
        'lblSCode
        '
        Me.lblSCode.BackColor = System.Drawing.Color.LightSteelBlue
        Me.lblSCode.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblSCode.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!)
        Me.lblSCode.ForeColor = System.Drawing.SystemColors.WindowText
        Me.lblSCode.Location = New System.Drawing.Point(184, 48)
        Me.lblSCode.Name = "lblSCode"
        Me.lblSCode.Size = New System.Drawing.Size(300, 24)
        Me.lblSCode.TabIndex = 13
        Me.lblSCode.Text = "12345679801234567890123456789012"
        Me.lblSCode.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'cmbSCode
        '
        Me.cmbSCode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbSCode.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbSCode.FormattingEnabled = True
        Me.cmbSCode.Location = New System.Drawing.Point(12, 48)
        Me.cmbSCode.MaxDropDownItems = 16
        Me.cmbSCode.Name = "cmbSCode"
        Me.cmbSCode.Size = New System.Drawing.Size(156, 24)
        Me.cmbSCode.TabIndex = 12
        '
        'Label2
        '
        Me.Label2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label2.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label2.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label2.Location = New System.Drawing.Point(184, 24)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(300, 24)
        Me.Label2.TabIndex = 11
        Me.Label2.Text = "åüç∏ï\ñ èÛë‘ñºèÃ"
        Me.Label2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label1
        '
        Me.Label1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label1.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label1.Location = New System.Drawing.Point(12, 24)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(154, 24)
        Me.Label1.TabIndex = 10
        Me.Label1.Text = "åüç∏ï\ñ èÛë‘No"
        Me.Label1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'dgvData
        '
        Me.dgvData.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvData.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvData.Location = New System.Drawing.Point(12, 80)
        Me.dgvData.Name = "dgvData"
        Me.dgvData.RowTemplate.Height = 21
        Me.dgvData.Size = New System.Drawing.Size(1048, 832)
        Me.dgvData.TabIndex = 9
        '
        'frmSCodeJudge
        '
        Me.ClientSize = New System.Drawing.Size(1272, 997)
        Me.Name = "frmSCodeJudge"
        Me.Text = "åüç∏ï\ñ èÛë‘èåèê›íËâÊñ "
        Me.pnlItem.ResumeLayout(False)
        Me.grpBase.ResumeLayout(False)
        CType(Me.dgvData, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents lblSCode As System.Windows.Forms.Label
    Friend WithEvents cmbSCode As System.Windows.Forms.ComboBox
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents Label1 As System.Windows.Forms.Label
	Friend WithEvents dgvData As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents dgvCommon As tClassLibrary.tCnt.tCnt_DataGridView

End Class
