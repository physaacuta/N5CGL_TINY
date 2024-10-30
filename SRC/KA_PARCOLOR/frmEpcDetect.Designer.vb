<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmEpcDetect
    Inherits frmMain_AutoParSet_UpList_SysName

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
        Me.Label6 = New System.Windows.Forms.Label()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.GroupBox6 = New System.Windows.Forms.GroupBox()
        Me.dgvPScode = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.ofdDcdFile = New System.Windows.Forms.OpenFileDialog()
        Me.GroupBox3 = New System.Windows.Forms.GroupBox()
        Me.dgvCamAngleScode = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.camName = New KA_PARCOLOR.CamName()
        Me.pnlItem.SuspendLayout()
        Me.grpBase.SuspendLayout()
        Me.GroupBox6.SuspendLayout()
        CType(Me.dgvPScode, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.GroupBox3.SuspendLayout()
        CType(Me.dgvCamAngleScode, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'pnlItem
        '
        Me.pnlItem.TabIndex = 2
        '
        'cmbUpCodeItem
        '
        Me.cmbUpCodeItem.TabIndex = 0
        '
        'btnUpCodeItem_UpDate
        '
        Me.btnUpCodeItem_UpDate.TabIndex = 45
        '
        'lblUpCodeItem
        '
        Me.lblUpCodeItem.TabIndex = 1
        '
        'lblTitolCopySend
        '
        Me.lblTitolCopySend.TabIndex = 1
        '
        'lblTitolCopyTo
        '
        Me.lblTitolCopyTo.TabIndex = 1
        '
        'btnUpCodeItem_Clrea
        '
        Me.btnUpCodeItem_Clrea.TabIndex = 44
        '
        'btnUpCodeItem_All
        '
        Me.btnUpCodeItem_All.TabIndex = 43
        '
        'grpBase
        '
        Me.grpBase.Controls.Add(Me.GroupBox3)
        Me.grpBase.Controls.Add(Me.GroupBox6)
        Me.grpBase.Controls.Add(Me.lblSCode)
        Me.grpBase.Controls.Add(Me.cmbSCode)
        Me.grpBase.Controls.Add(Me.Label6)
        Me.grpBase.Controls.Add(Me.Label7)
        Me.grpBase.TabIndex = 1
        Me.grpBase.Text = "·råüèoê›íË"
        '
        'lstVMenu
        '
        Me.lstVMenu.TabIndex = 0
        '
        'grpFunction
        '
        Me.grpFunction.TabIndex = 3
        '
        'btnF02
        '
        Me.btnF02.Enabled = True
        Me.btnF02.TabIndex = 40
        Me.btnF02.Text = "éüï≈"
        '
        'btnF01
        '
        Me.btnF01.Enabled = True
        Me.btnF01.TabIndex = 39
        Me.btnF01.Text = "ëOï≈"
        '
        'btnF12
        '
        Me.btnF12.Enabled = True
        Me.btnF12.Text = "èIóπ"
        '
        'btnF11
        '
        Me.btnF11.Enabled = True
        Me.btnF11.TabIndex = 41
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
        Me.lblSCode.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblSCode.ForeColor = System.Drawing.SystemColors.ControlText
        Me.lblSCode.Location = New System.Drawing.Point(172, 52)
        Me.lblSCode.Name = "lblSCode"
        Me.lblSCode.Size = New System.Drawing.Size(300, 24)
        Me.lblSCode.TabIndex = 44
        Me.lblSCode.Text = "12345678901234567890123456789012"
        Me.lblSCode.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'cmbSCode
        '
        Me.cmbSCode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbSCode.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!)
        Me.cmbSCode.FormattingEnabled = True
        Me.cmbSCode.Location = New System.Drawing.Point(12, 52)
        Me.cmbSCode.Name = "cmbSCode"
        Me.cmbSCode.Size = New System.Drawing.Size(156, 24)
        Me.cmbSCode.TabIndex = 0
        '
        'Label6
        '
        Me.Label6.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label6.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!)
        Me.Label6.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label6.Location = New System.Drawing.Point(172, 28)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(300, 24)
        Me.Label6.TabIndex = 42
        Me.Label6.Text = "åüç∏ï\ñ èÛë‘ñºèÃ"
        Me.Label6.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label7
        '
        Me.Label7.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label7.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!)
        Me.Label7.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label7.Location = New System.Drawing.Point(12, 28)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(154, 24)
        Me.Label7.TabIndex = 41
        Me.Label7.Text = "åüç∏ï\ñ èÛë‘No"
        Me.Label7.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'GroupBox6
        '
        Me.GroupBox6.Controls.Add(Me.dgvPScode)
        Me.GroupBox6.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox6.ForeColor = System.Drawing.SystemColors.ControlText
        Me.GroupBox6.Location = New System.Drawing.Point(12, 84)
        Me.GroupBox6.Name = "GroupBox6"
        Me.GroupBox6.Size = New System.Drawing.Size(1048, 624)
        Me.GroupBox6.TabIndex = 1
        Me.GroupBox6.TabStop = False
        Me.GroupBox6.Text = "åüç∏ï\ñ èÛë‘ï "
        '
        'dgvPScode
        '
        Me.dgvPScode.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvPScode.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvPScode.Location = New System.Drawing.Point(8, 24)
        Me.dgvPScode.Name = "dgvPScode"
        Me.dgvPScode.RowTemplate.Height = 21
        Me.dgvPScode.Size = New System.Drawing.Size(1032, 592)
        Me.dgvPScode.TabIndex = 1
        '
        'ofdDcdFile
        '
        Me.ofdDcdFile.FileName = "OpenFileDialog1"
        '
        'GroupBox3
        '
        Me.GroupBox3.Controls.Add(Me.dgvCamAngleScode)
        Me.GroupBox3.Font = New System.Drawing.Font("ÇlÇr ñæí©", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.GroupBox3.ForeColor = System.Drawing.SystemColors.ControlText
        Me.GroupBox3.Location = New System.Drawing.Point(12, 720)
        Me.GroupBox3.Name = "GroupBox3"
        Me.GroupBox3.Size = New System.Drawing.Size(1048, 176)
        Me.GroupBox3.TabIndex = 45
        Me.GroupBox3.TabStop = False
        Me.GroupBox3.Text = "ÉJÉÅÉâäpìxÅEåüç∏ï\ñ èÛë‘ï "
        '
        'dgvCamAngleScode
        '
        Me.dgvCamAngleScode.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvCamAngleScode.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvCamAngleScode.Location = New System.Drawing.Point(8, 24)
        Me.dgvCamAngleScode.Name = "dgvCamAngleScode"
        Me.dgvCamAngleScode.RowTemplate.Height = 21
        Me.dgvCamAngleScode.Size = New System.Drawing.Size(1032, 144)
        Me.dgvCamAngleScode.TabIndex = 0
        '
        'camName
        '
        Me.camName.Location = New System.Drawing.Point(4, 412)
        Me.camName.Name = "camName"
        Me.camName.Size = New System.Drawing.Size(188, 224)
        Me.camName.TabIndex = 13
        '
        'frmEpcDetect
        '
        Me.ClientSize = New System.Drawing.Size(1272, 997)
        Me.Controls.Add(Me.camName)
        Me.Name = "frmEpcDetect"
        Me.Text = "·råüèoê›íËâÊñ "
        Me.Controls.SetChildIndex(Me.lstVMenu, 0)
        Me.Controls.SetChildIndex(Me.grpBase, 0)
        Me.Controls.SetChildIndex(Me.pnlItem, 0)
        Me.Controls.SetChildIndex(Me.grpFunction, 0)
        Me.Controls.SetChildIndex(Me.camName, 0)
        Me.pnlItem.ResumeLayout(False)
        Me.grpBase.ResumeLayout(False)
        Me.GroupBox6.ResumeLayout(False)
        CType(Me.dgvPScode, System.ComponentModel.ISupportInitialize).EndInit()
        Me.GroupBox3.ResumeLayout(False)
        CType(Me.dgvCamAngleScode, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents lblSCode As System.Windows.Forms.Label
    Friend WithEvents cmbSCode As System.Windows.Forms.ComboBox
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents GroupBox6 As System.Windows.Forms.GroupBox
    Friend WithEvents ofdDcdFile As System.Windows.Forms.OpenFileDialog
    Friend WithEvents GroupBox3 As System.Windows.Forms.GroupBox
    Friend WithEvents dgvCamAngleScode As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents dgvPScode As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents camName As KA_PARCOLOR.CamName

End Class
