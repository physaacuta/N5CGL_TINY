<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPCodeJudge
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
        Me.dgvData = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.cmbPCode = New System.Windows.Forms.ComboBox()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.Label1 = New System.Windows.Forms.Label()
        Me.lblPCode = New System.Windows.Forms.Label()
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
        Me.grpBase.Controls.Add(Me.lblPCode)
        Me.grpBase.Controls.Add(Me.cmbPCode)
        Me.grpBase.Controls.Add(Me.Label2)
        Me.grpBase.Controls.Add(Me.Label1)
        Me.grpBase.Controls.Add(Me.dgvData)
        Me.grpBase.Text = "検査パターン条件設定"
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
        'dgvData
        '
        Me.dgvData.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvData.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvData.Location = New System.Drawing.Point(12, 75)
        Me.dgvData.Name = "dgvData"
        Me.dgvData.RowTemplate.Height = 21
        Me.dgvData.Size = New System.Drawing.Size(1048, 832)
        Me.dgvData.TabIndex = 0
        '
        'cmbPCode
        '
        Me.cmbPCode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbPCode.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.cmbPCode.FormattingEnabled = True
        Me.cmbPCode.Location = New System.Drawing.Point(12, 48)
        Me.cmbPCode.MaxDropDownItems = 16
        Me.cmbPCode.Name = "cmbPCode"
        Me.cmbPCode.Size = New System.Drawing.Size(172, 24)
        Me.cmbPCode.TabIndex = 6
        '
        'Label2
        '
        Me.Label2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label2.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label2.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label2.Location = New System.Drawing.Point(212, 24)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(300, 24)
        Me.Label2.TabIndex = 5
        Me.Label2.Text = "検査パターン名称"
        Me.Label2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label1
        '
        Me.Label1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label1.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label1.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label1.Location = New System.Drawing.Point(12, 24)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(172, 24)
        Me.Label1.TabIndex = 4
        Me.Label1.Text = "検査パターンコード"
        Me.Label1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'lblPCode
        '
        Me.lblPCode.BackColor = System.Drawing.Color.LightSteelBlue
        Me.lblPCode.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.lblPCode.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.lblPCode.ForeColor = System.Drawing.SystemColors.WindowText
        Me.lblPCode.Location = New System.Drawing.Point(212, 48)
        Me.lblPCode.Name = "lblPCode"
        Me.lblPCode.Size = New System.Drawing.Size(300, 24)
        Me.lblPCode.TabIndex = 8
        Me.lblPCode.Text = "12345679801234567890123456789012"
        Me.lblPCode.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'frmPCodeJudge
        '
        Me.ClientSize = New System.Drawing.Size(1272, 997)
        Me.Name = "frmPCodeJudge"
        Me.Text = "検査パターン条件設定画面"
        Me.pnlItem.ResumeLayout(False)
        Me.grpBase.ResumeLayout(False)
        CType(Me.dgvData, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents dgvData As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents cmbPCode As System.Windows.Forms.ComboBox
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents lblPCode As System.Windows.Forms.Label

End Class
