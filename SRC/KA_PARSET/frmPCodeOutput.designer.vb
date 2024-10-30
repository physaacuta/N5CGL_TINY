<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class frmPCodeOutput
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
        Me.dgvEtc = New tClassLibrary.tCnt.tCnt_DataGridView()
        Me.Label4 = New System.Windows.Forms.Label()
        Me.Label6 = New System.Windows.Forms.Label()
        Me.Label3 = New System.Windows.Forms.Label()
        Me.Label5 = New System.Windows.Forms.Label()
        Me.Label7 = New System.Windows.Forms.Label()
        Me.pnlItem.SuspendLayout()
        Me.grpBase.SuspendLayout()
        CType(Me.dgvData, System.ComponentModel.ISupportInitialize).BeginInit()
        CType(Me.dgvEtc, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'btnUpCodeItem_UpDate
        '
        '
        'grpBase
        '
        Me.grpBase.Controls.Add(Me.Label7)
        Me.grpBase.Controls.Add(Me.Label5)
        Me.grpBase.Controls.Add(Me.Label6)
        Me.grpBase.Controls.Add(Me.Label4)
        Me.grpBase.Controls.Add(Me.dgvEtc)
        Me.grpBase.Controls.Add(Me.lblPCode)
        Me.grpBase.Controls.Add(Me.Label3)
        Me.grpBase.Controls.Add(Me.dgvData)
        Me.grpBase.Controls.Add(Me.cmbPCode)
        Me.grpBase.Controls.Add(Me.Label2)
        Me.grpBase.Controls.Add(Me.Label1)
        Me.grpBase.Text = "検査パターン別出力設定"
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
        Me.dgvData.Location = New System.Drawing.Point(12, 76)
        Me.dgvData.Name = "dgvData"
        Me.dgvData.RowTemplate.Height = 21
        Me.dgvData.Size = New System.Drawing.Size(1048, 779)
        Me.dgvData.TabIndex = 33
        '
        'cmbPCode
        '
        Me.cmbPCode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.cmbPCode.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.cmbPCode.FormattingEnabled = True
        Me.cmbPCode.Location = New System.Drawing.Point(12, 48)
        Me.cmbPCode.Name = "cmbPCode"
        Me.cmbPCode.Size = New System.Drawing.Size(172, 24)
        Me.cmbPCode.TabIndex = 31
        '
        'Label2
        '
        Me.Label2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label2.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.Label2.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label2.Location = New System.Drawing.Point(212, 24)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(279, 24)
        Me.Label2.TabIndex = 30
        Me.Label2.Text = "検査パターン名称"
        Me.Label2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'Label1
        '
        Me.Label1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label1.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!)
        Me.Label1.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label1.Location = New System.Drawing.Point(12, 24)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(172, 24)
        Me.Label1.TabIndex = 29
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
        Me.lblPCode.Size = New System.Drawing.Size(280, 24)
        Me.lblPCode.TabIndex = 35
        Me.lblPCode.Text = "123456789"
        Me.lblPCode.TextAlign = System.Drawing.ContentAlignment.MiddleLeft
        '
        'dgvEtc
        '
        Me.dgvEtc.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.dgvEtc.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize
        Me.dgvEtc.Location = New System.Drawing.Point(12, 858)
        Me.dgvEtc.Name = "dgvEtc"
        Me.dgvEtc.RowTemplate.Height = 21
        Me.dgvEtc.Size = New System.Drawing.Size(1031, 20)
        Me.dgvEtc.TabIndex = 36
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label4.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label4.Location = New System.Drawing.Point(508, 30)
        Me.Label4.Name = "Label4"
        Me.Label4.Size = New System.Drawing.Size(256, 16)
        Me.Label4.TabIndex = 37
        Me.Label4.Text = "※'―’は、出力無しの設定です。"
        '
        'Label6
        '
        Me.Label6.AutoSize = True
        Me.Label6.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label6.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label6.Location = New System.Drawing.Point(12, 878)
        Me.Label6.Name = "Label6"
        Me.Label6.Size = New System.Drawing.Size(552, 16)
        Me.Label6.TabIndex = 40
        Me.Label6.Text = "※[DB保存]の「一般」のみを指定する場合、全て「一般」扱いとなります。"
        '
        'Label3
        '
        Me.Label3.AutoSize = True
        Me.Label3.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label3.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label3.Location = New System.Drawing.Point(508, 50)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(456, 16)
        Me.Label3.TabIndex = 34
        Me.Label3.Text = "※疵種が「無し」の行は、設定ボタン押下後に削除されます。"
        '
        'Label5
        '
        Me.Label5.AutoSize = True
        Me.Label5.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label5.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label5.Location = New System.Drawing.Point(572, 878)
        Me.Label5.Name = "Label5"
        Me.Label5.Size = New System.Drawing.Size(424, 16)
        Me.Label5.TabIndex = 41
        Me.Label5.Text = "※[DB保存]の「優先」のみを指定することは出来ません。"
        '
        'Label7
        '
        Me.Label7.AutoSize = True
        Me.Label7.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(128, Byte))
        Me.Label7.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label7.Location = New System.Drawing.Point(12, 896)
        Me.Label7.Name = "Label7"
        Me.Label7.Size = New System.Drawing.Size(712, 16)
        Me.Label7.TabIndex = 42
        Me.Label7.Text = "※[DB保存]の「一般」と「優先」を同一グレードで指定する場合、全て「優先」扱いとなります。"
        '
        'frmPCodeOutput
        '
        Me.ClientSize = New System.Drawing.Size(1272, 997)
        Me.Name = "frmPCodeOutput"
        Me.Text = "検査パターン別出力設定画面"
        Me.pnlItem.ResumeLayout(False)
        Me.grpBase.ResumeLayout(False)
        Me.grpBase.PerformLayout()
        CType(Me.dgvData, System.ComponentModel.ISupportInitialize).EndInit()
        CType(Me.dgvEtc, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents dgvData As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents cmbPCode As System.Windows.Forms.ComboBox
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents lblPCode As System.Windows.Forms.Label
    Friend WithEvents dgvEtc As tClassLibrary.tCnt.tCnt_DataGridView
    Friend WithEvents Label4 As System.Windows.Forms.Label
	Friend WithEvents Label6 As System.Windows.Forms.Label
	Friend WithEvents Label3 As System.Windows.Forms.Label
	Friend WithEvents Label7 As System.Windows.Forms.Label
	Friend WithEvents Label5 As System.Windows.Forms.Label

End Class
