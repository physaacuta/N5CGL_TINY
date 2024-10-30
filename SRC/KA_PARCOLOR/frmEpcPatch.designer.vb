<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmEpcPatch
    Inherits frmMain_AutoParSet_UpList

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
        Me.grpBound = New System.Windows.Forms.GroupBox
        Me.dgvCamSetAngleScode = New tClassLibrary.tCnt.tCnt_DataGridView
        Me.lblSCode = New System.Windows.Forms.Label
        Me.cmbSCode = New System.Windows.Forms.ComboBox
        Me.Label6 = New System.Windows.Forms.Label
        Me.Label7 = New System.Windows.Forms.Label
        Me.camName = New KA_PARENG.CamName
        Me.pnlItem.SuspendLayout()
        Me.grpBase.SuspendLayout()
        Me.grpBound.SuspendLayout()
        CType(Me.dgvCamSetAngleScode, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'btnUpCodeItem_UpDate
        '
        '
        'grpBase
        '
        Me.grpBase.Controls.Add(Me.lblSCode)
        Me.grpBase.Controls.Add(Me.cmbSCode)
        Me.grpBase.Controls.Add(Me.Label6)
        Me.grpBase.Controls.Add(Me.Label7)
        Me.grpBase.Controls.Add(Me.grpBound)
        Me.grpBase.Text = "パッチ二値化"
        '
        'btnF02
        '
        Me.btnF02.Enabled = True
        Me.btnF02.Text = "次項"
        '
        'btnF01
        '
        Me.btnF01.Enabled = True
        Me.btnF01.Text = "前項"
        '
        'btnF12
        '
        Me.btnF12.Enabled = True
        Me.btnF12.Text = "終了"
        '
        'btnF11
        '
        Me.btnF11.Enabled = True
        Me.btnF11.Text = "設定"
        '
        'btnF09
        '
        Me.btnF09.Enabled = True
        Me.btnF09.Text = "切替"
        '
        'grpBound
        '
        Me.grpBound.Controls.Add(Me.dgvCamSetAngleScode)
        Me.grpBound.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.grpBound.ForeColor = System.Drawing.SystemColors.ControlText
        Me.grpBound.Location = New System.Drawing.Point(12, 92)
        Me.grpBound.Name = "grpBound"
        Me.grpBound.Size = New System.Drawing.Size(1052, 264)
        Me.grpBound.TabIndex = 0
        Me.grpBound.TabStop = False
        Me.grpBound.Text = "カメラ角度・カメラセット・検査表面状態別"
        '
        'dgvCamSetAngleScode
        '
        Me.dgvCamSetAngleScode.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvCamSetAngleScode.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvCamSetAngleScode.Location = New System.Drawing.Point(9, 23)
        Me.dgvCamSetAngleScode.Name = "dgvCamSetAngleScode"
        Me.dgvCamSetAngleScode.RowTemplate.Height = 21
        Me.dgvCamSetAngleScode.Size = New System.Drawing.Size(1035, 229)
        Me.dgvCamSetAngleScode.TabIndex = 1
        '
        'lblSCode
        '
        Me.lblSCode.BackColor = System.Drawing.Color.LightSteelBlue
        Me.lblSCode.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblSCode.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.lblSCode.ForeColor = System.Drawing.SystemColors.ControlText
        Me.lblSCode.Location = New System.Drawing.Point(184, 56)
        Me.lblSCode.Name = "lblSCode"
        Me.lblSCode.Size = New System.Drawing.Size(288, 24)
        Me.lblSCode.TabIndex = 38
        Me.lblSCode.Text = "12345678901234567890123456789012"
        Me.lblSCode.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'cmbSCode
        '
        Me.cmbSCode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbSCode.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.cmbSCode.FormattingEnabled = True
        Me.cmbSCode.Location = New System.Drawing.Point(12, 56)
        Me.cmbSCode.Name = "cmbSCode"
        Me.cmbSCode.Size = New System.Drawing.Size(156, 24)
        Me.cmbSCode.TabIndex = 37
        '
        'Label6
        '
        Me.Label6.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label6.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.Label6.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label6.Location = New System.Drawing.Point(184, 32)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(288, 24)
        Me.Label6.TabIndex = 36
        Me.Label6.Text = "検査表面状態名称"
        Me.Label6.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label7
        '
        Me.Label7.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label7.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.Label7.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label7.Location = New System.Drawing.Point(12, 32)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(156, 24)
        Me.Label7.TabIndex = 35
        Me.Label7.Text = "検査表面状態No"
        Me.Label7.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'camName
        '
        Me.camName.Location = New System.Drawing.Point(4, 412)
        Me.camName.Name = "camName"
        Me.camName.Size = New System.Drawing.Size(188, 224)
        Me.camName.TabIndex = 14
        '
        'frmEpcPatch
        '
        Me.ClientSize = New System.Drawing.Size(1272, 997)
        Me.Controls.Add(Me.camName)
        Me.Name = "frmEpcPatch"
        Me.Text = "パッチ二値化"
        Me.Controls.SetChildIndex(Me.lstVMenu, 0)
        Me.Controls.SetChildIndex(Me.grpBase, 0)
        Me.Controls.SetChildIndex(Me.pnlItem, 0)
        Me.Controls.SetChildIndex(Me.grpFunction, 0)
        Me.Controls.SetChildIndex(Me.camName, 0)
        Me.pnlItem.ResumeLayout(False)
        Me.grpBase.ResumeLayout(False)
        Me.grpBound.ResumeLayout(False)
        CType(Me.dgvCamSetAngleScode, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents grpBound As System.Windows.Forms.GroupBox
    Friend WithEvents dgvCamSetAngleScode As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents lblSCode As System.Windows.Forms.Label
    Friend WithEvents cmbSCode As System.Windows.Forms.ComboBox
    Friend WithEvents Label6 As System.Windows.Forms.Label
    Friend WithEvents Label7 As System.Windows.Forms.Label
    Friend WithEvents camName As KA_PARENG.CamName

End Class
