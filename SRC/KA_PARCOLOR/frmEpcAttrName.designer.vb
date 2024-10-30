<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmEpcAttrName
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
        Me.Label9 = New System.Windows.Forms.Label()
        Me.cmbAttrName = New System.Windows.Forms.ComboBox()
        Me.Label10 = New System.Windows.Forms.Label()
        Me.dgvAttrName = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.grpBase.SuspendLayout()
        CType(Me.dgvAttrName, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'grpBase
        '
        Me.grpBase.Controls.Add(Me.Label9)
        Me.grpBase.Controls.Add(Me.cmbAttrName)
        Me.grpBase.Controls.Add(Me.Label10)
        Me.grpBase.Controls.Add(Me.dgvAttrName)
        Me.grpBase.Text = "特徴量名称マスター設定"
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
        'btnF08
        '
        Me.btnF08.Enabled = True
        Me.btnF08.Text = "画面保存"
        '
        'btnF07
        '
        Me.btnF07.Enabled = True
        Me.btnF07.Text = "画面印刷"
        '
        'Label9
        '
        Me.Label9.AutoSize = True
        Me.Label9.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label9.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label9.Location = New System.Drawing.Point(800, 868)
        Me.Label9.Name = "Label9"
        Me.Label9.Size = New System.Drawing.Size(184, 32)
        Me.Label9.TabIndex = 70
        Me.Label9.Text = "※不使用時はEdasysIDに" & Global.Microsoft.VisualBasic.ChrW(13) & Global.Microsoft.VisualBasic.ChrW(10) & "0を入力して下さい。"
        '
        'cmbAttrName
        '
        Me.cmbAttrName.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbAttrName.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.cmbAttrName.FormattingEnabled = True
        Me.cmbAttrName.Location = New System.Drawing.Point(12, 52)
        Me.cmbAttrName.Name = "cmbAttrName"
        Me.cmbAttrName.Size = New System.Drawing.Size(156, 24)
        Me.cmbAttrName.TabIndex = 69
        '
        'Label10
        '
        Me.Label10.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label10.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.Label10.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label10.Location = New System.Drawing.Point(12, 28)
        Me.Label10.Name = "Label10"
        Me.Label10.Size = New System.Drawing.Size(156, 24)
        Me.Label10.TabIndex = 68
        Me.Label10.Text = "特徴量テーブル"
        Me.Label10.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'dgvAttrName
        '
        Me.dgvAttrName.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvAttrName.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvAttrName.Location = New System.Drawing.Point(12, 84)
        Me.dgvAttrName.Name = "dgvAttrName"
        Me.dgvAttrName.RowTemplate.Height = 21
        Me.dgvAttrName.Size = New System.Drawing.Size(780, 820)
        Me.dgvAttrName.TabIndex = 67
        '
        'frmEpcAttrName
        '
        Me.ClientSize = New System.Drawing.Size(1272, 997)
        Me.MaximizeBox = False
        Me.Name = "frmEpcAttrName"
        Me.Text = "特徴量名称マスター設定画面"
        Me.grpBase.ResumeLayout(False)
        Me.grpBase.PerformLayout()
        CType(Me.dgvAttrName, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents Label9 As System.Windows.Forms.Label
    Friend WithEvents cmbAttrName As System.Windows.Forms.ComboBox
    Friend WithEvents Label10 As System.Windows.Forms.Label
	Friend WithEvents dgvAttrName As tClassLibrary.tCnt.tCnt_DataGridView

End Class
